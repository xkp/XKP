
#ifndef XSS_ERROR_HH
#define XSS_ERROR_HH

#include <base.h>

namespace xkp
{
  //to be thrown as an exception
  struct xss_error
    {
      xss_error() {}
      xss_error(param_list _data): data(_data) {}

      param_list data;
    };
    
  inline void xss_throw(const param_list data)
    {
      throw xss_error(data);
    }
}

#endif