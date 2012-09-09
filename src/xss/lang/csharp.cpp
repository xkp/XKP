
#include <xss/lang/csharp.h>

using namespace xkp;

XSSContext cs_lang::create_context()
  {
    XSSContext result(new xss_context(XSSContext()));

    result->add_type(XSSType(new xss_type("int",    type_schema<int>())), str());
    result->add_type(XSSType(new xss_type("float",  type_schema<float>())), str());
    result->add_type(XSSType(new xss_type("double", type_schema<double>())), str());
    result->add_type(XSSType(new xss_type("bool",   type_schema<bool>())), str());
    result->add_type(XSSType(new xss_type("string", type_schema<str>())), str());

    XSSType var_type = result->add_type(XSSType(new xss_type("var", null)), str());
    return result;
  }
