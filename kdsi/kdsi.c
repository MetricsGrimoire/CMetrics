/* counts number of statements, lines of code, comments, comment lines
 * and blank lines
 */
/***************************************************************************
* kdsi is written by Brian Renaud, is in the public domain, and            *
* may be used by any person or organization, in any way and for any        *
* purpose.                                                                 *
*                                                                          *
* There is no warranty of merchantability nor any warranty of fitness for  *
* a particular purpose nor any other warranty, either express or implied,  *
* as to the accuracy of the enclosed materials or as to their suitability  *
* for any particular purpose.  Accordingly, I assume no responsibility     *
* for their use by the recipient.  Further, I assume no obligation to      *
* furnish any assistance of any kind whatsoever, or to furnish any         *
* additional information or documentation.                                 *
*                                                                          *
* address as of July 1988: bdr%huron.uucp@umix.cc.umich.edu also known as: *
*                          {ames, apollo, rutgers, uunet}!umix!huron!bdr   *
***************************************************************************/

#include <stdio.h>

typedef	int	Token;
#define	STOP_INPUT	0
#define	NEWLINE	1
#define START_COMMENT	2
#define END_COMMENT	3
#define	MISC_CHARACTER	4
#define WHITE_SPACE	5

typedef char	Bool;
#define True	1
#define False	0

typedef int	State;
#define	Code	0
#define Comment	1
#define Quiescent	2

#define FNULL	( (FILE *) 0)
#define CNULL	( (char *) 0)

Bool	only_stdin = False;		/* true if reading from stdin */

main(argc, argv)
	int	argc;
	char	*argv[];
{
	Token	GetChar();
	FILE	*nextfp();
	register Token	input;
	register State	statevar = Quiescent, laststate = Quiescent;
	FILE	*fp;
	char	*filename;
	int	filecount = 0;
	long	cod_linect, com_linect, blnk_linect, comment_ct;
	long	tot_cdline, tot_cmline, tot_bkline, tot_comment;
	Bool	following_com = False;

	tot_cdline = tot_cmline = tot_bkline = tot_comment = 0;
	while ( (fp = nextfp(argc, argv, &filename)) != FNULL )
	{
		cod_linect = com_linect = blnk_linect = comment_ct = 0;
		filecount++;

		while ( (input = GetChar(fp)) != STOP_INPUT )
		{
			switch ( input )
			{
			case NEWLINE:
				if ( statevar == Code )
					cod_linect++;
				else if ( statevar == Comment )
					com_linect++;
				/* state is quiescent */
				else if ( laststate == Comment )
				{
					/* if is supposed to catch cases where a comment
					 * follows a line of code
					 */
					if ( following_com )
						cod_linect++;
					else
						com_linect++;
				}
				else
					blnk_linect++;
				if ( statevar != Comment )
				{
					laststate = Quiescent;
					statevar = Quiescent;
				}
				following_com = False;
				break;
			case START_COMMENT:
				laststate = statevar;
				statevar = Comment;
				break;
			case END_COMMENT:
				comment_ct++;
					/* if true, is a comment on same line as code */
				if ( laststate == Code )
					following_com = True;

				laststate = Comment;
				statevar = Quiescent;
				break;
			case MISC_CHARACTER:
				if ( statevar == Quiescent )
				{
					laststate = statevar;
					statevar = Code;
				}
				break;
			default:
				fprintf(stderr, "kdsi: illegal token (%d) returned from GetChar\n", input);
				exit(1);
				break;

			}
		}
		if ( !only_stdin )
			printf("%8ld %8ld %8ld %7ld  %s\n",
				cod_linect, blnk_linect, com_linect, comment_ct,
				filename);
		else
			printf("%8ld %8ld %8ld %7ld\n",
				cod_linect, blnk_linect, com_linect, comment_ct);
		tot_cdline += cod_linect;
		tot_cmline += com_linect;
		tot_bkline += blnk_linect;
		tot_comment += comment_ct;
	}
	if ( !only_stdin && filecount > 1 )
		printf("%8ld %8ld %8ld %7ld  total\n",
			tot_cdline, tot_bkline, tot_cmline, tot_comment);
	exit(0);
}

Token
GetChar( file )
	FILE	*file;
{
	/* return token for char type, taking into account comment delims */
	/* ignores spaces and tabs */

	register int	c;
	register Token	retval;
	static int	buf;
	static Bool	inbuf = False;

	do
	{
		if ( inbuf )
		{
			c = buf;
			inbuf = False;
		}
		else
			c = getc(file);
		
		switch ( c )
		{
		case EOF:
			retval = STOP_INPUT;
			break;
		case '\n':
			retval = NEWLINE;
			break;
		case '/':
			buf = getc( file );
			if ( buf == '*' )
				retval = START_COMMENT;
			else
			{
				inbuf = True;
				retval = MISC_CHARACTER;
			}
			break;
		case '*':
			buf = getc( file );
			if ( buf == '/' )
				retval = END_COMMENT;
			else
			{
				inbuf = True;
				retval = MISC_CHARACTER;
			}
			break;
		case ' ':
		case '\t':
			retval = WHITE_SPACE;
			break;
		default:
			retval = MISC_CHARACTER;
		}
	}
	while ( retval == WHITE_SPACE );

	return (retval);
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
		{
			result = stdin;
			only_stdin = True;
		}
		first = False;
	}
	return ( result );
}
