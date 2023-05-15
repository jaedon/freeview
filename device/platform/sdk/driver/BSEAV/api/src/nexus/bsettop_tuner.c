/***************************************************************************
 *  Copyright (c) 2003-2012, Broadcom Corporation
 *  All Rights Reserved
 *  Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_tuner.c $
 * $brcm_Revision: 68 $
 * $brcm_Date: 11/13/12 9:54a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_tuner.c $
 * 
 * 68   11/13/12 9:54a erickson
 * SW7435-372: convert to NEXUS_Frontend_ReadSoftDecisions
 * 
 * 67   10/8/12 2:20p erickson
 * SW7425-3782: use NEXUS_FrontendConnector
 * 
 * 66   8/1/12 4:06p vishk
 * SW7231-907, SW7425-3519: Undo removing untune() upon channel change.
 * 
 * 65   7/30/12 11:20a jtna
 * SW7346-943: wrap with NEXUS_HAS_FRONTEND
 * 
 * 64   7/26/12 5:21p jtna
 * SW7346-943: do upto two fake frontend<->PB mappings in Brutus to handle
 * some PIP usage modes
 * 
 * 63   7/18/12 11:55a vishk
 * SW7231-885: Disable untune between channel change to avoid powering
 * down the tuner.
 * 
 * 62   6/27/12 2:59p jtna
 * SW7425-3350: rework Brutus MTSIF frontend handling
 * 
 * 61   5/15/12 5:09p mphillip
 * SW7358-277: berErrors was renamed to berErrorCount
 * 
 * 60   4/27/12 2:12p mward
 * SW7435-151:  Initialize nStatus in btuner_get_vsb_status.
 * 
 * 59   3/26/12 10:03a katrep
 * SW3461-169:build broken for platforms without frontend
 * 
 * 58   3/13/12 4:11p mphillip
 * SW7344-263: Update lna and tuner status for 7344/7346
 * 
 * 57   3/12/12 4:53p vishk
 * SW3461-169: Implement btuner_get_fast_status api.
 * 
 * 56   3/2/12 4:45p vishk
 * SW7552-183: The fec counters need to be accumulated and not reset on
 * every read. The counters are reset upon every lock change.
 * 
 * 55   2/24/12 12:11p jtna
 * SW7425-2387: coverity defect STACK_USE
 * 
 * 54   1/2/12 2:51p erickson
 * SW7552-156: add btuner_ofdm_status.signal_strength
 * 
 * SW7552-156/1   12/27/11 6:10p bingz
 * SW7552-156: Need to add signal level as opposed to RF and IF
 * 
 * 53   9/6/11 9:31p vishk
 * SW3461-48: Fix 3461 frontend issues.
 * 
 * 52   6/6/11 2:47p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
 * Add support for Frequency Offset and make dvbt the default mode for
 * ofdm cores and temporarily set wait_for_lock to false.
 * 
 * 51   5/27/11 9:42p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
 * Implement brutus support for the downstream core.
 * 
 * 50   5/23/11 3:34p jrubio
 * SW7346-213: add untune functions
 * 
 * 49   3/18/11 5:26p VISHK
 * SW7231-76: Add support for FBTSFF 97231 + 3128 board.
 * 
 * 48   3/18/11 11:41a VISHK
 * SW7231-76: Add support for FBTSFF 97231 + 3128 board.
 * 
 * 47   2/25/11 4:35p mward
 * SW7125-769:  Do not use BDBG_ERR() for an open attempt on unavailable
 * tuner.  Return NULL to the app, let the app decide what to do about
 * it.
 * 
 * 46   2/7/11 10:21a mward
 * SW7125-821:  For certain standby scenarios, don't automatically untune
 * when closing tuner.
 * 
 * 45   1/27/11 11:29a mward
 * SW7125-726:  Add QAM Annex C.
 * 
 * 44   1/21/11 5:41p nitinb
 * SW7550-526: SNR on Brutus status page is incorrect for OFDM
 * 
 * 43   10/29/10 3:30p erickson
 * SW35230-1306: fix bandwidth units
 * 
 * 42   9/29/10 2:44p mward
 * SW35230-1306: deprecate NEXUS_FrontendXxxBandwidth. use unsigned
 * integer in units of Hz instead.
 * 
 * 41   6/10/10 6:29p mward
 * SW7125-35: Use the spectrum settings in btuner_tune_qam().
 * 
 * 40   6/7/10 9:53a erickson
 * SW7550-236: back out btuner_tune_ofdm #if 0. this needs to be fixed in
 * lower levels.
 *
 * 39   6/4/10 11:16a erickson
 * SW7550-236: merge
 *
 * Tuner_Hack/SW7550-236/2   5/21/10 3:02p pinglong
 * SW7550-236: move the postRsBer calculation to info_status.cpp
 *
 * Tuner_Hack/SW7550-236/1   4/21/10 9:49p pinglong
 * SW7550-236: It's not right for some item for status page of brutus.
 *
 * Tuner_Hack/3   3/18/10 3:18p farshidf
 * SW7550-38: update the SNR settings
 *
 * Tuner_Hack/2   2/5/10 2:24p VISHK
 * CR7550-115 :7550 QAM/DVB-C support in Settop and Brutus.
 *
 * Tuner_Hack/1   12/22/09 2:48p farshidf
 * SW7550-38: Hack the code so it does not send a new acquire if not
 * locked
 *
 * 38   1/4/10 6:59p mward
 * SW7125-156:  Corrected QAM SNR estimate calculation.
 *
 * 37   12/11/09 4:57p mphillip
 * SW7550-112: Merge 7550 changes to main branch
 *
 * Refsw_7550/1   11/23/09 2:41p chengli
 * SW7550-64: fill field mode in NEXUS_FrontendOfdmSettings
 *
 * 36   12/9/09 12:04p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 35   7/8/09 6:49a erickson
 * PR56607: call NEXUS_StopCallbacks before NEXUS_Frontend_Untune. This
 * ensures that no callbacks will come from Frontend after btuner has
 * been freed. call NEXUS_StartCallbacks before every
 * NEXUS_Frontend_TuneXxx function as well.
 *
 * 34   6/29/09 6:39p vishk
 * PR 56070: BCM31xx: Need to read dsChannelPower
 *
 * 33   5/1/09 9:53a katrep
 * PR43490: Fixed crash at brutus quit,during bstream_close for 656/i2S
 * tuning
 *
 * 32   4/16/09 11:28a vishk
 * PR 46431: Add support for BCM3117, BCM3114, and BCM3112
 *
 * 31   3/5/09 11:51a prasadv
 * PR52808: OOB SNR should be in unit of 1/100 dB
 *
 * 30   3/5/09 10:44a prasadv
 * PR52808: correct SNR value for display
 *
 * 29   3/3/09 1:04p vishk
 * PR 46431: ADS PI, Add support for BCM3117, BCM3114, and BCM3112. Making
 * enums consistent.
 *
 * 28   2/11/09 10:30a anilmm
 * PR47842:  Correct BER calculation for qam
 *
 * 27   2/5/09 6:03p katrep
 * PR43490: fixed kernel crash during 656/i2s input during get soft
 * decisions api call
 *
 * 26   1/8/09 11:27a katrep
 * PR48985: Fixed compiler warning
 *
 * 25   12/18/08 9:29p agin
 * PR49983: Incorrect channel status reported for power, BER, SNR for
 * 97335 AST.
 *
 * 24   11/15/08 7:10a agin
 * PR48985: Support LDPC and Turbo at the same time.
 *
 * 23   9/23/08 9:16a jgarrett
 * PR 46513: Merging 97459 support to mainline
 *
 * 22   9/8/08 7:10p katrep
 * PR36068: Increase OFDM lock timeout value
 *
 * 21   8/5/08 12:05p vishk
 * PR 45270: Asset/Seg-Fault in bsettop_tuner.c:340 when input source
 * removed post btuner_close()
 *
 * 20   7/30/08 8:15p shyam
 * PR43426 : Fix compile warnings
 *
 * 19   7/17/08 4:49p shyam
 * PR43426 : Add 7043 656 Encode support
 *
 * PR44830/2   7/15/08 6:16p anilmm
 * PR44830: Merge from main branch
 *
 * PR44830/1   7/15/08 6:10p anilmm
 * PR44830: Add settop api to nexus support for BCM97459
 *
 * 18   7/9/08 4:10p katrep
 * PR43490: Add support 656/i2s tuning using analog tune api.
 *
 * 17   7/1/08 10:59a agin
 * PR44448: Fix SNR reporting problem.
 *
 * 16   5/22/08 3:43p jgarrett
 * PR 42054: Fixing 8psk for 450x
 *
 * 15   5/22/08 9:03a jgarrett
 * PR 42945: Converting SNR units
 *
 * 14   4/24/08 3:34p jrubio
 * PR42054: add diseqc code and status check workaround PR42052
 *
 * 13   4/3/08 5:42p jgarrett
 * PR 41312: Setting callback events
 *
 * 12   4/1/08 9:19a erickson
 * PR36068: fix compilation error
 *
 * 11   3/31/08 7:49p katrep
 * PR36068: Fixed the ofdm setting conversion from settop api to nexus
 *
 * 10   3/25/08 12:50p jgarrett
 * PR 40835: Grouping soft decisions for efficiency
 *
 * 9   3/25/08 10:59a erickson
 * PR36068: fix
 *
 * 8   3/25/08 10:10a erickson
 * PR36068: added analog tuner
 *
 * 7   3/17/08 5:40p katrep
 * PR36068:Add ofdm support
 *
 * 6   3/12/08 5:31p katrep
 * PR40489:Do not set if freq for 8VSB. Let the nexus layer set the
 * correct values
 *
 * 5   2/19/08 9:59a jgarrett
 * PR 36068: Updating tuner api
 *
 * 4   2/4/08 10:00a erickson
 * PR36068: update
 *
 * 3   11/27/07 11:15p erickson
 * PR36068: add vsb
 *
 * 2   10/16/07 12:35p erickson
 * PR36068: brutus up over settop api/nexus
 *
 ***************************************************************************/
