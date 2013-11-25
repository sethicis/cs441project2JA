%{

#include <stdlib.h>
#include "calc3.h"
#include "y.tab.h"
#include <string.h> /* Added by Kyle Blagg */
void yyerror(char *);
extern int lineno;      /* Added by KB */
void setStr(char*);
%}
/* Modified line: Added '+' by Kyle Blagg */
/* Modified: Added case to detect float. AK-KB */
/* Increment the lineno count */
/* Modified by Kyle Blagg, removed '\n' and added a special rule just for it */

%%
"double"        {return DB;}
"int"           {return INT;}
">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"while"         return WHILE;
"if"            return IF;
"else"          return ELSE;
"print"         return PRINT;
[-()<>=+*/;{}.] {
    return *yytext;
}

[a-zA-Z]+   {
    printf("The value of yytext is: %s and the size is: %d\n",yytext,strlen(yytext)+1);
                yylval.str = *yytext - 'a';
                setStr(yytext);
                return VARIABLE;
            }

0           {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

[1-9][0-9]* {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

[0-9]*'.'[0-9]+ {
                    yylval.fValue = atof(yytext);
                    return DOUBLE;
                }
'\n'            ++lineno;
[ \t]+        ;       /* ignore whitespace */

.               yyerror("Unknown character");
%%
int yywrap(void) {
    return 1;
}

void setStr(char* yytext){
    printf("Debug: str is: %s len is: %d\n",yytext,strlen(yytext));
    vName = (char*)malloc(strlen(yytext)+1);
    strcpy(vName,yytext);
    printf("Finished copy\n");
    //vName[strlen(yytext)+1] = '\0';
}
