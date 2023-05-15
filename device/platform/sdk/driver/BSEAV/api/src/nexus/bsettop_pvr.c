/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_pvr.c $
 * $brcm_Revision: 109 $
 * $brcm_Date: 3/1/12 4:09p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_pvr.c $
 * 
 * 109   3/1/12 4:09p erickson
 * SW7425-1264: set sign of trickmode rate correctly
 * 
 * 108   2/13/12 1:36p jessem
 * SW7425-2390: Fixed coverity issues.
 *
 * 107   1/16/12 3:23p erickson
 * SW7552-194: fix PVR_SUPPORT=n builds
 *
 * 106   10/26/11 3:41p erickson
 * SW7425-1264: clean up trick mode params to avoid invalid cases
 *
 * 105   5/12/11 3:46p jtna
 * SW7550-739: replace all instances of 'NEXUS_HAS_DMA &&
 * NEXUS_HAS_SECURITY' with 'NEXUS_ENCRYPTED_DVR_WITH_M2M'. replace some
 * instances of 'NEXUS_HAS_DMA' with 'NEXUS_NUM_DMA_CHANNELS'
 *
 * 104   4/21/11 11:03a jtna
 * SW7425-365: fix build warnings for 2.6.37 kernel
 *
 * 103   9/2/10 4:44p erickson
 * SWDEPRECATED-1003: added bplayback_player_mode_time_skip
 *
 * 102   6/24/10 12:25p mward
 * SW7125-491:  Coverity Defect ID:22323,  22324.  CHECKED_RETURN (from
 * NEXUS_Playback_GetStatus()).
 *
 * 101   6/23/10 1:00p erickson
 * SW7550-462: fix frame advance change-of-directions. bplayback_pause
 * needs to give control of trick mode selection back to nexus.
 *
 * 100   6/22/10 4:28p mphillip
 * SW7550-463: Clear keyslot when there is security but no DMA
 *
 * 99   6/18/10 3:09p erickson
 * SW7550-424: fix PVR_SUPPORT=n
 *
 * 98   6/11/10 11:19a mphillip
 * SW7550-380: Free keyslot after playback is stopped, not before.
 *
 * 97   6/10/10 4:59p erickson
 * SWDEPRECATED-2726: implement bsettop_fileio_fifo
 *
 * 96   6/10/10 2:45p mphillip
 * SW7550-380: Enable non-DMA PVR encryption for 7550 and similar
 * platforms
 *
 * 95   5/18/10 11:17a erickson
 * SW7550-424: fix warning for PVR_SUPPORT=n
 *
 * 94   2/11/10 2:54p erickson
 * SW7550-252: fix dma/security testes
 *
 * 93   1/8/10 10:11a erickson
 * SW7400-2645: tpit api change
 *
 * 92   12/22/09 1:37p erickson
 * SW3548-2677: fix warnings
 *
 * 91   12/1/09 4:51p mward
 * SW7125-129: Enable TPIT indexing for H264 recording. Needed for DQT.
 *
 * 91   12/1/09 4:49p mward
 * SW7125-129: Enable TPIT indexing for H264 recording. Needed for DQT.
 *
 * SW7125-129/1   11/25/09 4:25p mward
 * SW7125-129:  Enable TPIT indexing for H264 recording.  Needed for DQT.
 *
 * 90   9/24/09 1:46p gmohile
 * SW7405-2659 : DQT should be enabled for reverse trick modes
 *
 * 89   9/23/09 5:36p ahulse
 * SW7405-2724: Switch MAD off when in playback pause to allow accurate
 * frame stepping
 *
 * 88   9/9/09 6:48p ahulse
 * SW7405-2724: Add override so settopAPI can use accurate seek
 *
 * 87   9/7/09 4:44p ssood
 * SW7420-340: modified live streaming api to differentiate it w/ network
 * recording
 *
 * 86   9/1/09 2:00p gmohile
 * SW7403-861 : Fixed avi trickmodes
 *
 * 85   8/17/09 1:44p erickson
 * PR56180: fix warning
 *
 * 84   7/29/09 1:50p katrep
 * PR56180: cset th default max decode rate to 4 x normal playback speed
 *
 * 83   7/21/09 7:18p katrep
 * PR56977: updated set trickmodes according to nexus api definitions.
 *
 * 82   7/20/09 4:23p vishk
 * PR 56180: SettopAPI Shim: Allows user to specify expected maximum
 * decoder rate
 *
 * 81   7/17/09 12:24p katrep
 * PR56423: nexus playback module no longer set the audio muute
 * apllication need to mute the audio for host and decoder trickmodes.
 *
 * 80   7/13/09 3:56p jtna
 * PR56423: fix GOP trickmode
 *
 * 79   7/9/09 5:52p katrep
 * PR56707: Audi mute shuld be done after trick modes are set correctly
 * othrewise audio is lost
 *
 * 78   7/9/09 5:23p katrep
 * PR56703: Fixed trickmodes on video only streams
 *
 * 77   6/24/09 5:53p katrep
 * PR56180: Compiler warning
 *
 * 76   6/23/09 3:36p mward
 * PR 56274: PR 56275: Coverity Defect  FORWARD_NULL.
 *
 * 75   6/22/09 6:44p jtna
 * PR56180: NEXUS_PlaybackTrickModeSettings.overrideStcTrick is superceded
 * by maxDecoderRate
 *
 * 74   6/22/09 6:38p mward
 * PR 52099: Mute audio for slow trick modes <= half speed.
 *
 * 73   6/22/09 5:26p jtna
 * PR55571: fix I/IP-only decoder trick modes
 *
 * 72   6/15/09 4:59p jtna
 * PR43001: add support for TTS+encryption record/playback
 *
 * 71   6/12/09 11:10a jtna
 * PR43001: support TS/TTS record
 *
 * 70   5/11/09 4:32p ssood
 * PR54955: Record TTS streams over HTTP: indicate to IP Applib that it is
 * a TTS stream
 *
 * 69   4/21/09 4:13p katrep
 * PR52519: For mp4 and mkv file can do playback stop,start test
 *
 * 68   4/16/09 1:04p katrep
 * PR50207: Stop playback before closing the stream.
 *
 * 67   4/15/09 2:40p katrep
 * PR53824: Correctly use overrideStcTrick to perevnt all the trickmodes
 * to default to stc trickmodes.Also fixed issue of resume playback from
 * ongoing decoder trickmodes
 *
 * 66   4/9/09 3:43p erickson
 * PR52519: reenabled the test NEXUS_Playback_Start code so that files
 * with bad indexes can still play. added better comments so this code
 * doesn't get commented out again.
 *
 * 65   3/9/09 6:59p nickh
 * PR52996: Fix compilation errors when NEXUS_HAS_SECURITY is disabled
 *
 * 64   2/26/09 11:50a katrep
 * PR52519: Removed unnecessary nexus playbach start and stop to fix the
 * timeshifted record/playback in the latest nexus
 *
 * 63   2/25/09 6:44p katrep
 * PR52099: For decoder trickmodes need to mute audio outside of nexus
 *
 * 62   2/25/09 11:36a katrep
 * PR52099: Add support for decoder trick modes
 *
 * 61   2/4/09 6:18p katrep
 * PR47815: Add debug
 *
 * 60   2/2/09 2:54p gmohile
 * PR 47815 : Add manual control of trickmodes
 *
 * 59   12/24/08 12:42p mphillip
 * PR49607: Keyladder key derivation support added for encrypted PVR in
 * nexus shim
 *
 * 58   12/24/08 11:56a jgarrett
 * PR 50703: Allowing security module to be absent
 *
 * 57   11/10/08 5:49p jrubio
 * PR48782: make sure that ip is seperated by the B_HAS_IP flags
 *
 * 56   11/7/08 3:00p ssood
 * PR48782: added network record support
 *
 * 55   10/17/08 4:02p katrep
 * PR47690: Add option to generate index index on video pid during allpass
 * record, disabled by default.To enable define ALLPASS_RECORD_WITH_INDEX
 *
 * 54   10/16/08 6:20p katrep
 * PR47690: Adding allpass record from playback parser
 *
 * 53   10/15/08 2:42p mphillip
 * PR45211: Reset encryption parameters when doing an unencrypted record
 *
 * 52   10/9/08 2:24p erickson
 * PR47608: added stcTrick based on use_rap_trick_modes env variable
 *
 * 51   10/7/08 11:33p erickson
 * PR47232: NEXUS_PlaypumpSettings.timestamp api change
 *
 * 50   9/25/08 1:06p katrep
 * PR47154: Add support for recording with null packets and without null
 * packets
 *
 * 49   9/22/08 1:53p katrep
 * PR47154: More fine tuning of allpass record
 *
 * 48   9/19/08 7:49p katrep
 * PR47154: Add support for allpass record
 *
 * 47   9/15/08 5:34p jrubio
 * PR46925:  fix PVR_SUPPORT=n compile issue
 *
 * 46   9/4/08 6:00p vsilyaev
 * PR 44493: Propagate timestamp configuration
 *
 * 45   8/29/08 5:43p vsilyaev
 * PR 33812: Make goto_indx alias of goto_timestamp
 *
 * 44   8/8/08 6:27p katrep
 * PR45213: Bring the nexus playback setting to reset state after the
 * playback is stopped.
 *
 * 43   8/5/08 10:24a erickson
 * PR45422: add DIVX_DRM_SUPPORT
 *
 * 42   7/28/08 3:35p katrep
 * PR43242: Support changing the loopback modes inside the eof,bof
 * callbacks
 *
 * 41   7/22/08 2:59p erickson
 * PR44874: allow static allocation of bstream for playback. this is
 * required to support bstream_close after a bplayback_stop.
 *
 * 40   7/10/08 3:23p katrep
 * PR43636: Add support MSDRM ND
 *
 * 39   7/2/08 3:47p erickson
 * PR42150: added BDBG_ERR if not bplaypump_decode_mode_all
 *
 * 38   7/2/08 12:33p erickson
 * PR36836: report has_index correctly
 *
 * 37   6/27/08 3:36p mphillip
 * PR42901: Free PVR keyslot on close/stop, support encrypted record
 *
 * 36   6/26/08 6:21p mphillip
 * PR42901: Encrypted PVR playback support
 *
 * 35   6/25/08 6:12p vsilyaev
 * PR 41869: Use keySlotHandle instead of keySlot[Number]
 *
 * 34   6/24/08 4:10p erickson
 * PR44107: rename to TrickMode
 *
 * 33   6/16/08 6:29p vsilyaev
 * PR 43636: Fixed key passing
 *
 * 32   6/12/08 9:07p katrep
 * PR43636: Add support for MSDRM
 *
 * 31   6/11/08 7:20p ssood
 * PR42739: added virtual interface back to the playback module to allow
 * ip implementation in addition to the regular playback implementation
 *
 * 30   5/22/08 3:37p erickson
 * PR36068: remove 1.2x for now
 *
 * 29   5/16/08 3:38p erickson
 * PR36068: clean up DBG output
 *
 * 28   5/14/08 1:24p vsilyaev
 * PR 42119: Preserve PES->TS packetizer settings over
 * Playpump_Stop/Playpump_Start calls
 *
 * 27   5/13/08 5:14p erickson
 * PR42119: don't call NEXUS_Playback_Start until all pid channels are
 * opened
 *
 * 26   5/6/08 11:36a erickson
 * PR42365: impl playback callbacks
 *
 * 25   5/2/08 9:20a erickson
 * PR42339: fix pid channel alloc for PVR
 *
 * 24   5/1/08 4:08p erickson
 * PR42339: properly connect and disconnect timeshifting
 *
 * 23   4/28/08 11:54a erickson
 * PR42197: remove NEXUS_ParserBand_ePlayback enums
 *
 * 22   4/10/08 5:10p erickson
 * PR40832: set NEXUS_PlaybackSettings.timeshifting
 *
 * 21   4/10/08 9:58a erickson
 * PR36068: coverity fix
 *
 * 20   4/9/08 5:54p jgarrett
 * PR 41567: Opening playback video pid at start time
 *
 * 19   4/7/08 12:04p erickson
 * PR41431: call NEXUS_Playback_Start from inside bplayback_start
 *
 * 18   4/7/08 10:40a jgarrett
 * PR 41362: Revising pid channel management
 *
 * 17   4/4/08 10:43a jgarrett
 * PR 41312: Synchronizing callbacks
 *
 * 16   3/20/08 1:56a erickson
 * PR36068: impl goto_timestamp
 *
 * 15   3/14/08 3:02p katrep
 * PR40031:Implement some more shhim layer apis
 *
 * 15   3/14/08 2:59p katrep
 * PR40031:Implement some more shhim layer apis
 *
 * 14   3/13/08 12:34p erickson
 * PR36068: fix signed/unsigned math
 *
 * 13   3/6/08 4:10p erickson
 * PR36068: api change
 *
 * 12   2/4/08 10:07a erickson
 * PR36068: api update
 *
 * 11   1/8/08 3:21p erickson
 * PR36068: fix trick modes and record stop
 *
 * 10   1/2/08 9:55a erickson
 * PR36068: Playback changes
 *
 * 9   12/20/07 10:28a erickson
 * PR37590: add timeshifting
 *
 * 8   12/4/07 3:09p erickson
 * PR36068: playpumpSettings.transportType added
 *
 * 7   12/3/07 4:59p erickson
 * PR36068: rework audio, defer playback start until after decode start
 *
 * 6   11/14/07 1:29p erickson
 * PR36068: added record
 *
 * 5   11/13/07 11:58a erickson
 * PR36068: trick modes working
 *
 * 4   11/12/07 2:34p erickson
 * PR36068: update
 *
 * 3   10/16/07 12:35p erickson
 * PR36068: brutus up over settop api/nexus
 *
 * 2   10/15/07 4:13p erickson
 * PR36068: initial
 *
 * 1   10/15/07 2:36p erickson
 * PR36068: initial
 *
 ***************************************************************************/
