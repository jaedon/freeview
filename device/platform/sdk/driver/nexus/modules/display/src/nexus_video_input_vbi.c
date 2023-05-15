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
 * $brcm_Workfile: nexus_video_input_vbi.c $
 * $brcm_Revision: 64 $
 * $brcm_Date: 7/13/12 3:39p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/display/7400/src/nexus_video_input_vbi.c $
 * 
 * 64   7/13/12 3:39p erickson
 * SW7425-3107: refactor cc/tt/gs capture to prevent isr race-condition,
 *  prevent cpu-reordering failure, and to reduce code duplication
 *
 * 63   3/23/12 12:08p jtna
 * SW7346-725: use NEXUS_VideoInput_P_GetExisting() to avoid creating VDC
 *  source when not needed
 *
 * 62   3/14/12 12:25p erickson
 * SW7401-4517: remove old code
 *
 * 61   12/20/11 9:42a erickson
 * SW7346-617: move BVBI_TT_Line array off stack to avoid potential isr-
 *  time overflow
 *
 * 60   11/15/11 3:27p erickson
 * SW7425-1747: remove old code
 *
 * 59   9/19/11 10:12a ahulse
 * SW7405-5416: don't call SetDisplayConnection_priv needlessly
 *
 * 58   3/15/11 10:06a erickson
 * SW7342-276: store exact NEXUS_ClosedCaptionData from VideoDecoder
 *
 * 57   3/11/11 9:14a erickson
 * SW7342-276: init NEXUS_ClosedCaptionData and set cc.type. remove
 *  unnecessary memsets from flush functions. add critical sections on
 *  possible non-atomic accesses.
 *
 * 56   12/23/10 12:50p erickson
 * SW7425-34: remove NEXUS_IS_ANALOG_DISPLAY. instead, use
 *  BAVC_VbiPath_eUnknown returned by BVDC_Display_GetVbiPath to determine
 *  if display is VBI capable
 *
 * 55   6/24/10 7:40p petlee
 * SW35230-116: Update VBI-TT defaults for 35230
 *
 * 54   5/11/10 5:03p jtna
 * SW7125-307: Coverity - check for null pointer arguments before
 *  dereferencing them
 *
 * 53   12/21/09 4:02p erickson
 * SW3556-898: add flush calls for vps, cgms and wss input
 *
 * 52   11/6/09 4:22p erickson
 * SW7405-3334: disambiguate MSG
 *
 * 51   9/9/09 1:19p erickson
 * SW7405-2977: allow build with no video decoder
 *
 * 50   8/10/09 8:17a gmohile
 * PR 56400 : Merge legacy vdc support to main line
 *
 * 49   7/13/09 9:00a erickson
 * PR56641: fix lastLine logic in teletext copy
 *
 * 48   6/15/09 5:01p erickson
 * PR56022: deprecated numWindows and numDisplays
 *
 * 47   5/4/09 1:39p jtna
 * PR54523: fix handling of EIA-708 data
 *
 * 46   3/30/09 12:53p erickson
 * PR53544: add NEXUS_VideoInput_P_DigitalVbiData_isr so that
 *  video_decoder doesn't have to call VBI PI
 *
 * 45   3/16/09 11:07a erickson
 * PR53248: remove badParityBlock filter, add
 *  NEXUS_ClosedCaptionData.parityError.
 *
 * 44   3/10/09 4:30p erickson
 * PR53042: check for NULL on every call to NEXUS_VideoInput_P_Get
 *
 * 43   3/10/09 10:58a erickson
 * PR51253: added NEXUS_TeletextLine.topField
 *
 * 42   3/6/09 10:15a erickson
 * PR52853: added coverity hint
 *
 * 41   2/27/09 10:17a erickson
 * PR52115: provide filter on bad CC data
 *
 * 40   2/17/09 4:56p erickson
 * PR52115: another change to ensure cc data with parity errors is passed
 *  along
 *
 * 39   2/17/09 4:27p erickson
 * PR52115: added NEXUS_ClosedCaptionData.noData boolean
 *
 * 38   12/24/08 12:40p jgarrett
 * PR 50703: Disabling VBI for 7420
 *
 * 37   12/8/08 5:04p erickson
 * PR50058: rework internal VideoInput vbi accounting, don't store refcnt
 *
 * 36   11/24/08 10:15a erickson
 * PR49623: allow vbi encoding on bypass display
 *
 * 35   11/24/08 9:40a erickson
 * PR43413: fix BKNI_Memcmp logic
 *
 * 34   11/21/08 12:53p erickson
 * PR43413: added vps decode
 *
 * 33   11/3/08 1:34p erickson
 * PR47392: only test WSS on top fields
 *
 * 32   11/3/08 10:30a erickson
 * PR47392: remove unneeded ERR
 *
 * 31   10/31/08 12:15a erickson
 * PR47392: use value of 0xFFFF for no WSS data
 *
 * 30   10/24/08 11:16a erickson
 * PR48006: protect against NULL pFmtInfo
 *
 * 29   10/16/08 5:25p erickson
 * PR48006: call NEXUS_AnalogVideoDecoder_P_SetVbi with VDC source
 *  callback format
 *
 * 28   10/16/08 1:55p erickson
 * PR47070: remove comment
 *
 * 27   10/16/08 12:32p erickson
 * PR47070: change NEXUS_GemStarData structure
 *
 * 26   10/7/08 9:21p erickson
 * PR47070: added gemstar refcnt
 *
 * 25   10/1/08 3:43p erickson
 * PR47070: extended NEXUS_GemStarData to include baseLineNumber, lineMask
 *  and errorMask
 *
 * 24   9/26/08 11:58a erickson
 * PR46515: add VDEC and VBI power management
 *
 * 23   9/24/08 5:21p erickson
 * PR47070: add vbi closedCaption, teletext and gemStar DataReady
 *  callbacks
 *
 * 22   9/18/08 12:53p erickson
 * PR47070: added gemstar options to NEXUS_VideoInputVbiSettings
 *
 * 21   9/8/08 9:46a erickson
 * PR46068: move BDBG_OBJECT_ASSERT earlier in
 *  NEXUS_VideoInput_P_VbiData_isr
 *
 * 20   9/2/08 7:48p katrep
 * PR46457: Configure nexus features at the run time based on board strap
 *  options
 *
 * 19   7/23/08 1:55p erickson
 * PR44557: BVBI_Field_GetWSSData_isr rc is actually
 *  BVBI_ERR_FIELD_BADDATA
 *
 * 18   7/18/08 5:10p erickson
 * PR44557: allow BVBI_LINE_ERROR_WSS_PARITY from
 *  BVBI_Field_GetWSSData_isr to succeed
 *
 * 17   6/16/08 1:56p erickson
 * PR43504: added GemStar
 *
 * 16   5/20/08 10:16a erickson
 * PR42595: only store EIA-708 data if closedCaptionEnabled is true;
 *  otherwise, we get false overflow messages.
 *
 * 15   5/1/08 9:38a erickson
 * PR41533: set cc data.field for 608
 *
 * 14   4/29/08 9:55a erickson
 * PR41533: check for no cc buffer before use
 *
 * 13   4/28/08 4:20p erickson
 * PR41533: update 3563 vbi
 *
 * 12   4/28/08 4:10p erickson
 * PR41533: unify EIA-608 and EIA-708 closed caption data capture in
 *  NEXUS_VideoInput_ReadClosedCaption
 *
 * 11   4/23/08 4:29p erickson
 * PR42071: merge
 *
 * PR42071/1   4/23/08 6:51p honglang
 * PR42071:  return VBI list handle before abnormal function return.
 *
 * 10   4/10/08 1:41p erickson
 * PR39000: fix compilation error
 *
 * 9   4/10/08 11:39a erickson
 * PR39000: don't call NEXUS_VideoInput_P_Get in isr context
 *
 * 8   4/9/08 9:18a erickson
 * PR39000: merge
 *
 * PR39000/2   4/9/08 1:09p chengs
 * PR39000: Fix the bug that cause kylin kill when switch source or
 *  channel with CC support.
 *
 * 7   3/31/08 1:41p erickson
 * PR41077: added NEXUS_ASSERT_MODULE to _priv function
 *
 * 6   3/24/08 11:28a erickson
 * PR39000: added NEXUS_VideoInput_FlushClosedCaption and
 *  NEXUS_VideoInput_FlushTeletext
 *
 * PR39000/1   3/21/08 6:44p chengs
 * PR39000: Add a new API to support closed cpationing.
 *
 * 5   2/7/08 12:15p erickson
 * PR38679: added teletext data processing short circuiting, added
 *  comments
 *
 * 4   2/4/08 10:40a erickson
 * PR38679: debug update
 *
 * 3   2/1/08 3:32p erickson
 * PR38679: fix no-analog platforms
 *
 * 2   2/1/08 3:20p erickson
 * PR38679: add vbi support
 *
 * 1   1/18/08 2:20p jgarrett
 * PR 38808: Merging to main branch
 *
 * Nexus_Devel/1   1/11/08 1:51p erickson
 * PR38679: add vbi read/write api's
 *
 **************************************************************************/
