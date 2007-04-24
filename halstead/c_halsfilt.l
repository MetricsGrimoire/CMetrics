/* this lex routine breaks up C programs into operators and operands	*/
/* operands go to stdout, operators go to stderr			*/
/* distinction is based on my reading of various articles and K&R	*/
/* this routine expects comments and strings to be stripped before	*/
%%
(\'.\'|\'\\.\'|\'\\[0-9]+\') {
			/* character constants go to operand output */
			printf(" %s ", yytext);
			}

(\-\>|\+\+|\-\-|\<\<|\>\>|\<\=|\>\=|\=\=|\!\=|\&\&|\|\||[+*/%\-]\=)	{
			/* print 2 character symbols to operator output */
			fprintf(stderr,"%s\n", yytext);
			putchar(' ');
		 	}

[\.\(\)\[\]\!\~\&\+\/\%\<\>\&\^\|\?\=\,\-] {
			/* print most 1 char symbols to operator output */
			fprintf(stderr,"%s\n", yytext);
			putchar(' ');
			}

\*/[^ \t\n0-9]		{
			/* print indirection operator to operator output */
			fprintf(stderr,"*indirection\n");
			putchar(' ');
			}

\*			{
			/* multiplication operator */
			fprintf(stderr,"%s\n", yytext);
			putchar(' ');
			}

[\:\;\{\}]		{
			/* delete delimiters */
			/* NOTE - prints to stdout, unlike above */
			putchar(' ');
			}

sizeof/([ \t\(]|$)	{
			/* sizeof operator to operator output */
			fprintf(stderr, "sizeof\n" );
			putchar(' ');
			}

^#[a-z]+/[ \t\n]	{
			/* preprocessor operators to operator output */
			fprintf(stderr, "%s\n", yytext);
			putchar(' ');
			}

(if|else|while|do|for|switch|case|default|break|continue|return|goto|case)/[:; \t\(\n] {
			/* statements */
			fprintf(stderr, "%s\n", yytext);
			putchar(' ');
			}

(auto|static|extern|register|typedef)/[ \t\(\n] {
			/* storage class specifiers */
			fprintf(stderr, "%s\n", yytext);
			putchar(' ');
			}

(char|short|int|long|unsigned|float|double)/[ \t\(\)\n] {
			/* type specifier */
			fprintf(stderr, "%s\n", yytext);
			putchar(' ');
			}

(struct|union)/[ \t\(\n]		{
			/* struct or union specifier */
			fprintf(stderr, "%s\n", yytext);
			putchar(' ');
			}

[a-zA-Z_][a-zA-Z0-9_]*	{
			/* catch operand names */
			printf(" %s ", yytext);
			}

.			ECHO;
%%
