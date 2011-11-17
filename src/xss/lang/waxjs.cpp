#include <xss/lang/waxjs.h>

using namespace xkp;

const str SLanguage("language");

const str SCannotForkOnIteration("Cannot perform asynchronic operations inside loops");

#define CHECK_ACTIVE  if (found_) return; curr_++;

struct wax_splitter : code_visitor
  {
    virtual void if_(stmt_if& info)
      {
        CHECK_ACTIVE

        if (forked(info.expr))
          {
            cut();
          }
        else
          {
            CodeSplit result;
            if (forked(info.if_code, result))
              {
                CodeSplit my_split = cut(result);
                my_split->split_on_if = true;
                return;
              }
            
            if (forked(info.else_code, result))
              {
                CodeSplit my_split = cut(result);
                my_split->split_on_else = true;
              }
          }
      }

    virtual void variable_(stmt_variable& info)
      {
        CHECK_ACTIVE
        if (forked(info.value))
          {
            cut();
          }
      }

    virtual void for_(stmt_for& info)
      {
        CHECK_ACTIVE
        if (forked(info.cond_expr) || forked(info.iter_expr))
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SCannotForkOnIteration);
            error.add("comment", str("dont get cute"));
            xss_throw(error);
          }

        if (forked(info.init_expr))
          {
            cut();
          }
      }

    virtual void iterfor_(stmt_iter_for& info)
      {
        CHECK_ACTIVE

        if (forked(info.iter_expr))
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

        if (forked(info.expr))
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
        if (forked(info.expr))
          {
            cut();
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

      bool forked(expression& expr)
        {
          expression_analizer ea; 
          ea.analyze(expr, ctx_);

          if (ea.is_call())
            {
              XSSMethod mthd = ea.method();
              if (mthd)
                return mthd->get<bool>("asynch", false);
            }

          return false;
        }
      
      
      bool forked(code& cde, CodeSplit& split)
        {
          wax_splitter splitter;
          cde.visit(&splitter);

          split = splitter.get(); 
          return split; 
        }

      CodeSplit cut()
        {
          return CodeSplit();
        }

      CodeSplit cut(CodeSplit split)
        {
          return CodeSplit();
        }
  };    

//waxjs_code_renderer
waxjs_code_renderer::waxjs_code_renderer()
  {
  }

waxjs_code_renderer::waxjs_code_renderer(const waxjs_code_renderer& other):
  base_code_renderer(other)
  {
  }

waxjs_code_renderer::waxjs_code_renderer(code& cde, param_list_decl& params, XSSContext ctx):
  base_code_renderer(cde, params, ctx)
  {
  }

str waxjs_code_renderer::render()
  {
    wax_splitter splitter;
    code_.visit(&splitter);

    CodeSplit fork = splitter.get();
    if (!fork)
      {
        assert(false); //td: generate normally
      }

    return render_split(fork);
  }

str waxjs_code_renderer::render_split(CodeSplit fork)
  {
    variant& st = fork->target.get_stament(fork->split_idx);
    if (st.is<stmt_if>())
      {
      }
    else if (st.is<stmt_variable>())
      {
      }
    else if (st.is<stmt_expression>())
      {
      }
    else
      {
        assert(false);
      }
      return str();
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
