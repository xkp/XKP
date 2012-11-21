#ifndef XSS_COMPILER_HH
#define XSS_COMPILER_HH

#include <map>

#include <base.h>
#include <schema.h>
#include <dynamic_objects.h>
#include <xs.h>

#include "xss_context.h"
#include "object_model.h"

#include <json/writer.h>

namespace xkp
{
  //forwards
  class xss_compiler;
  class xss_string;
  class xss_math;

  //interfaces
  struct IXSSRenderer;

  //reference types
  typedef reference<xss_compiler> XSSCompiler;
  typedef reference<xss_string>   XSSString;
  typedef reference<xss_math>     XSSMath;
  typedef reference<IXSSRenderer> XSSRenderer;

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
      virtual str                       get()                                         = 0;

      virtual bool busy() = 0; //td:
    };

  class xss_visitor
    {
      public:
        virtual void visit(const str& tag, const str& text, param_list* args) = 0;
    };

  //0.9.5
  //struct IPreprocessHandler
  //  {
  //    virtual void handle(XSSObject obj, XSSModule module) = 0;
  //  };

  //utils
  struct dependency_list
    {
      XSSObjectList items;

      void add(XSSObject dep);

      private:
        std::map<str, int> cache_;
    };

  //classes
	
  //0.9.5
  //class xss_application_renderer : public xss_object
 //   {
 //     public:
 //       xss_application_renderer(fs::path entry_point, Language lang, XSSCompiler compiler);
 //     public:
 //       XSSContext context();
 //       void          register_module(const str& id, XSSModule module);
 //       void          register_singleton(XSSObject singleton);
 //       str           target();
 //       void          set_target(const str& target);
 //       fs::path      entry_point();
 //       fs::path      output_path();
 //       void          output_path(fs::path path);
 //       void          set_output_path(const str& path);
 //       XSSModule     instance_idiom(XSSObject inst);
 //       XSSModule     type_idiom(const str& type);
 //       XSSModule     get_idiom(const str& id);
 //       XSSObjectList get_singletons();
 //     public:
 //       std::vector<XSSModule>& modules();
 //     private:
 //       fs::path               filename_;
 //       fs::path               output_path_;
 //       str                    target_;
 //       XSSContext             context_;
 //       std::vector<XSSModule> modules_;
 //       XSSObjectList          singletons_;
 //   };

 // enum pre_process_result
 //   {
 //     PREPROCESS_HANDLED,
 //     PREPROCESS_KEEPGOING,
 //   };

	//class xss_module : public xss_object
 //   {
 //     public:
 //       xss_module();
 //       xss_module(XSSContext ctx);
 //     public:
 //       pre_process_result pre_process(XSSObject obj, XSSObject parent);
 //       void               pre_process_type(XSSType type);
 //       DynamicArray       instances();
 //       fs::path           path();
 //       void               set_path(fs::path p);
 //       void               register_module_type(XSSType type);
 //       void               register_user_type(XSSType type);
 //       bool               has_type(const str& type);
 //       void               register_instance(XSSObject obj);
 //       bool               one_of_us(XSSObject obj);
 //       void               set_definition(XSSObject def);
 //       void               used();
 //       void               set_host(XSSModule host);
 //     private:
 //       XSSContext   ctx_;
 //       size_t       ev_pprocess_;
 //       size_t       ev_pprocess_type_;
 //       fs::path     path_;
 //       bool         used_; 
 //       XSSModule    host_;
 //     private:
 //       //types
 //       typedef std::map<str, XSSType>  type_list;
 //       typedef std::pair<str, XSSType> type_list_pair;

 //       type_list types_;
 //     public:
 //       //glue visibility
 //       DynamicArray  instances_;
 //       DynamicArray  utypes_;
 //       XSSObjectList dependencies_;
 //       DynamicArray  all_types();
 //   };

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
        //0.9.5
        //str          render_expression(const str& expr, XSSObject this_);

				XSSRenderer  compile_xss_file(const str& src_file, XSSContext ctx, const str& html_template = str());
				XSSRenderer  compile_xss_file(fs::path src_file, XSSContext ctx, const str& html_template = str());
				XSSRenderer  compile_xss(const str& src, XSSContext ctx, fs::path path = fs::path(), const str& html_template = str());
			  void         output_file(const str& fname, const str& contents);
			  void         output_file(fs::path fpath, const str& contents);
        str          output_path();
        void         set_output_path(const fs::path& path);
        str          project_path();
        str          genid(const str& what);
        void         xss(const param_list params);
        void         inject(const param_list params);
        void         log(const param_list params);
        void         error(const param_list params);
        bool         parse_expression(variant v);
        variant      compile_expression(const str& expr);
        str          render_expr(const expression& expr, XSSObject this_);
        str          replace_identifier(const str& s, const str& src, const str& dst);
        variant      evaluate_property(XSSProperty prop);
        str          renderer_file(const str& file);
        str          idiom_path(XSSObject obj, const str& file);
        fs::path     type_path(const str& type_name);
        str          full_path(const str& file);
        fs::path     compiling();
        void         copy_file(const str& src, const str& dst);
        DynamicArray find_files(const str& init_path, const str& filter);
        void         out(variant what);
        XSSType      get_type(const str& type);
        XSSObject    get_instance(const str& instance);
        XSSType      type_of(variant v);
        str          property_set(XSSProperty prop, const str& path, const str& value);
        str          property_get(XSSProperty prop, const str& path);
        XSSProperty  add_object_property(const param_list params);
        bool         is_type(variant v);
        str          instantiate(const param_list params);
        str          render_ctor_args(const param_list params);
        bool         application_object(XSSObject obj);

        //0.9.5
        void       file_system(FileSystem fs);
        void       register_symbol(const str& symbol, variant value);
        void       build(const fs::path& entry, const fs::path& output);
        XSSContext context();
        void       render_code(XSSCode code);
        str        code_to_string(XSSCode code); 
        void       render_signature(XSSSignature sig);
        str        signature_to_string(XSSSignature sig); 
        str        type_to_string(XSSType sig); 
        void       render_expression(XSSExpression expr);
        str        expression_to_string(XSSExpression expr); 

        //0.9.5
        //void         build(fs::path xml, param_list& args);
        //XSSModule    idiom_by_class(const str& class_name);
        //XSSModule    idiom_by_id(const str& id);
        //void         using_idiom(const str& idiom);
        //XSSObject    analyze_expression(const param_list params);
        //str          file(fs::path path);
        //str          render_code(const str& code, param_list_decl& args, XSSContext ctx);
        //void         add_dependencies(XSSObjectList& dependencies, XSSObject idiom);
        //DynamicArray get_dependencies();
        //DynamicArray idiom_dependencies(const str& idiom);
        str          build_project(const param_list params);
        str          get_result();
        void         render_app_types(const str& renderer);
        void         type_dependencies(XSSType type, dependency_list& deps);
        str          render_value(variant value);
        str          get_env_var(const str& key);
        str          get_os_name();
        void         no_ouput();
        str          escape_file(const str& filename);
      public:
        //renderer stack
        void        push_renderer(XSSRenderer renderer);
        XSSRenderer pop_renderer();
        XSSRenderer current_renderer();
        XSSRenderer previous_renderer();
        //XSSRenderer entry_renderer();
        XSSContext  current_context();
		    fs::path	  current_output_file();	
      public:
        XSSObject options_;
		  private:
        ICompilerOutput*         out_;
        Application              app_;
        std::vector<XSSRenderer> renderers_;
        fs::path                 base_path_;
        fs::path                 output_path_;
        fs::path                 compiling_;
        param_list               params_;
        bool                     no_output_;    

        //0.9.5
        XSSContext ctx_;         
        FileSystem fs_;      

        //0.9.5
        //fs::path                 project_path_;
        //fs::path                 app_path_;
        //bool                     use_event_instance_;
        //std::vector<XSSType>     app_types_;
        //XSSRenderer              entry_;
        //str                      result_;
        //XSSModule     read_module(const str& src, XSSApplicationRenderer app, XSSObject module);
        //XSSObject     read_project(fs::path xml_file, param_list& args);
        //void          read_application_types(std::vector<XSSObject> & applications, param_list& args);
        //void          read_types(XSSObject module_data, XSSApplicationRenderer app, XSSModule module);
        //XSSObjectList read_instances(XSSObject module_data, XSSApplicationRenderer app, XSSModule module);
        //void          read_includes(XSSObject project_data);
        //void          read_include(fs::path def, fs::path src, XSSContext ctx, XSSApplicationRenderer app);
        //void          read_application(const str& app_file);
        //void          compile_ast(xs_container& ast, XSSContext ctx);
        //void          pre_process(XSSApplicationRenderer renderer, XSSObject obj, XSSObject parent, IPreprocessHandler* handler, bool exclude_module = false);
        //void          preprocess_type(XSSType clazz, XSSObject def_class, const str& super, XSSContext ctx, XSSApplicationRenderer app);
        bool          options(const str& name);
        Language      get_language(const str& name);
        void          register_language_objects(const str& language_name, XSSContext context);
        void          run();
        void          copy_files(XSSObject project_data);
        void          xss_args(const param_list params, param_list& result, fs::path& output_file, str& marker, MARKER_SOURCE& marker_source, XSSContext& ctx, str& html_template);
        void          init_project_context(code_context& cctx);
      private:
        //id gen
        typedef std::map<str, int> genid_list;

        genid_list genid_;
      private:
        //cache
        std::multimap<int, XSSRenderer> xss_cache;
        std::map<str, XSSRenderer>      xss_file_cache;

        //0.9.5;
        //str   load_file(fs::path file);
        //void  read_object_array(fs::path file, XSSContext ctx, std::vector<XSSObject>& classes_data);
		    //void  compile_xs_file(fs::path file, xs_container& result, XSSContext ctx);
      private:
        //0.9.5;
        //dependencies
        //typedef std::map<str, int>  dependency_map;
        //typedef std::pair<str, int> dependency_pair;

        //dependency_map dependencies_;
        //XSSObjectList  deps_;

        //void add_dependency(const str& href, XSSObject obj, XSSObject idiom);
        //void collect_dependencies(XSSType type, XSSType context = XSSType());
      public:
        //glue
        str  __instantiation(const param_list params);
        str  __assignment(const str& path, variant left, variant right);
        void __render_assignment(const str& path, variant left, variant right);
		};

  class xss_string
    {
      public:
        int  size(const str& s);
        int  find(const str& s, const str& subs, int pos = 0);
        int  find_first(const str& s, const str& subs);
        int  find_last(const str& s, const str& subs);
        int  find_first_not(const str& s, const str& subs);
        int  find_last_not(const str& s, const str& subs);
        bool empty(const str& s);
        str  erase(const str& s, int pos = 0, int npos = 0);
        str  substr(const str& s, int pos = 0, int npos = 0);
        str  strip_spaces(str s);
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

        //dynamic_function_<XSSObject>  ("analyze_expression",  &xss_compiler::analyze_expression);
        dynamic_function_<XSSProperty>("add_object_property", &xss_compiler::add_object_property);
        dynamic_function_<str>        ("instantiate",         &xss_compiler::instantiate);
        dynamic_function_<str>        ("render_ctor_args",    &xss_compiler::render_ctor_args);
        dynamic_function_<str>        ("build",               &xss_compiler::build_project);
        dynamic_function_<str>        ("instantiation_to_string", &xss_compiler::__instantiation);

        readonly_property<XSSObject>("options", &xss_compiler::options_);

        //0.9.5
        method_<void, 1>("render_code",	         &xss_compiler::render_code);
        method_<str,  1>("code_to_string",       &xss_compiler::code_to_string);
        method_<void, 1>("render_signature",	   &xss_compiler::render_signature);
        method_<str,  1>("signature_to_string",  &xss_compiler::signature_to_string);
        method_<str,  1>("type_to_string",       &xss_compiler::type_to_string);
        method_<void, 1>("render_expression",    &xss_compiler::render_expression);
        method_<str,  1>("expression_to_string", &xss_compiler::expression_to_string);
        method_<void, 3>("render_assignment",    &xss_compiler::__render_assignment);
        method_<str,  3>("assignment_to_string", &xss_compiler::__assignment);


        //0.9.5
        //method_<str,          0>("project_path",       &xss_compiler::project_path);
        //method_<str,          2>("render_expression",  &xss_compiler::render_expression);

        method_<str,          1>("genid",	             &xss_compiler::genid);
        method_<bool,         1>("parse_expression",	 &xss_compiler::parse_expression);
        method_<variant,      1>("compile_expression", &xss_compiler::compile_expression);
        method_<str,	        3>("replace_identifier", &xss_compiler::replace_identifier);
        method_<variant,      1>("evaluate_property",  &xss_compiler::evaluate_property);
        method_<str,          1>("renderer_file",	     &xss_compiler::renderer_file);
        method_<str,          2>("idiom_path",	       &xss_compiler::idiom_path);
        method_<str,          1>("full_path",	         &xss_compiler::full_path);
        method_<str,          0>("output_path",        &xss_compiler::output_path);
        method_<void,         2>("copy_file",          &xss_compiler::copy_file);
        method_<void,         1>("out",                &xss_compiler::out);
        method_<XSSType,      1>("get_type",           &xss_compiler::get_type);
        method_<XSSObject,    1>("get_instance",       &xss_compiler::get_instance);
        method_<XSSType,      1>("type_of",            &xss_compiler::type_of);
        method_<str,          3>("property_set",       &xss_compiler::property_set);
        method_<str,          2>("property_get",       &xss_compiler::property_get);
        method_<bool,         1>("is_type",            &xss_compiler::is_type);
        method_<bool,         1>("application_object", &xss_compiler::application_object);
        //0.9.5
        //method_<variant,      1>("idiom_by_id",        &xss_compiler::idiom_by_id);
        //method_<variant,      1>("idiom_by_class",     &xss_compiler::idiom_by_class);
        //method_<void,         1>("using_idiom",        &xss_compiler::using_idiom);
        //method_<void,         1>("render_app_types",   &xss_compiler::render_app_types);
        //method_<DynamicArray, 0>("get_dependencies",   &xss_compiler::get_dependencies);
        //method_<DynamicArray, 1>("idiom_dependencies", &xss_compiler::idiom_dependencies);

        method_<str,          1>("render_value",       &xss_compiler::render_value);
        method_<DynamicArray, 2>("find_files",         &xss_compiler::find_files);
        method_<str,          1>("get_env_var",        &xss_compiler::get_env_var);
        method_<str,          0>("os",                 &xss_compiler::get_os_name);
        method_<str,          1>("escape_file",        &xss_compiler::escape_file);
      }
  };

