
#define BOOST_TEST_MODULE xs
#include <boost/test/unit_test.hpp>

#include "stdafx.h"
#include <iostream>
#include <fstream>

#include <schema.h>
#include <xs.h>
#include "xs/vm.h"
#include "xs/ast.h"
#include "xs/compiler.h"
#include "xs/linker.h"
#include "xs/debug_utils.h"
#include "xs/array.h"
#include "text/text_renderer.h"

using namespace xkp;

//utils

//this c++ class will be both used and inherited
struct console : dynamic_object<console>
  {
    void print(variant what)
      {
        str w = what;
        std::cout << w;
      }
      
    void println(variant what)
      {
        str w = what;
        std::cout << w << std::endl;
      }
  };
  
//and this is its type information
struct console_schema : dynamic_object_schema<console>  
  {
    console_schema()
      {
        implements<IDynamicObject>();
        //inherit_from<IDynamicObject>();
      
        method_<void, 1>( "print",   &console::print );
        method_<void, 1>( "println", &console::println );
      }
  };

//simple loading, this should be handled somewhere consistently
bool file2str(const str& file, str& contents)
  {
    std::ifstream ifs(file.c_str());
    if (!ifs) 
      return false; //td: error
    
   // Read the whole file into a string
   std::stringstream ss;
   ss << ifs.rdbuf();  
   
   contents = ss.str();
   return true;
  }
  
DynamicObject run_test(const str& filename)
  {
    xs_utils      util;
    str           source;
    code_context  ctx;

    basic_scope        scope;
    reference<console> console_singleton( new console );
    scope.register_symbol( "console", console_singleton );

    type_registry types;
    
    meta_array_schema array_type;
    types.add_type("array", &array_type);
    
    ctx.types_ = &types;
    ctx.scope_ = &scope; 
    
    BOOST_REQUIRE(file2str(filename, source));
    util.compile(source, ctx);
    
    variant       vi;
    DynamicObject instance;
    BOOST_REQUIRE(scope.resolve("test_suite", vi));
    BOOST_REQUIRE_NO_THROW(instance = vi);

    ctx.this_     = instance;
    ctx.this_type = instance->get_type();
    util.execute_xs("test();", ctx);
    
    return instance;
  }
  
//*************************** EXPRESSION TEST ****************************************
//Compiles and evaluate a simple expression

BOOST_AUTO_TEST_CASE( test_expressions )
{
  str parse_string = "2 + 5*(7 - 2)/(10 * -0.5)";
  xs_compiler cmp;
  expression  ctx; 
  
  BOOST_REQUIRE( cmp.compile_expression(parse_string, ctx) );

  expr_evaluator eval;
  ctx.visit( &eval );   
  variant result = eval.value();
  float result_value;
  BOOST_REQUIRE_NO_THROW( result_value = result; );
  BOOST_CHECK( result_value == -3.0 );  
}

//*************************** COMPLEX EXPRESSION TEST ****************************************
//Tests expressions containing calls, indexing and operators
//prints the result 
//td: verify results somehow)
BOOST_AUTO_TEST_CASE( complex_expressions )
{
  str parse_string = "(x.y.z[5]++) + qwer(x, y, z - 3)";
  xs_compiler cmp;

  expression  result;
  BOOST_REQUIRE( cmp.compile_expression(parse_string, result) );
    
  print_expression dv;
  result.visit( &dv );
}

//*************************** COMPILING AND LIKING CODE ****************************************
//compiles, link and execute code

BOOST_AUTO_TEST_CASE( test_link )
{
  xs_compiler cmp;

  str parse_string = "int ww = 35 + 20*5; for(int i = 0; i < 6; i++) { ww -= 5;} ww += 45/ww + 3; return ww;" ;
  code result;
  BOOST_REQUIRE( cmp.compile_code(parse_string, result) );
  
  //update the linker  
  code_linker linker;
  result.visit(&linker);
  
  //and produce executable code
  ByteCode bc = linker.link();

  BOOST_REQUIRE( bc );

  execution_context ec(bc);
  
  int exec_result;
  BOOST_REQUIRE_NO_THROW( exec_result = ec.execute(); );
  BOOST_REQUIRE( exec_result == 108 );
}

//*************************** USING SCOPES AND this POINTERS ****************************************
//uses a c++ type (scriptable) in the linking process
//adds constants in the scope and uses a *this* pointer

