
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
#include <boost/algorithm/string.hpp>

using namespace xkp;

const str SOutOfContext("not-sure");
const str SNotImplemented("not-implemented");
const str SCannotResolve("unkown-identifier");
const str SFileError("404");
const str STypeMismatch("type-mismatch");
const str SCompile("syntax-error");

const str SInstancesMustProvideAClass("Instances must provide a class");
const str SWhatWasThisAgain("What was that again?");
const str SClassesMustHaveId("Classes must provide an id");
const str SDuplicateClass("Class already declared");
const str SNoSoup("Super class not found");
const str SUnknownClass("Class not found");
const str SUnknownInstance("Instance not found");
const str SFileNotFound("File not found");
const str SOutputtingNonString("Trying to output a value that does not translate to a string");
const str SInvalidExpression("Cannot compile expression");
const str SIncompatibleReturnType("Return type differs from declared type");

struct property_mixer : dynamic_visitor
  {
    property_mixer(XSSObject source, XSSObject dest):
      src_(source),
      dst_(dest)
      {
      }

    virtual void item(const str& name, variant value)
      {
				if (!dst_->has(name))
					{
						dst_->add_property(name, value);
					}
      }

    private:
      XSSObject	src_;
      XSSObject	dst_;
  };

//xss_object
xss_object::xss_object():
	sealed_(false),
	children_(new dynamic_array),
	properties_(new dynamic_array),
	methods_(new dynamic_array),
	events_(new dynamic_array)
	{
		add_property("children", children_);
		//add_property("properties", properties_);
		add_property("methods", methods_);
		add_property("events", events_);
	}

XSSObject xss_object::type()
	{
		return type_;
	}

void xss_object::xss_type(XSSObject type)
	{
		type_ = type;	
		
		XSSObject me(shared_from_this());
		property_mixer pm(type, me);
		type_->visit(&pm);

		DynamicArray tprops = type->properties();
		for(int i = 0; i < tprops->size(); i++)
			{
				XSSProperty prop		= tprops->at(i);
				variant			value		= dynamic_get(type, prop->name);
				XSSProperty myprop	= get_property(prop->name);
				variant			myvalue	= dynamic_get(this, prop->name);

				if (!myprop)
					{
						XSSProperty cln(new xss_property(*(prop.get())));
						cln->copy(prop.get()); //td: proper cloning
						cln->value_ = !myvalue.empty()? myvalue : value;

						properties_->insert(cln);
					}
				else
					{
						assert(false); //td: mix properties
					}
			}

		DynamicArray tevents = type->events();
		for(int i = 0; i < tevents->size(); i++)
			{
				XSSEvent ev   = tevents->at(i);
				XSSEvent myev = get_event(ev->name);

				if (!myev)
					{
						XSSEvent cln(new xss_event(*(ev.get())));
						cln->copy(ev.get()); //td: proper cloning

						events_->insert(cln);
					}
				else
					{
						myev->copy(ev.get());
					}
			}
	}

XSSObject xss_object::parent()
	{
		return parent_;
	}

DynamicArray xss_object::children()
	{
		return children_;
	}

DynamicArray xss_object::properties()
	{
		return properties_;
	}

DynamicArray xss_object::methods()
	{
		return methods_;
	}

DynamicArray xss_object::events()
	{
		return events_;
	}

void xss_object::copy(xss_object* other)
	{
		item_list::iterator it = other->items_.begin();
		item_list::iterator nd = other->items_.end();
		for(; it != nd; it++)
			{
				if (has(it->first))
					continue; //td: decide
				
				//make sure we only copy dynamic stuff
				anonymous_getter* getter = dynamic_cast<anonymous_getter*>(it->second.get.get());
				if (getter)
					{
						variant value = other->get_anonymous(getter->idx_);	
						add_property(it->first, value);
					}
			}
	}

XSSProperty xss_object::get_property(const str& name)
	{
		for(int i = 0; i < properties_->size(); i++)
			{
				XSSProperty prop = properties_->at(i);
				if (prop->name == name)
					return prop;
			}

		return XSSProperty();
	}

