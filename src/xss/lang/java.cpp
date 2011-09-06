
#include <xss/lang/java.h>
#include <xss/language.h>
#include <xss/xss_error.h>

using namespace xkp;

const str SJavaLanguage("java-language");

const str SUnknownType("Cannot resolve type");

//java_code_renderer
java_code_renderer::java_code_renderer():
  base_code_renderer()
  {
  }

java_code_renderer::java_code_renderer(const java_code_renderer& other):
  base_code_renderer(other)
  {
    Language lang = ctx_->get_language();
    lang->init_context(ctx_);
  }

java_code_renderer::java_code_renderer(code& cde, param_list_decl& params, XSSContext ctx, int indent):
  base_code_renderer(cde, params, ctx, indent)
  {
    Language lang = ctx->get_language();
    lang->init_context(ctx);
  }

void java_code_renderer::variable_(stmt_variable& info)
  {
    //td: when declaration separated by comma appears
    //    implement loop section with this
    str output_type;
    resolve_info ri;
    if (ctx_->resolve(info.id, ri))
      {
        XSSType type = ri.type;
        output_type = type->output_id();
      }
    else
      {
        XSSType type = ctx_->get_type(info.type);
        output_type = type->output_id();
      }

    std::stringstream ss;
    ss << output_type << " " << info.id;

    if (!info.value.empty())
      ss << " = " << render_expression(info.value, ctx_);

    ss << ";";

    add_line(ss.str());
  }

void java_code_renderer::for_(stmt_for& info)
	{
    //td: when declaration separated by comma appears
    //    implement loop section with this
    XSSType type = ctx_->get_type(info.init_variable.type);
    str output_type = type->output_id();

    std::stringstream ss;
    str iterable_declaration = output_type + " " + info.init_variable.id;
    ss << "for(" << iterable_declaration << " = " << render_expression(info.init_variable.value, ctx_)
        << "; " << render_expression(info.cond_expr, ctx_)
        << "; " << render_expression(info.iter_expr, ctx_) << ")";

    add_line(ss.str());
    add_line("{");
		  render_code(info.for_code);
		add_line("}");
	}

void java_code_renderer::iterfor_(stmt_iter_for& info)
	{
    XSSType type = ctx_->get_type(info.type.name);
    str output_type = type->output_id();

    std::stringstream ss;
    ss << "for(" << output_type << " " << info.id << " : "
      << render_expression(info.iter_expr, ctx_) << ")";

		add_line(ss.str());
		add_line("{");
			render_code(info.for_code);
		add_line("}");
	}

str java_code_renderer::render_expression(expression& expr, XSSContext ctx)
  {
    return lang_utils::render_expression<java_expr_renderer>(expr, ctx);
  }

