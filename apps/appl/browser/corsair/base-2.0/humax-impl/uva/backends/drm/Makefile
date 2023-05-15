# -*- Mode: Makefile; tab-width: 4 -*-
#
# The output of the make process will be libuvadrmbackend.so
#
# This makefile depends on some defines
#
# CROSS
# Cross compiler sufflix used (if any).
#
# UVA_INCLUDE
# The directory containing UVA headers.
#
# PLAYREADYDIR
# The directory containing PlayReady PK sources.
#
# UVA_INCPATH
# Include paths that should be added during compilation
#

-include ../../../../build/config.mk

UVA_INCLUDE = ../../include
UVA_INCLUDE += $(NATIVE_APP_PATH)/apps/appl/browser/corsair/base-2.0/port/include/
UVA_INCLUDE += $(OPERA_HUMAX_IMPL_HOME)/../build/src/uva/include/
UVA_INCLUDE += $(NATIVE_APP_PATH)/apps/octo/library/hlib/include/
UVA_INCLUDE += $(NATIVE_APP_PATH)/apps/octo/library/memory/
UVA_INCLUDE += $(NATIVE_APP_PATH)/apps/make/include/
UVA_INCLUDE += ../include/

ifeq ($(CONFIG_DEBUG),y)
DEBUG := YES
else
DEBUG := NO
endif

ifeq ($(CONFIG_SUPPORT_IMMA), y)
PLAYREADY=YES
endif
CXX := ${CROSS}g++
CC := ${CROSS}gcc
INCLUDE_PATH := -I.
INCLUDE_PATH += $(addprefix -I, $(UVA_INCLUDE))

ifneq ($(UVA_INCPATH),)
  INCLUDE_PATH += $(addprefix -I, $(UVA_INCPATH))
endif

CFLAGS := -Wall

CFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE

ifeq ($(DEBUG), YES)
    CFLAGS += -g -DDEBUG
else
    CFLAGS += -O2
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

PLAYREADY_HUMAXDEFINE = $(NATIVE_APP_PATH)/packages/playready/include/humax_platform.h

ifeq (exist, $(shell [ -e $(PLAYREADY_HUMAXDEFINE) ] && echo exist))
PLAYREADY_CFLAGS := -include $(NATIVE_APP_PATH)/packages/playready/include/humax_platform.h
else
#For PlayReady 2.0
PLAYREADY_CFLAGS := -DDRM_BUILD_PROFILE=10 -DDRM_SUPPORT_LOCKING=0 -DDRM_SUPPORT_MULTI_THREADING=0 -DDRM_SUPPORT_DATASTORE_PREALLOC=1 -DDRM_SUPPORT_NATIVE_64BIT_TYPES=1 -DDRM_SUPPORT_FORCE_ALIGN=0 -DDRM_SUPPORT_ASSEMBLY=1 -DDRM_SUPPORT_PRECOMPUTE_GTABLE=0 -DDRM_SUPPORT_TRACING=0 -DDRM_SUPPORT_PRIVATE_KEY_CACHE=0 -DDRM_SUPPORT_EMBEDDED_CERTS=0 -DDRM_SUPPORT_DEVICE_SIGNING_KEY=0 -DDRM_SUPPORT_NONVAULTSIGNING=1 -DDRM_USE_IOCTL_HAL_GET_DEVICE_INFO=0 -D_DATASTORE_WRITE_THRU=1 -D_ADDLICENSE_WRITE_THRU=0 -DDRM_HDS_COPY_BUFFER_SIZE=32768 -DDRM_TEST_SUPPORT_NET_IO=0 -DUSE_PK_NAMESPACES=0 -DDRM_INCLUDE_PK_NAMESPACE_USING_STATEMENT=0 -DDRM_KEYFILE_VERSION=3 -DDBG=1 -D_CRT_SECURE_NO_DEPRECATE -D_CRT_OBSOLETE_NO_DEPRECATE -D_CRT_SECURE_FORCE_DEPRECATE -D_X86_ -DTARGET_LITTLE_ENDIAN=1 -DPLAY_READY_20

