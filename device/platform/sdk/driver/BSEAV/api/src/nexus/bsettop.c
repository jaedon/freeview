/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop.c $
 * $brcm_Revision: 86 $
 * $brcm_Date: 8/20/12 4:47p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop.c $
 * 
 * 86   8/20/12 4:47p mward
 * SW7346-119:  maxDataRate can only be changed via
 * NEXUS_TransportModuleSettings.
 * 
 * 85   5/11/12 10:10a erickson
 * SW7425-568: remove fast channel change datarate workaround
 * 
 * 84   3/15/12 12:22p erickson
 * SW7425-1747: remove old code
 * 
 * 83   2/24/12 12:10p jtna
 * SW7425-2387: coverity defect STACK_USE
 * 
 * 82   2/24/12 9:45a gmohile
 * SW7425-2473 : add uninit frontend
 * 
 * 81   12/16/11 7:18p jgarrett
 * SW7425-1406: Adding OGG/Vorbis
 * 
 * 80   12/8/11 2:44p mward
 * SW7425-568: Don't attempt to change maxdatarate if no streamer.
 * 
 * 79   12/6/11 1:40p erickson
 * SW7231-462: do not turn on nexus multiprocess server by default. add
 * nexus_server=y option.
 * 
 * 78   11/15/11 9:00a jgarrett
 * SW7425-1593: Adding additional video codecs to supported settop/brutus
 * formats
 * 
 * 77   11/14/11 12:31p mananp
 * SW7231-462:Added the support for the multiprocessing
 * 
 * 76   6/30/11 5:15p erickson
 * SW7346-299: fix NEXUS_VideoFrameRate_e7_493 typo
 * 
 * 74   5/18/11 7:24p jgarrett
 * SW7231-148: Adding RMFF and Cook support
 * 
 * 73   5/17/11 11:21a gmohile
 * SW7408-1 : Fix platform with no parser band
 * 
 * 72   5/11/11 5:07p jtna
 * SW7425-568: increase parserband maxdatarate on 40nm platforms for FCC
 * feature
 * 
 * 71   2/16/11 4:13p mward
 * SW7125-790:  Eliminate warnings for video formats (112,114,115) not
 * mapped to bvideo_format.
 * 
 * 70   2/15/11 4:14p agin
 * SWNOOS-441:  Support UCOS.
 * 
 * 69   1/25/11 3:54p davidp
 * SW7422-22: Add support for SVC protocol.
 * 
 * 68   11/22/10 1:01p spothana
 * SW7420-1078: assign heap index per video window and per graphics frame
 * buffer
 * 
 * 67   10/26/10 10:24p agin
 * SWNOOS-429:  Changed NO_OS to NO_OS_DIAGS.
 * 
 * 66   10/18/10 9:22a gmohile
 * SW7408-1 : Fix warning
 * 
 * 65   5/27/10 3:49p mward
 * SW7400-2772: Eliminate error message about video format 38, not
 * supported in Settop API.
 * 
 * 64   4/19/10 3:25p nickh
 * SW7420-709: Add DBS support and remove BCHP_CHIP
 * 
 * 63   3/1/10 10:56a erickson
 * SW7550-272: add bgraphics_pixel_format_a8_y8_cb8_cr8
 *
 * 62   1/21/10 6:47p garetht
 * SW7420-553: Add platform for 93380vms
 *
 * 61   12/29/09 2:48p erickson
 * SW7420-515: remove unsupported pixel formats
 *
 * 60   12/9/09 12:03p gmohile
 * SW7408-1 : Add 7408 support
 *
 * 59   12/8/09 2:05p rjain
 * SW7550-112: Dont require this on a branch. Just make sure to use
 * version 10 or above of /nexus/base/include/nexus_audio_types.h to
 * build wiht DRA
 *
 * Refsw_7550/2   12/8/09 2:03p rjain
 * SW7550-122: Dont require this on a branch. Just make sure to use
 * version 10 or above of /nexus/base/include/nexus_audio_types.h to
 * build wiht DRA
 *
 * Refsw_7550/1   11/5/09 11:25a nitinb
 * sw7550-64: Initial brutus build for 7550
 *
 * 58   10/8/09 6:04p jgarrett
 * SW7405-3064: Adding DRA
 *
 * 57   9/15/09 12:09p jjordan
 * SW7405-3011: add control of sync path swap capability from settop api
 * environment variable
 *
 * 56   9/14/09 3:05p jgarrett
 * SW7405-3011: Splitting sync control into precision and basic
 *
 * 55   9/14/09 10:37a jgarrett
 * SW3548-2188: Adding AMR support
 *
 * 54   9/10/09 5:17p jgarrett
 * SW7405-3011: Adding sync options to bsettop_init_settings
 *
 * 53   9/4/09 11:45a jgarrett
 * SWDEPRECATED-3910: Exposing audio and video FIFO sizes
 *
 * 52   8/25/09 12:36p erickson
 * SW7420-228: fix warnings
 *
 * 51   8/10/09 1:41p jgarrett
 * PR 51648: Adding option to set band gap settings in API
 *
 * 50   7/30/09 6:46p vsilyaev
 * PR 57220: Added 7.943 and 14.985 frame rates
 *
 * 49   7/30/09 6:01p mward
 * PR56558: Don't init frontend more than once, may have been done by a
 * different call to  bsettop_init_frontend() (in DUAL_OUTPUT Brutus, for
 * example), not just in bsettop_init().
 *
 * 48   7/28/09 11:51a lwhite
 * PR55693: Added DTCP/IP intialization
 *
 * 46   7/24/09 10:48a erickson
 * PR51648: set NEXUS_DisplayModuleSettings.dacBandGapAdjust[] based on
 * DACx_BAND_GAP env variable.
 *
 * 45   7/15/09 7:42p vsilyaev
 * PR 55653: Added WAV format
 *
 * 44   7/10/09 7:29p agin
 * PR56698: Return NULL for no-os for bsettop_get_config.
 *
 * 43   7/2/09 10:47a erickson
 * PR56558: add bsettop_set_init_settings and bsettop_init_frontend
 *
 * 42   6/30/09 4:23p katrep
 * PR55809: support for nexus_pcmwav for avi with pcm audio
 *
 * 41   6/25/09 3:34p katrep
 * PR55809: Add support for PCM audio in AVI files
 *
 * 40   6/9/09 11:30a rjlewis
 * PR55744: david says we can't always call base module functions -- only
 * ok in vxworks.
 *
 * 39   6/5/09 7:18p rjlewis
 * PR55749: use nexus version to fix environment issue in VxWorks.
 *
 * 38   3/18/09 12:31p erickson
 * PR53340: add 1080p 50hz support
 *
 * 37   2/26/09 2:53p erickson
 * PR49584: add a dma-based memory bandwidth stress test feature
 *
 * 36   2/11/09 3:32p vle
 * PR 51414, PR 45656: Add 1080p/60Hz support in Brutus and enable for
 * 7420
 *
 * 35   11/14/08 6:19p agin
 * PR48985: Support LDPC and Turbo at the same time.
 *
 * 34   9/15/08 5:24p jrubio
 * PR46925:  fix PVR_SUPPORT=n compile issue
 *
 * 33   8/7/08 4:51p vsilyaev
 * PR 45137: Added slave mode support to the SettopApi shim
 *
 * 32   7/21/08 10:11a erickson
 * PR36068: add debug
 *
 * 31   7/3/08 6:09p erickson
 * PR36068: impl vesa
 *
 * 30   6/27/08 9:30a erickson
 * PR43653: add vesa support
 *
 * 29   6/26/08 6:21p mphillip
 * PR42901: Encrypted PVR playback support
 *
 * 28   6/18/08 10:13p erickson
 * PR36068: add DTV panel support to shim (use component api)
 *
 * 27   5/22/08 3:46p erickson
 * PR36068: remove bvideo_format ERR's for hdmi
 *
 * 26   5/20/08 5:03p vishk
 * PR 42871: Unable to tune QAM64/QAM256/VSB using Brutus for
 * nexus_latest.
 * PR 42058: Can not tune to QAM256 using onboard 3510 tuner
 *
 * 25   5/14/08 7:25p jrubio
 * PR42694: add TUNER_ENABLE_7335 flag as a workaround for streamer/tuner
 * not working together
 *
 * 24   4/24/08 2:53p jrubio
 * PR42054: add sds2nexus function
 *
 * 23   4/10/08 9:56a erickson
 * PR36068: coverity fixes
 *
 * 22   4/2/08 3:49p jrubio
 * PR41191: add uninit functions
 *
 * 21   3/31/08 6:04p jgarrett
 * PR 40606: Adding b_os_init/uninit
 *
 * 20   3/27/08 5:33p jrubio
 * PR40919: add userio init and uninit
 *
 * 19   3/27/08 2:28p erickson
 * PR40928: fix bsettop_uninit
 *
 * 18   3/25/08 12:52p erickson
 * PR36068: added b_nexus2framerate
 *
 * 17   3/25/08 12:35p erickson
 * PR40744: standardized capitalization on enums
 *
 * 16   3/25/08 10:53a katrep
 * PR40893: Add support for HDMI capabilities
 *
 * 15   3/25/08 10:13a jgarrett
 * PR 40857: Adding memory routines
 *
 * 14   3/21/08 11:19a erickson
 * PR36068: rework around vesa. still left TODO.
 *
 * 13   3/17/08 5:38p katrep
 * PR40021:Adding call to tuner init
 *
 * 12   3/12/08 2:28p katrep
 * PR40514:Add support for AVS codec
 *
 * 11   3/11/08 11:09a erickson
 * PR36068: added b_timer_schedule support
 *
 * 10   3/3/08 3:36p vsilyaev
 * PR 39818: Support of MKV format
 *
 * 9   2/4/08 10:00a erickson
 * PR36068: update
 *
 * 8   1/2/08 9:55a erickson
 * PR36068: Playback changes
 *
 * 7   12/20/07 10:28a erickson
 * PR37590: fix warnings
 *
 * 6   11/27/07 11:16p erickson
 * PR36068: added g_settopModule to allow use of nexus timers
 *
 * 5   11/13/07 10:24a erickson
 * PR36068: added b_lock_init
 *
 * 4   11/2/07 1:24p erickson
 * PR36633: update nexus enums
 *
 * 3   10/16/07 12:35p erickson
 * PR36068: brutus up over settop api/nexus
 *
 * 2   10/15/07 4:13p erickson
 * PR36068: initial
 *
 * 1   10/15/07 2:35p erickson
 * PR36068: initial
 *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bsettop_impl.h"
