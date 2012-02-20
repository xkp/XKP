
#include "xss/xss_renderer.h"
#include "xss/xss_error.h"
#include "xss/html_parser.h"

using namespace xkp;

const str SRenderer("xss");

const str SCannotEvaluate("Cannot evaluate expression");
const str SClassMustHaveARenderEvent("xss:class must have a render event");
const str SDuplicateMarker("Duplicate xss:marker");
const str SExpectingOutputAttribute("xss:file must have an output attribute");
const str SMarkerWithNoName("xss:marker must have an id");
const str SUnknownMarker("Unknown xss:marker");
const str SUnnamedInstance("xss:instance must have an id");
const str SUnnamedParameter("xss:parameter must have an id");
const str SUnknownParameterType("xss:parameter must have a valid type");
const str SBadHTML("Invalid html file");
const str SInvalidTag("HTML tag not found");

//text_renderer
text_renderer::text_renderer(const str& text):
  text_(text)
  {
  }

str text_renderer::render(XSSObject this_, param_list* args)
  {
    return text_;
  }

struct xss_code_renderer : item_renderer
  {
    //item_renderer
    virtual str render(XSSObject this_, param_list* args)
      {
        execution_context ctx(code_, this_, args);
        ctx.execute();
        return str();
      }

    void compile(const str& text, param_list* args, XSSContext ctx, fs::path file)
      {
        xs_utils     xs;
        code_context code_ctx;
        code_ctx = ctx->get_compile_context();

        code_  = xs.compile_code(text, code_ctx, file);
        code_->file = file;
      }
    private:
      ByteCode code_;
  };

struct xss_expression_renderer : item_renderer
  {
    //item_renderer
    virtual str render(XSSObject this_, param_list* args)
      {
        execution_context ctx(code_, this_, args);
        variant res = ctx.execute();

		    str result = xss_utils::var_to_string(res);
		    if (result == "@@Error")
		      {
				      param_list error;
				      error.add("id", SRenderer);
				      error.add("desc", SCannotEvaluate);
				      error.add("expression", expr_);
				      xss_throw(error);
		      }

        return result;
      }

    void compile(const str& text, param_list* args, XSSContext ctx, fs::path file)
      {
        expr_ = text;
        if (text.empty() && args)
          {
            variant vv = args->get("value");
            if (vv.empty())
					    vv = args->get("v");

            if (!vv.empty())
              expr_ = variant_cast<str>(vv, "");
          }

        xs_utils     xs;
        code_context code_ctx;
        code_ctx = ctx->get_compile_context();

        str code_str = "return " + expr_ + ";";
        code_        = xs.compile_code(code_str, code_ctx, file);
      }
    private:
      ByteCode code_;
      str      expr_;
  };

struct class_renderer : item_renderer
  {
    class_renderer() :
      instance_(new xss_object)
      {
        //register the render event
        IEditableObject* editable = variant_cast<IEditableObject*>(instance_.get(), null);

        schema_item itm;
        itm.flags = EVENT_DECL;

        editable->add_item("render", itm);
      }

    //item_renderer
    virtual str render(XSSObject this_, param_list* args)
      {
        param_list pl;
        if (!args)
          args = &pl;

        //bind values
        renderer_parameter_list::iterator pit = params_.begin();
        renderer_parameter_list::iterator pnd = params_.end();

        for(; pit != pnd; pit++)
          {
            dynamic_set(instance_, pit->id, args->get(pit->id));
          }

        instance_->dispatch_event(render_, *args);

        return str();
      }

    void compile(const str& text, param_list* args, XSSContext ctx, renderer_parameter_list& params, fs::path file)
      {
        params_ = params;

        xs_utils xs;

        //trickey, the xss parameters will be set as properties of the class,
        //so they are scope available
        renderer_parameter_list::iterator pit = params.begin();
        renderer_parameter_list::iterator pnd = params.end();

        for(; pit != pnd; pit++)
          {
            dynamic_set(instance_, pit->id, variant());
          }

        code_context code_ctx;
        code_ctx = ctx->get_compile_context();
        code_ctx.this_ = instance_;

        //what we'll really compile is an instance, but xss:class sounder classier
        xs.compile_implicit_instance(text, instance_, code_ctx, file);
        render_ = instance_->event_id("render");

        if (render_ == 0)
          {
						param_list error;
						error.add("id", SRenderer);
						error.add("desc", SClassMustHaveARenderEvent);
						xss_throw(error);
          }
      }
    private:
      DynamicObject instance_;
      size_t        render_;
      renderer_parameter_list params_;
  };

