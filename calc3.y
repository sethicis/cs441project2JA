%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "calc3.h"
#include "symbol_table.h"
#include "PstackInterface.h"
#include "globals.h"
#include "apm.h"

/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *id(char* i,int type);
nodeType *con(int value);
nodeType *fl(float value);
nodeType *chkInit(int declar,char* name,int type);
void freeNode(nodeType *p);
int yylex(void);
void yyerror(char *s);

%}

%union {
    int iValue;                 /* integer value */
    char* sIndex;                /* symbol table index */
    float fValue;
    nodeType *nPtr;             /* node pointer */
};

%token <iValue> INTEGER
%token <fValue> FLOAT
%token <sIndex> VARIABLE
%token BEGIN_PROC END_PROC QUIT
%token WHILE IF PRINT
%token DO REPEAT UNTIL
%nonassoc IFX
%nonassoc ELSE
%right FL INT
%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list mLine

%%

program:
        function                { return 0; }
        |QUIT                    { return 0; }
/*
|program error '\n'	 {yyerrok; yyerror("Missed something"); }
|error ';'			{yyerrok; yyerror("Missed a ';'");}
|error '\n'			{ yyerrok; yyerror("missed something");}
 */
        ;

function:
          function stmt         { ex($2); freeNode($2); }
        | /* NULL */
        ;

stmt:
          ';'                            { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                       { $$ = $1; }
        | FL mLine ';'                   { $$ = opr(',', 1, $2); }
        | INT mLine ';'                  { $$ = opr(',', 1, $2); }
        | PRINT expr ';'                 { $$ = opr(PRINT, 1, $2); }
        | VARIABLE '=' expr ';'          { $$ = opr('=', 2, chkInit(1,$1,typeId), $3); }
        | FL VARIABLE '=' expr ';'       { $$ = opr('=', 2, chkInit(0,$2,typeFloat), $4); }
        | INT VARIABLE '=' expr ';'      { $$ = opr('=', 2, chkInit(0,$2,typeCon), $4);}
        | WHILE '(' expr ')' stmt        { $$ = opr(WHILE, 2, $3, $5); }
        | IF '(' expr ')' stmt %prec IFX { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt { $$ = opr(IF, 3, $3, $5, $7); }
        | DO stmt WHILE '(' expr ')' ';' { $$ = opr(DO, 2, $2, $5); }
        | REPEAT stmt UNTIL '(' expr ')' ';' { $$ = opr(REPEAT, 2, $2, $5); }
        | '{' stmt_list '}'              { $$ = $2; }
		| BEGIN_PROC stmt_list END_PROC {$$ = opr(BEGIN_PROC,1, $2); }
/*
		| WHILE '(' error ')' stmt        { yyerrok; yyerror("Error occured in: "); }
| IF '(' error ')' stmt %prec IFX { yyerrok; yyerror("Error occured in: ");}
		| IF '(' error ')' stmt ELSE stmt { yyerrok; yyerror("Error occured in: ");}
| DO stmt WHILE '(' error ')' ';' { yyerrok; yyerror("Error occurred in: ");}
| REPEAT stmt UNTIL '(' error ')' ';' { yyerrok; yyerror("Error occurred in: ");}
| '{' error '}'              { yyerrok; yyerror("Error occurred in: ");}
|stmt error '\n'			{yyerrok; yyerror("Error seen");}
 */
        ;

stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); }
        ;

expr:
          INTEGER               { $$ = con($1); }
        | FLOAT                { $$ = fl($1); }
        | VARIABLE              { $$ = chkInit(1,$1,typeId); }
        | INT VARIABLE          { $$ = chkInit(0,$2,typeCon); }
        | FL VARIABLE           { $$ = chkInit(0,$2,typeFloat); }
        | '-' expr %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
        | expr '+' expr         { $$ = opr('+', 2, $1, $3); }
        | expr '-' expr         { $$ = opr('-', 2, $1, $3); }
        | expr '*' expr         { $$ = opr('*', 2, $1, $3); }
        | expr '/' expr         { $$ = opr('/', 2, $1, $3); }
        | expr '<' expr         { $$ = opr('<', 2, $1, $3); }
        | expr '>' expr         { $$ = opr('>', 2, $1, $3); }
        | expr GE expr          { $$ = opr(GE, 2, $1, $3); }
        | expr LE expr          { $$ = opr(LE, 2, $1, $3); }
        | expr NE expr          { $$ = opr(NE, 2, $1, $3); }
        | expr EQ expr          { $$ = opr(EQ, 2, $1, $3); }
        | '(' expr ')'          { $$ = $2; }
