#include "xss/xss_expression.h"
#include "xss/language.h"

using namespace xkp;

const str SExpression("xss-expression");

const str SAssignOperatorOnlyTop("Assign operators can only be used as the base of an expression");

struct expression_builder : expression_visitor
  {
    virtual void push(variant operand, bool top)
      {
        if (operand.is<expression_identifier>())
          {
            expression_identifier debuggg = operand;
            debuggg.value = str();
          }
        stack_.push(operand);
      }

    virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top)
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

        int op_prec = lang_utils::operator_prec(op);
        switch(op)
          {
            case op_dec:
            case op_inc:
            case op_unary_plus:
            case op_unary_minus:
            case op_not:
              {
                XSSExpression curr = get_expression(arg1);
                XSSExpression this_expr(new xss_expression(op, curr));

                push(this_expr, top);
                break;
              }

            case op_assign:
            case op_plus_equal:
            case op_minus_equal:
            case op_mult_equal:
            case op_div_equal:
            case op_shift_right_equal:
            case op_shift_left_equal:
              {
                if (!top)
                  {
						        param_list error;
						        error.add("id", SExpression);
						        error.add("desc", SAssignOperatorOnlyTop);

						        xss_throw(error);
                  }
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
              {
                XSSExpression left  = get_expression(arg1);
                XSSExpression right = get_expression(arg2);
                XSSExpression this_expr(new xss_expression(op, left, right));

                push(this_expr, top);
                break;
              }

				    case op_dot:
              {
                XSSValue value = get_value(arg1);

                expression_identifier ei = arg2;

                value_operation op(OP_READ, ei.value);

                value->add_operation(op);
                push(value, top);
                break;
              }

            case op_dot_call:
              {
                XSSValue value = get_value(arg1);
                expression_identifier ei = arg2;

                XSSArguments args(new xss_arguments);
                value_operation op(OP_CALL, ei.value);
                op.set_arguments(args);

                value->add_operation(op);
                push(value, top);
                break;
              }

            case op_instantiate:
              {
                xs_type caller    = arg1;
                int     arg_count = arg2;

                XSSArguments args = get_stack_arguments(arg_count, true);
                XSSArguments type_args(new xss_arguments);
                
                std::vector<xs_const>::iterator tpit = caller.args.begin();
                std::vector<xs_const>::iterator tpnd = caller.args.end();
                for(; tpit != tpnd; tpit++)
                  {
                    type_args->add(tpit->name, xss_expression_utils::compile_expression(tpit->value));
                  }
                
                XSSValue result(new xss_value);
                value_operation op(OP_INSTANTIATION, caller.name); 
                op.set_arguments(args);
                op.set_type_arguments(type_args);

                result->add_operation(op);
                push(result, top);
                break;
              }

            case op_object:
              {
                std::vector<xs_const> values = arg1;
                XSSArguments obj_values(new xss_arguments);

                std::vector<xs_const>::iterator it = values.begin();
                std::vector<xs_const>::iterator nd = values.end();
                for(; it != nd; it++)
                  {
                    obj_values->add(it->name, xss_expression_utils::compile_expression(it->value));
                  }

                XSSValue result(new xss_value);

                value_operation op(OP_OBJECT, str()); 
                op.set_arguments(obj_values);

                result->add_operation(op);
                push(result, top);
                break;
              }

            case op_func_call:
              {
                expression_identifier ei        = arg1;
                int                   arg_count = arg2;
                XSSArguments          args      = get_stack_arguments(arg_count, false);

                XSSValue result(new xss_value);
                value_operation op(OP_CALL, ei.value); 
                op.set_arguments(args);

                result->add_operation(op);
                push(result, top);
                break;
              }

            case op_call:
              {
                int          arg_count = arg1;
                XSSArguments args      = get_stack_arguments(arg_count, true);
                XSSValue     value     = variant_cast<XSSValue>(pop(), XSSValue()); assert(value);

                value_operation& op = value->get_last();
                op.set_operation(OP_CALL);
                op.set_arguments(args);

                push(value, top);
                break;
              }

            case op_parameter:
              {
                expression_identifier ei = arg1; 
                param_names_.push_back(ei.value); //td: this is only added for named parameters, wrong all the way to the expression compiler
                break;
              }

            case op_array:
              {
						    int           arg_count = arg1;
                XSSArguments  args      = get_stack_arguments(arg_count, true);

                XSSValue result(new xss_value);
                value_operation op(OP_ARRAY, str()); 
                op.set_arguments(args);

                result->add_operation(op);
                push(result, top);
						    break;
              }

            case op_index:
              {
						    int           arg_count = arg1;
                XSSArguments  args      = get_stack_arguments(arg_count, false);
                XSSValue      value     = get_value(pop()); assert(value);

                value_operation& op = value->get_last();
                op.set_operation(OP_INDEX);
                op.set_arguments(args);
                push(value, top);
                break;
              }

				    default:
              assert(false); //td:
          }
      }

    XSSExpression get()
      {
        assert(stack_.size() == 1);
        return get_expression(stack_.top());    
      }

    private:
      std::stack<variant> stack_;
      std::vector<str>    param_names_;

      variant pop()
        {
          variant result = stack_.top();
          stack_.pop();
          return result;
        }

      XSSExpression get_expression(variant value)
        {
          if (value.is<XSSExpression>())
            return (XSSExpression)value;
          else 
            {
              XSSValue val = get_value(value);
              return XSSExpression(new xss_expression(val));
            }
        }

      XSSValue get_value(variant value)
        {
          if (value.is<XSSValue>())
            return (XSSValue)value;
          else if (value.is<expression_identifier>())
            {
              expression_identifier ei = value;
              XSSValue result(new xss_value);
              value_operation op(OP_READ, ei.value); 
              result->add_operation(op);
              return result;
            }
          else 
            {
              //out to be a constant
              XSSValue result(new xss_value);
              value_operation op(OP_CONSTANT, str()); 
              op.set_constant(value);
              result->add_operation(op);
              return result;
            }
        }
      
      XSSArguments get_stack_arguments(int count, bool invert)
        {
          XSSArguments result = XSSArguments(new xss_arguments);
          for(int i = 0; i < count; i++)
            {
              XSSExpression expr = get_expression(stack_.top());
              stack_.pop();

              str param_name;
              if (param_names_.size() == count)
                param_name = param_names_[i];

              if (invert)
                result->push_front(param_name, expr);
              else
                result->add(param_name, expr);
            }

          return result;
        }
  };

