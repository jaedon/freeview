/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_config.c $
 * $brcm_Revision: 66 $
 * $brcm_Date: 7/12/12 4:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_config.c $
 * 
 * 66   7/12/12 4:03p erickson
 * SW7405-5488: use NEXUS_GetDisplayCapabilities
 * 
 * 65   5/3/12 4:46p jgarrett
 * SW7425-2802: Adding dts_transcode/ac3_transcode options to bconfig
 * 
 * 64   5/2/12 6:46p jgarrett
 * SW7425-2802: Removing RAP_XXX build variables for audio post processing
 * in favor of runtime detection
 * 
 * 63   4/25/12 12:15p sgadara
 * SW7358-272: [7358] Support AVL on Brutus
 * 
 * 62   3/20/12 1:59p mward
 * SW7435-7:  Limit number of playback objects to 13 to prevent a run-time
 * warning until there is a requirement for more.
 * 
 * 61   3/19/12 6:30p erickson
 * SW7425-2664: remove board/chip logic. rely on
 * NEXUS_Platform_GetConfiguration.
 * 
 * 60   2/24/12 12:10p jtna
 * SW7425-2387: coverity defect STACK_USE
 * 
 * 59   6/5/11 8:51p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
 * Dynamically Populate frontend tuners.
 * 
 * 58   6/1/11 8:43p xhuang
 * SW7552-34: Bringup frontend in Brutus
 * 
 * 57   5/27/11 9:41p vishk
 * SW7231-32: Implement the Nexus code to support the 3461 frontend chip.
 * Implement brutus support for the downstream core.
 * 
 * 56   3/9/11 5:41p katrep
 * SW7231-30:add qam tuner
 * 
 * 55   2/15/11 7:15p xhuang
 * SW7358-34: Add 7358 support
 * 
 * 54   2/7/11 11:26a nickh
 * SW7420-1078: Allow full search of 2HD buffers on both memory
 * controllers
 * 
 * 53   12/29/10 11:33a erickson
 * SW7420-1078: rework logic to detect max HD format. take into account
 * all possible hd and fullHd buffer settings.
 *
 * 52   12/15/10 10:10a jrubio
 * SW7344-9: add 7344/7346 support
 *
 * 51   11/22/10 1:01p spothana
 * SW7420-1078: assign heap index per video window and per graphics frame
 * buffer
 *
 * 50   11/18/10 6:05p nickh
 * SW7422-80: Add support for increased input bands and playbacks for 7422
 *
 * 49   4/23/10 6:20p mward
 * SW7125-375: Set correct (zero) number of streamers for 97019.
 *
 * 48   4/23/10 2:35p erickson
 * SW3548-2904: add B3 support
 *
 * 47   4/22/10 11:37a nitinb
 * SW7550-339: Enable "channel edit" functionality in Brutus for OFDM
 * input
 *
 * 46   4/12/10 7:34p nitinb
 * SW7550-339: Enable "channel edit" functionality in Brutus for OFDM
 * input
 *
 * 44   3/11/10 3:12p mward
 * SW7405-3965: deprecrate NEXUS_VideoDecoderModuleSettings.numDecodes,
 * etc.
 *
 * 43   1/18/10 4:27p randyjew
 * SW7468-6:Limit 97468 boards to 480p
 *
 * 42   12/9/09 5:19p randyjew
 * SW7468-6: Add 7468/7208 support
 *
 * 41   12/9/09 12:03p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 40   10/15/09 4:34p erickson
 * SWDEPRECATED-3881: use NEXUS_PlatformConfiguration members to learn #
 * of displays and windows available based on memory strapping options
 *
 * 39   9/11/09 11:32a jrubio
 * SW7342-11: add 8 input bands
 *
 * 38   9/3/09 3:35p mward
 * SW7125-4:  Use additional "streamers" to select input bands for QAM DS
 * set up with BBS script.
 *
 * 37   9/1/09 10:51a mward
 * SW7125-4: Disable SW streamer select for now, use DIP switch.
 *
 * 36   8/26/09 11:57a mward
 * SW7125-4: Configure number of streamers for 7125 boards.
 *
 * 35   8/19/09 10:58a jrubio
 * PR55232: add 7342/7340 support
 *
 * 34   3/20/09 1:14p katrep
 * PR52340: Support srs true volume and dolby volume through brutus ans
 * settop api
 *
 *
 * 33   3/18/09 12:31p erickson
 * PR53340: add 1080p 50hz support
 *
 * 32   3/16/09 3:22p erickson
 * PR53267: convert bconfig_system to programmatic init
 *
 * 31   3/5/09 3:19p katrep
 * PR52599: More support for 7205
 *
 * 30   2/20/09 1:44p erickson
 * PR52355: add ANR support
 *
 * 29   2/18/09 6:22p jrubio
 * PR52261: 7335 has 2 possible inputs ASI and LVDS. LVDS not working on
 * P3 > boards
 *
 * 28   2/11/09 3:32p vle
 * PR 51414, PR 45656: Add 1080p/60Hz support in Brutus and enable for
 * 7420
 *
 * 27   2/3/09 11:08a mward
 * PR39972: Support E0 chip version.
 *
 * 26   1/16/09 4:40p katrep
 * PR51215: VESA should be enabled only for HD modes
 *
 * 25   12/19/08 7:39p katrep
 * PR50567: Add Support for 97466 platform
 *
 * 24   9/26/08 12:08p katrep
 * PR46457: 1080p 60 Hz not supported
 *
 * 23   9/2/08 7:44p katrep
 * PR46457: Configure settop at run time, based on resources exposed by
 * nexus platfrom.
 *
 * 22   8/22/08 7:46p erickson
 * PR45795: use NEXUS_DisplayModule_GetSettings to determine 1080p
 * capabilities
 *
 * 21   8/14/08 11:46a jgarrett
 * PR 39042: Adding PCM capture APIs
 *
 * 20   7/17/08 4:34p shyam
 * PR43426 : Add 7043 656 Encode support
 *
 * 19   7/9/08 4:10p katrep
 * PR43490: Add support 656/i2s tuning using analog tune api.
 *
 * 18   6/27/08 3:48p mphillip
 * PR42901: Enable PVR encryption in bconfig
 *
 * 17   6/25/08 1:23p erickson
 * PR36068: add 93549 and 93556
 *
 * 16   6/11/08 11:53a jgarrett
 * PR 43489: Adding brutus support for 93549/93556
 *
 * 15   5/28/08 8:02p jrubio
 * PR43087: add TUNER_ENABLE_7335 flags
 *
 * 14   5/2/08 1:54p jrubio
 * PR42353: add 7335
 *
 * 13   3/25/08 7:43p vsilyaev
 * PR 40862: Use nexus configuration to set supported formats
 *
 * 12   3/18/08 4:02p erickson
 * PR40307: implemented still decode
 *
 * 11   3/17/08 5:39p katrep
 * PR40021:Add support for OFDM tuner on 7405
 *
 * 10   3/12/08 3:17p jgarrett
 * PR 40017: Fixing size of mosaic array
 *
 * 9   3/6/08 4:23p jrubio
 * PR40169: add 7325
 *
 * 8   2/4/08 10:07a erickson
 * PR36068: api update
 *
 * 7   2/4/08 10:00a erickson
 * PR36068: update
 *
 * 6   11/27/07 11:16p erickson
 * PR36068: add 3563 frontend
 *
 * 5   11/27/07 9:19a erickson
 * PR37590: allow no B_MAX_STILL_DECODES
 *
 * 4   11/12/07 5:26p erickson
 * PR36802: limit # of records
 *
 * 3   11/12/07 2:34p erickson
 * PR36068: update
 *
 * 2   10/16/07 12:35p erickson
 * PR36068: brutus up over settop api/nexus
 *
 * 1   10/15/07 2:35p erickson
 * PR36068: initial
 *
 ***************************************************************************/
