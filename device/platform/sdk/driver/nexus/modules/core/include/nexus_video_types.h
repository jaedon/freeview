/***************************************************************************
*     (c)2004-2012 Broadcom Corporation
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
* $brcm_Workfile: nexus_video_types.h $
* $brcm_Revision: 57 $
* $brcm_Date: 10/11/12 2:58p $
*
* API Description:
*   API name: Base
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/include/nexus_video_types.h $
* 
* 57   10/11/12 2:58p vsilyaev
* SW7435-329: Fixed interface for C++ applications
* 
* 56   10/11/12 12:25p vsilyaev
* SW7435-329: Converted NEXUS_VideoInput to NEXUS_VideoInputHandle and
*  added object verification
* 
* 55   8/16/12 2:49p erickson
* SW7435-276: add 482/483 NEXUS_VideoFormat enums
* 
* 54   8/2/12 4:48p erickson
* SW7435-276: clarify that NEXUS_VideoFormat_eNtsc is also used for 480i
* 
* 53   5/24/12 1:12p vsilyaev
* SW7425-3103: Added 15 and 20 FPS frame rates
* 
* 52   5/7/12 2:09p jtna
* SW7429-143: added NEXUS_VideoFrameRate_e10
* 
* 51   2/14/12 12:46p vsilyaev
* SW7425-2258: Added Motion Jpeg codec type
* 
* 50   8/15/11 3:11p vsilyaev
* SW7425-938: Added mapping for SH263 and VP8 video codecs
* 
* 49   6/30/11 5:15p erickson
* SW7346-299: fix NEXUS_VideoFrameRate_e7_493 typo
* 
* 48   4/22/11 3:24p vsilyaev
* SW7425-416: Removed 3DLeft/3DRight
* 
* 47   3/7/11 5:06p jhaberf
* SWDTV-5812: Used defines for legacy LG modes to avoid changing
*  enumeration numbering
* 
* 46   3/7/11 4:49p jhaberf
* SWDTV-5812: Added 720p 24/30HZ
* 
* 45   3/4/11 11:39a jhaberf
* SWDTV-5519: Updated nexus_video_types.h with 3D formats
* 
* 44   3/3/11 12:00p erickson
* SW7422-221: clarify that 3D NEXUS_VideoFormat enums are full-res
*
* 43   1/19/11 3:48p vsilyaev
* SW7422-216: Added VP6 video codec
*
* 42   11/29/10 11:33a vsilyaev
* SW7422-65: Fixed mapping of SVC/MVC video codecs
*
* 41   11/12/10 11:12a vsilyaev
* SW7422-64: Merged 3D types to follow BFMT
*
* 40   10/26/10 1:09p vsilyaev
* SW7422-64: Reordered  VideoOrientation enum
*
* 39   10/25/10 7:02p vsilyaev
* SW7422-64: Added 3D video format and orientation
*
* 38   9/28/10 6:44p vsilyaev
* SW7422-65: Added SVC types
*
* 37   9/21/10 6:25p jhaberf
* SW35230-765: Add new PC formats
*
* 36   9/20/10 6:37p jhaberf
* SW35230-1185: Added 3D types for 35230
*
* 35   9/8/10 12:05p vsilyaev
* SW7468-129: Added video decoder on ZSP
*
* 34   8/19/10 2:03p jhaberf
* SW35230-1059: Added HDMI 3D support to nexus header files
*
* 33   8/17/10 4:22p erickson
* SW7420-928: use NEXUS_VideoBufferType for striped surface
*
* 32   6/23/10 12:09p erickson
* SW7550-453: move NEXUS_StripedSurfaceHandle to nexus_striped_surface.h
*
* 31   6/23/10 9:53a erickson
* SWDEPRECATED-2425: improve comments for NEXUS_VideoFormat_eCustom0, 1
*  and 2
*
* SW7468-129/1   8/5/10 4:17p vsilyaev
* SW7468-129: Added RMFF and RV40 support
*
* 30   2/24/10 2:34p erickson
* SW3548-2764: added NEXUS_VideoFormat_e1440x576p50hz
*
* BCM3D/3   1/29/10 7:09p hongtaoz
* SW3548-2680, SW3548-2681: removed 1470p custom formats;
*
* BCM3D/2   1/20/10 10:39a erickson
* SW3548-2727: add more 3D TV types
*
* BCM3D/1   1/19/10 2:30p erickson
* SW3548-2727: add 3DTV formats
*
* 28   12/17/09 4:19p erickson
* SW3556-933: added video format
*
* 27   11/2/09 11:20a erickson
* SWDEPRECATED-2425: expand comments
*
* 26   10/7/09 10:30a erickson
* SWDEPRECATED-2425: fixed comment for NEXUS_VideoCodec_eNone
*
* 25   9/15/09 9:28a erickson
* SW7405-2991: clarify NEXUS_AspectRatio_eSar and eSquarePixel
*
* 24   8/6/09 2:22p erickson
* PR57387: add 720p 25/30 to NEXUS_VideoFormat list
*
* 23   7/30/09 5:39p erickson
* PR57220: added new NEXUS_VideoFrameRate values
*
* 22   5/4/09 3:40p erickson
* PR53373: merge HDMI 1.3a support
*
* PR53373_DeepColor/1   4/16/09 3:57p vle
* PR53373: Add HDMI 1.3a support, deep color, to Brutus/nexus
*
* 21   3/19/09 11:37a erickson
* PR53234: add two DVI VESA formats
*
* 20   11/24/08 12:09p erickson
* PR35457: update NEXUS_AspectRatio_eSar comments
*
* 19   11/20/08 5:00p erickson
* PR35457: updated comment
*
* 18   11/19/08 10:06a erickson
* PR48764: merge in macrovision status
*
* PR48764/1   11/10/08 4:39p chengli
* PR48764 : add NEXUS_MacrovisionType
*
* 17   8/11/08 11:40a erickson
* PR45219: merge
*
* 16   8/8/08 6:59p jgarrett
* PR 45171: Adding new PAL modes
*
* 15   6/12/08 2:05p erickson
* PR43599: NEXUS_VideoFormat_e1080p is just an alias for
*  NEXUS_VideoFormat_e1080p60hz
*
* 14   5/21/08 10:33a erickson
* PR42678: rename NEXUS_DvoOutput to NEXUS_PanelOutput
*
* 13   5/2/08 1:39p erickson
* PR41314: colocate the alias enum
*
* 12   5/2/08 10:59a erickson
* PR41314: fix naming convention on ColorSpace
*
* 11   5/2/08 10:53a erickson
* PR41314: add SECAM variants
*
* 10   4/16/08 12:45p erickson
* PR39009: merge
*
* PR39009/1   4/17/08 12:40a honglang
* PR39009: add two format conversion: NTSC_443, PAL_60
*
* 9   3/25/08 12:36p erickson
* PR40744: standardized capitalization on enums
*
* 8   3/21/08 9:45a erickson
* PR40744: merge
*
* PR40744/1   3/19/08 4:17p dyzhang
* PR40744: NEXUS_VideoInput_GetStatus returns unknown format for all VESA
*  formats
*
* 7   3/12/08 2:30p katrep
* PR40514:Add support for AVS codecs
*
* 6   3/10/08 4:36p erickson
* PR40348: update comment on NEXUS_VideoFormat_eCustom0
*
* 5   2/29/08 5:39p erickson
* PR40156: implement ColorMatrix function
*
* 4   2/26/08 11:41p erickson
* PR38888: add types for NEXUS_VideoDecoderStreamInformation
*
* 3   2/25/08 2:58p erickson
* PR39786: added NEXUS_StripedSurfaceHandle
*
* 2   1/28/08 9:41a erickson
* PR35457: comment update
*
* 1   1/18/08 2:12p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/10   1/9/08 12:28p erickson
* PR36814: added NEXUS_ColorSpace
*
* Nexus_Devel/PR36159/3   1/9/08 8:53p honglang
* PR36814: Add color space type.
*
* Nexus_Devel/9   11/28/07 10:37a erickson
* PR35457: doc update
*
* Nexus_Devel/8   11/26/07 11:27a vsilyaev
* PR36159: Merged 3563 code
*
* Nexus_Devel/PR36159/2   11/26/07 9:48a gezhang
* PR36159: Bring up Nexus on 93563 platform
*
* Nexus_Devel/7   11/2/07 4:42p vsilyaev
* PR 36696: Used connector model for VideoInput's and VideoOutput's
*
* Nexus_Devel/6   11/1/07 9:04a erickson
* PR36633: rework enums to naming convention
*
* Nexus_Devel/5   10/4/07 3:34p vsilyaev
* PR 34662: Removed VideoInput descriptor
*
* Nexus_Devel/4   10/4/07 1:09p vsilyaev
* PR 34662: Removed VideoOutputDesc
*
* Nexus_Devel/3   9/28/07 10:04a erickson
* PR34925: added NEXUS_AspectRatio, remove quasi-PSI numbers from
* VideoCodec, added eMax throughout
*
* Nexus_Devel/2   9/21/07 4:06p vsilyaev
* PR 34662: Added 'Unknown' video format
*
* Nexus_Devel/1   9/12/07 1:55p vsilyaev
* PR 34419: Base audio and video types
*
*
***************************************************************************/
#ifndef NEXUS_VIDEO_TYPES_H
#define NEXUS_VIDEO_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

