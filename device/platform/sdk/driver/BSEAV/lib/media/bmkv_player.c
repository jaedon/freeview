/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmkv_player.c $
 * $brcm_Revision: 106 $
 * $brcm_Date: 9/6/12 1:26p $
 *
 * Module Description:
 *
 * MKV player library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmkv_player.c $
 * 
 * 106   9/6/12 1:26p vsilyaev
 * SW7358-408: Insert meta data for a mpeg4asp key frames
 * 
 * 105   9/5/12 3:23p vsilyaev
 * SW7552-338: Fixed typo
 * 
 * 104   8/31/12 10:53a vsilyaev
 * SW7429-201: Fixed typo
 * 
 * 103   8/29/12 3:09p vsilyaev
 * SW7429-201: Better handle cases where playback/seek reaches end of file
 * 
 * 102   8/24/12 3:13p vsilyaev
 * SW7425-3786: Set 'first_frame' mark even for seeks to head of the file
 * 
 * 101   5/8/12 2:45p vsilyaev
 * SW7425-2994: Properly handle pass-through copy of compressed payloads
 * 
 * 100   5/5/12 10:46a rgreen
 * SW7425-2994: Fix compilation error
 * 
 * 99   5/4/12 3:54p vsilyaev
 * SW7425-2994: Updated mkv support to reject tracks with unsupported
 * compression, except the 'auxilary' tracks
 * 
 * 98   1/3/12 10:47a vsilyaev
 * SW7425-2040: Fixed typo in processing inner vorbis payloads
 * 
 * 97   12/1/11 12:38p vsilyaev
 * SWBLURAY-27850: Added handling of invalid MKV/H264 streams with 'raw
 * ES' frame payload
 * 
 * 95   8/25/11 6:21p vsilyaev
 * SWDTV-8388: Added OGG support
 * 
 * 94   8/10/11 1:07p vsilyaev
 * FWAVD-138: Fixed warning
 * 
 * 93   8/10/11 12:05p vsilyaev
 * FWAVD-138: Added EOS for VP8 stream
 * 
 * 92   8/9/11 5:06p vsilyaev
 * FWAVD-138: Used dedicated bounded buffer for the PES headers
 * 
 * 91   8/1/11 7:18p vsilyaev
 * SWDTV-8077, FWAVD-138: Fixed handling of bounded PES payloads
 * 
 * 90   7/21/11 11:16a vsilyaev
 * FWAVD-138: Better handle bounded PES packets with size larger then max
 * for the pes packet
 * 
 * 89   7/12/11 5:54p vsilyaev
 * FWAVD-138: Used bounded PES
 * 
 * 88   7/6/11 6:40p vsilyaev
 * FWAVD-138: Added support for VP8 video
 * 
 * 87   7/6/11 3:50p vsilyaev
 * SWDTV-7732: Use 1:1 mapping between vorbis and BCMA packet
 * 
 * 86   7/1/11 5:43p vsilyaev
 * SWDTV-7732: Added vorbis support
 * 
 * 85   5/17/11 2:31p vsilyaev
 * SWDTV-6826: Updated construction of EOS sequence
 * 
 * 84   5/16/11 6:58p vsilyaev
 * SWBLURAY-25681: [merge] Always send SPS/PPS before the first AVC frame
 * 
 * 83   5/3/11 4:34p vsilyaev
 * SWBLURAY-22800: Use common code to parser H264 meta data
 * 
 * 82   4/14/11 4:04p jtna
 * SWDTV-6321: fix build warnings for 2.6.37 kernel
 * 
 * 81   4/11/11 5:58p vsilyaev
 * SW7405-5223: Added support for some audio codecs in A_MS/ACM format
 * 
 * 80   2/23/11 10:36a vsilyaev
 * SWDTV-4570, SW7550-689: Use proper type for the index entries
 * 
 * 79   12/15/10 1:35p vsilyaev
 * SW35230-2279: Use proper type for the index entries
 * 
 * 78   10/1/10 7:06p vsilyaev
 * SW35230-1433: Refactored decompression so it codec independent
 * 
 * 77   10/1/10 5:49p vsilyaev
 * SW35230-1433: Added handling of 'Header Stripping' payload compression
 * 
 * 76   8/31/10 1:54p vsilyaev
 * SW7405-4833: In AC3 payloads probe for frame-sync and add if missing
 * 
 * 75   8/19/10 6:31p vsilyaev
 * SWBLURAY-21863: Unified support PES sub-packetization for MP4 and MKV
 * players
 * 
 * 74   8/19/10 5:22p vsilyaev
 * SWBLURAY-21863: Added option to divide large video frame into multiple
 * PES packets
 * 
 * 73   8/10/10 10:00a gmohile
 * SW3548-3050 : Parse multiple attachment links in trackentry
 * 
 * 72   7/27/10 4:48p gmohile
 * SW3548-2994 : Use header information to generate adts header if codec
 * private is absent
 * 
 * 71   7/26/10 12:00p gmohile
 * SW3548-2994 : Fix player timestamp calculation
 * 
 * 70   7/20/10 3:53p gmohile
 * SW3548-2994 : Fix DRM callback
 * 
 * 69   7/19/10 4:18p gmohile
 * SW3548-2994 : Add callback to decrypt DRM tracks
 * 
 * 68   7/16/10 1:23p vsilyaev
 * SW7550-473: Fixed error code propagation in the FastForward path
 * 
 * 67   6/28/10 1:36p vsilyaev
 * SW3548-3001: Updated code to return an error when instead of the next
 * cluster encountered object with invalid ID
 * 
 * 66   6/25/10 12:01p vsilyaev
 * SW3548-2995: Separate requested reserve size and minimal size for
 * succesive parsing
 * 
 * 65   6/21/10 12:49p vsilyaev
 * SW3548-2975: Added code to detect offset for the next EBML object (next
 * volume)
 * 
 * 64   4/13/10 4:06p vsilyaev
 * SW7405-3349: Updated format of the MKV auxiliary data to be independent
 * from the underlying carriage (PES or ES)
 * 
 * 63   4/6/10 6:50p vsilyaev
 * SWBLURAY-19311: Properly use flags from the SimpleBlock
 * 
 * fix_bdvd_v3.0_issue19311/1   4/6/10 2:48p ialauder
 * If the file contains simple blocks, check the data flags to see if the
 * frame is a keyframe.
 * 
 * 62   3/18/10 2:56p jtna
 * SW3548-2842: added mpeg2 video support
 * 
 * 61   3/18/10 1:52p vsilyaev
 * SWBLURAY-18986: Updated code to identify key frames.
 * 
 * fix_bdvd_v3.0_issue18986/3   3/18/10 3:32p ialauder
 * If the block group in the cluster is invalid, then it should not be a
 * keyframe
 * 
 * fix_bdvd_v3.0_issue18986/2   3/17/10 2:09p ialauder
 * update conditional for keyframe
 * 
 * fix_bdvd_v3.0_issue18986/1   3/15/10 12:08p ialauder
 * When verifying a keyframe, check if both the block and the block group
 * are valid when checking the reference block
 * 
 * 60   3/10/10 5:50p vsilyaev
 * SW3548-2810: Added code to dump starcodes from the payload
 * 
 * 59   2/24/10 3:31p vsilyaev
 * SWBLURAY-18797: Merged from branch
 * 
 * fix_bdvd_v3.0_issue18797/1   2/24/10 6:09p ialauder
 * If there are not 64 bytes remaining in the segment use the remaining
 * amount.
 * 
 * 58   2/18/10 6:11p vsilyaev
 * SW3556-913: Properly propagate end of stream condition
 * 
 * 57   2/18/10 12:35p vsilyaev
 * SW3556-913: Fixed typo
 * 
 * 56   2/18/10 12:05p vsilyaev
 * SW3556-913: Differentiate between index error and end of stream
 * 
 * 55   2/11/10 5:29p vsilyaev
 * SW7408-75: use ASSERTS to check for invalid file offsets
 * 
 * 54   1/13/10 5:06p vsilyaev
 * SW3556-913: Remeber file I/O related errors and propagate them
 * 
 * 53   1/13/10 3:51p vsilyaev
 * SW3556-913: Fixed missing initialization
 * 
 * 52   1/12/10 7:57p vsilyaev
 * SW3556-913: Separate data and index reads, allow incomplete data reads
 * 
 * 51   12/2/09 11:26a vsilyaev
 * SW7405-3368: Fixed typos
 * 
 * dev_bdvd_v3.0_issue15380/1   12/2/09 11:23a gooslin
 * Fix typos
 * 
 * 50   12/1/09 3:13p vsilyaev
 * SW7405-3513: Merged changes to probe SBR information in the AAC streams
 * 
 * fix_bdvd_v3.0_issue17634/1   11/25/09 4:24p ialauder
 * Changes to detect AAC-HE
 * 
 * 49   11/23/09 5:32p vsilyaev
 * SW7405-3368: Added parsing of MKV attachments
 * 
 * 48   11/17/09 12:09p vsilyaev
 * SW7405-3368: Added code to insert the AttachmentLink UID
 * 
 * 47   11/5/09 8:14p vsilyaev
 * SW7405-3349: Sent codec private data and duration among with auxilary
 * payload
 * 
 * 46   11/2/09 3:10p gmohile
 * SW7405-3330 : Add support to extract auxiliary track
 * 
 * 45   6/23/09 5:51p vsilyaev
 * PR 55845: Added content type for the media player entry, this fixes key
 * frame extraction for AVI 2.0 (OpenDML) streams
 * 
 * 44   6/19/09 3:47p vsilyaev
 * PR 55989: Added decode_mode  paramtewr to to the set_direction method
 * 
 * 43   6/2/09 7:39p vsilyaev
 * PR 55417: Added support for read method returning no data or completing
 * with a partial read
 * 
 * 42   5/15/09 11:22a vsilyaev
 * PR 55142: Fixed use of VC1/WMV codecs, separated VC1_SM/VC_AP and rest
 * of WMV
 * 
 * 41   4/24/09 10:18a gooslin
 * PR 54566: Return the index error count with status
 * 
 * 40   4/1/09 3:15p vsilyaev
 * PR 53744: Use common location for the H264 RAI sequence
 * 
 * 39   4/1/09 2:54p vsilyaev
 * PR 53744: Insert random access indicator after PPS/SPS
 * 
 * fix_bdvd_v2.0_pr13769/fix_bdvd_v2.0_pr13751/fix_bdvd_v2.0_pr13893/3   3/31/09 4:38p gooslin
 * Fix typo
 * 
 * fix_bdvd_v2.0_pr13769/fix_bdvd_v2.0_pr13751/fix_bdvd_v2.0_pr13893/2   3/31/09 4:23p gooslin
 * Merge latest
 * 
 * 38   3/31/09 10:01a vsilyaev
 * PR 53707: Merged AVC support in the VFW form.
 * 
 * fix_bdvd_v2.0_pr13769/fix_bdvd_v2.0_pr13751/3   3/30/09 3:40p mthadani
 * change eos vector for vfw_avc
 * 
 * fix_bdvd_v2.0_pr13769/fix_bdvd_v2.0_pr13751/2   3/30/09 10:37a gooslin
 * Merge latest from main
 * 
 * fix_bdvd_v2.0_pr13769/fix_bdvd_v2.0_pr13751/1   3/27/09 11:26a mthadani
 * add support for AVC under CodecID="VFW"
 * 
 * 37   3/27/09 12:51p vsilyaev
 * PR 50311: Return readPosition in the Nexus playback status. In
 * combination with the position it could be used to measure depth of
 * decoder and playback buffers in units of time.
 * 
 * 36   3/26/09 11:32a vsilyaev
 * PR53620 : Prevent infinite loop when handling certain streams
 * 
 * fix_bdvd_v2.0_pr13769/1   3/18/09 6:55p gooslin
 * Prevent infinite loop by verifying the same position isn't returned
 * twice during rewind
 * 
 * 35   3/12/09 1:45p vsilyaev
 * PR 53155: Better support for the seek table
 * 
 * 34   3/6/09 11:30a vsilyaev
 * PR 52903: Added the 'break;' statement after BDBG_ASSERT(0)
 * 
 * 33   3/2/09 3:05p vsilyaev
 * PR 52434: Post merge updates/unification of parsing video data
 * 
 * 32   3/2/09 10:27a vsilyaev
 * PR 52434: Merge XViD and VC1 AP support in MKV
 * 
 * fix_bdvd_v1.42_pr12986/9   2/27/09 5:26p gooslin
 * Add vc1 seq header info type
 * 
 * fix_bdvd_v1.42_pr12986/8   2/27/09 4:52p gooslin
 * Merge to the latest bmedia code
 * 
 * 31   2/17/09 11:19a vsilyaev
 * PR 52084: Added support for MPEG audio
 * 
 * fix_bdvd_v1.42_pr12986/7   2/23/09 12:18p gooslin
 * Add VC1AP seq header parser
 * 
 * fix_bdvd_v1.42_pr12986/6   2/23/09 11:57a gooslin
 * Add error checking and comments to xvid support
 * 
 * fix_bdvd_v1.42_pr12986/5   2/21/09 3:57p gooslin
 * Break loop after failing to get reordered PTS data
 * 
 * fix_bdvd_v1.42_pr12986/4   2/21/09 2:57p gooslin
 * Push queued frames through at EOS
 * 
 * fix_bdvd_v1.42_pr12986/3   2/20/09 5:54p gooslin
 * Cleanup VC1AP and xvid support changes
 * 
 * fix_bdvd_v1.42_pr12986/1   2/18/09 1:43p gooslin
 * Add ssupport for MP4 in MKV
 * 
 * 30   1/12/09 5:42p vsilyaev
 * PR 50763: Improved seek to position 0
 * 
 * 29   1/12/09 12:36p vsilyaev
 * PR 50763: Added insertion of EOS packets
 * 
 * 28   12/19/08 3:56p vsilyaev
 * PR 50214: Allowed set_direction function to fail and added framework
 * for players to communicate about an errors
 * 
 * 27   12/19/08 12:20a vsilyaev
 * PR 50500: Added support for AC3+ audio
 * 
 * 26   12/18/08 7:04p vsilyaev
 * PR 47854: Improved handling of conditions where asynchronous read
 * exhaust allocated buffer
 * 
 * 25   12/10/08 12:49p vsilyaev
 * PR 49718: Fixed error reporting/handling from MKV player
 * 
 * 24   11/21/08 6:31p gmohile
 * PR 49494 : Increase buffer size
 * 
 * 23   10/16/08 6:54p vsilyaev
 * PR 47995: Added control over ID's used for media->PES conversion
 * 
 * 22   10/8/08 12:18p vsilyaev
 * PR 47280: Added support for another way of embedding AAC audio into MKV
 * files
 * 
 * 21   10/1/08 3:34p vsilyaev
 * PR 47282: PR 46848: Added support for SimpleBlock
 * 
 * 20   9/24/08 8:53p vsilyaev
 * PR 47282: Only shutdown block group if it was initialized
 * 
 * 19   9/4/08 4:51p vsilyaev
 * PR 46540: Fixed build in the 'ES' mode
 * 
 * 18   8/4/08 1:06p vsilyaev
 * PR 45388: Fixed use of custom TimeScale
 * 
 * 17   8/4/08 12:36p vsilyaev
 * PR 45388: Fixed handling of MKV files without seek table
 * 
 * 16   8/4/08 12:34p vsilyaev
 * PR 45388: Fixed handling of MKV files without seek table
 * 
 * 15   8/1/08 5:37p erickson
 * PR45361: c89 fixes
 * 
 * 14   7/31/08 1:01p vsilyaev
 * PR 45307: Fixed handling trickmodes after seek to the begining of the
 * stream
 * 
 * 13   7/23/08 12:58p vsilyaev
 * PR 45079: Fixed handling of asynchronous end-of-stream
 * 
 * 12   4/3/08 1:55p vsilyaev
 * PR 39818: Added asynchronous moded for play,forward and reverse
 * 
 * 11   3/18/08 1:05p vsilyaev
 * PR 39818: Added support for rewind and fastforward
 * 
 * 10   3/6/08 10:49a vsilyaev
 * PR 39818: Added track resync after seek and fast forward mode
 * 
 * 9   3/5/08 6:58p vsilyaev
 * PR 39818: Fixed handling of stream bounds
 * 
 * 8   3/5/08 6:26p vsilyaev
 * PR 39818: Unified cache support
 * 
 * 7   3/5/08 3:14p vsilyaev
 * PR 39818: Fixed indexing of seek table
 * 
 * 6   3/5/08 3:08p vsilyaev
 * PR 39818: Added DTS audio
 * 
 * 5   3/5/08 2:31p vsilyaev
 * PR 39818: Added seek support
 * 
 * 4   3/4/08 12:38p vsilyaev
 * PR 39818: Fixed memory leak
 * 
 * 3   3/4/08 12:23p vsilyaev
 * PR 39818: Use common code to parse MKV file
 * 
 * 2   3/3/08 6:32p vsilyaev
 * PR 39818: Improved diagnostics and fixed problem of not reading
 * complete blocks
 * 
 * 1   3/3/08 11:33a vsilyaev
 * PR 39818: Support of MKV format
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bmkv_parser.h"
#include "bmkv_player.h"
#include "blst_squeue.h"
#include "bmedia_util.h"
#include "bmpeg4_util.h"
#include "biobits.h"
#include "bmkv_file_parser.h"
#include "bmedia_index.h"
#include "blifo.h"

BDBG_MODULE(bmkv_player);
#define BDBG_MSG_TRACE(x) /* BDBG_MSG(x) */
#define B_MKV_ES_ONLY	0

/* 
 * number of cache entries for seek and cue tables, this cache is used to speed-up random access, 
 * each entriy point to the exact location, and then linear walk performed to arrive  at destination 
 */
#define B_MKV_PLAYER_INDEX_ENTRIES	256

/* since there is no random access structure that could be used to point to key frames, we use two pass 
 * algorithm, on first pass we jump back, then traverse CUE and capture entries into the stack.
 * On the second pass we extract entries from the stack and play them 
 */
/* this is maxinmum number of entries we would capture */
#define B_MKV_PLAYER_TRICK_ENTRIES	128
/* this is step back, it's important that between steps number of key frames (cue points) is less then B_MKV_PLAYER_REWIND_ENTRIES, 
 * since other entries would be discarded (e.g.  rewind would not show all frames that it could */
#define B_MKV_PLAYER_REWIND_STEP (50* BMEDIA_PLAYER_POS_SCALE)

/* size of read transaction for the data stream */
#define B_MKV_PLAYER_DATA_BLOCK_SIZE (60*1024)
/* size of read transaction for the index stream */
#define B_MKV_PLAYER_INDEX_BLOCK_SIZE (1024)

/* maximum size of the seek entry */
#define B_MKV_PLAYER_MAX_SEEK_ENTRY (B_MKV_MAX_ELEMENT_HEAD +  (B_MKV_MAX_ELEMENT_HEAD + 8 /* size */) + (B_MKV_MAX_ELEMENT_HEAD + 8 /* id */) )
#define B_MKV_PLAYER_MIN_SEEK_ENTRY (B_MKV_MIN_ELEMENT_HEAD +  (B_MKV_MIN_ELEMENT_HEAD + 1 /* size */) + (B_MKV_MIN_ELEMENT_HEAD + 1 /* id */) )
/* maximum size of the element head (id + size) */
#define B_MKV_PLAYER_INDEX_ENTRY	(1024)


#define B_AC3_SYNC      0x0B77
static const uint8_t b_mkv_ac3_frame_sync[] = { 0x0b, 0x77 };

typedef struct bmkv_Cluster {
	uint64_t Timecode;
	bmkv_table SilentTracks;
	uint64_t Position;
	uint64_t PrevSize;
	bmkv_size BlockGroup;
	bmkv_size SimpleBlock;
	struct {
		bool Timecode;
		bool SilentTracks;
		bool Position;
		bool PrevSize;
		bool BlockGroup;
		bool SimpleBlock;
	} validate;
} bmkv_Cluster;

typedef struct bmkv_Block {
	unsigned track;
	int16_t timecode;
	uint16_t nframes;
	uint8_t flags;
	size_t *frames;
	size_t _frames[16]; /* embedded frames */
} bmkv_Block;

typedef struct bmkv_BlockGroup {
	bmkv_Block data;
	unsigned BlockDuration;
	unsigned ReferencePriority;
	unsigned ReferenceBlock;
	bmkv_size Block;
	bmkv_size BlockVirtual;
        uint8_t DRMInfo[16];
	struct {
		bool Block;
		bool BlockVirtual;
		bool BlockDuration;
		bool ReferencePriority;
		bool ReferenceBlock;
	        bool DRMInfo;
	} validate;
} bmkv_BlockGroup;


typedef struct bmkv_CueTrackPosition {
	unsigned CueTrack;
	uint64_t CueClusterPosition;
	unsigned CueBlockNumber;
	struct {
		bool CueTrack;
		bool CueClusterPosition;
		bool CueBlockNumber;
	} validate;
} bmkv_CueTrackPosition;

typedef struct bmkv_CuePoint {
	uint64_t CueTime;
	bmkv_table CueTrackPositions;
	struct {
		bool CueTime;
		bool CueTrackPositions;
	} validate;
} bmkv_CuePoint;

typedef enum b_mkv_codec_type {
	b_mkv_codec_type_avc,
	b_mkv_codec_type_mpeg4asp,
	b_mkv_codec_type_vfw_vc1_ap,
	b_mkv_codec_type_vfw_mpeg4asp,
	b_mkv_codec_type_vfw_avc,
	b_mkv_codec_type_aac,
	b_mkv_codec_type_ac3,
	b_mkv_codec_type_ac3_plus,
	b_mkv_codec_type_dts,
	b_mkv_codec_type_mpeg_audio,
	b_mkv_codec_type_vorbis,
	b_mkv_codec_type_vp8,
    b_mkv_codec_type_auxilary,
	b_mkv_codec_type_unknown
} b_mkv_codec_type;

typedef struct b_mkv_pts_reorder_info {
    bmedia_player_pos timestamp;
} b_mkv_pts_reorder_info;

#define B_MKV_VORBIS_HEADERS    3
struct b_mkv_player_track {
	BLST_S_ENTRY(b_mkv_player_track) next;
	unsigned track_no;
	const bmkv_TrackEntry *mkv_track;
	b_mkv_codec_type codec_type;
	unsigned TimecodeScale;
	bmedia_pes_info pes_info;
    bmkv_file_encoding_info encoding_info;
    bool bounded_pes;
	bool first_frame;
	bool seek_sync;
    bool pts_reorder_enabled;
	union {
        bmedia_adts_header aac;
		struct {
            bmedia_h264_meta meta;
		} avc;
		struct {
            bmedia_bitmapinfo bitmap_info;
            union {
                struct {
                    batom_vec seq_hdr;
                    bmedia_vc1ap_info seq_hdr_info;
                } vc1;
            } codec;
		} vfw;
        struct {
            enum {b_mkv_ac3_frame_sync_unknown, b_mkv_ac3_frame_sync_present, b_mkv_ac3_frame_sync_missing} frame_sync;
        } ac3;
        struct {
            uint8_t waveFormatEx[BMEDIA_WAVEFORMATEX_BASE_SIZE];
            uint8_t headerSizes[B_MKV_VORBIS_HEADERS][2]; /* headers are 2 byte */
            batom_vec meta[1+2*B_MKV_VORBIS_HEADERS];  /* one vector for WAV header, and 2 vectors for each of header packets */
        } vorbis;
	} codec;

    struct {
        batom_pipe_t pipe_needs_reorder;
        batom_pipe_t pipe_reorder_done;
        batom_accum_t accum_reorder;
        bmedia_player_pos last_timestamp;
    } pts_queue;
	size_t hdr_len;
};


typedef struct b_mkv_key_frame_entry {
	off_t cluster_off; /* cluster offset in the segment */
	bmedia_player_pos time; /* time of the key frame */
} b_mkv_key_frame_entry;

typedef enum b_mkv_fetch_block_result {
	b_mkv_fetch_block_result_success, 
	b_mkv_fetch_block_result_err_read, 
	b_mkv_fetch_block_result_err_parse_cluster, 
	b_mkv_fetch_block_result_err_no_data, 
	b_mkv_fetch_block_result_err_eof, 
	b_mkv_fetch_block_result_err_block
} b_mkv_fetch_block_result;

typedef enum b_mkv_result {
	b_mkv_result_success, 
	b_mkv_result_end_of_data,
	b_mkv_result_error
} b_mkv_result;

typedef enum b_mkv_frame_step {
    b_mkv_frame_step_done,
    b_mkv_frame_step_cluster,
    b_mkv_frame_step_block,
    b_mkv_frame_step_rewind_refill
} b_mkv_frame_step;


BDBG_OBJECT_ID(bmkv_player_t);
struct bmkv_player {
	BDBG_OBJECT(bmkv_player_t)
	batom_factory_t factory;
	batom_accum_t accum_dest;
	batom_accum_t accum_temp;
	bmedia_player_config config;
	bmedia_player_status status;
	bmedia_time_scale time_scale;
	bmedia_player_stream stream;
	size_t  chunk_size;

	BLST_S_HEAD(b_mkv_player_tracks, b_mkv_player_track) tracks;

