
#include <xss/lang/js.h>
#include <xss/language.h>
#include <xss/xss_compiler.h>
#include <xss/xss_error.h>
#include <xss/dsl/html5_asynch.h>
#include <xss/dsl/ajax.h>

using namespace xkp;

const str SJSLanguage("js-language");

const str SWriteOnlyProperty("Trying to acces write only property");
const str SReadOnlyProperty("Trying to acces read only property");

//js_code_renderer
js_code_renderer::js_code_renderer():
  base_code_renderer()
  {
  }

js_code_renderer::js_code_renderer(const js_code_renderer& other):
  base_code_renderer(other)
  {
  }

js_code_renderer::js_code_renderer(code& cde, param_list_decl& params, XSSContext ctx):
  base_code_renderer(cde, params, ctx)
  {
  }

void js_code_renderer::use_this_id(bool value)
  {
    ctx_->register_symbol(RESOLVE_CONST, "#use_this_id", value, true);
  }

void js_code_renderer::force_this(bool value)
  {
    ctx_->register_symbol(RESOLVE_CONST, "#force_this", value);
  }

void js_code_renderer::this_string(const str& value)
  {
    ctx_->register_symbol(RESOLVE_CONST, "#this_string", value);
  }

str js_code_renderer::render_expression(expression& expr, XSSContext ctx)
  {
    return lang_utils::render_expression<js_expr_renderer>(expr, ctx);
  }

str js_code_renderer::render_code(code& cde)
  {
    XSSContext ctx(new xss_context(ctx_));
    lang_utils::var_gatherer(cde, ctx);

    param_list_decl pld;
    js_code_renderer inner(cde, pld, ctx);
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

str js_expr_renderer::render_expression(expression& expr)
  {
    return lang_utils::render_expression<js_expr_renderer>(expr, ctx_);
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

    bool first = true;
    for(; it != nd; it++)
      {
        if (first)
          first = false;
        else
          oss << ",";

        oss << it->name; 
      }

    extra_arg_list::iterator eit = extra_.begin();
    extra_arg_list::iterator end = extra_.end();
    for(; eit != end; eit++)
      {
        if (first)
          first = false;
        else
          oss << ",";

        oss << eit->first; 
      }

    return oss.str();
  }

str js_args_renderer::render_expression(expression& expr, XSSContext ctx)
  {
    return lang_utils::render_expression<js_expr_renderer>(expr, ctx);
  }

//js_lang
void js_lang::init_application_context(XSSContext ctx)
  {
    ctx->register_xss_dsl("asynch", XSSDSL(new dsl_h5_asynch));
    ctx->register_xss_dsl("ajax", XSSDSL(new dsl_ajax)); //td: !!! manage dsl better
  }

variant js_lang::compile_code(code& cde, param_list_decl& params, XSSContext ctx)
  {
    return reference<js_code_renderer>(new js_code_renderer(cde, params, ctx));
  }

variant js_lang::compile_expression(expression expr, XSSContext ctx)
  {
    return reference<js_expr_renderer>(new js_expr_renderer(expr, ctx));
  }

variant js_lang::compile_args(param_list_decl& params, XSSContext ctx)
  {
    return reference<js_args_renderer>(new js_args_renderer(params, ctx));
  }

str js_lang::resolve_this(XSSContext ctx)
  {
    bool use_id = variant_cast<bool>(ctx->resolve("#use_this_id", RESOLVE_CONST), false);
    if (use_id)
      {
        XSSObject this_ = ctx->get_this(); assert(this_);
        return this_->output_id();
      }
    
    str this_string = variant_cast<str>(ctx->resolve("#this_string", RESOLVE_CONST), str());        
    if (!this_string.empty())
      return this_string;

    return "this";
  }

str js_lang::property_get(XSSProperty prop, const str& path, XSSContext ctx)
  {
    compile_property(prop, ctx);
    XSSObject get = prop->find("get");
    if (get)
      {
        XSSRenderer rend   = XSSObject(prop)->get<XSSRenderer>("#get_renderer", XSSRenderer());
        bool        global = get->get<bool>("global", false);

        param_list params;
        params.add("path", path);
        str result = rend->render(XSSObject(), &params);
        return global? result : path + "." + result;
      }
    else
      {
        XSSObject set = prop->find("set");
        if (set)
          {
            //read only, set but no get
            param_list error;
            error.add("id", SJSLanguage);
            error.add("desc", SReadOnlyProperty);
            error.add("property", prop->id());
            xss_throw(error);
          }
        else
          {
            return path + "." + prop->output_id();
          }
      }

    return str();
  }

str js_lang::property_set(XSSProperty prop, const str& path, const str& value, XSSContext ctx)
  {
    compile_property(prop, ctx);
    XSSObject set = prop->find("set");
    if (set)
      {
        XSSRenderer rend   = XSSObject(prop)->get<XSSRenderer>("#set_renderer", XSSRenderer());
        bool        global = set->get<bool>("global", false);

        param_list params;
        params.add("path", path);
        params.add("value", value);

        str result = rend->render(XSSObject(), &params);
        return global? result : path + "." + result;
      }
    else
      {
        XSSObject get = prop->find("get");
        if (get)
          {
            param_list error;
            error.add("id", SJSLanguage);
            error.add("desc", SWriteOnlyProperty);
            error.add("property", prop->id());
            xss_throw(error);
          }
        else
          {
            return path + "." + prop->output_id() + " = " + value;
          }
      }

    return str();
  }

str js_lang::render_assignment(const str& path, const str& prop, const str& value)
  {
    if (path.empty())
      return prop + " = " + value;
    return path + "." + prop + " = " + value;
  }

str js_lang::expression_path(const str& expr )
  {
    size_t pos = expr.find_last_of(".");
    if (pos != str::npos)
      {
        return str(expr.begin(), expr.begin() + pos);
      }
    return str();
  }

str js_lang::array_operation(operator_type op, const str& arr, const str& value, XSSContext ctx)
  {
    if (op == op_plus_equal)
      return arr + ".push(" + value + ")";
    else if (op == op_minus_equal)
      return arr + ".erase(" + value + ")";

    assert(false); //only this operators are supported, is this a valid use case?
    return str();
  }

str js_lang::instantiate(XSSType type, XSSObject instance, DynamicArray rt, param_list& args)
  {
    if (type->id() == "object")
      return "{}";

    return base_lang::instantiate(type, instance, rt, args);
  }

bool js_lang::custom_operator(XSSType lt, XSSType rt, str l, str r, operator_type op, str& res)
  {
    if (op == op_typecheck)
      {
        res = l + " instanceof " + r;
        return true;
      }
    return false;
  }
