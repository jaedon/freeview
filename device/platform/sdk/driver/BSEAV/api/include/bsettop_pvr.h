/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_pvr.h $
 * $brcm_Revision: 40 $
 * $brcm_Date: 9/2/10 4:43p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_pvr.h $
 * 
 * 40   9/2/10 4:43p vsilyaev
 * SWDEPRECATED-1003: Added bplayback_player_mode_time_skip
 * 
 * 39   9/29/09 4:55p vsilyaev
 * SW7405-3107: Added new member to the bplayback_status to facilitate
 * extraction of IP timeouts
 * 
 * 38   6/24/09 6:48p vsilyaev
 * PR53773: Adding DTCP/IP + SSL Support in Brutus
 * 
 * PR53773/1   6/24/09 5:22p lwhite
 * PR53773: Adding DTCP/IP + SSL Support in Brutus
 * 
 * 37   6/22/09 7:11p vsilyaev
 * PR 56180: Added max_decoder_rate
 * 
 * 36   10/31/08 9:40a jgarrett
 * PR 47230: Adding TTS Pacing
 * 
 * PR47230/1   10/28/08 10:37a jjordan
 * PR47230: Port TTS Pacing to Nexus
 * 
 * 35   8/11/08 1:44p vsilyaev
 * PR 45431: Allowed to use id_addr field as the domain name and increased
 * its length
 * 
 * 34   7/21/08 2:28p jgarrett
 * PR 45013: Removing record index and data write callbacks
 * 
 * 33   5/5/08 5:55p vsilyaev
 * PR 42365: Added auxilary way of setting loopmode
 * 
 * 32   3/6/08 2:01p vsilyaev
 * PR 36248: Pass mpeg parameters into the brecord_file_open_network and
 * bfile_posix_out_network
 * 
 * 31   1/30/08 10:37a vsilyaev
 * PR 39014: Added skip count for DQR trick modes
 * 
 * 30   1/16/08 11:54a jgarrett
 * PR 38608: Merging to main branch
 * 
 * 29   1/3/08 10:32a vsilyaev
 * PR37038: PR37038: convert continuous record to use beginning_of_stream
 * callbacks
 * 
 * PR37038/1   12/11/07 2:51p erickson
 * PR37038: convert continuous record to use beginning_of_stream callbacks
 * 
 * Netaccel_Devel/1   12/18/07 6:04p ssood
 * PR38068: added trick play related options to socket params for HTTP
 * network trickmode support
 * 
 * 28   8/9/07 5:49p jgarrett
 * PR 33830: Adding network record prototypes
 * 
 * 27   6/28/07 4:36p vsilyaev
 * PR 32609: Added option to start playback in paused state
 * 
 * 26   5/17/07 1:17p vsilyaev
 * PR 30310: Added bplayback_player_mode_by_gop type
 * 
 * 25   4/19/07 7:28p jgarrett
 * PR 30010: Add TCP support
 * 
 * 24   12/7/06 10:48a jgarrett
 * PR 26293: Adding missing prototype for bplayback_ip_close.
 * 
 * 23   11/29/06 3:57p ssood
 * PR26252: IP STB SW unification w/ Generic Brutus release
 * 
 * 22   11/17/06 10:43a erickson
 * PR25230: move bcmplayer from SetTop/bcmplayer to BSEAV/lib/bcmplayer.
 * Remove SetTop from Brutus/Settop API build.
 * 
 * 21   10/4/06 5:59p dlwin
 * PR24714: Add support for RTSP protocol, merged from branch.
 * 
 * PROD_IPSTB_REFSW_Devel/13   10/3/06 4:49p jjordan
 * PR24714: Add support for RTSP protocol
 * 
 * 20   9/12/06 5:13p erickson
 * PR23062: added bplayback_player_mode_by_rate
 * 
 * 19   7/26/06 9:38a dlwin
 * PR22906: Merged into main-line, for: Integrate liveMedia open-source
 * for RTP and RTCP support
 * 
 * 18   7/12/06 3:18p ahulse
 * PR16324: get playback / record time via call to bplayback_get_status /
 * brecord_get_status
 * 
 * 17   4/20/06 3:45p dlwin
 * PR17547: add basic RTP support to NET_IF DMA kernel-mode driver, merged
 * to main branch.
 * 
 * PROD_IPSTB_REFSW_Devel/11   4/20/06 12:54p jjordan
 * PR17547: add basic RTP support to NET_IF DMA kernel-mode driver
 * 
 * 16   3/6/06 3:22p erickson
 * PR19785: added decode_mode to public api
 * 
 * 15   3/1/06 4:52p dlwin
 * PR18981: PR18981: fix "live playback" and add config option
 * IP_USE_LIVE_PLAYBACK
 * 
 * PR18981/1   3/1/06 3:24p jjordan
 * PR18981: fix "live playback" and add config option IP_USE_LIVE_PLAYBACK
 * 
 * 14   1/9/06 2:47p dlwin
 * PR 14867: Update documentation to reflect that for non-legacy
 * platforms, the SettopAPI is thread safe.
 * 
 * 13   11/30/05 3:14p vsilyaev
 * PR 18183: Added FIFO file support
 * 
 * 12   11/17/05 4:23p vsilyaev
 * PR 18183: fixed some comments
 * 
 * 11   10/13/05 4:48p dlwin
 * PR 17547: Created the merge arrow for IP STB.
 * 
 * 10   10/12/05 4:37p erickson
 * PR17547: Add support for RTP (PROD_IPSTB_REFSW_Devel)
 * 
 * 9   8/4/05 9:08a erickson
 * PR16504: converted playpump_params.band to
 * playpump_params.route_through_parser_band
 * 
 * 8   7/27/05 3:29p vsilyaev
 * PR 16385: Merge IP record feature
 * 
 * PROD_IPSTB_REFSW_Devel/7   8/4/05 4:12p wesleyl 
 * PR15072: remove mpeg from brecord_params 
 *  
 * PROD_IPSTB_REFSW_Devel/6   8/4/05 2:45p wesleyl 
 * PR15072: Remove unncessary recording stuffs in bplayback_param 
 *  
 * PROD_IPSTB_REFSW_Devel/5   7/25/05 6:41p wesleyl
 * PR15072: fix comments
 * 
 * PROD_IPSTB_REFSW_Devel/4   7/15/05 4:04p wesleyl
 * PR15072: Add IP record support
 * 
 * PROD_IPSTB_REFSW_Devel/3   7/13/05 4:01p wesleyl
 * PR15072: Modify bplayback_param for IP recording
 * 
 * PROD_IPSTB_REFSW_Devel/2   7/10/05 2:29p wesleyl
 * PR15072: Add callback functions to brecord_ip_param
 * 
 * PROD_IPSTB_REFSW_Devel/1   6/30/05 11:07a wesleyl
 * PR15072: Add IP record support
 * 
 * 7   6/9/05 3:24p erickson
 * PR15513: all pass record is now audio pid 0 or 0x1fff. large warnings
 * now printed.
 * 
 * 6   5/6/05 11:15a jjordan
 * PR15200: make IP STB buffer scheme selectable
 * 
 * 5   4/20/05 4:55p vsilyaev
 * PR 14724: Added option to activate use of timestamps.
 * 
 * 4   3/18/05 3:52p erickson
 * PR14511: updated docs
 * 
 * 3   2/16/05 4:44p erickson
 * PR13959: remove #if's from public header files
 * 
 * 2   2/12/05 11:06a dlwin
 * Merge down for release 2005_REFSW_MERGETOMAIN:
 * 
 * Irvine_BSEAVSW_Devel/48   2/8/05 9:45a jjordan
 * PR13959: Initial checkin of IP STB
 * 
 * Irvine_BSEAVSW_Devel/47   2/2/05 10:08a erickson
 * PR14006: added file_close functions for playback and record
 * 
 * Irvine_BSEAVSW_Devel/46   11/23/04 8:33p vsilyaev
 * PR 13351: returned back bplayback_close.
 * 
 * Irvine_BSEAVSW_Devel/45   11/23/04 6:54p vsilyaev
 * PR 13351: Refactoring record API.
 * 
 * Irvine_BSEAVSW_Devel/44   11/23/04 4:40p vsilyaev
 * PR 13351: Refactoring playback API.
 * 
 * Irvine_BSEAVSW_Devel/BESOTFPVR/3   11/13/04 1:56p vsilyaev
 * Separated otfpvr to backend and frontend  parts.
 * Added band to the playback parameters added contstructor for otfpvr.
 * 
 * Irvine_BSEAVSW_Devel/BESOTFPVR/2   11/12/04 1:37p vsilyaev
 * Refactored playback interface.
 * 
 * Irvine_BSEAVSW_Devel/BESOTFPVR/1   10/8/04 7:59p vsilyaev
 * Added BESPVR suport
 * 
 * Irvine_BSEAVSW_Devel/43   8/3/04 9:20a erickson
 * PR11124: added comment about usage of file handles
 * 
 * Irvine_BSEAVSW_Devel/42   6/29/04 11:28a erickson
 * PR11135: updated deprecation notices for settop api
 * 
 * Irvine_BSEAVSW_Devel/41   6/17/04 9:36a erickson
 * PR11135: recpump callback enabled
 * 
 * Irvine_BSEAVSW_Devel/40   4/14/04 11:59a vsilyaev
 * PR 10613: Removed position callback.
 * 
 * Irvine_BSEAVSW_Devel/39   1/6/04 12:07p vsilyaev
 * PR 8850: Fixed compile warnings for non OS.
 * 
 * Irvine_BSEAVSW_Devel/38   12/19/03 7:41p vsilyaev
 * PR 8850: Fixed typo.
 * 
 * Irvine_BSEAVSW_Devel/Irvine_BSEAVSW_Devel_7038/1   11/19/03 2:58p vsilyaev
 * Make 7038 compile
 * 
 * Irvine_BSEAVSW_Devel/36   11/10/03 2:38p erickson
 * PR8563: added transport timestamp support to driver, settop api and
 * bcmplayer
 * 
 * Irvine_BSEAVSW_Devel/35   11/7/03 4:28p erickson
 * PR8583: removing, fixing or implementing TODO's
 * 
 * Irvine_BSEAVSW_Devel/34   11/4/03 2:42p erickson
 * documentation changes
 * 
 * Irvine_BSEAVSW_Devel/33   10/31/03 2:18p erickson
 * fixed mpegvideo slowmotion rate. 0 or 1 is normal. 2 is 2x slowmotion.
 * 
 * Irvine_BSEAVSW_Devel/32   10/31/03 11:58a erickson
 * settop api changes after internal review
 * 
 * Irvine_BSEAVSW_Devel/31   10/28/03 11:26a erickson
 * settop api reworks after internal design view
 * 
 * Irvine_BSEAVSW_Devel/30   10/22/03 11:22a erickson
 * settop api pass-through type conversion
 * 
 * Irvine_BSEAVSW_Devel/29   10/15/03 10:55a erickson
 * no longer use "closure" for is_hits. I was getting segfaults in an app
 * (for unknown, but obviously debuggable reasons), but I just don't
 * think this is a good way to write a simple C api. This is the only
 * example of closure in the public api. So I removed it.
 * Also updated some comments.
 * 
 * Irvine_BSEAVSW_Devel/28   10/13/03 2:08p erickson
 * moved trickmode rate code into settop api and converted from two int
 * parameters to a float
 *
 * Irvine_BSEAVSW_Devel/27   10/1/03 10:16a erickson
 * updated information for docjet
 * 
 * Irvine_BSEAVSW_Devel/26   9/30/03 1:13a erickson
 * updated docjet documentation
 * 
 * Irvine_BSEAVSW_Devel/25   9/18/03 5:05p erickson
 * reworked settop api, driver and brutus directory structure
 * 
 * Irvine_BSEAVSW_Devel/24   9/17/03 5:04p erickson
 * updated documentation for DocJet
 * 
 * Irvine_BSEAVSW_Devel/23   9/8/03 5:00p erickson
 * changed goto api to return actual locations
 * play() no longer requires useCurrentPTS flag
 * pipes are now nonblocking
 * 
 * Irvine_BSEAVSW_Devel/22   9/8/03 11:20a erickson
 * combined all status into bplayback_get_status
 * 
 * Irvine_BSEAVSW_Devel/21   9/5/03 3:25p erickson
 * reworked pvr api and threading model. We're now using select to block
 * on either the driver,
 * the command interface or a timeshifting record. We no longer use
 * mutexes.
 * We've removed the getParam/setParam interface and replaced it with
 * get_index_position,
 * get_mpeg_position, get_state, goto_index, goto_timestamp,
 * goto_mpegoffset. It should
 * be much more flexible.
 *
 * Irvine_BSEAVSW_Devel/20   9/2/03 2:26p erickson
 * allow multiple timeshift playbacks. NOTE: the implementation is
 * flawed...for now
 * 
 * Irvine_BSEAVSW_Devel/19   8/28/03 1:21p erickson
 * numBytesPlayed is tracked in the settop and not from syslib
 *
 * Irvine_BSEAVSW_Devel/18   8/19/03 1:42p erickson
 * added comment for future extensions
 * 
 * Irvine_BSEAVSW_Devel/17   8/19/03 11:54a erickson
 * initial brutus bringup in progress
 * 
 * Irvine_BSEAVSW_Devel/16   8/18/03 4:53p erickson
 * timeshifting pvr now working
 *
 * Irvine_BSEAVSW_Devel/15   8/18/03 3:47p erickson
 * brutus bringup, in progress
 * 
 * Irvine_BSEAVSW_Devel/14   8/18/03 11:02a erickson
 * initial bringup for brutus 7320, still in progress
 * 
 * Irvine_BSEAVSW_Devel/13   8/12/03 9:43a erickson
 * changes after pre-review meeting
 * 
 * Irvine_BSEAVSW_Devel/12   7/30/03 2:08p erickson
 * removed all return value comments because we're standardized and they
 * add nothing
 *
 * Irvine_BSEAVSW_Devel/11   7/29/03 6:03p erickson
 * more rework
 * 
 * Irvine_BSEAVSW_Devel/10   7/28/03 4:43p erickson
 * more comment work
 *
 * Irvine_BSEAVSW_Devel/9   7/25/03 8:14p vsilyaev
 * open now has two arguments.
 * 
 * Irvine_BSEAVSW_Devel/8   7/25/03 4:22p erickson
 * in the middle of rework, does not compile
 *
 * Irvine_BSEAVSW_Devel/7   7/9/03 5:19p erickson
 * rework for new bband_t model
 *
 * Irvine_BSEAVSW_Devel/6   6/26/03 3:59p vsilyaev
 * Use structure to pass parameters to the playback_start function.
 *
 * Irvine_BSEAVSW_Devel/5   6/24/03 5:29p vsilyaev
 * Use brecord_file_t and bplayback_file_t instead bfile_in and bfile_out.
 *
 * Irvine_BSEAVSW_Devel/4   6/23/03 7:13p vsilyaev
 * Added index format to the record start.
 *
 * Irvine_BSEAVSW_Devel/3   6/20/03 5:56p vsilyaev
 * Added playback support.
 *
 * Irvine_BSEAVSW_Devel/2   6/16/03 8:18p vsilyaev
 * Added constructors for file_in and file_out .
 *
 * Irvine_BSEAVSW_Devel/1   6/13/03 3:09p vsilyaev
 * SetTop reference API.
 *
 *
 ***************************************************************************/
