/***************************************************************************
 *     (c)2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_audio_decoder_primer.c $
 * $brcm_Revision: 3 $
 * $brcm_Date: 10/4/12 10:02a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/modules/audio/src/common/nexus_audio_decoder_primer.c $
 * 
 * 3   10/4/12 10:02a erickson
 * SW7346-1027: enable primer, add ACQUIRE/RELEASE, flush on PTS discont,
 *  stop on close, use BERR_TRACE is NOT_SUPPORTED
 * 
 * 2   7/30/12 4:35p jgarrett
 * SW7435-266: Switching to unified audio module structure
 * 
 * 1   6/22/12 10:20a mward
 * SW7425-2741: add AudioDecoderPrimer (stubs, see SW7400-3079)
 * 
 * 2   6/19/12 5:42p vsilyaev
 * SW7420-2085: Converted from BDBG_OBJECT to NEXUS_OBJECT
 * 
 * 1   6/14/12 3:17p erickson
 * SW7425-2741: add AudioDecoderPrimer
 * 
 * 1   6/14/12 2:35p erickson
 * SW7425-2741: add AudioDecoderPrimer
 * 
 * SW7425-2741/3   6/14/12 9:21a ahulse
 * SW7425-2741: set bandhold to prevent overflow
 * 
 * SW7425-2741/2   6/8/12 2:42p ahulse
 * SW7425-2741: re-factor function names to make them more consistent
 * 
 * SW7425-2741/1   6/7/12 7:18p ahulse
 * SW7425-2741: add audio primer
 * 
 **************************************************************************/
#include "nexus_audio_module.h"
#include "priv/nexus_pid_channel_priv.h"

BDBG_MODULE(nexus_audio_decoder_primer);

#if BCHP_CHIP == 7400
#define NEXUS_AUDIO_DECODER_PRIMER_SUPPORTED 0 /* This feature is not working on 7400.  Enable and debug if required. */
#else
/* TODO: was this intended to be an option? */
#define NEXUS_AUDIO_DECODER_PRIMER_SUPPORTED 1
#endif

#ifdef DEBUG_PRIMER
#define BDBG_MSG_TRACE(X) BDBG_MSG(X) 
#else
#define BDBG_MSG_TRACE(X) /* BDBG_MSG(X) */
#endif

#define TIMER_INTERVAL 30 /* mSec */

struct NEXUS_AudioDecoderPrimer
{
    NEXUS_OBJECT(NEXUS_AudioDecoderPrimer);
    NEXUS_AudioDecoderHandle audioDecoder;
    NEXUS_AudioDecoderStartSettings startSettings;
    NEXUS_AudioDecoderPrimerSettings settings;
    int div2ms; /* constant to convert PTS units to millisecond units. 45 for MPEG, 27000 for DSS */
    NEXUS_RaveHandle rave;
    NEXUS_HeapHandle heap; /* heap for this rave context */
    NEXUS_TimerHandle timer;
    
    BAVC_XptContextMap cx_map;
    bool active;
    bool overflow;
    unsigned itb_base;
    uint32_t sitb_read; /* shadow read register */
    bool playback; /* true for playback TSM, not playback feed */
    
    /* audio doesn't have a "GOP", but we reuse the term to stay in sync with video primer. 
    in this case, "GOP" is the set of audio frames delimited by a coded PTS. */
#define MAX_GOPS 100
    struct {
        uint32_t cdb_read;
        uint32_t itb_read;
        uint32_t pts;
        uint32_t pcr_offset;
    } gops[MAX_GOPS];
    unsigned next_gop; /* next_gop-1 is the last GOP seen by ITB */
    unsigned consumed_gop; /* the GOP that we've consumed up to */

    /* last base_address and pcr_offset ITB's seen */
    uint32_t cdb_base_entry; /* address in cdb pointed by last base_address ITB. if 0, none seen. */
    uint32_t itb_base_entry; /* address of base_address itb */
    uint32_t pcr_offset;     /* only valid if pcr_offset_set is true */
    bool pcr_offset_set;
    uint32_t last_pts;
};

#if NEXUS_AUDIO_DECODER_PRIMER_SUPPORTED

struct itb_entry_t {
    uint32_t word0;
    uint32_t word1;
    uint32_t word2;
    uint32_t word3;
};
#define ITB_SIZE sizeof(struct itb_entry_t)

/* TODO: this logic is duplicated from audio.inc and should be refactored */
#if BCHP_CHIP == 7400
/* rap_v1 */
#elif BCHP_CHIP == 7125 || BCHP_CHIP == 7325 || BCHP_CHIP == 7335 || BCHP_CHIP == 7336 || BCHP_CHIP == 7340 || BCHP_CHIP == 7342 || BCHP_CHIP == 7420 || BCHP_CHIP == 7468 || BCHP_CHIP == 7550 || BCHP_CHIP == 7405
/* rap_v3 */
#elif BCHP_CHIP == 7408
/* ape mips */
#define IS_APE_PLATFORM 1
#else
/* ape raaga */
#define IS_APE_PLATFORM 1
#endif

