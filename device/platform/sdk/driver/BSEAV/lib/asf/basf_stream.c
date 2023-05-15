/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: basf_stream.c $
 * $brcm_Revision: 90 $
 * $brcm_Date: 7/18/12 2:24p $
 *
 * Module Description:
 *
 * ASF parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/asf/basf_stream.c $
 * 
 * 90   7/18/12 2:24p jtna
 * SW7425-3513: fix sending of end-of-sequence startcode for vc1sm codec
 * 
 * 89   5/30/12 2:12p vsilyaev
 * SW7425-3144: Improved handling of concatenated streams
 * 
 * 88   1/13/12 12:35p vsilyaev
 * SW7346-626: Replaced ASSERT with a regular test and flush
 * 
 * 87   6/14/11 12:03p vsilyaev
 * SW7405-5368: Use original audio type for PR streams
 * 
 * 86   2/28/11 1:39p vsilyaev
 * SWDTV-2483: Don't try to pick into the picture type if there is a per-
 * object callback
 * 
 * 85   2/28/11 1:31p vsilyaev
 * SWDTV-2483: Added code to capture data
 * 
 * 84   9/28/10 11:04a vsilyaev
 * SW7405-4904: Added code to recongnize 'PR' files
 * 
 * 83   8/3/10 5:54p vsilyaev
 * SW7405-4439: Limited depth of payload queue
 * 
 * 82   8/3/10 5:38p vsilyaev
 * SW7405-4439: Added code to queue and reorder out-of-order payloads
 * 
 * 81   6/10/10 1:34p vsilyaev
 * SW7468-69: Use compile time assert where applicable
 * 
 * 80   5/10/10 4:31p jtna
 * SW7125-281: Coverity Defect ID:20443 RESOURCE_LEAK (struct
 * parsed_stream)
 * 
 * 79   4/14/10 12:03p jtna
 * SW7125-282: Coverity Defect ID:20336 CHECKED_RETURN
 * 
 * 78   2/26/10 4:48p vsilyaev
 * SW7468-69: Fixed typo and improved error recovery
 * 
 * 77   2/24/10 4:36p vsilyaev
 * SW7468-69: Use common definiton for BCMA packet type
 * 
 * 76   1/21/10 5:56p vsilyaev
 * SW3556-1003: Added option to disable timestamp reordering inside bmedia
 * 
 * 75   11/23/09 6:09p vsilyaev
 * SW3548-2588: Fixed typo
 * 
 * 74   11/2/09 10:25a gmohile
 * SW7405-3330 : Fix warnings
 * 
 * 73   11/2/09 9:34a maivu
 * SW3548-2587: Fixed warnings
 * 
 * 72   10/30/09 1:30p gmohile
 * SW7405-3330 : Add support to extract auxiliary stream
 * 
 * 71   10/16/09 1:14p vsilyaev
 * SW7405-3217: Removed unused variable
 * 
 * 70   10/16/09 1:10p vsilyaev
 * SW7405-3217: VC1 support in AVI streams
 * 
 * fix_bdvd_v2.0_pr16643/1   10/14/09 3:19p gooslin
 * Add VC1 support to AVI
 * 
 * 69   6/12/09 12:29p vsilyaev
 * PR 55896 PR 55873: Also added AAC audio support
 * 
 * 68   6/11/09 3:56p vsilyaev
 * PR 55896: Fixed warning
 * 
 * 67   6/10/09 12:54p vsilyaev
 * PR 55896: After merge minor fixes
 * 
 * 66   6/10/09 12:43p vsilyaev
 * PR 55896: Merged changes for extended ASF codec support
 * 
 * dev_bdvd_v2.1_extend_asf_codec_support/6   6/5/09 4:42p gooslin
 * Use meta_header flag to indicate a meta data packet
 * 
 * dev_bdvd_v2.1_extend_asf_codec_support/5   6/5/09 3:15p gooslin
 * Fix RAI insert for AVC
 * 
 * dev_bdvd_v2.1_extend_asf_codec_support/4   6/5/09 12:23p gooslin
 * Send metadata for mp4 streams
 * 
 * dev_bdvd_v2.1_extend_asf_codec_support/3   6/4/09 5:30p gooslin
 * Set marker PTS for both VC1 SM and AP meta atom
 * 
 * dev_bdvd_v2.1_extend_asf_codec_support/2   6/4/09 3:48p gooslin
 * Add H.264 support to ASF
 * 
 * dev_bdvd_v2.1_extend_asf_codec_support/1   6/2/09 6:01p gooslin
 * Extend ASF codec support
 * 
 * 65   5/15/09 11:36a vsilyaev
 * PR 55142: Removed test for WMV1 and WMV2 codecs
 * 
 * 64   5/15/09 11:22a vsilyaev
 * PR 55142: Fixed use of VC1/WMV codecs, separated VC1_SM/VC_AP and rest
 * of WMV
 * 
 * 63   5/15/09 10:56a vsilyaev
 * PR 55142: Added MP43 video support for ASF files
 * 
 * dev_bdvd_v1.5_add_mp4v3_support_to_asf/1   5/13/09 5:54p gooslin
 * Add MP4v3 support to ASF
 * 
 * 62   5/4/09 6:08p vsilyaev
 * PR 52434: Use common routines to parse Wave and Bitmap headers
 * 
 * 61   4/28/09 5:54p vsilyaev
 * PR 53911: Added support for processing of the ASF hidden tracks
 * 
 * 60   3/5/09 7:06p vsilyaev
 * PR 52434: Unify location of frame start codes
 * 
 * 59   3/4/09 8:01p vsilyaev
 * PR 52819: Updated meta data when parsing new streamProperties object
 * 
 * 58   3/2/09 3:05p vsilyaev
 * PR 52434: Post merge updates/unification of parsing video data
 * 
 * 57   2/20/09 4:20p vsilyaev
 * PR 52353: Proper account for preroll when accessing the index entries
 * 
 * 56   2/20/09 12:47p vsilyaev
 * PR 52353: Added code to offset PTS'es by file's preroll
 * 
 * 55   11/4/08 9:57a vsilyaev
 * PR 48608: Merged code that alligns audio/video streams after seek.
 * 
 * 54   10/22/08 12:25p vishk
 * PR 48076: Coverity Defect ID:12250 DEADCODE basf_stream.c
 * Product=97401linux
 * 
 * 53   10/2/08 6:00p vsilyaev
 * PR36319 PR34771 PR47073 PR46504: Reduced memory requirements needed to
 * reorder frames in ASF files
 * 
 * bdvd_v1.4/2   9/25/08 2:49p gooslin
 * PR_10564 [ &nbsp; ].  Clean up ASF A/V sync code
 * 
 * bdvd_v1.4/1   9/23/08 9:50p gooslin
 * PR_10564 [ &nbsp; ].  Fix A/V sync issue when playing ASF files from a
 * point other than the origin
 * 
 * fix_bdvd_v1.4_pr10564/2   9/24/08 8:32p gooslin
 * Add get info call for syncing
 * 
 * fix_bdvd_v1.4_pr10564/1   9/23/08 4:10p gooslin
 * Changes to resync audio and video after a seek in the stream
 * 
 * 52   6/12/08 3:07p vsilyaev
 * PR 43607: Renamed alloc and free to bmem_alloc and bmem_free
 * 
 * 51   3/4/08 9:39a gmohile
 * PR 39762 : Send 10K PES packet filled with zeros for WMA audio
 * 
 * phase7.1/1   2/28/08 3:11p gmohile
 * PR 39762 : Send 10K PES packet filled with zeros for WMA audio
 * 
 * 50   10/29/07 5:38p gmohile
 * PR 34100 : Get meta for split audio from atom
 * 
 * 49   10/1/07 3:03p jtna
 * PR35208: LIB-Converity (CID 3070): RESOURCE_LEAK, fix compile error
 * 
 * 48   9/28/07 5:59p jtna
 * PR35208: LIB-Converity (CID 3070): RESOURCE_LEAK
 * 
 * 47   7/24/07 10:18a vsilyaev
 * PR 32846: Don't dereference stream_info when stream properties weren't
 * extracted from the ASF header
 * 
 * 46   7/20/07 4:22p vsilyaev
 * PR 32846: Call payload callback after callback was verified
 * 
 * 45   7/20/07 4:17p vsilyaev
 * PR 32846: Removed unused callback and fixed payload and media_object
 * callbacks
 * 
 * 44   7/12/07 12:28p vsilyaev
 * PR 29574: PR 32846: Updated paresing of picture type. Clear key frame
 * only mode on reset
 * 
 * 43   7/11/07 3:27p vsilyaev
 * PR 32846: Fixed handling of key frame only trickmodes
 * 
 * 42   7/11/07 2:20p vsilyaev
 * PR 32846: Fixed typo
 * 
 * 41   7/11/07 1:53p vsilyaev
 * PR 32846: PR 29815: Added method to the sink interface that notifies
 * sink about end of data object.
 * 
 * 40   7/10/07 6:16p vsilyaev
 * PR 32846: Fixed header for the audio stream
 * 
 * 39   7/9/07 3:38p vsilyaev
 * PR 32846: Updated ASF code to work with batom/SPF framework
 * 
 * 38   3/9/07 5:21p vsilyaev
 * PR 27880: Added processing for WMA PRO audio
 * 
 * 37   3/9/07 3:14p dlwin
 * PR 28367, PR 27573:
 * - Fix is logic comparsion error when checking for Simple and Main.
 * - ASF Parser issue, problem playing wmv movie "Mission Impossible III"
 * due to resource allocation issue.
 * 
 * 36   3/6/07 3:52p ptimariu
 * PR20685:changed media obj params
 * 
 * 35   3/6/07 2:30p ptimariu
 * PR20685: changed media obj handler params
 * 
 * 34   1/26/07 4:50p vsilyaev
 * PR 27347: Added recovery code to BASF_ASSERT
 * 
 * 33   1/22/07 3:47p gmullen
 * PR24746: b_asf_process_media_object needs to skip first two bio_vecs
 * 
 * 31   1/12/07 10:26a gmullen
 * PR24746: Added hook to intercept completed media object
 * 
 * 30   11/3/06 2:54p erickson
 * PR25555: fix logic for conversion of 0xFFFFFFFF PTS found in stream
 * 
 * 29   9/26/06 4:51p vsilyaev
 * PR 24572: Added code to probe start code in the first payload of VC1 AP
 * stream
 * 
 * 28   9/22/06 12:52p vsilyaev
 * PR 24055: Addec code to filter out 0xFFFFFFFF PTS values
 * 
 * 27   9/13/06 1:11p vsilyaev
 * PR 24056: Added code to contruct 'PTS Marked' PES stream for VC1
 * Simple&Main Profiles
 * 
 * 26   9/8/06 9:33a vsilyaev
 * PR 23304: Added code to mark and test 'free' stream items
 * 
 * 25   9/7/06 1:11p vsilyaev
 * PR 22445: Added better comments
 * 
 * 24   9/1/06 10:38a vsilyaev
 * PR 22578: Fixed FOURCC codes for video
 * 
 * 23   8/16/06 4:05p vsilyaev
 * PR 22700: Fixed formating of the meta data for the splitted audio
 * blocks
 * 
 * 22   8/10/06 6:16p vsilyaev
 * PR 22700:  Added code to split audio packets by block boundaries
 * 
 * 21   8/7/06 5:30p ptimariu
 * PR20685: remove drm references from asf demux
 * 
 * 20   8/4/06 12:46p vsilyaev
 * PR 22876: Added support for paced trickmodes to allow rate accurate
 * trick modes
 * 
 * 19   8/3/06 3:01p dlwin
 * PR 22578: Fixed compiler warning.
 * 
 * 18   7/28/06 8:51p vsilyaev
 * PR 22578: Improved handling of cases where index entry pointes either
 * to multiple key frames and/or incomplete key frame
 * 
 * 17   7/28/06 4:14p vsilyaev
 * PR 22578: Improved handling of skipped objects for the VC1 SP/MP
 * 
 * 16   7/20/06 6:03p ptimariu
 * PR20586: merge asf drm integration
 * 
 * 15   7/19/06 5:55p vsilyaev
 * PR 20683: Improved comments
 * 
 * 14   7/14/06 4:11p vsilyaev
 * PR 22578: Yet another fix for the codeIn format
 * 
 * 13   7/12/06 3:22p vsilyaev
 * PR 22578: Fixed packetization for the codeIn format
 * 
 * 12   7/11/06 11:15a vsilyaev
 * PR 22578: Fixed typo
 * 
 * 11   7/10/06 4:30p vsilyaev
 * PR 22578: Added support for the 'CodeIn' PES format
 * 
 * 10   5/2/06 5:51p vsilyaev
 * PR 20683: Added support for the navigation in ASF files
 * 
 * 9   5/1/06 2:58p vsilyaev
 * PR 20683: Improved the asf_player support
 * 
 * 8   4/19/06 10:13a vsilyaev
 * PR 20308: Improved audio parsing
 * 
 * 7   4/17/06 3:32p vsilyaev
 * PR 20308: Added support for audio streams
 * 
 * 6   4/14/06 10:07a vsilyaev
 * PR 20308: Added way to pause a ASF parser from a object handler
 * 
 * 5   4/13/06 3:14p vsilyaev
 * PR 20308: Improved API description
 * 
 * 4   4/12/06 10:33a vsilyaev
 * PR 20308: Improved parsing of compressed payload
 * 
 * 3   4/7/06 12:29p vsilyaev
 * PR 20683: added basf_player
 * 
 * 2   4/5/06 4:18p vsilyaev
 * PR 20308: Rerranged data flow to accumulate entire media object, before
 * converting to PES
 * 
 * 1   4/4/06 6:47p vsilyaev
 * PR 20577: Separated ASF parser and ASF stream
 * 
 * 
 *******************************************************************************/
#include "bstd.h"
#include "basf_stream.h"
#include "bkni.h"
#include "basf_impl.h"
#include "blst_slist.h"
#include "biobits.h"

#include "common_crypto.h"
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
#include <stdio.h>
#include "drmmanager.h"
#endif

BDBG_MODULE(basf_stream);

#ifdef PROFILE
#undef BDBG_MSG
#define BDBG_MSG(x)
#endif
#define BDBG_MSG_TRACE(x)  /* BDBG_MSG(x) */

#define B_ASF_MAX_PES_HEADER_SIZE	96
#define B_ASF_MAX_PAYLOADS		128
#define B_ASF_STREAM_META_ATOM	48
#define B_ASF_NSTREAMS			8

#define B_ASF_CAPTURE_DATA 0

#if B_ASF_CAPTURE_DATA 
#include <stdio.h>
static void b_asf_p_dump(FILE *fp, const void *context, const void *addr, size_t len, size_t *capture_len)
{
    uint8_t header[12];

    if(*capture_len==0) {
        return;
    }
    if(*capture_len<len) {
        len = *capture_len;
    } 
    *capture_len -= len;

    header[0] = ((uint32_t)addr)>>24;
    header[1] = ((uint32_t)addr)>>16;
    header[2] = ((uint32_t)addr)>>8;
    header[3] = ((uint32_t)addr);

    header[4] = ((uint32_t)len)>>24;
    header[5] = ((uint32_t)len)>>16;
    header[6] = ((uint32_t)len)>>8;
    header[7] = ((uint32_t)len);

    header[8] = ((uint32_t)context)>>24;
    header[9] = ((uint32_t)context)>>16;
    header[10] = ((uint32_t)context)>>8;
    header[11] = ((uint32_t)context);
    fwrite(header, 1, sizeof(header), fp);
    fwrite(addr, 1, len, fp);
    return;
}

static void b_asf_p_capture_data(FILE **fp, const void *context, const char *name, const batom_cursor *cursor, size_t length)
{
    if(*fp==NULL) {
        *fp = fopen(name, "w+b");
    }
    if(*fp) {
        unsigned i = cursor->pos;

        if(cursor->left>0) {
            b_asf_p_dump(*fp, context, cursor->cursor,cursor->left, &length);
            i++;
        }

        for(;i<cursor->count;i++) {
            b_asf_p_dump(*fp, context, cursor->vec[i].base, cursor->vec[i].len, &length);
        }
        fflush(*fp);
    }
}
#endif /* B_ASF_CAPTURE_DATA */

enum b_asf_stream_type {
	b_asf_stream_type_vc1_ap,
	b_asf_stream_type_vc1_sm, /* WM9 simple or main profile in code-in format */
	b_asf_stream_type_mpeg2,
	b_asf_stream_type_mp4p2,
	b_asf_stream_type_mp4v3,
	b_asf_stream_type_h264,
	b_asf_stream_type_wma,
	b_asf_stream_type_pcm,
	b_asf_stream_type_aac,
	b_asf_stream_type_generic_audio,
	b_asf_stream_type_aux
};

struct b_asf_demux_object_handler {
	basf_parser_object_handler handler; /* must be first */
	basf_demux_t demux; /* pointer to demux */ 
};

struct b_asf_stream_info {
	unsigned stream_number;
	BLST_S_ENTRY(b_asf_stream_info) link;
	enum b_asf_stream_type stream_type;
    void (*stream_handler)(basf_stream_t stream, const struct basf_payload_info *info);
	basf_qword time_offset;
    basf_stream_t stream; /* pointer back to associated stream, could be NULL */
	bool encrypted_content_flag;
	const void *meta;
    const batom_vec *vecs[2];
	size_t meta_length;
    union {
        struct {
            bmedia_adts_header adts_header;
        } aac;
    } codec;
};

typedef struct b_asf_stream_object_reordering_payload {
	BLST_S_ENTRY(b_asf_stream_object_reordering_payload) link;
    batom_t data;
	basf_dword offset_into_media_object;
} b_asf_stream_object_reordering_payload;

#define B_ASF_STREAM_OBJECT_REORDERING_QUEUE_SIZE   128

typedef struct b_asf_stream_object_reordering_queue {
	BLST_S_HEAD(b_asf_stream_object_reordering_queue_lst, b_asf_stream_object_reordering_payload) payloads;
    bool valid; /* whether queue is valid */
    uint16_t free_payload; /* offset of the free packet */
	basf_dword media_object_size; /* size of the current object */
	basf_dword pres_time; /* time of the current object */
    basf_dword media_object_queued; /* total length of fragments queued */
    b_asf_stream_object_reordering_payload reordering_buffer[B_ASF_STREAM_OBJECT_REORDERING_QUEUE_SIZE];
} b_asf_stream_object_reordering_queue; 

