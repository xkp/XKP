#include <xss/lang/waxjs.h>

using namespace xkp;

struct code_split;

typedef reference<code_split> CodeSplit;

struct code_split
  {
    code      target;
    int       split_idx;
    CodeSplit split_code;

    bool      split_on_if;
    bool      split_on_else;
  };

#define CHECK_ACTIVE  if (found_) return; curr_++;

struct wax_splitter
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


      }

    virtual void for_(stmt_for& info)
      {
      }

    virtual void iterfor_(stmt_iter_for& info)
      {
      }

    virtual void while_(stmt_while& info)
      {
      }

    virtual void break_()
      {
      }

    virtual void continue_()
      {
      }

    virtual void return_(stmt_return& info)
      {
      }

    virtual void expression_(stmt_expression& info)
      {
      }

    virtual void dsl_(dsl& info)
      {
      }

    virtual void dispatch(stmt_dispatch& info)
      {
      }
    
    private:
      bool found_;
      int  curr_;

      bool      forked(expression& expr);  
      bool      forked(code& cde, CodeSplit& split);  
      CodeSplit cut();
      CodeSplit cut(CodeSplit split);
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
    return str();
  }
