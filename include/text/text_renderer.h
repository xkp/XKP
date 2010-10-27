
#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <xs.h>
#include <xs/vm.h>

namespace xkp
{
  struct text_renderer
    {
      bool compile(str src, code_context& ctx);
      str  render(code_context& ctx);

      private:
        struct render_info
          {
            str      text;
            ByteCode code;
          };
          
        typedef std::vector<render_info> render_list;
        
        render_list render_;
    };
}

#endif