BDBG_OBJECT_ID(basf_stream_t);


struct basf_stream {
	BDBG_OBJECT(basf_stream_t)
	BLST_S_ENTRY(basf_stream) link;
	unsigned stream_number;
	batom_accum_t accum;
	struct {
		size_t bytes;
        basf_dword pres_time;
        basf_dword media_object_size;
        bool reordering;
	} object;
	batom_t meta;
    const batom_vec *external_vecs[2];
	struct b_asf_stream_info *stream_info; /* NULL if stream is not connected */
	batom_pipe_t pipe; /* destination pipe */
	struct { /* reordering queue */
		batom_pipe_t pipe; /* pipe that is used to reorder PTS'es */
		uint32_t last_pts; /* PTS of last frame added into queue */
		bool key_frame; /* set to true if first frame in the queue is the key frame */
		uint16_t nframes; /* number of frames queued in the pipe */
	} queue;
	unsigned nkey_frames; /* numer of key frames captured since last flush */
	basf_demux_t demux;
    b_asf_stream_object_reordering_queue payload_queue; 
	basf_stream_cfg cfg;
};

struct basf_video_stream_info {
	struct b_asf_stream_info stream;
	uint8_t seq_header[1024];
	union {
        bmedia_vc1sm_info vc1_sm;
        bmedia_vc1ap_info vc1_ap;
	} seq_hdr;
};

struct basf_audio_stream_info {
	struct      b_asf_stream_info stream;
	size_t      block_size;
	uint16_t    force_packet_size;
	uint8_t     seq_header[48];
};

struct basf_aux_stream_info {
	struct      b_asf_stream_info stream;
};


BDBG_OBJECT_ID(basf_demux_t);
struct basf_demux {
	BDBG_OBJECT(basf_demux_t)
	BLST_S_HEAD(b_asf_stream_lst, basf_stream) streams;
	BLST_S_HEAD(b_asf_stream_info_lst, b_asf_stream_info) stream_info;
	bmedia_time_scale time_scale;
	bool key_frame_only;
	basf_demux_cfg cfg;
	basf_parser_t parser;
	batom_factory_t factory;
	balloc_iface_t alloc;
	struct b_asf_demux_object_handler stream_properties_handler;
	struct b_asf_demux_object_handler extended_stream_properties_handler;
	basf_qword preroll;
	
#if defined (HUMAX_PLATFORM_BASE)
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
/*************************
*	PLAYREADY
*************************/
	void* pHSession;
	unsigned char* wrmheader;
	unsigned int wrmheaderlen;
	bool bWmDrmEncryptedContents;
	bool bPlayReadyEncryptedContents;
	DRM_DECRYPT_CONTEXT stDecryptContext;
	CommonCryptoHandle cryptoHandle;
	NEXUS_KeySlotHandle keyWmdrm;
	NEXUS_KeySlotHandle keyPrdy;
	unsigned char* pEkl;
	struct b_asf_demux_object_handler extended_contents_encryption_object_handler;
	struct b_asf_demux_object_handler playready_header_object_handler;
#if defined(CONFIG_PLAYREADY_25)
	bool bSetContext;
	DRM_APP_CONTEXT* pAppContext;
	DRM_VOID *f_pOEMContext;
#endif
#endif
#endif
};

#if defined (HUMAX_PLATFORM_BASE)
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT)
httpclient_license_post_callback_t soap_post_callback;
httpclient_license_post_callback_t default_post_callback;
get_secure_clock_callback_t secure_clock_callback;
#endif
#endif

#define B_ASF_STREAM_VEC_FRAME	0
#define B_ASF_STREAM_VEC_EOS	1

static void basf_stream_unlink(basf_demux_t asf, basf_stream_t stream);

static bool
b_asf_stream_probe_vc1_scode(batom_accum_t accum)
{
    int vc1_scode;
	batom_cursor cursor;
	batom_cursor_from_accum(&cursor, accum);
	vc1_scode = bmedia_video_read_scode(&cursor);
	BDBG_MSG_TRACE(("b_asf_stream_probe_vc1_scode: %#lx scode:0x%06x", (unsigned long)accum, (unsigned)vc1_scode));
	return  vc1_scode>=0;
}


typedef struct b_asf_audio_aac_hdr {
	bmedia_packet_header pes; /* should be first */
	uint8_t frame_hdr[BMEDIA_ADTS_HEADER_SIZE]; 
} b_asf_audio_aac_hdr;

static const batom_user b_asf_audio_aac_atom = {
	NULL,
	sizeof(b_asf_audio_aac_hdr)
};

static uint32_t
b_asf_get_pts(basf_qword pres_time, basf_qword preroll, bmedia_time_scale time_scale) {
    if(pres_time >= preroll) {
        pres_time -= preroll;
    } else {
        BDBG_MSG(("b_asf_get_pts: ignoring preroll %u:%u", (unsigned)preroll, (unsigned)pres_time));
    }

	return bmedia_time2pts(pres_time, time_scale);	
}

static void
b_asf_stream_feed_audio_generic(basf_stream_t stream, const struct basf_payload_info *info) {
	batom_t atom;
	bmedia_packet_header hdr;

	BDBG_OBJECT_ASSERT(stream, basf_stream_t);
	BDBG_OBJECT_ASSERT(stream->demux, basf_demux_t);
	BDBG_ASSERT(stream->pipe);
	BDBG_ASSERT(stream->accum);
	BDBG_ASSERT(info);

	BMEDIA_PACKET_HEADER_INIT(&hdr);
	hdr.pts = b_asf_get_pts(info->pres_time, stream->demux->preroll, stream->demux->time_scale);
	hdr.pts_valid = true;
	hdr.key_frame = true;

	BDBG_MSG_TRACE(("b_asf_stream_feed_audio_generic>: %#lx:%#lx audio %s:%u %u", (unsigned long)stream->demux, (unsigned long)stream, hdr.pts_valid?"pts":"no-pts", (unsigned)hdr.pts, batom_accum_len(stream->accum)));
	atom = batom_from_accum(stream->accum, bmedia_atom, &hdr);
	if(atom) {
		batom_pipe_push(stream->pipe, atom);
	} else {
		batom_accum_clear(stream->accum);
		BDBG_MSG_TRACE(("b_asf_stream_feed_audio_generic<: %#lx:%#lx %u->NULL", (unsigned long)stream->demux, (unsigned long)stream, info->media_object_size));
	}
	return;
}

static void
b_asf_stream_feed_audio_aac(basf_stream_t stream, const struct basf_payload_info *info) {
	batom_t atom;
	b_asf_audio_aac_hdr hdr;

	BDBG_OBJECT_ASSERT(stream, basf_stream_t);
	BDBG_OBJECT_ASSERT(stream->demux, basf_demux_t);
	BDBG_ASSERT(stream->pipe);
	BDBG_ASSERT(stream->accum);
	BDBG_ASSERT(info);

	BMEDIA_PACKET_HEADER_INIT(&hdr.pes);
	hdr.pes.pts = b_asf_get_pts(info->pres_time, stream->demux->preroll, stream->demux->time_scale);
	hdr.pes.pts_valid = true;
	hdr.pes.key_frame = true;
	hdr.pes.header_off = 0;
	hdr.pes.header_len = bmedia_adts_header_fill(hdr.frame_hdr, &stream->stream_info->codec.aac.adts_header, info->media_object_size);
	BDBG_ASSERT(hdr.pes.header_len <= sizeof(hdr.frame_hdr));

	BDBG_MSG_TRACE(("b_asf_stream_feed_audio_aac>: %#lx:%#lx audio %s:%u %u", (unsigned long)stream->demux, (unsigned long)stream, hdr.pes.pts_valid?"pts":"no-pts", (unsigned)hdr.pes.pts, batom_accum_len(stream->accum)));
	atom = batom_from_accum(stream->accum, &b_asf_audio_aac_atom, &hdr);
	if(atom) {
		batom_pipe_push(stream->pipe, atom);
	} else {
		batom_accum_clear(stream->accum);
		BDBG_MSG_TRACE(("b_asf_stream_feed_audio_aac<: %#lx:%#lx %u->NULL", (unsigned long)stream->demux, (unsigned long)stream, info->media_object_size));
	}
	return;
}

static void
b_asf_stream_feed_audio_pcm(basf_stream_t stream, const struct basf_payload_info *info) {
	const struct basf_audio_stream_info *audio;
	bmedia_bcma_hdr hdr;
	size_t data_len;

	BDBG_OBJECT_ASSERT(stream, basf_stream_t);
	BDBG_OBJECT_ASSERT(stream->demux, basf_demux_t);
	BDBG_ASSERT(stream->pipe);
	BDBG_ASSERT(stream->accum);
	BDBG_ASSERT(info);
	audio = (struct basf_audio_stream_info *) stream->stream_info;
	BDBG_ASSERT(audio);
	BDBG_ASSERT(audio->force_packet_size);

	BMEDIA_PACKET_HEADER_INIT(&hdr.pes);
	data_len = batom_accum_len(stream->accum);
	BDBG_MSG_TRACE(("b_asf_stream_feed_audio_pcm: %#lx %u", (unsigned long)stream, data_len));
	hdr.pes.pts = b_asf_get_pts(info->pres_time, stream->demux->preroll, stream->demux->time_scale);
	hdr.pes.key_frame = hdr.pes.pts_valid = true;

	for(;;) {
		batom_t pes_atom;
		batom_t frame;
		batom_cursor first, last;

		data_len = batom_accum_len(stream->accum);
    
		BDBG_MSG_TRACE(("b_asf_stream_feed_audio_pcm: %#lx data %u:%u", (unsigned long)stream, data_len, audio->force_packet_size));
		if(data_len<audio->force_packet_size) {
			break;
		}
		batom_cursor_from_accum(&first, stream->accum);
		BATOM_CLONE(&last, &first);
		data_len = batom_cursor_skip(&last, audio->force_packet_size);
		BDBG_ASSERT(data_len==audio->force_packet_size);
		frame = batom_accum_extract(stream->accum, &first, &last, NULL, NULL);
		batom_accum_trim(stream->accum, &last);
		if(frame) {
			BDBG_MSG_TRACE(("b_asf_stream_feed_audio_pcm>: %#lx:%#lx audio %s:%u %u", (unsigned long)stream->demux, (unsigned long)stream, hdr.pes.pts_valid?"pts":"no-pts", (unsigned)hdr.pes.pts, batom_len(frame)));
			hdr.pes.header_off = 4;
			hdr.pes.header_type = B_ASF_STREAM_VEC_FRAME;
            bmedia_bcma_hdr_init(&hdr, audio->force_packet_size);

			pes_atom = batom_from_vec_and_atom(batom_get_vec(stream->meta, 0), frame, &bmedia_bcma_atom, &hdr);
			batom_release(frame);
			if(pes_atom) {
				batom_pipe_push(stream->pipe, pes_atom);
			}
			/* Mark the pts as invalid for all following frames */
			hdr.pes.key_frame = hdr.pes.pts_valid = false;
		}
	}
	return;
}

static void
b_asf_stream_feed_audio_wma_split(basf_stream_t stream, const struct basf_payload_info *info, const struct basf_audio_stream_info *audio, bmedia_bcma_hdr *hdr)
{
	batom_cursor first,last;
	batom_accum_t accum;
	size_t block_size;
	batom_t atom;
	batom_t raw_atom;

	BDBG_MSG_TRACE(("b_asf_stream_feed_audio_wma_split>: %#lx:%#lx %u:%u", (unsigned long)stream->demux, (unsigned long)stream, info->media_object_size, audio->block_size));
	BSTD_UNUSED(info);

	accum = stream->accum;
	block_size = audio->block_size;
	batom_cursor_from_accum(&last, accum);
	BATOM_CLONE(&first, &last);
	batom_cursor_skip(&last, audio->stream.meta_length + block_size);
	/* create first atom */
	atom = batom_accum_extract(accum, &first, &last, &bmedia_bcma_atom, hdr);
	for(;;) {
		if(!atom) {
			BDBG_WRN(("b_asf_stream_feed_audio_wma_split: %#lx:%#lx can't create audio atom",  (unsigned long)stream->demux, (unsigned long)stream));
			break;
		}
		batom_pipe_push(stream->pipe, atom);
		BDBG_MSG_TRACE(("b_asf_stream_feed_audio_wma_split: %#lx:%#lx %#lx:%u", (unsigned long)stream->demux, (unsigned long)stream, (unsigned long)atom, batom_len(atom)));
		hdr->pes.pts_valid = false; /* clear PTS */
        hdr->pes.key_frame = false;
		BATOM_CLONE(&first, &last);
		if(block_size!=batom_cursor_skip(&last, block_size)) {
			BDBG_MSG_TRACE(("b_asf_stream_feed_audio_wma_split: %#lx:%#lx processed %u:%u bytes", (unsigned long)stream->demux, (unsigned long)stream, info->media_object_size, batom_cursor_pos(&last)-audio->stream.meta_length));
			break;
		}
		/* extract portion of the audio stream */
		raw_atom = batom_accum_extract(accum, &first, &last, NULL, NULL);
		if(!raw_atom) {
			BDBG_WRN(("b_asf_stream_feed_audio_wma_split: %#lx:%#lx can't create split audio atom",  (unsigned long)stream->demux, (unsigned long)stream));
			break;
		}
		/* create new atom with the audio header */      
		atom = batom_from_vec_and_atom(batom_get_vec(stream->meta, 0), raw_atom, &bmedia_bcma_atom, hdr);
		batom_release(raw_atom);
	}
	batom_accum_clear(accum);
	BDBG_MSG_TRACE(("b_asf_stream_feed_audio_wma_split<: %#lx:%#lx %u:%u", (unsigned long)stream->demux, (unsigned long)stream, info->media_object_size, audio->block_size));
	return;
}

static void
b_asf_stream_feed_audio_wma(basf_stream_t stream, const struct basf_payload_info *info) {
	const struct basf_audio_stream_info *audio;
	batom_t atom;
	bmedia_bcma_hdr hdr;
	size_t payload_length;

	BDBG_OBJECT_ASSERT(stream, basf_stream_t);
	BDBG_OBJECT_ASSERT(stream->demux, basf_demux_t);
	BDBG_ASSERT(stream->pipe);
	BDBG_ASSERT(stream->accum);
	BDBG_ASSERT(info);
	audio = (struct basf_audio_stream_info *) stream->stream_info;
	BDBG_ASSERT(audio);

	BMEDIA_PACKET_HEADER_INIT(&hdr.pes);
	hdr.pes.header_type = B_ASF_STREAM_VEC_FRAME;
	hdr.pes.header_off = 4;
	hdr.pes.pts_valid = true;
	hdr.pes.key_frame = true;
	hdr.pes.pts = b_asf_get_pts(info->pres_time, stream->demux->preroll, stream->demux->time_scale);
	if (info->media_object_size%audio->block_size == 0) {
		payload_length = audio->block_size;
	} else {
		payload_length = info->media_object_size;
		BDBG_WRN(("b_asf_stream_feed_audio_wma: %#lx invalid audio object size %u (block_size %u)", (unsigned long)stream->demux, (unsigned)info->media_object_size, (unsigned)audio->block_size));
	}
    bmedia_bcma_hdr_init(&hdr, payload_length);
	if(payload_length==info->media_object_size) {
		atom = batom_from_accum(stream->accum, &bmedia_bcma_atom, &hdr);
		if(atom) {
			batom_pipe_push(stream->pipe, atom);
			BDBG_MSG_TRACE(("b_asf_stream_feed_audio_wma<: %#lx:%#lx %u->%#x", (unsigned long)stream->demux, (unsigned long)stream, info->media_object_size, (unsigned long)atom));
		}
	} else {
		b_asf_stream_feed_audio_wma_split(stream, info, audio, &hdr);
		BDBG_MSG_TRACE(("b_asf_stream_feed_audio_wma<: %#lx:%#lx %u->SPLIT AUDIO", (unsigned long)stream->demux, (unsigned long)stream, info->media_object_size));
	}
}

static bmedia_video_pic_type 
b_asf_get_pic_type(basf_stream_t stream, const struct basf_payload_info *info, batom_cursor *cursor)
{
	bmedia_video_pic_type pic_type;
	const struct basf_video_stream_info *video = (struct basf_video_stream_info *) stream->stream_info;
	BDBG_ASSERT(stream);
	BDBG_ASSERT(video);
	BDBG_MSG_TRACE(("b_asf_get_pic_type>: %#lx %#lx", (unsigned long)stream, (unsigned long)video));

	switch(video->stream.stream_type) {
	case b_asf_stream_type_mp4p2:
        pic_type = bmpeg4_video_read_pic_type(cursor);
        break;
	case b_asf_stream_type_vc1_ap:
		pic_type = bmedia_vc1ap_read_pic_type(cursor, &video->seq_hdr.vc1_ap);
		break;
	case b_asf_stream_type_vc1_sm:
        pic_type = (info->media_object_size<=1) ? bmedia_video_pic_type_P : bmedia_vc1sm_read_pic_type(cursor, &video->seq_hdr.vc1_sm);
        break;
	default:
		pic_type = bmedia_video_pic_type_unknown;
		break;
	}
	BDBG_MSG_TRACE(("b_asf_get_pic_type<: %#lx %#lx pic_type:%u", (unsigned long)stream, (unsigned long)video, (unsigned)pic_type));
	return pic_type;
}
static void 
b_asf_stream_object_reordering_queue_init(b_asf_stream_object_reordering_queue *queue)
{
    BLST_S_INIT(&queue->payloads);
    queue->valid = false;
    queue->free_payload = 0;
    queue->media_object_size = 0;
    queue->pres_time = 0;
    queue->media_object_queued = 0;
    return;
}

static void 
b_asf_stream_object_reordering_queue_flush(b_asf_stream_object_reordering_queue *queue)
{
    b_asf_stream_object_reordering_payload *payload;

    while(NULL!=(payload=BLST_S_FIRST(&queue->payloads))) {
        batom_release(payload->data);
        BLST_S_REMOVE_HEAD(&queue->payloads, link);
    }
    b_asf_stream_object_reordering_queue_init(queue);
    return;
}

