
#include "xss/idiom.h"

#include <fstream>

using namespace xkp;

//td: !!! stop duplicating this array
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

const int operator_prec[] =
  {
    2,  //"++",   //op_inc,
    2,  //"--",   //op_dec,
    2,  //"&",    //op_ref,
    2,  //"+",    //op_unary_plus,
    2,  //"-",    //op_unary_minus,
    2,  //"!",    //op_not,
    3,  //"*",    //op_mult,
    3,  //"/",    //op_divide,
    3,  //"%",    //op_mod,
    2,  //"as",   //op_typecast,
    2,  //"is",   //op_typecheck,
    2,  //"has",  //op_namecheck,
    4,  //"+",    //op_plus,
    4,  //"-",    //op_minus,
    5,  //">>",   //op_shift_right,
    5,  //"<<",   //op_shift_left,
    14, //">>=",  //op_shift_right_equal,
    14, //"<<=",  //op_shift_left_equal,
    7,  //"==",   //op_equal,
    7,  //"!=",   //op_notequal,
    6,  //">",    //op_gt,
    6,  //"<",    //op_lt,
    6,  //">=",   //op_ge,
    6,  //"<=",   //op_le,
    11, //"&&",   //op_and,
    12, //"||",   //op_or,
    14, //"=",    //op_assign,
    14, //"+=",   //op_plus_equal,
    14, //"-=",   //op_minus_equal,
    14, //"*=",   //op_mult_equal,
    14, //"/=",   //op_div_equal,
    1,  //".",    //op_dot,
    1,  //".",    //op_dot_call
    1,  //"[]",   //op_index,
    1,  //"",     //op_call,
    1,  //"",     //op_func_call
    1,  //"",     //op_array,
    1,  //"",     //op_parameter
  };


//detail

struct already_rendered
  {
    already_rendered() : precedence(0)
    {
    }

    str     value;
    variant object;
    int precedence;
  };

