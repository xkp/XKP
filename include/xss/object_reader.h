#ifndef APPLICATION_READER_H
#define APPLICATION_READER_H

#include "xss_context.h"

#include <tinyxml.h>
#include <json/value.h>

namespace xkp
{
	class xss_object_reader
		{
      public:
        xss_object_reader(XSSContext ctx = XSSContext());
			public:
				XSSObject							 read(const str& text);
				std::vector<XSSObject> read_array(const str& text);
			public:
        void enforce_types(bool value);
      private:
        XSSContext ctx_;
        bool       enforce_types_;
      private:
        //common
        void process_property(XSSObject result, XSSType type, const str& attr_name, variant attr_value);
        void process_child(XSSObject child);
      private:
        //xml
        TiXmlDocument doc_;

        bool parse_xml(const str& file);

        XSSObject     read_xml_object(TiXmlElement* node, XSSObject parent, bool do_special, XSSType force_type = XSSType());
        XSSObjectList read_xml_array(TiXmlElement* node);
        variant       xml_attribute_value(const TiXmlAttribute* attr);
        bool          xml_special_node(TiXmlElement* node, XSSObject parent, XSSObject& result);
        bool          read_xml_property(TiXmlElement* node, const str& type, XSSObject parent, XSSObject& result);
        bool          read_xml_method(TiXmlElement* node, const str& type, XSSObject parent, XSSObject& result);
        bool          read_xml_event(TiXmlElement* node, const str& type, XSSObject parent, XSSObject& result);
        bool          read_xml_array(TiXmlElement* node, const str& type, XSSObject parent, XSSObject& result);
      private:
        //json
        Json::Value json_;

        bool parse_json(const str& text);

        XSSObject     read_json_object(Json::Value node, XSSObject parent, bool do_special, XSSType force_type = XSSType());
        bool          json_special_node(const str& class_name, Json::Value node, XSSObject parent, XSSObject& result);
        variant       json_attr_value(Json::Value attr);

        bool          read_json_property(Json::Value node, XSSObject parent, XSSObject& result);
        bool          read_json_method(Json::Value node, XSSObject parent, XSSObject& result);
        bool          read_json_event(Json::Value node, XSSObject parent, XSSObject& result);
		};
}

#endif