#ifndef BRACE_PARSER_HH
#define BRACE_PARSER_HH

#include <base.h>
#include "xss_compiler.h"

namespace xkp
{
  struct brace_parser
    {
      void parse(const str& what, xss_visitor* visitor);
    };
}

#endif