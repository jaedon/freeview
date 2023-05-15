# -*- Mode: Makefile; tab-width: 4 -*-
#
# The output of the make process will be videobackend.so
#
# This makefile contains some defines
#
# CROSS
# Cross compiler sufflix used (if any).
#
# OVB_INCLUDE       Path to the directory that contains the opera video backend include file(s)
#
# HTTP_SUPPORT		Answer YES to VideoCanPlayURL and shows how the platform can handle the download.
#
# FORCEPAUSE_SUPPORT Answer YES to limit number of playing instances to 1

-include ../../build/config.mk

PLUGIN=videobackend.so

# Some default values if not forced on command line
OVB_INCLUDE?=../..
DEBUG=YES
HTTP_SUPPORT=YES
FORCEPAUSE_SUPPORT=NO

CXX = $(CROSS)g++
CFLAGS = -shared -fPIC
INCPATH = -I. -I.. -I$(OVB_INCLUDE) -I$(NATIVE_PORT_PATH)/include
LIBS = -lpthread
CFLAGS +=  -I$(NATIVE_APP_PATH)/apps/make/include/

SIMPLETESTPLUGIN=videobackend.o videobackend_utils.o videobackend_list.o

# provide for by-hand extra flags:
CFLAGS += $(COMPILERFLAGS)

ifeq ($(DEBUG), YES)
  CFLAGS += -g -DDEBUG
endif

ifeq ($(HTTP_SUPPORT), YES)
  CFLAGS += -DHTTP_SUPPORT
endif

ifeq ($(FORCEPAUSE_SUPPORT), YES)
  CFLAGS += -DFORCEPAUSE_SUPPORT
endif

default all: $(PLUGIN)

$(PLUGIN): %.so: $(SIMPLETESTPLUGIN)
	$(CXX) $(CFLAGS) -o $@ $(INCPATH) $(SIMPLETESTPLUGIN) $(LIBS)

$(SIMPLETESTPLUGIN): %.o : %.cpp $(wildcard %.h)
	$(CXX) -o $@ $(CFLAGS) $(INCPATH) -c $<

clean:
	rm -rf $(PLUGIN) $(SIMPLETESTPLUGIN)

install:
	cp -f videobackend.so $(OPERA_DIR)/video/

