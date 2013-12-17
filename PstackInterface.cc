#include "PstackInterface.h"
#include "pstcode.h"
#include "symbol_table.h"
#include <stack>

std::stack<int>* proc_Start_Pos;
std::stack<int>* proc_Addr_Pos;
PstackCode* code; /* Global P stack pointer */

void chkDef(){
	if (code == NULL)
		code = new PstackCode;
	if (proc_Start_Pos == NULL) {
		proc_Start_Pos = new std::stack<int>;
	}
	if (proc_Addr_Pos == NULL) {
		proc_Addr_Pos = new std::stack<int>;
	}
}

void addI(int op){
	chkDef();
	code->add(op);
}

void addF(float op){
	chkDef();
	code->add(op);
}

void begin_prog(){
	chkDef();
	code->begin_prog();
}

void end_prog(int varlen){
	chkDef();
	code->end_prog(varlen);
}
void begin_proc(){
	chkDef();
	/* Setup the relative jump call */
	addI(I_JR);
	addI(0);
	proc_Addr_Pos->push(GetPos() - 1);
	proc_Start_Pos->push(GetPos() + 1);
	code->begin_proc();
}
void end_proc(){
	chkDef();
	/* Supply the address of where the code started and get the number of vars in
	 current stack */
	code->end_proc(proc_Start_Pos->top(),getCurrentSymbolTableSize());
	/* Set the relative jmp just after the process ends */
	*I_refToPos(proc_Addr_Pos->top()) = GetPos() - proc_Addr_Pos->top() + 1;
	addI(I_CALL);
	addI(getCurrentLevel()-1);
	addI(proc_Start_Pos->top()- 1); /* Call the process block just made */
	proc_Start_Pos->pop(); /* Remove the block from addr */
	proc_Addr_Pos->pop();  /* End of proc block */
}

int writeOut(char* fileName,int binary){
	chkDef();
	std::string tmp(fileName);
	return code->write(tmp,(bool)binary);
}

int GetPos(){
	chkDef();
	return code->pos();
}

int I_valAtPos(int p){
	chkDef();
	return code->at(p);
}

int* I_refToPos(int p){
	chkDef();
	return &(code->at(p)); /* Not sure if this code will work.
							Hopefully the instruction's address is returned
							and not a copy of it */
}

float F_valAtPos(int p){
	chkDef();
	return code->f_at(p);
}

float* F_refToPos(int p){
	chkDef();
	return &(code->f_at(p));
}
