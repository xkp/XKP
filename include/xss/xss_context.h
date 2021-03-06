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
class   xss_dsl;

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
typedef reference<xss_dsl>		          XSSDSL;
typedef reference<ILanguage>		        Language;
typedef reference<ICodeRenderer>	      CodeRenderer;
typedef reference<IExpressionRenderer>  ExpressionRenderer;
typedef reference<IArgumentRenderer>    ArgumentRenderer;

//enums
enum MARKER_SOURCE
  {
    MS_CURRENT,
    MS_PREVIOUS,
    MS_ENTRY,
  };

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

      template <typename T>
      void set(const str& what, T value)
        {
          dynamic_set(this, what, value);
        }

      str           id();
      str           output_id();
      str           type_name();
			XSSObject			parent();
			DynamicArray	children();
			DynamicArray	properties();
			DynamicArray	methods();
			DynamicArray	events();

      void set_id(const str& id);
      void set_output_id(const str& id);
      void set_type_name(const str& id);
      void set_parent(XSSObject parent);

			virtual XSSType	  type();
			virtual void      set_type(XSSType type);
			virtual XSSObject	idiom();
			virtual void      set_idiom(XSSObject id);
    public:
      struct query_info
        {
          query_info(const str& expr): 
            expression(expr), result(new dynamic_array) {}

          void register_property(XSSProperty prop);
          void add_property(XSSProperty prop);
          bool has(XSSProperty prop);

          DynamicArray                result;
          str                         expression;
          std::map<str, XSSProperty>  found;
        };

      void                   query_properties_impl(query_info &info);
      DynamicArray           query_properties(const str& query);
      bool                   has_property(const str& prop);
      bool                   have_value(const str& prop);
		public:
      //misc
      XSSObject              find(const str& what);
      XSSObject              find_by_id(const str& what);
      std::vector<XSSObject> find_by_class(const str& which);
      DynamicArray           find_by_type(const str& which);
      DynamicArray           get_event_impl(const str& event_name, XSSEvent& ev);
      DynamicArray           get_event_code(const str& event_name);
      variant                get_event_args(const str& event_name);
      DynamicArray           get_attributes();
		  bool                   is_injected(const str& name);
      void                   add_method(const str& event_name, XSSMethod m);
		  bool                   empty();
      void                   propertize();
    public:
      //children management
			void add_child(XSSObject obj);
      void remove_child(XSSObject obj);
      void fixup_children(XSSContext ctx);
		public:
      void add_property_(XSSProperty prop);
      XSSProperty add_property(const str& name, variant value, XSSType type);
      void register_property(const str& name, XSSProperty new_prop = XSSProperty());
      void register_method(const str& name, XSSMethod new_mthd = XSSMethod());
      void register_event_impl(const str& name, XSSEvent new_evt = XSSEvent());

			XSSProperty get_property(const str& name);
      XSSProperty get_shallow_property(const str& name);
			XSSEvent    get_event(const str& name);
			XSSMethod		get_method(const str& name);
      variant     attribute_value(const str& name);

    public:
      str           id_;
      str           output_id_;
      str           type_name_;
			XSSType       type_;
			XSSObject     parent_; //td: !!! weaks
			DynamicArray	children_;
			DynamicArray	properties_;
			DynamicArray	methods_;
			DynamicArray	events_;
			XSSObject     idiom_;
	};

