
#include <xss/xml_data_reader.h>

using namespace xkp;

//data_entity
str data_entity::id()
  {
    return id_;
  }

str data_entity::type()
  {
    return type_;
  }

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

str data_entity::attr(const str& attr_name)
  {
    attribute_map::iterator it = attributes_.find(attr_name);
    if (it != attributes_.end())
      return it->second;
    return str();
  }

DataEntity data_entity::create_child()
  {
    DataEntity result(new data_entity);
    children_.push_back(result);

    return result;
  }

entity_list data_entity::find_all(const str& type_name)
  {
    entity_list result;
    
    entity_list::iterator it = children_.begin();
    entity_list::iterator nd = children_.end();
    for(; it != nd; it++)
      {
        DataEntity de = *it;
        if (de->type() == type_name)
          result.push_back(de);
      }
    
    return result;
  }

DataEntity data_entity::get(const str& type_name)
  {
    entity_list result;
    
    entity_list::iterator it = children_.begin();
    entity_list::iterator nd = children_.end();
    for(; it != nd; it++)
      {
        DataEntity de = *it;
        if (de->type() == type_name)
          return de;
      }
    
    return DataEntity();
  }
