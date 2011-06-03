#ifndef XSS_COMPILER_HH
#define XSS_COMPILER_HH

#include <base.h>
#include <schema.h>
#include <dynamic_objects.h>
#include <xs.h>

#include "xss_context.h"

namespace xkp
{
  //forwards
	class xss_application_renderer;
  class xss_module;

  //reference types
  typedef reference<xss_application_renderer> XSSApplicationRenderer;
  typedef reference<xss_module>               XSSModule;

  //classes
	class xss_application_renderer : public xss_object
    {
      public:
        xss_application_renderer(const str& xss_file);
      public:
        XSSContext context();
        str        target(); 
        void       register_module(const str& id, XSSModule module);
      private:
        str        filename_;
        str        target_;
        XSSContext context_;
    };
  
	class xss_module : public xss_object
    {
      public:
        xss_module(XSSContext ctx);
      private:
        XSSContext ctx_;
    };

  class xss_compiler
		{
      public:
        void build(const str& xml);
		  private:
        std::vector<XSSApplicationRenderer> applications_;
        fs::path base_path_;
        fs::path source_path_;
        
        XSSObject    read_project(const str& xml);
        void         read_application_types(std::vector<XSSObject> & applications);
        XSSModule    read_module(const str& src, XSSApplicationRenderer app, XSSObject module);
        void         read_types(XSSObject module_data, XSSApplicationRenderer app, XSSModule module);
        void         read_includes(XSSObject project_data);
        void         read_include(fs::path def, fs::path src, XSSContext ctx);
        void         compile_ast(xs_container& ast, XSSContext ctx);
        DynamicArray get_event_impl(XSSObject obj, const str& event_name, XSSEvent& ev);
      private:
        //cache
        void read_object_array(fs::path file, std::vector<XSSObject>& classes_data);
		    void compile_xs_file(fs::path file, xs_container& result);
		};
}


#endif