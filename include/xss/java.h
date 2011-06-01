
#ifndef XSS_JAVA
#define XSS_JAVA

#include <xss/idiom.h>

namespace xkp{

struct java_code : base_xss_code
	{
    typedef std::map<str, schema*> map_variables;

		java_code()	                                               : base_xss_code() {};
    java_code(XSSContext ctx, code& code, map_variables &vars) : base_xss_code(ctx, code) 
      {
        vars_ = vars;
      };

    virtual void variable_(stmt_variable& info);
    virtual void for_(stmt_for& info);
    virtual void iterfor_(stmt_iter_for& info);

    protected:
      virtual str render_expression(expression& expr, XSSContext ctx);
    private:
      // return the type name of the given variable name
      // from std::map<str, schema*> vars_
      str get_type_name(const str& var_name);
	};

struct java_expr : base_xss_expression
	{
		java_expr()																  : base_xss_expression()					 {};
		java_expr(XSSContext ctx, expression& expr) : base_xss_expression(ctx, expr) {};
	};

struct java_args : base_xss_args
	{
		java_args()	             				 : base_xss_args()	   {};
		java_args(param_list_decl& args) : base_xss_args(args) {};

		virtual str resolve_param(const param_decl& param);
	};

typedef reference<java_code> JavaCode;
typedef reference<java_expr> JavaExpression;
typedef reference<java_args> JavaArgs;

//then the idiom per se
struct java_idiom : base_idiom
  {
    java_idiom()                                            {}
    java_idiom(const java_idiom& other)	: base_idiom(other) {}

    virtual variant process_code(code& cde, param_list_decl& params, XSSContext ctx);
    virtual variant process_expression(expression expr, XSSObject this_);
		virtual variant process_args(param_list_decl& params);
    virtual str     resolve_this(XSSContext ctx);
    virtual str     resolve_separator(XSSObject lh);
  };

struct java_expression_renderer : expression_renderer
  {
    java_expression_renderer(XSSContext ctx, bool id_as_this = false)
      : expression_renderer(ctx, id_as_this) {}

    //expression_visitor
    virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top);
  };

//typeinfo
struct java_idiom_schema : base_idiom_schema<java_idiom>
  {
    virtual void declare()
      {
        implements<java_idiom>();
      }
  };

struct java_code_schema : translator_schema<java_code>
	{
	};

struct java_expr_schema : translator_schema<java_expr>
	{
	};

struct java_args_schema : translator_schema<java_args>
	{
	};

register_complete_type(java_idiom, java_idiom_schema);
register_complete_type(java_code,  java_code_schema);
register_complete_type(java_expr,  java_expr_schema);
register_complete_type(java_args,  java_args_schema);

} //namespace

#endif
