
#ifndef XS_COMPILER_H
#define XS_COMPILER_H

#include <xs/ast.h>
#include <boost/lexical_cast.hpp>

namespace xkp
{

class xs_compiler
  {
    public:
      xs_compiler();
      xs_compiler(std::vector<str>& dsls);
    public:
      bool compile_expression(const str& expr, expression& result);
      bool compile_code(const str& code_str, code& result);
      bool compile_xs(const str& code_str, xs_container& result);
      bool compile_xs_file(const str& filename, xs_container& result);
    private:
      std::vector<str> dsls_;
      
      void init_grammar();
      str  process_dsl(const str& src, std::vector<str>& dsl_texts);
      str  file2str(const str& filename);
  };

}

#endif