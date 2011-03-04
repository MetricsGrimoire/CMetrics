AC_INIT([cmetrics],[0.11], [libresoft-tools-devel@lists.morfeo-project.org])
AM_INIT_AUTOMAKE([1.11])

AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_PROG_AWK
AM_PROG_LEX
AM_PATH_PYTHON([2.7])

case "$PYTHON_VERSION" in
   3.0*|3.1*|3.2*)
	AC_MSG_ERROR([
		Python 3 not compatible with cmetrics.
		Set PYTHON to your Python 2.x binary.
		]);;
esac

AC_CONFIG_FILES([
Makefile 
mccabe/Makefile 
halstead/Makefile 
kdsi/Makefile 
utils/Makefile
])

AC_OUTPUT
