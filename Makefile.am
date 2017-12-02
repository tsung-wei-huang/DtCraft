# Automake options.
AUTOMAKE_OPTIONS = foreign

# Make sure that when we re-make ./configure, we get the macros we need.
ACLOCAL_AMFLAGS = -I m4 --install

# Automake directories.
SUBDIRS  =
SUBDIRS += .

# Package-related substitution variables.
CXX = @CXX@
CPPFLAGS = @CPPFLAGS@
CXXFLAGS = @CXXFLAGS@
LIBS = @LIBS@
TEST_LIBS = @TEST_LIBS@
DEFS = @DEFS@

# Initialize variables here so we can use += operator everywhere else.
lib_LTLIBRARIES =
noinst_LTLIBRARIES =
sbin_PROGRAMS =
bin_PROGRAMS =
noinst_PROGRAMS =
pkglibexec_PROGRAMS =
include_HEADERS =
pkginclude_HEADERS =
nobase_include_HEADERS =
nobase_pkginclude_HEADERS =
dist_bin_SCRIPTS =
dist_pkglibexec_SCRIPTS =
nobase_dist_pkgdata_DATA =
nodist_sbin_SCRIPTS =
check_PROGRAMS =
dist_check_SCRIPTS =
check_SCRIPTS =
BUILT_SOURCES =
CLEANFILES =
EXTRA_DIST =
PHONY_TARGETS =
LDADD =
TESTS =

# Libraries
LIBS += -lstdc++fs
LIBS += $(PTHREAD_LIBS)

# CXX flags
CXXFLAGS += -std=c++1z
CXXFLAGS += -Wall
CXXFLAGS += $(PTHREAD_CFLAGS)

# Preprocessor directive
CPPFLAGS += $(BOOST_CPPFLAGS)

# DtCraft Package include
CPPFLAGS += -Iinclude
nobase_pkginclude_HEADERS += include/dtc/archive/binary.hpp
nobase_pkginclude_HEADERS += include/dtc/archive/archive.hpp
nobase_pkginclude_HEADERS += include/dtc/statgrab/statgrab.hpp
nobase_pkginclude_HEADERS += include/dtc/error.hpp
nobase_pkginclude_HEADERS += include/dtc/traits.hpp
nobase_pkginclude_HEADERS += include/dtc/event/reactor.hpp
nobase_pkginclude_HEADERS += include/dtc/event/select.hpp
nobase_pkginclude_HEADERS += include/dtc/event/demux.hpp
nobase_pkginclude_HEADERS += include/dtc/event/event.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/streambuf.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/device.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/domain.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/fifo.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/ipc.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/shm.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/ios.hpp
nobase_pkginclude_HEADERS += include/dtc/ipc/socket.hpp
nobase_pkginclude_HEADERS += include/dtc/headerdef.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/os.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/allocator.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/table.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/random.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/mempool.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/http_parser.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/hash.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/lambda.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/literals.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/singleton.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/json.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/chrono.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/uuid.hpp
nobase_pkginclude_HEADERS += include/dtc/utility/logger.hpp
nobase_pkginclude_HEADERS += include/dtc/unittest/catch.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/synchronized.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/concurrent.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/deque.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/mutex.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/fifo.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/threadpool.hpp
nobase_pkginclude_HEADERS += include/dtc/concurrent/queue.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/solution.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/common.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/loadinfo.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/killtask.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/taskinfo.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/protobuf.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/topology.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/resource.hpp
nobase_pkginclude_HEADERS += include/dtc/protobuf/frontier.hpp
nobase_pkginclude_HEADERS += include/dtc/dtc.hpp
nobase_pkginclude_HEADERS += include/dtc/macrodef.hpp
nobase_pkginclude_HEADERS += include/dtc/utility.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/executor.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/manager.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/graph.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/scheduler.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/master.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/container.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/agent.hpp
nobase_pkginclude_HEADERS += include/dtc/kernel/cgroup.hpp
nobase_pkginclude_HEADERS += include/dtc/policy.hpp
nobase_pkginclude_HEADERS += include/dtc/webui/webui.hpp

