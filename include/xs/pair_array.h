
#ifndef XS_PAIR_ARRAY_HH
#define XS_PAIR_ARRAY_HH

#include <schema.h>
#include <map>

namespace xkp
{
  template<typename F, typename S>
  struct pair_value
    {
      public:
        pair_value() {}
        pair_value(F& _f, S& _s): f(_f), s(_s) {}
          
      public:
        F f;
        S s;
    };

  template<typename F, typename S>
  struct pair_iterator
    {
      typedef typename std::map<F, S> container;

      pair_iterator()
        {
          assert(false); //should never be created
        }
        
      ~pair_iterator()
        {
        }
        
      pair_iterator(const pair_iterator& other)
        {
          ref_ = other.ref_;
          it_  = other.it_; 
        }

      pair_iterator(container& ref, typename container::iterator& it):
        ref_(ref),
        it_(it)
        {
        }
    
      pair_iterator advance()
        {
          it_++;
          return pair_iterator(ref_, it_);
        }
        
      bool compare(pair_iterator other)
        {
          if (other.ref_ != ref_)
            return false;
            
          return other.it_ == it_;
        }
        
      variant value()
        {
          return pair_value<F, S>(it->first, it->second);
        }
        
      public:
        typename container::iterator it_;
        typename container&          ref_; //td: this is as unsafe as it gets
    };

  template<typename F, typename S>
  struct pair_array
    {
      typedef std::map<F, S>      container;
      typedef pair_iterator<F, S> iterator; 
      
      pair_array():
        ref_(null)
        {
        }
        
      pair_array(const pair_array& other):
        ref_(other.ref_)
        {
        }

      ~pair_array()
        {
        }

      iterator begin()
        {
          return iterator(ref_, ref_->begin());
        }
      
      iterator end()
        {
          return iterator(ref_, ref_->end());
        }
        
      void insert(F f, S s)
        {
          ref_.insert( std::pair<F, S>(f, s) );
        }
        
      schema* iterated_type()
        {
          return type_schema<pair_value<F, S>>();
        }
      private:
        container* ref_;
    };
    
  typedef pair_array<variant, variant> dynamic_pair_array;
  typedef reference<dynamic_pair_array> DynamicMap;
    
  //array types
  template<typename F, typename S>
  struct pair_iterator_schema : object_schema< pair_iterator<F, S> >  
    {
      typedef pair_iterator<F, S> this_type;
    
      pair_iterator_schema()
        {
          method_<this_type, 0>("++",    &this_type::advance);
          method_<bool,      1>("==",    &this_type::compare);
          readonly_property<T> ("value", &this_type::value  );
        }
    };
    
  template<typename T, typename F, typename S>
  struct pair_array_schema : object_schema<T>  
    {
      pair_array_schema()
        {
          class_property<pair_array_schema, schema*> ("iterated_type", &pair_array_schema::iterated_type);

          readonly_property<T::iterator>("begin",         &T::begin);
          readonly_property<T::iterator>("end",           &T::end);
          method_<void, 1>              ("insert",        &T::insert);
        }
        
      virtual size_t options()
        {
          return TYPE_ITERATED;
        }

      virtual schema* iterated_type()
        {
          return type_schema<pair_value<F, S>>();
        }
        
      virtual bool create(variant& result, param_list* args = null)
        {
          //td: unsure of what to do with the arguments
          return object_schema<T>::create(result, args);
        }
    };

  struct dynamic_pair_array_schema : pair_array_schema<dynamic_pair_array, variant, variant>  
    {
    };

  //meta types    
  struct meta_pair_array_schema : basic_schema
    {
      virtual size_t options() { return TYPE_META; }
      virtual bool   create(variant& result, param_list* args);
      virtual void*  get_pointer(void**);
      virtual bool   clone(const variant v, variant& result);
    };
  
  //registry
  register_complete_type(dynamic_pair_array,              dynamic_pair_array_schema);
  //register_type         (pair_iterator<variant, variant>, pair_iterator_schema<variant, variant> );
}

#endif
