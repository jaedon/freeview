/***************************************************************************
 *     (c)2007-2011 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: nexus_display_extensions.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 7/20/11 4:49p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/src/nexus_display_extensions.h $
 * 
 * 9   7/20/11 4:49p akam
 * SWDTV-8031: Re-organizise Nexus display extension folders.
 * 
 * 8   5/4/11 2:56p mbatchel
 * SWDTV-6916: Top level header cleanup for nexus tuning extension.
 * 
 * 7   1/14/11 6:39p mbatchel
 * SW35230-2675: Add Display extension header for RTC.
 * 
 * 6   11/18/10 6:10a petlee
 * SW35230-2089: Add new tuning file for PDP Random Positional Dither
 *  (RPD)
 * 
 * 5   9/30/10 10:15a mbatchel
 * SW35230-1452: Add VDEC tuning API.
 * 
 * 4   9/22/10 2:21p petlee
 * SW35230-1313: Add Display extension for Interpolation
 * 
 * 3   12/9/09 10:44a erickson
 * SW3556-851: added extension hook
 * 
 * 2   10/2/08 5:09p erickson
 * PR46300: added test feature 1 extension
 *
 * 1   8/21/08 3:02p erickson
 * PR45959: add sample extension
 *
 **************************************************************************/
#ifndef NEXUS_DISPLAY_EXTENSIONS_H__
#define NEXUS_DISPLAY_EXTENSIONS_H__

/* this is included by nexus_display_module.h so that the thunk layer has access to
the entire public API for the Display module, including extensions */

/* add your extension here */
#if NEXUS_DISPLAY_EXTENSION_SAMPLE
#include "nexus_sample_ext.h"
#endif

#if NEXUS_VIDEO_TEST_FEATURE1
/* this is a real extension for 3548/3556 internal test */
#include "nexus_video_test_feature1.h"
#endif

#if NEXUS_CUSTOM_VIDEO_ADJUST_EXTENSION
#include "nexus_custom_video_adjust_extension.h"
#endif

#if NEXUS_DISPLAY_EXTENSION_INTERPOLATION
#include "nexus_interpolation_ext.h"
#endif

#if NEXUS_DISPLAY_EXTENSION_TUNING_SHIM
#include "nexus_display_tuning_shim.h"
#endif

/* Include Nexus PQ customization module:
 * NEXUS_DISPLAY_EXTENSION_PQ_CUSTOMIZATION gets defined if the shell has the
 * custom PQ path exported, e.g.,
 *
 * export NEXUS_DISPLAY_CUSTOM_PQ_EXTENSION_INC=./extensions/display/35230/display_custompq_ext.inc
 */
#if NEXUS_DISPLAY_EXTENSION_PQ_CUSTOMIZATION
#include "nexus_custompq_ext.h"
#endif

#if NEXUS_DISPLAY_EXTENSION_TOP
#include "nexus_display_extension_top.h"
#endif

#endif

