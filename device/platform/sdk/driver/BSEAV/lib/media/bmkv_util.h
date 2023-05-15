/***************************************************************************
 *     Copyright (c) 2007-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmkv_util.h $
 * $brcm_Revision: 23 $
 * $brcm_Date: 7/6/11 6:40p $
 *
 * Module Description:
 *
 * MKV container parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmkv_util.h $
 * 
 * 23   7/6/11 6:40p vsilyaev
 * FWAVD-138: Added support for VP8 video
 * 
 * 22   7/1/11 5:43p vsilyaev
 * SWDTV-7732: Added vorbis support
 * 
 * 21   7/1/11 12:59p vsilyaev
 * SWDTV-7732: Added webm DocType
 * 
 * 20   4/11/11 5:58p vsilyaev
 * SW7405-5223: Added support for some audio codecs in A_MS/ACM format
 * 
 * 19   10/1/10 5:49p vsilyaev
 * SW35230-1433: Added handling of 'Header Stripping' payload compression
 * 
 * 18   8/10/10 10:00a gmohile
 * SW3548-3050 : Parse multiple attachment links in trackentry
 * 
 * 17   7/14/10 2:49p gmohile
 * SW3548-2994 : Extrac TracksData for DRM payload
 * 
 * 16   7/2/10 1:43p gmohile
 * SW3548-2994 : Parse trick play track info
 * 
 * 15   6/25/10 12:01p vsilyaev
 * SW3548-2995: Separate requested reserve size and minimal size for
 * succesive parsing
 * 
 * 14   6/21/10 12:49p vsilyaev
 * SW3548-2975: Added code to detect offset for the next EBML object (next
 * volume)
 * 
 * 13   6/11/10 10:31a gmohile
 * SW3548-2978 : Add support to parse mkv chapters
 * 
 * 12   11/23/09 4:11p vsilyaev
 * SW7405-3368: Added parsing of MKV attachments
 * 
 * 11   11/17/09 12:09p vsilyaev
 * SW7405-3368: Added code to insert the AttachmentLink UID
 * 
 * 10   11/2/09 3:10p gmohile
 * SW7405-3330 : Add support to extract auxiliary track
 * 
 * 9   7/1/09 11:01a vsilyaev
 * PR 56503: Fixed handling of unknown entries in the MKV record, extended
 * size of TrackUID entry to 64 bits
 * 
 * 8   3/12/09 1:45p vsilyaev
 * PR 53155: Better support for the seek table
 * 
 * fix_bdvd_v2.0_pr13665/2   3/11/09 10:33p gooslin
 * Change bmkv_elsement_parse_from_desc() result type
 * 
 * fix_bdvd_v2.0_pr13665/1   3/11/09 9:13p gooslin
 * Skip element if it is different than the element type requested
 * 
 * 7   3/2/09 10:27a vsilyaev
 * PR 52434: Merge XViD and VC1 AP support in MKV
 * 
 * fix_bdvd_v1.42_pr12986/3   2/27/09 4:53p gooslin
 * Merge to the latest bmedia code
 * 
 * 6   2/17/09 11:19a vsilyaev
 * PR 52084: Added support for MPEG audio
 * 
 * fix_bdvd_v1.42_pr12986/2   2/19/09 5:31p gooslin
 * Add first pass VC1AP support
 * 
 * fix_bdvd_v1.42_pr12986/1   2/18/09 12:04p gooslin
 * Add ssupport for MP4 in MKV
 * 
 * 5   12/19/08 12:20a vsilyaev
 * PR 50500: Added support for AC3+ audio
 * 
 * 4   10/8/08 12:18p vsilyaev
 * PR 47280: Added support for another way of embedding AAC audio into MKV
 * files
 * 
 * 3   3/5/08 3:08p vsilyaev
 * PR 39818: Added DTS audio
 * 
 * 2   3/4/08 12:38p vsilyaev
 * PR 39818: Added function to validate EBML header
 * 
 * 1   3/3/08 11:33a vsilyaev
 * PR 39818: Support of MKV format
 * 
 *******************************************************************************/
