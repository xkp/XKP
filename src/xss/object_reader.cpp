
#include "xss/object_reader.h"
#include "xss/xss_context.h"
#include "xss/xss_error.h"

#include <boost/regex.hpp>

#include <json/writer.h>
#include <json/reader.h>

using namespace xkp;

const str SReaderError("reader");

const str SPropertyMustHaveName("Properties must have a name");
const str SPropertyMustHaveType("Properties must have a valid type");
const str SMethodMustHaveReturnType("Methods must have a valid return type");
const str SCannotParseXML("Invalid XML file");
const str SCannotParseDef("Invalid Data file, make sure to provide a XML or json file");
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

XSSObject xss_object_reader::read(const str& text)
	{
    if (parse_xml(text))
      return read_xml_object(doc_.RootElement(), XSSObject(), true);
    else
      {
        //td: finish json
		    param_list error;
		    error.add("id", SReaderError);
		    error.add("desc", SCannotParseXML);
		    error.add("xml error", str(doc_.ErrorDesc()));
		    error.add("line", doc_.ErrorRow());
		    error.add("column", doc_.ErrorCol());

		    xss_throw(error);
      }

    if (parse_json(text))
      return read_json_object(doc_.RootElement(), XSSObject(), true);

    return XSSObject();
	}

std::vector<XSSObject> xss_object_reader::read_array(const str& text)
	{
    parse_xml(text);

    return read_xml_array(doc_.RootElement());
	}

void xss_object_reader::enforce_types(bool value)
  {
    enforce_types_ = value;
  }

void xss_object_reader::process_property(XSSObject result, XSSType type, const str& attr_name, variant attr_value)
  {
    variant value = attr_value;

    //if the type already contains this attribute's name
    //we'll simply assign its value, creating a property in the process
    if (type)
      {
        XSSProperty prop = type->get_property(attr_name);
        XSSType     prop_type;
        if (prop)
          {
            prop_type = prop->type();
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

                Language lang      = ctx_->get_language();
                str      value_str =  variant_cast<str>(attr_value, str());
                str      expr_str  = prop_type->id() + lang->resolve_separator() + value_str;
                          
                expression expr;
                xs_utils   xs; 
                xs.compile_expression(expr_str, expr);

                value = lang->compile_expression(expr, ctx_);
              }

            if (!prop_type)
              prop_type = ctx_->get_type(value.get_schema());
                      
            result->add_property(attr_name, value, prop_type); //td: type checking
          }
      }
                
    //add the value anyway
    result->add_attribute(attr_name, value);
  }

void xss_object_reader::process_child(XSSObject child)
  {
  }

bool xss_object_reader::parse_xml(const str& xml)
  {
    doc_.Parse(xml.c_str());
    return !doc_.Error();
  }

XSSObject xss_object_reader::read_xml_object(TiXmlElement* node, XSSObject parent, bool do_special, XSSType force_type)
  {
    XSSObject result(new xss_object());

    str class_name = node->Value();
    if (!do_special || !xml_special_node(node, parent, result))
      {
        //read class
        const char* cc = node->Attribute("class");
        if (cc)
          class_name = cc;
        else
          class_name = node->Value();

        XSSType type = ctx_? ctx_->get_type(class_name) : XSSType();

        if (!type && class_name == "class_name")
          {
            //when reading types, look for the super class as "type"
            const char* cc = node->Attribute("super");
            if (cc)
              {
                type = ctx_? ctx_->get_type(cc) : XSSType();
              }
          }


        if (force_type)
          {
            type = force_type;
            class_name = force_type->output_id();
          }
        
        result->set_type(type);
        result->set_type_name(type? type->id() : class_name);

        //read properties
        const TiXmlAttribute* attr = node->FirstAttribute();
	      while(attr)
	        {
            str attr_name  = attr->Name();
            str attr_value = attr->Value();

            if (attr_name == "class_name")
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
                        XSSType prop_type = prop->type();
                        variant value;
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

                            value = lang->compile_expression(expr, ctx_);
                          }
                        else
                          value = xml_attribute_value(attr);

                        
                        result->add_property(attr_name, value, prop_type); //td: type checking
                      }
                  }
                
                //add the value anyway
                result->add_attribute(attr_name, xml_attribute_value(attr));
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
                    XSSObject child = read_xml_object(child_node, result, true, type_prop->type());
                    
                    XSSProperty new_prop(new xss_property);
                    new_prop->copy(XSSObject(type_prop));
                    new_prop->value_ = child;

                    result->properties()->push_back(new_prop);
                  }
              }

            if (!handled)
              {
                XSSObject child = read_xml_object(child_node, result, true);
                if (child)
                  result->add_child( child );
              }

            child_node = child_node->NextSiblingElement();
          }
      }

    return result;
  }

