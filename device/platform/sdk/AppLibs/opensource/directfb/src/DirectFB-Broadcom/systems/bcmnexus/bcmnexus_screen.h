/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: bcmnexus_screen.h $
 * $brcm_Revision: DirectFB_1_4_15_Port/3 $
 * $brcm_Date: 3/7/12 5:26p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /AppLibs/opensource/directfb/src/broadcom_files/private/DirectFB/1.4.15/systems/bcmnexus/bcmnexus_screen.h $
 * 
 * DirectFB_1_4_15_Port/3   3/7/12 5:26p christ
 * SW7231-699: DirectFB-1.4.15: Clean up Broadcom code tree
 * 
 * DirectFB_1_4_15_Port/2   10/21/11 3:28p apandav
 * SW7420-2105: DirectFB: Add support for setting encoder aspect ratio
 * 
 * DirectFB_1_4_14_Port/1   7/28/11 11:23a christ
 * SW7425-946: DirectFB: Create AppLibs DirectFB 1.4.14 release.
 * 
 * DirectFB_1_4_5_Port/4   7/11/11 5:33p robertwm
 * SW7231-292: DirectFB-1.4.5: Ensure that outputs are not connected to
 *  the display twice.
 * 
 * DirectFB_1_4_5_Port/3   7/7/11 6:46p tross
 * SWBLURAY-26478: Merge in Bluray libinit support.
 * 
 * DirectFB_1_4_5_Port/2   6/16/11 6:19p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 * 
 * DirectFB_1_4_5_Port/1   6/14/11 5:05p christ
 * SW7425-707: DirectFB: Create DirectFB-1.4.5 Phase 2.0 Release.
 * 
 * DirectFB_1_4_Port/8   7/1/11 2:42p tross
 * SWBLURAY-26422: Merge in Bluray libinit support.
 * 
 * DirectFB_1_4_Port/7   6/16/11 5:20p robertwm
 * SW7405-5363: DirectFB-1.4.1: Cannot build system driver due to missing
 *  Nexus 3DTV functions and structures.
 * 
 * DirectFB_1_4_Port/6   6/10/11 5:17p christ
 * SW7422-138: DirecftFB: Add support for stereoscopic 3D graphics
 *  display.
 *
 * DirectFB_1_4_Port/5   6/2/11 4:37p robertwm
 * SW7405-5326: DirectFB: Re-factor platform code for new API.
 *
 ***************************************************************************/

#ifndef __CORE__BCMNEXUS__SCREEN_H__
#define __CORE__BCMNEXUS__SCREEN_H__

#include <core/core.h>
#include <core/coredefs.h>
#include <core/coretypes.h>
#include <core/screen.h>
#include <core/screens.h>
#include "bcmnexus.h"

/******************************************************************************/
/* Screen */

#define bcmnexus_sys_SUPPORTED_SCREEN_ENCODER_FLAGS ( \
        DSECONF_TV_STANDARD | \
        DSECONF_MIXER       | \
        DSECONF_SCANMODE    | \
        DSECONF_FREQUENCY   | \
        DSECONF_CONNECTORS  | \
        DSECONF_RESOLUTION  | \
        DSECONF_FRAMING     | \
        DSECONF_ASPECT_RATIO  \
        )

typedef struct {
    DFBScreenID           displayId;
    DFB_PlatformDisplaySettings displaySettings;
} bcmScreenCallSetDisplaySettings;

typedef struct {
    DFBScreenID                   displayId;
    DFB_PlatformDisplayOutputType outputType;
    int                           outputIndex;
    bool                          enable;
} bcmScreenCallEnableDisplayOutput;

typedef struct {
    DFBScreenID             id;
    DFBScreenPowerMode      powerMode;
    bool                    fixedResolution;
    DFBScreenEncoderConfig  encoderCfg;
    bool                    componentDisconnected;
} bcmScreenData;


extern DFBScreenPowerMode bcmGetPowerMode(CoreScreen *screen);
extern ScreenFuncs bcmnexusPrimaryScreenFuncs;
#if (NUM_DISPLAYS > 1)
extern ScreenFuncs bcmnexusSecondaryScreenFuncs;
#endif

#endif
