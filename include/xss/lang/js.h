#ifndef XSS_LANG_JS_H
#define XSS_LANG_JS_H

#include <xss/xss_context.h>
#include <xss/lang/base.h>

namespace xkp{

struct js_code_renderer : public base_code_renderer
  {
    js_code_renderer();
    js_code_renderer(const js_code_renderer& other);
    js_code_renderer(code& cde, param_list_decl& params, XSSContext ctx);

    void set_this(variant this_);

    protected:
      virtual str render_expression(expression& expr, XSSContext ctx);
      virtual str render_code(code& cde);
  };

struct js_expr_renderer : public base_expr_renderer
  {
    js_expr_renderer();
    js_expr_renderer(const js_expr_renderer& other);
    js_expr_renderer(expression& expr, XSSContext ctx);

    //IExpressionRenderer
    virtual str     render();
  };

struct js_args_renderer : public base_args_renderer
  {
    js_args_renderer();
    js_args_renderer(const js_args_renderer& other);
    js_args_renderer(param_list_decl& params, XSSContext ctx);

    virtual str render();

    protected:
      virtual str render_expression(expression& expr, XSSContext ctx);
  };

struct js_lang : public base_lang
  {
    virtual variant compile_code(code& cde, param_list_decl& params, XSSContext ctx);
    virtual variant compile_expression(expression expr, XSSContext ctx);
		virtual variant compile_args(param_list_decl& params, XSSContext ctx);
  };

//glue
struct js_code_renderer_schema : renderer_code_schema<js_code_renderer>
  {
    virtual void declare()
      {
        method_<void, 1>("set_this", &js_code_renderer::set_this);
      }
  };

register_complete_type(js_code_renderer, js_code_renderer_schema);
register_complete_type(js_expr_renderer, renderer_expr_schema<js_expr_renderer>);
register_complete_type(js_args_renderer, renderer_schema<js_args_renderer>);

}
#endif