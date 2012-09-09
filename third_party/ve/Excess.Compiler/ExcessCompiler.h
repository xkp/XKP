// ExcessCompiler.h

#pragma once

using namespace System;

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
}
