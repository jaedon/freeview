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
 * $brcm_Workfile: nexus_still_decoder_priv.c $
 * $brcm_Revision: 30 $
 * $brcm_Date: 5/30/12 4:51p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/7400/src/nexus_still_decoder_priv.c $
 * 
 * 30   5/30/12 4:51p vsilyaev
 * SW7425-3028: Added VP8 still support
 * 
 * 29   3/14/12 12:34p erickson
 * SW7401-4517: remove old code
 * 
 * 28   11/3/11 10:59a erickson
 * SW7346-486: add open/start time codec and buffer control, add
 *  NEXUS_StillDecoder_RequestMemorySize
 * 
 * 27   9/2/11 6:23p vsilyaev
 * SW7425-1103: Added BDBG_CWARNING
 * 
 * 26   5/12/11 3:49p hongtaoz
 * SW7425-466: rework avd/mfd/decoder/xdm mapping;
 * 
 * SW7425-466/1   5/11/11 12:59p hongtaoz
 * SW7425-466: rework avd/mfd/decoder channel mapping;
 * 
 * 25   4/14/11 3:32p jtna
 * SWDTV-6321: fix build warnings for 2.6.37 kernel
 * 
 * 24   1/4/11 10:10a erickson
 * SW7400-2842: remove NEXUS_VIDEO_DECODER_SINGLEDECODE
 *
 * 23   8/17/10 4:22p erickson
 * SW7420-928: use NEXUS_VideoBufferType for striped surface
 *
 * 22   7/7/10 11:44a mward
 * SW7400-2842: Add an option NEXUS_VIDEO_DECODER_SINGLEDECODE=y for XVD
 *  singledecode on 97400.
 *
 * 21   6/23/10 12:11p erickson
 * SW7550-453: refactor NEXUS_StripedSurface to use
 *  nexus_striped_surface.h API
 *
 * 20   7/31/09 4:24p gmohile
 * PR 56512 : Add 7403 support
 *
 * 19   1/27/09 9:56a erickson
 * PR51468: refactor to reduce exposed globals
 *
 * 18   1/26/09 10:55a erickson
 * PR51468: global variable naming convention
 *
 * 17   12/11/08 1:45p erickson
 * PR50171: send XVD correct HD or SD enum
 *
 * 16   8/14/08 5:26p katrep
 * PR45674: Fix compiiler warning in kernel mode non debug builds
 *
 * 15   6/24/08 3:37p erickson
 * PR43693: set stripedSurface.pitch based on XVD's reported
 *  ulStripedWidth
 *
 * 14   6/20/08 12:30p erickson
 * PR43222: fix VC1 stills
 *
 * 13   6/20/08 11:28a erickson
 * PR43222: add divx still support
 *
 * 12   3/22/08 2:07a erickson
 * PR40831: call BXVD_StopDecode if still decode timed out
 *
 * 11   3/21/08 12:29p vsilyaev
 * PR 40307: Fixed compile on 7401
 *
 * 10   3/18/08 4:44p erickson
 * PR40307: set eBufferType
 *
 * 9   3/12/08 9:21a erickson
 * PR40307: fix 7401
 *
 * 8   3/8/08 7:08a erickson
 * PR40103: convert to TaskCallback and IsrCallback
 *
 * 7   3/5/08 2:42p erickson
 * PR40307: add AVC and VC1
 *
 * 6   2/28/08 12:00p erickson
 * PR39786: added picMemIndex for 7400
 *
 * 5   2/26/08 2:42p erickson
 * PR39786: added Stop
 *
 * 4   2/25/08 4:58p erickson
 * PR39786: support non UMA 7405
 *
 * 3   2/25/08 3:01p erickson
 * PR39786: update
 *
 * 2   2/21/08 9:27a erickson
 * PR39786: fix 7400 compilation
 *
 * 1   2/20/08 4:57p erickson
 * PR39786: initial impl
 *
 **************************************************************************/
#include "nexus_video_decoder_module.h"
#include "nexus_still_decoder_impl.h"
#include "priv/nexus_core.h"

BDBG_MODULE(nexus_still_decoder_priv);