#include "bsettop_impl.h"
#include "nexus_audio.h"
#include "nexus_transport_capabilities.h"

BDBG_MODULE(config);

static const bobject_t
    b_obj_1[2] = {B_ID(0),NULL},
    b_obj_2[2] = {B_ID(1),NULL}, /* only 1 bobject, but starting at 1 */
    b_obj_1_2[3] = {B_ID(0), B_ID(1),NULL},
    b_obj_1_2_3[4] = {B_ID(0), B_ID(1), B_ID(2),NULL},
    b_obj_1_2_3_4[5] = {B_ID(0), B_ID(1), B_ID(2), B_ID(3), NULL},
    b_obj_1_2_3_4_5[6] = {B_ID(0), B_ID(1), B_ID(2), B_ID(3), B_ID(4), NULL},
    b_obj_1_2_3_4_5_6[7] = {B_ID(0), B_ID(1), B_ID(2), B_ID(3), B_ID(4), B_ID(5), NULL},
    b_obj_1_2_3_4_5_6_7[8] = {B_ID(0), B_ID(1), B_ID(2), B_ID(3), B_ID(4), B_ID(5), B_ID(6), NULL},
    b_obj_1_2_3_4_5_6_7_8[9] = {B_ID(0), B_ID(1), B_ID(2), B_ID(3), B_ID(4), B_ID(5), B_ID(6),B_ID(7), NULL},
    b_obj_1_2_3_4_5_6_7_8_9[10] = {B_ID(0), B_ID(1), B_ID(2), B_ID(3), B_ID(4), B_ID(5), B_ID(6),B_ID(7), B_ID(8), NULL},
    b_obj_1_2_3_4_5_6_7_8_9_10[11] = {B_ID(0), B_ID(1), B_ID(2), B_ID(3), B_ID(4), B_ID(5), B_ID(6),B_ID(7), B_ID(8), B_ID(9), NULL},
    b_obj_1_2_3_4_5_6_7_8_9_10_11[12] = {B_ID(0), B_ID(1), B_ID(2), B_ID(3), B_ID(4), B_ID(5), B_ID(6),B_ID(7), B_ID(8), B_ID(9), B_ID(10), NULL},
    b_obj_1_2_3_4_5_6_7_8_9_10_11_12[13] = {B_ID(0), B_ID(1), B_ID(2), B_ID(3), B_ID(4), B_ID(5), B_ID(6),B_ID(7), B_ID(8), B_ID(9), B_ID(10), B_ID(11), NULL},
    b_obj_1_2_3_4_5_6_7_8_9_10_11_12_13[14] = {B_ID(0), B_ID(1), B_ID(2), B_ID(3), B_ID(4), B_ID(5), B_ID(6),B_ID(7), B_ID(8), B_ID(9), B_ID(10), B_ID(11), B_ID(12), NULL},
    b_obj_5_6_7_8[5] = {B_ID(5), B_ID(6), B_ID(7), B_ID(8),NULL},
    b_obj_8[9] = {B_ID(0), B_ID(1), B_ID(2), B_ID(3), B_ID(4), B_ID(5), B_ID(6), B_ID(7),NULL};

