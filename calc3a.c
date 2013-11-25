#include <stdio.h>
#include "calc3.h"
#include "y.tab.h"
#include "symbol_table.h"

double ex(nodeType *p) {
    printf("Beginning ex function\n");
    if (!p) return 0;
    switch(p->type) {
        case typeCon:       printf("Returning con val\n");return p->con.value;
        case typeId:    printf("checking typeId of: %d against: %d\n",getSymbolEntry(p->id.s)->type,typeFloat);
            if (getSymbolEntry(p->id.s)->type == typeFloat)
                    {
                        return getSymbolEntry(p->id.s)->fVal;
                    }else{
                        return getSymbolEntry(p->id.s)->iVal;
                    }
    case typeFloat:        return p->fl.value;
    case typeOpr:
        switch(p->opr.oper) {
        case WHILE:     while(ex(p->opr.op[0])) ex(p->opr.op[1]); return 0;
        case IF:        if (ex(p->opr.op[0]))
                            ex(p->opr.op[1]);
                        else if (p->opr.nops > 2)
                            ex(p->opr.op[2]);
                        return 0;
            case PRINT:     printf("Attempting to print\n");printf("%f\n", ex(p->opr.op[0])); return 0;
        case ';':       ex(p->opr.op[0]); return ex(p->opr.op[1]);
        case '=':       if (p->opr.op[0]->type == typeFloat){
                            getSymbolEntry(p->opr.op[0]->id.s)->fVal = ex(p->opr.op[1]);
                        }else{getSymbolEntry(p->opr.op[0]->id.s)->iVal = ex(p->opr.op[1]);}
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
