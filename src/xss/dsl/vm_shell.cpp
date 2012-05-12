
#include <xss/dsl/vm_shell.h>
#include <xss/dsl/ga_parser.h>
#include <xss/xss_error.h>

#include <boost/process.hpp>

namespace bp = boost::processes;

using namespace xkp;

const str SLanguage("shell");
const str SCannotParseAssign("Cannot parse a simple assign text, rtfm");
const str SCrashedApplication("Executable application is crashed");

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
            std::vector<str> valuable_items;

            std::vector<str>::iterator iit = it->items.begin();
            std::vector<str>::iterator ind = it->items.end();

            for(; iit != ind; iit++)
              {
                valuable_items.push_back(*iit);
              }

            if (!valuable_items.size()) continue;

            //evaluate params
            std::vector<shell_param>::reverse_iterator rpit = it->params.rbegin();
            std::vector<shell_param>::reverse_iterator rpnd = it->params.rend();
            for(; rpit != rpnd; rpit++)
              {
                shell_param param = *rpit;

                if (!param.id.empty())
                  {
                    variant v = args.get(curr++);
                    str var_value = xss_utils::var_to_string(v);

                    valuable_items[param.item_idx].insert(param.item_spot, var_value);
                  }
              }

            //finally, execute the application
            try
              {
                std::string exe = bp::find_executable_in_path(valuable_items[0]);
                std::vector<std::string> args;

                std::vector<str>::iterator vit = valuable_items.begin();
                std::vector<str>::iterator vnd = valuable_items.end();
                for(; vit != vnd; vit++)
                  args.push_back(*vit);

                //td: recover the result of execution and save into variable, how to?
                //... this is temporal, only for debug purpose
                bp::context ctx;
                ctx.add(bp::close_stream(bp::stdin_fileno))
                   .add(bp::inherit_stream(bp::stdout_fileno))
                   .add(bp::inherit_stream(bp::stderr_fileno));

                bp::child c = bp::launch(exe, args, ctx);
                const bp::status s = c.wait();
              }
            catch (const boost::system::system_error&)
              {
                param_list error;
                error.add("id", SLanguage);
                error.add("desc", SCrashedApplication);
                error.add("exec", valuable_items[0]);
                xss_throw(error);
              }

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
        std::vector<shell_param>::reverse_iterator rpit = it->params.rbegin();
        std::vector<shell_param>::reverse_iterator rpnd = it->params.rend();

        for(; rpit != rpnd; rpit++)
          {
            if (rpit->id.empty())
              continue;

            expressions.push_back(rpit->id);
          }
      }

    return DSLWorker(new shellworker(result));
  }