#ifndef _BMKV_UTIL_H__
#define _BMKV_UTIL_H__

#include "bioatom.h"
#include "bmedia_util.h"
#include "blst_squeue.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* Element id */
typedef uint32_t bmkv_id;

/* Size of the element */
typedef uint64_t bmkv_size;

/* maximum size of the element head (id + size) */
#define B_MKV_MAX_ELEMENT_HEAD 12 /* 4 bytes head and 8 bytes size */
#define B_MKV_MIN_ELEMENT_HEAD 2 /* 1 byte head and 1 byte size */

typedef struct bmkv_header {
    bmkv_id id;
    bmkv_size size;
} bmkv_header;

bool bmkv_parse_header(batom_cursor *cursor, bmkv_header *header);
uint64_t bmkv_parse_unsigned64(batom_cursor *cursor);
int64_t bmkv_parse_signed64(batom_cursor *cursor);
int64_t bmkv_read_signed(batom_cursor *cursor, size_t size);

#define BMKV_INVALID_ID  ((bmkv_id)(-1))
#define BMKV_INVALID_SIZE ((bmkv_size)(-1))
#define BMKV_RESERVED_SIZE ((bmkv_size)(-2))

typedef struct bmkv_unique_id {
    uint8_t data[128/8];
} bmkv_unique_id;

typedef double bmkv_float;
typedef int64_t bmkv_date;

typedef struct bmkv_utf8 {
    size_t utf8_len; /* must be the first field */
    uint8_t *utf8_data;
} bmkv_utf8;

typedef struct bmkv_data {
    size_t data_len; /* must be the first field */
    void *data;
} bmkv_data;

typedef struct bmkv_string {
    size_t string_len; /* must be the first field */
    char *string_data;
} bmkv_string;

typedef struct bmkv_table {
    size_t bmkv_table_alloc_nelems;
    size_t nelems;
    void *data;
} bmkv_table;


#define BMKV_TABLE_ELEM(table,type,n) (((type *)(table).data)[n])

typedef enum bmkv_parser_entry_type {
    bmkv_parser_entry_type_unsigned,
    bmkv_parser_entry_type_signed, 
    bmkv_parser_entry_type_unique_id,
    bmkv_parser_entry_type_date,
    bmkv_parser_entry_type_float,
    bmkv_parser_entry_type_string,
    bmkv_parser_entry_type_unsigned64,
    bmkv_parser_entry_type_id,
    bmkv_parser_entry_type_utf8,
    bmkv_parser_entry_type_table,
    bmkv_parser_entry_type_large_string,
    bmkv_parser_entry_type_data,
    bmkv_parser_entry_type_bool,
    bmkv_parser_entry_type_stop,
    bmkv_parser_entry_type_last
} bmkv_parser_entry_type;

typedef struct bmkv_parser_entry {
    bmkv_id  id;
    bmkv_parser_entry_type type;
    uint16_t elem_off;
    uint16_t elem_size;
    uint16_t validate_off;
    uint16_t validate_size;
    const char *name;
} bmkv_parser_entry;

typedef struct bmkv_parser_desc  {
    char bmkv_parser_desc_name[32]; /* must be  the first field and array */
    const bmkv_parser_entry *entries;
    size_t size;
    void (*init)(void *object);
} bmkv_parser_desc;

typedef enum bmkv_element_parse_result {
    bmkv_element_parse_result_success = 0,
    bmkv_element_parse_result_unknown_id,
    bmkv_element_parse_result_error
} bmkv_element_parse_result;

#define BMKV_PARSER_DECLARE(name) bmkv_parser_entry name[];

