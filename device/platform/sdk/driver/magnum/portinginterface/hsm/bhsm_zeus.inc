#/***************************************************************************
# *     (c)2007-2011 Broadcom Corporation
# *
# *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
# *  and may only be used, duplicated, modified or distributed pursuant to the terms and
# *  conditions of a separate, written license agreement executed between you and Broadcom
# *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
# *  no license (express or implied), right to use, or waiver of any kind with respect to the
# *  Software, and Broadcom expressly reserves all rights in and to the Software and all
# *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
# *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
# *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
# *
# *  Except as expressly set forth in the Authorized License,
# *
# *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
# *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
# *  and to use this information only in connection with your use of Broadcom integrated circuit products.
# *
# *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
# *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
# *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
# *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
# *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
# *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
# *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
# *  USE OR PERFORMANCE OF THE SOFTWARE.
# *
# *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
# *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
# *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
# *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
# *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
# *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
# *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
# *  ANY LIMITED REMEDY.
# *
# * $brcm_Workfile: bhsm_zeus.inc $
# * $brcm_Revision: Hydra_Software_Devel/8 $
# * $brcm_Date: 12/5/12 1:28a $
# *
# * Module Description:
# *
# * Revision History:
# *
# * $brcm_Log: /magnum/portinginterface/hsm/bhsm_zeus.inc $
# * 
# * Hydra_Software_Devel/8   12/5/12 1:28a atruong
# * SW7563-16: HSM PI support for 7563A0
# * 
# * Hydra_Software_Devel/7   12/4/12 7:46p atruong
# * SW7584-13: HSM PI support for 7584A0 - Zeus setting parsing
# * 
# * Hydra_Software_Devel/6   11/20/12 1:54a atruong
# * SW7584-46: HSM PI/Nexus Security support for 7584A0
# * 
# * Hydra_Software_Devel/5   11/19/12 11:01a atruong
# * SW7584-46: HSM PI/Nexus Security support for 7584A0 SW7435-232: HSM
# * PI/Nexus Security support for 7435B0
# * 
# * Hydra_Software_Devel/4   10/23/12 11:03a atruong
# * SW7435-439: To support Unified Refsw for 40-nm platforms in Nexus
# * Security/HSM PI
# * 
# * Hydra_Software_Devel/3   10/3/12 12:00p atruong
# * SWSECURITY-185: Remove warning messages in include files
# * 
# * Hydra_Software_Devel/2   9/21/12 11:13a qxie
# * SW7360-34:add support to 7360
# * 
# * Hydra_Software_Devel/1   9/20/12 2:41p atruong
# * SWSECURITY-175: Removing Symbolic Link in HSM PI
# * 
# * Hydra_Software_Devel/10   9/11/12 2:38p atruong
# * SWSECURITY-175: Remove symbolic links in HSM PI
# * 
# * Hydra_Software_Devel/9   4/3/12 2:09a atruong
# * SW7425-2807:Add support for BSP PKL hardware
# * 
# * Hydra_Software_Devel/8   10/17/11 5:26p atruong
# * SWSECURITY-69: Support for 40-nm B0 Zeus 2.0 platforms
# * 
# * Hydra_Software_Devel/7   10/12/11 10:48p atruong
# * SW7425-1251: HSM PI Support For BCM7425B0
# * 
# * Hydra_Software_Devel/6   9/23/11 7:09p atruong
# * SW7346-495: HSM_SOURCE_AVAILABLE option fix
# * 
# * Hydra_Software_Devel/5   9/21/11 11:55a atruong
# * SW7346-495: Add NSK2AVK support
# * 
# * Hydra_Software_Devel/4   6/2/11 6:08p atruong
# * SW7422-451: New Folder structure/BSP shared header file separation
# * 
# * Hydra_Software_Devel/3   5/31/11 11:34p atruong
# * SW7422-451: New Folder structure/BSP shared header file separation
# * 
# * Hydra_Software_Devel/2   1/24/11 10:44a atruong
# * SW7422-26: Add HSM PI Support For 7422A0  - CC header format
# *
# ******************************************************************************/

