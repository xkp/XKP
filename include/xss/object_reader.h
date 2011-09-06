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
			public:
        void enforce_types(bool value);
      private:
        XSSContext    ctx_;
        TiXmlDocument doc_;
        bool          enforce_types_;

        void          parse_xml(const str& file);
        XSSObject     read_object(TiXmlElement* node, XSSObject parent, bool do_special, XSSType force_type = XSSType());
        XSSObjectList read_array(TiXmlElement* node);
        variant       attribute_value(const TiXmlAttribute* attr);
        bool          special_node(TiXmlElement* node, XSSObject parent, XSSObject& result);
      private:
        bool read_property(TiXmlElement* node, const str& type, XSSObject parent, XSSObject& result);
        bool read_method(TiXmlElement* node, const str& type, XSSObject parent, XSSObject& result);
        bool read_event(TiXmlElement* node, const str& type, XSSObject parent, XSSObject& result);
        bool read_array(TiXmlElement* node, const str& type, XSSObject parent, XSSObject& result);
		};
}

#endif