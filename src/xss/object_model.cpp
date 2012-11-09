
#include <xss/object_model.h>
#include <xss/xml_data_reader.h>
#include <xss/xss_context.h>
#include <xss/xss_error.h>
#include <xss/xss_expression.h>
#include <xss/xss_code.h>
#include <xss/xss_compiler.h>
#include <xss/xss_expression.h>

#include <xs/compiler.h>
#include <xs/xs_error.h>

#include <boost/algorithm/string/split.hpp> 
#include <boost/algorithm/string/classification.hpp>

using namespace xkp;

//error codes
const str SProjectError("project");

const str SNamelessProjectParameter("project parameters expect a 'id' tag");
const str SMustProvideEntryPointForApplicationType("Applications must provide an entry point");
const str SMustProvideLanguageForApplicationType("Applications must provide a language");
const str SApplicationsMustHaveOnlyOneRoot("Application definition must aontain only one app");
const str SNeedsId("Entity needs id");
const str SUnkownImport("Unkown type import");
const str SIdiomsMustHaveId("Idioms must have id");
const str SImportCannotInherit("Type imports do not support inheritance");
const str SDupIdiom("Idiom already exists");
const str SUnkownType("Type not found");
const str SDupProperty("Duplicate property");
const str SUnsoportedInclude("Only classes, instances and behaviors are valid on class files");
const str SUndeclaredEvent("Event not found");
const str SBadEventSignature("Event parameter mistmach");
const str SDupEvent("There is already an event with this name");
const str SIdiomFileNotFound("Idiom not found");
const str SFileNotFound("File not found");
const str SMissingArgType("Argument must have a type");
const str SInvalidChild("An instance of this type is not insertable on this parent");
const str SEventNotFound("Event not found");


//utils
struct text_utils
  {
    static void text_extents(const str& text, file_position& b, file_position& e)
      {
        std::vector<str> strs;
        boost::split(strs, text, boost::is_any_of("\n"));

        b.line = 1;
        b.column = 1;
        e.line = strs.size();
        e.column = strs[strs.size() - 1].size() + 1;
      }
  };

struct om_entity_visitor : data_entity_visitor
  {
    om_entity_visitor(object_model* owner, const variant& this__, om_context& ctx):
      owner_(owner),
      ctx_(ctx),
      this_(this__)      
      {
      }

    public:
      //entity_visitor
      virtual void attribute(const str& attr, variant value)
        {
          if (attr == "id" || attr == "output_id")
            return; //system            
          
          attribute_handler_map::iterator it = attr_.find(attr);
          if (it != attr_.end())
            (owner_->*(it->second))(attr, value, this_, ctx_);
          else
            {
              it = attr_.find("*");
              if (it != attr_.end())
                (owner_->*(it->second))(attr, value, this_, ctx_);
            }
        }

      virtual void child(DataEntity de)
        {
          child_handler_map::iterator it = children_.find(de->type());
          if (it != children_.end())
            (owner_->*(it->second))(de, this_, ctx_);
          else
            {
              it = children_.find("*");
              if (it != children_.end())
                (owner_->*(it->second))(de, this_, ctx_);
            }
        }
    public:
      //handlers
      typedef void (object_model::* attribute_handler_func)(const str& attr, const str& value, const variant& this_, om_context& ctx);
      typedef void (object_model::* child_handler_func)    (DataEntity de, const variant& this_, om_context& ctx);

      //handler registration
      void attribute_handler(const str& attr, attribute_handler_func handler)
        {
          attribute_handler_map::iterator it = attr_.find(attr); assert(it == attr_.end());
          attr_.insert(std::pair<str, attribute_handler_func>(attr, handler));
        }

      void child_handler(const str& child, child_handler_func handler)
        {
          child_handler_map::iterator it = children_.find(child); assert(it == children_.end());
          children_.insert(std::pair<str, child_handler_func>(child, handler));
        }
    private:
      typedef std::map<str, attribute_handler_func> attribute_handler_map;
      typedef std::map<str, child_handler_func>     child_handler_map;

      object_model*         owner_;
      om_context&           ctx_;
      variant               this_;
      attribute_handler_map attr_;
      child_handler_map     children_;
  };

//idiom
void idiom::bind(XSSContext ctx)
  {
    type_list::iterator it = types_.begin();
    type_list::iterator nd = types_.end();
    for(; it != nd; it++)
      {
        ctx->add_type(*it, namespace_);
      }
  }

void idiom::set_namespace(const str& _namespace)
  {
    namespace_ = _namespace;
  }

void idiom::add_type(XSSType type)
  {
    types_.push_back(type);
  }

void idiom::add_import(XSSType import)
  {
    imports_.push_back(import);
  }

XSSType idiom::import(const str& id)
  {
    type_list::iterator it = imports_.begin();
    type_list::iterator nd = imports_.end();
    for(; it != nd; it++)
      {
        XSSType import = *it;
        if (import->id() == id)
          return import;
      }

    return XSSType();
  }

str idiom::get_namespace()
  {
    return namespace_;
  }

//application
application::application()
  {
    assert(false); //shouldn't be called
  }

application::application(const application& other)
  {
    assert(false); //shouldn't be called
  }

application::application(fs::path path):
  path_(path)
  {
  }

void application::entry_point(fs::path ep)
  {
    renderer_ = ep;
  }

void application::output_file(const str& ep)
  {
    output_ = ep;
  }

void application::compiler_options(DataEntity options)
  {
    //td: we dont have no options?
  }

void application::set_context(XSSContext ctx)
  {
    assert(!ctx_);
    ctx_ = ctx;
  }

code_context& application::exec_context()
  {
    return code_ctx_;
  }

XSSContext application::context()
  {
    return ctx_;
  }

fs::path application::path()
  {
    return path_;
  }

document* application::create_document(fs::path fname, XSSContext ctx)
  {
    assert(documents_.find(fname) == documents_.end());
    documents_.insert(std::pair<fs::path, document>(fname, document(ctx)));
    return get_document(fname);
  }

document* application::get_document(fs::path path)
  {
    document_map::iterator it = documents_.find(path);
    if (it != documents_.end())
      return &it->second;

    return null;
  }

error_list& application::errors()
  {
    return errors_;
  }

void application::add_error(const str& desc, param_list* info, file_location& loc)
  {
    error_list::iterator it = errors_.begin();
    error_list::iterator nd = errors_.end();

    for(; it != nd; it++)
      {
        if (it->desc == desc && loc.file == it->loc.file && loc.begin.line == it->loc.begin.line)
          {
            //avoid dups
            it->loc.begin.column = loc.begin.column;
			      it->desc = desc; 
            return;
          }
      }

    errors_.push_back(error_info(desc, info, loc));
  }

void application::visit_errors(const fs::path& fname, error_visitor* visitor)
  {
    error_list::iterator it = errors_.begin();
    error_list::iterator nd = errors_.end();

    for(; it != nd; it++)
      {
        if (it->loc.file == fname || fname.empty())
          visitor->visit(*it);
      }
  }

void application::clear_file_errors(const fs::path& fname)
  {
	  for(int i = errors_.size() - 1; i >= 0; i--)
      {
        if (errors_[i].loc.file == fname)
			    errors_.erase(errors_.begin() + i);
      }
  }

APPLICATION_ITEM application::app_item(const str& fname)
  {
    if (app_path_ == fname)
      return AI_APP;

    return AI_CLASS;
  }

void application::app_path(const fs::path& fname)
  {
    app_path_ = fname;
  }

fs::path application::app_path()
  {
    return app_path_;
  }

void application::build()
  {
    //JsonOutput out; 
    ConsoleOutput out;
    
    XSSCompiler compiler(new xss_compiler(&out));
    compiler->file_system(fs_);
    compiler->set_output_path(output_path_);
    
    Application self = shared_from_this();
    compiler->register_symbol("application", self);
    compiler->register_symbol("compiler", compiler);
    compiler->register_symbol("project", project_);
    
    XSSContext cctx = compiler->context();
    cctx->set_language(ctx_->get_language());
    cctx->errors(ctx_->errors());
    
    compiler->build(renderer_, output_);
  }

void application::project_object(XSSObject project)
  {
    project_ = project;
  }

void application::file_system(FileSystem fs)
  {
    fs_ = fs;
  }

void application::output_path(const fs::path& path)
  {
    output_path_ = path;
  }

fs::path application::output_path()
  {
    return output_path_;
  }

str application::random_id(XSSObject instance)
  {
    XSSType type = instance->type();
    str type_id = type? type->id() : "object"; 
    return type_id + "__" + boost::lexical_cast<str>( instance.get() ); 
  }

DynamicArray application::__instances()
  {
    DynamicArray result(new dynamic_array);

    instance_list::iterator it = instances_.begin();
    instance_list::iterator nd = instances_.end();
    for(; it != nd; it++)
      {
        result->push_back(*it);
      }

    return result;
  }

DynamicArray application::__user_types()
  {
    DynamicArray result(new dynamic_array);

    type_list::iterator it = user_types_.begin();
    type_list::iterator nd = user_types_.end();
    for(; it != nd; it++)
      {
        result->push_back(*it);
      }

    return result;
  }

void application::add_instance(XSSObject instance)
  {
    assert(instance);
    str  id  = instance->id();
    bool add = true; 
    if (id.empty())
      {
        add = false;
        if (instance->output_id().empty())
          {
            instance->set_output_id(random_id(instance));
          }
      }

    instances_.push_back(instance);

    if (add)
      ctx_->register_symbol(RESOLVE_INSTANCE, id, instance);
  }

void application::add_class(XSSType result)
  {
    user_types_.push_back(result);
  }

XSSObject application::root()
  {
    return app_;
  }

void application::set_root(XSSObject r)
  {
    app_ = r;
  }

str application::name()
  {
    return app_name_;
  }

void application::set_name(const str& name)
  {
    app_name_ = name;
  }

//document
void document::add(XSSContext context)
  {
    assert(context->begin().line >= 0 && context->end().line >= 0);
    items_.push_back(snap_shot(context->begin(), context->end(), context));
  }

void document::synch_extents()
  {
    snap_shots::iterator it = items_.begin();
    snap_shots::iterator nd = items_.end();
    for(; it != nd; it++)
      {
		    it->context->set_extents(it->begin, it->end);
	    }
  }

