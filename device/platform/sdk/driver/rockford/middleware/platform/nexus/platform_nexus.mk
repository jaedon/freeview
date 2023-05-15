# make the Nexus platform layer
#
BUILD_DYNAMIC ?= 1

ifdef DEBUG
B_REFSW_DEBUG ?= $(DEBUG)
endif

ifeq ($(B_REFSW_DEBUG),)
B_REFSW_DEBUG = y
endif

ifeq ($(VERBOSE),)
Q := @
endif

NEXUS_TOP ?= ../../../../nexus
NEXUS_PLATFORM:=$(PLATFORM)
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

V3D_DIR ?= $(NEXUS_TOP)/../rockford/middleware/v3d

CFLAGS += \
	-fpic -DPIC \
	-I. \
	-I$(V3D_DIR)/interface/khronos/include \
	-I$(NEXUS_TOP)/../BSEAV/lib/zlib \
	-I$(NEXUS_TOP)/../BSEAV/lib/libpng \
	-DBCHP_CHIP=$(BCHP_CHIP)

CFLAGS += -c $(foreach dir,$(NEXUS_APP_INCLUDE_PATHS),-I$(dir)) $(foreach def,$(NEXUS_APP_DEFINES),-D"$(def)")

include ../platform_common.inc
CFLAGS += $(COMMON_PLATFORM_CFLAGS)

ifndef B_REFSW_ARCH
B_REFSW_ARCH = mipsel-linux
endif

LDFLAGS = -lpthread

ifeq ($(filter ${B_REFSW_ARCH}, mips-linux mips-uclibc mips-linux-uclibc), ${B_REFSW_ARCH})
CFLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_BIG
else
CFLAGS += -DBSTD_CPU_ENDIAN=BSTD_ENDIAN_LITTLE
endif

CC = $(B_REFSW_ARCH)-gcc
C++ = $(B_REFSW_ARCH)-g++

ifeq ($(B_REFSW_DEBUG),y)
CFLAGS += -O0 -g
LDFLAGS += -g
OBJDIR = obj_$(PLATFORM)_debug
LIBDIR = lib_$(PLATFORM)_debug
else
CFLAGS += -g -Os -DNDEBUG
LDFLAGS += -g -s
OBJDIR = obj_$(PLATFORM)_release
LIBDIR = lib_$(PLATFORM)_release
endif

LDFLAGS += -L$(V3D_DIR)/$(LIBDIR) -lv3ddriver

SOURCES =	default_nexus.c \
			memory_nexus.c \
			hardware_nexus.c

SINGLE := 0
ifeq ($(NEXUS_MODE),)
SINGLE := 1
endif

ifeq ($(NEXUS_MODE),proxy)
ifneq ($(CLIENT),y)
SINGLE := 1
endif
endif


ifeq ($(SINGLE),0)

# Multi-process surface compositor client
SOURCES += surface_comp_display_nexus.c

else

CFLAGS += -DSINGLE_PROCESS

ifeq ($(NULL_DISPLAY),y)

# Null display
SOURCES += null_display_nexus.c

CFLAGS += -DNULL_DISPLAY
#LDFLAGS += -L$(NEXUS_TOP)/../BSEAV/lib/zlib -lz \
#			-L$(NEXUS_TOP)/../BSEAV/lib/libpng -lpng
else

# Single process exclusive/blit display
SOURCES += exclusive_display_nexus.c blit_display_nexus.c

endif # NULL_DISPLAY

endif # SINGLE

ifeq ($(BUILD_DYNAMIC),1)
all: $(LIBDIR)/libnxpl.so
else
all: $(LIBDIR)/libnxpl.a
endif

.phony: OUTDIR
OUTDIR :
	$(Q)mkdir -p $(OBJDIR)

.phony: V3DDriver
V3DDriver:
	$(Q)$(MAKE) --no-print-directory -C $(V3D_DIR) -f V3DDriver.mk $(MAKECMDGOALS)
	$(Q)cp $(V3D_DIR)/$(LIBDIR)/libv3ddriver.so $(NEXUS_BIN_DIR)

# $(1) = src
# $(2) = obj
define CCompileRule
OBJS += $(2)
$(2) : $(1)
	$(Q)echo Compiling $(1)
	$(Q)$(CC) -c $(CFLAGS) -o "$(2)" "$(1)"

endef

$(foreach src,$(SOURCES),$(eval $(call CCompileRule,$(src),$(OBJDIR)/$(basename $(notdir $(src))).o)))

# $(1) = src
# $(2) = d
# $(3) = obj
define DependRule_C
$(2) : $(1) | OUTDIR
	$(Q)echo Making depends for $(1)
	$(Q)touch $(2).tmp
	$(Q)$(CC) -D__SSE__ -D__MMX__ -M -MQ $(3) -MF $(2).tmp -MM $(CFLAGS) $(1)
	$(Q)sed 's/D:/\/\/D/g' < $(2).tmp | sed 's/C:/\/\/C/g' > $(2)
	$(Q)rm -f $(2).tmp

PRE_BUILD_RULES += $(2)
# Don't know why, but a comment on this line is necessary
endef

ifneq ($(MAKECMDGOALS),clean)
$(foreach src,$(filter %.c,$(SOURCES)),$(eval $(call DependRule_C,$(src),$(OBJDIR)/$(basename $(notdir $(src))).d,\
              $(OBJDIR)/$(basename $(notdir $(src))).o)))

$(foreach src,$(filter %.c,$(SOURCES)),$(eval -include $(OBJDIR)/$(basename $(notdir $(src))).d))
endif

$(LIBDIR)/libnxpl.so: $(PRE_BUILD_RULES) V3DDriver $(OBJS)
	$(Q)echo Linking ... libnxpl.so
	$(Q)mkdir -p $(LIBDIR)
	$(Q)$(CC) $(LDFLAGS) -shared -o $(LIBDIR)/libnxpl.so $(OBJS)
	$(Q)cp $(LIBDIR)/libnxpl.so $(NEXUS_BIN_DIR)

$(LIBDIR)/libnxpl.a: V3DDriver $(PRE_BUILD_RULES) $(OBJS)
	$(Q)echo Archiving ... libnxpl.a
	$(Q)mkdir -p $(LIBDIR)
	$(Q)ar -rcs $(LIBDIR)/libnxpl.a $(OBJS)

# clean out the dross
clean:
	$(Q)rm -f $(LIBDIR)/libnxpl.so *~ $(OBJS)
	$(Q)rm -f $(LIBDIR)/libnxpl.a
	$(Q)rm -f $(OBJDIR)/*.d
	$(Q)$(MAKE) --no-print-directory -C $(V3D_DIR) -f V3DDriver.mk clean
