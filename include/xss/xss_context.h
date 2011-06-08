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

enum RESOLVE_ITEM
  {
    RESOLVE_INSTANCE,
  };

struct xss_context
  {
    xss_context(XSSContext parent = XSSContext());
	  
    public:
      XSSType   get_type(const str& type);
      XSSObject get_this();
      void      set_this(XSSObject this_);
      Language  get_language();
    public:
      variant resolve(const str& id, RESOLVE_ITEM item_type);
      variant resolve_path(const std::vector<str>& path);
    protected:
			XSSContext parent_;	
      Language   lang_;
      XSSObject  this_;
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
}


#endif