/*
| '(' error ')'			{ yyerrok; printf("Error seeen\n");}
| expr error '\n'		{ yyerrok; printf("Error seen\n");}
 */

        ;
mLine:
         VARIABLE '=' expr ',' mLine    { $$ = opr(',', 2,$5, (opr('=', 2, chkInit(0,$1,typeId), $3))); }
        |VARIABLE '=' expr              { $$ = opr('=',2, chkInit(0,$1,typeId), $3); }
        |VARIABLE ',' mLine             { $$ = opr(',', 2, $3,chkInit(0,$1,typeId)); }
        |VARIABLE                       { $$ = chkInit(0,$1,0); }
        ;

%%

#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)

/* Helper function used to check if a variable has already been defined
 @param declar: This is a flag used to tell the function if you expect
 the variable to have been declared or if you are declaring the variable.
 @param var: This contains the variable name.
 */
nodeType *chkInit(int declar, char* var,int type){
    if (!declar)
	{
        if ((getSymbolEntry(var)) == 0)
		{
        	return id(var,type);
		/* Check if the variable exists in the current scope */
        }
		else if (getSymbolEntry(var)->blk_level != getCurrentLevel())
		{
			return id(var,type);
		}
		else
		{
			fprintf(stderr, "ERROR @ LINE# %d:: Variable: '%s' already defined\n",lineno,var); exit(0);
			}
	}
    else{
        if ((getSymbolEntry(var)) != 0)
	{
            return id(var,type);
        }
	else
	{
            fprintf(stderr, "ERROR @ LINE# %d:: Variable: '%s' not declared\n",lineno,var); exit(0);
        }
    }
}

nodeType *con(int value) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(conNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeCon;
    p->con.value = value;

    return p;
}

nodeType *fl(float value) {
    nodeType *p;
    size_t nodeSize;
    
    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(floatNodeType);
    if ((p = malloc(nodeSize)) == NULL)
    yyerror("out of memory");
    
    /* copy information */
    p->type = typeFloat;
    p->fl.value = value;
    return p;
}

nodeType *id(char* name,int type) {
    nodeType *p;
    size_t nodeSize;
    symbol_entry *e;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");
		if (getSymbolEntry(name) == 0){
			e = (symbol_entry*)malloc(sizeof(symbol_entry));
			e->type = type;
			e->name = name;
			//printf("About to GetPos()\n");
			e->addr = GetPos(); /* Checks will needed to be added for block levels */
			//printf("About to set getCurrentOffset()\n");
			e->size = 1; /* For now size is staticly set to 1 AK-KB */
			varCount++;
			addSymbol(e,lineno);
			//printf("Finished addSymbols\n");
		}else if (getSymbolEntry(name)->blk_level != getCurrentLevel())
		{
            e = (symbol_entry*)malloc(sizeof(symbol_entry));
            e->type = type;
            e->name = name;
            //printf("About to GetPos()\n");
            e->addr = GetPos(); /* Checks will needed to be added for block levels */
            //printf("About to set getCurrentOffset()\n");
            e->size = 1; /* For now size is staticly set to 1 AK-KB */
            varCount++;
            addSymbol(e,lineno);
            //printf("Finished addSymbols\n");
        }
    /* copy information */
    p->type = typeId;
    p->id.s = name;
    return p;
}

nodeType *opr(int oper, int nops, ...) {
    va_list ap;
    nodeType *p;
    size_t nodeSize;
    int i;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(oprNodeType) +
        (nops - 1) * sizeof(nodeType*);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}

void freeNode(nodeType *p) {
    int i;
    //printf("Called freeNode()\n");
    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }
    free (p);
}

void yyerror(char *s) {
	/*fprintf(stderr, "Error called");*/
    fprintf(stderr, "%s @ line# %d\n", s,lineno);
}

int main(int argc,char** argv) {
    /*if (argc > 1)
    {
        fileName = strdup((char *) argv[1]);
    }
    else
    {
        printf("%s \n", "File Error, No Parameters Passed!");
    }*/
    binary = 1; /* Toggles the format of the p-stack code. binary vs numeric */
    varCount = 0;
    strcpy(fileName,"test.apm");
    ARGs = 3;
    lineno = 1;
    prog_addr = 1;
    begin_prog();           /* Generate code to begin a program */
    pushSymbolTable();
    yyparse();
    end_prog(getTotalSymbolTableSize()); /* Cue pstack for end of program */
    if (!writeOut(fileName,binary)) {
        fprintf(stderr,"ERROR @ Code Gen:: No code compiled. Problems detected.");
    }
    popSymbolTable();
    return 0;
}
