
#ifndef XSS_RENDERER
#define XSS_RENDERER

#include "xss_parser.h"
#include "xss_context.h"
#include "xss_compiler.h"

#include <xs.h>
#include <map>

namespace xkp
{
  //forwards
  struct item_renderer;

  typedef reference<item_renderer> ItemRenderer;

  //interface
  struct item_renderer
    {
      virtual str render(XSSObject this_, param_list* args) = 0;
    };

  template <typename T>
  class base_xss_renderer : public xss_visitor
    {
      public:
        //xss_visitor
        virtual void visit(const str& tag, const str& text, param_list* args)
          {
            typename handler_map::iterator it = handlers_.find(tag);
            if (it != handlers_.end())
              {
                T* this_ = dynamic_cast<T*>(this);
                (this_->*(it->second))(text, args);
              }
            else
              {
                param_list error;
                error.add("id", str("renderer"));
                error.add("desc", str("invalid xss tag"));
                error.add("tag", tag);
                xss_throw(error);
              }
          }
      protected:
        //tag handlers
        typedef void (T::* tag_handler)(const str& text, param_list* args);
        typedef std::map<str, tag_handler>  handler_map;
        typedef std::pair<str, tag_handler> handler_pair;

        handler_map handlers_;

        //item renderers
        typedef std::vector<ItemRenderer> item_list;

        item_list   items_;
    };


  class xss_renderer : public base_xss_renderer<xss_renderer>,
                       public IXSSRenderer,
                       public boost::enable_shared_from_this<xss_renderer>
    {
      public:
        xss_renderer(XSSCompiler compiler, XSSContext ctx, fs::path xss_file);
      public:
        //IXSSRenderer
        virtual renderer_parameter_list&  params();
        virtual str                       render(XSSObject this_, param_list* args);
        virtual void                      append(const str& what);
        virtual void                      append_at(const str& what, const str& marker);
        virtual XSSContext                context();
        virtual fs::path                  file();
        virtual str                       get();
        virtual bool                      busy();
      protected:
        XSSContext  context_;
        XSSCompiler compiler_;
        str         result_; //td: !!! makes the whole thing single threaded
        fs::path    file_;
        bool        busy_;

			  //markers, this simplifies the xss code in ways it didnt expect
			  struct marker_info
				  {
					  marker_info()										: idx(-1) {}
					  marker_info(const marker_info& other) : idx(other.idx), value(other.value) {}

					  str value;
					  int idx;
				  };

			  typedef std::map<str, marker_info>	marker_map;
			  typedef std::pair<str, marker_info> marker_pair;

			  marker_map markers_;

        //parameters
        renderer_parameter_list params_;

        //actual handlers
        void handle_text(const str& text, param_list* args);
        void handle_code(const str& text, param_list* args);
        void handle_expression(const str& text, param_list* args);
        void handle_class(const str& text, param_list* args);
        void handle_file(const str& text, param_list* args);
			  void handle_marker(const str& text, param_list* args);
			  void handle_instance(const str& text, param_list* args);
			  void handle_parameter(const str& text, param_list* args);
    };

  class html_renderer : public xss_renderer
    {
      public:
        html_renderer(XSSCompiler compiler, XSSContext ctx, fs::path xss_file, const str& html_file);

      public:
        //base_xss_renderer
        virtual void visit(const str& tag, const str& text, param_list* args);
      public:
        //xss_renderer
        virtual str  render(XSSObject this_, param_list* args);
        virtual void append(const str& what);
        virtual void append_at(const str& what, const str& marker);
      private:
        typedef std::map<str, str>  tag_map;
        typedef std::pair<str, str> tag_map_pair;

        str     template_;
        str     content_;
        str     dependencies_;
        str     style_;
        tag_map tasks_;
    };

  //standard renderers
  struct text_renderer : item_renderer
    {
      text_renderer(const str& text);

      //item_renderer
      virtual str render(XSSObject this_, param_list* args);
      private:
        str text_;
    };

}

#endif