	struct {
		bfile_cached_segment cache;
		bmkv_BlockGroup group;
		batom_cursor block_cursor; /* cursor that points to first byte of payload */
		bmkv_Cluster mkv_cluster;
		off_t offset_next; /* file offset for the next cluster */
		bool cluster_valid;
		bool block_valid;
		bool first_found;
		bool seek_to_index;
		bool resync_video;
		uint64_t seek_table_offset;
		off_t offset_first;
		bmedia_player_pos seek_target;
		b_mkv_fetch_block_result fetch_block_result;
		struct {
			struct {
				bool Block;
				bool BlockVirtual;
			} validate;
		} blockheader;
		struct {
			struct {
				bool parse_payload;
		 		void (*block_complete)(bmkv_player_t player, bfile_segment_async_result result);
			} next_block;
			struct {
				bool parse_payload;
		 		void (*block_complete)(bmkv_player_t player, bfile_segment_async_result result);
			} fetch_block;
            struct {
                bmedia_player_entry entry;
                void (*frame_complete)(bmkv_player_t player, bfile_segment_async_result result); /* function called on completion */
                bfile_segment_async_result (*step)(b_mkv_frame_step state, bmkv_player_t player, bfile_segment_async_result result); /* function called to handle state transitions */
                bool parse_payload; /* whether to parse payload during frame scan */
            } frame;
            struct {
                bmedia_player_pos pos; /* target for the fast forward modes */
            } trick;
            struct {
	            bmedia_player_pos base_pos; /* base position of the key frame stack */
	            b_mkv_key_frame_entry *key; /* target key frame */
            } rewind;
            enum {b_mkv_data_continuation_next_block, b_mkv_data_continuation_fetch_block} continuation_type;
            bool continuation_active;
            bool busy;
		} async;
	} cluster;

	struct { /* data structures used for trick modes, e.g. extracting Key frames in fast-fast or rewind modes */
		BLIFO_HEAD(b_mkv_key_frame_stack, b_mkv_key_frame_entry)  rewind_frames;
	} trick;

	bfile_buffer_t index_buffer;
	struct {
		bfile_cached_segment cache;
		bmedia_index_t index;
	} seek_table;

	struct {
		bfile_cached_segment cache;
		bmedia_index_t index;
		bmedia_player_pos mkv_pos;
		bmkv_CueTrackPosition mkv_track_pos;
		bmedia_player_pos index_pos;
		bool validate;
	} cue;
    bool eof_reached;
	unsigned frame_buf_off; /* offset into the frame buffer */
    unsigned frame_buf_persistent_alloc; /* bytes at the end of the frame buff which are allocated for the life of the player */

	struct bmkv_file_parser mkv_file;

	uint8_t frame_buf[1024];
	b_mkv_key_frame_entry key_frames_buf[B_MKV_PLAYER_TRICK_ENTRIES];
	uint8_t file_buffer[64*BIO_BLOCK_SIZE];
};

static const uint8_t b_mkv_aux_header_data[] = { 'b', 'm', 'k', 'v'};
static const batom_vec b_mkv_aux_header = BATOM_VEC_INITIALIZER((void *)b_mkv_aux_header_data, sizeof(b_mkv_aux_header_data)); 

static void 
b_mkv_data_atom_free(batom_t atom, void *user)
{
	BDBG_MSG_TRACE(("b_mkv_data_atom_free:%#lx", (unsigned long)atom));
	BSTD_UNUSED(atom);
	BSTD_UNUSED(user);
	return;
}

static const batom_user b_mkv_data_atom = {
	b_mkv_data_atom_free,
	sizeof(void **)
};

static const batom_user b_mkv_asp_data_atom = {
    b_mkv_data_atom_free,
    sizeof(b_mkv_pts_reorder_info)
};

static const BMKV_PARSER_BEGIN(bmkv_BlockGroup_parser)
    BMKV_PARSER_FIELD_UNSIGNED(bmkv_BlockGroup, 0x9B, BlockDuration)
    BMKV_PARSER_FIELD_UNSIGNED(bmkv_BlockGroup, 0xFA, ReferencePriority)
    BMKV_PARSER_FIELD_UNSIGNED(bmkv_BlockGroup, 0xFB, ReferenceBlock)
    BMKV_PARSER_FIELD_STRING(bmkv_BlockGroup, 0xDD, DRMInfo)
    BMKV_PARSER_FIELD_STOP(bmkv_BlockGroup, 0xA1, Block)
    BMKV_PARSER_FIELD_STOP(bmkv_BlockGroup, 0xA2, BlockVirtual)
BMKV_PARSER_END(bmkv_BlockGroup_parser);

static void
bmkv_BlockGroup_init(bmkv_BlockGroup *group)
{
	group->validate.BlockDuration = false;
	group->validate.ReferencePriority = false;
	group->validate.ReferenceBlock = false;
	group->validate.DRMInfo = false;
	group->data.frames = group->data._frames;
	group->data.nframes = 0;
	return;
}

static bool
bmkv_BlockGroup_parse(batom_cursor *cursor, size_t len, bmkv_BlockGroup *group)
{
	group->validate.Block = false;
	group->validate.BlockVirtual = false;
	if(bmkv_element_parse(cursor, len, bmkv_BlockGroup_parser, "BlockGroup", group)) {
		bmkv_element_print(bmkv_BlockGroup_parser, BDBG_eMsg, 0, "BlockGroup", group);
		return true;
	}
	return false;
}


static int
bmkv_BlockGroup_alloc(bmkv_BlockGroup *group, size_t nframes)
{
	BDBG_ASSERT(group->data.frames == group->data._frames);
	group->data.nframes = nframes;
	if(nframes>sizeof(group->data._frames)/sizeof(group->data._frames[0])) {
		group->data.frames = BKNI_Malloc(nframes*sizeof(*group->data.frames));
		if(!group->data.frames) {
			group->data.nframes = 0;
			BDBG_WRN(("bmkv_Block_init: can't allocated %u bytes for the size array", nframes*sizeof(*group->data.frames)));
			return -1;
		}
	}
	return 0;
}

static void
bmkv_BlockGroup_shutdown(bmkv_BlockGroup *group)
{
	if(group->data.frames != group->data._frames) {
		BKNI_Free(group->data.frames);
		group->data.frames = group->data._frames;
	}
	return;
}

typedef struct bmkv_SilentTrack {
	unsigned SilentTrackNumber;
	struct {
		bool SilentTrackNumber;
	} validate;
} bmkv_SilentTrack;

static const BMKV_PARSER_BEGIN(bmkv_SilentTrack_parser)
    BMKV_PARSER_FIELD_UNSIGNED(bmkv_SilentTrack, 0x5897, SilentTrackNumber)
BMKV_PARSER_END(bmkv_SilentTrack_parser);

static const bmkv_parser_desc bmkv_SilentTrack_desc = {
    "SilentTrack",
    bmkv_SilentTrack_parser,
    sizeof(bmkv_SilentTrack),
	NULL /* memset init */
};


static const BMKV_PARSER_BEGIN(bmkv_Cluster_parser)
    BMKV_PARSER_FIELD_UNSIGNED64(bmkv_Cluster, 0xE7, Timecode)
    BMKV_PARSER_FIELD_TABLE(bmkv_Cluster, 0x5854, SilentTracks, bmkv_SilentTrack_desc)
    BMKV_PARSER_FIELD_UNSIGNED64(bmkv_Cluster, 0xA7, Position)
    BMKV_PARSER_FIELD_UNSIGNED64(bmkv_Cluster, 0xAB, PrevSize)
    BMKV_PARSER_FIELD_STOP(bmkv_Cluster, 0xA0, BlockGroup)
    BMKV_PARSER_FIELD_STOP(bmkv_Cluster, 0xA3, SimpleBlock)
BMKV_PARSER_END(bmkv_Cluster_parser);

static bool
b_mkv_player_fill_segment(bmkv_player_t player, const bmkv_SeekElement *seek, bmkv_id id, bfile_segment *segment )
{
	bmkv_header header;
	batom_cursor cursor;
	batom_t atom;
	bfile_buffer_result read_result;
	off_t offset;

	BDBG_MSG_TRACE(("%s:%#lx reading seek target %u id %#x", "b_mkv_player_fill_segment", (unsigned long)player, (unsigned)seek->SeekPosition, (unsigned)id));
	if(!seek->validate.SeekPosition) {
		goto err_validate;
	}
	offset = (off_t)seek->SeekPosition + player->mkv_file.segment.start;
	atom = bfile_buffer_read(player->index_buffer, offset, B_MKV_MAX_ELEMENT_HEAD, &read_result);
	if(!atom) {
		BDBG_MSG(("%s:%#lx can't read seek target %u", "b_mkv_player_fill_segment", (unsigned long)player, (unsigned)seek->SeekPosition));
		goto err_read;
	}
	batom_cursor_from_atom(&cursor, atom);
	if(!bmkv_parse_header(&cursor, &header)) {
		goto err_header;
	}
	if(header.id != id) {
		BDBG_WRN(("%s:%#lx reading seek target %u unexpected id %#x(%#x)", "b_mkv_player_fill_segment", (unsigned long)player, (unsigned)seek->SeekPosition, (unsigned)header.id, (unsigned)id));
		goto err_id; /* unexpected id */
	}
	batom_release(atom);
	segment->start = offset + batom_cursor_pos(&cursor);
	segment->len = header.size;
	if(header.size == BMKV_RESERVED_SIZE) {
		segment->len = (uint64_t)-1;
	}
    BDBG_ASSERT(segment->start>=0);
	return true;
err_id:
err_header:
	batom_release(atom);
err_read:
err_validate:
	return false;
}

static void
bmkv_Cluster_init(bmkv_Cluster *cluster)
{
	cluster->SilentTracks.nelems = 0;
	cluster->validate.Timecode = false;
	cluster->validate.SilentTracks = false;
	cluster->validate.Position = false;
	cluster->validate.PrevSize = false;
	return;
}

static bool
bmkv_Cluster_parse(batom_cursor *cursor, size_t len, bmkv_Cluster *cluster)
{
	/* clear cluster */
	cluster->validate.BlockGroup = false;
	cluster->validate.SimpleBlock = false;
	if(bmkv_element_parse(cursor, len, bmkv_Cluster_parser, "Cluster", cluster)) {
		bmkv_element_print(bmkv_Cluster_parser, BDBG_eMsg, 0, "Cluster", cluster);
		return true;
	}
	return false;
}

static void 
b_mkv_player_index_error(bmkv_player_t player)
{
	player->status.index_error_cnt++;
	BDBG_MSG_TRACE(("b_mkv_player_index_error:%#lx index_error:%u", (unsigned long)player, (unsigned long)player->status.index_error_cnt));
	player->config.error_detected(player->config.cntx);
	return;
}

static void 
b_mkv_player_data_error(bmkv_player_t player)
{
	player->status.data_error_cnt++;
	BDBG_MSG_TRACE(("b_mkv_player_data_error:%#lx data_error:%u", (unsigned long)player, (unsigned long)player->status.data_error_cnt));
	player->config.error_detected(player->config.cntx);
	return;
}

static bool
b_mkv_player_cluster_reserve(bmkv_player_t player, size_t reserve_size)
{
    if(player->config.atom_ready) { /* using async reads, there are provisions to separate data and index reads */
        return bfile_cached_segment_reserve_custom_buffer(&player->cluster.cache, reserve_size, player->index_buffer);
    } else {
        return bfile_cached_segment_reserve(&player->cluster.cache, reserve_size);
    }
}

static bool
b_mkv_player_cluster_reserve_min(bmkv_player_t player, size_t reserve_size, size_t min_size)
{
    if(player->config.atom_ready) { /* using async reads, there are provisions to separate data and index reads */
        return bfile_cached_segment_reserve_custom_buffer_min(&player->cluster.cache, reserve_size, min_size, player->index_buffer);
    } else {
        return bfile_cached_segment_reserve_min(&player->cluster.cache, reserve_size, min_size);
    }
}



static int
b_mkv_player_next_cluster(bmkv_player_t player, bool sequential, bool *endofstream)
{
	size_t parsed_size;

	bfile_cached_segment_seek(&player->cluster.cache, 0);
    *endofstream = false;

	BDBG_MSG_TRACE(("%s:%#lx %s segment.len:%u", "b_mkv_player_next_cluster", (unsigned long)player, sequential?"sequential":"", (unsigned)player->seek_table.cache.segment.len));
	while(sequential && player->seek_table.cache.segment.len) {
		bmkv_SeekElement seek;
        bmkv_element_parse_result parse_result;

		BDBG_MSG_TRACE(("%s:%#lx locating cluster in the seek table %u(%u):%u", "b_mkv_player_next_cluster", (unsigned long)player, (unsigned)player->seek_table.cache.accum_offset+batom_cursor_pos(&player->seek_table.cache.cursor), (unsigned)player->seek_table.cache.segment.len, (unsigned)(player->seek_table.cache.segment.start)));

		if(!bfile_cached_segment_reserve_min(&player->seek_table.cache, B_MKV_PLAYER_MAX_SEEK_ENTRY, B_MKV_PLAYER_MIN_SEEK_ENTRY)) {
			BDBG_MSG(("%s:%#lx reached end of cluster_seekhead", "b_mkv_player_next_cluster", (unsigned long)player));
			break;
		}
		player->cluster.seek_table_offset = bfile_cached_segment_tell(&player->seek_table.cache);
        parse_result = bmkv_element_parse_from_desc(&player->seek_table.cache.cursor, &bmkv_SeekElement_desc, BMKV_SEEK, &seek);
		if(parse_result == bmkv_element_parse_result_unknown_id && bmkv_element_skip(&player->seek_table.cache.cursor)) {
            continue;
        } else if(parse_result != bmkv_element_parse_result_success) {
            break;
		}

		if( !(seek.validate.SeekID && seek.validate.SeekPosition && seek.SeekID == BMKV_CLUSTER_ID) ) {
			continue;
		}
        if (((off_t)seek.SeekPosition + player->mkv_file.segment.start) < player->cluster.offset_next) {
            BDBG_WRN(("%s:%#lx Skipping SEEK segment. SeekPosition is not current %llu(%llu) %llu", "b_mkv_player_next_cluster", (unsigned long)player, seek.SeekPosition, player->mkv_file.segment.start, player->cluster.offset_next));
            continue;
        }

		if(!b_mkv_player_fill_segment(player, &seek, BMKV_CLUSTER_ID, &player->cluster.cache.segment)) {
			goto err_cluster;
		}
        if(!player->cluster.first_found) {
            player->cluster.first_found = true;
            player->cluster.offset_first = (off_t)seek.SeekPosition + player->mkv_file.segment.start;
        }
		player->cluster.seek_to_index = true;
		goto read_cluster;
	}
	player->cluster.seek_to_index = false;
	BDBG_MSG_TRACE(("%s:%#lx scanning for cluster in file %u", "b_mkv_player_next_cluster", (unsigned long)player, (unsigned)player->cluster.offset_next));
	for(;;) { /* scan top level elements until we found Cluster element, or reached read error */
		bmkv_header header;
		bfile_cached_segment_set(&player->cluster.cache, player->cluster.offset_next, B_MKV_MAX_ELEMENT_HEAD);
		if(!b_mkv_player_cluster_reserve_min(player, B_MKV_MAX_ELEMENT_HEAD, B_MKV_MIN_ELEMENT_HEAD)) {
            *endofstream = player->cluster.cache.last_read_result == (bfile_buffer_result)bfile_segment_async_result_success || player->cluster.cache.last_read_result == (bfile_buffer_result)bfile_segment_async_result_eof;
			BDBG_MSG(("%s:%#lx can't read data at %u(%u) %s", "b_mkv_player_next_cluster", (unsigned long)player, (unsigned)player->cluster.offset_next, B_MKV_MAX_ELEMENT_HEAD, *endofstream?"EndOfStream":""));
			goto err_read;
		}
		if(!bmkv_parse_header(&player->cluster.cache.cursor, &header)) {
			goto err_header;
		}
		BDBG_MSG_TRACE(("%s:%#lx id:%#lx size:%lu", "b_mkv_player_next_cluster", (unsigned long)player, (unsigned long)header.id, (unsigned long)header.size));
        if(header.id==BMKV_INVALID_ID) {
            goto err_id;
        }
		parsed_size = batom_cursor_pos(&player->cluster.cache.cursor);
		player->cluster.offset_next += parsed_size;
		if(header.id == BMKV_CLUSTER_ID) {

			/* bingo, fill in cluster location */
			bfile_cached_segment_set(&player->cluster.cache, player->cluster.offset_next, header.size);
			if(!player->cluster.first_found) {
				player->cluster.first_found = true;
				player->cluster.offset_first = player->cluster.offset_next - parsed_size;
			}
			goto read_cluster;
		}
		if(header.size == BMKV_RESERVED_SIZE) {
			goto err_size;
		}
		player->cluster.offset_next += header.size; /* advance to the next element */
	}
read_cluster:
	BDBG_MSG_TRACE(("%s:%#lx reading cluster from %u", "b_mkv_player_next_cluster", (unsigned long)player, (unsigned)player->cluster.cache.segment.start));
	/* save pointer to the next cluster */
	player->cluster.offset_next = player->cluster.cache.segment.start + player->cluster.cache.segment.len;
	player->cluster.cluster_valid = true;
	bmkv_Cluster_init(&player->cluster.mkv_cluster);

	return 0;

err_id:
err_cluster:
err_header:
err_read:
err_size:
	return -1;	
}

static const BMKV_PARSER_BEGIN(bmkv_CueTrackPosition_parser)
    BMKV_PARSER_FIELD_UNSIGNED(bmkv_CueTrackPosition, 0xF7, CueTrack)
    BMKV_PARSER_FIELD_UNSIGNED64(bmkv_CueTrackPosition, 0xF1, CueClusterPosition)
    BMKV_PARSER_FIELD_UNSIGNED(bmkv_CueTrackPosition, 0x5378, CueBlockNumber)
BMKV_PARSER_END(bmkv_CueTrackPosition_parser);

static const bmkv_parser_desc bmkv_CueTrackPosition_desc = {
    "CueTrackPosition",
    bmkv_CueTrackPosition_parser,
    sizeof(bmkv_CueTrackPosition),
	NULL /* memset init */
};

#define	BMKV_CUEPOINT_ID	0xBB

static const BMKV_PARSER_BEGIN(bmkv_CuePoint_parser)
    BMKV_PARSER_FIELD_UNSIGNED64(bmkv_CuePoint, 0xB3, CueTime)
    BMKV_PARSER_FIELD_TABLE(bmkv_CuePoint, 0xB7, CueTrackPositions, bmkv_CueTrackPosition_desc)
BMKV_PARSER_END(bmkv_CuePoint_parser);

static bool
b_mkv_player_CuePoint_parse(batom_cursor *cursor, bmkv_CuePoint *point, size_t len)
{
	point->validate.CueTime = false;
	point->validate.CueTrackPositions = false;
	point->CueTrackPositions.nelems = 0;
	point->CueTrackPositions.bmkv_table_alloc_nelems = 0;
	point->CueTrackPositions.data = NULL;
	if(bmkv_element_parse(cursor, len, bmkv_CuePoint_parser, "CuePoint", point)) {
		bmkv_element_print(bmkv_CuePoint_parser, BDBG_eMsg, 0, "CuePoint", point);
		return true;
	}
	return false;
}

static void
b_mkv_player_CuePoint_shutdown(bmkv_CuePoint *point)
{
	bmkv_element_shutdown(bmkv_CuePoint_parser, point);
}

static b_mkv_result 
b_mkv_player_load_index_entry(bmkv_player_t player, bmkv_CuePoint *CuePoint)
{
	bmkv_header header;

	for(;;) {
		if(!bfile_cached_segment_reserve_min(&player->cue.cache, B_MKV_MAX_ELEMENT_HEAD, B_MKV_MIN_ELEMENT_HEAD)) {
			BDBG_MSG_TRACE(("%s:%#lx bfile_cached_segment_reserve failed %u", "b_mkv_player_load_index_entry", (unsigned long)player, player->cue.cache.last_read_result));
            if(player->cue.cache.last_read_result==bfile_buffer_result_ok) {
                return b_mkv_result_end_of_data;
            }
			break;
		}
		if(!bmkv_parse_header(&player->cue.cache.cursor, &header)) {
			BDBG_MSG_TRACE(("%s:%#lx bmkv_parse_header failed", "b_mkv_player_load_index_entry", (unsigned long)player));
			break;
		}
		if(!bfile_cached_segment_reserve(&player->cue.cache, header.size)) {
			BDBG_MSG_TRACE(("%s:%#lx bfile_cached_segment_reserve failed", "b_mkv_player_load_index_entry", (unsigned long)player));
			break;
		}
		if(header.id==BMKV_CUEPOINT_ID) {
			if(!b_mkv_player_CuePoint_parse(&player->cue.cache.cursor, CuePoint, header.size)) {
				BDBG_MSG_TRACE(("%s:%#lx b_mkv_player_CuePoint_parse failed", "b_mkv_player_load_index_entry", (unsigned long)player));
				break;
			}
			return b_mkv_result_success;
		} else {
			batom_cursor_skip(&player->cue.cache.cursor, header.size);
		}
	}
	return b_mkv_result_error;
}

static bmedia_player_pos
b_mkv_player_track_time(const struct b_mkv_player_track *track, uint64_t timecode)
{
	bmedia_player_pos time;
	if(track->TimecodeScale==1000000) {
		time = timecode;
	} else {
		time = (timecode*track->TimecodeScale)/1000000; /* timestamp in ms */
	}
    return time;
}

static bmedia_player_pos
b_mkv_player_track_timestamp(bmkv_player_t player, const struct b_mkv_player_track *track, int16_t delta)
{
	bmedia_player_pos time;
	BDBG_MSG_TRACE(("b_mkv_player_track_timestamp:%#lx TimecodeScale:%lu Timecode:%lu  delta:%d", (unsigned long)player, (unsigned long)track->TimecodeScale, (unsigned long)player->cluster.mkv_cluster.Timecode, (int)delta));
    time = b_mkv_player_track_time(track, player->cluster.mkv_cluster.Timecode + delta);
	if(time>player->status.bounds.last) { /* try to catch case where cluster Timecode is bogus */
		BDBG_MSG(("b_mkv_player_track_timestamp:%#lx detected invalid timestamp %u:%u",  (unsigned long)player, (unsigned)time, (unsigned)player->status.bounds.last));
		time = 0;
		if(delta>0) {
			time = player->status.bounds.first + ((delta)*(uint64_t)1000000)/track->TimecodeScale; /* timestamp in ms */
		} 
		time += player->status.bounds.first;
	}
	return time;
}

static bmedia_player_pos
b_mkv_player_timestamp(bmkv_player_t player, uint64_t Timecode)
{
	if(Timecode == 1000000)  {
		return (bmedia_player_pos) Timecode;
	} else {
		return (Timecode * player->mkv_file.segment_info.TimecodeScale) / 1000000; /* timestamp in ms */
	}
}

static void
b_mkv_player_locate_index(bmkv_player_t player)
{
	unsigned i;
	if(!player->mkv_file.validate.seekhead) {
		return;
	}
	for(i=0;i<player->mkv_file.seekhead.Seek.nelems;i++) {
		bmkv_SeekElement *entry = &BMKV_TABLE_ELEM(player->mkv_file.seekhead.Seek,bmkv_SeekElement,i);
		if(!(entry->validate.SeekPosition && entry->validate.SeekID)) {
			continue;
		}
		if(entry->SeekID == BMKV_CUES_ID) {
			off_t offset;
			bmkv_header header;
			offset = entry->SeekPosition + player->mkv_file.segment.start;
			bfile_cached_segment_set(&player->cue.cache, offset, B_MKV_MAX_ELEMENT_HEAD); /* simulate segment so we could use reserve function */
			if(bfile_cached_segment_reserve_min(&player->cue.cache, B_MKV_MAX_ELEMENT_HEAD, B_MKV_MIN_ELEMENT_HEAD) || 1) {
				if(bmkv_parse_header(&player->cue.cache.cursor, &header)) {
					unsigned count;
					bfile_cached_segment_set(&player->cue.cache, offset + batom_cursor_pos(&player->cue.cache.cursor), header.size);
					player->cue.validate = true;
					BDBG_MSG(("%s:%#lx found index at %u:%u", "b_mkv_player_locate_index", (unsigned long)player, (unsigned)player->cue.cache.segment.start, (unsigned)player->cue.cache.segment.len));
					for(count=0;count<10;count++) { /* try first 10 entries */
						bmkv_CuePoint CuePoint;

						if(b_mkv_player_load_index_entry(player, &CuePoint)!=b_mkv_result_success) {
							break;
						}
						if( CuePoint.validate.CueTime) {
							player->status.bounds.first = b_mkv_player_timestamp(player, CuePoint.CueTime);
							player->status.position = player->status.bounds.first;
							BDBG_MSG(("%s:%#lx stream starts at %u", "b_mkv_player_locate_index", (unsigned long)player, (unsigned)player->status.bounds.first));
							if(player->status.bounds.first < 10 * BMEDIA_PLAYER_POS_SCALE) {
								/* reset bounds */
								player->status.bounds.first = 0;
								player->status.position = 0;
							}
							b_mkv_player_CuePoint_shutdown(&CuePoint);
							break;
						}
						b_mkv_player_CuePoint_shutdown(&CuePoint);
					}
				} else {
					b_mkv_player_index_error(player);
					BDBG_WRN(("%s:%#lx error parsing cue(index) head at %u", "b_mkv_player_locate_index", (unsigned long)player, (unsigned)offset));
				}
			} else {
				BDBG_WRN(("%s:%#lx can't read cue(index) table at %u", "b_mkv_player_locate_index", (unsigned long)player, (unsigned)offset));
			}
			break;
		}
	}
	return;
}


