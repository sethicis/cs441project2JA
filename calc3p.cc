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
void scopeCheck(const char *name);
int* tmp; /* Temporary variable pointing to an instruction to change */
int currP;
int forCurrP;
int forCondition;

int ex(nodeType *p) {
    if (!p) return 0;
	printf("Entering ex\n");
    switch(p->type) {
		case typeCon:       //retVal.Integer = p->con.value; return 1;
			printf("Entering typeCon\n");
			addI(I_CONSTANT);
			addI(p->con.value);
			retval = 1;
			return 0;
		case typeFloat:     //retVal.Real = p->fl.value; return 2;
			printf("Entering typeFloat\n");
			addI(R_CONSTANT);
			addF(p->fl.value);
			retval = 2;
			return 0;
		case typeId:
		printf("Entering typeId\n");
		{
			symbol_entry* e;
			e = (symbol_entry*)malloc(sizeof(symbol_entry));
			if (getSymbolEntry(p->id.s) == 0){
				e = (symbol_entry*)malloc(sizeof(symbol_entry));
				e->type = p->symType;
				e->name = p->id.s;
				e->addr = GetPos();
				e->size = 1;
				//varCount++;
				addSymbol(e,p->lineNum);
			}else if ((getSymbolEntry(p->id.s)->blk_level != getCurrentLevel()) && !p->declar)
			{
				e = (symbol_entry*)malloc(sizeof(symbol_entry));
				e->type = p->symType;
				e->name = p->id.s;
				e->addr = GetPos();
				e->size = 1;
				//varCount++;
				addSymbol(e,p->lineNum);
			}
		}
	if (getSymbolEntry(p->id.s)->type == TYPE_INT){
		addI(I_VARIABLE);
		addI(getCurrentLevel()-getSymbolEntry(p->id.s)->blk_level);
		addI(getSymbolEntry(p->id.s)->offset);
		addI(I_VALUE);
		retval = 1;
		//std::cout << "For variable: " << p->id.s << " Blk_lvl: " << getSymbolEntry(p->id.s)->blk_level << " offset: " << getSymbolEntry(p->id.s)->offset << std::endl;
		//std::cout << "Value is: " << getSymbolEntry(p->id.s)->val.i << std::endl;
	}else{
        //retVal.Real = getSymbolEntry(p->id.s)->val.f; return 2;
		addI(R_VARIABLE);
		//scopeCheck(p->id.s);
		addI(getSymbolEntry(p->id.s)->blk_level);
		addI(getSymbolEntry(p->id.s)->offset);
		addI(R_VALUE);
		//std::cout << "Value is: " << getSymbolEntry(p->id.s)->val.f << std::endl;
		retval = 2;}
	return 0;
    case typeOpr:
		printf("Entering ex opr\n");
        switch(p->opr.oper) {
			case BEGIN_PROC:
				//printSymbolTable();
				pushSymbolTable();
				//addI(I_JR);
				//addI(0);
				//currP = GetPos() - 1;
				begin_proc();
				ex(p->opr.op[0]); //Does not support return values right now
				//std::cout << "Current symbol table size: " << getCurrentSymbolTableSize() << std::endl;
				end_proc();
				/* Set the relative jmp just after the process ends */
				//*I_refToPos(currP) = GetPos() - currP + 1;
				//addI(I_CALL);
				//addI(getCurrentLevel());
				//addI(currP+1); /* Call the process block just made */
				popSymbolTable(); printf("Removing Symbol Table\n");
				return 0;
			case FOR:
				{
					//std::cout << "Beginning For Loop!" << std::endl;
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
					//std::cout << "Ending For Loop!" << std::endl;
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
			addI(I_CONSTANT);
			addI(0);		/* Push constant false onto stack */
			addI(I_JMP_IF_FALSE);
			addI(0);		/* Put placeholder on stack */
			currP = GetPos();/* Save the current position right before the statement */
			ex(p->opr.op[1]); /* Statement code */
			tmp = I_refToPos(currP - 1); /* Get a reference to the placeholder */
			//std::cout << "About to I_refToPos()" << std::endl;
			*tmp = GetPos(); /* Set the placeholder to the condition code */
			
			ex(p->opr.op[0]); /* Condition code */
			addI(I_JMP_IF_TRUE); /* If true run the statement code until false */
			addI(currP);		/* Add address of statement code */
			//std::cout << "After I_refToPos()" << std::endl;
			tmp = NULL;
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
                       		//ex(p->opr.op[1]); return parseRet(p->opr.op[0],NULL,',');
				ex(p->opr.op[1]);
				ex(p->opr.op[0]);
				return 0;
			}
			else{
				ex(p->opr.op[1]);
				return 0;
			}
		case PRINT:
			ex(p->opr.op[0]);
			if (retval == 1) {
				addI(I_WRITE);
				addI(1);}
			else{
				addI(R_WRITE);
				addI(1);}
			return 0;
		case ';':
			ex(p->opr.op[0]);
			ex(p->opr.op[1]);
			return 0; /* Return from case */
		case '=':
			/* Check the variables type and perform the assignment as appropriate */
			/* If not Initalized do it! */
			if (getSymbolEntry(p->opr.op[0]->id.s) == 0) {
				symbol_entry* e;
				e = (symbol_entry*)malloc(sizeof(symbol_entry));
				if (getSymbolEntry(p->opr.op[0]->id.s) == 0){
					e = (symbol_entry*)malloc(sizeof(symbol_entry));
					e->type = p->opr.op[0]->symType;
					e->name = p->opr.op[0]->id.s;
					e->addr = GetPos();
					e->size = 1;
					//varCount++;
					addSymbol(e,p->opr.op[0]->lineNum);
			}else if ((getSymbolEntry(p->opr.op[0]->id.s)->blk_level != getCurrentLevel()) && !p->opr.op[0]->declar)
				{
					e = (symbol_entry*)malloc(sizeof(symbol_entry));
					e->type = p->opr.op[0]->symType;
					e->name = p->opr.op[0]->id.s;
					e->addr = GetPos();
					e->size = 1;
					//varCount++;
					addSymbol(e,p->opr.op[0]->lineNum);
				}
			}
			if (getSymbolEntry(p->opr.op[0]->id.s)->type == TYPE_INT)
			{
					addI(I_VARIABLE);
					addI(getSymbolEntry(p->opr.op[0]->id.s)->blk_level);
					addI(getSymbolEntry(p->opr.op[0]->id.s)->offset);
				ex(p->opr.op[1]); /* Get the value to be assigned */
				if (retval == 1) { /* If the value being assigned is int */
					retVal.Integer = I_valAtPos(GetPos()-1);
					printf("Assigning: %d to %s\n",retVal.Integer,p->opr.op[0]->id.s);
					getSymbolEntry(p->opr.op[0]->id.s)->val.i = retVal.Integer;
					addI(I_ASSIGN); /* If the value to be assigned is an int */
					addI(1);
				}
				else{ /* Else the value to be assigned is a Real */
					retVal.Real = F_valAtPos(GetPos()-1); /* If the value is a Real */
					getSymbolEntry(p->opr.op[0]->id.s)->val.i = retVal.Real;
					addI(R_TO_I); /* Convert real to int before assignment */
					addI(I_ASSIGN);
					addI(1);}
					return 0;
			}else{
				addI(R_VARIABLE);
				addI(getSymbolEntry(p->opr.op[0]->id.s)->blk_level);
				addI(getSymbolEntry(p->opr.op[0]->id.s)->offset);
				ex(p->opr.op[1]);
				if (retval == 1) {
					retVal.Integer = I_valAtPos(GetPos()-1);
					getSymbolEntry(p->opr.op[0]->id.s)->val.f = retVal.Integer;
					addI(I_TO_R);
					addI(R_ASSIGN);
					addI(1);}
				else{
					retVal.Real = F_valAtPos(GetPos()-1);
					getSymbolEntry(p->opr.op[0]->id.s)->val.f = retVal.Real;
					addI(R_ASSIGN);
					addI(1);}
				return 0;
			}
		case UMINUS:
			ex(p->opr.op[0]);
			if (retval == 1) {
				addI(I_MINUS);}
			else{
				addI(R_MINUS);}
			return 0;
		case '+':
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


// Check if Variable Exists Within Scope and Lower Scopes
void scopeCheck(const char *var_name){
	//NOTE::: Commented out because code MAY be depricated...
	int scope_level = getCurrentLevel();
	int table_size;

	std::cout << "Variable Reference Called(I think...)! Checking Scope Level for Variable!" << std::endl << std::endl;

	// If variable name is no where in the entire stack
	if(!(getSymbolEntry(var_name)))
	{
		std::cout << "Variable hasn't been declared or doesn't exist!" <<
		std::endl << std::endl;
	}
	else // Variable name exists in entire stack somewhere
	{
		// Do a Top-Bottom Scope Level Depth Check
		for(int j = scope_level; j >= 0; j--)
		{
			// Obtain Current Activation Record Size
			table_size = getSymbolTableSize(j);

			// Check all variables within record
			for(int i = table_size; i > 0; i--)
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
					std::cout << "Variable Offset Value: ";
					std::cout << i + 2 << std::endl;
					std::cout << "Scope Offset Level: ";
					std::cout << j << std::endl;

					std::cout << "Variable " << var_name;
					std::cout << " exists within " << j;
					std::cout << " Scope Level(s) Away from";
					std::cout << " the Current Scope!";
					std::cout << std::endl << std::endl;
				}
			}
		}
	}
}
	 
/* Simple helper function that notifies the caller what type of
 instruction to use. Real or Integer.
 Also, this function handles making sure both values on the stack
 are of the same instruction type before returning the type to use.
 --JKB
 */
bool operandType(nodeType* a,nodeType* b){
	bool fl = false;
	ex(a);
	if (retval == 2) {
		fl = true;
	}
	ex(b);
	if (retval == 2) {
		if (!fl) {
			addI(I_SWAP);
			addI(I_TO_R);
			addI(I_SWAP);
		}
		fl = true;
	}else if(fl){
		addI(I_TO_R);
		retval = 2;
	}
	return fl;
}
