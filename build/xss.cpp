
#include <iostream>
#include <fstream>

#include <types.h>
#include <base.h>
#include <schema.h>
#include <serial.h>

#include <xs.h>
#include "xs/ast.h"
#include "xs/compiler.h"
#include "xs/debug_utils.h"
#include "xs/array.h"
#include "xs/xs_error.h"
#include "xs/runtime_error.h"
#include "gen/generator.h"
#include "xss/xss_parser.h"
#include "xss/xss_error.h"
#include "xss/xss_compiler.h"
#include "xss/xss_expression.h"

#include "boost/filesystem.hpp"

#include "xss/object_model.h"
#include "xss/localfs.h"
#include "xss/lang_factory.h"

using namespace xkp;

const str SProjectError("project");
const str SCannotReadProjectFile("Could not read project file");


void print_error(param_list data)
  {
    str id   = variant_cast<str>(data.get("id"), "");
    str desc = variant_cast<str>(data.get("desc"), "");

    std::cout << "Error [" << id << "] " << desc << '\n';

    for(size_t i = 0; i < data.size(); i++)
      {
        str name = data.get_name(i);
        if (name == "id" || name == "desc")
          continue;

        variant value = data.get(i);
        str     value_str = variant_cast<str>(value, str("[cannot-resolve]"));

        std::cout << name << " = " << value_str << '\n';
      }
  }

XSSExpression test_expression(const str& expr)
  {
    expression e;

    xs_utils xs;
    xs.compile_expression(expr, e);
    XSSExpression result = xss_expression_utils::compile_expression(e);
    return result;
  }

void test_expressions()
  {
    test_expression("a.b.c = c.d.e");
    test_expression("a.b[i, j].c = c.d(j + 3, f).e");
    test_expression("b[i, j] = new myobj( 3/(a+ b), null)");
    test_expression("myobj( 3/(a+ b), null, hello = true)");
    test_expression("a[5] = { a : 3, b : [2, 3, 4]}");
  }

void test_object_model(fs::path target)
  {
    FileSystem fs(new local_filesystem);
    LanguageFactory lf(new lang_factory);

    ObjectModel om(new object_model(fs, lf));

    param_list args;
    DataReader prj = fs->load_data(target);
    om->load(prj, args, target.parent_path());
  }

int main(int argc, char* argv[])
  {
    test_expressions(); //td: !!!DEBUG!!!

    if (argc < 2)
      {
        std::cout << "Usage: xss [Project File]";
        std::cin.get();
        return 0;
      }

    char* fname = argv[1];
		fs::path target = fs::system_complete(fname);

    if (!fs::exists(target))
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SCannotReadProjectFile);
        print_error(error);

        return -1;
      }

    test_object_model(target); //td: !!!DEBUG!!!
    return 0;

    //0.9.5
   // bool wait = false;
   // bool json_out = false;
   // for(int i = 2; i < argc; i++)
   //   {
   //     str param(argv[i]);
   //     if (param == "wait")
   //       wait = true;
   //     else if (param == "json")
   //       json_out = true;
   //   }

   // //setup output
   // ConsoleOutput console;
   // JsonOutput    json; 

   // ICompilerOutput* out = &console;
   // if (json_out)
   //   out = &json;
		
   // XSSCompiler compiler(new xss_compiler(out));

   // //handle errors 
   // int        error_number = 0;
   // param_list error;
   // 
   // try
   //   {
   //     param_list args;
   //     compiler->build(target, args);
   //   }
   // catch(xs_error xse)
   //   {
   //     error_number = 1;
   //     fs::path file;

   //     if (xse.data.has("file"))
   //       {
   //         str ff = variant_cast<str>(xse.data.get("file"), str());
   //         file = fs::path(ff);
   //       }
   //     else
   //       {
   //         fs::path compiling = compiler->compiling();
   //         if (!compiling.empty())
   //           file = compiling;
   //         else
   //           {
   //             XSSRenderer rend = compiler->current_renderer();
   //             if (rend)
   //               {
   //                 file = rend->file();
   //               }
   //           }
   //       }

   //     error = xse.data;
   //     if (!file.empty())
   //       error.add("file", file.string());
   //   }
   // catch(xss_error xsse)
   //   {
   //     error_number = 2;

   //     fs::path file;

   //     if (xsse.data.has("file"))
   //       {
   //         str ff = variant_cast<str>(xsse.data.get("file"), str());
   //         file = fs::path(ff);
   //       }
   //     else
   //       {
   //         fs::path compiling = compiler->compiling();
   //         if (!compiling.empty())
   //           file = compiling;
   //         else
   //           {
   //             XSSRenderer rend = compiler->current_renderer();
   //             if (rend)
   //               {
   //                 file = rend->file();
   //               }
   //           }
   //       }

   //     error = xsse.data;
   //     if (!file.empty())
   //       error.add("file", file.string());
   //   }
   // catch(runtime_error rte)
   //   {
   //     error_number = 3;

   //     error = rte.data;
   //   }

   // if (error_number == 0)
   //     out->success();
   // else
   //     out->error(error);

   // str out_str = out->string();
   // if (!out_str.empty())
   //   std::cout << out_str; //"Success are greateful";

   // if (wait)
   //   std::cin.get();

	  //return error_number;
  }
