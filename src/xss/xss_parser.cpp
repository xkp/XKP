
#include "xss/xss_parser.h"

#include <locale>

using namespace xkp;
using namespace xkp::detail;

//xss_parser
void xss_parser::register_tag(const str& tag)
  {
    tags_.push_back(tag);
  }
  
bool xss_parser::parse(const str& text, xss_visitor* visitor)
  {
    size_t    ip          = 0;
    size_t    last_ip     = 0;
    size_t    sz          = text.size();
    bool      reading_tag = false;
    int       open_count  = 1;
    tag_info  current_tag; 
    
    while(ip < sz)
      {
        char curr = text[ip++];
        if (curr == '<')
          {
            size_t   end_tag;
            tag_info tag;
            int      step_ip = ip; 
            if (read_tag(text, ip, tag, end_tag))
              {
                ip = end_tag;
                if (reading_tag)
                  {
                    if (tag.name == current_tag.name)
                      {
                        if (tag.closed)
                          {
                            open_count--;
                            if (open_count == 0)
                              {
                                reading_tag = false;
                                str tag_text = text.substr(last_ip, step_ip - last_ip - 1);
                                visitor->visit(current_tag.name, tag_text, &current_tag.args);
                                
																last_ip = ip;

																skip_spaces(text, ip);
                                if (text[ip] == '\n')
																	{
																		ip++;
																		last_ip = ip;
																	}
																else 
																	ip = last_ip;
                              }
                          }
                        else if (!tag.single)
                          {
                            open_count++;
                          }
                      }
                  }
                else if (!tag.closed)
                  {
                    //find out one of our tags was found
                    std::vector<str>::iterator it = tags_.begin();
                    std::vector<str>::iterator nd = tags_.end();
                    
                    bool found = false;
                    for(; it != nd; it++)
                      {
                        if (*it == tag.name)
                          {
                            found = true;
                            break;
                          }
                      }
                    
                    if (found)
                      {
                        if (ip > last_ip)
                          {
                            str txt = text.substr(last_ip, step_ip - last_ip - 1); //no tag
                            visitor->visit("text", txt, null);
                          }
                        
                        last_ip = ip;
                        
                        if (tag.single)
                          {
                            visitor->visit(tag.name, "", &tag.args);
                          }
                        else
                          {
                            current_tag = tag;
                            reading_tag = true;
                            open_count  = 1;
                          }  
                      }
                  }  
              }
          }
      }
      
    if (last_ip < text.size())
      {
        str txt = text.substr(last_ip, text.size() - last_ip); 
        visitor->visit("text", txt, null);
      }      
      
    return true;
  }
  
bool xss_parser::read_tag(const str& text, size_t pos, detail::tag_info& tag, size_t& end_tag)
  {
    std::locale loc;
  
    size_t ip = pos;
    size_t sz = text.size();

    if (text[ip] == '/')
      {
        tag.closed = true;
        ip++;
      }
    
    //skip spaces
    if (!skip_spaces(text, ip))
      return false;
        
    bool   found   = false;
    size_t name_ip = ip;
    while(ip < sz)    
      {
        char c = text[ip];
        if (!isalpha(c) && c != ':' && c != '_')
          {
            tag.name = text.substr(name_ip, ip - name_ip);
            break;
          }
        ip++;
      }
      
    if (!registered_tag(tag.name))
      return false;
    
    if (!skip_spaces(text, ip))
      return false;

    if (text[ip] == '/')
      {
        if (tag.closed)
          return false;
        
        tag.single = true;
        ip++;  

        if (text[ip] == '>')
          {
            end_tag = ip + 1;
            return true;
          }
        else    
          return false;
      }
    else if (text[ip] == '>')
      {
        end_tag = ip + 1;
        return true;
      }
    
    bool result = read_attributes(text, ip, tag);
    
    end_tag = ip + 1;
    return result;
  }

bool xss_parser::skip_spaces(const str& text, size_t& pos)
  {
    while (text[pos] == ' ' && pos < text.size())
      pos++;
      
    return pos < text.size();
  }
  
bool xss_parser::read_attributes(const str& text, size_t& pos, detail::tag_info& tag)
  {
    int state = 0;
    str attr_name;
    str attr_value;
    while (pos < text.size())
      {
        char c = text[pos];
        switch(state)
          {
            case 0: //expecting identifier or the end
              {
                if (isalpha(c))
                  {
                    state = 1; //read identifier
                    continue;
                  }
                else if (c == '/')
                  {
                    if (pos < text.size() - 1 && text[pos + 1] == '>')
                      {
                        tag.single = true;
                        pos++;
                        return true;
                      }
                    else 
                      return false;
                  }
                else if (c == '>')  
                  {
                    return true;
                  }
                else if (c != ' ')
                  return false;
                
                break;
              }
            case 1: 
              {
                if (isalpha(c) || c == '_')
                  attr_name += c;
                else if (c == ' ' || c == '=')
                  {
                    if (!skip_spaces(text, pos))
                      return false;
                    
                    if (pos >= text.size())
                      return false;
                      
                    if (text[pos] != '=')
                      return false;
                    
                    pos++;
                    if (!skip_spaces(text, pos))
                      return false;
                    
                    if (text[pos] != '"')
                      return false;

                    state = 2; //reading value
                  }
                break;  
              }
            case 2:
              {
                if (c == '"')
                  {
                    tag.args.add(attr_name, attr_value);
                    attr_name  = "";
                    attr_value = "";
                    
                    if (!skip_spaces(text, pos))
                      return false;
                      
                    state  = 0;
                  }
                else 
                  attr_value += c;
                break;
              }
          }
        pos++;
      }
      
    return false;
  }
  
bool xss_parser::registered_tag(const str& tagname)
  {
    std::vector<str>::iterator it = tags_.begin();
    std::vector<str>::iterator nd = tags_.end();
    
    for(; it != nd; it++)
      {
        if (*it == tagname)
          return true;
      }
      
    return false;
  }
