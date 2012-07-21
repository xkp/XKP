
#ifndef VM_DSL_SHELL_HH
#define VM_DSL_SHELL_HH

#include "vm_base.h"

namespace xkp {
  
  class vm_shell : public vm_dsl
    {
      public:
        vm_shell(XSSCompiler compiler) : vm_dsl(compiler) {}
      protected:
		    virtual DSLWorker create_worker(dsl& info, code_linker& owner, std::vector<str>& in, std::vector<str>& out);
    };
}

#endif