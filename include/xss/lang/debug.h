
#ifndef DEBUG_LANGUAGE_HH
#define DEBUG_LANGUAGE_HH

#include "xss/xss_context.h"

namespace xkp
{

  class debug_language : public ILanguage
    {
      public:  
        virtual variant compile_code(code& cde, param_list_decl& params, XSSContext ctx);
        virtual variant compile_expression(expression expr, XSSContext ctx);
		    virtual variant compile_args(param_list_decl& params, XSSContext ctx);
        virtual str     resolve_this(XSSContext ctx);
        virtual str     resolve_separator(XSSObject lh = XSSObject());
        virtual bool    can_cast(XSSType left, XSSType right);
        virtual void    init_context(XSSContext ctx);
        virtual XSSType resolve_array_type(XSSType type, const str& at_name, XSSContext ctx);
        virtual str     render_value(XSSType type, variant value);
    };
}

#endif