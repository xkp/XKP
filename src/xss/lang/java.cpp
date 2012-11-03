
#include <xss/lang/java.h>
#include <xss/xss_expression.h>

//#include <xss/xss_error.h>

using namespace xkp;

const str SJavaLanguage("java-language");

const str SUnknownType("Cannot resolve type");
const str SWriteOnlyProperty("Trying to acces write only property");
const str SReadOnlyProperty("Trying to acces read only property");

XSSContext java_lang::create_context()
  {
	  XSSContext result = typed_lang::create_context();

    XSSType type;

    // translate types name
    if (type = result->get_type("int"))
      type->set_output_id("Integer");

    if (type = result->get_type("float"))
      type->set_output_id("Float");

    if (type = result->get_type("double"))
      type->set_output_id("Double");

    if (type = result->get_type("bool"))
      type->set_output_id("Boolean");

    if (type = result->get_type("string"))
      type->set_output_id("String");

    if (type = result->get_type("array"))
      {
        type->set_output_id("ArrayList");

        XSSProperty sz_prop = XSSProperty(new xss_property());
        sz_prop->set_id("size");
        sz_prop->set_type(result->get_type("int"));

        param_list iparams;
        InlineRenderer getter(new inline_renderer());
        getter->compile("size()", false, iparams);

        sz_prop->getter(getter);

        type->register_property("size", sz_prop);
      }

    //td: remember: Byte(byte), Character(char), Short(short), Long(long)

	  return result;
  }

bool java_lang::render_type_name(XSSType type, XSSContext ctx, std::ostringstream& result)
  {
    if (!type)
	    {
		    result << "void";
		    return true;
	    }

	  if (type->is_array())
      {
        result << "ArrayList<";
        XSSType at = type->array_type();
        if (!render_type_name(at, ctx, result))
          return false;

        result << ">";
      }
    else if (type->is_variant())
      {
        result << "Object";
      }
    else
      return typed_lang::render_type_name(type, ctx, result);
    
    return true;
  }

bool java_lang::render_foreach(IStatementForEach* info, XSSContext ctx, std::ostringstream& result)
  {
    result << '\n' << "for(";
    result << info->type()->output_id() << " " << info->id() << " : ";
    if (!render_expression(info->iter_expr(), ctx, result))
      return false;

    result << ")";
    result << '\n' << "{";

    if (!render_code(info->for_code(), ctx, result))
      return false;

    result << '\n' << "}";
    return true;
  }

bool java_lang::render_object(value_operation& op, XSSContext ctx, std::ostringstream& result)
  {
    assert(op.bound());
    assert(op.resolve_id() == RESOLVE_CONST);
    XSSType type = variant_cast<XSSType>(op.resolve_value(), XSSType()); assert(type);
    return render_instantiation(type, op.args(), ctx, result);
  }

bool java_lang::render_array(value_operation& op, XSSContext ctx, std::ostringstream& result)
  {
    assert(op.bound());
    assert(op.resolve_id() == RESOLVE_CONST);
    
    result << "new ArrayList(Arrays.asList(";
    if (op.args()->size() > 0)
      {
        result << "new Object[] ";
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

    result << "))";
    return true;
  }

bool java_lang::render_array_assignment(XSSExpression expr, XSSArguments index, XSSContext ctx, std::ostringstream& result)
  {
    operator_type op = expr->op();
    if (index)
      {
        //here we render the "get" expression which will include the brackets,
		    //this will break if the target language does not support the operator directly
        if (!render_expression(expr->left(), ctx, result))
          return false;

		    //result << '[';
        //
        //if (!render_arguments(index, ctx, result))
        //  return false;

        //result << "] ";
		
		    result << operator_utils::to_string(op) << " ";
        if (!render_expression(expr->right(), ctx, result))
          return false;
      }
    else
      {
        //td: !!! move to csharp
        assert(op == op_plus_equal || op == op_minus_equal);
        str opstr = "add";
        if (expr->op() == op_minus_equal)
          opstr = "remove";

        if (!render_expression(expr->left(), ctx, result))
          return false;

        result << "." << opstr << "(";

        if (!render_expression(expr->right(), ctx, result))
          return false;

        result << ")";
      }

    return true;
  }

bool java_lang::render_pre_statement(XSSStatement info, XSSContext ctx, std::ostringstream& result)
  {
    return true;
  }

bool java_lang::render_operator(XSSExpression expr, XSSContext ctx, std::ostringstream& result)
  {
    return typed_lang::render_operator(expr, ctx, result);
  }

bool java_lang::render_constant(variant& value, XSSContext ctx, std::ostringstream& result)
  {
    if (value.is<float>() || value.is<double>())
      {
        float f = value;

        result.setf(std::ios::fixed, std::ios::floatfield);
        result.precision(6);

        result << f;

        if (value.is<float>()) result << "f";

        return true;
      }

    return typed_lang::render_constant(value, ctx, result);
  }
