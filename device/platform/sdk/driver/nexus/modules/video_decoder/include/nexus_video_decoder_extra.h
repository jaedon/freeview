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
 * $brcm_Workfile: nexus_video_decoder_extra.h $
 * $brcm_Revision: 27 $
 * $brcm_Date: 8/31/12 11:28a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/include/nexus_video_decoder_extra.h $
 * 
 * 27   8/31/12 11:28a erickson
 * SW7429-253: add NEXUS_VideoDecoderStatus.bufferLatency
 * 
 * 26   1/16/12 11:23a erickson
 * SW7346-629: add NEXUS_VideoDecoder_GetCrcData
 * 
 * 25   11/2/11 4:20p erickson
 * SW7420-1992: move types to nexus_video_decoder_types.h
 * 
 * 24   7/26/11 2:24p erickson
 * SW7405-5424: add
 *  NEXUS_VideoDecoderExtendedSettings.ignoreNumReorderFramesEqZero
 * 
 * 23   4/15/11 10:44a erickson
 * SWDTV-6630: add NEXUS_VideoDecoderExtendedSettings.treatIFrameAsRap
 *
 * 22   4/6/11 2:56p erickson
 * SW7342-340: add HW rave indices to NEXUS_VideoDecoderExtendedStatus
 *
 * 21   3/7/11 11:04a jtna
 * SW7422-304: add 3D SVC decode support
 *
 * 20   2/16/11 1:04p erickson
 * SW7420-1491: add nelem required with nelem_out
 *
 * 19   2/15/11 2:08p erickson
 * SW7420-1491: switch to nelem_out for two out params
 *
 * 18   11/16/10 4:20p jtna
 * SW7405-4997: Add 3DTV status timer
 *
 * 17   10/14/10 6:39p jtna
 * SW7405-4817: add JP3D MPEG2 3D userdata signaling
 *
 * 16   6/29/10 2:21p erickson
 * SW7405-3998: update NEXUS_VideoDecoder_Get3DTVStatus to return codec-
 *  generic 3DTV status
 *
 * 15   6/23/10 12:11p erickson
 * SW7550-453: refactor NEXUS_StripedSurface to use
 *  nexus_striped_surface.h API
 *
 * 14   5/12/10 12:04p erickson
 * SW7405-3998: merge
 *
 * SW7405-3998/1   5/4/10 5:14p jtna
 * SW7405-3998: added SEI frame packing arrangement message callback
 *
 * 13   4/26/10 3:10p erickson
 * SW3556-1090: added
 *  NEXUS_VideoDecoderExtendedSettings.zeroDelayOutputMode
 *
 * 12   2/26/10 1:24p erickson
 * SW3556-1058: added
 *  NEXUS_VideoDecoderExtendedSettings.ignoreDpbOutputDelaySyntax
 *
 * 11   3/24/09 5:00p erickson
 * PR53384: added avd sw uart
 *
 * 10   1/28/09 7:04p vsilyaev
 * PR 46527: Added hints to generate correct proxy code
 *
 * 9   1/16/09 1:41p erickson
 * PR50905: added NEXUS_VideoDecoder_GetMostRecentPts
 *
 * 8   1/12/09 12:29p erickson
 * PR50979: clarify comments on NEXUS_VideoDecoder_CreateStripedSurface
 *
 * 7   10/20/08 9:56a erickson
 * PR46527: add NEXUS_VideoDecoder_CreateStripedMosaicSurfaces and related
 *  code
 *
 * 6   9/8/08 3:03p erickson
 * PR46527: intial checkin of test code to route from AVD->M2MC->GFD
 *
 * 5   7/28/08 12:35p erickson
 * PR42924: move NEXUS_VideoDecoder_ChannelChangeReport out of public API
 *
 * 4   7/7/08 4:46p erickson
 * PR42924: added NEXUS_VideoDecoder_ChannelChangeReport for test
 *
 * 3   5/20/08 3:42p erickson
 * PR42128: added dataReadyCallback to ExtendedSettings
 *
 * 2   5/9/08 3:04p erickson
 * PR42128: provide extended status information
 *
 * 1   4/25/08 10:54a erickson
 * PR41968: added NEXUS_VideoDecoder_Reset
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_DECODER_EXTRA_H__
#define NEXUS_VIDEO_DECODER_EXTRA_H__

#include "nexus_video_decoder.h"
#include "nexus_striped_surface.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
Reset the decoder

Description:
This will cause a reload of video decoder firmware. All current decodes will be restarted and all Nexus handles remain valid.

This could cause the reset of more than just this NEXUS_VideoDecoderHandle, depending on internal implementation requirements.

This is provided for application API requirements. There should be no need for any application to call this function.
If you find that you must call this function, please notify Broadcom so that we can address the problem.
**/
void NEXUS_VideoDecoder_Reset(
    NEXUS_VideoDecoderHandle handle
    );