void document::refresh_context(XSSContext ctx)
  {
    snap_shots::iterator it = items_.begin();
    snap_shots::iterator nd = items_.end();

    bool          unstable = false; //are we a little lost?
    XSSContext    result = ctx_;
    file_position result_pos;

    for(; it != nd; it++)
      {
        if (it->context == ctx)
          {
            it->begin = ctx->begin();
            it->end   = ctx->end();
          }
      }
  }

XSSContext document::context()
  {
    return ctx_;
  }

void document::context(XSSContext ctx)
  {
    assert(!ctx_);
    ctx_ = ctx;
  }

XSSContext document::changed(int line, int col, int oldEndLine, int oldEndCol, int newEndLine, int newEndCol)
  {
    int diff = newEndLine - oldEndLine;

    if (diff == 0)
      return XSSContext(); //same line change, ignore

    snap_shots::iterator it = items_.begin();
    snap_shots::iterator nd = items_.end();

    bool          unstable = false; //are we a little lost?
    XSSContext    result = ctx_;
    file_position result_pos;

    for(; it != nd; it++)
      {
        if (it->end.line < line)
          continue; //unaffected

        if (it->begin.line > oldEndLine)
          {
            //simple modify
            it->begin.line += diff;
            it->end.line += diff;
            continue;
          }

        if (unstable)
          {
            //just update boundaries
            if (it->begin.line > oldEndLine)
              {
                it->begin.line += diff;
                it->end.line += diff;
              }
            else if (it->end.line > oldEndLine)
              it->end.line += diff;

            continue;
          }

        //analize the intersection, use cases:
        //1- boundaries of the context are broken, this will trigger a rebuild later
        //2- fully inside, look for the best candidate
        //3- fully contains, look for the best candidate

        bool broken_up   = (line < it->begin.line && oldEndLine < it->end.line);
        bool broken_down = (line > it->begin.line && oldEndLine > it->end.line);

        if (broken_up)
          {
            it->begin.line = line;
            it->end.line += diff;
            result = it->context->parent();
            unstable = true;
          }
        else if (broken_down)
          {
            it->end.line += diff;
            result = it->context->parent();
            unstable = true;
          }
        else
          {
            bool full_inside = line >= it->begin.line && oldEndLine <= it->end.line;
            if (full_inside)
              {
                it->end.line += diff;

                //is it a better candidate?
                if (it->begin.line > result_pos.line)
                  {
                    result  = it->context;
                    result_pos = it->begin;
                  }
              }
            else
              {
                //the changed region contains this context, so it is no more
                it->begin.line = -1;
                it->end.line   = -1;
              }
          }
      }
    
    synch_extents();
	return result;
  }

document::document()
  {
  }

document::document(XSSContext ctx):
  ctx_(ctx)
  {
  }

XSSContext document::find_context(int line, int column)
  {
    if (items_.empty())
      return ctx_;

    snap_shots::iterator it = items_.begin();
    snap_shots::iterator nd = items_.end();

    //candidate
    XSSContext result = ctx_;

    file_position current;

    for(; it != nd; it++)
      {
        if (it->begin.line > line)
          continue;

        if (it->begin.line == line && it->begin.column > column)
          continue;

        if (it->end.line < line)
          continue;

        if (it->end.line == line && it->end.column < column)
          continue;

        if (it->begin.line > current.line ||
            it->begin.line == current.line && it->begin.column > current.column)
          {
            result  = it->context;
            current = it->begin;
          }
      }

    return result;
  }

XSSContext document::context_by_identity(CONTEXT_IDENTITY id, variant value)
  {
    snap_shots::iterator it = items_.begin();
    snap_shots::iterator nd = items_.end();
    for(; it != nd; it++)
      {
        XSSContext ctx = it->context;
        if (ctx->identity() != id)
          continue;

        switch(id)
          {
            case CTXID_CODE:
              {
                XSSCode mine = variant_cast<XSSCode>(ctx->identity_value(), XSSCode());
                XSSCode val  = variant_cast<XSSCode>(value, XSSCode());
                if (mine && val && val == mine)
                  return ctx;

                break;
              }
            
            case CTXID_INSTANCE:
              {
                XSSObject mine = variant_cast<XSSObject>(ctx->identity_value(), XSSObject());
                XSSObject val  = variant_cast<XSSObject>(value, XSSObject());
                if (mine && val && val == mine)
                  return ctx;

                break;
              }
            
            case CTXID_TYPE:
              {
                XSSType mine = variant_cast<XSSType>(ctx->identity_value(), XSSType());
                XSSType val  = variant_cast<XSSType>(value, XSSType());
                if (mine && val && val == mine)
                  return ctx;

                break;
              }
          }
      }
    return XSSContext();
  }

struct om_error_handler : IErrorHandler
  {
    om_error_handler(Application app):
      app_(app)
      {
      }
    
    virtual void add(const str& description, param_list* data, file_location& loc)
      {
        app_->add_error(description, data, loc);
      }

    virtual bool has_errors()
      {
        return !app_->errors().empty();
      }

    virtual error_list& errors()
      {
        return app_->errors();
      }
    private:
      Application app_;
  };

struct local_error_handler : IErrorHandler
  {
    virtual void add(const str& description, param_list* data, file_location& loc)
      {
        errors_.push_back(error_info(description, data, loc));
      }

    virtual bool has_errors()
      {
        return !errors_.empty();
      }

    virtual error_list& errors()
      {
        return errors_;
      }
    private:
      error_list errors_;
  };

//object_model
object_model::object_model(FileSystem fs, LanguageFactory languages):
  fs_(fs),
  languages_(languages),
  changed_(false)
  {
  }

Application object_model::load(DataReader project, param_list& args, fs::path base_path)
  {
    Application result(new application(base_path));
	  result->file_system(fs_);

    om_context octx;
    octx.application = result;

    DataEntity  prj = assure_unique_root(project, octx);

    //setup
    result->file_system(fs_);
    str appname = prj->attr("name");
    if (appname.empty())
      appname = "untitled";

    str outpath = prj->attr("output_path");
    fs::path output_path = outpath;
    if (!output_path.is_complete())
      output_path = base_path / output_path;

    result->set_name(appname);
    result->output_path(output_path);

    //create the global context for this application
    Language   lang   = read_language(prj, result);
    XSSContext global = lang->create_context();
    global->set_path(base_path);
    global->set_language(lang);

    ErrorHandler eh(new om_error_handler(result));
    global->errors(eh);

    octx.xss_ctx = global;

    //initialize the execution context on which the xss files will run, 
    //note this is different from the data context
    code_context& code_ctx = result->exec_context();

    //in that context there will be a "project" object containing stuff of usability
    XSSObject app_project(new xss_object);
    result->project_object(app_project);

    //read project params
    entity_list parameters = prj->find_all("parameter");
    entity_list::iterator pit = parameters.begin();
    entity_list::iterator pnd = parameters.end();

    for(; pit != pnd; pit++)
      {
        DataEntity param = *pit;
        
        str pid = param->id();
        if (pid.empty())
          {
            add_error(result, SNamelessProjectParameter, null, file_location());
            continue;
          }

        app_project->add_attribute(pid, read_value(param));
      }

    //fillup our application
    str ep_filename = prj->attr("entry_point");
    fs::path ep = fs_->locate(ep_filename, fs::path());
    if (ep.empty())
      {
        ep = fs_->locate(ep_filename, base_path);

        if (ep.empty())
          add_error(result, SMustProvideEntryPointForApplicationType, null, file_location());
      }
    
    result->entry_point(ep);
    result->output_file(prj->attr("output"));
    result->compiler_options(prj->get("options"));
    result->set_context(global);

    //load idioms
    entity_list idioms = prj->find_all("idiom");
    entity_list::iterator it = idioms.begin();
    entity_list::iterator nd = idioms.end();

    for(; it != nd; it++)
      {
        DataEntity _idiom     = *it;
        str        idiom_src  = _idiom->attr("src");
        fs::path   idiom_path = fs_->locate(idiom_src, base_path); 

        if (idiom_path.empty())
          {
            param_list error;
            error.add("file", idiom_src);
            add_error(result, SIdiomFileNotFound, &error, file_location());
          }

        Idiom idiom = find_idiom(idiom_path.string());
        
        if (!idiom)
          {
            DataReader idiom_data = fs_->load_data(idiom_path);
            idiom = read_idiom(assure_unique_root(idiom_data, octx), octx);
            register_idiom(idiom_path.string(), result, idiom);
          }

        assert(idiom);
        idiom->bind(global);
      }

    //load includes
    entity_list includes = prj->find_all("include");
    entity_list::iterator iit = includes.begin();
    entity_list::iterator ind = includes.end();

    //results
    for(; iit != ind; iit++)
      {
        DataEntity _include = *iit;
        str        def = _include->attr("def");  
        str        src = _include->attr("src");  

        //create a separate context for includes
        XSSContext include_ctx(new xss_context(CTXID_FILE, src, global));

        //remember the old one
        XSSContext old_ctx = octx.xss_ctx;
        octx.xss_ctx = include_ctx;
        handle_include(result, def, src, octx);
        octx.xss_ctx = old_ctx;
      }

    //register classes
    type_map::iterator cit = octx.classes.begin();
    type_map::iterator cnd = octx.classes.end();
    for(; cit != cnd; cit++)
      {
        global->add_type(cit->first, cit->second);
        result->add_class(cit->second);
      }

    //load application
    str def = prj->attr("def");
    str src = prj->attr("src");
    XSSContext app_ctx(new xss_context(CTXID_FILE, src, global));
    XSSObject  app_obj(new xss_object);
	  XSSType    app_type = global->get_type("Application"); 
    app_obj->set_id("application");
    app_obj->set_type(app_type);

    XSSContext old_ctx = octx.xss_ctx;
    octx.xss_ctx = app_ctx;
    handle_instance(result, app_obj, def, src, octx);
    octx.xss_ctx = old_ctx;

    //compile
    fix_it_up(global, octx);
    bind_it_up(global, octx);

    fs::path ap = fs_->locate(src, base_path);
    result->app_path(ap);
    result->set_root(app_obj);
    return result;
  }

