
#include <xss/dsl/vm_shell.h>
#include <xss/dsl/ga_parser.h>
#include <xss/xss_error.h>
#include <xss/utils.h>

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <boost/process.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#include <boost/filesystem.hpp>

using namespace boost::process;
using namespace boost::process::initializers;
using namespace boost::iostreams;

namespace bp  = boost::process;
namespace bio = boost::iostreams;
namespace fs  = boost::filesystem;

using namespace xkp;

const str SLanguage("shell");
const str SCannotParseAssign("Cannot parse a simple assign text, rtfm");
const str SShellVariableNotFound("Cannot find shell variable");
const str SCrashedApplication("Executable application is crashed");

//utils
struct shellworker : IWorker
  {
    shellworker(std::vector<ga_item>& result):
      result_(result)
      {
      }

    virtual DynamicObject work(const param_list& args)
      {
        DynamicObject result(new sponge_object);

        int curr = 0;

        variant v = args.get(curr++);
        int counter = variant_cast<int>(v, 0);

        fs::path working_path = fs::current_path();
        str environment_vars; //td: correctly
        bool break_errors = false;
        bool shell_cmd = false;

        if (counter & 1)
          {
            variant v = args.get(curr++);
            working_path = xss_utils::var_to_string(v);
            //working_path = fs::path(working_path).generic_string();
            working_path = fs::path(working_path).make_preferred();
          }

        if (counter & 2)
          {
            variant v = args.get(curr++);
            environment_vars = xss_utils::var_to_string(v);
          }

        if (counter & 4)
          {
            variant v = args.get(curr++);
            break_errors = variant_cast<bool>(v, false);
          }

        if (counter & 8)
          {
            variant v = args.get(curr++);
            shell_cmd = variant_cast<bool>(v, false);
          }

        std::vector<ga_item>::iterator it = result_.begin();
        std::vector<ga_item>::iterator nd = result_.end();
        for(; it != nd; it++)
          {
            std::vector<str> valuable_items;

            std::vector<str>::iterator iit = it->items.begin();
            std::vector<str>::iterator ind = it->items.end();
            for(; iit != ind; iit++)
              {
                str str_value = *iit;
                valuable_items.push_back(str_value);
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
                    if(v.empty())
                      {
                        param_list error;
                        error.add("id", SLanguage);
                        error.add("desc", SShellVariableNotFound);
                        error.add("variable", param.id);
                        xss_throw(error);
                      }

                    str var_value = xss_utils::var_to_string(v);
                    valuable_items[param.item_idx].insert(param.item_spot, var_value);
                  }
              }

            //finally, execute the application
            try
              {
                std::wstring exe;

                str &first_param = valuable_items[0];

                int pos = -1;
                for (int i = first_param.size() - 1; i >= 0; --i)
                  {
                    char c = first_param[i];
                    if (c == '/' || c == '\\')
                      {
                        pos = i;
                        break;
                      }
                  }

                if (pos != -1)
                  {
                    str cmd_path      = first_param.substr(0, pos + 1);
                    str cmd_filename  = first_param.substr(pos + 1);
                    exe = search_path(str2wide(cmd_filename), str2wide(cmd_path));
                  }
                else
                  {
                    exe = search_path(str2wide(first_param));
                  }

                std::vector<std::wstring> args;

                std::vector<str>::iterator vit = valuable_items.begin();
                std::vector<str>::iterator vnd = valuable_items.end();

                std::wstring command;
                for(; vit != vnd; vit++)
                  {
                    str arg = *vit;

                    //td: tests more use cases
                    //if (arg.find(' ') != str::npos)
                    //  arg = '"' + arg + '"';

                    command += str2wide(arg) + L" ";

                    //don't add empty params
                    if (!arg.empty())
                      args.push_back(str2wide(arg));
                  }

                boost::system::error_code ec;
                bp::pipe p = create_pipe();
                  {
                    bio::file_descriptor_sink sink(p.sink, bio::close_handle);
                    bp::child c = bp::execute(
                                    run_exe(exe),
                                    set_on_error(ec),
                                    set_args(args),
                                    bind_stdout(sink),
                                    bind_stderr(sink),
                                    close_stdin(),
                                    start_in_dir(working_path),
                                    inherit_env() //td:
                                  );

                    //td: return this value like exit_status
                    int s = bp::wait_for_exit(c);
                  }

                if (!it->variable.empty())
                  {
                    DynamicArray shell_data(new dynamic_array);

                    bio::file_descriptor_source source(p.source, bio::close_handle);
                    bio::stream<bio::file_descriptor_source> is(source);

                    str line;
                    while (std::getline(is, line))
                      shell_data->push_back(line);

                    dynamic_set(result, it->variable, shell_data);
                  }

//td: personalize class with static function or only functions
//#if defined(__unix__) || defined(__unix) || defined(unix) || defined(__linux__)
//                if (s.exited()) {
//                    //std::cout << "Program returned exit code " << s.exit_status() << std::endl;
//                } else if (s.signaled()) {
//                    //std::cout << "Program received signal " << s.term_signal() << std::endl;
//                    if (s.dumped_core())
//                      {
//                        //std::cout << "Program also dumped core" << std::endl;
//                      }
//                } else if (s.stopped()) {
//                    //std::cout << "Program stopped by signal" << s.stop_signal() << std::endl;
//                } else {
//                    //std::cout << "Unknown termination reason" << std::endl;
//                }
//#elif defined(_WIN32) || defined(_WIN64) || defined(__WIN32__)
//#endif
//
//                if (s.exited() && s.exit_status() == EXIT_SUCCESS)
//                  {
//                    //std::cout << "[" << exe << "]" << " Shell application is executed successfully." << std::endl;
//                  }
//                else
//                  {
//                    //std::cout << "[" << exe << "]" << " Shell application failed." << std::endl;
//                    if (break_errors)
//                      {
//                        param_list error;
//                        error.add("id", SLanguage);
//                        error.add("desc", SCrashedApplication);
//                        error.add("exec", valuable_items[0]);
//                        xss_throw(error);
//                      }
//                  }
              }
            catch (const boost::system::system_error &error)
              {
                //td: custom error messages
                boost::system::error_code err_code = error.code();
                str what_err = error.what();

                str msg_err = err_code.message();
                int val_err = err_code.value();

                if (break_errors)
                  {
                    param_list error;
                    error.add("id", SLanguage);
                    error.add("desc", SCrashedApplication);
                    error.add("exec", valuable_items[0]);
                    xss_throw(error);
                  }
              }
          }

        return result;
      }
    private:
      std::vector<ga_item> result_;
  };

