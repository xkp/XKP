#ifndef XS_CONTEXT_HH
#define XS_CONTEXT_HH

#include <xs.h>
#include <xs/array.h>

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

//reference types
typedef reference<xss_object>		XSSObject;
typedef reference<xss_property> XSSProperty;
typedef reference<xss_context>	XSSContext;
typedef reference<xss_event>		XSSEvent;
typedef reference<xss_method>		XSSMethod;
typedef reference<ILanguage>		Language;
typedef reference<xss_type>		  XSSType;

//misc
typedef std::vector<XSSObject> XSSObjectList;

class xss_object : public editable_object<xss_object>,
									 public boost::enable_shared_from_this<xss_object>
	{
		public:
			xss_object();
		public:
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
			XSSObject			type();
			void					set_type(XSSObject type);
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
      void                   add_surrogate(XSSObject s);
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
      XSSObjectList surrogates_;
      str           id_;
      str           type_name_;
			XSSObject     type_;
			XSSObject     parent_;
			DynamicArray	children_;
			DynamicArray	properties_;
			DynamicArray	methods_;
			DynamicArray	events_;
	};

class xss_type : public xss_object  
  {
    public:
      void set_super(XSSType super);
      void set_definition(XSSObject def);
    private:
      XSSType super_;
  };

//the idiom interface, under designed
struct ILanguage
  {
    virtual variant compile_code(code& cde, param_list_decl& params, XSSContext ctx)	= 0;
    virtual variant compile_expression(expression expr, XSSObject this_)							= 0;
		virtual variant compile_args(param_list_decl& params)															= 0;
    virtual str     resolve_this(XSSContext ctx)																			= 0;
    virtual str     resolve_separator(XSSObject lh = XSSObject())											= 0;
  };

//resolver
enum RESOLVE_ITEM
  {
    RESOLVE_INSTANCE,
  };

struct xss_context
  {
    xss_context(XSSContext parent = XSSContext());
	  
    public:
      XSSType   get_type(const str& type);
      void      add_type(const str& id, XSSType type, bool override_parent = false);
      XSSObject get_this();
      void      set_this(XSSObject this_);
      Language  get_language();
      void      set_language(Language lang);
    public:
      variant resolve(const str& id, RESOLVE_ITEM item_type);
      variant resolve_path(const std::vector<str>& path);
    protected:
      typedef std::map<str, XSSType>  type_list;
      typedef std::pair<str, XSSType> type_list_pair;

			XSSContext parent_;	
      Language   lang_;
      XSSObject  this_;
      type_list  types_;
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

			str       name;
			variant   get;
			variant   set;
			size_t    flags;
			XSSObject this_;
			variant   value_;
			XSSType	  type;

			//td: revise interface
      //str     generate_value();
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

//glue
template <typename T>
struct xss_object_schema : editable_object_schema<T>
  {
    virtual void declare()
      {
				this->template property_<DynamicArray>("properties",  &T::properties_);
				this->template property_<DynamicArray>("events",			&T::events_);
				this->template property_<DynamicArray>("methods",		  &T::methods_);
				this->template property_<DynamicArray>("children",		&T::children_);
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
				inherit_from<xss_object>();

				xss_object_schema<xss_property>::declare();

				property_("name",  &xss_property::name);
        property_("get",   &xss_property::get);
        property_("set",   &xss_property::set);
        property_("value", &xss_property::value_);
        property_("type",  &xss_property::type);
      }
  };

register_complete_type(xss_object,    xss_object_schema<xss_object>);
register_complete_type(xss_event,		  xss_event_schema);
register_complete_type(xss_property,	xss_property_schema);
register_complete_type(xss_method,		xss_method_schema);

register_iterator(XSSObject);

}


#endif