struct expression_renderer : expression_visitor
  {
    expression_renderer(XSSContext ctx) : ctx_(ctx) {}

    XSSProperty get_property(variant v)
      {
        if (v.is<XSSProperty>())
          return v;

        if (v.is<already_rendered>())
          {
            already_rendered ar = v;
            return get_property(ar.object);
          }
        else if (v.is<expression_identifier>())
          {
            expression_identifier ei = v;
            return ctx_->get_property(ei.value);
          }

        return XSSProperty();
      }

    XSSProperty get_property(variant v, const str& name)
      {
        if (v.is<DynamicObject>())
          {
            DynamicObject obj = v;
            return ctx_->get_property(obj, name);
          }
        else if (v.is<already_rendered>())
          {
            already_rendered ar = v;
            return get_property(ar.object, name);
          }
        else if (v.is<expression_identifier>())
          {
            assert(false); //td:
          }

        return XSSProperty();
      }

    DynamicObject get_instance(variant v)
      {
        if (v.is<DynamicObject>())
          {
            DynamicObject obj = v;
            return obj;
          }
        else if (v.is<already_rendered>())
          {
            already_rendered ar = v;
            return get_instance(ar.object);
          }
        else if (v.is<expression_identifier>())
          {
            expression_identifier ei = v;
            return ctx_->resolve_instance(ei.value);
          }

        return DynamicObject();
      }

    DynamicObject get_instance(variant v, const str& name)
      {
        if (v.is<DynamicObject>())
          {
            DynamicObject obj = v;
            return variant_cast<DynamicObject>(ctx_->evaluate_property(obj, name), DynamicObject());
          }
        else if (v.is<already_rendered>())
          {
            already_rendered ar = v;
            return variant_cast<DynamicObject>(ctx_->evaluate_property(ar.object, name), DynamicObject());
          }
        else if (v.is<expression_identifier>())
          {
            assert(false);
          }

        return DynamicObject();
      }

    //expression_visitor
    virtual void push(variant operand)
      {
        stack_.push(operand);
      }

    virtual void exec_operator(operator_type op, int pop_count, int push_count)
      {
        variant arg1, arg2;
        switch(pop_count)
          {
            case 0: break;
            case 1: arg1 = stack_.top(); stack_.pop(); break;
            case 2:
              {
                arg2 = stack_.top(); stack_.pop();
                arg1 = stack_.top(); stack_.pop();
                break;
              }
            default: assert(false);
          }

        int op_prec = operator_prec[op];
        switch(op)
          {
            case op_dec:
            case op_inc:
              {
                int prec;
                str os = operand_to_string(arg1, DynamicObject(), &prec);

                std::stringstream ss;
                if (op_prec < prec)
                  ss << "(" << os << ")" << operator_str[op];
                else
                  ss << os << operator_str[op];

                push_rendered(ss.str(), op_prec, arg1.get_schema());
                break;
              }

            case op_unary_plus:
            case op_unary_minus:
            case op_not:
              {
                int prec;
                str os = operand_to_string(arg1, DynamicObject(), &prec);

                std::stringstream ss;
                if (op_prec < prec)
                  ss << operator_str[op] << "(" << os << ")";
                else
                  ss << operator_str[op] << os;

                push_rendered(ss.str(), op_prec, arg1.get_schema());
                break;
              }

            case op_assign:
              {
                std::stringstream ss;
                str os1 = operand_to_string(arg1);
                str os2 = operand_to_string(arg2);

                //we need to know if we're assigning to a property that might have a setter
                XSSProperty prop = get_property(arg1);
                if (prop)
                  {
                    str set_fn = variant_cast<str>(dynamic_get(prop, "set_fn"), ""); //let the outside world determine
                                                                                     //if a native function call shouls be made 
                    if (!set_fn.empty())
                      {
                        //replace the property name with the set function
                        //this gets a little hacky, I admit, because of a use case
                        //where the get function has already been inserted
                        str  prop_name = prop->name;
                        bool found     = false;
                        if (os1.size() > prop_name.size())
                          {
                            //check for the property name at the end of os1
                            str ppnm = os1.substr(os1.size() - prop_name.size(), prop_name.size());
                            if (ppnm == prop_name)
                              {
                                os1.erase(os1.size() - prop_name.size(), prop_name.size());
                                os1 += set_fn;
                                found = true;
                              }
                          }
                        
                        if (!found)
                          {
                            str get_fn = variant_cast<str>(dynamic_get(prop, "get_fn"), ""); 
                            if (!get_fn.empty() && os1.size() > (get_fn.size() + 2)) //in the hacky case () has been added
                              {
                                str ppnm = os1.substr(os1.size() - (get_fn.size() + 2), get_fn.size() + 2);
                                if (ppnm == (get_fn + "()"))
                                  {
                                    os1.erase(os1.size() - 2, 2); //just get rid of the ()
                                    found = true;
                                  }
                              }
                          }
                        
                        ss << os1 << "(" << os2 << ")";
                      }
                    else if (!prop->set.empty())
                      ss << os1 << "_set(" << os2 << ")"; //td: formalize the calling conventions
                    else 
                      ss << os1 << " = " << os2;
                  }
                else
                  {
                    ss << os1 << " = " << os2;
                  }

                push_rendered(ss.str(), op_prec, prop);
                break;
              }

            case op_mult:
            case op_divide:
            case op_mod:
            case op_typecast:
            case op_typecheck:
            case op_namecheck:
            case op_plus:
            case op_minus:
            case op_shift_right:
            case op_shift_left:
            case op_equal:
            case op_notequal:
            case op_gt:
            case op_lt:
            case op_ge:
            case op_le:
            case op_and:
            case op_or:

            case op_plus_equal:
            case op_minus_equal:
            case op_mult_equal:
            case op_div_equal:
            case op_shift_right_equal:
            case op_shift_left_equal:
              {
                int p1;
                int p2;

                str os1 = operand_to_string(arg1, DynamicObject(), &p1);
                str os2 = operand_to_string(arg2, DynamicObject(), &p2); //td: resolve properties and stuff

                if (op_prec < p1)
                  os1 = "(" + os1 + ")";

                if (op_prec < p2)
                  os2 = "(" + os2 + ")";

                std::stringstream ss;
                ss << os1 << " " << operator_str[op] << " " << os2;
                push_rendered(ss.str(), op_prec, variant());
                break;
              }

            case op_dot:
              {
                //so here we'll try to find out if we're generating an object
                //with a get function on a property. This does not constitute
                //type checking, that's a lot of work unless the code_linker feels
                //like cooperating
                DynamicObject o1 = get_instance(arg1);
                str           s2 = operand_to_string(arg2, o1);
                DynamicObject o2 = get_instance(o1, s2);

                XSSProperty   prop       = get_property(o1, s2);
                bool          has_getter = prop && !prop->get.empty();

                std::stringstream ss;
                ss << operand_to_string(arg1);
                
                if (prop)
                  {
                    str get_fn = variant_cast<str>(dynamic_get(prop, "get_fn"), ""); 
                    if (!get_fn.empty())
                      ss << "." << get_fn << "()";
                    else if (has_getter)
                      ss << "." << s2 << "_get()";
                    else   
                      ss << "." << s2;
                  }
                else 
                   ss << "." << s2;

                if (prop)
                  push_rendered(ss.str(), op_prec, prop);
                else
                  push_rendered(ss.str(), op_prec, o2);
                break;
              }

            case op_dot_call:
              {
                std::stringstream ss;
                ss << operand_to_string(arg1) << "." << operand_to_string(arg2);
                push_rendered(ss.str(), op_prec, variant());
                break;
              }

            case op_call:
            case op_func_call:
              {
                std::stringstream result;

                result << "(";

                int args = arg1;
                //pop the arguments
                for(int i = 0; i < args; i++)
                  {
                    variant arg  = stack_.top(); stack_.pop();
                    str     sarg = operand_to_string(arg);
                    
                    XSSProperty prop = get_property(arg);
                    if (prop)
                      {
                        str get_fn = variant_cast<str>(dynamic_get(prop, "get_fn"), ""); 
                        if (!get_fn.empty())
                          result << get_fn << "()";
                        else if (!prop->get.empty())
                          result << sarg << "_get()";
                        else   
                          result << sarg;
                      }
                    else 
                       result << sarg;

                    if (i < args - 1)
                      {
                        result << ", ";
                      }
                  }

                result << ")";

                variant top = stack_.top(); stack_.pop();
                str caller = operand_to_string(top);

                push_rendered(caller + result.str(), op_prec, variant()); //td: we could find out the type here or something
                break;
              }

            case op_parameter:
              {
                //do nothing, this is just a notification
                break;
              }

            case op_array:
              {
                assert(false); //td
                break;
              }

            case op_index:
            default:
              assert(false); //td:
          }
      }

    str get()
      {
        if(stack_.empty())
          return "***EMPTY STACK***"; //td: error

        variant result = stack_.top();
        if (result.is<already_rendered>())
          {
            already_rendered ar = result;
            return ar.value;
          }
        else if (result.is<str>())
          {
            str res = result;
            str ss = '"' + res + '"';
            return ss;
          }

        str to_string = result;
        return to_string;
      }

    protected:
      typedef std::stack<variant> expr_stack;

      expr_stack        stack_;
      std::stringstream result_;
      XSSContext        ctx_;

      str operand_to_string(variant operand, DynamicObject parent = DynamicObject(), int* prec = null)
        {
          str result;
          int result_prec = 0;
          if (operand.is<expression_identifier>())
            {
              expression_identifier ei = operand;
              result = ei.value;

              if (!parent)
                {
                  //here we ought to resolve a single symbol (ex width = 10)
                  //thid *could* belong to the "this" pointer
                  
                  if (ctx_->idiom_)
                    {
                      XSSProperty prop = ctx_->get_property(ei.value);
                      if (prop)
                        {
                          //well, lets see how the idiom handles thises
                          str this_str = ctx_->idiom_->resolve_this(ctx_);
                          if (!this_str.empty())
                            result = this_str + "." + ei.value; //otherwise it doesnt get translated 
                        }
                    }
                }
            }
          else if (operand.is<already_rendered>())
            {
              already_rendered ar = operand;
              result = ar.value;
              result_prec = ar.precedence;
            }
          else if (operand.is<str>())
            {
              str opstr = operand;
              result = '"' + opstr + '"';
            }
          else
            {
              str opstr = operand;
              result = opstr;
            }

          if (prec) *prec = result_prec;
          return result;
        }

      void push_rendered(str value, int prec, variant object)
        {
          already_rendered ar;
          ar.object = object;
          ar.value = value;
          ar.precedence = prec;

          stack_.push(ar);
        }
  };

