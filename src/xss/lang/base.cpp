
#include <xss/lang/base.h>
#include <xss/xss_parser.h>
#include <xss/xss_error.h>
#include <xss/brace_parser.h>
#include <xss/xss_renderer.h>

#include <boost/functional/hash.hpp>
#include <boost/algorithm/string.hpp>

using namespace xkp;

const str SLanguage("language");

const str SEmptyExpression("Empty expression");
const str SAssignOperator("Assign operators can only be used as the base of an expression");
const str SGetNeedsText("Property.get expects a 'text' attribute");
const str SSetNeedsText("Property.set expects a 'text' attribute");
const str SCannontResolve("Unknown identifier");
const str SUnknownType("Cannot resolve type");
const str SUnknownTypeFromExpression("Cannot deduce type from assigned expression");
const str SCannotCast("Cannot cast variable value");
const str SIncrementOperatorOnlyTop("Increment operators cannot be used as part of an expression");
const str SCannotResolveParam("Cannot resolve constructor parameter");
const str SCannotResolveCtorProperty("Cannot resolve constructor property");
const str STooManyParamsInCtor("Too many params in constructor");
const str SUnknownDSL("Unkown dsl");
const str SCannotResolveType("Unkown type");
const str SOnlyOneCatch("Only One Catch, please");

//variable_gather
variable_gather::variable_gather(XSSContext ctx):
  ctx_(ctx)
  {
  }

void variable_gather::apply()
  {
    XSSType var_type = ctx_->get_type("var");

    var_list::iterator it = var_vars_.begin();
    var_list::iterator nd = var_vars_.end();
    for(; it != nd; it++)
      {
        ctx_->register_symbol(RESOLVE_VARIABLE, *it, var_type);
      }
  }

void variable_gather::variable_(stmt_variable& info)
  {
    XSSType result = ctx_->get_type(info.type);
    if (!result)
      {
        param_list error;
        error.add("id", SLanguage);
        error.add("desc", SUnknownType);
        error.add("var name", info.id);
        error.add("type", info.type);
        xss_throw(error);
      }

    XSSType value  = result;

    if (!info.value.empty())
      value = lang_utils::expr_type(info.value, ctx_);

    if (value && value != result)
      {
        if (result->is_variant())
          {
            result = value;
            if (!result)
              {
                param_list error;
                error.add("id", SLanguage);
                error.add("desc", SUnknownTypeFromExpression);
                error.add("var name", info.id);
                xss_throw(error);
              }
          }
        else
        if (ctx_->get_language()->can_cast(result, value))
          {
            if (result->is_array() && result->array_type()->is_variant())
              {
                result = value;
              }
          }
        else
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SCannotCast);
            error.add("var name", info.id);
            error.add("var type", result->id());
            error.add("value type", value->id());
            xss_throw(error);
          }
      }

    if (result->is_variant())
      var_vars_.push_back(info.id);
    else
      ctx_->register_symbol(RESOLVE_VARIABLE, info.id, result);
  }

void variable_gather::if_(stmt_if& info)
  {
    lang_utils::var_gatherer(info.if_code, ctx_);

    if (!info.else_code.empty())
			{
        lang_utils::var_gatherer(info.else_code, ctx_);
			}
  }

void variable_gather::for_(stmt_for& info)
  {
    variable_gather vg(ctx_);
    vg.variable_(info.init_variable);

    lang_utils::var_gatherer(info.for_code, ctx_);
  }

void variable_gather::iterfor_(stmt_iter_for& info)
  {
    XSSType result = ctx_->get_type(info.type.name);
    if (!result)
      {
        param_list error;
        error.add("id", SLanguage);
        error.add("desc", SUnknownType);
        error.add("var name", info.id);
        error.add("type", info.type);
        xss_throw(error);
      }

    ctx_->register_symbol(RESOLVE_VARIABLE, info.id, result);

    lang_utils::var_gatherer(info.for_code, ctx_);
  }

void variable_gather::while_(stmt_while& info)
  {
    lang_utils::var_gatherer(info.while_code, ctx_);
  }