#if IS_APE_PLATFORM
#define LOCK_TRANSPORT()    NEXUS_Module_Lock(g_NEXUS_audioModuleData.settings.modules.transport)
#define UNLOCK_TRANSPORT()  NEXUS_Module_Unlock(g_NEXUS_audioModuleData.settings.modules.transport)
#else
#define LOCK_TRANSPORT()    NEXUS_Module_Lock(g_NEXUS_audioModuleData.transport)
#define UNLOCK_TRANSPORT()  NEXUS_Module_Unlock(g_NEXUS_audioModuleData.transport)
#endif

#ifdef DEBUG_PRIMER
static void NEXUS_AudioDecoder_P_PrintItb2(NEXUS_AudioDecoderPrimerHandle primer, uint32_t from, uint32_t to)
{
    struct itb_entry_t *from_itb;
    struct itb_entry_t *to_itb;
    void *temp;
    BERR_Code rc;
    unsigned count;

    BDBG_OBJECT_ASSERT(primer, NEXUS_AudioDecoderPrimer);

    rc = BMEM_ConvertOffsetToAddress(NEXUS_Heap_GetMemHandle(primer->heap), from, &temp);
    BDBG_ASSERT(!rc);
    from_itb = temp;
    rc = BMEM_ConvertOffsetToAddress(NEXUS_Heap_GetMemHandle(primer->heap), to, &temp);
    BDBG_ASSERT(!rc);
    to_itb = temp;
    BDBG_ASSERT(from_itb <= to_itb);
    count = 0;
    while (from_itb < to_itb) {
        BDBG_MSG(("primer: %p ITB %p: %08x %08x %08x %08x", primer, from_itb, from_itb->word0, from_itb->word1, from_itb->word2, from_itb->word3));
        from_itb++;
        if (++count == 20) break;
    }
}

static void NEXUS_AudioDecoder_P_PrintItb(NEXUS_AudioDecoderPrimerHandle primer)
{
    uint32_t itb_valid, itb_read,itb_base;
    itb_valid = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Valid);
    itb_read = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Read);
    itb_base = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Base);

    if (itb_valid < itb_read) {
        uint32_t itb_wrap;
        itb_wrap = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Wrap);
        BDBG_MSG(("PrintItb %08x->%08x, %08x->%08x", itb_read, itb_wrap, itb_base, itb_valid));
        NEXUS_AudioDecoder_P_PrintItb2(primer, itb_read, itb_wrap);
        NEXUS_AudioDecoder_P_PrintItb2(primer, itb_base, itb_valid);
    }
    else {
        BDBG_MSG(("PrintItb %08x->%08x", itb_read, itb_valid));
        NEXUS_AudioDecoder_P_PrintItb2(primer, itb_read, itb_valid);
    }
}

static void NEXUS_AudioDecoder_P_DumpRegisters(NEXUS_AudioDecoderPrimerHandle primer)
{
    uint32_t valid, read, end, base;

    valid = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Valid);
    read = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Read);
    base = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Base);
    end = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_End);
    BDBG_MSG(("ITB: valid=%08x read=%08x base=%08x end=%08x depth=%d%%", valid, read, base, end, 
        (valid>=read?valid-read:(valid-base)+(end-read)) * 100 / (end-base)
        ));

    valid = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.CDB_Valid);
    read = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.CDB_Read);
    base = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.CDB_Base);
    end = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.CDB_End);
    BDBG_MSG(("CDB: valid=%08x read=%08x base=%08x end=%08x depth=%d%%", valid, read, base, end,
        (valid>=read?valid-read:(valid-base)+(end-read)) * 100 / (end-base)
        ));
}
#endif

