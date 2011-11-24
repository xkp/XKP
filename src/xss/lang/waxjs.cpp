#include <xss/lang/waxjs.h>

using namespace xkp;

const str SLanguage("language");

const str SCannotForkOnIteration("Cannot perform asynchronic operations inside loops");

#define CHECK_ACTIVE  if (found_) return; curr_++;

struct wax_splitter : code_visitor
  {
    wax_splitter(XSSContext ctx, code& _code):
      ctx_(ctx),
      found_(false),
      curr_(0),
      code_(_code)
      {
      }

    virtual void if_(stmt_if& info)
      {
        CHECK_ACTIVE

        XSSMethod mthd;
        if (forked(info.expr, mthd))
          {
            cut(mthd);
          }
        else
          {
            CodeSplit result;
            if (forked(info.if_code, result))
              {
                cut(result);
                result_->split_on_if = true;
                return;
              }
            
            if (forked(info.else_code, result))
              {
                cut(result);
                result_->split_on_else = true;
              }
          }
      }

    virtual void variable_(stmt_variable& info)
      {
        CHECK_ACTIVE

        XSSMethod mthd;
        if (forked(info.value, mthd))
          {
            cut(mthd);
          }
      }

    virtual void for_(stmt_for& info)
      {
        CHECK_ACTIVE
        
        XSSMethod mthd;
        if (forked(info.cond_expr, mthd) || forked(info.iter_expr, mthd))
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SCannotForkOnIteration);
            error.add("comment", str("dont get cute"));
            xss_throw(error);
          }

        if (forked(info.init_expr, mthd))
          {
            cut(mthd);
          }
      }

    virtual void iterfor_(stmt_iter_for& info)
      {
        CHECK_ACTIVE

        XSSMethod mthd;
        if (forked(info.iter_expr, mthd))
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SCannotForkOnIteration);
            error.add("comment", str("dont get cute"));
            xss_throw(error);
          }
      }

    virtual void while_(stmt_while& info)
      {
        CHECK_ACTIVE

        XSSMethod mthd;
        if (forked(info.expr, mthd))
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SCannotForkOnIteration);
            error.add("comment", str("dont get cute"));
            xss_throw(error);
          }
      }

    virtual void break_()
      {
        CHECK_ACTIVE
      }

    virtual void continue_()
      {
        CHECK_ACTIVE
      }

    virtual void return_(stmt_return& info)
      {
        CHECK_ACTIVE
      }

    virtual void expression_(stmt_expression& info)
      {
        CHECK_ACTIVE

        XSSMethod mthd;
        if (forked(info.expr, mthd))
          {
            cut(mthd);
          }
      }

    virtual void dsl_(dsl& info)
      {
        assert(false); //not yet
      }

    virtual void dispatch(stmt_dispatch& info)
      {
        assert(false); //ditto
      }
    
    CodeSplit get()
      {
        return result_;
      }

    private:
      bool       found_;
      int        curr_;
      XSSContext ctx_;
      CodeSplit  result_;
      code&      code_;

      bool forked(expression& expr, XSSMethod& result)
        {
          //handle some cases, there will be limitations
          operator_type op;
          expr.top_operator(op);
          switch(op)
            {
              case op_dot:
              case op_call:
                break; //path like expression, handled below
              case op_assign:
              case op_plus_equal:
              case op_minus_equal:
              case op_mult_equal:
              case op_div_equal:
              case op_shift_right_equal:
              case op_shift_left_equal:
                {
                  //assigns, lets break it down
                  expression_splitter es(op);
                  expr.visit(&es);
                  return forked(es.right, result);
                }
              default: //limitations
                return false;
            }

          expression_analizer ea; 
          ea.analyze(expr, ctx_);

          if (ea.is_call())
            {
              XSSMethod mthd = ea.method();
              if (mthd)
                {
                  bool asynch = mthd->get<bool>("asynch", false);
                  if (asynch)
                    {
                      result = mthd;
                    }
                  return asynch;
                }
            }

          return false;
        }
      
      
      bool forked(code& cde, CodeSplit& split)
        {
          wax_splitter splitter(ctx_, cde);
          cde.visit(&splitter);

          split = splitter.get(); 
          return split; 
        }

      void cut(XSSMethod mthd)
        {
          assert(!result_);

          found_ = true;
          
          result_ = CodeSplit(new code_split(code_));
          result_->split_idx = curr_;
          result_->method = mthd;
        }

      void cut(CodeSplit split)
        {
          assert(!result_);

          found_ = true;

          result_ = CodeSplit(new code_split(code_));
          result_->split_idx = curr_;
          result_->split_code = split;
        }
  };    

