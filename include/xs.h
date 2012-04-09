#ifndef XS_HEADER_H
#define XS_HEADER_H

#include <types.h>
#include <base.h>
#include <dynamic_objects.h>
#include <xs/vm.h>
#include <xs/ast.h>

namespace xkp
{
  //the infamous scope. just a random collection of identifiers
  //you wish part of the linking. All this different resolvers
  //could be somehow collapsed. However the concepts are pretty
  //different (parameter list, type collections, etc).
  //So, it stays...
  struct scope
    {
      virtual void register_symbol(const str& name, variant value) = 0; 
      virtual bool resolve(const str& name, variant& result)       = 0;
    };
    
  typedef reference<scope> Scope;
  
  //this is the data needed by the xs compiler, all information 
  //here is optional, mix and match.

  typedef std::map<str, variant>  dsl_list;
  typedef std::pair<str, variant> dsl_list_pair;

  struct code_context
    {
      code_context(): 
        types_(null),
        scope_(null),
        args_(null),
        this_type(null),
        dsl_(null)
        {
        }
        
      code_context(code_context& other): 
        types_(other.types_),
        scope_(other.scope_),
        args_(other.args_),
        this_type(other.this_type),
        dsl_(other.dsl_)
        {
        }

      type_registry* types_;
      scope*         scope_;
      param_list*    args_;
      schema*        this_type;
      variant        this_;
      dsl_list*      dsl_; 
    };
    
  //a simple scope
  struct basic_scope : scope
    {
      virtual void register_symbol(const str& name, variant value); 
      virtual bool resolve(const str& name, variant& result);
      
      private:
        typedef std::map<str, variant>  symbol_list;
        typedef std::pair<str, variant> symbol_pair;
        
        symbol_list symbols_;
    };  

  struct base_code_context : code_context
    {
      base_code_context() : code_context()
        {
          types_ = &types;
          scope_ = &scope;
          args_  = &args;
          dsl_   = &dsls; 
        }

      base_code_context(base_code_context& other) : code_context(other),
        scope(other.scope),
        types(other.types),
        dsls(other.dsls),
        args(other.args)
        {
          types_ = &types;
          scope_ = &scope;
          args_  = &args;
          dsl_   = &dsls; 
        }
      public:
        basic_scope   scope;
        type_registry types; 
        dsl_list      dsls;
        param_list    args;
    };

  //some utils, we'll provide some high level functionality
  //how ever you can check for the "low" level samples and
  //put together whatever is not provided here.
  
  //td: if I dont get the ugly struct in here it gives me linking errors
  struct xs_utils
    {
      //expressions
      variant evaluate_expression(const str& src);
      variant evaluate_xs_expression(const str& src, code_context& ctx, fs::path file);
      variant evaluate_xs_expression(const expression& src, code_context& ctx, fs::path file);
      bool    compile_expression(const str& src, expression& expr);
    
      //plain execute
      variant execute_xs(const str& src, code_context& ctx, fs::path file);
      
      //plain compile
      ByteCode compile_code(const str& src, code_context& ctx, fs::path file);
      void     compile_code(const str& src, code_context& ctx, fs::path file, code& result);
      
      //compile instance code
      void compile_instance(const str& src, DynamicObject instance, code_context& ctx, fs::path file);
      void compile_implicit_instance(const str& src, DynamicObject instance, code_context& ctx, fs::path file);
      
      //compile class code, the result types will be added to the context's type list
      //note these 2 functions will fail (via exception error) when there is mixed code
      //i.e instances mixed with classes. The next function down will handle that
      void compile_class(const str& src, code_context& ctx, fs::path file);
      
      //compile high level xs from and into a context
      //means classes will be added to the type registry
      //and instances to the scope. This can and probably will change.
      void compile(const str& src, code_context& ctx, fs::path file);
    };
    
  //an object wrapper
  struct object_scope : scope
    {
      object_scope(IDynamicObject* obj) : obj_(obj) {}
    
      virtual void register_symbol(const str& name, variant value); 
      virtual bool resolve(const str& name, variant& result);
      
      IDynamicObject* obj_;
    };
    
  //a composite
  struct composite_scope : scope
    {
      composite_scope(scope* s1, scope* s2): s1_(s1), s2_(s2) {}
    
      virtual void register_symbol(const str& name, variant value); 
      virtual bool resolve(const str& name, variant& result);
      
      scope* s1_;
      scope* s2_;
    };
    
  //a scope serving a linking context that needs to access
  //properties and methods as callables.
  struct type_scope : scope
    {
      type_scope(schema* type) : type_(type) {}
    
      virtual void register_symbol(const str& name, variant value); 
      virtual bool resolve(const str& name, variant& result);
      
      schema* type_;
    };
    
  struct editable_scope : basic_scope, IEditableObject
    {
      public:
        virtual void add_item(const str& name, schema_item& item)
          {
            register_symbol(name, item);
          }
        virtual size_t register_event(const str& name)
          {
            assert(false); //see no reason to
            return 0;
          }
    };              
}

#endif