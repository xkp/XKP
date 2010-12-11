
#include "xss/project.h"
#include "xss/idiom.h"
#include "xss/dsl_out.h"
#include "xss/xss_error.h"

#include "xs/linker.h"
#include "xs/compiler.h"
#include "archive/json_archive.h"
#include "archive/xml_archive.h"

#include <dynamic_objects.h>

#include <iostream>
#include <fstream>

using namespace xkp;

const str SOutOfContext("not-sure");
const str SNotImplemented("not-implemented");

const str SInstancesMustProvideAClass("Instances must provide a class");
const str SWhatWasThisAgain("What was that again?");
const str SClassesMustHaveId("Classes must provide an id");
const str SDuplicateClass("Class already declared");

//visitors, fun stuff like that
struct class_internal_gather : dynamic_visitor
  {
    class_internal_gather(IEditableObject* editable, IDynamicObject* output):
      editable_(editable),
      output_(output)
      {
      }

    virtual void item(const str& name, variant value)
      {
        if (name == "properties")
          return; //bail

        //td: utilify
        int idx = output_->add_anonymous(value);
        schema_item si;
        si.flags = DYNAMIC_ACCESS;
        si.get   = Getter( new anonymous_getter(idx) );
        si.type  = value.get_schema();

        editable_->add_item(name, si);
      }

      private:
        IEditableObject* editable_;
        IDynamicObject*  output_;
  };


struct property_mixer : dynamic_visitor
  {
    property_mixer(DynamicObject prop, DynamicObject instance):
      prop_(prop),
      instance_(instance),
      result_(new xss_property())
      {
        //td: XSS props must have type
        result_->flags  = 0;
        result_->this_  = instance;
      }

    virtual void item(const str& name, variant value)
      {
        if (name == "name")
          result_->name = variant_cast<str>(value, "");
        else if (name == "default_value")
          result_->value_ = value;
        else if (name == "type")
          type_ = variant_cast<str>(value, "");
        else
          result_->add(name, value);
      }

    XSSProperty result()
      {
        assert(!result_->name.empty());
        variant instance_value = dynamic_get(instance_, result_->name); //td: do.query
        if (!instance_value.empty())
          {
            result_->value_ = instance_value;
          }
        else if (result_->value_.empty())
          {
            //td: get default value from type
          }

        return result_;
      }
    private:
      DynamicObject prop_;
      DynamicObject instance_;
      XSSProperty   result_;
      str           type_;
  };

//xss_code_context
xss_code_context::xss_code_context(const variant _project, xss_idiom* idiom) :
  base_code_context(),
  project_(_project),
  idiom_(idiom)
  {
  }

xss_code_context::xss_code_context(xss_code_context& other) :
  base_code_context(other),
  project_(other.project_),
  idiom_(other.idiom_)
  {
  }

XSSProperty xss_code_context::get_property(const str& name)
  {
    DynamicObject obj = variant_cast<DynamicObject>(this_, DynamicObject());
    if (obj)
      {
        return get_property(obj, name);
      }
    return XSSProperty();
  }

XSSProperty xss_code_context::get_property(DynamicObject obj, const str& name)
  {
    if (obj)
      {
        XSSProject project = project_; //I know, unnecesary cast, it aint that big a deal
        DynamicArray props = project->get_property_array(obj);
        for(size_t i = 0; i < props->size(); i++)
          {
            XSSProperty prop = props->at(i);
            if (prop->name == name)
              {
                return prop;
              }
          }
      }

    return XSSProperty();
  }

DynamicObject xss_code_context::resolve_instance(const str& id)
  {
    XSSProject project = project_;
    return project->get_instance(id); //td: consider this, scope, and stuff
  }

variant xss_code_context::evaluate_property(DynamicObject obj, const str& name)
  {
    XSSProperty prop = get_property(obj, name);
    if (prop)
      {
        return prop->get_value();
      }

    return variant();
  }

