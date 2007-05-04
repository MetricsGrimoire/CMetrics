/* strip comments from c program */
/* read from stdin, write to stdout */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "argfiles.h"

FILE	*nextfp();
int	filetype();

FILE *
nextfp( argc, argv, p_filetype)
	int	argc;
	char	*argv[];
	int	*p_filetype;
{
	/* looks through parameters trying to return next FILE * to next
	 * specified file
	 * passes back the filetype as a side effect
	 */
	
	static Bool	first = True;
	static int	index = 1;
	static FILE	*result = FNULL;
	int	curr_index, temp_type;

	temp_type = SYSERR;	/* default to ensure no accidental execution */
	
	if ( result != FNULL )
	{
		fclose( result );
		result = FNULL;
	}
	/* skip over any flags to this routine */
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
		curr_index = index++;
	}
	if ( first )
	{
		/* if no files specified, read from stdin */
		if ( result == FNULL )
			result = stdin;
		first = False;
	}
	if ( result != FNULL )
	{
		if ( result == stdin )
			temp_type = STDINP;
		else
			temp_type = filetype( argv[curr_index] );
	}
	*p_filetype = temp_type;
	return ( result );
}

int
filetype( filename )
	char	*filename;
{
	int	type;
	register int	len;
	register char	*suffix;

	if ( !filename )
		type = SYSERR;
	else
	{
		/* find where the suffix starts */
		len = strlen(filename);
		suffix = filename + len - 1; /* end of string */
		while ( *suffix != '.' && suffix != filename )
			suffix--;

		if ( suffix == filename )
		{
			if ( *filename == 'M' || *filename == 'm' )
			{
				if (strncmp(filename+1,"akefile",7)==0)
					type = MAKEFILE;
				else
					type = OTHER;
			}
		}
		else
		{
			/* if suffix is one char long, optimize by doing
			 * switch on char, otherwise, have to do strcmp
			 */
			if ( strlen(++suffix) == 1 )
			{
				switch ( *suffix )
				{
				case 'c':	/* a C file */
					type = C;
					break;
				case 'h':	/* a header file */
					type = HEADER;
					break;
				case 'y':	/* a yacc file */
					type = YACC;
					break;
				case 'p':	/* a pascal file */
					type = PASCAL;
					break;
				case 's':	/* an assembly file */
					type = ASSEMBLY;
					break;
				case 'm':	/* a Visual/Menu file */
					type = VMENU;
					break;
				default:
					type = OTHER;
					break;
				}
			}
			else
			{
				if ( strcmp(suffix, "sh") == 0 )
					type = SHELL;
				else if (strcmp(suffix, "cob") == 0)
					type = COBOL;
				else if (strcmp(suffix, "awk") == 0)
					type = AWK;
				else
					type = OTHER;
			}
		}
	}
	return type;
}