static void NEXUS_AudioDecoder_P_PrimerSetRave(NEXUS_AudioDecoderPrimerHandle primer)
{
    uint32_t serialStc;
    int gop_index = -1;
    struct itb_entry_t *itb;
    unsigned itb_read;
    void *temp;
    int min_diff = 0;
    
    LOCK_TRANSPORT();
    NEXUS_StcChannel_GetSerialStc_priv(primer->startSettings.stcChannel, &serialStc);
    UNLOCK_TRANSPORT();

    if (primer->active) {
        unsigned i;

        i = primer->consumed_gop;
        while (i != primer->next_gop) {
            uint32_t stc = serialStc + primer->gops[i].pcr_offset; /* Serial STC + offset = STC */
            int diff = primer->gops[i].pts - stc;
            
#if 0
            BDBG_MSG_TRACE(("%p: eval%d stc=%#x pts=%#x at %#x/%#x", primer, i, stc, primer->gops[i].pts, primer->gops[i].cdb_read, primer->gops[i].itb_read));
#endif
            if (diff <= 0) {
                if (gop_index == -1 || diff > min_diff) {
                    gop_index = i;
                    min_diff = diff;
                }
            }
            
            if (++i == MAX_GOPS) {
                i = 0;
            }
        }
    }
    else {
        return;
    }                  
    
    if (gop_index == -1) {
#if 0
        BDBG_MSG_TRACE(("%p: no action. Serial STC=%08x, closest diff %5d, %d %d", primer, serialStc, min_diff, primer->consumed_gop, primer->next_gop));
#endif
        return;
    }
    primer->consumed_gop = gop_index+1;
    if (primer->consumed_gop == MAX_GOPS) {
        primer->consumed_gop = 0;
    }
    
    if (!primer->playback) {
        /* back up and write one pcr_offset ITB */
        itb_read = primer->gops[gop_index].itb_read;
        if (itb_read - ITB_SIZE > primer->itb_base) {
            itb_read -= ITB_SIZE;
        }
        else {
            /* there's no room for the PCR_OFFSET entry without setting the WRAP pointer, etc. just wait for a little more
            data to come. */
            return;
        }
        (void)BMEM_ConvertOffsetToAddress(NEXUS_Heap_GetMemHandle(primer->heap), itb_read, &temp);
        itb = temp;
        itb->word0 = 0x22800000; /* pcr offset, marked valid */
        itb->word1 = primer->gops[gop_index].pcr_offset;
        itb->word2 = 0;
        itb->word3 = 0;
    }

    BDBG_MSG_TRACE(("%p: SetRave PTS=%08x STC=%08x (%08x %08x), diff %5d, CDB %#x ITB %#x", primer,
        primer->gops[gop_index].pts, serialStc + primer->gops[gop_index].pcr_offset, serialStc, primer->gops[gop_index].pcr_offset, min_diff, 
        primer->gops[gop_index].cdb_read, primer->gops[gop_index].itb_read));
    
    {
        /* convert back to RAVE's inclusive semantics */
        unsigned temp = primer->gops[gop_index].cdb_read;
        if (temp != primer->cx_map.CDB_Base) temp--;
        BREG_Write32(g_pCoreHandles->reg, primer->cx_map.CDB_Read, temp);
        temp = primer->gops[gop_index].itb_read;
        if (temp != primer->cx_map.ITB_Base) temp--;
        BREG_Write32(g_pCoreHandles->reg, primer->cx_map.ITB_Read, temp);
    }
}
    
static void NEXUS_AudioDecoder_P_PrimerProcessItb(NEXUS_AudioDecoderPrimerHandle primer, uint32_t itb_valid)
{
    NEXUS_Error rc;
    struct itb_entry_t * pitb;
    struct itb_entry_t * pitb_end;
    uint8_t type;
    BMEM_Handle mem;
    void *temp;

    BDBG_OBJECT_ASSERT(primer, NEXUS_AudioDecoderPrimer);

    mem = NEXUS_Heap_GetMemHandle(primer->heap);
    rc = BMEM_ConvertOffsetToAddress(mem, primer->sitb_read, &temp);
    if (rc) {
        BDBG_ERR(("bad sitb_read=%08x", primer->sitb_read));
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
        BDBG_MSG_TRACE(("p=%p: w0=%08x %08x %08x %08x", primer, pitb->word0, pitb->word1, pitb->word2, pitb->word3));
        /* determine entry type */
        type = pitb->word0 >> 24;
        switch(type)
        {
        case 0x22: /* pcr offset see RAVE ITB docs */
            primer->pcr_offset = pitb->word1;
            primer->pcr_offset_set = true;
            break;

        case 0x21: /* pts */
            {
                uint32_t pts = pitb->word1;
                /* don't store PTS unless it meets a time threshold. this prevents the audio primer gops[] from 
                overflowing - if it overflows there will be an extended period of no sound when switching from
                 primer to audio decoder */      
                unsigned minPtsThresh=45*200;  /* 45kHz * 200 milliseconds */
                if ( primer->div2ms == 27000 ) minPtsThresh *=600; /* convert to 27Mhz units */

                if (primer->last_pts && pts < primer->last_pts) {
                    /* flush on PTS discontinuity */
                    NEXUS_AudioDecoderPrimer_Flush(primer);
                }
                
                if (primer->pcr_offset_set && primer->cdb_base_entry && (pts > primer->last_pts + minPtsThresh || !primer->last_pts)) {
                    primer->last_pts = primer->gops[primer->next_gop].pts = pts;
                    primer->gops[primer->next_gop].pcr_offset = primer->pcr_offset;
                    primer->gops[primer->next_gop].cdb_read = primer->cdb_base_entry;
                    primer->gops[primer->next_gop].itb_read = primer->itb_base_entry;
                    if (++primer->next_gop == MAX_GOPS) {
                        primer->next_gop = 0;
                    }
                    if (primer->next_gop == primer->consumed_gop) {
                        /* If you get overflow, there is a high chance of dropped audio data, resulting in no sound */
                        BDBG_ERR(("L%d p=%p primer overflow nxtGop=%d consGop=%d minThresh=%d " , __LINE__ , primer 
                            , primer->next_gop , primer->consumed_gop, minPtsThresh ));
                        
                        /* BDBG_ASSERT(primer->next_gop != primer->consumed_gop); */
                    }
                    /* NEXUS_AudioDecoder_P_PrimerSetRave(primer); */
                }
            }
            break;
#define CDB_OVERFLOW 0x10000
#define ITB_OVERFLOW 0x20000
        case 0x20: /* base_address  */
            primer->cdb_base_entry = pitb->word1;
            rc = BMEM_ConvertAddressToOffset(mem, pitb, &primer->itb_base_entry);
            if (rc) {rc = BERR_TRACE(rc); return;}

            if(0 != ( (CDB_OVERFLOW|ITB_OVERFLOW) & pitb->word2)){

                BDBG_ERR(("p=%p %s %s RAVE signalled overflow L%d " , primer, (CDB_OVERFLOW & pitb->word2) ? "CDB":"" , 
                                                            (ITB_OVERFLOW & pitb->word2) ? "ITB":"" , __LINE__ ));
                primer->overflow = true;
            }
            break;
        }
        pitb++;
    }

    rc = BMEM_ConvertAddressToOffset(mem, pitb, &primer->sitb_read);
    if (rc) {rc = BERR_TRACE(rc); return;}

}

