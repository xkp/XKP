
#ifndef DSL_HTML5_ASYNCH_HH
#define DSL_HTML5_ASYNCH_HH

#include "vm_base.h"

namespace xkp {
  
  struct asynch_param_info
    {
      str     id;
      str     value;
      XSSType type;
    };

  typedef std::vector<asynch_param_info> asynch_params;

  class dsl_h5_asynch : public xss_dsl
    {
      public:
        virtual str render(dsl& info, XSSContext ctx);
      private:
        str decode_param(asynch_param_info& param, XSSCompiler compiler, XSSContext ctx);
    };
}

#endif