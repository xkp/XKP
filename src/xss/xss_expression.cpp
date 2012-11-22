#include "xss/xss_expression.h"
#include "xss/language.h"

using namespace xkp;

const str SExpression("xss-expression");

const str SAssignOperatorOnlyTop("Assign operators can only be used as the base of an expression");
const str SInvalidAssign("Invalid assignment");
const str SCannotResolve("Cannot resolve");
const str SExpectingProperty("Assignment expects a property or variable");
const str SExpectingMethod("A call expects a method");
const str SCannotResolveArrayOperator("This entity does not support the bracket operator");
const str SCannotResolveExpressionType("Could not resolve the type of this expression");
const str SCannotAssigningNonValue("Invalid assignment");
const str STypeMismatch("type mismatch");

//stuff
const int operator_precedence[] =
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
    1,  //"new",  //op_instantiate
    1,  //"{}",   //op_object
  };

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
    "",     //op_instantiate
    "",     //op_object
  };

struct expression_builder : expression_visitor
  {
    expression_builder(file_position& begin, file_position& end):
      begin_(begin), 
      end_(end)
      {
      }

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

        int op_prec = operator_precedence[op];
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
                this_expr->set_extents(begin_, end_);

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
                this_expr->set_extents(begin_, end_);

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
                
                XSSValue result(new xss_value(begin_, end_));
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

                XSSValue result(new xss_value(begin_, end_));

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

                XSSValue result(new xss_value(begin_, end_));
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

                XSSValue result(new xss_value(begin_, end_));
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
              XSSExpression result(new xss_expression(val));
              result->set_extents(begin_, end_);
              
              return result;
            }
        }

      XSSValue get_value(variant value)
        {
          if (value.is<XSSValue>())
            return (XSSValue)value;
          else if (value.is<expression_identifier>())
            {
              expression_identifier ei = value;
              XSSValue result(new xss_value(begin_, end_));
              value_operation op(OP_READ, ei.value); 
              result->add_operation(op);
              return result;
            }
          else 
            {
              //out to be a constant
              XSSValue result(new xss_value(begin_, end_));
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
    private:
      file_position begin_;
      file_position end_;
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
    if (type_)
      return type_;

    xss_parameters::iterator it = args_.begin();
    xss_parameters::iterator nd = args_.end();

    for(; it != nd; it++)
      {
        XSSType curr = it->value()->type();
        if (!type_)
          {
            type_ = curr;
            if (!type_)
              return type_; //no type can be resolved
          }
        else if (curr != type_)
          {
            type_.reset(); //td: handle this better
            break;
          }
      }

    return type_;
  }

void xss_arguments::type(XSSType t)
  {
    type_ = t;
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

void value_operation::id(VALUE_OPERATION op)
  {
    op_ = op;
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

void value_operation::args(XSSArguments value)
  {
    args_ = value;
  }

str value_operation::identifier()
  {
    return identifier_;
  }

bool value_operation::is_constant()
  {
    return op_ == OP_CONSTANT;
  }

bool value_operation::bound()
  {
    return op_ == OP_CONSTANT || resolve_what_ != RESOLVE_ANY;
  }

void value_operation::set_operation(VALUE_OPERATION op)
  {
    op_ = op;
  }

RESOLVE_ITEM value_operation::resolve_id()
  {
    return resolve_what_;	   
  }

variant value_operation::resolve_value()
  {
	  return resolve_value_;	   
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
xss_value::xss_value():
  state_(BS_UNBOUND)
  {
  }

xss_value::xss_value(const xss_value& other):
  type_(other.type_),
  operations_(other.operations_),      
  begin_(other.begin_),
  end_(other.end_),
  state_(other.state_)
  {
  }

xss_value::xss_value(file_position& begin, file_position& end) :
  state_(BS_UNBOUND),
  begin_(begin),
  end_(end)
  {
  }

void xss_value::bind(XSSContext ctx, bool as_setter)
  {
    assert(state_ != BS_BOUND);

    value_operations::iterator it = operations_.begin();
    value_operations::iterator nd = operations_.end();

    bool         first = true;
    resolve_info left;
    XSSType      current;
    for(; it != nd; it++)
      {
        if ((state_ == BS_FIXUP || state_ == BS_ERROR) && it->bound())
          continue;
        
        resolve_info  resolver;
        if (!first)
		      resolver.left = &left;

        bool last = (it + 1) == nd;

        //handle assignments
        if (last && as_setter)
          {
            switch(it->id())
			        {
				        case OP_READ:
				          {
                    it->id(OP_WRITE);
                    break;
				          }
                case OP_INDEX:
                  {
                    break;
                  }
                default:
                  {
		                ctx->error(SInvalidAssign, null, begin_, end_);
                    state_ = BS_ERROR;
                    continue;
                  }
			        }
          }

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

                type_ = ctx->get_type(it->identifier());
                it->bind(RESOLVE_INSTANCE, type_);
                return;
              };
            case OP_OBJECT:
              {
                assert(first);
                assert(operations_.size() == 1);
                
                XSSArguments args = it->args();
                args->bind(ctx);

                type_ = ctx->get_type("var"); //td: 0.9.5 instance_type(args)
                it->bind(RESOLVE_INSTANCE, type_);
                return;
              };
            case OP_ARRAY:
              {
                assert(first);
                assert(operations_.size() == 1);
                
                XSSArguments args = it->args();
                args->bind(ctx);

				        XSSType arg_type = args->type();
				        if (arg_type)
					        type_ = ctx->get_array_type(arg_type); 
				        else if (args->size() == 0)
					        type_ = ctx->get_array_type(ctx->get_type("var"));
                it->bind(RESOLVE_CONST, type_);
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
                    state_ = BS_ERROR;
		                
                    param_list error;
                    error.add("identifier", it->identifier());
		                ctx->error(SCannotResolve, &error, begin_, end_);
                  }
                break;
              }
            case OP_WRITE:
              {
                if (ctx->resolve(it->identifier(), resolver))
                  {
                    current = resolver.type;
                    if (resolver.what != RESOLVE_PROPERTY &&
                        resolver.what != RESOLVE_VARIABLE)
                      {
                        param_list error;
                        error.add("identifier", it->identifier());
		                    ctx->error(SExpectingProperty, null, begin_, end_);
                        state_ = BS_ERROR;
                      }

                    it->bind(resolver.what, resolver.value);
                    left = resolver;
                  }
                else
                  {
                    param_list error;
                    error.add("identifier", it->identifier());
		                ctx->error(SCannotResolve, &error, begin_, end_);
                    state_ = BS_ERROR;
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
                        param_list error;
                        error.add("identifier", it->identifier());
		                    ctx->error(SExpectingMethod, &error, begin_, end_);
                        state_ = BS_ERROR;
                      }
                  }
                else
                  {
                    param_list error;
                    error.add("identifier", it->identifier());
		                ctx->error(SCannotResolve, &error, begin_, end_);
                    state_ = BS_ERROR;
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
					          else if (resolver.type->is_array())
					            {
                        current = resolver.type->array_type();
						            it->bind(resolver.what, resolver.value);
						            left.what  = RESOLVE_TYPE;
						            left.type  = current;
					            }
					          else 
                      {
                        param_list error;
                        error.add("identifier", it->identifier());
		                    ctx->error(SCannotResolveArrayOperator, &error, begin_, end_);
                        state_ = BS_ERROR;
                      }
                  }
                else
                  {
                    param_list error;
                    error.add("identifier", it->identifier());
		                ctx->error(SCannotResolve, &error, begin_, end_);
                    state_ = BS_ERROR;
                  }
                break;
              }
          }

        if (state_ == BS_ERROR)
          break;

        first = false;

        if (last)
          state_ = BS_BOUND;
      }

    if (state_ == BS_BOUND)
      {
        type_ = current; 
        
        if (!type_)
          {
            //td: !!! fix up
		        //ctx->error(SCannotResolveExpressionType, null, begin_, end_);
            
            state_ = BS_FIXUP;
            type_ = ctx->get_type("var");
          }
      }
  }

