
#ifndef XSS_JAVA_HH
#define XSS_JAVA_HH

#include "typed.h"

namespace xkp{

struct java_lang : public typed_lang
  {
    virtual XSSContext create_context();
    virtual bool render_type_name(XSSType type, XSSContext ctx, std::ostringstream& result);
    virtual bool render_foreach(IStatementForEach* info, XSSContext ctx, std::ostringstream& result);
    virtual bool render_object(value_operation& op, XSSContext ctx, std::ostringstream& result);
    virtual bool render_array(value_operation& op, XSSContext ctx, std::ostringstream& result);
    virtual bool render_pre_statement(XSSStatement info, XSSContext ctx, std::ostringstream& result);
    virtual bool render_array_assignment(XSSExpression expr, XSSArguments index, XSSContext ctx, std::ostringstream& result); //handled as a separate case, for complications
    virtual bool render_operator(XSSExpression expr, XSSContext ctx, std::ostringstream& result);
    virtual bool render_constant(variant& value, XSSContext ctx, std::ostringstream& result);
  };

}
#endif
