/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     Ident = 258,
     IntLit = 259,
     FloatLit = 260,
     Int = 261,
     Float = 262,
     Write = 263,
     WriteLn = 264,
     WriteSpace = 265,
     Read = 266,
     EQ = 267,
     IF = 268,
     ELSE = 269,
     WHILE = 270,
     NEQ = 271,
     Boolean = 272,
     BooleanLit = 273,
     NOT = 274,
     OR = 275,
     AND = 276,
     LT = 277,
     GT = 278,
     LTE = 279,
     GTE = 280,
     MAIN = 281,
     VOID = 282
   };
#endif
/* Tokens.  */
#define Ident 258
#define IntLit 259
#define FloatLit 260
#define Int 261
#define Float 262
#define Write 263
#define WriteLn 264
#define WriteSpace 265
#define Read 266
#define EQ 267
#define IF 268
#define ELSE 269
#define WHILE 270
#define NEQ 271
#define Boolean 272
#define BooleanLit 273
#define NOT 274
#define OR 275
#define AND 276
#define LT 277
#define GT 278
#define LTE 279
#define GTE 280
#define MAIN 281
#define VOID 282




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 27 "ExprEval.y"
{
  long val;
  char * string;
  struct ExprRes * ExprRes;
  struct ExprResList *ExprResList;
  struct InstrSeq * InstrSeq;
  struct CondRes * CondRes;
}
/* Line 1529 of yacc.c.  */
#line 112 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

