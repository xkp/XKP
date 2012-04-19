
#ifndef GA_PARSER_HH
#define GA_PARSER_HH

#include <base.h>

namespace xkp {
  
  struct ga_item
    {
      str              variable;
      std::vector<str> text;
      std::vector<str> params;
    };

  struct ga_parser
    {
      bool parse(str text, std::vector<ga_item>& result);

      private:
        void parse_item(const str& text, ga_item& result);
    };
}

#endif