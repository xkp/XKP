
#ifndef DSL_OUT_HH
#define DSL_OUT_HH

#include <xss/xss_compiler.h>
#include <xs/linker.h>

namespace xkp {
  
  class out_linker : public dsl_linker
    {
      public:
        out_linker(XSSCompiler compiler) : compiler_(compiler) {}
      public:
        virtual void link(dsl& info, code_linker& owner);
      private:
        XSSCompiler compiler_;

        void read_params(param_list& params);
    };
  
}

#endif