/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmp4_track.h $
 * $brcm_Revision: 10 $
 * $brcm_Date: 2/23/12 3:33p $
 *
 * Module Description:
 *
 * MP4 library, media track interface
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmp4_track.h $
 * 
 * 10   2/23/12 3:33p vsilyaev
 * SW7425-2422: Group PCM samples to reduce SW overhead
 * 
 * 9   10/17/11 2:20p vsilyaev
 * SW7425-1516: For the edit box use timescale from the movie header, not
 * from the track header
 * 
 * 8   4/8/10 6:58p vsilyaev
 * SW7405-3773: Initial support for playback of MPEG4-Part12 fragmented
 * files
 * 
 * 7   2/23/10 10:59a vsilyaev
 * SW3556-913: On seek differentiate 'endoffile' and stream/file error,
 * and propagate error in the case of error
 * 
 * 6   2/18/10 12:05p vsilyaev
 * SW3556-913: Differentiate between index error and end of stream
 * 
 * 5   1/14/10 7:43p vsilyaev
 * SW7405-3050: Added support for parsing the edit box
 * 
 * 4   5/10/07 5:05p vsilyaev
 * PR 28631: Added support for navigation(seek)
 * 
 * 3   4/30/07 4:56p vsilyaev
 * PR 28631: Added sample scheduller
 * 
 * 2   4/29/07 3:55p vsilyaev
 * PR 28631: Added processing of composition time
 * 
 * 1   4/29/07 1:38p vsilyaev
 * PR 28631: Interface to navigate in the single MP4 track
 * 
 * 
 *******************************************************************************/
#ifndef _BMP4_TRACK_H__
#define _BMP4_TRACK_H__

#include "bmp4_util.h"
#include "bfile_cache.h"
#include "bmedia_player.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bmp4_track_info {
	bmp4_trackheaderbox trackheader;
	bmp4_mediaheaderbox mediaheader;
	bmp4_handlerbox handler;
	bmp4_sample_info	sample_info;
	bfile_segment decode_t2s; /* (decoding) time-to-sample */
	bfile_segment composition_t2s; /* (composition) time-to-sample */
	bfile_segment samplesize;
	bfile_segment usamplesize;
	bfile_segment sampletochunk; /* sample-to-chunk, partial data-offset information */
	bfile_segment chunkoffset; /* chunk offset, partial data-offset information */ 
	bfile_segment chunkoffset64; 
	bfile_segment syncsample;
    bfile_segment edit;
    const bmp4_movieheaderbox *movieheader;
} bmp4_track_info;

typedef struct bmp4_sample {
	off_t offset;
	size_t len;
	bmedia_player_pos time; /* composition time, in msec */
    unsigned sample_count;
	bool	syncsample; 
    bool    endofstream;
} bmp4_sample;

typedef struct bmp4_stream_sample_status {
    unsigned sample_count; /* number of samples immediattly avaliable in the track */
} bmp4_stream_sample_status;

typedef struct bmp4_stream_sample *bmp4_stream_sample_t;

void bmp4_track_info_init(bmp4_track_info *track);
bmp4_stream_sample_t bmp4_stream_sample_create(bfile_io_read_t fd, const bmp4_track_info *track);
void bmp4_stream_sample_destroy(bmp4_stream_sample_t stream);
int bmp4_stream_sample_next(bmp4_stream_sample_t stream, bmp4_sample *sample, size_t max_sample_count);
bmedia_player_pos bmp4_stream_sample_seek(bmp4_stream_sample_t stream, bmedia_player_pos pos, bool *endofstream);
void bmp4_stream_sample_get_status(bmp4_stream_sample_t stream, bmp4_stream_sample_status *status);

#ifdef __cplusplus
}
#endif

#endif /* _BMP4_TRACK_H__ */



