############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP ?= $(shell cd ../../../../../../nexus && cd)
else
NEXUS_TOP ?= $(shell cd ../../../../../../nexus; pwd)
endif

ifndef PLATFORM
$(error PLATFORM is not defined)
endif

ifndef B_REFSW_ARCH
B_REFSW_ARCH = mipsel-linux
endif

V3D_DIR ?= $(NEXUS_TOP)/../rockford/middleware/v3d
V3D_PLATFORM_DIR ?= $(NEXUS_TOP)/../rockford/middleware/platform

# include cross-compiler definitions
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

APPLIBS_TOP:=$(NEXUS_TOP)/../AppLibs
include $(APPLIBS_TOP)/opensource/directfb/build/directfb_common.inc

THEAPPS = earth_es2_windowed

CFLAGS += -I$(V3D_DIR)/interface/khronos/include/ \
		  -I$(DIRECTFB_INSTALL_INCLUDE_DIR)/directfb \
		  -I$(V3D_PLATFORM_DIR)/directfb \
		  -I../../nexus/earth_es2 \
		  -DDATADIR=\"/usr/local/share/directfb-1.4.15/images\" \
		  -DFONTDIR=\"/usr/local/share/fonts\" \
		  -DFONT=\"/usr/local/share/fonts/decker.ttf\"

APPS = $(THEAPPS)

.PHONY: api directfb_check clean

ifeq ($(APP),)
all: $(APPS)
$(APPS): api
else
all: $(APP)
$(APP): api
endif

ifdef DEBUG
B_REFSW_DEBUG ?= $(DEBUG)
endif

ifeq ($(B_REFSW_DEBUG),)
B_REFSW_DEBUG = y
endif

ifeq ($(B_REFSW_DEBUG),y)
V3D_LIB := lib_$(PLATFORM)_debug
else
V3D_LIB := lib_$(PLATFORM)_release
endif

# For linux builds, link to the correct libraries
ifneq ($(findstring linux,$(B_REFSW_OS)),)
LDFLAGS := -lnexus -L${NEXUS_BIN_DIR} -lpthread -lm -lrt \
		-L$(V3D_DIR)/$(V3D_LIB) -lv3ddriver \
		-L$(V3D_PLATFORM_DIR)/directfb/$(V3D_LIB) -ldbpl \
		-L$(DIRECTFB_INSTALL_LIB_DIR) \
		-Wl,-rpath-link,$(DIRECTFB_INSTALL_LIB_DIR) \
		-ldirectfb
endif

# Check to make sure DirectFB is already built
directfb_check:
	@if [ ! -d $(DIRECTFB_INSTALL_DIRECTFB_INCLUDE_DIR) ]; then \
		echo "ERROR: Cannot find directfb installation - aborting!"; \
		/bin/false; \
	fi

# This builds the nexus api, v3d driver & platform glue
api: directfb_check
	$(MAKE) -C $(NEXUS_TOP)/build B_REFSW_DEBUG=$(B_REFSW_DEBUG)
	$(MAKE) -C $(V3D_DIR) -f V3DDriver.mk B_REFSW_DEBUG=$(B_REFSW_DEBUG)
	$(MAKE) -C $(V3D_PLATFORM_DIR)/directfb -f platform_directfb.mk B_REFSW_DEBUG=$(B_REFSW_DEBUG)

# This cleans nexus and local apps
clean:
	$(MAKE) -C $(NEXUS_TOP)/build B_REFSW_DEBUG=$(B_REFSW_DEBUG) clean
	$(MAKE) -C $(V3D_DIR) -f V3DDriver.mk B_REFSW_DEBUG=$(B_REFSW_DEBUG) clean
	$(MAKE) -C $(V3D_PLATFORM_DIR)/directfb -f platform_directfb.mk B_REFSW_DEBUG=$(B_REFSW_DEBUG) clean
	-$(RM) $(APPS) *.d *.o *.out;

# DFB has lots of warnings if not gnu99
# Always build with gnu99 & debug
CFLAGS += -std=gnu99 -g

EXTRA_OBJS := ../../nexus/cube/esutil.c ../earth_es2/load_texture.c

# Implicit rule for building local apps
%$(APPSUFFIX): %.c $(EXTRA_OBJS) $(VXOBJS)
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ $(filter %.c %.s %.o, $^) $(CFLAGS) $(LDFLAGS)
	@install -m 775 -D $@ $(APPLIBS_TARGET_BIN_DIR)/directfb/${DIRECTFB_MAJOR}.${DIRECTFB_MINOR}/$@; \
	mkdir -p $(APPLIBS_TARGET_DIR)/share/directfb-${DIRECTFB_MAJOR}.${DIRECTFB_MINOR}.${DIRECTFB_MICRO}/textures; \
	install -m 664 ../../nexus/earth_es2/textures/* $(APPLIBS_TARGET_DIR)/share/directfb-${DIRECTFB_MAJOR}.${DIRECTFB_MINOR}.${DIRECTFB_MICRO}/textures; \
	cd $(APPLIBS_TARGET_BIN_DIR)/directfb/${DIRECTFB_MAJOR}.${DIRECTFB_MINOR}; \
	ln -sf ../../../share/directfb-${DIRECTFB_VERSION}/textures

install: all

