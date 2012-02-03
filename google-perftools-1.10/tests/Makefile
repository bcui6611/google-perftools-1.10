# Hard coded Makefile for windows

CC = x86_64-w64-mingw32-gcc
CXX = x86_64-w64-mingw32-g++

TCMALLOC_FLAGS =
TCMALLOC_FLAGS += -Wwrite-strings \
                    -Wno-sign-compare -DNO_FRAME_POINTER \
                    -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc \
                    -fno-builtin-calloc -fno-builtin-cfree \
                    -fno-builtin-memalign -fno-builtin-posix_memalign \
                    -fno-builtin-valloc -fno-builtin-pvalloc
TCLIB=../.libs
TCMALLOC_LIB_FLAGS += -Wl,-u__tcmalloc \
                 $(TCLIB)/libtcmalloc_minimal.a -lPsapi -lshlwapi \
                 $(TCLIB)/libstacktrace.a \
                 -lstdc++.dll


TCMALLOC_FLAGS += -Wl,-u__tcmalloc
LIBTCMALLOC = $(TCLIB)/libtcmalloc_minimal.a \
       $(TCLIB)/libwindows.a $(TCLIB)/liblogging.a \
       $(TCLIB)/libsysinfo.a $(TCLIB)/libspinlock.a $(TCLIB)/libstacktrace.a 

BINARIES= test.exe

LIB=$(LOCALLIB) 
INCLUDE= -Iwin32 
CPPFLAGS= $(MARCH) -g -DHAVE_CONFIG_H ${INCLUDE} -Wall 

all: ${BINARIES}

CFLAGS = -std=gnu99 -DDEBUG -g \
           -fno-strict-aliasing -Wstrict-prototypes -Wmissing-prototypes \
           -Wmissing-declarations -Wredundant-decls \
		   $(TCMALLOC_FLAGS)

CXXFLAGS = -std=gnu++0x \
           -pedantic -Wshadow -fdiagnostics-show-option \
           -Wformat=2 -Wconversion -Wstrict-aliasing -Wextra \
           -Woverloaded-virtual -Wnon-virtual-dtor -Wctor-dtor-privacy \
           -Wno-long-long  -Wmissing-declarations -Wno-redundant-decls \
		   $(TCMALLOC_FLAGS)

TEST_C_SRC = \
           test.c \
		  
TEST_OBJS = ${TEST_C_SRC:%.c=%.o}


test.exe: ${TEST_OBJS}
	${LINK.cc} -o $@  ${TEST_OBJS} \
                  $(TCMALLOC_LIB_FLAGS) \
                  -lpthread

%.o: %.cpp
	${COMPILE.cc} -MMD $< -o $@

%.o: %.c
	${COMPILE.c} -MMD $< -o $@

clean:
	$(RM) ${TEST_OBJS} ${BINARIES} ${TEST_OBJS:.o=.d} \
              ${GENFILES} ${SQLITE3_OBJS} ${SQLITE3_OBJS:.o=.d}

-include ${TEST_OBJS:.o=.d}
