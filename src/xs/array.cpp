
#include <xs/array.h>

using namespace xkp;

//meta_array_schema
bool meta_array_schema::create(variant& result, param_list* args)
  {
    if (!args)
      {
        result = type_schema<dynamic_array>();
        return true;
      }
    return false;
  }
  
void* meta_array_schema::get_pointer(void**)
  {
    assert(false);
    return null;
  }
  
bool meta_array_schema::clone(const variant v, variant& result)
  {
    assert(false);
    return false;
  }
