
#ifndef XSS_OBJECT_MODEL_HH
#define XSS_OBJECT_MODEL_HH

#include "xss_context.h"
#include "data_reader.h"

#include <xs.h>

namespace xkp{

//forwards
class idiom;
class application;
class object_model;
class idiom;

class ILanguageFactory;
class IFileSystem;

//references
typedef reference<idiom>        Idiom;
typedef reference<application>  Application;
typedef reference<object_model> ObjectModel;

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
typedef std::vector<XSSType>      type_list;
typedef std::map<str, XSSType>    type_map;
typedef std::map<str, XSSContext> context_map;

typedef std::map<str, Idiom> idiom_list;

typedef std::vector<XSSObject>   instance_list; //td: !!! instances
typedef std::map<str, XSSObject> instance_map;

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

class application
  {
    public:
      void          entry_point(fs::path ep);
      void          output_file(const str& ep);
      void          compiler_options(DataEntity options);
      void          set_context(XSSContext ctx);
      code_context& exec_context();
    private:
      fs::path     renderer_;
      fs::path     output_;
      XSSContext   ctx_;
      code_context code_ctx_;
      XSSObject    app_; 
  };

struct document
  {
    public: 
      XSSContext find_context(int line, int column);
      void       add(XSSContext context);
    private:
      fs::path   path_;
      XSSContext ctx_;

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
  };

typedef std::map<fs::path, document> document_map;

//lazy eval
enum FIXUP_TYPE
  {
    FIXUP_OBJECT_TYPE,
    FIXUP_PROPERTY_TYPE,
    FIXUP_RETURN_TYPE,
    FIXUP_INSTANCE_EVENT,
    FIXUP_SUPER_TYPE, 
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
    
    document*    doc;
  };

class object_model
  {
    public:
      object_model(FileSystem fs, LanguageFactory languages);
    public:
      Application load(DataReader project, param_list& args, fs::path base_path);
    private:
      LanguageFactory languages_;
      FileSystem      fs_;
      idiom_list      idioms_;

      void            assure_id(DataEntity de);
      DataEntity      assure_unique_root(DataReader dr);
      void            register_idiom(const str& id, Idiom idiom);
      Idiom           find_idiom(const str& idiom);
      void            compile_include(const str& def, const str& src, XSSContext ctx, om_context& octx);
      void            compile_xs(const str& text, XSSContext ctx, om_context& octx);

    private:
      //data reader
      Language        read_language(DataEntity project);
      variant         read_value(DataEntity de);
      Idiom           read_idiom(DataEntity de, XSSContext ctx);
      XSSType         read_type(DataEntity de, Idiom parent, XSSContext ctx);
      XSSType         read_enum(DataEntity de, XSSContext ctx);
      XSSSignature    read_signature(DataEntity de, Idiom idiom, XSSContext ctx);
      XSSProperty     read_property(DataEntity de, Idiom idiom, XSSContext ctx);
      XSSMethod       read_method(DataEntity de, Idiom idiom, XSSContext ctx);
      XSSEvent        read_event(DataEntity de, Idiom idiom, XSSContext ctx);
      InlineRenderer  read_inline_renderer(DataEntity de);
      XSSExpression   read_expression(DataEntity de, XSSType type, const str& attribute);
      void            read_include_def(DataEntity de, XSSContext ctx, om_context& octx);
      void            read_include_singleton(DataEntity de, XSSContext ctx, om_context& octx);
    private:
      //document model
      document_map documents_;

      document& create_document(const str& src_file, XSSContext ctx);
      void      fix_it_up(XSSContext ctx, om_context& octx);
      void      bind_it_up(XSSContext ctx, om_context& octx);
      bool      check_type(XSSType type, const str& type_name, XSSContext ctx);
    private:
      //error handling
      struct error_info
        {
          error_info(const str _desc, param_list* _info, file_location& _loc):
            desc(_desc),
            info(_info? *_info : param_list()),
            loc(_loc)
            {
            }

          str           desc;
          param_list    info;
          file_location loc;
        };

      typedef std::vector<error_info> error_list;

      error_list errors_;
    public:
      void add_error(const str& desc, param_list* info, file_location& loc);
  };
}
#endif