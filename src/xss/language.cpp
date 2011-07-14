
#include <xss/language.h>
#include <xss/xss_error.h>

using namespace xkp;

const str SLinker("linker");

//source_collector
void source_collector::property_(xs_property& info)
  {
    properties.push_back(info);
  }

void source_collector::method_(xs_method& info)
  {
    methods.push_back(info);
  }

void source_collector::event_(xs_event& info)
  {
    events.push_back(info);
  }

void source_collector::event_decl_(xs_event_decl& info)
  {
  }

void source_collector::const_(xs_const& info)
  {
  }

void source_collector::instance_(xs_instance& info)
  {
    instances.push_back(info);
  }

void source_collector::class_(xs_class& info)
  {
  }

void source_collector::behaviour_(xs_behaviour& info)
  {
  }

void source_collector::behaveas_(xs_implement_behaviour& info)
  {
  }

void source_collector::dsl_(dsl& info)
  {
  }

//code_type_resolver
code_type_resolver::code_type_resolver(XSSContext ctx)
  {
  }

XSSType code_type_resolver::get()
  {
    assert(false); //implement
    return XSSType();
  }

void code_type_resolver::variable_(stmt_variable& info)
  {
  }

void code_type_resolver::return_(stmt_return& info)
  {
    assert(false); //td: punt til java
  }

void code_type_resolver::if_(stmt_if& info)
  {
  }

void code_type_resolver::for_(stmt_for& info)
  {
  }

void code_type_resolver::iterfor_(stmt_iter_for& info)
  {
  }

void code_type_resolver::while_(stmt_while& info)
  {
  }

void code_type_resolver::break_()
  {
  }

void code_type_resolver::continue_()
  {
  }

void code_type_resolver::expression_(stmt_expression& info)
  {
  }

void code_type_resolver::dsl_(dsl& info)
  {
  }

void code_type_resolver::dispatch(stmt_dispatch& info)
  {
  }

//expr_type_resolver
expr_type_resolver::expr_type_resolver(XSSContext ctx) : 
  is_variant_(false), 
  ctx_(ctx)
  {
  }

XSSType expr_type_resolver::get()
	{
		if (is_variant_)
			return ctx_->get_type("var");

		assert(stack_.size() <= 1);

    if (stack_.size() == 1)
		  return resolve_type(stack_.top());

    return ctx_->get_type("void");
	}

void expr_type_resolver::push(variant operand, bool top)
	{
		stack_.push(operand);
	}