static void NEXUS_AudioDecoder_P_PrimerCallback(void *context)
{
    NEXUS_AudioDecoderPrimerHandle primer = context;
    uint32_t itb_valid, itb_base, itb_wrap;

    primer->timer = NULL;
    
    /* convert ITB_Valid and ITB_Wrap to standard ring buffer semantics in this function. instead of pointing to the last byte of the ITB it should point
    to the first byte of the next ITB. Note that RAVE has an exception when valid == base. */
    itb_valid = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Valid);
    itb_base = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Base);
    if (itb_valid != itb_base) itb_valid++;
    /* check for itb wrap around and handle it */
    if (itb_valid < primer->sitb_read) {
        /* for this to be true, the HW must have wrapped. */
        itb_wrap = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Wrap);
        BDBG_ASSERT(itb_wrap);
        itb_wrap++;
        NEXUS_AudioDecoder_P_PrimerProcessItb(primer, itb_wrap);
        /* if we consumed up to wrap point then wrap */
        if (primer->sitb_read >= itb_wrap) {
            primer->sitb_read = primer->itb_base;
            NEXUS_AudioDecoder_P_PrimerProcessItb(primer, itb_valid);
        }
    }
    else {
        NEXUS_AudioDecoder_P_PrimerProcessItb(primer, itb_valid);
    }
    
    NEXUS_AudioDecoder_P_PrimerSetRave(primer);

    if (primer->active) {
        primer->timer = NEXUS_ScheduleTimer(TIMER_INTERVAL, NEXUS_AudioDecoder_P_PrimerCallback, primer);
    }
}

/* TEMP */
static NEXUS_Error NEXUS_AudioDecoder_P_GetRaveSettings(NEXUS_AudioDecoderHandle handle, NEXUS_RaveOpenSettings *pRaveOpenSettings, const NEXUS_AudioDecoderOpenSettings *pSettings)
{
#if IS_APE_PLATFORM
    LOCK_TRANSPORT();
    NEXUS_Rave_GetDefaultOpenSettings_priv(pRaveOpenSettings);
    UNLOCK_TRANSPORT();
    
    BAPE_Decoder_GetDefaultCdbItbConfig(handle->channel, &pRaveOpenSettings->config);

    if ( pSettings->fifoSize == 0 )
    {
        /* NOTE: Don't automatically increase CDB/ITB for IP Settop internally. */
    }
    else
    {
        /* Make ITB proportional to CDB */
        pRaveOpenSettings->config.Itb.Length = 1024*((pRaveOpenSettings->config.Itb.Length/1024) * (pSettings->fifoSize/1024))/(pRaveOpenSettings->config.Cdb.Length/1024);
        BDBG_ASSERT(0 != pRaveOpenSettings->config.Itb.Length);
        pRaveOpenSettings->config.Cdb.Length = pSettings->fifoSize;
    }
#else
/* RAP */        
    NEXUS_Error rc;
    BSTD_UNUSED(handle);    
    
    LOCK_TRANSPORT();
    NEXUS_Rave_GetDefaultOpenSettings_priv(pRaveOpenSettings);
    UNLOCK_TRANSPORT();
#if BRAP_VER >= 4
    raveSettings.config = sCdbItbCfg[0];
#else
    rc = BRAP_GetBufferConfig(g_NEXUS_audioModuleData.hRap, BRAP_BufferCfgMode_eAdvAudioMode, &pRaveOpenSettings->config);
    if ( rc ) return BERR_TRACE(rc);
#endif
    if ( pSettings->fifoSize == 0 )
    {
        /* NOTE: Don't automatically increase CDB/ITB for IP Settop internally. */
    }
    else
    {
        /* Make ITB proportional to CDB */
        pRaveOpenSettings->config.Itb.Length = 1024*((pRaveOpenSettings->config.Itb.Length/1024) * (pSettings->fifoSize/1024))/(pRaveOpenSettings->config.Cdb.Length/1024);
        BDBG_ASSERT(0 != pRaveOpenSettings->config.Itb.Length);
        pRaveOpenSettings->config.Cdb.Length = pSettings->fifoSize;
    }
#endif

    return 0;
}

