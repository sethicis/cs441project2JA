%{

#include <stdlib.h>
#include "calc3.h"
#include "y.tab.h"
#include <string.h> /* Added by Kyle Blagg */
#include "globals.h"
#include "symbol_table.h"
void yyerror(char *);
void setStr(char*);

%}
/* Modified line: Added '+' by Kyle Blagg */
/* Modified: Added case to detect float. AK-KB */
/* Increment the lineno count */
/* Modified by Kyle Blagg, removed '\n' and added a special rule just for it */
%x C_COMMENT
%x CC_COMMENT
%%

"//"						{printf("Start C++ comment\n");BEGIN(CC_COMMENT);}
<CC_COMMENT>\n			{printf("End C++\n");lineno = lineno + 1;BEGIN(INITIAL);}
"/*"						{BEGIN(C_COMMENT);}
<C_COMMENT>\n				{lineno = lineno + 1;}
<C_COMMENT>"*/"				{BEGIN(INITIAL);}
<C_COMMENT,CC_COMMENT>.	{ /* Do nothing */ }
"float"        return FL;
"int"           return INT;
">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"while"         return WHILE;
"if"            return IF;
"else"          return ELSE;
"print"         return PRINT;
"do"        return DO;
"repeat"    return REPEAT;
"until"     return UNTIL;
"begin"		{pushSymbolTable(); return BEGIN_PROC;}
"end"       return END_PROC;
"quit"      return QUIT;
[-()<>=+*/;{},] {
    return *yytext;
}

[a-zA-Z]+   {
                yylval.sIndex = strdup(yytext);
                //setStr(yytext);
                return VARIABLE;
            }
[0-9]*\.[0-9]+ {
                    yylval.fValue = atof(yytext);
                    return FLOAT;
                }

0|[1-9][0-9]* { 
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }
\n          {lineno = lineno + 1;}
[ \t]+        ;       /* ignore whitespace */
<<EOF>>      { return QUIT;}
.               yyerror("Unknown character");
%%
int yywrap(void) {
    return 1;
}
