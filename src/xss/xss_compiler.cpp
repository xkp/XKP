
#include "xss/xss_compiler.h"
#include "xss/object_reader.h"
#include "xss/xss_renderer.h"
#include "xss/xss_error.h"
#include "xss/language.h"
#include "xss/lang/debug.h"
#include "xss/lang/js.h"
#include "xss/lang/java.h"
#include "xss/dsl_out.h"

#include "xs/linker.h"
#include "xs/compiler.h"

#include <boost/functional/hash.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>
#include <fstream>

using namespace xkp;

//error codes
const str SProjectError("project");
const str SCannotResolve("cannot-resolve");
const str STypeMismatch("type-mismatch");
const str SFileError("404");
const str SIdiomError("idiom");

const str SCannotReadModule("Unable to read module");
const str SMustProvideEntryPointForApplicationType("Applications must provide an entry point");
const str SOnlyClassAndEnum("Only classes and enums are allowed as types");
const str SUnnamedType("Types must have an id");
const str SCannotReadProjectFile("Types must have an id");
const str SBadInheritance("A class cannot inherit different types");
const str SDuplicateClassOnLibrary("Duplicate class");
const str SEmptyInclude("Includes must have either def or src");
const str SOnlyClassAllowedInInclude("Only classes are allowed in includes");
const str SUnknownClass("Cannot find type");
const str SIncompatibleReturnType("Return type is inconsistent");
const str SUnknownInstance("Instance not found");
const str SFileNotFound("File not found");
const str SMustProvideLanguageForApplicationType("Applications must provide a language");
const str SNotaLanguage("There is no language with this name");
const str SAppsMustHaveTarget("Application renderer (entry_point) must be present in your idiom");
const str SMustHaveProjectTarget("A target application must be specified on your project");
const str STargetNotFound("A target was not found for a type of application");
const str STypeNotFound("Cannot find type");
const str SXSSNeedsFile("compiler.xss() needs a xss file to render");
const str SInjectingEventsAtTheWrongTime("You must be rendering an application in order to inject events");
const str SInjectingEventsOnNoRenderer("You must be rendering an application in order to inject events (#app)");
const str SInjectRequiresAnEvent("You must pass an event name to compiler.inject()");
const str SCannotParseExpression("Cannot parse expression");
const str SInstanceNotFoundInIdiom("Instance not found in any idiom");
const str SUnknownImportClass("Trying to import an unknown class");
const str SCopyFileEmpty("A file was specified for copy without a file name");
const str SXSSBadOutput("compiler.xss(output_file) does not accept empty strings");
const str SEmptyMarker("compiler.xss(marker) does not accept empty strings");
const str SEmptyMarkerSource("Empty marker source");
const str SInvalidMarkerSource("Invalid marker source");
const str SNotAnInstance("Expecting instance");
const str SNotAProperty("Expecting property");
const str SAliasMustHaveAliased("Alias types must have a 'aliased' attribute");
const str SBadAliasType("Alias type not found");
const str SCannotCompileExpression("Cannot compile expression");


//xss_application_renderer
xss_application_renderer::xss_application_renderer(fs::path entry_point, Language lang, XSSCompiler compiler):
  filename_(entry_point)
  {
    context_ = XSSContext(new xss_context(XSSContext(), entry_point.parent_path()));
    context_->set_language(lang);

    //register default instances
    context_->register_symbol(RESOLVE_NATIVE, "compiler", compiler);

    XSSString xss_str(new xss_string);
    context_->register_symbol(RESOLVE_NATIVE, "String", xss_str);

    //register standard dsls
    context_->register_dsl("out", DslLinker(new out_linker(compiler)));

    //register default types
    context_->add_type("string", XSSType(new xss_type(type_schema<str>())))->set_id("string");
    context_->add_type("int",    XSSType(new xss_type(type_schema<int>())))->set_id("int");
    context_->add_type("float",  XSSType(new xss_type(type_schema<float>())))->set_id("float");
    context_->add_type("bool",   XSSType(new xss_type(type_schema<bool>())))->set_id("bool");

    XSSType variant_type(new xss_type());
    variant_type->set_id("var");
    variant_type->as_variant();
    context_->add_type("var", variant_type);

    XSSType object_type(new xss_type());
    object_type->set_id("object");
    object_type->as_variant();
    object_type->as_object();
    context_->add_type("object", object_type);

    XSSType array_type(new xss_type(type_schema<DynamicArray>()));
    array_type->set_id("array");
    array_type->as_array(XSSType(variant_type));
    context_->add_type("array", array_type)->set_id("array");
  }

XSSContext xss_application_renderer::context()
  {
    return context_;
  }

void xss_application_renderer::register_module(const str& id, XSSModule module)
  {
    context_->register_symbol(RESOLVE_INSTANCE, id, module);
    modules_.push_back(module);
  }

str xss_application_renderer::target()
  {
    return target_;
  }

fs::path xss_application_renderer::entry_point()
  {
    return filename_;
  }

fs::path xss_application_renderer::output_path()
  {
    return output_path_;
  }

void xss_application_renderer::output_path(fs::path path)
  {
    output_path_ = path;
  }

void xss_application_renderer::set_output_path(const str& path)
  {
    output_path_ = path;
  }

XSSModule xss_application_renderer::instance_idiom(XSSObject inst)
  {
    std::vector<XSSModule>::iterator it = modules_.begin();
    std::vector<XSSModule>::iterator nd = modules_.end();
    for(; it != nd; it++)
      {
        XSSModule mod = *it;
        DynamicArray instances = mod->instances();

        std::vector<variant>::iterator rit = instances->ref_begin();
        std::vector<variant>::iterator rnd = instances->ref_end();
        for(; rit != rnd; rit++)
          {
            XSSObject robj = variant_cast<XSSObject>(*rit, XSSObject());
            if (!robj)
              continue;

              if (robj == inst)
                return mod;
          }
      }

    return XSSModule();
  }

XSSModule xss_application_renderer::type_idiom(const str& type)
  {
    std::vector<XSSModule>::iterator it = modules_.begin();
    std::vector<XSSModule>::iterator nd = modules_.end();
    for(; it != nd; it++)
      {
        XSSModule mod = *it;
        if (mod->has_type(type))
          return mod;
      }

    return XSSModule();
  }

void xss_application_renderer::set_target(const str& target)
  {
    target_ = target;
  }

std::vector<XSSModule>& xss_application_renderer::modules()
  {
    return modules_;
  }

