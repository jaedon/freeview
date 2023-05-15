/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_player.h $
 * $brcm_Revision: 53 $
 * $brcm_Date: 9/4/12 4:45p $
 *
 * Module Description:
 *
 * BMedia library, player interface
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_player.h $
 * 
 * 53   9/4/12 4:45p erickson
 * SW7425-3594: adaptive rate trick modes requests simulated_tsm for
 * precision rate
 * 
 * 52   8/23/12 4:34p vsilyaev
 * SW7346-997: Limit number of itterations in the retro-active seek
 * 
 * 51   12/7/11 4:00p vsilyaev
 * SW7425-1917: Added player for RMFF content
 * 
 * 50   4/19/11 5:28p vsilyaev
 * SW7422-14: Differentiate extra audio/video and 'other' tracks
 * 
 * 49   3/23/11 3:00p vsilyaev
 * SW7422-14: Added facility to associate  stream id(pid) with a chunk of
 * payload
 * 
 * 48   3/10/11 11:44a vsilyaev
 * SW7422-14: Increased limit on the frame/block size and print  warning
 * when limit is applied
 * 
 * 47   3/2/11 1:07p vsilyaev
 * SWBLURAY-24399: Updated logic to handle custom step size
 * 
 * 46   3/2/11 12:36p vsilyaev
 * SWBLURAY-24399: Added option to control step size in the TS player.
 * 
 * fix_bdvd_v3.0_SWBLURAY-24399/1   2/18/11 1:40p ialauder
 * Allow the index seek step to be configurable.
 * 
 * 45   9/28/10 1:56p erickson
 * SWDEPRECATED-1003: add
 * bmedia_player_decoder_mode.force_source_frame_rate
 *
 * 44   9/23/10 3:48p vsilyaev
 * SWBLURAY-22622: Renamed chunk_size to the key_frame_distance
 *
 * 43   9/23/10 3:02p vsilyaev
 * SWBLURAY-22622: Allow user to specify block size used by some players.
 *
 * 42   9/9/10 1:03p erickson
 * SW3548-3062: add bmedia_player_config.data_file_size so that player can
 * truncate index if it extends past the data file
 *
 * 41   9/2/10 4:25p erickson
 * SWDEPRECATED-1003: replace lookup tables with
 * bmedia_player_host_trick_mode_time_skip
 *
 * 40   8/19/10 5:22p vsilyaev
 * SWBLURAY-21863: Added option to divide large video frame into multiple
 * PES packets
 *
 * 39   7/19/10 4:03p gmohile
 * SW3548-2994 : Add callback to decrypt DRM tracks
 *
 * 38   3/12/10 6:12p vsilyaev
 * SW3556-1077: Added code to auto-select between different players
 *
 * 37   2/18/10 12:05p vsilyaev
 * SW3556-913: Differentiate between index error and end of stream
 *
 * 36   10/1/09 12:07p vsilyaev
 * SW7405-3121: Merge changes for handling not interleaved AVI streams
 *
 * fix_bdvd_v1.5_pr15387/1   9/30/09 3:31p gooslin
 * Add support for indicating to the AVI player which streams should be
 * dropped
 *
 * 35   9/10/09 12:15p vsilyaev
 * SW7335-529: Use bmedia_types for container, video and audio types
 *
 * 34   7/8/09 3:24p vsilyaev
 * PR 55989: Used size of the video decoder CDB to control size of the
 * data chunk
 *
 * 33   7/8/09 11:52a vsilyaev
 * PR 55989: Added modes to support OTF and fragmented trickmodes
 *
 * 32   6/23/09 5:51p vsilyaev
 * PR 55845: Added content type for the media player entry, this fixes key
 * frame extraction for AVI 2.0 (OpenDML) streams
 *
 * 31   6/19/09 4:56p vsilyaev
 * PR 56169: Allow fast decode trickmodes even without STC trickmodes
 *
 * 30   6/19/09 3:47p vsilyaev
 * PR 55989: Added decode_mode  paramtewr to to the set_direction method
 *
 * 29   6/3/09 10:10a vsilyaev
 * PR 55417: Added support for read method returning no data or completing
 * with a partial read
 *
 * 28   3/27/09 12:51p vsilyaev
 * PR 50311: Return readPosition in the Nexus playback status. In
 * combination with the position it could be used to measure depth of
 * decoder and playback buffers in units of time.
 *
 * 27   3/2/09 10:26a vsilyaev
 * PR 52434: Merge XViD and VC1 AP support in MKV
 *
 * fix_bdvd_v1.42_pr12986/1   2/18/09 11:51a gooslin
 * Add a player entry type for an empty operation
 *
 * 26   2/9/09 2:37p gmohile
 * PR 51813 : Truncate large entries due to corrupted index
 *
 * 25   2/3/09 3:03p vsilyaev
 * PR 51822: Added upper bound on number of bytes read when opening AVI
 * file
 *
 * 24   2/2/09 2:53p gmohile
 * PR 47815 : Add manual control of trickmodes
 *
 * 23   1/20/09 5:25p vsilyaev
 * PR 51351: Added code for on-the-fly indexing of MP3 streams
 *
 * 22   12/19/08 3:56p vsilyaev
 * PR 50214: Allowed set_direction function to fail and added framework
 * for players to communicate about an errors
 *
 * 21   12/10/08 12:49p vsilyaev
 * PR 49718: Fixed error reporting/handling from MKV player
 *
 * 20   12/10/08 1:38a vsilyaev
 * PR 48760: Added ES player for accurate progress monitor and navigation
 *
 * 19   12/8/08 11:20a erickson
 * PR49930: add bmedia_player_lookup_pts
 *
 * 18   10/16/08 6:32p vsilyaev
 * PR 47995: Added control over ID's used for media->PES conversion
 *
 * 17   10/7/08 5:21p vsilyaev
 * PR 47122: PR 47570: Added code to properly handle decoder fast forward
 * trickmodes
 *
 * 16   8/29/08 5:48p vsilyaev
 * PR 33812: Added support for navigation in the MPEG-2 TS file without an
 * index
 *
 * 15   6/4/08 10:18a vsilyaev
 * PR 43184: Don't detect EOS in the timeshifting mode
 *
 * 14   5/16/08 12:12p vsilyaev
 * PR 42379: Moved bmedia_plauer_pos to bmedia_util.h
 *
 * 13   3/6/08 11:23a gmohile
 * PR 38347 : Set seperate timestamp offset for audio and video
 *
 * 12   1/10/08 2:48p erickson
 * PR36068: rename btm to brcm
 *
 * 11   12/20/07 10:29a vsilyaev
 * PR 38073: Updated to work with new bmedia_player
 *
 * PR38073/2   12/19/07 5:01p vsilyaev
 * PR 38073: Moved time tracking code for host paced mode into the
 * bmedia_player
 *
 * PR38073/1   12/14/07 4:51p vsilyaev
 * PR 38073: Added NAV player into the media player
 *
 * 10   8/17/07 3:50p vsilyaev
 * PR 33812: Added factory into the player configuration
 *
 * 9   7/16/07 6:10p vsilyaev
 * PR 29125: Added documentation
 *
 * 8   6/6/07 5:23p vsilyaev
 * PR 31887: Use asynchronous I/O for MP4 container
 *
 * 7   5/16/07 9:57p vsilyaev
 * PR 28631: Use scatter/gather object from the file buffer
 *
 * 6   5/11/07 4:05p vsilyaev
 * PR 29921: Added fast forward for MP4 container
 *
 * 5   5/8/07 3:13p vsilyaev
 * PR 29921: Added timestamp utilities
 *
 * 4   5/1/07 3:24p vsilyaev
 * PR 28631: Integrated support for MP4 container
 *
 * 3   4/24/07 4:40p vsilyaev
 * PR 29921: Added timestamp scaler
 *
 * 2   4/17/07 7:38p vsilyaev
 * PR 29921: Improved AVI media player
 *
 * 1   4/17/07 1:57p vsilyaev
 * PR 29921: AVI player
 *
 *
 *******************************************************************************/
