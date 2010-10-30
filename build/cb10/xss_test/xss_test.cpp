#define BOOST_TEST_MODULE xss
#include <boost/test/unit_test.hpp>

#include "stdafx.h"
#include <iostream>
#include <fstream>

#include <schema.h>
#include <xs.h>
#include "xs/ast.h"
#include "xs/compiler.h"
#include "xs/debug_utils.h"
#include "xs/array.h"
#include "text/text_renderer.h"
#include "gen/generator.h"

using namespace xkp;

BOOST_AUTO_TEST_CASE( howdy )
{
  xs_compiler  compiler;
  xs_container results;
  
  str src = "class foo : bar { property xx : string; method bar(int x, y) { for(int i = 0 ; i < y.count; i++) { y.cmp.visible = true; }}}";
  compiler.compile_xs(src, results);

  generator g;
  g.consume(results);
  
  str output = g.generate();
  std::cout << output << "\n";
  
  //update the linker  
  //code_linker linker;
  //result.visit(&linker);
}

BOOST_AUTO_TEST_CASE( finished )
{
  //for some reason the test frameworks exits without letting me see the goods
  std::cin.get();
}