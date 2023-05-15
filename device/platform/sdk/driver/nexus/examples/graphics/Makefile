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
# $brcm_Revision: 23 $
# $brcm_Date: 5/30/12 4:51p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /nexus/examples/graphics/Makefile $
# 
# 23   5/30/12 4:51p vsilyaev
# SW7425-3028: Added VP8 still support
# 
# 22   3/23/12 12:02p erickson
# SW7425-2480: add raster_ops example
# 
# 21   2/29/12 4:46p erickson
# SW7405-3671: remove show_png, build graphics_color_adj
# 
# 20   1/20/12 3:32p erickson
# SWDEPRECATED-2425: add colorbar
# 
# 19   12/8/11 11:35a erickson
# SW7425-1921: add colorkey_blit example
# 
# 18   11/29/11 2:01p erickson
# SW7425-618: move 40nm 3DTV example
# 
# 17   11/16/11 4:49p erickson
# SW7208-173: add example for scrolling an oversized framebuffer
# 
# 16   4/21/11 1:42p vsilyaev
# SW7425-404: Added 3D graphics example
# 
# 15   4/8/11 1:50p erickson
# SW7335-704: update examples for settops that have SID
#
# 14   3/16/11 5:34p erickson
# SW7420-1635: add app
#
# 13   8/17/10 4:05p erickson
# SW7420-703: update
#
# 12   8/16/10 11:08a erickson
# SW7405-3671: add packet examples
#
# 11   7/13/10 2:06p erickson
# SW7420-872: add hdsd_graphics
#
# 10   6/22/10 11:43a ttrammel
# SW7405-4530: Update NFE to Phase 7.0 release.
#
# 9   3/22/10 6:13p ttrammel
# SW7405-3833: Modify for pre-built binary build.
#
# 8   11/20/09 6:28p mphillip
# SW7405-3328: add porter-duff example to the build
#
# DirectFB_1_4_Port/1   10/30/09 2:15p robertwm
# SW7405-3328: Support Porter-Duff Fill Compositions.
#
# 7   10/16/09 4:41p erickson
# SW7400-2511: switch to NEXUS_PLATFORM
#
# 6   7/9/09 11:20a erickson
# PR35457: add surface_heap
#
# 5   5/11/09 11:46a erickson
# PR54954: added multithreaded_fill
#
# 4   2/25/09 4:56p vsilyaev
# PR 52506: Added example that shows different ways to synchronize with
#  the graphics pipeline
#
# 3   2/4/09 3:51p erickson
# PR51841: refactor
#
# 2   2/4/09 2:48p erickson
# PR51841: reorganize examples
#
# 1   2/4/09 2:43p erickson
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

show_webp: CFLAGS += -I$(NEXUS_TOP)/../BSEAV/lib/media -I$(NEXUS_TOP)/../BSEAV/lib/utils

THEAPPS = \
    blit \
    blit_with_alpha_blend \
    checkpoint \
	colorbar \
	colorkey_blit \
    fill \
    font_blit \
    framebuffer \
    graphics \
    graphics_color_adj \
    hd_graphics \
    hdsd_graphics \
    image_input \
    multithreaded_fill \
    multibuffering \
    packet_blit \
    packet_fill \
    packet_font_blit \
    palette \
    palette_blit \
    raster_ops \
    scroll_framebuffer \
    surface_heap \
    porter_duff \
    jpeg_image_input \
    picture_decoder \
    scale_jpeg \
    show_webp \
    slide_jpeg

include ../Makefile.inc
