#include <xss/lang/waxjs.h>
#include <xss/dsl/sql.h>
#include <xss/dsl/shell.h>

#include <boost/algorithm/string/replace.hpp>

using namespace xkp;

const str SLanguage("language");

const str SCannotForkOnIteration("Cannot perform asynchronic operations inside loops");
const str SCodeAfterReturn("Unreachable code");
const str SBadHTML("Errors parsing html");
const str SMissingHTMLTag("Missing declared tag");
const str SUnknownDSL("Unkown dsl");

#define CHECK_ACTIVE  if (found_) return; curr_++;

struct wax_splitter : code_visitor
  {
    wax_splitter(XSSContext ctx, code& _code):
      ctx_(ctx),
      found_(false),
      curr_(0),
      code_(_code),
      maybe_(false)
      {
      }

    bool maybe()
      {
        return maybe_;
      }

    virtual void if_(stmt_if& info)
      {
        CHECK_ACTIVE

        XSSMethod mthd;
        if (forked(info.expr, mthd))
          {
            cut(mthd);
          }
        else
          {
            CodeSplit result;
            if (forked(info.if_code, result))
              {
                cut(result);
                result_->split_on_if = true;
                return;
              }
            
            if (forked(info.else_code, result))
              {
                cut(result);
                result_->split_on_else = true;
              }
          }
      }

    virtual void variable_(stmt_variable& info)
      {
        CHECK_ACTIVE

        XSSMethod mthd;
        if (forked(info.value, mthd))
          {
            cut(mthd);
          }
      }

    virtual void for_(stmt_for& info)
      {
        CHECK_ACTIVE
        
        XSSMethod mthd;
        if (forked(info.cond_expr, mthd) || forked(info.iter_expr, mthd))
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SCannotForkOnIteration);
            error.add("comment", str("dont get cute"));
            xss_throw(error);
          }

        if (forked(info.init_expr, mthd))
          {
            cut(mthd);
          }
      }

    virtual void iterfor_(stmt_iter_for& info)
      {
        CHECK_ACTIVE

        XSSMethod mthd;
        if (forked(info.iter_expr, mthd))
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SCannotForkOnIteration);
            error.add("comment", str("dont get cute"));
            xss_throw(error);
          }
      }

    virtual void while_(stmt_while& info)
      {
        CHECK_ACTIVE

        XSSMethod mthd;
        if (forked(info.expr, mthd))
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SCannotForkOnIteration);
            error.add("comment", str("dont get cute"));
            xss_throw(error);
          }
      }

    virtual void break_()
      {
        CHECK_ACTIVE
      }

    virtual void continue_()
      {
        CHECK_ACTIVE
      }

    virtual void return_(stmt_return& info)
      {
        CHECK_ACTIVE

        XSSMethod mthd;
        if (forked(info.expr, mthd))
          {
            cut(mthd);
          }
      }

    virtual void expression_(stmt_expression& info)
      {
        CHECK_ACTIVE

        XSSMethod mthd;
        if (forked(info.expr, mthd))
          {
            cut(mthd);
          }
      }

    virtual void dsl_(dsl& info)
      {
        CHECK_ACTIVE

        XSSDSL dsl = ctx_->get_xss_dsl(info.name);
        if (!dsl)
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SUnknownDSL);
            error.add("dsl", info.name);
            xss_throw(error);
          }

        bool asynch = XSSObject(dsl)->get<bool>("asynch", false);
        if (asynch)
          cut(XSSMethod());
      }

    virtual void dispatch(stmt_dispatch& info)
      {
        assert(false); //ditto
      }
    
    CodeSplit get()
      {
        return result_;
      }

    private:
      bool       found_;
      int        curr_;
      XSSContext ctx_;
      CodeSplit  result_;
      code&      code_;
      bool       maybe_;

      bool forked(expression& expr, XSSMethod& result)
        {
          //handle some cases, there will be limitations
          operator_type op;
          expr.top_operator(op);
          switch(op)
            {
              case op_dot:
              case op_call:
                break; //path like expression, handled below
              case op_func_call:
                break;
              case op_assign:
              case op_plus_equal:
              case op_minus_equal:
              case op_mult_equal:
              case op_div_equal:
              case op_shift_right_equal:
              case op_shift_left_equal:
                {
                  //assigns, lets break it down
                  expression_splitter es(op);
                  expr.visit(&es);
                  return forked(es.right, result);
                }
              default: //limitations
                return false;
            }

          expression_analizer ea; 
          ea.analyze(expr, ctx_);

          if (ea.is_call())
            {
              XSSMethod mthd = ea.method();
              if (mthd)
                {
                  bool asynch = mthd->get<bool>("asynch", false);
                  if (asynch)
                    {
                      result = mthd;
                    }
                  else
                    {
                      //need to check for local methods that might have not been checked for 
                      //splittage

                      IRenderer*           rend = variant_cast<IRenderer*>(mthd->code(), null);
                      waxjs_code_renderer* wcr  = dynamic_cast<waxjs_code_renderer*>(rend);
                      
                      if (wcr)
                        {
                          //one of us
                          if (wcr->check_fork(mthd))
                            {
                              if (wcr->forked())
                                {
                                  result = mthd;
                                  return true;
                                }
                            }
                          else
                            {
                              maybe_ = true;
                              result = mthd;
                              return true;
                            }
                        }
                    }
                  return asynch;
                }
            }

          return false;
        }
      
      
      bool forked(code& cde, CodeSplit& split)
        {
          wax_splitter splitter(ctx_, cde);
          cde.visit(&splitter);

          split = splitter.get(); 
          return split; 
        }

      void cut(XSSMethod mthd)
        {
          assert(!result_);

          found_ = true;
          
          result_ = CodeSplit(new code_split(code_));
          result_->split_idx = curr_;
          result_->method = mthd;
        }

      void cut(CodeSplit split)
        {
          assert(!result_);

          found_ = true;

          result_ = CodeSplit(new code_split(code_));
          result_->split_idx = curr_;
          result_->split_code = split;
        }
  };    