str render_expression(expression& expr, XSSContext ctx)
  {
    expression_renderer er(ctx);
    expr.visit(&er);

    return er.get();
  }

struct code_renderer : code_visitor
  {
    code_renderer(XSSContext ctx, int indent): ctx_(ctx), indent_(indent) {}

    str get()
      {
        return result_;
      }

    //code_visitor
    virtual void if_(stmt_if& info)
      {
        std::stringstream ss;
        str ind = get_indent_str();

        ss  << ind << "if (" << render_expression(info.expr, ctx_) << ")" << '\n'
            << ind << "{" << '\n'
                   << render_code(info.if_code, indent_ + 1) << '\n'
            << ind << "}" << '\n';

        if (!info.else_code.empty())
          ss  << ind << "{"
                << render_code(info.else_code, indent_ + 1)
              << ind << "}";

        add_line(ss.str());
      }

    virtual void variable_(stmt_variable& info)
      {
        std::stringstream ss;
        str ind = get_indent_str();

        ss << ind << "var " << info.id;
        if (!info.value.empty())
          ss << " = " << render_expression(info.value, ctx_);

        ss << ";\n";

        add_line(ss.str());
      }

    virtual void for_(stmt_for& info)
      {
        std::stringstream ss;
        str ind = get_indent_str();

        ss << ind << "for(var " << info.init_variable.id << " = " << render_expression(info.init_variable.value, ctx_)
           << "; " << render_expression(info.cond_expr, ctx_)
           << "; " << render_expression(info.iter_expr, ctx_) << ")\n";

        ss << ind << "{" << "\n"
                  << render_code(info.for_code, indent_ + 1);
        ss << ind << "}" << "\n";

        add_line(ss.str());
      }

    virtual void iterfor_(stmt_iter_for& info)
      {
        std::stringstream ss;
        str ind = get_indent_str();

        str iterable_name = info.id + "_iterable";
        str iterator_name = info.id + "_iterator";
        ss << ind << "var " << iterable_name << " = " << render_expression(info.iter_expr, ctx_) << "\n";
        ss << ind << "for(var " << iterator_name << " = 0; " 
           << iterator_name << " < " << iterable_name << ".length; "
           << iterator_name << "++" << ")\n";

        ss << ind << "{" << "\n"
                  <<    "var " << info.id << " = " << iterable_name << "[" << iterator_name << "];\n"
                  <<    render_code(info.for_code, indent_ + 1);
        ss << ind << "}" << "\n";

        add_line(ss.str());
      }

    virtual void while_(stmt_while& info)
      {
        std::stringstream ss;
        str ind = get_indent_str();

        ss << ind << "while(" << render_expression(info.expr, ctx_) << ")\n" ;
        ss << ind << "{" << "\n"
                  <<    render_code(info.while_code, indent_ + 1);
        ss << ind << "}" << "\n";

        add_line(ss.str());
      }

    virtual void break_()
      {
        add_line("break;", true);
      }

    virtual void continue_()
      {
        add_line("continue;", true);
      }

    virtual void return_(stmt_return& info)
      {
        if (info.expr.empty())
          add_line("return;", true);
        else
          add_line("return " + render_expression(info.expr, ctx_) + ";", true);
      }

    virtual void expression_(stmt_expression& info)
      {
        str value = render_expression(info.expr, ctx_);
        add_line(value + ";", true);
      }

    virtual void dsl_(dsl& info)
      {
        assert(false); //td: there is some stuff to implement here... later
      }

    virtual void dispatch(stmt_dispatch& info)
      {
        assert(false); //td: ought to define what to do here, it would seem like the idiom would like
                       //to handle this 
      }
    private:
      str        result_;
      XSSContext ctx_;
      int        indent_;

      void add_line(const str& line, bool dress_line = false)
        {
          if (dress_line)
            result_ += get_indent_str();

          result_ += line;

          if (dress_line)
            result_ += '\n';
        }

      str render_code(code& code, int indent)
        {
          code_renderer renderer(ctx_, indent);
          code.visit(&renderer);

          return renderer.get();
        }

      str get_indent_str()
        {
          str result;
          for(int i = 0; i < indent_*4; i++)
            {
              result += ' ';
            }

          return result;
        }
  };

