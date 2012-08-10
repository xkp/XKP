#include <xss/lang/waxjs.h>
#include <xss/dsl/sql.h>
#include <xss/dsl/shell.h>
#include <xss/dsl/smarty.h>
#include <xss/xss_renderer.h>
#include <xss/brace_parser.h>

#include <boost/algorithm/string/replace.hpp>

using namespace xkp;

const str SLanguage("language");

const str SCannotForkOnIteration("Cannot perform asynchronic operations inside loops");
const str SCodeAfterReturn("Unreachable code");
const str SBadHTML("Errors parsing html");
const str SMissingHTMLTag("Missing declared tag");
const str SUnknownDSL("Unkown dsl");
const str SMissingReplicatorEvent("Replicators expect a 'render' event");
const str STooManyReplicatorEvents("Replicators expect only one 'render' event");
const str SReplicatorOnlyData("Replicators only contain the 'data' property");
const str SReplicatorNoFork("Replicators 'render' event does not support offline operations");
const str SMultipleCatchesNotImplemented("Multiple catch staments are not implemented");
const str SPageDoesNotReturn("pages can only return a redirect");
const str SRedirectToSmartyExpectsFile("redirect to smarty expects a file");
const str SRedirectToPageExpectsFile("redirect to page expects a valid page name");
const str SRedirectExpectsPageOrTpl("redirect expects either 'page' or 'tpl'");

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
        if (forked(info.cond_expr, mthd) || forked(info.iter_expr, mthd) || forked(info.init_expr, mthd))
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SCannotForkOnIteration);
            error.add("comment", str("dont get cute"));
            xss_throw(error);
          }

        CodeSplit result;
        if (forked(info.for_code, result))
          {
            cut(result);
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

        CodeSplit result;
        if (forked(info.for_code, result))
          {
            cut(result);
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

        CodeSplit result;
        if (forked(info.while_code, result))
          {
            cut(result);
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
        CHECK_ACTIVE
      }
    
    virtual void switch_(stmt_switch& info)
      {
        assert(false); //ditto
      }

    virtual void try_(stmt_try& info)
      {
        CHECK_ACTIVE

        CodeSplit result;
        if (forked(info.try_code, result))
          {
            cut(result);
          }
      }

    virtual void throw_(stmt_throw& info)
      {
        CHECK_ACTIVE
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
          bool is_page = variant_cast<bool>(ctx_->resolve("#wax_page"), false);
          bool is_service = variant_cast<bool>(ctx_->resolve("#wax_service"), false);

          std::ostringstream result;
          if (is_page)
            {
              param_list args;
              if (wax_utils::check_redirect(info.expr, ctx_, args))
                {
                  if (args.has("tpl"))
                    {
                      str tpl_file;
                      expression value = args.get("tpl");
                      if (value.size() == 1)
                        tpl_file = variant_cast<str>(value.first(), str());

                      if (tpl_file.empty())
                        {
                          param_list error;
                          error.add("id", SLanguage);
                          error.add("desc", SRedirectToSmartyExpectsFile);
                          xss_throw(error);
                        }
                    
                      XSSCompiler compiler = ctx_->resolve("compiler");
                      str tpl_contents = compiler->file(tpl_file);

                      //td: generalize all this
                      XSSDSL dsl = ctx_->get_xss_dsl("smarty");
                      dsl_smarty* smarty = dynamic_cast<dsl_smarty*>(dsl.get()); assert(smarty); 
                      result << "\nresponse.end(" << smarty->gen_call(tpl_contents, ctx_, args) << ");";
                    }
                  else if (args.has("page"))
                    {
                      str page;
                      expression value = args.get("page");
                      if (value.size() == 1)
                        page = variant_cast<str>(value.first(), str());

                      if (page.empty())
                        {
                          param_list error;
                          error.add("id", SLanguage);
                          error.add("desc", SRedirectToPageExpectsFile);
                          xss_throw(error);
                        }

                      result << "\napplication." << page << "(request, response);";
                    }
                  else
                    {
                      param_list error;
                      error.add("id", SLanguage);
                      error.add("desc", SRedirectExpectsPageOrTpl);
                      xss_throw(error);
                    }
                  
                  result << "\nreturn true;";
                }
            }
          else
            {
              result << "\n" << "return_function(" << render_expression(info.expr, ctx_) << ");";
              result << "\n" << "return true;";
            }
          
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
    
    variant v = new dsl_smarty;
    ctx->register_xss_dsl("smarty", XSSDSL(new dsl_smarty));
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
    lang_utils::var_gatherer(cde, ctx_);
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

    ctx_->register_symbol(RESOLVE_CONST, "#wax_page", is_page, true);
    ctx_->register_symbol(RESOLVE_CONST, "#wax_service", is_service, true);

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
    //pre render, get access to variables
    if (!fork->context)
      {
        XSSContext parent_ctx = parent && parent->context? parent->context : ctx_;
        fork->context = XSSContext(new xss_context(parent_ctx));
        lang_utils::var_gatherer(fork->target, fork->context);
      }

    //inherit error_handler
    if (fork->error_handler.empty() && parent)
      fork->error_handler = parent->error_handler;

    std::ostringstream result;

    int split_idx = fork->split_idx - 1;

    //render the code before the split
    code pre_split;
    fork->target.range(0, split_idx, pre_split);
    result << render_plain_code(pre_split, fork->context);

    //render the after code as a function, will be called on splits
    std::ostringstream code_after;
    str                split_callback;

    code after_wards;
    fork->target.cut(fork->split_idx, after_wards);
    
    bool has_code_after = !after_wards.empty();    

    if (has_code_after || !fork->hook.empty())
      {
        split_callback = compiler_->genid("callback");

        code_after << "function " << split_callback << "() \n{\n";
        bool has_error_handler = !fork->error_handler.empty();
        if (has_error_handler)
          {
            code_after << "\n\ttry";
            code_after << "\n\t{";
          }

        bool split;
        code_after << split_and_render(after_wards, fork, fork->callback, &split);
        if (!split && !fork->hook.empty())
          code_after << fork->hook << "();";

        if (has_error_handler)
          {
            code_after << "\n\t}";
            code_after << "\n\tcatch(__error)";
            code_after << "\n\t{";
            code_after << "\n\t\t" << fork->error_handler << "(__error);";
            code_after << "\n\t}";
          }

        code_after << "\n}\n";
      }
    else if (!fork->callback.empty())
      split_callback = fork->callback;

    //then split
    variant& st = fork->target.get_stament(split_idx);
    if (st.is<stmt_if>())
      {
        result << code_after.str() << split_if(fork, split_callback);
      }
    else if (st.is<stmt_variable>())
      {
        result << split_variable(fork, code_after.str(), split_callback);
      }
    else if (st.is<stmt_expression>())
      {
        result << code_after.str() << split_expression(fork, split_callback);
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
        result << code_after.str() << split_dsl(fork, split_callback);
      }
    else if (st.is<stmt_for>())
      {
        result << code_after.str() << split_for(fork, split_callback);
      }
    else if (st.is<stmt_iter_for>())
      {
        result << code_after.str() << split_iter_for(fork, split_callback);
      }
    else if (st.is<stmt_while>())
      {
        result << code_after.str() << split_while(fork, split_callback);
      }
    else if (st.is<stmt_try>())
      {
        result << code_after.str() << split_try(fork, split_callback);
      }
    else
      {
        assert(false);
      }
      return result.str();
  }

str waxjs_code_renderer::split_if(CodeSplit fork, const str& callback)
  {
    if (fork->split_code)
      fork->split_code->callback = callback;

    std::ostringstream result;
    
    stmt_if stmnt = fork->target.get_stament(fork->split_idx - 1);
    if (fork->split_on_if)
      {
        result << "\nif (" << render_expression(stmnt.expr, ctx_) << ")";
        result << "\n{";
        result << render_split(fork->split_code, fork);
        result << "\n}";

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
        
        result << "\n" << render_callback(fork) << "\n}";
        result << "\nelse\n{\n";
        result << render_split(fork->split_code, fork);
        result << "\n}";
      }
    else
      {
        //last use case, the split happens on the if expression, 
        //the key to the algo here is that code gets split only once
        std::ostringstream method_body;
        method_body << "\n\tif (return_value)";
        method_body << "\n\t{\n";
        method_body << split_and_render(stmnt.if_code, fork);
        method_body << "\n\t}";
        if (!stmnt.else_code.empty())
          {
            method_body << "\n\telse\n{\n";
            method_body << split_and_render(stmnt.else_code, fork);
            method_body << "\n\t}";
          }
        else
            method_body << render_callback(fork);

        result << split_method(fork->method, fork, compile_args(stmnt.expr), method_body.str());
      }

    return result.str();
  }

str waxjs_code_renderer::render_callback(CodeSplit fork)
  {
    if (!fork || fork->callback.empty())
      return str();

    return fork->callback + "()";
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
    result << "\nresponse.end();";
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
    result << "\nresponse.end(JSON.stringify(return_value))";
    result << "\n}";
    return result.str();
  }

str waxjs_code_renderer::split_variable(CodeSplit fork, const str& code_after, const str& callback)
  {
    stmt_variable stmnt = fork->target.get_stament(fork->split_idx - 1);
    
    std::ostringstream result;
    result << "var " << stmnt.id << ";\n";
    result << code_after;

    std::ostringstream method_body;
    method_body << stmnt.id << " = return_value;";
    method_body << "\n" << callback << "();";
    result << split_method(fork->method, fork, compile_args(stmnt.value), method_body.str());
    return result.str();
  }

DynamicArray waxjs_code_renderer::compile_args(expression& expr)
  {
    DynamicArray result(new dynamic_array);

    expr_param_resolver params;
    expr.visit(&params);

    param_list& pl = params.get();

    for(size_t i = 0; i < pl.size(); i++)
      {
        str name = pl.get_name(i);
        expression value = pl.get(i);

        str value_str = render_expression(value, ctx_);
        XSSObject param(new xss_object);
        param->set_id(name);
        param->add_attribute("value", value_str);

        result->push_back(param);
      }

    return result;
  }

str waxjs_code_renderer::split_expression(CodeSplit fork, const str& callback)
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

    std::ostringstream method_body;
    if (assign)
      {
        expression_splitter es(op);
        stmnt.expr.visit(&es);
        method_body << render_expression(es.left, ctx_) << " " << lang_utils::operator_string(op) << " return_value;";
      }

    method_body << callback << "()"; //render_callback(fork);

    return split_method(fork->method, fork, compile_args(stmnt.expr), method_body.str());
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
    ctx->register_symbol(RESOLVE_CONST, "#wax_callback", callback + "()");

    if (!fork->error_handler.empty())
      ctx->register_symbol(RESOLVE_CONST, "#wax_error_handler", fork->error_handler);

    return dsl->render(dd, ctx);
  }

str waxjs_code_renderer::split_return(CodeSplit fork)
  {
    stmt_return stmnt = fork->target.get_stament(fork->split_idx - 1);

    std::ostringstream method_body;
    method_body << "\n{\n";
    method_body << "return_function(return_value);";
    method_body << "\n});";

    return split_method(fork->method, fork, compile_args(stmnt.expr), method_body.str());
  }

str waxjs_code_renderer::split_for(CodeSplit fork, const str& callback)
  {
    stmt_for stmnt = fork->target.get_stament(fork->split_idx - 1);

    std::ostringstream result;

    if (stmnt.init_variable.empty())
      result << "\n" << render_expression(stmnt.init_expr, ctx_) << ";";
    else
      {
        result << "\nvar " << stmnt.init_variable.id;
        if (!stmnt.init_variable.value.empty())
          result << " = " << render_expression(stmnt.init_variable.value, ctx_);
        result << ";";
      }
    
    //add a function to check do the loop iteration
    str cond_cb = compiler_->genid("for_cond");
    str iter_cb = compiler_->genid("for_iter");
    str code_cb = compiler_->genid("for_code");

    //replace the callback in the stack by our condition, 
    //this will be the hook for loop continuation
    fork->split_code->hook = iter_cb;

    //render the parts of the for as functions
    result << "function " << iter_cb << "()";
    result << "\n{\n";
    result << "\n\t" << render_expression(stmnt.iter_expr, ctx_) << ';';
    result << "\n\t" << cond_cb << "();";
    result << "\n}\n";

    result << "function " << code_cb << "()";
    result << "\n{\n";
    result << render_split(fork->split_code, fork);
    result << "\n}";


    result << "function " << cond_cb << "()";
    result << "\n{";
    result << "\n\tif (" << render_expression(stmnt.cond_expr, ctx_) << ")";
    result << "\n\t" << code_cb << "();";
    result << "\n\telse";
    result << "\n\t" << callback << "();";
    result << "\n}\n";

    result << "\n" << cond_cb << "();\n";

    return result.str();
  }

str waxjs_code_renderer::split_iter_for(CodeSplit fork, const str& callback)
  {
    stmt_iter_for stmnt = fork->target.get_stament(fork->split_idx - 1);

    std::ostringstream result;

    str iterable_name = stmnt.id + "_iterable";
    str iterator_name = stmnt.id + "_iterator";

    result << "\nvar " << iterable_name << " = " << render_expression(stmnt.iter_expr, ctx_) << ";";
    result << "\nvar " << iterator_name << " = 0;";
    
    //add a function to check do the loop iteration
    str cond_cb = compiler_->genid("for_cond");
    str iter_cb = compiler_->genid("for_iter");
    str code_cb = compiler_->genid("for_code");

    //replace the callback in the stack by our condition, 
    //this will be the hook for loop continuation
    fork->split_code->hook = iter_cb;

    //render the parts of the for as functions
    result << "\nfunction " << code_cb << "()";
    result << "\n{\n";
    result << "\n\tvar " << stmnt.id << " = " << iterable_name << "[" << iterator_name << "]" << ';';
    result << render_split(fork->split_code, fork);
    result << "\n}";

    result << "\nfunction " << iter_cb << "()";
    result << "\n{\n";
    result << "\n\t" << iterator_name << "++;";
    result << "\n\t" << cond_cb << "();";
    result << "\n}\n";

    result << "\nfunction " << cond_cb << "()";
    result << "\n{";
    result << "\n\tif (" << iterator_name << " < " << iterable_name << ".length" << ")";
    result << "\n\t" << code_cb << "();";
    result << "\n\telse";
    result << "\n\t" << callback << "();";
    result << "\n}\n";

    result << "\n" << cond_cb << "();\n";

    return result.str();
  }

str waxjs_code_renderer::split_while(CodeSplit fork, const str& callback)
  {
    stmt_while stmnt = fork->target.get_stament(fork->split_idx - 1);

    std::ostringstream result;

    //add a function to check do the loop iteration
    str cond_cb = compiler_->genid("while_cond");
    str code_cb = compiler_->genid("while_code");

    //replace the callback in the stack by our condition, 
    //this will be the hook for loop continuation
    fork->split_code->hook = cond_cb;

    //render the parts of the for as functions
    result << "\nfunction " << code_cb << "()";
    result << "\n{\n";
    result << render_split(fork->split_code, fork);
    result << "\n}";

    result << "\nfunction " << cond_cb << "()";
    result << "\n{";
    result << "\n\tif (" << render_expression(stmnt.expr, ctx_) << ")";
    result << "\n\t" << code_cb << "();";
    result << "\n\telse";
    result << "\n\t" << callback << "();";
    result << "\n}\n";

    result << "\n" << cond_cb << "();\n";

    return result.str();
  }

str waxjs_code_renderer::split_try(CodeSplit fork, const str& callback)
  {
    std::ostringstream result;
    stmt_try stmnt = fork->target.get_stament(fork->split_idx - 1);

    if (!stmnt.finally_code.empty())
      {
        str finally_fn = compiler_->genid("finally");
        fork->split_code->hook = finally_fn;
        result << "\nfunction " << finally_fn << "()";
        result << "\n{";
        result << split_and_render(stmnt.finally_code, fork, callback);
        result << "\n}";
      }

    str catch_fn = compiler_->genid("catch");
    result << "\nfunction " << catch_fn << "(__error)";
    result << "\n{";

    if (stmnt.catches.size() > 1)
      {
        std::vector<catch_>::iterator it = stmnt.catches.begin();
        std::vector<catch_>::iterator nd = stmnt.catches.end();
        for(; it != nd; it++)
          {
            //td: !!!
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SMultipleCatchesNotImplemented);
            xss_throw(error);
          }
      }
    else
      {
        catch_& the_catch = stmnt.catches[0];
        str catch_id;
        if (!the_catch.id.empty())
          catch_id = the_catch.id;
        else if (!the_catch.type.name.empty())
          catch_id = the_catch.type.name; //td: !!!

        if (!catch_id.empty())
          result << "\n var " << catch_id << " = __error;";
        
        result << split_and_render(the_catch.catch_code, fork, callback);

        fork->split_code->error_handler = catch_fn;
      }
    result << "\n}";

    fork->split_code->callback = callback;

    result << "\ntry";
    result << "\n{";
    result << render_split(fork->split_code, fork);
    result << "\n}";
    result << "\ncatch(__error)";
    result << "\n{";
    result << "\n\t" << catch_fn << "(__error)";
    result << "\n}";

    return result.str();
  }

