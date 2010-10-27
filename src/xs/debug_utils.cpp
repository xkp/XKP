
#include <xs/debug_utils.h>
#include <iostream>

using namespace xkp;

//print_expression
void print_expression::push(variant operand)
  {
    print_ident();
    if (operand.is<expression_identifier>())
      {
        expression_identifier ei = operand;
        std::cout << "ident: " << ei.value << std::endl;
      }
    else if (operand.is<int>())
      {
        int i = operand;
        std::cout << "value: " <<  i << std::endl;
      }
    else if (operand.is<float>())
      {
        float i = operand;
        std::cout << "value: " << i << std::endl;
      }
    else 
      std::cout << "unknown operand" << std::endl;
  }
  
void print_expression::exec_operator(operator_type op, int pop_count, int push_count)
  {
    print_ident();
    std::cout << "operator: " << op <<std::endl;
  }
  
void print_expression::print_ident()
  {
    for(int i = 0; i < indent_; i++)
      std::cout << " ";
  }
    
//print_code
void print_code::if_(stmt_if& info)
  {
    print_info( "IF:" );
    print_info( "condition:" );
    print_expr( indent_ + 4, info.expr );
    print_info( "code:" );
    print( indent_ + 4, info.if_code);
    
    if (!info.else_code.empty())
      {
        print_info( "else:" );
        print( indent_ + 4, info.else_code);
      }
  }
  
void print_code::variable_(stmt_variable& info)
  {
    print_info( "LOCAL VARIABLE:" );
    print_info( "type: " + info.type );
    print_info( "name: " + info.id );
    
    if (!info.value.empty())
      {
        print_info( "value: " );
        print_expr( indent_ + 4, info.value );
      }
  }
  
void print_code::for_(stmt_for& info)
  {
    print_info( "FOR:" );
    if (!info.init_variable.empty())
      {
        print_info( "init variable:" );
        variable_(info.init_variable);
      }
    else if (!info.init_expr.empty())
      {
        print_info( "init expression:" );
        print_expr( indent_ + 4, info.init_expr );
      }
      
    if (!info.cond_expr.empty())
      {
        print_info( "condition:" );
        print_expr( indent_ + 4, info.cond_expr );
      }

    if (!info.iter_expr.empty())
      {
        print_info( "iteration:" );
        print_expr( indent_ + 4, info.iter_expr );
      }

    print_info( "code:" );
    print( indent_ + 4, info.for_code );
  }
  
void print_code::iterfor_(stmt_iter_for& info)
  {
    print_info( "ITERATED FOR:" );
    print_info( "iterator type: " + info.type.name );
    print_info( "iterator name: " + info.id );
    print_info( "iterator: " );
    print_expr( indent_ + 4, info.iter_expr );
    
    print_info( "code:" );
    print( indent_ + 4, info.for_code );
  }
  
void print_code::while_(stmt_while& info)
  {
    print_info( "WHILE:" );
    print_info( "condition:" );
    print_expr( indent_ + 4, info.expr );
    print_info( "code:" );
    print( indent_ + 4, info.while_code );
  }
  
void print_code::break_()
  {
    print_info( "BREAK;" );
  }
  
void print_code::continue_()
  {
    print_info( "CONTINUE;" );
  }
  
void print_code::return_(stmt_return& info)
  {
    if (info.expr.empty())
      print_info( "RETURN;" );
    else
      {
        print_info( "RETURN:" );
        print_info( "value:" );
        print_expr( indent_ + 4, info.expr );
      }
  }
  
void print_code::expression_(stmt_expression& info)
  {
     print_info( "CODE EXPRESSION:" );
     print_expr( indent_ + 4, info.expr );
  }
  
void print_code::dsl_(dsl& info)
  {
     print_info( "DSL:" );
     print_info( "name:" + info.name);
     print_info( "id:" + info.id);
     print_info( "text:" + info.text);
  }

void print_code::dispatch(stmt_dispatch& info)
  {
     print_info( "DISPATCH:" );
  }
  
void print_code::print_ident()
  {
    for(int i = 0; i < indent_; i++)
      std::cout << " ";
  }
  
void print_code::print_info( const str& info )
  {
    print_ident(); std::cout << info << std::endl;
  }

