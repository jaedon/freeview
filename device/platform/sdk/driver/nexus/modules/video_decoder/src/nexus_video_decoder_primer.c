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
 * $brcm_Workfile: nexus_video_decoder_primer.c $
 * $brcm_Revision: 26 $
 * $brcm_Date: 6/19/12 12:44p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/video_decoder/7400/src/nexus_video_decoder_primer.c $
 * 
 * 26   6/19/12 12:44p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 25   6/13/12 3:43p erickson
 * SW7425-3170: refactor VideoDecoderPrimer api
 * 
 * 24   6/11/12 2:23p erickson
 * SW7425-3170: refactor VideoDecoderPrimer api
 * 
 * 23   6/8/12 2:18p erickson
 * SW7425-3170: promote NEXUS_VideoDecoderPrimerHandle to a tracked
 *  handle, refactor API with backward compatible macros
 * 
 * 22   6/7/12 3:18p jtna
 * SW7425-1040: don't call StopPrimer in ClosePrimer if already stopped
 * 
 * 21   4/27/12 11:47a jtna
 * SW7425-1070: wrap NEXUS_StcChannel_SetPcrOffsetContextAcquireMode_priv.
 *  fix DSS check.
 * 
 * 20   8/30/11 2:52p jtna
 * SW7425-1040: error out on multiple StopPrimer() calls. error out on
 *  StartDecodeWithPrimer() preceded by StopPrimer()
 * 
 * 19   8/24/11 5:31p jtna
 * SW7405-5461: fix MPEG1 case
 * 
 * 18   8/12/11 4:20p jtna
 * SW7425-1070: merge video primer ptsStcDiffCorrection feature
 * 
 * 17   9/29/10 3:18p jtna
 * SW7405-4723: remove unneeded primer profiling code
 * 
 * 16   9/14/10 12:12p jtna
 * SW7405-4723: better handling of AVC streams and cases where pcr_offset
 *  entry is NULL
 * 
 * 15   9/9/10 4:31p erickson
 * SW7468-129: pass open settings to NEXUS_VideoDecoder_P_GetRaveSettings
 *
 * 14   9/8/10 12:25p vsilyaev
 * SW7468-129: Added video decoder on ZSP
 *
 * SW7468-129/1   2/26/10 7:09p vsilyaev
 * SW7468-129: Added ZSP video decoder stub
 *
 * 13   8/23/10 3:10p jtna
 * SW7405-4723: added DSS support for new algo
 *
 * 12   8/23/10 2:49p jtna
 * SW7405-4723: merge new algo
 *
 * 11   1/26/10 4:51p jtna
 * SW7405-3753: increase MAX_GOPS and add optional messages
 *
 * 10   12/23/09 3:43p jtna
 * SW7405-3672: allow primer to be stopped then started again
 *
 * 9   10/23/09 4:24p jtna
 * SW7405-3265: protect against starting a started primer. fix overflow
 *  recovery code.
 *
 * 8   10/9/09 5:46p jtna
 * SW7405-3119: add options to control primer transition behavior
 *
 * 7   9/25/09 3:27p jtna
 * SWDEPRECATED-4001: add recovery code for CDB overflow
 *
 * 6   9/25/09 3:25p jtna
 * SWDEPRECATED-4001: initial support for AVC content
 *
 * 5   9/10/09 12:23p jtna
 * SW7405-3003: add video primer profiling
 *
 * 4   7/30/09 4:47p erickson
 * PR54880: replace picture count-based algo with simple TSM-algo
 *
 ***************************************************************************/
#include "nexus_video_decoder_module.h"
#include "nexus_memory.h"
#include "priv/nexus_core.h"
#include "priv/nexus_stc_channel_priv.h"
#include "bvlc.h"

BDBG_MODULE(nexus_video_decoder_primer);

#define ITB_SIZE 16
#define BDBG_MSG_TRACE(X) /* BDBG_MSG(X) */

struct NEXUS_VideoDecoderPrimer
{
    NEXUS_OBJECT(NEXUS_VideoDecoderPrimer);
    NEXUS_VideoDecoderHandle videoDecoder;
    NEXUS_VideoDecoderStartSettings startSettings;
    NEXUS_VideoDecoderPrimerSettings primerSettings;
    int div2ms; /* constant to convert PTS units to millisecond units. 45 for MPEG, 27000 for DSS */
    NEXUS_RaveHandle rave;
    NEXUS_HeapHandle heap; /* heap for this rave context */
    NEXUS_TimerHandle timer;

    BAVC_XptContextMap cx_map;
    uint32_t cdb_base;          /* copy of cdb base register */
    uint32_t itb_base;          /* copy of itb base register */
    bool active;
    int last_diff;
    bool overflow;

    /* read registers */
#define MAX_GOPS 24
    struct {
        uint32_t cdb_read;
        uint32_t itb_read;
        uint32_t pts;
        uint32_t pcr_offset;
        bool pcr_offset_set;

        /* needed for AVC content only. these are initialized to -1 */
        int sps_id;
        int pps_id;
    } gops[MAX_GOPS];
    unsigned next_gop; /* next_gop-1 is the last GOP seen by ITB */
    int consumed_gop; /* the GOP that we've consumed upto */

    /* shadow read registers */
    uint32_t sitb_read;

    /* last base_address ITB seen */
    uint32_t cdb_base_entry;    /* address in cdb pointed by last base_address ITB */
    uint32_t itb_base_entry;    /* address of base_address itb */
};

struct itb_entry_t {
    uint32_t word0;
    uint32_t word1;
    uint32_t word2;
    uint32_t word3;
};

#define NEXT_GOP_INDEX(i) ((i+1==MAX_GOPS) ? 0 : (i+1))
#define PREV_GOP_INDEX(i) ((i==0) ? (MAX_GOPS-1) : (i-1))

