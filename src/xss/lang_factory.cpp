
#include <xss/lang_factory.h>
#include <xss/lang/csharp.h>

using namespace xkp;

//lang_factory
Language lang_factory::create(const str& name)
  {
    if (name == "cs")
      return Language(new cs_lang); 

    return Language();
  }
