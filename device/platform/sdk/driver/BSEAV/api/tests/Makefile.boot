BSEAV=$(shell cd ../..; pwd)

# Use debug version of libraries
B_REFSW_DEBUG ?=y

CHIP=7038
APP=boot

all: ${APP} 
${APP}: libs


.PHONY: libs



include ${BSEAV}/api/include/api.mak

CFLAGS += -Wall -W
CFLAGS += ${BSETTOP_CFLAGS}
LDFLAGS += -lpthread -L ../lib/97038.debug -lsettop_base -lsettop 
LIBS = ../lib/97038.debug/libsettop_base.so ../lib/97038.debug/libsettop.so 
OBJS = ${APP}.o

clean-libs:
	make -C ../build -f Makefile.base veryclean
	make -C ../build -f Makefile.settop veryclean

libs:
	make -C ../build -f Makefile.base
	make -C ../build -f Makefile.settop

include ${BSETTOP}/build/rules.mak

include ${BSETTOP}/build/tools.mak

${LIBS}:libs

ifdef DESTDIR
.PHONY: install

install: all
	${CP} ${APP} ${LIBS} ${DESTDIR}
endif

