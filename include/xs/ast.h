
#ifndef XS_AST_H
#define XS_AST_H

#include <base.h>
#include <schema.h>
#include "xs/operators.h"

#include <vector>
#include <stack>

namespace xkp
{
  struct file_position
    {
      file_position() : 
        line(-1),
        column(0)
        {
        }

      file_position(int l, int c):
        line(l),
        column(c)
        {
        }

      file_position(const file_position& other):
        line(other.line),
        column(other.column)
        {
        }

      void min(file_position& other)
        {
          if (line < 0 || line > other.line)
            {
              line = other.line;
              column = other.column;
            }
          else if (line == other.line && column > other.column)
              column = other.column;
        }

      void max(file_position& other)
        {
          if (line < 0 || line < other.line)
            {
              line = other.line;
              column = other.column;
            }
          else if (line == other.line && column < other.column)
              column = other.column;
        }

      int line;
      int column;
    };

  struct expression_visitor
    {
      virtual void push(variant operand, bool top)																					= 0;
      virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top) = 0;
    };
    
  struct expression_identifier
    {
      expression_identifier()                                  {}
      expression_identifier(const str& _value) : value(_value) {}
      
      str value;
    };

  struct ast_base
    {
      file_position begin;
      file_position end;
    };

  struct expression : ast_base
    {
      public:
        void    push_operator(operator_type op);
        void    push_operand(variant op);
        void    push_identifier(const str& id);
        bool    empty();
        size_t  size();
        variant first();
      public:
        void visit(expression_visitor* v);
        bool is_constant(variant& value);
				bool top_operator(operator_type& op);
				variant pop_first();
        variant last();
        variant pop();
				void clear();
      private:
        typedef std::vector<variant> expr_stack;
        expr_stack                   stack_; 
    };
    
  //expression visitors
  struct expr_evaluator : expression_visitor
    {
      virtual void push(variant operand, bool top);
      virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top);
      
      variant value();
      
      protected: 
        typedef std::stack<variant> expr_stack;

        expr_stack        stack_;      
        operator_registry operators_;       
    };      
    
