
#include <xss/jscript.h>
#include <xss/xss_error.h>

using namespace xkp;

const str SIdiom("idiom");
const str SEveryInstanceMustHaveId("Trying to use an instance without id");

//js_args
str js_args::resolve_param(const param_decl& param)
	{
		return param.name;
	}

//js_idiom
void js_idiom::set_context(XSSContext ctx)
	{
		ctx_ = ctx;
	}

variant js_idiom::process_code(code& cde, param_list_decl& params, XSSContext ctx)
	{
    JSCode result(new js_code(ctx, cde));
    return result;
	}

variant js_idiom::process_expression(expression expr, XSSObject this_)
	{
    XSSContext ctx(new xss_composite_context(ctx_));
    ctx->this_ = this_;

    JSExpression result(new js_expr(ctx, expr));
    return result;
	}

variant js_idiom::process_args(param_list_decl& params)
	{
		JSArgs result(new js_args(params));
		return result;
	}

str js_idiom::resolve_this(XSSContext ctx)
	{
    //Programming languages do not agree on how to use *this* pointers
    //c++ lets you ignore them, jsva script et all force you to 
    //and there are other intrincate circumstances (like functions in js)
    //where something must be done.
    
		XSSObject instance = variant_cast<XSSObject>(ctx->this_, XSSObject());
		bool		  override_id_as_this = instance->has("@class");

		if (!override_id_as_this && id_as_this_)
      {
        if (!ctx->this_.empty())
          {
            str iid = variant_cast<str>(dynamic_get(ctx->this_, "id"), ""); 
            if (iid.empty())
              {
                param_list error;
                error.add("id", SIdiom);
                error.add("desc", SEveryInstanceMustHaveId);
                xss_throw(error);
              }

            return iid;
          }
      }
    else if (!ctx->this_.empty())
      {
        return "this";
      }    
      
    return "";
	}

str js_idiom::resolve_separator(XSSObject lh)
	{
		return ".";
	}
