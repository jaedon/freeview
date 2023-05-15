############################################################
#     Copyright (c) 2003-2012, Broadcom Corporation
#     All Rights Reserved
#     Confidential Property of Broadcom Corporation
#
#  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
#  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
#  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# $brcm_Workfile: bhsm_legacy.inc $
# $brcm_Revision: Hydra_Software_Devel/58 $
# $brcm_Date: 9/28/12 11:00a $
#
# Module Description:
#
# Revision History:
#
# Created: 01/09/2007 by Jon Garrett
#
# $brcm_Log: /magnum/portinginterface/hsm/bhsm_legacy.inc $
# 
# Hydra_Software_Devel/58   9/28/12 11:00a atruong
# SWSECURITY-175: Symbolic link removal for 7408 and 7550 HSM PI
# 
# Hydra_Software_Devel/57   9/20/12 11:03a atruong
# SWSECURITY-175: Bug fix - Removing Symbolic Link in HSM PI
# 
# Hydra_Software_Devel/56   9/14/12 9:26a atruong
# SWSECURITY-175: Remove symbolic links in HSM PI
# 
# Hydra_Software_Devel/55   7/28/11 11:49a cdisc
# SWDTV-8093: adding bhsm_misc.c to BHSM_SOURCES for 35xxx chips
# 
# Hydra_Software_Devel/54   4/11/11 6:08p cdisc
# SWDTV-6345: 35233 bring up
# 
# Hydra_Software_Devel/53   3/18/11 5:12p cdisc
# SWBLURAY-25150: adding AACS support
# 
# Hydra_Software_Devel/52   2/10/11 1:53p cdisc
# SWBLURAY-23691: missing endif
# 
# Hydra_Software_Devel/51   2/8/11 12:47p cdisc
# SWBLURAY-23691: adding 7640 specific mods
# 
# Hydra_Software_Devel/50   11/9/10 10:26a cdisc
# SW35125-13: adding hsm directory symlink to 35230
# 
# Hydra_Software_Devel/49   7/21/10 3:04p cdisc
# SW35230-581: adding bhsm_user_cmd to INCLUDES if chip is 35230
# 
# Hydra_Software_Devel/48   6/10/10 5:29p qcheng
# SW7550-413 : bhsm.inc prints warnings on every build
# 
# Hydra_Software_Devel/47   6/2/10 2:32p cdisc
# SW35230-51:   Magnum HSM PI needs to build for the 35230
# 
# Hydra_Software_Devel/46   3/4/10 3:28p qcheng
# SW7408-82 : Add security support for 7408 A0
# 
# Hydra_Software_Devel/45   12/9/09 12:13p atruong
# SW7550-102 : Add Security support for 7550
# 
# Hydra_Software_Devel/44   12/9/09 9:46a atruong
# SW7468-16: HSM PI Support for 7468A0
# 
# Hydra_Software_Devel/43   10/28/09 1:38a atruong
# SW7405-3298: Modification for ICAM Build
# 
# Hydra_Software_Devel/42   10/9/09 1:54a atruong
# SW7405-3173: New HSM PI and Nexus API function for reading OTP IDs
# 
# Hydra_Software_Devel/41   9/15/09 12:21a atruong
# SW7630-18: HSM PI Support for 7630B0
# 
# Hydra_Software_Devel/40   9/14/09 12:33a atruong
# SW7630-18: HSM PI Support for 7630B0
# 
# Hydra_Software_Devel/39   8/20/09 6:29p atruong
# PR55544: HSM PI Support For 7125A0
# 
# Hydra_Software_Devel/38   8/11/09 4:13p atruong
# PR55231: HSM PI Support For 7340A0
# 
# Hydra_Software_Devel/37   8/5/09 10:30a atruong
# PR55231: HSM PI Support for BCM7342A0
# 
# Hydra_Software_Devel/36   7/24/09 3:27p atruong
# PR56432: HSM PI Support for Secure RSA/Access cmds and FastCycling
# 
# Hydra_Software_Devel/35   4/3/09 11:07a yili
# PR53745:Added DS key routing to HSM
# 
# Hydra_Software_Devel/34   4/1/09 10:54a atruong
# PR53733: MS DRM Support Flag Added
# 
# Hydra_Software_Devel/33   3/31/09 5:46p atruong
# PR53733: Add MSDRM_PD_HWDECRYPT Support
# 
# Hydra_Software_Devel/32   3/30/09 1:02p atruong
# PR52979: Support for 7635A0
# 
# Hydra_Software_Devel/31   3/26/09 10:57a vsilyaev
# PR 53611: Use simply expanded variables everywhere where $(shell ...)
# is used. It prevents evaluation of $(shell ) every where variable is
# used
# 
# Hydra_Software_Devel/30   12/5/08 8:39p atruong
# PR50045: HSM PI Support for BCM7420 A0
# 
# Hydra_Software_Devel/29   11/13/08 2:44p atruong
# PR48869: Support HSM PI Raw Commands for BSP/Aegis
# 
# Hydra_Software_Devel/28   8/27/08 8:44a atruong
# PR45631: BCM97601A0 Bring Up
# 
# Hydra_Software_Devel/27   8/21/08 12:03p atruong
# PR45511: Add support for uScVal definition for newer chips (7400C0 and
# up)
# 
# Hydra_Software_Devel/26   8/12/08 4:27p atruong
# PR45631: HSM PI support for BCM7601 platform
# 
# Hydra_Software_Devel/25   4/9/08 11:05a atruong
# PR38948: HSM PI support for BCM3548
# PR39464: HSM PI support for BCM3556
# 
# Hydra_Software_Devel/24   2/13/08 3:53p qxie
# PR36907:Invalid use of CFLAGS in bhsm.inc
# 
# Hydra_Software_Devel/23   12/13/07 12:45p qxie
# PR38185: HSM: remove warning message in bhsm.inc
# 
# Hydra_Software_Devel/22   12/13/07 12:42p qxie
# PR 38185: HSM: remove warning message in bhsm.inc
# 
# Hydra_Software_Devel/21   12/6/07 10:26a qxie
# PR 37907: HSM: change  defaultBSECK=OFF
# 
# Hydra_Software_Devel/20   11/21/07 3:52p qxie
# PR 37479: HSM: change bhsm.inc to support BSECK for 7400 D0
# 
# Hydra_Software_Devel/19   11/3/07 9:25a lyang
# PR 35987: HSM to support BSECK, MS IPTV commands, SHA1 udpate
# 
# Hydra_Software_Devel/18   10/30/07 11:34a lyang
# PR36565: bhsm.inc spaces in ifeq cause trouble setting defaults
# elsewhere
# 
# Hydra_Software_Devel/17   10/24/07 3:36p lyang
# PR 35987: HSM to support BSECK, MS IPTV commands, SHA1 udpate
# 
# Hydra_Software_Devel/16   10/22/07 3:25p lyang
# PR 35987: HSM to support BSECK, MS IPTV commands, SHA1 udpate
# 
# Hydra_Software_Devel/15   10/22/07 1:06p lyang
# PR 35987: HSM to support BSECK, MS IPTV commands, SHA1 udpate
# 
# Hydra_Software_Devel/14   10/19/07 5:18p lyang
# PR 35987: HSM to support BSECK, MS IPTV commands, SHA1 udpate
# 
# Hydra_Software_Devel/13   9/26/07 2:59p lyang
# PR 34706: add support for 7400 D0 to portinginterface/hsm
# 
# Hydra_Software_Devel/12   8/28/07 5:19p lyang
# PR 33325: make RMX patch selectable for all chips; default on for those
# confirmed chips; OFF for others
# 
# Hydra_Software_Devel/11   5/9/07 4:06p lyang
# PR 27711: Add 7400B0, 7405A0 support to HSM. Purge and shared *.h
# source udpated
# 
# Hydra_Software_Devel/10   4/26/07 12:48p lyang
# PR 29703: Add HSM support for 7403 A1, 7118A1, Program secret update
# 
# Hydra_Software_Devel/9   4/25/07 12:43p lyang
# PR 29703: Add HSM support for 7403 A1, 7118A1, Program secret update
# 
# Hydra_Software_Devel/8   4/3/07 5:43p lyang
# PR 27763: HSM to add 2 new PIs to support 7440B0
# 
# Hydra_Software_Devel/7   4/3/07 5:40p lyang
# PR 27763: HSM to add 2 new PIs to support 7440B0
# 
# Hydra_Software_Devel/6   4/3/07 3:54p lyang
# PR 27763: HSM to add 2 new PIs to support 7440B0
# 
# Hydra_Software_Devel/5   3/5/07 9:17p lyang
# PR 27763: HSM to add a new PI to support 7440B0, version upper to lower
# casse
# 
# Hydra_Software_Devel/4   3/2/07 10:20p lyang
# PR 27763: HSM to add a new PI to support 7440B0
# 
# Hydra_Software_Devel/3   3/2/07 4:33p lyang
# PR 27763: HSM to add a new PI to support 7440B0
# 
# Hydra_Software_Devel/2   3/2/07 4:23p lyang
# PR 27763: HSM to add a new PI to support 7440B0.
# 
# Hydra_Software_Devel/1   1/11/07 3:37p jgarrett
# PR27004: Adding new .inc files
# 
############################################################