/*
Summary:
Rate measured in frames per second.

Description:
23_976 means 23.976 frames per second.

See Also:
NEXUS_VideoDecoderStatus
*/
typedef enum NEXUS_VideoFrameRate {
    NEXUS_VideoFrameRate_eUnknown = 0,
    NEXUS_VideoFrameRate_e23_976,
    NEXUS_VideoFrameRate_e24,
    NEXUS_VideoFrameRate_e25,
    NEXUS_VideoFrameRate_e29_97,
    NEXUS_VideoFrameRate_e30,
    NEXUS_VideoFrameRate_e50,
    NEXUS_VideoFrameRate_e59_94,
    NEXUS_VideoFrameRate_e60,
    NEXUS_VideoFrameRate_e14_985,
    NEXUS_VideoFrameRate_e7_493,
    NEXUS_VideoFrameRate_e10,
    NEXUS_VideoFrameRate_e15,
    NEXUS_VideoFrameRate_e20,
    NEXUS_VideoFrameRate_eMax
} NEXUS_VideoFrameRate;

/*
Summary:
Video signal polarity

Description:
Used in NEXUS_AnalogVideoStatus
*/
typedef enum NEXUS_VideoPolarity {
    NEXUS_VideoPolarity_ePositive,
    NEXUS_VideoPolarity_eNegative
} NEXUS_VideoPolarity ;

