
#include <schema.h>
#include <xs/linker.h>
#include <xs/vm.h>
#include <xs/behaviour.h>
#include <xs/array.h>
#include <xs/xs_error.h>

using namespace xkp;

//strings
const str SLinker("linker");
const str SOutOfContext("not-sure");
const str SUnknownIdentifier("cannot-resolve");
const str STypeMismatch("type-mismatch");
const str SDuplicate("dup");
const str SAssignError("assign");

const str SCannotResolve("Unknown identifier");
const str STypedArrayMustContainTheSameType("Adding an element to an array that does not match the type");
const str SCannotResolveType("Unknown type");
const str SCannotResolveCall("Unknown identifier");
const str STryingToCallANonCollable("Trying to call something it shouldnt be called");
const str SAssigningMismatchedLocalVariable("Incompatible types assigning a local variable");
const str SLocalVariableAlreadyDeclared("Local variable already declared");
const str SAccesingThisOnAThislessCode("There isn't a 'this' on this context");
const str SCannotAssign("Cannot assign");
const str SAssigningToWriteOnly("Cannot assign, write only");
const str SAssigningToConstant("Cannot assign, constant");
const str SAlteringInmutableObject("Cannot enhance an inmutable object");
const str SInstanceNotFound("Instance not found");
const str STypeNotFound("Type not found");
const str SBehaviourNotFound("Behaviour not found");
const str STypeIsNotABehaviour("Expecting behaviour");
const str SSuperclassNotFound("Superclass not found");
const str SCannotResolveOperator("Cannot resolve operator");
const str SUsingControlOutsideALoop("Using control statements (break, continue) outside a loop");

const char* operator_name[] =
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
    ".",    //op_dot_call
    "[]",   //op_index,
    "",     //op_call,
    "",     //op_func_call
    "",     //op_array,
    "",     //op_parameter
    "",     //op_parameter_name
    "new",  //op_instantiate
    "{}",   //op_object
  };

//just a very bad name
operator_type native_op[] =
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
    op_not, //op_instantiate
    op_not, //op_object
  };

struct array_evaluator : expr_evaluator
	{
		array_evaluator(): result( new dynamic_array ) {}

    virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top)
			{
				assert(!top || op == op_array);
				switch(op)
					{
						case op_array:
							{
								int count = stack_.top(); stack_.pop();
								assert(result->size() == count);
								assert(stack_.size() == 0);
								stack_.push(result);
								break;
							}
						case op_parameter:
							{
								assert(stack_.size() == 2);
                stack_.pop();
								variant val = stack_.top(); stack_.pop();
								result->insert(val);
								break;
							}
						default:
							expr_evaluator::exec_operator(op, pop_count, push_count, top);
					}
			}

		DynamicArray result;
	};


//code_linker
code_linker::code_linker():
  local_count_(0),
  pc_(0),
	resolving_assigner_(false)
  {
    context_.types_ = &default_types_;
  }

code_linker::code_linker(code_context& context):
  local_count_(0),
  pc_(0),
  context_(context),
	resolving_assigner_(false)
  {
    context_.this_ = context.this_;
    if (!context_.types_)
      context_.types_ = &default_types_;

    param_list* args = context_.args_;
    if (args && !args->empty())
      {
        size_t sz = args->size();
        for(size_t i = 0; i < sz; i++)
          {
            str     name  = args->get_name(i);
            variant value = args->get(i);

            int local_idx = local_count_++;

						schema* arg_type = null;
						if (!value.empty())
							{
								arg_type = value.get_schema();
							}

            local_variable lv(local_idx, arg_type);
            locals_.insert( locals_pair(name, lv) );
          }
      }

    if (context_.dsl_)
      {
        dsl_list::iterator it = context_.dsl_->begin();
        dsl_list::iterator nd = context_.dsl_->end();

        for(; it != nd; ++it)
          {
            dsl_linkers_.insert(dsl_linker_pair(it->first, it->second));
          }
      }
  }

ByteCode code_linker::link(fs::path file)
  {
    return ByteCode( new byte_code(code_, constants_, file) );
  }

void code_linker::link(ByteCode result)
  {
    return link(result.get());
  }

void code_linker::link(byte_code* result)
  {
    result->instructions = code_;
    result->constants    = constants_;
  }

code_context& code_linker::context()
  {
    return context_;
  }

void code_linker::if_(stmt_if& info)
  {
    link_expression(info.expr);
    int if_jump = add_instruction( i_jump_if_not );

    link_code(info.if_code, false);

    if (info.else_code.empty())
      {
        instruction_data( if_jump, pc_ );
      }
    else
      {
        //skip else code
        int afterif_jump = add_instruction( i_jump );
        instruction_data( if_jump, pc_ );

        link_code(info.else_code, false);

        instruction_data( afterif_jump, pc_ );
      }
  }

void code_linker::variable_(stmt_variable& info)
  {
    register_variable( info );
  }

void code_linker::for_(stmt_for& info)
  {
    //link initialization
    if (!info.init_variable.empty())
      register_variable(info.init_variable);
    else if (!info.init_expr.empty())
      link_expression(info.init_expr);

    //link condition
    int exit_jump = 0;
    int loop_jump = pc_;

    if (!info.cond_expr.empty())
      {
        link_expression(info.cond_expr);
        exit_jump = add_instruction( i_jump_if_not );
      }

    //link code
    link_code(info.for_code, true, -1, -2);

    //add a couple of jumps so breaks and continues work
    int continue_jump = add_instruction( i_jump, pc_ + 2 );
    int break_jump = add_instruction( i_jump );

		//link iterator

		link_expression(info.iter_expr);
    add_instruction( i_jump, loop_jump);

    if (exit_jump > 0)
      instruction_data( exit_jump, pc_ );

		if (break_jump > 0)
      instruction_data( break_jump, pc_ );
  }

void code_linker::iterfor_(stmt_iter_for& info)
  {
    schema* value_type    = typeof_(info.type);
    schema* iterable_type = link_expression(info.iter_expr);

    //td !!! type checking, correct iterator mechanism
		//if (iterable_type)
    //  {
    //    //grab the iterated type
    //    schema_item itm;
    //    schema* array_type = null;
    //    if (iterable_type->resolve("iterated_type", itm) && itm.get)
    //      array_type = itm.get->get( iterable_type ); //expects the iterated_type to be a class property
    //    else
    //      assert(false); //error, invalid iterable

    //    if (array_type != value_type)
    //      {
    //        if (!value_type)
    //          value_type = array_type;
    //        else
    //          assert(false); //type mismatch
    //      }
    //  }

    int value_ = register_variable(info.id, value_type);

    //now on top of the stack lies an iterable,
    //as you can see there is a contract that closely
    //resembles the std interface

    //must have it twice in the stack (to grab begin and end)
    add_instruction(i_dup_top);

    //generate local variables for iterators & value
    schema* iterator_type = add_stack_lookup("begin", iterable_type);
    int     iterator_     = register_variable("iterator@" + info.id, iterator_type);
    add_instruction(i_store, iterator_);

    schema* end_type = add_stack_lookup("end", iterable_type); assert(end_type == iterator_type);
    int     end_     = register_variable("end_iterator@" + info.id, end_type);
    add_instruction(i_store, end_);

    int loop_jump = pc_;

    //td: this is sort of slow, begin and end should be on top of the stack
    expression test_iterator;
    test_iterator.push_operand( local_variable(iterator_, iterator_type) );
    test_iterator.push_operand( local_variable(end_, end_type) );
    test_iterator.push_operator(op_equal);

    link_expression(test_iterator);

    int exit_jump = add_instruction( i_jump_if );

    //update variable
    expression update_variable;
    update_variable.push_operand   ( local_variable(iterator_, iterator_type) );
    update_variable.push_identifier( "value" );
    update_variable.push_operator  ( op_dot );

    link_expression(update_variable);

    add_instruction(i_store, value_);

    //then the code
    link_code(info.for_code, true, -1, -2);

    //add a couple of jumps so breaks and continues work
    int continue_jump = add_instruction( i_jump, pc_ + 2 );
    int break_jump = add_instruction( i_jump );

		//advance
    expression advance_iterator;
    advance_iterator.push_operand( local_variable(iterator_, iterator_type) );
    advance_iterator.push_operator( op_inc );

    link_expression(advance_iterator);

    add_instruction(i_pop);
    add_instruction(i_pop);
    add_instruction( i_jump, loop_jump);
    instruction_data( exit_jump, pc_ );
    instruction_data( break_jump, pc_ );
}

