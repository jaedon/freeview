#
# Makefile
#

# How to build directly
#	make HUMAX_CC=/opt/toolchains/stbgcc-4.5.4-2.6/bin/mipsel-linux-gcc CFLAGS= LDFLAGS=

OPTFLAGS = -O2 -fomit-frame-pointer -D_FILE_OFFSET_BITS=64
WARNFLAGS = -Wall
DEBUGFLAGS = 

CFLAGS?=
CFLAGS+=$(OPTFLAGS) $(WARNFLAGS) $(DEBUGFLAGS)

INCCFLAGS := -I$(HUMAX_WORK_DIR)/include/hos

LDFLAGS?=
LDFLAGS+=-L$(HUMAX_PREPARE_ROOT_DIR)/usr/lib
LDFLAGS += -lvkernel -lrt -ldl -lssp

OBJS = usbctrl.o

all: usbctrl

usbctrl: $(OBJS)
	$(HUMAX_CC) $(INCCFLAGS) $(LDFLAGS) $^ -o $@
ifneq ($(HUMAX_PREPARE_ROOT_DIR),)
	@cp -af $@ $(HUMAX_PREPARE_ROOT_DIR)/usr/bin/
endif

clean:
	-@rm *.o 

.c.o:
	@echo "Compile $@ ......"
	@$(HUMAX_CC) $(CFLAGS) $(INCCFLAGS) $(LDFLAGS) -c $< -o $@

.PHONY: usbctrl
