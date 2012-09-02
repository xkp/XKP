
#include <xss/xss_code.h>
#include <xss/xss_expression.h>

using namespace xkp;

struct code_builder : code_visitor
  {
    code_builder(XSSCode result, IContextCallback* callback):
      result_(result), 
      callback_(callback)
      {
      }

    virtual void if_(stmt_if& info)
      {
        XSSExpression expr      = xss_expression_utils::compile_expression(info.expr);
        XSSCode       if_code   = info.if_code.empty()?    XSSCode() : compile_code(info.if_code);
        XSSCode       else_code = info.else_code.empty()?  XSSCode() : compile_code(info.else_code);

        result_->add(XSSStatement(new statement_if(expr, if_code, else_code, info.begin, info.end)));
      }

    virtual void variable_(stmt_variable& info)
      {
        XSSExpression value = xss_expression_utils::compile_expression(info.value);
        result_->add(XSSStatement(new statement_variable(info.id, info.type, value, info.begin, info.end)));
      }

    virtual void for_(stmt_for& info)
      {
        XSSExpression init_value = xss_expression_utils::compile_expression(info.init_variable.value);
        XSSExpression init_expr  = xss_expression_utils::compile_expression(info.init_expr);
        XSSExpression cond_expr  = xss_expression_utils::compile_expression(info.cond_expr);
        XSSExpression iter_expr  = xss_expression_utils::compile_expression(info.iter_expr);
        XSSCode       for_code   = compile_code(info.for_code);

        result_->add(XSSStatement(new statement_for(info.init_variable.id, info.init_variable.type, init_value,  
                                                    init_expr, cond_expr, iter_expr,
                                                    for_code, info.begin, info.end)));

      }

    virtual void iterfor_(stmt_iter_for& info)
      {
        XSSExpression iter_expr  = xss_expression_utils::compile_expression(info.iter_expr);
        XSSCode       for_code   = compile_code(info.for_code);
        result_->add(XSSStatement(new statement_foreach(info.id, info.type.name, iter_expr, for_code, info.begin, info.end)));
      }

    virtual void while_(stmt_while& info)
      {
        XSSExpression expr  = xss_expression_utils::compile_expression(info.expr);
        XSSCode       code  = compile_code(info.while_code);
        result_->add(XSSStatement(new statement_while(expr, code, info.begin, info.end)));
      }

    virtual void break_(stmt_break& info)
      {
        result_->add(XSSStatement(new xss_statement(STATEMENT_BREAK, info.begin, info.end)));
      }

    virtual void continue_(stmt_continue& info)
      {
        result_->add(XSSStatement(new xss_statement(STATEMENT_CONTINUE, info.begin, info.end)));
      }

    virtual void return_(stmt_return& info)
      {
        XSSExpression expr  = xss_expression_utils::compile_expression(info.expr);
        result_->add(XSSStatement(new expr_statement(STATEMENT_RETURN, expr, info.begin, info.end)));
      }

    virtual void expression_(stmt_expression& info)
      {
        XSSExpression expr  = xss_expression_utils::compile_expression(info.expr);
        result_->add(XSSStatement(new expr_statement(STATEMENT_EXPRESSION, expr, info.begin, info.end)));
      }

    virtual void dsl_(dsl& info)
      {
        assert(false); //td: !!!
      }

    virtual void dispatch(stmt_dispatch& info)
      {
        assert(false); //td: !!!
      }

    virtual void switch_(stmt_switch& info)
      {
        XSSExpression expr         = xss_expression_utils::compile_expression(info.expr);
        XSSCode       default_code = compile_code(info.default_code);

        statement_switch* result = new statement_switch(expr, default_code, info.begin, info.end);

        std::vector<switch_section>::iterator it = info.sections.begin();
        std::vector<switch_section>::iterator nd = info.sections.end();

        for(; it != nd; it++)
          {
            XSSCode case_code = compile_code(it->case_code);
            statement_switch_section& section = result->create_section(case_code);

            std::vector<expression>::iterator eit = it->cases.begin();
            std::vector<expression>::iterator end = it->cases.end();

            for(; eit != end; eit++)
              section.cases.push_back(xss_expression_utils::compile_expression(*eit));
          }
          
        result_->add(XSSStatement(result));
      }

    virtual void try_(stmt_try& info)
      {
        XSSCode try_code     = compile_code(info.try_code);
        XSSCode finally_code = compile_code(info.finally_code);
        
        statement_try* result = new statement_try(try_code, finally_code, info.begin, info.end);

        std::vector<catch_>::iterator it = info.catches.begin();
        std::vector<catch_>::iterator nd = info.catches.end();

        for(; it != nd; it++)
          {
            XSSCode catch_code = compile_code(it->catch_code);
            result->create_section(it->id, it->type.name, catch_code); //td: !!! generics
          }

        result_->add(XSSStatement(result));
      }

    virtual void throw_(stmt_throw& info)
      {
        XSSExpression expr  = xss_expression_utils::compile_expression(info.expr);
        result_->add(XSSStatement(new expr_statement(STATEMENT_THROW, expr, info.begin, info.end)));
      }

    private:
      XSSCode           result_;
      IContextCallback* callback_;

      XSSCode compile_code(code& cde)
        {
          XSSCode result(new xss_code);

          code_builder cb(result, callback_);
          cde.visit(&cb);
          return result;
        }
  };    


//xss_code_utils
XSSCode xss_code_utils::compile_code(code& cde, IContextCallback* callback)
  {
    XSSCode result(new xss_code);

    code_builder cb(result, callback);
    cde.visit(&cb);
    return result;
  }

XSSSignature xss_code_utils::compile_arg_list(param_list_decl& args)
  {
    XSSSignature result;

    param_list_decl::iterator it = args.begin();
    param_list_decl::iterator nd = args.end();

    for(; it != nd; it++)
      {
        XSSExpression default_value;
        if (!it->default_value.empty())
          default_value = xss_expression_utils::compile_expression(it->default_value);
        
        result->add_argument(it->name, it->type, default_value);
      }

    return result;
  }