//xss_module
xss_module::xss_module()
  {
    assert(false); //should not be called
  }

xss_module::xss_module(XSSContext ctx):
  ctx_(ctx),
  instances_(new dynamic_array),
  utypes_(new dynamic_array)
  {
    ev_pprocess_ = register_event("pre_process");
    DYNAMIC_INHERITANCE(xss_module)
  }

pre_process_result xss_module::pre_process(XSSObject obj, XSSObject parent)
  {
    XSSObject ev(new xss_object());

    param_list args;
    args.add("obj", obj);
    args.add("ev",  ev);
    dispatch_event(ev_pprocess_, args);

    if (ev->get<bool>("final", false))
      return PREPROCESS_HANDLED; //canceled by user

    //we'll add sort of an arbitrary logic here
    //the objects with types belonging to this module will be logged
    //as intances. this can be used later in xss to render the proper instances
    if (one_of_us(obj))
      {
        register_instance(obj);
      }

    return PREPROCESS_KEEPGOING;
  }

DynamicArray xss_module::instances()
  {
    return instances_;
  }

fs::path xss_module::path()
  {
    return path_;
  }

void xss_module::set_path(fs::path p)
  {
    path_ = p;
  }

void xss_module::register_module_type(XSSType type)
  {
    type_list::iterator it = types_.find(type->id());
    if (it != types_.end())
			{
				param_list error;
				error.add("id", SProjectError);
				error.add("desc", SDuplicateClassOnLibrary);
				error.add("class", type->id());
				error.add("module", id());
				xss_throw(error);
			}

    types_.insert(type_list_pair(type->id(), type));
  }

void xss_module::register_user_type(XSSType type)
  {
    register_module_type(type);
    utypes_->push_back(type);
  }

bool xss_module::has_type(const str& type)
  {
    type_list::iterator it = types_.find(type);
    return it != types_.end();
  }

void xss_module::register_instance(XSSObject obj)
  {
    str obj_id = obj->id();
    if (!obj_id.empty())
      {
        ctx_->register_symbol(RESOLVE_INSTANCE, obj_id, obj);
      }

    instances_->push_back(obj);
  }

bool xss_module::one_of_us(XSSObject obj)
  {
    XSSType obj_type = obj->type();
    while(obj_type)
      {
        type_list::iterator it = types_.find(obj_type->id());
        if (it != types_.end())
          return true;

        obj_type = obj_type->get_super();
      }

    return false;
  }

DynamicArray xss_module::all_types()
  {
    DynamicArray result(new dynamic_array);
    type_list::iterator it = types_.begin();
    type_list::iterator nd = types_.end();

    for(; it != nd; it++)
      {
        result->push_back(it->second);
      }

    return result;
  }

//xss_compiler
void xss_compiler::build(fs::path xml)
  {
    fs::path pp = xml;
    base_path_ = pp.parent_path();

    project_path_ = base_path_;

    XSSObject project_data = read_project(xml);

    //options
    XSSObject options = project_data->find("options");
    if (options)
      {
        str event_scope = options->get<str>("event_scope", str("application"));
        use_event_instance_ = event_scope != "application";
      }
    else
      use_event_instance_ = false; //default

    //resolve path
    output_path_       = base_path_;
    XSSObject path_obj = project_data->find("path");
    if (path_obj)
      {
        XSSObject op = path_obj->find("output");
        if (op)
          output_path_ /= op->get<str>("path", str());
      }

    read_includes(project_data);

    str app_file = project_data->get<str>("target", str());
    read_application(app_file);

    run();

    copy_files(project_data);
  }

XSSRenderer xss_compiler::compile_xss_file(const str& src_file, XSSContext ctx)
  {
    fs::path path = ctx->path() / src_file;
    return compile_xss(load_file(path), ctx, path);
  }

XSSRenderer xss_compiler::compile_xss_file(fs::path src_file, XSSContext ctx)
  {
    compiling_ = src_file;
    XSSRenderer result = compile_xss(load_file(src_file), ctx, src_file);
    compiling_ = fs::path();
    return result;
  }

XSSRenderer xss_compiler::compile_xss(const str& src, XSSContext ctx, fs::path path)
  {
    boost::hash<std::string> string_hash;
    int hash = string_hash(src);
    
    std::multimap<int, XSSRenderer>::iterator it = xss_cache.find(hash);
    while (it != xss_cache.end() && it->first == hash)
      {
        XSSRenderer rend = it->second;
        if (!rend->busy())
          {
            //td: !!!!
            //renderers cannot be reentered, so everytime the matching renderers
            //are already being used a new one will be compiled. please fix result_ 
            return rend;
          }

        it++;
      }

    //keep cache
    xss_renderer* renderer = new xss_renderer(shared_from_this(), ctx, path);
    XSSRenderer result(renderer);
    xss_cache.insert(std::pair<int, XSSRenderer>(hash, result));

    //do the rendering
    xss_parser parser;
    parser.register_tag("xss:code");
    parser.register_tag("xss:e");
    parser.register_tag("xss:class");
    parser.register_tag("xss:file");
    parser.register_tag("xss:marker");
    parser.register_tag("xss:instance");
    parser.register_tag("xss:parameter");

		parser.parse(src, renderer);

    return result;
  }

str delete_blanks(const str& contents)
  {
    std::vector<str> strs;
    boost::split(strs, contents, boost::is_any_of("\n"));

    str result;

    std::vector<str>::iterator it = strs.begin();
    std::vector<str>::iterator nd = strs.end();
    for(; it != nd; it++)
      {
        str line = *it;
        boost::trim(line);
        if (line.empty())
          continue;

        result += *it + '\n';
      }

    return result;
  }

void xss_compiler::output_file(const str& fname, const str& contents)
  {
    fs::path path = output_path_ / fname;
    output_file(path, delete_blanks(contents));
  }

void xss_compiler::output_file(fs::path fpath, const str& contents)
  {
    fs::create_directories(fpath.parent_path());

    std::ofstream ofs(fpath.string().c_str());
    ofs << delete_blanks(contents);
    ofs.close();
  }

str xss_compiler::output_path()
  {
    return output_path_.string();
  }

str xss_compiler::project_path()
  {
    return project_path_.string();
  }

str xss_compiler::genid(const str& what)
  {
		genid_list::iterator ait  = genid_.find(what);
		int									 aidx = 1;
		if (ait == genid_.end())
			{
				genid_.insert( std::pair<str, int>(what, aidx ));
			}
		else
			{
				aidx = ++ait->second;
			}

		return "__" + what + boost::lexical_cast<str>(aidx);
  }