#include "nexus_memory.h"

BDBG_MODULE(settop);

static void bsettop_p_monitor(void *data)
{
    unsigned i;
    BSTD_UNUSED(data);
    for (i=0;i<B_MAX_DECODES;i++) {
        bdecode_t decode = &g_decode[i];
        if (decode->started) {
            bdecode_status status;
            bdecode_get_status(decode, &status);
            BKNI_Printf("decode[%d]: pts 0x%08x, stc 0x%08x, diff %d, fifo %d%%\n",
                i,
                status.video_pts, status.video_stc, status.video_stc - status.video_pts,
                status.video_fifo_size ? status.video_fifo_depth * 100 / status.video_fifo_size : 0
                );
            BKNI_Printf("  audio0: pts 0x%08x, stc 0x%08x, diff %d, fifo %d%%\n",
                status.audio_pts, status.audio_stc, status.audio_stc - status.audio_pts,
                status.audio_fifo_size ? status.audio_fifo_depth * 100 / status.audio_fifo_size : 0
                );
        }
    }

    b_timer_schedule(1000, bsettop_p_monitor, NULL);
}

static bsettop_init_settings g_bsettop_init_settings;

static bool g_bsettop_frontend_opened = false;
static bool g_init_settings_initialized = false;

static void bsettop_p_init_settings(void)
{
    if ( !g_init_settings_initialized )
    {
        /* coverity[stack_use_local_overflow] */
        NEXUS_PlatformSettings platformSettings;

        NEXUS_Platform_GetDefaultSettings(&platformSettings);
        BDBG_CASSERT(BSETTOP_MAX_VIDEO_DACS == NEXUS_MAX_VIDEO_DACS);
        BKNI_Memcpy(g_bsettop_init_settings.video_dac_band_gap,
                    platformSettings.displayModuleSettings.dacBandGapAdjust,
                    sizeof(g_bsettop_init_settings.video_dac_band_gap));
        g_bsettop_init_settings.open_frontend = true;
        g_bsettop_init_settings.lipsync_mode = bsettop_lipsync_mode_precision;
        g_bsettop_init_settings.sync_path_swap = bsettop_get_config("sync_path_swap")?true:false;

        if ( bsettop_get_config("force_vsync") )
        {
            g_bsettop_init_settings.lipsync_mode = bsettop_lipsync_mode_disabled;
        }
        else if ( bsettop_get_config("sync_disabled") )
        {
            g_bsettop_init_settings.lipsync_mode = bsettop_lipsync_mode_tsm;
        }
        else if ( bsettop_get_config("precision_sync_disabled") )
        {
            g_bsettop_init_settings.lipsync_mode = bsettop_lipsync_mode_basic;
        }

#ifdef B_HAS_IP
        /* Increase the CDB for IP builds: video CDB is used as the dejitter buffer */
        g_bsettop_init_settings.video_fifo_size = 3 * 1024 * 1024;
#endif

        g_init_settings_initialized = true;
    }
}

