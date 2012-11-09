#ifndef XS_CONTEXT_HH
#define XS_CONTEXT_HH

#include <xs.h>
#include <xs/array.h>
#include <xs/linker.h>

#include "boost/filesystem.hpp"
namespace fs = boost::filesystem;

namespace xkp {

//forwards
struct  xss_context;
class		xss_object;
class		xss_type;
class   xss_expression;
class   xss_value;
class   xss_signature;
class   xss_operator;
class   xss_arguments;
class   xss_property;
class   xss_event;
class   xss_event_impl;
class   xss_method;
class   xss_dsl;
struct  inline_renderer;
class   xss_statement;
class   xss_code;

//interfaces
struct ILanguage;
struct IRenderer;
struct ICodeRenderer;
struct IExpressionRenderer;
struct IArgumentRenderer;
struct IErrorHandler;

//reference types
typedef reference<xss_object>		        XSSObject;
typedef weak_reference<xss_object>	    WeakXSSObject;
typedef reference<xss_context>	        XSSContext;
typedef weak_reference<xss_context>	    WeakXSSContext;
typedef reference<xss_expression>       XSSExpression;
typedef reference<xss_value>            XSSValue;
typedef reference<xss_operator>         XSSOperator;
typedef reference<xss_arguments>        XSSArguments;
typedef reference<xss_property>         XSSProperty;
typedef reference<xss_method>		        XSSMethod;
typedef reference<xss_event>		        XSSEvent;
typedef reference<xss_event_impl>		    XSSEventImpl;
typedef reference<xss_type>		          XSSType;
typedef reference<xss_dsl>		          XSSDSL;
typedef reference<xss_signature>		    XSSSignature;
typedef reference<ILanguage>		        Language;
typedef reference<ICodeRenderer>	      CodeRenderer;
typedef reference<IExpressionRenderer>  ExpressionRenderer;
typedef reference<IArgumentRenderer>    ArgumentRenderer;
typedef reference<inline_renderer>      InlineRenderer;
typedef reference<xss_statement>        XSSStatement;
typedef reference<xss_code>             XSSCode;
typedef reference<IErrorHandler>        ErrorHandler;

//enums
enum MARKER_SOURCE
  {
    MS_CURRENT,
    MS_PREVIOUS,
    MS_ENTRY,
  };

enum VALUE_OPERATION
  {
    OP_CONSTANT,
    OP_READ,
    OP_WRITE,
    OP_CALL,
    OP_INDEX,
    OP_INSTANTIATION,
    OP_OBJECT,
    OP_ARRAY,
  };

enum RESOLVE_ITEM
  {
    RESOLVE_ANY,
    RESOLVE_INSTANCE,
    RESOLVE_METHOD,
    RESOLVE_PROPERTY,
    RESOLVE_EVENT,
    RESOLVE_NATIVE,
    RESOLVE_CONST,
    RESOLVE_VARIABLE,
    RESOLVE_TYPE,
    RESOLVE_CHILD,
  };

enum PARENT_CHILD_ACTION
  {
    PCA_ASPROPERTY,
    PCA_ASCHILD,
    PCA_INDEPENDENT_CHILD,
    PCA_DEFAULT,
    PCA_REJECT,
    PCA_FIXUP,
  };

//context
struct context_visitor
  {
    virtual void visit(RESOLVE_ITEM type, const str& id, variant value) = 0;  
  };

struct resolve_info
  {
    resolve_info():
      what(RESOLVE_ANY),
      left(null),
      search_this(true),
      output(null),
      found_this(false),
      shallow(false)
      {
      }

    resolve_info(const resolve_info& other):
      what(other.what),
      left(other.left),
      search_this(other.search_this),
      output(null),
      found_this(false),
      shallow(false)
      {
      }

    RESOLVE_ITEM  what;
    XSSType       type;
    variant       value;
    str*          output;
    resolve_info* left;
    bool          search_this;
    bool          found_this;
    bool          shallow;
  };

//data
class xss_parameter
  {
    public:
      xss_parameter(str name, XSSExpression value):
        name_(name),
        value_(value)
        {
        }
      
      XSSExpression value();
      str           name();
      XSSType       type();
    private:
      str           name_;
      XSSExpression value_;
  };

typedef std::vector<xss_parameter> xss_parameters;

class xss_arguments
  {
    public:
      void           bind(XSSContext ctx);
      XSSType        type(); 
      void           type(XSSType t); 
      void           add(const str& name, XSSExpression value);
      void           push_front(const str& name, XSSExpression value);
    public:
      //iterators
      size_t                   size();
      xss_parameter&           get(size_t idx);
      xss_parameters::iterator begin();
      xss_parameters::iterator end();
    private:
      xss_parameters args_;
      XSSType        type_;
  };

class value_operation
  {
    public:
      value_operation(VALUE_OPERATION op, const str& identifier);

      VALUE_OPERATION id();
      void            id(VALUE_OPERATION op);
      variant         constant();
      void            bind(RESOLVE_ITEM what, variant value);
      XSSArguments    args();
      void            args(XSSArguments value);
      str             identifier();
      bool            is_constant();
      bool            bound();
      RESOLVE_ITEM    resolve_id();
      variant         resolve_value();

