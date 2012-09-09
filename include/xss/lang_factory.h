
#ifndef XSS_LANG_FACTORY_HH
#define XSS_LANG_FACTORY_HH

#include "object_model.h"

namespace xkp{

class lang_factory : public ILanguageFactory
  {
    public:
      virtual Language create(const str& name);
  };

}

#endif