#ifndef BSETTOP_PVR_H__
#define BSETTOP_PVR_H__

#include "bsettop_stream.h"
#include "bsettop_recpump.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*=*********************************************************
PVR (personal video recorder) is a common name for playback and record
of digital video using a set-top's harddrive. It is also referred to as
DVR (digital video recorder).

The Settop API handles All threading, indexing and file i/o issues
involved in implementing PVR. We support two index formats, and you can also
playback with no index. Indexes are required for trick modes.

The Settop API supports trick modes and timeshifting
(simultaneous playback and record of the same stream).

For legacy platforms (ie non-Magnum platforms), the Settop API in general is not re-entrant except for explicitly allowed.
However, you can call bplayback_get_status and brecord_get_status from another
thread.

For non-legacy platforms, the Settop API in general is thread-safe unless explicitly noted.

************************************************************/

/* Playback API */

/*
Summary:
	Opens a new playback object.
*/
bplayback_t bplayback_open(void);

/*
Summary:
	Open an IP playback channel.
*/
bplayback_t bplayback_ip_open(void);


/*
Summary:
	Close a playback object.
*/
void bplayback_close(
	bplayback_t playback /* Handle returned by bplayback_open */
	);


/*
Summary:
	Close an IP playback channel.
*/
void bplayback_ip_close(
	bplayback_t playback /* Handle returned by bplayback_ip_open */
	);


