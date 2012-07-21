
#include <xss/dsl/vm_base.h>

using namespace xkp;

int vm_dsl::counter = 0;

struct myworker
  {
    myworker(): w_(DSLWorker()) {}
    myworker(DSLWorker w): w_(w) {}

    virtual variant work(const param_list& args)
      {
        if (w_)
          return w_->work(args);
        return variant();
      }

    private:
      DSLWorker w_;
  };

typedef reference<myworker> MyWorker;

struct myworker_schema : object_schema<myworker>
  {
    virtual void declare()
      {
        dynamic_function_<variant>( "work", &myworker::work );
      }
  };

register_complete_type(myworker, myworker_schema);

//vm_dsl
void vm_dsl::link(dsl& info, code_linker& owner)
  {
    std::vector<str> expressions;
    std::vector<str> out;

		DSLWorker iwrk = create_worker(info, owner, expressions, out);
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

    //execute the call
    owner.add_call(wrk, "work", expressions.size(), false);

    //grab the output
    if (!out.empty())
      {
        str var_name = "__dsl_res" + boost::lexical_cast<str>(counter++);
        int var_idx  = owner.register_variable(var_name, null);
        owner.add_instruction(i_store, var_idx);

        std::vector<str>::iterator oit = out.begin();
        std::vector<str>::iterator ond = out.end();
        for(; oit != ond; oit++)
          {
            str out_name = *oit;
            str expr_str = out_name + " = " + var_name + "." + out_name;

            expression expr;
            xs.compile_expression(expr_str, expr);

            owner.link_expression(expr);
          }
      }
  }
