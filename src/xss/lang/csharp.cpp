
#include <xss/lang/csharp.h>

using namespace xkp;

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
    else 
      {
        result << ")";
      }

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