#FOR PLAY_READY 2.5
#PLAYREADY_CFLAGS := -DDRM_BUILD_PROFILE=900 -DTARGET_LITTLE_ENDIAN=1 -DTARGET_SUPPORTS_UNALIGNED_DWORD_POINTERS=1 -DLINUX_BUILD -DHUMAX_PLATFORM_BASE -DDRM_ACTIVATION_PLATFORM=0 -DDRM_SUPPORT_LOCKING=0 -DDRM_SUPPORT_MULTI_THREADING=1 -DDRM_SUPPORT_ECCPROFILING=1 -DDRM_SUPPORT_INLINEDWORDCPY=1 -DDRM_SUPPORT_DATASTORE_PREALLOC=1 -DDRM_SUPPORT_REACTIVATION=0 -DDRM_SUPPORT_NATIVE_64BIT_TYPES=1 -DDRM_SUPPORT_FORCE_ALIGN=1 -DDRM_SUPPORT_ASSEMBLY=0 -DDRM_SUPPORT_PRECOMPUTE_GTABLE=1 -DDRM_SUPPORT_SECUREOEMHAL=0 -DDRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY=0 -DDRM_SUPPORT_TRACING=0 -DDRM_SUPPORT_PRIVATE_KEY_CACHE=0 -DDRM_SUPPORT_EMBEDDED_CERTS=0 -DDRM_SUPPORT_DEVICE_SIGNING_KEY=0 -DDRM_SUPPORT_NONVAULTSIGNING=1 -DDRM_USE_IOCTL_HAL_GET_DEVICE_INFO=0 -D_DATASTORE_WRITE_THRU=1 -D_ADDLICENSE_WRITE_THRU=0 -DDRM_HDS_COPY_BUFFER_SIZE=32768 -DDRM_TEST_SUPPORT_NET_IO=0 -DDRM_TEST_SUPPORT_ACTIVATION=0 -DUSE_PK_NAMESPACES=0 -DDRM_INCLUDE_PK_NAMESPACE_USING_STATEMENT=1 -DDRM_KEYFILE_VERSION=3 -DDRM_TOOLS_PRND_TRANSMITTER_USES_UPNP=0 -DDBG=1 -DPLAY_READY_25
endif


OPENSSLDIR=$(NATIVE_APP_PATH)/packages/openssl/
PLAYREADYDIR=$(NATIVE_APP_PATH)/packages/playready/
CALYPSODIR=$(NATIVE_APP_PATH)/packages/calypso/

PKG_CONFIG_PATH?=/usr/lib/pkgconfig
DUMMY_PLUGIN_OBJ := dummy/dummy_drm_backend.o
PLAYREADY_PLUGIN_OBJ := playready/pr_drm_backend.o
CLEARKEY_PLUGIN_OBJ := clearkey/ck_drm_backend.o

ifneq ($(DUMMYDRM),NO)
    DUMMY_PLUGIN := libuvadrmbackend-dummy.so
    DUMMY_CFLAGS := -DDUMMY_SUPPORT
    DUMMY_INCPATH := -I./dummy
else
    DUMMY_PLUGIN :=
    DUMMY_CFLAGS :=
    DUMMY_INCPATH :=
endif

ifneq ($(OPENSSLDIR),)
    CLEARKEY_PLUGIN := libuvadrmbackend-clearkey.so
    CLEARKEY_CFLAGS := -DCLEARKEY_SUPPORT
    CLEARKEY_INCPATH := -I./clearkey -I$(OPENSSLDIR)/include
    CLEARKEY_INCPATH += -I$(CALYPSODIR)/release/include
    ifeq ($(wildcard $(OPENSSLDIR)/lib/libcrypto.a),)
        CLEARKEY_LFLAGS := -L$(OPENSSLDIR)/lib -lcrypto
        CLEARKEY_STATIC_LINK :=
    else
        CLEARKEY_LFLAGS :=
        CLEARKEY_STATIC_LINK := $(OPENSSLDIR)/lib/libcrypto.a
    endif
else
    CLEARKEY_PLUGIN :=
    CLEARKEY_CFLAGS :=
    CLEARKEY_INCPATH :=
    CLEARKEY_STATIC_LINK :=
endif

ifeq ($(PLAYREADY),YES)
    PLAYREADY_PLUGIN := libuvadrmbackend-playready.so
    PLAYREADY_CFLAGS += -DPLAYREADY_SUPPORT
    PLAYREADY_INCPATH := -I./playready -I$(PLAYREADYDIR)/include
    PLAYREADY_LFLAGS := -L$(PLAYREADYDIR)/lib -lprdy
    PLAYREADY_INCPATH += -I$(CALYPSODIR)/release/include
    PLAYREADY_INCPATH += -I$(OPENSSLDIR)/include
    ifeq ($(wildcard $(OPENSSLDIR)/lib/libcrypto.a),)
        PLAYREADY_LFLAGS += -L$(OPENSSLDIR)/lib -lcrypto
    else
        PLAYREADY_STATIC_LINK := $(OPENSSLDIR)/lib/libcrypto.a
    endif
