
#include <xss/lang/base.h>
#include <xss/xss_error.h>

#include <boost/algorithm/string.hpp>

using namespace xkp;

const str SLanguage("language");

const str SEmptyExpression("Empty expression");
const str SAssignOperator("Assign operators can only be used as the base of an expression");
const str SUnknownType("Cannot resolve type");

//base_code_renderer
base_code_renderer::base_code_renderer()
  {
  }

base_code_renderer::base_code_renderer(const base_code_renderer& other):
  code_(other.code_),
  params_(other.params_),
  ctx_(other.ctx_),
  result_(other.result_),
  expr_(other.expr_),
  indent_(other.indent_),
  return_type_(other.return_type_)
  {
  }

base_code_renderer::base_code_renderer(code& cde, param_list_decl& params, XSSContext ctx, int indent):
  code_(cde),
  ctx_(new xss_context(ctx)),
  indent_(indent),
  return_type_(false)
  {
    //add arguments to context
    param_list_decl::iterator itb = params.begin();
    param_list_decl::iterator ite = params.end();
    for(; itb != ite; itb++)
      {
        param_decl &param = *itb;
        XSSType type;

        if (param.type.empty())
          {
            if (!param.default_value.empty())
              {
                type = lang_utils::expr_type(param.default_value, ctx_);
              }
            else
              {
                type = ctx_->get_type("var");
              }
          }
        else
          {
            type = ctx_->get_type(param.type);

            if (!type)
              {
                param_list error;
                error.add("id", SLanguage);
                error.add("desc", SUnknownType);
                error.add("type", param.type);
                xss_throw(error);
              }
          }

        ctx_->register_symbol(RESOLVE_VARIABLE, param.name, type);
      }
  }

str base_code_renderer::render()
  {
    //resolve types of code context
    type_ = type();

		result_ = ""; //td: cache, not parallel
    code_.visit(this);

    return result_;
  }

XSSType base_code_renderer::type()
  {
    if (!type_ && !return_type_)
      {
        type_ = lang_utils::code_type(code_, ctx_);
        return_type_ = true;
      }

    return type_;
  }

void base_code_renderer::if_(stmt_if& info)
  {
    add_line("if (" + render_expression(info.expr, ctx_) + ")");
    add_line("{");
			render_code(info.if_code);
		add_line("}");

    if (!info.else_code.empty())
			{
				add_line("else");
				add_line("{");
					render_code(info.else_code);
				add_line("}");
			}
  }

void base_code_renderer::variable_(stmt_variable& info)
  {
    std::stringstream ss;

    ss << "var " << info.id;
    if (!info.value.empty())
      ss << " = " << render_expression(info.value, ctx_);

    ss << ";";

    add_line(ss.str());
  }

void base_code_renderer::for_(stmt_for& info)
  {
    std::stringstream ss;

    ss << "for(var " << info.init_variable.id << " = " << render_expression(info.init_variable.value, ctx_)
        << "; " << render_expression(info.cond_expr, ctx_)
        << "; " << render_expression(info.iter_expr, ctx_) << ")";

    add_line(ss.str());
    add_line("{");
			render_code(info.for_code);
		add_line("}");
  }

void base_code_renderer::iterfor_(stmt_iter_for& info)
  {
    std::stringstream ss;

    str iterable_name = info.id + "_iterable";
    str iterator_name = info.id + "_iterator";

    add_line("var " + iterable_name + " = " + render_expression(info.iter_expr, ctx_) + ";");
    ss << "for(var " << iterator_name << " = 0; "
        << iterator_name << " < " << iterable_name << ".length; "
        << iterator_name << "++" << ")";
		add_line(ss.str());
		add_line("{");
		add_line(indent_str_ + "var " + info.id + " = " + iterable_name + "[" + iterator_name + "];\n", false);
			render_code(info.for_code);
		add_line("}");
  }

void base_code_renderer::while_(stmt_while& info)
  {
    add_line("while(" + render_expression(info.expr, ctx_) + ")");
		add_line("{");
			render_code(info.while_code);
		add_line("}");
  }

void base_code_renderer::break_()
  {
		add_line("break;");
  }

void base_code_renderer::continue_()
  {
		add_line("continue;");
  }

void base_code_renderer::return_(stmt_return& info)
  {
    if (info.expr.empty())
      add_line("return;");
    else
      add_line("return " + render_expression(info.expr, ctx_) + ";");
  }

