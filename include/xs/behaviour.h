
#ifndef X_BEHAVIOUR_H
#define X_BEHAVIOUR_H

#include "ast.h"
#include <dynamic_objects.h>

namespace xkp
{
  struct behaviour_schema : default_schema<behaviour_schema>, 
                            xs_visitor
    {
      behaviour_schema(behaviour_schema* super);
      
      //interface
      xs_container& publics()   {return contents_;}
      xs_container& privates()  {return privates_;}
      xs_container& delegates() {return delegates_;}
      
      //schema
      virtual void visit(schema_visitor* visitor);
      virtual void declare();

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

      xs_container      contents_;
      xs_container      delegates_;
      xs_container      privates_;
    };
    
  struct delegate_visitor : implemented_xs_visitor
    {
      delegate_visitor(IDynamicObject* bindings, IEditableObject* editable, DynamicObject output);
      
      virtual void property_(xs_property& info);
      virtual void method_(xs_method& info);

      IDynamicObject*  bindings_;
      IEditableObject* editable_;
      DynamicObject    output_;
    };
}

#endif