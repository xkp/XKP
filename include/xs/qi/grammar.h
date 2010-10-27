
#ifndef XS_GRAMMAR_H
#define XS_GRAMMAR_H

#include "ast.h"
#include "code.h"

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_container.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::spirit;
using namespace boost::spirit::qi;
using namespace boost::spirit::ascii;
using namespace boost::spirit::arg_names;

using boost::phoenix::function;
using boost::phoenix::ref;
using boost::phoenix::size;

namespace xkp
{
  //skipper
  template <typename Iterator>
  struct white_space : grammar<Iterator>
    {
      white_space() : white_space::base_type(start)
      {
          start =
                  space                               // tab/space/cr/lf
              |   "/*" >> *(char_ - "*/") >> "*/"     // C-style comments
              ;
      }

      rule<Iterator> start;
    };
  
  //operators
  struct postfix_operator_t : symbols<char, operator_type>
    {
      postfix_operator_t()
        {
          add
              ("++" , op_inc)
              ("--" , op_dec)
          ;
        }
    }postfix_operator;

  struct unary_operator_t : symbols<char, operator_type>
    {
      unary_operator_t()
        {
          add
              ("+" , op_unary_plus)
              ("-" , op_unary_minus)
              ("!" , op_not)
          ;
        }
    }unary_operator;

  struct multiplicative_operator_t : symbols<char, operator_type>
    {
      multiplicative_operator_t()
        {
          add
              ("*" ,  op_mult)
              ("/" ,  op_divide)
              ("%" ,  op_mod)
              ("as",  op_typecast)
              ("is",  op_typecheck)
              ("has", op_namecheck)
          ;
        }
    }multiplicative_operator;

  struct additive_operator_t : symbols<char, operator_type>
    {
      additive_operator_t()
        {
          add
              ("+" , op_plus)
              ("-" , op_minus)
          ;
        }
    }additive_operator;

  struct shift_operator_t : symbols<char, operator_type>
    {
      shift_operator_t()
        {
          add
              (">>" , op_shift_right)
              ("<<" , op_shift_left)
          ;
        }
    }shift_operator;

  struct equality_operator_t : symbols<char, operator_type>
    {
      equality_operator_t()
        {
          add
              ("==" , op_equal)
              ("!=" , op_notequal)
          ;
        }
    }equality_operator;

  struct relational_operator_t : symbols<char, operator_type>
    {
      relational_operator_t()
        {
          add
              (">" ,  op_gt)
              ("<" ,  op_lt)
              (">=" , op_ge)
              ("<=" , op_le)
          ;
        }
    }relational_operator;

  struct logical_operator_t : symbols<char, operator_type>
    {
      logical_operator_t()
        {
          add
              ("&&" , op_and)
              ("||" , op_or)
          ;
        }
    }logical_operator;

  struct assign_operator_t : symbols<char, operator_type>
    {
      assign_operator_t()
        {
          add
              ("=" ,   op_assign)
              ("+=" ,  op_plus_equal)
              ("-=" ,  op_minus_equal)
              ("*=" ,  op_mult_equal)
              ("/=" ,  op_div_equal);
        }
    }assign_operator;

  struct bool_constants_t : symbols<char, bool>
  {
      bool_constants_t()
      {
          add
              ("true",  true)
              ("false", false)
          ;
      }

  }bool_constants;

  //semantic actions
  struct code_push
    {
      template <typename A>
      struct result { typedef void type; };
      
      code_context& code;
      code_push(code_context& _code) : code( _code ) {}

      void operator()(operator_type op) const
        {
            code.push_operator(op);
        }
      
      void operator()(std::string val) const
        {
            code.push_operand(val);
        }

      void operator()(int val) const
        {
            code.push_operand(val);
        }

      void operator()(double val) const
        {
            code.push_operand(val);
        }

      void operator()(bool val) const
        {
            code.push_operand(val);
        }
    };
    
  struct code_push_ident
    {
      template <typename A>
      struct result { typedef void type; };

      code_context& code;
      code_push_ident(code_context& _code) : code( _code ) {}

