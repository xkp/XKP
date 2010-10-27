
#include <xs/ast.h>

using namespace xkp;

const int operand_count[] = 
  {
    1, //op_inc,
    1, //op_dec,
    1, //op_ref,
    1, //op_unary_plus,
    1, //op_unary_minus,
    1, //op_not,
    2, //op_mult,
    2, //op_divide,
    2, //op_mod,
    2, //op_typecast,
    2, //op_typecheck,
    2, //op_namecheck,
    2, //op_plus,
    2, //op_minus,
    2, //op_shift_right,
    2, //op_shift_left,
    2, //op_shift_right_equal,
    2, //op_shift_left_equal,
    2, //op_equal,
    2, //op_notequal,
    2, //op_gt,
    2, //op_lt,
    2, //op_ge,
    2, //op_le,
    2, //op_and,
    2, //op_or,
    2, //op_assign,
    2, //op_plus_equal,
    2, //op_minus_equal,
    2, //op_mult_equal,
    2, //op_div_equal,
    2, //op_dot,
    2, //op_dot_call
    2, //op_index,
    1, //op_call,
    2, //op_func_call
    1, //op_array,
    0, //op_parameter
  };

//expression
void expression::push_operator(operator_type op)
  {
    if (op == op_call)
      {
        //this is sort of tricky, basically an op_dot followed by an op_call
        //will turn that dot into a dot_call, this way the called knows 
        //wheter is a property access or a method call.
        assert(stack_.size() > 1);
        
        variant last_op = stack_[stack_.size() - 2];
        if (last_op.is<operator_type>())
          {
            operator_type ot = last_op;
            if (ot == op_dot)
              {
                variant arg_count = stack_[stack_.size() - 1];
                stack_.erase(stack_.end() - 2, stack_.end());
                
                stack_.push_back(op_dot_call);
                stack_.push_back(arg_count);
              }
          }
      }

    stack_.push_back( op );
  }
  
void expression::push_operand(variant op)
  {
    stack_.push_back( op );
  }
  
void expression::push_identifier(const str& id)
  {
    stack_.push_back( expression_identifier(id) );
  }
  
bool expression::empty()
  {
    return stack_.empty();
  }
  
void expression::visit(expression_visitor* v)
  {
    expr_stack::iterator it = stack_.begin();
    expr_stack::iterator nd = stack_.end();
    for(; it != nd; it++)
      {
        if (it->is<operator_type>())
          {
            operator_type op = *it;
            v->exec_operator(op, operand_count[op], 1 );
          }
        else 
            v->push(*it);
      }
  }

bool expression::is_constant(variant& value)
  {
    if (stack_.size() == 1)
      {
        value = stack_[0];
        return true;
      }
    return false;
  }

//expr_evaluator
void expr_evaluator::push(variant operand)
  {
    stack_.push(operand);
  }
  
void expr_evaluator::exec_operator(operator_type op, int pop_count, int push_count )
  {
    variant arg1, arg2;
    switch(pop_count)
      {
        case 1: arg1 = stack_.top(); stack_.pop(); break;
        case 2: 
          {
            arg2 = stack_.top(); stack_.pop(); 
            arg1 = stack_.top(); stack_.pop(); 
            break;
          }
        default: assert(false);
      }
    
    operator_exec* exec = operators_.get_operator(op, arg1.get_schema(), arg2.get_schema()); assert(exec);
    variant result = exec->exec(arg1, arg2);
    
    if (push_count > 0)
      push( result );
  }
  
variant expr_evaluator::value()
  {
    if (stack_.empty())
      return variant();
      
    return stack_.top();
  }

//code
void code::add_statement(variant st)
  {
    statements_.push_back( st );
  }
  
void code::visit(code_visitor* visitor)
  {
    statement_list::iterator it = statements_.begin();
    statement_list::iterator nd = statements_.end();
    
    for(; it != nd; it++)
      {
        if (it->is<stmt_if>())
          {
            stmt_if _if = *it;
            visitor->if_(_if);
          }
        else if (it->is<stmt_variable>())
          {
            stmt_variable var = *it;
            visitor->variable_(var);
          }
        else if (it->is<stmt_for>())
          {
            stmt_for _for = *it;
            visitor->for_(_for);
          }
        else if (it->is<stmt_iter_for>())
          {
            stmt_iter_for _for = *it;
            visitor->iterfor_(_for);
          }
        else if (it->is<stmt_while>())
          {
            stmt_while _while = *it;
            visitor->while_(_while);
          }
        else if (it->is<stmt_break>())
          {
            visitor->break_();
          }
        else if (it->is<stmt_continue>())
          {
            visitor->continue_();
          }
        else if (it->is<stmt_return>())
          {
            stmt_return _return = *it;
            visitor->return_(_return);
          }
        else if (it->is<stmt_expression>())
          {
            stmt_expression expr = *it;
            visitor->expression_(expr);
          }
        else if (it->is<dsl>())
          {
            dsl d = *it;
            visitor->dsl_(d);
          }
        else if (it->is<stmt_dispatch>())
          {
            stmt_dispatch d = *it;
            visitor->dispatch(d);
          }
        else
          {
            assert(false); 
          }
      }
  }

bool code::empty()
  {
    return statements_.empty();
  }

//xs_container
void xs_container::visit(xs_visitor* visitor)
  {
    xs_items::iterator it = items_.begin();
    xs_items::iterator nd = items_.end();
    
    for(; it != nd; it++)
      {
        variant& curr = *it;
        if (curr.is<xs_property>())
          {
            xs_property info = curr;
            visitor->property_(info);
          }
        else if (curr.is<xs_method>())
          {
            xs_method info = curr;
            visitor->method_(info);
          }
        else if (curr.is<xs_event>())
          {
            xs_event info = curr;
            visitor->event_(info);
          }
        else if (curr.is<xs_event_decl>())
          {
            xs_event_decl info = curr;
            visitor->event_decl_(info);
          }
        else if (curr.is<xs_const>())
          {
            xs_event_decl info = curr;
            visitor->event_decl_(info);
          }
        else if (curr.is<xs_instance>())
          {
            xs_instance info = curr;
            visitor->instance_(info);
          }
        else if (curr.is<xs_class>())
          {
            xs_class info = curr;
            visitor->class_(info);
          }
        else if (curr.is<dsl>())
          {
            dsl info = curr;
            visitor->dsl_(info);
          }
        else if (curr.is<xs_behaviour>())
          {
            xs_behaviour info = curr;
            visitor->behaviour_(info);
          }
        else if (curr.is<xs_implement_behaviour>())
          {
            xs_implement_behaviour info = curr;
            visitor->behaveas_(info);
          }
        else assert(false);
      }
  }
  
void xs_container::add(variant item)
  {
    items_.push_back(item);
  }

size_t xs_container::size()
  {
    return items_.size();
  }
  
variant xs_container::get(size_t idx)
  {
    return items_[idx];
  }
  