/*
Summary:
Open a playback file to pass to bplayback_start().
Description:
The bplayback_file_t can be used for only one playback at a time.
*/
bplayback_file_t bplayback_file_open(
	const char *mpeg_file_name,		/* mpeg file name. Should include path relative to the
									current directory of the application. */
	const char *index_file_name		/* index file name. Should include path relative to the
									current directory of the application.
									Can be NULL for no index.
									The format of the index is specified in the
									index_format member of bplayback_params. */
	);

/**
Close a file after playback is stopped
**/
void bplayback_file_close(
	bplayback_file_t file
	);


/**
Summary:
	State of bplayback returned by bplayback_get_status.
**/
typedef enum bplayback_state {
	bplayback_state_stopped,	/* Not started or stopped. */
	bplayback_state_playing,	/* Normal playback */
	bplayback_state_paused,		/* Decode is paused but can be resumed. */
	bplayback_state_trickmode,	/* Some variety of trick mode in progress. */
	bplayback_state_frameadvance	/* In a paused state, but a frame advance is either
									in progress or has been done since the pause. */
} bplayback_state;

/*
Summary:
	PVR index formats.
*/
typedef enum bindex_format {
	bindex_format_none,			/* no index */
	bindex_format_sct,			/* start code index created by hardware and
												playable with tsplayer. See bcmplayer source and docs. */
	bindex_format_bcm			/* Broadcom index created by bcmindexer and
												playable with bcmplayer. See bcmplayer source and docs.*/
} bindex_format;

