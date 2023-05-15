############################################################################
#
#    Copyright (c) 2007, Broadcom Corporation
#    All Rights Reserved
#    Confidential Property of Broadcom Corporation
#
#    THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#    AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#    EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
#    $brcm_Workfile: Rules.make $
#    $brcm_Revision: 1 $
#    $brcm_Date: 10/30/07 3:41p $
#
#    Author:            Troy Trammel
#
#    Description:       DirectFB rules Makefile
#
#    Revision History:
#
#    $brcm_Log: $
############################################################################


##
#
# C source files
#
C_SRCS += $(addsuffix .c, $(notdir $(MAGNUM_OBJ)))


##
#
# C objects files
#
C_OBJS += $(C_SRCS:%.c=$(OBJECT_DIR)/%.o)


##
#
# VPATH for source code searching
#
vpath %.c $(sort $(dir $(MAGNUM_OBJ)))


##
#
# VPATH for source code searching
#
ifeq ($(DEBUG),n)
CFLAGS += -O3
endif
CFLAGS += $(MAGNUM_CFLAGS)


##
#
# Implicit C compilation rule
#
$(OBJECT_DIR)/%.o : %.c
	@echo Compiling $<
	$(CC) -MD $(CFLAGS) -c $< -o $@


##
#
# Static library rule
#
$(OBJECT_DIR)/$(STATIC_LIB): $(C_OBJS)
	@echo Linking $@
	if [ -e $@ ]; then \
		$(VERBOSE)$(RM) $@; \
	fi
	$(AR) crv $@ $(C_OBJS)
	$(RANLIB) $@


##
#
# Shared library rule
#
$(OBJECT_DIR)/$(SHARED_LIB): $(C_OBJS)
	@echo Linking $@
	$(CC) -shared -Wl,-x -o $@ -Wl,-soname,$(SHARED_LIB) $(C_OBJS)




