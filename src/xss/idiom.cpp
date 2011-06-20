
#include <xss/idiom.h>
#include <xss/xss_error.h>

#include <fstream>

using namespace xkp;

//strings
const str SEmptyStack("empty-stack");
const str SIdiom("idiom");
const str SInvalidOperator("invalid-operator");
const str SDeclaration("variable-declaration");
const str SCannotResolve("unkown-identifier");
const str SVariableAssigner("variable-assigner");

const str SEmptyExpression("Trying to use an empty expression");
const str SEveryInstanceMustHaveId("Trying to use an instance without id");
const str SAssignOperator("Assign operators can only be used as the base of an expression");
const str SDuplicateVariable("Variable already declared");
const str SUnknownVariable("Variable not found");
const str SMultipleTypeVariable("Multiple type assign to only one variable");

//td: !!! stop duplicating this array
const char* operator_str[] =
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
  };

const int operator_prec[] =
  {
    2,  //"++",   //op_inc,
    2,  //"--",   //op_dec,
    2,  //"&",    //op_ref,
    2,  //"+",    //op_unary_plus,
    2,  //"-",    //op_unary_minus,
    2,  //"!",    //op_not,
    3,  //"*",    //op_mult,
    3,  //"/",    //op_divide,
    3,  //"%",    //op_mod,
    2,  //"as",   //op_typecast,
    2,  //"is",   //op_typecheck,
    2,  //"has",  //op_namecheck,
    4,  //"+",    //op_plus,
    4,  //"-",    //op_minus,
    5,  //">>",   //op_shift_right,
    5,  //"<<",   //op_shift_left,
    14, //">>=",  //op_shift_right_equal,
    14, //"<<=",  //op_shift_left_equal,
    7,  //"==",   //op_equal,
    7,  //"!=",   //op_notequal,
    6,  //">",    //op_gt,
    6,  //"<",    //op_lt,
    6,  //">=",   //op_ge,
    6,  //"<=",   //op_le,
    11, //"&&",   //op_and,
    12, //"||",   //op_or,
    14, //"=",    //op_assign,
    14, //"+=",   //op_plus_equal,
    14, //"-=",   //op_minus_equal,
    14, //"*=",   //op_mult_equal,
    14, //"/=",   //op_div_equal,
    1,  //".",    //op_dot,
    1,  //".",    //op_dot_call
    1,  //"[]",   //op_index,
    1,  //"",     //op_call,
    1,  //"",     //op_func_call
    1,  //"",     //op_array,
    1,  //"",     //op_parameter
  };


//expression_renderer
expression_renderer::expression_renderer(XSSContext ctx, bool id_as_this)
  : assigner(null), ctx_(ctx), capturing_property_(false)
  {
  }

XSSProperty expression_renderer::get_property(variant v)
  {
    if (v.is<XSSProperty>())
      return v;

    if (v.is<already_rendered>())
      {
        already_rendered ar = v;
        return get_property(ar.object);
      }
    else if (v.is<expression_identifier>())
      {
        expression_identifier ei = v;
        return ctx_->get_property(ei.value);
      }

    return XSSProperty();
  }

XSSProperty expression_renderer::get_property(variant v, const str& name)
  {
    if (v.is<XSSObject>())
      {
        XSSObject obj = v;
        return ctx_->get_property(obj, name);
      }
    else if (v.is<already_rendered>())
      {
        already_rendered ar = v;
        return get_property(ar.object, name);
      }
    else if (v.is<expression_identifier>())
      {
        assert(false); //td:
      }

    return XSSProperty();
  }

XSSObject expression_renderer::get_instance(variant v)
  {
    if (v.is<XSSObject>())
      {
        XSSObject obj = v;
        return obj;
      }
    else if (v.is<already_rendered>())
      {
        already_rendered ar = v;
        return get_instance(ar.object);
      }
    else if (v.is<expression_identifier>())
      {
        expression_identifier ei = v;
        return ctx_->resolve_instance(ei.value);
      }

    return XSSObject();
  }

XSSObject expression_renderer::get_instance(variant v, const str& name)
  {
    if (v.is<XSSObject>())
      {
        XSSObject obj = v;
        return variant_cast<XSSObject>(ctx_->evaluate_property(obj, name), XSSObject());
      }
    else if (v.is<already_rendered>())
      {
        already_rendered ar = v;
        return variant_cast<XSSObject>(ctx_->evaluate_property(ar.object, name), XSSObject());
      }
    else if (v.is<expression_identifier>())
      {
        assert(false);
      }

    return XSSObject();
  }

