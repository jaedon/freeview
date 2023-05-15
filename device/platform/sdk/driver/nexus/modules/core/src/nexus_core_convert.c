/***************************************************************************
*     (c)2008-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_core_convert.c $
* $brcm_Revision: 38 $
* $brcm_Date: 8/16/12 2:49p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/src/nexus_core_convert.c $
* 
* 38   8/16/12 2:49p erickson
* SW7435-276: add 482/483 NEXUS_VideoFormat enums
*
* 37   6/1/12 11:38a vsilyaev
* SW7425-3103: Added conversion fo 15 and 20 FPS
*
* 36   5/24/12 1:12p vsilyaev
* SW7425-3103: Added 15 and 20 FPS frame rates
*
* 35   5/10/12 12:13p jtna
* SW7429-143: add CASSERT back in
*
* 34   5/8/12 2:46p erickson
* SW7435-27: allow file to be used locally
*
* 33   5/8/12 11:14a jtna
* SW7429-143: comment out until magnum label moves for all platforms
*
* 32   5/7/12 2:22p jtna
* SW7429-143: added NEXUS_VideoFrameRate_e10
*
* 31   3/14/12 12:38p erickson
* SW7401-4517: remove old code
*
* 30   2/24/12 1:59p vsilyaev
* SW7425-2258: Use relaxed match for framerate lookup
*
* 29   1/23/12 4:34p erickson
* SW7435-27: rearrange so nexus_core_convert.c has no public functions
*
* 28   6/30/11 5:15p erickson
* SW7346-299: fix NEXUS_VideoFrameRate_e7_493 typo
*
* 27   5/20/11 11:37a erickson
* SW7425-467: define NEXUS_VideoFormat_eCustom0, 1, 2 for all platforms
*
* 26   5/11/11 10:52a erickson
* SW7401-4501: add NEXUS_PixelFormat_eL8
*
* 25   4/25/11 2:23p jtna
* SW7425-365: silence more build warnings for 2.6.37 kernel
*
* 24   4/25/11 1:52p erickson
* SW7630-104: Added support for A8_Y8, Cb8 and Cr8 formats.
*
* 23   4/22/11 5:16p vsilyaev
* SW7425-40: Use compile-time conversion for video framerates
*
* 22   4/22/11 3:24p vsilyaev
* SW7425-416: Removed 3DLeft/3DRight
*
* 21   4/20/11 6:16p jtna
* SW7425-365: fix build warnings for 2.6.37 kernel
*
* 20   4/18/11 2:04p jhaberf
* SWDTV-4721: Updated with some DTV formats
*
* 19   4/15/11 7:40p vsilyaev
* SW7425-232: Move video codec conversion to local file
*
* 18   3/2/11 4:20p erickson
* SW7405-3671: add three pixel formats
*
* 17   12/9/10 8:05p vsilyaev
* SW7425-40: Added new types for the video encoder
*
* 16   11/12/10 11:12a vsilyaev
* SW7422-64: Merged 3D types to follow BFMT
*
* 15   10/25/10 7:02p vsilyaev
* SW7422-64: Added 3D video format and orientation
*
* 14   9/22/10 11:33a jhaberf
* SW35230-765: Added #ifdef's around some formats since 35230 is using a
*  branched version of bfmt.h
*
* 13   9/21/10 6:24p jhaberf
* SW35230-765: Add new PC formats
*
* 12   9/21/10 5:22p jhaberf
* SW35230-1185: Added 3D modes supported by bcm35425 chip
*
* 11   9/13/10 10:18a erickson
* SW7405-4864: fix alpha_per_pixel for palettized pixel formats
*
* 10   8/19/10 2:09p petlee
* SW35230-1059: Add new video formats to conversion table
*
* 9   8/19/10 10:49a erickson
* SW7405-3671: fix g_pixelFormatInfo for ARGB8888 formats
*
* 8   8/16/10 11:02a erickson
* SW7405-3671: expand and rearrange NEXUS_PixelFormat for compatibility
*  with M2MC packet blit
*
* 7   6/11/10 2:31p erickson
* SW3556-1124: add NEXUS_P_RefreshRate_FromFrameRate. removed unneeded
*  function.
*
* 6   3/3/10 10:03a erickson
* SW35230-109: add BGR pixel formats
*
* 5   2/24/10 2:34p erickson
* SW3548-2764: added NEXUS_VideoFormat_e1440x576p50hz
*
* BCM3D/3   1/29/10 7:01p hongtaoz
* SW3548-2680, SW3548-2681: ermoved 1470p custom formats; wrapped 2160i
*  custom format with compile flag for legacy 3DTV chipsets;
*
* BCM3D/2   1/20/10 10:39a erickson
* SW3548-2727: add more 3D TV types
*
* BCM3D/1   1/19/10 2:31p erickson
* SW3548-2727: add 3DTV formats
*
* 3   12/29/09 2:47p erickson
* SW7420-515: remove NEXUS_PixelFormat_eA0 and
*  NEXUS_PixelFormat_eVideoTunnel
*
* 2   12/17/09 4:22p erickson
* SW3556-933: fixed NEXUS_VideoFormat_e1440x480p60hz
*
* 1   12/17/09 4:20p erickson
* SW3556-933: added new format. moved all type lookup and conversion code
*  into separate .c so we don't have to rev nexus_core.c so often.
*
***************************************************************************/
/* this file should not #include nexus_core_modules.h so that it can be used locally.
this means it cannot have public API's. */
#include "nexus_base.h"
#include "nexus_types.h"
#include "priv/nexus_core.h"
#include "bfmt.h"

BDBG_MODULE(nexus_core_convert);

static const struct {
    NEXUS_TransportType transportType;
    BAVC_StreamType streamType;
} g_NEXUS_transportTypes[] = {
    {NEXUS_TransportType_eEs, BAVC_StreamType_eEs},
    {NEXUS_TransportType_eTs, BAVC_StreamType_eTsMpeg},
    {NEXUS_TransportType_eMpeg2Pes, BAVC_StreamType_ePes},
    {NEXUS_TransportType_eVob, BAVC_StreamType_ePes},
    {NEXUS_TransportType_eMpeg1Ps, BAVC_StreamType_eMpeg1System}, /* ? */
    {NEXUS_TransportType_eDssEs, BAVC_StreamType_eDssEs},
    {NEXUS_TransportType_eDssPes, BAVC_StreamType_eDssPes},
    /* {NEXUS_TransportType_eAsf, xxx}, - ASF is not supported in Magnum */
    /* {NEXUS_TransportType_eFlv, xxx}, - Flv is not supported in Magnum */
    {NEXUS_TransportType_eAvi, BAVC_StreamType_eAVI},
    {NEXUS_TransportType_eMp4, BAVC_StreamType_eMPEG4},
    {NEXUS_TransportType_eMkv, BAVC_StreamType_eMKV}
};