void base_code_renderer::expression_(stmt_expression& info)
  {
    str value = render_expression(info.expr, ctx_);
    add_line(value + ";");
  }

void base_code_renderer::dsl_(dsl& info)
  {
		assert(false); //td: there is some stuff to implement here... later
  }

void base_code_renderer::dispatch(stmt_dispatch& info)
  {
    assert(false); //td: ought to define what to do here, it would seem like the idiom would like
                    //to handle this
  }

str base_code_renderer::indent()
  {
    str result;
    for(int i = 0; i < indent_; i++)
      result += '\t';

    return result;
  }

void base_code_renderer::add_line(str line, bool trim)
  {
    if (indent_ != indent_str_.size())
      indent_str_ = indent();

    if (trim)
      {
        boost::trim(line);
        result_ += indent_str_;
      }

    result_ += line;

    if (trim)
      result_ += '\n';
  }

//base_expr_renderer
base_expr_renderer::base_expr_renderer():
  assigner(null),
  capturing_property_(false)
  {
  }

base_expr_renderer::base_expr_renderer(const base_expr_renderer& other):
  assigner(null),
  capturing_property_(false),
  ctx_(other.ctx_),
  expr_(other.expr_)
  {
  }

base_expr_renderer::base_expr_renderer(expression& expr, XSSContext ctx):
  assigner(null),
  capturing_property_(false),
  ctx_(ctx),
  expr_(expr)
  {
  }

str base_expr_renderer::resolve_assigner(variant operand, XSSObject instance, assign_info* ai)
  {
		XSSProperty prop;
		str					result;

    XSSObject caller = get_instance(operand);
    Language  lang   = ctx_->get_language();
    str separator    = lang->resolve_separator(caller);

		bool use_this = false;
    if (operand.is<expression_identifier>())
			{
				expression_identifier ei = operand;
				if (instance)
          prop = ctx_->resolve(ei.value, instance, RESOLVE_PROPERTY);
        else
          {
            use_this = true;
            prop = ctx_->resolve(ei.value, RESOLVE_PROPERTY);
          }
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
    else if (operand.is<XSSProperty>())
      prop = operand;

    if (prop)
      {
        if (prop->has("property_xss_set") && !dynamic_get(prop, "property_xss").empty())
          {
            capture_property_.prop = prop;
            capture_property_.xss  = variant_cast<str>(dynamic_get(prop, "property_xss_set"), str());

            for(size_t i = 0; i < capture_property_.xss.size(); i++)
              {
                if (capture_property_.xss[i] == '\'')
	                capture_property_.xss[i] ='"';
              }

            push_rendered(operand_to_string(instance), 0, instance);

						ai->type = FN_XSS_CALL;
						ai->data = render_captured_property();

            return ai->data;
          }

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
              {
							  if (use_this)
                  {
                    str this_str = lang->resolve_this(ctx_);
                    if (!this_str.empty())
                      return this_str + separator + set_fn;
                  }
                return set_fn;
              }
						else
              return result + separator + set_fn;
          }
        else if (!prop->set.empty())
					{
						ai->type = FN_CALL;
						if (result.empty())
              {
                str fn_call = prop->output_id() + "_set";
							  if (use_this)
                  {
                    str this_str = lang->resolve_this(ctx_);
                    if (!this_str.empty())
                      return this_str + separator + fn_call;
                  }

							  return fn_call;
              }
						else
              return result + separator + prop->id() + "_set";
					}
      }

		ai->type = VANILLA;
		return operand_to_string(operand, instance, null);
  }