XSSEvent xss_object::get_event(const str& name)
	{
		for(int i = 0; i < events_->size(); i++)
			{
				XSSEvent ev = events_->at(i);
				if (ev->name == name)
					return ev;
			}

		return XSSEvent();
	}


struct limbo_setter : setter
  {
		limbo_setter(XSSObject obj, const str& id): 
			obj_(obj), 
			id_(id)
			{
			}

    virtual void set( void* instance, const variant value )
			{
				obj_->limbo_add(id_, value);
			}

    XSSObject obj_;
		str				id_;
  };

struct limbo_getter : getter
  {
		limbo_getter(XSSObject obj, const str& id): 
			obj_(obj), 
			id_(id)
			{
			}

		virtual variant get(void* instance)
			{
				return variant();
			}

    XSSObject obj_;
		str				id_;
  };

void xss_object::limbo_add(const str& id, variant value)
	{
		assert(!sealed_);

		XSSObject obj = variant_cast<XSSObject>(value, XSSObject());
		if (obj)
			{
				children_->insert(obj);
			}
		else
			{
				//reg prop
				add_property(id, value);
			}
	}

void xss_object::seal()
	{
		sealed_ = true;
	}
	
bool xss_object::resolve(const str& name, schema_item& result)
	{
		if (editable_object<xss_object>::resolve(name, result))
			return true;

		if (!sealed_)
			{
				XSSObject me(shared_from_this());

				//we are acquiring children (quite likely in the serialization process)
				result.get   = Getter( new limbo_getter(me, name) );
				result.set   = Setter( new limbo_setter(me, name) );
				result.flags = DYNAMIC_ACCESS;
			}
		else
			{
				//check for class properties to cache, this is the easy way
				variant value;
				if (type_)
					value = dynamic_get(type_, name);

				//otherwise behave just like the old sponge
				int idx      = add_anonymous(value);
				result.get   = Getter( new anonymous_getter(idx) );
				result.set   = Setter( new anonymous_setter(idx) );
				result.flags = DYNAMIC_ACCESS;

				items_.insert( item_pair(name, result) );
			}

		return true;
	}

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
    XSSObject obj = variant_cast<XSSObject>(this_, XSSObject());
    if (obj)
      {
        return get_property(obj, name);
      }
    return XSSProperty();
  }

XSSProperty xss_code_context::get_property(XSSObject obj, const str& name)
  {
    if (obj)
			return obj->get_property(name);

    return XSSProperty();
  }

XSSObject xss_code_context::resolve_instance(const str& id)
  {
    XSSProject project = project_;
    return project->get_instance(id); //td: consider this, scope, and stuff
  }

variant xss_code_context::evaluate_property(XSSObject obj, const str& name)
  {
    XSSProperty prop = get_property(obj, name);
    if (prop)
      {
        return prop->get_value();
      }

    return variant();
  }

schema* xss_code_context::get_type(const str& name)
	{
		if (!types_)
			return null; 

		return types_->get_type(name);
	}

str	xss_code_context::get_type_name(schema* type)
	{
		if (!types_)
			return ""; 

		return types_->type_name(type);
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
    XSSObject _this_ = variant_cast<XSSObject>(this_, XSSObject());
    return ctx_->get_property(_this_, name);
  }

XSSProperty xss_composite_context::get_property(XSSObject obj, const str& name)
  {
    return ctx_->get_property(obj, name);
  }

XSSObject xss_composite_context::resolve_instance(const str& id)
  {
    return ctx_->resolve_instance(id);
  }

variant xss_composite_context::evaluate_property(XSSObject obj, const str& name)
  {
    return ctx_->evaluate_property(obj, name);
  }

//xss_event
xss_event::xss_event():
  impls(new dynamic_array)
  {
		DYNAMIC_INHERITANCE(xss_event)
  }

xss_event::xss_event(const xss_event& other):
  name(other.name),
  impls(new dynamic_array)
  {
		DYNAMIC_INHERITANCE(xss_event)

		for(int i = 0; i < other.impls->size(); i++)
			{
				impls->insert(other.impls->at(i));
			}
	}

