%{

/* Homework Final For Cs 442 By Kai Wu Version 1.0.0 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yaccExample.h"
#include "Semantics.h"
#include "CodeGen.h"
#include "typeFile.h"

extern int yylex();	/* The next token function. */
extern char *yytext;   /* The matched token text.  */
extern int yyleng;      /* The token text length.   */
extern int yyparse();
void dumpTable();

extern struct SymTab *table;
extern struct SymTab *argumentTable;
extern struct SymEntry *entry;
//	extern int * variables;

%}


%union {
  long val;
  char * string;
  struct ExprRes * ExprRes;
  struct ExprResList *ExprResList;
  struct InstrSeq * InstrSeq;
  struct CondRes * CondRes;
}

%type <string> Id
%type <string> ArraySize
%type <ExprRes> Term
%type <ExprRes> Expr
%type <ExprRes> Factor
%type <ExprResList> Exprtail
%type <ExprResList> Exprls
%type <InstrSeq> StmtSeq
%type <InstrSeq> Functions
%type <InstrSeq> Functionstail
%type <InstrSeq> Stmt
%type <InstrSeq> ArgumentList
%type <InstrSeq> ArgumentTail
%type <InstrSeq> Argument
%type <CondRes> Cond

%token Ident 		
%token IntLit
%token FloatLit
%token Int
%token Float
%token Write
%token WriteLn
%token WriteSpace
%token Read
%token EQ
%token IF
%token ELSE
%token WHILE
%token NEQ
%token Boolean
%token BooleanLit
%token NOT
%token OR
%token AND
%token LT
%token GT
%token LTE
%token GTE
%token MAIN
%token VOID

%%

Prog		:	Declarations StmtSeq						{Finish($2); } ;
Declarations	:	Dec Declarations						{ };
Declarations	:											{ };
Dec		:	Int Id';'										{struct typeInfo *attribute;
															EnterName(table, $2, &entry); 
															attribute = (struct typeInfo *)malloc(sizeof(struct typeInfo));
															attribute->type = INT;
															attribute -> localVariable = 0;
															SetAttr(entry, attribute);}
Dec		:	Float Id';'										{struct typeInfo *attribute;
															EnterName(table, $2, &entry); 
															attribute = (struct typeInfo *)malloc(sizeof(struct typeInfo));
															attribute->type = FLOAT;
															attribute -> localVariable = 0;
															SetAttr(entry, attribute); }
Dec		:	Boolean Id';'									{struct typeInfo *attribute;
															EnterName(table, $2, &entry); 
															attribute = (struct typeInfo *)malloc(sizeof(struct typeInfo));
															attribute->type = BOOLEAN;
															attribute -> localVariable = 0;
															SetAttr(entry, attribute); }
Dec		:	Int Id '[' ArraySize ']' ';'					{struct typeInfo *attribute;
															EnterName(table, $2, &entry); 
															attribute = (struct typeInfo *)malloc(sizeof(struct typeInfo));
															attribute->type = INTARRAY;
															attribute -> localVariable = 0;
															attribute -> arraysize = $4;
															printf("the size is ::%s", attribute -> arraysize);
															SetAttr(entry, attribute); }
Dec		:	Float Id '[' ArraySize ']' ';'					{struct typeInfo *attribute;
															EnterName(table, $2, &entry); 
															attribute = (struct typeInfo *)malloc(sizeof(struct typeInfo));
															attribute->type = FLOATARRAY;
															attribute -> localVariable = 0;
															attribute -> arraysize = getArraySize($4);
															SetAttr(entry, attribute); }
Dec		:	Boolean Id '[' ArraySize ']' ';'				{struct typeInfo *attribute;
															EnterName(table, $2, &entry); 
															attribute = (struct typeInfo *)malloc(sizeof(struct typeInfo));
															attribute->type = BOOLEANARRAY;
															attribute -> localVariable = 0;
															attribute -> arraysize = $4;
															SetAttr(entry, attribute); }
