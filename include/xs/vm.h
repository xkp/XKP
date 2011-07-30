
#ifndef XS_VM_H
#define XS_VM_H

#include <base.h>
//#include <xs.h>
#include <xs/operators.h>

#include <map>
#include <stack>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

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
			i_dynamic_binary_assign_operator, //25
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
        
      instruction(instruction_type _id, unsigned char param_count, bool is_dynamic, bool invert): 
        id(_id)
        {
          data.call_data.is_dynamic  = is_dynamic;
					data.call_data.invert			 = invert;
          data.call_data.param_count = param_count;
        }
    
      instruction(instruction_type _id, bool is_dynamic, bool invert_set): 
        id(_id)
        {
          data.set_data.is_dynamic  = is_dynamic;
          data.set_data.invert			= invert_set;
        }

			union instruction_data
        {
          short value;
          
          struct __call_data
            {
              bool     is_dynamic  : 1;
              bool     invert			 : 1;
              unsigned param_count : 14;
            }call_data;

          struct __set_data
            {
              bool is_dynamic	: 8;
              bool invert			: 8;
            }set_data;
				};

      short            id;
      instruction_data data;
    };
    
  typedef std::vector<instruction> instruction_list;
  typedef std::vector<variant>     constant_list;
    
  struct byte_code
    {
      byte_code(fs::path _file):
        file(_file)
        {
        }

      byte_code( instruction_list& _instructions, constant_list& _constants, fs::path _file):
        instructions(_instructions),
        constants(_constants),
        file(_file)
        {
        }
        
      instruction_list instructions;
      constant_list    constants;
      fs::path         file; 
    };
    
  typedef reference<byte_code> ByteCode;
    
  struct execution_context
    {
      execution_context(ByteCode code, variant _this = variant(), param_list* args = null, fs::path file = fs::path());
    
      variant  execute();
      fs::path file();

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
        fs::path          file_;
        
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

  //singleton
  typedef std::stack<execution_context*> call_stack;
  
  struct vm
    {
      static vm& instance();
      
      public:
        void        enter(execution_context* ctx);
        void        leave();
        void        reset();
        call_stack& state();
        fs::path    file();
        void        error();
      private:
        call_stack call_stack_;
        bool       error_;
    };

  struct callstack_helper
      {
        callstack_helper(execution_context* ctx)
          {
            vm::instance().enter(ctx);
          }

        ~callstack_helper()
          {
            vm::instance().leave();
          }
      };
}

#endif