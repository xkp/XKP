
#ifndef XSS_DSL_SHELL
#define XSS_DSL_SHELL

#include <xss/xss_context.h>
#include "generic_assign.h"

namespace xkp {
  
  class dsl_shell : public dsl_generic_assign
    {
      public:
        dsl_shell()
          {
            add_attribute("asynch", true);
          }
      protected:
        virtual XSSModule get_idiom(XSSCompiler compiler, XSSContext ctx);
        virtual str       get_event();
        virtual void      add_parameters(param_list& pl, XSSContext ctx);
    };
  
}

#endif