#define BMKV_PARSER_BEGIN(name) bmkv_parser_entry name[] = {
/* #define B_MKV_PARSER_ENTRY(id, type, object, field) {(id), (type), &((struct object *)NULL->(field)), sizeof((struct object *)NULL->(field)), &((struct object *)NULL->validate.(field))}, */
/* #define B_MKV_PARSER_ENTRY(id, type, object, field) {(id), (type), &(((struct object *)NULL)->(field))   }, */
#define B_MKV_OFFSETOF(type, member) (size_t)&(((type*)0)->member)
#define B_MKV_SIZEOF(type, member) sizeof(((type*)0)->member)
#define B_MKV_PARSER_ENTRY(id, object, type, field) {(id), (type), B_MKV_OFFSETOF(object, field), B_MKV_SIZEOF(object,field),B_MKV_OFFSETOF(object,validate.field), B_MKV_SIZEOF(object,validate.field), #field},
#define BMKV_PARSER_FIELD_UNSIGNED(object,id,field) B_MKV_PARSER_ENTRY((id), object, bmkv_parser_entry_type_unsigned, field )
#define BMKV_PARSER_FIELD_SIGNED(object,id,field) B_MKV_PARSER_ENTRY((id), object, bmkv_parser_entry_type_signed, field )
#define BMKV_PARSER_FIELD_ID(object,id,field) B_MKV_PARSER_ENTRY((id), object, bmkv_parser_entry_type_id, field )
#define BMKV_PARSER_FIELD_STRING(object,id,field) B_MKV_PARSER_ENTRY((id), object, bmkv_parser_entry_type_string, field)
#define BMKV_PARSER_FIELD_UID(object,id,field) B_MKV_PARSER_ENTRY((id), object, bmkv_parser_entry_type_unique_id, field )
#define BMKV_PARSER_FIELD_UNSIGNED64(object,id,field) B_MKV_PARSER_ENTRY((id), object, bmkv_parser_entry_type_unsigned64, field )
#define BMKV_PARSER_FIELD_FLOAT(object,id,field) B_MKV_PARSER_ENTRY((id), object, bmkv_parser_entry_type_float, field )
#define BMKV_PARSER_FIELD_DATE(object,id,field) B_MKV_PARSER_ENTRY((id), object, bmkv_parser_entry_type_date, field )
#define BMKV_PARSER_FIELD_BOOL(object,id,field) B_MKV_PARSER_ENTRY((id), object, bmkv_parser_entry_type_bool, field )
#define BMKV_PARSER_FIELD_STOP(object,id,field) B_MKV_PARSER_ENTRY((id), object, bmkv_parser_entry_type_stop, field ) 

/* B_MKV_OFFSETOF(object, field.utf8_len) shall be equal to B_MKV_OFFSETOF(object, field), however it also verifies that we pass data of cortect type */
#define BMKV_PARSER_FIELD_UTF8(object,id,field) {(id), bmkv_parser_entry_type_utf8, B_MKV_OFFSETOF(object, field.utf8_len), B_MKV_SIZEOF(object,field),B_MKV_OFFSETOF(object,validate.field), B_MKV_SIZEOF(object,validate.field), #field},

/* B_MKV_OFFSETOF(object, field.bmkv_table_alloc_nelems) shall be equal to B_MKV_OFFSETOF(object, field), however it also verifies that we pass data of correct type */
#define BMKV_PARSER_FIELD_TABLE(object,id,field,table_meta) {(id), bmkv_parser_entry_type_table, B_MKV_OFFSETOF(object, field.bmkv_table_alloc_nelems), B_MKV_SIZEOF(object,field),B_MKV_OFFSETOF(object,validate.field), B_MKV_SIZEOF(object,validate.field), table_meta.bmkv_parser_desc_name},

#define BMKV_PARSER_FIELD_DATA(object,id,field) {(id), bmkv_parser_entry_type_data, B_MKV_OFFSETOF(object, field.data_len), B_MKV_SIZEOF(object,field),B_MKV_OFFSETOF(object,validate.field), B_MKV_SIZEOF(object,validate.field), #field},

#define BMKV_PARSER_FIELD_VSTRING(object,id,field) {(id), bmkv_parser_entry_type_data, B_MKV_OFFSETOF(object, field.string_len), B_MKV_SIZEOF(object,field), B_MKV_OFFSETOF(object,validate.field), B_MKV_SIZEOF(object,validate.field), #field},


