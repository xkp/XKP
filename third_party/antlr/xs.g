grammar xs;

options {
	language=C;
    backtrack = true;
    memoize = true;
}


//------------------------------------------------- Main
program
	:	block
	|	constructList
	|	expression
	;


//------------------------------------------------- High level constructs
constructList
	:	construct+
	;

constructBlock
	:	'{' constructList? '}'
	;

adjList
	:	adjective+
	;

adjective
	:	'delegate'
	|	'public'
	|	'private'
	|	'class'
	;

argDeclListOpt
	:	argDeclList?
	;

argDeclList
	:	argumentDecl (',' argumentDecl)*
	;

argumentDecl
	:	type? Identifier ('=' expression)?
	;

classArgDeclList
	:	classArgumentDecl (',' classArgumentDecl)*
	;

classArgumentDecl
	:	Identifier ('=' primaryExpr)?
	;

classArgListOpt
	:	('<' classArgDeclList '>')?
	;

construct
	:	adjList? property
	|	adjList? methodDecl
	|	'instance' qualifiedID typeOpt constructBlock
	|	'class' Identifier classArgListOpt typeOpt constructBlock
	|	'behaviour' Identifier typeOpt constructBlock
	|	'on' qualifiedID '(' argDeclListOpt ')' block
	|	'const' Identifier '=' expression ';'
	|	'event' Identifier '(' argDeclListOpt ')' ';'
	|	'behave' 'as' Identifier block
	|	'behave' 'as' Identifier ';'
	|	expression '->' expression ';'
	|	dsl
	;

property
	:	'property' Identifier typeOpt propDescriptor
	|	localVarDecl ';'
	;

methodName
	:	'method'
	|	'function'
	;

methodDecl
	:	methodName Identifier '(' argDeclListOpt ')' typeOpt block
	|	type Identifier '(' argDeclListOpt ')' block
	;

typeOpt
	:	(':' type)?
	;

propDescriptor
	:	block
	|	'=' block block?
	|	'=' expression block
	|	('=' expression)? ';'
	;

	
//------------------------------------------------- Code
stmList
	:	statement+
	;

statement
	:	localVarDecl ';'
	|	'if' '(' expression ')' statement ('else' statement)?
	|	'for' '(' forInitOpt ';' forConditionOpt ';' forIteratorOpt ')' statement
	|	'for' '(' type Identifier 'in' expression ')' statement
	|	'while' '(' expression ')' statement
	|	'switch' '(' switchHeader ')' '{' switchSectionsOpt '}'
	|	'dispatch' qualifiedID '(' argListOpt ')' ';'
	|	'break' ';'
	|	'continue' ';'
	|	'return' expressionOpt ';'
	|	'try' block catchClauses finallyClauseOpt
	|	'throw' expression ';'
	|	expression ';'
	|	block
	|	dsl
	;

catchClauses
	:	catchClause*
	;

catchClause
	:	'catch' ('(' type Identifier? ')')? block
	;

finallyClauseOpt
	:	('finally' block)?
	;

block
	:	'{' stmList? '}'
	;

localVarDecl
	:	type variableDeclarator
	;

variableDeclarator
	:	Identifier ('=' expression)?
	;

switchHeader
	:	expression?
	;

switchSectionsOpt
	:	switchSection*
	;

switchSection
	:	switchLabels stmList
	;

switchLabels
	:	switchLabel+
	;

switchLabel
	:	'case' expression ':'
	|	'default' ':'
	;

forInitOpt
	:	(localVarDecl | expression)?
	;

forIteratorOpt
	:	expression?
	;

forConditionOpt
	:	expression?
	;


//------------------------------------------------- DSLs
dsl
	:	DSLIdentifier (Identifier? '(' argListOpt ')')? '@' DecLiteral
	;


//------------------------------------------------- Expressions
expressionOpt
	:	expression?
	;

expressionList
	:	expression (',' expression)*
	;

