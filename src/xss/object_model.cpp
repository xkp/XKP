
#include <xss/object_model.h>
#include <xss/xml_data_reader.h>
#include <xss/xss_context.h>
#include <xss/xss_error.h>
#include <xss/xss_expression.h>
#include <xss/xss_code.h>

#include <xs/compiler.h>

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

//document
void document::add(XSSContext context)
  {
    assert(context->begin().line >= 0 && context->end().line >= 0);
    items_.push_back(snap_shot(context->begin(), context->end(), context));
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

//object_model
Application object_model::load(DataReader project, param_list& args)
  {
    Application result(new application);
    DataEntity  prj = assure_unique_root(project);

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

        compile_include(def, src, global);
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

void object_model::compile_include(const str& def_file, const str& src_file, XSSContext ctx)
  {
    type_map     classes;
    instance_map instances;

    fs::path path;
    if (!def_file.empty())
      {
        DataReader def      = fs_->load_data(def_file);
        DataEntity def_root = assure_unique_root(def);

        classes   = read_include_def(def_root, ctx);
        instances = read_include_singleton(def_root, ctx);
      }

    if (!src_file.empty())
      {
        str       code = fs_->load_file(src_file);
        document& doc  = create_document(src_file);
        compile_xs(code, ctx, classes, instances, doc);

        assert(false); //td:
      //  //we start by processing the classes that have some code
      //  std::vector<xs_class>::iterator sit = source_classes.begin();
      //  std::vector<xs_class>::iterator snd = source_classes.end();
      //  for(; sit != snd; sit++)
      //    {
				  //  xs_class& ci  = *sit;

      //      //lookup the classes
      //      std::map<str, int>::iterator dcit = def_types.find(ci.name); assert(dcit != def_types.end());
      //      XSSObject def_class = classdefs[dcit->second];
						//XSSType   clazz     = classes[dcit->second];

      //      //mark it as handled
      //      def_types.erase(dcit);

						//XSSType super;
      //      XSSContext ictx(new xss_context(ctx, path.parent_path()));

				  //  if (!ci.super.empty())
					 //   {
      //          str def_super = def_class? def_class->get<str>("super", str()) : str();
      //          if (!def_super.empty() && def_super != ci.super)
						//	    {
						//		    param_list error;
						//		    error.add("id", SProjectError);
						//		    error.add("desc", SBadInheritance);
						//		    error.add("class", ci.super);
						//		    xss_throw(error);
						//	    }

						//    super = ctx->get_type(ci.super);
						//    if (!super)
						//	    {
						//		    param_list error;
						//		    error.add("id", SProjectError);
						//		    error.add("desc", SUnknownClass);
						//		    error.add("class", ci.super);
						//		    xss_throw(error);
						//	    }
					 //   }

      //      clazz->set_definition(def_class);
      //      clazz->set_super(super);
      //      clazz->set_context(ictx);
      //      clazz->fixup_children(ictx);
				  //  
      //      ictx->set_this(XSSObject(clazz));

      //      preprocess_type(clazz, def_class, ci.super, ictx, app);
      //      XSSModule module = app->type_idiom(ci.super);
      //      if (module)
      //        module->pre_process_type(clazz);

				  //  //then compile the code
      //      compile_ast(ci, ictx);
          //}
			}

    ////process the left over classes
    //std::map<str, int>::iterator it = def_types.begin();
    //std::map<str, int>::iterator nd = def_types.end();
    //for(; it != nd; it++)
    //  {
    //    XSSObject def_class = classdefs[it->second];
				//XSSType   clazz     = classes[it->second];

    //    clazz->set_definition(def_class);

    //    str super = def_class->get<str>("super", str());
    //    if (!super.empty())
    //      {
    //        XSSType super_clazz = ctx->get_type(super);
    //        if (!super_clazz)
    //          {
				//				param_list error;
				//				error.add("id", SProjectError);
				//				error.add("desc", SUnknownClass);
				//				error.add("class", super);
				//				xss_throw(error);
    //          }

    //        clazz->set_super(super_clazz);
    //      }

    //    XSSContext ictx(new xss_context(ctx, path.parent_path()));
    //    preprocess_type(clazz, def_class, super, ictx, app);

    //    XSSModule module = app->type_idiom(super);
    //    if (module)
    //      module->pre_process_type(clazz);

    //    clazz->set_context(ictx);
    //  }

    //std::vector<XSSType>::iterator cit = classes.begin();
    //std::vector<XSSType>::iterator cnd = classes.end();

    //for(; cit != cnd; cit++)
    //  {
    //    XSSType ct = *cit;
    //    ct->propertize();
    //  }
  }

enum FIXUP_TYPE
  {
    FIXUP_OBJECT_TYPE,
    FIXUP_INSTANCE_EVENT,
    FIXUP_SUPER_TYPE, 
  };

struct fixup_data
  {
    fixup_data(FIXUP_TYPE _id, variant _data):
      id(_id),
      data(_data)
      {
      }      

    FIXUP_TYPE id;
    variant    data;
  };

typedef std::vector<fixup_data> fixup_list;

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
					  error.add("id", SProjectError);
					  error.add("desc", SDupProperty);
					  error.add("object", target_->id());
					  error.add("property", info.name);
					  xss_throw(error);
          }

        if (!result)
          {
            result = XSSProperty(new xss_property);
            result->set_id(info.name);
            target_->insert_property(result);
          }
        
        fixup_.push_back(fixup_data(FIXUP_OBJECT_TYPE, fixup_type(info.type, XSSObject(result))));
        
        if (!info.value.empty())
          {
            XSSExpression value = xss_expression_utils::compile_expression(info.value);
            result->expr_value(value);
          }

        if (!info.get.empty())
          {
            XSSCode getter = xss_code_utils::compile_code(info.get, null); //td: callback
            result->code_getter(getter);
          }

        if (!info.set.empty())
          {
            XSSCode setter = xss_code_utils::compile_code(info.set, null); //td: callback
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

        fixup_.push_back(fixup_data(FIXUP_OBJECT_TYPE, fixup_type(info.type, XSSObject(result))));

        XSSSignature sig  = xss_code_utils::compile_arg_list(info.args);
        result->set_signature(sig);

        XSSCode code = xss_code_utils::compile_code(info.cde, null); //td: callback
        result->set_code(code);
      }

    virtual void event_(xs_event& info)
      {
        XSSSignature sig  = xss_code_utils::compile_arg_list(info.args);
        XSSCode      code = xss_code_utils::compile_code(info.cde, null); //td: callback

        if (info.name.size() > 1)
          {
            fixup_.push_back(fixup_data(FIXUP_INSTANCE_EVENT, fix_instance_event(info.name, sig, code)));
            return;
          }

        XSSEvent ev = target_->get_event(info.name[0]);
        if (!ev)
          {
					  param_list error;
					  error.add("id", SProjectError);
					  error.add("desc", SUndeclaredEvent);
					  error.add("object", target_->id());
					  error.add("method", info.name);
					  xss_throw(error);
          }

        if (!ev->signature()->match_signature(sig))
          {
					  param_list error;
					  error.add("id", SProjectError);
					  error.add("desc", SBadEventSignature);
					  error.add("object", target_->id());
					  error.add("event", info.name[0]);
					  xss_throw(error);
          }

        target_->add_event_impl(ev, code);
      }

    virtual void event_decl_(xs_event_decl& info)
      {
        XSSEvent result = target_->get_event(info.name);
        if (result)
          {
					  param_list error;
					  error.add("id", SProjectError);
					  error.add("desc", SDupEvent);
					  error.add("object", target_->id());
					  error.add("event", info.name);
					  xss_throw(error);
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
					  error.add("id", SProjectError);
					  error.add("desc", SDupProperty);
					  error.add("object", target_->id());
					  error.add("const", info.name);
					  xss_throw(error);
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
    include_visitor(XSSContext ctx, type_map& classes, instance_map& instances, document& doc, fixup_list& fixup, IContextCallback* callback):
      ctx_(ctx),
      classes_(classes),
      instances_(instances),
      doc_(doc),
      fixup_(fixup),
      callback_(callback)
      {
      }

    virtual void instance_(xs_instance& info)
      {
        XSSObject instance;

        assert(info.id.size() == 1); //td: !!!qualified
        instance_map::iterator it = instances_.find(info.id[0]);
        if (it != instances_.end())
          instance = it->second;
        else
          {
            str id = info.id[0];
            
            instance = XSSObject(new xss_object);
            instance->set_id(id);
            instances_.insert(std::pair<str, XSSObject>(id, instance));
          }

        object_visitor ov(instance, ctx_, fixup_, callback_);
        info.visit(&ov);
      }

    virtual void class_(xs_class& info)
      {
        XSSType type;

        type_map::iterator it = classes_.find(info.name);
        if (it != classes_.end())
          type = it->second;
        else
          {
            type = XSSType(new xss_type);
            type->set_id(info.name);
            classes_.insert(std::pair<str, XSSType>(info.name, type));
          }

        if (!info.super.empty())
          {
            fixup_.push_back(fixup_data(FIXUP_SUPER_TYPE, fixup_super(info.super, type)));
          }
        
        object_visitor ov(XSSObject(type), ctx_, fixup_, callback_);
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

    virtual void property_(xs_property& info)             {error();}
    virtual void method_(xs_method& info)                 {error();}
    virtual void event_(xs_event& info)                   {error();}
    virtual void event_decl_(xs_event_decl& info)         {error();}
    virtual void const_(xs_const& info)                   {error();}
    virtual void behaveas_(xs_implement_behaviour& info)  {error();}

    private:
      XSSContext        ctx_;
      type_map&         classes_;
      instance_map&     instances_;
      document&         doc_;
      fixup_list&       fixup_;
      IContextCallback* callback_;

      void error()
        {
					param_list error;
					error.add("id", SProjectError);
					error.add("desc", SUnsoportedInclude);
					xss_throw(error);
        }
  };

struct document_builder : IContextCallback
  {
    document_builder(document& doc):
      doc_(doc)
      {
      }

    virtual void notify(XSSContext context)
      {
        doc_.add(context);
      }

    private:
      document& doc_;
  };

void object_model::compile_xs(const str& text, XSSContext ctx, type_map& classes, instance_map& instances, document& doc)
  {
    xs_container results;
    std::vector<str> dsls;
    ctx->collect_xss_dsls(dsls);

    xs_compiler compiler(dsls);
    compiler.compile_xs(text, results); //td: errors

    fixup_list       fu;
    document_builder callback(doc);

    include_visitor iv(ctx, classes, instances, doc, fu, &callback); 
    results.visit(&iv);

    assert(false); //td: fixup and registration

    ////first we ought to register the type before processing
    //std::vector<xs_class> source_classes;
    //for(size_t i = 0; i < results.size(); i++)
    //  {
				//variant vv = results.get(i);
				//if (!vv.is<xs_class>())
				//	{
				//		//what kinda class are you?
				//		param_list error;
				//		error.add("id", SProjectError);
				//		error.add("desc", SOnlyClassAllowedInInclude);
				//		xss_throw(error);
				//	}

    //    source_classes.push_back(vv);

				//xs_class& ci  = source_classes.back();
    //    str       cid = ci.name;

    //    //look for a declaration on the xml file
    //    XSSObject def_class;
    //    std::map<str, int>::iterator dtit = def_types.find(cid);
    //    if (dtit == def_types.end())
    //      {
				//    XSSType clazz(new xss_type());
    //        clazz->set_id(cid);

    //        def_types.insert(std::pair<str, int>(cid, classdefs.size()));
				//		classdefs.push_back(XSSObject());
				//		classes.push_back(clazz);

    //        //register early
    //        XSSModule module = app->type_idiom(ci.super);
    //        if (module)
    //          module->register_user_type(clazz);
    //        else
    //          app_types_.push_back(clazz);

    //        //mark as defined user type
    //        clazz->add_attribute("user_defined", true);

    //        ctx->add_type(cid, clazz);
    //      }
    //  }
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

DataEntity object_model::assure_unique_root(DataReader dr)
  {
    if (dr->root().size() != 1)
      {
        param_list error;
        error.add("id", SProjectError);
        error.add("desc", SApplicationsMustHaveOnlyOneRoot);
        xss_throw(error);
      }
    return dr->root()[0];
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
    XSSMethod result(new xss_method());
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
    XSSEvent result(new xss_event);
    result->set_id(de->id());

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

type_map object_model::read_include_def(DataEntity de, XSSContext ctx)
  {
    type_map result;

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

    return result;
  }

instance_map object_model::read_include_singleton(DataEntity de, XSSContext ctx)
  {
    instance_map result;

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

    return result;
  }

document& object_model::create_document(const str& src_file)
  {
    fs::path src = fs_->locate(src_file);
    document_map::iterator it = documents_.find(src);
    if (it != documents_.end())
      return it->second;

    documents_.insert(std::pair<fs::path, document>(src, document()));
    it = documents_.find(src);
    return it->second;
  }
