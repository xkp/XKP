#ifndef XSS_LANG_BASE_H
#define XSS_LANG_BASE_H

#include <xss/language.h>
#include <xss/xss_context.h>

namespace xkp{


//expression rendering
struct already_rendered
  {
    already_rendered() : precedence(0)
    {
    }

    str     value;
    variant object;
    int precedence;
    str path;
  };

struct capture_property
	{
		XSSProperty prop;
		str					xss;
	};

struct variable_gather : code_visitor
  {
    variable_gather(XSSContext ctx);

    void apply();

    //code_visitor
    virtual void variable_(stmt_variable& info);
    virtual void if_(stmt_if& info);
    virtual void for_(stmt_for& info);
    virtual void iterfor_(stmt_iter_for& info);
    virtual void while_(stmt_while& info);
    virtual void expression_(stmt_expression& info);

    virtual void return_(stmt_return& info)           {}
    virtual void break_()                             {}
    virtual void continue_()                          {}
    virtual void dsl_(dsl& info)                      {}
    virtual void dispatch(stmt_dispatch& info)        {}
    virtual void switch_(stmt_switch& info)           {}

    private:
      typedef std::vector<str> var_list;
      var_list        var_vars_;
      XSSContext      ctx_;
  };

struct base_code_renderer : ICodeRenderer,
                            code_visitor
  {
    base_code_renderer();
    base_code_renderer(const base_code_renderer& other);
    base_code_renderer(code& cde, param_list_decl& params, XSSContext ctx, int indent = 0);

    code& get_code();

    //ICodeRenderer
    virtual str     render();
    virtual XSSType type();

    //code_visitor
    virtual void if_(stmt_if& info);
    virtual void variable_(stmt_variable& info);
    virtual void for_(stmt_for& info);
    virtual void iterfor_(stmt_iter_for& info);
    virtual void while_(stmt_while& info);
    virtual void break_();
    virtual void continue_();
    virtual void return_(stmt_return& info);
    virtual void expression_(stmt_expression& info);
    virtual void dsl_(dsl& info);
    virtual void dispatch(stmt_dispatch& info);
    virtual void switch_(stmt_switch& info);

    private:
      param_list_decl params_;
      str             result_;
      str             expr_;
      XSSType         type_;
      bool            return_type_;

    protected:
      code            code_;
      XSSContext      ctx_;
      int             indent_;
      str             indent_str_;

      str indent();

      void    add_line(str line, bool trim = true);
      virtual str render_expression(expression& expr, XSSContext ctx) = 0;
      virtual str render_code(code& cde)                              = 0;
  };

struct base_expr_renderer : IExpressionRenderer,
                            expression_visitor
  {
    base_expr_renderer();
    base_expr_renderer(const base_expr_renderer& other);
    base_expr_renderer(expression& expr, XSSContext ctx);

    //interface, verbatim from 0.8, got to keep it working
    virtual str operand_to_string(variant operand, XSSObject parent = XSSObject(), int* prec = null);
    virtual str array_operation(const str& left, const str& right, operator_type op);
    virtual str get();

    //there has to be a way to make this prettier
    assign_info* assigner;

    //IExpressionRenderer
    virtual XSSType type();
    virtual variant eval(XSSContext ctx);
    virtual str     render() = 0;

    //expression_visitor
    virtual void push(variant operand, bool top);
    virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top);

    protected:
      XSSType    type_;

      XSSObject   get_instance(variant v);
      XSSProperty get_property(variant v);
    protected:
      //meat
      typedef std::stack<variant> expr_stack;

      XSSContext        ctx_;
      expression        expr_;

      expr_stack        stack_;
			bool							capturing_property_;
			capture_property	capture_property_;

      void push_rendered(str value, int prec, variant object, const str& path = str());
			str	 render_captured_property();
  };

struct base_args_renderer : public IArgumentRenderer
  {
    base_args_renderer();
    base_args_renderer(const base_args_renderer& other);
    base_args_renderer(param_list_decl& params, XSSContext ctx);

    //IArgumentRenderer
    virtual str              render() = 0;
    virtual void             add(const str& name, XSSType type);
    virtual param_list_decl& get();

    protected:
      typedef std::vector< std::pair<str, XSSType> > extra_arg_list;

      XSSContext      ctx_;
      param_list_decl args_;
      extra_arg_list  extra_;

      virtual str render_expression(expression& expr, XSSContext ctx) = 0;
  };

struct base_lang : public ILanguage
  {
    virtual variant compile_code(code& cde, param_list_decl& params, XSSContext ctx) = 0;
    virtual variant compile_expression(expression expr, XSSContext ctx)              = 0;
		virtual variant compile_args(param_list_decl& params, XSSContext ctx)            = 0;
    virtual str     resolve_this(XSSContext ctx);
    virtual str     resolve_separator(XSSObject lh = XSSObject());
    virtual bool    can_cast(XSSType left, XSSType right);
    virtual void    init_context(XSSContext ctx);
    virtual void    init_application_context(XSSContext ctx);
    virtual XSSType resolve_array_type(XSSType type, const str& at_name, XSSContext ctx);
    virtual str     render_value(XSSType type, variant value);
    virtual str     render_expression(expression& expr, XSSContext ctx);
    virtual str     instantiate(XSSType type, XSSObject instance, DynamicArray params);
    virtual bool    custom_operator(XSSType lt, XSSType rt, str l, str r, operator_type op, str& res);

    protected:
      void compile_property(XSSProperty prop, XSSContext ctx);
  };

//interface glue
template <typename T>
struct renderer_code_schema : renderer_schema<T>
  {
    virtual void declare_base()
      {
        this->template implements<ICodeRenderer>();

        this->template readonly_property<XSSType>("type", &T::type);

        //maintain base implementations
        renderer_schema<T>::declare_base();
      }
  };

template <typename T>
struct renderer_expr_schema : renderer_schema<T>
  {
    virtual void declare_base()
      {
        this->template implements<IExpressionRenderer>();

        this->template readonly_property<XSSType>("type", &T::type);

        //maintain base implememtations
        renderer_schema<T>::declare_base();
      }
  };

template <typename T>
struct renderer_args_schema : renderer_schema<T>
  {
    virtual void declare_base()
      {
        this->template implements<IArgumentRenderer>();

        //maintain base implememtations
        renderer_schema<T>::declare_base();
      }
  };
}
#endif
