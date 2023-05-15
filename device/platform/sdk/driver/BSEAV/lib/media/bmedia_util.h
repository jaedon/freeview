/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmedia_util.h $
 * $brcm_Revision: 51 $
 * $brcm_Date: 10/11/12 12:04p $
 *
 * Module Description:
 *
 * BMedia library, misc utilities
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmedia_util.h $
 * 
 * 51   10/11/12 12:04p vsilyaev
 * SW7425-3955: Return proper tracks for BD audio
 * 
 * 50   9/28/12 12:10p vsilyaev
 * SW7435-384: Addes support for EAC3 audio in ASF container
 * 
 * 49   3/5/12 12:32p vsilyaev
 * SW7425-2179: Added support for QT MJPEG types A and B
 * 
 * 48   2/23/12 7:53p vsilyaev
 * SW7425-2179: Added MJPEG support
 * 
 * 47   2/21/12 5:12p vsilyaev
 * SW7425-2378: Support for Monkey Audio (APE) files
 * 
 * 46   1/24/12 4:25p vsilyaev
 * SWBLURAY-28118: Added MP4S codec (appear to be compatible with MPEG-4
 * Part2
 * 
 * 45   1/20/12 1:39p vsilyaev
 * SW7425-2178: Added MJPEG video codec
 * 
 * 44   12/7/11 4:00p vsilyaev
 * SW7425-1917: Added player for RMFF content
 * 
 * 43   9/28/11 9:04a vsilyaev
 * FWAVD-169: Added  handling for EndOfFile condition
 * 
 * 42   8/18/11 5:33p vsilyaev
 * SW7468-69: Added support for AAC codec in RMFF format
 * 
 * 41   8/10/11 12:05p vsilyaev
 * FWAVD-138: Added EOS for VP8 stream
 * 
 * 40   8/9/11 5:06p vsilyaev
 * FWAVD-138: Used dedicated bounded buffer for the PES headers
 * 
 * 39   8/3/11 3:09p vsilyaev
 * SWBLURAY-26343: Added handling for avc1 track type
 * 
 * 38   7/21/11 11:16a vsilyaev
 * FWAVD-138: Better handle bounded PES packets with size larger then max
 * for the pes packet
 * 
 * 37   7/13/11 8:39p vsilyaev
 * SWDTV-7897: Added code to wrap PCM file with the WAV header
 * 
 * 36   7/1/11 5:43p vsilyaev
 * SWDTV-7732: Added vorbis support
 * 
 * 35   5/3/11 2:49p vsilyaev
 * SWBLURAY-22800: Added common code to parse H264 meta data
 * 
 * 34   3/2/11 12:27p vsilyaev
 * SWBLURAY-24399: Merged DTS-CD support
 * 
 * fix_bdvd_v3.0_SWBLURAY-24399/1   2/18/11 1:40p ialauder
 * Add ability to search for DTS-CD sync words.
 * 
 * 33   12/6/10 6:22p vsilyaev
 * SW35230-1953: Added function to return vector array and size
 * 
 * 32   11/23/10 2:34p vsilyaev
 * SW7422-107: Use common definition for 'BCMV' header
 * 
 * 31   10/27/10 2:35p vsilyaev
 * SW35230-1952: Added support for MS ADPCM audio
 * 
 * 30   8/19/10 6:31p vsilyaev
 * SWBLURAY-21863: Unified support PES sub-packetization for MP4 and MKV
 * players
 * 
 * 29   5/14/10 3:57p vsilyaev
 * SW3548-2918: Added range limit into the bmedia_video_scan_scode
 * function
 * 
 * 28   5/6/10 3:42p vsilyaev
 * SW7405-3773: Added support for demuxing fragments from fragmented MP4
 * container
 * 
 * 27   4/15/10 12:27p vsilyaev
 * SWBLURAY-20165: Added support to extract multichannel PCM audio
 * 
 * 26   2/24/10 4:36p vsilyaev
 * SW7468-69: Use common definiton for BCMA packet type
 * 
 * 25   12/22/09 1:20p vsilyaev
 * SW7405-3646: replaced bmedia_probe_es_scan_scode with
 * bmedia_video_scan_scode
 * 
 * 24   10/16/09 1:10p vsilyaev
 * SW7405-3217: VC1 support in AVI streams
 * 
 * fix_bdvd_v2.0_pr16643/1   10/14/09 3:45p gooslin
 * Add VC1 support to AVI
 * 
 * 23   7/16/09 3:31p vsilyaev
 * PR 55653: Improved handling of WAVFORMAT object
 * 
 * 22   6/10/09 12:44p vsilyaev
 * PR 55896: Merged changes for extended ASF codec support
 * 
 * dev_bdvd_v2.1_extend_asf_codec_support/1   6/2/09 5:53p gooslin
 * Extend ASF codec support
 * 
 * 21   5/15/09 11:36a vsilyaev
 * PR 55142: Removed test for WMV1 and WMV2 codecs
 * 
 * 20   5/15/09 11:22a vsilyaev
 * PR 55142: Fixed use of VC1/WMV codecs, separated VC1_SM/VC_AP and rest
 * of WMV
 * 
 * 19   5/15/09 10:56a vsilyaev
 * PR 55142: Added MP43 video support for ASF files
 * 
 * dev_bdvd_v1.5_add_mp4v3_support_to_asf/1   5/13/09 5:57p gooslin
 * Move DivX vectors to a more generic location
 * 
 * 18   4/8/09 11:29a vsilyaev
 * PR 53319: Added user data insertion for the divx FOURCC's
 * 
 * 17   4/1/09 3:15p vsilyaev
 * PR 53744: Use common location for the H264 RAI sequence
 * 
 * 16   3/26/09 11:35a vsilyaev
 * PR 53617: Handling of large WaveFormatEx structures
 * 
 * fix_bdvd_v2.0_pr13769/1   3/20/09 3:21p gooslin
 * Allow truncating the WaveFormatEx struct
 * 
 * 15   3/16/09 5:47p vsilyaev
 * PR 53282: Added code to derive audio sampling rate
 * 
 * fix_bdvd_v2.0_pr13666/1   3/13/09 3:55p gooslin
 * Calculate sample rate when an invalid coded sample rate is detected
 * 
 * 14   3/5/09 7:02p vsilyaev
 * PR 52434: Unify location of frame start codes
 * 
 * 13   3/4/09 3:31p vsilyaev
 * PR 52434: MPEG4 parsing functions should be compiled with bmedia_player
 * 
 * 12   3/2/09 3:05p vsilyaev
 * PR 52434: Post merge updates/unification of parsing video data
 * 
 * 11   3/2/09 10:27a vsilyaev
 * PR 52434: Merge XViD and VC1 AP support in MKV
 * 
 * fix_bdvd_v1.42_pr12986/5   2/23/09 12:17p gooslin
 * Add VC1AP seq header parser
 * 
 * fix_bdvd_v1.42_pr12986/4   2/20/09 5:50p gooslin
 * Add VC1 frame detection
 * 
 * fix_bdvd_v1.42_pr12986/3   2/19/09 5:32p gooslin
 * Add first pass VC1AP support
 * 
 * fix_bdvd_v1.42_pr12986/2   2/18/09 12:00p gooslin
 * Add video picture type
 * 
 * fix_bdvd_v1.42_pr12986/1   2/18/09 11:52a gooslin
 * Move read bitmapinfo/waveformatex calls to bmedia scope
 * 
 * 10   1/12/09 12:36p vsilyaev
 * PR 50763: Added insertion of EOS packets
 * 
 * 9   12/10/08 1:38a vsilyaev
 * PR 48760: Added ES player for accurate progress monitor and navigation
 * 
 * 8   12/4/08 5:25p vsilyaev
 * PR 49993: Added code to inform application about errors detected during
 * stream parsing
 * 
 * 7   11/26/08 12:12p vsilyaev
 * PR 49646: Use bmedia_probe_util
 * 
 * 6   11/25/08 1:12a vsilyaev
 * PR 49646: Added code to parse and print H264 profile/level information
 * 
 * 5   11/4/08 9:57a vsilyaev
 * PR 48608: Merged code that alligns audio/video streams after seek.
 * 
 * bdvd_v1.4/bdvd_v1.42/fix_bdvd_v1.42_pr11402/1   11/1/08 7:46p gooslin
 * Cleanup A/V sync modifications
 * 
 * bdvd_v1.4/1   9/25/08 2:50p gooslin
 * PR_10564 [ &nbsp; ].  Clean up ASF A/V sync code
 * 
 * fix_bdvd_v1.4_pr10564/1   9/24/08 8:24p gooslin
 * Add sync support for AVI
 * 
 * 4   5/16/08 10:42a vsilyaev
 * PR 42379: Unified timestamp calculation between avi_player and
 * avi_stream. Added heuristics in timestamp calculation to pass quirks
 * in certain streams.
 * 
 * 3   3/3/08 12:17p vsilyaev
 * PR 39818: Added utility functions to construct PES header and AAC ADTS
 * header
 * 
 * 2   12/20/07 10:29a vsilyaev
 * PR 38073: Updated to work with new bmedia_player
 * 
 * PR38073/1   12/18/07 5:02p vsilyaev
 * PR 38073: Improved code to handle MPEG-2 TS packets
 * 
 * 1   4/24/07 4:34p vsilyaev
 * PR 29921: Timestamp to PTS conversion
 * 
 * 
 *******************************************************************************/
#ifndef _BMEDIA_UTIL_H__
#define _BMEDIA_UTIL_H__

#include "bioatom.h"
#include "bmedia_pes.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
Summary:
  position in msec 
**/
typedef unsigned long bmedia_player_pos;

#define B_MEDIA_INVALID_PTS 0xFFFFFFFFul
#define B_MEDIA_MARKER_PTS 0xFFFFFFFFul
#define B_MEDIA_MAX_PES_SIZE    32768 

#define BMPEG2PES_PTS_UNITS	45000
#define BMEDIA_TIME_SCALE_BASE 100
#define BMEDIA_PTS_MODULO		(((uint64_t)1)<<32)
typedef long bmedia_time_scale; /* time scale is a fixed point number with base (1.0) equal to BMEDIA_TIME_SCALE_BASE */
uint32_t bmedia_pts2time(uint32_t pts, bmedia_time_scale scale);
uint32_t bmedia_time2pts(uint32_t time, bmedia_time_scale scale);

typedef struct bmedia_adts_header {
    uint8_t adts[3];
    uint8_t adts_3;
} bmedia_adts_header;

typedef struct bmedia_info_aac {
    uint8_t sampling_frequency_index;
    uint8_t channel_configuration;
    uint8_t profile;
} bmedia_info_aac;

bool bmedia_info_aac_set_sampling_frequency_index(bmedia_info_aac *aac, unsigned sampling_frequency);
unsigned bmedia_info_aac_sampling_frequency_from_index(unsigned index);
bool bmedia_info_aac_set_channel_configuration(bmedia_info_aac *aac, unsigned nchannels);

void bmedia_adts_header_init_aac(bmedia_adts_header *header, const bmedia_info_aac *aac);
#define BMEDIA_ADTS_HEADER_SIZE 7
size_t bmedia_adts_header_fill(uint8_t *buf, const bmedia_adts_header *header, size_t frame_length);

typedef struct bmedia_bcma_hdr {
	bmedia_packet_header pes; /* should be first */
	uint8_t pkt_len[4]; 
} bmedia_bcma_hdr;

typedef struct bmedia_bcmv_hdr {
	bmedia_packet_header pes;
	uint8_t header[4/*size*/ + 2/* type */];
} bmedia_bcmv_hdr;

typedef struct bmedia_adts_hdr {
	bmedia_packet_header pes;
	uint8_t adts[BMEDIA_ADTS_HEADER_SIZE ];
} bmedia_adts_hdr;

typedef struct bmedia_pes_info {
    uint8_t stream_id;
    bool dts_valid;
    bool pts_valid;
    uint32_t pts; 
    uint32_t dts;
} bmedia_pes_info;

#define BMEDIA_FIELD_LEN(name,type) sizeof(type)

#define BMEDIA_PES_HEADER_MAX_SIZE 32
#define BMEDIA_WAVEFORMATEX_BASE_SIZE          18
#define BMEDIA_WAVEFORMATEX_MAX_ASF_EXTENSION_SIZE 10
#define BMEDIA_WAVEFORMATEX_MAX_PCM_EXTENSION_SIZE (BMEDIA_FIELD_LEN(samplesPerBlock, uint16_t)+BMEDIA_FIELD_LEN(channelMask, uint32_t)+BMEDIA_FIELD_LEN(subFormat, bmedia_guid))

#define BMEDIA_PES_INFO_INIT(info, id) do {(info)->stream_id=(id);(info)->pts_valid=false;(info)->dts_valid=false;}while(0)
#define BMEDIA_PES_SET_PTS(info, pts_value) do{(info)->pts_valid=true;(info)->pts=pts_value;}while(0)
#define BMEDIA_PES_SET_DTS(info, dts_value) do{(info)->dts_valid=true;(info)->dts=dts_value;}while(0)
#define BMEDIA_PES_UNSET_DTS(info) do{(info)->dts_valid=false;}while(0)
#define BMEDIA_PES_UNSET_PTS(info) do{(info)->pts_valid=false;}while(0)
void bmedia_pes_info_init(bmedia_pes_info *info, uint8_t id);
size_t bmedia_pes_header_init(uint8_t *pes_header, size_t length, const bmedia_pes_info *info);

typedef struct bmedia_parsing_errors {
    unsigned sync_errors;
    unsigned resync_events;
    unsigned format_errors;
} bmedia_parsing_errors;

#define BMEDIA_PARSING_ERRORS_INIT(errors) do {(errors)->sync_errors=(errors)->resync_events=(errors)->format_errors=0;}while(0)

extern const batom_user bmedia_bcma_atom;
extern const batom_user bmedia_bcmv_atom;
extern const batom_user bmedia_adts_atom;
extern const batom_vec bmedia_eos_h264;
extern const batom_vec bmedia_eos_mpeg4;
extern const batom_vec bmedia_frame_mpeg4;
extern const batom_vec bmedia_frame_bcma;
extern const batom_vec bmedia_frame_bcmv;
extern const batom_vec bmedia_eos_bcmv;
extern const batom_vec bmedia_frame_vc1;
extern const batom_vec bmedia_eos_vc1;
extern const batom_vec bmedia_eos_stuffing;
extern const batom_vec bmedia_null_vec;
extern const batom_vec bmedia_rai_h264;
extern const batom_vec bmedia_divx311_seq_1;
extern const batom_vec bmedia_divx311_seq_2;
extern const batom_vec bmedia_divx4_seq;
extern const batom_vec bmedia_nal_vec;
extern const batom_vec bmedia_jpeg_stream_header;
extern const batom_vec bmedia_jpeg_default_dht;
extern const batom_vec bmedia_jpeg_dqt;
extern const batom_vec bmedia_jpeg_dht;
extern const batom_vec bmedia_jpeg_sof;
extern const batom_vec bmedia_jpeg_sos;
extern const batom_vec bmedia_jpeg_eoi;

typedef enum bmedia_video_pic_type {
	bmedia_video_pic_type_I,
	bmedia_video_pic_type_P,
	bmedia_video_pic_type_B,
	bmedia_video_pic_type_skipped,
	bmedia_video_pic_type_unknown
} bmedia_video_pic_type;

typedef struct bmedia_waveformatex {
	uint16_t	wFormatTag;
	uint16_t	nChannels;
	uint32_t	nSamplesPerSec;
	uint32_t 	nAvgBytesPerSec;
	uint16_t	nBlockAlign;
	uint16_t	wBitsPerSample;
	uint16_t	cbSize;
    unsigned    meta_length;
	uint8_t     meta[128];
} bmedia_waveformatex;

typedef struct bmedia_waveformatex_header {
	uint16_t	wFormatTag;
	uint16_t	nChannels;
	uint32_t	nSamplesPerSec;
	uint32_t 	nAvgBytesPerSec;
	uint16_t	nBlockAlign;
	uint16_t	wBitsPerSample;
	uint16_t	cbSize;
}bmedia_waveformatex_header;

typedef struct bmedia_bitmapinfo {
	uint32_t	biSize;
	int32_t	    biWidth;
	int32_t	    biHeight;
	uint16_t	biPlanes;
	uint16_t	biBitCount;
	uint32_t	biCompression;
	uint32_t	biSizeImage;
	int32_t	    biXPelsPerMeter;
	int32_t	    biYPelsPerMeter;
	uint32_t	biClrUsed;
	uint32_t	biClrImportant;
} bmedia_bitmapinfo;

typedef struct bmedia_vc1ap_info {
    bool interlaced;
} bmedia_vc1ap_info;

typedef struct bmedia_vc1sm_info {
	bool finterpflag;
	bool rangered;
	bool maxbframes;
} bmedia_vc1sm_info;

typedef struct bmedia_guid {
	uint8_t guid[16]; 
} bmedia_guid;

typedef struct bmedia_waveformatextensible {
    union {
        uint16_t validBitsPerSample;
        uint16_t samplesPerBlock;
        uint16_t reserved;
    } samples;
    uint32_t channelMask;
    bmedia_guid subformat;
} bmedia_waveformatextensible;


typedef struct bmedia_h264_meta_data {
    unsigned no;
    const uint8_t *data;
}bmedia_h264_meta_data;

typedef struct bmedia_h264_meta {
    uint8_t configurationVersion;
    uint8_t profileIndication;
    uint8_t profileCompatibility;
    uint8_t levelIndication;
    size_t nalu_len;
    bmedia_h264_meta_data sps;
    bmedia_h264_meta_data pps;
} bmedia_h264_meta;

typedef enum b_media_dts_cd_search_state {
    b_media_dts_cd_search_state_init,
    b_media_dts_cd_search_state_14bit_syncword_1,
    b_media_dts_cd_search_state_14bit_syncword_2,
    b_media_dts_cd_search_state_16bit_syncword_1,
    b_media_dts_cd_search_state_16bit_syncword_2
} b_media_dts_cd_search_state;

int bmedia_compare_guid(const bmedia_guid *g1, const bmedia_guid *g2);
bool bmedia_read_guid(batom_cursor *c, bmedia_guid *guid);
const char* bmedia_guid2str(const bmedia_guid *guid, char *str, size_t size);


#define BMEDIA_FOURCC_FORMAT "'%c%c%c%c'"
#define BMEDIA_FOURCC_ARG(f) ((f)&0xFF), (((f)>>8)&0xFF), (((f)>>16)&0xFF), (((f)>>24)&0xFF)
#define BMEDIA_FOURCC(c1,c2,c3,c4) (((uint32_t)c1) | ((uint32_t)c2<<8) | ((uint32_t)c3<<16) | ((uint32_t)c4<<24))

#define BMEDIA_FOURCC_DIVX4_CODEC(fourcc)  (\
				    BMEDIA_FOURCC('d','i','v','x')==(fourcc) || \
				    BMEDIA_FOURCC('D','I','V','X')==(fourcc))

