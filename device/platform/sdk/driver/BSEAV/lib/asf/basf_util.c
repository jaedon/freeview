/***************************************************************************
 *     Copyright (c) 2006-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: basf_util.c $
 * $brcm_Revision: 24 $
 * $brcm_Date: 9/28/12 12:10p $
 *
 * Module Description:
 *
 * ASF parser library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/asf/basf_util.c $
 * 
 * 24   9/28/12 12:10p vsilyaev
 * SW7435-384: Addes support for EAC3 audio in ASF container
 * 
 * 23   6/14/11 12:03p vsilyaev
 * SW7405-5368: Use original audio type for PR streams
 * 
 * 22   9/28/10 11:04a vsilyaev
 * SW7405-4904: Added code to recongnize 'PR' files
 * 
 * 21   7/22/10 4:59p vsilyaev
 * SWBLURAY-21061: Better detect DRM files
 * 
 * dev_bdvd_v3.0_issue21061/1   7/21/10 3:03p ialauder
 * Add support to probe for the Extended Content Encryption object.
 * Return Encryption version 1 if only Content Encryption is found and
 * version 2 if the Exteneded Content Encryption is found.
 * 
 * 20   6/28/10 3:03p vsilyaev
 * SWBLURAY-21061: Refactored code to allow separation bettwen no-DRM and
 * parsing error
 * 
 * 19   6/28/10 12:19p vsilyaev
 * SWBLURAY-21061: Added code to look/parse the Content Encryption Object.
 * 
 * fix_bdvd_v3.0_issue21061/1   6/22/10 8:31a ialauder
 * Add support to parse Content Encryption object for DRM
 * 
 * 18   4/23/10 6:54p vsilyaev
 * SW3548-2908: Skip over various objects when looking for the simple
 * index
 * 
 * 17   4/15/10 12:27p vsilyaev
 * SWBLURAY-20165: Added support to extract multichannel PCM audio
 * 
 * 16   12/18/09 12:58p vsilyaev
 * SW3556-946: Added fields from the ASF FileObject
 * 
 * 15   10/30/09 1:30p gmohile
 * SW7405-3330 : Add support to extract auxiliary stream
 * 
 * 14   6/10/09 12:54p vsilyaev
 * PR 55896: After merge minor fixes
 * 
 * 13   5/4/09 6:08p vsilyaev
 * PR 52434: Use common routines to parse Wave and Bitmap headers
 * 
 * 12   4/28/09 5:54p vsilyaev
 * PR 53911: Added support for processing of the ASF hidden tracks
 * 
 * 11   3/6/09 12:09p vsilyaev
 * PR 52903: Refactored basf_read_vword function
 * 
 * 10   7/23/07 5:51p vsilyaev
 * PR 32813: Fixed comments and unnecessary code
 * 
 * 9   7/17/07 4:30p vsilyaev
 * PR 32813: Added MPEG-2 TS support for the probe library
 * 
 * 8   7/10/07 5:25p vsilyaev
 * PR 32813: Added function to parse file properties
 * 
 * 7   7/9/07 3:37p vsilyaev
 * PR 32846: Updated ASF code to work with batom/SPF framework
 * 
 * 6   8/4/06 12:46p vsilyaev
 * PR 22876: Added support for paced trickmodes to allow rate accurate
 * trick modes
 * 
 * 5   5/1/06 2:58p vsilyaev
 * PR 20683: Improved the asf_player support
 * 
 * 4   4/12/06 10:33a vsilyaev
 * PR 20308: Improved parsing of compressed payload
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
#include "bstd.h"
#include "basf_util.h"
#include "bkni.h"
#include "biobits.h"

BDBG_MODULE(basf_util);

#define BASF_AUDIO_PR(audio) ((audio)->wFormatTag==0x5052)

#define BASF_VIDEO_PR(x)    ((x)->bi.biCompression==BMEDIA_FOURCC('P','R','D','Y'))

int
basf_compare_guid(const basf_guid *g1, const basf_guid *g2)
{
    return bmedia_compare_guid(g1, g2);
}

void 
basf_read_guid(batom_cursor *c, basf_guid *guid)
{
    bmedia_read_guid(c, guid);
}



basf_dword
basf_read_vword(batom_cursor *c, unsigned type)
{
    if(type==0) { return 0;}
    if(type<=3) {
#if B_ATOM_FAST	
        if (c->left>=4) {
            basf_dword d = c->cursor[0];
            if(type>1) {
                d |= ((basf_word)(c->cursor[1])<<8);  /* type == 2 */
                if(type==3) { 
                    d |= ((basf_dword)(c->cursor[2])<<16) | ((basf_dword)(c->cursor[3])<<24); /* type == 3*/
                    type=4;
                } 
                c->cursor += type;
                c->left -= type;
                return d;
            }
        }
