#ifndef LANGUAGE_HH
#define LANGUAGE_HH

#include "xs/compiler.h"

namespace xkp
{
  
  //util
  struct source_collector :  xs_visitor
    {
      virtual void property_(xs_property& info);
      virtual void method_(xs_method& info);
      virtual void event_(xs_event& info);
      virtual void event_decl_(xs_event_decl& info);
      virtual void const_(xs_const& info);
      virtual void instance_(xs_instance& info);
      virtual void class_(xs_class& info);
      virtual void behaviour_(xs_behaviour& info);
      virtual void behaveas_(xs_implement_behaviour& info);
      virtual void dsl_(dsl& info);
      
      public:
          std::vector<xs_property> properties;
          std::vector<xs_method>   methods;
          std::vector<xs_event>    events;
          std::vector<xs_instance> instances;
    };

struct code_type_resolver : code_visitor
	{
		code_type_resolver(XSSContext ctx);

		XSSType get();

		//code_visitor
    virtual void variable_(stmt_variable& info);
    virtual void return_(stmt_return& info);
		virtual void if_(stmt_if& info);
    virtual void for_(stmt_for& info);
    virtual void iterfor_(stmt_iter_for& info);
    virtual void while_(stmt_while& info);
    virtual void break_();
    virtual void continue_();
    virtual void expression_(stmt_expression& info);
    virtual void dsl_(dsl& info);
    virtual void dispatch(stmt_dispatch& info);

		public:
			void register_var(const str& name, XSSType type);
		private:
			std::map<str, XSSType> vars_;
			XSSContext						 ctx_;
			bool									 is_variant_;
			schema*								 result_;
	};

}

#endif