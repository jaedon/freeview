#
# Makefile
#

# How to build directly
# 	make HUMAX_CC=/opt/toolchains/stbgcc-4.5.4-2.6/bin/mipsel-linux-gcc LDFLAGS="-L../../../../../packages/libext2fs/lib -lblkid -luuid" CFLAGS="-I../../../../../packages/libext2fs/include -I../../../../../packages/libext2fs/include/blkid" all

OPTFLAGS = -O2 -fomit-frame-pointer -D_FILE_OFFSET_BITS=64
WARNFLAGS = -Wall
DEBUGFLAGS = 

CFLAGS?=
CFLAGS+=$(OPTFLAGS) $(WARNFLAGS) $(DEBUGFLAGS)
CFLAGS+=-I$(HUMAX_WORK_DIR)/../packages/libext2fs/include
CFLAGS+=-I$(HUMAX_WORK_DIR)/../packages/libext2fs/include/blkid

INCCFLAGS := -I$(HUMAX_WORK_DIR)/include/hos

LDFLAGS?=
LDFLAGS+=-L$(HUMAX_PREPARE_ROOT_DIR)/lib
LDFLAGS+=-L$(HUMAX_PREPARE_ROOT_DIR)/usr/lib
LDFLAGS+=-lblkid -luuid
LDFLAGS += -lvkernel -lrt -ldl -lssp

OBJS = probefs.o

all: probefs

probefs: $(OBJS)
	$(HUMAX_CC) $(LDFLAGS) $(INCCFLAGS) $^ -o $@
ifneq ($(HUMAX_PREPARE_ROOT_DIR),)
	@cp -af $@ $(HUMAX_PREPARE_ROOT_DIR)/sbin/
endif


clean:
	@rm probefs
	@rm *.o 

.c.o:
	@echo "Compile $@ ......"
	@$(HUMAX_CC) $(CFLAGS) $(INCCFLAGS) -c $< -o $@

.IGNORE: $(OBJS) probefs clean
.PHONY: probefs
