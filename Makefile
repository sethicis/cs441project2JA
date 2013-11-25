## This makefile needs editing to work
CC         = gcc -Wall
OPTIONS    = -lfl
OBJECTS    = y.tab.o lex.yy.o symbol_table.o calc3a.o
HEADERS    = calc3.h symbol_table.h y.tab.h
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
calc3a: $(OBJECTS) $(HEADERS)
	g++ -Wall $(OBJECTS) -o calc3a.exe
calc3a.o: $(HEADERS)
	$(CC) -c calc3a.c
#calc3b: $(OBJECTS)
#	$(CC) $(OBJECTS) calc3b.c -o calc3b.exe
#calc3g: $(OBJECTS)
#	$(CC) $(OBJECTS) calc3g.c -o calc3g.exe
y.tab.o: $(HEADERS) y.tab.c
	$(CC) -c y.tab.c
lex.yy.o: $(HEADERS) lex.yy.c
	$(CC) -c lex.yy.c
y.tab.c: calc3.y
	bison -y -d calc3.y
lex.yy.c: calc3.l
	flex calc3.l
symbol_table.o: $(HEADERS) symbol_table.cc
	g++ -Wall -c symbol_table.cc
clean:
	rm -f *.o *.exe lex.yy.c y.tab.c