static void 
b_asf_stream_object_reordering_queue_seed(basf_stream_t stream, b_asf_stream_object_reordering_queue *queue, batom_accum_t data, const struct basf_payload_info *info)
{
    b_asf_stream_object_reordering_payload *payload;

    if(queue->valid) {
        b_asf_stream_object_reordering_queue_flush(queue);
    }
    if(batom_accum_len(data)) {
		if(stream->stream_info->stream_type==b_asf_stream_type_wma) {
            batom_cursor temp_cursor;

            /* trim BCMA header */
            batom_cursor_from_accum(&temp_cursor, data);
            batom_cursor_skip(&temp_cursor, batom_len(stream->meta));
            batom_accum_trim(data, &temp_cursor);
        }
        BDBG_MSG(("b_asf_stream_object_reordering_queue_seed:%#lx adding %u bytes out of %u", (unsigned long)stream, batom_accum_len(data), info->media_object_size));
        queue->media_object_size = info->media_object_size;
        queue->pres_time = info->pres_time;
        queue->valid = true;
        payload = queue->reordering_buffer + queue->free_payload;
        payload->offset_into_media_object = 0;
        payload->data = batom_from_accum(data, NULL, NULL);
        if(payload->data) {
            BLST_S_INSERT_HEAD(&queue->payloads, payload, link);
        }
        queue->free_payload++;
    }
    return;
}

static bool
b_asf_stream_object_reordering_queue_enqueue(basf_stream_t stream, b_asf_stream_object_reordering_queue *queue, batom_accum_t source, const batom_cursor *packet, batom_cursor *replicated_data, const struct basf_payload_info *info, batom_accum_t dest)
{
    b_asf_stream_object_reordering_payload *payload_insert;
    b_asf_stream_object_reordering_payload *payload;
    batom_cursor temp_cursor;
    size_t len;

    BDBG_MSG_TRACE(("%s:%#lx enqueue %u bytes total %u:%u", "b_asf_stream_object_reordering_queue_enqueue", (unsigned long)stream, (unsigned)info->payload_length, (unsigned)info->media_object_size, (unsigned)queue->media_object_queued));
    if(queue->valid) {
        if(queue->pres_time!=info->pres_time || queue->media_object_size!=info->media_object_size || queue->media_object_queued+info->payload_length>info->media_object_size || queue->free_payload==B_ASF_STREAM_OBJECT_REORDERING_QUEUE_SIZE) {
            BDBG_MSG(("%s:%#lx flushing %u:%u out-of-order payloads pres_time:%u:%u media_object_size:%u:%u(%u)", "b_asf_stream_object_reordering_queue_enqueue", (unsigned long)stream, queue->free_payload, queue->media_object_queued, queue->pres_time, info->pres_time, queue->media_object_size, info->media_object_size, queue->media_object_queued+info->payload_length));
            b_asf_stream_object_reordering_queue_flush(queue);
        }
    }
    payload_insert=NULL;
    if(queue->valid) {
        for(payload_insert=NULL, payload=BLST_S_FIRST(&queue->payloads);payload;payload=BLST_S_NEXT(payload,link)) {
            if(payload->offset_into_media_object>=info->offset_into_media_object) {
                break;
            }
            payload_insert = payload;
        }
    } else {
        queue->media_object_size = info->media_object_size;
        queue->pres_time = info->pres_time;
        queue->free_payload = 0;
        queue->media_object_queued = 0;
        queue->valid = true;
    }

    if(queue->free_payload>=B_ASF_MAX_PAYLOADS) {
        BDBG_WRN(("%s:%#lx overflow of reordering queue %u bytes total %u:%u", "b_asf_stream_object_reordering_queue_enqueue", (unsigned long)stream, (unsigned)queue->free_payload, (unsigned)info->media_object_size, (unsigned)queue->media_object_queued));
        goto error;
    }
    payload = queue->reordering_buffer + queue->free_payload;
    if(stream->cfg.payload) {
        BATOM_CLONE(&temp_cursor, packet);
        stream->cfg.payload(stream->cfg.stream_cnxt, source, &temp_cursor, replicated_data, info);
    }
 	BATOM_CLONE(&temp_cursor, packet);
    len = batom_cursor_skip(&temp_cursor, info->payload_length);
    if(len!=info->payload_length) {
	    BDBG_WRN(("%s:%#lx corrupted packet %u:%u", "b_asf_stream_object_reordering_queue_enqueue", (unsigned long)stream, (unsigned)len, (unsigned)info->payload_length));
        goto error;
    }
    payload->data = batom_accum_extract(source, packet, &temp_cursor, NULL, NULL);
    if(!payload->data) {
	    BDBG_WRN(("%s:%#lx out of memory %u", "b_asf_stream_object_reordering_queue_enqueue", (unsigned long)stream, (unsigned)info->payload_length));
        goto error;
    }
    if(payload_insert) {
        BLST_S_INSERT_AFTER(&queue->payloads, payload_insert, payload, link);
    } else {
        BLST_S_INSERT_HEAD(&queue->payloads, payload, link);
    }
    queue->free_payload++;
    payload->offset_into_media_object = info->offset_into_media_object;
    queue->media_object_queued += info->payload_length;
    if(queue->media_object_queued == info->media_object_size) { /* accumulated entire object */

		if(stream->stream_info->stream_type==b_asf_stream_type_wma) {
			/* add WMA header */
			batom_accum_add_atom(dest, stream->meta);
		}
        while(NULL!=(payload=BLST_S_FIRST(&queue->payloads))) {
            if(payload->offset_into_media_object!=batom_accum_len(dest)) {
	            BDBG_WRN(("%s:%#lx out of order %u:%u", "b_asf_stream_object_reordering_queue_enqueue", (unsigned long)stream, (unsigned)payload->offset_into_media_object, batom_accum_len(dest)));
                batom_accum_clear(dest);
                goto error;
            }
            batom_accum_add_atom(dest, payload->data);
            batom_release(payload->data);
            BLST_S_REMOVE_HEAD(&queue->payloads, link);
        }
        BDBG_MSG(("%s:%#lx enqueue completed %u %u:%u", "b_asf_stream_object_reordering_queue_enqueue", (unsigned long)stream, queue->free_payload, (unsigned)info->media_object_size, (unsigned)info->pres_time));
        /* b_asf_stream_object_reordering_queue_init(queue); */
        queue->valid = false;
        return true;
    } else if(queue->free_payload>=B_ASF_MAX_PAYLOADS) {
        BDBG_WRN(("%s:%#lx exceeded depth %u of reordering queue %u:%u", "b_asf_stream_object_reordering_queue_enqueue", (unsigned long)stream, queue->free_payload, (unsigned)info->media_object_size, (unsigned)queue->media_object_queued));
        goto error;
    }
    return false;
   
error:
    if(queue->valid) {
        b_asf_stream_object_reordering_queue_flush(queue);
    }
    return false;
}

static void
b_asf_stream_queue_flush(basf_stream_t stream)
{
	batom_t atom;
	batom_pipe_t stream_pipe;
	batom_pipe_t queue_pipe;
	uint32_t last_pts;
	BDBG_MSG_TRACE(("b_asf_stream_queue_flush>: %#lx:%#lx nframes:%u", (unsigned long)stream->demux, (unsigned long)stream, (unsigned long)stream->queue.nframes));
	BDBG_OBJECT_ASSERT(stream, basf_stream_t);

	BDBG_ASSERT(stream->pipe);
	stream_pipe = stream->pipe;
	BDBG_ASSERT(stream->queue.pipe);
	queue_pipe = stream->queue.pipe;
	/* 1.  insert sequence header, (it relies on the fact that first frame is
	 * key frame and key frame is I frame */
	if(stream->queue.key_frame) {
		bmedia_packet_header hdr;

		stream->queue.key_frame = false;
		BDBG_ASSERT(stream->stream_info);
		BDBG_ASSERT(stream->meta);
		BMEDIA_PACKET_HEADER_INIT(&hdr);
		stream->queue.key_frame = 0;
		if(stream->stream_info->stream_type == b_asf_stream_type_vc1_sm) {
			hdr.pts_valid = true;
			hdr.key_frame = false;
			hdr.pts = B_MEDIA_MARKER_PTS;
		}
		hdr.meta_header = true;
		atom = batom_clone(stream->meta, bmedia_atom, &hdr);
		if (!atom) {
			goto error;
		}
		batom_pipe_push(stream_pipe, atom);
	}

	/* extract frames from the queue */
	for(last_pts = stream->queue.last_pts;NULL!=(atom = batom_pipe_pop(queue_pipe));) {
		uint32_t temp_pts; /* used to swap pts */
		bmedia_packet_header *pes = batom_userdata(atom);
		BDBG_ASSERT(pes);
		/* swap PTS'es, first frame in the pipe is the I or P frame and rest
		 * are B frames:
		 * for N frames in the pipe:
		 * if N = 0  (I or P frame)
		 * 	PTS(0) = PTS(N-1);
		 * else  (B frame)
		 * 	PTS(N) = PTS(N-1)
		 */
		temp_pts = pes->pts;
		pes->pts = last_pts;
		last_pts = temp_pts;
		batom_pipe_push(stream_pipe, atom);
		BDBG_MSG_TRACE(("b_asf_stream_queue_flush: %#lx:%#lx %#x(%u) pts:(new:%u old:%u)", (unsigned long)stream->demux, (unsigned long)stream, (unsigned long)atom, batom_len(atom), (unsigned)pes->pts, (unsigned)last_pts));
	}
done:
	BDBG_MSG_TRACE(("b_asf_stream_queue_flush<: %#lx:%#lx", (unsigned long)stream->demux, (unsigned long)stream));
	stream->queue.nframes = 0;
	return;
error:
	batom_pipe_flush(queue_pipe);
	goto done;
}

static void
b_asf_stream_feed_video_generic(basf_stream_t stream, const struct basf_payload_info *info) {
	batom_t atom;
	bmedia_packet_header hdr;

	BDBG_OBJECT_ASSERT(stream, basf_stream_t);
	BDBG_OBJECT_ASSERT(stream->demux, basf_demux_t);
	BDBG_ASSERT(stream->pipe);
	BDBG_ASSERT(stream->accum);
	BDBG_ASSERT(info);

	if(info->key_frame && stream->meta && batom_len(stream->meta)) {
		BMEDIA_PACKET_HEADER_INIT(&hdr);
		hdr.meta_header = true;
		hdr.key_frame = false;

		/* send a sequence header */
		atom = batom_clone(stream->meta, bmedia_atom, &hdr);
		if (!atom) {
			goto error;
		}
		BDBG_MSG_TRACE(("b_asf_stream_feed_video_generic>: %#lx:%#lx video meta header", (unsigned long)stream->demux, (unsigned long)stream));
		batom_pipe_push(stream->pipe, atom);
	}

	BMEDIA_PACKET_HEADER_INIT(&hdr);
	hdr.header_type = B_ASF_STREAM_VEC_FRAME;
	hdr.pts = b_asf_get_pts(info->pres_time, stream->demux->preroll, stream->demux->time_scale);
	hdr.pts_valid = true;
	hdr.key_frame = info->key_frame;
	
	BDBG_MSG_TRACE(("b_asf_stream_feed_video_generic>: %#lx:%#lx video %s:%u %u", (unsigned long)stream->demux, (unsigned long)stream, hdr.pts_valid?"pts":"no-pts", (unsigned)hdr.pts, batom_accum_len(stream->accum)));
	atom = batom_from_accum(stream->accum, bmedia_atom, &hdr);
	if(!atom) {
		goto error;
	}
	batom_pipe_push(stream->pipe, atom);
	return;
error:
	batom_accum_clear(stream->accum);
	BDBG_MSG_TRACE(("b_asf_stream_feed_video_generic<: %#lx:%#lx %u->NULL", (unsigned long)stream->demux, (unsigned long)stream, info->media_object_size));
}

static void
b_asf_stream_feed_video_mp4v3(basf_stream_t stream, const struct basf_payload_info *info) {
	batom_t atom;
	bmedia_packet_header hdr;

	BDBG_OBJECT_ASSERT(stream, basf_stream_t);
	BDBG_OBJECT_ASSERT(stream->demux, basf_demux_t);
	BDBG_ASSERT(stream->pipe);
	BDBG_ASSERT(stream->accum);
	BDBG_ASSERT(info);

	if(info->key_frame && stream->meta && batom_len(stream->meta)) {
		BMEDIA_PACKET_HEADER_INIT(&hdr);
		hdr.pts = B_MEDIA_MARKER_PTS;
		hdr.pts_valid = true;
		hdr.meta_header = true;
		hdr.key_frame = false;

		/* send a sequence header */
		atom = batom_clone(stream->meta, bmedia_atom, &hdr);
		if (!atom) {
			goto error;
		}
		BDBG_MSG_TRACE(("b_asf_stream_feed_video_mp4v3>: %#lx:%#lx video meta header", (unsigned long)stream->demux, (unsigned long)stream));
		batom_pipe_push(stream->pipe, atom);
	}

	BMEDIA_PACKET_HEADER_INIT(&hdr);
	hdr.header_type = B_ASF_STREAM_VEC_FRAME;
	hdr.pts = b_asf_get_pts(info->pres_time, stream->demux->preroll, stream->demux->time_scale);
	if(hdr.pts==B_MEDIA_MARKER_PTS) {
		hdr.pts -= 1;
	}
	hdr.pts_valid = true;
	hdr.key_frame = info->key_frame;

	BDBG_MSG_TRACE(("b_asf_stream_feed_video_mp4v3>: %#lx:%#lx video %s:%u %u", (unsigned long)stream->demux, (unsigned long)stream, hdr.pts_valid?"pts":"no-pts", (unsigned)hdr.pts, batom_accum_len(stream->accum)));
	atom = batom_from_accum(stream->accum, bmedia_atom, &hdr);
	if(!atom) {
		goto error;
	}
	batom_pipe_push(stream->pipe, atom);
	return;
 error:
	batom_accum_clear(stream->accum);
	BDBG_MSG_TRACE(("b_asf_stream_feed_video_mp4v3<: %#lx:%#lx %u->NULL", (unsigned long)stream->demux, (unsigned long)stream, info->media_object_size));
}

static void
b_asf_stream_feed_video_vc1(basf_stream_t stream, const struct basf_payload_info *info) {
	batom_t atom;
	bmedia_packet_header hdr;
	batom_accum_t accum;
	bmedia_video_pic_type  pic_type;
	uint32_t pts;

	BDBG_OBJECT_ASSERT(stream, basf_stream_t);
	BDBG_OBJECT_ASSERT(stream->demux, basf_demux_t);
	BDBG_ASSERT(stream->stream_info);
	BDBG_ASSERT(info);
	accum = stream->accum;
	BDBG_ASSERT(accum);

	BMEDIA_PACKET_HEADER_INIT(&hdr);
	pts = b_asf_get_pts(info->pres_time, stream->demux->preroll, stream->demux->time_scale);

	switch(stream->stream_info->stream_type) {
	case b_asf_stream_type_vc1_ap:
	  if(!b_asf_stream_probe_vc1_scode(accum)) {
		  hdr.header_type = B_ASF_STREAM_VEC_FRAME;
	  }
	  break;
	case b_asf_stream_type_vc1_sm:
	  if(pts==B_MEDIA_MARKER_PTS) {
		  pts -= 1;
	  }
	  hdr.header_type = B_ASF_STREAM_VEC_FRAME;
	  break;
	default:
	  break;
	}

	/* for VC-1 in ASF container, timestamps are stored in the display
	 * order and frames stored in the coded order. In order to associate
	 * frames with right timestamps we need to queue-up frames and reorder
	 * PTS'es */
	if(!stream->demux->key_frame_only) {
		batom_cursor cursor;

		batom_cursor_from_accum(&cursor, accum);
		/* 1. get picture type */
		pic_type = b_asf_get_pic_type(stream, info, &cursor);

		/* 2. create atom with pts */
		hdr.key_frame = (pic_type==bmedia_video_pic_type_I);
		hdr.pts_valid = true;
		hdr.pts = pts;
		atom = batom_from_accum(accum, bmedia_atom, &hdr);
		if(!atom) {
			goto error;
		}
        if(stream->cfg.reorder_timestamps) {
            /* 3. check picture type */	
            if(pic_type!=bmedia_video_pic_type_B) {
                /* 4. if not B then flush queue */
                b_asf_stream_queue_flush(stream);
                stream->queue.key_frame = info->key_frame;
            }

            /* 5. push frame into the queue */ 
            stream->queue.last_pts = pts;
            stream->queue.nframes++;
            batom_pipe_push(stream->queue.pipe, atom);
            BDBG_MSG_TRACE(("b_asf_stream_feed_video_vc1: %#lx queued frame %#lx (%u bytes)", (unsigned long)stream, (unsigned long)atom, batom_len(atom)));
        } else {
            /* push frame into the queue */ 
            stream->queue.key_frame = info->key_frame;
            stream->queue.last_pts = pts;
            stream->queue.nframes++;
            batom_pipe_push(stream->queue.pipe, atom);
            BDBG_MSG_TRACE(("b_asf_stream_feed_video_vc1: %#lx queued frame %#lx (%u bytes)", (unsigned long)stream, (unsigned long)atom, batom_len(atom)));
            /* and immediatly flush it out */
            b_asf_stream_queue_flush(stream);
        }
	} else { /* key frame only mode */
		hdr.key_frame = true;
		hdr.pts_valid = true;
		hdr.pts = pts;
		atom = batom_from_accum(accum, bmedia_atom, &hdr);
		if(!atom) {
			goto error;
		}

		/* push frame into the queue */ 
		stream->queue.key_frame = info->key_frame;
		pic_type = bmedia_video_pic_type_I;
		stream->queue.last_pts = pts;
		stream->queue.nframes++;
		batom_pipe_push(stream->queue.pipe, atom);
		/* and immediatly flush it out */
		b_asf_stream_queue_flush(stream);
	}

	BDBG_MSG_TRACE(("b_asf_stream_feed_video_vc1>: %#lx:%#lx queued frame:%#lx:%u:%u nframes:%u", (unsigned long)stream->demux, (unsigned long)stream, (unsigned long)atom, pic_type, pts, (unsigned)stream->queue.nframes));
	return;
 error:
	batom_accum_clear(accum);
	BDBG_MSG_TRACE(("b_asf_stream_feed_video_vc1<: %#lx:%#lx %u->NULL", (unsigned long)stream->demux, (unsigned long)stream, info->media_object_size));
	return;
}

