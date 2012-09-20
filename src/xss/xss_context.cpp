
#include "xss/xss_context.h"
#include "xss/xss_error.h"
#include "xss/language.h"
#include "xss/lang/base.h"

using namespace xkp;

const str STypeMismatch("type-mismatch");
const str SContextError("context");

const str SCannotOverrideTypes("Cannot override types");
const str SDupType("Duplicate type");
const str SContextHasNoPath("Requesting path on a context without it");
const str SDuplicateSymbol("A symbol with this name already exists");
const str SEmptyArrayType("Arrays must have an inner type");
const str SCannotResolve("Cannot resolve");
const str SArrayMismatch("An array has been declared that already exists on the super class as a non array");
const str SPropertyAlreadyExists("Trying to add a property that already exists");
const str SDupDSL("Duplicate dsl");
const str SGetterNotReturning("property getters must return a value");
const str SGetterTypeMismatch("property getters must return the same type as its property");
const str SMethodReturnTypeMismatch("a method returns a different type than its declaration");

str xss_utils::var_to_string(variant& v)
  {
    if (v.is<str>())
      return variant_cast<str>(v, str());

    if (v.is<bool>())
      {
        bool vv = v;
        if (vv)
          return "true";
        else
          return "false";
      }

    if (v.is<XSSObject>())
      {
        XSSObject obj = v;

        str result = "null";
        if (obj)
          result = obj->id();

        return result;
      }

    if (v.empty())
      return "null";

    return variant_cast<str>(v, str());
  }

//xss_context_scope
xss_context_scope::xss_context_scope(XSSContext owner):
  owner_(owner)
  {
  }

void xss_context_scope::set(XSSContext owner)
  {
    owner_ = owner;
  }

void xss_context_scope::register_symbol(const str& name, variant value)
  {
    assert(false); //td: why would this be called?
  }

bool xss_context_scope::resolve(const str& name, variant& result)
  {
    if (name == "#context")
      {
        result = owner_;
        return true;
      }

    result = owner_->resolve(name);
    return !result.empty();
  }

//xss_context
xss_context::xss_context(XSSContext parent, fs::path path):
  parent_(parent),
  path_(path),
  code_scope_(),
  got_dsls_(false),
  search_native_(false),
  identity_(CTXID_NONE)
  {
    code_types_.add_type<xss_object>("object");
    code_types_.add_type<DynamicArray>("array");
  }

xss_context::xss_context(CONTEXT_IDENTITY identity, variant idobj, XSSContext parent):
  parent_(parent),
  got_dsls_(false),
  search_native_(false),
  identity_(identity),
  identity_obj_(idobj)
  {
    //td: !!! move to app
    code_types_.add_type<xss_object>("object");
    code_types_.add_type<DynamicArray>("array");
  }

XSSType xss_context::get_type(const str& type)
  {
    type_list::iterator it = types_.find(type);
    if (it != types_.end())
      return it->second;

    if (XSSContext parent = parent_.lock())
      return parent->get_type(type);

    return XSSType();
  }

XSSType xss_context::get_type(schema* type)
  {
    type_list::iterator it = types_.begin();
    type_list::iterator nd = types_.end();

    for(; it != nd; it++)
      {
        if (it->second->native_type() == type)
          return it->second;
      }

    if (XSSContext parent = parent_.lock())
      return parent->get_type(type);

    return XSSType();
  }

XSSType xss_context::get_type(const str& type, const str& ns)
  {
    if (ns.empty())
      return get_type(type);

    //td: !!! namespaces
    return get_type(type);
  }

XSSType xss_context::get_array_type(XSSType type)
  {
    str type_name = type->id();
    if (type_name.empty())
      {
        return XSSType();
      }

    str array_type_name = "array<" + type_name + ">"; //im gonna go basic on this one

    resolve_info ri;
    if (resolve(array_type_name, ri))
      {
        if (ri.value.is<XSSType>())
          {
            XSSType type = ri.value;
            return type;
          }
        else
          return XSSType();
      }

    //create it and register
    XSSType new_type = get_language()->resolve_array_type(type, array_type_name, XSSContext(new xss_context(*this)));
    add_type(array_type_name, new_type, true);

    return new_type;
  }

XSSType xss_context::add_type(const str& id, XSSType type, bool override_parent)
  {
    //td: !!! revise this
    if (override_parent)
      {
        type_list::iterator it = types_.find(id);
        if (it != types_.end())
          {
			  it->second = type;
			  type->set_language(lang_);
			  return type;
          }
      }
    else
      {
        XSSType already = get_type(id);
        if (already)
          {
            param_list error;
            error.add("id", SContextError);
            error.add("desc", SDupType);
            error.add("type", id);
            xss_throw(error);
          }
      }

    type->set_language(lang_);
    types_.insert(type_list_pair(id, type));
    return type;
  }

XSSType xss_context::add_type(XSSType type, const str& ns)
  {
    XSSType curr_type = get_type(type->id());
    if (!curr_type)
      types_.insert(type_list_pair(type->id(), type));

    //td: !!! namespaces
    return type;
  }

XSSObject xss_context::get_this()
  {
    assert(false); //gt rid of
    //if (this_)
    //  return this_;

    //if (XSSContext parent = parent_.lock())
    //  return parent->get_type(type);

    return XSSObject();
  }

void xss_context::set_this(XSSObject _this_)
  {
    this_ = _this_;
  }

Language xss_context::get_language()
  {
    if (lang_)
      return lang_;

    if (XSSContext parent = parent_.lock())
      return parent->get_language();

    return Language();
  }

void xss_context::set_language(Language lang)
  {
    lang_ = lang;
  }

code_context xss_context::get_compile_context()
  {
    code_scope_.set(shared_from_this());
    collect_dsl();

    code_context result;
    result.types_  = &code_types_;
    result.scope_  = &code_scope_;
    result.dsl_    = &dsls_;
    result.args_   = &args_;

    return result;
  }

void xss_context::set_path(fs::path path)
  {
    path_ = path;
  }

fs::path xss_context::path()
  {
    if (!path_.empty())
      return path_;

    if (XSSContext parent = parent_.lock())
      return parent->path();

    param_list error;
    error.add("id", SContextError);
    error.add("desc", SContextHasNoPath);
    xss_throw(error);

    return fs::path();
  }

void xss_context::register_dsl(const str& id, DslLinker dsl)
  {
    dsls_.insert(dsl_list_pair(id, dsl));
  }

void xss_context::register_xss_dsl(const str& id, XSSDSL dsl)
  {
    xss_dsl_list::iterator it = xss_dsls_.find(id);
    if (it != xss_dsls_.end())
      {
        param_list error;
        error.add("id", SContextError);
        error.add("desc", SDupDSL);
        error.add("dsl", id);
        xss_throw(error);
      }

    xss_dsls_.insert(xss_dsl_pair(id, dsl));
  }

XSSDSL xss_context::get_xss_dsl(const str& id)
  {
    xss_dsl_list::iterator it = xss_dsls_.find(id);
    if (it != xss_dsls_.end())
      return it->second;

    if (XSSContext parent = parent_.lock())
      return parent->get_xss_dsl(id);

    return XSSDSL();
  }