//xss_property
xss_property::xss_property(): flags(0) 
	{
		//el lamo... this is the old inheritance without template problem
		//I dont care to fix it as of now, keep your inheritance shallow
		DYNAMIC_INHERITANCE(xss_property)
	}

xss_property::xss_property(const xss_property& other):
  flags(other.flags),
  name(other.name),
  get(other.get),
  set(other.set),
  value_(other.value_),
  this_(other.this_)
  {
		DYNAMIC_INHERITANCE(xss_property)
  }

xss_property::xss_property(const str& _name, const str& _type, variant _value, XSSObject _this_):
  flags(0),
  name(_name),
  value_(_value),
  this_(_this_),
	type(_type)
  {
		DYNAMIC_INHERITANCE(xss_property)
  }

xss_property::xss_property(const str& _name, const str& _type, variant _value, variant _get, variant _set, XSSObject _this_):
  flags(0),
  name(_name),
  get(_get),
  set(_set),
  value_(_value),
  this_(_this_),
	type(_type)
  {
		DYNAMIC_INHERITANCE(xss_property)
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

str	xss_property::resolve_assign(const str& value)
	{
    str set_fn = variant_cast<str>(dynamic_get(this, "set_fn"), ""); //let the outside world determine
                                                                      //if a native function call shouls be made 

    str set_xss = variant_cast<str>(dynamic_get(this, "set_xss"), ""); //such world can request to parse xss

		if (!set_xss.empty())
			{
				assert(false); //td:
			}
		else if (!set_fn.empty())
      {
				return set_fn + '(' + value + ')';
      }
    else if (!set.empty())
			{
				return name + "_set(" + value + ')';
			}

		return name + " = " + value;
  }

//pre_process
struct pre_process : dynamic_visitor
  {
    pre_process(XSSObject object, XSSObject parent, xss_project& owner, bool do_register):
      object_(object),
      owner_(owner),
      parent_(parent),
			do_register_(do_register)
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
        else if (name == "children" || name == "application")
					{
						DynamicArray children = value;
						for(int i = 0; i < children->size(); i++)
							{
								XSSObject child = children->at(i);
								child->seal();

                variant v   = dynamic_get(child, "id");
                str     id  = variant_cast<str>(v, "");
								str			class_name;

                if (dynamic_try_get(child, "class", v))
                  {
                    class_name = str(v);
										
										instantiate(child, id, class_name);

										//go down the hierarchy
										bool is_name_space = variant_cast<bool>(dynamic_get(child, "namespace"), false);
										pre_process pp(child, object_, owner_, do_register_ && !is_name_space);
										child->visit(&pp);
                  }
							}
					}
			}

		void instantiate(XSSObject obj, str& id, const str& class_name)
			{
        if (class_name == "property")
          {
            //shameful plug, yay me
            str type = variant_cast<str>(dynamic_get(obj, "type"), "");
						XSSProperty prop(new xss_property(id, type, obj, object_));

            //grab the "properties" array
            assert(parent_);
            DynamicArray obj_properties = owner_.get_property_array(object_);
            obj_properties->push_back(prop);
            return;
          }

        dynamic_set(obj, "class_name", class_name);
										
				//hook up the type
				XSSObject clazz = owner_.get_class(class_name);
				if (!clazz && do_register_)
					{
						param_list error;
						error.add("id", SCannotResolve);
						error.add("desc", SUnknownClass);
						error.add("class", class_name);
						xss_throw(error);
					}

				if (clazz)
					{
						obj->xss_type(clazz);

						//unnamed object will appear with an id identical to its class
						if (id == class_name && class_name != "application")
							{
								id = owner_.get_anonymous_id(class_name);
								dynamic_set(obj, "id", id);
							}

						//register it into the project
						if (do_register_)
							owner_.register_instance(id, obj);
					}
			}

    private:
      XSSObject			object_;
      XSSObject			parent_;
      xss_project&  owner_;
			bool					do_register_;
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
				instances.push_back(info);
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
        std::vector<xs_instance> instances;
  };

