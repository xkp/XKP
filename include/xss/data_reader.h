
#ifndef XSS_DATA_READER_HH
#define XSS_DATA_READER_HH

#include <xs.h>

#include <vector>
#include <map>

namespace xkp{

class data_entity;

typedef std::map<str, str>       attribute_map;
typedef std::vector<data_entity> entity_list;

class data_entity
  { 
    public:
      void         set_type(const str& id);
      void         set_id(const str& type);
      void         add_attribute(const str& attr_name, const str& value);
      data_entity& create_child();
    private:
      str type_;    
      str id_;
      
      attribute_map attributes_;
      entity_list   children_;    
  };
  
class IDataReader
  {
    public:
      virtual entity_list& root() = 0;
  };

}

#endif