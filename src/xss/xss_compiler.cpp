
#include "xss/xss_compiler.h"
#include "xss/object_reader.h"
#include "xss/xss_error.h"

using namespace xkp;

//error codes
const str SProjectError("project");

const str SCannotReadModule("Unable to read module");
const str SMustProvideEntryPointForApplicationType("Applications must provide an entry point");
const str SOnlyClassAndEnum("Only classes and enums are allowed as types");
const str SUnnamedType("Types must have an id");
const str SCannotReadProjectFile("Types must have an id");

//xss_application_renderer
xss_application_renderer::xss_application_renderer(const str& xss_file):
  filename_(xss_file),
  context_(new XSSContext())
  {
  }

XSSContext xss_application_renderer::getContext()
  {
  }

void xss_application_renderer::register_module(const str& id, XSSModule module)
  {
  }

  
//xss_module
xss_module(XSSContext ctx);

//xss_compiler
void xss_compiler::build(const str& xml)
  {
    read_project(xml);
  }

void xss_compiler::read_project(const str& xml)
  {
    xss_object_reader reader;
    XSSObject project_data = reader.read(xml);
    if (!project_data)
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SCannotReadProjectFile);
        xss_throw(error);
      }
    
    //we need to find out how many different types of applications we'll be compiling
    //it would be able to load a single application type without the extra syntax
    std::vector<XSSObject> applications = project_data->find_by_class("application");
    if (applications.empty())
        applications.push_back(project_data);

    read_application_types(applications);
  }

void xss_compiler::read_application_types(std::vector<XSSObject> & applications)
  {
    std::vector<XSSObject>::iterator it = applications.begin();
    std::vector<XSSObject>::iterator nd = applications.begin();

    for(; it != nd; it++)
      {
        XSSObject app_data = *it;

        //get the appropiate data
        str entry_point = app_data->get<str>("entry_point", str());
        if (entry_point.empty())
          {
            param_list error;
            error.add("id", SProjectError);
            error.add("desc", SMustProvideEntryPointForApplicationType);
            xss_throw(error);
          }
        
        XSSApplicationRenderer app(new xss_application_renderer(entry_point));

        //load modules
        XSSObject module_data = app_data->get<XSSObject>("modules", XSSObject());
        if (module_data)
          {
            DynamicArray modules = module_data->children();

            for(int i = 0; i < modules->size(); i++)
              {
                XSSObject module = modules->at(i);
                str src = module->get<str>("src", str());
                str id  = module->get<str>("id",  str());

                XSSModule compiled = read_module(src, app, module);
                
                app->register_module(id, compiled);
              }
          }

        applications_.push_back(app);
      }
  }

XSSModule xss_compiler::read_module(const str& src, XSSApplicationRenderer app, XSSObject module)
  {
    xss_object_reader reader;
    XSSObject module_data = reader.read(src); //td: path

    if (!module_data)
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SCannotReadModule);
        xss_throw(error);
      }

    XSSModule result(new xss_module(app->getContext()));
    read_types(module, app, result);

    return result;
  }

void xss_compiler::read_types(XSSObject module_data, XSSApplicationRenderer app, XSSModule module)
  {
    XSSObject types = module->find("types");
    if (types)
      {
        for(int i = 0; i < types->children()->size(); i++)
          {
            XSSObject type_data = types->children()->at(i);
            str       type_name = type_data->getId();
            if (type_name.empty())
              {
                param_list error;
                error.add("id", SProjectError);
                error.add("desc", SUnnamedType);
                xss_throw(error);
              }

            str class_name = type_data->getClassName();
            if (class_name == "class")
              {
              }
            else if (class_name == "enum")
              {
              }
            else
              {
                param_list error;
                error.add("id", SProjectError);
                error.add("desc", SOnlyClassAndEnum);
                xss_throw(error);
              }
              
            //XSSType type(new xss_type(type_data));
            //app->register_type(type_name, type);
          }
      }    
  }
