/***************************************************************************
 *     Copyright (c) 2003-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_pcm.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 11/19/09 8:02p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_pcm.h $
 * 
 * 4   11/19/09 8:02p jgarrett
 * SW7405-3357: Adding audio output routing for decode and pcm playback
 * prior to start
 * 
 * 3   8/14/08 11:46a jgarrett
 * PR32047: PR32047: Example using PCM capture in Brutus.
 * PR32047: Initial PCM capture API implementation, ignores display.
 * 
 * PR32047/2   5/19/08 7:24p mward
 * PR32047: Example using PCM capture in Brutus.
 * 
 * PR32047/1   5/16/08 3:31p mward
 * PR32047: Initial PCM capture API implementation, ignores display.
 * 
 * 2   10/16/06 11:59a vsilyaev
 * PR 24957: Addded FIFO information into the PCM status structure. Added
 * detection and handling for the underflow of PCM FIFO
 * 
 * 1   2/7/05 7:05p dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/12   7/6/04 3:54p erickson
 * PR11771: settop api dataflow redesign
 * 
 * Irvine_BSEAVSW_Devel/11   6/29/04 11:28a erickson
 * PR11135: updated deprecation notices for settop api
 * 
 * Irvine_BSEAVSW_Devel/10   11/7/03 4:28p erickson
 * PR8583: removing, fixing or implementing TODO's
 * 
 * Irvine_BSEAVSW_Devel/9   11/5/03 11:22a erickson
 * documentation changes
 * 
 * Irvine_BSEAVSW_Devel/8   10/28/03 11:28a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/7   10/10/03 5:33p erickson
 * converted all streaming api's to use size_t instead of unsigned
 * 
 * Irvine_BSEAVSW_Devel/6   9/25/03 3:26p erickson
 * added get_status to wait for file to complete
 * 
 * Irvine_BSEAVSW_Devel/5   9/25/03 11:22a erickson
 * renamed init_settings functions
 * 
 * Irvine_BSEAVSW_Devel/4   9/24/03 11:51a erickson
 * initial pcm play implementation
 * 
 * Irvine_BSEAVSW_Devel/3   9/24/03 11:06a erickson
 * pcm work
 * 
 * Irvine_BSEAVSW_Devel/2   9/22/03 11:29a erickson
 * updated api
 *
 * Irvine_BSEAVSW_Devel/1   9/19/03 5:29p erickson
 * new api
 *
 ***************************************************************************/
#ifndef BSETTOP_PCM_H__
#define BSETTOP_PCM_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*=***********************************
The PCM interface is used to write PCM data to a PCM playback
channel.

PCM playback can be used for sound effects or music (often with a
CPU-based decoder for formats like MP3).

The API is the minimal needed for streaming data. The BSEAV team also
provides a file-based API located in BSEAV/lib/pcmfile. The file-based
API includes WAV file support.
**************************************/

/*
Summary:
	PCM playback handle returned by bpcm_play_open.
Description:
	The implementation of the handle is private.
*/
typedef struct bpcm_play *bpcm_play_t;

/*
Summary:
	PCM audio settings which are used in bpcm_play_settings and bpcm_record_settings.
Description:
	The total bitrate of the PCM stream is:
	bits_per_second = bits_per_sample * sample_rate * channels.
*/
typedef struct bpcm_settings {
	unsigned bits_per_sample;	/* Either 8 or 16 */
	unsigned sample_rate;	/* Samples per channel per second. Either 32000, 44100, 48000. */
	unsigned channels;		/* Either 1 (mono) or 2 (stereo) */
} bpcm_settings;

/*
Summary:
	PCM play settings passed to bpcm_play_start.
Description:
	Even though there is no more data than just bpcm_settings, we've added this
	to support future API extensions.
*/
typedef struct bpcm_play_settings {
	bpcm_settings pcm;	/* PCM format */
	bsettop_callback callback; /* This callback notifies the user that there is
		space available in the pcm playback buffer. The user may call bpcm_play_get_buffer
		but should not call bpcm_play_write_complete. After receiving a callback,
		no additional callback will be received until bpcm_play_write_complete
		is called. */
	void *callback_context; /* User defined context which is passed into the
		above callback function. */
} bpcm_play_settings;

/*
Summary:
	Required to initialize the bpcm_settings structure.
*/
void bpcm_play_settings_init(
	bpcm_play_settings *settings,	/* [out] */
	bpcm_play_t pcmplay		/* required for possible resource-dependent defaults */
	);

/*
Summary:
	Open a PCM playback channel.
*/
bpcm_play_t bpcm_play_open(
	bobject_t id				/* id corresponds to the pcm playback channel. */
	);