struct scriptable
  {
    scriptable()        : ww(0) {}
    scriptable(int _ww) : ww(_ww) {}
  
    void print(int value)
      {
        std::cout << "Printing: " << value << " ww: " << ww << std::endl;
      }
      
    void print_string(const str& what)
      {
        std::cout << what << std::endl;
      }

    int ww;
  };  
  
struct scriptable_schema : object_schema<scriptable>  
  {
    scriptable_schema()
      {
        property_("ww", &scriptable::ww);
        method_<void, 1>( "print", &scriptable::print );
        method_<void, 1>( "print_string", &scriptable::print_string );
      }
  };
  
register_complete_type(scriptable, scriptable_schema);

BOOST_AUTO_TEST_CASE( test_scope_and_this )
{
  xs_utils util;
  
  //an object to be inserted in the scope as a constant
  scriptable scope_object(34);
  
  //create a simple scope and add some constants
  basic_scope scope;
  scope.register_symbol("scope_constant", 5);
  scope.register_symbol("scope_object", scope_object);

  //use a *this* instance
  scriptable this_object(43);

  //prepare the linking context
  code_context ctx;
  ctx.scope_    = &scope;
  ctx.this_type = type_schema<scriptable>();
  ctx.this_     = &this_object;
  
  str parse_string = 
  "\
      int ll = scope_object.ww + scope_constant; \
      if (ll > 5)\
        print(145);\
      ww = ll;\
      scope_object.print(154);\
      return ll;";

  variant exec_result = util.execute_xs(parse_string, ctx); 

  BOOST_REQUIRE( exec_result.is<int>() );
  int r = exec_result;
  
  BOOST_REQUIRE( r == 39 );
  BOOST_REQUIRE( this_object.ww == 39 );
}

//*************************** TESTING ITERATORS ****************************************

register_iterator(int)

BOOST_AUTO_TEST_CASE( testing_iterators )
{
  xs_utils util;

  std::vector<int> array_;
  array_.push_back( 10 );
  array_.push_back( 20 );
  array_.push_back( 55 );
  
  basic_scope scope;
  scope.register_symbol("int_array", &array_);

  code_context ctx;
  ctx.scope_    = &scope;
  
      //array native_array = [34, 56, 2];\
      //for(int j in native_array)\
      //  total += i;\

  str parse_string = 
  "\
      int total = 0;\
      for(int i in int_array)\
        total += i;\
      return total;";

  variant exec_result = util.execute_xs(parse_string, ctx); 

  BOOST_REQUIRE( exec_result.is<int>() );
  int r = exec_result;
  
  BOOST_REQUIRE( r == 85 );
}

//*************************** DSL TEST ****************************************

//DSL (domain specific language) parsers can be embedded
//into xs, this tests a very simple one. note that dsl implementors 
//will be supplied the full string between braces

struct test_dsl_linker : dsl_linker
  {
    virtual void link(dsl& info, code_linker& owner)
      {
        std::cout << "Linking test dsl: " << info.id << std::endl;
        expression expr;
        
        //generate some code, mind you, you could insert instructions outright
        //without this expression thingy
        expr.push_identifier("this");
        expr.push_identifier("print_string");
        expr.push_operator(op_dot);
        expr.push_operand(info.text);
        expr.push_operand(1);
        expr.push_operator(op_call);
        
        owner.link_expression(expr);
      }
  };

BOOST_AUTO_TEST_CASE( test_dsl )
{
  xs_utils util;

  //td: re-enable dsls
  return;

  scriptable this_;

  //prepare the linking context
  code_context ctx;
  ctx.this_type = type_schema<scriptable>();
  ctx.this_     = &this_; 
  
  //register our custom linker
  //ctx.register_dsl("test_dsl", DslLinker(new test_dsl_linker));
  
  str parse_string = 
  "\
      int ww = 45 + 10; \
      test_dsl xx(2, e, tyy)\
      {\
          this is just random text\
          including { and }\
      }\
      return ww;";

  variant exec_result = util.execute_xs(parse_string, ctx); 
  
  //just for kicks
  BOOST_REQUIRE(exec_result.is<int>() && ((int)exec_result) == 55);
}

//*************************** CLASS TEST ****************************************
//this test will parse a class file (test.xs)
//and then dynamically instantiate and use some of them

register_complete_type(console, console_schema);
  
