/***************************************************************************
 *     Copyright (c) 2007-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmp4_probe.h $
 * $brcm_Revision: 15 $
 * $brcm_Date: 6/20/11 6:38p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmp4_probe.h $
 * 
 * 15   6/20/11 6:38p vsilyaev
 * SWBLURAY-26273: Communicate MP4 DRM information to application
 * 
 * 14   11/4/10 11:29a vsilyaev
 * SW35230-2096: Added extraction of the language information
 * 
 * 13   8/26/10 5:35p vsilyaev
 * SW7420-876: Return location of the rmra box
 * 
 * 12   3/19/10 5:56p vsilyaev
 * SW7405-3773: Added detection of fragmented MP4 files
 * 
 * 11   1/14/10 12:50p vsilyaev
 * SWBLURAY-14718: Updated comments in regard to track duration and number
 * of samples, this information is sufficient to extract frame rate for
 * the video tracks
 * 
 * 10   1/14/10 12:40p vsilyaev
 * SWBLURAY-14718: Merged frame rate detection
 * 
 * fix_bdvd_v3.0_issue18060/1   1/14/10 1:30p gooslin
 * Return video frame rate on probe operation
 * 
 * 9   12/8/09 10:46a vsilyaev
 * SW7405-3556: Added flag to mark encrypted tracks
 * 
 * fix_bdvd_v3.0_issue16965/2   12/8/09 9:16a ialauder
 * Rename encryption flag
 * 
 * fix_bdvd_v3.0_issue16965/1   12/7/09 5:35p ialauder
 * AAC DRM detection
 * 
 * 8   12/1/09 3:29p vsilyaev
 * SW7405-3513: Return AAC+ codec if stream has SBR information
 * 
 * 7   12/1/09 3:13p vsilyaev
 * SW7405-3513: Merged changes to probe SBR information in the AAC streams
 * 
 * fix_bdvd_v3.0_issue17634/1   11/30/09 1:41p ialauder
 * AAC-HE support
 * 
 * 6   3/16/09 5:47p vsilyaev
 * PR 53282: Added code to derive audio sampling rate
 * 
 * fix_bdvd_v2.0_pr13666/1   3/13/09 3:55p gooslin
 * Calculate sample rate when an invalid coded sample rate is detected
 * 
 * 5   12/4/08 5:46p vsilyaev
 * PR 49899: Detect all brands of Mpeg4Part12 containers, and pass
 * compability test to application
 * 
 * 4   9/17/08 6:25p vsilyaev
 * PR 45431: Added segments for moov and mdat objects
 * 
 * 3   7/25/07 1:50p vsilyaev
 * PR 32813: Separated public and internal API of bmedia_probe
 * 
 * 2   7/17/07 4:31p vsilyaev
 * PR 32813: Added MPEG-2 TS support for the probe library
 * 
 * 1   7/9/07 4:05p vsilyaev
 * PR 32813: MP4 probe module
 * 
 *******************************************************************************/
#ifndef _BMP4_PROBE_H__
#define _BMP4_PROBE_H__

#include "bmedia_probe_impl.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bmp4_probe_segment {
    uint64_t offset;
    uint64_t size;
} bmp4_probe_segment;

typedef struct bmp4_probe_stream {
    bmedia_probe_stream media;
    bmp4_probe_segment moov;
    bmp4_probe_segment mdat;
    bmp4_probe_segment rmra;
    bmp4_probe_segment uuid;
    bool compatible;
    bool fragmented;
    char ftyp[5];
} bmp4_probe_stream;

typedef struct bmp4_probe_track {
    bmedia_probe_track media;
    unsigned sample_count; /* number of samples in the stream */
    uint64_t duration; /* track duration in miliseconds */
    size_t protection_scheme_information_size;
    bool encrypted;
    char language[4]; /* NULL terminated ISO-639-2/T language code */
    uint8_t protection_scheme_information[128];
} bmp4_probe_track;


extern const bmedia_probe_format_desc bmp4_probe;

#ifdef __cplusplus
}
#endif


#endif /* _BMP4_PROBE_H__ */

