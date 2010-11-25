
#ifndef XS_ERROR_HH
#define XS_ERROR_HH

#include <base.h>

namespace xkp
{
  //to be thrown as an exception
  struct xs_error
    {
      xs_error() {}
      xs_error(param_list _data): data(_data) {}

      param_list data;
    };
    
  inline void xs_throw(const param_list data)
    {
      throw xs_error(data);
    }
}

#endif