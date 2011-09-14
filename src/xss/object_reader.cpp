
#include "xss/object_reader.h"
#include "xss/xss_context.h"
#include "xss/xss_error.h"

#include <boost/regex.hpp>


using namespace xkp;

const str SReaderError("reader");

const str SPropertyMustHaveName("Properties must have a name");
const str SPropertyMustHaveType("Properties must have a valid type");
const str SCannotParseXML("Invalid XML file");
const str STypeNotFound("Cannot find type");
const str SUnnamedArray("Arrays must have id");
const str SPropertyTypeMismatch("Property type does not its value");
const str SInvalidEnum("Invalid enum");

//xss_object_reader
xss_object_reader::xss_object_reader(XSSContext ctx):
  ctx_(ctx),
  enforce_types_(true)
  {
  }

XSSObject xss_object_reader::read(const str& xml)
	{
    parse_xml(xml);
    return read_object(doc_.RootElement(), XSSObject(), true);
	}

std::vector<XSSObject> xss_object_reader::read_array(const str& xml)
	{
    parse_xml(xml);

    return read_array(doc_.RootElement());
	}

void xss_object_reader::enforce_types(bool value)
  {
    enforce_types_ = value;
  }

void xss_object_reader::parse_xml(const str& xml)
  {
    doc_.Parse(xml.c_str());
    if (doc_.Error())
      {
				param_list error;
				error.add("id", SReaderError);
				error.add("desc", SCannotParseXML);
				error.add("error", str(doc_.ErrorDesc()));
				error.add("line", doc_.ErrorRow());
				error.add("column", doc_.ErrorCol());

				xss_throw(error);
      }
  }

XSSObject xss_object_reader::read_object(TiXmlElement* node, XSSObject parent, bool do_special, XSSType force_type)
  {
    XSSObject result(new xss_object());

    str class_name = node->Value();
    if (!do_special || !special_node(node, parent, result))
      {
        //read class
        const char* cc = node->Attribute("class");
        if (cc)
          class_name = cc;
        else
          class_name = node->Value();

        XSSType type = ctx_? ctx_->get_type(class_name) : XSSType();
        if (force_type)
          {
            type = force_type;
            class_name = force_type->output_id();
          }
        
        result->set_type(type);
        result->set_type_name(class_name);

        //read properties
        const TiXmlAttribute* attr = node->FirstAttribute();
	      while(attr)
	        {
            str attr_name  = attr->Name();
            str attr_value = attr->Value();

            if (attr_name == "class")
              {
                //do nothing
              }
            else if (attr_name == "id")
              result->set_id(attr_value);
            else if (attr_name == "output_id")
              result->set_output_id(attr_value);
            else
              {
                if (type)
                  {
                    //if the type already contains this attribute's name
                    //we'll simply assign its value, creating a property in the process
                    XSSProperty prop = type->get_property(attr_name);
                    if (prop)
                      {
                        XSSProperty new_prop(new xss_property(prop->id(), prop->type(), variant(), prop->get, prop->set, result));
                        new_prop->copy(XSSObject(prop)); //sort of inefficient, but safe & consistent
                        
                        XSSType prop_type = prop->type();
                        if (prop_type->is_enum())
                          {
                            XSSProperty enum_prop = prop_type->get_property(attr_value);
                            if (!enum_prop)
                              {
				                        param_list error;
				                        error.add("id", SReaderError);
				                        error.add("desc", SInvalidEnum);
				                        error.add("enum type", prop_type->id());
				                        error.add("value", attr_value);

				                        xss_throw(error);
                              }

                            Language lang     = ctx_->get_language();
                            str      expr_str = prop_type->id() + lang->resolve_separator() + attr_value;
                            
                            expression expr;
                            xs_utils   xs; 
                            xs.compile_expression(expr_str, expr);

                            new_prop->value_ = lang->compile_expression(expr, ctx_);
                          }
                        else
                          new_prop->value_ = attribute_value(attr);

                        result->properties()->push_back(new_prop);
                      }
                  }
                
                //add the value anyway
                result->add_property(attr_name, attribute_value(attr));
              }

	          attr = attr->Next();
	        }

        //children
        TiXmlElement* child_node  = node->FirstChildElement();
        XSSType       result_type = result->type();
        while(child_node)
          {
            //special case where a child is a property of the object's type
            bool handled = false;
            if (result_type)
              {
                XSSProperty type_prop = result_type->get_property(child_node->Value());
                if (type_prop)
                  {
                    handled = true;
                    XSSObject child = read_object(child_node, result, true, type_prop->type());
                    
                    XSSProperty new_prop(new xss_property);
                    new_prop->copy(XSSObject(type_prop));
                    new_prop->value_ = child;

                    result->properties()->push_back(new_prop);
                  }
              }

            if (!handled)
              {
                XSSObject child = read_object(child_node, result, true);
                result->add_child( child );
              }

            child_node = child_node->NextSiblingElement();
          }
      }

    return result;
  }

XSSObjectList xss_object_reader::read_array(TiXmlElement* node)
  {
    XSSObjectList result;

    TiXmlElement* child = node->FirstChildElement();
    while(child)
      {
        XSSObject child_obj = read_object(child, XSSObject(), true);
        result.push_back(child_obj);

        child = child->NextSiblingElement();
      }

    return result;
  }

