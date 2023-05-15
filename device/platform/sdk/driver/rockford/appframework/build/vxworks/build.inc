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
# $brcm_Revision: Hydra_Software_Devel/4 $
# $brcm_Date: 3/15/11 3:57p $
#
# File Description:
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/build/vxworks/build.inc $
# 
# Hydra_Software_Devel/4   3/15/11 3:57p darnstein
# SW7425-164: Adapt to changes in the "plat" script.
# - Change DEBUG to B_REFSW_DEBUG.
# - Change ARCH to B_REFSW_ARCH.
# - Change OS to B_REFSW_OS.
# - Change mips-linux-uclibc to mips-linux.
# - Change mipsel-linux-uclibc to mipsel-linux.
# 
# Hydra_Software_Devel/3   6/13/08 11:30a jessem
# PR 38765: Moved FRAMEWORK_SOURCES to framework.inc.
# 
# Hydra_Software_Devel/2   6/20/06 11:04a maivu
# PR 22212: Moved framework sources out of framework.inc
# 
# Hydra_Software_Devel/1   4/11/06 1:38p jessem
# PR18367: Intial version.
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

CFLAGS += -Wall -W -G 0 -mno-branch-likely -mips3 -EB -fno-builtin -DMIPSEB -DCPU=MIPS64 -I$(WIND_BASE)/target/h/ -I$(WIND_BASE)/target/h/types -DTOOL_FAMILY=gnu -DTOOL=sfgnu   
CFLAGS += $(addprefix -I${FRAMEWORK_DIR}/, ${FRAMEWORK_INCLUDES})
CFLAGS += $(MAGNUM_CFLAGS) $(addprefix -I${MAGNUM_DIR}/, ${MAGNUM_INCLUDES})
CFLAGS += $(COMMON_FLAGS) $(addprefix -I, ${COMMON_INCLUDES})
CFLAGS += $(APP_CFLAGS) $(addprefix -I, ${APP_INCLUDES})
LDFLAGS += $(FRAMEWORK_LDFLAGS)	$(APP_LDFLAGS)

SRC += $(addprefix $(MAGNUM_DIR)/, $(MAGNUM_SOURCES))
SRC += $(COMMON_SOURCES)

SRC += $(addprefix $(FRAMEWORK_DIR)/src/$(TARGET_OS)/ $(FRAMEWORK_DIR)/src/common/appframework/ $(FRAMEWORK_DIR)/src/board/$(PLATFORM)/, $(FRAMEWORK_SOURCES))

# All all magnum and framework directories to C file search path
vpath %.c $(dir $(SRC))
SRC_NODIR = $(notdir $(SRC))

# Determine prefix of objects and executable directories
ifeq (${B_REFSW_DEBUG},y)
PREFIX_DIR = $(PLATFORM).$(BCHP_REV).debug
CFLAGS += -g
LDFLAGS += -g -r -nostdlib -L$(WIND_BASE)\host\x86-win32\mips-wrs-vxworks\lib\MIPS64gnu -nostdlib -r -Wl,-EB -Wl,-X $(WIND_BASE)\target\lib\mips\MIPS64\gnu\libgnucplus.a
else
PREFIX_DIR = $(PLATFORM).$(BCHP_REV)
endif

# Location of magnum and framework objects
ODIR = $(FRAMEWORK_DIR)/build/vxworks/$(PREFIX_DIR)

# Name of application that is parameter specific
APP_CHIP = $(PREFIX_DIR)/$(APP)

.PHONY: ${APP_CHIP} clean
all: $(APP_CHIP)

OBJ = $(APP_SOURCES:%.c=$(PREFIX_DIR)/%.o) $(addprefix $(ODIR)/, $(SRC_NODIR:%.c=%.o))

ifneq (${VERBOSE},y)
Q_ = @
endif

$(APP_CHIP): $(ODIR) $(PREFIX_DIR) $(OBJ)
	@echo Compiling $@
ifeq ($(SYSTEM), vxworks)
	${Q_}$(CC) -o $@ $(CFLAGS) $(OBJ) $(LDFLAGS)
else
	${Q_}$(CC) -o $@ $(CFLAGS) $(OBJ) -lpthread $(LDFLAGS)
endif

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
	$(RM) $(OBJ) $(APP_CHIP)
