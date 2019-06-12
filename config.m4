dnl $Id$
dnl config.m4 for extension arraylist

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(arraylist, for arraylist support,
dnl Make sure that the comment is aligned:
dnl [  --with-arraylist             Include arraylist support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(arraylist, whether to enable arraylist support,
dnl Make sure that the comment is aligned:
[  --enable-arraylist      Enable arraylist support])

if test -z "$PHP_DEBUG"; then
    PHP_ARG_ENABLE(debug, whether to enable arraylist debug support,
    dnl Make sure that the comment is aligned:
[  --enable-debug          Enable arraylist debuging support],
    [PHP_DEBUG=$enableval],[PHP_DEBUG=no]
   )
fi

if test "$PHP_ARRAYLIST" != "no"; then
  dnl Write more examples of tests here...

  dnl # get library FOO build options from pkg-config output
  dnl AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  dnl AC_MSG_CHECKING(for libfoo)
  dnl if test -x "$PKG_CONFIG" && $PKG_CONFIG --exists foo; then
  dnl   if $PKG_CONFIG foo --atleast-version 1.2.3; then
  dnl     LIBFOO_CFLAGS=`$PKG_CONFIG foo --cflags`
  dnl     LIBFOO_LIBDIR=`$PKG_CONFIG foo --libs`
  dnl     LIBFOO_VERSON=`$PKG_CONFIG foo --modversion`
  dnl     AC_MSG_RESULT(from pkgconfig: version $LIBFOO_VERSON)
  dnl   else
  dnl     AC_MSG_ERROR(system libfoo is too old: version 1.2.3 required)
  dnl   fi
  dnl else
  dnl   AC_MSG_ERROR(pkg-config not found)
  dnl fi
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBDIR, ARRAYLIST_SHARED_LIBADD)
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)

  dnl # --with-arraylist -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/arraylist.h"  # you most likely want to change this
  dnl if test -r $PHP_ARRAYLIST/$SEARCH_FOR; then # path given as parameter
  dnl   ARRAYLIST_DIR=$PHP_ARRAYLIST
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for arraylist files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       ARRAYLIST_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$ARRAYLIST_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the arraylist distribution])
  dnl fi

  dnl # --with-arraylist -> add include path
  dnl PHP_ADD_INCLUDE($ARRAYLIST_DIR/include)

  dnl # --with-arraylist -> check for lib and symbol presence
  dnl LIBNAME=arraylist # you may want to change this
  dnl LIBSYMBOL=arraylist # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ARRAYLIST_DIR/$PHP_LIBDIR, ARRAYLIST_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_ARRAYLISTLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong arraylist lib version or lib not found])
  dnl ],[
  dnl   -L$ARRAYLIST_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  PHP_SUBST(ARRAYLIST_SHARED_LIBADD)

  PHP_NEW_EXTENSION(arraylist, arraylist.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