str expression_renderer::resolve_assigner(variant operand, XSSObject instance, assign_info* ai)
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
              return result + separator + set_xss;
					}
				else if (!set_fn.empty())
          {
						ai->type = FN_CALL;
						if (result.empty())
							return set_fn;
						else
              return result + separator + set_fn;
          }
        else if (!prop->set.empty())
					{
						ai->type = FN_CALL;
						if (result.empty())
							return prop->name + "_set";
						else
              return result + separator + prop->name + "_set";
					}
      }

		ai->type = VANILLA;
		return operand_to_string(operand, instance, null);
	}

void expression_renderer::push(variant operand, bool top)
  {
		if (top && assigner)
			{
				str ass = resolve_assigner(operand, XSSObject(), assigner);
				push_rendered(ass, 0, operand.get_schema());
			}

    stack_.push(operand);
  }

str expression_renderer::render_captured_property()
	{
		variant prop = stack_.top(); stack_.pop();
		str     value = operand_to_string(prop);

		//td: !!! CONTEXT
		XSSProject project_ = ctx_->project_;
		xss_idiom* idiom_		= ctx_->idiom_;
		XSSContext context(new xss_code_context(project_, idiom_, fs::path()));
		xss_code_context& ctx = *context.get();

		XSSGenerator gen(new xss_generator(context));
		project_->push_generator(gen);

		project_->prepare_context(ctx, gen);

		ctx.scope_->register_symbol("path",	value);
		ctx.scope_->register_symbol("property",	prop);

		str result = project_->generate_xss(capture_property_.xss, gen);

		project_->pop_generator();

		return result;
	}

