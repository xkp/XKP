
#ifndef XSS_INTERFACE_HH
#define XSS_INTERFACE_HH

#include <string>
#include <vector>

struct walk_info
{
	walk_info(int _type, const std::string& _symbol):
		type(_type),
		symbol(_symbol)
		{
		}

	int			type;
	std::string symbol;
};

typedef std::vector<walk_info> walk_list;

struct xss_error_info
{
	xss_error_info(const std::string& _desc, const std::string& _file, int _sline, int _scol, int _eline, int _ecol):
	desc(_desc),
	sline(_sline),
	scol(_scol),
	eline(_eline),
	ecol(_ecol),
	file(_file)
	{
	}

	std::string desc;
	std::string file;
	int         sline;
	int         scol;
	int         eline;
	int         ecol;
};

typedef std::vector<xss_error_info> xss_error_info_list;

class IObjectModel
{
	public:
		virtual int  load(const std::string& filename)												  = 0;
		virtual void addIncludes(int appId, const std::string& def_file, const std::string& src_file) = 0;
		virtual bool notifyChange(const std::string& fname, int line, int col, 
								  int oldEndLine, int oldEndCol, int newEndLine, int newEndCol)		  = 0;
		virtual bool updateChanges(int ms)															  = 0;
		virtual void queueChange(const std::string& fname, const std::string& content)				  = 0;
		virtual void walkContext(const std::string& fname, const std::string& text, 
								 int line, int col, walk_list& results)								  = 0;
		virtual void walkErrors(const std::string& fname, xss_error_info_list& errors)				  = 0;
		virtual bool buildProject(const std::string& fname, xss_error_info_list& errors)			  = 0;
		virtual std::string appName(const std::string& fname)										  = 0;
};

#endif