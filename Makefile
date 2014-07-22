# C related flags. This project is mostly C++ at this point, but it still uses
# the C API provided by inih.
CC=/usr/bin/gcc
CFLAGS=-O3

# C++ related flags. Note that you may need to modify the CXXFLAGS variable to
# get SmallWM to build with Clang++.
CXX=/usr/bin/g++
CXXFLAGS=-g -IUnitTest++/src -Itest -Iinih -Isrc -Wold-style-cast --std=c++11
LINKERFLAGS=-lX11 -lXrandr

# Binaries are classified into two groups - ${BINS} includes the main smallwm
# binary only, while ${TESTS} includes all the binaries for the test suite.
BINS=bin/smallwm
TESTS=$(patsubst test/%.cpp,bin/test-%,$(wildcard test/*.cpp))

# We need to use := do to immediate evaluation. Since inih/ini.c is not with
# the rest of the C sources files, we handle it as an explicit case at the end.
# 
# Makefile variables with = are 'lazy', and thus self-reference doesn't work.
# If we do the naive thing and just use += on the second set, then inih/ini.c
# is included in ${OBJS} because the $(patsubst ...) expression cannot match
# it.
CFILES:=$(wildcard src/*.cpp)
OBJS:=$(patsubst src/%.cpp,obj/%.o,${CFILES})

CFILES:=${CFILES} $(wildcard src/model/*.cpp)
OBJS:=$(patsubst src/model/%.cpp,obj/model/%.o,${CFILES})

CFILES:=${CFILES} inih/ini.c
OBJS:=${OBJS} obj/ini.o

# ${HEADERS} exists mostly to make building Doxygen output more consistent
# since a change in the headers may require the API documentation to be
# re-created.
HEADERS=$(wildcard src/*.h src/model/*.h)

all: bin/smallwm

# Used to probe for compiler errors, without linking everything
check: ${OBJS}

doc: ${HEADERS} ${SRCS}
	doxygen

bin:
	[ -d bin ] || mkdir bin

obj:
	[ -d obj ] || mkdir obj
	[ -d obj/model ] || mkdir obj/model

test: ${TESTS}

clean:
	rm -rf bin obj doc

bin/smallwm: bin obj ${OBJS}
	${CXX} ${CXXFLAGS} ${OBJS} ${LINKERFLAGS} -o bin/smallwm

obj/ini.o: obj inih/ini.c
	${CC} ${CFLAGS} -c inih/ini.c -o obj/ini.o

obj/%.o: obj

obj/%.o: src/%.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@

obj/model/%.o: src/model/%.cpp
	${CXX} ${CXXFLAGS} -c $< -o $@

# Getting unit tests to build is a bit awkward. Since I want to avoid
# distributing a static library along with SmallWM, it is necessary to build
# UnitTest++ on-demand from source. Hence, recursive make...
bin/libUnitTest++.a: bin
	export CXX
	$(MAKE) -C UnitTest++ libUnitTest++.a
	cp UnitTest++/libUnitTest++.a bin

# This simply ensure that the test itself builds properly. It doesn't do
# anything, since if it builds, the test is a success.
bin/test-test: bin bin/libUnitTest++.a test/test.cpp
	${CXX} ${CXXFLAGS} test/test.cpp bin/libUnitTest++.a -o bin/test-test

bin/test-configparse: bin/libUnitTest++.a obj/test-configparse.o obj/ini.o obj/configparse.o obj/utils.o
	${CXX} ${CXXFLAGS} obj/test-configparse.o bin/libUnitTest++.a obj/configparse.o obj/ini.o obj/utils.o ${LINKERFLAGS} -o bin/test-configparse

obj/test-configparse.o: obj test/configparse.cpp
	${CXX} ${CXXFLAGS} -c test/configparse.cpp -o obj/test-configparse.o

bin/test-client-model: bin/libUnitTest++.a obj/test-client-model.o obj/model/client-model.o
	${CXX} ${CXXFLAGS} obj/test-client-model.o bin/libUnitTest++.a obj/model/client-model.o ${LINKER_FLAGS} -o bin/test-client-model

obj/test-client-model.o: obj test/client-model.cpp src/model/changes.h src/model/client-model.h src/model/desktop-type.h src/model/unique-multimap.h
	${CXX} ${CXXFLAGS} -c test/client-model.cpp -o obj/test-client-model.o
