
#ifndef XKP_BASE_H
#define XKP_BASE_H

#include "types.h"

#include <map>
#include <vector>
#include <boost/lexical_cast.hpp>

namespace xkp
{
  //forwards
  struct variant;
  struct schema;
  struct param_list;

  typedef reference<param_list> ParamList;
  typedef reference<schema>     Schema;

  //exception to be thrown on variant mismatches
  struct type_mismatch {};

  //the nobodies
  struct empty_type {};

  //the dynamic part of the schema will be made of these, basically an intermediate representation
  //of executable items.
  struct setter   { virtual void    set ( void* instance, const variant value )   = 0; };
  struct getter   { virtual variant get ( void* instance )                        = 0; };
  struct executer { virtual variant exec( void* instance, const param_list args ) = 0; };

  typedef reference<setter>   Setter;
  typedef reference<getter>   Getter;
  typedef reference<executer> Executer;

  //execution flags, unfortunately this always pop up
  //its kind of nice to keep them low and well documented.
  enum schema_item_flags
    {
      CONST           = 0x001,  //constant, you can call get only once on them, no instance required
      DYNAMIC_ACCESS  = 0x002,  //items marked like this will need an explicit dynamic object cast
                                //at call time.
      EVENT           = 0x004,  //event, we'll use the get to obtain the event id and then exec
      EVENT_DECL      = 0x008,  //event declaration
      STATIC_FIELD    = 0x010,  //a field that already exists in the (c++) class, not need to create it

      CLASS           = 0x020,  //a class item
      TRANSIENT       = 0x040,
      OP_BLOCK_ASSIGN = 0x080,  //td: !!! this is the first unethical mechanism I use, the solution has to do
                                //with implementing the (*)= operators properly, users of this flag
                                //will tell the compiler not to perform an assigment after executing the operator
                                //for instance, the += of an array.
    };

  struct schema_item
    {
      schema_item() : flags (0), type(null) {}

      size_t   flags;
      Setter   set;
      Getter   get;
      Executer exec;
      schema*  type;
    };

  //the schema itself, it is the representation of types in xkp,
  //variants will carry them around so at any time you can access
  //the values either statically or dynamically

  //options
  enum schema_options
    {
      TYPE_ITERATED         = 0x01,
      TYPE_META             = 0x02,
      TYPE_NON_INSTANTIABLE = 0x04,
      TYPE_SIMPLE           = 0x08,
      TYPE_MUTABLE          = 0x10,
      TYPE_DYNAMIC          = 0x20,
    };

  struct schema_visitor
    {
      virtual void visit(const str& name, size_t idx, schema_item& item) = 0;
    };

  struct schema
    {
      public:
        virtual size_t  options()                                         = 0;
        virtual void    visit(schema_visitor* visitor)                    = 0;
        virtual bool    resolve(const str& name, schema_item& item)       = 0;
        virtual size_t  size()                                            = 0;
        virtual bool    at(size_t idx, schema_item& item)                 = 0;
        virtual void*   get_pointer(void**)                               = 0;
        virtual void    cast(const variant, schema*, variant&)            = 0;
        virtual bool    create(variant& result, param_list* args = null)  = 0;
        virtual bool    clone(const variant v, variant& result)           = 0;
        virtual void*   get_access()                                      = 0;

        //this process happened on the constructor of schemas, gcc tends to disagree
        //It should really be thought thru
        virtual void declare_base() = 0;
        virtual void declare()      = 0;
    };

  //this is the mechanism to bind types to schemas
  struct opaque_schema : schema
    {
      virtual size_t options();
      virtual void   visit(schema_visitor* visitor);
      virtual bool   resolve(const str& name, schema_item& item);
      virtual size_t size();
      virtual bool   at(size_t idx, schema_item& item);
      virtual void   cast(const variant v, schema* s, variant& r);
      virtual void*  get_pointer(void** src);
      virtual bool   create(variant& result, param_list* args = null);
      virtual bool   clone(const variant v, variant& result);
      virtual void*  get_access();
      virtual void   declare_base();
      virtual void   declare();
    };

  template <typename T>
  struct schema_descriptor
    {
      typedef opaque_schema type;
    };

  #define register_type(T, S) template <>  struct schema_descriptor<T> { typedef S type; };

