%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "calc3.h"
#include "symbol_table.h"
    
/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *id(int var);
nodeType *con(int value);
nodeType *fl(double value);
/*nodeType *chkInit(int type, char* var);*/
void freeNode(nodeType *p);
double ex(nodeType *p);
int yylex(void);

void yyerror(char *s);
/* int sym[26];  commented out by Kyle Blagg */    /* symbol table */
%}

%union {
    int iValue;                 /* integer value */
    /* Modified by KB changed char sIndex into char* sType */
    char str;                /* symbol type */
    int vType;                /* variable type */
    double fValue;               /* double value */
    nodeType *nPtr;             /* node pointer */
};

/* Modified by KB, added DOUBLE and changed VARIABLE to VTYPE */
%token <fValue> DOUBLE
%token <iValue> INTEGER
%token <vType> VTYPE
%token <str> VARIABLE
%token WHILE IF PRINT
%nonassoc IFX
%nonassoc ELSE
%left DB INT
%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list

%%

program:
        function                { exit(0); }
        ;

function:
function stmt         { ex($2); freeNode($2); pushSymbolTable(); }
        | /* NULL */
        ;

stmt:
          ';'                            { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                       { $$ = $1; }
        | PRINT expr ';'                 { $$ = opr(PRINT, 1, $2); }
        | VARIABLE '=' expr ';'          { $$ = opr('=', 2, id($1), $3); }
        | WHILE '(' expr ')' stmt        { $$ = opr(WHILE, 2, $3, $5); }
        | IF '(' expr ')' stmt %prec IFX { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt { $$ = opr(IF, 3, $3, $5, $7); }
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

%%

#define SIZEOF_NODECONTYPE ((char *)&p->con - (char *)p)
#define SIZEOF_NODEFLTYPE ((char *)&p->fl - (char *)p)
/*
nodeType *chkInit(int type, char* var){
    if (getSymbolEntry(var) != 0){
        if (type == typeId) {
            type = getSymbolEntry(var)->type;
            
        }else{
            yyerror("Variable defined more than once");
        }
    }else{
        if (type == typeId) {
            yyerror("Variable not defined");
        }
        else if (type == typeCon){
            return id(var);
        }
        else if (type == typeFloat){
            return id(var);
        }else
            yyerror("Unidentified datatype");
    }
    return id(var);
}*/

nodeType *con(int value) {

    nodeType *p;
    size_t nodeSize;

    /* copy information */
    /* allocate node */
    nodeSize = SIZEOF_NODECONTYPE + sizeof(nodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");
    p->type = typeCon;
    p->con.value = (int)value;
    return p;
}


nodeType *fl(double value) {
    
    nodeType *p;
    size_t nodeSize;
    
/* copy information */
    /* allocate node */
    nodeSize = SIZEOF_NODEFLTYPE + sizeof(nodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");
    p->type = typeFloat;
    p->fl.value = value;

    return p;
}


nodeType *id(int var) {
    printf("Adding var: %d\n",var);
    
    nodeType *p;
    struct symbol_entry *entry;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODECONTYPE + sizeof(nodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");
    if (getSymbolEntry(vName) == 0) {
        nodeSize = sizeof(struct symbol_entry);
        if ((entry = malloc(nodeSize)) == NULL)
            yyerror("out of memory");
        /* Place elements into symbol table by: AK-KB */
        entry->type = typeId;
        entry->name = vName;
        entry->addr = ADDR++; /* Checks will needed to be added for block levels */
        entry->blk_level = getCurrentLevel(); /* Get the curr blk lvl AK-KB */
        entry->size = 1; /* For now size is staticly set to 1 AK-KB */
        if (ARGs < 0 || ARGs >= 3) {
            entry->offset = ARGs++;
        }else{
            ARGs = 3;
            entry->offset = ARGs++;
        }
        printf("Adding a new element to the symbol_table\n");
        addSymbol(entry,lineno);
    }
    
    /* copy information */
    p->type = typeId;
    p->id.s = vName;
    

    return p;
}

nodeType *opr(int oper, int nops, ...) {
    va_list ap;
    nodeType *p;
    size_t nodeSize;
    int i;

    /* allocate node */
    nodeSize = sizeof(oprNodeType) + (nops - 1) * sizeof(nodeType*);
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
    pushSymbolTable();
    ARGs = 3;
    ADDR = 0;
    lineno = 0;
    yyparse();
    return 0;
}
