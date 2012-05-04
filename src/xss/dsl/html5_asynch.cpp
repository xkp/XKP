

#include <xss/dsl/html5_asynch.h>
#include <xss/xss_renderer.h>
#include <xs/compiler.h>

using namespace xkp;

const str SLanguage("html5 asynch");

const str SAsynchObjectNotSerializable("Objects need to be serializable in order to enter threads");

struct asynch_param_info
  {
    str     id;
    str     value;
    XSSType type;
  };

typedef std::vector<asynch_param_info> asynch_params;

struct synch_code
  {
    str           id;
    str           code;  
    asynch_params params;
  };

void process_asynch_params(XSSCompiler compiler, const param_list params, asynch_params& result)
  {
    for(size_t i = 0; i < params.size(); i++)
      {
        str pname  = params.get_name(i);
        expression pvalue = params.get(i); 
        
        asynch_param_info res;
        res.id = pname;
        res.value = compiler->render_expr(pvalue, XSSObject());
        res.type = compiler->type_of(pvalue);

        result.push_back(res);
      }
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
          
          asynch_params params;
          process_asynch_params(compiler_, info.params, params);
          
          result << "\nvar " << id << " = {";
          
          asynch_params::iterator it = params.begin();
          asynch_params::iterator nd = params.end();

          for(; it != nd; it++)
            {
              result << it->id << " : " << it->value << ",";
            }
          
          result << "};";
          result << "\npostMessage(" << id << ");";

          synch_code sc;
          sc.id = id;
          sc.code = info.text;
          sc.params = params;

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

            asynch_params params;
            process_asynch_params(compiler, info.params, params);

            asynch_params::iterator pit = params.begin();
            asynch_params::iterator pnd = params.end();

            for(; pit != pnd; pit++)
              {
                str     var_name = pit->id;
                XSSType ptype    = pit->type;
                if (ptype && !ptype->is_variant())
                  {
                    str serializer = ptype->get<str>("serializer", str());
                    if (serializer.empty())
                      {
                        param_list error;
                        error.add("id", SLanguage);
                        error.add("desc", SAsynchObjectNotSerializable);
                        error.add("type", ptype->id());
                        xss_throw(error);
                      }

                    str path = compiler->idiom_path(XSSObject(ptype), serializer);
                    XSSRenderer rend = compiler->compile_xss_file(path, ctx);
                    
                    param_list spl;
                    spl.add(ptype);
                    spl.add("data", "message.data." + var_name);

                    str serial_str = rend->render(XSSObject(ptype), &spl);

                    result << "\nvar " << var_name << " = " << serial_str;

                    
                  }
                else
                  {
                    result << "\nvar " << var_name << " = message.data." << var_name << ";";
                  }

                pdl.name = var_name;
                pdl.type = pit->type? pit->type->id() : "var";

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