//0.9.5
//struct xss_module_schema : xss_object_schema<xss_module>
//  {
//    virtual void declare()
//      {
//				xss_object_schema<xss_module>::declare();
//
//				inherit_from<xss_object>();
//
//        property_("instances",  &xss_module::instances_);
//        property_("user_types", &xss_module::utypes_);
//
//        readonly_property<DynamicArray>("types", &xss_module::all_types);
//      }
//  };

struct xss_string_schema : object_schema<xss_string>
  {
    virtual void declare()
      {
        method_<int,  1>("size",          &xss_string::size);
        method_<int,  3>("find",          &xss_string::find);
        method_<int,  2>("find_first",    &xss_string::find_first);
        method_<int,  2>("find_last",     &xss_string::find_last);
        method_<int,  2>("find_first_not",&xss_string::find_first_not);
        method_<int,  2>("find_last_not", &xss_string::find_last_not);
        method_<bool, 1>("empty",         &xss_string::empty);
        method_<str,  3>("erase",         &xss_string::erase);
        method_<str,  3>("substr",        &xss_string::substr);
        method_<str,  1>("strip_spaces",  &xss_string::strip_spaces);
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
  //0.9.5
  //register_complete_type(xss_module,    xss_module_schema);
  register_complete_type(xss_string,    xss_string_schema);
  register_complete_type(xss_math,      xss_math_schema);
}


#endif
