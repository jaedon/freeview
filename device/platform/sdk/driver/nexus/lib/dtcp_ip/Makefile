#############################################################################
#    (c)2012 Broadcom Corporation
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
# $brcm_Workfile: Makefile $
# $brcm_Revision: 3 $
# $brcm_Date: 10/4/12 11:29a $
# 
# Module Description:
# 
# Revision History:
# 
# $brcm_Log: /nexus/lib/dtcp_ip/Makefile $
# 
# 3   10/4/12 11:29a cdisc
# SW7425-3979: DTCP-IP streamlining
# 
# SW7425-3979/1   10/3/12 1:26p cdisc
# SW7425-3979: updating Makefile
# 
# 2   4/18/12 6:16p celysee
# SW7425-2894: Support for DTCP-IP with robustness rules
# 
# 1   6/5/09 4:06p leisun
# PR 55693: checking in DTCP-IP lib
# 
#############################################################################
#Module descriptioin: DTCP-IP Makefile for nexus build
#
#Revision History: 
#Created:
#
##############################################################

B_LIB_TOP := $(shell cd ../; pwd)

#test for DTCP-IP sources
DTCP_IP_SOURCES_AVAILABLE:=$(shell find $(B_LIB_TOP)/dtcp_ip/src -type d -empty && echo y )

ifneq ($(DTCP_IP_SOURCES_AVAILABLE),y)
install: all

all:
$(shell cp -f $(B_LIB_TOP)/dtcp_ip/bin/libb_dtcp_ip.so $(B_LIB_TOP)/../bin)

clean:
	rm -f $(B_LIB_TOP)/../bin/libb_dtcp_ip.so

endif


ifeq ($(DTCP_IP_SOURCES_AVAILABLE),y)
#include library definations
include $(B_LIB_TOP)/build/b_lib_defs.inc

#instruct the build system to build only this library and name it libb_dtcp_ip
B_BUILD_LIBS := B_DTCP_IP_LIB
B_LIB_OUTPUT_NAME := b_dtcp_ip

#include module source.
include dtcp_ip_lib.inc

#include library rules
include $(B_LIB_TOP)/build/b_lib_rules.inc

endif