/**
Summary:
bplayback_loopmode is a value you can return from the end_of_file or beginning_of_file
callbacks to tell the playback engine what action to take.

Description:
We implemented loopmode as a callback because the app may take different actions
at different times and because the app might need to be notified when
loops take place. This accomplishes both.
**/
typedef enum bplayback_loopmode {
	bplayback_loopmode_loop,	/* Loop to the other end of the stream */
	bplayback_loopmode_pause,	/* Pause the decode at the current spot */
	bplayback_loopmode_play	    /* Resume normal play at the beginning or pause at
									the end. */
} bplayback_loopmode;

/**
Summary:
bplayback_position is passed on bplayback_callback's position callback.

Description:
You can read various types of position information. The pointer members should
be tested for NULL first. If not NULL, then you can dereference them and read
valid values. Non-pointer members are always valid.
**/
typedef struct bplayback_position {
	off_t mpeg_file_offset;		/* Position in the mpeg file. */
	bool has_index;				/* If true, then the following index position values
									are valid. */
	unsigned long index_offset;	/* Offset of the current index entry in the index.
									Only valid if has_index is true */
	unsigned long timestamp;	/* Timestamp of the current index entry in the index.
									Timestamp is different from PTS. Timestamp is generated
									by bcmindexer, it begins at 0 for any recorded stream,
									and is guaranteed to be continuous increasing throughout
									the stream. The units of the timestamp depend on the
									implementation of bcmindexer, but they are defaulted
									to milliseconds.
									Only valid if has_index is true */
} bplayback_position;

/*
Summary:
This strcuture is used to communicate the buffering scheme employed by the IP STB
*/
typedef enum bplayback_ip_buffer_scheme {
	bplayback_ip_buffer_scheme_precharge,			/* precharge the buffer only */
	bplayback_ip_buffer_scheme_stc_clock_steering	/* steer the STC clock in response to buffer violations */
} bplayback_ip_buffer_scheme;

typedef struct bplayback_ip_params {
	bplayback_ip_buffer_scheme buffer_scheme;
	bool use_live_playback_mode;
} bplayback_ip_params;

typedef struct bplayback_tts_params {
	bool auto_detect; /* automatically detect if the stream is TTS or TS */
	unsigned long pacing_max_error; /* set the timestamp error bound, as used by the playback pacing logic */
	unsigned init_buf_depth; /* sets the initial buffer depth before playback begins, as used by the playback pacing logic */
	unsigned min_buf_depth;	/* minimum buffer depth before tts throttle buffer depth violation */
	unsigned max_buf_depth;	/* maximum buffer depth before tts throttle buffer depth violation */
	unsigned max_clock_mismatch;	/* specify the maximum clock mismatch (in ppm) between server/encoder and STB */
} bplayback_tts_params;
/*
Summary:
This structure used to communicate initial parameters to the
playback_start function.
*/
typedef struct bplayback_params {
	int starting_index_offset; /* Offset into the index at which playback is to start. The meaning of the
									offset depends on index format. -1 means end of file. */
	bindex_format index_format; /* Index format for the index_file_name passed to bplayback_file_open. */
	bool timestamp_enabled;	/* If true, then a 4 byte timestamp has been prepended to every
								transport packet. brecord_params.timestamp_enabled
								must match. */

	void *callback_context;			/* Context pointer which is returned in the callback functions.
										This could be the bplayback_t handle, or anything you'd like. */
	bplayback_loopmode (*end_of_stream)(void *context);	/* Called out of the internal playback thread whenever
										it reaches the end of the stream. You cannot call a settop api
										function from this callback. */
	bplayback_loopmode (*beginning_of_stream)(void *context); /* Called out of the internal playback thread whenever
										it reaches the beginning of the stream. You cannot call a settop api
										function from this callback. */
	void (*error)(void *context);		/* Called out of the internal playback thread whenever
										the a fatal error happens. This means that the playback is about to be stopped.
										You cannot call a settop api function from this callback. */
	bool route_through_parser_band;		/* if true, playback shall be routed through a parser band */
	bool timestamp_active; 	/* use timestamps  to dynamicaly control playback rate (pacing) */
	bool start_paused;	/* start playback in paused state, bplayback_play function could be used to resume playback */
	bplayback_tts_params tts_params;
	bplayback_ip_params ip_params;
    struct {
        bplayback_loopmode end_of_stream; /* shall match value returned by end_of_stream callback */
        bplayback_loopmode beginning_of_stream; /* shall match value returned by beginning_of_stream callback */
    } loopmode; 
} bplayback_params;

