// ExcessCompiler.h

#pragma once

using namespace System;
#using <mscorlib.dll>
using namespace System::Runtime::InteropServices; 

#include "../xss/interface.h"
#include <map>

using namespace System::Collections::Generic;

[DllImport("xss.dll", EntryPoint = "create_object_model")]
IObjectModel* create_object_model();

[DllImport("xss.dll", EntryPoint = "get_token")]
wchar_t* get_token(wchar_t *InputBuf, long InputSize, long *InputHere, long *Line, long *Column, int *Symbol);

namespace ExcessCompiler {

	public enum TokenType
	{
		Keyword,
		CharLiteral,
		Number,
		Identifier,
		Delimiter,
		Comment,
		Operator,
		Unknown,
	};

	public ref class Token
	{
		public:	
			Token(String^ text, int symbol, int _StartLine, int _EndLine, int _StartColumn, int _EndColumn):
				text_(text),
				symbol_(symbol),
				StartLine(_StartLine),
				EndLine(_EndLine),
				StartColumn(_StartColumn),
				EndColumn(_EndColumn)
			{
			}

			int StartLine;
			int EndLine;
			int StartColumn;
			int EndColumn;

			String^   getText();
			int		  getSymbol();
			TokenType classify();
		private:
			String^ text_;
			int     symbol_;
	};

	public ref class Tokenizer
	{
		public:
			Tokenizer(String^ s);
			~Tokenizer();
		public:
			Token^ Next();
		private:
			IntPtr input_;
			long   len_;
			long   curr_;
			bool   comment_;
	};

	public ref class Compiler
	{
		public:
			Compiler();
		public:
			Tokenizer^ CreateTokenizer(String^ s);
	};

	public enum class CompletionType
	{
		Property,
		Method,
		Event,
		Variable,
		Type,
		Instance,
	};

	public ref class ExcessCompletionItem
	{
		public:
			ExcessCompletionItem(String^ item, String^ desc, CompletionType id):
			Item(item),
			Desc(desc),
			Id(id)
			{
			}

			ExcessCompletionItem(ExcessCompletionItem% other):
			Item(other.Item),
			Desc(other.Desc),
			Id(other.Id)
			{
			}
		public:
			String^		   Item;
			String^		   Desc;
			CompletionType Id;
	};

	public ref class ExcessErrorInfo
	{
		public:
			ExcessErrorInfo(String^ _desc, int _BeginLine, int	_BeginColumn, int _EndLine, int	_EndColumn):
			desc(_desc),
			BeginLine(_BeginLine),
			BeginColumn(_BeginColumn),
			EndLine(_EndLine),
			EndColumn(_EndColumn)
			{
			}

		String^ desc;
		int		BeginLine;
		int		BeginColumn;
		int		EndLine;
		int		EndColumn;
	};

	public ref class ExcessModel
	{
		public:	
			ExcessModel();
			~ExcessModel();
		public:	
			int  loadProject(String^ filename, String^ path);
			void unloadProject(Guid project);
			bool buildProject(String^ filename);
			bool buildAll();
            void addInclude(String^ projectPath, String^ def, String^ src);
            void notifyChange(String^ filename, String^ contents, int line, int col, int oldEndLine, int oldEndCol, int newEndLine, int newEndCol);
			void updateChanges();

			//access
			List<ExcessCompletionItem^>^ getCompletion(String^ filename, String^ text, int line, int col);
            List<ExcessErrorInfo^>^		 getErrors(String^ filename);
		private:
			IObjectModel*				model_; //wtf, no smart pointers?
			std::map<std::string, int>*	projects_;
			List<String^>				dirt_content_;
			std::map<std::string, int>*	dirt_;
			std::map<int, std::string>*	dirt_index_;
			
			CompletionType getCompletionType(int id);
	};
}
