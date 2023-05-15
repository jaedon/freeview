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
 * $brcm_Workfile: nexus_video_decoder_priv.h $
 * $brcm_Revision: 18 $
 * $brcm_Date: 12/16/11 5:28p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/7400/include/priv/nexus_video_decoder_priv.h $
 * 
 * 18   12/16/11 5:28p erickson
 * SW7425-1878: merge XUDlib support for video encoder userdata
 * 
 * SW7425-1878/1   12/9/11 1:33p ajagmag
 * SW7425-1878: Integrate XUDlib into nexus
 * 
 * 17   8/8/11 7:36p bandrews
 * SW3548-1159: merge to main
 * 
 * 16   6/14/11 3:11p vsilyaev
 * SW7425-654: Propagate STG/trigger index from display to the video
 *  decoder
 * 
 * 15   12/10/10 5:06p erickson
 * SW35230-2209: add NEXUS_VideoDecoderSettings.scanMode
 * 
 * SW3548-1159/1   11/2/10 6:39p bandrews
 * SW3548-1159: added non-TSM mode recovery methods and some options for
 *  controlling ASTM better
 * 
 * 14   3/10/10 2:35p erickson
 * SW7405-3965: add NEXUS_VideoDecoderDisplayConnection.parentIndex
 *
 * 13   12/9/09 3:10p erickson
 * SW7405-3394: refactor mosaic mode API
 *
 * 12   5/4/09 1:38p erickson
 * PR54523: added is608data param
 *
 * 11   3/30/09 12:52p erickson
 * PR53544: remove vbi dependency from video_decoder
 *
 * 10   3/30/09 11:16a erickson
 * PR48963: added NEXUS_VideoDecoder_GetHeap_priv
 *
 * 9   12/15/08 10:21a jgarrett
 * PR49720: PR49720: Added watchdog recovery to astm
 *
 * 8   12/8/08 9:51a jgarrett
 * PR 41322: auto start astm from decoder start signal
 *
 * PR41322/1   12/5/08 6:07p bandrews
 * PR41322: auto start astm from decoder start signal
 *
 * PR49720/1   12/5/08 5:31p bandrews
 * PR49720: Added watchdog recovery to astm
 *
 * 7   3/26/08 9:46p bandrews
 * PR40090: stc channel must be passed to astm before video is started
 *
 * 6   3/26/08 3:38p vsilyaev
 * PR 40862: Added code to propagate display framerate to the decoder
 *
 * 5   3/25/08 5:25p bandrews
 * PR40090: Added compile flag for ASTM
 *
 * 4   3/24/08 3:16p bandrews
 * PR40090: API changes
 *
 * 3   2/1/08 3:17p erickson
 * PR38679: rework display connection
 *
 * 2   1/31/08 9:44a erickson
 * PR36808: added private SyncChannel connection
 *
 * Nexus_Devel/11   10/8/07 11:56a erickson
 * PR34926: reworked VBI interface
 *
 * Nexus_Devel/10   10/4/07 3:44p erickson
 * PR34926: NEXUS_VideoDecoder_GetSourceId_priv
 *
 * Nexus_Devel/9   10/4/07 3:07p erickson
 * PR34926: remove video descriptor
 *
 * Nexus_Devel/8   10/3/07 1:06p erickson
 * PR34926: impl userdata
 *
 * Nexus_Devel/7   10/3/07 9:50a erickson
 * PR34926: code convention
 *
 * Nexus_Devel/6   9/28/07 11:32a erickson
 * PR34926: rename videodecoder to video_decoder
 *
 * Nexus_Devel/5   9/26/07 11:01a erickson
 * PR34926: improve comments
 *
 * Nexus_Devel/4   9/24/07 12:52p erickson
 * PR34926: update Display connection
 *
 * Nexus_Devel/3   9/24/07 9:29a erickson
 * PR34926: added _priv suffix
 *
 * Nexus_Devel/2   9/21/07 4:18p erickson
 * PR34926: added DisplayConnection private interface
 *
 **************************************************************************/
#ifndef NEXUS_VIDEO_DECODER_PRIV_H__
#define NEXUS_VIDEO_DECODER_PRIV_H__

#include "nexus_video_decoder.h"
#include "priv/nexus_core_video.h"
#include "bint.h"
#include "bavc.h"

/*-**************
Private API for VideoDecoder.
****************/