#-----------------------------------------------------------------------------------#
# HSM build-command-line-options:
#  CUST=G  or D  or E  or N or A  or GT  or NGNA (to turn on DCAS)  or S  or G or H to select customer mode
#  RMXPATCH=ON               to turn on RMX patch
#  SCVALUE=ON                to turn on scrambling ctrl bits setting with specified value
#  BHSM_REGIONVERIFY=ON      to turn on region verification with Generic mode  (invalid option for non-Generic mode build)
#  BHSM_BSECK=ON             to turn on/off BSECK feature 
#  BHSM_IPTV=ON              to turn on/off IPTV feature
#  BHSM_KEYLADDER=ON         to select spitted, protected secure PIs 
#  BHSM_MISC=ON              to select spitted, protected secure PIs 
#  BHSM_OTPMSP=ON            to select spitted, protected secure PIs 
#  BHSM_SECURE_RSA=ON        to turn on Secure RSA feature
#  BHSM_SECURE_ACCESS=ON     to turn on Secure Access (DRM Data Op) feature
#  BHSM_IP_LICENSING=ON      to turn on IP Licensing feature
#
#-----------------------------------------------------------------------------------#


#
# Conditional Options for this Makefile
#
# BHSM_CUST - HSM Specific Option.  This value will be provided for you if appropriate.
#