struct waxjs_internal_renderer : public js_code_renderer
  {
      waxjs_internal_renderer(code& cde, param_list_decl& params, XSSContext ctx) : 
        js_code_renderer(cde, params, ctx)
        {
        }

      virtual void return_(stmt_return& info)
        {
          std::ostringstream result;
          result << "\n" << "return_function(" << render_expression(info.expr, ctx_) << ");";
          result << "\n" << "return true;";
          
          add_line(result.str());
        }

      virtual str render_code(code& cde)
        {
          XSSContext ctx(new xss_context(ctx_));
          lang_utils::var_gatherer(cde, ctx);

          param_list_decl pld;
          waxjs_internal_renderer inner(cde, pld, ctx);
          str result = inner.render();
          add_line(result);
          return result;
        }
  };

//waxjs_lang
void waxjs_lang::init_application_context(XSSContext ctx)
  {
    ctx->register_xss_dsl("sql", XSSDSL(new dsl_sql));
    ctx->register_xss_dsl("shell", XSSDSL(new dsl_shell));
  }

variant waxjs_lang::compile_code(code& cde, param_list_decl& params, XSSContext ctx)
  {
    return reference<waxjs_code_renderer>(new waxjs_code_renderer(cde, params, ctx));
  }

//waxjs_code_renderer
waxjs_code_renderer::waxjs_code_renderer():
  checked_(false)
  {
  }

waxjs_code_renderer::waxjs_code_renderer(const waxjs_code_renderer& other):
  checked_(false),
  compiler_(other.compiler_),
  base_code_renderer(other)
  {
  }

waxjs_code_renderer::waxjs_code_renderer(code& cde, param_list_decl& params, XSSContext ctx):
  checked_(false),
  base_code_renderer(cde, params, ctx)
  {
    compiler_ = variant_cast<XSSCompiler>(ctx->resolve("compiler"), XSSCompiler()); assert(compiler_);
  }

str waxjs_code_renderer::render()
  {
    CodeSplit fork = fork_;
    if (!checked_)
      {
        wax_splitter splitter(ctx_, code_);
        code_.visit(&splitter);
        fork = splitter.get();
        fork_ = fork;
      }

    //check for goodies
    bool is_service = false;
    bool is_page = false;
    
    if (owner_)
      {
        is_service = owner_->get<bool>("wax_service", false);
        is_page = owner_->get<bool>("wax_page", false);
      }

    std::ostringstream result;

    if (is_service)
      result << render_service();

    if (is_page)
      result << render_page();

    if (is_service || is_page)
      result << render_parameters();

    if (!fork)
      result << render_code(code_);
    else
      result << render_split(fork, CodeSplit());
    
    if (is_page)
      result << "\nreturn_function();";

    return result.str();
  }

