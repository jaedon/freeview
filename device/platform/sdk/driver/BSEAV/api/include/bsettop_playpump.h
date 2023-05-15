/***************************************************************************
 *     Copyright (c) 2004-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_playpump.h $
 * $brcm_Revision: 28 $
 * $brcm_Date: 10/31/08 9:40a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/include/bsettop_playpump.h $
 * 
 * 28   10/31/08 9:40a jgarrett
 * PR 47230: Adding TTS Pacing
 * 
 * PR47230/1   10/28/08 10:37a jjordan
 * PR47230: Port TTS Pacing to Nexus
 * 
 * 27   3/5/08 3:23p vsilyaev
 * PR38347: PR 38347 : Set seperate timestamp offset for audio and video
 * 
 * PR38347/1   12/19/07 11:04a gmohile
 * PR 38347 : Set seperate timestamp offset for audio and video
 * 
 * 26   6/12/07 4:36p jgarrett
 * PR 30310: Adding bplaypump_state_display_queue_trick_mode
 * 
 * 25   4/25/07 10:45a vsilyaev
 * PR 29921: Added code to manage timestamps for navigation and trickmodes
 * 
 * 24   4/19/07 7:28p jgarrett
 * PR 30010: Add TCP support
 * 
 * 23   12/21/06 12:34p jgarrett
 * PR 26768: Merging to main branch.
 * 
 * PROD_IPSTB_REFSW_Devel/6   12/21/06 10:09a jjordan
 * PR26768: Improve RTP Header extension processing
 * 
 * PROD_IPSTB_REFSW_Devel/5   12/21/06 9:10a jjordan
 * PR26768: merge from main branch
 * 
 * PROD_IPSTB_REFSW_Devel/4   10/3/06 4:50p jjordan
 * PR24714: Add support for RTSP protocol
 * 
 * 20   8/4/06 12:46p vsilyaev
 * PR 22876: Added support for paced trickmodes to allow rate accurate
 * trick modes
 * 
 * 19   7/26/06 9:35a dlwin
 * PR22906: Merged into main-line, for: Integrate liveMedia open-source
 * for RTP and RTCP support
 * 
 * 18   7/20/06 8:43p dlwin
 * PR20685: asf drm integration
 * 
 * PR20685_MSDRM/1   6/30/06 12:59p ptimariu
 * PR20685: asf drm integration
 * 
 * 17   4/25/06 3:11p vsilyaev
 * PR 20683: Added code for the ASF trickmodes
 * 
 * 15   3/30/06 5:58p dlwin
 * PR 20221: Merge from IPSTB development.
 * 
 * PROD_IPSTB_REFSW_Devel/1   3/30/06 5:31p jjordan
 * PR20221: export overflow as status for use by playback_ip
 * 
 * 14   3/15/06 4:51p vsilyaev
 * PR20221: NetIF DMA interface
 * 
 * 13   3/14/06 6:54p dlwin
 * PR 20206: Removed B_HAS_IP use.  Now it always incude
 * 
 * 12   3/1/06 6:24p dlwin
 * PR 18981: Corrected for Merge Manager problem
 * 
 * 11   3/1/06 4:11p dlwin
 * PR18981: PR18981: fix "live playback" and add config option
 * IP_USE_LIVE_PLAYBACK
 * PR18981: prototype "live playback"
 * 
 * 10   2/3/06 3:53p vsilyaev
 * PR 18642: Update PCRlibrary to support playback of constant bitrate
 * streams
 * 
 * PR18642/1   1/27/06 4:18p vsilyaev
 * PR18642: Use special configuration mode to change offsets for the
 * constant rate streams
 * 
 * 9   11/2/05 6:19p vsilyaev
 * PR 16982: split bsettop_impl.h to bsettop_impl.h and bsettop_impl_cfg.h
 * 
 * 8   8/4/05 9:08a erickson
 * PR16504: converted playpump_params.band to
 * playpump_params.route_through_parser_band
 * 
 * 7   8/1/05 12:20p erickson
 * PR16300: added bplaypump_set and bplaypump_get
 * 
 * 6   5/5/05 11:45a jjordan
 * PR15200: Add support for IP STB STC clock steering
 * 
 * 6   5/5/05 11:44a jjordan
 * PR15200: Add support for IP STB STC clock steering
 * 
 * 6   5/5/05 11:30a jjordan
 * PR15200: Add support for IP STB STC clock steering
 * 
 * 5   5/4/05 5:23p erickson
 * PR15173: comments added to bplaypump_read_complete
 * 
 * 4   5/4/05 4:59p erickson
 * PR14576: added num_descriptors and extended comments
 * 
 * 3   4/20/05 4:55p vsilyaev
 * PR 14724: Added option to activate use of timestamps.
 * 
 * 2   3/18/05 11:25a vsilyaev
 * PR 14509: Route playback thru pidparser.
 * 
 * Irvine_BSEAVSW_Devel/BESOTFPVR/4   11/15/04 8:44p vsilyaev
 * Added band parameter.
 * 
 * Irvine_BSEAVSW_Devel/BESOTFPVR/3   11/13/04 1:56p vsilyaev
 * Separated otfpvr to backend and frontend  parts.
 * Removed play interface.
 * 
 * Irvine_BSEAVSW_Devel/BESOTFPVR/2   10/21/04 8:08p vsilyaev
 * Merged 7038 branch
 * 
 * Irvine_BSEAVSW_Devel/BESOTFPVR/BESOTFPVR_7038/1   10/18/04 6:25p vsilyaev
 * Changed bsettop_play_start to return a stream.
 * 
 * Irvine_BSEAVSW_Devel/BESOTFPVR/1   10/8/04 7:59p vsilyaev
 * Added BESPVR suport
 * 
 * Irvine_BSEAVSW_Devel/5   6/29/04 11:29a erickson
 * PR11135: updated deprecation notices for settop api
 * 
 * Irvine_BSEAVSW_Devel/4   6/24/04 10:27a erickson
 * PR11135: converted to generic bsettop_callback for playpump and recpump
 * 
 * Irvine_BSEAVSW_Devel/3   6/16/04 5:16p erickson
 * PR11135: added bsettop_stream.h include
 * 
 * Irvine_BSEAVSW_Devel/2   6/10/04 2:11p erickson
 * PR11135: continued playpump devel
 * 
 * Irvine_BSEAVSW_Devel/1   6/8/04 7:40a erickson
 * PR11135: added iniital playpump impl for 97038
 *
 *******************************************************************************/
