/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bavi_stream.c $
 * $brcm_Revision: 91 $
 * $brcm_Date: 3/5/12 12:32p $
 *
 * Module Description:
 *
 * AVI parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/avi/bavi_stream.c $
 * 
 * 91   3/5/12 12:32p vsilyaev
 * SW7425-2179: Added support for QT MJPEG types A and B
 * 
 * 90   2/24/12 1:24p vsilyaev
 * SW7425-2179: Fixed memory leak
 * 
 * 89   2/23/12 7:53p vsilyaev
 * SW7425-2179: Added MJPEG support
 * 
 * 88   9/29/11 10:26a vsilyaev
 * SW7403-210, FWAVD-169: Insert special DTS only for Dx3.11 streams
 * 
 * 87   7/6/11 11:58a vsilyaev
 * SWDTV-7764, SWDTV-4772: Fixed typo that affects handling of ADPCM audio
 * 
 * 86   12/27/10 1:35p vsilyaev
 * SW35230-2589: Flush PCM data on end-of-stream
 * 
 * 85   12/20/10 11:03a vsilyaev
 * SW35230-2589: Fixed warning
 * 
 * 84   12/17/10 2:40p vsilyaev
 * SW35230-2589: Flush accumulated PCM data on end of AVI stream
 * 
 * 83   12/6/10 6:23p vsilyaev
 * SW35230-1953: Use better way to handle ADPCM audio
 * 
 * 82   10/27/10 2:35p vsilyaev
 * SW35230-1952: Added support for MS ADPCM audio
 * 
 * 81   6/18/10 4:39p vsilyaev
 * SW3548-2988: Verify bounds for allocated track
 * 
 * 80   5/14/10 3:57p vsilyaev
 * SW3548-2918: Added range limit into the bmedia_video_scan_scode
 * function
 * 
 * 79   1/21/10 5:56p vsilyaev
 * SW3556-1003: Added option to disable timestamp reordering inside bmedia
 * 
 * 78   12/22/09 1:20p vsilyaev
 * SW7405-3646: replaced bmedia_probe_es_scan_scode with
 * bmedia_video_scan_scode
 * 
 * 77   12/22/09 1:12p vsilyaev
 * SW7405-3646: Resend first frame after each seek and/or changes in the
 * direction.
 * 
 * fix_bdvd_v3.0_issue17654/1   12/21/09 8:22p gooslin
 * When a preframe received for MP4p2 attempt to detect the sequence
 * header and forward it in the PES stream
 * 
 * 76   10/30/09 1:30p gmohile
 * SW7405-3330 : Add support to extract auxiliary stream
 * 
 * 75   10/20/09 10:20a vsilyaev
 * SW7405-3217: Fixed typo
 * 
 * 74   10/16/09 1:10p vsilyaev
 * SW7405-3217: VC1 support in AVI streams
 * 
 * fix_bdvd_v2.0_pr16643/2   10/15/09 4:29p gooslin
 * Seperate divx3 and vc1 atom users
 * 
 * fix_bdvd_v2.0_pr16643/1   10/14/09 3:41p gooslin
 * Add VC1 support to AVI
 * 
 * 73   9/2/09 3:35p vsilyaev
 * SW7405-1399: Align packets not on 4 byte boundary but on a
 * sample_size*channels boundary
 * 
 * 72   7/16/09 3:31p vsilyaev
 * PR 55653: Improved handling of WAVFORMAT object
 * 
 * 71   7/15/09 2:57p vsilyaev
 * PR 46190: Fixed use of stale data accross stream flush
 * 
 * 70   6/10/09 12:44p vsilyaev
 * PR 55896: Merged changes for extended ASF codec support
 * 
 * dev_bdvd_v2.1_extend_asf_codec_support/1   6/5/09 4:42p gooslin
 * Use meta_header flag to indicate a meta data packet
 * 
 * 69   5/15/09 11:22a vsilyaev
 * PR 55142: Fixed use of VC1/WMV codecs, separated VC1_SM/VC_AP and rest
 * of WMV
 * 
 * 68   5/15/09 10:56a vsilyaev
 * PR 55142: Added MP43 video support for ASF files
 * 
 * dev_bdvd_v1.5_add_mp4v3_support_to_asf/1   5/13/09 5:55p gooslin
 * Move DivX vectors to a more generic location
 * 
 * 67   5/6/09 1:11p vsilyaev
 * PR 53319: Fixed values that is used for 'no data'
 * 
 * 66   4/20/09 11:10a gmohile
 * PR 53319 : Re-enable user data insertion
 * 
 * 65   4/16/09 6:26p vsilyaev
 * PR 53319: Disabled user data insertion, since decoder chokes on it
 * 
 * 64   4/7/09 6:35p vsilyaev
 * PR 53319: Added user data insertion for the divx FOURCC's
 * 
 * 62   3/26/09 12:46p vsilyaev
 * PR 53617: Merged support for large WaveFormatEx
 * 
 * fix_bdvd_v2.0_pr13769/2   3/20/09 3:18p gooslin
 * Force WMA and PCM WaveFormatEx struct to have a maximum size
 * 
 * fix_bdvd_v2.0_pr13769/1   3/18/09 6:54p gooslin
 * Remove old code to send only the first WMA PTS. This is now handled
 * through bmedia_pes
 * 
 * 61   3/16/09 6:29p vsilyaev
 * PR 53281: Make option to sent PTS once per stream and stream type
 * agnostic
 * 
 * 61   3/16/09 6:29p vsilyaev
 * PR 53281: Make option to sent PTS once per stream and stream type
 * agnostic
 * 
 * 60   3/16/09 5:44p vsilyaev
 * PR 53281: Added option to send PTS only once
 * 
 * 59   3/5/09 7:02p vsilyaev
 * PR 52434: Unify location of frame start codes
 * 
 * 58   3/2/09 3:05p vsilyaev
 * PR 52434: Post merge updates/unification of parsing video data
 * 
 * fix_bdvd_v2.0_pr13666/1   3/15/09 3:17p gooslin
 * Allow sending only the first PTS for AVC streams
 * 
 * 59   3/5/09 7:02p vsilyaev
 * PR 52434: Unify location of frame start codes
 * 
 * 58   3/2/09 3:05p vsilyaev
 * PR 52434: Post merge updates/unification of parsing video data
 * 
 * 57   3/2/09 10:26a vsilyaev
 * PR 52434: Merge XViD and VC1 AP support in MKV
 * 
 * fix_bdvd_v1.42_pr12986/3   2/20/09 5:47p gooslin
 * Use bmpeg4 versio of picture detection
 * 
 * fix_bdvd_v1.42_pr12986/2   2/18/09 11:47a gooslin
 * Move read bitmapinfo/waveformatex calls to bmedia scope
 * 
 * fix_bdvd_v1.42_pr12986/1   2/18/09 11:42a gooslin
 * Move FourCC defines to bmedia scope
 * 
 * 56   2/10/09 12:18p vsilyaev
 * PR 46190: Fixed timesamp calculations for PCM audio
 * 
 * 55   1/14/09 2:30p vsilyaev
 * PR 51102: Added 3ivx FOURCC
 * 
 * 54   1/14/09 1:12p vsilyaev
 * PR 46190: Fixed ES formating for PCM audio
 * 
 * 53   1/14/09 10:03a vsilyaev
 * PR 50813: Merged fix for WMA audio
 * 
 * dev_bdvd_v1.42_send_only_first_wma_timestamp_in_avi/1   1/5/09 10:58a gooslin
 * Send only the first WMA PTS to allow the decoder to interpolate all
 * following PTS values.  This will allow support for VBR WMA streams
 * 
 * dev_bdvd_v2.0_pr12502/1   1/8/09 2:36p gooslin
 * Add 3ivx support for AVI
 * 
 * 52   12/18/08 10:42a vsilyaev
 * PR 46190: Adding support for PCM audio
 * 
 * 51   11/5/08 4:59p vsilyaev
 * PR 25701: PR11069_DVD: Added test for XVID streams labeled as DIVX5
 * 
 * 50   11/4/08 9:57a vsilyaev
 * PR 48608: Merged code that alligns audio/video streams after seek.
 * 
 * bdvd_v1.4/int_bdvd_v1.4_pr11214/bdvd_v1.42/fix_bdvd_v1.42_pr11402/1   11/1/08 7:45p gooslin
 * Cleanup A/V sync modifications
 * 
 * bdvd_v1.4/int_bdvd_v1.4_pr11214/1   10/18/08 5:42p gooslin
 * Update to latest BSEAV media code
 * 
 * bdvd_v1.4/1   9/25/08 2:50p gooslin
 * PR_10564 [ &nbsp; ].  Clean up ASF A/V sync code
 * 
 * fix_bdvd_v1.4_pr10564/1   9/24/08 8:28p gooslin
 * Add sync support for AVI
 * 
 * 49   9/29/08 2:16p gmohile
 * PR 46573 : Support packed xvid streams
 * 
 * 48   9/29/08 9:17a gmohile
 * PR 44712 : Send 2 eos packets
 * 
 * 47   9/26/08 3:13p gmohile
 * PR 44712 : send 2 eos packets
 * 
 * 46   9/15/08 1:49p vsilyaev
 * PR 46175: Remove accumulated data on bavi_stream_deactivate
 * 
 * 45   9/15/08 1:45p vsilyaev
 * PR 46175: Handle bavi_demux_flush without attached consumer
 * 
 * 44   9/12/08 12:39p vsilyaev
 * PR 46175: Added code to handle case when AVI frame is larger then PVR
 * FIFO size
 * 
 * 43   9/10/08 12:37p vsilyaev
 * PR 43477: PR10623_DVD: Increased number of streams to support 8 audio,
 * 8 subtitiles and 1 video
 * 
 * 42   9/3/08 2:48p vsilyaev
 * PR 46412: Use special PTS handling for H.264 streams
 * 
 * 41   9/2/08 4:23p vsilyaev
 * PR 46412: Added support for probing OpenDML AVI 2.0 files
 * 
 * 40   8/8/08 11:59a vsilyaev
 * PR 45314: Merged fix for sendin EOS for WMA audio  in AVI files
 * 
 * 39   7/31/08 1:10p vsilyaev
 * PR 45314: Merged WMA audio support in AVI files
 * 
 * dev_bdvd_pr9870/4   7/31/08 2:39p gooslin
 * Move meta data vector destroy
 * 
 * dev_bdvd_pr9870/3   7/31/08 11:52a gooslin
 * Cleanup WMA audio changes
 * 
 * dev_bdvd_pr9870/2   7/30/08 5:32p gooslin
 * Support multiple WMA chunks in one AVI data block
 * 
 * dev_bdvd_pr9870/1   7/29/08 4:22p gooslin
 * Add support for WMA streams in AVI files
 * 
 * 38   6/16/08 2:15p vsilyaev
 * PR 39944: Fixed reordering on stream discontinuities
 * 
 * 37   6/12/08 4:17p gmohile
 * PR 39944 : Fix getting of pic type
 * 
 * 36   6/12/08 3:07p vsilyaev
 * PR 43607: Renamed alloc and free to bmem_alloc and bmem_free
 * 
 * 35   5/27/08 10:44a gmohile
 * PR 39944 : Add support for Xvid B frame PTS reordering
 * 
 * 34   5/16/08 10:42a vsilyaev
 * PR 42379: Unified timestamp calculation between avi_player and
 * avi_stream. Added heuristics in timestamp calculation to pass quirks
 * in certain streams.
 * 
 * 33   5/13/08 5:57p vsilyaev
 * PR 42119: Unified capturing of atom size
 * 
 * 32   5/7/08 5:05p vsilyaev
 * PR 42379: Fixed typo
 * 
 * 31   5/7/08 5:00p vsilyaev
 * PR 42379: Added more debug output for audio streams
 * 
 * 30   3/6/08 11:23a gmohile
 * PR 38347 : Set seperate timestamp offset for audio and video
 * 
 * 29   7/24/07 10:19a vsilyaev
 * PR 32813: Fixed memory leaks
 * 
 * 28   7/17/07 4:30p vsilyaev
 * PR 32813: Added MPEG-2 TS support for the probe library
 * 
 * 27   5/24/07 10:28a vsilyaev
 * PR 29815: Added function batom_empty to create empty atom (e.g. without
 * payload)
 * 
 * 26   5/23/07 12:38p vsilyaev
 * PR 29921: Fixed handling of audio VBR streams
 * 
 * 25   5/21/07 6:04p vsilyaev
 * PR 29815: Use common code to handle insertion of EOS and filling data
 * 
 * 24   4/25/07 6:31p vsilyaev
 * PR 29815: Added code to insert EOS sequence at end of the PES stream
 * 
 * 23   4/24/07 4:42p vsilyaev
 * PR 29921: Added timestamp scaler
 * 
 * 22   4/19/07 2:25p vsilyaev
 * PR 30009: Capture both xxdb and xxdc objects for video stream
 * 
 * 21   4/11/07 4:18p vsilyaev
 * PR 28559: Added h264 FOURCC
 * 
 * 20   3/27/07 6:41p vsilyaev
 * PR 25701: Fixed typo
 * 
 * 19   3/21/07 1:02p vsilyaev
 * PR 28559: Commented out  AVC/H.264 FOURCC codes
 * 
 * 18   3/21/07 10:41a vsilyaev
 * PR 28559: Added handling for AVC FOURCC codes
 * 
 * 17   3/20/07 7:22p vsilyaev
 * PR 25701: Added XVID FOURCC tag
 * 
 * 16   3/20/07 5:56p vsilyaev
 * PR 25701: Improved handling of stream wrap-around
 * 
 * 15   3/20/07 12:06p vsilyaev
 * PR 25701: Fixed invalid header size for audio
 * 
 * 14   3/9/07 2:28p vsilyaev
 * PR 25701: Fixed size of DIVX311 PES packets
 * 
 * 13   3/2/07 5:30p vsilyaev
 * PR 25701: For the CBR audio streams insert PTS in every 30th frame
 * 
 * 12   3/1/07 7:08p vsilyaev
 * PR 25701: Added flush function
 * 
 * 11   2/28/07 1:11p vsilyaev
 * PR 25701: Added forward and backward seek
 * 
 * 10   2/22/07 5:20p vsilyaev
 * PR 25701: Added code to handle resource starvation
 * 
 * 9   2/15/07 3:11p vsilyaev
 * PR 25701: Added code to handle parsing during resource starvation
 * 
 * 8   2/14/07 5:44p vsilyaev
 * PR 25701: Split packets for video and audio
 * 
 * 7   2/13/07 7:02p vsilyaev
 * PR 25701: Added support for DivX3.11
 * 
 * 6   2/12/07 6:14p vsilyaev
 * PR 25701: Fixed timestamps for audio
 * 
 * 5   2/12/07 2:52p vsilyaev
 * PR 25701: Added audio support
 * 
 * 4   2/9/07 7:30p vsilyaev
 * PR 25701: Added code to output PES packets
 * 
 * 3   2/9/07 1:11p vsilyaev
 * PR 25701: Added stream and format specific hanlder
 * 
 * 2   2/9/07 11:16a vsilyaev
 * PR 25701: Renamed bavi_t to bavi_parser_t
 * 
 * 1   2/8/07 11:44p vsilyaev
 * PR 25701: bavi_stream parser
 * 
 * 
 *******************************************************************************/