bool waxjs_code_renderer::check_fork(variant owner)
  {
    if (checked_)
      return true;

    checked_ = true;
    wax_splitter splitter(ctx_, code_);
    code_.visit(&splitter);
    fork_ = splitter.get();

    if (fork_)
      {
        XSSMethod mthd = variant_cast<XSSMethod>(owner, XSSMethod());
        if (mthd)
          mthd->add_property("asynch", true, XSSType());
      }

    if (splitter.maybe())
      {
        checked_ = false;
        return false;
      }

    return true;
  }

bool waxjs_code_renderer::forked()
  {
    return fork_;
  }

void waxjs_code_renderer::use_this_id(bool value)
  {
    ctx_->register_symbol(RESOLVE_CONST, "#use_this_id", value);
  }

str waxjs_code_renderer::render_expression(expression& expr, XSSContext ctx)
  {
    return lang_utils::render_expression<js_expr_renderer>(expr, ctx);
  }

str waxjs_code_renderer::render_code(code& cde)
  {
    XSSContext ctx(new xss_context(ctx_));
    lang_utils::var_gatherer(cde, ctx);

    str result = render_plain_code(cde, ctx);
    add_line(result);
    return result;
  }

str waxjs_code_renderer::render_plain_code(code& cde, XSSContext ctx)
  {
    param_list_decl pld;
    waxjs_internal_renderer inner(cde, pld, ctx);
    return inner.render();
  }

str waxjs_code_renderer::render_split(CodeSplit fork, CodeSplit parent)
  {
    if (parent)
      {
        //update the split call list
        fork->add.insert(fork->add.begin(), parent->add.begin(), parent->add.end());
      }

    //pre render, get access to variables
    if (!fork->context)
      {
        XSSContext parent_ctx = parent && parent->context? parent->context : ctx_;
        fork->context = XSSContext(new xss_context(parent_ctx));
        lang_utils::var_gatherer(fork->target, fork->context);
      }

    std::ostringstream result;

    int split_idx = fork->split_idx - 1;

    //render the code before the split
    code pre_split;
    fork->target.range(0, split_idx, pre_split);
    result << render_plain_code(pre_split, fork->context);

    //render the after code as a function, will be called on splits
    std::ostringstream code_after;
    str                split_name;

    code after_wards;
    fork->target.cut(fork->split_idx, after_wards);
    
    bool has_code_after = !after_wards.empty();    

    if (has_code_after)
      {
        split_name = compiler_->genid("callback");
        code_after << "function " << split_name << "() \n{\n";
        code_after << split_and_render(after_wards, fork);
        code_after << "\n}\n";

        split_name += "()";
        fork->add.insert(fork->add.begin(), split_name);
      }

    //then split
    variant& st = fork->target.get_stament(split_idx);
    if (st.is<stmt_if>())
      {
        result << code_after.str() << split_if(fork);
      }
    else if (st.is<stmt_variable>())
      {
        result << split_variable(fork, code_after.str());
      }
    else if (st.is<stmt_expression>())
      {
        result << code_after.str() << split_expression(fork);
      }
    else if (st.is<stmt_return>())
      {
        if (has_code_after)
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SCodeAfterReturn);
            xss_throw(error);
          }

        result << split_return(fork);
      }
    else if (st.is<dsl>())
      {
        result << code_after.str() << split_dsl(fork, split_name);
      }
    else
      {
        assert(false);
      }
      return result.str();
  }

