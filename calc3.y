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
nodeType *id(char* i);
nodeType *con(int value);
nodeType *fl(float value);
//nodeType *chkInit(int declar,char* name,int type);
nodeType *initId(char* name,int type);
void freeNode(nodeType *p);
int yylex(void);
void yyerror(char *s);
nodeType *nest(void);

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
%token FUNC
%token WHILE IF PRINT
%token FOR
%token DO REPEAT UNTIL
%nonassoc IFX
%nonassoc ELSE
%right FL INT FLMULTI INTMULTI
%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%token STEP TO
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list mLine subExp

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
FUNC VARIABLE '(' ')' '{' stmt_list '}'	{ printf("Saw a function!\n");/* do stuff */ }
		| function stmt         { ex($2); freeNode($2); }
        | /* NULL */
        ;

stmt:
          ';'                            { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                       { $$ = $1; }
        | PRINT expr ';'                 { $$ = opr(PRINT, 1, $2); }
        | VARIABLE '=' expr ';'          { $$ = opr('=', 2, id($1), $3); }
        | FL VARIABLE '=' expr ';'       { $$ = opr('=',2,initId($2,TYPE_FLOAT), $4); }
        | INT VARIABLE '=' expr ';'      { $$ = opr('=', 2,initId($2,TYPE_INT), $4);}
| INT mLine ';'                   { $$ = opr(',', 1, $2); }
		| FL mLine ';'                  { $$ = opr(',', 1, $2); }
        | WHILE '(' expr ')' stmt        { $$ = opr(WHILE, 2, $3, $5); }
        | IF '(' expr ')' stmt %prec IFX { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt { $$ = opr(IF, 3, $3, $5, $7); }
        | DO stmt WHILE '(' expr ')' ';' { $$ = opr(DO, 2, $2, $5); }
        | REPEAT stmt UNTIL '(' expr ')' ';' { $$ = opr(REPEAT, 2, $2, $5); }
        | '{' stmt_list '}'              { $$ = $2; }
		| BEGIN_PROC stmt_list END_PROC {$$ = opr(BEGIN_PROC,1, $2); }
	| FOR '(' VARIABLE '=' subExp STEP INTEGER TO subExp ')' stmt	{ $$ = opr(FOR, 4, opr('=',2,id($3), $5) , opr('=',2,id($3),opr('+',2,id($3),con($7))) , opr('>',2,id($3),$9) , $11); }
/*
		| WHILE '(' error ')' stmt        { yyerrok; yyerror("Error occured in: "); }
| IF '(' error ')' stmt %prec IFX { yyerrok; yyerror("Error occured in: ");}
		| IF '(' error ')' stmt ELSE stmt { yyerrok; yyerror("Error occured in: ");}
| DO stmt WHILE '(' error ')' ';' { yyerrok; yyerror("Error occurred in: Do While Condition");}
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
        | VARIABLE              { $$ = id($1); }
        | INT VARIABLE          { $$ = initId($2,TYPE_INT); }
        | FL VARIABLE           { $$ = initId($2,TYPE_FLOAT); }
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
         VARIABLE '=' expr ',' mLine   { $$ = opr(',', 2,$5, (opr('=', 2, initId($1,multiAssignType), $3))); }
        |VARIABLE '=' expr       { $$ = opr('=',2, initId($1,multiAssignType), $3); }
        |VARIABLE ',' mLine      { $$ = opr(',', 2, $3,initId($1,multiAssignType)); }
        |VARIABLE                { $$ = initId($1,multiAssignType); }
        ;

subExp:
		  INTEGER               { $$ = con($1); }
		| FLOAT                { $$ = fl($1); }
		| VARIABLE              { $$ = id($1); }
		| '-' subExp %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
		| subExp '+' subExp         { $$ = opr('+', 2, $1, $3); }
		| subExp '-' subExp         { $$ = opr('-', 2, $1, $3); }
		| subExp '*' subExp         { $$ = opr('*', 2, $1, $3); }
		| subExp '/' subExp         { $$ = opr('/', 2, $1, $3); }
		| subExp '<' subExp         { $$ = opr('<', 2, $1, $3); }
		| subExp '>' subExp         { $$ = opr('>', 2, $1, $3); }
		| subExp GE subExp          { $$ = opr(GE, 2, $1, $3); }
		| subExp LE subExp          { $$ = opr(LE, 2, $1, $3); }
		| subExp NE subExp          { $$ = opr(NE, 2, $1, $3); }
		| subExp EQ subExp          { $$ = opr(EQ, 2, $1, $3); }
		| '(' subExp ')'          { $$ = $2; }
		;

%%

#define SIZEOF_NODETYPE ((char *)&p->con - (char *)p)

// Check if Variable Exists Within Scope and Lower Scopes
void scopeCheck(const char *var_name){
//	NOTE::: Commented out because code MAY be depricated...
	int scope_level = getCurrentLevel();
	int table_size;
	int i, j;

	fprintf(stderr, "Variable Reference Called(I think...)! Checking Scope Level for Variable!\n\n");

	// If variable name is no where in the entire stack
	if(!(getSymbolEntry(var_name)))
	{
		fprintf(stderr, "Variable hasn't been declared or doesn't exist!\n\n");
	}
	else // Variable name exists in entire stack somewhere
	{
		// Do a Top-Bottom Scope Level Depth Check
		for(j = scope_level; j >= 0; j--)
		{
			// Obtain Current Activation Record Size
			table_size = getSymbolTableSize(j);

			// Check all variables within record
			for(i = table_size; i > 0; i--)
			{
//				std::cout << "Current Symbol Table Size: ";
//				std::cout << i << std::endl;
//				std::cout << "Current Scope Level: ";
//				std::cout << j << std::endl;
				//std::cout << getSymbolEntryByRelAddr(j, i + 2)->name << " -> Variable Name" << std::endl;				

				// If Variable Names Match

				// Note: getSymbolEntryByRelAddr uses relative
				// scope level, which 0 is the current scope
				// level and 1, 2, 3 and etc are relative
				// scope levels away from current one
				if(!(strcmp(getSymbolEntryByRelAddr(j, i + 2)->name, var_name)))
				{ 
					fprintf(stderr, "Variable Offset Value: %d\n\nScope Offset Level: %d\n\nVariable %s exists within %d Scope Level(s) Away from the Current Scope!\n\n", i + 2, j, var_name, j);
		//			std::cout << "Variable Offset Value: ";
		//			std::cout << i + 2 << std::endl;
		///			std::cout << "Scope Offset Level: ";
		//			std::cout << j << std::endl;
//
//					std::cout << "Variable " << var_name;
//					std::cout << " exists within " << j;
//					std::cout << " Scope Level(s) Away from";
//					std::cout << " the Current Scope!";
//					std::cout << std::endl << std::endl;
				}
			}
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
nodeType *initId(char* name,int type){
	nodeType *p;
    size_t nodeSize;
	
    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = malloc(nodeSize)) == NULL)
	yyerror("out of memory");
	p->type = initIdtype;
    p->id.s = name;
	//printf("IN id assigning type: %d",type);
	p->symType = type;
	p->lineNum = lineno;
    return p;
	
}
nodeType *id(char* name) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");
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
	if (!p) return;
	//printf("Freeing var: %x\n",p);
    free (p);
}

void yyerror(char *s) {
	/*fprintf(stderr, "Error called");*/
    fprintf(stderr, "%s @ line# %d\n", s,lineno);
}

int main(int argc,char** argv) {
    binary = 1; /* Toggles the format of the p-stack code. binary vs numeric */
    strcpy(fileName,"calc3p_out.apm");
    ARGs = 3;
    lineno = 1;
    prog_addr = 1;
	pushSymbolTable();
    begin_prog();           /* Generate code to begin a program */
    yyparse();
    end_prog(getTotalSymbolTableSize()); /* Cue pstack for end of program */
    if (!writeOut(fileName,binary)) {
        fprintf(stderr,"ERROR @ Code Gen:: No code compiled. Problems detected.");
    }
    popSymbolTable();
    return 0;
}
