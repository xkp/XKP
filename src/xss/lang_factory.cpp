
#include <xss/lang_factory.h>

#include <xss/lang/csharp.h>
#include <xss/lang/java.h>

using namespace xkp;

//lang_factory
Language lang_factory::create(const str& name)
  {
    if (name == "cs")
      return Language(new cs_lang);
    else
    if (name == "java")
      return Language(new java_lang);

    return Language();
  }