  //and this is the accesor for the outside world, for instance:
  //schema* ts = type_schema<int>();
  template <typename T>
  inline schema* type_schema()
    {
      typedef typename schema_descriptor<T>::type type_type;

#if defined(__GNUC__)

      //silly rabbit
      static schema* type_singleton = null;
      if (!type_singleton)
        {
          //td: !!! type_schema is still being called inside constructors
          //(on the composite schema), this causes all kind of problems
          schema* tmp = new type_type;
          if (!type_singleton)
            {
              type_singleton = tmp;
              type_singleton->declare_base();
              type_singleton->declare();
            }
          else
            delete tmp;
        }

      return type_singleton;

#else

      static type_type result;
      static bool      declared = false;

      if (!declared)
        {
          declared = true;
          result.declare_base();
          result.declare();
        }

      return &result;

#endif
    };

  template <>
  inline schema* type_schema<void>()
    {
      static schema_descriptor<empty_type>::type result;
      return &result;
    };

namespace detail
  {
    // function pointer table
    struct fxn_ptr_table
      {
        void    (*static_delete)(void**);
        void    (*clone)(void* const*, void**);
        void    (*move)(void* const*,void**);
        schema* (*get_schema)();
      };

    template<bool is_small>
    struct fxns
      {
        template<typename T>
        struct type
          {
            static void static_delete(void** x)
              {
                reinterpret_cast<T*>(x)->~T();
              }
            static void clone(void* const* src, void** dest)
              {
                new(dest) T(*reinterpret_cast<T const*>(src));
              }
            static void move(void* const* src, void** dest)
              {
                reinterpret_cast<T*>(dest)->~T();
                *reinterpret_cast<T*>(dest) = *reinterpret_cast<T const*>(src);
              }
            static schema* get_schema()
              {
                return type_schema<T>();
              }
          };
      };

    // static functions for big value-types (bigger than a void*)
    template<>
    struct fxns<false>
      {
        template<typename T>
        struct type {
          static void static_delete(void** x)
            {
              delete(*reinterpret_cast<T**>(x));
            }
          static void clone(void* const* src, void** dest)
            {
              T** t = reinterpret_cast<T**>(dest);
              T* const* s = reinterpret_cast<T* const*>(src);
              T*  r = new T(**s);
              *t    = r;
            }
          static void move(void* const* src, void** dest)
            {
              (*reinterpret_cast<T**>(dest))->~T();
              **reinterpret_cast<T**>(dest) = **reinterpret_cast<T* const*>(src);
            }
          static schema* get_schema()
            {
              return type_schema<T>();
            }
        };
      };

    template<typename T>
    struct get_table
      {
        static const bool is_small = sizeof(T) <= sizeof(void*);

        static fxn_ptr_table* get()
          {
            static fxn_ptr_table static_table =
              {
                fxns<is_small>::template type<T>::static_delete,
                fxns<is_small>::template type<T>::clone,
                fxns<is_small>::template type<T>::move,
                fxns<is_small>::template type<T>::get_schema,
              };
            return &static_table;
          }
      };
  };

  //variants, these are how data is passed around in xkp
  //as you can see they carry their schema
  struct variant
    {
      detail::fxn_ptr_table* table;
      void*                  v;

      template <typename T>
      variant(const T& x)
        {
          table = detail::get_table<T>::get();
          if (sizeof(T) <= sizeof(void*))
            new(&v) T(x);
          else
            v = new T(x);
        }

      variant()
        {
          table = detail::get_table<empty_type>::get();
          v     = null;
        }

      variant(const variant& x)
        {
          table = detail::get_table<empty_type>::get();
          assign(x);
        }

      ~variant()
        {
          table->static_delete(&v);
        }

      // type checking
      template<typename T>
      bool is() const
        {
          return table == detail::get_table<T>::get();
        }

      // automatic casting operator
      template<typename T>
      operator T() const
        {
          detail::fxn_ptr_table* table_ = detail::get_table<T>::get();
          if (table == table_)
            {
              if (sizeof(T) <= sizeof(void*))
                {
                  return *reinterpret_cast<T const*>(&v);
                }
              else
                {
                  return *reinterpret_cast<T const*>(v);
                }
            }
          else
            {
              //xkp
              schema* s_ = type_schema<T>();
              variant custom_cast;
              get_schema()->cast(*this, s_, custom_cast);
              T result = custom_cast;
              return result;
            }
        }

      // assignment
      variant& assign(const variant& x)
        {
          if (table == x.table && false) //td: this version of move craps the bed with references somehow
            {
              table->move(&x.v, &v);
            }
          else
            {
              reset();
              x.table->clone(&x.v, &v);
              table = x.table;
            }
          return *this;
        }