str waxjs_code_renderer::split_and_render(code& c, CodeSplit parent, const str& callback, bool* split)
  {
    wax_splitter splitter(ctx_, c);
    c.visit(&splitter);

    CodeSplit fork = splitter.get();
    if (split)
      *split = fork;

    if (fork)
      {
        fork->hook = parent? parent->hook : str();
        fork->callback = callback;
        if (callback.empty())
          fork->callback = parent->callback;

        if (fork->error_handler.empty())
          fork->error_handler = parent? parent->error_handler : str();

        return render_split(fork, parent);
      }

    //no split, just render the code and call the splits
    std::ostringstream result;

    XSSContext parent_ctx = parent && parent->context? parent->context : ctx_;
    XSSContext ctx(new xss_context(parent_ctx));
    lang_utils::var_gatherer(c, ctx);

    result << render_plain_code(c, ctx);

    if (!callback.empty())
      result << "\n" << callback << "();"; 
    
    return result.str();
  }

str waxjs_code_renderer::split_method(XSSMethod method, CodeSplit fork, DynamicArray args, const str& body)
  {
    XSSCompiler compiler = ctx_->resolve("compiler");
    
    str render_file = method->get<str>("renderer", str());
    if (!render_file.empty())
      {
        render_file = compiler->idiom_path(XSSObject(ctx_->get_type("web_service")), render_file);
        
        XSSRenderer renderer(new xss_renderer(compiler, ctx_, ctx_->path()));
        compiler->push_renderer(renderer);

        //let subclasses add parameters
        param_list pl;
        pl.add(render_file);
        pl.add("it", method);
        pl.add("args", args);
        pl.add("body", body);

        if (fork && !fork->error_handler.empty())
          {
            pl.add("error_handler", fork->error_handler);
          }

        compiler->xss(pl);

        compiler->pop_renderer();
        return renderer->get();
      }
    else
      {
        std::ostringstream result;
        result << method->output_id() << "(";

        //args
        //td: !!! data model
        XSSObjectList methd_args = method->find_by_class("parameter");
        XSSObjectList::iterator it = methd_args.begin();
        XSSObjectList::iterator nd = methd_args.end();
        int idx = 0;
        for(; it != nd; it++, idx++)
          {
            if (idx < args->size())
              {
                XSSObject arg = args->at(idx); 
                result << arg->get<str>("value", str());
              }
            else 
                result << (*it)->get<str>("default_value", str());

            result << ", ";
          }

        result << "function(" << method->get<str>("callback_args", str()) << ")";
        result << "\n{";

        str error_arg = method->get<str>("error_arg", str());
        if (!error_arg.empty())
          {
            result << "\n\tif (" << error_arg << ")";
            if (!fork->error_handler.empty())
              result << "\n\t{\n\t\t" << fork->error_handler << "(" << error_arg << "); \n\t\t return; \n\t}";
            else 
              result << "\n\t\t" << "throw " << error_arg << ";";
          }

        result << body;
        result << "\n});";
        return result.str();
      }
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

struct visitor_45 : code_visitor
  {
    visitor_45(XSSContext ctx):
      ctx_(ctx)
      {
      }

    //code_visitor
    virtual void expression_(stmt_expression& info)
      {
        //we're only looking for expression in the form: element.stuff = modifier
		    
        operator_type op;
		    if (info.expr.top_operator(op) && op == op_assign)
          {
            expression_splitter assign_splitter(op);
            info.expr.visit(&assign_splitter);

            expression& left = assign_splitter.left;
            if ((left.size() == 3) && left.top_operator(op) && op == op_dot)
              {
                //we only need to resolve the modifier, the object 45 will register it
                expr_type_resolver tr(ctx_);
                left.visit(&tr);
              }
          }
      }

    virtual void if_(stmt_if& info)
      {
        visitor_45 v45(ctx_);
        info.if_code.visit(&v45);
        info.else_code.visit(&v45);
      }

    virtual void for_(stmt_for& info)
      {
        visitor_45 v45(ctx_);
        info.for_code.visit(&v45);
      }

    virtual void iterfor_(stmt_iter_for& info)
      {
        visitor_45 v45(ctx_);
        info.for_code.visit(&v45);
      }

    virtual void while_(stmt_while& info)
      {
        visitor_45 v45(ctx_);
        info.while_code.visit(&v45);
      }

    virtual void try_(stmt_try& info)
      {
        visitor_45 v45(ctx_);
        info.try_code.visit(&v45);
      }

    virtual void return_(stmt_return& info)
      {
        bool is_page = variant_cast<bool>(ctx_->resolve("#wax_page"), false);
        param_list args;
        if (is_page && wax_utils::check_redirect(info.expr, ctx_, args))
          {
            if (args.has("tpl"))
              {
                str tpl_file;
                expression value = args.get("tpl");
                if (value.size() == 1)
                  tpl_file = variant_cast<str>(value.first(), str());

                if (tpl_file.empty())
                  {
                    param_list error;
                    error.add("id", SLanguage);
                    error.add("desc", SRedirectToSmartyExpectsFile);
                    xss_throw(error);
                  }
                    
                XSSCompiler compiler = ctx_->resolve("compiler");
                str tpl_contents = compiler->file(tpl_file);

                //td: generalize all this
                XSSDSL dsl = ctx_->get_xss_dsl("smarty");
                dsl_smarty* smarty = dynamic_cast<dsl_smarty*>(dsl.get()); assert(smarty); 
                smarty->pre_process_file(tpl_contents, ctx_);
              }
          }
        else
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SPageDoesNotReturn);
            xss_throw(error);
          }
      }

    virtual void dsl_(dsl& info)
      {
        XSSDSL dsl = ctx_->get_xss_dsl(info.name);
        if (dsl)
            dsl->pre_process(info, ctx_);
      }

    virtual void variable_(stmt_variable& info)       {}
    virtual void break_()                             {}
    virtual void continue_()                          {}
    virtual void dispatch(stmt_dispatch& info)        {}
    virtual void switch_(stmt_switch& info)           {}
    virtual void throw_(stmt_throw& info)             {}

    private:
      XSSContext ctx_;
      XSSObject  result_;
  }; 