void code_linker::while_(stmt_while& info)
  {
    int loop_jump = pc_;
    link_expression(info.expr);
    int exit_jump = add_instruction( i_jump_if_not );

    link_code(info.while_code, true, -1, -2);

    add_instruction( i_jump, loop_jump);
    instruction_data( exit_jump, pc_ );
  }

void code_linker::break_()
  {
    add_fixup(add_instruction( i_jump ), fixup_break);
  }

void code_linker::continue_()
  {
    add_fixup(add_instruction( i_jump ), fixup_continue);
  }

void code_linker::return_(stmt_return& info)
  {
    if (info.expr.empty())
      add_instruction( i_return );
    else
      {
        link_expression(info.expr);
        add_instruction( i_return_value );
      }
  }

void code_linker::expression_(stmt_expression& info)
  {
    //after expressions are evaluated as staments
    //we should not push the result into the operand list
    bool empty_stack = true;
    link_expression(info.expr, false, &empty_stack);

    if (!empty_stack)
      add_instruction(i_pop);
  }

void code_linker::dsl_(dsl& info)
  {
    dsl_linker_list::iterator it = dsl_linkers_.find(info.name);
    if (it != dsl_linkers_.end())
      {
        it->second->link(info, *this);
      }
  }

void code_linker::dispatch(stmt_dispatch& info)
  {
    //do parameters
    link_expression(info.args);

    //extract the caller and event id
    std::vector<str> id = info.target;
    str ev_name = id.back();
    id.erase(id.end() - 1);

    expression caller;
    std::vector<str>::iterator it = id.begin();
    std::vector<str>::iterator nd = id.end();
    bool first = true;
    for(; it != nd; it++)
      {
        if (first)
          {
            caller.push_identifier( *it );
            first = false;
          }
        else
          {
            caller.push_operator( op_dot );
            caller.push_identifier( *it );
          }
      }

    schema* caller_type = null;
    if (!caller.empty())
      caller_type = link_expression(caller);
    else
      {
        caller_type = context_.this_type;
        add_instruction(i_load_this);
      }

    //we must keep the caller in the stack
    add_instruction(i_dup_top);

    //now resolve the actual event name
    expression event_expr;
    event_expr.push_operand( already_in_stack(caller_type) );
    event_expr.push_identifier( ev_name );
    event_expr.push_operator( op_dot );
    link_expression(event_expr);

    add_instruction(i_dispatch, info.arg_count);
  }

void code_linker::switch_(stmt_switch& info)
  {
    assert(false);
  }

void code_linker::try_(stmt_try& info)
  {
    assert(false);
  }

void code_linker::throw_(stmt_throw& info)
  {
    assert(false);
  }

void code_linker::push(variant operand, bool top)
  {
    if (operand.is<expression_identifier>() ||
        operand.is<already_in_stack>()      ||
        operand.is<local_variable>()        )
      {
        stack_.push(operand);
      }
    else
      {
        //td: !!! stuff should be added (on code) as they come, not when the operators get popped
        constant c;
        c.value = operand;
        stack_.push(c);
      }
  }

