
#include "xss/lang/debug.h"

using namespace xkp;

const char* operator_str[] =
  {
    "++",   //op_inc,
    "--",   //op_dec,
    "&",    //op_ref,
    "+",    //op_unary_plus,
    "-",    //op_unary_minus,
    "!",    //op_not,
    "*",    //op_mult,
    "/",    //op_divide,
    "%",    //op_mod,
    "as",   //op_typecast,
    "is",   //op_typecheck,
    "has",  //op_namecheck,
    "+",    //op_plus,
    "-",    //op_minus,
    ">>",   //op_shift_right,
    "<<",   //op_shift_left,
    ">>=",  //op_shift_right_equal,
    "<<=",  //op_shift_left_equal,
    "==",   //op_equal,
    "!=",   //op_notequal,
    ">",    //op_gt,
    "<",    //op_lt,
    ">=",   //op_ge,
    "<=",   //op_le,
    "&&",   //op_and,
    "||",   //op_or,
    "=",    //op_assign,
    "+=",   //op_plus_equal,
    "-=",   //op_minus_equal,
    "*=",   //op_mult_equal,
    "/=",   //op_div_equal,
    ".",    //op_dot,
    ".",    //op_dot_call
    "[]",   //op_index,
    "",     //op_call,
    "",     //op_func_call
    "",     //op_array,
    "",     //op_parameter
  };

struct expr_renderer : expression_visitor 
  {
    expr_renderer(const str& indent):
      indent_(indent)
      {
      }

    virtual void push(variant operand, bool top)
      {
        str value;
        if (operand.is<expression_identifier>())
          {
            expression_identifier ei = operand;
            value = ei.value;
          }
        else
          {
            value = variant_cast<str>(operand, str("cannot convert to string"));
          }

        result_ += indent_ + value + '\n';
      }

    virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top)
      {
        result_ += indent_ + operator_str[op] + '\n';
      }

    str get()
      {
        return result_;
      }

    private:
      str indent_;
      str result_;
  };

struct code_renderer : ICodeRenderer,
                       code_visitor
  {
    code_renderer(code& cde, XSSContext ctx, int indent = 0):
      code_(cde),
      ctx_(ctx),
      indent_(indent)
      {
      }

    //ICodeRenderer
    virtual str render()
      {
        result_ = str();
        code_.visit(this);

        return result_;
      }

    virtual XSSType type()
      {
        return XSSType();
      }

    //code_visitor
    virtual void if_(stmt_if& info)
      {
        result_ += indent() + "IF\n";
        result_ += indent() + "CONDITION:\n";

        render_expression(info.expr);

        result_ += indent() + "CODE:\n";
        render_code(info.if_code);

        if (!info.else_code.empty())
          {
            result_ += indent() + "ELSE\n";
            render_code(info.else_code);
          }
      }

    virtual void variable_(stmt_variable& info)
      {
        result_ += indent() + "VARIABLE " + info.id + '\n';
        result_ += indent() + "\tTYPE \n" + info.type;

        if (!info.value.empty())
          {
            result_ += indent() + "\tVALUE \n";
            render_expression(info.value);
          }
      }

    virtual void for_(stmt_for& info)
      {
        result_ += indent() + "FOR\n";
        result_ += indent() + "\tVARIABLE " + info.init_variable.id;
        result_ += indent() + "\t\tTYPE " + info.init_variable.type;
        if (!info.init_variable.value.empty())
          {
            result_ += indent() + "\t\tVALUE \n";
            indent_ += 2;
            render_expression(info.init_variable.value);
            indent_ -= 2;
          }

        if (!info.cond_expr.empty())
          {
            result_ += indent() + "COND\n";
            render_expression(info.cond_expr);
          }

        if (!info.iter_expr.empty())
          {
            result_ += indent() + "ITER\n";
            render_expression(info.iter_expr);
          }

        render_code(info.for_code);
      }

    virtual void iterfor_(stmt_iter_for& info)
      {
        result_ += indent() + "FOR\n";
        result_ += indent() + "\tVARIABLE " + info.id;
        result_ += indent() + "\t\tTYPE " + info.type.name;
        result_ += indent() + "\tITER ";

        render_expression(info.iter_expr);
        render_code(info.for_code);
      }

    virtual void while_(stmt_while& info)
      {
        result_ += indent() + "WHILE\n";
        result_ += indent() + "\tEXPR ";

        render_expression(info.expr);
        render_code(info.while_code);
      }

    virtual void break_()
      {
        result_ += indent() + "BREAK\n";
      }

    virtual void continue_()
      {
        result_ += indent() + "CONTINUE\n";
      }

    virtual void return_(stmt_return& info)
      {
        result_ += indent() + "RETURN\n";
        if (!info.expr.empty())
          render_expression(info.expr);
      }

    virtual void expression_(stmt_expression& info)
      {
        result_ += indent() + "EXPRESSION\n";
        render_expression(info.expr);
      }

    virtual void dsl_(dsl& info)
      {
      }

    virtual void dispatch(stmt_dispatch& info)
      {
      }
    private:
      code            code_;
      param_list_decl params_;
      XSSContext      ctx_;
      str             result_;
      str             expr_;
      int             indent_;

      str indent()
        {
          str result;
          for(int i = 0; i < indent_; i++)
            result += '\t';

          return result;
        }

      str render_expression(expression& expr)
        {
          expr_ = str();
          
          indent_++;

          str indent__ = indent();
          expr_renderer rend(indent__);
          expr.visit(&rend);

          return rend.get();
        }

      str render_code(code& cde)
        {
          code_renderer cdr(cde, ctx_, indent_ + 1);
          return cdr.render();
        }
  };

struct expression_renderer : IExpressionRenderer
  {
    expression_renderer(expression& expr):
      expr_(expr)
      {
      }

    virtual XSSType type()
      {
        return XSSType();
      }

    virtual str render()
      {
        expr_renderer rend("");
        expr_.visit(&rend);
        return rend.get();
      }
    private:
      expression& expr_;
  };

//debug_language
CodeRenderer debug_language::compile_code(code& cde, param_list_decl& params, XSSContext ctx)
  {
    return CodeRenderer(new code_renderer(cde, ctx));
  }

ExpressionRenderer debug_language::compile_expression(expression expr, XSSContext ctx)
  {
    return ExpressionRenderer(new expression_renderer(expr));
  }

ArgumentRenderer debug_language::compile_args(param_list_decl& params, XSSContext ctx)
  {
    return ArgumentRenderer();  //td:
  }

str debug_language::resolve_this(XSSContext ctx)
  {
    return "THIS";
  }

str debug_language::resolve_separator(XSSObject lh)
  {
    return ".";
  }

