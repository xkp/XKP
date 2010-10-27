
#include "dynamic_objects.h"
#include <xs/vm.h> //td: move const getter

#include <boost/functional/hash.hpp>

using namespace xkp;

//base_dynamic_object
base_dynamic_object::base_dynamic_object():
  schema_(type_schema<base_dynamic_object>())
  {
  }
  
base_dynamic_object::base_dynamic_object(const base_dynamic_object& other):
  class_(other.class_),
  schema_(other.schema_),
  anonymous_(other.anonymous_)
  {
  }
  
schema* base_dynamic_object::get_type()
  {
    return schema_;
  }
  
void base_dynamic_object::set_type(DynamicClass type)
  {
    class_  = type;
    schema_ = type->get_schema();
  }
  
size_t base_dynamic_object::add_anonymous(const variant value)
  {
    anonymous_.push_back( value );
    return anonymous_.size() - 1;
  }
  
void base_dynamic_object::set_anonymous(size_t idx, const variant value)
  {
    if (idx > anonymous_.size())
      {
        assert(false); //td: error system
      }
      
    anonymous_[idx] = value;
  }
  
variant base_dynamic_object::get_anonymous(size_t idx)
  {
    if (idx > anonymous_.size())
      {
        assert(false); //td: error system
      }

    return anonymous_[idx];
  }
  
bool base_dynamic_object::resolve(const str& name, schema_item& result)
  {
    return schema_->resolve(name, result);
  }
  
DynamicObject base_dynamic_object::resolve_instance(std::vector<str> name)
  {
    return DynamicObject(); //no resolving
  }
  
size_t base_dynamic_object::event_id(const str& name)
  {
    return class_->event_id( name );
  }
  
void base_dynamic_object::dispatch_event(size_t ev_id, param_list& args)
  {
    class_->dispatch_event(this, ev_id, args);
  }
  
//event_holder
bool event_holder::add_item(const str& name, schema_item& item)
  {
    if (item.flags & EVENT)
      {
        int ev_id = item.get->get(null);
        events_.insert( event_pair(ev_id, item.exec) );
      }
    else if (item.flags & EVENT_DECL)
      {
        boost::hash<std::string> string_hash;
        size_t ev_id = string_hash(name);

        event_decl_.insert(event_decl_pair(name, ev_id)); //td: isnt this redundant?
        
        event_info ei(ev_id);
        item.flags |= CONST;
        item.get    = Getter( new const_getter(ei) );
        return false;
        
      }
    else
      return false;
    
    return true;
  }
  
size_t event_holder::event_id(const str& name)
  {
    event_decl_list::iterator it = event_decl_.find(name);
    if (it != event_decl_.end())
      return it->second;
    return 0;
  }
  
void event_holder::dispatch_event(IDynamicObject* this_, size_t ev_id, param_list& args)
  {
    event_list::iterator it = events_.find(ev_id);
    while(it != events_.end() && it->first == ev_id)
      {
        it->second->exec(this_, args);
        it++;
      }
  }
  
//base_editable_object
base_editable_object::base_editable_object()
  {
  }
  
base_editable_object::base_editable_object(const base_editable_object& other):
  base_dynamic_object(other),
  items_(other.items_)
  {
  }
  
bool base_editable_object::resolve(const str& name, schema_item& result)
  {
    if (base_dynamic_object::resolve(name, result))
      return true;
      
    item_list::iterator it = items_.find(name);
    if (it != items_.end())
      {
        result = it->second;
        return true;
      }
    
    return false;
  }
  
size_t base_editable_object::event_id(const str& name)
  {
    size_t result = base_dynamic_object::event_id(name);
    if (result != 0)
      return result;
      
    event_decl_list::iterator it = event_decl_.find(name);
    if (it != event_decl_.end())      
      return it->second;
    
    return 0;
  }
  
void base_editable_object::dispatch_event(size_t ev_id, param_list& args)
  {
    base_dynamic_object::dispatch_event(ev_id, args);
    event_holder::dispatch_event(this, ev_id, args);
  }
  
void base_editable_object::visit(dynamic_visitor* visitor)
  {
    item_list::iterator it = items_.begin();
    item_list::iterator nd = items_.end();
    
    for(; it != nd; it++)
      {
        if (it->second.get)
          {
            variant value = it->second.get->get(this);
            visitor->item(it->first, value);
          }
      }
  }
  
void base_editable_object::add_item(const str& name, schema_item& item)
  {
    if (!event_holder::add_item(name, item))
      {
        items_.insert( item_pair(name, item) );
      }
  }
  
//anonymous_setter
void anonymous_setter::set(void* instance, const variant value)
  {
    IDynamicObject* self = static_cast<IDynamicObject*>(instance);
    self->set_anonymous(idx_, value);
  }