void object_model::register_app(const fs::path& fname, Application app)
  {
    apps_.insert(std::pair<fs::path, Application>(fname, app));
	  changed_ = true;
  }

Application object_model::remove_app(const fs::path& fname)
  {
    application_list::iterator it = apps_.find(fname);
    if (it != apps_.end())
      {
		    Application result = it->second;
        apps_.erase(it);
        return result;
      }

    return Application();
  }

Application object_model::get_application(const str& fname)
  {
    application_list::iterator it = apps_.find(fname);
    if (it != apps_.end())
      return it->second;

    return Application();
  }

document* object_model::get_document(const str& fname)
  {
    fs::path path(fname);
    if (!fs::exists(path))
      return null;

    application_list::iterator it = apps_.begin();
    application_list::iterator nd = apps_.end();

    for(; it != nd; it++)
      {
        document* result = it->second->get_document(path);
        if (result)
          return result;
      }

    return null;
  }

void object_model::update_document(const str& fname, om_response& response)
  {
	  document* doc = get_document(fname);
	  if (doc)
	    {
        Application app = response.data.application; assert(app);

        //bail on errors
        ErrorHandler eh = response.ctx->errors();
        if (eh && eh->has_errors())
          {
            //add new errors, keep old for presentation sake
            error_list& errors = eh->errors();
            error_list::iterator it = errors.begin();
            error_list::iterator nd = errors.end();

            for(; it != nd; it++)
			        {
                it->loc.file = fname;
                add_error(app, it->desc, &it->info, it->loc);
			        }

            return;
          }

        response.ctx->errors(ErrorHandler()); //let them go thru
		    response.ctx->set_parent(app->context());
		    response.ctx->source_file(fs::path(fname));
		
		    XSSContext  ctx = response.ctx;

        //the code compiled successfully, lets clear errors
        clear_file_errors(fname);

		fix_it_up(response.ctx, response.data);

        //then bind
        type_map::iterator it = response.data.classes.begin();
        type_map::iterator nd = response.data.classes.end();

        for(; it != nd; it++)
          {
            XSSType new_type = it->second; 
            XSSType old_type = ctx->get_type(it->first);
            ctx->add_type(it->first, new_type, true); //td: missing on a lot of optimizations by ignoring old_type

            XSSContext tctx = response.doc.context_by_identity(CTXID_TYPE, new_type); assert(tctx);
            new_type->bind(tctx);
          }

        instance_map::iterator iit = response.data.instances.begin();
        instance_map::iterator ind = response.data.instances.end();

        for(; iit != ind; iit++)
          {
            XSSObject new_instance = iit->second; 
            XSSObject old_instance = ctx->resolve(iit->first, RESOLVE_INSTANCE);
            ctx->register_symbol(RESOLVE_INSTANCE, iit->first, new_instance, true); 

            XSSContext ictx = response.doc.context_by_identity(CTXID_INSTANCE, new_instance); assert(ictx);
            new_instance->bind(ictx);
          }

		    *doc = response.doc;
        doc->context(ctx);
	    }
  }

void object_model::add_include(Application app, const str& def, const str& src)
  {
    fs::path def_path = xss_utils::relative_path(fs::path(def), app->path());
    fs::path src_path = xss_utils::relative_path(fs::path(src), app->path());

    om_context octx;
    XSSContext global = app->context();
    XSSContext include_ctx(new xss_context(CTXID_FILE, str(src), global));
    octx.xss_ctx = include_ctx;
    handle_include(app, def, src, octx);

    fix_it_up(global, octx);
    bind_it_up(global, octx);
  }

Language object_model::read_language(DataEntity project, Application app)
  {
    str language_name = project->attr("language");
    if (language_name.empty())
      {
        add_error(app, SMustProvideLanguageForApplicationType, null, file_location());
        return Language();
      }

    return languages_->create(language_name);
  }

variant object_model::read_value(DataEntity de)
  {
    str type  = de->attr("type");
    str value = de->attr("value");

    try
      {
        if (type == "int")
          {
            if (value.empty())
              return 0;

            return boost::lexical_cast<int>( value );
          }
        else if (type == "float")
          {
            if (value.empty())
              return 0.0f;

            return boost::lexical_cast<float>( value );
          }
        else if (type == "double")
          {
            if (value.empty())
              return 0.0f;

            return boost::lexical_cast<double>( value );
          }
        else if (type == "bool")
          {
            if (value.empty())
              return false;
            if (value == "true")
              return true;
            else if (value == "false")
              return false;
            
            throw boost::bad_lexical_cast();
          }
      }
    catch(boost::bad_lexical_cast&)
      {
        assert(false); //td: compile expression
      }

    return value; //as string
  }

Idiom object_model::read_idiom(DataEntity de, om_context& ctx)
  {
    Idiom result(new idiom);
    str iid = de->id();

    if (iid.empty())
      ctx.xss_ctx->error(SIdiomsMustHaveId, null, file_position(), file_position());

    //read
    ctx.idiom = result;

    om_entity_visitor oev(this, result, ctx);
    oev.attribute_handler("namespace",  &object_model::r_idiom_namespace);
    oev.attribute_handler("src",        &object_model::r_attr_nop); //td: !!! idioms

    oev.child_handler("enum",   &object_model::r_idiom_enum);
    oev.child_handler("import", &object_model::r_idiom_import);
    oev.child_handler("class",  &object_model::r_idiom_type);
    oev.child_handler("type",   &object_model::r_idiom_type);

    oev.attribute_handler("*", &object_model::r_invalid_idiom_attr);
    oev.child_handler    ("*", &object_model::r_invalid_idiom_child);

    de->visit(&oev);

    ctx.idiom = Idiom();
    return result; 
  }

void object_model::handle_include(Application app, const str& def_file, const str& src_file, om_context& ctx)
  {
    fs::path path;
    if (!def_file.empty())
      {
        DataReader def      = fs_->load_data(def_file, ctx.xss_ctx->path());
        DataEntity def_root = assure_unique_root(def, ctx);

        om_entity_visitor oev(this, variant(), ctx);
        oev.child_handler("class",    &object_model::r_include_class);
        oev.child_handler("instance", &object_model::r_include_instance);
        oev.child_handler("*",        &object_model::r_invalid_include_child);

        def_root->visit(&oev);
      }

    if (!src_file.empty())
      {
        fs::path src_path = fs_->locate(src_file, ctx.xss_ctx->path());
        if (src_path.empty())
          { 
            param_list error;
            error.add("file", src_file);
            add_error(ctx.application, SFileNotFound, &error, file_location());
            return;
          }

        ctx.xss_ctx->source_file(src_path);

        str       code = fs_->load_file(src_path);
        document* doc  = create_document(app, src_file, ctx.xss_ctx);
        
        assert(!ctx.doc);
        ctx.doc = doc;
        compile_class(code, ctx.xss_ctx, ctx);
        ctx.doc = null;
			}
  }

void object_model::handle_instance(Application app, XSSObject instance, const str& def_file, const str& src_file, om_context& ctx)
  {
    fs::path path;
    if (!def_file.empty())
      {
        DataReader def      = fs_->load_data(def_file, ctx.xss_ctx->path());
        DataEntity def_root = assure_unique_root(def, ctx);

        om_entity_visitor oev(this, instance, ctx);
        oev.attribute_handler("*",        &object_model::r_object_attr);
        oev.child_handler    ("property", &object_model::r_object_property);
        oev.child_handler    ("method",   &object_model::r_object_method);
        oev.child_handler    ("event",    &object_model::r_object_event);
        oev.child_handler    ("*",        &object_model::r_object_instance);

        def_root->visit(&oev);
      }

    if (!src_file.empty())
      {
        fs::path src_path = fs_->locate(src_file, ctx.xss_ctx->path());
        if (src_path.empty())
          { 
            param_list error;
            error.add("file", src_file);
            add_error(ctx.application, SFileNotFound, &error, file_location());
            return;
          }

        ctx.xss_ctx->source_file(src_path);

        str       code = fs_->load_file(src_path);
        document* doc  = create_document(app, src_file, ctx.xss_ctx);
        
        XSSContext old_ctx = ctx.xss_ctx;
        XSSContext ictx(new xss_context(CTXID_INSTANCE, instance, ctx.xss_ctx));

        assert(!ctx.doc);
        ctx.doc = doc;
        ctx.instance = instance;
        ctx.xss_ctx = ictx;
        compile_instance(instance, code, ictx, ctx);
        
		    //assign the whole file to this instance
        file_position b, e;
		    text_utils::text_extents(code, b, e);
		    ictx->set_extents(b, e);
        doc->refresh_context(ictx);

        //output
		    ctx.contexts.insert(std::pair<str, XSSContext>(instance->id(), ictx));
        ctx.instances.insert(std::pair<str, XSSObject>(instance->id(), instance));
        
        //restore ctx
        ctx.doc = null;
        ctx.instance.reset();
        ctx.xss_ctx = old_ctx;
			}
  }

//fixer uppers
struct fixup_type
  {
    fixup_type(const str& _type_name, XSSObject _target):
      type_name(_type_name),
      target(_target)
      {
      }

    str       type_name;
    XSSObject target;
  };

struct fixup_property_type
  {
    fixup_property_type(const xs_type& _type_name, XSSProperty _target):
      type_name(_type_name),
      target(_target)
      {
      }

    xs_type     type_name;
    XSSProperty target;
  };

struct fixup_return_type
  {
    fixup_return_type(const str& _type_name, XSSMethod _target):
      type_name(_type_name),
      target(_target)
      {
      }

    str       type_name;
    XSSMethod target;
  };

struct fix_instance_event
  {
    fix_instance_event(XSSObject _target, XSSContext _ctx, std::vector<str>& _instance, XSSSignature _sig, XSSCode _code):
      instance(_instance),
      sig(_sig),
      code(_code),
      target(_target),
      ctx(_ctx)
      {
      }

    XSSObject        target;
    std::vector<str> instance;
    XSSSignature     sig;
    XSSCode          code;
    XSSContext       ctx;
  };

struct fixup_super
  {
    fixup_super(const str& _super, XSSType _type):
      super(_super), 
      type(_type)
      {
      }

    str     super;
    XSSType type;
  };

