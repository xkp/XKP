

#include <xss/dsl/html5_asynch.h>

using namespace xkp;

struct synch_code
  {
    str        id;
    str        code;  
    param_list params;
  };

class dsl_h5_synch : public xss_dsl
  {
    public:
      dsl_h5_synch(std::vector<synch_code>& synchs, XSSCompiler compiler):  
        compiler_(compiler),
        synchs_(synchs)
        {
        }
    public:
      virtual str render(dsl& info, XSSContext ctx)
        { 
          std::ostringstream result;
          str id = compiler_->genid("wwmsg");
          
          result << "\nvar " << id << " = {";
          for(size_t i = 0; i < info.params.size(); i++)
            {
              str pname = info.params.get_name(i);
              result << pname << " : " << pname << ",";
            }
          
          result << "};";
          result << "\npostMessage(" << id << ");";

          synch_code sc;
          sc.id = id;
          sc.code = info.text;
          sc.params = info.params;

          synchs_.push_back(sc);

          return str(); //td: wassup with this?
        }
    private:
      XSSCompiler              compiler_;
      std::vector<synch_code>& synchs_;
  };

//dsl_native
str dsl_h5_asynch::render(dsl& info, XSSContext ctx)
  {
    XSSCompiler compiler = ctx->resolve("compiler");
    XSSRenderer rend     = compiler->current_renderer();

    XSSContext myctx(new xss_context(ctx));
    
    std::vector<synch_code> synchsters;
    XSSDSL dsl(new dsl_h5_synch(synchsters, compiler));
    myctx->register_xss_dsl("synch", dsl);

    Language lang = ctx->get_language();

    xs_utils     xs;
    code_context code_ctx;
    code_ctx = ctx->get_compile_context();

    code asynch_code;
    xs.compile_code(info.text, code_ctx, ctx->path(), asynch_code);

    param_list_decl params;
    param_decl pdl; //td: !!! xss expressions already
    
    for(size_t p = 0; p < info.params.size(); p++)
      {
        str var_name = info.params.get_name(p);
        pdl.name = var_name;
        //pdl.type; balh

        expression e;
        e.push_operand(info.params.get(p));
        pdl.default_value = e;

        params.push_back(pdl);
      }

    variant compiled    = lang->compile_code(asynch_code, params, myctx);
    str     worker_name = compiler->genid("worker");
    str     file_name   = compiler->genid("asynch") + ".js";
    
    rend->append("\nvar " + worker_name + " = new Worker('" + file_name + "');");

    if (!synchsters.empty())
      {
        rend->append("\n" + worker_name + ".onmessage = function(message) {");
        std::vector<synch_code>::iterator it = synchsters.begin();
        std::vector<synch_code>::iterator nd = synchsters.end();

        for(; it != nd; it++)
          {
            rend->append("\nif (message.id == '" + it->id + "') {");            
              
              //declare variables for synch params
              param_list_decl synch_params;
              for(size_t i = 0; i < it->params.size(); i++)
                {
                  str var_name = it->params.get_name(i);
                  rend->append("\nvar " + var_name + " = message." + var_name + ";");

                  pdl.name = var_name;

                  expression e;
                  e.push_operand(info.params.get(i));
                  pdl.default_value = e;

                  synch_params.push_back(pdl);
                }

              //render code
              rend->append(compiler->render_code(it->code, synch_params, myctx));
            rend->append("\n}");            
          }
        rend->append("\n}");
      }

    return info.text;
  }