//xss_parameter
XSSExpression xss_parameter::value() 
  {
    return value_;
  }

str xss_parameter::name()  
  {
    return name_;
  }

XSSType xss_parameter::type()  
  {
    return value_? value_->type() : XSSType();
  }

//xss_arguments
void xss_arguments::bind(XSSContext ctx)
  {
    xss_parameters::iterator it = args_.begin();
    xss_parameters::iterator nd = args_.end();

    for(; it != nd; it++)
      it->value()->bind(ctx);
  }

XSSType xss_arguments::type()
  {
    XSSType result;
    xss_parameters::iterator it = args_.begin();
    xss_parameters::iterator nd = args_.end();

    for(; it != nd; it++)
      {
        XSSType curr = it->value()->type();
        if (!result)
          {
            result = curr;
            if (!result)
              return result;
          }
        else if (curr != result)
          return XSSType();
      }

    return result;
  }

void xss_arguments::add(const str& name, XSSExpression value)
  {
    args_.push_back(xss_parameter(name, value));
  }

void xss_arguments::push_front(const str& name, XSSExpression value)
  {
    args_.insert(args_.begin(), xss_parameter(name, value));
  }

size_t xss_arguments::size()
  {
    return args_.size();
  }

xss_parameter& xss_arguments::get(size_t idx)
  {
    return args_[idx];
  }

xss_parameters::iterator xss_arguments::begin()
  {
    return args_.begin();
  }

xss_parameters::iterator xss_arguments::end()
  {
    return args_.end();
  }

//value_operation
value_operation::value_operation(VALUE_OPERATION op, const str& identifier):
  op_(op),
  identifier_(identifier),
  resolve_what_(RESOLVE_ANY)
  {
  }

VALUE_OPERATION value_operation::id()
  {
    return op_;
  }

variant value_operation::constant()
  {
    return constant_;
  }

void value_operation::bind(RESOLVE_ITEM what, variant value)
  {
    resolve_what_  = what;
    resolve_value_ = value;
  }

XSSArguments value_operation::args()
  {
    return args_;
  }

str value_operation::identifier()
  {
    return identifier_;
  }

void value_operation::set_operation(VALUE_OPERATION op)
  {
    op_ = op;
  }

void value_operation::set_arguments(XSSArguments args)
  {
      args_ = args;
  }

void value_operation::set_type_arguments(XSSArguments type_args)
  {
    type_args_ = type_args;
  }

void value_operation::set_constant(variant constant)
  {
    constant_ = constant;
  }

