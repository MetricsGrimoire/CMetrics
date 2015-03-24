AC_INIT([cmetrics],[0.2], [libresoft-tools-devel@lists.morfeo-project.org])
AM_INIT_AUTOMAKE([1.11])

AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_PROG_AWK
AM_PROG_LEX
AM_PATH_PYTHON([3])

case "$PYTHON_VERSION" in
   2.*)
	AC_MSG_ERROR([
		Python 2 not compatible with cmetrics.
		Set PYTHON to your Python 3.x binary.
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