static void NEXUS_VideoDecoder_P_PrimerSetRave(NEXUS_VideoDecoderPrimerHandle vpcx)
{
    unsigned itbBackup;
    void *temp;
    uint32_t *itb;
    unsigned i;
    uint32_t itb_read;
    uint32_t serialStc, stc;
    int32_t diff, min_diff = 0x80000000;
    int gop_index = vpcx->consumed_gop;
    int last_gop_in_itb = (vpcx->next_gop==0)?MAX_GOPS-1:vpcx->next_gop-1;
    int decoderPtsOffset = 0;
    if (vpcx->primerSettings.ptsStcDiffCorrectionEnabled && vpcx->videoDecoder->settings.ptsOffset > 0) {
        decoderPtsOffset = vpcx->videoDecoder->settings.ptsOffset;
    }

    LOCK_TRANSPORT();
    NEXUS_StcChannel_GetSerialStc_priv(vpcx->startSettings.stcChannel, &serialStc);
    UNLOCK_TRANSPORT();

    if (vpcx->active) {
        uint32_t min_diff_next = 0xFFFFFFFF;
        int min_diff_next_idx = -1;

        for (i=0;i<MAX_GOPS;i++) {
            if (!vpcx->gops[i].itb_read) continue; /* no SQH or SPS found */
            if (!vpcx->gops[i].pcr_offset_set) continue; /* no pcr_offset entry found */

            stc = serialStc + vpcx->gops[i].pcr_offset; /* Serial STC + offset = STC */
            diff = vpcx->gops[i].pts + decoderPtsOffset - stc;

#if 0 /* debug print to monitor PTS-STC progression. enable with only 1 primer active */
            BDBG_MSG(("%p: [%2d] %8d, 0x%06x 0x%06x 0x%06x", vpcx, i, diff/vpcx->div2ms, vpcx->gops[i].pts + decoderPtsOffset, serialStc, vpcx->gops[i].pcr_offset));
#endif

            /* never consume a GOP in the future (PTS > STC). this comes at a slightly higher CDB size requirement
               than simply consuming to the nearest GOP */
            if (diff > 0) {
                if ((unsigned)diff < min_diff_next) {
                    min_diff_next = diff;
                    min_diff_next_idx = i;
                }
                continue;
            }

            /* diff is negative here, so min_diff will always be negative. we're looking for the closest one to the present */
            if (diff > min_diff) {
                min_diff = diff;
                gop_index = i;
            }
        }

        /* there is one exception to not consuming a GOP in the future: when all available GOPs are in the 
           in the future, then we have to consume the nearest one in the future. this is important for some streams 
           (e.g. AVC) where the PTS-PCR diff is several seconds long */
        if (gop_index==-1) {
            gop_index = min_diff_next_idx;
        }
    }
    else { /* last-second adjustment */
        bool done = false;
        uint32_t min_diff_past = 0xFFFFFFFF, min_diff_future = 0xFFFFFFFF;
        int min_diff_past_idx = vpcx->consumed_gop, min_diff_future_idx = vpcx->consumed_gop;
        unsigned pastLimit = vpcx->primerSettings.pastTolerance;
        unsigned futureLimit = vpcx->primerSettings.futureTolerance;
        int idx = vpcx->consumed_gop;
        vpcx->last_diff = 0;

        if (vpcx->consumed_gop==-1) return;

        BDBG_MSG(("%p:> SetRave GOPS %d:%d, tol %d:%d", vpcx, vpcx->consumed_gop, last_gop_in_itb,
            vpcx->primerSettings.pastTolerance, vpcx->primerSettings.futureTolerance));
        
        /* traverse through all GOPs in the buffer to determine the optimal GOP to use */
        while (!done) {
            if (idx==MAX_GOPS) idx=0;

            if ((vpcx->startSettings.codec==NEXUS_VideoCodec_eMpeg2 || vpcx->startSettings.codec==NEXUS_VideoCodec_eMpeg1) 
                && !vpcx->gops[idx].itb_read) {
                /* see if the most recent GOP is usable */
                if (vpcx->gops[PREV_GOP_INDEX(idx)].itb_read!=vpcx->itb_base_entry && vpcx->gops[idx].pcr_offset && vpcx->gops[idx].pts) {
                    done = true;
                }
                else {
                    break;
                }
            }
            else if (vpcx->startSettings.codec==NEXUS_VideoCodec_eH264 && idx==last_gop_in_itb) {
                /* AVC is more complicated (i.e. an SPS entry updates the current itb_read without wiping out 
                   the next itb_read). peeking ahead is not really an option */
                done = true;
            }
            
            stc = serialStc + vpcx->gops[idx].pcr_offset;
            diff = vpcx->gops[idx].pts + decoderPtsOffset - stc;

#if 0
            if (!vpcx->gops[idx].pcr_offset) {
                BDBG_MSG(("%p:  skip    GOP %2d, pcr_offset 0", vpcx, idx));
                idx++;
                continue;
            }
#endif
            BDBG_MSG(("%p:  test    GOP %2d, diff %7d:%5d", vpcx, idx, diff, diff/vpcx->div2ms));

            diff = diff/vpcx->div2ms;
            if (diff < 0) {
                diff *= -1;
                if ((unsigned)diff < min_diff_past) {
                    min_diff_past = diff;
                    min_diff_past_idx = idx;
                }
            }
            else {
                if ((unsigned)diff < min_diff_future) {
                    min_diff_future = diff;
                    min_diff_future_idx = idx;
                }
            }
            idx++;
        }

        /* if neither tolerance is satisfied */
        if ((min_diff_past>pastLimit) && (min_diff_future>futureLimit)) {
            /* then the public setting is taken as a preference */
            if (pastLimit > futureLimit) {
                gop_index = min_diff_past_idx;
            }
            else if (pastLimit < futureLimit) {
                gop_index = min_diff_future_idx;
            }
            else { /* if past == future, then select the nearest one */
                if (min_diff_past <= min_diff_future) {
                    gop_index = min_diff_past_idx;
                }
                else {
                    gop_index = min_diff_future_idx;
                }
            }
        }
        /* if both tolerances are satisfied */
        else if ((min_diff_past<=pastLimit) && (min_diff_future<=futureLimit)) {
            /* then select the nearest one */
            if (min_diff_past <= min_diff_future) {
                gop_index = min_diff_past_idx;
            }
            else {
                gop_index = min_diff_future_idx;
            }
        }
        /* if only one tolerance is satisfied */
        else {
            if (min_diff_past<=pastLimit) {
                gop_index = min_diff_past_idx;
            }
            else {
                gop_index = min_diff_future_idx;
            }
        }

        /* if advancing the readptrs */
        if (done && gop_index==NEXT_GOP_INDEX(last_gop_in_itb)) {
            BDBG_ASSERT(!vpcx->gops[gop_index].cdb_read);
            BDBG_ASSERT(!vpcx->gops[gop_index].itb_read);
            vpcx->gops[gop_index].cdb_read = vpcx->cdb_base_entry;
            vpcx->gops[gop_index].itb_read = vpcx->itb_base_entry;
        }

        stc = serialStc + vpcx->gops[gop_index].pcr_offset;
        min_diff = vpcx->gops[gop_index].pts + decoderPtsOffset - stc;
        BDBG_MSG(("%p:< SetRave GOP %2d, diff %7d:%5d", vpcx, gop_index, min_diff, min_diff/vpcx->div2ms));

        vpcx->last_diff = min_diff;
    }

    if (gop_index == -1 || gop_index == vpcx->consumed_gop) return;
    vpcx->consumed_gop = gop_index;

    /* AVD reads from the ITB in 128 byte blocks. So, we must back up to the previous 128 byte boundary and
    pad it with harmless ITB's */
    itb_read = vpcx->gops[gop_index].itb_read;
    itbBackup = itb_read % 128;
    if (itbBackup == 0) {
        if (itb_read == vpcx->itb_base) {
            /* TODO: rework the algo so that there's always room for adding a PCR_OFFSET entry */
            BDBG_ERR(("Unable to set pcr_offset"));
        }
        else {
            itbBackup = 128;
        }
    }
    itb_read -= itbBackup;
    (void)BMEM_ConvertOffsetToAddress(NEXUS_Heap_GetMemHandle(vpcx->heap), itb_read, &temp);
    itb = (uint32_t *)temp;
    for (i=0;i<itbBackup/ITB_SIZE;i++) {
        itb[0] = 0x22800000; /* pcr offset, marked valid */
        itb[1] = vpcx->gops[gop_index].pcr_offset;
        itb[2] = 0;
        itb[3] = 0;
        itb += 4;
    }

    BDBG_MSG_TRACE(("%p: SetRave GOP %d, PTS=%08x STC=%08x (%08x %08x), diff %5d, CDB %08x ITB %08x, add %d ITB leading pad", vpcx,
        gop_index, vpcx->gops[gop_index].pts, serialStc + vpcx->gops[gop_index].pcr_offset,
        serialStc, vpcx->gops[gop_index].pcr_offset, min_diff,
        vpcx->gops[gop_index].cdb_read, itb_read, itbBackup/ITB_SIZE));

    BREG_Write32(g_pCoreHandles->reg, vpcx->cx_map.CDB_Read, vpcx->gops[gop_index].cdb_read);
    BREG_Write32(g_pCoreHandles->reg, vpcx->cx_map.ITB_Read, itb_read);
}

