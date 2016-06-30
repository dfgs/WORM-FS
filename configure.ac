AC_INIT([WORM], [1.0], [dfgs83@gmail.com])
CFLAGS="$CFLAGS -D_FILE_OFFSET_BITS=64"
AC_CHECK_LIB(fuse,fuse_main, ,[AC_MSG_ERROR([Couldn't find libfuse...] )])
AC_CHECK_HEADER(fuse.h,, [AC_MSG_ERROR([Couldn't find fuse.h...] )])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_CONFIG_FILES([Makefile])
AC_OUTPUT