
#include <xss/dsl_out.h>
#include <xss/utils.h>
#include <xss/utils.h>
#include <xss/xss_error.h>
#include <xs/ast.h>

#include <boost/algorithm/string.hpp>

using namespace xkp;

//strings
const str SInvalidTag("xss-tags");
const str STypeMismatch("type-mismatch");

const str SUnknownXSSTag("Using an unknown tag");
const str SExpectingExpression("Expecting expression");

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
      virtual void visit(const str& tag, const str& text, param_list* args)
        {
          if (tag == "text")
            result_.push_back(part(text));
          else  if (tag == "xss:quote")
            {
              result_.push_back(part('"' + text + '"'));
            }
          else if (tag == "xss:e")
            {
              str expr_text = text;
              if (expr_text.empty() && args)
                expr_text = variant_cast<str>(args->get("value"), "");

              if (!expr_text.empty())
                {
                  result_.push_back(part(result_.size()));
                  expressions_.push_back(expr_text);
                }
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
              param_list error;
              error.add("id", SInvalidTag);
              error.add("desc", SUnknownXSSTag);
              error.add("tag", tag);
              xss_throw(error);
            }
        }
    private:
      part_list&        result_;
      std::vector<str>& expressions_;
  };

struct worker
  {
    public:
      worker() : gen_(0), tab_(4) {}
      worker(xss_generator* gen, part_list parts, int tab, bool dont_break):
        gen_(gen),
        parts_(parts),
        indent_(-1),
        tab_(tab),
				dont_break_(dont_break)
        {
        }
    public:
      void generate(const param_list params)
        {
          part_list::iterator it = parts_.begin();
          part_list::iterator nd = parts_.end();

          indent_ = params.get(params.size() - 1);
          
          size_t param = params.size() - 2;
          str    result;
          for(; it != nd; it++)
            {
              if (it->dyn_idx < 0)
                result += it->text;
              else
                {
                  variant vv = params.get(param--);
                  if (vv.empty())
                    vv = str("null"); //td: this should be an error?
                
                  str expr_value = vv;
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

                          result += *lit;
													
													if (!dont_break_)
														result += '\n';
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

              result += curr_line;
							
							if(!dont_break_)
								result += '\n';
            }

          return result;
        }

    private:
      xss_generator* gen_; //td: make it a references, to be politically correct
      int            indent_;
      int            tab_;
      part_list      parts_;
			bool					 dont_break_;

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
    virtual void declare()
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
    int tab_size = 4;
    bool dont_break = false;
    bool do_break = false;
		bool trim = false;

    std::vector<str> expressions;

    //process parameters
    variant    indent_value = info.params.get("indent");
    expression indent_expr;
    if (!indent_value.empty())
      {
        try
          {
            indent_expr = indent_value; 
          }
        catch(type_mismatch tm)
          {
            param_list error;
            error.add("id", STypeMismatch);
            error.add("desc", SExpectingExpression);
            xss_throw(error);
          }
      }
    else
      {
        //we'll always pass the value of indent dynamically (resolved at runtime)
        //in this case the user did not especify an indent param, so we fake an expression
        int default_indent   = -1;
        indent_expr.push_operand(default_indent);
      }

    //td: utilify
    variant tab_value = info.params.get("tab_size");
    if (!tab_value.empty())
      {
        expression expr = tab_value;
        tab_size = owner.evaluate_expression(expr); //td: only contants, lazy me
      }

    variant dont_break_v = info.params.get("dont_break");
    if (!dont_break_v.empty())
      {
        expression expr = dont_break_v;
        dont_break = owner.evaluate_expression(expr); //td: only contants, lazy me
      }

    variant do_break_v = info.params.get("do_break");
    if (!do_break_v.empty())
      {
        expression expr = do_break_v;
        do_break = owner.evaluate_expression(expr); //td: only contants, lazy me
      }

		variant trim_v = info.params.get("trim");
    if (!trim_v.empty())
      {
        expression expr = trim_v;
        trim = owner.evaluate_expression(expr); //td: only contants, lazy me
      }
		
    //process xss
		part_list parts;
		xss_gather gather(parts, expressions);

    xss_parser parser;
    parser.register_tag("xss:e");
    parser.register_tag("xss:code");
    parser.register_tag("xss:open_brace");
    parser.register_tag("xss:close_brace");
		
		str to_parse = info.text;
		if (trim)
			boost::trim(to_parse);

		if (do_break)
				to_parse += '\n';

		parser.parse(to_parse, &gather);

    //now xs
    xs_utils xs;

    //create a safe reference to be inserted in the execution context later on
		Worker wrk(new worker(&gen_, parts, tab_size, dont_break));
    owner.add_instruction(i_load_constant, owner.add_constant(wrk));

    //and so we link the indent, after having the worker on
    owner.link_expression(indent_expr);

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

    //for this call, remember the indent is always the forat parameter
    owner.add_call(wrk, "generate", expressions.size() + 1, false);
  }