void xss_context::collect_xss_dsls(std::vector<str>& dsls)
  {
    xss_dsl_list::iterator it = xss_dsls_.begin();
    xss_dsl_list::iterator nd = xss_dsls_.end();
    for(; it != nd; it++)
      {
        dsls.push_back(it->first);          
      }

    if (XSSContext parent = parent_.lock())
      parent->collect_xss_dsls(dsls);
  }

void xss_context::add_parameter(const str& id, XSSType type)
  {
    args_.add(id, variant()); //td: !!! parameter types
  }

void xss_context::set_args(param_list& args)
  {
    args_ = args;
  }

param_list& xss_context::get_args()
  {
    return args_;
  }

void xss_context::search_native(bool enabled)
  {
    search_native_ = enabled;
  }

XSSType xss_context::get_operator_type(operator_type op, XSSType left, XSSType right)
  {
    return get_type("var"); //td: !!!
  }

XSSType xss_context::assign_type(XSSType decl, XSSType value)
  {
    if (!decl->is_variant() && decl != value)
      return XSSType();

    return value;
  }

XSSType xss_context::assure_type(const str& type)
  {
    XSSType result = get_type(type);
    if (!result)
      return result;

    return result;
  }

void xss_context::set_extents(file_position& begin, file_position& end)
  {
    begin_ = begin;
    end_   = end;
  }

file_position& xss_context::begin()
  {
    return begin_;
  }

file_position& xss_context::end()
  {
    return end_;
  }

XSSContext xss_context::parent()
  {
    return parent_.lock();
  }

void xss_context::set_parent(XSSContext ctx)
  {
    parent_ = ctx;
  }

void xss_context::identity(CONTEXT_IDENTITY id, variant idobj)
  {
    assert(identity_ == CTXID_NONE);
    identity_     = id;
    identity_obj_ = idobj;
  }

void xss_context::visit(context_visitor* visitor)
  {
    switch(identity_)
      {
        case CTXID_INSTANCE:
        case CTXID_TYPE:
          {
            XSSObject obj = variant_cast<XSSObject>(identity_obj_, XSSObject());
            obj->context_visit(visitor);
            break;
          }
      }

    symbol_list::iterator it = symbols_.begin();
    symbol_list::iterator nd = symbols_.end();

    for(; it != nd; it++)
      visitor->visit(it->second.type, it->first, it->second.value);

    type_list::iterator tit = types_.begin();
    type_list::iterator tnd = types_.end();

    for(; tit != tnd; tit++)
      visitor->visit(RESOLVE_TYPE, tit->second->id(), tit->second);

    if (XSSContext parent = parent_.lock())
      parent->visit(visitor);
  }

ErrorHandler xss_context::errors()
  {
    if (errors_)
      return errors_;

    if (XSSContext parent = parent_.lock())
      return parent->errors();

    return ErrorHandler();
  }

void xss_context::errors(ErrorHandler handler)
  {
    errors_ = handler;
  }

CONTEXT_IDENTITY xss_context::identity()
  {
	  return identity_;
  }	

variant xss_context::identity_value()
  {
	  return identity_obj_;
  }
  	
variant xss_context::resolve(const str& id, RESOLVE_ITEM item_type)
  {
    resolve_info si;
    si.what = item_type;
    if (resolve(id, si))
      return si.value;

    return empty_type_value(item_type);
  }

variant xss_context::resolve(const str& id, XSSObject instance, RESOLVE_ITEM item_type)
  {
    resolve_info left;
    left.what  = RESOLVE_INSTANCE;
    left.value = instance;
    left.type  = instance->type();

    resolve_info si;
    si.what  = item_type;
    si.left = &left;

    if (resolve(id, si))
      return si.value;

    return empty_type_value(item_type);
  }

bool xss_context::resolve_dot(const str& id, resolve_info& info)
  {
    bool any = false; //kids, dont do this at home

    bool search_this = info.search_this;
    if (this_)
      {
          info.search_this = false;
      }

    switch(info.what)
      {
        case RESOLVE_ANY:
          {
            any = true;
            //fallthru
          }

        case RESOLVE_TYPE:
          {
            if (!info.left)
              {
                type_list::iterator it = types_.find(id);
                if (it != types_.end())
                  {
                    info.what  = RESOLVE_TYPE;
                    info.value = it->second;
                    info.type  = it->second;
                    return true;
                  }
              }

            if (!any) //continue falling thru
              break;
          }

        case RESOLVE_METHOD:
          {
            XSSObject inst;
            if (info.left)
              inst = variant_cast<XSSObject>(info.left->value, XSSObject());
            else if (search_this)
              inst = get_this();

            if (inst)
              {
                XSSMethod mthd = inst->get_method(id);
                if (mthd)
                  {
                    info.what = RESOLVE_METHOD;
                    info.value = mthd;
                    info.type = mthd->type();
                    return true;
                  }
              }

            if (!any)
              return false;
          }

        case RESOLVE_PROPERTY:
          {
            XSSObject inst;
            if (info.left)
              inst = variant_cast<XSSObject>(info.left->value, XSSObject());
            else if (search_this)
              inst = get_this();

            if (inst)
              {
                XSSProperty prop = inst->get_property(id);
                if (prop)
                  {
                    info.what = RESOLVE_PROPERTY;
                    info.value = prop;
                    info.type = prop->type();
                    return true;
                  }
              }

            if (!any)
              return false;
          }

        case RESOLVE_CHILD:
          {
            XSSObject obj;
            if (info.left)
              {
                switch(info.left->what)
                  {
                    case RESOLVE_INSTANCE:
                      {
                        obj = info.left->value;
                        break;
                      }
                    case RESOLVE_VARIABLE:
                      {
                        if (info.left->type && (info.left->type->is_object() || info.left->type->is_variant()))
                          obj = info.left->value;
                        break;
                      }
                    default:
                      {
                        if (!any)
                          return false; //td: is this an error condition?
                      }
                  }

              }
            else if (search_this)
              obj = get_this();

            //td: think this through
            if (obj && obj->type()) // && obj->type()->has("resolve_children")
              {
                XSSObject child = obj->find_by_id(id);
                if (child)
                  {
                    info.what  = RESOLVE_INSTANCE;
                    info.type  = child->type();
                    info.value = child;
                    return true;
                  }
              }

            if (!any)
              break;
          }
      }

    return false;
  }

bool xss_context::identity_search(const str& id, resolve_info& info)
  {
    switch(identity_)
      {
        case CTXID_NONE:
        case CTXID_CODE:
          return false; //nothing to see here

        case CTXID_INSTANCE:
        case CTXID_TYPE:
          {
            XSSObject obj = variant_cast<XSSObject>(identity_obj_, XSSObject());
            assert(obj); //bad context

            return obj->context_resolve(id, info);
          }
      }
    return false;
  }

fs::path xss_context::source_file()
  {
    if (!src_file_.empty())
      return src_file_;

    if (XSSContext parent = parent_.lock())
      return parent->source_file();

    return fs::path();
  }

void xss_context::source_file(fs::path& sf)
  {
    src_file_ = sf;
  }

void xss_context::error(const str& desc, param_list* info, file_position begin, file_position end)
  {
    ErrorHandler eh = errors(); assert(eh);
    fs::path     ep = source_file(); 

    file_location loc(ep, begin, end);
    eh->add(desc, info, loc);
  }

