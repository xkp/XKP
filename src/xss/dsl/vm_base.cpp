
#include <xss/dsl/vm_base.h>

using namespace xkp;

struct myworker
  {
    myworker(): w_(DSLWorker()) {}
    myworker(DSLWorker w): w_(w) {}

    virtual void work(const param_list& args)
      {
        if (w_)
          w_->work(args);
      }

    private:
      DSLWorker w_;
  };

typedef reference<myworker> MyWorker;

struct myworker_schema : object_schema<myworker>
  {
    virtual void declare()
      {
        dynamic_method_( "work", &myworker::work );
      }
  };

register_complete_type(myworker, myworker_schema);

//vm_dsl
void vm_dsl::link(dsl& info, code_linker& owner)
  {
    std::vector<str> expressions;

		DSLWorker iwrk = create_worker(info, owner, expressions);
    MyWorker    wrk(new myworker(iwrk)); 
    owner.add_instruction(i_load_constant, owner.add_constant(wrk));

    //push the expression to be used as parameters, push 'em in reverse order
    std::vector<str>::reverse_iterator it = expressions.rbegin();
    std::vector<str>::reverse_iterator nd = expressions.rend();
    xs_utils xs;
    for(; it != nd; it++)
      {
        expression expr;
        if (!xs.compile_expression(*it, expr))
          assert(false);

        owner.link_expression(expr);
      }

    //for this call, remember the indent is always the forat parameter
    owner.add_call(wrk, "work", expressions.size(), false);
  }
