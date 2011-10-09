#ifndef XSS_LANG_JAVA_H
#define XSS_LANG_JAVA_H

#include <xss/xss_context.h>
#include <xss/lang/base.h>

namespace xkp{

struct java_code_renderer : public base_code_renderer
  {
    java_code_renderer();
    java_code_renderer(const java_code_renderer& other);
    java_code_renderer(code& cde, param_list_decl& params, XSSContext ctx, int indent = 0);

    //code_visitor
    virtual void variable_(stmt_variable& info);
    virtual void for_(stmt_for& info);
    virtual void iterfor_(stmt_iter_for& info);

    protected:
      virtual str render_expression(expression& expr, XSSContext ctx);
      virtual str render_code(code& cde);
  };

struct java_expr_renderer : public base_expr_renderer
  {
    java_expr_renderer();
    java_expr_renderer(const java_expr_renderer& other);
    java_expr_renderer(expression& expr, XSSContext ctx);

    //interface, verbatim from 0.8, got to keep it working
    virtual str operand_to_string(variant operand, XSSObject parent = XSSObject(), int* prec = null);
    virtual str array_operation(const str& left, const str& right, operator_type op);

    //IExpressionRenderer
    virtual str     render();

    //expression_visitor
    virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top);
  };

struct java_args_renderer : public base_args_renderer
  {
    java_args_renderer();
    java_args_renderer(const java_args_renderer& other);
    java_args_renderer(param_list_decl& params, XSSContext ctx);

    virtual str render();

    protected:
      virtual str render_expression(expression& expr, XSSContext ctx);
  };

struct java_lang : public base_lang
  {
    virtual variant compile_code(code& cde, param_list_decl& params, XSSContext ctx);
    virtual variant compile_expression(expression expr, XSSContext ctx);
		virtual variant compile_args(param_list_decl& params, XSSContext ctx);
    virtual str     resolve_this(XSSContext ctx);
    virtual bool    can_cast(XSSType left, XSSType right);
    virtual void    init_context(XSSContext ctx);
    virtual XSSType resolve_array_type(XSSType type, const str& at_name, XSSContext ctx);
    virtual str     render_value(XSSType type, variant value);
    virtual str     property_get(XSSProperty prop, const str& path, XSSContext ctx);
    virtual str     property_set(XSSProperty prop, const str& path, const str& value, XSSContext ctx);
    virtual str     render_asignment(const str& path, const str& prop, const str& value);
  };

register_complete_type(java_code_renderer, renderer_code_schema<java_code_renderer>);
register_complete_type(java_expr_renderer, renderer_expr_schema<java_expr_renderer>);
register_complete_type(java_args_renderer, renderer_schema<java_args_renderer>);

}
#endif