#include "bsettop_impl.h"

#if B_HAS_ANALOG
#include "nexus_composite_input.h"
#include "nexus_component_input.h"
#include "nexus_svideo_input.h"
#endif

#if B_HAS_TRANSCODE
#include "nexus_transcoder_vdec.h"
#endif
#include "nexus_core_utils.h"

BDBG_MODULE(tuner);

BDBG_OBJECT_ID(btuner);

struct btuner {
    BDBG_OBJECT(btuner)
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendHandle nTuner;
#endif
    bsettop_callback lock_callback;
    void *callback_context;
    B_EventHandle lockEvent, waitEvent;
    b_event_id_t lockEventId;
    bband_t band;
    struct {
        struct bstream stream;
        btuner_analog_params params;
    } analog;
};

#if NEXUS_HAS_FRONTEND
static void btuner_p_lock_handler(void *context);
#endif

void btuner_p_init(void)
{
    /* Register the tuners with the bconfig interface */
    bconfig_p_set_tuners();
}

bband_t b_get_inband(btuner_t tuner)
{
    if (tuner) return tuner->band;
    else return NULL;
}

btuner_t btuner_open(bobject_t tuner_id)
{
#if NEXUS_HAS_FRONTEND
    btuner_t tuner;
    unsigned index;
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_FrontendUserParameters userParams;


    BDBG_MSG(("btuner_open: %d", B_ID_GET_INDEX(tuner_id)));

    /* The Settop API tuner code depends on the reference platform "GetConfiguration" API for
    frontend detection and instance management. See 97400 or related platforms for API and implementation. */
    NEXUS_Platform_GetConfiguration(&platformConfig);

    index = B_ID_GET_INDEX(tuner_id);

#if B_HAS_EXTERNAL_ANALOG
    /* tuner numbber 7 has special meaning,
        need to make this dynamic */
    if(index==7)
    {
        tuner = BKNI_Malloc(sizeof(*tuner));
        BKNI_Memset(tuner, 0, sizeof(*tuner));
        BDBG_OBJECT_SET(tuner, btuner);
        return tuner;
    }
#endif
#if B_HAS_TRANSCODE
    if (index >= NEXUS_MAX_FRONTENDS && index < NEXUS_MAX_FRONTENDS+B_MAX_ENCODES)
    {
        tuner = BKNI_Malloc(sizeof(*tuner));
        BKNI_Memset(tuner, 0, sizeof(*tuner));
        BDBG_OBJECT_SET(tuner, btuner);
        return tuner;
    }
    else
#endif

    if (index >= NEXUS_MAX_FRONTENDS || !platformConfig.frontend[index]) {
        return NULL;
    }

    tuner = BKNI_Malloc(sizeof(*tuner));
    BKNI_Memset(tuner, 0, sizeof(*tuner));
    BDBG_OBJECT_SET(tuner, btuner);
    tuner->nTuner = platformConfig.frontend[index];
    tuner->lockEvent = B_Event_Create(NULL);
    if ( NULL == tuner->lockEvent )
    {
        BSETTOP_ERROR(berr_external_error);
        goto error;
    }
    tuner->waitEvent = B_Event_Create(NULL);
    if ( NULL == tuner->waitEvent )
    {
        BSETTOP_ERROR(berr_external_error);
        goto error;
    }
    tuner->lockEventId = b_event_register(tuner->lockEvent, btuner_p_lock_handler, tuner);
    if ( NULL == tuner->lockEventId )
    {
        BSETTOP_ERROR(berr_external_error);
        goto error;
    }

    NEXUS_Frontend_GetUserParameters(tuner->nTuner, &userParams);
    tuner->band = &g_bands[userParams.param1]; /* Platform initializes this to input band, our default mapping is ibN->pbN */

    return tuner;

error:
    btuner_close(tuner);
#else
    BSTD_UNUSED(tuner_id);
#endif

    return NULL;
}

void btuner_untune(btuner_t tuner)
{
#if NEXUS_HAS_FRONTEND
    BDBG_OBJECT_ASSERT(tuner, btuner);
    if(tuner->nTuner)
    {
        NEXUS_StopCallbacks(tuner->nTuner);
        NEXUS_Frontend_Untune(tuner->nTuner);
    }
#else
    BSTD_UNUSED(tuner);
#endif
}

