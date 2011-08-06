
#ifndef XS_OPERATORS_H
#define XS_OPERATORS_H

#include <base.h>
#include <memory>

namespace xkp
{
  enum operator_type
    {
      op_inc,               //0
      op_dec,               //1
      op_ref,               //2  
      op_unary_plus,        //3
      op_unary_minus,       //4
      op_not,               //5
      op_mult,              //6
      op_divide,            //7  
      op_mod,               //8  
      op_typecast,          //9
      op_typecheck,         //10
      op_namecheck,         //11
      op_plus,              //12
      op_minus,             //13
      op_shift_right,       //14
      op_shift_left,        //15
      op_shift_right_equal, //16
      op_shift_left_equal,  //17
      op_equal,             //18
      op_notequal,          //19  
      op_gt,                //20
      op_lt,                //21
      op_ge,                //22
      op_le,                //23  
      op_and,               //24
      op_or,                //25
      op_assign,            //26
      op_plus_equal,        //27
      op_minus_equal,       //28
      op_mult_equal,        //29
      op_div_equal,         //30
      op_dot,               //31
      op_dot_call,          //32
      op_index,             //33
      op_call,              //34  
      op_func_call,         //35  
      op_array,             //36
      op_parameter,         //37 
    };    
    
  struct operator_exec
    {
      virtual variant exec(variant& arg1, variant& arg2) = 0;
    };
     
  class operator_registry
    {
      public:
        operator_registry();
      public:
        size_t         register_operator(operator_type op, schema* t1, schema* t2, operator_exec* exec);
        size_t         register_default_operator(operator_type op, operator_exec* exec);
        size_t         register_wildcard(operator_type op, schema* t1, schema* t2, operator_exec* exec ); 
        operator_exec* get_operator(operator_type op, schema* t1, schema* t2);
        bool           get_operator_index(operator_type op, schema* t1, schema* t2, size_t& result, schema** result_type);
        bool           get_default_operator(operator_type op, operator_exec** exec, schema** result_type);
        operator_exec* get_operator(size_t idx);
        schema*        get_result_type(schema* t1, schema* t2);
      private:
        typedef std::vector<operator_exec*>	executor_list;
        typedef std::vector<schema*>				type_rank;
        
        //dynamic search struct, probably not too fast        
        struct key
          {
            operator_type op;
            schema*       arg1;
            schema*       arg2;
            
            key(operator_type _op, schema* _arg1, schema* _arg2) : 
              op(_op),
              arg1(_arg1),
              arg2(_arg2)
              {
              }
              
            bool operator < (const key& k) const
              {
                if (k.op < op)          return false;
                else if (k.op > op)     return true;
                  
                if (k.arg1 < arg1)      return false;
                else if (k.arg1 > arg1) return true;

                if (k.arg2 < arg2)      return false;
                else if (k.arg2 > arg2) return true;

                return false;
              }
          };
          
        typedef std::map<key, size_t>											dynamic_executor_list;
        typedef std::pair<key, size_t>										dynamic_executor_pair;
        typedef std::map<operator_type, operator_exec*>		default_executor_list;
        typedef std::pair<operator_type, operator_exec*>	default_executor_pair;
        
        executor_list         executors_;
        dynamic_executor_list dynamic_executors_;
        dynamic_executor_list wildcard_executors_;
				default_executor_list	default_ops_;

        type_rank             rank_;
    };
}

#endif