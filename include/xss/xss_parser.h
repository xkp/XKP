
#ifndef XSS_PARSER_HH
#define XSS_PARSER_HH

#include <base.h>
#include "xss_compiler.h"

namespace xkp
{
  namespace detail
  {
    struct tag_info
      {
        tag_info() : single(false), closed(false) {}
      
        str         name;
        bool        single;
        bool        closed;
        param_list  args;
      };  
  }
  
  class xss_parser
  {
    public:
      void register_tag(const str& tag); 
      bool parse(const str& text, xss_visitor* visitor);
    private:
      std::vector<str> tags_;
      bool read_tag(const str& text, size_t pos, detail::tag_info& tag, size_t& end_tag);
      bool skip_spaces(const str& text, size_t& pos);
      bool read_attributes(const str& text, size_t& pos, detail::tag_info& tag);
      bool registered_tag(const str& tagname);
  };
}

#endif