LocalDeclarations	:	LocalDec LocalDeclarations			{ };
LocalDeclarations	:										{ };
LocalDec		:	Int Id';'								{struct typeInfo *attribute;
															EnterName(table, $2, &entry); 
															attribute = (struct typeInfo *)malloc(sizeof(struct typeInfo));
															attribute->type = INT;
															attribute -> localVariable = 1;
															SetAttr(entry, attribute);}
LocalDec		:	Float Id';'								{struct typeInfo *attribute;
															EnterName(table, $2, &entry); 
															attribute = (struct typeInfo *)malloc(sizeof(struct typeInfo));
															attribute->type = FLOAT;
															attribute -> localVariable = 1;
															SetAttr(entry, attribute); }
LocalDec		:	Boolean Id';'							{struct typeInfo *attribute;
															EnterName(table, $2, &entry); 
															attribute = (struct typeInfo *)malloc(sizeof(struct typeInfo));
															attribute->type = BOOLEAN;
															attribute -> localVariable = 1;
															SetAttr(entry, attribute); }
LocalDec		:	Int Id '[' ArraySize ']' ';'			{struct typeInfo *attribute;
															EnterName(table, $2, &entry); 
															attribute = (struct typeInfo *)malloc(sizeof(struct typeInfo));
															attribute->type = INTARRAY;
															attribute -> localVariable = 1;
															attribute -> arraysize = $4;
															printf("the size is ::%s", attribute -> arraysize);
															SetAttr(entry, attribute); }
LocalDec		:	Float Id '[' ArraySize ']' ';'			{struct typeInfo *attribute;
															EnterName(table, $2, &entry); 
															attribute = (struct typeInfo *)malloc(sizeof(struct typeInfo));
															attribute->type = FLOATARRAY;
															attribute -> localVariable = 1;
															attribute -> arraysize = getArraySize($4);
															SetAttr(entry, attribute); }
LocalDec		:	Boolean Id '[' ArraySize ']' ';'		{struct typeInfo *attribute;
															EnterName(table, $2, &entry); 
															attribute = (struct typeInfo *)malloc(sizeof(struct typeInfo));
															attribute->type = BOOLEANARRAY;
															attribute -> localVariable = 1;
															attribute -> arraysize = $4;
															SetAttr(entry, attribute); }