static int NEXUS_VideoDecoder_P_PrimerGetCDBLevel(NEXUS_VideoDecoderPrimerHandle vpcx)
{
    uint32_t base, end, read, valid, wrap, diff;

    read  = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.CDB_Read);
    base  = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.CDB_Base);
    wrap  = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.CDB_Wrap);
    valid = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.CDB_Valid);
    end   = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.CDB_End);

    if (valid > read) {
        diff = valid - read;
    }
    else {
        diff = (valid - base) + (wrap - read);
    }
    return diff *100 / (end-base);
}

static void NEXUS_VideoDecoder_P_PrimerSetGOP(NEXUS_VideoDecoderPrimerHandle vpcx)
{
    /* we found a SQH or SPS, but no pcr_offset entry. see if we can copy the previous one */
    if (vpcx->gops[vpcx->next_gop].pcr_offset_set == false) {
        unsigned prev_gop = vpcx->next_gop ? vpcx->next_gop-1 : MAX_GOPS-1;
        if (vpcx->gops[prev_gop].pcr_offset_set) {
            vpcx->gops[vpcx->next_gop].pcr_offset = vpcx->gops[prev_gop].pcr_offset;
            vpcx->gops[vpcx->next_gop].pcr_offset_set = true;
            BDBG_MSG_TRACE(("%p: copying pcr_offset %#x from GOP %d", vpcx, vpcx->gops[prev_gop].pcr_offset, prev_gop));
        }
    }

    if (++vpcx->next_gop == MAX_GOPS) {
        vpcx->next_gop = 0;
    }
    if (vpcx->next_gop == (unsigned)(vpcx->consumed_gop)) {
        BDBG_WRN(("%p: MAX_GOPS overflow %d", vpcx, vpcx->next_gop)); /* GOPS are getting overwritten and MAX_GOPS needs to be increased */
    }
    vpcx->gops[vpcx->next_gop].itb_read = 0;
    vpcx->gops[vpcx->next_gop].cdb_read = 0;
    vpcx->gops[vpcx->next_gop].pcr_offset = 0;
    vpcx->gops[vpcx->next_gop].pcr_offset_set = false;
    vpcx->gops[vpcx->next_gop].pts = 0;
    if (vpcx->startSettings.codec == NEXUS_VideoCodec_eH264) {
        vpcx->gops[vpcx->next_gop].sps_id = -1;
        vpcx->gops[vpcx->next_gop].pps_id = -1;
    }
}

