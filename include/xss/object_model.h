
#ifndef XSS_OBJECT_MODEL_HH
#define XSS_OBJECT_MODEL_HH

#include "xss_context.h"
#include "data_reader.h"

#include <xs.h>

#include <boost/thread.hpp>

namespace boost {
    struct thread::dummy {};
}

namespace xkp{

//forwards
class  idiom;
class  application;
class  object_model;
class  object_model_thread;
class  idiom;
struct document;

class ILanguageFactory;
class IFileSystem;

//references
typedef reference<idiom>			         Idiom;
typedef reference<application>		     Application;
typedef reference<object_model>		     ObjectModel;
typedef reference<object_model_thread> ObjectModelThread;
typedef reference<document>            Document;

typedef reference<ILanguageFactory> LanguageFactory;
typedef reference<IFileSystem>      FileSystem;

class ILanguageFactory 
  {
    public:
      virtual Language create(const str& name) = 0;
  };

class IFileSystem
  {
    public:
      virtual fs::path   locate(const str& filename, fs::path base_path)    = 0;
      virtual DataReader load_data(const str& filename, fs::path base_path) = 0;
      virtual str        load_file(const str& filename, fs::path base_path) = 0;
      virtual str        load_file(fs::path file)                           = 0;
      virtual DataReader load_data(fs::path file)                           = 0;
  };

//data structures
typedef std::vector<XSSType>            type_list;
typedef std::map<str, XSSType>          type_map;
typedef std::map<str, XSSContext>       context_map;
typedef std::map<str, Idiom>            idiom_list;
typedef std::vector<XSSObject>          instance_list; //td: !!! instances
typedef std::map<str, XSSObject>        instance_map;
typedef std::map<fs::path, document>    document_map;
typedef std::map<fs::path, Application> application_list;

//misc
enum APPLICATION_ITEM
  {
    AI_NONE,
    AI_INSTANCE,
    AI_CLASS,
    AI_APP,
  };

//classes
class idiom
  {
    public:
      void    bind(XSSContext ctx);
      void    set_namespace(const str& _namespace);
      void    add_type(XSSType type);
      void    add_import(XSSType import);
      XSSType import(const str& id);
      str     get_namespace();
    private:
      type_list types_;
      type_list imports_;
      str       namespace_;
  };

class application : public boost::enable_shared_from_this<application>
  {
    public:
      application();
      application(const application& other);
      application(fs::path path);
    public:
      void              entry_point(fs::path ep);
      void              output_file(const str& ep);
      void              compiler_options(DataEntity options);
      void              set_context(XSSContext ctx);
      code_context&     exec_context();
      XSSContext        context();
      fs::path          path();
      document*         create_document(fs::path fname, XSSContext ctx);
      document*         get_document(fs::path path);
      error_list&       errors();
      void              add_error(const str& desc, param_list* info, file_location& loc);
      void              visit_errors(const fs::path& fname, error_visitor* visitor);
      void              clear_file_errors(const fs::path& fname);
      APPLICATION_ITEM  app_item(const str& fname);
      void              app_path(const fs::path& fname);
      fs::path          app_path();
      void              build();
      void              project_object(XSSObject project);
      void              file_system(FileSystem fs);
      void              output_path(const fs::path& fname);
      fs::path          output_path();
      void              add_instance(XSSObject instance);
    public:
      XSSObject root(); 
      void      set_root(XSSObject r); 
      str       name(); 
      void      set_name(const str& name); 
    private:
      fs::path     renderer_;
      fs::path     output_;
      fs::path     output_path_;
      XSSContext   ctx_;
      code_context code_ctx_;
      XSSObject    app_; 
      fs::path     path_;
      fs::path     app_path_;
      XSSObject    project_;
      FileSystem   fs_;
      str          app_name_; 

      document_map  documents_;
      error_list    errors_;
      instance_list instances_;

      str random_id(XSSObject instance);
    public:
      //glue
      DynamicArray __instances();
  };

struct document
  {
    public: 
      document();
      document(XSSContext ctx);
    public: 
      XSSContext  find_context(int line, int column);
      XSSContext  context_by_identity(CONTEXT_IDENTITY id, variant value);
      void        add(XSSContext context);
		  XSSContext  changed(int line, int col, int oldEndLine, int oldEndCol, int newEndLine, int newEndCol);	
      void        refresh_context(XSSContext ctx);
      XSSContext  context();
      void        context(XSSContext ctx);
    private:
      fs::path    path_;
      XSSContext  ctx_;

      struct snap_shot
        {
          snap_shot(file_position& _begin, file_position& _end, XSSContext _context):
            begin(_begin),
            end(_end), 
            context(_context)
            {
            }

          snap_shot(const snap_shot& other):
            begin(other.begin),
            end(other.end),
            context(other.context)
            {
            }