#include "bstd.h"
#include "bavi_stream.h"
#include "blst_squeue.h"
#include "bmedia_util.h"
#include "bmpeg4_util.h"
#include "biobits.h"
#include "bkni.h"

BDBG_MODULE(bavi_stream);

#define BDBG_MSG_TRACE(x)	BDBG_MSG(x)

BDBG_OBJECT_ID(bavi_demux_t);

typedef struct b_avi_demux_object_handler {
	bavi_parser_handler handler; /* must be first member */
	bavi_demux_t demux;
} b_avi_demux_object_handler;

struct bavi_demux {
	BLST_SQ_HEAD(b_avi_streams, bavi_stream) streams;
	bavi_parser_t parser;
	batom_factory_t factory;
	balloc_iface_t alloc;
	bavi_demux_cfg cfg;
	bavi_file_status status;
	b_avi_demux_object_handler hdrl;
	b_avi_demux_object_handler strl;
	struct {
		unsigned stream_no; /* while parsing header, current stream number */
		bool delayed;
		uint32_t timestamp_offset; /* offset, in ms from the beginning of the stream */
		bmedia_time_scale time_scale; /* scale coefficient that is used in conversion from timestamps (in ms) to PTS */
	} state;
	batom_accum_t acc;
	BDBG_OBJECT(bavi_demux_t)
};


BDBG_OBJECT_ID(bavi_stream_t);

typedef enum b_avi_stream_type {
	b_avi_stream_type_audio,
	b_avi_stream_type_video_divx3,
	b_avi_stream_type_video_vc1,
	b_avi_stream_type_video_mjpeg,
	b_avi_stream_type_other
} b_avi_stream_type;

struct b_avi_stream_handler {
	bavi_parser_handler handler; /* must be first member */
	bavi_fourcc fourcc;
	bavi_stream_t stream;
};


#define B_AVI_STREAM_VEC_FRAME	0
#define B_AVI_STREAM_VEC_EOS	1

struct bavi_stream {
    struct b_avi_stream_handler handlers[4];
    batom_accum_t accum;
    bavi_demux_t demux;
    unsigned frame_no; /* current frame number */
    size_t data_len;
    batom_pipe_t pipe; /* output pipe */
    bool (*stream_handler)(bavi_stream_t stream, batom_t atom);
    bool (*preframe_handler)(bavi_stream_t stream, batom_t atom);
    bool  parsed; /* set to true if the stream information was parsed */
    bool  active;  /* if stream is actively parsed */
    BLST_SQ_ENTRY(bavi_stream) link; /* field that is used to link streams together */
    struct bavi_stream_status status;
    b_avi_stream_type stream_type;
    struct { /* members of this structure are used for incremental feeding (it become necessary when resources are exhausted */
          batom_pipe_t acc_pipe; /* temporary PIPE for atom's that can't be processed */
    } step;
    union {
          struct {
                bavi_audio_state state;
                size_t pcm_data_len;
                size_t pcm_packet_size;
          } audio;
          struct {
                batom_t atom_seq;
          } video_divx3;
          struct {
                batom_t atom_seq;
          } video_vc1;
          struct {
              bool has_bframes;
              bool send_pts;
          } video_h264;
          struct {
                batom_t jpeg_header;
          } video_mjpeg;
    } u;
    const batom_vec *vecs[2];
    batom_vec meta_data_vec;
    uint32_t timestamp_offset;
    struct { /* reordering queue */
          batom_pipe_t pipe; /* pipe that is used to reorder PTS'es */
          uint32_t last_pts; /* PTS of last frame added into queue */
          bmedia_video_pic_type last_pic_type; /* Picture type of last frame added into queue */
    } queue;
    bool unpacked; /* Indicated packed on unpacked bit stream for Xvid */
    bavi_stream_config stream_config;    
    BDBG_OBJECT(bavi_stream_t)
};

static void b_avi_demux_reset_stream(bavi_demux_t demux, bavi_stream_t stream);
static void b_avi_stream_free(bavi_stream_t stream);

#define B_AVI_DEFAULT_STREAMS	2
#define B_AVI_MAX_PES_HDR_SIZE	64
void
bavi_demux_default_cfg(bavi_demux_cfg *cfg)
{
	cfg->user_cntx = NULL;
	cfg->pes_hdr_size = B_AVI_MAX_PES_HDR_SIZE;
	cfg->max_streams = 32;
	cfg->preallocated_streams = B_AVI_DEFAULT_STREAMS;
	return;
}

static bool
b_avi_stream_handler_discard(bavi_stream_t stream, batom_t data)
{
	BSTD_UNUSED(stream);
	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	BDBG_ASSERT(data);
	BDBG_MSG_TRACE(("b_avi_stream_handler_discard: %#lx %s %u", (unsigned long)stream, stream->status.stream_type==bavi_stream_type_video?"video":(stream->status.stream_type==bavi_stream_type_audio?"audio":"unknown"), batom_len(data)));

	batom_release(data);
	return true;
}

static bool 
b_avi_stream_auxillary(bavi_stream_t stream, batom_t data)
{
	bmedia_packet_header hdr;
	batom_t pes_atom;

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	BDBG_ASSERT(data);
	BDBG_MSG_TRACE(("b_avi_stream_auxillary: %#lx %u", (unsigned long)stream, batom_len(data)));
	BMEDIA_PACKET_HEADER_INIT(&hdr);

	pes_atom = batom_clone(data, bmedia_atom, &hdr);	
	if(!pes_atom) {
		return false;
	}
	batom_release(data);
	batom_pipe_push(stream->pipe, pes_atom);
	return true;
}

static void
b_avi_stream_clear(bavi_stream_t stream)
{
	stream->status.stream_type =  bavi_stream_type_invalid;
	stream->active = false;
	stream->parsed = false;
	stream->frame_no = 0;
	stream->data_len = 0;
	stream->handlers[0].fourcc = 0;
	stream->handlers[0].stream = stream;
	stream->handlers[1].fourcc = 0;
	stream->handlers[1].stream = stream;
	stream->handlers[2].fourcc = 0;
	stream->handlers[2].stream = stream;
	stream->handlers[3].fourcc = 0;
	stream->handlers[3].stream = stream;
	stream->stream_handler = b_avi_stream_handler_discard;
    stream->preframe_handler = b_avi_stream_handler_discard;
	stream->stream_type = b_avi_stream_type_other;
	stream->unpacked=true; /*For XviD we will start with the asusmption that it is unpacked bit stream*/
	stream->vecs[B_AVI_STREAM_VEC_FRAME] = &bmedia_null_vec;
	stream->vecs[B_AVI_STREAM_VEC_EOS] = &bmedia_null_vec;
	BATOM_VEC_INIT(&stream->meta_data_vec, 0, 0);

	bavi_audio_state_init(&stream->u.audio.state);
	stream->u.audio.pcm_data_len = 0;
	stream->u.audio.pcm_packet_size = 0;
	return;
}

static bavi_stream_t
b_avi_stream_create(bavi_demux_t demux)
{
	bavi_stream_t stream;

	stream = BKNI_Malloc(sizeof(*stream));
	if(!stream) {
		BDBG_ERR(("b_avi_stream_create: out of memory %u", sizeof(*stream)));
		goto err_alloc;
	}
	BDBG_OBJECT_INIT(stream, bavi_stream_t);
	stream->demux = demux;
	stream->pipe = NULL;	
    bavi_demux_default_stream_cfg(&stream->stream_config);
	stream->accum = batom_accum_create(demux->factory);
	if(!stream->accum) {
		goto err_accum;
	}
	stream->step.acc_pipe = batom_pipe_create(demux->factory);
	if(!stream->step.acc_pipe) {
		goto err_pipe;
	}
	stream->queue.last_pts=0;
	stream->queue.last_pic_type = bmedia_video_pic_type_unknown;
	stream->queue.pipe = batom_pipe_create(demux->factory);
	if(!stream->queue.pipe) {
		goto err_queue_pipe;
	}
	b_avi_stream_clear(stream);
	return stream;
err_queue_pipe:
	batom_pipe_destroy(stream->step.acc_pipe);
	batom_accum_destroy(stream->accum);
err_pipe:
err_accum:
	BKNI_Free(stream);
err_alloc:
	return NULL;
}


