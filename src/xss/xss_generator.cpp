
#include "xss/xss_generator.h"
#include "xss/dsl_out.h"
#include "xss/utils.h"
#include "xss/xss_error.h"
#include "xss/project.h"

#include <xs.h>


using namespace xkp;

const str SRuntime("xss-runtime");
const str SExpectingData("xss-expecting");
const str SDup("dup");

const str SCannotEvaluate("Cannot evaluate expression");
const str SExpectingOutputAttribute("Tag must contain 'output' attribute");
const str SMarkerWithNoName("Markers must contain 'name' attribute");
const str SDuplicateMarker("Duplicate marker");
const str SUnknownMarker("Unknown marker");
const str SUnnamedInstance("Instances must have id");

//xss_generator
xss_generator::xss_generator(XSSContext context):
  context_(context)
  {
    handlers_.insert(handler_pair("text",					&xss_generator::handle_text));
    handlers_.insert(handler_pair("xss:code",			&xss_generator::handle_code));
    handlers_.insert(handler_pair("xss:e",				&xss_generator::handle_expression));
    handlers_.insert(handler_pair("xss:class",		&xss_generator::handle_class));
    handlers_.insert(handler_pair("xss:file",			&xss_generator::handle_file));
    handlers_.insert(handler_pair("xss:marker",		&xss_generator::handle_marker));
    handlers_.insert(handler_pair("xss:instance",	&xss_generator::handle_instance));
	}

str xss_generator::get()
  {
		marker_map::iterator it = markers_.begin();
		marker_map::iterator nd = markers_.end();

		for(; it != nd; it++)
			{
				if (it->second.idx >= 0)
					{
						result_.insert(result_.begin() + it->second.idx, it->second.value.begin(), it->second.value.end());
					}
				else
					{
						param_list error;
						error.add("id", SRuntime);
						error.add("desc", SUnknownMarker);
						error.add("name", it->first);
						xss_throw(error);
					}
			}

    return result_;
  }

void xss_generator::append(const str& s)
  {
    result_ += s;
  }

void xss_generator::append_marker(const str& name, const str& s)
	{
		marker_map::iterator it = markers_.find(name);
		if (it != markers_.end())
			{
				it->second.value += s;
			}
		else
			{
				marker_info mi;
				mi.value = s;

				markers_.insert(marker_pair(name, mi));
			}
	}

void xss_generator::visit(const str& tag, const str& text, param_list* args)
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

bool xss_generator::handle_text(const str& text, param_list* args)
  {
    result_ += text;
    return true;
  }

bool xss_generator::handle_code(const str& text, param_list* args)
  {
    //details, details...
    trim_last_empty_line(result_);

    code_context& ctx = *(context_.get());

    xs_utils xs;
    xs.execute_xs(text, ctx);
		return true;
}

bool xss_generator::handle_expression(const str& text, param_list* args)
  {
    xs_utils xs;

    str expr = text;
    if (text.empty() && args)
      {
        variant vv = args->get("value");
        if (vv.empty())
					vv = args->get("v");

        if (!vv.empty())
          {
            expr = variant_cast<str>(vv, "");
          }
      }

    code_context& ctx = *(context_.get());

		str result = variant_cast<str>(xs.evaluate_xs_expression(expr, ctx), str("@@Error"));
		if (result == "@@Error")
		{
				param_list error;
				error.add("id", SRuntime);
				error.add("desc", SCannotEvaluate);
				error.add("expression", expr);
				xss_throw(error);
		}

    result_   += result;
    return true;
  }

bool xss_generator::handle_class(const str& text, param_list* args)
  {
    xs_utils xs;

    //what we'll really compile is an instance, but xss:class sounder classier
    DynamicObject instance(new xss_object);

    xss_composite_context ctx(context_);
    ctx.this_ = instance;

		XSSProject owner = context_->project_;
    DslLinker ol(new out_linker(owner));
    ctx.dsls.insert(dsl_list_pair("out", ol));
    ctx.dsl_ = &ctx.dsls; //td: !!! this will break under many dsls

    xs.compile_implicit_instance(text, instance, ctx);
    str entry_point = "generate";
    if (args)
      {
        variant vv = args->get("entry_point");
        if (!vv.empty())
          entry_point = variant_cast<str>(vv, "");
      }

    param_list pl;
    dynamic_exec(instance, entry_point, pl);
    return true;
  }

bool xss_generator::handle_marker(const str& text, param_list* args)
	{
		variant vname = args->get("name");
		if (vname.empty())
			{
				param_list error;
				error.add("id", SExpectingData);
				error.add("desc", SMarkerWithNoName);
				xss_throw(error);
			}

		str name = variant_cast<str>(vname, str());

		marker_map::iterator it = markers_.find(name);
		if (it != markers_.end())
			{
				if (it->second.idx >= 0)
					{
						//duplicate marker
						param_list error;
						error.add("id", SDup);
						error.add("desc", SDuplicateMarker);
						error.add("name", name);
						xss_throw(error);
					}

				it->second.idx = result_.size();
			}
		else
			{
				marker_info mi;
				mi.idx = result_.size();

				markers_.insert(marker_pair(name, mi));
			}

		return true;
	}

bool xss_generator::handle_file(const str& text, param_list* args)
	{
		XSSProject project = context_->project_;
		assert(args); //just checking

		str output = variant_cast<str>(args->get("output"), "");
		str src		 = variant_cast<str>(args->get("src"), "");

		if (output.empty())
			{
				param_list error;
				error.add("id", SExpectingData);
				error.add("desc", SExpectingOutputAttribute);
				error.add("tag", str("xss:file"));
				xss_throw(error);
			}

		XSSContext ctx = context_;

		str this_id = variant_cast<str>(args->get("this"), "");
		if (!this_id.empty())
			{
				assert(false);
			}

		str result;
		if (!src.empty())
			{
				result = project->generate_file(src, context_);
			}
		else
			{
				//td: utilify
				XSSContext safe_context(new xss_composite_context(context_));
				xss_code_context& ctx = *safe_context.get();

				XSSGenerator gen(new xss_generator(safe_context));

				project->push_generator(gen);

				project->prepare_context(ctx, gen);
				result = project->generate_xss(text, gen);

				project->pop_generator();
			}

		if (output == "inline")
			project->generator()->append(result);
		else
			project->output_file(output, result);

		return true;
	}

bool xss_generator::handle_instance(const str& text, param_list* args)
	{
		str id = variant_cast<str>(args->get("id"), "");
		if (id.empty())
			{
					param_list error;
					error.add("id", SExpectingData);
					error.add("desc", SUnnamedInstance);
					xss_throw(error);
			}

		xs_utils xs;

    //what we'll really compile is an instance, but xss:class sounder classier
    DynamicObject instance(new xss_object);

    xss_composite_context ctx(context_);
    ctx.this_ = instance;

		XSSProject owner = context_->project_;
    DslLinker ol(new out_linker(owner));
    ctx.dsls.insert(dsl_list_pair("out", ol));
    ctx.dsl_ = &ctx.dsls; //td: !!! this will break under many dsls

    xs.compile_implicit_instance(text, instance, ctx);

		context_->scope_->register_symbol(id, instance); //td: !!! redeisgn the context
    return true;
	}