void expression_renderer::exec_operator(operator_type op, int pop_count, int push_count, bool top)
  {
		variant arg1, arg2;
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

		if (top && assigner)
			{
				assert(op == op_dot ||
               op == op_index);   //I'm sure there are more use cases, but I'll deal with this one exclusively
															    //for now
				//assert(op == op_dot); //I'm sure there are more use cases, but I'll deal with this one exclusively
				//											//for now
			}

    int op_prec = operator_prec[op];
    switch(op)
      {
        case op_dec:
        case op_inc:
          {
						assign_info ai;
						str ass = resolve_assigner(arg1, XSSObject(), &ai);

						int prec;
            str os = operand_to_string(arg1, XSSObject(), &prec);

						std::stringstream ss;
						switch (ai.type)
							{
								case VANILLA:
									{
										if (op_prec < prec)
											ss << "(" << os << ")" << operator_str[op];
										else
											ss << os << operator_str[op];
										break;
									}
								case FN_CALL:
									{
										str op_str = operator_str[op];
										op_str.erase(op_str.end() - 1); //now this is too much fun

										ss << ass << "(" << os << " " << op_str << " 1)";
										break;
									}
								default:
									assert(false); //use case trap
							}

            push_rendered(ss.str(), op_prec, arg1.get_schema());
            break;
          }

        case op_unary_plus:
        case op_unary_minus:
        case op_not:
          {
            int prec;
            str os = operand_to_string(arg1, XSSObject(), &prec);

            std::stringstream ss;
            if (op_prec < prec)
              ss << operator_str[op] << "(" << os << ")";
            else
              ss << operator_str[op] << os;

            push_rendered(ss.str(), op_prec, arg1.get_schema());
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
						param_list error;
						error.add("id", SInvalidOperator);
						error.add("desc", SAssignOperator);

						xss_throw(error);
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
            int p1;
            int p2;

						str os1 = operand_to_string(arg1, XSSObject(), &p1);
            str os2 = operand_to_string(arg2, XSSObject(), &p2); //td: resolve properties and stuff

            if (op_prec < p1)
              os1 = "(" + os1 + ")";

            if (op_prec < p2)
              os2 = "(" + os2 + ")";

            std::stringstream ss;
            ss << os1 << " " << operator_str[op] << " " << os2;
            push_rendered(ss.str(), op_prec, variant());
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
                  xs_type_info xsti = ctx_->get_xss_type(ei.value); //td: meh, do the context
																												            //also do static members, this is getting sloppy
                  caller = xsti.object;
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
						    //And since I am on it...
						    //there is a use case where some properties need a variable to represent them
						    //unfortunately such variables need the whole property chain in order to be effective
						    if (prop->has("property_xss"))
							    {
								    assert(!capturing_property_); //only a variable property per schema, please

								    capture_property_.prop = prop;
								    capture_property_.xss  = variant_cast<str>(dynamic_get(prop, "property_xss"), str());

								    for(size_t i = 0; i < capture_property_.xss.size(); i++)
									    {
										    if (capture_property_.xss[i] == '\'')
											    capture_property_.xss[i] ='"';
									    }

                    push_rendered(operand_to_string(arg1), 0, prop);
                    push_rendered(render_captured_property(), op_prec, prop);
                    break;
							    }

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

            str opnd1 = operand_to_string(arg1);
            str opnd2 = operand_to_string(arg2);

						str separator = ctx_->getIdiom()->resolve_separator();

				    //here comes the hacky hoo
            size_t first_dot = opnd2.find_first_of(separator);
				    if (first_dot != str::npos)
					    {
						    str str_this = opnd2.substr(0, first_dot);
                if (str_this == opnd1)
                  opnd2.erase(0, first_dot + 1);
					    }

            ss << opnd1 << "." << opnd2;
            push_rendered(ss.str(), op_prec, variant());
            break;
          }

        case op_func_call:
          {
            //td!!! this is silly, function calls ( foo(bar) instead of foo1.foo(bar) ) configure
            //the stack differently, so we must duplicate the code.

            std::stringstream result;

            str caller = operand_to_string(arg1);

						result << caller << "(";

            int args = arg2;

            //pop the arguments
			      std::vector<str> params;
            for(int i = 0; i < args; i++)
              {
                variant arg  = stack_.top(); stack_.pop();
                str     sarg = operand_to_string(arg);

                XSSProperty prop = get_property(arg);
                if (prop)
                  {
                    str get_fn = variant_cast<str>(dynamic_get(prop, "get_fn"), "");
                    if (!get_fn.empty())
                      sarg = get_fn + "()";
                    else if (!prop->get.empty())
                      sarg = sarg + "_get()";
                  }

                params.push_back(sarg);
			        }

			      std::vector<str>::reverse_iterator pit = params.rbegin();
			      std::vector<str>::reverse_iterator pnd = params.rend();
			      int i = 0;
			      for(; pit != pnd; pit++, i++)
			        {
  				      result << *pit;
                if (i < args - 1)
                  {
                    result << ", ";
                  }
              }

            result << ")";

            push_rendered(result.str(), op_prec, variant()); //td: we could find out the type here or something
            break;
          }

        case op_call:
          {
            std::stringstream result;

            result << "(";

            int args = arg1;

            //pop the arguments
	      		std::vector<str> params;
            for(int i = 0; i < args; i++)
              {
                variant arg  = stack_.top(); stack_.pop();
                str     sarg = operand_to_string(arg);

                XSSProperty prop = get_property(arg);
                if (prop)
                  {
                    str get_fn = variant_cast<str>(dynamic_get(prop, "get_fn"), "");
                    if (!get_fn.empty())
                      sarg = get_fn + "()";
                    else if (!prop->get.empty())
                      sarg = sarg + "_get()";
                  }

                params.push_back(sarg);
			        }

			      std::vector<str>::reverse_iterator pit = params.rbegin();
			      std::vector<str>::reverse_iterator pnd = params.rend();
			      int i = 0;
			      for(; pit != pnd; pit++, i++)
			        {
  				      result << *pit;
                if (i < args - 1)
                  {
                    result << ", ";
                  }
              }

            result << ")";

            variant top = stack_.top(); stack_.pop();
            str caller = operand_to_string(top);

            push_rendered(caller + result.str(), op_prec, variant()); //td: we could find out the type here or something
            break;
          }

        case op_parameter:
          {
            //do nothing, this is just a notification
            break;
          }

        case op_array:
          {
            std::stringstream result;

            result << "[";

						std::vector<str> params;
						int arg_count = arg1;
            for(int i = 0; i < arg_count; i++)
							{
								variant opnd = stack_.top(); stack_.pop();
								params.push_back( operand_to_string(opnd) );
							}

						std::vector<str>::reverse_iterator arit = params.rbegin();
						std::vector<str>::reverse_iterator arnd = params.rend();
						for(; arit != arnd; arit++)
							{
								result << *arit;
								if (arit + 1 != arnd)
									result << ", ";
							}

            result << "]";
            push_rendered(result.str(), op_prec, variant());
						break;
          }

        case op_index:
          {
            std::stringstream result;
						result << operand_to_string(arg1) << "[" << operand_to_string(arg2) << "]";
            push_rendered(result.str(), op_prec, variant());
            break;
          }

				default:
          assert(false); //td:
      }
  }