void btuner_close(btuner_t tuner)
{
#if NEXUS_HAS_FRONTEND
    BDBG_OBJECT_ASSERT(tuner, btuner);

    if(tuner->nTuner)
    {
        NEXUS_StopCallbacks(tuner->nTuner);
#if (0==(NEXUS_HOST_CONTROL_EXTERNAL_FRONTEND_STANDBY+NEXUS_HOST_CONTROL_INTERNAL_FRONTEND_STANDBY))
/* No implicit untune.  Keep frontends on in case of live handoff between host and cable modem control (7125 family) */
        NEXUS_Frontend_Untune(tuner->nTuner);
#endif
    }

    if ( NULL != tuner->lockEventId )
    {
        b_event_unregister(tuner->lockEventId);
    }
    if ( NULL != tuner->lockEvent )
    {
        B_Event_Destroy(tuner->lockEvent);
    }
    if ( NULL != tuner->waitEvent )
    {
        B_Event_Destroy(tuner->waitEvent);
    }

    BDBG_OBJECT_UNSET(tuner, btuner);
    BKNI_Free(tuner);
#else
    BSTD_UNUSED(tuner);
#endif
}

void btuner_qam_us_params_init(btuner_qam_us_params *us_param, btuner_t tuner)
{
    BSTD_UNUSED(tuner);
    BKNI_Memset(us_param, 0, sizeof(*us_param));
    us_param->mode = btuner_qam_us_mode_dvs178;
    us_param->symbol_rate = 128000;
    us_param->power_level = 0;
}

void btuner_qam_params_init(btuner_qam_params *qam, btuner_t tuner)
{
    BSTD_UNUSED(tuner);
    BKNI_Memset(qam, 0, sizeof(*qam));
    qam->mode = 64;
    qam->symbol_rate = 5056941;
    qam->annex = btuner_qam_annex_b;
    qam->spectrum = btuner_qam_spectral_inversion_auto;
    qam->wait_for_lock = true;
    qam->auto_acquire = true;
    qam->bandwidth = btuner_qam_bandwidth_6Mhz;
    qam->enable_power_measurement = true;
    qam->frequencyOffset = 150000;
}

void
btuner_sds_params_init(btuner_sds_params *sds, btuner_t tuner)
{
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendSatelliteSettings satSettings;
    BSTD_UNUSED(tuner);
    BKNI_Memset(sds, 0, sizeof(*sds));

    NEXUS_Frontend_GetDefaultSatelliteSettings(&satSettings);
    sds->mode = btuner_sds_mode_qpsk_dvb;
    sds->carrier_offset = satSettings.carrierOffset;
    sds->symbol_rate =satSettings.symbolRate;
    sds->spinv = satSettings.spectralInversion;
    sds->search_range = satSettings.searchRange;
    sds->diseqc_polarity = 13;
    sds->wait_for_lock = false;
    sds->ldpc_pilot = satSettings.ldpcPilot;
    sds->ldpc_pilot_pll = satSettings.ldpcPilotPll;
    sds->coderate.bits_per_symbol =    satSettings.codeRate.bitsPerSymbol;
    sds->coderate.denom = satSettings.codeRate.denominator;
    sds->coderate.num = satSettings.codeRate.numerator;
    sds->diseqc_tone = false;
#else
    BSTD_UNUSED(tuner);
    BKNI_Memset(sds, 0, sizeof(*sds));
#endif

}

void
btuner_vsb_params_init(btuner_vsb_params *vsb, btuner_t tuner)
{
    BSTD_UNUSED(tuner);
    BKNI_Memset(vsb, 0, sizeof(*vsb));
    vsb->mode = btuner_vsb_mode_vsb8;
    vsb->symbol_rate = 5381119;
    vsb->wait_for_lock = true;
}

void
btuner_analog_params_init(btuner_analog_params *analog, btuner_t tuner)
{
    BSTD_UNUSED(tuner);
    BKNI_Memset(analog, 0, sizeof(*analog));
    analog->video_format = bvideo_format_ntsc;
    analog->audio_rate = 48000;
    analog->btsc_mode = boutput_rf_btsc_mode_stereo;
    analog->comb_filter = true;
    analog->colorspace = btuner_analog_colorspace_yprpb;
}

void btuner_ofdm_params_init( btuner_ofdm_params *ofdm, btuner_t tuner )
{
    BSTD_UNUSED(tuner);
    BKNI_Memset(ofdm, 0, sizeof(*ofdm));
    ofdm->bandwidth = btuner_ofdm_bandwidth_8Mhz;
    ofdm->wait_for_lock = false;
    ofdm->mode = btuner_ofdm_mode_dvbt;
}

/* Must be static allocation. Settop API doesn't have btuner_linein_close. */
#define MAX_LINEINPUTS 8
struct btuner_linein g_lineinputs[MAX_LINEINPUTS];