//xss_composite_context
xss_composite_context::xss_composite_context(XSSContext ctx):
  xss_code_context(ctx->project_, ctx->idiom_),
  ctx_(ctx)
  {
    types_    = ctx->types_;
    scope_    = ctx->scope_;
    args_     = ctx->args_;
    this_type = ctx->this_type;
    this_     = ctx->this_;
    dsl_      = ctx->dsl_;
  }

XSSProperty xss_composite_context::get_property(const str& name)
  {
    DynamicObject _this_ = variant_cast<DynamicObject>(this_, DynamicObject());
    return ctx_->get_property(_this_, name);
  }

XSSProperty xss_composite_context::get_property(DynamicObject obj, const str& name)
  {
    return ctx_->get_property(obj, name);
  }

DynamicObject xss_composite_context::resolve_instance(const str& id)
  {
    return ctx_->resolve_instance(id);
  }

variant xss_composite_context::evaluate_property(DynamicObject obj, const str& name)
  {
    return ctx_->evaluate_property(obj, name);
  }

//xss_event
xss_event::xss_event():
  impls(new dynamic_array)
  {
  }

xss_event::xss_event(const xss_event& other):
  name(other.name),
  impls(other.impls)
  {
  }

//xss_property
xss_property::xss_property(const xss_property& other):
  flags(other.flags),
  name(other.name),
  get(other.get),
  set(other.set),
  value_(other.value_),
  this_(other.this_)
  {
  }

xss_property::xss_property(const str& _name, variant _value, DynamicObject _this_):
  flags(0),
  name(_name),
  value_(_value),
  this_(_this_)
  {
  }

xss_property::xss_property(const str& _name, variant _value, variant _get, variant _set, DynamicObject _this_):
  flags(0),
  name(_name),
  get(_get),
  set(_set),
  value_(_value),
  this_(_this_)
  {
  }

str xss_property::generate_value()
  {
    if (value_.empty())
      return "null";

    param_list pl;
    variant    v;
    if (!dynamic_try_exec(value_, "generate", pl, v))
      {
        //if it an object?
        IDynamicObject* obj = variant_cast<IDynamicObject*>(value_, null);
        if (obj)
          {
            //td: check for xss writers
            //td: this wont work (will always return null) until the write archive understand mutables
            //json_write_archive json;
            //json.save(obj);
            //
            //return json.result();

            return "null";
          }
        else if (value_.is<str>())
          {
            // changed by Cuba
            //str ss = (str)value_;
            str ss = variant_cast<str>(value_, "");
            v = '"' + ss + '"';
          }
        else
          {
            //else it will just cast it to string, might fail
            v = value_;
          }
      }
    else
      {
      }

    str result;
    try
      {
        str vv = v;
        result = vv;
      }
    catch(type_mismatch)
      {
        return "Error: cannot resolve the value of " + name;
      }
    return result;
  }

variant xss_property::get_value()
  {
    return value_;
  }

//pre_process
struct pre_process : dynamic_visitor
  {
    pre_process(DynamicObject object, DynamicObject parent, xss_project& owner):
      object_(object),
      owner_(owner),
      parent_(parent)
      {
      }

    virtual void item(const str& name, variant value)
      {
        if (name == "parent")
          return; //td: generalize

        if (name == "source")
          {
            str file_name = value;
            owner_.add_application_file(file_name, object_);
          }
        else
          {
            DynamicObject visitable = variant_cast<DynamicObject>(value, DynamicObject());
            if (visitable)
              {
                variant v   = dynamic_get(visitable, "id");
                str     id  = variant_cast<str>(v, "");

                //debug
                str     debugid = variant_cast<str>(dynamic_get(object_, "id"), "");
                str     debugid_parent;
                if (parent_)
                  debugid_parent = variant_cast<str>(dynamic_get(parent_, "id"), "");

                if (dynamic_try_get(visitable, "class", v))
                  {
                    str class_name = v;
                    if (class_name == "property")
                      {
                        //shameful plug, yay me
                        XSSProperty prop(new xss_property(id, visitable, object_));

                        //grab the "properties" array
                        assert(parent_);
                        DynamicArray obj_properties = owner_.get_property_array(object_);
                        obj_properties->push_back(prop);
                        return;
                      }

                    dynamic_set(visitable, "class_name", class_name);
                  }

                //aside of that, we'll just interpret as an object
                owner_.register_instance(id, visitable, object_);
                pre_process pp(visitable, object_, owner_);
                visitable->visit(&pp);
              }
						else
							{
								//DynamicArray arr = variant_cast<DynamicArray>(value, DynamicArray());
								//td: check this case out
							}
          }
      }

    private:
      DynamicObject object_;
      DynamicObject parent_;
      xss_project&  owner_;
  };