/*
Summary:
Video buffer type

Description:
Used in NEXUS_StripedSurfaceCreateSettings
*/
typedef enum NEXUS_VideoBufferType
{
   NEXUS_VideoBufferType_eFrame = 1,
   NEXUS_VideoBufferType_eFieldPair,
   NEXUS_VideoBufferType_eTopField,
   NEXUS_VideoBufferType_eBotField,
   NEXUS_VideoBufferType_eMax
} NEXUS_VideoBufferType;

/*
Summary:
Codec used for video compression.

Description:
See Also:
NEXUS_VideoDecoderStartSettings
NEXUS_VideoDecoderModuleSettings
*/
typedef enum NEXUS_VideoCodec {
    NEXUS_VideoCodec_eUnknown = 0,     /* unknown/not supported video codec */
    NEXUS_VideoCodec_eNone = 0,        /* unknown/not supported video codec */
    NEXUS_VideoCodec_eMpeg1,           /* MPEG-1 Video (ISO/IEC 11172-2) */
    NEXUS_VideoCodec_eMpeg2,           /* MPEG-2 Video (ISO/IEC 13818-2) */
    NEXUS_VideoCodec_eMpeg4Part2,      /* MPEG-4 Part 2 Video */
    NEXUS_VideoCodec_eH263,            /* H.263 Video. The value of the enum is not based on PSI standards. */
    NEXUS_VideoCodec_eH264,            /* H.264 (ITU-T) or ISO/IEC 14496-10/MPEG-4 AVC */
    NEXUS_VideoCodec_eH264_Svc,        /* Scalable Video Codec extension of H.264 */
    NEXUS_VideoCodec_eH264_Mvc,        /* Multi View Coding extension of H.264 */
    NEXUS_VideoCodec_eVc1,             /* VC-1 Advanced Profile */
    NEXUS_VideoCodec_eVc1SimpleMain,   /* VC-1 Simple & Main Profile */
    NEXUS_VideoCodec_eDivx311,         /* DivX 3.11 coded video */
    NEXUS_VideoCodec_eAvs,             /* AVS coded video */
    NEXUS_VideoCodec_eRv40,            /* RV 4.0 coded video */
    NEXUS_VideoCodec_eVp6,             /* VP6 coded video */
    NEXUS_VideoCodec_eVp7,             /* VP7 coded video */
    NEXUS_VideoCodec_eVp8,             /* VP8 coded video */
    NEXUS_VideoCodec_eSpark,           /* H.263 Sorenson Spark coded video */
    NEXUS_VideoCodec_eMotionJpeg,      /* Motion Jpeg video codec */
    NEXUS_VideoCodec_eMax
} NEXUS_VideoCodec;

