/***************************************************************************
 *     (c)2007-2013 Broadcom Corporation
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
 * $brcm_Workfile: nexus_video_decoder_userdata.c $
 * $brcm_Revision: SW7425-4680-1/1 $
 * $brcm_Date: 6/24/13 12:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/src/nexus_video_decoder_userdata.c $
 * 
 * SW7425-4680-1/1   6/24/13 12:34p scho
 * SW7425-4680-1: continue processing userdata for 3DTV signaling during
 *  trick mode
 * 
 * 38   3/20/12 1:20p erickson
 * SW7420-2280: protect UserDataReadComplete from bad calls. add flush to
 *  assist app recovery.
 * 
 * 37   2/8/12 1:42p erickson
 * SW7346-605: drop NULL EIA-608 cc data from DSS streams
 * 
 * 36   12/16/11 5:29p erickson
 * SW7425-1878: merge XUDlib support for video encoder userdata
 * 
 * SW7425-1878/2   12/15/11 7:13a ajagmag
 * SW7425-1878: Bug fixes for user data encode use case
 * 
 * SW7425-1878/1   12/13/11 6:19p ajagmag
 * SW7425-1878: Pass through the userdata callback to the display module.
 * 
 * 35   12/12/11 11:24a erickson
 * SW7346-594: move UDP data off stack to avoid kernel mode stack overflow
 * 
 * 34   4/14/11 3:32p jtna
 * SWDTV-6321: fix build warnings for 2.6.37 kernel
 * 
 * 33   3/11/11 3:34p erickson
 * SW7342-276: fix dss cc bug
 *
 * 32   1/18/11 11:59a jtna
 * SW7405-4817: add commented-out debug prints for JP3D parsing
 *
 * 31   11/16/10 5:35p jtna
 * SW7405-4558: enable userdata parsing based on
 *  NEXUS_VideoDecoderExtendedSettings.s3DTVStatusEnabled
 *
 * 30   11/15/10 3:52p erickson
 * SW7342-276: added DSS SD cc and subtitle support via new UDPlib
 *  function
 *
 * 29   11/3/10 4:04p erickson
 * SW3548-2364: use UDP isr functions because we are calling from isr
 *  context
 *
 * 28   10/27/10 3:15p jtna
 * SW7405-4817: fix JP3D parsing logic that was getting stuck in a loop
 *
 * 27   10/21/10 5:21p erickson
 * SW7401-2571: temp check in for dccparse
 *
 * 26   10/14/10 6:39p jtna
 * SW7405-4817: add JP3D MPEG2 3D userdata signaling
 *
 * 25   8/10/10 1:15p erickson
 * SW3548-3022: switch to UDP (userdata parser) for userdata parsing
 *
 * 24   8/9/10 4:56a erickson
 * SW7405-4637: fix logic error in
 *  NEXUS_VideoDecoder_P_UserDataFreeSpace_isr
 *
 * 23   7/29/10 8:27a erickson
 * SW7405-4637: handle userdata ringbuffer wraps internally for easier app
 *  code
 *
 * 22   5/4/10 5:24p mward
 * SW7125-347:  Don't assert the vbiDataCallback_isr in
 *  NEXUS_VideoDecoder_P_ParseUserdata_isr(). It's possible it might be
 *  NULL when the interrupt occurs.
 *
 * 21   1/11/10 2:02p erickson
 * SW7405-3718: use AVD's parsing of AFD from userdata instead of VBIlib.
 *  this eliminates a redundant code path. it also allows the AFD value to
 *  be obtained even if not connected to a video window.
 *
 * 20   11/6/09 5:27p erickson
 * SW7405-3334: refactor to remove display & mosaic logic from
 *  nexus_video_decoder_userdata.c code
 *
 * 19   11/6/09 5:12p erickson
 * SW7405-3334: fix case where userdata might be enabled on mosaic[0], but
 *  routing isn't.
 *
 * 18   11/6/09 4:30p erickson
 * SW7405-3334: implement NEXUS_VideoDecoder_SetMosaicVbiRouting
 *
 * 17   5/27/09 3:59p jtna
 * PR49924: comments to clarify preferredUserDataFormat
 *
 * 16   5/20/09 3:30p erickson
 * PR55042: refactor active format description support. add callback and 8
 *  bit status value.
 *
 * 15   5/4/09 1:40p jtna
 * PR54523: fix handling of EIA-708 data
 *
 * 14   3/30/09 12:52p erickson
 * PR53544: remove vbi dependency from video_decoder
 *
 * 13   3/26/09 4:22p mphillip
 * PR53615: Change formatParsed to be a count, add lastFormatParsed
 *
 * 12   3/25/09 10:36a erickson
 * PR53572: call BVBIlib_List_Return_isr if a VBI PI function fails after
 *  BVBIlib_List_Obtain_isr. also, ensure that we pass only supported
 *  polarity masks to BVBI_Field_SetPolarity_isr.
 *
 * 11   3/17/09 3:16p mphillip
 * PR53336: Update formatParsed with all CC formats encountered.
 *
 * 10   3/5/09 3:04p erickson
 * PR52674: if 608, don't send cc data if cc_valid is false; if 708, set
 *  noData=true if cc_valid is false
 *
 * 9   3/5/09 12:08p erickson
 * PR52813: added NEXUS_UserDataFormat_eAfd53
 *
 * 8   12/12/08 4:20p jtna
 * PR49924: add support for
 *  NEXUS_VideoDecoderSettings.preferredUserDataFormat
 *
 * 7   11/17/08 11:50a erickson
 * PR49202: implement more efficient NEXUS_VideoDecoder_P_AddUserData_isr
 *
 * 6   5/19/08 3:06p erickson
 * PR41962: warn if userDataBufferSize is zero. fix bad assert.
 *
 * 5   5/8/08 3:49p erickson
 * PR41962: added PictureCoding for MPEG video
 *
 * 4   4/28/08 4:11p erickson
 * PR41533: unify EIA-608 and EIA-708 closed caption data capture in
 *  NEXUS_VideoInput_ReadClosedCaption
 *
 * 3   4/21/08 1:50p erickson
 * PR41962: added NEXUS_UserDataHeader.pictureCodingExtension
 *
 * 2   4/18/08 1:53p erickson
 * PR41533: impl EIA-708 parsing and capture
 *
 * 1   4/17/08 4:38p erickson
 * PR41533: rework NEXUS_VideoDecoder_GetUserDataBuffer to use
 *  NEXUS_UserDataHeader before payload
 *
 **************************************************************************/