void xss_compiler::xss_args(const param_list params, param_list& result, fs::path& output_file, str& marker, MARKER_SOURCE& marker_source)
  {
    for(size_t i = 0; i < params.size(); i++)
      {
        str     pname = params.get_name(i);
        variant value = params.get(i);

        if (pname == "output_file")
          {
            str vv = variant_cast<str>(value, str());
            if (vv.empty())
              {
                param_list error;
                error.add("id", SProjectError);
                error.add("desc", SXSSBadOutput);
                xss_throw(error);
              }

            output_file = output_path_ / vv;
            continue;
          }
        else if (pname == "marker")
          {
            str vv = variant_cast<str>(value, str());
            if (vv.empty())
              {
                param_list error;
                error.add("id", SProjectError);
                error.add("desc", SEmptyMarker);
                xss_throw(error);
              }

            marker = vv;
            continue;
          }
        else if (pname == "marker_source")
          {
            str vv = variant_cast<str>(value, str());
            if (vv.empty())
              {
                param_list error;
                error.add("id", SProjectError);
                error.add("desc", SEmptyMarkerSource);
                xss_throw(error);
              }
            else if (vv == "entry")
              {
                marker_source = MS_ENTRY;
              }
            else if (vv == "current")
              {
                marker_source = MS_CURRENT;
              }
            else
              {
                param_list error;
                error.add("id", SProjectError);
                error.add("desc", SInvalidMarkerSource);
                error.add("value", vv);
                xss_throw(error);
              }

            continue;
          }

        result.add(pname, value);
      }
  }

void xss_compiler::xss(const param_list params)
  {
    if (params.size() == 0)
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SXSSNeedsFile);
        xss_throw(error);
      }

    str file_name = variant_cast<str>(params.get(0), str());
    if (file_name.empty())
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SXSSNeedsFile);
        xss_throw(error);
      }

    //grab options, leave the rest of the arguments intact
    param_list    my_args;
    fs::path      the_output_file;
    str           marker;
    MARKER_SOURCE marker_source = MS_CURRENT;
    xss_args(params, my_args, the_output_file, marker, marker_source);

    //resolve file name
    XSSRenderer r   = current_renderer();
    XSSContext  ctx = r->context();

    fs::path file(file_name);
    if (!file.is_complete())
      {
        file = vm::instance().file();
        if (file.empty())
          file = ctx->path() / file_name;
        else
          file = file.parent_path() / file_name;
      }

    XSSRenderer result = compile_xss_file(file, ctx);

    //match the parameters
    renderer_parameter_list& result_args = result->params();
    renderer_parameter_list::iterator it = result_args.begin();
    renderer_parameter_list::iterator nd = result_args.end();

    size_t     param_count = my_args.size();
    size_t     curr_param  = 1; //the first parameter is taken
    param_list result_params;   //what to pass to the xss
    for(; it != nd; it++)
      {
        if (my_args.has(it->id))
          {
            result_params.add(it->id, my_args.get(it->id));
          }
        else if (curr_param < param_count)
          {
            bool named;
            do
              {
                named = !my_args.get_name(curr_param).empty();
                if (named)
                  curr_param++;
              }
            while(named);

            if (curr_param < param_count)
              result_params.add(it->id, my_args.get(curr_param++));
            else
              result_params.add(it->id, it->default_value);
          }
        else
          {
            result_params.add(it->id, it->default_value);
          }
      }

    if (the_output_file.empty())
      {
        if (!marker.empty())
          r->append_at(result->render(XSSObject(), &result_params), marker);
        else
          {
            switch (marker_source)
              {
                case MS_ENTRY:
                  {
                    assert(entry_);
                    entry_->append(result->render(XSSObject(), &result_params));
                    break;
                  }
                case MS_CURRENT:
                  {
                    r->append(result->render(XSSObject(), &result_params));
                    break;
                  }
              }
          }
      }
    else
      output_file(the_output_file, result->render(XSSObject(), &result_params));
  }

void xss_compiler::inject(const param_list params)
  {
    XSSRenderer cr = current_renderer();
    if (!cr)
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SInjectingEventsAtTheWrongTime );
        xss_throw(error);
      }

    XSSContext             ctx = cr->context();
    XSSApplicationRenderer renderer = variant_cast<XSSApplicationRenderer>(ctx->resolve("#app", RESOLVE_CONST), XSSApplicationRenderer());

    if (!renderer)
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SInjectingEventsOnNoRenderer );
        xss_throw(error);
      }

    //disptach events to all modules, aka injecting
    str evname = variant_cast<str>(params.get(0), str());
    if (evname.empty())
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SInjectRequiresAnEvent );
        xss_throw(error);
      }

    param_list pl;
    for(size_t i = 1; i < params.size(); i++)
      pl.add(params.get(i));

    std::vector<XSSModule> modules = renderer->modules();
    std::vector<XSSModule>::iterator it = modules.begin();
    std::vector<XSSModule>::iterator nd = modules.end();
    for(; it != nd; it++)
      {
        XSSModule mod = *it;
        size_t evid = mod->event_id(evname);
        if (evid > 0)
          mod->dispatch_event(evid, pl);
      }
  }

void xss_compiler::log(const param_list params)
	{
    for(int i = (int)params.size() - 1; i >= 0; i--)
      {
        str param_name = params.get_name(i);
        variant value  = params.get(i);

        str string_value = xss_utils::var_to_string(value);
				if (value.empty())
					{
            string_value = "null";
					}
				else if (string_value.empty())
					{
						XSSObject obj_value    = variant_cast<XSSObject>(value, XSSObject());

						str	obj_id;
						if (obj_value)
							{
								obj_id = variant_cast<str>(dynamic_get(obj_value, "id"), str(""));
								string_value = "Object with id: " + obj_id;
							}
					}

				std::cout << "Log: " << string_value << '\n';
      }
	}

void xss_compiler::error(const param_list params)
  {
    str desc;
    if (!params.empty())
      desc = variant_cast<str>(params.get(0), str());

		param_list error;
		error.add("id", SIdiomError);
		error.add("desc", desc);

    for(size_t i = 1; i < params.size(); i++)
    {
		  str pname = params.get_name(i);
      error.add(pname, params.get(i));
    }

		xss_throw(error);
  }

bool xss_compiler::parse_expression(variant v)
	{
		if (!v.is<str>())
			return false;

		str s = variant_cast<str>(v, str());
		xs_compiler compiler;
		expression expr;
		return compiler.compile_expression(s, expr);
	}