static int
b_mkv_player_open_file(bmkv_player_t player)
{
	batom_pipe_t pipe_mkv;
	int rc;
	unsigned i;

	BDBG_OBJECT_ASSERT(player, bmkv_player_t);
	player->cluster.offset_next = 0;
	player->cluster.cluster_valid = false;
	player->cluster.block_valid = false;
	player->cluster.first_found = false;
	player->cluster.seek_to_index = false;
	player->cluster.resync_video = false;
	player->frame_buf_off = 0;
    player->frame_buf_persistent_alloc = 0;
    player->eof_reached = false;
    player->cluster.async.busy = false;
    player->cluster.async.continuation_active = false;

	player->cue.validate = false;
	BKNI_Memset(&player->cluster.mkv_cluster, 0, sizeof(player->cluster.mkv_cluster));
	BLST_S_INIT(&player->tracks);

	rc = bmkv_file_parser_init(&player->mkv_file, player->factory);
	if(rc!=0) {
		BDBG_ERR(("%s: %#lx can't initialize parser", "b_mkv_player_open_file", (unsigned long)player));
		goto err_mkv;
	}
	rc = -1;

	pipe_mkv  = batom_pipe_create(player->factory);
	if(!pipe_mkv) {
		BDBG_ERR(("%s: %#lx can't create pipe", "b_mkv_player_open_file", (unsigned long)player));
		goto err_pipe;
	}


	rc = bmkv_file_parser_parse(&player->mkv_file, player->index_buffer, pipe_mkv);

	player->cluster.offset_next = player->mkv_file.segment.start;

	if(player->mkv_file.validate.seekhead) {
		bool has_cluster_id = false;
		const bmkv_SeekElement *cluster_seek = NULL;

		/* find seekhead that points to clusters */
		/* 1. Test whether first seekhead have pointers to cluster */
		for(i=0;i<player->mkv_file.seekhead.Seek.nelems;i++) {
			bmkv_SeekElement *entry = &BMKV_TABLE_ELEM(player->mkv_file.seekhead.Seek,bmkv_SeekElement,i);
			if(!(entry->validate.SeekPosition && entry->validate.SeekID)) {
				continue;
			}
			switch(entry->SeekID) {
			case BMKV_CLUSTER_ID:
				has_cluster_id = true;
				break;
			case BMKV_SEEKHEAD_ID:
				if(!cluster_seek) {
					cluster_seek = entry;
				}
				break;
			}
		}
		player->cluster.offset_next = player->mkv_file.segment.start;
		if(cluster_seek && !has_cluster_id) {
			bfile_segment seek_segment;
			/* there are links to other seeks table, load one */
			if(b_mkv_player_fill_segment(player, cluster_seek, cluster_seek->SeekID, &seek_segment)) {
				/* assume this table has links to cluster */
				player->mkv_file.cluster_seekhead = seek_segment;
				player->cluster.offset_next = 0;
				goto found_cluster_seekhead;
			}
		} 
		if(!has_cluster_id) {
			bfile_segment_clear(&player->mkv_file.cluster_seekhead);
			BDBG_WRN(("%s: %#lx can't locate index for the cluster location", "b_mkv_player_open_file", (unsigned long)player));

#if defined(HUMAX_PLATFORM_BASE)
/*************************
*	Error Event occurs when mkv header doesn't have SeekID field.
*	It's not supposed to happen because there's no problem to play!
*************************/
#else
			b_mkv_player_index_error(player);
#endif
		}
	}

found_cluster_seekhead:

	if(player->mkv_file.segment_info.validate.TimecodeScale && player->mkv_file.segment_info.TimecodeScale>0) {
		if(player->mkv_file.cluster_seekhead.len) {
			bfile_cached_segment_set(&player->seek_table.cache, player->mkv_file.cluster_seekhead.start, player->mkv_file.cluster_seekhead.len);
		}
		b_mkv_player_locate_index(player);
		rc = 0;
	}
	batom_pipe_flush(pipe_mkv);
	batom_pipe_destroy(pipe_mkv);
err_pipe:
	bmkv_file_parser_shutdown(&player->mkv_file);
err_mkv:
	return rc;
}

static void *
b_mkv_player_frame_buf_alloc(bmkv_player_t player, size_t size)
{
	void *result = &player->frame_buf[player->frame_buf_off];

	if(player->frame_buf_off + size < (sizeof(player->frame_buf) - player->frame_buf_persistent_alloc)) {
		player->frame_buf_off += size;
		return result;
	}
    BDBG_WRN(("b_mkv_player_frame_buf_alloc:%p out of memory %u,%u,%u", player, size, player->frame_buf_off, (sizeof(player->frame_buf) - player->frame_buf_persistent_alloc)));
	return NULL;
}

static void *
b_mkv_player_frame_buf_alloc_persistent(bmkv_player_t player, size_t size)
{
	void *result = &player->frame_buf[sizeof(player->frame_buf) - size];

	if(player->frame_buf_off <= (sizeof(player->frame_buf) - size)) {
		player->frame_buf_persistent_alloc += size;
		return result;
	}
	return NULL;
}

static void
b_mkv_player_frame_buf_clear(bmkv_player_t player)
{
	BDBG_MSG_TRACE(("b_mkv_player_frame_buf_clear:%#lx used %u[%u]", (unsigned long)player, player->frame_buf_off, sizeof(player->frame_buf)));
	player->frame_buf_off = 0;
	return;
}

static bool
b_mkv_player_is_a_keyframe(bmkv_player_t player)
{
    if (player->cluster.mkv_cluster.validate.BlockGroup) {
        if (player->cluster.group.validate.Block) {
            return !player->cluster.group.validate.ReferenceBlock;
        } else {
           return player->cluster.group.data.flags&0x80;
        }
    } else if (player->cluster.mkv_cluster.validate.SimpleBlock) {
        return player->cluster.group.data.flags&0x80;
    } else {
        return false;
    }
}

static bmedia_video_pic_type
b_mkv_player_get_pic_type(bmkv_player_t player, struct b_mkv_player_track *track, batom_cursor cursor)
{
	bmedia_video_pic_type pic_type = bmedia_video_pic_type_unknown;

    if (!b_mkv_player_is_a_keyframe(player)) {
	    switch(track->codec_type) {
        case b_mkv_codec_type_vfw_vc1_ap:
		    pic_type = bmedia_vc1ap_read_pic_type(&cursor, &track->codec.vfw.codec.vc1.seq_hdr_info);
		    break;
        case b_mkv_codec_type_mpeg4asp:
            pic_type = bmpeg4_video_read_pic_type(&cursor);
            break;
        default:
            BDBG_WRN(("b_mkv_player_get_pic_type:%#lx unable to determine frame type for codec type %u", (unsigned long)player, track->codec_type));
            break;
        }
    } else {
        pic_type = bmedia_video_pic_type_I;
    }

	return pic_type;
}

static bool
b_mkv_player_track_initialize_pts_reorder_queue(bmkv_player_t player, struct b_mkv_player_track *track, const bmkv_TrackEntry *mkv_track)
{
	BSTD_UNUSED(player);
    BSTD_UNUSED(mkv_track);

    BDBG_MSG_TRACE(("b_mkv_player_track_initialize_pts_reorder_queue:%#lx Enabling PTS reordering", (unsigned long)player));

    track->pts_queue.last_timestamp = 0;

    track->pts_queue.pipe_needs_reorder  = batom_pipe_create(player->factory);
	if(!track->pts_queue.pipe_needs_reorder) {
		goto err_pipe_needs_reorder;
	}

    track->pts_queue.pipe_reorder_done  = batom_pipe_create(player->factory);
	if(!track->pts_queue.pipe_reorder_done) {
		goto err_pipe_reorder_done;
	}

	track->pts_queue.accum_reorder = batom_accum_create(player->factory);
	if(!track->pts_queue.accum_reorder) {
		goto err_pts_accum;
	}

    return true;

err_pts_accum:
	batom_pipe_destroy(track->pts_queue.pipe_reorder_done);
err_pipe_reorder_done:
	batom_pipe_destroy(track->pts_queue.pipe_needs_reorder);
err_pipe_needs_reorder:
    BDBG_ERR(("b_mkv_player_track_initialize_pts_reorder_queue:%#lx Enabling PTS reordering FAILED!", (unsigned long)player));
    return false;
}

static void
b_mkv_track_player_perform_pts_reorder(bmkv_player_t player, struct b_mkv_player_track *track)
{
	batom_t atom;
	bmedia_player_pos temp, last_timestamp;
    b_mkv_pts_reorder_info *reorder_atom_info;

	BSTD_UNUSED(player);
	BDBG_ASSERT(track->pts_queue.pipe_reorder_done);

    for(last_timestamp = track->pts_queue.last_timestamp;NULL!=(atom = batom_pipe_pop(track->pts_queue.pipe_needs_reorder));){
        reorder_atom_info = (b_mkv_pts_reorder_info*)batom_userdata(atom);
        BDBG_ASSERT(reorder_atom_info);

		temp = reorder_atom_info->timestamp;
		reorder_atom_info->timestamp = last_timestamp;
		last_timestamp = temp;
        batom_pipe_push(track->pts_queue.pipe_reorder_done, atom);
    }

	return;
}

static bool
b_mkv_player_reorder_pts_get_data(bmkv_player_t player, struct b_mkv_player_track *track, bmedia_player_pos *timestamp, bool key_frame_only)
{
    batom_t atom;
    b_mkv_pts_reorder_info *reorder_atom_info;

    BSTD_UNUSED(player);
	BDBG_ASSERT(track->pts_queue.accum_reorder);
	BDBG_ASSERT(track->pts_queue.pipe_reorder_done);

    if (!key_frame_only) {
        atom = batom_pipe_pop(track->pts_queue.pipe_reorder_done);
        if (atom) {
            reorder_atom_info = (b_mkv_pts_reorder_info*)batom_userdata(atom);
            BDBG_ASSERT(reorder_atom_info);

            /* Clear the accum to ensure only one frame in the accum at a time */
            batom_accum_clear(track->pts_queue.accum_reorder);
            batom_accum_add_atom(track->pts_queue.accum_reorder, atom);

            /* Set the current timestamp to match the frame in the accum */
            *timestamp = reorder_atom_info->timestamp;
            batom_release(atom);
        }
    }
    return batom_accum_len(track->pts_queue.accum_reorder) ? true : false;
}

static bool
b_mkv_player_reorder_pts_add_data(bmkv_player_t player, batom_cursor *cursor, struct b_mkv_player_track *track, bmedia_player_pos timestamp, bool key_frame_only)
{
	batom_cursor frame_start;
    batom_t atom = NULL;
    unsigned i;
    bmedia_video_pic_type pic_type;
    b_mkv_pts_reorder_info reorder_atom_info;

    BSTD_UNUSED(track);
    BSTD_UNUSED(player);
	BDBG_ASSERT(track->pts_queue.accum_reorder);

    batom_accum_clear(track->pts_queue.accum_reorder);

    for(i=0;i<player->cluster.group.data.nframes;i++) {
        BATOM_CLONE(&frame_start, cursor);

        /* Create an atom for this frame and add it to the PTS reorder queue */
        if(batom_cursor_skip(cursor, player->cluster.group.data.frames[i]) !=  player->cluster.group.data.frames[i]) {
            goto err_payload;
        }

        /* If we are in a key frame only mode, just push this directly onto the output accum */
        if (key_frame_only) {
            batom_accum_append(track->pts_queue.accum_reorder, player->cluster.cache.accum, &frame_start, cursor);
            continue;
        }

        /* Determine this frames picture type and flush the PTS queue if necessary*/
        pic_type = b_mkv_player_get_pic_type(player, track, frame_start);
        if(pic_type != bmedia_video_pic_type_B){
            /* If this is not a B frame then all the frames on the queue are ready to be reordered */
            b_mkv_track_player_perform_pts_reorder(player, track);
        }

        /* Initialize frame info */
        reorder_atom_info.timestamp = timestamp;

        /* Create an atom based on this frame so it can be added to the reorder queue */
        batom_accum_append(track->pts_queue.accum_reorder, player->cluster.cache.accum, &frame_start, cursor);
        atom = batom_from_accum(track->pts_queue.accum_reorder, &b_mkv_asp_data_atom, &reorder_atom_info);
        batom_pipe_push(track->pts_queue.pipe_needs_reorder, atom);
        batom_accum_clear(track->pts_queue.accum_reorder);

        /* Update PTS queue values */
        track->pts_queue.last_timestamp = timestamp;
    }

    return true;
err_payload:
    return false;
}

static void
b_mkv_track_player_flush_pts_reorder_queue(bmkv_player_t player, struct b_mkv_player_track *track)
{
	BSTD_UNUSED(player);
    batom_pipe_flush(track->pts_queue.pipe_reorder_done);
    batom_pipe_flush(track->pts_queue.pipe_needs_reorder);
    batom_accum_clear(track->pts_queue.accum_reorder);
	return;
}

static void
b_mkv_track_player_destroy_pts_reorder_queue(bmkv_player_t player, struct b_mkv_player_track *track)
{
	BSTD_UNUSED(player);

    b_mkv_track_player_flush_pts_reorder_queue(player, track);

    if (track->pts_queue.accum_reorder) {
	    batom_accum_destroy(track->pts_queue.accum_reorder);
    }

    if (track->pts_queue.pipe_reorder_done) {
	    batom_pipe_destroy(track->pts_queue.pipe_reorder_done);
    }

    if (track->pts_queue.pipe_needs_reorder) {
	    batom_pipe_destroy(track->pts_queue.pipe_needs_reorder);
    }
}

static bool
b_mkv_player_prepare_vfw_track(bmkv_player_t player, struct b_mkv_player_track *track, const bmkv_TrackEntry *mkv_track, const bmkv_TrackEntryVideo *Video)
{
	batom_vec vec;
	batom_cursor cursor;

	BSTD_UNUSED(player);
	BSTD_UNUSED(Video);

	if(! (mkv_track->validate.CodecPrivate && mkv_track->CodecPrivate.data && mkv_track->CodecPrivate.data_len>=sizeof(bmedia_bitmapinfo))) {
		return false;
	}
	batom_vec_init(&vec, mkv_track->CodecPrivate.data,  mkv_track->CodecPrivate.data_len);
	batom_cursor_from_vec(&cursor, &vec, 1);
    /* V_MS/VFW/FOURCC ID http://haali.cs.msu.ru/mkv/codecs.pdf */
    if (!bmedia_read_bitmapinfo(&track->codec.vfw.bitmap_info, &cursor)) {
        return false;
    }

    return true;
}

static bool
b_mkv_player_prepare_vfw_vc1ap_track(bmkv_player_t player, struct b_mkv_player_track *track, const bmkv_TrackEntry *mkv_track, const bmkv_TrackEntryVideo *Video)
{
	batom_vec vec;
	batom_cursor cursor;
	batom_checkpoint chk;
    void *seq_hdr_buf;
    unsigned seq_hdr_len;

	BSTD_UNUSED(player);
	BSTD_UNUSED(Video);

	if(! (mkv_track->validate.CodecPrivate && mkv_track->CodecPrivate.data && mkv_track->CodecPrivate.data_len>=sizeof(bmedia_bitmapinfo))) {
		return false;
	}
	batom_vec_init(&vec, mkv_track->CodecPrivate.data,  mkv_track->CodecPrivate.data_len);
	batom_cursor_from_vec(&cursor, &vec, 1);

    /* Skip the bitmap info header at the front of the private data */
    batom_cursor_skip(&cursor, sizeof(bmedia_bitmapinfo));
	batom_cursor_save(&cursor, &chk);

    /* The sequence header occurs after the bitmap info for VC1AP */
    seq_hdr_len = batom_cursor_size(&cursor);
	if(batom_cursor_byte(&cursor)!=0) {
		seq_hdr_len--;
		BDBG_MSG(("WVC1/WVCA detected with nonzero first byte, ignoring first byte"));
	} else {
		batom_cursor_rollback(&cursor, &chk);
	}

    /* Copy the VC1 sequence header into a buffer to be used during playback */
	seq_hdr_buf = b_mkv_player_frame_buf_alloc_persistent(player, seq_hdr_len);
	if(!seq_hdr_buf) {
		return false;
	}

    if (batom_cursor_copy(&cursor, seq_hdr_buf, seq_hdr_len) != seq_hdr_len) {
		return false;
    }
    batom_vec_init(&track->codec.vfw.codec.vc1.seq_hdr, seq_hdr_buf, seq_hdr_len);

    /* Parse the vc1ap seq header */
    batom_cursor_from_vec(&cursor, &track->codec.vfw.codec.vc1.seq_hdr, 1);
    if (!bmedia_vc1ap_read_info(&track->codec.vfw.codec.vc1.seq_hdr_info, &cursor)) {
		return false;
    }

    return true;
}

static bool
b_mkv_player_prepare_mkvaac_track(bmkv_player_t player, struct b_mkv_player_track *track, const bmkv_TrackEntry *mkv_track, const bmkv_TrackEntryAudio *audio)
{
	bmedia_info_aac aac;
	unsigned word;

	BSTD_UNUSED(audio);
	BSTD_UNUSED(player);

	if(! (mkv_track->validate.CodecPrivate && mkv_track->CodecPrivate.data && mkv_track->CodecPrivate.data_len>=2)) {
		aac.profile = 2;				
		bmedia_info_aac_set_sampling_frequency_index(&aac, (unsigned)audio->SamplingFrequency);
		bmedia_info_aac_set_channel_configuration(&aac, audio->Channels);
	} else {
	word = ((unsigned)((uint8_t*)mkv_track->CodecPrivate.data)[0]<<8) | ((uint8_t*)mkv_track->CodecPrivate.data)[1];
	aac.profile =  B_GET_BITS(word, 15, 11);
	if(aac.profile==0) {
		return false;
	}
	aac.sampling_frequency_index = B_GET_BITS(word, 10, 7);
	aac.channel_configuration = B_GET_BITS(word, 6, 3);
	}
	bmedia_adts_header_init_aac(&track->codec.aac, &aac);
	return true;
}

static bool
b_mkv_player_prepare_vorbis_track(bmkv_player_t player, struct b_mkv_player_track *track, const bmkv_TrackEntry *mkv_track, const bmkv_TrackEntryAudio *audio)
{
    unsigned packet_count;
    unsigned packet_size[3];
    unsigned i;
    size_t left;
    batom_cursor cursor;
    batom_vec vec;
    bmedia_waveformatex_header wf;
    size_t wf_len;

	BSTD_UNUSED(audio);
	BSTD_UNUSED(player);

    /* http://www.matroska.org/technical/specs/codecid/index.html */
	if(! (mkv_track->validate.CodecPrivate && mkv_track->CodecPrivate.data && mkv_track->CodecPrivate.data_len>=1 && mkv_track->CodecPrivate.data_len<65536)) {
        return false;
	} 
    BATOM_VEC_INIT(&vec, mkv_track->CodecPrivate.data, mkv_track->CodecPrivate.data_len);
    batom_cursor_from_vec(&cursor, &vec, 1);
    packet_count = batom_cursor_byte(&cursor);
    if(BATOM_IS_EOF(&cursor)) { goto error; }

    if(packet_count!=B_MKV_VORBIS_HEADERS-1) {
        BDBG_WRN(("%s:%#lx invalid packet_count:%u(%u)",  "b_mkv_player_prepare_vorbis_track", (unsigned long)player, packet_count, 2));
        goto error;
    }
    packet_size[packet_count] = 0;
    for(i=0;i<packet_count;i++) {
        int byte;
        unsigned frame_size = 0;
        do {
            byte = batom_cursor_next(&cursor);
            if(byte==BATOM_EOF) { goto error; }
            frame_size += (unsigned)byte;
        } while(byte==0xFF);
        packet_size[i] = frame_size;
        packet_size[packet_count] += frame_size;
    }
    left = batom_cursor_size(&cursor);
    if(packet_size[packet_count] > left) {
        return false;
    }
    packet_size[packet_count] = left - packet_size[packet_count];
    packet_count++;
    bmedia_init_waveformatex(&wf);
    if(audio->validate.SamplingFrequency) {
        wf.nSamplesPerSec = audio->SamplingFrequency;
    }
    if(audio->validate.Channels) {
        wf.nChannels = audio->Channels;
    }
    if(audio->validate.BitDepth) {
        wf.wBitsPerSample = audio->BitDepth;
    }
    wf.wFormatTag = 0xFFFF;
    for(wf.cbSize=0,i=0;i<packet_count;i++) {
        B_MEDIA_SAVE_UINT16_BE(track->codec.vorbis.headerSizes[i], packet_size[i]);
        wf.cbSize += packet_size[i] + sizeof(track->codec.vorbis.headerSizes[i]);;
        BATOM_VEC_INIT(track->codec.vorbis.meta + 1 + i*2, track->codec.vorbis.headerSizes[i], sizeof(track->codec.vorbis.headerSizes[i]));
        BATOM_VEC_INIT(track->codec.vorbis.meta + 1 + i*2+1, cursor.cursor, packet_size[i]);
        batom_cursor_skip(&cursor, packet_size[i]);
    }
    wf_len=bmedia_write_waveformatex(track->codec.vorbis.waveFormatEx, &wf);
    BDBG_ASSERT(wf_len==sizeof(track->codec.vorbis.waveFormatEx));
    BATOM_VEC_INIT(track->codec.vorbis.meta + 0, track->codec.vorbis.waveFormatEx, sizeof(track->codec.vorbis.waveFormatEx));

	return true;
error:
    return false;
}

static const uint8_t b_nal_prefix[]=
{
#if B_MKV_ES_ONLY	
	0, /* for compability with other tool */
#endif
	0,0,1
};

static bool
b_mkv_player_prepare_avc_track(bmkv_player_t player, struct b_mkv_player_track *track, const bmkv_TrackEntry *mkv_track, const bmkv_TrackEntryVideo *Video)
{
	BSTD_UNUSED(player);
	BSTD_UNUSED(Video);

	if(! (mkv_track->validate.CodecPrivate && mkv_track->CodecPrivate.data && mkv_track->CodecPrivate.data_len>=8)) {
		return false;
	}
    if(! bmedia_read_h264_meta(&track->codec.avc.meta, mkv_track->CodecPrivate.data,  mkv_track->CodecPrivate.data_len)) {
		return false;
    }
	return true;
}

