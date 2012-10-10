
#ifndef XSS_CODE_HH
#define XSS_CODE_HH

#include "xss_context.h"

namespace xkp{

class statement_if : public xss_statement, 
                     public IStatementIf 
  {
    public:
      statement_if(XSSExpression expr, XSSCode if_code, XSSCode else_code, file_position& begin, file_position& end):
        xss_statement(STATEMENT_IF, begin, end),
        expr_(expr),
        if_code_(if_code),
        else_code_(else_code)
        {
        }
    public:
      virtual void bind(XSSContext ctx);
    public:
      virtual XSSExpression expr()      {return expr_;}
      virtual XSSCode       if_code()   {return if_code_;}
      virtual XSSCode       else_code() {return else_code_;}
    private:
      XSSExpression expr_;
      XSSCode       if_code_; 
      XSSCode       else_code_;
  };

class statement_variable : public xss_statement, 
                           public IStatementVar
  {
    public:
      statement_variable(const str& id, const xs_type& type, XSSExpression value, file_position& begin, file_position& end):
        xss_statement(STATEMENT_VAR, begin, end),
        id_(id),
        type_info_(type),
        value_(value),
        needs_cast_(false)
        {
        }
    public:
      virtual void bind(XSSContext ctx);
    public:
      virtual str           id()          {return id_;}
      virtual str           type_name()   {return type_info_.name;}
      virtual xs_type&      type_info()   {return type_info_;}
      virtual XSSType       type()        {return type_;}
      virtual XSSExpression value()       {return value_;}
      virtual bool          needs_cast()  {return needs_cast_;}
    private:
      str           id_;
      xs_type       type_info_;
      XSSType       type_;
      XSSExpression value_;
      bool          needs_cast_;
  };

class statement_for : public xss_statement, 
                      public IStatementFor
  {
    public:
      statement_for(const str& id, const xs_type& type, XSSExpression init_value, 
                    XSSExpression init_expr, XSSExpression cond_expr, XSSExpression iter_expr,
                    XSSCode for_code, file_position& begin, file_position& end):
        xss_statement(STATEMENT_FOR, begin, end),
        id_(id),
        type_info_(type),
        init_value_(init_value),
        init_expr_(init_expr),
        cond_expr_(cond_expr),
        iter_expr_(iter_expr),
        for_code_(for_code),
        needs_cast_(false)
        {
        }
    public:
      virtual void bind(XSSContext ctx);
    public:
      virtual str           id()         {return id_;             }
      virtual str           type_name()  {return type_info_.name; }
      virtual xs_type&      type_info()  {return type_info_;      } 
      virtual XSSType       type()       {return type_;           }
      virtual XSSExpression init_value() {return init_value_;     }
      virtual XSSExpression init_expr()  {return init_expr_;      } 
      virtual XSSExpression cond_expr()  {return cond_expr_;      } 
      virtual XSSExpression iter_expr()  {return iter_expr_;      } 
      virtual XSSCode       for_code()   {return for_code_;       } 
      virtual bool          cast_init()  {return needs_cast_;     }
    private:
      str           id_;
      xs_type       type_info_;
      XSSType       type_;
      XSSExpression init_value_;
      XSSExpression init_expr_;
      XSSExpression cond_expr_;
      XSSExpression iter_expr_;
      XSSCode       for_code_;
      bool          needs_cast_;
  };

class statement_foreach : public xss_statement, 
                          public IStatementForEach
  {
    public:
      statement_foreach(const str& id, const str& type, XSSExpression iter_expr, XSSCode for_code, file_position& begin, file_position& end):
        xss_statement(STATEMENT_FOREACH, begin, end),
        id_(id),
        type_name_(type),
        iter_expr_(iter_expr),
        for_code_(for_code),
        needs_cast_(false)
        {
        }
    public:
      virtual void bind(XSSContext ctx);
    public:
      virtual str           id()         {return id_;         }
      virtual str           type_name()  {return type_name_;  }
      virtual XSSType       type()       {return type_;       }
      virtual XSSExpression iter_expr()  {return iter_expr_;  } 
      virtual XSSCode       for_code()   {return for_code_;   } 
      virtual bool          needs_cast() {return needs_cast_; }  
    private:
      str           id_;
      str           type_name_;
      XSSType       type_;
      XSSExpression iter_expr_;
      XSSCode       for_code_;
      bool          needs_cast_;
  };

class statement_while : public xss_statement, 
                        public IStatementWhile
  {
    public:
      statement_while(XSSExpression expr, XSSCode code, file_position& begin, file_position& end):
        xss_statement(STATEMENT_WHILE, begin, end),
        expr_(expr),
        code_(code)
        {
        }
    public:
      virtual void bind(XSSContext ctx);
    public:
      virtual XSSExpression expr()  {return expr_;  } 
      virtual XSSCode       code()  {return code_;   } 
    private:
      XSSExpression expr_;
      XSSCode       code_;
  };

class statement_switch : public xss_statement, 
                         public IStatementSwitch
  {
    public:
      statement_switch(XSSExpression expr, XSSCode default_code, file_position& begin, file_position& end):
        xss_statement(STATEMENT_SWITCH, begin, end),
        expr_(expr),
        default_code_(default_code)
        {
        }
    public:
      statement_switch_section& create_section(XSSCode case_code)
        {
          sections_.push_back(statement_switch_section(case_code));
          return *(sections_.end() - 1);
        }
    public:
      virtual void bind(XSSContext ctx);
    public:
      virtual XSSExpression    expr()          {return expr_;        } 
      virtual XSSCode          default_code()  {return default_code_;}
      virtual switch_sections& sections()      {return sections_;} 
    private:
      XSSExpression   expr_;
      XSSCode         default_code_;
      switch_sections sections_;
  };

class statement_try : public xss_statement, 
                      public IStatementTry
  {
    public:
      statement_try(XSSCode try_code, XSSCode finally_code, file_position& begin, file_position& end):
        xss_statement(STATEMENT_TRY, begin, end),
        try_code_(try_code),
        finally_code_(finally_code)
        {
        }
    public:
      statement_catch_section& create_section(const str& id, const str& type, XSSCode catch_code)
        {
          sections_.push_back(statement_catch_section(id, type, catch_code));
          return *(sections_.end() - 1);
        }
    public:
      virtual void bind(XSSContext ctx);
    public:
      virtual XSSCode         try_code()      {return try_code_;     } 
      virtual XSSCode         finally_code()  {return finally_code_; }
      virtual catch_sections& sections()      {return sections_;     } 
    private:
      XSSCode        try_code_;
      XSSCode        finally_code_;
      catch_sections sections_;
  };

class expr_statement : public xss_statement, 
                       public IStatementExpression
  {
    public:
      expr_statement(STATEMENT_TYPE id, XSSExpression expr, file_position& begin, file_position& end):
        xss_statement(id, begin, end),
        expr_(expr)
        {
        }
    public:
      virtual void bind(XSSContext ctx);
    public:
      virtual XSSExpression expr() {return expr_; } 
    private:
      XSSExpression expr_;
  };

class xss_loop_statement : public xss_statement
  {
    public:
      xss_loop_statement(STATEMENT_TYPE id, file_position& begin, file_position& end):
        xss_statement(id, begin, end)
        {
        }
    public:
      virtual void bind(XSSContext ctx);
  };

struct xss_code_utils
  {
    static XSSCode      compile_code(code& cde, IContextCallback* callback);
    static XSSSignature compile_arg_list(param_list_decl& args);
  };

}

#endif