static bconfig_system bconfig_;

const bconfig_system * const bconfig = &bconfig_;
void bconfig_p_add_pvr(void);

#ifndef B_MAX_STILL_DECODES
#define B_MAX_STILL_DECODES 0
#endif

static const bconfig_obj_list b_pcm = {"pcm_playback", 2, b_obj_1_2};
#if B_MAX_STILL_DECODES > 0
static const bobject_t
    b_decode_still_list[] = {
        B_ID(B_MAX_DECODES)
#if B_MAX_STILL_DECODES>1
        ,B_ID(B_MAX_DECODES+1)
#endif
        ,NULL};
static const bconfig_obj_list b_decode_still = {"decode_still", B_MAX_STILL_DECODES, b_decode_still_list};
#endif

#if B_MAX_AUDIO_ES_DECODES> 0
static const bobject_t
    b_decode_es_list[] = {
        B_ID(B_MAX_DECODES+B_MAX_STILL_DECODES)
#if B_MAX_AUDIO_ES_DECODES>1
        ,B_ID(B_MAX_DECODES+B_MAX_DECODES+1)
#endif
        ,NULL};
static const bconfig_obj_list b_decode_es = {"decode_es", B_MAX_AUDIO_ES_DECODES, b_decode_es_list};
#endif

static const bconfig_obj_list b_pcm_capture = {"pcm_capture", 2, b_obj_1_2};

#if B_MAX_MOSAIC_DECODES
static void bconfig_p_add_mosaics(void);
#endif

