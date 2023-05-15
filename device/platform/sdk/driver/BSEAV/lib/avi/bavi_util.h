/***************************************************************************
 *     Copyright (c) 2006-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bavi_util.h $
 * $brcm_Revision: 23 $
 * $brcm_Date: 4/4/11 12:18p $
 *
 * Module Description:
 *
 * AVI parser library
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/avi/bavi_util.h $
 * 
 * 23   4/4/11 12:18p vsilyaev
 * SWBLURAY-25181: Better handle AC3 audio streams
 * 
 * fix_bdvd_v3.0_sec_mdal_SWBLURAY-25181/1   4/4/11 11:39a ialauder
 * Add Format Tag to audio state
 * 
 * 22   3/2/09 10:26a vsilyaev
 * PR 52434: Merge XViD and VC1 AP support in MKV
 * 
 * fix_bdvd_v1.42_pr12986/1   2/18/09 11:48a gooslin
 * Move FourCC defines to bmedia scope
 * 
 * 21   1/15/09 12:16p vsilyaev
 * PR 49837: Added FMP4 FourCC code
 * 
 * 20   1/14/09 2:31p vsilyaev
 * PR 51102: Added 3ivx FOURCC
 * 
 * dev_bdvd_v2.0_pr12502/1   1/8/09 2:36p gooslin
 * Add 3ivx support for AVI
 * 
 * 19   12/18/08 10:42a vsilyaev
 * PR 46190: Adding support for PCM audio
 * 
 * 18   9/25/08 2:18p vsilyaev
 * PR 47334: Use calculated max audio frame size to select between VBR and
 * CBR audio
 * 
 * 17   9/2/08 4:23p vsilyaev
 * PR 46412: Added support for probing OpenDML AVI 2.0 files
 * 
 * 16   8/25/08 3:03p vsilyaev
 * PR 46119: Improved audio timestamp algorithm to handle streams with
 * CBR->VBR transition
 * 
 * 15   7/31/08 1:10p vsilyaev
 * PR 45314: Merged WMA audio support in AVI files
 * 
 * dev_bdvd_pr9870/1   7/29/08 4:22p gooslin
 * Add support for WMA streams in AVI files
 * 
 * 14   7/25/08 5:07p gmohile
 * PR42817: Added support for AVI2.0 index
 * 
 * 13   7/17/08 3:19p vsilyaev
 * PR 44915: Added lowercase FOURCC and MP43 codec type
 * 
 * 12   6/20/08 11:44a erickson
 * PR39944: fix macro
 *
 * 11   5/16/08 10:41a vsilyaev
 * PR 42379: Unified timestamp calculation between avi_player and
 * avi_stream. Added heuristics in timestamp calculation to pass quirks
 * in certain streams.
 *
 * 10   4/23/08 11:29a vsilyaev
 * PR 42029: Added H264 FourCC code
 *
 * 9   4/25/07 6:30p vsilyaev
 * PR 29815: Added code to insert EOS sequence at end of the PES stream
 *
 * 8   4/13/07 5:21p vsilyaev
 * PR 25701: Fixed typo with dwScale
 *
 * 7   3/12/07 9:45a vsilyaev
 * PR 25701: Fixed B_AVI_MIN define
 *
 * 6   2/13/07 7:02p vsilyaev
 * PR 25701: Added support for DivX3.11
 *
 * 5   2/12/07 6:14p vsilyaev
 * PR 25701: Fixed timestamps for audio
 *
 * 4   2/9/07 1:10p vsilyaev
 * PR 25701: Added stream and format specific hanlder
 *
 * 3   2/8/07 11:47p vsilyaev
 * PR 25701: bavi_stream parser
 *
 * 2   2/6/07 6:48p vsilyaev
 * PR 25701: Modified AVI parser to work with batom framework
 *
 * 1   12/18/06 7:24p vsilyaev
 * PR 25701: AVI parser
 *
 *******************************************************************************/

#ifndef _BAVI_UTIL_H__
#define _BAVI_UTIL_H__

#include "bioatom.h"
#include "bmedia_util.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* type corresponding to the 4 bytes AVI value */
typedef uint32_t bavi_atom;

/* type corresponding to the BYTE type in the AVI specification */
typedef uint8_t bavi_byte;
/* type corresponding to the WORD type in the AVI specification */
typedef uint16_t bavi_word;
/* type corresponding to the DWORD type in the AVI specification */
typedef bavi_atom bavi_dword;
typedef bavi_atom bavi_fourcc;
typedef int16_t bavi_short_int;
typedef int32_t bavi_long;

