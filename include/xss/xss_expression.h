
#ifndef XSS_EXPRESSION_HH
#define XSS_EXPRESSION_HH

#include <xs.h>
#include "xss_context.h"

namespace xkp {


struct xss_expression_utils
  {
    static XSSExpression constant_expression(variant value);
    static XSSExpression compile_expression(expression& expr);
    static XSSExpression compile_expression(const str& expr);
  };

}

#endif