NEXUS_AudioDecoderPrimerHandle NEXUS_AudioDecoderPrimer_Open( NEXUS_AudioDecoderHandle audioDecoder )
{
    NEXUS_AudioDecoderPrimerHandle primer;
    NEXUS_RaveOpenSettings raveOpenSettings;
    NEXUS_Error rc = 0;
    
    primer = BKNI_Malloc(sizeof(*primer));
    if (!primer) {
        BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }
    NEXUS_OBJECT_INIT(NEXUS_AudioDecoderPrimer, primer);
    primer->audioDecoder = audioDecoder;
    
    /* TODO: audioDecoder doesn't store its open settings. it needs to. */
    {
        NEXUS_AudioDecoderOpenSettings openSettings;
        NEXUS_AudioDecoder_GetDefaultOpenSettings(&openSettings);
        /* TODO: should call into AudioDecoder for standard CDB/ITB sizes */
        rc = NEXUS_AudioDecoder_P_GetRaveSettings(audioDecoder, &raveOpenSettings, &openSettings);
        if (rc) {rc = BERR_TRACE(rc); goto error;}
    }
    LOCK_TRANSPORT();
    primer->rave = NEXUS_Rave_Open_priv(&raveOpenSettings);
    if (!primer->rave) {
        rc = BERR_TRACE(NEXUS_UNKNOWN);
    }
    UNLOCK_TRANSPORT();
    if (rc) {rc = BERR_TRACE(rc); goto error;}
    return primer;
    
error:
    NEXUS_AudioDecoderPrimer_Close(primer);
    return NULL;
}

static void NEXUS_AudioDecoderPrimer_P_Finalizer( NEXUS_AudioDecoderPrimerHandle primer )
{
    NEXUS_OBJECT_ASSERT(NEXUS_AudioDecoderPrimer, primer);

    if (primer->active) {
        NEXUS_AudioDecoderPrimer_Stop(primer);
    }
    
    if ( primer->rave == primer->audioDecoder->raveContext ) {
        /* audio decoder is pointing at primed rave context, restore audio decoder's rave */
        primer->audioDecoder->raveContext = primer->audioDecoder->savedRaveContext;
        primer->audioDecoder->savedRaveContext = NULL;
    }

    if (primer->rave) {
        LOCK_TRANSPORT();
        NEXUS_Rave_Close_priv(primer->rave);
        UNLOCK_TRANSPORT();
    }    
    NEXUS_OBJECT_DESTROY(NEXUS_AudioDecoderPrimer, primer);
    BKNI_Free(primer);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_AudioDecoderPrimer, NEXUS_AudioDecoderPrimer_Close);

void NEXUS_AudioDecoderPrimer_Flush( NEXUS_AudioDecoderPrimerHandle primer )
{
    uint32_t valid, base;
    
    BDBG_OBJECT_ASSERT(primer, NEXUS_AudioDecoderPrimer);

    /* If playback wrapped, rave was flushed, so reset primers pointers - all the data is gone */
    primer->cdb_base_entry = 0;
    primer->itb_base_entry = 0;
    BKNI_Memset(primer->gops, 0, sizeof(primer->gops));
    primer->next_gop = 0;
    primer->consumed_gop = 0;
    primer->pcr_offset = 0;
    primer->last_pts = 0;
    primer->overflow = false;
    primer->pcr_offset_set = primer->playback;
    
    /* empty the buffer */
    valid = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.CDB_Valid);
    base = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.CDB_Base);
    BREG_Write32(g_pCoreHandles->reg, primer->cx_map.CDB_Read, (valid == base)?base:valid+1);
    
    valid = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Valid);
    base = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Base);
    BREG_Write32(g_pCoreHandles->reg, primer->cx_map.ITB_Read, (valid == base)?base:valid+1);
}

