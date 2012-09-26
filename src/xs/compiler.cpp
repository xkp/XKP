
#include <xs/compiler.h>
#include <xs/xs_error.h>

extern "C"
{
  #include <xs/engine.h>
  #include <xs/grammar.h>
}

#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace xkp;

const str SCompilerError("syntax-error");
const str SErrorCompiling("Error while compiling xs");

//utils
inline str wide2str(const std::wstring& w)
  {
    str result;
    result.assign(w.begin(), w.end());

    return result;
  }

inline std::wstring str2wide(const str& s)
  {
    std::wstring result;
    result.assign(s.begin(), s.end());

    return result;
  }

inline file_position from_token(TokenStruct* token)
  {
    return file_position(token->Line, token->Column);
  }

//a list of rules of interest, note that this struct ought to be updated
//everytime the grammar is changed, use the rule description as a guide
enum xs_rules
  {
    rule_qualified_id   = 0,        //<Qualified ID> ::= Identifier <Member List>
    rule_member_list    = 1,        //<Member List> ::= <Member List> MemberName
    rule_member_empty   = 2,        //<Member List> ::=
    rule_single_expr    = 13,       //<Expression List> ::= <Expression>
    rule_expr_list      = 14,       //<Expression List> ::= <Expression> ',' <Expression List>
    rule_assign         = 15,       //<Expression> ::= <Conditional Exp> '=' <Expression>
    rule_plusequal      = 16,       //<Expression> ::= <Conditional Exp> '+=' <Expression>
    rule_minusequal     = 17,       //<Expression> ::= <Conditional Exp> '-=' <Expression>
    rule_multequal      = 18,       //<Expression> ::= <Conditional Exp> '*=' <Expression>
    rule_divequal       = 19,       //<Expression> ::= <Conditional Exp> '/=' <Expression>
    rule_shiftlequal    = 20,       //<Expression> ::= <Conditional Exp> '<<=' <Expression>
    rule_shiftrequal    = 21,       //<Expression> ::= <Conditional Exp> '>>=' <Expression>
    rule_conditional    = 23,       //<Conditional Exp> ::= <Or Exp> '?' <Or Exp> ':' <Conditional Exp>
    rule_or             = 25,       //<Or Exp> ::= <Or Exp> '||' <And Exp>
    rule_and            = 27,       //<And Exp> ::= <And Exp> '&&' <Equality Exp>
    rule_equal          = 29,       //<Equality Exp> ::= <Equality Exp> '==' <Compare Exp>
    rule_notequal       = 30,       //<Equality Exp> ::= <Equality Exp> '!=' <Compare Exp>
    rule_lt             = 32,       //<Compare Exp> ::= <Compare Exp> '<' <Shift Exp>
    rule_gt             = 33,       //<Compare Exp> ::= <Compare Exp> '>' <Shift Exp>
    rule_le             = 34,       //<Compare Exp> ::= <Compare Exp> '<=' <Shift Exp>
    rule_ge             = 35,       //<Compare Exp> ::= <Compare Exp> '>=' <Shift Exp>
    rule_cast           = 36,       //<Compare Exp> ::= <Compare Exp> as <Shift Exp>
    rule_typecheck      = 37,       //<Compare Exp> ::= <Compare Exp> is <Shift Exp>
    rule_namecheck      = 38,       //<Compare Exp> ::= <Compare Exp> has <Shift Exp>
    rule_shiftleft      = 40,       //<Shift Exp> ::= <Shift Exp> '<<' <Add Exp>
    rule_shiftright     = 41,       //<Shift Exp> ::= <Shift Exp> '>>' <Add Exp>
    rule_add            = 43,       //<Add Exp> ::= <Add Exp> '+' <Mult Exp>
    rule_sub            = 44,       //<Add Exp> ::= <Add Exp> '-' <Mult Exp>
    rule_mult           = 46,       //<Mult Exp> ::= <Mult Exp> '*' <Unary Exp>
    rule_div            = 47,       //<Mult Exp> ::= <Mult Exp> '/' <Unary Exp>
    rule_mod            = 48,       //<Mult Exp> ::= <Mult Exp> '%' <Unary Exp>
    rule_not            = 50,       //<Unary Exp> ::= '!' <Unary Exp>
    rule_unary_minus    = 51,       //<Unary Exp> ::= '-' <Unary Exp>
    rule_pre_inc        = 52,       //<Unary Exp> ::= '++' <Unary Exp>
    rule_pre_dec        = 53,       //<Unary Exp> ::= -- <Unary Exp>
    rule_members        = 57,       //<Method> ::= MemberName
    rule_member_call    = 58,       //<Method> ::= MemberName '(' <Arg List Opt> ')'
    rule_index          = 59,       //<Method> ::= '[' <Expression List> ']'
    rule_post_inc       = 60,       //<Method> ::= '++'
    rule_post_dec       = 61,       //<Method> ::= --
    rule_object_value   = 66,       //<Object Value> ::= Identifier ':' <Expression>
    rule_instantiation  = 68,       //<Primary Exp> ::= new <Type> '(' <Arg List Opt> ')'
    rule_object         = 69,       //<Primary Exp> ::= '{' <Object List Opt> '}'
    rule_array          = 71,       //<Array Literal> ::= '[' <Arg List Opt> ']'
    rule_func_call      = 74,       //<Primary> ::= Identifier '(' <Arg List Opt> ')'
    rule_empty_args     = 77,       //<Arg List Opt> ::=
    rule_argument_list  = 78,       //<Arg List> ::= <Arg List> ',' <Argument>
    rule_named_argument = 80,       //<Argument> ::= Identifier '=' <Conditional Exp>
    rule_argument       = 81,       //<Argument> ::= <Conditional Exp>
    rule_type_params    = 82,       //<Type> ::= Identifier '<' <Type Arg List> '>'
    rule_type           = 83,       //<Type> ::= Identifier
    rule_type_args      = 84,       //<Type Arg List> ::= <Type Arg List> ',' <Type Argument>
    rule_type_arg_named = 86,       //<Type Argument> ::= Identifier '=' <Primary Exp>
    rule_complete_dsl   = 88,       //<DSL> ::= DSLIdentifier Identifier '(' <Arg List Opt> ')' '@' DecLiteral
    rule_parameter_dsl  = 89,       //<DSL> ::= DSLIdentifier '(' <Arg List Opt> ')' '@' DecLiteral
    rule_simple_dsl     = 90,       //<DSL> ::= DSLIdentifier '@' DecLiteral
    rule_if             = 94,       //<Statement> ::= if '(' <Expression> ')' <Statement>
    rule_ifelse         = 95,       //<Statement> ::= if '(' <Expression> ')' <Statement> else <Statement>
    rule_for            = 96,       //<Statement> ::= for '(' <For Init Opt> ';' <For Condition Opt> ';' <For Iterator Opt> ')' <Statement>
    rule_iterfor        = 97,       //<Statement> ::= for '(' <Type> Identifier in <Expression> ')' <Statement>
    rule_while          = 98,       //<Statement> ::= while '(' <Expression> ')' <Statement>
    rule_switch         = 99,       //<Statement> ::= switch '(' <Switch Header> ')' '{' <Switch Sections Opt> '}'
    rule_dispatch       = 100,      //<Statement> ::= dispatch <Qualified ID> '(' <Arg List Opt> ')' ';'
    rule_break          = 101,      //<Statement> ::= break ';'
    rule_continue       = 102,      //<Statement> ::= continue ';'
    rule_return         = 103,      //<Statement> ::= return <Expression Opt> ';'
    rule_try            = 104,      //<Statement> ::= try <Block> <Catch Clauses> <Finally Clause Opt>
    rule_throw          = 105,      //<Statement> ::= throw <Expression> ';'
    rule_expr_stmt      = 106,      //<Statement> ::= <Expression> ';'
    rule_code_block     = 107,      //<Statement> ::= <Block>
    rule_code_dsl       = 108,      //<Statement> ::= <DSL>
    rule_catch_id       = 111,      //<Catch Clause> ::= catch '(' <Type> Identifier ')' <Block>
    rule_catch_type     = 112,      //<Catch Clause> ::= catch '(' <Type> ')' <Block>
    rule_catch_any      = 113,      //<Catch Clause> ::= catch <Block>
    rule_finally        = 114,      //<Finally Clause Opt> ::= finally <Block>
    rule_block          = 116,      //<Block> ::= '{' <Stm List> '}'
    rule_vardecl        = 118,      //<Local Var Decl> ::= <Type> <Variable Declarator>
    rule_varname        = 119,      //<Variable Declarator> ::= Identifier
    rule_varinit        = 120,      //<Variable Declarator> ::= Identifier '=' <Expression>
    rule_switchsection  = 125,      //<Switch Section> ::= <Switch Labels> <Stm List>
    rule_switchcase     = 128,      //<Switch Label> ::= case <Expression> ':'
    rule_switchdefault  = 129,      //<Switch Label> ::= default ':'
    rule_for_vardecl    = 130,      //<For Init Opt> ::= <Local Var Decl>
    rule_for_initexpr   = 131,      //<For Init Opt> ::= <Expression>
    rule_for_opt        = 132,      //<For Iterator Opt> ::= <Expression>
    rule_for_cond       = 135,      //<For Condition Opt> ::= <Expression>
    rule_adjetive_list  = 141,      //<Adj List> ::= <Adj List> <Adjetive>
    rule_plist_empty    = 148,      //<Arg Decl List Opt> ::=
    rule_plist_decl     = 149,      //<Arg Decl List> ::= <Arg Decl List> ',' <Argument Decl>
    rule_default_arg    = 151,      //<Argument Decl> ::= <Type> Identifier '=' <Expression>
    rule_arg            = 152,      //<Argument Decl> ::= <Type> Identifier
    rule_untyped_arg    = 153,      //<Argument Decl> ::= Identifier
    rule_class_plist    = 154,      //<Class Arg Decl List> ::= <Class Arg Decl List> ',' <Class Argument Decl>
    rule_class_def_arg  = 156,      //<Class Argument Decl> ::= Identifier '=' <Primary Exp>
    rule_class_arg      = 157,      //<Class Argument Decl> ::= Identifier
    rule_class_args     = 158,      //<Class Arg List Opt> ::= '<' <Class Arg Decl List> '>'
    rule_class_no_args  = 159,      //<Class Arg List Opt> ::=
    rule_adj_property   = 160,      //<Construct> ::= <Adj List> <Property>
    rule_adj_method     = 161,      //<Construct> ::= <Adj List> <Method Decl>
    rule_instance       = 164,      //<Construct> ::= instance <Qualified ID> <Type Opt> <Construct Block>
    rule_class          = 165,      //<Construct> ::= class Identifier <Class Arg List Opt> <Type Opt> <Construct Block>
    rule_behaviour      = 166,      //<Construct> ::= behaviour Identifier <Type Opt> <Construct Block>
    rule_event          = 167,      //<Construct> ::= on <Qualified ID> '(' <Arg Decl List Opt> ')' <Block>
    rule_const          = 168,      //<Construct> ::= const Identifier '=' <Expression> ';'
    rule_event_decl     = 169,      //<Construct> ::= event Identifier '(' <Arg Decl List Opt> ')' ';'
    rule_behaveas       = 170,      //<Construct> ::= behave as Identifier <Block>
    rule_behaveas_empty = 171,      //<Construct> ::= behave as Identifier ';'
    rule_bind           = 172,      //<Construct> ::= <Expression> '->' <Expression> ';'
    rule_construct_dsl  = 173,      //<Construct> ::= <DSL>
    rule_property       = 174,      //<Property> ::= property Identifier <Type Opt> <Prop Descriptor>
    rule_var_property   = 175,      //<Property> ::= <Local Var Decl> ';'
    rule_method         = 178,      //<Method Decl> ::= <Method Name> Identifier '(' <Arg Decl List Opt> ')' <Type Opt> <Block>
    rule_method_c       = 179,      //<Method Decl> ::= <Type> Identifier '(' <Arg Decl List Opt> ')' <Block>
    rule_typed          = 180,      //<Type Opt> ::= ':' <Type>
    rule_prop_get_set   = 183,      //<Prop Descriptor> ::= '=' <Block> <Block>
    rule_prop_get       = 184,      //<Prop Descriptor> ::= '=' <Block>
    rule_prop_value_set = 185,      //<Prop Descriptor> ::= '=' <Expression> <Block>
    rule_prop_value     = 186,      //<Prop Descriptor> ::= '=' <Expression> ';'
  };