# Library definition: lib/libDtCraft.la
lib_LTLIBRARIES           += lib/libDtCraft.la
lib_libDtCraft_la_SOURCES  =
lib_libDtCraft_la_SOURCES += src/statgrab/statgrab.cpp
lib_libDtCraft_la_SOURCES += src/event/reactor.cpp
lib_libDtCraft_la_SOURCES += src/event/event.cpp
lib_libDtCraft_la_SOURCES += src/event/select.cpp
lib_libDtCraft_la_SOURCES += src/ipc/device.cpp
lib_libDtCraft_la_SOURCES += src/ipc/socket.cpp
lib_libDtCraft_la_SOURCES += src/ipc/ipc.cpp
lib_libDtCraft_la_SOURCES += src/ipc/domain.cpp
lib_libDtCraft_la_SOURCES += src/ipc/fifo.cpp
lib_libDtCraft_la_SOURCES += src/ipc/shm.cpp
lib_libDtCraft_la_SOURCES += src/ipc/streambuf.cpp
lib_libDtCraft_la_SOURCES += src/utility/os.cpp
lib_libDtCraft_la_SOURCES += src/utility/http_parser.cpp
lib_libDtCraft_la_SOURCES += src/utility/table.cpp
lib_libDtCraft_la_SOURCES += src/error.cpp
lib_libDtCraft_la_SOURCES += src/policy.cpp
lib_libDtCraft_la_SOURCES += src/protobuf/resource.cpp
lib_libDtCraft_la_SOURCES += src/protobuf/frontier.cpp
lib_libDtCraft_la_SOURCES += src/protobuf/taskinfo.cpp
lib_libDtCraft_la_SOURCES += src/protobuf/protobuf.cpp
lib_libDtCraft_la_SOURCES += src/protobuf/solution.cpp
lib_libDtCraft_la_SOURCES += src/protobuf/common.cpp
lib_libDtCraft_la_SOURCES += src/protobuf/topology.cpp
lib_libDtCraft_la_SOURCES += src/kernel/container.cpp
lib_libDtCraft_la_SOURCES += src/kernel/cgroup.cpp
lib_libDtCraft_la_SOURCES += src/kernel/manager.cpp
lib_libDtCraft_la_SOURCES += src/kernel/scheduler.cpp
lib_libDtCraft_la_SOURCES += src/kernel/graph.cpp
lib_libDtCraft_la_SOURCES += src/kernel/master.cpp
lib_libDtCraft_la_SOURCES += src/kernel/agent.cpp
lib_libDtCraft_la_SOURCES += src/kernel/executor.cpp
lib_libDtCraft_la_SOURCES += src/webui/webui.cpp

#### DtCraft binaries ####

# Program: main/master.cpp
noinst_PROGRAMS += bin/master
bin_master_SOURCES  = main/master.cpp
bin_master_LDADD    = lib/libDtCraft.la

# Program: main/agent.cpp
noinst_PROGRAMS += bin/agent
bin_agent_SOURCES  = main/agent.cpp
bin_agent_LDADD    = lib/libDtCraft.la

#### Examples ####

# Program: example/pi.cpp
noinst_PROGRAMS += example/pi
example_pi_SOURCES  = example/pi.cpp
example_pi_LDADD    = lib/libDtCraft.la

# Program: example/hello_world.cpp
noinst_PROGRAMS += example/hello_world
example_hello_world_SOURCES  = example/hello_world.cpp
example_hello_world_LDADD    = lib/libDtCraft.la

# Program: example/kmeans.cpp
noinst_PROGRAMS += example/kmeans
example_kmeans_SOURCES  = example/kmeans.cpp
example_kmeans_LDADD    = lib/libDtCraft.la

# Program: example/prime.cpp
noinst_PROGRAMS += example/prime
example_prime_SOURCES  = example/prime.cpp
example_prime_LDADD    = lib/libDtCraft.la

# Program: example/reduce_sum.cpp
noinst_PROGRAMS += example/reduce_sum
example_reduce_sum_SOURCES  = example/reduce_sum.cpp
example_reduce_sum_LDADD    = lib/libDtCraft.la

#### Unittest ####

# Program: unittest/archive
noinst_PROGRAMS += unittest/archive
unittest_archive_LDADD     = lib/libDtCraft.la
unittest_archive_LDADD    += $(TEST_LIBS)
unittest_archive_SOURCES   = unittest/archive.cpp
TESTS += unittest/archive.sh

# Program: unittest/statgrab
noinst_PROGRAMS += unittest/statgrab
unittest_statgrab_LDADD     = lib/libDtCraft.la
unittest_statgrab_LDADD    += $(TEST_LIBS)
unittest_statgrab_SOURCES   = unittest/statgrab.cpp
TESTS += unittest/statgrab.sh

# Program: unittest/webui
noinst_PROGRAMS += unittest/webui
unittest_webui_LDADD     = lib/libDtCraft.la
unittest_webui_LDADD    += $(TEST_LIBS)
unittest_webui_SOURCES   = unittest/webui.cpp
TESTS += unittest/webui.sh