      void set_operation(VALUE_OPERATION op);
      void set_arguments(XSSArguments args);
      void set_type_arguments(XSSArguments type_args);
      void set_constant(variant constant);
    private:
      VALUE_OPERATION op_;
      str             identifier_;
      variant         constant_; 
      XSSArguments    args_;
      XSSArguments    type_args_;
      RESOLVE_ITEM    resolve_what_;
      variant         resolve_value_;
  };

//misc
typedef std::vector<XSSObject> XSSObjectList;

class xss_object : public editable_object<xss_object>,
									 public boost::enable_shared_from_this<xss_object>
	{
		public:
			xss_object();
    public:
      virtual void copy(XSSObject obj);
		public:
      //IDinamicObject
      virtual bool resolve(const str& name, schema_item& result);
		public:
			//accesors
      template <typename T>
      T get(const str& what, T default_value)
        {
          return variant_cast<T>(dynamic_get(this, what), default_value);
        }

      template <typename T>
      void set(const str& what, T value)
        {
          dynamic_set(this, what, value);
        }

      str           id();
      str           output_id();
      str           type_name();
			XSSObject			parent();
			DynamicArray	children();
			DynamicArray	properties();
			DynamicArray	methods();
			DynamicArray	events();

      void set_id(const str& id);
      void set_output_id(const str& id);
      void set_type_name(const str& id);
      void set_parent(XSSObject parent);

			virtual XSSType	  type();
			virtual void      set_type(XSSType type);
			virtual XSSObject	idiom();
			virtual void      set_idiom(XSSObject id);

      //0.9.5
      void add_event_impl(XSSEvent ev, XSSCode code, XSSSignature sig, XSSObject instance = XSSObject(), XSSExpression instance_path = XSSExpression());
      bool context_resolve(const str& id, resolve_info& info);
      void context_visit(context_visitor* visitor);
      XSSProperty instantiate_property(const str& prop_name);
    public:
      struct query_info
        {
          query_info(const str& expr): 
            expression(expr), result(new dynamic_array) {}

          void register_property(XSSProperty prop);
          void add_property(XSSProperty prop);
          bool has(XSSProperty prop);

          DynamicArray                result;
          str                         expression;
          std::map<str, XSSProperty>  found;
        };

      void                   query_properties_impl(query_info &info);
      DynamicArray           query_properties(const str& query);
      bool                   has_property(const str& prop);
      bool                   have_value(const str& prop);
		public:
      //misc
      XSSObject              find(const str& what);
      XSSObject              find_by_id(const str& what);
      std::vector<XSSObject> find_by_class(const str& which);
      DynamicArray           find_by_type(const str& which);
      DynamicArray           get_event_impl(const str& event_name, XSSEvent& ev);
      DynamicArray           get_event_code(const str& event_name);
      DynamicArray           get_evimpl();
      variant                get_event_args(const str& event_name);
      DynamicArray           get_attributes();
		  bool                   is_injected(const str& name);
      void                   add_method(const str& name, XSSMethod m);
		  bool                   empty();
      void                   propertize();
    public:
      //0.9.5
      void insert_method(XSSMethod mthd);
      void insert_event(XSSEvent ev);
      void insert_property(XSSProperty prop);
    public:
      //children management
			void add_child(XSSObject obj);
      void remove_child(XSSObject obj);
      void fixup_children(XSSContext ctx);
		public:
      void add_property_(XSSProperty prop);
      XSSProperty add_property(const str& name, XSSExpression value);
      void register_property(const str& name, XSSProperty new_prop = XSSProperty());
      void register_method(const str& name, XSSMethod new_mthd = XSSMethod());

			XSSProperty get_property(const str& name);
      XSSProperty get_shallow_property(const str& name);
			XSSEvent    get_event(const str& name);
			XSSMethod		get_method(const str& name);
      variant     attribute_value(const str& name);

    public:
      str           id_;
      str           output_id_;
      str           type_name_;
			XSSType       type_;
			WeakXSSObject parent_; 
			DynamicArray	children_;
			DynamicArray	properties_;
			DynamicArray	methods_;
			DynamicArray	events_;
			XSSObject     idiom_;

      //0.9.5
      //td: !!! finish the job
      typedef std::vector<XSSEventImpl> event_implementors;
      event_implementors event_impl_;

      virtual void bind(XSSContext ctx);
	};

typedef std::vector<XSSType> XSSTypeList;

//type data
struct parent_policy
  {
    parent_policy():
      action(PCA_DEFAULT),
      match_all(false),
      fixup(false)
      {
      }

    parent_policy(PARENT_CHILD_ACTION _action, XSSType _type):
      action(_action),
      type(_type),
      match_all(false)
      {
      }

    parent_policy(bool _fixup):
      fixup(_fixup),
      action(PCA_DEFAULT),
      match_all(false)
      {
      }

    //what to do
    PARENT_CHILD_ACTION action;

    //info
    XSSType  type;
    bool     match_all;
    ByteCode condition;
    bool     fixup; 

    bool match(XSSObject obj, bool& fixup);
  };

typedef std::vector<parent_policy> parent_policy_list;

class xss_type : public xss_object
  {
    public:
      xss_type();
      xss_type(schema* xs_type);
      xss_type(const str& id, schema* xs_type = null);
    public:
      //xss_object
      virtual bool resolve(const str& name, schema_item& result);
    public:
      XSSOperator get_operator(operator_type op, XSSArguments args);
    public:
      //td: 0.9.5 rethink!
      void          set_super(XSSType super);
      void          set_definition(XSSObject def);
      schema*       native_type();
      XSSType       array_type();
      void          inherit();
      XSSType       get_super();
      DynamicArray  ctor_args();
      void          register_instance(XSSObject obj);
      void          register_foreign_instance(XSSObject obj);
      XSSObjectList get_dependencies();