//ditto for symbols
enum xs_terminals
  {
    terminal_char_string  = 49, //"CharLiteral"
    terminal_int          = 53, //"DecLiteral"
    terminal_delegate     = 55, //"delegate"
    terminal_false        = 60, //"false"
    terminal_hex          = 65, //"HexLiteral"
    terminal_identifier   = 66, //"Identifier"
    terminal_member_name  = 71, //"MemberName"
    terminal_null         = 74, //"null"
    terminal_private      = 76, //"private"
    terminal_public       = 78, //"public"
    terminal_float        = 79, //"RealLiteral"
    terminal_string       = 81, //"StringLiteral"
    terminal_true         = 84, //"true"
  };

//parse tree visitors, will walk the parse tree and return a human usable
//abstract syntax tree
struct parsetree_visitor
  {
    template <typename T>
    void visit(TokenStruct* token, T& data)
      {
        if (!data.visit(token, this))
          {
            if (token->ReductionRule >= 0) //ignore terminals
              {
                //if a rule is ignored by the acting visitor lets traverse the children
                int TokenCount = Grammar.RuleArray[token->ReductionRule].SymbolsCount;

                for(int i = 0; i < TokenCount; i++)
                  {
                    visit(token->Tokens[i], data);
                  }
              }
          }

        ast_base* target = data.target();
        if (target)
          {
            file_position begin;
            file_position end;
            if (token->ReductionRule < 0)
              {
                //terminal
                begin.line = token->Line;
                begin.column = token->Column;
                end.line = token->Line;
                end.column = token->Column + wcslen(token->Data);
              }
            else
              {
                int TokenCount = Grammar.RuleArray[token->ReductionRule].SymbolsCount;

                begin.line = token->Tokens[0]->Line;
                begin.column = token->Tokens[0]->Column;
                
                TokenStruct* last = token->Tokens[TokenCount - 1];
                end.line = last->Line;
                end.column = last->Column;
                //if (last->ReductionRule < 0)
                //    end.column += wcslen(last->Data);
              }

            target->begin.set_min(begin);
            target->end.set_max(end);
          }
      }

    str get_dsl(int index)
      {
        return dsl_text[index];
      }

    std::vector<str> dsl_text;
  };

//a base for visitors, allow binding of rules to local functions
template <typename T>
struct visitor_base
  {
    //registered rule handlers
    typedef void (T::* rule_handler)(TokenStruct*, parsetree_visitor*);
    typedef std::map<xs_rules, rule_handler>  handler_map;
    typedef std::pair<xs_rules, rule_handler> handler_pair;

    handler_map handlers_;

    void register_rule(xs_rules rule, rule_handler handler)
      {
        handlers_.insert( handler_pair(rule, handler) );
      }

    bool visit_rule(T* self, TokenStruct* token, parsetree_visitor* visitor)
      {
        typename handler_map::iterator it = handlers_.find( static_cast<xs_rules>(token->ReductionRule) );
        if (it != handlers_.end())
          {
            (self->*(it->second))(token, visitor);
            return true;
          }

        return false;
      }

    virtual ast_base* target()
      {
        return null;
      }
  };

//expressions
struct expression_;

struct parameters_ : visitor_base<parameters_>
  {
    parameters_(expression_& _expr);

    bool visit(TokenStruct* token, parsetree_visitor* visitor);

    //data
    expression_& expr;
    param_list   values;
    int          param_count;
    str          curr_name; 

    //rule handlers
    void named_argument( TokenStruct* token, parsetree_visitor* visitor);
    void argument_list( TokenStruct* token, parsetree_visitor* visitor);
    void empty_args( TokenStruct* token, parsetree_visitor* visitor);
  };

struct expr_list_ : visitor_base<expr_list_>
  {
    expr_list_(expression_& _expr);

    bool visit(TokenStruct* token, parsetree_visitor* visitor);

    //data
    expression_& expr;
    int          param_count;

    //rule handlers
    void single_expr( TokenStruct* token, parsetree_visitor* visitor);
    void expr_list( TokenStruct* token, parsetree_visitor* visitor);
  };

struct expression_ : visitor_base<expression_>
  {
    expression_( expression& ctx );

    bool visit(TokenStruct* token, parsetree_visitor* visitor);
    virtual ast_base* target()
      {
        return &ctx_;
      }

    //data
    expression& ctx_;

    void register_rules();

    void parameter_added(const str& name)
      {
        ctx_.push_identifier( name );
        ctx_.push_operator( op_parameter );
      }

    //actual handlers
    template <operator_type op>
    void binary_operator( TokenStruct* token, parsetree_visitor* visitor)
      {
        visitor->visit( token->Tokens[0], *this );
        visitor->visit( token->Tokens[2], *this );

        ctx_.push_operator( op );
      }

    template <operator_type op>
    void unary_operator( TokenStruct* token, parsetree_visitor* visitor)
      {
        visitor->visit( token->Tokens[1], *this );
        ctx_.push_operator( op );
      }

    template <operator_type op>
    void post_unary_operator( TokenStruct* token, parsetree_visitor* visitor)
      {
        ctx_.push_operator( op );
      };

    void index( TokenStruct* token, parsetree_visitor* visitor);
    void function_call( TokenStruct* token, parsetree_visitor* visitor);
    void member_call( TokenStruct* token, parsetree_visitor* visitor);
    void instantiate( TokenStruct* token, parsetree_visitor* visitor);
    void build_array( TokenStruct* token, parsetree_visitor* visitor);
    void object( TokenStruct* token, parsetree_visitor* visitor);
    void to_do( TokenStruct* token, parsetree_visitor* visitor);
  };

//types
struct type_ : visitor_base<type_>
  {
    type_(xs_type& output) : output_(output)
      {
        register_rule(rule_type_params, &type_::parametric );
        register_rule(rule_type,        &type_::simple );
      }

    bool visit(TokenStruct* token, parsetree_visitor* visitor);
    void parametric( TokenStruct* token, parsetree_visitor* visitor);
    void simple( TokenStruct* token, parsetree_visitor* visitor);

    xs_type& output_;
  };

//code
struct code_ : visitor_base<code_>
  {
    code_( code& output );

    bool visit(TokenStruct* token, parsetree_visitor* visitor);

    virtual ast_base* target()
      {
        return &output_;
      }

    void code_block( TokenStruct* token, parsetree_visitor* visitor);
    void statement_if( TokenStruct* token, parsetree_visitor* visitor);
    void statement_for( TokenStruct* token, parsetree_visitor* visitor);
    void statement_iter_for( TokenStruct* token, parsetree_visitor* visitor);
    void statement_while( TokenStruct* token, parsetree_visitor* visitor);
    void statement_switch( TokenStruct* token, parsetree_visitor* visitor);
    void statement_dispatch( TokenStruct* token, parsetree_visitor* visitor);
    void statement_break( TokenStruct* token, parsetree_visitor* visitor);
    void statement_continue( TokenStruct* token, parsetree_visitor* visitor);
    void statement_return( TokenStruct* token, parsetree_visitor* visitor);
    void statement_expression( TokenStruct* token, parsetree_visitor* visitor);
    void statement_try( TokenStruct* token, parsetree_visitor* visitor);
    void statement_throw( TokenStruct* token, parsetree_visitor* visitor);
    void declare_variable( TokenStruct* token, parsetree_visitor* visitor);
    void complete_dsl(TokenStruct* token, parsetree_visitor* visitor);
    void parameter_dsl( TokenStruct* token, parsetree_visitor* visitor);
    void simple_dsl( TokenStruct* token, parsetree_visitor* visitor);

    code& output_;
  };

struct if_ : visitor_base<if_>
  {
    if_(stmt_if& output);

    bool visit(TokenStruct* token, parsetree_visitor* visitor);

    void without_else( TokenStruct* token, parsetree_visitor* visitor);
    void wih_else( TokenStruct* token, parsetree_visitor* visitor);

    virtual ast_base* target()
      {
        return &output_;
      }

    stmt_if& output_;
  };

struct for_ : visitor_base<for_>
  {
    for_(stmt_for& output);

    bool visit(TokenStruct* token, parsetree_visitor* visitor);

    void init_var( TokenStruct* token, parsetree_visitor* visitor);

    virtual ast_base* target()
      {
        return &output_;
      }

    stmt_for& output_;
  };

struct iterfor_ : visitor_base<iterfor_>
  {
    iterfor_(stmt_iter_for& output);

    bool visit(TokenStruct* token, parsetree_visitor* visitor);

    virtual ast_base* target()
      {
        return &output_;
      }

    stmt_iter_for& output_;
  };