void variable_gather::expression_(stmt_expression& info)
  {
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

                if (lt && lt->is_array())
                  {
                    XSSType lta = lt->array_type();
                    if (lta != rt && !ctx_->get_language()->can_cast(lta, rt))
                      {
                        param_list error;
                        error.add("id", SLanguage);
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

                if (!rt) break; //render pure text

                //resolve type of left expression
                XSSType lt = lang_utils::expr_type(es.left, ctx_);

                if ((!lt || (lt->is_variant()) && !rt->is_variant()))
                  {
                    variant obj = lang_utils::object_expr(es.left, ctx_);

                    if (obj.is<expression_identifier>())
                      {
                        expression_identifier ei = obj;
                        str ei_name = ei.value;

                        if (!ei_name.empty())
                          {
                            bool found = false;
                            var_list::iterator it = var_vars_.begin();
                            var_list::iterator nd = var_vars_.end();
                            for(; it != nd; it++)
                              {
                                if (*it == ei_name)
                                  {
                                    var_vars_.erase(it); //no need to check no mo
                                    ctx_->register_symbol(RESOLVE_VARIABLE, ei_name, rt);
                                    found = true;
                                    break;
                                  }
                              }

                            if (!found)
                              {
                                param_list error;
                                error.add("id", SLanguage);
                                error.add("desc", SCannontResolve);
                                error.add("identifier", ei_name);
                                xss_throw(error);
                              }
                          }
                      }
                    else
                    if (obj.is<XSSProperty>())
                      {
                        XSSProperty prop = variant_cast<XSSProperty>(obj, XSSProperty());

                        if (prop)
                          {
                            prop->set_type(rt);
                          }
                      }

                    lt = rt;
                  }

                if (!lt) break; //render pure text

                //check types
                if (lt != rt && !lt->is_variant() && !rt->is_variant())
                  {
                    if (!ctx_->get_language()->can_cast(lt, rt))
                      {
                        param_list error;
                        error.add("id", SLanguage);
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

//utils
XSSRenderer compile_braces(const str& text, XSSContext ctx)
  {
    brace_parser parser;
    XSSCompiler  compiler = ctx->resolve("compiler");

    xss_renderer* result = new xss_renderer(compiler, ctx, fs::path());
    result->context()->set_args(ctx->get_args());

    parser.parse(text, result);

    return XSSRenderer(result);
  }

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
    Language lang = ctx_->get_language();
    lang->init_context(ctx_);
  }

base_code_renderer::base_code_renderer(code& cde, param_list_decl& params, XSSContext ctx, int indent):
  code_(cde),
  ctx_(new xss_context(ctx)),
  indent_(indent),
  return_type_(false)
  {
    Language lang = ctx_->get_language();
    lang->init_context(ctx_);

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

    //register our variables into the context
    lang_utils::var_gatherer(code_, ctx_);
  }

code& base_code_renderer::get_code()
  {
    return code_;
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
    XSSDSL dsl = ctx_->get_xss_dsl(info.name);
    if (!dsl)
      {
        param_list error;
        error.add("id", SLanguage);
        error.add("desc", SUnknownDSL);
        error.add("dsl", info.name);
        xss_throw(error);
      }

    dsl->render(info, ctx_);
  }

void base_code_renderer::dispatch(stmt_dispatch& info)
  {
    assert(false); //td: ought to define what to do here, it would seem like the idiom would like
                    //to handle this
  }

bool constant_switch(stmt_switch& info)
  {
    if (info.expr.empty())
      return false;

    std::vector<switch_section>::iterator it = info.sections.begin();
    std::vector<switch_section>::iterator nd = info.sections.end();
    for(; it != nd; it++)
      {
        std::vector<expression>::iterator cit = it->cases.begin();
        std::vector<expression>::iterator cnd = it->cases.end();

        for(; cit != cnd; cit++)
          {
            variant v;
		        if (!cit->is_constant(v))
              return false;
          }
      }

    return true;
  }

void base_code_renderer::switch_(stmt_switch& info)
  {
    if (constant_switch(info))
      {
        add_line("switch(" + render_expression(info.expr, ctx_) + ")");
		    add_line("{");

          std::vector<switch_section>::iterator it = info.sections.begin();
          std::vector<switch_section>::iterator nd = info.sections.end();
          for(; it != nd; it++)
            {
              std::vector<expression>::iterator cit = it->cases.begin();
              std::vector<expression>::iterator cnd = it->cases.end();

              for(; cit != cnd; cit++)
                {
		              add_line("case " + render_expression(*cit, ctx_) + " : ");
                }
		          
              add_line("{");
                render_code(it->case_code);

                if (!it->case_code.empty())
                  {
                    variant& last_stmt = it->case_code.get_stament(it->case_code.size() - 1);
                    if (!last_stmt.is<stmt_break>())
                      add_line("break;");
                  }
		          add_line("}");
            }

          if (!info.default_code.empty())
            {
		          add_line("default: ");
              add_line("{");
                render_code(info.default_code);
		          add_line("}");
            }
		    
        add_line("}");
      }
    else
      {
        //generate as ifs
        std::vector<switch_section>::iterator it = info.sections.begin();
        std::vector<switch_section>::iterator nd = info.sections.end();
        bool first = true;
        for(; it != nd; it++)
          {
            str case_expr;

            if (it->cases.size() == 1)
              case_expr = render_expression(it->cases[0], ctx_);
            else
              {
                std::vector<expression>::iterator cit = it->cases.begin();
                std::vector<expression>::iterator cnd = it->cases.end();
                bool cfirst = true;
                for(; cit != cnd; cit++)
                  {
                    if (cfirst)  
                      cfirst = false;
                    else 
                      case_expr += " || ";

		                case_expr += "(" + render_expression(*cit, ctx_) + ")";
                  }
              }
		          
            if (first)  
              {
                first = false;
                add_line("if (" + case_expr + ")");
              }
            else 
                add_line("else if (" + case_expr + ")");
            
            add_line("{");
              render_code(it->case_code);
		        add_line("}");
          }

        if (!info.default_code.empty())
          {
		        add_line("else");
            add_line("{");
              render_code(info.default_code);
		        add_line("}");
          }
      }
  }

void base_code_renderer::try_(stmt_try& info)
  {
    add_line("try");
		add_line("{");
      render_code(info.try_code);
		add_line("}");

    if (!info.catches.empty())
      {
        if (info.catches.size() > 1)
          {
            //lazy me
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SOnlyOneCatch);
            xss_throw(error);
          }

        add_line("catch(__exception)");
		    add_line("{");
          catch_& lonely = info.catches[0];
          if (!lonely.id.empty())
		        add_line("var " + lonely.id + " = __exception;");
            render_code(lonely.catch_code);
		    add_line("}");
      }

    if (!info.finally_code.empty())
      {
        add_line("finally");
		    add_line("{");
            render_code(info.finally_code);
		    add_line("}");
      }
  }

void base_code_renderer::throw_(stmt_throw& info)
  {
    add_line("throw " + render_expression(info.expr, ctx_) + ";");
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
    Language lang = ctx_->get_language();
    lang->init_context(ctx_);
  }

base_expr_renderer::base_expr_renderer(expression& expr, XSSContext ctx):
  assigner(null),
  capturing_property_(false),
  ctx_(ctx),
  expr_(expr)
  {
    Language lang = ctx_->get_language();
    lang->init_context(ctx_);
  }

str base_expr_renderer::operand_to_string(variant operand, XSSObject parent, int* prec)
  {
    str result;
    int result_prec = 0;
    Language lang = ctx_->get_language();

    if (operand.is<expression_identifier>())
      {
        expression_identifier ei = operand;

        if (ei.value == "this")
          result = lang->resolve_this(ctx_);
        else
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
                    case RESOLVE_TYPE:
                      {
                        XSSType type = si.value;
                        result = type->id();
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
                    return lang->property_get(prop, lang->resolve_this(ctx_), ctx_);
                  }
                case RESOLVE_INSTANCE:
                  {
                    XSSObject obj = si.value;
                    return obj->output_id();
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
    code_context cctx;
    cctx = ctx->get_compile_context();
    xs_utils xs;
    return xs.evaluate_xs_expression(expr_, cctx, file);
  }

void base_expr_renderer::push(variant operand, bool top)
  {
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
            if (!top)
              {
						    param_list error;
						    error.add("id", SLanguage);
						    error.add("desc", SIncrementOperatorOnlyTop);

						    xss_throw(error);
              }

						int         prec;
            Language    lang = ctx_->get_language();
            XSSProperty prop = get_property(arg1);
            str         os   = operand_to_string(arg1, XSSObject(), &prec);

            //find the path
            str path;
            if (arg1.is<already_rendered>())
              {
                already_rendered ar = arg1;
                path = ar.path;
              }
            else if (prop)
              {
                //no path on a property, must belong to this
                path = lang->resolve_this(ctx_);
              }

            str result;
            if (prop)
              {
								str op_str = lang_utils::operator_string(op);
								op_str.erase(op_str.end() - 1); //now this is too much fun

                result = lang->property_get(prop, path, ctx_);
                result = lang->property_set(prop, path, result + " " + op_str + " 1", ctx_);
              }
            else
              result = os + lang_utils::operator_string(op);

            push_rendered(result, op_prec, arg1.get_schema(), path);
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

            push_rendered(ss.str(), op_prec, arg1.get_schema(), os);
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

            XSSType xstype1 = lang_utils::resolve_type(arg1, ctx_);
            XSSType xstype2 = lang_utils::resolve_type(arg2, ctx_);

            Language lang = ctx_->get_language();

            str result;
            if (lang->custom_operator(xstype1, xstype2, os1, os2, op, result))
              {
                push_rendered(result, op_prec, variant());
              }
            else
              {
                if (op_prec < p1)
                  os1 = "(" + os1 + ")";

                if (op_prec < p2)
                  os2 = "(" + os2 + ")";

                std::stringstream ss;
                ss << os1 << " " << lang_utils::operator_string(op) << " " << os2;
                push_rendered(ss.str(), op_prec, variant());
              }
            break;
          }

				case op_dot:
          {
            Language lang = ctx_->get_language();

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
                else if (arg1.is<already_rendered>())
                  {
                    already_rendered ar = arg1;
                    caller = variant_cast<XSSObject>(ar.object, XSSObject());
                  }
			        }

						str separator = ctx_->get_language()->resolve_separator(caller);
            if (caller)
              {
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
                            push_rendered(caller->output_id() + separator + obj->output_id(), 0, obj, caller->output_id());
                            return;
                          }
                        case RESOLVE_PROPERTY:
                          {
                            XSSProperty prop = right.value;
                            push_rendered(lang->property_get(prop, caller_str, ctx_), 0, prop, caller_str);
                            return;
                          }
                        case RESOLVE_METHOD:
                          {
                            XSSMethod mthd = right.value;
                            right_str = mthd->output_id();

                            str result = caller_str + separator + right_str + "()";
                            push_rendered(result, 0, mthd, caller_str);
								            return;
                          }
                        default:
                          {
                            assert(false); //use case
                          }
                      }
                  }
              }

            push_rendered(caller_str + separator + right_str, 0, variant(), caller_str);
            break;
          }

        case op_dot_call:
          {
            Language lang = ctx_->get_language();

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
                else if (arg1.is<already_rendered>())
                  {
                    already_rendered ar = arg1;
                    caller = variant_cast<XSSObject>(ar.object, XSSObject());
                  }
			        }

						str separator = ctx_->get_language()->resolve_separator(caller);
            variant call_obj;
            if (caller)
              {
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
                        case RESOLVE_PROPERTY:
                          {
                            assert(false);
                            break; //td: error
                          }
                        case RESOLVE_METHOD:
                          {
                            XSSMethod mthd = right.value;
                            call_obj = mthd;
                            right_str = mthd->output_id();
                            break;
                          }
                        default:
                          {
                            assert(false); //use case
                          }
                      }
                  }
              }

            push_rendered(caller_str + separator + right_str, 0, call_obj, caller_str);
            break;
          }

        case op_instantiate:
          {
            std::stringstream result;
            xs_type caller = arg1;
            int     args   = arg2;

            //pop the arguments
			      std::vector<str> params;
            for(int i = 0; i < args; i++)
              {
                variant arg  = stack_.top(); stack_.pop();
                str     sarg = operand_to_string(arg);
                params.push_back(sarg);
			        }
            
            XSSType type = ctx_->get_type(caller.name);

            if (!type)
              {
                param_list error;
                error.add("id", SLanguage);
                error.add("desc", SCannotResolveType);
                error.add("type", type->id());
                xss_throw(error);
              }
            
            push_rendered(render_instantiation(type, params), op_prec, variant());
            break;
          }

        case op_object:
          {
            //td: !!! move to js
            std::stringstream result;
            std::vector<xs_const> values = arg1;

            result << "{";
            
            std::vector<xs_const>::iterator it = values.begin();
            std::vector<xs_const>::iterator nd = values.end();
            bool first = true;
            for(; it != nd; it++)
              {
                if (first)
                  first = false;
                else
                  result << ',';
                  
                result << it->name << " : " << render_expression(it->value);
              }
            
            result << "}";

            push_rendered(result.str(), op_prec, variant());
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

            XSSType instantiation = ctx_->get_type(caller);
            if (instantiation)
              push_rendered(render_instantiation(instantiation, params), op_prec, variant());
            else
              push_rendered(result.str(), op_prec, variant(), caller); //td: we could find out the type here or something
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

            bool gotit = false;
            if (top.is<already_rendered>())
              {
                already_rendered arr = top;
                XSSMethod mthd = variant_cast<XSSMethod>(arr.object, XSSMethod());
                if (mthd && mthd->get<bool>("ignore_args", false))
                  {
                    push_rendered(caller, op_prec, variant(), caller); 
                    break;
                  }
              }

            push_rendered(caller + result.str(), op_prec, variant(), caller); //td: we could find out the type here or something
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
            str os1 = operand_to_string(arg1);
						result << os1 << "[" << operand_to_string(arg2) << "]";
            push_rendered(result.str(), op_prec, variant(), os1);
            break;
          }

				default:
          assert(false); //td:
      }
  }