#define BMKV_PARSER_END(name) {0, bmkv_parser_entry_type_last, 0, 0, 0, 0, NULL} }

bool bmkv_element_parse(batom_cursor *cursor, size_t elem_size, const bmkv_parser_entry *entries, const char *elem_name, void *elems);
bool bmkv_element_skip(batom_cursor *cursor);
bmkv_element_parse_result bmkv_element_parse_from_desc(batom_cursor *cursor, const bmkv_parser_desc *desc, bmkv_id id, void *elem);

void bmkv_element_shutdown(const bmkv_parser_entry *entries, void *elems);
void bmkv_element_print(const bmkv_parser_entry *entries, BDBG_Level level, unsigned padding, const char *name, const void *elem);

#define BMKV_EBML_ID    0x1A45DFA3

typedef struct bmkv_EBML {
    unsigned EBMLVersion;
    unsigned EBMLReadVersion;
    unsigned EBMLMaxIDLength;
    unsigned EBMLMaxSizeLength;
    char DocType[16];
    unsigned DocTypeVersion;
    unsigned DocTypeReadVersion;
    struct {
        bool EBMLVersion;
        bool EBMLReadVersion;
        bool EBMLMaxIDLength;
        bool EBMLMaxSizeLength;
        bool DocType;
        bool DocTypeVersion;
        bool DocTypeReadVersion;
    } validate;
} bmkv_EBML;

#define BMKV_SEEK   0x4DBB
typedef struct bmkv_SeekElement{
    bmkv_id SeekID;
    uint64_t SeekPosition;
    struct {
        bool SeekID;
        bool SeekPosition;
    } validate;
} bmkv_SeekElement;
extern const bmkv_parser_desc bmkv_SeekElement_desc;

#define BMKV_SEEKHEAD_ID 0x114D9B74
typedef struct bmkv_SeekHead {
    bmkv_table Seek;
    struct {
        bool    Seek;
    } validate;
} bmkv_SeekHead;

typedef struct bmkv_ChapterTranslate {
    unsigned ChapterTranslateEditionUID;
    unsigned ChapterTranslateCodec;
    struct {
        bool ChapterTranslateEditionUID;
        bool ChapterTranslateCodec;
    } validate;
} ChapterTranslate;


#define BMKV_VOID_ID    0xEC
#define BMKV_SEGMENT_ID 0x18538067
    
#define BMKV_SEGMENTINFO_ID 0x1549A966
typedef struct bmkv_SegmentInformation {
    bmkv_unique_id SegmentUID;
    bmkv_utf8 SegmentFilename;
    bmkv_unique_id PrevUID;
    bmkv_utf8 PrevFilename;
    bmkv_unique_id NextUID;
    bmkv_utf8 NextFilename;
    bmkv_unique_id SegmentFamily;
    bmkv_table ChapterTranslate;
    unsigned TimecodeScale;
    bmkv_float Duration;
    bmkv_date DateUTC;
    bmkv_utf8 Title;
    bmkv_utf8 MuxingApp;
    bmkv_utf8 WritingApp;
    struct {
        bool SegmentUID;
        bool SegmentFilename;
        bool PrevUID;
        bool PrevFilename;
        bool NextUID;
        bool NextFilename;
        bool SegmentFamily;
        bool ChapterTranslate;
        bool TimecodeScale;
        bool Duration;
        bool DateUTC;
        bool Title;
        bool MuxingApp;
        bool WritingApp;
    } validate;
} bmkv_SegmentInformation;