      //0.9.5
      void         add_constructor(XSSSignature ctor);
      void         import(XSSType import);
      XSSArguments get_constructor(XSSArguments args);
      bool         accepts_child(XSSObject child, PARENT_CHILD_ACTION& result);
      bool         accepts_parent(XSSObject child, PARENT_CHILD_ACTION& result);
      void         add_instance(XSSObject instance);
      void         add_child_policy(parent_policy& policy);
      void         add_parent_policy(parent_policy& policy);
    public:
      void as_enum();
      void as_array(XSSType type);
      void as_variant();
      void as_object();
      void as_unresolved();
    public:
      bool is_enum();
      bool is_array();
      bool is_object();
      bool is_native();
      bool is_variant();
      bool is_unresolved();
    public:
      Language get_language();
      void     set_language(Language lang);
    private:
      XSSType     array_type_;
      schema*     xs_type_;
      bool        is_enum_;
      bool        is_array_;
      bool        is_object_;
      bool        is_variant_;
      bool        is_unresolved_;
    public:
      //td: fix all these
      XSSType       super_;
      Language      lang_;
      DynamicArray  ctor_args_; //td: constructors
      DynamicArray  all_instances_;
      DynamicArray  local_instances_;
      DynamicArray  foreign_instances_;
      XSSObjectList dependencies_;
      
      //0.9.5
      std::vector<XSSOperator>  operators_;
      std::vector<XSSSignature> constructors_;
      parent_policy_list        child_policy_; 
      parent_policy_list        parent_policy_;
  };

class xss_dsl : public xss_object
  {
    public:
      virtual str  render(dsl& info, XSSContext ctx) = 0;
      virtual void pre_process(dsl& info, XSSContext ctx) {}
  };

//the language interface
struct IRenderer
  {
    virtual str render() = 0;
  };

struct ICodeRenderer : public IRenderer
  {
    virtual XSSType type() = 0;
  };

struct IExpressionRenderer : public IRenderer
  {
    virtual XSSType type() = 0;
    virtual variant eval(XSSContext ctx) = 0;
  };

struct IArgumentRenderer : public IRenderer
  {
    virtual void             add(const str& name, XSSType type) = 0;
    virtual param_list_decl& get()                              = 0;
  };

struct IContextCallback
  {
    virtual void notify(XSSContext context) = 0;
  };

//error handling
struct error_info
  {
    error_info(const str _desc, param_list* _info, file_location& _loc):
      desc(_desc),
      info(_info? *_info : param_list()),
      loc(_loc)
      {
      }

    str           desc;
    param_list    info;
    file_location loc;
  };

typedef std::vector<error_info> error_list;

struct error_visitor
  {
    virtual void visit(error_info& err) = 0;
  };

struct IErrorHandler
  {
    virtual void        add(const str& description, param_list* data, file_location& loc) = 0;
    virtual bool        has_errors()                                                      = 0;
    virtual error_list& errors()                                                          = 0;
  };

//code scope, this should not be public
//td: 0.9.5 separate exec and data context
struct xss_context_scope : scope
  {
    xss_context_scope() {}
    xss_context_scope(XSSContext owner);

    void set(XSSContext owner);

    virtual void register_symbol(const str& name, variant value);
    virtual bool resolve(const str& name, variant& result);
    private:
      XSSContext owner_; //td: !!! weak references
  };

struct symbol_data
  {
    symbol_data(RESOLVE_ITEM _type, variant _value):
      type(_type),
      value(_value)
      {
      }

    RESOLVE_ITEM type;
    variant      value;
  };

enum CONTEXT_IDENTITY
  {
    CTXID_NONE,
    CTXID_CODE,
    CTXID_INSTANCE,
    CTXID_TYPE,
    CTXID_FILE,
    CTXID_RUNTIME_CODE,
  };

enum TYPE_MATCH
  {
    MATCH,
    VARIANT,
    TYPECAST
  };

struct xss_context : boost::enable_shared_from_this<xss_context>
  {
    xss_context(XSSContext parent, fs::path path = fs::path());
    xss_context(CONTEXT_IDENTITY identity, variant idobj, XSSContext parent);

    public:
      XSSType       get_type(const str& type);
      XSSType       get_type(schema* type);
      XSSType       get_type(const str& type, const str& ns);
      XSSType       get_type(const xs_type& type, const str& ns = str());

      XSSType       get_array_type(XSSType type);
      XSSType       add_type(const str& id, XSSType type, bool override_parent = false);
      XSSType       add_type(XSSType type, const str& ns);
      XSSObject     get_this();
      void          set_this(XSSObject this_);
      Language      get_language();
      void          set_language(Language lang);
      code_context  get_compile_context(); //td: !!! 0.9.5
      fs::path      path();
      void          set_path(fs::path path);
      void          register_dsl(const str& id, DslLinker dsl);
      void          register_xss_dsl(const str& id, XSSDSL dsl);
      XSSDSL        get_xss_dsl(const str& id);
      void          collect_xss_dsls(std::vector<str>& dsls);
      void          add_parameter(const str& id, XSSType type);
      void          set_args(param_list& args);
      param_list&   get_args();
      void          search_native(bool enabled);
      
      //0.9.5
      XSSType        get_operator_type(operator_type op, XSSType left, XSSType right);
      bool           match_types(XSSType left, XSSType right, TYPE_MATCH& result);
      XSSType        assure_type(const str& type);
      void           set_extents(file_position& begin, file_position& end); 
      file_position& begin();
      file_position& end();
      XSSContext     parent();
      void           set_parent(XSSContext ctx);
      void           identity(CONTEXT_IDENTITY id, variant idobj); 
	    CONTEXT_IDENTITY identity();
      ErrorHandler   errors();
      void           errors(ErrorHandler handler); 
			void           visit(context_visitor* visitor);
	    variant        identity_value();
      bool           add_instance(XSSObject instance);
    public:
      variant resolve(const str& id, RESOLVE_ITEM item_type = RESOLVE_ANY);
      bool    resolve(const str& id, resolve_info& info);
      variant resolve(const str& id, XSSObject instance, RESOLVE_ITEM item_type = RESOLVE_ANY);
      bool    resolve_path(const std::vector<str>& path, resolve_info& info);
      void    register_symbol(RESOLVE_ITEM type, const str& id, variant symbol, bool overrite = false);
    protected:
      typedef std::map<str, XSSType>  type_list;
      typedef std::pair<str, XSSType> type_list_pair;