struct fixup_arg_type
  {
    fixup_arg_type(const str& _type, XSSSignature _args, int _idx):
      args(_args), 
      type(_type),
      idx(_idx)
      {
      }

    str          type;
    XSSSignature args;
    int          idx;
  };

struct fixup_obj_child
  {
    fixup_obj_child(XSSObject _parent, DataEntity _de):
      parent(_parent), 
      de(_de)
      {
      }

    XSSObject  parent;
    DataEntity de;
  };

struct fixup_policy_type
  {
    fixup_policy_type(const str& _type, XSSType _owner, PARENT_CHILD_ACTION _pca):
      type(_type),
      owner(_owner),
      pca(_pca)
      {
      }

    str                 type;
    XSSType             owner;
    PARENT_CHILD_ACTION pca;
  };

//visitors
struct object_visitor : xs_visitor
  {
    object_visitor(XSSObject target, XSSContext ctx, fixup_list& fixup, IContextCallback* cb):
      target_(target),
      ctx_(ctx),
      fixup_(fixup),
      callback_(cb)
      {
      }

    virtual void property_(xs_property& info)
      {
        XSSProperty result = target_->get_property(info.name);

        if (result && !target_->get_shallow_property(info.name))
          {
					  param_list error;
					  error.add("object", target_->id());
					  error.add("property", info.name);
            ctx_->error(SDupProperty, &error, info.begin, info.end);
          }

        if (!result)
          {
            result = XSSProperty(new xss_property);
            result->set_id(info.name);

            target_->insert_property(result);
          }
        
        if (!info.type.name.empty())
          fixup_.push_back(fixup_data(FIXUP_PROPERTY_TYPE, fixup_property_type(info.type, result)));
        
        if (!info.value.empty())
          {
            XSSExpression value = xss_expression_utils::compile_expression(info.value);
            result->expr_value(value);
          }

        if (!info.get.empty())
          {
            XSSCode getter = xss_code_utils::compile_code(info.get, callback_); 
            result->code_getter(getter);
          }

        if (!info.set.empty())
          {
            XSSCode setter = xss_code_utils::compile_code(info.set, callback_); 
            result->code_setter(setter);
          }
      }

    virtual void method_(xs_method& info)
      {
        XSSMethod result = target_->get_method(info.name);

        if (result)
          {
            assert(false); //td: overrides, multi-dispatch
          }

        if (!result)
          {
            result = XSSMethod(new xss_method);
            result->set_id(info.name);
            target_->insert_method(result);
          }

        if (!info.type.name.empty()) //td: generics
          fixup_.push_back(fixup_data(FIXUP_RETURN_TYPE, fixup_return_type(info.type.name, result)));

        XSSSignature sig  = xss_code_utils::compile_arg_list(info.args);
        result->signature(sig);

        XSSCode code = xss_code_utils::compile_code(info.cde, callback_); 
        result->code(code);
      }

    virtual void event_(xs_event& info)
      {
        XSSSignature sig  = xss_code_utils::compile_arg_list(info.args);
        XSSCode      code = xss_code_utils::compile_code(info.cde, callback_); 

        if (info.name.size() > 1)
          {
            fixup_.push_back(fixup_data(FIXUP_INSTANCE_EVENT, fix_instance_event(target_, ctx_, info.name, sig, code)));
            return;
          }

        XSSEvent ev = target_->get_event(info.name[0]);
        if (!ev)
          {
					  param_list error;
					  error.add("object", target_->id());
					  error.add("method", info.name);
            ctx_->error(SUndeclaredEvent, &error, info.begin, info.end);
          }

        if (!ev->signature()->match_signature(sig))
          {
					  param_list error;
					  error.add("object", target_->id());
					  error.add("event", info.name[0]);
            ctx_->error(SBadEventSignature, &error, info.begin, info.end);
          }

        target_->add_event_impl(ev, code, ev->signature());
      }

    virtual void event_decl_(xs_event_decl& info)
      {
        XSSEvent result = target_->get_event(info.name);
        if (result)
          {
					  param_list error;
					  error.add("object", target_->id());
					  error.add("event", info.name);
            ctx_->error(SDupEvent, &error, info.begin, info.end);
          }

        result = XSSEvent(new xss_event);
        result->set_id(info.name);
        result->set_signature(xss_code_utils::compile_arg_list(info.args));
        target_->insert_event(result);
      }

    virtual void const_(xs_const& info)
      {
        XSSProperty result = target_->get_property(info.name);

        if (result)
          {
					  param_list error;
					  error.add("object", target_->id());
					  error.add("const", info.name);
            ctx_->error(SDupProperty, &error, info.begin, info.end);
          }

        result = XSSProperty(new xss_property);
        
        XSSExpression value = xss_expression_utils::compile_expression(info.value);
        result->set_id(info.name);
        result->expr_value(value);
        result->as_const();

        target_->insert_property(result);
      }

    virtual void behaveas_(xs_implement_behaviour& info)
      {
        assert(false); //td: !!!
      }

    virtual void dsl_(dsl& info)
      {
        assert(false); //td: !!!
      }

    virtual void instance_(xs_instance& info)   {assert(false);} //td: 0.9.5, these make sense inside an instance
    virtual void class_(xs_class& info)         {assert(false);}
    virtual void behaviour_(xs_behaviour& info) {assert(false);}

    private:
      XSSObject         target_;
      XSSContext        ctx_;
      fixup_list&       fixup_;
      IContextCallback* callback_;
  };

struct include_visitor : xs_visitor
  {
    include_visitor(XSSContext ctx, om_context& octx, IContextCallback* callback):
      ctx_(ctx),
      octx_(octx),
      callback_(callback)
      {
      }

    virtual void instance_(xs_instance& info)
      {
        XSSObject instance;

        assert(info.id.size() == 1); //td: !!!qualified
        str id = info.id[0];
        
        instance_map::iterator it = octx_.instances.find(id);
        if (it != octx_.instances.end())
          instance = it->second;
        else
          {
            instance = XSSObject(new xss_object);
            instance->set_id(id);

            octx_.instances.insert(std::pair<str, XSSObject>(id, instance));
          }

        //create the context into which the instance will be bound
        XSSContext i_ctx(new xss_context(CTXID_INSTANCE, instance, ctx_));
        i_ctx->set_extents(info.begin, info.end);

        //remember the context
        octx_.contexts.insert(std::pair<str, XSSContext>(id, i_ctx)); 
        
        if (callback_)
          callback_->notify(i_ctx);

        object_visitor ov(instance, i_ctx, octx_.fixup, callback_);
        info.visit(&ov);
      }

    virtual void class_(xs_class& info)
      {
        XSSType type;

        type_map::iterator it = octx_.classes.find(info.name);
        if (it != octx_.classes.end())
          type = it->second;
        else
          {
            type = XSSType(new xss_type);
            type->set_id(info.name);
            octx_.classes.insert(std::pair<str, XSSType>(info.name, type));
          }

        if (!info.super.empty())
          {
            octx_.fixup.push_back(fixup_data(FIXUP_SUPER_TYPE, fixup_super(info.super, type)));
          }
        
        //create the context into which the instance will be bound
        XSSContext t_ctx(new xss_context(CTXID_TYPE, type, ctx_));
        t_ctx->set_extents(info.begin, info.end);

        //remember the context & notify
        octx_.contexts.insert(std::pair<str, XSSContext>(info.name, t_ctx)); 

        if (callback_)
          callback_->notify(t_ctx);

        //keep moving down the hierarchy
        object_visitor ov(XSSObject(type), t_ctx, octx_.fixup, callback_);
        info.visit(&ov);
      }
    
    virtual void behaviour_(xs_behaviour& info)
      {
        assert(false); //td: !!!
      }

    virtual void dsl_(dsl& info)
      {
        assert(false); //td: instance dsl
      }

    virtual void property_(xs_property& info)             {error(info.begin, info.end);}
    virtual void method_(xs_method& info)                 {error(info.begin, info.end);}
    virtual void event_(xs_event& info)                   {error(info.begin, info.end);}
    virtual void event_decl_(xs_event_decl& info)         {error(info.begin, info.end);}
    virtual void const_(xs_const& info)                   {error(info.begin, info.end);}
    virtual void behaveas_(xs_implement_behaviour& info)  {error(info.begin, info.end);}

    private:
      XSSContext        ctx_;
      om_context&       octx_;
      IContextCallback* callback_;

      void error(file_position& begin, file_position& end)
        {
          ctx_->error(SUnsoportedInclude, null, begin, end);
        }
  };

struct document_builder : IContextCallback
  {
    document_builder(document* doc):
      doc_(doc)
      {
      }

    virtual void notify(XSSContext context)
      {
        doc_->add(context);
      }

    private:
      document* doc_;
  };

bool object_model::compile_xs(const str& text, XSSContext ctx, om_context& octx, xs_visitor* visitor)
  {
    xs_container results;
    std::vector<str> dsls;
    ctx->collect_xss_dsls(dsls);

    xs_compiler compiler(dsls);
    bool        error = false;
    try
      {
        compiler.compile_xs(text, results); 
      }
    catch(xs_error err)
      {
        error = true;
        int l = variant_cast<int>(err.data.get("line"), -1);
        int c = variant_cast<int>(err.data.get("column"), -1);
        str desc = variant_cast<str>(err.data.get("desc"), str());
        ctx->error(desc, null, file_position(l, c), file_position(l, c)); //td: !!! handle errors better
      }

    if (!error)
      {
        ctx->set_extents(results.begin, results.end);
        results.visit(visitor);
      }

	return !error;
  }

bool object_model::compile_class(const str& text, XSSContext ctx, om_context& octx)
  {
    document_builder callback(octx.doc);

    include_visitor iv(ctx, octx, &callback); 
    if (!compile_xs(text, ctx, octx, &iv))
	    return false;
    
    callback.notify(ctx);
    return true;
  }

bool object_model::compile_instance(XSSObject instance, const str& text, XSSContext ctx, om_context& octx)
  {
    document_builder callback(octx.doc);

    object_visitor ov(instance, ctx, octx.fixup, &callback); 
    if (!compile_xs(text, ctx, octx, &ov))
	    return false;
    
    callback.notify(ctx);
    return true;
  }

