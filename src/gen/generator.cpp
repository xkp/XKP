
#include "gen/generator.h"

using namespace xkp;


void generator::consume(xs_container xs)
  {
    xs.visit(this);
  }
  
str generator::generate()
  {
    str result;
    
    return result;
  }

void generator::property_(xs_property& info)
  {
    assert(false); //td
  }
  
void generator::method_(xs_method& info)
  {
    assert(false); //td
  }
  
void generator::event_(xs_event& info)
  {
    assert(false); //td
  }
  
void generator::event_decl_(xs_event_decl& info)
  {
    assert(false); //td
  }
  
void generator::const_(xs_const& info)
  {
    assert(false); //td
  }
  
void generator::instance_(xs_instance& info)
  {
    assert(false); //td
  }
  
void generator::class_(xs_class& info)
  {
    idiom_map::iterator it = idioms_.find("class");
    if (it != idioms_.end())
      {
      }
    else
      {
        assert(false); //td
      }  
  }
  
void generator::behaviour_(xs_behaviour& info)
  {
    assert(false); //td
  }
  
void generator::behaveas_(xs_implement_behaviour& info)
  {
    assert(false); //td
  }
  
void generator::dsl_(dsl& info)
  {
    assert(false); //td
  }
  
void generator::register_idiom(const str& idiom, variant handler)
  {
    idioms_.insert(std::pair<str, variant>( idiom, handler ));
  }
