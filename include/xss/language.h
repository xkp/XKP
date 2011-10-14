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

    virtual void expression_(stmt_expression& info)   {}
    virtual void variable_(stmt_variable& info)       {}
    virtual void break_()                             {}
    virtual void continue_()                          {}
    virtual void dsl_(dsl& info)                      {}
    virtual void dispatch(stmt_dispatch& info)        {}
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

		private:
			XSSType resolve_type(variant var);
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

    private:
      bool        is_identifier_;
      bool        is_constant_;
      bool        is_property_;
      bool        this_property_;
      str         identifier_;
      variant     constant_;
      expression  path_;
      str         property_name_; 
      XSSProperty property_;
  };

struct lang_utils
	{
    static str     operator_string(operator_type op);
    static int     operator_prec(operator_type op);
    static XSSType code_type(code& code, XSSContext ctx);
    static XSSType expr_type(expression& expr, XSSContext ctx);
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

								    //td: !!!! clutter
                    ////get the assigner
            //        T assign_renderer(es.left, ctx);
								    //assign_info ai;
            //        ai.data = value;
								    //assign_renderer.assigner = &ai;

								    //es.left.visit(&assign_renderer);

								    //str assign = assign_renderer.get();

								    //str result;
								    //switch(ai.type)
									   // {
										  //  case FN_CALL:
											 //   {
												//    if (simple_assign)
												//	    result = assign + "(" + value + ")";
												//    else
												//	    {
												//		    //we need to resolve the getter as well for *= operators
            //                    T getter(es.left, ctx);
												//		    es.left.visit(&getter);

												//		    //this is lame
            //                    str op_str = lang_utils::operator_string(op);

												//		    assert(op_str.size() > 1);
												//		    op_str.erase(op_str.end() - 1);

												//		    result = assign + "(" + getter.get() + " " + op_str + " " + value + ")";
												//	    }
												//    break;
											 //   }

										  //  case VANILLA:
											 //   {
            //                resolve_info ri;
            //                result = assign + " " + lang_utils::operator_string(op) + " " + value;

            //                if (ctx->resolve(assign, ri))
            //                  {
            //                    if (ri.type && ri.type->is_array())
            //                      {
            //                        if (op == op_plus_equal || op == op_minus_equal)
            //                          {
            //                            result = assign_renderer.array_operation(assign, value, op);
            //                          }
            //                        else if (simple_assign && ai.flag && !ai.data.empty())
            //                          {
            //                            result = ai.data;
            //                          }
            //                      }
            //                  }

												//    break;
											 //   }
										  //  case XSS_RESOLVE:
											 //   {
            //                T getter_(es.left, ctx);
												//    es.left.visit(&getter_);
												//    str getter = getter_.get();

												//    if (!simple_assign)
												//	    {
            //                    str op_str = lang_utils::operator_string(op);
												//		    assert(op_str.size() > 1);
												//		    op_str.erase(op_str.end() - 1);

												//		    value = getter + " " + op_str + " " + value;
												//	    }

												//		//lets find out if the getter is a single word (like "width") or a composite (like "object.width")
												//		//if composite we'll get rid of the last accesor (width) to get the actual accesor

												//		Language lang = ctx->get_language();
												//		str separator = lang->resolve_separator();
												//		size_t last_dot = getter.find_last_of(separator);
												//    if (last_dot != str::npos)
												//	    {
												//		    size_t count = getter.size() - last_dot + 1;
												//		    getter.erase(getter.end() - count, getter.end());
												//	    }
												//    else
												//			getter = lang->resolve_this(ctx);

												//    //replace quotes, life is hard
												//    for(size_t i = 0; i < assign.size(); i++)
												//	    {
												//		    if (assign[i] == 39)
												//			    assign[i] = '"';
												//	    }

            //                XSSContext assign_ctx(new xss_context(XSSContext()));
												//    assign_ctx->register_symbol(RESOLVE_CONST, "value",	value);
												//    assign_ctx->register_symbol(RESOLVE_CONST, "caller", getter);

												//    XSSCompiler compiler = ctx->resolve("compiler");
            //                XSSRenderer renderer = compiler->compile_xss(assign, assign_ctx);

												//    result = renderer->render(XSSObject(), null);
												//    break;
											 //   }

										  //  case FN_XSS_CALL:
											 //   {
												//    result = assign + "(" + value + ")";

												//    break;
											 //   }

										  //  default:
											 //   assert(false); //trap use cases
									   // }

								    //return result;
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
