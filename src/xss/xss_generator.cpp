
#include "xss/xss_generator.h"
#include "xss/dsl_out.h"
#include "xss/utils.h"

#include <xs.h>

using namespace xkp;

//xss_generator
xss_generator::xss_generator(code_context& context) : context_(context)
  {
    handlers_.insert(handler_pair("text",     &xss_generator::handle_text));
    handlers_.insert(handler_pair("xss:code", &xss_generator::handle_code));
    handlers_.insert(handler_pair("xss:e",    &xss_generator::handle_expression));
  }
  
str xss_generator::get()
  {
    return result_;
  }
  
void xss_generator::append(const str& s)
  {
    result_ += s;
  }
  
void xss_generator::visit(const str& tag, const str& text, param_list* args)
  {
    handler_map::iterator it = handlers_.find(tag);  
    if (it != handlers_.end())
      {
        (this->*(it->second))(text, args);
      }
    else
      {
        //td: error handling
      }
  }
  
bool xss_generator::handle_text(const str& text, param_list* args)
  {
    result_ += text;
    return true;
  }

bool xss_generator::handle_code(const str& text, param_list* args)
  {
    //details, details... 
    trim_last_empty_line(result_);
    
    xs_utils xs;
    xs.execute_xs(text, context_);
    return true;
  }

bool xss_generator::handle_expression(const str& text, param_list* args)
  {
    xs_utils xs;
    
    str expr = text;
    if (text.empty() && args)
      {
        variant vv = args->get("value");
        if (!vv.empty())
          {
            expr = variant_cast<str>(vv, "");
          }
      }
     
    str result = variant_cast<str>(xs.evaluate_xs_expression(expr, context_), str("Error"));
    result_   += result;
    return true;
  }
