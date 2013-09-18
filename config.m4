dnl $Id$
dnl config.m4 for extension gumbo

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(gumbo, for gumbo support,
[  --with-gumbo[[=DIR]]             Include gumbo support])

if test "$PHP_GUMBO" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-gumbo -> check with-path
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="/include/gumbo.h"  # you most likely want to change this
  if test -r $PHP_GUMBO/$SEARCH_FOR; then # path given as parameter
    GUMBO_DIR=$PHP_GUMBO
  else # search default path list
    AC_MSG_CHECKING([for gumbo files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        GUMBO_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  dnl
  if test -z "$GUMBO_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the gumbo distribution])
  fi

  # --with-gumbo -> add include path
  PHP_ADD_INCLUDE($GUMBO_DIR/include)

  # --with-gumbo -> check for lib and symbol presence
  LIBNAME=gumbo # you may want to change this
  LIBSYMBOL=gumbo_destroy_output # you most likely want to change this

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $GUMBO_DIR/lib, GUMBO_SHARED_LIBADD)
    AC_DEFINE(HAVE_GUMBOLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong gumbo lib version or lib not found])
  ],[
    -L$GUMBO_DIR/lib -lm
  ])
  dnl
  PHP_SUBST(GUMBO_SHARED_LIBADD)

  PHP_NEW_EXTENSION(gumbo, gumbo.c, $ext_shared)
fi
