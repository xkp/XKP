#ifndef XS_UTILS_HH
#define XS_UTILS_HH

#include <base.h>

namespace xkp
{

inline bool is_multi_line(const str& s)  
  {
    return s.find('\n') != str.npos;
  }  

inline str last_padding(const str& s)
  {
    size_t last_eol = s.find_last_of('\n');
    if (last_eol == str.npos)    
      last_eol = 0;
    else 
      last_eol++;
      
    str result = s.substr(last_eol, s.size());
    for(int i = 0; i < result.size(); i++)
      {
        //replace characters with spaces, is needed
        if (result[i] != ' ' && result[i] != '\t')
          result[i] = ' ';
      }
    return result;
  } 
                      
inline void split_lines(const str& s, std::vector<str>& lines)
  {
    size_t curr = 0;
    for(size_t i = 0; i < s.length(); i++)
      {
        if (s[i] == '\n')
          {
            if (curr < i)
              lines.push_back( s.substr(curr, i - curr) );
            curr = i + 1;
          }
      }
  }
  
inline void trim_last_empty_line(str& s)
  {
    if (s.empty())
      return;
    
    bool keep_going = true;
    for(size_t i = s.size() - 1; i >= 0 && keep_going; i--)
      {
        char ch = s[i];
        switch(ch)
          {
            case ' ':
            case '/t':
              break; //let it go
            case '/n':
            case 10: //td: there's gotta be a way to do this right, boost?
              {
                s.erase(i + 1, s.size() - i);
              }
            default:
              keep_going = false;
          }
      }
  }

  
}

#endif