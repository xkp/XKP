
#ifndef XKP_DYNAMIC_OBJECTS_H
#define XKP_DYNAMIC_OBJECTS_H

#include "schema.h"
#include <boost/enable_shared_from_this.hpp>

namespace xkp
{
  //forwards
  class IDynamicObject;
  class IEditableObject;
  class IDynamicClass;

  //setter and getters
  struct anonymous_setter : setter
    {
      anonymous_setter(int _idx): idx_(_idx) {}
      virtual void set( void* instance, const variant value );

      int idx_;
    };

  struct anonymous_getter : getter
    {
      anonymous_getter(int _idx): idx_(_idx) {}
      virtual variant get(void* instance);

      int idx_;
    };

  typedef reference<IDynamicObject>  DynamicObject;
  typedef reference<IEditableObject> EditableObject;
  typedef reference<IDynamicClass>   DynamicClass;

  //some iterables
  typedef std::vector<DynamicObject>            DynamicObjectList;
  typedef reference<std::vector<DynamicObject> > DynamicObjectListRef;

  //the dynamic interface, this should be the point of connection
  //to other vms, note the default dynamic object is not editable
  //which makes it not suitable to be used as an xs instance (meta-instance?).
  //in other words, the object types you can not modify those object at run time
  struct dynamic_visitor
    {
      virtual void item(const str& name, variant value) = 0;
    };

  class IDynamicObject : public ITypedObject
    {
      public:
        virtual void          set_type(DynamicClass type)                     = 0;
        virtual size_t        add_anonymous(const variant value)              = 0;
        virtual void          set_anonymous(size_t idx, const variant value)  = 0;
        virtual variant       get_anonymous(size_t idx)                       = 0;
        virtual bool          resolve(const str& name, schema_item& result)   = 0;
        virtual DynamicObject resolve_instance(std::vector<str> name)         = 0;
        virtual size_t        event_id(const str& name)                       = 0;
        virtual void          dispatch_event(size_t ev_id, param_list& args)  = 0;
        virtual void          visit(dynamic_visitor* visitor)                 = 0;
    };

  class IEditableObject
    {
      public:
        virtual void add_item(const str& name, schema_item& item) = 0;
    };

  class IDynamicClass
    {
      public:
        virtual size_t  event_id(const str& name)                                             = 0;
        virtual void    dispatch_event(IDynamicObject* instance, size_t ev_id, param_list& args) = 0;
        virtual schema* get_schema()                                                          = 0;
    };

  //the engine, we'll try to provide as many useful
  //classes as we possibly can. Note that you only subclass
  //the types below when your object will be somehow dynamic.
  //See below:

  //A lightweight dynamic object that will be able to be subclassed
  //note that objects inheriting from these types can have methods and properties
  class base_dynamic_object : public IDynamicObject
    {
      public:
        base_dynamic_object();
        base_dynamic_object(const base_dynamic_object& other);
      public:
        virtual schema*       get_type();
        virtual void          set_type(DynamicClass type);
        virtual size_t        add_anonymous(const variant value);
        virtual void          set_anonymous(size_t idx, const variant value);
        virtual variant       get_anonymous(size_t idx);
        virtual bool          resolve(const str& name, schema_item& result);
        virtual DynamicObject resolve_instance(std::vector<str> name);
        virtual size_t        event_id(const str& name);
        virtual void          dispatch_event(size_t ev_id, param_list& args);
      protected:
        DynamicClass         class_; //note this reference guarantees the pointer below
        schema*              schema_;
        std::vector<variant> anonymous_;
    };

  struct event_info
    {
      size_t id;

      event_info(size_t _id) : id(_id) {}
    };


  class event_holder
    {
      public:
        bool   add_item(const str& name, schema_item& item);
        size_t event_id(const str& name);
        void   dispatch_event(IDynamicObject* this_, size_t ev_id, param_list& args);
      protected:
        typedef std::multimap<size_t, Executer> event_list;
        typedef std::pair<size_t, Executer>     event_pair;
        typedef std::map<str, size_t>           event_decl_list;
        typedef std::pair<str, size_t>          event_decl_pair;

        event_list      events_;
        event_decl_list event_decl_;
    };

  //An "instantiable" one. You know, the ones that can be modified at run time
  //something like:
  //
  //some_class var;
  //instance var
  //{
  //    method foo() { ... }
  //}
  class base_editable_object : public base_dynamic_object,
                               public event_holder,
                               public IEditableObject
    {
      public:
        base_editable_object();
        base_editable_object(const base_editable_object& other);
      public:
        //IDynamicObject
        virtual bool   resolve(const str& name, schema_item& result);
        virtual size_t event_id(const str& name);
        virtual void   dispatch_event(size_t ev_id, param_list& args);
        virtual void   visit(dynamic_visitor* visitor);

        //IEditableObject
        virtual void add_item(const str& name, schema_item& item);
      protected:
        typedef std::map<str, schema_item>    item_list;
        typedef std::pair<str, schema_item>   item_pair;

        item_list items_;
    };