variant xss_object_reader::attribute_value(const TiXmlAttribute* attr)
  {
    str value(attr->Value());

    static const boost::regex rfloat("^\\d*\\.\\d*$");
    if (boost::regex_match(value, rfloat))    
      {
        double dv;
        attr->QueryDoubleValue(&dv);
        return (float)dv;
      }

    static const boost::regex rint("^\\d*$");
    if (boost::regex_match(value, rint))    
      {
		    int iv;
        if (attr->QueryIntValue(&iv) == TIXML_SUCCESS)
          return iv;
      }
      
		if (value == "true")
			return true;
		else if (value == "false")
			return false;
		
		return value;
  }

bool xss_object_reader::special_node(TiXmlElement* node, XSSObject parent, XSSObject& result)
  {
    str class_name = node->Value();
    
    const char* ta = node->Attribute("type");
    str type(ta? ta : "");

    if (class_name == "property")
      return read_property(node, type, parent, result);
    else if (class_name == "method")
      return read_method(node, type, parent, result);
    else if (class_name == "event")
      return read_event(node, type, parent, result);
    else if (class_name == "array")
      return read_array(node, type, parent, result);

    return false;
  }

bool xss_object_reader::read_property(TiXmlElement* node, const str& type_name, XSSObject parent, XSSObject& result)
  {
    variant value;
    bool is_array = type_name == "array";
    if (is_array)
      {
        XSSObjectList children = read_array(node);
            
        //convert to vm friendly
        DynamicArray array_value(new dynamic_array);
        XSSObjectList::iterator it = children.begin();
        XSSObjectList::iterator nd = children.end();

        for(; it != nd; it++)
          {
            array_value->push_back(*it);
          }

        value = array_value;
      }
    else
      {
        //check for known values
        const TiXmlAttribute* attr = node->FirstAttribute();
	      while(attr)
	        {
	          if (attr->Name() == "value")
              {
                value = attribute_value(attr);
                break;
              }
            
            attr = attr->Next();
          }
      }
        
    str name = node->Attribute("id")? str(node->Attribute("id")) : str();
    if (name.empty())
      {
				param_list error;
				error.add("id", SReaderError);
				error.add("desc", SPropertyMustHaveName);
				error.add("container", parent->id());

				xss_throw(error);
      }

    XSSType type;
    if (type_name.empty())
      {
        if (!value.empty())
          {
            type = ctx_->get_type(value.get_schema());
          }
      }
    else
      {
        type = ctx_->get_type(type_name);
        if (!value.empty())
          {
            XSSType vtype = ctx_->get_type(value.get_schema());
            if (type != vtype)
              {
				        param_list error;
				        error.add("id", SReaderError);
				        error.add("desc", SPropertyTypeMismatch);
				        error.add("property name", name);
                if (parent) error.add("container", parent->id());
                if (type)   error.add("declared type", type->id());
                if (vtype)  error.add("value type", vtype->id());
				        xss_throw(error);
              }
          }
      }

    if (!type)
      {
        if (type_name.empty() || enforce_types_)
          {
				    param_list error;
				    error.add("id", SReaderError);
				    error.add("desc", SPropertyMustHaveType);
				    error.add("property name", name);
				    error.add("container", parent->id());
				    xss_throw(error);
          }
        
        //create an unresolved type
        XSSType unresolved(new xss_type);
        unresolved->set_id(type_name);
        unresolved->as_unresolved();

        type = unresolved;
      }

    XSSProperty prop(new xss_property(name, type, value, parent)); 

    //grab the rest of the stuff
    XSSObject surrogate = read_object(node, XSSObject(), false);
    prop->copy(surrogate);

    parent->properties()->push_back(prop); //td: check for existing properties

    return true;
  }

bool xss_object_reader::read_method(TiXmlElement* node, const str& type, XSSObject parent, XSSObject& result)
  {
    XSSMethod mthd(new xss_method);
    XSSObject surrogate = read_object(node, XSSObject(), false);
    mthd->copy(surrogate);

    parent->methods()->push_back(mthd); //td: check for existing methods
    return true;
  }

bool xss_object_reader::read_event(TiXmlElement* node, const str& type, XSSObject parent, XSSObject& result)
  {
    XSSEvent ev(new xss_event);
    XSSObject surrogate = read_object(node, XSSObject(), false);
    ev->copy(surrogate);

    parent->events()->push_back(ev); //td: check for existing events
    return true;
  }

bool xss_object_reader::read_array(TiXmlElement* node, const str& type, XSSObject parent, XSSObject& unused)
  { 
    const char* idd = node->Attribute("id");
    if (!idd)
      {
				param_list error;
				error.add("id", SReaderError);
				error.add("desc", SUnnamedArray);

				xss_throw(error);
      }

    str           id(idd);
    DynamicArray  result(new dynamic_array);
    XSSObjectList aa = read_array(node);

    XSSObjectList::iterator it = aa.begin();
    XSSObjectList::iterator nd = aa.end();

    for(; it != nd; it++)
      result->push_back(*it);

    parent->add_property(id, result);
    return true;
  }

