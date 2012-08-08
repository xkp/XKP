
#ifndef DSL_SMARTY_HH
#define DSL_SMARTY_HH

#include "xss/xss_context.h"

namespace xkp {
  
  class dsl_smarty : public xss_dsl
    {
      public:
        virtual str render(dsl& info, XSSContext ctx);
        virtual void pre_process(dsl& info, XSSContext ctx);
        void pre_process_file(const str& tpl_contents, XSSContext ctx);
        str gen_call(const str& contents, XSSContext ctx, param_list& args);
      private:
        void process_params(param_list& params, XSSContext ctx, str& html_id, str& data_id);
        void add_initializer(XSSContext ctx, const str& text);
    };
}

#endif