#include "nexus_base.h"
#include "nexus_display_module.h"
#include "priv/nexus_core.h"

BDBG_MODULE(nexus_video_input_vbi);

void NEXUS_VideoInput_GetVbiSettings( NEXUS_VideoInput videoInput, NEXUS_VideoInputVbiSettings *pSettings )
{
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_Get(videoInput);
    if (link) {
        *pSettings = link->vbiSettings;
    }
    else {
        BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    }
}

NEXUS_Error NEXUS_VideoInput_SetVbiSettings( NEXUS_VideoInput videoInput, const NEXUS_VideoInputVbiSettings *pSettings )
{
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_Get(videoInput);

    if (!link) return BERR_TRACE(NEXUS_UNKNOWN);

    link->vbiSettings = *pSettings;
    NEXUS_IsrCallback_Set(link->vbi.wss.isrCallback, &link->vbiSettings.wssChanged);
    NEXUS_IsrCallback_Set(link->vbi.cgms.isrCallback, &link->vbiSettings.cgmsChanged);
    NEXUS_IsrCallback_Set(link->vbi.vps.isrCallback, &link->vbiSettings.vpsChanged);
    NEXUS_IsrCallback_Set(link->vbi.cc.isrCallback, &link->vbiSettings.closedCaptionDataReady);
    NEXUS_IsrCallback_Set(link->vbi.tt.isrCallback, &link->vbiSettings.teletextDataReady);
    NEXUS_IsrCallback_Set(link->vbi.gs.isrCallback, &link->vbiSettings.gemStarDataReady);
    return NEXUS_VideoInput_P_SetVbiState(videoInput);
}