bavi_stream_t
bavi_demux_get_stream(bavi_demux_t demux, unsigned n)
{
	bavi_stream_t stream;
	unsigned i;

	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	for(i=0,stream=BLST_SQ_FIRST(&demux->streams);stream;i++,stream=BLST_SQ_NEXT(stream,link)) {
		BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
		if(i==n) {
			return stream;
		}
	}
	if(n>=demux->cfg.max_streams || i>=demux->cfg.max_streams) {
		return NULL;
	}
	/* allocate more streams */
	for(i--;i<n;i++) {
		stream = b_avi_stream_create(demux);
		if(!stream) {
			break;
		}
		BLST_SQ_INSERT_TAIL(&demux->streams, stream, link);
	}
	return stream;
}

static bavi_parser_action  
b_avi_demux_hdrl(bavi_parser_handler *handler, bavi_fourcc fourcc, batom_t object)
{
	bavi_demux_t demux = ((b_avi_demux_object_handler *)handler)->demux;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	BDBG_MSG(("b_avi_demux_hdrl: " BMEDIA_FOURCC_FORMAT "[" BMEDIA_FOURCC_FORMAT "] %u bytes", BMEDIA_FOURCC_ARG(handler->fourcc), BMEDIA_FOURCC_ARG(fourcc), object?batom_len(object):0));
	if (fourcc == BMEDIA_FOURCC('a','v','i','h')) {
		BDBG_ASSERT(object);
		demux->state.stream_no = 0;
		demux->status.header_valid = bavi_read_mainheader(&demux->status.mainheader, object);
	}
	if(object) {
		batom_release(object);
	}
	return bavi_parser_action_none;
}


static bavi_parser_action  
b_avi_stream_payload(bavi_parser_handler *handler, bavi_fourcc fourcc, batom_t object)
{
	bavi_stream_t stream = ((struct b_avi_stream_handler *)handler)->stream;
	BDBG_MSG_TRACE(("b_avi_stream_payload: %#lx " BMEDIA_FOURCC_FORMAT "[" BMEDIA_FOURCC_FORMAT "] %u bytes", (unsigned long)stream, BMEDIA_FOURCC_ARG(handler->fourcc), BMEDIA_FOURCC_ARG(fourcc), object?batom_len(object):0));
	if (fourcc == handler->fourcc) { /* XXX handling of LIST payloads */
		if(object) {
			size_t atom_len = batom_len(object);
			if(stream->pipe) {
				if(batom_pipe_peek(stream->step.acc_pipe)==NULL) {
                    bool consumed = (stream->timestamp_offset != B_MEDIA_INVALID_PTS) ? stream->stream_handler(stream, object) : 
                                                                                        stream->preframe_handler(stream, object);
					if(consumed) {
						stream->frame_no++; /* increment frame no */
						stream->data_len += atom_len;
						goto done;
					}
				} 
				BDBG_MSG(("b_avi_stream_payload: %#lx delay payload %#lx", (unsigned long)stream, (unsigned long)object));
				/* append object to the pipe */
				batom_pipe_push(stream->step.acc_pipe, object);
				stream->demux->state.delayed = true;
				return bavi_parser_action_return;
			} else {
				/* no pipe attached, discard object, but keep counting */
				stream->frame_no++; /* increment frame no */
				stream->data_len += atom_len;
				goto discard;
			}
		}
		goto done;
	}
discard:
	if (object) {
		batom_release(object);
	}
done:
	return bavi_parser_action_none;
}

static void
b_avi_demux_atom_free(batom_t atom, void *user)
{
	bavi_demux_t demux = *(bavi_demux_t *)user;
	const batom_vec *pes_vec;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	BDBG_MSG_TRACE(("b_avi_demux_atom_free: %#lx %#lx", (unsigned long)atom, (unsigned long)demux));
	pes_vec = batom_get_vec(atom, 0);
	BDBG_ASSERT(pes_vec);
	demux->alloc->bmem_free(demux->alloc, pes_vec->base);
	return;
}

static const batom_user b_avi_demux_atom = {
	b_avi_demux_atom_free,
	sizeof(bavi_demux_t)
};

static bool
b_avi_stream_audio(bavi_stream_t stream, batom_t data)
{
	size_t data_len;
	bmedia_packet_header hdr;
	batom_t pes_atom;
	bavi_demux_t demux;
	bmedia_player_pos timestamp;

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	demux = stream->demux;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	BDBG_ASSERT(data);
	data_len = batom_len(data);
	BDBG_MSG_TRACE(("b_avi_stream_audio: %#lx %u", (unsigned long)stream, data_len));
	BDBG_ASSERT(stream->pipe);
	BMEDIA_PACKET_HEADER_INIT(&hdr);

	timestamp = bavi_audio_state_get_timestamp(&stream->u.audio.state, stream->frame_no, stream->data_len);
	bavi_audio_state_update(&stream->u.audio.state, data_len);
	hdr.pts = bmedia_time2pts(timestamp+stream->timestamp_offset, demux->state.time_scale);
	hdr.pts_valid = true;
	hdr.key_frame = true;
	BDBG_MSG(("b_avi_stream_audio:%#lx %u audio %s:%u dataLen:%u dataLen:%u , timestamp_offset %d", (unsigned long)stream, (unsigned)timestamp, hdr.pts_valid?"pts":"no-pts", (unsigned)hdr.pts, data_len, stream->data_len, stream->timestamp_offset));
	pes_atom = batom_clone(data, bmedia_atom, &hdr);
	if(!pes_atom) {
		return false;
	}
	batom_release(data);
	batom_pipe_push(stream->pipe, pes_atom);

	return true;
}

static bool
b_avi_stream_split_audio(bavi_stream_t stream, batom_t data, uint32_t block_size, const batom_user *user, bmedia_packet_header *hdr)
{
	batom_t raw_atom;
	batom_t pes_atom;
	batom_cursor first,last;
	batom_accum_add_atom(stream->accum, data);
	batom_cursor_from_accum(&last, stream->accum);
	BATOM_CLONE(&first, &last);
	batom_cursor_skip(&last, block_size);

	for(;;) {
		raw_atom = batom_accum_extract(stream->accum, &first, &last, NULL, NULL);
		if(!raw_atom) {
			BDBG_WRN(("b_avi_stream_split_audio: %#lx:%#lx can't create raw audio atom",  (unsigned long)stream->demux, (unsigned long)stream));
			batom_accum_clear(stream->accum);
			return false;
		}
		pes_atom = batom_from_vec_and_atom(&stream->meta_data_vec, raw_atom, user, hdr);
		batom_release(raw_atom);
		if(!pes_atom) {
			BDBG_WRN(("b_avi_stream_split_audio: %#lx:%#lx can't create split audio atom",  (unsigned long)stream->demux, (unsigned long)stream));
			batom_accum_clear(stream->accum);
			return false;
		}
		batom_pipe_push(stream->pipe, pes_atom);
		hdr->pts_valid = false; /* clear PTS */
		hdr->key_frame = false;
		BATOM_CLONE(&first, &last);
		if(batom_cursor_skip(&last, block_size) != block_size) {
			BDBG_MSG_TRACE(("b_avi_stream_split_audio: %#lx:%#lx processed %u:%u bytes", (unsigned long)stream->demux, (unsigned long)stream, block_size, batom_cursor_pos(&last)));
			break;
		}
	}

	batom_accum_clear(stream->accum);
	return true;
}

static bool
b_avi_stream_audio_wma(bavi_stream_t stream, batom_t data)
{
	size_t data_len;
	bmedia_bcma_hdr hdr;
	batom_t pes_atom;
	bavi_demux_t demux;
	bmedia_player_pos timestamp;
	bmedia_waveformatex *audio;
	size_t payload_length;
	bool split_success;

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	audio = &stream->status.stream_info.audio;
	demux = stream->demux;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	BDBG_ASSERT(data);
	BDBG_ASSERT(audio);
	data_len = batom_len(data);
	BDBG_MSG_TRACE(("b_avi_stream_audio_wma: %#lx %u", (unsigned long)stream, data_len));
	BDBG_ASSERT(stream->pipe);
	BMEDIA_PACKET_HEADER_INIT(&hdr.pes);

	timestamp = bavi_audio_state_get_timestamp(&stream->u.audio.state, stream->frame_no, stream->data_len);
	bavi_audio_state_update(&stream->u.audio.state, data_len);
	hdr.pes.pts = bmedia_time2pts(timestamp+stream->timestamp_offset, demux->state.time_scale);
	hdr.pes.pts_valid = true;
	hdr.pes.key_frame = true;

	BDBG_MSG(("b_avi_stream_audio_wma:%#lx %u audio %s:%u dataLen:%u dataLen:%u , timestamp_offset %d", (unsigned long)stream, (unsigned)timestamp, hdr.pes.pts_valid?"pts":"no-pts", (unsigned)hdr.pes.pts, data_len, stream->data_len, stream->timestamp_offset));

	/* The default payload size is one audio chunk chunk */
	payload_length = audio->nBlockAlign;
	if (data_len % audio->nBlockAlign != 0) {
		/* This data block is of some strange size, send as one chunk */
		payload_length = data_len;
		BDBG_WRN(("b_avi_stream_audio_wma: %#lx invalid audio object size %u (block_size %u)", (unsigned long)demux, (unsigned)data_len, (unsigned)audio->nBlockAlign));
	}

	hdr.pes.header_off = 4;
	hdr.pes.header_type = B_AVI_STREAM_VEC_FRAME;
    bmedia_bcma_hdr_init(&hdr, payload_length);
	hdr.pes.header_len = sizeof(hdr.pkt_len);
	hdr.pkt_len[0] = B_GET_BITS(payload_length,31,24);
	hdr.pkt_len[1] = B_GET_BITS(payload_length,23,16);
	hdr.pkt_len[2] = B_GET_BITS(payload_length,15,8);
	hdr.pkt_len[3] = B_GET_BITS(payload_length,7,0);

	if (payload_length==data_len) {
		/* Add entire chunk as one atom */
		pes_atom = batom_from_vec_and_atom(&stream->meta_data_vec, data, &bmedia_bcma_atom, &hdr.pes);
		if(!pes_atom) {
			return false;
		}
		batom_pipe_push(stream->pipe, pes_atom);
	}
	else {
		/* Split chunk into multiple atoms */
		split_success = b_avi_stream_split_audio(stream, data, audio->nBlockAlign, &bmedia_bcma_atom, &hdr.pes);
		if(!split_success) {
			return false;
		}
	}

	batom_release(data);
	return true;
}

static bool
b_avi_stream_audio_pcm(bavi_stream_t stream, batom_t data)
{
    bavi_demux_t demux;
    size_t packet_size;
    size_t data_len;
    bool pts_valid;

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	demux = stream->demux;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	BDBG_ASSERT(data);
	BDBG_ASSERT(stream->pipe);
	BDBG_ASSERT(stream->accum);
    data_len = batom_len(data);
    BDBG_MSG_TRACE(("b_avi_stream_audio_pcm: %#lx %u", (unsigned long)stream, data_len));
    bavi_audio_state_update(&stream->u.audio.state, data_len);
	batom_accum_add_atom(stream->accum, data);
    batom_release(data);
    packet_size = stream->u.audio.pcm_packet_size;
    for(pts_valid=true;packet_size>0;) {
	    bmedia_bcma_hdr hdr;
        batom_t pes_atom;
        batom_t frame;
        batom_cursor first, last;
        bmedia_player_pos timestamp;

        data_len = batom_accum_len(stream->accum);
    
        BDBG_MSG_TRACE(("b_avi_stream_audio_pcm: %#lx data %u:%u", (unsigned long)stream, data_len, packet_size));
        if(data_len<packet_size) {
            break;
        }
        batom_cursor_from_accum(&first, stream->accum);
        BATOM_CLONE(&last, &first);
        data_len = batom_cursor_skip(&last, packet_size);
        BDBG_ASSERT(data_len==packet_size);
        frame = batom_accum_extract(stream->accum, &first, &last, NULL, NULL);
        batom_accum_trim(stream->accum, &last);
        if(frame) {
            timestamp = bavi_audio_state_get_timestamp(&stream->u.audio.state, stream->frame_no, stream->u.audio.pcm_data_len);
            BMEDIA_PACKET_HEADER_INIT(&hdr.pes);
            hdr.pes.pts = bmedia_time2pts(timestamp+stream->timestamp_offset, demux->state.time_scale);
            hdr.pes.pts_valid = pts_valid;
            pts_valid = false; /* send PTS only on first frame */
            hdr.pes.key_frame = true;
            BDBG_MSG(("b_avi_stream_audio_pcm:%#lx %u audio %s:%u data_len:%u:%u pcm_data_len:%u , timestamp_offset:%d", (unsigned long)stream, (unsigned)timestamp, hdr.pes.pts_valid?"pts":"no-pts", (unsigned)hdr.pes.pts, data_len, batom_len(frame), stream->u.audio.pcm_data_len, stream->timestamp_offset));

            hdr.pes.header_off = 4;
            hdr.pes.header_type = B_AVI_STREAM_VEC_FRAME;
            bmedia_bcma_hdr_init(&hdr, packet_size);

            pes_atom = batom_from_vec_and_atom(&stream->meta_data_vec, frame, &bmedia_bcma_atom, &hdr.pes);
            batom_release(frame);
            if(pes_atom) {
                batom_pipe_push(stream->pipe, pes_atom);
            }
        }
        stream->u.audio.pcm_data_len += packet_size;
    }
	return true;
}


