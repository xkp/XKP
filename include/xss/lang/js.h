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

    void use_this_id(bool value);
    void force_this(bool value);

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
    virtual str     resolve_this(XSSContext ctx);
    virtual str     property_get(XSSProperty prop, const str& path, XSSContext ctx);
    virtual str     property_set(XSSProperty prop, const str& path, const str& value, XSSContext ctx);
    virtual str     render_assignment(const str& path, const str& prop, const str& value);
    virtual str     expression_path(const str& expr );
    virtual str     array_operation(operator_type op, const str& arr, const str& value, XSSContext ctx);
    virtual bool    custom_operator(XSSType lt, XSSType rt, str l, str r, operator_type op, str& res);
  };

//glue
struct js_code_renderer_schema : renderer_code_schema<js_code_renderer>
  {
    virtual void declare()
      {
        method_<void, 1>("use_this_id", &js_code_renderer::use_this_id);
        method_<void, 1>("force_this", &js_code_renderer::force_this);
      }
  };

register_complete_type(js_code_renderer, js_code_renderer_schema);
register_complete_type(js_expr_renderer, renderer_expr_schema<js_expr_renderer>);
register_complete_type(js_args_renderer, renderer_args_schema<js_args_renderer>);

}
#endif