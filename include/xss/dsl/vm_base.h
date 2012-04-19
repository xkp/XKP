
#ifndef VM_DSL_BASE_HH
#define VM_DSL_BASE_HH

#include <xss/xss_compiler.h>
#include <xs/linker.h>

namespace xkp {
  
  struct IWorker
    {
      virtual void work(const param_list& args) = 0;
    };

  typedef reference<IWorker> DSLWorker;
  
  class vm_dsl : public dsl_linker
    {
      public:
        vm_dsl(XSSCompiler compiler) : compiler_(compiler) {}
      public:
        virtual void link(dsl& info, code_linker& owner);
      protected:
        XSSCompiler compiler_;

		    virtual DSLWorker create_worker(dsl& info, code_linker& owner, std::vector<str>& expressions) = 0;
    };
}

#endif