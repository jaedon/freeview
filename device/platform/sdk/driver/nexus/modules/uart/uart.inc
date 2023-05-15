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
# $brcm_Workfile: uart.inc $
# $brcm_Revision: 3 $
# $brcm_Date: 7/16/12 1:20p $
#
# Module Description:
#
# Revision History:
#
# $brcm_Log: /nexus/modules/uart/uart.inc $
# 
# 3   7/16/12 1:20p erickson
# SW7435-235: refactor directories to remove symbolic links
# 
# 2   3/9/09 12:55p erickson
# PR52362: merge
# 
# PR52362/1   2/20/09 5:11p cnovak
# PR52362: Tell burt.c to use interrupt mode.
# 
# 1   1/18/08 2:17p jgarrett
# PR 38808: Merging to main branch
# 
# Nexus_Devel/1   11/20/07 1:28p erickson
# PR37423: added uart, gpio, spi modules
# 
############################################################

#
# Conditional Options for this Makefile
#

######################
# Define this module #
######################
NEXUS_MODULES += UART

##################
# Module Options #
##################

###################
# Module Includes #
###################
NEXUS_UART_PUBLIC_INCLUDES = $(NEXUS_TOP)/modules/uart/include

##################
# Module Sources #
##################
NEXUS_UART_SOURCES := $(NEXUS_TOP)/modules/uart/src/nexus_uart.c

# thunk layer
NEXUS_UART_PRIVATE_INCLUDES += $(NEXUS_TOP)/modules/uart/src $(NEXUS_SYNCTHUNK_DIR)
NEXUS_UART_SOURCES += $(NEXUS_SYNCTHUNK_DIR)/nexus_uart_thunks.c

#######################
# Module Dependencies #
#######################
NEXUS_UART_DEPENDENCIES := BASE CORE

##################
# Magnum Modules #
##################
NEXUS_UART_DEFINES += USE_BURT_INT_CALLBACK

# These modules will be excluded from app-only builds
NEXUS_UART_MAGNUM_MODULES =  \
    $(MAGNUM)/portinginterface/urt/burt.inc