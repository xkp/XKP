#include "stdafx.h"
#include "base.h"
#include "schema.h"

#include <iostream>

using namespace xkp;

struct testing 
  {
    int foo()
      { 
        std::cout << "foo";
        return 1;
      }
      
    int foo1( int x)
      { 
        std::cout << "foo1 " << x;
        return x;
      }

    void bar() const
      { 
        std::cout << "bar " << xx;
      }
      
    void sety(int y)
      {
        yy = y;
      }

    int xx;
    int yy;
  };
  
struct test_schema : object_schema<testing>  
  {
    test_schema()
      {
        property_("x", &testing::xx);
        property_<int>("y", &testing::yy, &testing::sety);

        method_<int, 0> ( "foo", &testing::foo );
        method_<int, 1> ( "foo1", &testing::foo1 );
        method_<void, 0>( "bar", &testing::bar );
      }
  };
  
register_type(testing, test_schema);

int _tmain(int argc, _TCHAR* argv[])
{
  int i = 190;
  variant v = i;

  int ii = v;

  testing t;
  t.xx = 2;
  t.yy = 27;
  v = t;

  int xx = var_get<int>(v, "x" );
  var_set( v, "x", 5 );
  var_set( v, "y", 578 );
  
  str ss = var_get<str>(v, "x", str("not found or not string") );

  testing tt = v;

  schema_item item;
  param_list pl;
  if (v.get_schema()->resolve("foo", item))
    {
      int result = item.exec->exec( v.v, pl );
      std::cout << "foo returned " << result;
    }
  
  int rr = var_exec(v, "foo");

  if (v.get_schema()->resolve("foo1", item))
    {
      pl.add( 5 );
      item.exec->exec( v.v, pl );
    }

	return 0;
}

