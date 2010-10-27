
#include <text/text_renderer.h>

#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/confix.hpp>
#include <boost/spirit/utility/escape_char.hpp>
#include <boost/spirit/utility/chset.hpp>
#include <boost/spirit/utility/functor_parser.hpp>
#include <boost/spirit/utility/distinct.hpp>
#include <boost/spirit/symbols/symbols.hpp>
#include <boost/spirit/iterator.hpp> 
#include <boost/spirit/utility/grammar_def.hpp>

using namespace boost::spirit;
using namespace xkp;

//at parse time the text will be split in rederable pieces
struct piece
  {
    str  text;
    bool code;
  };
  
typedef std::vector<piece> piece_list;

//this will be achieved using spirit, so
//here is a little context
struct parse_context
  {
    piece_list results;
    
    void add_text(str text)
      {
        for(size_t i = 0; i < text.size(); i++)
          {
            if (text[i] == 13)
              text[i] = '\n';
          }

        piece p;
        p.code = false;
        p.text = text;
        results.push_back( p );
      }
      
    void add_code(str text)
      {
        piece p;
        p.code = true;
        p.text = text;
        results.push_back( p );
      }
  };

//spirit types, tied to a scanner
typedef char                             char_t;
typedef position_iterator<str::iterator> iterator_t;
typedef scanner<iterator_t>              scanner_t;
typedef rule<scanner_t>                  rule_t;

//semantic actions connect the grammar to the context, a little bit bulky
//but easy to write nonetheless.
struct tr_action
  {
    parse_context& ctx_;
    
    tr_action(parse_context& ctx) : ctx_(ctx) {}
  };

struct add_text : public tr_action
  {
    add_text( parse_context& ctx ) : tr_action(ctx) {}
    
    void operator()( iterator_t begin, iterator_t end ) const 
      { 
        str val(begin, end);
        ctx_.add_text( val ); 
      }
  };

struct add_code : public tr_action
  {
    add_code( parse_context& ctx ) : tr_action(ctx) {}
    
    void operator()( iterator_t begin, iterator_t end ) const 
      { 
        str val(begin, end);
        ctx_.add_code( val ); 
      }
  };

struct end_parse
  {
    void operator()( iterator_t begin, iterator_t end ) const 
      { 
        throw 1234; 
      }
  };
  
struct fail_functor
  {
    typedef int result_t;
    template <typename ScannerT>
    int operator()(ScannerT const& scan, result_t& result) const
      {
        return -1;
      }
  };

typedef functor_parser<fail_functor> fail_p;


//finally, the grammar
struct tr_grammar : grammar<tr_grammar>
  {
    parse_context& ctx;
  
    tr_grammar(parse_context& _ctx) : ctx(_ctx) {}

    template <typename ScannerT>
    struct definition
    {
        definition(tr_grammar const& self)
        {
          text_r   = (+(anychar_p - ("<%" | end_p)))  [add_text(self.ctx)];

          code_r   =  str_p("<%")                              
                   >> (*(anychar_p - ("%>")))         [add_code(self.ctx)]
                   >> str_p("%>");
                      
          
          main_r   =  *(  code_r 
                       |  text_r
                       |  epsilon_p                   [end_parse()] 
                       ); 
          
        }

      rule<ScannerT> text_r;
      rule<ScannerT> code_r;
      rule<ScannerT> main_r;
      
      rule<ScannerT> const& start() const { return main_r; }
    };
  };


//text_renderer
bool text_renderer::compile(str src, code_context& ctx)
  {
    //spirit iterators
    iterator_t begin(src.begin(), src.end());
    iterator_t end;
    begin.set_tabchars(4);

    parse_context parse_ctx;
    tr_grammar grammar( parse_ctx );
    parse_info<iterator_t> pi;
    bool succeeded = true;
    try
      {
        pi = boost::spirit::parse(begin, end, *grammar, fail_p());
      }
    catch(int e)
      {
        assert(e == 1234);
      }
      
    piece_list::iterator it = parse_ctx.results.begin();    
    piece_list::iterator nd = parse_ctx.results.end();    
    
    str current_text;
    for(; it != nd; it++)
      {
        if (it->code)
          {
            xs_utils    util;
            render_info ri;
            ri.text = current_text;
            ri.code = util.compile_code( it->text, ctx );
            
            render_.push_back( ri );
            
            current_text = "";
          }
        else
          {
            assert( current_text.empty() );
            current_text = it->text;
          }
      }
      
    if (!current_text.empty())
      {
        render_info ri;
        ri.text = current_text;
        ri.code = ByteCode();
        
        render_.push_back( ri );
      }
      
    return true;
  }
  
str text_renderer::render(code_context& ctx)
  {
    render_list::iterator it = render_.begin();
    render_list::iterator nd = render_.end();
    
    str result;
    for(; it != nd; it++)
      {
        result += it->text;
        if (it->code)
          {
            execution_context e(it->code, ctx.this_, ctx.args_);
            str res = e.execute();
            
            result += res;
          }
      }
      
    return result;      
  }