      template <typename T>
      variant& assign(const T& x)
        {
          // are we copying between the same type?
          detail::fxn_ptr_table* table_ = detail::get_table<T>::get();
          if (table_ == table)
            {
              // if so, we can avoid deallocating and resuse memory
              if (sizeof(T) <= sizeof(void*))
                {
                  // create copy on-top of object pointer itself
                  new(&v) T(x);
                }
              else
                {
                  // create copy on-top of old version
                  new(v) T(x);
                }
            }
          else
            {
              reset();
              if (sizeof(T) <= sizeof(void*))
                {
                  // create copy on-top of object pointer itself
                  new(&v) T(x);
                  // update table pointer
                  table = table_;
                }
              else
                {
                  v = new T(x);
                  table = table_;
                }
            }

          return *this;
        }

      // assignment operator
      variant& operator = ( const variant& rhs )
        {
          return assign(rhs);
        }

      bool empty() const
        {
          return table == detail::get_table<empty_type>::get();
        }

      void reset()
        {
          if (empty()) return;

          table->static_delete(&v);
          table = detail::get_table<empty_type>::get();
          v = null;
        }

      schema* get_schema() const
        {
          return table->get_schema();
        }

      void* get_pointer()
        {
          return get_schema()->get_pointer( &v );
        }
    };

  //this is opaque's implementation, must happen after the variant is defined
  inline size_t opaque_schema::options()
    {
      return 0;
    }

  inline void opaque_schema::visit(schema_visitor* visitor)
    {
    }

  inline bool opaque_schema::resolve(const str& name, schema_item& item)
    {
      return false;
    }

  inline size_t opaque_schema::size()
    {
      return 0;
    }

  inline bool opaque_schema::at(size_t idx, schema_item& item)
    {
      return false;
    }

  inline void opaque_schema::cast(const variant v, schema* s, variant& r)
    {
      throw type_mismatch();
    }

  inline void* opaque_schema::get_pointer(void** src)
    {
      return null;
    }

  inline bool opaque_schema::create(variant& result, param_list* args)
    {
      return false;
    }

  inline bool opaque_schema::clone(const variant v, variant& result)
    {
      return false;
    }

  inline void* opaque_schema::get_access()
    {
      return this;
    }

  inline void opaque_schema::declare_base()
    {
    }

  inline void opaque_schema::declare()
    {
    }

  //this is the static-only version of the schema, it will provide no reflection
  //and it will be assigned by default to any type that doesn't explicitly define one
  template <typename T>
  struct default_schema : schema
    {
      virtual size_t options()                                   {return 0;}
      virtual void   visit(schema_visitor* visitor)              {}
      virtual bool   resolve(const str& name, schema_item& item) {return false;}
      virtual size_t size()                                      {return 0;}
      virtual bool   at(size_t idx, schema_item& item)           {return false;}

      virtual void cast(const variant v, schema* s, variant& r)
        {
          throw type_mismatch();
        }

      virtual void* get_pointer(void** src)
        {
          if (sizeof(T) <= sizeof(void*))
            return src;

          return *src;
        }

      virtual bool create(variant& result, param_list* args = null)
        {
          return false;
        }

      virtual bool clone(const variant v, variant& result)
        {
          return false;
        }

      virtual void* get_access()
        {
          return this;
        }

      virtual void declare_base()
        {
        }
    };


  //for polymorphic instances there is no way to deduce its internal type, so
  //we must provide it on a per-object basis. It pains me to no end.
  struct ITypedObject
    {
      virtual schema* get_type() = 0;
    };

  template <typename T>
  struct typed_object : virtual public ITypedObject
    {
      virtual schema* get_type()
        {
          return type_schema<T>();
        }
    };

  //a little named list of arguments
  struct param_list
    {
      variant get(size_t idx) const
        {
          return values_[idx];
        }

      variant get(const str& name) const
        {
          name_container::const_iterator it = names_.find(name);
          if (it != names_.end())
            {
              return values_[it->second];
            }

          return variant();
        }

			bool has(const str& name) const
				{
          name_container::const_iterator it = names_.find(name);
          return it != names_.end();
				}

      void add( variant value )
        {
          values_.push_back(value);
        }

      void add( const str name, variant value )
        {
          name_container::iterator it = names_.find(name);
          if (it != names_.end())
            {
              values_[it->second] = value;
            }
          else
            {
              size_t idx = values_.size();
              values_.push_back(value);

              if (!name.empty())
                {
                  names_.insert( std::pair<str, size_t>( name, idx) );
                }
            }
        }

