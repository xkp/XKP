#ifndef XSS_COMPILER_HH
#define XSS_COMPILER_HH

#include <map>

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
      renderer_parameter(str _id, XSSType _type, variant _default_value):
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
      virtual XSSContext                context()                                     = 0;
      virtual fs::path                  file()                                        = 0;
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
        void       set_target(const str& target); 
        fs::path   entry_point(); 
        fs::path   output_path();
        void       output_path(fs::path path);
        void       set_output_path(const str& path); 
        XSSModule  instance_idiom(XSSObject inst);
        XSSModule  type_idiom(const str& type);
      public:
        std::vector<XSSModule>& modules();
      private:
        fs::path               filename_;
        fs::path               output_path_;
        str                    target_;
        XSSContext             context_;
        std::vector<XSSModule> modules_;
    };
  
  enum pre_process_result
    {
      PREPROCESS_HANDLED,
      PREPROCESS_KEEPGOING,
    };

	class xss_module : public xss_object
    {
      public:
        xss_module();
        xss_module(XSSContext ctx);
      public:
        pre_process_result pre_process(XSSObject obj, XSSObject parent); 
        DynamicArray       instances();
        fs::path           path(); 
        void               set_path(fs::path p); 
        void               register_module_type(XSSType type);
        void               register_user_type(XSSType type);
        bool               has_type(const str& type);
      private:
        XSSContext   ctx_;
        size_t       ev_pprocess_;
        fs::path     path_; 

        void register_instance(XSSObject obj);
        bool one_of_us(XSSObject obj);
      private:
        //types
        typedef std::map<str, XSSType>  type_list;
        typedef std::pair<str, XSSType> type_list_pair;

        type_list types_;
      public:
        //glue visibility
        DynamicArray instances_;
        DynamicArray utypes_;
        DynamicArray all_types();
    };

  class xss_compiler : public boost::enable_shared_from_this<xss_compiler>
		{
      public:
        void        build(fs::path xml);
				XSSRenderer compile_xss_file(const str& src_file, XSSContext ctx);
				XSSRenderer compile_xss_file(fs::path src_file, XSSContext ctx);
				XSSRenderer compile_xss(const str& src, XSSContext ctx, fs::path path = fs::path());
			  void        output_file(const str& fname, const str& contents);
			  void        output_file(fs::path fpath, const str& contents);
        str         genid(const str& what);
        void        xss(const param_list params);
        void        inject(const param_list params);
        void        log(const param_list params);
        void        error(const param_list params);
        bool        parse_expression(variant v);
        str         render_expression(const str& expr, XSSObject this_);
        str         replace_this(const str& s, const str& this_);
        variant     resolve_property(const str& prop, variant parent);
        str         renderer_file(const str& file);
        str         idiom_path(XSSObject obj, const str& file);
        str         full_path(const str& file);
        fs::path    compiling();
      public:
        //renderer stack
        void        push_renderer(XSSRenderer renderer);
        void        pop_renderer();
        XSSRenderer current_renderer();
        XSSContext  current_context();
		  private:
        std::vector<XSSApplicationRenderer> applications_;
        std::stack<XSSRenderer>             renderers_;
        fs::path                            base_path_;
        fs::path                            source_path_;
        fs::path                            output_path_;
        fs::path                            compiling_;
        XSSObject                           options_;
        
        XSSObject   read_project(fs::path xml_file);
        void        read_application_types(std::vector<XSSObject> & applications);
        XSSModule   read_module(const str& src, XSSApplicationRenderer app, XSSObject module);
        void        read_types(XSSObject module_data, XSSApplicationRenderer app, XSSModule module);
        void        read_includes(XSSObject project_data);
        void        read_include(fs::path def, fs::path src, XSSContext ctx, XSSApplicationRenderer app);
        void        read_application(const str& app_file);
        void        compile_ast(xs_container& ast, XSSContext ctx);
        bool        options(const str& name);
        Language    get_language(const str& name);
        void        pre_process(XSSApplicationRenderer renderer, XSSObject obj, XSSObject parent);
        void        run();
        void        copy_files(XSSObject project_data);
        void        xss_args(const param_list params, param_list& result, fs::path& output_file, str& marker);
      private:
        //id gen
        typedef std::map<str, int> genid_list;
        
        genid_list genid_;
      private:
        //cache
        std::map<int, XSSRenderer> xss_cache;

        str   load_file(fs::path file);
        void  read_object_array(fs::path file, std::vector<XSSObject>& classes_data);
		    void  compile_xs_file(fs::path file, xs_container& result);
		};

//glue
struct xss_compiler_schema : object_schema<xss_compiler>
  {
    virtual void declare()
      {
        dynamic_method_ ("xss",    &xss_compiler::xss);
        dynamic_method_ ("inject", &xss_compiler::inject);
        dynamic_method_ ("log",    &xss_compiler::log);
        dynamic_method_ ("error",  &xss_compiler::error);

        method_<str,      1>("genid",	            &xss_compiler::genid);
        method_<bool,     1>("parse_expression",	&xss_compiler::parse_expression);
        method_<str,      2>("render_expression", &xss_compiler::render_expression);
        method_<str,	    2>("replace_this",			&xss_compiler::replace_this);
        method_<variant,  2>("resolve_property",	&xss_compiler::resolve_property);
        method_<str,      1>("renderer_file",	    &xss_compiler::renderer_file);
        method_<str,      2>("idiom_path",	      &xss_compiler::idiom_path);
        method_<str,      1>("full_path",	        &xss_compiler::full_path);
      }
  };

struct xss_module_schema : xss_object_schema<xss_module>
  {
    virtual void declare()
      {
				xss_object_schema<xss_module>::declare();

				inherit_from<xss_object>();

        property_("instances",  &xss_module::instances_);
        property_("user_types", &xss_module::utypes_);
        
        readonly_property<DynamicArray>("types", &xss_module::all_types);
      }
  };

  register_complete_type(xss_compiler,  xss_compiler_schema);
  register_complete_type(xss_module,    xss_module_schema);
}


#endif