
#ifndef DSL_OUT_HH
#define DSL_OUT_HH

#include "xss_generator.h"
#include <xs/linker.h>

namespace xkp {
  
  class out_linker : public dsl_linker
    {
      public:
        out_linker(xss_generator& gen) : gen_(gen) {}
      public:
        virtual void link(dsl& info, code_linker& owner);
      private:
        xss_generator& gen_;
    };
  
}

#endif