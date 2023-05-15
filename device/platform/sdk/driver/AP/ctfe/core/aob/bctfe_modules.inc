#############################################################################
#    (c)2011 Broadcom Corporation
# 
# This program is the proprietary software of Broadcom Corporation and/or its licensors,
# and may only be used, duplicated, modified or distributed pursuant to the terms and
# conditions of a separate, written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
# no license (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software and all
# intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
# HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
# NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
#  
# Except as expressly set forth in the Authorized License,
#  
# 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
# secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of Broadcom integrated circuit products.
#  
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
# AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
# WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
# THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
# OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
# LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
# OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
# USE OR PERFORMANCE OF THE SOFTWARE.
# 
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
# LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
# EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
# USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
# THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
# ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
# LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
# ANY LIMITED REMEDY.
#
# $brcm_Workfile: bctfe_modules.inc $
# $brcm_Revision: 2 $
# $brcm_Date: 11/3/11 12:07p $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /AP/ctfe/core/aob/bctfe_modules.inc $
# 
# 2   11/3/11 12:07p farshidf
# SW3128-60: merge to main
# 
# Fw_Integration_Devel/2   11/3/11 11:46a farshidf
# SW3128-60: merge to Integ
# 
# Fw_Integration_Devel/AP_V3_0_AOB_DEV/1   10/28/11 2:36p farshidf
# SW3461-75: Move to new IRQ handler
# 
# Fw_Integration_Devel/1   10/12/11 10:40a farshidf
# SW3128-54: merge to Integ branch
# 
# Fw_Integration_Devel/AP_V2_0_AOB_DEV/1   8/30/11 10:41p mpovich
# SW3128-24: Move front-end 3128/3461 core drivers to dedicated AP VOB
#  directory.  Add support for 3128 Rev. B0 chip.
# 
# Fw_Integration_Devel/AP_V2_0_AOB_DEV/SW3128-45/1   8/30/11 1:34a mpovich
# SW3128-45: Reconfigure makefiles to build core drivers from non-chip
#  specific directories.
# 
#############################################################################
#
#  BCM CTFE AOB modules .inc makefile 
#

# The core/driver name (same as the containing directory of its source modules)
BCTFE_LIBNAME := aob

# The core/driver source modules
BCTFE_LIB_SRCS := baob_api.c baob_acquire.c baob_status.c baob_utils.c 

# The core/driver specific CPPFLAGS (note: place only core/driver specific flags here.)  
# Do NOT place platform or target specific flags here)
BCTFE_LIB_CPPFLAGS := 

# The core/driver include directories (note: Place only core/driver specific include paths here.)  
# Do NOT include platform or target specific (e.g. mangum PI) directories here)
BCTFE_INCLUDE_DIRS :=  