void code_linker::exec_operator(operator_type op, int pop_count, int push_count, bool top)
  {
    variant arg1, arg2;
    switch(pop_count)
      {
        case 0: break;
        case 1: arg1 = stack_.top(); stack_.pop(); break;
        case 2:
          {
            arg2 = stack_.top(); stack_.pop();
						if (arg2.is<already_in_stack>())
							{
								//the second argument being on the stack already causes the order of the parameters for the call to invert
								//one way to solve is to create a temporary variable and make sure it gets loaded later
								already_in_stack ais = arg2;
								int lv = register_variable("", ais.type, null);
								add_instruction(i_store, lv);

								arg2 = local_variable(lv, ais.type); //note the switcheroo
							}

            arg1 = stack_.top(); stack_.pop();
            break;
          }
        default: assert(false);
      }

    switch(op)
      {
        case op_dec:
        case op_inc:
          {
            resolve_value(arg1);
            add_instruction( i_dup_top );

            bool dont_assign = false;
            resolve_unary_operator( op, arg1, &dont_assign);

            if (!dont_assign)
              resolve_assign(arg1);
            break;
          }

        case op_unary_plus:
        case op_unary_minus:
        case op_not:
          {
            resolve_unary_operator( op, arg1, null );
            break;
          }

        case op_mult:
        case op_divide:
        case op_mod:
        case op_typecast:
        case op_typecheck:
        case op_namecheck:
        case op_plus:
        case op_minus:
        case op_shift_right:
        case op_shift_left:
        case op_equal:
        case op_notequal:
        case op_gt:
        case op_lt:
        case op_ge:
        case op_le:
        case op_and:
        case op_or:
          {
            resolve_operator(op, arg1, arg2, null);
            break;
          }
        case op_index:
          {
            resolve_operator(op, arg1, arg2, null);
            break;
          }
        case op_assign:
        case op_plus_equal:
        case op_minus_equal:
        case op_mult_equal:
        case op_div_equal:
        case op_shift_right_equal:
        case op_shift_left_equal:
          {
						//td: phase out
						assert(false);
            operator_type xe = op_assign;
            switch (op)
              {
                case op_plus_equal:         xe = op_plus;        break;
                case op_minus_equal:        xe = op_minus;       break;
                case op_mult_equal:         xe = op_mult;        break;
                case op_div_equal:          xe = op_divide;      break;
                case op_shift_right_equal:  xe = op_shift_right; break;
                case op_shift_left_equal:   xe = op_shift_left;  break;
              }

            bool dont_assign = false;
						if (xe != op_assign)
              {
                resolve_operator(xe, arg1, arg2, &dont_assign);
              }
            else
              resolve_value(arg2);

            if (!dont_assign)
              resolve_assign( arg1 );

            if (!stack_.empty())
              stack_.pop(); //td: !!! this line prevents (*)= operators (+=, etc) from being used
                            //as expressions (if (a -= 3 > 5)) but only as statements (a -= b)
                            //there is a problem in the operand stack (how to remove the top when
                            //such a statement is executed). I added a pop instruction, but in general
                            //I'm not too happy with the solution.
            break;
          }
        case op_dot:
          {
            schema* type = null;
            expression_identifier ei = arg2;

						resolve_value(arg1, &type);

            if (type &&
								type != type_schema<empty_type>() &&
								type != type_schema<IDynamicObject*>() &&
								type != type_schema<variant>() &&
								type != type_schema<DynamicObject>())
              {
                schema_item si;
                if (type->resolve(ei.value, si))
									{
										if (resolving_assigner_ && top)
											{
												stack_.push( si );
											}
										else
											{
												schema* type = null;
												variant vv = si;
												resolve_value(vv, &type);

												stack_.push( already_in_stack(type) );
											}
									}
                else
                  {
                    IDynamicObject* do_ = variant_cast<IDynamicObject*>(arg1, null);
                    if (do_ && do_->resolve(ei.value, si))
											{
												if (resolving_assigner_ && top)
													{
														stack_.push( si );
													}
												else
													{
														schema* type = null;
														variant vv = si;
														resolve_value(vv, &type);

														stack_.push( already_in_stack(type) );
													}
											}
										else if (type && (type->options() & TYPE_DYNAMIC) != 0)
											{
												if (resolving_assigner_ && top)
													{
												    add_instruction(i_dynamic_set, add_constant(ei.value));
													}
                        else
                          {
												    add_instruction(i_dynamic_get, add_constant(ei.value));

												    int lv = register_variable("", null, null);
												    add_instruction(i_store, lv);

												    //i've decided to save dynamic gets into the stack
												    //to avoid general unplesantness with alredy_in_stack.
												    stack_.push( local_variable(lv, null) );
                          }
											}
										else
                      {
                        param_list error;
                        error.add("id", SUnknownIdentifier);
                        error.add("desc", SCannotResolve);
                        error.add("identifier", ei.value);
                        xs_throw(error);
                      }
                  }
              }
            else
              {
								if (resolving_assigner_)
									{
										add_instruction(i_dynamic_set, add_constant(ei.value));
									}
								else
									{
										add_instruction(i_dynamic_get, add_constant(ei.value));

										int lv = register_variable("", null, null);
										add_instruction(i_store, lv);

										//i've decided to save dynamic gets into the stack
										//to avoid general unplesantness with alredy_in_stack.
										stack_.push( local_variable(lv, null) );
									}
              }
            break;
          }

        case op_dot_call:
          {
            schema* type = null;
            resolve_value(arg1, &type);

            expression_identifier ei = arg2;
						if (ei.value == "linker_breakpoint")
						{
							str xxx("set breakpoint here");
						}

            if (type && type != type_schema<empty_type>() && type != type_schema<variant>())
              {
                schema_item si;
                if (type->resolve(ei.value, si))
                  stack_.push( si );
                else
                  {
                    IDynamicObject* do_ = variant_cast<IDynamicObject*>(arg1, null);
                    if (do_ && do_->resolve(ei.value, si))
                      stack_.push( si );
                    else
                      {
                        param_list error;
                        error.add("id", SUnknownIdentifier);
                        error.add("desc", SCannotResolve);
                        error.add("identifier", ei.value);
                        xs_throw(error);
                      }
                  }
              }
            else
              {
                add_instruction(i_dynamic_resolve, add_constant(ei.value));

								int lv = register_variable("", null, null);
								add_instruction(i_store, lv);

								//i've decided to save dynamic gets into the stack
								//to avoid general unplesantness with alredy_in_stack.
								stack_.push( local_variable(lv, null) );
              }
            break;
          }

        case op_call:
          {
            constant    arg_count    = arg1;
            int         args         = arg_count.value;
            bool        named_params = has_parameter_names();

            //pop the arguments
            for(int i = 0; i < args; i++)
              {
                variant arg = stack_.top(); stack_.pop();
                resolve_value(arg);

                if (named_params)
                  {
                    str param_name = param_names_[i];
                    add_instruction(i_load_constant, add_constant(param_name));
                  }
              }

            assert(!stack_.empty());
            variant call_value = stack_.top(); stack_.pop();

            bool    is_dyamic = false;
            schema* result_type = null;
            if (call_value.is<schema_item>())
              {
                schema_item call = call_value;

                is_dyamic = call.flags&DYNAMIC_ACCESS;
                result_type = call.type;

                assert(call.exec);
                add_instruction(i_load_constant, add_constant(call.exec));
              }
            else if (call_value.is<already_in_stack>())
              {
                already_in_stack ais = call_value;
                assert(!ais.type); //I'm not sure why this would hapeen, I'll wait and see
                is_dyamic = true;
              }
            else if (call_value.is<local_variable>())
              {
                local_variable lv = call_value;
								add_instruction(i_load, lv.index);
                is_dyamic = true;
              }
            else
              assert(false);

            add_call(i_call, args, is_dyamic, named_params);
            stack_.push( already_in_stack(result_type) );
            break;
          }

        case op_func_call:
          {
            expression_identifier name          = arg1;
            constant              arg_count     = arg2;
            int                   args          = arg_count.value;
            bool                  named_params  = has_parameter_names();

            //simple functions can be either:
            // - instantiation
            // - calling a local (this) method
            // - global functions

            type_registry*  types     = context_.types_;
            schema*         this_type = context_.this_type;
            schema*         instantiator;
            schema_item     this_item;
            IDynamicObject* dynamic = null;
            variant         scope_call;
            if (types && (instantiator = types->get_type(name.value)))
              {
                //pop the arguments, code is repeated below to generate the proper stack
                //could be prettier
                for(int i = 0; i < args; i++)
                  {
                    variant arg = stack_.top(); stack_.pop();
                    resolve_value(arg);

                    if (named_params)
                      {
                        str param_name = param_names_[i];
                        add_instruction(i_load_constant, add_constant(param_name));
                      }
                  }

                add_instruction(i_load_constant, add_constant(instantiator));
                add_instruction(i_instantiate, args);
              }
            else if (this_type && this_type->resolve(name.value, this_item))
              {
                assert(this_item.exec);

                for(int i = 0; i < args; i++)
                  {
                    variant arg = stack_.top(); stack_.pop();
                    resolve_value(arg);

                    if (named_params)
                      {
                        str param_name = param_names_[i];
                        add_instruction(i_load_constant, add_constant(param_name));
                      }
                  }

                add_instruction(i_load_constant, add_constant(this_item.exec));
                add_call(i_this_call, args, this_item.flags & DYNAMIC_ACCESS, named_params);
                stack_.push( already_in_stack(this_item.type) );
              }
            else if (!context_.this_.empty() &&
                     (dynamic = variant_cast<IDynamicObject*>(context_.this_, null)) &&
                     dynamic->resolve(name.value, this_item))
              {
                assert(this_item.exec);
                //td: sort this out, stop repeating the argument loading code.
                for(int i = 0; i < args; i++)
                  {
                    variant arg = stack_.top(); stack_.pop();
                    resolve_value(arg);

                    if (named_params)
                      {
                        str param_name = param_names_[i];
                        add_instruction(i_load_constant, add_constant(param_name));
                      }
                  }

                add_instruction(i_load_constant, add_constant(this_item.exec));
                add_call(i_this_call, args, this_item.flags & DYNAMIC_ACCESS, named_params );
              }
            else if (context_.scope_ && context_.scope_->resolve(name.value, scope_call))
              {
                assert(scope_call.is<schema_item>());
                this_item = scope_call;
                assert(this_item.exec);

                for(int i = 0; i < args; i++)
                  {
                    variant arg = stack_.top(); stack_.pop();
                    resolve_value(arg);

                    if (named_params)
                      {
                        str param_name = param_names_[i];
                        add_instruction(i_load_constant, add_constant(param_name));
                      }
                  }

                add_instruction(i_load_constant, add_constant(this_item.exec));
                add_call(i_this_call, args, this_item.flags & DYNAMIC_ACCESS, named_params );
              }
            else
              {
                //td: globals
                param_list error;
                error.add("id", SUnknownIdentifier);
                error.add("desc", SCannotResolve);
                error.add("identifier", name.value);
                xs_throw(error);
              }
            break;
          }

        case op_parameter:
          {
            expression_identifier ei = arg1;
            param_names_.insert(param_names_.begin(), ei.value);

            //this is neccessary to avoid hanging members (a.b) in a parameter list
            //what happens is the schema item for the getter stays in the stack waiting
            //to be a call. So if we receive this one we're good to call the getter
            if (!stack_.empty())
              {
                variant arg = stack_.top();
                if (arg.is<schema_item>())
                  {
                    schema* type = null;
                    resolve_value(arg, &type);
                    stack_.pop();
                    stack_.push( already_in_stack(type) );
                  }
              }
            break;
          }

        case op_array:
          {
            constant arg_count = arg1;
            int      args      = arg_count.value;

            for(int i = 0; i < args; i++)
              {
                variant array_value = stack_.top();
                stack_.pop();

                schema* value_type = null;
                resolve_value(array_value, &value_type);

                if (array_type_ && array_type_ != value_type)
                  {
                    param_list error;
                    error.add("id", STypeMismatch);
                    error.add("desc", STypedArrayMustContainTheSameType);
                    xs_throw(error);
                  }
              }

            //grab the array type needed
            type_registry* types      = context_.types_;
            param_list     pl;
            if (array_type_)
              pl.add(array_type_);

            schema* array_type = types->get_type("array", array_type_? &pl : null );
            assert(array_type);

            add_instruction(i_load_constant, add_constant(array_type));
            add_instruction(i_instantiate, args);

						stack_.push( already_in_stack(array_type) );
            break;
          }
        default:
          assert(false); //td:
      }
  }

void code_linker::register_dsl(const str& name, DslLinker linker)
  {
    dsl_linkers_.insert( dsl_linker_pair(name, linker) );
  }

variant code_linker::evaluate_expression(expression& expr)
  {
    //td: only constants for now

		operator_type op;
		if (expr.top_operator(op) && op == op_array)
			{
				array_evaluator eval;
				expr.visit( &eval );

				return eval.result;
			}

    expr_evaluator eval;
    expr.visit( &eval );
    return eval.value();
  }

schema* code_linker::typeof_(const xs_type type)
  {
    type_registry* types  = context_.types_;
    schema*        result = null;
    if (types && !type.name.empty())
      {
        result = types->get_type(type.name);
        if (!result && type.name != "var")
          {
            param_list error;
            error.add("id", SUnknownIdentifier);
            error.add("desc", SCannotResolveType);
            error.add("type", type.name);
            xs_throw(error);
          }
      }

    return result;
  }

