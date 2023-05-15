/***************************************************************************
 *     Copyright (c) 2007-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: basf_probe.h $
 * $brcm_Revision: 16 $
 * $brcm_Date: 4/11/11 3:12p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/asf/basf_probe.h $
 * 
 * 16   4/11/11 3:12p vsilyaev
 * SWDTV-6571: Added parsing of deviceConformanceTemplate
 * 
 * 15   11/4/10 10:37a vsilyaev
 * SW35230-2093: Added parsing of language information
 * 
 * 14   9/28/10 11:04a vsilyaev
 * SW7405-4904: Added code to recongnize 'PR' files
 * 
 * 13   8/25/10 3:33p vsilyaev
 * SW3548-3068: Added stereoscopic fields
 * 
 * 12   7/22/10 4:59p vsilyaev
 * SWBLURAY-21061: Better detect DRM files
 * 
 * dev_bdvd_v3.0_issue21061/1   7/21/10 3:03p ialauder
 * Add support to probe for the Extended Content Encryption object.
 * Return Encryption version 1 if only Content Encryption is found and
 * version 2 if the Exteneded Content Encryption is found.
 * 
 * 11   2/22/10 2:37p vsilyaev
 * SWBLURAY-18922: Added audio->video and video->audio differencies.
 * 
 * 10   2/19/10 6:16p vsilyaev
 * SWBLURAY-18922: Added parsing of the index object to return distance
 * between video and audio streams.
 * 
 * fix_bdvd_v3.0_issue18922/2   2/22/10 12:13p gooslin
 * Return both the maximum distance an audio frame is ahead of a video
 * frame at a given PTS and the maximum distance a video frame is ahead
 * of an audio frame at a given PTS
 * 
 * fix_bdvd_v3.0_issue18922/1   2/19/10 3:48p gooslin
 * Detect the maximum byte difference between a audio and video frame at a
 * given PTS
 * 
 * 9   1/13/10 7:05p vsilyaev
 * SW3556-986: Returned averageTimePerFrame from the ASF Extended Stream
 * Properties Object
 * 
 * 8   12/29/09 4:25p vsilyaev
 * SW3556-959: Added ranges for the header, data and simple_index objects
 * 
 * 7   12/18/09 12:58p vsilyaev
 * SW3556-946: Added fields from the ASF FileObject
 * 
 * 6   8/19/09 11:58a vsilyaev
 * PR 57639: Added dynamic range control information
 * 
 * 5   6/3/09 7:35p vsilyaev
 * PR 55617: Added code to extract Pixel Aspect Ration
 * 
 * 4   4/28/09 5:54p vsilyaev
 * PR 53911: Added support for processing of the ASF hidden tracks
 * 
 * 3   7/25/07 1:50p vsilyaev
 * PR 32813: Separated public and internal API of bmedia_probe
 * 
 * 2   7/17/07 4:30p vsilyaev
 * PR 32813: Added MPEG-2 TS support for the probe library
 * 
 * 1   7/9/07 4:00p vsilyaev
 * PR 32813: ASF probe module
 * 
 *******************************************************************************/
#ifndef _BASF_PROBE_H__
#define _BASF_PROBE_H__

#include "bmedia_probe_impl.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define BASF_DEVICE_CONFORMANCE_TEMPLATE_BUFFER 32

/* Microsof Digital Rights Manager Version
*/
typedef enum basf_encryption_type {
    basf_encryption_none,
    basf_encryption_version_1,
    basf_encryption_version_2,
    basf_encryption_version_pr
}basf_encryption_type;

typedef struct basf_probe_segment {
    uint64_t offset;
    uint64_t size;
} basf_probe_segment;

typedef enum basf_stereoscopic_layout {
    basf_stereoscopic_SideBySideRF,
    basf_stereoscopic_SideBySideLF,
    basf_stereoscopic_OverUnderRT,
    basf_stereoscopic_OverUnderLT,
    basf_stereoscopic_DualStream,
    basf_stereoscopic_MultiStream,
    basf_stereoscopic_unknown
} basf_stereoscopic_layout;

typedef struct basf_probe_stereoscopic_info {
    bool stereoscopic; /* set to true of stream is stereoscopic */
    bool skip_valid;
    basf_stereoscopic_layout layout; 
    uint64_t skip;
} basf_probe_stereoscopic_info;

typedef struct basf_probe_stream {
	bmedia_probe_stream media;
    bool seekableFlag;
    bool broadcastFlag;
    bool liveFlag;
    uint64_t fileSize;
    basf_probe_segment header;
    basf_probe_segment data;
    basf_probe_segment simple_index;
    basf_probe_segment index;
    off_t max_audio_to_video_delta; /* The maximum number of bytes a frame of audio was found to be ahead of a frame of video at a given PTS. -1 if unknown */
    off_t max_video_to_audio_delta; /* The maximum number of bytes a frame of video was found to be ahead of a frame of audio at a given PTS. -1 if unknown */
    basf_encryption_type encryption_type;
    basf_probe_stereoscopic_info stereoscopic_info;
    unsigned deviceConformanceTemplateCount; /* number of entries in deviceConformanceTemplates */
    uint16_t deviceConformanceTemplates[BASF_DEVICE_CONFORMANCE_TEMPLATE_BUFFER]; /* sequence of UCS-2 NULL termiated strings */
} basf_probe_stream;

typedef struct basf_probe_track {
	bmedia_probe_track media;
	bool encryptedContentFlag;
    bool hidden;
    bool aspectRatioValid; /* qualifier for the aspectRation field */
    bool dynamicRangeControlValid;
    struct {
        unsigned x; 
        unsigned y;
    } aspectRatio; /* Pixel aspect ratio from the Metadata Object */
    struct {
        unsigned peakReference;
        unsigned peakTarget;
        unsigned averageReference;
        unsigned averageTarget;
    } dynamicRangeControl;
    uint64_t averageTimePerFrame; /* average time duriation, in 100-nanosecond units, 0 if unknown */
    uint16_t *language; /* if NOT NULL, then pointer to UCS-2 NULL termiated string that specifies the track language, should use RFC-1766 */
} basf_probe_track;
		

extern const bmedia_probe_format_desc basf_probe ;

#ifdef __cplusplus
}
#endif


#endif /* _BASF_PROBE_H__ */