static void NEXUS_VideoDecoder_P_PrimerProcessAVC(NEXUS_VideoDecoderPrimerHandle vpcx, uint8_t nal_header, uint8_t offset)
{
    #define BDBG_MSG_TRACE_AVC_L1(X) /* BDBG_MSG(X) */ /* msg for SPS, PPS, SEI and IDR that are processed */
    #define BDBG_MSG_TRACE_AVC_L2(X) /* BDBG_MSG(X) */ /* msg for PPS, SEI and IDR that are ignored */

    uint8_t nal_unit_type = nal_header & 0x1f;
    BMEM_Handle mem;
    void *addr;
    uint32_t *temp_addr;
    uint8_t word_offset;
    uint32_t be_word[3], le_word[3];
    #define TOTAL_PAYLOAD 8
    uint8_t payload[TOTAL_PAYLOAD];
    unsigned index = 0, bit = 7;
    unsigned i;
    BERR_Code rc;

    #define TOTAL_SPS_ID 32
    #define TOTAL_PPS_ID 256

    if (nal_unit_type < 0x5 || nal_unit_type > 0x8) { /* we only care about 0x5 through 0x8: IDR, SEI, SPS, PPS */
        return;
    }

    if ((nal_header & 0x80) != 0) { /* forbidden_zero_bit */
        return;
    }

    mem = NEXUS_Heap_GetMemHandle(vpcx->heap);
    rc = BMEM_ConvertOffsetToAddress(mem, vpcx->cdb_base_entry, &addr);
    if (rc) {
        BDBG_ERR(("bad cdb_base_entry read=%08x", vpcx->cdb_base_entry));
        rc = BERR_TRACE(rc);
        return;
    }
    
    addr = (uint8_t*)addr + offset + 1; /* addr points to start of AVC payload in CDB. +1 since we're currently at the nal_header */

    word_offset = (uint32_t)addr%4; /* number of bytes to nearest previous word alignment */
    temp_addr = (uint32_t*)((uint8_t*)addr - word_offset);

    /* grab the payload in BE */
    be_word[0] = *temp_addr;
    be_word[1] = *(temp_addr+1);
    be_word[2] = *(temp_addr+2);

    /* BE -> LE swap */
    for (i=0; i<3; i++) {
        le_word[i] =
            ((be_word[i] & 0xFF000000) >> 24) |
            ((be_word[i] & 0x00FF0000) >> 8) |
            ((be_word[i] & 0x0000FF00) << 8) |
            ((be_word[i] & 0x000000FF) << 24);
    }

    /* setup the payload for VLC decode */
    for (i=0; i<TOTAL_PAYLOAD; i++) {
        payload[i] = *((uint8_t*)le_word + word_offset + i);
    }

#if 0 /* BE->LE debug */
    BDBG_MSG(("nal_header 0x%02x", nal_header));
    BDBG_MSG(("be_word %08x %08x %08x", be_word[0], be_word[1], be_word[2]));
    BDBG_MSG(("le_word %08x %08x %08x", le_word[0], le_word[1], le_word[2]));
    BDBG_MSG(("payload %02x %02x %02x %02x %02x %02x %02x %02x",
        payload[0], payload[1], payload[2], payload[3], payload[4], payload[5], payload[6], payload[7]));
#endif

    switch (nal_unit_type) {
        case 7: /* SPS */
        {
            int sps_id;
            index = 3; /* skip the first 3 bytes, corresponding to profile_idc, constraint_flags and level_idc */

            sps_id = b_vlc_decode(payload, TOTAL_PAYLOAD, index, bit, &index, &bit);

            if (vpcx->gops[vpcx->next_gop].sps_id != -1) {
                if (vpcx->gops[vpcx->next_gop].sps_id != sps_id) {
                    BDBG_MSG(("%p: GOP%d, new SPS %d seen before IDR/SEI", vpcx, vpcx->next_gop, sps_id));
                    vpcx->gops[vpcx->next_gop].pps_id = -1; /* invalidate PPS */
                }
            }

            if (sps_id < TOTAL_SPS_ID) {
                vpcx->gops[vpcx->next_gop].sps_id = sps_id;

                /* RAP is at the SPS, not at the IDR */
                vpcx->gops[vpcx->next_gop].cdb_read = vpcx->cdb_base_entry;
                vpcx->gops[vpcx->next_gop].itb_read = vpcx->itb_base_entry;
                BDBG_MSG_TRACE_AVC_L1(("%p: GOP%d, SPS %d (CDB %08x)", vpcx, vpcx->next_gop, sps_id, vpcx->gops[vpcx->next_gop].cdb_read));
            }

            BDBG_MSG_TRACE_AVC_L2(("%p: CDB level %d%%", vpcx, NEXUS_VideoDecoder_P_PrimerGetCDBLevel(vpcx)));
            BSTD_UNUSED(NEXUS_VideoDecoder_P_PrimerGetCDBLevel);
        }
            break;

        case 8: /* PPS */
        {
            int pps_id, sps_id;

            pps_id = b_vlc_decode(payload, TOTAL_PAYLOAD, index, bit, &index, &bit);
            sps_id = b_vlc_decode(payload, TOTAL_PAYLOAD, index, bit, &index, &bit);

            if (sps_id == vpcx->gops[vpcx->next_gop].sps_id) {
                if (pps_id < TOTAL_PPS_ID) {
                    vpcx->gops[vpcx->next_gop].pps_id = pps_id;
                    BDBG_MSG_TRACE_AVC_L1(("%p: GOP%d, PPS %d (SPS %d)", vpcx, vpcx->next_gop, pps_id, sps_id));
                }
            }
            else {
                BDBG_MSG_TRACE_AVC_L2(("%p: GOP%d, PPS %d seen with SPS %d (!= current SPS %d)", vpcx, vpcx->next_gop, pps_id, sps_id,
                    vpcx->gops[vpcx->next_gop].sps_id));
            }
        }
            break;

        case 5: /* IDR slice */
        {
            int first_mb_in_slice, slice_type, pps_id;

            first_mb_in_slice = b_vlc_decode(payload, TOTAL_PAYLOAD, index, bit, &index, &bit);
            slice_type = b_vlc_decode(payload, TOTAL_PAYLOAD, index, bit, &index, &bit);
            pps_id = b_vlc_decode(payload, TOTAL_PAYLOAD, index, bit, &index, &bit);

            if (pps_id == vpcx->gops[vpcx->next_gop].pps_id) {
                if (first_mb_in_slice == 0) { /* true for an IDR that immediately follows an SPS and PPS */
                    BDBG_MSG_TRACE_AVC_L1(("%p: GOP%d, IDR", vpcx, vpcx->next_gop));
                    NEXUS_VideoDecoder_P_PrimerSetGOP(vpcx);
                    NEXUS_VideoDecoder_P_PrimerSetRave(vpcx);
                }
            }
            else {
                if (vpcx->gops[vpcx->next_gop].sps_id != -1) { /* IDRs between an SPS and a PPS */
                    BDBG_MSG_TRACE_AVC_L2(("%p: GOP%d, IDR seen with PPS %d (!= current PPS %d)", vpcx, vpcx->next_gop, pps_id,
                        vpcx->gops[vpcx->next_gop].pps_id));
                }
            }
        }
            break;

        case 6: /* SEI */
        {
            unsigned last_payload_type_byte, last_payload_size_byte;

            do { /* there can be multiple SEIs in a single NAL */

                if (index > TOTAL_PAYLOAD-2) {
                    /* consider increasing TOTAL_PAYLOAD; we could be missing recovery point messages */
                    BDBG_WRN(("%p: GOP%d, insufficient NAL payload during SEI parsing", vpcx, vpcx->next_gop));
                    BERR_TRACE(-1);
                    break;
                }

                bit = 7;
                last_payload_type_byte = payload[index++];
                last_payload_size_byte = payload[index++];

                if (last_payload_type_byte != 0x6) {
                    index += last_payload_size_byte;
                }
                else { /* recovery point message */

                    /* AVD FW doesn't care about the 4 fields inside the message.
                       The mere presence is a sufficient RAP */
                    if (vpcx->gops[vpcx->next_gop].sps_id != -1 && vpcx->gops[vpcx->next_gop].pps_id != -1) {
                        BDBG_MSG_TRACE_AVC_L1(("%p: GOP%d, SEI recovery point", vpcx, vpcx->next_gop));
                        NEXUS_VideoDecoder_P_PrimerSetGOP(vpcx);
                        NEXUS_VideoDecoder_P_PrimerSetRave(vpcx);
                    }
                    else {
                        BDBG_MSG_TRACE_AVC_L2(("%p: GOP%d, SEI recovery point without SPS(%d) or PPS(%d)", vpcx, vpcx->next_gop,
                            vpcx->gops[vpcx->next_gop].sps_id, vpcx->gops[vpcx->next_gop].pps_id));
                    }
                }

            } while (index<TOTAL_PAYLOAD && payload[index]!=0x80);
        }
            break;

    }
}

