
#ifndef GENERIC_ASSIGN_HH
#define GENERIC_ASSIGN_HH

#include <xss/xss_context.h>
#include <xss/xss_compiler.h>

namespace xkp {
  
  class dsl_generic_assign : public xss_dsl
    {
      public:
        virtual str render(dsl& info, XSSContext ctx);
      protected:
        virtual XSSModule get_idiom(XSSCompiler compiler, XSSContext ctx) = 0;
        virtual str       get_event()                                     = 0;
        virtual void      add_parameters(param_list& pl, XSSContext ctx)  {}
    };
  
}

#endif