void code_linker::link_code(code& cde, bool track, int continue_pc, int break_pc)
  {
    fixup_list fixup_cpy;
    if (track)
      {
				loops_.push(std::pair<int, int>(continue_pc, break_pc));
        fixup_cpy = fixup_;
        fixup_.clear();
      }

    cde.visit( this );

    fixup_list::iterator it = fixup_.begin();
    fixup_list::iterator nd = fixup_.end();

    for(; it != nd; it++)
      {
        if(loops_.empty())
          {
            param_list error;
            error.add("id", SLinker);
            error.add("desc", SUsingControlOutsideALoop);
            xs_throw(error);
          }

				std::pair<int, int> info = loops_.top();

				int continue_address	= info.first >= 0? info.first : pc_ + abs(info.first) - 1;
				int break_address			= info.second >= 0? info.second : pc_ + abs(info.second) - 1;
				switch(it->dest)
          {
            case fixup_continue:
              instruction_data(it->instruction_idx, continue_address);
              break;
            case fixup_break:
              instruction_data(it->instruction_idx, break_address);
              break;
          }
      }

    if (track)
      {
        loops_.pop();
        fixup_ = fixup_cpy;
      }
  }

schema* code_linker::link_expression(expression& expr, bool assigner, bool* empty_stack, schema* array_type)
  {
    while (!stack_.empty()) stack_.pop();
    array_type_ = array_type;

		//deal with assignment
		operator_type op;
		if (!assigner && expr.top_operator(op))
			{
				bool simple_assign = false;
				switch(op)
					{
						case op_assign:
							simple_assign = true;
            case op_plus_equal:
            case op_minus_equal:
            case op_mult_equal:
            case op_div_equal:
            case op_shift_right_equal:
            case op_shift_left_equal:
							{
								expression_splitter es(op);
								expr.visit(&es);

								schema* right_type = link_expression(es.right, false);

								if (simple_assign)
									{
										link_expression(es.left, true);
									}
								else
									{
										size_t  result;
										schema* result_type;
										bool		dont_assign = false;

										schema* left_type	 = link_expression(es.left, false);
										if (operators_.get_operator_index(op, left_type, right_type, result, &result_type))
											{
												add_instruction(i_binary_operator, static_cast<short>(result));
												already_in_stack ais(result_type);
												stack_.push(ais);
												dont_assign = true;
											}
										else
											{
												if (!resolve_custom_operator(op, left_type, true, &dont_assign))
													{
														if (!resolve_custom_operator(op, right_type, true, &dont_assign))
															{
																//now that we've tried everything folk,
																//lets try the native operator (like + in +=)
																//and an assignment
																operator_type nop = native_op[op];
																if (operators_.get_operator_index(nop, left_type, right_type, result, &result_type))
																	{
																		resolve_operator(nop, already_in_stack(left_type), already_in_stack(right_type), null);

																		//the aforementioned assignment
																		link_expression(es.left, true);
																		resolve_assign(already_in_stack(left_type));
																	}
																else
																	{
																		//oh well, I tried... now we must prepare for eventualities
																		//use case: the dynamic operator to be solved could be an array +=, no assign
																		//use case: regular +=, must assign so:
																		//i_dynamic_binary_assign_operator will return whether to assign or not
																		add_instruction(i_dynamic_binary_assign_operator, static_cast<short>(op));

																		int assign_jump = add_instruction(i_jump_if);
																		link_expression(es.left, true);

																		instruction_data(assign_jump, pc_);
																	}
															}
													}
											}
									}

								return null;
							}
					}
			}

		//if not an assigment...
		assert(assigner || !resolving_assigner_);

		resolving_assigner_ = assigner;
    expr.visit(this);
		resolving_assigner_ = false;

    if (!stack_.empty())
      {
		assert(stack_.size() == 1);

        if (empty_stack) *empty_stack = false;

        variant top      = stack_.top(); stack_.pop();
        schema* top_type = null;

				if (assigner)
					resolve_assign(top, true);
				else
					resolve_value(top, &top_type);
        return top_type;
      }

    return null;
  }

int code_linker::add_instruction( instruction_type i )
  {
    instruction ii(i);
    code_.push_back(ii);
    pc_++;
    return pc_ - 1;
  }

int code_linker::add_instruction( instruction_type i, short data )
  {
    instruction ii(i, data);
    code_.push_back(ii);
    pc_++;
    return pc_ - 1;
  }

void code_linker::instruction_data( int idx, short data )
  {
    code_[idx].data.value = data;
  }

int code_linker::add_call( instruction_type i, unsigned char param_count, bool is_dynamic, bool named_params, bool invert )
  {
    assert(i == i_call || i == i_this_call);

    if (named_params && !param_names_.empty())
      assert(param_names_.size() == param_count);
      
    instruction ii(i, param_count, is_dynamic, invert);
    ii.data.call_data.named_params = named_params;
    code_.push_back(ii);
    pc_++;

    param_names_.clear();
    return pc_ - 1;
  }

int code_linker::add_set(bool is_dynamic, bool invert)
  {
		instruction ii(i_set, is_dynamic, invert);
    code_.push_back(ii);
    pc_++;
    return pc_ - 1;
  }

int code_linker::add_call(variant caller, const str& name, int param_count, bool is_dynamic, bool invert )
  {
    param_names_.clear(); //td: deal with the use case

    schema* caller_type = true_type(caller);
    if (!caller_type)
      {
        assert(false); //td: dynamic access
      }

    schema_item si;
    if (!caller_type->resolve(name, si))
      {
        param_list error;
        error.add("id", SUnknownIdentifier);
        error.add("desc", SCannotResolveCall);
        error.add("call", name);
        xs_throw(error);
      }

    if (!si.exec)
      {
        param_list error;
        error.add("id", SOutOfContext);
        error.add("desc", STryingToCallANonCollable);
        error.add("call", name);
        xs_throw(error);
      }

    add_instruction(i_load_constant, add_constant(si.exec));
		return add_call(i_call, param_count, is_dynamic, invert );
  }

int code_linker::add_anonymous_local()
  {
    stmt_variable v;
    v.id = "@" + boost::lexical_cast<str>(local_count_);
    return register_variable(v);
  }

void code_linker::add_fixup( int idx, fixup_dest dest )
  {
    fixup_.push_back( fixup_data(idx, dest) );
  }

schema* code_linker::add_stack_lookup(const str& query, schema* type)
  {
    expression expr;
    expr.push_operand   ( already_in_stack(type) );
    expr.push_identifier( query                  );
    expr.push_operator  ( op_dot                 );

    return link_expression(expr);
  }

bool code_linker::has_parameter_names()
  {
    std::vector<str>::iterator it = param_names_.begin();
    std::vector<str>::iterator nd = param_names_.end();

    for(; it != nd; it++)
      { 
        if (!it->empty())
          return true;
      }

    return false;
  }

int code_linker::register_variable(stmt_variable& info)
  {
    return register_variable(info.id, info.type, &info.value);
  }

int code_linker::register_variable(const str& name, const str& type, expression* value)
  {
    type_registry* types_   = context_.types_;
    schema*        var_type = null;
    if (types_ && !type.empty())
      {
        var_type = types_->get_type(type);

        if (!var_type && type != "var")
          {
            param_list error;
            error.add("id", SUnknownIdentifier);
            error.add("desc", SCannotResolveType);
            error.add("type", type);
            error.add("variable name", name);
            xs_throw(error);
          }

        assert(var_type || type == "var");
      }

    return register_variable(name, var_type, value);
  }

//expression visitors
//td: this is the meat of type checking, I just have some other stuff to do
/*
struct expr_typeof : expression_visitor
  {
    schema*      result;
    code_linker& linker;

    virtual void push(variant operand)
      {
        if (operand.is<expression_identifier>())
          {
            stack_.push(operand);
          }
        else if (operand.is<already_in_stack>())
          {
            stack_.push(linker.get_stack_type());
          }
        else if (operand.is<local_variable>())
          {
            local_variable lv;
            stack_.push(lv.type);
          }
        else if (operand.is<schema*>())
          {
            assert(false); //not sure?
          }
        else
          {
            stack_.push(true_type(operand));
          }
      }

    virtual void exec_operator(operator_type op, int pop_count, int push_count)
      {
        variant arg1, arg2;
        switch(pop_count)
          {
            case 1:
              {
                arg1 = stack_.top(); stack_.pop(); break;
              }
            case 2:
              {
                arg2 = stack_.top(); stack_.pop();
                arg1 = stack_.top(); stack_.pop();
                break;
              }
            default: assert(false);
          }

        schema* type1 = null;
        schema* type2 = null;
        if (op == dot)
          {
            assert(false); //debug
          }
        else
          {
            type1 = true_type(arg1);
            type2 = true_type(arg2);
          }

        schema* result;
        operator_exec* exec = operators_.get_operator(op, type1, type1, &result);

        if (push_count > 0)
          push( result );
      }

    private:
      typedef std::stack<variant> expr_stack;

      expr_stack        stack_;
      operator_registry operators_;
  };
*/

