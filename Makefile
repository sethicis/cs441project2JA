## This makefile needs editing to work
CC         = gcc -Wall -g
OPTIONS    = -lfl
OBJECTS    = y.tab.o lex.yy.o symbol_table.o calc3p.o PstackInterface.o pstcode.o
HEADERS    = calc3.h symbol_table.h y.tab.h PstackInterface.h pstcode.h apm.h globals.h
FLEX       = /usr/bin/flex
BIN_NAME   = cs441-sample

# build environment for cygwin

# bison -d calc1.y    produces calc1.tab.c
# bison -y -d calc1.y produces y.tab.c (the standard yacc output)

# for calc1 and calc2, you may have to
# include the following in the lex ".l" file:
#    extern int yylval;

# calc1
#calc1.exe
#	bison -y -d calc1.y
#	flex calc1.l
#	gcc -c y.tab.c lex.yy.c
#	gcc y.tab.o lex.yy.o -o calc1.exe

#calc2
#	bison -y -d calc2.y
#	flex calc2.l
#	gcc -c y.tab.c lex.yy.c
#	gcc y.tab.o lex.yy.o -o calc2.exe

#calc3
calc3p: $(OBJECTS) $(HEADERS)
	g++ -Wall -g $(OBJECTS) -o $@
calc3p.o: calc3p.cc
	g++ -c calc3p.cc
y.tab.o: y.tab.c calc3.y
	$(CC) -c y.tab.c
lex.yy.o: lex.yy.c calc3.l
	$(CC) -c lex.yy.c
y.tab.c: calc3.y
	bison -y -d calc3.y
lex.yy.c: calc3.l
	flex calc3.l
PstackInterface.o: PstackInterface.cc
	g++ -c PstackInterface.cc
pstcode.o: pstcode.cc
	g++ -c pstcode.cc
symbol_table.o: symbol_table.cc
	g++ -Wall -c symbol_table.cc
clean:
	rm -f *.o *.exe calc3p lex.yy.c y.tab.c y.tab.h
