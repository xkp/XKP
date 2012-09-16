
#ifndef XSS_INTERFACE_HH
#define XSS_INTERFACE_HH

#include <string>

class IObjectModel
{
	public:
		virtual int  load(const std::string& filename)												  = 0;
		virtual void addIncludes(int appId, const std::string& def_file, const std::string& src_file) = 0;
		virtual bool notifyChange(const std::string& fname, int line, int col, 
								  int oldEndLine, int oldEndCol, int newEndLine, int newEndCol)		  = 0;
		virtual void updateChanges(int ms)															  = 0;
		virtual void queueChange(const std::string& fname, const std::string& content)				  = 0;
};

#endif