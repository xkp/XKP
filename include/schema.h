
#ifndef XKP_SCHEMAS
#define XKP_SCHEMAS

#include "base.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <map>

namespace xkp
{
  namespace detail
  {
    template <typename T, typename M>
    struct returns0 : public executer
      {
        M m_;
        returns0( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            return boost::bind( m_, static_cast<T*>(instance))();
          }
      };

    template <typename T, typename M>
    struct void0 : public executer
      {
        M m_;
        void0( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            boost::bind( m_, static_cast<T*>(instance))();
            return variant();
          }
      };

    template <typename T, typename M>
    struct returns1 : public executer
      {
        M m_;
        returns1( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            return boost::bind( m_, static_cast<T*>(instance), _1)( args.get(0) );
          }
      };

    template <typename T, typename M>
    struct void1 : public executer
      {
        M m_;
        void1( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            boost::bind( m_, static_cast<T*>(instance), _1)( args.get(0) );
            return variant();
          }
      };

    template <typename T, typename M>
    struct void45 : public executer
      {
        M m_;
        void45( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            boost::bind( m_, static_cast<T*>(instance), _1)( args );
            return variant();
          }
      };

    template <typename T, typename M>
    struct returns2 : public executer
      {
        M m_;
        returns2( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            return boost::bind( m_, static_cast<T*>(instance), _1, _2)( args.get(0), args.get(1));
          }
      };

    template <typename T, typename M>
    struct void2 : public executer
      {
        M m_;
        void2( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            boost::bind( m_, static_cast<T*>(instance), _1, _2)( args.get(0), args.get(1));
            return variant();
          }
      };

    template <typename T, typename M>
    struct returns3 : public executer
      {
        M m_;
        returns3( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            return boost::bind( m_, static_cast<T*>(instance), _1, _2, _3)( args.get(0), args.get(1), args.get(2));
          }
      };

    template <typename T, typename M>
    struct void3 : public executer
      {
        M m_;
        void3( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            boost::bind( m_, static_cast<T*>(instance), _1, _2, _3)( args.get(0), args.get(1), args.get(2));
            return variant();
          }
      };

    template <typename T, typename M>
    struct returns4 : public executer
      {
        M m_;
        returns4( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            return boost::bind( m_, static_cast<T*>(instance), _1, _2, _3, _4)( args.get(0), args.get(1), args.get(2), args.get(3));
          }
      };

    template <typename T, typename M>
    struct void4 : public executer
      {
        M m_;
        void4( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            boost::bind( m_, static_cast<T*>(instance), _1, _2, _3, _4)( args.get(0), args.get(1), args.get(2), args.get(3));
            return variant();
          }
      };

    template <typename T, typename M>
    struct returns5 : public executer
      {
        M m_;
        returns5( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            return boost::bind( m_, static_cast<T*>(instance), _1, _2, _3, _4, _5)( args.get(0), args.get(1), args.get(2), args.get(3), args.get(4));
          }
      };

    template <typename T, typename M>
    struct void5 : public executer
      {
        M m_;
        void5( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            boost::bind( m_, static_cast<T*>(instance), _1, _2, _3, _4, _5)( args.get(0), args.get(1), args.get(2), args.get(3), args.get(4));
            return variant();
          }
      };

    template <typename T, typename RT, typename M, int N>
    struct executer_type
      {
        //compiler error, no "type"
      };

    template <typename T, typename RT, typename M>
    struct executer_type<T, RT, M, 0> { typedef returns0<T, M> type; };

    template <typename T, typename M>
    struct executer_type<T, void, M, 0> { typedef void0<T, M> type; };

    template <typename T, typename RT, typename M>
    struct executer_type<T, RT, M, 1> { typedef returns1<T, M> type; };

    template <typename T, typename M>
    struct executer_type<T, void, M, 1> { typedef void1<T, M> type; };

    template <typename T, typename RT, typename M>
    struct executer_type<T, RT, M, 2> { typedef returns2<T, M> type; };

    template <typename T, typename M>
    struct executer_type<T, void, M, 2> { typedef void2<T, M> type; };

    template <typename T, typename RT, typename M>
    struct executer_type<T, RT, M, 3> { typedef returns3<T, M> type; };