str expression_renderer::get()
  {
    if(stack_.empty())
      {
        param_list error;
        error.add("id", SEmptyStack);
        error.add("desc", SEmptyExpression);
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
				xss_idiom* idiom = ctx_->idiom_;
        str separator = idiom->resolve_separator();

				expression_identifier ei = result;
				if (assigner)
					{
						if (ctx_->has_var(ei.value))
							return ei.value;

						XSSObject this_ = variant_cast<XSSObject>(ctx_->this_, XSSObject());
						str ass = resolve_assigner(ei, this_, assigner);

						str result = idiom->resolve_this(ctx_);
						if (result.empty())
							return ass;

						return result + separator + ass;
					}

				//test the this pointer
        XSSProperty prop = ctx_->get_property(ei.value);
				XSSMethod mthd = ctx_->get_method(ei.value);
        if (prop)
          {
            str this_str = ctx_->idiom_->resolve_this(ctx_);
            if (!this_str.empty())
              {
                if (prop->has("property_xss"))
                  {
                    capture_property_.prop = prop;
                    capture_property_.xss  = variant_cast<str>(dynamic_get(prop, "property_xss"), str());

                    for(int i = 0; i < capture_property_.xss.size(); i++)
                      {
                        if (capture_property_.xss[i] == '\'')
                          capture_property_.xss[i] ='"';
                      }

                    push_rendered(this_str, 0, prop);
                    return render_captured_property();
                  }

                return this_str + separator + ei.value;
						}
					}
				else if (mthd)
					{
            str this_str = ctx_->idiom_->resolve_this(ctx_);
            if (!this_str.empty())            str this_str = ctx_->idiom_->resolve_this(ctx_);
						if (!this_str.empty())
              return this_str + separator + ei.value;
					}

				return ei.value;
			}
    else if (result.is<str>())
      {
        str res = result;
        str ss = '"' + res + '"';
        return ss;
      }
		else if (result.is<bool>())
			{
				bool rr = result;
				if (rr)
					return "true";
				else
					return "false";
			}

    str to_string = result;
    return to_string;
  }

str expression_renderer::operand_to_string(variant operand, XSSObject parent, int* prec)
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
                      result = this_str + separator + result; //otherwise it doesnt get translated
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

str expression_renderer::array_operation(str lopnd, str ropnd, operator_type op)
  {
    return lopnd + " " + idiom_utils::get_operator_str(op) + " " + ropnd;
  }

void expression_renderer::push_rendered(str value, int prec, variant object)
  {
    already_rendered ar;
    ar.object = object;
    ar.value = value;
    ar.precedence = prec;

    stack_.push(ar);
  }

//idiom_utils
str idiom_utils::get_operator_str(operator_type op)
  {
    return str(operator_str[op]);
  }

int idiom_utils::get_operator_prec(operator_type op)
  {
    return operator_prec[op];
  }

str idiom_utils::expr2str(expression& expr, XSSContext ctx)
	{
    return render_expression<expression_renderer>(expr, ctx);
	}

//expr_type_resolver
expr_type_resolver::expr_type_resolver(XSSContext ctx)
  : is_variant_(false), local_(ctx->vars_), ctx_(ctx)
  {
  }

xs_type_info expr_type_resolver::get()
	{
		if (is_variant_)
			return null;

		assert(stack_.size() <= 1);

    if (stack_.size() == 1)
		  return resolve_type(stack_.top());
    else
      return xs_type_info(type_schema<variant>());
	}

