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
 * $brcm_Workfile: nexus_mosaic_video_decoder.h $
 * $brcm_Revision: 9 $
 * $brcm_Date: 5/12/11 4:18p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/7400/include/nexus_mosaic_video_decoder.h $
 * 
 * 9   5/12/11 4:18p erickson
 * SW7420-283: remove linked mosaic decoder feature
 * 
 * 8   12/14/10 3:25p erickson
 * SW7420-1285: add null_allowed attribute
 * 
 * 7   10/22/10 11:16a erickson
 * SW7420-1135: add NEXUS_VideoDecoderMosaicSettings for runtime changes
 *  of maxWidth/maxHeight for mosaic decodes
 * 
 * 6   12/9/09 3:10p erickson
 * SW7405-3394: refactor mosaic mode API
 *
 * 5   11/6/09 4:25p erickson
 * SW7405-3334: add NEXUS_VideoDecoder_SetMosaicVbiRouting
 *
 * 4   9/8/09 4:18p erickson
 * SW7420-283: add support for linked mosaic decoders
 *
 * 3   2/6/08 12:42p vsilyaev
 * PR 38682: Added attributes for the linux/kernel proxy mode
 *
 * 2   1/28/08 9:42a erickson
 * PR35457: comment update
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/2   11/28/07 10:37a erickson
 * PR35457: doc update
 *
 * Nexus_Devel/1   11/8/07 9:11a erickson
 * PR36802: added mosaic mode
 *
 ***************************************************************************/
#ifndef NEXUS_MOSAIC_VIDEO_DECODER_H__
#define NEXUS_MOSAIC_VIDEO_DECODER_H__

#include "nexus_video_decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=****************
In mosaic mode, the video decoder timeslices its main decoder in order to decoder several lower resolution streams
(e.g. CIF or QCIF sized). Each mosaic stream is routed to a special mosaic video window.

See nexus/modules/display/CHIP/include/nexus_mosaic_display.h for the mosaic mode video window API.

See nexus/examples/video/mosaic_decode.c and nexus/examples/dvr/mosaic_playback.c for an example of how to use the
mosaic decode and display API's.
*/

/**
Summary:
Wrapper around normal video decoder settings to provide additional mosaic settings.

Description:
See Also:
NEXUS_VideoDecoder_GetDefaultOpenMosaicSettings
NEXUS_VideoDecoder_OpenMosaic
**/
typedef struct NEXUS_VideoDecoderOpenMosaicSettings
{
    NEXUS_VideoDecoderOpenSettings openSettings; /* normal video decoder settings */
    unsigned maxWidth;      /* Maximum width of a stream to be decoded. This is used to subdivide resources for Mosaic decodes. */
    unsigned maxHeight;     /* Maximum height of a stream to be decoded. This is used to subdivide resources for Mosaic decodes. */
} NEXUS_VideoDecoderOpenMosaicSettings;

/**
Summary:
Get default settings for use in NEXUS_VideoDecoder_OpenMosaic.

Description:
See Also:
NEXUS_VideoDecoder_OpenMosaic
**/
void NEXUS_VideoDecoder_GetDefaultOpenMosaicSettings(
    NEXUS_VideoDecoderOpenMosaicSettings *pOpenSettings   /* [out] */
    );

/**
Summary:
Open a mosaic video decoder within the parent video decoder.

Description:
If you open a mosaic video decoder, so can no longer use the parent video decoder.
Once you close the last mosaic video decoder, you can use the parent video decoder for a non-mosaic decode.

See Also:
NEXUS_VideoDecoder_OpenMosaic
NEXUS_VideoDecoder_Close - use the normal close function for mosaic video decoders
**/
NEXUS_VideoDecoderHandle NEXUS_VideoDecoder_OpenMosaic( /* attr{destructor=NEXUS_VideoDecoder_Close}  */
    unsigned parentIndex,       /* The index of the main decoder which will be used for mosaic decode. You cannot
                                   open a main decoder with this index (using NEXUS_VideoDecoder_Open()) at the same time. */
    unsigned index,             /* The index of the mosaic within the main decoder. These must be sequential.
                                   The index of the mosaic must correspond to the index of the mosaic VideoWindow. This is no sharing of handles
                                   to make this association. */
    const NEXUS_VideoDecoderOpenMosaicSettings *pOpenSettings /* attr{null_allowed=y} Settings for the video decoder which include normal VideoDecoder settings as well
                                   as mosaic-specific settings. */
    );
    
/**
Summary:
Run-time mosaic decoder settings

Description:
For non-mosaic, the max width and height are set at init time using NEXUS_VideoDecoderModuleSettings.maxDecodeFormat.
NEXUS_VideoDecoder_GetMosaicSettings will return 0,0. NEXUS_VideoDecoder_SetMosaicSettings cannot 
change these settings. Instead, it will issue a BDBG_WRN, then succeed. Future chips may allow customization of these decoder capabilities.
**/
typedef struct NEXUS_VideoDecoderMosaicSettings
{
    unsigned maxWidth;  /* Maximum width of a stream to be decoded. 
                           A change will require internal XVD channel to be reopened, which requires there to be no connection to a VideoWindow */
    unsigned maxHeight; /* Maximum height of a stream to be decoded. 
                           See maxWidth comments. */
} NEXUS_VideoDecoderMosaicSettings;

/**
Summary:
If not a mosaic, it will return HD settings.
**/
void NEXUS_VideoDecoder_GetMosaicSettings(
    NEXUS_VideoDecoderHandle handle,
    NEXUS_VideoDecoderMosaicSettings *pSettings /* [out] */
    );
    
/**
Summary:
**/
NEXUS_Error NEXUS_VideoDecoder_SetMosaicSettings(
    NEXUS_VideoDecoderHandle handle,
    const NEXUS_VideoDecoderMosaicSettings *pSettings   
    );
    
#ifdef __cplusplus
}
#endif

#endif