variant xss_compiler::compile_expression(const str& expr)
  {
    XSSContext ctx  = current_context();
    Language   lang = ctx->get_language();

    xs_utils   xs;
    expression result;
    if (!xs.compile_expression(expr, result))
      {
				param_list error;
				error.add("id", SProjectError);
				error.add("desc", SCannotCompileExpression);
				error.add("expression", expr);

				xss_throw(error);
      }

    return lang->compile_expression(result, ctx); 
  }

str xss_compiler::render_expression(const str& expr, XSSObject this_)
	{
		xs_utils	 xs;
		expression e;
		if (!xs.compile_expression(expr, e))
			{
				param_list error;
				error.add("id", SProjectError);
				error.add("desc", SCannotParseExpression);
				error.add("expression", expr);

				xss_throw(error);
			}

    XSSContext ctx  = current_context();
    Language   lang = ctx->get_language();

    //compile
    XSSContext my_ctx(new xss_context(ctx));
		my_ctx->set_this(this_);

    variant ev = lang->compile_expression(e, my_ctx);
    //IXSSRenderer* rend = variant_cast<IXSSRenderer*>(ev, null); assert(rend);
    IExpressionRenderer* rend = variant_cast<IExpressionRenderer*>(ev, null); assert(rend);

    //and render
		//return rend->render(this_, null);
    return rend->render();
	}

str xss_compiler::replace_identifier(const str& s, const str& src, const str& dst)
	{
		str    result;
		size_t curr = 0;
		size_t pos = s.find(src);
		while(pos != str::npos)
			{
				result += s.substr(curr, pos - curr) + dst;
				curr = pos + src.size();
				pos = s.find(src, curr);
			}

		result += s.substr(curr, s.size() - curr);
		return result;
	}

variant xss_compiler::resolve_property(const str& prop, variant parent)
	{
    XSSContext ctx = current_context();
    XSSObject  base;
		str        path_str;
		if (parent.is<str>())
			{
				str          parent_id = variant_cast<str>(parent, str());
        resolve_info parent_ri;
        parent_ri.search_this = false;
        parent_ri.output      = &path_str;
        if (ctx->resolve_path(lang_utils::unwind(parent_id), parent_ri))
          {
            if (parent_ri.what != RESOLVE_INSTANCE)
              {
                param_list error;
                error.add("id", SProjectError);
                error.add("desc", SNotAnInstance);
                error.add("searching for", prop);
                xss_throw(error);
              }

              base = parent_ri.value;
          }
        else
          {
            param_list error;
            error.add("id", SProjectError);
            error.add("desc", SCannotResolve);
            error.add("what", prop);
            xss_throw(error);
          }
			}
		else
			base = variant_cast<XSSObject>(parent, XSSObject());

    ctx = XSSContext(new xss_context(ctx));
    ctx->set_this(base);

    str path;
    resolve_info ri;
    resolve_info left;
    ri.output = &path;
    if (base)
      {
        left.what = RESOLVE_INSTANCE;
        left.value = base;
        ri.left = &left;
      }

    XSSObject   result(new xss_object);
    std::vector<str> prop_path = lang_utils::unwind(prop);
		if (ctx->resolve_path(prop_path, ri))
      {
        if (ri.what != RESOLVE_PROPERTY)
          {
            param_list error;
            error.add("id", SProjectError);
            error.add("desc", SNotAProperty);
            error.add("what", prop);
            xss_throw(error);
          }

        XSSProperty prop = ri.value;
        result->add_attribute("prop", prop);
		    result->add_attribute("prop_name", prop->id());

        if (!path_str.empty())
          {
            path = path_str + ctx->get_language()->resolve_separator() + path;
          }
      }
		else
      {
			  result->add_attribute("prop", variant());
			  result->add_attribute("prop_name", prop_path[prop_path.size() - 1]);
      }

		result->add_attribute("request", prop);
		result->add_attribute("path", path);
    result->add_attribute("this_property", ri.found_this);

		return result;
	}

variant xss_compiler::evaluate_property(XSSProperty prop)
  {
    if (prop)
      { 
        return prop->eval(current_context());
      }

    return variant();
  }

str xss_compiler::renderer_file(const str& file)
  {
    XSSRenderer r = current_renderer();
    fs::path    p = fs::system_complete(r->context()->path() / file);

    return p.string();
  }

str xss_compiler::idiom_path(XSSObject obj, const str& file)
  {
    XSSContext             ctx = current_context();
    XSSApplicationRenderer app = ctx->resolve("#app");

    XSSModule idiom = app->instance_idiom(obj);
    if (!idiom)
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SInstanceNotFoundInIdiom);
        error.add("object", obj->id());
        xss_throw(error);
      }

    fs::path p = idiom->path() / file;
    return p.string();
  }

str xss_compiler::full_path(const str& file)
  {
    fs::path pth = vm::instance().file();

    fs::path file_pth = pth.parent_path() / file;
    return file_pth.string();
  }

fs::path xss_compiler::compiling()
  {
    return compiling_;
  }

void xss_compiler::copy_file(const str& src_file, const str& dst_file)
  {
    if (src_file.empty())
      {
				param_list error;
				error.add("id", SProjectError);
				error.add("desc", SCopyFileEmpty);

				xss_throw(error);
      }

    //fs::path src = base_path_ / src_file;
    fs::path src = src_file;

    if (!fs::exists(src))
      {
				param_list error;
				error.add("id", SProjectError);
				error.add("desc", SFileNotFound);
				error.add("file", src.string());

				xss_throw(error);
      }

    //do output
    fs::path dst = output_path_ / dst_file;

    fs::path dst_path = dst;
    bool is_directory = dst_file[dst_file.size() - 1] == '/'; //td: !!!
    if (!is_directory)
      dst_path = dst.parent_path();
    else
      dst /= src.filename();

    bool do_copy = true;
    if (fs::exists(dst))
      {
        std::time_t src_modified = boost::filesystem::last_write_time( src ) ;
        std::time_t dst_modified = boost::filesystem::last_write_time( dst ) ;

        do_copy = src_modified != dst_modified;
      }

    if (do_copy)
      {
        dst_path = fs::system_complete(dst_path);
        fs::create_directories(dst_path);
        fs::copy_file(src, dst, fs::copy_option::overwrite_if_exists);
      }
  }

void xss_compiler::out(variant w)
  {
    XSSRenderer rend = current_renderer();
    rend->append(xss_utils::var_to_string(w));
  }

