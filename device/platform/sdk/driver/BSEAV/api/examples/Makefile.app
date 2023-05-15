BSEAV=$(shell cd ../..; pwd)

# Use debug version of libraries
BRUTUS_ODIR ?= y

# Irrelevant any supported chip should do it
CHIP ?= 7115


all: ${APP}

include ${BSEAV}/api/include/api.mak

CFLAGS += -Wall -W
CFLAGS += ${BSETTOP_CFLAGS}
LDFLAGS += ${BSETTOP_LDFLAGS}
LDFLAGS += $(BSETTOP_LIB_THREAD) $(BSETTOP_LIB_PVR) $(BSETTOP_LIB_ENCODER)


OBJS = ${APP}.o


include ${BSETTOP}/build/rules.mak

ifdef DESTDIR
.PHONY: install

install: all
	${CP} ${APP} ${DESTDIR}
else
install:
	$(error Must define DESTDIR)
endif

include ${BSETTOP}/build/tools.mak