#include "bsettop_impl.h"
#include <stdlib.h>

#if B_HAS_IP
#include "b_playback_ip_lib.h"
#endif

BDBG_MODULE(pvr);

/* #define ALLPASS_RECORD_WITH_INDEX 1 */

#if (defined NEXUS_HAS_PLAYBACK ) || (defined NEXUS_HAS_RECORD)
/* This is called from nexus and fires an event */
static void b_generic_callback(void *context, int param)
{

    B_EventHandle event = (B_EventHandle)param;
    BSTD_UNUSED(context);
    B_Event_Set(event);
}
#endif

#if NEXUS_HAS_PLAYBACK
static void bplayback_p_end_of_stream_handler(void *context)
{
    bplayback_t playback = context;
    if (playback->params.end_of_stream) {
        bplayback_loopmode loopmode;
        b_unlock();
        loopmode = (*playback->params.end_of_stream)(playback->params.callback_context);
        b_lock();
        if (loopmode != playback->params.loopmode.end_of_stream) {
            NEXUS_PlaybackSettings nPlaybackSettings;
            NEXUS_PlaybackStatus nPlaybackStatus;
            NEXUS_Playback_GetSettings(playback->nPlayback,&nPlaybackSettings);
            nPlaybackSettings.endOfStreamAction = (NEXUS_PlaybackLoopMode)loopmode;
            NEXUS_Playback_SetSettings(playback->nPlayback,&nPlaybackSettings);
            playback->params.loopmode.end_of_stream = loopmode;
            if (NEXUS_Playback_GetStatus(playback->nPlayback,&nPlaybackStatus)!= NEXUS_SUCCESS) {
                BDBG_ERR(("%s: ERROR: NEXUS_Playback_GetStatus() Failed\n", __FUNCTION__));
				return;
			}
            switch(nPlaybackStatus.state)
            {
            default:
            case NEXUS_PlaybackState_eStopped:
            case NEXUS_PlaybackState_eTrickMode:
                /* do nothing just changed the loop mode */
                break;
            case NEXUS_PlaybackState_ePlaying:
                if(loopmode == bplayback_loopmode_pause)
                    NEXUS_Playback_Pause(playback->nPlayback);
                break;
            case NEXUS_PlaybackState_ePaused:
                if(loopmode == bplayback_loopmode_loop)
                    NEXUS_Playback_Play(playback->nPlayback);
                break;
            }
            BDBG_WRN(("eof changing the playback loopmode to %d",nPlaybackSettings.endOfStreamAction));
        }
    }
}
#endif


#if NEXUS_HAS_PLAYBACK
static void bplayback_p_beginning_of_stream_handler(void *context)
{
    bplayback_t playback = context;
    if (playback->params.beginning_of_stream) {
        bplayback_loopmode loopmode;
        b_unlock();
        loopmode = (*playback->params.beginning_of_stream)(playback->params.callback_context);
        b_lock();
        if (loopmode != playback->params.loopmode.beginning_of_stream) {
            NEXUS_PlaybackSettings nPlaybackSettings;
            NEXUS_PlaybackStatus nPlaybackStatus;
            NEXUS_Playback_GetSettings(playback->nPlayback,&nPlaybackSettings);
            nPlaybackSettings.beginningOfStreamAction = (NEXUS_PlaybackLoopMode)loopmode;
            NEXUS_Playback_SetSettings(playback->nPlayback,&nPlaybackSettings);
            playback->params.loopmode.beginning_of_stream = loopmode;
            if (NEXUS_Playback_GetStatus(playback->nPlayback,&nPlaybackStatus)!= NEXUS_SUCCESS) {
                BDBG_ERR(("%s: ERROR: NEXUS_Playback_GetStatus() Failed\n", __FUNCTION__));
				return;
			}
            switch(nPlaybackStatus.state)
            {
            default:
            case NEXUS_PlaybackState_eStopped:
            case NEXUS_PlaybackState_eTrickMode:
                /* do nothing just changed the loop mode */
                break;
            case NEXUS_PlaybackState_ePlaying:
                if(loopmode == bplayback_loopmode_pause)
                    NEXUS_Playback_Pause(playback->nPlayback);
                break;
            case NEXUS_PlaybackState_ePaused:
                if(loopmode == bplayback_loopmode_loop)
                    NEXUS_Playback_Play(playback->nPlayback);
                break;
            }
            BDBG_WRN(("bof changing the playback loopmode to %d",nPlaybackSettings.beginningOfStreamAction));
        }
    }
}
#endif

#if NEXUS_HAS_PLAYBACK
static void bplayback_p_error_handler(void *context)
{
    bplayback_t playback = context;
    if (playback->params.error) {
        b_unlock();
        (*playback->params.error)(playback->params.callback_context);
        b_lock();
    }
}
#endif

bplayback_file_t bplayback_file_open(const char *mpeg_file_name, const char *index_file_name)
{
    return bplayback_p_file_open(mpeg_file_name, index_file_name, NULL);
}

bplayback_file_t bplayback_p_file_open(const char *mpeg_file_name, const char *index_file_name, bfile_out_fifo_t writer)
{
#if NEXUS_HAS_PLAYBACK
    bplayback_file_t file = BKNI_Malloc(sizeof(*file));
    BDBG_WRN(("FILE %s , %s",mpeg_file_name, index_file_name ));
    file->has_index = (index_file_name != NULL);

    if (writer) {
        file->nFile = NEXUS_FifoPlay_Open(mpeg_file_name, index_file_name, ((brecord_file_t)writer)->nFileFifo);
    }
    else {
        file->nFile = NEXUS_FilePlay_OpenPosix(mpeg_file_name, index_file_name);
    }

    if (!file->nFile) {
        bplayback_file_close(file);
        return NULL;
    }

    return file;
#else
    BSTD_UNUSED(mpeg_file_name);
    BSTD_UNUSED(index_file_name);
    BSTD_UNUSED(writer);
    return NULL;
#endif
}

void bplayback_file_close(bplayback_file_t file)
{
#if NEXUS_HAS_PLAYBACK
    BDBG_MSG(("%s: file %p\n", __FUNCTION__, file));
    if (file->nFile) {
        NEXUS_FilePlay_Close(file->nFile);
    }
    BKNI_Free(file);
#else
    BSTD_UNUSED(file);
#endif
}

void bplayback_params_init_indx(bplayback_params *params, bplayback_t playback)
{
    BSTD_UNUSED(playback);
    BKNI_Memset(params, 0, sizeof(*params));
}

