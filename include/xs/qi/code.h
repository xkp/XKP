
#ifndef XS_CODE_H
#define XS_CODE_H

#include <base.h>
#include "operators.h"

namespace xkp
{
  enum intermediate_instruction
    {
      ii_vardecl,
      ii_varinit,
      ii_delegate,
      ii_for_cond,
      ii_for_begin,
      ii_for_action,
      ii_for_actionT,
      ii_for_end,
      ii_iterfor_begin,
      ii_iterfor_value,
      ii_if,
      ii_else,
      ii_endif,
      ii_while_begin,
      ii_while_value,
      ii_endwhile,
      ii_switch,
      ii_case_begin,
      ii_case,
      ii_case_end,
      ii_default_begin,
      ii_default_end,
      ii_switch_end,
      ii_return,
      ii_dispatch_begin,
      ii_dispatch,
    };

  struct code_context
    {
      code_context(): trying_(false) {}
    
      void push_operand(variant op);
      void push_operator(operator_type op);
      void push_identifier(std::string id);
      void push_instruction(intermediate_instruction ii);
      void push_var(std::string vn);
      void push_var_type(std::vector<std::string> vt);
      
      void try_rule();
      void accept_rule();
      void reject_rule();
      
      //utils
      variant evaluate_expression();

      private:
        typedef std::vector<variant> instruction_list;
        typedef std::vector<variant> operand_list;

        instruction_list ops_;
        operand_list  opnds_;
        
        bool trying_;
        instruction_list tryops_;
        operand_list  tryopnds_;
        
        instruction_list& get_ops();
        operand_list&  get_operands();
    };
}

#endif