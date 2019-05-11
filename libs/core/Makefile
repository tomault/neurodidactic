SHELL := /bin/bash

# Module components
MODULE_SRC_DIR=src/main/cpp
MODULE_TESTS_DIR=src/test/cpp

# Build configuration and compiler
export CONFIGURATION ?= DEBUG
export CXX ?= g++
export CXX_OPTS_DEBUG = -pthread -g
export CXX_OPTS_RELEASE = -pthread -g -O3

# Master repository location
export REPO_DIR ?= /home/tomault/cpp_repo
export REPO_INC_DIR ?= ${REPO_DIR}/include
export REPO_LIB_DIR ?= ${REPO_DIR}/lib
export REPO_BIN_DIR ?= ${REPO_DIR}/bin

# Location of MKL
export MKL_ROOT ?= /opt/intel/parallel_studio_xe_2019/compilers_and_libraries_2019/linux/mkl

# Neurodidactic repository location
export NEURODIDACTIC_REPO_DIR = ../../target

# Neurodidactic modules this module depends upon
export NEURODIDACTIC_DEPS =

# Pistis modules the current module depends upon
export PISTIS_DEPS = exceptions util

# Headers and libraries needed for unit tests only
export NEURODIDACTIC_TEST_INC_DIRS =
export NEURODIDACTIC_TEST_LIB_DIRS =
export NEURODIDACTIC_TEST_LIBS = 

# Third party dependencies
export THIRD_PARTY_INC_DIRS = -DMKL_ILP64 -I${MKL_ROOT}/include
export THIRD_PARTY_LIB_DIRS = -L${MKL_ROOT}/lib/intel64 -Wl,--no-as-needed
export THIRD_PARTY_LIBS= -lmkl_intel_lp64 -lmkl_gnu_thread -lmkl_core -lgomp -lpthread -lm -ldl

# Version information.  Release versions have decimal revision numbers, while
# snapshot versions have an "S" appended to the revision number.  Snapshot
# versions represent code under active development and may be updated in-place.
# Release versions are fixed and should be built from code in the
# source-control repository tagged with the release version number.  The
# functionality of a release version library or executable should not change --
# bug fixes and new features should go into the next release.
export VERSION= 0.1.1
export SOVERSION= 0.1
export LIBRARY_NAME= neurodidactic_core
export LIBRARY= lib${LIBRARY_NAME}.so.${VERSION}
export SONAME= lib${LIBRARY_NAME}.so.${SOVERSION}

${NEURODIDACTIC_REPO_DIR}/include ${NEURODIDACTIC_REPO_DIR}/lib ${NEURODIDACTIC_REPO_DIR}/bin:
	[ -d $@ ] || mkdir $@

dirs: ${NEURODIDACTIC_REPO_DIR}/include ${NEURODIDACTIC_REPO_DIR}/lib ${NEURODIDACTIC_REPO_DIR}/bin
	cd ${MODULE_SRC_DIR} && ${MAKE} dirs
	cd ${MODULE_TESTS_DIR} && ${MAKE} dirs

compile:
	cd ${MODULE_SRC_DIR} && ${MAKE} compile

link:
	cd ${MODULE_SRC_DIR} && ${MAKE} link

compile-test:
	cd ${MODULE_TESTS_DIR} && ${MAKE} compile

link-test:
	cd ${MODULE_TESTS_DIR} && ${MAKE} link

clean-test:
	cd ${MODULE_TESTS_DIR} && ${MAKE} clean

test: link
	cd ${MODULE_TESTS_DIR} && ${MAKE} test

install: test
	cd ${MODULE_SRC_DIR} && ${MAKE} install

install-without-test:
	cd ${MODULE_SRC_DIR} && ${MAKE} install

clean:
	-rm -rf target