void NEXUS_StillDecoder_P_CheckForEndCode( void *context )
{
    NEXUS_StillDecoderHandle stillDecoder = context;
    uint8_t endCode;
    bool found;
    BXVD_DecodeStillMode stillMode;
    BERR_Code rc=0;
    bool supportHd = 
        stillDecoder->settings.output.maxHeight ? stillDecoder->settings.output.maxHeight > 480 :
        (g_NEXUS_videoDecoderModuleSettings.maxStillDecodeFormat >= NEXUS_VideoFormat_e480p);

    BDBG_OBJECT_ASSERT(stillDecoder, NEXUS_StillDecoder);

    stillDecoder->timer = NULL;

    switch (stillDecoder->settings.codec) {
    case NEXUS_VideoCodec_eMpeg2:
        endCode = 0xB7;
        stillMode = supportHd?BXVD_DecodeModeStill_eMPEG_HD:BXVD_DecodeModeStill_eMPEG_SD;
        break;
    case NEXUS_VideoCodec_eH264:
        endCode = 0x0A;
        stillMode = supportHd?BXVD_DecodeModeStill_eAVC_HD:BXVD_DecodeModeStill_eAVC_SD;
        break;
    case NEXUS_VideoCodec_eVc1:
    case NEXUS_VideoCodec_eVc1SimpleMain:
        endCode = 0x0A;
        stillMode = supportHd?BXVD_DecodeModeStill_eVC1_HD:BXVD_DecodeModeStill_eVC1_SD;
        break;
    case NEXUS_VideoCodec_eMpeg4Part2:
    case NEXUS_VideoCodec_eDivx311:
        endCode = 0xB1;
        stillMode = supportHd?BXVD_DecodeModeStill_eMPEG4Part2_HD:BXVD_DecodeModeStill_eMPEG4Part2_SD;
        break;
    case NEXUS_VideoCodec_eVp8:
        endCode = 0;
        stillMode = supportHd?BXVD_DecodeModeStill_eVP8_SD:BXVD_DecodeModeStill_eVP8_HD;
        break;
    default:
        rc=BERR_TRACE(NEXUS_NOT_SUPPORTED);
        return;
    }

    LOCK_TRANSPORT();
    if(endCode) {
        found = NEXUS_Rave_FindVideoStartCode_priv(stillDecoder->rave, endCode);
    } else {
        found = NEXUS_Rave_FindPts_priv(stillDecoder->rave, 0xFFFFFFFF);
    }
    UNLOCK_TRANSPORT();

    if (found) {
        NEXUS_RaveStatus raveStatus;
        NEXUS_Time curTime;

        NEXUS_Time_Get(&curTime);
        BDBG_MSG(("Found still in %d msec", NEXUS_Time_Diff(&curTime, &stillDecoder->startTime)));

        stillDecoder->status.endCodeFound = true;
        NEXUS_TaskCallback_Fire(stillDecoder->endCodeFoundCallback);

        LOCK_TRANSPORT();
        rc = NEXUS_Rave_GetStatus_priv(stillDecoder->rave, &raveStatus);
        UNLOCK_TRANSPORT();
        if (rc) {rc = BERR_TRACE(rc);return;}

        /* setting this to null is our indication that we are waiting for a still decode */
        stillDecoder->stripedSurface.mem = NULL;

        rc = BXVD_DecodeStillPicture(stillDecoder->xvdChannel, stillMode, &raveStatus.xptContextMap);
        if (rc) {rc = BERR_TRACE(rc);return;}
    }
    else {
        /* try again */
        stillDecoder->timer = NEXUS_ScheduleTimer(10, NEXUS_StillDecoder_P_CheckForEndCode, stillDecoder);
    }
}

void NEXUS_VideoDecoder_P_StillReady_isr(void *context, int param, void *data)
{
    NEXUS_StillDecoderHandle stillDecoder = (NEXUS_StillDecoderHandle)context;
    BXVD_StillPictureBuffers *buf = (BXVD_StillPictureBuffers *)data;
    NEXUS_Time curTime;

    BSTD_UNUSED(param);
    BDBG_OBJECT_ASSERT(stillDecoder, NEXUS_StillDecoder);

    /* store at isr time, create NEXUS_StripedSurfaceHandle at task time */
    stillDecoder->stripedSurface.recreate = true;
    stillDecoder->stripedSurface.mem = stillDecoder->parentDecoder->device->mem;

    /* GetDefault done in Open. just update the fields we know about. */
    stillDecoder->stripedSurface.createSettings.picMemIndex = stillDecoder->parentDecoder->mfdIndex;
    stillDecoder->stripedSurface.createSettings.pitch = buf->ulStripedWidth;

    /* copying the members prevents bxvd.h from having to be #included elsewhere */
    stillDecoder->stripedSurface.createSettings.pLumaBuffer = buf->pLumaBuffer;
    stillDecoder->stripedSurface.createSettings.pChromaBuffer = buf->pChromaBuffer;
    stillDecoder->stripedSurface.createSettings.imageWidth = buf->ulImageWidth;
    stillDecoder->stripedSurface.createSettings.imageHeight = buf->ulImageHeight;
    stillDecoder->stripedSurface.createSettings.stripedWidth = buf->ulStripedWidth;
    stillDecoder->stripedSurface.createSettings.lumaStripedHeight = buf->ulLumaStripedHeight;
    stillDecoder->stripedSurface.createSettings.chromaStripedHeight = buf->ulChromaStripedHeight;
    BDBG_CWARNING(BXVD_BufferType_eMax == (BXVD_BufferType)NEXUS_VideoBufferType_eMax);
    stillDecoder->stripedSurface.createSettings.bufferType = buf->eBufferType;

    NEXUS_Time_Get(&curTime);
    BDBG_MSG(("Decoded still in %d msec", NEXUS_Time_Diff(&curTime, &stillDecoder->startTime)));
    stillDecoder->status.stillPictureReady = true;
    NEXUS_IsrCallback_Fire_isr(stillDecoder->stillPictureReadyCallback);
}