btuner_linein_t btuner_linein_open(bobject_t id)
{
#if B_HAS_ANALOG || B_HAS_TRANSCODE
    NEXUS_PlatformConfiguration platformConfig;
    unsigned index = B_ID_GET_INDEX(id);
    btuner_linein_t linein;

#if B_HAS_TRANSCODE
    if (index >= MAX_LINEINPUTS+B_MAX_ENCODES) {
#else
    if (index >= MAX_LINEINPUTS) {
#endif
        BSETTOP_ERROR(berr_not_supported);
        return NULL;
    }

    linein = &g_lineinputs[index];
    BKNI_Memset(linein, 0, sizeof(*linein));

    NEXUS_Platform_GetConfiguration(&platformConfig);

    switch (index) {
#if B_HAS_ANALOG
    case 0: linein->composite = platformConfig.inputs.composite[0]; break;
    case 1: linein->composite = platformConfig.inputs.composite[1]; break;
    case 2: linein->composite = platformConfig.inputs.composite[2]; break;
    case 3: linein->composite = platformConfig.inputs.composite[3]; break;
    case 4: linein->svideo    = platformConfig.inputs.svideo[0]; break;
    case 5: linein->svideo    = platformConfig.inputs.svideo[1]; break;
    case 6: linein->component = platformConfig.inputs.component[0]; break;
    case 7: linein->component = platformConfig.inputs.component[1]; break;
#endif
#if B_HAS_TRANSCODE
    case 8 : linein->vdec7043DeviceNo = 0 ; break ;
    case 9 : linein->vdec7043DeviceNo = 1 ; break ;
#endif
    default:
        BSETTOP_ERROR(berr_not_supported); return NULL;
    }

    return linein;
#else
    BSTD_UNUSED(id);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
#endif
}

#if NEXUS_HAS_FRONTEND
static void btuner_p_lock_callback(void *context, int param)
{
    btuner_t tuner = (btuner_t)context;
    BDBG_OBJECT_ASSERT(tuner, btuner);
    BSTD_UNUSED(param);
    BDBG_MSG((" LOCK CALLBACK"));

    /* Reset FEC counters */
    NEXUS_Frontend_ResetStatus(tuner->nTuner);

    B_Event_Set(tuner->lockEvent);
    B_Event_Set(tuner->waitEvent);
}

static void btuner_p_lock_handler(void *context)
{
    btuner_t tuner = context;
    BDBG_OBJECT_ASSERT(tuner, btuner);
    if (tuner->lock_callback) {
        b_unlock();
        (*tuner->lock_callback)(tuner->callback_context);
        b_lock();
    }
}
#endif

#if NEXUS_HAS_FRONTEND
void btuner_p_set_dummy_mtsif_mapping(btuner_t tuner)
{
    NEXUS_ParserBand dummy = NEXUS_NUM_PARSER_BANDS-1;
    NEXUS_ParserBandSettings parserSettings;
    NEXUS_FrontendUserParameters userParams;
    NEXUS_Frontend_GetUserParameters(tuner->nTuner, &userParams);
    if (userParams.isMtsif) {
        /* map the frontend output to a dummy parserband, to avoid a warning message from nexus_frontend */
        NEXUS_ParserBand_GetSettings(dummy, &parserSettings);
        if (parserSettings.sourceTypeSettings.mtsif!=NULL) { /* if fake mapping already used, pick another. needed to support PIP */
            dummy = NEXUS_NUM_PARSER_BANDS-2;
            NEXUS_ParserBand_GetSettings(dummy, &parserSettings);
            if (parserSettings.sourceTypeSettings.mtsif!=NULL) {
                BDBG_WRN(("Unable to set dummy frontend<->PB %u mapping", (unsigned)dummy));
            }
        }
        parserSettings.sourceType = NEXUS_ParserBandSourceType_eMtsif;
        parserSettings.sourceTypeSettings.mtsif = NEXUS_Frontend_GetConnector(tuner->nTuner);
        NEXUS_ParserBand_SetSettings(dummy, &parserSettings);
        
        tuner->band->nFrontend = tuner->nTuner; /* this tells bsettop_stream that the parserband source is MTSIF */
    }
}
#endif

bband_t btuner_tune_qam(btuner_t tuner, unsigned freq, const btuner_qam_params *params)
{
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendQamSettings settings;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(tuner, btuner);
    BDBG_MSG(("QAM freq %d,symbol rate %d bandwidth %d",freq,params->symbol_rate,params->bandwidth));

    b_lock2idle();

    NEXUS_Frontend_GetDefaultQamSettings(&settings);
    switch(params->annex)
    {
    case btuner_qam_annex_a:
        settings.annex = NEXUS_FrontendQamAnnex_eA;
        break;
    case btuner_qam_annex_b:
        settings.annex = NEXUS_FrontendQamAnnex_eB;
        break;
    case btuner_qam_annex_c:
        settings.annex = NEXUS_FrontendQamAnnex_eC;
        break;
    default:
        BDBG_WRN(("unsupported annex %d,using annexB",params->annex));
        settings.annex = NEXUS_FrontendQamAnnex_eB;
    }

    switch(params->mode)
    {
    case btuner_qam_mode_16:
        settings.mode = NEXUS_FrontendQamMode_e16;
        break;
    case btuner_qam_mode_32:
        settings.mode = NEXUS_FrontendQamMode_e32;
        break;
    case btuner_qam_mode_64:
        settings.mode = NEXUS_FrontendQamMode_e64;
        break;
    case btuner_qam_mode_128:
        settings.mode = NEXUS_FrontendQamMode_e128;
        break;
    case btuner_qam_mode_256:
        settings.mode = NEXUS_FrontendQamMode_e256;
        break;
    case btuner_qam_mode_1024:
        settings.mode = NEXUS_FrontendQamMode_e1024;
        break;
    default:
        BDBG_ERR(("unsupported QAM %d",params->mode));
    }
    switch (params->spectrum)
    {
    case btuner_qam_spectral_inversion_auto: 
        settings.spectralInversion = NEXUS_FrontendQamSpectralInversion_eNormal;
        settings.spectrumMode = NEXUS_FrontendQamSpectrumMode_eAuto;
        break;
    case btuner_qam_spectral_inversion_noinverted: 
        settings.spectralInversion = NEXUS_FrontendQamSpectralInversion_eNormal;
        settings.spectrumMode = NEXUS_FrontendQamSpectrumMode_eManual;
        break;
    case btuner_qam_spectral_inversion_inverted: 
        settings.spectralInversion = NEXUS_FrontendQamSpectralInversion_eInverted;
        settings.spectrumMode = NEXUS_FrontendQamSpectrumMode_eManual;
        break;
    default:
        BDBG_ERR(("unsupported QAM spectrum setting %d",params->spectrum));
    }
    switch (params->bandwidth)
    {
    case btuner_qam_bandwidth_5Mhz:
        settings.bandwidth = 5000000; 
        break;
    case btuner_qam_bandwidth_6Mhz:
        settings.bandwidth = 6000000; 
        break;
    case btuner_qam_bandwidth_7Mhz:
        settings.bandwidth = 7000000; 
        break;
    case btuner_qam_bandwidth_8Mhz:
        settings.bandwidth = 8000000; 
        break;
    default:
        BDBG_ERR(("unsupported QAM bandwidth setting %d",params->bandwidth));
    }
    settings.frequency = freq;
    settings.symbolRate = params->symbol_rate;
    settings.autoAcquire = params->auto_acquire;
    settings.enablePowerMeasurement = params->enable_power_measurement;
    tuner->lock_callback = params->lock_callback;
    tuner->callback_context = params->callback_context;
    settings.lockCallback.callback = btuner_p_lock_callback;
    settings.lockCallback.context = tuner;
    settings.frequencyOffset = params->frequencyOffset;
    btuner_p_set_dummy_mtsif_mapping(tuner);

    NEXUS_StartCallbacks(tuner->nTuner);
    B_Event_Reset(tuner->waitEvent);
    rc = NEXUS_Frontend_TuneQam(tuner->nTuner, &settings);
    b_idle2lock();
    if (rc) {BERR_TRACE(rc);return NULL;}

    if (params->wait_for_lock) {
        b_unlock();
        rc = B_Event_Wait(tuner->waitEvent, 5000);
        b_lock();
        if (rc) {BERR_TRACE(rc);return NULL;}
    }

    return tuner->band;
#else
    BSTD_UNUSED(tuner);
    BSTD_UNUSED(freq);
    BSTD_UNUSED(params);
    return NULL;
#endif
}

bresult btuner_tune_oob(btuner_t tuner, unsigned freq, const btuner_qam_params *params)
{
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendOutOfBandSettings settings;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(tuner, btuner);
    BDBG_MSG(("OOB freq %d,symbol rate %d",freq,params->symbol_rate));

    b_lock2idle();

    NEXUS_Frontend_GetDefaultOutOfBandSettings(&settings);
    switch(params->annex)
    {
    case btuner_qam_oob_annex_a:
        settings.mode = NEXUS_FrontendOutOfBandMode_eDvs167Qpsk;
        break;
    case btuner_qam_oob_dvs178:
        settings.mode = NEXUS_FrontendOutOfBandMode_eDvs178Qpsk;
        break;
    case btuner_qam_oob_pod_annex_a:
        settings.mode = NEXUS_FrontendOutOfBandMode_ePod_Dvs167Qpsk;
        break;
    case btuner_qam_oob_pod_dvs178:
        settings.mode = NEXUS_FrontendOutOfBandMode_ePod_Dvs178Qpsk;
        break;
    default:
        BDBG_WRN(("unsupported annex %d",params->annex));
        return BSETTOP_ERROR(berr_invalid_parameter);
    }

    switch (params->spectrum) {
    case btuner_qam_spectral_inversion_auto:
        settings.spectrum = NEXUS_FrontendOutOfBandSpectrum_eAuto;
        break;
    case btuner_qam_spectral_inversion_inverted:
        settings.spectrum = NEXUS_FrontendOutOfBandSpectrum_eInverted;
        break;
    case btuner_qam_spectral_inversion_noinverted:
        settings.spectrum = NEXUS_FrontendOutOfBandSpectrum_eNonInverted;
        break;
    default:
        BDBG_WRN(("unsupported spectrum %d",params->spectrum));
        return BSETTOP_ERROR(berr_invalid_parameter);
    }
    settings.frequency = freq;
    settings.symbolRate = params->symbol_rate;

    tuner->lock_callback = params->lock_callback;
    tuner->callback_context = params->callback_context;
    settings.lockCallback.callback = btuner_p_lock_callback;
    settings.lockCallback.context = tuner;
    btuner_p_set_dummy_mtsif_mapping(tuner);

    NEXUS_StartCallbacks(tuner->nTuner);
    B_Event_Reset(tuner->waitEvent);
    rc = NEXUS_Frontend_TuneOutOfBand(tuner->nTuner, &settings);
    b_idle2lock();
    if (rc) {BERR_TRACE(rc);return berr_external_error;}

    /* OOB always waits for lock*/
    if ( 1 /*params->wait_for_lock*/)
    {
        b_unlock();
        rc = B_Event_Wait(tuner->waitEvent, 1000);
        b_lock();
        if (rc) {BERR_TRACE(rc);return berr_not_available;}
    }

    return b_ok;
#else
    BSTD_UNUSED(tuner);
    BSTD_UNUSED(freq);
    BSTD_UNUSED(params);
    return b_ok;
#endif
}

bresult btuner_tune_qam_us(btuner_t tuner, unsigned freq, const btuner_qam_us_params *params)
{
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendUpstreamSettings settings;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(tuner, btuner);
    BDBG_MSG(("OOB Upstream freq %d,symbol rate %d",freq,params->symbol_rate));

    b_lock2idle();

    NEXUS_Frontend_GetDefaultUpstreamSettings(&settings);


    switch (params->mode) {
    case btuner_qam_us_mode_annexa:
        settings.mode = NEXUS_FrontendUpstreamMode_eAnnexA;
        break;
    case btuner_qam_us_mode_dvs178:
        settings.mode = NEXUS_FrontendUpstreamMode_eDvs178;
        break;
    case btuner_qam_us_mode_pod_annexa :
        settings.mode = NEXUS_FrontendUpstreamMode_ePodAnnexA;
        break;
    case btuner_qam_us_mode_pod_dvs178:
        settings.mode = NEXUS_FrontendUpstreamMode_ePodDvs178;
        break;
    case btuner_qam_us_mode_docsis:
        settings.mode = NEXUS_FrontendUpstreamMode_eDocsis;
        break;
    default:
        return BSETTOP_ERROR(berr_not_supported);
    }

    settings.frequency = freq;
    settings.symbolRate = params->symbol_rate;
    settings.powerLevel = params->power_level;
    btuner_p_set_dummy_mtsif_mapping(tuner);

    NEXUS_StartCallbacks(tuner->nTuner);
    B_Event_Reset(tuner->waitEvent);
    rc = NEXUS_Frontend_TuneUpstream(tuner->nTuner, &settings);
    b_idle2lock();
    if (rc) {BERR_TRACE(rc);return berr_external_error;}

    return b_ok;
#else
    BSTD_UNUSED(tuner);
    BSTD_UNUSED(freq);
    BSTD_UNUSED(params);
    return b_ok;
#endif
}

#if NEXUS_HAS_FRONTEND
static void btuner_get_diseqc_reply(void *context, int param)
{
    uint8_t rcvBuf[3];
    size_t replyLen;
    NEXUS_FrontendDiseqcMessageStatus status;
    btuner_t tuner = (btuner_t)context;

    BSTD_UNUSED(param);

    NEXUS_Frontend_GetDiseqcReply(tuner->nTuner, &status, &rcvBuf[0], sizeof(rcvBuf), &replyLen);

    if (status == NEXUS_FrontendDiseqcMessageStatus_eSuccess)
    {
        size_t i;
        for (i=0; i<replyLen; i++)
        {
            BDBG_WRN(("%d 0x%02x\n", i, *(rcvBuf+i)));
        }
    }
    else
    {
        BDBG_ERR(("btuner_get_diseqc_reply, status=%d", status));
    }
}
#endif

bband_t btuner_tune_sds(btuner_t tuner, unsigned freq, const btuner_sds_params *params)
{
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendSatelliteSettings satSettings;
    NEXUS_FrontendDiseqcSettings diseqcSettings;
    NEXUS_Error rc;
    int i=1;
    NEXUS_FrontendSatelliteStatus satStatus;

    BDBG_OBJECT_ASSERT(tuner, btuner);

    b_lock2idle();

    BKNI_Memset(&diseqcSettings,0,sizeof(NEXUS_FrontendDiseqcSettings));
    NEXUS_Frontend_GetDiseqcSettings(tuner->nTuner, &diseqcSettings);
    diseqcSettings.toneEnabled = params->diseqc_tone;
    diseqcSettings.voltage = (params->diseqc_polarity == btuner_sds_diseqc_polarity_13v)?NEXUS_FrontendDiseqcVoltage_e13v:NEXUS_FrontendDiseqcVoltage_e18v;
    NEXUS_Frontend_SetDiseqcSettings(tuner->nTuner, &diseqcSettings);

    #if 0
    /* Example of sending and receiving disecq message */
    {
        const uint8_t sendBuf[3] = {0xe2, 0x00, 0x04};
        NEXUS_CallbackDesc send_disecq_msg_cb_desc;
        send_disecq_msg_cb_desc.callback = btuner_get_diseqc_reply;
        send_disecq_msg_cb_desc.context = tuner;
        NEXUS_Frontend_SendDiseqcMessage(tuner->nTuner, &sendBuf[0], 3, &send_disecq_msg_cb_desc);
    }
    #else
    BSTD_UNUSED(btuner_get_diseqc_reply);
    #endif

    NEXUS_Frontend_GetDefaultSatelliteSettings(&satSettings);
    satSettings.carrierOffset = params->carrier_offset;
    satSettings.codeRate.bitsPerSymbol = params->coderate.bits_per_symbol;
    satSettings.codeRate.denominator = params->coderate.denom;
    satSettings.codeRate.numerator = params->coderate.num;
    satSettings.frequency = freq;
    satSettings.ldpcPilot = params->ldpc_pilot;
    if(satSettings.ldpcPilot)
        satSettings.ldpcPilotPll = params->ldpc_pilot_pll;
    satSettings.mode = b_sdsmode2nexus(params->mode);

    satSettings.searchRange = params->search_range;
    satSettings.spectralInversion = params->spinv;
    satSettings.symbolRate = params->symbol_rate;


    tuner->lock_callback = params->lock_callback;
    tuner->callback_context = params->callback_context;
    satSettings.lockCallback.callback = btuner_p_lock_callback;
    satSettings.lockCallback.context = tuner;
    btuner_p_set_dummy_mtsif_mapping(tuner);

    NEXUS_StartCallbacks(tuner->nTuner);
    B_Event_Reset(tuner->waitEvent);
    rc = NEXUS_Frontend_TuneSatellite(tuner->nTuner, &satSettings);
    b_idle2lock();
    if (rc) {BERR_TRACE(rc);return NULL;}

    /* Wait for up to 1 sec for tune */
    for (i= 0; i < 20; i++)
    {
        BKNI_Memset(&satStatus, 0, sizeof(NEXUS_FrontendSatelliteStatus));
        NEXUS_Frontend_GetSatelliteStatus(tuner->nTuner, &satStatus);
        BDBG_MSG(("Sat Status locked  %d",(int)satStatus.demodLocked));
        if(satStatus.demodLocked)
            break;
        else
            BKNI_Sleep(50);
    }

#if 0 /* PR42052: Callback not triggered by AST PI*/
    if (params->wait_for_lock)
    {
        b_unlock();
        rc = B_Event_Wait(tuner->waitEvent, 1000);
        b_lock();
        if (rc) {BERR_TRACE(rc);return NULL;}
    }
#endif

    return tuner->band;
#else
    BSTD_UNUSED(tuner);
    BSTD_UNUSED(freq);
    BSTD_UNUSED(params);
    return NULL;
#endif
}

bband_t btuner_tune_vsb(btuner_t tuner, unsigned freq, const btuner_vsb_params *params)
{
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendVsbSettings settings;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(tuner, btuner);

    b_lock2idle();

    NEXUS_Frontend_GetDefaultVsbSettings(&settings);
    settings.mode = (params->mode == 8) ? NEXUS_FrontendVsbMode_e8 : NEXUS_FrontendVsbMode_e16;
    settings.frequency = freq;
    settings.ifFrequency = params->if_freq;
    settings.symbolRate = params->symbol_rate;
    settings.ifFrequencyOffset = params->if_freq_offset;
    settings.symbolRateOffset = params->symbol_rate_offset;
    settings.ntscOffset = params->ntsc_offset;

    tuner->lock_callback = params->lock_callback;
    tuner->callback_context = params->callback_context;
    settings.lockCallback.callback = btuner_p_lock_callback;
    settings.lockCallback.context = tuner;
    btuner_p_set_dummy_mtsif_mapping(tuner);

    NEXUS_StartCallbacks(tuner->nTuner);
    B_Event_Reset(tuner->waitEvent);
    rc = NEXUS_Frontend_TuneVsb(tuner->nTuner, &settings);
    b_idle2lock();
    if (rc) {BERR_TRACE(rc);return NULL;}

    if (params->wait_for_lock) {
        b_unlock();
        rc = B_Event_Wait(tuner->waitEvent, 1000);
        b_lock();
        if (rc) {BERR_TRACE(rc);return NULL;}
    }

    return tuner->band;
#else
    BSTD_UNUSED(tuner);
    BSTD_UNUSED(freq);
    BSTD_UNUSED(params);
    return NULL;
#endif
}

#if B_HAS_ANALOG
bresult btuner_p_set_analog_input(NEXUS_AnalogVideoDecoderHandle analogVideoDecoder, btuner_t tuner, btuner_linein_t linein)
{
    NEXUS_Error rc;
    NEXUS_AnalogVideoDecoderSettings analogVideoDecoderSettings;
    BDBG_MSG(("btuner_p_set_analog_input %p %p %p", analogVideoDecoder, tuner, linein));
    if (linein) {
        if (linein->composite) {
            NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_CompositeInput_GetConnector(linein->composite), &analogVideoDecoderSettings);
        }
        else if (linein->component) {
            NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_ComponentInput_GetConnector(linein->component), &analogVideoDecoderSettings);
        }
        else if (linein->svideo) {
            NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_SvideoInput_GetConnector(linein->svideo), &analogVideoDecoderSettings);
        }
        else {
            return BERR_TRACE(berr_not_available);
        }
    }
    else if (tuner->nTuner) {
        NEXUS_AnalogVideoDecoder_GetDefaultSettingsForVideoInput(NEXUS_Frontend_GetAnalogVideoConnector(tuner->nTuner), &analogVideoDecoderSettings);
    }
    else {
        return BERR_TRACE(berr_not_available);
    }

    rc = NEXUS_AnalogVideoDecoder_SetSettings(analogVideoDecoder, &analogVideoDecoderSettings);
    if (rc) return BERR_TRACE(rc);
    return 0;
}
#endif

