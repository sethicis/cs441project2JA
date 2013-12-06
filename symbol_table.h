
#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

/* Added JWJ */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct symbol_entry symbol_entry;
struct symbol_entry {
	int 	addr;
	int 	blk_level;
	int 	type;       /* May actually  be used to determine if local or param */
	int 	size;
	int 	offset;
   union { /* Added */
     int i;
     float f;
   }val;
	const char *name; /* changed JWJ */
};

extern void printSymbolTable();
extern void popSymbolTable();
extern void pushSymbolTable(); // once
extern void addSymbol(struct symbol_entry*, int lineno); // need this
extern struct symbol_entry* getSymbolEntryByRelAddr(int, int);
extern struct symbol_entry* getSymbolEntry(const char *); // need this
extern int getTotalSymbolTableSize();
extern int getCurrentSymbolTableSize();
extern int getSymbolTableSize(int);
extern int getCurrentLevel();

/* Added JWJ */
#ifdef __cplusplus
}
#endif

#endif /* SYMBOL_TABLE_H */

