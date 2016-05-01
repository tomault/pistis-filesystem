# Location of this module's root directory
MODULE_DIR= ../../..

# Translate PISTIS_DEPS into the appropriate include and library directories
PISTIS_LIBS= ${foreach l,${PISTIS_DEPS},-lpistis_${l}}
PISTIS_SOLIBS= ${foreach l,${PISTIS_DEPS},${REPO_LIB_DIR}/libpistis_${l}.so.${VERSION}}

# Variables used to build this module
TARGET_DIR= ${MODULE_DIR}/target
OUTPUT_DIRS= ${TARGET_DIR} ${TARGET_DIR}/test ${TARGET_DIR}/test/obj ${TARGET_DIR}/test/bin
INC_DIRS= -I. -I${MODULE_DIR}/src/main/cpp -I${REPO_INC_DIR} ${PISTIS_TEST_INC_DIRS} ${THIRD_PARTY_INC_DIRS}
LIB_DIRS= -L${TARGET_DIR}/lib -L${REPO_LIB_DIR} ${PISTIS_TEST_LIB_DIRS} ${THIRD_PARTY_LIB_DIRS}
CXX_COMPILE_OPTS= ${CXX_OPTS_${CONFIGURATION}} -std=c++14 -D_REENTRANT -DNDEBUG -ftemplate-depth=128
CXX_COMPILE_FLAGS= ${CXX_COMPILE_OPTS} ${INC_DIRS}
CXX_LINK_OPTS= ${CXX_OPTS_${CONFIGURATION}} -rdynamic
CXX_LINK_FLAGS= ${CXX_LINK_OPTS} ${LIB_DIRS}
TEST_BIN= ${TARGET_DIR}/test/bin/unit_tests

# Source files are all *.cpp files in this directory or a subdirectory
SRC_DIRS := ${subst ./,,${shell find . -regextype posix-egrep -type d -not -name . -not -regex '.*/\..*' -print}}
SRC_FILES= ${foreach p,${SRC_DIRS},$p/*.cpp} *.cpp

# Derive object files from source files. Object files will be stored in
# ${TARGET_DIR}/test/obj
OBJ_SUBDIRS= ${foreach p,${SRC_DIRS},${TARGET_DIR}/test/obj/$p}
OBJ_FILES= ${foreach p,${patsubst %.cpp,%.o,${wildcard ${SRC_FILES}}}, ${TARGET_DIR}/test/obj/${p}}

# Derive dependency files from source files.  These will also be stored in
# ${TARGET_DIR}/test/obj
DEP_FILES= ${foreach p,${patsubst %.cpp,%.d,${wildcard ${SRC_FILES}}}, ${TARGET_DIR}/test/obj/${p}}

# Rules used to build targets
.PHONY: all dirs depends compile link deploy clean

all: test

${TARGET_DIR}/test/obj/%.d: %.cpp
	[ -d ${dir $@} ] || ${MAKE} dirs
	${CXX} -c ${CXX_COMPILE_FLAGS} -DMAKEDEPEND -MM ${CXXFLAGS} -I.obj -I.. -MF $@ -MQ $(@:%.d=%.o) -MQ $(@) $<

${TARGET_DIR}/test/obj/%.o: %.cpp
	${CXX} ${CXX_COMPILE_FLAGS} -c -o $@ $<

${TEST_BIN}: ${OBJ_FILES} ${PISTIS_SOLIBS}
	${CXX} ${CXX_LINK_FLAGS} -o $@ ${OBJ_FILES} -lgtest -lgtest_main -l${LIBRARY_NAME} ${PISTIS_SOLIBS} ${PISTIS_TEST_LIBS} ${THIRD_PARTY_LIBS}

ifneq ($(MAKECMDGOALS),dirs)
ifneq ($(MAKECMDGOALS),clean)
include ${DEP_FILES}
endif
endif

${OUTPUT_DIRS} ${OBJ_SUBDIRS}:
	[ -d $@ ] || mkdir $@

dirs: ${OUTPUT_DIRS} ${OBJ_SUBDIRS}

compile: dirs ${OBJ_FILES}

link: compile ${TEST_BIN}

test: link
	cd ${TARGET_DIR}/test/bin
	LD_LIBRARY_PATH=${TARGET_DIR}/lib:${REPO_LIB_DIR}:/usr/local/lib:${LD_LIBRARY_PATH} ${TEST_BIN}

clean:
	-rm -rf ${TEST_BIN} ${TARGET_DIR}/test/obj/*
