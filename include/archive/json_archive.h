
#ifndef JSON_ARCHIVE_HH
#define JSON_ARCHIVE_HH

#include <serial.h>
#include <json/value.h>

namespace xkp
{
  struct json_writer : writer
    {
      json_writer(Json::Value& value) : val_(value) {}
    
      //writer
      virtual void          attribute(const str& name, const variant& value);
      virtual Writer        create_node(const str& name); 
      virtual WriteIterator create_iterator(const str& name, schema* type = null); 
      
      Json::Value& val_;
    };

  struct json_write_archive : base_write_archive
    {
      json_write_archive(type_registry* types = null);

      str result() {return result_;}
      
      //write archive
      virtual Writer create_root(); 
      virtual void   save(const variant& what);
      
      private:
        str         result_;      
        Json::Value root_;
    };

  struct json_reader : reader
    {
      json_reader(Json::Value& val) : val_(val) {}
    
      virtual bool         attribute(const str& name, schema* type, variant& result);
      virtual Reader       create_node(const str& name);
      virtual ReadIterator create_iterator(const str& name, schema* type); 
      virtual void         visit(reader_visitor* visitor);
      
      private:
        Json::Value& val_;
        variant json2xkp(schema* type, Json::Value& attr);
    };

  struct json_read_archive : base_read_archive
    {
      json_read_archive(const str& json, type_registry* types = null);
      
      protected:
        virtual Reader create_root(); 
      private:
        Json::Value root_;
    };
}

#endif