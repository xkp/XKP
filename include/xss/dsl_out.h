
#ifndef DSL_OUT_HH
#define DSL_OUT_HH

#include "xss/project.h"
#include <xs/linker.h>

namespace xkp {
  
  class out_linker : public dsl_linker
    {
      public:
        out_linker(XSSProject project) : project_(project) {}
      public:
        virtual void link(dsl& info, code_linker& owner);
      private:
        XSSProject project_;
    };
  
}

#endif