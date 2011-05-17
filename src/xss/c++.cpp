
#include <xss/c++.h>
#include <xss/xss_error.h>

using namespace xkp;

//strings
const str SCPPEmptyStack("empty-stack");
const str SCPPIdiom("idiom");
const str SCPPInvalidOperator("invalid-operator");

const str SCPPEmptyExpression("Trying to use an empty expression");
const str SCPPEveryInstanceMustHaveId("Trying to use an instance without id");
const str SCPPAssignOperator("Assign operators can only be used as the base of an expression");

//cpp_code
void cpp_code::for_(stmt_for& info)
	{
    std::stringstream ss;
    str iterable_declaration = info.init_variable.type + " " + info.init_variable.id;
    cpp_expression_renderer cpp_rend(ctx_);
    ss << "for(" << iterable_declaration << " = " << render_expression(info.init_variable.value, ctx_)
        << "; " << render_expression(info.cond_expr, ctx_)
        << "; " << render_expression(info.iter_expr, ctx_) << ")";

    add_line(ss.str(), true);
    add_line("{", true);
		render_code(info.for_code, indent_ + 1);
		add_line("}", true);
	}

void cpp_code::variable_(stmt_variable& info)
  {
		schema* sche_type = ctx_->get_type(info.type);
    cpp_expression_renderer cpp_rend(ctx_);
    str str_type;
		if (!sche_type)
			{
				if (!info.value.empty())
					{
						expr_type_resolver typer(vars_, ctx_);
						info.value.visit(&typer);

						sche_type = typer.get();
						str_type = typer.type_name();
					}
			}

    if (sche_type)
			{
				str_type = ctx_->get_type_name(sche_type);
			}

		vars_.insert(std::pair<str, schema*>(info.id, sche_type)); //td: !!! XSSType

    std::stringstream ss;
    str ind = get_indent_str();

    ss << ind << str_type << " " << info.id;
    if (!info.value.empty())
      ss << " = " << render_expression(info.value, ctx_);

    ss << ";\n";

    add_line(ss.str());
  }

str cpp_code::render_expression(expression& expr, XSSContext ctx)
  {
    return idiom_utils::render_expression<cpp_expression_renderer>(expr, ctx);
  }

//cpp_args
str cpp_args::resolve_param(const param_decl& param)
	{
    return param.type + " " + param.name;
	}

//cpp_idiom
variant cpp_idiom::process_code(code& cde, param_list_decl& params, XSSContext ctx)
	{
    CPPCode result(new cpp_code(ctx, cde));
    return result;
	}

variant cpp_idiom::process_expression(expression expr, XSSObject this_)
	{
    XSSContext ctx(new xss_composite_context(ctx_));
    ctx->this_ = this_;

    CPPExpression result(new cpp_expr(ctx, expr));
    return result;
	}

variant cpp_idiom::process_args(param_list_decl& params)
	{
		CPPArgs result(new cpp_args(params));
		return result;
	}

str cpp_idiom::resolve_this(XSSContext ctx)
	{
    //Programming languages do not agree on how to use *this* pointers
    //c++ lets you ignore them, jsva script et all force you to 
    //and there are other intrincate circumstances (like functions in js)
    //where something must be done.
    
		XSSObject instance = variant_cast<XSSObject>(ctx->this_, XSSObject());
		bool		  override_id_as_this = instance->has("@class");

		if (!override_id_as_this && id_as_this_)
      {
        if (!ctx->this_.empty())
          {
            str iid = variant_cast<str>(dynamic_get(ctx->this_, "id"), str());
            if (iid.empty())
              {
                param_list error;
                error.add("id", SCPPIdiom);
                error.add("desc", SCPPEveryInstanceMustHaveId);
                xss_throw(error);
              }

            //another use case, we might have an instance that has an internal id
	          //which means a name to be used in code instead of the plain instance name
	          XSSObject obj = ctx->resolve_instance(iid);
	          if (obj && obj->has("internal_id"))
		          {
			          iid = variant_cast<str>(dynamic_get(obj, "internal_id"), str());

                // so i need some internal_id emptys
			          //assert(!iid.empty());
		          }

            return iid;
          }
      }
    else if (!ctx->this_.empty())
      {
        return "this";
      }
      
    return "";
	}

str cpp_idiom::resolve_separator(XSSObject lh)
	{
		return "->";
	}

