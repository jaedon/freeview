
##############################################################################
#
# Copyright (c) 2004-2010 Broadcom Corporation
#
# This program is the proprietary software of Broadcom Corporation and/or
# its licensors, and may only be used, duplicated, modified or distributed
# pursuant to the terms and conditions of a separate, written license
# agreement executed between you and Broadcom (an "Authorized License").
# Except as set forth in an Authorized License, Broadcom grants no license
# (express or implied), right to use, or waiver of any kind with respect to
# the Software, and Broadcom expressly reserves all rights in and to the
# Software and all intellectual property rights therein.  IF YOU HAVE NO
# AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY,
# AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE
# SOFTWARE.  
#
# Except as expressly set forth in the Authorized License,
#
# 1.     This program, including its structure, sequence and organization,
# constitutes the valuable trade secrets of Broadcom, and you shall use all
# reasonable efforts to protect the confidentiality thereof, and to use this
# information only in connection with your use of Broadcom integrated circuit
# products.
#
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
# "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
# OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
# RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL
# IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR
# A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET
# ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME
# THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
#
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
# OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
# INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY
# RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM
# HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN
# EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
# WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY
# FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#
##############################################################################
############################################################################
##                 DEFINES
############################################################################ 

############################################################################
##                 PATHS
############################################################################ 

PWD_DIR	          = $(CURDIR)
CORE_V2_APP_DIR   = ../../../../../Bfc/BfcApp
V2_COMMON_DIR     = ../../../../../Bfc
USER_IFACE_DIR    = $(V2_COMMON_DIR)/UserInterface
IP_HELPERS_DIR    = $(V2_COMMON_DIR)/IpHelpers
V2_DHCP_DIR       = $(V2_COMMON_DIR)/IpHelpers/Dhcp
OS_WRAPPER_DIR    = $(V2_COMMON_DIR)/OperatingSystem
NETWORKING_DIR 	  = $(V2_COMMON_DIR)/Networking
PING_DIR          = $(IP_HELPERS_DIR)/Ping
CONSOLE_DIR       = ../../../Common/Linux
VERSION_DIR       = ../../../../Common
BSPCOMMON_DIR     = ../../../../../cm_bsp_v2/bsp_common/common_inc

PORTABLE_APP_DIR  = ../../../StbApp_DsgCC
TARGETOS_APP_DIR  = ../../../StbApp_DsgCC/$(LC_TARGETOS)


############################################################################
##                 OBJECT FILES
############################################################################                                       

PORTABLE_APP_OBJS = BfcApplication.o \

ifeq ($(BCM_NONVOL_SUPPORT),1)
	PORTABLE_APP_OBJS += BfcAppCompositeNonVolSettings.o 
endif

ifeq ($(BCM_REDUCED_IMAGE_SIZE), 1)
else
ifeq ($(BCM_CONSOLE_SUPPORT),1)
	ifeq ($(BCM_NONVOL_SUPPORT),1)
		PORTABLE_APP_OBJS += BfcAppCompositeNonVolCommandTable.o 
	endif
endif
endif
                     
                
V2_COMMON_OBJS    = IpAddress.o \
	IpV4Address.o \
	IpV6Address.o \
        MacAddress.o \
        OctetBuffer.o \
        Utilities.o \
        EventPublisher.o \
        IpAddressList.o \
	Counter64.o 

ifeq ($(BCM_NONVOL_SUPPORT),1)
        V2_COMMON_OBJS    += V2Main.o
endif

IP_HELPER_OBJS    = #IpHelperFactory.o \
                    
OS_WRAPPER_OBJS   = CountingSemaphore.o \
        Event.o \
        EventSet.o \
        MutexSemaphore.o \
        OperatingSystem.o \
        OperatingSystemFactory.o \
        $(OS_PREFIX)CountingSemaphore.o \
        $(OS_PREFIX)Event.o \
        $(OS_PREFIX)EventSet.o \
        $(OS_PREFIX)MutexSemaphore.o \
        $(OS_PREFIX)OperatingSystem.o \
        $(OS_PREFIX)Timer.o \
        SystemTime.o \
        Thread.o \
        Timer.o \
        TimerMux.o \
        TimerMuxACT.o \
	CompletionHandlerACT.o \
        #$(OS_PREFIX)ArpHelper.o \
        #HeapMonitorThread.o

ifeq "$(TARGETOS)" "Linux"
OS_WRAPPER_OBJS  +=     \
        LnxTimeout.o    
endif
        
USER_IFACE_OBJS   = MessageLog.o \
        MessageLogSettings.o \
        CmAppVersionInfo.o

HARDWARE_OBJS     = EthernetPacket.o \
		    stringfind.o

CORE_V2_OBJS =  $(PORTABLE_APP_OBJS) \
        $(V2_COMMON_OBJS) \
        $(IP_HELPER_OBJS) \
        $(OS_WRAPPER_OBJS) \
        $(USER_IFACE_OBJS) \
        $(HARDWARE_OBJS)
                

############################################################################
##                 CFLAGS
############################################################################

ifeq ($(BCM_BOOT_LOADER_SUPPORT),1)
  ADDED_CFLAGS   += -DBOOT_LOADER_INCLUDE=$(BCM_BOOT_LOADER_SUPPORT)
  CORE_V2_OBJS   += bootloaderstore.o
endif

############################################################################
##                 EXTRA_INCLUDE - include path
############################################################################ 

EXTRA_INCLUDE += -I$(CORE_V2_APP_DIR) \
        -I$(TARGETOS_APP_DIR) \
        -I$(V2_COMMON_DIR) \
        -I$(IP_HELPERS_DIR) \
        -I$(OS_WRAPPER_DIR) \
        -I$(PORTABLE_APP_DIR) \
        -I$(USER_IFACE_DIR) \
        -I$(V2_DHCP_DIR) \
	-I$(NETWORKING_DIR) \
	-I$(PING_DIR) \
	-I$(CONSOLE_DIR) \
	-I$(VERSION_DIR) \
	-I$(BSPCOMMON_DIR)
                 
############################################################################
##                 VPATH - source path
############################################################################

VPATH +=$(CORE_V2_APP_DIR):\
        $(TARGETOS_APP_DIR):\
        $(PORTABLE_APP_DIR):\
        $(V2_COMMON_DIR):\
        $(IP_HELPERS_DIR):\
        $(OS_WRAPPER_DIR):\
        $(USER_IFACE_DIR):\
        $(V2_DHCP_DIR):\
	$(NETWORKING_DIR):\
	$(CONSOLE_DIR):

############################################################################
##                 DSGCC_OBJS - object files
############################################################################                                       
                 
DSGCC_OBJS    += $(CORE_V2_OBJS)

############################################################################
##                 LIB_EXTRA - libraries
############################################################################                                       

############################################################################
##                 Special targets
############################################################################                                       

#ifneq "$(BOARDNONVOLDEFAULTS)" ""
#MergedNonVolDefaults.h : $(BOARDNONVOLDEFAULTS) CustomerNonVolDefaults.h
#	@ cp ../../CustomerNonVolDefaults.h MergedNonVolDefaults.h
#	@ attrib -r MergedNonVolDefaults.h
#	@ sed -f ../undefines.sed $< >> MergedNonVolDefaults.h
#endif