str waxjs_code_renderer::split_if(CodeSplit fork)
  {
    std::ostringstream result;
    
    stmt_if stmnt = fork->target.get_stament(fork->split_idx - 1);
    if (fork->split_on_if)
      {
        result << "\nif (" << render_expression(stmnt.expr, ctx_) << ")";
        result << "\n{";

        result << render_split(fork->split_code, fork);

        if (!stmnt.else_code.empty())
          {
            result << "\nelse\n{\n";
            result << split_and_render(stmnt.else_code, fork);
            result << "\n}";
          }
      }
    else if (fork->split_on_else)
      {
        result << "\nif (" << render_expression(stmnt.expr, ctx_) << ")";
        result << "\n{";

        result << render_plain_code(stmnt.if_code, fork->context);
        
        result << after_code(fork) << "\n}";
        result << "else\n{\n";
        result << render_split(fork->split_code, fork);
        result << "\n}";
      }
    else
      {
        //last use case, the split happens on the if expression, 
        //the key to the algo here is that code gets split only once
        result << split_method(fork->method);
        result << "\n{\n";
        result << "if (return_value)";
        result << "\n{\n";

        result << split_and_render(stmnt.if_code, fork);

        result << "\n}";

        if (!stmnt.else_code.empty())
          {
            result << "else\n{\n";
            result << split_and_render(stmnt.else_code, fork);
            result << "\n}";
          }
        else
            result << after_code(fork);

        result << "\n});";
      }

    return result.str();
  }

str waxjs_code_renderer::after_code(CodeSplit fork)
  {
    std::ostringstream result;

    std::vector<str>::iterator it = fork->add.begin();
    std::vector<str>::iterator nd = fork->add.end();

    size_t sz = fork->add.size();
    size_t i  = 0;
    for(; it != nd; it++, i++)
      {
        result <<  "\n";
        if (i < sz - 1)
          result << "if (" << *it << ")\nreturn;"; 
        else
          result << *it << ";";
      }

    return result.str();
  }

str waxjs_code_renderer::render_page()
  {
    str pre_code = owner_->get<str>("pre_code", str());
    str rf       = owner_->get<str>("return_function", str());

    std::ostringstream result;
    result << pre_code << "\n";
    result << "\nfunction return_function()";
    result << "\n{";
    result << "\n" << rf;
    result << "\n}";
    return result.str();
  }

str waxjs_code_renderer::render_parameters()
  {
    std::ostringstream result;

    DynamicArray params = variant_cast<DynamicArray>(dynamic_get(owner_, "#wax_parameters"), DynamicArray());
    if (params)
      {
        result << "\nvar __params = url.parse(request.url, true).query;";

        std::vector<variant>::iterator it = params->ref_begin();
        std::vector<variant>::iterator nd = params->ref_end();

        for(; it != nd; it++)
          {
            str param = variant_cast<str>(*it, str()); assert(!param.empty());
            result << "\nvar " << param << " = __params." << param << ";";
          }
      }

      return result.str();
  }

str waxjs_code_renderer::render_service()
  {
    std::ostringstream result;
    result << "\nfunction return_function(return_value)";
    result << "\n{";
    result << "\nreqest.end(JSON.stringify(return_value))";
    result << "\n}";
    return result.str();
  }

str waxjs_code_renderer::split_variable(CodeSplit fork, const str& code_after)
  {
    stmt_variable stmnt = fork->target.get_stament(fork->split_idx - 1);
    
    std::ostringstream result;
    result << "var " << stmnt.id << ";\n";
    result << code_after;
    result << split_method(fork->method);
    result << "\n{\n";
    result << stmnt.id << " = return_value;";
    result << after_code(fork);
    result << "\n});\n";

    return result.str();
  }

str waxjs_code_renderer::split_expression(CodeSplit fork)
  {
    stmt_expression stmnt = fork->target.get_stament(fork->split_idx - 1);

    operator_type op;
    bool assign = false;
    if (stmnt.expr.top_operator(op))
      {
        switch(op)
          {
						case op_assign:
            case op_plus_equal:
            case op_minus_equal:
            case op_mult_equal:
            case op_div_equal:
            case op_shift_right_equal:
            case op_shift_left_equal:
							{
                assign = true;
                break;
              }
            case op_call:
              break;
            default:
              assert(false);
          }
      }

    std::ostringstream result;
    result << split_method(fork->method);
    result << "\n{\n";

    if (assign)
      {
        expression_splitter es(op);
        stmnt.expr.visit(&es);
        result << render_expression(es.left, ctx_) << " " << lang_utils::operator_string(op) << " return_value;";

        result << after_code(fork);
      }
    else
      result << after_code(fork);
    
    result << "\n});";

    return result.str();
  }

