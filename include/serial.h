
#ifndef XKP_SERIALIZATION
#define XKP_SERIALIZATION

#include "base.h"
#include "dynamic_objects.h"

namespace xkp
{
  //interface
  struct reader;
  struct writer;
  struct write_iterator;
  struct read_iterator;

  typedef reference<read_iterator>  ReadIterator;
  typedef reference<reader>         Reader;
  typedef reference<write_iterator> WriteIterator;
  typedef reference<writer>         Writer;

  struct read_iterator
    {
      virtual bool next(Reader& reader, variant& result) = 0;
    };  
    
  struct reader_visitor
    {
      virtual void attribute(const str& name, const variant value)  = 0;
      virtual void object(const str& name, Reader reader)           = 0;
      virtual void iterator(const str& name, ReadIterator reader)   = 0;
    };

  struct reader
    {
      virtual bool         attribute(const str& name, schema* type, variant& result) = 0;
      virtual Reader       create_node(const str& name)                              = 0;
      virtual ReadIterator create_iterator(const str& name, schema* type)            = 0; 
      virtual void         visit(reader_visitor* visitor)                            = 0; 
    };
    
  struct write_iterator
    {
      virtual Writer next(const variant& v) = 0;
    };  

  struct writer
    {
      virtual void          attribute(const str& name, const variant& value)  = 0;
      virtual Writer        create_node(const str& name)                      = 0; 
      virtual WriteIterator create_iterator(const str& name, schema* type)    = 0; 
    };
    
  struct read_archive
    {
      virtual variant get(schema* type)      = 0;
      virtual variant query(const str& name) = 0;
    };

  struct write_archive
    {
      virtual void save(const variant& what) = 0;
    };
    
  //engine
  struct base_archive
    {
      virtual bool attributed_type(schema* type);
    };
  
  struct base_write_archive : base_archive, write_archive
    {
      base_write_archive(type_registry* types = null) : types_(types) {}
    
      virtual void save(const variant& what);
      
      protected:
        //implementor interface
        virtual Writer create_root() = 0; 
      protected:
        //detail
        void write(Writer w, const str& name, variant& v);
        
        type_registry* types_;
    };

  struct base_read_archive : base_archive, read_archive
    {
      base_read_archive(type_registry* types = null) : types_(types) {}


      virtual variant get(schema* type);
      virtual variant query(const str& name);
      
      protected:
        //implementor interface
        virtual Reader create_root() = 0; 

      public:
        bool read(Reader w, schema* type, variant& result);
        void read_iterated(Reader r, const str& name, variant& iterated);
        void read_object(Reader r, const str& name, variant& obj, schema* type);
        void read_iterable(ReadIterator rit, variant obj, schema* type);
      protected:
        type_registry* types_;
    };
}

#endif