/* start playback now that we have a consumer */
bresult bplayback_p_start(bplayback_t playback)
{
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlaybackSettings nSettings;
    NEXUS_Error rc;

    BDBG_MSG(("bplayback_p_start"));
    NEXUS_Playback_GetSettings(playback->nPlayback, &nSettings);
    nSettings.stcChannel = playback->stream->consumers.decode ? playback->stream->consumers.decode->stcChannel : NULL;
    nSettings.startPaused = false;
    nSettings.stcTrick = !bsettop_get_config("use_rap_trick_modes") && nSettings.stcChannel ;
    nSettings.accurateSeek = bsettop_get_config("use_accurate_seek");
    rc = NEXUS_Playback_SetSettings(playback->nPlayback, &nSettings);

    /* We don't call NEXUS_Playback_Start until after we have a consumer. The media framework requires all pids (video and audio)
    before processing any data. */
    rc = NEXUS_Playback_Start(playback->nPlayback, playback->source->nFile, NULL);
    if (rc) return BSETTOP_ERROR(rc);
#else
    BSTD_UNUSED(playback);
#endif

    return 0;

}

bstream_t bplayback_start_timeshifting_indx(bplayback_t playback, bplaypump_t playpump, const bstream_mpeg *mpeg,
    bplayback_file_t source, const bplayback_params *params, brecord_t record)
{
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlaybackSettings nSettings;
    NEXUS_Error rc;

   BDBG_MSG(("bplayback_start %p", playback));
    if (playback->stream) {
        BDBG_ERR(("already started"));
        return NULL;
    }

    /* set minimum settings for test start */
    NEXUS_Playback_GetSettings(playback->nPlayback, &nSettings);
    nSettings.playpump = playpump->nPlaypump;
    nSettings.playpumpSettings.transportType = b_mpegtype2nexus(mpeg->mpeg_type);
    rc = NEXUS_Playback_SetSettings(playback->nPlayback, &nSettings);
    if (rc) {BSETTOP_ERROR(rc); return NULL;}

    /* mkv and mp4 can not be started without pid channnel configured */
    if (mpeg->mpeg_type != bstream_mpeg_type_mkv &&  mpeg->mpeg_type != bstream_mpeg_type_mp4 && mpeg->mpeg_type != bstream_mpeg_type_avi)
    {
        /* this is just a test start. this is necessary to see if the file and index are good.
        if not, we want bplayback_start to fail immediately.
        actual playback will start when consumer is started (record or decode)*/
        rc = NEXUS_Playback_Start(playback->nPlayback, source->nFile, NULL);
        if (rc!=NEXUS_SUCCESS)
        {
            rc = BSETTOP_ERROR(rc);
            return NULL;
        }
        NEXUS_Playback_Stop(playback->nPlayback);
    }

    /* Now, continue setting the rest of the settings. */
    /* The nexus shim uses bplayback_params.loopmode.XXX. Magnum-based Settop API does not. */
    BDBG_CASSERT(bplayback_loopmode_play == (bplayback_loopmode)NEXUS_PlaybackLoopMode_ePlay);
    nSettings.endOfStreamAction = (bplayback_loopmode)params->loopmode.end_of_stream;
    nSettings.beginningOfStreamAction = (bplayback_loopmode)params->loopmode.beginning_of_stream;
    nSettings.timeshifting = (record != NULL);
    nSettings.endOfStreamCallback.callback = b_generic_callback;
    nSettings.endOfStreamCallback.context = playback;
    nSettings.endOfStreamCallback.param = (int)playback->end_of_stream_event;
    nSettings.beginningOfStreamCallback.callback = b_generic_callback;
    nSettings.beginningOfStreamCallback.context = playback;
    nSettings.beginningOfStreamCallback.param = (int)playback->beginning_of_stream_event;
    nSettings.errorCallback.callback = b_generic_callback;
    nSettings.errorCallback.context = playback;
    nSettings.errorCallback.param = (int)playback->error_event;
    nSettings.playpumpSettings.timestamp.type = params->timestamp_enabled ?
        NEXUS_TransportTimestampType_eMod300 : /* TODO: DSS */
        NEXUS_TransportTimestampType_eNone;

    if (mpeg->mpeg_type == bstream_mpeg_type_ts && mpeg->encryption.type != bencryption_type_none) {
#if NEXUS_HAS_SECURITY
        if (!mpeg->encryption.key_ladder) {
            switch (mpeg->encryption.type) {
            case bencryption_type_des:
                if (mpeg->encryption.key_length != 64) {
                    BSETTOP_ERROR(berr_invalid_parameter);
                    return NULL;
                }
                break;
            case bencryption_type_3des:
                if (mpeg->encryption.key_length != 128) {
                    BSETTOP_ERROR(berr_invalid_parameter);
                    return NULL;
                }
                break;
            case bencryption_type_aes:
                if (mpeg->encryption.key_length != 128) {
                    BSETTOP_ERROR(berr_invalid_parameter);
                    return NULL;
                }
                break;
            default:
                BDBG_ERR(("Unsupported PVR encryption algorithm"));
                BSETTOP_ERROR(berr_invalid_parameter);
                return NULL;
            }
        } else if (mpeg->encryption.key_length != 8*sizeof(bcrypto_keyladder_data) || !mpeg->encryption.long_key) {
            BSETTOP_ERROR(berr_invalid_parameter); return NULL;
        }
        nSettings.playpumpSettings.securityContext = b_keyslot_m2m_allocate(&mpeg->encryption, false, params->timestamp_enabled);
        playback->hKeySlot = nSettings.playpumpSettings.securityContext;
#if NEXUS_NUM_DMA_CHANNELS
        nSettings.playpumpSettings.securityDma = g_dma.hDma;
        BDBG_MSG(("Enabling decryption on playback: slot: %p, dma: %p",nSettings.playpumpSettings.securityContext,nSettings.playpumpSettings.securityDma));
#else
        BDBG_MSG(("Enabling decryption on playback: slot: %p",nSettings.playpumpSettings.securityContext));
#endif
        if (!nSettings.playpumpSettings.securityContext
#if NEXUS_NUM_DMA_CHANNELS
                || !nSettings.playpumpSettings.securityDma
#endif
                ) {
            BDBG_ERR(("Enabling encryption on playback FAILED!"));
#if NEXUS_NUM_DMA_CHANNELS
            BDBG_ERR(("slot: %p, dma: %p",nSettings.playpumpSettings.securityContext,nSettings.playpumpSettings.securityDma));
#else
            BDBG_ERR(("slot: %p",nSettings.playpumpSettings.securityContext));
#endif
            BSETTOP_ERROR(berr_external_error);
            return NULL;
        }
#else
        BDBG_ERR(("PVR encryption is not supported"));
        BSETTOP_ERROR(berr_invalid_parameter);
        return NULL;
#endif
    }
#if B_HAS_MSDRM_PD || B_HAS_MSDRM_ND
    if(mpeg->mpeg_type == bstream_mpeg_type_asf) {
        nSettings.playpumpSettings.securityContext = (NEXUS_KeySlotHandle) (*(void **)mpeg->encryption.key);
    }
#endif
#if B_HAS_DIVX_DRM
    if(mpeg->mpeg_type == bstream_mpeg_type_avi) {
        nSettings.playpumpSettings.securityContext = (NEXUS_KeySlotHandle) (*(void **)mpeg->encryption.key);
    }
#endif

    nSettings.startPaused = true;
    rc = NEXUS_Playback_SetSettings(playback->nPlayback, &nSettings);
    if (rc) {BSETTOP_ERROR(rc); return NULL;}

    if (record) {
        rc = NEXUS_Record_AddPlayback(record->nRecord, playback->nPlayback);
        if (rc) {rc = BSETTOP_ERROR(rc); return NULL;}
        playback->record = record;
        record->playback = playback;
    }

    playback->stream = playback->stream_alloc;
    rc = bstream_p_set(playback->stream_alloc, mpeg);
    if (rc) {rc = BSETTOP_ERROR(rc); goto err_stream;}

    playback->params = *params;
    playback->source = source;

    return playback->stream;

err_stream:
    playback->stream = NULL;
    if ( record )
    {
        NEXUS_Record_RemovePlayback(record->nRecord, playback->nPlayback);
        playback->record = NULL;
        record->playback = NULL;
    }
#else
    BSTD_UNUSED(playback);
    BSTD_UNUSED(mpeg);
    BSTD_UNUSED(playpump);
    BSTD_UNUSED(record);
    BSTD_UNUSED(params);
    BSTD_UNUSED(source);
#endif

    return NULL;
}

bstream_t bplayback_start_indx(bplayback_t playback, bplaypump_t playpump, const bstream_mpeg *mpeg,
    bplayback_file_t source, const bplayback_params *params)
{
#if NEXUS_HAS_PLAYBACK
    return bplayback_start_timeshifting_indx(playback, playpump, mpeg, source, params, NULL);
#else
    BSTD_UNUSED(playback);
    BSTD_UNUSED(playpump);
    BSTD_UNUSED(mpeg);
    BSTD_UNUSED(source);
    BSTD_UNUSED(params);
    return NULL;
#endif
}

bresult bplayback_stop_timeshifting_indx(bplayback_t playback)
{
#if NEXUS_HAS_PLAYBACK
    if (playback->record) {
        NEXUS_Error rc;
        NEXUS_PlaybackSettings nPlaybackSettings;

        NEXUS_Playback_GetSettings(playback->nPlayback,&nPlaybackSettings);
        nPlaybackSettings.timeshifting = false;
        NEXUS_Playback_SetSettings(playback->nPlayback,&nPlaybackSettings);

        rc = NEXUS_Record_RemovePlayback(playback->record->nRecord, playback->nPlayback);
        if (rc) return BSETTOP_ERROR(rc);
        playback->record->playback = NULL;
        playback->record = NULL;
    }
#else
    BSTD_UNUSED(playback);
#endif
    return 0;
}