NEXUS_Error NEXUS_AudioDecoder_P_StartPrimer( NEXUS_AudioDecoderHandle audioDecoder, NEXUS_AudioDecoderPrimerHandle primer, const NEXUS_AudioDecoderStartSettings *pStartSettings, bool initial )
{
    NEXUS_Error rc;
    NEXUS_PidChannelStatus pidChannelStatus;
    NEXUS_RaveStatus raveStatus;

    if ( primer->startSettings.pidChannel ) {
        rc = NEXUS_PidChannel_GetStatus(primer->startSettings.pidChannel, &pidChannelStatus);
        if (rc) return BERR_TRACE(rc);
        primer->playback = primer->audioDecoder->isPlayback = pidChannelStatus.playback;
        BDBG_MSG(("p=%p primer->audioDecoder->isPlayback=%d" , primer, primer->audioDecoder->isPlayback ));
        if ( audioDecoder->started ) {
            if ( primer->audioDecoder->isPlayback != primer->playback ) {
                return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            }
        }
    }
    else {
        BDBG_MSG(("p=%p no pidchannel set" , primer ));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    primer->div2ms = NEXUS_IS_DSS_MODE(pidChannelStatus.transportType) ? 27000 : 45;
    BDBG_MSG(("p=%p div2ms=%d" , primer, primer->div2ms ));

    if ( initial ) {
        /* Reconfiguring Rave from scratch wipes out any data in rave - don't do it */
/* TODO: unify this API */
#if IS_APE_PLATFORM
        rc = NEXUS_AudioDecoder_P_ConfigureRave(primer->audioDecoder, primer->rave, pStartSettings, &pidChannelStatus);
#else
        rc = NEXUS_AudioDecoder_P_ConfigureRave(primer->audioDecoder, primer->rave, pStartSettings);
#endif
        if (!rc) {
            /* Don't need this because stop has already disabled RAVE ?? */
            LOCK_TRANSPORT();
            rc = NEXUS_Rave_GetStatus_priv(primer->rave, &raveStatus);
            if (!rc) {
                NEXUS_Rave_Disable_priv(primer->rave);
                NEXUS_Rave_Flush_priv(primer->rave);
            }
            UNLOCK_TRANSPORT();
        }
        if (rc) return BERR_TRACE(rc);
    }
    else {
        if (audioDecoder->raveContext) {
            LOCK_TRANSPORT();
            rc = NEXUS_Rave_GetStatus_priv(primer->rave, &raveStatus);
            UNLOCK_TRANSPORT();
            if (rc) return BERR_TRACE(rc);
        }
        else {
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
        }
    }

    if (pStartSettings->stcChannel) {
        LOCK_TRANSPORT();
        NEXUS_StcChannel_EnablePidChannel_priv(pStartSettings->stcChannel, pStartSettings->pidChannel);
        UNLOCK_TRANSPORT();
    }

    primer->heap      = g_pCoreHandles->nexusHeap[0]; /* This is the heap used for RAVE access */
    primer->cx_map    = raveStatus.xptContextMap;
    primer->itb_base  = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Base);
    primer->sitb_read = BREG_Read32(g_pCoreHandles->reg, primer->cx_map.ITB_Read);
    if (primer->sitb_read & 0xF) {
        /* READ ptr can come from HW as inclusive, but we treat it as exclusive */
        primer->sitb_read++;
        BDBG_ASSERT(primer->sitb_read % 0x10 == 0);
    }

    if ( initial ) {
        primer->cdb_base_entry = 0;
        primer->itb_base_entry = 0;
        primer->active = true;
        primer->overflow = false;
        BKNI_Memset(primer->gops, 0, sizeof(primer->gops));
        primer->next_gop = 0;
        primer->consumed_gop = 0;
        primer->pcr_offset = 0;
        primer->pcr_offset_set = primer->playback;
        primer->last_pts = 0;
    } else {
        primer->active = true;
        primer->overflow = false;
        primer->pcr_offset_set = primer->playback;
    }

    BDBG_ASSERT(!primer->timer);
    primer->timer = NEXUS_ScheduleTimer(TIMER_INTERVAL, NEXUS_AudioDecoder_P_PrimerCallback, primer);
    if (!primer->timer) {
        return BERR_TRACE(NEXUS_UNKNOWN);
    }

    LOCK_TRANSPORT();
    NEXUS_Rave_Enable_priv(primer->rave);

    if (!primer->playback) {
        /* set the pcr offset context into acquire mode, so that we get at least one pcr offset entry in the ITB */
        NEXUS_StcChannel_SetPcrOffsetContextAcquireMode_priv(primer->startSettings.stcChannel);
    }
    UNLOCK_TRANSPORT();

    NEXUS_PidChannel_ConsumerStarted(primer->startSettings.pidChannel ); /* needed to unpause playback & stuff like that */

    return 0;
}