/*
Summary:
	Close a PCM playback channel.
Description:
	Playback must already be stopped.
*/
void bpcm_play_close(
	bpcm_play_t pcmplay
	);

/*
Summary:
	Start playing PCM audio.
*/
bresult bpcm_play_start(
	bpcm_play_t pcmplay,
	bdisplay_t display,					/* which output to play to. */
	const bpcm_play_settings *settings
	);

/*
Summary:
	Status information returned by bpcm_play_get_status
*/
typedef struct bpcm_play_status {
	bool started;	/* Is the pcm playback engine currently played? */
	unsigned queued_bytes;	/* Number of bytes waiting to be played. */
	unsigned long fifo_size;			/* Size in bytes of the pcm buffer */
	void *buffer_base;		/* Pointer to the base of the pcm buffer.
				 	   This can be used for calculating your exact position
					   in the buffer for alignment considerations. */
} bpcm_play_status;

/*
Summary:
	Get status information.
*/
bresult bpcm_play_get_status(
	bpcm_play_t pcmplay,
	bpcm_play_status *status /* [out] */
	);

/**
Summary:
Get space available in the pcm playback buffer.

Description:
This is a non-destructive call. You can call it as many times as you want.
After writing data into the buffer, you should call bpcm_play_write_complete
to report how much of the buffer was used.
**/
bresult bpcm_play_get_buffer(
	bpcm_play_t pcmplay,
	void **data,
	size_t *length
	);

/**
Summary:
**/
bresult bpcm_play_write_complete(
	bpcm_play_t pcmplay,
	size_t amount_written
	);

/*
Summary:
Stop PCM playback
*/
bresult bpcm_play_stop(
	bpcm_play_t pcmplay
	);

/*
Summary:
PCM Playback Output Settings  
*/
typedef struct bpcm_play_output_settings
{
    bdisplay_t display;           /* The display that will receive audio from this bpcm_play_t.  On some chips, audio input/output
                                     mapping cannot be changed while audio is started, setting this variable prior to start will
                                     associate the audio decoder with the proper display.  Optional, pass NULL to use the display from
                                     bpcm_play_start() instead. */
} bpcm_play_output_settings;

/*
Summary:
Get PCM Playback Output Settings 
*/
void bpcm_play_get_output_settings(
    bpcm_play_t pcmplay,
    bpcm_play_output_settings *settings /* [out] */
    );

/*
Summary:
Set PCM Playback Output Settings 
*/
bresult bpcm_play_set_output_settings(
    bpcm_play_t pcmplay,
    const bpcm_play_output_settings *settings
    );


typedef struct bpcm_capture *bpcm_capture_t;

typedef struct bpcm_capture_settings {
        bpcm_settings pcm;	/* PCM format */
        bsettop_callback callback; /* This callback notifies the user that there is
        data available in the capture buffer. The user may call bpcm_capture_get_buffer
        but should not call bpcm_capture_read_complete. After receiving a callback,
        no additional callback will be received until bpcm_capture_read_complete
        is called. */
        void *callback_context; /* User defined context which is passed into the
        above callback function. */
} bpcm_capture_settings;

/*
Summary:
Required to initialize the bpcm_capture_settings structure.
*/
void bpcm_capture_settings_init(
    bpcm_capture_settings *settings,	/* [out] */
    bpcm_capture_t pcmcapture		/* required for possible resource-dependent defaults */
);

/*
Summary:
Open a PCM capture channel.
*/
bpcm_capture_t bpcm_capture_open(
    bobject_t id				/* id corresponds to the pcm capture channel. */
);

/*
Summary:
Close a PCM capture channel.
Description:
Capture must already be stopped.
*/
void bpcm_capture_close(
    bpcm_capture_t pcmcapture
);

/*
Summary:
Start capturing PCM audio.
*/
bresult bpcm_capture_start(
    bpcm_capture_t pcmcapture,
    bdisplay_t display,					/* which output to capture from. */
    const bpcm_capture_settings *settings
);

/**
Summary:
Get data available in the pcm capture buffer.

Description:
This is a non-destructive call. You can call it as many times as you want.
After reading data from the buffer, you should call bpcm_capture_read_complete
to report how much of the buffer was consumed.
**/
bresult bpcm_capture_get_buffer(
    bpcm_capture_t pcmcapture,
    void **data,
    size_t *length
);

/**
Summary:
**/
bresult bpcm_capture_read_complete(
    bpcm_capture_t pcmcapture,
    size_t amount_read
);

/*
Summary:
Stop PCM capture
*/
bresult bpcm_capture_stop(
    bpcm_capture_t pcmcapture
);

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_PCM_H__ */

