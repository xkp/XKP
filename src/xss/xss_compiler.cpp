

#include "xss/xss_compiler.h"
#include "xss/object_reader.h"
#include "xss/xss_error.h"
#include "xss/language.h"

#include "xs/linker.h"
#include "xs/compiler.h"

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

//xss_application_renderer
xss_application_renderer::xss_application_renderer(const str& xss_file):
  filename_(xss_file),
  context_(new xss_context)
  {
  }

XSSContext xss_application_renderer::context()
  {
    return context_;
  }

void xss_application_renderer::register_module(const str& id, XSSModule module)
  {
  }

str xss_application_renderer::target()
  {
    return target_;
  }
   
void xss_application_renderer::set_target(const str& target)
  {
    target_ = target;
  }
 
//xss_module
xss_module::xss_module(XSSContext ctx):
  ctx_(ctx)
  {
  }

//xss_compiler
void xss_compiler::build(fs::path xml)
  {
    XSSObject project_data = read_project(xml.string());
    read_includes(project_data);
  }

fs::path xss_compiler::output_path()
  {
    assert(false); //td implement
    return fs::path();
  }

XSSObject xss_compiler::read_project(const str& xml)
  {
    xss_object_reader reader;
    XSSObject project_data = reader.read(xml);
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
    std::vector<XSSObject>::iterator nd = applications.begin();

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
        
        XSSApplicationRenderer app(new xss_application_renderer(entry_point));

        //load modules
        XSSObject module_data = app_data->get<XSSObject>("modules", XSSObject());
        if (module_data)
          {
            DynamicArray modules = module_data->children();

            for(int i = 0; i < modules->size(); i++)
              {
                XSSObject module = modules->at(i);
                str src = module->get<str>("src", str());
                str id  = module->get<str>("id",  str());

                XSSModule compiled = read_module(src, app, module);
                
                app->register_module(id, compiled);
              }
          }

        applications_.push_back(app);
      }
  }

XSSModule xss_compiler::read_module(const str& src, XSSApplicationRenderer app, XSSObject module)
  {
    xss_object_reader reader(app->context());
    XSSObject module_data = reader.read(src); //td: path

    if (!module_data)
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SCannotReadModule);
        xss_throw(error);
      }

    XSSModule result(new xss_module(app->context()));
    read_types(module, app, result);

    return result;
  }

void xss_compiler::read_types(XSSObject module_data, XSSApplicationRenderer app, XSSModule module)
  {
    XSSObject types = module->find("types");
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

            str class_name = type_data->type_name();
            if (class_name == "class")
              {
              }
            else if (class_name == "enum")
              {
              }
            else
              {
                param_list error;
                error.add("id", SProjectError);
                error.add("desc", SOnlyClassAndEnum);
                xss_throw(error);
              }
              
            //XSSType type(new xss_type(type_data));
            //app->register_type(type_name, type);
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
    if (!def.empty())
      {
        std::vector<XSSObject> classes_data;
        read_object_array(def, classes_data);

        std::vector<XSSObject>::iterator cit = classes_data.begin(); 
        std::vector<XSSObject>::iterator cnd = classes_data.end(); 

        for(; cit != cnd; cit++)
          {
            XSSObject clazz_data;
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
            clazz->add_surrogate(def_class);
            clazz->set_super(super);

				    XSSContext ictx(new xss_context(ctx));
				    ictx->set_this(XSSObject(clazz));

				    compile_ast(ci, ictx);
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
        if (!pit->value.empty())
          value = lang->compile_expression(pit->value, instance);

        XSSType prop_type = ctx->get_type(pit->type);
				XSSProperty new_prop(new xss_property(pit->name, prop_type, value, getter, setter, instance));
        properties->push_back(new_prop); //td: check types with class
      }

    std::vector<xs_method>::iterator mit = gather.methods.begin();
    std::vector<xs_method>::iterator mnd = gather.methods.end();
    for(; mit != mnd; mit++)
      {
				code_type_resolver typer(ctx);
				
				param_list_decl::iterator mait = mit->args.begin();
				param_list_decl::iterator mand = mit->args.end();
				for(; mait != mand; mait++)
					{
            assert(false); //td: get rid of
						//typer.register_var(mait->name, ctx->get_type(mait->type));
					}
 
				mit->cde.visit(&typer);
				
        XSSType decl_type   = ctx->get_type(mit->type);
        XSSType return_type = typer.get();

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

				variant args = lang->compile_args(mit->args);									assert(!args.empty());
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
                actual_instance = ctx->resolve(inst_name, RESOLVE_INSTANCE);
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

				if (actual_instance != instance && options("gen_event_method"))
					{
            assert(false); //td: rewrite, this should not be a compiler option, maybe language options or application renderer

						//here's one. It would seem convinient that events implemented in
						//the context of a different instance would generate code using such
						//instance as the *this* pointer. At least this should be a compiler option.

						str aid    = variant_cast<str>(dynamic_get(actual_instance, "id"), str("")); assert(!aid.empty());
						str iid    = variant_cast<str>(dynamic_get(instance, "id"), str("")); assert(!iid.empty());
						str	mname  = aid + "_" + event_name;

						//so, create a method in the original instance
						variant margs = lang->compile_args(it->args);								 assert(!margs.empty());
						variant mcde  = lang->compile_code(it->cde, it->args, ctx);  assert(!mcde.empty());

						XSSMethod mthd(new xss_method(mname, XSSType(), margs, mcde));
						methods->push_back(mthd);

						//and generate a call to it on the actual method implementation
						//the generation is a simple string.
						std::stringstream css;
						css << iid << '.' << mname << "(";

						//stringify the parameters
						std::stringstream pss;
						param_list_decl::iterator pit = it->args.begin();
						param_list_decl::iterator pnd = it->args.end();
						for(; pit != pnd; pit++)
							{
								css << pit->name;
								if (pit + 1 != pnd)
									{
										css << ",";
									}
							}

						css << ");";

						xs_compiler xsc;
						code				cde;
						if(xsc.compile_code(css.str(), cde))
							{
								XSSContext ictx(new xss_context(ctx));
								ictx->set_this(actual_instance);

								variant impl = lang->compile_code(cde, it->args, ictx);
								impls->push_back(impl);
							}
					}
				else
					{
						//let the idiom process implementations
						XSSContext ictx(new xss_context(ctx));
						ictx->set_this(actual_instance);
						
            variant impl = lang->compile_code(it->cde, it->args, ictx);
						impls->push_back(impl);
					}

				ev->args = lang->compile_args(it->args);
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
    classes_data = reader.read_array(file.string()); //td: errors
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