#ifdef __cplusplus
extern "C" {
#endif

/**
Summary:
**/
typedef void (*NEXUS_VideoDecoderDataReadyCallback)(void *context, const BAVC_MFD_Picture *picture);

/**
Summary:
**/
typedef void (*NEXUS_VideoDecoderUserDataCallback)(void *context, const BAVC_USERDATA_info *userDataInfo);


/**
Summary:
VBI isr callback from VideoDecoder to Display.
**/
typedef void (*NEXUS_DigitalVbiDataCallback)(
    NEXUS_VideoInput input,
    bool is608data, /* true if EIA-608 */
    const NEXUS_ClosedCaptionData *pData /* one item of EIA-608 or EIA-708 data. */
    );

/**
Summary:
Data needed to connect VideoDecoder to a Display.
**/
typedef struct NEXUS_VideoDecoderDisplayConnection
{
    struct
    {
        BINT_Id intId; /* These L2 interrupt ID's allow the VideoDecoder to receive VEC interrupts to drive the display manager. */
    } top, bottom, frame;

    NEXUS_VideoDecoderDataReadyCallback dataReadyCallback_isr; /* Allows VideoDecoder to notify Display when a new picture is available. */

    NEXUS_VideoDecoderUserDataCallback userDataCallback_isr; /* Allows VideoDecoder to pass user data to display for VEC encoding */

    NEXUS_DigitalVbiDataCallback vbiDataCallback_isr; /* Allows VideoInput to pass VBI data to Display for user capture or VEC encoding. */

    void *callbackContext;

    /* status */
    unsigned mosaicIndex; /* VideoDecoder reports the XVD channel index so VDC can match it */
    unsigned parentIndex; /* parentIndex corresponds to the MFD index.
                             For mosaic, it's the parent index. For non-mosaic, it's the decoder index. */
    bool psfMode;                             
} NEXUS_VideoDecoderDisplayConnection;

#if NEXUS_HAS_ASTM
/**
Summary:
Used by Astm
**/
typedef struct NEXUS_VideoDecoderAstmSettings
{
    bool enableAstm; /* Astm will enable/disable astm mode */
    unsigned int syncLimit; /* limit outside of which decoder will not apply astm mode */
    bool enableTsm; /* Astm will enable/disable tsm. */
    bool enablePlayback; /* Astm will enable/disable playback. */
    int32_t ptsOffset; /* Astm will use this while in TSM-disabled mode */
    NEXUS_Callback firstPts_isr; /* Notify Astm when First PTS occurs. */
    NEXUS_Callback tsmPass_isr; /* Notify Astm when TSM passes. */
    NEXUS_Callback tsmFail_isr; /* Notify Astm when TSM fails. */
    NEXUS_Callback tsmLog_isr; /* Notify Astm when TSM log buffer fills */
    NEXUS_Callback lifecycle_isr; /* Notify Astm when decoder is started/stopped */
    NEXUS_Callback watchdog_isr; /* Notify Astm when a watchdog occurs */
    void *callbackContext; /* user context passed callback_isr */
} NEXUS_VideoDecoderAstmSettings;

/**
Summary:
Used by Astm
**/
typedef struct NEXUS_VideoDecoderAstmStatus
{
    bool started;
    uint32_t pts;
    int32_t ptsStcDiff;
    unsigned decodedCount;
    struct
    {
        unsigned int address;
        unsigned int size;
    } tsmLog;
} NEXUS_VideoDecoderAstmStatus;

#endif /* NEXUS_HAS_ASTM */

/**
Summary:
Get current connection to Display
**/
void NEXUS_VideoDecoder_GetDisplayConnection_priv(
    NEXUS_VideoDecoderHandle handle,
    NEXUS_VideoDecoderDisplayConnection *pConnection /* [out] */
    );

/**
Summary:
Update connection to Display
**/
NEXUS_Error NEXUS_VideoDecoder_SetDisplayConnection_priv(
    NEXUS_VideoDecoderHandle handle,
    const NEXUS_VideoDecoderDisplayConnection *pConnection
    );

/**
Summary:
BAVC_SourceId indicates the MPEG feeder (MFD) where the picture will be available
**/
void NEXUS_VideoDecoder_GetSourceId_priv(
    NEXUS_VideoDecoderHandle handle,
    BAVC_SourceId *pSource /* [out] */
    );

/**
Summary:
Heap used by the video decoder
**/
void NEXUS_VideoDecoder_GetHeap_priv(
    NEXUS_VideoDecoderHandle handle,
    NEXUS_HeapHandle *pHeap /* [out] */
    );

/**
Summary:
SyncChannel calls VideoInput which calls this.
**/
void NEXUS_VideoDecoder_GetSyncSettings_priv(
    NEXUS_VideoDecoderHandle videoDecoder,
    NEXUS_VideoInputSyncSettings *pSyncSettings /* [out] */
    );

/**
Summary:
SyncChannel calls VideoInput which calls this.
**/
NEXUS_Error NEXUS_VideoDecoder_SetSyncSettings_priv(
    NEXUS_VideoDecoderHandle videoDecoder,
    const NEXUS_VideoInputSyncSettings *pSyncSettings
    );

/**
Summary:
SyncChannel calls VideoInput which calls this.
**/
NEXUS_Error NEXUS_VideoDecoder_GetSyncStatus_isr(
    NEXUS_VideoDecoderHandle videoDecoder,
    NEXUS_VideoInputSyncStatus *pSyncStatus /* [out] */
    );

typedef struct NEXUS_VideoDecoder_DisplayInformation {
    unsigned refreshRate; /* refresh rate in units of 1/100th Hz.  For example 60.00Hz would be 6000, and 59.94Hz would be 5994 */
    unsigned stgIndex; /* if display type is simple time genenrator, this would be index (number) of STG hardware, otherwise undefined */
} NEXUS_VideoDecoder_DisplayInformation;

void NEXUS_VideoDecoder_UpdateDisplayInformation_priv(
    NEXUS_VideoDecoderHandle videoDecoder,
    const NEXUS_VideoDecoder_DisplayInformation *displayInformation
    );



#if NEXUS_HAS_ASTM
/**
Summary:
Astm calls this.
**/
void NEXUS_VideoDecoder_GetAstmSettings_priv(
    NEXUS_VideoDecoderHandle videoDecoder,
    NEXUS_VideoDecoderAstmSettings * pAstmSettings /* [out] */
    );

/**
Summary:
Astm calls this.
**/
NEXUS_Error NEXUS_VideoDecoder_SetAstmSettings_priv(
    NEXUS_VideoDecoderHandle videoDecoder,
    const NEXUS_VideoDecoderAstmSettings * pAstmSettings
    );

/**
Summary:
Astm calls this.
**/
NEXUS_Error NEXUS_VideoDecoder_GetAstmStatus_isr(
    NEXUS_VideoDecoderHandle videoDecoder,
    NEXUS_VideoDecoderAstmStatus * pAstmStatus /* [out] */
    );

#endif /* NEXUS_HAS_ASTM */

#ifdef __cplusplus
}
#endif

#endif