struct while_ : visitor_base<while_>
  {
    while_(stmt_while& output);

    bool visit(TokenStruct* token, parsetree_visitor* visitor);

    virtual ast_base* target()
      {
        return &output_;
      }

    stmt_while& output_;
  };

struct variable_ : visitor_base<variable_>
  {
    variable_(stmt_variable& output):
      output_(output)
      {
        register_rule(rule_varname,  &variable_::var_no_init );
        register_rule(rule_varinit,  &variable_::var_init );
      }

    bool visit(TokenStruct* token, parsetree_visitor* visitor);
    void var_no_init( TokenStruct* token, parsetree_visitor* visitor);
    void var_init( TokenStruct* token, parsetree_visitor* visitor);

    virtual ast_base* target()
      {
        return &output_;
      }

    stmt_variable& output_;
  };

struct switch_ : visitor_base<switch_>
  {
    switch_(stmt_switch& output):
      output_(output)
      {
        register_rule(rule_switch,         &switch_::main );
        register_rule(rule_switchcase,     &switch_::case_);
        register_rule(rule_switchdefault,  &switch_::default_);
        register_rule(rule_switchsection,  &switch_::section_);
      }

    bool visit(TokenStruct* token, parsetree_visitor* visitor);
    void main( TokenStruct* token, parsetree_visitor* visitor);
    void case_( TokenStruct* token, parsetree_visitor* visitor);
    void default_( TokenStruct* token, parsetree_visitor* visitor);
    void section_( TokenStruct* token, parsetree_visitor* visitor);

    virtual ast_base* target()
      {
        return &output_;
      }

    stmt_switch&            output_;
    std::vector<expression> cases_;
    bool                    has_default;
  };

//parameters_
parameters_::parameters_(expression_& _expr) :
  expr(_expr),
  param_count(0)
  {
    register_rule( rule_empty_args,     &parameters_::empty_args );
    register_rule( rule_argument_list,  &parameters_::argument_list );
    register_rule( rule_named_argument, &parameters_::named_argument );
  };

bool parameters_::visit(TokenStruct* token, parsetree_visitor* visitor)
  {
    if (!visit_rule(this, token, visitor))
      {
        //delegate on the expression
        param_count++;
        visitor->visit( token, expr);
        expr.parameter_added(str());
      }

    return true;
  };

void parameters_::named_argument( TokenStruct* token, parsetree_visitor* visitor)
  {
    //rule_named_argument = 73,       //<Argument> ::= Identifier '=' <Expression>
    TokenStruct* tpm = token->Tokens[0];        assert( tpm->ReductionRule < 0 );
    str param_name = wide2str( tpm->Data );     assert(!param_name.empty());

    expression  value;
    expression_ value_(value);
    visitor->visit( token->Tokens[2], value_);
    
    values.add(param_name, value);

    visitor->visit( token->Tokens[2], expr);
    expr.parameter_added(param_name);
    param_count++;
  }

void parameters_::argument_list( TokenStruct* token, parsetree_visitor* visitor)
  {
    visit( token->Tokens[0], visitor);
    visit( token->Tokens[2], visitor);
  }

void parameters_::empty_args( TokenStruct* token, parsetree_visitor* visitor)
  {
    param_count = 0;
  }

//expr_list_
expr_list_::expr_list_(expression_& _expr):
  expr(_expr),
  param_count(0)
  {
    register_rule( rule_single_expr, &expr_list_::single_expr );
    register_rule( rule_expr_list,   &expr_list_::expr_list );
  }

bool expr_list_::visit(TokenStruct* token, parsetree_visitor* visitor)
  {
    return visit_rule(this, token, visitor);
  }

void expr_list_::single_expr( TokenStruct* token, parsetree_visitor* visitor)
  {
    //<Expression List> ::= <Expression>
    visitor->visit( token->Tokens[0], expr);
    param_count++;
  }

void expr_list_::expr_list( TokenStruct* token, parsetree_visitor* visitor)
  {
    //<Expression List> ::= <Expression> ',' <Expression List>
    if (token->Tokens[2]->ReductionRule == rule_expr_list)
      visit( token->Tokens[2], visitor);
    else
      {
        visitor->visit( token->Tokens[2], expr);
        param_count++;
      }

    visitor->visit( token->Tokens[0], expr);
    param_count++;
  }

//expression_

expression_::expression_( expression& ctx )
  : ctx_(ctx)
  {
    register_rule(rule_assign,        &expression_::binary_operator<op_assign>);
    register_rule(rule_plusequal,     &expression_::binary_operator<op_plus_equal>);
    register_rule(rule_minusequal,    &expression_::binary_operator<op_minus_equal>);
    register_rule(rule_multequal,     &expression_::binary_operator<op_mult_equal>);
    register_rule(rule_divequal,      &expression_::binary_operator<op_div_equal>);
    register_rule(rule_shiftlequal,   &expression_::binary_operator<op_shift_left_equal>);
    register_rule(rule_shiftrequal,   &expression_::binary_operator<op_shift_right_equal>);
    register_rule(rule_or,            &expression_::binary_operator<op_or>);
    register_rule(rule_and,           &expression_::binary_operator<op_and>);
    register_rule(rule_equal,         &expression_::binary_operator<op_equal>);
    register_rule(rule_notequal,      &expression_::binary_operator<op_notequal>);
    register_rule(rule_lt,            &expression_::binary_operator<op_lt>);
    register_rule(rule_gt,            &expression_::binary_operator<op_gt>);
    register_rule(rule_le,            &expression_::binary_operator<op_le>);
    register_rule(rule_ge,            &expression_::binary_operator<op_ge>);
    register_rule(rule_typecheck,     &expression_::binary_operator<op_typecheck>);
		register_rule(rule_cast,				  &expression_::binary_operator<op_typecast>);
    register_rule(rule_namecheck,     &expression_::binary_operator<op_namecheck>);
    register_rule(rule_shiftleft,     &expression_::binary_operator<op_shift_left>);
    register_rule(rule_shiftright,    &expression_::binary_operator<op_shift_right>);
    register_rule(rule_add,           &expression_::binary_operator<op_plus>);
    register_rule(rule_sub,           &expression_::binary_operator<op_minus>);
    register_rule(rule_mult,          &expression_::binary_operator<op_mult>);
    register_rule(rule_div,           &expression_::binary_operator<op_divide>);
    register_rule(rule_mod,           &expression_::binary_operator<op_mod>);
    register_rule(rule_not,           &expression_::unary_operator<op_not>);
    register_rule(rule_unary_minus,   &expression_::unary_operator<op_unary_minus>);
    register_rule(rule_pre_inc,       &expression_::unary_operator<op_inc>);
    register_rule(rule_pre_dec,       &expression_::unary_operator<op_dec>);
    register_rule(rule_post_inc,      &expression_::post_unary_operator<op_inc>);
    register_rule(rule_pre_inc,       &expression_::unary_operator<op_inc>);
    register_rule(rule_post_dec,      &expression_::post_unary_operator<op_dec>);
    register_rule(rule_index,         &expression_::index);
    register_rule(rule_func_call,     &expression_::function_call);
    register_rule(rule_member_call,   &expression_::member_call);
    register_rule(rule_array,         &expression_::build_array);
    register_rule(rule_instantiation, &expression_::instantiate);
    register_rule(rule_object,        &expression_::object);
    register_rule(rule_conditional,   &expression_::to_do);
  }

bool expression_::visit(TokenStruct* token, parsetree_visitor* visitor)
  {
    if (token->ReductionRule < 0 )
      {
        assert(token->Data);

        variant operand;
        std::wstring val(token->Data);
        switch (token->Symbol)
          {
            case terminal_char_string:
            case terminal_string:
              {
                str res = wide2str( val );
                res.erase(0, 1);
                res.erase(res.size() - 1, 1);
                operand = res;
                break;
              }
            case terminal_int:          operand = boost::lexical_cast<int>( val ); break;
            case terminal_false:        operand = false; break;
            case terminal_hex:          operand = boost::lexical_cast<int>( val ); break;
            case terminal_identifier:
              {
                ctx_.push_identifier(wide2str( val ));
                return true;
              }
            case terminal_null:         break; //pass a null
            case terminal_float:        operand = boost::lexical_cast<float>( val ); break;
            case terminal_true:         operand = true; break;
            case terminal_member_name:
              {
                str member = wide2str( val );
                member.erase(0, 1);
                ctx_.push_identifier(member);
                ctx_.push_operator(op_dot);
                return true;
              }

            default: return true; //just ingnore?
          }

        ctx_.push_operand(operand);
        return true;
      }

    return visit_rule(this, token, visitor);
  }

void expression_::index( TokenStruct* token, parsetree_visitor* visitor)
  {
    expr_list_ args(*this);
    visitor->visit(token->Tokens[1], args);

    ctx_.push_operand( args.param_count );

    //visitor->visit( token->Tokens[1], *this );
    ctx_.push_operator( op_index );
  }

void expression_::function_call( TokenStruct* token, parsetree_visitor* visitor)
  {
    //grab the parameters
    parameters_ args(*this);
    visitor->visit(token->Tokens[2], args);

    //save info on the call
    //td: named parameters
    visitor->visit( token->Tokens[0], *this); //push our identifier
    ctx_.push_operand( args.param_count );
    ctx_.push_operator( op_func_call );
  }

void expression_::member_call( TokenStruct* token, parsetree_visitor* visitor)
  {
    visitor->visit( token->Tokens[0], *this); //identifier

    //grab the parameters
    parameters_ args(*this);
    visitor->visit(token->Tokens[2], args);

    //save info on the call
    //td: named parameters
    ctx_.push_operand( args.param_count );
    ctx_.push_operator( op_call );
  }

void expression_::instantiate( TokenStruct* token, parsetree_visitor* visitor)
  {
    // new <Type> '(' <Arg List Opt> ')'

    //grab type
    xs_type type;
    type_ tt(type);

    visitor->visit(token->Tokens[1], tt);

    //grab the parameters
    parameters_ args(*this);
    visitor->visit(token->Tokens[3], args);

    //save info on the call
    //td: named parameters
    ctx_.push_operand( type );
    ctx_.push_operand( args.param_count );
    ctx_.push_operator( op_instantiate );
  }

struct object_ : visitor_base<object_>
  {
    object_::object_()
      {
        register_rule(rule_object_value,   &object_::object_value);
      }

    std::vector<xs_const> values;

    bool visit(TokenStruct* token, parsetree_visitor* visitor)
      {
        return visit_rule(this, token, visitor);
      }

    void object_value( TokenStruct* token, parsetree_visitor* visitor)
      {
        xs_const result;
        result.name =  wide2str(token->Tokens[0]->Data);
        
        expression_ value(result.value);
        visitor->visit(token->Tokens[2], value);

        values.push_back(result);
      }
  };

