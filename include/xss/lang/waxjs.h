
#ifndef WAX_JS_HH
#define WAX_JS_HH

#include <xss/lang/base.h>

namespace xkp{

struct code_split;

typedef reference<code_split> CodeSplit;

struct code_split
  {
    code      target; //td: !!! reference 
    int       split_idx;
    CodeSplit split_code;

    bool      split_on_if;
    bool      split_on_else;
  };

struct waxjs_code_renderer : public base_code_renderer
  {
    waxjs_code_renderer();
    waxjs_code_renderer(const waxjs_code_renderer& other);
    waxjs_code_renderer(code& cde, param_list_decl& params, XSSContext ctx);

    //ICodeRenderer
    virtual str render();
    
    private:
      str render_split(CodeSplit fork);
  };

}

#endif