//vm_dsl
DSLWorker vm_shell::create_worker(dsl& info, code_linker& owner, std::vector<str>& in, std::vector<str>& out)
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

    //push values of dsl params into expression
    size_t count = 0;
    std::vector<str> aux_exprs;
    if (info.param_count)
      {
        str text = "working_path environment_vars break_errors shell_cmd";
        std::vector<str> params;
        boost::split(params, text, boost::is_space());

        for(size_t idx = 0, pos = 1; idx < params.size(); idx++, pos <<= 1)
          {
            variant param = info.params.get(params[idx]);
            if (!param.empty())
              {
                expression expr = param;
                assert(expr.size() == 1); //td:

                variant first = expr.first();
                if (first.is<expression_identifier>())
                  {
                    expression_identifier ei = first;
                    str value = ei.value;
                    aux_exprs.push_back(value);
                    count |= pos;
                  }
                else
                if (first.is<str>())
                  {
                    str s = "\"" + variant_cast<str>(first, str("")) + "\"";
                    aux_exprs.push_back(s);
                    count |= pos;
                  }
                else
                if (first.is<bool>())
                  {
                    aux_exprs.push_back(xss_utils::var_to_string(first));
                    count |= pos;
                  }
                else
                  assert(false); //catch another case use
              }
          }
      }

    in.push_back(boost::lexical_cast<str>(count));
    in.insert(in.end(), aux_exprs.begin(), aux_exprs.end());

    std::vector<ga_item>::iterator it = result.begin();
    std::vector<ga_item>::iterator nd = result.end();
    for(; it != nd; it++)
      {
        if (!it->variable.empty())
          out.push_back(it->variable);

        //fun stuff, gather the params in the query (in the form @value)
        std::vector<shell_param>::reverse_iterator rpit = it->params.rbegin();
        std::vector<shell_param>::reverse_iterator rpnd = it->params.rend();

        for(; rpit != rpnd; rpit++)
          {
            if (rpit->id.empty())
              continue;

            in.push_back(rpit->id);
          }
      }

    return DSLWorker(new shellworker(result));
  }
