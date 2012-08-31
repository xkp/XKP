
#ifndef  XSS_AST_HH
#define XSS_AST_HH

#include "xss_context.h"

namespace xkp{

class ast;
class ast_property;
class ast_method;
class ast_event;
class ast_event_decl;
class ast_const;
class ast_instance;
class ast_class;
class ast_behaviour;
class ast_behaveas;
class ast_dsl;

typedef reference<ast>            Ast;
typedef reference<ast_property>   AstProperty;
typedef reference<ast_method>     AstMethod;
typedef reference<ast_event>      AstEvent;
typedef reference<ast_event_decl> AstEventDecl;
typedef reference<ast_const>      AstConst;
typedef reference<ast_instance>   AstInstance;
typedef reference<ast_class>      AstClass;
typedef reference<ast_behaviour>  AstBehaviour;
typedef reference<ast_behaveas>   AstBehaviourImpl;
typedef reference<ast_dsl>        AstDsl;

typedef std::vector<variant> ast_list;

class ast
  {
    private:
      ast_list items_;
  };

class ast_property
  {
    public:
      ast_property(xs_property& info);
  };

class ast_method
  {
    public:
      ast_method(xs_method& info);
  };

class ast_event
  {
    public:
      ast_event(xs_event& info);
  };

class ast_event_decl
  {
    public:
      ast_event_decl(xs_event_decl& info);
  };

class ast_const
  {
    public:
      ast_const(xs_const& info);
  };

class ast_instance
  {
    public:
      ast_instance(xs_instance& info);
  };

class ast_class
  {
    public:
      ast_class(xs_class& info);
  };

class ast_behaviour
  {
    public:
      ast_behaviour(xs_behaviour& info);
  };

class ast_behaveas
  {
    public:
      ast_behaveas(xs_implement_behaviour& info);
  };

class ast_dsl
  {
    public:
      ast_dsl(dsl& info);
  };

}

#endif