XSSMethod wax_utils::compile_page(XSSObject page, variant code_renderer)
  {
    //do the html thing
    str html_text;
    
    str src = page->get<str>("application", str()); 
    if (!src.empty())
      {
        JsonOutput json; 
		
        XSSCompiler compiler(new xss_compiler(&json));
        fs::path app_path = fs::path(compiler_->project_path()) / src;
        param_list pl;
        compiler->no_ouput();
        compiler->build(app_path, pl);
        html_text = compiler->get_result();
      }
    else
      {
        src = page->get<str>("src", str()); 
        html_text = compiler_->file(src);
      }

    if (html_text.empty())
      {
        param_list error;
        error.add("id", SLanguage);
        error.add("desc", SBadHTML);
        error.add("file", src);
        xss_throw(error);
      }

    html_parser parser;
    tag_list    tags;
    if (!parser.parse(html_text, tags))
      {
        param_list error;
        error.add("id", SLanguage);
        error.add("desc", SBadHTML);
        error.add("file", src);
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
    ctx->register_symbol(RESOLVE_CONST, "#wax_page", true);

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

        obj->copy(elem);
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

    //then, masterfully I must say, I must go looking for assigns to the elements 
    code& cde = cr->get_code();
    
    visitor_45 tr(ctx);
    cde.visit(&tr);

    //use case, some dsls may add modifiers, so lets take that into account
    DynamicArray extra_modifiers = variant_cast<DynamicArray>(ctx->resolve("#extra_modifiers"), DynamicArray());
    if (extra_modifiers)
      {
        std::vector<variant>::iterator eit = extra_modifiers->ref_begin();
        std::vector<variant>::iterator end = extra_modifiers->ref_end();

        for(; eit != end; eit++)
          {
            XSSObject em = *eit;
            str html_id  = em->get<str>("html_id", str());
            str modifier = em->get<str>("modifier", str());

            if (!html_id.empty() && !modifier.empty())
              {
                std::vector<Object45>::iterator oit = items.begin();
                std::vector<Object45>::iterator ond = items.end();

                for(; oit != ond; oit++)
                  {
                    Object45 o45 = *oit;
                    if (o45->id() == html_id)
                      {
                        o45->modifiers().push_back(modifier);
                        break;
                      }
                  }
              }
          }
      }


    //on pages the last intruction will be a call to the return_function
    stmt_expression rfe;
    xs_compiler xsc;
    xsc.compile_expression("return_function()", rfe.expr);
    cde.add_statement(rfe);

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
                
                str cmt;
                int tag_idx = tag_idxs[idx];
                if (!custom_modifier(XSSObject(obj), modifier, html_text, tags, tag_idx, cmt))
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
                else 
                  {
                    return_function << cmt;
                    tt = tags.get(tag_idx);
                  }
              }
          }

        curr = tt.close + 1;
      }

    return_function << render_html_text(html_text.substr(curr, html_text.size() - curr));

    result->add_attribute("pre_code",        declarations.str());
    result->add_attribute("return_function", return_function.str());
    return result;
  }

