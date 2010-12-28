
#ifndef XS_VM_H
#define XS_VM_H

#include <base.h>
//#include <xs.h>
#include <xs/operators.h>

#include <map>
#include <stack>

namespace xkp
{
  enum instruction_type
    {
      i_jump,                     //0
      i_jump_if,                  //1  
      i_jump_if_not,              //2
      i_store,                    //3
      i_load,                     //4
      i_load_constant,            //5
      i_return,                   //6
      i_return_value,             //7  
      i_dup_top,                  //8 
      i_binary_operator,          //9
      i_unary_operator,           //10 
      i_load_this,                //11  
      i_get,                      //12  
      i_set,                      //13
      i_call,                     //14
      i_instantiate,              //15
      i_this_call,                //16
      i_dispatch,                 //17  
      i_pop,                      //18  
      i_dynamic_binary_operator,  //19
      i_dynamic_unary_operator,   //20   
      i_dynamic_get,              //21   
      i_dynamic_resolve,          //22
			i_dynamic_set,							//23
			i_nop,											//24
    };  

  struct instruction
    {
      instruction(const instruction& other):
        id(other.id),
        data(other.data)
        {
        }

      instruction(instruction_type _id):
        id(_id)
        {
          data.value = 0;  
        }
        
      instruction(instruction_type _id, short _data): 
        id(_id) 
        {
          data.value = _data;    
        }
        
      instruction(instruction_type _id, unsigned char param_count, bool is_dynamic): 
        id(_id)
        {
          data.call_data.is_dynamic  = is_dynamic;
          data.call_data.param_count = param_count;
        }
    
      union instruction_data
        {
          short value;
          
          struct __call_data
            {
              bool     is_dynamic  : 8;
              unsigned param_count : 8;
            }call_data;
        };

      short            id;
      instruction_data data;
    };
    
  typedef std::vector<instruction> instruction_list;
  typedef std::vector<variant>     constant_list;
    
  struct byte_code
    {
      byte_code() {}

      byte_code( instruction_list& _instructions, constant_list& _constants):
        instructions(_instructions),
        constants(_constants)
        {
        }
        
      instruction_list instructions;
      constant_list    constants;
    };
    
  typedef reference<byte_code> ByteCode;
    
  struct execution_context
    {
      execution_context(ByteCode code, variant _this = variant(), param_list* args = null);
    
      variant execute();
      private:
        typedef std::vector<variant> frame_stack;
        typedef std::stack<variant>  operand_stack;
        
        ByteCode          code_; 
        instruction_list& instructions_;
        constant_list&    constants_;
        size_t            pc_;
        bool              jump_;
        frame_stack       stack_;
        operand_stack     operands_;
        operator_registry operators_; //td: this is way too expensive
        variant           this_;
        
        void    push(variant v);
        variant pop();
    };
    
  struct code_setter : setter   
    { 
      code_setter(ByteCode _code, int var_idx = -1);
      virtual void set( void* instance, const variant value );

      ByteCode code_;
      int      idx_; 
    };
  
  struct code_getter : getter   
    { 
      code_getter(ByteCode _code);
      virtual variant get(void* instance); 
      
      ByteCode code_;
    };

  struct code_executer: executer 
    { 
      code_executer(ByteCode _code);
      virtual variant exec( void* instance, const param_list args ); 
      
      ByteCode code_;
    };
    
  struct const_getter : getter   
    { 
      const_getter(variant value) : value_(value) {}
      virtual variant get(void* instance)         {return value_;}  
      
      variant value_;
    };

}

#endif