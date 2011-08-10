
#include <xss/dsl_out.h>
#include <xss/utils.h>
#include <xss/utils.h>
#include <xss/xss_error.h>
#include <xss/xss_renderer.h>
#include <xs/ast.h>

#include <boost/algorithm/string.hpp>
#include "boost/filesystem.hpp"

namespace fs = boost::filesystem;
using namespace xkp;

//strings
const str SInvalidTag("xss-tags");
const str STypeMismatch("type-mismatch");
const str SOut("out");

const str SUnknownXSSTag("Using an unknown tag");
const str SExpectingExpression("Expecting expression");
const str SFileParameterMustHaveName("File parameters must have names");
const str SUnexpectedTag("Unexpected tag");
const str SExpectingOutputAttribute("You must specify an output attribue on an xss:file tag");
const str SExpectingSourceAttribute("You must specify an source attribue on an xss:file tag");
const str SCannotResolveContex("There is no xss context registered");

struct file_parameter
	{
		file_parameter(const str& _name, const str& _value):
			name(_name),
			value(_value)
			{
			}

		str name;
		str value;
    int expr_idx;
	};

struct file_parser : xss_visitor
	{
    virtual void visit(const str& tag, const str& text, param_list* args)
      {
				if (tag == "text")
					{
						//do nothing, only looking for parameters
					}
				else if (tag == "parameter")
					{
						if (args->has("name"))
							{
								str name  = variant_cast<str>(args->get("name"), str()); assert(!name.empty());
								str value = variant_cast<str>(args->get("value"), name);

								parameters.push_back(file_parameter(name, value));
							}
						else
							{
								param_list error;
								error.add("id", SInvalidTag);
								error.add("desc", SFileParameterMustHaveName);
								error.add("tag", tag);
								xss_throw(error);
							}
					}
				else
					{
						param_list error;
						error.add("id", SInvalidTag);
						error.add("desc", SUnexpectedTag);
						error.add("tag", tag);
						xss_throw(error);
					}
			}

		str result;
		std::vector<file_parameter> parameters;
	};

struct out_expr_renderer : item_renderer
  {
    out_expr_renderer(int idx):
      idx_(idx)
      {
      }

    virtual str render(XSSObject this_, param_list* args)
      {
        return xss_utils::var_to_string(args->get(idx_));
      }
    private:
      int idx_;
  };

struct out_file_renderer : item_renderer
  {
    out_file_renderer(XSSCompiler compiler, std::vector<file_parameter>& parameters):
      compiler_(compiler)
      {
        std::vector<file_parameter>::iterator it = parameters.begin();
        std::vector<file_parameter>::iterator nd = parameters.end();
        for(; it != nd; it++)
          { 
            params_.insert(std::pair<str, int>(it->name, it->expr_idx));
          }
      }
    
    void compile(XSSContext ctx, param_list* args)
      {
		    output_ = variant_cast<str>(args->get("output"), str());
		    str src	= variant_cast<str>(args->get("src"), str());

		    if (output_.empty())
			    {
				    param_list error;
				    error.add("id", SOut);
				    error.add("desc", SExpectingOutputAttribute);
				    xss_throw(error);
			    }
        
		    if (src.empty())
			    {
				    param_list error;
				    error.add("id", SOut);
				    error.add("desc", SExpectingSourceAttribute);
				    xss_throw(error);
			    }

        renderer_ = compiler_->compile_xss_file(src, ctx);
      }

    //item_renderer
    virtual str render(XSSObject this_, param_list* args)
      {
        renderer_parameter_list& params = renderer_->params(); //td: check types
        param_list file_params;
        
        //translate outside parameters into file parameters
        renderer_parameter_list::iterator it = params.begin();
        renderer_parameter_list::iterator nd = params.end();
        for(; it != nd; it++)
          {
            std::map<str, int>::iterator pp = params_.find(it->id);
            if (pp != params_.end())
              file_params.add(it->id, args->get(pp->second));
            else
              file_params.add(it->id, it->default_value);
          }

        str result = renderer_->render(XSSObject(), &file_params);
        if (output_ != "inline")
          {
            compiler_->output_file(output_, result);
            return str();
          }

        return result;
      }
    
    private:
      std::map<str, int> params_;
      XSSRenderer        renderer_;
      XSSCompiler        compiler_;
      str                output_; 
  };

