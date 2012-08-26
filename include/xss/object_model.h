
#ifndef XSS_OBJECT_MODEL_HH
#define XSS_OBJECT_MODEL_HH

#include "data_reader.h"

namespace xkp{

class object_model
  {
    public:
      void load(const str& project, param_list& args);
  };
}
#endif