//out
out::out()
	{
	}

out::out(XSSProject prj): prj_(prj)
	{
	}

void out::append(variant v)
	{
		str value;
		try
			{
				value = str(v);
			}
		catch(type_mismatch)
			{
				param_list error;
				error.add("id", STypeMismatch);
				error.add("desc", SOutputtingNonString);
				xss_throw(error);
			}

		prj_->generator()->append(value);
	}

str out::line_break()
	{
		return "\n";
	}

//xss_project
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
    dynamic_set(application, "x", 0);
    dynamic_set(application, "y", 0);
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

		fs::path fname = base_path_ / output_path_ / of; 
		save_file(fname.string(), result);
  }

str xss_project::output_path()
  {
    return output_path_.string();
  }

void xss_project::base_path(fs::path path)
	{
		base_path_ = path;
	}

void xss_project::compile_ast(xs_container& ast, XSSObject instance)
	{
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

				XSSProperty new_prop(new xss_property(pit->name, pit->type, value, getter, setter, instance));
        properties->push_back(new_prop);
      }

    std::vector<xs_method>::iterator mit = gather.methods.begin();
    std::vector<xs_method>::iterator mnd = gather.methods.end();
    for(; mit != mnd; mit++)
      {
				code_type_resolver typer(context_);
				mit->cde.visit(&typer);
				str type;
				schema* tt = typer.get();
				if (tt)
					type = context_->get_type_name(tt);

				if (type.empty())
					type = mit->type;

				if (!mit->type.empty() && type != mit->type)
					{
						param_list error;
						error.add("id", STypeMismatch);
						error.add("desc", SIncompatibleReturnType);
						error.add("declared as", mit->type);
						error.add("returns",type);
						
						xss_throw(error);
					}

        variant impl = idiom_->process_method(instance, *mit);
        methods->push_back(impl);
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
                actual_instance = get_instance(inst_name);
              }
            else
              {
                complete_name += '.' + inst_name;
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

        DynamicArray impls = get_event_impl(actual_instance, event_name);

        //let te idiom process implementations
        variant impl = idiom_->process_event(actual_instance, event_name, *it);
        impls->push_back(impl);
      }

		std::vector<xs_instance>::iterator iit = gather.instances.begin();
		std::vector<xs_instance>::iterator ind = gather.instances.end();
		for(; iit != ind; iit++)
			{
				xs_instance& instance_ast = *iit;
				XSSObject		 instance_instance = resolve_path(instance_ast.id, instance);

				compile_ast(instance_ast, instance_instance);
			}
	}

str wind(const std::vector<str> path)
	{
		str result;
		for(size_t i = 0; i < path.size(); i++)
			result += path[i] + '.';

		if (!result.empty())
			result.erase(result.end() - 1);

		return result;
	}

std::vector<str> unwind(const str& path)
	{
		std::vector<str> result;
		size_t pos = 0;
		while(pos < path.size())
			{
				size_t last_pos = pos;
				pos = path.find('.', pos);
				if (pos != str::npos)
					{
						result.push_back(path.substr(last_pos, pos - last_pos));
						pos++;
					}
				else
					{
						result.push_back(path.substr(last_pos, path.size() - last_pos));
					}
			}

		return result;
	}

variant xss_project::resolve_property(const str& prop, variant parent)
	{
		XSSObject base;
		str				path_str;

		if (parent.is<str>())
			{
				str id(parent);
				base = resolve_path(unwind(id), XSSObject());
				path_str = id;
			}
		else if (parent.is<XSSObject>())
			base = parent;

		std::vector<str> path = unwind(prop); assert(!path.empty());
		str prop_name = path[path.size() - 1];
		path.erase(path.end() - 1);

		XSSObject obj = resolve_path(path, base);
		if (!obj)
			return variant();

		XSSProperty propobj = obj->get_property(prop_name);
		if (!propobj)
			return variant();

		sponge_object* r = new sponge_object;
		r->add_property("prop", propobj);
		r->add_property("path", wind(path));
		r->add_property("obj", obj);

		DynamicObject result(r);
		return result;
	}