#ifndef _BMEDIA_PLAYER_H__
#define _BMEDIA_PLAYER_H__

#include "bfile_io.h"
#include "bmedia_types.h"
#include "bmedia_util.h"
#include "bfile_buffer.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BMEDIA_PLAYER_MAX_TRACKS	16

/*================== Module Overview =====================================
This module is a helper moduie that provides uniform interface to playback with trickmodes for
the various multimedia container formats (ASF, AVI, MP4).

Result of this module is an action that shall be performed by actual playback module. There is a list of some actions:
    * read XXX bytes from offsett YYYY and send the to bmedia_filter
	* send XXX bytes from the buffer YYY and send the  to bmedia_filter

This module is just a wrapper for the stream specific modules, e.g. it unifies functionality
of bavi_player, basf_player, bmp4_player with a single interface.
========================================================================*/


/**
Summary:
  bmedia_player_t is the object that encapsulates state if the library.
**/
typedef struct bmedia_player *bmedia_player_t;



/**
Summary:
 Number of bmedia_player_pos units in one second
**/
#define BMEDIA_PLAYER_POS_SCALE	(1000)

/**
Summary:
 Interval (in msec) on how often bmedia_player_update_position shall be called
**/
#define BMEDIA_UPDATE_POSITION_INTERVAL (1000)


