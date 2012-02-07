
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

//str dsl_sql::render(dsl& info, XSSContext ctx)
//  {
//    //do the parsing
//    assign_context actx;
//    
//    assign_grammar grammar(actx);
//    parse_info<iterator_t> pi;
//    bool succeeded = true;
//    try
//      {
//        pi = boost::spirit::classic::parse(info.text.begin(), info.text.end(), *grammar, space_p);
//      }
//    catch(int e)
//      {
//        succeeded = e == 1234;
//      }
//      
//    if (!succeeded)
//      {
//        param_list error;
//        error.add("id", SLanguage);
//        error.add("desc", SCannotParseSQL);
//        error.add("text", info.text);
//        xss_throw(error);
//      }
//
//    str callback = variant_cast<str>(ctx->resolve("#wax_callback"), str());
//    return render_statement(actx.tags, callback, 0);
//  }
