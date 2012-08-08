
#ifndef WAX_JS_HH
#define WAX_JS_HH

#include <xss/lang/js.h>
#include <xss/html_parser.h>

namespace xkp{

struct code_split;

typedef reference<code_split> CodeSplit;

struct code_split
  {
    code_split(code& _target):
      target(_target),
      split_idx(false),
      split_on_if(false),
      split_on_else(false)
      {
      }

    code       target; //td: !!! reference 
    int        split_idx;
    CodeSplit  split_code;
    XSSMethod  method;
    XSSContext context;
    str        callback;
    str        hook;
    str        error_handler;

    bool      split_on_if;
    bool      split_on_else;
  };

struct waxjs_lang : public js_lang
  {
    virtual void    init_application_context(XSSContext ctx);
    virtual variant compile_code(code& cde, param_list_decl& params, XSSContext ctx);
};

struct waxjs_code_renderer : public base_code_renderer
  {
    waxjs_code_renderer();
    waxjs_code_renderer(const waxjs_code_renderer& other);
    waxjs_code_renderer(code& cde, param_list_decl& params, XSSContext ctx);

    bool check_fork(variant owner);
    bool forked();
    void use_this_id(bool value);
    str  render_plain_code(code& cde, XSSContext ctx);

    //ICodeRenderer
    virtual str render();

    protected:
      virtual str render_expression(expression& expr, XSSContext ctx);
      virtual str render_code(code& cde);

    private:
      XSSCompiler compiler_;
      bool        checked_;
      CodeSplit   fork_;

      str render_split(CodeSplit fork, CodeSplit parent);
      str split_if(CodeSplit fork, const str& callback);
      str split_variable(CodeSplit fork, const str& code_after, const str& callback);
      str split_expression(CodeSplit fork, const str& callback);
      str split_dsl(CodeSplit fork, const str& callback);
      str split_return(CodeSplit fork);
      str split_for(CodeSplit fork, const str& callback);
      str split_iter_for(CodeSplit fork, const str& callback);
      str split_while(CodeSplit fork, const str& callback);
      str split_try(CodeSplit fork, const str& callback);
      str split_and_render(code& c, CodeSplit parent, const str& callback = str(), bool* split = null);
      str split_method(XSSMethod method, CodeSplit fork, DynamicArray args, const str& body);
      str render_callback(CodeSplit fork);
      str render_service();
      str render_page();
      str render_parameters();
      DynamicArray compile_args(expression& expr);
    public:
      XSSMethod owner_;
  };

struct wax_utils
  {
    wax_utils(); 
    wax_utils(XSSCompiler compiler);

    XSSMethod compile_page(XSSObject page, variant code);
    void      pre_process_args(XSSMethod methd);
    str       escape(const str& text);
    
    static bool check_redirect(expression& return_expr, XSSContext ctx, param_list& args);

    private:
      XSSCompiler compiler_;

      str  split_html(const str& html_text, code& cde, tag_list& tags, DynamicArray elements, int hStart, int hEnd, str& declarations);
      bool custom_modifier(XSSObject obj, const str& modifier, const str& html_text, tag_list& tags, int& tag_idx, str& result);
      str  render_html_text(const str& text);
  };

typedef reference<wax_utils> WaxUtils;

//glue
struct waxjs_code_renderer_schema : renderer_code_schema<waxjs_code_renderer>
  {
    virtual void declare()
      {
        property_<XSSMethod>    ("owner",	      &waxjs_code_renderer::owner_);
        readonly_property<bool> ("forked",	    &waxjs_code_renderer::forked);
        method_<bool, 1>        ("check_fork",  &waxjs_code_renderer::check_fork);
        method_<void, 1>        ("use_this_id", &waxjs_code_renderer::use_this_id);
      }
  };

struct wax_utils_schema : object_schema<wax_utils>
  {
    virtual void declare()
      {
        method_<XSSMethod, 2>("compile_page",      &wax_utils::compile_page);
        method_<void, 1>     ("pre_process_args",  &wax_utils::pre_process_args);
        method_<str, 1>      ("escape",            &wax_utils::escape); 
      }
  };

register_complete_type(waxjs_code_renderer, waxjs_code_renderer_schema);
register_complete_type(wax_utils, wax_utils_schema);

}

#endif