#define NEXUS_VideoInput_P_FifoWptr(fifo) (&((uint8_t*)(fifo)->data)[(fifo)->elementSize*(fifo)->wptr])

static void NEXUS_VideoInput_P_IncFifo_isr(NEXUS_VideoInput_P_Link_Fifo *fifo) 
{
    if (++fifo->wptr == fifo->bufferSize) {
        fifo->wptr = 0;
    }
    if (fifo->wptr == fifo->rptr) {
        BDBG_WRN(("%s overflow", fifo->name));
    }
    NEXUS_IsrCallback_Fire_isr(fifo->isrCallback);
}

static void NEXUS_VideoInput_P_Fifo_Flush(NEXUS_VideoInput_P_Link_Fifo *fifo)
{
    /* because we're modifying the wptr & rptr in sync, we must do a critical section */
    BKNI_EnterCriticalSection();
    fifo->wptr = 0;
    fifo->rptr = 0;
    BKNI_LeaveCriticalSection();
    return;
}

static NEXUS_Error NEXUS_VideoInput_P_ReallocFifo(unsigned desiredNum, NEXUS_VideoInput_P_Link_Fifo *fifo)
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    if (desiredNum != fifo->bufferSize) {
        /* enter/leave critical section used as barrier against already running isr */
        BKNI_EnterCriticalSection();
        fifo->inactive = true;
        fifo->wptr = 0;
        fifo->rptr = 0;
        BKNI_LeaveCriticalSection();
        if (fifo->data) {
            BKNI_Free(fifo->data);
            fifo->data = NULL;
        }
        fifo->bufferSize = desiredNum;
        if (desiredNum) {
            fifo->data = BKNI_Malloc(fifo->elementSize * desiredNum);
            if (!fifo->data) {
                rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
            }
        }
        /* enter/leave critical section used as barrier so '*ptr' would get updated prior to 'inactive' */
        BKNI_EnterCriticalSection();
        fifo->inactive = false;
        BKNI_LeaveCriticalSection();
    }
    return rc;
}

