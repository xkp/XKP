
#ifndef XSS_DATA_READER_HH
#define XSS_DATA_READER_HH

#include <xs.h>

#include <vector>
#include <map>

namespace xkp{

//forwards
class data_entity;
class IDataReader;

//reference
typedef reference<data_entity> DataEntity;
typedef reference<IDataReader> DataReader;

//data structures
typedef std::map<str, str>      attribute_map;
typedef std::vector<DataEntity> entity_list;

class data_entity
  { 
    public:
      str         id();
      str         type();
      void        set_type(const str& id);
      void        set_id(const str& type);
      void        add_attribute(const str& attr_name, const str& value);
      str         attr(const str& attr_name);
      DataEntity  create_child();
      entity_list find_all(const str& type_name);
      DataEntity  get(const str& type_name);
    private:
      str type_;    
      str id_;
      
      attribute_map attributes_;
      entity_list   children_;    
  };
  
class IDataReader
  {
    public:
      virtual entity_list root() = 0;
  };

}

#endif