#-----------------------------------------------------------------------------------#
# HSM build-command-line-options:
#  CUST=GENERIC3A  or 
#  CUST=GENERIC3B  or
#  CUST=DIRECTV    or 
#  CUST=ECHOSTAR   or
#  CUST=CONAX      or
#  CUST=CISCO      or
#  CUST=IRDETO     or
#  CUST=MICROSOFT  or
#  CUST=MOTOROLA   or
#  CUST=NAGRA      or
#  CUST=NDS        or
#  CUST=NDSDSS     or
#  CUST=ALL        to select customer mode
#  CUST=Not Set or Invalid -  Customer Mode defaults to GENERIC3A
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
#  BHSM_MISC=ON              to turn on support for DEBUG_DUMP_EXEC command
#  BHSM_PWR_MGMT=ON          to turn on support for power management
#  BHSM_WEB_CPU=ON           to turn on support for Web CPU interface
#  BHSM_SAGE_INTF=ON         to turn on SAGE-Host interface support
#  BHSM_SAGE_PI=ON           to compile HSM PI for SAGE
#-----------------------------------------------------------------------------------#


BCHP_VER_LOWER := $(shell awk 'BEGIN{print tolower("$(BCHP_VER)")}')

#
# Check for core architecture
#

ifeq ($(filter $(BCHP_CHIP), 7231 7344 7346 7358 7360 7422 7425 7429 7435 7552 7563 7584), $(BCHP_CHIP))
CORETECH := core40nm
endif


#
# Now check for Zeus version
#
ifeq ($(CORETECH),core40nm)

ifeq ($(BCHP_VER_LOWER), a0)
ifneq ($(filter $(BCHP_CHIP), 7429 7435 7360 7563 7584), $(BCHP_CHIP))
ZEUSVER := Zeus10
else
ifeq ($(filter $(BCHP_CHIP), 7429 7360 7563), $(BCHP_CHIP))
ZEUSVER := Zeus20
endif
ifeq ($(BCHP_CHIP), 7435)
ZEUSVER := Zeus25
endif
ifeq ($(BCHP_CHIP), 7584)
ZEUSVER := Zeus30
endif

endif
endif

ifeq ($(BCHP_VER_LOWER), a1)
ZEUSVER := Zeus10
endif

ifeq ($(BCHP_VER_LOWER), b0)
ifeq ($(filter $(BCHP_CHIP), 7429), $(BCHP_CHIP))
ifeq ($(CUST), NAGRA)
ZEUSVER := Zeus20
else
ZEUSVER := Zeus25
endif
else
ifeq ($(filter $(BCHP_CHIP), 7435), $(BCHP_CHIP))
ZEUSVER := Zeus30
else
ZEUSVER := Zeus20
endif
endif
endif

ifeq ($(filter $(BCHP_VER_LOWER), b1 b2), $(BCHP_VER_LOWER))
ZEUSVER := Zeus25
endif


endif

#$(warning  ..........bhsm_40nm.inc entered with CORETECH = $(CORETECH) and ZEUSVER = $(ZEUSVER))


#
# Conditional Options for this Makefile
#
# BHSM_CUST - HSM Specific Option.  This value will be provided for you if appropriate.
#

# Define this module
MAGNUM_MODULES += BHSM



# Definitions
BHSM_DEFINES += BHSM_AEGIS_SUPPORT

# Support uScValue definition for newer chip (7400C0 and later)
BHSM_DEFINES += BHSM_SC_VALUE
BSP_SC_VALUE_SUPPORT=ON

         
CUST_SETTING := OFF

#-----------------------------------------------------------------------------------#
# old support "make CUST=ECHOSTAR"
# space can NOT be put after 'ECHOSTAR' in ifeq ($(CUST),  ECHOSTAR),  yet can be put before 'ECHOSTAR'

