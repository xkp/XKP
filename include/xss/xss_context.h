#ifndef XS_CONTEXT_HH
#define XS_CONTEXT_HH

#include <xs.h>
#include <xs/array.h>
#include <xs/linker.h>

#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;

namespace xkp {

//forwards
class		xss_object;
class   xss_property;
class   xss_event;
class   xss_method;
struct  xss_context;
class		xss_type;

//interfaces
struct ILanguage;
struct IRenderer;
struct ICodeRenderer;
struct IExpressionRenderer;
struct IArgumentRenderer;

//reference types
typedef reference<xss_object>		        XSSObject;
typedef reference<xss_property>         XSSProperty;
typedef reference<xss_context>	        XSSContext;
typedef reference<xss_event>		        XSSEvent;
typedef reference<xss_method>		        XSSMethod;
typedef reference<xss_type>		          XSSType;
typedef reference<ILanguage>		        Language;
typedef reference<ICodeRenderer>	      CodeRenderer;
typedef reference<IExpressionRenderer>  ExpressionRenderer;
typedef reference<IArgumentRenderer>    ArgumentRenderer;

//misc
typedef std::vector<XSSObject> XSSObjectList;

class xss_object : public editable_object<xss_object>,
									 public boost::enable_shared_from_this<xss_object>
	{
		public:
			xss_object();
    public:
      virtual void copy(XSSObject obj);
		public:
      //IDinamicObject
      virtual bool resolve(const str& name, schema_item& result);
		public:
			//accesors
      template <typename T> 
      T get(const str& what, T default_value)
        {
          return variant_cast<T>(dynamic_get(this, what), default_value);
        }

      str           id();
      str           type_name();
			XSSType			  type();
			void					set_type(XSSType type);
			XSSObject			parent();
			DynamicArray	children();
			DynamicArray	properties();
			DynamicArray	methods();
			DynamicArray	events();

      void set_id(const str& id);
      void set_type_name(const str& id);
		public:
      //misc
      XSSObject              find(const str& what);
      std::vector<XSSObject> find_by_class(const str& which);
      DynamicArray           get_event_impl(const str& event_name, XSSEvent& ev);
		public:
      //children management
			void add_child(XSSObject obj);
      void remove_child(XSSObject obj);
		public:
			XSSProperty           get_property(const str& name);
			std::vector<XSSEvent> get_events(const str& name);
			XSSMethod		          get_method(const str& name);
    public:
      str           id_;
      str           type_name_;
			XSSType       type_;
			XSSObject     parent_;
			DynamicArray	children_;
			DynamicArray	properties_;
			DynamicArray	methods_;
			DynamicArray	events_;
	};

class xss_type : public xss_object  
  {
    public:
      xss_type();
      xss_type(schema* xs_type);
    public:
      void    set_super(XSSType super);
      void    set_definition(XSSObject def);
      schema* native_type();
    public:
      void as_enum();
      void as_array(XSSType type);
      void as_variant();
    public:
      bool is_enum();
      bool is_array();
      bool is_object();
      bool is_native();
      bool is_variant();
    private:
      XSSType super_;
      schema* xs_type_;
      bool    is_enum_;
      bool    is_array_;
      bool    is_object_;
      bool    is_variant_;
  };

//the language interface
struct IRenderer
  {
    virtual str render() = 0;
  };

struct ICodeRenderer : public IRenderer
  {
    virtual XSSType type() = 0;
  };

struct IExpressionRenderer : public IRenderer 
  {
    virtual XSSType type() = 0;
  };

struct IArgumentRenderer : public IRenderer
  {
  };

struct ILanguage
  {
    virtual CodeRenderer       compile_code(code& cde, param_list_decl& params, XSSContext ctx)	= 0;
    virtual ExpressionRenderer compile_expression(expression expr, XSSContext ctx)							= 0;
		virtual ArgumentRenderer   compile_args(param_list_decl& params, XSSContext ctx)					  = 0;
    virtual str                resolve_this(XSSContext ctx)																			= 0;
    virtual str                resolve_separator(XSSObject lh = XSSObject())										= 0;
  };

//resolver
enum RESOLVE_ITEM
  {
    RESOLVE_ANY,
    RESOLVE_INSTANCE,
  };

//code scope, this should not be public
struct xss_context_scope : scope
  {
    xss_context_scope() {}
    xss_context_scope(XSSContext owner);

    void set(XSSContext owner);

    virtual void register_symbol(const str& name, variant value);
    virtual bool resolve(const str& name, variant& result);
    private:
      XSSContext owner_; //td: !!! weak references
  };

struct resolve_info
  {
    resolve_info(RESOLVE_ITEM _type, variant _value):
      type(_type),
      value(_value)
      {
      } 

    RESOLVE_ITEM type;
    variant      value; 
  };

struct xss_context : boost::enable_shared_from_this<xss_context>
  {
    xss_context(XSSContext parent, fs::path path = fs::path());
	  
