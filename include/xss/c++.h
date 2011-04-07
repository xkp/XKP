
#ifndef XSS_CPP
#define XSS_CPP

#include <xss/idiom.h>

namespace xkp{

struct cpp_code : base_xss_code
	{
		cpp_code()													 : base_xss_code()					 {};
		cpp_code(XSSContext ctx, code& code) : base_xss_code(ctx, code) {};

    virtual void for_(stmt_for& info);
    virtual void variable_(stmt_variable& info);

    protected:
      virtual str render_expression(expression& expr, XSSContext ctx);
	};

struct cpp_expr : base_xss_expression
	{
		cpp_expr()																 : base_xss_expression()					 {};
		cpp_expr(XSSContext ctx, expression& expr) : base_xss_expression(ctx, expr) {};
	};

struct cpp_args : base_xss_args
	{
		cpp_args()	             				: base_xss_args()	    {};
		cpp_args(param_list_decl& args) : base_xss_args(args) {};

		virtual str resolve_param(const param_decl& param);
	};

typedef reference<cpp_code> CPPCode;
typedef reference<cpp_expr> CPPExpression;
typedef reference<cpp_args> CPPArgs;

//then the idiom per se
struct cpp_idiom : base_idiom
  {
    cpp_idiom()                       : id_as_this_(false)                   {}
    cpp_idiom(const cpp_idiom& other)	: base_idiom(other),id_as_this_(false) {}

    virtual variant process_code(code& cde, param_list_decl& params, XSSContext ctx);
    virtual variant process_expression(expression expr, XSSObject this_);
		virtual variant process_args(param_list_decl& params);
    virtual str     resolve_this(XSSContext ctx);
    virtual str     resolve_separator(XSSObject lh);

    public:
      bool id_as_this_;
  };

struct cpp_expression_renderer : expression_renderer
  {
    cpp_expression_renderer(XSSContext ctx, bool id_as_this = false)
      : expression_renderer(ctx, id_as_this) {}

    virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top);

    virtual str resolve_assigner(variant operand, XSSObject instance, assign_info* ai);
    virtual str operand_to_string(variant operand, XSSObject parent = XSSObject(), int* prec = null);
    virtual str get();
  };

//typeinfo
struct cpp_idiom_schema : base_idiom_schema<cpp_idiom>
  {
    virtual void declare()
      {
        implements<xss_idiom>();
        property_("id_as_this", &cpp_idiom::id_as_this_);
      }
  };

struct cpp_code_schema : translator_schema<cpp_code>
	{
	};

struct cpp_expr_schema : translator_schema<cpp_expr>
	{
	};

struct cpp_args_schema : translator_schema<cpp_args>
	{
	};

register_complete_type(cpp_idiom, cpp_idiom_schema);
register_complete_type(cpp_code,  cpp_code_schema);
register_complete_type(cpp_expr,  cpp_expr_schema);
register_complete_type(cpp_args,  cpp_args_schema);

} //namespace

#endif
