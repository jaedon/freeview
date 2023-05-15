############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP ?= $(shell cd ../../../../../../nexus && cd)
else
NEXUS_TOP ?= $(shell cd ../../../../../../nexus; pwd)
endif

NEXUS_PLATFORM ?= $(PLATFORM)
ifndef NEXUS_PLATFORM
$(error Neither NEXUS_PLATFORM or PLATFORM is defined)
endif

# include cross-compiler definitions
include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc

V3D_DIR ?= $(NEXUS_TOP)/../rockford/middleware/v3d
V3D_PLATFORM_DIR ?= $(NEXUS_TOP)/../rockford/middleware/platform

LDFLAGS = -lm -lrt -lstdc++

CFLAGS += -I$(V3D_DIR)/interface/khronos/include/ \
		  -I$(V3D_PLATFORM_DIR)/nexus \
		  -I$(NEXUS_TOP)/../rockford/middleware/platform/nexus \
		  -I$(NEXUS_TOP)/../BSEAV/lib/libpng \
		  -I$(NEXUS_TOP)/../BSEAV/lib/zlib

APPS = benchmark

.PHONY: api clean

all: $(APPS)
$(APPS): api

ifdef DEBUG
B_REFSW_DEBUG ?= $(DEBUG)
endif

ifeq ($(B_REFSW_DEBUG),)
B_REFSW_DEBUG = y
endif

ifeq ($(B_REFSW_DEBUG),y)
V3D_LIB := lib_$(NEXUS_PLATFORM)_debug
else
V3D_LIB := lib_$(NEXUS_PLATFORM)_release
endif

NEXUS_MODE ?= $(MODE)
ifeq ($(NEXUS_MODE),)
CFLAGS += -DSINGLE_PROCESS
endif

ifeq ($(NEXUS_MODE),proxy)
ifeq ($(CLIENT),)
CFLAGS += -DSINGLE_PROCESS
endif
endif

ifeq ($(NEXUS_MODE),client)
CFLAGS += -DUSER_MODE_MULTIPROC_CLIENT
endif

# For linux builds, link to the correct libraries
ifneq ($(findstring linux,$(B_REFSW_OS)),)
LDFLAGS += -lnexus$(NEXUS_LIB_SUFFIX) -L${NEXUS_BIN_DIR} -lpthread \
		-L$(V3D_DIR)/$(V3D_LIB) -lv3ddriver \
		-L$(V3D_PLATFORM_DIR)/nexus/$(V3D_LIB) -lnxpl \
		-L$(NEXUS_TOP)/../BSEAV/lib/libpng -lpng \
		-L$(NEXUS_TOP)/../BSEAV/lib/zlib/$(B_REFSW_ARCH) -lz
endif

# This builds the nexus api, v3d driver & platform glue
api:
	$(MAKE) -C $(NEXUS_TOP)/build B_REFSW_DEBUG=$(B_REFSW_DEBUG)
	$(MAKE) -C $(V3D_DIR) -f V3DDriver.mk B_REFSW_DEBUG=$(B_REFSW_DEBUG)
	$(MAKE) -C $(V3D_PLATFORM_DIR)/nexus -f platform_nexus.mk B_REFSW_DEBUG=$(B_REFSW_DEBUG)
	$(MAKE) -C $(NEXUS_TOP)/../BSEAV/lib/zlib
	$(MAKE) -C $(NEXUS_TOP)/../BSEAV/lib/libpng

.PHONY: clean_api clean_v3d clean_app
# This cleans nexus and local apps
clean: clean_api clean_v3d clean_app clean_png

clean_api:
	$(MAKE) -C $(NEXUS_TOP)/build B_REFSW_DEBUG=$(B_REFSW_DEBUG) clean
	
clean_v3d:
	$(MAKE) -C $(V3D_DIR) -f V3DDriver.mk B_REFSW_DEBUG=$(B_REFSW_DEBUG) clean
	$(MAKE) -C $(V3D_PLATFORM_DIR)/nexus -f platform_nexus.mk B_REFSW_DEBUG=$(B_REFSW_DEBUG) clean
	
clean_app:
	-$(RM) $(APPS) *.d *.o *.out;

clean_png:
	$(MAKE) -C $(NEXUS_TOP)/../BSEAV/lib/zlib clean
	$(MAKE) -C $(NEXUS_TOP)/../BSEAV/lib/libpng clean

# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES)) -I$(NEXUS_TOP)/../BSEAV/lib/netaccel/include

# Always build with debug
CFLAGS += -g

SOURCES := \
	esutil.c \
	load_texture.c \
	../common/init.c \
	benchmark.cpp

# $(1) = src
# $(2) = obj
define CCompileRule
OBJECTS += $(2)
$(2) : $(1)
	@echo [Compiling... $(1)]
	@$(CC) $(CFLAGS) -c -o "$(2)" "$(1)"

endef

REMOVEME = -std=c89
CXXFLAGS = $(filter-out $(REMOVEME),$(CFLAGS)) 

# $(1) = src
# $(2) = obj
define CPPCompileRule
OBJECTS += $(2)
$(2) : $(1)
	@echo [Compiling... $(1)]
	@$(CC) $(CXXFLAGS) -std=c++98 -c -o "$(2)" "$(1)"

endef

$(foreach src,$(filter %.c,$(SOURCES)),$(eval $(call CCompileRule,$(src),$(basename $(notdir $(src))).o)))
$(foreach src,$(filter %.cpp,$(SOURCES)),$(eval $(call CPPCompileRule,$(src),$(basename $(notdir $(src))).o)))

# Implicit rule for building local apps
$(APPS) : $(OBJECTS)
	@echo [Link... $@]
	@$(CC) $(filter %.cpp %.c %.s %.o, $^) $(LDFLAGS) -o $@
	cp -af $@ ${NEXUS_BIN_DIR}
	cp -af *.txt ${NEXUS_BIN_DIR}
	cp -af earth_cloud0.pkm ${NEXUS_BIN_DIR}
	@echo -e "\n\n*********************************************************************************"
	@echo "You can run the benchmark with './nexus benchmark batch=depth.txt +s d=1280x720'"
	@echo "or './nexus benchmark batch=nodepth.txt +s d=1280x720 -depth -stencil'"
	@echo "*********************************************************************************"


