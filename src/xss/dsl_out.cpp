
#include "xss/dsl_out.h"
#include "xss/utils.h"
#include "xs/ast.h"

using namespace xkp;

struct part
  {
    int dyn_idx;
    str text;

    part(): dyn_idx(-1)                          {}
    part(const str& txt): dyn_idx(-1), text(txt) {}
    part(int didx): dyn_idx(didx)                {}
  };

typedef std::vector<part> part_list;

struct xss_gather : xss_visitor
  {
    public:
      xss_gather(part_list& result, std::vector<str>& expressions) : result_(result), expressions_(expressions) {}
    public:
      virtual void visit(const str& tag, const str& text)
        {
          if (tag == "text")
            result_.push_back(part(text));
          else  if (tag == "xss:quote")
            {
              result_.push_back(part('"' + text + '"'));
            }
          else if (tag == "xss:e")
            {
              result_.push_back(part(result_.size()));
              expressions_.push_back(text);
            }
          else if (tag == "xss:open_brace")
            {
              result_.push_back(part("{"));
            }
          else if (tag == "xss:close_brace")
            {
              result_.push_back(part("}"));
            }
          else if (tag == "xss:code")
            {
              //td: !!!
              result_.push_back(part("code can not be embbeded in out"));
            }
          else
            {
              assert(false); //td: error, unknown tag
            }
        }
    private:
      part_list&        result_;
      std::vector<str>& expressions_;
  };

struct worker
  {
    public:
      worker() : gen_(0), indent_(0), tab_(4) {}
      worker(xss_generator* gen, part_list parts, int indent, int tab):
        gen_(gen),
        parts_(parts),
        indent_(indent),
        tab_(tab)
        {
        }
    public:
      void generate(const param_list params)
        {
          part_list::iterator it = parts_.begin();
          part_list::iterator nd = parts_.end();

          size_t param = params.size() - 1;
          str    result;
          for(; it != nd; it++)
            {
              if (it->dyn_idx < 0)
                result += it->text;
              else
                {
                  str expr_value = params.get(param--);
                  if (is_multi_line(expr_value))
                    {
                      //we'll try to keep the original indentation
                      str padding = last_padding(result);

                      //so we'll add the original padding to every line
                      std::vector<str> lines;
                      split_lines(expr_value, lines);

                      std::vector<str>::iterator lit = lines.begin();
                      std::vector<str>::iterator lnd = lines.end();
                      bool first = true;
                      for(; lit != lnd; lit++)
                        {
                          if (first)
                            first = false;
                          else
                            result += padding;

                          result += *lit + '\n';
                        }
                    }
                  else
                    result += expr_value;
                }
            }

          if (indent_ >= 0)
            {
              result = apply_indent(result, indent_);
            }
          gen_->append(result);
        }

      str apply_indent(const str& s, int indent)
        {
          str result;

          std::vector<str> lines;
          split_lines(s, lines);

          std::vector<str>::iterator it = lines.begin();
          std::vector<str>::iterator nd = lines.end();

          //find out the minimum start position per line
          //it will assume it is properly formatted
          //from there on
          size_t _min  = str::npos;

          for(; it != nd; it++)
            {
              size_t _curr = 0;
              bool   _counting = true;
              str    line = *it;
              for(size_t i = 0; i < line.length(); i++)
                {
                  char ch = line[i];
                  switch(ch)
                    {
                      case ' ':
                        {
                          if (_counting)
                            _curr++;
                          break;
                        }
                      case '\t':
                        {
                          if (_counting)
                            _curr += tab_;
                          break;
                        }
                      default:
                        {
                          _counting = false;
                        }
                    }
                }

              if (_curr < _min)
                _min = _curr;
            }

          //now delete that amont
          it = lines.begin();
          for(; it != nd; it++)
            {
              //find out the minimum start position per line
              //it will assume it is properly formatted
              //from there on
              size_t _curr = 0;
              size_t to_erase = 0;
              str    curr_line = *it;
              while(to_erase < curr_line.size() && _curr < _min)
                {
                  char ch = curr_line[to_erase];
                  if (ch == ' ')
                    _curr++;
                  else if (ch == '\t')
                    _curr += 4;
                  else
                    break;

                  to_erase++;
                }

              curr_line.erase(0, to_erase);
              if (!curr_line.empty())
                {
                  for(int i = 0; i < indent_*tab_; i++)
                    curr_line.insert(0, " ");
                }

              result += curr_line + '\n';
            }

          return result;
        }

    private:
      xss_generator* gen_; //td: make it a references, to be politically correct
      int            indent_;
      int            tab_;
      part_list      parts_;

      std::vector<str> load_lines(const str& s)
        {
          std::vector<str> result;
          size_t curr = 0;
          for(size_t i = 0; i < s.length(); i++)
            {
              if (s[i] == '\n')
                {
                  result.push_back( s.substr(curr, i) );
                  curr = i;
                }
            }

          return result;
        }
  };

typedef reference<worker> Worker;

struct worker_schema : object_schema<worker>
  {
    worker_schema()
      {
        dynamic_method_( "generate", &worker::generate );
      }
  };

namespace xkp
{
    register_complete_type(worker, worker_schema);
}

//out_linker
void out_linker::link(dsl& info, code_linker& owner)
  {
    int indent   = -1;
    int tab_size = 4;

    //process parameters
    variant indent_value = info.params.get("indent");
    if (!indent_value.empty())
      {
        expression expr = indent_value;
        indent = owner.evaluate_expression(expr);
      }

    //td: utilify
    variant tab_value = info.params.get("tab_size");
    if (!tab_value.empty())
      {
        expression expr = tab_value;
        tab_size = owner.evaluate_expression(expr);
      }

    std::vector<str> expressions;
    part_list parts;
    xss_gather gather(parts, expressions);

    //process xss
    xss_parser parser;
    parser.register_tag("xss:e");
    parser.register_tag("xss:code");
    parser.register_tag("xss:open_brace");
    parser.register_tag("xss:close_brace");

    parser.parse(info.text, &gather);

    //now xs
    xs_utils xs;

    //create a safe reference to be inserted in the execution context later on
    Worker wrk(new worker(&gen_, parts, indent, tab_size));
    owner.add_instruction(i_load_constant, owner.add_constant(wrk));

    //push the expression to be used as parameters
    std::vector<str>::iterator it = expressions.begin();
    std::vector<str>::iterator nd = expressions.end();
    for(; it != nd; it++)
      {
        expression expr;
        if (!xs.compile_expression(*it, expr))
          assert(false);

        owner.link_expression(expr);
      }

    //for this call
    owner.add_call(wrk, "generate", expressions.size(), false);
  }