void object_model::assure_id(DataEntity de, om_context& octx)
  {
    str id = de->id();
    if (id.empty())
      {
        param_list error;
        error.add("type", de->type());
        add_error(octx.application, SNeedsId, &error, file_location()); //td: de->position
      }
  }

DataEntity object_model::assure_unique_root(DataReader dr, om_context& octx)
  {
    if (dr->root().size() != 1)
      {
        add_error(octx.application, SApplicationsMustHaveOnlyOneRoot, null, file_location());

        if (dr->root().empty())
          return DataEntity();
      }
    return dr->root()[0];
  }

void object_model::r_attr_nop(const str& attr, const str& value, const variant& this_, om_context& ctx)
  {
    //no op
  }

void object_model::r_child_nop(DataEntity de, const variant& this_, om_context& ctx)
  {
    //no op
  }

void object_model::r_idiom_namespace(const str& attr, const str& value, const variant& this_, om_context& ctx)
  {
    ctx.idiom->set_namespace(value);
  }

void object_model::r_invalid_idiom_attr(const str& attr, const str& value, const variant& this_, om_context& ctx)
  {
    assert(false);
    //ctx.xss_ctx->error();
  }

void object_model::r_idiom_enum(DataEntity de, const variant& this_, om_context& ctx)
  {
    assure_id(de, ctx);

    XSSType int_type = ctx.xss_ctx->get_type("int");

    XSSType result(new xss_type);
    result->set_id(de->id());
    result->set_output_id(de->attr("output_id"));
    result->as_enum();
    
    om_entity_visitor oev(this, result, ctx);
    oev.attribute_handler("output_id", &object_model::r_attr_nop);
    oev.attribute_handler("*",         &object_model::r_object_attr);

    oev.child_handler    ("item", &object_model::r_enum_item);
    oev.child_handler    ("*",    &object_model::r_invalid_enum_item);

    de->visit(&oev);

    ctx.xss_ctx->add_type(result, str()); //td: namespaces
  }

void object_model::r_idiom_import(DataEntity de, const variant& this_, om_context& ctx)
  {
    assure_id(de, ctx);

    XSSType type = this_;

    XSSType import = ctx.idiom->import(de->id());
    if (!import)
      {
        param_list error;
        error.add("import", de->id());
        ctx.xss_ctx->error(SUnkownImport, &error, file_position(), file_position());
        return;
      }

    type->import(import);
  }

void object_model::r_type_super(const str& attr, const str& value, const variant& this_, om_context& ctx)
  {
    XSSType super_type = ctx.xss_ctx->get_type(value, str()); //td: ctx.idiom->get_namespace()
    if (!super_type)
      {
        param_list error;
        error.add("type", value);
        add_error(ctx.application, SUnkownType, &error, file_location());
      }

    XSSType type = this_;
    type->set_type(super_type);
  }

void object_model::r_type_import(DataEntity de, const variant& this_, om_context& ctx)
  {
    assure_id(de, ctx);

    XSSType import = ctx.idiom->import(de->id());
    if (!import)
      {
        param_list error;
        error.add("import", de->id());
        ctx.xss_ctx->error(SUnkownImport, &error, file_position(), file_position());
        return;
      }

    XSSType type = this_;
    type->import(import);
  }

void object_model::r_type_constructor(DataEntity de, const variant& this_, om_context& ctx)
  {
    XSSSignature cs = read_signature(de, ctx);

    XSSType type = this_;
    type->add_constructor(cs);
  }

void object_model::r_object_property(DataEntity de, const variant& this_, om_context& ctx)
  {
    assure_id(de, ctx);
    XSSObject obj = this_;

    XSSProperty result;
    XSSProperty prop     = read_property(de, obj, ctx);
    XSSProperty obj_prop = obj->instantiate_property(de->id());
    if (obj_prop)
      {
        merge_property(obj_prop, prop, obj, ctx);
        result = obj_prop;
      }
    else
      {
        obj->insert_property(prop);
        result = prop;
      }
      
    //read property includes all loading logic, bue we still hace to search
    //for metadata, so we'll ignore all of the attributes/nodes already loaded

    om_entity_visitor oev(this, result, ctx);
    oev.attribute_handler("output_id",     &object_model::r_attr_nop);
    oev.attribute_handler("type",          &object_model::r_attr_nop);
    oev.attribute_handler("value",         &object_model::r_attr_nop);
    oev.attribute_handler("*",             &object_model::r_object_attr);

    oev.child_handler    ("get",           &object_model::r_child_nop);
    oev.child_handler    ("set",           &object_model::r_child_nop);
    oev.child_handler    ("value",         &object_model::r_child_nop);
    oev.child_handler    ("*",             &object_model::r_invalid_property_child);

    de->visit(&oev);
  
  }

void object_model::r_object_method(DataEntity de, const variant& this_, om_context& ctx)
  {
    assure_id(de, ctx);

    XSSMethod mthd = read_method(de, ctx);
    XSSObject obj = this_;
    obj->insert_method(mthd);
  }

void object_model::r_object_event(DataEntity de, const variant& this_, om_context& ctx)
  {
    assure_id(de, ctx);

    XSSEvent ev = read_event(de, ctx);
    XSSObject obj = this_;
    obj->insert_event(ev);
  }

bool object_model::parse_pca(const str& action, PARENT_CHILD_ACTION& result)
  {
    result = PCA_DEFAULT;
    if (!action.empty())
      {
        if (action == "as_property")
            result = PCA_ASPROPERTY;
        else if (action == "as_child")
            result = PCA_ASCHILD;
        else if (action == "independent")
            result = PCA_INDEPENDENT_CHILD;
        else 
            return false;
      }
    return true;
  }

bool object_model::parse_policy(DataEntity de, XSSType owner, parent_policy& result, om_context& ctx, bool& fixup)
  {
    fixup = false;

    //check wha to do when objects of this type are accepted
    str                 action = de->attr("append");
    PARENT_CHILD_ACTION pca;
    if (!parse_pca(action, pca))
      {
        assert(false); //td: error
        return false;
      }

    str by_type   = de->attr("type");
    str condition = de->attr("when");
     
    if (!by_type.empty())
      {
        if (by_type == "*")
          result.match_all = true;
        else 
          {
            XSSType policy_type = variant_cast<XSSType>(ctx.xss_ctx->get_type(by_type), XSSType());
            if (policy_type)
              result.type = policy_type;
            else
              {
                fixup = true;
                ctx.fixup.push_back(fixup_data(FIXUP_POLICY_TYPE, fixup_policy_type(by_type, owner, pca)));
              }
          }
      }
    else if (!condition.empty())
      {
        xs_utils xs;
        code_context code_ctx = ctx.xss_ctx->get_compile_context();
        
        param_list args;
        args.add("it", variant());
        code_ctx.args_ = &args;

        ByteCode bc = xs.compile_code("return " + condition + ";", code_ctx, fs::path());
        result.condition = bc;
      }

    result.action = pca;
    return true;
  }

XSSExpression object_model::compile_expression(const str& expr, XSSType type)
  {
    if (type && type->id() == "string")
      return xss_expression_utils::constant_expression(expr);

    if (!expr.empty())
      return xss_expression_utils::compile_expression(expr);

    return XSSExpression();
  }

variant object_model::str2var(const str& value)
  {
    try
      {
        int result = boost::lexical_cast<int>( value );
        return result;
      }
    catch(boost::bad_lexical_cast&)
      {
      }

    try
      {
        float result = boost::lexical_cast<float>( value );
        return result;
      }
    catch(boost::bad_lexical_cast&)
      {
      }

    try
      {
        double result = boost::lexical_cast<double>( value );
        return result;
      }
    catch(boost::bad_lexical_cast&)
      {
      }

    if (value == "true")
      return true;
    else if (value == "false")
      return false;
    
    return value;            
  }

void object_model::merge_property(XSSProperty dest, XSSProperty incoming, XSSObject owner, om_context& ctx)
  {
    XSSType incoming_type = incoming->property_type();

    //type checking
    if (incoming_type)
      {
        XSSType dest_type = dest->property_type();
        if (incoming_type && dest_type != incoming_type)
          {
            assert(false); //td: error
          }
      }

    //value
    XSSExpression incoming_value  = incoming->expr_value();
    XSSObject     incoming_ivalue = incoming->instance_value();

    if (incoming_value)
      {
        assert(!incoming_ivalue); //we shouldn't have both
        dest->expr_value(incoming_value);
        dest->instance_value(XSSObject());
      }
    else if (incoming_ivalue)
      {
        dest->expr_value(XSSExpression());
        dest->instance_value(incoming_ivalue);
      }

    //code
    XSSCode incoming_getter = incoming->code_getter();
    XSSCode incoming_setter = incoming->code_setter();

    if (incoming_getter)
      {
        if (dest->code_getter())
          {
            assert(false); //td: error, trying to override accesors?
          }

        dest->code_getter(incoming_getter);
      }

    if (incoming_setter)
      {
        if (dest->code_setter())
          {
            assert(false); //td: error, trying to override accesors?
          }

        dest->code_setter(incoming_setter);
      }
  }

void object_model::r_type_child_policy(DataEntity de, const variant& this_, om_context& ctx)
  {
    XSSType type = this_;

    parent_policy result;
    bool fixup = false;
    if (parse_policy(de, type, result, ctx, fixup))
      {
        if (fixup)
          {
            parent_policy fixup_result(true);
            type->add_child_policy(fixup_result);
          }
        else
          {
            type->add_child_policy(result);
          }
      }
  }

void object_model::r_type_reject_child(DataEntity de, const variant& this_, om_context& ctx)
  {
    XSSType type = this_;

    parent_policy result;
    bool fixup = false;
    if (parse_policy(de, XSSType(), result, ctx, fixup))
      {
        result.action = PCA_REJECT;
        if (fixup)
          {
            parent_policy fixup_result(true);
            type->add_child_policy(fixup_result);
          }
        else
          {
            type->add_child_policy(result);
          }
      }
  }

void object_model::r_type_parent_policy(DataEntity de, const variant& this_, om_context& ctx)
  {
    XSSType type = this_;

    parent_policy result;
    bool fixup = false;
    if (parse_policy(de, XSSType(), result, ctx, fixup))
      {
        if (fixup)
          {
            parent_policy fixup_result(true);
            type->add_parent_policy(fixup_result);
          }
        else
          {
            type->add_parent_policy(result);
          }
      }
  }