static int
b_mkv_player_map_tracks(bmkv_player_t player)
{
	unsigned i;
	struct b_mkv_player_track *track;

	if(!player->mkv_file.validate.tracks || !player->mkv_file.trackData.validate.TrackEntry) {
		return -1;
	}
	player->status.bounds.last = player->status.bounds.first + b_mkv_player_timestamp(player, player->mkv_file.segment_info.Duration);
	player->cue.index = bmedia_index_create(player->status.bounds.last - player->status.bounds.first, sizeof(player->cue.cache.accum_offset), B_MKV_PLAYER_INDEX_ENTRIES);
	if(!player->cue.index) {
		BDBG_ERR(("%s: can't create CUE index", "b_mkv_player_map_tracks"));
		return -1;
	}
	if(player->seek_table.cache.segment.len) {
		player->seek_table.index = bmedia_index_create(player->status.bounds.last - player->status.bounds.first, sizeof(player->seek_table.cache.accum_offset), B_MKV_PLAYER_INDEX_ENTRIES);
		if(!player->seek_table.index) {
			BDBG_ERR(("%s: can't create SEEK index", "b_mkv_player_map_tracks"));
			return -1;
		}
	}

	for(track=NULL,i=0;i<player->mkv_file.trackData.TrackEntry.nelems;i++) {
		const bmkv_TrackEntry *mkv_track = &BMKV_TABLE_ELEM(player->mkv_file.trackData.TrackEntry,bmkv_TrackEntry,i);
		uint8_t pes_id;

		if( !(mkv_track->validate.TrackNumber && mkv_track->validate.TrackTimecodeScale && mkv_track->TrackTimecodeScale>0)) {
			BDBG_WRN(("%s:%#lx invalid selected track %u", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)i));
			continue;
		}
		if(!bmedia_player_stream_test(&player->stream, mkv_track->TrackNumber)) {
			BDBG_MSG(("%s:%#lx not selected track %u", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber));
			continue;
		}
		if(!track) {
			track = BKNI_Malloc(sizeof(*track));
			if(!track) {
				BDBG_ERR(("%s:%#lx can't allocate track (%u) bytes", "b_mkv_player_map_tracks", (unsigned long)player, sizeof(*track)));
				break;
			}
			track->codec_type = b_mkv_codec_type_unknown;
			track->hdr_len = 0;
			BLST_S_INSERT_HEAD(&player->tracks, track, next);
		}
		pes_id = bmedia_player_stream_get_id(&player->stream, mkv_track->TrackNumber);
		BMEDIA_PES_INFO_INIT(&track->pes_info, pes_id);
		track->TimecodeScale = (unsigned)(mkv_track->TrackTimecodeScale * player->mkv_file.segment_info.TimecodeScale);
		track->track_no = mkv_track->TrackNumber;
		track->mkv_track = mkv_track;
		track->bounded_pes = false;
		track->first_frame = true;
		track->seek_sync = false;
		track->pts_reorder_enabled = false;
        bmkv_file_parser_process_encoding_info(mkv_track, &track->encoding_info);
        if(!track->encoding_info.supported && !bmkv_IsTrackAuxiliary(mkv_track)) {
		    BDBG_WRN(("%s:%#lx unsupported encoding for track %u", "b_mkv_player_map_tracks", (unsigned long)player, mkv_track->TrackNumber));
            continue;
        }
		if(mkv_track->validate.Video) {
			const bmkv_TrackEntryVideo *video;

			if(mkv_track->Video.nelems!=1) {
				BDBG_WRN(("%s:%#lx unsupported video entry %u:'%s'", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, mkv_track->CodecID));
				continue;
			}
			video = &BMKV_TABLE_ELEM(mkv_track->Video,bmkv_TrackEntryVideo, 0);
			BDBG_ASSERT(video);

			if(bmkv_IsTrackVideoAvc(mkv_track)) {
				BDBG_MSG(("%s:%#lx AVC video track %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
				if(!b_mkv_player_prepare_avc_track(player, track, mkv_track, video)) {
                    track->codec.avc.meta.sps.no=track->codec.avc.meta.pps.no=0;
                    track->codec.avc.meta.nalu_len = 0;
					BDBG_WRN(("%s:%#lx AVC video track invalid format %u:(%#lx) assume raw H.264 ES payload", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
				} 
				BDBG_MSG(("%s:%#lx AVC video track %u:(%#lx) NALU len:%u sps:%u nps:%u", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track, track->codec.avc.meta.nalu_len, track->codec.avc.meta.sps.no, track->codec.avc.meta.pps.no));
				track->codec_type = b_mkv_codec_type_avc;
			} else if(bmkv_IsTrackVideoMpeg2(mkv_track)) {
				track->codec_type = b_mkv_codec_type_unknown;
			} else if(bmkv_IsTrackVideoMpeg4Asp(mkv_track)) {
				BDBG_MSG(("%s:%#lx MPEG4 ASP video track %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
				track->codec_type = b_mkv_codec_type_mpeg4asp;
				track->pts_reorder_enabled = player->config.reorder_timestamps;
			} else if(bmkv_IsTrackVideoVp8(mkv_track)) {
				BDBG_MSG(("%s:%#lx VP8 video track %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
			    track->bounded_pes = true;
				track->codec_type = b_mkv_codec_type_vp8;
			} else if(bmkv_IsTrackVideoVFW(mkv_track)) {
				BDBG_MSG(("%s:%#lx VFW video track %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
				if(!b_mkv_player_prepare_vfw_track(player, track, mkv_track, video)) {
					BDBG_WRN(("%s:%#lx VFW video track invalid format %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
					continue;
				}

				if(BMEDIA_FOURCC_DIVX5_CODEC(track->codec.vfw.bitmap_info.biCompression) || BMEDIA_FOURCC_XVID_CODEC(track->codec.vfw.bitmap_info.biCompression) || BMEDIA_FOURCC_3IVX_CODEC(track->codec.vfw.bitmap_info.biCompression) ) {
					track->codec_type = b_mkv_codec_type_vfw_mpeg4asp;
				} else if(BMEDIA_FOURCC_VC1AP_CODEC(track->codec.vfw.bitmap_info.biCompression)) {
					if(!b_mkv_player_prepare_vfw_vc1ap_track(player, track, mkv_track, video)) {
						BDBG_WRN(("%s:%#lx VFW video track invalid format %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
						continue;
					}
					track->codec_type = b_mkv_codec_type_vfw_vc1_ap;
				    track->pts_reorder_enabled = player->config.reorder_timestamps;
				} else if(BMEDIA_FOURCC_H264_CODEC(track->codec.vfw.bitmap_info.biCompression)) {
					track->codec_type = b_mkv_codec_type_vfw_avc;
					track->pts_reorder_enabled = false;
				} else {
					BDBG_WRN(("%s:%#lx VFW video track invalid format %u:(%#lx) " BMEDIA_FOURCC_FORMAT, "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track, BMEDIA_FOURCC_ARG(track->codec.vfw.bitmap_info.biCompression)));
					continue;
				}

			} else {
				BDBG_WRN(("%s:%#lx unsupported video track %u:'%s'", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, mkv_track->CodecID));
				continue;
			}

			if(track->pts_reorder_enabled && !b_mkv_player_track_initialize_pts_reorder_queue(player, track, mkv_track)) {
				BDBG_WRN(("%s:%#lx Unable to enable PTS reorder queue %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
				continue;
			}
		} else if(mkv_track->validate.Audio) {
			const bmkv_TrackEntryAudio *audio;
			unsigned sampling_frequency;
			bmedia_info_aac aac_info;

			track->bounded_pes = true;
			if(mkv_track->Audio.nelems!=1) {
				BDBG_WRN(("%s:%#lx unsupported audio entry %u:'%s'", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, mkv_track->CodecID));
				continue;
			}
			audio = &BMKV_TABLE_ELEM(mkv_track->Audio,bmkv_TrackEntryAudio, 0);
			BDBG_ASSERT(audio);
			if( !(audio->validate.SamplingFrequency && audio->validate.Channels)) {
				BDBG_WRN(("%s:%#lx unsupported audio information %u:'%s'", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, mkv_track->CodecID));
				continue;
			}
			sampling_frequency = 2*((((unsigned)audio->SamplingFrequency) + 1)/2); /* round sampling frequency */

			if(bmkv_IsTrackAudioMkvAac(mkv_track)) {

				BDBG_MSG(("%s:%#lx MKV AAC audio track %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
				if(!b_mkv_player_prepare_mkvaac_track(player, track, mkv_track, audio)) {
					BDBG_WRN(("%s:%#lx MKV AAC audio track invalid format %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
					continue;
				}
#if 0
				if( !(bmedia_info_aac_set_channel_configuration(&aac, audio->Channels) && bmedia_info_aac_set_sampling_frequency_index(&aac, sampling_frequency))) {
					BDBG_WRN(("%s:%#lx AAC audio track invalid format %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
					continue;
				}
#endif
				track->codec_type = b_mkv_codec_type_aac;
			} else if(bmkv_IsTrackAudioAac(mkv_track, &aac_info)) {
				BDBG_MSG(("%s:%#lx AAC audio track %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
				bmedia_adts_header_init_aac(&track->codec.aac, &aac_info);
				track->codec_type = b_mkv_codec_type_aac;
			} else if(bmkv_IsTrackAudioAc3(mkv_track) || bmkv_IsTrackAudioAc3Plus(mkv_track)) {
                bool ac3_plus = bmkv_IsTrackAudioAc3Plus(mkv_track);
				BDBG_MSG(("%s:%#lx AC3%s audio track %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, ac3_plus?"+":"", (unsigned)mkv_track->TrackNumber, (unsigned long)track));
				track->codec_type = ac3_plus? b_mkv_codec_type_ac3: b_mkv_codec_type_ac3_plus;
                track->codec.ac3.frame_sync = b_mkv_ac3_frame_sync_unknown;
			} else if(bmkv_IsTrackAudioDts(mkv_track)) {
				BDBG_MSG(("%s:%#lx DTS audio track %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
				track->codec_type = b_mkv_codec_type_dts;
			} else if(bmkv_IsTrackAudioMpeg1Layer3(mkv_track) || bmkv_IsTrackAudioMpeg1Layer1_2(mkv_track)) {
				BDBG_MSG(("%s:%#lx MPEG audio track %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
				track->codec_type = b_mkv_codec_type_mpeg_audio;
			} else if(bmkv_IsTrackAudioVorbis(mkv_track)) {
   				BDBG_MSG(("%s:%#lx Vorbis audio track %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
				if(!b_mkv_player_prepare_vorbis_track(player, track, mkv_track, audio)) {
					BDBG_WRN(("%s:%#lx Vorbis audio track invalid format %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
					continue;
				}
				track->codec_type = b_mkv_codec_type_vorbis;
            } else if(bmkv_IsTrackAudioACM(mkv_track)) {
                batom_vec vec;
                batom_cursor cursor;
                bmedia_waveformatex wf;
                
                BDBG_MSG(("%s:%#lx ACM audio track %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
                BATOM_VEC_INIT(&vec, mkv_track->CodecPrivate.data,  mkv_track->CodecPrivate.data_len);
                batom_cursor_from_vec(&cursor, &vec, 1);
                if (bmedia_read_waveformatex(&wf, &cursor)) {
                    if(BMEDIA_WAVFMTEX_AUDIO_MPEG(&wf) || BMEDIA_WAVFMTEX_AUDIO_MP3(&wf)) {
                        track->codec_type = b_mkv_codec_type_mpeg_audio;
                    } else if(BMEDIA_WAVFMTEX_AUDIO_DTS(&wf)) {
                        track->codec_type = b_mkv_codec_type_dts;
                    } else {
                        BDBG_WRN(("%s:%#lx unsupported audio track %u:%u", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, wf.wFormatTag));
                    }
                } else {
                    BDBG_WRN(("%s:%#lx invalid audio track %u:'%s'", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, mkv_track->CodecID));
                }
            } else {
				BDBG_WRN(("%s:%#lx unsupported audio track %u:'%s'", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, mkv_track->CodecID));
				continue;
			}
		} else {			
			if(bmkv_IsTrackAuxiliary(mkv_track)){
   				BDBG_MSG(("%s:%#lx auxilary track %u:(%#lx)", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber, (unsigned long)track));
				track->bounded_pes = true;
                track->codec_type = b_mkv_codec_type_auxilary;
			}
			else{
			    BDBG_WRN(("%s:%#lx unknown track %u type", "b_mkv_player_map_tracks", (unsigned long)player, (unsigned)mkv_track->TrackNumber));
				continue;
			}
		}

		track = NULL; /* signal that track was parsed */
	}
	if(track) {
		BLST_S_REMOVE_HEAD(&player->tracks, next);
		BKNI_Free(track);
	}
	if(BLST_S_FIRST(&player->tracks)) {
		return 0;
	}
	return -1;
}

bmkv_player_t
bmkv_player_create(bfile_io_read_t fd, const bmedia_player_config *config, const bmedia_player_stream *stream)
{
	bmkv_player_t player;
	bfile_buffer_cfg buffer_cfg;
	int rc;

	BDBG_ASSERT(fd);
	BDBG_ASSERT(config);
	BDBG_ASSERT(stream && stream->format == bstream_mpeg_type_mkv);

	if(!config->buffer) {
		BDBG_ERR(("%s: buffer has to be provided", "bmkv_player_create"));
		goto err_buffer;
	}
	player = BKNI_Malloc(sizeof(*player));
	if (!player) {
		BDBG_ERR(("%s: can't allocate %u bytes", "bmkv_player_create", sizeof(*player)));
		goto err_alloc;
	}
	BDBG_OBJECT_INIT(player, bmkv_player_t);
	player->config = *config;
	player->stream = *stream;
	player->status.direction = 0;
	player->status.bounds.first = 0;
	player->status.bounds.last = 0;
	player->status.index_error_cnt = 0;
	player->status.data_error_cnt = 0;
	player->time_scale = BMEDIA_TIME_SCALE_BASE;
	player->status.position = 0;
	player->cue.index = NULL;
	player->seek_table.index = NULL;
	BLIFO_INIT(&player->trick.rewind_frames, player->key_frames_buf, B_MKV_PLAYER_TRICK_ENTRIES);

	player->factory = batom_factory_create(bkni_alloc, 64);
	if(!player->factory) {
		goto err_factory;
	}

	bfile_buffer_default_cfg(&buffer_cfg);
	buffer_cfg.buf = player->file_buffer;
	buffer_cfg.buf_len = sizeof(player->file_buffer);
	buffer_cfg.nsegs = buffer_cfg.buf_len / BIO_BLOCK_SIZE;
	buffer_cfg.fd = fd;
	player->index_buffer = bfile_buffer_create(player->factory, &buffer_cfg);
	if(!player->index_buffer) {
		BDBG_ERR(("%s: %#lx can't create buffer", "bmkv_player_create", (unsigned long)player));
		goto err_buf;
	}

	rc = bfile_cached_segment_init(&player->cluster.cache, player->config.buffer, player->factory, B_MKV_PLAYER_DATA_BLOCK_SIZE);
	if(rc!=0) {
		goto err_cache_cluster;
	}
	rc = bfile_cached_segment_init(&player->cue.cache, player->index_buffer, player->factory, B_MKV_PLAYER_INDEX_BLOCK_SIZE);
	if(rc!=0) {
		goto err_cache_cue;
	}
	rc = bfile_cached_segment_init(&player->seek_table.cache, player->index_buffer, player->factory, B_MKV_PLAYER_INDEX_BLOCK_SIZE);
	if(rc!=0) {
		goto err_cache_seek;
	}
	player->accum_dest = batom_accum_create(player->factory);
	if(!player->accum_dest) {
		goto err_accum_dest;
	}
	player->accum_temp = batom_accum_create(player->factory);
	if(!player->accum_temp) {
		goto err_accum_temp;
	}

	if(b_mkv_player_open_file(player)<0) {
		goto err_index;
	}
	if(b_mkv_player_map_tracks(player)<0) {
		goto err_tracks;
	}

	return player;

err_tracks:
err_index:
	bmkv_file_parser_release(&player->mkv_file);
	batom_accum_destroy(player->accum_temp);
err_accum_temp:
	batom_accum_destroy(player->accum_dest);
err_accum_dest:
	if(player->seek_table.index) {
		bmedia_index_destroy(player->seek_table.index);
	}
	bfile_cached_segment_shutdown(&player->seek_table.cache);
err_cache_seek:
	if(player->cue.index) {
		bmedia_index_destroy(player->cue.index);
	}
	bfile_cached_segment_shutdown(&player->cue.cache);
err_cache_cue:
	bfile_cached_segment_shutdown(&player->cluster.cache);
err_cache_cluster:
	bfile_buffer_destroy(player->index_buffer);
err_buf:
	batom_factory_destroy(player->factory);
err_factory:
	BKNI_Free(player);
err_alloc:
err_buffer:
	return NULL;
}

void 
bmkv_player_destroy(bmkv_player_t player)
{
	struct b_mkv_player_track *track;
	BDBG_OBJECT_ASSERT(player, bmkv_player_t);

	bmkv_file_parser_release(&player->mkv_file);
	while(NULL!=(track=BLST_S_FIRST(&player->tracks))) {
		BLST_S_REMOVE_HEAD(&player->tracks, next);
		if(track->mkv_track->validate.Video && track->pts_reorder_enabled) {
		    b_mkv_track_player_destroy_pts_reorder_queue(player, track);
        }
		BKNI_Free(track);
	}

	batom_accum_destroy(player->accum_temp);
	batom_accum_destroy(player->accum_dest);

	if(player->seek_table.index) {
		bmedia_index_destroy(player->seek_table.index);
	}
	bfile_cached_segment_shutdown(&player->seek_table.cache);

	if(player->cue.index) {
		bmedia_index_destroy(player->cue.index);
	}
	bfile_cached_segment_shutdown(&player->cue.cache);
	bfile_cached_segment_shutdown(&player->cluster.cache);
	bfile_buffer_destroy(player->index_buffer);
	batom_factory_destroy(player->factory);
	BDBG_OBJECT_DESTROY(player, bmkv_player_t);
	BKNI_Free(player);

	return;
}

static bool
b_mkv_player_parse_Block_header(bmkv_player_t player, batom_cursor *cursor)
{
	int last;

	player->cluster.group.data.track = bmkv_parse_unsigned64(cursor);
	player->cluster.group.data.timecode = bmkv_read_signed(cursor,2);
	last = batom_cursor_next(cursor);
	player->cluster.group.data.flags = (uint8_t)last;
	return last != BATOM_EOF;
}

static bool
b_mkv_player_parse_Block(bmkv_player_t player, batom_cursor *cursor, unsigned blockSize)
{
	int nframes;
	int i;
	unsigned frame_size;		
	size_t elem_start;
	size_t payload_size;
#if BDBG_DEBUG_BUILD
    static const char type[][12]={"no", "Xiph", "fixed-size", "EBML"};
    BSTD_UNUSED(type);
#endif

	elem_start = batom_cursor_pos(cursor);
	if(!b_mkv_player_parse_Block_header(player, cursor)) {
		goto err_eof;
	}

    BDBG_MSG_TRACE(("%s:%#lx track:%u", "b_mkv_player_parse_Block", (unsigned long)player, (unsigned)player->cluster.group.data.track));
    BDBG_MSG_TRACE(("%s:%#lx Timecode:%d", "b_mkv_player_parse_Block", (unsigned long)player, (int)player->cluster.group.data.timecode));
    BDBG_MSG_TRACE(("%s:%#lx Flags:%#x %s %s lacing %s %s", "b_mkv_player_parse_Block", (unsigned long)player, (unsigned)player->cluster.group.data.flags, (player->cluster.group.data.flags&0x80)?"KEY":"", (player->cluster.group.data.flags&0x10)?"INVISIBLE":"", type[0x3&(player->cluster.group.data.flags>>1)], (player->cluster.group.data.flags&0x1)?"DISCARDABLE":""));

	payload_size = blockSize - (batom_cursor_pos(cursor) - elem_start);
	i = 0x3&(player->cluster.group.data.flags>>1);
	if(i==0) { /* no lacing */
		BDBG_MSG_TRACE(("%#s:%#lx no lacing frame[0]=%u", "b_mkv_player_parse_Block", (unsigned long)player, (unsigned)payload_size));
		if(bmkv_BlockGroup_alloc(&player->cluster.group, 1)<0) {
			goto err_alloc;
		}
        player->cluster.group.data.frames[i] = payload_size;
		player->cluster.group.data.nframes = 1;
	} else {
		/* read number of frames */
		nframes = batom_cursor_next(cursor);
		if(nframes==BATOM_EOF) {
			goto err_eof;
		}
		nframes++;
		BDBG_MSG_TRACE(("%#s:%#lx nframes=%d", "b_mkv_player_parse_Block", (unsigned long)player, nframes));
		if(bmkv_BlockGroup_alloc(&player->cluster.group, (unsigned)nframes)<0) {
			goto err_alloc;
		}
		if(i==2) { /* fixed-size lacing */
			frame_size = (payload_size-1)/nframes;
            BDBG_MSG_TRACE(("%#s:%#lx fixed-size frames[0..%u]=%u", "b_mkv_player_parse_Block", (unsigned long)player, nframes-1, frame_size));
			for(i=0;i<nframes-1;i++) {
				player->cluster.group.data.frames[i] = frame_size;
			}
		} else {
			if(i==3) { /* EBML lacing */
				frame_size = bmkv_parse_unsigned64(cursor);
				player->cluster.group.data.frames[0] = frame_size;
				for(i=1;i<nframes-1;i++) {
					frame_size = frame_size+(int)bmkv_parse_signed64(cursor);
                    BDBG_MSG_TRACE(("%#s:%#lx EBML frame[%u]=%u", "b_mkv_player_parse_Block", (unsigned long)player, i, frame_size));
					player->cluster.group.data.frames[i] = frame_size;
				}
			} else { /* i==1 ->  Xiph lacing */
				for(i=0;i<nframes-1;i++) {
					int byte;

					frame_size = 0;
					do {
						byte = batom_cursor_next(cursor);
						if(byte==BATOM_EOF) {
							goto err_eof;
						}
						frame_size += (unsigned)byte;
					} while(byte==0xFF);
					player->cluster.group.data.frames[i]=frame_size;
                    BDBG_MSG_TRACE(("%#s:%#lx Xiph frame[%u]=%u", "b_mkv_player_parse_Block", (unsigned long)player, i, frame_size));
				}
			}
		}
		/* calculate size of the last frame */
		payload_size = blockSize - (batom_cursor_pos(cursor) - elem_start);
		for(i=0;i<nframes-1;i++) {
			payload_size -= player->cluster.group.data.frames[i];
		}
		player->cluster.group.data.frames[nframes-1] = payload_size;
        BDBG_MSG_TRACE(("%#s:%#lx last frame[%u]=%u", "b_mkv_player_parse_Block", (unsigned long)player, nframes-1, payload_size));
	}
	return true;

err_alloc:
err_eof:
	return false;
}

static bool
b_mkv_player_parse_SimpleBlock(bmkv_player_t player, batom_cursor *cursor)
{
    BDBG_MSG_TRACE(("b_mkv_player_parse_SimpleBlock: %#lx", (unsigned long)player));
    return b_mkv_player_parse_Block(player, cursor, player->cluster.mkv_cluster.SimpleBlock);
}

static void
b_mkv_player_parse_block_payload(bmkv_player_t player)
{
	batom_cursor block_data;

	BDBG_ASSERT(player->cluster.block_valid);
	BATOM_CLONE(&block_data, &player->cluster.block_cursor);
	player->cluster.block_valid = false;
	player->cluster.group.validate.Block = player->cluster.blockheader.validate.Block;
	player->cluster.group.validate.BlockVirtual = player->cluster.blockheader.validate.BlockVirtual;
	if(player->cluster.mkv_cluster.validate.BlockGroup) {
		if(player->cluster.group.validate.Block) {
			if(b_mkv_player_parse_Block(player, &block_data, player->cluster.group.Block )) {
				player->cluster.block_valid = true;
				BATOM_CLONE(&player->cluster.block_cursor, &block_data);
			}
		} else if(player->cluster.group.validate.BlockVirtual) {
			BDBG_WRN(("BlockVirtual not supported"));
		}
	} else if(player->cluster.mkv_cluster.validate.SimpleBlock) {
        if(b_mkv_player_parse_SimpleBlock(player, &block_data)) {
            player->cluster.block_valid = true;
            BATOM_CLONE(&player->cluster.block_cursor, &block_data);
        }
	}
	return;
}

static bool
b_mkv_player_next_block_blockgroup(bmkv_player_t player, bool parse_payload)
{
	size_t elem_offset;
	size_t parse_offset;
	BDBG_MSG_TRACE(("%s:%#lx", "b_mkv_player_next_block_blockgroup", (unsigned long)player));
	bmkv_BlockGroup_init(&player->cluster.group);
	elem_offset = batom_cursor_pos(&player->cluster.cache.cursor);
	parse_offset = elem_offset;
	do {
		batom_cursor block_data;

		BDBG_MSG_TRACE(("%s:%#x parsing group %u..%u", "b_mkv_player_next_block_blockgroup", (unsigned long)player, (unsigned)parse_offset-elem_offset, (unsigned)player->cluster.mkv_cluster.BlockGroup));
		if(!bmkv_BlockGroup_parse(&player->cluster.cache.cursor, player->cluster.mkv_cluster.BlockGroup-(parse_offset-elem_offset), &player->cluster.group)) {
			BDBG_WRN(("%s:%#x error parsing BlockGroup %u..%u", "b_mkv_player_next_block_blockgroup", (unsigned long)player, (unsigned)parse_offset-elem_offset, (unsigned)player->cluster.mkv_cluster.BlockGroup));
			b_mkv_player_data_error(player);
			goto err_parse_blockgroup;
		}
		BATOM_CLONE(&block_data, &player->cluster.cache.cursor);
		if(player->cluster.group.validate.Block) {
			if(parse_payload) {
				if(b_mkv_player_parse_Block(player, &block_data, player->cluster.group.Block )) {
					player->cluster.block_valid = true;
					BATOM_CLONE(&player->cluster.block_cursor, &block_data);
				} else {
					BDBG_WRN(("%s:%#x error parsing Block %u..%u", "b_mkv_player_next_block_blockgroup", (unsigned long)player, (unsigned)parse_offset-elem_offset, (unsigned)player->cluster.mkv_cluster.BlockGroup));
					b_mkv_player_data_error(player);
				}
			} else {
				if(b_mkv_player_parse_Block_header(player, &block_data)) {
					/* save information for further use */
					player->cluster.blockheader.validate.Block = player->cluster.group.validate.Block;
					player->cluster.blockheader.validate.BlockVirtual = player->cluster.group.validate.BlockVirtual;
					player->cluster.block_valid = true;
					BATOM_CLONE(&player->cluster.block_cursor, &player->cluster.cache.cursor);
				}
			}
			batom_cursor_skip(&player->cluster.cache.cursor, player->cluster.group.Block);
		} else if(player->cluster.group.validate.BlockVirtual) {
			batom_cursor_skip(&player->cluster.cache.cursor, player->cluster.group.BlockVirtual);
			BDBG_WRN(("BlockVirtual not supported"));
		}	 
		parse_offset = batom_cursor_pos(&player->cluster.cache.cursor);
	} while((parse_offset-elem_offset)<player->cluster.mkv_cluster.BlockGroup);
	return true;
err_parse_blockgroup:
	return false;
}

static void
b_mkv_player_return_no_data(bmkv_player_t player)
{
	bmedia_player_entry entry;
    BDBG_MSG(("b_mkv_player_return_no_data:%#lx", (unsigned long)player));
    bmedia_player_init_entry(&entry);
    player->cluster.async.continuation_active = true;
    player->cluster.async.busy = false;
    entry.type = bmedia_player_entry_type_no_data;
	BDBG_ASSERT(player->config.atom_ready);
	player->config.atom_ready(player->config.cntx, &entry);
    return;
}


static void 
b_mkv_player_next_block_blockgroup_complete(void *p, bfile_segment_async_result async_result)
{
	bmkv_player_t player = p;

	BDBG_MSG_TRACE(("b_mkv_player_next_block_blockgroup_complete:%#lx %u", (unsigned long)player, (unsigned)async_result));
	BDBG_OBJECT_ASSERT(player, bmkv_player_t);

	switch(async_result) {
	case bfile_segment_async_result_success:
		break;
    case bfile_segment_async_result_no_data:
        b_mkv_player_return_no_data(player);
        return;
	case bfile_segment_async_result_error:
		BDBG_WRN(("b_mkv_player_next_block_blockgroup_complete:%#lx can't read data")); 
		b_mkv_player_data_error(player);
		/* fall through */
	default:
		goto err_read;
	}
	if(!b_mkv_player_next_block_blockgroup(player, player->cluster.async.next_block.parse_payload)) {goto err_parse_blockgroup;}
	player->cluster.async.next_block.block_complete(player, bfile_segment_async_result_success);
	return;
err_read:
err_parse_blockgroup:
	player->cluster.async.next_block.block_complete(player, bfile_segment_async_result_error);
	return;
}

static bool
b_mkv_player_next_simpleblock(bmkv_player_t player, bool parse_payload)
{
    batom_cursor block_data;

	BDBG_MSG_TRACE(("%s:%#lx", "b_mkv_player_next_simpleblock", (unsigned long)player));

    bmkv_BlockGroup_init(&player->cluster.group);

    BATOM_CLONE(&block_data, &player->cluster.cache.cursor);
    if(parse_payload) {
        if(b_mkv_player_parse_SimpleBlock(player, &block_data)) {
            player->cluster.block_valid = true;
            BATOM_CLONE(&player->cluster.block_cursor, &block_data);
        } else {
            BDBG_WRN(("%s:%#x error parsing SimpleBlock %u", "b_mkv_player_next_simpleblock", (unsigned long)player,  (unsigned)player->cluster.mkv_cluster.SimpleBlock));
        }
    } else {
        if(b_mkv_player_parse_Block_header(player, &block_data)) {
            /* save information for further use */
            player->cluster.block_valid = true;
		    BATOM_CLONE(&player->cluster.block_cursor, &player->cluster.cache.cursor);
        }
    }
    batom_cursor_skip(&player->cluster.cache.cursor, player->cluster.mkv_cluster.SimpleBlock);
	return true;
}

static void 
b_mkv_player_next_simpleblock_complete(void *p, bfile_segment_async_result async_result)
{
	bmkv_player_t player = p;

	BDBG_MSG_TRACE(("b_mkv_player_next_simpleblock_complete:%#lx", (unsigned long)player));
	BDBG_OBJECT_ASSERT(player, bmkv_player_t);

	if(async_result!=bfile_segment_async_result_success) {goto err_read;}
	if(!b_mkv_player_next_simpleblock(player, player->cluster.async.next_block.parse_payload)) {goto err_parse_simpleblock;}
	player->cluster.async.next_block.block_complete(player, bfile_segment_async_result_success);
	return;

err_read:
err_parse_simpleblock:
	player->cluster.async.next_block.block_complete(player, bfile_segment_async_result_error);
	return;
}

static bfile_segment_async_result
b_mkv_player_next_block_async(bmkv_player_t player, bool parse_payload, void (*block_complete)(bmkv_player_t player, bfile_segment_async_result result))
{
	BDBG_MSG_TRACE(("%s:%#lx parsing block data at %u", "b_mkv_player_next_block_async", (unsigned long)player, (unsigned)(player->cluster.cache.segment.start + player->cluster.cache.accum_offset)));
	player->cluster.block_valid = false;
	player->cluster.async.next_block.parse_payload = parse_payload;
	player->cluster.async.next_block.block_complete = block_complete;
    player->cluster.async.continuation_type = b_mkv_data_continuation_next_block;

	if(player->cluster.mkv_cluster.validate.BlockGroup) {
		bfile_segment_async_result async_result = bfile_cached_segment_async_reserve(&player->cluster.cache, player->cluster.mkv_cluster.BlockGroup, b_mkv_player_next_block_blockgroup_complete, player);
		if(async_result!=bfile_segment_async_result_success) {return async_result;}
		if(!b_mkv_player_next_block_blockgroup(player, parse_payload)) {goto err_parse_blockgroup;}
	} else if(player->cluster.mkv_cluster.validate.SimpleBlock) {
   		bfile_segment_async_result async_result = bfile_cached_segment_async_reserve(&player->cluster.cache, player->cluster.mkv_cluster.SimpleBlock, b_mkv_player_next_simpleblock_complete, player);
		if(async_result!=bfile_segment_async_result_success) {return async_result;}
		if(!b_mkv_player_next_simpleblock(player, parse_payload)) {goto err_parse_simpleblock;}
	}
	return bfile_segment_async_result_success;

err_parse_simpleblock:
err_parse_blockgroup:
	return bfile_segment_async_result_error;
}

static void
b_mkv_player_fetch_block_data(bmkv_player_t player,bfile_segment_async_result async_result)
{
	BDBG_MSG_TRACE(("b_mkv_player_fetch_block_data:%#lx", (unsigned long)player));
	if(async_result==bfile_segment_async_result_error) {
		player->cluster.fetch_block_result = b_mkv_fetch_block_result_err_block;
	}
	player->cluster.async.fetch_block.block_complete(player, async_result);
	return;
}

static bfile_segment_async_result 
b_mkv_player_fetch_block_parse_async(bmkv_player_t player)
{
	bfile_segment_async_result async_result;
	BDBG_MSG_TRACE(("%s:%#lx", "b_mkv_player_fetch_block_parse_async", (unsigned long)player));
	if(!bmkv_Cluster_parse(&player->cluster.cache.cursor, (size_t)(player->cluster.cache.segment.len - bfile_cached_segment_tell(&player->cluster.cache)), &player->cluster.mkv_cluster)) {
		BDBG_WRN(("%s:%#lx error parsing cluster %u..%u at:%lu", "b_mkv_player_fetch_block_parse_async", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
		player->cluster.fetch_block_result = b_mkv_fetch_block_result_err_parse_cluster;
		goto error;
	}
	if( ! (player->cluster.mkv_cluster.validate.BlockGroup || player->cluster.mkv_cluster.validate.SimpleBlock)) {
   		BDBG_WRN(("%s:%#lx error no data in block %u..%u at:%lu", "b_mkv_player_fetch_block_parse_async", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
		player->cluster.fetch_block_result = b_mkv_fetch_block_result_err_no_data;
		goto error;
	}
	async_result = b_mkv_player_next_block_async(player, player->cluster.async.fetch_block.parse_payload, b_mkv_player_fetch_block_data);
	if(async_result==bfile_segment_async_result_error) {
		BDBG_WRN(("%s:%#lx error parsing block %u..%u at:%lu", "b_mkv_player_fetch_block_parse_async", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
		b_mkv_player_data_error(player);
		player->cluster.fetch_block_result = b_mkv_fetch_block_result_err_read;
		goto error;
	}
	BDBG_MSG_TRACE(("%s:%#lx -> %u", "b_mkv_player_fetch_block_parse_async", (unsigned long)player, (unsigned)async_result));
	return async_result;
error:
	return bfile_segment_async_result_error;
}

static void
b_mkv_player_frame_async_step(b_mkv_frame_step state, bmkv_player_t player, bfile_segment_async_result async_result)
{
    BDBG_ASSERT(player->cluster.async.busy);
    BDBG_ASSERT(async_result != bfile_segment_async_result_async);
    async_result = player->cluster.async.frame.step(state, player,  async_result);
    if(async_result != bfile_segment_async_result_async) {
        player->cluster.async.frame.frame_complete(player, async_result);
    }
}

static void 
b_mkv_player_fetch_block_header_complete(void *p, bfile_segment_async_result async_result)
{
	bmkv_player_t player = p;

	BDBG_MSG_TRACE(("%s:%#lx %u", "b_mkv_player_fetch_block_header_complete", (unsigned long)player, (unsigned)async_result));
	BDBG_OBJECT_ASSERT(player, bmkv_player_t);

    BDBG_ASSERT(async_result!=bfile_segment_async_result_async);
    switch(async_result) {
    case bfile_segment_async_result_no_data:
        b_mkv_player_return_no_data(player);
        return;
    case bfile_segment_async_result_success:
        break;
    default:
        goto err_read;
    }

	async_result = b_mkv_player_fetch_block_parse_async(player);
    switch(async_result) {
    case bfile_segment_async_result_async:
        break;
    case bfile_segment_async_result_error:
        BDBG_WRN(("%s:%#lx error reading data %u..%u at:%lu", "b_mkv_player_fetch_block_header_complete", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
		b_mkv_player_data_error(player);
        /* fall through */
	case bfile_segment_async_result_eof:
    case bfile_segment_async_result_success:
        b_mkv_player_frame_async_step(b_mkv_frame_step_block, player, async_result);
        break;
    case bfile_segment_async_result_no_data:
        BDBG_ERR(("%s: %#lx unexpected 'no_data'","b_mkv_player_fetch_block_header_complete", (unsigned long)player));
        break;
    }
	return;

err_read:
	player->cluster.fetch_block_result = b_mkv_fetch_block_result_err_read;
    b_mkv_player_frame_async_step(b_mkv_frame_step_block, player, bfile_segment_async_result_error);
	return;
}


static bfile_segment_async_result
b_mkv_player_fetch_block_async(bmkv_player_t player, bool parse_payload, void (*block_complete)(bmkv_player_t player, bfile_segment_async_result result))
{
	bfile_segment_async_result async_result;
	BDBG_MSG_TRACE(("%s:%#lx", "b_mkv_player_fetch_block_async", (unsigned long)player));

	player->cluster.fetch_block_result = b_mkv_fetch_block_result_err_read;
	player->cluster.async.fetch_block.parse_payload = parse_payload;
	player->cluster.async.fetch_block.block_complete = block_complete;
    player->cluster.async.continuation_type = b_mkv_data_continuation_fetch_block;

	async_result = bfile_cached_segment_async_reserve(&player->cluster.cache, B_MKV_MAX_ELEMENT_HEAD, b_mkv_player_fetch_block_header_complete, player);
	if(async_result==bfile_segment_async_result_success) {
		return b_mkv_player_fetch_block_parse_async(player);
	} else {
		if(async_result==bfile_segment_async_result_error) {
   			BDBG_WRN(("%s:%#lx error reading data %u..%u at:%lu", "b_mkv_player_fetch_block_async", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
		}
		return async_result;
	}
}



static int
b_mkv_player_next_block(bmkv_player_t player, bool parse_payload)
{
	BDBG_MSG_TRACE(("%s:%#lx parsing block data at %u", "b_mkv_player_next_block", (unsigned long)player, (unsigned)(player->cluster.cache.segment.start + player->cluster.cache.accum_offset)));
	player->cluster.block_valid = false;
	if(player->cluster.mkv_cluster.validate.BlockGroup) {
		if(!b_mkv_player_cluster_reserve(player, player->cluster.mkv_cluster.BlockGroup)) { goto err_read; }
		if(!b_mkv_player_next_block_blockgroup(player, parse_payload)) { goto err_parse_blockgroup;}
	} else if(player->cluster.mkv_cluster.validate.SimpleBlock) {
		if(!b_mkv_player_cluster_reserve(player, player->cluster.mkv_cluster.SimpleBlock)) { goto err_read; }
        if(!b_mkv_player_next_simpleblock(player, parse_payload)) { goto err_parse_blockgroup;}
	}
	return 0;

err_read:
err_parse_blockgroup:
	return -1;
}

static b_mkv_fetch_block_result
b_mkv_player_fetch_block(bmkv_player_t player, bool parse_payload)
{
    int rc;
    size_t segment_remaining = player->cluster.cache.segment.len - bfile_cached_segment_tell(&player->cluster.cache);

    if(!b_mkv_player_cluster_reserve(player, B_MKV_MAX_ELEMENT_HEAD < segment_remaining ? B_MKV_MAX_ELEMENT_HEAD : segment_remaining)) {
        BDBG_WRN(("%s:%#lx error reading data %u..%u at:%lu", "b_mkv_player_fetch_block", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
        return b_mkv_fetch_block_result_err_read;
    }
    if(!bmkv_Cluster_parse(&player->cluster.cache.cursor, segment_remaining, &player->cluster.mkv_cluster)) {
        BDBG_WRN(("%s:%#lx error parsing cluster %u..%u at:%lu", "b_mkv_player_fetch_block", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
		b_mkv_player_data_error(player);
        return b_mkv_fetch_block_result_err_parse_cluster;
    }
    if( !(player->cluster.mkv_cluster.validate.BlockGroup || player->cluster.mkv_cluster.validate.SimpleBlock) ) {
        BDBG_WRN(("%s:%#lx error no data in block %u..%u at:%lu", "b_mkv_player_fetch_block", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
        return b_mkv_fetch_block_result_err_no_data;
    }
    rc = b_mkv_player_next_block(player, parse_payload);
    if(rc!=0) {
        BDBG_WRN(("%s:%#lx error parsing block %u..%u at:%lu", "b_mkv_player_fetch_block", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
		b_mkv_player_data_error(player);
        return b_mkv_fetch_block_result_err_block;
    }
    return b_mkv_fetch_block_result_success;
}

static bool
b_mkv_player_avc_block(bmkv_player_t player, batom_accum_t src, batom_cursor *src_cursor, const struct b_mkv_player_track *track, bool key_frame)
{
	batom_accum_t dst = player->accum_dest;
	unsigned i;

	if( track->first_frame
#if !B_MKV_ES_ONLY
/* for compability with other tool */
            || key_frame 
#endif
			) {

        if(track->codec.avc.meta.sps.no || track->codec.avc.meta.pps.no) {
            bmedia_copy_h264_meta(dst, &track->codec.avc.meta);
        }
	}
	for(i=0;i<player->cluster.group.data.nframes;i++) {
        batom_accum_t frame = src;
        batom_cursor *frame_cursor = src_cursor;
        batom_cursor nal;
        size_t left=player->cluster.group.data.frames[i];
        size_t nalu_len = track->codec.avc.meta.nalu_len;
        BDBG_MSG_TRACE(("%s:%#x avc frame %u %s","b_mkv_player_avc_block", (unsigned long)player, left, key_frame?"KEY":""));

        /* batom_cursor_dump(frame_cursor, "NAL"); */
        if(nalu_len>0) {
            while(left>nalu_len) {
                size_t nal_len;

                /* batom_cursor_dump(frame_cursor,"NAL"); */
                left -= nalu_len;
                nal_len = batom_cursor_vword_be(frame_cursor, nalu_len);
                BDBG_MSG_TRACE(("%s:%#x avc nal %u:%u","b_mkv_player_avc_block", (unsigned long)player, nal_len, left));
                if(nal_len>left) {
                    BDBG_WRN(("%s:%#x invalid nal len %u:%u","b_mkv_player_avc_block", (unsigned long)player, nal_len, left));
                    break;
                }
                BATOM_CLONE(&nal, frame_cursor);
                if(batom_cursor_skip(frame_cursor, nal_len) !=  nal_len) {
                    break;
                }
                left -= nal_len;
                batom_accum_add_range(dst, b_nal_prefix, sizeof(b_nal_prefix));
                batom_accum_append(dst, frame, &nal, frame_cursor);
            }
        } else {
            BDBG_MSG_TRACE(("%s:%#x using %u bytes of payload","b_mkv_player_avc_block", (unsigned long)player, left));
            BATOM_CLONE(&nal, frame_cursor);
            batom_cursor_skip(frame_cursor, left);
            batom_accum_append(dst, frame, &nal, frame_cursor);
        }
        if(frame!=src) {
            batom_accum_clear(frame);
        }
		if(left!=0) {
			goto err_payload;
		}
	}
	return true;

err_payload:
	return true;
	return false;
}

static bool 
b_mkv_player_vc1ap_block(bmkv_player_t player, struct b_mkv_player_track *track, bmedia_player_pos timestamp)
{
    bool insert_sc = false;
    void *pes_hdr;
    uint32_t pes_hdr_len;
	batom_cursor frame_start;
	batom_cursor sc_detect;
    bmedia_video_pic_type pic_type;
    batom_cursor cursor;
    size_t payload = batom_accum_len(track->pts_queue.accum_reorder); /* Assume there is always one frame on the input */

    if (!payload) {
        goto err_payload;
    }

    batom_cursor_from_accum(&cursor, track->pts_queue.accum_reorder);
    BATOM_CLONE(&frame_start, &cursor);
    BATOM_CLONE(&sc_detect, &frame_start);

    if(batom_cursor_skip(&cursor, payload) !=  payload) {
        goto err_payload;
    }

    pic_type = b_mkv_player_get_pic_type(player, track, frame_start);
    if (pic_type == bmedia_video_pic_type_I) {
        /* Add the sequence header to the start of this I frame */
        pes_hdr = b_mkv_player_frame_buf_alloc(player, BMEDIA_PES_HEADER_MAX_SIZE);
        if (!pes_hdr) {
            goto err_mem;
        }
        track->pes_info.pts_valid = false;
        pes_hdr_len = bmedia_pes_header_init(pes_hdr, track->codec.vfw.codec.vc1.seq_hdr.len, &track->pes_info); /* insert PES header with length  0 (unbounded) */
        batom_accum_add_range(player->accum_dest, pes_hdr, pes_hdr_len);
        batom_accum_add_vec(player->accum_dest, &track->codec.vfw.codec.vc1.seq_hdr);
    }

    /* Determine if we will need to add the VC1 start code.  If so, update the payload length */
    if (bmedia_video_read_scode(&sc_detect) == -1) {
        insert_sc = true;
        payload+=bmedia_frame_vc1.len;
    }

    /* Create and add the PES header for this frame */
    pes_hdr = b_mkv_player_frame_buf_alloc(player, BMEDIA_PES_HEADER_MAX_SIZE);
    if (!pes_hdr) {
        goto err_mem;
    }
    BMEDIA_PES_SET_PTS(&track->pes_info, bmedia_time2pts(timestamp, player->time_scale));
    pes_hdr_len = bmedia_pes_header_init(pes_hdr, payload, &track->pes_info);
    batom_accum_add_range(player->accum_dest, pes_hdr, pes_hdr_len);

    /* If needed, add the VC1 start code to this frame */
    if (insert_sc) {batom_accum_add_vec(player->accum_dest, &bmedia_frame_vc1);}

    /* Add the frame data to the output accum and clear the frame from the PTS reorder accum*/
    batom_accum_append(player->accum_dest, track->pts_queue.accum_reorder, &frame_start, &cursor);
    batom_accum_clear(track->pts_queue.accum_reorder);
    return true;

err_mem:
err_payload:
    return false;
}

static bool 
b_mkv_player_mpeg4asp_block(bmkv_player_t player, struct b_mkv_player_track *track, bmedia_player_pos timestamp)
{
    void *pes_hdr;
    uint32_t pes_hdr_len;
	batom_cursor frame_start;
    batom_cursor cursor;
    size_t frame_payload = batom_accum_len(track->pts_queue.accum_reorder); /* Assume there is always one frame on the input */
    size_t payload = frame_payload;
    size_t meta_payload = 0;

    if (!payload) {
        return false;
    }

    if(track->mkv_track->validate.CodecPrivate &&  track->mkv_track->CodecPrivate.data && track->mkv_track->CodecPrivate.data_len>0) {
        if( track->first_frame || b_mkv_player_is_a_keyframe(player))  {
            meta_payload = track->mkv_track->CodecPrivate.data_len;
            payload += meta_payload;
        }
    }

    batom_cursor_from_accum(&cursor, track->pts_queue.accum_reorder);
    BATOM_CLONE(&frame_start, &cursor);

    if(batom_cursor_skip(&cursor, frame_payload) !=  frame_payload) {
        goto err_payload;
    }

    /* Create and add the PES header for this frame */
    pes_hdr = b_mkv_player_frame_buf_alloc(player, BMEDIA_PES_HEADER_MAX_SIZE);
    if (!pes_hdr) {
        goto err_mem;
    }
    BMEDIA_PES_SET_PTS(&track->pes_info, bmedia_time2pts(timestamp, player->time_scale));
    pes_hdr_len = bmedia_pes_header_init(pes_hdr, frame_payload, &track->pes_info);
    batom_accum_add_range(player->accum_dest, pes_hdr, pes_hdr_len);
    if(meta_payload) {
        batom_accum_add_range(player->accum_dest, track->mkv_track->CodecPrivate.data, track->mkv_track->CodecPrivate.data_len);
    }

    /* Add the frame data to the output accum and clear the frame from the PTS reorder accum*/
    batom_accum_append(player->accum_dest, track->pts_queue.accum_reorder, &frame_start, &cursor);
    batom_accum_clear(track->pts_queue.accum_reorder);
    return true;

err_mem:
err_payload:
    return false;
}

static bool 
b_mkv_player_aac_block(bmkv_player_t player, batom_accum_t src, batom_cursor *cursor, struct b_mkv_player_track *track)
{
	batom_accum_t dst = player->accum_dest;
	unsigned i;

	for(i=0;i<player->cluster.group.data.nframes;i++) {
		size_t payload=player->cluster.group.data.frames[i];
		batom_cursor block;
		void *hdr  = b_mkv_player_frame_buf_alloc(player, BMEDIA_ADTS_HEADER_SIZE);

		if(!hdr) {
			goto err_no_mem;
		}
		BATOM_CLONE(&block, cursor);
		if(batom_cursor_skip(cursor, payload) !=  payload) {
			goto err_payload;
		}
        if(track->pes_info.stream_id!=1) { /* not RAW */
		    batom_accum_add_range(dst, hdr, bmedia_adts_header_fill(hdr, &track->codec.aac, payload));
        }
		batom_accum_append(dst, src, &block, cursor);
	}
	return true;
err_no_mem:
err_payload:
	return false;
}

static bool 
b_mkv_player_ac3_block(bmkv_player_t player, batom_accum_t src, batom_cursor *cursor, struct b_mkv_player_track *track)
{
	batom_accum_t dst;
	unsigned i;
	size_t payload;
	batom_cursor block;

    if(track->codec.ac3.frame_sync == b_mkv_ac3_frame_sync_unknown) {
        uint16_t temp;

	    BATOM_CLONE(&block, cursor);
        temp = batom_cursor_uint16_be(&block);
        if(!BATOM_IS_EOF(&block)) {
            track->codec.ac3.frame_sync = temp==B_AC3_SYNC ? b_mkv_ac3_frame_sync_present : b_mkv_ac3_frame_sync_missing;
            BDBG_MSG(("b_mkv_player_ac3_block;%#lx track:%u frame sync %s %#x", (unsigned long)player, track->track_no, track->codec.ac3.frame_sync==b_mkv_ac3_frame_sync_present?"present":"missing", temp));
        }
    }
	dst = player->accum_dest;
	for(payload=0,i=0;i<player->cluster.group.data.nframes;i++) {
	    BATOM_CLONE(&block, cursor);
		payload+=player->cluster.group.data.frames[i];
		if(batom_cursor_skip(cursor, player->cluster.group.data.frames[i]) !=  player->cluster.group.data.frames[i]) {
			goto err_payload;
		}
        if(track->codec.ac3.frame_sync == b_mkv_ac3_frame_sync_missing) {
	        batom_accum_add_range(dst, b_mkv_ac3_frame_sync, sizeof(b_mkv_ac3_frame_sync));
        }
	    batom_accum_append(dst, src, &block, cursor);
	}

	return true;
err_payload:
	return false;
}

static bool 
b_mkv_player_vorbis_block(bmkv_player_t player, batom_accum_t src, batom_cursor *cursor, struct b_mkv_player_track *track)
{
	batom_accum_t dst = player->accum_dest;
	unsigned i;

	for(i=0;i<player->cluster.group.data.nframes;i++) {
	    batom_cursor block;
		size_t payload=player->cluster.group.data.frames[i];
		uint8_t *hdr  = b_mkv_player_frame_buf_alloc(player, sizeof(uint32_t));

		if(!hdr) {
			goto err_no_mem;
		}

		BATOM_CLONE(&block, cursor);
		if(batom_cursor_skip(cursor, payload) !=  payload) {
			goto err_payload;
		}
        if(track->pes_info.stream_id!=1) { /* not RAW */
            unsigned j;
            B_MEDIA_SAVE_UINT32_BE(hdr, payload);
            batom_accum_add_vec(dst, &bmedia_frame_bcma);
		    batom_accum_add_range(dst, hdr, sizeof(uint32_t));
            for(j=0;j<sizeof(track->codec.vorbis.meta)/sizeof(track->codec.vorbis.meta[0]);j++) {
                batom_accum_add_vec(dst, track->codec.vorbis.meta+j);
            }
        }
		batom_accum_append(dst, src, &block, cursor);
	}
	return true;
err_no_mem:
err_payload:
	return false;
}

static bool
b_mkv_player_vp8_block(bmkv_player_t player, batom_accum_t src, batom_cursor *cursor, struct b_mkv_player_track *track)
{
	batom_accum_t dst = player->accum_dest;
	unsigned i;

	for(i=0;i<player->cluster.group.data.nframes;i++) {
	    batom_cursor block;
		size_t payload=player->cluster.group.data.frames[i];
        const size_t header_len = BMEDIA_FIELD_LEN("size", uint32_t) + BMEDIA_FIELD_LEN("type", uint16_t);
		uint8_t *hdr = b_mkv_player_frame_buf_alloc(player, header_len);

		if(!hdr) {
			goto err_no_mem;
		}

		BATOM_CLONE(&block, cursor);
		if(batom_cursor_skip(cursor, payload) !=  payload) {
			goto err_payload;
		}
        if(track->pes_info.stream_id!=1) { /* not RAW */
            B_MEDIA_SAVE_UINT32_BE(hdr, payload+header_len+bmedia_frame_bcmv.len); /* size */
            B_MEDIA_SAVE_UINT16_BE(hdr+BMEDIA_FIELD_LEN("size",uint32_t), 0); /* type */;
            batom_accum_add_vec(dst, &bmedia_frame_bcmv);
		    batom_accum_add_range(dst, hdr, header_len);
        }
		batom_accum_append(dst, src, &block, cursor);
	}
	return true;

err_no_mem:
err_payload:
	return false;
}

static bool 
b_mkv_player_other_block(bmkv_player_t player, batom_accum_t src, batom_cursor *cursor)
{
	batom_accum_t dst = player->accum_dest;
	unsigned i;
	size_t payload;
	batom_cursor block;

	BATOM_CLONE(&block, cursor);
	for(payload=0,i=0;i<player->cluster.group.data.nframes;i++) {
        size_t frame_size = player->cluster.group.data.frames[i];
		payload+=frame_size;
		if(batom_cursor_skip(cursor, frame_size) !=  frame_size) {
			goto err_payload;
		}
	}
	batom_accum_append(dst, src, &block, cursor);

	return true;
err_payload:
	return false;
}

/*
Format of PES payload for MKV auxilary data:
 
A. header. 4 bytes
  - 'b'
  - 'm'
  - 'k'
  - 'v'

B. Metadata
    3 bytes length and 1 byte type
 - length bits 23..16
 - length bits 15..8
 - length bits 7..0
 - payload type bits 7..0
   followed by the payload.

  Where length is lenght of payload only, it doesn't include 4 bytes of header and 4 bytes of  metadata.

  So if there is a 511 byte of payload, then packet data is:
  ['b' 'm' 'k' 'v'] [0 1 255] [0] [...payload...]

C. Type of the payload, on of the following:
 0 - payload
 1 - codec private data
 2 - duration (payload is 32 bits, in miliseconds, in BE format)
 3 - attachment link (copy of data from the attachment link)

Miscellaneous notes:
 codec private data sent only once
 duration is only send if it's coded into the MKV block
 payload is send last after any other associated metadata 

Example packet at 1000ms with 300 bytes of payload
  ['b' 'm' 'k' 'v'] [0 1 44] [2] [0 0 3 232] ['b' 'm' 'k' 'v'] [0 1 44] [0] [...payload...]

*/

static bool 
b_mkv_player_auxilary_block(bmkv_player_t player, batom_accum_t src, batom_cursor *cursor, struct b_mkv_player_track *track)
{
    batom_accum_t dst = player->accum_dest;
    unsigned i;
    uint8_t *data;

    if(track->first_frame && track->mkv_track) {
        if(track->mkv_track->validate.CodecPrivate && track->mkv_track->CodecPrivate.data && track->mkv_track->CodecPrivate.data_len) {
            BDBG_MSG(("b_mkv_player_auxilary_block:%#x adding %u bytes %s", (unsigned long)track, track->mkv_track->CodecPrivate.data_len, "CodecPrivate"));
            data = b_mkv_player_frame_buf_alloc(player, 4);
            if(!data) {
                goto err_alloc;
            }
            batom_accum_add_vec(dst, &b_mkv_aux_header);
            data[0] = B_GET_BITS(track->mkv_track->CodecPrivate.data_len, 23, 16);
            data[1] = B_GET_BITS(track->mkv_track->CodecPrivate.data_len, 15, 8);
            data[2] = B_GET_BITS(track->mkv_track->CodecPrivate.data_len, 7, 0);
            data[3] = B_MKV_AUX_TAG_CODEC_PRIVATE;
            batom_accum_add_range(dst, data, 4);
            batom_accum_add_range(dst, track->mkv_track->CodecPrivate.data, track->mkv_track->CodecPrivate.data_len);
        }
        if(track->mkv_track->validate.AttachmentLink) {
	    const bmkv_AttachmentLink *bmkv_attachment = &BMKV_TABLE_ELEM(track->mkv_track->AttachmentLink, bmkv_AttachmentLink, 0);
            const size_t data_len = sizeof(bmkv_attachment->AttachmentLink);
            BDBG_MSG(("b_mkv_player_auxilary_block:%#x adding %u bytes %s", (unsigned long)track, data_len, "AttachmentLink"));
            data = b_mkv_player_frame_buf_alloc(player, 4+data_len);
            if(!data) {
                goto err_alloc;
            }
            batom_accum_add_vec(dst, &b_mkv_aux_header);
            data[0] = B_GET_BITS(data_len, 23, 16);
            data[1] = B_GET_BITS(data_len, 15, 8);
            data[2] = B_GET_BITS(data_len, 7, 0);
            data[3] = B_MKV_AUX_TAG_ATTACHMENT_LINK;
            data[4] = B_GET_BITS(bmkv_attachment->AttachmentLink, 63, 56);
            data[5] = B_GET_BITS(bmkv_attachment->AttachmentLink, 55, 48);
            data[6] = B_GET_BITS(bmkv_attachment->AttachmentLink, 47, 40);
            data[7] = B_GET_BITS(bmkv_attachment->AttachmentLink, 39, 32);
            data[8] = B_GET_BITS(bmkv_attachment->AttachmentLink, 31, 24);
            data[9] = B_GET_BITS(bmkv_attachment->AttachmentLink, 23, 16);
            data[10] = B_GET_BITS(bmkv_attachment->AttachmentLink, 15, 8);
            data[11] = B_GET_BITS(bmkv_attachment->AttachmentLink, 7, 0);
            batom_accum_add_range(dst, data, 12);
        }
    }
    if(player->cluster.group.validate.BlockDuration) {
        bmedia_player_pos duration = b_mkv_player_track_time(track, player->cluster.group.BlockDuration);
        BDBG_MSG(("b_mkv_player_auxilary_block:%#x adding duration %u", (unsigned long)track, duration));
        data = b_mkv_player_frame_buf_alloc(player, 8);
        if(!data) {
            goto err_alloc;
        }
        batom_accum_add_vec(dst, &b_mkv_aux_header);
        data[0] = 0;
        data[1] = 0;
        data[2] = 4;
        data[3] = B_MKV_AUX_TAG_DURATION;
        data[4] = B_GET_BITS(duration, 31, 24);
        data[5] = B_GET_BITS(duration, 23, 16);
        data[6] = B_GET_BITS(duration, 15, 8);
        data[7] = B_GET_BITS(duration, 7, 0);
        batom_accum_add_range(dst, data, 8);
    }
    for(i=0;i<player->cluster.group.data.nframes;i++) {
        size_t payload_len;
        batom_cursor block;

        BATOM_CLONE(&block, cursor);
        payload_len = player->cluster.group.data.frames[i];
        if(batom_cursor_skip(cursor, player->cluster.group.data.frames[i]) != payload_len) {
            goto err_payload;
        }
        batom_accum_add_vec(dst, &b_mkv_aux_header);
        data = b_mkv_player_frame_buf_alloc(player, 4);
        if(!data) {
            goto err_alloc;
        }
        data[0] = B_GET_BITS(payload_len, 23, 16);
        data[1] = B_GET_BITS(payload_len, 15, 8);
        data[2] = B_GET_BITS(payload_len, 7, 0);
        data[3] = B_MKV_AUX_TAG_PAYLOAD;
        batom_accum_add_range(dst, data, 4);
        batom_accum_append(dst, src, &block, cursor);
        BDBG_MSG(("b_mkv_player_auxilary_block:%#x adding payload:%u frame:%u", (unsigned long)track, payload_len, i));
    }
    /* batom_accum_dump(dst, "AUX"); */
    return true;

err_payload:
err_alloc:
    return false;
}


static bool
b_mkv_player_decompress_block(bmkv_player_t player, batom_accum_t src, batom_cursor *src_cursor, const struct b_mkv_player_track *track, batom_accum_t *pframe, batom_cursor *frame_cursor)
{
    BDBG_ASSERT(track->encoding_info.ContentCompAlgo_valid);
    if(track->encoding_info.ContentCompAlgo==3) { /* Header Stripping */
        unsigned i;
        size_t skipped;
        batom_accum_t frame = *pframe;

        BDBG_MSG_TRACE(("%s:%#x header stripping adding %u bytes","b_mkv_player_decompress_block", (unsigned long)player, track->encoding_info.ContentCompSettings.data_len));
	    for(i=0;i<player->cluster.group.data.nframes;i++) {
            size_t frame_size=player->cluster.group.data.frames[i];

            BATOM_CLONE(frame_cursor, src_cursor);
            skipped = batom_cursor_skip(src_cursor, frame_size);
            if(skipped!=frame_size) {
                return false;
            }
            player->cluster.group.data.frames[i] += track->encoding_info.ContentCompSettings.data_len;
            batom_accum_add_range(frame, track->encoding_info.ContentCompSettings.data, track->encoding_info.ContentCompSettings.data_len);
            batom_accum_append(frame, src, frame_cursor, src_cursor);
        }
        batom_cursor_from_accum(frame_cursor, frame);
    } else  {
        BATOM_CLONE(frame_cursor, src_cursor);
        *pframe = src;
        if(track->codec_type == b_mkv_codec_type_auxilary) {
            /* skip a warning application would decompress */
        } else {
            BDBG_WRN(("%s:%#x compression %u not supported","b_mkv_player_decompress_block", (unsigned long)player, track->encoding_info.ContentCompAlgo));
        }
    }
    return true;
}


static int
b_mkv_player_make_frame(bmkv_player_t player, struct b_mkv_player_track *track, bmedia_player_pos timestamp, bmedia_player_entry *entry)
{
	bool success;
	size_t pes_hdr_len;
	void *pes_hdr;
    batom_cursor *frame_cursor,_frame_cursor;
    batom_accum_t frame_data;
    bool make_pes_stream = track->pes_info.stream_id >= 2;

	if(player->stream.decrypt_callback){
		player->stream.decrypt_callback(player->stream.cntx, &player->cluster.block_cursor, player->cluster.group.data.frames, player->cluster.group.DRMInfo, track->track_no);
	}

    bmedia_player_init_entry(entry);
    entry->content = bmedia_player_entry_content_payload;
	pes_hdr = b_mkv_player_frame_buf_alloc(player, BMEDIA_PES_HEADER_MAX_SIZE);
	if(!pes_hdr) {
		goto err_alloc;
	}
	BMEDIA_PES_SET_PTS(&track->pes_info, bmedia_time2pts(timestamp, player->time_scale));
	pes_hdr_len = bmedia_pes_header_init(pes_hdr, 0, &track->pes_info); /* insert PES header with length  0 (unbounded) */
	batom_accum_clear(player->accum_dest);
    if(make_pes_stream) {
#if !B_MKV_ES_ONLY
        if (player->config.max_pes_size==0) { /* max pes size of 0 means that the pes packet can be unbounded */
            batom_accum_add_range(player->accum_dest, pes_hdr, pes_hdr_len);
        }
#endif
    }
	if(player->cluster.seek_to_index) {
		player->cluster.seek_to_index = false;
		BDBG_ASSERT(player->seek_table.index);
		if(player->status.position >= bmedia_index_next(player->seek_table.index)+player->status.bounds.first) {
			BDBG_MSG(("%s:%#lx saving seek_table index entry %u->%u", "b_mkv_player_make_frame",  (unsigned long)player, (unsigned)player->status.position, (unsigned)player->cluster.seek_table_offset));
			bmedia_index_add(player->seek_table.index, player->status.position-player->status.bounds.first, &player->cluster.seek_table_offset);
		}
	}
    if(!track->encoding_info.ContentCompAlgo_valid) {
        frame_cursor = &player->cluster.block_cursor;
        frame_data = player->cluster.cache.accum;
        success = true;
    } else {
        frame_data = player->accum_temp;
        frame_cursor = &_frame_cursor;
        success = b_mkv_player_decompress_block(player, player->cluster.cache.accum, &player->cluster.block_cursor, track, &frame_data, &_frame_cursor);
    }
    if(success) {
        switch(track->codec_type) {
        case b_mkv_codec_type_avc:
            success = b_mkv_player_avc_block(player, frame_data, frame_cursor, track, b_mkv_player_is_a_keyframe(player));
            break;
        case b_mkv_codec_type_aac:
            success = b_mkv_player_aac_block(player, frame_data, frame_cursor, track);
            break;
        case b_mkv_codec_type_vfw_vc1_ap:
            success = b_mkv_player_vc1ap_block(player, track, timestamp);
            break;
        case b_mkv_codec_type_mpeg4asp:
            success = b_mkv_player_mpeg4asp_block(player, track, timestamp);
            break;
        case b_mkv_codec_type_auxilary:
            success = b_mkv_player_auxilary_block(player, frame_data, frame_cursor, track);
            break;
        case b_mkv_codec_type_ac3:
        case b_mkv_codec_type_ac3_plus:
            success = b_mkv_player_ac3_block(player, frame_data, frame_cursor, track);
            break;
        case b_mkv_codec_type_vorbis:
            success = b_mkv_player_vorbis_block(player, frame_data, frame_cursor, track);
            break;
        case b_mkv_codec_type_vp8:
            success = b_mkv_player_vp8_block(player, frame_data, frame_cursor, track);
            break;
        default:
            success = b_mkv_player_other_block(player, frame_data, frame_cursor);
            break;
        }
    }
    if(frame_data == player->accum_temp) {
        batom_accum_clear(frame_data);
    }
	if(success) {
		BDBG_MSG(("%s:%#lx parsed %u bytes track %u:%s timestamp:%u pts:%u", "b_mkv_player_make_frame", (unsigned long)player, batom_accum_len(player->accum_dest),  (unsigned)track->track_no, track->mkv_track->CodecID, (unsigned)timestamp, (unsigned)track->pes_info.pts));
		track->first_frame = false;
        
        if(make_pes_stream) {
            size_t pes_hdr_buf_size = sizeof(player->frame_buf) - player->frame_buf_off - player->frame_buf_persistent_alloc - 1;

            if (player->config.max_pes_size != 0){
                void *pes_hdr_buf = b_mkv_player_frame_buf_alloc(player, pes_hdr_buf_size);
                if(pes_hdr_buf==NULL) {
                    goto err_alloc;
                }
                entry->atom = bmedia_pes_subdivide_packet(player->accum_dest, &track->pes_info, player->accum_temp, pes_hdr_buf, pes_hdr_buf_size, player->config.max_pes_size);
            } else {
                size_t payload_len = batom_accum_len(player->accum_dest);
                if(track->bounded_pes && payload_len > B_MEDIA_MAX_PES_SIZE) {
                    void *pes_hdr_buf = b_mkv_player_frame_buf_alloc(player, pes_hdr_buf_size);
                    batom_cursor cursor;

                    if(pes_hdr_buf==NULL) {
                        goto err_alloc;
                    }
                    batom_cursor_from_accum(&cursor, player->accum_dest);
                    batom_cursor_skip(&cursor, pes_hdr_len);
                    batom_accum_trim(player->accum_dest, &cursor);
                    entry->atom = bmedia_pes_subdivide_packet(player->accum_dest, &track->pes_info, player->accum_temp, pes_hdr_buf, pes_hdr_buf_size, B_MEDIA_MAX_PES_SIZE);
                } else {
                    entry->atom = batom_from_accum(player->accum_dest, NULL, NULL);
                    if(track->bounded_pes) {
                        BDBG_ASSERT(payload_len > pes_hdr_len);
                        bmedia_pes_header_init(pes_hdr, payload_len - pes_hdr_len, &track->pes_info); /* correct length of PES header */
                    }
                }
            }  
        } else {
            entry->atom = batom_from_accum(player->accum_dest, NULL, NULL);
        }
	} else {
		BDBG_WRN(("%s:%#lx parsing error track %u:%s at %u", "b_mkv_player_make_frame", (unsigned long)player, (unsigned)track->track_no, track->mkv_track->CodecID, (unsigned)(player->cluster.cache.segment.start + bfile_cached_segment_tell(&player->cluster.cache))));
		batom_accum_clear(player->accum_dest);
	}
	entry->embedded = NULL;
	entry->length = 0;
	entry->type = bmedia_player_entry_type_atom;
	entry->timestamp = timestamp;
	if(entry->atom) {
		player->status.position = timestamp;
		entry->length = batom_len(entry->atom);
	}  
err_alloc:
	b_mkv_player_frame_buf_clear(player);
	return entry->atom ? 0 : -1;
}

static int
b_mkv_player_next_forward_block(bmkv_player_t player, bmedia_player_entry *entry)
{
    int rc=0;
    if(player->cluster.block_valid && player->cluster.mkv_cluster.validate.Timecode) {
        struct b_mkv_player_track *track;
	    for(track=BLST_S_FIRST(&player->tracks);track;track=BLST_S_NEXT(track, next)) {
	        if(track->track_no == player->cluster.group.data.track) {
		        bmedia_player_pos timestamp;
			    if(player->cluster.resync_video) {
			        if(track->mkv_track->validate.Video && b_mkv_player_is_a_keyframe(player)) {
				        player->cluster.resync_video = false;
                        BDBG_MSG(("%s:%#lx found video key frame", "b_mkv_player_next_forward_block", (unsigned long)player));
                    } else {
				        BDBG_MSG_TRACE(("%s:%#lx waiting for key frame", "b_mkv_player_next_forward_block", (unsigned long)player));
					    break;
                    }
                }
                timestamp = b_mkv_player_track_timestamp(player, track, player->cluster.group.data.timecode);
                if(player->status.direction>0) {
		            if(track->mkv_track->validate.Video && b_mkv_player_is_a_keyframe(player)) {
			            if( (!track->seek_sync) && timestamp < player->status.position+player->status.direction) {
                            BDBG_MSG_TRACE(("%s:%#lx filtering out block; track:%u %u:%u", "b_mkv_player_next_forward_block", (unsigned long)player, (unsigned)track->track_no, (unsigned)entry->timestamp, (unsigned)(player->status.position + player->status.direction)));
                            break;
                        }
                        BDBG_MSG(("%s:%#lx passing track:%u for fast forward", "b_mkv_player_next_forward_block", (unsigned long)player, (unsigned)track->track_no));
                    } else {
                        BDBG_MSG_TRACE(("%s:%#lx filtering out %s track:%u in fast forward", "b_mkv_player_next_forward_block", (unsigned long)player, track->mkv_track->validate.Video?"Video":"", (unsigned)track->track_no));
                        break;
                    }
                }
                if(track->seek_sync) {
                    if(timestamp < player->cluster.seek_target) {
			            BDBG_MSG_TRACE(("%s:%#lx track:%u dropping block for sync %u:%u", "b_mkv_player_next_forward_block", (unsigned long)player, (unsigned)track->track_no, (unsigned)timestamp, (unsigned)player->cluster.seek_target));
				        break;
                    }
                    BDBG_MSG_TRACE(("%s:%#lx track:%u synced after seek %u:%u", "b_mkv_player_next_forward_block", (unsigned long)player, (unsigned)track->track_no, (unsigned)timestamp, (unsigned)player->cluster.seek_target));
                    track->seek_sync = false;
                }
                if(track->pts_reorder_enabled) {
                    /* Push this frame data through the PTS reordering queue and update the timestamp to reflect the actual output frame */
                    if (!b_mkv_player_reorder_pts_add_data(player, &player->cluster.block_cursor, track, timestamp, false)) {
                        break;
                    }

                    if (!b_mkv_player_reorder_pts_get_data(player, track, &timestamp, false)) {
                        entry->type = bmedia_player_entry_type_noop;
				        break;
                    }
                }
                rc = b_mkv_player_make_frame(player, track, timestamp, entry);

                break;
            }
        }
    }
	if(player->cluster.block_valid) {
        bmkv_BlockGroup_shutdown(&player->cluster.group);
    }
    BDBG_MSG_TRACE(("%s:%#lx next block %u..%u", "b_mkv_player_next_forward_block", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len));
    return rc;
}


static int
b_mkv_player_next_forward_complete(bmkv_player_t player, bmedia_player_entry *entry)
{
    int rc=-1;

    struct b_mkv_player_track *track;
    for(track=BLST_S_FIRST(&player->tracks);track;track=BLST_S_NEXT(track, next)) {
        if(track->pts_reorder_enabled) {
            bmedia_player_pos timestamp = 0;

            /* Ensure all frames are moved through the PTS reorder queue */
            b_mkv_track_player_perform_pts_reorder(player, track);
            if (!b_mkv_player_reorder_pts_get_data(player, track, &timestamp, false)) {
                break;
            }

            /* Try to make a frame based on internal buffers */
            rc = b_mkv_player_make_frame(player, track, timestamp, entry);
            if(rc==0) {
                player->status.position = timestamp;
            }
        }
    }
    if(rc==-1) {
        entry->type = bmedia_player_entry_type_end_of_stream;
        rc = 0;
    }

    return rc;
}

static b_mkv_frame_step
b_mkv_player_next_forward_async_block(bmkv_player_t player, bfile_segment_async_result result)
{
    int rc;
    bmedia_player_entry *entry;

    switch(result) {
    case bfile_segment_async_result_success:
        break;
#if 0
    case bfile_segment_async_result_no_data:
        b_mkv_player_return_no_data(player);
        return b_mkv_frame_step_done;
#endif
    default:
	    goto err_frame;
    }

    entry = &player->cluster.async.frame.entry;
    rc = b_mkv_player_next_forward_block(player, entry);
    if(entry->atom || entry->type == bmedia_player_entry_type_end_of_stream || rc!=0) {
        return b_mkv_frame_step_done;
    }
    return b_mkv_frame_step_cluster;

err_frame:
    switch(player->cluster.fetch_block_result) {
	case b_mkv_fetch_block_result_success:
        BDBG_ASSERT(0);
        break;
	case b_mkv_fetch_block_result_err_no_data: 
	case b_mkv_fetch_block_result_err_parse_cluster: 
    case b_mkv_fetch_block_result_err_block:
		BDBG_MSG(("%s:%#lx end of cluster %u..%u at:%lu", "b_mkv_player_next_forward_async_block", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
		player->cluster.cluster_valid = false;
        return b_mkv_frame_step_cluster;

    case b_mkv_fetch_block_result_err_eof:
	case b_mkv_fetch_block_result_err_read:
        break;
    }
    return b_mkv_frame_step_done;
}

static void
b_mkv_player_next_forward_async_block_complete(bmkv_player_t player, bfile_segment_async_result result)
{
    BDBG_ASSERT(result != bfile_segment_async_result_async);
    b_mkv_player_frame_async_step(b_mkv_frame_step_block, player, result);
    return;
}


static bfile_segment_async_result
b_mkv_player_next_async_cluster(bmkv_player_t player, bool sequential)
{
    player->cluster.fetch_block_result = b_mkv_fetch_block_result_success;
    if(player->cluster.cluster_valid && bfile_cached_segment_tell(&player->cluster.cache) >= player->cluster.cache.segment.len) {
        BDBG_MSG(("%s:%#lx end of cluster %u..%u at:%lu", "b_mkv_player_next_async_cluster", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
        player->cluster.cluster_valid = false;
    }
    if(!player->cluster.cluster_valid) {
        bool endofstream;
        int rc = b_mkv_player_next_cluster(player, sequential, &endofstream);
        if(rc!=0) {
	        BDBG_MSG(("%s:%#lx error fetching cluster %u..%u at:%lu", "b_mkv_player_next_async_cluster", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
            if(!endofstream) {
                goto err_cluster;
            } else {
                player->cluster.fetch_block_result = b_mkv_fetch_block_result_err_eof;
                return bfile_segment_async_result_eof;
            }
        }
        BDBG_ASSERT(player->cluster.cluster_valid);
    }

    player->cluster.async.frame.entry.start = player->cluster.cache.segment.start + bfile_cached_segment_tell(&player->cluster.cache);
    return b_mkv_player_fetch_block_async(player, player->cluster.async.frame.parse_payload, b_mkv_player_next_forward_async_block_complete);

err_cluster:
    player->cluster.fetch_block_result = b_mkv_fetch_block_result_err_read;
    return bfile_segment_async_result_error;
}

static bfile_segment_async_result
b_mkv_player_next_forward_async_step(b_mkv_frame_step state, bmkv_player_t player, bfile_segment_async_result async_result)
{
    BDBG_MSG_TRACE(("%s:>%#lx %u:%u", "b_mkv_player_next_forward_async_step", (unsigned long)player, (unsigned)state, (unsigned)async_result)); 
    for(;;) {
        /* since we can't use mutually recursive functions because of stack-grow, instead use loop with multiple entry points */
        BDBG_MSG_TRACE(("%s:%#lx  (%u:%u)", "b_mkv_player_next_forward_async_step", (unsigned long)player, (unsigned)state, (unsigned)async_result)); 
        switch(state) {
        case b_mkv_frame_step_cluster:
            async_result = b_mkv_player_next_async_cluster(player, true);
            if(async_result == bfile_segment_async_result_async) { goto done; }
            /* keep going */
        case b_mkv_frame_step_block:
            state = b_mkv_player_next_forward_async_block(player, async_result);
            BDBG_ASSERT(state != b_mkv_frame_step_block);
            break;
        case b_mkv_frame_step_rewind_refill:
            BDBG_ASSERT(0);
            break;
        case b_mkv_frame_step_done:
             if(async_result!=bfile_segment_async_result_eof) {
                if(player->cluster.async.frame.entry.type == bmedia_player_entry_type_end_of_stream) {
                    async_result = bfile_segment_async_result_eof;
                } else if(player->cluster.async.frame.entry.atom == NULL) {
                    async_result = bfile_segment_async_result_error;
                }
            }
            goto done;
        }
        continue;
done:
        BDBG_MSG_TRACE(("%s:<%#lx -> %u", "b_mkv_player_next_forward_async_step", (unsigned long)player, (unsigned)async_result)); 
        return async_result;
    }
}


static bfile_segment_async_result
b_mkv_player_next_forward_async(bmkv_player_t player)
{
    player->cluster.async.frame.step = b_mkv_player_next_forward_async_step;
    player->cluster.async.frame.parse_payload = true;
    return b_mkv_player_next_forward_async_step(b_mkv_frame_step_cluster, player, bfile_segment_async_result_success);
}

static int
b_mkv_player_next_forward(bmkv_player_t player, bmedia_player_entry *entry)
{
	int rc=0;
	BDBG_MSG_TRACE(("%s>:%#lx", "b_mkv_player_next_forward", (unsigned long)player));
	entry->atom = NULL;
	for(;;) {
        b_mkv_fetch_block_result block_result;
		if(player->cluster.cluster_valid && bfile_cached_segment_tell(&player->cluster.cache) >= player->cluster.cache.segment.len) {
			goto end_of_data;
		}
		if(!player->cluster.cluster_valid) {
            bool endofstream;
			rc = b_mkv_player_next_cluster(player, true, &endofstream);
			if(rc!=0) {
		        BDBG_MSG(("%s:%#lx error fetching cluster %u..%u at:%lu", "b_mkv_player_next_forward", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
				goto err_cluster;
			}
			BDBG_ASSERT(player->cluster.cluster_valid);
		}

		entry->start = player->cluster.cache.segment.start + bfile_cached_segment_tell(&player->cluster.cache);
        block_result = b_mkv_player_fetch_block(player, true);
        switch(block_result) {
        case b_mkv_fetch_block_result_success:
            break;
        case b_mkv_fetch_block_result_err_eof:
            entry->type = bmedia_player_entry_type_end_of_stream;
            goto err_eof;
        default:
        case b_mkv_fetch_block_result_err_read:
            goto err_read;
        case b_mkv_fetch_block_result_err_parse_cluster:
            goto err_parse_cluster;
        case b_mkv_fetch_block_result_err_no_data:
            goto err_no_data;
        case b_mkv_fetch_block_result_err_block:
            goto err_block;
        }
        rc = b_mkv_player_next_forward_block(player, entry);
		if(entry->atom || rc!=0) {
			BDBG_MSG_TRACE(("%s>:%#lx -> %d", "b_mkv_player_next_forward", (unsigned long)player, rc));
			return rc;
		}
		continue;
err_block:
err_parse_cluster:
err_no_data:
end_of_data:
		BDBG_MSG(("%s:%#lx end of cluster %u..%u at:%lu", "b_mkv_player_next_forward", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
		player->cluster.cluster_valid = false;
	}
err_cluster:
err_read:
err_eof:

    /* Try to extract any entries from internal buffers, even of file error, this would cause to tread [some] file errors as end of stream */
    rc = b_mkv_player_next_forward_complete(player, entry);
	BDBG_MSG_TRACE(("%s>:%#lx -> %d", "b_mkv_player_next_forward", (unsigned long)player, rc));
	return rc;
}


static int
b_mkv_player_cue_seek_prepare(bmkv_player_t player, bmedia_player_pos pos)
{
	uint64_t offset;

	if(!player->cue.index || !player->cue.validate) {
		BDBG_WRN(("%s:%#lx can't seek/rewind without an index", "b_mkv_player_cue_seek_prepare", (unsigned long)player));
		goto error;
	}
	player->cue.index_pos = bmedia_index_get(player->cue.index, pos - player->status.bounds.first, &offset);
	if(player->cue.index_pos==BMEDIA_PLAYER_INVALID) {
		offset = 0;	
	} else {
		BDBG_MSG_TRACE(("%s:%#lx index pos:%u(%u) offset:%u", "b_mkv_player_cue_seek_prepare", (unsigned long)player, (unsigned)player->cue.index_pos,(unsigned)pos, (unsigned)offset));
	}
	BKNI_Memset(&player->cue.mkv_track_pos, 0, sizeof(player->cue.mkv_track_pos));
	player->cue.index_pos = bmedia_index_next(player->cue.index) + player->status.bounds.first;
	bfile_cached_segment_seek(&player->cue.cache, offset);
	player->cue.mkv_pos = pos;
	return 0;
error:
	return -1;
}

static b_mkv_result
b_mkv_player_cue_next(bmkv_player_t player)
{
	uint64_t offset;
	unsigned i;
	bmkv_CuePoint CuePoint;
    b_mkv_result result = b_mkv_result_success;

	for(;;) {
		offset = bfile_cached_segment_tell(&player->cue.cache);

		result = b_mkv_player_load_index_entry(player, &CuePoint);
		if(result!=b_mkv_result_success) {
			BDBG_MSG(("%s:%#lx can't load entry (%u)", "b_mkv_player_cue_next", (unsigned long)player, result));
            if(result==b_mkv_result_error) {
			    BDBG_WRN(("%s:%#lx can't load entry", "b_mkv_player_cue_next", (unsigned long)player));
            } 
			goto err_load;
		}
		if( CuePoint.validate.CueTime) {
			bmedia_player_pos mkv_pos = b_mkv_player_timestamp(player, CuePoint.CueTime);
			BDBG_MSG_TRACE(("%s:%#lx mkv_pos:%u", "b_mkv_player_cue_next", (unsigned long)player, (unsigned)mkv_pos));
			if(mkv_pos>=player->cue.index_pos) {
				BDBG_MSG(("%s:%#lx saving cue index entry %u->%u", "b_mkv_player_cue_next", (unsigned long)player, (unsigned)mkv_pos, (unsigned)offset));
				bmedia_index_add(player->cue.index, mkv_pos-player->status.bounds.first, &offset);
				player->cue.index_pos = bmedia_index_next(player->cue.index);
			}
			for(i=0;i<CuePoint.CueTrackPositions.nelems;i++) {
				const bmkv_CueTrackPosition *track_pos = &BMKV_TABLE_ELEM(CuePoint.CueTrackPositions,bmkv_CueTrackPosition,i);
				if(track_pos->validate.CueTrack && player->stream.master == track_pos->CueTrack && track_pos->validate.CueClusterPosition) {
					player->cue.mkv_track_pos = *track_pos;
					player->cue.mkv_pos = mkv_pos;
					b_mkv_player_CuePoint_shutdown(&CuePoint);
					return b_mkv_result_success;
				}
			}
		}
		b_mkv_player_CuePoint_shutdown(&CuePoint);
	}
err_load:
	return result;
}

static b_mkv_result
b_mkv_player_cue_seek(bmkv_player_t player, bmedia_player_pos pos)
{
	int rc;
    b_mkv_result result = b_mkv_result_success;

	rc = b_mkv_player_cue_seek_prepare(player, pos);
	if(rc!=0) {
		return b_mkv_result_error;
	}
	for(;;) {
        result = b_mkv_player_cue_next(player);
		if(result!=b_mkv_result_success) {
			break;
		}
		BDBG_MSG_TRACE(("%s:%#lx position:%u destination:%u", "b_mkv_player_cue_seek", (unsigned long)player, (unsigned)player->cue.mkv_pos, (unsigned)pos));
		if(player->cue.mkv_pos >= pos) {
			return b_mkv_result_success;
		}
	}
	return result;
}

static bmedia_player_pos
b_mkv_player_next_rewind_refill(bmkv_player_t player, bmedia_player_pos *pbase_pos)
{
    int rc;
    b_mkv_key_frame_entry *key;
    bmedia_player_pos pos;
    bmedia_player_pos rewind_jump;
    bmedia_player_pos base_pos = *pbase_pos;
    bool first_pass=true;  

	BDBG_ASSERT(player->status.direction<0);
	rewind_jump = -player->status.direction;
	if(rewind_jump < B_MKV_PLAYER_REWIND_STEP) {
		rewind_jump = B_MKV_PLAYER_REWIND_STEP;
	}
	BDBG_MSG_TRACE(("%s>:%#lx rewind_jump:%u position:%u base_pos:%u LIFO:%u", "b_mkv_player_next_rewind_refill", (unsigned long)player, (unsigned)rewind_jump, (unsigned)player->status.position, (unsigned)base_pos, (unsigned)BLIFO_READ_PEEK(&player->trick.rewind_frames)));

    while(BLIFO_READ_PEEK(&player->trick.rewind_frames)==0) { /* loop to fetch key frames locations */

	    if((!first_pass && base_pos == player->status.bounds.first) || (base_pos + 8*rewind_jump) < player->status.position) {
	        /* we either reached beginning of the file, or haven't see any frames for 10 rewind steps */
		    goto end_of_file;
        }
	    /* calclulate our destination after jump */
	    pos = base_pos;
		
	    if(player->status.bounds.first + rewind_jump < pos) {
	        pos -= rewind_jump;
        } else {
	        pos = player->status.bounds.first;
        }
        BDBG_MSG_TRACE(("%s:%#lx position:%u destination:%u", "b_mkv_player_next_rewind_refill", (unsigned long)player, (unsigned)player->status.position, (unsigned)pos));
        BDBG_ASSERT(pos>=player->status.bounds.first); /* we shall not jump outside of file limits */
        if(b_mkv_player_cue_seek(player, pos) != b_mkv_result_success) {
            rc=-1;
            goto err_seek;
        }
	    while(BLIFO_WRITE_PEEK(&player->trick.rewind_frames)) {
	        key = BLIFO_WRITE(&player->trick.rewind_frames);
		    BDBG_ASSERT(player->cue.mkv_track_pos.validate.CueClusterPosition);
            BDBG_MSG_TRACE(("%s:%#lx cue %u:%u %s", "b_mkv_player_next_rewind_refill", (unsigned long)player, (unsigned)player->cue.mkv_pos, (unsigned)player->status.position, player->cluster.resync_video?"resync":""));
		    if( (player->cue.mkv_pos > player->status.position) ||
		        (player->cue.mkv_pos == player->status.position && !player->cluster.resync_video)) {  /* after sync allow exact match */
                /* don't fill duplicate frames */
		        break;
            }
            key->time = player->cue.mkv_pos;
		    key->cluster_off = player->cue.mkv_track_pos.CueClusterPosition;
		    BLIFO_WRITE_COMMIT(&player->trick.rewind_frames,1);
            BDBG_MSG_TRACE(("%s:%#lx saving frame[%u:%u] position:%u offset:%u", "b_mkv_player_next_rewind_refill", (unsigned long)player, BLIFO_READ_PEEK(&player->trick.rewind_frames), B_MKV_PLAYER_TRICK_ENTRIES, (unsigned)key->time, (unsigned)key->cluster_off));
            if(b_mkv_player_cue_next(player)!=b_mkv_result_success) {
		        break;
            }
        } 
        base_pos = pos;
        first_pass = false;
    }
    /* calculate target frame, taking into account direction */
    pos = player->status.position;
    if(!player->cluster.resync_video) {
        BDBG_ASSERT(player->status.direction<0);
        if(pos > (unsigned)(-player->status.direction)) {
	        pos -= (-player->status.direction);
        } else {
            pos = 0;
        }
    }
    *pbase_pos = base_pos;
    return pos;

end_of_file:
err_seek:
    return BMEDIA_PLAYER_INVALID;
}

static int
b_mkv_player_next_rewind_block(bmkv_player_t player, bmedia_player_entry *entry, const b_mkv_key_frame_entry *key, bool *end_of_frames)
{
    int rc=0;
    BDBG_MSG_TRACE(("%s:%#lx %s block track:%u timecode:%u", "b_mkv_player_next_rewind_block", (unsigned long)player, player->cluster.block_valid?"":"invalid", (unsigned)player->cluster.group.data.track, (unsigned)(player->cluster.group.data.timecode+player->cluster.mkv_cluster.Timecode)));
    if(player->cluster.block_valid && player->cluster.mkv_cluster.validate.Timecode) {
        struct b_mkv_player_track *track;
	    for(track=BLST_S_FIRST(&player->tracks);track;track=BLST_S_NEXT(track, next)) {
	        bmedia_player_pos timestamp;
		    if(track->track_no != player->cluster.group.data.track) {
		        continue;
            }
		    if(track->mkv_track->validate.Video && b_mkv_player_is_a_keyframe(player)) {
                timestamp = b_mkv_player_track_timestamp(player, track, player->cluster.group.data.timecode);
                BDBG_MSG_TRACE(("%s:%#lx video frame %u(%u:%u)", "b_mkv_player_next_rewind_block", (unsigned long)player, (unsigned)timestamp, (unsigned)key->time, (unsigned)player->status.position));
			    if(timestamp > player->status.position) {
			        goto end_of_frames;
                }
                if(timestamp >= key->time) {
                    BDBG_MSG(("%s:%#lx selected video frame %u(%u:%u)", "b_mkv_player_next_rewind_block", (unsigned long)player, (unsigned)timestamp, (unsigned)key->time, (unsigned)player->status.position));
                    b_mkv_player_parse_block_payload(player);
                    if(player->cluster.block_valid) { 
                        track->seek_sync = false;
                        player->cluster.resync_video = false;
                        if(track->pts_reorder_enabled) {
                            if (!b_mkv_player_reorder_pts_add_data(player, &player->cluster.block_cursor, track, timestamp, true)) {
                                break;
                            }

                            if (!b_mkv_player_reorder_pts_get_data(player, track, &timestamp, true)) {
                                entry->type = bmedia_player_entry_type_noop;
        				        break;
                            }
                        }
                        rc = b_mkv_player_make_frame(player, track, timestamp, entry);
                    }
                }
            }
            break;
        }
    }
    *end_of_frames = false;
    bmkv_BlockGroup_shutdown(&player->cluster.group);
    BDBG_MSG_TRACE(("%s:%#lx next block %u..%u", "b_mkv_player_next_rewind_block", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len));

    return rc;
end_of_frames:
    *end_of_frames = true;
    return 0;
}

static b_mkv_frame_step
b_mkv_player_next_rewind_async_block(bmkv_player_t player, bfile_segment_async_result result)
{
    int rc;
    bmedia_player_entry *entry;
    bool end_of_frames;

    switch(result) {
    case bfile_segment_async_result_success:
        break;
#if 0
    case bfile_segment_async_result_no_data:
        b_mkv_player_return_no_data(player);
        return;
#endif
    default:
	    goto err_frame;
    }

    entry = &player->cluster.async.frame.entry;
    rc = b_mkv_player_next_rewind_block(player, entry, player->cluster.async.rewind.key, &end_of_frames);
    if(end_of_frames) {
        return b_mkv_frame_step_rewind_refill; /* jump to the next cluster */
    }

    if(entry->atom || rc!=0) {
        return b_mkv_frame_step_done;
    }
    return b_mkv_frame_step_cluster;

err_frame:
    switch(player->cluster.fetch_block_result) {
	case b_mkv_fetch_block_result_success:
        BDBG_ASSERT(0);
        break;
	case b_mkv_fetch_block_result_err_parse_cluster: 
	case b_mkv_fetch_block_result_err_no_data: 
    case b_mkv_fetch_block_result_err_block:
	case b_mkv_fetch_block_result_err_eof:
		BDBG_MSG(("%s:%#lx end of cluster %u..%u at:%lu", "b_mkv_player_next_forward_async_block", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
		player->cluster.cluster_valid = false;
        if(player->cluster.fetch_block_result==b_mkv_fetch_block_result_err_block) {
            return b_mkv_frame_step_cluster;
        } else {
            return b_mkv_frame_step_rewind_refill;
        }
	case b_mkv_fetch_block_result_err_read:
        break;
    }
    return b_mkv_frame_step_done;
}

static bfile_segment_async_result
b_mkv_player_next_rewind_async_step(b_mkv_frame_step state, bmkv_player_t player, bfile_segment_async_result async_result)
{
	bmedia_player_pos pos;
    bmedia_player_pos last_refill_pos = BMEDIA_PLAYER_INVALID;
	b_mkv_key_frame_entry *key;
    BDBG_MSG_TRACE(("%s:>%#lx %u:%u", "b_mkv_player_next_rewind_async_step", (unsigned long)player, (unsigned)state, (unsigned)async_result)); 

    for(;;) {
        BDBG_MSG_TRACE(("%s:>%#lx (%u:%u)", "b_mkv_player_next_rewind_async_step", (unsigned long)player, (unsigned)state, (unsigned)async_result)); 
        switch(state) {
        case b_mkv_frame_step_rewind_refill:
            async_result = bfile_segment_async_result_success;
            pos = b_mkv_player_next_rewind_refill(player, &player->cluster.async.rewind.base_pos);
            if(pos==BMEDIA_PLAYER_INVALID) {
                player->cluster.async.frame.entry.type = bmedia_player_entry_type_error;
                async_result = bfile_segment_async_result_eof;
                goto done;
            }
            player->cluster.async.trick.pos = pos;
            for(key=NULL;BLIFO_READ_PEEK(&player->trick.rewind_frames)>0;) {
                key = BLIFO_READ(&player->trick.rewind_frames);
                BLIFO_READ_COMMIT(&player->trick.rewind_frames,1);
			    BDBG_ASSERT(key);
    		    if(key->time <= pos) {
                    break;
                }
			    BDBG_MSG_TRACE(("%s:%#lx filtering out key entry %u(%u:%u)", "b_mkv_player_next_rewind_async_step", (unsigned long)player, (unsigned)key->time, (unsigned)pos, (unsigned)player->status.position));
                key = NULL; /* Invalidate key here incase the LIFO is emptied before a valid frame is found */
            }
            if(key==NULL) {
                BDBG_ASSERT(BLIFO_READ_PEEK(&player->trick.rewind_frames)==0);
                if (pos==last_refill_pos) {
                    BDBG_MSG_TRACE(("%s:%#lx Unable to find a frame before time %u %u:%u", "b_mkv_player_next_rewind_async_step", (unsigned long)player, (unsigned)pos, (unsigned)last_refill_pos, (unsigned)player->status.position));
                    async_result = bfile_segment_async_result_error;
                    goto done;
                }
                last_refill_pos = pos;
                break;
            }
            player->cluster.async.rewind.key = key;
            player->cluster.cluster_valid = false;
            player->cluster.offset_next = player->mkv_file.segment.start + key->cluster_off;
            /* fall through */
        case b_mkv_frame_step_cluster:
            BDBG_ASSERT(player->cluster.async.rewind.key);
            async_result = b_mkv_player_next_async_cluster(player, false);
            if(async_result == bfile_segment_async_result_async) { goto done; }
            /* fall through */
        case b_mkv_frame_step_block:
            BDBG_ASSERT(player->cluster.async.rewind.key);
            state = b_mkv_player_next_rewind_async_block(player, async_result);
            BDBG_ASSERT(state != b_mkv_frame_step_block);
            break;
        case b_mkv_frame_step_done:
            if(player->cluster.async.frame.entry.atom == NULL) {
                async_result = bfile_segment_async_result_error;
            }
            goto done;
        }
        continue;
done:
        BDBG_MSG_TRACE(("%s:<%#lx -> %u", "b_mkv_player_next_rewind_async_step", (unsigned long)player, (unsigned)async_result)); 
        return async_result;
    }
}

static bfile_segment_async_result
b_mkv_player_next_rewind_async(bmkv_player_t player)
{
    player->cluster.async.rewind.base_pos = player->status.position;
    player->cluster.async.rewind.key = NULL;
    player->cluster.async.frame.step = b_mkv_player_next_rewind_async_step;
    player->cluster.async.frame.parse_payload = false;

    return b_mkv_player_next_rewind_async_step(b_mkv_frame_step_rewind_refill, player, bfile_segment_async_result_success);
}



static int
b_mkv_player_next_rewind(bmkv_player_t player, bmedia_player_entry *entry)
{
	int rc=0;
	bmedia_player_pos base_pos, pos;
    bmedia_player_pos last_refill_pos = BMEDIA_PLAYER_INVALID;
	b_mkv_key_frame_entry *key;
	entry->atom = NULL;

	BDBG_ASSERT(player->status.direction<0);
	for(base_pos=player->status.position;;) { /* loop to load key frames */

        pos = b_mkv_player_next_rewind_refill(player, &base_pos);
        if(pos==BMEDIA_PLAYER_INVALID) {
            entry->type = bmedia_player_entry_type_end_of_stream;
            return 0;
        }

		while(BLIFO_READ_PEEK(&player->trick.rewind_frames)>0) {  /* loop to consume accumulated entries */
            bool endofstream;
			key = BLIFO_READ(&player->trick.rewind_frames);
			BLIFO_READ_COMMIT(&player->trick.rewind_frames,1);
			BDBG_ASSERT(key);
			if(key->time > pos) {
				BDBG_MSG_TRACE(("%s:%#lx filtering out key entry %u(%u:%u)", "b_mkv_player_next_rewind", (unsigned long)player, (unsigned)key->time, (unsigned)pos, (unsigned)player->status.position));
				continue;
			}
			player->cluster.offset_next = player->mkv_file.segment.start + key->cluster_off;
			rc = b_mkv_player_next_cluster(player, false, &endofstream);
			if(rc!=0) {
		        BDBG_MSG(("%s:%#lx error fetching cluster %u..%u at:%lu", "b_mkv_player_next_rewind", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
				goto err_cluster;
			}
			BDBG_ASSERT(player->cluster.cluster_valid);
			while(player->cluster.cluster_valid && bfile_cached_segment_tell(&player->cluster.cache) < player->cluster.cache.segment.len) { /* loop to read frames from the cluster */
                bool end_of_frames;
                b_mkv_fetch_block_result block_result = b_mkv_player_fetch_block(player, false);

                switch(block_result) {
                case b_mkv_fetch_block_result_success:
                    break;
                default:
                case b_mkv_fetch_block_result_err_read:
                    goto err_read;
                case b_mkv_fetch_block_result_err_parse_cluster:
                    goto err_parse_cluster;
                case b_mkv_fetch_block_result_err_no_data:
                    goto err_no_data;
                case b_mkv_fetch_block_result_err_block:
                    goto err_block;
                }
                rc = b_mkv_player_next_rewind_block(player, entry, key, &end_of_frames);
                if(end_of_frames) { goto end_of_frames; }
                if(entry->atom || rc!=0) {
			        BDBG_MSG_TRACE(("%s<:%#lx position:%u LIFO:%u", "b_mkv_player_next_rewind", (unsigned long)player, (unsigned)player->status.position, (unsigned)BLIFO_READ_PEEK(&player->trick.rewind_frames)));
				    return rc;
				}
				continue;
err_block:
err_no_data:
err_parse_cluster:
end_of_frames:
				break; /* jump to the next cluster */
			}
		}
        if (pos==last_refill_pos) {
            BDBG_MSG_TRACE(("%s:%#lx Unable to find a frame before time %u %u:%u", "b_mkv_player_next_rewind", (unsigned long)player, (unsigned)pos, (unsigned)last_refill_pos, (unsigned)player->status.position));
            goto err_refill;
        }
        last_refill_pos = pos;
	}

err_read:
err_cluster:
err_refill:
	return -1;
}

static bmedia_player_pos
b_mkv_player_next_fastforward_seek(bmkv_player_t player, bool *endofstream)
{
	int rc;
	bmedia_player_pos pos;
    b_mkv_result result;

	BDBG_ASSERT(player->status.direction>0);

    *endofstream = false;
	pos=player->status.position;
	if(!player->cluster.resync_video) {  /* after sync allow exact match */
		pos += player->status.direction;
	}
	if(pos >= player->status.bounds.last) {
        *endofstream = true;
		/* we reached end of the file */
		goto end_of_file;
	}
    result = b_mkv_player_cue_seek(player, pos);
    if(result!=b_mkv_result_success) {
        if(result==b_mkv_result_end_of_data) {
            *endofstream = true;
            /* we reached end of the file */
            goto end_of_file;
        } else {
            rc = -1;
            goto err_seek;
        }
    }
	while( player->cue.mkv_pos < pos) {
		BDBG_MSG_TRACE(("%s:%#lx filtering out frame position:%u offset:%u", "b_mkv_player_next_fastforward_seek", (unsigned long)player, (unsigned)player->cue.mkv_pos, (unsigned)player->cue.mkv_track_pos.CueClusterPosition));
		if(b_mkv_player_cue_next(player)!=b_mkv_result_success) {
			goto err_seek;
		} 
	}
	BDBG_MSG_TRACE(("%s:%#lx frame position:%u offset:%u", "b_mkv_player_next_fastforward_seek", (unsigned long)player, (unsigned)player->cue.mkv_pos, (unsigned)player->cue.mkv_track_pos.CueClusterPosition));

	player->cluster.offset_next = player->mkv_file.segment.start + player->cue.mkv_track_pos.CueClusterPosition;
    return pos;

end_of_file:
err_seek:
    return BMEDIA_PLAYER_INVALID;
}

static int
b_mkv_player_next_fastforward_block(bmkv_player_t player, bmedia_player_entry *entry, bmedia_player_pos pos)
{
    int rc=0;

    BDBG_MSG_TRACE(("%s:%#lx %s block track:%u timecode:%u", "b_mkv_player_next_fastforward_block", (unsigned long)player, player->cluster.block_valid?"":"invalid", (unsigned)player->cluster.group.data.track, (unsigned)(player->cluster.group.data.timecode+player->cluster.mkv_cluster.Timecode)));
    if(player->cluster.block_valid && player->cluster.mkv_cluster.validate.Timecode) {
        struct b_mkv_player_track *track;
	    for(track=BLST_S_FIRST(&player->tracks);track;track=BLST_S_NEXT(track, next)) {
	        bmedia_player_pos timestamp;
		    if(track->track_no != player->cluster.group.data.track) {
		        continue;
            }
            if(track->mkv_track->validate.Video && b_mkv_player_is_a_keyframe(player)) {
                timestamp = b_mkv_player_track_timestamp(player, track, player->cluster.group.data.timecode);
                BDBG_MSG_TRACE(("%s:%#lx video frame %u(%u:%u)", "b_mkv_player_next_fastforward_block", (unsigned long)player, (unsigned)timestamp, (unsigned)pos, (unsigned)player->status.position));
			    if(timestamp >= pos) {
                    BDBG_MSG(("%s:%#lx selected video frame %u(%u:%u)", "b_mkv_player_next_fastforward_block", (unsigned long)player, (unsigned)timestamp, (unsigned)pos, (unsigned)player->status.position));
				    b_mkv_player_parse_block_payload(player);
				    if(player->cluster.block_valid) { 
				        track->seek_sync = false;
					    player->cluster.resync_video = false;
                        if(track->pts_reorder_enabled) {
                            if (!b_mkv_player_reorder_pts_add_data(player, &player->cluster.block_cursor, track, timestamp, true)) {
                                break;
                            }

                            if (!b_mkv_player_reorder_pts_get_data(player, track, &timestamp, true)) {
                                entry->type = bmedia_player_entry_type_noop;
        				        break;
                            }
                        }
					    rc = b_mkv_player_make_frame(player, track, timestamp, entry);
					    if(rc==0) {
					        player->status.position = timestamp;
                        }
                    }
                }
            }
            break;
        }
        BDBG_MSG_TRACE(("%s:%#lx next block %u..%u", "b_mkv_player_next_fastforward_block", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len));
    }
    bmkv_BlockGroup_shutdown(&player->cluster.group);
    return rc;
}

static int
b_mkv_player_next_fastforward(bmkv_player_t player, bmedia_player_entry *entry)
{
	bmedia_player_pos pos;
    int rc;
    bool endofstream;

	entry->atom = NULL;
    pos = b_mkv_player_next_fastforward_seek(player, &endofstream);
    if(pos==BMEDIA_PLAYER_INVALID) {
        if(endofstream) {
            entry->type = bmedia_player_entry_type_end_of_stream;
            return 0;
        }
        goto err_seek;
    }
	rc = b_mkv_player_next_cluster(player, false, &endofstream);
	if(rc!=0) {
        BDBG_MSG(("%s:%#lx error fetching cluster %u..%u at:%lu", "b_mkv_player_next_fastforward", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
        if(!endofstream) {
            goto err_cluster;
        } else {
            entry->type = bmedia_player_entry_type_end_of_stream;
            return 0;
        }
	}
	BDBG_ASSERT(player->cluster.cluster_valid);
	while(player->cluster.cluster_valid && bfile_cached_segment_tell(&player->cluster.cache) < player->cluster.cache.segment.len) { /* loop to read frames from the cluster */
	    b_mkv_fetch_block_result block_result = b_mkv_player_fetch_block(player, false);
        switch(block_result) {
        case b_mkv_fetch_block_result_success:
            break;
        default:
        case b_mkv_fetch_block_result_err_read:
            goto err_read;
        case b_mkv_fetch_block_result_err_parse_cluster:
            goto err_parse_cluster;
        case b_mkv_fetch_block_result_err_no_data:
            goto err_no_data;
        case b_mkv_fetch_block_result_err_block:
            goto err_block;
        }
 
        rc = b_mkv_player_next_fastforward_block(player, entry, pos);
		if(entry->atom || rc!=0) {
            return rc;
        }
	}
err_block:
err_no_data:
err_parse_cluster:
err_read:
err_cluster:
err_seek:
	return -1;
}


static b_mkv_frame_step
b_mkv_player_next_fastforward_async_block(bmkv_player_t player, bfile_segment_async_result result)
{
    int rc;
    bmedia_player_entry *entry;

    switch(result) {
    case bfile_segment_async_result_success:
        break;
#if 0
    case bfile_segment_async_result_no_data:
        b_mkv_player_return_no_data(player);
        return;
#endif
    default:
	    goto err_frame;
    }

    entry = &player->cluster.async.frame.entry;
    rc = b_mkv_player_next_fastforward_block(player, entry, player->cluster.async.trick.pos);

    if(entry->atom || rc!=0) {
        return b_mkv_frame_step_done;
    }
    return b_mkv_frame_step_cluster;

err_frame:
    switch(player->cluster.fetch_block_result) {
	case b_mkv_fetch_block_result_success:
        BDBG_ASSERT(0);
        break;
	case b_mkv_fetch_block_result_err_parse_cluster: 
	case b_mkv_fetch_block_result_err_no_data: 
    case b_mkv_fetch_block_result_err_block:
		BDBG_MSG(("%s:%#lx end of cluster %u..%u at:%lu", "b_mkv_player_next_forward_async_block", (unsigned long)player, (unsigned)bfile_cached_segment_tell(&player->cluster.cache), (unsigned)player->cluster.cache.segment.len, (unsigned long)player->cluster.cache.segment.start));
		player->cluster.cluster_valid = false;
        return b_mkv_frame_step_cluster;

    case b_mkv_fetch_block_result_err_eof:
	case b_mkv_fetch_block_result_err_read:
        break;
    }
    return b_mkv_frame_step_done;
}

static bfile_segment_async_result
b_mkv_player_next_fastforward_async_step(b_mkv_frame_step state, bmkv_player_t player, bfile_segment_async_result async_result)
{
    BDBG_MSG_TRACE(("%s:>%#lx %u:%u", "b_mkv_player_next_fastforward_async_step", (unsigned long)player, (unsigned)state, (unsigned)async_result)); 

    for(;;) {
        /* since we can't use mutually recursive functions because of stack-grow, instead use loop with multiple entry points */
        BDBG_MSG_TRACE(("%s:%#lx  (%u:%u)", "b_mkv_player_next_fastforward_async_step", (unsigned long)player, (unsigned)state, (unsigned)async_result)); 
        switch(state) {
        case b_mkv_frame_step_cluster:
            async_result = b_mkv_player_next_async_cluster(player, false);
            if(async_result == bfile_segment_async_result_async) { goto done; }
            /* keep going */
        case b_mkv_frame_step_block:
            state = b_mkv_player_next_fastforward_async_block(player, async_result);
            break;
        case b_mkv_frame_step_rewind_refill:
            BDBG_ASSERT(0);
            break;
        case b_mkv_frame_step_done:
            if(async_result!=bfile_segment_async_result_eof) {
                if(player->cluster.async.frame.entry.atom == NULL) {
                    async_result = bfile_segment_async_result_error;
                }
            }
            goto done;
        }
        continue;
done:
        BDBG_MSG_TRACE(("%s:<%#lx -> %u", "b_mkv_player_next_fastforward_async_step", (unsigned long)player, (unsigned)async_result)); 
        return async_result;
    }
}

static bfile_segment_async_result
b_mkv_player_next_fastforward_async(bmkv_player_t player)
{
	bmedia_player_pos pos;
    bool endofstream;

    pos = b_mkv_player_next_fastforward_seek(player, &endofstream);
    if(pos==BMEDIA_PLAYER_INVALID) {
        if(endofstream) {
            return bfile_segment_async_result_eof;
        }
        goto err_seek;
    }

    player->cluster.async.frame.step = b_mkv_player_next_fastforward_async_step;
    player->cluster.async.frame.parse_payload = false;
    player->cluster.async.trick.pos = pos;
	player->cluster.cluster_valid = false;

    return b_mkv_player_next_fastforward_async_step(b_mkv_frame_step_cluster, player, bfile_segment_async_result_success);
err_seek:
    return bfile_segment_async_result_error;
}

static int
b_mkv_player_make_eos_packet(bmkv_player_t player, bmedia_player_entry *entry)
{
    struct b_mkv_player_track *track;
	batom_accum_t dst = player->accum_dest;

    BDBG_MSG(("b_mkv_player_make_eos_packet: %#lx", (unsigned long)player));
    bmedia_player_init_entry(entry);
    batom_accum_clear(player->accum_dest);
    for(track=BLST_S_FIRST(&player->tracks);track;track=BLST_S_NEXT(track, next)) {
        const batom_vec *eos_vec;
        size_t pes_hdr_len;
        void *pes_hdr;
        switch(track->codec_type) {
		case b_mkv_codec_type_vfw_avc:
        case b_mkv_codec_type_avc:
            eos_vec = &bmedia_eos_h264;
            break;
        case b_mkv_codec_type_vfw_mpeg4asp:
        case b_mkv_codec_type_mpeg4asp:
            eos_vec = &bmedia_eos_mpeg4;
            break;
        case b_mkv_codec_type_vfw_vc1_ap:
            eos_vec = &bmedia_eos_vc1;
            break;
        case b_mkv_codec_type_vp8:
            eos_vec = &bmedia_eos_bcmv;
            break;
        default:
            eos_vec = NULL;
            break;
        }
        
        if(eos_vec==NULL) {
            continue;
        }
        pes_hdr = b_mkv_player_frame_buf_alloc(player, BMEDIA_PES_HEADER_MAX_SIZE);
        if(!pes_hdr) {
            goto err_alloc;
        }
        BMEDIA_PES_UNSET_PTS(&track->pes_info);
        if(track->codec_type==b_mkv_codec_type_vp8) {
            BMEDIA_PES_SET_PTS(&track->pes_info, 0);
        }
	    pes_hdr_len = bmedia_pes_header_init(pes_hdr, eos_vec->len+bmedia_eos_stuffing.len, &track->pes_info); 
        if(track->pes_info.stream_id>=2) {
            batom_accum_add_range(dst, pes_hdr, pes_hdr_len);
        }
        batom_accum_add_vec(dst, eos_vec);
        batom_accum_add_vec(dst, &bmedia_eos_stuffing);
        if(track->codec_type==b_mkv_codec_type_vp8) {
            if(track->pes_info.stream_id>=2) {
                batom_accum_add_range(dst, pes_hdr, pes_hdr_len);
            }
            batom_accum_add_vec(dst, eos_vec);
            batom_accum_add_vec(dst, &bmedia_eos_stuffing);
        }
    }
    batom_accum_add_vec(dst, &bmedia_eos_stuffing);
err_alloc:    
	entry->length = batom_accum_len(dst);
    entry->atom = batom_from_accum(dst, NULL, NULL);
	entry->type = bmedia_player_entry_type_atom;
    entry->content = bmedia_player_entry_content_payload;
	entry->timestamp = player->status.position;
	b_mkv_player_frame_buf_clear(player);
    return 0;
}

static void
b_mkv_player_frame_completed(bmkv_player_t player, bfile_segment_async_result async_result)
{
    BDBG_MSG_TRACE(("b_mkv_player_frame_completed: %#lx %u", (unsigned long)player, (unsigned)async_result));
    BDBG_ASSERT(player->cluster.async.busy);
    player->cluster.async.busy = false;
    switch(async_result) {
    case bfile_segment_async_result_async:
        BDBG_ASSERT(0);
        break;
    case bfile_segment_async_result_error:
    case bfile_segment_async_result_eof:
    	BDBG_MSG_TRACE(("b_mkv_player_frame_completed: %#lx can't read a frame", (unsigned long)player));
		b_mkv_player_data_error(player);
        bmedia_player_init_entry(&player->cluster.async.frame.entry);
        player->cluster.async.frame.entry.type = bmedia_player_entry_type_error;
        break;
    case bfile_segment_async_result_no_data:
        BDBG_ERR(("b_mkv_player_frame_completed: %#lx unexpected 'no_data'",(unsigned long)player));
        break;
    case bfile_segment_async_result_success:
        BDBG_ASSERT(player->cluster.async.frame.entry.atom);
        break;
    }
    BDBG_MSG_TRACE(("b_mkv_player_frame_completed: %#lx atom:%#lx len:%u timestamp:%u ", (unsigned long)player, (unsigned long)player->cluster.async.frame.entry.atom, player->cluster.async.frame.entry.atom?batom_len(player->cluster.async.frame.entry.atom):0, (unsigned)player->cluster.async.frame.entry.timestamp));

    player->config.atom_ready(player->config.cntx, &player->cluster.async.frame.entry);
    return;
}

static int
b_mkv_player_next_async_result(bmkv_player_t player, bmedia_player_entry *entry, bfile_segment_async_result async_result)
{
    BDBG_MSG_TRACE(("b_mkv_player_next_async_result: %#lx %u", (unsigned long)player, (unsigned)async_result));
    switch(async_result) {
    case bfile_segment_async_result_success:
        BDBG_ASSERT(player->cluster.async.frame.entry.atom);
        *entry = player->cluster.async.frame.entry;
        return 0;
    case bfile_segment_async_result_async:
        entry->type = bmedia_player_entry_type_async;
        player->cluster.async.frame.frame_complete = b_mkv_player_frame_completed;
        player->cluster.async.busy = true;
        return 0;
	case bfile_segment_async_result_eof:
        entry->type = bmedia_player_entry_type_end_of_stream;
        return 0;
    default:
    case bfile_segment_async_result_error:
        return -1;
    }
}


int
bmkv_player_next(bmkv_player_t player, bmedia_player_entry *entry)
{
	int rc=0;
    bfile_segment_async_result async_result;
	BDBG_ASSERT(entry);
	BDBG_OBJECT_ASSERT(player, bmkv_player_t);

    BDBG_ASSERT(!player->cluster.async.busy);

    bmedia_player_init_entry(&player->cluster.async.frame.entry);
    bmedia_player_init_entry(entry);

    if(!player->eof_reached) {
        if(player->config.atom_ready && player->cluster.async.continuation_active) {
            BDBG_MSG(("%#lx: resume async_continuation: %u", (unsigned long)player, (unsigned)player->cluster.async.continuation_type));
            player->cluster.async.continuation_active = false;
            switch(player->cluster.async.continuation_type)
            {
            case b_mkv_data_continuation_next_block:
                async_result = b_mkv_player_next_block_async(player, player->cluster.async.next_block.parse_payload, player->cluster.async.next_block.block_complete);
                break;
            case b_mkv_data_continuation_fetch_block:
                async_result = b_mkv_player_fetch_block_async(player, player->cluster.async.fetch_block.parse_payload, player->cluster.async.fetch_block.block_complete);
                break;
            default:
                async_result = bfile_segment_async_result_error;
                BDBG_ASSERT(0);
                break;
            }
            switch(async_result) {
            case bfile_segment_async_result_error:
            case bfile_segment_async_result_async:
                rc = b_mkv_player_next_async_result(player, entry, async_result);
                break;
                /* fall through */
            case bfile_segment_async_result_eof:
            case bfile_segment_async_result_success:
                async_result = player->cluster.async.frame.step(b_mkv_frame_step_block, player, async_result);
                rc = b_mkv_player_next_async_result(player, entry, async_result);
                break;
            case bfile_segment_async_result_no_data:
                BDBG_ERR(("%s: %#lx unexpected 'no_data'","bmkv_player_next", (unsigned long)player));
                entry->type = bmedia_player_entry_type_no_data;
                break;
            }
        } else if(player->status.direction>=0) {
            if(player->status.direction<(4*(BMEDIA_PLAYER_POS_SCALE/30))) { /* normal or slow fast forward, if forward and slower than 4x */
                if(player->config.atom_ready) {
                    async_result = b_mkv_player_next_forward_async(player); 
                    rc = b_mkv_player_next_async_result(player, entry, async_result);
                } else {
                    rc = b_mkv_player_next_forward(player, entry); 
                }
            } else {
                if(player->config.atom_ready) { /* fast fast forward */
                    async_result = b_mkv_player_next_fastforward_async(player); 
                    rc = b_mkv_player_next_async_result(player, entry, async_result);
                } else {
                    rc = b_mkv_player_next_fastforward(player, entry); 
                }
            }
        } else {
            if(player->config.atom_ready) { 
                async_result = b_mkv_player_next_rewind_async(player); 
                rc = b_mkv_player_next_async_result(player, entry, async_result);
            } else {
                rc = b_mkv_player_next_rewind(player, entry);
            }
        }
        if(entry->type!=bmedia_player_entry_type_async) {
            BDBG_MSG_TRACE(("bmkv_player_next: %#lx atom:%#lx len:%u timestamp:%u rc:%d", (unsigned long)player, (unsigned long)entry->atom, entry->atom?batom_len(entry->atom):0, (unsigned)entry->timestamp, rc));
        } else {
            BDBG_MSG_TRACE(("bmkv_player_next: %#lx ASYNC", (unsigned long)player));
        }
    } else {
        entry->type = bmedia_player_entry_type_end_of_stream;
        rc = 0;
    }

	if(rc==0 && entry->type == bmedia_player_entry_type_end_of_stream ) {
        if(!player->eof_reached) {
            player->eof_reached = true;
            rc = b_mkv_player_make_eos_packet(player, entry);
        } else {
		    BDBG_MSG(("bmkv_player_next: %#lx EOF reached", (unsigned long)player));
        }
	}
	return rc;
}

void 
bmkv_player_tell(bmkv_player_t player, bmedia_player_pos *pos)
{
	BDBG_OBJECT_ASSERT(player, bmkv_player_t);
	*pos = player->status.position;
	return;
}

void 
bmkv_player_get_status(bmkv_player_t player, bmedia_player_status *status)
{
	BDBG_OBJECT_ASSERT(player, bmkv_player_t);
	BDBG_MSG_TRACE(("bmkv_player_get_status:> %#lx", (unsigned long)player));
	status->bounds = player->status.bounds;
	status->direction = player->status.direction;
    status->index_error_cnt = player->status.index_error_cnt;
	BDBG_MSG_TRACE(("bmkv_player_get_status:< %#lx %ld:%ld", (unsigned long)player, (long)status->bounds.first, (long)status->bounds.last));
	return;
}

int
bmkv_player_set_direction(bmkv_player_t player, bmedia_player_step direction, bmedia_time_scale time_scale)
{
	int rc=0;
	struct b_mkv_player_track *track;
	BDBG_OBJECT_ASSERT(player, bmkv_player_t);
	BDBG_MSG_TRACE(("bmkv_player_set_direction:> %#lx %ld", (unsigned long)player, (long)direction));

    player->eof_reached = false;
    player->cluster.async.continuation_active = false;

	if(direction!=0 && !player->cue.validate) {
		BDBG_MSG_TRACE(("bmkv_player_set_direction: %#lx FF/REW not supported without index", (unsigned long)player));
		rc = -1;
		goto done;
	}
	if(player->status.direction != direction) {
		BLIFO_INIT(&player->trick.rewind_frames, player->key_frames_buf, B_MKV_PLAYER_TRICK_ENTRIES);
	}
	player->status.direction = direction;
	player->time_scale = time_scale;

    for(track=BLST_S_FIRST(&player->tracks);track;track=BLST_S_NEXT(track, next)) {
		if(track->mkv_track->validate.Video && track->pts_reorder_enabled) {
		    b_mkv_track_player_flush_pts_reorder_queue(player, track);
        }
	}
done:
	BDBG_MSG_TRACE(("bmkv_player_set_direction:< %#lx %ld", (unsigned long)player, (long)rc));
	return rc;
}

static void
b_mkv_player_sync_seek_table(bmkv_player_t player)
{
	uint64_t offset;
	bmedia_player_pos index_pos;
	bmedia_player_pos pos = player->status.position;
	bmkv_SeekElement seek;

	if(!player->seek_table.index) {
		BDBG_MSG(("b_mkv_player_sync_seek_table:%#lx no seek table", (unsigned long)player));
		return; /* nothing to sink */
	}
	if( player->mkv_file.segment.start == player->cluster.offset_next  || 
		(player->cluster.first_found && player->cluster.offset_next == player->cluster.offset_first)) {
		/* seek to start */
		bfile_cached_segment_seek(&player->seek_table.cache, 0);
		return;
	}
	for(;;) {
		if(pos==0) {
			offset = 0;
			break;
		}
		index_pos = bmedia_index_get(player->seek_table.index, pos, &offset);
		if(index_pos==BMEDIA_PLAYER_INVALID) {
			offset = 0;	
			break;
		} else {
			BDBG_MSG_TRACE(("b_mkv_player_sync_seek_table:%#lx index pos:%u(%u) offset:%u", (unsigned long)player, (unsigned)index_pos,(unsigned)pos, (unsigned)offset));
		}
		if(index_pos < pos) {
			break;
		}
		/* since seek_table index only populated on the play (e.g. cluster scan), after long seek, there is a set of empty entries, which all then populated by the seek destination */
		/* it's impractical to populate index during search in index, since it would require for each entry to read cluster, so instead we are looking for the entry which earlier then any 
		 * possible duplicate */
		bfile_cached_segment_seek(&player->seek_table.cache, offset);
		if(!bfile_cached_segment_reserve_min(&player->seek_table.cache, B_MKV_PLAYER_MAX_SEEK_ENTRY, B_MKV_PLAYER_MIN_SEEK_ENTRY)) {
			goto err_sync;
		}
		if(bmkv_element_parse_from_desc(&player->seek_table.cache.cursor, &bmkv_SeekElement_desc, BMKV_SEEK, &seek) != bmkv_element_parse_result_success) {
			goto err_sync;
		}
		/* verify that entry valid and points to earlier cluster, (assume that clusters are stored in order) */
		if( seek.validate.SeekID && seek.validate.SeekPosition && seek.SeekID == BMKV_CLUSTER_ID && (player->mkv_file.segment.start+seek.SeekPosition) <= (uint64_t)player->cluster.offset_next ) {
			break;
		}
		/* look back in time */
		pos = pos/2; 
	}
	bfile_cached_segment_seek(&player->seek_table.cache, offset);
	for(;;) {
		if(!bfile_cached_segment_reserve_min(&player->seek_table.cache, B_MKV_PLAYER_MAX_SEEK_ENTRY, B_MKV_PLAYER_MIN_SEEK_ENTRY)) {
			goto err_sync;
		}
		if(bmkv_element_parse_from_desc(&player->seek_table.cache.cursor, &bmkv_SeekElement_desc, BMKV_SEEK, &seek) != bmkv_element_parse_result_success) {
			goto err_sync;
		}
		/* verify that entry valid and points to the cluster, (assume that clusters are stored in order) */
		if( seek.validate.SeekID && seek.validate.SeekPosition && seek.SeekID == BMKV_CLUSTER_ID && (player->mkv_file.segment.start+seek.SeekPosition) == (uint64_t)player->cluster.offset_next ) {
			BDBG_MSG(("b_mkv_player_sync_seek_table:%#lx index found cluster:%u at offset:%u", (unsigned long)player, (unsigned)player->cluster.offset_next, (unsigned)offset));
			bfile_cached_segment_seek(&player->seek_table.cache, offset); /* seek to found location */
			break;
		}
		offset = bfile_cached_segment_tell(&player->seek_table.cache);
	}
	return;
err_sync:
	BDBG_WRN(("b_mkv_player_sync_seek_table:%#lx can't find cluster in the seek table, invalidating seek_table", (unsigned long)player));
	player->seek_table.cache.segment.len = 0;
	return;
}



int 
bmkv_player_seek(bmkv_player_t player, bmedia_player_pos pos)
{
	struct b_mkv_player_track *track;
	BDBG_OBJECT_ASSERT(player, bmkv_player_t);

	BDBG_MSG(("bmkv_player_seek:> %#lx %u(%u)", (unsigned long)player, (unsigned)pos, (unsigned)player->status.position));
	if(pos<player->status.bounds.first || pos>player->status.bounds.last) {
		BDBG_WRN(("bmkv_player_seek: %#lx out of bounds %u: %u..%u", (unsigned long)player, (unsigned)pos, (unsigned)player->status.bounds.first, (unsigned)player->status.bounds.last));
	}
    player->eof_reached = false;
    player->cluster.async.continuation_active = false;

    BDBG_ASSERT(!player->cluster.async.busy);

	if(pos<=player->status.bounds.first) {
		player->status.position = player->status.bounds.first;
        if(player->seek_table.cache.segment.len) {
            bfile_cached_segment_seek(&player->seek_table.cache, 0);
        }
		
		if(player->cluster.first_found) {
			player->cluster.offset_next = player->cluster.offset_first;
		} else {
			player->cluster.offset_next = player->mkv_file.segment.start;
		}

        for(track=BLST_S_FIRST(&player->tracks);track;track=BLST_S_NEXT(track, next)) {
            track->first_frame = true;
        }
    } else {
		if(b_mkv_player_cue_seek_prepare(player, pos)<0) {
			goto error;
		}

		for(;;) {
            b_mkv_result result = b_mkv_player_cue_next(player);
			if(result!=b_mkv_result_success) {
                if(result==b_mkv_result_end_of_data) {
                    break;
                } else {
			        goto error;
                }
			}
			BDBG_MSG_TRACE(("bmkv_player_seek:%#lx mkv_pos:%u destination:%u", (unsigned long)player, (unsigned)player->cue.mkv_pos, (unsigned)pos));
			player->status.position = player->cue.mkv_pos;
			if(player->cue.mkv_pos >= pos) {
				goto end_of_seek;
			}
		}
		if(!player->cue.mkv_track_pos.validate.CueClusterPosition) {
			BDBG_WRN(("bmkv_player_seek:%#lx can't found cue entries", (unsigned long)player, (unsigned)pos, (unsigned)player->status.position));
			goto error;
		}
end_of_seek:
		BDBG_ASSERT(player->cue.mkv_track_pos.validate.CueClusterPosition);
		player->cluster.offset_next = player->cue.mkv_track_pos.CueClusterPosition + player->mkv_file.segment.start;
        for(track=BLST_S_FIRST(&player->tracks);track;track=BLST_S_NEXT(track, next)) {
            track->seek_sync = true;
			track->first_frame = true;
            if(track->mkv_track->validate.Video) { 
                /* there is a video track and we shall start from Key frame */
                player->cluster.resync_video = true;
            }
        }
    }
	b_mkv_player_sync_seek_table(player);
	BLIFO_INIT(&player->trick.rewind_frames, player->key_frames_buf, B_MKV_PLAYER_TRICK_ENTRIES);
	player->cluster.cluster_valid = false;
	player->cluster.block_valid = false;
	player->cluster.seek_target = pos;
	bfile_cached_segment_set(&player->cluster.cache, 0, 0);
	BDBG_MSG(("bmkv_player_seek:< %#lx %u:%u cluster:%u", (unsigned long)player, (unsigned)pos, (unsigned)player->status.position, (unsigned)player->cluster.offset_next));
	return 0;
error:
	BDBG_WRN(("bmkv_player_seek: %#lx %u:%u fails", (unsigned long)player, (unsigned)pos, (unsigned)player->status.position));
	return -1;
}

static void *
b_mkv_player_create(bfile_io_read_t fd, const bmedia_player_config *config, const bmedia_player_stream *stream)
{
	return bmkv_player_create(fd, config, stream);
}

static void
b_mkv_player_destroy(void *player)
{
	bmkv_player_destroy(player);
}

static int
b_mkv_player_next(void *player, bmedia_player_entry *entry)
{
	return bmkv_player_next(player, entry);
}

static void 
b_mkv_player_tell(void *player, bmedia_player_pos *pos)
{
	bmkv_player_tell(player, pos);
	return;
}

static void 
b_mkv_player_get_status(void *player, bmedia_player_status *status)
{
	bmkv_player_get_status(player, status);
	return;
}

static int
b_mkv_player_set_direction(void *player, bmedia_player_step direction, bmedia_time_scale time_scale, bmedia_player_decoder_mode *mode)
{
    BSTD_UNUSED(mode);
	return bmkv_player_set_direction(player, direction, time_scale);
}

static int  
b_mkv_player_seek(void *player, bmedia_player_pos pos)
{
	return bmkv_player_seek(player, pos);
}

const bmedia_player_methods bmkv_player_methods = {
	b_mkv_player_create,
	b_mkv_player_destroy,
	b_mkv_player_next,
	b_mkv_player_tell,
	b_mkv_player_get_status,
	b_mkv_player_set_direction,
	b_mkv_player_seek
};