/**
Summary:
 Max size of block to send to playback. Truncate any blocks larger than that size.
**/
#define BMEDIA_PLAYER_MAX_BLOCK (4*1024*1024)


/**
Summary:
	position increment in msec
**/
typedef signed long bmedia_player_step;

#define BMEDIA_PLAYER_INVALID ((bmedia_player_pos)(-1))

/**
Summary:
  This enum specifies type of action requested by the player
**/
typedef enum bmedia_player_entry_type {
	bmedia_player_entry_type_file, /* player requested to read data from the file */
	bmedia_player_entry_type_embedded, /* player has provided data */
	bmedia_player_entry_type_noop, /* player was not able to provide any information at this time, however it should be able to at a later time */
	bmedia_player_entry_type_atom, /* player has provided  batom_t with a data */
	bmedia_player_entry_type_async, /* player requested asyncronoush action, application must wait fo asynchronous action to complete before calling into the hmedia_player */
	bmedia_player_entry_type_no_data, /* player temporarily wasn't able to read data from the file */
	bmedia_player_entry_type_error, /* player encountered error */
	bmedia_player_entry_type_end_of_stream /* player reached end of stream (or begining of the stream if running in reverse) */
} bmedia_player_entry_type;

/**
Summary:
  This enum specifies kind of data is sent with the entry
**/
typedef enum bmedia_player_entry_content {
    bmedia_player_entry_content_unknown, /* type of content is unknown */
    bmedia_player_entry_content_header, /* content is a type of a header */
    bmedia_player_entry_content_payload /* content is a data payload */
} bmedia_player_entry_content;

/**
Summary:
  This structure is used to set timestamp when resync is required
**/
typedef struct bmedia_player_sync_entry {
    struct {
        uint16_t stream_id;
	    bmedia_player_pos timestamp;
    } resync[BMEDIA_PLAYER_MAX_TRACKS+1];  /* same size media_player_stream other */
} bmedia_player_sync_entry;


/**
Summary:
  This structure defined action that requested by player from an application
**/
typedef struct bmedia_player_entry {
	bmedia_player_pos timestamp; /* if not 0, then timestamp  of the current entry, application shall call function bmedia_filter_set_offset function */
  	const bmedia_player_sync_entry *entry; /* if not NULL, then timestamp  of the current entry, application shall call function bmedia_filter_set_offset function for each stream */
	off_t start;	/* start offset to the block */
	size_t length;  /* length of the block */
	bmedia_player_entry_type type; /* type of the entry */
	const void *embedded; /* if type=bmedia_player_entry_type_embedded  than this pointer is not NULL, then application shall use given data, instead of trying read data from the file */
	batom_t atom; /* if type=bmedia_player_entry_type_atom than this pointer is not NULL, and application shall use given data, instead of trying read data from the file */
    bmedia_player_entry_content content; /* type of content is pointed by the  media_player_entry */
    uint16_t filteredPid; /* if not zero, then payload of only this pid should be handed down */
} bmedia_player_entry;


/**
Summary:
  This structure is used to return bounds (limits) of the media container
**/
typedef struct bmedia_player_bounds {
	bmedia_player_pos first; /* first valid location in the file, in ms */
	bmedia_player_pos last;  /* last location in the file, in ms */
} bmedia_player_bounds;

/**
Summary:
  This structure is used to return status of the player
**/
typedef struct bmedia_player_status {
	  bmedia_player_bounds bounds; /* bounds of the current container */
	  bmedia_player_step direction; /* current direction */
	  unsigned index_error_cnt; /* number of errors detected when processing index data */
	  unsigned data_error_cnt; /* bumber of errors detected when processing data payload */
	  bstream_mpeg_type format; /* formats of the streeam that is produced by the player, it's usually the same as input format of MPEG-2 PES, format remains constant for the players lifecycle */
      bmedia_player_pos position; /* current position player position,  and it's different from results of bmedia_player_tell which often returns currently decoded/displayed position */
} bmedia_player_status;