void object_model::r_type_reject_parent(DataEntity de, const variant& this_, om_context& ctx)
  {
    XSSType type = this_;

    parent_policy result;
    bool fixup = false;
    if (parse_policy(de, XSSType(), result, ctx, fixup))
      {
        result.action = PCA_REJECT;
        if (fixup)
          {
            parent_policy fixup_result(true);
            type->add_parent_policy(fixup_result);
          }
        else
          {
            type->add_parent_policy(result);
          }
      }
  }

void object_model::r_invalid_type_item(DataEntity de, const variant& this_, om_context& ctx)
  {
    assert(false); //td: error
  }

void object_model::r_include_class(DataEntity de, const variant& this_, om_context& ctx)
  {
    XSSType result(new xss_type());
    result->set_id(de->id());
    result->set_output_id(de->attr("output_id"));
    
    XSSContext old_ctx = ctx.xss_ctx;
    XSSContext type_ctx(new xss_context(CTXID_TYPE, result, old_ctx));
    
    ctx.xss_ctx = type_ctx;
    om_entity_visitor oev(this, result, ctx);
    oev.attribute_handler("super", &object_model::r_type_super);
    oev.attribute_handler("*",     &object_model::r_object_attr);

    oev.child_handler("property", &object_model::r_object_property);
    oev.child_handler("method",   &object_model::r_object_method);
    oev.child_handler("event",    &object_model::r_object_event);
    oev.child_handler("*",        &object_model::r_object_instance);

    de->visit(&oev);

    ctx.xss_ctx = old_ctx;
    ctx.classes.insert(std::pair<str, XSSType>(de->id(), result));
  }

void object_model::r_include_instance(DataEntity de, const variant& this_, om_context& ctx)
  {
    XSSObject result = read_object(de, ctx);
    ctx.instances.insert(std::pair<str, XSSObject>(de->id(), result));
  }

void object_model::r_invalid_include_child(DataEntity de, const variant& this_, om_context& ctx)
  {
    assert(false); //td: error
  }

void object_model::r_invalid_array_attr(const str& attr, const str& value, const variant& this_, om_context& ctx)
  {
    assert(false); //td: error
  }

void object_model::r_array_item(DataEntity de, const variant& this_, om_context& ctx)
  {
    XSSArguments value = this_;
    XSSType      value_type = value->type();

    if (value_type)
      {
        if (value_type->is_object())
          {
            assert(false); //td: for objects we may need to store instances somewhere and keep their names in the values
          }
        else
          {
            str value_str = de->attr("value");
            if (!value_str.empty())
              {
                XSSExpression result;
                if (value_type->id() == "string")
                  result = xss_expression_utils::constant_expression(value_str);
                else
                  result = xss_expression_utils::compile_expression(value_str);

                value->add(str(), result);
              }
            else
              {
                assert(false); //td: error
              }
          }
      }
    else
      {
        assert(false); //td: error
      }
  }

void object_model::r_invalid_array_item(DataEntity de, const variant& this_, om_context& ctx)
  {
    assert(false); //td: error
  }

void object_model::r_invalid_property_child(DataEntity de, const variant& this_, om_context& ctx)
  {
    assert(false); //td: error
  }

void object_model::r_invalid_event_child(DataEntity de, const variant& this_, om_context& ctx)
  {
    assert(false); //td: error
  }

XSSObject object_model::read_object_instance(DataEntity de, XSSObject parent, om_context& ctx)
  {
    if (parent->has_property(de->type())) //use case, the node refers to a property
      {
        XSSProperty prop = parent->instantiate_property(de->type());
        XSSType     type = prop->property_type();

        if (type->is_object())
          {
            //tricky: properties can contain instances, those are hard to
            //encapsulate in an expression, they will be stored on their own
            XSSObject inst = prop->instance_value();
            if (inst)
              read_object(de, ctx, inst);
            else
              {
                inst = read_object(de, ctx, XSSObject(), type);
                prop->instance_value(inst);
              }

            return inst;
          }
        else
          {
            assert(false); //td: error
          }
      }
    else
      {
        XSSObject child  = read_object(de, ctx);
        if (!child)
          ctx.fixup.push_back(fixup_data(FIXUP_OBJECT_CHILD, fixup_obj_child(parent, de)));
        else
          {
            resolve_parent_child(parent, child, ctx);
            return child;
          }
      }

    return XSSObject();
  }

XSSExpression object_model::read_array(DataEntity de, XSSType array_type, om_context& ctx)
  {
    XSSArguments args(new xss_arguments);
    args->type(array_type);

    om_entity_visitor oev(this, args, ctx);
    oev.attribute_handler("*",   &object_model::r_invalid_array_attr);
    oev.child_handler   ("item", &object_model::r_array_item);
    oev.child_handler   ("*",    &object_model::r_invalid_array_item);

    de->visit(&oev);
    
    XSSExpression result(new xss_expression);
    result->as_array(args);
    return result;
  }

void object_model::r_object_instance(DataEntity de, const variant& this_, om_context& ctx)
  {
    XSSObject parent = this_;
    read_object_instance(de, parent, ctx);
  }

void object_model::r_idiom_type(DataEntity de, const variant& this_, om_context& ctx)
  {
    XSSType result(new xss_type());
    result->set_id(de->id());
    result->set_output_id(de->attr("output_id"));
    
    om_entity_visitor oev(this, result, ctx);
    oev.attribute_handler("output_id",     &object_model::r_attr_nop);
    oev.attribute_handler("super",         &object_model::r_type_super);
    oev.attribute_handler("*",             &object_model::r_object_attr);

    oev.child_handler    ("import",        &object_model::r_type_import);
    oev.child_handler    ("constructor",   &object_model::r_type_constructor);
    oev.child_handler    ("property",      &object_model::r_object_property);
    oev.child_handler    ("method",        &object_model::r_object_method);
    oev.child_handler    ("event",         &object_model::r_object_event);
    oev.child_handler    ("accept_child",  &object_model::r_type_child_policy);
    oev.child_handler    ("reject_child",  &object_model::r_type_reject_child);
    oev.child_handler    ("accept_parent", &object_model::r_type_parent_policy);
    oev.child_handler    ("reject_parent", &object_model::r_type_reject_parent);
    oev.child_handler    ("*",             &object_model::r_object_instance);

    de->visit(&oev);
    ctx.xss_ctx->add_type(result, str());
  }

void object_model::r_invalid_idiom_child(DataEntity de, const variant& this_, om_context& ctx)
  {
    assert(false); //td: error
  }

void object_model::r_enum_item(DataEntity de, const variant& this_, om_context& ctx)
  {
    assure_id(de, ctx);

    XSSType type = this_;
    int     idx  = type->properties()->size();

    XSSExpression value = xss_expression_utils::constant_expression(idx);
    XSSProperty item(new xss_property(de->id(), value, XSSObject())); 
    type->insert_property(item);
  }

void object_model::r_invalid_enum_item(DataEntity de, const variant& this_, om_context& ctx)
  {
    assert(false); //td: error
  }

void object_model::r_object_attr(const str& attr, const str& value_str, const variant& this_, om_context& ctx)
  {
    XSSObject obj = this_;
    if (obj->has_property(attr))
      {
        XSSProperty prop      = obj->instantiate_property(attr);
        XSSType     prop_type = prop->property_type();

        XSSExpression value = compile_expression(value_str, prop_type);
        prop->expr_value(value);
      }
    
    //add as a object attribute regardless
    variant vv = str2var(value_str);
    obj->add_attribute(attr, vv);
  }

XSSType object_model::read_type(DataEntity de, Idiom parent, XSSContext ctx)
  {
    assert(false); //td: !!! cleanup
    return XSSType();
    //0.9.5
    //XSSType result(new xss_type());
    //result->set_id(de->id());
    //result->set_output_id(de->attr("output_id"));
    //
    //str super = de->attr("super");
    //if (!super.empty())
    //  {
    //    XSSType super_type = ctx->get_type(super, parent->get_namespace());
    //    if (!super_type)
    //      {
    //        param_list error;
    //        error.add("type", super);
    //        add_error(SUnkownType, &error, file_location());
    //      }

    //    result->set_type(super_type);
    //  }

    ////look for imports, these are supposed to be useful
    //entity_list imports = de->find_all("import");
    //entity_list::iterator iit = imports.begin();
    //entity_list::iterator ind = imports.end();

    //for(; iit != ind; iit++)
    //  {
    //    DataEntity id = *iit;
    //    assure_id(id);

    //    XSSType import = parent->import(id->id());
    //    if (!import)
    //      {
    //        param_list error;
    //        error.add("import", id->id());
    //        ctx->error(SUnkownImport, &error, file_position(), file_position());
    //        continue;
    //      }

    //    result->import(import);
    //  }

    ////read constructors
    //entity_list ctors = de->find_all("constructor");
    //entity_list::iterator cit = ctors.begin();
    //entity_list::iterator cnd = ctors.end();

    //for(; cit != cnd; cit++)
    //  {
    //    DataEntity cd = *cit;
    //    XSSSignature cs = read_signature(cd, parent, ctx);

    //    result->add_constructor(cs);
    //  }

    ////read properties
    //entity_list props = de->find_all("property");
    //entity_list::iterator pit = props.begin();
    //entity_list::iterator pnd = props.end();

    //for(; pit != pnd; pit++)
    //  {
    //    DataEntity pd = *pit;
    //    assure_id(pd);

    //    XSSProperty prop = read_property(pd, parent, ctx);

    //    result->insert_property(prop);
    //  }

    ////read methods
    //entity_list mthds = de->find_all("method");
    //entity_list::iterator mit = mthds.begin();
    //entity_list::iterator mnd = mthds.end();

    //for(; mit != mnd; mit++)
    //  {
    //    DataEntity md = *mit;
    //    assure_id(md);

    //    XSSMethod mthd = read_method(md, parent, ctx);
    //    result->insert_method(mthd);
    //  }

    ////read events
    //entity_list events = de->find_all("event");
    //entity_list::iterator eit = mthds.begin();
    //entity_list::iterator end = mthds.end();

    //for(; eit != end; eit++)
    //  {
    //    DataEntity ed = *eit;
    //    assure_id(ed);

    //    XSSEvent ev = read_event(ed, parent, ctx);
    //    result->insert_event(ev);
    //  }

    //return result;
  }