    template <typename T, typename M>
    struct executer_type<T, void, M, 3> { typedef void3<T, M> type; };

    template <typename T, typename RT, typename M>
    struct executer_type<T, RT, M, 4> { typedef returns4<T, M> type; };

    template <typename T, typename M>
    struct executer_type<T, void, M, 4> { typedef void4<T, M> type; };

    template <typename T, typename RT, typename M>
    struct executer_type<T, RT, M, 5> { typedef returns5<T, M> type; };

    template <typename T, typename M>
    struct executer_type<T, void, M, 5> { typedef void5<T, M> type; };

    template <typename T, typename M>
    struct functor_returns0 : public executer
      {
        M m_;
        functor_returns0( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            return boost::bind( m_, _1)( static_cast<T*>(instance) );
          }
      };

    template <typename T, typename M>
    struct functor_void0 : public executer
      {
        M m_;
        functor_void0( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            boost::bind( m_, _1)(static_cast<T*>(instance));
            return variant();
          }
      };

    template <typename T, typename M>
    struct functor_returns1 : public executer
      {
        M m_;
        functor_returns1( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            return boost::bind( m_, _1, _2)( static_cast<T*>(instance), args.get(0) );
          }
      };

    template <typename T, typename M>
    struct functor_void1 : public executer
      {
        M m_;
        functor_void1( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            boost::bind( m_, _1, _2)( static_cast<T*>(instance), args.get(0) );
            return variant();
          }
      };

    template <typename T, typename M>
    struct functor_returns2 : public executer
      {
        M m_;
        functor_returns2( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            return boost::bind(m_, _1, _2, _3)( static_cast<T*>(instance), args.get(0), args.get(1));
          }
      };

    template <typename T, typename M>
    struct functor_void2 : public executer
      {
        M m_;
        functor_void2( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            boost::bind( m_, _1, _2, _3)( static_cast<T*>(instance), args.get(0), args.get(1));
            return variant();
          }
      };

    template <typename T, typename M>
    struct functor_returns3 : public executer
      {
        M m_;
        functor_returns3( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            return boost::bind( m_, _1, _2, _3, _4)( static_cast<T*>(instance), args.get(0), args.get(1), args.get(2));
          }
      };

    template <typename T, typename M>
    struct functor_void3 : public executer
      {
        M m_;
        functor_void3( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            boost::bind( m_, _1, _2, _3, _4)( static_cast<T*>(instance), args.get(0), args.get(1), args.get(2));
            return variant();
          }
      };

    template <typename T, typename M>
    struct functor_returns4 : public executer
      {
        M m_;
        functor_returns4( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            return boost::bind( m_, _1, _2, _3, _4, _5)( static_cast<T*>(instance), args.get(0), args.get(1), args.get(2), args.get(3));
          }
      };

    template <typename T, typename M>
    struct functor_void4 : public executer
      {
        M m_;
        functor_void4( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            boost::bind( m_, _1, _2, _3, _4, _5)( static_cast<T*>(instance), args.get(0), args.get(1), args.get(2), args.get(3));
            return variant();
          }
      };

    template <typename T, typename M>
    struct functor_returns5 : public executer
      {
        M m_;
        functor_returns5( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            return boost::bind( m_, _1, _2, _3, _4, _5, _6)( static_cast<T*>(instance), args.get(0), args.get(1), args.get(2), args.get(3), args.get(4));
          }
      };

    template <typename T, typename M>
    struct functor_void5 : public executer
      {
        M m_;
        functor_void5( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            boost::bind( m_, _1, _2, _3, _4, _5, _6)( static_cast<T*>(instance), args.get(0), args.get(1), args.get(2), args.get(3), args.get(4));
            return variant();
          }
      };

    //and now I have to refactor all this for global functions... how am I using bind wrong?
    template <typename T, typename RT, typename M, int N>
    struct functor_executer_type
      {
        //compiler error, no "type"
      };

    template <typename T, typename RT, typename M>
    struct functor_executer_type<T, RT, M, 0> { typedef functor_returns0<T, M> type; };

    template <typename T, typename M>
    struct functor_executer_type<T, void, M, 0> { typedef functor_void0<T, M> type; };