static uint32_t
b_avi_stream_video_pts(bavi_stream_t stream)
{
	uint32_t pts;
	uint32_t timestamp;

	timestamp = (uint32_t)(((stream->frame_no + stream->status.header.dwStart)*(uint64_t)stream->status.header.dwScale*1000)/stream->status.header.dwRate) + stream->timestamp_offset;

	pts = bmedia_time2pts(timestamp, stream->demux->state.time_scale);
	BDBG_MSG(("b_avi_stream_video_pts: %#lx %d %u:%u  , timestamp_offset %d", (unsigned long)stream, (int)stream->demux->state.time_scale, (unsigned)timestamp, (unsigned)pts, stream->timestamp_offset));
	return pts;
}

static void
b_avi_stream_queue_flush(bavi_stream_t stream, bool reorder)
{
	batom_t atom;
	uint32_t temp_pts, last_pts;
	bmedia_packet_header *pes_hdr;

	BDBG_ASSERT(stream->queue.pipe);
	for(last_pts = stream->queue.last_pts;NULL!=(atom = batom_pipe_pop(stream->queue.pipe));){
		if(stream->pipe) {
			if(reorder){
				pes_hdr = batom_userdata(atom);
				BDBG_ASSERT(pes_hdr);
				if (!pes_hdr->meta_header && pes_hdr->pts_valid) {
					temp_pts = pes_hdr->pts;
					pes_hdr->pts = last_pts;
					last_pts = temp_pts;
				}
			}
			batom_pipe_push(stream->pipe, atom);
		} else {
			batom_release(atom);
		}
	}
	return;
}

static bool
b_avi_stream_video_xvid(bavi_stream_t stream, batom_t data)
{
	bmedia_packet_header hdr;
	batom_t pes_atom;
	bmedia_video_pic_type pic_type;
	size_t frame_len = batom_len(data);
	batom_cursor cursor;

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	BDBG_ASSERT(data);
	BDBG_MSG_TRACE(("b_avi_stream_video_xvid: %#lx %u", (unsigned long)stream, batom_len(data)));
	BMEDIA_PACKET_HEADER_INIT(&hdr);
	batom_cursor_from_atom(&cursor, data);
	pic_type = bmpeg4_video_read_pic_type(&cursor);	
	hdr.pts = b_avi_stream_video_pts(stream);
	hdr.pts_valid = true;
	hdr.key_frame = (pic_type == bmedia_video_pic_type_I);
	
	pes_atom = batom_clone(data, bmedia_atom, &hdr);	

	if(!pes_atom) {
		return false;
	}			

	/* XVID does not have packed P/B frames. So needs PTS reordering*/
	batom_release(data);
	
	/* Less than 8 bytes indicates a not coded P frame. This indicates possiblity 
	   of packed bit stream. No PTS re-oredering is required in this case.
	   TODO : Do we have stream with mixed packed and unpacked? Then we will need 
	   to toggle the unpacked bool, else it can be set just one time*/
	if(pic_type == bmedia_video_pic_type_P &&
	   stream->queue.last_pic_type == bmedia_video_pic_type_B && 
	   frame_len<=8){
		stream->unpacked=false;
	}

    if(stream->stream_config.reorder_timestamps) {
        if(pic_type != bmedia_video_pic_type_B){
            b_avi_stream_queue_flush(stream, stream->unpacked);								
        }
        stream->queue.last_pts = hdr.pts;
        stream->queue.last_pic_type = pic_type;
        batom_pipe_push(stream->queue.pipe, pes_atom);   
    } else {
        stream->queue.last_pts = hdr.pts;
        stream->queue.last_pic_type = pic_type;
        batom_pipe_push(stream->queue.pipe, pes_atom);   
        b_avi_stream_queue_flush(stream, false);								
    }

	return true;
}

static bool
b_avi_stream_video_mpeg4part2_preframe(bavi_stream_t stream, batom_t data)
{
	bmedia_packet_header hdr;
	batom_t pes_atom;
	batom_cursor cursor;

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	BDBG_ASSERT(data);
	BDBG_MSG_TRACE(("b_avi_stream_video_mpeg4part2_preframe: %#lx %u", (unsigned long)stream, batom_len(data)));
	BMEDIA_PACKET_HEADER_INIT(&hdr);
	batom_cursor_from_atom(&cursor, data);

    /* Currently we only handle a single preframe chunk for MP4p2 */
    if (stream->frame_no == 0) {
        uint32_t scode;
        batom_cursor start, last;
        BATOM_CLONE(&start, &cursor);

        while ((scode = bmedia_video_scan_scode(&cursor,0xFFFFFFFFul))) {
		    if ((scode&0xFF) >= 0x20 && (scode&0xFF) <= 0x2F) { /* MP4P2 sequence header */
                batom_cursor_skip(&start, batom_cursor_distance(&start, &cursor)-B_MEDIA_VIDEO_SCODE_LEN); /* Move back four bytes to include the start code */
                BATOM_CLONE(&last, &start);

                while ((scode = bmedia_video_scan_scode(&cursor,0xFFFFFFFFul))) {
		            if (scode == 0x01B6) { /* MP4P2 picture */
                        batom_cursor_skip(&last, batom_cursor_distance(&start, &cursor)-B_MEDIA_VIDEO_SCODE_LEN); /* Move back four bytes to include the start code */
                        pes_atom = batom_extract(data, &start, &last, bmedia_atom, &hdr);
                        if(!pes_atom) {
                            return false;
                        }
                        batom_pipe_push(stream->pipe, pes_atom);
                    }
                }

                /* We're out of data, break */
                break;
            }
        }
    }

	batom_release(data);
	return true;
}

static bool
b_avi_stream_video_mpeg4part2(bavi_stream_t stream, batom_t data)
{
	bmedia_packet_header hdr;
	batom_t pes_atom;
	bmedia_video_pic_type pic_type;
	batom_cursor cursor;

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	BDBG_ASSERT(data);
	BDBG_MSG_TRACE(("b_avi_stream_video_mpeg4part2: %#lx %u", (unsigned long)stream, batom_len(data)));
	BMEDIA_PACKET_HEADER_INIT(&hdr);
	batom_cursor_from_atom(&cursor, data);
	pic_type = bmpeg4_video_read_pic_type(&cursor);	
	hdr.key_frame = (pic_type == bmedia_video_pic_type_I);
	hdr.pts = b_avi_stream_video_pts(stream);
	hdr.pts_valid = true;
	pes_atom = batom_clone(data, bmedia_atom, &hdr);	
	if(!pes_atom) {
		return false;
	}
	batom_release(data);
	batom_pipe_push(stream->pipe, pes_atom);
	return true;
}

static bool
b_avi_stream_video_divx4(bavi_stream_t stream, batom_t data)
{
	batom_t atom;
	bmedia_packet_header hdr;
	bmedia_video_pic_type pic_type;
	batom_cursor cursor;

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	BDBG_ASSERT(data);
	BDBG_MSG_TRACE(("b_avi_stream_video_divx4: %#lx %u", (unsigned long)stream, batom_len(data)));
	batom_cursor_from_atom(&cursor, data);
	pic_type = bmpeg4_video_read_pic_type(&cursor);	
	BMEDIA_PACKET_HEADER_INIT(&hdr);
	hdr.key_frame = (pic_type == bmedia_video_pic_type_I);
	hdr.pts_valid = true;
	hdr.pts = b_avi_stream_video_pts(stream);

	hdr.header_type = stream->frame_no==0 ? B_AVI_STREAM_VEC_FRAME : B_MEDIA_PACKET_NO_HEADER;

	atom = batom_clone(data, bmedia_atom, &hdr);
	if(!atom) {
		return false;
	}
	batom_release(data);
	batom_pipe_push(stream->pipe, atom);
	return true;
}

static bool
b_avi_stream_video_vc1_sm(bavi_stream_t stream, batom_t data)
{
	bavi_demux_t demux;
	batom_t atom;
	bmedia_packet_header hdr;
	batom_cursor c;
	bmedia_video_pic_type pic_type;

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	demux = stream->demux;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	BDBG_ASSERT(data);
	BDBG_MSG_TRACE(("b_avi_stream_video_vc1_sm: %#lx %u", (unsigned long)stream, batom_len(data)));
	BDBG_ASSERT(stream->stream_type == b_avi_stream_type_video_vc1);

	batom_cursor_from_atom(&c, data);
	pic_type = bmedia_vc1sm_read_pic_type(&c, &stream->status.seq_hdr.vc1_sm);	

	BMEDIA_PACKET_HEADER_INIT(&hdr);
	hdr.key_frame = (pic_type == bmedia_video_pic_type_I);
	hdr.pts_valid = true;
	hdr.header_type = B_AVI_STREAM_VEC_FRAME;
	hdr.pts = b_avi_stream_video_pts(stream);
	if(hdr.pts==B_MEDIA_MARKER_PTS) {
		hdr.pts -= 1;
	}

	if(hdr.key_frame && stream->u.video_vc1.atom_seq) {
		bmedia_packet_header seq_hdr;
		BMEDIA_PACKET_HEADER_INIT(&seq_hdr);
		seq_hdr.meta_header = true;
		seq_hdr.pts = B_MEDIA_MARKER_PTS;
		seq_hdr.pts_valid = true;

		/* send a sequence header */
		atom = batom_clone(stream->u.video_vc1.atom_seq, bmedia_atom, &seq_hdr);
		if (!atom) {
			return false;
		}
		batom_pipe_push(stream->queue.pipe, atom);
	}

	atom = batom_clone(data, bmedia_atom, &hdr);
	if(!atom) {
		return false;
	}
	batom_release(data);

    if(stream->stream_config.reorder_timestamps) {
        if(pic_type != bmedia_video_pic_type_B){
            b_avi_stream_queue_flush(stream, true);
        }
        stream->queue.last_pts = hdr.pts;
        stream->queue.last_pic_type = pic_type;
        batom_pipe_push(stream->queue.pipe, atom);   
    } else {
        stream->queue.last_pts = hdr.pts;
        stream->queue.last_pic_type = pic_type;
        batom_pipe_push(stream->queue.pipe, atom);   
        b_avi_stream_queue_flush(stream, false);
    }
	return true;
}

