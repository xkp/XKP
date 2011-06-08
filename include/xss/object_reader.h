#ifndef APPLICATION_READER_H
#define APPLICATION_READER_H

#include "xss_context.h"

#include <tinyxml.h>

namespace xkp
{
	class xss_object_reader
		{
      public:
        xss_object_reader(XSSContext ctx = XSSContext());
			public:
				XSSObject							 read(const str& xml);
				std::vector<XSSObject> read_array(const str& xml);
      private:
        XSSContext    ctx_;
        TiXmlDocument doc_;

        void          parse_xml(const str& file);
        XSSObject     read_object(TiXmlElement* node, XSSObject parent);
        XSSObjectList read_array(TiXmlElement* node);
        variant       attribute_value(const TiXmlAttribute* attr);
        bool          special_node(TiXmlElement* node, XSSObject parent, XSSObject& result);
		};
}

#endif