str expr_type_resolver::type_name()
	{
		assert(stack_.size() == 1);

		xs_type_info ti = resolve_type(stack_.top());
    if (ti.type)
      return ctx_->get_type_name(ti.type);
		else
			{
				XSSObject obj = resolve_xss_type(stack_.top());
				if (obj)
					{
						if (obj->has("internal_id"))
							return variant_cast<str>(dynamic_get(obj, "internal_id"), str());

						if (obj->has("id"))
							return variant_cast<str>(dynamic_get(obj, "id"), str());
					}
			}

		return "";
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
						stack_.push(xs_type_info(resolve_type(top)));
						break;
					}

				case op_not:
				case op_typecheck:
				case op_namecheck:
					{
						stack_.pop();
						stack_.push(xs_type_info(type_schema<bool>()));
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
						stack_.push(xs_type_info(type_schema<bool>()));
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
            error.add("id", SIdiom);
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

            xs_type_info result(type_schema<variant>());
            xs_type_info xsti = resolve_type(arg2);

            if (xsti.is_array)
              result.type = xsti.array_type;

            stack_.push(result);
						break;
					}

				case op_array:
          {
            int arg_count = stack_.top(); stack_.pop();

            //verify array type from the type of there elements
            schema *arr_type = null;
            for(int i = 0; i < arg_count; i++)
							{
								variant opnd = stack_.top(); stack_.pop();
								xs_type_info xsti = resolve_type(opnd);
                if (arr_type != xsti.type)
                  {
                    if (arr_type)
                      arr_type = type_schema<variant>();
                    else
                      arr_type = xsti.type;
                  }
							}

            xs_type_info result;
            result.type = type_schema<DynamicArray>();
            result.is_array = true;
            result.array_type = arr_type;

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

						xs_type_info xstype1 = resolve_type(arg1);
						xs_type_info xstype2 = resolve_type(arg2);
						schema* result = null;
						size_t  idx;

            // i don't imagine how it's operations work with array operands
            if (operators_.get_operator_index(op, xstype1.type, xstype2.type, idx, &result) && result)
							stack_.push(xs_type_info(result));
						else
							is_variant_ = true;
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

            xs_type_info xsti;
            XSSMethod mthd = ctx_->get_method(ei.value);
            if (mthd)
              {
                schema* result = ctx_->get_type(mthd->type);
						    if (result)
                  xsti.type = result;
						    else
                  xsti = ctx_->get_xss_type(mthd->type);
              }

            stack_.push(xsti);
            break;
          }

				case op_dot:
					{
						expression_identifier arg1  = stack_.top(); stack_.pop();
						expression_identifier arg2  = stack_.top(); stack_.pop();

						XSSObject obj	 = resolve_object(arg2);
						if (!obj)
							obj	 = resolve_xss_type(arg2);

						xs_type_info xsti;
            if (obj)
              {
								XSSProperty prop = obj->get_property(arg1.value);
                if (prop)
                  {
										schema* result = ctx_->get_type(prop->type);
										if (result)
											xsti.type = result;
										else
											{
                        xsti = ctx_->get_xss_type(prop->type);
												if (!xsti.object && obj->has("enum"))
													xsti.object = obj;
											}
                  }
              }

            stack_.push(xsti);
            break;
					}

				case op_dot_call:
          {
						expression_identifier arg1  = stack_.top(); stack_.pop();
						expression_identifier arg2  = stack_.top(); stack_.pop();

						XSSObject obj	 = resolve_object(arg2);
						if (!obj)
							obj	 = resolve_xss_type(arg2);

						xs_type_info xsti;
            if (obj)
              {
                XSSMethod mthd = obj->get_method(arg1.value);
                if (mthd)
                  {
										schema* result = ctx_->get_type(mthd->type);
										if (result)
											xsti.type = result;
										else
											{
                        xsti = ctx_->get_xss_type(mthd->type);
												if (!xsti.object && obj->has("enum"))
													xsti.object = obj;
											}
                  }
              }

            stack_.push(xsti);
            break;
          }
			} // switch
	}

XSSObject expr_type_resolver::resolve_xss_type(variant var)
	{
		if (var.is<XSSObject>())
			return var;
		else if (var.is<expression_identifier>())
			{
				expression_identifier ei = var;
				XSSObject		obj = ctx_->resolve_instance(ei.value);
				XSSProperty	prop;
				if (obj)
					{
						return obj->type();
					}
				else if (prop = ctx_->get_property(ei.value))
					{
            return ctx_->get_xss_type(prop->type).object;
					}

        return ctx_->get_xss_type(ei.value).object; //td: variables
			}

		return XSSObject();
	}

xs_type_info expr_type_resolver::resolve_type(variant var)
	{
		xs_type_info result;

		if (var.is<xs_type_info>())
			return var;
		else if (var.is<expression_identifier>())
			{
				expression_identifier ei = var;
				if (!resolve_variable(ei.value, result))
					{
						XSSObject		obj = ctx_->resolve_instance(ei.value);
						XSSProperty	prop;
						if (obj)
							{
                result.type = type_schema<XSSObject>();
							}
						else if (prop = ctx_->get_property(ei.value))
							{
                result.type = ctx_->get_type(prop->type);
							}
					}
			}
		else if (!var.is<XSSObject>())
      result.type = var.get_schema();

		return result;
	}

bool expr_type_resolver::resolve_variable(const str& id, xs_type_info &xstype)
	{
		local_variables::iterator it = local_.find(id);
		if (it != local_.end())
			{
        xstype = it->second;
				return true;
			}
		return false;
	}