static bool
b_avi_stream_video_vc1_ap(bavi_stream_t stream, batom_t data)
{
	bavi_demux_t demux;
	batom_t atom;
	bmedia_packet_header hdr;
	batom_cursor c;
	bmedia_video_pic_type pic_type;
	batom_checkpoint c_start;

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	demux = stream->demux;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	BDBG_ASSERT(data);
	BDBG_MSG_TRACE(("b_avi_stream_video_vc1_ap: %#lx %u", (unsigned long)stream, batom_len(data)));
	BDBG_ASSERT(stream->stream_type == b_avi_stream_type_video_vc1);

	/* Skip any empty atoms */
	if (batom_len(data) == 0) {
		batom_release(data);
		return true;
	}

	batom_cursor_from_atom(&c, data);
	BATOM_SAVE(&c, &c_start);
	pic_type = bmedia_vc1ap_read_pic_type(&c, &stream->status.seq_hdr.vc1_ap);	

	BMEDIA_PACKET_HEADER_INIT(&hdr);
	hdr.key_frame = (pic_type == bmedia_video_pic_type_I);
  	hdr.pts_valid = true;
	hdr.pts = b_avi_stream_video_pts(stream);
	batom_cursor_rollback(&c, &c_start);
	if (bmedia_video_read_scode(&c)<0) {
		hdr.header_type = B_AVI_STREAM_VEC_FRAME;
    }

	if(hdr.key_frame) {
		if (stream->u.video_vc1.atom_seq) {
			bmedia_packet_header seq_hdr;
			BMEDIA_PACKET_HEADER_INIT(&seq_hdr);
			seq_hdr.meta_header = true;
			atom = batom_clone(stream->u.video_vc1.atom_seq, bmedia_atom, &seq_hdr);
			if(!atom) {
				return false;
			}
			batom_pipe_push(stream->queue.pipe, atom);
		}
	}

	atom = batom_clone(data, bmedia_atom, &hdr);
	if(!atom) {
		return false;
	}
	batom_release(data);

    if(stream->stream_config.reorder_timestamps) {
        if(pic_type != bmedia_video_pic_type_B) {
            b_avi_stream_queue_flush(stream, true);
        }
        stream->queue.last_pts = hdr.pts;
        stream->queue.last_pic_type = pic_type;
        batom_pipe_push(stream->queue.pipe, atom);   
    } else {
        stream->queue.last_pts = hdr.pts;
        stream->queue.last_pic_type = pic_type;
        batom_pipe_push(stream->queue.pipe, atom);   
        b_avi_stream_queue_flush(stream, false);
    }
	return true;
}

static bool
b_avi_stream_video_h264(bavi_stream_t stream, batom_t data)
{
	bmedia_packet_header hdr;
	batom_t pes_atom;
    batom_cursor cursor;
    uint32_t nal;
    bool need_pts=true;

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	BDBG_ASSERT(data);
	BDBG_MSG_TRACE(("b_avi_stream_video_h264: %#lx %u", (unsigned long)stream, batom_len(data)));
	BMEDIA_PACKET_HEADER_INIT(&hdr);
    if(stream->stream_config.reorder_timestamps) {
        batom_cursor_from_atom(&cursor, data);
        nal = batom_cursor_uint32_be(&cursor);
        /* For H264 if there are any B frames in the stream, then we send PTS'e only for IDR frames */
        if((nal&0xFFFFFF00)==0x100) { 
            stream->u.video_h264.has_bframes |=  (nal&0x60)==0;
            if(stream->u.video_h264.has_bframes) {
                switch(nal&0x1F) {
                case 5: /* IDR */
                    hdr.key_frame = true; /* The decoder can only restart from IDR frames (and certin SEI frames but we ignore that for now) */
                case 6: /* SEI */
                case 7: /* SPS */
                case 8: /* PPS */
                case 9: /* AU */
                    break;
                default:
                    need_pts = false;
                    break;
                }
            }
            else {
                hdr.key_frame = true; 
            }
        }
        need_pts |= stream->frame_no==0;
        BDBG_MSG(("b_avi_stream_video_h264:%#lx  nal:%#x %s %s", (unsigned long)stream, nal, stream->u.video_h264.has_bframes?"BFRAMES":"",  need_pts?"PTS":""));
    }
    if(need_pts) {
        hdr.pts = b_avi_stream_video_pts(stream);
        hdr.pts_valid = true;
    }
		
	pes_atom = batom_clone(data, bmedia_atom, &hdr);	

	if(!pes_atom) {
		return false;
	}

	batom_release(data);
    batom_pipe_push(stream->pipe, pes_atom);

	return true;
}

static void
b_avi_stream_bmem_atom_free(batom_t atom, void *user)
{
	bavi_demux_t demux;
	balloc_iface_t alloc;
	const batom_vec *vec;
    unsigned vectors;
    unsigned i;

	BDBG_MSG_TRACE(("b_avi_stream_bmem_atom_free: %#lx %#lx", (unsigned long)atom, (unsigned long)user));
	demux = *(bavi_demux_t *)user;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	alloc = demux->alloc;
    vectors = batom_get_nvecs(atom);
    for(i=0;i<vectors;i++) {
	    vec = batom_get_vec(atom, i);
	    BDBG_ASSERT(vec);
	    alloc->bmem_free(alloc, vec->base);
    }
	return;
}

static const batom_user b_avi_stream_bmem_atom = {
	b_avi_stream_bmem_atom_free,
	sizeof(bavi_stream_t)
};

/* this function allocates resources needed to demux vc1 stream */
static int
b_avi_stream_alloc_vc1(bavi_stream_t stream)
{
	balloc_iface_t alloc;
	bavi_demux_t demux;
	void *seq;

	BDBG_MSG_TRACE(("b_avi_stream_alloc_vc1: %#lx", (unsigned long)stream));

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	demux = stream->demux;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	alloc = demux->alloc;

	seq = alloc->bmem_alloc(alloc, stream->status.seq_header_len);
	if(!seq) {
		goto err_seq;
	}
	BKNI_Memcpy(seq, stream->status.seq_header, stream->status.seq_header_len);
	stream->u.video_vc1.atom_seq = batom_from_range(demux->factory, seq, stream->status.seq_header_len, &b_avi_stream_bmem_atom, &demux);
	if(!stream->u.video_vc1.atom_seq) {
		goto err_atom;
	}
	BDBG_MSG_TRACE(("b_avi_stream_alloc_vc1: atom %#lx",stream->u.video_vc1.atom_seq));

	return 0;

err_atom:
	alloc->bmem_free(alloc, seq);
err_seq:
	stream->u.video_vc1.atom_seq = NULL;
	BDBG_ERR(("b_avi_stream_alloc_vc1: not enough resources"));
	return -1;
}

static void
b_avi_stream_free_vc1(bavi_stream_t stream)
{
	bavi_demux_t demux;

	BDBG_MSG_TRACE(("b_avi_stream_free_vc1: %#lx", (unsigned long)stream));
	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	demux = stream->demux;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	BDBG_ASSERT(stream->stream_type == b_avi_stream_type_video_vc1);
	if(stream->u.video_vc1.atom_seq) {
		BDBG_MSG_TRACE(("b_avi_stream_free_vc1 : atom %#lx",stream->u.video_vc1.atom_seq));
		batom_release(stream->u.video_vc1.atom_seq);
		stream->u.video_vc1.atom_seq = NULL;
	}
	return;
}

static void
b_avi_stream_divx3_atom_free(batom_t atom, void *user)
{
	bavi_demux_t demux;
	balloc_iface_t alloc;
	const batom_vec *vec;

	BDBG_MSG_TRACE(("b_avi_demux_divx3_seq_free: %#lx %#lx", (unsigned long)atom, (unsigned long)user));
	demux = *(bavi_demux_t *)user;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	alloc = demux->alloc;
	vec = batom_get_vec(atom, 0);
	BDBG_ASSERT(vec);
	alloc->bmem_free(alloc, vec->base);
	vec = batom_get_vec(atom, 1);
	BDBG_ASSERT(vec);
	alloc->bmem_free(alloc, vec->base);
	return;
}

static const batom_user b_avi_stream_divx3_atom = {
	b_avi_stream_divx3_atom_free,
	sizeof(bavi_stream_t)
};

/* this function allocates resources needed to demux divx3 stream */
static int
b_avi_stream_alloc_divx3(bavi_stream_t stream)
{
	balloc_iface_t alloc;
	uint8_t *hdr;
	bavi_demux_t demux;
	void *seq_1;
	void *seq_2;

	BDBG_MSG_TRACE(("b_avi_stream_alloc_divx3: %#lx", (unsigned long)stream));

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	demux = stream->demux;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	alloc = demux->alloc;

	BDBG_ASSERT(batom_accum_len(demux->acc)==0);
	/* we use two different buffers for the sequence header, this is for sole purpose of calling alloc with the same (and small size) */
	/* perhaps it would allow to use poll allocator */
	/* data allocated here would be persistent for entire duration of file playback */
	seq_1 = alloc->bmem_alloc(alloc, demux->cfg.pes_hdr_size);
	if(!seq_1) {
		goto err_seq_1;
	}
	seq_2 = alloc->bmem_alloc(alloc, demux->cfg.pes_hdr_size);
	if(!seq_2) {
		goto err_seq_2;
	}
	BDBG_ASSERT(bmedia_divx311_seq_1.len<=demux->cfg.pes_hdr_size);
	BKNI_Memcpy(seq_1, bmedia_divx311_seq_1.base, bmedia_divx311_seq_1.len);
	batom_accum_add_range(demux->acc, seq_1, bmedia_divx311_seq_1.len);
	hdr = seq_2;
	BDBG_ASSERT(stream->status.stream_type==bavi_stream_type_video);
	hdr[0] = B_GET_BITS(stream->status.stream_info.video.biWidth,11,4);
	hdr[1] = 
		B_SET_BITS("width [3..0]", B_GET_BITS(stream->status.stream_info.video.biWidth ,3,0), 7, 4) |
		B_SET_BITS("'10'", 0x02, 3, 2) |
		B_SET_BITS("height [11..10]", B_GET_BITS(stream->status.stream_info.video.biHeight,11,10), 1, 0);
	hdr[2] = B_GET_BITS(stream->status.stream_info.video.biHeight,9,2);
	hdr[3]= 
		B_SET_BITS("height [1.0]", B_GET_BITS(stream->status.stream_info.video.biHeight,1,0), 7, 6) |
		B_SET_BITS("'100000'", 0x20, 5, 0);
	BDBG_ASSERT(4+bmedia_divx311_seq_2.len<=(int)demux->cfg.pes_hdr_size);
	BKNI_Memcpy(hdr+4, bmedia_divx311_seq_2.base, bmedia_divx311_seq_2.len);
	batom_accum_add_range(demux->acc,  seq_2, bmedia_divx311_seq_2.len+4);

	stream->u.video_divx3.atom_seq = batom_from_accum(demux->acc, &b_avi_stream_divx3_atom, &demux);
	if(!stream->u.video_divx3.atom_seq) {
		goto err_atom;
	}
	BDBG_MSG_TRACE(("b_avi_stream_alloc_divx3: atom %#lx",stream->u.video_divx3.atom_seq));

	return 0;

err_atom:
	alloc->bmem_free(alloc, seq_2);
err_seq_2:
	alloc->bmem_free(alloc, seq_1);
err_seq_1:
	BDBG_ERR(("b_avi_stream_alloc_divx3: not enough resources"));
	return -1;
}

static void
b_avi_stream_free_divx3(bavi_stream_t stream)
{
	bavi_demux_t demux;

	BDBG_MSG_TRACE(("b_avi_stream_free_divx3: %#lx", (unsigned long)stream));
	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	demux = stream->demux;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	BDBG_ASSERT(stream->stream_type == b_avi_stream_type_video_divx3);
	if(stream->u.video_divx3.atom_seq) {
		BDBG_MSG_TRACE(("b_avi_stream_free_divx3 : atom %#lx",stream->u.video_divx3.atom_seq));
		batom_release(stream->u.video_divx3.atom_seq);
		stream->u.video_divx3.atom_seq = NULL;
	}
	return;
}

