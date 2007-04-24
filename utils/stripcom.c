/* strip comments from c program */
/* read from stdin, write to stdout */
#include <stdio.h>
#include "bdr.h"
#include "argfiles.h"

#define COMMENT 0
#define CODE 1
#define STRING 2
#define CHARSTR 3

void	cstrip();
void	shstrip();
void	poundstrip();
void	vmenustrip();
FILE	*nextfp();

main(argc, argv)
	int	argc;
	char	*argv[];
{
	FILE	*input;
	int	filetype;

	while ( (input = nextfp(argc, argv, &filetype)) != FNULL )
	{
		switch ( filetype )
		{
		case HEADER:
			cstrip( input );
			break;
		case YACC:
			cstrip( input );
			break;
		case C:
			cstrip( input );
			break;
		case PASCAL:
			fprintf(stderr,
				"%s: does not support comment stripping for pascal programs\n",
				argv[0]);
			break;
		case SHELL:
			shstrip( input );
			break;
		case ASSEMBLY:
			fprintf(stderr,
				"%s: does not support comment stripping for assembly programs\n",
				argv[0]);
			break;
		case AWK:
			poundstrip( input );
			break;
		case VMENU:
			vmenustrip( input );
			break;
		case COBOL:
			fprintf(stderr,
				"%s: does not support comment stripping for Cobol programs\n",
				argv[0]);
			break;
		case MAKEFILE:
			poundstrip( input );
			break;
		case STDINP:
			cstrip( input );
			break;
		case OTHER:
			cstrip( input );
			break;
		default:
			fprintf(stderr,
				"%s: SYSTEM ERROR, bad filetype (%d)\n",
				filetype);
			break;
		}
	}
	exit(0);
}

void
cstrip( fp )
	register FILE	*fp;
{
	register int	c;
	register int	state=CODE;

	while ( (c = getc( fp )) != EOF )
	{
		if ( state != COMMENT && c == '\\' )
		{
			if ( (c = getc(fp)) != EOF )
			{
				if ( c == '\'' || c == '"' )
				{
					putchar( '\\' );
					putchar( c );
					continue;
				}
				else
					putchar( '\\' );
			}
			else
			{
				putchar( '\\' );
				break;
			}
		}
		if ( state == CODE )
		{
			if ( c == '/' )
			{
				if ( (c = getc(fp)) != EOF )
				{
					if ( c == '*' )
						state = COMMENT;
					else
					{
						putchar( '/' );
						ungetc( c, fp );
					}
				}
				else
				{
					putchar( '/' );
					break;
				}
			}
			else if ( c == '"' )
			{
				state = STRING;
				putchar( c );
			}
			else if ( c == '\'' )
			{
				state = CHARSTR;
				putchar( c ) ;
			}
			else
				putchar( c );
		}
		else if ( state == STRING )
		{
			if ( c == '"' )
				state = CODE;
			putchar( c );
		}
		else if ( state == CHARSTR )
		{
			if ( c == '\'' )
				state = CODE;
			putchar( c );
		}
		else	/* state is comment */
		{
			if ( c == '*' )
			{
				if ( (c = getc(fp)) != EOF )
				{
					if ( c == '/' )
						state = CODE;
					else
						ungetc( c, fp );
				}
				else
					break;
			}
		}
	}
}

void
shstrip( fp )
	register FILE	*fp;
{
	/* strip comments in a shell file */
	/* ignores continuations on a : comment; to wit:
	 * : this is an example of a\
	 * continuation
	 * also ignores a : not on first char of line
	 */

	register int	c;
	register int	state = CODE;
	Bool	first = True;


	while ( (c = getc( fp )) != EOF )
	{
		switch ( c )
		{
		case '#':
			if ( state == CODE )
				state = COMMENT;
			break;
		case ':':
			if ( first )
				state = COMMENT;
			break;
		case '\n':
			if ( (c = getc(fp)) != EOF )
			{
				if ( c == ':' )
				{
					state = COMMENT;
					putchar('\n');
				}
				else
				{
					ungetc( c, fp );
					state = CODE;
					c = '\n';
				}
			}
			else
			{
				if ( state == COMMENT )
					putchar( '\n' );
				else
					c = '\n';
			}
			break;
		case '\\':
			/* things like \# do not start comments */
			if ( (c = getc(fp)) != EOF )
			{
				if ( c != '#' )
				{
					ungetc( c, fp );
					c = '\\';
				}
				else
					putchar( '\\' );
			}
			break;
		case '"':
			if ( state == STRING )
				state = CODE;
			else if ( state == CODE )
				state = STRING;
			/* if it is in a comment, just ignore it */
			break;
		case '\'':
			if ( state == CHARSTR )
				state = CODE;
			else if ( state == CODE )
				state = CHARSTR;
			/* if it is in a comment, just ignore it */
			break;
		default:
			break;
		}
		if ( state != COMMENT )
			putchar( c );
		first = False;
	}
}

void
poundstrip( fp )
	register FILE	*fp;
{
	/* strip of the form # ...comment... <NEWLINE> */

	register int	c;
	register int	state = CODE;
	int	old_state;

	while ( (c = getc( fp )) != EOF )
	{
		if ( c == '#' && state == CODE )
			state = COMMENT;
		else if ( c == '\n' )
			state = CODE;
		else if ( c == '\\' )
		{
			/* things like \# do not start comments */
			if ( (c = getc(fp)) != EOF )
			{
				if ( c != '#' )
					ungetc( c, fp );
				else
					putchar( '\\' );
			}
			else
				break;
		}
		else if ( c == '"' )
		{
			if ( state == STRING )
				state = CODE;
			else if ( state == CODE )
				state = STRING;
			/* if it is in a comment, just ignore it */
		}
		else if ( c == '\'' )
		{
			if ( state == CHARSTR )
				state = CODE;
			else if ( state == CODE )
				state = CHARSTR;
			/* if it is in a comment, just ignore it */
		}
		if ( state != COMMENT )
			putchar( c );
	}
}

void
vmenustrip( fp )
	register FILE	*fp;
{
	register int	c;
	/* this is real easy now, visual has no comments */

	while ( (c = getc( fp )) != EOF )
		putchar( c );
}
