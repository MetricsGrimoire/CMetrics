/* remove strings from input - where a string is anything 
 * between (and including) two quotes (")
 */
#include <stdio.h>


typedef char	Bool;
#define True	1
#define False	0

typedef int	State;
#define	Not_String	0
#define String	1
#define Escape	2
#define Prime	3

#define FNULL	( (FILE *) 0)
#define CNULL	( (char *) 0)

#ifdef DEBUG
#define transstate(X,Y)  printf("[%d to %d]", X, Y)
#else
#define transstate(X,Y)
#endif /* DEBUG */

int
main(argc, argv)
	int	argc;
	char	*argv[];
{
	register int	input;
	register State	statevar;
	register FILE	*fp;
	State	laststate;
	char	*filename;
	Bool	long_prime=False;	/* where many chars between primes */
	Bool	inprime=False;	/* true if processing a "prime string" */

	FILE	*nextfp();

	while ( (fp = nextfp(argc, argv, &filename)) != FNULL )
	{

		statevar = laststate = Not_String;
		while ( (input = getc(fp)) != EOF )
		{
			switch ( statevar )
			{
			case Not_String:
				if ( input == '"' )
				{
					transstate(statevar, String);
					statevar = String;
				}
				else 
				{
					if ( input == '\\' )
					{
						laststate = statevar;
						statevar = Escape;
						transstate(laststate, statevar);
					}
					else if ( input == '\'' )
					{
						/* if it is a long prime string
						 * just dump the prime and
						 * continue
						 */
						if ( !long_prime )
						{
							laststate = statevar;
							statevar = Prime;
							transstate(laststate, statevar);
						}
						long_prime = False;
					}
					putchar( input );
				}
				break;
			case String:
				if ( input == '"' )
				{
					transstate( statevar, Not_String );
					statevar = Not_String;
				}
				else if ( input == '\\' )
				{
					laststate = statevar;
					statevar = Escape;
					transstate(laststate, statevar);
				}
				break;
			case Escape:	/* collapses two states, so use */
					/* laststate variable */
				if ( laststate != String )
					putchar( input );
				transstate(statevar, laststate);
				statevar = laststate;
				break;
			case Prime:
				putchar( input );
				if ( input == '\\' )
				{
					laststate = statevar;
					statevar = Escape;
					transstate(Prime, Escape);
					inprime = True;
				}
				else
				{
					if ( inprime )
					{
						if ( input == '\'' )
							long_prime = False;
						else
							long_prime = True;
						statevar = Not_String;
						transstate( Prime, Not_String );
						inprime = False;
					}
					else
						inprime = True;
				}
				break;
			default:
				fprintf(stderr,
					"%s: PROGRAM ERROR: state %d used but not defined; (file %s)\n",
					argv[0], statevar, filename);
				exit(1);
				break;
			}
		}
	}
	exit(0);
}

FILE *
nextfp( argc, argv, p_filename)
	int	argc;
	char	*argv[];
	char	**p_filename;
{
	/* looks through parameters trying to return next FILE * to next
	 * specified file
	 * passes back a pointer to the filename as a side effect
	 */
	
	static Bool	first = True;
	static int	index = 1;
	static FILE	*result = FNULL;

	*p_filename = CNULL;

	if ( result != FNULL )
	{
		fclose( result );
		result = FNULL;
	}
	while ( index < argc && *argv[index] == '-' )
		index++;

	if ( index < argc )
	{
		if ( (result = fopen( argv[index], "r")) == NULL )
		{
			fprintf(stderr, "%s: unable to open %s for read\n",
				argv[0], argv[index]);
			exit(1);
		}
		else
			*p_filename = argv[index];
		index++;
	}
	if ( first )
	{
		/* if no files specified, read from stdin */
		/* filename remains null */
		if ( result == FNULL )
			result = stdin;
		first = False;
	}
	return ( result );
}
