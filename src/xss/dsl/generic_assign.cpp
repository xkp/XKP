
#include <xss/dsl/generic_assign.h>
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

str dsl_generic_assign::render(dsl& info, XSSContext ctx)
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
        //td: line/column
        param_list error;
        error.add("id", SLanguage);
        error.add("desc", SCannotParseSQL);
        error.add("text", info.text);
        xss_throw(error);
      }

    //we need to capture the output to return it 
    XSSCompiler compiler = ctx->resolve("compiler");
    XSSRenderer renderer(new xss_renderer(compiler, ctx, ctx->path()));
    
    //all rendering will be done by the appropiate idiom, selected by subclasses
    XSSModule idiom = get_idiom(compiler, ctx);
    if (!idiom)
      {
        //td: error
        assert(false);        
      }

    //convert the tags into xs objects
    DynamicArray tags(new dynamic_array);
    
    assign_list::iterator it = actx.tags.begin();
    assign_list::iterator nd = actx.tags.end();
    for(; it != nd; it++)
      {
        XSSObject tag(new xss_object);
        tag->add_attribute("text", it->text);

        tags->push_back(tag);
      }

    //do the rendering by invoking a subclass selected event
    compiler->push_renderer(renderer);

    //let subclasses add parameters
    param_list pl;
    pl.add("target", tags);

    add_parameters(pl, ctx);

    size_t evid = idiom->event_id(get_event());
    if (evid > 0)
      idiom->dispatch_event(evid, pl);
    else
      {
        //td: error
      }

    compiler->pop_renderer();
    return renderer->get();
  }
