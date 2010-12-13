
#include <schema.h>
#include <xs/operators.h>

using namespace xkp;

//here comes that wonderful time of the year again, lets combine types!
//the idea here is to be able to produce operators that operate on different
//yet related types, like an int and a float.

//here is a crack at type lists, up to 5 types, the templates below produce
//a concrete type out of an index.
template <typename T, int C> struct concrete_type {typedef void type;};
template <typename T> struct concrete_type<T, 5> { typedef typename T::type5 type; };
template <typename T> struct concrete_type<T, 4> { typedef typename T::type4 type; };
template <typename T> struct concrete_type<T, 3> { typedef typename T::type3 type; };
template <typename T> struct concrete_type<T, 2> { typedef typename T::type2 type; };
template <typename T> struct concrete_type<T, 1> { typedef typename T::type1 type; };

template<typename T1, typename T2, template <typename, typename> class E, int C1, int C2>
struct type_register
  {
    typedef typename concrete_type<T1, C1>::type concrete1;
    typedef typename concrete_type<T2, C2>::type concrete2;

    operator_registry& registry_;
    operator_type      op_;
    type_register(operator_registry& registry, operator_type op) : registry_(registry), op_(op) {}

    void do_register()
      {
        //registers the current types
        registry_.register_operator(op_, type_schema<concrete1>(),  type_schema<concrete2>(), new E<concrete1, concrete2>());

        //and combine the ones below in the list
        type_register<T1, T2, E, C1 - 1, C2    > c1(registry_, op_); c1.do_register();
        type_register<T1, T2, E, C1 - 1, C2 - 1> c2(registry_, op_); c2.do_register();
        type_register<T1, T2, E, C1,     C2 - 1> c3(registry_, op_); c3.do_register();
      }
  };

//these 3 stops the recursion at 0
template<typename T1, typename T2, template <typename, typename> class E, int C2>
struct type_register<T1, T2, E, 0, C2>
  {
    type_register(operator_registry& registry, operator_type op) {}
    void do_register() { }
  };

template<typename T1, typename T2, template <typename, typename> class E, int C1>
struct type_register<T1, T2, E, C1, 0>
  {
    type_register(operator_registry& registry, operator_type op) {}
    void do_register() { }
  };

template<typename T1, typename T2, template <typename, typename> class E>
struct type_register<T1, T2, E, 0, 0>
  {
    type_register(operator_registry& registry, operator_type op) {}
    void do_register() { }
  };

//now type collections
struct numerical_types
  {
    static const int size = 3;

    typedef int   type1;
    typedef float type2;
    typedef long  type3;
  };

struct integral_types
  {
    static const int size = 4;

    typedef int           type1;
    typedef unsigned int  type2;
    typedef long          type3;
    typedef short         type4;
  };

struct null_types
  {
    static const int size = 1;

    typedef empty_type type1;
  };

struct string_types
  {
    static const int size = 1;

    typedef str type1;
  };

//and predefined operators
template<typename T1, typename T2>
struct add_ : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        T1 t1 = arg1;
        T2 t2 = arg2;

        return t1 + t2;
      }
  };

template<typename T1, typename T2>
struct subs_ : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        T1 t1 = arg1;
        T2 t2 = arg2;

        return t1 - t2;
      }
  };

template<typename T1, typename T2>
struct minus_ : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        T1 t1 = arg1;

        return -t1;
      }
  };

template<typename T1, typename T2>
struct inc_ : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        T1 t1 = arg1;

        return ++t1;
      }
  };

template<typename T1, typename T2>
struct dec_ : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        T1 t1 = arg1;

        return --t1;
      }
  };

template<typename T1, typename T2>
struct mult_ : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        T1 t1 = arg1;
        T2 t2 = arg2;

        return t1 * t2;
      }
  };

template<typename T1, typename T2>
struct div_ : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        T1 t1 = arg1;
        T2 t2 = arg2;

        return t1 / t2;
      }
  };

template<typename T1, typename T2>
struct equal_ : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        T1 t1 = arg1;
        T2 t2 = arg2;

        return t1 == t2;
      }
  };

template<typename T1, typename T2>
struct notequal_ : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        T1 t1 = arg1;
        T2 t2 = arg2;

        return t1 != t2;
      }
  };