XSSType xss_value::type()
  {
    return type_;
  }

void xss_value::type(XSSType type)
  {
	  type_ = type;
  }

bool xss_value::bound()
  {
    return state_ == BS_BOUND;
  }

value_operations& xss_value::operations()
  {
    return operations_;
  }

void xss_value::add_operation(value_operation& op)
  {
    operations_.push_back(op);
  }

value_operation& xss_value::get_last()
  {
    return *(operations_.end() - 1);
  }

bool xss_value::is_constant()
  {
    value_operation& last = get_last();
    return last.is_constant();
  }

bool xss_value::is_array()
  {
	  if (operations_.size() != 1)
	    return false;
    
	  value_operation& last = get_last();
	  return last.id() == OP_ARRAY;
  }

variant xss_value::constant()
  {
    value_operation& last = get_last();
    return last.constant();
  }

XSSValue xss_value::path()
  { 
    if (operations_.size() <= 1)
      return XSSValue();

    XSSValue result(new xss_value(begin_, end_));
    result->state_ = state_;

    value_operations::iterator it = operations_.begin();      
    value_operations::iterator nd = operations_.end() - 1;      

    for(; it != nd; it++)
      {
        result->add_operation(*it);
      }

    return result;
  }

//xss_expression
xss_expression::xss_expression(operator_type op, XSSExpression arg1, XSSExpression arg2, XSSExpression arg3):
  op_(op),  
  arg1_(arg1), 
  arg2_(arg2), 
  arg3_(arg3),
  is_assign_(false)
  {
  }

