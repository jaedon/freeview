############################################################
#     (c)2007-2012 Broadcom Corporation
#  
#  This program is the proprietary software of Broadcom Corporation and/or its licensors,
#  and may only be used, duplicated, modified or distributed pursuant to the terms and
#  conditions of a separate, written license agreement executed between you and Broadcom
#  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
#  no license (express or implied), right to use, or waiver of any kind with respect to the
#  Software, and Broadcom expressly reserves all rights in and to the Software and all
#  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
#  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
#  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
#   
#  Except as expressly set forth in the Authorized License,
#   
#  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#   
#  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
#  USE OR PERFORMANCE OF THE SOFTWARE.
#  
#  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
#  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
#  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
#  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
#  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
#  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
#  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
#  ANY LIMITED REMEDY.
# 
# $brcm_Workfile: wmdrmpd_core.inc $
# $brcm_Revision: 2 $
# $brcm_Date: 10/16/12 11:39a $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/wmdrmpd_core/wmdrmpd_core.inc $
# 
# 2   10/16/12 11:39a hugost
# SW7425-4020: Module depends on security
# 
# 1   7/27/12 2:26p jgarrett
# SW7425-3281: Merge to main branch
# 
# SW7425-3281/4   7/25/12 6:16a hugost
# sw7231-917: Move the inclusion of libdrmrootfs.a to the nexus extralib
#  variable in the if linuxkernel. This is needed to resolve an issue
#  where the drmrootfs used in the kernel build was the one compiled for
#  user space. Which lead to a linking problem where abicalls files were
#  linked with non-abicalls files.
# 
# SW7425-3281/3   7/18/12 5:02p piyushg
# SW7425-3281: Remove realpath from make commend. Irvine build server
#  does not support it.
# 
# SW7425-3281/2   6/22/12 6:47a jgarrett
# SW7425-3281: Adding nexus_wmdrmpd_core_fileio.c
# 
# SW7425-3281/1   6/20/12 2:35p piyushg
# SW7425-3281: Initial checkin of Nexus directory structure and header
#  files.
# 
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += WMDRMPD_CORE

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_WMDRMPD_CORE_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/wmdrmpd_core/include


##################
# Module Sources #
##################
NEXUS_WMDRMPD_CORE_SOURCES := \
	$(NEXUS_TOP)/modules/wmdrmpd_core/src/nexus_wmdrmpd_core.c \
	$(NEXUS_TOP)/modules/wmdrmpd_core/src/nexus_wmdrmpd_core_fileio.c \
	$(NEXUS_TOP)/modules/wmdrmpd_core/src/nexus_wmdrmpd_core_module.c

# thunk layer
NEXUS_WMDRMPD_CORE_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/wmdrmpd_core/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_WMDRMPD_CORE_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_wmdrmpd_core_thunks.c


ifeq ($(PRDY_ROBUSTNESS_ENABLE), y)
NEXUS_WMDRMPD_CORE_PRIVATE_INCLUDES += $(BSEAV)/lib/drmrootfs
endif

#
# This pulls in the bdrm sources for nexus kernel mode build
#
ifeq ($(B_REFSW_OS), linuxkernel)
#$(warning kernel wmdrmpd)

ifeq ($(PRDY_ROBUSTNESS_ENABLE), y)
NEXUS_EXTRALIBS += $(BSEAV)/lib/drmrootfs/lib/$(B_REFSW_OS)/libdrmrootfs.a
endif


include $(BSEAV)/lib/playready/Makefile.wmdrmpd
NEXUS_WMDRMPD_CORE_DEFINES += $(WMDRMPD_CFLAGS)
NEXUS_WMDRMPD_CORE_DEFINES += B_HAS_MSDRM_PRDY=1 B_HAS_MSDRM_PD=1 
NEXUS_WMDRMPD_CORE_DEFINES += NEXUS_WMDRMPD_FILE_HANDLING

ifneq ($(PRDY_ROBUSTNESS_ENABLE), y)
NEXUS_WMDRMPD_CORE_DEFINES += PRDY_USE_SW_DECRYPTION=1
endif

NEXUS_WMDRMPD_CORE_PRIVATE_INCLUDES += $(WMDRMPD_INCLUDES)
NEXUS_WMDRMPD_CORE_SOURCES += $(WMDRMPD_SRCS)

# Guard against double inclusion of precompiled SDK libs
ifeq (,$(findstring oem_ansi,$(NEXUS_EXTRALIBS)))
NEXUS_EXTRALIBS += $(sort $(WMDRMPD_PRECOMPILED_LIBS))
endif
endif

#
# This pulls in the bdrm sources for nexus user mode build
#
ifneq ($(NEXUS_MODE),proxy)
ifeq ($(B_REFSW_OS), linuxuser)
#$(warning user wmdrmpd)

ifeq ($(PRDY_ROBUSTNESS_ENABLE), y)
NEXUS_EXTRALIBS += $(BSEAV)/lib/drmrootfs/lib/$(B_REFSW_OS)/libdrmrootfs.a
endif


include $(BSEAV)/lib/playready/Makefile.wmdrmpd
NEXUS_WMDRMPD_CORE_DEFINES += $(WMDRMPD_CFLAGS)
NEXUS_WMDRMPD_CORE_DEFINES += B_HAS_MSDRM_PRDY=1 B_HAS_MSDRM_PD=1

ifneq ($(PRDY_ROBUSTNESS_ENABLE), y)
NEXUS_WMDRMPD_CORE_DEFINES += PRDY_USE_SW_DECRYPTION=1
endif

NEXUS_WMDRMPD_CORE_PRIVATE_INCLUDES += $(WMDRMPD_INCLUDES)

ifneq ($(NEXUS_MODE),client)
NEXUS_WMDRMPD_CORE_SOURCES += $(WMDRMPD_SRCS)

# Guard against double inclusion of precompiled SDK libs
ifeq (,$(findstring oem_ansi,$(NEXUS_EXTRALIBS)))
NEXUS_EXTRALIBS += $(sort $(WMDRMPD_PRECOMPILED_LIBS))
endif
endif
endif

endif

#######################
# Module Dependencies #
#######################
NEXUS_WMDRMPD_CORE_DEPENDENCIES := BASE CORE DMA SECURITY TRANSPORT WMDRMPD_IO 

##################
# Magnum Modules #
##################
# These modules will be excluded from app-only builds