ifeq ($(CUST),ECHOSTAR)
BHSM_CUST := ECHOSTAR
CUST_SETTING := ON
endif


#-----------------------------------------------------------------------------------#
# 4/11/07 to add EchoStar build support 
# this is complicated, discard:  ifeq ($(findstring E, $(BHSM_CUST)), $(BHSM_CUST))

ifeq ($(BHSM_CUST),ECHOSTAR)
# change itself from the upper case to lower for directory mapping

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_echostar.inc
endif



#-----------------------------------------------------------------------------------#
# old support "make CUST=DIRECTV"
ifeq ($(CUST),DIRECTV)
BHSM_CUST := DIRECTV
CUST_SETTING := ON
endif

ifeq ($(BHSM_CUST),DIRECTV)

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_directv.inc
endif


#-----------------------------------------------------------------------------------#
# old support "make CUST=NDS"
ifeq ($(CUST),NDS)
BHSM_CUST := NDS
CUST_SETTING := ON
endif

ifeq ($(BHSM_CUST),NDS)

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_nds.inc
endif


#-----------------------------------------------------------------------------------#
# old support "make CUST=MICROSOFT"
ifeq ($(CUST),MICROSOFT)
BHSM_CUST := MICROSOFT
CUST_SETTING := ON
endif

ifeq ($(BHSM_CUST),MICROSOFT)

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_microsoft.inc
endif



#-----------------------------------------------------------------------------------#
# old support "make CUST=CISCO"
ifeq ($(CUST),CISCO)
BHSM_CUST := CISCO
CUST_SETTING := ON
endif

ifeq ($(BHSM_CUST),CISCO)

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_cisco.inc
endif

#-----------------------------------------------------------------------------------#
# old support "make CUST=CONAX"
ifeq ($(CUST),CONAX)
BHSM_CUST := CONAX
CUST_SETTING := ON
endif

ifeq ($(BHSM_CUST),CONAX)

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_conax.inc
endif


#-----------------------------------------------------------------------------------#
# old support "make CUST=IRDETO"
ifeq ($(CUST),IRDETO)
BHSM_CUST := IRDETO
CUST_SETTING := ON
endif

ifeq ($(BHSM_CUST),IRDETO)

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_irdeto.inc
endif

#-----------------------------------------------------------------------------------#
# old support "make CUST=MOTOROLA"
ifeq ($(CUST),MOTOROLA)
BHSM_CUST := MOTOROLA
CUST_SETTING := ON
endif

ifeq ($(BHSM_CUST),MOTOROLA)

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_motorola.inc
endif

#-----------------------------------------------------------------------------------#
# old support "make CUST=NAGRA"
ifeq ($(CUST),NAGRA)
BHSM_CUST := NAGRA
CUST_SETTING := ON
endif

ifeq ($(BHSM_CUST),NAGRA)

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_nagra.inc
endif

#-----------------------------------------------------------------------------------#
# old support "make CUST=NDSDSS"
ifeq ($(CUST),NDSDSS)
BHSM_CUST := NDSDSS
CUST_SETTING := ON
endif

ifeq ($(BHSM_CUST),NDSDSS)

BHSM_CUST_DIR := $(shell awk 'BEGIN{print tolower("$(BHSM_CUST)")}')
         
include $(MAGNUM)/portinginterface/hsm/bhsm_ndsdss.inc
endif


#-----------------------------------------------------------------------------------#
# old support "make CUST=ALL"
ifeq ($(CUST),ALL)
BHSM_CUST := ALL
CUST_SETTING := ON
endif

ifeq ($(BHSM_CUST),ALL)

BHSM_CUST_DIR = all
         
include $(MAGNUM)/portinginterface/hsm/bhsm_all.inc
endif


#-----------------------------------------------------------------------------------#
ifeq ($(CUST),GENERIC3A)
BHSM_CUST := GENERIC3A
CUST_SETTING := ON
endif
ifeq ($(BHSM_CUST),GENERIC3A)
GREAT_GENERIC=true
# always 
BHSM_CUST_DIR = generic3A
endif

