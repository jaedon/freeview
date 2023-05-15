############################################################
#     (c)2003-2008 Broadcom Corporation
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
# $brcm_Workfile: b_psip_lib.inc $
# $brcm_Revision: Hydra_Software_Devel/3 $
# $brcm_Date: 11/7/08 3:04p $
#
# Module Description:
#   This file defines the sources, include paths, and defintions required
#   to use this library
#
# Revision History:
#
# Created: 03/26/2008 by Darren Tokushige
#
# $brcm_Log: /rockford/lib/psip/b_psip_lib.inc $
# 
# Hydra_Software_Devel/3   11/7/08 3:04p jrubio
# PR48782: update the path for mpeg ts lib
# 
# Hydra_Software_Devel/2   10/6/08 1:45p tokushig
# PR42421: fix memory deallocation on error conditions (check all
# pointers before freeing memory)
# 
# Hydra_Software_Devel/1   9/16/08 7:36p cttok
# PR46977: imported from nexus
# 
# 5   7/3/08 5:10p tokushig
# PR42421: added directed channel change parsing code to build
# 
# 4   6/30/08 5:04p tokushig
# PR42421: add mpeg2_ts_parse directory as a clearcase symbolic link.
#  update psip makefiles to match.
# 
# 3   6/13/08 6:12p tokushig
# PR42421: split b_psip_lib.c into multiple files, updated comments and
#  headers, removed some dead code
# 
# 2   5/5/08 11:27a tokushig
# PR42421: changes to support new location of psip example app
#  (rockford/unittests/applibs/psip)
# 
# 1   5/2/08 6:11p tokushig
# PR42421: add PSIP application library
# 
############################################################

#
# Conditional options for this makefile
#
# None at this time
#

#
# Identify this library
#
B_LIBS += B_PSIP_LIB

#
# Define source files in this library
#
B_PSIP_LIB_SOURCES += \
		    $(B_MPEG2_TS_PARSE)/ts_priv.c \
		    $(B_MPEG2_TS_PARSE)/ts_psi.c \
		    $(B_MPEG2_TS_PARSE)/ts_pat.c \
		    $(B_MPEG2_TS_PARSE)/ts_pmt.c \
		    $(B_MPEG2_TS_PARSE)/psip_mgt.c \
		    $(B_MPEG2_TS_PARSE)/psip_vct.c \
		    $(B_MPEG2_TS_PARSE)/psip_eit.c \
		    $(B_MPEG2_TS_PARSE)/psip_ett.c \
		    $(B_MPEG2_TS_PARSE)/psip_mss.c \
		    $(B_MPEG2_TS_PARSE)/psip_decode_tree_01.c \
		    $(B_MPEG2_TS_PARSE)/psip_decode_tree_02.c \
		    $(B_MPEG2_TS_PARSE)/psip_stt.c \
		    $(B_MPEG2_TS_PARSE)/psip_rrt.c \
		    $(B_MPEG2_TS_PARSE)/psip_dcct.c \
		    $(B_MPEG2_TS_PARSE)/psip_dccsct.c \
		    $(B_MPEG2_TS_PARSE)/psip_descriptor.c \
		    $(B_PSIP_TOP)/src/tsid_table.c \
		    $(B_PSIP_TOP)/src/b_psip_utils.c \
		    $(B_PSIP_TOP)/src/b_psip_stateengine.c \
		    $(B_PSIP_TOP)/src/b_psip_table.c \
		    $(B_PSIP_TOP)/src/b_psip_gettable.c \
		    $(B_PSIP_TOP)/src/b_psip_gettablever.c \
		    $(B_PSIP_TOP)/src/b_psip_getchlist.c \
		    $(B_PSIP_TOP)/src/b_psip_getproginfo.c \
		    $(B_PSIP_TOP)/src/b_psip_gettime.c \
		    $(B_PSIP_TOP)/src/b_psip_debug.c \
		    $(B_PSIP_TOP)/src/b_psip_lib.c
#
# Define public include path(s) for this library
#
B_PSIP_LIB_PUBLIC_INCLUDES += $(B_PSIP_TOP)/include  \
                              $(B_LIB_TOP)/os/include \
			      $(BSEAV)/lib/mpeg2_ts_parse 
#
# Define private include path(s) for this library
#
B_PSIP_LIB_PRIVATE_INCLUDES += $(B_PSIP_TOP)/src \
				 $(BSEAV)/lib/mpeg2_ts_parse 
				

#
# Define other libs that this library is dependent upon.  
# This will check for cyclic dependencies and setup include paths
#
B_PSIP_LIB_DEPENDENCIES := B_OS_LIB

#
# Define extra C definitions
#
#B_PSIP_LIB_DEFINES += ## N/A for this library

#
# Define possible options - this is only for debug purposes, it does _not_ actually define them.
#
#B_PSIP_LIB_OPTIONS += ## N/A for this library

