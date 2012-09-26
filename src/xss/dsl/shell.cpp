
#include <xss/dsl/shell.h>
#include <xss/xss_compiler.h>
#include <xss/xss_error.h>

using namespace xkp;

XSSModule dsl_shell::get_idiom(XSSCompiler compiler, XSSContext ctx)
  {
    return compiler->idiom_by_id("shell");
  }

str dsl_shell::get_event()
  {
    return "render_commands";
  }

void dsl_shell::add_parameters(param_list& pl, dsl& info, XSSContext ctx)
  {
    str callback      = variant_cast<str>(ctx->resolve("#wax_callback"), str());
    
    pl.add("callback", callback);
    pl.add("error_handler", ctx->resolve("#wax_error_handler"));

    param_list options;
    load_parameter("cwd", ctx, info.params, options);
    load_parameter("env", ctx, info.params, options);

    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for(size_t i = 0; i < options.size(); i++)
      {
        if (first) first = false;
        else oss << ", ";

        str pname = options.get_name(i);
        str pvalue = variant_cast<str>(options.get(i), str("null"));

        oss << pname << " : " << pvalue;
      }

    oss << "}";
    pl.add("options", oss.str());
  }