XSSObject	expr_type_resolver::resolve_object(const variant v)
	{
		if (v.is<XSSObject>())
			return v;
		else if (v.is<expression_identifier>())
			{
				expression_identifier ei = v;
				return ctx_->resolve_instance(ei.value);
			}
		else
			assert(false); //use case

		return XSSObject();
	}

variant expr_type_resolver::top_stack()
  {
    assert(stack_.size() == 1);

    return stack_.top();
  }

//code_type_resolver
xs_type_info code_type_resolver::get()
	{
		if (is_variant_)
			return null;

    xs_type_info result_;
    local_variables::iterator it = vars_.find("@@result_");
    if (it != vars_.end())
      result_ = it->second;

		return result_;
	}

void code_type_resolver::variable_(stmt_variable& info)
	{
    xs_type_info xsti;
    xsti.type = ctx_->get_type(info.type);
		if (!xsti.type)
			{
				if (!info.value.empty())
					{
						expr_type_resolver typer(ctx_);
						info.value.visit(&typer);

            variant ty_top = typer.top_stack();

            if (ty_top.is<xs_type_info>())
              xsti = ty_top;
            else
              xsti = typer.get();
					}
			}
    else if (xsti.type->options() & TYPE_ITERATED)
      {
        xsti.is_array = true;
        xsti.array_type = type_schema<variant>();

        if (!info.value.empty())
          {
            expr_type_resolver typer(ctx_);
            info.value.visit(&typer);

            xsti = typer.get();
          }
      }

    register_var(info.id, xsti, info.type);
	}

void code_type_resolver::return_(stmt_return& info)
	{
		if (is_variant_)
			return;

		expr_type_resolver typer(ctx_);
		info.expr.visit(&typer);

		xs_type_info xstype = typer.get();
    if (!xstype.type)
			{
				is_variant_ = true;
				return;
			}

    xs_type_info result_;
    local_variables::iterator it = vars_.find("@@result_");
    if (it != vars_.end())
      result_ = it->second;

    if (!result_.type)
      {
			  result_ = xstype;
        vars_["@@result_"] = result_;
      }
    else if (result_.type != xstype.type)
			{
				is_variant_ = true;
				return;
			}
	}

void code_type_resolver::expression_(xkp::stmt_expression &info)
  {
    expression_type_resolver(info.expr, ctx_);
  }

void code_type_resolver::if_(stmt_if& info)
  {
    expression_type_resolver(info.expr, ctx_);

    info.if_code.visit(this);

    if (!info.else_code.empty())
			{
        info.else_code.visit(this);
			}
  }

void code_type_resolver::for_(stmt_for& info)
  {
    variable_(info.init_variable);

    expression_type_resolver(info.init_expr, ctx_);
    expression_type_resolver(info.cond_expr, ctx_);
    expression_type_resolver(info.iter_expr, ctx_);

    info.for_code.visit(this);
  }

void code_type_resolver::iterfor_(stmt_iter_for& info)
  {
    register_var(info.id, xs_type_info(ctx_->get_type(info.type.name)), info.type.name);
    expression_type_resolver(info.iter_expr, ctx_);

    info.for_code.visit(this);
  }

void code_type_resolver::while_(stmt_while& info)
	{
		expression_type_resolver(info.expr, ctx_);

		info.while_code.visit(this);
	}

void code_type_resolver::register_var(const str& name, xs_type_info type, const str& str_type)
	{
    std::pair < local_variables::iterator, bool > ret;

    if (!str_type.empty())
      {
		    XSSProject project = ctx_->project_;
        type.object        = project->get_class(str_type);
      }

    ret = vars_.insert(std::pair<str, xs_type_info>(name, type));
    if (!ret.second)
      {
        param_list error;
        error.add("id", SDeclaration);
        error.add("desc", SDuplicateVariable);
        error.add("variable", name);

        xss_throw(error);
      }
	}

void code_type_resolver::update_type(const str& name, xs_type_info type)
  {
		local_variables::iterator it = vars_.find(name);
		if (it != vars_.end())
			{
				it->second = type;
			}
    else
      {
        param_list error;
        error.add("id", SCannotResolve);
        error.add("desc", SUnknownVariable);
        error.add("variable", name);

        xss_throw(error);
      }
  }

