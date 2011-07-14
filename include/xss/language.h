#ifndef LANGUAGE_HH
#define LANGUAGE_HH

#include "xs/compiler.h"
#include "xss_context.h"

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

struct code_type_resolver : code_visitor
	{
		code_type_resolver(XSSContext ctx);

		XSSType get();

		//code_visitor
    virtual void variable_(stmt_variable& info);
    virtual void return_(stmt_return& info);
		virtual void if_(stmt_if& info);
    virtual void for_(stmt_for& info);
    virtual void iterfor_(stmt_iter_for& info);
    virtual void while_(stmt_while& info);
    virtual void break_();
    virtual void continue_();
    virtual void expression_(stmt_expression& info);
    virtual void dsl_(dsl& info);
    virtual void dispatch(stmt_dispatch& info);
	};

struct expr_type_resolver : expression_visitor
	{
    expr_type_resolver(XSSContext ctx);

		XSSType get();
    variant top_stack();

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
      //CUBA
      XSSType   result_xs_type_info;
			XSSObject result_xss;

			XSSType resolve_type(variant var);
			XSSObject resolve_xss_type(variant var);
  };

struct lang_utils
	{
    static str get_operator_str(operator_type op);
    static int get_operator_prec(operator_type op);

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

                    T value_renderer(ctx);
								    es.right.visit(&value_renderer);

								    str value = value_renderer.get();

								    //get the assigner
                    T assign_renderer(ctx);
								    assign_info ai;
								    assign_renderer.assigner = &ai;

								    es.left.visit(&assign_renderer);

								    str assign = assign_renderer.get();

								    str result;
								    switch(ai.type)
									    {
										    case FN_CALL:
											    {
												    if (simple_assign)
													    result = assign + "(" + value + ")";
												    else
													    {
														    //we need to resolve the getter as well for *= operators
                                T getter(ctx);
														    es.left.visit(&getter);

														    //this is lame
                                str op_str = get_operator_str(op);

														    assert(op_str.size() > 1);
														    op_str.erase(op_str.end() - 1);

														    result = assign + "(" + getter.get() + " " + op_str + " " + value + ")";
													    }
												    break;
											    }

										    case VANILLA:
											    {
                            result = assign + " " + get_operator_str(op) + " " + value;
												    break;
											    }
										    case XSS_RESOLVE:
											    {
                            T getter_(ctx);
												    es.left.visit(&getter_);
												    str getter = getter_.get();

												    if (!simple_assign)
													    {
                                str op_str = get_operator_str(op);
														    assert(op_str.size() > 1);
														    op_str.erase(op_str.end() - 1);

														    value = getter + " " + op_str + " " + value;
													    }

														//lets find out if the getter is a single word (like "width") or a composite (like "object.width")
														//if composite we'll get rid of the last accesor (width) to get the actual accesor

														xss_idiom* idiom = ctx->idiom_;
														str separator = idiom->resolve_separator();
														size_t last_dot = getter.find_last_of(separator);
												    if (last_dot != str::npos)
													    {
														    size_t count = getter.size() - last_dot + 1;
														    getter.erase(getter.end() - count, getter.end());
													    }
												    else
															getter = idiom->resolve_this(ctx);

												    //replace quotes, life is hard
												    for(size_t i = 0; i < assign.size(); i++)
													    {
														    if (assign[i] == 39)
															    assign[i] = '"';
													    }

												    //td: is all this really neccesary?
												    XSSProject project_ = ctx->project_;
												    xss_idiom* idiom_		= ctx->idiom_;
														XSSContext context(new xss_code_context(project_, idiom_, fs::path()));
												    xss_code_context& ctx = *context.get();

												    XSSGenerator gen(new xss_generator(context));
												    project_->push_generator(gen);

												    project_->prepare_context(ctx, gen);

												    ctx.scope_->register_symbol("value",	value);
												    ctx.scope_->register_symbol("caller", getter);

												    result = project_->generate_xss(assign, gen);

												    project_->pop_generator();
												    break;
											    }
										    default:
											    assert(false); //trap use cases
									    }

								    return result;
							    }
					    }
			    }

        T er(ctx);
        expr.visit(&er);

        return er.get();
      }

		static str expr2str(expression& expr, XSSContext ctx);
	};

}

#endif