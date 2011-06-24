
#ifndef XSS_GENERATOR
#define XSS_GENERATOR

#include "xss_parser.h"
#include "xss_context.h"

#include <xs.h>
#include <map>

namespace xkp
{
  template <typename T>
  class base_xss_renderer : public xss_visitor
    {
      public:
        //xss_visitor
        virtual void visit(const str& tag, const str& text, param_list* args)
          {
            handler_map::iterator it = handlers_.find(tag);
            if (it != handlers_.end())
              {
                (this->*(it->second))(text, args);
              }
            else
              {
                //td: error handling
              }
          }
      protected:
        typedef bool (T::* tag_handler)(const str& text, param_list* args);
        typedef std::map<str, tag_handler>  handler_map;
        typedef std::pair<str, tag_handler> handler_pair;

        handler_map handlers_;
    };                              
  

  class xss_renderer : base_xss_renderer<xss_renderer>
  {
    public:
      xss_renderer();
    public:
      str   get(XSSContext ctx);
      void  append(const str& s);
      void  append_marker(const str& name, const str& s);
    private:
      str        result_;

			//markers, this simplifies the xss code in ways it didnt expect
			struct marker_info
				{
					marker_info()										      : idx(-1) {}
					marker_info(const marker_info& other) : idx(other.idx), value(other.value) {}

					str value;
					int idx;
				};

			typedef std::map<str, marker_info>	marker_map;
			typedef std::pair<str, marker_info> marker_pair;

			marker_map markers_;

      //handlers, I do this for fun
      typedef bool (xss_generator::* tag_handler)(const str& text, param_list* args);
      typedef std::map<str, tag_handler>  handler_map;
      typedef std::pair<str, tag_handler> handler_pair;

      handler_map handlers_;

      //actual handlers
      bool handle_text(const str& text, param_list* args);
      bool handle_code(const str& text, param_list* args);
      bool handle_expression(const str& text, param_list* args);
      bool handle_class(const str& text, param_list* args);
      bool handle_file(const str& text, param_list* args);
			bool handle_marker(const str& text, param_list* args);
			bool handle_instance(const str& text, param_list* args);
  };

  typedef reference<xss_generator> XSSGenerator;
}

#endif
