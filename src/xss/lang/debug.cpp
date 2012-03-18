
#include "xss/lang/debug.h"
#include "xss/lang/base.h"
#include "xss/language.h"

using namespace xkp;

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
        result_ += indent_ + lang_utils::operator_string(op) + '\n';
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
    code_renderer():
      indent_(0)
      {
      }

    code_renderer(code& cde, param_list_decl& params, XSSContext ctx, int indent = 0):
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

    virtual void switch_(stmt_switch& info)
      {
      }

    virtual void try_(stmt_try& info)
      {
      }

    virtual void throw_(stmt_throw& info)
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
          param_list_decl pld;
          code_renderer cdr(cde, pld, ctx_, indent_ + 1);
          return cdr.render();
        }
  };

struct expression_renderer : IExpressionRenderer
  {
    expression_renderer()
      {
      }

    expression_renderer(expression& expr):
      expr_(expr)
      {
      }

    ~expression_renderer()
      {
      }

    virtual XSSType type()
      {
        return XSSType();
      }

    virtual variant eval(XSSContext ctx)
      {
        return variant();
      }

    virtual str render()
      {
        expr_renderer rend("");
        expr_.visit(&rend);
        return rend.get();
      }
    private:
      expression expr_;
  };

//debug_language
variant debug_language::compile_code(code& cde, param_list_decl& params, XSSContext ctx)
  {
    return reference<code_renderer>(new code_renderer(cde, params, ctx));
  }

variant debug_language::compile_expression(expression expr, XSSContext ctx)
  {
    return reference<expression_renderer>(new expression_renderer(expr));
  }

variant debug_language::compile_args(param_list_decl& params, XSSContext ctx)
  {
    return variant(); //reference<param_list_renderer>(new param_list_renderer(params));
  }

str debug_language::resolve_this(XSSContext ctx)
  {
    return "THIS";
  }

str debug_language::resolve_separator(XSSObject lh)
  {
    return ".";
  }

bool debug_language::can_cast(XSSType left, XSSType right)
  {
    return true;
  }

void debug_language::init_context(XSSContext ctx)
  {
  }

void debug_language::init_application_context(XSSContext ctx)
  {
  }

XSSType debug_language::resolve_array_type(XSSType type, const str& at_name, XSSContext ctx)
  {
    XSSType new_type(new xss_type);
    new_type->set_id(at_name);
    new_type->as_array(type);

    return new_type;
  }

str debug_language::render_value(XSSType type, variant value)
  {
    return "whatever";
  }

str debug_language::property_get(XSSProperty prop, const str& path, XSSContext ctx)
  {
    return "I do";
  }

str debug_language::property_set(XSSProperty prop, const str& path, const str& value, XSSContext ctx)
  {
    return "what I want";
  }

str debug_language::render_assignment(const str& path, const str& prop, const str& value)
  {
    return "why am I still here?";
  }

str debug_language::expression_path(const str& expr )
  {
    return "there is no path";
  }

str debug_language::array_operation(operator_type op, const str& arr, const str& value, XSSContext ctx)
  {
    return "no rhyme";
  }
  
str debug_language::render_expression(expression& expr, XSSContext ctx)
  {
    return "no goal";
  }

str debug_language::instantiate(XSSType type, XSSObject instance, DynamicArray params)
  {
    return "only instances";
  }

bool debug_language::custom_operator(XSSType lt, XSSType rt, str l, str r, operator_type op, str& res)
  {
    res = "custom operator";
    return false;
  }

//glue

namespace xkp
{
register_complete_type(code_renderer,       renderer_code_schema<code_renderer>);
register_complete_type(expression_renderer, renderer_expr_schema<expression_renderer>);
}
