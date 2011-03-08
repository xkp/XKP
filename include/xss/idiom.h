#ifndef XSS_IDIOM_H
#define XSS_IDIOM_H

#include <base.h>
#include <schema.h>

#include <xs/ast.h>
#include <xss/project.h>

namespace xkp{

struct ITranslator
	{
		virtual str generate(const param_list pl) = 0;
	};

//engine, this are the items that get translated
struct base_xss_args : ITranslator
  {
    base_xss_args();
    base_xss_args(const base_xss_args& other);
    base_xss_args(param_list_decl& args);
    
		//ITranslator
		virtual str generate(const param_list pl);

		//interface
		virtual str resolve_param(const param_decl& param) = 0;
    protected:
      param_list_decl args_;
  };

struct base_xss_expression : ITranslator
  {
    base_xss_expression();
    base_xss_expression(XSSContext ctx, expression& expr);
  
		//ITranslator
		virtual str generate(const param_list pl);

		private:
      expression expr_;
      XSSContext ctx_;
  };

struct base_xss_code : ITranslator, code_visitor
  {
    base_xss_code();
    base_xss_code(XSSContext ctx, code& code);
  
		//ITranslator
		virtual str generate(const param_list pl);
		
		//code_visitor
    virtual void if_(stmt_if& info);
    virtual void variable_(stmt_variable& info);
    virtual void for_(stmt_for& info);
    virtual void iterfor_(stmt_iter_for& info);
    virtual void while_(stmt_while& info);
    virtual void break_();
    virtual void continue_();
    virtual void return_(stmt_return& info);
    virtual void expression_(stmt_expression& info);
    virtual void dsl_(dsl& info);
    virtual void dispatch(stmt_dispatch& info);

		private:
      code       code_;
      XSSContext ctx_;
			str				 result_;	
			int				 indent_;
			
			void add_line(const str& line, bool dress_line = false);
			str	 get_indent_str();
      void render_code(code& cde, int indent);
			void register_var(const str& name, const str& type);
  };

//utils
struct idiom_utils
	{
		static str expr2str(expression& expr, XSSContext ctx);
	};

//utils, this is soon to be changed completely
struct code_type_resolver : code_visitor
	{
		code_type_resolver(XSSContext ctx) : result_(null), ctx_(ctx), is_variant_(false) {}

		schema* get();

		//code_visitor
    virtual void variable_(stmt_variable& info);     
    virtual void return_(stmt_return& info);         
		
		virtual void if_(stmt_if& info)									{}
    virtual void for_(stmt_for& info)               {}
    virtual void iterfor_(stmt_iter_for& info)      {}
    virtual void while_(stmt_while& info)           {}
    virtual void break_()                           {}
    virtual void continue_()                        {}
    virtual void expression_(stmt_expression& info) {}
    virtual void dsl_(dsl& info)                    {}
    virtual void dispatch(stmt_dispatch& info)      {}
		
		private:
			std::map<str, schema*> vars_;
			XSSContext						 ctx_;	
			bool									 is_variant_;
			schema*								 result_;
	};



//type info
template<typename T>
struct translator_schema : object_schema<T>  
	{
    virtual void declare_base()
      {
        implements<ITranslator>();
				dynamic_function_<str>("generate", &T::generate);
      }

		virtual void declare()
			{
			}
	};

} //namespace


#endif