void expression_::object( TokenStruct* token, parsetree_visitor* visitor)
  {
    object_ obj;
    visitor->visit(token->Tokens[1], obj);

    ctx_.push_operand( obj.values );
    ctx_.push_operator( op_object );
  }

void expression_::build_array( TokenStruct* token, parsetree_visitor* visitor)
  {
    parameters_ args(*this);
    visitor->visit(token->Tokens[1], args);

    ctx_.push_operand( args.param_count );
    ctx_.push_operator( op_array );
  }

void expression_::to_do( TokenStruct* token, parsetree_visitor* visitor)
  {
    assert( false );
  }

//qualified_id_
struct qualified_id_ : visitor_base<qualified_id_>
  {
    qualified_id_(std::vector<str>& output) : output_(output)
      {
        register_rule(rule_qualified_id, &qualified_id_::main );
        register_rule(rule_member_list,  &qualified_id_::member_list );
        register_rule(rule_member_empty, &qualified_id_::member_list_empty );
      }

    bool visit(TokenStruct* token, parsetree_visitor* visitor)
      {
        return visit_rule(this, token, visitor);
      }

    std::vector<str>& output_;

    void main( TokenStruct* token, parsetree_visitor* visitor)
      {
        output_.push_back( wide2str(token->Tokens[0]->Data) );
        visitor->visit(token->Tokens[1], *this);
      }

    void member_list( TokenStruct* token, parsetree_visitor* visitor)
      {
        visitor->visit(token->Tokens[0], *this);
        str member_name = wide2str(token->Tokens[1]->Data);
        member_name.erase(0, 1);
        output_.push_back(member_name);
      }

    void member_list_empty( TokenStruct* token, parsetree_visitor* visitor)
      {
      }
  };

struct type_args_ : visitor_base<type_args_>
  {
    type_args_(std::vector<xs_const>& output) : output_(output)
      {
        register_rule( rule_type_args,      &type_args_::argument_list );
        register_rule( rule_type_arg_named, &type_args_::named_argument );
      }

    bool visit(TokenStruct* token, parsetree_visitor* visitor)
      {
        if (!visit_rule(this, token, visitor))
          {
            xs_const result;
            expression_ expr(result.value);
            visitor->visit( token, expr);

            output_.push_back( result );
          }

        return true;
      }

    void argument_list( TokenStruct* token, parsetree_visitor* visitor)
      {
        visit( token->Tokens[0], visitor );
        visit( token->Tokens[2], visitor );
      }

    void named_argument( TokenStruct* token, parsetree_visitor* visitor)
      {
        xs_const result;

        result.name = wide2str( token->Tokens[0]->Data );

        expression_ expr(result.value);
        visitor->visit( token->Tokens[2], expr);

        output_.push_back( result );
      }

    std::vector<xs_const>& output_;
  };

struct try_ : visitor_base<try_>
  {
    try_(stmt_try& output) : output_(output)
      {
        register_rule( rule_try,        &try_::main );
        register_rule( rule_catch_any,  &try_::catch_any );
        register_rule( rule_catch_type, &try_::catch_type );
        register_rule( rule_catch_id,   &try_::catch_id );
        register_rule( rule_finally,    &try_::finally_ );
      }

    bool visit(TokenStruct* token, parsetree_visitor* visitor)
      {
        return visit_rule(this, token, visitor);
      }
    
    virtual ast_base* target()
      {
        return &output_;
      }

    void main( TokenStruct* token, parsetree_visitor* visitor)
      {
        //try <Block> <Catch Clauses> <Finally Clause Opt>
        code_ try_code(output_.try_code);
        visitor->visit(token->Tokens[1], try_code);

        visitor->visit(token->Tokens[2], *this);
        visitor->visit(token->Tokens[3], *this);
      }

    void catch_any( TokenStruct* token, parsetree_visitor* visitor)
      {
        //catch <Block>
        catch_ cc;
        code_  cc_(cc.catch_code);
        visitor->visit(token->Tokens[1], cc_);

        output_.catches.push_back(cc);
      }

    void catch_type( TokenStruct* token, parsetree_visitor* visitor)
      {
        //catch '(' <Qualified ID> ')' <Block>
        catch_ cc;
        type_  type(cc.type); 
        visitor->visit(token->Tokens[2], type);

        code_  cc_(cc.catch_code);
        visitor->visit(token->Tokens[4], cc_);

        output_.catches.push_back(cc);
      }

    void catch_id( TokenStruct* token, parsetree_visitor* visitor)
      {
        //catch '(' <Qualified ID> Identifier ')' <Block>
        catch_ cc;
        type_  type(cc.type); 
        visitor->visit(token->Tokens[2], type);

        cc.id = wide2str( token->Tokens[3]->Data );

        code_  cc_(cc.catch_code);
        visitor->visit(token->Tokens[5], cc_);

        output_.catches.push_back(cc);
      }

    void finally_( TokenStruct* token, parsetree_visitor* visitor)
      {
        code_  cc_(output_.finally_code);
        visitor->visit(token->Tokens[1], cc_);
      }

    stmt_try& output_;
  };
//type_
bool type_::visit(TokenStruct* token, parsetree_visitor* visitor)
  {
    if (!visit_rule(this, token, visitor))
      {
        //ought to be an identifier
        output_.name = wide2str(token->Data);
      }
    return true;
  }

void type_::parametric( TokenStruct* token, parsetree_visitor* visitor)
  {
    output_.name = wide2str(token->Tokens[0]->Data);

    type_args_ args(output_.args);
    visitor->visit(token->Tokens[2], args);
  }

void type_::simple( TokenStruct* token, parsetree_visitor* visitor)
  {
    output_.name = wide2str(token->Tokens[0]->Data);
  }

//code_
code_::code_( code& output ) :
  output_(output)
  {
    register_rule( rule_block,         &code_::code_block );
    register_rule( rule_if,            &code_::statement_if );
    register_rule( rule_ifelse,        &code_::statement_if );
    register_rule( rule_for,           &code_::statement_for );
    register_rule( rule_iterfor,       &code_::statement_iter_for );
    register_rule( rule_while,         &code_::statement_while );
    register_rule( rule_switch,        &code_::statement_switch );
    register_rule( rule_dispatch,      &code_::statement_dispatch );
    register_rule( rule_break,         &code_::statement_break );
    register_rule( rule_continue,      &code_::statement_continue );
    register_rule( rule_return,        &code_::statement_return );
    register_rule( rule_expr_stmt,     &code_::statement_expression );
    register_rule( rule_vardecl,       &code_::declare_variable );
    register_rule( rule_complete_dsl,  &code_::complete_dsl );
    register_rule( rule_parameter_dsl, &code_::parameter_dsl );
    register_rule( rule_simple_dsl,    &code_::simple_dsl );
    register_rule( rule_try,           &code_::statement_try );
    register_rule( rule_throw,         &code_::statement_throw );
  }

bool code_::visit(TokenStruct* token, parsetree_visitor* visitor)
  {
    return visit_rule(this, token, visitor);
  }

void code_::code_block( TokenStruct* token, parsetree_visitor* visitor)
  {
    visitor->visit(token->Tokens[1], *this);
  }

void code_::statement_if( TokenStruct* token, parsetree_visitor* visitor)
  {
    stmt_if si;
    if_     v(si);
    visitor->visit(token, v);

    si.begin.set_min(from_token(token->Tokens[0]));
    if (si.else_code.empty())
      si.end.set_max(si.if_code.end);
    else
      si.end.set_max(si.else_code.end);

    output_.add_statement( si );
  }

void code_::statement_for( TokenStruct* token, parsetree_visitor* visitor)
  {
    stmt_for sf;
    for_     f(sf);
    visitor->visit(token, f);

    sf.begin.set_min(from_token(token->Tokens[0]));
    sf.end.set_max(sf.for_code.end);

    output_.add_statement( sf );
  }

void code_::statement_iter_for( TokenStruct* token, parsetree_visitor* visitor)
  {
    stmt_iter_for sif;
    iterfor_      iif(sif);
    visitor->visit(token, iif);

    sif.begin.set_min(from_token(token->Tokens[0]));
    sif.end.set_max(sif.for_code.end);
    output_.add_statement( sif );
  }

void code_::statement_while( TokenStruct* token, parsetree_visitor* visitor)
  {
    stmt_while sw;
    while_     w(sw);
    visitor->visit(token, w);

    sw.begin.set_min(from_token(token->Tokens[0]));
    sw.end.set_max(sw.while_code.end);
    output_.add_statement( sw );
  }

void code_::statement_switch( TokenStruct* token, parsetree_visitor* visitor)
  {
    //<Statement> ::= switch '(' <Switch Header> ')' '{' <Switch Sections Opt> '}'
    
    stmt_switch ss;
    switch_ s(ss);
    visitor->visit(token, s);

    ss.begin.set_min(from_token(token->Tokens[0]));
    ss.end.set_max(from_token(token->Tokens[6]));

    output_.add_statement(ss);
  }

void code_::statement_dispatch( TokenStruct* token, parsetree_visitor* visitor)
  {
    //<Statement> ::= dispatch <Qualified ID> '(' <Arg List Opt> ')' ';'
    stmt_dispatch sd;

    qualified_id_ qi(sd.target);
    visitor->visit(token->Tokens[1], qi);

    expression_ args_expr(sd.args);
    parameters_ args(args_expr);
    visitor->visit(token->Tokens[3], args);
    sd.arg_count = args.param_count;

    sd.begin.set_min(from_token(token->Tokens[0]));
    sd.end.set_max(from_token(token->Tokens[5]));
    output_.add_statement( sd );
  }

void code_::statement_break( TokenStruct* token, parsetree_visitor* visitor)
  {
    stmt_break sb;

    sb.begin.set_min(from_token(token->Tokens[0]));
    sb.end.set_max(from_token(token->Tokens[1]));
    output_.add_statement( sb );
  }

void code_::statement_continue( TokenStruct* token, parsetree_visitor* visitor)
  {
    stmt_continue sc;
    sc.begin.set_min(from_token(token->Tokens[0]));
    sc.end.set_max(from_token(token->Tokens[1]));

    output_.add_statement( sc );
  }

void code_::statement_return( TokenStruct* token, parsetree_visitor* visitor)
  {
    stmt_return sr;
    expression_ expr(sr.expr);
    visitor->visit(token->Tokens[1], expr);

    sr.begin.set_min(from_token(token->Tokens[0]));
    sr.end.set_max(from_token(token->Tokens[2]));
    output_.add_statement( sr );
  }

