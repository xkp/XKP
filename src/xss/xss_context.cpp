
#include "xss/xss_context.h"

using namespace xkp;

//xss_object
xss_object::xss_object()
  {
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

