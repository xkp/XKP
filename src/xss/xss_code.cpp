
#include <xss/xss_code.h>
#include <xss/xss_expression.h>

using namespace xkp;

const str SExpectingIterable("foreach expects an iterable");
const str SExpectingConstantCase("case expression must be constant in this context");
const str SUnknownType("cannot resolve type");
const str STypeMismatch("type mismatch");

//statement_if
void statement_if::bind(XSSContext ctx)
  {
      expr_->bind(ctx);
      if (if_code_)
		if_code_->bind(ctx);
      if (else_code_)
        else_code_->bind(ctx);
  }

//statement_variable
void statement_variable::bind(XSSContext ctx)
  {
    type_ = ctx->get_type(type_name_);
    if (!type_)
	    {
	      param_list pl;
	      pl.add("type", type_name_);
	      ctx->error(SUnknownType, &pl, begin_, end_);
      }
      
    if (value_)
      {
        value_->bind(ctx);
        XSSType vtype = value_->type();

        if (type_ && vtype)
          {
            TYPE_MATCH tm;
            if (ctx->match_types(type_, vtype, tm))
              {
                switch(tm)
                  {
                    case VARIANT:
                      {
                        type_ = vtype;
                        break;
                      }
                    case TYPECAST:
                      {
                        needs_cast_ = true;
                        break;
                      }
                  }
              }
            else
              {
	              param_list pl;
	              pl.add("type1", type_->id());
	              pl.add("type2", vtype->id());
	              ctx->error(STypeMismatch, &pl, begin_, end_);
              }
          }
      }

    ctx->register_symbol(RESOLVE_VARIABLE, id_, type_);
  }

//statement_for
void statement_for::bind(XSSContext ctx)
  {
    if (id_.empty())
      {
        init_expr_->bind(ctx);
      }
    else
      {
        type_ = ctx->get_type(type_name_);
        if (!type_)
	        {
	          param_list pl;
	          pl.add("type", type_name_);
	          ctx->error(SUnknownType, &pl, begin_, end_);
          }

        init_value_->bind(ctx);
        XSSType vtype = init_value_->type();

        if (type_ && vtype)
          {
            TYPE_MATCH tm;
            if (ctx->match_types(type_, vtype, tm))
              {
                switch(tm)
                  {
                    case VARIANT:
                      {
                        type_ = vtype;
                        break;
                      }
                    case TYPECAST:
                      {
                        needs_cast_ = true;
                        break;
                      }
                  }
              }
            else
              {
	              param_list pl;
	              pl.add("type1", type_->id());
	              pl.add("type2", vtype->id());
	              ctx->error(STypeMismatch, &pl, begin_, end_);
              }
          }
        
        for_code_->context()->register_symbol(RESOLVE_VARIABLE, id_, type_);
      }

    if (cond_expr_)
      cond_expr_->bind(ctx);

    if (iter_expr_)
      iter_expr_->bind(ctx);

    for_code_->bind(ctx);
  }

//statement_foreach
void statement_foreach::bind(XSSContext ctx)
  {
    type_ = ctx->get_type(type_name_);
    if (!type_)
	    {
	      param_list pl;
	      pl.add("type", type_name_);
	      ctx->error(SUnknownType, &pl, begin_, end_);
      }

    iter_expr_->bind(ctx);

    XSSType iter_type = iter_expr_->type();
    if (iter_type && !iter_type->is_array())
        ctx->error(SExpectingIterable, null, begin_, end_);

    XSSType vtype = iter_type? iter_type->array_type() : XSSType();
    if (type_ && vtype)
      {
        TYPE_MATCH tm;
        if (ctx->match_types(type_, vtype, tm))
          {
            switch(tm)
              {
                case VARIANT:
                  {
                    type_ = vtype;
                    break;
                  }
                case TYPECAST:
                  {
                    needs_cast_ = true;
                    break;
                  }
              }
          }
        else
          {
	          param_list pl;
	          pl.add("type1", type_->id());
	          pl.add("type2", vtype->id());
	          ctx->error(STypeMismatch, &pl, begin_, end_);
          }
      }

    for_code_->context()->register_symbol(RESOLVE_VARIABLE, id_, type_);
    for_code_->bind(ctx);
  }

//statement_while
void statement_while::bind(XSSContext ctx)
  {
    expr_->bind(ctx);
    code_->bind(ctx);
  }

//statement_switch
void statement_switch::bind(XSSContext ctx)
  {
    default_code_->bind(ctx);
    switch_sections::iterator it = sections_.begin();
    switch_sections::iterator nd = sections_.end();

    if (expr_)
      expr_->bind(ctx);

    for(; it != nd; it++)
      {
        std::vector<XSSExpression>::iterator eit = it->cases.begin();
        std::vector<XSSExpression>::iterator end = it->cases.end();

        for(; eit != end; eit++)
          {
            XSSExpression case_expr = *eit;
            if (expr_)
              {
                if (!case_expr->is_constant())
                  ctx->error(SExpectingConstantCase, null, case_expr->begin(), case_expr->end());
              }
            else 
              case_expr->bind(ctx);
          }

        it->case_code->bind(ctx);
      }
  }

//statement_try
void statement_try::bind(XSSContext ctx)
  {
    try_code_->bind(ctx);
    if (finally_code_)
      finally_code_->bind(ctx);
    
    catch_sections::iterator it = sections_.begin();
    catch_sections::iterator nd = sections_.end();

    for(; it != nd; it++)
      {
        XSSType ct = ctx->assure_type(it->type);
        it->catch_code->context()->register_symbol(RESOLVE_VARIABLE, it->id, ct);
        it->catch_code->bind(ctx);
      }
  }

//expr_statement
void expr_statement::bind(XSSContext ctx)
  {
    //td: !!! notify return stuff
    expr_->bind(ctx);
  }

//xss_loop_statement
void xss_loop_statement::bind(XSSContext ctx)
  {
    //td: !!! check loop, notify context
  }

//utils
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
		XSSExpression value;
		if (!info.value.empty())
			value = xss_expression_utils::compile_expression(info.value);
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
        result_->add(XSSStatement(new xss_loop_statement(STATEMENT_BREAK, info.begin, info.end)));
      }

    virtual void continue_(stmt_continue& info)
      {
        result_->add(XSSStatement(new xss_loop_statement(STATEMENT_CONTINUE, info.begin, info.end)));
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
          result->set_extents(cde.begin, cde.end);

          if (callback_)
            callback_->notify(result->context());

          code_builder cb(result, callback_);
          cde.visit(&cb);
          return result;
        }
  };    


//xss_code_utils
XSSCode xss_code_utils::compile_code(code& cde, IContextCallback* callback)
  {
    XSSCode result(new xss_code);

    XSSContext ctx = result->context();
    ctx->set_extents(cde.begin, cde.end); 

    if (callback)
      callback->notify(ctx);

	code_builder cb(result, callback);
    cde.visit(&cb);

    return result;
  }

XSSSignature xss_code_utils::compile_arg_list(param_list_decl& args)
  {
    XSSSignature result(new xss_signature);

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