static void 
b_asf_stream_feed_auxiliary(basf_stream_t stream, const struct basf_payload_info *info)
{
	batom_t atom;
	bmedia_packet_header hdr;

	BDBG_OBJECT_ASSERT(stream, basf_stream_t);
	BDBG_OBJECT_ASSERT(stream->demux, basf_demux_t);
	BDBG_ASSERT(stream->pipe);
	BDBG_ASSERT(stream->accum);
	BDBG_ASSERT(info);
	BMEDIA_PACKET_HEADER_INIT(&hdr);

	atom = batom_from_accum(stream->accum, bmedia_atom, &hdr);
	if(!atom) {
		goto error;
	}
	batom_pipe_push(stream->pipe, atom);
	return;
error:
	batom_accum_clear(stream->accum);
	BDBG_MSG_TRACE(("b_asf_stream_feed_auxiliary<: %#lx:%#lx %u->NULL", (unsigned long)stream->demux, (unsigned long)stream, info->media_object_size));

}

static void
b_asf_stream_accum_process_object(basf_stream_t stream, const struct basf_payload_info *info, batom_cursor *temp_cursor)
{
	batom_accum_t accum = stream->accum;

    BDBG_MSG(("b_asf_stream_accum_process_object: %#lx complete object %u:%u", (unsigned long)stream, (unsigned)batom_accum_len(accum), (unsigned)info->pres_time));
#if B_ASF_CAPTURE_DATA 
    {
        static FILE *fp=NULL;
        batom_cursor_from_accum(temp_cursor, accum);
        if(stream->stream_info->stream_type==b_asf_stream_type_wma) {
            /* skip WMA header */
            batom_cursor_skip(temp_cursor, batom_len(stream->meta));
        }
        b_asf_p_capture_data(&fp, stream, "asf_object.dat", temp_cursor, batom_cursor_size(temp_cursor));
    }
#endif
    if(stream->cfg.media_object) {
        batom_cursor_from_accum(temp_cursor, accum);
        if(stream->stream_info->stream_type==b_asf_stream_type_wma) {
            /* skip WMA header */
            batom_cursor_skip(temp_cursor, batom_len(stream->meta));
        }
        stream->cfg.media_object(stream->cfg.stream_cnxt, temp_cursor, info);
    }
    stream->stream_info->stream_handler(stream, info);
    return;
}