struct source_code_gather :  xs_visitor
  {
    virtual void property_(xs_property& info)
      {
        properties.push_back(info);
      }

    virtual void method_(xs_method& info)
      {
        methods.push_back(info);
      }

    virtual void event_(xs_event& info)
      {
        events.push_back(info);
      }

    virtual void event_decl_(xs_event_decl& info)
      {
        assert(false); //td: implement
      }

    virtual void const_(xs_const& info)
      {
        assert(false); //td: implement
      }

    virtual void instance_(xs_instance& info)
      {
        assert(false); //td: implement
      }

    virtual void class_(xs_class& info)
      {
        assert(false); //td: implement
      }

    virtual void behaviour_(xs_behaviour& info)
      {
        assert(false); //td: implement
      }

    virtual void behaveas_(xs_implement_behaviour& info)
      {
        assert(false); //td: implement
      }

    virtual void dsl_(dsl& info)
      {
        assert(false); //td: implement
      }

    public:
        std::vector<xs_property> properties;
        std::vector<xs_method>   methods;
        std::vector<xs_event>    events;
  };

void xss_project::build()
  {
    //grab the path
    //td: change path to config... or something.
    str sp = variant_cast<str>(dynamic_get(path, "source_path"), "");
    source_path_ = sp;

    str op = variant_cast<str>(dynamic_get(path, "output_path"), "");
    output_path_ = op;

    //read the classes
    str class_library = variant_cast<str>(dynamic_get(path, "class_library"), "");
    read_classes(class_library);

    //grab the idiom
    idiom_ = idiom;

    //contextualize
    XSSProject me(shared_from_this());
    context_  = XSSContext(new xss_code_context(me, idiom_));
    current_  = XSSGenerator(new xss_generator(context_));

    //td: stupid references
    xss_code_context& code_ctx = *(context_.get());

		prepare_context(code_ctx, current_);
    code_ctx.scope_->register_symbol("project", me);

    idiom_->set_context(context_);

    //go at it
    preprocess();

    //the application object is manually handled... not sure why atm
    dynamic_set(application, "class_name", str("application"));
    register_instance("application", application);

    //compile the code aasociated with the app, note that any
    //node in the app can ave their separate file
    file_list::iterator it = app_files_.begin();
    file_list::iterator nd = app_files_.end();

    for(; it != nd; it++)
      {
        compile_instance(it->file, it->obj);
      }

    str generator_file = variant_cast<str>(dynamic_get(path, "generator"), "");
    str result = generate_file(generator_file);

    str of = variant_cast<str>(dynamic_get(path, "output_file"), "");
    save_file(output_path_ + of, result);
  }

str xss_project::output_path()
  {
    return output_path_;
  }

