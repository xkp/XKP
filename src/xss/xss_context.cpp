
#include "xss/xss_context.h"
#include "xss/xss_error.h"

using namespace xkp;

const str STypeMismatch("type-mismatch");
const str SContextError("context");

const str SCannotOverrideTypes("Cannot override types");
const str SDupType("Duplicate type");
const str SContextHasNoPath("Requesting path on a context without it");
const str SDuplicateSymbol("A symbol with this name already exists");
const str SEmptyArrayType("Arrays must have an inner type");

str xss_utils::var_to_string(variant& v)
  {
    if (v.is<str>())
      return variant_cast<str>(v, str());

    if (v.is<bool>())
      {
        bool vv = v;
        if (vv)
          return "true";
        else
          return "false";
      }
    
    return variant_cast<str>(v, str());
  }

//xss_context_scope
xss_context_scope::xss_context_scope(XSSContext owner):
  owner_(owner)
  {
  }

void xss_context_scope::set(XSSContext owner)
  {
    owner_ = owner;
  }

void xss_context_scope::register_symbol(const str& name, variant value)
  {
    assert(false); //td: why would this be called?
  }

bool xss_context_scope::resolve(const str& name, variant& result)
  {
    if (name == "#context")
      {
        result = owner_;
        return true;
      }

    result = owner_->resolve(name);
    return !result.empty();
  }

//xss_context
xss_context::xss_context(XSSContext parent, fs::path path):
  parent_(parent),
  path_(path),
  code_scope_(),
  got_dsls_(false)
  {
  }

XSSType xss_context::get_type(const str& type)
  {
    type_list::iterator it = types_.find(type);
    if (it != types_.end())
      return it->second;

    if (parent_)
      return parent_->get_type(type);

    return XSSType();
  }

XSSType xss_context::get_type(schema* type)
  {
    type_list::iterator it = types_.begin();
    type_list::iterator nd = types_.end();

    for(; it != nd; it++)
      {
        if (it->second->native_type() == type)
          return it->second;
      }

    if (parent_)
      return parent_->get_type(type);

    return XSSType();
  }

XSSType xss_context::get_array_type(XSSType type)
  {
    str type_name = type->id();
    if (type_name.empty())
      {
        param_list error;
        error.add("id", SContextError);
        error.add("desc", SEmptyArrayType);
        xss_throw(error);
      }

    str array_type_name = "array<" + type_name + ">"; //im gonna go basic on this one

    type_list::iterator it = types_.find(array_type_name);
    if (it != types_.end())
      {
        return it->second;
      }

    //create it
    XSSType new_type(new xss_type);
    new_type->set_id(array_type_name);
    new_type->as_array(type);

    return new_type;
  }

XSSType xss_context::add_type(const str& id, XSSType type, bool override_parent)
  {
    if (override_parent)
      {
        type_list::iterator it = types_.find(id);
        if (it != types_.end())
          {
            param_list error;
            error.add("id", SContextError);
            error.add("desc", SDupType);
            error.add("type", id);
            xss_throw(error);
          }
      }
    else
      {
        XSSType already = get_type(id);
        if (already)
          {
            param_list error;
            error.add("id", SContextError);
            error.add("desc", SDupType);
            error.add("type", id);
            xss_throw(error);
          }
      }

    types_.insert(type_list_pair(id, type));
    return type;
  }

XSSObject xss_context::get_this()
  {
    return this_;
  }

void xss_context::set_this(XSSObject _this_)
  {
    this_ = _this_;
  }

Language xss_context::get_language()
  {
    if (lang_)
      return lang_;

    if (parent_)
      return parent_->get_language();

    return Language();
  }

void xss_context::set_language(Language lang)
  {
    lang_ = lang;
  }

code_context xss_context::get_compile_context()
  {
    code_scope_.set(shared_from_this());
    collect_dsl();
    
    code_context result;
    result.types_  = &code_types_;
    result.scope_  = &code_scope_;
    result.dsl_    = &dsls_; 
    result.args_   = &args_; 

    return result;
  }

