
#include <xss/lang/typed.h>
#include <xss/xss_expression.h>

using namespace xkp;

//typed_lang
XSSContext typed_lang::create_context()
  {
    XSSContext result(new xss_context(XSSContext()));

    result->add_type(XSSType(new xss_type("int",    type_schema<int>())), str());
    result->add_type(XSSType(new xss_type("float",  type_schema<float>())), str());
    result->add_type(XSSType(new xss_type("double", type_schema<double>())), str());
    result->add_type(XSSType(new xss_type("bool",   type_schema<bool>())), str());
    result->add_type(XSSType(new xss_type("string", type_schema<str>())), str());

    XSSType var_type = result->add_type(XSSType(new xss_type("var", null)), str());
    var_type->as_variant();

    XSSType object_type = result->add_type(XSSType(new xss_type("object", null)), str());
    object_type->as_variant();

    XSSType array_type = result->add_type(XSSType(new xss_type("array", null)), str());
    array_type->as_array(var_type);

    return result;
  }

bool typed_lang::render_code(XSSCode code, XSSContext ctx, std::ostringstream& result)
  {
    statement_list& sl = code->statements();
    statement_list::iterator it = sl.begin();
    statement_list::iterator nd = sl.end();

    for(; it != nd; it++)
      {
        XSSStatement st = *it;
        switch(st->id())
          {
            case STATEMENT_IF:
              {
                IStatementIf* info = st->cast<IStatementIf>(); assert(info);
                if (!render_if(info, ctx, result))
                  return false;
                break;
              }
            case STATEMENT_VAR:
              {
                IStatementVar* info = st->cast<IStatementVar>(); assert(info);
                if (!render_variable(info, ctx, result))
                  return false;
                break;
              }
            case STATEMENT_FOR:
              {
                IStatementFor* info = st->cast<IStatementFor>(); assert(info);
                if (!render_for(info, ctx, result))
                  return false;
                break;
              }
            case STATEMENT_FOREACH:
              {
                IStatementForEach* info = st->cast<IStatementForEach>(); assert(info);
                if (!render_foreach(info, ctx, result))
                  return false;
                break;
              }
            case STATEMENT_WHILE:
              {
                IStatementWhile* info = st->cast<IStatementWhile>(); assert(info);
                if (!render_while(info, ctx, result))
                  return false;
                break;
              }
            case STATEMENT_SWITCH:
              {
                IStatementSwitch* info = st->cast<IStatementSwitch>(); assert(info);
                if (!render_switch(info, ctx, result))
                  return false;
                break;
              }
            case STATEMENT_TRY:
              {
                IStatementTry* info = st->cast<IStatementTry>(); assert(info);
                if (!render_try(info, ctx, result))
                  return false;
                break;
              }
            case STATEMENT_BREAK:
              {
                if (!render_break(ctx, result))
                  return false;
                break;
              }
            case STATEMENT_CONTINUE:
              {
                if (!render_continue(ctx, result))
                  return false;
                break;
              }
            case STATEMENT_RETURN:
              {
                IStatementExpression* info = st->cast<IStatementExpression>(); assert(info);
                if (!render_return(info, ctx, result))
                  return false;
                break;
              }
            case STATEMENT_EXPRESSION:
              {
                IStatementExpression* info = st->cast<IStatementExpression>(); assert(info);
                if (!render_expr_statement(info, ctx, result))
                  return false;
                break;
              }
            case STATEMENT_THROW:
              {
                IStatementExpression* info = st->cast<IStatementExpression>(); assert(info);
                if (!render_throw(info, ctx, result))
                  return false;
                break;
              }
            default:
              assert(false); //td: handle new statements
          }
      }

    return true;
  }

bool typed_lang::render_expression(XSSExpression expr, XSSContext ctx, std::ostringstream& result)
  {
    XSSValue value = expr->value();
    if (value)
      return render_value(value, ctx, result);

    if (expr->is_assign())  
      {
        XSSValue left_value = expr->left()->value(); assert(left_value);
        return render_assignment(expr->op(), left_value, expr->right(), ctx, result);
      }
    
    return render_operator(expr, ctx, result);
  }

