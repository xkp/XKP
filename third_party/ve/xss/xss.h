// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the XSS_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// XSS_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef XSS_EXPORTS
#define XSS_API __declspec(dllexport)
#else
#define XSS_API __declspec(dllimport)
#endif

#include "interface.h"

extern "C" XSS_API IObjectModel* __stdcall create_object_model();
extern "C" XSS_API wchar_t*	     __stdcall get_token(wchar_t *InputBuf, long InputSize, long *InputHere, long *Line, long *Column, int *Symbol);

