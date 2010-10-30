
#ifndef XSS_PROJECT_HH
#define XSS_PROJECT_HH

#include <base.h>
#include <schema.h>
#include <xs.h>

#include "xss_generator.h"
#include "xs/array.h"

namespace xkp{

//forwards
class xss_project;
struct xss_property;

//and their references
typedef reference<xss_project> XSSProject;
typedef reference<xss_property> XSSProperty;
  
struct xss_code_context : base_code_context
  {
    xss_code_context(xss_project& _project);
    xss_code_context(xss_code_context& other);
      
    //this will function as resolver
    virtual XSSProperty   get_property(const str& name);
    virtual XSSProperty   get_property(DynamicObject obj, const str& name);
    virtual DynamicObject resolve_instance(const str& id);
    virtual variant       evaluate_property(DynamicObject obj, const str& name);
    public:
      xss_project& project;
  };
  
typedef reference<xss_code_context> XSSContext;

struct xss_composite_context : xss_code_context
  {
    xss_composite_context(XSSContext ctx);
      
    //this will function as resolver
    virtual XSSProperty   get_property(const str& name);
    virtual XSSProperty   get_property(DynamicObject obj, const str& name);
    virtual DynamicObject resolve_instance(const str& id);
    virtual variant       evaluate_property(DynamicObject obj, const str& name);
    private:
      XSSContext ctx_;
  };

//the idiom interface, under designed
struct xss_idiom
  {
    virtual void    set_context(XSSContext ctx)                                                = 0;
    virtual variant process_method(DynamicObject instance, xs_method& mthd)                    = 0; 
    virtual variant process_event(DynamicObject instance, const str& event_name, xs_event& ev) = 0;
    virtual variant process_code(code& cde, DynamicObject this_)                               = 0;
    virtual variant process_expression(expression expr, DynamicObject this_)                   = 0;
  };

//these are basically copies of their xs counterpart, but offer xss stuff, like generating
//they are also vm friendly, unlike the low level xs's ast.  
struct xss_property
  {
    xss_property(): flags(0) {}
    xss_property(const xss_property& other);
    xss_property(const str& name, variant value, DynamicObject _this_);
    xss_property(const str& name, variant value, variant _get, variant _set, DynamicObject _this_);

    str           name;
    variant       get;
    variant       set;
    size_t        flags;
    DynamicObject this_;
    
    str     generate_value();
    variant get_value();
    private:
      variant value_;
  };
  
struct xss_event
  {
    xss_event();
    xss_event(const xss_event& other);

    str          name;
    DynamicArray impls;
  };
  
typedef reference<xss_event> XSSEvent;
    
class xss_project : public boost::enable_shared_from_this<xss_project>
  {
    public:
      xss_project() {}
      xss_project(const xss_project& other) {}
    public:
      void build();
    public:
      std::vector<str>  includes;
      variant           application;
      variant           idiom;
      variant           path;
      DynamicObjectList instances;
      DynamicObjectList classes;
      
      void compile_instance(const str& filename, DynamicObject instance);
      void register_instance(const str& id, DynamicObject instance);
      void render_instance(DynamicObject instance, const str& xss);      
      str  resolve_dispatcher(DynamicObject instance, const str& event_name);
      str  instance_class(DynamicObject instance);
      DynamicObject find_class(const str& event_name);
      void breakpoint(const param_list params);
    public:
      //access 
      DynamicArray  get_property_array(DynamicObject obj);
      DynamicArray  get_method_array(DynamicObject obj);
      DynamicArray  get_event_array(DynamicObject obj);

      //some utils, god those are long names
      DynamicObject get_instance(const str& id);
      void          add_application_file(const str& file, DynamicObject obj);
      DynamicArray  get_event_impl(DynamicObject obj, const str& event_name);
    public:
      //instance registry, for generation purposes
      //these are applications objects (i.e, pure data)
      typedef std::map<str,  size_t> instance_registry;
      typedef std::pair<str, size_t> instance_registry_pair;
      
      instance_registry instances_;
    public:
      //must keep some info about files
      struct file_info
        {
          file_info(const str& f, DynamicObject o): obj(o), file(f) {}
          
          DynamicObject obj;
          str           file;
        };
        
      typedef std::vector<file_info> file_list;
      
      file_list app_files_;
    private:
      xss_idiom*        idiom_; 
      str               source_path_; 
      str               output_path_; 
      meta_array_schema array_type_;
      XSSGenerator      current_; 
      XSSContext        context_; 

      void prepare_context(base_code_context& context, xss_generator& gen);
      str  generate_xss(const str& xss, xss_generator& gen);
      str  load_file(const str& fname);
      void save_file(const str& fname, const str& contents);
      str  generate_file(const str& fname);
      void preprocess();
      str  localize_file(const str& );
  };  

struct xss_project_schema : object_schema<xss_project>  
  {
    xss_project_schema()
      {
        static_field("includes",  &xss_project::includes);
        static_field("instances", &xss_project::instances, TRANSIENT);
        static_field("classes",   &xss_project::classes, TRANSIENT);

        property_("application", &xss_project::application);
        property_("path",        &xss_project::path);
        property_("idiom",       &xss_project::idiom);

        method_<void, 2>("compile_instance",    &xss_project::compile_instance);
        method_<void, 2>("register_instance",   &xss_project::register_instance);
        method_<void, 2>("render_instance",     &xss_project::render_instance);
        method_<str,  2>("resolve_dispatcher",  &xss_project::resolve_dispatcher);
        method_<str,  1>("instance_class",      &xss_project::instance_class);
        dynamic_method_ ("breakpoint",          &xss_project::breakpoint);
      }
  };
  
struct xss_event_schema : object_schema<xss_event>  
  {
    xss_event_schema()
      {
        property_("name",  &xss_event::name);
        property_("impls", &xss_event::impls);
      }
  };

struct xss_property_schema : object_schema<xss_property>  
  {
    xss_property_schema()
      {
        property_("name",  &xss_property::name);
        property_("get",   &xss_property::get);
        property_("set",   &xss_property::set);
        
        method_<str, 0>("generate_value", &xss_property::generate_value);
      }
  };

register_complete_type(xss_project,   xss_project_schema);
register_complete_type(xss_event,     xss_event_schema);
register_complete_type(xss_property,  xss_property_schema);
  
}

#endif