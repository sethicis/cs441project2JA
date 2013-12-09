

// Hey emacs!  This is a -*- C++ -*- header file!

/*
 * PROJECT         : CS 441,JWJ CS UK
 * FILE NAME       : pstcode.h
 * PROGRAMMER      : prepred for CS 441 (NFM)
 *
 * DATE WRITTEN    : 29 Apr 2000
 *
 * SYNOPSIS        : Interface for class PstackCode.  An implementation is
 *                   provided in pstcode.cc (q.v.).
 *
 * DESCRIPTION     : Class implementing a block of pstack code.  The code
 *                   is represented using a vector of integers.
 *
 * MODIFICATION HISTORY:
 *          29 Apr 2000      Put implementation into code.cc (was inline).
 *          28 Apr 2000      Add string support, and patchup for prolog.
 *          25 Apr 2000      Initial version.
 */

#ifndef _PSTCODE_H_INCLUDED
#define _PSTCODE_H_INCLUDED

#include <string>
#include <list>
#include <vector>
#include "apm.h"

// Temporaries used by Code::add_*()
#define ADDR_TEMP  (STORAGE - 1)
#define ADDR_TEMP2 (STORAGE - 2)
#define ADDR_TEMP3 (STORAGE - 3)

class PstackCode
{
    public:
	PstackCode();
	virtual ~PstackCode();

	// Add an instruction or (non-relocatable) operand
	virtual void add(int op);

  // Add a real operand
  virtual void add(float op);

	// Generate code to duplicate the top element of the stack.
	virtual void add_dup();

	// Generate code to duplicate the top two elements of the stack.
	virtual void add_2dup();

	// Generate code to copy the 3rd element on the stack to the topa
	virtual void add_3pick();

	// Generate code to begin a program
	virtual void begin_prog();

	// Generate code to end a program
	// virtual void end_prog();  removed jwj
	virtual void end_prog(int); // added jwj

        // Generate code to start a process block
        virtual void begin_proc();

        // Generate code to end a process block
        virtual void end_proc(int patch_loc, int varsize);

	// Write the code to a file.
	virtual bool write(const std::string &file, bool binary) const;

	// Return the current position.
	virtual int pos(void) const;

	// Return a reference to the instruction at the specified position.
	virtual int at(int p) const;
	virtual int &at(int p);
  // Return a reference to a float stored at the specified position.
  virtual float f_at(int p) const;
  virtual float &f_at(int p);
    private:
	std::vector<WORD> code;
};

#endif // _PSTCODE_H_INCLUDED