XSSObjectList xss_object_reader::read_xml_array(TiXmlElement* node)
  {
    XSSObjectList result;

    TiXmlElement* child = node->FirstChildElement();
    while(child)
      {
        XSSObject child_obj = read_xml_object(child, XSSObject(), true);
        result.push_back(child_obj);

        child = child->NextSiblingElement();
      }

    return result;
  }

variant xss_object_reader::xml_attribute_value(const TiXmlAttribute* attr)
  {
    str value(attr->Value());
    static const boost::regex rfloat("^[+-]?\\d*\\.\\d*$");
    if (boost::regex_match(value, rfloat))    
      {
        double dv;
        attr->QueryDoubleValue(&dv);
        return (float)dv;
      }

    //static const boost::regex rint("^\\d*$");
    static const boost::regex rint("^[+-]?\\d+$");
    
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

bool xss_object_reader::xml_special_node(TiXmlElement* node, XSSObject parent, XSSObject& result)
  {
    str class_name = node->Value();
    
    const char* ta = node->Attribute("type");
    str type(ta? ta : "");

    if (class_name == "property")
      return read_xml_property(node, type, parent, result);
    else if (class_name == "method")
      return read_xml_method(node, type, parent, result);
    else if (class_name == "event")
      return read_xml_event(node, type, parent, result);
    else if (class_name == "array")
      return read_xml_array(node, type, parent, result);
    else if (class_name == "html")
      {
        std::ostringstream ss;
        ss << *node;
        str res = ss.str();
        res.erase(res.begin(), res.begin() + str("<html>").size());
        res.erase(res.begin() + res.size() - str("</html>").size(), res.end());
        
        result = XSSObject(new xss_object());
        result->set_type_name("html");
        result->add_attribute("text", res);
        return true;
      }

    return false;
  }

bool xss_object_reader::read_xml_property(TiXmlElement* node, const str& type_name, XSSObject parent, XSSObject& result)
  {
    result = XSSObject();

    variant value;
    bool is_array = type_name == "array";
    if (is_array)
      {
        XSSObjectList children = read_xml_array(node);
            
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
                value = xml_attribute_value(attr);
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
    XSSObject surrogate = read_xml_object(node, XSSObject(), false);
    prop->copy(surrogate);

    parent->add_property_(prop); 
    return true;
  }

bool xss_object_reader::read_xml_method(TiXmlElement* node, const str& type, XSSObject parent, XSSObject& result)
  {
    result = XSSObject();

    XSSMethod mthd(new xss_method);
    XSSObject surrogate = read_xml_object(node, XSSObject(), false);
    mthd->copy(surrogate);

    XSSType ret_type = ctx_->get_type(type);

    if (!ret_type)
      {
        if (type.empty())
          {
				    param_list error;
				    error.add("id", SReaderError);
				    error.add("desc", SMethodMustHaveReturnType);
            error.add("method name", mthd->id());
				    error.add("container", parent->id());

				    //xss_throw(error);
          }
        else
          {
            //create an unresolved type
            XSSType unresolved(new xss_type);
            unresolved->set_id(type);
            unresolved->as_unresolved();

            ret_type = unresolved;
          }
      }

    mthd->set_type(ret_type);

    parent->methods()->push_back(mthd); //td: check for existing methods
    return true;
  }

bool xss_object_reader::read_xml_event(TiXmlElement* node, const str& type, XSSObject parent, XSSObject& result)
  {
    result = XSSObject();

    XSSEvent ev(new xss_event);
    XSSObject surrogate = read_xml_object(node, XSSObject(), false);
    ev->copy(surrogate);

    parent->events()->push_back(ev); //td: check for existing events
    return true;
  }

bool xss_object_reader::read_xml_array(TiXmlElement* node, const str& type, XSSObject parent, XSSObject& result)
  { 
    result = XSSObject();

    const char* idd = node->Attribute("id");
    if (!idd)
      {
				param_list error;
				error.add("id", SReaderError);
				error.add("desc", SUnnamedArray);

				xss_throw(error);
      }

    str           id(idd);
    DynamicArray  my_result(new dynamic_array);
    XSSObjectList aa = read_xml_array(node);

    XSSObjectList::iterator it = aa.begin();
    XSSObjectList::iterator nd = aa.end();

    for(; it != nd; it++)
      my_result->push_back(*it);

    parent->add_attribute(id, my_result);
    return true;
  }

bool xss_object_reader::parse_json(const str& text)
  {
    Json::Reader r;
    return r.parse(text, json_, false);
  }

XSSObject xss_object_reader::read_json_object(Json::Value node, XSSObject parent, bool do_special, XSSType force_type)
  {
    XSSObject result(new xss_object());

    Json::Value jclass = node.get("class", Json::Value::null_value);
    str         class_name = jclass.asString();
    if (!do_special || !json_special_node(class_name, node, parent, result))
      {
        XSSType type = ctx_? ctx_->get_type(class_name) : XSSType();
        if (force_type)
          {
            type = force_type;
            class_name = force_type->output_id();
          }
        
        result->set_type(type);
        result->set_type_name(class_name);

        //read properties
        Json::Value::Members members = node.getMemberNames();
        std::vector<Json::Value> children;

        Json::Value::Members::iterator it = members.begin();
        Json::Value::Members::iterator nd = members.end();
        for(; it != nd; it++)
          {
            str attr_name = *it;
            Json::Value attr = node.get(attr_name, Json::Value::null_value);
            variant attr_value = json_attr_value(attr);

            if (attr.isObject())
              children.push_back(attr);
            else
              {
                if (attr_name == "class")
                  {
                    //do nothing
                  }
                else if (attr_name == "id")
                  result->set_id(attr.asString());
                else if (attr_name == "output_id")
                  result->set_output_id(attr.asString());
                else 
                  process_property(result, type, attr_name, attr_value);
              }
	        }

        //children
        std::vector<Json::Value>::iterator cit = children.begin();
        std::vector<Json::Value>::iterator cnd = children.end();

        XSSType result_type = result->type();
        for(; cit != cnd; cit++)
          {
            XSSObject child = read_json_object(*cit, result, true);
            process_child(child);

            //bool handled = false;
            //if (result_type)
            //  {
            //    XSSProperty type_prop = result_type->get_property(child_node->Value());
            //    if (type_prop)
            //      {
            //        handled = true;
            //        XSSObject child = read_xml_object(child_node, result, true, type_prop->type());
            //        
            //        XSSProperty new_prop(new xss_property);
            //        new_prop->copy(XSSObject(type_prop));
            //        new_prop->value_ = child;

            //        result->properties()->push_back(new_prop);
            //      }
            //  }

            //if (!handled)
            //  {
            //    XSSObject child = read_xml_object(child_node, result, true);
            //    if (child)
            //      result->add_child( child );
            //  }
          }

      }

    return result;
  }

bool xss_object_reader::json_special_node(const str& class_name, Json::Value node, XSSObject parent, XSSObject& result)
  {
    if (class_name == "property")
      return read_json_property(node, parent, result);
    else if (class_name == "method")
      return read_json_method(node, parent, result);
    else if (class_name == "event")
      return read_json_event(node, parent, result);

    return false;
  }

variant xss_object_reader::json_attr_value(Json::Value attr)
  {
    if (attr.isNull())
      return variant();
    else if (attr.isBool())
      return attr.asBool();
    else if (attr.isInt())
      return attr.asInt();
    else if (attr.isUInt())
      return attr.asInt(); //td: do uints
    else if (attr.isDouble())
      return (float)attr.asDouble(); //td: do doubles
    else if (attr.isString())
      return attr.asString(); 
    else if (attr.isArray())
      {
        assert(false); //punt
      }
    else
      {
        assert(attr.isObject()); 
      }
		
    assert(false); //as fas as I know it shouldn't get here
    return variant();
  }

bool xss_object_reader::read_json_property(Json::Value node, XSSObject parent, XSSObject& result)
  {
    result = XSSObject();

    Json::Value jid    = node.get("id",    Json::Value::null_value);
    Json::Value jvalue = node.get("value", Json::Value::null_value);
    Json::Value jtype  = node.get("type",  Json::Value::null_value);

    if (jid.empty())
      {
				param_list error;
				error.add("id", SReaderError);
				error.add("desc", SPropertyMustHaveName);
				error.add("container", parent->id());

				xss_throw(error);
      }

    XSSType type;
    variant value = json_attr_value(jvalue);
    str     name  = jid.asString();

    if (jtype.empty())
      {
        if (!jvalue.empty())
          {
            type = ctx_->get_type(value.get_schema());
          }
      }
    else
      {
        type = ctx_->get_type(jtype.asString());
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
        if (jtype.empty() || enforce_types_)
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
        unresolved->set_id(jtype.asString());
        unresolved->as_unresolved();

        type = unresolved;
      }

    XSSProperty prop(new xss_property(name, type, value, parent)); 

    //grab the rest of the stuff
    XSSObject surrogate = read_json_object(node, XSSObject(), false);
    prop->copy(surrogate);

    parent->add_property_(prop); 
    return true;
  }

bool xss_object_reader::read_json_method(Json::Value node, XSSObject parent, XSSObject& result)
  {
    result = XSSObject();

    XSSMethod mthd(new xss_method);
    XSSObject surrogate = read_json_object(node, XSSObject(), false);
    mthd->copy(surrogate);

    parent->methods()->push_back(mthd); //td: check for existing methods
    return true;
  }

bool xss_object_reader::read_json_event(Json::Value node, XSSObject parent, XSSObject& result)
  {
    result = XSSObject();

    XSSEvent ev(new xss_event);
    XSSObject surrogate = read_json_object(node, XSSObject(), false);
    ev->copy(surrogate);

    parent->events()->push_back(ev); //td: check for existing events
    return true;
  }