void bsettop_get_init_settings( bsettop_init_settings *settings )
{
    bsettop_p_init_settings();
    *settings = g_bsettop_init_settings;
}

int bsettop_set_init_settings( const bsettop_init_settings *settings )
{
    g_bsettop_init_settings = *settings;
    return 0;
}

bresult bsettop_init(bsettop_version version)
{
    NEXUS_Error rc;
    /* coverity[stack_use_local_overflow] */
    NEXUS_PlatformSettings platformSettings;
    NEXUS_PlatformConfiguration platformConfig;

    const char *max_hd_display_format = bsettop_get_config("max_hd_display_format");

    if (version != BSETTOP_VERSION) {
        fprintf(stderr, "BSETTOP_VERSION is out of date!  Expected %d, got %d\n", BSETTOP_VERSION, version);
        return BSETTOP_ERROR(berr_invalid_parameter);
    }

    bsettop_p_init_settings();

    if(bsettop_get_config("mode_slave")==NULL) {
        unsigned i;

        /* Bring up all modules for a platform in a default configuration for this platform */
        NEXUS_Platform_GetDefaultSettings(&platformSettings);

        platformSettings.openFrontend = g_bsettop_init_settings.open_frontend;
        g_bsettop_frontend_opened = false;
        BKNI_Memcpy(platformSettings.displayModuleSettings.dacBandGapAdjust,
                    g_bsettop_init_settings.video_dac_band_gap,
                    sizeof(g_bsettop_init_settings.video_dac_band_gap));

        if (max_hd_display_format) {
            if (!strcasecmp(max_hd_display_format, "1080p")) {
                if(platformSettings.displayModuleSettings.fullHdBuffers.count)
                { 
                   platformSettings.displayModuleSettings.fullHdBuffers.format = NEXUS_VideoFormat_e1080p;
                }
                else
                {  unsigned primaryDisplayHeapIndex= platformSettings.displayModuleSettings.primaryDisplayHeapIndex;
                   platformSettings.displayModuleSettings.displayHeapSettings[primaryDisplayHeapIndex].fullHdBuffers.format = NEXUS_VideoFormat_e1080p;
                }
            }
        }

        for (i=0;i<NEXUS_MAX_VIDEO_DACS;i++) {
            char buf[32];
            BKNI_Snprintf(buf, 32, "DAC%d_BAND_GAP", i);
            if (NEXUS_GetEnv(buf)) {
                platformSettings.displayModuleSettings.dacBandGapAdjust[i] = NEXUS_atoi(NEXUS_GetEnv(buf));
            }
        }

        if (NEXUS_GetEnv("max_data_rate")) {
            unsigned max_data_rate = NEXUS_atoi(NEXUS_GetEnv("max_data_rate"));
			for (i=0;i<NEXUS_MAX_PARSER_BANDS;i++) {
                platformSettings.transportModuleSettings.maxDataRate.parserBand[i] = max_data_rate*1024*1024;
			}
            BDBG_MSG(("max_data_rate %d Mbps",max_data_rate));
        }

        rc = NEXUS_Platform_Init(&platformSettings);
        if (rc) {
            fprintf(stderr, "Failed to initialize NEXUS\n");
            return BSETTOP_ERROR(berr_external_error);
        }
    } else {
        rc = NEXUS_Platform_Join();    

        if (rc) {
            fprintf(stderr, "Failed to join NEXUS\n");
            return BSETTOP_ERROR(berr_external_error);
        }
    }

    /*For Multiprocessing*/
    NEXUS_Platform_GetConfiguration(&platformConfig);

    if (NEXUS_GetEnv("nexus_server")) {
        NEXUS_PlatformStartServerSettings serverSettings;
        NEXUS_Platform_GetDefaultStartServerSettings(&serverSettings);
        serverSettings.allowUnauthenticatedClients = true;
        serverSettings.unauthenticatedConfiguration.mode = NEXUS_ClientMode_eUntrusted;
        serverSettings.unauthenticatedConfiguration.heap[2] = platformConfig.heap[0]; /* doesn't have to be 0-->0 mapping */
        rc = NEXUS_Platform_StartServer(&serverSettings);
        BDBG_ASSERT(!rc);
    }

    b_os_init();
    bconfig_p_init();
    bstream_p_init();
    bdisplay_p_init();
    bdecode_p_init();
    btuner_p_init();
    buserio_p_init();
    bdma_p_init();
#ifdef B_HAS_IP
    bplayback_ip_p_init();
#endif

    if (NEXUS_GetEnv("monitor")) {
        b_timer_schedule(1000, bsettop_p_monitor, NULL);
    }

#if NEXUS_HAS_DMA
    if (NEXUS_GetEnv("mem_stress")) {
        bdma_p_mem_stress();
    }
#endif

    return 0;
}

