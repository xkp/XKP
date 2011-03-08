
#include <archive/xml_archive.h>

using namespace xkp;

struct xml_simpletype_writer : simpletype_visitor
  {
    xml_simpletype_writer(TiXmlElement* val, const str& attr):
      val_(val),
      attr_(attr), 
      hit(true)
      {
      } 
    
    //simpletype_visitor
    virtual void int_(int value)
      {
        assert(!attr_.empty());
        val_->SetAttribute(attr_.c_str(), value);
      }
      
    virtual void long_(long value)
      {
        assert(!attr_.empty());
        val_->SetAttribute(attr_.c_str(), value);
      }
      
    virtual void float_(float value)
      {
        assert(!attr_.empty());
        val_->SetDoubleAttribute(attr_.c_str(), value);
      }
      
    virtual void double_(double value)
      {
        assert(!attr_.empty());
        val_->SetDoubleAttribute(attr_.c_str(), value);
      }
      
    virtual void bool_(bool value)
      {
        assert(!attr_.empty());
        val_->SetAttribute(attr_.c_str(), value? "true" : "false");
      }
      
    virtual void str_(const str& value)
      {
        assert(!attr_.empty());
        val_->SetAttribute(attr_.c_str(), value.c_str());
      }
      
    virtual void unknown()
      {
        hit = false;
      }
      
    TiXmlElement* val_;
    str           attr_;
    bool          hit; 
  };

//xml_writer
void xml_writer::attribute(const str& name, const variant& value)
  {
    if (name == "class")
      {
        str node_name  = node_->Value();
        str class_name = value;
        if (node_name == class_name)
          return;
      }  
    
    xml_simpletype_writer stw(node_, name);
    visit_simpletype(value, &stw);
    
    assert(stw.hit);
  }
  
Writer xml_writer::create_node(const str& name)
  {
    TiXmlElement* node = new TiXmlElement(name.c_str());
    node_->LinkEndChild( node );
    
    return Writer( new xml_writer(node, types_) );
  }
  
struct simpletype_nodewriter : simpletype_visitor
  {
    simpletype_nodewriter(TiXmlElement* node):
      node_(node),
      hit(true)
      {
      } 
    
    //simpletype_visitor
    virtual void int_(int value)
      {
        TiXmlElement* node = new TiXmlElement("int");
        node_->LinkEndChild( node );

        node->SetAttribute("value", value);
      }
      
    virtual void long_(long value)
      {
        TiXmlElement* node = new TiXmlElement("long");
        node_->LinkEndChild( node );

        node->SetAttribute("value", value);
      }
      
    virtual void float_(float value)
      {
        TiXmlElement* node = new TiXmlElement("float");
        node_->LinkEndChild( node );

        node->SetDoubleAttribute("value", value);
      }
      
    virtual void double_(double value)
      {
        TiXmlElement* node = new TiXmlElement("double");
        node_->LinkEndChild( node );

        node->SetDoubleAttribute("value", value);
      }
      
    virtual void bool_(bool value)
      {
        TiXmlElement* node = new TiXmlElement("bool");
        node_->LinkEndChild( node );

        node->SetAttribute("value", value? "true" : "false");
      }
      
    virtual void str_(const str& value)
      {
        TiXmlElement* node = new TiXmlElement("string");
        node_->LinkEndChild( node );

        node->SetAttribute("value", value.c_str());
      }
      
    virtual void unknown()
      {
        hit = false;
      }
      
    TiXmlElement* node_;
    bool          hit; 
  };

struct xml_write_iterator : write_iterator
  {
    xml_write_iterator(TiXmlElement* node, schema* type, type_registry* types) : 
      node_(node),
      types_(types) 
      {
        if (type && types)
          {
            type_ = types->type_name( type);
          }
      }
  
    virtual Writer next(const variant& v)
      {
        simpletype_nodewriter stnw(node_);
        visit_simpletype(v, &stnw);
        
        if (!stnw.hit)
          {
            str node_name = type_;
            if (node_name.empty())
              {
                schema* node_type = true_type(v);

                if (types_)
                  node_name = types_->type_name( node_type );
                else
                  {
                    assert((node_type->options() & TYPE_NON_INSTANTIABLE) == 0);
                    node_name = "object";
                  }
              }
            
            assert(!node_name.empty());
            TiXmlElement* node = new TiXmlElement(node_name.c_str());
            node_->LinkEndChild( node );
            return Writer( new xml_writer(node, types_) );
          }
          
        return Writer();
      }
      
    TiXmlElement*   node_;  
    str             type_;
    type_registry*  types_;
  };

WriteIterator xml_writer::create_iterator(const str& name, schema* type)
  {
    if (name.empty())
      return WriteIterator( new xml_write_iterator(node_, type, types_) );
    
    TiXmlElement* node = new TiXmlElement(name.c_str());
    node_->LinkEndChild( node );
    return WriteIterator( new xml_write_iterator(node, type, types_) );
  }
  
//xml_write_archive
xml_write_archive::xml_write_archive(type_registry* types):
  base_write_archive(types)
  {
    TiXmlElement* node = new TiXmlElement("xml");
    doc_.LinkEndChild( node );
  }
  
Writer xml_write_archive::create_root()
  {
    return Writer( new xml_writer(doc_.RootElement(), types_) );
  }
  
void xml_write_archive::save(const variant& what)
  {
	  TiXmlPrinter printer;
	  printer.SetIndent( "\t" );

	  base_write_archive::save( what ); 
	  
	  doc_.Accept( &printer );
	  result_ = printer.Str();
  }
  
