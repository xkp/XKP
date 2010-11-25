
#ifndef XS_RUNTIME_ERROR_HH
#define XS_RUNTIME_ERROR_HH

#include <base.h>

namespace xkp
{
  //to be thrown as an exception
  struct runtime_error
    {
      runtime_error() {}
      runtime_error(param_list _data): data(_data) {}

      param_list data;
    };
    
  inline void runtime_throw(const param_list data)
    {
      throw runtime_error(data);
    }
}

#endif