
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
const str SOnlyClassAllowed("Only class are allowed in include files");
const str SBadInheritance("A class cannot inherit different types");
const str SDuplicateClassOnLibrary("Duplicate class");

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
	sealed_(true),
	children_(new dynamic_array),
	properties_(new dynamic_array),
	methods_(new dynamic_array),
	events_(new dynamic_array)
	{
	}

void xss_object::visit(dynamic_visitor* visitor)
	{
		visitor->item("children", children_);
		visitor->item("properties", properties_);
		visitor->item("methods", methods_);
		visitor->item("events", events_);

		editable_object<xss_object>::visit(visitor);
	}

XSSObject xss_object::type()
	{
		return type_;
	}

void xss_object::propertize()
	{
		XSSObject me(shared_from_this());

		//convert attributes to properties
		item_list::iterator it = items_.begin();
		item_list::iterator nd = items_.end();
		for(; it != nd; it++)
			{
				if (it->first == "type" ||
						it->first == "class" ||
						it->first == "class_name" ||
						it->first == "id")
						continue;

				str			type  = ""; //td: !!!
				variant	value = dynamic_get(this, it->first);
				XSSProperty prop(new xss_property(it->first, type, value, me));

				properties_->push_back(prop);
			}
	}

void xss_object::xss_type(XSSObject type)
	{
		type_ = type;

		XSSObject me(shared_from_this());
		property_mixer pm(type, me);
		type_->visit(&pm);

		bool native = variant_cast<bool>(dynamic_get(type, "native"), false);
		if (native)
			{
				str id = variant_cast<str>(dynamic_get(type, "id"), str());
				dynamic_set(this, "native_class", id);
			}

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

		if (native)
			return; //no need to add class methods, these will be generated in the native class

		DynamicArray tmethods = type->methods();
		for(int i = 0; i < tmethods->size(); i++)
			{
				XSSMethod mthd	 = tmethods->at(i);
				XSSMethod mymthd = get_method(mthd->name);

				if (mymthd)
					{
						assert(false); //td: do super
					}

				XSSMethod cln(new xss_method(*(mthd.get())));
				cln->copy(mthd.get()); //td: proper cloning

				methods_->insert(cln);
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

		DynamicArray tchildren = type->children();
		for(int i = 0; i < tchildren->size(); i++)
			{
				XSSObject child = tchildren->at(i);

				str iiid = variant_cast<str>(dynamic_get( child, "id" ), str());
				str iidd = variant_cast<str>(dynamic_get( this, "id" ), str());

				children_->insert(child); //td: cloning
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

void xss_object::remove_child(XSSObject obj)
	{
		int child_count = children_->size();
		for(int i = child_count - 1; i >= 0; i--)
			{
				XSSObject child = variant_cast<XSSObject>(children_->at(i), XSSObject());
				if (child && child == obj)
					children_->remove(i);
			}
	}

void xss_object::copy(xss_object* other)
	{
		item_list::iterator it = other->items_.begin();
		item_list::iterator nd = other->items_.end();
		for(; it != nd; it++)
			{
				if (it->first == "@class")
					continue; //td: generalize system variables

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

XSSMethod	xss_object::get_method(const str& name)
	{
		for(int i = 0; i < methods_->size(); i++)
			{
				XSSMethod mthd = methods_->at(i);
				if (mthd->name == name)
					return mthd;
			}

		return XSSMethod();
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
				str iiid = variant_cast<str>(dynamic_get( obj, "id" ), str());
				str iidd = variant_cast<str>(dynamic_get( this, "id" ), str());

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
		if (sealed_)
			return;

		sealed_ = true;
		for(int i = 0; i < children_->size(); i++)
			{
				XSSObject obj = variant_cast<XSSObject>(children_->at(i), XSSObject());
				if (obj)
					obj->seal();
			}
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

XSSMethod xss_code_context::get_method(const str& name)
	{
    XSSObject obj = variant_cast<XSSObject>(this_, XSSObject());
    if (obj)
      {
        return get_method(obj, name);
      }
    return XSSMethod();
	}

XSSMethod	xss_code_context::get_method(XSSObject obj, const str& name)
	{
    if (obj)
			return obj->get_method(name);

    return XSSMethod();
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

XSSObject	xss_code_context::get_xss_type(const str& name)
	{
		variable_types::iterator it = vars_.find(name);
		if (it != vars_.end())
			return it->second;
		return XSSObject();
	}

bool xss_code_context::has_var(const str& name)
	{
		variable_types::iterator it = vars_.find(name);
		return it != vars_.end();
	}

void xss_code_context::register_variable(const str& name, XSSObject xss_type)
	{
		vars_.insert(std::pair<str, XSSObject>(name, xss_type));
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
  this_(other.this_),
	type(other.type)
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

str	xss_property::resolve_value()
	{
    str get_fn = variant_cast<str>(dynamic_get(this, "get_fn"), "");

		if (!get.empty())
			return name + "_get()";
		else if (!get_fn.empty())
			return get_fn + "()";
		return name;
  }

//xss_method
xss_method::xss_method()
	{
		DYNAMIC_INHERITANCE(xss_method)
	}

xss_method::xss_method(const xss_method& other):
	xss_object(other),
	name(other.name),
	type(other.type),
	args(other.args),
	code(other.code)
	{
		DYNAMIC_INHERITANCE(xss_method)
	}

xss_method::xss_method(const str& _name, const str& _type, variant _args, variant _code) :
	name(_name),
	type(_type),
	args(_args),
	code(_code)
	{
		DYNAMIC_INHERITANCE(xss_method)
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
						for(int i = children->size() - 1; i >= 0; i--)
							{
								XSSObject child = children->at(i);
								child->seal();

                variant v   = dynamic_get(child, "id");
                str     id  = variant_cast<str>(v, "");
								str			class_name;

								if (dynamic_try_get(child, "class", v) && !v.empty())
                  {
                    class_name = variant_cast<str>(v, str());

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
            str type			= variant_cast<str>(dynamic_get(obj, "type"), "");
						variant value = obj;

						//and as long as I'm plugging, what the hell
						if (type == "array")
							{
								//build up child objects based on their attributes
								DynamicArray children = obj->children();
								for(int i = 0; i < children->size(); i++)
									{
										//td: simple types
										XSSObject obj = children->at(i);
										obj->propertize();
									}
							}

						if (obj->has("value"))
							value = dynamic_get(obj, "value");

						str prop_name = variant_cast<str>(dynamic_get(obj, "name"), id);
						XSSProperty prop(new xss_property(prop_name, type, value, object_));

            object_->properties()->push_back(prop);
						object_->remove_child(obj);
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
				value = variant_cast<str>(v, str());
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
    idiom_ = idiom;

		//grab the path
    //td: change path to config... or something.
    str sp = variant_cast<str>(dynamic_get(path, "source_path"), "");
    source_path_ = sp;

    str op = variant_cast<str>(dynamic_get(path, "output_path"), "");
    output_path_ = op;

    //contextualize
    XSSProject me(shared_from_this());
    context_  = XSSContext(new xss_code_context(me, idiom_));
    current_  = XSSGenerator(new xss_generator(context_));

    //td: stupid references
    xss_code_context& code_ctx = *(context_.get());

		prepare_context(code_ctx, current_);
    code_ctx.scope_->register_symbol("project", me);

    idiom_->set_context(context_);

		//load the classes
    str class_library = variant_cast<str>(dynamic_get(path, "class_library"), "");

    read_classes(class_library);
		do_includes();

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
        push_file(it->file);
				compile_instance(it->file, it->obj);
        pop_file();
      }

    str generator_file = variant_cast<str>(dynamic_get(path, "generator"), "");

		str result = generate_file(generator_file);

    str of = variant_cast<str>(dynamic_get(path, "output_file"), "");

		fs::path fname = base_path_ / output_path_ / of;
		save_file(fname.string(), result);
  }

void xss_project::do_includes()
	{
		XSSSerialObjectList::iterator it = includes.begin();
		XSSSerialObjectList::iterator nd = includes.end();
		for(; it != nd; it++)
			{
				str src_file = variant_cast<str>(dynamic_get(*it, "src"), str(""));
				str def_file = variant_cast<str>(dynamic_get(*it, "def"), str(""));

				//so we must mix instances again, the strategy in this case is to
				//grab the code and then add the props coming from the definition
				type_registry types;
				types.set_default_type(type_schema<xss_serial_object>());

				fs::path filepath = base_path_ / source_path_ / def_file;
				xml_read_archive arch(load_file(filepath.string()), &types, XML_RESOLVE_CLASS|XML_RESOLVE_ID); //td: generalize xml

				//grab the definition clasess and extract their names for efficiency purposes
				typedef std::map<str, XSSObject> def_clazz_list;

				XSSSerialObjectList def_classes = arch.get( type_schema<XSSSerialObjectList>() );

				def_clazz_list def_clazzes;
				XSSSerialObjectList::iterator dit = def_classes.begin();
				XSSSerialObjectList::iterator dnd = def_classes.end();
				for(; dit != dnd; dit++)
					{
						XSSObject def_clazz = (XSSObject)(*dit);
						def_clazz->seal(); //plumbing

						str	def_class_id  = variant_cast<str>(dynamic_get(def_clazz, "id"), str()); assert(!def_class_id.empty());

						def_clazz_list::iterator dcit = def_clazzes.find(def_class_id);
						if (dcit != def_clazzes.end())
							{
								param_list error;
								error.add("id", SOutOfContext);
								error.add("desc", SDuplicateClassOnLibrary);
								error.add("class", def_class_id);
								xss_throw(error);
							}

						def_clazzes.insert(std::pair<str, XSSObject>(def_class_id, def_clazz));
					}

				//now get the xs classes
				XSSObjectList src_classes = read_xs_classes(src_file);

				XSSObjectList::iterator sit = src_classes.begin();
				XSSObjectList::iterator snd = src_classes.end();
				for(; sit != snd; sit++)
					{
						XSSObject clazz    = *sit;
						clazz->seal();

						str				class_id = variant_cast<str>(dynamic_get(clazz, "id"), str()); assert(!class_id.empty());
						str				super		 = variant_cast<str>(dynamic_get(clazz, "super"), str());
						XSSObject super_clazz;

						def_clazz_list::iterator dcit = def_clazzes.find(class_id);
						if (dcit != def_clazzes.end())
							{
								//check super classes
								str	def_super = variant_cast<str>(dynamic_get(dcit->second, "super"), str());
								if (!def_super.empty())
									{
										if (super.empty())
											{
												super       = def_super;
												super_clazz = get_class(def_super, true);
												clazz->xss_type(super_clazz);
											}
										else if (super != def_super)
											{
												param_list error;
												error.add("id", SOutOfContext);
												error.add("desc", SBadInheritance);
												error.add("super 1", def_super);
												error.add("super 2", super);
												xss_throw(error);
											}
									}


								super_clazz = get_class(super, true);
								dynamic_set(clazz, "super", super);
								dynamic_set(clazz, "super_class", super_clazz);

								//and composite
								clazz->xss_type(dcit->second);
							}

						clazz->add_property("native", true);
						classes.push_back(clazz);
						classes_.insert(class_registry_pair(class_id, clazz));
					}
			}
	}

xss_project::XSSObjectList xss_project::read_xs_classes(const str& xs_file)
	{
		XSSObjectList result;

		fs::path filepath = base_path_ / source_path_ / xs_file;

		xs_compiler  compiler;
    xs_container results;
		compiler.compile_xs(load_file(filepath.string()), results);

    for(size_t i = 0; i < results.size(); i++)
      {
				variant vv = results.get(i);
				if (!vv.is<xs_class>())
					{
						//what kinda class are you?
						param_list error;
						error.add("id", SCompile);
						error.add("desc", SOnlyClassAllowed);
						xss_throw(error);
					}

				XSSObject clazz = XSSObject(new xss_object());

				xs_class  ci = vv;
				if (!ci.super.empty())
					{
						XSSObject super = get_class(ci.super);
						if (!super)
							{
								param_list error;
								error.add("id", SCannotResolve);
								error.add("desc", SUnknownClass);
								error.add("class", ci.super);
								xss_throw(error);
							}

						clazz->xss_type(super);
						dynamic_set(clazz, "super", ci.super);
						dynamic_set(clazz, "super_class", super);
					}

				dynamic_set(clazz, "id", ci.name);
				dynamic_set(clazz, "@class", true); //some fun

				XSSContext ictx(new xss_composite_context(context_));
				ictx->this_ = clazz;

				compile_ast(ci, ictx);
				result.push_back(clazz);
      }

		return result;
	}

void xss_project::push_file(const str& fname)
	{
    std::cout << "Processing: " << fname << '\n';
		file_stack_.push(fname);
	}

void xss_project::pop_file()
	{
		file_stack_.pop();
	}

str	xss_project::top_file()
	{
		if (file_stack_.empty())
			return "pre-process";

		return file_stack_.top();
	}

str xss_project::output_path()
  {
    return output_path_.string();
  }

void xss_project::base_path(fs::path path)
	{
		base_path_ = path;
	}

void xss_project::compile_ast(xs_container& ast, XSSContext ctx)
	{
    source_code_gather gather;
    ast.visit(&gather);

    //create a safe array to pass around, this should be the standard practice
    //
		XSSObject instance = variant_cast<XSSObject>(ctx->this_, XSSObject());

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

						getter = idiom_->process_code(pit->get, args, ctx);
					}

        if (!pit->set.empty())
					{
						param_list_decl args;
						param_decl			arg;
						arg.name = "value";
						arg.type = pit->type;
						args.push_back(arg);
						setter = idiom_->process_code(pit->set, args, ctx);
					}

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
				
				param_list_decl::iterator mait = mit->args.begin();
				param_list_decl::iterator mand = mit->args.end();
				for(; mait != mand; mait++)
					{
						typer.register_var(mait->name, context_->get_type(mait->type));
					}
 
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

				variant args = idiom_->process_args(mit->args);									assert(!args.empty());
				variant cde  = idiom_->process_code(mit->cde, mit->args, ctx);  assert(!cde.empty());

				XSSMethod mthd(new xss_method(mit->name, type, args, cde));
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

				XSSEvent		 ev;
        DynamicArray impls = get_event_impl(actual_instance, event_name, ev);

				if (actual_instance != instance && options("gen_event_method"))
					{
						//here's one. It would seem convinient that events implemented in
						//the context of a different instance would generate code using such
						//instance as the *this* pointer. At least this should be a compiler option.

						str aid    = variant_cast<str>(dynamic_get(actual_instance, "id"), str("")); assert(!aid.empty());
						str iid    = variant_cast<str>(dynamic_get(instance, "id"), str("")); assert(!iid.empty());
						str	mname  = aid + "_" + event_name;

						//so, create a method in the original instance
						variant margs = idiom_->process_args(it->args);								 assert(!margs.empty());
						variant mcde  = idiom_->process_code(it->cde, it->args, ctx);  assert(!mcde.empty());

						XSSMethod mthd(new xss_method(mname, "", margs, mcde));
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
								XSSContext ictx(new xss_composite_context(ctx));
								ictx->this_ = actual_instance;

								variant impl = idiom_->process_code(cde, it->args, ictx);
								impls->push_back(impl);
							}
					}
				else
					{
						//let the idiom process implementations
						XSSContext ictx(new xss_composite_context(ctx));
						ictx->this_ = actual_instance;
						variant impl = idiom_->process_code(it->cde, it->args, ictx);
						impls->push_back(impl);
					}

				ev->args = idiom_->process_args(it->args);
      }

		std::vector<xs_instance>::iterator iit = gather.instances.begin();
		std::vector<xs_instance>::iterator ind = gather.instances.end();
		for(; iit != ind; iit++)
			{
				xs_instance& instance_ast = *iit;
				str					 pth;
				XSSObject		 instance_instance = resolve_path(instance_ast.id, instance, pth);

				XSSContext ictx(new xss_composite_context(context_));
				ictx->this_ = instance_instance;
				compile_ast(instance_ast, ictx);
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
				str id = variant_cast<str>(parent, str());
				str	pth;
				base = resolve_path(unwind(id), XSSObject(), pth);
				path_str = id;
			}
		else if (parent.is<XSSObject>())
			base = parent;

		std::vector<str> path = unwind(prop); assert(!path.empty());
		str prop_name = path[path.size() - 1];
		path.erase(path.end() - 1);

		str pth;
		XSSObject obj = resolve_path(path, base, pth);
		if (!obj)
			return variant();

		XSSProperty propobj = obj->get_property(prop_name);

		sponge_object* r = new sponge_object;

		if (propobj)
			r->add_property("prop", propobj);
		else
			r->add_property("prop", variant());

		r->add_property("prop_name", prop_name);
		r->add_property("path", pth);
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

str xss_project::generate_expression(const str& expr, XSSObject this_)
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
		context->this_ = this_;

		return idiom_utils::expr2str(e, context);
	}

bool xss_project::parse_expression(variant v)
	{
		if (!v.is<str>())
			return false;

		str s = variant_cast<str>(v, str());
		xs_compiler compiler;
		expression expr;
		return compiler.compile_expression(s, expr);
	}

bool xss_project::options(const str& name)
	{
		XSSObject obj = variant_cast<XSSObject>(options_, XSSObject());
		if (obj)
			{
				bool result = variant_cast<bool>(dynamic_get(obj, name), false);
				return result;
			}

		return false;
	}

bool xss_project::is_object(const variant v)
	{
		XSSObject obj = variant_cast<XSSObject>(v, XSSObject());
		return (bool)obj;
	}

str xss_project::generate_property(XSSProperty prop, XSSObject this_)
	{

		return "IMPLEMENT ME";
	}

str xss_project::translate_type(const str& type_name)
	{
		return idiom_->translate_type(type_name);
	}

xss_idiom* xss_project::get_idiom()
	{
		return idiom_;
	}

str xss_project::replace_this(const str& s, const str& this_)
	{
		str    result;
		size_t curr = 0;
		size_t pos = s.find("this");
		while(pos != str::npos)
			{
				result += s.substr(curr, pos - curr) + this_;
				curr = pos + 4;
				pos = s.find("this", curr);
			}

		result += s.substr(curr, s.size() - curr);
		return result;
	}

XSSObject xss_project::resolve_path(const std::vector<str>& path, XSSObject base, str& result)
	{
    XSSObject obj = base;
    str inst_name;
		bool first = true;
    for(size_t idx = 0; idx < path.size(); idx++)
      {
        inst_name = path[idx];
				if (obj)
					{
						//look for properties first
						XSSProperty prop	= obj->get_property(inst_name);
						bool				found = false;
						if (prop)
							{
								if (!prop->type.empty())
									{
										XSSObject clazz = get_class(prop->type);
										if (clazz)
											{
												obj = clazz; //td: using the type, its ok because the resolving process is const

												if (first)
													{
														XSSContext ctx(new xss_composite_context(context_));
														ctx->this_ = base;

														result += idiom_->resolve_this(ctx);
														first  = false;
													}

												result += first? prop->resolve_value() : "." + prop->resolve_value();
												found = true;
											}
										else
											{
												assert(false); //use case
											}
									}
								else
									{
										if (first)
											{
												XSSContext ctx(new xss_composite_context(context_));
												ctx->this_ = base;

												result += idiom_->resolve_this(ctx);
												first  = false;
											}

										result += first? prop->resolve_value() : "." + prop->resolve_value();
										found = true;
									}
							}

						if (found)
							continue;
					}

        if (first)
          {
						result = inst_name;
            obj    = get_instance(inst_name);
          }
        else
          {
            result += '.' + inst_name;
						variant vv;
            if (dynamic_try_get(result, inst_name, vv))
              {
                obj = variant_cast<XSSObject>(vv, XSSObject());
              }
            else
							{
								param_list error;
								error.add("id", SCannotResolve);
								error.add("desc", SUnknownInstance);
								error.add("instance", result);

								xss_throw(error);
							}
          }
      }

    if (!obj)
			{
        param_list error;
        error.add("id", SCannotResolve);
        error.add("desc", SUnknownInstance);
        error.add("instance", result);

        xss_throw(error);
			}

		return obj;
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

		XSSContext ictx(new xss_composite_context(context_));
		ictx->this_ = instance;
		compile_ast(ast, ictx);
  }

void xss_project::register_instance(const str& id, XSSObject it)
  {
    instances_.insert(instance_registry_pair(id, instances.size()));
    instances.push_back(it);
  }

void xss_project::render_instance(XSSObject instance, const str& xss, int indent)
  {
		push_file(xss);

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

		pop_file();
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

DynamicArray xss_project::get_event_impl(XSSObject obj, const str& event_name, XSSEvent& ev)
  {
		DynamicArray events = obj->events();
    for(size_t i = 0; i < events->size(); i++)
      {
        ev = events->at(i);
        if (ev->name == event_name)
          return ev->impls;
      }

    //not implemented, create
    ev = XSSEvent(new xss_event);
    ev->name = event_name;

    events->push_back(ev);
    return ev->impls;
  }

XSSGenerator xss_project::generator()
	{
		return current_;
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
    parser.register_tag("xss:instance");

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

XSSObject xss_project::get_class(const str& name, bool enforce)
	{
    class_registry::iterator cit = classes_.find(name);
		if (cit != classes_.end())
			return cit->second;

		if (enforce)
			{
        param_list error;
        error.add("id", SCannotResolve);
				error.add("desc", SUnknownClass);
        xss_throw(error);
			}

		return XSSObject();
	}

variant xss_project::evaluate_property(XSSObject obj, const str& prop_name)
	{
		if (obj && obj->has(prop_name))
			return dynamic_get(obj, prop_name);

		DynamicArray props = obj->properties();
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
		push_file(fname);

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
		pop_file();

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
    types.set_default_type(type_schema<xss_serial_object>());
		types.add_type<xss_property>("property");
		types.add_type<xss_method>("method");
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

				obj->seal();
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