bool
b_avi_stream_video_divx3(bavi_stream_t stream, batom_t data)
{
	bavi_demux_t demux;
	bool key_frame = stream->frame_no==0;
	batom_t atom;
	bmedia_packet_header hdr;

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	demux = stream->demux;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	BDBG_ASSERT(data);
	BDBG_MSG_TRACE(("b_avi_stream_video_divx3: %#lx %u", (unsigned long)stream, batom_len(data)));
	BDBG_ASSERT(stream->stream_type == b_avi_stream_type_video_divx3);
	BMEDIA_PACKET_HEADER_INIT(&hdr);
	hdr.pts_valid = true;
	if(key_frame && stream->u.video_divx3.atom_seq) {
		hdr.pts = B_MEDIA_MARKER_PTS;
		hdr.meta_header = true;
		/* send a sequence header */
		atom = batom_clone(stream->u.video_divx3.atom_seq, bmedia_atom, &hdr);
		if (!atom) {
			return false;
		}
		batom_pipe_push(stream->pipe, atom);
	}
	hdr.header_type = 0;
	hdr.pts = b_avi_stream_video_pts(stream);
	hdr.meta_header = false;
	hdr.key_frame = key_frame;

	if(hdr.pts==B_MEDIA_MARKER_PTS) {
		hdr.pts -= 1;
	}
	atom = batom_clone(data, bmedia_atom, &hdr);
	if(!atom) {
		return false;
	}
	batom_release(data);
	batom_pipe_push(stream->pipe, atom);
	return true;
}

/* we can allocate only by B_AVI_MAX_PES_HDR_SIZE units, therefore would have to use multiple vectors  */
#define B_MJPEG_HDR_VECS   8
static int
b_avi_stream_alloc_mjpeg(bavi_stream_t stream)
{
	balloc_iface_t alloc;
	bavi_demux_t demux;
    size_t header_len = bmedia_jpeg_stream_header.len  + bmedia_jpeg_default_dht.len;
    unsigned i;
    unsigned offset;
    batom_vec vecs[B_MJPEG_HDR_VECS];

	BDBG_MSG_TRACE(("b_avi_stream_alloc_mjpeg: %#lx", (unsigned long)stream));

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	demux = stream->demux;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	alloc = demux->alloc;
    BDBG_ASSERT(B_AVI_MAX_PES_HDR_SIZE * B_MJPEG_HDR_VECS >= header_len);
    for(i=0;i<B_MJPEG_HDR_VECS;i++) {
        vecs[i].base = NULL;
    }
    for(offset=0,i=0;offset<header_len;i++) {
        unsigned len = header_len - offset;
	    void *header;
        BDBG_ASSERT(i<B_MJPEG_HDR_VECS);
        if(len>=B_AVI_MAX_PES_HDR_SIZE) {
            len = B_AVI_MAX_PES_HDR_SIZE;
        }
	    header = alloc->bmem_alloc(alloc, len);
        if(!header) {
            goto err_alloc;
        }
        if(offset<bmedia_jpeg_stream_header.len) {
            size_t stream_header_len = bmedia_jpeg_stream_header.len - offset;
            if(stream_header_len>B_AVI_MAX_PES_HDR_SIZE) {
                stream_header_len=B_AVI_MAX_PES_HDR_SIZE;
            }
	        BKNI_Memcpy(header, (const uint8_t *)bmedia_jpeg_stream_header.base+offset, stream_header_len);
            if(stream_header_len != B_AVI_MAX_PES_HDR_SIZE) {
	            BKNI_Memcpy((uint8_t *)header+stream_header_len, (const uint8_t *)bmedia_jpeg_default_dht.base, B_AVI_MAX_PES_HDR_SIZE-stream_header_len);
            }
        } else {
	        BKNI_Memcpy(header, (const uint8_t *)bmedia_jpeg_default_dht.base+(offset-bmedia_jpeg_stream_header.len), len);
        }
        BATOM_VEC_INIT(&vecs[i], header, len);
        offset += len;
    }
	stream->u.video_mjpeg.jpeg_header = batom_from_vectors(demux->factory, vecs, i, &b_avi_stream_bmem_atom, &demux);
	if(!stream->u.video_mjpeg.jpeg_header) {
		goto err_atom;
	}
	BDBG_MSG_TRACE(("b_avi_stream_alloc_mjpeg: atom %#lx",stream->u.video_mjpeg.jpeg_header));

	return 0;

err_atom:
err_alloc:
    for(i=0;i<B_MJPEG_HDR_VECS;i++) {
        if(vecs[i].base) {
	        alloc->bmem_free(alloc, vecs[i].base);
        }
    }
 
	stream->u.video_mjpeg.jpeg_header=NULL;
	BDBG_ERR(("b_avi_stream_alloc_mjpeg: not enough resources"));
	return -1;
}

static void
b_avi_stream_free_mjpeg(bavi_stream_t stream)
{
	bavi_demux_t demux;

	BDBG_MSG_TRACE(("b_avi_stream_free_mjpeg: %#lx", (unsigned long)stream));
	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	demux = stream->demux;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	BDBG_ASSERT(stream->stream_type == b_avi_stream_type_video_mjpeg);
	if(stream->u.video_mjpeg.jpeg_header) {
		BDBG_MSG_TRACE(("b_avi_stream_free_mjpeg: atom %#lx",stream->u.video_mjpeg.jpeg_header));
		batom_release(stream->u.video_mjpeg.jpeg_header);
		stream->u.video_mjpeg.jpeg_header=NULL;
	}
	return;
}

static bool
b_avi_stream_video_mjpeg(bavi_stream_t stream, batom_t data)
{
    bmedia_bcmv_hdr hdr;
	batom_t pes_atom, frame;
    size_t payload_length;
    batom_cursor cursor;
    batom_cursor end;
    uint16_t header_length;
    uint32_t type;
    int byte;

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	BDBG_ASSERT(data);
    payload_length = batom_len(data);
	BDBG_MSG_TRACE(("b_avi_stream_video_mjpeg: %#lx %u", (unsigned long)stream, payload_length));
    batom_accum_add_atom(stream->accum, data);
    batom_cursor_from_accum(&cursor, stream->accum);
    batom_cursor_skip(&cursor,BMEDIA_FIELD_LEN(SOI, uint16_t) + BMEDIA_FIELD_LEN(APP0, uint16_t));
    header_length = batom_cursor_uint16_be(&cursor);
    type = batom_cursor_uint32_le(&cursor);
    byte = batom_cursor_next(&cursor);
    if(byte!=0 || type!=BMEDIA_FOURCC('A','V','I','1') || header_length<5) {
        goto bad_frame;
    }
    batom_cursor_skip(&cursor, header_length-7);
    BATOM_CLONE(&end, &cursor);
    payload_length = payload_length - (header_length + 4);
    if(payload_length!=batom_cursor_skip(&end, payload_length)) {
        goto bad_frame;
    }
    frame=batom_accum_extract(stream->accum, &cursor, &end, NULL, NULL);
    batom_accum_clear(stream->accum);
    if(frame==NULL) {
        return false;
    }
    BDBG_ASSERT(batom_len(frame)==payload_length);
	BMEDIA_PACKET_HEADER_INIT(&hdr.pes);
    hdr.pes.header_type = B_AVI_STREAM_VEC_FRAME;
    BMEDIA_PES_SET_PTS(&hdr.pes, b_avi_stream_video_pts(stream));
    payload_length += bmedia_jpeg_stream_header.len  + bmedia_jpeg_default_dht.len;
    bmedia_bcmv_hdr_init(&hdr, payload_length);
    if(stream->u.video_mjpeg.jpeg_header) {
        batom_accum_add_atom(stream->accum, stream->u.video_mjpeg.jpeg_header);
    }
    batom_accum_add_atom(stream->accum, frame);
    batom_release(frame);
	pes_atom = batom_from_accum(stream->accum, &bmedia_bcmv_atom, &hdr);	
	if(!pes_atom) {
		return false;
	}
	batom_release(data);
	batom_pipe_push(stream->pipe, pes_atom);
	return true;

bad_frame:
    batom_accum_clear(stream->accum);
    batom_release(data);
    return true;
}

