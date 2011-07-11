#ifndef XSS_LANG_JS_H
#define XSS_LANG_JS_H

#include <xss/xss_context.h>

namespace xkp{

struct js_code_renderer : ICodeRenderer,
                          code_visitor
  {
    js_code_renderer();
    js_code_renderer(const js_code_renderer& other);
    js_code_renderer(code& cde, XSSContext ctx);

    //ICodeRenderer
    virtual str     render();
    virtual XSSType type();

    //code_visitor
    virtual void if_(stmt_if& info);
    virtual void variable_(stmt_variable& info);
    virtual void for_(stmt_for& info);
    virtual void iterfor_(stmt_iter_for& info);
    virtual void while_(stmt_while& info);
    virtual void break_();
    virtual void continue_();
    virtual void return_(stmt_return& info);
    virtual void expression_(stmt_expression& info);
    virtual void dsl_(dsl& info);
    virtual void dispatch(stmt_dispatch& info);

    private:
      code            code_;
      param_list_decl params_;
      XSSContext      ctx_;
      str             result_;
      str             expr_;

      str render_expression(expression& expr);
      str render_code(code& cde);
  };

struct js_expr_renderer : IExpressionRenderer
  {
    js_expr_renderer();
    js_expr_renderer(const js_expr_renderer& other);
    js_expr_renderer(expression& expr);

    virtual XSSType type();
    virtual str     render();
    
    private:
      expression expr_;
  };

struct js_args_renderer : public IArgumentRenderer
  {
    js_args_renderer();
    js_args_renderer(const js_args_renderer& other);
    js_args_renderer(param_list_decl& params);

    virtual str render();
  };

struct js_lang : public ILanguage
  {
    virtual variant compile_code(code& cde, param_list_decl& params, XSSContext ctx);
    virtual variant compile_expression(expression expr, XSSContext ctx);
		virtual variant compile_args(param_list_decl& params, XSSContext ctx);
    virtual str     resolve_this(XSSContext ctx);
    virtual str     resolve_separator(XSSObject lh = XSSObject());
  };

}
#endif