int bsettop_init_frontend(void)
{
    NEXUS_Error rc;

    if (g_bsettop_frontend_opened) {
        /* already opened */
        return 0;
    }

#if NEXUS_HAS_FRONTEND
    rc = NEXUS_Platform_InitFrontend();
    if (rc) return BERR_TRACE(rc);

    /* it's ok to call this a second time */
    bconfig_p_set_tuners();
    g_bsettop_frontend_opened = true;    
#else
    g_bsettop_frontend_opened = false;
    rc = BERR_SUCCESS;
#endif   
    return rc;
}

void bsettop_uninit(void)
{
#ifdef B_HAS_IP
    bplayback_ip_p_uninit();
#endif
    bdecode_p_uninit();
    bdisplay_p_uninit();
    buserio_p_uninit();
    b_os_uninit();
    if (NEXUS_GetEnv("nexus_server")) {
        NEXUS_Platform_StopServer();
    }
    NEXUS_Platform_Uninit();
}

void bsettop_uninit_frontend(void)
{    
    if (!g_bsettop_frontend_opened) {
        return;
    }

#if NEXUS_HAS_FRONTEND
    NEXUS_Platform_UninitFrontend();        
#endif   

    g_bsettop_frontend_opened = false;       

    return;    
}

void bsettop_get_power_state(bsettop_power_state *state)
{
    /* TODO */
    state->av_outputs = true;
    BSTD_UNUSED(state);
}

bresult bsettop_set_power_state(const bsettop_power_state *state)
{
    BSTD_UNUSED(state);
    /* TODO */
    return 0;
}

struct {
    NEXUS_VideoCodec nexus;
    bvideo_codec settop;
} g_videoCodec[] = {
    {NEXUS_VideoCodec_eUnknown, bvideo_codec_none},
    {NEXUS_VideoCodec_eUnknown, bvideo_codec_unknown},
    {NEXUS_VideoCodec_eMpeg1, bvideo_codec_mpeg1},
    {NEXUS_VideoCodec_eMpeg2, bvideo_codec_mpeg2},
    {NEXUS_VideoCodec_eMpeg4Part2, bvideo_codec_mpeg4_part2},
    {NEXUS_VideoCodec_eH263, bvideo_codec_h263},
    {NEXUS_VideoCodec_eH264, bvideo_codec_h264},
    {NEXUS_VideoCodec_eVc1, bvideo_codec_vc1},
    {NEXUS_VideoCodec_eVc1SimpleMain, bvideo_codec_vc1_sm},
    {NEXUS_VideoCodec_eDivx311, bvideo_codec_divx_311},
    {NEXUS_VideoCodec_eH264_Svc,bvideo_codec_h264_svc},    
    {NEXUS_VideoCodec_eH264_Mvc,bvideo_codec_h264_mvc},
    {NEXUS_VideoCodec_eAvs,bvideo_codec_avs},
    {NEXUS_VideoCodec_eSpark, bvideo_codec_spark},
    {NEXUS_VideoCodec_eVp6, bvideo_codec_vp6},
    {NEXUS_VideoCodec_eRv40, bvideo_codec_rv40},
    {NEXUS_VideoCodec_eVp8, bvideo_codec_vp8}
};

struct {
    NEXUS_AudioCodec nexus;
    baudio_format settop;
} g_audioCodec[] = {
   {NEXUS_AudioCodec_eUnknown, baudio_format_unknown},
   {NEXUS_AudioCodec_eMpeg, baudio_format_mpeg},
   {NEXUS_AudioCodec_eMp3, baudio_format_mp3},
   {NEXUS_AudioCodec_eAac, baudio_format_aac},
   {NEXUS_AudioCodec_eAacPlus, baudio_format_aac_plus},
   {NEXUS_AudioCodec_eAacPlusAdts, baudio_format_aac_plus_adts},
   {NEXUS_AudioCodec_eAacPlusLoas, baudio_format_aac_plus_loas},
   {NEXUS_AudioCodec_eAc3, baudio_format_ac3},
   {NEXUS_AudioCodec_eAc3Plus, baudio_format_ac3_plus},
   {NEXUS_AudioCodec_eDts, baudio_format_dts},
   {NEXUS_AudioCodec_eLpcmHdDvd, baudio_format_lpcm_hddvd},
   {NEXUS_AudioCodec_eLpcmBluRay, baudio_format_lpcm_bluray},
   {NEXUS_AudioCodec_eDtsHd, baudio_format_dts_hd},
   {NEXUS_AudioCodec_eWmaStd, baudio_format_wma_std},
   {NEXUS_AudioCodec_eWmaPro, baudio_format_wma_pro},
   {NEXUS_AudioCodec_eLpcmDvd, baudio_format_lpcm_dvd},
   {NEXUS_AudioCodec_eAvs, baudio_format_avs},
   {NEXUS_AudioCodec_eAmr, baudio_format_amr},
   {NEXUS_AudioCodec_eDra, baudio_format_dra},
   {NEXUS_AudioCodec_ePcmWav, baudio_format_pcm},
   {NEXUS_AudioCodec_eCook, baudio_format_cook},
   {NEXUS_AudioCodec_eVorbis, baudio_format_vorbis}
};