    template <typename T, typename RT, typename M>
    struct functor_executer_type<T, RT, M, 1> { typedef functor_returns1<T, M> type; };

    template <typename T, typename M>
    struct functor_executer_type<T, void, M, 1> { typedef functor_void1<T, M> type; };

    template <typename T, typename RT, typename M>
    struct functor_executer_type<T, RT, M, 2> { typedef functor_returns2<T, M> type; };

    template <typename T, typename M>
    struct functor_executer_type<T, void, M, 2> { typedef functor_void2<T, M> type; };

    template <typename T, typename RT, typename M>
    struct functor_executer_type<T, RT, M, 3> { typedef functor_returns3<T, M> type; };

    template <typename T, typename M>
    struct functor_executer_type<T, void, M, 3> { typedef functor_void3<T, M> type; };

    template <typename T, typename RT, typename M>
    struct functor_executer_type<T, RT, M, 4> { typedef functor_returns4<T, M> type; };

    template <typename T, typename M>
    struct functor_executer_type<T, void, M, 4> { typedef functor_void4<T, M> type; };

    template <typename T, typename RT, typename M>
    struct functor_executer_type<T, RT, M, 5> { typedef functor_returns5<T, M> type; };

    template <typename T, typename M>
    struct functor_executer_type<T, void, M, 5> { typedef functor_void5<T, M> type; };

    template <typename T, typename M>
    struct dynamic_method_executer : public executer
      {
        M m_;
        dynamic_method_executer( M m ): m_(m) {}

        virtual variant exec( void* instance, const param_list args )
          {
            boost::bind(m_, static_cast<T*>(instance))(args);
            return variant();
          }
      };

    //properties
    template <typename T, typename G>
    struct property_getter : getter
      {
        G g_;
        property_getter( G g ): g_(g) {}

        virtual variant get ( void* instance )
          {
            return boost::bind( g_, static_cast<T*>(instance))();
          }
      };

    template <typename T, typename S>
    struct property_setter : setter
      {
        S s_;
        property_setter( S s ): s_(s) {}

        virtual void set ( void* instance, const variant value )
          {
            boost::bind( s_, static_cast<T*>(instance), _1)( value );
          }
      };

    template <typename T, typename RT>
    struct member_getter : getter
      {
        RT T::* mp_;

        member_getter(RT T::* mp) : mp_(mp) {}

        virtual variant get ( void* instance )
          {
            T* self = static_cast<T*>(instance);
            return self->*mp_;
          }
      };

    template <typename T, typename RT>
    struct static_field_getter : getter
      {
        RT T::* mp_;

        static_field_getter(RT T::* mp) : mp_(mp) {}

        virtual variant get ( void* instance )
          {
            T* self = static_cast<T*>(instance);
            return &(self->*mp_);
          }
      };

    template <typename T, typename G>
    struct functor_property_getter : getter
      {
        G g_;
        functor_property_getter( G g ): g_(g) {}

        virtual variant get ( void* instance )
          {
            return boost::bind(g_, _1)(static_cast<T*>(instance));
          }
      };

    template <typename T, typename S>
    struct functor_property_setter : setter
      {
        S s_;
        functor_property_setter( S s ): s_(s) {}

        virtual void set ( void* instance, const variant value )
          {
            boost::bind( s_, _1, _2)( static_cast<T*>(instance), value );
          }
      };

    template <typename T, typename RT>
    struct member_setter : setter
      {
        RT T::* mp_;

        member_setter(RT T::* mp) : mp_(mp) {}

        virtual void set ( void* instance, const variant value )
          {
            T* self = static_cast<T*>(instance);
			      self->*mp_ = (RT)value;
          }
      };

    template <typename T>
    struct member_setter<T, str> : setter
      {
        str T::* mp_;

        member_setter(str T::* mp) : mp_(mp) {}

        virtual void set ( void* instance, const variant value )
          {
            T* self = static_cast<T*>(instance);
            str v = value;
			self->*mp_ = v;
          }
      };
  }

  struct caster
    {
      virtual variant cast(const variant src) = 0;
    };

  typedef reference<caster> Caster;