#include "nexus_video_decoder_module.h"
#include "budp_jp3dparse.h"
#include "budp_dccparse_dss.h"

BDBG_MODULE(nexus_video_decoder_userdata);

#define BDBG_MSG_TRACE(X) /* BDBG_MSG(X) */

NEXUS_Error NEXUS_VideoDecoder_GetUserDataBuffer(NEXUS_VideoDecoderHandle videoDecoder, void **pBuffer, unsigned *pSize)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    if (!videoDecoder->openSettings.openSettings.userDataBufferSize) {
        BDBG_ERR(("You must specify NEXUS_VideoDecoderOpenSettings.userDataBufferSize to use this feature."));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    BKNI_EnterCriticalSection();
    *pBuffer = &videoDecoder->userdata.buf[videoDecoder->userdata.rptr];
    if (videoDecoder->userdata.rptr <= videoDecoder->userdata.wptr) {
        *pSize = videoDecoder->userdata.wptr - videoDecoder->userdata.rptr;
    }
    else if (videoDecoder->userdata.wrap_ptr) {
        BDBG_ASSERT(videoDecoder->userdata.wrap_ptr > videoDecoder->userdata.rptr);
        *pSize = videoDecoder->userdata.wrap_ptr - videoDecoder->userdata.rptr;
    }
    else {
        *pSize = videoDecoder->openSettings.openSettings.userDataBufferSize - videoDecoder->userdata.rptr;
    }
    BKNI_LeaveCriticalSection();

    videoDecoder->userdata.lastGetBufferSize = *pSize;
    return 0;
}

