
#ifndef WAX_JS_HH
#define WAX_JS_HH

#include <xss/lang/js.h>

namespace xkp{

struct code_split;

typedef reference<code_split> CodeSplit;

struct code_split
  {
    code_split(code& _target):
      target(_target),
      split_idx(false),
      split_on_if(false),
      split_on_else(false)
      {
      }

    code      target; //td: !!! reference 
    int       split_idx;
    CodeSplit split_code;
    XSSMethod method;

    bool      split_on_if;
    bool      split_on_else;

    std::vector<str> add;
  };

struct waxjs_lang : public js_lang
  {
    virtual variant compile_code(code& cde, param_list_decl& params, XSSContext ctx);
  };

struct waxjs_code_renderer : public js_code_renderer
  {
    waxjs_code_renderer();
    waxjs_code_renderer(const waxjs_code_renderer& other);
    waxjs_code_renderer(code& cde, param_list_decl& params, XSSContext ctx);

    //ICodeRenderer
    virtual str render();

    private:
      XSSCompiler compiler_;

      str render_split(CodeSplit fork, CodeSplit parent);
      str split_if(CodeSplit fork);
      str split_variable(CodeSplit fork, const str& code_after);
      str split_expression(CodeSplit fork);
      str split_and_render(code& c, CodeSplit parent);
      str split_method(XSSMethod method);
      str after_code(CodeSplit fork);
  };

register_complete_type(waxjs_code_renderer, renderer_schema<waxjs_code_renderer>);

}

#endif