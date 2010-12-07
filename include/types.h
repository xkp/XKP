
#ifndef XKP_TYPES_H
#define XKP_TYPES_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace xkp
{
  typedef std::string str;

  const int null = 0;

  template <class T>
  class reference : public boost::shared_ptr<T>
    {
      public:
        reference(): boost::shared_ptr<T>()
        {
        }

        reference(boost::shared_ptr<T>& p): boost::shared_ptr<T>(p)
        {
        }

        // added by Cuba
        reference(const boost::shared_ptr<T> p): boost::shared_ptr<T>(p)
        {
        }

        template<class Y>
        explicit reference( Y * p ): boost::shared_ptr<T>( p )
        {
        }
    };

  #define weak_reference boost::weak_ptr
}

#endif