static void NEXUS_VideoDecoder_P_PrimerProcessItb(NEXUS_VideoDecoderPrimerHandle vpcx, uint32_t itb_valid)
{
    NEXUS_Error rc;
    struct itb_entry_t * pitb;
    struct itb_entry_t * pitb_end;
    uint8_t type;
    uint32_t * pword;
    uint32_t word = 0;
    uint8_t sc;
    uint8_t offset;
    uint32_t count;
    BMEM_Handle mem;
    void *temp;

    BDBG_OBJECT_ASSERT(vpcx, NEXUS_VideoDecoderPrimer);

    mem = NEXUS_Heap_GetMemHandle(vpcx->heap);
    rc = BMEM_ConvertOffsetToAddress(mem, vpcx->sitb_read, &temp);
    if (rc) {
        BDBG_ERR(("bad sitb_read=%08x", vpcx->sitb_read));
        rc = BERR_TRACE(rc);
        return;
    }
    pitb = temp;
    rc = BMEM_ConvertOffsetToAddress(mem, itb_valid, &temp);
    if (rc) {
        BDBG_ERR(("bad itb_valid=%08x", itb_valid));
        rc = BERR_TRACE(rc);
        BKNI_Fail();
        return;
    }
    pitb_end = temp;

    while (pitb < pitb_end) {
        BDBG_MSG_TRACE(("%p: %08x %08x %08x %08x", vpcx, pitb->word0, pitb->word1, pitb->word2, pitb->word3));
        /* determine entry type */
        type = pitb->word0 >> 24;
        switch(type)
        {
        case 0: /* start code entry */
            pword = &pitb->word1;
            count = 0;
            do {
                if(0 == (count & 1)){
                    word = *pword;
                    pword++;
                }
                sc = word >> 24;
                offset = (word >> 16) & 0xff;
                if(0xff == offset){
                    break;
                }
                switch(sc){
                case 0xb3: /* sequence header */
                    /* BDBG_WRN(("%p: GOP%d, SQH, PTS %08x, CDB read %08x, ITB read %08x", vpcx, vpcx->next_gop, vpcx->gops[vpcx->next_gop].pts, vpcx->cdb_base_entry, vpcx->itb_base_entry)); */
                    vpcx->gops[vpcx->next_gop].cdb_read = vpcx->cdb_base_entry;
                    vpcx->gops[vpcx->next_gop].itb_read = vpcx->itb_base_entry;

                    NEXUS_VideoDecoder_P_PrimerSetGOP(vpcx);
                    NEXUS_VideoDecoder_P_PrimerSetRave(vpcx);
                    break;

                default:
                    if (vpcx->startSettings.codec == NEXUS_VideoCodec_eH264) {
                        uint8_t nal_unit_type = sc & 0x1f;
                        if (nal_unit_type >= 0x5 && nal_unit_type <= 0x8) {
                            NEXUS_VideoDecoder_P_PrimerProcessAVC(vpcx, sc, offset);
                        }
                    }
                    break;
                }
                word <<= 16;
                count++;
            }while(count < 6);
            break;

        case 0x22: /* pcr offset */
            if (pitb->word0 & 0x800000) {
                vpcx->gops[vpcx->next_gop].pcr_offset = pitb->word1;
                vpcx->gops[vpcx->next_gop].pcr_offset_set = true;
                /* BDBG_WRN(("%p: GOP%d, pcr offset %08x", vpcx, vpcx->next_gop, vpcx->gops[vpcx->next_gop].pcr_offset)); */
            }
            break;

        case 0x21: /* pts */
            vpcx->gops[vpcx->next_gop].pts = pitb->word1;
            /* BDBG_WRN(("%p: GOP%d, pts %08x", vpcx, vpcx->next_gop, vpcx->gops[vpcx->next_gop].pts)); */
            break;

        case 0x20: /* base_address  */
#if 0 /* TS random access indicator. disabled for now until we actually need this */
            int RAI = (pitb->word2 & 0x40) >> 7;
#endif
            vpcx->cdb_base_entry = pitb->word1;
            rc = BMEM_ConvertAddressToOffset(mem, pitb, &vpcx->itb_base_entry);
            if (rc) {rc = BERR_TRACE(rc); return;}

            /* BDBG_WRN(("%p: GOP%d cdb_base %08x itb_base %08x", vpcx, vpcx->next_gop, vpcx->cdb_base_entry, vpcx->itb_base_entry)); */
            if(0 != (0x30000 & pitb->word2)){
                BDBG_ERR(("overflow %p", vpcx));
                vpcx->overflow = true;

#if 0 /* TODO: this is untested. it may be preferrable to leave an overflow unhandled */
                LOCK_TRANSPORT();
                NEXUS_Rave_Disable_priv(vpcx->rave);
                NEXUS_Rave_Flush_priv(vpcx->rave);

                if (vpcx->startSettings.stcChannel) {
                    NEXUS_StcChannel_EnablePidChannel_priv(vpcx->startSettings.stcChannel, vpcx->startSettings.pidChannel);
                }
                UNLOCK_TRANSPORT();

                vpcx->cdb_base = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.CDB_Base);
                vpcx->itb_base = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.ITB_Base);
                vpcx->sitb_read = vpcx->itb_base;

                LOCK_TRANSPORT();
                NEXUS_Rave_Enable_priv(vpcx->rave);
                UNLOCK_TRANSPORT();
                return;
#endif
            }
            break;
        }
        pitb++;
    }

    rc = BMEM_ConvertAddressToOffset(mem, pitb, &vpcx->sitb_read);
    if (rc) {rc = BERR_TRACE(rc); return;}
}


