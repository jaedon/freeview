# -*- Mode: Makefile; tab-width: 4 -*-
#
# The output of the make process will be videobackend.so
#
# This makefile contains some defines
#
# CROSS
# Cross compiler sufflix used (if any).
#
# UVA_INCLUDE
# The directory containing UVA headers.
#

-include ../../../../build/config.mk

UVA_INCLUDE ?= ../../include
UVA_INCLUDE += $(NATIVE_APP_PATH)/apps/appl/browser/corsair/base-2.0/port/include/
UVA_INCLUDE += $(OPERA_HUMAX_IMPL_HOME)/../build/src/uva/include/

PLUGIN := libuvabackend-hmp.so
CXX := ${CROSS}g++
CC := ${CROSS}gcc
INCPATH := -I.
INCPATH += $(addprefix -I, $(UVA_INCLUDE))
CFLAGS := -Wall -fomit-frame-pointer -MMD

ifeq ($(DEBUG), YES)
	CFLAGS	+=-g -DDEBUG
else
	CFLAGS	+= -O2
endif

DUMMY_PLUGIN=hmp_backend.o hmp_factory.o mse_backend.o mse_filedump.o

ifeq ($(ARCHITECTURE:mipsel=mips), mips)
  CFLAGS += -fPIC
else
  CFLAGS += -fpic
endif

default all: $(PLUGIN)

$(PLUGIN): %.so : $(DUMMY_PLUGIN)
	$(CXX) -o $@ -shared -Wl,-soname,$@ $(CFLAGS) $(INCPATH) $^

$(DUMMY_PLUGIN): %.o : %.cpp $(wildcard %.h)
	$(CXX) -c $(CFLAGS) $(INCPATH) $< -o $@
#	$(CXX) -o $@ $(CFLAGS) $(INCPATH) -c $<

clean:
	rm -rf $(PLUGIN) $(DUMMY_PLUGIN)