XSSSignature object_model::read_signature(DataEntity de, om_context& ctx)
  {
    XSSSignature result(new xss_signature);
    str native = de->attr("native_signature");
    if (!native.empty())
      result->native(native);

    entity_list items = de->find_all("argument");
    entity_list::iterator it = items.begin();
    entity_list::iterator nd = items.end();

    int idx = 0;
    for(; it != nd; it++)
      {
        DataEntity ade = *it;
        assure_id(ade, ctx);

        str id    = ade->id();
        str stype = ade->attr("type");
        XSSType type;
        if (stype.empty())
          {
            param_list error;
            error.add("arg", id);
            ctx.xss_ctx->error(SMissingArgType, &error, file_position(), file_position());
          }
        else
          {
            type = ctx.xss_ctx->get_type(stype, ctx.idiom->get_namespace());
            if (!type)
              {
                ctx.fixup.push_back(fixup_data(FIXUP_ARGUMENT_TYPE, fixup_arg_type(stype, result, idx)));
                
                //td: !!! fixup errors
                //param_list error;
                //error.add("type", stype);
                //ctx->error(SUnkownType, &error, file_position(), file_position());
              }
          }

        XSSExpression default_value = read_expression(ade, type, "default_value");
        result->add_argument(id, type, default_value);
      }

    return result;
  }

XSSProperty object_model::read_property(DataEntity de, XSSObject recipient, om_context& ctx)
  {
    str         prop_id = de->id();
    XSSProperty result;
    XSSType     type;
    str         stype;

    if (recipient && recipient->has_property(prop_id))
      {
        result = recipient->instantiate_property(prop_id);
        type   = result->property_type();
      }
    else
      {
        //new prop
        result = XSSProperty(new xss_property());
        result->set_id(de->id());
        result->set_output_id(de->attr("output_id"));

        stype = de->attr("type");
        if (stype.empty())
          {
            type = ctx.xss_ctx->get_type("var");
          }
        else if (stype == "array")
          {
            str at = de->attr("array_type");
            if (at.empty())
              at = "var";
        
            XSSType array_type = ctx.xss_ctx->get_type(at);
            if (!array_type)
              {
                param_list error;
                error.add("type", stype);
                error.add("property", de->id());
                ctx.xss_ctx->error(SUnkownType, &error, file_position(), file_position());
              }
            else
              type = ctx.xss_ctx->get_array_type(array_type);
          }
        else
          {
            type = ctx.xss_ctx->get_type(stype, str()); //idiom->get_namespace()
          }

        result->property_type(type);
    
        //getter/setter
        DataEntity ge = de->get("get");
        if (ge)
          {
            param_list iparams;
            iparams.add("path", variant());
            
            InlineRenderer getter = read_inline_renderer(ge, iparams);
            result->getter(getter);
          }

        DataEntity se = de->get("set");
        if (se)
          {
            param_list iparams;
            iparams.add("path",  str());
            iparams.add("value", str());

            InlineRenderer setter = read_inline_renderer(se, iparams);
            result->setter(setter);
          }
      }

    //assure typing
    if (!type)
      {
        param_list error;
        error.add("type", stype);
        error.add("property", de->id());
        ctx.xss_ctx->error(SUnkownType, &error, file_position(), file_position());
        return XSSProperty(); //td: fixup this
      }

    str value_str = de->attr("value");
    if (!value_str.empty())
      {
        XSSExpression value = read_expression(de, type, "value");
        result->expr_value(value);
      }
    else
      {
        DataEntity vde = de->get("value");
        if (vde)
          {
            //this is a value in node form, so we'll assume its either an object or an array
            if (type->is_array())
              {
                XSSType       array_type = type->array_type();
                XSSExpression value      = read_array(vde, array_type, ctx);
                result->expr_value(value);
              }
            else if (type->is_object())
              {
                XSSObject value = result->instance_value();
                if (value)
                  read_object(vde, ctx, value, type);
                else
                  value = read_object(vde, ctx, XSSObject(), type);

                result->instance_value(value);
              }
            else
              {
                assert(false); //td: error
              }
          }
      }
    return result;
  }

XSSMethod object_model::read_method(DataEntity de, om_context& ctx)
  {
    XSSMethod result(new xss_method());
    result->set_id(de->id());
    result->set_output_id(de->attr("output_id"));

    str stype = de->attr("return_type");
    if (!stype.empty())
      {
        XSSType type = ctx.xss_ctx->get_type(stype, ctx.idiom->get_namespace());
        if (!type)
          {
            param_list error;
            error.add("type", stype);
            error.add("method", de->id());
            ctx.xss_ctx->error(SUnkownType, &error, file_position(), file_position());

            type = ctx.xss_ctx->get_type("var");
          }
        
        result->return_type(type);
      }

    DataEntity ce = de->get("call");
    if (ce)
      {
        param_list iparams;
        iparams.add("path",  str());
        iparams.add("args",  str());

        InlineRenderer caller = read_inline_renderer(ce, iparams);
        result->renderer(caller);
      }

    XSSSignature sig = read_signature(de, ctx);

    result->signature(sig);

    return result;
  }

XSSEvent object_model::read_event(DataEntity de, om_context& ctx)
  {
    XSSEvent result(new xss_event);
    result->set_id(de->id());
    result->set_output_id(de->attr("output_id"));

    DataEntity dse = de->get("dispatch");
    if (dse)
      {
        param_list iparams;
        iparams.add("path",  str());
        iparams.add("args",  str());

        InlineRenderer dispatcher = read_inline_renderer(dse, iparams);
        result->set_dispatcher(dispatcher);
      }

    XSSSignature sig = read_signature(de, ctx);
    result->set_signature(sig);

    om_entity_visitor oev(this, XSSObject(result), ctx);
    oev.attribute_handler("output_id",     &object_model::r_attr_nop);
    oev.attribute_handler("*",             &object_model::r_object_attr);

    oev.child_handler    ("dispatch",      &object_model::r_child_nop);
    oev.child_handler    ("*",             &object_model::r_invalid_event_child);

    de->visit(&oev);

    return result;
  }

InlineRenderer object_model::read_inline_renderer(DataEntity de, param_list& params)
  {
    bool global = de->attr("global") == "true";
    str  text   = de->attr("text");

    InlineRenderer result(new inline_renderer());
    result->compile(text, global, params);
    return result;
  }

void object_model::register_idiom(const str& id, Application app, Idiom idiom)
  {
    idiom_list::iterator it = idioms_.find(id);
    if (it != idioms_.end())
      {
        param_list error;
        error.add("import", id);
        add_error(app, SDupIdiom, &error, file_location());
        return;
      }

    idioms_.insert(std::pair<str, Idiom>(id, idiom));
  }

Idiom object_model::find_idiom(const str& idiom)
  {
    idiom_list::iterator it = idioms_.find(idiom);
    if (it != idioms_.end())
      return it->second;
    
    return Idiom();
  }

XSSExpression object_model::read_expression(DataEntity de, XSSType type, const str& attribute)
  {
    str svalue = de->attr(attribute);

    if (type && type->id() == "string")
      return xss_expression_utils::constant_expression(svalue);

    if (!svalue.empty())
      return xss_expression_utils::compile_expression(svalue);

    return XSSExpression();
  }

XSSObject object_model::read_object(DataEntity de, om_context& ctx, XSSObject instance, XSSType type)
  {
    if (!type)
      {
        if (instance)
          type = instance->type();
        else 
          type = ctx.xss_ctx->get_type(de->type());
        
        if (!type)
          return XSSObject();
      }

    XSSObject result = instance;
    
    if (!result)
      {
        result = XSSObject(new xss_object);
        result->set_id(de->id());
        result->set_type(type);
      }

    om_entity_visitor oev(this, result, ctx);
    oev.attribute_handler("*", &object_model::r_object_attr);

    oev.child_handler("property",    &object_model::r_object_property);
    oev.child_handler("method",      &object_model::r_object_method);
    oev.child_handler("event",       &object_model::r_object_event);
    oev.child_handler("*",           &object_model::r_object_instance);

    de->visit(&oev);

    return result;
  }

document* object_model::create_document(Application app, const str& src_file, XSSContext ctx)
  {
    fs::path src = fs_->locate(src_file, ctx->path());
    if (!fs::exists(src))
      {
        param_list error;
        error.add("file", src_file);
        ctx->error("File not found", &error, file_position(), file_position());
        return null;
      }

    return app->create_document(src, ctx);
  }

