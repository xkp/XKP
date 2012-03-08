#ifndef HTML_RENDERER_HH 
#define HTML_RENDERER_HH 

#include <base.h>

namespace xkp 
{
  //html tag information
  typedef std::map<str, str>  prop_list;
  typedef std::pair<str, str> prop_pair;

  struct tag
    {
      size_t        start;
      size_t        close;
      str           tag_name;
      bool          closes;
      bool          complete;
      prop_list     props;
      
      void clear()
        {
          props.clear();
          
          tag_name = "";
          closes   = false;
          complete = false;
        }
        
      void add_prop(const str& name, const str& value )
        {
          props.insert( prop_pair(name, value) );
        }
    };
    
  struct tag_list
    {
      typedef std::vector<tag>::iterator iterator;
      
      iterator begin();
      iterator end();
      void     push_back(const tag t); 
      int      find(const str& id);
      tag&     get(int idx);
      size_t   size();
      private:
        std::vector<tag> tags_;
    };

  //content iterators
  typedef str::iterator iterator_t;

  class html_parser
    {
      public:   
        bool parse(str& text, tag_list& result);
    };

}

#endif