#define BMEDIA_FOURCC_DIVX5_CODEC(fourcc)  (\
                    BMEDIA_FOURCC_DIVX4_CODEC(fourcc) || \
                   	BMEDIA_FOURCC('m','p','4','s')==(fourcc) || \
                   	BMEDIA_FOURCC('M','P','4','S')==(fourcc) || \
				    BMEDIA_FOURCC('d','i','v','5')==(fourcc) || \
				    BMEDIA_FOURCC('D','I','V','5')==(fourcc) || \
				    BMEDIA_FOURCC('d','i','v','6')==(fourcc) || \
				    BMEDIA_FOURCC('D','I','V','6')==(fourcc) || \
				    BMEDIA_FOURCC('d','x','5','0')==(fourcc) || \
				    BMEDIA_FOURCC('D','X','5','0')==(fourcc))

#define BMEDIA_FOURCC_XVID_CODEC(fourcc) (\
                    BMEDIA_FOURCC('x','v','i','d')==(fourcc) || \
                    BMEDIA_FOURCC('X','V','I','D')==(fourcc) || \
                    BMEDIA_FOURCC('F','M','P','4')==(fourcc) || \
                    BMEDIA_FOURCC('f','m','p','4')==(fourcc))

#define BMEDIA_FOURCC_3IVX_CODEC(fourcc) (\
                    BMEDIA_FOURCC('3','I','V','1')==(fourcc) || \
                    BMEDIA_FOURCC('3','i','v','1')==(fourcc) || \
                    BMEDIA_FOURCC('3','I','V','2')==(fourcc) || \
                    BMEDIA_FOURCC('3','i','v','2')==(fourcc))

