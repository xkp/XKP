
#ifndef XSS_DSL_SQL
#define XSS_DSL_SQL

#include <xss/xss_context.h>

namespace xkp {
  
  class dsl_sql : public xss_dsl
    {
      public:
        dsl_sql()
          {
            add_attribute("asynch", true);
          }
      public:
        virtual str render(dsl& info, XSSContext ctx);
    };
  
}

#endif