      WeakXSSContext parent_;
      Language       lang_;
      XSSObject      this_;
      type_list      types_;
      fs::path       path_;
      param_list     args_;
      bool           search_native_;

      file_position begin_;
      file_position end_;
      ErrorHandler  errors_;

      variant empty_type_value(RESOLVE_ITEM item_type);
    protected:
      //symbols
      typedef std::map<str, symbol_data>  symbol_list;
      typedef std::pair<str, symbol_data> symbol_list_pair;

      symbol_list symbols_;

      bool find_symbol(const str& id, resolve_info& info);
    protected:
      //this might not be the best way to simulate the link context
      //a much better way will be to integrate the contexts, that'll be latter
      type_registry     code_types_;
      xss_context_scope code_scope_;
      dsl_list          dsls_;

      bool got_dsls_;
      void collect_dsl();
    protected:  
      //XSS dsls, they are different than code dsls
      typedef std::map<str, XSSDSL>  xss_dsl_list;
      typedef std::pair<str, XSSDSL> xss_dsl_pair;

      xss_dsl_list xss_dsls_;

    private:
      //0.9.5
      CONTEXT_IDENTITY identity_;
      variant          identity_obj_;
      fs::path         src_file_;
      fs::path         output_file_;
       
      bool     resolve_dot(const str& id, resolve_info& info);
      bool     identity_search(const str& id, resolve_info& info);
    public:
      //error handling
      void     error(const str& desc, param_list* info, file_position begin, file_position end);
      fs::path source_file();
      void     source_file(fs::path& sf);
      fs::path output_file();
      void     output_file(fs::path& of);
};

//these are basically copies of their xs counterpart, but offer xss stuff, like generating
//they are also vm friendly, unlike the low level xs's ast.
typedef std::vector<value_operation> value_operations;

//expressions
class xss_value
  {
    public:
      xss_value();
      xss_value(const xss_value& other);
      xss_value(file_position& begin, file_position& end);
    public:
      void              bind(XSSContext ctx, bool as_setter);
      XSSType           type();
	    void              type(XSSType type);
      bool              bound();
      value_operations& operations();      
      void              add_operation(value_operation& op);
      value_operation&  get_last();
      bool              is_constant();
	    bool              is_array();
      variant           constant();
      file_position&    begin(); 
      file_position&    end(); 
      XSSValue          path();
    private:
      XSSType          type_;
      value_operations operations_;      
      file_position    begin_;
      file_position    end_;
    private:
      //bind state
      enum BIND_STATE
        {
          BS_UNBOUND,
          BS_FIXUP,
          BS_ERROR,
          BS_BOUND,
        };

      BIND_STATE state_;
  };

class xss_expression
  {
    public:
      xss_expression():
        op_(op_none),
        is_assign_(false)
        {
          
        } 

      xss_expression(const xss_expression& other) {assert(false);}

      xss_expression(operator_type op, XSSExpression arg1, XSSExpression arg2 = XSSExpression(), XSSExpression arg3 = XSSExpression());
      xss_expression(XSSValue value);
    public:
      void    bind(XSSContext ctx);
    public:
      operator_type op();
      void          op(operator_type val);
      XSSValue      value();
      void          value(XSSValue val);
      XSSType       type();
      void          type(XSSType t);
      bool          is_constant();
      variant       constant_value();
      bool          is_assign();
      XSSExpression left();
      void          left(XSSExpression val);
      XSSExpression right();
      void          right(XSSExpression val);
      XSSExpression third();
      void          third(XSSExpression val);
      XSSOperator   xop();
      void          xop(XSSOperator val);

      void as_array(XSSArguments items);
    public:
      void set_extents(file_position& begin, file_position& end);
      file_position& begin();
      file_position& end();
    private:
      operator_type op_;
      XSSOperator   xop_;
      XSSType       type_;
      XSSValue      value_;
      XSSExpression arg1_;
      XSSExpression arg2_;
      XSSExpression arg3_;
      bool          is_assign_;

      file_position begin_;
      file_position end_;
  };

enum STATEMENT_TYPE
  {
    STATEMENT_IF,
    STATEMENT_VAR,
    STATEMENT_FOR,
    STATEMENT_FOREACH,
    STATEMENT_WHILE,
    STATEMENT_SWITCH,
    STATEMENT_TRY,
    STATEMENT_BREAK,
    STATEMENT_CONTINUE,
    STATEMENT_RETURN,
    STATEMENT_EXPRESSION,
    STATEMENT_THROW,
  };

//statement interfaces
struct IStatementIf
  {
    virtual XSSExpression expr()      = 0;
    virtual XSSCode       if_code()   = 0;
    virtual XSSCode       else_code() = 0;
  };

struct IStatementVar
  {
    virtual str           id()          = 0;
    virtual str           type_name()   = 0;
    virtual XSSType       type()        = 0;
    virtual XSSExpression value()       = 0;
    virtual bool          needs_cast()  = 0;
  };

struct IStatementFor
  {
    virtual str           id()         = 0;
    virtual str           type_name()  = 0;
    virtual XSSType       type()       = 0;
    virtual XSSExpression init_value() = 0;
    virtual XSSExpression init_expr()  = 0;
    virtual XSSExpression cond_expr()  = 0;
    virtual XSSExpression iter_expr()  = 0;
    virtual XSSCode       for_code()   = 0;
    virtual bool          cast_init()  = 0;
  };

struct IStatementForEach
  {
    virtual str           id()          = 0;
    virtual str           type_name()   = 0;
    virtual XSSType       type()        = 0;
    virtual XSSExpression iter_expr()   = 0;
    virtual XSSCode       for_code()    = 0;
    virtual bool          needs_cast()  = 0;
  };

struct IStatementWhile
  {
    virtual XSSExpression expr() = 0;
    virtual XSSCode       code() = 0;
  };

struct statement_switch_section
  {
    statement_switch_section(XSSCode _case_code):
      case_code(_case_code)
      {
      }