fs::path xss_context::path()
  {
    if (!path_.empty())
      return path_;

    if (parent_)
      return parent_->path();

    param_list error;
    error.add("id", SContextError);
    error.add("desc", SContextHasNoPath);
    xss_throw(error);
    
    return fs::path();
  }

void xss_context::register_dsl(const str& id, DslLinker dsl)
  {
    dsls_.insert(dsl_list_pair(id, dsl));
  }

void xss_context::add_parameter(const str& id, XSSType type)
  {
    args_.add(id, variant()); //td: !!! parameter types
  }

variant xss_context::resolve(const str& id, RESOLVE_ITEM item_type)
  {
    resolve_info si;
    si.what = item_type;
    if (resolve(id, si))
      return si.value;

    return empty_type_value(item_type);
  }

variant xss_context::resolve(const str& id, XSSObject instance, RESOLVE_ITEM item_type)
  {
    resolve_info left;
    left.what  = RESOLVE_INSTANCE;
    left.value = instance;
    left.type  = instance->type();

    resolve_info si;
    si.what  = item_type;
    si.left = &left; 

    if (resolve(id, si))
      return si.value;

    return empty_type_value(item_type);
  }

bool xss_context::resolve(const str& id, resolve_info& info)
  {
    //resolve types
    bool search_types = info.what == RESOLVE_TYPE;
    if (search_types)
      {
        assert(!info.left);
      }

    if (!search_types && info.what == RESOLVE_ANY)
      {
        search_types = true;
      }
    
    if (search_types)
      {
        type_list::iterator it = types_.find(id);
        if (it != types_.end())
          {
            info.what  = RESOLVE_TYPE;
            info.value = it->second;
            info.type  = it->second;
            return true;
          }
      }
    
    //search symbols
    if (!find_symbol(id, info))
      {
        //daddy!
        return parent_? parent_->resolve(id, info) : false;
      }

    switch(info.what)
      {
        case RESOLVE_INSTANCE:
          {
            XSSObject obj = info.value;
            info.type = obj->type();
            break;
          }
        case RESOLVE_METHOD:
          {
            XSSMethod mthd = info.value;
            info.type = mthd->type;
            break;
          }
        case RESOLVE_PROPERTY:
          {
            XSSProperty prop = info.value;
            info.type = prop->type;
            break;
          }
        
        case RESOLVE_VARIABLE:
          {
            XSSType type = info.value;
            info.type = type;
            break;
          }

        case RESOLVE_CHILD:
          {
            XSSObject obj = this_;
            if (info.left)
              {
                assert(info.what == RESOLVE_INSTANCE);
                obj = info.left->value;
              }

            XSSObject child = obj->find(id);
            if (!child)
              return false;

            info.what  = RESOLVE_INSTANCE;
            info.type  = child->type();
            info.value = child;
            break;
          }

        case RESOLVE_ANY:
        case RESOLVE_NATIVE:
        case RESOLVE_CONST:
          //all done
          break;
      }

    return true;
  }

variant xss_context::resolve_path(const std::vector<str>& path)
  {
    assert(false);
    return variant();
  }

void xss_context::register_symbol(RESOLVE_ITEM type, const str& id, variant symbol)
  {
      symbol_list::iterator it = symbols_.find(id);
      if (it != symbols_.end())
        {
          param_list error;
          error.add("id", SContextError);
          error.add("desc", SDuplicateSymbol);
          error.add("symbol", id);
          xss_throw(error);
        }

      symbols_.insert(symbol_list_pair(id, symbol_data(type, symbol)));
  }

variant xss_context::empty_type_value(RESOLVE_ITEM item_type)
  {
    switch (item_type)
      {
        case RESOLVE_INSTANCE:  return XSSObject();
        case RESOLVE_METHOD:    return XSSMethod();
        case RESOLVE_PROPERTY:  return XSSProperty();
        case RESOLVE_VARIABLE:  return XSSType();
        case RESOLVE_TYPE:      return XSSType();
        case RESOLVE_CHILD:     return XSSObject();
      }
    return variant();
  }

bool xss_context::find_symbol(const str& id, resolve_info& info)
  {
    symbol_list::iterator it = symbols_.find(id);
    if (it != symbols_.end())
      {
        info.what  = it->second.type;
        info.value = it->second.value;
        return true;
      }

      return false;
  }