/*
Summary:
Video formats

Description:
See Also:
NEXUS_DisplaySettings
*/
typedef enum NEXUS_VideoFormat {
    NEXUS_VideoFormat_eUnknown = 0,     /* unknown/not supported video format */
    NEXUS_VideoFormat_eNtsc,            /* 480i, NTSC-M for North America */
    NEXUS_VideoFormat_eNtsc443,         /* NTSC encoding with the PAL color carrier frequency. */
    NEXUS_VideoFormat_eNtscJapan,       /* Japan NTSC, no pedestal level */
    NEXUS_VideoFormat_ePalM,            /* PAL Brazil */
    NEXUS_VideoFormat_ePalN,            /* PAL_N */
    NEXUS_VideoFormat_ePalNc,           /* PAL_N, Argentina */
    NEXUS_VideoFormat_ePalB,            /* Australia */
    NEXUS_VideoFormat_ePalB1,           /* Hungary */
    NEXUS_VideoFormat_ePalD,            /* China */
    NEXUS_VideoFormat_ePalD1,           /* Poland */
    NEXUS_VideoFormat_ePalDK1=NEXUS_VideoFormat_ePalD1,          /* Eastern Europe */
    NEXUS_VideoFormat_ePalDK2,          /* Eastern Europe */
    NEXUS_VideoFormat_ePalDK3,          /* Eastern Europe */
    NEXUS_VideoFormat_ePalG,            /* Europe. Same as NEXUS_VideoFormat_ePal. */
    NEXUS_VideoFormat_ePal = NEXUS_VideoFormat_ePalG,     /* PAL Europe */
    NEXUS_VideoFormat_ePalH,            /* Europe */
    NEXUS_VideoFormat_ePalK,            /* Europe */
    NEXUS_VideoFormat_ePalI,            /* U.K. */
    NEXUS_VideoFormat_ePal60hz,         /* 60Hz PAL */
    NEXUS_VideoFormat_eSecamL,          /* France */
    NEXUS_VideoFormat_eSecam = NEXUS_VideoFormat_eSecamL, /* Backward compatibility */
    NEXUS_VideoFormat_eSecamB,          /* Middle East */
    NEXUS_VideoFormat_eSecamG,          /* Middle East */
    NEXUS_VideoFormat_eSecamD,          /* Eastern Europe */
    NEXUS_VideoFormat_eSecamK,          /* Eastern Europe */
    NEXUS_VideoFormat_eSecamH,          /* Line SECAM */
    NEXUS_VideoFormat_e480p,            /* NTSC Progressive (27Mhz) */
    NEXUS_VideoFormat_e576p,            /* HD PAL Progressive 50hz for Australia */
    NEXUS_VideoFormat_e1080i,           /* HD 1080 Interlaced */
    NEXUS_VideoFormat_e1080i50hz,       /* European 50hz HD 1080 */
    NEXUS_VideoFormat_e1080p24hz,       /* HD 1080 Progressive, 24hz */
    NEXUS_VideoFormat_e1080p25hz,       /* HD 1080 Progressive, 25hz */
    NEXUS_VideoFormat_e1080p30hz,       /* HD 1080 Progressive, 30hz */
    NEXUS_VideoFormat_e1080p50hz,       /* HD 1080 Progressive, 50hz. */
    NEXUS_VideoFormat_e1080p60hz,       /* HD 1080 Progressive, 60hz */
    NEXUS_VideoFormat_e1080p = NEXUS_VideoFormat_e1080p60hz,
    NEXUS_VideoFormat_e1250i50hz,       /* HD 1250 Interlaced, 50hz */
    NEXUS_VideoFormat_e720p,            /* HD 720 Progressive */
    NEXUS_VideoFormat_e720p50hz,        /* HD 720p 50hz for Australia */
    NEXUS_VideoFormat_e720p24hz,        /* HD 720p 24hz */
    NEXUS_VideoFormat_e720p25hz,        /* HD 720p 25hz */
    NEXUS_VideoFormat_e720p30hz,        /* HD 720p 30hz */
    NEXUS_VideoFormat_e240p60hz,        /* NTSC 240p */
    NEXUS_VideoFormat_e288p50hz,        /* PAL 288p */
    NEXUS_VideoFormat_e1440x480p60hz,   /* CEA861B */
    NEXUS_VideoFormat_e1440x576p50hz,   /* CEA861B */
    NEXUS_VideoFormat_eCustomer1440x240p60hz,             /* 240p 60hz 7411 custom format. */
    NEXUS_VideoFormat_eCustomer1440x288p50hz,             /* 288p 50hz 7411 custom format. */
    NEXUS_VideoFormat_eCustomer1366x768p60hz,             /* Custom 1366x768 mode */
    NEXUS_VideoFormat_eCustomer1366x768p50hz,             /* Custom 1366x768 50hz mode */
    NEXUS_VideoFormat_eVesa640x480p60hz,                  /* DVI Safe mode for computer monitors */
    NEXUS_VideoFormat_eVesa800x600p60hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1024x768p60hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x768p60hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x768p60hzRed,              /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x720p50hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x720p60hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x720pReducedBlank,         /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa640x350p60hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa640x350p70hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa640x350p72hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa640x350p75hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa640x350p85hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa640x400p60hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa640x400p70hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa640x400p72hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa640x400p75hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa640x400p85hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa640x480p66hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa640x480p70hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa640x480p72hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa640x480p75hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa640x480p85hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa720x400p60hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa720x400p70hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa720x400p72hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa720x400p75hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa720x400p85hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa800x600p56hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa800x600p59hzRed,               /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa800x600p70hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa800x600p72hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa800x600p75hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa800x600p85hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa848x480p60hz,                  /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1024x768p66hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1024x768p70hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1024x768p72hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1024x768p75hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1024x768p85hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1064x600p60hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x720p70hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x720p72hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x720p75hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x720p85hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1024x768i87hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1152x864p75hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x768p75hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x768p85hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x800p_60Hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x960p60hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x960p85hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x1024p60hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x1024p69hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x1024p75hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x1024p85hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa832x624p75hz,                  /* MAC-16 */
    NEXUS_VideoFormat_eVesa1360x768p60hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1366x768p60hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1400x1050p60hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1400x1050p60hzReducedBlank,    /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1400x1050p75hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1440x900p60hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1600x1200p60hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1920x1080p60hzReducedBlank,    /* DVI VESA mode for computer monitors */

    NEXUS_VideoFormat_e720p_3DOU_AS,                      /* HD 3D 720p */
    NEXUS_VideoFormat_e720p50hz_3DOU_AS,                  /* HD 3D 720p50 */
    NEXUS_VideoFormat_e720p30hz_3DOU_AS,                  /* HD 3D 720p30 */
    NEXUS_VideoFormat_e720p24hz_3DOU_AS,                  /* HD 3D 720p24 */
    NEXUS_VideoFormat_e1080p24hz_3DOU_AS,                 /* HD 1080p 24Hz, 2750 sample per line, SMPTE 274M-1998 */
    NEXUS_VideoFormat_e1080p30hz_3DOU_AS,                 /* HD 1080p 30Hz, 2200 sample per line, SMPTE 274M-1998 */

    NEXUS_VideoFormat_eVesa1680x1050p_60Hz,               /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x800p_60Hz_Red,            /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1600x1200p_75Hz,               /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1600x900p_60Hz_Red,            /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1680x1050p_60Hz_Red,           /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1920x1200p_60Hz,               /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1920x1200p_60Hz_Red,           /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1152x720p_60Hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1152x720p_75Hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1152x720p_85Hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1152x864p_60Hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1152x864p_85Hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1152x870p_75Hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1152x900p_66Hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1152x900p_76Hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1170x584p_50Hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x1024p_70Hz,               /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x1024p_72Hz,               /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x1024p_76Hz,               /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x768p_50Hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1280x960p_75Hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1600x1024p_60Hz,               /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1600x1024p_76Hz,               /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa1728x1080p_60Hz,               /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa800x600p_100Hz,                /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa800x600p_90Hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa848x480p_75Hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa848x480p_85Hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_eVesa852x480p_60Hz,                 /* DVI VESA mode for computer monitors */
    NEXUS_VideoFormat_e720x482_NTSC,                      /* 720x482i NTSC-M for North America */
    NEXUS_VideoFormat_e720x482_NTSC_J,                    /* 720x482i Japan */
    NEXUS_VideoFormat_e720x483p,                          /* 720x483p */

    /* dynamics: custom format */
    NEXUS_VideoFormat_eCustom0,         /* Customized video format 0. This enums allows the user to select custom display settings programmed in
                                           magnum/commonutils/fmt/bfmt_custom.c. This cannot be changed at run time.
                                           Typically, the default bfmt_custom.c sets eCustom0 to be a 60Hz format and eCustom1 to be a 50Hz format.
                                           Custom settings can be obtained upon request. */
    NEXUS_VideoFormat_eCustom1,         /* Customized video format 1. See NEXUS_VideoFormat_eCustom0 for details. */
    NEXUS_VideoFormat_eCustom2,         /* eCustom2 is defined at run time by calling NEXUS_Display_SetCustomFormatSettings.
                                           This runtime modification cannot be done for eCustom0 and eCustom1.
                                           Typically, apps set eCustom2 to be a 48Hz format, which also supports 24Hz content. */

    /* 3D source & display formats. These are all full-res O/U 3D formats. Half-res does not require a special 3D format. */
    NEXUS_VideoFormat_eCustom1920x2160i_48Hz, /* 3DTV panel format */

    /* Legacy 3D timing and display formats */
    NEXUS_VideoFormat_eCustom_3D_720p,        /* HD 3D 720p with custom timing */
    NEXUS_VideoFormat_eCustom_3D_720p_50hz,   /* HD 3D 720p50hz with custom timing */
    NEXUS_VideoFormat_eCustom_3D_720p_30hz,   /* HD 3D 720p30hz with custom timing */
    NEXUS_VideoFormat_eCustom_3D_720p_24hz,   /* HD 3D 720p24hz with custom timing */
    NEXUS_VideoFormat_eCustom_3D_1080p_24hz,  /* HD 3D 1080p24hz with custom timing */
    NEXUS_VideoFormat_eCustom_3D_1080p_30hz,  /* HD 3D 1080p30hz with custom timing */

    NEXUS_VideoFormat_eMax              /* Total number of video formats */
} NEXUS_VideoFormat;