bool xss_context::resolve(const str& id, resolve_info& info)
  {
    if (info.left)
      return resolve_dot(id, info);

    //check globals now
    if (find_symbol(id, info))
      {
        switch(info.what)
          {
            case RESOLVE_INSTANCE:
              {
                XSSObject obj = info.value;
                info.type = obj->type();
                break;
              }
            case RESOLVE_METHOD:
              {
                XSSMethod mthd = info.value;
                info.type = mthd->type();
                break;
              }
            case RESOLVE_PROPERTY:
              {
                XSSProperty prop = info.value;
                info.type = prop->type();
                break;
              }

            case RESOLVE_VARIABLE:
              {
                XSSType type = info.value;
                info.type = type;
                break;
              }

            case RESOLVE_NATIVE:
            case RESOLVE_CONST:
              //all done
              break;
          }

        return true;
      }

    if (identity_search(id, info))
      return true;

	XSSType type = get_type(id);
	if (type)
	  {
		  info.what = RESOLVE_TYPE;
		  info.value = type;
		  return true;
	  }

    if (info.shallow)
      return false;

    if (XSSContext parent = parent_.lock())
      return parent->resolve(id, info);
    
    return false;
  }

bool xss_context::resolve_path(const std::vector<str>& path, resolve_info& info)
  {
    std::vector<str>::const_iterator it = path.begin();
    std::vector<str>::const_iterator nd = path.end();

    Language lang = get_language();

		//state
    bool          first      = true;
		bool          just_paste = false;
    str           result;
    XSSProperty   curr_property;
    XSSObject     curr_instance;

    //resolvers, we'll keep copies
    resolve_info  ri = info;
    resolve_info  last;
    if (info.left)
      {
        last = *info.left;
        ri.left = &last;
      }

    for(; it != nd; it++)
      {
        str item = *it;
        if (just_paste)
          {
            result += lang->resolve_separator() + item;
            continue;
          }

        if (!first)
          ri.search_this = false;

        bool is_last = (it + 1) == nd;

        if (resolve(item, ri))
          {
            switch(ri.what)
              {
                case RESOLVE_INSTANCE:
                  {
                    curr_instance = ri.value;
                    curr_property = XSSProperty();

                    if (!is_last)
                      {
                        if (!first)
                          result += lang->resolve_separator();
                        result += curr_instance->output_id();
                      }
                    break;
                  }
                case RESOLVE_PROPERTY:
                  {
                    curr_instance = XSSObject();
                    curr_property = ri.value;

                    if (first)
                      info.found_this = true;

                    if (!is_last)
                      {
                        if (!first)
                          result += lang->resolve_separator();
                        result += curr_property->output_id();
                      }
                    break;
                  }
                default:
                  {
                    assert(false); //use case
                  }
              }

            last = ri;
            ri.left = &last;
            ri.what = RESOLVE_ANY;
          }
        else
          {
            result = item;
            just_paste = true;
          }

        if (first)
          first = false;
      }

    if (info.output)
      *info.output = result;

    if (curr_instance)
      {
        info.what  = RESOLVE_INSTANCE;
        info.value = curr_instance;
      }
    else if (curr_property)
      {
        info.what  = RESOLVE_PROPERTY;
        info.value = curr_property;
      }
    else
      {
        info.value = variant(); //just makin' sure
      }

    return !just_paste;
  }

void xss_context::register_symbol(RESOLVE_ITEM type, const str& id, variant symbol, bool overrite)
  {
    symbol_list::iterator it = symbols_.find(id);
    if (it != symbols_.end())
      {
        if (overrite)
          {
            it->second.value = symbol;
            return;
          }
        else
          {
            assert(false); //check outside
          }
      }

    symbols_.insert(symbol_list_pair(id, symbol_data(type, symbol)));
  }

variant xss_context::empty_type_value(RESOLVE_ITEM item_type)
  {
    switch (item_type)
      {
        case RESOLVE_INSTANCE:  return XSSObject();
        case RESOLVE_METHOD:    return XSSMethod();
        case RESOLVE_PROPERTY:  return XSSProperty();
        case RESOLVE_VARIABLE:  return XSSType();
        case RESOLVE_TYPE:      return XSSType();
        case RESOLVE_CHILD:     return XSSObject();
      }
    return variant();
  }

bool xss_context::find_symbol(const str& id, resolve_info& info)
  {
    symbol_list::iterator it = symbols_.find(id);
    if (it != symbols_.end())
      {
        info.what  = it->second.type;
        info.value = it->second.value;
        return true;
      }

    return false;
  }

void xss_context::collect_dsl()
  {
    if (got_dsls_)
      return;

    if (XSSContext parent = parent_.lock())
      {
        //aint the most efficient way
        code_context pctx;
        pctx = parent->get_compile_context();
        dsl_list::iterator it = pctx.dsl_->begin();
        dsl_list::iterator nd = pctx.dsl_->end();

        for(; it != nd; it++)
          {
            dsl_list::iterator mine = dsls_.find(it->first);
            if (mine == dsls_.end())
              dsls_.insert(dsl_list_pair(it->first, it->second));
          }
      }

      got_dsls_ = true;
  }

//copy dynamic variables
struct copier : dynamic_visitor
  {
    copier(XSSObject src, xss_object* dest):
      src_(src),
      dest_(dest)
      {
      }

    virtual void item(const str& name, variant value)
      {
        if (name == "type")
          return;

        if (dest_->has(name))
          {
            schema_item result;
            dest_->resolve(name, result);

            if (result.type == type_schema<DynamicArray>())
              {
                //merge array
                if (result.get)
                  {
                    variant value = result.get->get(&(*dest_));

                    DynamicArray da = variant_cast<DynamicArray>(value, DynamicArray());
                    if (da)
                      {
                        DynamicArray obj_da = variant_cast<DynamicArray>(dynamic_get(src_, name), DynamicArray());
                        if (obj_da)
                          {
                            std::vector<variant>::iterator objit = obj_da->ref_begin();
                            std::vector<variant>::iterator objnd = obj_da->ref_end();

                            for(; objit != objnd; objit++)
                              {
                                XSSObject nobj = variant_cast<XSSObject>(*objit, XSSObject());
                                if (nobj)
                                  {
                                    XSSObject cloner(new xss_object);
                                    cloner->copy(nobj);
                                    da->push_back(cloner);
                                  }
                                else
                                  da->push_back(*objit);
                              }
                          }
                      }
                  }
              }
            else
              {
                dynamic_set((IDynamicObject*)dest_, name, value);
              }
          }
        else
          {
            if (value.is<DynamicArray>())
              {
                //some duplicate codes, but,... live is hard :) 
                //td: in next iteration fix this
                DynamicArray dest_da(new dynamic_array()); 
                dest_->add_attribute(name, dest_da);

                if (dest_da)
                  {
                    DynamicArray src_da = variant_cast<DynamicArray>(dynamic_get(src_, name), DynamicArray());
                    if (src_da)
                      {
                        std::vector<variant>::iterator srcit = src_da->ref_begin();
                        std::vector<variant>::iterator srcnd = src_da->ref_end();

                        for(; srcit != srcnd; srcit++)
                          {
                            XSSObject new_obj = variant_cast<XSSObject>(*srcit, XSSObject());
                            if (new_obj)
                              {
                                XSSObject cloner(new xss_object);
                                cloner->copy(new_obj);
                                dest_da->push_back(cloner);
                              }
                            else
                              dest_da->push_back(*srcit);
                          }
                      }
                  }
              }
            else
              {
                dest_->add_attribute(name, value);
              }
          }
      }

    private:
      XSSObject   src_;
      xss_object* dest_;
  };

