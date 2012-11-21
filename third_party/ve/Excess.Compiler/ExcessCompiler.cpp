// This is the main DLL file.

#include "stdafx.h"

#include "ExcessCompiler.h"
#include <string>
#include <wchar.h>
#include "vcclr.h"

#include <time.h>

using namespace System::Runtime::InteropServices;

namespace ExcessCompiler {

//Token
String^ Token::getText()
{
	return text_;
}

int	Token::getSymbol()
{
	return symbol_;
}

TokenType Token::classify()
{
	switch(symbol_)
	{
		case 3: //{5,L"Comment End"},
		case 4: //{6,L"Comment Line"},
		case 5: //{4,L"Comment Start"},
			return Comment;
		case 6: //{1,L"-"},
		case 7: //{1,L"--"},
		case 8: //{1,L"!"},
		case 9: //{1,L"!="},
		case 10: //{1,L"%"},
		case 11: //1,L"&&"},
		case 14: //{1,L"*"},
		case 15: //{1,L"*="},
		case 17: //{1,L"/"},
		case 18: //{1,L"/="},
		case 26: //{1,L"||"},
		case 28: //{1,L"+"},
		case 29: //{1,L"++"},
		case 30: //{1,L"+="},
		case 31: //{1,L"<"},
		case 32: //{1,L"<<"},
		case 33: //{1,L"<<="},
		case 34: //{1,L"<="},
		case 35: //{1,L"="},
		case 36: //{1,L"-="},
		case 37: //{1,L"=="},
		case 38: //{1,L">"},
		case 39: //{1,L"->"},
		case 40: //{1,L">="},
		case 41: //{1,L">>"},
		case 42: //{1,L">>="},
			return Operator;

		case 12: //{1,L"("},
		case 13: //{1,L")"},
		case 16: //{1,L","},
		case 19: //{1,L":"},
		case 20: //{1,L";"},
		case 21: //{1,L"?"},
		case 22: //{1,L"@"},
		case 23: //{1,L"["},
		case 24: //{1,L"]"},
		case 25: //{1,L"{"},
		case 27: //{1,L"}"},
			return Delimiter;
		
		case 43: //{1,L"as"},
		case 44: //{1,L"behave"},
		case 45: //{1,L"behaviour"},
		case 46: //{1,L"break"},
		case 47: //{1,L"case"},
		case 48: //{1,L"catch"},
		case 50: //{1,L"class"},
		case 51: //{1,L"const"},
		case 52: //{1,L"continue"},
		case 54: //{1,L"default"},
		case 55: //{1,L"delegate"},
		case 56: //{1,L"dispatch"},
		case 58: //{1,L"else"},
		case 59: //{1,L"event"},
		case 60: //{1,L"false"},
		case 61: //{1,L"finally"},
		case 62: //{1,L"for"},
		case 63: //{1,L"function"},
		case 64: //{1,L"has"},
		case 67: //{1,L"if"},
		case 68: //{1,L"in"},
		case 69: //{1,L"instance"},
		case 70: //{1,L"is"},
		case 72: //{1,L"method"},
		case 73: //{1,L"new"},
		case 74: //{1,L"null"},
		case 75: //{1,L"on"},
		case 76: //{1,L"private"},
		case 77: //{1,L"property"},
		case 78: //{1,L"public"},
		case 80: //{1,L"return"},
		case 82: //{1,L"switch"},
		case 83: //{1,L"throw"},
		case 84: //{1,L"true"},
		case 85: //{1,L"try"},
		case 86: //{1,L"while"},
			return Keyword;
		
		case 49: //{1,L"CharLiteral"},
		case 81: //{1,L"StringLiteral"},
			return CharLiteral;

		case 53: //{1,L"DecLiteral"},
		case 65: //{1,L"HexLiteral"},
		case 79: //{1,L"RealLiteral"},
			return Number;
		
		case 66: //{1,L"Identifier"},
		case 71: //{1,L"MemberName"},
			return Identifier;
	}

	return Unknown;
}

//Tokenizer
Tokenizer::Tokenizer(String^ s):
curr_(0),
comment_(false)
{
    input_ = Marshal::StringToHGlobalUni(s);
	len_ = s->Length;
}

Tokenizer::~Tokenizer()
{
	Marshal::FreeHGlobal(input_);
}

Token^ Tokenizer::Next()
{
	wchar_t* input = static_cast<wchar_t*>(input_.ToPointer());
	if (!input || curr_ >= len_)
		return nullptr;

	long line, end_column;
	int symbol;
	long curr = curr_;
	wchar_t* result = get_token(input, len_, &curr, &line, &end_column, &symbol);

	if (symbol == 5 || symbol == 4 || symbol == 3)
		comment_ = true;

	long column = curr_;
	curr_  = curr;
    String^ text = gcnew String(result);

	free(result);
	return gcnew Token(text, comment_? 5 : symbol, 1, 1, column, curr_);	
}

//Compiler
Compiler::Compiler()
{
}

Tokenizer^ Compiler::CreateTokenizer(String^ s)
{
	return gcnew Tokenizer(s);
}

//ExcessModel
ExcessModel::ExcessModel()
{
	model_ = create_object_model();
	projects_ = new std::map<std::string, int>();
	dirt_ = new std::map<std::string, int>();
	dirt_index_ = new std::map<int, std::string>();
}

ExcessModel::~ExcessModel()
{
	delete model_;
	delete projects_;
	delete dirt_;
	delete dirt_index_;
}

struct StringUtils
{
	static String^ toString(const std::string& s)
	{
		String^ result = gcnew String(s.c_str());
		return result;
	}