void xss_context::collect_dsl()
  {
    if (got_dsls_)
      return;

    if (parent_)
      {
        //aint the most efficient way
        code_context pctx = parent_->get_compile_context();
        dsl_list::iterator it = pctx.dsl_->begin();        
        dsl_list::iterator nd = pctx.dsl_->end();        

        for(; it != nd; it++)
          {
            dsl_list::iterator mine = dsls_.find(it->first);
            if (mine == dsls_.end())
              dsls_.insert(dsl_list_pair(it->first, it->second));
          }
      }

      got_dsls_ = true;
  }

//xss_object
xss_object::xss_object():
	children_(new dynamic_array),
	properties_(new dynamic_array),
	methods_(new dynamic_array),
	events_(new dynamic_array)
  {
  }

void xss_object::copy(XSSObject obj)
  {
    if (id_.empty())
      id_ = obj->id();

    if (type_name_.empty())
      type_name_ = obj->type_name();

    if (!type_)
      type_ = obj->type();
    
    //type information will not be copied, should it?
		DynamicArray obj_children = obj->children();
		std::vector<variant>::iterator it = obj_children->ref_begin();
		std::vector<variant>::iterator nd = obj_children->ref_end();

    for(; it != nd; it++)
      {
        XSSObject obj_child = *it;
        XSSObject my_child = find(obj_child->id());
        if (!my_child)
          my_child = XSSObject(new xss_object);
        
        my_child->copy(obj_child); 

        children_->push_back(my_child);
      }

		DynamicArray obj_properties = obj->properties();
		it = obj_properties->ref_begin();
		nd = obj_properties->ref_end();

    for(; it != nd; it++)
      {
        XSSProperty obj_prop = *it;
        
        XSSProperty my_prop = get_property(obj_prop->id());
        if (!my_prop)
          my_prop = XSSProperty(new xss_property);
        
        my_prop->copy(XSSObject(obj_prop)); 

        properties_->push_back(my_prop);
      }

		DynamicArray obj_methods = obj->methods();
		it = obj_methods->ref_begin();
		nd = obj_methods->ref_end();

    for(; it != nd; it++)
      {
        XSSMethod obj_mthd = *it;
        
        XSSMethod my_methd = get_method(obj_mthd->id());
        if (!my_methd)
          my_methd = XSSMethod(new xss_method);
        
        my_methd->copy(XSSObject(obj_mthd)); 
        methods_->push_back(my_methd);
      }

		DynamicArray obj_events = obj->events();
		it = obj_events->ref_begin();
		nd = obj_events->ref_end();

    for(; it != nd; it++)
      {
        XSSEvent obj_ev = *it;
        XSSEvent my_ev = XSSEvent(new xss_event);
        
        my_ev->copy(XSSObject(obj_ev)); 
        events_->push_back(my_ev);
      }

      //copy dynamic variables
      struct copier : dynamic_visitor
        {
          copier(XSSObject src, xss_object* dest):
            src_(src), 
            dest_(dest)
            {
            }

          virtual void item(const str& name, variant value)
            {
              if (name == "type")
                return; //type is not copied, assumed to be set somewhere else,
                        //those kind of assumptions are bad 

              dynamic_set((IDynamicObject*)dest_, name, value);
            }

          private:
            XSSObject   src_;
            xss_object* dest_;
        } copier_(obj, this);

      obj->visit(&copier_);
  }

bool xss_object::resolve(const str& name, schema_item& result)
	{
		if (editable_object<xss_object>::resolve(name, result))
			return true;
    
    variant value;
    bool    read_only = false;
    if (type_ && type_->has(name))
      {
        value = dynamic_get(type_, name);
        read_only = true;
      }

		//sponge-like
		int idx      = add_anonymous(value);
		result.get   = Getter( new anonymous_getter(idx) );
		result.set   = !read_only? Setter( new anonymous_setter(idx) ) : Setter();
		result.flags = DYNAMIC_ACCESS;

		items_.insert( item_pair(name, result) );
		return true;
	}

str xss_object::id()
  {
    return id_;
  }