/*
Summary:
	Required to initialize the bplayback_params structure.
*/	
void
bplayback_params_init(
	bplayback_params *params,	/* [out] */
	bplayback_t playback		/* required for possible resource-dependent defaults */
	);

/*
Summary:
  Configures playback to be started (NOTE: does not actually start playback).

Description:
	If bplayback_start succeeds, it returns a digital stream handle which can
	be decoded or recorded.

	Playback does not actually begin until the stream has a consumer.
	Consumers could be decode or record or loopback through a live band.
*/
bstream_t bplayback_start(
	bplayback_t playback, /* Handle returned by bplayback_open */
	bplaypump_t playpump, /* playpump object used to feed data into the system */
	const bstream_mpeg *mpeg, /* MPEG settings for the source */
	bplayback_file_t source, /* Source for the data */
	const bplayback_params *params /* pointer to the playback parameters */
	);

/*
Summary:
	Configure playback to start in timeshifting mode.

Description:
	Timeshifting mode is when you are playing a stream that you are also recording.
	The benefit of having an explicit timeshifting mode is that when playback
	is tight to the end of the file and is waiting for data, it can go to sleep
	until another record event happens. This reduces CPU load.
*/
bstream_t bplayback_start_timeshifting(
	bplayback_t playback, /* Handle returned by bplayback_open */
	bplaypump_t playpump, /* playpump object used to feed data into the system */
	const bstream_mpeg *mpeg, /* MPEG settings for the source */
	bplayback_file_t source,  /* source for the data */
	const bplayback_params *params, /* pointer to the playback parameters */
	brecord_t record /* Active record to which this playback is linked. */
	);

/*
Summary:
	Stop timeshifting but do not stop playback.

Description:
	This function is only required to be called if you want to stop
	record before stopping timeshifting playback. If you try to stop
	record while still timeshifting, brecord_stop() will fail.
*/
bresult bplayback_stop_timeshifting(
	bplayback_t playback /* Handle returned by bplayback_open */
	);

/*
Summary:
	Stop playback.
Description:
	If you are decoding the stream, the decode will be stopped as well.
*/
bresult bplayback_stop(bplayback_t playback);

/*
Summary:
	Mode for bcmplayer or tsplayer.
Description:
	This translated to eBpPlayModeParam in BSEAV/lib/bcmplayer/include/playertypes.h.
*/
typedef enum bplayback_player_mode {
	bplayback_player_mode_normal,	/* Play all frames in the forward direction. "value" is forced to be 1. */
	bplayback_player_mode_i,		/* Play Only I Frames. "value" is the number of I-frames to advance
						each time, and can be positive or negative.
						Note that this is the only valid reverse mode apart from Broadcom
 						Trick Modes (eBpPlayBrcm). */
	bplayback_player_mode_skip_b,	/* Skip B frames. "value" is the number of
						B's that should be shown before a single skip. For example, 1
						would mean play every-other B frame. If value >= the total
						B's in a GOP, it is equivalent to eBpPlayNormal. */
	bplayback_player_mode_ip,		/* Play all I and P Frames. Skip all B's. "value" is ignored. This can
						by used with GOP-based or HITS streams. */
	bplayback_player_mode_skip_p,	/* Skip trailing P frames in a GOP.
						"value" is equal to the number of P's to be played per GOP.
						If value >= the total P's in a GOP, it is equivalent to
						eBpPlayIP.
						Not supported in tsplayer. */
	bplayback_player_mode_brcm,	/* Broadcom's Mpeg Trick Mode playback algorithm.
						"value" is the number of frames to advance each time. It can be positive
						or negative. A value of 1 is not allowed.

						However, for performance reasons we recommend only using values 2, -3,-2 and
						-1 for GOP-based trick modes that can't be obtained from other modes,
						otherwise use the other modes.
						Not supported in tsplayer. */
	bplayback_player_mode_by_gop, /* Player will send portions of GOP's to decoder.
						Display queue trick modes will be used in reverse. 
						"value" is the number of pictures per GOP to send. If value is negative,
						then it is reverse. */
	bplayback_player_mode_by_rate, /* The application specifies a rate. The Settop API, using player,
						decoder, STC and any other means tries to achieve that exact rate.
						"value" is the rate, in units of BPLAYBACK_NORMALPLAY_SPEED. */						
	bplayback_player_mode_time_skip,
	bplayback_player_mode_max
} bplayback_player_mode;

