
#include <xs/vm.h>
#include <xs/runtime_error.h>

#include <types.h>
#include <dynamic_objects.h>

using namespace xkp;

const str SRuntime("vm");

const str SCallingNull("Trying to access null object");
const str STypeDoesNotInstantiate("Cannot instantiate type");
const str SCannotResolveDynamically("Cannot resolve");
const str SCannotAssignDynamically("Cannot assign resolve");
const str SNotExecutableDynamic("Attempting to call a non-function");
const str SCannotResolveOperator("Cannot resolve operator");

//td: un duplicate
const char* vm_operator_name[] =
  {
    "++",   //op_inc,
    "--",   //op_dec,
    "&",    //op_ref,
    "+",    //op_unary_plus,
    "-",    //op_unary_minus,
    "!",    //op_not,
    "*",    //op_mult,
    "/",    //op_divide,
    "%",    //op_mod,
    "as",   //op_typecast,
    "is",   //op_typecheck,
    "has",  //op_namecheck,
    "+",    //op_plus,
    "-",    //op_minus,
    ">>",   //op_shift_right,
    "<<",   //op_shift_left,
    ">>=",  //op_shift_right_equal,
    "<<=",  //op_shift_left_equal,
    "==",   //op_equal,
    "!=",   //op_notequal,
    ">",    //op_gt,
    "<",    //op_lt,
    ">=",   //op_ge,
    "<=",   //op_le,
    "&&",   //op_and,
    "||",   //op_or,
    "=",    //op_assign,
    "+=",   //op_plus_equal,
    "-=",   //op_minus_equal,
    "*=",   //op_mult_equal,
    "/=",   //op_div_equal,
    ".",    //op_dot,
		".",		//op_dot_call
    "[]",   //op_index,
    "",     //op_call,
    "",     //op_func_call
    "",     //op_array,
    "",     //op_parameter
    "",     //op_parameter_name
  };

operator_type vm_native_op[] =
  {
		op_not, //op_inc,
    op_not, //op_dec,
    op_not, //op_ref,
    op_not, //op_unary_plus,
    op_not, //op_unary_minus,
    op_not, //op_not,
    op_not, //op_mult,
    op_not, //op_divide,
    op_not, //op_mod,
    op_not, //op_typecast,
    op_not, //op_typecheck,
    op_not, //op_namecheck,
    op_not, //op_plus,
    op_not, //op_minus,
    op_not, //op_shift_right,
    op_not, //op_shift_left,
    op_shift_right, //op_shift_right_equal,
    op_shift_left,	//op_shift_left_equal,
    op_not, //op_equal,
    op_not, //op_notequal,
    op_not, //op_gt,
    op_not, //op_lt,
    op_not, //op_ge,
    op_not, //op_le,
    op_not, //op_and,
    op_not, //op_or,
    op_not, //op_assign,
    op_plus,		//op_plus_equal,
    op_minus,		//op_minus_equal,
    op_mult,		//op_mult_equal,
    op_divide,	//op_div_equal,
    op_not, //op_dot,
    op_not, //op_dot_call
    op_not, //op_index,
    op_not, //op_call,
    op_not, //op_func_call
    op_not, //op_array,
    op_not, //op_parameter
    op_not, //op_parameter_name
  };

//execution_context
execution_context::execution_context(ByteCode code, variant _this, param_list* args, fs::path file):
  code_(code),
  instructions_(code->instructions),
  constants_(code->constants),
  pc_(0),
  jump_(false),
  this_(_this),
  file_(file)
  {
    stack_.resize(1024); //td: arbitrary, inefficient
    if (args)
      {
        //bind parameters
        //td: type checking, named parameters, etc
        size_t sz = args->size();
        for(size_t i = 0; i < sz; i++)
          {
            stack_[i] = args->get(i);
          }
      }
  }

fs::path execution_context::file()
  {
    return code_->file;
  }

bool var2bool(variant v) //td: generalize
  {
    if (v.is<bool>())
      return v;

    try 
      {
        DynamicObject obj = v;
        return obj? true : false;
      }
    catch(type_mismatch)
      {
        //not an object
      }

    return variant_cast<bool>(v, false);
  }

