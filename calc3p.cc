/* This file contains the code used to generate the
 p-stack opcodes. --JKB
 */

#include <stdio.h> //Used for I/O operations
#include "calc3.h" //For nodeType definitions
#include "y.tab.h" //For global defines
#include "symbol_table.h" //For symbol table operations
#include "pstcode.h"		//For p-stack opcode instructions
#include "PstackInterface.h"//For legacy C friendly interaction with pstack class
#include <iostream>
#include <cstring>

int retval;
WORD retVal;
bool operandType(nodeType* a,nodeType* b);
int createSymbolEntry(nodeType*);
int* tmp; /* Temporary variable pointing to an instruction to change */
int currP;
int forCurrP;
int forCondition;

int ex(nodeType *p) {
    if (!p) return 0;
    switch(p->type) {
		case typeCon:       //retVal.Integer = p->con.value; return 1;
			addI(I_CONSTANT);
			addI(p->con.value);
			retval = 1;
			return 0;
		case typeFloat:     //retVal.Real = p->fl.value; return 2;
			addI(R_CONSTANT);
			addF(p->fl.value);
			retval = 2;
			return 0;
		/* Case for when a variable already exists */
		case typeId:
			/* Make sure we didn't miss something */
			if (!getSymbolEntry(p->id.s)) {
				fprintf(stderr,"Variable: %s does not exist in symbol table\n",p->id.s);
				exit(1);
			}
			if (getSymbolEntry(p->id.s)->type == TYPE_INT){
				addI(I_VARIABLE);
				addI(getCurrentLevel() - getSymbolEntry(p->id.s)->blk_level);
				addI(getSymbolEntry(p->id.s)->offset);
				retval = 1;
			}else{
				addI(R_VARIABLE);
				addI(getCurrentLevel() - getSymbolEntry(p->id.s)->blk_level);
				addI(getSymbolEntry(p->id.s)->offset);
				retval = 2;}
			return 0;
		/* Case for initalizing a new variable */
		case initIdtype:
			/* Check for errors */
			if(createSymbolEntry(p))
				{fprintf(stderr,"Variable already defined\n");
					exit(1);}
			if (getSymbolEntry(p->id.s)->type == TYPE_INT){
				addI(I_VARIABLE);
				addI(getCurrentLevel()-getSymbolEntry(p->id.s)->blk_level);
				addI(getSymbolEntry(p->id.s)->offset);
				retval = 1;
			}else{
				addI(R_VARIABLE);
				addI(getCurrentLevel()-getSymbolEntry(p->id.s)->blk_level);
				addI(getSymbolEntry(p->id.s)->offset);
				retval = 2;}
			return 0;
    case typeOpr:
        switch(p->opr.oper) {
			case BEGIN_PROC:
				pushSymbolTable();
				begin_proc();
				ex(p->opr.op[0]); //Does not support return values right now
				end_proc();
				popSymbolTable(); //printf("Removing Symbol Table\n");
				return 0;
			case FOR:
				{
					ex(p->opr.op[0]);			/* Variable x = something */
					forCondition = GetPos();
					ex(p->opr.op[2]);			/* Evaluate condition */
					addI(I_JMP_IF_TRUE);		/* If condition is true */
					addI(0);					/* Placeholer */
					forCurrP = GetPos() - 1; /* Get placeholder addr */
					ex(p->opr.op[3]); /* Statement to perform */
					ex(p->opr.op[1]); /* increment value */
					addI(I_JMP);			/* Loop it over again */
					addI(forCondition);	/* Jump back and doe the statement again */
					*I_refToPos(forCurrP) = GetPos();	/* Set the value for the placeholder */
					return 0;
				}
		case DO:
			currP = GetPos();
			ex(p->opr.op[0]); /* Statement */
			ex(p->opr.op[1]); /* Condition */
			addI(I_JMP_IF_TRUE);
			addI(currP);		/* Address to jump to */
			return 0;
		case REPEAT:
			currP = GetPos();
			ex(p->opr.op[0]);
			ex(p->opr.op[1]);
			addI(I_JMP_IF_FALSE);
			addI(currP);
			return 0;
		case WHILE: /* P-stack code for while loop, implemented in a do while form */
			addI(I_JMP);
			addI(0);		/* Put placeholder on stack */
			currP = GetPos();/* Save the current position right before the statement */
			ex(p->opr.op[1]); /* Statement code */
			*I_refToPos(currP-1) = GetPos(); /* Set the placeholder to the condition code */
			
			ex(p->opr.op[0]); /* Condition code */
			addI(I_JMP_IF_TRUE); /* If true run the statement code until false */
			addI(currP);		/* Add address of statement code */
			return 0;
		case IF:
			ex(p->opr.op[0]);
			addI(I_JMP_IF_FALSE);
			addI(0);			/* 1st Placeholder for address */
			currP = GetPos(); /* Save placeholder position */
			ex(p->opr.op[1]); /* True case code */
			addI(I_JMP);
			addI(0);		/* 2nd Placeholder for end of IF statement */
			*I_refToPos(currP-1) = GetPos(); /* Set 1st placeholder to this addr */
			tmp = I_refToPos(GetPos()-1); /* Save second placeholder for later */
			/* "ELSE" case */
			if (p->opr.nops > 2)
				ex(p->opr.op[2]);
			/* END of IF sequence */
			*tmp = GetPos();	/* Set 2nd placeholder to end IF sequence */
			return 0;
		case ',':
			if (p->opr.nops > 1) {
				ex(p->opr.op[1]);
				ex(p->opr.op[0]);
				return 0;
			}
			else{
				ex(p->opr.op[0]);
				return 0;
			}
		case PRINT:
			ex(p->opr.op[0]);
				
			if (retval == 1) {
				/* If the operand to be printed is a variable. Get its value */
				if ((p->opr.op[0]->type == typeId) || (p->opr.op[0]->type == initIdtype)) {
					addI(I_VALUE);
				}
				addI(I_WRITE);
				addI(1);}
			else{
				/* If the value being printed is a variable get its value */
				if ((p->opr.op[0]->type == typeId) || (p->opr.op[0]->type == initIdtype)) {
					addI(R_VALUE);
				}
				addI(R_WRITE);
				addI(1);}
			return 0;
		case ';':
			ex(p->opr.op[0]);
			ex(p->opr.op[1]);
			return 0; /* Return from case */
		case '=':
			/* Check the variables type and perform the assignment as appropriate */
				ex(p->opr.op[0]);
				/* Type checking used to insure that reals and integers are mixed
				 up.  In the case of assignments the datatype of the variable
				 takes precedence and the value being assigned to the variable
				 is cast to its appropriate datatype */
			if (retval == TYPE_INT)
			{
				ex(p->opr.op[1]); /* Get the value to be assigned */
				if (retval == TYPE_INT) { /* If the value being assigned is int */
					if ((p->opr.op[1]->type == typeId) || (p->opr.op[1]->type == initIdtype))
							addI(I_VALUE);
					retVal.Integer = I_valAtPos(GetPos()-1);
					//printf("Assigning: %d to %s\n",retVal.Integer,p->opr.op[0]->id.s);
					getSymbolEntry(p->opr.op[0]->id.s)->val.i = retVal.Integer;
					addI(I_ASSIGN); /* If the value to be assigned is an int */
					addI(1);
				}
				else{ /* Else the value to be assigned is a Real */
					if ((p->opr.op[1]->type == typeId) || (p->opr.op[1]->type == initIdtype))
						addI(R_VALUE);
					retVal.Real = F_valAtPos(GetPos()-1); /* If the value is a Real */
					getSymbolEntry(p->opr.op[0]->id.s)->val.i = retVal.Real;
					addI(R_TO_I); /* Convert real to int before assignment */
					addI(I_ASSIGN);
					addI(1);}
					return 0;
			}else{ /* Case variable is a Real */
				ex(p->opr.op[1]);
				if (retval == TYPE_INT) {
					if ((p->opr.op[1]->type == typeId) || (p->opr.op[1]->type == initIdtype))
						addI(I_VALUE);
					retVal.Integer = I_valAtPos(GetPos()-1);
					getSymbolEntry(p->opr.op[0]->id.s)->val.f = retVal.Integer;
					addI(I_TO_R);
					addI(R_ASSIGN);
					addI(1);}
				else{ /* Case Real */
					if ((p->opr.op[1]->type == typeId) || (p->opr.op[1]->type == initIdtype))
						addI(R_VALUE);
					retVal.Real = F_valAtPos(GetPos()-1);
					getSymbolEntry(p->opr.op[0]->id.s)->val.f = retVal.Real;
					addI(R_ASSIGN);
					addI(1);}
				return 0;
			}
		/* All operations in this switch case are filtered through a type checking
		 algorithm that determines what type to cast everything.  Floats
		 take the highest priority, thus any numeric operations involving Reals
		 will be cast into operations involving only reals.
		 The only exception to this case is when the user is making assignments
		 to a variable.  See assignment section for more details */
		case UMINUS:
			ex(p->opr.op[0]);
			/* retval is used as a flag for the return type.
			 This is a kind of rudementary type checking tool */
			if (retval == TYPE_INT) {
				if ((p->opr.op[0]->type == typeId) || (p->opr.op[0]->type == initIdtype))
					addI(I_VALUE);
				addI(I_MINUS);}
			else{
				if ((p->opr.op[0]->type == typeId) || (p->opr.op[0]->type == initIdtype))
					addI(R_VALUE);
				addI(R_MINUS);}
			return 0;
		case '+':
			/* Depending on the type of data on the stack use the appropriate instruction */
			if (operandType(p->opr.op[0],p->opr.op[1])) {
				addI(R_ADD);}
			else{
				addI(I_ADD);}
			return 0;
		case '-':
			if (operandType(p->opr.op[0],p->opr.op[1])) {
				addI(R_SUBTRACT);}
			else{
				addI(I_SUBTRACT);}
			return 0;
		case '*':
			if (operandType(p->opr.op[0],p->opr.op[1])) {
				addI(R_MULTIPLY);}
			else{
				addI(I_MULTIPLY);}
			return 0;
		case '/':
			if (operandType(p->opr.op[0],p->opr.op[1])) {
				addI(R_DIVIDE);}
			else{
				addI(I_DIVIDE);}
			//fl = false;
			return 0;
		case '<':
			if (operandType(p->opr.op[0],p->opr.op[1])) {
				addI(R_LESS);}
			else{
				addI(I_LESS);}
			return 0;
		case '>':
			//return parseRet(p->opr.op[0],p->opr.op[1],'>');
			if (operandType(p->opr.op[0],p->opr.op[1])) {
				addI(R_GREATER);}
			else{
				addI(I_GREATER);
			}
			return 0;
		case GE:
			//return parseRet(p->opr.op[0],p->opr.op[1],GE);
			if (operandType(p->opr.op[0],p->opr.op[1])) {
				addI(R_GREATER);}
			else{
				addI(I_GREATER);}
			/* Now stack has x > y result */
			if (operandType(p->opr.op[0],p->opr.op[1])) {
				addI(R_EQUAL);}
			else{
				addI(I_EQUAL);}
			/* Now stack has x == y (result), x > y (result) */
			addI(I_OR); /* By definition, greater OR equal to */
			return 0;
		case LE:
			//return parseRet(p->opr.op[0],p->opr.op[1],LE);
			if (operandType(p->opr.op[0],p->opr.op[1])) {
				addI(R_LESS);}
			else{
				addI(I_LESS);}
			/* Now stack has x < y result */
			if (operandType(p->opr.op[0],p->opr.op[1])) {
				addI(R_EQUAL);}
			else{
				addI(I_EQUAL);}
			/* Order of operations here doesn't matter.  technically we're doing
			 equal to or less */
			/* Now stack has x == y (result), x < y (result) */
			addI(I_OR); /* By definition, less OR equal to. */
			return 0;
		case NE:
			//return parseRet(p->opr.op[0],p->opr.op[1],NE);
			if (operandType(p->opr.op[0],p->opr.op[1])) {
				addI(R_EQUAL);}
			else{
				addI(I_EQUAL);}
			addI(I_NOT);
			return 0;
		case EQ:
			//return parseRet(p->opr.op[0],p->opr.op[1],EQ);
			if (operandType(p->opr.op[0],p->opr.op[1])) {
				addI(R_EQUAL);}
			else{
				addI(I_EQUAL);}
			return 0;
	}
    }
    return 0;
}
	 
