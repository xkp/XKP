// This is the main DLL file.

#include "stdafx.h"

#include "ExcessCompiler.h"
#include <string>
#include <wchar.h>
#include "vcclr.h"

#include <xs/compiler.h>

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
	wchar_t* result = xkp::grammar_utils::GetToken(input, len_, &curr, &line, &end_column, &symbol);

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
    //Grammar.CaseSensitive = False;
    //Grammar.InitialSymbol = 132;
    //Grammar.InitialDfaState = 0;
    //Grammar.InitialLalrState = 0;
    //Grammar.SymbolCount = 150;
    //Grammar.SymbolArray = GrammarSymbolArray;
    //Grammar.RuleCount = 191;
    //Grammar.RuleArray = GrammarRuleArray;
    //Grammar.DfaStateCount = 235;
    //Grammar.DfaArray = GrammarDfaStateArray;
    //Grammar.LalrStateCount = 372;
    //Grammar.LalrArray = GrammarLalrStateArray;
}

Tokenizer^ Compiler::CreateTokenizer(String^ s)
{
	return gcnew Tokenizer(s);
}

}