NEXUS_Error
NEXUS_P_TransportType_ToMagnum(NEXUS_TransportType transportType, BAVC_StreamType *streamType)
{
    unsigned i;
    for (i=0;i<sizeof(g_NEXUS_transportTypes)/sizeof(g_NEXUS_transportTypes[0]);i++) {
        if (g_NEXUS_transportTypes[i].transportType == transportType) {
            *streamType = g_NEXUS_transportTypes[i].streamType;
            return NEXUS_SUCCESS;
        }
    }
    BDBG_WRN(("unknown transport type nexus:%u ",transportType));
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

static const struct {
    BFMT_VideoFmt mformat;
    NEXUS_VideoFormat nformat;
} b_formats[] = {
    {BFMT_VideoFmt_eNTSC                      , NEXUS_VideoFormat_eNtsc},
    {BFMT_VideoFmt_eNTSC_J                    , NEXUS_VideoFormat_eNtscJapan},
    {BFMT_VideoFmt_eNTSC_443                  , NEXUS_VideoFormat_eNtsc443},
    {BFMT_VideoFmt_ePAL_60                    , NEXUS_VideoFormat_ePal60hz},
    {BFMT_VideoFmt_ePAL_G                     , NEXUS_VideoFormat_ePal},
    {BFMT_VideoFmt_ePAL_N                     , NEXUS_VideoFormat_ePalN},
    {BFMT_VideoFmt_ePAL_M                     , NEXUS_VideoFormat_ePalM},
    {BFMT_VideoFmt_ePAL_NC                    , NEXUS_VideoFormat_ePalNc},
    {BFMT_VideoFmt_ePAL_B                     , NEXUS_VideoFormat_ePalB},
    {BFMT_VideoFmt_ePAL_B1                    , NEXUS_VideoFormat_ePalB1},
    {BFMT_VideoFmt_ePAL_D                     , NEXUS_VideoFormat_ePalD},
    {BFMT_VideoFmt_ePAL_D1                    , NEXUS_VideoFormat_ePalD1},
    {BFMT_VideoFmt_ePAL_D                     , NEXUS_VideoFormat_ePalDK2},
    {BFMT_VideoFmt_ePAL_D                     , NEXUS_VideoFormat_ePalDK3},
    {BFMT_VideoFmt_ePAL_G                     , NEXUS_VideoFormat_ePalG},
    {BFMT_VideoFmt_ePAL_H                     , NEXUS_VideoFormat_ePalH},
    {BFMT_VideoFmt_ePAL_K                     , NEXUS_VideoFormat_ePalK},
    {BFMT_VideoFmt_ePAL_I                     , NEXUS_VideoFormat_ePalI},
    {BFMT_VideoFmt_eSECAM_L                   , NEXUS_VideoFormat_eSecamL},
    {BFMT_VideoFmt_eSECAM_B                   , NEXUS_VideoFormat_eSecamB},
    {BFMT_VideoFmt_eSECAM_G                   , NEXUS_VideoFormat_eSecamG},
    {BFMT_VideoFmt_eSECAM_D                   , NEXUS_VideoFormat_eSecamD},
    {BFMT_VideoFmt_eSECAM_K                   , NEXUS_VideoFormat_eSecamK},
    {BFMT_VideoFmt_eSECAM_H                   , NEXUS_VideoFormat_eSecamH},
    {BFMT_VideoFmt_e1080i                     , NEXUS_VideoFormat_e1080i},
    {BFMT_VideoFmt_e1080p_24Hz                , NEXUS_VideoFormat_e1080p24hz},
    {BFMT_VideoFmt_e1080p_25Hz                , NEXUS_VideoFormat_e1080p25hz},
    {BFMT_VideoFmt_e1080p_30Hz                , NEXUS_VideoFormat_e1080p30hz},
    {BFMT_VideoFmt_e1080i_50Hz                , NEXUS_VideoFormat_e1080i50hz},
    {BFMT_VideoFmt_e1080p                     , NEXUS_VideoFormat_e1080p60hz},
    {BFMT_VideoFmt_e720p                      , NEXUS_VideoFormat_e720p},
    {BFMT_VideoFmt_e720p_24Hz                 , NEXUS_VideoFormat_e720p24hz},
    {BFMT_VideoFmt_e720p_25Hz                 , NEXUS_VideoFormat_e720p25hz},
    {BFMT_VideoFmt_e720p_30Hz                 , NEXUS_VideoFormat_e720p30hz},
    {BFMT_VideoFmt_e720p_50Hz                 , NEXUS_VideoFormat_e720p50hz},
    {BFMT_VideoFmt_e1250i_50Hz                , NEXUS_VideoFormat_e1250i50hz},
    {BFMT_VideoFmt_e480p                      , NEXUS_VideoFormat_e480p},
    {BFMT_VideoFmt_e576p_50Hz                 , NEXUS_VideoFormat_e576p},
    {BFMT_VideoFmt_eCUSTOM_1440x240p_60Hz     , NEXUS_VideoFormat_eCustomer1440x240p60hz},
    {BFMT_VideoFmt_eCUSTOM_1440x288p_50Hz     , NEXUS_VideoFormat_eCustomer1440x288p50hz},
    {BFMT_VideoFmt_eCUSTOM_1366x768p          , NEXUS_VideoFormat_eCustomer1366x768p60hz},
    {BFMT_VideoFmt_eCUSTOM_1366x768p_50Hz     , NEXUS_VideoFormat_eCustomer1366x768p50hz},
    {BFMT_VideoFmt_eDVI_640x480p              , NEXUS_VideoFormat_eVesa640x480p60hz},
    {BFMT_VideoFmt_eDVI_800x600p              , NEXUS_VideoFormat_eVesa800x600p60hz},
    {BFMT_VideoFmt_eDVI_1024x768p             , NEXUS_VideoFormat_eVesa1024x768p60hz},
    {BFMT_VideoFmt_eDVI_1280x768p             , NEXUS_VideoFormat_eVesa1280x768p60hz},
    {BFMT_VideoFmt_e1080p_50Hz                , NEXUS_VideoFormat_e1080p50hz},
    {BFMT_VideoFmt_e240p_60Hz                 , NEXUS_VideoFormat_e240p60hz},
    {BFMT_VideoFmt_e288p_50Hz                 , NEXUS_VideoFormat_e288p50hz},
    {BFMT_VideoFmt_e1440x480p_60Hz            , NEXUS_VideoFormat_e1440x480p60hz},
    {BFMT_VideoFmt_e1440x576p_50Hz            , NEXUS_VideoFormat_e1440x576p50hz},
    {BFMT_VideoFmt_eDVI_640x350p_60Hz         , NEXUS_VideoFormat_eVesa640x350p60hz},
    {BFMT_VideoFmt_eDVI_640x350p_70Hz         , NEXUS_VideoFormat_eVesa640x350p70hz},
    {BFMT_VideoFmt_eDVI_640x350p_72Hz         , NEXUS_VideoFormat_eVesa640x350p72hz},
    {BFMT_VideoFmt_eDVI_640x350p_75Hz         , NEXUS_VideoFormat_eVesa640x350p75hz},
    {BFMT_VideoFmt_eDVI_640x350p_85Hz         , NEXUS_VideoFormat_eVesa640x350p85hz},
    {BFMT_VideoFmt_eDVI_640x400p_60Hz         , NEXUS_VideoFormat_eVesa640x400p60hz},
    {BFMT_VideoFmt_eDVI_640x400p_70Hz         , NEXUS_VideoFormat_eVesa640x400p70hz},
    {BFMT_VideoFmt_eDVI_640x400p_72Hz         , NEXUS_VideoFormat_eVesa640x400p72hz},
    {BFMT_VideoFmt_eDVI_640x400p_75Hz         , NEXUS_VideoFormat_eVesa640x400p75hz},
    {BFMT_VideoFmt_eDVI_640x400p_85Hz         , NEXUS_VideoFormat_eVesa640x400p85hz},
    {BFMT_VideoFmt_eDVI_640x480p_66Hz         , NEXUS_VideoFormat_eVesa640x480p66hz},
    {BFMT_VideoFmt_eDVI_640x480p_70Hz         , NEXUS_VideoFormat_eVesa640x480p70hz},
    {BFMT_VideoFmt_eDVI_640x480p_72Hz         , NEXUS_VideoFormat_eVesa640x480p72hz},
    {BFMT_VideoFmt_eDVI_640x480p_75Hz         , NEXUS_VideoFormat_eVesa640x480p75hz},
    {BFMT_VideoFmt_eDVI_640x480p_85Hz         , NEXUS_VideoFormat_eVesa640x480p85hz},
    {BFMT_VideoFmt_eDVI_720x400p_60Hz         , NEXUS_VideoFormat_eVesa720x400p60hz},
    {BFMT_VideoFmt_eDVI_720x400p_70Hz         , NEXUS_VideoFormat_eVesa720x400p70hz},
    {BFMT_VideoFmt_eDVI_720x400p_72Hz         , NEXUS_VideoFormat_eVesa720x400p72hz},
    {BFMT_VideoFmt_eDVI_720x400p_75Hz         , NEXUS_VideoFormat_eVesa720x400p75hz},
    {BFMT_VideoFmt_eDVI_720x400p_85Hz         , NEXUS_VideoFormat_eVesa720x400p85hz},
    {BFMT_VideoFmt_eDVI_800x600p_56Hz         , NEXUS_VideoFormat_eVesa800x600p56hz},
    {BFMT_VideoFmt_eDVI_800x600p_59Hz_Red     , NEXUS_VideoFormat_eVesa800x600p59hzRed},
    {BFMT_VideoFmt_eDVI_800x600p_70Hz         , NEXUS_VideoFormat_eVesa800x600p70hz},
    {BFMT_VideoFmt_eDVI_800x600p_72Hz         , NEXUS_VideoFormat_eVesa800x600p72hz},
    {BFMT_VideoFmt_eDVI_800x600p_75Hz         , NEXUS_VideoFormat_eVesa800x600p75hz},
    {BFMT_VideoFmt_eDVI_800x600p_85Hz         , NEXUS_VideoFormat_eVesa800x600p85hz},
    {BFMT_VideoFmt_eDVI_848x480p_60Hz         , NEXUS_VideoFormat_eVesa848x480p60hz},
    {BFMT_VideoFmt_eDVI_1024x768p_66Hz        , NEXUS_VideoFormat_eVesa1024x768p66hz},
    {BFMT_VideoFmt_eDVI_1024x768p_70Hz        , NEXUS_VideoFormat_eVesa1024x768p70hz},
    {BFMT_VideoFmt_eDVI_1024x768p_72Hz        , NEXUS_VideoFormat_eVesa1024x768p72hz},
    {BFMT_VideoFmt_eDVI_1024x768p_75Hz        , NEXUS_VideoFormat_eVesa1024x768p75hz},
    {BFMT_VideoFmt_eDVI_1024x768p_85Hz        , NEXUS_VideoFormat_eVesa1024x768p85hz},
    {BFMT_VideoFmt_eDVI_1280x720p_50Hz        , NEXUS_VideoFormat_eVesa1280x720p50hz},
    {BFMT_VideoFmt_eDVI_1280x720p             , NEXUS_VideoFormat_eVesa1280x720p60hz},
    {BFMT_VideoFmt_eDVI_1280x720p_ReducedBlank, NEXUS_VideoFormat_eVesa1280x720pReducedBlank},
    {BFMT_VideoFmt_eDVI_1280x720p_70Hz        , NEXUS_VideoFormat_eVesa1280x720p70hz},
    {BFMT_VideoFmt_eDVI_1280x720p_72Hz        , NEXUS_VideoFormat_eVesa1280x720p72hz},
    {BFMT_VideoFmt_eDVI_1280x720p_75Hz        , NEXUS_VideoFormat_eVesa1280x720p75hz},
    {BFMT_VideoFmt_eDVI_1280x720p_85Hz        , NEXUS_VideoFormat_eVesa1280x720p85hz},
    {BFMT_VideoFmt_eDVI_1280x768p_Red         , NEXUS_VideoFormat_eVesa1280x768p60hzRed},
    {BFMT_VideoFmt_eDVI_1064x600p_60Hz        , NEXUS_VideoFormat_eVesa1064x600p60hz},
    {BFMT_VideoFmt_eDVI_1024x768i_87Hz        , NEXUS_VideoFormat_eVesa1024x768i87hz},
    {BFMT_VideoFmt_eDVI_1152x864p_75Hz        , NEXUS_VideoFormat_eVesa1152x864p75hz},
    {BFMT_VideoFmt_eDVI_1280x768p_75Hz        , NEXUS_VideoFormat_eVesa1280x768p75hz},
    {BFMT_VideoFmt_eDVI_1280x768p_85Hz        , NEXUS_VideoFormat_eVesa1280x768p85hz},
    {BFMT_VideoFmt_eDVI_1280x960p_60Hz        , NEXUS_VideoFormat_eVesa1280x960p60hz},
    {BFMT_VideoFmt_eDVI_1280x960p_85Hz        , NEXUS_VideoFormat_eVesa1280x960p85hz},
    {BFMT_VideoFmt_eDVI_1280x1024p_60Hz       , NEXUS_VideoFormat_eVesa1280x1024p60hz},
    {BFMT_VideoFmt_eDVI_1280x1024p_69Hz       , NEXUS_VideoFormat_eVesa1280x1024p69hz},
    {BFMT_VideoFmt_eDVI_1280x1024p_75Hz       , NEXUS_VideoFormat_eVesa1280x1024p75hz},
    {BFMT_VideoFmt_eDVI_1280x1024p_85Hz       , NEXUS_VideoFormat_eVesa1280x1024p85hz},
    {BFMT_VideoFmt_eDVI_832x624p_75Hz         , NEXUS_VideoFormat_eVesa832x624p75hz},
    {BFMT_VideoFmt_eDVI_1360x768p_60Hz        , NEXUS_VideoFormat_eVesa1360x768p60hz},
    {BFMT_VideoFmt_eDVI_1366x768p_60Hz        , NEXUS_VideoFormat_eVesa1366x768p60hz},
    {BFMT_VideoFmt_eDVI_1400x1050p_60Hz       , NEXUS_VideoFormat_eVesa1400x1050p60hz},
    {BFMT_VideoFmt_eDVI_1400x1050p_60Hz_Red   , NEXUS_VideoFormat_eVesa1400x1050p60hzReducedBlank},
    {BFMT_VideoFmt_eDVI_1400x1050p_75Hz       , NEXUS_VideoFormat_eVesa1400x1050p75hz},
    {BFMT_VideoFmt_eDVI_1440x900p_60Hz        , NEXUS_VideoFormat_eVesa1440x900p60hz},
    {BFMT_VideoFmt_eDVI_1600x1200p_60Hz       , NEXUS_VideoFormat_eVesa1600x1200p60hz},
    {BFMT_VideoFmt_eDVI_1920x1080p_60Hz_Red   , NEXUS_VideoFormat_eVesa1920x1080p60hzReducedBlank},

    {BFMT_VideoFmt_e720p_60Hz_3DOU_AS         , NEXUS_VideoFormat_e720p_3DOU_AS},
    {BFMT_VideoFmt_e720p_50Hz_3DOU_AS         , NEXUS_VideoFormat_e720p50hz_3DOU_AS},
#if NEXUS_DTV_PLATFORM > 2
    {BFMT_VideoFmt_e720p_30Hz_3DOU_AS         , NEXUS_VideoFormat_e720p30hz_3DOU_AS},
    {BFMT_VideoFmt_e720p_24Hz_3DOU_AS         , NEXUS_VideoFormat_e720p24hz_3DOU_AS},
#endif
    {BFMT_VideoFmt_e1080p_24Hz_3DOU_AS        , NEXUS_VideoFormat_e1080p24hz_3DOU_AS},
    {BFMT_VideoFmt_e1080p_30Hz_3DOU_AS        , NEXUS_VideoFormat_e1080p30hz_3DOU_AS},

#if NEXUS_DTV_PLATFORM > 2
    {BFMT_VideoFmt_eDVI_1280x800p_60Hz        , NEXUS_VideoFormat_eVesa1280x768p85hz},
    {BFMT_VideoFmt_eDVI_1680x1050p_60Hz       , NEXUS_VideoFormat_eVesa1680x1050p_60Hz,},
    {BFMT_VideoFmt_eDVI_1280x800p_60Hz_Red    , NEXUS_VideoFormat_eVesa1280x800p_60Hz_Red},
    {BFMT_VideoFmt_eDVI_1600x1200p_75Hz       , NEXUS_VideoFormat_eVesa1600x1200p_75Hz},
    {BFMT_VideoFmt_eDVI_1600x900p_60Hz_Red    , NEXUS_VideoFormat_eVesa1600x900p_60Hz_Red},
    {BFMT_VideoFmt_eDVI_1680x1050p_60Hz_Red   , NEXUS_VideoFormat_eVesa1680x1050p_60Hz_Red},
    {BFMT_VideoFmt_eDVI_1920x1200p_60Hz       , NEXUS_VideoFormat_eVesa1920x1200p_60Hz},
    {BFMT_VideoFmt_eDVI_1920x1200p_60Hz_Red   , NEXUS_VideoFormat_eVesa1920x1200p_60Hz_Red},
    {BFMT_VideoFmt_eDVI_1152x720p_60Hz        , NEXUS_VideoFormat_eVesa1152x720p_60Hz},
    {BFMT_VideoFmt_eDVI_1152x720p_75Hz        , NEXUS_VideoFormat_eVesa1152x720p_75Hz},
    {BFMT_VideoFmt_eDVI_1152x720p_85Hz        , NEXUS_VideoFormat_eVesa1152x720p_85Hz},
    {BFMT_VideoFmt_eDVI_1152x864p_60Hz        , NEXUS_VideoFormat_eVesa1152x864p_60Hz},
    {BFMT_VideoFmt_eDVI_1152x864p_85Hz        , NEXUS_VideoFormat_eVesa1152x864p_85Hz},
    {BFMT_VideoFmt_eDVI_1152x870p_75Hz        , NEXUS_VideoFormat_eVesa1152x870p_75Hz},
    {BFMT_VideoFmt_eDVI_1152x900p_66Hz        , NEXUS_VideoFormat_eVesa1152x900p_66Hz},
    {BFMT_VideoFmt_eDVI_1152x900p_76Hz        , NEXUS_VideoFormat_eVesa1152x900p_76Hz},
    {BFMT_VideoFmt_eDVI_1170x584p_50Hz        , NEXUS_VideoFormat_eVesa1170x584p_50Hz},
    {BFMT_VideoFmt_eDVI_1280x1024p_70Hz       , NEXUS_VideoFormat_eVesa1280x1024p_70Hz},
    {BFMT_VideoFmt_eDVI_1280x1024p_72Hz       , NEXUS_VideoFormat_eVesa1280x1024p_72Hz},
    {BFMT_VideoFmt_eDVI_1280x1024p_76Hz       , NEXUS_VideoFormat_eVesa1280x1024p_76Hz},
    {BFMT_VideoFmt_eDVI_1280x768p_50Hz        , NEXUS_VideoFormat_eVesa1280x768p_50Hz},
    {BFMT_VideoFmt_eDVI_1280x960p_75Hz        , NEXUS_VideoFormat_eVesa1280x960p_75Hz},
    {BFMT_VideoFmt_eDVI_1600x1024p_60Hz       , NEXUS_VideoFormat_eVesa1600x1024p_60Hz},
    {BFMT_VideoFmt_eDVI_1600x1024p_76Hz       , NEXUS_VideoFormat_eVesa1600x1024p_76Hz},
    {BFMT_VideoFmt_eDVI_1728x1080p_60Hz       , NEXUS_VideoFormat_eVesa1728x1080p_60Hz},
    {BFMT_VideoFmt_eDVI_800x600p_100Hz        , NEXUS_VideoFormat_eVesa800x600p_100Hz},
    {BFMT_VideoFmt_eDVI_800x600p_90Hz         , NEXUS_VideoFormat_eVesa800x600p_90Hz},
    {BFMT_VideoFmt_eDVI_848x480p_75Hz         , NEXUS_VideoFormat_eVesa848x480p_75Hz},
    {BFMT_VideoFmt_eDVI_848x480p_85Hz         , NEXUS_VideoFormat_eVesa848x480p_85Hz},
    {BFMT_VideoFmt_eDVI_852x480p_60Hz         , NEXUS_VideoFormat_eVesa852x480p_60Hz},
#endif
    {BFMT_VideoFmt_e720x482_NTSC, NEXUS_VideoFormat_e720x482_NTSC},
    {BFMT_VideoFmt_e720x482_NTSC_J, NEXUS_VideoFormat_e720x482_NTSC_J},
    {BFMT_VideoFmt_e720x483p, NEXUS_VideoFormat_e720x483p},

    {BFMT_VideoFmt_eCustom0                   , NEXUS_VideoFormat_eCustom0},
    {BFMT_VideoFmt_eCustom1                   , NEXUS_VideoFormat_eCustom1},
    {BFMT_VideoFmt_eCustom2                   , NEXUS_VideoFormat_eCustom2}

#if NEXUS_DTV_PLATFORM
    , /* continue the list */

    /* 3D display & source */
#if BFMT_LEGACY_3DTV_SUPPORT
    {BFMT_VideoFmt_eCUSTOM1920x2160i_48Hz     , NEXUS_VideoFormat_eCustom1920x2160i_48Hz},
#endif

#if BFMT_BCM35425_3DTV_SUPPORT
    {BFMT_VideoFmt_eCUSTOM_3D_720p            , NEXUS_VideoFormat_eCustom_3D_720p},
    {BFMT_VideoFmt_eCUSTOM_3D_720p_50Hz       , NEXUS_VideoFormat_eCustom_3D_720p_50hz},
    {BFMT_VideoFmt_eCUSTOM_3D_720p_30Hz       , NEXUS_VideoFormat_eCustom_3D_720p_30hz},
    {BFMT_VideoFmt_eCUSTOM_3D_720p_24Hz       , NEXUS_VideoFormat_eCustom_3D_720p_24hz},
    {BFMT_VideoFmt_eCUSTOM_3D_1080p_24Hz      , NEXUS_VideoFormat_eCustom_3D_1080p_24hz},
    {BFMT_VideoFmt_eCUSTOM_3D_1080p_30Hz      , NEXUS_VideoFormat_eCustom_3D_1080p_30hz},
#endif

    {BFMT_VideoFmt_e3D_720p                   , NEXUS_VideoFormat_e3D_720p},
    {BFMT_VideoFmt_e3D_720p_50Hz              , NEXUS_VideoFormat_e3D_720p_50Hz},
#if NEXUS_DTV_PLATFORM > 2
    /* for DTV only */
    {BFMT_VideoFmt_e3D_720p_30Hz              , NEXUS_VideoFormat_e3D_720p_30Hz},
    {BFMT_VideoFmt_e3D_720p_24Hz              , NEXUS_VideoFormat_e3D_720p_24Hz},
#endif
    {BFMT_VideoFmt_e3D_1080p_24Hz             , NEXUS_VideoFormat_e3D_1080p_24Hz},
    {BFMT_VideoFmt_e3D_1080p_30Hz             , NEXUS_VideoFormat_e3D_1080p_30Hz}
#endif
};

NEXUS_Error
NEXUS_P_VideoFormat_ToMagnum(NEXUS_VideoFormat format, BFMT_VideoFmt *mformat)
{
    unsigned i;
    for(i=0;i<sizeof(b_formats)/sizeof(*b_formats);i++) {
        if (format==b_formats[i].nformat) {
            *mformat = b_formats[i].mformat;
            return BERR_SUCCESS;
        }
    }
    /* fail silently. this is a utility function and querying an unsupported format is not necessarily an ERR. */
    return BERR_INVALID_PARAMETER;
}

NEXUS_VideoFormat
NEXUS_P_VideoFormat_FromMagnum(BFMT_VideoFmt format)
{
    unsigned i;
    for(i=0;i<sizeof(b_formats)/sizeof(*b_formats);i++) {
        if (format==b_formats[i].mformat) {
            return b_formats[i].nformat;
        }
    }
    return NEXUS_VideoFormat_eUnknown;
}


NEXUS_AspectRatio
NEXUS_P_AspectRatio_FromMagnum(BFMT_AspectRatio eAspectRatio)
{
    BDBG_CASSERT(BFMT_AspectRatio_eSAR == (BFMT_AspectRatio)NEXUS_AspectRatio_eSar);
    if (eAspectRatio <= BFMT_AspectRatio_eSAR)
        return (NEXUS_AspectRatio)eAspectRatio;
    else
        return NEXUS_AspectRatio_eUnknown;
}

BFMT_AspectRatio
NEXUS_P_AspectRatio_ToMagnum(NEXUS_AspectRatio aspectRatio)
{
    if (aspectRatio <= NEXUS_AspectRatio_eSar)
        return (BFMT_AspectRatio)aspectRatio;
    else
        return BFMT_AspectRatio_eUnknown;
}


NEXUS_VideoFrameRate
NEXUS_P_FrameRate_FromMagnum(BAVC_FrameRateCode magnumFramerate)
{
    BDBG_CASSERT(NEXUS_VideoFrameRate_eUnknown == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_eUnknown);
    BDBG_CASSERT(NEXUS_VideoFrameRate_e23_976 == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_e23_976);
    BDBG_CASSERT(NEXUS_VideoFrameRate_e24 == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_e24);
    BDBG_CASSERT(NEXUS_VideoFrameRate_e25 == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_e25);
    BDBG_CASSERT(NEXUS_VideoFrameRate_e29_97 == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_e29_97);
    BDBG_CASSERT(NEXUS_VideoFrameRate_e30 == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_e30);
    BDBG_CASSERT(NEXUS_VideoFrameRate_e50 == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_e50);
    BDBG_CASSERT(NEXUS_VideoFrameRate_e59_94 == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_e59_94);
    BDBG_CASSERT(NEXUS_VideoFrameRate_e60 == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_e60);
    BDBG_CASSERT(NEXUS_VideoFrameRate_e14_985 == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_e14_985);
    BDBG_CASSERT(NEXUS_VideoFrameRate_e7_493 == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_e7_493);
    BDBG_CASSERT(NEXUS_VideoFrameRate_e10 == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_e10);
    BDBG_CASSERT(NEXUS_VideoFrameRate_e15 == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_e15);
    BDBG_CASSERT(NEXUS_VideoFrameRate_e20 == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_e20);
    BDBG_CWARNING(NEXUS_VideoFrameRate_eMax == (NEXUS_VideoFrameRate)BAVC_FrameRateCode_eMax);

    return (NEXUS_VideoFrameRate)magnumFramerate;
}

NEXUS_Error NEXUS_P_FrameRate_ToMagnum(NEXUS_VideoFrameRate frameRate, BAVC_FrameRateCode *pMagnumFramerate)
{
    if((unsigned)frameRate < NEXUS_VideoFrameRate_eMax) {
        *pMagnumFramerate = (BAVC_FrameRateCode)frameRate;
        return NEXUS_SUCCESS;
    } else {
        *pMagnumFramerate = BAVC_FrameRateCode_eUnknown;
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }
}

static const struct {
    unsigned frequency;
    NEXUS_VideoFrameRate nexusFramerate;
} b_verticalfrequency[NEXUS_VideoFrameRate_eMax] = {
/* array should be sorted by the fraquency to faclilitate implementations of NEXUS_P_RefreshRate_FromFrameRate and NEXUS_P_LookupFrameRate */
    { 7493, NEXUS_VideoFrameRate_e7_493},
    {10000, NEXUS_VideoFrameRate_e10},
    {14985, NEXUS_VideoFrameRate_e14_985},
    {15000, NEXUS_VideoFrameRate_e15},
    {20000, NEXUS_VideoFrameRate_e20},
    {23976, NEXUS_VideoFrameRate_e23_976},
    {24000, NEXUS_VideoFrameRate_e24},
    {25000, NEXUS_VideoFrameRate_e25},
    {29970, NEXUS_VideoFrameRate_e29_97},
    {30000, NEXUS_VideoFrameRate_e30},
    {50000, NEXUS_VideoFrameRate_e50},
    {59940, NEXUS_VideoFrameRate_e59_94},
    {60000, NEXUS_VideoFrameRate_e60}
};

unsigned NEXUS_P_RefreshRate_FromFrameRate(NEXUS_VideoFrameRate frameRate)
{
    unsigned i;
    for(i=0;i<sizeof(b_verticalfrequency)/sizeof(*b_verticalfrequency);i++) {
        if (frameRate == b_verticalfrequency[i].nexusFramerate) {
            return b_verticalfrequency[i].frequency;
        }
    }
    BERR_TRACE(NEXUS_NOT_SUPPORTED);
    return 0; /* NEXUS_VideoFrameRate_eUnknown */
}

void
NEXUS_P_LookupFrameRate(unsigned frameRateInteger, NEXUS_VideoFrameRate *pNexusFrameRate)
{
    unsigned i;
    /* 1. use precise match */
    for(i=0;i<sizeof(b_verticalfrequency)/sizeof(*b_verticalfrequency);i++) {
        if (frameRateInteger == b_verticalfrequency[i].frequency) {
            *pNexusFrameRate = b_verticalfrequency[i].nexusFramerate;
            return;
        }
    }
    /* 2. use 1% fudge factor */
    for(i=0;i<sizeof(b_verticalfrequency)/sizeof(*b_verticalfrequency);i++) {
        if ((frameRateInteger*258)/256>= b_verticalfrequency[i].frequency && (frameRateInteger*254)/256<=b_verticalfrequency[i].frequency) {
            *pNexusFrameRate = b_verticalfrequency[i].nexusFramerate;
            return;
        }
    }
    /* 3. use 10% fudge factor */
    for(i=0;i<sizeof(b_verticalfrequency)/sizeof(*b_verticalfrequency);i++) {
        if ((frameRateInteger*276)/256>= b_verticalfrequency[i].frequency && (frameRateInteger*236)/256<=b_verticalfrequency[i].frequency) {
            *pNexusFrameRate = b_verticalfrequency[i].nexusFramerate;
            return;
        }
    }
    *pNexusFrameRate = NEXUS_VideoFrameRate_eUnknown;
    return;
}

static const NEXUS_PixelFormatInfo g_pixelFormatInfo[NEXUS_PixelFormat_eMax] = {
    {false, false, false, 0, BPXL_INVALID, NEXUS_PixelFormat_eUnknown},
    {true, false, false, 16, BPXL_eR5_G6_B5, NEXUS_PixelFormat_eR5_G6_B5},
    {true, false, false, 16, BPXL_eB5_G6_R5, NEXUS_PixelFormat_eB5_G6_R5},

    {true, false, true,  16, BPXL_eA1_R5_G5_B5, NEXUS_PixelFormat_eA1_R5_G5_B5},
    {true, false, false, 16, BPXL_eX1_R5_G5_B5, NEXUS_PixelFormat_eX1_R5_G5_B5},
    {true, false, true,  16, BPXL_eA1_B5_G5_R5, NEXUS_PixelFormat_eA1_B5_G5_R5},
    {true, false, false, 16, BPXL_eX1_B5_G5_R5, NEXUS_PixelFormat_eX1_B5_G5_R5},
    {true, false, true,  16, BPXL_eR5_G5_B5_A1, NEXUS_PixelFormat_eR5_G5_B5_A1},
    {true, false, false, 16, BPXL_eR5_G5_B5_X1, NEXUS_PixelFormat_eR5_G5_B5_X1},
    {true, false, true,  16, BPXL_eB5_G5_R5_A1, NEXUS_PixelFormat_eB5_G5_R5_A1},
    {true, false, false, 16, BPXL_eB5_G5_R5_X1, NEXUS_PixelFormat_eB5_G5_R5_X1},

    {true, false, true,  16, BPXL_eA4_R4_G4_B4, NEXUS_PixelFormat_eA4_R4_G4_B4},
    {true, false, false, 16, BPXL_eX4_R4_G4_B4, NEXUS_PixelFormat_eX4_R4_G4_B4},
    {true, false, true,  16, BPXL_eA4_B4_G4_R4, NEXUS_PixelFormat_eA4_B4_G4_R4},
    {true, false, false, 16, BPXL_eX4_B4_G4_R4, NEXUS_PixelFormat_eX4_B4_G4_R4},
    {true, false, true,  16, BPXL_eR4_G4_B4_A4, NEXUS_PixelFormat_eR4_G4_B4_A4},
    {true, false, false, 16, BPXL_eR4_G4_B4_X4, NEXUS_PixelFormat_eR4_G4_B4_X4},
    {true, false, true,  16, BPXL_eB4_G4_R4_A4, NEXUS_PixelFormat_eB4_G4_R4_A4},
    {true, false, false, 16, BPXL_eB4_G4_R4_X4, NEXUS_PixelFormat_eB4_G4_R4_X4},

    {true, false, true,  32, BPXL_eA8_R8_G8_B8, NEXUS_PixelFormat_eA8_R8_G8_B8},
    {true, false, false, 32, BPXL_eX8_R8_G8_B8, NEXUS_PixelFormat_eX8_R8_G8_B8},
    {true, false, true,  32, BPXL_eA8_B8_G8_R8, NEXUS_PixelFormat_eA8_B8_G8_R8},
    {true, false, false, 32, BPXL_eX8_B8_G8_R8, NEXUS_PixelFormat_eX8_B8_G8_R8},
    {true, false, true,  32, BPXL_eR8_G8_B8_A8, NEXUS_PixelFormat_eR8_G8_B8_A8},
    {true, false, false, 32, BPXL_eR8_G8_B8_X8, NEXUS_PixelFormat_eR8_G8_B8_X8},
    {true, false, true,  32, BPXL_eB8_G8_R8_A8, NEXUS_PixelFormat_eB8_G8_R8_A8},
    {true, false, false, 32, BPXL_eB8_G8_R8_X8, NEXUS_PixelFormat_eB8_G8_R8_X8},

    {true, false, true, 8, BPXL_eA8, NEXUS_PixelFormat_eA8},
    {true, false, true, 4, BPXL_eA4, NEXUS_PixelFormat_eA4},
    {true, false, true, 2, BPXL_eA2, NEXUS_PixelFormat_eA2},
    {true, false, true, 1, BPXL_eA1, NEXUS_PixelFormat_eA1},

    {true, false, true, 1, BPXL_eW1, NEXUS_PixelFormat_eW1},

    {true, true, true, 16, BPXL_eA8_P8, NEXUS_PixelFormat_eA8_Palette8},
    {true, true, true, 8,  BPXL_eP8, NEXUS_PixelFormat_ePalette8},
    {true, true, true, 4,  BPXL_eP4, NEXUS_PixelFormat_ePalette4},
    {true, true, true, 2,  BPXL_eP2, NEXUS_PixelFormat_ePalette2},
    {true, true, true, 1,  BPXL_eP1, NEXUS_PixelFormat_ePalette1},

    {true,  true, true,   16, BPXL_eY8_P8, NEXUS_PixelFormat_eY8_Palette8},
    {false, false, false, 8,  BPXL_eY8,    NEXUS_PixelFormat_eY8},
    {false, false, true,  8,  BPXL_eA8_Y8, NEXUS_PixelFormat_eA8_Y8},

    {false, false, false, 16, BPXL_eCb8_Cr8, NEXUS_PixelFormat_eCb8_Cr8},
    {false, false, false, 16, BPXL_eCr8_Cb8, NEXUS_PixelFormat_eCr8_Cb8},
    {false, false, false, 16, BPXL_eCb8, NEXUS_PixelFormat_eCb8},
    {false, false, false, 16, BPXL_eCr8, NEXUS_PixelFormat_eCr8},

    {false, false, false, 32, BPXL_eY08_Cb8_Y18_Cr8, NEXUS_PixelFormat_eY08_Cb8_Y18_Cr8},
    {false, false, false, 32, BPXL_eY08_Cr8_Y18_Cb8, NEXUS_PixelFormat_eY08_Cr8_Y18_Cb8},
    {false, false, false, 32, BPXL_eY18_Cb8_Y08_Cr8, NEXUS_PixelFormat_eY18_Cb8_Y08_Cr8},
    {false, false, false, 32, BPXL_eY18_Cr8_Y08_Cb8, NEXUS_PixelFormat_eY18_Cr8_Y08_Cb8},
    {false, false, false, 32, BPXL_eCb8_Y08_Cr8_Y18, NEXUS_PixelFormat_eCb8_Y08_Cr8_Y18},
    {false, false, false, 32, BPXL_eCb8_Y18_Cr8_Y08, NEXUS_PixelFormat_eCb8_Y18_Cr8_Y08},
    {false, false, false, 32, BPXL_eCr8_Y18_Cb8_Y08, NEXUS_PixelFormat_eCr8_Y18_Cb8_Y08},
    {false, false, false, 32, BPXL_eCr8_Y08_Cb8_Y18, NEXUS_PixelFormat_eCr8_Y08_Cb8_Y18},

    {false, false, false, 32, BPXL_eX2_Cr10_Y10_Cb10, NEXUS_PixelFormat_eX2_Cr10_Y10_Cb10},

    {false, false, true,  32, BPXL_eA8_Y8_Cb8_Cr8, NEXUS_PixelFormat_eA8_Y8_Cb8_Cr8},
    {false, false, true,  32, BPXL_eA8_Cr8_Cb8_Y8, NEXUS_PixelFormat_eA8_Cr8_Cb8_Y8},
    {false, false, true,  32, BPXL_eCr8_Cb8_Y8_A8, NEXUS_PixelFormat_eCr8_Cb8_Y8_A8},
    {false, false, true,  32, BPXL_eY8_Cb8_Cr8_A8, NEXUS_PixelFormat_eY8_Cb8_Cr8_A8},

    {false, false, false, 32, BPXL_eY010_Cb10_Y110_Cr10, NEXUS_PixelFormat_eY010_Cb10_Y110_Cr10},
    {false, false, false, 32, BPXL_eY010_Cr10_Y110_Cb10, NEXUS_PixelFormat_eY010_Cr10_Y110_Cb10},
    {false, false, false, 32, BPXL_eY110_Cb10_Y010_Cr10, NEXUS_PixelFormat_eY110_Cb10_Y010_Cr10},
    {false, false, false, 32, BPXL_eY110_Cr10_Y010_Cb10, NEXUS_PixelFormat_eY110_Cr10_Y010_Cb10},
    {false, false, false, 32, BPXL_eCb10_Y010_Cr10_Y110, NEXUS_PixelFormat_eCb10_Y010_Cr10_Y110},
    {false, false, false, 32, BPXL_eCb10_Y110_Cr10_Y010, NEXUS_PixelFormat_eCb10_Y110_Cr10_Y010},
    {false, false, false, 32, BPXL_eCr10_Y110_Cb10_Y010, NEXUS_PixelFormat_eCr10_Y110_Cb10_Y010},
    {false, false, false, 32, BPXL_eCr10_Y010_Cb10_Y110, NEXUS_PixelFormat_eCr10_Y010_Cb10_Y110},

    {false, false, false, 8,  BPXL_eL8, NEXUS_PixelFormat_eL8},
    {false, false, true,  8,  BPXL_eL4_A4, NEXUS_PixelFormat_eL4_A4},
    {false, false, true,  16, BPXL_eL8_A8, NEXUS_PixelFormat_eL8_A8},
    {false, false, true,  8,  BPXL_eL15_L05_A6, NEXUS_PixelFormat_eL15_L05_A6},

    {true, false, false, 24, BPXL_eR8_G8_B8, NEXUS_PixelFormat_eR8_G8_B8},
    {false, false, false, 32, BPXL_eX2_Y010_Cb10_Y110_X2_Cr10_Y010_Cb10_X2_Y110_Cr10_Y010_X2_Cb10_Y110_Cr10, NEXUS_PixelFormat_eYCbCr422_10bit}
};


NEXUS_PixelFormat
NEXUS_P_PixelFormat_FromMagnum(BPXL_Format magnumPixelFormat)
{
    unsigned i;
    for(i=0;i<NEXUS_PixelFormat_eMax;i++) {
#if 0
        /* enable this if things get off */
        if (g_pixelFormatInfo[i].nexus != i) {BDBG_ERR(("NEXUS_PixelFormat %d has a wrong lookup in g_pixelFormatInfo[].", i));}
#endif
        if(g_pixelFormatInfo[i].magnum == magnumPixelFormat) {
            BDBG_ASSERT(g_pixelFormatInfo[i].nexus == i);
            return i;
        }
    }
    return NEXUS_PixelFormat_eUnknown;
}

NEXUS_Error
NEXUS_P_PixelFormat_ToMagnum(NEXUS_PixelFormat nexusPixelFormat, BPXL_Format *magnumPixelFormat)
{
    if (nexusPixelFormat < NEXUS_PixelFormat_eMax) {
        BDBG_ASSERT(g_pixelFormatInfo[nexusPixelFormat].nexus == nexusPixelFormat);
        *magnumPixelFormat = g_pixelFormatInfo[nexusPixelFormat].magnum;
        return NEXUS_SUCCESS;
    }
    return BERR_INVALID_PARAMETER;
}

const NEXUS_PixelFormatInfo *NEXUS_PixelFormat_GetInfo(NEXUS_PixelFormat pixelFormat)
{
    if (pixelFormat < NEXUS_PixelFormat_eMax) {
        return &g_pixelFormatInfo[pixelFormat];
    }
    BDBG_ERR(("Invalid NEXUS_PixelFormat %d", pixelFormat));
    return NULL;
}

NEXUS_ColorSpace NEXUS_P_ColorSpace_FromMagnum(BAVC_Colorspace colorSpace)
{
    BDBG_CASSERT(NEXUS_ColorSpace_eMax == (NEXUS_ColorSpace)BAVC_Colorspace_eFuture);
    BDBG_CASSERT(NEXUS_ColorSpace_eRgb == (NEXUS_ColorSpace)BAVC_Colorspace_eRGB);
    BDBG_CASSERT(NEXUS_ColorSpace_eYCbCr444 == (NEXUS_ColorSpace)BAVC_Colorspace_eYCbCr444);
    BDBG_CASSERT(NEXUS_ColorSpace_eYCbCr422 == (NEXUS_ColorSpace)BAVC_Colorspace_eYCbCr422);
    return (NEXUS_ColorSpace)colorSpace;
}

BAVC_Colorspace NEXUS_P_ColorSpace_ToMagnum(NEXUS_ColorSpace colorSpace)
{
    return (BAVC_Colorspace)colorSpace;
}

NEXUS_HdmiColorDepth NEXUS_P_HdmiColorDepth_FromMagnum(BAVC_HDMI_BitsPerPixel eBitsPerPixel)
{
    BDBG_CASSERT(NEXUS_HdmiColorDepth_e8bit == (NEXUS_HdmiColorDepth)BAVC_HDMI_BitsPerPixel_e24bit);
    BDBG_CASSERT(NEXUS_HdmiColorDepth_e10bit == (NEXUS_HdmiColorDepth)BAVC_HDMI_BitsPerPixel_e30bit);
    BDBG_CASSERT(NEXUS_HdmiColorDepth_e12bit == (NEXUS_HdmiColorDepth)BAVC_HDMI_BitsPerPixel_e36bit);
    BDBG_CASSERT(NEXUS_HdmiColorDepth_e16bit == (NEXUS_HdmiColorDepth)BAVC_HDMI_BitsPerPixel_e48bit);
    return (NEXUS_HdmiColorDepth)eBitsPerPixel;
}

BAVC_HDMI_BitsPerPixel NEXUS_P_HdmiColorDepth_ToMagnum(NEXUS_HdmiColorDepth colorDepth)
{
    return (BAVC_HDMI_BitsPerPixel)colorDepth;
}

bool
NEXUS_P_VideoFormat_IsSd(NEXUS_VideoFormat format)
{
    NEXUS_VideoFormatInfo info;
    NEXUS_VideoFormat_GetInfo(format, &info);
    return info.digitalHeight <= 576 && info.interlaced;
}

bool
NEXUS_P_VideoFormat_IsInterlaced(NEXUS_VideoFormat format)
{
    NEXUS_VideoFormatInfo info;
    NEXUS_VideoFormat_GetInfo(format, &info);
    return info.interlaced;
}

/* This function does not promise an exact match. Instead, it returns a NEXUS_VideoFormat which most closely matches the given info.
This allows applications to make general configuration decisions.
Always make sure there's no 50/60 Hz or interlaced/progressive mixup. */
NEXUS_VideoFormat NEXUS_P_VideoFormat_FromInfo(unsigned height, unsigned frameRate, bool interlaced)
{
    bool is50 = (frameRate == NEXUS_VideoFrameRate_e50) || (frameRate == NEXUS_VideoFrameRate_e25);

    if (height > 720) {
        if (interlaced) {
            return is50 ? NEXUS_VideoFormat_e1080i50hz : NEXUS_VideoFormat_e1080i;
        }
        else {
            switch (frameRate) {
            case NEXUS_VideoFrameRate_e23_976:
            case NEXUS_VideoFrameRate_e24:
                return NEXUS_VideoFormat_e1080p24hz;
            case NEXUS_VideoFrameRate_e25: return NEXUS_VideoFormat_e1080p25hz;
            case NEXUS_VideoFrameRate_e29_97: return NEXUS_VideoFormat_e1080p30hz;
            case NEXUS_VideoFrameRate_e30: return NEXUS_VideoFormat_e1080p30hz;
            case NEXUS_VideoFrameRate_e50: return NEXUS_VideoFormat_e1080p50hz;
            default: return NEXUS_VideoFormat_e1080p; /* 60hz */
            }
        }
    }

    /* NOTE: the analog height of 480p is 483 */
    if (((is50 && height > 576) || (!is50 && height > 483)) && !interlaced) {
        switch (frameRate) {
        case NEXUS_VideoFrameRate_e23_976:
        case NEXUS_VideoFrameRate_e24:
            return NEXUS_VideoFormat_e720p24hz;
        case NEXUS_VideoFrameRate_e25:
        case NEXUS_VideoFrameRate_e50:
            return NEXUS_VideoFormat_e720p50hz;
        default:
            return NEXUS_VideoFormat_e720p; /* 60hz */
        }
    }

    if (is50) {
        if (height == 288 && !interlaced) {
            /* handle one specific type */
            return NEXUS_VideoFormat_e288p50hz;
        }

        /* the catch all for 50Hz is 576i/576p */
       return interlaced ? NEXUS_VideoFormat_ePal : NEXUS_VideoFormat_e576p;
    }
    else {
        if (height == 240 && !interlaced) {
            /* handle one specific type */
            return NEXUS_VideoFormat_e240p60hz;
        }

        /* the catch all, if not 50Hz, is 480i/480p */
        return interlaced ? NEXUS_VideoFormat_eNtsc : NEXUS_VideoFormat_e480p;
    }
}

NEXUS_VideoOrientation
NEXUS_P_VideoOrientation_FromMagnum(BFMT_Orientation orientation)
{
    BDBG_CASSERT(NEXUS_VideoOrientation_e2D==(NEXUS_VideoOrientation)BFMT_Orientation_e2D);
    BDBG_CASSERT(NEXUS_VideoOrientation_e3D_OverUnder==(NEXUS_VideoOrientation)BFMT_Orientation_e3D_OverUnder);
    BDBG_CASSERT(NEXUS_VideoOrientation_e3D_LeftRight==(NEXUS_VideoOrientation)BFMT_Orientation_e3D_LeftRight);
#if 0
    BDBG_CASSERT(NEXUS_VideoOrientation_e3D_Left==(NEXUS_VideoOrientation)BFMT_Orientation_e3D_Left);
    BDBG_CASSERT(NEXUS_VideoOrientation_e3D_Right==(NEXUS_VideoOrientation)BFMT_Orientation_e3D_Right);
#endif
    return (NEXUS_VideoOrientation)orientation;
}

BFMT_Orientation
NEXUS_P_VideoOrientation_ToMagnum(NEXUS_VideoOrientation orientation)
{
    return (BFMT_Orientation)orientation;
}


