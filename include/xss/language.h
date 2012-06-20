#ifndef LANGUAGE_HH
#define LANGUAGE_HH

#include "xs/compiler.h"
#include "xss_context.h"
#include "xss_compiler.h"
#include "xss_error.h"

namespace xkp
{

  //util
  struct source_collector :  xs_visitor
    {
      virtual void property_(xs_property& info);
      virtual void method_(xs_method& info);
      virtual void event_(xs_event& info);
      virtual void event_decl_(xs_event_decl& info);
      virtual void const_(xs_const& info);
      virtual void instance_(xs_instance& info);
      virtual void class_(xs_class& info);
      virtual void behaviour_(xs_behaviour& info);
      virtual void behaveas_(xs_implement_behaviour& info);
      virtual void dsl_(dsl& info);

      public:
        std::vector<xs_property> properties;
        std::vector<xs_method>   methods;
        std::vector<xs_event>    events;
        std::vector<xs_instance> instances;
    };

enum assign_type
	{
		VANILLA,
		FN_CALL,
    FN_XSS_CALL,
		XSS_RESOLVE
	};

struct assign_info
	{
		assign_info() : type(VANILLA), flag(false) {}

		assign_type type;
		str					data;
		bool				flag;
	};

struct code_type_resolver : code_visitor
	{
		code_type_resolver(XSSContext ctx, code cde);

		XSSType get();

		//code_visitor
    virtual void return_(stmt_return& info);
		virtual void if_(stmt_if& info);
    virtual void for_(stmt_for& info);
    virtual void iterfor_(stmt_iter_for& info);
    virtual void while_(stmt_while& info);
    virtual void switch_(stmt_switch& info);
    virtual void try_(stmt_try& info);

    virtual void expression_(stmt_expression& info)   {}
    virtual void variable_(stmt_variable& info)       {}
    virtual void break_()                             {}
    virtual void continue_()                          {}
    virtual void dsl_(dsl& info)                      {}
    virtual void dispatch(stmt_dispatch& info)        {}
    virtual void throw_(stmt_throw& info)             {}  
		private:
      code             cde_;
			XSSContext	     ctx_;
			bool				     is_variant_;
      XSSType          result_;

      void return_type_found(XSSType type);
	};

struct expr_type_resolver : expression_visitor
	{
    expr_type_resolver(XSSContext ctx);

		XSSType get();

		//expression_visitor
    virtual void push(variant operand, bool top);
		virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top);

    private:
		  typedef std::stack<variant>		      expr_stack;

		  expr_stack				stack_;
		  bool							is_variant_;
		  operator_registry operators_;
		  XSSContext				ctx_;
  };

struct expr_object_resolver : expression_visitor
  {
    expr_object_resolver(XSSContext ctx);

    variant get();

    //expression_visitor
    virtual void push(variant operand, bool top);
    virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top);

    private:
      typedef std::stack<variant> expr_stack;

      expr_stack stack_;
      XSSContext ctx_;
      bool       one_opnd_;
      bool       not_found_;
  };

struct expr_param_resolver : expression_visitor
  {
    expr_param_resolver() : recording_(false) {}

    param_list& get();

    //expression_visitor
    virtual void push(variant operand, bool top);
    virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top);

    private:
      typedef std::stack<variant> expr_stack;

      bool       recording_; 
      expr_stack stack_;
      param_list result_;
      expression curr_expr_; 
  };

struct expression_analizer
  {
    expression_analizer();

    void        analyze(expression& expr, XSSContext ctx);
    bool        is_property();
    bool        is_identifier();
    bool        this_property();
    XSSProperty get_property();
    expression& get_path();
    str         property_name();
    str         get_identifier();
    variant     get_first();
    str         first_string();
    bool        first_property();
    bool        is_call();
    XSSMethod   method();
    str         method_name();

    private:
      bool        is_identifier_;
      bool        is_constant_;
      bool        is_property_;
      bool        this_property_;
      bool        is_call_;
      str         identifier_;
      variant     constant_;
      expression  path_;
      str         property_name_; 
      XSSProperty property_;
      variant     first_;
      str         first_string_;
      bool        first_property_;
      XSSMethod   method_;
      str         method_name_;

      void analyze_path(expression& expr, operator_type op, XSSContext ctx, str& last_id, XSSObject& instance);
  };