          file_position begin;
          file_position end;
          XSSContext context;
        };

      typedef std::vector<snap_shot> snap_shots;
      snap_shots items_;

	    void synch_extents();
};

//lazy eval
enum FIXUP_TYPE
  {
    FIXUP_OBJECT_TYPE,
    FIXUP_PROPERTY_TYPE,
    FIXUP_RETURN_TYPE,
    FIXUP_INSTANCE_EVENT,
    FIXUP_SUPER_TYPE, 
    FIXUP_ARGUMENT_TYPE, 
    FIXUP_OBJECT_CHILD,
    FIXUP_POLICY_TYPE,
  };

struct fixup_data
  {
    fixup_data(FIXUP_TYPE _id, variant _data):
      id(_id),
      data(_data)
      {
      }      

    FIXUP_TYPE id;
    variant    data;
  };

typedef std::vector<fixup_data> fixup_list;

//internal context
struct om_context
  {
    om_context():
      doc(NULL)
      {
      }

    type_map     classes;
    instance_map instances;
    fixup_list   fixup;
    context_map  contexts; 
    Application  application; 
    XSSObject    instance;
    XSSContext   xss_ctx; 
    Idiom        idiom;

    document*    doc;
  };

struct om_response
  {
    om_response():
      id(0),
      ai_(AI_NONE)
      {
      }

	  om_response(int _id, XSSContext _ctx, om_context& _data, document& _doc, str _request_id, APPLICATION_ITEM _ai):
      id(_id),
      ctx(_ctx),
      data(_data),
      doc(_doc),
      request_id(_request_id),
      ai_(_ai)
      {
      }

    int        id;
    str        request_id; 
    XSSContext ctx;
    om_context data;
    document   doc;
    APPLICATION_ITEM ai_;
  };

class object_model
  {
    public:
      object_model(FileSystem fs, LanguageFactory languages);
    public:
      Application load(DataReader project, param_list& args, fs::path base_path);
			void        register_app(const fs::path& fname, Application app);
      Application remove_app(const fs::path& fname);
      Application get_application(const str& fname);
	    document*   get_document(const str& fname);
	    void		    update_document(const str& fname, om_response& data);
      void        add_include(Application app, const str& def, const str& src);
      void        add_error(Application app, const str& desc, param_list* info, file_location& loc);
      void        visit_errors(const fs::path& fname, error_visitor* visitor);
      Application app_by_file(const fs::path& path);
      bool        changed();
	  
    public:
	    FileSystem filesystem() {return fs_;}	
    private:
      LanguageFactory languages_;
      FileSystem      fs_;
      idiom_list      idioms_;
	    bool			      changed_;	

      void            assure_id(DataEntity de, om_context& octx);
      DataEntity      assure_unique_root(DataReader dr, om_context& octx);
      void            register_idiom(const str& id, Application app, Idiom idiom);
      Idiom           find_idiom(const str& idiom);
      void            handle_include(Application app, const str& def, const str& src, om_context& ctx);
      void            handle_instance(Application app, XSSObject instance, const str& def_file, const str& src_file, om_context& octx);
    public:
      static bool compile_xs(const str& text, XSSContext ctx, om_context& octx, xs_visitor* visitor);
      static bool compile_class(const str& text, XSSContext ctx, om_context& octx);
      static bool compile_instance(XSSObject instance, const str& text, XSSContext ctx, om_context& octx);
    private:
      //data reader
      Language        read_language(DataEntity project, Application app);
      variant         read_value(DataEntity de);
      Idiom           read_idiom(DataEntity de, om_context& ctx);
      XSSType         read_type(DataEntity de, Idiom parent, XSSContext ctx);
      XSSSignature    read_signature(DataEntity de, om_context& ctx);
      XSSProperty     read_property(DataEntity de, XSSObject recipient, om_context& ctx);
      XSSMethod       read_method(DataEntity de, om_context& ctx);
      XSSEvent        read_event(DataEntity de, om_context& ctx);
      InlineRenderer  read_inline_renderer(DataEntity de, param_list& params);
      XSSExpression   read_expression(DataEntity de, XSSType type, const str& attribute);
      XSSObject       read_object(DataEntity de, om_context& ctx, XSSObject instance = XSSObject(), XSSType type = XSSType());
      XSSObject       read_object_instance(DataEntity de, XSSObject parent, om_context& ctx);
      XSSExpression   read_array(DataEntity de, XSSType array_type, om_context& ctx);

      void read_attribute(const str& attr, const str& value, om_context& ctx);
      void read_child(DataEntity de, om_context& ctx);
    public:
      //readers, public for access
      void r_attr_nop(const str& attr, const str& value, const variant& this_, om_context& ctx);
      void r_child_nop(DataEntity de, const variant& this_, om_context& ctx);

