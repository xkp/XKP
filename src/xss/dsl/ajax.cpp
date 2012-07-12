
#include <xss/dsl/ajax.h>
#include <xss/lang/waxjs.h>

using namespace xkp;

//dsl_native
str dsl_ajax::render(dsl& info, XSSContext ctx)
  {
    XSSContext my_ctx(new xss_context(ctx));
    Language lang(new waxjs_lang);

    my_ctx->set_language(lang);
    lang->init_application_context(my_ctx);

    std::vector<str> dl;
    my_ctx->collect_xss_dsls(dl);

    xs_compiler xc(dl);
    code ajax_code;
    xc.compile_code(info.text, ajax_code);

    param_list_decl plistdecl = param_list_decl();

    CodeRenderer rend = lang->compile_code(ajax_code, plistdecl, my_ctx);
    str result = rend->render();
    return result;
  }