bresult bplayback_stop_indx(bplayback_t playback)
{
#if NEXUS_HAS_PLAYBACK

    NEXUS_PlaybackSettings nPlaybackSettings;

    BDBG_MSG(("bplayback_stop %p", playback));
    if (!playback->stream) {
        BDBG_ERR(("not started"));
        return BSETTOP_ERROR(berr_not_available);
    }

    if (playback->record) {
        bplayback_stop_timeshifting_indx(playback);
    }

    /*PR50207:playback must be stopped before closing the stream */
    NEXUS_Playback_Stop(playback->nPlayback);
    bstream_p_close(playback->stream);

    playback->stream = NULL;

    if (playback->hKeySlot) {
        b_keyslot_m2m_free(playback->hKeySlot);
        playback->hKeySlot = NULL;
    }

    /* reset playback settings */
    NEXUS_Playback_GetDefaultSettings(&nPlaybackSettings);
    NEXUS_Playback_SetSettings(playback->nPlayback,&nPlaybackSettings);
#else
    BSTD_UNUSED(playback);
#endif

    return 0;
}

void bplayback_trickmode_params_init(bplayback_trickmode_params *params)
{
    BKNI_Memset(params, 0, sizeof(*params));
    params->mode = bplayback_player_mode_normal;
    params->mode_modifier = 1; /* don't care */
    params->slow_motion_rate = 1; /* none */
    params->decode_mode = bplaypump_decode_mode_all;
    /* By default allow 4x STC trickmodes */
    params->maxDecoderRate = 4*BPLAYBACK_NORMALPLAY_SPEED;
}

bresult bplayback_get_status_indx(bplayback_t p, bplayback_status *status)
{
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlaybackStatus nStatus;
    NEXUS_Error rc;

    BKNI_Memset(status, 0, sizeof(*status));

    rc = NEXUS_Playback_GetStatus(p->nPlayback, &nStatus);
    if (rc) return BSETTOP_ERROR(rc);

    status->position.mpeg_file_offset = nStatus.position;
    status->position.index_offset = nStatus.position;
    status->position.has_index = p->source->has_index;
    status->position.timestamp = nStatus.position;

    status->mpeg_file_size = nStatus.last; /* Nexus doesn't provide this. Get from File if needed. */
    status->has_index = p->source->has_index;
    status->index_first_offset = nStatus.first; /* Nexus doesn't provide this. Not needed. */
    status->index_last_offset = nStatus.last; /* Nexus doesn't provide this. Not needed. */
    status->fifo_depth = nStatus.fifoDepth;
    status->fifo_size = nStatus.fifoSize;
    status->bytes_played = nStatus.bytesPlayed;
    status->last_timestamp = nStatus.last;
    status->first_timestamp = nStatus.first;

    switch (nStatus.state) {
    default:
    case NEXUS_PlaybackState_eStopped: status->state = bplayback_state_stopped; break;
    case NEXUS_PlaybackState_ePlaying: status->state = bplayback_state_playing; break;
    case NEXUS_PlaybackState_ePaused: status->state = bplayback_state_paused; break;
    case NEXUS_PlaybackState_eTrickMode: status->state = bplayback_state_trickmode; break;
    /* note: no frameadvance */
    }
    status->trickmode_params = p->trick_mode_params;
#else
    BSTD_UNUSED(p);
    BSTD_UNUSED(status);
#endif

    return 0;
}

bresult bplayback_pause_indx(bplayback_t p)
{
#if NEXUS_HAS_PLAYBACK
    NEXUS_Error rc;
    NEXUS_PlaybackTrickModeSettings trickModeSettings;
    bdecode_window_t window = p->stream->consumers.decode->window;

    bdecode_p_window_set_no_delay( window, true );

    /* Give nexus control of the trick mode selection for frame advance. this allows for change of direction,
    brcm/host selection based on codec, etc.
    This is needed because bplayback_trickmode_indx() takes that trick mode control away from nexus in order to
    map the old Settop API trick modes directly to the video decoder. */
    NEXUS_Playback_GetDefaultTrickModeSettings(&trickModeSettings);
    trickModeSettings.mode = NEXUS_PlaybackHostTrickMode_eNone;
    trickModeSettings.rate = 0; /* this is actually a don't care, but setting to 0 is safe.
                                   the actual pause is NEXUS_Playback_Pause */
    rc = NEXUS_Playback_TrickMode(p->nPlayback, &trickModeSettings);
    if (rc) return BSETTOP_ERROR(rc);

    rc = NEXUS_Playback_Pause(p->nPlayback);
    if (rc) return BSETTOP_ERROR(rc);
#else
    BSTD_UNUSED(p);
#endif

    return 0;
}

bresult bplayback_catchup_record_indx(bplayback_t p)
{
#if NEXUS_HAS_PLAYBACK
    NEXUS_PlaybackStatus playbackStatus;
    NEXUS_Error rc;

    rc = NEXUS_Playback_GetStatus(p->nPlayback, &playbackStatus);
    if (rc) return BERR_TRACE(rc);

    rc = NEXUS_Playback_Seek(p->nPlayback, playbackStatus.last - 3000);
    if (rc) return BERR_TRACE(rc);
#else
    BSTD_UNUSED(p);
#endif
    return 0;
}

bresult bplayback_play_indx(bplayback_t p)
{
#if NEXUS_HAS_PLAYBACK
    NEXUS_Error rc;
    bdecode_t decode = p->stream->consumers.decode;
    NEXUS_AudioDecoderTrickState audioDecoderTrickState;
    bdecode_window_t window = p->stream->consumers.decode->window;
    BDBG_MSG(("bplayback_play_indx"));

    bdecode_p_window_set_no_delay( window, false );

    rc = NEXUS_Playback_Play(p->nPlayback);
    if (rc) return BSETTOP_ERROR(rc);
    /* restore to normal playback if we were doing decoder trickmodes */
    if (decode && decode->video_decode && decode->video_decode->nVideoDecoder)
    {
        NEXUS_VideoDecoderTrickState decoderTrickState;
        NEXUS_VideoDecoder_GetTrickState(decode->video_decode->nVideoDecoder,&decoderTrickState);
        if (decoderTrickState.decodeMode != NEXUS_VideoDecoderDecodeMode_eAll)
        {
            decoderTrickState.decodeMode = NEXUS_VideoDecoderDecodeMode_eAll;
            rc = NEXUS_VideoDecoder_SetTrickState(decode->video_decode->nVideoDecoder,&decoderTrickState);
            if (rc) return BSETTOP_ERROR(rc);

        }
    }
    if (decode && decode->audio_decode && decode->audio_decode->nAudioDecoder)
    {
        NEXUS_AudioDecoder_GetTrickState(decode->audio_decode->nAudioDecoder,&audioDecoderTrickState);
        if (audioDecoderTrickState.muted)
        {
            audioDecoderTrickState.muted=false;
            audioDecoderTrickState.rate=NEXUS_NORMAL_DECODE_RATE;
            rc = NEXUS_AudioDecoder_SetTrickState(decode->audio_decode->nAudioDecoder,&audioDecoderTrickState);
            if (rc) return BSETTOP_ERROR(rc);
        }
    }
    if (decode && decode->audio_decode && decode->audio_decode->secondary_audio_decode && decode->audio_decode->secondary_audio_decode->nAudioDecoder)
    {
        NEXUS_AudioDecoder_GetTrickState(decode->audio_decode->secondary_audio_decode->nAudioDecoder,&audioDecoderTrickState);
        if (audioDecoderTrickState.muted)
        {
            audioDecoderTrickState.muted=false;
            audioDecoderTrickState.rate=NEXUS_NORMAL_DECODE_RATE;
            rc = NEXUS_AudioDecoder_SetTrickState(decode->audio_decode->secondary_audio_decode->nAudioDecoder,&audioDecoderTrickState);
            if (rc) return BSETTOP_ERROR(rc);
        }
    }

#else
    BSTD_UNUSED(p);
#endif

    return 0;
}

bresult bplayback_frameadvance_indx(bplayback_t p, bplayback_player_mode playermode, bool forward)
{
#if NEXUS_HAS_PLAYBACK
    NEXUS_Error rc;

    BSTD_UNUSED(playermode);

    rc = NEXUS_Playback_FrameAdvance(p->nPlayback, forward);
    if (rc) return BSETTOP_ERROR(rc);
#else
    BSTD_UNUSED(p);
    BSTD_UNUSED(playermode);
    BSTD_UNUSED(forward);
#endif
    return 0;
}

