
#include <xs/code.h>
#include <stack>

using namespace xkp;

//code_context
void code_context::push_operand(variant op)
  {
    opnds_.push_back( op );
  }

void code_context::push_operator(operator_type op)
  {
    get_ops().push_back( op );
  }
  
void code_context::push_identifier(std::string id)
  {
    get_operands().push_back( id );
  }
  
void code_context::push_instruction(intermediate_instruction ii)
  {
    //td: turn operator list into variants
  }
  
void code_context::push_var(std::string vn)
  {
    //td: turn operator list into variants
  }
  
void code_context::push_var_type(std::vector<std::string> vt)
  {
    //td: turn operator list into variants
  }
  
void code_context::try_rule()
  {
    assert( !trying_ );
    trying_ = true;
  }
  
void code_context::accept_rule()
  {
    trying_ = false;

    ops_.insert(ops_.end(), tryops_.begin(), tryops_.end());
    opnds_.insert(opnds_.end(), tryopnds_.begin(), tryopnds_.end());
  }
  
void code_context::reject_rule()
  {
    trying_ = false;
    tryops_.clear();
    tryopnds_.clear();
  }
  
  
variant code_context::evaluate_expression()
  {
    //this should be a singleton?
    operator_registry operators;
  
    //make a copy of the current values
    std::stack<variant> stack;
    operand_list::iterator oit = opnds_.begin();
    operand_list::iterator ond = opnds_.end();
    for(; oit != ond; oit++)
      stack.push( *oit );
    
    instruction_list::iterator it = ops_.begin();
    instruction_list::iterator nd = ops_.end();
    for(; it != nd; it++)
      {
        operator_type op = *it; //note this will throw if there is some other instruction type
                                //making this function only usable for plain old parsed expressions.
                                
        int ops = operand_count[op];
        variant opnd1, opnd2;
        switch(ops)
          {
            case 2: 
              {
                opnd2 = stack.top(); stack.pop();
                opnd1 = stack.top(); stack.pop();
                break;
              }
            case 1:
              {
                opnd1 = stack.top(); stack.pop();
                break;
              }
            default: assert( false );
          }
        
        operator_exec* exec = operators.get_operator(op, opnd1.s, opnd2.s);
        assert(exec);
        
        variant res = exec->exec(opnd1, opnd2);
        stack.push(res);
      }
      
    if (stack.empty())
      return variant();
      
    return stack.top();
  }

code_context::instruction_list& code_context::get_ops()
  {
    if (trying_)
      return tryops_;
      
    return ops_;
  }
  
code_context::operand_list& code_context::get_operands()
  {
    if (trying_)
      return tryopnds_;
      
    return opnds_;
  }
  
