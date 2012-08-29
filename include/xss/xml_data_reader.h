#ifndef XML_DATA_READER_HH
#define XML_DATA_READER_HH

#include "data_reader.h"
#include <tinyxml.h>

namespace xkp{

class xml_data_reader : public IDataReader
  {
    public:
      bool load(const str& xml);
    public:
      virtual entity_list root();
    private:
      entity_list root_;

      bool parse_xml(const str& xml, TiXmlDocument& doc);
      void read_node(TiXmlElement* node, DataEntity entity);
  };

}

#endif