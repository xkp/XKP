
#include <xss/language.h>
#include <xss/xss_error.h>

#include <xss/lang/base.h>

using namespace xkp;

const str SLinker("xss-linker");

const str SAssignOperator("Assign operators can only be used as the base of an expression");
const str SCannontResolveOperator("Cannot resolve operator");
const str SCannontResolve("Unknown identifier");
const str SCannotCast("Cannot find a suitable cast");
const str SIncompatibleIterator("Cannot find a suitable cast for iterator");
const str SInconsistentReturnType("Inconsistent return type");
const str SNotAnIterator("Expecting an iterable type");
const str SUnknownType("Cannot resolve type");
const str SUnknownTypeFromExpression("Cannot deduce type from assigned expression");
const str SInconsistentExpressionType("Inconsistent type of expression assigned");
const str SInvalidAssign("Invalid assign");
const str SCannotResolveAssign("Cannot resolve assign identifier");

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
code_type_resolver::code_type_resolver(XSSContext ctx, code cde):
  cde_(cde),
  is_variant_(false),
  ctx_(new xss_context(ctx))
  {
    Language lang = ctx_->get_language();
    lang->init_context(ctx_);

    lang_utils::var_gatherer(cde_, ctx_);
  }

XSSType code_type_resolver::get()
  {
		if (is_variant_)
			return ctx_->get_type("var");

		return result_;
  }

void code_type_resolver::return_(stmt_return& info)
  {
		if (is_variant_)
			return;

    XSSType result = lang_utils::expr_type(info.expr, ctx_);

    if (result)
      return_type_found(result);
  }

void code_type_resolver::if_(stmt_if& info)
  {
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
    code_type_resolver typer(ctx_, info.for_code);
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

    XSSType lt = var_type;
    XSSType rt = iterator_type->array_type();

    if (lt != rt && !ctx_->get_language()->can_cast(lt, rt))
      {
        param_list error;
        error.add("id", SLinker);
        error.add("desc", SIncompatibleIterator);
        error.add("type", iterator_type->id());
        xss_throw(error);
      }

    XSSType result = lang_utils::code_type(info.for_code, ctx_);

    if (result)
      return_type_found(result);
  }

void code_type_resolver::while_(stmt_while& info)
  {
    XSSType result = lang_utils::code_type(info.while_code, ctx_);
    if (result)
      return_type_found(result);
  }

