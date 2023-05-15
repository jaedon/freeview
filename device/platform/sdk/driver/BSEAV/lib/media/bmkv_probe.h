/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmkv_probe.h $
 * $brcm_Revision: 20 $
 * $brcm_Date: 5/4/12 3:54p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmkv_probe.h $
 * 
 * 20   5/4/12 3:54p vsilyaev
 * SW7425-2994: Updated mkv support to reject tracks with unsupported
 * compression, except the 'auxilary' tracks
 * 
 * 19   7/1/11 12:59p vsilyaev
 * SWDTV-7732: Added webm DocType
 * 
 * 18   10/1/10 5:49p vsilyaev
 * SW35230-1433: Added handling of 'Header Stripping' payload compression
 * 
 * 17   9/14/10 10:39p joyang
 * SW35230-1085: Added CodecName support.
 * 
 * 16   8/10/10 10:00a gmohile
 * SW3548-3050 : Parse multiple attachment links in trackentry
 * 
 * 15   7/27/10 4:45p gmohile
 * SW3548-2994 : Extract display width information
 * 
 * 14   7/14/10 2:48p gmohile
 * SW3548-2994 : Extrac TracksData for DRM payload
 * 
 * 13   7/2/10 1:42p gmohile
 * SW3548-2994 : Parse trick play track info
 * 
 * 12   6/21/10 12:49p vsilyaev
 * SW3548-2975: Added code to detect offset for the next EBML object (next
 * volume)
 * 
 * 11   6/15/10 4:24p gmohile
 * SW3548-2978 : Add edition entry list
 * 
 * 10   6/11/10 10:31a gmohile
 * SW3548-2978 : Add support to parse mkv chapters
 * 
 * 9   1/14/10 12:52p vsilyaev
 * SWBLURAY-14718: Return default duration of each sample, this could be
 * used to derive frame rate for the video tracks
 * 
 * 8   1/14/10 12:40p vsilyaev
 * SWBLURAY-14718: Merged frame rate detection
 * 
 * fix_bdvd_v3.0_issue18060/2   1/14/10 1:56p gooslin
 * Clarify comment
 * 
 * fix_bdvd_v3.0_issue18060/1   1/14/10 1:30p gooslin
 * Return video frame rate on probe operation
 * 
 * 7   12/1/09 6:55p vsilyaev
 * SW7405-3368: Added codec private data
 * 
 * 6   12/1/09 3:29p vsilyaev
 * SW7405-3513: Return AAC+ codec if stream has SBR information
 * 
 * 5   12/1/09 3:13p vsilyaev
 * SW7405-3513: Merged changes to probe SBR information in the AAC streams
 * 
 * fix_bdvd_v3.0_issue17634/1   11/30/09 1:41p ialauder
 * AAC-HE support
 * 
 * 4   11/23/09 4:11p vsilyaev
 * SW7405-3368: Added parsing of MKV attachments
 * 
 * 3   11/5/09 2:25p gmohile
 * SW7405-3348 : Extract mkv codec id string
 * 
 * 2   10/8/08 12:18p vsilyaev
 * PR 47280: Added support for another way of embedding AAC audio into MKV
 * files
 * 
 * 1   3/4/08 12:21p vsilyaev
 * PR 39818: MKV stream probes
 * 
 *******************************************************************************/
#ifndef _BMKV_PROBE_H__
#define _BMKV_PROBE_H__

#include "bmedia_probe_impl.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct bmkv_Attachment {
    BLST_SQ_ENTRY(bmkv_Attachment) link; /* this field is used to link attachments together */
    bool FileData_valid;
    bool FileUID_valid;
    uint64_t FileData_size; 
    uint64_t FileData_offset;
    uint64_t FileUID;
    const char *FileName;
    const char *FileDescription; 
    const char *FileMimeType;
} bmkv_Attachment;

typedef BLST_SQ_HEAD(bmkv_AttachmentList, bmkv_Attachment) bmkv_AttachmentList;


typedef struct bmkv_Chapters {
    BLST_SQ_ENTRY(bmkv_Chapters) link; /* this field is used to link chapters together */
    bool ChapterUID_valid;
    uint64_t ChapterUID;
    uint32_t ChapterTimeStart;
    uint32_t ChapterTimeEnd;
    uint32_t ChapterFlagHidden;
    uint32_t ChapterFlagEnabled;
    struct {
        const char *ChapString;
        const char *ChapLanguage;
        const char *ChapCountry;
    } ChapterDisplay;
} bmkv_Chapters;

typedef BLST_SQ_HEAD(bmkv_ChaptersList, bmkv_Chapters) bmkv_ChaptersList;

typedef struct bmkv_Editions {
      BLST_SQ_ENTRY(bmkv_Editions) link; /* this field is used to link editions together */
      uint64_t EditionUID;
      uint32_t EditionFlagHidden;
      uint32_t EditionFlagDefault;
      uint32_t EditionFlagOrdered;
      bmkv_ChaptersList chapters;
} bmkv_Editions;

typedef struct bmkv_probe_next_volume {
    bool next_volume_offset_valid;
    uint64_t next_volume_offset;
} bmkv_probe_next_volume;

typedef BLST_SQ_HEAD(bmkv_EditionsList, bmkv_Editions) bmkv_EditionsList;

typedef struct bmkv_probe_stream {
    bmedia_probe_stream media;
    bmkv_AttachmentList attachments;
    bmkv_EditionsList editions;
    bmkv_probe_next_volume next_volume;
    uint32_t TracksDataVersion;
    uint32_t TracksDataSize;
    const void *TracksDataPayload;
    char docType[16]; /* null terminated doc type */
} bmkv_probe_stream;

#define BMKV_PROBE_MAX_ATTACHMENT_LINKS 4

typedef struct bmkv_probe_track {
    bmedia_probe_track media;
    bool unsupported; /* set to true is detected some MKV syntat that can't be supported during playback */
    char language[16]; 
    char codec_id[32];
    bool TrickTrackFlag;
    bool ContentCompAlgo_valid;
    bool AttachmentLink_valid;
    unsigned ContentCompAlgo;
    uint64_t AttachmentLink[BMKV_PROBE_MAX_ATTACHMENT_LINKS];
    size_t CodecPrivate_len; /* size of the CodecPrivate data, 0 if unavailable */
    const void *CodecPrivate_data;
    size_t CodecName_len;
    uint8_t *CodecName_data;
    unsigned DisplayWidth;
    unsigned DisplayHeight;
    uint64_t DefaultDuration; /* default duration of single sample, in ns */
    uint64_t TrackUID;
    uint64_t TrickTrackUID;
    uint64_t MasterTrackUID;
} bmkv_probe_track;

extern const bmedia_probe_format_desc bmkv_probe;

#ifdef __cplusplus
}
#endif


#endif /* _BMKV_PROBE_H__ */

