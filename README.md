cs441project2JA
===============

Final Submission overview:
--------------------------

Almost everything in our project works except for functions.
Keywords like 'do' 'while' 'for' 'to' 'step' 'quit' ect. can NOT be used as variable names
these are reserved names. Also, valid variable names must begin with either upper/
lower alpha characters or one or more underscores '_'.
We did not have time to complete the logic for generating code for the void functions, but we do have dummy logic in place for recognizing the grammar of a function.
Things to note.  Our program expects that do-while loops will be ended in c++ style so they expect a ';' at the end of the while condition.  Furthermore, our type checking is handled in the compile time.  We don't stop the code when operating on values of different datatypes instead we convert the values we are operating on reals if any value in the expression is a real, but if the operation is being assigned to a variable the result is converted into the datatype appropriate for that variable before assignment.  This logic works well and allows for a lot more natural interaction with the calculator program.
Speaking of variables.  Our variables automatically set a default value of '0' on initialization, thus if you feel like jumping the gun and declaring a variable x and immediately printing it out, that's fine, you'll just get a nice zero.
Our for-loops expect the incrementer amount to be a constant integer.  Reals are not accepted as incrementers, nor are variables.
Though functions are not supported in this program 'begin' and 'end' blocks
are and can be nested within each other to define separate scopes.
Note: As long as a variable identifier has not be initialized in a 'begin' 'end' block
the variable associated with that identifier can be referenced in a lower scope
level. I.E. global variables.
	Note, behavior for referencing variables in lower scopes is undefined, but
	doing so will most likely result in syntax error, or worse.
	
USAGE:
	Our calculator program expects that instructions will be fed to it via stdin
	I/O redirection, though it is possible to type instructions into the
	calculator program like an interpretor. (NOTE: If one is using the program
	like an interpretor you can exit the program by either sending a EOF character
	(ctrl+d) or by typing "quit" to signal the program to terminate and writeout)
	When the program completes an output file is generated with the name
	calc3p_out.apm.  This file is a binary file that can be executed with the
	api file.
	Variables MUST be declared with a datatype of 'int' or 'float' before they
	can be used in the program. (See the first paragraph for more information about
	variables and their use.)
	Different program scopes can be defined with the use of 'begin' and 'end'
	keywords, but functions are not currently supported.


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

Milesone D/E
================

Contributors: Kyle Blagg, Alex Kane.

NOT WORKING or QUERKY:

Only dummy code exists for recognizing the grammar of a function.  No actual p-stack code generation occures or function tracking takes place.

Type checking is handled in compile time.  Instead of throwing an error we opted to actually convert the instructions to the same datatype before operating on it, but we do have logic in place at almost all critical points to catch datatypes and use the appropriate instructions.

Our do while loops are modeled after c++ style do while loops so they expect a ';' at the end of the while condition.

WORKING:
--------

semantic type checking, nested blocking multiple levels with little to no issues during testing, for loops, while loops, do while loops, repeat until, if else logic, variable names, operators, extremely big print statements, variable to variable assignment, multi line assignment of variables.
Besides a few of our own small tests we were able to get through testing file test-0 through test-3 with flying colors. (Note: test-3 was different because of the way we handle type checking.)

