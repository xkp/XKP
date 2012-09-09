
#ifndef XSS_CSHARP_HH
#define XSS_CSHARP_HH

#include "js.h"

namespace xkp{

struct cs_lang : public js_lang //td: !!! base_lang
  {
    virtual XSSContext create_context();
  };

}

#endif