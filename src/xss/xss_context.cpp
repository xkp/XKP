
#include "xss/xss_context.h"
#include "xss/xss_error.h"

using namespace xkp;

const str STypeMismatch("type-mismatch");
const str SContextError("context");

const str SCannotOverrideTypes("Cannot override types");
const str SDupType("Duplicate type");

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

//xss_context
xss_context::xss_context(XSSContext parent):
  parent_(parent)
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

void xss_context::add_type(const str& id, XSSType type, bool override_parent)
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
    assert(false); //td:

    code_context result;
    result.types_;
    result.scope_;
    result.args_;
    result.this_type;
    result.this_;
    result.dsl_; 

    return result;
  }

fs::path xss_context::path()
  {
    assert(false); //td:

    return fs::path();
  }

variant xss_context::resolve(const str& id, RESOLVE_ITEM item_type)
  {
    assert(false);
    return variant();
  }

variant xss_context::resolve_path(const std::vector<str>& path)
  {
    assert(false);
    return variant();
  }

void xss_context::register_symbol(RESOLVE_ITEM type, const str& id, variant symbol)
  {
  }

//xss_object
xss_object::xss_object():
	children_(new dynamic_array),
	properties_(new dynamic_array),
	methods_(new dynamic_array),
	events_(new dynamic_array)
  {
  }

bool xss_object::resolve(const str& name, schema_item& result)
	{
		if (editable_object<xss_object>::resolve(name, result))
			return true;

		//we no have it, lets surrogate
    XSSObjectList::iterator it = surrogates_.begin();
    XSSObjectList::iterator nd = surrogates_.end();

    for(; it != nd; it++)
      {
        XSSObject s = *it;
        if (s->has(name))
          return s->resolve(name, result);
      }

		//otherwise behave just like the old sponge
		int idx      = add_anonymous(variant());
		result.get   = Getter( new anonymous_getter(idx) );
		result.set   = Setter( new anonymous_setter(idx) );
		result.flags = DYNAMIC_ACCESS;

		items_.insert( item_pair(name, result) );
		return true;
	}

str xss_object::id()
  {
    return id_;
  }

str xss_object::type_name()
  {
    return type_name_;
  }

XSSObject	xss_object::type()
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

void xss_object::set_type(XSSObject type)
  {
    type_ = type;
  }

XSSObject xss_object::find(const str& what)
  {
    std::vector<variant>::iterator it = children_->begin();
    std::vector<variant>::iterator nd = children_->end();

    for(; it != nd; it++)
      {
        XSSObject child = *it;
        if (child->id() == what)
          return child;
      }

    return XSSObject();
  }

std::vector<XSSObject> xss_object::find_by_class(const str& which)
  {
    std::vector<XSSObject> result;
    
    std::vector<variant>::iterator it = children_->begin();
    std::vector<variant>::iterator nd = children_->end();

    for(; it != nd; it++)
      {
        XSSObject child = *it;
        if (child->type_name() == which)
          result.push_back(child);
      }

    return result;
  }

void xss_object::add_surrogate(XSSObject s)
  {
    surrogates_.push_back(s);
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
    std::vector<variant>::iterator it = properties_->begin();
    std::vector<variant>::iterator nd = properties_->end();

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
    std::vector<variant>::iterator it = events_->begin();
    std::vector<variant>::iterator nd = events_->end();

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
    std::vector<variant>::iterator it = properties_->begin();
    std::vector<variant>::iterator nd = properties_->end();

    for(; it != nd; it++)
      {
        XSSMethod mthd = *it;
        if (mthd->id() == name)
          return mthd;
      }

    return XSSMethod();
  }

//xss_type
void xss_type::set_super(XSSType super)
  {
    super_ = super;
  }

void xss_type::set_definition(XSSObject def)
  {
    add_surrogate(def);
    //td: resolve property sharing
  }

//xss_property
xss_property::xss_property():
	flags(0)
  {
  }

xss_property::xss_property(const xss_property& other):
	name(other.name),
	get(other.get),
	set(other.set),
	flags(other.flags),
	this_(other.this_),
	value_(other.value_),
	type(other.type)
  {
  }

xss_property::xss_property(const str& _name, XSSType _type, variant _value, XSSObject _this_):
	name(_name),
	this_(_this_),
	value_(_value),
	type(_type)
  {
  }

xss_property::xss_property(const str& _name, XSSType _type, variant _value, variant _get, variant _set, XSSObject _this_):
	name(_name),
	get(_get),
	set(_set),
	this_(_this_),
	value_(_value),
	type(_type)
  {
  }

//xss_event
xss_event::xss_event()
  {
  }

xss_event::xss_event(const xss_event& other):
  name(other.name),
  impls(other.impls),
  args(other.args)
  {
  }

xss_event::xss_event(const str& _name):
  name(_name)
  {
  }

bool xss_event::implemented()
	{
		return impls->size() > 0;
	}

//xss_method
xss_method::xss_method()
  {
  }

xss_method::xss_method(const xss_method& other):
	name(other.name),
	type(other.type),
	args(other.args),
  code(other.code)
  {
  }

xss_method::xss_method(const str& _name, XSSType _type, variant _args, variant _code):
	name(_name),
	type(_type),
	args(_args),
  code(_code)
  {
  }