void code_::statement_expression( TokenStruct* token, parsetree_visitor* visitor)
  {
    stmt_expression se;
    expression_ expr(se.expr);
    visitor->visit(token, expr);

    se.begin.set_min(from_token(token->Tokens[0]));
    se.end.set_max(from_token(token->Tokens[1]));
    output_.add_statement( se );
  }

void code_::statement_try( TokenStruct* token, parsetree_visitor* visitor)
  {
    // try <Block> <Catch Clauses> <Finally Clause Opt>
    stmt_try ts;
    try_     t(ts);
    visitor->visit(token, t);

    ts.begin.set_min(from_token(token->Tokens[0]));
    ts.end.set_max(ts.finally_code.end);

    output_.add_statement( ts );
  }

void code_::statement_throw( TokenStruct* token, parsetree_visitor* visitor)
  {
    stmt_throw  tw;
    expression_ e(tw.expr);
    visitor->visit(token->Tokens[1], e);

    tw.begin.set_min(from_token(token->Tokens[0]));
    tw.end.set_max(from_token(token->Tokens[2]));

    output_.add_statement( tw );
  }

void code_::declare_variable( TokenStruct* token, parsetree_visitor* visitor)
  {
    stmt_variable sv;
    variable_     v(sv);
    visitor->visit(token, v);

    sv.begin.set_min(from_token(token->Tokens[0]));
    sv.end.set_max(sv.value.end);

    output_.add_statement( sv );
  }

void code_::complete_dsl(TokenStruct* token, parsetree_visitor* visitor)
  {
    dsl result;
    result.name = wide2str(token->Tokens[0]->Data);
    result.name.erase(0, 2);
    result.id   = wide2str(token->Tokens[1]->Data);

    expression_ arg_exp(result.param_expr);
    parameters_ args(arg_exp);
    visitor->visit(token->Tokens[3], args);
    result.param_count = args.param_count;

    int text_index = boost::lexical_cast<int>( token->Tokens[6]->Data );

    result.text = visitor->get_dsl(text_index);

    output_.add_statement( result );
  }

void code_::parameter_dsl( TokenStruct* token, parsetree_visitor* visitor)
  {
    dsl result;
    result.name = wide2str(token->Tokens[0]->Data);
    result.name.erase(0, 2);

    expression_ arg_exp(result.param_expr);
    parameters_ args(arg_exp);
    visitor->visit(token->Tokens[2], args);

    result.params      = args.values;
    result.param_count = args.param_count;

    int text_index = boost::lexical_cast<int>( token->Tokens[5]->Data );

    result.text = visitor->get_dsl(text_index);

    output_.add_statement( result );
  }

void code_::simple_dsl( TokenStruct* token, parsetree_visitor* visitor)
  {
    dsl result;
    result.name = wide2str(token->Tokens[0]->Data);
    result.name.erase(0, 2);

    int text_index = boost::lexical_cast<int>( token->Tokens[5]->Data );
    result.text = visitor->get_dsl(text_index);

    output_.add_statement( result );
  }

//if_
if_::if_(stmt_if& output) :
  output_(output)
  {
    register_rule( rule_if,     &if_::without_else );
    register_rule( rule_ifelse, &if_::wih_else );
  }

bool if_::visit(TokenStruct* token, parsetree_visitor* visitor)
  {
    return visit_rule(this, token, visitor);
  }

void if_::without_else( TokenStruct* token, parsetree_visitor* visitor)
  {
    expression_ expr(output_.expr);
    visitor->visit(token->Tokens[2], expr);

    code_ if_code(output_.if_code);
    visitor->visit(token->Tokens[4], if_code);
  }

void if_::wih_else( TokenStruct* token, parsetree_visitor* visitor)
  {
    expression_ expr(output_.expr);
    visitor->visit(token->Tokens[2], expr);

    code_ if_code(output_.if_code);
    visitor->visit(token->Tokens[4], if_code);

    code_ else_code(output_.else_code);
    visitor->visit(token->Tokens[6], else_code);
  }

//for_
for_::for_(stmt_for& output):
  output_(output)
  {
    register_rule(rule_vardecl, &for_::init_var );
  }

bool for_::visit(TokenStruct* token, parsetree_visitor* visitor)
  {
    if (!visit_rule(this, token->Tokens[2], visitor))
      {
        expression_ init_expr(output_.init_expr);
        visitor->visit(token->Tokens[2], init_expr);
      }

    expression_ cond_expr(output_.cond_expr);
    visitor->visit(token->Tokens[4], cond_expr);

    expression_ iter_expr(output_.iter_expr);
    visitor->visit(token->Tokens[6], iter_expr);

    code_ for_code(output_.for_code);
    visitor->visit(token->Tokens[8], for_code);

    return true;
  }

void for_::init_var( TokenStruct* token, parsetree_visitor* visitor)
  {
    variable_     v(output_.init_variable);
    visitor->visit(token, v);
  }

//iterfor_
iterfor_::iterfor_(stmt_iter_for& output):
  output_(output)
  {
  }

bool iterfor_::visit(TokenStruct* token, parsetree_visitor* visitor)
  {
    //for '(' <Type> Identifier in <Expression> ')' <Statement>

    type_ type(output_.type);
    visitor->visit(token->Tokens[2], type);
    output_.id   = wide2str( token->Tokens[3]->Data );

    expression_ iter_expr(output_.iter_expr);
    visitor->visit(token->Tokens[5], iter_expr);

    code_ for_code(output_.for_code);
    visitor->visit(token->Tokens[7], for_code);
    return true;
  }

//while_
while_::while_(stmt_while& output):
  output_(output)
  {
  }

bool while_::visit(TokenStruct* token, parsetree_visitor* visitor)
  {
    expression_ expr(output_.expr);
    visitor->visit(token->Tokens[2], expr);

    code_ while_code(output_.while_code);
    visitor->visit(token->Tokens[4], while_code);
    return true;
  }

//variable_
bool variable_::visit(TokenStruct* token, parsetree_visitor* visitor)
  {
    output_.type = wide2str( token->Tokens[0]->Tokens[0]->Data );

    return visit_rule(this, token->Tokens[1], visitor);
  }

void variable_::var_no_init( TokenStruct* token, parsetree_visitor* visitor)
  {
    output_.id = wide2str( token->Tokens[0]->Data );
  }

void variable_::var_init( TokenStruct* token, parsetree_visitor* visitor)
  {
    output_.id = wide2str( token->Tokens[0]->Data );

    expression_ expr(output_.value);
    visitor->visit(token->Tokens[2], expr);
  }

//switch_
bool switch_::visit(TokenStruct* token, parsetree_visitor* visitor)
  {
    return visit_rule(this, token, visitor);
  }

void switch_::main( TokenStruct* token, parsetree_visitor* visitor)
  {
    //switch '(' <Switch Header> ')' '{' <Switch Sections Opt> '}'
    expression_ expr(output_.expr);
    visitor->visit(token->Tokens[2], expr);
    visitor->visit(token->Tokens[5], *this);
  }

void switch_::case_( TokenStruct* token, parsetree_visitor* visitor)
  {
    //case <Expression> ':'
    expression  e;
    expression_ expr(e);
    visitor->visit(token->Tokens[1], expr);
    cases_.push_back(e);
  }

void switch_::default_( TokenStruct* token, parsetree_visitor* visitor)
  {
    has_default = true;
  }

void switch_::section_( TokenStruct* token, parsetree_visitor* visitor)
  {
    //<Switch Labels> <Stm List>
    cases_.clear();
    has_default = false;

    visitor->visit(token->Tokens[0], *this);

    if (!has_default)
      {
        switch_section result;
        result.cases = cases_;

        code_ sc(result.case_code);
        visitor->visit(token->Tokens[1], sc);

        output_.sections.push_back(result);
      }
    else
      {
        code_ sc(output_.default_code);
        visitor->visit(token->Tokens[1], sc);
      }
  }

//high level visitors
struct param_decl_ : visitor_base<param_decl_>
  {
    param_decl_(param_list_decl& output) : output_(output)
      {
      }

    bool visit(TokenStruct* token, parsetree_visitor* visitor)
      {
        switch (token->ReductionRule)
          {
            case rule_plist_empty: break;
            case rule_plist_decl:
              {
                visit(token->Tokens[0], visitor);
                visit(token->Tokens[2], visitor);
                break;
              }
            case rule_default_arg:
              {
                param_decl pd;
                pd.type = wide2str(token->Tokens[0]->Data);
                pd.name = wide2str(token->Tokens[1]->Data);

                expression_ dv(pd.default_value);
                visitor->visit(token->Tokens[3], dv);
                output_.push_back( pd);
                break;
              }
            case rule_arg:
              {
                param_decl pd;
                pd.type = wide2str(token->Tokens[0]->Tokens[0]->Data);
                pd.name = wide2str(token->Tokens[1]->Data);

                output_.push_back( pd);
                break;
              }
            default:
              {
                //ought to be a identifier
                param_decl pd;
                pd.name = wide2str( token->Tokens[0]->Data );
                output_.push_back( pd);
              }
          }
        return true;
      }

    param_list_decl& output_;
  };

struct opt_type_ : visitor_base<opt_type_>
  {
    opt_type_()
      {
        register_rule(rule_typed, &opt_type_::main );
      }

    bool visit(TokenStruct* token, parsetree_visitor* visitor)
      {
        return visit_rule(this, token, visitor);
      }

    void main( TokenStruct* token, parsetree_visitor* visitor)
      {
        type_ = wide2str(token->Tokens[1]->Tokens[0]->Data);
      }

    str type_;
  };