NEXUS_Error NEXUS_AudioDecoderPrimer_StopDecodeAndStartPrimer( NEXUS_AudioDecoderPrimerHandle primer, NEXUS_AudioDecoderHandle audioDecoder )
{
    NEXUS_Error errCode;

    BDBG_OBJECT_ASSERT(primer, NEXUS_AudioDecoderPrimer);

    if (primer->active) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }

    if (audioDecoder != primer->audioDecoder ) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if ( !primer->audioDecoder->started )
    {
        BDBG_ERR(("Decoder not started, so can't stop it !"));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

#if IS_APE_PLATFORM
    if ( audioDecoder->programSettings.input )
    {
        if ( NEXUS_AudioInput_P_SupportsFormatChanges( audioDecoder->programSettings.input) )
        {
            /* If this input supports dynamic format changes, disable the dynamic format change interrupt. */
            (void)NEXUS_AudioInput_P_SetFormatChangeInterrupt( audioDecoder->programSettings.input, NULL, NULL, 0);
        }
    }
#endif

    /* Stop Audio decode without flushing RAVE data */
    errCode = NEXUS_AudioDecoder_P_Stop(audioDecoder, false);
    if ( errCode )
    {
        errCode = BERR_TRACE(errCode);
    }

    audioDecoder->running = false;
    audioDecoder->started = false;
    audioDecoder->trickForceStopped = false; /* do we need to forcedly unmute on Stop, in a way it helps if in a PIP change mode decoder is moved from trickmode on one channel to normal mode on another channel, however it hurts if one stops decoder just in order to change a PID/ audio program */

    BKNI_Memset(&audioDecoder->programSettings, 0, sizeof(audioDecoder->programSettings));

    /* The primer interface may have replaced the main RAVE context with a primed context. this reverses that. */
    if (audioDecoder->savedRaveContext) {
        audioDecoder->raveContext = audioDecoder->savedRaveContext;
        audioDecoder->savedRaveContext = NULL;
    }
    
    /* transfer ownership to primer */
    NEXUS_OBJECT_ACQUIRE(primer, NEXUS_PidChannel, primer->startSettings.pidChannel);
    NEXUS_OBJECT_RELEASE(primer->audioDecoder, NEXUS_PidChannel, primer->startSettings.pidChannel);

    return  NEXUS_AudioDecoder_P_StartPrimer( primer->audioDecoder, primer, &primer->startSettings, false );
}