  //some defult casters
  template <typename S, typename D>
  struct ref2super : caster
    {
      virtual variant cast(const variant src)
        {
          reference<S> src_ = src;
          D* res_pointer = static_cast<D*>( src_.get() );

          reference<D> res_;
          res_.px = res_pointer;
          res_.pn = src_.pn;
          return res_;
        }
    };

  template <typename S, typename D>
  struct ref2super_pointer  : caster
    {
      virtual variant cast(const variant src)
        {
          reference<S> src_ = src;
          D* res_ = static_cast<D*>( src_.get() );
          return res_;
        }
    };

  template <typename S, typename D>
  struct ref2super_obj  : caster
    {
      virtual variant cast(const variant src)
        {
          reference<S> src_ = src;
          D* res_ = static_cast<D*>( src_.get() );
          return *res_;
        }
    };

  template <typename S, typename D>
  struct pointer2super  : caster
    {
      virtual variant cast(const variant src)
        {
          S* src_ = src;
          D* res_ = static_cast<D*>( src_ );
          return res_;
        }
    };

  template <typename S, typename D>
  struct pointer2super_obj : caster
    {
      virtual variant cast(const variant src)
        {
          S* src_ = src;
          D* res_ = static_cast<D*>( src_ );
          return *res_;
        }
    };

  struct basic_schema : schema
    {
      basic_schema()
        {
        }
      
      virtual void declare_base()
        {
        }
        
      virtual size_t options()
        {
          return 0;
        }

      virtual void visit(schema_visitor* visitor)
        {
          vmt_indices::iterator it = indices_.begin();
          vmt_indices::iterator nd = indices_.end();

          for(; it != nd; it++)
            {
              visitor->visit( it->first, it->second, vmt_[it->second]);
            }
        }

      virtual bool resolve(const str& name, schema_item& item)
        {
          vmt_indices::iterator it = indices_.find(name);
          if (it != indices_.end())
            {
              item = vmt_[it->second];
              return true;
            }

          return false;
        }

      virtual size_t size()
        {
          return vmt_.size();
        }

      virtual bool at(size_t idx, schema_item& item)
        {
          if (idx < vmt_.size())
            {
              item = vmt_[idx];
              return true;
            }
          return false;
        }

    virtual bool create(variant& result, param_list* args = null)
      {
        return false;
      }

    virtual void* get_access()
      {
        return this;
      }

    protected:
      typedef std::vector<schema_item> vmt;
      typedef std::map <str, size_t>   vmt_indices;
      typedef std::pair<str, size_t>   vmt_pair;

      struct cast_types
        {
          schema* src;
          schema* dest;

          bool operator < (const cast_types& k) const
            {
              if (k.src < src)      return false;
              else if (k.src > src) return true;

              if (k.dest < dest)      return false;
              else if (k.dest > dest) return true;

              return false;
            }
        };

      typedef std::map<cast_types, Caster>  caster_map;
      typedef std::pair<cast_types, Caster> caster_pair;

      vmt         vmt_;
      vmt_indices indices_;
      caster_map  casters_;

      void register_method(const str& name, executer* exec, schema* type, size_t flags)
        {
          schema_item itm;
          itm.flags = flags;
          itm.exec  = Executer(exec);
          itm.type  = type;

          indices_.insert(vmt_pair(name, vmt_.size()));
          vmt_.push_back(itm);
        }

      void register_property(const str& name, getter* get, setter* set, schema* type, size_t flags)
        {
          schema_item itm;
          itm.flags = flags;
          itm.get   = Getter(get);
          itm.set   = Setter(set);
          itm.type  = type;

          indices_.insert(vmt_pair(name, vmt_.size()));
          vmt_.push_back(itm);
        }

      void register_caster(cast_types& ct, Caster c)
        {
          casters_.insert( caster_pair(ct, c) );
        }

    public:
      //casting, pretty slow for now
      template <typename S, typename D, typename C>
      void caster()
        {
          cast_types ct;
          ct.src = type_schema<S>();
          ct.dest = type_schema<D>();
          register_caster(ct, Caster(new C));
        }

