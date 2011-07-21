
#include "xss/xss_compiler.h"
#include "xss/object_reader.h"
#include "xss/xss_renderer.h"
#include "xss/xss_error.h"
#include "xss/language.h"
#include "xss/lang/debug.h"
#include "xss/lang/js.h"
#include "xss/dsl_out.h"

#include "xs/linker.h"
#include "xs/compiler.h"

#include <boost/functional/hash.hpp>

using namespace xkp;

//error codes
const str SProjectError("project");
const str SCannotResolve("cannot-resolve");
const str STypeMismatch("type-mismatch");
const str SFileError("404");

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
const str SXSSNeedsFile("compiler.xss needs a xss file to render");

//xss_application_renderer
xss_application_renderer::xss_application_renderer(fs::path entry_point, Language lang, XSSCompiler compiler):
  filename_(entry_point)
  {
    context_ = XSSContext(new xss_context(XSSContext(), entry_point.parent_path()));
    context_->set_language(lang);

    //register default instances
    context_->register_symbol(RESOLVE_NATIVE, "compiler", compiler);
    
    //register standard dsls
    context_->register_dsl("out", DslLinker(new out_linker(compiler)));

    //register default types
    context_->add_type("string", XSSType(new xss_type(type_schema<str>())))->set_id("string");
    context_->add_type("int",    XSSType(new xss_type(type_schema<int>())))->set_id("int");
    context_->add_type("float",  XSSType(new xss_type(type_schema<float>())))->set_id("float");
    context_->add_type("bool",   XSSType(new xss_type(type_schema<bool>())))->set_id("bool");

    XSSType variant_type(new xss_type());
    variant_type->as_variant();
    context_->add_type("var", variant_type);
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
  instances_(new dynamic_array)
  {
    ev_pprocess_ = register_event("render");
    DYNAMIC_INHERITANCE(xss_module)
  }

pre_process_result xss_module::pre_process(XSSObject obj, XSSObject parent)
  {
    XSSObject ev(new xss_object());
       
    param_list args;
    args.add("ev", ev);
    dispatch_event(ev_pprocess_, args);

    if (ev->get<bool>("final", false))
      return PREPROCESS_HANDLED; //canceled by user

    //we'll add sort of an arbitrary logic here
    //the objects with types belonging to this module will be logged 
    //as intances. this can be used later in xss to render the proper instances
    XSSObject types     = find("types");
    str       type_name = obj->type_name();
    bool      found     = false;
    if (types)
      {
        if (types->find(type_name))
          {
            found = true;
            register_instance(obj);
          }
      }

    if (!found)
      {
        XSSObject types = find("process_types");
        if (types && types->find(type_name))
          {
            register_instance(obj);
          }
      }

    return PREPROCESS_KEEPGOING;
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

//xss_compiler
void xss_compiler::build(fs::path xml)
  {
    fs::path pp = xml;
    base_path_ = pp.parent_path();

    XSSObject project_data = read_project(xml);
    
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
  }

XSSRenderer xss_compiler::compile_xss_file(const str& src_file, XSSContext ctx)
  {
    fs::path path = ctx->path() / src_file;
    return compile_xss(load_file(path), ctx, path);
  }

XSSRenderer xss_compiler::compile_xss_file(fs::path src_file, XSSContext ctx)
  {
    return compile_xss(load_file(src_file), ctx, src_file);
  }

XSSRenderer xss_compiler::compile_xss(const str& src, XSSContext ctx, fs::path path)
  {
    boost::hash<std::string> string_hash;
    int hash = string_hash(src);
    
    std::map<int, XSSRenderer>::iterator it = xss_cache.find(hash);
    if (it != xss_cache.end())
      return it->second;

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

void xss_compiler::output_file(const str& fname, const str& contents)
  {
    fs::path path = output_path_ / fname;
    output_file(path, contents);
  }

void xss_compiler::output_file(fs::path fpath, const str& contents)
  {
    fs::create_directories(fpath.parent_path());

    std::ofstream ofs(fpath.string().c_str());
    ofs << contents;
    ofs.close();
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

    XSSRenderer r   = current_renderer();
    XSSContext  ctx = r->context();

    fs::path path = ctx->path() / file_name;
    XSSRenderer result = compile_xss_file(path, ctx);
    
    //match the parameters, by order
    //td: !!! please get parameter names
    renderer_parameter_list& result_args = result->params();

    param_list result_params;
    size_t     param_count = params.size() - 1;
    for(size_t i = 0; i < result_args.size(); i++)
      {
        variant value = i < param_count? 
                          params.get(i + 1) : 
                          variant(); 
        
        //td: check types
        result_params.add(result_args[i].id, value);

      }

    push_renderer(result);
    r->append(result->render(XSSObject(), &result_params)); //td: change dynamic methods to return a variant
    pop_renderer();
  }
void xss_compiler::push_renderer(XSSRenderer renderer)
  {
    renderers_.push(renderer);
  }

void xss_compiler::pop_renderer()
  {
    renderers_.pop();
  }

XSSRenderer xss_compiler::current_renderer()
  {
    assert(!renderers_.empty());
    return renderers_.top();
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

        fs::path op = fs::path(app_data->get<str>("output", str()));
        app->output_path(op);

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
    
    read_types(module, app, result);

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

            XSSType type(new xss_type());
            type->set_id(type_data->id());
            type->set_definition(type_data);

            str class_name = type_data->type_name();
            if (class_name == "class")
              {
                //nothing so far
              }
            else if (class_name == "enum")
              {
                type->as_enum();
              }
            else
              {
                param_list error;
                error.add("id", SProjectError);
                error.add("desc", SOnlyClassAndEnum);
                xss_throw(error);
              }
              

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
                read_include(base_path_ / xml_file, base_path_ / src_file, app->context());
              }
          }
	    }
  }

