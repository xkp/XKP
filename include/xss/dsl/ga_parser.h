
#ifndef GA_PARSER_HH
#define GA_PARSER_HH

#include <base.h>

namespace xkp {
  
  struct shell_param
    {
      size_t  item_idx;
      size_t  item_spot;
      str     id;
    };

  struct ga_item
    {
      str                       variable;
      std::vector<str>          items;
      std::vector<shell_param>  params;
    };

  struct ga_parser
    {
      bool parse(str text, std::vector<ga_item>& result);

      private:
        void parse_item(const str& text, ga_item& result);
    };
}

#endif