str wax_utils::split_html(const str& html_text, code& cde, tag_list& tags, DynamicArray elements, int hStart, int hEnd, str& declarations)
  {
    //here comes tricky, every registered element will get its own instance
    //then the code will be analyzed to find the changes intended for the html
    XSSContext ctx(new xss_context(compiler_->current_context()));
    ctx->search_native(true); //looks important...

		std::vector<variant>::iterator it = elements->ref_begin();
		std::vector<variant>::iterator nd = elements->ref_end();

    std::vector<Object45> items;
    std::vector<int>      tag_idxs;
    std::map<int, int>    sort;
    for(; it != nd; it++)
      {
        XSSObject elem = *it;
        Object45  obj(new object_45);
        obj->copy(elem);

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

    //then, masterfully I must say, I must go looking for assigns to the elements 
    visitor_45 tr(ctx);
    cde.visit(&tr);

    //wait, not over yet... now we must split the html along the modifiers,
    //and generate the the oh sweet code.
    std::map<int, int>::iterator sit = sort.begin();
    std::map<int, int>::iterator snd = sort.end();

    size_t             curr = hStart;
    std::ostringstream result;
    std::ostringstream decl_builder;

    for(; sit != snd; sit++)
      {
        int      idx = sit->second - 1;
        tag&     tt  = tags.get(tag_idxs[idx]);
        Object45 obj = items[idx];

        if (obj->modified())
          {
            decl_builder << "var " << obj->id() << " = {};\n";

            //it has been touched by the user, otherwise just leave it as it was
            result << render_html_text(html_text.substr(curr, tt.start - curr));

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
                
                str cmt;
                int tag_idx = tag_idxs[idx];
                if (!custom_modifier(XSSObject(obj), modifier, html_text, tags, tag_idx, cmt))
                  {
                    //add the runtime value as an attribute
                    result << "response.write(\"<" << tt.tag_name << " " << modifier << " = \");\n";
                    result << "response.write('\\\"' + " << obj->id() << "." << modifier << " + '\\\"' );\n";
                    result << "response.write(\"";

                    prop_list::iterator pit = pl.begin();
                    prop_list::iterator pnd = pl.end();

                    for(; pit != pnd; pit++)
                      {
                        result << pit->first << " = \\\"" << pit->second << "\\\"";
                      }

                    result << "\");\n";
                  }
                else
                  {
                    result << cmt;
                    tt = tags.get(tag_idx);
                  }
              }
          }

        curr = tt.close + 1;
      }

    declarations = decl_builder.str();
    result << render_html_text(html_text.substr(curr, hEnd - curr));
    return result.str();
  }