    std::vector<XSSExpression> cases;
    XSSCode                    case_code;
  };

typedef std::vector<statement_switch_section> switch_sections;

struct IStatementSwitch
  {
    virtual XSSExpression    expr()         = 0;
    virtual XSSCode          default_code() = 0;
    virtual switch_sections& sections()     = 0;
  };

struct statement_catch_section
  {
    statement_catch_section(const str& _id, const str& _type, XSSCode _catch_code):
      type_name(_type),
      id(_id),
      catch_code(_catch_code)
      {
      }
    
    str     type_name;
    str     id;
    XSSType type;
    XSSCode catch_code;
  };

typedef std::vector<statement_catch_section> catch_sections;

struct IStatementTry
  {
    virtual XSSCode         try_code()     = 0;
    virtual XSSCode         finally_code() = 0;
    virtual catch_sections& sections()     = 0;
  };

struct IStatementExpression
  {
    virtual XSSExpression expr() = 0;
  };

//the actual statement
class xss_statement
  {
    public:
      xss_statement(STATEMENT_TYPE id, file_position& begin, file_position& end):
        id_(id),
        begin_(begin),
        end_(end)
        {
          assert(begin_.line >= 0 && end_.line >= 0);
        }

    public:
      STATEMENT_TYPE id() {return id_;}
    public:
      template<typename T> T* cast()
        {
          return dynamic_cast<T*>(this);
          //return variant_cast<T*>(this, null);
        }

      file_position& begin() {return begin_;}
      file_position& end()   {return end_;  }
      
      virtual void bind(XSSContext ctx) = 0;
    protected:
      STATEMENT_TYPE id_;
      file_position  begin_;
      file_position  end_;
  };

typedef std::vector<XSSStatement> statement_list;

class xss_code
  {
    public:
      xss_code() : 
        ctx_(XSSContext(new xss_context(XSSContext())))
        {
        }
    public:
      void            add(XSSStatement st);
      XSSContext      context();
      void            bind(XSSContext ctx);
      XSSType         return_type();
      void            set_extents(file_position& begin, file_position& end); 
      file_position&  begin(); 
      file_position&  end(); 
      statement_list& statements();
      bool            empty();
    private:
      statement_list statements_;
      XSSContext     ctx_; 
      XSSType        return_type_;
  };

struct signature_item
  {
    signature_item();
    signature_item(const signature_item& other);
    signature_item(const str& _name, XSSType _type, XSSExpression _value);
    signature_item(const str&_name, const str& _type_name, XSSExpression _value);

    str           name;
    str           type_name;
    XSSType       type;
    XSSExpression default_value; 
    bool          cast_default; //default_value's type must be cast
  };

typedef std::vector<signature_item> signature_items;

class xss_signature
  {
    public:
      xss_signature();
      xss_signature(const xss_signature& other);
    public:
      signature_items& items();

      bool match(XSSArguments args);
      bool match_signature(XSSSignature sig);

      void add_argument(const str& name, XSSType type, XSSExpression default_value);
      void add_argument(const str& name, const str& type_name, XSSExpression default_value);

      void bind(XSSContext ctx);
      void arg_type(int idx, XSSType type);
      void native(const str& s);
      str  native(); 
    private:
      signature_items items_;
      str             native_;
  };

class xss_operator
  {
    public:
      xss_operator(operator_type op, XSSType result);
      xss_operator(operator_type op, XSSType result, XSSType right);
      xss_operator(operator_type op, XSSType result, XSSType left, XSSType right);
      xss_operator(operator_type op, XSSType result, XSSSignature signature);
    public:
      XSSType         type();
      operator_type   opid();
      bool            match(XSSArguments args);
      bool            match(XSSType type);
      InlineRenderer  renderer();
    private:
      operator_type   op_;
      XSSType         result_;
      XSSType         left_;
      XSSType         right_;
      XSSSignature    signature_; 
      InlineRenderer  renderer_;
  };

struct ILanguage
  {
    //0.9.5
  //  virtual variant compile_code(code& cde, param_list_decl& params, XSSContext ctx)	                    = 0;
  //  virtual variant compile_expression(expression expr, XSSContext ctx)							                      = 0;
		//virtual variant compile_args(param_list_decl& params, XSSContext ctx)					                        = 0;
  //  virtual str     resolve_this(XSSContext ctx)																			                    = 0;
  //  virtual str     resolve_separator(XSSObject lh = XSSObject())										                      = 0;
  //  virtual bool    can_cast(XSSType left, XSSType right)                                                 = 0;
  //  virtual void    init_context(XSSContext ctx)                                                          = 0; //td: !!!
  //  virtual void    init_application_context(XSSContext ctx)                                              = 0;
  //  virtual XSSType resolve_array_type(XSSType type, const str& at_name, XSSContext ctx)                  = 0;
  //  virtual str     render_value(XSSType type, variant value)                                             = 0;
  //  virtual str     property_get(XSSProperty prop, const str& path, XSSContext ctx)                       = 0;
  //  virtual str     property_set(XSSProperty prop, const str& path, const str& value, XSSContext ctx)     = 0;
  //  virtual str     render_assignment(const str& path, const str& prop, const str& value)                 = 0;
  //  virtual str     expression_path(const str& expr )                                                     = 0;
  //  virtual str     array_operation(operator_type op, const str& arr, const str& value, XSSContext ctx)   = 0;
  //  virtual str     render_expression(expression& expr, XSSContext ctx)                                   = 0;
  //  virtual str     instantiate(XSSType type, XSSObject instance, DynamicArray rt, param_list& args)      = 0;
  //  virtual str     render_ctor_args(XSSType type, XSSObject instance, DynamicArray rt, param_list& args) = 0;
  //  virtual bool    custom_operator(XSSType lt, XSSType rt, str l, str r, operator_type op, str& res)     = 0;
    
