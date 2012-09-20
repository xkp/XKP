// xss.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <xss/object_model.h>
#include <xss/lang_factory.h>
#include <xss/xss_expression.h>
#include <xss/xss_context.h>
#include <xss/localfs.h>
#include <xs/compiler.h>

#include <boost/algorithm/string.hpp>

using namespace xkp;

class xss_object_model : public IObjectModel
{
	public:
		xss_object_model(FileSystem fs, LanguageFactory languages)
		{
			model_ = ObjectModel(new object_model(fs, languages));
			thread_ = ObjectModelThread(new object_model_thread);
			thread_->run();
		}
	public:
		virtual int load(const std::string& filename)
		{
			fs::path filepath     = filename;
			fs::path project_path = filepath.parent_path();

			param_list args;
			DataReader prj = model_->filesystem()->load_data(filepath);
			Application app = model_->load(prj, args, project_path);

			apps_.push_back(app);
			return apps_.size() - 1;
		}
		
		virtual void addIncludes(int appId, const std::string& def_file, const std::string& src_file)
		{
			model_->add_include(apps_[appId], def_file, src_file);
		}

		virtual bool notifyChange(const std::string& fname, int line, int col, int oldEndLine, int oldEndCol, int newEndLine, int newEndCol)
		{
			document* doc = model_->get_document(fname);
			if (doc)
			{
				doc->changed(line, col, oldEndLine, oldEndCol, newEndLine, newEndCol);	
				return true;		
			}
			return false;		
		}

		virtual void updateChanges(int ms)
		{
			str			fname;
			om_response data;

			while(true)
			{
				bool has_job = thread_->fetch(fname, data);
				if (!has_job)
					break;

				model_->update_document(fname, data);
			}
		}

		virtual void queueChange(const std::string& fname, const std::string& content)
		{
			thread_->compile_request(content, fname);
		}

		struct walker : context_visitor
		{
			walker(walk_list& results):
			results_(results)
			{
			}
			
			virtual void visit(RESOLVE_ITEM type, const std::string& id, variant value)
			{
				switch(type)
				{
					case RESOLVE_ANY:
					case RESOLVE_NATIVE:
					case RESOLVE_CONST:
						return; //ignore & insert nothing
				}

				int tid = (int)type;
				
				if (!id.empty())
					results_.push_back(walk_info(tid, id));
			}

			private:
				walk_list& results_;
		};

		virtual void walkContext(const std::string& fname, const std::string& text, int line, int col, walk_list& results)
		{
			document* doc = model_->get_document(fname);
			if (doc)
			{
				XSSContext ctx = doc->find_context(line, col); assert(ctx);
				
				std::string s = find_expression(text);
				walker		w(results);
				if (!s.empty())
				{
					XSSExpression expr = xss_expression_utils::compile_expression(s);
					if (expr)
					{
						expr->bind(ctx);
						XSSValue val = expr->value();
						if (!val)
							return; //nothing to see here
						
						value_operation op = val->get_last();
						RESOLVE_ITEM	ri = op.resolve_id();

						switch(ri)
						{
							case RESOLVE_INSTANCE:
							case RESOLVE_TYPE:
							case RESOLVE_VARIABLE:
							{
								XSSObject obj = variant_cast<XSSObject>(op.resolve_value(), XSSObject());
								if (obj)
								{
									obj->context_visit(&w);
								}
								break;
							}
						}
					}
				}
				else
					ctx->visit(&w); //get all
			}
		}

		struct error_walker : error_visitor
		{
			error_walker(xss_error_info_list& results):
			results_(results)
			{
			}
			
			virtual void visit(error_info& err)
			{
				std::ostringstream ss;
				ss << err.desc;
				if (!err.info.empty())
				{
					ss << " (";

					bool first = true;
					for(size_t i = 0; i < err.info.size(); i++)
					{
						if (first)
							first = false;
						else 
							ss << ", ";

						std::string name  = err.info.get_name(i);
						std::string value = xss_utils::var_to_string(err.info.get(i));
						ss << name << " : " << value;
					}

					ss << ") ";
				}

				results_.push_back(xss_error_info(ss.str(), err.loc.begin.line, err.loc.begin.column, err.loc.end.line, err.loc.end.column));
			}

			private:
				xss_error_info_list& results_;
		};

		virtual void walkErrors(const std::string& fname, xss_error_info_list& errors)
		{
			error_walker ew(errors);
			model_->visit_errors(fname, &ew);
		}
	private:
		ObjectModel				 model_;
		ObjectModelThread		 thread_;
		std::vector<Application> apps_;

		std::string find_expression(const std::string& text)
		{
			//td: !!! be serious, please
			std::string result;

			if (text.empty())
				return result;

			std::string::const_reverse_iterator it = text.rbegin();
			std::string::const_reverse_iterator nd = text.rend();

			bool finding_last = true;
			for(; it != nd; it++)
			{
				char c = *it;

				if (c == '.' && finding_last)
				{
					finding_last = false;
					continue;
				}

				if (!isalpha(c) && c != '_')
					break;
				
				if (!finding_last)
					result.insert(result.begin(), c); 
			}

			return result;
		}
};


XSS_API IObjectModel* __stdcall create_object_model()
{
    FileSystem		fs(new local_filesystem);
    LanguageFactory lf(new lang_factory);
	xs_compiler		xs;

	return new xss_object_model(fs, lf); 
}

XSS_API wchar_t* __stdcall  get_token(wchar_t *InputBuf, long InputSize, long *InputHere, long *Line, long *Column, int *Symbol)
{
	return xkp::grammar_utils::GetToken(InputBuf, InputSize, InputHere, Line, Column, Symbol);
}
