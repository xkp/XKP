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
  class xss_compiler;

  //interfaces
  struct IXSSRenderer;

  //reference types
  typedef reference<xss_application_renderer> XSSApplicationRenderer;
  typedef reference<xss_module>               XSSModule;
  typedef reference<xss_compiler>             XSSCompiler;
  typedef reference<IXSSRenderer>             XSSRenderer;

  //interfaces
  struct renderer_parameter
    {
      renderer_parameter(str _id, XSSType _type, str _default_value):
        id(_id),
        type(_type),
        default_value(_default_value)
        {
        }

      str     id;
      XSSType type;
      variant default_value;
    };

  typedef std::vector<renderer_parameter> renderer_parameter_list;
  
  struct IXSSRenderer
    {
      virtual renderer_parameter_list&  params()                                      = 0;
      virtual str                       render(XSSObject this_, param_list* args)     = 0;
      virtual void                      append(const str& what)                       = 0;
      virtual void                      append_at(const str& what, const str& marker) = 0;
    };

  //classes
	class xss_application_renderer : public xss_object
    {
      public:
        xss_application_renderer(fs::path entry_point, Language lang, XSSCompiler compiler);
      public:
        XSSContext context();
        void       register_module(const str& id, XSSModule module);
        str        target(); 
        fs::path   entry_point(); 
        void       set_target(const str& target); 
      private:
        fs::path   filename_;
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

  class xss_compiler : public boost::enable_shared_from_this<xss_compiler>
		{
      public:
        void        build(fs::path xml);
        fs::path    output_path();
				XSSRenderer compile_xss_file(const str& src_file, XSSContext ctx);
				XSSRenderer compile_xss_file(fs::path src_file, XSSContext ctx);
				XSSRenderer compile_xss(const str& src, XSSContext ctx, fs::path path = fs::path());
			  void        output_file(const str& fname, const str& contents);
      public:
        //renderer stack
        void        push_renderer(XSSRenderer renderer);
        void        pop_renderer();
        XSSRenderer current_renderer();
		  private:
        std::vector<XSSApplicationRenderer> applications_;
        std::stack<XSSRenderer>             renderers_;
        fs::path                            base_path_;
        fs::path                            source_path_;
        XSSObject                           options_;
        
        XSSObject read_project(fs::path xml_file);
        void      read_application_types(std::vector<XSSObject> & applications);
        XSSModule read_module(const str& src, XSSApplicationRenderer app, XSSObject module);
        void      read_types(XSSObject module_data, XSSApplicationRenderer app, XSSModule module);
        void      read_includes(XSSObject project_data);
        void      read_include(fs::path def, fs::path src, XSSContext ctx);
        void      compile_ast(xs_container& ast, XSSContext ctx);
        bool      options(const str& name);
        Language  get_language(const str& name);
        void      run();
      private:
        //cache
        str   load_file(fs::path file);
        void  read_object_array(fs::path file, std::vector<XSSObject>& classes_data);
		    void  compile_xs_file(fs::path file, xs_container& result);
		};
}


#endif