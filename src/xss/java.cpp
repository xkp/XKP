
#include <xss/java.h>
#include <xss/xss_error.h>

using namespace xkp;

//strings
const str SJavaIdiom("idiom");
const str SJavaCannotResolve("unkown-identifier");

const str SJavaEveryInstanceMustHaveId("Trying to use an instance without id");
const str SJavaUnknownInstance("Instance not found");

//java_code
str java_code::get_type_name(const str& var_name)
  {
    str str_type;
    std::map<str, xs_type_info>::iterator it = vars_.find(var_name);
    if (it != vars_.end())
      {
        xs_type_info xsti = it->second;
        schema *sche_type = xsti.type;
        str_type = ctx_->getIdiom()->translate_type(sche_type);
      }

    return str_type;
  }

void java_code::variable_(stmt_variable& info)
  {
    //td: when declaration separated by comma appears
    //    implement loop section with this
    str str_type = get_type_name(info.id);

    std::stringstream ss;
    str ind = get_indent_str();
    ss << ind << str_type << " " << info.id;

    if (!info.value.empty())
      ss << " = " << render_expression(info.value, ctx_);

    ss << ";\n";

    add_line(ss.str());
  }

void java_code::for_(stmt_for& info)
	{
    //td: when declaration separated by comma appears
    //    implement loop section with this
    str str_type = get_type_name(info.init_variable.id);

    std::stringstream ss;
    str iterable_declaration = str_type + " " + info.init_variable.id;
    ss << "for(" << iterable_declaration << " = " << render_expression(info.init_variable.value, ctx_)
        << "; " << render_expression(info.cond_expr, ctx_)
        << "; " << render_expression(info.iter_expr, ctx_) << ")";

    add_line(ss.str(), true);
    add_line("{", true);
		  render_code(info.for_code, indent_ + 1);
		add_line("}", true);
	}

void java_code::iterfor_(stmt_iter_for& info)
	{
    str iterable_type = get_type_name(info.id);

    std::stringstream ss;
    str ind = get_indent_str();

    ss << ind << "for(" << iterable_type << " " << info.id << " : "
      << render_expression(info.iter_expr, ctx_) << ")\n";

		add_line(ss.str());
		add_line("{", true);
			render_code(info.for_code, indent_ + 1);
		add_line("}", true);
	}

str java_code::render_expression(expression& expr, XSSContext ctx)
  {
    return idiom_utils::render_expression<java_expression_renderer>(expr, ctx);
  }

//java_args
str java_args::resolve_param(const param_decl& param)
	{
    return param.type + " " + param.name;
	}

//java_idiom
str java_idiom::translate_type(schema* type)
  {
    str str_type;
    if (!type)
      {
        str_type = "Object";
      }
    else if (type->options() & TYPE_DYNAMIC)
      {
        str_type = "Object";
      }
    else if (type->options() & TYPE_ITERATED)
      {
        str_type = "ArrayList";
      }
    else if (type == type_schema<int>())
      {
        str_type = "Integer";
      }
    else if (type == type_schema<str>())
      {
        str_type = "String";
      }
    else if (type == type_schema<float>())
      {
        str_type = "Double";
      }
    else if (type == type_schema<bool>())
      {
        str_type = "Boolean";
      }
    //td: specialize xs custom class with serialize methods

    return str_type;
  }

bool java_idiom::allow_cast(schema* fts_type, schema* sec_type)
  {
    if (fts_type == type_schema<float>())
      {
        if (sec_type == type_schema<double>() ||
          sec_type == type_schema<int>())
          return true;
      }
    else if (fts_type == type_schema<double>())
      {
        if (sec_type == type_schema<float>() ||
          sec_type == type_schema<int>())
          return true;
      }

    return false;
  }

variant java_idiom::process_code(code& cde, param_list_decl& params, XSSContext ctx)
	{
    code_type_resolver ctyper(ctx);
    cde.visit(&ctyper);

    JavaCode result(new java_code(ctx, cde));
    return result;
	}

variant java_idiom::process_expression(expression expr, XSSObject this_)
	{
    XSSContext ctx(new xss_composite_context(ctx_));
    ctx->this_ = this_;

    JavaExpression result(new java_expr(ctx, expr));
    return result;
	}

variant java_idiom::process_args(param_list_decl& params)
	{
		JavaArgs result(new java_args(params));
		return result;
	}

str java_idiom::resolve_this(XSSContext ctx)
	{
    //Programming languages do not agree on how to use *this* pointers
    //c++ lets you ignore them, jsva script et all force you to
    //and there are other intrincate circumstances (like functions in js)
    //where something must be done.

    if (!ctx->this_.empty())
      {
        str iid = variant_cast<str>(dynamic_get(ctx->this_, "id"), str());
        if (iid.empty())
          {
            param_list error;
            error.add("id", SJavaIdiom);
            error.add("desc", SJavaEveryInstanceMustHaveId);
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

    return "";
	}

str java_idiom::resolve_separator(XSSObject lh)
	{
		return ".";
	}

//java_expression_renderer
void java_expression_renderer::exec_operator(operator_type op, int pop_count, int push_count, bool top)
  {
		if (top && assigner)
			{
				assert(op == op_dot ||
               op == op_index);   //I'm sure there are more use cases, but I'll deal with this one exclusively
															    //for now
			}

    variant arg1, arg2;
    int op_prec = idiom_utils::get_operator_prec(op);
    switch(op)
      {
        // add here the operators that I'll customize
        case op_index:
        case op_typecast:
        case op_array:
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
            default: break;
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

        case op_call:
        case op_dot_call:
        case op_func_call:
        case op_parameter:
				case op_dot:
          {
            // execute exec_operator of base class with the same parameters
            expression_renderer::exec_operator(op, pop_count, push_count, top);
            break;
          }

        case op_typecast:
          {
            int p1;

						str os1 = operand_to_string(arg1, XSSObject(), &p1);

            expression_identifier ei2 = arg2;
            str os2 = ei2.value;
            schema* totype = ctx_->get_type(os2);

            std::stringstream ss;
            //ss << java_idiom::get_type(totype) << ".valueOf(" << os1 << ")";
            ss << ctx_->getIdiom()->translate_type(totype) << ".valueOf(" << os1 << ")";

            push_rendered(ss.str(), op_prec, variant());
            break;
          }

        case op_array:
          {
            std::stringstream result;

            str str_type = "Object"; //td: this value must come from expression
            result << "new ArrayList(Arrays.asList( new " << str_type << " [] {";

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

            result << "} ))";

            push_rendered(result.str(), op_prec, variant());
						break;
          }

        case op_index:
          {
            //i assume that arg1 is always array type
            std::stringstream result;
            str op1 = operand_to_string(arg1);
            str op2 = operand_to_string(arg2);

            if (assigner)
              {
					      result << op1;
                //enormous patch
                assigner->data = op1 + ".set(" + op2 + ", " +
                                 assigner->data + ")";
              }
            else
              result << op1 << ".get(" << op2 << ")";

            push_rendered(result.str(), op_prec, variant());
            break;
          }

				default:
          assert(false); //td:
      }
  }

str java_expression_renderer::array_operation(str lopnd, str ropnd, operator_type op)
  {
    str result = lopnd + " " + idiom_utils::get_operator_str(op) + " " + ropnd;

    if (op == op_plus_equal)
      result = lopnd + ".add(" + ropnd + ")";
    else if (op == op_minus_equal)
      result = lopnd + ".remove(" + ropnd + ")"; //td: verify correct object erase

    return result;
  }