/**
Summary:
  Enumerates the host trick modes
**/
typedef enum bmedia_player_host_trick_mode
{
	bmedia_player_host_trick_mode_auto,
	bmedia_player_host_trick_mode_normal,
	bmedia_player_host_trick_mode_I,
	bmedia_player_host_trick_mode_skipB,
	bmedia_player_host_trick_mode_IP,
	bmedia_player_host_trick_mode_skipP,
	bmedia_player_host_trick_mode_brcm,
	bmedia_player_host_trick_mode_gop,
	bmedia_player_host_trick_mode_time_skip
} bmedia_player_host_trick_mode;

/**
Summary:
  This structure is used to specify decoder configuration
**/
typedef struct bmedia_player_decoder_config {
	  bool brcm; /* Broadcom's Mpeg Trick Mode playback algorithm. */
	  bool dqt; /* Broadcom's display queue trickmode playback algorithm. */
      bool fragmented; /* Decoder supports decoding of fragmented data */
      bool otf; /* Decoder supports on-the-fly trickmodes (rewind and fast forward) of fragmented data */
      bool stc; /* Whether decoder supports STC based trickmodes (e.g. decoder could run faster then rate specified in the stream) */
	  bmedia_time_scale max_decoder_rate; /* maximum fast forward rate supported by the decoder */
	  bmedia_player_host_trick_mode host_mode; /* requires mode host mode of operation */
	  int mode_modifier; /* modified for host_mode */
      unsigned video_buffer_size; /* size of the video decoder compressed buffer, bytes */
} bmedia_player_decoder_config;


/**
Summary:
  This structure is used to specify application specific information
**/
typedef struct bmedia_player_config {
	  bfile_buffer_t buffer;  /* buffer that is used to access a file, this buffer shall be configured to support asynchronous operations */
	  batom_factory_t factory;
	  void *cntx; /* context that is passed into the user callback */
	  void (*error_detected)(void *cntx); /* callback is called when error detected */
	  void (*atom_ready)(void *cntx, bmedia_player_entry *entry); /* callback is called after completion of asynchronous action */
	  bool timeshifting; /* true if playback file is used as timeshifting buffer */
      bool reorder_timestamps; /* reorder timestamps from display into the decode order */
      bool autoselect_player; /* auto select which player to use */
      size_t prefered_read_size; /* prefered block size  for read from the index file */
      size_t max_data_parsed; /* maximum amount of data player could read when discovering the stream properties */
	  bmedia_player_decoder_config decoder_features; /* decoders mode of operation */
      size_t max_pes_size; /* maximum amount of data to be placed in a pes packet.  0 is unbounded */
      size_t key_frame_distance; /* distance (bytes) in the source stream between key frames, if 0 software guesses distance between key frames */
	  off_t data_file_size; /* fixed size of the data file. used to trim an index that extends past the data file. */
      bmedia_player_pos force_seek_step; /* if the seek step is not 0, use that for the size of seeks */
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_DELAYED_RECORD)
      unsigned long delayRecordedOffsetAlign; /* for delay record file playback */
      unsigned long delayedRecordingStatus; /*current delayed recording status: bit0: 1 (current mode is delayed recording)*/
#endif
} bmedia_player_config;

/**
Summary:
  This structure is used to define stream that player shall manage
**/
typedef struct bmedia_player_stream {
	bstream_mpeg_type format; /* formats of the streeam */
	uint16_t master; /* master sub-stream(track), must be video track */
	uint16_t other[BMEDIA_PLAYER_MAX_TRACKS]; /* other sub-streams(tracks), 0 means empty slot */
	uint16_t drop[BMEDIA_PLAYER_MAX_TRACKS];  /* list of tracks which the player should ignore during output. All streams default to output unless specified here */
    bool without_index;
    struct {
        struct {
            unsigned packet_size; /* 188 or 192 bytes */
        } mpeg2ts;
        struct {
            unsigned bitrate; /* stream bitrate, in bits per second */
            bool auto_rate; /* use adaptive algorithm to measure the stream rate */
        } noindex;
		struct { /* ID's used to packetize a stream */
			uint8_t master;
			uint8_t other[BMEDIA_PLAYER_MAX_TRACKS];
		} id;
        struct {
            baudio_format audio_codec; /* audio codec for the master track */
            bvideo_codec video_codec; /* video codec for the master track */
	        baudio_format other_audio[BMEDIA_PLAYER_MAX_TRACKS]; /* audio types for other tracks, baudio_format_unknown means track is not audio tracke  */
	        bvideo_codec other_video[BMEDIA_PLAYER_MAX_TRACKS]; /* video codec for other tracks, bvideo_codec_unknown means track is not video track */
        } es;
    } stream;
	void *cntx;
	void (*decrypt_callback)(void *cntx, batom_cursor *cursor, size_t *length, void *drm_info, unsigned track_no);
} bmedia_player_stream;