#ifndef BSETTOP_PLAYPUMP_H__
#define BSETTOP_PLAYPUMP_H__

#include "bsettop_stream.h"
#include "bsettop_ip.h"

#ifdef __cplusplus
extern "C" {
#endif

/*=************************************************
The bplaypump API is a push-based PVR api. This means that the calling app is responsible
for reading PVR data from disk, memory or network in whatever chunks it wants. 

This differs from the bplayback API which is designed to pull data from disk. The
bplayback API is built on top of the bplaypump API.

There are several ways to interact with playpump in order to feed data:
1) don't create your own read-loop but use the read callback exclusively
2) create your own blocking read-loop and ignore the callback
3) create your own non-blocking read-loop and use the read callback to wake up the loop
	when space becomes available.

Example code exists for each above methods in the api/examples directory as
playpumpX.c. You can also refer to bsettop_playback.c for another example of 
how to use the playpump API.
***************************************************/


/**
Summary:
Parameters that must be passed into bplaypump_open.
**/
typedef struct bplaypump_open_params {
	size_t buffer_size; /* size in bytes of the playback buffer */
	unsigned alignment; /* alignment for the block. units of 2^alignment. Therefore a value of 2 will be 
		4 byte aligned, 3 will be 8 byte aligned, etc. */
	unsigned num_descriptors; /* Number of descriptors. In general, the number of descriptors should be the
		buffer_size / average read size. If you have too few, you won't be able to fill your playback buffer.
		If you have too many, you'll waste memory. Be aware that the number needed for normal playback might
		be far less than the number needed for a Broadcom Trick Mode or IP playback which requires small reads. */
    void* dcrypt_context;   /* crypto context used to call the decrypt func */
} bplaypump_open_params;

/**
Summary:
Returns default configuration parameters for bplaypump_open.
**/
void bplaypump_get_open_params(
		bobject_t id, 
		bplaypump_open_params *open_params /* [out] */
		);

/**
Summary:
Open a new playpump resource where the id refers to the physical playback channel.
**/
bplaypump_t bplaypump_open(
		bobject_t id,
		const bplaypump_open_params *open_params /* optional parameters for the playback pump */
		);

/**
Summary:
Close a playpump resource.
**/
void bplaypump_close(bplaypump_t playpump);

/**
Summary:
Params to be passed into bplaypump_start.
**/
typedef struct bplaypump_params {
	bsettop_callback read_callback; /* This callback is called whenever space becomes
		available for writing. Please see the documentation of bsettop_callback
		to learn about general rules for Settop API callbacks.
		In this callback, you may call bplaypump_get_buffer in order to schedule
		I/O. If no I/O is needed (for instance, if you're generated a BTP packet in
		memory), you can call bplaypump_read_complete and will not have any recursion. 
		See bplaypump_read_complete for more rules on using this callback. */
	void *callback_context; /* context passed into the read_callback */
	bool timestamp_enabled;	/* If true, then a 4 byte timestamp has been prepended to every
								transport packet. brecord_params.timestamp_enabled
								must match. */
	bsettop_callback error;	/* Called out of the internal playback thread whenever
										the a fatal error happens. This means that the playback is about to be stopped.
										You cannot call a settop api function from this callback. */
	bool route_through_parser_band;		/* if true, playback shall be routed through a parser band */
	bool timestamp_active; /* use timestamps  to dynamicaly control playback rate (pacing) */
	bool constant_rate; /* streams comes from a media which has a constant rate, i.e. video over IP */
	bool is_playback_ip; /* true if playpump is IP playback, false otherwise */
	bool use_live_playback_mode;
	unsigned long pacing_max_error; /* set the timestamp error bound, as used by the playback pacing logic */
} bplaypump_params;

/**
Summary:
Required to set bplaypump_params to an initial state.
**/
void bplaypump_params_init(
	bplaypump_params *params, /* [out] */
	bplaypump_t playpump
	);

/**
Summary:
Starts playback and returns a stream which can be decoded or recorded.
**/
bstream_t bplaypump_start(
	bplaypump_t playpump,
	const bstream_mpeg *mpeg, /* MPEG settings for the source.
								These are required by playpump for for decryption.
								They are also embedded in the returned bstream_t
								for use by bdecode. */
	const bplaypump_params *params
	);

/**
Summary:
Stop playback of the stream.
Description:
The bstream_t handle returned by bplaypump_start is no longer valid after
calling this.
**/
bresult bplaypump_stop(
	bplaypump_t playpump
	);

/**
Summary:
Get buffer space for the next read.

Description:
If block is false and no space is available, size will be 0 and
buffer will be NULL.
If block is true and no space is available, the function will block until space
becomes available or there is an error.

If you make a non-blocking call and no space is available, the read_callback will
be called when space becomes available.

This is a non-destructive call. This means you can call this function as many times
as you want and it won't consume any data or change any state.
**/
bresult bplaypump_get_buffer(
	bplaypump_t playpump,
	void **buffer, /* [out] Get buffer space for the next read. */
	size_t *size /* [out] The size of buffer space available for the next read. */
	);

/**
Summary:
Report back how much of the buffer returned by bplaypump_get_buffer has been
used and should be sent to playback.

Description:
You can call bplaypump_get_buffer as much as you like, but you should only
call bplaypump_read_complete once for every buffer pointer that you actually
read data into. 

This function name was chosen from the persepective of the application. Therefore it 
is called "read" complete because your application will most likely read data
into this buffer.

After calling read_complete, it is advised that the application take the initiative
to call bplaypump_get_buffer and see if there's more data that can be sent.
playpump may not generated another read callback after read_complete has been called
(the exact behavior is implementation and platform dependent, based on whether
the callback is edge or level triggered internally.) If you do not do this, you
may have to wait until the playback buffer is empty before sending the extra
chunk of data, and this will lead to playback underflow.

There are three scenarios for using the skip value:
1. When playpump comes to the end of its circular buffer, bplaypump_get_buffer may return a
	very small size. If that size is too small for your i/o requirements, you should call
	bplaypump_read_complete with a skip equal to the entire size of the buffer and amount 
	equal to 0. The small buffer will be skipped over and your next call to bplaypump_get_buffer
	should return a large buffer at the beginning of playback.
2. If your i/o method requires a certain byte alignment (e.g. 4K page aligned), you'll need to use
	skip to offset into the buffer and start reading at the address you need.
3. If you don't have any of these requirements, then always set skip to 0 and set amount to the
	amount you are reading.
**/
bresult bplaypump_read_complete(
	bplaypump_t playpump,
	size_t skip, /* The number of bytes at the beginning of the buffer returned by bplaypump_get_buffer
						which should be ignored. 
					skip can be any value between 0 and the buffer_size.
					skip + amount must be <= buffer_size. */
	size_t amount /* The amount of data written into the buffer returned by bplaypump_get_buffer, 
						following the skip amount. It does not include the skipped amount.
					amount can be any value between 0 and the buffer_size.
					skip + amount must be <= buffer_size. */
	);

/**
Summary:
Decoder states for playback. See bplaypump_settings for usage.
**/
typedef enum bplaypump_state {
	bplaypump_state_normal, /* Host sends unmodified stream to decoder */
	bplaypump_state_host_trick_mode, /* Host manipulates the stream before sending to decoder for vsync decode */
	bplaypump_state_brcm_trick_mode, /* Host manipulates the stream, including embedded BTP's, before sending to decoder for vsync decode */
	bplaypump_state_host_paced,      /* Host manipulates the stream and the PTS's before sending to decoder for TSM decode */
	bplaypump_state_display_queue_trick_mode /* Host sends beginning of GOP for decode and post-decode reordering */
} bplaypump_state;

/**
Summary:
Special modes for the decoder. See bplaypump_settings for usage.
**/
typedef enum bplaypump_decode_mode {
	bplaypump_decode_mode_all,
	bplaypump_decode_mode_i,
	bplaypump_decode_mode_ip
} bplaypump_decode_mode;

/**
Summary:
This macro is used in bplaypump_settings.rate to specify decoder or STC based
slowmotion, pause and fastforward.
**/
#define BPLAYPUMP_NORMALPLAY_SPEED 100

/**
Summary:
Combination of decoder and host based trick mode settings.
**/
typedef struct bplaypump_settings {
	bplaypump_state state;	/* Used to enable host-based trick modes. */
	bool forward;	/* Used for host-based trick modes.
		If true, decode top and bottom fields in the forward direction. If false,
		reverse the decode of fields. No effect on progressive content. */
	unsigned decode_rate;	/* Used for decoder-based trick modes.
		This is the rate at which the decode should decode and display the content. 
		Use BPLAYPUMP_NORMALPLAY_SPEED to compute the rate.
		For instance, BPLAYPUMP_NORMALPLAY_SPEED/2 is 2x slow motion.
		BPLAYPUMP_NORMALPLAY_SPEED*2 is 2x fast forward.
		Do not assume the value of BPLAYPUMP_NORMALPLAY_SPEED because it may change in the future.
		The rate 0 is a decoder pause. */
	bplaypump_decode_mode decode_mode;	/* Special mode the decoder. Often used to skip content. */        
} bplaypump_settings;

/**
Summary:
Change the state of playpump to accomplish trick modes.

Description:
bplaypump_settings enables both decoder and host based trick modes. 
For host based trick modes, higher-level software must work in conjunction with playpump to accomplish the trick mode.
See bsettop_pvr.h and bsettop_playback.c for examples of higher-level trick mode
implementation.

When you change the bplaypump_state or direction (the forward variable), it will reset the pump such that the next
bplaypump_read function is guaranteed to be intended for the new state. The higher-level
software is resposible for also flushing its state and any queues it has.

Changing decode_rate and decode_mode will not cause any flush or reset. If only decode_rate and decode_mode are changed,
no higher-level flush or reset is required.

Use of host and decoder based trick mode settings are orthogonal. They can be combined in a variety of ways
to accomplish various trick modes.

If the decode_rate is changed, playpump will internally decide if either video decoder or STC trick modes will be used, 
depending on the context. In general STC trick modes are preferred because the audio FIFO is not cleared 
and you can resume normal play with immediate audio decode.
**/
bresult bplaypump_set(
	bplaypump_t playpump,
	const bplaypump_settings *settings
	);

/**
Summary:
Return the current settings of playpump as set by bplaypump_set.
**/
void bplaypump_get(
	bplaypump_t playpump,
	bplaypump_settings *settings
	);

/**
Summary:
This function is deprecated and may be removed. See bplaypump_set.
**/
bresult bplaypump_set_state(
	bplaypump_t playpump,
	bplaypump_state state,
	bool forward
	);

/**
Summary:
This function is deprecated and may be removed. See bplaypump_set.
**/
bresult bplaypump_set_decoder_rate(
	bplaypump_t playpump,
	unsigned rate
	);

/**
Summary:
Tell the decoder to advance one frame from a paused state.

Description:
You must be already in a paused state. This is done by setting bplaypump_settings.rate = 0.

Based on the context of the pause, you will be in a video decoder pause,
or an STC (system time clock) pause. Based on this state, this function will perform
an video decoder or STC frame advance.

This function cannot perform a frame reverse.
**/
bresult bplaypump_frame_advance(
	bplaypump_t playpump
	);

/**
Summary:
A simple flush, apart from a change of state, is needed for jumps.
**/
bresult bplaypump_flush(
	bplaypump_t playpump
	);

/**
Summary:
Status information which can be obtained from bplaypump_get_status.
**/
typedef struct bplaypump_status {
	unsigned long fifo_depth;			/* Depth in bytes of the playback buffer */
	unsigned long fifo_size;			/* Size in bytes of the playback buffer */
	void *buffer_base;		/* Pointer to the base of the playback buffer.
				 	   This can be used for calculating your exact position
					   in the buffer for alignment considerations. */
	uint64_t bytes_played;		/* Total number of bytes played since starting */
	bool running;
	unsigned noverflows;	/* for use by playback_ip to detect overflows in playpump_ip */
} bplaypump_status;

/**
Summary:
Get status of playpump and current transport playback buffer levels.
**/
bresult bplaypump_get_status(
	bplaypump_t p,
	bplaypump_status *status /* [out] current status information returned here */
	);

/**
Summary:
Sets the PWM value for the decoder clock associated with this playpump
**/
bresult bplaypump_p_set_pwm_value(
	bplaypump_t p, 
	int pwm_value);

typedef int bsettop_socket;

typedef enum bplaypump_source_type  {
	bplaypump_source_none=0, /* normal mode, data shalll be written into  the buffer in a sequential order */
	bplaypump_source_normal=0, /* normal mode, data shalll be written into  the buffer in a sequential order */
	bplaypump_source_segmented, /* segemented mode, data shall be fed in the format <desc> <data>,
								   where desc is structure of bplaypump_chunk_desc type and 
								   data is payload of variable size (size specified in the desc),
								   application shall ensure that desc doesn't occupyes continious region of memory 
								   */   
	bplaypump_source_socket /* playpump reads data from the IP/UDP socket */
}bplaypump_source_type;

#define BPLAYPUMP_SEGMENT_SIGNATURE ((((uint32_t)'B')<<24) | (((uint32_t)'P')<<16 ) | (((uint32_t)'L')<<8) | 0)
#define BPLAYPUMP_SEGMENT_STREAMS	2
typedef struct bplaypump_segment_desc {
	  uint32_t length; /* size of the segment, including  length field */
	  uint64_t offset; /* 64 bit offset in the source file */   
	  uint32_t timestamp; /* timestamp for this segment */
	  struct {   
		    uint16_t  stream_id;
		    int16_t timestamp_delta;
	  } timestamp_delta[9];	  
	  uint32_t signature; /* this field shall be set to BPLAYPUMP_SEGMENT_SIGNATURE */
}bplaypump_segment_desc;

typedef struct bplaypump_source {
	bplaypump_source_type source_type; /* source type */
	bsettop_socket socket; /* is source_type == bplaypump_source_socket this shall be valid IP/UDP socket */
	bsettop_ip_protocol ip_protocol;
	uint32_t remote_addr;
	uint32_t remote_port;
	unsigned local_addr;
} bplaypump_source;

void bplaypump_get_source(bplaypump_t playpump, bplaypump_source *source);
bresult bplaypump_set_source(bplaypump_t playpump, const bplaypump_source *source);


/**
Summary:
This macro specifies a default RTP header size for skipping when passing a
RTP packet payload the playback hardware
 **/
#define RTP_FIXED_HEADER_SIZE		12

/**
Summary:
This macro specifies the maximum number of RTP headers that can be returned
by a call to bplaypump_get_rtp_hdr_data()
 **/
#define B_MAX_RTP_HDRS				4000

/**
Summary:
This struct defines the fields of the RTP header
 **/
typedef struct bplaypump_rtp_header {
	uint8_t version;
	uint8_t padding;
	bool extension;
	uint8_t csrc_count;
	uint8_t marker;
	uint8_t payload_type;
	uint16_t sequence_num;
	uint32_t timestamp;
	uint32_t ssrc;

	struct {
		uint16_t defined_by_profile;
		uint16_t length;
	} extension_header;
} bplaypump_rtp_header;

/**
Summary:
This struct defines the fields of the fixed RTP header
 **/
typedef struct bplaypump_rtp_hdr_data_t {
	void *data; /* pointer to RTP header reception data (used by RTCP/RTSP) */
	unsigned entry_cnt; /* number of headers received */
} bplaypump_rtp_hdr_data_t;

/**
Summary:
Get RTP header data.
The entry_cnt output parameter specifies how many headers are returned by the 
rtp_header_data pointer
**/
bresult bplaypump_get_rtp_hdr_data(bplaypump_t p, const bplaypump_rtp_hdr_data_t *rtp_header_data, unsigned *entry_cnt);

#ifdef __cplusplus
}
#endif

#endif /* BSETTOP_PLAYPUMP_H__ */