      virtual void cast(const variant src, schema* s, variant& result)
        {
          cast_types ct;
          ct.src  = src.get_schema();
          ct.dest = s;
          caster_map::iterator it = casters_.find(ct);
          if (it != casters_.end())
            {
              result = it->second->cast(src);
              return;
            }

          throw type_mismatch();
        }
    };

  struct composite_schema : schema
    {
      composite_schema(schema* _delgate) :
        delgate_(_delgate)
        {
        }

      virtual size_t  options()                                             {return delgate_->options();}
      virtual void    visit(schema_visitor* visitor)                        {return delgate_->visit(visitor);}
      virtual bool    resolve(const str& name, schema_item& item)           {return delgate_->resolve(name, item);}
      virtual size_t  size()                                                {return delgate_->size();}
      virtual bool    at(size_t idx, schema_item& item)                     {return delgate_->at(idx, item);}
      virtual void    cast(const variant src, schema* ds, variant& result)  {delgate_->cast(src, ds, result);}
      virtual void*   get_pointer(void** src)                               {return delgate_->get_pointer(src);}
      virtual bool    create(variant& result, param_list* args = null)      {return delgate_->create(result, args);}
      virtual bool    clone(const variant v, variant& result)               {return delgate_->clone(v, result);}
      virtual void*   get_access()                                          {return delgate_;}
      virtual void    declare_base()                                        {} 
      virtual void    declare()                                             {} 

      schema* delgate_;
    };

  template<typename T>
  struct object_schema : basic_schema
    {
      template <typename RT, int N, typename M>
      void method_(const str& name, M m, size_t flags = 0)
        {
          typedef typename detail::executer_type<T, RT, M, N>::type et;
          register_method(name, new et( m ), type_schema<RT>(),  flags);
        }

      template <typename M>
      void dynamic_method_(const str& name, M m, size_t flags = 0)
        {
          typedef detail::void45<T, M> et;
          register_method(name, new et(m), type_schema<void>(), flags);
        }

      template <typename RT, typename G, typename S>
      void property_(const str& name, G g, S s, size_t flags = 0 )
        {
          register_property(name, new detail::property_getter<T, G>( g ), new detail::property_setter<T, S>( s ), type_schema<RT>(), flags);
        }

      template <typename RT>
      void property_(const str& name, RT T::* mp, size_t flags = 0 )
        {
          register_property(name, new detail::member_getter<T, RT>( mp ), new detail::member_setter<T, RT>( mp ), type_schema<RT>(), flags);
        }

      template <typename RT, typename G>
      void readonly_property(const str& name, G g, size_t flags = 0 )
        {
          register_property(name, new detail::property_getter<T, G>( g ), null, type_schema<RT>(), flags);
        }

      template <typename RT, typename S>
      void writeonly_property(const str& name, S s, size_t flags = 0 )
        {
          register_property(name, null, new detail::property_setter<T, S>( s ), type_schema<RT>(), flags);
        }

      template <typename RT>
      void static_field(const str& name, RT T::* mp, size_t flags = 0 )
        {
          register_property(name, new detail::static_field_getter<T, RT>( mp ), null, type_schema<RT>(), flags|STATIC_FIELD);
        }

      template <typename C, typename RT, typename G>
      void class_property(const str& name, G g, size_t flags = 0 )
        {
          register_property(name, new detail::property_getter<C, G>( g ), null, type_schema<RT>(), flags|CLASS);
        }

      template <typename I>
      void inherit_from()
        {
          typedef reference<T> my_ref;
          typedef T*           my_pointer;
          typedef reference<I> their_ref;
          typedef I*           their_pointer;

          caster<my_ref, their_ref,         ref2super<T, I>          >();
          caster<my_ref, their_pointer,     ref2super_pointer<T, I>  >();
          caster<my_ref, I,                 ref2super_obj<T, I>      >();
          caster<my_pointer, their_pointer, pointer2super<T, I>      >();
          caster<my_pointer, I,             pointer2super_obj<T, I>  >();
        }

      template <typename I>
      void implements()
        {
          typedef reference<T> my_ref;
          typedef T*           my_pointer;
          typedef reference<I> their_ref;
          typedef I*           their_pointer;

          caster<my_ref, their_ref,         ref2super<T, I>          >();
          caster<my_ref, their_pointer,     ref2super_pointer<T, I>  >();
          caster<my_pointer, their_pointer, pointer2super<T, I>      >();
        }

