/*
 This module works as an interface for any C code that may need to access
 features of the pstcode.cc class structure.
*/
#ifndef PSTACKINTERFACE_H
#define PSTACKINTERFACE_H

/*added by Kyle */
#ifdef __cplusplus
extern "C" {
#endif

void addI(int);		/* Adds an instruction to the p stack */
void addF(float);	/* Adds a real to the p stack */
void begin_prog();	/* Begins program */
void end_prog(int); /* Ends program */
void begin_proc(); /* Creates a new nested block and scope */
void end_proc();	/* Ends nested block and scope */
int writeOut(char*,int);	/* Writes the pstack code to a file */
int GetPos(void);			/* Returns the address of the next instruction */
int I_valAtPos(int);		/* Returns the instruction value at pos */
int* I_refToPos(int);		/* Returns a pointer to an instruction value at pos */
float F_valAtPos(int);	/* Returns a real value from the stack @ pos */
float* F_refToPos(int);	/* Returns a pointer to a real value from the stack @ pos*/

#ifdef __cplusplus
}
#endif

#endif /* PSTACKINTERFACE_H */
