
#include <xss/lang/js.h>
#include <xss/language.h>

using namespace xkp;

//js_code_renderer
js_code_renderer::js_code_renderer(): 
  base_code_renderer()
  {
  }

js_code_renderer::js_code_renderer(const js_code_renderer& other):
  base_code_renderer(other)
  {
  }

js_code_renderer::js_code_renderer(code& cde, XSSContext ctx):
  base_code_renderer(cde, ctx)
  {
  }

void js_code_renderer::set_this(variant this_)
  {
    ctx_->register_symbol(RESOLVE_CONST, "#this", this_); 
    ctx_->set_this(variant_cast<XSSObject>(this_, XSSObject()));
  }

str js_code_renderer::render_expression(expression& expr, XSSContext ctx)
  {
    return lang_utils::render_expression<js_expr_renderer>(expr, ctx);
  }

str js_code_renderer::render_code(code& cde)
  {
    js_code_renderer inner(cde, ctx_); 
    str result = inner.render();
    add_line(result);
    return result;
  }

//js_expr_renderer
js_expr_renderer::js_expr_renderer():
  base_expr_renderer()
  {
  }

js_expr_renderer::js_expr_renderer(const js_expr_renderer& other):
  base_expr_renderer(other)
  {
  }

js_expr_renderer::js_expr_renderer(expression& expr, XSSContext ctx):
  base_expr_renderer(expr, ctx)
  {
  }

str js_expr_renderer::render()
  {
    return lang_utils::render_expression<js_expr_renderer>(expr_, ctx_);
  }

//js_args_renderer 
js_args_renderer::js_args_renderer():
  base_args_renderer()
  {
  }

js_args_renderer::js_args_renderer(const js_args_renderer& other):
  base_args_renderer(other)
  {
  }

js_args_renderer::js_args_renderer(param_list_decl& params, XSSContext ctx):
  base_args_renderer(params, ctx)
  {
  }

str js_args_renderer::render()
  {
    std::ostringstream oss;

    param_list_decl::iterator it = args_.begin();
    param_list_decl::iterator nd = args_.end();
    for(; it != nd; it++)
      {
        oss << it->name << ","; //td: interface
      }

    str result = oss.str();
    if (!result.empty())
      result.erase(result.end() - 1);

    return result;
  }

//js_lang
variant js_lang::compile_code(code& cde, param_list_decl& params, XSSContext ctx)
  {
    return reference<js_code_renderer>(new js_code_renderer(cde, ctx));
  }

variant js_lang::compile_expression(expression expr, XSSContext ctx)
  {
    return reference<js_expr_renderer>(new js_expr_renderer(expr, ctx));
  }

variant js_lang::compile_args(param_list_decl& params, XSSContext ctx)
  {
    return reference<js_args_renderer>(new js_args_renderer(params, ctx));
  }