      template <typename RT, int N, typename M>
      void functor_method(const str& name, M m, size_t flags = 0)
        {
          typedef typename detail::functor_executer_type<T, RT, M, N>::type et;
          register_method(name, new et( m ), type_schema<RT>(),  flags);
        }

      template <typename RT, typename G, typename S>
      void functor_property(const str& name, G g, S s, size_t flags = 0 )
        {
          register_property(name, new detail::functor_property_getter<T, G>( g ), new detail::functor_property_setter<T, S>( s ), type_schema<RT>(), flags);
        }

      template <typename RT, typename G>
      void functor_readonly_property(const str& name, G g, size_t flags = 0 )
        {
          register_property(name, new detail::functor_property_getter<T, G>( g ), null, type_schema<RT>(), flags);
        }

      //schema
      virtual void* get_pointer(void** src)
        {
          if (sizeof(T) <= sizeof(void*))
            return src;

          return *src;
        }

      virtual bool clone(const variant v, variant& result)
        {
          result = v;
          return true;
        }

      virtual bool create(variant& result, param_list* args = null)
        {
          result = reference<T>( new T() );
          return true;
        }
    };

  template <typename T>
  struct pointer_schema : composite_schema
    {
      pointer_schema():
        composite_schema(type_schema<T>())
        {
        }

      virtual void* get_pointer(void** src)
        {
          return *src;
        }

      virtual void cast(const variant src, schema* s, variant& result)
        {
          if (delgate_ == s)
            {
              T* self = src;
              result  = *self;
            }
          else if (s == type_schema<bool>())
            {
              T* self = src;
              bool not_null = self != null;
              result  = not_null;
            }
          else
            composite_schema::cast(src, s, result);
        }

      virtual bool clone(const variant v, variant& result)
        {
          T* t = v;
          result = new T(*t);
          return true;
        }
    };

  template <typename T>
  struct reference_schema : composite_schema
    {
      reference_schema():
        composite_schema(type_schema<T>())
        {
        }

      virtual void* get_pointer(void** src)
        {
          reference<T>* t = reinterpret_cast<reference<T>*>(*src);
          return t->get();
        }

      virtual void cast(const variant src, schema* s, variant& result)
        {
          if (s == delgate_)
            {
              reference<T> t = src;

              T* self = t.get();
              result  = *self;
            }
          else if (s == type_schema<T*>())
            {
              reference<T> t = src;
              result  = t.get();
            }
          else if (s == type_schema<bool>())
            {
              reference<T> t = src;
              bool not_null = t;
              result  = not_null;
            }
          else
            composite_schema::cast(src, s, result);
        }

      virtual bool clone(const variant v, variant& result)
        {
          reference<T> t = v;
          result = reference<T>( new T(*(t.get())) );
          return true;
        }
    };

  #define register_complete_type(T, S) \
    template <>  struct schema_descriptor<T>  { typedef S type; }; \
    template <>  struct schema_descriptor<T*> { typedef pointer_schema<T> type; }; \
    template <>  struct schema_descriptor< reference<T> > { typedef reference_schema<T> type; };

  //interfaces
  template<typename T>
  struct interface_schema : basic_schema
    {
      template <typename I>
      void implements()
        {
          typedef reference<T> my_ref;
          typedef T*           my_pointer;
          typedef reference<I> their_ref;
          typedef I*           their_pointer;

          caster<my_ref, their_ref,         ref2super<T, I>          >();
          caster<my_ref, their_pointer,     ref2super_pointer<T, I>  >();
          caster<my_pointer, their_pointer, pointer2super<T, I>      >();
        }

      virtual size_t  options()
        {
          return TYPE_NON_INSTANTIABLE;
        }

      virtual void* get_pointer(void** src)
        {
          assert(false);
          return null;
        }

      virtual bool clone(const variant v, variant& result)
        {
          assert(false);
          return false;
        }
    };

  template <typename T>
  struct interface_pointer_schema : composite_schema
    {
      interface_pointer_schema():
        composite_schema(type_schema<T>())
        {
        }