//anonymous_getter
variant anonymous_getter::get(void* instance)
  {
    IDynamicObject* self = static_cast<IDynamicObject*>(instance);
    return self->get_anonymous(idx_);
  }
  
//dynamic_class_schema
dynamic_class_schema::dynamic_class_schema(DynamicObject _base):
  base_(_base)
  {
    base_schema_ = base_->get_type();
  }
  
bool dynamic_class_schema::resolve(const str& name, schema_item& item)
  {
    if (basic_schema::resolve(name, item))
      return true;
    
    return base_schema_->resolve(name, item);    
  }
  
size_t dynamic_class_schema::size()
  {
    return base_schema_->size() + vmt_.size();
  }
  
bool dynamic_class_schema::at(size_t idx, schema_item& item)
  {
    if (idx < base_schema_->size())
      return base_schema_->at(idx, item);
    
    return basic_schema::at(idx - base_schema_->size(), item);
  }
  
void dynamic_class_schema::cast(const variant src, schema* s, variant& result)
  {
    base_schema_->cast(src, s, result);
  }
  
void* dynamic_class_schema::get_pointer(void** p)
  {
    return base_schema_->get_pointer(p);
  }
  
bool dynamic_class_schema::create(variant& result, param_list* args)
  {
    //td: use args
    if (!base_schema_->clone(base_, result))
      {
        assert(false); //td: error
        return false;
      }
      
    DynamicObject do_                            = result;
    boost::shared_ptr<IDynamicClass> this_shared = shared_from_this();
    DynamicClass  type( this_shared );
    do_->set_type( type );
    return true;
  }
  
bool dynamic_class_schema::clone(const variant v, variant& result)  
  {
    return base_schema_->clone(v, result);
  }

void dynamic_class_schema::add_item(const str& name, schema_item& item)
  {
    if (!event_holder::add_item(name, item))
      {
        indices_.insert(vmt_pair(name, vmt_.size())); 
        vmt_.push_back(item); 
      }
  }
  
size_t dynamic_class_schema::event_id(const str& name)
  {
    return event_holder::event_id(name);
  }
  
void dynamic_class_schema::dispatch_event(IDynamicObject* instance, size_t ev_id, param_list& args)
  {
    event_holder::dispatch_event(instance, ev_id, args);
  }
  
schema* dynamic_class_schema::get_schema()
  {
    return this;
  }

//sponge_object    
void sponge_object::stop()
  {
    stopped_ = true;
  }

void sponge_object::visit(schema_visitor* visitor)
  {
    item_list::iterator it = items_.begin();
    item_list::iterator nd = items_.end();
    
    for(; it != nd; it++)
      {
        visitor->visit(it->first, 0, it->second);
      }
  }
  
bool sponge_object::resolve(const str& name, schema_item& result)
  {
    item_list::iterator it = items_.find(name);
    if (it != items_.end())
      {
        if (name == "id")
          {
            _asm nop;
          }
        result = it->second;
      }
    else 
      {
        if (stopped_)
          return false;
          
        int idx      = add_anonymous(variant());        
        result.get   = Getter( new anonymous_getter(idx) );
        result.set   = Setter( new anonymous_setter(idx) );
        result.flags = DYNAMIC_ACCESS;
        
        items_.insert( item_pair(name, result) );
      }
    return true;
  }

//type_registry
type_registry::type_registry(): default_type_(null)
  {
    add_type<int>   ( "int"     );
    add_type<str>   ( "string"  );
    add_type<float> ( "float"   );
    add_type<bool>  ( "bool"    );
    add_type<double>( "double"  );
  }
  
schema* type_registry::get_type(const str& type_id, param_list* args)
  {
    container::iterator it = types_.find( type_id );
    if (it != types_.end())
      {
        schema* result = it->second;
        
        if (result->options() & TYPE_META)
          {
            variant dynamic_type;
            if (result->create(dynamic_type, args))
              result = dynamic_type; //td: the meta tyoe is responsible for keeping the references to created types, iffy
            else
              return null;
          }
          
        return result;
      }
    
    return default_type_;
  }
  
void type_registry::add_type(const str& type_id, schema* s)
  {
    types_.insert( container_pair(type_id, s) );
  }

void type_registry::add_type(const str& type_id, DynamicClass dc)
  {
    dynamic_classes_.push_back(dc);
    types_.insert( container_pair(type_id, dc->get_schema()) );
  }
  
void type_registry::add_type(const str& type_id, Schema s)
  {
    schemas_.push_back(s);
    types_.insert( container_pair(type_id, s.get()) );
  }

str type_registry::type_name(schema* s)
  {
    container::iterator it = types_.begin();
    container::iterator nd = types_.end();
    
    for(; it != nd; it++)
      {
        if (it->second == s)
          return it->first;
      }
      
    return "";
  }
  
void type_registry::set_default_type(schema* default_type)
  {
    default_type_ = default_type;
  }
