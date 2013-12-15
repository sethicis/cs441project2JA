#include "PstackInterface.h"
#include "pstcode.h"
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
void begin_proc(int startPos){
	chkDef();
	addI(I_JR);
	addI(0);
	
	proc_Start_Pos->push(startPos);
	code->begin_proc();
}
void end_proc(int varCount){
	chkDef();
	code->end_proc(proc_Start_Pos->top(),varCount);
	proc_Start_Pos->pop();
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
