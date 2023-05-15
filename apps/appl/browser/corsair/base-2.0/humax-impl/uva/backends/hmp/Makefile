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
UVA_INCLUDE += $(NATIVE_APP_PATH)/packages/calypso/debug/include/
UVA_INCLUDE += $(OPERA_HUMAX_IMPL_HOME)/../build/src/uva/include/
UVA_INCLUDE += $(NATIVE_APP_PATH)/apps/make/include/
UVA_INCLUDE += $(NATIVE_APP_PATH)/apps/octo/library/hlib/include/
UVA_INCLUDE += $(NATIVE_APP_PATH)/apps/octo/library/memory/
UVA_INCLUDE += ../include/

ifeq ($(CONFIG_DEBUG),y)
DEBUG := YES
else
DEBUG := NO
endif

MSE=YES

PLUGIN := libuvabackend-hmp.so
CXX := ${CROSS}g++
CC := ${CROSS}gcc
INCPATH := -I.
INCPATH += $(addprefix -I, $(UVA_INCLUDE))
CFLAGS := -Wall -fomit-frame-pointer -MMD

CFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE

ifeq ($(DEBUG), YES)
	CFLAGS	+=-g -DDEBUG
else
	CFLAGS	+= -O2
endif

################################################################################
# Set the value for logging
################################################################################
ifeq ($(CONFIG_DEBUG),y)
CFLAGS += -DCONFIG_CORSAIR_LOG
endif

ifeq ($(CONFIG_DEBUG_RELEASE_LOG),y)
CFLAGS += -DCONFIG_CORSAIR_LOG
endif

ifeq ($(CONFIG_DEVELOP_RELEASE),y)
CFLAGS += -DCONFIG_CORSAIR_LOG
endif

ifeq ($(CONFIG_TIMESTAMP_64), y)
CFLAGS	+= -DCONFIG_TIMESTAMP_64
endif

ifeq ($(MSE), YES)
	CFLAGS	+= -DENABLE_MSE=1
endif

ifneq ($(UVA_COMPILERFLAGS),)
  CFLAGS += $(UVA_COMPILERFLAGS)
endif

HMP_PLUGIN	= mediaplayer.o msemediasource.o hmxmediaplayer.o msemediaplayer.o hmpbackend.o hmpfactory.o
DEPFILES	= $(HMP_PLUGIN:.o=.d)

ifeq ($(ARCHITECTURE:mipsel=mips), mips)
  CFLAGS += -fPIC
else
  CFLAGS += -fpic
endif

default all: $(PLUGIN)

$(PLUGIN): %.so : $(HMP_PLUGIN)
	$(CXX) -o $@ -shared -Wl,-soname,$@ $(CFLAGS) $(INCPATH) $^

$(HMP_PLUGIN): %.o : %.cpp $(wildcard %.h)
	$(CXX) -c $(CFLAGS) $(INCPATH) $< -o $@

clean:
	rm -rf $(PLUGIN) $(HMP_PLUGIN) $(DEPFILES)