class xss_type : public xss_object
  {
    public:
      xss_type();
      xss_type(schema* xs_type);
    public:
      //xss_object
      virtual bool resolve(const str& name, schema_item& result);
    public:
      void          set_super(XSSType super);
      void          set_definition(XSSObject def);
      schema*       native_type();
      XSSType       array_type();
      void          inherit();
      XSSType       get_super();
      DynamicArray  ctor_args();
      void          register_instance(XSSObject obj);
      void          register_foreign_instance(XSSObject obj);
      XSSContext    context();
      void          set_context(XSSContext ctx);
      XSSObjectList get_dependencies();
    public:
      void as_enum();
      void as_array(XSSType type);
      void as_variant();
      void as_object();
      void as_unresolved();
    public:
      bool is_enum();
      bool is_array();
      bool is_object();
      bool is_native();
      bool is_variant();
      bool is_unresolved();
    public:
      Language get_language();
      void     set_language(Language lang);
    private:
      XSSType     array_type_;
      schema*     xs_type_;
      bool        is_enum_;
      bool        is_array_;
      bool        is_object_;
      bool        is_variant_;
      bool        is_unresolved_;
      XSSContext  ctx_;
    public:
      XSSType       super_;
      Language      lang_;
      DynamicArray  ctor_args_; //td: constructors
      DynamicArray  all_instances_;
      DynamicArray  local_instances_;
      DynamicArray  foreign_instances_;
      XSSObjectList dependencies_;
  };

class xss_dsl : public xss_object
  {
    public:
      virtual str  render(dsl& info, XSSContext ctx) = 0;
      virtual void pre_process(dsl& info, XSSContext ctx) {}
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
    virtual variant eval(XSSContext ctx) = 0;
  };

struct IArgumentRenderer : public IRenderer
  {
    virtual void             add(const str& name, XSSType type) = 0;
    virtual param_list_decl& get()                              = 0;
  };

struct ILanguage
  {
    virtual variant compile_code(code& cde, param_list_decl& params, XSSContext ctx)	                    = 0;
    virtual variant compile_expression(expression expr, XSSContext ctx)							                      = 0;
		virtual variant compile_args(param_list_decl& params, XSSContext ctx)					                        = 0;
    virtual str     resolve_this(XSSContext ctx)																			                    = 0;
    virtual str     resolve_separator(XSSObject lh = XSSObject())										                      = 0;
    virtual bool    can_cast(XSSType left, XSSType right)                                                 = 0;
    virtual void    init_context(XSSContext ctx)                                                          = 0; //td: !!!
    virtual void    init_application_context(XSSContext ctx)                                              = 0;
    virtual XSSType resolve_array_type(XSSType type, const str& at_name, XSSContext ctx)                  = 0;
    virtual str     render_value(XSSType type, variant value)                                             = 0;
    virtual str     property_get(XSSProperty prop, const str& path, XSSContext ctx)                       = 0;
    virtual str     property_set(XSSProperty prop, const str& path, const str& value, XSSContext ctx)     = 0;
    virtual str     render_assignment(const str& path, const str& prop, const str& value)                 = 0;
    virtual str     expression_path(const str& expr )                                                     = 0;
    virtual str     array_operation(operator_type op, const str& arr, const str& value, XSSContext ctx)   = 0;
    virtual str     render_expression(expression& expr, XSSContext ctx)                                   = 0;
    virtual str     instantiate(XSSType type, XSSObject instance, DynamicArray rt, param_list& args)      = 0;
    virtual str     render_ctor_args(XSSType type, XSSObject instance, DynamicArray rt, param_list& args) = 0;
    virtual bool    custom_operator(XSSType lt, XSSType rt, str l, str r, operator_type op, str& res)     = 0;
  };

//resolver
enum RESOLVE_ITEM
  {
    RESOLVE_ANY,
    RESOLVE_INSTANCE,
    RESOLVE_METHOD,
    RESOLVE_PROPERTY,
    RESOLVE_NATIVE,
    RESOLVE_CONST,
    RESOLVE_VARIABLE,
    RESOLVE_TYPE,
    RESOLVE_CHILD,
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
    resolve_info():
      what(RESOLVE_ANY),
      left(null),
      search_this(true),
      output(null),
      found_this(false),
      shallow(false)
      {
      }

    resolve_info(const resolve_info& other):
      what(other.what),
      left(other.left),
      search_this(other.search_this),
      output(null),
      found_this(false),
      shallow(false)
      {
      }

    RESOLVE_ITEM  what;
    XSSType       type;
    variant       value;
    str*          output;
    resolve_info* left;
    bool          search_this;
    bool          found_this;
    bool          shallow;
  };

