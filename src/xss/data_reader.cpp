
#include <xss/xml_data_reader.h>

using namespace xkp;

//data_entity
void data_entity::set_type(const str& type)
  {
    type_ = type;    
  }

void data_entity::set_id(const str& id)
  {
    id_ = id;
  }

void data_entity::add_attribute(const str& attr_name, const str& value)
  {
    attribute_map::iterator it = attributes_.find(attr_name);
    if (it == attributes_.end())
      attributes_.insert(std::pair<str, str>(attr_name, value));
    else
      it->second = value;
  }

data_entity& data_entity::create_child()
  {
    data_entity de;
    children_.push_back(de);

    return *(children_.end() - 1);
  }

