
#include <xss/object_model.h>
#include <xss/xml_data_reader.h>
#include <xss/xss_context.h>
#include <xss/xss_error.h>
#include <xss/xss_expression.h>
#include <xss/xss_code.h>
#include <xss/xss_compiler.h>

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
    JsonOutput   json; 
    XSSCompiler compiler(new xss_compiler(&json));
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
    DataEntity  prj = assure_unique_root(project);

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
    Language   lang   = read_language(prj);
    XSSContext global = lang->create_context();
    global->set_path(base_path);
    global->set_language(lang);

    ErrorHandler eh(new om_error_handler(result));
    global->errors(eh);

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
            add_error(SNamelessProjectParameter, null, file_location());
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
          add_error(SMustProvideEntryPointForApplicationType, null, file_location());
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
            add_error(SIdiomFileNotFound, &error, file_location());
          }

        Idiom idiom = find_idiom(idiom_path.string());
        
        if (!idiom)
          {
            DataReader idiom_data = fs_->load_data(idiom_path);
            idiom = read_idiom(assure_unique_root(idiom_data), global);
            register_idiom(idiom_path.string(), idiom);
          }

        assert(idiom);
        idiom->bind(global);
      }

    //load includes
    entity_list includes = prj->find_all("include");
    entity_list::iterator iit = includes.begin();
    entity_list::iterator ind = includes.end();

    //results
    om_context octx;
    for(; iit != ind; iit++)
      {
        DataEntity _include = *iit;
        str        def = _include->attr("def");  
        str        src = _include->attr("src");  

        XSSContext include_ctx(new xss_context(CTXID_FILE, src, global));
        handle_include(result, def, src, include_ctx, octx);
      }

    //load application
    str def = prj->attr("def");
    str src = prj->attr("src");
    XSSContext app_ctx(new xss_context(CTXID_FILE, src, global));
    XSSObject  app_obj(new xss_object);
	  app_obj->set_id("application");
    handle_instance(result, app_obj, def, src, app_ctx, octx);

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
                add_error(it->desc, &it->info, it->loc);
			        }

            return;
          }

        Application app = response.data.application; assert(app);

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
    handle_include(app, def, src, include_ctx, octx);

    fix_it_up(global, octx);
    bind_it_up(global, octx);
  }