#if defined (HUMAX_PLATFORM_BASE)
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
#if defined(CONFIG_PLAYREADY_25)
const DRM_WCHAR rgwchStoreName [] = 
{
	ONE_WCHAR('/','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('l','\0'), ONE_WCHAR('i','\0'), ONE_WCHAR('b','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('h','\0'), ONE_WCHAR('u','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('x','\0'), ONE_WCHAR('t','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('d','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('y','\0'),ONE_WCHAR('r','\0'), ONE_WCHAR('e','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('d','\0'),ONE_WCHAR('y','\0'), ONE_WCHAR('2','\0'), ONE_WCHAR('5','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('s','\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('e', '\0'),
	ONE_WCHAR('.','\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('d','\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')
};
#else
const DRM_WCHAR rgwchStoreName [] = 
{
	ONE_WCHAR('/','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('l','\0'), ONE_WCHAR('i','\0'), ONE_WCHAR('b','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('h','\0'), ONE_WCHAR('u','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('a','\0'), ONE_WCHAR('x','\0'), ONE_WCHAR('t','\0'), ONE_WCHAR('v','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('d','\0'), ONE_WCHAR('r','\0'), ONE_WCHAR('m','\0'), ONE_WCHAR('/','\0'),
	ONE_WCHAR('s','\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('p','\0'), ONE_WCHAR('l','\0'), ONE_WCHAR('e', '\0'),
	ONE_WCHAR('.','\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('d','\0')
};
#endif

typedef enum
{
	BASF_ENCRYPTION_ALGORITHM_RC4=0,
	BASF_ENCRYPTION_ALGORITHM_AESCTR
}Basf_Encryption_Algorithm;

DRM_RESULT policy_callback(void *f_pvPolicyCallbackData, DRM_DWORD  f_dwCallbackType, void *f_pv)
{
	BSTD_UNUSED(f_pvPolicyCallbackData);
	BSTD_UNUSED(f_dwCallbackType);
	BSTD_UNUSED(f_pv);
	return DRM_SUCCESS;
}

DRM_RESULT commit_callback(void *f_pvPolicyCallbackData, DRM_DWORD  f_dwCallbackType,void *f_pv)
{
	BSTD_UNUSED(f_pvPolicyCallbackData);
	BSTD_UNUSED(f_dwCallbackType);
	BSTD_UNUSED(f_pv);
	return DRM_SUCCESS;
}

DRM_CONTENT_SET_PROPERTY b_asf_stream_check_header_version(basf_demux_t demux)
{
	unsigned int uiOffset=0;
	DRM_CONTENT_SET_PROPERTY eVersion=DRM_CSP_HEADER_NOT_SET;

/*************************
*	WRM Validation check
*************************/
	if(demux->wrmheaderlen < 60)
	{
		BDBG_ERR(("Invalid WRM HEADER! too small!\n"));
		return eVersion;
	}

/*************************
*	WRMHEADER tag with version info
*************************/
	while(uiOffset <= (demux->wrmheaderlen-18))
	{
		if((demux->wrmheader[uiOffset] == 'v') && (demux->wrmheader[uiOffset+2] == 'e') && (demux->wrmheader[uiOffset+4] == 'r') &&
			(demux->wrmheader[uiOffset+6] == 's') && (demux->wrmheader[uiOffset+8] == 'i') && (demux->wrmheader[uiOffset+10] == 'o') && (demux->wrmheader[uiOffset+12] == 'n'))
		{
			break;
		}
		uiOffset++;
	}

	if(demux->wrmheader[uiOffset+18] == '2')
	{
		eVersion = DRM_CSP_V2_HEADER;
	}
	else if(demux->wrmheader[uiOffset+18] == '4')
	{
		eVersion = DRM_CSP_V4_HEADER;
	}
	else
	{
		BDBG_ERR(("Invalid version!\n"));
	}
		
	return eVersion;
}

void b_asf_register_post_callback(httpclient_license_post_callback_t soap_callback, httpclient_license_post_callback_t default_callback)
{
	soap_post_callback = soap_callback;
	default_post_callback = default_callback;
}

void b_asf_register_get_secure_clock(get_secure_clock_callback_t callback)
{
	secure_clock_callback = callback;
}

void b_asf_stream_get_license(basf_demux_t demux)
{
	DRM_RESULT dResult;
	DRM_APP_CONTEXT* pAppContext = NULL;
	DRM_VOID *f_pOEMContext = NULL;
	DRM_BYTE *__macro_pbOpaqueBuffer = NULL;
	const DRM_CONST_STRING dstrStoreName = CREATE_DRM_STRING(rgwchStoreName);
	DRM_CONTENT_SET_PROPERTY eHeaderVersion;
	DRM_DWORD cRights = 1;
	const DRM_CONST_STRING *apdcsRights [cRights];
	const DRM_CONST_STRING *rgpdstrRights [1] = { 0 };
	DRM_DWORD f_cRights=0;
	DRM_CHAR* f_pchSilentURL=NULL;
	DRM_DWORD f_pcchSilentURL=0;
	DRM_BYTE* f_pbChallenge=NULL;
	DRM_DWORD f_pcbChallenge=0;
	DRM_BYTE* pucResponseData=NULL;
	DRM_BYTE* pucResponseStart=NULL;
	DRM_WCHAR32 uiResponseSize=0;
	DRM_BYTE *pbRevocationBuffer=NULL;
	DRM_LICENSE_RESPONSE LicenseResponse;
	int iResult;
	FILE* fd = NULL;
	int cookielen=0;
	int new_urllen=0;
	unsigned char* cookie=NULL;
	unsigned char* new_url=NULL;

	BKNI_Memset(&LicenseResponse, 0, sizeof (DRM_LICENSE_RESPONSE));
	apdcsRights[0] = (const DRM_CONST_STRING *)&g_dstrWMDRM_RIGHT_PLAYBACK;
	
	pAppContext = (DRM_APP_CONTEXT*)Oem_MemAlloc(SIZEOF(DRM_APP_CONTEXT));
	if(pAppContext == NULL)
	{
		BDBG_ERR(("Out of memory!\n"));
		return;
	}

	__macro_pbOpaqueBuffer = (DRM_BYTE*)Oem_MemAlloc(MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE);
	if(__macro_pbOpaqueBuffer == NULL)
	{
		BDBG_ERR(("Out of memory!\n"));
		Oem_MemFree(pAppContext);
		return;
	}
	
#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
	f_pOEMContext = (DRM_VOID*)Oem_Create();
	if(f_pOEMContext == NULL)
	{
		BDBG_ERR(("Oem_Create failed!\n"));
		Oem_MemFree(pAppContext);
		Oem_MemFree(__macro_pbOpaqueBuffer);
		return;
	}
#endif

/*************************
*	1. Drm Init
*************************/
	dResult = Drm_Initialize(pAppContext, f_pOEMContext, __macro_pbOpaqueBuffer, MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE, &dstrStoreName);
	if(dResult != DRM_SUCCESS)
	{
		BDBG_ERR(("Drm_Initialize failed! 0x%X\n", dResult));
		goto Done;
	}

#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)
	if(secure_clock_callback != NULL)
	{
		secure_clock_callback(pAppContext);
	}
#endif

/*************************
*	2. Try to Bind
*************************/
	eHeaderVersion = b_asf_stream_check_header_version(demux);
	dResult = Drm_Content_SetProperty(pAppContext, eHeaderVersion, demux->wrmheader, demux->wrmheaderlen);
	if(dResult != DRM_SUCCESS)
	{
		BDBG_ERR(("Drm_Content_SetProperty failed! 0x%X\n", dResult));
		goto Done;
	}

	dResult = Drm_Reader_Bind(pAppContext, apdcsRights, cRights, (DRMPFNPOLICYCALLBACK)policy_callback, NULL, &demux->stDecryptContext);
	if(dResult == DRM_SUCCESS)
	{
		BDBG_ERR(("License is already in SecureStore!\n"));
		goto Done;
	}

/*************************
*	3. make challenge
*************************/
	rgpdstrRights[0] = &g_dstrWMDRM_RIGHT_PLAYBACK;
	f_cRights = NO_OF(rgpdstrRights);
	
	dResult = Drm_LicenseAcq_GenerateChallenge(pAppContext,
		rgpdstrRights,
		f_cRights,
		NULL,
		NULL,
		0,
		NULL,
		&f_pcchSilentURL,
		NULL,
		NULL,
		NULL,
		&f_pcbChallenge);
	if(dResult == DRM_E_BUFFERTOOSMALL)
	{
		f_pchSilentURL = (DRM_CHAR*) Oem_MemAlloc(f_pcchSilentURL * SIZEOF (DRM_CHAR));
	    f_pbChallenge = (DRM_BYTE*) Oem_MemAlloc(f_pcbChallenge * SIZEOF(DRM_BYTE));

		dResult = Drm_LicenseAcq_GenerateChallenge(pAppContext,
			rgpdstrRights,
			f_cRights,
			NULL,
			NULL,
			0,
			f_pchSilentURL,
			&f_pcchSilentURL,
			NULL,
			NULL,
			f_pbChallenge,
			&f_pcbChallenge);
		f_pbChallenge[f_pcbChallenge] = '\0';
	}
	else
	{
		BDBG_ERR(("Drm_LicenseAcq_GenerateChallenge failed! 0x%X\n", dResult));
		goto Done;
	}

/*************************
*	4. Add Maxdome Cookie
*************************/
	fd = fopen("/var/lib/humaxtv/drm/cookie.txt", "r");
	if(fd != NULL)
	{
		fseek(fd, 0, SEEK_END);
		cookielen = ftell(fd);
		fseek(fd, 0, SEEK_SET);	

		if(cookielen != 0)
		{
			cookie = BKNI_Malloc(cookielen);
			if(cookie == NULL)
			{
				BDBG_ERR(("out of memory!\n"));
				goto Done;
			}
			fread(cookie, cookielen, 1, fd);
			fclose(fd);
			system("rm -rf /var/lib/humaxtv/drm/cookie.txt");

			new_urllen = f_pcchSilentURL + cookielen + 1 + 1;
			new_url = Oem_MemAlloc(new_urllen);
			if(new_url == NULL)
			{
				BKNI_Free(cookie);
				goto Done;
			}
			snprintf((char*)new_url ,new_urllen, "%s&%s", f_pchSilentURL, cookie);
			BDBG_ERR(("[%s][%d] New Url: %s\n\n\n", __FUNCTION__,__LINE__, new_url));

			BKNI_Free(cookie);

			if(f_pchSilentURL != NULL)
			{
				Oem_MemFree(f_pchSilentURL);
			}
			f_pchSilentURL = new_url;
			f_pcchSilentURL = new_urllen;
		}
	}	
	
/*************************
*	5. Post
*************************/
	if(eHeaderVersion == DRM_CSP_V4_HEADER)
	{
		iResult = soap_post_callback(
			(char*)f_pchSilentURL, 
			(char*)f_pbChallenge, 
			0, 
			2000, 
			HDR_LICGET_ACTION_VALUE,
			&pucResponseData, 
			&pucResponseStart, 
			&uiResponseSize);
		if(iResult != 0)
		{
			BDBG_ERR(("bhttpclient_license_post_soap failed!\n"));
			goto Done;
		}
	}
	else
	{
		iResult = default_post_callback(
				(char*)f_pchSilentURL, 
				(char*)f_pbChallenge,
				0, 
				2000, 
				NULL,
				&pucResponseData, 
				&pucResponseStart, 
				&uiResponseSize);
		if(iResult != 0)
		{
			BDBG_ERR(("bhttpclient_license_post_default failed!\n"));
			goto Done;
		}
	}

/*************************
*	6. Process
*************************/
	pbRevocationBuffer = (DRM_BYTE *)Oem_MemAlloc(REVOCATION_BUFFER_SIZE);
	dResult = Drm_Revocation_SetBuffer(pAppContext, pbRevocationBuffer, REVOCATION_BUFFER_SIZE);
	if(dResult != DRM_SUCCESS)
	{
		BDBG_ERR(("Drm_Revocation_SetBuffer failed! [%x]\n", dResult));
	}

	dResult = Drm_LicenseAcq_ProcessResponse(pAppContext, DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
		NULL, 
		NULL,
		pucResponseStart,
		uiResponseSize,
		&LicenseResponse);
	if(dResult == DRM_SUCCESS)
	{
		BDBG_ERR(("License is Stored well!\n"));
	}
	else if (dResult == DRM_S_MORE_DATA)
	{
		BDBG_ERR(("License is Stored well, and some of custom data exist!\n"));
	}
	else
	{
		BDBG_ERR(("Drm_LicenseAcq_ProcessResponse failed 0x%X\n", dResult));
	}

/*************************
*	7. Bind again
*************************/
	dResult = Drm_Reader_Bind(pAppContext, apdcsRights, cRights, (DRMPFNPOLICYCALLBACK)policy_callback, NULL, &demux->stDecryptContext);
	if(dResult != DRM_SUCCESS)
	{
		BDBG_ERR(("Drm_Reader_Bind failed! 0x%X\n", dResult));
		goto Done;
	}

/*************************
*	8. commit
*************************/
	dResult = Drm_Reader_Commit(pAppContext, (DRMPFNPOLICYCALLBACK)commit_callback, NULL);
	if(dResult != DRM_SUCCESS)
	{
		BDBG_ERR(("Drm_Reader_Commit failed! 0x%X\n", dResult));
		goto Done;
	}

Done:
#if defined(CONFIG_PLAYREADY_25)
	demux->pAppContext = pAppContext;
	demux->bSetContext = TRUE;
#else
	Drm_Uninitialize(pAppContext);
	if(pAppContext != NULL)
	{
		Oem_MemFree(pAppContext);
	}
#endif
	if(f_pbChallenge != NULL)
	{
		Oem_MemFree(f_pbChallenge);
	}
	if(f_pchSilentURL != NULL)
	{
		Oem_MemFree(f_pchSilentURL);
	}
	if(pbRevocationBuffer != NULL)
	{
		Oem_MemFree(pbRevocationBuffer);
	}
	if(__macro_pbOpaqueBuffer != NULL)
	{
		Oem_MemFree(__macro_pbOpaqueBuffer);
	}
	if(pucResponseData != NULL)
	{
		Oem_MemFree(pucResponseData);
	}

#if defined(CONFIG_BRCM_MOD_PRDY_ROBUSTNESS_ENABLE)	
#if defined(CONFIG_PLAYREADY_25)
	demux->f_pOEMContext = f_pOEMContext;
#else
	if(f_pOEMContext != NULL)
	{
		Oem_Destroy(f_pOEMContext);
		f_pOEMContext = NULL;
	}
#endif
#endif

	return;
}

static basf_parser_handler_action
b_asf_stream_wmdrm10_object(basf_parser_object_handler *handler,  batom_t header)
{
	basf_demux_t demux;
	batom_cursor cursor;

	CommonCryptoSettings cryptoSettings;
	NEXUS_Error nResult;
	NEXUS_DmaSettings dmaSettings;
	NEXUS_SecurityKeySlotSettings keySlotSettings;
	NEXUS_SecurityAlgorithmSettings nexusConfig;
	
	demux = ((struct b_asf_demux_object_handler *)handler)->demux;
	
	batom_cursor_from_atom(&cursor, header);

	if(demux->bWmDrmEncryptedContents == FALSE)
	{
/*************************
*	Data Size (4 Bytes)
*************************/
		demux->wrmheaderlen = basf_read_dword(&cursor);
	
/*************************
*	Wrm Header
*************************/
		demux->wrmheader = BKNI_Malloc(demux->wrmheaderlen);
		batom_cursor_copy(&cursor, demux->wrmheader, demux->wrmheaderlen);
		
/*************************
*	key setting
*************************/
		CommonCrypto_GetDefaultSettings(&cryptoSettings);
		if(demux->cryptoHandle == NULL)
		{
			demux->cryptoHandle = CommonCrypto_Open(&cryptoSettings);
		}
		
		NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
		keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

		if(demux->keyWmdrm == NULL) 
		{
			demux->keyWmdrm = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
		}
		
		if(demux->keyWmdrm == NULL) 
		{
		   BDBG_ERR(("NEXUS_Security_AllocateKeySlot failed\n"));
		   return basf_parser_handler_action_none;
		}	

	    NEXUS_Security_GetDefaultAlgorithmSettings(&nexusConfig);

	    nexusConfig.algorithm           = NEXUS_SecurityAlgorithm_eWMDrmPd;
	    nexusConfig.operation           = NEXUS_SecurityOperation_eDecrypt;
	    nexusConfig.terminationMode     = NEXUS_SecurityTerminationMode_eClear;
	    nexusConfig.enableExtKey        = TRUE;
	    nexusConfig.enableExtIv         = TRUE; 

	    nResult = NEXUS_Security_ConfigAlgorithm(demux->keyWmdrm, &nexusConfig);
	    if(nResult != NEXUS_SUCCESS)
		{
			BDBG_ERR(("NEXUS_Security_ConfigAlgorithm failed!\n"));
	        return basf_parser_handler_action_none;
	    }

		if(demux->keyPrdy == NULL)
		{
			demux->keyPrdy = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
		}
		
		if(demux->keyPrdy == NULL) 
		{
		   BDBG_ERR(("NEXUS_Security_AllocateKeySlot failed\n"));
		   return basf_parser_handler_action_none;
		}	

		NEXUS_Security_GetDefaultAlgorithmSettings(&nexusConfig);

		nexusConfig.algorithm			= NEXUS_SecurityAlgorithm_eAes128;
		nexusConfig.algorithmVar		= NEXUS_SecurityAlgorithmVariant_eCounter;
		nexusConfig.operation			= NEXUS_SecurityOperation_eDecrypt;
		nexusConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
		nexusConfig.enableExtKey		= TRUE;
		nexusConfig.enableExtIv 		= TRUE; 
		nexusConfig.aesCounterSize		= NEXUS_SecurityAesCounterSize_e64Bits;

		nResult = NEXUS_Security_ConfigAlgorithm(demux->keyPrdy, &nexusConfig);
		if(nResult != NEXUS_SUCCESS)
		{
			BDBG_ERR(("NEXUS_Security_ConfigAlgorithm failed!\n"));
			return basf_parser_handler_action_none;
		}

		NEXUS_Memory_Allocate(80, NULL, (void **)&(demux->pEkl));
		
/*************************
*	Get License
*************************/
		b_asf_stream_get_license(demux);
		demux->bWmDrmEncryptedContents = TRUE;
	}
	return basf_parser_handler_action_none;
}

static basf_parser_handler_action
b_asf_stream_playready_object(basf_parser_object_handler *handler,  batom_t header)
{
	basf_demux_t demux;
	batom_cursor cursor;
	basf_guid systemid;
	basf_dword sysversion;
	basf_dword psidatalen=0;
	basf_dword headerobjectlen=0;
	basf_word recordcount=0;
	basf_word recordtype=0;
	basf_word recordlen=0;
	unsigned int uiIndex=0;
	
	CommonCryptoSettings cryptoSettings;
	NEXUS_Error nResult;
	NEXUS_DmaSettings dmaSettings;
	NEXUS_SecurityKeySlotSettings keySlotSettings;
	NEXUS_SecurityAlgorithmSettings nexusConfig;
	
	demux = ((struct b_asf_demux_object_handler *)handler)->demux;
	
	batom_cursor_from_atom(&cursor, header);
	
	if(demux->bPlayReadyEncryptedContents == FALSE)
	{
/*************************
*	System ID 128 bits
*************************/
		basf_read_guid(&cursor, &systemid);
		
/*************************
*	System Version 32 bits
*************************/
		sysversion = basf_read_dword(&cursor);

/*************************
*	Data Size 32 bits
*************************/
		psidatalen = basf_read_dword(&cursor);

/*************************
*	Header Object Length 32 bits
*************************/
		headerobjectlen = basf_read_dword(&cursor);

/*************************
*	Record Count 16 bits
*************************/
		recordcount = basf_read_word(&cursor);

/*************************
*	WRM Header
*************************/
		for(uiIndex=0; uiIndex < recordcount; uiIndex++)
		{
			recordtype = basf_read_word(&cursor);
			recordlen = basf_read_word(&cursor);
			if(recordtype == 0x0001)
			{
				demux->wrmheader = BKNI_Malloc(recordlen);
				demux->wrmheaderlen = recordlen;
				batom_cursor_copy(&cursor, demux->wrmheader, demux->wrmheaderlen);
				break;
			}
			else
			{
				batom_cursor_skip(&cursor, recordlen);
			}
		}
		
/*************************
*	key setting
*************************/
		CommonCrypto_GetDefaultSettings(&cryptoSettings);
		if(demux->cryptoHandle == NULL)
		{
			demux->cryptoHandle = CommonCrypto_Open(&cryptoSettings);
		}
		
		NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
		keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

		if(demux->keyWmdrm == NULL) 
		{
			demux->keyWmdrm = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
		}
		
		if(demux->keyWmdrm == NULL) 
		{
		   BDBG_ERR(("NEXUS_Security_AllocateKeySlot failed\n"));
		   return basf_parser_handler_action_none;
		}	

	    NEXUS_Security_GetDefaultAlgorithmSettings(&nexusConfig);

	    nexusConfig.algorithm           = NEXUS_SecurityAlgorithm_eWMDrmPd;
	    nexusConfig.operation           = NEXUS_SecurityOperation_eDecrypt;
	    nexusConfig.terminationMode     = NEXUS_SecurityTerminationMode_eClear;
	    nexusConfig.enableExtKey        = TRUE;
	    nexusConfig.enableExtIv         = TRUE; 

	    nResult = NEXUS_Security_ConfigAlgorithm(demux->keyWmdrm, &nexusConfig);
	    if(nResult != NEXUS_SUCCESS)
		{
			BDBG_ERR(("NEXUS_Security_ConfigAlgorithm failed!\n"));
	        return basf_parser_handler_action_none;
	    }

		if(demux->keyPrdy == NULL)
		{
			demux->keyPrdy = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
		}
		
		if(demux->keyPrdy == NULL) 
		{
		   BDBG_ERR(("NEXUS_Security_AllocateKeySlot failed\n"));
		   return basf_parser_handler_action_none;
		}	

		NEXUS_Security_GetDefaultAlgorithmSettings(&nexusConfig);

		nexusConfig.algorithm			= NEXUS_SecurityAlgorithm_eAes128;
		nexusConfig.algorithmVar		= NEXUS_SecurityAlgorithmVariant_eCounter;
		nexusConfig.operation			= NEXUS_SecurityOperation_eDecrypt;
		nexusConfig.terminationMode 	= NEXUS_SecurityTerminationMode_eClear;
		nexusConfig.enableExtKey		= TRUE;
		nexusConfig.enableExtIv 		= TRUE; 
		nexusConfig.aesCounterSize		= NEXUS_SecurityAesCounterSize_e64Bits;

		nResult = NEXUS_Security_ConfigAlgorithm(demux->keyPrdy, &nexusConfig);
		if(nResult != NEXUS_SUCCESS)
		{
			BDBG_ERR(("NEXUS_Security_ConfigAlgorithm failed!\n"));
			return basf_parser_handler_action_none;
		}

		NEXUS_Memory_Allocate(80, NULL, (void **)&(demux->pEkl));
		
/*************************
*	Get License
*************************/
		b_asf_stream_get_license(demux);
		demux->bPlayReadyEncryptedContents = TRUE;
	}
	return basf_parser_handler_action_none;
}
#if defined(CONFIG_PLAYREADY_25)
void b_asf_stream_DoDecrypt_sw(basf_stream_t stream, unsigned char* pucData, unsigned int uiSize, unsigned char* iv)
#else
void b_asf_stream_DoDecrypt_sw(basf_stream_t stream, unsigned char* pucData, unsigned int uiSize)
#endif
{
	DRM_BYTE *last_15_bytes;
	DRM_RESULT dr;
#if defined(CONFIG_PLAYREADY_25)
	static DRM_AES_COUNTER_MODE_CONTEXT m_aes_context;
#endif

/*************************
*	1. NULL Check
*************************/
	if(pucData == NULL)
	{
		BDBG_ERR(("Buffer is NULL!\n"));
		return;
	}

/*************************
*	2. Cocktail decrypt
*************************/
	if (uiSize < 15)
	{
		last_15_bytes = (DRM_BYTE*)pucData;
	}
	else
	{
		last_15_bytes = (DRM_BYTE*)pucData + (uiSize - 15);
	}

#if defined(CONFIG_PLAYREADY_25)
	if(iv != NULL)
	{
		m_aes_context.qwInitializationVector = (DRM_UINT64)iv[0] << 56 | (DRM_UINT64)iv[1] << 48 | (DRM_UINT64)iv[2] << 40 | (DRM_UINT64)iv[3] << 32 | (DRM_UINT64)iv[4] << 24 | (DRM_UINT64)iv[5] << 16 | (DRM_UINT64)iv[6] << 8 | (DRM_UINT64)iv[7];
		m_aes_context.qwBlockOffset = 0;/*(DRM_UINT64)iv[8] << 56 | (DRM_UINT64)iv[9] << 48 | (DRM_UINT64)iv[10] << 40 | (DRM_UINT64)iv[11] << 32 | (DRM_UINT64)iv[12] << 24 | (DRM_UINT64)iv[13] << 16 | (DRM_UINT64)iv[14] << 8 | (DRM_UINT64)iv[15];*/
		m_aes_context.bByteOffset = 0;
	}
#endif
	dr = Drm_Reader_InitDecrypt(&stream->demux->stDecryptContext, last_15_bytes, uiSize); 
	if (dr == DRM_SUCCESS)
	{
#if defined(CONFIG_PLAYREADY_25)
		dr = Drm_Reader_Decrypt(&stream->demux->stDecryptContext, &m_aes_context, pucData, uiSize);
#else
		dr = Drm_Reader_Decrypt(&stream->demux->stDecryptContext, NULL, pucData, uiSize); 
#endif
	}
	else
	{
		BDBG_ERR(("InitDecrypt failed = 0x%x\n", dr));
	}
	return;
}

void b_asf_stream_DoDecrypt_hw(basf_stream_t stream, unsigned char* pucData, unsigned int uiSize, unsigned char* pIV, Basf_Encryption_Algorithm algorithm)
{
	uint32_t cbClear;
	DRM_CIPHER_CONTEXT *cipher_ctx;
	uint32_t ekl_size = 0;
	NEXUS_Error nResult;
	NEXUS_DmaJobBlockSettings blks[2];
	CommonCryptoJobSettings jobSettings;
	uint8_t *pEkl= NULL;
	DRM_BYTE *last_15_bytes;
	
/*************************
*	1. NULL Check
*************************/
	if(pucData == NULL)
	{
		BDBG_ERR(("Buffer is NULL!\n"));
		return;
	}

	if((algorithm == BASF_ENCRYPTION_ALGORITHM_AESCTR) && (pIV == NULL))
	{
		BDBG_ERR(("iv is null!\n"));
		return;
	}
	
	cipher_ctx = (DRM_CIPHER_CONTEXT* )stream->demux->stDecryptContext.rgbBuffer;
	if (!cipher_ctx->fInited)
	{
		BDBG_ERR(("Not Initialized!\n\n\n\n\n\n\n\n\n"));
		return;
	}

	pEkl = stream->demux->pEkl;

#if defined(CONFIG_PLAYREADY_25)
	b_asf_stream_DoDecrypt_sw(stream, pucData, uiSize, pIV);
#else
	if(algorithm == BASF_ENCRYPTION_ALGORITHM_RC4)
	{
		if(uiSize < 16)
		{
			b_asf_stream_DoDecrypt_sw(stream, pucData, uiSize);
			return;
		}
		last_15_bytes = (DRM_BYTE*)pucData + (uiSize - 15);
	
		/*8 byte DES key*/
		BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.shaOut[12],8);
		pEkl+=8;

		/*8 byte RC4 content key (the one at the end of of the ASF paylaod */
		cbClear = uiSize % 8;	/*Find Number of clear bytes*/
		BKNI_Memcpy(pEkl, &last_15_bytes[15-8-cbClear],8);
		pEkl+=8;

		/*8 byte D1 */
		/*D1 = "9.	From the 64 bytes obtained in step 4, use the last 8 bytes to XOR with the result of the DES encryption in step 8."*/
		BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.desS2[0],8);
		pEkl+=8;

		/*8 byte D0 */
		/*D0 = "From the 64 bytes obtained in step 4, the 8 bytes following the 48 bytes MAC key are used to XOR the RC4 content key obtained in step 5."*/
		BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.desS1[0],8);
		pEkl+=8;

		/*48 byte MAC key */
		BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.a1, 4);
		pEkl+=4;
		BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.b1, 4);
		pEkl+=4;
		BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.c1, 4);
		pEkl+=4;
		BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.d1, 4);
		pEkl+=4;
		BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.e1, 4);
		pEkl+=4;
		BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.f1, 4);
		pEkl+=4;
		BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.a2, 4);
		pEkl+=4;
		BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.b2, 4);
		pEkl+=4;
		BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.c2, 4);
		pEkl+=4;
		BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.d2, 4);
		pEkl+=4;
		BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.e2, 4);
		pEkl+=4;
		BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.f2, 4);

		ekl_size = 80;
	}
	else if(algorithm == BASF_ENCRYPTION_ALGORITHM_AESCTR)
	{
		BKNI_Memcpy(pEkl, cipher_ctx->cipher.cipherAES.rgbContentKey+8,8);
		BKNI_Memcpy(pEkl+8, cipher_ctx->cipher.cipherAES.rgbContentKey, 8);
		BKNI_Memset(&pEkl[16], 0, 8);
		BKNI_Memcpy(&pEkl[24], pIV, 8);
		ekl_size = 32;
	}
	else
	{
		BDBG_ERR(("Invalid algorithm!\n"));
		return;
	}
	
	NEXUS_DmaJob_GetDefaultBlockSettings(&blks[0]);
	blks[0].pSrcAddr				 = stream->demux->pEkl;
	blks[0].pDestAddr				 = stream->demux->pEkl;
	blks[0].blockSize				 = ekl_size;
	blks[0].cached					 = true;
	blks[0].resetCrypto 			 = true;
	blks[0].scatterGatherCryptoStart = true;
	blks[0].scatterGatherCryptoEnd	 = false;

	NEXUS_DmaJob_GetDefaultBlockSettings(&blks[1]);
	blks[1].pSrcAddr				 = pucData;
	blks[1].pDestAddr				 = pucData;
	blks[1].blockSize				 = uiSize;
	blks[1].cached					 = true;
	blks[1].resetCrypto 			 = true;
	blks[1].scatterGatherCryptoStart = false;
	blks[1].scatterGatherCryptoEnd	 = true;

	CommonCrypto_GetDefaultJobSettings(&jobSettings);
	
	if(algorithm == BASF_ENCRYPTION_ALGORITHM_RC4)
	{
		jobSettings.keySlot = stream->demux->keyWmdrm;
	}
	else if(algorithm == BASF_ENCRYPTION_ALGORITHM_AESCTR)
	{
		jobSettings.keySlot = stream->demux->keyPrdy;
	}
	
	nResult = CommonCrypto_DmaXfer(stream->demux->cryptoHandle, &jobSettings, &blks, 2);
	if(nResult != NEXUS_SUCCESS)
	{
		BDBG_ERR(("%s - CommonCrypto_DmaXfer failed rc %x\n", __FUNCTION__, nResult));
	}
#endif
	return;
}

void b_asf_stream_decrypt_rc4(basf_stream_t stream, const struct basf_payload_info *info, batom_cursor *temp_cursor)
{
	batom_checkpoint save;
	unsigned char* buffer=NULL;
	unsigned int off = 0, copy = 0;
	unsigned pos = 0;

/*************************
*	Hook data from cursor
*************************/
	NEXUS_Memory_Allocate(info->payload_length, NULL, &buffer);
	if(buffer == NULL)
	{
		BDBG_ERR(("Out of memory!\n"));
		return;
	}
	
	batom_cursor_save(temp_cursor, &save);
	batom_cursor_copy(temp_cursor, buffer, info->payload_length);

/*************************
*	Decrypt
*************************/
	b_asf_stream_DoDecrypt_hw(stream, buffer, info->payload_length, NULL, BASF_ENCRYPTION_ALGORITHM_RC4);
	
/*************************
*	Copy back to cursor
*************************/
	batom_cursor_rollback(temp_cursor, &save);
	if(temp_cursor->left >= info->payload_length)
	{
		BKNI_Memcpy((uint8_t*)temp_cursor->cursor, buffer, info->payload_length);
	}
	else
	{
		BDBG_MSG(("straddle!\n"));
		for (off=0; pos < info->payload_length; batom_cursor_skip(temp_cursor, temp_cursor->left+off)) {	 
			if(BATOM_IS_EOF(temp_cursor)) {
				BDBG_ASSERT(0); break;
			}

			if (pos+temp_cursor->left+off <= info->payload_length) {
				copy = temp_cursor->left+off;			   
			} else {
				copy = info->payload_length-pos;
			}
			BKNI_Memcpy((uint8_t*)temp_cursor->cursor-off, buffer, copy);
			buffer += copy;
			pos += copy;
			off = 1;				
		}
		buffer-=info->payload_length;
	}
	batom_cursor_rollback(temp_cursor, &save);
	
	if(buffer != NULL)
	{
		NEXUS_Memory_Free(buffer);
		buffer = NULL;
	}
}

