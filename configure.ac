#******************************************************************************
#*                                                                            *
#* Copyright (c) 2016, Tsung-Wei Huang and Martin D. F. Wong,                 *
#* University of Illinois at Urbana-Champaign (UIUC), IL, USA.                *
#*                                                                            *
#* All Rights Reserved.                                                       *
#*                                                                            *
#* This program is free software. You can redistribute and/or modify          *
#* it in accordance with the temms of the accompanying license agreement.     *
#* See LICENSE in the top-level directory for details.                        *
#*                                                                            *
#******************************************************************************/

# Force autoconf to be at least this version number:
#AC_PREREQ([2.68])

# AC_INIT([package], [version], [bug-report], [tarname], [url])
AC_INIT([DtCraft], [0.2.1], [twh760812@gmail.com],
        [DtCraft], [http://dtcraft.web.engr.illinois.edu/])

# Project-specific macros.
AC_DEFINE_UNQUOTED([DTC_HOME], ["`pwd`"], ["Home directory of DtCraft"])

# Safety check - list a source file that wouldn't be in other directories:
#AC_CONFIG_SRCDIR([src/hello.c])

# Put autotools auxiliary files in a subdir, so they don't clutter top dir:
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

# Check for which host we are on and setup a few host-specific things
AC_CANONICAL_HOST
case $host_os in
  *linux*)
  # Do something specific for linux
  ;;
  *)
  #Default Case
  AC_MSG_ERROR([
-------------------------------------------------------------------------------
Linux platform is required.
-------------------------------------------------------------------------------])
  ;;
esac


# Substitute variables.
AC_SUBST(TEST_LIBS)

# Enable "automake" to simplify creating makefiles:
AM_INIT_AUTOMAKE([subdir-objects tar-ustar])

# Enable "libtool" to simplify creating makefiles:
#LT_PREREQ([2.2])
LT_INIT
LT_LANG([C++])


# Specify where the Makefile should be produced. 
# These are the same locations as your Makefile.in's, but named as Makefile only.
# We need to do this because both Autoconf and Automake read this file and produce 
# Makefiles from this list.
AC_CONFIG_FILES([Makefile])
AC_CONFIG_HEADERS([include/dtc/config.hpp])

# Language configuration.
AC_LANG(C++)

# Program configuration.
AC_PROG_CXX
AC_PROG_YACC
AC_PROG_AWK
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S

# Check for the compiler
AX_COMPILER_VENDOR
AX_COMPILER_VERSION

AS_CASE($ax_cv_cxx_compiler_vendor,
  [gnu], [
    AX_COMPARE_VERSION(
      [$ax_cv_cxx_compiler_version], 
      [ge], 
      [7.0],
      [is_ge_gxx_7_0=yes], 
      [is_ge_gxx_7_0=no]
    )
    if [[ $is_ge_gxx_7_0 != "yes" ]]; then
      AC_MSG_ERROR(
        [GCC 7 (with c++17) or higher required (found $ax_cv_cxx_compiler_version)]
      )
    fi
  ], [
    AC_MSG_ERROR([$ax_cv_cxx_compiler_vendor is an unsupported compiler])
  ]
)


# Library, header check, so we can easily #include them:
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h])
AC_CHECK_HEADERS([sys/devpoll.h])
AC_CHECK_HEADERS([sys/epoll.h]) 
AC_CHECK_HEADERS([sys/poll.h]) 
AC_CHECK_HEADERS([sys/select.h]) 
AC_CHECK_HEADERS([sys/time.h]) 
AC_CHECK_HEADERS([sys/event.h]) 
AC_CHECK_HEADERS([sys/eventfd.h]) 
AC_CHECK_HEADERS([sys/signalfd.h]) 
AC_CHECK_HEADERS([sys/stat.h])

# Type check.
AC_CHECK_TYPES([fd_mask], , ,)

# Func check.
AC_CHECK_FUNCS(inotify_init)
AC_CHECK_FUNCS(epoll_ctl)
AC_CHECK_FUNCS(kqueue)
AC_CHECK_FUNCS(port_create)
AC_CHECK_FUNCS(poll) 
AC_CHECK_FUNCS(select) 
AC_CHECK_FUNCS(eventfd) 
AC_CHECK_FUNCS(signalfd)
AC_CHECK_FUNCS(getloadavg)

# Check for 3rd-party libraries.
#AC_SEARCH_LIBS([shm_open], [rt], [], [AC_MSG_ERROR([shm_open not found in librt])])
AX_PTHREAD([have_pthread=yes], [AC_MSG_ERROR([pthread library is required])])

#AX_LEVELDB(
#  [],
#  [],
#  [AC_MSG_ERROR([
#-------------------------------------------------------------------------------
#LevelDB is required 
#Please download the package at https://github.com/google/leveldb
#-------------------------------------------------------------------------------])]
#)

#AX_GTEST(
#  [],
#  [],
#  [AC_MSG_ERROR([
#-------------------------------------------------------------------------------
#gtest is required 
#
#Please use the following instructions to install the google test.
#
#1. Go to the google test framework
#
#  $ wget https://github.com/google/googletest/archive/release-1.7.0.tar.gz
#
#2. Unpack and build google test
#
#  $ tar xf release-1.7.0.tar.gz
#  $ cd googletest-release-1.7.0
#  $ cmake -DBUILD_SHARED_LIBS=ON .
#  $ make
#
#3. "Install" the headers and libs on your system.
#
#  $ sudo cp -a include/gtest /usr/include
#  $ sudo cp -a libgtest_main.so libgtest.so /usr/lib/
#
#4. Update the cache of the linker
#
#  $ sudo ldconfig -v | grep gtest
#-------------------------------------------------------------------------------])]
#)

#AX_LIBSTATGRAB(
#  [], 
#  [], 
#  [AC_MSG_ERROR([
#-------------------------------------------------------------------------------
#Libstatgrab is required 
#Please download the package at https://www.i-scream.org/libstatgrab/
#-------------------------------------------------------------------------------])]
#)

#BOOST_REQUIRE(
#  [1.5], 
#  [AC_MSG_ERROR([
#-------------------------------------------------------------------------------
#Boost library > 1.5 is required
#Please download the package at http://www.boost.org/
#-------------------------------------------------------------------------------])]
#)
#
#BOOST_VARIANT

# Do final output.
AC_OUTPUT
LT_OUTPUT