void expr_type_resolver::exec_operator(operator_type op, int pop_count, int push_count, bool top)
	{
		if (is_variant_)
			return; //cannot resolve anymore, bail

		switch(op)
			{
				case op_inc:
				case op_dec:
				case op_ref:
				case op_unary_plus:
				case op_unary_minus:
					{
						variant top = stack_.top(); stack_.pop();
						stack_.push(resolve_type(top));
						break;
					}

				case op_not:
				case op_typecheck:
				case op_namecheck:
					{
						stack_.pop();
						stack_.push(ctx_->get_type("bool"));
						break;
					}

				case op_equal:
				case op_notequal:
				case op_gt:
				case op_lt:
				case op_ge:
				case op_le:
				case op_and:
				case op_or:
					{
						stack_.pop();
						stack_.pop();
						stack_.push(ctx_->get_type("bool"));
						break;
					}

				case op_typecast:
					{
						variant top = stack_.top(); stack_.pop();
						stack_.pop();

						stack_.push(top);
						break;
					}

				case op_shift_right_equal:
				case op_shift_left_equal:
				case op_assign:
				case op_plus_equal:
				case op_minus_equal:
				case op_mult_equal:
				case op_div_equal:
					{
            param_list error;
            error.add("id", SLinker);
            error.add("desc", SAssignOperator);
            xss_throw(error);
						break;
					}

				case op_parameter:
          {
						//do nothing
            break;
          }

				case op_index:
					{
						//td: warning
						//is_variant_ = true;
            variant arg1 = stack_.top(); stack_.pop();
            variant arg2 = stack_.top(); stack_.pop();

            XSSType result = ctx_->get_type("var");
            XSSType xsti = resolve_type(arg2);

            if (xsti->is_array())
              result = xsti->array_type();

            stack_.push(result);
						break;
					}

				case op_array:
          {
            int arg_count = stack_.top(); stack_.pop();

            //verify array type from the type of there elements
            XSSType arr_type;
            for(int i = 0; i < arg_count; i++)
							{
								variant opnd = stack_.top(); stack_.pop();
								XSSType opnd_type = resolve_type(opnd);
                if (arr_type != opnd_type)
                  {
                    if (arr_type)
                      break; //different types
                    else
                      arr_type =opnd_type;
                  }
							}

            XSSType result = ctx_->get_array_type(arr_type);
            stack_.push(result);
            break;
          }

				case op_mult:
				case op_divide:
				case op_mod:
				case op_plus:
				case op_minus:
				case op_shift_right:
				case op_shift_left:
					{
						variant arg2 = stack_.top(); stack_.pop();
						variant arg1 = stack_.top(); stack_.pop();

						XSSType xstype1 = resolve_type(arg1);
						XSSType xstype2 = resolve_type(arg2);
						schema* result = null;
						size_t  idx;

            //handle native types, those combinable in xs
            if (operators_.get_operator_index(op, xstype1->native_type(), xstype2->native_type(), idx, &result) && result)
              {
							  stack_.push(ctx_->get_type(result));
              }
						else
              {
                param_list error;
                error.add("id", SLinker);
                error.add("desc", SCannontResolveOperator);
                error.add("op", xss_utils::operator_string(op));
                error.add("left", xstype1->id());
                error.add("right", xstype2->id());
                xss_throw(error);
              }
						break;
					}

				case op_call:
          {
            int args = stack_.top(); stack_.pop();

            //pop all parameters from stack
            for(int i = 0; i < args; i++)
							stack_.pop();

            //and do nothing
            break;
          }

				case op_func_call:
          {
            int args = stack_.top(); stack_.pop();
            expression_identifier ei = stack_.top(); stack_.pop();

            //pop all parameters from stack
            for(int i = 0; i < args; i++)
							stack_.pop();

            resolve_info call;
            if (!ctx_->resolve(ei.value, call))
              {
                param_list error;
                error.add("id", SLinker);
                error.add("desc", SCannontResolve);
                error.add("what", ei.value);
                xss_throw(error);
              }

            stack_.push(call.type);
            break;
          }

				case op_dot:
					{
						expression_identifier arg1  = stack_.top(); stack_.pop();
						expression_identifier arg2  = stack_.top(); stack_.pop();

            XSSType     result = ctx_->get_type("var");
            search_info left;
            if (ctx_->resolve(arg2.value, left))
              {
                search_info right;
                right.left = &left;
                if (ctx_->resolve(arg1.value, right))
                  {
                    result = right.type;
                    if (result->is_variant())
                      is_variant_ = true;
                  }
              }
            else
              is_variant_ = true;

            stack_.push(result);
            break;
					}

				case op_dot_call:
          {
						expression_identifier arg1  = stack_.top(); stack_.pop();
						expression_identifier arg2  = stack_.top(); stack_.pop();

            XSSType     result = ctx_->get_type("var");
            search_info left;
            if (ctx_->resolve(arg2.value, left))
              {
                search_info right;
                right.left = &left;
                if (ctx_->resolve(arg1.value, right))
                  {
                    switch(right.what)
                      {
                        case RESOLVE_METHOD:
                          {
                            assert(false);
                            XSSMethod mthd = right.value;
                            //td: result = mthd->type();
                            break;
                          }
                      }

                  }
              }

            stack_.push(result);
            break;
          }
			} // switch
	}

XSSType expr_type_resolver::resolve_type(variant var)
	{
		XSSType result;

		if (var.is<XSSType>())
			return var;
		else if (var.is<expression_identifier>())
			{
				expression_identifier ei = var;

        resolve_info item;
        if (ctx_->resolve(ei.value, item))
          {
            return item.type;
          }
			}
		else if (var.is<XSSObject>()) 
      {
        XSSObject obj = var;
        return obj->type();
      }
    else
      result = ctx_->get_type(var.get_schema());

		return result;
	}

variant expr_type_resolver::top_stack()
  {
    assert(stack_.size() == 1);

    return stack_.top();
  }

//code_type_resolver
//xs_type_info code_type_resolver::get()
//	{
//		if (is_variant_)
//			return null;
//
//    xs_type_info result_;
//    local_variables::iterator it = vars_.find("@@result_");
//    if (it != vars_.end())
//      result_ = it->second;
//
//		return result_;
//	}
//
//void code_type_resolver::variable_(stmt_variable& info)
//	{
//    xs_type_info xsti;
//    xsti.type = ctx_->get_type(info.type);
//		if (!xsti.type)
//			{
//				if (!info.value.empty())
//					{
//						expr_type_resolver typer(ctx_);
//						info.value.visit(&typer);
//
//            variant ty_top = typer.top_stack();
//
//            if (ty_top.is<xs_type_info>())
//              xsti = ty_top;
//            else
//              xsti = typer.get();
//					}
//			}
//    else if (xsti.type->options() & TYPE_ITERATED)
//      {
//        xsti.is_array = true;
//        xsti.array_type = type_schema<variant>();
//
//        if (!info.value.empty())
//          {
//            expr_type_resolver typer(ctx_);
//            info.value.visit(&typer);
//
//            xsti = typer.get();
//          }
//      }
//
//    register_var(info.id, xsti, info.type);
//	}
//