/*
Summary:
	All parameters needed to specify a trick mode.
*/
typedef struct bplayback_trickmode_params {
	bplayback_player_mode mode;	/* Basic type of trick mode to perform. */
	int mode_modifier;			/* Each trick mode has a potential modifier. See the
									bplayback_player_mode docs for more details.  */
	unsigned slow_motion_rate;	/* A slow motion rate can be added on to any trick mode.
								It specifies the number of times the decoder should display each
								field for a slow motion effect. Therefore 2 is a 2x slow
								motion, 3 is a 3x slow motion, etc. 0 or 1 is no slow motion. */
	bplaypump_decode_mode decode_mode; /* decoder mode, instructs decoder to drop certains types of pictures */
    unsigned gop_skip_count;     /* This parameter is used for DQT. It describes the number of gops to skip after a gop has been sent to decoder*/
    unsigned maxDecoderRate;    /* Maximum fast playnack rate supported by the decoder, defined in BPLAYBACK_NORMALPLAY_SPEED units. 
                                For forward rates up to this rate, implementation would feed entire stream to the decoders and then use either STC trick modes 
                                or host paced trickmode, to achieve the target rate.
                                However, decoder bandwidth may prevent the system for achieving the desired rate.
                                The maximum rate varies based on hardware capabilities and stream encoding.
                                Because of this the application can tell Playback what exactly rate is expected to be supported. */
} bplayback_trickmode_params;

/*
Summary:
	Required to initialize bplayback_trickmode_params
*/
void
bplayback_trickmode_params_init(bplayback_trickmode_params *params);

/*
Summary:
	Status returned by bplayback_get_status
*/
typedef struct bplayback_status {
	bplayback_position position;		/* Current position of playback. */
	off_t mpeg_file_size;				/* Size of the mpeg file in bytes */
	bool has_index;						/* Will be true if an index_file_name was passed
											to bplayback_file_open and the
											bplayback_params.index_format was not bindex_none. */
	unsigned long index_first_offset;	/* First index offset. For non-circular files, this is always 0. */
	unsigned long index_last_offset;	/* Last valid index offset.
										index_last_offset-index_first_offset+1 == total index entries. */
	bplayback_state state;				/* Current state of playback */
	bplayback_trickmode_params trickmode_params; /* Current trick mode params if state is
											bplayback_trickmode. */
	unsigned long fifo_depth;			/* Depth in bytes of the playback buffer */
	unsigned long fifo_size;			/* Size in bytes of the playback buffer */
	off_t bytes_played;					/* Total number of bytes played since starting */
	unsigned long first_timestamp;     /* first timestamp if stream played - time in mSecs */
	unsigned long last_timestamp;				/* last timestamp of stream played - time in mSecs */

    unsigned num_read_errors;           /* number of errors occurred when reading data from the mpeg file, for the case of network feed timeouts are also considered as errors */
} bplayback_status;

/*
Summary:
	Get a variety of information about the status of playback
Description:
	It will synchronize with the play thread, so it may block.
	Index information will only be populated if there is an index.
	It can only be called after playback has started.
*/
bresult bplayback_get_status(
	bplayback_t p, 
	bplayback_status *status	/* [out] */
	);

/*
Summary:
	Pause playback after it has started.
Description:
	It can only be called after playback has started.
*/
bresult bplayback_pause(bplayback_t p);

/*
Summary:
	Have the playback get as close as possible to the record data
	when timeshifting.
Description:
	This puts playback into normal play mode.
	It can only be called after playback has started.
*/
bresult bplayback_catchup_record(bplayback_t p);

/*
Summary:
	Resume normal playback from pause or trick mode.
Description:
	It can only be called after playback has started.
*/
bresult bplayback_play(bplayback_t p);

/*
Summary:
	If you are paused, this will advance or reverse one frame.
Description:
	It can only be called after playback has started.
*/
bresult bplayback_frameadvance(
	bplayback_t p, 
	bplayback_player_mode playermode,	/* For forward, this should be bplayback_player_mode_normal.
							For reverse, it can be either _i or _brcm. */
	bool forward	/* is it forward? otherwise, reverse. */
	);

/*
Summary:
	Set a trick mode, either fast or slow, forward or reverse.
Description:
	It can only be called after playback has started.
*/
bresult bplayback_trickmode(
	bplayback_t p,
	const bplayback_trickmode_params *trickmode
	);

/*
Summary:
Determine a trick mode using a rate.

Description:
Use multiples or divisions of BPLAYBACK_NORMALPLAY_SPEED to specify a rate. 

Integer rates specify fast trick modes from a pre-determined table. As the rate
increases, the trick mode used will be faster, but it is no guarantee of actual speed.
A rate of 2 is the 2nd fast forward speed, and a rate of 3 is the 3rd fast forward
speed, which is faster than the 2nd.

Fractional rates specify slow trick modes.
For instance: 0.5 is 2x slow motion.

Negative rates are reverse, positive rates are forward.

If you want exact rate (i.e. 7x will fast forward in exactly 7 times normal play),
you must use a brcm trick mode, or must use a host trick mode and decoder slow
motion based on your specific GOP structure. In this case, do not use this
function and populate the bplayback_trickmode_params structure directly.
This function does not provide exact rates nor is it intended to.
*/
bresult bplayback_get_trickmode_by_rate(
	bplayback_t p,
	int rate,	/* The rate parameter should be used with the
					BPLAYBACK_NORMALPLAY_SPEED macro in the following manner:
					rate = BPLAYBACK_NORMALPLAY_SPEED*10 = 10th fast forward speed (NOTE: this is not 10x).
					rate = BPLAYBACK_NORMALPLAY_SPEED/10 = 1/10 slow forward.
					rate = BPLAYBACK_NORMALPLAY_SPEED*-10 = 10th fast rewind speed (NOTE: this is not -10x).
					rate = BPLAYBACK_NORMALPLAY_SPEED/-10 = 1/10 slow rewind.
				*/
	bplayback_trickmode_params *trickmode			/* [out] */
	);

