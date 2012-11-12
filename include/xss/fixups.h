
#ifndef XSS_FIXUPS_HH
#define XSS_FIXUPS_HH

#include "xss_context.h"

namespace xkp
{
  struct fixup_variable_type
    {
      fixup_variable_type(const str& _var_name, XSSExpression _expr, XSSContext _ctx):
        var_name(_var_name),
        expr(_expr),
        ctx(_ctx)
        {
        }

      str           var_name;
      XSSExpression expr;
      XSSContext    ctx;
    };

struct fixup_type
  {
    fixup_type(const str& _type_name, XSSObject _target):
      type_name(_type_name),
      target(_target)
      {
      }

    str       type_name;
    XSSObject target;
  };

struct fixup_property_type
  {
    fixup_property_type(const xs_type& _type_name, XSSProperty _target):
      type_name(_type_name),
      target(_target)
      {
      }

    xs_type     type_name;
    XSSProperty target;
  };

struct fixup_return_type
  {
    fixup_return_type(const str& _type_name, XSSMethod _target):
      type_name(_type_name),
      target(_target)
      {
      }

    str       type_name;
    XSSMethod target;
  };

struct fix_instance_event
  {
    fix_instance_event(XSSObject _target, XSSContext _ctx, std::vector<str>& _instance, XSSSignature _sig, XSSCode _code):
      instance(_instance),
      sig(_sig),
      code(_code),
      target(_target),
      ctx(_ctx)
      {
      }

    XSSObject        target;
    std::vector<str> instance;
    XSSSignature     sig;
    XSSCode          code;
    XSSContext       ctx;
  };

struct fixup_super
  {
    fixup_super(const str& _super, XSSType _type):
      super(_super), 
      type(_type)
      {
      }

    str     super;
    XSSType type;
  };

struct fixup_arg_type
  {
    fixup_arg_type(const str& _type, XSSSignature _args, int _idx):
      args(_args), 
      type(_type),
      idx(_idx)
      {
      }

    str          type;
    XSSSignature args;
    int          idx;
  };

struct fixup_obj_child
  {
    fixup_obj_child(XSSObject _parent, DataEntity _de):
      parent(_parent), 
      de(_de)
      {
      }

    XSSObject  parent;
    DataEntity de;
  };

struct fixup_policy_type
  {
    fixup_policy_type(const str& _type, XSSType _owner, PARENT_CHILD_ACTION _pca):
      type(_type),
      owner(_owner),
      pca(_pca)
      {
      }

    str                 type;
    XSSType             owner;
    PARENT_CHILD_ACTION pca;
  };

}

#endif