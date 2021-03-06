
#include <xss/lang/java.h>
#include <xss/language.h>
#include <xss/xss_error.h>

using namespace xkp;

const str SJavaLanguage("java-language");

const str SUnknownType("Cannot resolve type");
const str SWriteOnlyProperty("Trying to acces write only property");
const str SReadOnlyProperty("Trying to acces read only property");

//java_code_renderer
java_code_renderer::java_code_renderer():
  base_code_renderer()
  {
  }

java_code_renderer::java_code_renderer(const java_code_renderer& other):
  base_code_renderer(other)
  {
  }

java_code_renderer::java_code_renderer(code& cde, param_list_decl& params, XSSContext ctx, int indent):
  base_code_renderer(cde, params, ctx, indent)
  {
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
                    case RESOLVE_PROPERTY:
                      {
                        XSSProperty prop = si.value;
                        result = lang->property_get(prop, lang->resolve_this(ctx_), ctx_);
                        break;
                      }
                    case RESOLVE_METHOD:
                      {
								        XSSMethod mthd = si.value;
                        bool user_defined = mthd->get<bool>("user_defined", false);
                        if (user_defined)
                          result = mthd->output_id();
                        else
                          {
                            str this_str = lang->resolve_this(ctx_);
                            if (!this_str.empty())
                              result = this_str + separator + ei.value; //otherwise it doesnt get translated
                          }
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
                        //td: specialize on exec_operator op_dot, op_dot_call
                        str prop_name = "static";
                        XSSType type = si.value;

                        bool static_mthd = false;
                        if (type->has(prop_name) || type->find(prop_name))
                          {
                            static_mthd = type->get<bool>(prop_name, false);
                          }

                        if (!type->is_enum() && !static_mthd)
                          result = "new " + type->output_id();
                        else
                          result = type->output_id();
                        break;
                      }
                    case RESOLVE_VARIABLE:
                      {
                        break; //do nothing
                      }
                    default:
                      assert(false); //trap use case
                  }
              }
          }
        else
          {
            resolve_info left;
            left.value = parent;

            resolve_info right;
            right.left = &left;
            if (ctx_->resolve(ei.value, right))
              {
                switch (right.what)
                  {
                    case RESOLVE_PROPERTY:
                      {
                        XSSProperty prop = parent->get_property(ei.value);
                        if (prop)
                          result = prop->output_id();
                        break;
                      }
                    case RESOLVE_METHOD:
                      {
                        XSSMethod mthd = parent->get_method(ei.value);
                        if (mthd)
                          result = mthd->output_id();
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

str java_expr_renderer::array_operation(const str &left, const str &right, operator_type op) //td: erase me and all inherits virtual methods
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

str java_expr_renderer::render_expression(expression& expr)
  {
    return lang_utils::render_expression<java_expr_renderer>(expr, ctx_);
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
        case op_dot_call:
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

        case op_and:
        case op_or:
        case op_equal:
        case op_notequal:
        case op_gt:
        case op_lt:
        case op_ge:
        case op_le:

        case op_call:
        case op_func_call:
        case op_parameter:
				case op_dot:
        case op_instantiate:
        case op_object:
          {
            // execute exec_operator of base class with the same parameters
            base_expr_renderer::exec_operator(op, pop_count, push_count, top);
            break;
          }

        case op_dot_call:
          {
            std::stringstream ss;

            str opnd1 = operand_to_string(arg1);

            XSSObject inst = get_instance(arg1);
            str opnd2 = operand_to_string(arg2, inst);

						str separator = ctx_->get_language()->resolve_separator();

            /*
				    //here comes the hacky hoo
            size_t first_dot = opnd2.find_first_of(separator);
				    if (first_dot != str::npos)
					    {
						    str str_this = opnd2.substr(0, first_dot);
                if (str_this == opnd1)
                  opnd2.erase(0, first_dot + 1);
					    }
            */

            if (inst)
              {
                resolve_info left;
                left.what  = RESOLVE_INSTANCE;
                left.value = inst;

                resolve_info right;
                right.left = &left;

                expression_identifier ei = arg2;
                if (ctx_->resolve(ei.value, right))
                  {
                    switch(right.what)
                      {
                        case RESOLVE_METHOD:
                          {
                            XSSMethod mthd = right.value;
                            bool user_def = mthd->get<bool>("user_defined", false);

                            if (user_def && mthd->output_id() != mthd->id())
                              {
                                ss << mthd->output_id();
                                push_rendered(ss.str(), op_prec, variant(), opnd1);
                                return;
                              }

                            break;
                          }
                        default:
                          assert(false); //td:
                      }
                  }
              }

            ss << opnd1 << separator << opnd2;
            push_rendered(ss.str(), op_prec, variant(), opnd1);
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

            push_rendered(ss.str(), op_prec, variant(), os1);
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

            push_rendered(ss.str(), op_prec, variant(), os1);
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

            push_rendered(result.str(), op_prec, variant(), op1);
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
    //XSSObject sub = variant_cast<XSSObject>(ctx->resolve("#this", RESOLVE_CONST), XSSObject());
    //if (sub)
    //  return sub->output_id();

    //return "this";
    XSSObject ths = ctx->get_this();
    if (ths)
      return ths->output_id();

    return str();
  }

bool java_lang::can_cast(XSSType left, XSSType right)
  {
    if (left->is_variant() || left->is_object())
       return true;

    schema *tleft = null;
    schema *tright = null;

    if (left->is_native() && right->is_native())
      {
        tleft = left->native_type();
        tright = right->native_type();
      }
    else
    if (left->is_array() && right->is_array())
      {
        if (left->array_type()->is_variant())
          return true;

        tleft = left->array_type()->native_type();
        tright = right->array_type()->native_type();
      }
    else
      return false;

    if (tleft == type_schema<float>())
      {
        if (tright == type_schema<double>())
          return true;
      }
    else if (tleft == type_schema<double>())
      {
        if (tright == type_schema<float>())
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
      type->set_output_id("Float");

    if (type = ctx->get_type("double"))
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
    if (value.is<float>() || value.is<double>())
      {
        float f = value;

        std::stringstream ss;
        ss.setf(std::ios::fixed, std::ios::floatfield);
        ss.precision(6); //td: ajust this value

        ss << f << "f";

        return ss.str();
      }

    return base_lang::render_value(type, value);
  }

bool java_lang::custom_operator(XSSType lt, XSSType rt, str l, str r, operator_type op, str& res)
  {
    if (!lt || !rt)
      return false;

    if (lt->id() == rt->id() && lt->id() == "string")
      {
        switch (op)
        {
          case op_notequal:
            {
              res += "!";
            }
          case op_equal:
            {
              res += l + ".equals(" + r + ")";
              break;
            }
          case op_gt:
          case op_lt:
          case op_ge:
          case op_le:
          case op_and:
          case op_or:
            {
              res += l + ".compareTo(" + r + ") " + lang_utils::operator_string(op) + " 0";
              break;
            }
          default:
            return false;
        }

        return true;
      }
    else
      res = l + " " + lang_utils::operator_string(op) + " " + r;

    return false;
  }

str java_lang::property_get(XSSProperty prop, const str& path, XSSContext ctx)
  {
    XSSObject get = prop->find("get");
    bool user_def = prop->get<bool>("user_defined", false);

    if (get)
      {
        if (user_def)
          get->add_attribute("text", prop->output_id() + "_get()");

        compile_property(prop, ctx);

        XSSRenderer rend   = XSSObject(prop)->get<XSSRenderer>("#get_renderer", XSSRenderer());
        bool        global = get->get<bool>("global", false);

        param_list params;
        params.add("path", path);
        str result = rend->render(XSSObject(), &params);
        return global || user_def ? result : path + "." + result;
      }
    else
      {
        XSSObject set = prop->find("set");
        if (set)
          {
            //read only, set but no get
            param_list error;
            error.add("id", SJavaLanguage);
            error.add("desc", SWriteOnlyProperty);
            error.add("property", prop->id());
            xss_throw(error);
          }
        else
          {
            str result = "";

            if (!user_def)
              result = path + ".";

            return result + prop->output_id();
          }
      }

    return str();
  }

str java_lang::property_set(XSSProperty prop, const str& path, const str& value, XSSContext ctx)
  {
    XSSObject set = prop->find("set");
    bool user_def = prop->get<bool>("user_defined", false);

    if (set)
      {
        if (user_def)
          set->add_attribute("text", prop->output_id() + "_set({value})");

        compile_property(prop, ctx);

        XSSRenderer rend   = XSSObject(prop)->get<XSSRenderer>("#set_renderer", XSSRenderer());
        bool        global = set->get<bool>("global", false);

        param_list params;
        params.add("path", path);
        params.add("value", value);

        str result = rend->render(XSSObject(), &params);
        return global || user_def ? result : path + "." + result;
      }
    else
      {
        XSSObject get = prop->find("get");
        if (get)
          {
            param_list error;
            error.add("id", SJavaLanguage);
            error.add("desc", SReadOnlyProperty);
            error.add("property", prop->id());
            xss_throw(error);
          }
        else
          {
            str result = "";

            if (!user_def)
              result = path + ".";

            return result + prop->output_id() + " = " + value;
          }
      }

    return str();
  }

str java_lang::render_assignment(const str& path, const str& prop, const str& value)
  {
    if (path.empty())
      return prop + " = " + value;
    return path + "." + prop + " = " + value;
  }

str java_lang::expression_path(const str& expr)
  {
    size_t pos = expr.find_last_of(".");
    if (pos != str::npos)
      {
        return str(expr.begin(), expr.begin() + pos);
      }
    return expr;
  }

str java_lang::array_operation(operator_type op, const str& arr, const str& value, XSSContext ctx)
  {
    if (op == op_plus_equal)
      return arr + ".add(" + value + ")";
    else if (op == op_minus_equal)
      return arr + ".remove(" + value + ")"; //td: verify correct object erase

    assert(false); //only this operators are supported, is this a valid use case?
    return str();
  }