    //0.9.5
    virtual XSSContext create_context()                                                                  = 0;
    virtual void       init_compile_context(XSSContext ctx)                                              = 0;
    virtual bool       render_code(XSSCode code, XSSContext ctx, std::ostringstream& result)             = 0; 
    virtual bool       render_expression(XSSExpression expr, XSSContext ctx, std::ostringstream& result) = 0; 
    virtual bool       render_value(XSSValue value, XSSContext ctx, std::ostringstream& result)          = 0;
    virtual bool       render_type_name(XSSType type, XSSContext ctx, std::ostringstream& result)        = 0;
    virtual bool       render_signature(XSSSignature type, XSSContext ctx, std::ostringstream& result)   = 0;
    
    //code rendering
    virtual bool render_if(IStatementIf* info, XSSContext ctx, std::ostringstream& result)                     = 0;
    virtual bool render_variable(IStatementVar* info, XSSContext ctx, std::ostringstream& result)              = 0;
    virtual bool render_for(IStatementFor* info, XSSContext ctx, std::ostringstream& result)                   = 0;
    virtual bool render_foreach(IStatementForEach* info, XSSContext ctx, std::ostringstream& result)           = 0;
    virtual bool render_while(IStatementWhile* info, XSSContext ctx, std::ostringstream& result)               = 0;
    virtual bool render_switch(IStatementSwitch* info, XSSContext ctx, std::ostringstream& result)             = 0;
    virtual bool render_try(IStatementTry* info, XSSContext ctx, std::ostringstream& result)                   = 0;
    virtual bool render_break(XSSContext ctx, std::ostringstream& result)                                      = 0;
    virtual bool render_continue(XSSContext ctx, std::ostringstream& result)                                   = 0;
    virtual bool render_return(IStatementExpression* info, XSSContext ctx, std::ostringstream& result)         = 0;
    virtual bool render_expr_statement(IStatementExpression* info, XSSContext ctx, std::ostringstream& result) = 0;
    virtual bool render_throw(IStatementExpression* info, XSSContext ctx, std::ostringstream& result)          = 0;

    //expression rendering
    virtual bool render_assignment(XSSExpression expr, XSSValue left_value, XSSExpression right, XSSContext ctx, std::ostringstream& result, const str& path = str()) = 0;
    virtual bool render_array_assignment(XSSExpression expr, XSSArguments index, XSSContext ctx, std::ostringstream& result)                 = 0;
    virtual bool render_operator(XSSExpression expr, XSSContext ctx, std::ostringstream& result)                                             = 0;
    virtual bool render_constant(variant& value, XSSContext ctx, std::ostringstream& result)                                                 = 0; 
    virtual bool render_read_operation(value_operation& op, XSSContext ctx, std::ostringstream& result)                                      = 0;  
    virtual bool render_call(value_operation& op, XSSContext ctx, std::ostringstream& result)                                                = 0;  
    virtual bool render_arguments(XSSArguments args, XSSContext ctx, std::ostringstream& result)                                             = 0;  
    virtual bool render_index_operation(value_operation& op, XSSContext ctx, std::ostringstream& result)                                     = 0;  
    virtual bool render_object(value_operation& op, XSSContext ctx, std::ostringstream& result)                                              = 0;  
    virtual bool render_array(value_operation& op, XSSContext ctx, std::ostringstream& result)                                               = 0;  
    virtual bool render_instantiation(XSSType type, XSSArguments args, XSSContext ctx, std::ostringstream& result)                           = 0;  

    //utils
    virtual bool render_pre_statement(XSSStatement info, XSSContext ctx, std::ostringstream& result) = 0;  
  };

//rendering helper
struct inline_renderer
  {
    inline_renderer();

    bool render(param_list& pl, std::ostringstream& result);
    bool compile(const str& text, bool global, param_list& params);

    private:
      variant renderer_; //shame
      bool    global_;
  };

//constructs
class xss_property : public xss_object
  {
	  public:
		  xss_property();
		  xss_property(const xss_property& other);
		  xss_property(const str& name, XSSExpression value, XSSObject _this_);
		  xss_property(const str& name, XSSExpression, variant _get, variant _set, XSSObject _this_);

			str get_name();

		  virtual void copy(XSSObject obj);
			virtual XSSType type();

      variant   get_;
		  variant   set_;
		  size_t    flags;
		  XSSObject this_;
		  variant   value_;

