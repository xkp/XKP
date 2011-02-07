
#include <xss/idiom.h>
#include <xss/xss_error.h>

#include <fstream>

using namespace xkp;

//strings
const str SEmptyStack("empty-stack");
const str SIdiom("idiom");
const str SInvalidOperator("invalid-operator");

const str SEmptyExpression("Trying to use an empty expression");
const str SEveryInstanceMustHaveId("Trying to use an instance without id");
const str SAssignOperator("Assign operators can only be used as the base of an expression");

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


//detail

struct already_rendered
  {
    already_rendered() : precedence(0)
    {
    }

    str     value;
    variant object;
    int precedence;
  };


enum assign_type
	{
		VANILLA,
		FN_CALL,
		XSS_RESOLVE
	};

struct assign_info
	{
		assign_info() : type(VANILLA), this_(false) {}

		assign_type type;
		str					data;
		bool				this_;
	};

struct expression_renderer : expression_visitor
  {
    expression_renderer(XSSContext ctx, bool id_as_this = false) : ctx_(ctx), assigner(null) {}

    XSSProperty get_property(variant v)
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

    XSSProperty get_property(variant v, const str& name)
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

    XSSObject get_instance(variant v)
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

    XSSObject get_instance(variant v, const str& name)
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

    //expression_visitor
		assign_info* assigner;

		str resolve_assigner(variant operand, XSSObject instance, assign_info* ai)
			{
				XSSProperty prop;
				str					result;

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
						size_t last_dot = result.find_last_of(".");
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
									return result + "." + set_xss;
							}
						else if (!set_fn.empty())
              {
								ai->type = FN_CALL;
								if (result.empty())
									return set_fn;
								else
									return result + "." + set_fn;
              }
            else if (!prop->set.empty())
							{
								ai->type = FN_CALL;
								if (result.empty())
									return prop->name + "_set";
								else
									return result + "." + prop->name + "_set";
							}
          }

				ai->type = VANILLA;
				return operand_to_string(operand, instance, null);
			}

    virtual void push(variant operand, bool top)
      {
				if (top && assigner)
					{
						str ass = resolve_assigner(operand, XSSObject(), assigner);
						push_rendered(ass, 0, operand.get_schema()); 
					}

        stack_.push(operand);
      }

    virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top)
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
						assert(op == op_dot); //I'm sure there are more use cases, but I'll deal with this one exclusively
																	//for now
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
								

                //std::stringstream ss;
                //if (op_prec < prec)
                //  ss << "(" << os << ")" << operator_str[op];
                //else
                //  ss << os << operator_str[op];

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
                XSSObject o1 = get_instance(arg1);
                str           s2 = operand_to_string(arg2, o1);
                XSSObject o2 = get_instance(o1, s2);

                XSSProperty   prop       = get_property(o1, s2);
                bool          has_getter = prop && !prop->get.empty();

                std::stringstream ss;
                ss << operand_to_string(arg1);
                
								if (top && assigner)
									{
										ss << "." << resolve_assigner(arg2, o1, assigner);	
									}
								else if (prop)
                  {
                    str get_fn = variant_cast<str>(dynamic_get(prop, "get_fn"), ""); 
                    if (!get_fn.empty())
                      ss << "." << get_fn << "()";
                    else if (has_getter)
                      ss << "." << s2 << "_get()";
                    else   
                      ss << "." << s2;
                  }
                else 
                   ss << "." << s2;

                if (prop)
                  push_rendered(ss.str(), op_prec, prop);
                else
                  push_rendered(ss.str(), op_prec, o2);
                break;
              }

            case op_dot_call:
              {
                std::stringstream ss;
                ss << operand_to_string(arg1) << "." << operand_to_string(arg2);
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
                for(int i = 0; i < args; i++)
                  {
                    variant arg  = stack_.top(); stack_.pop();
                    str     sarg = operand_to_string(arg);
                    
                    XSSProperty prop = get_property(arg);
                    if (prop)
                      {
                        str get_fn = variant_cast<str>(dynamic_get(prop, "get_fn"), ""); 
                        if (!get_fn.empty())
                          result << get_fn << "()";
                        else if (!prop->get.empty())
                          result << sarg << "_get()";
                        else   
                          result << sarg;
                      }
                    else 
                       result << sarg;

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
                for(int i = 0; i < args; i++)
                  {
                    variant arg  = stack_.top(); stack_.pop();
                    str     sarg = operand_to_string(arg);
                    
                    XSSProperty prop = get_property(arg);
                    if (prop)
                      {
                        str get_fn = variant_cast<str>(dynamic_get(prop, "get_fn"), ""); 
                        if (!get_fn.empty())
                          result << get_fn << "()";
                        else if (!prop->get.empty())
                          result << sarg << "_get()";
                        else   
                          result << sarg;
                      }
                    else 
                       result << sarg;

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

								std::vector<str>::reverse_iterator ait = params.rbegin();
								std::vector<str>::reverse_iterator and = params.rend();
								for(; ait != and; ait++)
									{
										result << *ait;
										if (ait + 1 != and)
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

    str get()
      {
        if(stack_.empty())
          {
            param_list error;
            error.add("id", SEmptyStack);
            error.add("desc", SEmptyExpression);
            xss_throw(error);
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
								XSSObject this_ = variant_cast<XSSObject>(ctx_->this_, XSSObject());
								str ass = resolve_assigner(ei, this_, assigner);

								xss_idiom* idiom = ctx_->idiom_;
								str result = idiom->resolve_this(ctx_);
								if (result.empty())
									return ass;

								return result + "." + ass;
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

    protected:
      typedef std::stack<variant> expr_stack;

      expr_stack        stack_;
      std::stringstream result_;
      XSSContext        ctx_;

      str operand_to_string(variant operand, XSSObject parent = XSSObject(), int* prec = null)
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
                      if (prop)
                        {
                          //well, lets see how the idiom handles thises
                          str this_str = ctx_->idiom_->resolve_this(ctx_);
                          if (!this_str.empty())
                            result = this_str + "." + ei.value; //otherwise it doesnt get translated 
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

      void push_rendered(str value, int prec, variant object)
        {
          already_rendered ar;
          ar.object = object;
          ar.value = value;
          ar.precedence = prec;

          stack_.push(ar);
        }
  };


str render_expression(expression& expr, XSSContext ctx)
  {
		//deal with assigns
		operator_type op;
		if (expr.top_operator(op))
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
								
								expression_renderer value_renderer(ctx); 
								es.right.visit(&value_renderer);

								str value = value_renderer.get();

								//get the assigner
								expression_renderer assign_renderer(ctx);
								assign_info ai;
								assign_renderer.assigner = &ai;

								es.left.visit(&assign_renderer);

								str assign = assign_renderer.get();

								str result;
								switch(ai.type)
									{
										case FN_CALL:
											{
												if (simple_assign)
													result = assign + "(" + value + ")";
												else
													{
														//we need to resolve the getter as well for *= operators
														expression_renderer getter(ctx);
														es.left.visit(&getter);

														//this is lame
														str op_str = operator_str[op];
														
														assert(op_str.size() > 1);
														op_str.erase(op_str.end() - 1);

														result = assign + "(" + getter.get() + " " + op_str + " " + value + ")";
													}
												break;
											}

										case VANILLA:
											{
												result = assign + " " + operator_str[op] + " " + value;
												break;
											}
										case XSS_RESOLVE:
											{
												expression_renderer getter_(ctx);
												es.left.visit(&getter_);
												str getter = getter_.get();

												if (!simple_assign)
													{
														str op_str = operator_str[op];
														assert(op_str.size() > 1);
														op_str.erase(op_str.end() - 1);

														value = getter + " " + op_str + " " + value; 
													}

												size_t last_dot = getter.find_last_of(".");
												if (last_dot != str::npos)
													{
														size_t count = getter.size() - last_dot;
														getter.erase(getter.end() - count, getter.end());
													}
												else
													getter = "this";

												//replace quotes, life is hard
												for(size_t i = 0; i < assign.size(); i++)
													{
														if (assign[i] == 39)
															assign[i] = '"';
													}
												
												//td: is all this really neccesary?
												XSSProject project_ = ctx->project_;
												xss_idiom* idiom_		= ctx->idiom_;
												XSSContext context(new xss_code_context(project_, idiom_));
												xss_code_context& ctx = *context.get();

												XSSGenerator gen(new xss_generator(context));
												project_->push_generator(gen);

												project_->prepare_context(ctx, gen);

												ctx.scope_->register_symbol("value",	value);
												ctx.scope_->register_symbol("object", getter);
												
												result = project_->generate_xss(assign, gen);

												project_->pop_generator();
												break;
											}
										default:
											assert(false); //trap use cases
									}
								
								return result;
							}
					}
			}

    expression_renderer er(ctx);
    expr.visit(&er);

    return er.get();
  }

str idiom_utils::expr2str(expression& expr, XSSContext ctx)
	{
		return render_expression(expr, ctx);
	}

struct expr_type_resolver : expression_visitor
	{
		typedef std::map<str, schema*> local_variables;

		expr_type_resolver(local_variables& local_vars, XSSContext ctx) : 
			is_variant_(false),
			local_(local_vars),
			ctx_(ctx)
			{
			}
		
		schema* get()
			{
				if (is_variant_)
					return null;

				assert(stack_.size() == 1);
				return resolve_type(stack_.top());
			}

		//expression_visitor
		virtual void push(variant operand, bool top)
			{
				stack_.push(operand);
			}

		virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top)
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
								stack_.push(type_schema<bool>());
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
								stack_.push(type_schema<bool>());
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

						case op_call:
						case op_func_call:
						case op_array:
						case op_parameter:
						case op_dot_call:
						case op_index:
							{
								//td: warning
								is_variant_ = true;
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

								schema* type1 = resolve_type(arg1);
								schema* type2 = resolve_type(arg2);
								schema* result = null;
								size_t  idx;
								if (operators_.get_operator_index(op, type1, type2, idx, &result) && result)
									stack_.push(result);
								else
									is_variant_ = true;
								break;
							}

						case op_dot:
							{
								XSSObject							obj = resolve_object(stack_.top()); stack_.pop();
								expression_identifier ei  = stack_.top(); stack_.pop();
								break;
							}
					}
			}

		private:
			typedef std::stack<variant>		 expr_stack;
			
			expr_stack				stack_;
			bool							is_variant_;	
			operator_registry operators_;  
			local_variables		local_;
			XSSContext				ctx_;

			schema* resolve_type(variant var)
				{
					schema* result = null;

					if (var.is<schema*>())
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
											result = type_schema<XSSObject>();
										}
									else if (prop = ctx_->get_property(ei.value))
										{
											result = ctx_->get_type(prop->type);
										}
								}
						}
					else
						result = var.get_schema();

					return result;
				}

			bool resolve_variable(const str& id, schema* &type)
				{
					local_variables::iterator it = local_.find(id);
					if (it != local_.end())
						{
							type = it->second;
							return true;
						}
					return false;
				}

			XSSObject	resolve_object(const variant v)
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
	};