bresult bplayback_trickmode_indx(bplayback_t p, const bplayback_trickmode_params *trickmode)
{
#if NEXUS_HAS_PLAYBACK
    NEXUS_Error rc;
    NEXUS_PlaybackTrickModeSettings trickModeSettings;
    int rate;
    bdecode_t decode = p->stream->consumers.decode;

    BDBG_MSG(("trickmode %d,decode trickmode %d,modifier %d",trickmode->mode,trickmode->decode_mode,trickmode->mode_modifier));

    NEXUS_Playback_GetDefaultTrickModeSettings(&trickModeSettings);

    /* Convert from Settop API trick mode to Nexus rate. This involves some large
    assumptions. The only accurate method is to use bplayback_player_mode_by_rate. */
    rate = NEXUS_NORMAL_PLAY_SPEED;

    switch (trickmode->mode) {
    case bplayback_player_mode_normal:
        trickModeSettings.mode = NEXUS_PlaybackHostTrickMode_eNormal;
        break;
    case bplayback_player_mode_i:
        trickModeSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayI;
        break;
    case bplayback_player_mode_skip_b:
        trickModeSettings.mode = NEXUS_PlaybackHostTrickMode_ePlaySkipB;
        break;
    case bplayback_player_mode_ip:
        trickModeSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayIP;
        break;
    case bplayback_player_mode_skip_p:
        trickModeSettings.mode = NEXUS_PlaybackHostTrickMode_ePlaySkipP;
        break;
    case bplayback_player_mode_brcm:
        trickModeSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayBrcm;
        break;
    case bplayback_player_mode_by_gop:
        trickModeSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayGop;
        break;
    case bplayback_player_mode_time_skip:
        trickModeSettings.mode = NEXUS_PlaybackHostTrickMode_eTimeSkip;
        break;
    default:
    case bplayback_player_mode_by_rate:
        trickModeSettings.mode = NEXUS_PlaybackHostTrickMode_eNone;
        rate = trickModeSettings.rate = trickmode->mode_modifier * NEXUS_NORMAL_PLAY_SPEED / BPLAYBACK_NORMALPLAY_SPEED;
        break;
    }

    if (trickModeSettings.mode != NEXUS_PlaybackHostTrickMode_eNone) {
        /* Decoder trick mode handling. requires NEXUS_PlaybackHostTrickMode be custom, not automatic. */
        if (decode && decode->video_decode && decode->video_decode->nVideoDecoder)
        {
            switch (trickmode->decode_mode)
            {
            case bplaypump_decode_mode_i:
                trickModeSettings.skipControl = NEXUS_PlaybackSkipControl_eDecoder;
                trickModeSettings.rateControl = NEXUS_PlaybackRateControl_eDecoder;
                trickModeSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayI;
                break;
            case bplaypump_decode_mode_ip:
                trickModeSettings.skipControl = NEXUS_PlaybackSkipControl_eDecoder;
                trickModeSettings.rateControl = NEXUS_PlaybackRateControl_eDecoder;
                trickModeSettings.mode = NEXUS_PlaybackHostTrickMode_ePlayIP;
                break;
            default:
                /*trickModeSettings.skipControl = NEXUS_PlaybackSkipControl_eHost;
                trickModeSettings.rateControl=NEXUS_PlaybackRateControl_eStream;*/
                break;
            }
        }

        trickModeSettings.mode_modifier = trickmode->mode_modifier;
    }

    if (trickmode->slow_motion_rate > 1) {
        rate /= (int)trickmode->slow_motion_rate;
    }

    trickModeSettings.rate = trickModeSettings.mode_modifier < 0 ? -rate : rate;
    trickModeSettings.maxDecoderRate = trickmode->maxDecoderRate * NEXUS_NORMAL_PLAY_SPEED/BPLAYBACK_NORMALPLAY_SPEED;

    if( bsettop_get_config("max_decoder_rate")) {
        trickModeSettings.maxDecoderRate = atoi(bsettop_get_config("max_decoder_rate"))*NEXUS_NORMAL_PLAY_SPEED;
    }

    BDBG_WRN(("trickmode rate %d,mode %d,%s rate ctrl,%s skip mode,max decode rate %d",
              trickModeSettings.rate,trickModeSettings.mode,trickModeSettings.rateControl?"decoder":"stream",
              trickModeSettings.skipControl?"decoder":"host",trickModeSettings.maxDecoderRate));

    rc = NEXUS_Playback_TrickMode(p->nPlayback, &trickModeSettings);
    if (rc) return BSETTOP_ERROR(rc);

    p->trick_mode_params = *trickmode;
#else
    BSTD_UNUSED(trickmode);
    BSTD_UNUSED(p);
#endif

    return 0;
}

static const bplayback_trickmode_params
g_revTrickModes[] = {
    {bplayback_player_mode_brcm,  -1, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_brcm,  -2, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_brcm,  -5, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_brcm,  -7, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_i,     -1, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_i,     -2, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_i,     -5, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_i,     -10, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_i,     -25, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_i,     -50, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_i,     -100, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_i,     -200, 1, bplaypump_decode_mode_all, 0, 0 }
};

static const bplayback_trickmode_params
g_fwdTrickModes[] = {
#if 0
/* not supported yet */
    {bplayback_player_mode_by_rate, 120, 1, bplaypump_decode_mode_all, 0 }, /* 1.2x STC trick mode */
#endif
    {bplayback_player_mode_skip_b,  1, 1, bplaypump_decode_mode_all, 0, 0 }, /* play only one B between P's and I's */
    {bplayback_player_mode_ip,     0, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_skip_p,  1, 1, bplaypump_decode_mode_all, 0, 0 }, /* play only one P between I's */
    {bplayback_player_mode_i,      1, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_i,      2, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_i,      5, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_i,      10, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_i,      25, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_i,      50, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_i,      100, 1, bplaypump_decode_mode_all, 0, 0 },
    {bplayback_player_mode_i,      200, 1, bplaypump_decode_mode_all, 0, 0 }
};

static void trick_getTrickModeSettings(int rate, bplayback_trickmode_params *params)
{
    if (rate > 1) {
        int total;
        rate -= 2;
        total = sizeof(g_fwdTrickModes)/sizeof(g_fwdTrickModes[0]);
        if (rate >= total) rate = total-1;
        *params = g_fwdTrickModes[rate];
    }
    else if (rate < 0) {
        int total;
        rate *= -1;
        rate -= 1;
        total = sizeof(g_revTrickModes)/sizeof(g_revTrickModes[0]);
        if (rate >= total) rate = total-1;
        *params = g_revTrickModes[rate];
    }
    else {
        static bplayback_trickmode_params normal = {bplayback_player_mode_normal, 1, 1, bplaypump_decode_mode_all, 0, 0};
        *params = normal;
    }
}

bresult bplayback_get_trickmode_by_rate_indx(bplayback_t p, int rate, bplayback_trickmode_params *params)
{

    bool allow_brcm_trick_modes = bsettop_get_config("no_brcm_trick_modes") == NULL;
    BSTD_UNUSED(p);

    bplayback_trickmode_params_init(params);

    if (rate == BPLAYBACK_NORMALPLAY_SPEED || rate == 0) {
        /* normal play isn't a trick mode */
        return BSETTOP_ERROR(berr_invalid_parameter);
    } else if (rate > BPLAYBACK_NORMALPLAY_SPEED) {
        /* fast forward */
        int fastrate = (rate + BPLAYBACK_NORMALPLAY_SPEED - 1) / BPLAYBACK_NORMALPLAY_SPEED;
        BDBG_ASSERT(fastrate >= 2);
        trick_getTrickModeSettings(fastrate, params);
    } else if (rate <= -BPLAYBACK_NORMALPLAY_SPEED) {
        /* rewind, including 1x rewind */
        int fastrate = rate / BPLAYBACK_NORMALPLAY_SPEED;
        BDBG_ASSERT(fastrate <= -1);
        trick_getTrickModeSettings(fastrate, params);
    } else {
        /* slow forward or reverse */
        if (rate < 0) {
            params->mode = allow_brcm_trick_modes ? bplayback_player_mode_brcm : bplayback_player_mode_i;
            params->mode_modifier = -1;
            rate = -rate;
        }
        params->slow_motion_rate = BPLAYBACK_NORMALPLAY_SPEED/rate;
    }
    return b_ok;
}


long bplayback_goto_timestamp_indx(bplayback_t p, long timestamp, int whence)
{
#if NEXUS_HAS_PLAYBACK

    NEXUS_Error rc;
    NEXUS_PlaybackStatus nStatus;

    rc = NEXUS_Playback_GetStatus(p->nPlayback, &nStatus);
    if (rc) {BSETTOP_ERROR(rc); return -1;}

    switch (whence) {
    case SEEK_SET:
        break;
    case SEEK_CUR:
        timestamp += nStatus.position;
        break;
    case SEEK_END:
        timestamp += nStatus.last;
        break;
    }

    rc = NEXUS_Playback_Seek(p->nPlayback, timestamp);
    if (rc) {BSETTOP_ERROR(rc); return -1;}

    rc = NEXUS_Playback_GetStatus(p->nPlayback, &nStatus);
    if (rc) {BSETTOP_ERROR(rc); return -1;}

    return nStatus.position;
#else
    BSTD_UNUSED(p);
    BSTD_UNUSED(timestamp);
    BSTD_UNUSED(whence);
    return 0;
#endif

}

long bplayback_goto_index_indx(bplayback_t p, long index_offset, int whence)
{
    /* TODO: Nexus support seek into streams in terms of time
       we need figure out ways to convert index_offset to msec */
    return bplayback_goto_timestamp_indx(p, index_offset, whence);
}

off_t bplayback_goto_mpeg_file_offset_indx(bplayback_t p, off_t mpeg_file_offset, int whence)
{
    /* TODO: Nexus support seek into streams in terms of time
       we need figure out ways to convert file offset to msec in the stream */
    BSTD_UNUSED(p);
    BSTD_UNUSED(mpeg_file_offset);
    BSTD_UNUSED(whence);
    BSETTOP_ERROR(berr_not_supported);
    return -1;
}

#if NEXUS_HAS_RECORD
static void brecord_p_error_handler(void *context);
static void brecord_p_overflow_handler(void *context);
#endif

brecord_t brecord_open(void)
{

#if NEXUS_HAS_RECORD
    brecord_t record = BKNI_Malloc(sizeof(*record));
    BKNI_Memset(record, 0, sizeof(*record));

    record->nRecord = NEXUS_Record_Create();
    if (!record->nRecord) {
        BKNI_Free(record);
        return NULL;
    }

    record->errorEvent = B_Event_Create(NULL);
    if ( NULL == record->errorEvent )
    {
        BSETTOP_ERROR(berr_external_error);
        goto error;
    }

    record->overflowEvent = B_Event_Create(NULL);
    if ( NULL == record->overflowEvent )
    {
        BSETTOP_ERROR(berr_external_error);
        goto error;
    }

    record->errorEventId = b_event_register(record->errorEvent, brecord_p_error_handler, record);
    if ( NULL == record->errorEventId )
    {
        BSETTOP_ERROR(berr_external_error);
        goto error;
    }

    record->overflowEventId = b_event_register(record->overflowEvent, brecord_p_overflow_handler, record);
    if ( NULL == record->overflowEventId )
    {
        BSETTOP_ERROR(berr_external_error);
        goto error;
    }

    return record;

error:
    brecord_close(record);
    return NULL;
#else
    return NULL;
#endif
}

void brecord_close(brecord_t record)
{

#if NEXUS_HAS_RECORD
    if (record->started) {
        brecord_stop(record);
    }

    if ( NULL != record->overflowEventId )
    {
        b_event_unregister(record->overflowEventId);
    }
    if ( NULL != record->errorEventId )
    {
        b_event_unregister(record->errorEventId);
    }
    if ( NULL != record->overflowEvent )
    {
        B_Event_Destroy(record->overflowEvent);
    }
    if ( NULL != record->errorEvent )
    {
        B_Event_Destroy(record->errorEvent);
    }

    NEXUS_Record_Destroy(record->nRecord);
    BKNI_Free(record);
#else
    BSTD_UNUSED(record);
#endif
}

void brecord_params_init(brecord_params *params, brecord_t record)
{
    BSTD_UNUSED(record);
    BKNI_Memset(params, 0, sizeof(*params));
    params->index_format = bindex_format_bcm;
    bencryption_params_init(&params->encryption);
}