str waxjs_code_renderer::split_dsl(CodeSplit fork, const str& callback)
  {
    dsl dd = fork->target.get_stament(fork->split_idx - 1);
    XSSDSL dsl = ctx_->get_xss_dsl(dd.name);
    if (!dsl)
      {
        param_list error;
        error.add("id", SLanguage);
        error.add("desc", SUnknownDSL);
        error.add("dsl", dd.name);
        xss_throw(error);
      }

    XSSContext ctx(new xss_context(ctx_));
    ctx->register_symbol(RESOLVE_CONST, "#wax_callback", callback);

    return dsl->render(dd, ctx);
  }

str waxjs_code_renderer::split_return(CodeSplit fork)
  {
    stmt_return stmnt = fork->target.get_stament(fork->split_idx - 1);

    std::ostringstream result;
    result << split_method(fork->method);
    result << "\n{\n";
    result << "return_function(return_value);";
    result << "\n});";

    return result.str();
  }

str waxjs_code_renderer::split_and_render(code& c, CodeSplit parent)
  {
    wax_splitter splitter(ctx_, c);
    c.visit(&splitter);

    CodeSplit fork = splitter.get();
    if (fork)
      return render_split(fork, parent);

    //no split, just render the code and call the splits
    std::ostringstream result;

    XSSContext parent_ctx = parent && parent->context? parent->context : ctx_;
    XSSContext ctx(new xss_context(parent_ctx));
    lang_utils::var_gatherer(c, ctx);

    result << render_plain_code(c, ctx);

    if (parent)
      result << after_code(parent); 
    
    return result.str();
  }

str waxjs_code_renderer::split_method(XSSMethod method)
  {
    std::ostringstream result;
    result << method->output_id() << "(function(return_value)";

    return result.str();
  }

//wax_utils
wax_utils::wax_utils() 
  {
  }

wax_utils::wax_utils(XSSCompiler compiler):
  compiler_(compiler)
  {
  }

class object_45 : public xss_object
  {
		public:
      object_45(){}
		public:
      //IDinamicObject
      virtual bool resolve(const str& name, schema_item& result)
        {
          if (!has(name))
            {
              std::vector<str>::iterator it = modifiers_.begin();
              std::vector<str>::iterator nd = modifiers_.end();

              for(; it != nd; it++)
                {
                  if (*it == name)
                    return false;
                }

              modifiers_.push_back(name);
              return false;
            }

          return xss_object::resolve(name, result);
        }
    public:
      bool modified()
        {
          return !modifiers_.empty();
        }

      std::vector<str>& modifiers()
        {
          return modifiers_;
        }
    private:
      std::vector<str> modifiers_;
  };

typedef reference<object_45> Object45; 

