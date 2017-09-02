AC_INIT([worm], [1.0], [dfgs83@gmail.com])
CFLAGS="$CFLAGS -D_FILE_OFFSET_BITS=64"

AC_CHECK_LIB(fuse,fuse_main, ,[AC_MSG_ERROR([Couldn't find libfuse...] )])
AC_CHECK_LIB(rt,mq_open, ,[AC_MSG_ERROR([Couldn't find librt...] )])
AC_CHECK_LIB(ini,ini_open, ,[AC_MSG_ERROR([Couldn't find libini...] )])
AC_CHECK_LIB(log,log_open, ,[AC_MSG_ERROR([Couldn't find liblog...] )])

AC_CHECK_HEADER(fuse.h,, [AC_MSG_ERROR([Couldn't find fuse.h...] )])
AC_CHECK_HEADER(mqueue.h,, [AC_MSG_ERROR([Couldn't find mqueue.h...] )])
AC_CHECK_HEADER(ini.h,, [AC_MSG_ERROR([Couldn't find ini.h...] )])
AC_CHECK_HEADER(log.h,, [AC_MSG_ERROR([Couldn't find log.h...] )])

AM_INIT_AUTOMAKE
AC_PROG_CC
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