#endif /* B_ATOM_FAST */    
        if(type==3) {
            type++;
        } 
        return batom_cursor_vword_le(c, type);
    } else {
		BDBG_ASSERT(0);
        return 0;
    }
}


const char*
basf_guid2str(const basf_guid *guid, char *str, size_t size)
{
    return bmedia_guid2str(guid, str, size);
}

const basf_guid
  basf_stream_properties_guid = 
	{{0xB7, 0xDC, 0x07, 0x91, 0xA9, 0xB7, 0x11, 0xCF, 0x8E, 0xE6, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65}};

const basf_guid
  basf_extended_stream_properties_guid = 
	{{0x14, 0xE6, 0xA5, 0xCB, 0xC6, 0x72, 0x43, 0x32, 0x83, 0x99, 0xA9, 0x69, 0x52, 0x06, 0x5B, 0x5A}};

const basf_guid
  basf_header_guid = 
	{{0x75, 0xB2, 0x26, 0x30, 0x66, 0x8E, 0x11, 0xCF, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C}};

const basf_guid
  basf_audio_media_guid = 
    {{0xF8, 0x69, 0x9E, 0x40, 0x5B, 0x4D, 0x11, 0xCF, 0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B}},
  basf_video_media_guid = 
    {{0xBC, 0x19, 0xEF, 0xC0, 0x5B, 0x4D, 0x11, 0xCF, 0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B}},
  basf_command_media_guid =
    {{0x59, 0xDA, 0xCF, 0xC0, 0x59, 0xE6, 0x11, 0xD0, 0xA3, 0xAC, 0x00, 0xA0, 0xC9, 0x03, 0x48, 0xF6}},
  basf_jfif_media_guid =
    {{0xB6, 0x1B, 0xE1, 0x00, 0x5B, 0x4E, 0x11, 0xCF, 0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B}},
  basf_degradable_jpeg_media_guid =
    {{0x35, 0x90, 0x7D, 0xE0, 0xE4, 0x15, 0x11, 0xCF, 0xA9, 0x17, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B}},
  basf_file_transfer_media_guid =
    {{0x91, 0xBD, 0x22, 0x2C, 0xF2, 0x1C, 0x49, 0x7A, 0x8B, 0x6D, 0x5A, 0xA8, 0x6B, 0xFC, 0x01, 0x85}},
  basf_binary_media_guid =
    {{0x3A, 0xFB, 0x65, 0xE2, 0x47, 0xEF, 0x40, 0xF2, 0xAC, 0x2C, 0x70, 0xA9, 0x0D, 0x71, 0xD3, 0x43}};

const basf_guid
  basf_file_properties_guid = 
	{{0x8C, 0xAB, 0xDC, 0xA1, 0xA9, 0x47, 0x11, 0xCF, 0x8E, 0xE4, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65}};

const basf_guid
  basf_simple_index_guid =
    {{0x33, 0x00, 0x08, 0x90, 0xE5, 0xB1, 0x11, 0xCF, 0x89, 0xF4, 0x00, 0xA0, 0xC9, 0x03, 0x49, 0xCB}};

const basf_guid
     basf_content_encryption_guid =
    {{0x22, 0x11, 0xB3, 0xFB, 0xBD, 0x23, 0x11, 0xD2, 0xB4, 0xB7, 0x00, 0xA0, 0xC9, 0x55, 0xFC, 0x6E}};

const basf_guid
     basf_extended_content_encryption_guid =
    {{0x22, 0x11, 0xB3, 0xFC, 0xBD, 0x23, 0x11, 0xD2, 0xB4, 0xB7, 0x00, 0xA0, 0xC9, 0x55, 0xFC, 0x6E}};

#if defined (HUMAX_PLATFORM_BASE)
const basf_guid
	basf_extended_content_encryption_object_guid = 
	{{0x29, 0x8A, 0xE6, 0x14, 0x26, 0x22, 0x4C, 0x17, 0xB9, 0x35, 0xDA, 0xE0, 0x7E, 0xE9, 0x28, 0x9C}};