#define BMEDIA_FOURCC_H264_CODEC(fourcc) (\
					BMEDIA_FOURCC('v','s','s','h')==(fourcc) || \
					BMEDIA_FOURCC('V','S','S','H')==(fourcc) || \
					BMEDIA_FOURCC('A','V','C',' ')==(fourcc) || \
					BMEDIA_FOURCC('a','v','c',' ')==(fourcc) || \
					BMEDIA_FOURCC('A','V','C','1')==(fourcc) || \
					BMEDIA_FOURCC('a','v','c','1')==(fourcc) || \
					BMEDIA_FOURCC('H','2','6','4')==(fourcc) || \
					BMEDIA_FOURCC('h','2','6','4')==(fourcc))

#define BMEDIA_FOURCC_DIVX3_CODEC(fourcc)  (\
           			BMEDIA_FOURCC('d','i','v','3')==(fourcc) || \
					BMEDIA_FOURCC('D','I','V','3')==(fourcc) || \
                   	BMEDIA_FOURCC('m','p','4','3')==(fourcc) || \
                   	BMEDIA_FOURCC('M','P','4','3')==(fourcc) || \
                   	BMEDIA_FOURCC('d','i','v','4')==(fourcc) || \
					BMEDIA_FOURCC('D','I','V','4')==(fourcc))

