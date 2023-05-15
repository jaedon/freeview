#############################################################################
#
#       Copyright (c) 2005-2012, Broadcom Corporation.
#       All rights reserved.
#       Confidential Property of Broadcom Corporation.
# 
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: build.inc $
# $brcm_Revision: Hydra_Software_Devel/28 $
# $brcm_Date: 9/19/12 4:05p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/build/linuxuser/build.inc $
# 
# Hydra_Software_Devel/28   9/19/12 4:05p katrep
# SW7445-45:updated for 7445
# 
# Hydra_Software_Devel/26   3/15/11 3:57p darnstein
# SW7425-164: Adapt to changes in the "plat" script.
# - Change DEBUG to B_REFSW_DEBUG.
# - Change ARCH to B_REFSW_ARCH.
# - Change OS to B_REFSW_OS.
# - Change mips-linux-uclibc to mips-linux.
# - Change mipsel-linux-uclibc to mipsel-linux.
# 
# Hydra_Software_Devel/25   5/25/10 11:58a spothana
# SW7420-770: vdc_test app failing to build with make APP_LDFLAGS=-static
# 
# Hydra_Software_Devel/24   10/21/08 1:16p jessem
# PR 47440: Added ${Q}  to clean rule.
# 
# Hydra_Software_Devel/23   6/20/08 10:51a jessem
# PR 43759: Removed VDB support.
# 
# Hydra_Software_Devel/22   6/18/08 1:05p jessem
# PR 43759: Added BVDB (Video Usage Modes Database)  support.
# 
# Hydra_Software_Devel/21   6/13/08 11:30a jessem
# PR 38765: Moved FRAMEWORK_SOURCES framework.inc. Added FRAMEWORK_CFLAGS
# to CFLAGS.
# 
# Hydra_Software_Devel/20   11/12/07 1:24p yuxiaz
# PR34524: Fix compile problem for xpt.
# 
# Hydra_Software_Devel/19   2/7/07 1:37p jasonh
# PR 27117: Updating to use module specific .inc files.
# 
# Hydra_Software_Devel/17   8/2/06 11:16a syang
# PR 22212: split board specific frame work out
# 
# Hydra_Software_Devel/16   7/7/06 2:12p syang
# PR 21846: Change -O3 to -O2 (which turns out optimize about the same,
# but easier to debug)
# 
# Hydra_Software_Devel/15   6/20/06 9:59a maivu
# PR 22212: Moved framework sources out of framework.inc
# 
# Hydra_Software_Devel/14   6/19/06 4:36p maivu
# PR 22212: Rearrange framework directories
# 
# Hydra_Software_Devel/13   6/13/06 4:55p syang
# PR 21846: change BCHP_REV to BCHP_VER
# 
# Hydra_Software_Devel/12   6/13/06 2:55p syang
# PR 21846: change gcc option mips2 to mips32
# 
# Hydra_Software_Devel/11   6/1/06 3:21p darnstein
# PR20430: Add dependency checking to the make system.
# 
# Hydra_Software_Devel/10   5/26/06 5:02p hongtaoz
# PR21846: moved optimization into release build;
# 
# Hydra_Software_Devel/9   5/26/06 3:03p syang
# PR 21846: added gcc option "-mips2 -O3"
# 
# Hydra_Software_Devel/8   4/18/06 1:56p darnstein
# PR18010: C compiler will now warn about unused function parameters.
# 
# Hydra_Software_Devel/7   4/13/06 2:32p syang
# PR 20748: change to search files in framework before commondriver and
# bsp
# 
# Hydra_Software_Devel/6   11/10/05 2:07p jasonh
# PR 18068: Updated directory structure.
# 
# Hydra_Software_Devel/5   9/22/05 11:37a jasonh
# PR 16967: Changed object and executable paths to include chip and
# revision information.
# 
# Hydra_Software_Devel/4   8/31/05 6:09p darnstein
# PR16924: Add -g flag when compiling.
# 
# Hydra_Software_Devel/3   8/31/05 5:22p jasonh
# PR 16943: Added support for COMMON sources, includes, and cflags
# 
# Hydra_Software_Devel/2   2/23/05 4:46p jasonh
# PR 13154: Adding APP_INCLUDES support.
# 
# Hydra_Software_Devel/1   1/19/05 5:02p jasonh
# PR 13863: Upgraded version of application framework.
# 
#############################################################################

include $(FRAMEWORK_DIR)/build/tools.inc
include $(FRAMEWORK_DIR)/build/framework.inc

# Determine list of all magnum sources and includes
MAGNUM_SOURCES = $(foreach module, $(MAGNUM_MODULES), $(${module}_SOURCES))
MAGNUM_INCLUDES = $(foreach module, $(MAGNUM_MODULES), $(${module}_INCLUDES))

# For computing file dependencies
CDEP_FLAG = -MD

CFLAGS += -Wall -W -Wstrict-prototypes -pedantic \
	-Wa,-32 -Wa,-march=mips32 -Wa,-mips32 -Wa,--trap
CFLAGS += -mips32 -O2
CFLAGS += $(addprefix -I${FRAMEWORK_DIR}/, ${FRAMEWORK_INCLUDES})
CFLAGS += $(MAGNUM_CFLAGS) $(addprefix -I, ${MAGNUM_INCLUDES})
CFLAGS += $(COMMON_FLAGS) $(addprefix -I, ${COMMON_INCLUDES})
CFLAGS += $(APP_CFLAGS) $(addprefix -I, ${APP_INCLUDES})
CFLAGS += $(addprefix -D, $(foreach module, $(MAGNUM_MODULES), $($(module)_DEFINES)))
CFLAGS += $(FRAMEWORK_CFLAGS)
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
	${Q_}$(CC) -o $@ $(CFLAGS) $(OBJ) -lpthread $(LDFLAGS) -lm

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
