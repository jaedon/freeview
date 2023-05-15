############################################################
#	  (c)2003-2012 Broadcom Corporation
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
#  1.	  This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#
#  2.	  TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
#  USE OR PERFORMANCE OF THE SOFTWARE.
#
#  3.	  TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
#  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
#  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
#  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
#  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
#  ANY LIMITED REMEDY.
#
# $brcm_Workfile: Makefile $
# $brcm_Revision: 11 $
# $brcm_Date: 2/20/12 1:58p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /nexus/examples/frontend/Makefile $
# 
# 11   2/20/12 1:58p erickson
# SW7552-204: update scan_ofdm for latest silicon
# 
# 10   2/17/12 4:33p erickson
# SW3461-50: add examples
# 
# 9   1/18/12 2:26p erickson
# SW3128-99: add spectrumdata
# 
# 8   5/7/10 1:41p jgarrett
# SW7550-294: Adding ofdm scan example
# 
# 7   3/22/10 6:07p ttrammel
# SW7405-3833: Modify for pre-built binary build.
# 
# THD_7550_Auto/2   3/3/10 2:09p farshidf
# SW7550-38: add scan_ofdm file
# 
# 6   10/16/09 4:41p erickson
# SW7400-2511: switch to NEXUS_PLATFORM
# 
# 5   6/4/09 10:37a jtna
# PR54416: added scan_satellite
# 
# 4   2/26/09 3:03p vishk
# PR 46436: AOB PI, Add support for BCM3117 and BCM3114.
# 
# 3   2/18/09 2:01p erickson
# PR51841: remove examples_lib
# 
# 2   2/4/09 3:51p erickson
# PR51841: refactor
#
# 1   2/4/09 2:42p erickson
# PR51841: reorganize examples
#
############################################################
# Nexus examples

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../.. && cd)
else
NEXUS_TOP := $(shell cd ../../; pwd)
endif

# include cross-compiler definitions
ifeq ($(NEXUS_PREBUILT_BINARY),y)
# do "make api" and "make nexus_headers" first, then "make NEXUS_PREBUILT_BINARY=y"
include $(NEXUS_TOP)/bin/include/platform_app.inc
Q_ ?= @
else
NEXUS_PLATFORM ?= $(PLATFORM)
ifndef NEXUS_PLATFORM
$(error NEXUS_PLATFORM is not defined)
endif

include $(NEXUS_TOP)/platforms/$(NEXUS_PLATFORM)/build/platform_app.inc
endif

THEAPPS = \
	tune_vsb \
	tune_ofdm \
	tune_qam \
	tune_satellite \
	tune_oob \
	scan_satellite \
	scan_ofdm \
	scan_ofdm_legacy \
	spectrumdata \
	scan_qam

include ../Makefile.inc