#define BMEDIA_FOURCC_MPEG2_CODEC(fourcc)  (\
           			BMEDIA_FOURCC('m','p','g','2')==(fourcc) || \
					BMEDIA_FOURCC('M','P','G','2')==(fourcc))

#define BMEDIA_FOURCC_VC1AP_CODEC(fourcc)  (\
           			BMEDIA_FOURCC('w','v','c','1')==(fourcc) || \
           			BMEDIA_FOURCC('W','V','C','1')==(fourcc) || \
           			BMEDIA_FOURCC('w','m','v','a')==(fourcc) || \
					BMEDIA_FOURCC('W','M','V','A')==(fourcc))

#define BMEDIA_FOURCC_VC1SM_CODEC(fourcc)  (\
           			BMEDIA_FOURCC('w','m','v','3')==(fourcc) || \
					BMEDIA_FOURCC('W','M','V','3')==(fourcc))

#define BMEDIA_FOURCC_MJPEG_CODEC(fourcc)  (\
				    BMEDIA_FOURCC('m','j','p','g')==(fourcc) || \
				    BMEDIA_FOURCC('M','J','P','G')==(fourcc))

#define BMEDIA_FOURCC_WAVE(fourcc) (fourcc==BMEDIA_FOURCC('W','A','V','E'))
#if 0
/* not supported */
#define BMEDIA_FOURCC_WMV_CODEC(fourcc)  (\
           			BMEDIA_FOURCC('w','m','v','1')==(fourcc) || \
           			BMEDIA_FOURCC('W','M','V','1')==(fourcc) || \
           			BMEDIA_FOURCC('w','m','v','2')==(fourcc) || \
           			BMEDIA_FOURCC('W','M','V','2')==(fourcc) || 