      str get_name(size_t idx) const
        {
          name_container::const_iterator it = names_.begin();
          name_container::const_iterator nd = names_.end();

          for(; it != nd; it++)
            if (it->second == idx)
              return it->first;

          return "";
        }

      bool empty() const
        {
          return values_.empty();
        }

      size_t size() const
        {
          return values_.size();
        }
      private:
        typedef std::map<str, size_t> name_container;
        typedef std::vector<variant>  variant_container;

        name_container    names_;
        variant_container values_;
    };

  //the errors
  struct xkp_error
    {
      param_list error_data;
    };

  //safe casting
  template<typename T>
  T variant_cast(variant v, const T default_value)
    {
      T res;
      try
        {
          res = v;
        }
      catch(type_mismatch)
        {
          res = default_value;
        }

      return res;
    }

  template<>
  inline str variant_cast<str>(variant v, const str default_value)
    {
      schema* s = v.get_schema();
      if (s == type_schema<str>())
        return *((str*)v.get_pointer());

			try
        {
          str res = v;
					return res;
        }
      catch(type_mismatch)
        {
        }

      return default_value;
    }

  template<typename T>
  T* interface_cast(variant v)
    {
      return variant_cast<T*>(v, null);
    }

  inline schema* true_type(const variant& v)
    {
      ITypedObject* obj = interface_cast<ITypedObject>( v );
      if (obj)
        return obj->get_type();

      return v.get_schema();
    }

  //dynamic access to variants
  inline variant variant_exec(variant& v, const str& item, const param_list pl)
    {
      schema_item itm;
      schema* type = true_type(v);

      if (type->resolve(item, itm))
        {
          void* access = itm.flags&CLASS? type->get_access() : v.get_pointer();

          if (itm.exec)
            return itm.exec->exec( access, pl );

          if (itm.set)
            itm.set->set( access, pl.get( 0 ) );
        }

      return variant();
    }

  inline bool variant_try_exec(variant& v, const str& item, const param_list pl, variant& result)
    {
      schema_item itm;
      schema* type = true_type(v);

      if (type->resolve(item, itm))
        {
          void* access = itm.flags&CLASS? type->get_access() : v.get_pointer();

          if (itm.exec)
            result = itm.exec->exec( access, pl );
          else if (itm.set)
            itm.set->set( access, pl.get( 0 ) );
          else
            return false;

          return true;
        }

      return false;
    }

  inline variant variant_get(variant& v, const str& item)
    {
      schema_item itm;
      schema* type = true_type(v);
      if (type->resolve(item, itm) && itm.get)
        {
          void* access = itm.flags&CLASS? type->get_access() : v.get_pointer();
          return itm.get->get( access );
        }

      return variant();
    }

  inline bool variant_set(variant& v, const str& item, variant value)
    {
      schema_item itm;
      schema* type = true_type(v);
      if (type->resolve(item, itm) && itm.set)
        {
          void* access = itm.flags&CLASS? type->get_access() : v.get_pointer();
          itm.set->set( access, value );
          return true;
        }

      return false;
    }

  inline bool variant_try_get(variant& v, const str& item, variant& result)
    {
      schema_item itm;
      schema* type = true_type(v);
      if (type->resolve(item, itm) && itm.get)
        {
          void* access = itm.flags&CLASS? type->get_access() : v.get_pointer();
          result = itm.get->get( access );
          return true;
        }

      return false;
    }

  //and some utils
  inline variant var_exec( variant& v, const str& what )
    {
      param_list pl;
      return variant_exec(v, what, pl);
    }

  template <typename P0>
  variant var_exec( variant& v, const str& what, P0 p0 )
    {
      param_list pl; pl.add( p0 );
      return variant_exec(v, what, pl);
    }

  template <typename P0, typename P1>
  variant var_exec( variant& v, const str& what, P0 p0, P1 p1 )
    {
      param_list pl; pl.add( p0 ); pl.add( p1 );
      return variant_exec(v, what, pl);
    }

  template <typename P0, typename P1, typename P2>
  variant var_exec( variant& v, const str& what, P0 p0, P1 p1, P2 p2 )
    {
      param_list pl; pl.add( p0 ); pl.add( p1 ); pl.add( p2 );
      return variant_exec(v, what, pl);
    }

  template <typename P0, typename P1, typename P2, typename P3 >
  variant var_exec( variant& v, const str& what, P0 p0, P1 p1, P2 p2, P3 p3 )
    {
      param_list pl; pl.add( p0 ); pl.add( p1 ); pl.add( p2 ); pl.add( p3 );
      return variant_exec(v, what, pl);
    }

