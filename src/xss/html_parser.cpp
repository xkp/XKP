
#include <xss/html_parser.h>

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

using namespace xkp;
using namespace boost::spirit::classic;

//tag_list
tag_list::iterator tag_list::begin()  
  {
    return tags_.begin();
  }

tag_list::iterator tag_list::end()
  {
    return tags_.end();
  }

void tag_list::push_back(const tag t)
  {
    tags_.push_back(t);
  }

int tag_list::find(const str& id, bool include_tag_name)
  {
    std::vector<tag>::iterator it = tags_.begin();
    std::vector<tag>::iterator nd = tags_.end();

    int tag_index = 0;
    for(; it != nd; it++, tag_index++)
      {
        if (include_tag_name && it->tag_name == id)
          return tag_index;

        prop_list::iterator pit = it->props.begin();
        prop_list::iterator pnd = it->props.end();
        for(; pit != pnd; pit++)
          {
            if (pit->first == "id" && pit->second == id)
              return tag_index;
          }
      }

    return -1;
  }

tag& tag_list::get(int idx)
  {
    return tags_[idx];
  }

size_t tag_list::size()
  {
    return tags_.size();
  }

void tag_list::inner_tags(int& tag_idx, tag_list& result)
  {
    tag& match_tag = tags_[tag_idx];
    if (match_tag.closes)
      return;

    int  matches = 1;

    while (tag_idx < tags_.size() && matches > 0)
      {
        tag_idx++;
        tag& curr_tag = tags_[tag_idx];

        if (curr_tag.tag_name == match_tag.tag_name)
          {
            if (curr_tag.closes)
              matches--;
            else 
              matches++;
          }

        if (matches > 0)
          result.push_back(curr_tag);
      }
  }

int tag_list::find_closing(int tag_idx)
  {
    tag& match_tag = tags_[tag_idx];
    if (match_tag.closes)
      return tag_idx;

    int  matches = 1;

    while (tag_idx < tags_.size() && matches > 0)
      {
        tag_idx++;
        tag& curr_tag = tags_[tag_idx];

        if (curr_tag.tag_name == match_tag.tag_name)
          {
            if (curr_tag.closes)
              matches--;
            else 
              matches++;
          }
      }

    return tag_idx;
  }

//an extremely simple html parser, at this point the htmls to be used are expected to
//be correct, so we'll keep an intermediate representation of the tags

struct html_context
  {
    tag_list    tags_;
    tag         current_; 
    str         propname_; 
    iterator_t  begin;
    
    void start_tag(size_t offset)
      {
        current_.clear();
        current_.start = offset;
      }
      
    void end_tag(size_t offset)
      {
        current_.close = offset;
        tags_.push_back( current_ );
      }
      
    void tag_name(const str& name)        { current_.tag_name = name; }
    void tag_closes()                     { current_.closes = true;   }
    void tag_complete()                   { current_.complete = true; }
    void property_name(const str& name)   { propname_ = name; }
    void property_value(const str& value) 
      { 
        current_.add_prop( propname_, value.substr(1, value.size() - 2) ); 
      }
  };

//semantic actions connect the grammar to the context, a little bit bulky
//but easy to write nonetheless.

struct html_action
  {
    html_context& ctx_;
    
    html_action(html_context& ctx) : ctx_(ctx) {}
  };

struct start_tag : public html_action
  {
    start_tag( html_context& ctx ) : html_action(ctx) {}
    
    void operator()( iterator_t begin, iterator_t end ) const 
      { 
        ctx_.start_tag( begin - ctx_.begin ); 
      }
  };

struct end_tag : public html_action
  {
    end_tag( html_context& ctx ) : html_action(ctx) {}
    
    void operator()( iterator_t begin, iterator_t end ) const 
      { 
        ctx_.end_tag( begin - ctx_.begin ); 
      }
  };

struct tag_name : public html_action
  {
    tag_name( html_context& ctx ) : html_action(ctx) {}
    
    void operator()( iterator_t begin, iterator_t end ) const 
      { 
        str name = str(begin, end);
        ctx_.tag_name( name ); 
      }
  };

struct tag_closes : public html_action
  {
    tag_closes( html_context& ctx ) : html_action(ctx) {}
    
    void operator()( char v ) const 
      { 
        ctx_.tag_closes(); 
      }
  };

struct tag_complete : public html_action
  {
    tag_complete( html_context& ctx ) : html_action(ctx) {}
    
    void operator()( char v ) const 
      { 
        ctx_.tag_complete(); 
      }
  };

struct property_name : public html_action
  {
    property_name( html_context& ctx ) : html_action(ctx) {}
    
    void operator()( iterator_t begin, iterator_t end ) const { ctx_.property_name(str(begin, end)); }
  };

struct property_value : public html_action
  {
    property_value( html_context& ctx ) : html_action(ctx) {}
    
    void operator()( iterator_t begin, iterator_t end ) const 
      { 
        ctx_.property_value(str(begin, end)); 
      }
  };
  
struct end_parse : public html_action
  {
    end_parse( html_context& ctx ) : html_action(ctx) {}
    void operator()( iterator_t begin, iterator_t end ) const 
      { 
        throw 1234; 
      }
  };

//finally, the grammar
struct html_grammar : grammar<html_grammar>
  {
    html_context& ctx;
  
    html_grammar(html_context& _ctx) : ctx(_ctx) {}

    template <typename ScannerT>
    struct definition
    {
        definition(html_grammar const& self)
        {
          string_r = confix_p( '"', (*c_escape_ch_p), '"' )
                   | confix_p( "'", (*c_escape_ch_p), "'" );

          ident_r  = lexeme_d[ +chset_p(":A-Za-z0-9_-") ];

          tag_r    =  str_p("<")              [start_tag(self.ctx)] 
                   >> ( (  ch_p('/')          [tag_closes(self.ctx)] 
                        >> ident_r            [tag_name(self.ctx)]
                        >> str_p(">")         [end_tag(self.ctx)]
                        )
                      | (   ident_r           [tag_name(self.ctx)]
                        >> *(   ident_r       [property_name(self.ctx)]
                            >>  '=' 
                            >>  string_r      [property_value(self.ctx)]
                            )
                        >>  (   !(ch_p('/')   [tag_complete(self.ctx)] )
                            >>  str_p(">")    [end_tag(self.ctx)] 
                            )
                        )
                      );
                      
          comment_r =   str_p("<") >> "!"
                    >>  *(anychar_p - '>')
                    >>  '>';
                      
          text_r   = *(anychar_p - ('<' | end_p) );
          
          html_r   =  *(  text_r
                       >> ( tag_r
                          | comment_r
                          | end_p             [end_parse(self.ctx)]
                          )                                     
                       )
                   >>  end_p; 
          
        }

      rule<ScannerT> string_r;
      rule<ScannerT> ident_r;
      rule<ScannerT> tag_r;
      rule<ScannerT> text_r;
      rule<ScannerT> comment_r;
      rule<ScannerT> html_r;
      
      rule<ScannerT> const& start() const { return html_r; }
    };
  };

//HTMLRenderer
bool html_parser::parse(str& text, tag_list& result)
  {
    //do the parsing
    html_context ctx;
    ctx.begin = text.begin();
    
    html_grammar grammar( ctx );
    parse_info<iterator_t> pi;
    bool succeeded = true;
    try
      {
        pi = boost::spirit::classic::parse(text.begin(), text.end(), *grammar, space_p);
      }
    catch(int e)
      {
        assert(e == 1234);
      }
      
    result = ctx.tags_;
    return true;
  }