      void r_idiom_namespace(const str& attr, const str& value, const variant& this_, om_context& ctx);
      void r_invalid_idiom_attr(const str& attr, const str& value, const variant& this_, om_context& ctx);
      void r_idiom_enum(DataEntity de, const variant& this_, om_context& ctx);
      void r_idiom_import(DataEntity de, const variant& this_, om_context& ctx);
      void r_idiom_type(DataEntity de, const variant& this_, om_context& ctx);
      void r_invalid_idiom_child(DataEntity de, const variant& this_, om_context& ctx);

      void r_object_attr(const str& attr, const str& value, const variant& this_, om_context& ctx);
      void r_object_property(DataEntity de, const variant& this_, om_context& ctx);
      void r_object_method(DataEntity de, const variant& this_, om_context& ctx);
      void r_object_event(DataEntity de, const variant& this_, om_context& ctx);
      void r_object_instance(DataEntity de, const variant& this_, om_context& ctx);

      void r_enum_item(DataEntity de, const variant& this_, om_context& ctx);
      void r_invalid_enum_item(DataEntity de, const variant& this_, om_context& ctx);

      void r_type_super(const str& attr, const str& value, const variant& this_, om_context& ctx);
      void r_type_import(DataEntity de, const variant& this_, om_context& ctx);
      void r_type_constructor(DataEntity de, const variant& this_, om_context& ctx);
      void r_type_child_policy(DataEntity de, const variant& this_, om_context& ctx);
      void r_type_reject_child(DataEntity de, const variant& this_, om_context& ctx);
      void r_type_parent_policy(DataEntity de, const variant& this_, om_context& ctx);
      void r_type_reject_parent(DataEntity de, const variant& this_, om_context& ctx);
      void r_invalid_type_item(DataEntity de, const variant& this_, om_context& ctx);

      void r_include_class(DataEntity de, const variant& this_, om_context& ctx);
      void r_include_instance(DataEntity de, const variant& this_, om_context& ctx);
      void r_invalid_include_child(DataEntity de, const variant& this_, om_context& ctx);

      void r_invalid_array_attr(const str& attr, const str& value, const variant& this_, om_context& ctx);
      void r_array_item(DataEntity de, const variant& this_, om_context& ctx);
      void r_invalid_array_item(DataEntity de, const variant& this_, om_context& ctx);

      void r_invalid_property_child(DataEntity de, const variant& this_, om_context& ctx);
    private:
      //document model
      application_list apps_;

      document* create_document(Application app, const str& src_file, XSSContext ctx);
      void fix_it_up(XSSContext ctx, om_context& octx);
      void bind_it_up(XSSContext ctx, om_context& octx);
      bool check_type(XSSType type, const str& type_name, XSSContext ctx);
      void clear_file_errors(const str& fname);
      void resolve_parent_child(XSSObject parent, XSSObject child, om_context& ctx);
      void parent_child_action(PARENT_CHILD_ACTION action, XSSObject parent, XSSObject child, om_context& ctx);
      bool parse_pca(const str& action, PARENT_CHILD_ACTION& result);
      bool parse_policy(DataEntity de, XSSType owner, parent_policy& result, om_context& ctx, bool& fixup);
      XSSExpression compile_expression(const str& expr, XSSType type);
      variant str2var(const str& value);
      void merge_property(XSSProperty dest, XSSProperty incoming, XSSObject owner, om_context& ctx);
  };

class object_model_thread
  {
    public:
      object_model_thread() : id_(0), stopped_(true) {}
    public:
      int  compile_request(const str& text, const str& id, Application app, APPLICATION_ITEM ai);
      bool get(const str& id, om_response& result);
      bool fetch(str& id, om_response& result);
      void run();
    private:
      //state
      typedef std::map<str, om_response>  response_map;
      typedef std::pair<str, om_response> response_pair;

      struct om_request
        {
          om_request():
            response_id(-1)
            {
            }

          om_request(const str& _id, APPLICATION_ITEM _ai, const str& _text, int _response_id, Application _app):
            id(_id),
            text(_text),
            response_id(response_id),
			      app(_app),
            app_item(_ai)
            {
            }
            
          str              id;
          str              text;
          int              response_id;
		      Application      app;
          APPLICATION_ITEM app_item;
        };
      
      typedef std::vector<om_request> request_list;

      request_list requests_; 
      response_map responses_;
      int          id_;

      //thread
      boost::shared_ptr<boost::thread> thread_;
      boost::mutex                     mutex_;
      bool                             stopped_;
      
      void do_work();
  };

struct application_schema : object_schema<application>
  {
    virtual void declare()
      {
        readonly_property<XSSObject>("root",  &application::root);
        readonly_property<XSSObject>("name",  &application::name);

        readonly_property<DynamicArray>("instances",  &application::__instances);
      }
  };

register_complete_type(application,  application_schema);

}
#endif