  template <typename P0, typename P1, typename P2, typename P3, typename P4 >
  variant var_exec( variant& v, const str& what, P0 p0, P1 p1, P2 p2, P3 p3, P4 p4 )
    {
      param_list pl; pl.add( p0 ); pl.add( p1 ); pl.add( p2 ); pl.add( p3 ); pl.add( p4 );
      return variant_exec(v, what, pl);
    }

  //utils for get and set
  template <typename T>
  T var_get( variant& v, const str& what )
    {
      return variant_get(v, what);
    }

  template <typename T>
  T var_get( variant& v, const str& what, T& default_value )
    {
      T res;
      try
        {
          res = (T)variant_get(v, what);
        }
      catch(type_mismatch)
        {
          res = default_value;
        }

      return res;
    }

  template <typename T>
  void var_set( variant& v, const str& what, T value )
    {
      variant_set(v, what, value);
    }

  //simple types
  template <typename T>
  struct simpletype_schema : default_schema<T>
    {
      virtual void declare()
        {
          //empty, for now
        }

      virtual size_t  options()
        {
          return TYPE_SIMPLE;
        }

      virtual void cast(const variant src, schema* s, variant& result)
        {
          if (s == type_schema<str>())
            {
              //string conversion
              T t = src;
              result = boost::lexical_cast<str>( t );
            }
          else if (s == type_schema<bool>()) //bool conversion
            {
              if (src.is<str>())
                result = true; //strings are always true
              else if (src.is<int>())
                {
                  int ii = src;
                  result = ii != 0;
                }
              else
                throw type_mismatch(); //td: do the rest of simple types
            }
          else if (s == type_schema<int>())
						{
							if (src.is<float>())
								{
									float ii = src;
									result = (int)ii;
								}
              else
                throw type_mismatch(); 
						}
          else if (s == type_schema<float>())
						{
							if (src.is<int>())
								{
									int ii = src;
									result = (float)ii;
								}
              else
                throw type_mismatch(); //td: do the rest of simple types
						}
					else
            throw type_mismatch(); //td: simple type casting (ints, etc)
        }

      virtual bool create(variant& result, param_list* args = null)
        {
          T res;
          result = res;
          return true;
        }

      virtual bool clone(const variant v, variant& result)
        {
          result = v;
          return true;
        }
    };

  //variant
  struct variant_schema : default_schema<variant>
    {
      virtual bool create(variant& result, param_list* args = null)
        {
          result.reset();
          return true;
        }

      virtual void cast(const variant src, schema* s, variant& result)
        {
          result = src;
        }

      virtual bool clone(const variant v, variant& result)
        {
          result = v;
          return true;
        }

      virtual void* get_pointer(void** src)
        {
          return null;
        }

      virtual void declare()
        {
        }
    };

  register_type(int,        simpletype_schema<int>);
  register_type(long,       simpletype_schema<long>);
  register_type(float,      simpletype_schema<float>);
  register_type(double,     simpletype_schema<double>);
  register_type(bool,       simpletype_schema<bool>);
  register_type(str,        simpletype_schema<str>);
  register_type(variant,    variant_schema);

  struct simpletype_visitor
    {
      virtual void int_(int value)        = 0;
      virtual void long_(long value)      = 0;
      virtual void float_(float value)    = 0;
      virtual void double_(double value)  = 0;
      virtual void bool_(bool value)      = 0;
      virtual void str_(const str& value) = 0;
      virtual void unknown()              = 0;
    };

  inline void visit_simpletype(const variant v, simpletype_visitor* visitor)
    {
      if (v.is<int>())
        {
          int value = v;
          visitor->int_(value);
        }
      else if (v.is<long>())
        {
          long value = v;
          visitor->long_(value);
        }
      else if (v.is<float>())
        {
          float value = v;
          visitor->float_(value);
        }
      else if (v.is<double>())
        {
          double value = v;
          visitor->double_(value);
        }
      else if (v.is<bool>())
        {
          bool value = v;
          visitor->bool_(value);
        }
      else if (v.is<str>())
        {
          str value = v;
          visitor->str_(value);
        }
      else
        visitor->unknown();
    }

  //more utils!
  struct schema_collector : schema_visitor
    {
      typedef std::map<str, schema_item> container;

      virtual void visit(const str& name, size_t idx, schema_item& item)
        {
          c.insert( std::pair<str, schema_item>( name, item ) );
        }

      container::iterator begin()
        {
          return c.begin();
        }

      container::iterator end()
        {
          return c.end();
        }

      container c;
    };

}

#endif
