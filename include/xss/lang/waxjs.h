
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

struct waxjs_code_renderer : public base_code_renderer
  {
    waxjs_code_renderer();
    waxjs_code_renderer(const waxjs_code_renderer& other);
    waxjs_code_renderer(code& cde, param_list_decl& params, XSSContext ctx);

    bool check_fork(variant owner);
    bool forked();
    void use_this_id(bool value);

    //ICodeRenderer
    virtual str render();

    protected:
      virtual str render_expression(expression& expr, XSSContext ctx);
      virtual str render_code(code& cde);

    private:
      XSSCompiler compiler_;
      bool        checked_;
      CodeSplit   fork_;

      str render_split(CodeSplit fork, CodeSplit parent);
      str split_if(CodeSplit fork);
      str split_variable(CodeSplit fork, const str& code_after);
      str split_expression(CodeSplit fork);
      str split_return(CodeSplit fork);
      str split_and_render(code& c, CodeSplit parent);
      str split_method(XSSMethod method);
      str after_code(CodeSplit fork);
      str render_service();
    public:
      XSSMethod owner_;
  };

//glue
struct waxjs_code_renderer_schema : renderer_code_schema<waxjs_code_renderer>
  {
    virtual void declare()
      {
        property_<XSSMethod>    ("owner",	      &waxjs_code_renderer::owner_);
        readonly_property<bool> ("forked",	    &waxjs_code_renderer::forked);
        method_<bool, 1>        ("check_fork",  &waxjs_code_renderer::check_fork);
        method_<void, 1>        ("use_this_id", &waxjs_code_renderer::use_this_id);
      }
  };

register_complete_type(waxjs_code_renderer, waxjs_code_renderer_schema);

}

#endif