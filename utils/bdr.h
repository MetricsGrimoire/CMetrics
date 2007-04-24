#ifndef BDR_H
#define BDR_H

typedef char	Bool;
#define True 1
#define False 0

typedef char	Status;
#define Error	0
#define Ok	1

typedef	char	FileIO;		/* also uses Error and Ok */
#define End_File	2

/* xxNULL retained for compatibility until some reasonable percentage of
 * code is changed, but xx_NIL is now preferred usage for 'null' pointer
*/
#define CNULL	( (char *) 0)
#define C_NIL	( (char *) 0)

#define FNULL	( (FILE *) 0)
#define F_NIL	( (FILE *) 0)

#define CNTRL(X)	('X' - 64)
#define Odd(X)	(X & 1)

/* Global means used througout system,
 * Local means it should be private to module */
#define Global	extern
#define Local	extern

#endif /* BDR_H */
