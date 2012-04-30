

#include <xss/dsl/html5_asynch.h>
#include <xss/xss_renderer.h>
#include <xs/compiler.h>

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

          return result.str();
        }
    private:
      XSSCompiler              compiler_;
      std::vector<synch_code>& synchs_;
  };

//dsl_native
str dsl_h5_asynch::render(dsl& info, XSSContext ctx)
  {
    XSSCompiler compiler = ctx->resolve("compiler");

    XSSContext myctx(new xss_context(ctx));
    myctx->set_this(XSSObject());
    
    std::vector<synch_code> synchsters;
    myctx->register_xss_dsl("synch", XSSDSL(new dsl_h5_synch(synchsters, compiler)));

    Language lang = ctx->get_language();

    //collect parameters
    param_list_decl    params;
    std::ostringstream params_object;
    std::ostringstream params_decode;
    param_decl         pdl; //td: !!! xss expressions already
    
    params_object << "{";
    for(size_t p = 0; p < info.params.size(); p++)
      {
        str        var_name  = info.params.get_name(p);
        expression var_value = info.params.get(p);
        str        var_str   = compiler->render_expr(var_value, ctx->get_this());  

        params_object << var_name << " : " << var_str << ",";
        params_decode << "var " << var_name << " = message.data." << var_name << ";\n";

        //keep the param
        pdl.name = var_name;
        //pdl.type; balh

        params.push_back(pdl);
      }
    params_object << "}";

    //shallow code compile
    std::vector<str> dl;
    myctx->collect_xss_dsls(dl);

    xs_compiler xc(dl);
    code asynch_code;
    xc.compile_code(info.text, asynch_code);

    //then deep compile done by the language
    CodeRenderer code_renderer    = variant_cast<CodeRenderer>(lang->compile_code(asynch_code, params, myctx), CodeRenderer()); assert(code_renderer);
    str          worker_name      = compiler->genid("worker");
    str          file_name        = compiler->genid("asynch") + ".js";
    
    //render the asynch code
    std::ostringstream code_str;
    code_str << "self.onmessage = function(message) {\n" 
             << params_decode.str() << "\n"
             << code_renderer->render() << "\n"
             << "}";
    
    //save it
    compiler->output_file(file_name, code_str.str());
    
    //back to the main code, create the worker
    std::ostringstream result;
    result << "\nvar " <<  worker_name <<  " = new Worker('" <<  file_name <<  "');";

    //render synchronizers
    if (!synchsters.empty())
      {
        result << "\n" << worker_name << ".onmessage = function(message) {";
        std::vector<synch_code>::iterator it = synchsters.begin();
        std::vector<synch_code>::iterator nd = synchsters.end();

        for(; it != nd; it++)
          {
            result << "\nif (message.data.id == '" << it->id << "') {";            
              
            //declare variables for synch params
            param_list_decl synch_params;
            for(size_t i = 0; i < it->params.size(); i++)
              {
                str var_name = it->params.get_name(i);
                result << "\nvar " << var_name << " = message.data." << var_name << ";";

                pdl.name = var_name;

                expression e;
                e.push_operand(info.params.get(i));
                pdl.default_value = e;

                synch_params.push_back(pdl);
              }

            //render code
            result << compiler->render_code(it->code, synch_params, ctx) << "\n}";
          }
        result << "\n}";
      }

    result << "\n" << worker_name << ".postMessage(" << params_object.str() << ");";
    
    return result.str();
  }
