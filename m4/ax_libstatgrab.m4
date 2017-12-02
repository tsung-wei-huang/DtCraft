AC_DEFUN([AX_LIBSTATGRAB], [

AC_CHECK_HEADERS(
  [statgrab.h],
  [AC_SEARCH_LIBS(
    [sg_init], 
    [statgrab],
    [have_libstatgrab=yes], 
    []
  )],
  [have_libstatgrab=no]
)

if [[ "$have_libstatgrab" = "yes" ]]; then
  m4_default([$2], [AC_MSG_NOTICE([libstatgrab is found])])
else
  m4_default([$3], [AC_MSG_ERROR([libstatgrab is not found])])
fi

])
