############################################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: tools.inc $
# $brcm_Revision: Hydra_Software_Devel/14 $
# $brcm_Date: 9/19/12 4:17p $
#
# Module Description: Tools include based on David Erikson's settop api
#                     tools.mak file.
#
# Revision History:
#
# $brcm_Log: /rockford/appframework/build/tools.inc $
# 
# Hydra_Software_Devel/14   9/19/12 4:17p katrep
# SW7445-45:revise for 7445

# Hydra_Software_Devel/13   9/14/12 10:18a katrep
# SW7445-45:revise for 7445
# 
# Hydra_Software_Devel/12   3/15/11 3:57p darnstein
# SW7425-164: Adapt to changes in the "plat" script.
# - Change DEBUG to B_REFSW_DEBUG.
# - Change ARCH to B_REFSW_ARCH.
# - Change OS to B_REFSW_OS.
# - Change mips-linux-uclibc to mips-linux.
# - Change mipsel-linux-uclibc to mipsel-linux.
# 
# Hydra_Software_Devel/11   9/7/10 3:44p hongtaoz
# SW7425-10: fixed emulation build; formatted the texts for easier
# reading;
# 
# Hydra_Software_Devel/10   4/14/10 2:32p pntruong
# SW7125-253: Need to handle big endian as well.
# 
# Hydra_Software_Devel/10   4/14/10 1:59p pntruong
# SW7125-253: Need to handle big endian as well.
#
# Hydra_Software_Devel/9   4/2/10 6:08p mward
# SW7125-253:  stbgcc-4.4.3-1.0 has mipsel-linux-uclibc-gcc not  mipsel-
# uclibc-gcc.
#
# Hydra_Software_Devel/8   3/24/10 11:20a erickson
# SW7405-4104: remove comment about g++ GPL contamination
#
# Hydra_Software_Devel/7   1/20/10 10:23a kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
#
# Hydra_Software_Devel/SW35230-56/1   1/18/10 6:06p kaiz
# SW35230-56: adding linuxpal support for Palladium emulation environment
#
# Hydra_Software_Devel/6   6/22/09 4:36p jessem
# PR 54380, PR 54881: Added linuxemu support.
#
# Hydra_Software_Devel/5   10/16/07 4:14p jessem
# PR 36129: Changed ARCH to /tools/bin for linuxclient.
#
# Hydra_Software_Devel/4   3/31/07 5:47p shyam
# PR 29285 : Added linuxclient support for simulations/emulation
#
# Hydra_Software_Devel/3   4/10/06 10:46a jessem
# PR18367: Added VxWorks support.
#
# Hydra_Software_Devel/2   9/15/05 10:15a jasonh
# PR 16967: Defining ARCH when not set
#
# Hydra_Software_Devel/1   1/19/05 4:26p jasonh
# PR 13863: Upgraded version of application framework.
#
# Hydra_Software_Devel/1   12/9/03 2:35p frusso
# PR8916: initial checkin
#
###########################################################################

# Handle the case when B_REFSW_ARCH is not defined

ifeq ($(SYSTEM),vxworks)
	ifndef B_REFSW_ARCH
		B_REFSW_ARCH = mips-vxworks
	endif
	
	# Defines macros for cross-development tools
	CROSS_COMPILE = mips
	CC      = cc${CROSS_COMPILE}
	CXX     = c++${CROSS_COMPILE}
	LD      = ld${CROSS_COMPILE}
	AR      = ar${CROSS_COMPILE}
	NM      = nm${CROSS_COMPILE}
	STRIP   = strip${CROSS_COMPILE}
	OBJCOPY = objcopy${CROSS_COMPILE}
	OBJDUMP = objdump${CROSS_COMPILE}
	RANLIB  = ranlib${CROSS_COMPILE}
	MKDIR   = mkdir
	PWD     = cd
	MV      = move

else

	ifeq ($(SYSTEM),linuxemu)

		# Running on the build server use the native compiler.
		B_REFSW_ARCH = host
		CROSS_COMPILE = /tools/bin/

	else

		ifeq ($(SYSTEM),linuxpal)
			# Running on the build server use the native compiler.
			B_REFSW_ARCH = host
			CROSS_COMPILE = /tools/oss/packages/i686-rhel5/gcc/4.2.1/bin/
		else

			ifndef B_REFSW_ARCH
				B_REFSW_ARCH = mipsel-linux
			endif

			# Defines macros for cross-development tools
			TS_DIR:=$(shell dirname $(shell dirname $(shell which mipsel-linux-uclibc-gcc 2>/dev/null) 2>/dev/null))

			ifeq ($(TS_DIR),)
				CROSS_COMPILE ?= $(B_REFSW_ARCH)-
			else
				ifneq ($(findstring $(B_REFSW_ARCH), mips-vxworks mips-linux),)
					CROSS_COMPILE = mips-linux-
				else
					CROSS_COMPILE = mipsel-linux-
				endif
			endif

		endif
	endif


	AS		= $(CROSS_COMPILE)as
	LD		= $(CROSS_COMPILE)ld
	CC		= $(CROSS_COMPILE)gcc
	CCPP	= $(CROSS_COMPILE)c++
	CPP		= $(CC) -E
	AR		= $(CROSS_COMPILE)ar
	NM		= $(CROSS_COMPILE)nm
	STRIP	= $(CROSS_COMPILE)strip
	OBJCOPY	= $(CROSS_COMPILE)objcopy
	OBJDUMP	= $(CROSS_COMPILE)objdump
	RANLIB	= $(CROSS_COMPILE)ranlib

	MKDIR   = mkdir -p
	PWD     = pwd
	MV      = mv
endif

# These are operations common to all environments.
CP      = cp -f
RM      = rm -f
SORT    = sort
SED     = sed
TOUCH   = touch
DIR		= dir