#endif


#define BMEDIA_WAVFMTEX_AUDIO_WMA(audio) \
                    ((audio)->wFormatTag==0x0161)

#define BMEDIA_WAVFMTEX_AUDIO_WMA_PRO(audio) \
                    ((audio)->wFormatTag==0x0162)

#define BMEDIA_WAVFMTEX_AUDIO_PCM(audio) \
                    bmedia_waveformatex_is_pcm(audio)

#define BMEDIA_WAVFMTEX_AUDIO_MP3(audio) \
                    ((audio)->wFormatTag==0x0055)

#define BMEDIA_WAVFMTEX_AUDIO_MPEG(audio) \
                    ((audio)->wFormatTag==0x0050)

#define BMEDIA_WAVFMTEX_AUDIO_AAC(audio) \
                    ((audio)->wFormatTag==0x00FF)

#define BMEDIA_WAVFMTEX_AUDIO_RAW_AAC(audio) \
                    ((audio)->wFormatTag==0x1601)

#define BMEDIA_WAVFMTEX_AUDIO_AC3(audio) \
                    ((audio)->wFormatTag==0x2000)

#define BMEDIA_WAVFMTEX_AUDIO_DTS(audio) \
                    ((audio)->wFormatTag==0x2001)

#define BMEDIA_WAVFMTEX_AUDIO_MS_ADPCM(audio) \
                    ((audio)->wFormatTag==0x0002)