void object_model::fix_it_up(XSSContext ctx, om_context& octx)
  {
    fixup_list process = octx.fixup;
    octx.fixup.clear();
    int count = 0;
    while (!process.empty() && count < 5)
      {
        fixup_list::iterator it = process.begin();
        fixup_list::iterator nd = process.end();
    
        for(; it != nd; it++)
          {
            switch(it->id)
              {
                case FIXUP_OBJECT_TYPE:
                  {
                    fixup_type fu   = it->data;
                    XSSType type = ctx->get_type(fu.type_name);
                    if (check_type(type, fu.type_name, ctx))
                      fu.target->set_type(type);
                    break;
                  }
                case FIXUP_PROPERTY_TYPE:
                  {
                    fixup_property_type fu = it->data;
                    XSSType type = ctx->get_type(fu.type_name);
				            if (check_type(type, fu.type_name.name, ctx))
                      fu.target->property_type(type);
                    break;
                  }
                case FIXUP_RETURN_TYPE:
                  {
                    fixup_return_type fu = it->data;
                    XSSType type = ctx->get_type(fu.type_name);
                    if (check_type(type, fu.type_name, ctx))
                      fu.target->return_type(type);
                    break;
                  }
                case FIXUP_INSTANCE_EVENT:
                  {
                    fix_instance_event fu = it->data;
                    std::ostringstream expr_str;
                    
                    //td: move this by itself, may need it somewhere else
                    std::vector<str>::iterator it = fu.instance.begin();
                    std::vector<str>::iterator nd = fu.instance.end();
                    
                    bool first = true;
                    str  event_id;
                    for(; it != nd; it++)
                      {
                        bool last = (it + 1) == nd;
                        if (last)
                          {
                            event_id = *it;   
                            break;
                          }

                        if (first)
                          first = false;
                        else
                          expr_str << '.';

                        expr_str << *it;
                      }

                    XSSExpression instance = xss_expression_utils::compile_expression(expr_str.str()); assert(instance);
                    instance->bind(fu.ctx);
                    value_operation vop = instance->value()->get_last();
                    if (vop.bound())
                      {
                        if (vop.resolve_id() != RESOLVE_INSTANCE)
                          {
                            assert(false); //td: error
                          }
                        else
                          {
                            XSSObject owner = vop.resolve_value();
                            XSSEvent  ev = owner->get_event(event_id);
                            if (!ev)
                              {
                                param_list error;
                                error.add("object", owner->id());
                                error.add("event", event_id);
                                ctx->error(SEventNotFound, &error, file_position(), file_position());
                              }
                            else
                              {
                                fu.target->add_event_impl(ev, fu.code, ev->signature(), owner, instance);
                              }
                          }
                      }
                    break;
                  }
                case FIXUP_SUPER_TYPE: 
                  {
                    fixup_super fu = it->data;
                    XSSType type = ctx->get_type(fu.super);
                    if (check_type(type, fu.super, ctx))
                      fu.type->set_type(type);
                    break;
                  }
                case FIXUP_ARGUMENT_TYPE:
                  {
                    fixup_arg_type fu = it->data;
                    XSSType type = ctx->get_type(fu.type);
                    if (check_type(type, fu.type, ctx))
                      fu.args->arg_type(fu.idx, type);
                    break;
                  }
                case FIXUP_OBJECT_CHILD:
                  {
                    fixup_obj_child fu    = it->data;
                    XSSObject       child = read_object_instance(fu.de, fu.parent, octx);

                    if (!child)
                      {
                        //td: error
                      }
                    break;
                  }
                default:
                  assert(false); //what am I missing?
              }
          }

        process = octx.fixup;
        octx.fixup.clear();

        count++;
      }
  }

void object_model::bind_it_up(XSSContext ctx, om_context& octx)
  {
    instance_map::iterator it = octx.instances.begin();
    instance_map::iterator nd = octx.instances.end();

    for(; it != nd; it++)
      {
        XSSObject  singleton = it->second;
        ctx->register_symbol(RESOLVE_INSTANCE, it->first, singleton);
      }

    //then bind
    type_map::iterator cit = octx.classes.begin();
    type_map::iterator cnd = octx.classes.end();

    for(; cit != cnd; cit++)
      {
        XSSType    type = cit->second;

        context_map::iterator ctxit = octx.contexts.find(cit->first);
        assert(ctxit != octx.contexts.end());

        type->bind(ctxit->second);
      }

    it = octx.instances.begin();
    nd = octx.instances.end();

    for(; it != nd; it++)
      {
        XSSObject  singleton = it->second;

        context_map::iterator ctxit = octx.contexts.find(it->first);
        assert(ctxit != octx.contexts.end());

        singleton->bind(ctxit->second);
      }
  }

bool object_model::check_type(XSSType type, const str& type_name, XSSContext ctx)
  {
    if (type)
      return true;

    param_list error;
		error.add("class", type_name);
		ctx->error(SUnkownType, &error, file_position(), file_position()); //td: wonky
    return false;
  }

Application object_model::app_by_file(const fs::path& path)
  {
    application_list::iterator it = apps_.begin();
    application_list::iterator nd = apps_.end();

    for(; it != nd; it++)
      {
        document* doc = it->second->get_document(path);
        if (doc)
          return it->second;
      }
    return Application();
  }

bool object_model::changed()
  {
	bool result = changed_;
	changed_ = false;
	return result;
  }

void object_model::add_error(Application app, const str& desc, param_list* info, file_location& loc)
  {
    ////find out who's error is it
    //Application app;
    //if (loc.file.empty())
    //  {
    //    if (apps_.size() != 1)
    //      return; //should I assert this? 
    //  }
    //else
    //  {
    //    app = app_by_file(loc.file); 
    //  }

    assert(app);
    app->add_error(desc, info, loc);
  }

void object_model::visit_errors(const fs::path& fname, error_visitor* visitor)
  {
    application_list::iterator it = apps_.begin();
    application_list::iterator nd = apps_.end();

    for(; it != nd; it++)
      {
		if (it->second)
			it->second->visit_errors(fname, visitor);
      }
  }

void object_model::clear_file_errors(const str& fname)
  {
    Application app = app_by_file(fname); assert(app);
    app->clear_file_errors(fname);
  }

void object_model::parent_child_action(PARENT_CHILD_ACTION action, XSSObject parent, XSSObject child, om_context& ctx)
  {
    switch(action)
      {
        case PCA_ASPROPERTY:
          {
            //add the child as a property
            str child_id = child->id(); 
            assert(!child_id.empty()); //td: naming policy
            
            XSSProperty prop = parent->get_property(child_id);
            if (!prop)
              prop = parent->add_property(child_id, XSSExpression());
            else
              {
                assert(false); //td: error
              }

            prop->property_type(child->type());
            prop->instance_value(child);
            break;
          }
        case PCA_ASCHILD:
          {
            parent->add_child(child);
            break;
          }
        case PCA_INDEPENDENT_CHILD:
          {
            //just register the instance, however, the parent relationship 
            //will be kept
            child->set_parent(parent); 
            break;
          }
        default:
          assert(false); //catch
      }

    //we also must decide what to do with the child entity
    //this code is here since the child instance has just been created
    if (!ctx.xss_ctx->add_instance(child))
      ctx.application->add_instance(child);
  }

void object_model::resolve_parent_child(XSSObject parent, XSSObject child, om_context& ctx)
  {
    //find the parent type (it could be a type itself)
    xss_type* parent_type = dynamic_cast<xss_type*>(parent.get());
    if (!parent_type)
      parent_type = parent->type().get();

    assert(parent_type);
    param_list error; //just in case
    error.add("parent", parent->id());
    error.add("child", child->id());

    PARENT_CHILD_ACTION pca = PCA_DEFAULT;
    if (parent_type->accepts_child(child, pca))
      {
        parent_child_action(pca, parent, child, ctx);
      }
    else if (pca == PCA_REJECT)
      {
        ctx.xss_ctx->error(SInvalidChild, &error, file_position(), file_position());
      }
    else
      {
        //probe the child
        XSSType child_type = child->type(); assert(child_type);
        if (child_type->accepts_parent(parent, pca))
          {
            parent_child_action(pca, parent, child, ctx);
          }
        else
          {
            ctx.xss_ctx->error(SInvalidChild, &error, file_position(), file_position());
          }
      }
  }

//object_model_thread
int object_model_thread::compile_request(const str& text, const str& id, Application app, APPLICATION_ITEM ai)
  {
    boost::mutex::scoped_lock l(mutex_);
    
    int result = id_++;

    //check there's no other reuqest queued
    request_list::iterator it = requests_.begin();
    request_list::iterator nd = requests_.end();
    for(; it != nd; it++)
      {
        if (it->id == id)
          {
            it->text = text;
            it->response_id = result;
            return result;
          }
      }

    requests_.push_back(om_request(id, ai, text, result, app));
    return result;
  }

bool object_model_thread::get(const str& id, om_response& result)
  {
    boost::mutex::scoped_lock l(mutex_);

    response_map::iterator it = responses_.find(id); 
    if (it != responses_.end())
      {
        result = it->second;
        return true;
      }

    return false;
  }

bool object_model_thread::fetch(str& id, om_response& result)
  {
    boost::mutex::scoped_lock l(mutex_);

    response_map::iterator it = responses_.begin(); 
    if (it != responses_.end())
      {
		    id = it->first;
        result = it->second;
		    responses_.erase(it);
        return true;
      }

    return false;
  }

void object_model_thread::run()
  {
    assert(stopped_);
	  stopped_ = false;
    thread_  = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&object_model_thread::do_work, this)));  
  }

void object_model_thread::do_work()
  {
      while (true)
        {
          if (stopped_)
            break;

          bool       got_work = false;
          om_request work;
          {
            boost::mutex::scoped_lock l(mutex_);
            if (!requests_.empty())
              {
                got_work = true;
                work = *requests_.begin();  
                requests_.erase(requests_.begin());
              }
          }

          if (!got_work)
            boost::thread::sleep(boost::get_system_time() + boost::posix_time::milliseconds(250));
          else
            {
              str        code = work.text;
              document   doc;
              om_context octx;
              octx.doc = &doc;
			        octx.application = work.app;

              XSSContext ctx(new xss_context(CTXID_FILE, work.id, XSSContext()));
              ctx->errors(ErrorHandler(new local_error_handler));

              switch(work.app_item)
                {
                  case AI_APP:
                  case AI_INSTANCE:
                    {
                      if (!octx.instance)
                        {
                          octx.instance = XSSObject(new xss_object);
                          if (work.app_item == AI_APP)
                            {
                              octx.instance->set_id("application"); //td: !!! item engine
                            }
                        }

                      XSSContext ictx(new xss_context(CTXID_INSTANCE, octx.instance, ctx));
                      object_model::compile_instance(octx.instance, code, ictx, octx);

						          //assign the whole file to this instance
					            file_position b, e;
						          text_utils::text_extents(code, b, e);
						          ictx->set_extents(b, e);

		                  octx.contexts.insert(std::pair<str, XSSContext>(octx.instance->id(), ictx));
                      octx.instances.insert(std::pair<str, XSSObject>(octx.instance->id(), octx.instance));
                      break;
                    }
                  case AI_CLASS:
                    {
                      object_model::compile_class(code, ctx, octx);
                      break;
                    }
                  default: assert(false);
                }

              boost::mutex::scoped_lock l(mutex_);
              response_map::iterator it = responses_.find(work.id);
              if (it != responses_.end())
                it->second = om_response(work.response_id, ctx, octx, doc, work.id, work.app_item);
              else
                responses_.insert(response_pair(work.id, om_response(work.response_id, ctx, octx, doc, work.id, work.app_item)));
            }
        }
  }           
