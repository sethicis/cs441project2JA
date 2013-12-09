cs441project2JA
===============

Milestone A
===========

Contributors:   Kyle Blagg
                Alex Kane

Multi line declarations and multi line assignmnents works.
do-while and repeat until works.
C / C++ style variable declarations.

NOTE WORKING: Right now our code can only output integer values.
The scanner and parser properly identify doubles, and ints but
we cannot output doubles or properly perform operations on them.

This is a cs441 project for modifying a pascal like calculator.

Milestone B
===========

Contributors:	Kyle Blagg
		Alex Kane

P-code Generation for Expressions consisting of numbers 
(behavior for variables is undefined, however it dereferences 
variable values as a number) for the "print" statement.

Design improvements were made to associate the P-code 
Generation for the program, so testing is handled easier.

I/O Redirection must be used for Program Execution for output file.
STDOUT is used for P-Code Generation Output, while STDERR is
used for Error Message Handling Outputs.

For example:
"./calc3a.exe > test.aa"

Continue execution of statements until satisfied, then use 
Ctrl+D for "EOF" so it can associate program end for P-code Generation.

Milestone C
=================

NOT WORKING:
float variables are still not tested.
blocking is not tested by code in place.
condition statements and loops some code is in place, but untested.
P-stack code is in place to recognize begin and end statements,
but is not fully tested with latest changes.
Code in general is still very much work in progress.
Issue right now, trying to figure out how to determine jump
locations for conditional statements and loops.
Repeate until and do while loops have p-code in place, but are untested.

Iteration by Kyle.
Last modified: 12-8-13
