

#include <xss/dsl/html5_asynch.h>
#include <xss/xss_renderer.h>
#include <xs/compiler.h>

using namespace xkp;

const str SLanguage("html5 asynch");

const str SAsynchObjectNotSerializable("Objects need to be serializable in order to enter threads");

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
str dsl_h5_asynch::decode_param(asynch_param_info& param, XSSCompiler compiler, XSSContext ctx)
  {
    std::ostringstream result;
    XSSType ptype    = param.type;
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

        fs::path path = compiler->type_path(ptype->id()) / serializer;
        XSSRenderer rend = compiler->compile_xss_file(path, ctx);
                    
        param_list spl;
        spl.add(ptype);
        spl.add("instance_id", param.id);
        spl.add("data", "message.data." + param.id);

        str serial_str = rend->render(XSSObject(ptype), &spl);

        result << "\nvar " << param.id << " = " << serial_str;
      }
    else
      {
        result << "\nvar " << param.id << " = message.data." << param.id << ";";
      }

    return result.str();
  }

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
    std::ostringstream dependencies;
    param_decl         pdl; //td: !!! xss expressions already
    dependency_list    deps;
    
    asynch_params code_params;
    process_asynch_params(compiler, info.params, code_params);

    params_object << "{";
   
    asynch_params::iterator cpit = code_params.begin();
    asynch_params::iterator cpnd = code_params.end();
    for(; cpit != cpnd; cpit++)
      {
        params_object << cpit->id << " : " << cpit->value << ",";
        params_decode << decode_param(*cpit, compiler, ctx);

        //keep the param
        pdl.name = cpit->id;

        if (cpit->type)
          {
            compiler->type_dependencies(cpit->type, deps);
            pdl.type = cpit->type->id();
          }
        

        params.push_back(pdl);
      }
    params_object << "}";

    //importScript
    XSSObjectList::iterator dit = deps.items.begin();
    XSSObjectList::iterator dnd = deps.items.end();
    for(; dit != dnd; dit++)
      {
        //make a copy, avoid future problems
        XSSObject dep(new xss_object);
        dep->copy(*dit);
        XSSObject idiom = dep->idiom();
        if (idiom)
          {
            //td: !!! there is no need for this
            size_t ev_id = idiom->event_id("compile_dependency");
            if (ev_id > 0)
              {
                param_list pl;
                pl.add(dep);
                idiom->dispatch_event(ev_id, pl);
              }
          }

        str href = dep->get<str>("href", str());
        dependencies << "\nimportScripts(\"" << href << "\");";
      }

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
    code_str << dependencies.str() << "\n"
             << "self.onmessage = function(message) {\n" 
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

            asynch_params::iterator pit = it->params.begin();
            asynch_params::iterator pnd = it->params.end();

            for(; pit != pnd; pit++)
              {
                str     var_name = pit->id;
                result << decode_param(*pit, compiler, ctx);

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