static void NEXUS_VideoInput_P_ReadFifo(NEXUS_VideoInput_P_Link_Fifo *fifo, void *pLines, unsigned numLines, unsigned *pNumLinesRead)
{
    unsigned local_wptr;
    unsigned total_read = 0;

    /* critical section used as barrier to ensure thread sees wptr update before data[] update */
    BKNI_EnterCriticalSection();
    local_wptr = fifo->wptr;
    BKNI_LeaveCriticalSection();
    while (total_read<numLines && local_wptr != fifo->rptr && fifo->data) {
        uint8_t *dest = pLines;
        uint8_t *src = fifo->data;
        dest = &dest[fifo->elementSize*total_read++];
        src = &src[fifo->elementSize*fifo->rptr++];
        BKNI_Memcpy(dest, src, fifo->elementSize);
        if (fifo->rptr == fifo->bufferSize) {
            fifo->rptr = 0;
        }
    }
    *pNumLinesRead = total_read;
}

NEXUS_Error NEXUS_VideoInput_ReadTeletext( NEXUS_VideoInput videoInput, NEXUS_TeletextLine *pLines,
    size_t numLines, size_t *pNumLinesRead )
{
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_Get(videoInput);
    if (!link) return BERR_TRACE(NEXUS_UNKNOWN);
    if (numLines && !pLines) return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    NEXUS_VideoInput_P_ReadFifo(&link->vbi.tt, pLines, numLines, pNumLinesRead);
    return 0;
}

NEXUS_Error NEXUS_VideoInput_ReadClosedCaption( NEXUS_VideoInput videoInput, NEXUS_ClosedCaptionData *pEntries,
    size_t numEntries, size_t *pNumEntriesRead )
{
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_Get(videoInput);
    if (!link) return BERR_TRACE(NEXUS_UNKNOWN);
    if (numEntries && !pEntries) return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    NEXUS_VideoInput_P_ReadFifo(&link->vbi.cc, pEntries, numEntries, pNumEntriesRead);
    return 0;
}

NEXUS_Error NEXUS_VideoInput_ReadGemStar( NEXUS_VideoInput videoInput, NEXUS_GemStarData *pEntries,
    size_t numEntries, size_t *pNumEntriesRead )
{
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_Get(videoInput);
    if (!link) return BERR_TRACE(NEXUS_UNKNOWN);
    if (numEntries && !pEntries) return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    NEXUS_VideoInput_P_ReadFifo(&link->vbi.gs, pEntries, numEntries, pNumEntriesRead);
    return 0;
}

NEXUS_Error NEXUS_VideoInput_GetWss( NEXUS_VideoInput videoInput, uint16_t *pWssData )
{
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_Get(videoInput);

    if (!link) return BERR_TRACE(NEXUS_UNKNOWN);
    BKNI_EnterCriticalSection();
    *pWssData = link->vbi.wss.data;
    BKNI_LeaveCriticalSection();
    return 0;
}

NEXUS_Error NEXUS_VideoInput_GetCgms( NEXUS_VideoInput videoInput, uint32_t *pCgmsData )
{
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_Get(videoInput);

    if (!link) return BERR_TRACE(NEXUS_UNKNOWN);
    BKNI_EnterCriticalSection();
    *pCgmsData = link->vbi.cgms.data;
    BKNI_LeaveCriticalSection();
    return 0;
}

NEXUS_Error NEXUS_VideoInput_GetVps( NEXUS_VideoInput videoInput, NEXUS_VpsData *pVpsData )
{
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_Get(videoInput);

    if (!link) return BERR_TRACE(NEXUS_UNKNOWN);
    BKNI_EnterCriticalSection();
    *pVpsData = link->vbi.vps.data;
    BKNI_LeaveCriticalSection();
    return 0;
}

