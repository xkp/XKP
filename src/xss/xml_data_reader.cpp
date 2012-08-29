
#include "xss/xml_data_reader.h"

using namespace xkp;

//xml_data_reader
bool xml_data_reader::load(const str& xml)
  {
    TiXmlDocument doc;
    if (!parse_xml(xml, doc))
      return false; //td: error location

    DataEntity root(new data_entity);
    root_.push_back(root);

    read_node(doc.RootElement(), root);

    return true;
  }

entity_list xml_data_reader::root()
  {
    return root_;
  }

bool xml_data_reader::parse_xml(const str& xml, TiXmlDocument& doc)
  {
    doc.Parse(xml.c_str());
    return !doc.Error();
  }

void xml_data_reader::read_node(TiXmlElement* node, DataEntity entity)
  {
    entity->set_type(node->Value());
    entity->set_id(str(node->Attribute("id")));

    const TiXmlAttribute* attr = node->FirstAttribute();
	  while(attr)
	    {
        entity->add_attribute(attr->Name(), attr->Value());
	      attr = attr->Next();
      }

    TiXmlElement* child_node  = node->FirstChildElement();
    while(child_node)
      { 
        DataEntity child = entity->create_child();
        read_node(child_node, child);

        child_node = child_node->NextSiblingElement();
      }
  }
