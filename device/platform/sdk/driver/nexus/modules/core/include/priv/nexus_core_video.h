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
* $brcm_Workfile: nexus_core_video.h $
* $brcm_Revision: 26 $
* $brcm_Date: 10/11/12 12:25p $
*
* API Description:
*
* Revision History:
*
* $brcm_Log: /nexus/modules/core/include/priv/nexus_core_video.h $
* 
* 26   10/11/12 12:25p vsilyaev
* SW7435-329: Converted NEXUS_VideoInput to NEXUS_VideoInputHandle and
*  added object verification
* 
* 25   8/3/12 7:02p bandrews
* SW7420-2314: merge to main
* 
* SW7420-2314/1   7/23/12 7:16p bandrews
* SW7420-2314: add event for window source changing
* 
* 24   8/12/11 4:20p jtna
* SW7425-1070: merge video primer ptsStcDiffCorrection feature
* 
* 23   6/28/11 4:29p bandrews
* SW7425-705: merge to main
* 
* SW7425-705/1   6/20/11 9:42p bandrews
* SW7425-705: Disable application of nonzero window delay when source is
*  NRT
* 
* 22   12/9/10 5:15p erickson
* SW7420-1177: add NEXUS_VideoOutputType_eHdmiDvo
* 
* 21   6/23/10 12:10p erickson
* SW7550-453: move NEXUS_StripedSurface to surface module, make handle
*  opaque, add api
*
* 20   2/2/10 7:26p bandrews
* SW7405-3774: add validity flag for delay
*
* 19   7/24/09 9:23a erickson
* PR52812: merge
*
* PR52812/1   5/12/09 4:34p bandrews
* PR52812: Add delay field to sync status
*
* 18   7/24/09 9:18a erickson
* PR56969: add NEXUS_VideoInputType_eHdDvi
*
* 17   4/1/09 11:17a erickson
* PR53623: merge
*
* 16   3/30/09 12:51p erickson
* PR53544: remove vbi dependency from core
*
* 15   10/30/08 11:07p erickson
* PR48453: merge
*
* PR48453/1   10/30/08 10:31p bandrews
* PR48453: DSS support added
*
* 14   6/10/08 7:20p bandrews
* PR42265: Added 120 ms delay for MPEG-2 video streams
*
* 13   5/21/08 10:33a erickson
* PR42678: rename NEXUS_DvoOutput to NEXUS_PanelOutput
*
* 12   4/28/08 4:11p erickson
* PR41533: unify EIA-608 and EIA-708 closed caption data capture in
*  NEXUS_VideoInput_ReadClosedCaption
*
* 11   4/11/08 3:37p vsilyaev
* PR 40921: Added SCART input
*
* 10   3/21/08 12:16p vsilyaev
* PR 39986: Added VideoImageInput
*
* 9   3/18/08 4:41p erickson
* PR40307: added eBufferType
*
* 8   2/28/08 12:00p erickson
* PR39786: added picMemIndex for 7400
*
* 7   2/25/08 2:59p erickson
* PR39786: added NEXUS_StripedSurface
*
* 6   2/20/08 1:32p erickson
* PR39405: added Rfm
*
* 5   2/1/08 3:16p erickson
* PR38679: added NEXUS_VbiDataCallback
*
* 4   1/30/08 5:01p erickson
* PR36808: added common SyncChannel connection structures
*
* 3   1/23/08 8:36p vobadm
* PR35457: update docs
*
* 2   1/23/08 5:16p erickson
* PR35457: update docs
*
* 1   1/18/08 2:22p jgarrett
* PR 38808: Merging to main branch
*
* Nexus_Devel/5   1/3/08 9:20p erickson
* PR38213: rename to AnalogVideoDecoder
*
* Nexus_Devel/4   12/18/07 1:34p erickson
* PR38213: analog video input rework
*
* Nexus_Devel/3   11/26/07 9:49a gezhang
* PR36159: Bring up Nexus on 93563 platform
*
* Nexus_Devel/2   11/6/07 1:19p erickson
* PR36570: rename to Dvo
*
* Nexus_Devel/1   11/2/07 4:36p vsilyaev
* PR 36696: Module that describes video connection interface
*
***************************************************************************/
#ifndef NEXUS_CORE_VIDEO_H_
#define NEXUS_CORE_VIDEO_H_

