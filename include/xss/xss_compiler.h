#ifndef XSS_COMPILER_HH
#define XSS_COMPILER_HH

#include <base.h>
#include <schema.h>
#include <dynamic_objects.h>
#include <xs.h>

#include "xss_context.h"

namespace xkp
{
  //forwards
	class xss_application_renderer;
  class xss_module;

  //reference types
  typedef reference<xss_application_renderer> XSSApplicationRenderer;
  typedef reference<xss_module>               XSSModule;

  //classes
	class xss_application_renderer
    {
      public:
        xss_application_renderer(const str& xss_file);
      public:
        XSSContext getContext();
        void       register_module(const str& id, XSSModule module);
      private:
        str        filename_;
        XSSContext context_;
    };
  
	class xss_module : public xss_object
    {
      public:
        xss_module(XSSContext ctx);
    };

  class xss_compiler
		{
      public:
        void build(const str& xml);
		  private:
        std::vector<XSSApplicationRenderer> applications_;

        void      read_project(const str& xml);
        void      read_application_types(std::vector<XSSObject> & applications);
        XSSModule read_module(const str& src, XSSApplicationRenderer app, XSSObject module);
        void      read_types(XSSObject module_data, XSSApplicationRenderer app, XSSModule module);
		};
}


#endif