str base_expr_renderer::render_instantiation(XSSType type, std::vector<str>& params)
  {
    //grab parameter info
    //td: types
    DynamicArray info(new dynamic_array);
    std::vector<str>::reverse_iterator pit = params.rbegin();
    std::vector<str>::reverse_iterator pnd = params.rend();
    for(; pit != pnd; pit++)
      {
        XSSObject param_info(new xss_object());
        param_info->add_attribute("value", *pit);

        info->push_back(param_info);
      }

    str xss = type->get<str>("instantiator", str());
    if (!xss.empty())
      {
        //prepare rendering parameters
        param_list args;
        args.add("type", type);
        args.add("runtime_args", info);

        XSSCompiler compiler = ctx_->resolve("compiler");
        fs::path type_path   = compiler->type_path(type->id());

        XSSContext  ctx(new xss_context(ctx_, type_path));
        XSSRenderer rend = compiler->compile_xss_file(xss, ctx);
        return rend->render(XSSObject(), &args);
      }
    else
      {
        Language lang = ctx_->get_language();
        param_list pl;
        return lang->instantiate(type, XSSObject(), info, pl);
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

void base_expr_renderer::push_rendered(str value, int prec, variant object, const str& path)
  {
    already_rendered ar;
    ar.object = object;
    ar.value = value;
    ar.precedence = prec;
    ar.path = path;

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

void base_args_renderer::add(const str& name, XSSType type)
  {
    extra_.push_back(std::pair<str, XSSType>(name, type));
  }

param_list_decl& base_args_renderer::get()
  {
    return args_;
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

void base_lang::init_application_context(XSSContext ctx)
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

str base_lang::render_expression(expression& expr, XSSContext ctx)
  {
    variant              vv   = compile_expression(expr, ctx);
    IExpressionRenderer* rend = vv;
    return rend->render();
  }

str base_lang::render_ctor_args(XSSType type, XSSObject instance, DynamicArray rt, param_list& args)
  {
    DynamicArray ctor_params = type->ctor_args();

    std::stringstream ss;
    std::vector<variant>::iterator it = ctor_params->ref_begin();
    std::vector<variant>::iterator nd = ctor_params->ref_end();

    bool   first = true;
    size_t curr  = 0;
    for(; it != nd; it ++)
      {
        str       value;
        XSSObject p = *it;
        str       constant = p->get<str> ("constant", str());
        str       prperty  = p->get<str> ("property", str());
        bool      runtime  = p->get<bool>("runtime",  false);
        if (!constant.empty())
          {
            value = constant;
          }
        else if (runtime && rt)
          {
            if (curr < rt->size())
              {
                XSSObject param_value = rt->at(curr++);
                          value       = param_value->get<str>("value", str()); 
              }
          }

        if (value.empty() && args.has(p->id()))
          {
            value = variant_cast<str>(args.get(p->id()), str());
          }

        if (value.empty())
          {
            if (!prperty.empty())
              {
                XSSProperty prop;
                if (instance)
                  prop = instance->get_property(prperty);
                else
                  prop = type->get_property(prperty);

                if (!prop)
                  {
                    param_list error;
                    error.add("id", SLanguage);
                    error.add("desc", SCannotResolveCtorProperty);
                    error.add("type", type->id());
                    error.add("property", prperty);
                    xss_throw(error);
                  }

                value = prop->render_value();
              }
            else
              {
                if (p->has("default_value"))
                  value = p->get<str> ("default_value", str());
                
                if (value.empty())
                  {
                    param_list error;
                    error.add("id", SLanguage);
                    error.add("desc", SCannotResolveParam);
                    error.add("type", type->id());
                    error.add("parameter", p->id());
                    xss_throw(error);
                  }
              }


          }

        if (first)
          first = false;
        else
          ss << ", ";

        ss << value;
      }
    return ss.str();
  }

str base_lang::instantiate(XSSType type, XSSObject instance, DynamicArray rt, param_list& args)
  {
    str class_name  = type->output_id();

    std::stringstream ss;
    ss << "new " << class_name << "(";
    ss << render_ctor_args(type, instance, rt, args);
    ss << ")";
    return ss.str();
  }

bool base_lang::custom_operator(XSSType lt, XSSType rt, str l, str r, operator_type op, str& res)
  {
    return false;
  }

void base_lang::compile_property(XSSProperty prop, XSSContext ctx)
  {
    XSSObject get = prop->find("get");
    XSSObject set = prop->find("set");

    if (get)
      {
        str text = get->get<str>("text", str());
        if (text.empty())
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SGetNeedsText);
            error.add("property", prop->id());
            xss_throw(error);
          }

        boost::hash<std::string> string_hash;
        int  hash       = string_hash(text);
        int  stored     = prop->get("#get_renderer_hash", 0);
        bool do_compile = true;
        if (stored != 0)
          {
            do_compile = stored != hash;
          }

        if (do_compile)
          {
            prop->set("#get_renderer_hash", hash);

            XSSContext my_ctx(new xss_context(ctx));
            my_ctx->add_parameter("path", ctx->get_type("string"));

            XSSRenderer rend = compile_braces(text, my_ctx);
            prop->add_attribute("#get_renderer", rend);
          }
      }

    if (set)
      {
        str text = set->get<str>("text", str());
        if (text.empty())
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SSetNeedsText);
            error.add("property", prop->id());
            xss_throw(error);
          }

        boost::hash<std::string> string_hash;
        int  hash       = string_hash(text);
        int  stored     = prop->get("#set_renderer_hash", 0);
        bool do_compile = true;
        if (stored != 0)
          {
            do_compile = stored != hash;
          }

        if (do_compile)
          {
            prop->set("#set_renderer_hash", hash);

            XSSContext my_ctx(new xss_context(ctx));
            my_ctx->add_parameter("path",  ctx->get_type("string"));
            my_ctx->add_parameter("value", ctx->get_type("string"));

            XSSRenderer rend = compile_braces(text, my_ctx);
            prop->add_attribute("#set_renderer", rend);
          }
      }
  }
