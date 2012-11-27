
#ifndef XSS_TYPED_LANG_HH
#define XSS_TYPED_LANG_HH

#include <xss/xss_context.h>

namespace xkp{

//td: !!! base language
struct typed_lang : ILanguage
  {
    virtual XSSContext create_context();
    virtual void       init_compile_context(XSSContext ctx);
    virtual bool       match_types(XSSType left, XSSType right, TYPE_MATCH& result);

    //generalities
    virtual bool render_code(XSSCode code, XSSContext ctx, std::ostringstream& result);
    virtual bool render_expression(XSSExpression expr, XSSContext ctx, std::ostringstream& result); 
    virtual bool render_value(XSSValue value, XSSContext ctx, std::ostringstream& result);
    virtual bool render_type_name(XSSType type, XSSContext ctx, std::ostringstream& result);
    virtual bool render_signature(XSSSignature sig, XSSContext ctx, std::ostringstream& result);
    
    //code rendering
    virtual bool render_if(IStatementIf* info, XSSContext ctx, std::ostringstream& result);
    virtual bool render_variable(IStatementVar* info, XSSContext ctx, std::ostringstream& result);
    virtual bool render_for(IStatementFor* info, XSSContext ctx, std::ostringstream& result);
    virtual bool render_while(IStatementWhile* info, XSSContext ctx, std::ostringstream& result);
    virtual bool render_switch(IStatementSwitch* info, XSSContext ctx, std::ostringstream& result);
    virtual bool render_try(IStatementTry* info, XSSContext ctx, std::ostringstream& result);
    virtual bool render_break(XSSContext ctx, std::ostringstream& result);
    virtual bool render_continue(XSSContext ctx, std::ostringstream& result);
    virtual bool render_return(IStatementExpression* info, XSSContext ctx, std::ostringstream& result);
    virtual bool render_expr_statement(IStatementExpression* info, XSSContext ctx, std::ostringstream& result);
    virtual bool render_throw(IStatementExpression* info, XSSContext ctx, std::ostringstream& result);

    //expression rendering
    virtual bool render_assignment(XSSExpression expr, XSSValue left_value, XSSExpression right, XSSContext ctx, std::ostringstream& result, const str& path = str());
    virtual bool render_array_assignment(XSSExpression expr, XSSArguments index, XSSContext ctx, std::ostringstream& result); //handled as a separate case, for complications
    virtual bool render_operator(XSSExpression expr, XSSContext ctx, std::ostringstream& result);
    virtual bool render_constant(variant& value, XSSContext ctx, std::ostringstream& result); 
    virtual bool render_read_operation(value_operation& op, XSSContext ctx, std::ostringstream& result);  
    virtual bool render_call(value_operation& op, XSSContext ctx, std::ostringstream& result);  
    virtual bool render_arguments(XSSArguments args, XSSContext ctx, std::ostringstream& result);  
    virtual bool render_index_operation(value_operation& op, XSSContext ctx, std::ostringstream& result);  
    virtual bool render_instantiation(XSSType type, XSSArguments args, XSSContext ctx, std::ostringstream& result);  

    //property rendering
    virtual bool render_get(XSSProperty prop, const str& path, std::ostringstream& result);  
    virtual bool render_set(XSSProperty prop, const str& path, const str& value, std::ostringstream& result);  
  };

}

#endif