######### Makefile ##########
CROSS	= mipsel-linux-uclibc-
CC	= $(CROSS)gcc
AR	= $(CROSS)ar
AS	= $(CROSS)as
NM	= $(CROSS)nm
STRIP	= $(CROSS)strip
CPP	= $(CC) -E

SETTOP_LIB = $(shell cd ../../make/driver/lib; pwd)
NEXUS_TOP  = $(shell cd ../../../platform/broadcom/bcm7405/bcm/nexus; pwd)
TOP_DIR = $(shell cd ../..; pwd)

INCS += $(TOP_DIR)/drv/include
INCS += $(TOP_DIR)/di/include
INCS += $(TOP_DIR)/utils/include
INCS += $(TOP_DIR)/include

SRCS=./main.c

OBJS=$(SRCS:.c=.o)

TARGET=hw_test

CFLAGS = $(addprefix -I,$(INCS)) 
CFLAGS += -g
LDFLAGS = -lpthread -lnexus -L$(NEXUS_TOP)/bin -L$(SETTOP_LIB) -ldriver

hw_test:$(OBJS)
	@echo [Compile $<]
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

clean: 
	@$(RM) *.o $(TARGET)

distclean: clean

.PHONY: clean distclean