static void NEXUS_VideoDecoder_P_PrimerCallback(void *context)
{
    NEXUS_VideoDecoderPrimerHandle vpcx = context;
    uint32_t itb_valid, itb_base, itb_wrap;

    /* convert ITB_Valid and ITB_Wrap to standard ring buffer semantics in this function. instead of pointing to the last byte of the ITB it should point
    to the first byte of the next ITB. Note that RAVE has an exception when valid == base. */
    itb_valid = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.ITB_Valid);
    itb_base = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.ITB_Base);
    if (itb_valid != itb_base) itb_valid++;
    /* check for itb wrap around and handle it */
    if (itb_valid < vpcx->sitb_read) {
        /* for this to be true, the HW must have wrapped. */
        itb_wrap = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.ITB_Wrap);
        BDBG_ASSERT(itb_wrap);
        itb_wrap++;
        NEXUS_VideoDecoder_P_PrimerProcessItb(vpcx, itb_wrap);
        /* if we consumed up to wrap point then wrap */
        if(vpcx->sitb_read >= itb_wrap){
            vpcx->sitb_read = vpcx->itb_base;
            NEXUS_VideoDecoder_P_PrimerProcessItb(vpcx, itb_valid);
        }
    }
    else {
        NEXUS_VideoDecoder_P_PrimerProcessItb(vpcx, itb_valid);
    }

    if (vpcx->active) {
    vpcx->timer = NEXUS_ScheduleTimer(30, NEXUS_VideoDecoder_P_PrimerCallback, vpcx);
}
}

#if 0
static void NEXUS_VideoDecoder_P_PrintItb2(NEXUS_VideoDecoderPrimerHandle vpcx, uint32_t from, uint32_t to)
{
    struct itb_entry_t *from_itb;
    struct itb_entry_t *to_itb;
    void *temp;
    BERR_Code rc;
    unsigned count;

    BDBG_OBJECT_ASSERT(vpcx, NEXUS_VideoDecoderPrimer);

    rc = BMEM_ConvertOffsetToAddress(NEXUS_Heap_GetMemHandle(vpcx->heap), from, &temp);
    BDBG_ASSERT(!rc);
    from_itb = temp;
    rc = BMEM_ConvertOffsetToAddress(NEXUS_Heap_GetMemHandle(vpcx->heap), to, &temp);
    BDBG_ASSERT(!rc);
    to_itb = temp;
    BDBG_ASSERT(from_itb <= to_itb);
    count = 0;
    while (from_itb < to_itb) {
        BDBG_WRN(("primer: %p ITB %p: %08x %08x %08x %08x", vpcx, from_itb, from_itb->word0, from_itb->word1, from_itb->word2, from_itb->word3));
        from_itb++;
        if (++count == 20) break;
    }
}

static void NEXUS_VideoDecoder_P_PrintItb(NEXUS_VideoDecoderPrimerHandle vpcx)
{
    uint32_t itb_valid, itb_read;
    itb_valid = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.ITB_Valid);
    itb_read = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.ITB_Read);
    if (itb_valid < itb_read) {
        uint32_t itb_wrap, itb_base;
        itb_wrap = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.ITB_Wrap);
        itb_base = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.ITB_Base);
        BDBG_WRN(("PrintItb %08x->%08x, %08x->%08x", itb_read, itb_wrap, itb_base, itb_valid));
        NEXUS_VideoDecoder_P_PrintItb2(vpcx, itb_read, itb_wrap);
        NEXUS_VideoDecoder_P_PrintItb2(vpcx, itb_base, itb_valid);
    }
    else {
        BDBG_WRN(("PrintItb %08x->%08x", itb_read, itb_valid));
        NEXUS_VideoDecoder_P_PrintItb2(vpcx, itb_read, itb_valid);
    }
}

static void NEXUS_VideoDecoder_P_DumpRegisters(NEXUS_VideoDecoderPrimerHandle vpcx)
{
    uint32_t valid, read, end, base;

    valid = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.ITB_Valid);
    read = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.ITB_Read);
    base = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.ITB_Base);
    end = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.ITB_End);
    BDBG_WRN(("ITB: valid=%08x read=%08x base=%08x end=%08x", valid, read, base, end));

    valid = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.CDB_Valid);
    read = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.CDB_Read);
    base = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.CDB_Base);
    end = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.CDB_End);
    BDBG_WRN(("CDB: valid=%08x read=%08x base=%08x end=%08x", valid, read, base, end));
}

