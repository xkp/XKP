
#ifndef DSL_OUT_HH
#define DSL_OUT_HH

#include "xss_generator.h"
#include <xs/linker.h>

namespace xkp {
  
  class out_linker : public dsl_linker
    {
      public:
        out_linker(XSSGenerator gen) : gen_(gen) {}
      public:
        virtual void link(dsl& info, code_linker& owner);
      private:
        XSSGenerator gen_;
    };
  
}

#endif