#if NEXUS_NUM_VIDEO_DECODERS
static void
NEXUS_VideoInput_P_DigitalVbiData_isr(NEXUS_VideoInput videoInput, bool is608data, const NEXUS_ClosedCaptionData *pData)
{
    NEXUS_DisplayModule_State *video= &g_NEXUS_DisplayModule_State;
    if ((pData->field == 0 || pData->field == 1) && is608data) {
        BVBI_Field_Handle vbiField;
        NEXUS_Error rc;

        /* put it into a VBI field structure. this allows analog and digital CC 608 to be processed in the same code path */
        rc = BVBIlib_List_Obtain_isr(video->vbilist, &vbiField);
        if (rc!=BERR_SUCCESS) {
            BDBG_ERR(("BVBIlib_List_Obtain_isr failed, rc=%d", rc));
            return;
        }
        rc = BVBI_Field_SetCCData_isr(vbiField, pData->data[0], pData->data[1]);
        if (rc!=BERR_SUCCESS) {
            BDBG_ERR(("BVBI_Field_SetCCData_isr %#x %#x failed, rc=%d", pData->data[0], pData->data[1], rc));
            BVBIlib_List_Return_isr(video->vbilist, vbiField);
            return;
        }
        rc = BVBI_Field_SetPolarity_isr(vbiField, (pData->field==0) ? (1<<BAVC_Polarity_eTopField) : (1<<BAVC_Polarity_eBotField));
        if (rc!=BERR_SUCCESS) {
            BDBG_ERR(("BVBI_Field_SetPolarity_isr field=%d, failed, rc=%d", pData->field, rc));
            BVBIlib_List_Return_isr(video->vbilist, vbiField);
            return;
        }

        NEXUS_VideoInput_P_VbiData_isr(videoInput, &vbiField, pData, 1);
    }
    else {
        /* this is CC708 data; therefore no encoding. */

        NEXUS_VideoInput_P_Link *link = videoInput->destination;

        if(!link) {
            BDBG_MSG(("708 VBI data from unknown input"));
            return;
        }

        BDBG_OBJECT_ASSERT(link, NEXUS_VideoInput_P_Link);

        if(!link->vbi.cc.inactive) {
            if (!link->vbi.cc.data) {
                BDBG_WRN(("No ClosedCaption buffer"));
            } else if (link->vbiSettings.closedCaptionEnabled) {
                NEXUS_ClosedCaptionData *pCc = (NEXUS_ClosedCaptionData *)NEXUS_VideoInput_P_FifoWptr(&link->vbi.cc);
                *pCc = *pData;
                NEXUS_VideoInput_P_IncFifo_isr(&link->vbi.cc);
            }
        }
    }
}
#endif