    public:
      XSSType       get_type(const str& type);
      XSSType       add_type(const str& id, XSSType type, bool override_parent = false);
      XSSObject     get_this();
      void          set_this(XSSObject this_);
      Language      get_language();
      void          set_language(Language lang);
      code_context  get_compile_context();
      fs::path      path();
      void          register_dsl(const str& id, DslLinker dsl);
      void          add_parameter(const str& id, XSSType type);
    public:
      variant resolve(const str& id, RESOLVE_ITEM item_type = RESOLVE_ANY);
      variant resolve_path(const std::vector<str>& path);
      void    register_symbol(RESOLVE_ITEM type, const str& id, variant symbol);
    protected:
      typedef std::map<str, XSSType>  type_list;
      typedef std::pair<str, XSSType> type_list_pair;

			XSSContext parent_;	
      Language   lang_;
      XSSObject  this_;
      type_list  types_;
      fs::path   path_;
      param_list args_;
    protected:
      //symbols
      typedef std::map<str, resolve_info>  symbol_list;
      typedef std::pair<str, resolve_info> symbol_list_pair;
      
      symbol_list symbols_;
    protected:
      //this might not be the best way to simulate the link context
      //a much better way will be to integrate the contexts, that'll be latter
      type_registry     code_types_;
      xss_context_scope code_scope_;
      dsl_list          dsls_; 

      bool got_dsls_;
      void collect_dsl();
  };

//these are basically copies of their xs counterpart, but offer xss stuff, like generating
//they are also vm friendly, unlike the low level xs's ast.
class xss_property : public xss_object
  {
		public:
			xss_property();
			xss_property(const xss_property& other);
			xss_property(const str& name, XSSType type, variant value, XSSObject _this_);
			xss_property(const str& name, XSSType type, variant value, variant _get, variant _set, XSSObject _this_);

			virtual void copy(XSSObject obj);

      variant   get;
			variant   set;
			size_t    flags;
			XSSObject this_;
			variant   value_;
			XSSType	  type;

      str render_value();

			//td: revise interface
			//variant get_value();
			//str			resolve_assign(const str& value);
			//str			resolve_value();
  };

class xss_event : public xss_object
  {
		public:
			xss_event();
			xss_event(const xss_event& other);
			xss_event(const str& name);

			str          name;
			DynamicArray impls;
			variant			 args;

			bool implemented();
  };

class xss_method : public xss_object
  {
		public:
			xss_method();
			xss_method(const xss_method& other);
			xss_method(const str& _name, XSSType type, variant _args, variant _code);

			str     name;
			XSSType	type;
			variant args;
			variant code;
  };

//utils
struct xss_utils
  {
    static str var_to_string(variant& v);
  };

//glue
template <typename T>
struct xss_object_schema : editable_object_schema<T>
  {
    virtual void declare()
      {
				this->template property_<str>         ("id",          &T::id_);
				this->template property_<DynamicArray>("properties",  &T::properties_);
				this->template property_<DynamicArray>("events",			&T::events_);
				this->template property_<DynamicArray>("methods",		  &T::methods_);
				this->template property_<DynamicArray>("children",		&T::children_);
		}
  };

struct xss_type_schema : xss_object_schema<xss_type>
  {
    virtual void declare()
      {
				xss_object_schema<xss_type>::declare();

				inherit_from<xss_object>();

				readonly_property<bool>("is_enum",    &xss_type::is_enum);
				readonly_property<bool>("is_array",   &xss_type::is_array);
				readonly_property<bool>("is_object",  &xss_type::is_object);
				readonly_property<bool>("is_native",  &xss_type::is_native);
				readonly_property<bool>("is_variant", &xss_type::is_variant);
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

struct xss_method_schema : xss_object_schema<xss_method>
  {
    virtual void declare()
      {
				xss_object_schema<xss_method>::declare();

				inherit_from<xss_object>();

        property_("name", &xss_method::name);
        property_("type", &xss_method::type);
        property_("args", &xss_method::args);
        property_("code", &xss_method::code);
      }
  };

struct xss_property_schema : xss_object_schema<xss_property>
  {
    virtual void declare()
      {
				xss_object_schema<xss_property>::declare();

				inherit_from<xss_object>();

				property_("name",  &xss_property::id_);
        property_("get",   &xss_property::get);
        property_("set",   &xss_property::set);
        property_("value", &xss_property::value_);
        property_("type",  &xss_property::type);

        method_<str, 0>("render_value", &xss_property::render_value);
      }
  };

register_complete_type(xss_object,    xss_object_schema<xss_object>);
register_complete_type(xss_type,      xss_type_schema);
register_complete_type(xss_event,		  xss_event_schema);
register_complete_type(xss_property,  xss_property_schema);
register_complete_type(xss_method,	  xss_method_schema);

register_iterator(XSSObject);

}


#endif
