
#ifndef WAX_JS_HH
#define WAX_JS_HH

#include <xss/lang/base.h>

namespace xkp{

struct waxjs_code_renderer : public base_code_renderer
  {
    waxjs_code_renderer();
    waxjs_code_renderer(const waxjs_code_renderer& other);
    waxjs_code_renderer(code& cde, param_list_decl& params, XSSContext ctx);

    //ICodeRenderer
    virtual str render();
  };

}

#endif