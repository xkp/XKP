
#include <serial.h>

#include <xs/array.h> //td: array doesn't need to be inside of xs

using namespace xkp;

//utils
struct mutable_reader : reader_visitor
  {
    mutable_reader(IDynamicObject* obj, base_read_archive& archive) : obj_(obj), archive_(archive) {}

    virtual void attribute(const str& name, const variant value)
      {
        schema_item itm;
        if (!obj_->resolve(name, itm))
          assert(false); //ought to be mutable

				//this tries to fix an importand design flaw of the mutant/xml reader... fault to deduce arrays
				variant val = value;
				if (itm.type)
					{
						int options = itm.type->options();
						if (options & TYPE_ITERATED)
							{
								schema* vtype = value.get_schema();
								if (!vtype || !(vtype->options() & TYPE_ITERATED))
									{
										return; //just bail if whats being assigned to an array is not
									}
							}
					}

        if (!itm.set)
					{
						assert(false); //same
					}

        itm.set->set(obj_, value);
      }

    virtual void object(const str& name, Reader reader)
      {
        variant result;
        archive_.read(reader, type_schema<variant>(), result);

        attribute(name, result);
      }

    virtual void iterator(const str& name, ReadIterator reader)
      {
        DynamicArray da( new dynamic_array );
        archive_.read_iterable(reader, da, null);

        attribute(name, da);
      }

    private:
      IDynamicObject*     obj_;
      base_read_archive&  archive_;
  };


//base_archive
bool base_archive::attributed_type(schema* type)
  {
    return (type == type_schema<int>())    ||
           (type == type_schema<long>())   ||
           (type == type_schema<float>())  ||
           (type == type_schema<double>()) ||
           (type == type_schema<bool>())   ||
           (type == type_schema<str>());
  }


//base_write_archive
void base_write_archive::save(const variant& what)
  {
    Writer w = create_root(); assert( w );

    variant v = what;
    write(w, "", v);
  }

void base_write_archive::write( Writer w, const str& name, variant& v)
  {
    schema* type  = true_type(v);
    void*   this_ = v.get_pointer();

    //3 use cases:
    //- we know the type as attibuted (ints, strings, things like that)
    //  note the implementor is free to define its custom types
    //
    //- we are saving an iterable type
    //
    //- or we are saving an object, these seem to be reasonable assumptions.
    if (attributed_type(type))
      {
        assert(!name.empty());
        w->attribute(name, v);
      }
    else
      {
        size_t type_options = type->options();
        if (type_options & TYPE_ITERATED)
          {
            iterable iter(v);
            iterator it = iter.begin();
            iterator nd = iter.end();

            WriteIterator wit = w->create_iterator(name, iter.iterated_type());

            for(; it != nd; ++it)
              {
                variant iv = *it;
                Writer  iw = wit->next(iv);
                if (iw)
                  write(iw, "", iv);
              }
          }
        else
          {
            Writer writer_ = name.empty()? w : w->create_node(name);

            schema* otype = v.get_schema(); assert(otype);
            size_t  oopt  = otype->options();
            if (oopt & TYPE_NON_INSTANTIABLE)
              {
                assert(type != otype);
                assert(types_);

                str class_id = types_->type_name(type); assert( !class_id.empty() );
                w->attribute("class", class_id);
              }

            //collect what we're saving
            schema_collector sc;
            type->visit(&sc);

            schema_collector::container::iterator it = sc.begin();
            schema_collector::container::iterator nd = sc.end();
            for(; it != nd; it++)
              {
                schema_item& item = it->second;
                if (item.flags & CONST)
                  continue;

                if (item.flags & TRANSIENT)
                  continue;

                if (item.get)
                  {
                    variant value = item.get->get(this_);
                    write(writer_, it->first, value);
                  }
              }
          }
      }
  }

//base_read_archive
variant base_read_archive::get(schema* type)
  {
    Reader  root = create_root();
    variant result;
    read(root, type, result);

    return result;
  }

variant base_read_archive::query(const str& name)
  {
    assert(false); //td:
    return variant();
  }

