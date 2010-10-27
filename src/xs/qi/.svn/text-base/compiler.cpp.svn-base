
#include <xs/compiler.h>
#include <xs/grammar.h>

using namespace boost::spirit;
using namespace boost::spirit::qi;

namespace xkp
{
  bool xs_compiler::compile_expression(const str& expr, code_context& result)
    {
      typedef expression_grammar<std::string::const_iterator> expression_grammar;
      expression_grammar eg( result ); 
      
      white_space<std::string::const_iterator> white_space_skipper;

      std::string::const_iterator iter = expr.begin();
      std::string::const_iterator end = expr.end();
      return phrase_parse(iter, end, eg, white_space_skipper);
    }
    
  bool xs_compiler::compile_code(const str& code, code_context& result)
    {
      typedef code_grammar<std::string::const_iterator> code_grammar;
      code_grammar cg( result ); 
      
      white_space<std::string::const_iterator> white_space_skipper;

      std::string::const_iterator iter = code.begin();
      std::string::const_iterator end = code.end();
      return phrase_parse(iter, end, cg, white_space_skipper);
    }
}


