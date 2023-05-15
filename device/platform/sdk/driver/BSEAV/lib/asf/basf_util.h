/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: basf_util.h $
 * $brcm_Revision: 24 $
 * $brcm_Date: 9/28/12 12:10p $
 *
 * Module Description:
 *
 * ASF parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/asf/basf_util.h $
 * 
 * 24   9/28/12 12:10p vsilyaev
 * SW7435-384: Addes support for EAC3 audio in ASF container
 * 
 * 23   9/28/10 11:04a vsilyaev
 * SW7405-4904: Added code to recongnize 'PR' files
 * 
 * 22   7/22/10 4:59p vsilyaev
 * SWBLURAY-21061: Better detect DRM files
 * 
 * dev_bdvd_v3.0_issue21061/1   7/21/10 3:03p ialauder
 * Add support to probe for the Extended Content Encryption object.
 * Return Encryption version 1 if only Content Encryption is found and
 * version 2 if the Exteneded Content Encryption is found.
 * 
 * 21   6/28/10 3:03p vsilyaev
 * SWBLURAY-21061: Refactored code to allow separation bettwen no-DRM and
 * parsing error
 * 
 * 20   6/28/10 12:19p vsilyaev
 * SWBLURAY-21061: Added code to look/parse the Content Encryption Object.
 * 
 * fix_bdvd_v3.0_issue21061/1   6/22/10 8:31a ialauder
 * Add support to parse Content Encryption object for DRM
 * 
 * 19   4/23/10 6:54p vsilyaev
 * SW3548-2908: Skip over various objects when looking for the simple
 * index
 * 
 * 18   4/15/10 12:27p vsilyaev
 * SWBLURAY-20165: Added support to extract multichannel PCM audio
 * 
 * 17   12/18/09 12:58p vsilyaev
 * SW3556-946: Added fields from the ASF FileObject
 * 
 * 16   10/30/09 1:30p gmohile
 * SW7405-3330 : Add support to extract auxiliary stream
 * 
 * 14   5/15/09 11:22a vsilyaev
 * PR 55142: Fixed use of VC1/WMV codecs, separated VC1_SM/VC_AP and rest
 * of WMV
 * 
 * 13   5/15/09 10:56a vsilyaev
 * PR 55142: Added MP43 video support for ASF files
 * 
 * dev_bdvd_v1.5_add_mp4v3_support_to_asf/1   5/13/09 5:54p gooslin
 * Add MP4v3 support to ASF
 * 
 * 12   5/4/09 6:08p vsilyaev
 * PR 52434: Use common routines to parse Wave and Bitmap headers
 * 
 * 11   4/28/09 5:54p vsilyaev
 * PR 53911: Added support for processing of the ASF hidden tracks
 * 
 * 10   7/10/07 5:25p vsilyaev
 * PR 32813: Added function to parse file properties
 * 
 * 9   7/9/07 3:38p vsilyaev
 * PR 32846: Updated ASF code to work with batom/SPF framework
 * 
 * 8   8/4/06 12:46p vsilyaev
 * PR 22876: Added support for paced trickmodes to allow rate accurate
 * trick modes
 * 
 * 7   7/28/06 8:52p vsilyaev
 * PR 21336: Removed local definition of BDBG_CASSERT
 * 
 * 6   5/2/06 5:51p vsilyaev
 * PR 20683: Added support for the navigation in ASF files
 * 
 * 5   5/1/06 2:58p vsilyaev
 * PR 20683: Improved the asf_player support
 * 
 * 4   4/13/06 3:14p vsilyaev
 * PR 20308: Improved API description
 * 
 * 3   4/7/06 12:29p vsilyaev
 * PR 20683: added basf_player
 * 
 * 2   3/30/06 4:04p vsilyaev
 * PR 20308: Support for looped streams
 * 
 * 1   3/28/06 11:02a vsilyaev
 * PR 20308: ASF parser library
 * 
 *******************************************************************************/

#ifndef _BASF_UTIL_H__
#define _BASF_UTIL_H__

#include "bioatom.h"
#include "bmedia_util.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* type corresponding to the BYTE type in the ASF specification */
typedef uint8_t basf_byte;
/* type corresponding to the WCHAR type in the ASF specification */
typedef uint16_t basf_wchar;
/* type corresponding to the WORD type in the ASF specification */
typedef uint16_t basf_word;
/* type corresponding to the DWORD type in the ASF specification */
typedef uint32_t basf_dword;
/* type corresponding to the QWORD type in the ASF specification */
typedef uint64_t basf_qword;

/* type corresponding to the GUID type in the ASF specification */
typedef bmedia_guid basf_guid;

extern const basf_guid basf_stream_properties_guid;
extern const basf_guid basf_extended_stream_properties_guid;
extern const basf_guid basf_audio_media_guid;
extern const basf_guid basf_video_media_guid;
extern const basf_guid basf_command_media_guid;
extern const basf_guid basf_jfif_media_guid;
extern const basf_guid basf_degradable_jpeg_media_guid;
extern const basf_guid basf_file_transfer_media_guid;
extern const basf_guid basf_binary_media_guid;  
extern const basf_guid basf_header_guid;
extern const basf_guid basf_file_properties_guid;
extern const basf_guid basf_simple_index_guid;
extern const basf_guid basf_content_encryption_guid;
extern const basf_guid basf_extended_content_encryption_guid;
#if defined (HUMAX_PLATFORM_BASE)
extern const basf_guid basf_extended_content_encryption_object_guid;
extern const basf_guid basf_protection_system_identifier_object_guid;
extern const basf_guid basf_playready_header_object_guid;
#endif
typedef struct basf_video_stream_properties {
	basf_dword encodedImageWidth;
	basf_dword encodedImageHeight; 
	basf_word formatDataSize;
    bmedia_bitmapinfo bi;
} basf_video_stream_properties;

