/***************************************************************************
 * Copyright (c) 2003-2010, Broadcom Corporation
 * All Rights Reserved
 * Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_pcm_playback.c $
 * $brcm_Revision: 9 $
 * $brcm_Date: 8/10/10 4:21p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_pcm_playback.c $
 * 
 * 9   8/10/10 4:21p erickson
 * SW7420-934: rename function
 * 
 * 8   12/3/09 4:06p jgarrett
 * SW7405-3452: Setting default playback start threshold to 0
 * 
 * 7   11/19/09 8:02p jgarrett
 * SW7405-3357: Adding audio output routing for decode and pcm playback
 * prior to start
 * 
 * 6   4/3/08 7:26p jgarrett
 * PR 40017: Fixing race between decode and pcm
 * 
 * 5   4/3/08 5:41p jgarrett
 * PR 41312: Setting callback events
 * 
 * 4   3/12/08 3:17p jgarrett
 * PR 40017: Adding PCM support
 * 
 * 3   12/20/07 10:28a erickson
 * PR37590: fix warnings
 * 
 * 2   10/16/07 12:35p erickson
 * PR36068: brutus up over settop api/nexus
 *
 ***************************************************************************/
#include "bsettop_impl.h"
#include "nexus_audio_playback.h"
#include "nexus_audio_input.h"
#include "nexus_audio_output.h"

/* This is hardcoded in bconfig right now */
#define B_N_PCM_PLAYS 2

BDBG_OBJECT_ID(bpcm_play);

BDBG_MODULE(pcm);

static bpcm_play g_pcmplays[B_N_PCM_PLAYS];
static void bpcm_play_p_callback(void *pParam1, int param2);
static void bpcm_play_p_callback_handler(void *context);

void bpcm_play_settings_init(bpcm_play_settings *settings, bpcm_play_t pcmplay)
{
    BDBG_ASSERT(NULL != settings);
    BSTD_UNUSED(pcmplay);
    BKNI_Memset(settings, 0, sizeof(*settings));
    settings->pcm.bits_per_sample=16;
    settings->pcm.channels = 2;
    settings->pcm.sample_rate = 44100;  /* This was the settop API default, brutus relies on that */
}

bpcm_play_t bpcm_play_open(bobject_t id)
{
    bpcm_play_t pcmplay;
    unsigned index = B_ID_GET_INDEX(id);

    if ( index >= B_N_PCM_PLAYS )
    {
        BSETTOP_ERROR(berr_invalid_parameter);
        return NULL;
    }

    pcmplay = &g_pcmplays[index];
    BDBG_MSG(("Opening PCM play %d (first=%d)", index));

    if ( pcmplay->opened )
    {
        BDBG_ERR(("PCM Playback %d (%p) already open (opened=%d)", index, pcmplay, pcmplay->opened));
        return NULL;
    }

    BDBG_OBJECT_SET(pcmplay, bpcm_play);

    pcmplay->nPlayback = NEXUS_AudioPlayback_Open(index, NULL);
    if ( NULL == pcmplay->nPlayback )
    {
        BDBG_ERR(("Unable to open nexus audio playback channel"));
        BDBG_OBJECT_UNSET(pcmplay, bpcm_play);
        return NULL;
    }
    pcmplay->event = B_Event_Create(NULL);
    if ( NULL == pcmplay->event )
    {
        BSETTOP_ERROR(berr_external_error);
        NEXUS_AudioPlayback_Close(pcmplay->nPlayback);
        BDBG_OBJECT_UNSET(pcmplay, bpcm_play);
        return NULL;
    }
    pcmplay->eventId = b_event_register(pcmplay->event, bpcm_play_p_callback_handler, pcmplay);
    if ( NULL == pcmplay->eventId )
    {
        BSETTOP_ERROR(berr_external_error);
        B_Event_Destroy(pcmplay->event);
        NEXUS_AudioPlayback_Close(pcmplay->nPlayback);
        BDBG_OBJECT_UNSET(pcmplay, bpcm_play);
        return NULL;
    }
    pcmplay->display = NULL;
    pcmplay->opened = true;
    pcmplay->disabled = false;

    return pcmplay;
}

void bpcm_play_close(bpcm_play_t pcmplay)
{
    BDBG_OBJECT_ASSERT(pcmplay, bpcm_play);
    BDBG_ASSERT(pcmplay->opened);

    b_event_unregister(pcmplay->eventId);
    B_Event_Destroy(pcmplay->event);

    if ( pcmplay->display )
    {
        BDBG_WRN(("Forcing stop on pcm playback channel %p", pcmplay));
        bpcm_play_stop(pcmplay);
    }

    NEXUS_AudioPlayback_Close(pcmplay->nPlayback);
    pcmplay->opened = false;
    BDBG_OBJECT_UNSET(pcmplay, bpcm_play);
}