/**
Summary:
  This enum is used to select what frames should be displayed by the decoder
**/
typedef enum bmedia_player_decoder_frames {
    bmedia_player_decoder_frames_all, /* all decodable frame should be displayed */
    bmedia_player_decoder_frames_IP,   /* display I(reference) and P frames only */
    bmedia_player_decoder_frames_I   /* display I(reference) frames only */
} bmedia_player_decoder_frames;

/**
Summary:
  This enum is used to select on the fly PVR mode of operation
**/
typedef enum bmedia_player_reordering_mode {
    bmedia_player_reordering_mode_none,         /* no reordering */
    bmedia_player_reordering_mode_sequential,   /* data sent without discontinuities */
    bmedia_player_reordering_mode_gop,          /* unused */
    bmedia_player_reordering_mode_interleaved,  /* data sent in interleaved chunks */
    bmedia_player_reordering_mode_forward,      /* data sent in forward direction */
    bmedia_player_reordering_mode_backward      /* data sent in forward direction */
} bmedia_player_reordering_mode;

/**
Summary:
  This structure is used to communicate back to the application desired state of the decoder
**/
typedef struct bmedia_player_decoder_mode {
    bmedia_time_scale time_scale; /* time factor of decode */
    bool discontinuity; /* player would introduce discontinuity into the stream */
    bool brcm; /* If true player uses Broadcom's Mpeg Trick Mode playback algorithm. */
    bool dqt; /* If true player uses Broadcom's display trickmode playback algorithm. */
    bool tsm; /* If true player generates stream, that requires decoder to perform timestamp management */
    bool continuous; /* If true player would send continuous data, otherwise player sends only parts of the stream */
    bool host_paced; /* player would change timestamps in the stream */
    bool otf; /* player would set random fragments of data */
    bool fragmented; /* player would set random fragments of data */
    unsigned force_source_frame_rate; /* forced source frame rate, in units of 1/1000 Hz */
    bmedia_player_decoder_frames display_frames; /* what frames should be displayed by the decoder, decoder free to discard other frames */
    bmedia_player_reordering_mode reordering_mode; /* reordering mode for on the fly PVR */
    bool simulated_tsm;
} bmedia_player_decoder_mode;

/**
Summary:
  This function initializes bmedia_player_stream structure
**/
void bmedia_player_init_stream(
		bmedia_player_stream *stream /* pointer to structure to initialize */
		);

/**
Summary:
  This function initializes bmedia_player_config structure
**/
void bmedia_player_init_config(
		bmedia_player_config *config /* pointer to structure to initialize */
		);

/**
Summary:
  This function creates new instance of bmedia_player_t
**/
bmedia_player_t
bmedia_player_create(
		bfile_io_read_t fd,  /* instance of read_only file descriptor */
		const bmedia_player_config *config,  /* pointer to the application specific configuration */
		const bmedia_player_stream *stream   /* pointer to the stream description */
		);

/**
Summary:
  This function releases resources allocated for the instance of bmedia_player
**/
void bmedia_player_destroy(
		bmedia_player_t player	/* instance of bmedia_player_t */
		);

/**
Summary:
  This function lets player to return next action
Description:
  The bmedia_player_next function fills out bmedia_player_entry structure with
  instructions on what application shall do.
Returns:
  0 - Success
  negative - if out of bound data was reached or error occured
**/
int bmedia_player_next(
		bmedia_player_t player, 	/* instance of bmedia_player_t */
		bmedia_player_entry *entry  /* pointer to the next action */
		);

/**
Summary:
  This function returns current position of the player
**/
void bmedia_player_tell(
		bmedia_player_t player,		/* instance of bmedia_player_t */
		bmedia_player_pos *pos		/* pointer to the location */
		);

/**
Summary:
  This function returns status of the bmedia_player
**/
void bmedia_player_get_status(
		bmedia_player_t player, 		/* instance of bmedia_player_t */
		bmedia_player_status *status
		);