str java_code_renderer::render_code(code& cde)
  {
    param_list_decl pld;
    java_code_renderer inner(cde, pld, ctx_, indent_ + 1);
    str result = inner.render();

    add_line(result, false);

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
    Language lang = ctx_->get_language();

    if (operand.is<expression_identifier>())
      {
        expression_identifier ei = operand;
        result = ei.value;

        if (!parent)
          {
            //here we ought to resolve a single symbol (ex width = 10)
            //thid *could* belong to the "this" pointer
            str separator = lang->resolve_separator();

            resolve_info si;
            if (ctx_->resolve(ei.value, si))
              {
                switch (si.what)
                  {
                    case RESOLVE_VARIABLE:
                    case RESOLVE_PROPERTY:
                      {
                        break; //do nothing
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
                    case RESOLVE_TYPE:
                      {
                        XSSType type = si.value;
                        result = "new " + type->output_id();
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
        str opstr = lang->render_value(XSSType(), operand);
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
        case op_typecheck:
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

        case op_typecheck:
          {
						str os1 = operand_to_string(arg1, XSSObject());

            expression_identifier ei2 = arg2;
            str os2 = ei2.value;
            XSSType type = ctx_->resolve(os2, RESOLVE_TYPE);

            if (!type)
              {
                param_list error;
                error.add("id", SJavaLanguage);
                error.add("desc", SUnknownType);
                error.add("type", os2);
                xss_throw(error);
              }

            std::stringstream ss;
            ss << os1 << " instanceof " << type->output_id();

            push_rendered(ss.str(), op_prec, variant());
            break;
          }

        case op_typecast:
          {
						str os1 = operand_to_string(arg1, XSSObject());

            expression_identifier ei2 = arg2;
            str os2 = ei2.value;
            XSSType totype = ctx_->resolve(os2, RESOLVE_TYPE);

            if (!totype)
              {
                param_list error;
                error.add("id", SJavaLanguage);
                error.add("desc", SUnknownType);
                error.add("type", os2);
                xss_throw(error);
              }

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
                assigner->data = op1 + ".set(" + op2 + ", " + assigner->data + ")";
                assigner->flag = true;
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
        XSSType type = ctx_->get_type(it->type);

        if (!type)
          type = ctx_->get_type("var");

        str output_type = type->output_id();

        oss << output_type << " " << it->name;

        if (!it->default_value.empty())
          oss << " = " << render_expression(it->default_value, ctx_);

        oss << ",";
      }

    str result = oss.str();
    if (!result.empty())
      result.erase(result.end() - 1);

    return result;
  }

str java_args_renderer::render_expression(expression& expr, XSSContext ctx)
  {
    return lang_utils::render_expression<java_expr_renderer>(expr, ctx);
  }

//java_lang
variant java_lang::compile_code(code& cde, param_list_decl& params, XSSContext ctx)
  {
    return reference<java_code_renderer>(new java_code_renderer(cde, params, ctx));
  }

variant java_lang::compile_expression(expression expr, XSSContext ctx)
  {
    return reference<java_expr_renderer>(new java_expr_renderer(expr, ctx));
  }

variant java_lang::compile_args(param_list_decl& params, XSSContext ctx)
  {
    return reference<java_args_renderer>(new java_args_renderer(params, ctx));
  }

str java_lang::resolve_this(XSSContext ctx)
  {
    XSSObject sub = variant_cast<XSSObject>(ctx->resolve("#this", RESOLVE_CONST), XSSObject());
    if (sub)
      return sub->output_id();

    return str();
  }

bool java_lang::can_cast(XSSType left, XSSType right)
  {
    if (left->is_variant() || left->is_object())
       return true;

    schema *left_native = null;
    schema *right_native = null;

    if (left->is_native() && right->is_native())
      {
        left_native = left->native_type();
        right_native = right->native_type();
      }
    else
      return false;

    if (left_native == type_schema<float>())
      {
        if (right_native == type_schema<double>())
          return true;
      }
    else if (left_native == type_schema<double>())
      {
        if (right_native == type_schema<float>())
          return true;
      }

    return false;
  }

void java_lang::init_context(XSSContext ctx)
  {
    XSSType type;

    //translate types
    if (type = ctx->get_type("bool"))
      type->set_output_id("Boolean");

    if (type = ctx->get_type("int"))
      type->set_output_id("Integer");

    if (type = ctx->get_type("string"))
      type->set_output_id("String");

    if (type = ctx->get_type("float"))
      type->set_output_id("Double");

    if (type = ctx->get_type("object"))
      type->set_output_id("Object");

    if (type = ctx->get_type("var"))
      type->set_output_id("Object");

    if (type = ctx->get_type("array"))
      type->set_output_id("ArrayList");
  }

XSSType java_lang::resolve_array_type(XSSType type, const str& at_name, XSSContext ctx)
  {
    str outputid_type = type->output_id();
    str new_outputid_type = "ArrayList<" + outputid_type + ">";

    XSSType new_type(new xss_type);
    new_type->set_id(at_name);
    new_type->set_output_id(new_outputid_type);
    new_type->as_array(type);

    //XSSProperty prop_size = XSSProperty(new xss_property("size", ctx->get_type("int"), variant(), XSSObject()));
    //new_type->add_xss_property("size", prop_size);

    XSSMethod mthd = XSSMethod(new xss_method("size", ctx->get_type("int"), variant(), variant()));
    new_type->register_method("size", mthd);

    return new_type;
  }

str java_lang::render_value(XSSType type, variant value)
  {
    if (value.is<str>())
      return variant_cast<str>(value, str());

    if (value.is<bool>())
      {
        bool vv = value;
        if (vv)
          return "true";
        else
          return "false";
      }

    if (value.is<float>() || value.is<double>())
      {
        float f = value;

        std::stringstream ss;
        ss.setf(std::ios::fixed, std::ios::floatfield);
        ss.precision(6);//td: ajust this value

        ss << f;

        return ss.str();
      }
    
    return variant_cast<str>(value, str());
  }