typedef struct bavi_mainheader {
	/*
	bavi_fourcc fcc;
	bavi_dword cb;
	*/
	bavi_dword	dwMicroSecPerFrame;
	bavi_dword 	dwMaxBytesPerSec;
	bavi_dword 	dwPaddingGranularity;
	bavi_dword 	dwFlags;
/* Index at end of file? */
#define BAVI_F_HASINDEX        0x00000010u
#define BAVI_F_MUSTUSEINDEX    0x00000020u
#define BAVI_F_ISINTERLEAVED   0x00000100u
/* Use CKType to find key frames */
#define BAVI_F_TRUSTCKTYPE     0x00000800u
#define BAVI_F_WASCAPTUREFILE  0x00010000u
#define BAVI_F_COPYRIGHTED     0x00020000u

	bavi_dword 	dwTotalFrames;
	bavi_dword 	dwInitialFrames;
	bavi_dword	dwStreams;
	bavi_dword	dwSuggestedBufferSize;
	bavi_dword	dwWidth;
	bavi_dword	dwHeight;
	bavi_dword	dwReserved[4];
} bavi_mainheader;

typedef struct bavi_streamheader {
	/*
	bavi_fourcc fcc;
	bavi_dword cb;
	*/
	bavi_fourcc	fccType;
	bavi_fourcc fccHandler;
	bavi_dword	dwFlags;
#define BAVI_SF_DISABLED          0x00000001u
#define BAVI_SF_VIDEO_PALCHANGES  0x00010000u
	bavi_word   wPriority;
	bavi_word 	wLanguage;
	bavi_word	dwInitialFrames;
	bavi_dword	dwScale;
	bavi_dword	dwRate;
	bavi_dword	dwStart;
	bavi_dword	dwLength;
	bavi_dword	dwSuggestedBufferSize;
	bavi_dword	dwQuality;
	bavi_dword	dwSampleSize;
	struct {
		bavi_short_int left;
		bavi_short_int top;
		bavi_short_int right;
		bavi_short_int bottom;
	}  rcFrame;
} bavi_streamheader;

typedef struct bavi_dmlh {
	bavi_dword dwTotalFrames;
} bavi_dmlh;

#define B_AVI_MIN(a,b) ((a)<(b)?(a):(b))


#define bavi_read_fourcc bavi_read_atom
#define bavi_read_dword	bavi_read_atom
#define bavi_read_byte batom_cursor_byte
#define bavi_read_word batom_cursor_uint16_le
#define bavi_read_atom batom_cursor_uint32_le
#define bavi_read_short_int (bavi_short_int)batom_cursor_uint16_le
#define bavi_read_long	(bavi_long)batom_cursor_uint32_le
#define bavi_read_qword batom_cursor_uint64_le

bool bavi_read_streamheader(bavi_streamheader *sh, batom_t header);
bool bavi_read_mainheader(bavi_mainheader *mh, batom_t header);
bool bavi_read_dmlh(bavi_dmlh *dmlh, batom_t header);

#define BAVI_FIELD_LEN(name,type) \
                    sizeof(bavi_##type)

typedef enum bavi_audio_stream_type {
        bavi_audio_stream_type_cbr, 
        bavi_audio_stream_type_vbr, 
        bavi_audio_stream_type_maybe_vbr,
        bavi_audio_stream_type_maybe_cbr
} bavi_audio_stream_type;

typedef struct bavi_audio_state {
    bavi_audio_stream_type type;
    unsigned block_count;
	size_t data_size;
    unsigned min_size;
    unsigned max_size;
    unsigned vbr_scale;
    unsigned cbr_scale;
    unsigned max_framelen;
	int skew; /* skew during transition from VBR to CBR */
	struct {
		unsigned data_size;
		unsigned block_count;
	} last;
    struct {
	    bavi_dword	dwStart;
	    bavi_dword	nSamplesPerSec;
        bavi_dword  dwRate;
		bavi_word  nBlockAlign;
    } header;
    uint16_t	wFormatTag;
} bavi_audio_state;

typedef struct bavi_audio_state_memo {
    bavi_audio_stream_type type;
	int skew; /* skew during transition from VBR to CBR */
} bavi_audio_state_memo;


void bavi_audio_state_clear_memo(bavi_audio_state_memo *memo);
void bavi_audio_state_get_memo(const bavi_audio_state *audio, bavi_audio_state_memo *memo);
void bavi_audio_state_set_memo(bavi_audio_state *audio, const bavi_audio_state_memo *memo, unsigned block_count, unsigned data_size);

void bavi_audio_state_init(bavi_audio_state *audio);
void bavi_audio_state_set_header(bavi_audio_state *audio, const bavi_streamheader *header, const bmedia_waveformatex *wave);
void bavi_audio_state_update(bavi_audio_state *audio, size_t block_len);
bmedia_player_pos bavi_audio_state_get_timestamp(const bavi_audio_state *audio, unsigned block_count, unsigned data_size);


#ifdef __cplusplus
}
#endif

#endif /* _BAVI_UTIL_H__ */