bool typed_lang::render_operator(XSSExpression expr, XSSContext ctx, std::ostringstream& result)
  {
    int op_prec = operator_utils::precedence(expr->op());
    XSSExpression left  = expr->left();
    XSSExpression right = expr->right();

    XSSOperator xop = expr->xop();
    if (xop)
      {
        InlineRenderer ir = xop->renderer();
        if (ir)
          {
            std::ostringstream left_text;
            std::ostringstream right_text;
            if (!render_expression(left, ctx, left_text) || !render_expression(right, ctx, right_text))
              return false;
            
            param_list pl;
            pl.add("left", left_text.str());
            pl.add("right", right_text.str());
            ir->render(pl, result);
            return true;
          }
      }

    //typical expression, render left << op << right
    if (left->value())
      {
        if (!render_value(left->value(), ctx, result))
          return false;
      }
    else
      {
        int lprec = operator_utils::precedence(left->op());
        if (op_prec < lprec)
          result << "(";

        if (!render_expression(left, ctx, result))
          return false;

        if (op_prec < lprec)
          result << ")";
      }

    //op
    result << " " << operator_utils::to_string(expr->op()) << " ";

    //right
    if (right->value())
      {
        if (!render_value(right->value(), ctx, result))
          return false;
      }
    else
      {
        int rprec = operator_utils::precedence(right->op());
        if (op_prec < rprec)
          result << "(";

        if (!render_expression(right, ctx, result))
          return false;

        if (op_prec < rprec)
          result << ")";
      }

    return true;
  }

bool typed_lang::render_value(XSSValue val, XSSContext ctx, std::ostringstream& result)
  {
    value_operations& ops = val->operations();
    value_operations::iterator it = ops.begin();
    value_operations::iterator nd = ops.end();

    std::ostringstream my_value;
    bool               first = true; 
    for(; it != nd; it++)
      {
        switch(it->id())
          {
            case OP_CONSTANT:
              {
                if (!render_constant(it->constant(), ctx, my_value))
                  return false;
                break;
              }
            case OP_READ:
              {
                if (it->bound())
                  {
                    if (!render_read_operation(*it, ctx, my_value))
                      return false;
                  }
                else
                  {
                    if (!first)
                      my_value << ".";
                      
                    str id = it->identifier();
                    if (id.empty())
                      return false;

                    my_value << id;
                  }
                break;
              }
            case OP_WRITE: 
              {
                assert(false); //should not get here
                break;
              }
            case OP_CALL:
              {
                if (it->bound())
                  {
                    if (!render_call(*it, ctx, my_value))
                      return false;
                  }
                else
                  {
                    if (!first)
                      my_value << ".";
                      
                    str id = it->identifier();
                    if (id.empty())
                      return false;

                    my_value << id << "(";
                    if (!render_arguments(it->args(), ctx, my_value))
                      return false;
                      
                    my_value << ")";
                    break;
                  }
                break;
              }
            case OP_INDEX:
              {
                if (it->bound())
                  {
                    if (!render_index_operation(*it, ctx, my_value))
                      return false;
                  }
                else
                  {
                    if (!first)
                      my_value << ".";
                      
                    str id = it->identifier();
                    if (id.empty())
                      return false;

                    my_value << id << "[";
                    if (!render_arguments(it->args(), ctx, my_value))
                      return false;
                      
                    my_value << "]";
                    break;
                  }
                break;
              }
            case OP_INSTANTIATION:
              {
                if (it->bound())
                  {
                    if (!render_index_operation(*it, ctx, my_value))
                      return false;
                  }
                else
                  {
                    assert(first);
                    str id = it->identifier();
                    if (id.empty())
                      return false;

                    my_value << "new " << id << "(";
                      
                    if (!render_arguments(it->args(), ctx, my_value))
                      return false;

                    my_value << ")";
                    break;
                  }
                break;
              }
            case OP_OBJECT:
              {
                assert(first);
                if (!render_object(*it, ctx, my_value))
                  return false;
                break;
              }
            case OP_ARRAY:
              {
                assert(first);
                if (!render_array(*it, ctx, my_value))
                  return false;
                break;
              }
            default: assert(false);
          }

        if (first)
          first = false;
      }

    result << my_value.str();
    return true;
  }

bool typed_lang::render_if(IStatementIf* info, XSSContext ctx, std::ostringstream& result)
  {
    result << '\n' << "if (";
    if (!render_expression(info->expr(), ctx, result))
      return false;
    result << ')';

    XSSCode if_code = info->if_code();
    XSSCode else_code = info->else_code();
    if (if_code)
      {
        result << '\n' << "{";
        if (!render_code(if_code, ctx, result))
          return false;
        result << '\n' << "}";
      }

    if (else_code)
      {
        result << '\n' << "{";
        if (!render_code(else_code, ctx, result))
          return false;
        result << '\n' << "}";
      }

    return true;
  }

