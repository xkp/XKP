
#ifndef XS_COMPILER_H
#define XS_COMPILER_H

#include <types.h>
#include <xs/code.h>

namespace xkp
{
  struct xs_compiler
    {
      bool compile_expression(const str& expr, code_context& result);
      bool compile_code(const str& code, code_context& result);
    };
}

#endif