      void operator()(std::string id) const
        {
            code.push_identifier(id);
        }
    };
  
  struct code_try_rule
    {
      typedef void result_type;
      struct result { typedef void type; };

      code_context& code;
      code_try_rule(code_context& _code) : code( _code ) {}

      void operator()() const
        {
            code.try_rule();
        }
    };

  struct code_accept_rule
    {
      typedef void result_type;
      struct result { typedef void type; };

      code_context& code;
      code_accept_rule(code_context& _code) : code( _code ) {}

      void operator()() const
        {
            code.accept_rule();
        }
    };

  struct code_reject_rule
    {
      typedef void result_type;
      struct result { typedef void type; };

      code_context& code;
      code_reject_rule(code_context& _code) : code( _code ) {}

      void operator()() const
        {
            code.reject_rule();
        }
    };

  //expressions
  template <typename Iterator>
  struct expression_grammar : grammar<Iterator, white_space<Iterator> >
    {
        expression_grammar(code_context& code) : 
          expression_grammar::base_type(expression_r),
          code_(code),
          push(code),
          push_ident(code),
          try_rule(code),
          accept_rule(code),
          reject_rule(code)
          {
            expression_r =
                assignment_r.alias()
                ;

            assignment_r =
                (   eps                               [try_rule()]
                    >>  identifier_r                  
                    >>  assign_operator               [push(_1)]
                    >   equality_r
                    >>  eps                           [accept_rule()]
                )
                |   
                (   eps                               [reject_rule()]
                    >>  equality_r
                )
                ;

            equality_r =
                relational_r
                >> *(   equality_operator             [push(_1)]
                    >   relational_r     
                    )
                ;

            relational_r =
                logical_r
                >> *(   relational_operator           [push(_1)]
                    >   logical_r
                    )
                ;

            logical_r =
                additive_r
                >> *(   logical_operator              [push(_1)]
                    >   additive_r
                    )
                ;

            additive_r =
                multiplicative_r
                >> *(   additive_operator             [push(_1)]
                    >   multiplicative_r
                    )
                ;

            multiplicative_r =
                unary_r
                >> *(   multiplicative_operator       [push(_1)]
                    >   unary_r
                    )
                ;

            unary_r =
                    primary_r                         
                |   (   unary_operator                [push(_1)]
                    >   primary_r                     
                    )
                ;

            primary_r =
                    constant_r
                |   array_r
                |   identifier_r
                |   '(' > expression_r > ')'
                ;

            string_r %= lexeme['"' >> +(char_ - '"') >> '"'];
        
            constant_r %= 
                    (int_ >> !(char_(".")))           [push(_1)]    
                |   double_                           [push(_1)]    
                |   string_r                          [push(_1)]    
                |   bool_constants                    [push(_1)]        
                ;
                
            simple_identifier_r %=
                raw[lexeme[alpha >> *(alnum | '_')]]  [push_ident(_1)]
                ;

            identifier_r =
                    simple_identifier_r             
                >>  *(  ( char_('.')                  [push(op_dot)]
                        > simple_identifier_r         [push_ident(_1)]
                        )
                        | (   char_('[')
                          >   expression_r
                          >   char_(']')              [push(op_index)]
                          )
                        | (   char_('(')
                          >>  -(
                                  expr                [++_a]
                                  >> *(',' > expr     [++_a])
                               )
                          >    char_(')')             [push(_a)] [push(op_call)]
                          )
                     )
                  ;
                     
            array_r = 
                    char_('[')
                >   -(    expr                        [++_a]
                    >>    *(',' > expr                [++_a])
                     )
                >   char_(']')                        [push(_a)] [push(op_array)];
          }
        
        //switch the target code
        void reset(code_context& code)
          {
            code_              = code;
            
            //is there a better way to access the phoenix::functions than this?
            push.f.code        = code;
            push_ident.f.code  = code;
            try_rule.f.code    = code;
            accept_rule.f.code = code;
            reject_rule.f.code = code;
          }

        //semantic actions
        function<code_push>        push;
        function<code_push_ident>  push_ident;
        function<code_try_rule>    try_rule;
        function<code_accept_rule> accept_rule;
        function<code_reject_rule> reject_rule;
        