//code_type_resolver
schema* code_type_resolver::get()
	{
		if (is_variant_)
			return null;

		return result_;
	}

void code_type_resolver::variable_(stmt_variable& info)     
	{
		schema* type = ctx_->get_type(info.type);
		if (!type)
			{
				if (!info.value.empty())
					{
						expr_type_resolver typer(vars_, ctx_);
						info.value.visit(&typer);

						type = typer.get();
					}
			}

		vars_.insert(std::pair<str, schema*>(info.id, type));
	}

void code_type_resolver::return_(stmt_return& info)         
	{
		if (is_variant_)
			return;

		expr_type_resolver typer(vars_, ctx_);
		info.expr.visit(&typer);

		schema* type = typer.get();
		if (!type)
			{
				is_variant_ = true;
				return;
			}

		if (!result_)
			result_ = type;
		else if (result_ != type)
			{
				is_variant_ = true;
				return;
			}
	}

struct code_renderer__ : code_visitor
  {
    code_renderer__(XSSContext ctx, int indent): ctx_(ctx), indent_(indent) {}

    str get()
      {
        return result_;
      }

    //code_visitor
    virtual void if_(stmt_if& info)
      {
        std::stringstream ss;
        str ind = get_indent_str();

        ss  << ind << "if (" << render_expression(info.expr, ctx_) << ")" << '\n'
            << ind << "{" << '\n'
                   << render_code(info.if_code, indent_ + 1) << '\n'
            << ind << "}" << '\n';

        if (!info.else_code.empty())
          ss  << ind << "else\n" 
							<< ind << "{\n"
                << render_code(info.else_code, indent_ + 1)
              << ind << "}\n";

        add_line(ss.str());
      }

    virtual void variable_(stmt_variable& info)
      {
        std::stringstream ss;
        str ind = get_indent_str();

        ss << ind << "var " << info.id;
        if (!info.value.empty())
          ss << " = " << render_expression(info.value, ctx_);

        ss << ";\n";

        add_line(ss.str());
      }

    virtual void for_(stmt_for& info)
      {
        std::stringstream ss;
        str ind = get_indent_str();

        ss << ind << "for(var " << info.init_variable.id << " = " << render_expression(info.init_variable.value, ctx_)
           << "; " << render_expression(info.cond_expr, ctx_)
           << "; " << render_expression(info.iter_expr, ctx_) << ")\n";

        ss << ind << "{" << "\n"
                  << render_code(info.for_code, indent_ + 1);
        ss << ind << "}" << "\n";

        add_line(ss.str());
      }

    virtual void iterfor_(stmt_iter_for& info)
      {
        std::stringstream ss;
        str ind = get_indent_str();

        str iterable_name = info.id + "_iterable";
        str iterator_name = info.id + "_iterator";
        ss << ind << "var " << iterable_name << " = " << render_expression(info.iter_expr, ctx_) << "\n";
        ss << ind << "for(var " << iterator_name << " = 0; " 
           << iterator_name << " < " << iterable_name << ".length; "
           << iterator_name << "++" << ")\n";

        ss << ind << "{" << "\n"
                  <<    "var " << info.id << " = " << iterable_name << "[" << iterator_name << "];\n"
                  <<    render_code(info.for_code, indent_ + 1);
        ss << ind << "}" << "\n";

        add_line(ss.str());
      }

    virtual void while_(stmt_while& info)
      {
        std::stringstream ss;
        str ind = get_indent_str();

        ss << ind << "while(" << render_expression(info.expr, ctx_) << ")\n" ;
        ss << ind << "{" << "\n"
                  <<    render_code(info.while_code, indent_ + 1);
        ss << ind << "}" << "\n";

        add_line(ss.str());
      }

    virtual void break_()
      {
        add_line("break;", true);
      }

    virtual void continue_()
      {
        add_line("continue;", true);
      }

    virtual void return_(stmt_return& info)
      {
        if (info.expr.empty())
          add_line("return;", true);
        else
          add_line("return " + render_expression(info.expr, ctx_) + ";", true);
      }

    virtual void expression_(stmt_expression& info)
      {
        str value = render_expression(info.expr, ctx_);
        add_line(value + ";", true);
      }

    virtual void dsl_(dsl& info)
      {
        assert(false); //td: there is some stuff to implement here... later
      }

    virtual void dispatch(stmt_dispatch& info)
      {
        assert(false); //td: ought to define what to do here, it would seem like the idiom would like
                       //to handle this 
      }
    private:
      str        result_;
      XSSContext ctx_;
      int        indent_;

      void add_line(const str& line, bool dress_line = false)
        {
          if (dress_line)
            result_ += get_indent_str();

          result_ += line;

          if (dress_line)
            result_ += '\n';
        }

      str render_code(code& code, int indent)
        {
          code_renderer__ renderer(ctx_, indent);
          code.visit(&renderer);

          return renderer.get();
        }

      str get_indent_str()
        {
          str result;
          for(int i = 0; i < indent_*4; i++)
            {
              result += ' ';
            }

          return result;
        }
  };

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
    return render_expression(expr_, ctx_);
  }

//base_xss_code
base_xss_code::base_xss_code() : indent_(0)
  {
  }

base_xss_code::base_xss_code(XSSContext ctx, code& code):
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

void base_xss_code::variable_(stmt_variable& info)
	{
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
    std::stringstream ss;
    str ind = get_indent_str();

    str iterable_name = info.id + "_iterable";
    str iterator_name = info.id + "_iterator";
    ss << ind << "var " << iterable_name << " = " << render_expression(info.iter_expr, ctx_) << "\n";
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