struct {
    NEXUS_VideoFormat nexus;
    bvideo_format settop;
} g_videoFormat[] = {
    {NEXUS_VideoFormat_eNtsc, bvideo_format_ntsc},
    {NEXUS_VideoFormat_eNtscJapan, bvideo_format_ntsc_japan},
    {NEXUS_VideoFormat_ePalM, bvideo_format_pal_m},
    {NEXUS_VideoFormat_ePalN, bvideo_format_pal_n},
    {NEXUS_VideoFormat_ePalNc, bvideo_format_pal_nc},
    {NEXUS_VideoFormat_ePalB, bvideo_format_pal_b},
    {NEXUS_VideoFormat_ePalB1, bvideo_format_pal_b1},
    {NEXUS_VideoFormat_ePalD, bvideo_format_pal_d},
    {NEXUS_VideoFormat_ePalD1, bvideo_format_pal_d1},
    {NEXUS_VideoFormat_ePalG, bvideo_format_pal_g},
    {NEXUS_VideoFormat_ePal, bvideo_format_pal},
    {NEXUS_VideoFormat_ePalH, bvideo_format_pal_h},
    {NEXUS_VideoFormat_ePalK, bvideo_format_pal_k},
    {NEXUS_VideoFormat_ePalI, bvideo_format_pal_i},
    {NEXUS_VideoFormat_eSecam, bvideo_format_secam},
    {NEXUS_VideoFormat_e480p, bvideo_format_480p},
    {NEXUS_VideoFormat_e576p, bvideo_format_576p},
    {NEXUS_VideoFormat_e1080i, bvideo_format_1080i},
    {NEXUS_VideoFormat_e1080i50hz, bvideo_format_1080i_50hz},
    {NEXUS_VideoFormat_e1080p, bvideo_format_1080p},
    {NEXUS_VideoFormat_e1080p24hz, bvideo_format_1080p_24hz},
    {NEXUS_VideoFormat_e1080p25hz, bvideo_format_1080p_25hz},
    {NEXUS_VideoFormat_e1080p30hz, bvideo_format_1080p_30hz},
    {NEXUS_VideoFormat_e1080p50hz, bvideo_format_1080p_50hz},
    {NEXUS_VideoFormat_e1250i50hz, bvideo_format_1250i_50hz},
    {NEXUS_VideoFormat_e720p, bvideo_format_720p},
    {NEXUS_VideoFormat_e720p50hz, bvideo_format_720p_50hz},
    {NEXUS_VideoFormat_e720p24hz, bvideo_format_720p_24hz},
    {NEXUS_VideoFormat_eCustom0, bvideo_format_1080i}, /* TODO */

    /* the following do not map, but we don't need ERR messages on the console */
    {NEXUS_VideoFormat_eNtsc443, bvideo_format_ntsc}, /* does not map */
    {NEXUS_VideoFormat_ePal60hz, bvideo_format_ntsc}, /* does not map */
    {NEXUS_VideoFormat_eSecamB, bvideo_format_ntsc}, /* does not map */
    {NEXUS_VideoFormat_eSecamG, bvideo_format_ntsc}, /* does not map */
    {NEXUS_VideoFormat_eSecamD, bvideo_format_ntsc}, /* does not map */
    {NEXUS_VideoFormat_eSecamK, bvideo_format_ntsc}, /* does not map */
    {NEXUS_VideoFormat_eSecamH, bvideo_format_ntsc}, /* does not map */
    {NEXUS_VideoFormat_e1080p60hz, bvideo_format_ntsc}, /* does not map */
    {NEXUS_VideoFormat_e1080p50hz, bvideo_format_ntsc}, /* does not map */
    {NEXUS_VideoFormat_e720p30hz, bvideo_format_ntsc}, /* does not map */
    {NEXUS_VideoFormat_e720p_3DOU_AS, bvideo_format_ntsc}, /* does not map */
    {NEXUS_VideoFormat_e720p50hz_3DOU_AS, bvideo_format_ntsc}, /* does not map */
    {NEXUS_VideoFormat_e1080p24hz_3DOU_AS, bvideo_format_ntsc}, /* does not map */
    {NEXUS_VideoFormat_e1080p30hz_3DOU_AS, bvideo_format_ntsc} /* does not map */
};

struct {
    NEXUS_DisplayAspectRatio nexus;
    unsigned settop;
} g_aspectRatio[] = {
  {NEXUS_DisplayAspectRatio_e4x3, bdisplay_aspect_ratio_4x3},
  {NEXUS_DisplayAspectRatio_e16x9, bdisplay_aspect_ratio_16x9}
};