variant execution_context::execute()
  {
    callstack_helper helper(this);

    size_t isz = instructions_.size();

    while(pc_ < isz)
      {
        instruction& i = instructions_[pc_];
        //td: a fuction table would be better, no?
        switch(i.id)
          {
            case i_jump:
              {
                pc_   = i.data.value;
                jump_ = true;
                break;
              }

            case i_jump_if:
              {
                bool control = var2bool(pop());
                if (control)
                  {
                    pc_   = i.data.value;
                    jump_ = true;
                  }
                break;
              }

            case i_jump_if_not:
              {
                bool control = var2bool(pop());
                if (!control)
                  {
                    pc_   = i.data.value;
                    jump_ = true;
                  }
                break;
              }

            case i_store:
              {
                stack_[i.data.value] = pop();
                break;
              }

            case i_load:
              {
                operands_.push(stack_[i.data.value]);
                break;
              }

            case i_load_constant:
              {
                operands_.push(constants_[i.data.value]);
                break;
              }

            case i_return:       return variant();
            case i_return_value: return operands_.top();

            case i_dup_top:
              {
                variant top = operands_.top();
                operands_.push( top );
                break;
              }
            case i_pop: operands_.pop(); break;

            case i_binary_operator:
              {
                operator_exec* e      = operators_.get_operator(i.data.value);
                variant        arg2   = pop();
                variant        arg1   = pop();
                variant        result = e->exec(arg1, arg2);

                operands_.push(result);
                break;
              }
            case i_unary_operator:
              {
                operator_exec* e      = operators_.get_operator(i.data.value);
                variant        arg1   = pop();
                variant        useless;
                variant        result = e->exec(arg1, useless);

                operands_.push(result);
                break;
              }
            case i_dynamic_binary_operator:
              {
                variant arg2  = pop();
                variant arg1  = pop();
                schema* type1 = true_type(arg1); assert(type1);
                schema* type2 = true_type(arg2); assert(type2);

                variant        result;
                operator_type  opid   = (operator_type)i.data.value;
                operator_exec* opexec = operators_.get_operator(opid, type1, type2);
                if (!opexec)
                  {
                    schema_item custom_operator;
                    if (type1->resolve(vm_operator_name[opid], custom_operator))
                      {
                        assert(custom_operator.exec); //read only

                        void*   caller_id;
                        if (custom_operator.flags & DYNAMIC_ACCESS)
                          {
                            IDynamicObject* obj = arg1; //td: catch type mismatch
                            caller_id = obj;
                          }
                        else
                          caller_id = arg1.get_pointer();

                        param_list args;
                        args.add(arg2);
                        result = custom_operator.exec->exec(caller_id, args );
                      }
                    else
                      {
												if (operators_.get_default_operator(opid, &opexec, null))
													{
														result = opexec->exec(arg1, arg2);
													}
												else
												{
													param_list error;
													error.add("id", SRuntime);
													error.add("desc", SCannotResolveOperator);
													error.add("operator", str(vm_operator_name[opid]));
													runtime_throw(error);
												}
                      }
                  }
                else result = opexec->exec(arg1, arg2);

                operands_.push(result);
                break;
              }
						case i_dynamic_binary_assign_operator:
							{
                bool		assign = false;
								variant arg1	 = pop();
                variant arg2   = pop();
                schema* type1  = true_type(arg1); assert(type1);
                schema* type2  = true_type(arg2); assert(type2);

                variant        result;
                operator_type  opid   = (operator_type)i.data.value;
                operator_exec* opexec = operators_.get_operator(opid, type1, type2);
                if (!opexec)
                  {
                    schema_item custom_operator;
                    if (type1->resolve(vm_operator_name[opid], custom_operator))
                      {
                        assert(custom_operator.exec); //read only

                        void*   caller_id;
                        if (custom_operator.flags & DYNAMIC_ACCESS)
                          {
                            IDynamicObject* obj = arg1; //td: catch type mismatch
                            caller_id = obj;
                          }
                        else
                          caller_id = arg1.get_pointer();

                        param_list args;
                        args.add(arg2);
                        result = custom_operator.exec->exec(caller_id, args );
                      }
										else
											{
												operator_exec* nopexec = operators_.get_operator(vm_native_op[opid], type1, type2);
												if (nopexec)
													{
														result = nopexec->exec(arg1, arg2);
														assign = true;
													}
												else
													{
														param_list error;
														error.add("id", SRuntime);
														error.add("desc", SCannotResolveOperator);
														error.add("operator", str(vm_operator_name[opid]));
														runtime_throw(error);
													}
											}
                  }
                else result = opexec->exec(arg1, arg2);

                operands_.push(result);
                operands_.push(!assign);
								break;
							}
            case i_dynamic_unary_operator:
              {
                variant arg1  = pop();
                schema* type1 = true_type(arg1); assert(type1);

                variant        result;
                operator_type  opid   = (operator_type)i.data.value;
                operator_exec* opexec = operators_.get_operator(opid, type1, null);
                if (!opexec)
                  {
                    schema_item custom_operator;
                    if (type1->resolve(vm_operator_name[opid], custom_operator))
                      {
                        assert(custom_operator.exec); //read only

                        void*   caller_id;
                        if (custom_operator.flags & DYNAMIC_ACCESS)
                          {
                            IDynamicObject* obj = arg1; //td: catch type mismatch
                            caller_id = obj;
                          }
                        else
                          caller_id = arg1.get_pointer();

                        param_list args;
                        result = custom_operator.exec->exec(caller_id, args );
                      }
                    else
                      {
												param_list error;
												error.add("id", SRuntime);
												error.add("desc", SCannotResolveOperator);
												error.add("operator", str(vm_operator_name[opid]));
												runtime_throw(error);
                      }
                  }
                else
                  {
                    variant arg2;
                    result = opexec->exec(arg1, arg2);
                  }

                operands_.push(result);
                break;
              }
            case i_dynamic_get:
              {
                str     getter_name = constants_[i.data.value];
                variant getted      = pop();

                variant result;
                if (!dynamic_try_get(getted, getter_name, result))
                  {
                    param_list error;
                    error.add("id", SRuntime);
                    error.add("desc", SCannotResolveDynamically);
                    error.add("object", getted);
                    error.add("property", getter_name);
                    runtime_throw(error);
                  }

                operands_.push(result);
                break;
              }
						case i_dynamic_set:
							{
                str     getter_name = constants_[i.data.value];
                variant getted      = pop();
								variant value			  = pop();

                variant result;
                if (!dynamic_set(getted, getter_name, value))
                  {
                    param_list error;
                    error.add("id", SRuntime);
                    error.add("desc", SCannotAssignDynamically);
                    error.add("object", getted);
                    error.add("property", getter_name);
                    runtime_throw(error);
                  }

                operands_.push(result);
								break;
							}
            case i_dynamic_resolve:
              {
                str     resolve_name  = constants_[i.data.value];
                variant resolver      = operands_.top();
                schema* resolver_type = true_type(resolver);

                schema_item itm;
                if (resolver_type->resolve(resolve_name, itm))
                  {
                    operands_.push(itm.exec); //td: not sure this should be done exclusively for executers
                  }
                else
                  {
                    IDynamicObject* obj = variant_cast<IDynamicObject*>(resolver, null);
                    if (obj && obj->resolve(resolve_name, itm))
                      {
                        if (!itm.exec)
                          {
                            param_list error;
                            error.add("id", SRuntime);
                            error.add("desc", SNotExecutableDynamic);
                            error.add("object", obj);
                            error.add("method", resolve_name);
                            runtime_throw(error);
                          }
                      
                        operands_.push(itm.exec);
                      }
                    else
                      {
                        param_list error;
                        error.add("id", SRuntime);
                        error.add("desc", SCannotResolveDynamically);
                        error.add("object", obj);
                        error.add("method", resolve_name);
                        runtime_throw(error);
                      }
                  }
                break;
              }
            case i_load_this:
              {
                operands_.push( this_ );
                break;
              }
            case i_call:
              {
                Executer call = pop();
                param_list pl;
                variant caller;

								if (i.data.call_data.invert)
								  caller = pop();

                if (i.data.call_data.named_params)
                  {
										for(int p = 0; p < i.data.call_data.param_count; p++)
											{
												str param_name = variant_cast<str>(pop(), str()); 
                        if (!param_name.empty())
                          pl.add(param_name, pop());
                        else
                          pl.add(pop());
											}
                  }
                else
                  {
										for(int p = 0; p < i.data.call_data.param_count; p++)
											{
												pl.add(pop());
											}
									}

								if (!i.data.call_data.invert)
								  caller = pop();

                if (caller.empty())
                  {
                    param_list error;
                    error.add("id", SRuntime);
                    error.add("desc", SCallingNull);
                    runtime_throw(error);
                  }

                void*   caller_id;
                if (i.data.call_data.is_dynamic)
                  {
                    IDynamicObject* obj = variant_cast<IDynamicObject*>(caller, null);
                    if (obj)
                      caller_id = obj;
                    else
                      caller_id = caller.get_pointer(); //revert to undynamic, there should be a better solution
                  }
                else
                  caller_id = caller.get_pointer();

                if (!caller_id)
                  {
										param_list error;
										error.add("id", SRuntime);
										error.add("desc", SCallingNull);
										runtime_throw(error);
                  }

                variant result = call->exec(caller_id, pl);
                operands_.push( result );
                break;
              }
            case i_this_call:
              {
                Executer call = pop();
                param_list pl;

                if (i.data.call_data.named_params)
                  {
										for(int p = 0; p < i.data.call_data.param_count; p++)
											{
												str param_name = variant_cast<str>(pop(), str()); 
                        if (!param_name.empty())
                          pl.add(param_name, pop());
                        else
                          pl.add(pop());
											}
                  }
                else
                  {
                    for(int p = 0; p < i.data.call_data.param_count; p++)
                      {
                        pl.add(pop());
                      }
                  }

                void* caller_id;
                if (i.data.call_data.is_dynamic)
                  {
                    IDynamicObject* obj = this_; //td: catch type mismatch
                    caller_id           = obj;
                  }
                else
                  caller_id = this_.get_pointer();

                operands_.push( call->exec(caller_id, pl) );
                break;
              }
            case i_get:
              {
                Getter  call   = pop();
                variant caller = pop();

                IDynamicObject* ooobbjjj = variant_cast<IDynamicObject*>(caller, null);

                if (caller.empty())
                  {
                    param_list error;
                    error.add("id", SRuntime);
                    error.add("desc", SCallingNull);
                    runtime_throw(error);
                  }

                void* caller_id;
                if (i.data.value)
                  {
                    IDynamicObject* obj = caller; //td: catch type mismatch
                    caller_id           = obj;
                  }
                else
                  caller_id = caller.get_pointer();

                if (!caller_id)
                  {
										param_list error;
										error.add("id", SRuntime);
										error.add("desc", SCallingNull);
										runtime_throw(error);
                  }

                variant result = call->get(caller_id);
                operands_.push(result);
                break;
              }
            case i_set:
              {
                Setter call = pop();
                
								variant value;
                variant caller;
								if (i.data.set_data.invert)
									{
										caller = pop();
										value  = pop();
									}
								else
									{
										value  = pop();
										caller = pop();
									}

                void* caller_id;
								if (i.data.set_data.is_dynamic)
                  {
                    IDynamicObject* obj = caller; //td: catch type mismatch
                    caller_id           = obj;
                  }
                else
                  caller_id = caller.get_pointer();

                if (!caller_id)
                  {
										param_list error;
										error.add("id", SRuntime);
										error.add("desc", SCallingNull);
										runtime_throw(error);
                  }

                call->set(caller_id, value);
                break;
              }
            case i_instantiate:
              {
                schema* type = pop();
                param_list pl;
                for(int p = 0; p < i.data.value; p++)
                  {
                    pl.add(pop());
                  }

                variant result;
                if (type->create(result, &pl))
                  operands_.push(result);
                else
                  {
                    param_list error;
                    error.add("id", SRuntime);
                    error.add("desc", STypeDoesNotInstantiate);
                    runtime_throw(error);
                  }
                break;
              }
            case i_dispatch:
              {
                event_info      ev     = pop();
                IDynamicObject* caller = pop();

                param_list pl;
                for(int p = 0; p < i.data.value; p++)
                  {
                    pl.add(pop());
                  }

                caller->dispatch_event(ev.id, pl);
                break;
              }
						case i_nop: break;
            default:
              assert(false); //say wha
          }

        if (!jump_)
          pc_++;

        jump_ = false;
      }

    return variant();
  }