//base_xss_expression
base_xss_expression::base_xss_expression()
  {
  }

base_xss_expression::base_xss_expression(XSSContext ctx, expression& expr):
  expr_(expr),
  ctx_(ctx)
  {
  }

str base_xss_expression::generate()
  {
    return render_expression(expr_, ctx_);
  }

//base_xss_code
base_xss_code::base_xss_code()
  {
  }

base_xss_code::base_xss_code(XSSContext ctx, code& code):
  code_(code),
  ctx_(ctx)
  {
  }

str base_xss_code::generate()
  {
    code_renderer renderer(ctx_, 0);
    code_.visit(&renderer);

    return renderer.get();
  }

//base_xss_args
base_xss_args::base_xss_args()
  {
  }

base_xss_args::base_xss_args(const base_xss_args& other):
  args_(other.args_)
  {
  }

base_xss_args::base_xss_args(param_list_decl& args):
  args_(args)
  {
  }

str base_xss_args::generate()
  {
    std::ostringstream oss;

    param_list_decl::iterator it = args_.begin();
    param_list_decl::iterator nd = args_.end();
    for(; it != nd; it++)
      {
        //td: defaults values, etc
        oss << it->name << ",";
      }

    str result = oss.str();
    if (!result.empty())
      result.erase(result.end() - 1);
    return result;
  }

