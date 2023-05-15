BSEAV=$(shell cd ../../..; pwd)

# Use debug version of libraries
B_REFSW_DEBUG ?=y

# Irrelevant any supported chip should do it 
CHIP=7115


all: ${APP}

include ${BSEAV}/linux/api/api.mak

CFLAGS += -Wall -W
CFLAGS += ${BSETTOP_CFLAGS}
LDFLAGS += ${BSETTOP_LDFLAGS}
LDFLAGS += ${BSETTOP_PVR_LDFLAGS}


OBJS = ${APP}.o


include ${BSETTOP}/rules.mak

ifdef DESTDIR
.PHONY: install

install: all
	${CP} ${APP} ${DESTDIR}
else
install:
	$(error Must define DESTDIR)
endif

include ${BSETTOP}/tools.mak