struct out_renderer : base_xss_renderer<out_renderer>
  {
    out_renderer(std::vector<str>& expressions, XSSCompiler compiler, XSSContext ctx):
      expressions_(expressions),
      compiler_(compiler),
      ctx_(ctx)
      {
        handlers_.insert(handler_pair("text",			        &out_renderer::handle_text));
        handlers_.insert(handler_pair("xss:e",		        &out_renderer::handle_expression));
        handlers_.insert(handler_pair("xss:file",         &out_renderer::handle_file));
        handlers_.insert(handler_pair("xss:open_brace",   &out_renderer::handle_open_brace));
        handlers_.insert(handler_pair("xss:close_brace",  &out_renderer::handle_close_brace));
      }

    item_list items()
      {
        return items_;
      }

    void handle_text(const str& text, param_list* args)
      {
        items_.push_back(ItemRenderer(new text_renderer(text)));
      }

    void handle_expression(const str& text, param_list* args)
      {
        str expr_ = text;
        if (text.empty() && args)
          {
            variant vv = args->get("value");
            if (vv.empty())
					    vv = args->get("v");

            if (!vv.empty())
              expr_ = variant_cast<str>(vv, "");
          }

        expressions_.push_back(expr_);
        items_.push_back(ItemRenderer(new out_expr_renderer(expressions_.size() - 1)));
      }

    void handle_file(const str& text, param_list* args)
      {
				file_parser fparser;
				xss_parser	xparser;

				xparser.register_tag("parameter");
				xparser.parse(text, &fparser);

        std::vector<file_parameter>::iterator it = fparser.parameters.begin();
        std::vector<file_parameter>::iterator nd = fparser.parameters.end();
        for(; it != nd; it++)
          {
            expressions_.push_back(it->value);
            it->expr_idx = expressions_.size() - 1;
          }

        out_file_renderer* result = new out_file_renderer(compiler_, fparser.parameters);
        result->compile(ctx_, args);

        items_.push_back(ItemRenderer(result));
      }

    void handle_open_brace(const str& text, param_list* args)
      {
        items_.push_back(ItemRenderer(new text_renderer("{")));
      }

    void handle_close_brace(const str& text, param_list* args)
      {
        items_.push_back(ItemRenderer(new text_renderer("}")));
      }
    private:
      std::vector<str>& expressions_;
      XSSCompiler       compiler_;
      XSSContext        ctx_;
  };

//runtime object
struct worker
  {
    public:
      worker() {}
      worker(XSSCompiler compiler, std::vector<ItemRenderer>& renderer, str marker):
        compiler_(compiler),
				marker_(marker),
        renderer_(renderer)
        {
        }
    public:
      void generate(const param_list params)
        {
					str result;
          param_list* args = const_cast<param_list*>(&params);

          std::vector<ItemRenderer>::iterator it = renderer_.begin();
          std::vector<ItemRenderer>::iterator nd = renderer_.end();
          for(; it != nd; it++)
            {
              ItemRenderer ir = *it;
              result += ir->render(XSSObject(), args);
            }

          XSSRenderer rr = compiler_->current_renderer();
          if (marker_.empty())
						rr->append(result);
					else
						rr->append_at(result, marker_);
        }

    private:
      XSSCompiler								compiler_;
			std::vector<ItemRenderer> renderer_;
      str												marker_;

      std::vector<str> load_lines(const str& s)
        {
          std::vector<str> result;
          size_t curr = 0;
          for(size_t i = 0; i < s.length(); i++)
            {
              if (s[i] == '\n')
                {
                  result.push_back( s.substr(curr, i) );
                  curr = i;
                }
            }

          return result;
        }
  };

typedef reference<worker> Worker;

struct worker_schema : object_schema<worker>
  {
    virtual void declare()
      {
        dynamic_method_( "generate", &worker::generate );
      }
  };

namespace xkp
{
    register_complete_type(worker, worker_schema);
}

//out_linker
void out_linker::link(dsl& info, code_linker& owner)
  {
    std::vector<str> expressions;
		variant marker_v = info.params.get("marker");
		str			marker;
		if (!marker_v.empty())
			{
        expression expr = marker_v;
        marker = variant_cast<str>(owner.evaluate_expression(expr), str(""));
			}

		//process xss
    variant ctx_var;
    if (!owner.context().scope_->resolve("#context", ctx_var))
      {
				param_list error;
				error.add("id", SOut);
				error.add("desc", SCannotResolveContex);
				xss_throw(error);
      }
    
    XSSContext ctx = ctx_var;

    out_renderer renderer(expressions, compiler_, ctx);

    xss_parser parser;
    parser.register_tag("xss:e");
    parser.register_tag("xss:open_brace");
    parser.register_tag("xss:close_brace");
		parser.register_tag("xss:file");

		str to_parse = info.text;
		parser.parse(to_parse, &renderer);

    //now xs
    xs_utils xs;

    //create a safe reference to be inserted in the execution context later on
		Worker wrk(new worker(compiler_, renderer.items(), marker));
    owner.add_instruction(i_load_constant, owner.add_constant(wrk));

    //push the expression to be used as parameters, push 'em in reverse order
    std::vector<str>::reverse_iterator it = expressions.rbegin();
    std::vector<str>::reverse_iterator nd = expressions.rend();
    for(; it != nd; it++)
      {
        expression expr;
        if (!xs.compile_expression(*it, expr))
          assert(false);

        owner.link_expression(expr);
      }

    //for this call, remember the indent is always the forat parameter
    owner.add_call(wrk, "generate", expressions.size(), false);
  }