XSSMethod wax_utils::compile_page(XSSObject page, variant code_renderer)
  {
    //do the html thing
    str page_data = page->get<str>("src", str()); 
    str html_text = compiler_->file(page_data);
    html_parser parser;
    tag_list    tags;
    if (!parser.parse(html_text, tags))
      {
        param_list error;
        error.add("id", SLanguage);
        error.add("desc", SBadHTML);
        error.add("file", page_data);
        xss_throw(error);
      }

    //grab the code
    waxjs_code_renderer* cr = variant_cast<waxjs_code_renderer*>(code_renderer, null); assert(cr);

    js_args_renderer* args = new js_args_renderer();
    args->add("request",  XSSType());
    args->add("response", XSSType());
    
    //create a method to be rendered later
    str page_id = page->get<str>("id", str()); 
    XSSMethod result(new xss_method(page_id, XSSType(), reference<js_args_renderer>(args), code_renderer));
    result->add_attribute("asynch",   true);
    result->add_attribute("wax_page", true);

    cr->owner_ = result;

    //here comes tricky, every registered element will get its own instance
    //then the code will be analyzed to find the changes intended for the html
    XSSContext ctx(new xss_context(compiler_->current_context()));
    ctx->search_native(true); //looks important...

    DynamicArray elements = page->children();
		std::vector<variant>::iterator it = elements->ref_begin();
		std::vector<variant>::iterator nd = elements->ref_end();

    std::vector<Object45> items;
    std::vector<int>      tag_idxs;
    std::map<int, int>    sort;
    for(; it != nd; it++)
      {
        Object45  obj(new object_45);
        XSSObject elem = *it;
        str       eid  = elem->id();
        int       idx  = tags.find(eid);

        obj->set_id(eid);
        if (idx < 0)
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SMissingHTMLTag);
            error.add("tag", eid);
            xss_throw(error);
          }

        tag_idxs.push_back(idx);
        items.push_back(obj);

        tag& tt = tags.get(idx);
        sort.insert(std::pair<int, int>(tt.start, items.size()));

        //let the script know
        ctx->register_symbol(RESOLVE_INSTANCE, eid, obj);
      }

    //then, masterfully I must say, I just have to apply any 
    //serious visitor and the properties the user intends to modify
    //get attached to our 45

    //in this case we just inquire the return type
    code& cde = cr->get_code();
    
    code_type_resolver tr(ctx, cde);
    cde.visit(&tr);

    //wait, not over yet... now we must split the html along the modifiers,
    //and generate the the oh sweet code.
    std::map<int, int>::iterator sit = sort.begin();
    std::map<int, int>::iterator snd = sort.end();

    size_t             curr = 0;
    std::ostringstream return_function;
    std::ostringstream declarations;

    for(; sit != snd; sit++)
      {
        int      idx = sit->second - 1;
        tag&     tt  = tags.get(tag_idxs[idx]);
        Object45 obj = items[idx];

        if (obj->modified())
          {
            declarations << "var " << obj->id() << " = {};\n";

            //it has been touched by the user, otherwise just leave it as it was
            return_function << render_html_text(html_text.substr(curr, tt.start - curr));

            std::vector<str>& modifiers = obj->modifiers();
            prop_list         pl        = tt.props;

            std::vector<str>::iterator mit = modifiers.begin();
            std::vector<str>::iterator mnd = modifiers.end();
            for(; mit != mnd; mit++)
              {
                str modifier = *mit;

                //delete from the property list if it was there
                prop_list::iterator plit = pl.find(modifier);
                if (plit != pl.end())
                    pl.erase(plit);
                
                if (!custom_modifier(XSSObject(obj), modifier, tags, tt))
                  {
                    //add the runtime value as an attribute
                    return_function << "response.write(\"<" << tt.tag_name << " " << modifier << " = \");\n";
                    return_function << "response.write('\\\"' + " << obj->id() << "." << modifier << " + '\\\"' );\n";
                    return_function << "response.write(\"";

                    prop_list::iterator pit = pl.begin();
                    prop_list::iterator pnd = pl.end();

                    for(; pit != pnd; pit++)
                      {
                        return_function << pit->first << " = \\\"" << pit->second << "\\\"";
                      }

                    return_function << "\");\n";
                  }
              }
          }

        curr = tt.close;
      }

    return_function << render_html_text(html_text.substr(curr, html_text.size() - curr));

    result->add_attribute("pre_code",        declarations.str());
    result->add_attribute("return_function", return_function.str());
    return result;
  }

void wax_utils::pre_process_args(XSSMethod methd)
  {
    IArgumentRenderer* rend = variant_cast<IArgumentRenderer*>(methd->get_parameters(), null); assert(rend);
    
    //copy arguments and nuke
    param_list_decl args = rend->get();
    rend->get().clear();

    //add standard service/page parameters
    param_decl pd;
    pd.name = "response";
    pd.type = "object";

    rend->get().push_back(pd);

    pd.name = "request";
    rend->get().push_back(pd);

    //remember the original ones
    DynamicArray wax_args(new dynamic_array);

    param_list_decl::iterator it = args.begin();
    param_list_decl::iterator nd = args.end();

    for(; it != nd; it++)
      {
         wax_args->push_back(it->name);
      }

    methd->add_attribute("#wax_parameters", wax_args);
  }

bool wax_utils::custom_modifier(XSSObject obj, const str& modifier, tag_list& tags, tag& t)
  {
    if (obj->has("replicator") && modifier == "items")
      {
        //td: !!! replicator
      }

    return false;
  }

str wax_utils::render_html_text(const str& text)
  {
    str result = text;
    boost::replace_all (result, "\n", "\\n");
    boost::replace_all (result, "\"", "\\\"");
    return "response.write(\"" + result + "\");\n";
  }

//glue
struct object_45_schema : xss_object_schema<object_45>
  {
    virtual void declare()
      {
				xss_object_schema<object_45>::declare();

				inherit_from<xss_object>();
      }
  };

register_complete_type(object_45, object_45_schema);