BCHP_VER_LOWER := $(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')

#
# Check for core architecture
#

ifeq ($(filter $(BCHP_CHIP), 7125 7340 7342 7420 7468), $(BCHP_CHIP))
CORETECH := core65nmASKM
endif

ifeq ($(filter $(BCHP_CHIP), 7325 7335 7336 7405), $(BCHP_CHIP))
CORETECH := core65nm
endif

ifeq ($(filter $(BCHP_CHIP), 7118 7400 7401 7403), $(BCHP_CHIP))
CORETECH := core130nm
endif

ifeq ($(filter $(BCHP_CHIP), 3548 3556 3563), $(BCHP_CHIP))
CORETECH := coreDTV
endif

ifeq ($(filter $(BCHP_CHIP), 7601 7630 7635 7640 ), $(BCHP_CHIP))
CORETECH := coreDVDBD
endif

ifeq ($(filter $(BCHP_CHIP), 7408 7550 35121 35125 35130 35230 35233), $(BCHP_CHIP))
CORETECH := coreNoBSP
endif


# Define this module
MAGNUM_MODULES += BHSM


# Definitions
BHSM_DEFINES += BHSM_AEGIS_SUPPORT

# Support uScValue definition for newer chip (7400C0 and later)
BHSM_DEFINES += BHSM_SC_VALUE

         


#-----------------------------------------------------------------------------------#
# old support "make CUST=E"
# space can NOT be put after 'E' in ifeq ($(CUST),  E),  yet can be put before 'E'

ifeq ($(CUST),E)
BHSM_CUST := E
endif


#-----------------------------------------------------------------------------------#
# 4/11/07 to add EchoStar build support 
# this is complicated, discard:  ifeq ($(findstring E, $(BHSM_CUST)), $(BHSM_CUST))

ifeq ($(BHSM_CUST),E)
# change itself from the upper case to lower for directory mapping

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_e.inc
endif



#-----------------------------------------------------------------------------------#
# old support "make CUST=D"
ifeq ($(CUST),D)
BHSM_CUST := D
endif

ifeq ($(BHSM_CUST),D)

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_d.inc
endif


#-----------------------------------------------------------------------------------#
# old support "make CUST=S"
ifeq ($(CUST),S)
BHSM_CUST := S
endif

ifeq ($(BHSM_CUST),S)

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_s.inc
endif


#-----------------------------------------------------------------------------------#
# old support "make CUST=T"
ifeq ($(CUST),T)
BHSM_CUST := T
endif

ifeq ($(BHSM_CUST),T)

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_t.inc
endif


#-----------------------------------------------------------------------------------#
# old support "make CUST=H"
ifeq ($(CUST),H)
BHSM_CUST := H
endif

ifeq ($(BHSM_CUST),H)

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_h.inc
endif


#-----------------------------------------------------------------------------------#
# old support "make CUST=M"
ifeq ($(CUST),M)
BHSM_CUST := M
endif

ifeq ($(BHSM_CUST),M)

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_m.inc
endif


#-----------------------------------------------------------------------------------#
# old support "make CUST=N"
ifeq ($(CUST),N)
BHSM_CUST := N
endif

ifeq ($(BHSM_CUST),N)

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_n.inc
endif

#-----------------------------------------------------------------------------------#
# old support "make CUST=A"
ifeq ($(CUST),A)
BHSM_CUST := A
endif

ifeq ($(BHSM_CUST),A)

BHSM_CUST_DIR = all
         
include $(MAGNUM)/portinginterface/hsm/bhsm_all_legacy.inc
endif


#-----------------------------------------------------------------------------------#
#  added 4/3/07 for DVD customer specific commands , narrow down for 7440B0 above only 
ifeq ($(BCHP_CHIP),7440)
ifneq ($(BCHP_VER_LOWER),a0)


ifeq ($(CUST),GT)    
BHSM_CUST := GT
endif

ifeq ($(BHSM_CUST),GT)         
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/include/${CORETECH}/$(BCHP_CHIP)/$(BCHP_VER_LOWER)/cust_toshiba
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/include/${CORETECH}/$(BCHP_CHIP)/$(BCHP_VER_LOWER)/cust_toshiba/bhsm_dvd_toshiba.c
BHSM_CUST := G
GREAT_GENERIC=true

endif


endif
endif


ifeq ($(findstring $(BCHP_CHIP), 7601 7635 7630), $(BCHP_CHIP))

ifeq ($(CUST),GT)    
BHSM_CUST := GT
endif

ifeq ($(BHSM_CUST),GT)         
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/include/${CORETECH}/$(BCHP_CHIP)/$(BCHP_VER_LOWER)/cust_toshiba
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/include/${CORETECH}/$(BCHP_CHIP)/$(BCHP_VER_LOWER)/cust_toshiba/bhsm_dvd_toshiba.c
BHSM_CUST := G
GREAT_GENERIC=true

endif


endif

#-----------------------------------------------------------------------------------#
#  added 4/25/07 for DCAS specific commands. No chip/version limit yet. may be added later

ifeq ($(CUST),NGNA)         
BHSM_CUST :=NGNA
endif


ifeq ($(BHSM_CUST),NGNA)
         
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/ngna
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/ngna/bhsm_dcas_cmds.c
GREAT_GENERIC=true

# not used here, for testApp build
BHSM_CUST_GENERICNGNA=1

endif




#-----------------------------------------------------------------------------------#
ifeq ($(CUST),G)
BHSM_CUST := G
endif
ifeq ($(BHSM_CUST),G)
GREAT_GENERIC=true
endif

ifeq ($(findstring G, $(BHSM_CUST)), $(BHSM_CUST))
BHSM_CUST := G
GREAT_GENERIC=true
endif

ifeq ($(findstring G, $(CUST)), $(CUST))
BHSM_CUST := G
GREAT_GENERIC=true
endif


#-----------------------------------------------------------------------------------#         
# Default BHSM_CUST to generic,   
# new support : "make BHSM_CUST=G"
#                     "make without touch of BHSM_CUST"
#	          "make CUST=X,  make BHSM_CUST=X,   X is anything but defined"
#ifeq ($(findstring G, $(BHSM_CUST)), $(BHSM_CUST))

ifeq ($(GREAT_GENERIC),true)
# this line has no effect if make BHSM_CUST=G :            BHSM_CUST := generic

# not used here, for testApp build
BHSM_CUST_GENERIC=1

BHSM_DEFINES += BSP_GENERIC

# 3/8/07, looks the platform or magnum does not add this definition
#CFLAGS += -DBSP_GENERIC

# always 
BHSM_CUST_DIR = generic

###Region_Verification support in Generic Mode, there is an option to add the Region_Verification or not
ifeq ($(BHSM_REGIONVERIFY),ON)
BHSM_REGIONVERIFY_MK=1
endif

ifeq ($(BHSM_REGIONVERIFY),OFF)
BHSM_REGIONVERIFY_MK=0
endif
         
ifeq ($(BHSM_REGIONVERIFY_MK),1)
BHSM_REG_VERIFY_FEATURE=1

#$(warning  ..........Region Verificationi Feature is built in with BHSM_REGIONVERIFY_MK= $(BHSM_REGIONVERIFY_MK) )
ifeq ($(CORETECH),coreDVDBD)
BHSM_INCLUDES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/${BCHP_CHIP}/region_verify \
			$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/${BCHP_CHIP}/region_verify/aegis
	
BHSM_SOURCES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/${BCHP_CHIP}/region_verify/bhsm_verify_reg.c

else
BHSM_INCLUDES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/region_verify \
			$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/region_verify/aegis
	
BHSM_SOURCES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/region_verify/bhsm_verify_reg.c
endif

endif
         
endif



#-----------------------------------------------------------------------------------#

###  7401C0/C1/C2 need RMX patch
ifeq ($(BCHP_CHIP),7401)
ifeq ($(BCHP_VER),C0)
BHSM_RMX_PATCH=1
endif 
ifeq ($(BCHP_VER),C1)
BHSM_RMX_PATCH=1
endif 
ifeq ($(BCHP_VER),C2)
BHSM_RMX_PATCH=1
endif 
endif

### 7403A0/A1 need RMX patch
ifeq ($(BCHP_CHIP),7403)
ifeq ($(BCHP_VER),A0)
BHSM_RMX_PATCH=1
endif 
ifeq ($(BCHP_VER),A1)
BHSM_RMX_PATCH=1
endif 
endif

### 7118, unknown yet.  if need, please use  'make RMXPATCH=ON/' to turn on, by default it is off
ifeq ($(BCHP_CHIP),7118)
ifeq ($(BCHP_VER),B0)

endif 
endif


### 7405 all 65nm, not needed
### 7400 B0 65nm, not needed,  A0?

## you can turn it ON for any chip, with 'make RMXPATCH=ON', even for 65nm chips like 7405
ifeq ($(RMXPATCH),ON)
BHSM_RMX_PATCH=1
endif

## you can turn it OFF for any chip, with 'make RMXPATCH=OFF', even for 130nm chips like 7401C0/C1/C2
ifeq ($(RMXPATCH),OFF)
BHSM_RMX_PATCH=0
endif
         
ifeq ($(BHSM_RMX_PATCH),1)
#CFLAGS += -DBHSM_RMX_PATCH=1
BHSM_DEFINES += BHSM_RMX_PATCH=1
#$(warning  ..........bhsm_all.inc is entered with BHSM_RMX_PATCH= $(BHSM_RMX_PATCH) )
endif

## you can turn it ON for any chip, with 'make BHSM_IPTV=ON', even for any chip
ifeq ($(BHSM_IPTV),ON)
BHSM_IPTV_MK=1
endif

## you can turn it OFF for any chip, with 'make BHSM_IPTV=OFF', even any chip
ifeq ($(BHSM_IPTV),OFF)
BHSM_IPTV_MK=0
endif
         
ifeq ($(BHSM_IPTV_MK),1)
#CFLAGS += -DBHSM_IPTV_CODE=1
BHSM_DEFINES += BHSM_IPTV_CODE=1
# $(warning  ..........IPTV Feature is built in with BHSM_IPTV_MK= $(BHSM_IPTV_MK) )
endif

#-----------------------------------------------------------------------------------#


#-----------------------------------------------------------------------------------#
###  BSECK  support 

ifeq ($(BCHP_CHIP),7400)
ifneq ($(findstring $(BCHP_VER),A0 B0 C0), $(BCHP_VER))
BHSM_BSECK_MK=0
endif
endif

ifeq ($(findstring $(BCHP_CHIP), 7405 7325 7335 7420 7340 7342 7125 7468), $(BCHP_CHIP))
BHSM_BSECK_MK=0
endif

## you can turn it ON for any chip, with 'make BHSM_BSECK=ON', even for any chip
ifeq ($(BHSM_BSECK),ON)
BHSM_BSECK_MK=1
endif

## you can turn it OFF for any chip, with 'make BHSM_BSECK=OFF', even any chip
ifeq ($(BHSM_BSECK),OFF)
BHSM_BSECK_MK=0
endif
         
ifeq ($(BHSM_BSECK_MK),1)
#CFLAGS += -DBHSM_BSECK_CODE=1
BHSM_DEFINES += BHSM_BSECK_CODE=1
#$(warning  ..........BSECK Feature is built in with BHSM_BSECK_MK= $(BHSM_BSECK_MK) )
endif

#-----------------------------------------------------------------------------------#
###  PI split, to hide certain security related PIs in a separate files


ifeq ($(BHSM_KEYLADDER),ON)
BHSM_SECURE_KEYLADDER_MK=1
endif

ifeq ($(BHSM_KEYLADDER),OFF)
BHSM_SECURE_KEYLADDER_MK=0
endif
         
ifeq ($(BSP_DS2_KEY_ROUTING),ON)
BHSM_DEFINES += BHSM_DS2_KEY_ROUTING
BHSM_SECURE_KEYLADDER_MK=1
endif

ifeq ($(BHSM_SECURE_KEYLADDER_MK),1)
#CFLAGS += -DBHSM_SECURE_PI_SUPPORT=1  -DBHSM_SECURE_PI_SUPPORT_KEYLADDER=1
BHSM_DEFINES += BHSM_SECURE_PI_SUPPORT=1
BHSM_DEFINES += BHSM_SECURE_PI_SUPPORT_KEYLADDER=1
#$(warning  ..........Secure Keyladder Feature is built in with BHSM_SECURE_KEYLADDER_MK= $(BHSM_SECURE_KEYLADDER_MK) )
else
#CFLAGS += -DBHSM_SECURE_PI_SUPPORT=0  -DBHSM_SECURE_PI_SUPPORT_KEYLADDER=0
BHSM_DEFINES += BHSM_SECURE_PI_SUPPORT=0
BHSM_DEFINES += BHSM_SECURE_PI_SUPPORT_KEYLADDER=0
endif

#-----------------------------------------------------------------------------------#

##--MS DRM Support--##
ifeq ($(MSDRM_PD_SUPPORT),y)
BHSM_DEFINES += MS_DRM
endif

ifeq ($(BHSM_MISC),ON)
BHSM_SECURE_MISC_MK=1
endif

ifeq ($(BHSM_MISC),OFF)
BHSM_SECURE_MISC_MK=0
endif
         
ifeq ($(BHSM_SECURE_MISC_MK),1)
#CFLAGS +=  -DBHSM_SECURE_PI_SUPPORT_MISC=1
BHSM_DEFINES += BHSM_SECURE_PI_SUPPORT_MISC=1
#$(warning  ..........Secure Misc Feature is built in with BHSM_SECURE_MISC_MK= $(BHSM_SECURE_MISC_MK) )
else
#CFLAGS +=  -DBHSM_SECURE_PI_SUPPORT_MISC=0
BHSM_DEFINES += BHSM_SECURE_PI_SUPPORT_MISC=0
endif


#-----------------------------------------------------------------------------------#
ifeq ($(BHSM_OTPMSP),ON)
BHSM_SECURE_OTPMSP_MK=1
endif

ifeq ($(BHSM_OTPMSP),OFF)
BHSM_SECURE_OTPMSP_MK=0
endif
         
ifeq ($(BHSM_SECURE_OTPMSP_MK),1)
#CFLAGS +=  -DBHSM_SECURE_PI_SUPPORT_OTPMSP=1
BHSM_DEFINES += BHSM_SECURE_PI_SUPPORT_OTPMSP=1
#$(warning  ..........Secure OTP_MSP Feature is built in with BHSM_SECURE_OTPMSP_MK= $(BHSM_SECURE_OTPMSP_MK) )
else
#CFLAGS +=  -DBHSM_SECURE_PI_SUPPORT_OTPMSP=0
BHSM_DEFINES += BHSM_SECURE_PI_SUPPORT_OTPMSP=0
endif

# Secure RSA Functions
ifeq ($(BHSM_SECURE_RSA),ON)
BHSM_SECURE_RSA_MK=1
endif

ifeq ($(BHSM_SECURE_RSA),OFF)
BHSM_SECURE_RSA_MK=0
endif
         
ifeq ($(BHSM_SECURE_RSA_MK),1)
BHSM_SECURE_RSA_FEATURE=1

#$(warning  ..........BHSM SECURE_RSA Feature is built in with BHSM_SECURE_RSA_MK= $(BHSM_SECURE_RSA_MK) )
         
BHSM_INCLUDES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/secure_rsa \
			$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/secure_rsa/aegis
	
BHSM_SOURCES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/secure_rsa/bhsm_secure_rsa.c

endif

# Secure Access Functions
ifeq ($(BHSM_SECURE_ACCESS),ON)
BHSM_SECURE_ACCESS_MK=1
endif

ifeq ($(BHSM_SECURE_ACCESS),OFF)
BHSM_SECURE_ACCESS_MK=0
endif
         
ifeq ($(BHSM_SECURE_ACCESS_MK),1)
BHSM_SECURE_ACCESS_FEATURE=1

#$(warning  ..........BHSM SECURE_ACCESS Feature is built in with BHSM_SECURE_ACCESS_MK= $(BHSM_SECURE_ACCESS_MK) )
         
BHSM_INCLUDES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/secure_access \
			$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/secure_access/aegis
	
BHSM_SOURCES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/secure_access/bhsm_secure_access.c

endif

# IP Licensing Functions
ifeq ($(BHSM_IP_LICENSING),ON)
BHSM_IP_LICENSING_MK=1
endif

ifeq ($(BHSM_IP_LICENSING),OFF)
BHSM_IP_LICENSING_MK=0
endif
         
ifeq ($(BHSM_IP_LICENSING_MK),1)
BHSM_IP_LICENSING_FEATURE=1

#$(warning  ..........BHSM IP_LICENSING Feature is built in with BHSM_IP_LICENSING_MK= $(BHSM_IP_LICENSING_MK) )
         
BHSM_INCLUDES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/ip_licensing \
			$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/ip_licensing/aegis
	
BHSM_SOURCES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/ip_licensing/bhsm_ip_licensing.c

endif
# Debug_RdWr feature
ifeq ($(BHSM_DEBUG_RD_WR),ON)
BHSM_DEBUG_RD_WR_MK=1
endif

ifeq ($(BHSM_DEBUG_RD_WR),OFF)
BHSM_DEBUG_RD_WR_MK=0
endif

ifeq ($(BHSM_DEBUG_RD_WR_MK),1)
BHSM_DEBUG_RD_WR_FEATURE=1

#$(warning  ..........BHSM DEBUG_RD_WR Feature is built in with BHSM_DEBUG_RD_WR_MK= $(BHSM_DEBUG_RD_WR_MK) )

ifeq ($(filter $(CORETECH), coreDTV coreDVDBD), $(CORETECH))
BHSM_INCLUDES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/${BCHP_CHIP}/debug_rdwr
	
BHSM_SOURCES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/${BCHP_CHIP}/debug_rdwr/bhsm_debug_rdwr.c
else         
BHSM_INCLUDES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/debug_rdwr
	
BHSM_SOURCES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/debug_rdwr/bhsm_debug_rdwr.c
endif

endif

#######################################################################

BHSM_KEY_LADDER_FEATURE=1

ifneq ($(findstring $(BCHP_CHIP),3563 3548 3556), $(BCHP_CHIP))
BHSM_OTPMSP_FEATURE=1
endif


#######################################################################

# Module Includes
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/common
ifeq ($(findstring $(BCHP_CHIP), 7550,7408), $(BCHP_CHIP))
#$(warning  ..........No Aegis header files needed for 7550  )
else
ifeq ($(CORETECH),coreDVDBD)
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/common/aegis
else
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/include/${CORETECH}/common/aegis
endif

endif

ifdef BHSM_KEY_LADDER_FEATURE
ifeq ($(filter $(CORETECH), coreDVDBD), $(CORETECH))
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/keyladder
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/keyladder/bhsm_keyladder.c
else
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/keyladder
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/keyladder/bhsm_keyladder.c
endif
ifeq ($(findstring $(BCHP_CHIP), 7550,7408), $(BCHP_CHIP))
#$(warning  ..........No Aegis header files needed for 7550  )
else
ifeq ($(findstring $(CORETECH), coreDVDBD), $(CORETECH))
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/keyladder/aegis
else
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/keyladder/aegis
endif
endif
endif


# automatically pick up the customer specific shared header files, cust_x or cust_generic
ifeq ($(findstring $(BCHP_CHIP), 7550,7408), $(BCHP_CHIP))
#$(warning  ..........No customer specific header files needed for 7550  )
else
ifeq ($(filter $(CORETECH), core65nmASKM), $(CORETECH))
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/include/${CORETECH}/common/cust_$(BHSM_CUST_DIR)
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/include/${CORETECH}/common/cust_$(BHSM_CUST_DIR)/aegis
else
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/include/${CORETECH}/$(BCHP_CHIP)/$(BCHP_VER_LOWER)/cust_$(BHSM_CUST_DIR)
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/include/${CORETECH}/$(BCHP_CHIP)/$(BCHP_VER_LOWER)/cust_$(BHSM_CUST_DIR)/aegis
endif

endif

# Module Sources
ifeq ($(findstring $(CORETECH), coreNoBSP coreDVDBD), $(CORETECH))
ifeq ($(findstring $(BCHP_CHIP), 35230, 35125, 35233), $(BCHP_CHIP))
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/common/bhsm_user_cmds.c
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/common/bhsm_misc.c
endif
ifeq ($(findstring $(BCHP_CHIP), 7550, 7408), $(BCHP_CHIP))
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/common/bhsm.c
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/common/bhsm_priv.c
else
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/common/bhsm.c
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/common/bhsm_priv.c
endif

else 
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/common/bhsm.c
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/common/bhsm_priv.c
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/common/bhsm_misc.c
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/common/bhsm_user_cmds.c
endif

ifeq ($(BHSM_SECURE_OTPMSP_MK),1)
ifdef BHSM_OTPMSP_FEATURE	

ifeq ($(filter $(CORETECH), coreNoBSP coreDVDBD), $(CORETECH))
ifeq ($(findstring $(BCHP_CHIP), 7550, 7408), $(BCHP_CHIP))
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/otpmsp
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/otpmsp/aegis
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/otpmsp/bhsm_otpmsp.c
else
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/otpmsp
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/otpmsp/aegis
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/otpmsp/bhsm_otpmsp.c
endif

else
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/otpmsp
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/otpmsp/aegis
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/otpmsp/bhsm_otpmsp.c
endif

ifeq ($(BHSM_PROG_PATTERN),ON)
BHSM_DEFINES += BSP_OTP_PROG_PATTERN_SEQUENCE=1
endif

endif
endif


# Extended options
ifeq ($(BCHP_CHIP),7118)
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/docsis

BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/docsis/bhsm_docsis.c
endif


ifeq ($(BCHP_CHIP),7440)
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/dvd_cmds
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/dvd_cmds/aegis

BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/dvd_cmds/bhsm_dvd_cmds.c
endif

ifeq ($(findstring $(BCHP_CHIP), 7601 7635 7630), $(BCHP_CHIP))
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/dvd_cmds
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/dvd_cmds/aegis

BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/dvd_cmds/bhsm_dvd_cmds.c
endif


ifeq ($(BHSM_IPTV_MK),1)
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/iptv
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/iptv/bhsm_iptv.c
#CFLAGS += -DBHSM_IPTV=1 -DBSP_MSIPTV
BHSM_DEFINES += BHSM_IPTV=1
BHSM_DEFINES += BSP_MSIPTV
endif

# IMS144828: Add BHSM_IPTV to zero to fix compiler warnings
ifeq ($(BHSM_IPTV_MK),0)
#CFLAGS += -DBHSM_IPTV=0
BHSM_DEFINES += BHSM_IPTV=0
endif
# IMS144828: end

ifeq ($(BHSM_BSECK_MK),1)
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/region_verify
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/region_verify/aegis
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/region_verify/bhsm_bseck.c
#CFLAGS += -DBSP_DOWNLOAD_CODE=1
BHSM_DEFINES += BSP_DOWNLOAD_CODE=1
endif

ifeq ($(BHSM_SECURE_KEYLADDER_MK),1)

ifeq ($(filter $(CORETECH), coreNoBSP coreDVDBD), $(CORETECH))
ifeq ($(findstring $(BCHP_CHIP), 7550, 7408), $(BCHP_CHIP))
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/keyladder/bhsm_keyladder_enc.c
else
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/keyladder/bhsm_keyladder_enc.c
endif

else
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/keyladder/bhsm_keyladder_enc.c
endif

endif

ifeq ($(BHSM_SECURE_MISC_MK),1)

ifeq ($(filter $(CORETECH), coreNoBSP coreDVDBD), $(CORETECH))
ifeq ($(findstring $(BCHP_CHIP), 7550, 7408), $(BCHP_CHIP))
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/common/bhsm_misc_enc.c
else
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/common/bhsm_misc_enc.c
endif

else
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/common/bhsm_misc_enc.c
endif

endif

# for 7640 overwrite INCLUDES and SOURCES since HSM is in lib form
ifeq ($(findstring $(BCHP_CHIP), 7640), $(BCHP_CHIP))
$(warning  ..........7640 Skipping compilation of sources!!  )
BHSM_INCLUDES = $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/common
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/common/aegis
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/include/${CORETECH}/$(BCHP_CHIP)/$(BCHP_VER_LOWER)/cust_generic/aegis
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/$(BCHP_CHIP)/keyladder
BHSM_SOURCES =
endif