      //0.9.5
      //XSSObject get_get();
      //XSSObject get_set();
      //void    set_value(const variant value, XSSType type);
      //str     render_value();
      //str     render_get();
      //str	    render_set(const str& value);
      //variant eval(XSSContext ctx);
      
      //0.9.5
      void            expr_value(XSSExpression value);
      XSSExpression   expr_value();
      void            getter(InlineRenderer getter);
      InlineRenderer  getter();
      void            setter(InlineRenderer setter);
      InlineRenderer  setter();
      void            code_getter(XSSCode getter);
      XSSCode         code_getter();
      void            code_setter(XSSCode setter);
      XSSCode         code_setter();
      void            as_const();
      bool            is_const();
      void            property_type(XSSType type);
      XSSType         property_type();
      XSSObject       instance_value();
      void            instance_value(XSSObject value);
      bool            original();
      void            original(bool value);
      
      virtual void bind(XSSContext ctx);
    public:
      //0.9.5
      XSSExpression  expr_value_;
      InlineRenderer getter_;
      InlineRenderer setter_;
      XSSCode        code_getter_;
      XSSCode        code_setter_;
      XSSType        prop_type_;
      XSSObject      instance_value_;
      bool           is_const_;
      bool           original_;
  };

class xss_event : public xss_object
  {
		public:
			xss_event();
			xss_event(const xss_event& other);
			xss_event(const str& name, XSSSignature signature, InlineRenderer dispatcher = InlineRenderer());

			str get_name();

			//0.9.5
      //DynamicArray impls;
			//variant			 args;

			//bool implemented();

      //0.9.5
      void         set_dispatcher(InlineRenderer dispatcher);
      void         set_signature(XSSSignature sig);
      XSSSignature signature();

      virtual void bind(XSSContext ctx);
    private:
      InlineRenderer dispatcher_;
      XSSSignature   signature_;
  };

class xss_event_impl : public xss_object
  {
		public:
			xss_event_impl();
			xss_event_impl(const xss_event_impl& other);
			xss_event_impl(XSSEvent ev, XSSSignature sig, XSSCode code, XSSObject instance = XSSObject(), XSSExpression instance_path = XSSExpression());

			void          set_ev(XSSEvent ev); 
			XSSEvent      ev(); 
      void          set_code(XSSCode code);
      XSSCode       code();
      void          set_signature(XSSSignature sig);
      XSSSignature  signature();
      void          set_instance(XSSObject instance);
      XSSObject     instance();
      void          set_instance_path(XSSExpression instance_path);
      XSSExpression instance_path();

      virtual void bind(XSSContext ctx);
    private:
			XSSEvent      ev_; 
      XSSCode       code_;
      XSSSignature  signature_;
      XSSObject     instance_;
      XSSExpression instance_path_;
  };

typedef std::vector<signature_item> signature_items;

class xss_method : public xss_object
  {
		public:
			xss_method();

      //0.9.5
			//xss_method(const xss_method& other);
			//xss_method(const str& name, XSSType type, variant args, variant code);
			//str      get_name();
      //variant  code();
      //void     add_parameter(const str& name);
      //variant& get_parameters(); 
			//variant args_;
			//variant code_;

      //xss_object
			virtual XSSType type();

      //0.9.5
      void           return_type(XSSType type);
      XSSType        return_type();
      void           renderer(InlineRenderer caller);
      InlineRenderer renderer();
      void           signature(XSSSignature sig);
      XSSSignature   signature();
      void           code(XSSCode code);
      XSSCode        code();

