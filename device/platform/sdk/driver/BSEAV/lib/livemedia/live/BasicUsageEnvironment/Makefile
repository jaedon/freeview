INCLUDES = -Iinclude -I../UsageEnvironment/include -I../groupsock/include
##### Change the following for your environment: 
#CROSS_COMPILE=		arm-elf-
#COMPILE_OPTS =		$(INCLUDES) -I. -O2 -DSOCKLEN_T=socklen_t -DNO_STRSTREAM=1 -D_LARGEFILE_SOURCE=1
#C =			c
#C_COMPILER =		$(CROSS_COMPILE)gcc
#C_FLAGS =		$(COMPILE_OPTS)
#CPP =			cpp
#CPLUSPLUS_COMPILER =	$(CROSS_COMPILE)gcc
#CPLUSPLUS_FLAGS =	$(COMPILE_OPTS) -Wall -DBSD=1
#OBJ =			o
#LINK =			$(CROSS_COMPILE)gcc -o
#LINK_OPTS =		-L.
#CONSOLE_LINK_OPTS =	$(LINK_OPTS)
#LIBRARY_LINK =		$(CROSS_COMPILE)ld -o
#LIBRARY_LINK_OPTS =	$(LINK_OPTS) -r -Bstatic
#LIB_SUFFIX =			a
#LIBS_FOR_CONSOLE_APPLICATION =
#LIBS_FOR_GUI_APPLICATION =
#EXE =

B_REFSW_CFLAGS += -DLINUX
B_REFSW_CFLAGS += -pipe
B_REFSW_CFLAGS += -W
B_REFSW_CFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE

B_REFSW_ARCH ?= mipsel-uclibc
ifeq ($(filter ${B_REFSW_ARCH}, mips-linux mips-uclibc mips-linux-uclibc), ${B_REFSW_ARCH})
CROSS_COMPILE ?= mips-linux-uclibc-
else
CROSS_COMPILE ?= mipsel-linux-uclibc-
endif

COMPILE_OPTS =		$(INCLUDES) -I. -DSOCKLEN_T=socklen_t -DNO_STRSTREAM=1 $(B_REFSW_CFLAGS) -O2
C =			c
C_COMPILER =		$(CROSS_COMPILE)gcc
C_FLAGS =		$(COMPILE_OPTS)
CPP =			cpp
# NOTE: We must use c++ and not g++ to avoid GPL contamination
CPLUSPLUS_COMPILER =	$(CROSS_COMPILE)c++
CPLUSPLUS_FLAGS =	$(COMPILE_OPTS) -Wall -DBSD=1
OBJ =			o
LINK =			$(CROSS_COMPILE)c++ -o
LINK_OPTS =		-L.
CONSOLE_LINK_OPTS =	$(LINK_OPTS)
LIBRARY_LINK =		$(CROSS_COMPILE)ld -o
LIBRARY_LINK_OPTS =	$(LINK_OPTS) -r -Bstatic
LIB_SUFFIX =			a
LIBS_FOR_CONSOLE_APPLICATION =
LIBS_FOR_GUI_APPLICATION =
EXE =
##### End of variables to change

LIB = libBasicUsageEnvironment.$(LIB_SUFFIX)
ALL = $(LIB)
all:	$(ALL)

OBJS = BasicUsageEnvironment0.$(OBJ) BasicUsageEnvironment.$(OBJ) \
	BasicTaskScheduler0.$(OBJ) BasicTaskScheduler.$(OBJ) \
	DelayQueue.$(OBJ) BasicHashTable.$(OBJ)

libBasicUsageEnvironment.$(LIB_SUFFIX): $(OBJS)
	$(LIBRARY_LINK)$@ $(LIBRARY_LINK_OPTS) \
		$(OBJS)

.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<       

.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) $<

BasicUsageEnvironment0.$(CPP):	include/BasicUsageEnvironment0.hh
include/BasicUsageEnvironment0.hh:	include/BasicUsageEnvironment_version.hh include/DelayQueue.hh
BasicUsageEnvironment.$(CPP):	include/BasicUsageEnvironment.hh
include/BasicUsageEnvironment.hh:	include/BasicUsageEnvironment0.hh
BasicTaskScheduler0.$(CPP):	include/BasicUsageEnvironment0.hh include/HandlerSet.hh
BasicTaskScheduler.$(CPP):	include/BasicUsageEnvironment.hh include/HandlerSet.hh
DelayQueue.$(CPP):		include/DelayQueue.hh
BasicHashTable.$(CPP):		include/BasicHashTable.hh

clean:
	-rm -rf *.$(OBJ) $(ALL) core *.core *~ include/*~

##### Any additional, platform-specific rules come here:
