
#include <xss/dsl/smarty.h>
#include <xss/xss_error.h>
#include <xss/language.h>

#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>

using namespace xkp;

const str SSmartyDSL("smarty");

const str SSmartyExpectsEitherDataOrFields("Smarty expects a data value or a collection os fields, not both or neither");
const str SSmartyExpectsLiteralHtmlId("Smarty expects a html_id value");

//dsl_native
str dsl_smarty::render(dsl& info, XSSContext ctx)
  {
    
    boost::hash<std::string> string_hash;
    size_t hash    = string_hash(info.text);
    str hash_id = "tpl" + boost::lexical_cast<str>(hash);

    str html_id;
    str data_id;
    process_params(info.params, ctx, html_id, data_id);

    if (data_id.empty())
      {
        param_list error;
        error.add("id", SSmartyDSL);
        error.add("desc", SSmartyExpectsEitherDataOrFields);
        xss_throw(error);
      }

    return html_id + ".inner_html = application." + hash_id + ".fetch(" + data_id + ");";
  }

void dsl_smarty::pre_process(dsl& info, XSSContext ctx)
  {
    //check our 
    str html_id;
    str data_id;
    process_params(info.params, ctx, html_id, data_id);

    if (html_id.empty())
      {
        param_list error;
        error.add("id", SSmartyDSL);
        error.add("desc", SSmartyExpectsLiteralHtmlId);
        xss_throw(error);
      }

    //we need to let the wax compiler know it needs to add modifiers 
    DynamicArray extra_modifiers = variant_cast<DynamicArray>(ctx->resolve("#extra_modifiers"), DynamicArray());
    if (!extra_modifiers)
      {
        extra_modifiers = DynamicArray(new dynamic_array);
        ctx->register_symbol(RESOLVE_CONST, "#extra_modifiers", extra_modifiers);
      }

    XSSObject extra_modifier(new xss_object);
    extra_modifier->add_attribute("html_id", html_id);
    extra_modifier->add_attribute("modifier", str("inner_html"));
    extra_modifiers->push_back(extra_modifier);

    //we'll create an initializer to be later rendered
    add_initializer(ctx, info.text);
  }

void dsl_smarty::process_params(param_list& params, XSSContext ctx, str& html_id, str& data_id)
  {
    html_id = str();
    data_id = str();

    Language lang = ctx->get_language();
    bool has_data = false;
    for(size_t i = 0; i < params.size(); i++)
      {
        str        param_name  = params.get_name(i);
        expression param_value = variant_cast<expression>(params.get(i), expression());

        if (param_name == "html_id")
          {
            if (!param_value.empty())
              {
                expression_analizer ea;
                ea.analyze(param_value, ctx);
        
                html_id = variant_cast<str>(ea.get_constant(), str());
                if (html_id.empty())
                  {
                    param_list error;
                    error.add("id", SSmartyDSL);
                    error.add("desc", SSmartyExpectsLiteralHtmlId);
                    xss_throw(error);
                  }
              }
          }
        else if (param_name == "data" || param_name.empty())
          {
            has_data = true;

            if (!data_id.empty())
              {
                param_list error;
                error.add("id", SSmartyDSL);
                error.add("desc", SSmartyExpectsEitherDataOrFields);
                xss_throw(error);
              }
              
            data_id = lang->render_expression(param_value, ctx);
          }
        else
          {
            if (has_data)
              {
                param_list error;
                error.add("id", SSmartyDSL);
                error.add("desc", SSmartyExpectsEitherDataOrFields);
                error.add("field", param_name);
                xss_throw(error);
              }

            if (data_id.empty())
              data_id = "{";
            else
              data_id += ",";
            
            data_id += param_name + " : " + lang->render_expression(param_value, ctx);
          }
      }

    if (!data_id.empty() && !has_data)
      data_id += "}";
  }

void dsl_smarty::pre_process_file(const str& tpl_contents, XSSContext ctx)
  {
    add_initializer(ctx, tpl_contents);
  }

str dsl_smarty::gen_call(const str& contents, XSSContext ctx, param_list& args)
  {
    boost::hash<std::string> string_hash;
    size_t hash    = string_hash(contents);
    str hash_id = "tpl" + boost::lexical_cast<str>(hash);

    str html_id;
    str data_id;
    process_params(args, ctx, html_id, data_id);
    return "application." + hash_id + ".fetch(" + data_id + ")";
  }

void dsl_smarty::add_initializer(XSSContext ctx, const str& text)
  {
    XSSObject app = ctx->resolve("application");
    DynamicArray initializers;
    initializers = app->get<DynamicArray>("initializers", DynamicArray());
    if (!initializers)
      {
        initializers = DynamicArray(new dynamic_array);
        app->add_attribute("initializers", initializers);
      }

    boost::hash<std::string> string_hash;
    size_t hash    = string_hash(text);
    str hash_id = "tpl" + boost::lexical_cast<str>(hash);

    XSSObject initializer(new xss_object);
    initializer->set_id(hash_id);
    initializer->add_attribute("text", text);
    initializer->add_attribute("renderer", str("smarty_init.xss"));

    initializers->push_back(initializer);
  }
