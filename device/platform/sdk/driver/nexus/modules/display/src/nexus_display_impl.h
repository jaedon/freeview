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
 * $brcm_Workfile: nexus_display_impl.h $
 * $brcm_Revision: 14 $
 * $brcm_Date: 6/15/12 12:45p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/src/nexus_display_impl.h $
 * 
 * 14   6/15/12 12:45p erickson
 * SW7425-3202: add NEXUS_VideoWindow_SetCompressionSettings
 * 
 * 13   5/15/12 3:02p erickson
 * SW7346-792: only resolve NEXUS_DisplayAspectRatio_eAuto when converting
 *  to magnum types to remove ambiguity
 * 
 * 12   11/15/11 3:27p erickson
 * SW7425-1747: remove old code
 * 
 * 11   6/12/09 11:13a erickson
 * PR55952: handle default aspect ratio for 1920x1080 display formats in a
 *  consistent way
 *
 * 10   12/1/08 2:24p erickson
 * PR49499: consolidate CUSTOM_VIDEO_ADJ code into
 *  NEXUS_VideoAdj_P_SetCustomValues function
 *
 * 9   9/25/08 9:45a erickson
 * PR47113: replace NEXUS_VideoWindow_SetScalerCoefficientIndex with
 *  NEXUS_VideoWindow_SetCoefficientIndexSettings
 *
 * 8   9/22/08 9:54a erickson
 * PR46063: support getting format info re: Custom2 inside and outside
 *  nexus
 *
 * 7   9/16/08 5:17p erickson
 * PR46967: fix naming of NEXUS_VideoWindowScalerSettings
 *
 * 6   5/23/08 10:49a erickson
 * PR42982: rework PQ Dirty logic to reapply settings on channel change
 *
 * 5   4/3/08 10:17a erickson
 * PR41122: move picture quality code to 7400
 *
 * 4   4/2/08 2:47p erickson
 * PR40950: move PictureCtrl custom data into function params
 *
 * 3   3/26/08 2:37p erickson
 * PR40950: set custom ANR/DNR data in custom function
 *
 * 2   2/29/08 5:39p erickson
 * PR40156: implement ColorMatrix function
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/6   1/8/08 3:52p erickson
 * PR36159: rename DNR/ANR/MAD functions to be VideoWindow
 *
 * Nexus_Devel/5   1/8/08 10:57a erickson
 * PR36159: refactor some 3563 Display API's
 *
 * Nexus_Devel/4   11/29/07 1:10p vsilyaev
 * PR 36159: Reworked capture function to return the Nexus Surface
 *
 * Nexus_Devel/3   11/28/07 1:44p vsilyaev
 * PR 36159: Added global UpdateMode, removed per-function ApplyChanges
 *
 * Nexus_Devel/2   11/27/07 10:58a vsilyaev
 * PR36159: Fixed compile time warnings
 *
 * Nexus_Devel/1   11/26/07 3:26p vsilyaev
 * PR36159: PR36159: Bring up Nexus on 93563 platform
 *
 * Nexus_Devel/PR36159/1   11/26/07 10:06a gezhang
 * PR36159: Bring up Nexus on 93563 platform
 *
 * Nexus_Devel/1   10/18/07 9:59p gezhang
 * PR36159: Bring up Nexus on 93563 platform
 *
 **************************************************************************/
#ifndef NEXUS_DISPLAY_IMPL_H__
#define NEXUS_DISPLAY_IMPL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*=*
Priv including file for nexus_display.c
**/

/* VideoAdj private context */
typedef struct NEXUS_VideoAdjContext
{
    NEXUS_VideoWindowDnrSettings    stDnrSettings;
    NEXUS_VideoWindowMadSettings    stMadSettings;
    NEXUS_VideoWindowScalerSettings stSclSettings;
    NEXUS_VideoWindowAnrSettings    stAnrSettings;
    NEXUS_VideoWindowGameModeDelay  stGameModeDelaySettings;
    NEXUS_VideoWindowCoefficientIndexSettings coefficientIndexSettings;
    NEXUS_VideoWindowCompressionSettings compressionSettings;

    /* Flags to identify settings that were set before a VDC window was created */
    bool    bDnrSet;
    bool    bMadSet;
    bool    bSclSet;
    bool    bAnrSet;
    bool    bGameModeDelaySet;
    bool    bCoefficientIndexSettingsSet;

    /* we copy custom DNR and ANR data into BKNI_Malloc'd memory */
    void *customDnrData;
    void *customAnrData;
} NEXUS_VideoAdjContext;


/* PictureCtrl private context */
typedef struct NEXUS_PictureCtrlContext
{
    /* color settings */
    NEXUS_PictureCtrlCommonSettings  stCommonSettings;
    /* advanced color settings */
    NEXUS_PictureCtrlAdvColorSettings   stAdvColorSettings;
    /* Customized settings */
    NEXUS_PictureCtrlSharpnessValue stCustomSharpness;
    NEXUS_PictureCtrlContrastStretch stCustomContrast;
    /* Color management settings */
    NEXUS_PictureCtrlCmsSettings stCmsSettings;
    /* Dither settings */
    NEXUS_PictureCtrlDitherSettings stDitherSettings;

    /* Flags to identify settings that were set before a VDC window was created */
    bool    bCommonSet;
    bool    bAdvColorSet;
    bool    bCustomSharpnessSet;
    bool    bCustomContrastSet;
    bool    bCmsSet;
    bool    bDitherSet;

    /* we copy custom contrast stretch data into BKNI_Malloc'd memory */
    void *customContrastStretchData;
} NEXUS_PictureCtrlContext;

NEXUS_Error NEXUS_PictureCtrl_P_ApplySetSettings( NEXUS_VideoWindowHandle window );
NEXUS_Error NEXUS_VideoAdj_P_ApplySetSettings( NEXUS_VideoWindowHandle window );
NEXUS_Error NEXUS_P_Display_GetMagnumVideoFormatInfo(NEXUS_DisplayHandle display, NEXUS_VideoFormat videoFormat, BFMT_VideoInfo *pInfo);

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_DISPLAY_IMPL_H__ */

