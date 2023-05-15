/***************************************************************************
 *     Copyright (c) 2003-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_transcode.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 7/23/08 3:19p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_transcode.h $
 * 
 * 2   7/23/08 3:19p jgarrett
 * PR43426: Fixing erroneous comment
 * 
 * 1   6/25/08 12:01p jgarrett
 * PR43426: PR43426 : Changes as per review comments
 * PR43426 : Further modification for dynamic controls and debug
 * PR43426 : Add transcode support to Settopapi
 * 
 * PR43426/4   6/23/08 2:12p shyam
 * PR43426 : Changes as per review comments
 * 
 * PR43426/3   6/18/08 5:03p shyam
 * PR43426 : Changes as per review comments
 * 
 * PR43426/2   6/12/08 7:14p shyam
 * PR43426 : Further modification for dynamic controls and debug
 * 
 * PR43426/1   6/6/08 7:13p shyam
 * PR43426 : Add transcode support to Settopapi
 *
 ***************************************************************************/
#ifndef BSETTOP_TRANSCODE_H__
#define BSETTOP_TRANSCODE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define BTRANSCODE_MAX_VIDEO_PROGRAM		6
#define BTRANSCODE_MAX_AUDIO_PROGRAM		6
#define BTRANSCODE_MAX_DATA_PROGRAM		6

typedef struct btranscode_program {

  struct {
  	uint16_t pid;                 /* Pid 0 means unused slot */
  	bvideo_codec format;
	unsigned width; /* width in pixels of the encoded MPEG stream  */
	unsigned height; /* height in pixels of the encoded MPEG stream  */
	unsigned bitrate; /* bitrate (bits per second) of the encoded MPEG video stream */
	bvideo_frame_rate framerate; /* framerate */
	bencode_gop_structure gop_structure; /* GOP structure */
	bool progressive ; /* Progressive or interlaced */

  } video[BTRANSCODE_MAX_VIDEO_PROGRAM];	/* video stream pids */
  struct {
  	uint16_t pid;
	baudio_format format; /* format of the digital audio in the encoded stream */
	bool passthrough ; /* Enable Pass Through of incoming stream */
	unsigned samplerate; /* sample rate (samples per second) of the encoded audio stream */
	unsigned bitrate; /* bitrate (bits per second) of the encoded audio stream */

  } audio[BTRANSCODE_MAX_AUDIO_PROGRAM];	/* audio stream pids */
  struct {
	  uint16_t pid;
  } ancillary[BTRANSCODE_MAX_DATA_PROGRAM];	/* data channels */
  uint16_t pcr_pid;
} btranscode_program;

/*
Summary:
Data structure used to initialize the transcoder.
*/
typedef struct btranscode_settings {
	bool realtime ; /* Enables or disables realtime */
	btranscode_program program ;
	bstream_mpeg_type stream_type;
	unsigned transport_bitrate ;
} btranscode_settings;


/**
Summary:
	The status data structure.
**/
typedef struct btranscode_status {
	bool is_locked ;			/* True if encoder is locked */
	unsigned frame_count;		/* Encoded frame count */
} btranscode_status;


/*
Summary:
    Open a transcode engine
Description:
*/
btranscode_t btranscode_open(
    bobject_t transcode_id   /* transcode object id */
   );

/**
Summary:
	Initialize transcoder settings for a particular quality level.
**/
void btranscode_settings_init(
    btranscode_settings *settings, /* [out] settings to be initialized */
    bencode_quality quality, /* quality level to use when chosing initial settings */
    bencode_vpp_mode vpp_mode       /* vpp settings to set to*/
	);

/*
Summary:
    Close a transcode
*/
void btranscode_close(
    btranscode_t transcode /* handle returned by btranscode_open */
    );

/*
Summary:
    Start transcoding a stream.
*/
bstream_t btranscode_start(
    btranscode_t transcode, /* handle returned by btranscode_open */
    bstream_t source,   /* source for the transcode, either analog or digital */
    const btranscode_settings *settings /* settings to be initialized */
    );

/*
Summary:
    Stop transcoding a stream.
Description:
    The stream remains valid after transcode is stopped. 
*/
void btranscode_stop(
    btranscode_t transcode /* handle returned by btranscode_open */
    );


/*
Summary:
    Pause a stream transcode.
*/
bresult btranscode_pause(
    btranscode_t transcode /* handle returned by btranscode_open */
    );


/*
Summary:
    Resume a stream transcode.
*/
bresult btranscode_resume(
    btranscode_t transcode /* handle returned by btranscode_open */
    );


/*
Summary:
    Get Settings for a stream transcode
Description:
    Get aand Set settings will take care of dyanmic parameter changes 
*/
void btranscode_get(
    btranscode_t transcode, /* handle returned by btranscode_open */
    btranscode_settings *settings
    ) ;


/*
Summary:
    Set Settings for a stream transcode
Description:
    Sets the transcoder settings 
*/
bresult btranscode_set(
    btranscode_t transcode, /* handle returned by btranscode_open */
    const btranscode_settings *settings
    ) ;

/*
Summary:
    Get Settings for a stream transcode
Description:
    Get aand Set settings will take care of dyanmic parameter changes 
*/
bresult btranscode_get_status(
    btranscode_t transcode, /* handle returned by btranscode_open */
    btranscode_status *status
    ) ;

#ifdef __cplusplus
}
#endif


#endif /* BSETTOP_TRANSCODE_H__ */