void NEXUS_VideoDecoder_UserDataReadComplete(NEXUS_VideoDecoderHandle videoDecoder, unsigned size)
{
    unsigned userDataBufferSize = videoDecoder->openSettings.openSettings.userDataBufferSize;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (size > videoDecoder->userdata.lastGetBufferSize) {
        BDBG_ERR(("invalid UserDataReadComplete %d > %d", size, videoDecoder->userdata.lastGetBufferSize));
        /* flush so that app has chance of recovery */
        NEXUS_VideoDecoder_FlushUserData(videoDecoder);
        return;
    }
    videoDecoder->userdata.lastGetBufferSize = 0;
    BKNI_EnterCriticalSection();
    BDBG_MSG_TRACE(("NEXUS_VideoDecoder_UserDataReadComplete %d, %d %d %d, %d", size, videoDecoder->userdata.rptr, videoDecoder->userdata.wptr, videoDecoder->userdata.wrap_ptr, userDataBufferSize));
    videoDecoder->userdata.rptr += size;
    if (videoDecoder->userdata.rptr >= userDataBufferSize || (videoDecoder->userdata.wrap_ptr && (videoDecoder->userdata.rptr >= videoDecoder->userdata.wrap_ptr))) {
        if (videoDecoder->userdata.rptr > userDataBufferSize) {
            /* this can't be, so ERR and discard data. */
            BDBG_ERR(("invalid NEXUS_VideoDecoder_UserDataReadComplete size %d (rptr %d %d)", size, videoDecoder->userdata.rptr, userDataBufferSize));
            videoDecoder->userdata.wptr = 0; /* wipe everything out to increase chances of recovery */
        }
        if (videoDecoder->userdata.wrap_ptr && videoDecoder->userdata.rptr > videoDecoder->userdata.wrap_ptr) {
            /* this can't be, so ERR and discard data. */
            BDBG_ERR(("invalid NEXUS_VideoDecoder_UserDataReadComplete size %d (wrap_ptr %d %d)", size, videoDecoder->userdata.rptr, videoDecoder->userdata.wrap_ptr));
            videoDecoder->userdata.wptr = 0; /* wipe everything out to increase chances of recovery */
        }
        videoDecoder->userdata.rptr = 0;
        videoDecoder->userdata.wrap_ptr = 0;
    }
    BDBG_ASSERT(!userDataBufferSize || videoDecoder->userdata.rptr < userDataBufferSize);
    BKNI_LeaveCriticalSection();
}

static void NEXUS_VideoDecoder_P_UserDataFreeSpace_isr(NEXUS_VideoDecoderHandle videoDecoder, unsigned *immediate_space, unsigned *after_wrap_space)
{
    if (videoDecoder->userdata.rptr > videoDecoder->userdata.wptr) {
        *immediate_space = videoDecoder->userdata.rptr - videoDecoder->userdata.wptr - 1;
        *after_wrap_space = 0;
    }
    else {
        if (videoDecoder->userdata.rptr) {
            *immediate_space = videoDecoder->openSettings.openSettings.userDataBufferSize - videoDecoder->userdata.wptr;
            *after_wrap_space = videoDecoder->userdata.rptr - 1;
        }
        else {
            *immediate_space = videoDecoder->openSettings.openSettings.userDataBufferSize - videoDecoder->userdata.wptr - 1;
            *after_wrap_space = 0;
        }
    }
    BDBG_ASSERT(*after_wrap_space+*immediate_space < videoDecoder->openSettings.openSettings.userDataBufferSize);
}

static void NEXUS_VideoDecoder_P_AddUserData_isr(NEXUS_VideoDecoderHandle videoDecoder, void *buffer, unsigned size)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    if (!size) return;
    if (buffer) {
        unsigned userDataBufferSize = videoDecoder->openSettings.openSettings.userDataBufferSize;

        BDBG_MSG_TRACE(("NEXUS_VideoDecoder_P_AddUserData_isr %d, %d %d %d, %d", size, videoDecoder->userdata.rptr, videoDecoder->userdata.wptr, videoDecoder->userdata.wrap_ptr, userDataBufferSize));
        BKNI_Memcpy(&videoDecoder->userdata.buf[videoDecoder->userdata.wptr], buffer, size);
        videoDecoder->userdata.wptr += size;
        if (videoDecoder->userdata.wptr == userDataBufferSize) {
            videoDecoder->userdata.wptr = 0;
        }
        else {
            BDBG_ASSERT(videoDecoder->userdata.wptr <= userDataBufferSize);
        }
    }
    else {
        /* skip the remainder */
        BDBG_MSG_TRACE(("NEXUS_VideoDecoder_P_AddUserData_isr skip %d %d %d, %d", videoDecoder->userdata.rptr, videoDecoder->userdata.wptr, videoDecoder->userdata.wrap_ptr, videoDecoder->openSettings.openSettings.userDataBufferSize));
        videoDecoder->userdata.wrap_ptr = videoDecoder->userdata.wptr;
        videoDecoder->userdata.wptr = 0;
        if (videoDecoder->userdata.wrap_ptr == videoDecoder->userdata.rptr) {
            videoDecoder->userdata.rptr = 0;
            videoDecoder->userdata.wrap_ptr = 0;
        }
    }
}

