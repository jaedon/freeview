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
# $brcm_Revision: Hydra_Software_Devel/9 $
# $brcm_Date: 3/15/11 3:57p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/build/linuxclient/build.inc $
# 
# Hydra_Software_Devel/9   3/15/11 3:57p darnstein
# SW7425-164: Adapt to changes in the "plat" script.
# - Change DEBUG to B_REFSW_DEBUG.
# - Change ARCH to B_REFSW_ARCH.
# - Change OS to B_REFSW_OS.
# - Change mips-linux-uclibc to mips-linux.
# - Change mipsel-linux-uclibc to mipsel-linux.
# 
# Hydra_Software_Devel/8   11/25/08 1:00p jessem
# PR 49655: Simplified PLATFORM variable use.
# 
# Hydra_Software_Devel/7   6/13/08 11:27a jessem
# PR 38765: Moved FRAMEWORK_SOURCES to framework.inc
# 
# Hydra_Software_Devel/6   4/30/08 10:00a rpan
# PR39807: Populate XXX_DEFINES from all the magnum modular make files.
# 
# Hydra_Software_Devel/5   2/7/08 6:01p jessem
# PR 39142: Added FRAMEWORK_CFLAGS to CFLAGS.
# 
# Hydra_Software_Devel/4   10/22/07 4:47p jessem
# PR 36129: Changed rule for sims to linuxclient and framework_client.c.
# 
# Hydra_Software_Devel/3   10/16/07 4:07p jessem
# PR 36129: Added support for IKOS (sims) emulation platform.
# 
# Hydra_Software_Devel/2   4/2/07 2:58p shyam
# PR 29285 : Added linuxclient support for simulations/emulation
# 
# Hydra_Software_Devel/1   3/31/07 10:52a shyam
# PR 29285 : Added linuxclient support for simulations/emulation
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
ifeq (${SYSTEM},linuxclient)
SRC += framework_client.c 
else
SRC += framework_board.c
endif
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
