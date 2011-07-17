
#include <xss/language.h>
#include <xss/xss_error.h>

using namespace xkp;

const str SLinker("linker");

const str SAssignOperator("Assign operators can only be used as the base of an expression");
const str SCannontResolveOperator("Cannot resolve operator");
const str SCannontResolve("Unknown identifier");
const str SCannotCast("Cannot find a suitable cast");
const str SIncompatibleIterator("Cannot find a suitable cast for iterator");
const str SInconsistenReturnType("Inconsisten return type");
const str SNotAnIterator("Expecting an iterable type");
const str SUnknownType("Cannot resolve type");

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
code_type_resolver::code_type_resolver(XSSContext ctx):
  ctx_(new xss_context(ctx))
  {
  }

XSSType code_type_resolver::get()
  {
		if (is_variant_)
			return ctx_->get_type("var");

		return result_;
  }

void code_type_resolver::variable_(stmt_variable& info)
  {
    XSSType xsti = ctx_->get_type(info.type);
    if (!xsti)
      { 
        param_list error;
        error.add("id", SLinker);
        error.add("desc", SUnknownType);
        error.add("type", info.type);
        xss_throw(error);
      }

		if (xsti->is_variant())
			{
				if (!info.value.empty())
					{
						expr_type_resolver typer(ctx_);
						info.value.visit(&typer);

            xsti = typer.get();    
					}

        if (xsti->is_variant())
          var_vars_.push_back(info.id);
			}
    else if (xsti->is_array())
      {
        if (!info.value.empty())
          {
            expr_type_resolver typer(ctx_);
            info.value.visit(&typer);

            xsti = typer.get();
          }
      }

		if (!xsti->is_variant())
      register_var(info.id, xsti);
  }

void code_type_resolver::return_(stmt_return& info)
  {
		if (is_variant_)
			return;

		expr_type_resolver typer(ctx_);
		info.expr.visit(&typer);

		XSSType xstype = typer.get();
    if (xstype)
      return_type_found(xstype);
  }

void code_type_resolver::if_(stmt_if& info)
  {
    code_type_resolver if_typer(ctx_);
    info.if_code.visit(&if_typer);

    XSSType result = lang_utils::code_type(info.if_code, ctx_);
    if (result)
      return_type_found(result);

    if (!info.else_code.empty())
			{
        result = lang_utils::code_type(info.else_code, ctx_);
        if (result)
          return_type_found(result);
			}
  }

void code_type_resolver::for_(stmt_for& info)
  {
    code_type_resolver typer(ctx_);
    typer.variable_(info.init_variable);

    info.for_code.visit(&typer);
    XSSType result = typer.get();
    if (result)
      return_type_found(result);
  }

void code_type_resolver::iterfor_(stmt_iter_for& info)
  {
    XSSType var_type = ctx_->get_type(info.type.name); //td: use 
    if (!var_type)
      {
        param_list error;
        error.add("id", SLinker);
        error.add("desc", SUnknownType);
        error.add("type", info.type);
        xss_throw(error);
      }

    XSSType iterator_type = lang_utils::expr_type(info.iter_expr, ctx_);
    if (!iterator_type->is_array())
      {
        param_list error;
        error.add("id", SLinker);
        error.add("desc", SNotAnIterator);
        error.add("type", iterator_type->id());
        xss_throw(error);
      }
    
    if (var_type->is_variant())
      {
        var_type = iterator_type->array_type();
      }
    else
      {
        if (var_type != iterator_type->array_type())
          {
            param_list error;
            error.add("id", SLinker);
            error.add("desc", SIncompatibleIterator);
            error.add("type", iterator_type->id());
            xss_throw(error);
          }
      }
    
    code_type_resolver typer(ctx_);
    typer.register_var(info.id, var_type);

    info.for_code.visit(&typer);

    XSSType result = typer.get();
    if (result)
      return_type_found(result);
  }

void code_type_resolver::while_(stmt_while& info)
  {
    XSSType result = lang_utils::code_type(info.while_code, ctx_);
    if (result)
      return_type_found(result);
  }

