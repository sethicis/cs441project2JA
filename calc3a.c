#include <stdio.h>
#include "calc3.h"
#include "y.tab.h"
#include "symbol_table.h"

void genOp(int);

int ex(nodeType *p) {
    if (!p) return 0;
    switch(p->type) {
    case typeCon:       return p->con.value;
    case typeId:        return getSymbolEntry(p->id.s)->val.i;
    case typeFloat:     return p->fl.value;
    case typeOpr:
        switch(p->opr.oper) {
	case DO:	do{ex(p->opr.op[0]);}while(ex(p->opr.op[1])); return 0; 
	case REPEAT:    do{ex(p->opr.op[0]);}while(!(ex(p->opr.op[1]))); return 0;
	case WHILE:     while(ex(p->opr.op[0])) ex(p->opr.op[1]); return 0;
       	case IF:        if (ex(p->opr.op[0]))
                            ex(p->opr.op[1]);
                        else if (p->opr.nops > 2)
                            ex(p->opr.op[2]);
                        return 0;
        case ',':       if (p->opr.nops > 1) {
                       		ex(p->opr.op[1]); return ex(p->opr.op[0]);}
			else{
                    		return ex(p->opr.op[0]);
            }
        /*case PRINT:     printf("%d\n", ex(p->opr.op[0])); return 0;*/
        case PRINT:     genOp(ex(p->opr.op[0])); return 0;
        case ';':       ex(p->opr.op[0]); return ex(p->opr.op[1]);
        case '=':       return getSymbolEntry(p->opr.op[0]->id.s)->val.i = ex(p->opr.op[1]);
        case UMINUS:    return -ex(p->opr.op[0]);
        case '+':       return ex(p->opr.op[0]) + ex(p->opr.op[1]);
        case '-':       return ex(p->opr.op[0]) - ex(p->opr.op[1]);
        case '*':       return ex(p->opr.op[0]) * ex(p->opr.op[1]);
        case '/':       return ex(p->opr.op[0]) / ex(p->opr.op[1]);
        case '<':       return ex(p->opr.op[0]) < ex(p->opr.op[1]);
        case '>':       return ex(p->opr.op[0]) > ex(p->opr.op[1]);
        case GE:        return ex(p->opr.op[0]) >= ex(p->opr.op[1]);
        case LE:        return ex(p->opr.op[0]) <= ex(p->opr.op[1]);
        case NE:        return ex(p->opr.op[0]) != ex(p->opr.op[1]);
        case EQ:        return ex(p->opr.op[0]) == ex(p->opr.op[1]);
        }
    }
    return 0;
}

void genOp(int constant){
    
    printf("%04d I_Constant value:%d\n",prog_addr,constant);
    prog_addr = prog_addr + 2; /* Increment intprog_addr */
    printf("%04d I_Write words:%d\n",prog_addr,1);
    prog_addr = prog_addr + 2;
    
}