//xss_object
xss_object::xss_object():
	children_(new dynamic_array),
	properties_(new dynamic_array),
	methods_(new dynamic_array),
	events_(new dynamic_array)
  {
  }

void xss_object::copy(XSSObject obj)
  {
    if (id_.empty())
      id_ = obj->id();

    if (output_id_.empty())
      output_id_ = obj->output_id_;

    if (type_name_.empty())
      type_name_ = obj->type_name();

    if (!type_)
      {
        type_ = obj->type();
      }

    //type information will not be copied, should it?
		DynamicArray obj_children = obj->children();
		std::vector<variant>::iterator it = obj_children->ref_begin();
		std::vector<variant>::iterator nd = obj_children->ref_end();

    for(; it != nd; it++)
      {
        XSSObject obj_child = *it;
        XSSObject my_child = find(obj_child->id());
        if (!my_child)
          my_child = XSSObject(new xss_object);

        my_child->copy(obj_child);

        add_child(my_child);
      }

		DynamicArray obj_properties = obj->properties();
		it = obj_properties->ref_begin();
		nd = obj_properties->ref_end();

    for(; it != nd; it++)
      {
        XSSProperty obj_prop = *it;

        str  prop_id = obj_prop->id();
        bool override_prop = obj_prop->get<bool>("override", false);

        if (override_prop)
          {
            XSSProperty my_prop(new xss_property);
            my_prop->copy(XSSObject(obj_prop));
            properties_->push_back(my_prop);
            continue;
          }

        XSSProperty my_prop = get_shallow_property(prop_id);
        bool        add     = true;
        if (my_prop)
          {
            add = false;
            my_prop->copy(XSSObject(obj_prop));
          }
        else
          {
            my_prop = XSSProperty(new xss_property);
            
            XSSProperty type_prop = get_property(prop_id);
            if (type_prop)
              {
                my_prop->copy(XSSObject(type_prop));
              }

            my_prop->copy(XSSObject(obj_prop));
          }
        
        if (add)
          properties_->push_back(my_prop);
      }

		DynamicArray obj_methods = obj->methods();
		it = obj_methods->ref_begin();
		nd = obj_methods->ref_end();

    for(; it != nd; it++)
      {
        XSSMethod obj_mthd = *it;

        XSSMethod my_methd = get_method(obj_mthd->id());
        if (!my_methd)
          my_methd = XSSMethod(new xss_method);

        my_methd->copy(XSSObject(obj_mthd));
        methods_->push_back(my_methd);
      }

		DynamicArray obj_events = obj->events();
		it = obj_events->ref_begin();
		nd = obj_events->ref_end();

    for(; it != nd; it++)
      {
        XSSEvent obj_ev = *it;
        XSSEvent my_ev = XSSEvent(new xss_event);

        my_ev->copy(XSSObject(obj_ev));
        my_ev->impls = obj_ev->impls;
        events_->push_back(my_ev);
      }

    copier cp(obj, this);
    obj->visit(&cp);
  }

bool xss_object::resolve(const str& name, schema_item& result)
	{
    if (editable_object<xss_object>::resolve(name, result))
			return true;

    XSSObject child = find(name);
    if (child)
      {
        result.flags = 0;
        result.get   = Getter( new const_getter(child) );
        return true;
      }

    variant value;
    bool    read_only = false;
    if (type_ && !type_->is_injected(name))
      {
        if (type_->has(name) || type_->find(name))
          {
            value = dynamic_get(type_, name);
            read_only = true;
          }
      }

		//sponge-like, items will be marked as "injected", which is synonym for
    //properties that were added at run time. They are not inheritable
		int idx      = add_anonymous(value);
		result.get   = Getter( new anonymous_getter(idx) );
		result.set   = !read_only? Setter( new anonymous_setter(idx) ) : Setter();
		result.flags = DYNAMIC_ACCESS|INJECTED;

    items_.insert( item_pair(name, result) );
		return true;
	}

str xss_object::id()
  {
    return id_;
  }

str xss_object::output_id()
  {
    return output_id_.empty()? id_ : output_id_;
  }

str xss_object::type_name()
  {
    return type_name_;
  }

XSSType	xss_object::type()
  {
    return type_;
  }

