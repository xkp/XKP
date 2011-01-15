#ifndef XS_CONTEXT_HH
#define XS_CONTEXT_HH

#include <xs.h>
#include <xs/array.h>

namespace xkp {

//forwards
class		xss_object;
class   xss_property;
class   xss_event;
struct  xss_code_context;
  
typedef reference<xss_object>				XSSObject;
typedef reference<xss_property>     XSSProperty;
typedef reference<xss_code_context> XSSContext;
typedef reference<xss_event>				XSSEvent;

class xss_object : public editable_object<xss_object>,
									 public boost::enable_shared_from_this<xss_object>
	{
		public:
			xss_object();

		public:
			//accesors
			XSSObject type();
			void xss_type(XSSObject type);
			XSSObject parent();
			DynamicArray	children();
			DynamicArray	properties();
			DynamicArray	methods();
			DynamicArray	events();
		public:
			void copy(xss_object* other);
		public:
			XSSProperty get_property(const str& name);
			XSSEvent		get_event(const str& name);
		public:
			//misc, not neccesarily to be published
			void limbo_add(const str& id, variant value);
			void seal();
		public:
      //IXSSObject
      virtual bool resolve(const str& name, schema_item& result);
		public:
			XSSObject type_;
			XSSObject parent_;
			DynamicArray	children_;
			DynamicArray	properties_;
			DynamicArray	methods_;
			DynamicArray	events_;
			bool					sealed_;
	};

//the idiom interface, under designed
struct xss_idiom
  {
    virtual void    set_context(XSSContext ctx)                                                = 0;
    virtual variant process_method(XSSObject instance, xs_method& mthd)                    = 0; 
    virtual variant process_event(XSSObject instance, const str& event_name, xs_event& ev) = 0;
    virtual variant process_code(code& cde, XSSObject this_)                               = 0;
    virtual variant process_expression(expression expr, XSSObject this_)                   = 0;
    virtual str     resolve_this(XSSContext ctx)                                               = 0;
  };

struct xss_code_context : base_code_context
  {
    xss_code_context(const variant project, xss_idiom* idiom);
    xss_code_context(xss_code_context& other);

    //this will function as resolver
    virtual XSSProperty   get_property(const str& name);
    virtual XSSProperty   get_property(XSSObject obj, const str& name);
    virtual XSSObject			resolve_instance(const str& id);
    virtual variant       evaluate_property(XSSObject obj, const str& name);
		virtual schema*				get_type(const str& name);
		virtual str						get_type_name(schema* type);
		
    public:
      //td: ugles, this is the way it is to circunvent c++ and its dependencies
      variant    project_;
      xss_idiom* idiom_;
  };
  
struct xss_composite_context : xss_code_context
  {
    xss_composite_context(XSSContext ctx);
      
    //this will function as resolver
    virtual XSSProperty   get_property(const str& name);
    virtual XSSProperty   get_property(XSSObject obj, const str& name);
    virtual XSSObject			resolve_instance(const str& id);
    virtual variant       evaluate_property(XSSObject obj, const str& name);
    private:
      XSSContext ctx_;
  };

//these are basically copies of their xs counterpart, but offer xss stuff, like generating
//they are also vm friendly, unlike the low level xs's ast.  
class xss_property : public xss_object
  {
		public:
			xss_property();
			xss_property(const xss_property& other);
			xss_property(const str& name, const str& type, variant value, XSSObject _this_);
			xss_property(const str& name, const str& type, variant value, variant _get, variant _set, XSSObject _this_);

			str       name;
			variant   get;
			variant   set;
			size_t    flags;
			XSSObject this_;
			variant   value_;
			str				type;
    
			str     generate_value();
			variant get_value();
			str			resolve_assign(const str& value);
  };
  
class xss_event : public xss_object
  {
		public:
			xss_event();
			xss_event(const xss_event& other);

			str          name;
			DynamicArray impls;

			bool implemented()
				{
					return impls->size() > 0;
				}
  };

}


#endif