struct symbol_data
  {
    symbol_data(RESOLVE_ITEM _type, variant _value):
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
      XSSType       get_type(schema* type);
      XSSType       get_array_type(XSSType type);
      XSSType       add_type(const str& id, XSSType type, bool override_parent = false);
      XSSObject     get_this();
      void          set_this(XSSObject this_);
      Language      get_language();
      void          set_language(Language lang);
      code_context  get_compile_context();
      fs::path      path();
      void          register_dsl(const str& id, DslLinker dsl);
      void          register_xss_dsl(const str& id, XSSDSL dsl);
      XSSDSL        get_xss_dsl(const str& id);
      void          collect_xss_dsls(std::vector<str>& dsls);
      void          add_parameter(const str& id, XSSType type);
      void          set_args(param_list& args);
      param_list&   get_args();
      void          search_native(bool enabled);
    public:
      variant resolve(const str& id, RESOLVE_ITEM item_type = RESOLVE_ANY);
      bool    resolve(const str& id, resolve_info& info);
      variant resolve(const str& id, XSSObject instance, RESOLVE_ITEM item_type = RESOLVE_ANY);
      bool    resolve_path(const std::vector<str>& path, resolve_info& info);
      void    register_symbol(RESOLVE_ITEM type, const str& id, variant symbol, bool overrite = false);
    protected:
      typedef std::map<str, XSSType>  type_list;
      typedef std::pair<str, XSSType> type_list_pair;

			XSSContext parent_;
      Language   lang_;
      XSSObject  this_;
      type_list  types_;
      fs::path   path_;
      param_list args_;
      bool       search_native_;

      variant empty_type_value(RESOLVE_ITEM item_type);
    protected:
      //symbols
      typedef std::map<str, symbol_data>  symbol_list;
      typedef std::pair<str, symbol_data> symbol_list_pair;

      symbol_list symbols_;

      bool find_symbol(const str& id, resolve_info& info);
    protected:
      //this might not be the best way to simulate the link context
      //a much better way will be to integrate the contexts, that'll be latter
      type_registry     code_types_;
      xss_context_scope code_scope_;
      dsl_list          dsls_;

      bool got_dsls_;
      void collect_dsl();
    protected:  
      //XSS dsls, they are different than code dsls
      typedef std::map<str, XSSDSL>  xss_dsl_list;
      typedef std::pair<str, XSSDSL> xss_dsl_pair;

      xss_dsl_list xss_dsls_;
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

			str get_name();

		  virtual void copy(XSSObject obj);
			virtual XSSType type();

      variant   get_;
		  variant   set_;
		  size_t    flags;
		  XSSObject this_;
		  variant   value_;

      XSSObject get_get();
      XSSObject get_set();

      void    set_value(const variant value, XSSType type);
      str     render_value();
      //str     render_get();
      //str	    render_set(const str& value);
      variant eval(XSSContext ctx);
  };

class xss_event : public xss_object
  {
		public:
			xss_event();
			xss_event(const xss_event& other);
			xss_event(const str& name, variant args);

			str get_name();

			DynamicArray impls;
			variant			 args;

			bool implemented();
  };