void
NEXUS_VideoInput_P_VbiData_isr(NEXUS_VideoInput videoInput, const BVBI_Field_Handle *pVbiData, const NEXUS_ClosedCaptionData *pData, unsigned vbiCount)
{
    #if !B_HAS_VBI
    BSTD_UNUSED(videoInput);
    BSTD_UNUSED(pVbiData);
    BSTD_UNUSED(pData);
    BSTD_UNUSED(vbiCount);
    #else
    unsigned i, j;
    NEXUS_DisplayModule_State *video= &g_NEXUS_DisplayModule_State;
    BERR_Code rc;
    NEXUS_VideoInput_P_Link *link = videoInput->destination;

    /* there may be no link. but if there is, we need to assure right away that it's good. */
    if (link) {
        BDBG_OBJECT_ASSERT(link, NEXUS_VideoInput_P_Link);
    }

#if 0
    BDBG_MSG(("NEXUS_VideoInput_P_VbiData_isr %d, type %s", vbiCount, pVbiData?"608":"708"));
#endif

    if(!link) {
        for (i=0;i<vbiCount;i++) {
            BVBIlib_List_Return_isr(video->vbilist, pVbiData[i]);
        }
        BDBG_MSG(("VBI data from unknown input"));
        return;
    }
    BDBG_ASSERT(videoInput->type == link->input_info.type && videoInput->source == link->input_info.source);

    /* capture data that's desired */
    for (i=0;i<vbiCount;i++) {
        uint32_t cgms;
        BVBI_VPSData vps;
        bool used;
        uint32_t polarity;

        (void)BVBI_Field_GetPolarity_isr(pVbiData[i], &polarity);

        if (link->vbiSettings.closedCaptionEnabled) {
            NEXUS_ClosedCaptionData cc;
            if (pData) {
                /* cc from VideoDecoder will have this set. don't extract from BVBI_Field. */
                cc = pData[i]; /* this will init and copy pts & type */
                rc = 0;
            }
            else {
                BKNI_Memset(&cc, 0, sizeof(cc));
                cc.type = NEXUS_VbiDataType_eClosedCaption;

                cc.field = (polarity & (1 << BAVC_Polarity_eTopField)) ? 0 : 1;

                rc = BVBI_Field_GetCCData_isr(pVbiData[i], &cc.data[0], &cc.data[1]);
                if (rc == 0) {
                    cc.noData = false;
                    cc.parityError = false;
                }
                else if (rc == BVBI_ERR_FIELD_BADDATA) {
                    uint32_t errInfo;
                    (void)BVBI_Field_GetErrorInfo_isr(pVbiData[i], &errInfo);
                    cc.noData = (errInfo & (BVBI_LINE_ERROR_FLDH_CONFLICT | BVBI_LINE_ERROR_CC_NODATA));
                    cc.parityError = (errInfo & BVBI_LINE_ERROR_CC_PARITY);
                    rc = 0;
                }
            }

            if (rc==BERR_SUCCESS && !link->vbi.cc.inactive) {
                if (!link->vbi.cc.data) {
                    BDBG_WRN(("No ClosedCaption buffer"));
                } else {
                    NEXUS_ClosedCaptionData *pCc = (NEXUS_ClosedCaptionData *)NEXUS_VideoInput_P_FifoWptr(&link->vbi.cc);
                    *pCc = cc;
                    NEXUS_VideoInput_P_IncFifo_isr(&link->vbi.cc);
                }
            }
        }

        if (link->vbiSettings.gemStarEnabled) {
            BVBI_GSData gsData;
            rc = BVBI_Field_GetGSData_isr(pVbiData[i], &gsData);
            if (rc==BERR_SUCCESS && !link->vbi.gs.inactive) {
                if (!link->vbi.gs.data) {
                    BDBG_WRN(("No GemStar buffer"));
                } else {
                    NEXUS_GemStarData *pGemStarData = (NEXUS_GemStarData *)NEXUS_VideoInput_P_FifoWptr(&link->vbi.gs);

                    /* convert from Magnum to Nexus */
                    pGemStarData->lineMask = gsData.ulDataLines;
                    pGemStarData->errorMask = gsData.ulErrorLines;
                    pGemStarData->topField = (polarity & (1 << BAVC_Polarity_eTopField));
                    BDBG_CASSERT(sizeof(pGemStarData->data) == sizeof(gsData.ulData));
                    BKNI_Memcpy(pGemStarData->data, gsData.ulData, sizeof(pGemStarData->data));

                    NEXUS_VideoInput_P_IncFifo_isr(&link->vbi.gs);
                }
            }
        }

        if (link->vbiSettings.teletextEnabled) {
            BVBI_TT_Line *ttLines = link->vbi.ttLines;
            rc = BVBI_Field_GetTTData_isr(pVbiData[i], B_VBI_TT_LINES, ttLines);
            if (rc==BERR_SUCCESS && !link->vbi.tt.inactive) {
                unsigned i, invalid_cnt = 0;
                unsigned lastLine = 0;

                /* TODO: I'm short circuiting the write loop. We should investigate a change in teletext data
                allocation, in nexus and/or PI, to avoid memcpy altogether. */
                /* take one pass to see if there are any valid lines */
                for (i=0;i<B_VBI_TT_LINES;i++) {
                    /* coverity[uninit_use: FALSE] - BVBI_Field_GetTTData_isr does set all ucFramingCode values, but the logic is complex */
                    if (ttLines[i].ucFramingCode != 0xFF) {
                        lastLine = i;
                    }
                    else {
                        invalid_cnt++;
                    }
                }
                if (!link->vbi.tt.data && invalid_cnt != B_VBI_TT_LINES) {
                    BDBG_WRN(("Teletext enabled and data found, but no buffer allocated"));
                }
                else if (invalid_cnt != B_VBI_TT_LINES) {
                    /* copy up to the last valid line */
                    for (i=0;i<=lastLine;i++) {
                        NEXUS_TeletextLine *pTT = (NEXUS_TeletextLine *)NEXUS_VideoInput_P_FifoWptr(&link->vbi.tt);
                        pTT->lineNumber = i;
                        /* coverity[uninit_use: FALSE] - BVBI_Field_GetTTData_isr does set all ucFramingCode values, but the logic is complex */
                        pTT->framingCode = ttLines[i].ucFramingCode;
                        pTT->topField = (polarity & (1 << BAVC_Polarity_eTopField));

                        if (pTT->framingCode != 0xFF) {
                            BDBG_MSG(("decoding teletext line fc=%x", pTT->framingCode));
                            BDBG_CASSERT(BVBI_TT_MAX_LINESIZE == NEXUS_TELETEXT_LINESIZE);
                            BKNI_Memcpy(pTT->data, ttLines[i].aucData, NEXUS_TELETEXT_LINESIZE);
                        }

                        NEXUS_VideoInput_P_IncFifo_isr(&link->vbi.tt);
                    }
                }
            }
        }

        /* WSS is defined for top-field only */
        if (polarity & (1 << BAVC_Polarity_eTopField)) {
            uint16_t wss;
            /* TODO: consider a decode-enabled flag so we don't always have to test for this */
            rc = BVBI_Field_GetWSSData_isr(pVbiData[i], &wss);
            /* Allow capture of data with parity errors */
            if (rc != BERR_SUCCESS && rc != BVBI_ERR_FIELD_BADDATA) {
                wss = 0xffff; /* there is no current WSS value, so use a special value. */
            }
            if (wss != link->vbi.wss.data) {
                link->vbi.wss.data = wss;
                NEXUS_IsrCallback_Fire_isr(link->vbi.wss.isrCallback);
            }
        }

        rc = BVBI_Field_GetCGMSData_isr(pVbiData[i], &cgms);
        if (!rc) {
            /* success means there is CGMS data present */
            if (cgms != link->vbi.cgms.data) {
                link->vbi.cgms.data = cgms;
                NEXUS_IsrCallback_Fire_isr(link->vbi.cgms.isrCallback);
            }
        }

        rc = BVBI_Field_GetVPSData_isr(pVbiData[i], &vps);
        if (!rc) {
            /* success means there is VPS data present */
            if (BKNI_Memcmp(&link->vbi.vps.data, &vps, sizeof(vps))) {
                BDBG_CASSERT(sizeof(BVBI_VPSData) == sizeof(NEXUS_VpsData));
                BKNI_Memcpy(&link->vbi.vps.data, &vps, sizeof(vps));
                NEXUS_IsrCallback_Fire_isr(link->vbi.vps.isrCallback);
            }
        }

        /* forward to each display connected to this input */
        used = false;
        for(j=0;j<sizeof(video->displays)/sizeof(video->displays[0]);j++) {
            NEXUS_DisplayHandle display = video->displays[j];
            if (display) {
                if (display->vbi.settings.vbiSource == videoInput) {
                    if (!NEXUS_Display_P_VbiData_isr(display, pVbiData[i])) {
                        used = true;
                    }
                }
            }
        }
        if (!used) {
            /* if not consumed by at least one display, return it */
            BVBIlib_List_Return_isr(video->vbilist, pVbiData[i]);
        }
    }
    #endif
}