struct {
    NEXUS_PixelFormat nexus;
    unsigned settop;
} g_pixelFormat[] = {
    {NEXUS_PixelFormat_eR5_G6_B5, bgraphics_pixel_format_r5_g6_b5},
    {NEXUS_PixelFormat_eA1_R5_G5_B5, bgraphics_pixel_format_a1_r5_g5_b5},
    {NEXUS_PixelFormat_eR5_G5_B5_A1, bgraphics_pixel_format_r5_g5_b5_a1},
    {NEXUS_PixelFormat_eA4_R4_G4_B4, bgraphics_pixel_format_a4_r4_g4_b4},
    {NEXUS_PixelFormat_eR4_G4_B4_A4, bgraphics_pixel_format_r4_g4_b4_a4},
    {NEXUS_PixelFormat_eA8_R8_G8_B8, bgraphics_pixel_format_a8_r8_g8_b8},
    {NEXUS_PixelFormat_ePalette8, bgraphics_pixel_format_palette8},
    {NEXUS_PixelFormat_eA8_Palette8, bgraphics_pixel_format_a8_palette8},
    {NEXUS_PixelFormat_eA8, bgraphics_pixel_format_a8},
    {NEXUS_PixelFormat_eY08_Cb8_Y18_Cr8, bgraphics_pixel_format_y08_cb8_y18_cr8},
    {NEXUS_PixelFormat_eCr8_Y18_Cb8_Y08, bgraphics_pixel_format_cr8_y18_cb8_y08},
    {NEXUS_PixelFormat_eA8_Y8_Cb8_Cr8, bgraphics_pixel_format_a8_y8_cb8_cr8},
    {NEXUS_PixelFormat_ePalette2, bgraphics_pixel_format_palette2},
    {NEXUS_PixelFormat_ePalette4, bgraphics_pixel_format_palette4}
};

struct {
    NEXUS_MessageFormat nexus;
    unsigned settop;
} g_messageFormat[] = {
    {NEXUS_MessageFormat_eTs, bmessage_format_ts},
    {NEXUS_MessageFormat_ePes, bmessage_format_pes},
    {NEXUS_MessageFormat_ePsi, bmessage_format_psi}
};

#if NEXUS_HAS_FRONTEND
struct {
    NEXUS_FrontendSatelliteMode nexus;
    unsigned settop;
} g_sdsMode[] = {
 { NEXUS_FrontendSatelliteMode_eDvb, btuner_sds_mode_qpsk_dvb},
 { NEXUS_FrontendSatelliteMode_eDss, btuner_sds_mode_qpsk_dss},
 { NEXUS_FrontendSatelliteMode_eDcii,  btuner_sds_mode_qpsk_dcii},
 { NEXUS_FrontendSatelliteMode_eQpskLdpc, btuner_sds_mode_qpsk_ldpc },
 { NEXUS_FrontendSatelliteMode_e8pskLdpc,  btuner_sds_mode_8psk_ldpc },
 { NEXUS_FrontendSatelliteMode_eLdpc,  btuner_sds_mode_ldpc },
 { NEXUS_FrontendSatelliteMode_eQpskTurbo, btuner_sds_mode_qpsk_turbo },
 { NEXUS_FrontendSatelliteMode_e8pskTurbo, btuner_sds_mode_8psk_turbo },
 { NEXUS_FrontendSatelliteMode_eTurbo, btuner_sds_mode_turbo }
};
#endif

struct {
    NEXUS_VideoFrameRate nexus;
    bvideo_frame_rate settop;
} g_videoFrameRate[] = {
    {NEXUS_VideoFrameRate_eUnknown, bvideo_frame_rate_unknown},
    {NEXUS_VideoFrameRate_e7_493, bvideo_frame_rate_7_493},
    {NEXUS_VideoFrameRate_e14_985, bvideo_frame_rate_14_985},
    {NEXUS_VideoFrameRate_e23_976, bvideo_frame_rate_23_976},
    {NEXUS_VideoFrameRate_e24, bvideo_frame_rate_24},
    {NEXUS_VideoFrameRate_e25, bvideo_frame_rate_25},
    {NEXUS_VideoFrameRate_e29_97, bvideo_frame_rate_29_97},
    {NEXUS_VideoFrameRate_e30, bvideo_frame_rate_30},
    {NEXUS_VideoFrameRate_e50, bvideo_frame_rate_50},
    {NEXUS_VideoFrameRate_e59_94, bvideo_frame_rate_59_94},
    {NEXUS_VideoFrameRate_e60, bvideo_frame_rate_60}
};

