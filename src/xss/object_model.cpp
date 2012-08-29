
#include <xss/object_model.h>
#include <xss/xml_data_reader.h>
#include <xss/xss_context.h>
#include <xss/xss_error.h>
#include <xss/xss_expression.h>

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


//application
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

//object_model
Application object_model::load(DataReader project, param_list& args)
  {
    Application result(new application);

    entity_list root = project->root();
    validate_root(root);

    DataEntity prj = root[0];

    //create the global context for this application
    Language   lang   = read_language(prj);
    XSSContext global = lang->create_context();

    //initialize the execution context on which the xss files will run, 
    //note this is different from the data context
    code_context& code_ctx = result->exec_context();

    //in that context there will be a "project" object containing stuff of usability
    XSSObject app_project(new xss_object);

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
            param_list error;
            error.add("id", SProjectError);
            error.add("desc", SNamelessProjectParameter);
            xss_throw(error);
          }

        app_project->add_attribute(pid, read_value(param));
      }

    code_ctx.scope_->register_symbol("project",     app_project);
    code_ctx.scope_->register_symbol("application", result);

    //fillup our application
    fs::path ep = fs_->locate(prj->attr("entry_point"));
    if (ep.empty())
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SMustProvideEntryPointForApplicationType);
        xss_throw(error);
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
        DataEntity _idiom = *it;
        Idiom      idiom = find_idiom(_idiom->id());
        
        if (!idiom)
          idiom = read_idiom(_idiom, global);

        assert(idiom);
        idiom->bind(global);
      }

    //load includes
    entity_list includes = prj->find_all("include");
    entity_list::iterator iit = includes.begin();
    entity_list::iterator ind = includes.end();

    for(; iit != ind; iit++)
      {
        DataEntity _include = *iit;
        str        def = _include->attr("def");  
        str        src = _include->attr("src");  

        Idiom idiom = compile_include(def, src);
        if (idiom)
          idiom->bind(global);
      }

    return result;
  }

Language object_model::read_language(DataEntity project)
  {
    str language_name = project->attr("lang");
    if (language_name.empty())
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SMustProvideLanguageForApplicationType);
        xss_throw(error);
      }

    return languages_->create(language_name);
  }

void object_model::validate_root(entity_list& root)
  {
    if (root.size() != 1)
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SApplicationsMustHaveOnlyOneRoot);
        xss_throw(error);
      }
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
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SIdiomsMustHaveId);
        xss_throw(error);
      }

    str _namespace = de->attr("namespace");
    if (_namespace.empty())
      _namespace = iid;

    result->set_namespace(_namespace);
    
    register_idiom(iid, result);

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
            error.add("id", SProjectError);
            error.add("desc", SImportCannotInherit);
            error.add("import", ed->id());
            xss_throw(error);
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

Idiom object_model::compile_include(const str& def, const str& src)
  {
    assert(false);
    return Idiom();
  }

void object_model::assure_id(DataEntity de)
  {
    str id = de->id();
    if (id.empty())
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SNeedsId);
        error.add("type", de->type());
        xss_throw(error);
      }
  }

XSSType object_model::read_type(DataEntity de, Idiom parent, XSSContext ctx)
  {
    XSSType result(new xss_type());
    result->set_id(de->id());
    
    str super = de->attr("super");
    if (!super.empty())
      {
        XSSType super_type = ctx->get_type(super, parent->get_namespace());
        assert(super_type); //td: fixup

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
            error.add("id", SProjectError);
            error.add("desc", SUnkownImport);
            error.add("import", id->id());
            xss_throw(error);
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

        XSSProperty prop = read_property(pd, ctx);

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

        XSSMethod mthd = read_method(md, ctx);
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

        XSSEvent ev = read_event(ed, ctx);
        result->insert_event(ev);
      }

    return result;
  }

XSSType object_model::read_enum(DataEntity de, XSSContext ctx)
  {
    XSSType int_type = ctx->get_type("int");

    XSSType result(new xss_type);
    result->set_id(de->id());
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
            error.add("id", SProjectError);
            error.add("desc", SUnkownType);
            error.add("import", stype);
            xss_throw(error);
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

    str stype = de->attr("type");
    XSSType type = ctx->get_type(stype, idiom->get_namespace());
    if (!type)
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SUnkownType);
        error.add("type", stype);
        error.add("property", de->id());
        xss_throw(error);
      }

    result->set_type(type);
    
    XSSExpression value = read_expression(de, type, "value");
    result->expr_value(value);

    //getter/setter
    DataEntity ge = de->get("get");
    if (ge)
      {
        InlineRenderer getter = read_inline_renderer(ge);
        result->set_getter(getter);
      }

    DataEntity se = de->get("set");
    if (se)
      {
        InlineRenderer setter = read_inline_renderer(se);
        result->set_setter(setter);
      }

    return result;
  }

XSSMethod object_model::read_method(DataEntity de, Idiom idiom, XSSContext ctx)
  {
    XSSProperty result(new xss_property());
    result->set_id(de->id());

    str stype = de->attr("return_type");
    if (!stype.empty())
      {
        XSSType type = ctx->get_type(stype, idiom->get_namespace());
        if (!type)
          {
            param_list error;
            error.add("id", SProjectError);
            error.add("desc", SUnkownType);
            error.add("type", stype);
            error.add("method", de->id());
            xss_throw(error);
          }
        
        result->return_type(type);
      }

    DataEntity ce = de->get("call");
    if (ce)
      {
        InlineRenderer caller = read_inline_renderer(ce);
        result->set_caller(caller);
      }

    XSSSignature sig = read_signature(de, idiom, ctx);
    result->set_signature(sig);

    return result;
  }

XSSEvent object_model::read_event(DataEntity de, Idiom idiom, XSSContext ctx)
  {
    XSSEvent result(new xss_event());
    result->set_id(de->id());

    DataEntity de = de->get("dispatch");
    if (de)
      {
        InlineRenderer dispatcher = read_inline_renderer(de);
        result->set_dispatcher(dispatcher);
      }

    XSSSignature sig = read_signature(de, idiom, ctx);
    result->set_signature(sig);

    return result;
  }

void object_model::register_idiom(const str& id, Idiom idiom)
  {
    idiom_list::iterator it = idioms_.find(id);
    if (it != idioms_.end())
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SDupIdiom);
        error.add("import", id);
        xss_throw(error);
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