static void NEXUS_VideoDecoder_P_DumpData(NEXUS_VideoDecoderPrimerHandle vpcx)
{
    uint32_t itb_read, itb_valid;
    uint8_t *fromptr;
    void *temp;
    BERR_Code rc;
    unsigned i, j;

    for (i=0;i<2;i++) {
        if (i == 0) {
            itb_read = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.ITB_Read);
            itb_valid = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.ITB_Valid);
            BKNI_Printf("ITB read=%08x (depth=%d):\n", itb_read, itb_valid - itb_read);
        }
        else {
            itb_read = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.CDB_Read);
            itb_valid = BREG_Read32(g_pCoreHandles->reg, vpcx->cx_map.CDB_Valid);
            BKNI_Printf("CDB read=%08x (depth=%d):\n", itb_read, itb_valid - itb_read);
        }

        if (itb_valid < itb_read) {
            return;
        }

        rc = BMEM_ConvertOffsetToAddress(NEXUS_Heap_GetMemHandle(vpcx->heap), itb_read, &temp);
        BDBG_ASSERT(!rc);
        fromptr = temp;
        for (j=0;j<64;j++) {
            uint8_t *ptr = &fromptr[j];
            ptr = ptr - (unsigned)ptr%4 + (3-(unsigned)ptr%4);
            BKNI_Printf("%02x ", *ptr); /* host endian/little endian, ITB */
            if ((j+1)%ITB_SIZE == 0) BKNI_Printf("\n");
        }
    }
}
#endif

NEXUS_VideoDecoderPrimerHandle NEXUS_VideoDecoderPrimer_P_Open_Avd( NEXUS_VideoDecoderHandle videoDecoder )
{
    NEXUS_VideoDecoderPrimerHandle primer;
    NEXUS_RaveOpenSettings raveOpenSettings;
    NEXUS_Error rc = 0;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    primer = BKNI_Malloc(sizeof(*primer));
    if (!primer) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }

    NEXUS_OBJECT_INIT(NEXUS_VideoDecoderPrimer, primer);
    primer->videoDecoder = videoDecoder;

    NEXUS_VideoDecoder_P_GetRaveSettings(videoDecoder, &raveOpenSettings, &videoDecoder->openSettings);
    raveOpenSettings.config.UsePictureCounter = true;
    LOCK_TRANSPORT();
    primer->rave = NEXUS_Rave_Open_priv(&raveOpenSettings);
    if (!primer->rave) {
        rc = BERR_TRACE(NEXUS_UNKNOWN);
    }
    UNLOCK_TRANSPORT();
    if (rc) goto error;

    return primer;

error:
    NEXUS_VideoDecoderPrimer_Close(primer);
    return NULL;
}

static void NEXUS_VideoDecoderPrimer_P_Finalizer( NEXUS_VideoDecoderPrimerHandle primer )
{
    NEXUS_VideoDecoderHandle videoDecoder;
    
    NEXUS_OBJECT_ASSERT(NEXUS_VideoDecoderPrimer, primer);
    videoDecoder = primer->videoDecoder;
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    if (videoDecoder->started && videoDecoder->rave == primer->rave) {
        NEXUS_VideoDecoder_Stop(videoDecoder);
    }
    else if (primer->active) {
        NEXUS_VideoDecoder_StopPrimer(videoDecoder, primer);
    }

    if (primer->rave) {
        LOCK_TRANSPORT();
        NEXUS_Rave_Close_priv(primer->rave);
        UNLOCK_TRANSPORT();
    }

    NEXUS_OBJECT_DESTROY(NEXUS_VideoDecoderPrimer, primer);
    BKNI_Free(primer);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_VideoDecoderPrimer, NEXUS_VideoDecoderPrimer_P_Close_Avd);

NEXUS_Error NEXUS_VideoDecoderPrimer_P_Start_Avd( NEXUS_VideoDecoderPrimerHandle primer, const NEXUS_VideoDecoderStartSettings *pStartSettings )
{
    NEXUS_RaveSettings raveSettings;
    NEXUS_RaveStatus raveStatus;
    NEXUS_Error rc = 0;
    NEXUS_PidChannelStatus pidChannelStatus;
    NEXUS_VideoDecoderHandle videoDecoder;

    BDBG_OBJECT_ASSERT(primer, NEXUS_VideoDecoderPrimer);
    videoDecoder = primer->videoDecoder;
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);

    if (primer->active) { /* primer already started */
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (videoDecoder->started && videoDecoder->rave == primer->rave) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    rc = NEXUS_PidChannel_GetStatus(pStartSettings->pidChannel, &pidChannelStatus);
    if (rc) return BERR_TRACE(rc);

    primer->startSettings = *pStartSettings;
    primer->div2ms = NEXUS_IS_DSS_MODE(pidChannelStatus.transportType) ? 27000 : 45;

    LOCK_TRANSPORT();
    NEXUS_Rave_GetDefaultSettings_priv(&raveSettings);
    raveSettings.pidChannel = pStartSettings->pidChannel;
    raveSettings.bandHold = false; /* should not be true when live, should not be true when IP */
    raveSettings.continuityCountEnabled = !pidChannelStatus.playback;
    rc = NEXUS_Rave_ConfigureVideo_priv(primer->rave, pStartSettings->codec, &raveSettings);
    if (!rc) {
        rc = NEXUS_Rave_GetStatus_priv(primer->rave, &raveStatus);
    }
    if (!rc) {
        NEXUS_Rave_Disable_priv(primer->rave);
        NEXUS_Rave_Flush_priv(primer->rave);
    }

    if (pStartSettings->stcChannel) {
        NEXUS_StcChannel_EnablePidChannel_priv(pStartSettings->stcChannel, pStartSettings->pidChannel);
    }

    UNLOCK_TRANSPORT();
    if (rc) return BERR_TRACE(rc);

    primer->heap = g_pCoreHandles->nexusHeap[0]; /* This is the heap used for RAVE access */
    primer->cx_map = raveStatus.xptContextMap;
    primer->cdb_base = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.CDB_Base);
    primer->itb_base = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Base);
    primer->sitb_read = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Read);
    primer->cdb_base_entry = 0;
    primer->itb_base_entry = 0;
    primer->active = true;
    primer->overflow = false;
    BKNI_Memset(primer->gops, 0, sizeof(primer->gops));
    primer->next_gop = 0;
    primer->consumed_gop = -1;
    primer->gops[primer->next_gop].sps_id = -1;
    primer->gops[primer->next_gop].pps_id = -1;

    primer->timer = NEXUS_ScheduleTimer(30, NEXUS_VideoDecoder_P_PrimerCallback, primer);
    if (!primer->timer) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }
    NEXUS_PidChannel_ConsumerStarted(pStartSettings->pidChannel); /* needed to unpause playback & stuff like that */

    LOCK_TRANSPORT();
    NEXUS_Rave_Enable_priv(primer->rave);
    if (pStartSettings->stcChannel) {
        /* set the pcr offset context into acquire mode, so that we get at least one pcr offset entry in the ITB */
        NEXUS_StcChannel_SetPcrOffsetContextAcquireMode_priv(primer->startSettings.stcChannel);
    }
    UNLOCK_TRANSPORT();

    return 0;
}