void NEXUS_VideoDecoder_FlushUserData(NEXUS_VideoDecoderHandle videoDecoder)
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BKNI_EnterCriticalSection();
    videoDecoder->userdata.rptr = videoDecoder->userdata.wptr = videoDecoder->userdata.wrap_ptr = 0;
    BKNI_Memset(&videoDecoder->userdata.status, 0, sizeof(videoDecoder->userdata.status));
    BKNI_LeaveCriticalSection();
    videoDecoder->userdata.lastGetBufferSize = 0;
}

static bool IsEquivalentFormat(NEXUS_UserDataFormat nexus_format, BUDP_DCCparse_Format bvbi_format)
{
    switch (nexus_format) {
    case NEXUS_UserDataFormat_eAtsc53:
        if (bvbi_format == BUDP_DCCparse_Format_ATSC53) { return true; }
        break;
    case NEXUS_UserDataFormat_eScte20:
        if (bvbi_format == BUDP_DCCparse_Format_DVS157) { return true; }
        break;
    case NEXUS_UserDataFormat_eScte21:
        if (bvbi_format == BUDP_DCCparse_Format_DVS053) { return true; }
        break;
    default:
    case NEXUS_UserDataFormat_eAny:
        return true;
        break;
    }
    return false;
}

static void NEXUS_VideoDecoder_P_ParseUserdata_isr(NEXUS_VideoDecoderHandle videoDecoder, const BAVC_USERDATA_info *info)
{
    uint32_t offset = 0;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BDBG_MSG(("ParseUserdata %d:%d:%#x:%d:%d", info->eUserDataType, info->ui32UserDataBufSize,
        (unsigned)info->pUserDataBuffer, info->bTopFieldFirst, info->bRepeatFirstField));

    while (offset < info->ui32UserDataBufSize) {
        BERR_Code rc;
        unsigned i;
        size_t bytesParsed = 0;
        uint8_t ccCount = 0;
        uint8_t dssCcCount = 0;
        uint8_t dssSubtitleCount = 0;
        /* DCCparse data is large enough to overflow the stack, so it is stored in the handle's storage */
        BUDP_DCCparse_ccdata *ccData = videoDecoder->udpData.ccData;
        BUDP_DCCparse_dss_cc_subtitle *dssCcData = videoDecoder->udpData.dssCcData;
        BUDP_DCCparse_dss_cc_subtitle *dssSubtitle = videoDecoder->udpData.dssSubtitle;
        
        if (videoDecoder->startSettings.codec == NEXUS_VideoCodec_eH264) {
            rc = BUDP_DCCparse_SEI_isr(info, offset, &bytesParsed, &ccCount, ccData);
        }
        else if (videoDecoder->transportType == NEXUS_TransportType_eDssEs) {
            /* BUDP_DCCparse_DSS_isr is only DSS SD. for DSS HD uses BUDP_DCCparse_isr.  */
            rc = BUDP_DCCparse_DSS_isr(info, offset, &bytesParsed, &dssCcCount, dssCcData, &dssSubtitleCount, dssSubtitle);
        }
        else {
            rc = BUDP_DCCparse_isr(info, offset, &bytesParsed, &ccCount, ccData);

            /* check for MPEG2 userdata 3D signaling */
            if (videoDecoder->startSettings.codec == NEXUS_VideoCodec_eMpeg2) {
                if (rc == BERR_BUDP_NO_DATA && (offset+bytesParsed < info->ui32UserDataBufSize)) { /* bytesParsed can be non-zero even if rc!=0 */
                    rc = BUDP_JP3Dstart_isr(info, offset, &bytesParsed);
#if 0 /* for debug */
                    for (i=offset; i<offset+bytesParsed; i++) {
                        BKNI_Printf("%02x ", *((uint8_t*)(info->pUserDataBuffer)+i)); /* it should be 0x000001B2 0x4A503344 */
                    }
                    BKNI_Printf("\n");
#endif

                    if (rc == BERR_SUCCESS) {
                        while (1) {
                            uint16_t type;
                            offset += bytesParsed;
                            rc = BUDP_JP3Dparse_isr(info, offset, &bytesParsed, &type);
#if 0 /* for debug */
                            for (i=offset; i<offset+bytesParsed; i++) {
                                BKNI_Printf("%02x ", *((uint8_t*)(info->pUserDataBuffer)+i)); /* it should be 0x038?04FF, where ? is the 3D-orientation  */
                            }
                            BKNI_Printf("\n");
#endif

                            if (rc || !bytesParsed) break;
                            NEXUS_VideoDecoder_P_Jp3dSignal_isr(videoDecoder, type);
                        }
                    }
                    else {
                        offset += bytesParsed;
                    }
                    continue; /* no more userdata to consume */
                }
            }
        }
        /* UDPlib takes pointer w/o size, so this must be true. Otherwise we have overflow, from which there is no recovery. */
        BDBG_ASSERT(ccCount <= B_MAX_VBI_CC_COUNT);
        BDBG_ASSERT(dssCcCount <= B_MAX_VBI_CC_COUNT);
        BDBG_ASSERT(dssSubtitleCount <= B_MAX_VBI_CC_COUNT);

        if (bytesParsed==0) { /* we aren't going anywhere */
            break;
        }
        offset += bytesParsed;
        /* We process bytesParsed even with error code. seems a bit dangerous. */
        if (rc == BERR_BUDP_PARSE_ERROR) {
            break;
        }
        else if (rc != BERR_SUCCESS) {
            continue;
        }

#if (defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BCM_TBD))
		if (videoDecoder->trickState.hostTrickModesEnabled || videoDecoder->trickState.brcmTrickModesEnabled) {
			/* in a host trick mode, we parse 3DTV signaling, but not closed caption.
			This is relatively harmless in a race condition, so don't propagate a 
			critical section on 'trickState' throughout. */
			continue;
		}
#endif

        for (i=0;i<ccCount;i++) {
            NEXUS_UserDataFormat nexusUserDataFormat;

            /* if preferredUserDataFormat is true, wait until that format is seen before using videoDecoder->userDataFormat for filtering */
            if (videoDecoder->settings.preferredUserDataFormat && !videoDecoder->useUserDataFormat) {
                if (IsEquivalentFormat(videoDecoder->userDataFormat, ccData[i].format)) {
                    videoDecoder->useUserDataFormat = true;
                }
            }
            else { /* otherwise, just use videoDecoder->userDataFormat */
                videoDecoder->useUserDataFormat = true;
            }

            switch (ccData[i].format) {
            case BUDP_DCCparse_Format_ATSC53: nexusUserDataFormat = NEXUS_UserDataFormat_eAtsc53; break;
            case BUDP_DCCparse_Format_DVS157: nexusUserDataFormat = NEXUS_UserDataFormat_eScte20; break;
            case BUDP_DCCparse_Format_DVS053: nexusUserDataFormat = NEXUS_UserDataFormat_eScte21; break;
            default: nexusUserDataFormat = NEXUS_UserDataFormat_eAny; break;
            }

            videoDecoder->userdata.status.formatParsed[nexusUserDataFormat]++;
            videoDecoder->userdata.status.lastFormatParsed = nexusUserDataFormat;

            switch (videoDecoder->useUserDataFormat ? videoDecoder->userDataFormat
                                                    : NEXUS_UserDataFormat_eAny) {
            case NEXUS_UserDataFormat_eScte20:
            case NEXUS_UserDataFormat_eScte21:
            case NEXUS_UserDataFormat_eAtsc53:
                if (nexusUserDataFormat != videoDecoder->userDataFormat) {
                    continue;
                }
                break;
            case NEXUS_UserDataFormat_eAny:
            default:
                if (videoDecoder->currentUserDataFormat == BUDP_DCCparse_Format_Unknown) {
                    /* Set it until the next Start. We never want to send >1 type of userdata to the VEC. */
                    videoDecoder->currentUserDataFormat = ccData[i].format;
                }
                else if (videoDecoder->currentUserDataFormat != ccData[i].format) {
                    continue;
                }
                break;
            }
            if (videoDecoder->userdata.vbiDataCallback_isr) {
                NEXUS_ClosedCaptionData data;

                BKNI_Memset(&data, 0, sizeof(data));
                data.type = NEXUS_VbiDataType_eClosedCaption;
                if (info->bPTSValid) {
                    data.pts = info->ui32PTS;
                }

                if (ccData[i].bIsAnalog) {
                    if (ccData[i].cc_valid) {
                        /* EIA-608 */
                        data.field = (ccData[i].polarity==BAVC_Polarity_eTopField)?0:1;
                        data.data[0] = ccData[i].cc_data_1;
                        data.data[1] = ccData[i].cc_data_2;
                        (*videoDecoder->userdata.vbiDataCallback_isr)(videoDecoder->userdata.vbiVideoInput, true, &data);
                    }
                    /* don't send invalid 608 data */
                }
                else {
                    /* EIA-708 */
                    data.field = ccData[i].seq.cc_type; /* can be any one of 0,1,2 or 3 */
                    data.data[0] = ccData[i].cc_data_1;
                    data.data[1] = ccData[i].cc_data_2;
                    data.noData = !ccData[i].cc_valid; /* must still send invalid 708 data */
                    (*videoDecoder->userdata.vbiDataCallback_isr)(videoDecoder->userdata.vbiVideoInput, false, &data);
                }
            }
        }

        /* DSS CC & subtitle has special processing */
        if (videoDecoder->userdata.vbiDataCallback_isr) {
            NEXUS_ClosedCaptionData data;

            for (i=0;i<dssCcCount;i++) {
                if (dssCcData[i].eCCType == BUDP_DCCparse_CC_Dss_Type_ClosedCaption) {
                    BKNI_Memset(&data, 0, sizeof(data));
                    data.type = NEXUS_VbiDataType_eClosedCaption;
                    if (info->bPTSValid) {
                        data.pts = info->ui32PTS;
                    }

                    if (dssCcData[i].bIsAnalog) {
                        /* EIA-608 */
                        if (dssCcData[i].cc_data_1 != 0x80 || dssCcData[i].cc_data_2 != 0x80) { /* discard null data */
                            data.field = (dssCcData[i].polarity==BAVC_Polarity_eTopField)?0:1;
                            data.data[0] = dssCcData[i].cc_data_1;
                            data.data[1] = dssCcData[i].cc_data_2;
                            (*videoDecoder->userdata.vbiDataCallback_isr)(videoDecoder->userdata.vbiVideoInput, true /*608*/, &data);
                        }
                    }
                    else {
                        /* EIA-708 */
                        data.field = dssCcData[i].language_type; /* this is cc_type */
                        data.data[0] = dssCcData[i].cc_data_1;
                        data.data[1] = dssCcData[i].cc_data_2;
                        (*videoDecoder->userdata.vbiDataCallback_isr)(videoDecoder->userdata.vbiVideoInput, false /*708*/, &data);
                    }
                }
            }
            for (i=0;i<dssSubtitleCount;i++) {
                if (dssSubtitle[i].eCCType == BUDP_DCCparse_CC_Dss_Type_Subtitle) {
                    BKNI_Memset(&data, 0, sizeof(data));
                    data.type = NEXUS_VbiDataType_eSubtitle;
                    if (info->bPTSValid) {
                        data.pts = info->ui32PTS;
                    }

                    data.field = dssSubtitle[i].language_type;
                    data.data[0] = dssSubtitle[i].cc_data_1;
                    data.data[1] = dssSubtitle[i].cc_data_2;
                    (*videoDecoder->userdata.vbiDataCallback_isr)(videoDecoder->userdata.vbiVideoInput, false /*not 608*/, &data);
                }
            }
        }

    }
    return;
}

