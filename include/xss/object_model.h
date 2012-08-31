
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
      virtual fs::path   locate(const str& filename)    = 0;
      virtual DataReader load_data(const str& filename) = 0;
      virtual str        load_file(const str& filename) = 0;
  };

//data structures
typedef std::vector<XSSType>   type_list;
typedef std::map<str, XSSType> type_map;

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
    private:
      fs::path   path_;
      XSSContext ctx_;

      struct snap_shot
        {
          snap_shot(code_pos& _begin, code_pos& _end, XSSContext _context):
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

          code_pos   begin;
          code_pos   end;
          XSSContext context;
        };

      typedef std::vector<snap_shot> snap_shots;
      snap_shots items_;
  };

typedef std::map<fs::path, document> document_map;

class object_model
  {
    public:
      object_model();
    public:
      Application load(DataReader project, param_list& args);
    private:
      LanguageFactory languages_;
      FileSystem      fs_;
      idiom_list      idioms_;

      void            assure_id(DataEntity de);
      DataEntity      assure_unique_root(DataReader dr);
      void            register_idiom(const str& id, Idiom idiom);
      Idiom           find_idiom(const str& idiom);
      void            compile_include(const str& def, const str& src, XSSContext ctx);
      void            compile_xs(const str& text, XSSContext ctx, type_map& classes, instance_map& instances, document& doc);

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
      type_map        read_include_def(DataEntity de, XSSContext ctx);
      instance_map    read_include_singleton(DataEntity de, XSSContext ctx);
    private:
      //document model
      document_map documents_;

      document& create_document(const str& src_file);
  };
}
#endif