void code_type_resolver::expression_type_resolver(xkp::expression &expr, xkp::XSSContext ctx)
  {
    operator_type op;
    if (expr.top_operator(op))
	    {
		    switch(op)
			    {
				    case op_assign:
            case op_plus_equal:
            case op_minus_equal:
            case op_mult_equal:
            case op_div_equal:
            case op_shift_right_equal:
            case op_shift_left_equal:
					    {
						    expression_splitter es(op);
						    expr.visit(&es);

                //resolve type of right expression
                expr_type_resolver right_type(ctx);
                es.right.visit(&right_type);

                xs_type_info right_tinfo = right_type.get();

                //resolve type of left expression
                expr_type_resolver left_type(ctx);
                es.left.visit(&left_type);

                xs_type_info left_tinfo = left_type.get();

                //i assume that first operand is an expression identifier
                variant left_var = es.left.pop_first();

                str var_name;
                if (left_var.is<expression_identifier>())
                  {
                    expression_identifier ei = left_var;
                    var_name = ei.value;
                  }
                else
                  {
                    str opndstr = left_var;
                    var_name = opndstr;
                  }

                if (!right_tinfo.type)
                  break;

                //somes stupid tipified use case
                if (left_tinfo.type != right_tinfo.type)
                  {
                    if (left_tinfo.type)
                      {
                        if (left_tinfo.is_array && left_tinfo.array_type != right_tinfo.type)
                          {
                            right_tinfo = left_tinfo;
                            right_tinfo.array_type = type_schema<variant>();

                            update_type(var_name, right_tinfo);
                          }
                        else if (!left_tinfo.is_array &&
                          !ctx->getIdiom()->allow_cast(left_tinfo.type, right_tinfo.type))
                          {
                            param_list error;
                            error.add("id", SVariableAssigner);
                            error.add("desc", SMultipleTypeVariable);
                            error.add("variable", var_name);
                            error.add("origin type", ctx->get_type_name(left_tinfo.type));
                            error.add("new type", ctx->get_type_name(right_tinfo.type));

                            xss_throw(error);
                          }
                      }
                    else if (!var_name.empty())
                      {
                        update_type(var_name, right_tinfo);
                      }
                  }
                else if (left_tinfo.is_array && right_tinfo.is_array)
                  {
                    if (left_tinfo.array_type != right_tinfo.array_type)
                      {
                        xs_type_info xsti = left_tinfo;
                        xsti.array_type = type_schema<variant>();

                        update_type(var_name, xsti);
                      }
                  }
					    }
            default: break;
			    } //switch
	    }
  }

//base_xss_expression
base_xss_expression::base_xss_expression()
  {
  }

base_xss_expression::base_xss_expression(XSSContext ctx, expression& expr):
  expr_(expr),
  ctx_(ctx)
  {
  }

str base_xss_expression::generate(const param_list pl)
  {
    return idiom_utils::render_expression<expression_renderer>(expr_, ctx_);
  }

//base_xss_code
base_xss_code::base_xss_code() : indent_(0)
  {
  }

base_xss_code::base_xss_code(XSSContext ctx, code& code):
  vars_(ctx->vars_),
  code_(code),
  ctx_(ctx),
	indent_(0)
  {
  }

str base_xss_code::generate(const param_list pl)
  {
		result_ = ""; //td: cache

    code_.visit(this);

    return result_;
  }

void base_xss_code::if_(stmt_if& info)
	{
    std::stringstream ss;
    str ind = get_indent_str();

    add_line("if (" + render_expression(info.expr, ctx_) + ")", true);
    add_line("{", true);
			render_code(info.if_code, indent_ + 1);
		add_line("}", true);

    if (!info.else_code.empty())
			{
				add_line("else", true);
				add_line("{", true);
					render_code(info.else_code, indent_ + 1);
				add_line("}", true);
			}

    add_line(ss.str());
	}

void base_xss_code::register_var(const str& name, const str& type)
	{
    xs_type_info xsti;
		XSSProject project	= ctx_->project_;
    xsti.object         = project->get_class(type);
		ctx_->register_variable(name, xsti);
	}

void base_xss_code::variable_(stmt_variable& info)
	{
		register_var(info.id, info.type); //td: resolve type from value
    std::stringstream ss;
    str ind = get_indent_str();

    ss << ind << "var " << info.id;
    if (!info.value.empty())
      ss << " = " << render_expression(info.value, ctx_);

    ss << ";\n";

    add_line(ss.str());
	}

void base_xss_code::for_(stmt_for& info)
	{
		register_var(info.init_variable.id, info.init_variable.type);

    std::stringstream ss;

    ss << "for(var " << info.init_variable.id << " = " << render_expression(info.init_variable.value, ctx_)
        << "; " << render_expression(info.cond_expr, ctx_)
        << "; " << render_expression(info.iter_expr, ctx_) << ")";

    add_line(ss.str(), true);
    add_line("{", true);
			render_code(info.for_code, indent_ + 1);
		add_line("}", true);
	}

