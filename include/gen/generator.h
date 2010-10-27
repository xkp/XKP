
#ifndef GENERATOR_HH
#define GENERATOR_HH

#include "xs/ast.h"

namespace xkp
{
  class generator : xs_visitor 
    {
      public:
        void consume(xs_container xs);
        str  generate();
      public:
        //xs_visitor
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
      public:
        void register_idiom(const str& idiom, variant handler);
      private:
        typedef std::map<str, variant> idiom_map;
        idiom_map idioms_;
    };
  
}

#endif