//xml_reader
bool xml_reader::attribute(const str& name, schema* type, variant& result)
  {
    const char* res;
    if (type == type_schema<int>())
      {
        int v;
        res = node_->Attribute(name.c_str(), &v);
        result = v;
      }
    else if (type == type_schema<float>())
      {
        double v;
        res = node_->Attribute(name.c_str(), &v);
        result = static_cast<float>(v);
      }
    else if (type == type_schema<double>())
      {
        double v;
        res = node_->Attribute(name.c_str(), &v);
        result = v;
      }
    else if (type == type_schema<str>())
      {
        res = node_->Attribute(name.c_str());
        
        if (res)
          {
            str v = res;
            result = v;
          }
      }
    else if (type == type_schema<bool>())
      {
        res = node_->Attribute(name.c_str());
        
        if (res)
          {
            str v = res;
            result = v == "true"? true : false;
          }
      }
      
    if (!res)
      {
        if (name == "class")
          {
            str v = node_->Value();
            result = v;
            return true;
          }
        
        return false;
      }
      
    return true;
  }
  
Reader xml_reader::create_node(const str& name)
  {
    TiXmlElement* node = node_->FirstChildElement(name.c_str());
    if (node)
      return Reader( new xml_reader(node, types_, options_) );
      
    return Reader();
  }
  
struct xml_read_iterator : read_iterator
  {
    xml_read_iterator(TiXmlElement* node, schema* type, type_registry* types, size_t options, str name = ""):
      type_(type),
      types_(types),
      name_(name),
      options_(options)
      {
        if (name_.empty())
          curr_ = node->FirstChildElement();
        else
          curr_ = node->FirstChildElement(name_.c_str());
      }  
  
    virtual bool next(Reader& reader, variant& result)
      {
        if (!curr_)
          return false;
        
        schema* curr_type = type_;
        if (!curr_type && types_)  
          {
            str type_name = curr_->Value();
            curr_type     = types_->get_type(type_name);
          }
        
        assert(curr_type);
        if (curr_type->options() & TYPE_SIMPLE)
          {
            xml_reader r(curr_, types_, options_);
            r.attribute("value", curr_type, result);
          }
        else
          {
            reader = Reader( new xml_reader(curr_, types_, options_) );
          }
      
        if (name_.empty())  
          curr_ = curr_->NextSiblingElement();
        else
          curr_ = curr_->NextSiblingElement(name_.c_str());
        return true;
      }
      
    private:
      TiXmlElement*  curr_;
      str            name_;
      schema*        type_;
      type_registry* types_;
      size_t         options_;
  };  

ReadIterator xml_reader::create_iterator(const str& name, schema* type)
  {
    if (name.empty())
      return ReadIterator( new xml_read_iterator(node_, type, types_, options_) );
      
    TiXmlElement* node = node_->FirstChildElement(name.c_str());
    if (!node)
      return ReadIterator();
    
    return ReadIterator( new xml_read_iterator(node, type, types_, options_) );
  }
  
void xml_reader::visit(reader_visitor* visitor)
  {
    if (options_ & XML_RESOLVE_ID)
      {
	      const char* id_attr = node_->Attribute( "id" );
	      if (id_attr == null)
	        {
	          str id(node_->Value());
	          visitor->attribute("id", id);
	        }
      }

    if (options_ & XML_RESOLVE_CLASS)
      {
	      const char* class_attr = node_->Attribute( "class" );
	      if (class_attr == null)
	        {
	          str clazz(node_->Value());
	          visitor->attribute("class", clazz);
	        }
      }

	  const TiXmlAttribute* attr = node_->FirstAttribute();
	  while(attr)
	    {
	      visitor->attribute(attr->Name(), attribute_value(attr));
	      attr = attr->Next();
	    }
	    
    TiXmlElement*    node = node_->FirstChildElement();
    std::vector<str> used_names;
    while(node)
      {
        str node_name = node->Value();
        
        //for convenience, we'll try to deduce arrays
        //td: there is a problem here where some nodes are mistaken as arrays
        //an scene full of cubes, for example
        //there is a poor solution down there that assumes the array must not have attributes
        //meh
        bool is_array  = false;
        
        if (!node->FirstAttribute())
          {
            TiXmlElement* curr = node->FirstChildElement();
            if (curr)
              {
                str child_name = curr->ValueStr();
                int same_count = 0;
                while(curr)
                  {
                    str curr_name = curr->Value();
                    if (curr_name == child_name)
                      same_count++;
                    else
                    {
                      same_count = 0;
                      break;
                    }
                    curr = curr->NextSiblingElement();
                  }
                  
                is_array = same_count >= 1;
              }
          }

        if (is_array)
          {
            ReadIterator ri( new xml_read_iterator(node, null, types_, options_) ); //td: types
            visitor->iterator(node_name, ri); 
          }
        else
          {
            Reader obj_reader( new xml_reader(node, types_, options_) );
            if (options_ & XML_RESOLVE_ID)
              {
	              const char* id_attr = node->Attribute( "id" );
	              if (id_attr)
	                {
	                  node_name = id_attr;
	                }
              }
            visitor->object(node_name, obj_reader); 
          }

        node = node->NextSiblingElement();
      }
  }

variant xml_reader::attribute_value(const TiXmlAttribute* attr)
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
  
//xml_read_archive
xml_read_archive::xml_read_archive(const str& xml, type_registry* types, size_t options):
  base_read_archive(types),
  options_(options)
  {
    doc_.Parse(xml.c_str());
  }
  
Reader xml_read_archive::create_root()
  {
    return Reader( new xml_reader(doc_.RootElement(), types_, options_) );
  }
  