void code_type_resolver::return_type_found(XSSType type)
  {
    if (type->is_variant())
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
            error.add("desc", SInconsistentReturnType);
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
    Language lang = ctx_->get_language();
    lang->init_context(ctx_);
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
					{
						stack_.pop();
						stack_.push(ctx_->get_type("bool"));
						break;
					}

				case op_typecheck:
				case op_namecheck:
          {
            stack_.pop(); stack_.pop();
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
						stack_.pop(); //pop parameter name
            break;
          }

				case op_index:
					{
						//td: warning
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
                      arr_type = ctx_->get_type("var");
                    else
                      arr_type = opnd_type;
                  }
							}

            XSSType result = arr_type? ctx_->get_array_type(arr_type) : ctx_->get_type("array");
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

            if (!xstype1 || !xstype2)
              {
                is_variant_ = true;
                return;
              }

            //handle native types, those combinable in xs
            if (operators_.get_operator_index(op, xstype1->native_type(), xstype2->native_type(), idx, &result) && result)
              {
							  stack_.push(ctx_->get_type(result));
              }
						else
              {
                is_variant_ = true; //td: fix

                /*
                param_list error;
                error.add("id", SLinker);
                error.add("desc", SCannontResolveOperator);
                error.add("op", lang_utils::operator_string(op));
                error.add("left", xstype1->id());
                error.add("right", xstype2->id());
                xss_throw(error);
                */
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
            XSSType result = ctx_->get_type("var");

						expression_identifier arg1  = stack_.top(); stack_.pop();

            variant top = stack_.top(); stack_.pop();
            resolve_info left;
            if (top.is<expression_identifier>())
              {
                expression_identifier arg2  = top;

                if (!ctx_->resolve(arg2.value, left))
                  is_variant_ = true;
              }
            else if (top.is<XSSType>())
              {
                XSSType type = top;
                left.what = RESOLVE_TYPE;
                left.value = top;
              }

            if (!is_variant_)
              {
                resolve_info right;
                right.left = &left;
                if (ctx_->resolve(arg1.value, right))
                  {
                    result = right.type;
                    if (!result || result->is_variant())
                      is_variant_ = true;
                  }
                else
                  is_variant_ = true;
              }

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
                            XSSMethod mthd = right.value;
                            result = mthd->type();
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

//expr_object_resolver
expr_object_resolver::expr_object_resolver(XSSContext ctx) :
  one_opnd_(true),
  not_found_(false),
  ctx_(ctx)
  {
  }

variant expr_object_resolver::get()
	{
    variant result;
    if (not_found_)  
      return result;

		assert(stack_.size() <= 1);

    if (stack_.size() == 1)
      {
        if (one_opnd_)
          {
            expression_identifier ei = stack_.top();
            resolve_info info;
            if (ctx_->resolve(ei.value, info) && (info.what != RESOLVE_VARIABLE))
              {
                  result = info.value;
              }
            else
              result = ei;
          }
        else
          result = stack_.top();
      }

    return result;
	}

void expr_object_resolver::push(variant operand, bool top)
	{
		stack_.push(operand);
	}

void expr_object_resolver::exec_operator(operator_type op, int pop_count, int push_count, bool top)
	{
    if (not_found_)
      return;

    one_opnd_ = false;
    variant result;

		switch(op)
			{
				case op_dot:
					{
						expression_identifier arg1 = stack_.top(); stack_.pop();
						variant v2 = stack_.top(); stack_.pop();

            resolve_info left;
            bool         found = true; 
            if (v2.is<expression_identifier>())
              {
                expression_identifier arg2 = v2;
                if (!ctx_->resolve(arg2.value, left))
                  {
                    found = false;
                  }
              }
            else if (v2.is<XSSProperty>())
              {
                left.what = RESOLVE_PROPERTY;
                left.value = (XSSProperty)v2;
              }
            else if (v2.is<XSSType>())
              {
                left.what = RESOLVE_TYPE;
                left.value = (XSSType)v2;
              }            
            else 
              {
                found = false;
              }            
            
            if (found)
              {
                resolve_info right;
                right.left = &left;
                if (ctx_->resolve(arg1.value, right))
                  {
                    result = right.value;
                  }
              }
            else not_found_ = true;

            stack_.push(result);
            break;
					}
        case op_index:
          {
            stack_.pop();
            expression_identifier arg2 = stack_.top(); stack_.pop();

            resolve_info left;
            if (ctx_->resolve(arg2.value, left))
              {
                switch (left.what)
                  {
                    case RESOLVE_PROPERTY:
                      {
                        XSSProperty prop      = left.value;
                        XSSType     prop_type = prop->type();
                        
                        if (prop_type->is_array())
                          result = prop_type->array_type();
                        else
                          assert(false); //td: throw
                        break;
                      }
                    case RESOLVE_VARIABLE:
                      {
                        XSSType var_type = left.value;
                        if (var_type->is_array())
                          result = var_type->array_type();
                        else
                          assert(false); //td: throw
                      }
                    default:
                      {
                        assert(false); //use case
                        result = left.value;
                      }
                  }
              }

            stack_.push(result);
            break;
          }

        default:
          {
            //we'll ignore this stuff for the time being
            for(int i = 0; i < pop_count; i++)
              stack_.pop();

            for(int i = 0; i < push_count; i++)
              stack_.push(variant());
          }
      }
  }

//expression_analizer
expression_analizer::expression_analizer():
  is_identifier_(false),
  is_constant_(false),
  is_property_(false),
  this_property_(false),
  first_property_(false)
  {
  }

void expression_analizer::analyze(expression& expr, XSSContext ctx)
  {
    if (expr.size() == 1)
      {
        variant value = expr.pop_first();
        if (value.is<expression_identifier>())
          {
            expression_identifier ei = value;

            is_identifier_ = true;
            identifier_    = ei.value;

            resolve_info ri;
            if (ctx->resolve(identifier_, ri))
              {
                switch (ri.what)
                  {
                    case RESOLVE_PROPERTY:
                      {
                        this_property_ = true;
                        is_property_   = true;
                        property_      = ri.value; 
                        break;
                      }
                  }
              }
          }
        else
          {
            is_constant_ = true;
            constant_ = value;
          }
      }
    else
      {
        operator_type op;
        expr.top_operator(op);

        switch(op)
          {
            case op_dot:
              {
                is_property_ = true;

                expression_splitter es(op);
                expr.visit(&es);

                path_ = es.left;
                
                //resolve 
                expression_identifier first_ei = path_.last();
                first_string_ = first_ei.value; //td: !!! []

                resolve_info fri;
                if (ctx->resolve(first_ei.value, fri))
                  {
                    switch(fri.what)
                      {
                        case RESOLVE_INSTANCE: break; 
                        case RESOLVE_PROPERTY: first_property_ = true; break; 
                        default : assert(false); //catch
                      }
                    first_ = fri.value;
                  }

                expression_identifier ei = es.right.pop_first();
                property_name_ = ei.value; 

                XSSObject   instance = variant_cast<XSSObject>(lang_utils::object_expr(path_, ctx), XSSObject());
                if (instance)
                  property_ = instance->get_property(property_name_);
                else
                  {
                    XSSType path_type = lang_utils::expr_type(path_, ctx);
                    if (path_type)
                      property_ = path_type->get_property(property_name_);
                  }

                //get info on the start of the chain
                expression_identifier vf = expr.first();
                resolve_info ri;
                if (ctx->resolve(vf.value, ri))
                  {
                    if (ri.what == RESOLVE_PROPERTY)
                      this_property_ = true;
                  }
                break;
              }
            default:
              assert(false); //more test cases later
          }
      }
  }

bool expression_analizer::is_property()
  {
    return is_property_;
  }

bool expression_analizer::is_identifier()
  {
    return is_identifier_;
  }

bool expression_analizer::this_property()
  {
    return this_property_;
  }

XSSProperty expression_analizer::get_property()
  {
    return property_;
  }

expression& expression_analizer::get_path()
  {
    return path_;
  }

str expression_analizer::property_name()
  {
    return property_name_;
  }

str expression_analizer::get_identifier()
  {
    return identifier_;
  }

variant expression_analizer::get_first()
  {
    return first_;
  }

str expression_analizer::first_string()
  {
    return first_string_;
  }

bool expression_analizer::first_property()
  {
    return first_property_;
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
    code_type_resolver result(ctx, code);
    code.visit(&result);

    return result.get();
  }

XSSType lang_utils::expr_type(expression& expr, XSSContext ctx)
  {
    expr_type_resolver result(ctx);
    expr.visit(&result);

    return result.get();
  }

void lang_utils::var_gatherer(code& cde, XSSContext ctx)
  {
    variable_gather vg(ctx);
    cde.visit(&vg);
  }

variant lang_utils::object_expr(expression& expr, XSSContext ctx)
  {
    expr_object_resolver result(ctx);
    expr.visit(&result);

    return result.get();
  }

str lang_utils::wind(const std::vector<str> path)
	{
		str result;
		for(size_t i = 0; i < path.size(); i++)
			result += path[i] + '.';

		if (!result.empty())
			result.erase(result.end() - 1);

		return result;
	}

std::vector<str> lang_utils::unwind(const str& path)
	{
		std::vector<str> result;
		size_t pos = 0;
		while(pos < path.size())
			{
				size_t last_pos = pos;
				pos = path.find('.', pos);
				if (pos != str::npos)
					{
						result.push_back(path.substr(last_pos, pos - last_pos));
						pos++;
					}
				else
					{
						result.push_back(path.substr(last_pos, path.size() - last_pos));
					}
			}

		return result;
	}

void lang_utils::invalid_assign()
  {
    param_list error;
    error.add("id", SLinker);
    error.add("desc", SInvalidAssign);

    xss_throw(error);
  }

void lang_utils::unresolved_assigner(const str& id)
  {
    param_list error;
    error.add("id", SLinker);
    error.add("desc", SCannotResolveAssign);
    error.add("identifier", id);

    xss_throw(error);
  }

str lang_utils::check_array_op(operator_type op, XSSProperty prop, XSSType type, const str& path, const str& value, XSSContext ctx)
  {
    Language lang = ctx->get_language();

    if (op == op_plus_equal || op == op_minus_equal)
      {
        if (!type)
          type = prop->type();

        if (type && type->is_array())
          {
            str left;
            if (prop)
              left = lang->property_get(prop, path, ctx);
            else
              left = path;
            return lang->array_operation(op, left, value, ctx);
          }
      }

    return str();
  }

str lang_utils::assign_operator(operator_type op, XSSProperty prop, const str& path, const str& value, XSSContext ctx)
  {
    Language  lang = ctx->get_language();
    XSSType   type = prop->type();
    str       aop  = check_array_op(op, prop, prop->type(), path, value, ctx);
    
    if (!aop.empty())
      return aop;

    str op_str = operator_string(op);
    op_str.erase(op_str.end() - 1);
    str vv = lang->property_get(prop, path, ctx) + " " + op_str + " " + value;
    return lang->property_set(prop, path, vv, ctx);
  }
