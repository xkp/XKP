
#include <xs/behaviour.h>
#include <xs/xs_error.h>

using namespace xkp;

//strings
const str SErrorOutOfContext("ooc");
const str SInstanceNotAllowedOnBehaviours("Instances are not allowed inside behaviours");
const str SClassesNotAllowedOnBehaviours("Classes are not allowed inside behaviours");
const str SNoNestedBehaviours("Nested behaviours are not allowed");
const str SInterfaceNotMet("behaviour-interface");
const str SProvideBehaviourWithProperty("This behaviour needs a property");
const str SProvideBehaviourWithMethod("This behaviour needs a method");

//behaviour_schema
behaviour_schema::behaviour_schema(behaviour_schema* super)
  {
    if (super)
      {
        privates_  = super->privates();
        contents_  = super->publics();
        delegates_ = super->delegates();
      }
  }
  
void behaviour_schema::visit(schema_visitor* visitor)
  {
    assert(false); //td: implement a mutant visitor
  }
  
void behaviour_schema::property_(xs_property& info)
  {
    if (info.adjetives & adj_delegate)
      {
        //td: check there is no code on delegate props and raise error
        delegates_.add(info);
      }
    else if (info.adjetives & adj_private)
      privates_.add(info); 
    else
      contents_.add(info);
  }
  
void behaviour_schema::method_(xs_method& info)
  {
    if (info.adjetives & adj_delegate)
      {
        //td: check there is no code on delegate props and raise error
        delegates_.add(info);
        return;
      }
    else if (info.adjetives & adj_private)
      privates_.add(info);
    else
      contents_.add(info);
  }
  
void behaviour_schema::event_(xs_event& info)
  {
    assert(false); //td:
  }
  
void behaviour_schema::event_decl_(xs_event_decl& info)
  {
    assert(false); //td:
  }
  
void behaviour_schema::const_(xs_const& info)
  {
    assert(false); //td:
  }
  
void behaviour_schema::instance_(xs_instance& info)
  {
    param_list error;
    error.add("id", SErrorOutOfContext);
    error.add("desc", SInstanceNotAllowedOnBehaviours);
    xs_throw(error);
  }
  
void behaviour_schema::class_(xs_class& info)
  {
    param_list error;
    error.add("id", SErrorOutOfContext);
    error.add("desc", SClassesNotAllowedOnBehaviours);
    xs_throw(error);
  }
  
void behaviour_schema::behaviour_(xs_behaviour& info)
  {
    param_list error;
    error.add("id", SErrorOutOfContext);
    error.add("desc", SNoNestedBehaviours);
    xs_throw(error);
  }
  
void behaviour_schema::behaveas_(xs_implement_behaviour& info)
  {
    param_list error;
    error.add("id", SErrorOutOfContext);
    error.add("desc", SNoNestedBehaviours);
    xs_throw(error);
  }
  
void behaviour_schema::dsl_(dsl& info)
  {
    assert(false); //td: hmmmm
  }
  
//delegate_visitor
delegate_visitor::delegate_visitor(IDynamicObject* bindings, IEditableObject* editable, DynamicObject output):
  bindings_(bindings),
  editable_(editable),
  output_(output)
  {
  }
  
void delegate_visitor::property_(xs_property& info)
  {
    schema_item result;
    if (output_->resolve(info.name, result))
      return; //already there
    
    if (bindings_->resolve(info.name, result))
      {
        variant binding = result.get->get(bindings_);
        if (binding.is<schema_item>())
          {
            schema_item si = binding;
            editable_->add_item(info.name, si);
          }
        else
          {
            int idx = output_->add_anonymous(binding);        
            schema_item si;
            si.flags = DYNAMIC_ACCESS;
            si.get   = Getter( new anonymous_getter(idx) );
            si.type  = binding.get_schema();
            editable_->add_item(info.name, si);
          }
      }
    else
      {
        param_list error;
        error.add("id", SInterfaceNotMet);
        error.add("desc", SProvideBehaviourWithProperty);
        error.add("property", info.name);
        xs_throw(error);
      }
  }
  
void delegate_visitor::method_(xs_method& info)
  {
    schema_item result;
    if (output_->resolve(info.name, result))
      return; //already there
    
    bool resolved = false;
    if (bindings_->resolve(info.name, result))
      {
        variant binding = result.get->get(bindings_);
        if (binding.is<schema_item>())
          {
            schema_item si = binding;
            editable_->add_item(info.name, si);

            resolved = true;
          }
      }
    
    if (!resolved)
      {
        param_list error;
        error.add("id", SInterfaceNotMet);
        error.add("desc", SProvideBehaviourWithMethod);
        error.add("property", info.name);
        xs_throw(error);
      }
  }
  