static bavi_parser_action  
b_avi_demux_strl(bavi_parser_handler *handler, bavi_fourcc fourcc, batom_t object)
{
	bavi_demux_t demux = ((b_avi_demux_object_handler *)handler)->demux;
	bavi_stream_t stream;
	batom_cursor c;

	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	BDBG_MSG(("b_avi_demux_strl: " BMEDIA_FOURCC_FORMAT "[" BMEDIA_FOURCC_FORMAT "] %u bytes", BMEDIA_FOURCC_ARG(handler->fourcc), BMEDIA_FOURCC_ARG(fourcc), object?batom_len(object):0));
	stream = bavi_demux_get_stream(demux, demux->state.stream_no);
	if (stream && demux->status.header_valid) {
		switch(fourcc) {
		case BAVI_FOURCC_BEGIN:
			b_avi_demux_reset_stream(demux, stream);
			break;
		case BMEDIA_FOURCC('s','t','r','h'):
			BDBG_ASSERT(object);
			if (bavi_read_streamheader(&stream->status.header, object)) {
				char strm_no[2];
			
				strm_no[0] = '0' | ((demux->state.stream_no/10)%10);
				strm_no[1] = '0' | (demux->state.stream_no%10);
				switch(stream->status.header.fccType) {
				case BMEDIA_FOURCC('a','u','d','s'):
					stream->handlers[0].fourcc = BMEDIA_FOURCC(strm_no[0], strm_no[1], 'w', 'b');
					stream->handlers[1].fourcc = 0;	
					stream->handlers[2].fourcc = 0;	
					stream->handlers[3].fourcc = 0;	
					stream->status.stream_type = bavi_stream_type_audio;
					break;
				case BMEDIA_FOURCC('v','i','d','s'):
					stream->handlers[0].fourcc = BMEDIA_FOURCC(strm_no[0], strm_no[1], 'd', 'c');
					stream->handlers[1].fourcc = BMEDIA_FOURCC(strm_no[0], strm_no[1], 'd', 'b');
					stream->handlers[2].fourcc = BMEDIA_FOURCC(strm_no[0], strm_no[1], 's', 'b');
					stream->handlers[3].fourcc = 0;
					stream->status.stream_type = bavi_stream_type_video;
					break;
				default:
					stream->status.stream_type = bavi_stream_type_unknown;
					break;
				}
			}
			break;
		case BMEDIA_FOURCC('s','t','r','f'):
			BDBG_ASSERT(object);
            b_avi_stream_free(stream);
			switch(stream->status.stream_type) {
			case bavi_stream_type_audio:
				batom_cursor_from_atom(&c,object);
				stream->parsed = bmedia_read_waveformatex(&stream->status.stream_info.audio, &c);
				if (stream->parsed) { 
                    bmedia_waveformatex *audio = &stream->status.stream_info.audio;
					BDBG_MSG(("b_avi_demux_strl: audio stream nBlockAlign:%u", audio->nBlockAlign));
					bavi_audio_state_set_header(&stream->u.audio.state, &stream->status.header, audio);
					stream->stream_type = b_avi_stream_type_audio;
					if(   BMEDIA_WAVFMTEX_AUDIO_WMA(audio) || BMEDIA_WAVFMTEX_AUDIO_WMA_PRO(audio) 
                       || BMEDIA_WAVFMTEX_AUDIO_PCM(audio) 
                       || BMEDIA_WAVFMTEX_AUDIO_ADPCM(audio)
                       ) {
						size_t max_wf_length = BMEDIA_WAVEFORMATEX_BASE_SIZE;
						uint8_t *meta_data;
						
						if (BMEDIA_WAVFMTEX_AUDIO_WMA(audio) || BMEDIA_WAVFMTEX_AUDIO_WMA_PRO(audio)) {
							max_wf_length += BMEDIA_WAVEFORMATEX_MAX_ASF_EXTENSION_SIZE;
						} else if (BMEDIA_WAVFMTEX_AUDIO_PCM(audio)) {
							max_wf_length += BMEDIA_WAVEFORMATEX_MAX_PCM_EXTENSION_SIZE;
						}
						
						meta_data = demux->alloc->bmem_alloc(demux->alloc, max_wf_length);
						if(meta_data) {
							size_t wf_length = bmedia_copy_waveformatex(audio, meta_data, max_wf_length);
							BATOM_VEC_INIT(&stream->meta_data_vec, meta_data, wf_length);
							
							stream->vecs[B_AVI_STREAM_VEC_FRAME] = &bmedia_frame_bcma;
							stream->vecs[B_AVI_STREAM_VEC_EOS] = &bmedia_null_vec;
							
							if(BMEDIA_WAVFMTEX_AUDIO_PCM(audio) || BMEDIA_WAVFMTEX_AUDIO_ADPCM(audio) ) {
                                stream->u.audio.pcm_packet_size = bmedia_waveformatex_pcm_block_size(audio);
								stream->stream_handler = b_avi_stream_audio_pcm;
								BDBG_MSG(("b_avi_demux_strl: %#lx [AD]PCM audio block_size %u", (unsigned long)stream, stream->u.audio.pcm_packet_size));
                            } else if (BMEDIA_WAVFMTEX_AUDIO_WMA(audio) || BMEDIA_WAVFMTEX_AUDIO_WMA_PRO(audio)) {
								BDBG_MSG(("b_avi_demux_strl: WMA audio"));
								stream->stream_handler = b_avi_stream_audio_wma;
							} else {
                                BDBG_ASSERT(0); /* all supportted codecs were handled previously */
                            }
						} else {
							BERR_Code rc = BERR_TRACE(BERR_OUT_OF_DEVICE_MEMORY);
							BSTD_UNUSED(rc);
						}
					} else {
						stream->stream_handler = b_avi_stream_audio;
					}
				}
				break;
			case bavi_stream_type_video:
				batom_cursor_from_atom(&c,object);
				stream->parsed = bmedia_read_bitmapinfo(&stream->status.stream_info.video, &c);
				if (stream->parsed) {
					bavi_fourcc codec=stream->status.stream_info.video.biCompression;
					
					if(BMEDIA_FOURCC_3IVX_CODEC(codec) || BMEDIA_FOURCC_XVID_CODEC(codec) || (BMEDIA_FOURCC_DIVX5_CODEC(codec) && BMEDIA_FOURCC_XVID_CODEC(stream->status.header.fccHandler))  ) {
						BDBG_MSG(("b_avi_demux_strl: XVID video codec"));
						stream->stream_handler = b_avi_stream_video_xvid;
						stream->stream_type = b_avi_stream_type_other;
						stream->vecs[B_AVI_STREAM_VEC_EOS] = &bmedia_eos_mpeg4;
					} else if(BMEDIA_FOURCC_DIVX4_CODEC(codec)) { /* this should preceed DIVX5 detection, since for the legacy reasons DIVX5 test includes DIVX4 FOURCC's */
						stream->stream_handler = b_avi_stream_video_divx4;
						stream->stream_type = b_avi_stream_type_other;
						stream->vecs[B_AVI_STREAM_VEC_FRAME] = &bmedia_divx4_seq;
						stream->vecs[B_AVI_STREAM_VEC_EOS] = &bmedia_eos_mpeg4;
					} else if(BMEDIA_FOURCC_DIVX5_CODEC(codec)) {
						BDBG_MSG(("b_avi_demux_strl: DivX5 video codec"));
						stream->stream_handler = b_avi_stream_video_mpeg4part2;
						stream->preframe_handler = b_avi_stream_video_mpeg4part2_preframe;
						stream->stream_type = b_avi_stream_type_other;
						stream->vecs[B_AVI_STREAM_VEC_EOS] = &bmedia_eos_mpeg4;
					} else if(BMEDIA_FOURCC_H264_CODEC(codec)) {
						/* AVC/H.264 experimental at the moment */
						BDBG_MSG(("b_avi_demux_strl: H.264/AVC video codec"));
						stream->stream_handler = b_avi_stream_video_h264;
						stream->stream_type = b_avi_stream_type_other;
						stream->u.video_h264.has_bframes = false;
						stream->u.video_h264.send_pts = true; /* Alsways send the first PTS */
						stream->vecs[B_AVI_STREAM_VEC_EOS] = &bmedia_eos_h264;
					} else if(BMEDIA_FOURCC_DIVX3_CODEC(codec)) {
						BDBG_MSG(("b_avi_demux_strl: DivX311 video codec"));
						stream->stream_handler = b_avi_stream_video_divx3;
						stream->stream_type = b_avi_stream_type_video_divx3;
						stream->vecs[B_AVI_STREAM_VEC_FRAME] = &bmedia_frame_mpeg4;
						stream->vecs[B_AVI_STREAM_VEC_EOS] = &bmedia_eos_mpeg4;
						if(stream->pipe) {
							b_avi_stream_alloc_divx3(stream);
						}				
					} else if(BMEDIA_FOURCC_VC1AP_CODEC(codec)) {
						batom_checkpoint chk;

						BDBG_ASSERT(stream->status.stream_info.video.biSize > 40);
						stream->status.seq_header_len = stream->status.stream_info.video.biSize - 40;

						stream->stream_handler = b_avi_stream_video_vc1_ap;
						stream->stream_type = b_avi_stream_type_video_vc1;
						stream->vecs[B_AVI_STREAM_VEC_FRAME] = &bmedia_frame_vc1;
						stream->vecs[B_AVI_STREAM_VEC_EOS] = &bmedia_eos_vc1;
						BATOM_SAVE(&c, &chk);
						if(batom_cursor_byte(&c)!=0) {
							BDBG_MSG(("WVC1/WVCA detected with nonzero first byte, ignoring first byte"));
							stream->status.seq_header_len--;
							BATOM_SAVE(&c, &chk); /* update the check point */
						}
						BDBG_ASSERT(stream->status.seq_header_len<=sizeof(stream->status.seq_header));

						batom_cursor_rollback(&c, &chk);
						bmedia_vc1ap_read_info(&stream->status.seq_hdr.vc1_ap, &c);

						batom_cursor_rollback(&c, &chk);
						batom_cursor_copy(&c, &stream->status.seq_header, stream->status.seq_header_len);

						b_avi_stream_alloc_vc1(stream);
					} else if(BMEDIA_FOURCC_VC1SM_CODEC(codec)) {
						int profile_number;
						size_t header_len;
						batom_cursor seq_hdr;


						BDBG_ASSERT(stream->status.stream_info.video.biSize > 40);
						stream->status.seq_header_len = stream->status.stream_info.video.biSize - 40;

						BATOM_CLONE(&seq_hdr, &c);
						if (stream->status.seq_header_len==5) {
							BDBG_MSG(("WMV3 detected with 5 bytes of data, ignoring last byte"));
							stream->status.seq_header_len--;
						}

						stream->stream_handler = b_avi_stream_video_vc1_sm;
						stream->stream_type = b_avi_stream_type_video_vc1;
						stream->vecs[B_AVI_STREAM_VEC_FRAME] = &bmedia_frame_vc1;
						stream->vecs[B_AVI_STREAM_VEC_EOS] = &bmedia_eos_vc1;
						header_len = 0;
						stream->status.seq_header[header_len+0] = 0x00;
						stream->status.seq_header[header_len+1] = 0x00;
						stream->status.seq_header[header_len+2] = 0x01;
						header_len +=3;
						stream->status.seq_header[header_len+0] = 0x0F;
						stream->status.seq_header[header_len+1]  = (stream->status.stream_info.video.biWidth >> 8)&0xFF;
						stream->status.seq_header[header_len+2]  = (stream->status.stream_info.video.biWidth)&0xFF;

						stream->status.seq_header[header_len+3]  = (stream->status.stream_info.video.biHeight >> 8)&0xFF;
						stream->status.seq_header[header_len+4]  = (stream->status.stream_info.video.biHeight)&0xFF;
						header_len += 5;

						bmedia_vc1sm_read_info(&stream->status.seq_hdr.vc1_sm, &seq_hdr);

						BDBG_ASSERT(stream->status.seq_header_len+header_len+7<=sizeof(stream->status.seq_header));
						batom_cursor_copy(&c, &stream->status.seq_header[header_len], stream->status.seq_header_len);
						stream->status.seq_header_len += header_len;
						/* extract profile number from header */
						profile_number = stream->status.seq_header[header_len] >> 4;
						if ((profile_number != 0) && (profile_number != 4)) {
							/* printout error message */
							BDBG_WRN(("b_asf_parse_video_stream_properties: %#lx unsupported video compression fmt (profile %d)", (unsigned long)demux, profile_number));
						}
						BKNI_Memset(&stream->status.seq_header[stream->status.seq_header_len], 0, 7);
						stream->status.seq_header_len += 7;

						if(stream->pipe) {
						    b_avi_stream_alloc_vc1(stream);
                        }

					} else if(BMEDIA_FOURCC_MJPEG_CODEC(codec)) {
						BDBG_MSG(("b_avi_demux_strl: MJPEG video codec"));
						stream->stream_handler = b_avi_stream_video_mjpeg;
	                    stream->stream_type = b_avi_stream_type_video_mjpeg;
						stream->vecs[B_AVI_STREAM_VEC_EOS] = &bmedia_eos_bcmv;
						stream->vecs[B_AVI_STREAM_VEC_FRAME] = &bmedia_frame_bcmv;
						if(stream->pipe) {
                            b_avi_stream_alloc_mjpeg(stream);
                        }
					} else {
						BDBG_WRN(("b_avi_demux_strl: unknown video codec " BMEDIA_FOURCC_FORMAT, BMEDIA_FOURCC_ARG(stream->status.stream_info.video.biCompression)));
						stream->stream_handler = b_avi_stream_auxillary;
						stream->stream_type = b_avi_stream_type_other;
						break;
					}
				}
				break;
			default:
				break;
			}
			break;
		case BAVI_FOURCC_END:
			demux->state.stream_no++;
			if(stream->parsed)  {
				unsigned i;
				switch(stream->status.stream_type) {
				case bavi_stream_type_audio:
				case bavi_stream_type_video:
					BDBG_OBJECT_ASSERT(stream->demux, bavi_demux_t);
					for(i=0;i<sizeof(stream->handlers)/sizeof(*stream->handlers);i++) {
						if(stream->handlers[i].fourcc) {
							bavi_parser_install_handler(stream->demux->parser, &stream->handlers[i].handler, stream->handlers[i].fourcc, b_avi_stream_payload); 
						}
					}
					stream->active = true;
					break;
				default:
				break;
				}
			}
			break;
		default:
			break;
		}
	}
	if(object) {
		batom_release(object);
	}
	return bavi_parser_action_none;
}


