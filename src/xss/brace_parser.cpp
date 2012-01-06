
#include <xss/brace_parser.h>
#include <xs/ast.h>

using namespace xkp;

struct indices
{
  indices(size_t _start, size_t _end):
    start(_start),
    end(_end)
    {
    }

  indices(const indices& other):
    start(other.start),
    end(other.end)
    {
    }

  size_t start;
  size_t end;
};

//brace_parser
void brace_parser::parse(const str& what, xss_visitor* visitor)
  {
    std::vector<indices> items;
    bool                 expecting_closed = false;
    size_t               idx              = 0;
    for(size_t i = 0; i < what.size(); i++)
      {
        char c = what[i];
        if (c == '{')
          {
            idx = i;
            expecting_closed = true;
          }
        else if (c == '}' && expecting_closed)
          {
            expecting_closed = false;

            indices found(idx, i);
            items.push_back(found);

            idx = i + 1;
          }
      }

    std::vector<indices>::iterator it = items.begin();
    std::vector<indices>::iterator nd = items.end();

    idx = 0;
    for(; it != nd; it++)
      {
        str expr = str(what.begin() + it->start + 1, what.begin() + it->end);
        xs_utils xs;
        expression useless;
        if (!xs.compile_expression(expr, useless))
          continue;

        if (idx < it->start)
          {
            str text = str(what.begin() + idx, what.begin() + it->start);
            visitor->visit("text", text, null);
          }

        visitor->visit("xss:e", expr, null);

        idx = it->end + 1;
      }

    if (idx < what.size())
      {
        str text = str(what.begin() + idx, what.end());
        visitor->visit("text", text, null);
      }
  }