/**
Summary:
For use in the rate parameter of bplayback_get_trickmode_by_rate.
Description:
The value of this parameter may change in the future and should not affect
any application.
**/
#define BPLAYBACK_NORMALPLAY_SPEED 100

/*
Summary:
	Goto a particular index, relative to your current position or the beginning.
Return value:
	Actual index found
	-1 on error
*/
long bplayback_goto_index(
	bplayback_t p,
	long index_offset, /* The index_offset to go to relative to whence.
					If whence is SEEK_CUR or SEEK_END, the index_offset may be negative */
	int whence	/* can be SEEK_SET or SEEK_CUR. SEEK_END is not supported yet */
	);

/*
Summary:
	Goto a particular timestamp, relative to your current position or the beginning.
Return value:
	Actual timestamp found
	-1 on error
*/
long bplayback_goto_timestamp(
	bplayback_t p,
	long timestamp, /* The timestamp to go to relative to whence.
					If whence is SEEK_CUR or SEEK_END, the index_offset may be negative */
	int whence	/* can be SEEK_SET or SEEK_CUR. SEEK_END is not supported yet */
	);

/*
Summary:
	Goto a particular mpeg offset, relative to your current position or the beginning.
Return value:
	Actual mpeg offset of the frame found
	-1 on error
*/
off_t bplayback_goto_mpeg_file_offset(
	bplayback_t p,
	off_t mpeg_file_offset, /* The impeg_file_offset to go to relative to whence.
					If whence is SEEK_CUR or SEEK_END, the index_offset may be negative */
	int whence	/* can be SEEK_SET or SEEK_CUR. SEEK_END is not supported yet */
	);

/* End of Playback API  */


/* Record API */

/*
Summary:
	Open a record channel.
*/
brecord_t brecord_open(void);

/*
Summary:
	Open an IP record channel.
*/
brecord_t brecord_ip_open(void);

/*
Summary:
	Close a record channel.
Description:
	Record should already have been stopped before calling close.
	The brecord_t handle will be invalidated. You must call brecord_open
	to record again.
*/
void brecord_close(
	brecord_t record /* handle returned by brecord_open */
	);

/*
Summary:
	Reason code returned in brecord_params.abnormal_stop callback.
*/
typedef enum brecord_error {
	brecord_error_disk_full,	/* No more data can be written because the disk is full. */
	brecord_error_max_file_size,	/* No more data can be written because the file is
									at a maximum size supported by the filesystem. */
	brecord_error_unknown		/* No more data can be written because of a system
									error. */
} brecord_error;

/*
Summary:
	Parameters passed into brecord_start().
Description:
	This structure must be initialized by brecord_params_init().
*/
typedef struct brecord_params {
	bindex_format index_format;
	bencryption_params encryption;
	bool timestamp_enabled;	/* If true, then a 4 byte timestamp is prepended to every
								transport packet. bplayback_params.timestamp_enabled
								must match. */
	void *callback_context;	/* Context pointer which is returned in the callback functions.
								This could be the brecord_t handle, or anything you'd like. */
	void (*error)(void *context, brecord_error errorcode);	/* Called whenever the
								record thread can no longer write data because of an error.
								The thread will be exiting after this callback. */
	void (*overflow)(void *context, bool *stop_record);	/* Called whenever a buffer
								overflow is detected. Recorded data will be corrupt.
								You can chose to modify the stop_record value to tell the
								record thread to keep recording anyway, or to stop. If
								it stops now, no corrupt data will be written to disk. */
} brecord_params;

/*
Summary:
	Required initialization of brecord_params.
*/
void
brecord_params_init(
	brecord_params *params,	/* [out] */
	brecord_t record		/* required for possible resource-dependent defaults */
	);

/*
Summary:
Open a record file to be passed to brecord_start().
Description:
The brecord_file_t can be used for only one record at a time.
The files will be created if they don't already exist, and truncated if they do.
*/
brecord_file_t brecord_file_open(
	const char *mpeg_file_name, /* mpeg file name to create */
	const char *index_file_name /* Index name to create. This can be NULL but only for
								 MPEG formats that don't produce any index data. */
    );

/*
Summary:
Open a record network file to be passed to brecord_start().
Description:
The brecord_file_t can be used for only one record at a time.
The files will be created if they don't already exist, and truncated if they do.
You must pass in a params file in order to contact the network server where
the file for record is opened.
*/
brecord_file_t
brecord_file_open_network(
    const char *mpeg_file_name, /* mpeg file name to create */
	const char *index_file_name, /* Index name to create. This can be NULL but only for
								 MPEG formats that don't produce any index data. */
    const bsocket_params * params,  /* Params used to create a network record file */
     const bstream_mpeg * mpeg_info /* Stream Info that Needs to be sent to Remote Server*/
    );
	
/**
Close a file after record is stopped
**/
void brecord_file_close(
	brecord_file_t file
	);

/*
Summary:
	Start recording a stream to a file.
Description:
You can only record one stream at a time for a given brecord_t handle.
Dual record is done with two brecord_t handles.

See brecpump_start for details on all-pass recording.
*/
bresult brecord_start(
	brecord_t record, /* handle returned by brecord_open */
	brecpump_t recpump, /* record pump handle */
	bstream_t stream, /* stream to record */
	brecord_file_t file,  /* destination for the data */
	const brecord_params *params
	);

