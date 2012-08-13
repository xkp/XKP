
#include <xss/dsl/template.h>
#include <xss/xss_error.h>

#include <boost/algorithm/string/split.hpp>

using namespace xkp;

const str STemplateDSL("template dsl");

const str SInvalidExpression("template expressions do not support nested braces");
const str SInvalidElseTag("template else must follow and if or elseif");

struct smarty_item;

typedef std::vector<smarty_item> smarty_items;

enum SMARTY_TYPE
  {
    SMARTY_TEXT,
    SMARTY_FOR,
    SMARTY_IF,
    SMARTY_ELSE,
    SMARTY_ELSEIF,
    SMARTY_EXPRESSION,
  };

struct smarty_item
  {
    smarty_item():
      type(SMARTY_TEXT)
      {
      }

    SMARTY_TYPE  type;
    str          text;
    smarty_items children;
  };

struct smarty_parser
  {
    smarty_parser(smarty_items& result):
      container_(result)
      {
      }

    void parse(const str& text)
      {
        str::const_iterator it = text.begin();
        str::const_iterator nd = text.end();
        for(; it != nd; it++)
          {
            char c = *it;
            switch(c)
              {
                case '{': 
                  {
                    int advance = open_brace(it, nd); 
                    it += advance;
                    break;
                  }
                case '}': 
                  {
                    close_brace(); 
                    break;
                  }
                default:
                  {
                    current_.text += c;
                  }
              }
          }
      }

    private:
      smarty_items&              container_;
      std::vector<smarty_items*> stack_;
      smarty_item                current_;

      int open_brace(str::const_iterator it, str::const_iterator nd)
        {
          if (current_.type == SMARTY_EXPRESSION)
            {
                param_list error;
                error.add("id", STemplateDSL);
                error.add("desc", SInvalidExpression);
                xss_throw(error);
            }  

          if (match("for ", it, nd))  
            {
              push(SMARTY_FOR);
              return 4;
            }
          else if (match("if ", it, nd))
            {
              push(SMARTY_IF);
              return 3;
            }
          else if (match("else ", it, nd))
            {
              if (!check_sibling(SMARTY_IF) && !check_sibling(SMARTY_ELSEIF))
                {
                  param_list error;
                  error.add("id", STemplateDSL);
                  error.add("desc", SInvalidElseTag);
                  xss_throw(error);
                }  

              push(SMARTY_ELSE);
              return 5;
            }
          else if (match("elseif ", it, nd))
            {
              if (!check_sibling(SMARTY_IF) && !check_sibling(SMARTY_ELSEIF))
                {
                  param_list error;
                  error.add("id", STemplateDSL);
                  error.add("desc", SInvalidElseTag);
                  xss_throw(error);
                }  

              push(SMARTY_ELSEIF);
              return 7;
            }
          else
            {
              push(SMARTY_EXPRESSION);
              return 0;
            }
        }

      void close_brace()
        {
          if (current_.type != SMARTY_TEXT)
            pop();
          else
            current_.text += '}';
        } 

      bool match(const str& keyword, str::const_iterator& it, str::const_iterator& end)
        {
          str::const_iterator curr = it;
          for(size_t i = 0; i < keyword.size(); i++, curr++)
            {
              if (curr == end)
                return false;

              if (*curr != keyword[i])
                return false;
            }

          return true;
        }

      bool check_sibling(SMARTY_TYPE tag)
        {
          if (container_.empty())
            return false;

          smarty_item& itm = container_[container_.size() - 1];
          return itm.type == tag;
        }

      void push(SMARTY_TYPE tag)
        {
          container_.push_back(current_);
          current_.type = tag;
          current_.text = str();
          current_.children.clear();

          stack_.push_back(&container_);
          container_ = current_.children;
        }

      void pop()
        {
          container_.push_back(current_);
          current_.type = SMARTY_TEXT;
          current_.text = str();
          current_.children.clear();

          container_ = *(stack_[stack_.size() - 1]);
          stack_.erase(stack_.end() - 1);
        }
  };

//dsl_native
str dsl_template::render(dsl& info, XSSContext ctx)
  {
    smarty_items  result;
    smarty_parser parser(result);

    parser.parse(info.text);
    return str();
  }