bresult bpcm_play_start(bpcm_play_t pcmplay, bdisplay_t display, const bpcm_play_settings *settings)
{
    NEXUS_Error errCode;

    BDBG_OBJECT_ASSERT(pcmplay, bpcm_play);
    BDBG_ASSERT(pcmplay->opened);
    BDBG_ASSERT(NULL != display);
    BDBG_ASSERT(NULL != settings);

    BDBG_ASSERT(false == pcmplay->disabled);

    BDBG_MSG(("play start"));

    if ( pcmplay->output_settings.display )
    {
        if ( display != pcmplay->output_settings.display )
        {
            BDBG_ERR(("PCM Playback %p is bound to display %p.  Cannot start on display %p", pcmplay, pcmplay->output_settings.display, display));
            return BSETTOP_ERROR(berr_invalid_parameter);
        }
    }

    if ( pcmplay->display )
    {
        BDBG_WRN(("Stopping existing playback"));
        bpcm_play_stop(pcmplay);
    }

    NEXUS_AudioPlayback_GetDefaultStartSettings(&pcmplay->nSettings);
    pcmplay->nSettings.bitsPerSample = settings->pcm.bits_per_sample;
    pcmplay->nSettings.sampleRate = settings->pcm.sample_rate;
    pcmplay->nSettings.signedData = true;
    pcmplay->nSettings.stereo = (settings->pcm.channels == 1)?false:true;
    pcmplay->nSettings.startThreshold = 0;  /* Start immediately, don't wait for data */
    pcmplay->callback = settings->callback;
    pcmplay->context = settings->callback_context;
    if ( pcmplay->callback )
    {
        pcmplay->nSettings.dataCallback.callback = bpcm_play_p_callback;
        pcmplay->nSettings.dataCallback.context = pcmplay;
    }

    if ( NULL == pcmplay->output_settings.display )
    {
        /* Hook up to mixer */
        bdisplay_p_enable_audio(display, false);
        errCode = NEXUS_AudioMixer_AddInput(display->nAudioMixer,
                                            NEXUS_AudioPlayback_GetConnector(pcmplay->nPlayback));
        bdisplay_p_enable_audio(display, true);
        if ( errCode )
        {
            return BSETTOP_ERROR(berr_external_error);
        }
    }

    /* Start Playback */
    BDBG_MSG(("Starting Nexus Playback"));
    errCode = NEXUS_AudioPlayback_Start(pcmplay->nPlayback, &pcmplay->nSettings);
    if ( errCode )
    {
        bdisplay_p_enable_audio(display, false);
        NEXUS_AudioMixer_RemoveInput(display->nAudioMixer,
                                     NEXUS_AudioPlayback_GetConnector(pcmplay->nPlayback));
        bdisplay_p_enable_audio(display, true);
        return BSETTOP_ERROR(berr_external_error);
    }
    display->mixerInputsStarted++;

    /* Bind to display */
    pcmplay->display = display;

    /* Success */
    BDBG_MSG(("Playback Started"));
    return b_ok;
}

bresult bpcm_play_get_status(bpcm_play_t pcmplay, bpcm_play_status *status)
{
    NEXUS_AudioPlaybackStatus nStatus;

    BDBG_OBJECT_ASSERT(pcmplay, bpcm_play);
    BDBG_ASSERT(pcmplay->opened);

    BKNI_Memset(status, 0, sizeof(*status));
    NEXUS_AudioPlayback_GetStatus(pcmplay->nPlayback, &nStatus);
    status->started = (NULL == pcmplay->display)?false:true;
    status->fifo_size = nStatus.fifoSize;
    if ( status->started )
    {
        status->queued_bytes = nStatus.queuedBytes;
        status->buffer_base = NULL;                     /* N/A */
    }

    return b_ok;
}

bresult bpcm_play_get_buffer(bpcm_play_t pcmplay, void **data, size_t *length)
{
    BDBG_OBJECT_ASSERT(pcmplay, bpcm_play);
    BDBG_ASSERT(pcmplay->opened);
    BDBG_ASSERT(NULL != data);
    BDBG_ASSERT(NULL != length);

    /*BDBG_MSG(("Get Buffer"));*/

    if ( NULL == pcmplay->display )
    {
        BDBG_WRN(("PCM Playback %p not started", pcmplay));
        return BSETTOP_ERROR(berr_invalid_parameter);
    }

    if ( NEXUS_AudioPlayback_GetBuffer(pcmplay->nPlayback, data, length) )
    {
        return BSETTOP_ERROR(berr_external_error);
    }

    return b_ok;
}

bresult bpcm_play_write_complete(bpcm_play_t pcmplay, size_t amount_written)
{
    BDBG_OBJECT_ASSERT(pcmplay, bpcm_play);
    BDBG_ASSERT(pcmplay->opened);

    /*BDBG_MSG(("Write Complete"));*/

    if ( NULL == pcmplay->display )
    {
        BDBG_WRN(("PCM Playback %p not started", pcmplay));
        return BSETTOP_ERROR(berr_invalid_parameter);
    }

    if ( NEXUS_AudioPlayback_WriteComplete(pcmplay->nPlayback, amount_written) )
    {
        return BSETTOP_ERROR(berr_external_error);
    }

    return b_ok;
}