expression
	:	conditionalExpr (('='|'+='|'-='|'*='|'/='|'<<='|'>>=') conditionalExpr)*
	;

conditionalExpr	
	:	orExpr ('?' orExpr ':' conditionalExpr)?
	;

orExpr
	:	andExpr ('||' andExpr)*
	;

andExpr
	:	equalityExpr ('&&' equalityExpr)*
	;

equalityExpr
	:	compExpr (('=='|'!=') compExpr)*
	;

compExpr
	:	shiftExpr (('<'|'>'|'<='|'>='|'as'|'is'|'has') shiftExpr)*
	;

shiftExpr
	:	addExpr (('<<'|'>>') addExpr)*
	;

addExpr
	:	multExpr (('+'|'-') multExpr)*
	;

multExpr
	:	unaryExpr (('*'|'/'|'%') unaryExpr)*
	;

unaryExpr
	:	'!' unaryExpr
	|	'-' unaryExpr
	|	'++' unaryExpr
	|	'--' unaryExpr
	|	methodExpr
	;

methodExpr
	:	primaryExpr+ method*
	;

method
	:	memberName ( '(' argListOpt ')' )?
	|	'[' expressionList ']'
	|	'++'
	|	'--'
	;

objectListOpt
	:	objectList?
	;

objectList
	:	objectValue (',' objectValue)*;

objectValue
	:	Identifier ':' expression
	;

primaryExpr
	:	primary
	|	'new' type '(' argListOpt ')'
	|	'{' objectListOpt '}'
	|	'(' expression ')'
	;

arrayLiteral
	:	'[' argListOpt ']'
	;

primary
	:	Identifier ( '(' argListOpt ')' )?
	|	arrayLiteral
	|	Literal
	;


//------------------------------------------------- Types
type
	:	Identifier ('<' typeArgList '>')?
	;

typeArgList
	:	typeArgument (',' typeArgument)*
	;

typeArgument
	:	(Identifier '=')? primaryExpr
	;


//------------------------------------------------- Arguments
argListOpt
	:	argList?
	;

argList
	:	argument (',' argument)*
	;

argument
	:	(Identifier '=')? conditionalExpr
	;


//------------------------------------------------- Literals
Literal
	:	'true'
	|	'false'
	|	'null'
	|	DecLiteral
	|	HexLiteral
	|	RealLiteral
	|	CharLiteral
	|	StringLiteral
	;

DSLIdentifier
	:	'@' Identifier
	;

memberName
	:	'.' Identifier
	;

qualifiedID
	:	Identifier memberList
	;

memberList
	:	memberName*
	;

Identifier
	:	(LETTER | '_') (LETTER | DIGIT | '_')*
	;

fragment
DecLiteral
	:	DIGIT+
	;

fragment
HexLiteral
	:	'0' ('x'|'X') HEX_DIGIT
	;

fragment
RealLiteral
	:	DIGIT* '.' DecLiteral
    ;

fragment
CharLiteral
	:	'\'' ( ESC_SEQ | ~('\''|'\\') ) '\''
	;

fragment
StringLiteral
	:	'"' ( ESC_SEQ | ~('\\'|'"') )* '"'
	;

fragment
LETTER
	:	('a'..'z'|'A'..'Z')
	;

WS  
	:	(' '|'\t'|'\r'|'\n') {$channel=HIDDEN;}
	;

COMMENT
    :   '//' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    |   '/*' ( options {greedy=false;} : . )* '*/' {$channel=HIDDEN;}
    ;

fragment
ESC_SEQ
	:	'\\' ('b'|'t'|'n'|'f'|'r'|'\"'|'\''|'\\')
	|	UNICODE_ESC
	;

fragment
UNICODE_ESC
	:	'\\' 'u' HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT
	;

fragment
HEX_DIGIT
	:	(DIGIT|'a'..'f'|'A'..'F') 
	;

fragment
DIGIT
	:	'0'..'9'
	;
