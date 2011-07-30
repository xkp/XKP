
#include "stdafx.h"
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
//#include "xss/jscript.h"
//#include "xss/c++.h"
//#include "archive/xml_archive.h"
//#include "xss/xss_generator.h"

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

		//td: 
    //std::cout << "\nFile:" << project->top_file() << '\n';
		//std::cout << "\nLast Rendererd: \n\n" << project->last_rendered(5);
  }
  
int main(int argc, char* argv[])
  {
    char* fname = argv[1];
		fs::path target(fname); 
    if (!fs::exists(target))
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SCannotReadProjectFile);
        print_error(error);

        return -1;
      }
      
		XSSCompiler compiler(new xss_compiler);

    bool succeeded = true;
    try
      {
        compiler->build(target);
      }
    catch(xs_error xse)
      {
        succeeded = false;

        print_error(xse.data);
      }
    catch(xss_error xsse)
      {
        succeeded = false;

        XSSRenderer renderer = compiler->current_renderer();
        if (renderer)
          {
            str file = renderer->file().string();
            xsse.data.add("file", file);
          }
        print_error(xsse.data);
      }
    catch(runtime_error rte)
      {
        succeeded = false;

        print_error(rte.data);
      }
		//catch(type_mismatch tm)
		//	{
  //      succeeded = false;
		//		param_list data;
		//		data.add("desc", str("Type mismatch"));
  //      print_error(data, project);
		//	}
    
    if (succeeded)
      {
        std::cout << "Success are greateful";
      }
      
    std::cin.get();
	  
	return 0;
  }