#define NEXUS_VideoFormat_e3D_720p       NEXUS_VideoFormat_e720p_3DOU_AS       /* HD 3D 720p */
#define NEXUS_VideoFormat_e3D_720p_24Hz  NEXUS_VideoFormat_e720p24hz_3DOU_AS   /* HD 3D 720p24 */
#define NEXUS_VideoFormat_e3D_720p_30Hz  NEXUS_VideoFormat_e720p30hz_3DOU_AS   /* HD 3D 720p30 */
#define NEXUS_VideoFormat_e3D_720p_50Hz  NEXUS_VideoFormat_e720p50hz_3DOU_AS   /* HD 3D 720p50 */
#define NEXUS_VideoFormat_e3D_1080p_24Hz NEXUS_VideoFormat_e1080p24hz_3DOU_AS  /* HD 1080p 24Hz, 2750 sample per line, SMPTE 274M-1998 */
#define NEXUS_VideoFormat_e3D_1080p_30Hz NEXUS_VideoFormat_e1080p30hz_3DOU_AS  /* HD 1080p 30Hz, 2200 sample per line, SMPTE 274M-1998 */

/**
Summary:
Video aspect ratio

Description:
See Also:
NEXUS_PcInputStatus
NEXUS_VideoDecoderStatus
**/
typedef enum NEXUS_AspectRatio {
    NEXUS_AspectRatio_eUnknown   = 0, /* Unknown/Reserved */
    NEXUS_AspectRatio_eSquarePixel,   /* Square pixel. This is equivalent to NEXUS_AspectRatio_eSar 1:1. */
    NEXUS_AspectRatio_e4x3,           /* 4:3 */
    NEXUS_AspectRatio_e16x9,          /* 16:9 */
    NEXUS_AspectRatio_e221x1,         /* 2.21:1 */
    NEXUS_AspectRatio_e15x9,          /* 15:9 */
    NEXUS_AspectRatio_eSar,           /* Sample aspect ratio - aspect ratio of the source calculated as the ratio of two numbers reported by the decoder.
                                         See NEXUS_VideoDecoderStreamInformation.sampleAspectRatioX and sampleAspectRatioY for an example.
                                         This aspect ratio is applied to the picture's source size (i.e. coded size), not the picture's display size. */
    NEXUS_AspectRatio_eMax
} NEXUS_AspectRatio;