XSSType xss_compiler::get_type(const str& type)
  {
    XSSContext ctx = current_context();
    return ctx->get_type(type);
  }

XSSType xss_compiler::type_of(variant v)
  {
    XSSObject obj = variant_cast<XSSObject>(v, XSSObject());
    if (obj)
      return obj->type();
    
    XSSContext ctx = current_context();
    return ctx->get_type(v.get_schema());
  }

void xss_compiler::push_renderer(XSSRenderer renderer)
  {
    if (renderers_.empty())
      entry_ = renderer;

    renderers_.push(renderer);
  }

void xss_compiler::pop_renderer()
  {
    renderers_.pop();

    if (renderers_.empty())
      entry_ = XSSRenderer();
  }

XSSRenderer xss_compiler::current_renderer()
  {
    if(renderers_.empty())
      return XSSRenderer();
    return renderers_.top();
  }

XSSRenderer xss_compiler::entry_renderer()
  {
    return entry_;
  }

XSSObject xss_compiler::read_project(fs::path xml_file)
  {
    xss_object_reader reader;
    XSSObject project_data = reader.read(load_file(xml_file));
    if (!project_data)
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SCannotReadProjectFile);
        xss_throw(error);
      }

    //we need to find out how many different types of applications we'll be compiling
    //it would be able to load a single application type without the extra syntax
    std::vector<XSSObject> applications = project_data->find_by_class("application");
    if (applications.empty())
        applications.push_back(project_data);

    read_application_types(applications);

    return project_data;
  }

void xss_compiler::read_application_types(std::vector<XSSObject> & applications)
  {
    std::vector<XSSObject>::iterator it = applications.begin();
    std::vector<XSSObject>::iterator nd = applications.end();

    for(; it != nd; it++)
      {
        XSSObject app_data = *it;

        //get the appropiate data
        str entry_point = app_data->get<str>("entry_point", str());
        if (entry_point.empty())
          {
            param_list error;
            error.add("id", SProjectError);
            error.add("desc", SMustProvideEntryPointForApplicationType);
            xss_throw(error);
          }

        str language_name = app_data->get<str>("language", str());
        if (language_name.empty())
          {
            param_list error;
            error.add("id", SProjectError);
            error.add("desc", SMustProvideLanguageForApplicationType);
            xss_throw(error);
          }

        Language lang = get_language(language_name);
        if (!lang)
          {
            param_list error;
            error.add("id", SProjectError);
            error.add("desc", SNotaLanguage);
            error.add("language", language_name);
            xss_throw(error);
          }

        fs::path path = fs::complete(base_path_ / entry_point);
        XSSApplicationRenderer app(new xss_application_renderer(path, lang, shared_from_this()));

        app->context()->register_symbol(RESOLVE_CONST, "#app", app);

        fs::path op = fs::path(app_data->get<str>("output", str()));
        app->output_path(op);

        options_ = app_data->get<XSSObject>("options", XSSObject());

        //load modules 
        std::vector<XSSObject> modules = app_data->find_by_class("idiom");
        std::vector<XSSObject>::iterator it = modules.begin();
        std::vector<XSSObject>::iterator nd = modules.end();

        for(; it != nd; it++)
          {
            XSSObject module = *it;
            str src = module->get<str>("src", str());

            XSSModule compiled = read_module(src, app, module);

            app->register_module(compiled->id(), compiled);
          }

        applications_.push_back(app);
      }
  }

XSSModule xss_compiler::read_module(const str& src, XSSApplicationRenderer app, XSSObject module)
  {
    fs::path path = base_path_ / src;
    XSSContext ctx(new xss_context(app->context(), path.parent_path()));

    xss_object_reader reader(ctx);
    reader.enforce_types(false);

    XSSObject module_data = reader.read(load_file(path));

    if (!module_data)
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SCannotReadModule);
        xss_throw(error);
      }

    XSSModule result(new xss_module(app->context()));
    result->copy(module_data);

    result->set_path(path.parent_path());

    //read types
    read_types(module, app, result);

		//and code, if present
    str code_file = module_data->get<str>("src", str());
    if (!code_file.empty())
      {
        xs_utils xs;

        code_context code_ctx;
        code_ctx = ctx->get_compile_context();
        fs::path file = ctx->path() / code_file;
        xs.compile_implicit_instance(load_file(file), DynamicObject(result), code_ctx, file);
      }

    return result;
  }

void xss_compiler::read_types(XSSObject module_data, XSSApplicationRenderer app, XSSModule module)
  {
    XSSObject  types = module->find("types");
    XSSContext ctx   = app->context();
    if (types)
      {
        for(int i = 0; i < types->children()->size(); i++)
          {
            XSSObject type_data = types->children()->at(i);
            str       type_name = type_data->id();
            if (type_name.empty())
              {
                param_list error;
                error.add("id", SProjectError);
                error.add("desc", SUnnamedType);
                xss_throw(error);
              }

            XSSType super;
            if (type_data->has("super"))
              {
                str super_name = type_data->get<str>("super", str());
                super = ctx->get_type(super_name);
                if (!super)
                  {
                    param_list error;
                    error.add("id", SProjectError);
                    error.add("desc", SUnknownClass);
                    error.add("class", super_name);
                    xss_throw(error);
                  }
              }

            XSSType type(new xss_type());
            type->set_id(type_data->id());
            type->set_super(super);
            type->set_output_id(type_data->output_id());
            type->set_definition(type_data);
            type->inherit();

            str class_name = type_data->type_name();
            bool alias = false; 
            if (class_name == "class")
              {
                //look for imports
                XSSObjectList imports = type_data->find_by_class("import");
                XSSObjectList::iterator it = imports.begin();
                XSSObjectList::iterator nd = imports.end();

                for(; it != nd; it++)
                  {
                    XSSObject import = *it;
                    XSSType   itype  = ctx->get_type(import->id());
                    if (!itype)
                      {
                        param_list error;
                        error.add("id", SProjectError);
                        error.add("desc", SUnknownImportClass);
                        error.add("class", import->id());
                        xss_throw(error);
                      }

                    type->copy(XSSObject(itype)); //td: will override, maybe not the desired effect
                  }
              }
            else if (class_name == "enum")
              {
                type->as_enum();
              }
            else if (class_name == "alias")
              {
                alias = true;
                
                str aliased = type->get<str>("aliased", str());
                if (aliased.empty())
                  { 
                    param_list error;
                    error.add("id", SProjectError);
                    error.add("desc", SAliasMustHaveAliased);
                    error.add("alias", type->id());
                    xss_throw(error);
                  }

                type = ctx->get_type(aliased);
                if (!type)
                  { 
                    param_list error;
                    error.add("id", SProjectError);
                    error.add("desc", SBadAliasType);
                    error.add("type", aliased);
                    xss_throw(error);
                  }
              }
            else
              {
                param_list error;
                error.add("id", SProjectError);
                error.add("desc", SOnlyClassAndEnum);
                xss_throw(error);
              }

            //resolve the unresolved
		        DynamicArray props = type->properties();
		        std::vector<variant>::iterator pit = props->ref_begin();
		        std::vector<variant>::iterator pnd = props->ref_end();

            for(; pit != pnd; pit++)
              {
                XSSProperty prop     = *pit;
                XSSType     old_type = prop->type();
                if (old_type->is_unresolved())
                  {
                    XSSType new_type = ctx->get_type(old_type->id());
                    if (!new_type)
                      {
                        param_list error;
                        error.add("id", SProjectError);
                        error.add("desc", SUnknownClass);
                        error.add("class", old_type->id());
                        xss_throw(error);
                      }

                      prop->set_type(new_type);
                  }
              }

            //and register
            if (!alias)
              module->register_module_type(type);
            
            ctx->add_type(type_name, type);
          }
      }
  }