StmtSeq		:	MAIN '(' ')' '{' LocalDeclarations StmtSeq '}'	Functionstail	{$$ = doMainFuctions($6); };
Functionstail	:	Functions Functionstail				{}
Functionstail	:							{}
Functions	:	VOID Id '(' ArgumentList ')' '{' LocalDeclarations StmtSeq '}'	{ $$ = doVoidFuctions($2,$4,$8); };
ArgumentList:	Argument ArgumentTail			{ $$ = doLinkInstrs($1, $2); };
ArgumentTail:	',' Argument ArgumentTail		{ $$ = doLinkInstrs($2, $3); };
ArgumentTail:									{ $$ = NULL; };
Argument	:	Int Id							{ $$ = doGeneralArgument($2); }; 
Argument	:	Float Id						{ $$ = doGeneralArgument($2); };
Argument	:	Boolean Id						{ $$ = doGeneralArgument($2); };
Argument	:	Int Id '[' ']'					{ $$ = doGeneralArgument($2); };
Argument	:	Float Id '[' ']'				{ $$ = doGeneralArgument($2); };
Argument	:	Boolean Id '[' ']'				{ $$ = doGeneralArgument($2); };
Argument	:	Int '&' Id							{ $$ = doRefArgument($3); }; 
Argument	:	Float '&' Id						{ $$ = doRefArgument($3); };
Argument	:	Boolean '&' Id						{ $$ = doRefArgument($3); };
Argument	:	Int '&' Id '[' ']'					{ $$ = doRefArgument($3); };
Argument	:	Float '&' Id '[' ']'				{ $$ = doRefArgument($3); };
Argument	:	Boolean '&' Id '[' ']'				{ $$ = doRefArgument($3); };
Argument	:									{ $$ = NULL;} ;
StmtSeq 	:	Stmt StmtSeq					{ $$ = AppendSeq($1, $2); } ;
StmtSeq		:									{ $$ = NULL;} ;
Stmt		:	Id '('  Exprls ')' ';'			{ $$ = callfc ($1, $3); };
Stmt		:	Write Exprls ';'				{ $$ = doPrint($2); };
Stmt		:	Read Id ';'						{ $$ = doRead($2); };
Stmt		:	WriteLn ';'						{ $$ = doPrintLn(); };
Stmt		:	WriteSpace ';'					{ $$ = doPrintSpace(); };
Stmt		:	Id '=' Expr ';'					{ $$ = doAssign($1, $3);} ;
Stmt		:	Id '[' Expr ']' '=' Expr ';'	{ $$ = doArrayAssign ($1, $3, $6); };
Stmt		:	IF Cond   '{' StmtSeq '}'		{ $$ = doIf($2, $4); };
Stmt		:	IF Cond '{' StmtSeq '}' ELSE '{' StmtSeq '}' {$$ = doElse($2,$4,$8); };
Stmt		: 	WHILE Cond '{' StmtSeq '}'      {$$ = doWhile($2,$4);};
Cond		:	'(' Expr EQ Expr ')'			{ $$ = doEqual($2, $4); };
Cond		:	'(' Expr NEQ Expr ')'			{ $$ = doNotEqual($2, $4); };
Cond		:	'(' Expr LT Expr ')'			{ $$ = doLessThan($2, $4); };
Cond		:	'(' Expr GT Expr ')'			{ $$ = doGreaterThan($2, $4); };
Cond		:	'(' Expr LTE Expr ')'			{ $$ = doLessThanEqual($2, $4); };
Cond		:	'(' Expr GTE Expr ')'			{ $$ = doGreaterThanEqual($2, $4); };
Exprls		:	   Expr Exprtail				{ $$ = doLink($1,$2); };
Exprls		:									{ $$ = NULL; };
Exprtail	:   ','Expr Exprtail				{ $$ = doLink($2,$3); };
Exprtail	:									{ $$ = NULL; };
Expr		:	'(' Term NEQ Expr ')'			{ $$ = doNEQ($2, $4); };
Expr		:	'(' Term EQ Expr ')'			{ $$ = doEQ($2, $4); };
Expr		:	'(' Term LT Expr ')'			{ $$ = doLT($2, $4); };
Expr		:	'(' Term GT Expr ')'			{ $$ = doGT($2, $4); };
Expr		:	'(' Term LTE Expr ')'			{ $$ = doLTE($2, $4); };
Expr		:	'(' Term GTE Expr ')'			{ $$ = doGTE($2, $4); };
Expr		:	 NOT Expr						{ $$ = doNOT($2); };
Expr		:	 Term OR Expr					{ $$ = doOR($1, $3); };
Expr		:	 Term AND Expr					{ $$ = doAND($1, $3); };					
Expr		:	Term '+' Expr					{ $$ = doAdd($1, $3); } ;
Expr		:	Term '-' Expr					{ $$ = doMinus($1, $3); } ;
Expr		:	Term							{ $$ = $1; } ;
Term		:	Factor '*' Term					{ $$ = doMult($1, $3); } ;
Term		:	Factor '/' Term					{ $$ = doDiv($1, $3); } ;
Term		:	Factor '%' Term					{ $$ = doMod($1, $3); } ;
Term		:	Factor '^' Term					{ $$ = doSimpleExpon($1, $3); } ;
Term		:	Factor '^' '^' Term					{ $$ = doExpon($1, $4); } ;
Term		:	Factor							{ $$ = $1; } ;
Factor		:	IntLit							{ $$ = doIntLit(yytext); };
Factor		:	FloatLit						{ $$ = doFloatLit(yytext); };
Factor		:	BooleanLit						{ $$ = doBooleanLit(yytext); };
Factor		:	Id								{ $$ = doRval($1); };
Factor		:	Id '[' Expr ']'					{ $$ = doArrayRval ($1, $3); };
ArraySize	:	IntLit							{ $$ = getArraySize(yytext); };
Id		: 	Ident								{ $$ = strdup(yytext);}
 
%%

yyerror(char *s)  {
  WriteIndicator(GetCurrentColumn());
  WriteMessage("Illegal Character in YACC");
}