void xss_compiler::read_include(fs::path def, fs::path src, XSSContext ctx)
  {
    std::map<str, XSSObject> def_types;
    fs::path path;
    if (!def.empty())
      {
        path = def;

        std::vector<XSSObject> classes_data;
        read_object_array(def, classes_data);

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
            clazz->set_definition(def_class);
            clazz->set_super(super);

				    XSSContext ictx(new xss_context(ctx, path.parent_path()));
				    ictx->set_this(XSSObject(clazz));

				    compile_ast(ci, ictx);

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
        XSSApplicationRenderer app_renderer= *it;

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
        pre_process(app_renderer, app_data, XSSObject());

        str src = app_data->get<str>("src", str());
        fs::path src_path = base_path_ / src;
        if (!src.empty() && !code_compiled)
          {
		        compile_xs_file(src_path, code);
            code_compiled = true;
          }

        XSSContext code_ctx(new xss_context(app_renderer->context(), src_path));
        code_ctx->set_this(app_data);
        
        compile_ast(code, code_ctx);

        app_data->set_id("application");
        app_renderer->context()->register_symbol(RESOLVE_INSTANCE, "application", app_data);
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
        if (!pit->value.empty())
          value = lang->compile_expression(pit->value, ctx);

        XSSType prop_type = ctx->get_type(pit->type);
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
        else
          {
            prop_type = ctx->get_type("var"); //td: undefined
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
        XSSType return_type = decl_type; //td: use type_resolver

				if (!decl_type)
					decl_type = return_type;

				if (!mit->type.empty() && decl_type != return_type)
					{
						param_list error;
						error.add("id", STypeMismatch);
						error.add("desc", SIncompatibleReturnType);
						error.add("declared as", mit->type);
						//td: error.add("returns",type);

						xss_throw(error);
					}

				variant args = lang->compile_args(mit->args, ctx);						assert(!args.empty());
				variant cde  = lang->compile_code(mit->cde, mit->args, ctx);  assert(!cde.empty());

				XSSMethod mthd(new xss_method(mit->name, decl_type, args, cde));
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
				ictx->set_this(actual_instance);
						
        variant impl = lang->compile_code(it->cde, it->args, ictx);
				impls->push_back(impl);

				ev->args = lang->compile_args(it->args, ctx);
      }

		std::vector<xs_instance>::iterator iit = gather.instances.begin();
		std::vector<xs_instance>::iterator ind = gather.instances.end();
		for(; iit != ind; iit++)
			{
				xs_instance& instance_ast = *iit;
				str					 pth;
				XSSObject		 instance_instance = ctx->resolve_path(instance_ast.id);

				XSSContext ictx(new xss_context(ctx));
				ictx->set_this(instance_instance);
				compile_ast(instance_ast, ictx);
			}
	}

void xss_compiler::read_object_array(fs::path file, std::vector<XSSObject>& classes_data)
  {
    xss_object_reader reader;
    classes_data = reader.read_array(load_file(file)); 
  }

void xss_compiler::compile_xs_file(fs::path file, xs_container& result)
  {
    xs_compiler compiler;
		compiler.compile_xs(load_file(file.string()), result); //td: errors
  }

str xss_compiler::load_file(fs::path fname)
  {
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

        std::vector<variant> children_copy = obj->children()->ref(); //copy array, in case it gets modified
        std::vector<variant>::iterator cit = children_copy.begin();
        std::vector<variant>::iterator cnd = children_copy.end();

        for(; cit != cnd; cit++)
          {
            XSSObject child = *cit;
            pre_process(renderer, child, obj);
          }

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