#define CONVERT(g_struct) \
    unsigned i; \
    for (i=0;i<sizeof(g_struct)/sizeof(g_struct[0]);i++) { \
        if (g_struct[i].settop == settop_value) { \
            return g_struct[i].nexus; \
        } \
    } \
    BDBG_ERR(("unable to find Settop API value %d in %s", settop_value, #g_struct)); \
    return g_struct[0].nexus

#define CONVERT_NEXUS(g_struct) \
    unsigned i; \
    for (i=0;i<sizeof(g_struct)/sizeof(g_struct[0]);i++) { \
        if (g_struct[i].nexus == nexus_value) { \
            return g_struct[i].settop; \
        } \
    } \
    BDBG_ERR(("unable to find Nexus value %d in %s", nexus_value, #g_struct)); \
    return g_struct[0].settop

NEXUS_VideoCodec b_videocodec2nexus(bvideo_codec settop_value)
{
    CONVERT(g_videoCodec);
}

bvideo_codec b_nexus2videocodec(NEXUS_VideoCodec nexus_value)
{
    CONVERT_NEXUS(g_videoCodec);
}

NEXUS_AudioCodec b_audiocodec2nexus(baudio_format settop_value)
{
    CONVERT(g_audioCodec);
}

baudio_format b_nexus2audiocodec(NEXUS_AudioCodec nexus_value)
{
    CONVERT_NEXUS(g_audioCodec);
}

bvideo_frame_rate b_nexus2framerate(NEXUS_VideoFrameRate nexus_value)
{
    CONVERT_NEXUS(g_videoFrameRate);
}

struct {
    NEXUS_VideoFormat vesaFormat;
    unsigned width;
    unsigned height;
    unsigned refresh_rate;
} g_vesaFormats[] = {
    {NEXUS_VideoFormat_eVesa640x480p60hz, 640, 480, 60},
    {NEXUS_VideoFormat_eVesa800x600p60hz, 800, 600, 60},
    {NEXUS_VideoFormat_eVesa1024x768p60hz, 1024, 768, 60},
    {NEXUS_VideoFormat_eVesa1280x768p60hz, 1280, 768, 60},
    {NEXUS_VideoFormat_eVesa1280x768p60hzRed, 1280, 768, 60},
    {NEXUS_VideoFormat_eVesa1280x720p50hz, 1280, 720, 50},
    {NEXUS_VideoFormat_eVesa1280x720p60hz, 1280, 720, 60},
    {NEXUS_VideoFormat_eVesa1280x720pReducedBlank, 1280, 720, 60},
    {NEXUS_VideoFormat_eVesa640x350p60hz, 640, 350, 60},
    {NEXUS_VideoFormat_eVesa640x350p70hz, 640, 350, 70},
    {NEXUS_VideoFormat_eVesa640x350p72hz, 640, 350, 72},
    {NEXUS_VideoFormat_eVesa640x350p75hz, 640, 350, 75},
    {NEXUS_VideoFormat_eVesa640x350p85hz, 640, 350, 85},
    {NEXUS_VideoFormat_eVesa640x400p60hz, 640, 400, 60},
    {NEXUS_VideoFormat_eVesa640x400p70hz, 640, 400, 70},
    {NEXUS_VideoFormat_eVesa640x400p72hz, 640, 400, 72},
    {NEXUS_VideoFormat_eVesa640x400p75hz, 640, 400, 75},
    {NEXUS_VideoFormat_eVesa640x400p85hz, 640, 400, 85},
    {NEXUS_VideoFormat_eVesa640x480p70hz, 640, 480, 70},
    {NEXUS_VideoFormat_eVesa640x480p72hz, 640, 480, 72},
    {NEXUS_VideoFormat_eVesa640x480p75hz, 640, 480, 75},
    {NEXUS_VideoFormat_eVesa640x480p85hz, 640, 480, 85},

    {NEXUS_VideoFormat_eVesa720x400p60hz, 720, 400, 60},
    {NEXUS_VideoFormat_eVesa720x400p70hz, 720, 400, 70},
    {NEXUS_VideoFormat_eVesa720x400p72hz, 720, 400, 72},
    {NEXUS_VideoFormat_eVesa720x400p75hz, 720, 400, 75},
    {NEXUS_VideoFormat_eVesa720x400p85hz, 720, 400, 85},
    {NEXUS_VideoFormat_eVesa800x600p56hz, 800, 600, 56},
    {NEXUS_VideoFormat_eVesa800x600p70hz, 800, 600, 70},
    {NEXUS_VideoFormat_eVesa800x600p72hz, 800, 600, 72},
    {NEXUS_VideoFormat_eVesa800x600p75hz, 800, 600, 75},
    {NEXUS_VideoFormat_eVesa800x600p85hz, 800, 600, 85},
    {NEXUS_VideoFormat_eVesa1024x768p70hz, 1024, 768, 70},
    {NEXUS_VideoFormat_eVesa1024x768p72hz, 1024, 768, 72},
    {NEXUS_VideoFormat_eVesa1024x768p75hz, 1024, 768, 75},
    {NEXUS_VideoFormat_eVesa1024x768p85hz, 1024, 768, 85},
    {NEXUS_VideoFormat_eVesa1280x720p70hz, 1280, 720, 70},
    {NEXUS_VideoFormat_eVesa1280x720p72hz, 1280, 720, 72},
    {NEXUS_VideoFormat_eVesa1280x720p75hz, 1280, 720, 75},
    {NEXUS_VideoFormat_eVesa1280x720p85hz, 1280, 720, 85},
    {NEXUS_VideoFormat_eVesa1024x768i87hz, 1024, 768, 87},
    {NEXUS_VideoFormat_eVesa1152x864p75hz, 1152, 864, 75},
    {NEXUS_VideoFormat_eVesa1280x768p75hz, 1280, 768, 75},
    {NEXUS_VideoFormat_eVesa1280x768p85hz, 1280, 768, 85},
    {NEXUS_VideoFormat_eVesa1280x960p60hz, 1280, 960, 60},
    {NEXUS_VideoFormat_eVesa1280x960p85hz, 1280, 960, 85},
    {NEXUS_VideoFormat_eVesa1280x1024p60hz, 1280, 1024, 60},
    {NEXUS_VideoFormat_eVesa1280x1024p75hz, 1280, 1024, 75},
    {NEXUS_VideoFormat_eVesa1280x1024p85hz, 1280, 1024, 85},
    {NEXUS_VideoFormat_eVesa832x624p75hz, 832, 624, 75},
    {NEXUS_VideoFormat_eVesa1360x768p60hz, 1360, 768, 60},
    {NEXUS_VideoFormat_eVesa1366x768p60hz, 1366, 768, 60},
    {NEXUS_VideoFormat_eVesa1400x1050p60hz, 1400, 1050, 60},
    {NEXUS_VideoFormat_eVesa1400x1050p60hzReducedBlank, 1400, 1050, 60},
    {NEXUS_VideoFormat_eVesa1400x1050p75hz, 1400, 1050, 75},
    {NEXUS_VideoFormat_eVesa1600x1200p60hz, 1600, 1200, 60},
    {NEXUS_VideoFormat_eVesa1920x1080p60hzReducedBlank, 1920, 1080, 60},
};

NEXUS_VideoFormat b_displayformat2nexus(bvideo_format settop_value, const bvideo_format_settings *vesa_settings)
{
    if (settop_value == bvideo_format_vesa) {
        unsigned i;
        BDBG_ASSERT(vesa_settings);
        for (i=0;i<sizeof(g_vesaFormats)/sizeof(g_vesaFormats[0]);i++) {
            if (vesa_settings->width == g_vesaFormats[i].width &&
                vesa_settings->height == g_vesaFormats[i].height &&
                vesa_settings->refresh_rate == g_vesaFormats[i].refresh_rate)
            {
                return g_vesaFormats[i].vesaFormat;
            }
        }
        /* unknown */
        return NEXUS_VideoFormat_eVesa640x480p60hz;
    }
    else {
        CONVERT(g_videoFormat);
    }
}

bvideo_format b_nexus2displayformat(NEXUS_VideoFormat nexus_value, bvideo_format_settings *vesa_settings)
{
    if (nexus_value >= NEXUS_VideoFormat_eVesa640x480p60hz && nexus_value <= NEXUS_VideoFormat_eVesa1920x1080p60hzReducedBlank )
    {
        if (vesa_settings) {
            unsigned i;
            BKNI_Memset(vesa_settings, 0, sizeof(*vesa_settings));
            for (i=0;i<sizeof(g_vesaFormats)/sizeof(g_vesaFormats[0]);i++) {
                if (nexus_value == g_vesaFormats[i].vesaFormat) {
                    vesa_settings->width = g_vesaFormats[i].width;
                    vesa_settings->height = g_vesaFormats[i].height;
                    vesa_settings->refresh_rate = g_vesaFormats[i].refresh_rate;
                    vesa_settings->frame_rate = g_vesaFormats[i].refresh_rate * 100;
                    vesa_settings->interlaced = false;
                }
            }
        }
        return bvideo_format_vesa;
    }
    else
    {
        CONVERT_NEXUS(g_videoFormat);
    }
}

NEXUS_DisplayAspectRatio b_displayaspectratio2nexus(bdisplay_aspect_ratio settop_value)
{
    CONVERT(g_aspectRatio);
}

NEXUS_PixelFormat b_graphicspixelformat2nexus(bgraphics_pixel_format settop_value)
{
    CONVERT(g_pixelFormat);
}

NEXUS_MessageFormat b_messageformat2nexus(bmessage_format settop_value)
{
    CONVERT(g_messageFormat);
}

#if NEXUS_HAS_FRONTEND
NEXUS_FrontendSatelliteMode b_sdsmode2nexus(btuner_sds_mode settop_value)
{
    CONVERT(g_sdsMode);
}
#endif

struct {
    NEXUS_TransportType nexus;
    unsigned settop;
} g_mpegType[] = {
    {NEXUS_TransportType_eTs, bstream_mpeg_type_unknown},
    {NEXUS_TransportType_eEs, bstream_mpeg_type_es},
    {NEXUS_TransportType_eTs, bstream_mpeg_type_bes},
    {NEXUS_TransportType_eMpeg2Pes, bstream_mpeg_type_pes},
    {NEXUS_TransportType_eTs, bstream_mpeg_type_ts},
    {NEXUS_TransportType_eDssEs, bstream_mpeg_type_dss_es},
    {NEXUS_TransportType_eDssPes, bstream_mpeg_type_dss_pes},
    {NEXUS_TransportType_eVob, bstream_mpeg_type_vob},
    {NEXUS_TransportType_eAsf, bstream_mpeg_type_asf},
    {NEXUS_TransportType_eAvi, bstream_mpeg_type_avi},
    {NEXUS_TransportType_eMpeg1Ps, bstream_mpeg_type_mpeg1},
    {NEXUS_TransportType_eMp4, bstream_mpeg_type_mp4},
    {NEXUS_TransportType_eMkv, bstream_mpeg_type_mkv},
    {NEXUS_TransportType_eWav, bstream_mpeg_type_wav},
    {NEXUS_TransportType_eFlv, bstream_mpeg_type_flv},
    {NEXUS_TransportType_eRmff, bstream_mpeg_type_rmff},
    {NEXUS_TransportType_eOgg, bstream_mpeg_type_ogg}
};

NEXUS_TransportType b_mpegtype2nexus(bstream_mpeg_type settop_value)
{
    CONVERT(g_mpegType);
}

#if NEXUS_HAS_PLAYBACK
NEXUS_VideoDecoderDecodeMode b_decodermode2nexus(bplaypump_decode_mode settop_value)
{
    return (NEXUS_VideoDecoderDecodeMode)settop_value;
}
#endif

/*
Summary:
Cached memory allocation function
Description:
This allocates a buffer on the current heap and in cached memory for use elsewhere.
*/
bmem_ptr bmem_alloc(
    size_t buffer_length
)
{
    bresult rc;
    void *pMemory;
    NEXUS_Error errCode;
    NEXUS_MemoryAllocationSettings allocSettings;
    NEXUS_Memory_GetDefaultAllocationSettings(&allocSettings);
    errCode = NEXUS_Memory_Allocate(buffer_length, &allocSettings, &pMemory);
    if ( errCode )
    {
        rc = BSETTOP_ERROR(berr_external_error);
        return NULL;
    }
    return pMemory;
}

/*
Summary:
Memory de-allocation for external use
Description:
This frees a buffer allocated with bsettop_mem_alloc()
*/
void bmem_free(
    void *buffer
)
{
    BDBG_ASSERT(NULL != buffer);
    NEXUS_Memory_Free(buffer);
}

const char *bsettop_get_config(const char *name)
{
#if defined(__vxworks) || defined(NO_OS_DIAGS) || defined(UCOS_DIAGS)
    return NEXUS_GetEnv(name);
#else
    return getenv(name);
#endif
}

int b_strcmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

