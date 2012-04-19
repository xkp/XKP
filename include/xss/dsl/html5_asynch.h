
#ifndef DSL_HTML5_ASYNCH_HH
#define DSL_HTML5_ASYNCH_HH

#include "vm_base.h"

namespace xkp {
  
  class dsl_h5_asynch : public xss_dsl
    {
      public:
        virtual str render(dsl& info, XSSContext ctx);
    };
}

#endif