/**
Summary:
Video color space
**/
typedef enum NEXUS_ColorSpace {
    NEXUS_ColorSpace_eRgb = 0,
    NEXUS_ColorSpace_eYCbCr422,
    NEXUS_ColorSpace_eYCbCr444,
    NEXUS_ColorSpace_eMax
} NEXUS_ColorSpace;

/**
Summary:
HDMI Color depth (8, 10, 12, and 16bit color mode).
Only applicable for platforms with HDMI 1.3 features.
**/
typedef enum NEXUS_HdmiColorDepth {
    NEXUS_HdmiColorDepth_e8bit = 0,
    NEXUS_HdmiColorDepth_e10bit,
    NEXUS_HdmiColorDepth_e12bit,
    NEXUS_HdmiColorDepth_e16bit
} NEXUS_HdmiColorDepth;

/**
Summary:
NEXUS_VideoOutput is an abstract connector token for routing video from to a destination.

Description:
It is used to connect specific VideoOutputs to the Display. It's also used for generic video settings and status.

All VideoOutputs also have a specific input (e.g. ComponentVideo, HdmiVideo).
This can be optionally used to control specific settings and to retrieve specific status.

See Also:
NEXUS_ComponentOutput_GetConnector
NEXUS_CompositeOutput_GetConnector
NEXUS_SvideoOutput_GetConnector
NEXUS_HdmiOutput_GetConnector
NEXUS_RfOutput_GetConnector
**/
typedef struct NEXUS_VideoOutputObject *NEXUS_VideoOutput;

/**
Summary:
NEXUS_VideoInput is an abstract connector token for routing video from a source.

Description:
It is used to connect VideoSources to VideoWindows. It's also used for generic video settings and status.

All VideoInputs also have a specific input (e.g. ComponentVideo, HdmiVideo).
This can be optionally used to control specific settings and to retrieve specific status.

See Also:
NEXUS_VideoDecoder_GetConnector
NEXUS_HdmiInput_GetConnector
NEXUS_AnalogVideo_GetConnector
**/
#ifdef __cplusplus
/* in C++  typeded struct A *A; is invalid, e.g. there is no separate namespace for structures */
typedef struct NEXUS_VideoInputObject *NEXUS_VideoInput;
#else
typedef struct NEXUS_VideoInput *NEXUS_VideoInput;
#endif

/**
Summary:
NEXUS_VideoInputHandle is synonym for NEXUS_VideoInput
**/
typedef NEXUS_VideoInput NEXUS_VideoInputHandle;

#define NEXUS_COLOR_MATRIX_COEFF_COUNT           (15)

/***************************************************************************
Summary:
Coefficient matrix for color space convertor

Description:
This is a 3x5 matrix for programming color space convertors (CSC) in various places in the video or graphics pipeline.
The following explanation comes from BVDC_Window_SetColorMatrix:

    [Y_out ]   [M[0]  M[1]  M[2]  M[3] [M[4] ]   [Y_in ]
    [Cb_out]   [M[5]  M[6]  M[7]  M[8] [M[9] ]   [Cb_in]
    [Cr_out] = [M[10] M[11] M[12] M[13][M[14]] * [Cr_in]
    [A_out ]   [0     0     0     1     0    ]   [A_in ]
    [  1   ]   [0     0     0     0     1    ]   [  1  ]

    {Y_in, Cb_in, Cr_in, A_in}     = Input pixel before apply M matrix.
    {Y_out, Cb_Out, Cr_out, A_out} = Output pixel after apply M matrix.

    Note: The last two rows of the above matrix are used to facilitate
    computation only. It is not implemented physically in hardware.
    Columns 0 to 2 are multiplicative coefficients. Columns 4 and 5 are
    additive coefficients.  Alpha multiply (4th column) is only supported
    graphics.

"shift" is a user-specified bit shift of the coeffMatrix values. This allows the user to specify
fixed point numbers in whatever precision they want. The actual precision supported by the hardware
block is use can vary.
Example:
    shift of 0 -> use the whole coeffMatrix[] value as an integer
    shift of 1 -> use (coeffMatrix[]>>1) as integer and (coeffMatrix[]&0x1)/2 as fractional part.
    shift of 4 -> use (coeffMatrix[]>>4) as integer and (coeffMatrix[]&0xF)/16 as fractional part.

See Also:
NEXUS_VideoInput_SetColorMatrix
NEXUS_VideoWindow_SetColorMatrix
NEXUS_PanelOutput_SetColorMatrix
NEXUS_Display_SetGraphicsColorMatrix
****************************************************************************/
typedef struct NEXUS_ColorMatrix
{
    uint32_t shift;
    int32_t  coeffMatrix[NEXUS_COLOR_MATRIX_COEFF_COUNT];
} NEXUS_ColorMatrix;