bstream_t btuner_tune_rf(btuner_t tuner, unsigned carrierfreq, const btuner_analog_params *params)
{
#if B_HAS_ANALOG || B_HAS_EXTERNAL_ANALOG
    bstream_t stream = &tuner->analog.stream;
    BKNI_Memset(stream, 0, sizeof(tuner->analog.stream));
    BDBG_OBJECT_SET(stream, bstream);
    stream->producer.tuner = tuner;
    BDBG_MSG(("freq=%d",carrierfreq));
    stream->producer.linein = NULL;
    BSTD_UNUSED(carrierfreq); /* TODO */
    tuner->analog.params = *params;
    tuner->analog.stream.static_alloc=true;
    return &tuner->analog.stream;
#else
    BSTD_UNUSED(tuner);
    BSTD_UNUSED(carrierfreq);
    BSTD_UNUSED(params);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
#endif
}

bstream_t btuner_tune_linein(btuner_t tuner, btuner_linein_t linein, const btuner_analog_params *params)
{
#if B_HAS_ANALOG || B_HAS_TRANSCODE
    bstream_t stream = &tuner->analog.stream;
    BKNI_Memset(&tuner->analog.stream, 0, sizeof(tuner->analog.stream));
    BDBG_OBJECT_SET(stream, bstream);

    stream->producer.tuner = tuner;
    stream->producer.linein = linein;
    tuner->analog.params = *params;

#if B_HAS_TRANSCODE
    {
        NEXUS_Transcoder_VdecSettings vdecSettings ;
        NEXUS_Transcoder_ConfigureVdec(linein->vdec7043DeviceNo, &vdecSettings ) ;
    }
#endif

    return stream;

#else
    BSTD_UNUSED(tuner);
    BSTD_UNUSED(linein);
    BSTD_UNUSED(params);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
#endif
}