/**
Summary:
  This function sets new direction for the player
Description:
  The bmedia_player_set_status function is used to set desired direction for the player. Player could operate if two distinctive modes of operation:
    - normal play, in this mode player would instruct application in a way that it sends all data to the stream processoer (e.g. audio and video)
	- truck modde, in this mode player would provide actions to application that it would only send key(I) frames for the selected master track. Distance between frames would not exceed value of direction, e.g. if direction is set to 10000, then application would send key(I) frames that at least 10 seconds appart
Returns:
  0 - Success
  Error code - can't change direction
**/
int bmedia_player_set_direction(
		bmedia_player_t player, /* instance of bmedia_player_t */
		bmedia_player_step direction, /* new direction, 0 is used for normal play, negative values for rewind and positive for fast forward */
		bmedia_time_scale time_scale, /* timescale factor that is currently used */
        bmedia_player_decoder_mode *mode /* new decoder mode */
		);

/**
Summary:
  This function sets new location for the player.
Description:
  Location shall be inside player bounds. After seek data would be delivered from the nearest next key(I) frames.
Returns:
  0 - success
  -1 - error during seek (e.g. seek out of bounds)
**/
int bmedia_player_seek(
		bmedia_player_t player, /* instance of bmedia_player_t */
		bmedia_player_pos pos   /* new location for the player, in ms */
		);

/**
Summary:
  This function updates player with latest presentation time, obtained from the decoder
**/
void bmedia_player_update_position(bmedia_player_t player, uint32_t pts);

/**
Summary:
  This function verifies whether given sub-stream listed in the stream descriptor
**/
bool bmedia_player_stream_test(
		const bmedia_player_stream *stream,
		unsigned sub_stream
		);

/**
Summary:
  This function returns id for media sub_stream (track)
**/
uint8_t bmedia_player_stream_get_id(
		const bmedia_player_stream *stream,
		unsigned sub_stream
		);

/**
Summary:
  This function returns delta between two timestamps
**/
int bmedia_player_pos_delta(bmedia_player_pos future, bmedia_player_pos past);

/**
Summary:
  This function checks whether to timestamps are within required range
**/
bool bmedia_player_pos_in_range(bmedia_player_pos future, bmedia_player_pos past, bmedia_player_pos range);


/**
Summary:
Lookup a PTS value based on a position
**/
int bmedia_player_lookup_pts(
		bmedia_player_t player,		/* instance of bmedia_player_t */
		bmedia_player_pos pos,		/* pointer to the location */
		uint32_t *p_pts /* [out] PTS for that location */
		);


/**
Summary:
Initialize bmedia_player_status structure
**/
void bmedia_player_init_status(bmedia_player_status *status);

/**
Summary:
Set media player decoder config
**/
int bmedia_player_set_decoder_config (bmedia_player_t player, const bmedia_player_decoder_config *config);

/**
Summary:
Get media player decoder config
**/
void  bmedia_player_get_decoder_config (bmedia_player_t player, bmedia_player_decoder_config *config);

/**
Summary:
  This function initializes bmedia_player_entry structure
**/
void bmedia_player_init_entry(
		bmedia_player_entry *entry /* pointer to structure to initialize */
		);

/**
Summary:
  This function initializes bmedia_player_sync_entry structure
**/
void bmedia_player_init_sync_entry(
		bmedia_player_sync_entry *entry /* pointer to structure to initialize */
		);


/**
Summary:
  This structure defines interface that each compatible media container shall implement
**/
typedef struct bmedia_player_methods {
	void *(*create)(bfile_io_read_t fd, const bmedia_player_config *config, const bmedia_player_stream *stream);
	void (*destroy)(void *player);
	int  (*next)(void *player, bmedia_player_entry *entry);
	void (*tell)(void *player, bmedia_player_pos *pos);
	void (*get_status)(void *player, bmedia_player_status *status);
	int (*set_direction)(void *player, bmedia_player_step direction, bmedia_time_scale time_scale, bmedia_player_decoder_mode *mode);
	int  (*seek)(void *player, bmedia_player_pos pos);
} bmedia_player_methods;


/* when reached EOS without finding right index, step back by 1000ms (e.g. 1sec) */
#define BMEDIA_PLAYER_SEEK_STEP (1000)
/* try 10 times and then fail */
#define BMEDIA_PLAYER_SEEK_TRIES    10


#ifdef __cplusplus
}
#endif


#endif /* _BMEDIA_PLAYER_H__ */