        //code
        code_context& code_;
        
        //rules
        typedef white_space<Iterator> white_space;
        rule<Iterator, white_space> expression_r;
        rule<Iterator, white_space> assignment_r;
        rule<Iterator, white_space> equality_r;
        rule<Iterator, white_space> relational_r;
        rule<Iterator, white_space> logical_r;
        rule<Iterator, white_space> additive_r;
        rule<Iterator, white_space> multiplicative_r;
        rule<Iterator, white_space> unary_r;
        rule<Iterator, white_space> primary_r;
        rule<Iterator, white_space> constant_r;
        rule<Iterator, std::string(),  white_space> simple_identifier_r;
        rule<Iterator, locals<int>, white_space> identifier_r;
        rule<Iterator, locals<int>, white_space> array_r;
        rule<Iterator, std::string(),  white_space> string_r;
    };

  struct code_instruction
    {
      template <typename A>
      struct result { typedef void type; };
      
      code_context& code;
      code_instruction(code_context& _code) : code( _code ) {}

      void operator()(intermediate_instruction ii) const
        {
            code.push_instruction(ii);
        }
    };

  struct code_var_name
    {
      template <typename A>
      struct result { typedef void type; };
      
      code_context& code;
      code_var_name(code_context& _code) : code( _code ) {}

      void operator()(std::string vn) const
        {
            code.push_var(vn);
        }
    };

  struct code_var_type
    {
      template <typename A>
      struct result { typedef void type; };
      
      code_context& code;
      code_var_type(code_context& _code) : code( _code ) {}

      void operator()(std::vector<std::string> vt) const
        {
            code.push_var_type(vt);
        }
    };

  template <typename Iterator>
  struct code_grammar : grammar<Iterator, white_space<Iterator> >
    {
      code_grammar(code_context& code) : 
        code_grammar::base_type(statements_r),
        code_(code),
        expr(code),
        instruction(code),
        var_name(code),
        var_type(code)
        {
          statements_r = *(statement_r);

          code_block_r  =   
              char_('{')  
              > *( statement_r ) 
              > char_('}');
                            
          block_or_statement_r = 
              code_block_r 
              | statement_r
              ;

          statement_r =   
                  var_decl_r
              |   for_r
              |   if_r
              |   while_r
              |   switch_r
              |   return_r
              |   dispatch_r
              //td:
              //|   waitfor_r     this should be handled at link level, maybe?
              //|   bind_r        code.try_rule has got to be recursive
              //|   instance_r    must figure out how to reuse other grammar's rules
              //|   dsl_r         how do we write custom parsers
              |   (expr > char_(';'))
              ;

          var_decl_r = 
                  identifier_r                    [var_type(_1)]
              >>  simple_identifier_r             [var_name(_1)] [instruction(ii_vardecl)] 
              >>  ( char_( '=' )
                  > ( expr                        [instruction(ii_varinit)]
                    | delegate_r                  [instruction(ii_delegate)] //td: _1 pass the code
                    )
                  )
                  | char_( ';' )                  
              ;
               
          for_r =   
              lit("for")
              >   char_('(')
              >   (
                    ( var_decl_r                  //traditional for 
                    | expr 
                    | eps
                    )
                    > char_(';')                  [instruction(ii_for_cond)]  
                    > ( expr | eps )
                    > char_(';')                  [instruction(ii_for_begin)]
                    > ( expr | eps )              [instruction(ii_for_action)]  
                  )
                  |                               //iterated for 
                  (
                    simple_identifier_r           [var_name(_1)]   
                    > -(  ',' 
                       >  simple_identifier_r     [var_name(_1)]   
                       )
                    >  lit( "in" )                [instruction(ii_iterfor_begin)]           
                    >  expr                       [instruction(ii_iterfor_value)]  
                  )
              >   char_(')')
              >   block_or_statement_r
              >   eps                             [instruction(ii_for_end)]  
              ;

          if_r =   
              lit("if")                                 
              >   char_('(')
              >   expr                            [instruction(ii_if)]  
              >   char_(')')
              >   block_or_statement_r
              >   -(  lit("else")                 [instruction(ii_else)]
                   >  block_or_statement_r
                   )
              >   eps                             [instruction(ii_endif)];
                            
          while_r =   
              lit("while")                        [instruction(ii_while_begin)]  
              >   char_('(')
              >   expr                            [instruction(ii_while_value)]  
              >   char_(')')
              >   block_or_statement_r
              >   eps                             [instruction(ii_endwhile)]
              ;
                            
          switch_r =   
                  lit("switch")                              
              >   char_('(')
              >   expr                            [instruction(ii_switch)]  
              >   char_(')')
              >   char_('{')
              >   *(  (   lit("case")             [instruction(ii_case_begin)]  
                      >   expr                    [instruction(ii_case)]  
                      >   char_(':')
                      >   block_or_statement_r    [instruction(ii_case_end)]
                      )
                      |
                      (   lit("default")
                      >   char_(':')              [instruction(ii_default_begin)]  
                      >   block_or_statement_r    [instruction(ii_default_end)]
                      )
                    )
              >   char_('}')                      [instruction(ii_switch_end)]
              ;

                            
          return_r  =   
                  lit( "return" )
              >   expr                            [instruction(ii_return)]
              >   char_(';')
              ;

          dispatch_r =   
                  lit("dispatch")                 [instruction(ii_dispatch_begin)]  
              >   expr                            [instruction(ii_dispatch)]  
              >   char_(';')
              ;
              
          delegate_r = 
              code_block_r; //td: push/pop code

          identifier_r %=
              simple_identifier_r % '.'
              ;
              
          simple_identifier_r %=
              raw[lexeme[alpha >> *(alnum | '_')]]
              ;
        }

