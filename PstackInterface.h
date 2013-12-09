#ifndef PSTACKINTERFACE_H
#define PSTACKINTERFACE_H

/*added by Kyle */
#ifdef __cplusplus
extern "C" {
#endif

void addI(int);
void addF(float);
void begin_prog();
void end_prog(int);
void begin_proc(int startPos);
void end_proc(int varCount);
int writeOut(char*,int);
int GetPos(void);
int I_valAtPos(int);
int* I_refToPos(int);
float F_valAtPos(int);
float* F_refToPos(int);

#ifdef __cplusplus
}
#endif

#endif /* PSTACKINTERFACE_H */