xss_expression::xss_expression(XSSValue value):
  op_(op_none),
  value_(value),
  is_assign_(false)
  {
  }

void xss_expression::bind(XSSContext ctx)
  {
    assert(!type_); //already bound

    if (value_)
      {
        value_->bind(ctx, false);
        type_ = value_->type();
      }
    else
      {
        is_assign_ = xss_expression_utils::is_assignment(op_);
        if (op_ == op_assign)
          {
            XSSValue val = arg1_->value();
            if (val)
              val->bind(ctx, true);
            else
		          ctx->error(SCannotAssigningNonValue, null, begin_, end_);

            arg2_->bind(ctx);

            XSSExpression expr = XSSExpression(shared_from_this());
            notification ntfy(NOTID_ASSIGN, expr);
            ctx->notify(ntfy);
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
  }

XSSType xss_expression::type()
  {
    return type_;
  }

void xss_expression::type(XSSType t)
  { 
    type_ = t;
  }

bool xss_expression::is_constant()
  {
    if (!value_)
      return false;
    return value_->is_constant();
  }

variant xss_expression::constant_value()
  {
    if (!value_)
      return variant();
    return value_->constant();
  }

operator_type xss_expression::op()
  {
    return op_;
  }

void xss_expression::op(operator_type val)
  {
    op_ = val;
  }

bool xss_expression::is_assign()
  {
    return is_assign_;
  }

XSSExpression xss_expression::left()
  {
    return arg1_;
  }

void xss_expression::left(XSSExpression val)
  {
    arg1_ = val;
  }

XSSExpression xss_expression::right()
  {
    return arg2_;
  }

void xss_expression::right(XSSExpression val)
  {
    arg2_ = val;
  }

XSSExpression xss_expression::third()
  {
    return arg3_;
  }

void xss_expression::third(XSSExpression val)
  {
    arg3_ = val;
  }

XSSOperator xss_expression::xop()
  {
    return xop_;
  }

void xss_expression::xop(XSSOperator val)
  {
    xop_ = val;
  }

void xss_expression::as_array(XSSArguments items)
  {
    op_ = op_none;
    value_ = XSSValue(new xss_value(file_position(), file_position()));
    
    value_operation array_op(OP_ARRAY, str());
    array_op.args(items);

    value_->add_operation(array_op);
  }

XSSValue xss_expression::value()
  {
    return value_;
  }

void xss_expression::value(XSSValue val)
  {
    value_ = val;
  }

void xss_expression::set_extents(file_position& begin, file_position& end)
  {
    begin_ = begin;
    end_   = end;
  }

file_position& xss_expression::begin()
  {
    return begin_;
  }

file_position& xss_expression::end()
  {
    return end_;
  }

//xss_code
XSSContext xss_code::context()
  {
    return ctx_;
  }

void xss_code::add(XSSStatement st)
  {
    statements_.push_back(st);
  }

void xss_code::bind(XSSContext ctx)
  {
    ctx_->set_parent(ctx);
    ctx_->identity(CTXID_CODE, variant()); //td: !!! investigate when the code is needed by the context, this is increasing coupling

    statement_list::iterator it = statements_.begin();
    statement_list::iterator nd = statements_.end();

    for(; it != nd; it++)
      {
        XSSStatement st = *it;
        st->bind(ctx_);
      }
  }

XSSType xss_code::return_type()
  {
    return return_type_;
  }

void xss_code::set_extents(file_position& begin, file_position& end)
  {
    ctx_->set_extents(begin, end);
  }

file_position& xss_code::begin()
  {
    return ctx_->begin();
  }

file_position& xss_code::end()
  {
    return ctx_->end();
  }

statement_list& xss_code::statements()
  {
    return statements_;
  }

bool xss_code::empty()
  {
    return statements_.empty();
  }

//signature_item
signature_item::signature_item():
  cast_default(false)
  {
  }

signature_item::signature_item(const signature_item& other):
  name(other.name),
  type(other.type),
  type_name(other.type_name),
  default_value(other.default_value),
  cast_default(other.cast_default)
  {
  }

signature_item::signature_item(const str& _name, XSSType _type, XSSExpression _value):
  name(_name),
  type(_type),
  default_value(_value),
  cast_default(false)
  {
  }

signature_item::signature_item(const str& _name, const str& _type_name, XSSExpression _value):
  name(_name),
  type_name(_type_name),
  default_value(_value) 
  {
  }

//xss_signature
xss_signature::xss_signature()
  {
  }

xss_signature::xss_signature(const xss_signature& other):
  items_(other.items_)
  {
  }

signature_items& xss_signature::items()
  {
    return items_;
  }

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

bool xss_signature::match_signature(XSSSignature sig)
  {
    signature_items& other = sig->items();
    if (other.size() > items_.size())
      return false;

    signature_items::iterator it  = items_.begin();
    signature_items::iterator nd  = items_.end();
    signature_items::iterator oit = other.begin();
    signature_items::iterator ond = other.end();

    for(; it != nd; it++, oit++)
      {
        if (oit == ond)
          return true;

        if (it->name != oit->name)
          return false;

        if (it->type && it->type != oit->type)
          return false;

        if (!it->type && it->type_name != oit->type_name)
          return false;
      }

    return true;
  }

void xss_signature::add_argument(const str& name, XSSType type, XSSExpression default_value)
  {
    items_.push_back(signature_item(name, type, default_value));
  }

void xss_signature::add_argument(const str& name, const str& type_name, XSSExpression default_value)
  {
    items_.push_back(signature_item(name, type_name, default_value));
  }

void xss_signature::bind(XSSContext ctx)
  {
    signature_items::iterator it = items_.begin();
    signature_items::iterator nd = items_.end();

    for(; it != nd; it++)
      {
        it->type = ctx->assure_type(it->type_name);
        if (it->default_value)
          {
            it->default_value->bind(ctx);

            XSSType type1 = it->type;
            XSSType type2 = it->default_value->type();

            if (type1 && type2)
              {
                TYPE_MATCH tm;
                if (ctx->match_types(type1, type2, tm))
                  {
                    switch(tm)
                      {
                        case VARIANT:
                          {
                            it->type = type2;
                            break;
                          }
                        case TYPECAST:
                          {
                            it->cast_default = true;
                            break;
                          }
                      }
                  }
                else
                  {
	                  param_list pl;
	                  pl.add("type1", type1->id());
	                  pl.add("type2", type2->id());
	                  ctx->error(STypeMismatch, &pl, ctx->begin(), ctx->begin()); //td: location for parameters, etc
                  }
              }
          }
      }
  }

void xss_signature::arg_type(int idx, XSSType type)
  {
    items_[idx].type = type;
  }

void xss_signature::native(const str& s)
  {
    native_ = s;
  }

str xss_signature::native()
  {
    return native_;
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

InlineRenderer xss_operator::renderer()
  {
    return renderer_;
  }

//xss_expression_utils
XSSExpression xss_expression_utils::constant_expression(variant value)
  {
    expression expr;
    expr.push_operand(value);
    
    file_position b, e;
    expression_builder builder(b, e);
    expr.visit(&builder);
    return builder.get();    
  }

XSSExpression xss_expression_utils::compile_expression(expression& expr)
  {
    if (expr.empty())
      return XSSExpression();

    expression_builder eb(expr.begin, expr.end);
    expr.visit(&eb);
    return eb.get();    
  }  

XSSExpression xss_expression_utils::compile_expression(const str& expr)
  {
    xs_compiler cmp;
    expression  res; 
    
    if (!cmp.compile_expression(expr, res) )
      return XSSExpression(); 

    return compile_expression(res);
  }

bool xss_expression_utils::is_assignment(operator_type op)
  {
    switch(op)
      {
        case op_shift_right_equal:
        case op_shift_left_equal:
        case op_plus_equal:
        case op_minus_equal:
        case op_mult_equal:
        case op_div_equal:
        case op_assign:
          return true;
      }
    return false;
  }

//operator_utils
int operator_utils::precedence(operator_type op)
  {
    return operator_precedence[op];
  }

str operator_utils::to_string(operator_type op)
  {
    return str(operator_str[op]);
  }

