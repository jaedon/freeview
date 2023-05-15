#
# GNUmakefile for wl/exe
#
# Copyright (c) 2000, Broadcom Corp.
# $Id: GNUmakefile,v 1.4 2010/06/17 00:34:48 Exp $

ifndef	SRCBASE
	SRCBASE = ../../..
endif

include ../../../modules.inc

UNAME = $(shell uname)

#-----------------------------------------------------------------
# Windows build
# 1) windows, don't include Makerules due to all: conflict

ifeq ($(findstring CYGWIN,$(UNAME)),CYGWIN)

include ../../../GNUmakefile.inc

else # UNAME

# 2) not windows, need to include first to pick up TARGETENV dependent vars
include $(SRCBASE)/Makerules

ifeq (${WIFI_DRIVER}, aardvark)
CFLAGS += -DAARDVARK_WIFI_DRIVER
endif
#ifdef BCMWPA2
CFLAGS += -DBCMWPA2
#endif

CFLAGS += -DBCMSUP_PSK

#ifdef WLCNT
CFLAGS += -DWLCNT
#endif

#-----------------------------------------------------------------
# Linux build
#
# This should be one of values recognized in src/Makerules

ifneq ($(findstring $(TARGETENV), "linux linuxmips linuxmips_be linuxarm linuxarm_le"),)

# Discard any "MMX" or other qualifications on x86 so that
# any TARGETARCH containing x86 is just "x86"
ifeq ($(findstring x86_mmx,$(TARGETARCH)),x86_mmx)
	TARGETARCH = x86
endif

OBJDIR = $(TARGETARCH)

# $(TARGETARCH) is set based on TARGETENV in src/Makerules.* files
#BWL_OBJS := bwl.o bwl_utils.o bwl_linux.o bwl_wps_enr.o bwltest.o
BWL_OBJS := bwl.o bwl_utils.o bwl_linux.o  
BWL_EXE  := bwl
BWL_LIB	 := lib$(BWL_EXE).so

ifeq (${WIFI_DRIVER}, aardvark)
WLU_OBJS := bcmutils.o bcmwifi_channels.o 
BLD_BWL_VER := bwl_ver
else
WLU_OBJS := bcmutils.o bcmwifi.o 
endif

ifeq ($(INCLUDE_WPS),1)
BWL_OBJS += bwl_wps_enr.o 
WPS_TARGET := wps_enr 
WPS_PATH := $(OBJDIR)
WPS_LIBS  = $(WPS_PATH)/libwpsenr.a $(WPS_PATH)/libwpscom.a $(WPS_PATH)/libbcmcrypto.a
WPS_OBJS := wps_linux_hooks.o wl_wps.o
CFLAGS += -DINCLUDE_WPS
LDFLAGS  += -L$(WPS_PATH) 
endif

ifndef SHARED_LIB
BWL_OBJS += bwltest.o 
endif

ifneq ($(TARGETARCH),x86)
  BWL_EXE  := $(BWL_EXE)$(TARGETARCH)
  BWL_OBJS := $(BWL_OBJS:%.o=$(OBJDIR)/%.o)
  WLU_OBJS := $(WLU_OBJS:%.o=$(OBJDIR)/%.o)
  WPS_OBJS := $(WPS_OBJS:%.o=$(OBJDIR)/%.o)
endif

###########################
# openssl
###########################
BCM_PARTIAL_CRYPTO = 1
EXTERNAL_OPENSSL = 0

ifdef INCLUDE_WPS
export CFLAGS += -include wps_openssl.h
export CXXFLAGS += -include wps_openssl.h
endif

ifdef INCLUDE_WPS

ifeq ($(BCM_PARTIAL_CRYPTO),1)
  WPS_LIBS += $(WPS_PATH)/libbcmcrypto.a
  LDFLAGS  += -Wl,-static,-lwpsenr,-lwpscom,-lbcmcrypto,-Bsymbolic,-dy,-lstdc++
else
  LDFLAGS  += -Wl,-static,-lwpsenr,-lwpscom,-Bsymbolic,-dy,-lstdc++
  WPS_OBJS += bcmcrypto/rijndael-alg-fst.o bcmcrypto/hmac_sha256.o bcmcrypto/aes.o
endif

export EXTERNAL_OPENSSL_INC = $(EXTERNAL_OPENSSL_BASE)/include
export EXTERNAL_OPENSSL_LIB = $(EXTERNAL_OPENSSL_BASE)/lib/libcrypto.a

ifeq ($(EXTERNAL_OPENSSL), 1)
	export CFLAGS += -DUSE_EXTERNAL_OPENSSL
	export CXXFLAGS += -DUSE_EXTERNAL_OPENSSL
	EXTERNAL_OPENSSL_LIB = $(EXTERNAL_OPENSSL_BASE)/lib/libcrypto.a
	ifeq ($(BCM_PARTIAL_CRYPTO),1)
		CFLAGS += -I$(EXTERNAL_OPENSSL_INC) -I$(EXTERNAL_OPENSSL_INC)/openssl -I$(SRCBASE)/include/bcmcrypto
	else
		CFLAGS += -I$(EXTERNAL_OPENSSL_INC) -I$(EXTERNAL_OPENSSL_INC)/openssl
	endif
else
	CFLAGS += -I$(SRCBASE)/include/bcmcrypto
	EXTERNAL_OPENSSL_LIB =
endif
CFLAGS += -I$(SRCBASE)/wps/common/include
endif

CFLAGS += -DWLPFN
CFLAGS += -DWLC_HIGH
CFLAGS += -I$(SRCBASE)/wl/sys
CFLAGS += -I$(SRCBASE)/wl/exe
#CFLAGS += -I$(SRCBASE)/wps/common/include

CFLAGS += -fPIC

vpath %.c $(addprefix $(SRCBASE)/,shared wl/sys wl/exe wps/linux/enr wps/common/enrollee) $(WLAN_ComponentSrcDirs)

#all: wps_enr $(BWL_EXE)
#all: $(BWL_TARGETS)

ifdef SHARED_LIB
CFLAGS += -DBUILD_SHARED_LIB -DBCMSUP_PSK
all: $(WPS_TARGET) $(BWL_LIB)
else
all: $(WPS_TARGET) $(BWL_EXE) 
endif

$(BWL_LIB): $(WLU_OBJS) $(WPS_OBJS) $(BWL_OBJS)
	${CC} -shared $(GCDEFS) -o $@  $^ ${LDFLAGS}

$(BWL_EXE): $(WLU_OBJS) $(WPS_OBJS) $(BWL_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(WPS_LIBS) $(EXTERNAL_OPENSSL_LIB)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)
	$(CC) -c $(CFLAGS) -o $@ $^

wps_enr:
	make -f bwl_wps_libs.mk

bwl_ver:
	make -f MakeVersion

clean:
	rm -f $(BWL_EXE) $(BWL_OBJS) $(WLU_OBJS) $(WPS_OBJS)
	rm -rf $(OBJDIR)
	make -f bwl_wps_libs.mk clean

.PHONY: all clean

endif # TARGETENV linux


endif # UNAME