void NEXUS_VideoDecoder_P_UserdataReady_isr(void *data, int unused, void *not_used)
{
    BERR_Code rc = BXVD_ERR_USERDATA_NONE;
    BAVC_USERDATA_info info;
    NEXUS_VideoDecoderHandle videoDecoder = (NEXUS_VideoDecoderHandle)data;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BSTD_UNUSED(unused);
    BSTD_UNUSED(not_used);

    for ( ;; )
    {
        /* TODO: reduce XVD userdata buffer size because we evacuate XVD at isr time */

        /* get data */
        rc = BXVD_Userdata_Read_isr(videoDecoder->userdata.handle, &info);
        if ( rc==BXVD_ERR_USERDATA_NONE ) {
            break;
        }
        else if ( rc==BXVD_ERR_USERDATA_INVALID ) {
            continue; /* keep reading data */
        }
        else if ( rc!=BERR_SUCCESS ) {
            BDBG_ERR(("BXVD_Userdata_Read_isr returned error %#x, ignore", rc));
            break;
        }

        if (videoDecoder->settings.userDataEnabled) {
            unsigned pad;
            NEXUS_UserDataHeader header;
            unsigned after_wrap_space, immediate_space, needed_space;

            /* add padding to payload to ensure efficient 32 bit aligned access of the header */
            pad = 4 - ((sizeof(header) + info.ui32UserDataBufSize) % 4);
            if (pad == 4) pad = 0;

            BDBG_CASSERT(BAVC_Polarity_eFrame == (BAVC_Polarity)NEXUS_PicturePolarity_eFrame);
            BDBG_CASSERT(BAVC_USERDATA_Type_eSlice == (BAVC_USERDATA_Type)NEXUS_UserDataType_eSlice);
            BDBG_CASSERT(BAVC_USERDATA_PictureCoding_eB == (BAVC_PictureCoding)NEXUS_PictureCoding_eB);

            header.blockSize = sizeof(header) + info.ui32UserDataBufSize + pad;
            header.polarity = info.eSourcePolarity;
            header.type = info.eUserDataType;
            header.topFieldFirst = info.bTopFieldFirst;
            header.repeatFirstField = info.bRepeatFirstField;
            header.ptsValid = info.bPTSValid;
            header.pts = info.ui32PTS;
            header.pictureCoding = info.ePicCodingType;
            BKNI_Memcpy(header.pictureCodingExtension, info.ui32PicCodExt, sizeof(header.pictureCodingExtension));
            header.payloadSize = info.ui32UserDataBufSize;

            /* userdata packets should be copied all or nothing. don't allow overflow to corrupt the header + payload format. */
            NEXUS_VideoDecoder_P_UserDataFreeSpace_isr(videoDecoder, &immediate_space, &after_wrap_space);
            needed_space = sizeof(header)+info.ui32UserDataBufSize+pad;
            if (needed_space <= immediate_space || needed_space <= after_wrap_space) {
                char padbytes[4] = {0,0,0,0};
                if (needed_space > immediate_space) {
                    /* skip the remaining space in the ring buffer so that header + payload + pad never wraps. */
                    NEXUS_VideoDecoder_P_AddUserData_isr(videoDecoder, NULL, immediate_space);
                }
                NEXUS_VideoDecoder_P_AddUserData_isr(videoDecoder, &header, sizeof(header));
                NEXUS_VideoDecoder_P_AddUserData_isr(videoDecoder, info.pUserDataBuffer, info.ui32UserDataBufSize);
                NEXUS_VideoDecoder_P_AddUserData_isr(videoDecoder, padbytes, pad);
                NEXUS_IsrCallback_Fire_isr(videoDecoder->userdataCallback);
            }
            else {
                /* do not write partial data. just discard. */
                BDBG_WRN(("userdata overflow"));
            }
        }

        /* parse and send data to VideoInput (in Display module) */
        if (videoDecoder->userdata.vbiDataCallback_isr || videoDecoder->extendedSettings.s3DTVStatusEnabled) {
            NEXUS_VideoDecoder_P_ParseUserdata_isr(videoDecoder, &info);
        }

        if (videoDecoder->displayConnection.userDataCallback_isr) {
            videoDecoder->displayConnection.userDataCallback_isr(videoDecoder->displayConnection.callbackContext, &info);
        }
    }
}

NEXUS_Error NEXUS_VideoDecoder_SetUserDataFormatFilter( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_UserDataFormat format )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    videoDecoder->userDataFormat = format;
    return 0;
}

NEXUS_Error NEXUS_VideoDecoder_GetUserDataStatus( NEXUS_VideoDecoderHandle videoDecoder, NEXUS_VideoDecoderUserDataStatus *pStatus )
{
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    *pStatus = videoDecoder->userdata.status;
    return 0;
}
