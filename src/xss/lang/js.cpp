
#include <xss/lang/js.h>
#include <xss/language.h>
#include <xss/xss_compiler.h>
#include <xss/xss_error.h>

using namespace xkp;

const str SLanguage("language");

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
    ctx_->register_symbol(RESOLVE_CONST, "#use_this_id", value);
  }

str js_code_renderer::render_expression(expression& expr, XSSContext ctx)
  {
    return lang_utils::render_expression<js_expr_renderer>(expr, ctx);
  }

str js_code_renderer::render_code(code& cde)
  {
    param_list_decl pld;
    js_code_renderer inner(cde, pld, ctx_);
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

str js_args_renderer::render_expression(expression& expr, XSSContext ctx)
  {
    return lang_utils::render_expression<js_expr_renderer>(expr, ctx);
  }

//js_lang
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
            error.add("id", SLanguage);
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
            error.add("id", SLanguage);
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