XSSObject	xss_object::parent()
  {
    return parent_.lock();
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

void xss_object::set_id(const str& id)
  {
    id_ = id;
  }

void xss_object::set_output_id(const str& id)
  {
    output_id_ = id;
  }

void xss_object::set_type_name(const str& id)
  {
    assert(type_name_.empty());
    assert(!type_ || type_->id() == id);

    type_name_ = id;
  }

void xss_object::set_parent(XSSObject parent)
  {
    //td: !!! assert(!parent_);
    parent_ = parent;
  }

void xss_object::set_type(XSSType type)
  {
    bool typeless = !type_;
    type_         = type;

    if (typeless && type)
      {
        //shit, binding... long story short there is lag 
        //between obtaining a type and being loaded.
        //So some properties values may have been set
        //this code accounts for that
        item_list::iterator it = items_.begin();
        item_list::iterator nd = items_.end();
        for(; it != nd; it++)
          {
            str         prop_name = it->first;
            XSSProperty prop      = get_property(prop_name);

            if (prop)
              {
                Getter get = it->second.get;
                if (!get)
                  continue; // ?

                variant value = get->get(this);
                add_property(prop_name, value, prop->type());
              }
          }
      }
  }

XSSObject	xss_object::idiom()
  {
    return idiom_;
  }

void xss_object::set_idiom(XSSObject id)
  {
    idiom_ = id;
  }

void xss_object::add_event_impl(XSSEvent ev, XSSCode code)
  {
    assert(false); //td:
  }

bool xss_object::context_resolve(const str& id, resolve_info& info)
  {
    if (info.what == RESOLVE_ANY || info.what == RESOLVE_PROPERTY)
      {
        XSSProperty prop = get_property(id);
        if (prop)
          {
            info.what = RESOLVE_PROPERTY;
            info.value = prop;
            info.type  = prop->property_type();
            return true;
          }
      }
      
    if (info.what == RESOLVE_ANY || info.what == RESOLVE_METHOD)
      {
        XSSMethod mthd = get_method(id);
        if (mthd)
          {
            info.what = RESOLVE_METHOD;
            info.value = mthd;
            info.type  = mthd->return_type();
            return true;
          }
      }

    if (info.what == RESOLVE_ANY || info.what == RESOLVE_EVENT)
      {
        XSSEvent ev = get_event(id);
        if (ev)
          {
            info.what = RESOLVE_EVENT;
            info.value = ev;
            return true;
          }
      }

    return false;
  }

void xss_object::context_visit(context_visitor* visitor)
  {
    std::vector<variant>::iterator it = properties_->ref_begin();
    std::vector<variant>::iterator nd = properties_->ref_end();

    for(; it != nd; it++)
      {
        //td: !!! get rid of Dynamic Array
        XSSProperty prop = *it;
        visitor->visit(RESOLVE_PROPERTY, prop->id(), prop);
      }
      
    it = methods_->ref_begin();
    nd = methods_->ref_end();
    for(; it != nd; it++)
      {
        //td: !!! get rid of Dynamic Array
        XSSMethod mthd = *it;
        visitor->visit(RESOLVE_METHOD, mthd->id(), mthd);
      }

    it = events_->ref_begin();
    nd = events_->ref_end();
    for(; it != nd; it++)
      {
        //td: !!! get rid of Dynamic Array
        XSSEvent ev = *it;
        visitor->visit(RESOLVE_EVENT, ev->id(), ev);
      }

    if (type_)
      type_->context_visit(visitor);
  }
//struct xss_object::query_info
void xss_object::query_info::add_property(XSSProperty prop)
  {
    if (has(prop))
      return;

    register_property(prop);
    result->insert(prop);
  }

void xss_object::query_info::register_property(XSSProperty prop)
  {
    if (has(prop))
      return;

    found.insert(std::pair<str, XSSProperty>(prop->id(), prop));
  }

bool xss_object::query_info::has(XSSProperty prop)
  {
    return found.find(prop->id()) != found.end();
  }

//continue xss_object
void xss_object::query_properties_impl(query_info &info)
  {
    fs::path file;
    code_context cctx;

    std::vector<variant>::iterator it = properties_->ref_begin();
    std::vector<variant>::iterator nd = properties_->ref_end();
    for(; it != nd; it++)
      {
        XSSProperty prop = *it;

        if (info.has(prop))
          continue;

        bool addp = true;
        if (info.expression != "*")
          {
            xs_utils xs;
            cctx.this_ = prop;
            variant result = xs.evaluate_xs_expression(info.expression, cctx, file);
            addp = variant_cast<bool>(result, false);
          }

        if (addp)
          {
            info.add_property(prop);
          }
        else
          {
            info.register_property(prop);
          }
      }

    if (type_)
      {
        type_->query_properties_impl(info);
      }
  }

DynamicArray xss_object::query_properties(const str& query)
  {
    query_info info(query);

    query_properties_impl(info);

    return info.result;
  }

bool xss_object::has_property(const str& prop)
  {
    return get_property(prop);
  }

bool xss_object::have_value(const str& prop)
  {
    std::vector<variant>::iterator it = properties_->ref_begin();
    std::vector<variant>::iterator nd = properties_->ref_end();

    for(; it != nd; it++)
      {
        XSSProperty xprop = *it;
        if (xprop->id() == prop)
          {
            return !xprop->value_.empty();
          }
      }

    return false;
  }

XSSObject xss_object::find(const str& what)
  {
    if (what.empty())
      return XSSObject();

    std::vector<variant>::iterator it = children_->ref_begin();
    std::vector<variant>::iterator nd = children_->ref_end();

    for(; it != nd; it++)
      {
        XSSObject child = *it;
        if (child->id() == what || child->type_name() == what)
          return child;
      }

    return XSSObject();
  }

XSSObject xss_object::find_by_id(const str& what)
  {
    if (what.empty())
      return XSSObject();

    std::vector<variant>::iterator it = children_->ref_begin();
    std::vector<variant>::iterator nd = children_->ref_end();

    for(; it != nd; it++)
      {
        XSSObject child = *it;
        if (child->id() == what)
          return child;
      }

    return XSSObject();
  }

std::vector<XSSObject> xss_object::find_by_class(const str& which)
  {
    std::vector<XSSObject> result;

    std::vector<variant>::iterator it = children_->ref_begin();
    std::vector<variant>::iterator nd = children_->ref_end();

    for(; it != nd; it++)
      {
        XSSObject child = *it;
        if (child->type_name() == which)
          result.push_back(child);
      }

    return result;
  }

DynamicArray xss_object::find_by_type(const str& which)
  {
    DynamicArray result(new dynamic_array);
    std::vector<variant>::iterator it = children_->ref_begin();
    std::vector<variant>::iterator nd = children_->ref_end();

    for(; it != nd; it++)
      {
        XSSObject child = *it;
        if (child->type_name() == which)
          result->push_back(child);
      }

    return result;
  }

DynamicArray xss_object::get_event_impl(const str& event_name, XSSEvent& ev)
  {
    for(size_t i = 0; i < events_->size(); i++)
      {
        ev = events_->at(i);
        if (ev->id() == event_name)
          return ev->impls;
      }

    //not implemented, create
    ev = XSSEvent(new xss_event(event_name, variant()));
    XSSEvent type_ev = type_? type_->get_event(event_name) : XSSEvent();
    if (type_ev)
      {
        ev->copy(XSSObject(type_ev));
      }

    ev->set_id(event_name);

    events_->push_back(ev);
    return ev->impls;
  }

DynamicArray xss_object::get_event_code(const str& event_name)
  {
    for(size_t i = 0; i < events_->size(); i++)
      {
        XSSEvent ev = events_->at(i);
        if (ev->id() == event_name)
          return ev->impls;
      }
    
    return DynamicArray(new dynamic_array);
  }

variant xss_object::get_event_args(const str& event_name)
  {
    for(size_t i = 0; i < events_->size(); i++)
      {
        XSSEvent ev = events_->at(i);
        if (ev->id() == event_name)
          return ev->args;
      }
    
    return variant();
  }
DynamicArray xss_object::get_attributes()
  {
    DynamicArray result(new dynamic_array);

    struct attr_visitor : dynamic_visitor
      {
        attr_visitor(DynamicArray result):
          result_(result)
          {
          }

        virtual void item(const str& name, variant value)
          {
            XSSObject result(new xss_object);
            result->add_attribute("name", name); //td: types
            result->add_attribute("value", value); 

            result_->insert(result);

          }

        DynamicArray result_;
      };

    attr_visitor av(result);
    this->visit(&av);

    return result;
  }

bool xss_object::is_injected(const str& name)
  {
    item_list::iterator it = items_.find(name);
    if (it != items_.end())
      {
        return it->second.flags&INJECTED;
      }

    return false;
  }

void xss_object::add_method(const str& name, XSSMethod m)
  {
    methods_->push_back(m); //td: check for it not existing
  }

bool xss_object::empty()
  {
    return properties_->empty() && methods_->empty() && events_->empty();
  }

void xss_object::propertize()
  {
    //find values that correspond to our properties, update values
    item_list::iterator iit = items_.begin();
    item_list::iterator ind = items_.end();

    for(; iit != ind; iit++)
      {
        XSSProperty myprop = get_shallow_property(iit->first);
        XSSProperty typeprop = type_? type_->get_property(iit->first) : XSSProperty();
            
        variant value;
        if (iit->second.get)
            value = iit->second.get->get(this);

        if (myprop)
          myprop->value_ = value;
        else if (typeprop)
          {
            myprop = XSSProperty(new xss_property);
            myprop->copy(XSSObject(typeprop));
            myprop->value_ = value;
            add_property_(myprop);
          }
      }
  }

void xss_object::insert_method(XSSMethod mthd)
  {
    //td: !!! check dups
    methods_->push_back(mthd);
  }

void xss_object::insert_event(XSSEvent ev)
  {
    //td: !!! check dups
    events_->push_back(ev);
  }

void xss_object::insert_property(XSSProperty prop)
  {
    //td: !!! check dups
    properties_->push_back(prop);
  }

void xss_object::add_child(XSSObject obj)
  {
    obj->set_parent(shared_from_this());
    children_->push_back(obj);
  }

void xss_object::remove_child(XSSObject obj)
  {
    assert(false); //hate removing
  }

void xss_object::fixup_children(XSSContext ctx)
  {
    std::vector<variant>::iterator it = children_->ref_begin();
    std::vector<variant>::iterator nd = children_->ref_end();

    size_t idx = 0;
    std::vector<size_t> to_remove;
    for(; it != nd; it++, idx++)
      {
        XSSObject child = *it;

        XSSProperty myprop = get_shallow_property(child->type_name());
        XSSProperty typeprop = type_? type_->get_property(child->type_name()) : XSSProperty();

        bool found = true;
        if (myprop)
          myprop->value_ = child;
        else if (typeprop)
          {
            myprop = XSSProperty(new xss_property);
            myprop->copy(XSSObject(typeprop));
            myprop->value_ = child;
            add_property_(myprop);
          }
        else 
          {
            found = false;
            XSSType   child_type = child->type();
            if (!child_type)
              {
                child_type = ctx->get_type(child->type_name());
                child->set_type(child_type);
                child->propertize();
              }
          }

        if (found)
          {
            child->set_type(myprop->type());
            child->propertize();

            to_remove.push_back(idx);
          }

        child->fixup_children(ctx);
      }

    std::vector<size_t>::reverse_iterator rit = to_remove.rbegin();
    std::vector<size_t>::reverse_iterator rnd = to_remove.rend();
    for(; rit != rnd; rit++)
      children_->remove(*rit);
  }

XSSProperty xss_object::get_property(const str& name)
  {
    std::vector<variant>::iterator it = properties_->ref_begin();
    std::vector<variant>::iterator nd = properties_->ref_end();

    for(; it != nd; it++)
      {
        XSSProperty prop = *it;
        if (prop->id() == name)
          return prop;
      }

    if (type_)
      return type_->get_property(name);

    return XSSProperty();
  }

XSSProperty xss_object::get_shallow_property(const str& name)
  {
    std::vector<variant>::iterator it = properties_->ref_begin();
    std::vector<variant>::iterator nd = properties_->ref_end();

    for(; it != nd; it++)
      {
        XSSProperty prop = *it;
        if (prop->id() == name)
          return prop;
      }

    return XSSProperty();
  }

XSSEvent xss_object::get_event(const str& name)
  {
    std::vector<variant>::iterator it = events_->ref_begin();
    std::vector<variant>::iterator nd = events_->ref_end();

    for(; it != nd; it++)
      {
        XSSEvent ev = *it;
        if (ev->id() == name)
          return ev;
      }

    if (type_)
      return type_->get_event(name);

    return XSSEvent();
  }

XSSMethod xss_object::get_method(const str& name)
  {
    std::vector<variant>::iterator it = methods_->ref_begin();
    std::vector<variant>::iterator nd = methods_->ref_end();

    for(; it != nd; it++)
      {
        XSSMethod mthd = *it;
        if (mthd->id() == name)
          return mthd;
      }

    if (type_)
      return type_->get_method(name);

    return XSSMethod();
  }

variant xss_object::attribute_value(const str& name)
  {
    return dynamic_get(this, name);
  }

void xss_object::bind(XSSContext ctx)
  {
    std::vector<variant>::iterator it = children_->ref_begin();
    std::vector<variant>::iterator nd = children_->ref_end();
    for(; it != nd; it++)
      {
        XSSObject child = *it;
        child->bind(XSSContext(new xss_context(CTXID_INSTANCE, child, ctx)));
      }

    //td: !!! property multimap
    it = properties_->ref_begin();
    nd = properties_->ref_end();
    for(; it != nd; it++)
      {
        XSSProperty prop = *it;
        prop->bind(ctx); //note we're not creating a new context for properties or methods, they'll use the instance context
                         //dont see a case 
      }

    it = methods_->ref_begin();
    nd = methods_->ref_end();
    for(; it != nd; it++)
      {
        XSSMethod mthd = *it;
        mthd->bind(ctx);
      }

    it = events_->ref_begin();
    nd = events_->ref_end();
    for(; it != nd; it++)
      {
        XSSEvent ev = *it;
        ev->bind(ctx);
      }

    //td: !!! event implementation
  }

void xss_object::add_property_(XSSProperty prop)
  {
    std::vector<variant>::iterator it = properties_->ref_begin();
    std::vector<variant>::iterator nd = properties_->ref_end();

    XSSProperty mine;
    for(; it != nd; it++)
      {
        XSSProperty p = *it;
        if (prop->id() == p->id())
          {
            mine = p;
            break;
          }
      }

    if (mine)
      {
        mine->set_value(prop->value_, prop->type());
      }
    else
      {
        //search the type
        mine = get_property(prop->id());
        if (mine)
          {
            XSSProperty mashup(new xss_property);
            mashup->copy(XSSObject(mine));
            mashup->copy(XSSObject(prop));
            mashup->this_ = shared_from_this();
            properties_->push_back(mashup);
          }
        else
          {
            properties_->push_back(prop);
          }
      }
  }

XSSProperty xss_object::add_property(const str& name, variant value, XSSType type)
  {
    XSSProperty result;
    XSSProperty old_prop = get_property(name);
    if (old_prop)
      {
        //we have the property, the logic is:
        //if we (this) have already a value for it then that value will be replaced
        //otherwise (when its inherited) a new property will be created wilth the incoming value
        std::vector<variant>::iterator it = properties_->ref_begin();
        std::vector<variant>::iterator nd = properties_->ref_end();

        for(; it != nd; it++)
          {
            XSSProperty prop = *it;
            if (prop->id() == name)
              {
                prop->set_value(value, type);
                return prop;
              }
          }

        //otherwise
        result = XSSProperty(new xss_property());
        result->copy(XSSObject(old_prop));
        result->this_ = shared_from_this();
        result->set_value(value, type);
      }

    if (!result)
      result = XSSProperty(new xss_property(name, type, value, shared_from_this()));

    //do 'this' of xss_object the parent of property
    result->set_parent(XSSObject(this));

    properties_->push_back(result);
    return result;
  }

void xss_object::register_property(const str& name, XSSProperty new_prop)
  {
    XSSProperty prop = get_property(name);
    if (!prop)
      {
        properties_->push_back(new_prop);
      }
  }

void xss_object::register_method(const str &name, XSSMethod new_mthd)
  {
    XSSMethod mthd = get_method(name);
    if (!mthd)
      {
        methods_->push_back(new_mthd);
      }
  }

void xss_object::register_event_impl(const str &name, XSSEvent new_evt)
  {
    assert(false); //td:
  }

//xss_type
xss_type::xss_type():
  xss_object(),
  xs_type_(null),
  is_enum_(false),
  is_array_(false),
  is_object_(true),
  is_variant_(false),
  is_unresolved_(false),
  ctor_args_(new dynamic_array),
  all_instances_(new dynamic_array),
  local_instances_(new dynamic_array),
  foreign_instances_(new dynamic_array)
  {
    DYNAMIC_INHERITANCE(xss_type)
  }

xss_type::xss_type(schema* _xs_type):
  xss_object(),
  xs_type_(_xs_type),
  is_enum_(false),
  is_array_(false),
  is_object_(false),
  is_variant_(false),
  is_unresolved_(false),
  ctor_args_(new dynamic_array),
  all_instances_(new dynamic_array),
  local_instances_(new dynamic_array),
  foreign_instances_(new dynamic_array)
  {
    DYNAMIC_INHERITANCE(xss_type)
  }

xss_type::xss_type(const str& id, schema* _xs_type):
  xss_object(),
  xs_type_(_xs_type),
  is_enum_(false),
  is_array_(false),
  is_object_(false),
  is_variant_(false),
  is_unresolved_(false),
  ctor_args_(new dynamic_array),
  all_instances_(new dynamic_array),
  local_instances_(new dynamic_array),
  foreign_instances_(new dynamic_array)
  {
    id_ = id;
    DYNAMIC_INHERITANCE(xss_type)
  }

bool xss_type::resolve(const str& name, schema_item& result)
  {
    if (is_enum_ && !has(name))
      {
        XSSProperty enum_item = get_property(name);
        if (enum_item)
          {
		        int idx      = add_anonymous(enum_item);
		        result.get   = Getter( new anonymous_getter(idx) );
		        result.set   = Setter();
		        result.flags = DYNAMIC_ACCESS;

            items_.insert( item_pair(name, result) );
		        return true;
          }
      }

    return xss_object::resolve(name, result);
  }

XSSOperator xss_type::get_operator(operator_type opid, XSSArguments args)
  {
    std::vector<XSSOperator>::iterator it = operators_.begin();
    std::vector<XSSOperator>::iterator nd = operators_.end();

    for(; it != nd; it++)
      {
        XSSOperator op = *it;
        if (op->opid() != opid)
          continue;

        if (!op->match(args))
          continue;

        return op;
      }

    return XSSOperator();
  }

void xss_type::set_super(XSSType super)
  {
    super_ = super;
    type_  = super;

    if (super_)
      {
        type_name_ = super_->id();

        //copy construction parameters
        DynamicArray super_args = super_->ctor_args();

        if (super_args->size() > 0)
          {
            DynamicArray copy = ctor_args_;
            ctor_args_  = DynamicArray(new dynamic_array);

            std::vector<variant>::iterator it = super_args->ref_begin();
            std::vector<variant>::iterator nd = super_args->ref_end();
            for(; it != nd; it++)
              {
                XSSObject robj = variant_cast<XSSObject>(*it, XSSObject());
                if (!robj)
                  continue;
            
                ctor_args_->push_back(robj);
              }

            it = copy->ref_begin();
            nd = copy->ref_end();
            for(; it != nd; it++)
              {
                XSSObject robj = variant_cast<XSSObject>(*it, XSSObject());
                if (!robj)
                  continue;
            
                ctor_args_->push_back(robj);
              }
          }
      }
  }

void xss_type::set_definition(XSSObject def)
  {
    if (def)
      {
        XSSObjectList deps = def->find_by_class("dependency");

        XSSObjectList::iterator it = deps.begin();
        XSSObjectList::iterator nd = deps.end();

        for(; it != nd; it++)
          {
            dependencies_.push_back(*it);
          }

        copy(def);
      }
  }

schema* xss_type::native_type()
  {
    return xs_type_;
  }

XSSType xss_type::array_type()
  {
    return array_type_;
  }

void xss_type::inherit()
  {
    if (!super_)
      return;

    copier cp(XSSObject(super_), this);
    super_->visit(&cp);
  }

XSSType xss_type::get_super()
  {
    return super_;
  }

DynamicArray xss_type::ctor_args()
  {
    return ctor_args_;
  }

void xss_type::register_instance(XSSObject obj)
  {
    //td: big mess differentiating between reader object and types
    //particularly their children
    obj->set_parent(shared_from_this());

    all_instances_->push_back(obj);
    local_instances_->push_back(obj);
  }

void xss_type::register_foreign_instance(XSSObject obj)
  {
	//td: !!! 0.9.5
    //XSSObject id = obj->idiom();
    //if (id)
    //  { 
    //    //td: crap
    //    xss_module* idiom = dynamic_cast<xss_module*>(id.get());
    //    if (idiom)
    //      idiom->used();
    //  }

    //obj->set_parent(shared_from_this());

    //all_instances_->push_back(obj);
    //foreign_instances_->push_back(obj);
  }

XSSObjectList xss_type::get_dependencies()
  {
    return dependencies_;
  }

void xss_type::add_constructor(XSSSignature ctor)
  {
    constructors_.push_back(ctor); 
  }

void xss_type::import(XSSType import)
  {
    assert(false); //td:
  }

void xss_type::as_enum()
  {
    is_enum_ = true;
  }

void xss_type::as_array(XSSType type)
  {
    is_object_ = false;
    is_array_  = true;

    array_type_ = type;
  }

void xss_type::as_variant()
  {
    is_object_ = false;
    is_variant_ = true;
  }

void xss_type::as_object()
  {
    is_object_ = true;
  }

void xss_type::as_unresolved()
  {
    is_unresolved_ = true;
  }

bool xss_type::is_enum()
  {
    return is_enum_;
  }

bool xss_type::is_array()
  {
    return is_array_;
  }

bool xss_type::is_object()
  {
    return is_object_;
  }

bool xss_type::is_native()
  {
    return xs_type_ != null;
  }

bool xss_type::is_variant()
  {
    return is_variant_;
  }

bool xss_type::is_unresolved()
  {
    return is_unresolved_;
  }

Language xss_type::get_language()
  {
    return lang_;
  }

void xss_type::set_language(Language lang)
  {
    lang_ = lang;
  }

//inline_renderer
inline_renderer::inline_renderer(str text, bool global)
  {
    //td: rendering
  }

//xss_property
xss_property::xss_property():
	flags(0)
  {
    DYNAMIC_INHERITANCE(xss_property)
  }

xss_property::xss_property(const xss_property& other):
  xss_object(other),
	get_(other.get_),
	set_(other.set_),
	flags(other.flags),
	this_(other.this_),
	value_(other.value_)
  {
    DYNAMIC_INHERITANCE(xss_property)
	  id_ = other.id_;
	  type_ = other.type_;
  }

xss_property::xss_property(const str& _name, XSSType _type, variant _value, XSSObject _this_):
	this_(_this_),
	value_(_value)
  {
    DYNAMIC_INHERITANCE(xss_property)
	  id_ = _name;
	  type_ = _type;
  }

xss_property::xss_property(const str& _name, XSSType _type, variant _value, variant _get, variant _set, XSSObject _this_):
	get_(_get),
	set_(_set),
	this_(_this_),
	value_(_value)
  {
    DYNAMIC_INHERITANCE(xss_property)
	  id_ = _name;
	  type_ = _type;
  }

str xss_property::get_name()
  {
    return id_;
  }

void xss_property::copy(XSSObject obj)
  {
    xss_property* prop = dynamic_cast<xss_property*>(obj.get());
    //XSSProperty prop = variant_cast<XSSProperty>(obj, XSSProperty());
    if (prop)
      {
        get_ = prop->get_;
			  set_ = prop->set_;
			  flags = prop->flags;
			  this_ = prop->this_;
			  value_ = prop->value_;

        XSSObject get = obj->find("get");
        XSSObject set = obj->find("set");

        if (get) 
          {
            XSSObject cpy(new xss_object);
            cpy->copy(get);
            add_child(cpy);
          }
        
        if (set) 
          {
            XSSObject cpy(new xss_object);
            cpy->copy(set);
            add_child(cpy);
          }
      }

    xss_object::copy(obj);
  }

XSSObject xss_property::get_get()
  {
    return find("get");
  }

XSSObject xss_property::get_set()
  {
    return find("set");
  }

void xss_property::set_value(const variant value, XSSType type)
  {
    value_ = value; //td: type checking
    type_  = type;
  }

str xss_property::render_value()
  {
    if (value_.empty())
      return "null"; //td: somehow the language must resolve this

    IRenderer* renderer = variant_cast<IRenderer*>(value_, null);
    if (renderer)
      {
        str result = renderer->render();
        return result;
      }

    if (value_.is<str>())
      {
        str result = variant_cast<str>(value_, str());
        return '"' + result + '"';
      }

    XSSType prop_type = type();
    if (prop_type)
      return prop_type->get_language()->render_value(prop_type, value_);

    return xss_utils::var_to_string(value_);
  }

variant xss_property::eval(XSSContext ctx)
  {
    IExpressionRenderer *ier = variant_cast<IExpressionRenderer *>(value_, null);

    if (ier)
      {
        return ier->eval(ctx);
      }

    return variant();
  }

void xss_property::expr_value(XSSExpression value)
  {
    expr_value_ = value;
  }

void xss_property::set_getter(InlineRenderer getter)
  {
    getter_ = getter;
  }

void xss_property::set_setter(InlineRenderer setter)
  {
    setter_ = setter;
  }

void xss_property::code_getter(XSSCode getter)
  {
    code_getter_ = getter;
  }

void xss_property::code_setter(XSSCode setter)
  {
    code_setter_ = setter;
  }

void xss_property::as_const()
  {
    assert(false); //td:
  }

void xss_property::property_type(XSSType type)
  {
    prop_type_ = type;
  }

XSSType xss_property::property_type()
  {
    return prop_type_;
  }

void xss_property::bind(XSSContext ctx)
  {
    xss_object::bind(ctx);
    if (code_getter_)
      {
        code_getter_->bind(ctx);
        XSSType rt = code_getter_->return_type();
        if (!prop_type_)
          {
            prop_type_ = rt;
            if (!prop_type_)
              {
		            ctx->error(SGetterNotReturning, null, code_getter_->begin(), code_getter_->end());
                prop_type_ = ctx->get_type("var");
              }
          }
        else if (prop_type_ != rt)
		      ctx->error(SGetterTypeMismatch, null, code_getter_->begin(), code_getter_->end());
      }

    if (!prop_type_)
      prop_type_ = ctx->get_type("var");

    if (code_setter_)
      {
        XSSContext sctx = code_setter_->context();
        sctx->register_symbol(RESOLVE_VARIABLE, "value", prop_type_);
        
        code_setter_->bind(ctx);
      }
  }

XSSType xss_property::type()
  {
    //if (!type_)
    //  {
    //    ICodeRenderer* icr = variant_cast<ICodeRenderer *>(get_, null);

    //    if (icr)
    //      {
    //        type_ = icr->type();
    //      }
    //    else
    //      {
    //        IExpressionRenderer *ier = variant_cast<IExpressionRenderer *>(value_, null);

    //        if (ier)
    //          {
    //            type_ = ier->type();
    //          }
    //      }
    //  }

    //return type_;
    assert(false); //whats all this crap again?
    return XSSType();
  }

//xss_event
xss_event::xss_event():
  impls(new dynamic_array)
  {
    DYNAMIC_INHERITANCE(xss_event)
  }

xss_event::xss_event(const xss_event& other):
  impls(other.impls),
  args(other.args)
  {
    DYNAMIC_INHERITANCE(xss_event)
    id_ = other.id_;
  }

xss_event::xss_event(const str& _name, variant args):
  impls(new dynamic_array)
  {
    DYNAMIC_INHERITANCE(xss_event)
    id_ = _name;
  }

str xss_event::get_name()
  {
    return id_;
  }

bool xss_event::implemented()
	{
		return impls->size() > 0;
	}

void xss_event::set_dispatcher(InlineRenderer dispatcher)
  {
    dispatcher_ = dispatcher;
  }

void xss_event::set_signature(XSSSignature sig)
  {
    signature_ = sig;
  }

XSSSignature xss_event::signature()
  {
    return signature_;
  }

void xss_event::bind(XSSContext ctx)
  {
    xss_object::bind(ctx);
    signature_->bind(ctx);

    //td: !!! implemented events
  }

//xss_method
xss_method::xss_method()
  {
    DYNAMIC_INHERITANCE(xss_method)
  }

xss_method::xss_method(const xss_method& other):
	args_(other.args_),
  code_(other.code_)
  {
    DYNAMIC_INHERITANCE(xss_method)
	  type_ = other.type_;
    id_ = other.id_;
  }

xss_method::xss_method(const str& name, XSSType type, variant args, variant code):
	args_(args),
  code_(code)
  {
    DYNAMIC_INHERITANCE(xss_method)
	  type_ = type;
    id_ = name;
  }

str xss_method::get_name()
  {
    return id_;
  }

XSSType xss_method::type()
  {
    if (!type_)
      {
        ICodeRenderer* icr = variant_cast<ICodeRenderer *>(code_, null);

        if (icr)
          {
            type_ = icr->type();
          }
      }

    return type_;
  }

void xss_method::return_type(XSSType type)
  {
    return_type_ = type;
  }

XSSType xss_method::return_type()
  {
    return return_type_;
  }

void xss_method::set_caller(InlineRenderer caller)
  {
    caller_ = caller;
  }

void xss_method::set_signature(XSSSignature sig)
  {
    signature_ = sig;
  }

void xss_method::set_code(XSSCode code)
  {
    code__ = code;
  }

void xss_method::bind(XSSContext ctx)
  {
    xss_object::bind(ctx);

    if (signature_)
      signature_->bind(ctx);
    else
      signature_ = XSSSignature(new xss_signature);

    if (code__)
      {
        code__->bind(ctx);
        XSSType rt = code__->return_type();
        if (!return_type_)
            return_type_ = rt;
        else if (return_type_ != rt)
		      ctx->error(SMethodReturnTypeMismatch, null, code__->begin(), code__->end());
      }
  }

variant xss_method::code()
  {
    return code_;
  }

void xss_method::add_parameter(const str& name)
  {
    IArgumentRenderer* args = variant_cast<IArgumentRenderer*>(args_, null);
    if (args)
      args->add(name, XSSType()); //td: type
  }

variant& xss_method::get_parameters()
  {
    return args_;
  }
