
#include <xss/dsl/sql.h>
#include <xss/xss_compiler.h>
#include <xss/xss_error.h>

using namespace xkp;

XSSModule dsl_sql::get_idiom(XSSCompiler compiler, XSSContext ctx)
  {
    return compiler->idiom_by_class("sql_handler");
  }

str dsl_sql::get_event()
  {
    return "render_statements";
  }

void dsl_sql::add_parameters(param_list& pl, XSSContext ctx)
  {
    str callback = variant_cast<str>(ctx->resolve("#wax_callback"), str());
    pl.add("callback", callback);
  }
