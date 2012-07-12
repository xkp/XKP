
#ifndef DSL_AJAX_HH
#define DSL_AJAX_HH

#include "xss/xss_context.h"

namespace xkp {
  
  class dsl_ajax : public xss_dsl
    {
      public:
        virtual str render(dsl& info, XSSContext ctx);
    };
}

#endif