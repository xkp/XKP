
#include <xss/dsl/vm_shell.h>
#include <xss/dsl/ga_parser.h>
#include <xss/xss_error.h>

using namespace xkp;

const str SLanguage("shell");
const str SCannotParseAssign("Cannot parse a simple assign text, rtfm");

struct shellworker : IWorker
  {
    shellworker(std::vector<ga_item>& result):
      result_(result)
      {
      }

    virtual void work(const param_list& args)
      {
        int curr = 0;

        std::vector<ga_item>::iterator it = result_.begin();
        std::vector<ga_item>::iterator nd = result_.end();
        for(; it != nd; it++)
          {
            std::ostringstream callable;

            for(size_t i = 0; i < it->params.size(); i++)
              {
                str text  = it->text[i];
                str param = it->params[i];
                
                if (!param.empty())
                  callable << xss_utils::var_to_string(args.get(curr++));
                
                callable << text;
              }

            //EXECUTE callable.str(), do variables
          }
      }
    private:
      std::vector<ga_item> result_;
  };

//vm_dsl
DSLWorker vm_shell::create_worker(dsl& info, code_linker& owner, std::vector<str>& expressions)
  {
    ga_parser parser;
    std::vector<ga_item> result;
    if (!parser.parse(info.text, result))
      {
        param_list error;
        error.add("id", SLanguage);
        error.add("desc", SCannotParseAssign);
        error.add("text", info.text);
        xss_throw(error);
      }

    std::vector<ga_item>::iterator it = result.begin();
    std::vector<ga_item>::iterator nd = result.end();
    for(; it != nd; it++)
      {
        //fun stuff, gather the params in the query (in the form @value)
        std::vector<str>::iterator pit = it->params.begin();
        std::vector<str>::iterator pnd = it->params.end();

        for(; pit != pnd; pit++)
          {
            if (pit->empty())
              continue;

            expressions.push_back(*pit);
          }
      }

    return DSLWorker(new shellworker(result));
  }
