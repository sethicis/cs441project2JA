#ifndef CALC_3_H
#define CALC_3_H

typedef enum { typeCon, typeFloat, typeId, typeOpr, initIdtype} nodeEnum; /* added */

/* constants */
typedef struct {
    int value;                  /* value of constant */
} conNodeType;

/* floats - added */
typedef struct {
    float value;                /* value of float */
} floatNodeType;

/* identifiers */
typedef struct {
    char *s;	                /* variable name - changed JWJ */
} idNodeType;

/* operators */
typedef struct {
    int oper;                   /* operator */
    int nops;                   /* number of operands */
    struct nodeTypeTag *op[1];  /* operands (expandable) */
} oprNodeType;

typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */
	int declar;
	int symType;
	int lineNum;
    /* union must be last entry in nodeType */
    /* because operNodeType may dynamically increase */
    union {
        conNodeType con;        /* constants */
        floatNodeType fl;       /* floats */
        idNodeType id;          /* identifiers */
        oprNodeType opr;        /* operators */
    };
} nodeType;

#ifdef __cplusplus
extern "C" {
#endif
    int ex(nodeType*);
#ifdef __cplusplus
}
#endif

#define TYPE_INT   1
#define TYPE_FLOAT 2

/* removed extern int sym[26]; JWJ */

#endif /* CALC_3_H */