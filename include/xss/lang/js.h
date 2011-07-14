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

//expression rendering
struct already_rendered
  {
    already_rendered() : precedence(0)
    {
    }

    str     value;
    variant object;
    int precedence;
  };

enum assign_type
	{
		VANILLA,
		FN_CALL,
		XSS_RESOLVE
	};

struct assign_info
	{
		assign_info() : type(VANILLA), this_(false) {}

		assign_type type;
		str					data;
		bool				this_;
	};

struct capture_property
	{
		XSSProperty prop;
		str					xss;
	};

//td: !!! delete this \/
struct __expression_renderer : expression_visitor
  {
    __expression_renderer(XSSContext ctx);


    virtual str resolve_assigner(variant operand, XSSObject instance, assign_info* ai);
    virtual str operand_to_string(variant operand, XSSObject parent = XSSObject(), int* prec = null);
    virtual str array_operation(str lopnd, str ropnd, operator_type op);
    virtual str get();

    assign_info* assigner;

    protected:
      typedef std::stack<variant> expr_stack;

      expr_stack        stack_;
      std::stringstream result_;
      XSSContext        ctx_;
			bool							capturing_property_;
			capture_property	capture_property_;

      void push_rendered(str value, int prec, variant object);
			str	 render_captured_property();
  };

struct js_expr_renderer : IExpressionRenderer,
                          expression_visitor
  {
    js_expr_renderer();
    js_expr_renderer(const js_expr_renderer& other);
    js_expr_renderer(expression& expr, XSSContext ctx);

    //IExpressionRenderer
    virtual XSSType type();
    virtual str     render();

    //expression_visitor
    virtual void push(variant operand, bool top);
    virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top);
    
    private:
      XSSContext ctx_;
      expression expr_;
      XSSType    type_;
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