const basf_guid
	basf_protection_system_identifier_object_guid = 
	{{0x9A, 0x04, 0xF0, 0x79, 0x98, 0x40, 0x42, 0x86, 0xAB, 0x92, 0xE6, 0x5B, 0xE0, 0x88, 0x5F, 0x95}};

const basf_guid
	basf_playready_header_object_guid = 
	{{0xF4, 0x63, 0x70, 0x10, 0x03, 0xC3, 0x42, 0xCD, 0xB9, 0x32, 0xB4, 0x8A, 0xDF, 0x3A, 0x6A, 0x54}};
#endif

bool 
basf_read_video_stream_properties(batom_cursor *cursor, basf_video_stream_properties *video, bool *pr)
{
    BDBG_ASSERT(cursor);
    BDBG_ASSERT(video);

    *pr = false;
    video->encodedImageWidth = basf_read_dword(cursor);
    video->encodedImageHeight = basf_read_dword(cursor);
    batom_cursor_skip(cursor, /* ReservedFlags */ sizeof(basf_byte));
    video->formatDataSize = basf_read_word(cursor); 

    if(!bmedia_read_bitmapinfo(&video->bi, cursor)) {
        return false;
    }

    BDBG_MSG(("basf_read_video_stream_properties: %ux%u  (%ux%u) codec:%c%c%c%c(%s)", (unsigned)video->bi.biWidth, (unsigned)video->bi.biHeight, (unsigned)video->encodedImageWidth, (unsigned)video->encodedImageHeight, video->bi.biCompression&0xFF, (video->bi.biCompression>>8)&0xFF, (video->bi.biCompression>>16)&0xFF, (video->bi.biCompression>>24)&0xFF, BMEDIA_FOURCC_VC1SM_CODEC(video->bi.biCompression)?"VC1 S&M":BMEDIA_FOURCC_VC1AP_CODEC(video->bi.biCompression)?"VC1 AP":"other"));

    if(BASF_VIDEO_PR(video) && video->bi.biSize>(BASF_VIDEO_HEADER_LENGTH+4)) {
        batom_cursor data;
        BATOM_CLONE(&data, cursor);
        *pr = true;
        video->bi.biSize -=4;
        batom_cursor_skip(&data, video->bi.biSize-BASF_VIDEO_HEADER_LENGTH);
        video->bi.biCompression = batom_cursor_uint32_le(&data);
        if(BATOM_IS_EOF(&data)) {
            return false;
        }
    }
    return true;
}

#define WRITE_16_LE(ptr, data) do { (ptr)[0] = B_GET_BITS(data,7,0);(ptr)[1] = B_GET_BITS(data,15,8);} while(0)

/* this function reads basf_video_stream_properties, it stops reading data after common secttion */
bool 
basf_read_audio_stream_properties(batom_cursor *cursor, basf_audio_stream_properties *audio, bool *pr)
{
    bool result;

    *pr = false;
    result = bmedia_read_waveformatex(audio, cursor);
    if(!result || !BASF_AUDIO_PR(audio)) {
        return result;
    }
    *pr = true;
    if(audio->meta_length>2 && audio->cbSize>2) {
        audio->meta_length -=2;
        audio->cbSize -=2;
        audio->wFormatTag = (((uint16_t)audio->meta[audio->meta_length+1])<<8) + audio->meta[audio->meta_length];
        WRITE_16_LE(audio->meta, audio->wFormatTag); /* write back actual stream type */
    }

    return result;
}

bool 
basf_read_stream_properties(batom_cursor *cursor, basf_stream_properties *stream)
{
	basf_dword flags;

	BDBG_ASSERT(cursor);
	BDBG_ASSERT(stream);

	basf_read_guid(cursor, &stream->streamType);
	basf_read_guid(cursor, &stream->errorCorrectionType);
	stream->timeOffset = basf_read_qword(cursor);
	stream->typeSpecificLength = basf_read_dword(cursor);
	stream->errorCorrectionLength = basf_read_dword(cursor);
	flags = basf_read_word(cursor);
	stream->streamNumber = B_GET_BITS(flags,6,0);
	stream->encryptedContentFlag = B_GET_BIT(flags,15);
	batom_cursor_skip(cursor, sizeof(basf_dword));
	if(BATOM_IS_EOF(cursor)) {
		return false;
	}

	BDBG_MSG(("basf_read_stream_properties: streamNumber:%u %s", stream->streamNumber, BASF_STREAM_VIDEO(stream)?"video":BASF_STREAM_AUDIO(stream)?"audio":"unknown"));
	return true;
}