struct property_ : visitor_base<property_>
  {
    property_(xs_property& output) : output_(output)
      {
        register_rule(rule_property,        &property_::main );
        register_rule(rule_var_property,    &property_::as_var );
        register_rule(rule_block,           &property_::set_only );
        register_rule(rule_prop_get_set,    &property_::get_set );
        register_rule(rule_prop_get,        &property_::get_only );
        register_rule(rule_prop_value,      &property_::value );
        register_rule(rule_prop_value_set,  &property_::value_set );
      }

    bool visit(TokenStruct* token, parsetree_visitor* visitor)
      {
        return visit_rule(this, token, visitor);
      }

    virtual ast_base* target()
      {
        return &output_;
      }

    xs_property& output_;

    void main( TokenStruct* token, parsetree_visitor* visitor)
      {
        output_.name = wide2str(token->Tokens[1]->Data);

        opt_type_ tp;
        visitor->visit(token->Tokens[2], tp);
        output_.type = tp.type_;

        visitor->visit(token->Tokens[3], *this);
      }

    void as_var( TokenStruct* token, parsetree_visitor* visitor)
      {
        stmt_variable sv;
        variable_     v(sv);
        visitor->visit(token->Tokens[0], v);

        output_.name  = sv.id;
        output_.type  = sv.type;
        output_.value = sv.value;
      }

    void set_only( TokenStruct* token, parsetree_visitor* visitor)
      {
        code_ cd(output_.set);
        visitor->visit(token, cd);
      }

    void get_set( TokenStruct* token, parsetree_visitor* visitor)
      {
        code_ cd_get(output_.get);
        code_ cd_set(output_.set);
        visitor->visit(token->Tokens[1], cd_get);
        visitor->visit(token->Tokens[2], cd_set);
      }

    void get_only( TokenStruct* token, parsetree_visitor* visitor)
      {
        code_ cd_get(output_.get);
        visitor->visit(token->Tokens[1], cd_get);
      }

    void value( TokenStruct* token, parsetree_visitor* visitor)
      {
        expression_ e(output_.value);
        visitor->visit(token->Tokens[1], e);
      }

    void value_set( TokenStruct* token, parsetree_visitor* visitor)
      {
        expression_ e(output_.value);
        visitor->visit(token->Tokens[1], e);

        code_ set(output_.set);
        visitor->visit(token->Tokens[2], set);
      }

  };

struct method_ : visitor_base<method_>
  {
    method_(xs_method& output) : output_(output)
      {
        register_rule(rule_method, &method_::main );
        register_rule(rule_method_c, &method_::main_c );
      }

    bool visit(TokenStruct* token, parsetree_visitor* visitor)
      {
        return visit_rule(this, token, visitor);
      }

    virtual ast_base* target()
      {
        return &output_;
      }

    void main( TokenStruct* token, parsetree_visitor* visitor)
      {
        output_.name = wide2str(token->Tokens[1]->Data);

        param_decl_ pd(output_.args);
        visitor->visit(token->Tokens[3], pd);

        opt_type_ tp;
        visitor->visit(token->Tokens[5], tp);
        output_.type = tp.type_;

        code_ cd(output_.cde);
        visitor->visit(token->Tokens[6], cd);
      }

    void main_c( TokenStruct* token, parsetree_visitor* visitor)
      {
        output_.type = wide2str(token->Tokens[0]->Tokens[0]->Data);
        output_.name = wide2str(token->Tokens[1]->Data);

        param_decl_ pd(output_.args);
        visitor->visit(token->Tokens[3], pd);

        code_ cd(output_.cde);
        visitor->visit(token->Tokens[5], cd);
      }

    xs_method& output_;
  };

struct adjetive_ : visitor_base<adjetive_>
  {
    adjetive_() : adjetives(0) {}

    bool visit(TokenStruct* token, parsetree_visitor* visitor)
      {
        if (token->ReductionRule < 0)
          {
            switch (token->Symbol)
              {
                case terminal_public   : adjetives |= adj_public; break;
                case terminal_private  : adjetives |= adj_private; break;
                case terminal_delegate : adjetives |= adj_delegate; break;
                default: assert(false);
              }
          }

        return false;
      }

    int adjetives;
  };

struct class_args_ : visitor_base<class_args_>
  {
    class_args_(std::vector<xs_const>& output) : output_(output)
      {
        register_rule( rule_class_no_args, &class_args_::no_args );
        register_rule( rule_class_plist,   &class_args_::argument_list );
        register_rule( rule_class_def_arg, &class_args_::named_argument );
      }

    bool visit(TokenStruct* token, parsetree_visitor* visitor)
      {
        if (!visit_rule(this, token, visitor))
          {
            xs_const result;
            expression_ expr(result.value);
            visitor->visit( token, expr);

            output_.push_back( result );
          }

        return true;
      }

    void no_args( TokenStruct* token, parsetree_visitor* visitor)
      {
      }

    void argument_list( TokenStruct* token, parsetree_visitor* visitor)
      {
        visit( token->Tokens[0], visitor);
        visit( token->Tokens[2], visitor);
      }

    void named_argument( TokenStruct* token, parsetree_visitor* visitor)
      {
        xs_const result;

        result.name = wide2str( token->Tokens[0]->Data );

        expression_ expr(result.value);
        visitor->visit( token->Tokens[2], expr);

        output_.push_back( result );
      }

    std::vector<xs_const>& output_;
  };

struct xs_ : visitor_base<xs_>
  {
    xs_(xs_container& output) : output_(output)
      {
        register_rule(rule_adj_property,    &xs_::adj_property );
        register_rule(rule_adj_method,      &xs_::adj_method );
        register_rule(rule_instance,        &xs_::instance );
        register_rule(rule_class,           &xs_::class_ );
        register_rule(rule_behaviour,       &xs_::behaviour_ );
        register_rule(rule_event,           &xs_::event_ );
        register_rule(rule_const,           &xs_::const_ );
        register_rule(rule_event_decl,      &xs_::event_decl );
        register_rule(rule_property,        &xs_::add_property );
        register_rule(rule_var_property,    &xs_::add_property );
        register_rule(rule_method,          &xs_::add_method );
        register_rule(rule_method_c,        &xs_::add_method );
        register_rule( rule_complete_dsl,   &xs_::complete_dsl );
        register_rule( rule_parameter_dsl,  &xs_::parameter_dsl );
        register_rule( rule_simple_dsl,     &xs_::simple_dsl );
        register_rule( rule_behaveas,       &xs_::behaveas );
        register_rule( rule_behaveas_empty, &xs_::behaveas_simple );
      }

    bool visit(TokenStruct* token, parsetree_visitor* visitor)
      {
        return visit_rule(this, token, visitor);
      }

    virtual ast_base* target()
      {
        return &output_;
      }

    void adj_property( TokenStruct* token, parsetree_visitor* visitor)
      {
        xs_property xp;

        adjetive_ adj;
        visitor->visit(token->Tokens[0], adj);
        xp.adjetives = adj.adjetives;

        property_   p(xp);
        visitor->visit(token->Tokens[1], p);

        output_.add( xp );
      }

    void adj_method( TokenStruct* token, parsetree_visitor* visitor)
      {
        xs_method xm;
        adjetive_ adj;
        visitor->visit(token->Tokens[0], adj);
        xm.adjetives = adj.adjetives;

        method_   m(xm);
        visitor->visit(token->Tokens[1], m);

        output_.add( xm );
      }

    void instance( TokenStruct* token, parsetree_visitor* visitor)
      {
        xs_instance   xi;
        qualified_id_ qi(xi.id);
        visitor->visit(token->Tokens[1], qi);

        opt_type_ tp;
        visitor->visit(token->Tokens[2], tp);
        xi.class_name = tp.type_;

        xs_ c(xi);
        visitor->visit(token->Tokens[3], c);

        output_.add( xi );
      }

    void class_( TokenStruct* token, parsetree_visitor* visitor)
      {
        xs_class xc;
        xc.name = wide2str(token->Tokens[1]->Data);

        class_args_ ca(xc.args);
        visitor->visit(token->Tokens[2], ca);

        opt_type_ tp;
        visitor->visit(token->Tokens[3], tp);
        xc.super = tp.type_;

        xs_ c(xc);
        visitor->visit(token->Tokens[4], c);

        output_.add( xc );
      }

    void behaviour_( TokenStruct* token, parsetree_visitor* visitor)
      {
        xs_behaviour xb;
        xb.name = wide2str( token->Tokens[1]->Data );

        opt_type_ tp;
        visitor->visit(token->Tokens[2], tp);
        xb.super = tp.type_;

        xs_ c(xb);
        visitor->visit(token->Tokens[3], c);

        output_.add( xb );
      }

    void event_( TokenStruct* token, parsetree_visitor* visitor)
      {
        xs_event xe;
        qualified_id_ qi(xe.name);
        visitor->visit(token->Tokens[1], qi);

        param_decl_ pd(xe.args);
        visitor->visit(token->Tokens[3], pd);

        code_ cd(xe.cde);
        visitor->visit(token->Tokens[5], cd);

        output_.add( xe );
      }

    void const_( TokenStruct* token, parsetree_visitor* visitor)
      {
        xs_const xc;
        xc.name = wide2str(token->Tokens[1]->Data);

        expression_ ce(xc.value);
        visitor->visit(token->Tokens[3], ce);

        xc.begin = from_token(token->Tokens[1]);
        xc.end   = from_token(token->Tokens[4]);
        output_.add( xc );
      }

    void event_decl( TokenStruct* token, parsetree_visitor* visitor)
      {
        xs_event_decl xe;
        xe.name = wide2str(token->Tokens[1]->Data);

        param_decl_ pd(xe.args);
        visitor->visit(token->Tokens[3], pd);

        output_.add( xe );
      }

    void add_property( TokenStruct* token, parsetree_visitor* visitor)
      {
        xs_property xp;
        property_   p(xp);
        visitor->visit(token, p);

        output_.add( xp );
      }

    void add_method( TokenStruct* token, parsetree_visitor* visitor)
      {
        xs_method xm;
        method_   m(xm);
        visitor->visit(token, m);

        output_.add( xm );
      }

    void complete_dsl( TokenStruct* token, parsetree_visitor* visitor)
      {
        dsl result;
        result.name = wide2str(token->Tokens[0]->Data);
        result.name.erase(0, 2);
        result.id   = wide2str(token->Tokens[1]->Data);

        expression_ arg_exp(result.param_expr);
        parameters_ args(arg_exp);
        visitor->visit(token->Tokens[3], args);
        result.param_count = args.param_count;

        int text_index = boost::lexical_cast<int>( token->Tokens[6]->Data );

        result.text = visitor->get_dsl(text_index);

        output_.add( result );
      }

    void parameter_dsl( TokenStruct* token, parsetree_visitor* visitor)
      {
        dsl result;
        result.name = wide2str(token->Tokens[0]->Data);
        result.name.erase(0, 2);

        expression_ arg_exp(result.param_expr);
        parameters_ args(arg_exp);
        visitor->visit(token->Tokens[2], args);
        result.param_count = args.param_count;

        int text_index = boost::lexical_cast<int>( token->Tokens[5]->Data );

        result.text = visitor->get_dsl(text_index);

        output_.add( result );
      }

    void simple_dsl( TokenStruct* token, parsetree_visitor* visitor)
      {
        dsl result;
        result.name = wide2str(token->Tokens[0]->Data);
        result.name.erase(0, 2);

        int text_index = boost::lexical_cast<int>( token->Tokens[5]->Data );
        result.text = visitor->get_dsl(text_index);

        output_.add( result );
      }

    void behaveas_simple( TokenStruct* token, parsetree_visitor* visitor)
      {
        xs_implement_behaviour result;

        result.name = wide2str(token->Tokens[2]->Data);

        output_.add( result );
      }

    void behaveas( TokenStruct* token, parsetree_visitor* visitor)
      {
        xs_implement_behaviour result;

        result.name = wide2str(token->Tokens[2]->Data);

        code_ cde(result.cde);
        visitor->visit(token->Tokens[3], cde);

        output_.add( result );
      }

    xs_container& output_;
  };