  //type friendly
  template <typename T>
  class dynamic_object : public base_dynamic_object
    {
      public:
        dynamic_object()
          {
            schema_ = type_schema<T>();
          }

        dynamic_object(const dynamic_object& other):
          base_dynamic_object(other)
          {
          }
    };

  template <typename T>
  class editable_object : public base_editable_object
    {
      public:
        editable_object()
          {
            schema_ = type_schema<T>();
          }

        editable_object(const editable_object& other):
          base_editable_object(other)
          {
          }

        void add_property(const str& name, variant value = variant())
          {
            int idx = add_anonymous(value);

            schema_item si;
            si.flags = DYNAMIC_ACCESS;
            si.get   = Getter( new anonymous_getter(idx) );
            si.set   = Setter( new anonymous_setter(idx) );
            si.type  = schema_;

            items_.insert( item_pair(name, si) );
          }
    };


  //the dynamic schemas
  //this is at best not good looking, or unfortunate.
  //as interfaces are introduced the type system suffer of upward casting
  //so in order to resolve types safely we have to dynamic_cast.
  //however we must do that on the concrete type schema.
  template <typename T>
  struct do_interface_schema : default_schema<T>
    {
      virtual void cast(const variant src, schema* ss, variant& result)
        {
          T self = src;
          self->get_type()->cast(src, ss, result);
        }

      virtual bool create(variant& result, param_list* args = null)
        {
          throw type_mismatch(); //the actual type of the object is not known,
                                 //you should not instantiate types you are not
                                 //aware of.
          return false;
        }

      virtual bool clone(const variant v, variant& result)
        {
          T self = v;
          return self->get_type()->clone(v, result);
        }
    };

  register_type(IDynamicObject*, do_interface_schema<IDynamicObject*>);
  register_type(DynamicObject,   do_interface_schema<DynamicObject>  );
  register_iterator(DynamicObject);

  //Note the above schema is not for public comsumption. It basically
  //wraps the interface. Below are the schemas you should use to define your types
  //for instance: struct my_type_schema : dynamic_object_schema<my_type>
  template <typename T>
  struct dynamic_object_schema : object_schema<T>
    {
      dynamic_object_schema()
        {
          this->template implements<IDynamicObject>();
        }

      virtual void cast(const variant src, schema* ss, variant& result)
        {
          IDynamicObject* d;
          if (is_dynamic(src, d))
            {
              //now we can finally cast up safely.
              T* self = dynamic_cast<T*>(d);
              if (!self)
                throw type_mismatch();

              object_schema<T>::cast(self, ss, result);
              return;
            }

          object_schema<T>::cast(src, ss, result);
        }

      virtual bool clone(const variant v, variant& result)
        {
          IDynamicObject* do_ = null;
          if (v.is<DynamicObject>())
            {
              DynamicObject do__ = v;
              do_ = do__.get();
            }
          else if (v.is<IDynamicObject*>())
            do_ = v;

          if (do_)
            {
              T* t = dynamic_cast<T*>(do_);
              if (!t)
                {
                  throw type_mismatch();
                }

              reference<T> r = reference<T>( new T(*t) );
              result = r;
              return true;
            }

          return object_schema<T>::clone(v, result);
        }

      bool is_dynamic(const variant v, IDynamicObject*& result)
        {
          IDynamicObject* do_ = null;
          if (v.is<DynamicObject>())
            {
              DynamicObject do__ = v;
              result = do__.get();
            }
          else if (v.is<IDynamicObject*>())
            result = v;
          else
            return false;

          return true;
        }
    };

  //default xs objects,namely the ones being instantiated
  //when you dont inherit (class foo {...})
  class default_object : public editable_object<default_object>
    {
      public:
        default_object()                                                                    {}
        default_object(const default_object& other): editable_object<default_object>(other) {}
    };

  //a little fun, an object that implements all properties by simply adding them
  //on demand. see: behave as.
  class sponge_object : public editable_object<sponge_object>
    {
      public:
        sponge_object(): stopped_(false)
          {
          }

        sponge_object(const sponge_object& other):
          editable_object<sponge_object>(other),
          stopped_(other.stopped_)
          {
          }
      public:
        //interface
        void stop();
      public:
        //schema
        virtual void visit(schema_visitor* visitor);
      public:
        //IDynamicObject
        virtual bool resolve(const str& name, schema_item& result);

      private:
        bool stopped_;
    };

  //schemas
  struct default_object_schema : dynamic_object_schema<default_object>
    {
      default_object_schema()
        {
          implements<IEditableObject>();
        }
    };

  struct sponge_object_schema : dynamic_object_schema<sponge_object>
    {
      sponge_object_schema()
        {
          implements<IEditableObject>();
        }