      virtual void bind(XSSContext ctx);
    private:
      InlineRenderer renderer_;
    public:
      XSSCode      code_; 
      XSSSignature signature_;
      XSSType      return_type_;
  };

//utils
struct xss_utils
  {
    static str var2string(variant& v);
    static bool var2bool(variant& v);
    static fs::path relative_path(fs::path& src, fs::path& dst);
  };

//glue
template <typename T>
struct xss_object_schema : editable_object_schema<T>
  {
    virtual void declare()
      {
				this->template property_<str>         ("id",          &T::id_);
				this->template property_<str>         ("output_id",   &T::output_id,  &T::set_output_id);
				this->template property_<DynamicArray>("children",		&T::children_);
				this->template property_<DynamicArray>("properties",  &T::properties_);
				this->template property_<DynamicArray>("events",			&T::events_);
				this->template property_<DynamicArray>("methods",		  &T::methods_);
				this->template property_<str>         ("type_name",		&T::type_name_);
				this->template property_<str>         ("class_name",	&T::type_name_);
        this->template property_<XSSType>     ("type",        &T::type,       &T::set_type);
        this->template property_<XSSObject>   ("idiom",       &T::idiom_);

        this->template readonly_property<XSSObject>    ("parent", &T::parent );
        this->template readonly_property<DynamicArray> ("evimpl", &T::get_evimpl );

        this->template method_<DynamicArray, 1>("query_properties", &T::query_properties);
        this->template method_<XSSProperty,  1>("get_property",     &T::get_property);
        this->template method_<XSSProperty,  2>("add_property",     &T::add_property);
        this->template method_<bool,         1>("has_property",     &T::has_property);
        this->template method_<bool,         1>("have_value",       &T::have_value);
        this->template method_<void,         1>("add_child",        &T::add_child);
        this->template method_<XSSMethod,    1>("get_method",       &T::get_method);
        this->template method_<void,         2>("add_method",       &T::add_method);
        this->template method_<DynamicArray, 1>("find_by_type",     &T::find_by_type);
        this->template method_<XSSObject,    1>("find",             &T::find);
        this->template method_<variant,      1>("attribute_value",  &T::attribute_value);
        this->template method_<bool,         0>("empty",            &T::empty);
        this->template method_<DynamicArray, 0>("get_attributes",   &T::get_attributes);

        //0.9.5
        //this->template method_<DynamicArray, 1>("get_event_code",   &T::get_event_code);
        //this->template method_<variant,      1>("get_event_args",   &T::get_event_args);
		  }
  };

struct xss_type_schema : xss_object_schema<xss_type>
  {
    virtual void declare()
      {
				xss_object_schema<xss_type>::declare();

				inherit_from<xss_object>();

				readonly_property<XSSType>("super",                  &xss_type::get_super);
        readonly_property<bool>("is_enum",                   &xss_type::is_enum);
				readonly_property<bool>("is_array",                  &xss_type::is_array);
				readonly_property<bool>("is_object",                 &xss_type::is_object);
				readonly_property<bool>("is_native",                 &xss_type::is_native);
				readonly_property<bool>("is_variant",                &xss_type::is_variant);
				readonly_property<DynamicArray>("instances",         &xss_type::all_instances_);
				readonly_property<DynamicArray>("local_instances",   &xss_type::local_instances_);
				readonly_property<DynamicArray>("foreign_instances", &xss_type::foreign_instances_);

				readonly_property<DynamicArray>("constructor_params", &xss_type::ctor_args);
      }
  };

struct xss_event_schema : xss_object_schema<xss_event>
  {
    virtual void declare()
      {
				xss_object_schema<xss_event>::declare();

				inherit_from<xss_object>();
        readonly_property<str> ("name",       &xss_event::get_name);
        property_<XSSSignature>("signature",  &xss_event::signature, &xss_event::set_signature);

    //    property_("args",  &xss_event::args);
    //    property_("impls", &xss_event::impls);
				//readonly_property<bool>("implemented", &xss_event::implemented);
      }
  };

struct xss_event_impl_schema : xss_object_schema<xss_event_impl>
  {
    virtual void declare()
      {
				xss_object_schema<xss_event_impl>::declare();

				inherit_from<xss_object>();

        property_<XSSExpression>("ev",            &xss_event_impl::ev,             &xss_event_impl::set_ev);
        property_<XSSCode>      ("code",          &xss_event_impl::code,           &xss_event_impl::set_code);
        property_<XSSSignature> ("signature",     &xss_event_impl::signature,      &xss_event_impl::set_signature);
        property_<XSSObject>    ("instance",      &xss_event_impl::instance,       &xss_event_impl::set_instance);
        property_<XSSExpression>("instance_path", &xss_event_impl::instance_path,  &xss_event_impl::set_instance_path);
      }
  };

struct xss_method_schema : xss_object_schema<xss_method>
  {
    virtual void declare()
      {
				xss_object_schema<xss_method>::declare();

				inherit_from<xss_object>();

        //0.9.5
        //method_<void, 1>("add_parameter", &xss_method::add_parameter);
        //readonly_property<str>("name", &xss_method::get_name);

        //property_("args", &xss_method::args_);
        property_                      ("code",        &xss_method::code_);
        readonly_property<XSSSignature>("signature",   &xss_method::signature_);
        readonly_property<XSSType>     ("return_type", &xss_method::return_type_);
      }
  };

struct xss_property_schema : xss_object_schema<xss_property>
  {
    virtual void declare()
      {
				xss_object_schema<xss_property>::declare();

				inherit_from<xss_object>();

        readonly_property<str>("name", &xss_property::get_name);

        property_("get",   &xss_property::get_);
        property_("set",   &xss_property::set_);

        //0.9.5
        //property_("value", &xss_property::value_);
        //readonly_property<XSSObject>("get_", &xss_property::get_get);
        //readonly_property<XSSObject>("set_", &xss_property::get_set);
        //method_<str, 0>("render_value", &xss_property::render_value);

        //0.9.5
        readonly_property<XSSType>      ("property_type",  &xss_property::prop_type_);
        readonly_property<XSSExpression>("value",          &xss_property::expr_value_);
        readonly_property<XSSObject>    ("instance_value", &xss_property::instance_value_);
        readonly_property<XSSCode>      ("get_code",       &xss_property::code_getter_);
        readonly_property<XSSCode>      ("set_code",       &xss_property::code_setter_);
        readonly_property<bool>         ("is_const",       &xss_property::is_const_);
        property_("original",                              &xss_property::original_);
        
      
        //td: !!! do property interface, its a mess
      //InlineRenderer getter_;
      //InlineRenderer setter_;
      }
  };

struct xss_signature_schema : object_schema<xss_signature>
  {
    virtual void declare()
      {
			}
  };

struct xss_expression_schema : object_schema<xss_expression>
  {
    virtual void declare()
      {
			}
  };

struct xss_value_schema : object_schema<xss_value>
  {
    virtual void declare()
      {
			}
  };

register_complete_type(xss_object,      xss_object_schema<xss_object>);
register_complete_type(xss_type,        xss_type_schema);
register_complete_type(xss_event,		    xss_event_schema);
register_complete_type(xss_event_impl,	xss_event_impl_schema);
register_complete_type(xss_property,    xss_property_schema);
register_complete_type(xss_method,	    xss_method_schema);
register_complete_type(xss_signature,	  xss_signature_schema);
register_complete_type(xss_expression,	xss_expression_schema);
register_complete_type(xss_value,	      xss_value_schema);

register_iterator(XSSObject);

//interface glue
template <typename T>
struct renderer_schema : object_schema<T>
  {
    virtual void declare_base()
      {
        this->template implements<IRenderer>();

        this->template method_<str, 0>("render", &T::render);
      }

    virtual void declare()
      {
        //compiler pleaser
      }
  };

}


#endif