NEXUS_Error NEXUS_AudioDecoderPrimer_Start( NEXUS_AudioDecoderPrimerHandle primer, const NEXUS_AudioDecoderStartSettings *pStartSettings )
{
    
    BDBG_OBJECT_ASSERT(primer, NEXUS_AudioDecoderPrimer);
    
    if (primer->active) {
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }
    
    if ( !pStartSettings ) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (!primer->audioDecoder) {
        BDBG_ERR(("No audioDecoder associated with primer" ));
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    primer->startSettings = *pStartSettings;
    NEXUS_OBJECT_ACQUIRE(primer, NEXUS_PidChannel, primer->startSettings.pidChannel);

    return  NEXUS_AudioDecoder_P_StartPrimer( primer->audioDecoder, primer, pStartSettings, true );

}

void NEXUS_AudioDecoderPrimer_Stop( NEXUS_AudioDecoderPrimerHandle primer )
{
    BDBG_OBJECT_ASSERT(primer, NEXUS_AudioDecoderPrimer);
    
    if (primer->active==false) {
        BDBG_ERR(("%p: Primer already stopped", primer));
        return;
    }

    BDBG_MSG(("%p: Stop", primer));

    primer->active = false;

    /* Rave must be disabled in order for StartPrimer to work correctly later on */
    LOCK_TRANSPORT();
    NEXUS_Rave_Disable_priv(primer->rave);
    NEXUS_Rave_RemovePidChannel_priv(primer->rave); /* this is required for StartPrimer with a different pid */
    NEXUS_Rave_Flush_priv(primer->rave);
    UNLOCK_TRANSPORT();
    NEXUS_OBJECT_RELEASE(primer, NEXUS_PidChannel, primer->startSettings.pidChannel);

    if (primer->timer) {
        NEXUS_CancelTimer(primer->timer);
        primer->timer = NULL;
    }

    if ( primer->rave == primer->audioDecoder->raveContext ) {
        /* audio decoder is pointing at primed rave context, restore audio decoder's rave */
        primer->audioDecoder->raveContext = primer->audioDecoder->savedRaveContext;
        primer->audioDecoder->savedRaveContext = NULL;
    }
}

NEXUS_Error NEXUS_AudioDecoderPrimer_StopPrimerAndStartDecode( NEXUS_AudioDecoderPrimerHandle primer , NEXUS_AudioDecoderHandle audioDecoder )
{
    NEXUS_Error rc;
    
    BDBG_OBJECT_ASSERT(primer, NEXUS_AudioDecoderPrimer);

    if (audioDecoder != primer->audioDecoder) {
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if (audioDecoder->started) {
        BDBG_ERR(("AudioDecoder already started"));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    if (!primer->active) {
        BDBG_ERR(("%p: cannot StartDecodeWithPrimer on stopped primer", primer));
        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    BDBG_ASSERT(!audioDecoder->savedRaveContext);
    audioDecoder->savedRaveContext = audioDecoder->raveContext;
    audioDecoder->raveContext = primer->rave;

    primer->active = false;
    if (primer->timer) {
        NEXUS_CancelTimer(primer->timer);
        primer->timer = NULL;
    }

    BDBG_MSG_TRACE(("p=%p: starting decode", primer));
#ifdef DEBUG_PRIMER
    NEXUS_AudioDecoder_P_PrintItb(primer);
    /* NEXUS_AudioDecoder_P_DumpRegisters(primer); */
#endif

    rc = NEXUS_AudioDecoder_Start(audioDecoder, &primer->startSettings);
    if (rc!=NEXUS_SUCCESS) {
        audioDecoder->raveContext = audioDecoder->savedRaveContext;
        audioDecoder->savedRaveContext = NULL;
        return BERR_TRACE(rc);
    }

    /* NEXUS_AudioDecoder_Start acquires the handle, so primer must release */
    NEXUS_OBJECT_RELEASE(primer, NEXUS_PidChannel, primer->startSettings.pidChannel);
    
    return 0;
}

void NEXUS_AudioDecoder_GetPrimerSettings( NEXUS_AudioDecoderPrimerHandle primer, NEXUS_AudioDecoderPrimerSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(primer, NEXUS_AudioDecoderPrimer);
    *pSettings = primer->settings;
}

NEXUS_Error NEXUS_AudioDecoder_SetPrimerSettings( NEXUS_AudioDecoderPrimerHandle primer, const NEXUS_AudioDecoderPrimerSettings *pSettings )
{
    BDBG_OBJECT_ASSERT(primer, NEXUS_AudioDecoderPrimer);
    primer->settings = *pSettings;
    /* TODO: I've added the function for now, but audio primer may be simple enough to not need settings */
    return 0;
}

#else /* !NEXUS_AUDIO_DECODER_PRIMER_SUPPORTED  */
NEXUS_AudioDecoderPrimerHandle NEXUS_AudioDecoderPrimer_Open( NEXUS_AudioDecoderHandle audioDecoder )
{
    BSTD_UNUSED(audioDecoder);
    return NULL;
}

static void NEXUS_AudioDecoderPrimer_P_Finalizer( NEXUS_AudioDecoderPrimerHandle primer )
{
    NEXUS_OBJECT_ASSERT(NEXUS_AudioDecoderPrimer, primer);

    NEXUS_OBJECT_DESTROY(NEXUS_AudioDecoderPrimer, primer);
    BKNI_Free(primer);
}

NEXUS_OBJECT_CLASS_MAKE(NEXUS_AudioDecoderPrimer, NEXUS_AudioDecoderPrimer_Close);

void NEXUS_AudioDecoderPrimer_Flush( NEXUS_AudioDecoderPrimerHandle primer )
{
    BSTD_UNUSED(primer);
}

NEXUS_Error NEXUS_AudioDecoderPrimer_StopDecodeAndStartPrimer( NEXUS_AudioDecoderPrimerHandle primer, NEXUS_AudioDecoderHandle audioDecoder )
{
    BSTD_UNUSED(primer);
    BSTD_UNUSED(audioDecoder);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

NEXUS_Error NEXUS_AudioDecoderPrimer_Start( NEXUS_AudioDecoderPrimerHandle primer, const NEXUS_AudioDecoderStartSettings *pStartSettings )
{
    BSTD_UNUSED(primer);
    BSTD_UNUSED(pStartSettings);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

void NEXUS_AudioDecoderPrimer_Stop( NEXUS_AudioDecoderPrimerHandle primer )
{
    BSTD_UNUSED(primer);
}

NEXUS_Error NEXUS_AudioDecoderPrimer_StopPrimerAndStartDecode( NEXUS_AudioDecoderPrimerHandle primer , NEXUS_AudioDecoderHandle audioDecoder )
{
    BSTD_UNUSED(primer);
    BSTD_UNUSED(audioDecoder);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}

void NEXUS_AudioDecoder_GetPrimerSettings( NEXUS_AudioDecoderPrimerHandle primer, NEXUS_AudioDecoderPrimerSettings *pSettings )
{
    BSTD_UNUSED(primer);
    BSTD_UNUSED(pSettings);
}

NEXUS_Error NEXUS_AudioDecoder_SetPrimerSettings( NEXUS_AudioDecoderPrimerHandle primer, const NEXUS_AudioDecoderPrimerSettings *pSettings )
{
    BSTD_UNUSED(primer);
    BSTD_UNUSED(pSettings);
    return BERR_TRACE(NEXUS_NOT_SUPPORTED);
}
#endif
