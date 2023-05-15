# make the DirectFB platform layer
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

ifndef B_REFSW_ARCH
B_REFSW_ARCH = mipsel-linux
endif

NEXUS_TOP ?= ../../../../nexus
NEXUS_PLATFORM:=$(PLATFORM)

V3D_DIR ?= $(NEXUS_TOP)/../rockford/middleware/v3d

include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

APPLIBS_TOP:=$(NEXUS_TOP)/../AppLibs

include $(APPLIBS_TOP)/opensource/directfb/build/directfb_common.inc

SINGLE := 0
ifeq ($(NEXUS_MODE),)
SINGLE := 1
endif

ifeq ($(NEXUS_MODE),proxy)
ifneq ($(CLIENT),y)
SINGLE := 1
endif
endif

CFLAGS += \
	-fpic -DPIC \
	-I. \
	-I$(V3D_DIR)/interface/khronos/include \
	-DBCHP_CHIP=$(BCHP_CHIP) \
	-I${DIRECTFB_INSTALL_DIRECTFB_INCLUDE_DIR} \
	-DDIRECTFB

ifeq ($(SINGLE),1)
CFLAGS += -DSINGLE_PROCESS
endif

CFLAGS += -c $(foreach dir,$(NEXUS_APP_INCLUDE_PATHS),-I$(dir)) $(foreach def,$(NEXUS_APP_DEFINES),-D"$(def)")

include ../platform_common.inc
CFLAGS += $(COMMON_PLATFORM_CFLAGS)

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
CFLAGS += -Os -DNDEBUG
LDFLAGS += -s
OBJDIR = obj_$(PLATFORM)_release
LIBDIR = lib_$(PLATFORM)_release
endif

LDFLAGS += -L$(V3D_DIR)/$(LIBDIR) -lv3ddriver

SOURCES = default_directfb.c			\
			../nexus/hardware_nexus.c	\
			../nexus/memory_nexus.c		\
			display_directfb.c			\
			message_queue.c

ifeq ($(BUILD_DYNAMIC),1)
all: $(LIBDIR)/libdbpl.so
else
all: $(LIBDIR)/libdbpl.a
endif

.phony: OUTDIR
OUTDIR :
	$(Q)mkdir -p $(OBJDIR)

.phony: V3DDriver
V3DDriver:
	$(Q)$(MAKE) --no-print-directory -C $(V3D_DIR) -f V3DDriver.mk $(MAKECMDGOALS)
	$(Q)cp $(V3D_DIR)/$(LIBDIR)/libv3ddriver.so $(NEXUS_BIN_DIR)

# Check to make sure DirectFB is already built
.phony: directfb_check
directfb_check:
	@if [ ! -d $(DIRECTFB_INSTALL_DIRECTFB_INCLUDE_DIR) ]; then \
		echo "ERROR: Cannot find directfb installation - aborting!"; \
		/bin/false; \
	fi

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

PRE_BUILD_RULES += directfb_check

ifneq ($(MAKECMDGOALS),clean)
$(foreach src,$(filter %.c,$(SOURCES)),$(eval $(call DependRule_C,$(src),$(OBJDIR)/$(basename $(notdir $(src))).d,\
              $(OBJDIR)/$(basename $(notdir $(src))).o)))

$(foreach src,$(filter %.c,$(SOURCES)),$(eval -include $(OBJDIR)/$(basename $(notdir $(src))).d))
endif

$(LIBDIR)/libdbpl.so: $(PRE_BUILD_RULES) V3DDriver $(OBJS)
	$(Q)echo Linking ... libdbpl.so
	$(Q)mkdir -p $(LIBDIR)
	$(Q)$(CC) $(LDFLAGS) -shared -o $(LIBDIR)/libdbpl.so $(OBJS)
	@install -m 775 -D $(LIBDIR)/libdbpl.so $(APPLIBS_TARGET_LIB_DIR)/libdbpl.so
	@install -m 775 -D $(NEXUS_BIN_DIR)/libv3ddriver.so $(APPLIBS_TARGET_LIB_DIR)/libv3ddriver.so

$(LIBDIR)/libdbpl.a: $(PRE_BUILD_RULES) V3DDriver $(OBJS)
	$(Q)echo Archiving ... libdbpl.a
	$(Q)mkdir -p $(LIBDIR)
	$(Q)ar -rcs $(LIBDIR)/libdbpl.a $(OBJS)

# clean out the dross
clean:
	$(Q)rm -f $(LIBDIR)/libdbpl.so *~ $(OBJS)
	$(Q)rm -f $(LIBDIR)/libdbpl.a
	$(Q)rm -f $(OBJDIR)/*.d
	$(Q)$(MAKE) --no-print-directory -C $(V3D_DIR) -f V3DDriver.mk clean