//waxjs_lang
variant waxjs_lang::compile_code(code& cde, param_list_decl& params, XSSContext ctx)
  {
    return reference<waxjs_code_renderer>(new waxjs_code_renderer(cde, params, ctx));
  }

//waxjs_code_renderer
waxjs_code_renderer::waxjs_code_renderer()
  {
  }

waxjs_code_renderer::waxjs_code_renderer(const waxjs_code_renderer& other):
  compiler_(other.compiler_),
  js_code_renderer(other)
  {
  }

waxjs_code_renderer::waxjs_code_renderer(code& cde, param_list_decl& params, XSSContext ctx):
  js_code_renderer(cde, params, ctx)
  {
    compiler_ = variant_cast<XSSCompiler>(ctx->resolve("compiler"), XSSCompiler()); assert(compiler_);
  }

str waxjs_code_renderer::render()
  {
    wax_splitter splitter(ctx_, code_);
    code_.visit(&splitter);

    CodeSplit fork = splitter.get();
    if (!fork)
        return render_code(code_);
    
    str result = render_split(fork, CodeSplit());
    return result;
  }

str waxjs_code_renderer::render_split(CodeSplit fork, CodeSplit parent)
  {
    if (parent)
      {
        //update the split call list
        fork->add.insert(fork->add.begin(), parent->add.begin(), parent->add.end());
      }

    std::ostringstream result;

    int split_idx = fork->split_idx - 1;

    //render the code before the split
    code pre_split;
    fork->target.range(0, split_idx, pre_split);
    result << render_code(pre_split);

    //render the after code as a function, will be called on splits
    std::ostringstream code_after;
    str                split_name;

    code after_wards;
    fork->target.cut(fork->split_idx, after_wards);
    
    bool has_code_after = !after_wards.empty();    

    if (has_code_after)
      {
        split_name = compiler_->genid("callback");
        code_after << "function " << split_name << "() \n{\n";
        code_after << split_and_render(after_wards, fork);
        code_after << "\n}\n";

        split_name += "();\n";
        fork->add.insert(fork->add.begin(), split_name);
      }

    //then split
    variant& st = fork->target.get_stament(split_idx);
    if (st.is<stmt_if>())
      {
        result << code_after.str() << split_if(fork);
      }
    else if (st.is<stmt_variable>())
      {
        result << split_variable(fork, code_after.str());
      }
    else if (st.is<stmt_expression>())
      {
        result << code_after.str() << split_expression(fork);
      }
    else
      {
        assert(false);
      }
      return result.str();
  }

str waxjs_code_renderer::split_if(CodeSplit fork)
  {
    std::ostringstream result;
    
    stmt_if stmnt = fork->target.get_stament(fork->split_idx - 1);
    if (fork->split_on_if)
      {
        result << "\nif (" << render_expression(stmnt.expr, ctx_) << ")";
        result << "\n{";

        result << render_split(fork->split_code, fork);

        if (!stmnt.else_code.empty())
          {
            result << "\nelse\n{\n";
            result << split_and_render(stmnt.else_code, fork);
            result << "\n}";
          }
      }
    else if (fork->split_on_else)
      {
        result << "\nif (" << render_expression(stmnt.expr, ctx_) << ")";
        result << "\n{";

        render_code(stmnt.if_code); 
        
        result << after_code(fork) << "\n}";
        result << "else\n{\n";
        result << render_split(fork->split_code, fork);
      }
    else
      {
        //last use case, the split happens on the if expression, 
        //the key to the algo here is that code gets split only once
        result << split_method(fork->method);
        result << "\n{\n";
        result << "if (return_value)";
        result << "\n{\n";

        result << split_and_render(stmnt.if_code, fork);

        result << "\n}";

        if (!stmnt.else_code.empty())
          {
            result << "else\n{\n";
            result << split_and_render(stmnt.else_code, fork);
            result << "\n}";
          }
        else
            result << after_code(fork);

        result << "\n});\n";
      }

    return result.str();
  }

