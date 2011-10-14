
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
        virtual str     property_get(XSSProperty prop, const str& path, XSSContext ctx);
        virtual str     property_set(XSSProperty prop, const str& path, const str& value, XSSContext ctx);
        virtual str     render_assignment(const str& path, const str& prop, const str& value);
        virtual str     expression_path(const str& expr );
        virtual str     array_operation(operator_type op, const str& arr, const str& value, XSSContext ctx);
        virtual str     render_expression(expression& expr, XSSContext ctx);
    };
}

#endif