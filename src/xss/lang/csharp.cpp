
#include <xss/lang/csharp.h>

using namespace xkp;

XSSContext cs_lang::create_context()
  {
	  XSSContext result = typed_lang::create_context();
	  XSSType var_type = result->get_type("var");
	  var_type->set_output_id("object");
    
	  //td: generalize
    XSSType exception_type(new xss_type("Exception"));
    result->add_type(exception_type, str());

    result->register_symbol(RESOLVE_CONST, "#default_exception", exception_type);

	  return result;
  }

bool cs_lang::render_type_name(XSSType type, XSSContext ctx, std::ostringstream& result)
  {
    if (!type)
	    {
		    result << "void";
		    return true;
	    }

	  if (type->is_array())
      {
        result << "List<";
        XSSType at = type->array_type();
        if (!render_type_name(at, ctx, result))
          return false;

        result << ">";
      }
    else if (type->is_variant())
      {
        result << "object";
      }
    else
      return typed_lang::render_type_name(type, ctx, result);
    
    return true;
  }

bool cs_lang::render_foreach(IStatementForEach* info, XSSContext ctx, std::ostringstream& result)
  {
    result << '\n' << "foreach(";
    result << info->type()->output_id() << " " << info->id() << " in ";
    if (!render_expression(info->iter_expr(), ctx, result))
      return false;

    result << ")";
    result << '\n' << "{";

    if (!render_code(info->for_code(), ctx, result))
      return false;

    result << '\n' << "}";
    return true;
  }

bool cs_lang::render_object(value_operation& op, XSSContext ctx, std::ostringstream& result)
  {
    assert(op.bound());
    assert(op.resolve_id() == RESOLVE_CONST);
    XSSType type = variant_cast<XSSType>(op.resolve_value(), XSSType()); assert(type);
    return render_instantiation(type, op.args(), ctx, result);
  }

bool cs_lang::render_array(value_operation& op, XSSContext ctx, std::ostringstream& result)
  {
    assert(op.bound());
    assert(op.resolve_id() == RESOLVE_CONST);
    XSSType type       = variant_cast<XSSType>(op.resolve_value(), XSSType()); assert(type);
    XSSType array_type = type->array_type(); assert(array_type);
    
    result << "new List<" << array_type->output_id() << ">(";
    if (op.args()->size() > 0)
      {
        result << "new " << array_type->output_id() << "[]";
        xss_parameters::iterator it = op.args()->begin();
        xss_parameters::iterator nd = op.args()->end();

        result << "{";
        
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

        result << "}";
      }

    result << ")";
    return true;
  }

bool cs_lang::render_pre_statement(XSSStatement info, XSSContext ctx, std::ostringstream& result)
  {
    str src_map = variant_cast<str>(ctx->resolve("#src_map", RESOLVE_CONST), str());
    if (!src_map.empty())
      {
        file_position pos = info->begin();
        result << '\n' << "#line " << pos.line << " \"" << src_map << '"';
      }

    return true;
  }

bool cs_lang::render_operator(XSSExpression expr, XSSContext ctx, std::ostringstream& result)
  {
    return typed_lang::render_operator(expr, ctx, result);
  }