void xss_project::compile_instance(const str& filename, DynamicObject instance)
  {
    //parse the xs into top level constructs, like properties and stuff
    str source = load_file(source_path_ + filename);

    code_context  ctx;
    xs_compiler  compiler;
    xs_container ast;
    compiler.compile_xs(source, ast);

    source_code_gather gather;
    ast.visit(&gather);

    //create a safe array to pass around, this should be the standard practice
    //
    DynamicArray properties = get_property_array(instance);
    DynamicArray methods    = get_method_array(instance);

    std::vector<xs_property>::iterator pit = gather.properties.begin();
    std::vector<xs_property>::iterator pnd = gather.properties.end();
    for(; pit != pnd; pit++)
      {
        variant getter;
        variant setter;
        if (!pit->get.empty())
          getter = idiom_->process_code(pit->get, instance);

        if (!pit->set.empty())
          setter = idiom_->process_code(pit->set, instance);

        variant value;
        if (!pit->value.empty())
          value = idiom_->process_expression(pit->value, instance);

        XSSProperty new_prop(new xss_property(pit->name, value, getter, setter, instance));
        properties->push_back(new_prop);
      }

    std::vector<xs_method>::iterator mit = gather.methods.begin();
    std::vector<xs_method>::iterator mnd = gather.methods.end();
    for(; mit != mnd; mit++)
      {
        variant impl = idiom_->process_method(instance, *mit);
        methods->push_back(impl);
      }

    xs_event_list::iterator it = gather.events.begin();
    xs_event_list::iterator nd = gather.events.end();
    for(; it != nd; it++)
      {
        DynamicObject actual_instance = instance;
        str inst_name;
        for(size_t idx = 0; idx < it->name.size() - 1 && instance; idx++)
          {
            inst_name = it->name[idx];
            if (idx == 0)
              {
                actual_instance = get_instance(inst_name);
              }
            else
              {
                variant vv;
                if (dynamic_try_get(actual_instance, inst_name, vv))
                  {
                    actual_instance = variant_cast<DynamicObject>(vv, DynamicObject());
                  }
                else
                  assert(false);
              }
          }

        assert(actual_instance);
        str event_name = it->name[it->name.size() - 1];

        DynamicArray impls = get_event_impl(actual_instance, event_name);

        //let te idiom process implementations
        variant impl = idiom_->process_event(actual_instance, event_name, *it);
        impls->push_back(impl);
      }
  }

void xss_project::register_instance(const str& id, DynamicObject it, DynamicObject parent)
  {
    str _id = id;
    if (_id.empty())
      _id = "i" + boost::lexical_cast<str>(instances_.size());

    //td: debug code
    str rid = variant_cast<str>(dynamic_get(it, "id"), "");
    if (rid != _id)
      {
      }

    //update the instance, this is largely inefficient memory wise
    //but oh so easy to write.
    str class_name = variant_cast<str>(dynamic_get(it, "class_name"), "");
    class_registry::iterator cit = classes_.find(class_name);
    if (cit == classes_.end())
      assert(false);

    DynamicObject clazz      = cit->second;
    DynamicArray  properties = variant_cast<DynamicArray>(dynamic_get(clazz, "properties"), DynamicArray());
    DynamicArray  instance_properties = get_property_array(it);

    if (properties)
      {
        //Here it gets a little tricky, coherent, but tricky.
        //we need to convert the classes incoming properties into
        //XSSProperties, we should account for values that come in the instace
        //to be the proper values.
        for(size_t pp = 0; pp < properties->size(); pp++)
        {
          DynamicObject pobj = properties->at(pp);
          property_mixer mixer(pobj, it);
          pobj->visit(&mixer);

          XSSProperty ppprop = mixer.result(); assert(ppprop);
          instance_properties->push_back(ppprop);
        }
      }

    //we have only copied the high level properties, we still have to grab the internal
    //properties, the ones to be used by the generator.
    IEditableObject* editable = variant_cast<IEditableObject*>(it, null);
    IDynamicObject*  output   = it.get();
    class_internal_gather gather(editable, output);

    clazz->visit(&gather);

    //all set, lets register it
    dynamic_set(it, "id", _id);
    instances_.insert(instance_registry_pair(_id, instances.size()));
    instances.push_back(it);

    //and build a tree
    if (parent)
      {
        dynamic_set(it, "parent", parent);
        DynamicArray children = get_children_array(parent);
        children->push_back(it);
      }
  }