NEXUS_Error NEXUS_VideoInput_P_SetVbiState( NEXUS_VideoInput videoInput )
{
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_Get(videoInput);
    NEXUS_DisplayModule_State *video = &g_NEXUS_DisplayModule_State;
    NEXUS_Error rc = 0;
    NEXUS_VideoInputInternalVbiSettings internalVbiSettings;
    unsigned i;

    NEXUS_ASSERT_MODULE();
    BDBG_OBJECT_ASSERT(videoInput, NEXUS_VideoInput);
    if (!link) return BERR_TRACE(NEXUS_UNKNOWN);

    /* figure out what needs to be enabled for this video input */
    BKNI_Memset(&internalVbiSettings, 0, sizeof(internalVbiSettings));
    if (link->vbiSettings.teletextEnabled) internalVbiSettings.teletextRefCnt++;
    if (link->vbiSettings.closedCaptionEnabled) internalVbiSettings.closedCaptionRefCnt++;
    if (link->vbiSettings.wssChanged.callback) internalVbiSettings.wssRefCnt++;
    if (link->vbiSettings.cgmsChanged.callback) internalVbiSettings.cgmsRefCnt++;
    if (link->vbiSettings.gemStarEnabled) internalVbiSettings.gemStarRefCnt++;
    if (link->vbiSettings.vpsChanged.callback) internalVbiSettings.vpsRefCnt++;
    for (i=0;i<NEXUS_NUM_DISPLAYS;i++) {
        #if B_HAS_VBI
        if (video->displays[i] && video->displays[i]->vbi.settings.vbiSource == videoInput) {
            NEXUS_DisplayVbiSettings *displayVbiSettings = &video->displays[i]->vbi.settings;
            if (displayVbiSettings->teletextRouting) internalVbiSettings.teletextRefCnt++;
            if (displayVbiSettings->closedCaptionRouting) internalVbiSettings.closedCaptionRefCnt++;
            if (displayVbiSettings->wssRouting) internalVbiSettings.wssRefCnt++;
            if (displayVbiSettings->cgmsRouting) internalVbiSettings.cgmsRefCnt++;
        }
        #endif
    }

    rc = NEXUS_VideoInput_P_ReallocFifo(link->vbiSettings.closedCaptionBufferSize, &link->vbi.cc);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_VideoInput_P_ReallocFifo(link->vbiSettings.teletextBufferSize, &link->vbi.tt);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_VideoInput_P_ReallocFifo(link->vbiSettings.gemStarBufferSize, &link->vbi.gs);
    if (rc) return BERR_TRACE(rc);

    switch (videoInput->type) {
#if NEXUS_NUM_VIDEO_DECODERS
    case NEXUS_VideoInputType_eDecoder:
        {
        NEXUS_VideoDecoderDisplayConnection decoderConnect;
        bool changed = false;

        NEXUS_Module_Lock(video->modules.videoDecoder);
        NEXUS_VideoDecoder_GetDisplayConnection_priv((NEXUS_VideoDecoderHandle)videoInput->source, &decoderConnect);
        /* VideoDecoder only parses userdata for closedCaption */
        if (internalVbiSettings.closedCaptionRefCnt) {
            if (decoderConnect.vbiDataCallback_isr != NEXUS_VideoInput_P_DigitalVbiData_isr) {
                decoderConnect.vbiDataCallback_isr = NEXUS_VideoInput_P_DigitalVbiData_isr;
                changed = true;
            }
        }
        else {
            if (decoderConnect.vbiDataCallback_isr != NULL) {
                decoderConnect.vbiDataCallback_isr = NULL;
                changed = true;
            }
        }
        if (changed) { /* avoid unecessary work */
            rc = NEXUS_VideoDecoder_SetDisplayConnection_priv((NEXUS_VideoDecoderHandle)videoInput->source, &decoderConnect);
        }
        else {
            rc = 0;
        }
        NEXUS_Module_Unlock(video->modules.videoDecoder);
        }
        break;
#endif
#if NEXUS_NUM_ANALOG_VIDEO_DECODERS
    case NEXUS_VideoInputType_eAnalogVideoDecoder:
        {
        NEXUS_AnalogVideoDecoderHandle analogVideoDecoder = (NEXUS_AnalogVideoDecoderHandle)videoInput->source;
        analogVideoDecoder->vbiSettings = internalVbiSettings;
        /* if there's no current video format, wait to set vbi when there is */
        if (link->vdcSourceCallbackData.pFmtInfo) {
            rc = NEXUS_AnalogVideoDecoder_P_SetVbi(analogVideoDecoder, &link->vbiSettings, link->vdcSourceCallbackData.pFmtInfo->eVideoFmt);
        }
        }
        break;
#endif
    default:
        /* this is normal. don't print error message here. */
        break;
    }

    return rc;
}

