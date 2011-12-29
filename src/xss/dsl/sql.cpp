
#include <xss/dsl/sql.h>
#include <xss/xss_error.h>

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

using namespace xkp;
using namespace boost::spirit::classic;

const str SLanguage("language");

const str SCannotParseSQL("Cannot parse sql text, rtfm");

//an extremely simple html parser, at this point the htmls to be used are expected to
//be correct, so we'll keep an intermediate representation of the tags
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
        ctx_.add(assign_tag(ctx_.variable, str(begin, end))); 
      }
  };

struct no_variable : public __action
  {
    no_variable( assign_context& ctx ) : __action(ctx) {}
    
    void operator()( iterator_t begin, iterator_t end ) const 
      { 
        ctx_.add(assign_tag(str(), str(begin, end))); 
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
          text_r    = *(anychar_p - (';' | end_p) );
          assign_r  =  *( (   ident_r       [variable(self.ctx)]
                          >> "=" >> text_r  [inner(self.ctx)]
                          >> (';' 
                              | end_p       [end_parse(self.ctx)]
                             ) 
                          )
                          |
                          ( text_r          [no_variable(self.ctx)])
                       )
                   >>  end_p; 
          
        }

      rule<ScannerT> ident_r;
      rule<ScannerT> text_r;
      rule<ScannerT> assign_r;
      
      rule<ScannerT> const& start() const { return assign_r; }
    };
  };

//dsl_sql
str render_statement(assign_list& tags, const str& callback, int idx)
  {
    if (idx >= tags.size())
      return str();

    std::ostringstream result;
    assign_tag         tag = tags[idx];
        
    //render header
    bool first = idx == 0;
    bool last  = idx == tags.size() - 1;
    if (first)
      {
        result << "new mysql.Database({\n hostname: 'localhost', \nuser: 'user', \npassword: 'password', \ndatabase: 'test'})\n";
        result << ".connect(function(error) {\n";
      }

    //execute query
    str query = tag.text;
    boost::replace_all (query, "\n", "\\n");
    boost::replace_all (query, "\"", "\\\"");

    result << "this.query(\"" + query + "\")\n";
    result << ".execute(function(error, rows, cols) {\n";
    
    //render the actions
    result << tag.variable << " = rows;\n";
    if (last)
      result << callback << ";\n";
    else 
      result << render_statement(tags, callback, idx + 1);

    result << "});"; //close execute

    if (first)
      result << "});"; //close connect
    return result.str();
  }

str dsl_sql::render(dsl& info, XSSContext ctx)
  {
    //do the parsing
    assign_context actx;
    
    assign_grammar grammar(actx);
    parse_info<iterator_t> pi;
    bool succeeded = true;
    try
      {
        pi = boost::spirit::classic::parse(info.text.begin(), info.text.end(), *grammar, space_p);
      }
    catch(int e)
      {
        succeeded = e == 1234;
      }
      
    if (!succeeded)
      {
        param_list error;
        error.add("id", SLanguage);
        error.add("desc", SCannotParseSQL);
        error.add("text", info.text);
        xss_throw(error);
      }

    str callback = variant_cast<str>(ctx->resolve("#wax_callback"), str());
    return render_statement(actx.tags, callback, 0);
  }