#include "nexus_types.h"
#include "bavc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum NEXUS_VideoInputType {
    NEXUS_VideoInputType_eDecoder,
    NEXUS_VideoInputType_eCcir656,
    NEXUS_VideoInputType_eAnalogVideoDecoder,
    NEXUS_VideoInputType_ePc,
    NEXUS_VideoInputType_eComposite,
    NEXUS_VideoInputType_eSvideo,
    NEXUS_VideoInputType_eComponent,
    NEXUS_VideoInputType_eIfd,
    NEXUS_VideoInputType_eHdmi,
    NEXUS_VideoInputType_eImage,
    NEXUS_VideoInputType_eScartInput,
    NEXUS_VideoInputType_eHdDvi
} NEXUS_VideoInputType;


struct NEXUS_VideoInput {
    NEXUS_VideoInputType type; /* type of video input */
    void *source; /* polymorphic pointer to the source, must be not NULL */
    void *destination; /* polymorphic pointer to the destination (e.g. DisplayModule), must be NULL if disconnected, must be not NULL if connected */
    unsigned ref_cnt; /* reference counter is incremented if VideoInput is cached/stored in the destination */
    NEXUS_OBJECT(NEXUS_VideoInput); /* shall be last so do not change layout between debug and release builds */
};

typedef struct NEXUS_VideoInput NEXUS_VideoInputObject;

NEXUS_OBJECT_CLASS_DECLARE(NEXUS_VideoInput);

#define NEXUS_VIDEO_INPUT_INIT(input, inputType, src) do { NEXUS_OBJECT_SET(NEXUS_VideoInput, (input) );(input)->type = inputType;(input)->source=src;(input)->destination=NULL;(input)->ref_cnt=0;}while(0)

typedef enum NEXUS_VideoOutputType {
    NEXUS_VideoOutputType_eComposite,
    NEXUS_VideoOutputType_eSvideo,
    NEXUS_VideoOutputType_eComponent,
    NEXUS_VideoOutputType_ePanel,
    NEXUS_VideoOutputType_eRfm,
    NEXUS_VideoOutputType_eHdmi,
    NEXUS_VideoOutputType_eHdmiDvo,
    NEXUS_VideoOutputType_eScartInput,
    NEXUS_VideoOutputType_eCcir656
} NEXUS_VideoOutputType;

BDBG_OBJECT_ID_DECLARE(NEXUS_VideoOutput);

typedef struct NEXUS_VideoOutputObject {
    NEXUS_VideoOutputType type; /* type of video output */
    void *source; /* polymorphic pointer to the source, must be not NULL */
    void *destination; /* polymorphic pointer to the destination (e.g. DisplayHandle), must be NULL if disconnected, must be not NULL if connected */
    unsigned ref_cnt; /* reference counter is incremented if VideoInput is cached/stored in the destination */
    BDBG_OBJECT(NEXUS_VideoOutput) /* shall be last so do not change layout between debug and release builds */
} NEXUS_VideoOutputObject;

#define NEXUS_VIDEO_OUTPUT_INIT(output, outputType, src) do { BDBG_OBJECT_SET((output), NEXUS_VideoOutput);(output)->type = outputType;(output)->source=src;(output)->destination=NULL;(output)->ref_cnt=0;}while(0)

/**
Summary:
Used by SyncChannel
**/
typedef struct NEXUS_VideoInputSyncSettings
{
    unsigned delay;     /* This is the delay applied by SyncChannel. It is always in 45 KHz units. */
    bool mute;          /* Mute video until SyncChannel is ready. */
    unsigned delayCallbackThreshold; /* threshold (in 45 KHz units) to limit frequency of callback */

    NEXUS_Callback delayCallback_isr; /* Notify SyncChannel of phase difference between PTS and STC.
                                         Set to NULL to disable.
                                         int param contains the delay. */
    NEXUS_Callback startCallback_isr; /* Notify SyncChannel when on start/stop.
                                         Call NEXUS_VideoInput_GetSyncStatus_isr to get required info. */
    NEXUS_Callback formatCallback_isr;/* Notify SyncChannel when format changes.
                                         Call NEXUS_VideoInput_GetSyncStatus_isr to get required info. */
    NEXUS_Callback connectionChangedCallback_isr; /* Notify SyncChannel when an input connection changes. */
    void *callbackContext; /* user context passed callback_isr */
} NEXUS_VideoInputSyncSettings;

/**
Summary:
Used by SyncChannel
**/
typedef struct NEXUS_VideoInputSyncStatus
{
    bool started;
    bool digital;
    unsigned int height;
    bool interlaced;
    BAVC_FrameRateCode frameRate;
    bool lastPictureHeld;
    bool delayValid;
    int delay;
    bool nonRealTime;
    uint32_t customPtsOffset; /* milliseconds, replaces primerPtsOffsetMs, codec, and sarnoff_video_delay_workaround */
} NEXUS_VideoInputSyncStatus;

#ifdef __cplusplus
}
#endif

#endif /* NEXUS_CORE_VIDEO_H_ */