      void reset(code_context& code)
        {
          code_      = code;
          expr.reset( code );
          instruction.f.code = code;
          var_name.f.code    = code;
          var_type.f.code    = code;
        }
      
      code_context&                code_;
      expression_grammar<Iterator> expr; 

      //semantic actions
      function<code_instruction>  instruction;
      function<code_var_name>     var_name;
      function<code_var_type>     var_type;

      //rules
      typedef white_space<Iterator> white_space;
      rule<Iterator, white_space>   statements_r;
      rule<Iterator, white_space>   code_block_r;
      rule<Iterator, white_space>   block_or_statement_r;
      rule<Iterator, white_space>   statement_r;
      rule<Iterator, white_space>   var_decl_r;
      rule<Iterator, white_space>   for_r;
      rule<Iterator, white_space>   if_r;
      rule<Iterator, white_space>   while_r;
      rule<Iterator, white_space>   switch_r;
      rule<Iterator, white_space>   return_r;
      rule<Iterator, white_space>   dispatch_r;
      rule<Iterator, white_space>   delegate_r;
      rule<Iterator, std::vector<std::string>(), white_space> identifier_r;
      rule<Iterator, std::string(), white_space> simple_identifier_r;
    };

  //semantic actions
  template <typename Iterator>
  struct reset_code
    {
      template <typename A>
      struct result { typedef void type; };

      reset_code(code_grammar<Iterator>& code) : code_(code)
        {
        }

      void operator()(code_context& ctx) const
        {
            code_.reset(ctx);
        }
        
      code_grammar<Iterator>& code_;
    };

  template <typename Iterator>
  struct xs_grammar : grammar<Iterator, white_space<Iterator> >
    {
      xs_grammar() : 
        xs_grammar::base_type(xs_r),
        code_(useless),
        reset(code_) 
        {
          xs_r = 
              +( code_r )
              ;
        
          code_r = 
                  lit("{") [reset(_val)]
              >   code_
              >   lit("}")
              ;
        }
        
      code_context           useless;
      code_grammar<Iterator> code_; 

      //semantoc actions
      function< reset_code<Iterator> > reset;

      //rules
      typedef white_space<Iterator> white_space;
      rule<Iterator,                 white_space> xs_r;
      rule<Iterator, code_context(), white_space> code_r;
    };
    
}

#endif