void code_type_resolver::break_()
  {
  }

void code_type_resolver::continue_()
  {
  }

void code_type_resolver::expression_(stmt_expression& info)
  {
    XSSType result;

    expression& expr = info.expr;

    operator_type op;
    if (expr.top_operator(op))
	    {
		    switch(op)
			    {
            case op_minus_equal:
            case op_mult_equal:
            case op_div_equal:
            case op_shift_right_equal:
            case op_shift_left_equal:
					    {
                break;
              }

            case op_plus_equal:
              {
						    expression_splitter es(op);
						    expr.visit(&es);

                //resolve type of right expression
                XSSType lt = lang_utils::expr_type(es.left, ctx_);
                XSSType rt = lang_utils::expr_type(es.right, ctx_);

                if (lt->is_array())
                  {
                    XSSType lta = lt->array_type();
                    if (lta != rt)
                      {
                        param_list error;
                        error.add("id", SLinker);
                        error.add("desc", SCannotCast);
                        error.add("left",  lta->id());
                        error.add("right", rt->id());

                        xss_throw(error);
                      }
                  }
                break;
              }

				    case op_assign:
					    {
						    expression_splitter es(op);
						    expr.visit(&es);

                //resolve type of right expression
                XSSType rt = lang_utils::expr_type(es.right, ctx_);

                //check if its assigning to a variable not previously assigned
                variant value;
                if (es.right.is_constant(value))
                  {
                    str var_name = variant_cast<str>(value, str());
                    if (!var_name.empty())
                      {
                        var_list::iterator it = var_vars_.begin();
                        var_list::iterator nd = var_vars_.end();
                        for(; it != nd; it++)
                          {
                            if (*it == var_name)
                              {
                                if (!rt->is_variant())
                                  {
                                    var_vars_.erase(it); //no need to check no mo
                                    register_var(var_name, rt);
                                  }
                                break;
                              }
                          }
                      }
                  }


                //resolve type of left expression
                XSSType lt = lang_utils::expr_type(es.right, ctx_);

                //check types
                if (lt != rt && !lt->is_variant() && !rt->is_variant())
                  {
                    if (ctx_->get_language()->can_cast(lt, rt))
                          {
                            param_list error;
                            error.add("id", SLinker);
                            error.add("desc", SCannotCast);
                            error.add("left",  lt->id());
                            error.add("right", rt->id());

                            xss_throw(error);
                          }
                      }
                  }
					    }
	    }
  }

void code_type_resolver::dsl_(dsl& info)
  {
  }

void code_type_resolver::dispatch(stmt_dispatch& info)
  {
  }

void code_type_resolver::register_var(const str&id, XSSType type)
  {
    ctx_->register_symbol(RESOLVE_VARIABLE, id, type);
  }

void code_type_resolver::return_type_found(XSSType type)
  {
    if (!type->is_variant())
			{
				is_variant_ = true;
				return;
			}

    if (!result_ || result_->is_variant())
      {
        result_ = type;
      }
    else
      {
        if (result_ != type)
          {
            param_list error;
            error.add("id", SLinker);
            error.add("desc", SInconsistenReturnType);
            error.add("code returns", result_->id());
            error.add("trying to return", type->id());
            xss_throw(error);
          }
      }
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
                error.add("op", lang_utils::operator_string(op));
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
            resolve_info left;
            if (ctx_->resolve(arg2.value, left))
              {
                resolve_info right;
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
            resolve_info left;
            if (ctx_->resolve(arg2.value, left))
              {
                resolve_info right;
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

const int operator_precedence[] =
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

str lang_utils::operator_string(operator_type op)
  {
    return str(operator_str[op]);
  }

int lang_utils::operator_prec(operator_type op)
  {
    return operator_precedence[op];
  }

XSSType lang_utils::code_type(code& code, XSSContext ctx)
  {
    code_type_resolver result(ctx);
    code.visit(&result);

    return result.get();
  }

XSSType lang_utils::expr_type(expression& expr, XSSContext ctx)
  {
    expr_type_resolver result(ctx);
    expr.visit(&result);

    return result.get();
  }