typedef struct bmkv_TrackEntryVideo {
    bool FlagInterlaced;
    unsigned StereoMode;
    unsigned PixelWidth;
    unsigned PixelHeight;
    unsigned PixelCropBottom;
    unsigned PixelCropTop;
    unsigned PixelCropLeft;
    unsigned PixelCropRight;
    unsigned DisplayWidth;
    unsigned DisplayHeight;
    unsigned DisplayUnit;
    unsigned AspectRatioType;
    unsigned ColourSpace;
    bmkv_float GammaValue;
    struct {
        bool FlagInterlaced;
        bool StereoMode;
        bool PixelWidth;
        bool PixelHeight;
        bool PixelCropBottom;
        bool PixelCropTop;
        bool PixelCropLeft;
        bool PixelCropRight;
        bool DisplayWidth;
        bool DisplayHeight;
        bool DisplayUnit;
        bool AspectRatioType;
        bool ColourSpace;
        bool GammaValue;
    } validate;
} bmkv_TrackEntryVideo;

typedef struct bmkv_TrackEntryAudio {
    bmkv_float SamplingFrequency;
    bmkv_float OutputSamplingFrequency;
    unsigned Channels;
    bmkv_data ChannelPositions;
    unsigned BitDepth;
    struct {
        bool SamplingFrequency;
        bool OutputSamplingFrequency;
        bool Channels;
        bool ChannelPositions;
        bool BitDepth;
    } validate;
} bmkv_TrackEntryAudio;

typedef struct bmkv_ContentCompression {
    unsigned ContentCompAlgo;
    bmkv_data ContentCompSettings;
    struct {
        bool ContentCompAlgo;
        bool ContentCompSettings;
    } validate;
} bmkv_ContentCompression;

typedef struct bmkv_ContentEncryption {
    unsigned ContentEncAlgo;
    bmkv_data ContentEncKeyID;
    bmkv_data ContentSignature;
    bmkv_data ContentSigKeyID;
    unsigned ContentSigAlgo;
    unsigned ContentSigHashAlgo;
    struct {
        bool ContentEncAlgo;
        bool ContentEncKeyID;
        bool ContentSignature;
        bool ContentSigKeyID;
        bool ContentSigAlgo;
        bool ContentSigHashAlgo;
    } validate;
} bmkv_ContentEncryption;

typedef struct bmkv_ContentEncoding {
    unsigned ContentEncodingOrder;
    unsigned ContentEncodingScope;
    unsigned ContentEncodingType;
    bmkv_table ContentCompression;
    bmkv_table ContentEncryption;
    struct {
        bool ContentEncodingOrder;
        bool ContentEncodingScope;
        bool ContentEncodingType;
        bool ContentCompression;
        bool ContentEncryption;
    } validate;
} bmkv_ContentEncoding;

typedef struct bmkv_ContentEncodings {
    bmkv_table ContentEncoding;
    struct {
        bool ContentEncoding;
    } validate;
} bmkv_ContentEncodings;

typedef struct bmkv_AttachmentLink {
    uint64_t AttachmentLink;
     struct {
        bool AttachmentLink;
    } validate;
} bmkv_AttachmentLink;

typedef struct bmkv_TrackEntry {
    unsigned TrackNumber;
    uint64_t TrackUID;
    unsigned TrackType;
    bool FlagEnabled;
    bool FlagDefault;
    bool FlagForced;
    bool FlagLacing;
    unsigned MinCache;
    unsigned MaxCache;
    uint64_t DefaultDuration; 
    bmkv_float TrackTimecodeScale;
    signed TrackOffset;
    unsigned MaxBlockAdditionID;
    bmkv_utf8 Name;
    char Language[16];
    char CodecID[32];
    bmkv_data CodecPrivate;
    bmkv_utf8 CodecName;
    bmkv_table AttachmentLink;
    bmkv_utf8 CodecSettings;
    bmkv_string CodecInfoURL;
    bmkv_string CodecDownloadURL;
    bool CodecDecodeAll;
    unsigned TrackOverlay;
    uint64_t TrickTrackUID;
    uint64_t MasterTrackUID;
    bool TrickTrackFlag;
    bmkv_table TrackTranslate;
    bmkv_table Video;
    bmkv_table Audio;
    bmkv_table ContentEncodings;
    struct {
        bool TrackNumber;
        bool TrackUID;
        bool TrackType;
        bool FlagEnabled;
        bool FlagDefault;
        bool FlagForced;
        bool FlagLacing;
        bool MinCache;
        bool MaxCache;
        bool DefaultDuration; 
        bool TrackTimecodeScale;
        bool TrackOffset;
        bool MaxBlockAdditionID;
        bool Name;
        bool Language;
        bool CodecID;
        bool CodecPrivate;
        bool CodecName;
        bool AttachmentLink;
        bool CodecSettings;
        bool CodecInfoURL;
        bool CodecDownloadURL;
        bool CodecDecodeAll;
        bool TrackOverlay;
        bool TrickTrackUID;
        bool TrickTrackSegUID;
        bool MasterTrackUID;
        bool MasterTrackSegUID;
        bool TrickTrackFlag;
        bool TrackTranslate;
        bool Video;
        bool Audio;
        bool ContentEncodings;
    } validate;
} bmkv_TrackEntry;

