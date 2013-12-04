
/*
 * Files developed for CS 441 and CS 541 Compiler Courses
 * taught by Jerzy W. Jaromczyk,
 * Department of Computer Science, University of Kentucky
 * Copyright 2000-2013
*/
/*
 * PROJECT         : CS 441, tigerc (programming assignment #4)
 * FILE NAME       : code.cc
 * PROGRAMMER      : NFM
 *
 * DATE WRITTEN    : 29 Apr 2000
 *
 * SYNOPSIS        : Implements class PstackCode.  Defines a number of functions
 *                   (see below).  To use, #include "pstcode.h".
 *
 * DESCRIPTION     : Class implementing a block of pstack code.  The code
 *                   is represented using a vector of integers.
 *
 * MODIFICATION HISTORY:
 *          06 Dec 2010      Use vector of pstack WORDs, not int; add
 *                           floating-point support.
 *          29 Apr 2000      Put implementation into code.cc (was inline).
 *                           Move prolog from main.cc to code.cc.
 *          28 Apr 2000      Add string support, and patchup for prolog.
 *          25 Apr 2000      Initial version.
 */

#include <string>
#include <list>
#include <vector>
#include <utility>
#include <cstdio>
#include "pstcode.h"

using namespace std;

PstackCode::PstackCode() : code(0)
{}

PstackCode::~PstackCode()
{}

void PstackCode::add(int op)
{
	WORD w;
	w.Integer = op;
	code.push_back(w);
}
void PstackCode::add(float op)
{
	WORD w;
	w.Real = op;
	code.push_back(w);
}

void PstackCode::add_dup()
{
	// -- w
	add(I_CONSTANT);
	add(ADDR_TEMP);
	// -- addr_temp w
	add(I_SWAP);
	// -- w addr_temp
	add(I_ASSIGN);
	add(1);
	add(I_CONSTANT);
	add(ADDR_TEMP);
	add(I_VALUE);
	// -- w
	add(I_CONSTANT);
	add(ADDR_TEMP);
	add(I_VALUE);
	// -- w w
}

void PstackCode::add_2dup()
{
	// -- w x  (want -- w x w x)
	add(I_CONSTANT);
	add(ADDR_TEMP);
	// -- addr_temp w x
	add(I_SWAP);
	// -- w addr_temp x
	add(I_ASSIGN);
	add(1);
	// -- x
	add(I_CONSTANT);
	add(ADDR_TEMP2);
	// -- addr_temp2 x
	add(I_SWAP);
	// -- x addr_temp2
	add(I_ASSIGN);
	add(1);
	// --
	add(I_CONSTANT);
	add(ADDR_TEMP2);
	// -- addr_temp2
	add(I_VALUE);
	// -- x
	add(I_CONSTANT);
	add(ADDR_TEMP);
	// -- addr_temp x
	add(I_VALUE);
	// -- w x
	add(I_CONSTANT);
	add(ADDR_TEMP2);
	// -- addr_temp2 w x
	add(I_VALUE);
	// -- x w x
	add(I_CONSTANT);
	add(ADDR_TEMP);
	// -- addr_temp x w x
	add(I_VALUE);
	// -- w x w x
}


void PstackCode::add_3pick()
{
	// -- w x y  (want -- y w x y)
	add(I_CONSTANT);
	add(ADDR_TEMP);
	// -- addr_temp w x y
	add(I_SWAP);
	// -- w addr_temp x y
	add(I_ASSIGN);
	add(1);
	// -- x y
	add(I_CONSTANT);
	add(ADDR_TEMP2);
	// -- addr_temp2 x y
	add(I_SWAP);
	// -- x addr_temp2 y
	add(I_ASSIGN);
	add(1);
	// -- y
	add(I_CONSTANT);
	add(ADDR_TEMP3);
	// -- addr_temp3 y
	add(I_SWAP);
	// -- y addr_temp3
	add(I_ASSIGN);
	add(1);
	// --
	add(I_CONSTANT);
	add(ADDR_TEMP3);
	add(I_VALUE);
	// -- y
	add(I_CONSTANT);
	add(ADDR_TEMP2);
	add(I_VALUE);
	// -- x y
	add(I_CONSTANT);
	add(ADDR_TEMP);
	add(I_VALUE);
	// -- w x y
	add(I_CONSTANT);
	add(ADDR_TEMP3);
	add(I_VALUE);
	// -- y w x y
}

void PstackCode::begin_prog()
{
  // execution starts at address 1
  add(0);
  add(I_PROG);
  add(0); // address 2 will be patched by end_prog()
  add(pos() + 1);
}


void PstackCode::end_prog(int varsize) /* modified jwj: add parameter */
{
  at(2) = varsize;  // patch I_PROG's first parameter - added jwj
  add(I_ENDPROG);
}

void PstackCode::begin_proc()
{
  add(I_PROC);
  add(0);
  add(pos()+1);
}

void PstackCode::end_proc(int patch_loc, int varsize)
{
  printf("patch_loc: %d varsize: %d\n", patch_loc, varsize);
  at(patch_loc) = varsize;
  add(I_ENDPROC);
}

bool PstackCode::write(const string &file, bool binary) const
{
	FILE *str;
	str = fopen(file.c_str(), "w");

	if(!str)
		return false;

	vector<WORD>::const_iterator i;
	for(i=code.begin(); i!=code.end(); ++i) {
		if(binary) {
			fwrite(&*i, sizeof(WORD), 1, str);
		} else {
			// NOTE: ASCII format does not support floats!!
			fprintf(str, "%d\n", i->Integer);
		}
	}

	if(ferror(str)) {
		fclose(str);
		return false;
	}
	fclose(str);
	return true;
}

int PstackCode::pos() const
{
	return code.size();
}

int PstackCode::at(int p) const
{
	return code[p].Integer;
}

int &PstackCode::at(int p)
{
	return code[p].Integer;
}

float PstackCode::f_at(int p) const
{
	return code[p].Real;
}

float &PstackCode::f_at(int p)
{
	return code[p].Real;
}