else
    PLAYREADY_PLUGIN :=
    PLAYREADY_CFLAGS :=
    PLAYREADY_INCPATH :=
    PLAYREADY_LFLAGS :=
    PLAYREADY_STATIC_LINK :=
endif


#ARCHITECTURE?=x86

ifeq ($(ARCHITECTURE:mipsel=mips), mips)
  CFLAGS += -fPIC
else
  CFLAGS += -fpic
endif

CFLAGS += -DUVA_MEMORY_NEED_STL_ALLOCATOR

ifeq ($(ENDIAN), LITTLE)
    CFLAGS += -DLITTLE_ENDIAN
endif

ifneq ($(UVA_COMPILERFLAGS),)
  CFLAGS += $(UVA_COMPILERFLAGS)
endif

default all: $(DUMMY_PLUGIN) $(CLEARKEY_PLUGIN) $(PLAYREADY_PLUGIN)

$(DUMMY_PLUGIN): %.so : $(DUMMY_PLUGIN_OBJ) drm_factory_dummy.o managed_drm_backend.o
	$(CXX) -o $@ -shared -Wl,-soname,$@ $(CFLAGS) $(DUMMY_CFLAGS) $(LFLAGS) $^

$(CLEARKEY_PLUGIN): %.so : $(CLEARKEY_PLUGIN_OBJ) drm_factory_clearkey.o managed_drm_backend.o
	$(CXX) -o $@ -shared -Wl,-soname,$@ $(CFLAGS) $(CLEARKEY_CFLAGS) $(LFLAGS) $(CLEARKEY_LFLAGS) $^ $(CLEARKEY_STATIC_LINK)

$(PLAYREADY_PLUGIN): %.so : $(PLAYREADY_PLUGIN_OBJ) drm_factory_playready.o managed_drm_backend.o
	$(CXX) -o $@ -shared -Wl,-soname,$@ $(CFLAGS) $(PLAYREADY_CFLAGS) $(LFLAGS) $(PLAYREADY_LFLAGS) $^

drm_factory_dummy.o : drm_factory.cpp
	$(CXX) -o $@ $(CFLAGS) $(INCLUDE_PATH) $(DUMMY_CFLAGS) $(DUMMY_INCPATH) -c $<

managed_drm_backend.o: %.o : %.cpp $(wildcard %.h)
	$(CXX) -o $@ $(CFLAGS) $(INCLUDE_PATH) -c $<

$(DUMMY_PLUGIN_OBJ): %.o : %.cpp $(wildcard %.h)
	$(CXX) -o $@ $(CFLAGS) $(INCLUDE_PATH) $(DUMMY_CFLAGS) $(DUMMY_INCPATH) -c $<

drm_factory_clearkey.o : drm_factory.cpp
	$(CXX) -o $@ $(CFLAGS) $(INCLUDE_PATH) $(CLEARKEY_CFLAGS) $(CLEARKEY_INCPATH) -c $<

$(CLEARKEY_PLUGIN_OBJ): %.o : %.cpp $(wildcard %.h)
	$(CXX) -o $@ $(CFLAGS) $(INCLUDE_PATH) $(CLEARKEY_CFLAGS) $(CLEARKEY_INCPATH) -c $<

drm_factory_playready.o : drm_factory.cpp
	$(CXX) -o $@ $(CFLAGS) $(INCLUDE_PATH) $(PLAYREADY_CFLAGS) $(PLAYREADY_INCPATH) -c $<

$(PLAYREADY_PLUGIN_OBJ): %.o : %.cpp $(wildcard %.h)
	$(CXX) -o $@ $(CFLAGS) $(INCLUDE_PATH) $(PLAYREADY_CFLAGS) $(PLAYREADY_INCPATH) -c $<

clean:
	rm -rf $(CLEARKEY_PLUGIN_OBJ) $(PLAYREADY_PLUGIN_OBJ) $(DUMMY_PLUGIN_OBJ) *.so *.o *.d