##
## CUST=GENERIC3B
##

ifeq ($(CUST),GENERIC3B)
BHSM_CUST := GENERIC3B
CUST_SETTING := ON
endif
ifeq ($(BHSM_CUST),GENERIC3B)
GREAT_GENERIC=true
# always 
BHSM_CUST_DIR = generic3B
endif


#-----------------------------------------------------------------------------------#         
# Default BHSM_CUST to GENERIC3A,   
#               make without touch of BHSM_CUST
#	        make CUST=X,  make BHSM_CUST=X,   X is anything but defined

ifeq ($(CUST_SETTING),OFF)
CUST := GENERIC3A
BHSM_CUST := GENERIC3A
GREAT_GENERIC=true
#$(warning  ..........HSM PI built with BHSM_CUST= $(BHSM_CUST) )
# always 
BHSM_CUST_DIR = generic3A
endif

ifeq ($(GREAT_GENERIC),true)
# this line has no effect if make BHSM_CUST=G :            BHSM_CUST := generic

# not used here, for testApp build
BHSM_CUST_GENERIC=1

BHSM_DEFINES += BSP_GENERIC

# 3/8/07, looks the platform or magnum does not add this definition
#CFLAGS += -DBSP_GENERIC



###Region_Verification support in Generic Mode, there is an option to add the Region_Verification or not
ifeq ($(BHSM_REGIONVERIFY),ON)
BHSM_REGIONVERIFY_MK=1
endif

ifeq ($(BHSM_REGIONVERIFY),OFF)
BHSM_REGIONVERIFY_MK=0
endif
         
ifeq ($(BHSM_REGIONVERIFY_MK),1)
BHSM_REG_VERIFY_FEATURE=1

#$(warning  ..........Region Verification Feature is built in with BHSM_REGIONVERIFY_MK= $(BHSM_REGIONVERIFY_MK) )
         
BHSM_INCLUDES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/region_verify \
			$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/region_verify/aegis
	
BHSM_SOURCES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/region_verify/bhsm_verify_reg.c

endif
         
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
###  BSECK  support 


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
         
ifeq ($(BHSM_SECURE_KEYLADDER_MK),1)
#CFLAGS += -DBHSM_SECURE_PI_SUPPORT_KEYLADDER=1
BHSM_DEFINES += BHSM_SECURE_PI_SUPPORT_KEYLADDER=1
#$(warning  ..........Secure Keyladder Feature is built in with BHSM_SECURE_KEYLADDER_MK= $(BHSM_SECURE_KEYLADDER_MK) )
else
#CFLAGS += -DBHSM_SECURE_PI_SUPPORT_KEYLADDER=0
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

ifeq ($(BHSM_PWR_MGMT),ON)
BHSM_PWRMGMT_MK=1
endif

ifeq ($(BHSM_PWR_MGMT),OFF)
BHSM_PWRMGMT_MK=0
endif
         
ifeq ($(BHSM_PWRMGMT_MK),1)
#CFLAGS +=  -DBHSM_PWRMGMT_SUPPORT=1
BHSM_DEFINES += BHSM_PWRMGMT_SUPPORT=1
#$(warning  ..........Power Management Feature is built in with BHSM_PWRMGMT_MK= $(BHSM_PWRMGMT_MK) )
else
#CFLAGS +=  -DBHSM_PWRMGMT_SUPPORT=0
BHSM_DEFINES += BHSM_PWRMGMT_SUPPORT=0
endif


ifeq ($(BHSM_WEB_CPU),ON)
BHSM_WEBCPU_MK=1
endif

ifeq ($(BHSM_WEB_CPU),OFF)
BHSM_WEBCPU_MK=0
endif
         