bool typed_lang::render_variable(IStatementVar* info, XSSContext ctx, std::ostringstream& result)
  {
    str type_name = info->type_name();
    if (info->type())
      type_name = info->type()->output_id();

    result << '\n' << type_name << " " << info->id();

    XSSExpression value = info->value();
    if (value)
      {
        result << " = ";
        if (!render_expression(value, ctx, result))
          return false;
      }
      
    result << ";";
    return true;
  }

bool typed_lang::render_for(IStatementFor* info, XSSContext ctx, std::ostringstream& result)
  {
    result << '\n' << "for(";

    if (info->id().empty())
      {
        XSSExpression init_expr = info->init_expr();
        if (init_expr && !render_expression(init_expr, ctx, result))
          return false;
      }
    else
      {
        //there is a variable declaration in there
        XSSType var_type = info->type(); assert(var_type);
        result << var_type->output_id() << " " << info->id();

        XSSExpression init_value = info->init_value();
        if (init_value)
          {
            result << " = ";
            if (!render_expression(init_value, ctx, result))
              return false;
          }
      }
    
    result << ';';
    
    XSSExpression cond_expr = info->cond_expr();
    if (cond_expr && !render_expression(cond_expr, ctx, result))
          return false;
      
    XSSExpression iter_expr = info->iter_expr();
    if (iter_expr && !render_expression(iter_expr, ctx, result))
          return false;

    XSSCode for_code = info->for_code();
    result << '\n' << "{";
    if (for_code && !render_code(for_code, ctx, result))
      return false;

    result << '\n' << "}";
    return true;
  }

bool typed_lang::render_while(IStatementWhile* info, XSSContext ctx, std::ostringstream& result)
  {
    result << '\n' << "while(";

    XSSExpression while_expr = info->expr();
    if (!while_expr || !render_expression(while_expr, ctx, result))
      return false;

    result << ')';

    XSSCode while_code = info->code();
    result << '\n' << "{";
    if (while_code && !render_code(while_code, ctx, result))
      return false;

    result << '\n' << "}";
    return true;
  }

bool typed_lang::render_switch(IStatementSwitch* info, XSSContext ctx, std::ostringstream& result)
  {
    XSSExpression switch_expr = info->expr();
    if (switch_expr)
      {
        XSSType switch_expr_type = switch_expr->type(); assert(switch_expr_type);

        result << '\n' << "switch(";
        
        if (!render_expression(switch_expr, ctx, result))
          return false;

        result << ')';
        result << '\n' << "{";

          switch_sections& sections = info->sections();
          switch_sections::iterator it = sections.begin();
          switch_sections::iterator nd = sections.end();

          for(; it != nd; it++)
            {
              std::vector<XSSExpression>::iterator cit = it->cases.begin();
              std::vector<XSSExpression>::iterator cnd = it->cases.end();

              for(; cit != cnd; cit++)
                {
                  XSSExpression case_expr = *cit;
                  assert(!case_expr->is_constant());
                  variant case_value = case_expr->value()->constant();

                  result << "\ncase ";
                  if (!render_constant(case_value, ctx, result))
                    return false;

                  result << ":";
                }

              result << '\n' << "{";

              XSSCode case_code = it->case_code;
              if (case_code && !render_code(case_code, ctx, result))
                return false;

              result << '\n' << "break;";
              result << '\n' << "}";
            }

          XSSCode default_code = info->default_code();
          if (default_code)
            {
              result << '\n' << "default:";
              result << '\n' << "{";

              if (!render_code(default_code, ctx, result))
                return false;

              result << '\n' << "break;";
              result << '\n' << "}";
            }

        result << '\n' << "}";
      }
    else
      {
        //generate bunch of ifs
        switch_sections& sections = info->sections();
        switch_sections::iterator it = sections.begin();
        switch_sections::iterator nd = sections.end();

        bool first = true;
        for(; it != nd; it++)
          {
            if (first)
              first = false;
            else
              result << '\n' << "else ";

            result << '\n' << "if (";

            std::vector<XSSExpression>::iterator cit = it->cases.begin();
            std::vector<XSSExpression>::iterator cnd = it->cases.end();

            bool cfirst = true;
            for(; cit != cnd; cit++)
              {
                if (cfirst)
                  cfirst = false;
                else
                  result << " || ";

                XSSExpression case_expr = *cit;
                if (!render_expression(case_expr, ctx, result))
                  return false;
              }

            result << ")";

            result << '\n' << "{";

            XSSCode case_code = it->case_code;
            if (case_code && !render_code(case_code, ctx, result))
              return false;

            result << '\n' << "}";
          }

        XSSCode default_code = info->default_code();
        if (default_code)
          {
            result << '\n' << "else";
            result << '\n' << "{";

            if (!render_code(default_code, ctx, result))
              return false;

            result << '\n' << "break;";
            result << '\n' << "}";
          }
      }
    
    return true;
  }