void print_code::print_expr( int indent, expression& expr )
  {
    print_expression pe(indent);
    expr.visit(&pe);
  }

void print_code::print( int indent, code& c)
  {
    print_code pc(indent);
    c.visit( &pc );
  }

//print_xs
void print_xs::property_(xs_property& info)
  {
    print_info("PROPERTY");
    print_info("name: " + info.name);
    print_info("type: " + info.type);
    print_info("value: ");
    print_expr(indent_ + 4, info.value);
    print_info("get: ");
    print__code(indent_ + 4, info.get);
    print_info("set: ");
    print__code(indent_ + 4, info.set);
  }
  
void print_xs::method_(xs_method& info)
  {
    print_info("METHOD");
    print_info("name: " + info.name);
    print_info("parameters:");
    print_parameters(indent_ + 4, info.args);
    print_info("code:");
    print__code(indent_ + 4, info.cde);
  }
  
void print_xs::event_(xs_event& info)
  {
    print_info("EVENT");
    print_qualified(info.name);
    print_info("parameters:");
    print_parameters(indent_ + 4, info.args);
    print_info("code:");
    print__code(indent_ + 4, info.cde);
  }
  
void print_xs::event_decl_(xs_event_decl& info)
  {
    print_info("EVENT DECLARATION");
    print_info(info.name);
    print_info("parameters:");
    print_parameters(indent_ + 4, info.args);
  }
  
void print_xs::const_(xs_const& info)
  {
    print_info("CONSTANT");
    print_info(info.name);
    print_info("value:");
    print_expr(indent_ + 4, info.value);
  }
  
void print_xs::instance_(xs_instance& info)
  {
    print_info("INSTANCE");
    print_qualified(info.id);
    print_info("members:");
    print_block(indent_ + 4, info);
  }
  
void print_xs::class_(xs_class& info)
  {
    print_info("CLASS");
    print_info("name:" + info.name);
    print_info("inherits from:" + info.super);
    print_info("members:");
    print_block(indent_ + 4, info);
  }
  
void print_xs::behaviour_(xs_behaviour& info)
  {
    print_info("BEHAVIOUR");
    print_info("name:" + info.name);
    print_info("inherits from:" + info.super);
    print_info("members:");
    print_block(indent_ + 4, info);
  }

void print_xs::behaveas_(xs_implement_behaviour& info)
  {
    print_info("BEHAVE AS");
    print_info("name:" + info.name);
    print__code(indent_ + 4, info.cde);
  }
  
void print_xs::dsl_(dsl& info)
  {
     print_info( "DSL" );
     print_info( "name:" + info.name);
     print_info( "id:" + info.id);
     print_info( "text:" + info.text);
  }
  
void print_xs::print_ident()
  {
    for(int i = 0; i < indent_; i++)
      std::cout << " ";
  }
  
void print_xs::print_info( const str& info )
  {
    print_ident(); std::cout << info << std::endl;
  }
  
void print_xs::print_expr( int indent, expression& expr )
  {
    print_expression pe(indent);
    expr.visit(&pe);
  }
  
void print_xs::print__code( int indent, code& c)
  {
    print_code pc(indent);
    c.visit( &pc );
  }
  
void print_xs::print_parameters( int indent, param_list_decl& pl)
  {
    int old_indent = indent_;
    indent_ = indent;
    
    param_list_decl::iterator it = pl.begin();
    param_list_decl::iterator nd = pl.end();
    for(; it != nd; it++)
      {
        print_info("PARAMETER:");
        print_info("name: " + it->name);
        print_info("type: " + it->type);
        print_info("default value:");
        print_expr(indent + 4, it->default_value);
      }

    indent_ = old_indent;
  }

void print_xs::print_qualified(std::vector<str>& id)
  {
    std::vector<str>::iterator it = id.begin();
    std::vector<str>::iterator nd = id.end();
    str res;
    for(; it != nd; it++)
      res += *it + '.';
      
    res.erase(res.size() - 1, 1);
    print_info( res );
  }

void print_xs::print_block( int indent, xs_container& xs)
  {
    print_xs pxs(indent);
    xs.visit(&pxs);
  }
