
#ifndef XSS_JSCRIPT
#define XSS_JSCRIPT

#include <xss/idiom.h>

namespace xkp{

//unfortunately you must inherit the classes 
//to conform to the type system
struct js_code : base_xss_code
	{
		js_code()														: base_xss_code()					 {};
		js_code(XSSContext ctx, code& code) : base_xss_code(ctx, code) {};
	};

struct js_expr : base_xss_expression
	{
		js_expr()																	: base_xss_expression()					 {};
		js_expr(XSSContext ctx, expression& expr) : base_xss_expression(ctx, expr) {};
	};

struct js_args : base_xss_args
	{
		js_args()											 : base_xss_args()		 {};
		js_args(param_list_decl& args) : base_xss_args(args) {}; 

		virtual str resolve_param(const param_decl& param);
	};

typedef reference<js_code> JSCode;
typedef reference<js_expr> JSExpression;
typedef reference<js_args> JSArgs;

//then the idiom per se
struct js_idiom : base_idiom
  {
    js_idiom()                      : id_as_this_(false)										{}    
    js_idiom(const js_idiom& other)	: base_idiom(other), id_as_this_(false) {}
  
    virtual variant process_code(code& cde, param_list_decl& params, XSSContext ctx);
    virtual variant process_expression(expression expr, XSSObject this_);
		virtual variant process_args(param_list_decl& params);
    virtual str     resolve_this(XSSContext ctx);
    public:  
      bool id_as_this_;
  };

//typeinfo
struct js_idiom_schema : base_idiom_schema<js_idiom>
  {
    virtual void declare()
      {
        implements<xss_idiom>();
        property_("id_as_this", &js_idiom::id_as_this_);
      }
  };
  
struct js_code_schema : translator_schema<js_code>
	{
	};

struct js_expr_schema : translator_schema<js_expr>
	{
	};

struct js_args_schema : translator_schema<js_args>
	{
	};

register_complete_type(js_idiom, js_idiom_schema);
register_complete_type(js_code,  js_code_schema);
register_complete_type(js_expr,  js_expr_schema);
register_complete_type(js_args,  js_args_schema);

} //namespace

#endif