bband_t btuner_tune_ofdm( btuner_t tuner, unsigned freq, const btuner_ofdm_params* params )
{
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendOfdmSettings settings;
    NEXUS_Error rc;

    BDBG_OBJECT_ASSERT(tuner, btuner);

    BDBG_MSG(("OFDM freq %d,bw=%dMhz,cci %s,pri %s, mode %s", freq,params->bandwidth,
              params->cci_mode==btuner_ofdm_cci_mode_none?"off":"auto",params->priority==btuner_ofdm_priority_high?"high":"low",
              params->mode==btuner_ofdm_mode_dvbt?"dvbt":"isdbt"));

    b_lock2idle();

    NEXUS_Frontend_GetDefaultOfdmSettings(&settings);

    settings.bandwidth = params->bandwidth*1000000;
    settings.cciMode = params->cci_mode;
    settings.frequency = freq;
    settings.priority = params->priority;
    settings.mode = params->mode;

    tuner->lock_callback = params->lock_callback;
    tuner->callback_context = params->callback_context;
    settings.lockCallback.callback = btuner_p_lock_callback;
    settings.lockCallback.context = tuner;
    btuner_p_set_dummy_mtsif_mapping(tuner);
    
    NEXUS_StartCallbacks(tuner->nTuner);
    B_Event_Reset(tuner->waitEvent);
    rc = NEXUS_Frontend_TuneOfdm(tuner->nTuner, &settings);
    b_idle2lock();
    if (rc) {BERR_TRACE(rc);return NULL;}

    if (params->wait_for_lock) {
        b_unlock();
        rc = B_Event_Wait(tuner->waitEvent, 2500);
        b_lock();
        if (rc) {BERR_TRACE(rc);return NULL;}
    }

    return tuner->band;
#else
    BSTD_UNUSED(tuner);
    BSTD_UNUSED(freq);
    BSTD_UNUSED(params);
    return NULL;
#endif

}

