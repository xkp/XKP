#ifndef XSS_COMPILER_HH
#define XSS_COMPILER_HH

#include <map>

#include <base.h>
#include <schema.h>
#include <dynamic_objects.h>
#include <xs.h>

#include "xss_context.h"

#include <json/writer.h>

namespace xkp
{
  //forwards
	class xss_application_renderer;
  class xss_module;
  class xss_compiler;
  class xss_string;
  class xss_math;

  //interfaces
  struct IXSSRenderer;

  //reference types
  typedef reference<xss_application_renderer> XSSApplicationRenderer;
  typedef reference<xss_module>               XSSModule;
  typedef reference<xss_compiler>             XSSCompiler;
  typedef reference<xss_string>               XSSString;
  typedef reference<xss_math>                 XSSMath;
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

      virtual bool busy() = 0; //td:
    };

  class xss_visitor
    {
      public:
        virtual void visit(const str& tag, const str& text, param_list* args) = 0;
    };

  struct IPreprocessHandler
    {
      virtual void handle(XSSObject obj, XSSModule module) = 0;
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
        void               register_instance(XSSObject obj);
        bool               one_of_us(XSSObject obj);
      private:
        XSSContext   ctx_;
        size_t       ev_pprocess_;
        fs::path     path_;
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

  //output handling
  class ICompilerOutput
    {
      public:
        virtual void out(const str& cat, const str& text, param_list* params) = 0;
        virtual void success()                                                = 0;
        virtual void error(param_list& data)                                  = 0;
        virtual str  string()                                                 = 0; 
    };

  class ConsoleOutput : public ICompilerOutput
    {
      public:
        virtual void out(const str& cat, const str& text, param_list* params);
        virtual void success();
        virtual void error(param_list& data);
        virtual str  string(); 
    };

  class JsonOutput : public ICompilerOutput
    {
      public:
        JsonOutput();
      public:
        virtual void out(const str& cat, const str& text, param_list* params);
        virtual void success();
        virtual void error(param_list& data);
        virtual str  string(); 
      private:
        Json::Value json_;
    };

  class xss_compiler : public boost::enable_shared_from_this<xss_compiler>
		{
      public:
        xss_compiler();
        xss_compiler(ICompilerOutput* out);
      public:
        void        build(fs::path xml);
				XSSRenderer compile_xss_file(const str& src_file, XSSContext ctx);
				XSSRenderer compile_xss_file(fs::path src_file, XSSContext ctx);
				XSSRenderer compile_xss(const str& src, XSSContext ctx, fs::path path = fs::path());
			  void        output_file(const str& fname, const str& contents);
			  void        output_file(fs::path fpath, const str& contents);
        str         output_path();
        str         project_path();
        str         genid(const str& what);
        void        xss(const param_list params);
        void        inject(const param_list params);
        void        log(const param_list params);
        void        error(const param_list params);
        bool        parse_expression(variant v);
        variant     compile_expression(const str& expr);
        str         render_expression(const str& expr, XSSObject this_);
        str         replace_identifier(const str& s, const str& src, const str& dst);
        variant     evaluate_property(XSSProperty prop);
        str         renderer_file(const str& file);
        str         idiom_path(XSSObject obj, const str& file);
        fs::path    type_path(const str& type_name);
        str         full_path(const str& file);
        fs::path    compiling();
        void        copy_file(const str& src, const str& dst);
        void        out(variant what);
        XSSType     get_type(const str& type);
        XSSType     type_of(variant v);
        str         property_set(XSSProperty prop, const str& path, const str& value);
        str         property_get(XSSProperty prop, const str& path);
        XSSObject   analyze_expression(const str& expr, variant this_);
        bool        is_type(variant v);
        str         instantiate(variant v);
        str         file(fs::path path);
      public:
        //renderer stack
        void        push_renderer(XSSRenderer renderer);
        void        pop_renderer();
        XSSRenderer current_renderer();
        XSSRenderer previous_renderer();
        XSSRenderer entry_renderer();
        XSSContext  current_context();
      public:
        XSSObject options_;
		  private:
        ICompilerOutput*                    out_;
        std::vector<XSSApplicationRenderer> applications_;
        std::vector<XSSRenderer>            renderers_;
        fs::path                            base_path_;
        fs::path                            project_path_;
        fs::path                            output_path_;
        fs::path                            app_path_;
        fs::path                            compiling_;
        XSSRenderer                         entry_;
        bool                                use_event_instance_;
        XSSApplicationRenderer              current_app_;

        XSSObject   read_project(fs::path xml_file);
        void        read_application_types(std::vector<XSSObject> & applications);
        XSSModule   read_module(const str& src, XSSApplicationRenderer app, XSSObject module);
        void        read_types(XSSObject module_data, XSSApplicationRenderer app, XSSModule module);
        void        read_instances(XSSObject module_data, XSSApplicationRenderer app, XSSModule module);
        void        read_includes(XSSObject project_data);
        void        read_include(fs::path def, fs::path src, XSSContext ctx, XSSApplicationRenderer app);
        void        read_application(const str& app_file);
        void        compile_ast(xs_container& ast, XSSContext ctx);
        bool        options(const str& name);
        Language    get_language(const str& name);
        void        register_language_objects(const str& language_name, XSSContext context);
        void        pre_process(XSSApplicationRenderer renderer, XSSObject obj, XSSObject parent, IPreprocessHandler* handler, bool exclude_module = false);
        void        run();
        void        copy_files(XSSObject project_data);
        void        xss_args(const param_list params, param_list& result, fs::path& output_file, str& marker, MARKER_SOURCE& marker_source, XSSContext& ctx);
        void        preprocess_type(XSSType clazz, XSSObject def_class, const str& super, XSSContext ctx, XSSApplicationRenderer app);
      private:
        //id gen
        typedef std::map<str, int> genid_list;

        genid_list genid_;
      private:
        //cache
        std::multimap<int, XSSRenderer> xss_cache;

        str   load_file(fs::path file);
        void  read_object_array(fs::path file, XSSContext ctx, std::vector<XSSObject>& classes_data);
		    void  compile_xs_file(fs::path file, xs_container& result);
		};

  class xss_string
    {
      public:
        int  size(const str& s);
        int  find(const str& s, const str& subs, int pos = 0);
        int  find_last(const str& s, const str& subs);
        bool empty(const str& s);
        str  erase(const str& s, int pos = 0, int npos = 0);
        str  substr(const str& s, int pos = 0, int npos = 0);
    };

  class xss_math
    {
      public:
        double max(double v1, double v2);
        double min(double v1, double v2);
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

        readonly_property<XSSObject>("options", &xss_compiler::options_);

        method_<str,        1>("genid",	              &xss_compiler::genid);
        method_<bool,       1>("parse_expression",	  &xss_compiler::parse_expression);
        method_<str,        2>("render_expression",   &xss_compiler::render_expression);
        method_<variant,    1>("compile_expression",  &xss_compiler::compile_expression);
        method_<str,	      3>("replace_identifier",  &xss_compiler::replace_identifier);
        method_<variant,    1>("evaluate_property",   &xss_compiler::evaluate_property);
        method_<str,        1>("renderer_file",	      &xss_compiler::renderer_file);
        method_<str,        2>("idiom_path",	        &xss_compiler::idiom_path);
        method_<str,        1>("full_path",	          &xss_compiler::full_path);
        method_<str,        0>("output_path",         &xss_compiler::output_path);
        method_<str,        0>("project_path",        &xss_compiler::project_path);
        method_<void,       2>("copy_file",           &xss_compiler::copy_file);
        method_<void,       1>("out",                 &xss_compiler::out);
        method_<XSSType,    1>("get_type",            &xss_compiler::get_type);
        method_<XSSType,    1>("type_of",             &xss_compiler::type_of);
        method_<str,        3>("property_set",        &xss_compiler::property_set);
        method_<str,        2>("property_get",        &xss_compiler::property_get);
        method_<XSSObject,  2>("analyze_expression",  &xss_compiler::analyze_expression);
        method_<bool,       1>("is_type",             &xss_compiler::is_type);
        method_<str,        1>("instantiate",         &xss_compiler::instantiate);
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

struct xss_string_schema : object_schema<xss_string>
  {
    virtual void declare()
      {
        method_<int,  1>("size",      &xss_string::size);
        method_<int,  3>("find",      &xss_string::find);
        method_<int,  2>("find_last", &xss_string::find_last);
        method_<bool, 1>("empty",     &xss_string::empty);
        method_<str,  3>("erase",     &xss_string::erase);
        method_<str,  3>("substr",    &xss_string::substr);
      }
  };

struct xss_math_schema : object_schema<xss_math>
  {
    virtual void declare()
      {
        method_<double, 2>("max", &xss_math::max);
        method_<double, 2>("min", &xss_math::min);
      }
  };

  register_complete_type(xss_compiler,  xss_compiler_schema);
  register_complete_type(xss_module,    xss_module_schema);
  register_complete_type(xss_string,    xss_string_schema);
  register_complete_type(xss_math,      xss_math_schema);
}


#endif