bool 
basf_read_content_encryption_properties(batom_cursor *cursor, basf_content_encryption_properties *properties)
{
    basf_dword temp;
    basf_dword protection_type_length;
    basf_dword protection_type;    

    BDBG_ASSERT(cursor);

    properties->drm = false;

    temp = basf_read_dword(cursor); /* secret data length */
    if(BATOM_IS_EOF(cursor)) {
        return false;
    }

    batom_cursor_skip(cursor,temp); /* secret data */
    protection_type_length = basf_read_dword(cursor); /* protection type length */
    if(BATOM_IS_EOF(cursor)) {
        return false;
    }
    
    /* According to the ASF spec, in section '3.14	Content Encryption Object',
       for DRM the protection type should be 'DRM\0', which would be 4 bytes */
    if (protection_type_length == 0x04)
    {
        protection_type = basf_read_dword(cursor); 
        
        /* Protection Type should be 'DRM\0' - ASCII values of 0x44 0x52 0x4D 0x00 */
        if ((B_GET_BITS(protection_type,7,0) == 0x44) && (B_GET_BITS(protection_type,15,8) == 0x52) &&
            (B_GET_BITS(protection_type,23,16) == 0x4D) && (B_GET_BITS(protection_type,31,24) == 0))
        {
            properties->drm = true;
        }
    }
    return true;
}

bool 
basf_read_file_properties(batom_cursor *cursor, basf_file_properties *file)
{
	basf_dword flags;

	BDBG_ASSERT(cursor);
	BDBG_ASSERT(file);

	basf_read_guid(cursor, &file->fileID);
	file->fileSize = basf_read_qword(cursor);
	file->creationDate = basf_read_qword(cursor);
	file->dataPacketsCount = basf_read_qword(cursor);
	file->playDuration = basf_read_qword(cursor);
	file->sendDuration = basf_read_qword(cursor);
	file->preroll = basf_read_qword(cursor);
	flags = basf_read_dword(cursor);
	file->broadcastFlag = B_GET_BIT(flags,0);
	file->seekableFlag = B_GET_BIT(flags,1);
	file->liveFlag = B_GET_BIT(flags,3);
	file->minimumDataPacketSize = basf_read_dword(cursor);
	file->maximumDataPacketSize = basf_read_dword(cursor);
	file->maximumBitrate = basf_read_dword(cursor);
	if(BATOM_IS_EOF(cursor)) {
		return false;
	}
#if BDBG_DEBUG_BUILD
	{
		char guid_buf[40];
		BDBG_MSG(("basf_read_file_properties: '%s' size:%luKB packets:%u play:%usec preroll:%ums %s %s pkt_size:%u-%u %uKbps", basf_guid2str(&file->fileID, guid_buf, sizeof(guid_buf)), (unsigned long)(file->fileSize/1024), (unsigned)file->dataPacketsCount, (unsigned)(file->playDuration/(10*1000*1000)), (unsigned)file->preroll, file->broadcastFlag?"Broadcast":"", file->seekableFlag?"Seekable":"", (unsigned)file->minimumDataPacketSize, (unsigned)file->maximumDataPacketSize, (unsigned)file->maximumBitrate/1024));
	}
#endif
	return true;
}

