
#ifndef DSL_NATIVE_HH
#define DSL_NATIVE_HH

#include "vm_base.h"

namespace xkp {
  
  class dsl_native : public xss_dsl
    {
      public:
        virtual str render(dsl& info, XSSContext ctx);
    };
}

#endif