str xss_object::output_id()
  {
    return output_id_.empty()? id_ : output_id_;
  }

str xss_object::type_name()
  {
    return type_name_;
  }

XSSType	xss_object::type()
  {
    return type_;
  }

XSSObject	xss_object::parent()
  {
    return parent_;
  }

DynamicArray xss_object::children()
  {
    return children_;
  }

DynamicArray xss_object::properties()
  {
    return properties_;
  }

DynamicArray xss_object::methods()
  {
    return methods_;
  }

DynamicArray xss_object::events()
  {
    return events_;
  }

void xss_object::set_id(const str& id)
  {
    id_ = id;
  }

void xss_object::set_type_name(const str& id)
  {
    if (!type_name_.empty())
      {
        param_list error;
        error.add("id", STypeMismatch);
        error.add("desc", SCannotOverrideTypes);
        error.add("object", id_);
        error.add("old type", type_name_);
        error.add("new type", id);
        xss_throw(error);
      }

    if (type_ && type_->id() != id)
      {
        param_list error;
        error.add("id", STypeMismatch);
        error.add("desc", SCannotOverrideTypes);
        error.add("object", id_);
        error.add("old type", type_->id());
        error.add("new type", id);
        xss_throw(error);
      }

    type_name_ = id;
  }

void xss_object::set_type(XSSType type)
  {
    type_ = type;
  }

XSSObject xss_object::find(const str& what)
  {
    std::vector<variant>::iterator it = children_->ref_begin();
    std::vector<variant>::iterator nd = children_->ref_end();

    for(; it != nd; it++)
      {
        XSSObject child = *it;
        if (child->id() == what || child->type_name() == what)
          return child;
      }

    return XSSObject();
  }

std::vector<XSSObject> xss_object::find_by_class(const str& which)
  {
    std::vector<XSSObject> result;
    
    std::vector<variant>::iterator it = children_->ref_begin();
    std::vector<variant>::iterator nd = children_->ref_end();

    for(; it != nd; it++)
      {
        XSSObject child = *it;
        if (child->type_name() == which)
          result.push_back(child);
      }

    return result;
  }

DynamicArray xss_object::get_event_impl(const str& event_name, XSSEvent& ev)
  {
    for(size_t i = 0; i < events_->size(); i++)
      {
        ev = events_->at(i);
        if (ev->name == event_name)
          return ev->impls;
      }

    //not implemented, create
    ev = XSSEvent(new xss_event(event_name));
    ev->name = event_name;

    events_->push_back(ev);
    return ev->impls;
  }

void xss_object::add_child(XSSObject obj)
  {
    children_->push_back(obj);
  }

void xss_object::remove_child(XSSObject obj)
  {
    assert(false); //hate removing
  }

XSSProperty xss_object::get_property(const str& name)
  {
    std::vector<variant>::iterator it = properties_->ref_begin();
    std::vector<variant>::iterator nd = properties_->ref_end();

    for(; it != nd; it++)
      {
        XSSProperty prop = *it;
        if (prop->id() == name)
          return prop;
      }

    return XSSProperty();
  }


std::vector<XSSEvent> xss_object::get_events(const str& name)
  {
    std::vector<variant>::iterator it = events_->ref_begin();
    std::vector<variant>::iterator nd = events_->ref_end();

    std::vector<XSSEvent> result;
    for(; it != nd; it++)
      {
        XSSEvent ev = *it;
        if (ev->id() == name)
          result.push_back(ev);
      }

    return result;
  }

XSSMethod xss_object::get_method(const str& name)
  {
    std::vector<variant>::iterator it = properties_->ref_begin();
    std::vector<variant>::iterator nd = properties_->ref_end();

    for(; it != nd; it++)
      {
        XSSMethod mthd = *it;
        if (mthd->id() == name)
          return mthd;
      }

    return XSSMethod();
  }

//xss_type
xss_type::xss_type():
  xss_object(),
  xs_type_(null),
  is_enum_(false),
  is_array_(false),
  is_object_(true),
  is_variant_(false)
  {
    DYNAMIC_INHERITANCE(xss_type)
  }