void base_xss_code::iterfor_(stmt_iter_for& info)
	{
		register_var(info.id, info.type.name);

		//render
    std::stringstream ss;
    str ind = get_indent_str();

    str iterable_name = info.id + "_iterable";
    str iterator_name = info.id + "_iterator";
    ss << ind << "var " << iterable_name << " = " << render_expression(info.iter_expr, ctx_) << ";\n";
    ss << ind << "for(var " << iterator_name << " = 0; "
        << iterator_name << " < " << iterable_name << ".length; "
        << iterator_name << "++" << ")\n";

		add_line(ss.str());
		add_line("{", true);
		add_line("  var " + info.id + " = " + iterable_name + "[" + iterator_name + "];", true);
			render_code(info.for_code, indent_ + 1);
		add_line("}", true);
	}

void base_xss_code::while_(stmt_while& info)
	{
    add_line("while(" + render_expression(info.expr, ctx_) + ")", true);
		add_line("{", true);
			render_code(info.while_code, indent_ + 1);
		add_line("}", true);
	}

void base_xss_code::break_()
	{
		add_line("break;", true);
	}

void base_xss_code::continue_()
	{
		add_line("continue;", true);
	}

void base_xss_code::return_(stmt_return& info)
	{
    if (info.expr.empty())
      add_line("return;", true);
    else
      add_line("return " + render_expression(info.expr, ctx_) + ";", true);
	}

void base_xss_code::expression_(stmt_expression& info)
	{
    str value = render_expression(info.expr, ctx_);
    add_line(value + ";", true);
	}

void base_xss_code::dsl_(dsl& info)
	{
		assert(false); //td: there is some stuff to implement here... later
	}

void base_xss_code::dispatch(stmt_dispatch& info)
	{
    assert(false); //td: ought to define what to do here, it would seem like the idiom would like
                    //to handle this
	}

void base_xss_code::add_line(const str& line, bool dress_line)
  {
    if (dress_line)
      result_ += get_indent_str();

    result_ += line;

    if (dress_line)
      result_ += '\n';
  }

str base_xss_code::get_indent_str()
  {
    str result;
    for(int i = 0; i < indent_*4; i++)
      {
        result += ' ';
      }

    return result;
  }

void base_xss_code::render_code(code& cde, int indent)
	{
		int old_indent = indent_;
		indent_ = indent;

		cde.visit(this);

		indent_ = old_indent;
	}

str base_xss_code::render_expression(expression& expr, XSSContext ctx)
  {
    return idiom_utils::render_expression<expression_renderer>(expr, ctx);
  }

//base_idiom
base_idiom::base_idiom()
	{
	}

base_idiom::base_idiom(const base_idiom& other):
	ctx_(other.ctx_)
	{
	}

void base_idiom::set_context(XSSContext ctx)
	{
		ctx_ = ctx;
	}

str base_idiom::resolve_separator(XSSObject lh)
	{
		return ".";
	}

str base_idiom::translate_type(const str& type)
	{
		std::map<str, str>::iterator it = types_.find(type);
		if (it != types_.end())
			return it->second;
		return type;
	}

str base_idiom::translate_type(schema *type)
  {
    return ctx_->get_type_name(type);
  }

bool base_idiom::allow_cast(schema* fts_type, schema* sec_type)
  {
    return true;
  }

void base_idiom::setTypes(DynamicArray da)
	{
		for(int i = 0; i < da->size(); i++)
			{
				str ot = variant_cast<str>(dynamic_get(da->at(i), "xs_type"),		  str()); assert(!ot.empty());
				str rt = variant_cast<str>(dynamic_get(da->at(i), "output_type"), str()); assert(!rt.empty());

				types_.insert(std::pair<str, str>(ot, rt));
			}
	}

//base_xss_args
base_xss_args::base_xss_args()
  {
  }

base_xss_args::base_xss_args(const base_xss_args& other):
  args_(other.args_)
  {
  }

base_xss_args::base_xss_args(param_list_decl& args):
  args_(args)
  {
  }

str base_xss_args::generate(const param_list pl)
  {
    std::ostringstream oss;

    param_list_decl::iterator it = args_.begin();
    param_list_decl::iterator nd = args_.end();
    for(; it != nd; it++)
      {
				str param = resolve_param(*it);
        //td: defaults values, etc
        oss << param << ",";
      }

    str result = oss.str();
    if (!result.empty())
      result.erase(result.end() - 1);

    return result;
  }
