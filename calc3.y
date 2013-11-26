%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "calc3.h"
#include "symbol_table.h"

/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *id(char* i);
nodeType *con(int value);
nodeType *fl(double value);
void freeNode(nodeType *p);
int ex(nodeType *p);
int yylex(void);

void yyerror(char *s);
//int sym[26];                    /* symbol table */
%}

%union {
    int iValue;                 /* integer value */
    char* sIndex;                /* symbol table index */
    double fValue;
    nodeType *nPtr;             /* node pointer */
};

%token <iValue> INTEGER
%token <fValue> DOUBLE
%token <sIndex> VARIABLE
%token WHILE IF PRINT 
%token DO REPEAT UNTIL
%nonassoc IFX
%nonassoc ELSE
%right DB INT
%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list mLine

%%

program:
        function                { exit(0); }
        ;

function:
          function stmt         { ex($2); freeNode($2); }
        | /* NULL */
        ;

stmt:
          ';'                            { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                       { $$ = $1; }
        | DB mLine ';'                   { $$ = opr(',', 1, $2); }
        | INT mLine ';'                  { $$ = opr(',', 1, $2); }
        | PRINT expr ';'                 { $$ = opr(PRINT, 1, $2); }
        | VARIABLE '=' expr ';'          { $$ = opr('=', 2, id($1), $3); }
        | DB VARIABLE '=' expr ';'	 { $$ = opr('=', 2, id($2), $4); }
| INT VARIABLE '=' expr ';'	 { $$ = opr('=', 2, id($2), $4); printf("Inting\n");}
        | WHILE '(' expr ')' stmt        { $$ = opr(WHILE, 2, $3, $5); }
        | IF '(' expr ')' stmt %prec IFX { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt { $$ = opr(IF, 3, $3, $5, $7); }
	| DO stmt WHILE '(' expr ')' ';' { $$ = opr(DO, 2, $2, $5); }
	| REPEAT stmt UNTIL '(' expr ')' ';' { $$ = opr(REPEAT, 2, $2, $5); }
        | '{' stmt_list '}'              { $$ = $2; }
        ;

stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); }
        ;

expr:
          INTEGER               { $$ = con($1); }
        | DOUBLE                { $$ = fl($1); }
        | VARIABLE              { $$ = id($1); }
        | INT VARIABLE		{ $$ = id($2); }
        | DB VARIABLE		{ $$ = id($2); }
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
        ;
mLine:
VARIABLE '=' expr ',' mLine    { $$ = opr(',', 2,$5, (opr('=', 2, id($1), $3))); }
        |VARIABLE '=' expr              { $$ = opr('=',2, id($1), $3); }
        |VARIABLE ',' mLine             { $$ = opr(',', 2, $3,id($1)); }
        |VARIABLE                       { $$ = id($1); }
        ;

%%

#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)

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

nodeType *fl(double value) {
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

nodeType *id(char* name) {
    //printf("Entrying ID assigning: %s\n",name);
    nodeType *p;
    size_t nodeSize;
    symbol_entry *e;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");
        if (getSymbolEntry(name) == 0){
            e = (symbol_entry*)malloc(sizeof(symbol_entry));
            e->type = typeId;
            e->name = name;
            e->addr = ADDR++; /* Checks will needed to be added for block levels */
            e->blk_level = getCurrentLevel(); /* Get the curr blk lvl AK-KB */
            e->size = 1; /* For now size is staticly set to 1 AK-KB */
            if (ARGs < 0 || ARGs >= 3) {
                e->offset = ARGs++;
            }else{
                ARGs = 3;
                e->offset = ARGs++;
            }
            //printf("Adding a new element to the symbol_table\n");
            addSymbol(e,lineno);
        }

    /* copy information */
    p->type = typeId;
    p->id.s = name;
    //printf("Finished ID gen\n");
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

    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }
    free (p);
}

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}

int main(void) {
    lineno = 0;
    ARGs = 3;
    pushSymbolTable();
    yyparse();
    popSymbolTable();
    return 0;
}
