#!/bin/bash

# Predefined variables.
#echo "# Predefine variables."
#echo "EIGEN_HOME=3rd-party/eigen-eigen-5a0156e40feb"
#echo "EIGEN_INC=-I\$(EIGEN_HOME)"
#echo ""

# Automake options.
echo "# Automake options."
echo "AUTOMAKE_OPTIONS = foreign"
echo ""

# Make sure that when we re-make ./configure, we get the macros we need.
echo "# Make sure that when we re-make ./configure, we get the macros we need."
echo "ACLOCAL_AMFLAGS = -I m4 --install"
echo ""

# Automake directories.
echo "# Automake directories."
echo "SUBDIRS  ="
echo "SUBDIRS += ."
echo ""

# Package-related substitution variables.
echo "# Package-related substitution variables."
echo "CXX = @CXX@"
echo "CPPFLAGS = @CPPFLAGS@"
echo "CXXFLAGS = @CXXFLAGS@"
echo "LIBS = @LIBS@"
echo "TEST_LIBS = @TEST_LIBS@"
echo "DEFS = @DEFS@"
echo "CPU_COUNT = @CPU_COUNT@"
echo ""

# Initialize variables here so we can use += operator everywhere else.
echo "# Initialize variables here so we can use += operator everywhere else."
echo "lib_LTLIBRARIES ="
echo "noinst_LTLIBRARIES ="
echo "sbin_PROGRAMS ="
echo "bin_PROGRAMS ="
echo "noinst_PROGRAMS ="
echo "pkglibexec_PROGRAMS ="
echo "include_HEADERS ="
echo "pkginclude_HEADERS ="
echo "nobase_include_HEADERS ="
echo "nobase_pkginclude_HEADERS ="
echo "dist_bin_SCRIPTS ="
echo "dist_pkglibexec_SCRIPTS ="
echo "nobase_dist_pkgdata_DATA ="
echo "nodist_sbin_SCRIPTS ="
echo "check_PROGRAMS ="
echo "dist_check_SCRIPTS ="
echo "check_SCRIPTS ="
echo "BUILT_SOURCES ="
echo "CLEANFILES ="
echo "EXTRA_DIST ="
echo "PHONY_TARGETS ="
echo "LDADD ="
echo "TESTS ="
echo ""

# Libraries.
echo "# Libraries"
echo "LIBS += -lstdc++fs"
echo "LIBS += \$(PTHREAD_LIBS)"
echo ""

# CXXFLAGS
echo "# CXX flags"
echo "CXXFLAGS += -std=c++1z"
echo "CXXFLAGS += -Wall"
echo "CXXFLAGS += \$(PTHREAD_CFLAGS)"
echo ""

## DtCraft 3rd-party eigen include flag
#echo "# DtCraft 3rd-party eigen include flag"
#echo "CXXFLAGS += \$(EIGEN_INC)"
#echo ""

# DtCraft package include.
echo "# DtCraft Package include"
echo "CPPFLAGS += -Iinclude"
#for f in `find include -name *.hpp`
for f in `find include -type f`
do
  echo "nobase_pkginclude_HEADERS += $f"
done
echo ""

# Library definition: lib/libDtCraft.la
echo "# Library definition: lib/libDtCraft.la"
echo "lib_LTLIBRARIES           += lib/libDtCraft.la"
echo "lib_libDtCraft_la_SOURCES  ="
for f in `find src -name *.cpp`
do
  echo "lib_libDtCraft_la_SOURCES += $f"
done
echo ""

#### Bin binaries.
echo "#### DtCraft binaries ####"
echo""

for f in `find main -name *.cpp`
do
  filename=`echo $(basename "$f" .cpp) | sed -e 's/-/_/g'`
  echo "# Program: $f"
  echo "noinst_PROGRAMS += bin/$(basename "$f" .cpp)"
  echo "bin_${filename}_SOURCES  = $f"
  echo "bin_${filename}_LDADD    = lib/libDtCraft.la"
  echo ""
done

#### Example binaries.
echo "#### Examples ####"
echo ""

for f in `find example -name *.cpp`
do
  filename=$(basename "$f" .cpp)
  echo "# Program: $f"
  echo "noinst_PROGRAMS += example/$filename"
  echo "example_${filename}_SOURCES  = $f"
  echo "example_${filename}_LDADD    = lib/libDtCraft.la"
  echo ""
done

#### App binaries.
echo "#### App ####"
echo ""

