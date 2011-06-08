
#include "xss/object_reader.h"
#include "xss/xss_context.h"
#include "xss/xss_error.h"

using namespace xkp;

const str SReaderError("reader");

const str SPropertyMustHaveName("Properties must have a name");
const str SPropertyMustHaveType("Properties must have a type");

//xss_object_reader
xss_object_reader::xss_object_reader(XSSContext ctx):
  ctx_(ctx)
  {
  }

XSSObject xss_object_reader::read(const str& xml)
	{
    parse_xml(xml);
    return read_object(doc_.RootElement(), XSSObject());
	}

std::vector<XSSObject> xss_object_reader::read_array(const str& xml)
	{
    parse_xml(xml);

    return read_array(doc_.RootElement()->FirstChildElement());
	}

void xss_object_reader::parse_xml(const str& file)
  {
    const char* result = doc_.Parse(file.c_str());
    assert(false); //td: !!! throw error
  }

XSSObject xss_object_reader::read_object(TiXmlElement* node, XSSObject parent)
  {
    XSSObject result;

    str class_name = node->Value();
    if (!special_node(node, parent, result))
      {
        result = XSSObject(new xss_object());

	      bool found_class = false;
	      bool found_id    = false;

        //read properties
        const TiXmlAttribute* attr = node->FirstAttribute();
	      while(attr)
	        {
            str attr_name  = attr->Name();
            str attr_value = attr->Value();

            if (attr_name == "class")
              {
                result->set_type_name(attr_value);
              }
            else if (attr_name == "id")
              {
                result->set_id(attr_value);
              }
            else
              {
                result->add_property(attr_name, attribute_value(attr));
              }

	          attr = attr->Next();
	        }

        //children
        TiXmlElement* child_node = node->FirstChildElement();
        while(child_node)
          {
            XSSObject child = read_object(child_node, XSSObject());
            result->add_child( child );

            node = node->NextSiblingElement();
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
        XSSObject child_obj = read_object(child, XSSObject());
        result.push_back(child_obj);

        child = child->NextSiblingElement();
      }

    return result;
  }

variant xss_object_reader::attribute_value(const TiXmlAttribute* attr)
  {
    double dv;
    if (attr->QueryDoubleValue(&dv) == TIXML_SUCCESS)
      return (float)dv;

		int iv;
    if (attr->QueryIntValue(&iv) == TIXML_SUCCESS)
      return iv;
      
    str s(attr->Value());
		if (s == "true")
			return true;
		else if (s == "false")
			return false;
		
		return s;
  }

bool xss_object_reader::special_node(TiXmlElement* node, XSSObject parent, XSSObject& result)
  {
    str class_name = node->Value();
    str type       = node->Attribute("type");

    if (class_name == "property")
      {
        variant value;
        bool is_array = type == "array";
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
            str value_str = node->Attribute("value");
            if (!value_str.empty())
              {
                assert(false); //td: punting the type resolvation (sic) for later
              }
          }
        
        str name = node->Attribute("name");
        if (name.empty())
          {
						param_list error;
						error.add("id", SReaderError);
						error.add("desc", SPropertyMustHaveName);
						error.add("container", parent->id());

						xss_throw(error);
          }

        if (type.empty())
          {
						param_list error;
						error.add("id", SReaderError);
						error.add("desc", SPropertyMustHaveType);
						error.add("property name", name);
						error.add("container", parent->id());

						xss_throw(error);
          }

        XSSType xtype = ctx_? ctx_->get_type(type) : XSSType();
        XSSProperty prop(new xss_property(name, xtype, value, parent)); 

        parent->properties()->push_back(prop); //td: check for existing properties
      }

    return false;
  }