	static std::string fromString(String^ s)
	{
		const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
		std::string result(chars);
		Marshal::FreeHGlobal(IntPtr((void*)chars));	
		return result;
	}
};

int ExcessModel::loadProject(String^ filename, String^ path)
{
	std::string fname = StringUtils::fromString(filename);
	int appId = model_->load(fname);
	projects_->insert(std::pair<std::string, int>(fname, appId));

	return appId;
}

String^ ExcessModel::getAppName(String^ filename)
{
	return StringUtils::toString(model_->appName(StringUtils::fromString(filename)));
}

void ExcessModel::unloadProject(Guid project)
{
}

bool ExcessModel::buildProject(String^ filename, List<ExcessErrorInfo^>^ errors)
{
	std::vector<xss_error_info> _errors;
	bool result = model_->buildProject(StringUtils::fromString(filename), _errors);
	std::vector<xss_error_info>::iterator it = _errors.begin();
	std::vector<xss_error_info>::iterator nd = _errors.end();
	for(; it != nd; it++)
		errors->Add(gcnew ExcessErrorInfo(StringUtils::toString(it->desc), StringUtils::toString(it->file), it->sline, it->scol, it->eline, it->ecol));

	return result;
}

bool ExcessModel::buildAll()
{
	return false;
}

void ExcessModel::addInclude(String^ project, String^ def, String^ src)
{
	std::string prj		 = StringUtils::fromString(project);
	std::string def_file = StringUtils::fromString(def);
	std::string src_file = StringUtils::fromString(src);

	std::map<std::string, int>::iterator it = projects_->find(prj);
	if (it != projects_->end())
	{
		model_->addIncludes(it->second, def_file, src_file);
	}
}

void ExcessModel::notifyChange(String^ filename, String^ contents, int line, int col, int oldEndLine, int oldEndCol, int newEndLine, int newEndCol)
{
	std::string fname = StringUtils::fromString(filename);
	if (model_->notifyChange(fname, line, col, oldEndLine, oldEndCol, newEndLine, newEndCol))
	{
		std::map<std::string, int>::iterator it = dirt_->find(fname);
		if (it != dirt_->end())
		{
			dirt_content_[it->second] = contents;
			//td: record change information
		}
		else
		{
			dirt_->insert(std::pair<std::string, int>(fname, dirt_content_.Count));
			dirt_index_->insert(std::pair<int, std::string>(dirt_content_.Count, fname));
			dirt_content_.Add(contents);
		}
	}
}

bool ExcessModel::updateChanges()
{
	bool changed = model_->updateChanges(1000); //give it a second or so

	//queue jobs
	for(int i = dirt_content_.Count - 1; i >= 0; i--)
	{
		std::map<int, std::string>::iterator it = dirt_index_->find(i); 
		if (it != dirt_index_->end())
			model_->queueChange(it->second, StringUtils::fromString(dirt_content_[i]));
	}

	//clear dirt
	dirt_content_.Clear();
	dirt_->clear();
	dirt_index_->clear();
	return changed;

	//clock_t start = clock();
	//while(dirt_.Count > 0)
	//{
	//	DirtResults^ dirt = dirt_[0];
	//	XSSContext   ctx  = dirt_ctx_->at(dirt->CtxId);

	//	dirt_.RemoveAt(0);
	//	dirt_ctx_->erase(dirt_ctx_->begin() + dirt->CtxId);

	//	assert(false);//td: 

	//	clock_t elapsed = clock() - start;
	//	if (elapsed < 0 || elapsed > 1000)
	//		break;
	//}
}

void ExcessModel::addSearchPath(String^ path)
{
  std::string str_path = StringUtils::fromString(path);
  model_->addSearchPath(str_path);
}

List<ExcessCompletionItem^>^ ExcessModel::getCompletion(String^ filename, String^ text, int line, int col)
{
	List<ExcessCompletionItem^>^ result = gcnew List<ExcessCompletionItem^>();

	std::vector<walk_info> info;
	model_->walkContext(StringUtils::fromString(filename), StringUtils::fromString(text), line, col, info);

	std::vector<walk_info>::iterator it = info.begin();
	std::vector<walk_info>::iterator nd = info.end();
	for(; it != nd; it++)
		result->Add(gcnew ExcessCompletionItem(StringUtils::toString(it->symbol), "", getCompletionType(it->type)));
	return result;
}

List<ExcessErrorInfo^>^	ExcessModel::getErrors(String^ filename)
{
	std::vector<xss_error_info> errors;
	model_->walkErrors(StringUtils::fromString(filename), errors);

	//www
	List<ExcessErrorInfo^>^ result = gcnew List<ExcessErrorInfo^>();
	std::vector<xss_error_info>::iterator it = errors.begin();
	std::vector<xss_error_info>::iterator nd = errors.end();
	for(; it != nd; it++)
		result->Add(gcnew ExcessErrorInfo(StringUtils::toString(it->desc), StringUtils::toString(it->file), it->sline, it->scol, it->eline, it->ecol));
	return result;
}

List<ExcessErrorInfo^>^	ExcessModel::getAllErrors()
{
	std::vector<xss_error_info> errors;
	model_->walkErrors(std::string(), errors);

	//www
	List<ExcessErrorInfo^>^ result = gcnew List<ExcessErrorInfo^>();
	std::vector<xss_error_info>::iterator it = errors.begin();
	std::vector<xss_error_info>::iterator nd = errors.end();
	for(; it != nd; it++)
		result->Add(gcnew ExcessErrorInfo(StringUtils::toString(it->desc), StringUtils::toString(it->file), it->sline, it->scol, it->eline, it->ecol));
	return result;
}

CompletionType ExcessModel::getCompletionType(int id)
{
	const int RESOLVE_INSTANCE	= 1;
    const int RESOLVE_METHOD	= 2;
    const int RESOLVE_PROPERTY	= 3;
    const int RESOLVE_EVENT		= 4;
    const int RESOLVE_VARIABLE	= 7;
    const int RESOLVE_TYPE		= 8;

	switch(id)
	{
		case RESOLVE_INSTANCE:	return CompletionType::Instance;
		case RESOLVE_METHOD:	return CompletionType::Method;
		case RESOLVE_PROPERTY:	return CompletionType::Property;
		case RESOLVE_EVENT:		return CompletionType::Event;
		case RESOLVE_VARIABLE:	return CompletionType::Variable;
		case RESOLVE_TYPE:		return CompletionType::Type;
	}

	return CompletionType::Type;
}

}


