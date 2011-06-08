
#include <xss/language.h>

using namespace xkp;

//source_collector
void source_collector::property_(xs_property& info)
  {
    properties.push_back(info);
  }

void source_collector::method_(xs_method& info)
  {
    methods.push_back(info);
  }

void source_collector::event_(xs_event& info)
  {
    events.push_back(info);
  }

void source_collector::event_decl_(xs_event_decl& info)
  {
  }

void source_collector::const_(xs_const& info)
  {
  }

void source_collector::instance_(xs_instance& info)
  {
    instances.push_back(info);
  }

void source_collector::class_(xs_class& info)
  {
  }

void source_collector::behaviour_(xs_behaviour& info)
  {
  }

void source_collector::behaveas_(xs_implement_behaviour& info)
  {
  }

void source_collector::dsl_(dsl& info)
  {
  }

//code_type_resolver
code_type_resolver::code_type_resolver(XSSContext ctx)
  {
  }

XSSType code_type_resolver::get()
  {
    assert(false); //implement
    return XSSType();
  }

void code_type_resolver::variable_(stmt_variable& info)
  {
  }

void code_type_resolver::return_(stmt_return& info)
  {
    assert(false); //td: punt til java
  }

void code_type_resolver::if_(stmt_if& info)
  {
  }

void code_type_resolver::for_(stmt_for& info)
  {
  }

void code_type_resolver::iterfor_(stmt_iter_for& info)
  {
  }

void code_type_resolver::while_(stmt_while& info)
  {
  }

void code_type_resolver::break_()
  {
  }

void code_type_resolver::continue_()
  {
  }

void code_type_resolver::expression_(stmt_expression& info)
  {
  }

void code_type_resolver::dsl_(dsl& info)
  {
  }

void code_type_resolver::dispatch(stmt_dispatch& info)
  {
  }