bool base_read_archive::read(Reader r, schema* type, variant& result)
  {
    if (!type || type->options() & TYPE_NON_INSTANTIABLE || type == type_schema<variant>())
      {
        assert(types_);

        variant v;
        if (!r->attribute("class", type_schema<str>(), v))
          {
            assert(false); //??
            return false;
          }

        str class_id  = v;
        type          = types_->get_type(class_id); assert(type);
      }

    assert(type);

    //instantiate
    if (!type->create(result))
      {
        return false;
      }

    void* this_ = result.get_pointer();

    size_t type_options = type->options();
    if (type_options & TYPE_ITERATED)
      {
        iterable     iter(result);
        schema*      iter_type = iter.iterated_type();

        ReadIterator rit = r->create_iterator("", iter_type);

        Reader  item_reader;
        variant item;
        while(rit->next(item_reader, item))
          {
            variant result_item;

            if (item_reader)
              read(item_reader, iter_type, result_item);
            else
              result_item = item;

            iter.insert( result_item );
          }
      }
    else
      {
        schema_collector sc;
        type->visit(&sc);

        schema_collector::container::iterator it = sc.begin();
        schema_collector::container::iterator nd = sc.end();
        for(; it != nd; it++)
          {
            schema_item& item = it->second;
            if (item.flags & CONST)
              continue;

            if (item.flags & TRANSIENT)
              continue;

            if (item.set)
              {
                variant value;
                if (attributed_type(item.type))
                  {
                    if (r->attribute(it->first, item.type, value))
                      {
                        item.set->set(this_, value);
                      }
                  }
                else
                  {
                    Reader rr = r->create_node(it->first);
                    if (rr)
                      {
                        read(rr, item.type, value);
                        item.set->set(this_, value);
                      }
                  }
              }
            else if (item.flags & STATIC_FIELD)
              {
                assert(item.get);
                variant sf = item.get->get(this_);

                if (item.type->options() & TYPE_ITERATED)
                  {
                    read_iterated(r, it->first, sf);
                  }
                else
                  {
                    read_object(r, it->first, sf, sf.get_schema());
                  }
              }
          }

				if (type_options & TYPE_MUTABLE)
					{
						IDynamicObject* obj = result;
						mutable_reader mr(obj, *this);
						r->visit(&mr);

						obj = null;
					}
			}

    return true;
  }

void base_read_archive::read_iterated(Reader r, const str& name, variant& iterated)
  {
    iterable     iter(iterated);
    schema*      iter_type = iter.iterated_type();

    ReadIterator rit = r->create_iterator(name, iter_type);
    if (!rit)
      return;

    Reader  item_reader;
    variant item;
    while(rit->next(item_reader, item))
      {
        variant result_item;

        if (item_reader)
          read(item_reader, iter_type, result_item);
        else
          result_item = item;

        iter.insert( result_item );
      }
  }

void base_read_archive::read_object(Reader r, const str& name, variant& obj, schema* type)
  {
    //collect what we're saving
    schema_collector sc;
    type->visit(&sc);

    void* this_ = obj.get_pointer();

    schema_collector::container::iterator it = sc.begin();
    schema_collector::container::iterator nd = sc.end();
    for(; it != nd; it++)
      {
        schema_item& item = it->second;
        if (item.flags & CONST)
          continue;

        if (item.set)
          {
            variant value;
            if (attributed_type(item.type))
              {
                if (r->attribute(it->first, item.type, value))
                  {
                    item.set->set(this_, value);
                  }
              }
            else
              {
                Reader rr = r->create_node(it->first);
                if (rr)
                  {
                    read(rr, item.type, value);
                    item.set->set(this_, value);
                  }
              }
          }
        else if (item.flags & STATIC_FIELD)
          {
            //td: !!! resuse read_iterated & read_object

            assert(item.get);
            variant sf = item.get->get(this_);

            if (item.type->options() & TYPE_ITERATED)
              {
                read_iterated(r, it->first, sf);
              }
            else
              {
                read_object(r, it->first, sf, sf.get_schema());
              }
          }
      }
  }

void base_read_archive::read_iterable(ReadIterator rit, variant obj, schema* type)
  {
    iterable iter(obj);
    schema*  iter_type = iter.iterated_type();

    Reader  item_reader;
    variant item;
    while(rit->next(item_reader, item))
      {
        variant result_item;

        if (item_reader)
          read(item_reader, iter_type, result_item);
        else
          result_item = item;

        iter.insert( result_item );
      }
  }