/**
Summary:
See brecpump_status for the structure definition.
**/
typedef brecpump_status brecord_status;

/* extend brecpump_status or copy it out ?? - TBD */

/*
Summary:
	Get current status of the recording.
*/
void brecord_get_status(
	brecord_t record, /* handle returned by brecord_open */
	brecord_status *status	/* [out] */
	);

/*
Summary:
	Stop recording.
Description:
	The brecord_t handle is still valid. You can either start another
	recording with brecord_start or close using brecord_close.
*/
bresult brecord_stop(
	brecord_t record /* handle returned by brecord_open */
	);

/* End of Record API  */

/*
Summary:
	Opens an on the fly pvr playback .
*/
bplayback_t botfplay_open(
	bobject_t feeder_id	 /* id used to select a playback channel */
	);


#define TS_PKT_SIZE					188
#define TS_PKTS_PER_UDP_PKT			7
#define	UDP_PAYLOAD_SIZE			TS_PKT_SIZE * TS_PKTS_PER_UDP_PKT
#define RTP_PAYLOAD_SIZE			UDP_PAYLOAD_SIZE
#define IP_HALT_TASK_TIMEOUT_MSEC	1000
#define IP_RECV_TIMEOUT_USEC		250000
#define IP_RECV_BURST_USEC			(IP_RECV_TIMEOUT_USEC/5)
#define IP_PVR_PLAYSPEED_MAX_COUNT  (128)

/*
Summary:
	This structure used to communicate initial parameters to the
	bplayback_socket_open function.
Description:
	The field "ip_addr" is a dotted decimal IP Address represented as a string or a fully qualified domain name
	The field "port" is a UDP port number
	The field "protocol" is used to select the protocol employed (eg plain UDP or RTP, etc)
	The field "security" is used to select the security employed (eg none, SSL, DTCP-IP,  )
	The Field "fd" is an output parameter
	The Field "url" is used to pass a URL as a string
*/
typedef struct bplayback_socket_open_params {
	char 			ip_addr[64]; 
	unsigned int 	port;
	bsettop_ip_protocol protocol;
	bsettop_ip_security security;
	int fd;
	char *url;
	/* program info */
	bstream_mpeg    psi;
    uint32_t   first_pts;
    unsigned   duration; /* In msec */
    unsigned num_play_speed_entries;
	int	http_frame_repeat;	/* frame repeat count: computed & sent by the HTTP Streamer server */
	int	http_min_iframe_speed; /* Server only sends I Frames at this or higher speed */
    int play_speed[IP_PVR_PLAYSPEED_MAX_COUNT];
} bplayback_socket_open_params;

typedef enum bplayback_rtsp_ctrl_cmd_type {
	bplayback_rtsp_ctrl_cmd_type_play,
	bplayback_rtsp_ctrl_cmd_type_pause,
	bplayback_rtsp_ctrl_cmd_type_resume
} bplayback_rtsp_ctrl_cmd_type;

typedef struct bplayback_rtsp_ctrl_params {
	long start;
	long end;
	long scale;
} bplayback_rtsp_ctrl_params;

typedef struct bplayback_rtsp_ctrl_cmd {
	bplayback_rtsp_ctrl_cmd_type type;
	bplayback_rtsp_ctrl_params params;
} bplayback_rtsp_ctrl_cmd;

typedef struct bplayback_lm_callbacks {
	void (*lm_get_data)(void *context, void *buffer, unsigned int *buffer_size);
	void (*lm_session_open)(void **context, bplayback_socket_open_params *socket_open_params);
	void (*lm_session_start)(void *context, bplaypump_t pump);
	void (*lm_session_rtsp_ctrl)(void *context, bplayback_rtsp_ctrl_cmd *cmd);
	void (*lm_session_stop)(void *context);
	void (*lm_session_close)(void *context);
} bplayback_lm_callbacks;

typedef struct bplayback_socket_params {
	bplayback_socket_open_params open_params;
	bplayback_lm_callbacks lm_callbacks;
} bplayback_socket_params;

/*
Summary:
	Required to initialize the bplayback_socket_params structure.
*/
void 
bplayback_socket_params_init(
	bplayback_socket_params *params, /* [out] */
	bplayback_t playback
	);

/*
Summary:
	Open a IP playback socket to pass to bplayback_start().
Description:
	The bplayback_socket_t can be used for only one IP playback at a time.
*/
bplayback_file_t bplayback_socket_open(
	bplayback_t playback, /* Handle returned by bplayback_open */
	const bplayback_socket_params *params
	);

/*
Summary:
	Close a IP playback socket.
Description:
	Close a IP playback socket opened by bplayback_socket_open().
*/
void bplayback_socket_close(
	bplayback_t playback /* Handle returned by bplayback_open */
	);


/*
Summary:
	Required to initialize the bsocket_params structure.
*/
void 
bsocket_params_init(
	bsocket_params *params /* [out] */
	);

/*
Summary:
	Open a IP record socket to pass to brecord_start().
Description:
	The record socket can be used for only one IP record at a time.
*/
bstream_t brecord_socket_open(
	brecord_t record, /* Handle returned by bplayback_open */
	const bsocket_params *params
	);

/*
Summary:
	Close a IP record socket.
Description:
	Close a IP record socket opened by brecord_socket_open().
*/
void brecord_t_socket_close(
	brecord_t record /* Handle returned by brecord_ip_open */
	);

#ifdef __cplusplus
}
#endif


#endif /* BSETTOP_PVR_H__ */