brecord_file_t brecord_file_open(const char *mpeg_file_name, const char *index_file_name)
{
    return brecord_p_file_open(mpeg_file_name, index_file_name, false);
}

brecord_file_t brecord_p_file_open(const char *mpeg_file_name, const char *index_file_name, bool fifo)
{
#if NEXUS_HAS_RECORD
    brecord_file_t file = BKNI_Malloc(sizeof(*file));
    BKNI_Memset(file, 0, sizeof(*file));

    if (fifo) {
        file->nFileFifo = NEXUS_FifoRecord_Create(mpeg_file_name, index_file_name);
        if (!file->nFileFifo) {
            BKNI_Free(file);
            return NULL;
        }
        file->nFile = NEXUS_FifoRecord_GetFile(file->nFileFifo);
        if (!file->nFile) {
            BKNI_Free(file);
            return NULL;
        }
    }
    else {
        file->nFile = NEXUS_FileRecord_OpenPosix(mpeg_file_name, index_file_name);
        if (!file->nFile) {
            BKNI_Free(file);
            return NULL;
        }
    }
    return file;
#else
    BSTD_UNUSED(mpeg_file_name);
    BSTD_UNUSED(index_file_name);
    BSTD_UNUSED(fifo);
    return NULL;
#endif
}

brecord_file_t
brecord_file_open_network( const char *mpeg_file_name, const char *index_file_name, const bsocket_params * params, const bstream_mpeg * mpeg_info )
{
#if B_HAS_IP
    B_PlaybackIpSocketOpenSettings socketSettings;
    B_PlaybackIpPsiInfo psi;
    B_PlaybackIpError rc;

    brecord_file_t file = BKNI_Malloc(sizeof(*file));
    BKNI_Memset(file, 0, sizeof(*file));
    BKNI_Memset(&socketSettings, 0, sizeof(socketSettings));
    snprintf(socketSettings.ipAddr,
                sizeof(socketSettings.ipAddr),
                "%d.%d.%d.%d",params->addr.ipv4.host[0],
                params->addr.ipv4.host[1],params->addr.ipv4.host[2],
                params->addr.ipv4.host[3]);
    socketSettings.port = params->addr.ipv4.port;
    socketSettings.protocol = B_PlaybackIpProtocol_eHttp;
    BDBG_WRN(("%s: Recording %s to IP Addr %s, port %d\n",
                __FUNCTION__, mpeg_file_name, socketSettings.ipAddr, socketSettings.port));
    BKNI_Memset(&psi, 0, sizeof(psi));
    psi.videoPid = mpeg_info->video[0].pid;
    psi.audioPid = mpeg_info->audio[0].pid;
    psi.pcrPid = mpeg_info->pcr_pid;
    psi.videoCodec = (NEXUS_VideoCodec)mpeg_info->video[0].format;
    psi.audioCodec = (NEXUS_AudioCodec)mpeg_info->audio[0].format;
    psi.transportTimeStampEnabled = params->timestamp_enabled;

    rc = B_PlaybackIp_NetworkRecordingStart(&socketSettings, mpeg_file_name, index_file_name, &psi, &file->nFile);
    if (rc) {
        BKNI_Free(file);
        return NULL;
    }
    BDBG_MSG(("%s: record started, brecord_file %p\n", __FUNCTION__, file));
    return file;
#else
   BSTD_UNUSED(mpeg_file_name);
   BSTD_UNUSED(index_file_name);
   BSTD_UNUSED(params);
   BSTD_UNUSED(mpeg_info);
   return NULL;
#endif

}

void brecord_file_close(brecord_file_t file)
{
#if NEXUS_HAS_RECORD
    BDBG_MSG(("%s: brecord_file %p\n", __FUNCTION__, file));
    NEXUS_FileRecord_Close(file->nFile);
    BKNI_Free(file);
#else
    BSTD_UNUSED(file);
#endif
}

#if NEXUS_HAS_RECORD
static void brecord_p_error_handler(void *context)
{
    brecord_t record = (brecord_t)context;
    if (record->params.error) {
        b_unlock();
        (*record->params.error)(record->params.callback_context, record->errorcode);
        b_lock();
    }
}
#endif

#if NEXUS_HAS_RECORD
static void brecord_p_overflow_handler(void *context)
{
    brecord_t record = (brecord_t)context;
    if (record->params.overflow) {
        bool stop = false;
        b_unlock();
        (*record->params.overflow)(record->params.callback_context, &stop);
        b_lock();
        /* stop outparam is unused */
    }
}
#endif

bresult brecord_start(brecord_t record, brecpump_t recpump, bstream_t stream, brecord_file_t file, const brecord_params *params)
{
#if NEXUS_HAS_RECORD
    NEXUS_Error rc;
    NEXUS_RecordSettings recordCfg;

    BDBG_ASSERT(NULL != params);
    record->params = *params;

    if ( stream->consumers.record )
    {
        BDBG_ERR(("Can only connect a single record to a stream"));
        return BSETTOP_ERROR(berr_invalid_parameter);
    }

    if (record->started) {
        brecord_stop(record);
    }

    NEXUS_Record_GetSettings(record->nRecord, &recordCfg);
    recordCfg.indexFormat = params->index_format == bindex_format_bcm?NEXUS_RecordIndexType_eBcm:NEXUS_RecordIndexType_eNone;
    recordCfg.recpump = recpump->nRecpump;
    recordCfg.stopOnOverflow = true;
    recordCfg.overflowCallback.callback = b_generic_callback;
    recordCfg.overflowCallback.context = record;
    recordCfg.overflowCallback.param = (int)record->overflowEvent;
    recordCfg.errorCallback.callback = b_generic_callback;
    recordCfg.errorCallback.context = record;
    recordCfg.errorCallback.param = (int)record->errorEvent;
    recordCfg.recpumpSettings.timestampType = params->timestamp_enabled ?
        NEXUS_TransportTimestampType_eBinary : NEXUS_TransportTimestampType_eNone;

    if (params->encryption.type != bencryption_type_none) {
#if NEXUS_HAS_SECURITY
        if (!params->encryption.key_ladder) {
            switch (params->encryption.type) {
            case bencryption_type_des:
                if (params->encryption.key_length != 64)
                    return BSETTOP_ERROR(berr_invalid_parameter);
                break;
            case bencryption_type_3des:
                if (params->encryption.key_length != 128)
                    return BSETTOP_ERROR(berr_invalid_parameter);
                break;
            case bencryption_type_aes:
                if (params->encryption.key_length != 128)
                    return BSETTOP_ERROR(berr_invalid_parameter);
                break;
            default:
                BDBG_ERR(("Unsupported PVR encryption algorithm"));
                return BSETTOP_ERROR(berr_invalid_parameter);
            }
        } else if (params->encryption.key_length != 8*sizeof(bcrypto_keyladder_data) || !params->encryption.long_key) {
            return BSETTOP_ERROR(berr_invalid_parameter);
        }
        recordCfg.recpumpSettings.securityContext = b_keyslot_m2m_allocate(&params->encryption,true,
            recordCfg.recpumpSettings.timestampType==NEXUS_TransportTimestampType_eNone?false:true);
        record->hKeySlot = recordCfg.recpumpSettings.securityContext;
#if NEXUS_NUM_DMA_CHANNELS
        recordCfg.recpumpSettings.securityDma = g_dma.hDma;
        BDBG_MSG(("Enabling encryption on record: slot: %p, dma: %p",recordCfg.recpumpSettings.securityContext,recordCfg.recpumpSettings.securityDma));
#else
        BDBG_MSG(("Enabling encryption on record: slot: %p",recordCfg.recpumpSettings.securityContext));
#endif
        if (!recordCfg.recpumpSettings.securityContext
#if NEXUS_NUM_DMA_CHANNELS
                || !recordCfg.recpumpSettings.securityDma
#endif
                ) {
            BDBG_ERR(("Enabling encryption on record FAILED!"));
#if NEXUS_NUM_DMA_CHANNELS
            BDBG_ERR(("slot: %p, dma: %p",recordCfg.recpumpSettings.securityContext,recordCfg.recpumpSettings.securityDma));
#else
            BDBG_ERR(("slot: %p",recordCfg.recpumpSettings.securityContext));
#endif
            return BSETTOP_ERROR(berr_external_error);
        }
#else
        BDBG_ERR(("PVR encryption is not supported"));
        return BSETTOP_ERROR(berr_invalid_parameter);
#endif
    } else {
#if NEXUS_HAS_SECURITY
        recordCfg.recpumpSettings.securityContext = NULL;
#if NEXUS_NUM_DMA_CHANNELS
        recordCfg.recpumpSettings.securityDma = NULL;
#endif
#endif
    }
    NEXUS_Record_SetSettings(record->nRecord, &recordCfg);

    record->stream = stream;

    brecord_p_mpeg_change(record, &stream->mpeg);

    rc = NEXUS_Record_Start(record->nRecord, file->nFile);
    if (rc)
    {
        record->stream = NULL;
        brecord_p_mpeg_change(record, NULL);
        return BSETTOP_ERROR(berr_external_error);
    }

    record->started = true;
    stream->consumers.record = record;

    /* We have a consumer, start playback if data is coming from playback */
    if (stream->producer.playback) {
        rc = bplayback_p_start(stream->producer.playback);
    }

#else
    BSTD_UNUSED(record);
    BSTD_UNUSED(recpump);
    BSTD_UNUSED(stream);
    BSTD_UNUSED(file);
    BSTD_UNUSED(params);
#endif
    return 0;
}

void brecord_get_status(brecord_t record, brecord_status *status)
{
#if NEXUS_HAS_RECORD
    NEXUS_RecordStatus nStatus;
    BKNI_Memset(status, 0, sizeof(*status));
    NEXUS_Record_GetStatus(record->nRecord, &nStatus);

    status->mpeg_bytes_recorded = nStatus.recpumpStatus.data.bytesRecorded;
    status->mpeg_fifo_depth = nStatus.recpumpStatus.data.fifoDepth;
    status->mpeg_fifo_size = nStatus.recpumpStatus.data.fifoSize;
    status->has_index = nStatus.recpumpStatus.hasIndex;
    status->index_bytes_recorded = nStatus.recpumpStatus.index.bytesRecorded;
    status->index_fifo_depth = nStatus.recpumpStatus.index.fifoDepth;
    status->index_fifo_size = nStatus.recpumpStatus.index.fifoSize;
    status->last_timestamp = nStatus.lastTimestamp;
#else
    BSTD_UNUSED(record);
    BSTD_UNUSED(status);
#endif
}