struct file_renderer : item_renderer
  {
    file_renderer(XSSCompiler compiler):
      compiler_(compiler)
      {

      }

    //item_renderer
    virtual str render(XSSObject this_, param_list* args)
      {
		    str result = renderer_->render(this_, args);
        if (output_ == "inline")
			    return result;
		    else
          {
			      compiler_->output_file(output_, result);
            return str();
          }
      }

    void compile(const str& text, param_list* args, XSSContext ctx)
      {
		    str output = variant_cast<str>(args->get("output"), "");
		    str src		 = variant_cast<str>(args->get("src"), "");

		    if (output.empty())
			    {
				    param_list error;
				    error.add("id", SRenderer);
				    error.add("desc", SExpectingOutputAttribute);
				    error.add("tag", str("xss:file"));
				    xss_throw(error);
			    }

		    str this_id = variant_cast<str>(args->get("this"), "");
		    if (!this_id.empty())
			    {
				    assert(false); //td:
			    }

		    str result;
		    if (!src.empty())
				  renderer_ = compiler_->compile_xss_file(src, ctx);
		    else
				  renderer_ = compiler_->compile_xss(text, ctx);
      }
    private:
      XSSCompiler compiler_;
      XSSRenderer renderer_;
      str         output_;
  };

//xss_renderer
xss_renderer::xss_renderer(XSSCompiler compiler, XSSContext context, fs::path xss_file):
  context_(new xss_context(context, xss_file.parent_path())),
  compiler_(compiler),
  file_(xss_file),
  busy_(false)
  {
    handlers_.insert(handler_pair("text",			&xss_renderer::handle_text));
    handlers_.insert(handler_pair("xss:code",		&xss_renderer::handle_code));
    handlers_.insert(handler_pair("xss:e",			&xss_renderer::handle_expression));
    handlers_.insert(handler_pair("xss:class",		&xss_renderer::handle_class));
    handlers_.insert(handler_pair("xss:file",		&xss_renderer::handle_file));
    handlers_.insert(handler_pair("xss:marker",		&xss_renderer::handle_marker));
    handlers_.insert(handler_pair("xss:instance",	&xss_renderer::handle_instance));
    handlers_.insert(handler_pair("xss:parameter",  &xss_renderer::handle_parameter));
  }

renderer_parameter_list& xss_renderer::params()
  {
    return params_;
  }

str xss_renderer::render(XSSObject this_, param_list* args)
  {
    XSSRenderer me(shared_from_this());
    compiler_->push_renderer(me);

    //reset first
    assert(!busy_);

    busy_   = true;
    result_ = str();

		//loop thru the compiled items
    item_list::iterator it = items_.begin();
		item_list::iterator nd = items_.end();

    std::vector<int> positions;
    for(; it != nd; it++)
      {
        positions.push_back(result_.size());
        result_ += (*it)->render(XSSObject(), args);
      }

    //sort markers by position
    std::map<int, str> by_pos;

    marker_map::iterator mit = markers_.begin();
		marker_map::iterator mnd = markers_.end();

		for(; mit != mnd; mit++)
			{
				if (mit->second.idx < 0)
					{
            continue; //td: revise
						//param_list error;
						//error.add("id", SRenderer);
						//error.add("desc", SUnknownMarker);
						//error.add("name", mit->first);
						//xss_throw(error);
					}

        by_pos.insert(std::pair<int, str>(mit->second.idx, mit->second.value));
        mit->second.value = str();
      }

    std::map<int, str>::reverse_iterator bit = by_pos.rbegin();
    std::map<int, str>::reverse_iterator bnd = by_pos.rend();

		for(; bit != bnd; bit++)
			{
        int pos = positions[bit->first];
				result_.insert(result_.begin() + pos, bit->second.begin(), bit->second.end());
			}

    compiler_->pop_renderer();

    busy_   = false;
    return result_;
  }

void xss_renderer::append(const str& what)
  {
    result_ += what;
  }

void xss_renderer::append_at(const str& what, const str& marker)
  {
	  marker_map::iterator it = markers_.find(marker);
    if (it != markers_.end())
      {
        it->second.value += what;
      }
    else
      {
				marker_info mi;
        mi.value = what;
				markers_.insert(marker_pair(marker, mi));
      }
  }

XSSContext xss_renderer::context()
  {
    return context_;
  }

fs::path xss_renderer::file()
  {
    return file_;
  }

str  xss_renderer::get()
  {
    return result_;
  }

bool xss_renderer::busy()
  {
    return busy_;
  }

void xss_renderer::handle_text(const str& text, param_list* args)
  {
    items_.push_back(ItemRenderer(new text_renderer(text)));
  }

void xss_renderer::handle_code(const str& text, param_list* args)
  {
    xss_code_renderer* result = new xss_code_renderer();
    result->compile(text, args, context_, file_);

    items_.push_back(ItemRenderer(result));
  }

void xss_renderer::handle_expression(const str& text, param_list* args)
  {
    xss_expression_renderer* result = new xss_expression_renderer();
    result->compile(text, args, context_, file_);

    items_.push_back(ItemRenderer(result));
  }

void xss_renderer::handle_class(const str& text, param_list* args)
  {
    class_renderer* result = new class_renderer();
    result->compile(text, args, context_, params_, file_);

    items_.push_back(ItemRenderer(result));
  }