//xss_value
void xss_value::bind(XSSContext ctx)
  {
    value_operations::iterator it = operations_.begin();
    value_operations::iterator nd = operations_.end();

    bool         first = true;
    resolve_info left;
    XSSType      current;
    for(; it != nd; it++)
      {
        resolve_info  ri;
        resolve_info& resolver = left;
        if (first)
          resolver = ri;
        else
          resolver.left = &left;

        switch(it->id())
          {
            case OP_CONSTANT:
              {
                assert(first);
                assert(operations_.size() == 1);
                current = ctx->get_type(it->constant().get_schema());
                it->bind(RESOLVE_CONST, current);
                break;
              };
            case OP_INSTANTIATION:
              {
                assert(first);
                assert(operations_.size() == 1);
                
                XSSArguments args = it->args();
                args->bind(ctx);

                current = ctx->get_type(it->identifier());
                it->bind(RESOLVE_INSTANCE, current);
                return;
              };
            case OP_OBJECT:
              {
                assert(first);
                assert(operations_.size() == 1);
                
                XSSArguments args = it->args();
                args->bind(ctx);

                current = ctx->get_type("var"); //td: 0.9.5 instance_type(args)
                it->bind(RESOLVE_INSTANCE, current);
                return;
              };
            case OP_ARRAY:
              {
                assert(first);
                assert(operations_.size() == 1);
                
                XSSArguments args = it->args();
                args->bind(ctx);

                current = ctx->get_array_type(args->type()); 
                it->bind(RESOLVE_INSTANCE, current);
                return;
              };

            case OP_READ:
              {
                if (ctx->resolve(it->identifier(), resolver))
                  {
                    current = resolver.type;
                    it->bind(resolver.what, resolver.value);
                    left = resolver;
                  }
                else
                  {
                    //td: 0.9.5 throw??
                  }
                break;
              }
            case OP_CALL:
              {
                if (ctx->resolve(it->identifier(), resolver))
                  {
                    if (resolver.what == RESOLVE_METHOD)
                      {
                        it->args()->bind(ctx);

                        XSSMethod mthd = resolver.value; //td: 0.9.5 check signature
                        current = mthd->type();
                        

                        it->bind(RESOLVE_METHOD, mthd);
                        left.what  = RESOLVE_TYPE;
                        left.type  = current;
                      }
                    else
                      {
                        //td: 0.9.5 throw??
                      }
                  }
                else
                  {
                    //td: 0.9.5 throw??
                  }
                break;
              }
            case OP_INDEX:
              {
                if (ctx->resolve(it->identifier(), resolver))
                  {
                    XSSArguments args = it->args();
                    args->bind(ctx);
                    
                    XSSOperator op = resolver.type->get_operator(op_index, args);
                    if (op)
                      {
                        current = op->type();
                        it->bind(RESOLVE_TYPE, current);
                        left.what  = RESOLVE_TYPE;
                        left.type  = current;
                      }
                    else
                      {
                        //td: 0.9.5 throw??
                      }
                  }
                else
                  {
                    //td: 0.9.5 throw??
                  }
                break;
              }
          }

        first = false;
      }

      type_ = current; assert(type_);
  }

XSSType xss_value::type()
  {
    return type_;
  }

void xss_value::add_operation(value_operation& op)
  {
    operations_.push_back(op);
  }

value_operation& xss_value::get_last()
  {
    return *(operations_.end() - 1);
  }

//xss_expression
xss_expression::xss_expression(operator_type op, XSSExpression arg1, XSSExpression arg2, XSSExpression arg3):
  op_(op),  
  arg1_(arg1), 
  arg2_(arg2), 
  arg3_(arg3) 
  {
  }

xss_expression::xss_expression(XSSValue value):
  op_(op_none),
  value_(value)
  {
  }

void xss_expression::bind(XSSContext ctx)
  {
    assert(!type_); //already bound

    if (value_)
      {
        value_->bind(ctx);
        type_ = value_->type();
      }
    else
      {
        arg1_->bind(ctx);
        if (arg2_)
          {
            arg2_->bind(ctx);
            type_ = ctx->get_operator_type(op_, arg1_->type(), arg2_->type());
          }
        else
          type_ = ctx->get_operator_type(op_, arg1_->type(), XSSType());
      }
  }

XSSType xss_expression::type()
  {
    return type_;
  }

//xss_signature
bool xss_signature::match(XSSArguments args)
  {
    //td: just straight matching as of now
    signature_items::iterator it = items_.begin();
    signature_items::iterator nd = items_.end();

    xss_parameters::iterator pit = args->begin();
    xss_parameters::iterator pnd = args->end();

    for(; it != nd; it++, pit++)
      {
        if (pit == pnd)
          return false;

        if (it->type != pit->type())
          return false;
      }

    return pit == pnd;
  }

//xss_operator
xss_operator::xss_operator(operator_type op, XSSType result):
  op_(op),
  result_(result)
  {
  }

xss_operator::xss_operator(operator_type op, XSSType result, XSSType right):
  op_(op),
  result_(result),
  right_(right)
  {
  }

xss_operator::xss_operator(operator_type op, XSSType result, XSSType left, XSSType right):
  op_(op),
  result_(result),
  left_(left),
  right_(right)
  {
  }

xss_operator::xss_operator(operator_type op, XSSType result, XSSSignature signature):
  op_(op),
  result_(result),
  signature_(signature) 
  {
  }

XSSType xss_operator::type()
  {
    return result_;
  }

operator_type xss_operator::opid()
  {
    return op_;
  }

bool xss_operator::match(XSSArguments args)
  {
    if (signature_)
      return signature_->match(args);

    if (args->size() == 1)
      {
        xss_parameter& arg = args->get(0);
        XSSExpression value = arg.value();
        if (!value)
          return false;
        return match(value->type());
      }
    return false;
  }

bool xss_operator::match(XSSType type)
  {
    if (right_ && right_ == type)
      return true;

    if (left_ && left_ == type)
      return true;

    return false;
  }

//xss_expression_utils
XSSExpression xss_expression_utils::compile_expression(expression& expr)
  {
    expression_builder eb;
    expr.visit(&eb);
    return eb.get();    
  }  