bool typed_lang::render_try(IStatementTry* info, XSSContext ctx, std::ostringstream& result)
  {
    result << '\n' << "try";
    result << '\n' << "{";

    XSSCode try_code = info->try_code();
    if (try_code && !render_code(try_code, ctx, result))
      return false;

    result << '\n' << "}";

    catch_sections& sections = info->sections();
    catch_sections::iterator it = sections.begin();
    catch_sections::iterator nd = sections.end();

    for(; it != nd; it++)
      {
        //td: !!! catch type unresolved
        result << '\n' << "catch(" << it->type << " " << it->id << ")";
        result << '\n' << "{";

        if (!render_code(it->catch_code, ctx, result))
          return false;

        result << '\n' << "}";
      }

    XSSCode finally_code = info->finally_code();
    if (finally_code)
      {
        result << '\n' << "finally";
        result << '\n' << "{";

        if (!render_code(finally_code, ctx, result))
          return false;

        result << '\n' << "}";
      }

    return true;
  }

bool typed_lang::render_break(XSSContext ctx, std::ostringstream& result)
  {
    result << '\n' << "break;";
    return true;
  }

bool typed_lang::render_continue(XSSContext ctx, std::ostringstream& result)
  {
    result << '\n' << "continue;";
    return true;
  }

bool typed_lang::render_return(IStatementExpression* info, XSSContext ctx, std::ostringstream& result)
  {
    result << '\n' << "return ";
    
    XSSExpression return_expr = info->expr();
    if (return_expr && !render_expression(return_expr, ctx, result))
      return false;

    result << ';';
    return true;
  }

bool typed_lang::render_expr_statement(IStatementExpression* info, XSSContext ctx, std::ostringstream& result)
  {
    result << '\n';

    XSSExpression return_expr = info->expr();
    if (return_expr && !render_expression(return_expr, ctx, result))
      return false;

    result << ';';
    return true;
  }

bool typed_lang::render_throw(IStatementExpression* info, XSSContext ctx, std::ostringstream& result)
  {
    result << '\n' << "throw ";
    
    if (!render_expression(info->expr(), ctx, result))
      return false;

    result << ';';
    return true;
  }

bool typed_lang::render_assignment(operator_type op, XSSValue left_value, XSSExpression right, XSSContext ctx, std::ostringstream& result)
  {
    assert(op == op_assign); 

    if (left_value->bound())
      {
        XSSValue path = left_value->path();
        str           path_str;
        if (path)
          {
            std::ostringstream path_;
            if (!render_value(path, ctx, path_))
              return false;

            path_str = path_.str();
          }

        value_operation& vop = left_value->get_last();
        switch(vop.resolve_id())
          {
            case RESOLVE_PROPERTY:
              {
                bool this_property = path_str.empty();
                XSSProperty prop = vop.resolve_value();

                assert(!prop->is_const());

                InlineRenderer prop_setter = prop->setter();
                if (prop_setter)
                  {
                    std::ostringstream value_;
                    if (!render_expression(right, ctx, value_))
                      return false;
                    
                    param_list params;
                    params.add("path", path);
                    params.add("value", value_.str());

                    prop_setter->render(params, result);
                  }
                else
                  {
                    XSSCode code_setter = prop->code_setter();
                    if (code_setter)
                      {
                        //td: !! generalize this
                        result << path << '.' << prop->id() << "__set("; 
                        if (!render_expression(right, ctx, result))
                          return false;
                        result << ")";
                      }
                    else 
                      {
                        result << path << "." << prop->output_id() << " = ";
                        if (!render_expression(right, ctx, result))
                          return false;
                      }
                  }
                
                break;
              }
            case RESOLVE_VARIABLE:
              {
                result << vop.identifier() << " = ";
                if (!render_expression(right, ctx, result))
                  return false;
              }
            default: assert(false);
          }
      }
    else
      {
        if (!render_value(left_value, ctx, result))
          return false;
        
        result << " = ";
        
        if (!render_expression(right, ctx, result))
          return false;
      }

    return true;
  }

