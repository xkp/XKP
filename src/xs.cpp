
#include <xs.h>
#include <xs/ast.h>
#include <xs/compiler.h>
#include <xs/linker.h>
#include <xs/array.h>

using namespace xkp;

//utils
void prepare_dsl(code_context& ctx, code_linker& linker, std::vector<str>& names)
  {
    if (ctx.dsl_)
      {
        dsl_list::iterator it = ctx.dsl_->begin();
        dsl_list::iterator nd = ctx.dsl_->end();
        
        for(; it != nd; it++)
          {
            DslLinker lnk = it->second;
            names.push_back( it->first );
            
            linker.register_dsl(it->first, lnk);
          }
      }
  }

//expressions
variant xs_utils::evaluate_expression(const str& src)
  {
    xs_compiler cmp;
    expression  ctx; 
    
    if (!cmp.compile_expression(src, ctx) )
      return variant(); //td: error handling

    expr_evaluator eval;
    ctx.visit( &eval );   
    return eval.value();
  }

variant xs_utils::evaluate_xs_expression(const str& src, code_context& ctx)
  {
    return execute_xs("return " + src + ';', ctx);
  }

bool xs_utils::compile_expression(const str& src, expression& expr)
  {
    xs_compiler c;
    return c.compile_expression(src, expr);
  }
  
//code_context
ByteCode xs_utils::compile_code(const str& src, code_context& ctx)
  {
    code_linker linker(ctx);

    //import dsls
    std::vector<str> dl;
    prepare_dsl(ctx, linker, dl);

    //do the deed
    xs_compiler compiler(dl);
    
    code code_;
    compiler.compile_code(src, code_);

    code_.visit(&linker);
    
    return linker.link();
  }
  
variant xs_utils::execute_xs(const str& src, code_context& ctx)
  {
    type_registry types;
    if (!ctx.types_)
        ctx.types_ = &types;
    
    meta_array_schema array_type;
    if (!ctx.types_->has("array"))
      {
        ctx.types_->add_type("array", &array_type);
      }

    code_linker linker(ctx);

    //import dsls
    std::vector<str> dl;
    prepare_dsl(ctx, linker, dl);

    //do the deed
    xs_compiler compiler(dl);
    
    code code_;
    if (!compiler.compile_code(src, code_))
      return variant(); //td: !!! error locations, exceptions, deceptions

    code_.visit(&linker);
    
    //and execute such deed
    ByteCode dbc = linker.link();
    execution_context dec(dbc, ctx.this_);
    return dec.execute();
  }

void xs_utils::compile_class(const str& src, code_context& ctx)
  {
    xs_compiler  compiler;
    xs_container results;
    compiler.compile_xs(src, results);
    
    for(size_t i = 0; i < results.size(); i++)
      {
        //assumes there are only classes in there
        //td: this is really ugly
        xs_class ci = results.get(i);
        class_linker cl(ctx); 
        cl.link(ci);
      }
  }

void xs_utils::compile_instance(const str& src, DynamicObject instance, code_context& ctx) 
  {
    xs_compiler  compiler;
    xs_container results;
    compiler.compile_xs(src, results);
    
    for(size_t i = 0; i < results.size(); i++)
      {
        //assumes there are only classes in there
        xs_instance ii = results.get(i);
        instance_linker il(ctx, instance); 
        il.link(ii);
      }
  }

void xs_utils::compile(const str& src, code_context& ctx)
  {
    xs_compiler  compiler;
    xs_container results;
    compiler.compile_xs(src, results);
    
    IEditableObject* output = variant_cast<IEditableObject*>(ctx.this_, null);
    base_xs_linker linker(ctx, output);
    results.visit( &linker );
  }


//basic_scope
void basic_scope::register_symbol(const str& name, variant value)
  {
    symbols_.insert(symbol_pair(name, value));
  }
  
bool basic_scope::resolve(const str& name, variant& result)
  {
    symbol_list::iterator it = symbols_.find(name);
    if (it != symbols_.end())  
      {
        result = it->second;
        return true;
      }
      
    return false;
  }
  
//object_scope
void object_scope::register_symbol(const str& name, variant value)
  {
    assert(false); //td: error, this shouldnt happen
  }

bool object_scope::resolve(const str& name, variant& result)
  {
    assert(obj_);
    schema_item itm;
    if (obj_->resolve(name, itm))
      {
        result = itm;
        return true;
      }
    
    return false;
  }

//composite_scope
void composite_scope::register_symbol(const str& name, variant value)
  {
    assert(false); //td: this might be valid, but I dont see any use case
  }
  
bool composite_scope::resolve(const str& name, variant& result)
  {
    if (s1_->resolve(name, result))
      return true;
    else if (s2_->resolve(name, result))
      return true;

    return false;
  }    

//type_scope
void type_scope::register_symbol(const str& name, variant value)
  {
    assert(false); //td: error, this shouldnt happen
  }
  
bool type_scope::resolve(const str& name, variant& result)
  {
    assert(type_); //td: no null types, please
    
    schema_item si;
    if (type_->resolve(name, si))
      {
        result = si;
        return true;
      }
      
    return false;
  };