Language object_model::read_language(DataEntity project)
  {
    str language_name = project->attr("language");
    if (language_name.empty())
      {
        add_error(SMustProvideLanguageForApplicationType, null, file_location());
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

Idiom object_model::read_idiom(DataEntity de, XSSContext ctx)
  {
    Idiom result(new idiom);
    str iid = de->id();

    if (iid.empty())
      ctx->error(SIdiomsMustHaveId, null, file_position(), file_position());

    str _namespace = de->attr("namespace");
    if (_namespace.empty())
      _namespace = iid;

    result->set_namespace(_namespace);
    
    //read enums
    entity_list           enums = de->find_all("enum");
    entity_list::iterator eit   = enums.begin();
    entity_list::iterator end   = enums.end();

    for(; eit != end; eit++)
      {
        DataEntity ed = *eit;
        assure_id(ed);

        XSSType type = read_enum(ed, ctx);
        result->add_type(type);
      }
    
    //read imports
    entity_list imports = de->find_all("import");
    entity_list::iterator iit = imports.begin();
    entity_list::iterator ind = imports.end();

    for(; eit != end; eit++)
      {
        DataEntity ed = *eit;
        assure_id(ed);

        str super = ed->attr("super");
        if (!super.empty())
          {
            param_list error;
            error.add("import", ed->id());
            ctx->error(SImportCannotInherit, &error, file_position(), file_position()); //td: !!! data entity file pos
          }

        XSSType type = read_type(ed, result, ctx);
        result->add_import(type);
      }

    //read types
    entity_list           types = de->find_all("class");
    entity_list::iterator it    = types.begin();
    entity_list::iterator nd    = types.end();

    for(; it != nd; it++)
      {
        DataEntity td = *it;
        assure_id(td);
        
        XSSType type = read_type(td, result, ctx);
        result->add_type(type);
      }

    return result;
  }

void object_model::handle_include(Application app, const str& def_file, const str& src_file, XSSContext ctx, om_context& octx)
  {
    fs::path path;
    if (!def_file.empty())
      {
        DataReader def      = fs_->load_data(def_file, ctx->path());
        DataEntity def_root = assure_unique_root(def);

        read_include_def(def_root, ctx, octx);
        read_include_singleton(def_root, ctx, octx);
      }

    if (!src_file.empty())
      {
        fs::path src_path = fs_->locate(src_file, ctx->path());
        if (src_path.empty())
          { 
            param_list error;
            error.add("file", src_file);
            add_error(SFileNotFound, &error, file_location());
            return;
          }

        ctx->source_file(src_path);

        str       code = fs_->load_file(src_path);
        document* doc  = create_document(app, src_file, ctx);
        
        assert(!octx.doc);
        octx.doc = doc;
        compile_class(code, ctx, octx);
        octx.doc = null;
			}
  }

void object_model::handle_instance(Application app, XSSObject instance, const str& def_file, const str& src_file, XSSContext ctx, om_context& octx)
  {
    fs::path path;
    if (!def_file.empty())
      {
        DataReader def      = fs_->load_data(def_file, ctx->path());
        DataEntity def_root = assure_unique_root(def);

        assert(false);
        //read_instance(def_root, ctx, octx);
      }

    if (!src_file.empty())
      {
        fs::path src_path = fs_->locate(src_file, ctx->path());
        if (src_path.empty())
          { 
            param_list error;
            error.add("file", src_file);
            add_error(SFileNotFound, &error, file_location());
            return;
          }

        ctx->source_file(src_path);

        str       code = fs_->load_file(src_path);
        document* doc  = create_document(app, src_file, ctx);
        
        assert(!octx.doc);
        octx.doc = doc;
        octx.instance = instance;

        XSSContext ictx(new xss_context(CTXID_INSTANCE, instance, ctx));
        compile_instance(instance, code, ictx, octx);
        
		    //assign the whole file to this instance
        file_position b, e;
		    text_utils::text_extents(code, b, e);
		    ictx->set_extents(b, e);
        doc->refresh_context(ictx);

        //output
		    octx.contexts.insert(std::pair<str, XSSContext>(instance->id(), ictx));
        octx.instances.insert(std::pair<str, XSSObject>(instance->id(), instance));
        octx.doc = null;
        octx.instance.reset();
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
    fix_instance_event(std::vector<str>& _instance, XSSSignature _sig, XSSCode _code):
      instance(_instance),
      sig(_sig),
      code(_code)
      {
      }

    std::vector<str> instance;
    XSSSignature     sig;
    XSSCode          code;
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
            fixup_.push_back(fixup_data(FIXUP_INSTANCE_EVENT, fix_instance_event(info.name, sig, code)));
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

        target_->add_event_impl(ev, code);
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

void object_model::assure_id(DataEntity de)
  {
    str id = de->id();
    if (id.empty())
      {
        param_list error;
        error.add("type", de->type());
        add_error(SNeedsId, &error, file_location()); //td: de->position
      }
  }

DataEntity object_model::assure_unique_root(DataReader dr)
  {
    if (dr->root().size() != 1)
      {
        add_error(SApplicationsMustHaveOnlyOneRoot, null, file_location());

        if (dr->root().empty())
          return DataEntity();
      }
    return dr->root()[0];
  }

XSSType object_model::read_type(DataEntity de, Idiom parent, XSSContext ctx)
  {
    XSSType result(new xss_type());
    result->set_id(de->id());
    result->set_output_id(de->attr("output_id"));
    
    str super = de->attr("super");
    if (!super.empty())
      {
        XSSType super_type = ctx->get_type(super, parent->get_namespace());
        if (!super_type)
          {
            param_list error;
            error.add("type", super);
            add_error(SUnkownType, &error, file_location());
          }

        result->set_type(super_type);
      }

    //look for imports, these are supposed to be useful
    entity_list imports = de->find_all("import");
    entity_list::iterator iit = imports.begin();
    entity_list::iterator ind = imports.end();

    for(; iit != ind; iit++)
      {
        DataEntity id = *iit;
        assure_id(id);

        XSSType import = parent->import(id->id());
        if (!import)
          {
            param_list error;
            error.add("import", id->id());
            ctx->error(SUnkownImport, &error, file_position(), file_position());
            continue;
          }

        result->import(import);
      }

    //read constructors
    entity_list ctors = de->find_all("constructor");
    entity_list::iterator cit = ctors.begin();
    entity_list::iterator cnd = ctors.end();

    for(; cit != cnd; cit++)
      {
        DataEntity cd = *cit;
        XSSSignature cs = read_signature(cd, parent, ctx);

        result->add_constructor(cs);
      }

    //read properties
    entity_list props = de->find_all("property");
    entity_list::iterator pit = props.begin();
    entity_list::iterator pnd = props.end();

    for(; pit != pnd; pit++)
      {
        DataEntity pd = *pit;
        assure_id(pd);

        XSSProperty prop = read_property(pd, parent, ctx);

        result->insert_property(prop);
      }

    //read methods
    entity_list mthds = de->find_all("method");
    entity_list::iterator mit = mthds.begin();
    entity_list::iterator mnd = mthds.end();

    for(; mit != mnd; mit++)
      {
        DataEntity md = *mit;
        assure_id(md);

        XSSMethod mthd = read_method(md, parent, ctx);
        result->insert_method(mthd);
      }

    //read events
    entity_list events = de->find_all("event");
    entity_list::iterator eit = mthds.begin();
    entity_list::iterator end = mthds.end();

    for(; eit != end; eit++)
      {
        DataEntity ed = *eit;
        assure_id(ed);

        XSSEvent ev = read_event(ed, parent, ctx);
        result->insert_event(ev);
      }

    return result;
  }

XSSType object_model::read_enum(DataEntity de, XSSContext ctx)
  {
    XSSType int_type = ctx->get_type("int");

    XSSType result(new xss_type);
    result->set_id(de->id());
    result->set_output_id(de->attr("output_id"));
    result->as_enum();
    
    entity_list items = de->find_all("item");
    entity_list::iterator it = items.begin();
    entity_list::iterator nd = items.end();

    int idx = 0;
    for(; it != nd; it++)
      {
        DataEntity de = *it;
        assure_id(de);

        XSSProperty item(new xss_property(de->id(), int_type, idx, XSSObject())); //td: 0.9.5 rid of last parameter
        result->insert_property(item);
      }

    return result;
  }

XSSSignature object_model::read_signature(DataEntity de, Idiom idiom, XSSContext ctx)
  {
    XSSSignature result(new xss_signature);

    entity_list items = de->find_all("argument");
    entity_list::iterator it = items.begin();
    entity_list::iterator nd = items.end();

    int idx = 0;
    for(; it != nd; it++)
      {
        DataEntity ade = *it;
        assure_id(ade);

        str id    = ade->id();
        str stype = ade->attr("type");

        XSSType type = ctx->get_type(stype, idiom->get_namespace());
        if (!type)
          {
            param_list error;
            error.add("import", stype);
            ctx->error(SUnkownType, &error, file_position(), file_position());
          }

        XSSExpression default_value = read_expression(ade, type, "default_value");
        result->add_argument(id, type, default_value);
      }

    return result;
  }

XSSProperty object_model::read_property(DataEntity de, Idiom idiom, XSSContext ctx)
  {
    XSSProperty result(new xss_property());
    result->set_id(de->id());
    result->set_output_id(de->attr("output_id"));

    str stype = de->attr("type");
    XSSType type = ctx->get_type(stype, idiom->get_namespace());
    if (!type)
      {
        param_list error;
        error.add("type", stype);
        error.add("property", de->id());
        ctx->error(SUnkownType, &error, file_position(), file_position());

        type = ctx->get_type("var");
      }

    result->set_type(type);
    
    XSSExpression value = read_expression(de, type, "value");
    result->expr_value(value);

    //getter/setter
    DataEntity ge = de->get("get");
    if (ge)
      {
        InlineRenderer getter = read_inline_renderer(ge);
        result->getter(getter);
      }

    DataEntity se = de->get("set");
    if (se)
      {
        InlineRenderer setter = read_inline_renderer(se);
        result->setter(setter);
      }

    return result;
  }

XSSMethod object_model::read_method(DataEntity de, Idiom idiom, XSSContext ctx)
  {
    XSSMethod result(new xss_method());
    result->set_id(de->id());
    result->set_output_id(de->attr("output_id"));

    str stype = de->attr("return_type");
    if (!stype.empty())
      {
        XSSType type = ctx->get_type(stype, idiom->get_namespace());
        if (!type)
          {
            param_list error;
            error.add("type", stype);
            error.add("method", de->id());
            ctx->error(SUnkownType, &error, file_position(), file_position());

            type = ctx->get_type("var");
          }
        
        result->return_type(type);
      }

    DataEntity ce = de->get("call");
    if (ce)
      {
        InlineRenderer caller = read_inline_renderer(ce);
        result->renderer(caller);
      }

    XSSSignature sig = read_signature(de, idiom, ctx);
    result->signature(sig);

    return result;
  }

XSSEvent object_model::read_event(DataEntity de, Idiom idiom, XSSContext ctx)
  {
    XSSEvent result(new xss_event);
    result->set_id(de->id());
    result->set_output_id(de->attr("output_id"));

    DataEntity dse = de->get("dispatch");
    if (dse)
      {
        InlineRenderer dispatcher = read_inline_renderer(dse);
        result->set_dispatcher(dispatcher);
      }

    XSSSignature sig = read_signature(de, idiom, ctx);
    result->set_signature(sig);

    return result;
  }

InlineRenderer object_model::read_inline_renderer(DataEntity de)
  {
    bool global = de->attr("global") == "true";
    str  text   = de->attr("text");

    return InlineRenderer(new inline_renderer(text, global));
  }

void object_model::register_idiom(const str& id, Idiom idiom)
  {
    idiom_list::iterator it = idioms_.find(id);
    if (it != idioms_.end())
      {
        param_list error;
        error.add("import", id);
        add_error(SDupIdiom, &error, file_location());
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

    if (type->id() == "string")
      return xss_expression_utils::constant_expression(svalue);

    if (!svalue.empty())
      return xss_expression_utils::compile_expression(svalue);

    return XSSExpression();
  }

void object_model::read_include_def(DataEntity de, XSSContext ctx, om_context& octx)
  {
    return; //td: !!!
	  
	entity_list           classes = de->find_all("class");
    entity_list::iterator it      = classes.begin();
    entity_list::iterator nd      = classes.end();

    for(; it != nd; it++)
      {
        DataEntity cd = *it;

        str cid   = cd->id();
        str super = cd->attr("super");
        
        assert(false); //td: read_object
				//std::map<str, int>::iterator dcit = def_types.find(cid);
				//if (dcit != def_types.end())
				//	{
				//		param_list error;
				//		error.add("id", SProjectError);
				//		error.add("desc", SDuplicateClassOnLibrary);
				//		error.add("class", cid);
				//		xss_throw(error);
				//	}

    //    def_types.insert(std::pair<str, int>(cid, classdefs.size()));
				//classdefs.push_back(clazz_data);

				//XSSType clazz(new xss_type());
    //    clazz->set_id(cid);
				//classes.push_back(clazz);

    //    //register early
    //    XSSModule module = app->type_idiom(super);
    //    if (module)
    //      module->register_user_type(clazz);
    //    else
    //      app_types_.push_back(clazz);

    //    //mark as defined user type
    //    clazz->add_attribute("user_defined", true);

    //    ctx->add_type(cid, clazz);
      }
  }

void object_model::read_include_singleton(DataEntity de, XSSContext ctx, om_context& octx)
  {
    entity_list           singletons = de->find_all("instance");
    entity_list::iterator it         = singletons.begin();
    entity_list::iterator nd         = singletons.end();

    for(; it != nd; it++)
      {
        assert(false); //td: read_object 
        //if (clazz_data->type_name() == "instance")
        //  {
        //    str type_name = clazz_data->get<str>("super", str());
        //    XSSType stype = app->context()->get_type(type_name);
        //    clazz_data->set_type(stype);
        //    app->register_singleton(clazz_data); //td: code
        //    continue;
        //  }
      }
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
    fixup_list::iterator it = octx.fixup.begin();
    fixup_list::iterator nd = octx.fixup.end();

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
                assert(false); //td:
                break;
              }
            case FIXUP_SUPER_TYPE: 
              {
                fixup_type fu = it->data;
                XSSType type = ctx->get_type(fu.type_name);
                if (check_type(type, fu.type_name, ctx))
                  fu.target->set_type(type);
                break;
              }
            default:
              assert(false); //what am I missing?
          }
      }
  }

void object_model::bind_it_up(XSSContext ctx, om_context& octx)
  {
    //first register types into the context, they'll be needed during binding
    type_map::iterator cit = octx.classes.begin();
    type_map::iterator cnd = octx.classes.end();

    for(; cit != cnd; cit++)
      {
        XSSType    type = cit->second;
        ctx->add_type(cit->first, type);
      }

    instance_map::iterator it = octx.instances.begin();
    instance_map::iterator nd = octx.instances.end();

    for(; it != nd; it++)
      {
        XSSObject  singleton = it->second;
        ctx->register_symbol(RESOLVE_INSTANCE, it->first, singleton);
      }

    //then bind
    cit = octx.classes.begin();
    cnd = octx.classes.end();

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

void object_model::add_error(const str& desc, param_list* info, file_location& loc)
  {
    Application app = app_by_file(loc.file); assert(app);
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

//error_list& object_model::errors()
//  {
//    return errors_;
//  }

void object_model::clear_file_errors(const str& fname)
  {
    Application app = app_by_file(fname); assert(app);
    app->clear_file_errors(fname);
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