//cpp_expression_renderer
void cpp_expression_renderer::exec_operator(operator_type op, int pop_count, int push_count, bool top)
  {
		if (capturing_property_ && op != op_dot)
			{
				//the dot chain has stopped, lets collect
				//td: unfortunately this solution only works for straight dot chains. 
				//I'll worry about it later

				variant prop = stack_.top(); 
				capturing_property_ = false;

				already_rendered ar;
				ar.value = render_captured_property();
				ar.object = prop;

				stack_.push(ar);
			}

		if (top && assigner)
			{
				assert(op == op_dot); //I'm sure there are more use cases, but I'll deal with this one exclusively
															//for now
			}

    variant arg1, arg2;
    int op_prec = idiom_utils::get_operator_prec(op);
    switch(op)
      {
        // add here the operators that I'll customize
				case op_dot:
        case op_dot_call:
          {
            switch(pop_count)
              {
                case 0: break;
                case 1: arg1 = stack_.top(); stack_.pop(); break;
                case 2:
                  {
                    arg2 = stack_.top(); stack_.pop();
                    arg1 = stack_.top(); stack_.pop();
                    break;
                  }
                default: assert(false);
              }
          }
      }

    switch(op)
      {
        case op_dec:
        case op_inc:
        case op_unary_plus:
        case op_unary_minus:
        case op_not:
        case op_assign:
        case op_plus_equal:
        case op_minus_equal:
        case op_mult_equal:
        case op_div_equal:
        case op_shift_right_equal:
        case op_shift_left_equal:
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

        case op_array:
        case op_index:
        case op_call:
        case op_func_call:
        case op_parameter:
          {
            // execute exec_operator of base class with the same parameters
            expression_renderer::exec_operator(op, pop_count, push_count, top);
            break;
          }

				case op_dot:
          {
            //so here we'll try to find out if we're generating an object
            //with a get function on a property. This does not constitute
            //type checking, that's a lot of work unless the code_linker feels
            //like cooperating
			      XSSObject caller = get_instance(arg1);
			      if (!caller)
			      {
				      if (arg1.is<expression_identifier>())
					      {
						      expression_identifier ei = arg1;
						      caller = ctx_->get_xss_type(ei.value); //td: meh, do the context
																												 //also do static members, this is getting sloppy	
					      }
			      }
			
            str       s2 = operand_to_string(arg2, caller);
            XSSObject o2 = get_instance(caller, s2);

            XSSProperty prop = get_property(caller, s2);
						if (prop && prop->has("internal_id"))
							{
								//this I'm not sure how right is... but here goes anyway.
								//At the moment this solves enumerators that must carry different names into the output
								str iid = variant_cast<str>(dynamic_get(prop, "internal_id"), str()); assert(!iid.empty());
								push_rendered(iid, op_prec, variant());
								break;
							}

						//And since I am on it...
						//there is a use case where some properties need a variable to represent them 
						//unfortunately such variables need the whole property chain in order to be effective
						if (prop && prop->has("property_xss"))
							{
								assert(!capturing_property_); //only a variable property per schema, please
								capturing_property_ = true; 

								capture_property_.prop = prop;
								capture_property_.xss  = variant_cast<str>(dynamic_get(prop, "property_xss"), str());

								for(int i = 0; i < capture_property_.xss.size(); i++)
									{
										if (capture_property_.xss[i] == '\'')
											capture_property_.xss[i] ='"';
									}

								push_rendered(operand_to_string(arg1), op_prec, prop);
								break;
							}

            bool has_getter = prop && !prop->get.empty();

            std::stringstream ss;
            ss << operand_to_string(arg1);

						str separator = ctx_->getIdiom()->resolve_separator(caller);
            if (!ss.str().empty())
              ss << separator;
            
						if (top && assigner)
							{
								ss << resolve_assigner(arg2, caller, assigner);	
							}
						else if (prop)
              {
                str get_fn  = variant_cast<str>(dynamic_get(prop, "get_fn"), ""); 
                str get_xss = variant_cast<str>(dynamic_get(prop, "get_xss"), ""); 

                if (!get_xss.empty())
                  ss << get_xss;
                else if (!get_fn.empty())
                  ss << get_fn << "()";
                else if (has_getter)
                  ss << s2 << "_get()";
                else
                  ss << s2;
              }
            else 
               ss << s2;

            if (prop)
              push_rendered(ss.str(), op_prec, prop);
            else
              push_rendered(ss.str(), op_prec, o2);
            break;
          }

        case op_dot_call:
          {
            std::stringstream ss;
            
            str iid = operand_to_string(arg1);
            if (!iid.empty())
              ss << "->";

            ss << operand_to_string(arg2);
            push_rendered(ss.str(), op_prec, variant());
            break;
          }

				default:
          assert(false); //td:
      }
  }

str cpp_expression_renderer::operand_to_string(variant operand, XSSObject parent, int* prec)
  {
    str result;
    int result_prec = 0;
    if (operand.is<expression_identifier>())
      {
        expression_identifier ei = operand;
        result = ei.value;

        if (!parent)
          {
            //here we ought to resolve a single symbol (ex width = 10)
            //thid *could* belong to the "this" pointer
            
            str separator = ctx_->idiom_->resolve_separator();
            if (ctx_->idiom_)
              {
                XSSProperty prop = ctx_->get_property(ei.value);
								XSSMethod mthd = ctx_->get_method(ei.value);
								if (prop)
									{
										result = prop->resolve_value();
										str this_str = ctx_->idiom_->resolve_this(ctx_);
                    if (!this_str.empty())
                      result = this_str + separator + result;
									}
								else if (mthd)
                  {
                    str this_str = ctx_->idiom_->resolve_this(ctx_);
                    if (!this_str.empty())
                      result = this_str + separator + ei.value; //otherwise it doesnt get translated 
                  }
				        else
				          {
					          //another use case, we might have an instance that has an internal id
					          //which means a name to be used in code instead of the plain instance name
					          XSSObject obj = ctx_->resolve_instance(result);

					          if (obj && obj->has("internal_id"))
						          {
							          result = variant_cast<str>(dynamic_get(obj, "internal_id"), str());

                        // so i need some internal_id emptys
							          //assert(!result.empty());
						          }
				          }
              }
          }
      }
    else if (operand.is<already_rendered>())
      {
        already_rendered ar = operand;
        result = ar.value;
        result_prec = ar.precedence;
      }
    else if (operand.is<str>())
      {
        str opstr = operand;
        result = '"' + opstr + '"';
      }
    else
      {
        str opstr = operand;
        result = opstr;
      }

    if (prec) *prec = result_prec;
    return result;
  }