template<typename T1, typename T2>
struct gt_ : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        T1 t1 = arg1;
        T2 t2 = arg2;

        return t1 > t2;
      }
  };

template<typename T1, typename T2>
struct lt_ : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        T1 t1 = arg1;
        T2 t2 = arg2;

        return t1 < t2;
      }
  };

template<typename T1, typename T2>
struct ge_ : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        T1 t1 = arg1;
        T2 t2 = arg2;

        return t1 >= t2;
      }
  };

template<typename T1, typename T2>
struct le_ : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        T1 t1 = arg1;
        T2 t2 = arg2;

        return t1 <= t2;
      }
  };

template<typename T1, typename T2>
struct add_str_number : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        T1 t1 = arg1;
        T2 t2 = arg2;

        return t1 + boost::lexical_cast<str>(t2);
      }
  };

template<typename T1, typename T2>
struct add_number_str : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        T1 t1 = arg1;
        T2 t2 = arg2;

        return boost::lexical_cast<str>(t1) + t2;
      }
  };

struct opexec_false : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        return false;
      }
  };

struct opexec_true : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        return true;
      }
  };

struct default_and : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        bool a1 = variant_cast<bool>(arg1, false);
        if (!a1)
          return false;
        return variant_cast<bool>(arg2, false);
      }
  };

struct opexec_arg1 : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        return arg1;
      }
  };

struct opexec_arg2 : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        return arg2;
      }
  };

struct default_or : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
        bool a1 = variant_cast<bool>(arg1, false);
        if (a1)
          return true;

        return variant_cast<bool>(arg2, false);
      }
  };

struct opexec_null1 : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
				return arg1.empty();
      }
  };

struct opexec_null2 : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
				return arg2.empty();
      }
  };

struct opexec_notnull1 : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
				return !arg1.empty();
      }
  };

struct opexec_notnull2 : operator_exec
  {
    virtual variant exec(variant& arg1, variant& arg2)
      {
				return !arg2.empty();
      }
  };

//some utils
template<typename T1, typename T2, template <typename, typename> class E >
void combine_types(operator_registry& registry, operator_type op)
  {
    type_register<T1, T2, E, T1::size, T2::size> reg(registry, op);
    reg.do_register();
  }

template<typename T>
struct single_type
  {
    static const int size = 1;
    typedef T type1;
  };

template<typename T1, typename T2, template <typename, typename> class E >
void combine_type(operator_registry& registry, operator_type op)
  {
    typedef single_type<T1> st;
    type_register<st, T2, E, 1, T2::size> reg(registry, op);
    reg.do_register();
  }

template< template <typename, typename> class E >
void numeric_operator(operator_registry& registry, operator_type op)
  {
    combine_types<numerical_types, numerical_types, E>(registry, op);
  }

template< template <typename, typename> class E >
void numeric_unary_operator(operator_registry& registry, operator_type op)
  {
    combine_types<numerical_types, null_types, E>(registry, op);
  }