bresult brecord_stop(brecord_t record)
{
#if NEXUS_HAS_RECORD
    if (!record->started) {
        return 0;
    }

    if (record->playback) {
        return BSETTOP_ERROR(berr_not_supported);
    }

    NEXUS_Record_Stop(record->nRecord);
    record->started = false;

    /* Remove all pid channels */
    brecord_p_mpeg_change(record, NULL);

    if ( record->stream )
    {
        record->stream->consumers.record = NULL;
        record->stream = NULL;
    }


    if (record->hKeySlot) {
        b_keyslot_m2m_free(record->hKeySlot);
        record->hKeySlot = NULL;
    }
#else
    BSTD_UNUSED(record);
#endif
    return 0;
}

bplayback_t botfplay_open(bobject_t feeder_id)
{
    /* TODO */
    BSTD_UNUSED(feeder_id);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

void bsocket_params_init(bsocket_params *params)
{
    BSTD_UNUSED(params);
    return;
}

bstream_t brecord_socket_open(brecord_t record, const bsocket_params *params)
{
    /* TODO */
    BSTD_UNUSED(record);
    BSTD_UNUSED(params);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

void brecord_t_socket_close(brecord_t record)
{
    /* TODO */
    BSTD_UNUSED(record);
    BSETTOP_ERROR(berr_not_supported);
}

brecord_t brecord_ip_open(void)
{
    /* TODO */
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
}

#if NEXUS_HAS_RECORD
static void brecord_p_mpeg_change_allpass(brecord_t record , const bstream_mpeg *new_settings)
{
    uint16_t pid=0;
    int index=0;
    NEXUS_Error errCode;
    bstream_t stream = record->stream;
    NEXUS_RecordPidChannelSettings *pRecordPidChannnelSetting=NULL;

#if NEXUS_HAS_PLAYBACK
    if (stream->producer.playback)
    {
        NEXUS_PlaybackSettings playbackSettings;
        NEXUS_PlaypumpStatus playpumpStatus;
        NEXUS_Playback_GetSettings(stream->producer.playback->nPlayback, &playbackSettings);
        NEXUS_Playpump_GetStatus(playbackSettings.playpump,&playpumpStatus);
        if (new_settings->audio[0].pid == 0xFFFF)
        {
            pid = 0xFFFF;
            playbackSettings.playpumpSettings.acceptNullPackets=false;
        }
        else
        {
            pid = 0x1FFF;
            playbackSettings.playpumpSettings.acceptNullPackets=true;
        }
        playbackSettings.playpumpSettings.allPass=true;
        NEXUS_Playback_SetSettings(stream->producer.playback->nPlayback, &playbackSettings);
        BDBG_WRN(("Setting playback %d to allpass mode %s null packets",playpumpStatus.index,
                  playbackSettings.playpumpSettings.acceptNullPackets?"with":"without"));
    }
    else
#endif
    if (stream->producer.playpump)
    {
        NEXUS_PlaypumpSettings playpumpSettings;
        NEXUS_PlaypumpStatus playpumpStatus;
        NEXUS_Playpump_GetSettings(stream->producer.playpump->nPlaypump, &playpumpSettings);
        NEXUS_Playpump_GetStatus(stream->producer.playpump->nPlaypump,&playpumpStatus);
        if (new_settings->audio[0].pid == 0xFFFF)
        {
            pid = 0xFFFF;
            playpumpSettings.acceptNullPackets=false;
        }
        else
        {
            pid = 0x1FFF;
            playpumpSettings.acceptNullPackets=true;
        }
        playpumpSettings.allPass=true;
        errCode = NEXUS_Playpump_SetSettings(stream->producer.playpump->nPlaypump, &playpumpSettings);
		if (errCode)
		{
			errCode = BERR_TRACE(errCode);
			return;
		}

        BDBG_WRN(("Setting playpump %d to allpass mode %s null packets",playpumpStatus.index,
                  playpumpSettings.acceptNullPackets?"with":"without"));
    }
    else if (stream->producer.band)
    {
        NEXUS_ParserBandSettings parserBandSettings;
        NEXUS_ParserBand_GetSettings(stream->parser_band->nParserBand, &parserBandSettings);
        if (new_settings->audio[0].pid == 0xFFFF)
        {
            pid = 0xFFFF;
            parserBandSettings.acceptNullPackets=false;
        }
        else
        {
            pid = 0x1FFF;
            parserBandSettings.acceptNullPackets=true;
        }

        parserBandSettings.allPass=true;
        NEXUS_ParserBand_SetSettings(stream->parser_band->nParserBand, &parserBandSettings);

        BDBG_WRN(("Setting parser band %d to allpass mode %s null packets",stream->parser_band->nParserBand,
                  parserBandSettings.acceptNullPackets?"with":"without"));
    }
    else
    {
        BDBG_ERR(("Unknown producer"));
        return;
    }

    /* remove and close pid record pid channels */
    for ( index=0; index < BSETTOP_MAX_PROGRAMS; index++ )
    {
        if ( record->videoPid[index] )
        {
            NEXUS_Record_RemovePidChannel(record->nRecord, record->videoPid[index]);
            bstream_p_close_pid(record->stream, record->videoPid[index]);
            record->videoPid[index] = NULL;
        }
        if ( record->audioPid[index] )
        {
            NEXUS_Record_RemovePidChannel(record->nRecord, record->audioPid[index]);
            bstream_p_close_pid(record->stream, record->audioPid[index]);
            record->audioPid[index] = NULL;
        }
        if ( record->ancillaryPid[index] )
        {
            NEXUS_Record_RemovePidChannel(record->nRecord, record->ancillaryPid[index]);
            bstream_p_close_pid(record->stream, record->ancillaryPid[index]);
            record->ancillaryPid[index] = NULL;
        }
    }
    /* Add pid channnel to record */
    record->videoPid[0] = bstream_p_open_pid(stream,pid,bstream_pid_type_other);
    if ( NULL == record->videoPid[0] )
    {
        BDBG_WRN(("Unable to allocate record pid channel for all pass record"));
        BSETTOP_ERROR(berr_external_error);
    }

#if ALLPASS_RECORD_WITH_INDEX
    {
        NEXUS_RecordPidChannelSettings pidCfg;
        NEXUS_Record_GetDefaultPidChannelSettings(&pidCfg);
        pidCfg.recpumpSettings.pidType = NEXUS_PidType_eVideo;
        pidCfg.recpumpSettings.pidTypeSettings.video.index = true;
        pidCfg.recpumpSettings.pidTypeSettings.video.codec = b_videocodec2nexus(new_settings->video[1].format);
        /* generate index using pid number instead of pid chanel number */
        pidCfg.recpumpSettings.pidTypeSettings.video.pid = new_settings->video[1].pid;
        pRecordPidChannnelSetting= &pidCfg;
    }
#endif

    errCode = NEXUS_Record_AddPidChannel(record->nRecord, record->videoPid[0], pRecordPidChannnelSetting);
    if ( errCode )
    {
        BDBG_ERR(("Unable to add record pid channel"));
        BSETTOP_ERROR(berr_external_error);
    }
}
#endif

void brecord_p_mpeg_change(brecord_t record, const bstream_mpeg *new_settings)
{
#if NEXUS_HAS_RECORD
    int index;
    NEXUS_Error errCode;
#if 0
    bstream_mpeg empty_settings;
#endif

    BDBG_ASSERT(NULL != record);
    BDBG_ASSERT(NULL != record->stream);

    /* check to see if we all pass record is requested */
    if (new_settings)
    {
        for ( index=0; index < BSETTOP_MAX_PROGRAMS; index++ )
        {
            if (new_settings->ancillary[index].pid)
                break;
            if (new_settings->audio[index].pid && new_settings->audio[index].pid != 0xFFFF && new_settings->audio[index].pid != 0x1FFF  )
                break;
#if ALLPASS_RECORD_WITH_INDEX
            /* Ignore the video PID */
            if(index==1)
                continue;
#endif
            if (new_settings->video[index].pid)
                break;
        }
        if (index == BSETTOP_MAX_PROGRAMS)
        {
            /* all pass record is reqested */
            brecord_p_mpeg_change_allpass(record,new_settings);
            return;
        }
    }
    else
    {
        /* remove and close pid record pid channels */
        for ( index=0; index < BSETTOP_MAX_PROGRAMS; index++ )
        {
            if ( record->videoPid[index] )
            {
                NEXUS_Record_RemovePidChannel(record->nRecord, record->videoPid[index]);
                bstream_p_close_pid(record->stream, record->videoPid[index]);
                record->videoPid[index] = NULL;
            }
            if ( record->audioPid[index] )
            {
                NEXUS_Record_RemovePidChannel(record->nRecord, record->audioPid[index]);
                bstream_p_close_pid(record->stream, record->audioPid[index]);
                record->audioPid[index] = NULL;
            }
            if ( record->ancillaryPid[index] )
            {
                NEXUS_Record_RemovePidChannel(record->nRecord, record->ancillaryPid[index]);
                bstream_p_close_pid(record->stream, record->ancillaryPid[index]);
                record->ancillaryPid[index] = NULL;
            }
        }
        return;
    }

#if 0
    /* Free all PID channels with NULL settings */
    if ( NULL == new_settings )
    {
        bstream_mpeg_init(&empty_settings);
        new_settings = &empty_settings;
    }
#endif

    BDBG_WRN(("Record MPEG change"));

    for ( index=0; index < BSETTOP_MAX_PROGRAMS; index++ )
    {
        if ( (record->stream->mpeg.video[index].pid != new_settings->video[index].pid) ||
             (new_settings->video[index].pid != 0 && new_settings->video[index].pid < 0x1fff &&
              NULL == record->videoPid[index]) )
        {
            if ( record->videoPid[index] )
            {
                NEXUS_Record_RemovePidChannel(record->nRecord, record->videoPid[index]);
                bstream_p_close_pid(record->stream, record->videoPid[index]);
                record->videoPid[index] = NULL;
            }
            if ( new_settings->video[index].pid != 0 &&
                 new_settings->video[index].pid < 0x1fff )
            {
                record->videoPid[index] = bstream_p_open_pid(record->stream, new_settings->video[index].pid, bstream_pid_type_video);
                if ( NULL == record->videoPid[index] )
                {
                    BDBG_WRN(("Unable to allocate record pid channel for pid 0x%x (%d)", record->videoPid[index], record->videoPid[index]));
                    BSETTOP_ERROR(berr_external_error);
                }
                else
                {
                    bool setRapTpit = false;
                    NEXUS_RecordPidChannelSettings addPidChannelSettings;
                    NEXUS_Record_GetDefaultPidChannelSettings(&addPidChannelSettings);
                    addPidChannelSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
                    addPidChannelSettings.recpumpSettings.pidTypeSettings.video.codec = b_videocodec2nexus(new_settings->video[index].format);
                    addPidChannelSettings.recpumpSettings.pidType = NEXUS_PidType_eVideo;
                    if ( 0 == index && record->params.index_format != bindex_format_none )
                    {
                        BDBG_WRN(("Enabling video indexing (index=0)"));
                        addPidChannelSettings.recpumpSettings.pidTypeSettings.video.index = true;
                        if (addPidChannelSettings.recpumpSettings.pidTypeSettings.video.codec == NEXUS_VideoCodec_eH264)
                        {
                            setRapTpit = true;
                        }
                    }
                    else
                    {
                        BDBG_WRN(("Disabling video indexing"));
                        addPidChannelSettings.recpumpSettings.pidTypeSettings.video.index = false;
                    }
                    errCode = NEXUS_Record_AddPidChannel(record->nRecord, record->videoPid[index], &addPidChannelSettings);
                    if ( errCode )
                    {
                        BDBG_ERR(("Unable to add record pid channel"));
                        BSETTOP_ERROR(berr_external_error);
                    }

                    if (setRapTpit) {
                        NEXUS_RecpumpTpitFilter filter;
                        NEXUS_RecordSettings recordCfg;

                        NEXUS_Record_GetSettings(record->nRecord, &recordCfg);
                        NEXUS_Recpump_GetDefaultTpitFilter(&filter);
                        filter.config.mpeg.randomAccessIndicatorEnable = true;
                        filter.config.mpeg.randomAccessIndicatorCompValue = true;
                        errCode = NEXUS_Recpump_SetTpitFilter(recordCfg.recpump, record->videoPid[index], &filter);
                        if ( errCode )
                        {
                            BDBG_ERR(("Unable to set TPIT filter for H264 RAP's"));
                            BSETTOP_ERROR(berr_external_error);
                        }
                    }
                }
            }
        }
        if ( (record->stream->mpeg.audio[index].pid != new_settings->audio[index].pid) ||
             (new_settings->audio[index].pid != 0 && new_settings->audio[index].pid < 0x1fff &&
              NULL == record->audioPid[index]) )
        {
            if ( record->audioPid[index] )
            {
                NEXUS_Record_RemovePidChannel(record->nRecord, record->audioPid[index]);
                bstream_p_close_pid(record->stream, record->audioPid[index]);
                record->audioPid[index] = NULL;
            }
            if ( new_settings->audio[index].pid != 0 &&
                 new_settings->audio[index].pid < 0x1fff )
            {
                record->audioPid[index] = bstream_p_open_pid(record->stream, new_settings->audio[index].pid, bstream_pid_type_other);
                if ( NULL == record->audioPid[index] )
                {
                    BDBG_WRN(("Unable to allocate record pid channel for pid 0x%x (%d)", new_settings->audio[index].pid, new_settings->audio[index].pid));
                    BSETTOP_ERROR(berr_external_error);
                }
                else
                {
                    errCode = NEXUS_Record_AddPidChannel(record->nRecord, record->audioPid[index], NULL);
                    if ( errCode )
                    {
                        BDBG_ERR(("Unable to add record pid channel"));
                        BSETTOP_ERROR(berr_external_error);
                    }
                }
            }
        }
        if ( (record->stream->mpeg.ancillary[index].pid != new_settings->ancillary[index].pid) ||
             (new_settings->ancillary[index].pid != 0 && new_settings->ancillary[index].pid < BSETTOP_PAT_PID &&
              NULL == record->ancillaryPid[index]) )
        {
            if ( record->ancillaryPid[index] )
            {
                NEXUS_Record_RemovePidChannel(record->nRecord, record->ancillaryPid[index]);
                bstream_p_close_pid(record->stream, record->ancillaryPid[index]);
                record->ancillaryPid[index] = NULL;
            }
            if ( new_settings->ancillary[index].pid != 0 &&
                 new_settings->ancillary[index].pid < BSETTOP_PAT_PID )
            {
                uint16_t pid;
                pid = new_settings->ancillary[index].pid;
                if ( pid == BSETTOP_PAT_PID )
                {
                    pid = 0;
                }
                record->ancillaryPid[index] = bstream_p_open_pid(record->stream, pid, bstream_pid_type_other);
                if ( NULL == record->ancillaryPid[index] )
                {
                    BDBG_WRN(("Unable to allocate record pid channel for pid 0x%x (%d)", pid, pid));
                    BSETTOP_ERROR(berr_external_error);
                }
                else
                {
                    errCode = NEXUS_Record_AddPidChannel(record->nRecord, record->ancillaryPid[index], NULL);
                    if ( errCode )
                    {
                        BDBG_ERR(("Unable to add record pid channel"));
                        BSETTOP_ERROR(berr_external_error);
                    }
                }
            }
        }
    }
#else
    BSTD_UNUSED(new_settings);
    BSTD_UNUSED(record);
#endif

}

void bplayback_close_indx(bplayback_t playback)
{
#if NEXUS_HAS_PLAYBACK
    if (playback->stream) {
        bplayback_stop_indx(playback);
    }

    BDBG_ASSERT(playback->stream_alloc);
    playback->stream_alloc->static_alloc = false; /* bstream_close can now free this */
    bstream_close(playback->stream_alloc);

    if ( NULL != playback->beginning_of_stream_event_id )
    {
        b_event_unregister(playback->beginning_of_stream_event_id);
    }
    if ( NULL != playback->end_of_stream_event_id )
    {
        b_event_unregister(playback->end_of_stream_event_id);
    }
    if ( NULL != playback->error_event_id )
    {
        b_event_unregister(playback->error_event_id);
    }

    if ( NULL != playback->beginning_of_stream_event )
    {
        B_Event_Destroy(playback->beginning_of_stream_event);
    }
    if ( NULL != playback->end_of_stream_event )
    {
        B_Event_Destroy(playback->end_of_stream_event);
    }
    if ( NULL != playback->error_event )
    {
        B_Event_Destroy(playback->error_event);
    }

    NEXUS_Playback_Destroy(playback->nPlayback);
    BKNI_Free(playback);
#else
    BSTD_UNUSED(playback);
#endif
}

bplayback_t bplayback_open(void)
{
#if NEXUS_HAS_PLAYBACK
    bplayback_t playback = BKNI_Malloc(sizeof(*playback));

    BKNI_Memset(playback, 0, sizeof(*playback));
    playback->nPlayback = NEXUS_Playback_Create();

    playback->end_of_stream_event = B_Event_Create(NULL);
    if ( NULL == playback->end_of_stream_event )
    {
        BSETTOP_ERROR(berr_external_error);
        goto error;
    }
    playback->beginning_of_stream_event = B_Event_Create(NULL);
    if ( NULL == playback->beginning_of_stream_event )
    {
        BSETTOP_ERROR(berr_external_error);
        goto error;
    }
    playback->error_event = B_Event_Create(NULL);
    if ( NULL == playback->error_event )
    {
        BSETTOP_ERROR(berr_external_error);
        goto error;
    }
    playback->end_of_stream_event_id = b_event_register(playback->end_of_stream_event, bplayback_p_end_of_stream_handler, playback);
    if ( NULL == playback->end_of_stream_event_id )
    {
        BSETTOP_ERROR(berr_external_error);
        goto error;
    }
    playback->beginning_of_stream_event_id = b_event_register(playback->beginning_of_stream_event, bplayback_p_beginning_of_stream_handler, playback);
    if ( NULL == playback->beginning_of_stream_event_id )
    {
        BSETTOP_ERROR(berr_external_error);
        goto error;
    }
    playback->error_event_id = b_event_register(playback->error_event, bplayback_p_error_handler, playback);
    if ( NULL == playback->error_event_id )
    {
        BSETTOP_ERROR(berr_external_error);
        goto error;
    }

    /* initialize interface */
    bplayback_iface_init(&playback->iface);
    playback->iface.params_init = bplayback_params_init_indx;
    playback->iface.start = bplayback_start_indx;
    playback->iface.start_timeshifting = bplayback_start_timeshifting_indx;
    playback->iface.stop_timeshifting = bplayback_stop_timeshifting_indx;
    playback->iface.stop = bplayback_stop_indx;
    playback->iface.get_status = bplayback_get_status_indx;
    playback->iface.pause = bplayback_pause_indx;
    playback->iface.catchup_record = bplayback_catchup_record_indx;
    playback->iface.play = bplayback_play_indx;
    playback->iface.frameadvance = bplayback_frameadvance_indx;
    playback->iface.trickmode = bplayback_trickmode_indx;
    playback->iface.get_trickmode_by_rate = bplayback_get_trickmode_by_rate_indx;
    playback->iface.goto_index = bplayback_goto_index_indx;
    playback->iface.goto_timestamp = bplayback_goto_timestamp_indx;
    playback->iface.goto_mpeg_file_offset = bplayback_goto_mpeg_file_offset_indx;
    playback->iface.close = bplayback_close_indx;

    playback->stream_alloc = bstream_p_open(NULL, NULL, playback, 0, NULL);
    if ( NULL == playback->stream_alloc )
    {
        BSETTOP_ERROR(berr_external_error);
        goto error;
    }
    playback->stream_alloc->static_alloc = true; /* bstream_close will not free this */

    return playback;

error:
    BKNI_Free(playback);
    return NULL;
#else
    return NULL;
#endif
}