static void bconfig_p_get_objs(unsigned total, const bobject_t **objs)
{
    switch (total) {
    case 0: *objs = NULL; break;
    case 1: *objs = b_obj_1; break;
    case 2: *objs = b_obj_1_2; break;
    case 3: *objs = b_obj_1_2_3; break;
    case 4: *objs = b_obj_1_2_3_4; break;
    case 5: *objs = b_obj_1_2_3_4_5; break;
    case 6: *objs = b_obj_1_2_3_4_5_6; break;
    case 7: *objs = b_obj_1_2_3_4_5_6_7; break;
    case 8: *objs = b_obj_1_2_3_4_5_6_7_8; break;
    case 9: *objs = b_obj_1_2_3_4_5_6_7_8_9; break;
    case 10: *objs = b_obj_1_2_3_4_5_6_7_8_9_10; break;
    case 11: *objs = b_obj_1_2_3_4_5_6_7_8_9_10_11; break;
    case 12: *objs = b_obj_1_2_3_4_5_6_7_8_9_10_11_12; break;
    case 13: *objs = b_obj_1_2_3_4_5_6_7_8_9_10_11_12_13; break;
    default:
        BDBG_ERR(("Unable to get bconfig_obj_list for %d. Please extend Settop API.", total));
        *objs = NULL;
        break;
    }
}

void
bconfig_p_init(void)
{
    /* coverity[stack_use_local_overflow] */
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;
    unsigned i;
    NEXUS_PlatformStatus platformStatus;
    static char revision_str[] = "A0";
    static bconfig_chip main_chip;
    NEXUS_AudioCapabilities audioCaps;

    NEXUS_Platform_GetSettings(&platformSettings);
    NEXUS_Platform_GetConfiguration(&platformConfig);
    NEXUS_Platform_GetStatus(&platformStatus);

    BKNI_Memset(&bconfig_, 0, sizeof(bconfig_));
    bconfig_.board = BCM_BOARD_STR;
    bconfig_.chips[0] = &main_chip;
    main_chip.chip_no =  /* convert i from hex to decimal */
        (((platformStatus.chipId >> 12) & 0xF) * 1000) +
        (((platformStatus.chipId >> 8) & 0xF) * 100) +
        (((platformStatus.chipId >> 4) & 0xF) * 10) +
        (platformStatus.chipId & 0xF);
    revision_str[0] = ((platformStatus.chipRevision >> 4) & 0xF) + 'A' - 1;
    revision_str[1] = (platformStatus.chipRevision & 0xF) + '0';
    main_chip.revision = revision_str;
    main_chip.major = revision_str[0];
    main_chip.minor = revision_str[1];
    bconfig_.chips[1] = NULL;
    
#if B_XVD_NO_HD_SUPPORT
    bconfig_.features.video_hd = false;
#else
    bconfig_.features.video_hd = true;
#endif
#if B_DISPLAY_NO_HD_SUPPORT
    bconfig_.features.display_hd = false;
#else
    bconfig_.features.display_hd = true;
#endif
#if B_HAS_DCR
    bconfig_.features.dnr_dcr = true;
#endif
#if B_HAS_BNR
    bconfig_.features.dnr_bnr = true;
#endif
#if B_HAS_MNR
    bconfig_.features.dnr_mnr = true;
#endif
#if B_HAS_ANR
    bconfig_.features.anr = true;
#endif
#if B_HAS_MAD
    bconfig_.features.deinterlacer = true;
#endif
#if B_HAS_BOX_DETECT
    bconfig_.features.box_detect = true;
#endif

    bconfig_.features.pvr_encryption = true;

#if B_HAS_SHARPNESS
    bconfig_.features.sharpness = true;
#endif
#if B_HAS_CAB
    bconfig_.features.cab = true;
#endif
#if B_HAS_LAB
    bconfig_.features.lab = true;
#endif
#if B_MAX_MOSAIC_DECODES > 0
    bconfig_.features.mosaic = true;
#endif

    {
        NEXUS_DisplayCapabilities cap;
        bvideo_format_settings vesa_settings;
        NEXUS_GetDisplayCapabilities(&cap);
        /* pick a default vesa format for bvideo_format_vesa */
        b_nexus2displayformat(NEXUS_VideoFormat_eVesa640x480p60hz, &vesa_settings);
        for (i=0;i<bvideo_format_count;i++) {
            bconfig_.features.video_format_is_supported[i] = cap.displayFormatSupported[b_displayformat2nexus(i,&vesa_settings)];
        }
    }

    bconfig_.resources.display.type = "display";
    if (platformConfig.supportedDisplay[0] && platformConfig.supportedDisplay[1]) {
        bconfig_.resources.display.total = 2;
        bconfig_.resources.display.objects = b_obj_1_2;
    }
    else if (platformConfig.supportedDisplay[0]) {
        bconfig_.resources.display.total = 1;
        bconfig_.resources.display.objects = b_obj_1;
    }
    else if (platformConfig.supportedDisplay[1]) {
        bconfig_.resources.display.total = 1;
        bconfig_.resources.display.objects = b_obj_2; /* 2nd CMP is the primary display */
    }

    bconfig_.resources.decode.type = "decode";
    if (platformConfig.supportedDecoder[0] && platformConfig.supportedDecoder[1]) {
        bconfig_.resources.decode.total = 2;
        bconfig_.resources.decode.objects= b_obj_1_2;
    }
    else if (platformConfig.supportedDecoder[0]) {
        bconfig_.resources.decode.total = 1;
        bconfig_.resources.decode.objects= b_obj_1;
    }
    BDBG_MSG(("number of decodes =%d",bconfig_.resources.decode.total));

    bconfig_.resources.pcm_play = b_pcm;

#if B_MAX_STILL_DECODES> 0
    bconfig_.resources.decode_still = b_decode_still;
#endif
#if B_MAX_AUDIO_ES_DECODES> 0
    bconfig_.resources.decode_es = b_decode_es;
#endif
    bconfig_.resources.pcm_capture = b_pcm_capture;

    /* All nexus platforms support streamers so far */
#if BCHP_CHIP==7335 || BCHP_CHIP==7346 || BCHP_CHIP==7342
    bconfig_.resources.streamers.total = 2;
#elif (NEXUS_PLATFORM==97125)
    /* Actually has only 1.  This is a hack allowing "streamers" to be
    used to select QAM DS tuned at BNM console or with BBS. */
    bconfig_.resources.streamers.total = 3;
#elif (NEXUS_PLATFORM==97019)
    bconfig_.resources.streamers.total = 0;
#else
    bconfig_.resources.streamers.total = 1;
#endif
    bconfig_.resources.streamers.type = "streamer";
    bconfig_p_get_objs(bconfig_.resources.streamers.total, &bconfig_.resources.streamers.objects);

    bconfig_.resources.input_bands.total = B_MAX_INPUTS;
    bconfig_.resources.input_bands.type = "input_band";
    bconfig_p_get_objs(bconfig_.resources.input_bands.total, &bconfig_.resources.input_bands.objects);

    bconfig_p_add_pvr();

    bconfig_.resources.decode_mosaic.type = "mosaic";

    #if B_MAX_MOSAIC_DECODES > 0
    bconfig_p_add_mosaics();
    #endif

    NEXUS_GetAudioCapabilities(&audioCaps);
    bconfig_.features.srs_volume=audioCaps.dsp.truVolume;
    bconfig_.features.dolby_volume=audioCaps.dsp.dolbyVolume;
    bconfig_.features.auto_volume_level=audioCaps.dsp.autoVolumeLevel;
    bconfig_.features.dts_transcode = audioCaps.dsp.codecs[NEXUS_AudioCodec_eDts].encode;
    bconfig_.features.ac3_transcode = audioCaps.dsp.codecs[NEXUS_AudioCodec_eAc3].encode;

    return;
}