str waxjs_code_renderer::after_code(CodeSplit fork)
  {
    std::ostringstream result;

    std::vector<str>::iterator it = fork->add.begin();
    std::vector<str>::iterator nd = fork->add.end();
    for(; it != nd; it++)
      {
        result <<  "\n" << *it;
      }

    return result.str();
  }

str waxjs_code_renderer::split_variable(CodeSplit fork, const str& code_after)
  {
    stmt_variable stmnt = fork->target.get_stament(fork->split_idx - 1);
    
    //code after_var;
    //fork->target.cut(fork->split_idx, after_var);

    std::ostringstream result;
    result << "var " << stmnt.id << ";\n";
    result << code_after;
    result << split_method(fork->method);
    result << "\n{\n";
    result << stmnt.id << " = return_value;";
    
    result << after_code(fork);

    //result << split_and_render(after_var, fork->add);
    result << "\n});\n";

    return result.str();
  }

str waxjs_code_renderer::split_expression(CodeSplit fork)
  {
    stmt_expression stmnt = fork->target.get_stament(fork->split_idx - 1);

    operator_type op;
    bool assign = false;
    if (stmnt.expr.top_operator(op))
      {
        switch(op)
          {
						case op_assign:
            case op_plus_equal:
            case op_minus_equal:
            case op_mult_equal:
            case op_div_equal:
            case op_shift_right_equal:
            case op_shift_left_equal:
							{
                assign = true;
                break;
              }
            case op_call:
              break;
            default:
              assert(false);
          }
      }

    //code after_expr;
    //fork->target.cut(fork->split_idx, after_expr);

    std::ostringstream result;
    result << split_method(fork->method);
    result << "\n{\n";

    if (assign)
      {
        expression_splitter es(op);
        stmnt.expr.visit(&es);
        result << render_expression(es.left, ctx_) << " " << lang_utils::operator_string(op) << " return_value;";

        result << after_code(fork);
        //result << split_and_render(after_expr, fork->add);
      }
    else
      {
        result << after_code(fork);
        //result << split_and_render(after_expr, fork->add); //td: ! remove when working
      }
    
    result << "\n});\n";

    return result.str();
  }

str waxjs_code_renderer::split_and_render(code& c, CodeSplit parent)
  {
    wax_splitter splitter(ctx_, c);
    c.visit(&splitter);

    CodeSplit fork = splitter.get();
    if (fork)
      return render_split(fork, parent);

    //no split, just render the code and call the splits
    std::ostringstream result;
    result << render_code(c); 

    if (parent)
      result << after_code(parent); 
    
    return result.str();
  }

str waxjs_code_renderer::split_method(XSSMethod method)
  {
    std::ostringstream result;
    result << method->output_id() << "(function(return_value)";

    return result.str();
  }

//method whatever()
  //{
  //  var x = 3;
  //  if (delayed())
  //  {
  //    ....
  //  }

  //  var y = delayed();

  //  if (y > 3)
  //  {
  //    x = delayed();
  //  }

  //  return x + y;
  //}

  //function whatever(return_fn)
  //{
  //  var x = 3;
  //  delayed(function(result){
  //    if (result)
  //    {
  //      ...
  //    }

  //    delayed(function(result){
  //      var y = result;

  //      function after_if()
  //      {
  //        return_fn(x + y); 
  //      }

  //      if (y > 3)
  //      {
  //        delayed(function(result){
  //          x = result;
  //          after_if();
  //        });
  //        return;
  //      }

  //      after_if();
  //    });
  //  });  
  //}