bresult btuner_get_analog_status(btuner_t tuner, btuner_analog_status *status)
{
#if B_HAS_TRANSCODE
    BDBG_OBJECT_ASSERT(tuner, btuner);
    BSTD_UNUSED(status);
    return b_ok ;
#else
    BDBG_OBJECT_ASSERT(tuner, btuner);
    BSTD_UNUSED(status);
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

bresult btuner_get_qam_status(btuner_t tuner, btuner_qam_status *status)
{
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendQamStatus nStatus;
    NEXUS_FrontendOutOfBandStatus nStatus_oob;
    NEXUS_FrontendCapabilities caps;
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(tuner, btuner);

    BKNI_Memset(status, 0, sizeof(*status));

    NEXUS_Frontend_GetCapabilities(tuner->nTuner, &caps);

    b_lock2idle();
    if (caps.qam)
    {
        rc = NEXUS_Frontend_GetQamStatus(tuner->nTuner, &nStatus);
    }
    else
        rc = NEXUS_Frontend_GetOutOfBandStatus(tuner->nTuner, &nStatus_oob);
    b_idle2lock();
    if (rc) return BERR_TRACE(rc);

    if (caps.qam)
    {
        status->ext_agc_level = nStatus.rfAgcLevel;
        status->fec_corr_cnt = nStatus.fecCorrected;
        status->fec_lock = nStatus.fecLock;
        status->fec_uncorr_cnt = nStatus.fecUncorrected;
        status->freq_er2 = nStatus.carrierPhaseOffset/1000;
        status->freq_err = nStatus.carrierFreqOffset/1000;
        status->int_agc_level = nStatus.ifAgcLevel;
        status->qam_lock = nStatus.receiverLock;
        status->snr_estimate = (nStatus.snrEstimate * 256)/100;
        status->symbol_rate = nStatus.symbolRate;
        status->ber_estimate = nStatus.berEstimate;
        status->dsChannelPower = nStatus.dsChannelPower;
        status->mainTap = nStatus.mainTap;
        status->equalizerGain = nStatus.equalizerGain;
        status->postRsBer = nStatus.postRsBer;
        status->postRsBerElapsedTime = nStatus.postRsBerElapsedTime;
        status->dsChannelPower = nStatus.dsChannelPower;
        status->fec_clean_cnt = nStatus.fecClean;
        status->annex = nStatus.settings.annex;
    }
    else
    {
        status->ext_agc_level = nStatus_oob.agcExtLevel;
        status->fec_corr_cnt = nStatus_oob.correctedCount;
        status->fec_lock = nStatus_oob.isFecLocked;
        status->fec_uncorr_cnt = nStatus_oob.uncorrectedCount;
        status->freq_er2 = nStatus_oob.carrierPhaseOffset/1000;
        status->freq_err = nStatus_oob.carrierFreqOffset/1000;
        status->int_agc_level = nStatus_oob.agcIntLevel;
        status->qam_lock = nStatus_oob.isQamLocked;
        status->snr_estimate = (nStatus_oob.snrEstimate*256)/100;
        status->symbol_rate = nStatus_oob.symbolRate;
    }

    return 0;
#else
    BSTD_UNUSED(tuner);
    BKNI_Memset(status, 0, sizeof(*status));
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

bresult btuner_get_qam_us_status(btuner_t tuner, btuner_qam_us_status *status)
{
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendUpstreamStatus nStatus;
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(tuner, btuner);

    BKNI_Memset(status, 0, sizeof(*status));

    b_lock2idle();
    rc = NEXUS_Frontend_GetUpstreamStatus(tuner->nTuner, &nStatus);
    b_idle2lock();
    if (rc) return BERR_TRACE(rc);

    switch (nStatus.mode) {
        case NEXUS_FrontendUpstreamMode_ePodAnnexA:
            status->mode = btuner_qam_us_mode_pod_annexa;
            break;
        case NEXUS_FrontendUpstreamMode_ePodDvs178:
            status->mode = btuner_qam_us_mode_pod_dvs178;
            break;
        case NEXUS_FrontendUpstreamMode_eAnnexA :
            status->mode = btuner_qam_us_mode_annexa;
            break;
        case NEXUS_FrontendUpstreamMode_eDvs178:
            status->mode = btuner_qam_us_mode_dvs178;
            break;
        case NEXUS_FrontendUpstreamMode_eDocsis:
            status->mode = btuner_qam_us_mode_docsis;
            break;
        default:
            return BSETTOP_ERROR(berr_invalid_parameter);
    }

    status->power_level = nStatus.powerLevel;
    status->symbol_rate = nStatus.symbolRate;
    status->freq = nStatus.frequency;

    return b_ok;
#else
    BSTD_UNUSED(tuner);
    BKNI_Memset(status, 0, sizeof(*status));
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

#if NEXUS_FRONTEND_7346
#include "nexus_frontend_7346.h"
#endif
bresult btuner_get_sds_status(btuner_t tuner, btuner_sds_status *status)
{
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendSatelliteStatus nStatus;
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(tuner, btuner);

    BKNI_Memset(status, 0, sizeof(*status));

    b_lock2idle();
    rc = NEXUS_Frontend_GetSatelliteStatus(tuner->nTuner, &nStatus);
    b_idle2lock();
    if (rc) return BERR_TRACE(rc);

    status->pre_viterbi_error_count = nStatus.preViterbiErrorCount;
    status->ber_errors =  nStatus.berErrorCount;
    status->ber_estimate = nStatus.berEstimate;
    status->bert_locked = nStatus.bertLocked;
    status->carrier_error = nStatus.carrierError;
    status->sample_frequency = nStatus.sampleClock;
    status->coderate.bits_per_symbol = nStatus.codeRate.bitsPerSymbol;
    status->coderate.denom = nStatus.codeRate.denominator;
    status->coderate.num = nStatus.codeRate.numerator;
    status->fec_phase = nStatus.fecPhase;
    {
        /* These values are actually only valid for 4501 */
        /* Other frontends need to call the appropriate NEXUS_Frontend_GetXXXXLnaStatus()
         * function for LNA status information.  (See below for one example.)
         */
        status->lnaStatus.int_config = nStatus.lnaStatus.intConfig;
        status->lnaStatus.ext_config = nStatus.lnaStatus.extConfig;
        status->lnaStatus.version = nStatus.lnaStatus.version;
        status->lnaStatus.agc0 = nStatus.lnaStatus.agc0;
        status->lnaStatus.agc1 = nStatus.lnaStatus.agc1;
    }
    status->tuner_freq = nStatus.settings.frequency;
    status->channel = nStatus.channel;
    status->if_agc = nStatus.ifAgc;
    status->rf_agc = nStatus.rfAgc;
    status->output_bit_rate = nStatus.outputBitRate;
    status->fec_corr = nStatus.fecCorrected;
    status->fec_uncorr = nStatus.fecUncorrected;
    status->fec_clean = nStatus.fecClean;
    status->rs_locked = nStatus.demodLocked;
    status->vit_locked = nStatus.demodLocked;
    /* status->rs_uncorr = nStatus. */
    /* status->sample_frequency =  */
    status->snr_estimate = (nStatus.snrEstimate * 256)/100;
    status->spectral_inversion = nStatus.spectralInversion;
    /* status->vit_locked = nStatus. */
    /* status-> */
    status->agf = nStatus.agf;
#if NEXUS_FRONTEND_7346
    {
        /* re-use existing agc locations in the settop struct for the difference in this chip */
        NEXUS_7346LnaStatus lnaStatus;
        rc = NEXUS_Frontend_Get7346LnaStatus(tuner->nTuner, &lnaStatus);
        if (rc) return BERR_TRACE(rc);
        status->lnaStatus.agc0 = lnaStatus.agc;
        status->lnaStatus.agc1 = lnaStatus.baseBandAgc;
    }
#endif

    return 0;
#else
    BSTD_UNUSED(tuner);
    BKNI_Memset(status, 0, sizeof(*status));
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

bresult btuner_get_vsb_status(btuner_t tuner, btuner_vsb_status *status)
{
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendVsbStatus nStatus;
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(tuner, btuner);

    BKNI_Memset(status, 0, sizeof(*status));
    BKNI_Memset(&nStatus, 0, sizeof(nStatus));

    b_lock2idle();
    rc = NEXUS_Frontend_GetVsbStatus(tuner->nTuner, &nStatus);
    b_idle2lock();
    if (rc) return BERR_TRACE(rc);

    status->mode = nStatus.settings.mode;
    status->receiver_lock = nStatus.receiverLock;
    status->fec_lock = nStatus.fecLock;
    status->opll_lock = nStatus.opllLock;
    status->symbol_rate = nStatus.symbolRate;
    status->symbol_rate_offset = nStatus.settings.symbolRateOffset;
    status->symbol_rate_error = nStatus.symbolRateError;
    status->if_freq = nStatus.ifFrequency;
    status->if_freq_offset = nStatus.settings.ifFrequencyOffset;
    status->if_freq_error = nStatus.ifFrequencyError;
    status->ntsc_offset  = nStatus.settings.ntscOffset;
    status->ntsc_error = 0;
    status->int_agc_level = nStatus.ifAgcLevel;
    status->ext_agc_level = nStatus.rfAgcLevel;
    status->snr_estimate = (nStatus.snrEstimate * 256)/100;
    status->blk_corr = nStatus.fecCorrected;
    status->blk_uncorr = nStatus.fecUncorrected;
    status->err_corr = nStatus.bitErrCorrected;

    return 0;
#else
    BSTD_UNUSED(tuner);
    BKNI_Memset(status, 0, sizeof(*status));
    return BSETTOP_ERROR(berr_not_supported);
#endif

}

bresult btuner_get_soft_decisions(btuner_t tuner, btuner_soft_decision *data, size_t length)
{
#if B_HAS_EXTERNAL_ANALOG || !NEXUS_HAS_FRONTEND
    BSTD_UNUSED(tuner);
    BSTD_UNUSED(data);
    BSTD_UNUSED(length);
    return b_ok;
#else
    unsigned i;
    NEXUS_Error rc;
    NEXUS_FrontendSoftDecision *pSoftDecisions;
    unsigned num;

    BDBG_OBJECT_ASSERT(tuner, btuner);

    BDBG_MSG(("Reading %d soft decisions", length));

    pSoftDecisions = BKNI_Malloc(length*sizeof(NEXUS_FrontendSoftDecision));
    if ( NULL == pSoftDecisions )
    {
        return BSETTOP_ERROR(berr_out_of_memory);
    }

    b_lock2idle();
    rc = NEXUS_Frontend_ReadSoftDecisions(tuner->nTuner, pSoftDecisions, length, &num);
    b_idle2lock();
    if ( rc )
    {
        BKNI_Memset(data, 0, length*sizeof(btuner_soft_decision));
        BKNI_Free(pSoftDecisions);
        return BSETTOP_ERROR(berr_external_error);
    }

    for ( i = 0; i < num; i++ )
    {
        data[i].i = pSoftDecisions[i].i;
        data[i].q = pSoftDecisions[i].q;
    }

    BKNI_Free(pSoftDecisions);
    return b_ok;
#endif
}

bresult btuner_get_ofdm_status( btuner_t tuner, btuner_ofdm_status *status )
{
#if NEXUS_HAS_FRONTEND
    /* coverity[stack_use_local_overflow] */
    NEXUS_FrontendOfdmStatus nStatus;
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(tuner, btuner);

    BKNI_Memset(status, 0, sizeof(*status));

    rc = NEXUS_Frontend_GetOfdmStatus(tuner->nTuner, &nStatus);
    if (rc) return BERR_TRACE(rc);

    status->acq_count = nStatus.reacquireCount;
    status->blk_clean = nStatus.fecCleanBlocks;
    status->blk_corr = nStatus.fecCorrectedBlocks;
    status->blk_uncorr = nStatus.fecUncorrectedBlocks;
    status->carrier_offset = nStatus.carrierOffset;
    status->fec_lock = nStatus.fecLock;
    status->receiver_lock = nStatus.receiverLock;
    status->snr = nStatus.snr;
    status->timing_offset = nStatus.timingOffset;
    status->signal_strength = nStatus.signalStrength;

    return 0;
#else
    BSTD_UNUSED(tuner);
    BKNI_Memset(status, 0, sizeof(*status));
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

bresult btuner_get_fast_status(btuner_t tuner, btuner_fast_status *status )
{
#if NEXUS_HAS_FRONTEND
    NEXUS_FrontendFastStatus n_status;
    NEXUS_Error rc;
    BDBG_OBJECT_ASSERT(tuner, btuner);

    BKNI_Memset(status, 0, sizeof(*status));

    rc = NEXUS_Frontend_GetFastStatus(tuner->nTuner, &n_status);
    if (rc) {rc = BERR_TRACE(rc); goto done;}

    status->acquire_in_progress = n_status.acquireInProgress;
    status->lock_status = n_status.lockStatus;
done:
    return rc;
#else
	BSTD_UNUSED(tuner);
    BKNI_Memset(status, 0, sizeof(*status));
    return BSETTOP_ERROR(berr_not_supported);
#endif
}