class xss_method : public xss_object
  {
		public:
			xss_method();
			xss_method(const xss_method& other);
			xss_method(const str& name, XSSType type, variant args, variant code);

			str      get_name();
      variant  code();
      void     add_parameter(const str& name);
      variant& get_parameters(); 

      //xss_object
			virtual XSSType type();

			variant args_;
			variant code_;
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
				this->template property_<str>         ("output_id",   &T::output_id,  &T::set_output_id);
				this->template property_<DynamicArray>("children",		&T::children_);
				this->template property_<DynamicArray>("properties",  &T::properties_);
				this->template property_<DynamicArray>("events",			&T::events_);
				this->template property_<DynamicArray>("methods",		  &T::methods_);
				this->template property_<str>         ("type_name",		&T::type_name_);
				this->template property_<str>         ("class_name",	&T::type_name_);
        this->template property_<XSSType>     ("type",        &T::type,       &T::set_type);
				this->template property_<XSSObject>   ("parent",      &T::parent_);
        this->template property_<XSSObject>   ("idiom",       &T::idiom_);

        this->template method_<DynamicArray, 1>("query_properties", &T::query_properties);
        this->template method_<XSSProperty, 1> ("get_property",     &T::get_property);
        this->template method_<XSSProperty, 3> ("add_property",     &T::add_property);
        this->template method_<bool, 1>        ("has_property",     &T::has_property);
        this->template method_<bool, 1>        ("have_value",       &T::have_value);
        this->template method_<void, 1>        ("add_child",        &T::add_child);
        this->template method_<XSSMethod, 1>   ("get_method",       &T::get_method);
        this->template method_<void, 2>        ("add_method",       &T::add_method);
        this->template method_<DynamicArray, 1>("find_by_type",     &T::find_by_type);
        this->template method_<XSSObject, 1>   ("find",             &T::find);
        this->template method_<DynamicArray, 1>("get_event_code",   &T::get_event_code);
        this->template method_<variant, 1>     ("get_event_args",   &T::get_event_args);
        this->template method_<variant, 1>     ("attribute_value",  &T::attribute_value);
        this->template method_<bool, 0>        ("empty",            &T::empty);
        this->template method_<DynamicArray, 0>("get_attributes",   &T::get_attributes);
		  }
  };

struct xss_type_schema : xss_object_schema<xss_type>
  {
    virtual void declare()
      {
				xss_object_schema<xss_type>::declare();

				inherit_from<xss_object>();

				readonly_property<XSSType>("super",                  &xss_type::get_super);
        readonly_property<bool>("is_enum",                   &xss_type::is_enum);
				readonly_property<bool>("is_array",                  &xss_type::is_array);
				readonly_property<bool>("is_object",                 &xss_type::is_object);
				readonly_property<bool>("is_native",                 &xss_type::is_native);
				readonly_property<bool>("is_variant",                &xss_type::is_variant);
				readonly_property<DynamicArray>("instances",         &xss_type::all_instances_);
				readonly_property<DynamicArray>("local_instances",   &xss_type::local_instances_);
				readonly_property<DynamicArray>("foreign_instances", &xss_type::foreign_instances_);

				readonly_property<DynamicArray>("constructor_params", &xss_type::ctor_args);
      }
  };

struct xss_event_schema : xss_object_schema<xss_event>
  {
    virtual void declare()
      {
				xss_object_schema<xss_event>::declare();

				inherit_from<xss_object>();

        readonly_property<str>("name", &xss_event::get_name);

        property_("args",  &xss_event::args);
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

        readonly_property<str>("name", &xss_method::get_name);

        property_("args", &xss_method::args_);
        property_("code", &xss_method::code_);

        method_<void, 1>("add_parameter", &xss_method::add_parameter);
      }
  };

struct xss_property_schema : xss_object_schema<xss_property>
  {
    virtual void declare()
      {
				xss_object_schema<xss_property>::declare();

				inherit_from<xss_object>();

        readonly_property<str>("name", &xss_property::get_name);

        property_("get",   &xss_property::get_);
        property_("set",   &xss_property::set_);
        property_("value", &xss_property::value_);

        readonly_property<XSSObject>("get_", &xss_property::get_get);
        readonly_property<XSSObject>("set_", &xss_property::get_set);

        method_<str, 0>("render_value", &xss_property::render_value);
      }
  };

register_complete_type(xss_object,    xss_object_schema<xss_object>);
register_complete_type(xss_type,      xss_type_schema);
register_complete_type(xss_event,		  xss_event_schema);
register_complete_type(xss_property,  xss_property_schema);
register_complete_type(xss_method,	  xss_method_schema);

register_iterator(XSSObject);

//interface glue
template <typename T>
struct renderer_schema : object_schema<T>
  {
    virtual void declare_base()
      {
        this->template implements<IRenderer>();

        this->template method_<str, 0>("render", &T::render);
      }

    virtual void declare()
      {
        //compiler pleaser
      }
  };

}


#endif