void xss_renderer::handle_file(const str& text, param_list* args)
  {
    file_renderer* result = new file_renderer(compiler_);
    result->compile(text, args, context_);

    items_.push_back(ItemRenderer(result));
  }

void xss_renderer::handle_marker(const str& text, param_list* args)
  {
		variant vname = args->get("id");
		str name = variant_cast<str>(vname, str());
		if (name.empty())
			{
				param_list error;
				error.add("id", SRenderer);
				error.add("desc", SMarkerWithNoName);
				xss_throw(error);
			}

		marker_map::iterator it = markers_.find(name);
		if (it != markers_.end())
			{
				if (it->second.idx >= 0)
					{
						//duplicate marker
						param_list error;
						error.add("id", SRenderer);
						error.add("desc", SDuplicateMarker);
						error.add("name", name);
						xss_throw(error);
					}

				it->second.idx = items_.size();
			}
		else
			{
				marker_info mi;
				mi.idx = items_.size();

				markers_.insert(marker_pair(name, mi));
			}
  }

void xss_renderer::handle_instance(const str& text, param_list* args)
  {
		str id = variant_cast<str>(args->get("id"), str());
		if (id.empty())
			{
					param_list error;
					error.add("id", SRenderer);
					error.add("desc", SUnnamedInstance);
					xss_throw(error);
			}

    XSSObject     obj(new xss_object);
    DynamicObject instance(obj);

		xs_utils xs;

    code_context code_ctx;
    code_ctx = context_->get_compile_context();
    xs.compile_implicit_instance(text, instance, code_ctx, file_);

		context_->register_symbol(RESOLVE_INSTANCE, id, obj);
  }

void xss_renderer::handle_parameter(const str& text, param_list* args)
  {
    str id = variant_cast<str>(args->get("id"), str());
    if (id.empty())
	    {
			    param_list error;
			    error.add("id", SRenderer);
			    error.add("desc", SUnnamedParameter);
			    xss_throw(error);
	    }

    XSSType type = context_->get_type("var");
    str type_name = variant_cast<str>(args->get("type"), str());
    if (!type_name.empty())
      {
        type = context_->get_type(type_name);
        if (!type)
	        {
			      param_list error;
			      error.add("id", SRenderer);
			      error.add("desc", SUnknownParameterType);
			      error.add("type", type_name);
			      xss_throw(error);
	        }
      }

    params_.push_back(renderer_parameter(id, type, args->get("default")));
    context_->add_parameter(id, XSSType());
  }

//html_renderer
html_renderer::html_renderer(XSSCompiler compiler, XSSContext ctx, fs::path xss_file, const str& html_file):
  xss_renderer(compiler, ctx, xss_file),
  template_(html_file)
  {
  }

void html_renderer::visit(const str& tag, const str& text, param_list* args)
  {
    if (tag == "text")
      return; //ignore text, we'll provide 

    xss_renderer::visit(tag, text, args);
  }

str html_renderer::render(XSSObject this_, param_list* args)
  {
    str content = xss_renderer::render(this_, args);

    XSSCompiler compiler = context_->resolve("compiler");
    str html_text = compiler_->file(template_);
    
    html_parser parser;
    tag_list    tags;
    if (!parser.parse(html_text, tags))
      {
        param_list error;
        error.add("id", SRenderer);
        error.add("desc", SBadHTML);
        error.add("file", template_);
        xss_throw(error);
      }

    //now the old dance, mix & match
    typedef std::map<int, str>  mixmatch;
    typedef std::pair<int, str> mixmatch_pair;

    mixmatch m;

    tag_map::iterator tit = tasks_.begin();
    tag_map::iterator tnd = tasks_.end();

    for(; tit != tnd; tit++)
      {
        int tidx = tags.find(tit->first);
        if (tidx < 0)
          {
            param_list error;
            error.add("id", SRenderer);
            error.add("desc", SInvalidTag);
            error.add("tag", tit->first);
            xss_throw(error);
          }

        m.insert(mixmatch_pair(tidx, tit->second));
      }

    std::ostringstream result;

    //spit it out
    mixmatch::iterator it   = m.begin();
    mixmatch::iterator nd   = m.end();
    size_t             curr = 0;

    for(; it != nd; it++)
      {
        tag t = tags.get(it->first);
        if (t.closes)
          {
            param_list error;
            error.add("id", SRenderer);
            error.add("desc", SInvalidTag);
            error.add("tag", t.tag_name);
            xss_throw(error);
          }
        
        result << str(html_text.begin() + curr, html_text.begin() + t.close + 1);
        result << it->second;
        
        curr = t.close + 1;
      }

    result << str(html_text.begin() + curr, html_text.end());
    return result.str();
  }

void html_renderer::append(const str& what)
  {
    content_ += what;
  }

void html_renderer::append_at(const str& what, const str& marker)
  {
    tag_map::iterator it = tasks_.find(marker);
    if (it == tasks_.end())
      tasks_.insert(tag_map_pair(marker, what));
    else
      it->second += what;
  }