str base_expr_renderer::operand_to_string(variant operand, XSSObject parent, int* prec)
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
										    result = prop->render_get();
                        str this_str = lang->resolve_this(ctx_);
                        if (!this_str.empty())
                          result = this_str + separator + result; //otherwise it doesnt get translated
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
                    case RESOLVE_VARIABLE:
                      {
                        //do nothing
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

str base_expr_renderer::array_operation(const str &left, const str &right, operator_type op)
  {
    str result = left + " " + lang_utils::operator_string(op) + " " + right;

    return result;
  }

str base_expr_renderer::get()
  {
    if(stack_.empty())
      {
        param_list error;
        error.add("id", SLanguage);
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
				Language lang = ctx_->get_language();
        str separator = lang->resolve_separator();

				expression_identifier ei = result;
				if (assigner)
					{
            //resolve asign
            resolve_info si;
						if (ctx_->resolve(ei.value, si))
              {
						    str ass = resolve_assigner(si.value, ctx_->get_this(), assigner);
						    str result = lang->resolve_this(ctx_);
						    if (result.empty())
							    return ass;

						    return result + separator + ass;
              }

              return ei.value; //td: not sure at all
					}

        //
        resolve_info si;
				if (ctx_->resolve(ei.value, si))
          {
            switch(si.what)
              {
                case RESOLVE_METHOD:
                  {
				            XSSMethod mthd = si.value;
                    str this_str = lang->resolve_this(ctx_);
                    if (!this_str.empty())
                      return this_str + separator + ei.value;
                    break;
                  }
                case RESOLVE_PROPERTY:
                  {
                    XSSProperty prop = si.value;
                    str this_str = lang->resolve_this(ctx_);
                    if (!this_str.empty())
                      {
                        if (prop->has("property_xss") && !dynamic_get(prop, "property_xss").empty())
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

                        //return this_str + separator + ei.value;
                        return this_str + separator + prop->output_id();
						          }
                    else
                      {
                        return prop->output_id();
                      }
                    break;
                  }
              }
          }

				return ei.value;
			}
    else if (result.is<str>())
      {
        str res = result;
        str ss = '"' + res + '"';
        return ss;
      }
    else
      {
        str res = ctx_->get_language()->render_value(XSSType(), result);
        return res;
      }

    str to_string = result;
    return to_string;
  }

XSSType base_expr_renderer::type()
  {
    return !type_ ? lang_utils::expr_type(expr_, ctx_) : type_;
  }

variant base_expr_renderer::eval(XSSContext ctx)
  {
    fs::path file;
    code_context cctx = ctx->get_compile_context();
    xs_utils xs;
    return xs.evaluate_xs_expression(expr_, cctx, file);
  }

void base_expr_renderer::push(variant operand, bool top)
  {
		if (top && assigner)
			{
				str ass = resolve_assigner(operand, XSSObject(), assigner);
				push_rendered(ass, 0, operand.get_schema());
        return;
			}

    stack_.push(operand);
  }

void base_expr_renderer::exec_operator(operator_type op, int pop_count, int push_count, bool top)
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
			}

    int op_prec = lang_utils::operator_prec(op);
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
											ss << "(" << os << ")" << lang_utils::operator_string(op);
										else
											ss << os << lang_utils::operator_string(op);
										break;
									}
								case FN_CALL:
									{
										str op_str = lang_utils::operator_string(op);
										op_str.erase(op_str.end() - 1); //now this is too much fun

										ss << ass << "(" << os << " " << op_str << " 1)";
										break;
									}
                case FN_XSS_CALL:
                  {
								    ss << ass + "(" + os + ")";
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
              ss << lang_utils::operator_string(op) << "(" << os << ")";
            else
              ss << lang_utils::operator_string(op) << os;

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
						error.add("id", SLanguage);
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
            ss << os1 << " " << lang_utils::operator_string(op) << " " << os2;
            push_rendered(ss.str(), op_prec, variant());
            break;
          }

				case op_dot:
          {
            str caller_str = operand_to_string(arg1);

            expression_identifier ei = arg2;
            str right_str  = ei.value;

            //and who may I say is calling
            XSSObject caller = get_instance(arg1);
			      if (!caller)
			        {
				        if (arg1.is<expression_identifier>())
					        {
						        expression_identifier ei = arg1;
                    caller = variant_cast<XSSObject>(ctx_->resolve(ei.value, RESOLVE_ANY), XSSObject());
					        }
			        }

            bool var_caller = false;
            if (arg1.is<expression_identifier>())
              {
                expression_identifier left_ei = arg1;
                resolve_info left_ri;
                if (ctx_->resolve(left_ei.value, left_ri))
                  {
                    var_caller = left_ri.what == RESOLVE_VARIABLE;
                  }
              }

						str separator = ctx_->get_language()->resolve_separator(caller);
            if (caller)
              {
						    if (top && assigner)
							    {
                    str result;
                    str str_assigner = resolve_assigner(arg2, caller, assigner);

                    if (assigner->type == VANILLA || assigner->type == FN_CALL)
								      result = caller_str + separator + str_assigner;
                    else
                    if (assigner->type == FN_XSS_CALL)
                      result = str_assigner;
                    else
                      assert(false);

                    push_rendered(result, op_prec, variant());
                    return;
							    }

                resolve_info left;
                left.what  = RESOLVE_INSTANCE;
                left.value = caller;

                resolve_info right;
                right.left = &left;

                expression_identifier ei = arg2;
                if (ctx_->resolve(ei.value, right))
                  {
                    switch(right.what)
                      {
                        case RESOLVE_CHILD:
                        case RESOLVE_INSTANCE:
                          {
                            XSSObject obj = right.value;
                            push_rendered(caller->output_id() + separator + obj->output_id(), 0, obj);
                            return;
                          }
                        case RESOLVE_PROPERTY:
                          {
                            XSSProperty prop = right.value;
                            right_str = prop->output_id();

                            if (prop->has("property_xss") && !dynamic_get(prop, "property_xss").empty())
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
                                return;
							                }

                            str get_fn  = variant_cast<str>(dynamic_get(prop, "get_fn"), "");
                            str get_xss = variant_cast<str>(dynamic_get(prop, "get_xss"), "");
                            bool has_getter = prop && !prop->get.empty();

                            str result = caller->output_id() + separator;
                            if (var_caller)
                              result = caller_str + separator;

                            if (!get_xss.empty())
                              result += get_xss;
                            else if (!get_fn.empty())
                              result += get_fn + "()";
                            else if (has_getter)
                              result += right_str + "_get()";
                            else
                              result += right_str;

                            push_rendered(result, 0, prop);
								            return;
                          }
                        case RESOLVE_METHOD:
                          {
                            XSSMethod mthd = right.value;
                            right_str = mthd->output_id();

                            str result = caller_str + separator + right_str + "()";
                            push_rendered(result, 0, mthd);
								            return;
                          }
                        default:
                          {
                            assert(false); //use case
                          }
                      }
                  }
              }

            push_rendered(caller_str + separator + right_str, 0, variant());
            break;
          }

        case op_dot_call:
          {
            std::stringstream ss;

            str opnd1 = operand_to_string(arg1);
            str opnd2 = operand_to_string(arg2);

						str separator = ctx_->get_language()->resolve_separator();

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

XSSObject base_expr_renderer::get_instance(variant v)
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
        return ctx_->resolve(ei.value, RESOLVE_INSTANCE);
      }

    return XSSObject();
  }