bresult bpcm_play_stop(bpcm_play_t pcmplay)
{
    bdisplay_t display;
    BDBG_OBJECT_ASSERT(pcmplay, bpcm_play);
    BDBG_ASSERT(pcmplay->opened);

    BDBG_ASSERT(false == pcmplay->disabled);

    BDBG_MSG(("play stop"));

    display = pcmplay->display;
    if ( NULL == display )
    {
        BDBG_MSG(("Stopping an un-started playback"));
        return b_ok;
    }

    pcmplay->display = NULL;    /* Must do this before disabling audio */
    NEXUS_AudioPlayback_Stop(pcmplay->nPlayback);
    display->mixerInputsStarted--;
    if ( NULL == pcmplay->output_settings.display )
    {
        bdisplay_p_enable_audio(display, false);
        NEXUS_AudioMixer_RemoveInput(display->nAudioMixer,
                                     NEXUS_AudioPlayback_GetConnector(pcmplay->nPlayback));
        bdisplay_p_enable_audio(display, true);
    }

    return b_ok;
}

void bpcm_play_p_enable(bdisplay_t display, bool enabled)
{
    int i;

    B_LOCK_ASSERT();
    BDBG_MSG(("%sabling pcm", enabled?"en":"dis"));

    for ( i = 0; i < B_N_PCM_PLAYS; i++ )
    {
        bpcm_play_t pcmplay = &g_pcmplays[i];
        BDBG_MSG(("pcmplay %p open %d display %p check %p", pcmplay, pcmplay->opened, pcmplay->display, display));
        if ( pcmplay->opened && pcmplay->display == display )
        {
            if ( enabled )
            {
                pcmplay->disabled = false;
                NEXUS_AudioPlayback_Start(pcmplay->nPlayback, &pcmplay->nSettings);
                B_Event_Set(pcmplay->event);
            }
            else
            {
                pcmplay->disabled = true;
                BDBG_MSG(("Forcing stop of PCM playback channel %p", pcmplay));
                NEXUS_AudioPlayback_Stop(pcmplay->nPlayback);
            }
        }
    }
}

static void bpcm_play_p_callback(void *pParam1, int param2)
{
    bpcm_play_t pcmplay = pParam1;
    BDBG_OBJECT_ASSERT(pcmplay, bpcm_play);
    BSTD_UNUSED(param2);
    B_Event_Set(pcmplay->event);
}

static void bpcm_play_p_callback_handler(void *context)
{
    bpcm_play_t pcmplay = context;
    BDBG_OBJECT_ASSERT(pcmplay, bpcm_play);

    if ( pcmplay->callback )
    {
        b_unlock();
        pcmplay->callback(pcmplay->context);
        b_lock();
    }
}

void bpcm_play_get_output_settings(
    bpcm_play_t pcmplay,
    bpcm_play_output_settings *settings /* [out] */
    )
{
    BDBG_OBJECT_ASSERT(pcmplay, bpcm_play);
    BDBG_ASSERT(NULL != settings);
    *settings = pcmplay->output_settings;
}

bresult bpcm_play_set_output_settings(
    bpcm_play_t pcmplay,
    const bpcm_play_output_settings *settings
    )
{
    BDBG_OBJECT_ASSERT(pcmplay, bpcm_play);
    BDBG_ASSERT(NULL != settings);
    /* See if we're running */
    if ( pcmplay->display )
    {
        return BSETTOP_ERROR(berr_busy);
    }
    /* If we're already attached to a display, make sure nothing else is using it. */
    if ( pcmplay->output_settings.display )
    {
        if ( pcmplay->output_settings.display->mixerInputsStarted > 0 )
        {
            return BSETTOP_ERROR(berr_busy);
        }
        /* Remove ourself from the current display */
        if ( NEXUS_AudioMixer_RemoveInput(pcmplay->output_settings.display->nAudioMixer, 
                                          NEXUS_AudioPlayback_GetConnector(pcmplay->nPlayback)) )
        {
            return BSETTOP_ERROR(berr_external_error);
        }
        pcmplay->output_settings.display = NULL;
    }
    if ( settings->display )
    {
        if ( settings->display->mixerInputsStarted > 0 )
        {
            return BSETTOP_ERROR(berr_busy);
        }
        /* Add to the new display */
        if ( NEXUS_AudioMixer_AddInput(settings->display->nAudioMixer,
                                       NEXUS_AudioPlayback_GetConnector(pcmplay->nPlayback)) )
        {
            return BSETTOP_ERROR(berr_external_error);
        }
    }
    /* Successful, save settings */
    pcmplay->output_settings = *settings;
    return b_ok;
}