//base_xss_function
base_xss_function::base_xss_function()
  {
  }


base_xss_function::base_xss_function( const code& code, const str& name, XSSContext ctx, param_list_decl& args):
  code_(code),
  name_(name),
  args_(args),
  ctx_(ctx)
  {
  }

str base_xss_function::generate_code()
  {
    code_renderer renderer(ctx_, 0);
    code_.visit(&renderer);

    return renderer.get();
  }

//base_idiom
void base_idiom::set_context(XSSContext ctx)
  {
    ctx_ = ctx;
  }

variant base_idiom::process_method(DynamicObject instance, xs_method& mthd)
  {
    XSSContext ctx(new xss_composite_context(ctx_));
    ctx->this_ = instance;

    xssFunction result(new base_xss_function(mthd.cde, mthd.name, ctx, mthd.args));
    functions_.push_back(result);
    return result;
  }

variant base_idiom::process_event(DynamicObject instance, const str& event_name, xs_event& ev)
  {
    XSSContext ctx(new xss_composite_context(ctx_));
    ctx->this_ = instance;

    xssFunction result(new base_xss_function(ev.cde, event_name, ctx, ev.args));
    functions_.push_back(result);
    return result;
  }

variant base_idiom::process_code(code& cde, DynamicObject this_)
  {
    XSSContext ctx(new xss_composite_context(ctx_));
    ctx->this_ = this_;

    xssCode result(new base_xss_code(ctx, cde));
    return result;
  }

variant base_idiom::process_expression(expression expr, DynamicObject this_)
  {
    XSSContext ctx(new xss_composite_context(ctx_));
    ctx->this_ = this_;

    xssExpression result(new base_xss_expression(ctx, expr));
    return result;
  }

str base_idiom::resolve_this(XSSContext ctx)
  {
    //orogramming languages do not agree on how to use this pointers
    //c++ lets you ignore them, jsva script et all force you to 
    //and there are other intrincate circumstances (like functions in js)
    //where something must be done.
    
    if (id_as_this_)
      {
        if (!ctx->this_.empty())
          {
            str iid = variant_cast<str>(dynamic_get(ctx->this_, "id"), ""); 
            assert(!iid.empty()); //td: error, nameless instance
            
            return iid;
          }
      }
    else if (force_this_ && !ctx->this_.empty())
      {
        return "this";
      }    
      
    return "";
  }
