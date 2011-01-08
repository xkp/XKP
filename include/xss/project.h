
#ifndef XSS_PROJECT_HH
#define XSS_PROJECT_HH

#include <base.h>
#include <schema.h>
#include <dynamic_objects.h>
#include <xs.h>

#include "xss_context.h"
#include "xss_generator.h"
#include "xs/array.h"

#include "boost/filesystem.hpp" 
namespace fs = boost::filesystem;

namespace xkp{

class xss_project : public boost::enable_shared_from_this<xss_project>
  {
    public:
      xss_project() {}
      xss_project(const xss_project& other) {}
    public:
      void build();
      str  output_path();
			void base_path(fs::path path);
    public:
			typedef std::vector<XSSObject> XSSObjectList;

      std::vector<str>  includes;
      variant           application;
      variant           idiom;
      variant           path;
      XSSObjectList			instances;
      XSSObjectList			classes;

      void compile_instance(const str& filename, XSSObject instance);
			void compile_ast(xs_container& ast, XSSObject instance);
      void register_instance(const str& id, XSSObject instance);
      void render_instance(XSSObject instance, const str& xss, int indent);
      str  resolve_dispatcher(XSSObject instance, const str& event_name);
      str  instance_class(XSSObject instance);
      str  inline_properties(XSSObject instance);
      XSSObject find_class(const str& event_name);
      void breakpoint(const param_list params);
      str  generate_file(const str& fname, XSSContext context = XSSContext());
      void prepare_context(base_code_context& context, XSSGenerator gen);
      str  generate_xss(const str& xss, XSSGenerator gen);
			str	 source_file_name(const str& fname);
			str	 output_file_name(const str& fname);
      str  load_file(const str& fname);
			void output_file(const str& fname, const str& contents);
			variant evaluate_property(XSSObject obj, const str& prop);
			XSSObject get_class(const str& name);
			str	get_anonymous_id(const str& class_name);
			XSSObject resolve_path(const std::vector<str>& path, XSSObject base);
			variant resolve_property(const str& path, variant parent);
			str last_rendered(int count);
			void log(const param_list params);
    public:
      //access
      DynamicArray  get_property_array(XSSObject obj);
      DynamicArray  get_method_array(XSSObject obj);
      DynamicArray  get_event_array(XSSObject obj);
      DynamicArray  get_children_array(XSSObject obj);

      //some utils, god those are long names
      XSSObject get_instance(const str& id);
      void          add_application_file(const str& file, XSSObject obj);
      DynamicArray  get_event_impl(XSSObject obj, const str& event_name);

			//access to current generator
			XSSGenerator generator();
    public:
      //instance registry, for generation purposes
      //these are applications objects (i.e, pure data)
      typedef std::map<str,  size_t> instance_registry;
      typedef std::pair<str, size_t> instance_registry_pair;
      typedef std::map<str,  XSSObject>   class_registry;
      typedef std::pair<str, XSSObject>   class_registry_pair;

      instance_registry instances_;
      class_registry    classes_;
    public:
      //must keep some info about files
      struct file_info
        {
          file_info(const str& f, XSSObject o): obj(o), file(f) {}

          XSSObject obj;
          str           file;
        };

      typedef std::vector<file_info> file_list;

      file_list app_files_;
    private:
      xss_idiom*								idiom_;
      fs::path									base_path_;
      fs::path									source_path_;
      fs::path									output_path_;
      
			meta_array_schema					array_type_;
      XSSGenerator							current_;
      XSSContext								context_;
			std::stack<XSSGenerator>	generators_; 	

      void save_file(const str& fname, const str& contents);
      void preprocess();
      void read_classes(const str& class_library_file);

		public:
			void push_generator(XSSGenerator gen);
			void pop_generator();

		private:
			typedef std::map<str, int> anonymous_list;	
			anonymous_list anonymous_;
  };

typedef reference<xss_project> XSSProject;

//a little object to represent the output
struct out
	{
		out();
		out(XSSProject prj);

		void append(variant v);
		str  line_break(); 

		private:	
			XSSProject prj_;
	};

//glue
template <typename T>
struct xss_object_schema : editable_object_schema<T>
  {
    virtual void declare()
      {
				property_("properties", &T::properties_);
				property_("events",			&T::events_);
				property_("children",		&T::children_);
			}
  };


struct xss_project_schema : object_schema<xss_project>
  {
    virtual void declare()
      {
        static_field("includes",  &xss_project::includes);
        static_field("instances", &xss_project::instances, TRANSIENT);
        static_field("classes",   &xss_project::classes, TRANSIENT);

        property_("application", &xss_project::application);
        property_("path",        &xss_project::path);
        property_("idiom",       &xss_project::idiom);

        dynamic_method_ ("breakpoint", &xss_project::breakpoint);
        dynamic_method_ ("linker_breakpoint", &xss_project::breakpoint);
        dynamic_method_ ("log", &xss_project::log);

				method_<void,			2>("compile_instance",    &xss_project::compile_instance);
        method_<void,			2>("register_instance",   &xss_project::register_instance);
        method_<void,			3>("render_instance",     &xss_project::render_instance);
        method_<str,			2>("resolve_dispatcher",  &xss_project::resolve_dispatcher);
        method_<str,			1>("instance_class",      &xss_project::instance_class);
				method_<str,			1>("inline_properties",   &xss_project::inline_properties);
				method_<variant,	2>("evaluate_property",   &xss_project::evaluate_property);
				method_<str,			1>("genid",								&xss_project::get_anonymous_id);
				method_<variant,	2>("resolve_property",		&xss_project::resolve_property);
      }
  };

struct xss_event_schema : xss_object_schema<xss_event>
  {
    virtual void declare()
      {
				xss_object_schema<xss_event>::declare();

				inherit_from<xss_object>();

        property_("name",  &xss_event::name);
        property_("impls", &xss_event::impls);
				readonly_property<bool>("implemented", &xss_event::implemented);
      }
  };

struct xss_property_schema : xss_object_schema<xss_property>
  {
    virtual void declare()
      {
				inherit_from<xss_object>();

				xss_object_schema<xss_property>::declare();

				property_("name",  &xss_property::name);
        property_("get",   &xss_property::get);
        property_("set",   &xss_property::set);

        method_<str, 0>("generate_value", &xss_property::generate_value);
        method_<str, 1>("resolve_assign", &xss_property::resolve_assign);
      }
  };

struct out_schema : object_schema<out>
  {
    virtual void declare()
      {
				readonly_property<str>("new_line", &out::line_break);

        method_<void, 1>("<<",  &out::append);
      }
  };

register_complete_type(xss_object,		xss_object_schema<xss_object>);
register_complete_type(xss_project,		xss_project_schema);
register_complete_type(xss_event,			xss_event_schema);
register_complete_type(xss_property,  xss_property_schema);
register_complete_type(out,						out_schema);

register_iterator(XSSObject);

}

#endif
