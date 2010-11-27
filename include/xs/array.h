
#ifndef XS_ARRAY_H
#define XS_ARRAY_H

#include <schema.h>

namespace xkp
{
  template<typename T>
  struct typed_iterator
    {
      typedef std::vector<T>       container;
      typedef reference<container> container_ref;

      typed_iterator()
        {
          assert(false); //should never be created
        }

      ~typed_iterator()
        {
        }

      typed_iterator(const typed_iterator& other)
        {
          ref_ = other.ref_;
          idx_ = other.idx_;
        }

      typed_iterator(container_ref ref, int idx):
        ref_(ref),
        idx_(idx)
        {
        }

      typed_iterator advance()
        {
          if (idx_ >= 0)
            {
              idx_++;
              if (ref_ && ref_->size() <= idx_)
                idx_ = -1;
            }

          return typed_iterator(ref_, idx_);
        }

      bool compare(typed_iterator other)
        {
          if (other.ref_ != ref_)
            return false;

          if (other.idx_ < 0)
            return idx_ < 0 || (ref_ && ref_->size() <= idx_);

          if (idx_ < 0)
            return other.ref_ && other.ref_->size() <= other.idx_;

          return other.idx_ == idx_;
        }

      T value()
        {
          if (idx_ < 0 || !ref_ || ref_->size() <= idx_)
            assert(false); //td: error, invalid iterator

          return ref_->at(idx_);
        }

      public:
        int           idx_;
        container_ref ref_;
    };

  template<typename T>
  struct typed_array
    {
      typedef T                    container_type;
      typedef std::vector<T>       container;
      typedef reference<container> container_ref;
      typedef typed_iterator<T>    iterator;

      typed_array()
        {
          ref_ = container_ref( new container );
        }

      typed_array(const typed_array& other):
        ref_(other.ref_)
        {
        }

      ~typed_array()
        {
        }

      iterator begin()
        {
          if (ref_->empty())
            return iterator(ref_, -1);
          return iterator(ref_, 0);
        }

      iterator end()
        {
          return iterator(ref_, -1);
        }

      void insert(T value)
        {
          ref_->push_back( value );
        }

      schema* iterated_type()
        {
          return type_schema<T>();
        }
      protected:
        container_ref ref_;
    };

  struct dynamic_array : typed_array<variant>
    {
      dynamic_array()             : type_(null) {}
      dynamic_array(schema* type) : type_(type) {}

      ~dynamic_array()
        {
        }

      schema* iterated_type()
        {
          return type_;
        }

      void push_back(variant v)
        {
          ref_->push_back(v);
        }

      size_t size()
        {
          return ref_->size();
        }

      variant at(size_t idx)
        {
          return ref_->at(idx);
        }
      private:
        schema* type_;
    };

  typedef reference<dynamic_array> DynamicArray;

  //array types
  template<typename T>
  struct typed_iterator_schema : object_schema< typed_iterator<T> >
    {
      typedef typed_iterator<T> this_type;

      virtual void declare()
        {
          this->template method_<this_type, 0>("++",    &this_type::advance);
          this->template method_<bool,      1>("==",    &this_type::compare);
          this->template readonly_property<T> ("value", &this_type::value  );
        }
    };

  template<typename T>
  struct typed_array_schema : object_schema<T>
    {
      virtual void declare()
        {
          this->template class_property<typed_array_schema, schema*> ("iterated_type", &typed_array_schema::iterated_type);

          this->template readonly_property<typename T::iterator>("begin",  &T::begin);
          this->template readonly_property<typename T::iterator>("end",    &T::end);
          this->template method_<void, 1>              ("insert", &T::insert);
          this->template method_<void, 1>              ("+",      &T::insert, OP_BLOCK_ASSIGN);
          this->template method_<void, 1>              ("+=",     &T::insert, OP_BLOCK_ASSIGN);
        }

      virtual size_t options()
        {
          return TYPE_ITERATED;
        }

      virtual schema* iterated_type()
        {
          return type_schema<T>();
        }

      virtual bool create(variant& result, param_list* args = null)
        {
          reference<T> res = reference<T>( new T() );
          if (args)
            {
              for(size_t i = 0; i < args->size(); i++)
                {
                  typename T::container_type item = args->get(i);
                  res->insert(item);
                }
            }

          result = res;
          return true;
        }
    };

  struct dynamic_array_schema : typed_array_schema< dynamic_array >
    {
      dynamic_array_schema()             : type_(null) {}
      dynamic_array_schema(schema* type) : type_(type) {}

      virtual schema* iterated_type()
        {
          return type_;
        }

      private:
        schema* type_;
    };

  //meta types
  struct meta_array_schema : basic_schema
    {
      virtual size_t options() { return TYPE_META; }
      virtual bool   create(variant& result, param_list* args);
      virtual void*  get_pointer(void**);
      virtual bool   clone(const variant v, variant& result);
      virtual void   declare(); 
    };

  //registry
  register_complete_type(dynamic_array,           dynamic_array_schema);
  register_type         (typed_iterator<variant>, typed_iterator_schema<variant> );
}

#endif
