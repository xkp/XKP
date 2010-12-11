
#ifndef XSS_GENERATOR
#define XSS_GENERATOR

#include "xss_parser.h"
#include "xss_context.h"

#include <xs.h>
#include <map>

namespace xkp
{
  class xss_generator : public xss_visitor,
												public boost::enable_shared_from_this<xss_generator>
  {
    public:
      xss_generator(XSSContext context);
    public:
      str get();
      void append(const str& s);
    public:
      virtual void visit(const str& tag, const str& text, param_list* args);
    private:
      str        result_;
      XSSContext context_;
      
      //handlers, i do this for fun 
      typedef bool (xss_generator::* tag_handler)(const str& text, param_list* args);
      typedef std::map<str, tag_handler>  handler_map;
      typedef std::pair<str, tag_handler> handler_pair;
      
      handler_map handlers_;
      
      //actual handlers
      bool handle_text(const str& text, param_list* args);
      bool handle_code(const str& text, param_list* args);
      bool handle_expression(const str& text, param_list* args);
      bool handle_class(const str& text, param_list* args);
      bool handle_file(const str& text, param_list* args);
  };
  
  typedef reference<xss_generator> XSSGenerator;
}

#endif