XSSProperty base_expr_renderer::get_property(variant v)
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
        return ctx_->resolve(ei.value, RESOLVE_PROPERTY);
      }

    return XSSProperty();
  }

void base_expr_renderer::push_rendered(str value, int prec, variant object)
  {
    already_rendered ar;
    ar.object = object;
    ar.value = value;
    ar.precedence = prec;

    stack_.push(ar);
  }

str	base_expr_renderer::render_captured_property()
  {
		variant prop = stack_.top(); stack_.pop();
		str     value = operand_to_string(prop);

		XSSContext ctx(new xss_context(ctx_));

    ctx->register_symbol(RESOLVE_CONST, "path",	    value);
    ctx->register_symbol(RESOLVE_CONST, "property",	prop);

		XSSCompiler compiler = variant_cast<XSSCompiler>(ctx->resolve("compiler"), XSSCompiler()); assert(compiler);

		XSSRenderer rend = compiler->compile_xss(capture_property_.xss, ctx);
		return rend->render(XSSObject(), null);
  }

//base_args_renderer
base_args_renderer::base_args_renderer()
  {
  }

base_args_renderer::base_args_renderer(const base_args_renderer& other):
  args_(other.args_),
  ctx_(other.ctx_)
  {
  }

base_args_renderer::base_args_renderer(param_list_decl& params, XSSContext ctx):
  args_(params),
  ctx_(ctx)
  {
  }

//base_lang
str base_lang::resolve_this(XSSContext ctx)
  {
    return "this";
  }

str base_lang::resolve_separator(XSSObject lh)
  {
    return ".";
  }

bool base_lang::can_cast(XSSType left, XSSType right)
  {
    return true;
  }

void base_lang::init_context(XSSContext ctx)
  {
  }

XSSType base_lang::resolve_array_type(XSSType type, const str& at_name, XSSContext ctx)
  {
    XSSType new_type(new xss_type);
    new_type->set_id(at_name);
    new_type->as_array(type);

    return new_type;
  }

str base_lang::render_value(XSSType type, variant value)
  {
    return xss_utils::var_to_string(value);
  }