#define BMEDIA_WAVFMTEX_AUDIO_DVI_ADPCM(audio) \
                    ((audio)->wFormatTag==0x0011)

#define BMEDIA_WAVFMTEX_AUDIO_ADPCM(audio) (BMEDIA_WAVFMTEX_AUDIO_MS_ADPCM(audio) || BMEDIA_WAVFMTEX_AUDIO_DVI_ADPCM(audio))

#define BMEDIA_WAVFMTEX_AUDIO_G726(audio) \
                    ((audio)->wFormatTag==0x0064)


bool bmedia_read_bitmapinfo(bmedia_bitmapinfo *bi, batom_cursor *c);

bool bmedia_read_waveformatex(bmedia_waveformatex *wf, batom_cursor *c);
size_t bmedia_copy_waveformatex(const bmedia_waveformatex *wf, void *buf, size_t max_length);
size_t bmedia_write_waveformatex(void *buf, const bmedia_waveformatex_header *wf); /* writes waveformatex into the flat buffer, it _does_not_ write opaque data */
void bmedia_init_waveformatex(bmedia_waveformatex_header *wf);
bool bmedia_read_waveformatextensible(const bmedia_waveformatex *wf, bmedia_waveformatextensible *wfe);


bool bmedia_waveformatex_is_pcm(const bmedia_waveformatex *wf);

