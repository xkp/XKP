
#include <xss/dsl/native.h>

using namespace xkp;

//dsl_native
str dsl_native::render(dsl& info, XSSContext ctx)
  {
    XSSCompiler compiler = ctx->resolve("compiler");
    XSSRenderer rend     = compiler->current_renderer();
    rend->append(info.text);

    return info.text;
  }
