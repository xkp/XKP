
#include <archive/json_archive.h>
#include <json/writer.h>
#include <json/reader.h>

using namespace xkp;

struct simpletype_writer : simpletype_visitor
  {
    simpletype_writer(Json::Value& val, const str& attr):
      val_(val),
      attr_(attr), 
      hit(true)
      {
      } 
    
    //simpletype_visitor
    virtual void int_(int value)
      {
        Json::Value v(value);
        if (attr_.empty())
          val_.append(v);
        else
          val_[attr_] = v;
      }
      
    virtual void long_(long value)
      {
        Json::Value v(value);
        if (attr_.empty())
          val_.append(v);
        else
          val_[attr_] = v;
      }
      
    virtual void float_(float value)
      {
        Json::Value v(value);
        if (attr_.empty())
          val_.append(v);
        else
          val_[attr_] = v;
      }
      
    virtual void double_(double value)
      {
        Json::Value v(value);
        if (attr_.empty())
          val_.append(v);
        else
          val_[attr_] = v;
      }
      
    virtual void bool_(bool value)
      {
        Json::Value v(value);
        if (attr_.empty())
          val_.append(v);
        else
          val_[attr_] = v;
      }
      
    virtual void str_(const str& value)
      {
        Json::Value v(value);
        if (attr_.empty())
          val_.append(v);
        else
          val_[attr_] = v;
      }
      
    virtual void unknown()
      {
        hit = false;
      }
      
    Json::Value& val_;
    str          attr_;
    bool         hit; 
  };

struct json_write_iterator : write_iterator
  {
    json_write_iterator(Json::Value& val) : val_(val) {}
  
    virtual Writer next(const variant& v)
      {
        simpletype_writer stw(val_, "");
        visit_simpletype(v, &stw);
        
        if (!stw.hit)
          {
            Json::Value& val = val_[val_.size()];
            return Writer( new json_writer( val ) );
          }
          
        return Writer();
      }
      
    Json::Value& val_;  
  };
  
struct json_read_iterator : read_iterator
  {
    json_read_iterator(Json::Value& val) : val_(val), idx_(0) {}
    
    virtual bool next(Reader& reader, variant& result)
      {
        if (idx_ >= val_.size())
          return false;
          
        Json::Value& val = val_[idx_++];
        
        switch(val.type())
          {
            case Json::nullValue:     result = variant();                           break;
            case Json::intValue:      result = val.asInt();                         break;  
            case Json::uintValue:     result = val.asUInt();                        break;
            case Json::realValue:     result = static_cast<float>(val.asDouble());  break;
            case Json::stringValue:   result = val.asString();                      break;
            case Json::booleanValue:  result = val.asBool();                        break;
            case Json::arrayValue:    assert(false); return false; //td: whats this an array of arrays?
            case Json::objectValue:   reader = Reader( new json_reader(val) );      break;
            default:                  assert(false); //say wha?
          }
        return true;
      }

    Json::Value&      val_;  
    Json::Value::UInt idx_; 
  };  

//json_writer
void json_writer::attribute(const str& name, const variant& value)
  {
    assert(!name.empty());
    
    simpletype_writer stw(val_, name);
    visit_simpletype(value, &stw);
    
    assert(stw.hit);
  }
  
Writer json_writer::create_node(const str& name)
  {
    Json::Value& val = val_[name];
    
    return Writer( new json_writer(val) );
  }
  
WriteIterator json_writer::create_iterator(const str& name, schema* type)
  {
    Json::Value& val = name.empty()? val_ : val_[name];
    return WriteIterator( new json_write_iterator(val) );
  }
  
//json_write_archive
json_write_archive::json_write_archive(type_registry* types):
  base_write_archive(types)
  {
  }

void json_write_archive::save(const variant& what)
  {
    base_write_archive::save(what);
    
    Json::StyledWriter w;
    result_ = w.write(root_);
  }

Writer json_write_archive::create_root()
  {
    return Writer( new json_writer(root_) );
  }

//json_reader
bool json_reader::attribute(const str& name, schema* type, variant& result)
  {
    assert(!name.empty());
    
    if (!val_.isMember(name))
      return false;
      
    Json::Value attr = val_[name];
    result           = json2xkp(type, attr);
    return true;
  }
  
Reader json_reader::create_node(const str& name)
  {
    if (!val_.isMember(name))
      return Reader();
      
    return Reader( new json_reader(val_[name]) );
  }
  
ReadIterator json_reader::create_iterator(const str& name, schema* type)
  {
    if (name.empty())
      {
        assert(val_.isArray()); //td: error, type mismatch
        return ReadIterator( new json_read_iterator(val_) );
      }
    
    if (!val_.isMember(name))
      return ReadIterator();

    Json::Value& arr = val_[name];
    assert(arr.isArray()); //td: error, type mismatch
    
    return ReadIterator( new json_read_iterator(arr) );
  }
  
void json_reader::visit(reader_visitor* visitor)
  {
    assert(false); //td: implement
  }
  
variant json_reader::json2xkp(schema* type, Json::Value& attr)
  {
    if (attr.isBool())
      {
        assert(type == type_schema<bool>()); //td: type mismatch
        return attr.asBool();
      }
    else if (attr.isInt())
      {
        assert(type == type_schema<int>()); //td: type mismatch
        return attr.asInt();
      }
    else if (attr.isDouble())
      {
        assert(type == type_schema<float>()); //td: type mismatch
        return static_cast<float>(attr.asDouble()); //td: deal with doubles
      }
    else if (attr.isString())
      {
        assert(type == type_schema<str>()); //td: type mismatch
        return attr.asString();
      }
    else
      {
        assert(false); //td: not an attribute
      }
    
    return variant();
  }
  
//json_read_archive
json_read_archive::json_read_archive(const str& json, type_registry* types):
  base_read_archive(types)
  {
    Json::Reader r;
    if (!r.parse(json, root_, false))
      {
        assert(false); //td: error, failed parsing
      }
  }

Reader json_read_archive::create_root()
  {
    return Reader( new json_reader(root_) );
  }