      virtual void* get_pointer(void** src)
        {
          return *src;
        }
    };

  template <typename T>
  struct interface_reference_schema : composite_schema
    {
      interface_reference_schema():
        composite_schema(type_schema<T>())
        {
        }

      virtual void* get_pointer(void** src)
        {
          reference<T>* t = reinterpret_cast<reference<T>*>(*src);
          return t->get();
        }

      virtual void cast(const variant src, schema* s, variant& result)
        {
          if (s == type_schema<T*>())
            {
              reference<T> t = src;
              result  = t.get();
            }
          else
            composite_schema::cast(src, s, result);
        }
    };

  #define register_interface_type(T, S) \
    template <>  struct schema_descriptor<T>  { typedef S type; }; \
    template <>  struct schema_descriptor<T*> { typedef interface_pointer_schema<T> type; }; \
    template <>  struct schema_descriptor< reference<T> > { typedef interface_reference_schema<T> type; };

  //iterators
  struct iterator
    {
      iterator(variant impl) : impl_(impl) {}

      iterator operator ++()
        {
          variant result = var_exec(impl_, "++"); assert(!result.empty());
          return iterator( result );
        }

      bool operator == (const iterator& other)
        {
          bool result = var_exec(impl_, "==", other.impl_);
          return result;
        }

      bool operator != (const iterator& other)
        {
          return !(*this == other);
        }

      bool operator == (const variant& other)
        {
          bool result = var_exec(impl_, "==", other);
          return result;
        }

      variant operator *()
        {
          return variant_get(impl_, "value");
        }

      variant impl_;
    };

  struct iterable
    {
      iterable(variant impl) : impl_(impl) {}

      iterator begin()
        {
          variant result = variant_get(impl_, "begin");
          return iterator(result);
        }

      iterator end()
        {
          variant result = variant_get(impl_, "end");
          return iterator(result);
        }

      void insert(const variant& value)
        {
          param_list pl;
          pl.add( value );
          variant_exec(impl_, "insert", pl); //td: !!! must check if the interface is met
        }

      schema* iterated_type()
        {
          variant result = variant_get(impl_, "iterated_type");
          return interface_cast<schema>(result);
        }

      variant impl_;
    };

  template <typename T>
  struct vector_schema : object_schema< std::vector<T> >
    {
      typedef std::vector<T>                    this_type;
      typedef typename std::vector<T>::iterator this_iterator;

      static this_iterator begin(std::vector<T>* v)
        {
          return v->begin();
        }

      static this_iterator end(std::vector<T>* v)
        {
          return v->end();
        }

      static void insert(std::vector<T>* v, const T t)
        {
          v->push_back( t );
        }

      static schema* iterated_type(std::vector<T>* v)
        {
          return type_schema<T>();
        }

      virtual void declare()
        {
          this->template functor_readonly_property<this_iterator>("begin",         &vector_schema::begin);
          this->template functor_readonly_property<this_iterator>("end",           &vector_schema::end);
          this->template functor_method<void, 1>                 ("insert",        &vector_schema::insert);
          this->template functor_readonly_property<schema*>      ("iterated_type", &vector_schema::iterated_type);
        }

      virtual size_t options() {return TYPE_ITERATED;}
    };

  template <typename T>
  struct vector_iterator_schema : object_schema< typename std::vector<T>::iterator >
    {
      typedef typename std::vector<T>::iterator this_type;

      static this_type advance(this_type* it)
        {
          this_type& my_ref = *it;
          my_ref++;
          return my_ref;
        }

      static bool compare(this_type* it1, this_type it2)
        {
          return (*it1) == it2;
        }

      static T value(this_type* it)
        {
          return *(*it);
        }

      virtual void declare()
        {
          this->template functor_method<this_type, 0>("++",    &vector_iterator_schema::advance);
          this->template functor_method<bool,      1>("==",    &vector_iterator_schema::compare);
          this->template functor_readonly_property<T>("value", &vector_iterator_schema::value  );
        }
    };


  #define register_iterator(T) \
    register_complete_type(std::vector<T>, vector_schema<T>) \
    template <>  struct schema_descriptor< std::vector<T>::iterator > { typedef vector_iterator_schema<T> type; };

}

#endif
