#ifndef XS_CONTEXT_HH
#define XS_CONTEXT_HH

#include <xs.h>
#include <xs/array.h>

namespace xkp {

//forwards
class   xss_property;
struct  xss_code_context;
  
typedef reference<xss_property>     XSSProperty;
typedef reference<xss_code_context> XSSContext;

//the idiom interface, under designed
struct xss_idiom
  {
    virtual void    set_context(XSSContext ctx)                                                = 0;
    virtual variant process_method(DynamicObject instance, xs_method& mthd)                    = 0; 
    virtual variant process_event(DynamicObject instance, const str& event_name, xs_event& ev) = 0;
    virtual variant process_code(code& cde, DynamicObject this_)                               = 0;
    virtual variant process_expression(expression expr, DynamicObject this_)                   = 0;
    virtual str     resolve_this(XSSContext ctx)                                               = 0;
  };

struct xss_code_context : base_code_context
  {
    xss_code_context(const variant project, xss_idiom* idiom);
    xss_code_context(xss_code_context& other);

    //this will function as resolver
    virtual XSSProperty   get_property(const str& name);
    virtual XSSProperty   get_property(DynamicObject obj, const str& name);
    virtual DynamicObject resolve_instance(const str& id);
    virtual variant       evaluate_property(DynamicObject obj, const str& name);
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
    virtual XSSProperty   get_property(DynamicObject obj, const str& name);
    virtual DynamicObject resolve_instance(const str& id);
    virtual variant       evaluate_property(DynamicObject obj, const str& name);
    private:
      XSSContext ctx_;
  };

//these are basically copies of their xs counterpart, but offer xss stuff, like generating
//they are also vm friendly, unlike the low level xs's ast.  
struct xss_property : public sponge_object
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
    variant       value_;
    
    str     generate_value();
    variant get_value();
  };
  
struct xss_event
  {
    xss_event();
    xss_event(const xss_event& other);

    str          name;
    DynamicArray impls;
  };
  
typedef reference<xss_event> XSSEvent;
}


#endif