void xss_compiler::read_includes(XSSObject project_data)
  {
    std::vector<XSSObject>           includes = project_data->find_by_class("include");
    std::vector<XSSObject>::iterator it       = includes.begin();
    std::vector<XSSObject>::iterator nd       = includes.end();

    for(; it != nd; it++)
      {
        XSSObject include  = *it;
        str       xml_file = include->get<str>("def", str());
        str       src_file = include->get<str>("src", str());
        str       target   = include->get<str>("target", str());
        str       inc_app  = include->get<str>("application", str());

        //preconditions
        if (xml_file.empty() && src_file.empty())
          {
            param_list error;
            error.add("id", SProjectError);
            error.add("desc", SEmptyInclude);
            xss_throw(error);
          }

        //the native types will be repeated across every application,
        //a little pain now saves a lot of pain later
        std::vector<XSSApplicationRenderer>::iterator it = applications_.begin();
        std::vector<XSSApplicationRenderer>::iterator nd = applications_.end();
        for(;it != nd; it++)
          {
            XSSApplicationRenderer app = *it;
            if (app->target() == target || app->id() == inc_app)
              {
                read_include(base_path_ / xml_file, base_path_ / src_file, app->context(), app);
              }
          }
	    }
  }

void xss_compiler::read_include(fs::path def, fs::path src, XSSContext ctx, XSSApplicationRenderer app)
  {
    std::map<str, XSSObject> def_types;
    fs::path path;
    if (!def.empty())
      {
        path = def;

        std::vector<XSSObject> classes_data;
        read_object_array(def, ctx, classes_data);

        std::vector<XSSObject>::iterator cit = classes_data.begin();
        std::vector<XSSObject>::iterator cnd = classes_data.end();

        for(; cit != cnd; cit++)
          {
            XSSObject clazz_data = *cit;

            str cid = clazz_data->id();

						std::map<str, XSSObject>::iterator dcit = def_types.find(cid);
						if (dcit != def_types.end())
							{
								param_list error;
								error.add("id", SProjectError);
								error.add("desc", SDuplicateClassOnLibrary);
								error.add("class", cid);
								xss_throw(error);
							}

						def_types.insert(std::pair<str, XSSObject>(cid, clazz_data));
          }
      }

    if (!src.empty())
      {
        if (path.empty())
          path = src;

        xs_container results;
		    compile_xs_file(src, results);

        for(size_t i = 0; i < results.size(); i++)
          {
				    variant vv = results.get(i);
				    if (!vv.is<xs_class>())
					    {
						    //what kinda class are you?
						    param_list error;
						    error.add("id", SProjectError);
						    error.add("desc", SOnlyClassAllowedInInclude);
						    xss_throw(error);
					    }

				    xs_class ci  = vv;
            str      cid = ci.name;

            //look for a declaration on the xml file
            XSSObject def_class;
            std::map<str, XSSObject>::iterator dtit = def_types.find(cid);
            if (dtit != def_types.end())
              {
                def_class = dtit->second;
                def_types.erase(dtit);
              }

						XSSType super;
				    if (!ci.super.empty())
					    {
                str def_super = def_class->get<str>("super", str());
                if (!def_super.empty() && def_super != ci.super)
							    {
								    param_list error;
								    error.add("id", SProjectError);
								    error.add("desc", SBadInheritance);
								    error.add("class", ci.super);
								    xss_throw(error);
							    }

						    super = ctx->get_type(ci.super);
						    if (!super)
							    {
								    param_list error;
								    error.add("id", SProjectError);
								    error.add("desc", SUnknownClass);
								    error.add("class", ci.super);
								    xss_throw(error);
							    }
					    }

				    XSSType clazz(new xss_type());
            clazz->set_id(cid);
            clazz->set_definition(def_class);
            clazz->set_super(super);

				    XSSContext ictx(new xss_context(ctx, path.parent_path()));
				    ictx->set_this(XSSObject(clazz));

				    compile_ast(ci, ictx);

            XSSModule module = app->type_idiom(ci.super);
            if (module)
              module->register_user_type(clazz);

            ctx->add_type(cid, clazz);
          }
			}

    //process the left over classes
    std::map<str, XSSObject>::iterator it = def_types.begin();
    std::map<str, XSSObject>::iterator nd = def_types.end();
    for(; it != nd; it++)
      {
				XSSType clazz(new xss_type());
        clazz->set_definition(it->second);

        str super = it->second->get<str>("super", str());
        if (!super.empty())
          {
            XSSType super_clazz = ctx->get_type(super);
            if (!super_clazz)
              {
								param_list error;
								error.add("id", SProjectError);
								error.add("desc", SUnknownClass);
								error.add("class", super);
								xss_throw(error);
              }

            clazz->set_super(super_clazz);
          }

        XSSModule module = app->type_idiom(super);
        if (module)
          module->register_user_type(clazz);

        ctx->add_type(it->first, clazz);
      }
  }

