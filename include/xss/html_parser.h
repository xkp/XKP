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
    
  typedef std::vector<tag> tag_list;
  
  //content iterators
  typedef str::iterator iterator_t;

  class html_parser
    {
      public:   
        bool parse(str& text, tag_list& result);
    };

}

#endif