//operator_registry
operator_registry::operator_registry()
  {
    //define type ranks, so we can find the resulting type
    //between different types
    rank_.push_back( type_schema<bool>()   );
    rank_.push_back( type_schema<int>()    );
    rank_.push_back( type_schema<float>()  );
    rank_.push_back( type_schema<double>() );
    rank_.push_back( type_schema<str>()    );

    numeric_operator<add_>     (*this, op_plus);
    numeric_operator<subs_>    (*this, op_minus);
    numeric_operator<mult_>    (*this, op_mult);
    numeric_operator<div_>     (*this, op_divide);
    numeric_operator<equal_>   (*this, op_equal);
    numeric_operator<notequal_>(*this, op_notequal);
    numeric_operator<gt_>      (*this, op_gt);
    numeric_operator<lt_>      (*this, op_lt);
    numeric_operator<ge_>      (*this, op_ge);
    numeric_operator<le_>      (*this, op_le);

    //unary integral
    combine_types<integral_types, null_types, inc_>(*this, op_inc);
    combine_types<integral_types, null_types, dec_>(*this, op_dec);

    numeric_unary_operator<minus_>(*this, op_unary_minus);

    //string operators
    register_operator(op_plus, type_schema<str>(), type_schema<str>(), new add_<str, str>() );
    register_operator(op_equal, type_schema<str>(), type_schema<str>(), new equal_<str, str>() );
    combine_types<string_types, numerical_types, add_str_number>(*this, op_plus);
    combine_types<numerical_types, string_types, add_number_str>(*this, op_plus);

    //null operator
    register_operator(op_not,			 type_schema<empty_type>(), null,                      new opexec_true() ); //not null
    register_operator(op_not,			 type_schema<empty_type>(), type_schema<empty_type>(), new opexec_true() );
    register_operator(op_equal,		 type_schema<empty_type>(), type_schema<empty_type>(), new opexec_true() );
    register_operator(op_notequal, type_schema<empty_type>(), type_schema<empty_type>(), new opexec_false() );

    //boolean operators, this might be nuts, but i will register ands and ors that if the types
    //are unknown react to it, directly casting to boolean, sometimes
    register_wildcard(op_and, null,                      type_schema<empty_type>(), new opexec_false() );
    register_wildcard(op_and, type_schema<empty_type>(), null                     , new opexec_false() );
    register_wildcard(op_and, null,                      null                     , new default_and() );

    register_wildcard(op_or,	null,                      type_schema<empty_type>(), new opexec_arg1() );
    register_wildcard(op_and, type_schema<empty_type>(), null                     , new opexec_arg2() );
    register_wildcard(op_and, null,                      null                     , new default_or() );

    register_wildcard(op_equal,			null,											 type_schema<empty_type>(), new opexec_null1() );
    register_wildcard(op_equal,			type_schema<empty_type>(), null											, new opexec_null2() );
    register_wildcard(op_notequal,	null,											 type_schema<empty_type>(), new opexec_notnull1() );
    register_wildcard(op_notequal,	type_schema<empty_type>(), null											, new opexec_notnull2() );
}

size_t operator_registry::register_operator(operator_type op, schema* t1, schema* t2, operator_exec* exec)
  {
    size_t result = executors_.size();
    executors_.push_back( exec );

    key k(op, t1, t2);
    dynamic_executors_.insert(dynamic_executor_pair( k, result ));
    return result;
  }

operator_exec* operator_registry::get_operator(operator_type op, schema* t1, schema* t2)
  {
    key k(op, t1, t2);
    dynamic_executor_list::iterator it = dynamic_executors_.find( k );
    if (it != dynamic_executors_.end())
      return executors_[it->second];

    if (t2) //avoid unaries
      {
        //and test for wildcards
        key wk(op, null, t2);
        it = wildcard_executors_.find( wk );
        if (it != wildcard_executors_.end())
          return executors_[it->second];

        key wk2(op, t1, null);
        it = wildcard_executors_.find( wk2 );
        if (it != wildcard_executors_.end())
          return executors_[it->second];

        key wk3(op, null, null);
        it = wildcard_executors_.find( wk3 );
        if (it != wildcard_executors_.end())
          return executors_[it->second];
      }

    return null;
  }

bool operator_registry::get_operator_index(operator_type op, schema* t1, schema* t2, size_t& result, schema** result_type)
  {
    key k(op, t1, t2);
    dynamic_executor_list::iterator it = dynamic_executors_.find( k );
    if (it != dynamic_executors_.end())
      {
        if (result_type)
          {
            if (t1 == t2)
              *result_type = t1;
            else
              *result_type = get_result_type(t1, t2);
          }

        result = it->second;
        return true;
      }

    return false;
  }

operator_exec* operator_registry::get_operator(size_t idx)
  {
    return executors_[idx];
  }

schema* operator_registry::get_result_type(schema* t1, schema* t2)
  {
    if (t1 == t2)
      return t1;

    int r1 = -1;
    int r2 = -1;

    type_rank::iterator it   = rank_.begin();
    type_rank::iterator nd   = rank_.end();
    int                 curr = 0;
    for(; it != nd; it++, curr)
      {
        if (t1 == *it)
          r1 = curr;

        if (t2 == *it)
          r2 = curr;
      }

    if (r1 < 0 || r2 < 0)
      return null;
    else if (r1 > r2)
      return t1;

    return t2;
  }

size_t operator_registry::register_wildcard(operator_type op, schema* t1, schema* t2, operator_exec* exec )
  {
    size_t result = executors_.size();
    executors_.push_back( exec );

    key k(op, t1, t2);
    wildcard_executors_.insert(dynamic_executor_pair( k, result ));
    return result;
  }