static void b_asf_stream_decrypt_aesctr(basf_stream_t stream, const struct basf_payload_info *info, batom_cursor *temp_cursor, batom_cursor* replicated_data)
{	
	batom_checkpoint save;	
	unsigned char* buffer=NULL; 
	unsigned int off = 0, copy = 0; 
	unsigned pos = 0;	
	batom_checkpoint start_repl;			
	uint8_t repl_data[8];

/*************************
*	Get IV
*************************/	
	BATOM_SAVE(replicated_data, &start_repl);			
	batom_cursor_skip(replicated_data, info->replicated_data_length - 8);			
	batom_cursor_copy(replicated_data, &repl_data, 8);			
	batom_cursor_rollback(replicated_data, &start_repl);

	batom_cursor_from_accum(temp_cursor, stream->accum);	
	if(stream->stream_info->stream_type==b_asf_stream_type_wma) 	
	{		
		batom_cursor_skip(temp_cursor, batom_len(stream->meta));	
	}	

	NEXUS_Memory_Allocate(info->media_object_size, NULL, &buffer);	
	if(buffer == NULL)	
	{		
		BDBG_ERR(("Out of memory!\n")); 	
		return; 
	}	

/*************************
*	Hook data from cursor
*************************/	
	batom_cursor_save(temp_cursor, &save);	
	batom_cursor_copy(temp_cursor, buffer, info->media_object_size);

/*************************
*	Decrypt
*************************/	
	b_asf_stream_DoDecrypt_hw(stream, buffer, info->media_object_size, (unsigned char*)repl_data, BASF_ENCRYPTION_ALGORITHM_AESCTR);
	
/*************************
*	Copy back to cursor
*************************/	
	batom_cursor_rollback(temp_cursor, &save);		
	if(temp_cursor->left >= info->media_object_size)	
	{		
		BKNI_Memcpy((uint8_t*)temp_cursor->cursor, buffer, info->media_object_size);	
	}	
	else	
	{
		BDBG_MSG(("straddle!\n"));		
		for (off=0; pos < info->media_object_size; batom_cursor_skip(temp_cursor, temp_cursor->left+off)) 
		{			
			if(BATOM_IS_EOF(temp_cursor)) 
			{				 
				BDBG_ASSERT(0); 
				break;			  
			}						
			if (pos+temp_cursor->left+off <= info->media_object_size) 
			{				 
				copy = temp_cursor->left+off;							
			} 
			else 
			{				 
				copy = info->media_object_size-pos; 		   
			}			 
			BKNI_Memcpy((uint8_t*)temp_cursor->cursor-off, buffer, copy);			 
			buffer += copy; 		   
			pos += copy;			
			off = 1;						
		}		
		buffer-=info->media_object_size;	
	}	
	batom_cursor_rollback(temp_cursor, &save);		
	if(buffer != NULL)	
	{		
		NEXUS_Memory_Free(buffer);		
		buffer = NULL;	
	}	
}

#endif
#endif

static void
b_asf_stream_accum(basf_stream_t stream, batom_accum_t source, const batom_cursor *packet, batom_cursor *replicated_data, const struct basf_payload_info *info)
{
	size_t object_bytes;
	size_t offset_into_media_object;
	batom_accum_t accum;
	batom_cursor temp_cursor;

	BDBG_MSG_TRACE(("b_asf_stream_accum>: %#lx:%#lx %u", (unsigned long)stream->demux, (unsigned long)stream, info->payload_length));


	BDBG_ASSERT(stream->stream_info);
	accum = stream->accum;
    if(stream->object.reordering) {
        if(stream->object.media_object_size==info->media_object_size && stream->object.pres_time == stream->object.pres_time) {
            goto reordering;
        } else {
            b_asf_stream_object_reordering_queue_flush(&stream->payload_queue);
        }
    }

	object_bytes = stream->object.bytes;
	offset_into_media_object = info->offset_into_media_object;
	if(offset_into_media_object==0) {
		if(object_bytes!=0) {
            if(stream->object.media_object_size==info->media_object_size && stream->object.pres_time == stream->object.pres_time) {
                stream->object.reordering = true;
                b_asf_stream_object_reordering_queue_seed(stream, &stream->payload_queue, accum, info);
                goto reordering;
            } else {
                object_bytes = 0;
                batom_accum_clear(accum);
            }
		}
        /* start of the object */
        stream->object.media_object_size = info->media_object_size;
        stream->object.pres_time = info->pres_time;
        stream->object.reordering = false;
		if(stream->stream_info->stream_type==b_asf_stream_type_wma) {
			/* add WMA header */
			batom_accum_add_atom(accum, stream->meta);
		}
		if(stream->demux->key_frame_only) {
			/* we only let single key frame to pass between flushing */
			if(stream->nkey_frames > 1 || !info->key_frame) {
				goto done;
			}
			stream->nkey_frames++;
		} 	
	}

	if(offset_into_media_object==object_bytes) {
		size_t len;

		BATOM_CLONE(&temp_cursor, packet);
		len = batom_cursor_skip(&temp_cursor, info->payload_length);
		if(len==info->payload_length) {
			batom_accum_append(accum, source, packet, &temp_cursor);
#if B_ASF_CAPTURE_DATA 
            {
                static FILE *fp=NULL;
                b_asf_p_capture_data(&fp, stream, "asf_payload.dat", packet, info->payload_length);
            }
#endif
#if defined (HUMAX_PLATFORM_BASE)
			BATOM_CLONE(&temp_cursor, packet);
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
			if(stream->demux->bWmDrmEncryptedContents == TRUE)
			{	
				b_asf_stream_decrypt_rc4(stream, info, &temp_cursor);
			}
#endif
			if(stream->cfg.reorder_timestamps && offset_into_media_object == 0 && stream->cfg.media_object==NULL) { /* try to detect picture type using only first payload, and push queued frames */
				bmedia_video_pic_type  pic_type;
				switch(stream->stream_info->stream_type) {
				case b_asf_stream_type_vc1_ap:
				case b_asf_stream_type_vc1_sm:
				  batom_cursor_from_accum(&temp_cursor, accum);
				  pic_type = b_asf_get_pic_type(stream, info, &temp_cursor);
				  if(pic_type == bmedia_video_pic_type_I || pic_type == bmedia_video_pic_type_P) { /* if next frame is I or P, then flush a waiting queue */
					  BDBG_MSG_TRACE(("b_asf_stream_accum: %#lx flushed frame %#lx (%u bytes)", (unsigned long)stream, (unsigned long)batom_pipe_peek(stream->queue.pipe), batom_pipe_peek(stream->queue.pipe)?batom_len(batom_pipe_peek(stream->queue.pipe)):0));
					  b_asf_stream_queue_flush(stream);
				  }
				  break;
				default:
				  break;
				}
			}
			object_bytes += len;

			if(object_bytes == info->media_object_size) 
			{
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
				if(stream->demux->bPlayReadyEncryptedContents == TRUE)
				{
					b_asf_stream_decrypt_aesctr(stream, info, &temp_cursor, replicated_data);
				}
#endif
				stream->stream_info->stream_handler(stream, info);
				object_bytes = 0;
			}
#else
			if(stream->cfg.payload) {
				BATOM_CLONE(&temp_cursor, packet);
				stream->cfg.payload(stream->cfg.stream_cnxt, source, &temp_cursor, replicated_data, info);
			}
            if(stream->cfg.reorder_timestamps && offset_into_media_object == 0 && stream->cfg.media_object==NULL) { /* try to detect picture type using only first payload, and push queued frames */
				bmedia_video_pic_type  pic_type;
				switch(stream->stream_info->stream_type) {
				case b_asf_stream_type_vc1_ap:
				case b_asf_stream_type_vc1_sm:
				  batom_cursor_from_accum(&temp_cursor, accum);
				  pic_type = b_asf_get_pic_type(stream, info, &temp_cursor);
				  if(pic_type == bmedia_video_pic_type_I || pic_type == bmedia_video_pic_type_P) { /* if next frame is I or P, then flush a waiting queue */
					  BDBG_MSG_TRACE(("b_asf_stream_accum: %#lx flushed frame %#lx (%u bytes)", (unsigned long)stream, (unsigned long)batom_pipe_peek(stream->queue.pipe), batom_pipe_peek(stream->queue.pipe)?batom_len(batom_pipe_peek(stream->queue.pipe)):0));
					  b_asf_stream_queue_flush(stream);
				  }
				  break;
				default:
				  break;
				}
			}
			object_bytes += len;
			BDBG_MSG_TRACE(("b_asf_stream_accum+: %#lx %u %u:%u:%u", (unsigned long)stream, object_bytes, info->payload_length, info->media_object_size));
			if(object_bytes == info->media_object_size) {
                b_asf_stream_accum_process_object(stream, info, &temp_cursor);
				object_bytes = 0;
			}
#endif
		} else {
			BDBG_WRN(("b_asf_stream_accum: %#lx corrupted packet %u:%u", (unsigned long)stream, (unsigned)len, (unsigned)info->payload_length));
		}
	} else {
        if(object_bytes == 0 || (stream->object.media_object_size==info->media_object_size && stream->object.pres_time == stream->object.pres_time)) {
            stream->object.reordering = true;
            stream->object.media_object_size = info->media_object_size;
            stream->object.pres_time = info->pres_time;
            b_asf_stream_object_reordering_queue_seed(stream, &stream->payload_queue, accum, info);
            goto reordering;
        } 
		BDBG_MSG(("b_asf_stream_accum: %#lx out of sync packet %u:%u:%u", (unsigned long)stream, (unsigned)offset_into_media_object, (unsigned)object_bytes, (unsigned)info->payload_length));

	}
 done:
	stream->object.bytes = object_bytes;
	BDBG_MSG_TRACE(("b_asf_stream_accum<: %#lx %u", (unsigned long)stream, stream->object.bytes));
    return;
reordering:
    {
        bool reordering_complete;
        reordering_complete = b_asf_stream_object_reordering_queue_enqueue(stream, &stream->payload_queue, source, packet, replicated_data, info, accum);
        if(reordering_complete) {
            b_asf_stream_accum_process_object(stream, info, &temp_cursor);
            stream->object.reordering = false;
	        object_bytes = 0;
            goto done;
        }
    }
	return;
}

static void
b_asf_demux_payload(void *sink_cnxt, batom_accum_t source, const batom_cursor *payload, batom_cursor *replicated_data, const struct basf_payload_info *info)
{
	basf_demux_t asf=sink_cnxt;
	basf_stream_t stream;

	BDBG_MSG_TRACE(("b_asf_stream_payload>: %#lx %u %u", (unsigned long)asf, info->stream_number, info->payload_length));
	BDBG_OBJECT_ASSERT(asf, basf_demux_t);
	for(stream = BLST_S_FIRST(&asf->streams); stream; stream = BLST_S_NEXT(stream, link)) {
		if (stream->stream_number == info->stream_number) {
			if (stream->stream_info) {
				b_asf_stream_accum(stream, source, payload, replicated_data, info);
				BDBG_MSG_TRACE(("b_asf_stream_payload<: %#lx %u %u match", (unsigned long)asf, info->stream_number, info->payload_length));
				return ;
			} 
			break;
		}
	}
	BDBG_MSG_TRACE(("b_asf_stream_payload<: %#lx %u %u skip", (unsigned long)asf, info->stream_number, info->payload_length));
	return ;
}


static bool
b_asf_stream_parse_vc1_ap_seq(struct basf_video_stream_info *video, batom_cursor *cursor)
{
	return bmedia_vc1ap_read_info(&video->seq_hdr.vc1_ap, cursor);
}

static bool
b_asf_stream_parse_vc1_sm_seq(struct basf_video_stream_info *video, batom_cursor *cursor)
{
	return bmedia_vc1sm_read_info(&video->seq_hdr.vc1_sm, cursor);
}

static struct b_asf_stream_info *
b_asf_parse_aux_stream_properties(basf_demux_t demux, batom_cursor *cursor, struct b_asf_stream_info *stream)
{
	struct basf_aux_stream_info *aux;

	BSTD_UNUSED(demux);
	BSTD_UNUSED(cursor);

	aux = BKNI_Malloc(sizeof(*aux));
	if (!aux) {
		BDBG_ERR(("b_asf_parse_aux_stream_properties: %#lx out of memory %u", (unsigned long)demux, sizeof(*aux)));
		return NULL;
	}

	stream->stream_type = b_asf_stream_type_aux;
	stream->stream_handler = b_asf_stream_feed_auxiliary;	
	
	aux->stream = *stream;
	
	return &aux->stream;
}

static struct b_asf_stream_info *
b_asf_parse_video_stream_properties(basf_demux_t demux, batom_cursor *cursor, struct b_asf_stream_info *stream)
{
	basf_video_stream_properties asf_video;
	unsigned seq_header_len = 0;
	batom_checkpoint chk;
	struct basf_video_stream_info *video;
	basf_parser_t asf=demux->parser;
	batom_cursor seq_hdr;
    bool pr;

	BDBG_MSG(("b_asf_parse_video_stream_properties: %#lx stream:%u", (unsigned long)demux, stream->stream_number));

    if(!basf_read_video_stream_properties(cursor, &asf_video, &pr)) {
        BDBG_WRN(("b_asf_parse_video_stream_properties: %#lx header error", (unsigned long)demux));
        return NULL;	
    }
	video = BKNI_Malloc(sizeof(*video));
	if (!video) {
		BDBG_ERR(("b_asf_parse_video_stream_properties: %#lx out of memory %u", (unsigned long)demux, sizeof(*video)));
		return NULL;
	}

	if(BMEDIA_FOURCC_VC1AP_CODEC(asf_video.bi.biCompression)) {
		BASF_ASSERT(asf_video.bi.biSize > BASF_VIDEO_HEADER_LENGTH);
		seq_header_len = asf_video.bi.biSize - BASF_VIDEO_HEADER_LENGTH;

		stream->stream_type = b_asf_stream_type_vc1_ap;
		stream->stream_handler = b_asf_stream_feed_video_vc1;
		stream->vecs[B_ASF_STREAM_VEC_FRAME] = &bmedia_frame_vc1;
		stream->vecs[B_ASF_STREAM_VEC_EOS] = &bmedia_eos_vc1;
		BATOM_SAVE(cursor, &chk);
		if(basf_read_byte(cursor)!=0) {
			seq_header_len--;
			BDBG_MSG(("WVC1/WVCA detected with nonzero first byte, ignoring first byte"));
		} else {
			batom_cursor_rollback(cursor, &chk);
		}
		BASF_ASSERT(seq_header_len<=sizeof(video->seq_header));
		BATOM_CLONE(&seq_hdr, cursor);
		b_asf_stream_parse_vc1_ap_seq(video, &seq_hdr);
		batom_cursor_copy(cursor, video->seq_header, seq_header_len);
	} else if (BMEDIA_FOURCC_VC1SM_CODEC(asf_video.bi.biCompression)) {
		int profile_number;
		size_t header_len;

		BASF_ASSERT(asf_video.bi.biSize > BASF_VIDEO_HEADER_LENGTH);
        seq_header_len = asf_video.bi.biSize - BASF_VIDEO_HEADER_LENGTH;

		BATOM_CLONE(&seq_hdr, cursor);
		if (seq_header_len==5) {
			BDBG_MSG(("WMV3 detected with 5 bytes of data, ignoring last byte"));
			seq_header_len--;
		}
		BASF_ASSERT(seq_header_len==4);
		stream->stream_type = b_asf_stream_type_vc1_sm;
		stream->stream_handler = b_asf_stream_feed_video_vc1;
		stream->vecs[B_ASF_STREAM_VEC_FRAME] = &bmedia_frame_vc1;
		stream->vecs[B_ASF_STREAM_VEC_EOS] = &bmedia_eos_vc1;
		header_len = 0;
		video->seq_header[header_len+0] = 0x00;
		video->seq_header[header_len+1] = 0x00;
		video->seq_header[header_len+2] = 0x01;
		header_len +=3;
		video->seq_header[header_len+0] = 0x0F;
		video->seq_header[header_len+1]  = (asf_video.bi.biWidth >> 8)&0xFF;
		video->seq_header[header_len+2]  = (asf_video.bi.biWidth)&0xFF;
	
		video->seq_header[header_len+3]  = (asf_video.bi.biHeight >> 8)&0xFF;
		video->seq_header[header_len+4]  = (asf_video.bi.biHeight)&0xFF;
		header_len += 5;
		
		b_asf_stream_parse_vc1_sm_seq(video, &seq_hdr);

		BASF_ASSERT(seq_header_len+header_len+7<=sizeof(video->seq_header));
		batom_cursor_copy(cursor, &video->seq_header[header_len], seq_header_len);
		seq_header_len += header_len;
		/* extract profile number from header */
		profile_number = video->seq_header[header_len] >> 4;
		if ((profile_number != 0) && (profile_number != 4)) {
			 /* printout error message */
		     BDBG_WRN(("b_asf_parse_video_stream_properties: %#lx unsupported video compression fmt (profile %d)", (unsigned long)demux, profile_number));
		}
		BKNI_Memset(&video->seq_header[seq_header_len], 0, 7);
		seq_header_len += 7;
	} else if(BMEDIA_FOURCC_DIVX3_CODEC(asf_video.bi.biCompression)) {
		uint8_t *hdr;

		stream->stream_type = b_asf_stream_type_mp4v3;
		stream->stream_handler = b_asf_stream_feed_video_mp4v3;
		stream->vecs[B_ASF_STREAM_VEC_FRAME] = &bmedia_frame_mpeg4;
		stream->vecs[B_ASF_STREAM_VEC_EOS] = &bmedia_eos_mpeg4;
		BKNI_Memcpy(video->seq_header, bmedia_divx311_seq_1.base, bmedia_divx311_seq_1.len);
		hdr = &video->seq_header[bmedia_divx311_seq_1.len];
		hdr[0] = B_GET_BITS(asf_video.bi.biWidth,11,4);
		hdr[1] = 
		  B_SET_BITS("width [3..0]", B_GET_BITS(asf_video.bi.biWidth ,3,0), 7, 4) |
		  B_SET_BITS("'10'", 0x02, 3, 2) |
		  B_SET_BITS("height [11..10]", B_GET_BITS(asf_video.bi.biHeight,11,10), 1, 0);
		hdr[2] = B_GET_BITS(asf_video.bi.biHeight,9,2);
		hdr[3]= 
		  B_SET_BITS("height [1.0]", B_GET_BITS(asf_video.bi.biHeight,1,0), 7, 6) |
		  B_SET_BITS("'100000'", 0x20, 5, 0);

		BKNI_Memcpy(video->seq_header+bmedia_divx311_seq_1.len+4, bmedia_divx311_seq_2.base, bmedia_divx311_seq_2.len);
		seq_header_len = bmedia_divx311_seq_1.len+4+bmedia_divx311_seq_2.len;
	} else if(BMEDIA_FOURCC_DIVX5_CODEC(asf_video.bi.biCompression) || BMEDIA_FOURCC_3IVX_CODEC(asf_video.bi.biCompression) || BMEDIA_FOURCC_XVID_CODEC(asf_video.bi.biCompression)) {
		stream->stream_type = b_asf_stream_type_mp4p2;
		stream->stream_handler = b_asf_stream_feed_video_generic;
		stream->vecs[B_ASF_STREAM_VEC_FRAME] = BMEDIA_FOURCC_DIVX4_CODEC(asf_video.bi.biCompression) ? &bmedia_divx4_seq : &bmedia_null_vec;
		stream->vecs[B_ASF_STREAM_VEC_EOS] = &bmedia_eos_mpeg4;
		if (asf_video.bi.biSize > BASF_VIDEO_HEADER_LENGTH) {
			seq_header_len = asf_video.bi.biSize - BASF_VIDEO_HEADER_LENGTH;
			BASF_ASSERT(seq_header_len<=sizeof(video->seq_header));
			batom_cursor_copy(cursor, video->seq_header, seq_header_len);
		}
	} else if(BMEDIA_FOURCC_MPEG2_CODEC(asf_video.bi.biCompression)) {
		stream->stream_type = b_asf_stream_type_mpeg2;
		stream->stream_handler = b_asf_stream_feed_video_generic;
	} else if(BMEDIA_FOURCC_H264_CODEC(asf_video.bi.biCompression)) {
		stream->stream_type = b_asf_stream_type_h264;
		stream->stream_handler = b_asf_stream_feed_video_generic;
		stream->vecs[B_ASF_STREAM_VEC_EOS] = &bmedia_eos_h264;
		if (asf_video.bi.biSize > BASF_VIDEO_HEADER_LENGTH) {
			unsigned meta_len = asf_video.bi.biSize - BASF_VIDEO_HEADER_LENGTH;
			seq_header_len = meta_len + bmedia_rai_h264.len;
			BASF_ASSERT(seq_header_len<=sizeof(video->seq_header));
			batom_cursor_copy(cursor, video->seq_header, meta_len);
			BKNI_Memcpy(&video->seq_header[meta_len], bmedia_rai_h264.base, bmedia_rai_h264.len);
		}
	} else {
		BDBG_WRN(("%#lx unkwnown video compression: %c%c%c%c(%#x)", (unsigned long)asf, asf_video.bi.biCompression&0xFF, (asf_video.bi.biCompression>>8)&0xFF, (asf_video.bi.biCompression>>16)&0xFF, (asf_video.bi.biCompression>>24)&0xFF, asf_video.bi.biCompression));
		stream->stream_type = b_asf_stream_type_aux;
		stream->stream_handler = b_asf_stream_feed_auxiliary;		
	}
	video->stream = *stream;
	video->stream.meta = video->seq_header;
	video->stream.meta_length = seq_header_len;
	return &video->stream;
asf_assert_handler:
	BKNI_Free(video);
	return NULL;
}