ifeq ($(BHSM_WEBCPU_MK),1)
#CFLAGS +=  -DBHSM_WEBCPU_SUPPORT=1
BHSM_DEFINES += BHSM_WEBCPU_SUPPORT=1
BHSM_DEFINES += WEBCPU=1
#$(warning  ..........Power Management Feature is built in with BHSM_WEBCPU_MK= $(BHSM_WEBCPU_MK) )
else
#CFLAGS +=  -DBHSM_WEBCPU_SUPPORT=0
BHSM_DEFINES += BHSM_WEBCPU_SUPPORT=0
BHSM_DEFINES += WEBCPU=0
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
BHSM_DEFINES += BSP_SECURE_RSA=1
BHSM_DEFINES += BSP_SECURE_RSA_IN_VKL=1

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
         
BHSM_INCLUDES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/brcm_only
	
ifneq ($(HSM_SOURCE_AVAILABLE),n)
BHSM_SOURCES += 	$(MAGNUM)/portinginterface/hsm/src/${CORETECH}/brcm_only/bhsm_debug_rdwr.c
endif

endif

#######################################################################

BHSM_KEY_LADDER_FEATURE=1

BHSM_OTPMSP_FEATURE=1


#######################################################################

# Module Includes
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/common

# Common Aegis Includes
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/include/${CORETECH}/common/${ZEUSVER}/aegis

ifdef BHSM_KEY_LADDER_FEATURE
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/keyladder
ifneq ($(HSM_SOURCE_AVAILABLE),n)
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/keyladder/bhsm_keyladder.c
endif
endif

# automatically pick up the customer specific shared header files, cust_x or cust_generic
# For Unified Nexus Security/HSM PI, we do not use the following entry
#ifneq ($(BHSM_CUST),ALL)
#BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/include/${CORETECH}/${BCHP_CHIP}/${BCHP_VER_LOWER}/cust_$(BHSM_CUST_DIR)/aegis
#endif
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/include/${CORETECH}/common/${ZEUSVER}/cust_$(BHSM_CUST_DIR)/aegis
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/user_misc_cmd

# Module Sources
ifneq ($(HSM_SOURCE_AVAILABLE),n)
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/common/bhsm.c
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/common/bhsm_priv.c
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/user_misc_cmd/bhsm_misc.c
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/user_misc_cmd/bhsm_user_cmds.c
endif
ifeq ($(BHSM_SECURE_OTPMSP_MK),1)
ifdef BHSM_OTPMSP_FEATURE	
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/otpmsp
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/otpmsp/aegis
ifneq ($(HSM_SOURCE_AVAILABLE),n)
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/otpmsp/bhsm_otpmsp.c
endif
endif
endif

# Extended options

ifeq ($(BHSM_IPTV_MK),1)
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/iptv
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/iptv/aegis

ifneq ($(HSM_SOURCE_AVAILABLE),n)
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/iptv/bhsm_iptv.c
endif
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
ifneq ($(HSM_SOURCE_AVAILABLE),n)
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/region_verify/bhsm_bseck.c
endif
#CFLAGS += -DBSP_DOWNLOAD_CODE=1
BHSM_DEFINES += BSP_DOWNLOAD_CODE=1
endif

ifeq ($(BHSM_SECURE_KEYLADDER_MK),1)
ifneq ($(HSM_SOURCE_AVAILABLE),n)
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/keyladder/bhsm_keyladder_enc.c
endif
endif


ifeq ($(BHSM_PWRMGMT_MK),1)
ifneq ($(HSM_SOURCE_AVAILABLE),n)
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/power_management/bhsm_pwr_mgmt.c
endif
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/power_management
endif

ifeq ($(BHSM_SECURE_MISC_MK),1)
ifneq ($(HSM_SOURCE_AVAILABLE),n)
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/user_misc_cmd/bhsm_misc_enc.c
endif
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/$(CORETECH)/user_misc_cmd
endif

ifeq ($(BHSM_WEBCPU_MK),1)
ifneq ($(HSM_SOURCE_AVAILABLE),n)
BHSM_SOURCES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/webcpu/bhsm_webcpu.c
endif
BHSM_INCLUDES += $(MAGNUM)/portinginterface/hsm/src/${CORETECH}/webcpu
endif