int code_linker::register_variable(const str& name, schema* type, expression* value)
  {
    locals_list::iterator it = locals_.end();

		if (!name.empty())
			it = locals_.find(name);

		if (it == locals_.end())
      {
        int local_idx = local_count_++;

        if (value && !value->empty())
          {
            //td: type checking
            //expr_typeof to(*this);
            //value->visit(&to);

            schema* expr_type  = link_expression(*value); //to.result;
            add_instruction( i_store, local_idx );

            if (type == null)
              type = expr_type;

            if (type != expr_type && expr_type != null)
              {
                param_list error;
                error.add("id", STypeMismatch);
                error.add("desc", SAssigningMismatchedLocalVariable);
                error.add("variable", name);
                xs_throw(error);
              }
          }

        local_variable lv(local_idx, type);
        locals_.insert( locals_pair(name, lv) );
        return local_idx;
      }
    else
      {
        param_list error;
        error.add("id", SDuplicate);
        error.add("desc", SLocalVariableAlreadyDeclared);
        error.add("variable", name);
        xs_throw(error);
      }

    return -1;
  }

void code_linker::resolve_value(variant& arg, schema** type)
  {
    scope*				 scope_	= context_.scope_;
    type_registry* types_ = context_.types_;

    if (arg.is<local_variable>())
      {
        local_variable lv = arg;
        add_instruction(i_load, lv.index);

        if (type) *type = lv.type;
      }
    else if (arg.is<already_in_stack>())
      {
        already_in_stack ais = arg;
        if (type) *type = ais.type;
        return; //nothing to see here
      }
    else if (arg.is<expression_identifier>())
      {
        expression_identifier id = arg;

				schema* type_value = types_->get_type(id.value);
				if (type_value)
					{
						if (type) *type = type_schema<schema*>();

						arg = type_value;
						add_instruction(i_load_constant, add_constant(type_value));
					}
				else
					{
						locals_list::iterator it = locals_.find(id.value);
						if (it != locals_.end())
							{
								arg = it->second;
								if (type) *type = it->second.type;
								add_instruction(i_load, it->second.index);
							}
						else
							{
								//resolve symbol, note that loose identifiers
								//are always unqualified (are not part of of a member chain)
								//td: don't repeat constants on multiple access
								variant         result;
								schema_item     this_item;
								schema*         this_ = context_.this_type;
								IDynamicObject* dynamic = null;
								variant         scope_result;

								if (id.value == "this")
									{
										if(!this_)
											{
												param_list error;
												error.add("id", SOutOfContext);
												error.add("desc", SAccesingThisOnAThislessCode);
												xs_throw(error);
											}

										add_instruction(i_load_this);
										if (type) *type = this_;
									}
								else if (scope_ && scope_->resolve(id.value, scope_result))
									{
										arg = scope_result;
										add_instruction(i_load_constant, add_constant(scope_result));
										//test for concrete types
										if (type)
											{
												IDynamicObject* do_ = variant_cast<IDynamicObject*>(scope_result, null);
												if (do_)
													*type = do_->get_type();
												else
													*type = scope_result.get_schema();
											}
									}
								else if (this_ && this_->resolve(id.value, this_item))
									{
										assert(this_item.get); //must not be read only

										add_instruction(i_load_this);
										add_instruction(i_load_constant, add_constant(this_item.get));
										add_instruction(i_get, this_item.flags&DYNAMIC_ACCESS);

										if (type) *type = this_item.type;
									}
								else if (!context_.this_.empty() &&
												 (dynamic = variant_cast<IDynamicObject*>(context_.this_, null)) &&
												 dynamic->resolve(id.value, this_item))
									{
										//td: move all this resolving stuff into the context
										assert(this_item.get); //must not be read only

										add_instruction(i_load_this);
										add_instruction(i_load_constant, add_constant(this_item.get));
										add_instruction(i_get, this_item.flags&DYNAMIC_ACCESS);

										if (type) *type = this_item.type;
									}
								else if (scope_ && scope_->resolve(id.value, result))
									{
										add_instruction(i_load_constant, add_constant(result));
										if (type) *type = result.get_schema();
									}
								else
									{
										param_list error;
										error.add("id", SUnknownIdentifier);
										error.add("desc", SCannotResolve);
										error.add("identifier", id.value);
										xs_throw(error);
									}
							}
					}
      }
    else if (arg.is<schema_item>())
      {
        schema_item si = arg;
        assert(si.get);
        add_instruction(i_load_constant, add_constant(si.get));
        add_instruction(i_get, si.flags&DYNAMIC_ACCESS);

        if (type) *type = si.type;
      }
    else if (arg.empty())
      {
        if (type) *type = null;

        add_instruction(i_load_constant, add_constant(variant())); //td: not optimal, do i_load_null
      }
    else
      {
        constant c = arg;
        if (type) *type = c.value.get_schema();

        add_instruction(i_load_constant, add_constant(c.value));
      }
  }

void code_linker::resolve_operator(operator_type op, variant arg1, variant arg2, bool* dont_assign)
  {
    if (dont_assign) *dont_assign = false;
    if (arg1.is<constant>() && arg2.is<constant>())
      {
        //optimization case, both constants, just grab the result and keep going
        constant c1 = arg1;
        constant c2 = arg2;
        operator_exec* exec = operators_.get_operator(op, c1.value.get_schema(), c2.value.get_schema());
        if (exec)
          {
            variant  result = exec->exec(c1.value, c2.value);
            constant rc; rc.value = result;
            stack_.push( rc );
            return;
          }
      }

    schema* type1;
    schema* type2;
    resolve_value(arg1, &type1);
    resolve_value(arg2, &type2);

    size_t  result;
    schema* result_type;
    if (operators_.get_operator_index(op, type1, type2, result, &result_type))
      {
				if (op == op_typecast)
					{
						result_type = variant_cast<schema*>(arg2, null);
					}

				add_instruction(i_binary_operator, static_cast<short>(result));
        already_in_stack ais(result_type);
        stack_.push(ais);
      }
    else
      {
				bool invert = false; //arg1.is<already_in_stack>();
        if (!resolve_custom_operator(op, type1, invert, dont_assign))
          {
            if (!resolve_custom_operator(op, type2, invert, dont_assign))
              {
                add_instruction(i_dynamic_binary_operator, static_cast<short>(op));
                already_in_stack ais(null);
                stack_.push(ais);
              }
          }
      }
  }

void code_linker::resolve_unary_operator(operator_type op, variant arg1, bool* dont_assign)
  {
    if (dont_assign) *dont_assign = false;
    schema* type2 = type_schema<empty_type>();
    if (arg1.is<constant>())
      {
        //optimization case, both constants, jus grab the result and keep going
        constant c1 = arg1;
        operator_exec* exec = operators_.get_operator(op, c1.value.get_schema(), type2);
        if (exec)
          {
            variant  useless;
            variant  result = exec->exec(c1.value, useless);
            constant rc; rc.value = result;
            stack_.push( rc );
            return;
          }
      }

    schema* type1;
    resolve_value(arg1, &type1);

    size_t result;
    schema* result_type;
    if (operators_.get_operator_index(op, type1, type2, result, &result_type))
      {
        add_instruction(i_unary_operator, static_cast<short>(result));
        already_in_stack ais(result_type);
        stack_.push(ais);
      }
    else
      {
        if (type1 &&
						type1 != type_schema<empty_type>() &&
						type1 != type_schema<IDynamicObject*>() &&
						type1 != type_schema<variant>() &&
						type1 != type_schema<DynamicObject>())
          {
            schema_item custom_operator;
            if (type1->resolve(operator_name[op], custom_operator))
              {
                assert(custom_operator.exec);
                add_instruction(i_load_constant, add_constant(custom_operator.exec));
                add_call(i_call, 0, false, custom_operator.flags&DYNAMIC_ACCESS );
                stack_.push( already_in_stack(custom_operator.type) );

                if (dont_assign)
                  *dont_assign = custom_operator.flags&OP_BLOCK_ASSIGN;
              }
            else
              {
                param_list error;
                error.add("id", STypeMismatch);
                error.add("desc", SCannotResolveOperator);
                error.add("operator", str(operator_name[op]));
                xs_throw(error);
						}
          }
        else
          {
            add_instruction(i_dynamic_unary_operator, static_cast<short>(op));
          }
      }
  }