# Program: unittest/ipc
noinst_PROGRAMS += unittest/ipc
unittest_ipc_LDADD     = lib/libDtCraft.la
unittest_ipc_LDADD    += $(TEST_LIBS)
unittest_ipc_SOURCES   = unittest/ipc.cpp
TESTS += unittest/ipc.sh

# Program: unittest/reactor
noinst_PROGRAMS += unittest/reactor
unittest_reactor_LDADD     = lib/libDtCraft.la
unittest_reactor_LDADD    += $(TEST_LIBS)
unittest_reactor_SOURCES   = unittest/reactor.cpp
TESTS += unittest/reactor.sh

# Program: unittest/utility
noinst_PROGRAMS += unittest/utility
unittest_utility_LDADD     = lib/libDtCraft.la
unittest_utility_LDADD    += $(TEST_LIBS)
unittest_utility_SOURCES   = unittest/utility.cpp
TESTS += unittest/utility.sh

# Program: unittest/traits
noinst_PROGRAMS += unittest/traits
unittest_traits_LDADD     = lib/libDtCraft.la
unittest_traits_LDADD    += $(TEST_LIBS)
unittest_traits_SOURCES   = unittest/traits.cpp
TESTS += unittest/traits.sh

# Program: unittest/scheduler
noinst_PROGRAMS += unittest/scheduler
unittest_scheduler_LDADD     = lib/libDtCraft.la
unittest_scheduler_LDADD    += $(TEST_LIBS)
unittest_scheduler_SOURCES   = unittest/scheduler.cpp
TESTS += unittest/scheduler.sh

# Program: unittest/concurrent
noinst_PROGRAMS += unittest/concurrent
unittest_concurrent_LDADD     = lib/libDtCraft.la
unittest_concurrent_LDADD    += $(TEST_LIBS)
unittest_concurrent_SOURCES   = unittest/concurrent.cpp
TESTS += unittest/concurrent.sh

# Add files to the distribution list
EXTRA_DIST += Makefile.sh
EXTRA_DIST += conf
EXTRA_DIST += sbin
EXTRA_DIST += webui

# Regression 
regression: all-am
	@for t in $(TESTS); do $$t -d yes; done

# Clean hook
clean-local:
	@find . -name ._\* -delete

# Project-specific targets
echo_CXX:
	@echo $(CXX)

echo_CPPFLAGS:
	@echo $(CPPFLAGS)

echo_CXXFLAGS:
	@echo $(CXXFLAGS)

echo_LIBS:
	@echo $(LIBS)

echo_TEST_LIBS:
	@echo $(TEST_LIBS)

echo_DEFS:
	@echo $(DEFS)

echo_bin_PROGRAMS:
	@echo $(bin_PROGRAMS)

echo_sbin_PROGRAMS:
	@echo $(sbin_PROGRAMS)

echo_check_PROGRAMS:
	@echo $(check_PROGRAMS)

echo_check_SCRIPTS:
	@echo $(check_SCRIPTS)

echo_lib_LTLIBRARIES:
	@echo $(lib_LTLIBRARIES)

echo_prefix:
	@echo $(prefix)

echo_builddir:
	@echo $(builddir)

echo_abs_builddir:
	@echo $(abs_builddir)

echo_top_builddir:
	@echo $(top_builddir)

echo_abs_top_builddir:
	@echo $(abs_top_builddir)

echo_top_build_prefix:
	@echo $(top_build_prefix)

echo_srcdir:
	@echo $(srcdir)

echo_abs_srcdir:
	@echo $(abs_srcdir)

echo_top_srcdir:
	@echo $(top_srcdir)

echo_abs_top_srcdir:
	@echo $(abs_top_srcdir)

echo_bindir:
	@echo $(bindir)

echo_sbindir:
	@echo $(sbindir)

echo_docdir:
	@echo $(docdir)

echo_exec_prefix:
	@echo $(exec_prefix)

echo_libdir:
	@echo $(libdir)

echo_pkglibdir:
	@echo $(pkglibdir)

echo_includedir:
	@echo $(includedir)

echo_pkgincludedir:
	@echo $(pkgincludedir)

echo_libexecdir:
	@echo $(libexecdir)

echo_datadir:
	@echo $(datadir)

echo_OMP_CFLAGS:
	@echo $(OPENMP_CFLAGS)

echo_OMP_CXXFLAGS:
	@echo $(OPENMP_CXXFLAGS)

echo_BOOST_CPPFLAGS:
	@echo $(BOOST_CPPFLAGS)

