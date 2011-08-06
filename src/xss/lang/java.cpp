
#include <xss/lang/java.h>
#include <xss/language.h>

using namespace xkp;

//java_code_renderer
java_code_renderer::java_code_renderer():
  base_code_renderer()
  {
  }

java_code_renderer::java_code_renderer(const java_code_renderer& other):
  base_code_renderer(other)
  {
  }

java_code_renderer::java_code_renderer(code& cde, XSSContext ctx):
  base_code_renderer(cde, ctx)
  {
  }

void java_code_renderer::variable_(stmt_variable& info)
  {
    //td: when declaration separated by comma appears
    //    implement loop section with this
    XSSType type = ctx_->resolve(info.id, RESOLVE_TYPE);
    str str_type = type->output_id();

    std::stringstream ss;

    ss << indent_str_ << str_type << " " << info.id;

    if (!info.value.empty())
      ss << " = " << render_expression(info.value, ctx_);

    ss << ";";

    add_line(ss.str());
  }

void java_code_renderer::for_(stmt_for& info)
	{
    //td: when declaration separated by comma appears
    //    implement loop section with this
    XSSType type = ctx_->resolve(info.init_variable.id, RESOLVE_TYPE);
    str str_type = type->output_id();

    std::stringstream ss;
    str iterable_declaration = str_type + " " + info.init_variable.id;
    ss << "for(" << iterable_declaration << " = " << render_expression(info.init_variable.value, ctx_)
        << "; " << render_expression(info.cond_expr, ctx_)
        << "; " << render_expression(info.iter_expr, ctx_) << ")";

    add_line(ss.str(), true);
    add_line("{", true);
		  render_code(info.for_code);
		add_line("}", true);
	}

void java_code_renderer::iterfor_(stmt_iter_for& info)
	{
    XSSType type = ctx_->resolve(info.id, RESOLVE_TYPE);
    str iterable_type = type->output_id();

    std::stringstream ss;

    ss << indent_str_ << "for(" << iterable_type << " " << info.id << " : "
      << render_expression(info.iter_expr, ctx_) << ")\n";

		add_line(ss.str());
		add_line("{", true);
			render_code(info.for_code);
		add_line("}", true);
	}

str java_code_renderer::render_expression(expression& expr, XSSContext ctx)
  {
    return lang_utils::render_expression<java_expr_renderer>(expr, ctx);
  }

str java_code_renderer::render_code(code& cde)
  {
    java_code_renderer inner(cde, ctx_); 
    str result = inner.render();
    add_line(result);
    return result;
  }

//java_expr_renderer
java_expr_renderer::java_expr_renderer():
  base_expr_renderer()
  {
  }

java_expr_renderer::java_expr_renderer(const java_expr_renderer& other):
  base_expr_renderer(other)
  {
  }

java_expr_renderer::java_expr_renderer(expression& expr, XSSContext ctx):
  base_expr_renderer(expr, ctx)
  {
  }

str java_expr_renderer::operand_to_string(variant operand, XSSObject parent, int* prec)
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

            Language lang = ctx_->get_language();
            str separator = lang->resolve_separator();

            resolve_info si;
            if (ctx_->resolve(ei.value, si))
              {
                switch (si.what)
                  {
                    case RESOLVE_PROPERTY:
                      {
                        XSSProperty prop = si.value;
										    result = prop->render_value();
                        break;
                      }
                    case RESOLVE_METHOD:
                      {
								        XSSMethod mthd     = si.value;
                        str       this_str = lang->resolve_this(ctx_);
                        if (!this_str.empty())
                          result = this_str + separator + ei.value; //otherwise it doesnt get translated
                        break;
                      }
                    case RESOLVE_INSTANCE:
				              {
					              XSSObject obj = si.value;
							          result = obj->output_id();
                        break;
				              }
                    default:
                      assert(false); //trap use case
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
        str opstr = variant_cast<str>(operand, str());
        if (opstr.empty())
          {
            assert(false); //td: determine if this is an error condition  
          }

        result = opstr;
      }

    if (prec) *prec = result_prec;
    return result;
  }

str java_expr_renderer::array_operation(const str &left, const str &right, operator_type op)
  {
    str result = left + " " + lang_utils::operator_string(op) + " " + right;

    if (op == op_plus_equal)
      result = left + ".add(" + right + ")";
    else if (op == op_minus_equal)
      result = left + ".remove(" + right + ")"; //td: verify correct object erase

    return result;
  }

str java_expr_renderer::render()
  {
    return lang_utils::render_expression<java_expr_renderer>(expr_, ctx_);
  }

void java_expr_renderer::exec_operator(operator_type op, int pop_count, int push_count, bool top)
  {
		if (top && assigner)
			{
				assert(op == op_dot ||
               op == op_index);   //I'm sure there are more use cases, but I'll deal with this one exclusively
															    //for now
			}

    variant arg1, arg2;
    int op_prec = lang_utils::operator_prec(op);
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
            base_expr_renderer::exec_operator(op, pop_count, push_count, top);
            break;
          }

        case op_typecast:
          {
            int p1;

						str os1 = operand_to_string(arg1, XSSObject(), &p1);

            expression_identifier ei2 = arg2;
            str os2 = ei2.value;
            XSSType totype = ctx_->resolve(os2, RESOLVE_TYPE);

            std::stringstream ss;
            ss << totype->output_id() << ".valueOf(" << os1 << ")";

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

//java_args_renderer 
java_args_renderer::java_args_renderer():
  base_args_renderer()
  {
  }

java_args_renderer::java_args_renderer(const java_args_renderer& other):
  base_args_renderer(other)
  {
  }

java_args_renderer::java_args_renderer(param_list_decl& params, XSSContext ctx):
  base_args_renderer(params, ctx)
  {
  }

str java_args_renderer::render()
  {
    std::ostringstream oss;

    param_list_decl::iterator it = args_.begin();
    param_list_decl::iterator nd = args_.end();
    for(; it != nd; it++)
      {
        oss << it->type << " " << it->name << ","; //td: interface
      }

    str result = oss.str();
    if (!result.empty())
      result.erase(result.end() - 1);

    return result;
  }

//java_lang
variant java_lang::compile_code(code& cde, param_list_decl& params, XSSContext ctx)
  {
    return reference<java_code_renderer>(new java_code_renderer(cde, ctx));
  }

variant java_lang::compile_expression(expression expr, XSSContext ctx)
  {
    return reference<java_expr_renderer>(new java_expr_renderer(expr, ctx));
  }

variant java_lang::compile_args(param_list_decl& params, XSSContext ctx)
  {
    return reference<java_args_renderer>(new java_args_renderer(params, ctx));
  }

bool java_lang::can_cast(XSSType left, XSSType right)
  {
    if (left->is_variant())
       return true;

    schema *left_native, *right_native;
    if (left->is_native() && right->is_native())
      {
        left_native = left->native_type();
        right_native = right->native_type();
      }

    if (left_native == type_schema<float>())
      {
        if (right_native == type_schema<double>() ||
          right_native == type_schema<int>())
          return true;
      }
    else if (left_native == type_schema<double>())
      {
        if (right_native == type_schema<float>() ||
          right_native == type_schema<int>())
          return true;
      }

    return false;
  }

void java_lang::init_context(XSSContext ctx)
  {
    XSSType type;
    
    if (type = ctx->get_type("int"))
      type->set_output_id("Integer");

    else if (type = ctx->get_type("string"))
      type->set_output_id("String");


    assert(false); //td: translate types
  }