/***************************************************************************
Summary:
    Used to specify the possible standard matrix coefficients types for
    YCbCr color space interpretation of the digital video data.

Description:
    This enum follows the MPEG-2 standard Video-spec except the RGB output
    enum(=3), which is intended for HDMI use only in case of RGB output to
    an HDMI Rx device to indicate the RGB color space interpretation of the
    digital video data.

See Also:
    NEXUS_ColorPrimaries, NEXUS_TransferCharacteristics.
****************************************************************************/
typedef enum NEXUS_MatrixCoefficients
{
    /* Recommendation ITU-R BT.709;
       (ATSC HD or PAL HD, same as SMPTE274 and EIA770.3);

       note: it may also be used to indicate HD YCbCr output to an HDMI Rx
       device. */
    NEXUS_MatrixCoefficients_eItu_R_BT_709 = 1,

    /* FCC;
       (NTSC SD 1953, not exactly the same as BT.601, but similar) */
    NEXUS_MatrixCoefficients_eFCC = 4,

    /* Recommendation ITU-R BT.470-2 System B, G;
       (PAL SD, same as BT.601) */
    NEXUS_MatrixCoefficients_eItu_R_BT_470_2_BG = 5,

    /* SMPTE 170M;
       (NTSC SD, same as BT.601; but color primaries are different
       from BT.470-2 System B, G.);

       note: it may also be used to indicate SD YCbCr output to an HDMI Rx
       device. */
    NEXUS_MatrixCoefficients_eSmpte_170M = 6,

    /* SMPTE 240M (1987);
       (ATSC HD; color primaries are the same as SMPTE 170M.) */
    NEXUS_MatrixCoefficients_eSmpte_240M = 7,

    /* HDMI 1.3;
       (xvYCC709) */
    NEXUS_MatrixCoefficients_eXvYCC_709 = 8,

    /* HDMI 1.3;
       (xvYCC601) */
    NEXUS_MatrixCoefficients_eXvYCC_601 = 9,

    /* Unspecified Video: Image characteristics are unknown;
       VDC would handle 'Unknown' case as, i.e.
       if the decoded picture is in HD format(size is larger than
       720x576), then take default HD color matrix; else take default SD color
       matrix.

       Note: it may also be used to indicate the display doesn't output HDMI
       video. */
    NEXUS_MatrixCoefficients_eUnknown = 2,

    /* 0, 3,  8 - 255 reserved */

    /***********************************************************
     * NOTE: the following enum values are for HDMI/DVI display use only;
     *           video decompressor should not pass these values to vdc via the
     *           NEXUS_MFD_Picture or NEXUS_VDC_HdDvi_Picture data structure.
     * TODO: what about hd-dvi input when hd-dvi input module behaves as
     *           a stand-alone DVI Rx device?
     */
    /* for HDMI use only, in case of 220 steps limited range(16-236) RGB
        output to an HDMI Rx device */
    NEXUS_MatrixCoefficients_eHdmi_RGB = 0,

    /* for DVI use only, in case of 8-bit full range(0-255) RGB output to a
        DVI PC monitor */
    NEXUS_MatrixCoefficients_eDvi_Full_Range_RGB = 3

} NEXUS_MatrixCoefficients;

/***************************************************************************
Summary:
    Used to specify the possible color primaries.

Description:
    This enum follows the MPEG-2 standard Video-spec.

See Also:
    NEXUS_MatrixCoefficients, NEXUS_TransferCharacteristics.
****************************************************************************/
typedef enum NEXUS_ColorPrimaries
{
    /* Recommendation ITU-R BT.709;
       (ATSC HD or PAL HD) */
    NEXUS_ColorPrimaries_eItu_R_BT_709 = 1,

    /* Recommendation ITU-R BT.470-2 System M;
       (NTSC SD 1953, not the same as PAL SD nor SMPTE170) */
    NEXUS_ColorPrimaries_eItu_R_BT_470_2_M = 4,

    /* Recommendation ITU-R BT.470-2 System B, G;
       (PAL SD, similar to SMPTE170) */
    NEXUS_ColorPrimaries_eItu_R_BT_470_2_BG = 5,

    /* SMPTE 170M; (NTSC SD) */
    NEXUS_ColorPrimaries_eSmpte_170M = 6,

    /* SMPTE 240M (1987);
       (ATSC HD; same as SMPTE170) */
    NEXUS_ColorPrimaries_eSmpte_240M = 7,

    /* Generic file
       AVC specification ??? */
    NEXUS_ColorPrimaries_eGenericFilm = 8,

    /* Unspecified Video: Image characteristics are unknown;
       VDC would handle 'Unknown' case as follows, i.e.
       if the decoded picture is in HD format(size is larger than
       720x576), then take default HD color matrix; else take default SD color
       matrix. */
    NEXUS_ColorPrimaries_eUnknown = 2

    /* 0 - forbidden */

    /* 3, 8 - 255 reserved */

} NEXUS_ColorPrimaries;

