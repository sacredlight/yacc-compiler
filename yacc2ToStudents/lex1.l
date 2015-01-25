%{
#include "yaccExample.h"
#include "y.tab.h"
#include <stdbool.h>

#define YY_INPUT(buf,result,max_size) \
	{ int c = GetSourceChar(); \
          result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
        }

%}


letter [A-Za-z]
digit [0-9]

%%
\#					{return '#';}
int					{return Int;}
float					{return Float;}
PRINT					{return Write;}
PRINTLN					{return WriteLn;}
PRINTSPACE				{return WriteSpace;}
READ					{return Read;}
main					{return MAIN;}
void					{return VOID;}			
boolean					{return Boolean;}
if					{return IF;}
else					{return ELSE;}
while					{return WHILE;}
true					{return BooleanLit;}
false					{return BooleanLit;}
\=\=					{return EQ;}
{letter}({letter}|{digit})*		{return Ident;}
-?{digit}{digit}*			{return IntLit;}
-?{digit}{digit}*\.{digit}{digit}*	{return FloatLit;}
\,					{return ',';}
\<					{return LT;}
\>					{return GT;}
\<\=					{return LTE;}
\>\=					{return GTE;}
\!					{return NOT;}
\|\|					{return OR;}
\!\=					{return NEQ;}
\&\&					{return AND;}
\=					{return '=';}
\+					{return '+';}
\-					{return '-';}	
\*					{return '*';}
\/					{return '/';}
\%					{return '%';}
\^					{return '^';}
\;					{return ';';}
\(					{return '(';}
\)					{return ')';}
\{					{return '{';}
\}					{return '}';}
\[					{return '[';}
\]					{return ']';}
\&					{return '&';}

[ ]				{}
\t				{}
\r				{}
\n				{}
\00				{}

.				{	
					WriteIndicator(GetCurrentColumn());
				   	WriteMessage("Illegal Character in lex");
				}

%%

	
yywrap () {
}		
	