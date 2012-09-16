// xss.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <xss/object_model.h>
#include <xss/lang_factory.h>
#include <xss/localfs.h>
#include <xs/compiler.h>

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

	private:
		ObjectModel				 model_;
		ObjectModelThread		 thread_;
		std::vector<Application> apps_;
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