BOOST_AUTO_TEST_CASE( testing_classes )
{
  xs_container xs_file;
  xs_compiler cmp;
  BOOST_REQUIRE( cmp.compile_xs_file("D:\\xkp\\GI\\bin\\debug\\test.xs", xs_file) );
    
  type_registry types;
  types.add_type( "console", type_schema<console>() );
  
  basic_scope class_scope;
  reference<console> the_c( new console );
  class_scope.register_symbol( "the_console", the_c );
    
  for(int i = 0; i < 3; i++)
    {
      xs_class ci = xs_file.get(i);
      
      code_context ctx;
      ctx.scope_ = &class_scope;
      ctx.types_ = &types; 
      class_linker cl(ctx); 
      cl.link(ci);
    }
  
  str parse_string = 
  "\
      console xx = console(); \
      xx.print(567); \
      test_class yy = test_class(); \
      xx.print(yy.blue); \
      yy.red = 45;\
      inheriting_from_cpp c = inheriting_from_cpp();\
      c.print_info( \"what\", 56 );\
      testing_inherited ti = testing_inherited();\
      ti.red = 46;\
      ti.print_all();\
      ";

  code_context instantiate_ctx;
  instantiate_ctx.types_ = &types;

  xs_utils util;
  util.execute_xs(parse_string, instantiate_ctx);                  
}

//*************************** INSTANCE TEST ****************************************
//compile a class
//instanciate it
//modify it (add a method)
//call the new instance.

BOOST_AUTO_TEST_CASE( testing_instances )
{
  xs_utils util;

  type_registry types;
  code_context instance_ctx;
  instance_ctx.types_ = &types;
  
  str source;
  BOOST_REQUIRE(file2str("D:\\xkp\\GI\\bin\\debug\\instance_class.xs", source));
  util.compile_class(source, instance_ctx);
  
  DynamicObject instance = util.execute_xs("return testing_instance_class();", instance_ctx);
  
  BOOST_REQUIRE(file2str("D:\\xkp\\GI\\bin\\debug\\instance.xs", source));
  util.compile_instance(source, instance, instance_ctx);
  
  instance_ctx.this_     = instance;
  instance_ctx.this_type = instance->get_type();
  variant result = util.execute_xs("return added_method( 5 );", instance_ctx);
  BOOST_REQUIRE(result.is<int>());
  int result_value = result;
  BOOST_CHECK(result_value == 20);
}

//*************************** TESTING TEXT RENDERING ****************************************

BOOST_AUTO_TEST_CASE( testing_text_renderer )
{
  str translate_text = "this <% return value + \" and \" + value2; %> are good";
  
  param_list    values;
  values.add("value",  str("apple"));
  values.add("value2", str("orange"));
  
  code_context ctx; 
  ctx.args_ = &values;

  text_renderer renderer;
  BOOST_REQUIRE(renderer.compile(translate_text, ctx));
  str result = renderer.render(ctx);
  BOOST_CHECK(result == "this apple and orange are good");
}

//*************************** TESTING BEHAVIOURS ****************************************

BOOST_AUTO_TEST_CASE( testing_behaviours )
{
  xs_utils      util;
  str           source;
  code_context  ctx;

  basic_scope        scope;
  reference<console> console_singleton( new console );
  scope.register_symbol( "console", console_singleton );

  type_registry types;
  
  ctx.types_ = &types;
  ctx.scope_ = &scope; 
  
  BOOST_REQUIRE(file2str("D:\\xkp\\GI\\bin\\debug\\test_behaviour.xs", source));
  util.compile(source, ctx);
  
  variant       vi;
  DynamicObject instance;
  BOOST_REQUIRE(scope.resolve("test_suite", vi));
  BOOST_REQUIRE_NO_THROW(instance = vi);

  ctx.this_     = instance;
  ctx.this_type = instance->get_type();
  util.execute_xs("test();", ctx);
}

//*************************** TESTING EVENTS ****************************************

BOOST_AUTO_TEST_CASE( testing_events )
{
  DynamicObject result = run_test("D:\\xkp\\GI\\bin\\debug\\test_events.xs");
  BOOST_REQUIRE(result);
}

//*************************** TESTING ARRAYS ****************************************

BOOST_AUTO_TEST_CASE( testing_arrays )
{
  DynamicObject result = run_test("D:\\xkp\\GI\\bin\\debug\\test_array.xs");
  BOOST_REQUIRE(result);
}

BOOST_AUTO_TEST_CASE( finished )
{
  //for some reason the test frameworks exits without letting me see the goods
  std::cin.get();
}