typedef struct bmkv_TracksData {
    unsigned TracksDataVersion;
    unsigned TracksDataSize;
    bmkv_data TracksDataPayload;
    struct {
        bool TracksDataVersion;
        bool TracksDataSize;
        bool TracksDataPayload;
    } validate;
} bmkv_TracksData;

#define BMKV_TRACKS_ID  0x1654AE6B
typedef struct bmkv_Tracks {
    bmkv_table TrackEntry;
    bmkv_table TracksData;
    struct {
        bool TrackEntry;
        bool TracksData;
    } validate;
} bmkv_Tracks;

#define BMKV_CLUSTER_ID 0x1F43B675
#define BMKV_CUES_ID    0x1C53BB6B
#define BMKV_ATTACHMENT_ID 0x1941A469
#define BMKV_CHAPTERS_ID 0x1043A770



bool bmkv_EBML_parse(batom_cursor *cursor, size_t elem_size, bmkv_EBML *ebml);
bool bmkv_EBML_validate(const bmkv_EBML *ebml);
bool bmkv_SeekHead_parse(batom_cursor *cursor, size_t elem_size, bmkv_SeekHead *seek);
void bmkv_SeekHead_shutdown(bmkv_SeekHead *seek);
bool bmkv_SegmentInfo_parse(batom_cursor *cursor, size_t elem_size, bmkv_SegmentInformation *segment);
void bmkv_SegmentInfo_shutdown(bmkv_SegmentInformation *segment);
bool bmkv_Tracks_parse(batom_cursor *cursor, size_t elem_size, bmkv_Tracks *track);
void bmkv_Tracks_shutdown(bmkv_Tracks *track);
extern const char bmkv_matroska[9];

bool bmkv_IsTrackVideoAvc(const bmkv_TrackEntry *track);
bool bmkv_IsTrackVideoMpeg2(const bmkv_TrackEntry *track);
bool bmkv_IsTrackVideoVFW(const bmkv_TrackEntry *track);
bool bmkv_IsTrackVideoMpeg4Asp(const bmkv_TrackEntry *track);
bool bmkv_IsTrackAudioMkvAac(const bmkv_TrackEntry *track);
bool bmkv_IsTrackAudioAc3(const bmkv_TrackEntry *track);
bool bmkv_IsTrackAudioAc3Plus(const bmkv_TrackEntry *track);
bool bmkv_IsTrackAudioDts(const bmkv_TrackEntry *track);
bool bmkv_IsTrackAudioAac(const bmkv_TrackEntry *track, bmedia_info_aac *aac);
bool bmkv_IsTrackAudioMpeg1Layer3(const bmkv_TrackEntry *track);
bool bmkv_IsTrackAudioMpeg1Layer1_2(const bmkv_TrackEntry *track);
bool bmkv_IsTrackAuxiliary(const bmkv_TrackEntry *track);
bool bmkv_IsTrackAudioACM(const bmkv_TrackEntry *track);
bool bmkv_IsTrackAudioVorbis(const bmkv_TrackEntry *track);
bool bmkv_IsTrackVideoVp8(const bmkv_TrackEntry *track);


#ifdef __cplusplus
}
#endif


#endif /* _BMKV_UTIL_H__ */