void xss_project::render_instance(DynamicObject instance, const str& xss)
  {
    str gen_text = load_file(source_path_ + xss);

    str id = variant_cast<str>(dynamic_get(instance, "id"), "");
    str class_name = variant_cast<str>(dynamic_get(instance, "class_name"), "");

    //setup the context
		XSSProject me(shared_from_this());
    XSSContext context(new xss_code_context(me, idiom_));
    xss_code_context& ctx = *context.get();
    XSSGenerator gen(new xss_generator(context));
    
		//od the deed
		push_generator(gen);

    prepare_context(ctx, gen);
		context->scope_->register_symbol("it", instance); //td: make sure its not there

		str result = generate_xss(gen_text, gen);

		pop_generator();
  }

str xss_project::resolve_dispatcher(DynamicObject instance, const str& event_name)
  {
    str           class_name = instance_class(instance);
    DynamicObject clazz      = find_class(class_name);

    if (clazz)
      {
        iterable dispatchers(dynamic_get(clazz, "dispatchers"));
        iterator it = dispatchers.begin();
        iterator nd = dispatchers.end();

        for(; it != nd; ++it)
          {
            DynamicObject dp = *it;
            str key = variant_cast<str>(dynamic_get(dp, "id"), "");
            if (key == event_name)
              return variant_cast<str>(dynamic_get(dp, "value"), "");
          }
      }

    return "";
  }

str xss_project::instance_class(DynamicObject instance)
  {
    variant result;
    if (dynamic_try_get(instance, "class", result))
      {
        str clazz = result;
        return clazz;
      }

    param_list error;
    error.add("id", SOutOfContext);
    error.add("desc", SInstancesMustProvideAClass);
    xss_throw(error);

    return ""; //never gets here
  }

str xss_project::inline_properties(DynamicObject instance)
  {
    param_list error;
    error.add("id", SNotImplemented);
    error.add("desc", SWhatWasThisAgain);
    xss_throw(error);
    return "";
  }

DynamicObject xss_project::find_class(const str& event_name)
  {
    param_list error;
    error.add("id", SNotImplemented);
    error.add("desc", SWhatWasThisAgain);
    xss_throw(error);
    return DynamicObject();
  }

void xss_project::breakpoint(const param_list params)
  {
    for(int i = (int)params.size() - 1; i >= 0; i--)
      {
        str param_name = params.get_name(i);
        variant value  = params.get(i);

        str           string_value = variant_cast<str>(value, "");
        DynamicObject obj_value    = variant_cast<DynamicObject>(value, DynamicObject());
      }
  }

DynamicArray xss_project::get_property_array(DynamicObject obj)
  {
    if (!obj)
      return DynamicArray();

    DynamicArray result;
    variant v = dynamic_get(obj, "properties");

    if (v.empty())
      {
        result = DynamicArray(new dynamic_array());
        dynamic_set(obj, "properties", result);
      }
    else
      {
        assert(v.is<DynamicArray>());
        result = v;
      }

    return result;
  }

DynamicArray xss_project::get_event_impl(DynamicObject obj, const str& event_name)
  {
    DynamicArray events = get_event_array(obj);
    for(size_t i = 0; i < events->size(); i++)
      {
        XSSEvent ev = events->at(i);
        if (ev->name == event_name)
          return ev->impls;
      }

    //not implemented, create
    XSSEvent ev(new xss_event);
    ev->name = event_name;

    events->push_back(ev);
    return ev->impls;
  }

DynamicArray  xss_project::get_method_array(DynamicObject obj)
  {
    DynamicArray result;
    variant v = dynamic_get(obj, "methods");

    if (v.empty())
      {
        result = DynamicArray(new dynamic_array());
        dynamic_set(obj, "methods", result);
      }
    else
      {
        assert(v.is<DynamicArray>());
        result = v;
      }

    return result;
  }

DynamicArray xss_project::get_event_array(DynamicObject obj)
  {
    DynamicArray result;
    variant v = dynamic_get(obj, "events");

    if (v.empty())
      {
        result = DynamicArray(new dynamic_array());
        dynamic_set(obj, "events", result);
      }
    else
      {
        assert(v.is<DynamicArray>());
        result = v;
      }

    return result;
  }

