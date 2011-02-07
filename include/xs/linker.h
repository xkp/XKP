
#ifndef XS_LINKER_H
#define XS_LINKER_H

#include <schema.h>
#include <xs.h>
#include <dynamic_objects.h>

#include "ast.h"
#include "vm.h"
#include "behaviour.h"

namespace xkp
{
  //forwards
  struct  dsl_linker;
  typedef reference<dsl_linker> DslLinker;

  //da linker of da code
  struct code_linker : code_visitor,
                       expression_visitor
    {
      code_linker();
      code_linker(code_context& context);

      ByteCode link();
      void     link(ByteCode result);
      void     link(byte_code* result);

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

      //expression_visitor
      virtual void push(variant operand, bool top);
      virtual void exec_operator(operator_type op, int pop_count, int push_count, bool top);

      public:
        //the linker interface
        int     add_instruction( instruction_type i );
        int     add_instruction( instruction_type i, short data );
        void    instruction_data( int idx, short data );
        int     add_call(instruction_type i, unsigned char param_count, bool is_dynamic, bool invert = false);
        int     add_call(variant caller, const str& name, int param_count, bool is_dynamic, bool invert = false);
				int			add_set(bool is_dynamic, bool invert = false);
        int     add_anonymous_local();
        int     register_variable(stmt_variable& info);
        int     register_variable(const str& name, const str& type, expression* value = null);
        int     register_variable(const str& name, schema* type, expression* value = null);
        int     add_constant( variant constant );
        void    link_code(code& cde, bool track, int continue_pc = 0, int break_pc = 0);
        schema* link_expression(expression& expr, bool assigner = false, bool* empty_stack = null, schema* array_type = null);
        void    register_dsl(const str& name, DslLinker linker);
        variant evaluate_expression(expression& expr);
        schema* typeof_(const xs_type type);
      private:
        enum fixup_dest
          {
            fixup_break,
            fixup_continue,
          };

        struct fixup_data
          {
            fixup_data(int idx, fixup_dest _dest): instruction_idx(idx), dest(_dest) {}

            int        instruction_idx;
            fixup_dest dest;
          };

        struct local_variable
          {
            local_variable():                                 index(0),      type(null)  {}
            local_variable(int _index, schema* _type = null): index(_index), type(_type) {}

            int     index;
            schema* type;
          };

        //these structs will be part of the expression stack
        struct already_in_stack
          {
            already_in_stack()             : type(null)  {}
            already_in_stack(schema* _type): type(_type) {}

            schema* type;
          };

        struct constant
          {
            variant value;
          };

        //containers
        typedef std::map<str, local_variable>			locals_list;
        typedef std::pair<str, local_variable>		locals_pair;
        typedef std::stack<variant>								expr_stack;
        typedef std::vector<fixup_data>						fixup_list;
        typedef std::map<str, DslLinker>					dsl_linker_list;
        typedef std::pair<str, DslLinker>					dsl_linker_pair;
        typedef std::stack<std::pair<int, int> >	loop_stack;

        instruction_list  code_;
        locals_list       locals_;
        int               local_count_;
        int               pc_;
        fixup_list        fixup_;
        expr_stack        stack_;
        constant_list     constants_;
        type_registry     default_types_;
        operator_registry operators_; //td: these 2 should come from the outside world, especially the types
        code_context      context_;
        dsl_linker_list   dsl_linkers_;
        schema*           array_type_; //td: proper type expectancy
        loop_stack        loops_;
				bool							resolving_assigner_;

        void    resolve_value(variant& arg, schema** type = null);
        void    resolve_operator(operator_type op, variant arg1, variant arg2, bool* dont_assign);
        void    resolve_unary_operator(operator_type op, variant arg, bool* dont_assign);
        bool    resolve_custom_operator(operator_type op, schema* type, bool invert, bool* dont_assign);
        void    resolve_assign(const variant& arg, bool invert_set = false);
        void    add_fixup( int idx, fixup_dest dest );
        schema* add_stack_lookup(const str& query, schema* type);
    };

  struct dsl_linker
    {
      virtual void link(dsl& info, code_linker& owner) = 0;
    };

  struct base_xs_linker : xs_visitor
    {
      base_xs_linker(code_context& ctx, IEditableObject* editable_output = null):
        ctx_(ctx),
        editable_output_(editable_output)
        {
          output_ = variant_cast<DynamicObject>(ctx_.this_, DynamicObject());
        }

      //interface
      void                  link(xs_container& xs);
      virtual DynamicObject resolve_instance(const str& name);
      virtual DynamicObject resolve_instance(std::vector<str> name);

      //xs_visitor
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

      protected:
        struct link_item
          {
            link_item(ByteCode _bc, code _cde, schema* _this_type, ParamList _args = ParamList()):
              bc(_bc),
              cde(_cde),
              this_type(_this_type),
              args(_args)
              {
              }

            ByteCode  bc;
            code      cde;
            schema*   this_type;
            ParamList args;
          };

        std::vector<link_item> link_;
        code_context           ctx_;
        DynamicObject          output_;
        IEditableObject*       editable_output_;
    };

  struct class_linker : base_xs_linker
    {
      class_linker(code_context& ctx);

      void link(xs_class& info);

      DynamicClass          class_;
      dynamic_class_schema* result_;
    };

  struct instance_linker : base_xs_linker
    {
      instance_linker(code_context& ctx, DynamicObject instance);

      void link(xs_instance& info);

      DynamicObject instance_;
    };

  struct implicit_instance_linker : base_xs_linker
    {
      implicit_instance_linker(code_context& ctx, DynamicObject instance);

      void link(xs_container& info);

      DynamicObject instance_;
    };

  struct behaviour_linker
    {
      behaviour_linker(code_context& ctx);

      void link(xs_behaviour& info);

      code_context      ctx_;
      Schema            class_;
      behaviour_schema* result_;
    };

  struct prelink_visitor : base_xs_visitor
    {
      prelink_visitor(code_context& ctx, IEditableObject* editable, DynamicObject output) :
        ctx_(ctx),
        editable_(editable),
        output_(output)
        {
        }

      struct pre_link
        {
          ByteCode  bc;
          code      cde;
          ParamList args;
        };

      typedef std::vector<pre_link> prelink_list;

      virtual void property_(xs_property& info);
      virtual void method_(xs_method& info);

      prelink_list links;

      private:
        code_context     ctx_;
        IEditableObject* editable_;
        DynamicObject    output_;
    };

  struct name_collect_visitor : implemented_xs_visitor
    {
      virtual void property_(xs_property& info);
      virtual void method_(xs_method& info);

      std::vector<str> names;
    };


}

#endif