for app in app/*/
do
  filename=$(basename "$app")
  echo "# Application $app$filename"
  prefix=`echo "$app$filename" | tr / _`
  echo "noinst_PROGRAMS += $app$filename"
  #echo "${prefix}_CPPFLAGS = \$(CPPFLAGS) -I$app"
  echo "${prefix}_LDADD ="
  echo "${prefix}_LDADD += lib/libDtCraft.la"
  echo "${prefix}_SOURCES ="
  for f in `find $app -name *.cpp -o -name *.hpp`
  do
    echo "${prefix}_SOURCES += $f"
  done

  echo ""
done

#### Testing binaries.
echo "#### Unittest ####"
echo ""

for f in `find unittest -name *.cpp`
do
  filename=$(basename "$f" .cpp)
  echo "# Program: unittest/${filename}"
  echo "noinst_PROGRAMS += unittest/${filename}"
  echo "unittest_${filename}_LDADD     = lib/libDtCraft.la"
  echo "unittest_${filename}_LDADD    += \$(TEST_LIBS)"
  echo "unittest_${filename}_SOURCES   = ${f}"
  echo -en "#!/bin/bash\n\ntimeout 5m ./unittest/${filename} -d yes" > unittest/${filename}.sh
  chmod 755 unittest/${filename}.sh
  #echo "TESTS += unittest/${filename}"
  echo "TESTS += unittest/${filename}.sh"
  echo ""
done


# Extra distribution.
echo "# Add files to the distribution list"
echo "EXTRA_DIST += Makefile.sh"
echo "EXTRA_DIST += conf"
echo "EXTRA_DIST += sbin"
echo "EXTRA_DIST += webui"
echo "EXTRA_DIST += benchmark"
echo "EXTRA_DIST += README.md"
echo "EXTRA_DIST += LICENSE"
echo "EXTRA_DIST += logo.jpg"
#echo "EXTRA_DIST += 3rd-party"
echo ""

# Regression target.
echo "# Regression "
echo "regression: all-am"
echo "	@for t in \$(TESTS); do \$\$t -d yes; done"
echo ""

# Clean hook
echo "# Clean hook"
echo "clean-local:"
echo "	@find . -name ._\\* -delete"
echo ""

# Project-specific targets
echo "# Project-specific targets"
echo "echo_CXX:"
echo "	@echo \$(CXX)"
echo ""

echo "echo_CPPFLAGS:"
echo "	@echo \$(CPPFLAGS)"
echo ""

echo "echo_CXXFLAGS:"
echo "	@echo \$(CXXFLAGS)"
echo ""

echo "echo_LIBS:"
echo "	@echo \$(LIBS)"
echo ""

echo "echo_TEST_LIBS:"
echo "	@echo \$(TEST_LIBS)"
echo ""

echo "echo_DEFS:"
echo "	@echo \$(DEFS)"
echo ""

echo "echo_bin_PROGRAMS:"
echo "	@echo \$(bin_PROGRAMS)"
echo ""

echo "echo_sbin_PROGRAMS:"
echo "	@echo \$(sbin_PROGRAMS)"
echo ""

echo "echo_check_PROGRAMS:"
echo "	@echo \$(check_PROGRAMS)"
echo ""

echo "echo_check_SCRIPTS:"
echo "	@echo \$(check_SCRIPTS)"
echo ""

echo "echo_lib_LTLIBRARIES:"
echo "	@echo \$(lib_LTLIBRARIES)"
echo ""

echo "echo_prefix:"
echo "	@echo \$(prefix)"
echo ""

echo "echo_builddir:"
echo "	@echo \$(builddir)"
echo ""

echo "echo_abs_builddir:"
echo "	@echo \$(abs_builddir)"
echo ""

echo "echo_top_builddir:"
echo "	@echo \$(top_builddir)"
echo ""

echo "echo_abs_top_builddir:"
echo "	@echo \$(abs_top_builddir)"
echo ""

echo "echo_top_build_prefix:"
echo "	@echo \$(top_build_prefix)"
echo ""

echo "echo_srcdir:"
echo "	@echo \$(srcdir)"
echo ""

echo "echo_abs_srcdir:"
echo "	@echo \$(abs_srcdir)"
echo ""

echo "echo_top_srcdir:"
echo "	@echo \$(top_srcdir)"
echo ""

echo "echo_abs_top_srcdir:"
echo "	@echo \$(abs_top_srcdir)"
echo ""

echo "echo_bindir:"
echo "	@echo \$(bindir)"
echo ""

echo "echo_sbindir:"
echo "	@echo \$(sbindir)"
echo ""

echo "echo_docdir:"
echo "	@echo \$(docdir)"
echo ""

echo "echo_exec_prefix:"
echo "	@echo \$(exec_prefix)"
echo ""

echo "echo_libdir:"
echo "	@echo \$(libdir)"
echo ""

echo "echo_pkglibdir:"
echo "	@echo \$(pkglibdir)"
echo ""

echo "echo_includedir:"
echo "	@echo \$(includedir)"
echo ""

echo "echo_pkgincludedir:"
echo "	@echo \$(pkgincludedir)"
echo ""

echo "echo_libexecdir:"
echo "	@echo \$(libexecdir)"
echo ""

echo "echo_datadir:"
echo "	@echo \$(datadir)"
echo ""

echo "echo_OMP_CFLAGS:"
echo "	@echo \$(OPENMP_CFLAGS)"
echo ""

echo "echo_OMP_CXXFLAGS:"
echo "	@echo \$(OPENMP_CXXFLAGS)"
echo ""

echo "echo_BOOST_CPPFLAGS:"
echo "	@echo \$(BOOST_CPPFLAGS)"
echo ""

echo "echo_PYTHON_CPPFLAGS:"
echo "	@echo \$(PYTHON_CPPFLAGS)"
echo ""

echo "echo_PYTHON_LDFLAGS:"
echo "	@echo \$(PYTHON_LDFLAGS)"
echo ""

echo "echo_CPU_COUNT:"
echo "	@echo \$(CPU_COUNT)"
echo ""