bool code_linker::resolve_custom_operator(operator_type op, schema* type, bool invert, bool* dont_assign)
  {
    if (!type)
      return false;

    schema_item custom_operator;
    if (!type->resolve(operator_name[op], custom_operator))
      return false;

    assert(custom_operator.exec);
    add_instruction(i_load_constant, add_constant(custom_operator.exec));
		add_call(i_call, 1, custom_operator.flags&DYNAMIC_ACCESS, false, invert);
    stack_.push( already_in_stack(custom_operator.type) );

    if (dont_assign)
      {
        *dont_assign = custom_operator.flags&OP_BLOCK_ASSIGN;
      }
    return true;
  }

void code_linker::resolve_assign(const variant& arg, bool invert_set)
  {
    if (arg.is<local_variable>())
      {
        local_variable lv = arg;
        add_instruction(i_store, lv.index);
      }
    else if (arg.is<expression_identifier>())
      {
        schema_item     this_item;
        schema*         this_ = context_.this_type;
        IDynamicObject* dynamic = null;

        expression_identifier id = arg;
        locals_list::iterator it = locals_.find(id.value);
        if (it != locals_.end())
          {
            add_instruction(i_store, it->second.index);
          }
        else if (this_ && this_->resolve(id.value, this_item))
          {
            assert(this_item.set); //must not be write only

            //there is a little disagreement between the way the operand
            //stack is arranged when linking member sets with "this" sets
            //it all boils down to the semantic difference between x.y() and y()
            //this issue should be addressed properly.
            //td:

            int lidx = add_anonymous_local();
            add_instruction(i_store, lidx);

            add_instruction(i_load_this);
            add_instruction(i_load, lidx);
            add_instruction(i_load_constant, add_constant(this_item.set));
            add_set(this_item.flags&DYNAMIC_ACCESS);
          }
        else if (!context_.this_.empty() &&
                 (dynamic = variant_cast<IDynamicObject*>(context_.this_, null)) &&
                 dynamic->resolve(id.value, this_item))
          {
            //td: move all this resolving stuff into the context
            assert(this_item.set); //must not be write only

            int lidx = add_anonymous_local();
            add_instruction(i_store, lidx);

            add_instruction(i_load_this);
            add_instruction(i_load, lidx);
            add_instruction(i_load_constant, add_constant(this_item.set));
						add_set(this_item.flags&DYNAMIC_ACCESS);
          }
        else
          {
            param_list error;
            error.add("id", SAssignError);
            error.add("desc", SCannotAssign);
            error.add("identifier", id.value);
            xs_throw(error);
          }
      }
    else if (arg.is<schema_item>())
      {
        schema_item si = arg;
        if (!si.set)
          {
            param_list error;
            error.add("id", SAssignError);
            error.add("desc", SAssigningToWriteOnly);
            xs_throw(error);
          }

        add_instruction(i_load_constant, add_constant(si.set));
				add_set(si.flags&DYNAMIC_ACCESS, invert_set);
      }
    else if (arg.is<already_in_stack>())
      {
				//td: phasing out, this should do nothing

				////td: there's a bit of a cluster f*ck here
				////resolving assigners works must cases except for this one
				////where a dynamic get has already been inserted. The right solution
				////is already implemented at xss (splitting expressions)
    //    assert(last_pc >= 0);
				//assert(code_[last_pc].id == i_dynamic_get);
				//int idx = code_[last_pc].data.value;
				//code_[last_pc].id = i_nop; //invalidate the get, keep the stack intact

				//add_instruction(i_dynamic_set, idx);
      }
    else
      {
        param_list error;
        error.add("id", SAssignError);
        error.add("desc", SAssigningToConstant);
        xs_throw(error);
      }
  }

int code_linker::add_constant( variant constant )
  {
    constants_.push_back( constant );
    return static_cast<int>(constants_.size()) - 1;
  }

//base_xs_linker
void base_xs_linker::link(xs_container& xs)
  {
    //traverse the container items, this process will
    //register names and collect code to be linked
    xs.visit( this );

    //and then we link code, a this point all symbols
    //should be bound for linking
    std::vector<link_item>::iterator lit = link_.begin();
    std::vector<link_item>::iterator lnd = link_.end();

    for(; lit != lnd; lit++)
      {
        code_context ctx;
        ctx.types_    = ctx_.types_;
        ctx.scope_    = ctx_.scope_;
        ctx.this_     = ctx_.this_;
        ctx.dsl_      = ctx_.dsl_;
        ctx.this_type = lit->this_type;
        ctx.args_     = lit->args.get();

        code_linker linker(ctx);
        lit->cde.visit(&linker);

        linker.link(lit->bc);
      }
  }

DynamicObject base_xs_linker::resolve_instance(std::vector<str> name)
  {
    if (name.empty())
      return DynamicObject();

    DynamicObject result = resolve_instance(*name.begin());
    if (result)
      {
        name.erase(name.begin());
        if (!name.empty())
          return result->resolve_instance(name);
      }

    return result;
  }

DynamicObject base_xs_linker::resolve_instance(const str& name)
  {
    DynamicObject result;

    if (output_)
      {
        std::vector<str> id;
        id.push_back(name);
        result = output_->resolve_instance(id);
        if (result)
          return result;
      }

    variant scope_object;
    if (ctx_.scope_ && ctx_.scope_->resolve(name, scope_object))
      {
        result = scope_object; //td: catch exception, throw invalid instance error.
      }

    return result;
  }

struct decode_property
  {
    decode_property(code_context& ctx, xs_property& info, DynamicObject output, fs::path file)
      {
        itm.type  = info.type.empty()? null : ctx.types_->get_type(info.type);
        itm.flags = DYNAMIC_ACCESS;

        variant value;
        if (info.value.empty())
          {
            if (itm.type)
              itm.type->create(value);
          }
        else
          {
            code_linker exp_linker(ctx);
            value = exp_linker.evaluate_expression(info.value);

            if (itm.type)
              {
                if (value.get_schema() != itm.type)
                  {
                    param_list error;
                    error.add("id", STypeMismatch);
                    error.add("property", info.name);
                    error.add("type", info.type);
                    xs_throw(error);
                  }
              }
            else
              {
                itm.type = value.get_schema();
              }
          }

        if (!info.get.empty() && !info.set.empty())
          {
            //get & set
            get_ref = ByteCode( new byte_code(file) );
            set_ref = ByteCode( new byte_code(file) );

            get_code = info.get;
            set_code = info.set;

            itm.get   = Getter( new code_getter(get_ref) );
            itm.set   = Setter( new code_setter(set_ref) );
          }
        else if (!info.set.empty())
          {
            //set & store
            int idx = output->add_anonymous(value);
            itm.get = Getter( new anonymous_getter(idx) );

            set_ref  = ByteCode( new byte_code(file) );
            itm.set  = Setter( new code_setter(set_ref, idx) );

            set_code = info.set;
          }
        else if (!info.get.empty())
          {
            //read only
            get_ref  = ByteCode( new byte_code(file) );
            get_code = info.get;
            itm.get  = Getter( new code_getter(get_ref) );
          }
        else
          {
            //regular property, no code
            int idx = output->add_anonymous(value);
            itm.get = Getter( new anonymous_getter(idx) );
            itm.set = Setter( new anonymous_setter(idx) );
          }
      }

    schema_item itm;
    ByteCode    get_ref;
    ByteCode    set_ref;
    code        get_code;
    code        set_code;
  };

