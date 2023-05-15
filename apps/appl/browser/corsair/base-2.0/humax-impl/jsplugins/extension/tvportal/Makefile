#
#
#
#
#

include ../../../../build/config.mk
include ../makehead.env

SOURCES := plugin_sysinfo.cpp

TARGETS=$(SOURCES:.cpp=.so)

ifeq ($(ARCHITECTURE:mipsel=mips), mips)
	CFLAGS = -fPIC
else
	CFLAGS = -fpic
endif

CFLAGS +=  -I$(NATIVE_PORT_PATH)/include -I$(NATIVE_APP_PATH)/apps/ -I$(OPERA_HUMAX_IMPL_HOME)/jsplugins/
CFLAGS +=  -I$(NATIVE_APP_PATH)/apps/octo/library/memory -I$(NATIVE_APP_PATH)/apps/octo/library/hlib/include

all: $(TARGETS)

clean:
	rm -f $(TARGETS)

%.so: %.c
	$(CC) $(CFLAGS) -I.. -shared $< -o $@

%.so: %.cpp
	$(CXX) $(CFLAGS) -I.. -shared $< -o $@