str xss_project::last_rendered(int count)
	{
		if (!current_)
			return "";

		str result = current_->get();
		boost::trim(result);

		int curr_count = 0;
		int char_count = 0;

		str::reverse_iterator it = result.rbegin();
		str::reverse_iterator nd = result.rend();
		for(; it != nd; it++, char_count++)
			{
				char c = *it;
				if (c == '\n')
					{
						curr_count++;
						if (curr_count > count)
							{
								return result.substr(result.size() - char_count, char_count);
							}
					}
			}

		return result;
	}

void xss_project::log(const param_list params)
	{
    for(int i = (int)params.size() - 1; i >= 0; i--)
      {
        str param_name = params.get_name(i);
        variant value  = params.get(i);

        str string_value = variant_cast<str>(value, "");
				if (string_value.empty())
					{
					}
				else
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

str xss_project::generate_expression(const str& expr)
	{
		xs_utils	 xs;
		expression e;
		if (!xs.compile_expression(expr, e))
			{
				param_list error;
				error.add("id", SCompile);
				error.add("desc", SInvalidExpression);
				error.add("expression", expr);
						
				xss_throw(error);
			}

		XSSProject me(shared_from_this());
    XSSContext context(new xss_code_context(me, idiom_));
		fill_context(context);

		return idiom_utils::expr2str(e, context_);
	}

XSSObject xss_project::resolve_path(const std::vector<str>& path, XSSObject base)
	{
    XSSObject result = base;
    str inst_name;
		str complete_name;
    for(size_t idx = 0; idx < path.size(); idx++)
      {
        inst_name = path[idx];
        if (idx == 0)
          {
						complete_name = inst_name;
            result = get_instance(inst_name);
          }
        else
          {
            complete_name += '.' + inst_name;
						variant vv;
            if (dynamic_try_get(result, inst_name, vv))
              {
                result = variant_cast<XSSObject>(vv, XSSObject());
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

    if (!result)
			{
        param_list error;
        error.add("id", SCannotResolve);
        error.add("desc", SUnknownInstance);
        error.add("instance", complete_name);
						
        xss_throw(error);
			}

		return result;
	}

void xss_project::compile_instance(const str& filename, XSSObject instance)
  {
    //parse the xs into top level constructs, like properties and stuff
		fs::path fname = base_path_ / source_path_ / filename; 
    str source = load_file( fname.string() );

    code_context  ctx;
    xs_compiler  compiler;
    xs_container ast;
    compiler.compile_xs(source, ast);

		compile_ast(ast, instance);
  }

void xss_project::register_instance(const str& id, XSSObject it)
  {
    instances_.insert(instance_registry_pair(id, instances.size()));
    instances.push_back(it);
  }

void xss_project::render_instance(XSSObject instance, const str& xss, int indent)
  {
		fs::path fname = base_path_ / source_path_ / xss; 
		str gen_text = load_file(fname.string());

    str id = variant_cast<str>(dynamic_get(instance, "id"), "");
    str class_name = variant_cast<str>(dynamic_get(instance, "class_name"), "");

    //setup the context
		XSSProject me(shared_from_this());
    XSSContext context(new xss_code_context(me, idiom_));
    xss_code_context& ctx = *context.get();
    XSSGenerator gen(new xss_generator(context));
    
		push_generator(gen);
		
		//do the deed
    prepare_context(ctx, gen);
		context->scope_->register_symbol("it", instance); //td: make sure its not there

		str result = generate_xss(gen_text, gen);
		if (indent > 0)
			{
				str indent_str;
				int spaces = indent*4;
				for(int i = 0; i < spaces; i++) //td: tab size
					indent_str += " ";

				int npos = 0;
				do 
				{
					result.insert(result.begin() + npos + 1, indent_str.begin(), indent_str.end());
					npos = result.find("\n", npos + spaces + 1);
				}
				while (npos != str::npos && npos < result.size() - 1);
			}

		pop_generator();

		current_->append(result);
}

str xss_project::resolve_dispatcher(XSSObject instance, const str& event_name)
  {
    str           class_name = instance_class(instance);
    XSSObject clazz      = find_class(class_name);

    if (clazz)
      {
        iterable dispatchers(dynamic_get(clazz, "dispatchers"));
        iterator it = dispatchers.begin();
        iterator nd = dispatchers.end();

        for(; it != nd; ++it)
          {
            XSSObject dp = *it;
            str key = variant_cast<str>(dynamic_get(dp, "id"), "");
            if (key == event_name)
              return variant_cast<str>(dynamic_get(dp, "value"), "");
          }
      }

    return "";
  }

str xss_project::instance_class(XSSObject instance)
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

str xss_project::inline_properties(XSSObject instance)
  {
    param_list error;
    error.add("id", SNotImplemented);
    error.add("desc", SWhatWasThisAgain);
    xss_throw(error);
    return "";
  }

XSSObject xss_project::find_class(const str& event_name)
  {
    param_list error;
    error.add("id", SNotImplemented);
    error.add("desc", SWhatWasThisAgain);
    xss_throw(error);
    return XSSObject();
  }

void xss_project::breakpoint(const param_list params)
  {
    for(int i = (int)params.size() - 1; i >= 0; i--)
      {
        str param_name = params.get_name(i);
        variant value  = params.get(i);

        str           string_value = variant_cast<str>(value, "");
        XSSObject obj_value    = variant_cast<XSSObject>(value, XSSObject());
				
				str	obj_id;
				if (obj_value)
					{
						obj_id = variant_cast<str>(dynamic_get(obj_value, "id"), str(""));
					}
      }
  }

DynamicArray xss_project::get_property_array(XSSObject obj)
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

DynamicArray xss_project::get_event_impl(XSSObject obj, const str& event_name)
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

XSSGenerator xss_project::generator()
	{
		return current_;
	}

DynamicArray xss_project::get_method_array(XSSObject obj)
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

DynamicArray xss_project::get_event_array(XSSObject obj)
  {
		return obj->events();
  }

DynamicArray xss_project::get_children_array(XSSObject obj)
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

XSSObject xss_project::get_instance(const str& id)
  {
      instance_registry::iterator it = instances_.find(id);
      if (it != instances_.end())
        return instances[it->second];

      return XSSObject();
  }

void xss_project::add_application_file(const str& file, XSSObject obj)
  {
    app_files_.push_back( file_info(file, obj) );
  }

str	xss_project::source_file_name(const str& fname)
	{
		fs::path filename = base_path_ / source_path_ / fname; 
    return filename.string();
	}

str	xss_project::output_file_name(const str& fname)
	{
		fs::path filename = base_path_ / output_path_ / fname; 
    return filename.string();
	}

str xss_project::load_file(const str& fname)
  {
    std::ifstream ifs(fname.c_str());
		if (!ifs.good())
			{
				param_list error;
				error.add("id", SFileError);
				error.add("desc", SFileNotFound);
				error.add("file", fname);
						
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

void xss_project::save_file(const str& fname, const str& contents)
  {
    std::ofstream ofs(fname.c_str());
    ofs << contents;
    ofs.close();
  }

void xss_project::fill_context(XSSContext ctx)
	{
    ctx->scope.register_symbol("application", application);

    //setup the scope
    instance_registry::iterator it = instances_.begin();
    instance_registry::iterator nd = instances_.end();
    for(; it != nd; it++)
      {
        ctx->scope.register_symbol(it->first, it->second);
      }
	}

void xss_project::prepare_context(base_code_context& context, XSSGenerator gen)
  {
    context.this_type = type_schema<xss_project>();
    context.this_     = this; //makes our methods directly accesible

    //fill up the context
		XSSProject me(shared_from_this());
    DslLinker ol(new out_linker(me));
    context.dsls.insert(dsl_list_pair("out", ol));

    //add our identifiers
		reference<out> out_obj(new out(me));

    context.scope.register_symbol("application", application);
    context.scope.register_symbol("compiler", me);
    context.scope.register_symbol("out", out_obj);

    //setup the scope
    instance_registry::iterator it = instances_.begin();
    instance_registry::iterator nd = instances_.end();
    for(; it != nd; it++)
      {
        context.scope.register_symbol(it->first, it->second);
      }

    //and some types
    context.types_->add_type("array", &array_type_);
    context.types_->add_type<sponge_object>("object");
  }

str xss_project::generate_xss(const str& xss, XSSGenerator gen)
  {
    xss_parser parser;
    parser.register_tag("xss:code");
    parser.register_tag("xss:e");
    parser.register_tag("xss:class");
    parser.register_tag("xss:file");
    parser.register_tag("xss:marker");

		parser.parse(xss, gen.get());
    return gen->get();
  }

void xss_project::output_file(const str& fname, const str& contents)
	{
		fs::path filename = base_path_ / output_path_ / fname; 
		save_file(filename.string(), contents);
	}

str	xss_project::get_anonymous_id(const str& class_name)
	{
		anonymous_list::iterator ait  = anonymous_.find(class_name);
		int											 aidx = 1;
		if (ait == anonymous_.end())
			{
				anonymous_.insert( std::pair<str, int>(class_name, aidx ));
			}
		else
			{
				aidx = ++ait->second;
			}

		return "__" + class_name + boost::lexical_cast<str>(aidx);
	}

XSSObject xss_project::get_class(const str& name)
	{
    class_registry::iterator cit = classes_.find(name);
		if (cit != classes_.end())
			return cit->second;

		return XSSObject();
	}

variant xss_project::evaluate_property(XSSObject obj, const str& prop_name)
	{
		if (obj && obj->has(prop_name))
			return dynamic_get(obj, prop_name);

		DynamicArray props = get_property_array(obj);
    for(size_t i = 0; i < props->size(); i++)
      {
        XSSProperty prop = props->at(i);
        if (prop->name == prop_name)
          {
						return prop->generate_value();
          }
      }

		return variant();
	}	

str xss_project::generate_file(const str& fname, XSSContext context)
  {
		fs::path filepath = base_path_ / source_path_ / fname; 
    str gen_text = load_file(filepath.string());

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
    XSSObject app_object = application;
    pre_process pp(app_object, XSSObject(), *this, true);

		str id = "application";
		pp.instantiate(app_object, id, "application");
		app_object->visit(&pp);
  }

void xss_project::read_classes(const str& class_library_file)
  {
    type_registry types;
    types.set_default_type(type_schema<xss_object>());
		types.add_type<xss_property>("property");
		types.add_type<xss_event>("event");

		fs::path filepath = base_path_ / source_path_ / class_library_file; 

		xml_read_archive class_library_achive(load_file(filepath.string()), &types);

    classes = class_library_achive.get( type_schema<XSSObjectList>() );

    XSSObjectList::iterator it = classes.begin();
    XSSObjectList::iterator nd = classes.end();

    for(; it != nd; it++)
      {
        XSSObject     obj		= *it;
        str           id		= variant_cast<str>(dynamic_get(obj, "id"),		 "");
        str           super	= variant_cast<str>(dynamic_get(obj, "super"), "");

        if (id.empty())
          {
            param_list error;
            error.add("id", SOutOfContext);
            error.add("desc", SClassesMustHaveId);
            xss_throw(error);
          }

				if (!super.empty())
					{
						class_registry::iterator sit = classes_.find(super);
						if (sit == classes_.end())
							{
								param_list error;
								error.add("id", SCannotResolve);
								error.add("desc", SNoSoup);
								error.add("class", super);
								xss_throw(error);
							}

						XSSObject super = sit->second;
						obj->xss_type(super);
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
		generators_.pop();

		if (!generators_.empty())
			current_ = generators_.top();
		else
			current_ = XSSGenerator();
	}