DynamicArray xss_project::get_children_array(DynamicObject obj)
  {
    DynamicArray result;
    variant v = dynamic_get(obj, "children");

    if (v.empty())
      {
        result = DynamicArray(new dynamic_array());
        dynamic_set(obj, "children", result);
      }
    else
      {
        assert(v.is<DynamicArray>());
        result = v;
      }

    return result;
  }

DynamicObject xss_project::get_instance(const str& id)
  {
      instance_registry::iterator it = instances_.find(id);
      if (it != instances_.end())
        return instances[it->second];

      return DynamicObject();
  }

void xss_project::add_application_file(const str& file, DynamicObject obj)
  {
    app_files_.push_back( file_info(file, obj) );
  }

str xss_project::load_file(const str& fname)
  {
    std::ifstream ifs(fname.c_str());

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

void xss_project::save_file(const str& fname, const str& contents)
  {
    std::ofstream ofs(fname.c_str());
    ofs << contents;
    ofs.close();
  }

void xss_project::prepare_context(base_code_context& context, XSSGenerator gen)
  {
    context.this_type = type_schema<xss_project>();
    context.this_     = this; //makes our methods directly accesible

    //fill up the context
    DslLinker ol(new out_linker(gen));
    context.dsls.insert(dsl_list_pair("out", ol));

    //add our identifiers
    context.scope.register_symbol("application", application);

    //setup the scope
    instance_registry::iterator it = instances_.begin();
    instance_registry::iterator nd = instances_.end();
    for(; it != nd; it++)
      {
        context.scope.register_symbol(it->first, it->second);
      }

    //and some types
    context.types_->add_type("array", &array_type_);
  }

str xss_project::generate_xss(const str& xss, XSSGenerator gen)
  {
    xss_parser parser;
    parser.register_tag("xss:code");
    parser.register_tag("xss:e");
    parser.register_tag("xss:class");
    parser.register_tag("xss:file");

		parser.parse(xss, gen.get());
    return gen->get();
  }

void xss_project::output_file(const str& fname, const str& contents)
	{
    save_file(output_path_ + fname, contents);
	}

str xss_project::generate_file(const str& fname, XSSContext context)
  {
    str gen_text = load_file(source_path_ + fname);

		if (!context)
				context = context_;

    //setup the context & generator
    XSSContext safe_context(new xss_composite_context(context));
    xss_code_context& ctx = *safe_context.get();
    XSSGenerator gen(new xss_generator(safe_context));
    
		//od the deed
		push_generator(gen);

    prepare_context(ctx, gen);
    str result = generate_xss(gen_text, gen);

		pop_generator();
    return result;
  }

void xss_project::preprocess()
  {
    DynamicObject app_object = application;
    pre_process pp(app_object, DynamicObject(), *this);
    app_object->visit(&pp);
  }

void xss_project::read_classes(const str& class_library_file)
  {
    type_registry types;
    types.set_default_type(type_schema<sponge_object>());

    xml_read_archive class_library_achive(load_file(source_path_ + class_library_file), &types);
    classes = class_library_achive.get( type_schema<DynamicObjectList>() );

    DynamicObjectList::iterator it = classes.begin();
    DynamicObjectList::iterator nd = classes.end();

    for(; it != nd; it++)
      {
        DynamicObject obj = *it;
        str           id  = variant_cast<str>(dynamic_get(obj, "id"), "");

        if (id.empty())
          {
            param_list error;
            error.add("id", SOutOfContext);
            error.add("desc", SClassesMustHaveId);
            xss_throw(error);
          }

        class_registry::iterator it = classes_.find(id);
        if (it == classes_.end())
          {
            classes_.insert(class_registry_pair(id, obj));
          }
        else
          {
            param_list error;
            error.add("id", SOutOfContext);
            error.add("desc", SDuplicateClass);
            error.add("class", id);
            xss_throw(error);
          }
      }
  }

void xss_project::push_generator(XSSGenerator gen)
	{
		current_ = gen;
		generators_.push(gen); 	
	}

void xss_project::pop_generator()
	{
		assert(!generators_.empty());
		current_ = generators_.top();
		generators_.pop();
	}