void base_xs_linker::property_(xs_property& info)
  {
    if(!editable_output_)
      {
        param_list error;
        error.add("id", SOutOfContext);
        error.add("desc", SAlteringInmutableObject);
        error.add("property", info.name);
        error.add("type", info.type);
        xs_throw(error);
      }

    schema_item itm;
    itm.flags = DYNAMIC_ACCESS;

    if (info.type.empty())
      itm.type  = null;
    else
      {
        itm.type = ctx_.types_->get_type(info.type);
        if (!itm.type)
          {
            param_list error;
            error.add("id", SUnknownIdentifier);
            error.add("desc", SCannotResolveType);
            error.add("property", info.name);
            error.add("type", info.type);
            xs_throw(error);
          }
      }

    variant value;
    if (info.value.empty())
      {
        if (itm.type)
          itm.type->create(value);
      }
    else
      {
        code_linker exp_linker(ctx_);
        value = exp_linker.evaluate_expression(info.value);

        if (itm.type)
          {
            assert(value.get_schema() == itm.type); //td: type mismatch
            param_list error;
            error.add("id", STypeMismatch);
            error.add("property", info.name);
            error.add("type", info.type);
          }
        else
          {
            itm.type = value.get_schema();
          }
      }

    if (!info.get.empty() && !info.set.empty())
      {
        //get & set
        ByteCode get_code = ByteCode( new byte_code(file_) );
        ByteCode set_code = ByteCode( new byte_code(file_) );

        itm.get   = Getter( new code_getter(get_code) );
        itm.set   = Setter( new code_setter(set_code) );

        //keep track of the code, so we can link when the scope is complete
        link_item gli(get_code, info.get, ctx_.this_type);
        link_item sli(set_code, info.set, ctx_.this_type);

        link_.push_back(gli);
        link_.push_back(sli);
      }
    else if (!info.set.empty())
      {
        //set & store
        int idx = output_->add_anonymous(value);
        itm.get = Getter( new anonymous_getter(idx) );

        ByteCode set_code = ByteCode( new byte_code(file_) );
        itm.set           = Setter( new code_setter(set_code, idx) );

        link_item sli(set_code, info.set, ctx_.this_type);
        link_.push_back(sli);
      }
    else if (!info.get.empty())
      {
        //read only
        ByteCode get_code = ByteCode( new byte_code(file_) );
        itm.get = Getter( new code_getter(get_code) );
        link_item gli(get_code, info.get, ctx_.this_type);
        link_.push_back(gli);
      }
    else
      {
        //regular property, no code
        int idx = output_->add_anonymous(value);
        itm.get = Getter( new anonymous_getter(idx) );
        itm.set = Setter( new anonymous_setter(idx) );
      }

    editable_output_->add_item(info.name, itm);
  }

struct decode_method
  {
    decode_method(xs_method& info, code_context& ctx, fs::path file)
      {
        itm.flags = DYNAMIC_ACCESS;
        bc        = ByteCode( new byte_code(file) );
        itm.exec  = Executer( new code_executer(bc) );
        cde       = info.cde;

        pl = ParamList(new param_list);
        param_list_decl::iterator it = info.args.begin();
        param_list_decl::iterator nd = info.args.end();
        for(; it != nd; it++)
          {
            schema* param_type = it->type.empty()? null : ctx.types_->get_type(it->type);

            variant value;
            if (!it->default_value.empty())
              {
                code_linker exp_linker(ctx);
                value = exp_linker.evaluate_expression(it->default_value);

                assert(!param_type || value.get_schema() == param_type); //type mismatch
              }
            else if (param_type)
              {
                param_type->create(value);
              }

            pl->add(it->name, value);
          }
      }

    schema_item itm;
    ByteCode    bc;
    ParamList   pl;
    code        cde;
  };

void base_xs_linker::method_(xs_method& info)
  {
    if(!editable_output_)
      {
        param_list error;
        error.add("id", SOutOfContext);
        error.add("desc", SAlteringInmutableObject);
        error.add("method", info.name);
        error.add("type", info.type);
        xs_throw(error);
      }

    schema_item itm;
    itm.flags          = DYNAMIC_ACCESS;
    ByteCode exec_code = ByteCode( new byte_code(file_) );
    itm.exec           = Executer( new code_executer(exec_code) );

    ParamList pl = ParamList(new param_list);
    param_list_decl::iterator it = info.args.begin();
    param_list_decl::iterator nd = info.args.end();
    for(; it != nd; it++)
      {
        schema* param_type = it->type.empty()? null : ctx_.types_->get_type(it->type);

        variant value;
        if (!it->default_value.empty())
          {
            code_linker exp_linker(ctx_);
            value = exp_linker.evaluate_expression(it->default_value);

            assert(!param_type || value.get_schema() == param_type); //type mismatch
          }
        else if (param_type)
          {
            param_type->create(value);
          }

        pl->add(it->name, value);
      }


    link_item eli(exec_code, info.cde, ctx_.this_type, pl);
    link_.push_back(eli);

    editable_output_->add_item(info.name, itm);
  }

void base_xs_linker::event_(xs_event& info)
  {
    IDynamicObject*  object = output_?  output_.get() : null;
    IEditableObject* target = editable_output_;

    std::vector<str> name = info.name;
    str ev_name = name[info.name.size() - 1];
    name.erase(name.end() - 1);
    if (!name.empty())
      {
        assert(false); //td: !!! this is fffffffffffuuuuuuuuuuu 
        DynamicObject inst = resolve_instance(name);
        object = inst.get();
        target = variant_cast<IEditableObject*>(inst, null);
      }

    if(!object)
      {
        if (info.name.size() > 1)
          {
            param_list error;
            error.add("id", SUnknownIdentifier);
            error.add("desc", SInstanceNotFound);

            std::vector<str>::iterator it = name.begin();
            std::vector<str>::iterator nd = name.end();

            std::ostringstream oss;
            for(; it != nd; it++)
              oss << *it << ".";

            str instance_name = oss.str();
            instance_name = instance_name.substr(0, instance_name.size() - 1);

            error.add("instance", instance_name);
            xs_throw(error);
          }
        else
          assert(false); //internal malfunctioning
      }

    if(!target)
      {
        param_list error;
        error.add("id", SOutOfContext);
        error.add("desc", SAlteringInmutableObject);
        error.add("event", info.name);
        xs_throw(error);
      }

    size_t id = object->event_id(ev_name);
    if (id == 0)
      {
        //td: !!! revise this, it basically makes the event declarations absolote
        id = target->register_event(ev_name);
      }

    schema_item itm;
    itm.flags          = DYNAMIC_ACCESS|EVENT;
    ByteCode exec_code = ByteCode( new byte_code(file_) );
    itm.get            = Getter( new const_getter((int)id) ); //td: declare uint
    itm.exec           = Executer( new code_executer(exec_code) );

    ParamList pl = ParamList(new param_list);
    param_list_decl::iterator it = info.args.begin();
    param_list_decl::iterator nd = info.args.end();
    for(; it != nd; it++)
      {
        schema* param_type = it->type.empty()? null : ctx_.types_->get_type(it->type);

        variant value;
        if (!it->default_value.empty())
          {
            code_linker exp_linker(ctx_);
            value = exp_linker.evaluate_expression(it->default_value);

            assert(!param_type || value.get_schema() == param_type); //type mismatch
          }
        else if (param_type)
          {
            param_type->create(value);
          }

        pl->add(it->name, value);
      }

    link_item eli(exec_code, info.cde, ctx_.this_type, pl);
    link_.push_back(eli);

    target->add_item(ev_name, itm);
  }

void base_xs_linker::event_decl_(xs_event_decl& info)
  {
    assert(editable_output_);
    schema_item itm;
    itm.flags = EVENT_DECL; //td: signature

    editable_output_->add_item(info.name, itm);
  }

void base_xs_linker::const_(xs_const& info)
  {
    assert(false); //td:
  }

void base_xs_linker::instance_(xs_instance& info)
  {
    std::vector<str> id = info.id;
    assert(!id.empty());
    str instance_name = id[0];

    DynamicObject instance = resolve_instance(instance_name);
    id.erase(id.begin());

    if (!id.empty())
      {
        assert(instance);

        instance = instance->resolve_instance(id);

        if (!instance)
          {
            param_list error;
            error.add("id", SUnknownIdentifier);
            error.add("desc", SInstanceNotFound);
            error.add("instance", instance_name); //td: names to string
            xs_throw(error);
          }
      }

    if (!instance)
      {
        schema* i_type;

        if ((!info.class_name.empty()))
          i_type = ctx_.types_->get_type(info.class_name);
        else
          i_type = type_schema<default_object>();

        if (!i_type)
          {
            param_list error;
            error.add("id", SUnknownIdentifier);
            error.add("desc", STypeNotFound);
            error.add("type", info.class_name);
            xs_throw(error);
          }

        //instantiate
        variant res;
        i_type->create(res);

        instance = res; //td: check mismatch

        //td: rethink this mechanism at this stage
        //basically adding instances to the scope when they are not
        //found. At the moment it serves testng pursposes.
        if (ctx_.scope_)
          ctx_.scope_->register_symbol(instance_name, instance);
      }

    if (!instance)
      {
        param_list error;
        error.add("id", SUnknownIdentifier);
        error.add("desc", SInstanceNotFound);
        error.add("instance", instance_name);
        xs_throw(error);
      }

    instance_linker il(ctx_, instance, file_);
    il.link(info);
  }

