
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
void cpp_expression_renderer::push(variant operand, bool top)
  {
		if (top && assigner)
			{
				str ass = resolve_assigner(operand, XSSObject(), assigner);
				push_rendered(ass, 0, operand.get_schema()); 
			}

    stack_.push(operand);
  }

str cpp_expression_renderer::resolve_assigner(variant operand, XSSObject instance, assign_info* ai)
	{
		XSSProperty prop;
		str					result;

    XSSObject caller = get_instance(operand);
    str separator = ctx_->getIdiom()->resolve_separator(caller);

		if (operand.is<expression_identifier>())
			{
				expression_identifier ei = operand;
				prop = get_property(instance, ei.value);
			}
		else if (operand.is<already_rendered>())
			{
				already_rendered ar = operand;
				result = ar.value;

				//here comes the hacky hoo
				//size_t last_dot = result.find_last_of("->");
        size_t last_dot = result.find_last_of(separator);
				if (last_dot != str::npos)
					{
						size_t count = result.size() - last_dot;
						result.erase(result.end() - count, result.end());
					}

				prop = variant_cast<XSSProperty>(ar.object, XSSProperty());
			}

    if (prop)
      {
        str set_fn = variant_cast<str>(dynamic_get(prop, "set_fn"), ""); //let the outside world determine
                                                                         //if a native function call shouls be made 

        str set_xss = variant_cast<str>(dynamic_get(prop, "set_xss"), ""); //such world can request to parse xss

				if (!set_xss.empty())
					{
						ai->type = XSS_RESOLVE;
						ai->data = set_xss;

						if (result.empty())
							return set_xss;
						else
							//return result + "->" + set_xss;
              return result + separator + set_xss;
					}
				else if (!set_fn.empty())
          {
						ai->type = FN_CALL;
						if (result.empty())
							return set_fn;
						else
							//return result + "->" + set_fn;
              return result + separator + set_fn;
          }
        else if (!prop->set.empty())
					{
						ai->type = FN_CALL;
						if (result.empty())
							return prop->name + "_set";
						else
							//return result + "->" + prop->name + "_set";
              return result + separator + prop->name + "_set";
					}
      }

		ai->type = VANILLA;
		return operand_to_string(operand, instance, null);
	}

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

str cpp_expression_renderer::get()
  {
    if(stack_.empty())
      {
        param_list error;
        error.add("id", SCPPEmptyStack);
        error.add("desc", SCPPEmptyExpression);
        xss_throw(error);
      }

		if (capturing_property_)
			{
				capturing_property_ = false;
				return render_captured_property();
			}

    variant result = stack_.top();
    if (result.is<already_rendered>())
      {
        already_rendered ar = result;
        return ar.value;
      }
		if (result.is<expression_identifier>())
			{
				expression_identifier ei = result;
				if (assigner)
					{
						if (ctx_->has_var(ei.value))
							return ei.value;

						XSSObject this_ = variant_cast<XSSObject>(ctx_->this_, XSSObject());
						str ass = resolve_assigner(ei, this_, assigner);

						xss_idiom* idiom = ctx_->idiom_;
						str result = idiom->resolve_this(ctx_);
						if (result.empty())
							return ass;

						return result + "->" + ass;
					}

				//test the this pointer
        XSSProperty prop = ctx_->get_property(ei.value);
				XSSMethod mthd = ctx_->get_method(ei.value);
        if (prop || mthd)
          {
            str this_str = ctx_->idiom_->resolve_this(ctx_);
            if (!this_str.empty())
              return this_str + "->" + ei.value; 
          }

				return ei.value;
			}
    else if (result.is<str>())
      {
        str res = result;
        str ss = '"' + res + '"';
        return ss;
      }

    str to_string = result;
    return to_string;
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
            
            if (ctx_->idiom_)
              {
                XSSProperty prop = ctx_->get_property(ei.value);
								XSSMethod mthd = ctx_->get_method(ei.value);
								if (prop)
									{
										result = prop->resolve_value();
										str this_str = ctx_->idiom_->resolve_this(ctx_);
                    if (!this_str.empty())
                      result = this_str + "->" + result;  
									}
								else if (mthd)
                  {
                    str this_str = ctx_->idiom_->resolve_this(ctx_);
                    if (!this_str.empty())
                      result = this_str + "->" + ei.value; //otherwise it doesnt get translated 
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