void
bavi_demux_movi_end(bavi_demux_t demux)
{
    bavi_stream_t stream;

    BDBG_MSG_TRACE(("bavi_demux_movi_end>: %#lx", (unsigned long)demux));
    BDBG_OBJECT_ASSERT(demux, bavi_demux_t);

    for(stream=BLST_SQ_FIRST(&demux->streams);stream;stream=BLST_SQ_NEXT(stream,link)) {
        batom_t atom;

        BDBG_OBJECT_ASSERT(stream, bavi_stream_t);

        if(!stream->pipe) {
            continue;
        }

        switch(stream->status.stream_type)
        {
        case bavi_stream_type_video:
            {
                bmedia_packet_header hdr;

                b_avi_stream_queue_flush(stream, stream->unpacked);
                BMEDIA_PACKET_HEADER_INIT(&hdr);
                if(stream->stream_type == b_avi_stream_type_video_divx3) {
                    BMEDIA_PES_SET_PTS(&hdr, B_MEDIA_MARKER_PTS);
                }

                hdr.header_type = B_AVI_STREAM_VEC_EOS | B_MEDIA_PACKET_FLAG_EOS;
                atom = batom_empty(demux->factory, bmedia_atom, &hdr);
                if(atom) {
                    batom_pipe_push(stream->pipe, atom);
                }
            }
            break;
        case bavi_stream_type_audio:
            {
                bmedia_bcma_hdr hdr;
                size_t payload_length;
                bmedia_waveformatex *audio = &stream->status.stream_info.audio;

                if(BMEDIA_WAVFMTEX_AUDIO_PCM(audio) || BMEDIA_WAVFMTEX_AUDIO_ADPCM(audio)) {
                    batom_t frame;
                    BMEDIA_PACKET_HEADER_INIT(&hdr.pes);
                    frame= batom_from_accum(stream->accum, NULL, NULL);
                    if(frame) {
                        payload_length = batom_len(frame);
                        if(payload_length) {
                            hdr.pes.header_off = 4;
                            hdr.pes.header_type = B_AVI_STREAM_VEC_FRAME | B_MEDIA_PACKET_FLAG_EOS;
                            
                            bmedia_bcma_hdr_init(&hdr, payload_length);
                            atom = batom_from_vec_and_atom(&stream->meta_data_vec, frame, &bmedia_bcma_atom, &hdr.pes);
                            if(atom) {
                                batom_pipe_push(stream->pipe, atom);
                            }
                        }
                        batom_release(frame);
                    }
                }
                if( BMEDIA_WAVFMTEX_AUDIO_PCM(audio) || BMEDIA_WAVFMTEX_AUDIO_ADPCM(audio) || BMEDIA_WAVFMTEX_AUDIO_WMA(audio) || BMEDIA_WAVFMTEX_AUDIO_WMA_PRO(audio)) {
                    payload_length = 10240;
                    BMEDIA_PACKET_HEADER_INIT(&hdr.pes);
                    hdr.pes.header_off = 4;
                    hdr.pes.header_type = B_AVI_STREAM_VEC_FRAME | B_MEDIA_PACKET_FLAG_EOS;
                    bmedia_bcma_hdr_init(&hdr, payload_length);

                    atom = batom_from_vector(demux->factory, &stream->meta_data_vec, &bmedia_bcma_atom, &hdr.pes);
                    if(atom) {
                        batom_pipe_push(stream->pipe, atom);
                    }
                } 
            }
            break;
        default:
            break;
        }
    }
    BDBG_MSG_TRACE(("bavi_demux_movi_end<: %#lx", (unsigned long)demux));
    return;
}


void 
bavi_demux_default_stream_cfg(bavi_stream_config *stream_config)
{
    stream_config->reorder_timestamps = true;
    return;
}

void 
bavi_demux_flush(bavi_demux_t demux)
{
	bavi_stream_t stream;

	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	demux->state.delayed = false;
	demux->state.stream_no = 0;

	for(stream=BLST_SQ_FIRST(&demux->streams);stream;stream=BLST_SQ_NEXT(stream,link)) {
		b_avi_stream_queue_flush(stream, stream->unpacked);								
		batom_pipe_flush(stream->step.acc_pipe);
		batom_accum_clear(stream->accum);
	}
	return;
}

static void
b_avi_stream_free(bavi_stream_t stream)
{
	if(stream->pipe) {
        switch(stream->stream_type) {
        case b_avi_stream_type_video_divx3:
			b_avi_stream_free_divx3(stream);
            break;
        case b_avi_stream_type_video_vc1:
			b_avi_stream_free_vc1(stream);
            break;
        case b_avi_stream_type_video_mjpeg:
			b_avi_stream_free_mjpeg(stream);
            break;
        default:
            break;
		}
	}
    return;
}

static void
b_avi_demux_reset_stream(bavi_demux_t demux, bavi_stream_t stream)
{
	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	if (stream->active) {
		unsigned i;
		for(i=0;i<sizeof(stream->handlers)/sizeof(*stream->handlers);i++) {
			if(stream->handlers[i].fourcc) {
				bavi_parser_remove_handler(demux->parser, &stream->handlers[i].handler);
			}
		}

	}
    b_avi_stream_free(stream);
	if (stream->meta_data_vec.base) {
		demux->alloc->bmem_free(demux->alloc, stream->meta_data_vec.base);
		stream->meta_data_vec.base = NULL;
	}
	batom_pipe_flush(stream->step.acc_pipe);
	stream->queue.last_pts = 0;
	batom_pipe_flush(stream->queue.pipe);
	b_avi_stream_clear(stream);
	
	stream->timestamp_offset = 0;
	return;
}


void
bavi_demux_reset(bavi_demux_t demux)
{
	bavi_stream_t stream;

	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	bavi_demux_flush(demux);
	demux->status.header_valid = false;
	demux->state.delayed = false;
	demux->state.stream_no = 0;
	demux->state.timestamp_offset = 0;
	for(stream=BLST_SQ_FIRST(&demux->streams);stream;stream=BLST_SQ_NEXT(stream,link)) {	  
		b_avi_demux_reset_stream(demux, stream);
	}
	return;
}

static void 
b_avi_demux_payload_flush(void *sink_cntx)
{
	bavi_stream_t stream;
	bavi_demux_t demux = sink_cntx;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);

	demux->state.delayed = false;
	demux->state.stream_no = 0;

	for(stream=BLST_SQ_FIRST(&demux->streams);stream;stream=BLST_SQ_NEXT(stream,link)) {
		b_avi_stream_queue_flush(stream, stream->unpacked);								
	}

	return;
}

bavi_demux_t
bavi_demux_create(bavi_parser_t parser, batom_factory_t factory, balloc_iface_t alloc, const bavi_demux_cfg *cfg)
{
	bavi_demux_t demux;
	unsigned i;
	bavi_parser_payload_sink sink;

	BDBG_ASSERT(parser);
	BDBG_ASSERT(alloc);
	BDBG_ASSERT(cfg);

	demux = BKNI_Malloc(sizeof(*demux));
	if (!demux) {
		BDBG_ERR(("bavi_demux_create: out of memory %u", sizeof(*demux)));
		goto err_alloc;
	}
	BDBG_OBJECT_INIT(demux, bavi_demux_t);
	BDBG_ASSERT(cfg->pes_hdr_size >= B_AVI_MAX_PES_HDR_SIZE);
	demux->alloc = alloc;
	demux->factory = factory;
	demux->cfg = *cfg;
	demux->parser = parser;
	BLST_SQ_INIT(&demux->streams);
	demux->acc = batom_accum_create(factory);
	if(!demux->acc) {
		goto err_accum;
	}
	for(i=0;i<cfg->preallocated_streams;i++) {
		bavi_stream_t stream = b_avi_stream_create(demux);
		if(!stream) {
			break;
		}
		BLST_SQ_INSERT_TAIL(&demux->streams, stream, link);
	}
	demux->state.time_scale = BMEDIA_TIME_SCALE_BASE;
	bavi_demux_reset(demux);
	demux->hdrl.demux = demux;
	bavi_parser_install_handler(parser, &demux->hdrl.handler, BMEDIA_FOURCC('h','d','r','l'), b_avi_demux_hdrl); 
	demux->strl.demux = demux;
	bavi_parser_install_handler(parser, &demux->strl.handler, BMEDIA_FOURCC('s','t','r','l'), b_avi_demux_strl); 

	bavi_parser_default_payload_sink(&sink);
	sink.sink_cntx = demux;
	sink.payload_flush = b_avi_demux_payload_flush;
	bavi_parser_set_payload_sink(parser, &sink);

	return demux;

err_accum:
	BKNI_Free(demux);
err_alloc:
	return NULL;
}

void
bavi_demux_destroy(bavi_demux_t demux)
{
	bavi_stream_t stream;
	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);

	bavi_parser_remove_handler(demux->parser, &demux->hdrl.handler);
	bavi_parser_remove_handler(demux->parser, &demux->strl.handler);
	bavi_demux_reset(demux);
	while(NULL!=(stream=BLST_SQ_FIRST(&demux->streams))) {
		BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
		BLST_SQ_REMOVE_HEAD(&demux->streams, link);
		batom_pipe_destroy(stream->step.acc_pipe);
		batom_pipe_destroy(stream->queue.pipe);
		batom_accum_destroy(stream->accum);
		BDBG_OBJECT_DESTROY(stream, bavi_stream_t);
		BKNI_Free(stream);
	}
	batom_accum_destroy(demux->acc);
	BDBG_OBJECT_DESTROY(demux, bavi_demux_t);
	BKNI_Free(demux);
	return;
}

/* push data through if some data was delayed */
bool
bavi_demux_step(bavi_demux_t demux)
{
	bavi_stream_t stream;
	batom_t object;

	BDBG_MSG_TRACE(("bavi_demux_step: %#lx", (unsigned long)demux));

	BDBG_OBJECT_ASSERT(demux, bavi_demux_t);
	if(!demux->state.delayed) {
		return true;
	}
	for(stream=BLST_SQ_FIRST(&demux->streams);stream;stream=BLST_SQ_NEXT(stream, link)) {
		BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
		if(!stream->pipe) {
			continue;
		}
		/* see if some data left in the accumulator */
		while(NULL!= (object = batom_pipe_peek(stream->step.acc_pipe))) {
			size_t data_len = batom_len(object);
            bool consumed = (stream->timestamp_offset != B_MEDIA_INVALID_PTS) ? stream->stream_handler(stream, object) : 
                                                                                stream->preframe_handler(stream, object);
			if(consumed) {
				batom_pipe_drop(stream->step.acc_pipe);
				stream->frame_no++; /* increment frame no */
				stream->data_len+= data_len;
			} else {
				return false;
			}
		}
	}
	demux->state.delayed = false;
	return true;
}

int
bavi_stream_activate(bavi_stream_t stream, batom_pipe_t pipe, const bavi_stream_config *stream_config)
{
	int rc=0;
	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	BDBG_ASSERT(stream->pipe==NULL);
	BDBG_ASSERT(pipe);
	stream->pipe = pipe;
    stream->stream_config = *stream_config;

	switch(stream->stream_type) {
	case b_avi_stream_type_video_divx3:
		rc = b_avi_stream_alloc_divx3(stream);
		break;
	default:
		break;
	}

	return rc;
}

void
bavi_stream_deactivate(bavi_stream_t stream)
{
	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
    b_avi_stream_free(stream);
	stream->pipe = NULL;
	batom_pipe_flush(stream->step.acc_pipe);
	b_avi_stream_queue_flush(stream, stream->unpacked);
	return;
}

void 
bavi_stream_get_status(bavi_stream_t stream, bavi_stream_status *status)
{
	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	BDBG_ASSERT(status);

	*status =  stream->status;
	return;
}

void 
bavi_demux_set_offset(bavi_stream_t stream, uint32_t timestamp)
{
	BDBG_MSG_TRACE(("bavi_demux_set_offset>: %#lx %u", (unsigned long)stream, (unsigned)timestamp));
	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);

	stream->timestamp_offset = timestamp;

	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	stream->frame_no = 0;
	stream->data_len = 0;
	
	BDBG_MSG_TRACE(("bavi_demux_set_offset<: %#lx %u", (unsigned long)stream, (unsigned)timestamp));
	return;
}

void 
bavi_demux_set_timescale(bavi_demux_t demux, bmedia_time_scale time_scale)
{
	BDBG_MSG_TRACE(("bavi_demux_set_timescale>: %#lx %d", (unsigned long)demux, (int)time_scale));
	demux->state.time_scale = time_scale;
	BDBG_MSG_TRACE(("bavi_demux_set_timescale<: %#lx %d", (unsigned long)demux, (int)time_scale));
    return;
}

void 
bavi_stream_get_stream_cfg(bavi_stream_t stream, bmedia_pes_stream_cfg *cfg)
{
	BDBG_OBJECT_ASSERT(stream, bavi_stream_t);
	BDBG_ASSERT(cfg);
	bmedia_pes_default_stream_cfg(cfg);
	cfg->nvecs = sizeof(stream->vecs)/sizeof(stream->vecs[0]);
	cfg->vecs = stream->vecs;
	return;
}
