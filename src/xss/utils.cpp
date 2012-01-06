
#include "xss/utils.h"

using namespace xkp;

bool is_multi_line(const str& s)
  {
    return s.find('\n') != str::npos;
  }