/***************************************************************************
Summary:
    Used to specify the possible transfer characteristics.

Description:
    This enum follows the MPEG-2 standard Video-spec.

See Also:
    NEXUS_MatrixCoefficients, NEXUS_ColorPrimaries.
****************************************************************************/
typedef enum NEXUS_TransferCharacteristics
{
    /* Recommendation ITU-R BT.709;
       (ATSC HD or PAL HD) */
    NEXUS_TransferCharacteristics_eItu_R_BT_709 = 1,

    /* FCC, or Recommendation ITU-R BT.470-2 System M;
       (NTSC SD 1953, assumed display gamma 2.2) */
    NEXUS_TransferCharacteristics_eItu_R_BT_470_2_M = 4,

    /* Recommendation ITU-R BT.470-2 System B, G;
       (PAL SD, assumed display gamma 2.8) */
    NEXUS_TransferCharacteristics_eItu_R_BT_470_2_BG = 5,

    /* SMPTE 170M; (NTSC SD) */
    NEXUS_TransferCharacteristics_eSmpte_170M = 6,

    /* SMPTE 240M (1987); (ATSC HD) */
    NEXUS_TransferCharacteristics_eSmpte_240M = 7,

    /* Linear Transfer Characteristics */
    NEXUS_TransferCharacteristics_eLinear = 8,

    /* Recommendation ITU-T H.262, H.264; (IEC 61966-2-4 gamma, xvYCC) */
    NEXUS_TransferCharacteristics_eIec_61966_2_4 = 11,

    /* Unspecified Video: Image characteristics are unknown;
       VDC would handle 'Unknown' case as follows, i.e.
       if the decoded picture is in HD format(size is larger than
       720x576), then take default HD color matrix; else take default
       SD color. */
    NEXUS_TransferCharacteristics_eUnknown = 2

    /* 0 - forbidden */

    /* 3, 9 - 255 reserved */

} NEXUS_TransferCharacteristics;

/**
Summary:
Macrovision type

Description:
Used in NEXUS_AnalogVideoDecoder_GetStatus
**/
typedef enum NEXUS_MacrovisionType {
    NEXUS_MacrovisionType_eNoProtection = 0,   /* No copy protection */
    NEXUS_MacrovisionType_eType1,
    NEXUS_MacrovisionType_eType2,
    NEXUS_MacrovisionType_eType3,
    NEXUS_MacrovisionType_eCustomized,         /* User will provide table */
    NEXUS_MacrovisionType_eType1Rgb,           /* MV goes on RGB as well as other formats. */
    NEXUS_MacrovisionType_eType2Rgb,           /* MV goes on RGB as well as other formats. */
    NEXUS_MacrovisionType_eType3Rgb,           /* Mv goes on RGB as well as other formats. */
    NEXUS_MacrovisionType_eTest01,             /* MV certification test 01 */
    NEXUS_MacrovisionType_eTest02,             /* MV certification test 02 */
    NEXUS_MacrovisionType_eMax
} NEXUS_MacrovisionType;

/***************************************************************************
Summary:
    Enumerated type that specifies the structure of a 3D source
***************************************************************************/
typedef enum NEXUS_Video3DStructure
{
    NEXUS_Video3DStructure_eFramePacking,
    NEXUS_Video3DStructure_eFieldAlternative,
    NEXUS_Video3DStructure_eLineAlternative,
    NEXUS_Video3DStructure_eSidexSideFull,
    NEXUS_Video3DStructure_eLDepth,
    NEXUS_Video3DStructure_eLDepthGraphics,
    NEXUS_Video3DStructure_eTopAndBottom,
    NEXUS_Video3DStructure_eReserved,
    NEXUS_Video3DStructure_eSidexSideHalf, /* 0x8 1000 */
    NEXUS_Video3DStructure_eMax
} NEXUS_Video3DStructure;

/***************************************************************************
Summary:
    Enumerated type that specifies the sub-sampling method of a 3D source
***************************************************************************/
typedef enum NEXUS_Video3DSubSample
{
    NEXUS_Video3DSubSample_eNone,
    NEXUS_Video3DSubSample_eHorzOLOR = 0 ,
    NEXUS_Video3DSubSample_eHorzOLER,
    NEXUS_Video3DSubSample_eHorzELOR,
    NEXUS_Video3DSubSample_eHorzELER,
    NEXUS_Video3DSubSample_eQuinOLOR,
    NEXUS_Video3DSubSample_eQuinOLER,
    NEXUS_Video3DSubSample_eQuinELOR,
    NEXUS_Video3DSubSample_eQuinELER,
    NEXUS_Video3DSubSample_eMax
} NEXUS_Video3DSubSample;

/**
Summary:
Orientation of video

Description:
See Also:
NEXUS_VideoFormat
**/
typedef enum NEXUS_VideoOrientation {
    NEXUS_VideoOrientation_e2D,             /* 2D */
    NEXUS_VideoOrientation_e3D_LeftRight,   /* 3D left right */
    NEXUS_VideoOrientation_e3D_OverUnder,   /* 3D over under */
    NEXUS_VideoOrientation_eMax
} NEXUS_VideoOrientation;

/**
Summary: Internal function to create instance of NEXUS_VideoInput 
**/
NEXUS_VideoInputHandle NEXUS_VideoInput_Create( /* attr{destructor=NEXUS_VideoInput_Destroy } */
        void
        ); 

/**
Summary: Internal function to be paired with NEXUS_VideoInput_Create 
**/
void NEXUS_VideoInput_Destroy(
        NEXUS_VideoInputHandle videoInput
        );


#ifdef __cplusplus
}
#endif

#endif /* NEXUS_VIDEO_TYPES_H */