      virtual size_t options()
        {
          return dynamic_object_schema<sponge_object>::options() | TYPE_MUTABLE;
        }
    };

  register_complete_type(default_object, default_object_schema);
  register_complete_type(sponge_object,  sponge_object_schema);

  //this will represent a class created in xs
  struct dynamic_class_schema : basic_schema,
                                IEditableObject,
                                IDynamicClass,
                                boost::enable_shared_from_this<dynamic_class_schema>,
                                event_holder
    {
      dynamic_class_schema(DynamicObject _base);

      //schema
      virtual bool    resolve(const str& name, schema_item& item);
      virtual size_t  size();
      virtual bool    at(size_t idx, schema_item& item);
      virtual void    cast(const variant src, schema* s, variant& result);
      virtual void*   get_pointer(void**);
      virtual bool    create(variant& result, param_list* args = null);
      virtual bool    clone(const variant v, variant& result);

      //IEditableObject
      virtual void add_item(const str& name, schema_item& item);

      //IDynamicClass
      virtual size_t  event_id(const str& name);
      virtual void    dispatch_event(IDynamicObject* instance, size_t ev_id, param_list& args);
      virtual schema* get_schema();

      DynamicObject base_;
      schema*       base_schema_;
    };

  //a simple type collection, this has got to get a little
  //more sofisticated. thread safety and keeping shared pointers
  //of dynamic classes for starters
  struct type_registry
    {
      type_registry();

      schema* get_type(const str& type_id, param_list* args = null);
      void    add_type(const str& type_id, schema* s);
      void    add_type(const str& type_id, DynamicClass dc);
      void    add_type(const str& type_id, Schema s);
      str     type_name(schema* s);
      void    set_default_type(schema* default_type);

      template <typename T>
      void add_type(const str& type_id)
        {
          add_type(type_id, type_schema<T>());
        }

      bool has(const str& id)
        {
          container::iterator it = types_.find(id);
          return it != types_.end();
        }
      private:
        typedef std::vector<DynamicClass> dc_ref_list;
        typedef std::vector<Schema>       schema_ref_list;

        typedef std::map<str, schema*>  container;
        typedef std::pair<str, schema*> container_pair;

        container       types_;
        dc_ref_list     dynamic_classes_;
        schema_ref_list schemas_;
        schema*         default_type_;
    };

  //utils
  inline variant dynamic_get(variant v, const str& item)
    {
      variant result;
      if (variant_try_get(v, item, result))
        return result;

      IDynamicObject* obj = variant_cast<IDynamicObject*>(v, null);
      if (obj)
        {
          schema_item itm;
          if (obj->resolve(item, itm))
            return itm.get->get(obj);
        }

      return variant();
    }

  inline bool dynamic_try_get(variant v, const str& item, variant& result)
    {
      if (variant_try_get(v, item, result))
        return true;

      IDynamicObject* obj = variant_cast<IDynamicObject*>(v, null);
      if (obj)
        {
          schema_item itm;
          if (obj->resolve(item, itm))
            {
              result = itm.get->get(obj);
              return true;
            }
        }

      return false;
    }

  inline bool dynamic_set(variant v, const str& item, variant value)
    {
      if (variant_set(v, item, value))
        return true;

      IDynamicObject* obj = variant_cast<IDynamicObject*>(v, null);
      if (obj)
        {
          schema_item itm;
          if (obj->resolve(item, itm) && itm.set)
            {
              itm.set->set(obj, value);
              return true;
            }
        }

      return false;
    }

  inline variant dynamic_exec(variant v, const str& item, const param_list pl)
    {
      variant result;
      if (variant_try_exec(v, item, pl, result))
        return result;

      IDynamicObject* obj = variant_cast<IDynamicObject*>(v, null);
      if (obj)
        {
          schema_item itm;
          if (obj->resolve(item, itm) && itm.exec)
            {
              return itm.exec->exec(obj, pl);
            }
        }

      return variant();
    }

  inline bool dynamic_try_exec(variant v, const str& item, const param_list pl, variant& result)
    {
      if (variant_try_exec(v, item, pl, result))
        return true;

      IDynamicObject* obj = variant_cast<IDynamicObject*>(v, null);
      if (obj)
        {
          schema_item itm;
          if (obj->resolve(item, itm) && itm.exec)
            {
              result = itm.exec->exec(obj, pl);
              return true;
            }
        }

      return false;
    }

  inline void add_property(DynamicObject obj, const str& name, variant value)
    {
        IEditableObject* editable = variant_cast<IEditableObject*>(obj, null); assert(editable);

        int idx = obj->add_anonymous(value);

        schema_item itm;
        itm.flags = DYNAMIC_ACCESS;
        itm.get   = Getter( new anonymous_getter(idx) );
        itm.set   = Setter( new anonymous_setter(idx) );

        editable->add_item(name, itm);
    }
}

#endif
