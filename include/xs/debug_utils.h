
#ifndef XS_DEBUG_UTILS
#define XS_DEBUG_UTILS

#include "ast.h"

namespace xkp
{
  struct print_expression : expression_visitor
    {
      print_expression()           : indent_(0)      {}
      print_expression(int indent) : indent_(indent) {}
    
      virtual void push(variant operand);
      virtual void exec_operator(operator_type op, int pop_count, int push_count);
      
      private:
        void print_ident();
        
      int indent_;
    };
    
  struct print_code : code_visitor
    {
      print_code()           : indent_(0)      {}
      print_code(int indent) : indent_(indent) {}

      virtual void if_(stmt_if& info);
      virtual void variable_(stmt_variable& info);
      virtual void for_(stmt_for& info);
      virtual void iterfor_(stmt_iter_for& info);
      virtual void while_(stmt_while& info);
      virtual void break_();
      virtual void continue_();
      virtual void return_(stmt_return& info);
      virtual void expression_(stmt_expression& info);
      virtual void dsl_(dsl& info);
      virtual void dispatch(stmt_dispatch& info);
      
      private:
        int indent_;

        void print_ident();
        void print_info( const str& info );
        void print_expr( int indent, expression& expr );
        void print( int indent, code& c);
    };
    
  struct print_xs : xs_visitor
    {
      print_xs()           : indent_(0)      {}
      print_xs(int indent) : indent_(indent) {}

      virtual void property_(xs_property& info);
      virtual void method_(xs_method& info);
      virtual void event_(xs_event& info);
      virtual void event_decl_(xs_event_decl& info);
      virtual void const_(xs_const& info);
      virtual void instance_(xs_instance& info);
      virtual void class_(xs_class& info);
      virtual void behaviour_(xs_behaviour& info);
      virtual void behaveas_(xs_implement_behaviour& info);
      virtual void dsl_(dsl& info);

      private:
        int indent_;

        void print_ident();
        void print_info( const str& info );
        void print_expr( int indent, expression& expr );
        void print__code( int indent, code& c);
        void print_parameters( int indent, param_list_decl& pl);
        void print_qualified(std::vector<str>& id);
        void print_block( int indent, xs_container& xs);
    };
}

#endif