
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

#include "boost/filesystem.hpp"

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

int main(int argc, char* argv[])
  {
    if (argc < 2)
      {
        std::cout << "Usage: xss [Project File]" << std::endl;
        //std::cin.get();
        return -1;
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

    bool wait = false;
    bool json_out = false;
    for(int i = 2; i < argc; i++)
      {
        str param(argv[i]);
        if (param == "wait")
          wait = true;
        else if (param == "json")
          json_out = true;
      }

    //setup output
    ConsoleOutput console;
    JsonOutput    json; 

    ICompilerOutput* out = &console;
    if (json_out)
      out = &json;
		
    XSSCompiler compiler(new xss_compiler(out));

    fs::path exepath = fs::complete(argv[0]).parent_path();
    compiler->exe_path(exepath);

    //handle errors 
    int        error_number = 0;
    param_list error;
    
    try
      {
        param_list args;
        compiler->build(target, args);
      }
    catch(xs_error xse)
      {
        error_number = 1;
        fs::path file;

        if (xse.data.has("file"))
          {
            str ff = variant_cast<str>(xse.data.get("file"), str());
            file = fs::path(ff);
          }
        else
          {
            fs::path compiling = compiler->compiling();
            if (!compiling.empty())
              file = compiling;
            else
              {
                XSSRenderer rend = compiler->current_renderer();
                if (rend)
                  {
                    file = rend->file();
                  }
              }
          }

        error = xse.data;
        if (!file.empty())
          error.add("file", file.string());
      }
    catch(xss_error xsse)
      {
        error_number = 2;

        fs::path file;

        if (xsse.data.has("file"))
          {
            str ff = variant_cast<str>(xsse.data.get("file"), str());
            file = fs::path(ff);
          }
        else
          {
            fs::path compiling = compiler->compiling();
            if (!compiling.empty())
              file = compiling;
            else
              {
                XSSRenderer rend = compiler->current_renderer();
                if (rend)
                  {
                    file = rend->file();
                  }
              }
          }

        error = xsse.data;
        if (!file.empty())
          error.add("file", file.string());
      }
    catch(runtime_error rte)
      {
        error_number = 3;

        error = rte.data;
      }

    if (error_number == 0)
        out->success();
    else
        out->error(error);

    str out_str = out->string();
    if (!out_str.empty())
      std::cout << out_str; //"Success are greateful";

    if (wait)
      std::cin.get();

	  return error_number;
  }
