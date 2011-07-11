#include <xss/lang/js.h>

using namespace xkp;

//js_code_renderer
js_code_renderer::js_code_renderer()
  {
      code            code_;
      param_list_decl params_;
      XSSContext      ctx_;
      str             result_;
      str             expr_;
  }

js_code_renderer::js_code_renderer(const js_code_renderer& other)
  {
  }

js_code_renderer::js_code_renderer(code& cde, XSSContext ctx)
  {
  }

str js_code_renderer::render()
  {
  }

XSSType js_code_renderer::type()
  {
  }

void js_code_renderer::if_(stmt_if& info)
  {
  }

void js_code_renderer::variable_(stmt_variable& info)
  {
  }

void js_code_renderer::for_(stmt_for& info)
  {
  }

void js_code_renderer::iterfor_(stmt_iter_for& info)
  {
  }

void js_code_renderer::while_(stmt_while& info)
  {
  }

void js_code_renderer::break_()
  {
  }

void js_code_renderer::continue_()
  {
  }

void js_code_renderer::return_(stmt_return& info)
  {
  }

void js_code_renderer::expression_(stmt_expression& info)
  {
  }

void js_code_renderer::dsl_(dsl& info)
  {
  }

void js_code_renderer::dispatch(stmt_dispatch& info)
  {
  }

str js_code_renderer::render_expression(expression& expr)
  {
  }

str js_code_renderer::render_code(code& cde)
  {
  }

//js_lang
variant js_lang::compile_code(code& cde, param_list_decl& params, XSSContext ctx)
  {
    return reference<js_code_renderer>(new js_code_renderer(cde, ctx));
  }

variant js_lang::compile_expression(expression expr, XSSContext ctx)
  {
    return reference<js_expr_renderer>(new js_expr_renderer(expr));
  }

variant js_lang::compile_args(param_list_decl& params, XSSContext ctx)
  {
    return reference<js_args_renderer>(new js_args_renderer(params));
  }

str js_lang::resolve_this(XSSContext ctx)
  {
    return "this"; //td: deal with java script nested functions
  }

str js_lang::resolve_separator(XSSObject lh)
  {
    return ".";
  }

