AC_PREREQ(2.63)
AC_INIT(corewar, 1.0, none)
AM_INIT_AUTOMAKE(corewar, 1.0)

# Checks for programs.
AC_PROG_CC

AC_PROG_MAKE_SET


# Checks for libraries.
# FIXME: Replace `main' with a function in `-lX11':
AC_SEARCH_LIBS(XFlush, X11, [], [AC_MSG_ERROR([veuillez installez le packet: libx11-dev])])

# FIXME: Replace `main' with a function in `-lXpm':
AC_SEARCH_LIBS(XpmReadFileToPixmap, Xpm, [], [AC_MSG_ERROR([veuillez installez le packet: libxpm-dev])])

# Checks for header files.
AC_PATH_X
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([gettimeofday strrchr])


AC_CONFIG_FILES([Makefile 
				src/Makefile])
AC_OUTPUT