/* Simple helper function that notifies the caller what type of
 instruction to use. Real or Integer.
 Also, this function handles making sure both values on the stack
 are of the same instruction type before returning the type to use.
 --JKB
 */
bool operandType(nodeType* a,nodeType* b){
	/* check whether or not either of the operands are a variable */
	bool var1 = ((a->type == typeId) || (a->type == initIdtype));
	bool var2 = ((b->type == typeId) || (b->type == initIdtype));
	bool fl = false;
	ex(a);
	if (retval == TYPE_FLOAT) { /* variable was a Real */
		fl = true;
		if (var1)
			addI(R_VALUE); /* dereference value of variable real */
	}else{
		if (var1)
			addI(I_VALUE); /* Output the integer value of the variable */
	}
	ex(b);
	if (retval == TYPE_FLOAT) { /* second operand was a Real as well */
		if (var2)
			addI(R_VALUE); /* dereference value of variable real */
		if (!fl) {	/* If the first operand was not a real */
			addI(I_SWAP); /* Swap the new values on the stack */
			addI(I_TO_R); /* Change integer to Real */
			addI(I_SWAP); /* Swap back */
		}
		fl = true;
	}else if(fl){
		if (var2)
			addI(I_VALUE); /* dereference value of variable int */
		addI(I_TO_R);
		retval = 2;
	}else
		if (var2)
			addI(I_VALUE); /* dereference value of variable int */
	return fl; /* Return operandType */
}

int	createSymbolEntry(nodeType* p){
	symbol_entry* e = (symbol_entry*)malloc(sizeof(symbol_entry));
	if (getSymbolEntry(p->id.s) == 0){
		//printf("Assigning type: %d to %s\n",p->symType,p->id.s);
		e->type = p->symType;
		e->name = p->id.s;
		e->addr = GetPos();
		e->size = 1;
		//varCount++;
		addSymbol(e,p->lineNum);
		return 0;
	}else if ((getSymbolEntry(p->id.s)->blk_level != getCurrentLevel()))
	{
		//printf("Assigning type: %d to %s in a new scope\n",p->symType,p->id.s);
		e->type = p->symType;
		e->name = p->id.s;
		e->addr = GetPos();
		e->size = 1;
		//varCount++;
		addSymbol(e,p->lineNum);
		return 0;
	}
	return 1;
}