struct lang_utils
	{
    static str     operator_string(operator_type op);
    static int     operator_prec(operator_type op);
    static XSSType code_type(code& code, XSSContext ctx);
    static XSSType expr_type(expression& expr, XSSContext ctx);
    static XSSType resolve_type(variant var, XSSContext ctx);
    static void    var_gatherer(code& cde, XSSContext ctx);
    static variant object_expr(expression& expr, XSSContext ctx);
    static str wind(const std::vector<str> path);
    static std::vector<str> unwind(const str& path);
    
    template <typename T>
    static str render_expression(expression& expr, XSSContext ctx)
      {
		    //deal with assigns
		    operator_type op;
		    if (expr.top_operator(op))
			    {
				    bool simple_assign = false;
				    switch(op)
					    {
						    case op_assign:
							    simple_assign = true;
                case op_plus_equal:
                case op_minus_equal:
                case op_mult_equal:
                case op_div_equal:
                case op_shift_right_equal:
                case op_shift_left_equal:
							    {
								    expression_splitter es(op);
								    expr.visit(&es);

                    T value_renderer(es.right, ctx);
								    es.right.visit(&value_renderer);

								    str value = value_renderer.get();

                    //do assigning 
                    return render_assignment<T>(es.left, op, ctx, value);
							    }
					    }
			    }

        T er(expr, ctx);
        expr.visit(&er);

        return er.get();
      }

    template <typename T>
    static str render_assignment(expression& ass, operator_type op, XSSContext ctx, const str& value)
      {
        expression_analizer anal; 
        anal.analyze(ass, ctx);

        Language lang = ctx->get_language();
        if (anal.is_identifier())
          {
            //simpler case, a = value;
            resolve_info ri;
            str          it = anal.get_identifier();
            if (ctx->resolve(it, ri))
              {
                str path = lang->resolve_this(ctx); 
                switch (ri.what)
                  {
                    case RESOLVE_PROPERTY:
                      {
                        XSSProperty prop = ri.value;
                        if (op != op_assign)
                          return assign_operator(op, prop, path, value, ctx);
                        return lang->property_set(prop, path, value, ctx);
                      }

                    case RESOLVE_VARIABLE:
                      {
                        if (op != op_assign)
                          {
                            str aop = check_array_op(op, XSSProperty(), ri.value, it, value, ctx);
                            if (!aop.empty())
                              return aop;

                            return it + " " + operator_string(op) + " " + value;
                          }
                        return lang->render_assignment(str(), it, value);
                      }
                    default:
                      assert(false);
                  }
              }
            else
              {
                unresolved_assigner(it);
              }
          }
        else if (anal.is_property())
          {
            XSSProperty prop = anal.get_property();
            expression& path = anal.get_path();

            str path_str = lang_utils::render_expression<T>(path, ctx);
            if (prop)
              {
                if (op != op_assign)
                    return assign_operator(op, prop, path_str, value, ctx);

                return lang->property_set(prop, path_str, value, ctx);
              }
            else
              {
                //could't be resolved, so we'll let the language deal with that
                if (op != op_assign)
                  return path_str + lang->resolve_separator() + anal.property_name() + " " + operator_string(op) + " " + value;
                return lang->render_assignment(path_str, anal.property_name(), value);
              }
          }

        //what is it you're assigning?
        invalid_assign();
        return str();
      }

    private:
      static void invalid_assign();
      static void unresolved_assigner(const str& id);
      static str  assign_operator(operator_type op, XSSProperty prop, const str& path, const str& value, XSSContext ctx);
      static str  check_array_op(operator_type op, XSSProperty prop, XSSType type, const str& path, const str& value, XSSContext ctx);

	};

}

#endif
