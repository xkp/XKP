#ifndef APPLICATION_READER_H
#define APPLICATION_READER_H

#include "xss_context.h"

namespace xkp
{
	class xss_object_reader
		{
			public:
				XSSObject							 read(const str& xml);
				std::vector<XSSObject> read_array(const str& xml);
		};
}

#endif