void NEXUS_VideoInput_FlushClosedCaption(NEXUS_VideoInput videoInput)
{
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_GetExisting(videoInput);
    if (link) {
        NEXUS_VideoInput_P_Fifo_Flush(&link->vbi.cc);
    }
}

void NEXUS_VideoInput_FlushTeletext(NEXUS_VideoInput videoInput)
{
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_GetExisting(videoInput);
    if (link) {
        NEXUS_VideoInput_P_Fifo_Flush(&link->vbi.tt);
    }
}

void NEXUS_VideoInput_FlushGemStar(NEXUS_VideoInput videoInput)
{
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_GetExisting(videoInput);
    if (link) {
        NEXUS_VideoInput_P_Fifo_Flush(&link->vbi.gs);
    }
}

void NEXUS_VideoInput_FlushWss(NEXUS_VideoInput videoInput)
{
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_GetExisting(videoInput);
    if (link) {
        BKNI_EnterCriticalSection();
        link->vbi.wss.data = 0;
        BKNI_LeaveCriticalSection();
    }
}

void NEXUS_VideoInput_FlushCgms(NEXUS_VideoInput videoInput)
{
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_GetExisting(videoInput);
    if (link) {
        BKNI_EnterCriticalSection();
        link->vbi.cgms.data = 0;
        BKNI_LeaveCriticalSection();
    }
}

void NEXUS_VideoInput_FlushVps(NEXUS_VideoInput videoInput)
{
    NEXUS_VideoInput_P_Link *link = NEXUS_VideoInput_P_GetExisting(videoInput);
    if (link) {
        /* because the data is not atomic, we must do a critical section */
        BKNI_EnterCriticalSection();
        BKNI_Memset(&(link->vbi.vps.data), 0, sizeof(link->vbi.vps.data));
        BKNI_LeaveCriticalSection();
    }
}