/**
Summary:
Extended status information from internal "data ready" callback.

Description:
This status information has internal usage and is less likely to be used in typical applications. However, for completeness we have provided it.
Because of its internal nature, this structure is subject to change. Some fields may only apply for certain codecs.
Fields may be added or removed for internal reasons.

See NEXUS_VideoDecoderStatus for the most typical status information.
See NEXUS_VideoDecoderStreamInformation for additional information regarding the last decoded picture (at the head of the picture queue).

See magnum/commonutils/CHIP/bavc.h for additional documentation on these fields
**/
typedef struct NEXUS_VideoDecoderExtendedStatus
{
    unsigned                 dataReadyCount; /* # of dataReady interrupts that have fired since Start.
                                                For a 60fps stream, this number will wrap in 2 years. */
    NEXUS_PicturePolarity    interruptPolarity;
    NEXUS_PicturePolarity    sourcePolarity;
    bool                     isMpeg1; /* if the current NEXUS_VideoCodec is MPEG, this reports true if the VideoDecoder detects this is MPEG1, not MPEG2. */
    bool                     isYCbCr422; /* if true, the decoded video provided to Display if 4:2:2, otherwise it is 4:2:0 */
    bool                     fieldChrominanceInterpolationMode; /* true for field, false for frame */
    uint32_t                 adjQp;
    bool                     pictureRepeatFlag;
    uint32_t                 luminanceNMBY;
    uint32_t                 chrominanceNMBY;
    unsigned                 stripeWidth;
    bool                     captureCrc; /* only true if the decoder is set in CRC mode. */
    uint32_t                 idrPicID; /* only valid if captureCrc is true */
    int32_t                  picOrderCnt; /* only valid if captureCrc is true */
    uint32_t                 sourceClipTop;
    uint32_t                 sourceClipLeft;
    uint32_t                 lumaRangeRemapping;
    uint32_t                 chromaRangeRemapping;
    struct {
        int mainIndex;
        int mainSwRaveIndex; /* if -1, then unused */
        int enhancementIndex; /* if -1, then unused */
        int enhancementSwRaveIndex; /* if -1, then unused */
    } rave;
} NEXUS_VideoDecoderExtendedStatus;

/**
Summary:
Get NEXUS_VideoDecoderExtendedStatus
**/
NEXUS_Error NEXUS_VideoDecoder_GetExtendedStatus(
    NEXUS_VideoDecoderHandle handle,
    NEXUS_VideoDecoderExtendedStatus *pStatus /* [out] */
    );

/**
Summary:
Get extended settings
**/
void NEXUS_VideoDecoder_GetExtendedSettings(
    NEXUS_VideoDecoderHandle handle,
    NEXUS_VideoDecoderExtendedSettings *pSettings /* [out] */
    );

/**
Summary:
Set extended settings
**/
NEXUS_Error NEXUS_VideoDecoder_SetExtendedSettings(
    NEXUS_VideoDecoderHandle handle,
    const NEXUS_VideoDecoderExtendedSettings *pSettings
    );


/**
Summary:
3DTV formats
**/
typedef enum NEXUS_VideoDecoder3DTVFormat
{
    NEXUS_VideoDecoder3DTVFormat_eNone,             /* 2D */
    NEXUS_VideoDecoder3DTVFormat_eSideBySideHalf,
    NEXUS_VideoDecoder3DTVFormat_eTopAndBottomHalf,
    NEXUS_VideoDecoder3DTVFormat_eFramePacking,     /* aka O/U full */
    NEXUS_VideoDecoder3DTVFormat_eMax
} NEXUS_VideoDecoder3DTVFormat;


/**
Summary:
3DTV status

Description:
The 3DTV status is set via messages/userdata contained in the stream.
We recommend that applications use the generic NEXUS_VideoDecoder3DTVStatus.format
and only use the codec-specific data for special cases.
**/
typedef struct NEXUS_VideoDecoder3DTVStatus
{
    NEXUS_VideoDecoder3DTVFormat format; /* codec-generic 3DTV status */

    NEXUS_VideoCodec codec; /* use this to select the codecData union member */
    union {
        /* for avc, this is the SEI frame packing message */
        struct {
            bool valid; /* True if SEI message is valid. If false, the following values are undefined. */
            struct {
                uint32_t flags;
                uint32_t framePackingArrangementId;
                uint32_t framePackingArrangementType;
                uint32_t contentInterpretationType;
                uint32_t frame0GridPositionX;
                uint32_t frame0GridPositionY;
                uint32_t frame1GridPositionX;
                uint32_t frame1GridPositionY;
                uint32_t framePackingArrangementReservedByte;
                uint32_t framePackingArrangementRepetitionPeriod;
            } data;
        } avc; /* set for NEXUS_VideoCodec_eH264 */

        struct {
            bool valid; /* True if mpeg2 data is valid. If false, the following values are undefined. */
            struct {
                uint16_t stereoVideoFormatSignalingType;
            } jp3d; /* ISO/IEC JTC1/SC29/WG11 */

        } mpeg2; /* set for NEXUS_VideoCodec_eMpeg2 */
    } codecData;
} NEXUS_VideoDecoder3DTVStatus;