bool 
basf_read_extended_stream_properties(batom_cursor *cursor, basf_extended_stream_properties *stream)
{
    unsigned i;
	basf_dword flags;
    basf_guid stream_properties_guid;
    basf_qword stream_properties_len;

	BDBG_ASSERT(cursor);
	BDBG_ASSERT(stream);

    stream->startTime = basf_read_qword(cursor);
    stream->endTime = basf_read_qword(cursor);
    stream->dataBitrate = basf_read_dword(cursor);
    stream->bufferSize = basf_read_dword(cursor);
    stream->initialBufferFullness = basf_read_dword(cursor);
    stream->alternateDataBitrate = basf_read_dword(cursor);
    stream->alternateBufferSize = basf_read_dword(cursor);
    stream->alternateInitialBufferFillness = basf_read_dword(cursor);
    stream->maximumObjectSize = basf_read_dword(cursor);
    flags = basf_read_dword(cursor);
    stream->reliableFlag = B_GET_BIT(flags,0);
    stream->seekableFlag = B_GET_BIT(flags,1);
    stream->noCleanpointsFlag = B_GET_BIT(flags,2);
    stream->resendLiveCleanpointsFlag = B_GET_BIT(flags,3);
    stream->streamNumber = basf_read_word(cursor);
    stream->streamLanguageIDIndex = basf_read_word(cursor);
    stream->averageTimePerFrame = basf_read_qword(cursor);
    stream->streamNameCount = basf_read_word(cursor);
    stream->payloadExtensionCount = basf_read_word(cursor);

	if(BATOM_IS_EOF(cursor)) {
		return false;
	}
    BDBG_MSG(("%s: startTime:%u endTime:%u dataBitrate:%u bufferSize:%u initialBufferFullness:%u alternateDataBitrate:%u alternateBufferSize:%u", "basf_read_extended_stream_properties", (unsigned)stream->startTime, (unsigned)stream->endTime, (unsigned)stream->dataBitrate, (unsigned)stream->bufferSize, (unsigned)stream->initialBufferFullness, (unsigned)stream->alternateDataBitrate, (unsigned)stream->alternateBufferSize));
    BDBG_MSG(("%s: alternateInitialBufferFillness:%u maximumObjectSize:%u %s %s %s %s streamNumber:%u streamLanguageIDIndex:%u averageTimePerFrame:%u streamNameCount:%u payloadExtensionCount:%u", "basf_read_extended_stream_properties", (unsigned)stream->alternateInitialBufferFillness, (unsigned)stream->maximumObjectSize, stream->reliableFlag?"reliableFlag":"", stream->seekableFlag?"seekableFlag":"", stream->noCleanpointsFlag?"noCleanpointsFlag":"", stream->resendLiveCleanpointsFlag?"resendLiveCleanpointsFlag":"", (unsigned)stream->streamNumber, (unsigned)stream->streamLanguageIDIndex, (unsigned)stream->averageTimePerFrame, (unsigned)stream->streamNameCount, (unsigned)stream->payloadExtensionCount));

    for(i=0;i<stream->streamNameCount;i++) {
        basf_word length;
        basf_read_word(cursor);          /* Language ID Index */ 
        length = basf_read_word(cursor); /* Stream Name Length */
    	if(BATOM_IS_EOF(cursor)) {
		    return false;
	    }
        /* Strean Name */ batom_cursor_skip(cursor, 2*length);
    }
    for(i=0;i<stream->payloadExtensionCount;i++) {
        basf_dword length;
        batom_cursor_skip(cursor,
                sizeof(basf_guid) +         /* Extension System ID */
                sizeof(basf_word));         /* Extension Data Size */
        length = basf_read_dword(cursor);   /* Extension System Info Length */
    	if(BATOM_IS_EOF(cursor)) {
		    return false;
	    }
        batom_cursor_skip(cursor, length); /* Extension System Info */
    }
    basf_read_guid(cursor, &stream_properties_guid);
    stream_properties_len = basf_read_qword(cursor);
    stream->hasStreamPropertiesObject = !BATOM_IS_EOF(cursor) && basf_compare_guid(&stream_properties_guid, &basf_stream_properties_guid)==0 && stream_properties_len > sizeof(basf_guid)+sizeof(basf_qword);
   
    BDBG_MSG(("%s: %s StreamPropertiesObject", "basf_read_extended_stream_properties", stream->hasStreamPropertiesObject?"has":"no"));
    return true;
}

bool 
basf_waveformatex_is_eac3(const bmedia_waveformatex *wf)
{
    bmedia_waveformatextensible wfe;
    if(bmedia_read_waveformatextensible(wf, &wfe)) {
        static const bmedia_guid subtypePcm = {{0xa7,0xfb,0x87,0xaf,/*-*/0x2d,0x02,/*-*/0x42,0xfb,/*-*/0xa4,0xd4,0x05,0xcd,0x93,0x84,0x3b,0xdd}};
        if(bmedia_compare_guid(&wfe.subformat, &subtypePcm)==0) {
            return true;
        }
    }
    return false;
}