void xss_compiler::read_application(const str& app_file)
  {
    if (app_file.empty())
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SMustHaveProjectTarget);
        xss_throw(error);
      }

    fs::path app_path = base_path_ / app_file;

    //every application type will process a fresh copy of the application,
    //hence it will be parsed multiple times. A simple clone will save this operation
    //quite some ticks. Nonetheless I will do some minimal caching
    str          def           = load_file(app_path);
    bool         code_compiled = false;
    xs_container code;

    std::vector<XSSApplicationRenderer>::iterator it = applications_.begin();
    std::vector<XSSApplicationRenderer>::iterator nd = applications_.end();

    for(; it != nd; it++)
      {
        XSSApplicationRenderer app_renderer = *it;

        //read the application
        xss_object_reader reader(app_renderer->context());
        XSSObject app_data = reader.read(def);

        //make sure we match the app's target
        str target = app_renderer->target();
        if (!target.empty())
          {
            app_data = app_data->find(target);
            if (!app_data)
              {
                param_list error;
                error.add("id", SProjectError);
                error.add("desc", STargetNotFound);
                error.add("target", target);
                xss_throw(error);
              }
          }

        //pre process, basically the application will be traversed and
        //notified to the application modules. From then on it is their business
        //the expected result is an application object ready for rendering.
        current_app_ = app_renderer;
        pre_process(app_renderer, app_data, XSSObject());
        current_app_.reset();

        str src = app_data->get<str>("src", str());
        fs::path src_path = base_path_ / src;
        if (!src.empty() && !code_compiled)
          {
		        compile_xs_file(src_path, code);
            code_compiled = true;
          }

        app_data->set_id("application");
        app_renderer->context()->register_symbol(RESOLVE_INSTANCE, "application", app_data);

        XSSContext code_ctx(new xss_context(app_renderer->context(), src_path));
        code_ctx->set_this(app_data);

        compile_ast(code, code_ctx);

        //hook modules to the application
        std::vector<XSSModule> modules = app_renderer->modules();
        std::vector<XSSModule>::iterator it = modules.begin();
        std::vector<XSSModule>::iterator nd = modules.end();

        for(; it != nd; it++)
          {
            XSSModule module = *it;
            //XSSObject module = *it;
            dynamic_set(module, "application", app_data);
          }
      }
  }

void xss_compiler::compile_ast(xs_container& ast, XSSContext ctx)
	{
    source_collector gather;
    ast.visit(&gather);

		XSSObject instance = ctx->get_this();

    Language lang = ctx->get_language();

		DynamicArray properties = instance->properties();
		DynamicArray methods    = instance->methods();

    std::vector<xs_property>::iterator pit = gather.properties.begin();
    std::vector<xs_property>::iterator pnd = gather.properties.end();
    for(; pit != pnd; pit++)
      {
        variant getter;
        variant setter;
        if (!pit->get.empty())
					{
						param_list_decl args;

						getter = lang->compile_code(pit->get, args, ctx);
					}

        if (!pit->set.empty())
					{
						param_list_decl args;
						param_decl			arg;
						arg.name = "value";
						arg.type = pit->type;
						args.push_back(arg);
						setter = lang->compile_code(pit->set, args, ctx);
					}

        variant value;
        XSSType value_type;
        if (!pit->value.empty())
          {
            value = lang->compile_expression(pit->value, ctx);
            IExpressionRenderer* er = variant_cast<IExpressionRenderer*>(value, null); assert(er);
            value_type = er->type();
          }

        XSSType prop_type = value_type;
        if (!pit->type.empty())
          {
            prop_type = ctx->get_type(pit->type);
            if (!prop_type)
              {
                param_list error;
                error.add("id", SProjectError);
                error.add("desc", STypeNotFound);
                error.add("type", pit->type);
                xss_throw(error);
              }
          }

        if (!prop_type)
          {
            //we do not use empty types, a property without type is variant
            prop_type = ctx->get_type("var");
          }

        assert(prop_type);
				XSSProperty new_prop(new xss_property(pit->name, prop_type, value, getter, setter, instance));
        properties->push_back(new_prop); //td: check types with class
      }

    std::vector<xs_method>::iterator mit = gather.methods.begin();
    std::vector<xs_method>::iterator mnd = gather.methods.end();
    for(; mit != mnd; mit++)
      {
        XSSType decl_type   = ctx->get_type(mit->type);
        XSSType return_type = decl_type;

				if (!decl_type)
					decl_type = return_type;

				if (!mit->type.empty() && decl_type != return_type)
					{
						param_list error;
						error.add("id", STypeMismatch);
						error.add("desc", SIncompatibleReturnType);
						error.add("declared as", mit->type);
						error.add("returns", return_type->output_id());

						xss_throw(error);
					}

				variant args = lang->compile_args(mit->args, ctx);             assert(!args.empty());
				variant cde  = lang->compile_code(mit->cde, mit->args, ctx);   assert(!cde.empty());

				XSSMethod mthd(new xss_method(mit->name, return_type, args, cde));
        methods->push_back(mthd); //td: !!! inheritance!
      }

    xs_event_list::iterator it = gather.events.begin();
    xs_event_list::iterator nd = gather.events.end();
    for(; it != nd; it++)
      {
        XSSObject actual_instance = instance;
        str inst_name;
				str complete_name;
        for(size_t idx = 0; idx < it->name.size() - 1 && instance; idx++)
          {
            inst_name = it->name[idx];
            if (idx == 0)
              {
								complete_name = inst_name;

                variant iv = ctx->resolve(inst_name, RESOLVE_INSTANCE);
                if (iv.empty())
                  {
										param_list error;
										error.add("id", SCannotResolve);
										error.add("desc", SUnknownInstance);
										error.add("instance", complete_name);

										xss_throw(error);
                  }

                actual_instance = iv;
              }
            else
              {
                complete_name += ctx->get_language()->resolve_separator() + inst_name;
								variant vv;
                if (dynamic_try_get(actual_instance, inst_name, vv))
                  {
                    actual_instance = variant_cast<XSSObject>(vv, XSSObject());
                  }
                else
									{
										param_list error;
										error.add("id", SCannotResolve);
										error.add("desc", SUnknownInstance);
										error.add("instance", complete_name);

										xss_throw(error);
									}
              }
          }

        if (!actual_instance)
					{
            param_list error;
            error.add("id", SCannotResolve);
            error.add("desc", SUnknownInstance);
            error.add("instance", complete_name);

            xss_throw(error);
					}

        str event_name = it->name[it->name.size() - 1];

				XSSEvent		 ev;
        DynamicArray impls = actual_instance->get_event_impl(event_name, ev);

				//let the idiom process implementations
				XSSContext ictx(new xss_context(ctx));
        if (use_event_instance_)
          ictx->set_this(actual_instance);
        else
				  ictx->set_this(instance); 

        variant impl = lang->compile_code(it->cde, it->args, ictx);
				impls->push_back(impl);

				ev->args = lang->compile_args(it->args, ctx);
      }

		std::vector<xs_instance>::iterator iit = gather.instances.begin();
		std::vector<xs_instance>::iterator ind = gather.instances.end();
		for(; iit != ind; iit++)
			{
				xs_instance& instance_ast = *iit;
        XSSObject instance_instance;

        resolve_info ri;
        ri.what = RESOLVE_INSTANCE;
        if (ctx->resolve_path(instance_ast.id, ri))
          {
            instance_instance = ri.value;

				    XSSContext ictx(new xss_context(ctx));
				    ictx->set_this(instance_instance);
				    compile_ast(instance_ast, ictx);
          }
        else
					{
            param_list error;
            error.add("id", SCannotResolve);
            error.add("desc", SUnknownInstance);
            error.add("instance", lang_utils::wind(instance_ast.id));

            xss_throw(error);
					}
			}
	}