size_t bmedia_waveformatex_pcm_block_size(const bmedia_waveformatex *wf);

bool bmedia_vc1ap_read_info(bmedia_vc1ap_info *info, batom_cursor *cursor);

bool bmedia_vc1sm_read_info(bmedia_vc1sm_info *info, batom_cursor *cursor);

bmedia_video_pic_type bmedia_vc1ap_read_pic_type(batom_cursor *cursor, const bmedia_vc1ap_info *info);

bmedia_video_pic_type bmedia_vc1sm_read_pic_type(batom_cursor *cursor, const bmedia_vc1sm_info *info);

/* returns either startcode suffix or -1 if start code wasn't found at current position */
int bmedia_video_read_scode(batom_cursor *cursor);

bmedia_video_pic_type bmpeg4_video_read_pic_type(batom_cursor *cursor);

void bmedia_bcma_hdr_init(bmedia_bcma_hdr *hdr, size_t payload_len);

void bmedia_bcmv_hdr_init(bmedia_bcmv_hdr *hdr, size_t payload_len);

void bmedia_adts_hdr_init(bmedia_adts_hdr *hdr, const bmedia_adts_header *adts, size_t payload_len);

batom_t bmedia_pes_subdivide_packet(batom_accum_t acc_src, bmedia_pes_info *pes_info, batom_accum_t acc_dst, uint8_t *hdr_buf, size_t hdr_buf_size, size_t max_pes_size);

