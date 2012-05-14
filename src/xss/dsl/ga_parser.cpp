
#include <xss/dsl/ga_parser.h>
#include <xss/xss_error.h>
#include <xss/xss_renderer.h>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_confix.hpp>
#include <boost/spirit/include/classic_escape_char.hpp>
#include <boost/spirit/include/classic_chset.hpp>
#include <boost/spirit/include/classic_functor_parser.hpp>
#include <boost/spirit/include/classic_distinct.hpp>
#include <boost/spirit/include/classic_symbols.hpp>
#include <boost/spirit/include/classic_iterator.hpp> 
#include <boost/spirit/include/classic_file_iterator.hpp> 
#include <boost/spirit/include/classic_grammar_def.hpp>
#include <boost/spirit/include/classic_ref_actor.hpp>    
#include <boost/spirit/include/classic_ref_value_actor.hpp>
#include <boost/spirit/include/classic_assign_actor.hpp>    
#include <boost/spirit/include/classic_clear_actor.hpp>
#include <boost/spirit/include/classic_decrement_actor.hpp>
#include <boost/spirit/include/classic_erase_actor.hpp>     
#include <boost/spirit/include/classic_increment_actor.hpp>    
#include <boost/spirit/include/classic_insert_key_actor.hpp>
#include <boost/spirit/include/classic_insert_at_actor.hpp>
#include <boost/spirit/include/classic_push_back_actor.hpp>
#include <boost/spirit/include/classic_push_front_actor.hpp>
#include <boost/spirit/include/classic_actor.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

using namespace xkp;
using namespace boost::spirit::classic;

const str SLanguage("dsl");

const str SCannotParseAssign("Cannot parse a simple assign text, rtfm");

struct assign_tag
  {
    assign_tag(const str& _variable, const str& _text):
      variable(_variable),
      text(_text)
      {
      }

    str variable;
    str text;
  };

typedef str::iterator           iterator_t;
typedef std::vector<assign_tag> assign_list;

struct assign_context
  {
    void add(const assign_tag tag)
      {
        tags.push_back(tag);
      }

    assign_list tags;
    str         variable; 
  };

//semantic actions connect the grammar to the context, a little bit bulky
//but easy to write nonetheless.

struct __action
  {
    assign_context& ctx_;
    
    __action(assign_context& ctx) : ctx_(ctx) {}
  };

struct variable : public __action
  {
    variable( assign_context& ctx ) : __action(ctx) {}
    
    void operator()( iterator_t begin, iterator_t end ) const 
      { 
        ctx_.variable = str(begin, end); 
      }
  };

struct inner : public __action
  {
    inner( assign_context& ctx ) : __action(ctx) {}
    
    void operator()( iterator_t begin, iterator_t end ) const 
      {
        str value(begin, end);
        boost::replace_all (value, "\\;", ";");
        ctx_.add(assign_tag(ctx_.variable, value)); 
      }
  };

struct no_variable : public __action
  {
    no_variable( assign_context& ctx ) : __action(ctx) {}
    
    void operator()( iterator_t begin, iterator_t end ) const 
      { 
        ctx_.variable = "";
        str value(begin, end);
        boost::replace_all (value, "\\;", ";");
        ctx_.add(assign_tag(str(), value)); 
      }
  };

struct end_parse : public __action
  {
    end_parse( assign_context& ctx ) : __action(ctx) {}
    void operator()( iterator_t begin, iterator_t end ) const 
      { 
        throw 1234; 
      }
  };

//finally, the grammar
struct assign_grammar : grammar<assign_grammar>
  {
    assign_context& ctx;
  
    assign_grammar(assign_context& _ctx) : ctx(_ctx) {}

    template <typename ScannerT>
    struct definition
    {
        definition(assign_grammar const& self)
        {
          ident_r   = lexeme_d[ +chset_p("A-Za-z0-9_-") ];
          text_r    = *(  "\\;" | (anychar_p - (';' | end_p) ));
          assign_r  = *( (   ident_r          [variable(self.ctx)]
                          >>  "=" >> text_r   [inner(self.ctx)]
                          >>  (';' 
                              | end_p         [end_parse(self.ctx)]
                              ) 
                          )
                          |
                          (   text_r          [no_variable(self.ctx)]
                          >>  (';' 
                              | end_p         [end_parse(self.ctx)]
                              ) 
                          )
                       )
                   >>  end_p                  [end_parse(self.ctx)]; 
          
        }

      rule<ScannerT> ident_r;
      rule<ScannerT> text_r;
      rule<ScannerT> assign_r;
      
      rule<ScannerT> const& start() const { return assign_r; }
    };
  };

class ga_renderer
  {
    public:
      void parse(const str& text)
        {
        }
      
      str render_as_expr()
        {
          std::ostringstream result;
          bool first = true;
          for(size_t i = 0; i < texts_.size(); i++)
            {
              if (first)
                first = false;
              else
                result << " + ";

              str text  = texts_[i];
              str param = params_[i];

              boost::replace_all (text, "\n", "\\n");
              boost::replace_all (text, "\"", "\\\"");

              if (!param.empty())
                result << param << " + ";

              result << "\"" << text << "\""; 
            }

          return result.str();
        }

      private:
        std::vector<str> texts_;
        std::vector<str> params_;
    };

//ga_parser
bool ga_parser::parse(str text, std::vector<ga_item>& result)
  {
    //do the parsing
    assign_context actx;
    
    assign_grammar grammar(actx);
    parse_info<iterator_t> pi;
    bool succeeded = true;
    try
      {
        pi = boost::spirit::classic::parse(text.begin(), text.end(), *grammar, space_p);
      }
    catch(int e)
      {
        succeeded = e == 1234;
      }
      
    if (!succeeded)
      return false;
    
    assign_list::iterator it = actx.tags.begin();
    assign_list::iterator nd = actx.tags.end();

    for(; it != nd; it++)
      {
        ga_item res;
        res.variable = it->variable;
        
        parse_item(it->text, res);
        result.push_back(res);
      }
    return true;
  }

void ga_parser::parse_item(const str& text, ga_item& result)
  {
    std::vector<str> items;
    boost::split(items, text, boost::is_space());

    for(size_t idx = 0; idx < items.size(); idx++)
      {
        str item = items[idx];

        size_t token_pos = item.find_first_of("@");
        while(token_pos != str::npos)
          {
            shell_param param;
            param.item_idx = idx;
            param.item_spot = token_pos;

            bool found = false;
            for(size_t it = token_pos + 1; it < item.size(); it++)
              {
                if (!boost::is_alpha()(item[it]) && !boost::is_digit()(item[it]))
                  {
                    size_t count = it - token_pos - 1;
                    param.id = str(item, token_pos + 1, count);
                    item.erase(token_pos, count + 1);
                    found = true;
                    break;
                  }
              }

            if (!found)
              {
                param.id = str(item, token_pos + 1);
                item.erase(token_pos);
              }

            result.params.push_back(param);
            
            token_pos = item.find_first_of("@", token_pos);
          }

        result.items.push_back(item);
      }
  }