static struct b_asf_stream_info *
b_asf_parse_audio_stream_properties(basf_demux_t demux, batom_cursor *cursor, struct b_asf_stream_info *stream)
{
	basf_audio_stream_properties asf_audio;
	unsigned seq_header_len;
	basf_parser_t asf=demux->parser;
	struct basf_audio_stream_info *audio;
    uint16_t force_packet_size = 0;
    bool pr;

    if(!basf_read_audio_stream_properties(cursor, &asf_audio, &pr)) {
        BDBG_WRN(("b_asf_parse_audio_stream_properties: %#lx header error", (unsigned long)demux));
        return NULL;	
    }

	seq_header_len = 
		B_ASF_FIELD_LEN(wFormatTag, word) +
		B_ASF_FIELD_LEN(nChannels, word) +
		B_ASF_FIELD_LEN(nSamplesPerSec, dword) +
		B_ASF_FIELD_LEN(nAvgBytesPerSec, dword) +
		B_ASF_FIELD_LEN(nBlockAlign, word) +
		B_ASF_FIELD_LEN(wBitsPerSample, word) +
		B_ASF_FIELD_LEN(cbSize, word) +
		asf_audio.cbSize;
	BDBG_MSG(("b_asf_parse_audio_stream_properties:%#lx stream:%u", (unsigned long)demux, stream->stream_number));
	if (BMEDIA_WAVFMTEX_AUDIO_WMA(&asf_audio) || BMEDIA_WAVFMTEX_AUDIO_WMA_PRO(&asf_audio)) {
        stream->stream_type = b_asf_stream_type_wma;
        stream->stream_handler = b_asf_stream_feed_audio_wma;
        stream->vecs[B_ASF_STREAM_VEC_FRAME] = &bmedia_frame_bcma;
	} else if (BMEDIA_WAVFMTEX_AUDIO_PCM(&asf_audio)) {
        stream->stream_type = b_asf_stream_type_pcm;
        stream->stream_handler = b_asf_stream_feed_audio_pcm;
        stream->vecs[B_ASF_STREAM_VEC_FRAME] = &bmedia_frame_bcma;
        force_packet_size = (asf_audio.nChannels * asf_audio.wBitsPerSample * asf_audio.nSamplesPerSec * 30) / (1000*8); /* calculate packet size, so it contains 30msec worth of PCM samples */
	} else if (BMEDIA_WAVFMTEX_AUDIO_AAC(&asf_audio)) {
        bmedia_info_aac aac;
        /* set AAC information based on common data */
        aac.profile = 1;
        /* coverity[check_return] */
        bmedia_info_aac_set_sampling_frequency_index(&aac, asf_audio.nSamplesPerSec);
        bmedia_info_aac_set_channel_configuration(&aac, asf_audio.nChannels);

        if(asf_audio.cbSize>=2) {
            /*  read AAC information from the waveheader */
	        unsigned word;
            unsigned profile;
            const uint8_t *data= asf_audio.meta + (seq_header_len - asf_audio.cbSize);
            BDBG_ASSERT(seq_header_len > asf_audio.cbSize);
	        word = (((unsigned)data[0])<<8) | data[1];
            profile = B_GET_BITS(word, 15, 11);
            if(profile>0) {
                aac.profile = profile-1;
                aac.sampling_frequency_index = B_GET_BITS(word, 10, 7);
                aac.channel_configuration = B_GET_BITS(word, 6, 3);
            }
        } 
	    bmedia_adts_header_init_aac(&stream->codec.aac.adts_header, &aac);
        stream->stream_type = b_asf_stream_type_aac;
        stream->stream_handler = b_asf_stream_feed_audio_aac;
        stream->vecs[B_ASF_STREAM_VEC_FRAME] = &bmedia_frame_bcma;
        force_packet_size = (asf_audio.nChannels * asf_audio.wBitsPerSample * asf_audio.nSamplesPerSec * 30) / (1000*8); /* calculate packet size, so it contains 30msec worth of PCM samples */
	} else {
        stream->stream_type = b_asf_stream_type_generic_audio;
        stream->stream_handler = b_asf_stream_feed_audio_generic;
	}
	audio = BKNI_Malloc(sizeof(*audio));
	
	if (!audio) {
		BDBG_ERR(("b_asf_parse_audio_stream_properties: %#lx out of memory %u", (unsigned long)demux, sizeof(*audio)));
		return NULL;
	}
	BASF_ASSERT(seq_header_len<sizeof(audio->seq_header));
    BKNI_Memcpy(audio->seq_header, asf_audio.meta, asf_audio.meta_length);
	audio->stream = *stream;	
	audio->stream.meta = audio->seq_header;
	audio->stream.meta_length = seq_header_len;
	audio->block_size = asf_audio.nBlockAlign;
    audio->force_packet_size = force_packet_size;
	return &audio->stream;

asf_assert_handler:
	if (audio) {
		BKNI_Free(audio); 
    }
	return NULL;
}

static void
b_asf_demux_meta_atom_free(batom_t atom, void *user)
{
	basf_demux_t demux;
	balloc_iface_t alloc;
	const batom_vec *vec;
	unsigned i;	

	BDBG_MSG_TRACE(("b_asf_demux_meta_atom_free: %#lx %#lx", (unsigned long)atom, (unsigned long)user));
	demux = *(basf_demux_t *)user;
	BDBG_OBJECT_ASSERT(demux, basf_demux_t);
	alloc = demux->alloc;
	for(i=0;NULL!= (vec = batom_get_vec(atom, i));i++) {
		alloc->bmem_free(alloc, vec->base);
	}
	return;
}

static const batom_user b_asf_demux_meta_atom = {
	b_asf_demux_meta_atom_free,
	sizeof(basf_demux_t)
};

/* this function finds and link together stream_info and stream objects */
static void
b_asf_link_streams(basf_demux_t asf, unsigned stream_number)
{
	struct b_asf_stream_info *info=NULL;
	basf_stream_t stream;

	BDBG_MSG_TRACE(("b_asf_link_streams>: %#lx %u", (unsigned long)asf,  stream_number));
	for(stream=BLST_S_FIRST(&asf->streams);stream;stream=BLST_S_NEXT(stream, link)) {
		if (stream->stream_number==stream_number) {
			if (stream->stream_info!=NULL) {
				BDBG_MSG(("b_asf_link_streams:%#lx %u  stream %#lx already linked with %#lx", (unsigned long)asf, stream_number, (unsigned long)stream, (unsigned long)stream->stream_info));
				basf_stream_unlink(asf, stream);
			}
			for(info=BLST_S_FIRST(&asf->stream_info);info;info=BLST_S_NEXT(info, link)) {
				void *meta = NULL;
				if (info->stream_number == stream_number) {
					size_t alloc_length;
					size_t meta_off;
					unsigned i;
					BDBG_ASSERT(info->meta);
					BDBG_CASSERT(sizeof(stream->external_vecs)/sizeof(stream->external_vecs[0]) == sizeof(info->vecs)/sizeof(info->vecs[0]));
					for (i=0; i < sizeof(stream->external_vecs)/sizeof(stream->external_vecs[0]); i++) {
						stream->external_vecs[i] = info->vecs[i];
						BDBG_ASSERT(stream->external_vecs[i]);
					}

					batom_accum_clear(stream->accum);
					for(meta_off=0;meta_off<info->meta_length;meta_off+=alloc_length) {
						alloc_length = info->meta_length - meta_off;
						if(alloc_length>B_ASF_STREAM_META_ATOM) {
							alloc_length = B_ASF_STREAM_META_ATOM;
						}
						meta = asf->alloc->bmem_alloc(asf->alloc, alloc_length);
						if(meta) {
							BKNI_Memcpy(meta, (const uint8_t *)info->meta+meta_off, alloc_length);
							batom_accum_add_range(stream->accum, meta, alloc_length);
						} else {
							BDBG_WRN(("b_asf_link_streams: %#lx can't allocate memory for meta data", (unsigned long)asf));
							stream->meta = batom_from_accum(stream->accum, &b_asf_demux_meta_atom, &asf);
							/* we create atom in order to have batom_release to call b_asf_demux_meta_atom_free andd free memory from the heap */
							if(stream->meta) {
								batom_release(stream->meta);
								stream->meta = NULL;
							}
							goto error;
						}
					}
					stream->meta = batom_from_accum(stream->accum, &b_asf_demux_meta_atom, &asf);
					if(!stream->meta) {
					error:
						BDBG_WRN(("b_asf_link_streams: %#lx can't create atom for meta data", (unsigned long)asf));
						break;
					}
                    /* link info and stream */
					stream->stream_info=info;
                    info->stream = stream;
					BDBG_MSG(("b_asf_link_streams:%#lx %u:%u stream %#lx linked with %#lx", (unsigned long)asf, stream_number, info->stream_type, (unsigned long)stream, (unsigned long)info));
					break;
				}
			}
			break;
		}
	}
	BDBG_MSG_TRACE(("b_asf_link_streams>: %#lx %u %#lx %#lx", (unsigned long)asf,  stream_number, (unsigned long)stream, (unsigned long)info));
	
	return;
}

static void
b_asf_parse_stream_properties(basf_demux_t demux, batom_cursor *cursor)
{
	basf_stream_properties asf_stream;
	struct b_asf_stream_info stream;
	struct b_asf_stream_info *parsed_stream;
	basf_parser_info info;

	if(!basf_read_stream_properties(cursor, &asf_stream)) {
		BDBG_WRN(("%s: %#lx error in the stream header", "b_asf_stream_properties", (unsigned long)demux));
		goto done;
	}

	BKNI_Memset(&stream, 0, sizeof(stream));
	stream.time_offset = asf_stream.timeOffset;
	stream.stream_number = asf_stream.streamNumber;
	stream.encrypted_content_flag = asf_stream.encryptedContentFlag;
	stream.meta = NULL;
	stream.meta_length = 0;
	stream.vecs[B_ASF_STREAM_VEC_FRAME] = &bmedia_null_vec;
	stream.vecs[B_ASF_STREAM_VEC_EOS] = &bmedia_null_vec;

	if(BASF_STREAM_VIDEO(&asf_stream)) {
		parsed_stream = b_asf_parse_video_stream_properties(demux, cursor, &stream);
	} else if (BASF_STREAM_AUDIO(&asf_stream)) {
		parsed_stream = b_asf_parse_audio_stream_properties(demux, cursor, &stream);
        } else if (BASF_STREAM_AUX(&asf_stream)) {
		parsed_stream = b_asf_parse_aux_stream_properties(demux, cursor, &stream);		
	} else {
		BDBG_WRN(("%s: %#lx unknown stream guid '%s'", "b_asf_parse_stream_properties", (unsigned long)demux, b_asf_parser_guid2str(demux->parser, &asf_stream.streamType)));
		goto done;
	}

	if(parsed_stream && parsed_stream->stream_handler) {
		struct b_asf_stream_info *i;
		struct b_asf_stream_info *prev;
	
		for( prev=NULL,i=BLST_S_FIRST(&demux->stream_info); i; i=BLST_S_NEXT(i, link) ) {
			if (i->stream_number==parsed_stream->stream_number) {
				BDBG_MSG(("%s:%#lx detected duplicate stream id %u", "b_asf_parse_stream_properties", (unsigned long)demux, i->stream_number));
                if(i->stream_type == parsed_stream->stream_type && i->meta_length == parsed_stream->meta_length && (i->meta_length == 0 || BKNI_Memcmp(i->meta, parsed_stream->meta, i->meta_length)==0)) {
                    BKNI_Free(parsed_stream);
                    parsed_stream = NULL;
                    break; /* they are trully duplicateds */
                }
                if(i->stream) {
                    basf_stream_unlink(demux, i->stream);
                }
                if(prev==NULL) {
                    BLST_S_REMOVE_HEAD(&demux->stream_info, link);
                } else {
                    BLST_S_REMOVE_NEXT(&demux->stream_info, prev, link);
                }
                BKNI_Free(i);
                break;
			}
            prev = i;
		}
        if(parsed_stream) {
            BLST_S_INSERT_HEAD(&demux->stream_info, parsed_stream, link);
            b_asf_link_streams(demux, parsed_stream->stream_number);
        }
	}
	BDBG_ASSERT(!BATOM_IS_EOF(cursor));

	basf_parser_get_info(demux->parser, &info);
	demux->preroll = info.file.preroll;

done:
	/* coverity[leaked_storage: FALSE] */
	return ;
}

static basf_parser_handler_action
b_asf_stream_properties_object(basf_parser_object_handler *handler,  batom_t header)
{
	basf_demux_t demux;
	batom_cursor cursor;


	demux = ((struct b_asf_demux_object_handler *)handler)->demux;
	BDBG_OBJECT_ASSERT(demux, basf_demux_t);

	batom_cursor_from_atom(&cursor, header);
    b_asf_parse_stream_properties(demux, &cursor);

	return basf_parser_handler_action_none;
}

static basf_parser_handler_action
b_asf_extended_stream_properties_object(basf_parser_object_handler *handler,  batom_t header)
{
	basf_demux_t demux;
	batom_cursor cursor;
    basf_extended_stream_properties stream;

	demux = ((struct b_asf_demux_object_handler *)handler)->demux;
	BDBG_OBJECT_ASSERT(demux, basf_demux_t);

	batom_cursor_from_atom(&cursor, header);

    if(!basf_read_extended_stream_properties(&cursor, &stream)) {
		BDBG_WRN(("b_asf_extended_stream_properties_object: %#lx error in the stream header", (unsigned long)demux));
        goto done;
    }
    if(stream.hasStreamPropertiesObject) {
        b_asf_parse_stream_properties(demux, &cursor);
    }

 
done:
	return basf_parser_handler_action_none;
}


