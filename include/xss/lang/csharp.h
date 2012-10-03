
#ifndef XSS_CSHARP_HH
#define XSS_CSHARP_HH

#include "typed.h"

namespace xkp{

struct cs_lang : public typed_lang
  {
    virtual bool render_foreach(IStatementForEach* info, XSSContext ctx, std::ostringstream& result);
    virtual bool render_object(value_operation& op, XSSContext ctx, std::ostringstream& result);
    virtual bool render_array(value_operation& op, XSSContext ctx, std::ostringstream& result);
    virtual bool render_pre_statement(XSSStatement info, XSSContext ctx, std::ostringstream& result);  
  };

}

#endif