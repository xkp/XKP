
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
      virtual fs::path locate(const str& name) = 0;
  };

//data structures
typedef std::vector<XSSType> type_list;
typedef std::map<str, Idiom> idiom_list;

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

      Language      read_language(DataEntity project);
      variant       read_value(DataEntity de);
      Idiom         read_idiom(DataEntity de, XSSContext ctx);
      void          validate_root(entity_list& root);
      Idiom         compile_include(const str& def, const str& src);
      void          assure_id(DataEntity de);
      XSSType       read_type(DataEntity de, Idiom parent, XSSContext ctx);
      XSSType       read_enum(DataEntity de, XSSContext ctx);
      XSSSignature  read_signature(DataEntity de, Idiom idiom, XSSContext ctx);
      XSSProperty   read_property(DataEntity de, Idiom idiom, XSSContext ctx);
      XSSMethod     read_method(DataEntity de, XSSContext ctx);
      XSSEvent      read_event(DataEntity de, XSSContext ctx);
      void          register_idiom(const str& id, Idiom idiom);
      Idiom         find_idiom(const str& idiom);
      XSSExpression read_expression(DataEntity de, XSSType type, const str& attribute);
  };
}
#endif