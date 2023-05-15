#
#
#
#
#

include ../../../../build/config.mk
include ../makehead.env

SOURCES = maxdome.cpp systeminfo.cpp lanconfig.cpp wlanconfig.cpp ipconfig.cpp netstate.cpp netutils.cpp maxdome_sdi.cpp

OBJS	:= $(SOURCES:.cpp=.o)

ifeq ($(ARCHITECTURE:mipsel=mips), mips)
	CFLAGS = -fPIC
else
	CFLAGS = -fpic
endif

CFLAGS +=  -I$(NATIVE_PORT_PATH)/include -I$(NATIVE_APP_PATH)/packages/openssl/include -I$(OPERA_HUMAX_IMPL_HOME)/jsplugins/
CFLAGS +=  -I$(NATIVE_APP_PATH)/apps/octo/library/memory -I$(NATIVE_APP_PATH)/apps/octo/library/hlib/include
CFLAGS +=  -I$(OPERA_HUMAX_IMPL_HOME)/jsplugins/extension/



TARGET	:= maxdome.so

$(TARGET): $(OBJS)
	echo $(OBJS)
	$(CXX) -shared $(OBJS) -o maxdome.so
	
all: $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $< -o $@