void
bconfig_p_add_pvr(void)
{
    NEXUS_TransportCapabilities cap;
    NEXUS_GetTransportCapabilities(&cap);

    bconfig_.resources.record.type = "record";
    bconfig_.resources.record.total = cap.numRecpumps>4?4:cap.numRecpumps;
    bconfig_p_get_objs(bconfig_.resources.record.total, &bconfig_.resources.record.objects);

    bconfig_.resources.record_tsdma.type = "record_tsdma";
    bconfig_.resources.record_pes.type = "record_pes";

    bconfig_.resources.playback.type = "playback";
	/* arbitrary limit.  Extend bconfig_p_get_objs() if more are needed */
    bconfig_.resources.playback.total = (cap.numPlaypumps<14) ? cap.numPlaypumps : 13;
    bconfig_p_get_objs(bconfig_.resources.playback.total, &bconfig_.resources.playback.objects);
}

#if NEXUS_HAS_FRONTEND
static bobject_t b_sds_ids[NEXUS_MAX_FRONTENDS];
static bobject_t b_vsb_ids[NEXUS_MAX_FRONTENDS];
static bobject_t b_qam_ids[NEXUS_MAX_FRONTENDS];
static bobject_t b_ofdm_ids[NEXUS_MAX_FRONTENDS];
#endif
#if B_HAS_TRANSCODE
static bobject_t b_xcd_linein_ids[B_MAX_ENCODES] ;
#endif
void
bconfig_p_set_tuners(void)
{
#if NEXUS_HAS_FRONTEND
    NEXUS_PlatformConfiguration platformConfig;
    NEXUS_FrontendCapabilities capabilities;
    unsigned n_sds, n_vsb, n_ofdm,n_qam;
    int i, front_ends;
    /* The Settop API tuner code depends on the reference platform "GetConfiguration" API for
    frontend detection and instance management. See 97400 or related platforms for API and implementation. */
    NEXUS_Platform_GetConfiguration(&platformConfig);
    for(i=0,n_sds=0,n_vsb=0,n_ofdm=0,n_qam=0;i<NEXUS_MAX_FRONTENDS;i++)
    {
        /* if not open, then move on to next */
        if(!platformConfig.frontend[i])
            continue;

        NEXUS_Frontend_GetCapabilities(platformConfig.frontend[i],&capabilities);

        if(capabilities.vsb)
        {
            BDBG_MSG(("Frontend %d,vsb",i));
            b_vsb_ids[n_vsb] = B_ID(i);
            n_vsb++;
        }
        if(capabilities.qam)
        {
            BDBG_MSG(("Frontend %d,qam",i));
            b_qam_ids[n_qam] = B_ID(i);
            n_qam++;
        }
        if(capabilities.satellite)
        {
            BDBG_MSG(("Frontend %d,satellite",i));
            b_sds_ids[n_sds] = B_ID(i);
            n_sds++;
        }
        if(capabilities.ofdm)
        {
            BDBG_MSG(("Frontend %d,ofdm",i));
            b_ofdm_ids[n_ofdm] = B_ID(i);
            n_ofdm++;
        }
        /* TODO: add more frontends */
    }
    front_ends = i ;
    BDBG_MSG(("Total tuners vsb=%d,qam=%d,satellite=%d,ofdm=%d",n_vsb,n_qam,n_sds,n_ofdm));
    bconfig_.resources.sds.type = "sds";
    bconfig_.resources.sds.objects = b_sds_ids;
    bconfig_.resources.sds.total = n_sds;
    bconfig_.features.sds = bconfig_.resources.sds.total != 0;

    bconfig_.resources.vsb.type = "vsb";
    bconfig_.resources.vsb.objects = b_vsb_ids;
    bconfig_.resources.vsb.total = n_vsb;
    bconfig_.features.vsb = bconfig_.resources.vsb.total != 0;
    
    /* VSB card also support QAM */
    bconfig_.resources.qam.type = "qam";
    bconfig_.resources.qam.objects = b_qam_ids;
    bconfig_.resources.qam.total = n_qam;
    bconfig_.features.qam = bconfig_.resources.qam.total != 0;

    bconfig_.resources.ofdm.type = "ofdm";
    bconfig_.resources.ofdm.objects = b_ofdm_ids;
    bconfig_.resources.ofdm.total = n_ofdm;
    bconfig_.features.ofdm = bconfig_.resources.ofdm.total != 0;

#endif
#if B_HAS_TRANSCODE
    for(i=0;i<B_MAX_ENCODES;i++)
    {
        b_xcd_linein_ids[i] = B_ID(front_ends+i) ;
    }
    bconfig_.resources.linein.type = "linein" ;
    bconfig_.resources.linein.objects = b_xcd_linein_ids;
    bconfig_.resources.linein.total = front_ends+i;
    BDBG_ERR(("Config ofdm Objects %d", bconfig_.resources.ofdm.objects[0], bconfig_.resources.ofdm.objects[1]));
    BDBG_ERR(("Config line Total %d", bconfig_.resources.ofdm.total));
    BDBG_ERR(("Config line Type %s", bconfig_.resources.linein.type));
    BDBG_ERR(("Config line Objects %d %d", bconfig_.resources.linein.objects[0], bconfig_.resources.linein.objects[1]));
    BDBG_ERR(("Config line Total %d", bconfig_.resources.linein.total));
#endif
    return;
}

#if B_MAX_MOSAIC_DECODES > 0
static bobject_t b_mosaic_ids[B_MAX_MOSAIC_DECODES];

static void bconfig_p_add_mosaics(void)
{
    int i;

    for ( i = 0; i < B_MAX_MOSAIC_DECODES; i++ )
    {
        b_mosaic_ids[i] = B_ID(i);
    }

    bconfig_.resources.decode_mosaic.objects = b_mosaic_ids;
    bconfig_.resources.decode_mosaic.total = B_MAX_MOSAIC_DECODES;
}
#endif