struct expression_splitter : expression_visitor
  {
		expression_splitter(operator_type divider);

		//expression_visitor
    virtual void push(variant operand, bool top);
		virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top);

		//output
		expression left;
		expression right;
		
		private:
			operator_type divider_;

      std::vector<variant> result_;
      std::stack<int>      positions_; 
	};

  //helpers
  struct xs_const
    {
      str        name;
      expression value;
    };
    
  struct xs_type
    {
      str                   name;
      std::vector<xs_const> args;
    };

  //code ast representation, the desicion is to simply
  //store variants for each statement and translate on visit.
  struct code_visitor;
  
  struct code : ast_base
    {
      public:
        void     add_statement(variant st);
        void     visit(code_visitor* visitor);
        bool     empty();
        variant& get_stament(int idx);
        void     cut(int idx, code& result);
        void     range(int from, int to, code& result);
        size_t   size();
      private:
        typedef std::vector<variant> statement_list;

        statement_list statements_;
    };
    
  //statements
  struct stmt_if : ast_base
    {
      expression expr;
      code       if_code;
      code       else_code;
    };
    
  struct stmt_variable : ast_base
    {
      str        type;
      str        id;
      expression value;
      
      bool empty() {return type.empty();}
    };

  struct stmt_for : ast_base
    {
      stmt_variable init_variable;
      expression    init_expr;
      expression    cond_expr;
      expression    iter_expr;
      code          for_code;
    };
    
  struct stmt_iter_for : ast_base
    {
      xs_type    type;
      str        id;
      expression iter_expr;
      code       for_code;
    };
  
  struct stmt_while : ast_base
    {
      expression expr;
      code       while_code;
    };

  struct stmt_dispatch : ast_base
    {
      std::vector<str> target;
      expression       args;
      int              arg_count;
    };

  struct stmt_break : ast_base     {};
  struct stmt_continue : ast_base  {};

  struct stmt_return : ast_base
    {
      expression expr;
    };

  struct stmt_expression : ast_base
    {
      expression expr;
    };
    
  struct switch_section : ast_base
    {
      std::vector<expression> cases;
      code                    case_code;
    };

  struct stmt_switch : ast_base
    {
      expression                  expr;
      std::vector<switch_section> sections;
      code                        default_code;
    };

  struct catch_ : ast_base
    {
      xs_type type;
      str     id;
      code    catch_code;
    };

  struct stmt_try : ast_base
    {
      code                try_code;
      std::vector<catch_> catches;
      code                finally_code;
    };

  struct stmt_throw : ast_base
    {
      expression expr;
    };

  struct dsl : ast_base
    {
      str        name;
      str        id;
      expression param_expr;
      int        param_count;
      param_list params;
      str        text;
    };
    
  struct code_visitor
    {
      virtual void if_(stmt_if& info)                 = 0;
      virtual void variable_(stmt_variable& info)     = 0;
      virtual void for_(stmt_for& info)               = 0;
      virtual void iterfor_(stmt_iter_for& info)      = 0;
      virtual void while_(stmt_while& info)           = 0;
      virtual void break_(stmt_break& info)           = 0;
      virtual void continue_(stmt_continue& info)     = 0;
      virtual void return_(stmt_return& info)         = 0;
      virtual void expression_(stmt_expression& info) = 0;
      virtual void dsl_(dsl& info)                    = 0;
      virtual void dispatch(stmt_dispatch& info)      = 0;
      virtual void switch_(stmt_switch& info)         = 0;
      virtual void try_(stmt_try& info)               = 0;
      virtual void throw_(stmt_throw& info)           = 0;
    };    

  //forwards
  struct xs_visitor;

  struct param_decl
    {
      str        name;
      str        type;
      expression default_value;
    };
    
  typedef std::vector<param_decl> param_list_decl;
  
  enum xs_adjetive
    {
      adj_public    = 1,
      adj_private   = 2,
      adj_delegate  = 4,
    };
    
  struct xs_property : ast_base
    {
      xs_property() : adjetives(0) {}
      
      str name;
      str type;
      expression value;
      int        adjetives; 
      code       get; 
      code       set; 
    };

  typedef std::vector<xs_property>    xs_property_list;     
  typedef reference<xs_property_list> xs_property_list_ref; 
    
  struct xs_method : ast_base
    {
      xs_method() : adjetives(0) {}

      str             name;
      str             type;
      param_list_decl args;
      code            cde; 
      int             adjetives; 
    };
    
  struct xs_event : ast_base
    {
      std::vector<str> name;
      param_list_decl  args;
      code             cde; 
    };

  struct xs_event_decl : ast_base
    {
      str             name;
      param_list_decl args;
    };

  struct xs_implement_behaviour : ast_base
    {
      str  name;
      code cde; 
    };

  struct xs_container : ast_base
    {
      void    visit(xs_visitor* visitor);
      void    add(variant item);
      size_t  size();
      variant get(size_t idx);

      protected:
        typedef std::vector<variant> xs_items;
        
        xs_items items_;
    };
    
  struct xs_instance : xs_container
    {
      std::vector<str> id;
      str              class_name;
    };

  
  struct xs_class : xs_container
    {
      str                   name;
      str                   super;
      std::vector<xs_const> args;
    };
    
  struct xs_behaviour : xs_container
    {
      str name;
      str super;
    };

  //high level visitor
  struct xs_visitor
    {
      virtual void property_(xs_property& info)             = 0;
      virtual void method_(xs_method& info)                 = 0;
      virtual void event_(xs_event& info)                   = 0;
      virtual void event_decl_(xs_event_decl& info)         = 0;
      virtual void const_(xs_const& info)                   = 0;
      virtual void instance_(xs_instance& info)             = 0;
      virtual void class_(xs_class& info)                   = 0;
      virtual void behaviour_(xs_behaviour& info)           = 0;
      virtual void behaveas_(xs_implement_behaviour& info)  = 0;
      virtual void dsl_(dsl& info)                          = 0;
    };
    
  //and a empty implementor
  struct base_xs_visitor : xs_visitor
    {
      virtual void property_(xs_property& info)             {}
      virtual void method_(xs_method& info)                 {}
      virtual void event_(xs_event& info)                   {}
      virtual void event_decl_(xs_event_decl& info)         {}
      virtual void const_(xs_const& info)                   {}
      virtual void instance_(xs_instance& info)             {}
      virtual void class_(xs_class& info)                   {}
      virtual void behaviour_(xs_behaviour& info)           {}
      virtual void behaveas_(xs_implement_behaviour& info)  {}
      virtual void dsl_(dsl& info)                          {}
    };

  struct implemented_xs_visitor : xs_visitor
    {
      virtual void property_(xs_property& info)             {assert(false);}
      virtual void method_(xs_method& info)                 {assert(false);}
      virtual void event_(xs_event& info)                   {assert(false);}
      virtual void event_decl_(xs_event_decl& info)         {assert(false);}
      virtual void const_(xs_const& info)                   {assert(false);}
      virtual void instance_(xs_instance& info)             {assert(false);}
      virtual void class_(xs_class& info)                   {assert(false);}
      virtual void behaviour_(xs_behaviour& info)           {assert(false);}
      virtual void behaveas_(xs_implement_behaviour& info)  {assert(false);}
      virtual void dsl_(dsl& info)                          {assert(false);}
    };
    
  //some types for the folks out there
  typedef std::vector<xs_method>      xs_method_list;     
  typedef reference<xs_method_list>   xs_method_list_ref; 
  typedef std::vector<xs_event>       xs_event_list;     
  typedef reference<xs_event_list>    xs_event_list_ref; 
  
  register_iterator(xs_event);
}

#endif