bool bmedia_is_dts_cd(batom_cursor *cursor);

#define B_MEDIA_VIDEO_SCODE_LEN	(4)
/* looks for startcode startcode in first 'range' baytes and then return startcode or 0 if start code wasn't found */
uint32_t bmedia_video_scan_scode(batom_cursor *cursor, size_t range);

bool bmedia_read_h264_meta(bmedia_h264_meta *meta, const void *data, size_t data_len);
void bmedia_copy_h264_meta(batom_accum_t dst, const bmedia_h264_meta *meta);
void bmedia_copy_h264_meta_with_nal_vec(batom_accum_t dst, const bmedia_h264_meta *meta, const batom_vec *nal_vec);

const void *bmedia_seek_h264_meta_data(bmedia_h264_meta_data *meta_data, unsigned n, size_t *size);

#define B_MEDIA_SAVE_UINT16_BE(b, d) do { (b)[0] = (uint8_t)((d)>>8); (b)[1]=(uint8_t)(d);}while(0)
#define B_MEDIA_SAVE_UINT32_BE(b, d) do { (b)[0] = (uint8_t)((d)>>24);(b)[1]=(uint8_t)((d)>>16);(b)[2]=(uint8_t)((d)>>8); (b)[3] = (uint8_t)(d);}while(0)

#define B_MEDIA_SAVE_UINT16_LE(b, d) do { (b)[0] = (uint8_t)(d); (b)[1]=(uint8_t)((d)>>8);}while(0)
#define B_MEDIA_SAVE_UINT32_LE(b, d) do { (b)[0] = (uint8_t)(d); (b)[1]=(uint8_t)((d)>>8);(b)[2]=(uint8_t)((d)>>16); (b)[3] = (uint8_t)((d)>>24);}while(0)

#define B_MEDIA_LOAD_UINT16_BE(p,off) \
			(((uint16_t)(((uint8_t *)(p))[(off)+0])<<8) | \
			((uint16_t)(((uint8_t *)(p))[(off)+1])))

#define B_MEDIA_LOAD_UINT32_BE(p,off) \
			(((uint32_t)(((uint8_t *)(p))[(off)+0])<<24) | \
			((uint32_t)(((uint8_t *)(p))[(off)+1])<<16) | \
			((uint32_t)(((uint8_t *)(p))[(off)+2])<<8) | \
			((uint32_t)(((uint8_t *)(p))[(off)+3])))

#define B_MEDIA_LOAD_UINT32_LE(p,off) \
			(((uint32_t)(((uint8_t *)(p))[(off)+3])<<24) | \
			((uint32_t)(((uint8_t *)(p))[(off)+2])<<16) | \
			((uint32_t)(((uint8_t *)(p))[(off)+1])<<8) | \
			((uint32_t)(((uint8_t *)(p))[(off)+0])))

#define B_MEDIA_MPEG2TS_TRACKNO_MAKE(pid,subid) ((pid)|((subid)<<16))
#define B_MEDIA_MPEG2TS_TRACKNO_PID(track) ((track)&0x1FFF)
#define B_MEDIA_MPEG2TS_TRACKNO_SUBID(track) (((track)>>16)



#ifdef __cplusplus
}
#endif

#endif /* _BMEDIA_UTIL_H__ */