bool typed_lang::render_constant(variant& value, XSSContext ctx, std::ostringstream& result)
  {
    str vv = xss_utils::var_to_string(value);
    if (value.is<str>())
      result << '"' << vv << '"';
    else
      result << vv;

    return true;
  }

bool typed_lang::render_read_operation(value_operation& op, XSSContext ctx, std::ostringstream& result)
  {
    if (op.bound())
      {
        switch(op.resolve_id())
          {
            case RESOLVE_PROPERTY:
              {
                XSSProperty prop = op.resolve_value();
                InlineRenderer prop_getter = prop->getter();

                if (prop_getter)
                  {
                    str path = result.str();
                    bool this_property = path.empty();
                    
                    param_list params;
                    params.add("path", path);

                    result.clear();
                    prop_getter->render(params, result);
                    return true;
                  }
                else
                  {
                    XSSCode code_getter = prop->code_getter();
                    if (code_getter)
                      {
                        //td: !! generalize this
                        result << '.' << prop->id() << "__get()";
                        return true;
                      }
                  }
                break;
              }
          }
      }

    result << '.' << op.identifier();
    return true;
  }

bool typed_lang::render_call(value_operation& op, XSSContext ctx, std::ostringstream& result)
  {
    if (op.bound())
      {
        assert(op.resolve_id() == RESOLVE_METHOD);
        XSSMethod mthd = op.resolve_value();
        InlineRenderer renderer = mthd->renderer();
        if (renderer)
          {
            str path = result.str();
            bool this_property = path.empty();
                    
            std::ostringstream args;
            if (!render_arguments(op.args(), ctx, args))
              return false;

            param_list params;
            params.add("path", path);
            params.add("args", args.str());

            result.clear();
            renderer->render(params, result);
          }
        else
          {
            result << '.' << mthd->output_id() << '(';
            if (!render_arguments(op.args(), ctx, result))
              return false;
            result << ')';
          }
      }
    else
      {
        result << '.' << op.identifier() << '(';
        if (!render_arguments(op.args(), ctx, result))
          return false;
        result << ')';
      }
    return true;
  }

bool typed_lang::render_arguments(XSSArguments args, XSSContext ctx, std::ostringstream& result)
  {
    xss_parameters::iterator it = args->begin();
    xss_parameters::iterator nd = args->end();

    bool first = true;
    for(; it != nd; it++)
      {
        if (first)
          first = false;
        else 
          result << ", ";
        
        if (!render_expression(it->value(), ctx, result))
          return false;
      }
    return true;
  }

bool typed_lang::render_index_operation(value_operation& op, XSSContext ctx, std::ostringstream& result)
  {
    if (op.bound())
      {
        XSSType prop_type;
        switch(op.resolve_id())
          {
            case RESOLVE_PROPERTY:
              {
                XSSProperty prop = op.resolve_value();
                prop_type = prop->property_type();
                break;
              }
            case RESOLVE_VARIABLE:
              {
                prop_type = op.resolve_value();
                break;
              }
            default: assert(false); //catch use cases
          }

        assert(prop_type);
        XSSOperator xop = prop_type->get_operator(op_index, op.args());
        if (xop)
          {
            InlineRenderer renderer = xop->renderer();
            if (renderer)
              {
                std::ostringstream args;
                if (!render_arguments(op.args(), ctx, args))
                  return false;

                str path = result.str();
                bool this_property = path.empty();

                param_list pl;
                pl.add("index", args.str());
                pl.add("path", path);

                result.clear();
                renderer->render(pl, result);
                return true;
              }
          }
      }

    //no special case
    result << '.' << op.identifier() << '[';
    if (!render_arguments(op.args(), ctx, result))
      return false;
    result << ']';

    return true;
  }

bool typed_lang::render_instantiation(XSSType type, XSSArguments args, XSSContext ctx, std::ostringstream& result)
  {
    if (!type || !args)
      return false;

    XSSArguments constructor = type->get_constructor(args);
    if (!constructor)
      return false;
    
    result << "new " << type->output_id() << "(";
    if (!render_arguments(constructor, ctx, result))
      return false;
    
    result << ")";
    return true;      
  }