// validate parse's errors
const unsigned int head_parse_errors_count = 7;
const str parser_errors_head[head_parse_errors_count] = 
  {
    "",
    "Lexical error", 
    "Tokenizer error", 
    "Syntax error", 
    "Comment error", 
    "Out of memory", 
    ""
  };

const unsigned int descrip_parse_errors_count = 7;
const str parser_errors_descrip[descrip_parse_errors_count] = 
  {
    "",
    "The grammar does not specify what to do with '%s'.",
    "The tokenizer returned a non-terminal.",
    "Encountered '%s', but expected %s.",
    "The comment has no end, it was started but not finished.",
    "",
    ""
  };

enum xs_smallrules
  {
    smallrule_none            = 0,
    smallrule_parameters      = 1,
    smallrule_expression      = 2,
    smallrule_type            = 3,
    smallrule_code            = 4,
    smallrule_if              = 5,
    smallrule_for             = 6,
    smallrule_iter_for        = 7,
    smallrule_while           = 8,
    smallrule_variable        = 9,
    smallrule_switch          = 10,
    smallrule_try             = 11,
    smallrule_throw           = 12,
    smallrule_class           = 13,
    smallrule_property        = 14,
    smallrule_instance        = 15,
    smallrule_event           = 16
  };

int symbol_rules[150] = {
  /*   0  "EOF"                     */    0,
  /*   1  "Error"                   */    0,
  /*   2  "Whitespace"              */    0,
  /*   3  "Comment End"             */    0,
  /*   4  "Comment Line"            */    0,
  /*   5  "Comment Start"           */    0,
  /*   6  "-"                       */    2,
  /*   7  "--"                      */    2,
  /*   8  "!"                       */    2,
  /*   9  "!="                      */    2,
  /*  10  "%"                       */    2,
  /*  11  "&&"                      */    2,
  /*  12  "("                       */    0,
  /*  13  ")"                       */    0,
  /*  14  "*"                       */    2,
  /*  15  "*="                      */    2,
  /*  16  ","                       */    1,
  /*  17  "/"                       */    2,
  /*  18  "/="                      */    2,
  /*  19  ":"                       */    1,
  /*  20  ";"                       */    0,
  /*  21  "?"                       */    2,
  /*  22  "@"                       */    -1,
  /*  23  "["                       */    2,
  /*  24  "]"                       */    2,
  /*  25  "{"                       */    -2,
  /*  26  "||"                      */    2,
  /*  27  "}"                       */    -2,
  /*  28  "+"                       */    2,
  /*  29  "++"                      */    2,
  /*  30  "+="                      */    2,
  /*  31  "<"                       */    2,
  /*  32  "<<"                      */    2,
  /*  33  "<<="                     */    2,
  /*  34  "<="                      */    2,
  /*  35  "="                       */    2,
  /*  36  "-="                      */    2,
  /*  37  "=="                      */    2,
  /*  38  ">"                       */    2,
  /*  39  "->"                      */    2,
  /*  40  ">="                      */    2,
  /*  41  ">>"                      */    2,
  /*  42  ">>="                     */    2,
  /*  43  "as"                      */    2,
  /*  44  "behave"                  */    -1,
  /*  45  "behaviour"               */    -1,
  /*  46  "break"                   */    0,
  /*  47  "case"                    */    10,
  /*  48  "catch"                   */    11,
  /*  49  "CharLiteral"             */    2,
  /*  50  "class"                   */    13,
  /*  51  "const"                   */    -1,
  /*  52  "continue"                */    0,
  /*  53  "DecLiteral"              */    2,
  /*  54  "default"                 */    11,
  /*  55  "delegate"                */    -1,
  /*  56  "dispatch"                */    -1,
  /*  57  "DSLIdentifier"           */    -1,
  /*  58  "else"                    */    5,
  /*  59  "event"                   */    -1,
  /*  60  "false"                   */    2,
  /*  61  "finally"                 */    11,
  /*  62  "for"                     */    6,
  /*  63  "function"                */    -1,
  /*  64  "has"                     */    2,
  /*  65  "HexLiteral"              */    2,
  /*  66  "Identifier"              */    2,
  /*  67  "if"                      */    5,
  /*  68  "in"                      */    7,
  /*  69  "instance"                */    15,
  /*  70  "is"                      */    2,
  /*  71  "MemberName"              */    2,
  /*  72  "method"                  */    -1,
  /*  73  "new"                     */    2,
  /*  74  "null"                    */    2,
  /*  75  "on"                      */    16,
  /*  76  "private"                 */    -1,
  /*  77  "property"                */    14,
  /*  78  "public"                  */    -1,
  /*  79  "RealLiteral"             */    2,
  /*  80  "return"                  */    2,
  /*  81  "StringLiteral"           */    2,
  /*  82  "switch"                  */    10,
  /*  83  "throw"                   */    12,
  /*  84  "true"                    */    2,
  /*  85  "try"                     */    11,
  /*  86  "while"                   */    8,
  /*  87  "Add Exp"                 */    2,
  /*  88  "Adj List"                */    -1,
  /*  89  "Adjetive"                */    -1,
  /*  90  "And Exp"                 */    2,
  /*  91  "Arg Decl List"           */    1,
  /*  92  "Arg Decl List Opt"       */    1,
  /*  93  "Arg List"                */    1,
  /*  94  "Arg List Opt"            */    1,
  /*  95  "Argument"                */    1,
  /*  96  "Argument Decl"           */    1,
  /*  97  "Array Literal"           */    2,
  /*  98  "Block"                   */    -2,
  /*  99  "Catch Clause"            */    11,
  /* 100  "Catch Clauses"           */    11,
  /* 101  "Class Arg Decl List"     */    13,
  /* 102  "Class Arg List Opt"      */    13,
  /* 103  "Class Argument Decl"     */    13,
  /* 104  "Compare Exp"             */    2,
  /* 105  "Conditional Exp"         */    2,
  /* 106  "Construct"               */    -2,
  /* 107  "Construct Block"         */    -2,
  /* 108  "Construct List"          */    -2,
  /* 109  "DSL"                     */    -1,
  /* 110  "Equality Exp"            */    2,
  /* 111  "Expression"              */    2,
  /* 112  "Expression List"         */    2,
  /* 113  "Expression Opt"          */    2,
  /* 114  "Finally Clause Opt"      */    11,
  /* 115  "For Condition Opt"       */    6,
  /* 116  "For Init Opt"            */    6,
  /* 117  "For Iterator Opt"        */    6,
  /* 118  "Literal"                 */    2,
  /* 119  "Local Var Decl"          */    9,
  /* 120  "Member List"             */    2,
  /* 121  "Method"                  */    2,
  /* 122  "Method Decl"             */    -1,
  /* 123  "Method Exp"              */    2,
  /* 124  "Method Name"             */    -1,
  /* 125  "Mult Exp"                */    2,
  /* 126  "Object List"             */    2,
  /* 127  "Object List Opt"         */    2,
  /* 128  "Object Value"            */    2,
  /* 129  "Or Exp"                  */    2,
  /* 130  "Primary"                 */    2,
  /* 131  "Primary Exp"             */    2,
  /* 132  "Program"                 */    0,
  /* 133  "Prop Descriptor"         */    14,
  /* 134  "Property"                */    14,
  /* 135  "Qualified ID"            */    2,
  /* 136  "Shift Exp"               */    2,
  /* 137  "Statement"               */    -3,
  /* 138  "Stm List"                */    -3,
  /* 139  "Switch Header"           */    10,
  /* 140  "Switch Label"            */    10,
  /* 141  "Switch Labels"           */    10,
  /* 142  "Switch Section"          */    10,
  /* 143  "Switch Sections Opt"     */    10,
  /* 144  "Type"                    */    3,
  /* 145  "Type Arg List"           */    3,
  /* 146  "Type Argument"           */    3,
  /* 147  "Type Opt"                */    3,
  /* 148  "Unary Exp"               */    2,
  /* 149  "Variable Declarator"     */    9
  };

