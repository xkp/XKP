
#include <xss/c++.h>
#include <xss/xss_error.h>

using namespace xkp;

//strings
const str SCPPIdiom("idiom");
const str SCPPCannotResolve("unkown-identifier");

const str SCPPEveryInstanceMustHaveId("Trying to use an instance without id");
const str SCPPUnknownInstance("Instance not found");

//cpp_code
void cpp_code::variable_(stmt_variable& info)
  {
		schema* sche_type = ctx_->get_type(info.type);
    str str_type;
		if (!sche_type)
			{
				if (!info.value.empty())
					{
						expr_type_resolver typer(ctx_);
						info.value.visit(&typer);

            xs_type_info xsti = typer.get();
						sche_type = xsti.type;
						str_type = typer.type_name();
					}
			}

    if (sche_type)
			{
				str_type = ctx_->get_type_name(sche_type);
			}

		vars_.insert(std::pair<str, xs_type_info>(info.id, sche_type)); //td: !!! XSSType

    std::stringstream ss;
    str ind = get_indent_str();

    ss << ind << str_type << " " << info.id;
    if (!info.value.empty())
      ss << " = " << render_expression(info.value, ctx_);

    ss << ";\n";

    add_line(ss.str());
  }

void cpp_code::for_(stmt_for& info)
	{
    std::stringstream ss;
    str iterable_declaration = info.init_variable.type + " " + info.init_variable.id;
    ss << "for(" << iterable_declaration << " = " << render_expression(info.init_variable.value, ctx_)
        << "; " << render_expression(info.cond_expr, ctx_)
        << "; " << render_expression(info.iter_expr, ctx_) << ")";

    add_line(ss.str(), true);
    add_line("{", true);
		  render_code(info.for_code, indent_ + 1);
		add_line("}", true);
	}

void cpp_code::iterfor_(stmt_iter_for& info)
	{
    assert(false); //td:
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
      
    return "";
	}

str cpp_idiom::resolve_separator(XSSObject lh)
	{
		return "->";
	}

//cpp_expression_renderer
void cpp_expression_renderer::exec_operator(operator_type op, int pop_count, int push_count, bool top)
  {
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
				case op_dot:
          {
            // execute exec_operator of base class with the same parameters
            expression_renderer::exec_operator(op, pop_count, push_count, top);
            break;
          }

        case op_dot_call:
          {
            std::stringstream ss;
            
            str iid = operand_to_string(arg1);
            if (!iid.empty())
              ss << iid << "->";

            ss << operand_to_string(arg2);
            push_rendered(ss.str(), op_prec, variant());
            break;
          }

				default:
          assert(false); //td:
      }
  }