typedef bmedia_waveformatex basf_audio_stream_properties;

typedef struct basf_stream_properties {
	basf_guid streamType;
	basf_guid errorCorrectionType;
	basf_qword timeOffset;
	basf_dword typeSpecificLength;
	basf_dword errorCorrectionLength;
	uint8_t streamNumber;
	bool encryptedContentFlag;
} basf_stream_properties;

typedef struct basf_extended_stream_properties {
    basf_qword  startTime;
    basf_qword  endTime;
    basf_dword  dataBitrate;
    basf_dword  bufferSize;
    basf_dword  initialBufferFullness;
    basf_dword  alternateDataBitrate;
    basf_dword  alternateBufferSize;
    basf_dword  alternateInitialBufferFillness;
    basf_dword  maximumObjectSize;
    bool    reliableFlag;
    bool    seekableFlag;
    bool    noCleanpointsFlag;
    bool    resendLiveCleanpointsFlag;
    basf_word   streamNumber;
    basf_word   streamLanguageIDIndex;
    basf_qword  averageTimePerFrame;
    basf_word   streamNameCount;
    basf_word   payloadExtensionCount;
    bool    hasStreamPropertiesObject;
} basf_extended_stream_properties;

typedef struct basf_file_properties {
	basf_guid fileID;
	basf_qword fileSize;
	basf_qword creationDate;
	basf_qword dataPacketsCount;
	basf_qword playDuration;
	basf_qword sendDuration;
	basf_qword preroll;
	bool broadcastFlag;
	bool seekableFlag;
    bool liveFlag;
	basf_dword minimumDataPacketSize;
	basf_dword maximumDataPacketSize;
	basf_dword maximumBitrate;
} basf_file_properties; 

/* this function compares to GUID's, it returns 0 if guid1==guid2, negative valud if guid1<guid2 and positive value if guid1>guid2 */
int basf_compare_guid(const basf_guid *guid1, const basf_guid *guid2);

/* this function reads ASF GUID from the byte stream, return value is unspecified if end of stream was reached */
void basf_read_guid(batom_cursor *c, basf_guid *guid);

#define B_ASF_VIDEO_CODEC(c1,c2,c3,c4) (((uint32_t)c1) | ((uint32_t)c2<<8) | ((uint32_t)c3<<16) | ((uint32_t)c4<<24))

#define BASF_VIDEO_HEADER_LENGTH    40
/* this function reads basf_video_stream_properties, it stops reading data after common secttion */
bool basf_read_video_stream_properties(batom_cursor *cursor, basf_video_stream_properties *video, bool *pr);

/* this function reads basf_audio_stream_properties, it stops reading data after common secttion */
bool basf_read_audio_stream_properties(batom_cursor *cursor, basf_audio_stream_properties *audio, bool *pr);

/* this function reads basf_stream_properties */
bool basf_read_stream_properties(batom_cursor *cursor, basf_stream_properties *stream);

typedef struct basf_content_encryption_properties {
    bool drm;
} basf_content_encryption_properties;

bool basf_read_content_encryption_properties(batom_cursor *cursor, basf_content_encryption_properties *properties);

#define BASF_STREAM_VIDEO(stream) (basf_compare_guid(&(stream)->streamType, &basf_video_media_guid)==0)
#define BASF_STREAM_AUDIO(stream) (basf_compare_guid(&(stream)->streamType, &basf_audio_media_guid)==0)
#define BASF_STREAM_AUX(stream)   (basf_compare_guid(&(stream)->streamType, &basf_command_media_guid)==0 || \
				   basf_compare_guid(&(stream)->streamType, &basf_jfif_media_guid)==0 || \
				   basf_compare_guid(&(stream)->streamType, &basf_degradable_jpeg_media_guid)==0 || \
				   basf_compare_guid(&(stream)->streamType, &basf_file_transfer_media_guid)==0 || \
				   basf_compare_guid(&(stream)->streamType, &basf_binary_media_guid)==0)


/* this function reads basf_extended_stream_properties, it stops reading prior to reaching the basf_stream_properties */
bool basf_read_extended_stream_properties(batom_cursor *cursor, basf_extended_stream_properties *stream);

/* this function reads basf_file_properties  */
bool basf_read_file_properties(batom_cursor *cursor, basf_file_properties *file);

/* this function reads ASF QWORD from the byte stream, return value is unspecified if end of stream was reached  */
#define basf_read_qword(c) batom_cursor_uint64_le(c)

/* this function reads ASF DWORD from the byte stream, return value is unspecified if end of stream was reached  */
#define basf_read_dword(c) batom_cursor_uint32_le(c)

/* this function reads ASF WORD from the byte stream, return value is unspecified if end of stream was reached  */
#define basf_read_word(c) batom_cursor_uint16_le(c)

/* this function reads ASF BYTE from the byte stream, return value is unspecified if end of stream was reached  */
#define basf_read_byte(c) batom_cursor_byte(c)

/* this function reads ASF variable length word from the byte stream, return value is unspecified if end of stream was reached  */
basf_dword basf_read_vword(batom_cursor *c, unsigned type);
const char* basf_guid2str(const basf_guid *guid, char *str, size_t size);

bool basf_waveformatex_is_eac3(const bmedia_waveformatex *wf);


#ifdef __cplusplus
}
#endif

#endif /* _BASF_UTIL_H_a _*/