void execution_context::push(variant v)
  {
    //td: optimize, pushes and pops ought to be faster
    operands_.push(v);
  }

variant execution_context::pop()
  {
    if (operands_.empty())
      return variant();

    variant r = operands_.top();
    operands_.pop();
    return r;
  }

//code_setter
code_setter::code_setter(ByteCode _code, int var_idx):
  code_(_code),
  idx_(var_idx)
  {
  }

void code_setter::set( void* instance, const variant value )
  {
    IDynamicObject* self = static_cast<IDynamicObject*>(instance);

    if (idx_ >= 0)
      {
        //do the value assigment if there is an anonymous slot for it
        self->set_anonymous(idx_, value);
      }

    //td: pass the previous value
    execution_context e(code_, self);
    e.execute();
  }

//code_getter
code_getter::code_getter(ByteCode _code):
  code_(_code)
  {
  }

variant code_getter::get(void* instance)
  {
    IDynamicObject* self = static_cast<IDynamicObject*>(instance);
    execution_context e(code_, self);
    return e.execute();
  }

//code_executer
code_executer::code_executer(ByteCode _code):
  code_(_code)
  {
  }

variant code_executer::exec(void* instance, const param_list args)
  {
    //we must obtain the actual type of instance, not IDynamicObject 
    //but any class on top of it
    IDynamicObject* d = static_cast<IDynamicObject*>(instance);
    schema* type = d->get_type();
    variant this_; 
    type->cast(d, type, this_);

    execution_context e(code_, this_, const_cast<param_list*>(&args));
    return e.execute();
  }

//vm
vm& vm::instance()
  {
    static vm inst_;
    return inst_;
  }
      
void vm::enter(execution_context* ctx)
  {
    call_stack_.push(ctx);
  }

void vm::leave()
  {
    call_stack_.pop();
  }

void vm::reset()
  {
    while(!call_stack_.empty())
      call_stack_.pop();
  }

call_stack& vm::state()
  {
    return call_stack_;
  }

fs::path vm::file()
  {
    if (call_stack_.empty())
      return fs::path();

    execution_context* top = call_stack_.top();
    return top->file();
  }

