//
//#include <xss/dsl_out.h>
//#include <xss/utils.h>
//#include <xss/utils.h>
//#include <xss/xss_error.h>
//#include <xs/ast.h>
//
//#include <boost/algorithm/string.hpp>
//#include "boost/filesystem.hpp"
//
//namespace fs = boost::filesystem;
//using namespace xkp;
//
////strings
//const str SInvalidTag("xss-tags");
//const str STypeMismatch("type-mismatch");
//
//const str SUnknownXSSTag("Using an unknown tag");
//const str SExpectingExpression("Expecting expression");
//const str SFileParameterMustHaveName("File parameters must have names");
//const str SUnexpectedTag("Unexpected tag");
//const str SFileMustHaveOutput("You must specify an output attribue on this tag");
//const str SFileMustHaveSource("You must specify an source attribue on this tag");
//
//enum part_type
//	{
//		PART_TEXT,
//		PART_EXPRESSION,
//		PART_FILE,
//	};
//
//struct part
//  {
//		part_type type;
//    int				dyn_idx;
//    str				text;
//
//    part(): dyn_idx(-1), type(PART_TEXT)																									{}
//    part(const str& txt): dyn_idx(-1), type(PART_TEXT), text(txt)													{}
//		part(int didx): dyn_idx(didx), type(PART_EXPRESSION)																	{}
//		part(part_type _type, const str& txt, int idx): dyn_idx(idx), type(_type), text(txt)	{}
//  };
//
//typedef std::vector<part> part_list;
//
//struct file_parameter
//	{
//		file_parameter(const str& _name, const str& _value):
//			name(_name),
//			value(_value)
//			{
//			}
//
//		str name;
//		str value;
//	};
//
//struct file_parser : xss_visitor
//	{
//    virtual void visit(const str& tag, const str& text, param_list* args)
//      {
//				if (tag == "text")
//					{
//						result += text;
//					}
//				else if (tag == "parameter")
//					{
//						if (args->has("name"))
//							{
//								str name  = variant_cast<str>(args->get("name"), str()); assert(!name.empty());
//								str value = variant_cast<str>(args->get("value"), name);
//
//								parameters.push_back(file_parameter(name, value));
//							}
//						else
//							{
//								param_list error;
//								error.add("id", SInvalidTag);
//								error.add("desc", SFileParameterMustHaveName);
//								error.add("tag", tag);
//								xss_throw(error);
//							}
//					}
//				else
//					{
//						param_list error;
//						error.add("id", SInvalidTag);
//						error.add("desc", SUnexpectedTag);
//						error.add("tag", tag);
//						xss_throw(error);
//					}
//			}
//
//		str result;
//		std::vector<file_parameter> parameters;
//	};
//
//struct outfile_info
//	{
//		str source;
//		str output;
//		std::vector<file_parameter> parameters;
//		int param_offset;
//
//		outfile_info(str _source, str _output, std::vector<file_parameter>& _parameters, int _param_offset):
//			source(_source),
//			output(_output),
//			parameters(_parameters),
//			param_offset(_param_offset)
//			{
//			}
//	};
//
//struct xss_gather : xss_visitor
//  {
//    public:
//      xss_gather(part_list& result, std::vector<outfile_info>& files, std::vector<str>& expressions) : result_(result), expressions_(expressions), files_(files) {}
//    public:
//      virtual void visit(const str& tag, const str& text, param_list* args)
//        {
//          if (tag == "text")
//            result_.push_back(part(text));
//          else  if (tag == "xss:quote")
//            {
//              result_.push_back(part('"' + text + '"'));
//            }
//          else if (tag == "xss:e")
//            {
//              str expr_text = text;
//              if (expr_text.empty() && args)
//                expr_text = variant_cast<str>(args->get("value"), "");
//
//							if (expr_text.empty() && args)
//                expr_text = variant_cast<str>(args->get("v"), "");
//
//              if (!expr_text.empty())
//                {
//                  result_.push_back(part(result_.size()));
//                  expressions_.push_back(expr_text);
//                }
//            }
//          else if (tag == "xss:open_brace")
//            {
//              result_.push_back(part("{"));
//            }
//          else if (tag == "xss:close_brace")
//            {
//              result_.push_back(part("}"));
//            }
//					else if (tag == "xss:return")
//						{
//							result_.push_back(part("\n"));
//						}
//          else if (tag == "xss:code")
//            {
//              //td: !!!
//              result_.push_back(part("code can not be embbeded in out"));
//            }
//					else if (tag == "xss:file")
//						{
//							//grab info
//							str output = variant_cast<str>(args->get("output"), str(""));
//							if (output.empty())
//								{
//									param_list error;
//									error.add("id", SInvalidTag);
//									error.add("desc", SFileMustHaveOutput);
//									error.add("tag", tag);
//									xss_throw(error);
//								}
//
//							str source = variant_cast<str>(args->get("src"), str(""));
//
//							//grab the parameters
//							file_parser fparser;
//							xss_parser	xparser;
//
//							xparser.register_tag("parameter");
//							xparser.parse(text, &fparser);
//
//							std::vector<file_parameter>::iterator it = fparser.parameters.begin();
//							std::vector<file_parameter>::iterator nd = fparser.parameters.end();
//
//							//register the parameters as expressions
//							int param_offset = expressions_.size() + 1; //account for indent
//							for(; it != nd; it++)
//								{
//									str value = it->value;
//									if (value.empty())
//										value = it->name;
//									expressions_.push_back(value);
//								}
//
//							//keep track of the parts
//							files_.push_back(outfile_info(source, output, fparser.parameters, param_offset));
//							result_.push_back(part(PART_FILE, fparser.result, files_.size() - 1));
//						}
//          else
//            {
//              param_list error;
//              error.add("id", SInvalidTag);
//              error.add("desc", SUnknownXSSTag);
//              error.add("tag", tag);
//              xss_throw(error);
//            }
//        }
//    private:
//      part_list&									result_;
//      std::vector<str>&						expressions_;
//      std::vector<outfile_info>&	files_;
//  };
//
//struct worker
//  {
//    public:
//      worker() : tab_(4) {}
//      worker(XSSProject project, part_list parts, std::vector<outfile_info> files, int tab, bool dont_break, str marker):
//        project_(project),
//        parts_(parts),
//				files_(files),
//        indent_(-1),
//        tab_(tab),
//				dont_break_(dont_break),
//				marker_(marker)
//        {
//        }
//    public:
//      void generate(const param_list params)
//        {
//					XSSGenerator gen = project_->generator();
//
//          part_list::iterator it = parts_.begin();
//          part_list::iterator nd = parts_.end();
//
//          indent_ = params.get(params.size() - 1);
//
//          size_t param = params.size() - 2;
//          str    result;
//          for(; it != nd; it++)
//            {
//							switch(it->type)
//								{
//									case PART_TEXT:
//										{
//											result += it->text;
//											break;
//										}
//									case PART_EXPRESSION:
//										{
//											variant vv = params.get(param--);
//											if (vv.empty())
//												vv = str("null"); //td: this should be an error?
//
//											if (!vv.is<str>())
//												{
//													param_list pl;
//													variant    v;
//													if (dynamic_try_exec(vv, "generate", pl, v))
//														{
//															str ss = variant_cast<str>(v, str()); //td: error, maybe?
//															result += ss;
//															break;
//														}
//													else if (vv.is<bool>())
//														{
//															bool bb = vv;
//															if (bb)
//																result += "true";
//															else
//																result += "false";
//															break;
//														}
//												}
//
//											str expr_value = vv;
//											if (is_multi_line(expr_value))
//												{
//													//we'll try to keep the original indentation
//													str padding = last_padding(result);
//
//													//so we'll add the original padding to every line
//													std::vector<str> lines;
//													split_lines(expr_value, lines);
//
//													std::vector<str>::iterator lit = lines.begin();
//													std::vector<str>::iterator lnd = lines.end();
//													bool first = true;
//													for(; lit != lnd; lit++)
//														{
//															if (first)
//																first = false;
//															else
//																result += padding;
//
//															result += *lit;
//
//															if (!dont_break_)
//																result += '\n';
//														}
//												}
//											else
//												result += expr_value;
//
//											break;
//										}
//									case PART_FILE:
//										{
//											outfile_info& file_info = files_[it->dyn_idx];
//
//											str source = it->text;
//											bool got_src = false;
//											fs::path path = project_->current_context()->path_;
//											if (!file_info.source.empty())
//												{
//													got_src = true;
//													project_->push_file(file_info.source);
//
//													fs::path src_file = path / file_info.source;
//													source	 = project_->load_file(src_file.string());
//
//													path = src_file.parent_path();
//												}
//
//											//td: utilify this crap already, or something
//											XSSContext curr_ctx = project_->current_context();
//
//											XSSContext context(new xss_code_context(project_, project_->idiom, path));
//											xss_code_context& ctx = *context.get();
//
//											XSSGenerator gen(new xss_generator(context));
//											project_->push_generator(gen);
//
//											project_->prepare_context(ctx, gen);
//
//											std::vector<file_parameter>::iterator it = file_info.parameters.begin();
//											std::vector<file_parameter>::iterator nd = file_info.parameters.end();
//											int last = params.size() - 1;
//											int curr = 0;
//											for(; it != nd; it++, curr++)
//												{
//													param--;
//													variant vv = params.get(last - file_info.param_offset - curr);
//													ctx.scope_->register_symbol(it->name, vv);
//												}
//
//											str contents = project_->generate_xss(source, gen);
//
//											project_->pop_generator();
//											if (got_src)
//												project_->pop_file();
//
//											if (file_info.output == "inline")
//											{
//												result += contents;
//												//project_->generator()->append(result);
//											}
//											else
//												project_->output_file(file_info.output, contents);
//
//											break;
//										}
//								}
//            }
//
//          if (indent_ >= 0)
//            {
//              result = apply_indent(result, indent_);
//            }
//
//					if (marker_.empty())
//						gen->append(result);
//					else
//						gen->append_marker(marker_, result);
//        }
//
//      str apply_indent(const str& s, int indent)
//        {
//          str result;
//
//          std::vector<str> lines;
//          split_lines(s, lines);
//
//          std::vector<str>::iterator it = lines.begin();
//          std::vector<str>::iterator nd = lines.end();
//
//          //find out the minimum start position per line
//          //it will assume it is properly formatted
//          //from there on
//          size_t _min  = str::npos;
//
//          for(; it != nd; it++)
//            {
//              size_t _curr = 0;
//              bool   _counting = true;
//              str    line = *it;
//              for(size_t i = 0; i < line.length(); i++)
//                {
//                  char ch = line[i];
//                  switch(ch)
//                    {
//                      case ' ':
//                        {
//                          if (_counting)
//                            _curr++;
//                          break;
//                        }
//                      case '\t':
//                        {
//                          if (_counting)
//                            _curr += tab_;
//                          break;
//                        }
//                      default:
//                        {
//                          _counting = false;
//                        }
//                    }
//                }
//
//              if (_curr < _min)
//                _min = _curr;
//            }
//
//          //now delete that amont
//          it = lines.begin();
//          for(; it != nd; it++)
//            {
//              //find out the minimum start position per line
//              //it will assume it is properly formatted
//              //from there on
//              size_t _curr = 0;
//              size_t to_erase = 0;
//              str    curr_line = *it;
//              while(to_erase < curr_line.size() && _curr < _min)
//                {
//                  char ch = curr_line[to_erase];
//                  if (ch == ' ')
//                    _curr++;
//                  else if (ch == '\t')
//                    _curr += 4;
//                  else
//                    break;
//
//                  to_erase++;
//                }
//
//              curr_line.erase(0, to_erase);
//              if (!curr_line.empty())
//                {
//                  for(int i = 0; i < indent_*tab_; i++)
//                    curr_line.insert(0, " ");
//                }
//
//              result += curr_line;
//
//							if(!dont_break_ && !curr_line.empty())
//								result += '\n';
//            }
//
//          return result;
//        }
//
//    private:
//      XSSProject								project_;
//      int												indent_;
//      int												tab_;
//      part_list									parts_;
//			bool											dont_break_;
//			str												marker_;
//			std::vector<outfile_info> files_;
//
//      std::vector<str> load_lines(const str& s)
//        {
//          std::vector<str> result;
//          size_t curr = 0;
//          for(size_t i = 0; i < s.length(); i++)
//            {
//              if (s[i] == '\n')
//                {
//                  result.push_back( s.substr(curr, i) );
//                  curr = i;
//                }
//            }
//
//          return result;
//        }
//  };
//
//typedef reference<worker> Worker;
//
//struct worker_schema : object_schema<worker>
//  {
//    virtual void declare()
//      {
//        dynamic_method_( "generate", &worker::generate );
//      }
//  };
//
//namespace xkp
//{
//    register_complete_type(worker, worker_schema);
//}
//
////out_linker
//void out_linker::link(dsl& info, code_linker& owner)
//  {
//    int tab_size = 4;
//    bool dont_break = false;
//    bool do_break = false;
//		bool trim = false;
//
//    std::vector<str> expressions;
//
//    //process parameters
//    variant    indent_value = info.params.get("indent");
//    expression indent_expr;
//    if (!indent_value.empty())
//      {
//        try
//          {
//            indent_expr = indent_value;
//          }
//        catch(...)
//          {
//            param_list error;
//            error.add("id", STypeMismatch);
//            error.add("desc", SExpectingExpression);
//            xss_throw(error);
//          }
//      }
//    else
//      {
//        //we'll always pass the value of indent dynamically (resolved at runtime)
//        //in this case the user did not especify an indent param, so we fake an expression
//        int default_indent   = -1;
//        indent_expr.push_operand(default_indent);
//      }
//
//    //td: utilify
//    variant tab_value = info.params.get("tab_size");
//    if (!tab_value.empty())
//      {
//        expression expr = tab_value;
//        tab_size = owner.evaluate_expression(expr); //td: only contants, lazy me
//      }
//
//    variant dont_break_v = info.params.get("dont_break");
//    if (!dont_break_v.empty())
//      {
//        expression expr = dont_break_v;
//        dont_break = owner.evaluate_expression(expr); //td: only contants, lazy me
//      }
//
//    variant do_break_v = info.params.get("do_break");
//    if (!do_break_v.empty())
//      {
//        expression expr = do_break_v;
//        do_break = owner.evaluate_expression(expr); //td: only contants, lazy me
//      }
//
//		variant trim_v = info.params.get("trim");
//    if (!trim_v.empty())
//      {
//        expression expr = trim_v;
//        trim = owner.evaluate_expression(expr); //td: only contants, lazy me
//      }
//
//		variant marker_v = info.params.get("marker");
//		str			marker;
//		if (!marker_v.empty())
//			{
//        expression expr = marker_v;
//        marker = variant_cast<str>(owner.evaluate_expression(expr), str(""));
//			}
//
//		//process xss
//		part_list parts;
//		std::vector<outfile_info> files;
//		xss_gather gather(parts, files, expressions);
//
//    xss_parser parser;
//    parser.register_tag("xss:e");
//    parser.register_tag("xss:code");
//    parser.register_tag("xss:open_brace");
//    parser.register_tag("xss:close_brace");
//		parser.register_tag("xss:return");
//		parser.register_tag("xss:file");
//
//		str to_parse = info.text;
//		if (trim)
//			boost::trim(to_parse);
//
//		if (do_break)
//				to_parse += '\n';
//
//		parser.parse(to_parse, &gather);
//
//    //now xs
//    xs_utils xs;
//
//    //create a safe reference to be inserted in the execution context later on
//		Worker wrk(new worker(project_, parts, files, tab_size, dont_break, marker));
//    owner.add_instruction(i_load_constant, owner.add_constant(wrk));
//
//    //and so we link the indent, after having the worker on
//    owner.link_expression(indent_expr);
//
//    //push the expression to be used as parameters
//    std::vector<str>::iterator it = expressions.begin();
//    std::vector<str>::iterator nd = expressions.end();
//    for(; it != nd; it++)
//      {
//        expression expr;
//        if (!xs.compile_expression(*it, expr))
//          assert(false);
//
//        owner.link_expression(expr);
//      }
//
//    //for this call, remember the indent is always the forat parameter
//    owner.add_call(wrk, "generate", expressions.size() + 1, false);
//  }
//