struct perrors_validate
  {
    perrors_validate(int result, TokenStruct *root, TokenStackStruct *stack) : 
      parse_result(result),
      descrip_ecode(result),
      head_ecode(result),
      token_stack(stack),
      last_expr_state(0),
      token(root),
      column(0),
      line(0)
      {
        // do nothing
        if (parse_result == PARSEACCEPT) return;

        // trap another use cases
        assert(head_ecode < head_parse_errors_count);

        process_error();
      }

    void process_error()
      {
        // why is NULL?
        assert(token != NULL);
        assert(token_stack != NULL);

        int idx_lalr  = token->Symbol;
        line          = token->Line;
        column        = token->Column;

        // more stuff...
        rule_error = find_rule(token_stack);
        
        if (token->Data != NULL)
          token_data = wide2str(std::wstring(token->Data, 1024));

        if (parse_result != PARSESYNTAXERROR)
          return;

        assert(idx_lalr < Grammar.LalrStateCount);

        // get expected tokens in rule exploited
        LalrStateStruct &refLalrState = Grammar.LalrArray[idx_lalr];
        for(int i = 0; i < refLalrState.ActionCount; ++i)
          {
            int symbol = refLalrState.Actions[i].Entry;
            if (Grammar.SymbolArray[symbol].Kind == SYMBOLTERMINAL)
              {
                str symbol_name = wide2str(Grammar.SymbolArray[symbol].Name);
                expected_tokens.push_back(symbol_name);
              }
          }
      }

    int find_rule(TokenStackStruct *stack)
      {
        if (stack == NULL)
          return smallrule_none;

        TokenStruct *token = stack->Token;
        int small_rule = symbol_rules[token->Symbol];

        // locate last identifier or expression
        if (last_expr_state < 2)
          {
            if (token->Symbol == terminal_member_name)
              {
                str member = wide2str( token->Data );
                member.erase(0, 1);
                last_expression.push_identifier(member);
                last_expression.push_operator(op_dot);
                last_expr_state = 1;
              }
            else
            if (token->Symbol == terminal_identifier)
              {
                last_expression.push_identifier( wide2str(token->Data) );
                last_expr_state = 1;
              }
            else
            if (token->ReductionRule > -1 && small_rule == smallrule_expression)
              {
                parsetree_visitor v;
                expression_ ev(last_expression);
                v.visit(token, ev);
                last_expr_state = 2;
              }
          }
        else
        if (last_expr_state == 1)
          {
            last_expr_state = 2;
          }

        switch (small_rule)
          {
            case smallrule_none:
              {
                return find_rule(stack->NextToken);
              }

            case smallrule_expression:
            case smallrule_parameters:
            case smallrule_type:
              {
                int res = find_rule(stack->NextToken);
                return res != smallrule_none ? res : small_rule;
              }

            case smallrule_code:
            case smallrule_if:
            case smallrule_for:
            case smallrule_iter_for:
            case smallrule_while:
            case smallrule_variable:
            case smallrule_switch:
            case smallrule_try:
            case smallrule_throw:
            case smallrule_class:
            case smallrule_property:
            case smallrule_instance:
            case smallrule_event:
              {
                return small_rule;
              }

            // blocks delimiters...
            case -2: break;

            // statements delimiters...
            case -3: break;

            default:
              assert(false); // trap other cases
          }

        return 0;
      }

    str head_error()
      {
        return parser_errors_head[head_ecode];
      }

    str descrip_error()
      {
        std::stringstream ss;
        for (size_t i = 0; i < expected_tokens.size(); ++i)
          {
            if (i > 0) 
              {
                ss << ", ";
                if (i >= expected_tokens.size() - 2) ss << "or ";
              }
            ss << "'" << expected_tokens[i] << "'";
          }

        str expected = ss.str();
        const char *description = parser_errors_descrip[descrip_ecode].c_str();

        char buffer[2048];
        sprintf(buffer, description, token_data.c_str(), expected.c_str());

        return str(buffer);
      }

    str error()
      {
        if (head_ecode == 0) 
          return str();

        std::stringstream ss;

        ss << head_error() << 
          " at line " << line << 
          " column " << column << 
          ". \n";

        ss << descrip_error() << "\n";

        return ss.str();
      }

    std::vector<TokenStackStruct *> token_list;

    std::vector<str>  expected_tokens;
    expression        last_expression;
    int               last_expr_state;
    int               rule_error;
    int               head_ecode;
    int               descrip_ecode;
    str               token_data;
    unsigned int      line;
    unsigned int      column;

  private:
    int               parse_result;
    TokenStruct       *token;
    TokenStackStruct  *token_stack;
  };

//xs_compiler
xs_compiler::xs_compiler()
  {
    init_grammar();
  }

xs_compiler::xs_compiler(std::vector<str>& dsls):
  dsls_(dsls)
  {
    init_grammar();
  }

bool xs_compiler::compile_expression(const str& expr, expression& result)
  {
    TokenStruct* root;
    TokenStackStruct* token_stack;
    std::wstring buf          = str2wide(expr);
    bool         success      = false;
    int          parse_result = Parse((wchar_t*)buf.c_str(), buf.size(), 1, 0, &root, &token_stack);

    if (parse_result == PARSEACCEPT)
      {
        file_position begin, end;

        success = true;
        parsetree_visitor  v;
        expression_ ev(result);
        v.visit(root, ev);
      }

    //cleanup
    DeleteTokens(root);
    DeleteStack(token_stack);
    free(root);

    return success;
  }

bool xs_compiler::compile_code(const str& code_str, code& result)
  {
    parsetree_visitor v;
    str to_parse = process_dsl(code_str, v.dsl_text);

    TokenStruct* root;
    TokenStackStruct* token_stack;
    std::wstring buf          = str2wide("{ " + to_parse + " }");
    bool         success      = false;
    int          parse_result = Parse((wchar_t*)buf.c_str(), buf.size(), 1, 1, &root, &token_stack);

    if (parse_result == PARSEACCEPT)
      {
        file_position begin, end;
        success = true;
        code_ cd_(result);
        v.visit(root, cd_);
      }
    else
      {
        perrors_validate p_error(parse_result, root, token_stack);
        str res_error = p_error.error();

        //cleanup
        DeleteTokens(root);
        DeleteStack(token_stack);

        param_list error;
        error.add("id", SCompilerError);
        error.add("desc", SErrorCompiling);
        error.add("line", root->Line);
        error.add("column", root->Column);
        xs_throw(error);
      }

    //cleanup
    DeleteTokens(root);

    return success;
  }

bool xs_compiler::compile_xs(const str& code_str, xs_container& result)
  {
    parsetree_visitor v;
    str to_parse = process_dsl(code_str, v.dsl_text);

    TokenStruct* root;
    TokenStackStruct* token_stack;
    std::wstring buf          = str2wide(to_parse);
    bool         success      = false;
    int          parse_result = Parse((wchar_t*)buf.c_str(), buf.size(), 1, 1, &root, &token_stack);

    bool error = false;
    if (parse_result == PARSEACCEPT)
      {
        file_position begin, end;
        success = true;
        xs_ xs(result);
        v.visit(root, xs);
      }
    else if (root->ReductionRule == -1 && root->Symbol == 0)
			{
        success = true;
			}
		else
			error = true;

    perrors_validate p_error(parse_result, root, token_stack);
    str res_error = p_error.error();

    //cleanup
    DeleteTokens(root);
    DeleteStack(token_stack);

    if (error)
      {
        param_list error;
        error.add("id", SCompilerError);
        error.add("desc", SErrorCompiling);
        error.add("line", (int)root->Line);
        error.add("column", (int)root->Column);
        xs_throw(error);
      }

    return success;
  }

bool xs_compiler::compile_xs_file(const str& filename, xs_container& result)
  {
    return compile_xs(file2str(filename), result);
  }

void xs_compiler::init_grammar()
  {
    Grammar.CaseSensitive = False;
    Grammar.InitialSymbol = 132;
    Grammar.InitialDfaState = 0;
    Grammar.InitialLalrState = 0;
    Grammar.SymbolCount = 150;
    Grammar.SymbolArray = GrammarSymbolArray;
    Grammar.RuleCount = 191;
    Grammar.RuleArray = GrammarRuleArray;
    Grammar.DfaStateCount = 235;
    Grammar.DfaArray = GrammarDfaStateArray;
    Grammar.LalrStateCount = 372;
    Grammar.LalrArray = GrammarLalrStateArray;
  }

str xs_compiler::process_dsl(const str& src, std::vector<str>& dsl_texts)
  {
    str result = src;
    std::vector<str>::iterator it = dsls_.begin();
    std::vector<str>::iterator nd = dsls_.end();
    for(; it != nd; it++)
      {
        size_t curr = 0;
        str    dsl  = *it;
        while(true)
          {
            curr = result.find(dsl, curr);
            if (curr == str::npos)
              break;

            
            size_t prev_char = curr - 1;
            size_t next_char = curr + dsl.size();
            if (curr > 0 && !isspace(result[prev_char]) )
            {
                curr = next_char;
                continue; //just part of a word
            }

            if (next_char < result.size() && isalpha(result[next_char]) )
              {
                curr = next_char;
                continue; //just part of a word
              }

            //make sure this is not an accidental use of the keywords
            bool   is_dsl      = false;
            bool   not_dsl     = false;
            size_t search_pos  = curr;
            size_t brace_count = 0;
            size_t brace_start = 0;
            size_t brace_end   = 0;
            int    pcount      = 0;
						str    gap;
						bool   found_my_p = false; //have we found out our first parenthesis
						bool   close_my_p = false; //have closed it yet?
						bool   clear_gap  = false; //clear whatever we're keeping
            while (search_pos < result.size() && !not_dsl)
              {
                switch (result[search_pos])
                  {
                    case ';' :
                      {
                        if (brace_count == 0)
                          not_dsl = true;

                        break;
                      }

                    case '(' :  
											{
												if (!found_my_p)
													{
														found_my_p = true;

														//make sure there's no garbage
														boost::trim(gap);
														if (!gap.empty() && gap != dsl)
															{
																not_dsl = true;
																is_dsl = false;
															}
													}

												pcount++; 
												break;
											}

                    case ')' :  
											{
												pcount--; 
												if (!close_my_p && pcount == 0)
													{
														close_my_p = true;
														clear_gap = true;
													}

												break;
											}

                    case '{':
                      {
                        if (!close_my_p)
													{
														//no parenthesis
														not_dsl = true;
														is_dsl = false;
														break;
													}

												if (brace_count == 0)
                          {
														boost::trim(gap);
														if (!gap.empty())
															{
																not_dsl = true;
																is_dsl = false;
																break;
															}
														
														if (pcount != 0)
                              not_dsl = true;
                            else
                              {
                                is_dsl = true;
                                brace_start = search_pos;
                              }
                          }

                        brace_count++;
                        break;

                      case '}':
                        {
                          if (brace_count > 0)
                            {
                              brace_count--;
                              if (brace_count == 0)
                                {
                                  not_dsl   = true; //ugly, its just used tp stop the while
                                  brace_end = search_pos;
                                }
                            }
                          else
                            {
                              not_dsl = true; //td: there is actually an error here
                              is_dsl  = false;
                            }
                          break;
                        }
                      }
                  }

                gap += result[search_pos];
								if (clear_gap)
									{
										gap.clear();
										clear_gap = false;
									}

								search_pos++;
              }

            size_t to_advance = dsl.size();
            if (is_dsl)
              {
                //do the deed, all this dressing happens so the grammar doesn't do wacko.
                //first we capture the dsl text
                str dsl_text(result.begin() + brace_start + 1, result.begin() + brace_end);
                size_t text_idx = dsl_texts.size();
                dsl_texts.push_back( dsl_text );

                result.erase(brace_start, brace_end - brace_start + 1);

                //leave a note saying which text this is
                result.insert(brace_start, "@" + boost::lexical_cast<str>(static_cast<int>(text_idx)));

                //and mark the block as a dsl
                result.insert(curr, "@@");

                to_advance += 2;
              }

            curr += to_advance;
          }
      }

    return result;
  }

str xs_compiler::file2str(const str& filename)
  {
    std::ifstream ifs(filename.c_str());
    if (!ifs)
      return ""; //td: error

   // Read the whole file into a string
   std::stringstream ss;
   ss << ifs.rdbuf();

   return ss.str();
  }

//grammar_utils
wchar_t* grammar_utils::GetToken(wchar_t *InputBuf, long InputSize, long *InputHere, long *Line, long *Column, int *Symbol)
  {
	return RetrieveToken(InputBuf, InputSize, InputHere, Line, Column, Symbol);
  }		