void wax_utils::pre_process_args(XSSMethod methd)
  {
    IArgumentRenderer* rend = variant_cast<IArgumentRenderer*>(methd->get_parameters(), null); assert(rend);
    
    //copy arguments and nuke
    param_list_decl args = rend->get();
    rend->get().clear();

    //add standard service/page parameters
    param_decl pd;
    pd.name = "request";
    pd.type = "object";

    rend->get().push_back(pd);

    pd.name = "response";
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

bool wax_utils::custom_modifier(XSSObject obj, const str& modifier, const str& html_text, tag_list& tags, int& tag_idx, str& result)
  {
    if (obj->type_name() =="replicator")
      {
        if (modifier != "data")
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SReplicatorOnlyData);
            error.add("rep", obj->id());
            error.add("property", modifier);
            xss_throw(error);
          }

        DynamicArray code = obj->get_event_code("render");
        if (code->size() < 1)
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SMissingReplicatorEvent);
            error.add("replicator", obj->id());
            xss_throw(error);
          }
        else if (code->size() > 1)
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", STooManyReplicatorEvents);
            error.add("replicator", obj->id());
            xss_throw(error);
          }

        waxjs_code_renderer* cdr = variant_cast<waxjs_code_renderer*>(code->at(0), null); assert(cdr);
        if (cdr->forked())
          {
            param_list error;
            error.add("id", SLanguage);
            error.add("desc", SReplicatorNoFork);
            error.add("replicator", obj->id());
            xss_throw(error);
          }
        
        tag& start_tag = tags.get(tag_idx);

        tag_list rep_tags;
        tags.inner_tags(tag_idx, rep_tags);
        
        tag& end_tag = tags.get(tag_idx);
        //tag_idx++;

        str decl;
        str html_render = split_html(html_text, cdr->get_code(), rep_tags, obj->children(), rep_tags.get(0).start, rep_tags.get(rep_tags.size() - 1).close + 1, decl);
        
        std::ostringstream rep_text;
        str iterator = obj->id() + "_iterator";
        rep_text << '\n' << render_html_text(str(html_text.begin() + start_tag.start, html_text.begin() + start_tag.close + 1));
        rep_text << "\nfor(var " << iterator << " = 0; " << iterator << " < " << obj->id() << ".data.length; " << iterator << "++)";
        rep_text << "\n{";
        rep_text << "\nvar item = " << obj->id() << ".data[" << iterator << "];";
        rep_text << "\n" << decl;
        rep_text << "\n" << cdr->render();
        rep_text << "\n" << html_render;
        rep_text << "\n}";
        rep_text << '\n' << render_html_text(str(html_text.begin() + end_tag.start, html_text.begin() + end_tag.close + 1));

        result = rep_text.str();
        return true;
      }
    else if (modifier == "inner_html")
      {
        int closing = tags.find_closing(tag_idx);

        std::ostringstream ih_text;
        ih_text << "response.write( \"";

        if (closing == tag_idx)
          {
            tag& my_tag = tags.get(tag_idx);
            ih_text << render_html_text(str(html_text.begin() + my_tag.start, html_text.begin() + (my_tag.close - 1))) << ">";
            ih_text << "\" + " << obj->id() << " + \"";
            ih_text << "<\\" << my_tag.tag_name << ">";
          }
        else
          {
            tag& first = tags.get(tag_idx);
            tag& last  = tags.get(closing);
            ih_text << escape(str(html_text.begin() + first.start, html_text.begin() + first.close + 1));
            ih_text << "\" + " << obj->id() << ".inner_html + \"";
            ih_text << str(html_text.begin() + last.start, html_text.begin() + last.close + 1);
          }

        tag_idx = closing;
        ih_text << "\");";
        result = ih_text.str();
        return true;
      }

    return false;
  }

str wax_utils::escape(const str& text)
  {
    str result = text;
    boost::replace_all (result, "\n", "\\n");
    boost::replace_all (result, "\"", "\\\"");
    return result;
  }

str wax_utils::render_html_text(const str& text)
  {
    str result = text;
    boost::replace_all (result, "\n", "\\n");
    boost::replace_all (result, "\"", "\\\"");
    return "response.write(\"" + result + "\");\n";
  }

bool wax_utils::check_redirect(expression& return_expr, XSSContext ctx, param_list& args)
  {
    expression_analizer ea;
    ea.analyze(return_expr, ctx);

    if (ea.is_call() && ea.method_name() == "redirect")
      {
        args = ea.call_arguments();
        return true;
      }
    return false;
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