xss_type::xss_type(schema* _xs_type):
  xss_object(),
  xs_type_(_xs_type),
  is_enum_(false),
  is_array_(false),
  is_object_(false),
  is_variant_(false)
  {
    DYNAMIC_INHERITANCE(xss_type)
  }

void xss_type::set_super(XSSType super)
  {
    super_ = super;
  }

void xss_type::set_definition(XSSObject def)
  {
    copy(def); 
  }

schema* xss_type::native_type()
  {
    return xs_type_;
  }

XSSType xss_type::array_type()
  {
    return array_type_;
  }

void xss_type::as_enum()
  {
    is_enum_ = true;
  }

void xss_type::as_array(XSSType type)
  {
    is_object_ = false;
    is_array_  = true;

    array_type_ = type;
  }

void xss_type::as_variant()
  {
    is_object_ = false;
    is_variant_ = true;
  }

bool xss_type::is_enum()
  {
    return is_enum_;
  }

bool xss_type::is_array()
  {
    return is_array_;
  }

bool xss_type::is_object()
  {
    return is_object_;
  }

bool xss_type::is_native()
  {
    return xs_type_ != null;
  }

bool xss_type::is_variant()
  {
    return is_variant_;
  }

//xss_property
xss_property::xss_property():
	flags(0)
  {
    DYNAMIC_INHERITANCE(xss_property)
  }

xss_property::xss_property(const xss_property& other):
	get(other.get),
	set(other.set),
	flags(other.flags),
	this_(other.this_),
	value_(other.value_),
	type(other.type)
  {
    DYNAMIC_INHERITANCE(xss_property)
	  id_ = other.id_;
  }

xss_property::xss_property(const str& _name, XSSType _type, variant _value, XSSObject _this_):
	this_(_this_),
	value_(_value),
	type(_type)
  {
    DYNAMIC_INHERITANCE(xss_property)
	  id_ = _name;
  }

xss_property::xss_property(const str& _name, XSSType _type, variant _value, variant _get, variant _set, XSSObject _this_):
	get(_get),
	set(_set),
	this_(_this_),
	value_(_value),
	type(_type)
  {
    DYNAMIC_INHERITANCE(xss_property)
	  id_ = _name;
  }

void xss_property::copy(XSSObject obj)
  {
    xss_property* prop = dynamic_cast<xss_property*>(obj.get());
    //XSSProperty prop = variant_cast<XSSProperty>(obj, XSSProperty());
    if (prop)
      {
        get = prop->get;
			  set = prop->set;
			  flags = prop->flags;
			  this_ = prop->this_;
			  value_ = prop->value_;
			  type = prop->type;
      }

    xss_object::copy(obj);
  }

str xss_property::render_value()
  {
    if (value_.empty())
      return "null"; //td: somehow the language must resolve this

    IExpressionRenderer* renderer = variant_cast<IExpressionRenderer*>(value_, null);
    if (renderer)
      return renderer->render();

    if (value_.is<str>())
      {
        str result = variant_cast<str>(value_, str());
        return '"' + result + '"';
      }

    return xss_utils::var_to_string(value_);
  }

//xss_event
xss_event::xss_event():
  impls(new dynamic_array)
  {
    DYNAMIC_INHERITANCE(xss_event)
  }

xss_event::xss_event(const xss_event& other):
  name(other.name),
  impls(other.impls),
  args(other.args)
  {
    DYNAMIC_INHERITANCE(xss_event)
  }

xss_event::xss_event(const str& _name):
  impls(new dynamic_array),
  name(_name)
  {
    DYNAMIC_INHERITANCE(xss_event)
  }

bool xss_event::implemented()
	{
		return impls->size() > 0;
	}

//xss_method
xss_method::xss_method()
  {
    DYNAMIC_INHERITANCE(xss_method)
  }

xss_method::xss_method(const xss_method& other):
	name(other.name),
	type(other.type),
	args(other.args),
  code(other.code)
  {
    DYNAMIC_INHERITANCE(xss_method)
  }

xss_method::xss_method(const str& _name, XSSType _type, variant _args, variant _code):
	name(_name),
	type(_type),
	args(_args),
  code(_code)
  {
    DYNAMIC_INHERITANCE(xss_method)
  }
