#############################################################################
#
#       Copyright (c) 2005-2011, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
# 
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: build.inc $
# $brcm_Revision: Hydra_Software_Devel/2 $
# $brcm_Date: 3/15/11 3:57p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/build/linuxpal/build.inc $
# 
# Hydra_Software_Devel/2   3/15/11 3:57p darnstein
# SW7425-164: Adapt to changes in the "plat" script.
# - Change DEBUG to B_REFSW_DEBUG.
# - Change ARCH to B_REFSW_ARCH.
# - Change OS to B_REFSW_OS.
# - Change mips-linux-uclibc to mips-linux.
# - Change mipsel-linux-uclibc to mipsel-linux.
# 
# Hydra_Software_Devel/1   1/20/10 10:07a kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
# 
# Hydra_Software_Devel/SW35230-56/1   1/18/10 6:01p kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
#  
#############################################################################

include $(FRAMEWORK_DIR)/build/tools.inc
include $(FRAMEWORK_DIR)/build/framework.inc

# Determine list of all magnum sources and includes
MAGNUM_SOURCES = $(foreach module, $(MAGNUM_MODULES), $(${module}_SOURCES))
MAGNUM_INCLUDES = $(foreach module, $(MAGNUM_MODULES), $(${module}_INCLUDES))

# For computing file dependencies
CDEP_FLAG = -MD

CFLAGS += -Wall -W -Wstrict-prototypes -pedantic
CFLAGS += $(addprefix -I${FRAMEWORK_DIR}/, ${FRAMEWORK_INCLUDES})
CFLAGS += $(MAGNUM_CFLAGS) $(addprefix -I, ${MAGNUM_INCLUDES})
CFLAGS += $(COMMON_FLAGS) $(addprefix -I, ${COMMON_INCLUDES})
CFLAGS += $(APP_CFLAGS) $(addprefix -I, ${APP_INCLUDES})
CFLAGS += $(FRAMEWORK_CFLAGS)
CFLAGS += $(addprefix -D, $(foreach module, $(MAGNUM_MODULES), $($(module)_DEFINES)))
LDFLAGS += $(FRAMEWORK_LDFLAGS)	$(APP_LDFLAGS)

SRC += $(MAGNUM_SOURCES)
SRC += $(addprefix $(FRAMEWORK_DIR)/src/$(TARGET_OS)/ $(FRAMEWORK_DIR)/src/common/appframework/ $(FRAMEWORK_DIR)/src/board/$(PLATFORM)/, $(FRAMEWORK_SOURCES))
SRC += $(COMMON_SOURCES)

# All all magnum and framework directories to C file search path
vpath %.c $(dir $(SRC))
SRC_NODIR = $(notdir $(SRC))

# Determine prefix of objects and executable directories
ifeq (${B_REFSW_DEBUG},y)
PREFIX_DIR = $(PLATFORM).$(BCHP_VER).debug
CFLAGS += -g
LDFLAGS += -g
else
PREFIX_DIR = $(PLATFORM).$(BCHP_VER)
endif

# Location of magnum and framework objects
ODIR = $(FRAMEWORK_DIR)/build/$(TARGET_OS)/$(PREFIX_DIR)

# Name of application that is parameter specific
APP_CHIP = $(PREFIX_DIR)/$(APP)

.PHONY: ${APP_CHIP}
all: $(APP_CHIP)

OBJ = $(APP_SOURCES:%.c=$(PREFIX_DIR)/%.o) $(addprefix $(ODIR)/, $(SRC_NODIR:%.c=%.o))
DEP = $(OBJ:%.o=%.d)

ifneq (${VERBOSE},y)
Q_ = @
endif

$(APP_CHIP): $(ODIR) $(PREFIX_DIR) $(OBJ)
	@echo Compiling $@
	${Q_}$(CC) -o $@ $(CFLAGS) $(OBJ) -lpthread $(LDFLAGS)

$(ODIR)/%.o : %.c
	@echo Compiling $@
	${Q_}$(CC) ${CDEP_FLAG} ${CFLAGS_STATIC} $(CFLAGS) -c $< -o $@

$(PREFIX_DIR)/%.o : %.c
	@echo Compiling $@
	${Q_}$(CC) ${CDEP_FLAG} ${CFLAGS_STATIC} $(CFLAGS) -c $< -o $@

$(ODIR):
	$(MKDIR) $(ODIR)

$(PREFIX_DIR):
	$(MKDIR) $(PREFIX_DIR)

clean:
	${Q_} $(RM) $(OBJ) $(DEP) $(APP_CHIP)

-include $(DEP)