void xss_compiler::read_object_array(fs::path file, XSSContext ctx, std::vector<XSSObject>& classes_data)
  {
    xss_object_reader reader(ctx);
    classes_data = reader.read_array(load_file(file));
  }

void xss_compiler::compile_xs_file(fs::path file, xs_container& result)
  {
    xs_compiler compiler;
		compiler.compile_xs(load_file(file.string()), result); //td: errors
  }

str xss_compiler::load_file(fs::path fname)
  {
    fs::path scp = fs::system_complete(fname);
    bool exists = fs::exists(fname);
    //td: please do this seriously
    std::ifstream ifs(fname.string().c_str());
		if (!ifs.good())
			{
				param_list error;
				error.add("id", SFileError);
				error.add("desc", SFileNotFound);
				error.add("file", fname.string());

				xss_throw(error);
			}

    str result;
    char buffer[1024];
    while(ifs.good())
      {
        ifs.getline(buffer, 1024);
        result += buffer;
        result += '\n';
      }

    return result;
  }

bool xss_compiler::options(const str& name)
	{
    if (options_ && options_->has(name))
      {
        return variant_cast<bool>(options_->get(name, variant()), false);
      }
		return false;
	}

Language xss_compiler::get_language(const str& name)
  {
    //look the other way ma
    if (name == "js")
      return Language(new js_lang());
    else if (name == "java")
      return Language(new java_lang());
    else if (name == "debug")
      return Language(new debug_language);

    return Language();
  }

void xss_compiler::pre_process(XSSApplicationRenderer renderer, XSSObject obj, XSSObject parent)
  {
    str type = obj->type_name();

    XSSModule module;

    std::vector<XSSModule>::iterator it = renderer->modules().begin();
    std::vector<XSSModule>::iterator nd = renderer->modules().end();
    for(; it != nd; it++)
      {
        XSSModule mod = *it;
        pre_process_result result = mod->pre_process(obj, parent);
        if (result == PREPROCESS_HANDLED)
          return;
      }

    std::vector<variant> children_copy = obj->children()->ref(); //copy array, in case it gets modified
    std::vector<variant>::iterator cit = children_copy.begin();
    std::vector<variant>::iterator cnd = children_copy.end();

    for(; cit != cnd; cit++)
      {
        XSSObject child = *cit;
        pre_process(renderer, child, obj);
      }

  }

void xss_compiler::run()
  {
    std::vector<XSSApplicationRenderer>::iterator it = applications_.begin();
    std::vector<XSSApplicationRenderer>::iterator nd = applications_.end();

    for(; it != nd; it++)
      {
        XSSApplicationRenderer app = *it;
        fs::path target = app->entry_point();
        if (target.empty())
          {
				    param_list error;
				    error.add("id", SProjectError);
				    error.add("desc", SAppsMustHaveTarget);

				    xss_throw(error);
          }

        XSSRenderer renderer = compile_xss_file(target, app->context());

        str result = renderer->render(XSSObject(), null);

        fs::path out_file = app->output_path();
        if (out_file.empty())
          std::cout << result;
        else
          output_file(output_path_ / out_file, result);
      }
  }

void xss_compiler::copy_files(XSSObject project_data)
  {
    std::vector<XSSObject> files = project_data->find_by_class("output_file");
    std::vector<XSSObject>::iterator it = files.begin();
    std::vector<XSSObject>::iterator nd = files.end();

    for(; it != nd; it++)
      {
        XSSObject data = *it;
        str src_file = data->get<str>("src",  str());
        str dst_file = data->get<str>("dst", str());

        if (src_file.empty())
          {
				    param_list error;
				    error.add("id", SProjectError);
				    error.add("desc", SCopyFileEmpty);

				    xss_throw(error);
          }

        fs::path src = base_path_ / src_file;

        if (!fs::exists(src))
          {
				    param_list error;
				    error.add("id", SProjectError);
				    error.add("desc", SFileNotFound);
				    error.add("file", src.string());

				    xss_throw(error);
          }

        //do output
        fs::path dst = output_path_ / dst_file;

        fs::path dst_path = dst;
        bool is_directory = dst_file[dst_file.size() - 1] == '/'; //td: !!!
        if (!is_directory)
          dst_path = dst.parent_path();
        else
          dst /= src.filename();

        bool do_copy = true;
        if (fs::exists(dst))
          {
            std::time_t src_modified = boost::filesystem::last_write_time( src ) ;
            std::time_t dst_modified = boost::filesystem::last_write_time( dst ) ;

            do_copy = src_modified != dst_modified;
          }

        if (do_copy)
          {
            dst_path = fs::system_complete(dst_path);
            fs::create_directories(dst_path);
            fs::copy_file(src, dst, fs::copy_option::overwrite_if_exists);
          }
      }
  }

XSSContext xss_compiler::current_context()
  {
    XSSRenderer rend = current_renderer();
    if (rend)
      return rend->context();

    if (current_app_)
      return current_app_->context();

    return XSSContext();
 }

//xss_string
int xss_string::size(const str& s)
  {
    return s.size();
  }

int xss_string::find(const str& s, const str& subs, int pos)
  {
    return s.find(subs, pos);
  }

bool xss_string::empty(const str& s)
  {
    return s.empty();
  }

str xss_string::erase(const str& s, int pos, int npos)
  {
    assert(false);
    return s.substr(pos, npos);
  }

str xss_string::substr(const str& s, int pos, int npos)
  {
    return s.substr(pos, npos);
  }
