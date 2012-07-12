
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

void dsl_shell::add_parameters(param_list& pl, XSSContext ctx)
  {
    str callback      = variant_cast<str>(ctx->resolve("#wax_callback"), str());
    
    pl.add("callback", callback);
    pl.add("error_handler", ctx->resolve("#wax_error_handler"));
  }