void base_xs_linker::class_(xs_class& info)
  {
    class_linker cl(ctx_, file_);
    cl.link(info);
  }

void base_xs_linker::behaviour_(xs_behaviour& info)
  {
    behaviour_linker linker(ctx_);
    linker.link(info);
  }

void base_xs_linker::behaveas_(xs_implement_behaviour& info)
  {
    schema* type = ctx_.types_->get_type(info.name);
    if (!type)
      {
        param_list error;
        error.add("id", SUnknownIdentifier);
        error.add("desc", SBehaviourNotFound);
        error.add("behaviour", info.name);
        xs_throw(error);
      }

    behaviour_schema* behaviour = dynamic_cast<behaviour_schema*>(type);
    if (!behaviour)
      {
        param_list error;
        error.add("id", SOutOfContext);
        error.add("desc", STypeIsNotABehaviour);
        error.add("type", info.name);
        xs_throw(error);
      }

    //this code, as fas as this early revision, is special
    //its purpose if to link symbols between the concrete type
    //and the behaviour. It's probably going to be touchy for
    //a while. So, please, assign known identifiers and dont get
    //smart. For instance:
    //behave as SomeBehaviour
    //{
    //    behaviour_delegate_property = my_property;
    //}
    //
    //td: this should be linked as a "class" method, gonna be so sweet
    //td: meta, this are gonna go together and be oh so sweeter.

    //so we collect the delegates names
    name_collect_visitor ncv;
    behaviour->delegates().visit( &ncv );

    //then create a fake object to
    default_object bindings;
    std::vector<str>::iterator it = ncv.names.begin();
    std::vector<str>::iterator nd = ncv.names.end();
    for(; it != nd; it++)
      {
        bindings.add_attribute(*it);
      }

    code_context    bind_ctx;
    type_scope      class_scope(ctx_.this_type); //class access
    object_scope    this_scope(output_.get());   //instance access
    composite_scope bind_scope(&class_scope, &this_scope);

    bind_ctx.this_     = &bindings;
    bind_ctx.this_type = null;
    bind_ctx.scope_    = &bind_scope;
    if (!info.cde.empty())
      {
        code_linker cl(bind_ctx);
        info.cde.visit( &cl );

        ByteCode bc = cl.link(file_);
        execution_context ec(bc, bind_ctx.this_);
        ec.execute();
      }

    //private items, the final frontier, the idea
    //here is to hide the private names from the output
    //so we'll stash then on a context
    editable_scope link_symbols;
    bind_ctx.types_ = ctx_.types_;
    prelink_visitor plv(bind_ctx, &link_symbols, output_, file_);
    behaviour->privates().visit(&plv);

    //and before we're ready to finally link the code, we'll add
    //the bindings to the output object, note the delegates items
    //will be public
    delegate_visitor dv(&bindings, editable_output_, output_);
    behaviour->delegates().visit(&dv);

    //reconfigure for the actual code
    composite_scope code_scope(&link_symbols, ctx_.scope_);

    code_context  link_ctx;
    link_ctx.types_    = ctx_.types_;
    link_ctx.this_     = output_;
    link_ctx.this_type = ctx_.this_type;
    link_ctx.scope_    = &code_scope;

    base_xs_linker linker(link_ctx, file_, editable_output_); //note the output is the actual object being behaved-assed

    linker.output_ = output_;
    linker.link(behaviour->publics());

    //now that everything is settled we can link the private items
    prelink_visitor::prelink_list::iterator lit = plv.links.begin();
    prelink_visitor::prelink_list::iterator lnd = plv.links.end();
    for(; lit != lnd; lit++)
      {
        link_ctx.args_  = lit->args.get();

        code_linker linker(link_ctx);
        lit->cde.visit(&linker);

        linker.link(lit->bc);
      }
  }

void base_xs_linker::dsl_(dsl& info)
  {
    assert(false); //td:
  }

//class_linker
class_linker::class_linker(code_context& ctx, fs::path file):
  base_xs_linker(ctx, file),
  result_(null)
  {
  }

void class_linker::link(xs_class& info)
  {
    if (!info.super.empty())
      {
        schema* super_schema = ctx_.types_->get_type(info.super);
        variant new_obj;
        if (super_schema && super_schema->create(new_obj))
          {
            output_ = new_obj;
          }
        else
          {
            param_list error;
            error.add("id", SUnknownIdentifier);
            error.add("desc", SSuperclassNotFound);
            error.add("type", info.super);
            xs_throw(error);
          }
      }
    else
      output_ = DynamicObject(new default_object);

    result_           = new dynamic_class_schema(output_);
    class_            = DynamicClass( result_ );
    editable_output_  = result_;
    ctx_.this_type    = result_;
    ctx_.this_        = variant();

    base_xs_linker::link( info );

    ctx_.types_->add_type(info.name, class_);
  }

//instance_linker
instance_linker::instance_linker(code_context& ctx, DynamicObject instance, fs::path file):
  base_xs_linker(ctx, file),
  instance_(instance)
  {
  }

void instance_linker::link(xs_instance& info)
  {
    output_          = instance_;
    editable_output_ = variant_cast<IEditableObject*>(instance_, null);
    ctx_.this_type   = instance_->get_type();
    ctx_.this_       = instance_;

    base_xs_linker::link( info );
  }

//implicit_instance_linker
implicit_instance_linker::implicit_instance_linker(code_context& ctx, DynamicObject instance, fs::path file):
  base_xs_linker(ctx, file),
  instance_(instance)
  {
  }

void implicit_instance_linker::link(xs_container& info)
  {
    output_          = instance_;
    editable_output_ = variant_cast<IEditableObject*>(instance_, null);
    ctx_.this_type   = instance_->get_type();
    ctx_.this_       = instance_;

    base_xs_linker::link( info );
  }

//behaviour_linker
behaviour_linker::behaviour_linker(code_context& ctx):
  ctx_(ctx),
  result_(null)
  {
  }

void behaviour_linker::link(xs_behaviour& info)
  {
    behaviour_schema* super = null;
    if (!info.super.empty())
      {
        super = dynamic_cast<behaviour_schema*>(ctx_.types_->get_type(info.super));
        if(!super)
          {
            param_list error;
            error.add("id", SUnknownIdentifier);
            error.add("desc", SSuperclassNotFound);
            error.add("type", info.super);
            xs_throw(error);
          }
      }

    result_ = new behaviour_schema(super); //td: lost
    class_  = Schema(result_);

    info.visit(result_);

    ctx_.types_->add_type(info.name, class_);
  }

//prelink_visitor
void prelink_visitor::property_(xs_property& info)
  {
    assert(editable_);
    decode_property prop(ctx_, info, output_, file_);

    if (!prop.get_code.empty())
      {
        pre_link pl;
        pl.bc  = prop.get_ref; assert(prop.get_ref);
        pl.cde = prop.get_code;

        links.push_back(pl);
      }

    if (!prop.set_code.empty())
      {
        pre_link pl;
        pl.bc  = prop.set_ref; assert(prop.set_ref);
        pl.cde = prop.set_code;

        links.push_back(pl);
      }

    editable_->add_item(info.name, prop.itm);
  }

void prelink_visitor::method_(xs_method& info)
  {
    decode_method meth(info, ctx_, file_);

    pre_link pl;
    pl.bc   = meth.bc;
    pl.cde  = meth.cde;
    pl.args = meth.pl;
    links.push_back(pl);

    editable_->add_item(info.name, meth.itm);
  }

//name_collect_visitor
void name_collect_visitor::property_(xs_property& info)
  {
    names.push_back(info.name);
  }

void name_collect_visitor::method_(xs_method& info)
  {
    names.push_back(info.name);
  }