void
basf_stream_initialize(basf_stream_cfg *cfg)
{
	BKNI_Memset(cfg, 0, sizeof(*cfg));
/*    cfg->reorder_timestamps = true; */
    cfg->reorder_timestamps = false;
	return;
}

 
basf_stream_t
basf_stream_attach(basf_demux_t asf, batom_pipe_t pipe, unsigned stream_number, const basf_stream_cfg *cfg)
{
	basf_stream_t stream;

	BDBG_ASSERT(cfg);
	BDBG_ASSERT(pipe);
	BDBG_MSG_TRACE(("basf_stream_attach>: %#lx %u", (unsigned long)asf, stream_number)); 
	BDBG_OBJECT_ASSERT(asf, basf_demux_t);
	stream = BKNI_Malloc(sizeof(*stream));
	if (!stream) {
		BDBG_ERR(("basf_stream_attach: out of memory %u", sizeof(*stream)));
		return NULL;
	}
	BDBG_OBJECT_INIT(stream, basf_stream_t);
	stream->stream_number = stream_number;
	stream->cfg = *cfg;
	stream->demux = asf;
	stream->stream_info = NULL;
	stream->meta = NULL;
	stream->object.bytes = 0;
    stream->object.pres_time = 0;
    stream->object.media_object_size = 0;
    stream->object.reordering = false;
	stream->nkey_frames = 0;
	stream->pipe = pipe;
	stream->queue.last_pts = 0;
	stream->queue.key_frame = false;
	stream->queue.nframes = 0;
	stream->external_vecs[B_ASF_STREAM_VEC_FRAME] = &bmedia_null_vec;
	stream->external_vecs[B_ASF_STREAM_VEC_EOS] = &bmedia_null_vec;
    b_asf_stream_object_reordering_queue_init(&stream->payload_queue);

	stream->accum = batom_accum_create(asf->factory);
	if(!stream->accum) {
		goto err_accum;
	}
	stream->queue.pipe = batom_pipe_create(asf->factory);
	if(!stream->queue.pipe) {
		goto err_queue_pipe;
	}
	/* XXX test for duplicate stream_number */
	BLST_S_INSERT_HEAD(&asf->streams, stream, link);
	b_asf_link_streams(asf, stream->stream_number);
	BDBG_MSG_TRACE(("basf_stream_attach<: %#lx %u", (unsigned long)asf, stream_number)); 
	return stream;

err_queue_pipe:
	batom_accum_destroy(stream->accum);
err_accum:
	BKNI_Free(stream);
	return NULL;
}


static void
b_asf_stream_flush(basf_stream_t stream)
{
	BDBG_MSG_TRACE(("b_asf_stream_flush>: %#lx", (unsigned long)stream)); 

	if(stream->stream_info) {
		switch(stream->stream_info->stream_type) {
		case b_asf_stream_type_vc1_ap:
		case b_asf_stream_type_vc1_sm:
			b_asf_stream_queue_flush(stream);
			break;
		default:
			break;
		}
	}
	stream->object.bytes = 0;
    stream->object.pres_time = 0;
    stream->object.media_object_size = 0;
    stream->object.reordering = false;
	stream->nkey_frames = 0;
	stream->queue.last_pts = 0;
	stream->queue.key_frame = false;
	stream->queue.nframes = 0;
	batom_accum_clear(stream->accum);
    b_asf_stream_object_reordering_queue_flush(&stream->payload_queue);
	batom_pipe_flush(stream->queue.pipe);

	BDBG_MSG_TRACE(("b_asf_stream_flush<: %lx", (unsigned long)stream)); 
	return;
}

static void
basf_stream_flush(basf_demux_t asf, basf_stream_t stream)
{
	BDBG_MSG_TRACE(("basf_stream_flush>: %#lx %lx", (unsigned long)asf, (unsigned long)stream)); 

	BDBG_OBJECT_ASSERT(asf, basf_demux_t);
	BDBG_OBJECT_ASSERT(stream, basf_stream_t);
	BDBG_ASSERT(stream->demux == asf);

	b_asf_stream_flush(stream);
	batom_pipe_flush(stream->pipe);

	BDBG_MSG_TRACE(("basf_stream_flush<: %#lx %lx", (unsigned long)asf, (unsigned long)stream)); 
	return;
}

static void
basf_stream_unlink(basf_demux_t asf, basf_stream_t stream)
{
	basf_stream_flush(asf, stream);
	if(stream->meta) {
		batom_release(stream->meta);
        stream->meta = NULL;
	}
    if(stream->stream_info) {
        BDBG_ASSERT(stream->stream_info->stream == stream); /* verify integrity */
        stream->stream_info->stream = NULL;
        stream->stream_info = NULL;
    }
    return;
}

void 
basf_stream_detach(basf_demux_t asf, basf_stream_t stream)
{

	BDBG_MSG_TRACE(("basf_stream_detach>: %#lx %lx", (unsigned long)asf, (unsigned long)stream)); 
	BDBG_OBJECT_ASSERT(asf, basf_demux_t);
	BDBG_OBJECT_ASSERT(stream, basf_stream_t);
	BDBG_ASSERT(stream->demux == asf);

    basf_stream_unlink(asf, stream);
	batom_accum_destroy(stream->accum);
	batom_pipe_destroy(stream->queue.pipe);

	BLST_S_REMOVE(&asf->streams, stream, basf_stream, link);
	BDBG_OBJECT_DESTROY(stream, basf_stream_t);
	BKNI_Free(stream);

	BDBG_MSG_TRACE(("basf_stream_detach<: %#lx %lx", (unsigned long)asf, (unsigned long)stream)); 
	return;
}

void 
basf_demux_default_cfg(basf_demux_cfg *cfg)
{
	BDBG_ASSERT(cfg);
	BKNI_Memset(cfg, 0, sizeof(*cfg));
	cfg->stream_hdr_size = B_ASF_STREAM_META_ATOM;
	return;
}

static void 
b_asf_demux_flush_partial(void *sink_cnxt)
{
	basf_demux_t demux=sink_cnxt;
	basf_stream_t stream;

	BDBG_MSG_TRACE(("b_asf_demux_flush_partial>: %#lx", (unsigned long)demux));
	BDBG_OBJECT_ASSERT(demux,basf_demux_t);
	for(stream = BLST_S_FIRST(&demux->streams); stream; stream = BLST_S_NEXT(stream, link)) {
		b_asf_stream_flush(stream);
	}
	BDBG_MSG_TRACE(("b_asf_demux_flush_partial<: %#lx", (unsigned long)demux));
	return;
}


static void 
b_asf_demux_payload_end(void *sink_cnxt)
{
	basf_demux_t demux=sink_cnxt;
	basf_stream_t stream;

	BDBG_MSG_TRACE(("b_asf_demux_payload_end>: %#lx", (unsigned long)demux));
	BDBG_OBJECT_ASSERT(demux,basf_demux_t);
	for(stream = BLST_S_FIRST(&demux->streams); stream; stream = BLST_S_NEXT(stream, link)) {
		batom_t atom;
		size_t payload_length;
	
		if(!stream->stream_info) {
			continue;
		}
		b_asf_stream_flush(stream);
		switch(stream->stream_info->stream_type) {
		case b_asf_stream_type_vc1_ap:
		case b_asf_stream_type_vc1_sm:
		case b_asf_stream_type_mp4v3:
		case b_asf_stream_type_mp4p2:
		case b_asf_stream_type_h264:
			{
				bmedia_packet_header hdr;

				BMEDIA_PACKET_HEADER_INIT(&hdr);
				hdr.header_type = B_ASF_STREAM_VEC_EOS | B_MEDIA_PACKET_FLAG_EOS;
				if (stream->stream_info->stream_type==b_asf_stream_type_vc1_sm) {
					hdr.pts = B_MEDIA_MARKER_PTS;
					hdr.pts_valid = true;
				}
				atom = batom_empty(stream->demux->factory, bmedia_atom, &hdr);
				if(atom) {
					batom_pipe_push(stream->pipe, atom);
				}
			}
			break;
		case b_asf_stream_type_wma:
			{
				bmedia_bcma_hdr audio_hdr;
				payload_length = 10240;
				BMEDIA_PACKET_HEADER_INIT(&audio_hdr.pes);
				audio_hdr.pes.header_type = B_ASF_STREAM_VEC_FRAME | B_MEDIA_PACKET_FLAG_EOS;
				audio_hdr.pes.header_off = 4;
				bmedia_bcma_hdr_init(&audio_hdr, payload_length);

				atom = batom_empty(stream->demux->factory, &bmedia_bcma_atom, &audio_hdr);
				if(atom) {
					batom_pipe_push(stream->pipe, atom);
				}
			}
		default:
			break;
		}
	}
	BDBG_MSG_TRACE(("b_asf_demux_payload_end<: %#lx", (unsigned long)demux));
	return;
}


/* this function is used to flush all pending data without affecting ASF demux state */
void 
basf_demux_flush(basf_demux_t demux)
{
	basf_stream_t stream;

	BDBG_MSG_TRACE(("basf_demux_flush>: %#lx", (unsigned long)demux));
	BDBG_OBJECT_ASSERT(demux,basf_demux_t);
	for(stream = BLST_S_FIRST(&demux->streams); stream; stream = BLST_S_NEXT(stream, link)) {
		basf_stream_flush(demux, stream);
	}
	BDBG_MSG_TRACE(("basf_demux_flush<: %#lx", (unsigned long)demux));
	return;
}


basf_demux_t 
basf_demux_create(basf_parser_t parser, batom_factory_t factory, balloc_iface_t alloc, const basf_demux_cfg *cfg)
{
	basf_demux_t demux;
	basf_parser_payload_sink sink;

	BDBG_MSG_TRACE(("basf_demux_create>: %#lx %#lx", (unsigned long)cfg, (unsigned long)parser));
	BDBG_ASSERT(cfg);
	BDBG_ASSERT(parser);
	BDBG_ASSERT(factory);
	BDBG_ASSERT(alloc);
  
	demux = BKNI_Malloc(sizeof(*demux));
	if (!demux) { 
		BDBG_ERR(("basf_create: out of memory %u", sizeof(*demux)));
		goto err_alloc_demux;
	}
	BDBG_OBJECT_INIT(demux,basf_demux_t);
	demux->cfg = *cfg;
	demux->factory = factory;
	demux->alloc = alloc;
	demux->parser = parser;
	demux->time_scale = BMEDIA_TIME_SCALE_BASE;
	BLST_S_INIT(&demux->stream_info);
	BLST_S_INIT(&demux->streams);
#if defined (HUMAX_PLATFORM_BASE)
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
	demux->bWmDrmEncryptedContents = FALSE;
	demux->bPlayReadyEncryptedContents = FALSE;
	demux->wrmheader = NULL;
	demux->wrmheaderlen = 0;
	demux->cryptoHandle = NULL;
	demux->keyWmdrm = NULL;
	demux->keyPrdy = NULL;
	demux->pEkl = NULL;
#if defined(CONFIG_PLAYREADY_25)
	demux->bSetContext = FALSE;
	demux->pAppContext = NULL;
	demux->f_pOEMContext = NULL;
#endif
#endif
#endif
	basf_demux_reset(demux);
	demux->stream_properties_handler.demux = demux;
	demux->extended_stream_properties_handler.demux = demux;
	basf_parser_install_object_handler(demux->parser, &demux->stream_properties_handler.handler, &basf_stream_properties_guid, b_asf_stream_properties_object);
	basf_parser_install_object_handler(demux->parser, &demux->extended_stream_properties_handler.handler, &basf_extended_stream_properties_guid, b_asf_extended_stream_properties_object); 
#if defined (HUMAX_PLATFORM_BASE)
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
/*************************
*	install handler
*************************/
	demux->extended_contents_encryption_object_handler.demux = demux;
	basf_parser_install_object_handler(demux->parser, &demux->extended_contents_encryption_object_handler.handler, &basf_extended_content_encryption_object_guid, b_asf_stream_wmdrm10_object);
	demux->playready_header_object_handler.demux = demux;
	basf_parser_install_object_handler(demux->parser, &demux->playready_header_object_handler.handler, &basf_protection_system_identifier_object_guid, b_asf_stream_playready_object);
#endif
#endif

	basf_parser_default_payload_sink(&sink);
	sink.sink_cnxt = demux;
	sink.payload = b_asf_demux_payload;
	sink.payload_error = b_asf_demux_flush_partial;
	sink.payload_flush = b_asf_demux_flush_partial;
	sink.payload_end = b_asf_demux_payload_end;
	basf_parser_set_payload_sink(demux->parser, &sink);

	BDBG_MSG_TRACE(("basf_demux_create>:%#lx %#lx %#lx", (unsigned long)demux, (unsigned long)cfg, (unsigned long)parser));
	return demux;

err_alloc_demux:
	BDBG_MSG_TRACE(("basf_demux_create>:%#lx %#lx %#lx", (unsigned long)NULL, (unsigned long)cfg, (unsigned long)parser));
	return NULL;
}



void
basf_demux_reset(basf_demux_t demux)
{
	struct b_asf_stream_info *info;
	basf_stream_t stream;

#if defined (HUMAX_PLATFORM_BASE)
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
#if defined(CONFIG_PLAYREADY_25)
	if(demux->bSetContext)
	{
		Drm_Reader_Close(&(demux->stDecryptContext));
		demux->bSetContext = FALSE;
	}
	if(demux->pAppContext != NULL)
	{
		Drm_Uninitialize(demux->pAppContext);
		Oem_MemFree(demux->pAppContext);
		demux->pAppContext = NULL;
	}
	if(demux->f_pOEMContext != NULL)
	{
		Oem_Destroy(demux->f_pOEMContext);
		demux->f_pOEMContext = NULL;
	}
#endif
	demux->bWmDrmEncryptedContents = FALSE;
	demux->bPlayReadyEncryptedContents = FALSE;

	if(demux->cryptoHandle)
	{
		CommonCrypto_Close(demux->cryptoHandle);
		demux->cryptoHandle = NULL;
	}
	if(demux->keyWmdrm)
	{
		NEXUS_Security_FreeKeySlot(demux->keyWmdrm);
		demux->keyWmdrm = NULL;
	}
	if(demux->keyPrdy)
	{
		NEXUS_Security_FreeKeySlot(demux->keyPrdy);
		demux->keyPrdy = NULL;
	}
	if(demux->pEkl)
	{
		NEXUS_Memory_Free(demux->pEkl);
		demux->pEkl = NULL;
	}
#endif
#endif
	BDBG_MSG_TRACE(("basf_demux_reset>: %#lx", (unsigned long)demux));
	BDBG_OBJECT_ASSERT(demux,basf_demux_t);
	demux->key_frame_only = false;
	demux->preroll = 0;
	while(NULL!=(stream=BLST_S_FIRST(&demux->streams))) {
		BDBG_WRN(("basf_demux_reset: %#lx implicit detach of stream %#lx", (unsigned long)demux, (unsigned long)stream));
		BDBG_ASSERT(stream->demux == demux);
		basf_stream_detach(demux, stream);
	}
	while(NULL!=(info=BLST_S_FIRST(&demux->stream_info))) {
		BLST_S_REMOVE_HEAD(&demux->stream_info, link);
		BKNI_Free(info);
	}

	BDBG_MSG_TRACE(("basf_demux_reset<: %#lx", (unsigned long)demux));

	return;
}

void
basf_demux_destroy(basf_demux_t demux)
{
	BDBG_MSG_TRACE(("basf_demux_destroy>: %#lx", (unsigned long)demux));
	BDBG_OBJECT_ASSERT(demux,basf_demux_t);
	basf_demux_reset(demux);
#if defined (HUMAX_PLATFORM_BASE)
#if defined (CONFIG_BRCM_MOD_MSDRM_PRDY_SUPPORT) && defined(CONFIG_MEDIA20)
	if(demux->wrmheader != NULL)
	{
		BKNI_Free(demux->wrmheader);
		demux->wrmheader = NULL;
		demux->wrmheaderlen = 0;
	}
	basf_parser_remove_object_handler(demux->parser, &demux->extended_contents_encryption_object_handler.handler);
	basf_parser_remove_object_handler(demux->parser, &demux->playready_header_object_handler.handler);
#endif
#endif
	basf_parser_remove_object_handler(demux->parser, &demux->extended_stream_properties_handler.handler);
	basf_parser_remove_object_handler(demux->parser, &demux->stream_properties_handler.handler);
	BDBG_OBJECT_DESTROY(demux, basf_demux_t);
	BKNI_Free(demux);
	BDBG_MSG_TRACE(("basf_demux_destroy<: %#lx", (unsigned long)demux));
	return;
}

void 
basf_demux_status_get(basf_demux_t asf, basf_demux_status *status)
{
	basf_stream_t stream;
	size_t count;
	BDBG_MSG_TRACE(("basf_demux_status_get>:%#lx", (unsigned long)asf));
	BDBG_OBJECT_ASSERT(asf,basf_demux_t);
	BDBG_ASSERT(status);
	for(count=0,stream=BLST_S_FIRST(&asf->streams); stream; stream = BLST_S_NEXT(stream, link)) { 
		BDBG_MSG(("basf_demux_status_get: %#lx stream:%#lx accum:%u pipe:%#lx queue_pipe:%#lx queue_last_pts:%u nframes:%u",  (unsigned long)asf, (unsigned long)stream, batom_accum_len(stream->accum), (unsigned long)batom_pipe_peek(stream->pipe),(unsigned long)batom_pipe_peek(stream->queue.pipe), stream->queue.last_pts, stream->queue.nframes));
		count++;
	}
	status->nstreams = count;
	BDBG_MSG_TRACE(("basf_demux_status_get<:%#lx %u", (unsigned long)asf, status->nstreams));
	return;
}

void 
basf_demux_set_timescale(basf_demux_t demux, bmedia_time_scale time_scale)
{
	BDBG_MSG_TRACE(("basf_demux_set_timescale>: %#lx %d", (unsigned long)demux, (int)time_scale));
	BDBG_OBJECT_ASSERT(demux,basf_demux_t);
	demux->time_scale = time_scale;
	BDBG_MSG_TRACE(("basf_demux_set_timescale<: %#lx %d", (unsigned long)demux, (int)time_scale));
	return;
}

void 
basf_stream_get_stream_cfg(basf_stream_t stream, bmedia_pes_stream_cfg *cfg)
{
	BDBG_OBJECT_ASSERT(stream,basf_stream_t);
	BDBG_ASSERT(cfg);
	bmedia_pes_default_stream_cfg(cfg);
	cfg->nvecs = sizeof(stream->external_vecs)/sizeof(stream->external_vecs[0]);
	cfg->vecs = stream->external_vecs;
	return;
}


void 
basf_demux_set_keyframeonly(basf_demux_t demux, bool key_frame_only)
{
	BDBG_MSG_TRACE(("basf_demux_set_keyframeonly>: %#lx %d", (unsigned long)demux, (int)key_frame_only));
	BDBG_OBJECT_ASSERT(demux,basf_demux_t);
	demux->key_frame_only = key_frame_only;
	BDBG_MSG_TRACE(("basf_demux_set_keyframeonly<: %#lx %d", (unsigned long)demux, (int)key_frame_only));
}


