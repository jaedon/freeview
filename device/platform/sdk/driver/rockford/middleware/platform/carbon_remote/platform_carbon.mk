# make the Nexus platform layer
#
BUILD_DYNAMIC ?= 0

NEXUS_TOP:=../../../../nexus
NEXUS_PLATFORM:=$(PLATFORM)
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

CFLAGS += \
	-fpic -DPIC \
	-I. \
	-I../../v3d/interface/khronos/include \
	-DBCHP_CHIP=$(BCHP_CHIP) \
	-DV3D_HAS_ENDIAN_CONTROL

CFLAGS += -c $(foreach dir,$(NEXUS_APP_INCLUDE_PATHS),-I$(dir)) $(foreach def,$(NEXUS_APP_DEFINES),-D"$(def)")

include ../platform_common.inc
CFLAGS += $(COMMON_PLATFORM_CFLAGS)

ifndef B_REFSW_ARCH
B_REFSW_ARCH = mipsel-uclibc
endif

ifeq ($(filter ${B_REFSW_ARCH}, mips-linux mips-uclibc mips-linux-uclibc), ${B_REFSW_ARCH})
CFLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_BIG
CC = mips-linux-uclibc-gcc
C++ = mips-linux-uclibc-g++
else
CFLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE
CC = mipsel-linux-uclibc-gcc
C++ = mipsel-linux-uclibc-g++
endif

ifeq ($(DEBUG),y)
CFLAGS += -O0 -g
OBJDIR = obj_$(PLATFORM)_debug
LIBDIR = lib_$(PLATFORM)_debug
else
CFLAGS += -Os -DNDEBUG
OBJDIR = obj_$(PLATFORM)_release
LIBDIR = lib_$(PLATFORM)_release
endif

LDFLAGS = -s -lpthread

SOURCES =	default_carbon.c \
			../nexus/memory_nexus.c \
			../nexus/display_nexus.c \
			hardware_carbon.c \
			carbon_wrapper.c

ifeq ($(BUILD_DYNAMIC),1)
all: OUTDIR $(LIBDIR)/libcapl.so
else
all: OUTDIR $(LIBDIR)/libcapl.a
endif

.phony: OUTDIR
OUTDIR :
	@mkdir -p $(OBJDIR)

# $(1) = src
# $(2) = obj
define CCompileRule
OBJS += $(2)
$(2) : $(1)
	@echo Compiling $(1)
	@$(CC) -c $(CFLAGS) -o "$(2)" "$(1)"

endef

$(foreach src,$(SOURCES),$(eval $(call CCompileRule,$(src),$(OBJDIR)/$(basename $(notdir $(src))).o)))

$(LIBDIR)/libcapl.so: $(OBJS)
	@echo Linking ... libcapl.so
	@mkdir -p $(LIBDIR)
	@$(CC) $(LDFLAGS) -shared -o $(LIBDIR)/libcapl.so $(OBJS)

$(LIBDIR)/libcapl.a: $(OBJS)
	@echo Archiving ... libcapl.a
	@mkdir -p $(LIBDIR)
	@ar -rcs $(LIBDIR)/libcapl.a $(OBJS)

# clean out the dross
clean:
	rm -f $(LIBDIR)/libcapl.so *~ $(OBJS)
	rm -f $(LIBDIR)/libcapl.a
