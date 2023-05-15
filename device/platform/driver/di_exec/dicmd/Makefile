#
# Makefile
#

# How to build directly
#	make HUMAX_CC=/opt/toolchains/stbgcc-4.5.4-2.6/bin/mipsel-linux-gcc CFLAGS= LDFLAGS= all

OPTFLAGS = -O2 -fomit-frame-pointer -D_FILE_OFFSET_BITS=64
WARNFLAGS = -Wall
DEBUGFLAGS = 

CFLAGS?=
CFLAGS+=$(OPTFLAGS) $(WARNFLAGS) $(DEBUGFLAGS)

LDFLAGS?=

OBJS = cmdclient.o

all: dicmd 

dicmd: $(OBJS)
	$(HUMAX_CC) $(LDFLAGS) $^ -o $@
ifneq ($(HUMAX_PREPARE_ROOT_DIR),)
	@cp -af $@ $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/
endif

clean:
	-@rm *.o 

.c.o:
	@echo "Compile $@ ......"
	@$(HUMAX_CC) $(CFLAGS) -c $< -o $@

.PHONY: dicmd