static void NEXUS_VideoDecoderPrimer_Stop_priv( NEXUS_VideoDecoderPrimerHandle primer )
{
    BDBG_OBJECT_ASSERT(primer, NEXUS_VideoDecoderPrimer);
    primer->active = false;
    if (primer->timer) {
        NEXUS_CancelTimer(primer->timer);
        primer->timer = NULL;
    }
}

void NEXUS_VideoDecoderPrimer_P_Stop_Avd( NEXUS_VideoDecoderPrimerHandle primer )
{
    NEXUS_VideoDecoderHandle videoDecoder;
    
    BDBG_OBJECT_ASSERT(primer, NEXUS_VideoDecoderPrimer);
    videoDecoder = primer->videoDecoder;
    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    if (primer->active==false) {
        BDBG_ERR(("%p: Primer already stopped", primer));
        return;
    }

    BDBG_MSG(("%p: Stop", primer));

    primer->active = false;
#if 0
    NEXUS_VideoDecoder_P_PrintItb(primer);
    NEXUS_VideoDecoder_P_DumpData(primer);
#endif

    /* Rave must be disabled in order for StartPrimer to work correctly later on */
    LOCK_TRANSPORT();
    NEXUS_Rave_Disable_priv(primer->rave);
    NEXUS_Rave_RemovePidChannel_priv(primer->rave); /* this is required for StartPrimer with a different pid */
    NEXUS_Rave_Flush_priv(primer->rave);
    UNLOCK_TRANSPORT();

    if (primer->timer) {
        NEXUS_CancelTimer(primer->timer);
        primer->timer = NULL;
    }
}

NEXUS_Error NEXUS_VideoDecoderPrimer_P_StopPrimerAndStartDecode_Avd( NEXUS_VideoDecoderPrimerHandle primer, NEXUS_VideoDecoderHandle videoDecoder )
{
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(videoDecoder, NEXUS_VideoDecoder);
    BDBG_OBJECT_ASSERT(primer, NEXUS_VideoDecoderPrimer);
    if (videoDecoder != primer->videoDecoder) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (videoDecoder->started) {
        BDBG_ERR(("VideoDecoder already started"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    if (primer->active==false) {
        BDBG_ERR(("%p: cannot StartDecodeWithPrimer on stopped primer", primer));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    BDBG_ASSERT(!videoDecoder->savedRave);
    videoDecoder->savedRave = videoDecoder->rave;
    videoDecoder->rave = primer->rave;

    NEXUS_VideoDecoderPrimer_Stop_priv(primer);

    /* last-second adjustment */
    NEXUS_VideoDecoder_P_PrimerCallback(primer);
    NEXUS_VideoDecoder_P_PrimerSetRave(primer);

    if (primer->consumed_gop < 0) {
        BDBG_MSG(("%p: StartDecodeWithPrimer without GOP", primer)); /* no previous RAP found */
    }

    if (primer->last_diff < 0 && primer->primerSettings.ptsStcDiffCorrectionEnabled && primer->overflow == false) {
        videoDecoder->primerPtsOffset = -primer->last_diff;
        BDBG_MSG(("%p: applying pts offset %u", primer, videoDecoder->primerPtsOffset));
    }
    else {
        videoDecoder->primerPtsOffset = 0;
    }

    rc = NEXUS_VideoDecoder_Start(videoDecoder, &primer->startSettings);
    if (rc!=NEXUS_SUCCESS) {
        videoDecoder->rave = videoDecoder->savedRave;
        videoDecoder->savedRave = NULL;
        return BERR_TRACE(rc);
    }

    return 0;
}

void NEXUS_VideoDecoderPrimer_GetSettings(NEXUS_VideoDecoderPrimerHandle primer, NEXUS_VideoDecoderPrimerSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(primer, NEXUS_VideoDecoderPrimer);
    *pSettings = primer->primerSettings;
}

NEXUS_Error NEXUS_VideoDecoderPrimer_SetSettings(NEXUS_VideoDecoderPrimerHandle primer, const NEXUS_VideoDecoderPrimerSettings *pSettings)
{
    BDBG_OBJECT_ASSERT(primer, NEXUS_VideoDecoderPrimer);
    primer->primerSettings = *pSettings;
    return 0;
}

NEXUS_VideoDecoderHandle nexus_videodecoderprimer_p_getdecoder(NEXUS_VideoDecoderPrimerHandle primer)
{
    BDBG_OBJECT_ASSERT(primer, NEXUS_VideoDecoderPrimer);
    BDBG_OBJECT_ASSERT(primer->videoDecoder, NEXUS_VideoDecoder);
    return primer->videoDecoder;
}
