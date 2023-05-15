ifeq ($(STATIC_SETTOPAPI),y)
SHAREDLIB = no
STATICLIB = yes
LDFLAGS += -static
else
STATICLIB = no
endif

# Nexus uses MEDIA_ASF_SUPPORT MEDIA_AVI_SUPPORT,brutus needs ASF_SUPPORT and AVI_SUPPORT flags
ifeq ($(MEDIA_ASF_SUPPORT),y)
ASF_SUPPORT=y
endif

ifeq ($(MEDIA_AVI_SUPPORT),y)
AVI_SUPPORT=y
endif

#Nexus uses MSDRM_PD_SUPPORT and MSDRM_ND_SUPPORT
ifeq ($(MSDRM_PD_SUPPORT),y)
DRM_SUPPORT=y
endif

ifeq ($(MSDRM_ND_SUPPORT),y)
DRMND_SUPPORT=y
endif


ifeq ($(ASF_SUPPORT), y)
ifeq ($(DRM_SUPPORT), y)
DRM_BIN_DIR = $(BSEAV)/lib/drm/wmdrmpd10/bin
LDFLAGS += $(DRM_BIN_DIR)/oem_ansi.a    \
	$(DRM_BIN_DIR)/drmmanager.a  	\
	$(DRM_BIN_DIR)/drmcore.a     	\
	$(DRM_BIN_DIR)/drmpkcrypto.a 	\
	$(DRM_BIN_DIR)/drmbignum.a   	\
	$(DRM_BIN_DIR)/drmdevcert.a  	\
	$(DRM_BIN_DIR)/drmblackbox.a 	\
	$(DRM_BIN_DIR)/drmcrypto.a   	\
        $(DRM_BIN_DIR)/drmtoolscommon.a \
	$(DRM_BIN_DIR)/drmdevicedevcert.a
endif
ifeq ($(DRMND_SUPPORT), y) 
DRMND_LIB_DIR = $(BSEAV)/lib/drmnd/wmdrmnd10
LDFLAGS += $(DRMND_LIB_DIR)/bin/drmndport.a	\
	$(DRMND_LIB_DIR)/oem/ansi/oem_ansi.a
endif
endif

include ${BSEAV}/api/build/tools.mak

.PHONY:	settop settop-clean api-clean

all: check_environment settop application

include ${BSEAV}/api/include/api.mak

CFLAGS += -Wall -W -g
CFLAGS += ${BSETTOP_CFLAGS} -I${BSETTOP}/utils/common
CFLAGS += ${B_REFSW_GENERIC_MAGNUM_DEFINES}
CFLAGS += ${B_REFSW_MAGNUM_INCLUDE_DIRS}
LDFLAGS += ${BSETTOP_LDFLAGS}

ifeq ($(SYSTEM),vxworks)
# The default output directory is the current directory.
# For some reason, the vxworks 'make' stops execution when mkdir tries to create "./" even if the command is preceeded 
# with a dash but doesn't error if its not "./" (I don't know why). Just use something else.
ifeq ($(ODIR),)
ODIR = ../vxworks_bin
endif
endif

OBJS = \
	$(BSETTOP)/utils/common/bargs.o \
	$(BSETTOP)/utils/common/b_namevalue_pairs.o \
	$(BSETTOP)/utils/common/bsettop_namevalue_pairs.o \
	$(BSETTOP)/utils/common/btune.o

ifeq ($(SYSTEM),vxworks)
# Create a main wrapper function (go) that parses the command string and passes the arguments to main correctly.
OBJS += $(BSETTOP)/utils/common/vxworks_cmd.o
endif

LIBS = ${BSETTOP_LIBS}
LDFLAGS += ${BSETTOP_LIB}

settop:
	@$(MAKE) -C ${BSETTOP}/build SHAREDLIB=$(SHAREDLIB) STATICLIB=$(STATICLIB)

api-clean: settop-clean

settop-clean:
	@$(MAKE) -C ${BSETTOP}/build clean


.NOTPARALLEL : application settop

DESTDIR ?= ${BSEAV}/bin
.PHONY: install
install: settop application
	${CP} ${BSETTOP_LIBS} ${APP} ${DESTDIR}


