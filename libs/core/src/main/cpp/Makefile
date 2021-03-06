SHELL := /bin/bash

# Location of this module's root directory
MODULE_DIR= ../../..

# Location of the neurodidactic repository
export NEURO_REPO_DIR= ../../../${NEURODIDACTIC_REPO_DIR}
export NEURO_INC_DIR= ${NEURO_REPO_DIR}/include
export NEURO_LIB_DIR= ${NEURO_REPO_DIR}/lib
export NEURO_BIN_DIR= ${NEURO_REPO_DIR}/bin

# Translate NEURODIDACTIC_DEPS into the appropriate libraries
NEURODIDACTIC_LIBS= ${foreach l,${NEURODIDACTIC_DEPS},-lneurodidactic_${l}}
NEURODIDACTIC_SOLIBS= ${foreach l,${NEURODIDACTIC_DEPS},${NEURO_REPO_DIR}/libneuro_${l}.so}

# Translate PISTIS_DEPS into the appropriate include and library directories
PISTIS_LIBS= ${foreach l,${PISTIS_DEPS},-lpistis_${l}}
PISTIS_SOLIBS= ${foreach l,${PISTIS_DEPS},${REPO_LIB_DIR}/libpistis_${l}.so.${VERSION}}

# Variables used to build this module
TARGET_DIR= ${MODULE_DIR}/target
OUTPUT_DIRS= ${TARGET_DIR} ${TARGET_DIR}/obj ${TARGET_DIR}/lib
INC_DIRS= -I. -I${NEURO_INC_DIR} -I${REPO_INC_DIR} ${THIRD_PARTY_INC_DIRS}
LIB_DIRS= -L${NEURO_LIB_DIR} -L${REPO_LIB_DIR} ${THIRD_PARTY_LIB_DIRS}
CXX_COMPILE_OPTS= ${CXX_OPTS_${CONFIGURATION}} -std=c++14 -fPIC -D_REENTRANT -DNDEBUG -ftemplate-depth=128
CXX_COMPILE_FLAGS= ${CXX_COMPILE_OPTS} ${INC_DIRS}
CXX_LINK_OPTS= ${CXX_OPTS_${CONFIGURATION}} -shared
CXX_LINK_FLAGS= ${CXX_LINK_OPTS} ${LIB_DIRS}

# Source files are all *.cpp files in this directory or a subdirectory
SRC_DIRS := ${subst ./,,${shell find . -regextype posix-egrep -type d -not -name . -not -regex '.*/\..*' -print}}
SRC_FILES= ${foreach p,${SRC_DIRS},$p/*.cpp} *.cpp

# Derive object files from source files. Object files will be stored in
# ${TARGET_DIR}/obj
OBJ_SUBDIRS= ${foreach p,${SRC_DIRS},${TARGET_DIR}/obj/$p}
OBJ_FILES= ${foreach p,${patsubst %.cpp,%.o,${wildcard ${SRC_FILES}}}, ${TARGET_DIR}/obj/${p}}

# Derive dependency files from source files.  These will also be stored in
# ${TARGET_DIR}/obj
DEP_FILES= ${foreach p,${patsubst %.cpp,%.d,${wildcard ${SRC_FILES}}}, ${TARGET_DIR}/obj/${p}}

# Rules used to build targets
.PHONY: all dirs depends compile link deploy clean

all: link

${TARGET_DIR}/obj/%.d: %.cpp
	[ -d ${dir $@} ] || ${MAKE} dirs
	${CXX} -c ${CXX_COMPILE_FLAGS} -DMAKEDEPEND -MM ${CXXFLAGS} -I.obj -I.. -MF $@ -MQ $(@:%.d=%.o) -MQ $(@) $<

${TARGET_DIR}/obj/%.o: %.cpp
	${CXX} ${CXX_COMPILE_FLAGS} -c -o $@ $<

${TARGET_DIR}/lib/${LIBRARY}: ${OBJ_FILES} ${NEURODIDACTIC_SOLIBS} ${PISTIS_SOLIBS}
	${CXX} ${CXX_LINK_FLAGS} -o $@ -Wl,-h,${SONAME} ${OBJ_FILES} ${NEURODIDACTIC_LIBS} ${PISTIS_LIBS} ${THIRD_PARTY_LIBS}
	cd ${TARGET_DIR}/lib && ln -sf ${LIBRARY} ${SONAME} && ln -sf ${LIBRARY} lib${LIBRARY_NAME}.so

ifneq ($(MAKECMDGOALS),dirs)
ifneq ($(MAKECMDGOALS),clean)
include ${DEP_FILES}
endif
endif

${OUTPUT_DIRS} ${OBJ_SUBDIRS}:
	[ -d $@ ] || mkdir $@

dirs: ${OUTPUT_DIRS} ${OBJ_SUBDIRS}

compile: dirs ${OBJ_FILES}

link: compile ${TARGET_DIR}/lib/${LIBRARY}

install:
	-[[ -d ${NEURO_INC_DIR}/neurodidactic/core ]] && rm -rf ${NEURO_INC_DIR}/ineurodidactic/core
	for d in ${SRC_DIRS}; do ([[ -d ${NEURO_INC_DIR}/$${d} ]] || mkdir -p ${NEURO_INC_DIR}/$${d}); cp $${d}/*.hpp ${NEURO_INC_DIR}/$${d}/.; done
	cp ${TARGET_DIR}/lib/${LIBRARY} ${NEURO_LIB_DIR}/.
	cd ${NEURO_LIB_DIR} && ln -sf ${LIBRARY} ${SONAME} && ln -sf ${LIBRARY} lib${LIBRARY_NAME}.so

clean:
	-rm -rf ${TARGET_DIR}/obj