/**
Summary:
Get NEXUS_VideoDecoder3DTVStatus

Description:
The status is cleared each time before starting decode, and is set when the first 3DTV message is found.
**/
NEXUS_Error NEXUS_VideoDecoder_Get3DTVStatus(
    NEXUS_VideoDecoderHandle handle,
    NEXUS_VideoDecoder3DTVStatus *pStatus /* [out] */
    );

/**
Summary:
Create a striped surface which maps to the most recent picture returned from the decoder

Description:
This is used for a AVD->M2MC->GFD path (BVN bypass) for live video.
See NEXUS_Display_ConnectVideoInput.
**/
NEXUS_StripedSurfaceHandle NEXUS_VideoDecoder_CreateStripedSurface(
    NEXUS_VideoDecoderHandle handle
    );

/**
Summary:
Destroy the surface returned by NEXUS_VideoDecoder_CreateStripedSurface
**/
void NEXUS_VideoDecoder_DestroyStripedSurface(
    NEXUS_VideoDecoderHandle videoDecoder,
    NEXUS_StripedSurfaceHandle stripedSurface
    );

/**
Summary:
Create a striped surface which maps to the most recent set of mosaic picture returned from the decoder

Description:
This is used for a AVD->M2MC->GFD path (BVN bypass) for live video.
See NEXUS_Display_ConnectVideoInput.
**/
NEXUS_Error NEXUS_VideoDecoder_CreateStripedMosaicSurfaces(
    NEXUS_VideoDecoderHandle videoDecoder,
    NEXUS_StripedSurfaceHandle *pStripedSurfaces, /* [out] attr{nelem=maxSurfaces;nelem_out=pSurfaceCount;reserved=10} array of striped surfaces */
    unsigned int maxSurfaces, /* total number of elements in pStripedSurfaces array */
    unsigned int *pSurfaceCount  /* [out] number of surfaces returned in pStripedSurfaces */
    );

/**
Summary:
Destory the surfaces returned by NEXUS_VideoDecoder_CreateStripedMosaicSurfaces
**/
void NEXUS_VideoDecoder_DestroyStripedMosaicSurfaces(
    NEXUS_VideoDecoderHandle videoDecoder,
    const NEXUS_StripedSurfaceHandle *pStripedSurfaces, /* attr{nelem=surfaceCount;reserved=10} array of striped surfaces */
    unsigned int surfaceCount
    );

/**
Summary:
DEPRECATED: use NEXUS_VideoDecoderStatus.bufferLatency instead
Get the PTS for the picture most recently added to the VideoDecoder's FIFO (i.e. CDB)

Description:
The difference between this PTS and the decoder's current PTS (NEXUS_VideoDecoderStatus.pts) represents
the amount of time in the decoder's FIFO. The app is responsible to take into account possible PTS discontinuities.

This function will fail if the FIFO is empty and there are no PTS's. This can happen if the FIFO has run empty.
**/
NEXUS_Error NEXUS_VideoDecoder_GetMostRecentPts(
    NEXUS_VideoDecoderHandle handle,
    uint32_t *pPts /* [out] */
    );

/**
Summary:
Enable the capture of AVD outer ARC messages
**/
NEXUS_Error NEXUS_VideoDecoderModule_SetDebugLog(
    unsigned avdCore,
    bool enabled
    );

/**
Summary:
Capture AVD outer ARC messages after NEXUS_VideoDecoder_SetDebugLog enables the feature
**/
NEXUS_Error NEXUS_VideoDecoderModule_ReadDebugLog(
    unsigned avdCore,
    void *buffer,         /* [out] attr{nelem=bufferSize;nelem_out=pAmountRead} */
    unsigned bufferSize,
    unsigned *pAmountRead /* [out] */
    );
    
/**
Summary:
AVD CRC
**/
typedef struct NEXUS_VideoDecoderCrc
{
    struct {
        uint32_t luma, cb, cr;
    } top, bottom;
} NEXUS_VideoDecoderCrc;

/**
Summar:
Read AVD CRC

Description:
To enable AVD CRC capture, set NEXUS_VideoDecoderExtendedSettings.crcFifoSize to a non-zero value.
**/
NEXUS_Error NEXUS_VideoDecoder_GetCrcData(
    NEXUS_VideoDecoderHandle handle,
    NEXUS_VideoDecoderCrc *pEntries, /* attr{nelem=numEntries;nelem_out=pNumReturned} */
    unsigned numEntries,
    unsigned *pNumReturned
    );

#ifdef __cplusplus
}
#endif

#endif
