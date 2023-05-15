/***************************************************************************
 *     Copyright (c) 2007-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg4_util.c $
 * $brcm_Revision: 8 $
 * $brcm_Date: 12/1/09 3:13p $
 *
 * Module Description:
 *
 * MPEG4 stream parsing 
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg4_util.c $
 * 
 * 8   12/1/09 3:13p vsilyaev
 * SW7405-3513: Merged changes to probe SBR information in the AAC streams
 * 
 * fix_bdvd_v3.0_issue17634/2   11/30/09 1:41p ialauder
 * AAC-HE support
 * 
 * fix_bdvd_v3.0_issue17634/1   11/25/09 4:23p ialauder
 * Changes to detect AAC-HE
 * 
 * 7   3/4/09 3:31p vsilyaev
 * PR 52434: MPEG4 parsing functions should be compiled with bmedia_player
 * 
 * 6   3/2/09 3:05p vsilyaev
 * PR 52434: Post merge updates/unification of parsing video data
 * 
 * 5   3/2/09 10:27a vsilyaev
 * PR 52434: Merge XViD and VC1 AP support in MKV
 * 
 * fix_bdvd_v1.42_pr12986/2   2/27/09 4:53p gooslin
 * Merge to the latest bmedia code
 * 
 * 4   2/10/09 3:22p gmohile
 * PR 51975 : support AAC Audio ISO IEC 13818-7
 * 
 * fix_bdvd_v1.42_pr12986/1   2/20/09 5:51p gooslin
 * Add mpeg4 frame detection
 * 
 * 3   10/16/08 6:32p vsilyaev
 * PR 47995: Added control over ID's used for media->PES conversion
 * 
 * 2   5/30/07 5:18p vsilyaev
 * PR 31697: Added support for MPEG4Part2 video
 * 
 * 1   5/9/07 3:04p vsilyaev
 * PR 28631: MPEG4 system parser
 * 
 * 
 *******************************************************************************/
#include "bstd.h"
#include "bmpeg4_util.h"
#include "biobits.h"

BDBG_MODULE(bmpeg4_util);

#define BDBG_MSG_TRACE(x)	/* BDBG_MSG(x) */

#define BMPEG4_ES_DescrTag	0x03
#define BMPEG4_DecoderConfigDescrTag	0x04
#define BMPEG4_DecSpecificInfoTag	0x05

static size_t
b_mp4_get_expandable_size(batom_cursor *cursor)
{
	/* ISO/IEC 14496-1 MPEG4 Part-1, page 97 */
	int byte;
	uint8_t nextByte;
	size_t sizeOfInstance;

	for(sizeOfInstance=0;;) {
		byte = batom_cursor_next(cursor);
		if (byte==BATOM_EOF) {
			return 0;
		}
		sizeOfInstance =  (sizeOfInstance<<7) | B_GET_BITS(byte, 6, 0);
		nextByte = B_GET_BIT(byte,7);
		if(!nextByte) {
			break;
		}
	}
	return sizeOfInstance;
}

static size_t
b_mp4_mpeg4_read_class(batom_cursor *cursor, batom_cursor *class, uint8_t tag)
{
	size_t size;
	uint8_t class_tag = batom_cursor_next(cursor);

	BDBG_MSG_TRACE(("b_mp4_mpeg4_read_class: tag %#x expected %#x", class_tag, tag));
	if(class_tag!=tag) {
		return 0;
	}
	BATOM_CLONE(class,cursor);
	size = b_mp4_get_expandable_size(class);
	if(size==0) {
		return 0;
	}
	if (batom_cursor_skip(cursor, size) != size) {
		return 0;
	}
	return size;
}


bool
bmpeg4_parse_es_descriptor(batom_cursor *cursor, bmpeg4_es_descriptor *descriptor)
{
	batom_cursor es_cursor;
	batom_cursor config_cursor;
	batom_cursor dec_cursor;
	int byte;
	size_t size;
    bmedia_info_aac info_aac;

	/* 7.2.6.5 ES_Descriptor, ISO/IEC 14496-1 MPEG4 Part-1, page 33 */
	if(!b_mp4_mpeg4_read_class(cursor, &es_cursor, BMPEG4_ES_DescrTag)) {
		return false;
	}
	descriptor->ES_ID = batom_cursor_uint16_be(&es_cursor);
	byte = batom_cursor_next(&es_cursor);
	if(byte==BATOM_EOF) {
		return false;
	}
	if(B_GET_BIT(byte, 7)) { /* streamDependenceFlag) */
		batom_cursor_skip(&es_cursor, sizeof(uint16_t) /* dependsOn_ES_ID */ );
	}
	if(B_GET_BIT(byte, 6)) { /* URL_Flag */
		int URLlength;
		URLlength = batom_cursor_next(&es_cursor);
		if(URLlength==BATOM_EOF) {
			return false;
		}
		batom_cursor_skip(&es_cursor, URLlength /* URLstring */);
	}
	if(B_GET_BIT(byte, 5)) { /* OCRstreamFlag) */ 
		batom_cursor_skip(&es_cursor, sizeof(uint16_t) /* OCR_ES_Id */ );
	}
	/* 7.2.6.6 DecoderConfigDescriptor, ISO/IEC 14496-1 MPEG4 Part-1, page 35*/
	if(!b_mp4_mpeg4_read_class(&es_cursor, &config_cursor, BMPEG4_DecoderConfigDescrTag)) {
		return false;
	}
	descriptor->objectTypeIndication = batom_cursor_byte(&config_cursor);
	if(descriptor->objectTypeIndication==0x00) {
		return false;
	}
	byte = batom_cursor_next(&config_cursor);
	if(byte==BATOM_EOF) {
		return false;
	}
	descriptor->streamType = B_GET_BITS(byte,7,2);
	batom_cursor_skip(&config_cursor, 3 /* bufferSizeDB */ + sizeof(uint32_t) /* maxBitrate */ + sizeof(uint32_t) /* avgBitrate */);
	switch(descriptor->objectTypeIndication) {
	case BMPEG4_Audio_ISO_IEC_14496_3:
	case BMPEG4_Audio_ISO_IEC_13818_7:
		if(!b_mp4_mpeg4_read_class(&config_cursor, &dec_cursor, BMPEG4_DecSpecificInfoTag)) {
			return false;
		}

        if(!bmedia_info_probe_aac_info(&dec_cursor, &info_aac)) {
            return false;
        }

        descriptor->decoder.iso_14496_3.audioObjectType = info_aac.profile;
        descriptor->decoder.iso_14496_3.channelConfiguration = info_aac.channel_configuration;
        descriptor->decoder.iso_14496_3.samplingFrequencyIndex = info_aac.sampling_frequency_index; 
		break;
	case BMPEG4_Video_ISO_IEC_14496_2:
		size = b_mp4_mpeg4_read_class(&config_cursor, &dec_cursor, BMPEG4_DecSpecificInfoTag);
		if(size==0) {
			return false;
		}
		if(size>sizeof(descriptor->decoder.iso_14496_2.header)) {
			BDBG_WRN(("bmpeg4_parse_es_descriptor: too large ISO IEC 14496.2 decoder information %u:%u", size, sizeof(descriptor->decoder.iso_14496_2.header)));
			return false;
		}
		descriptor->decoder.iso_14496_2.header_size = batom_cursor_copy(&dec_cursor, descriptor->decoder.iso_14496_2.header, size);
		break;
	}
	return true;
}

/* 1.6.2.1 AudoSpecificConfig, ISO/IEC 14496-3 {ed 3.0}, page 36*/
bool 
bmedia_info_probe_aac_info(batom_cursor *cursor, bmedia_info_aac *aac)
{
    int temp;
    int byte;
    bool delay_flag;
    bool ext_flag;
    bool fl_flag;
    bool sbr;
    batom_bitstream bs;        
    batom_checkpoint check_point;

    batom_cursor_save(cursor, &check_point);
    
    batom_bitstream_init(&bs, cursor);
    aac->profile = batom_bitstream_bits(&bs,5);
    aac->sampling_frequency_index = batom_bitstream_bits(&bs,4);

    if (aac->sampling_frequency_index == 0x0F)
    {
        BDBG_WRN(("bmedia_info_probe_aac_info: AudioSpecificConfig not supported samplingFrequencyIndex %#x, try basic parse", aac->sampling_frequency_index));
        goto basic_parse;
    }
    aac->channel_configuration = batom_bitstream_bits(&bs,4);
    
    
    if (aac->profile == 5)
    {
        aac->sampling_frequency_index = batom_bitstream_bits(&bs,4);

        if (aac->sampling_frequency_index == 0x0F)
        {
            BDBG_WRN(("bmedia_info_probe_aac_info: AudioSpecificConfig not supported samplingFrequencyIndex %#x, try basic parse", aac->sampling_frequency_index));
            goto basic_parse;
        }
        batom_bitstream_drop_bits(&bs, 5);
    }
    
    switch(aac->profile)
    {  
        case 1:
        case 2:
        case 3:
        case 4:
        case 6:
        case 7:
        case 17:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
            fl_flag = batom_bitstream_bit(&bs);
            delay_flag = batom_bitstream_bit(&bs);
            if(delay_flag) 
            {                    
                /* Delay is 14 bits */
                batom_bitstream_drop_bits(&bs, 14);
            }
            
            ext_flag = batom_bitstream_bit(&bs);
            
            if (aac->profile == 6 ||
                aac->profile == 20)
            {
                batom_bitstream_drop_bits(&bs, 13);
            }
            
            if (ext_flag)
            {
                if (aac->profile == 22)
                {
                    batom_bitstream_drop_bits(&bs, 16);
                }
                else if (aac->profile == 17 ||
                    aac->profile == 19 ||
                    aac->profile == 20 ||
                    aac->profile == 23)
                {
                    batom_bitstream_drop_bits(&bs, 3);
                }
                
                ext_flag = batom_bitstream_bit(&bs);
            }
            break;

        default:                
            break;
    }

    switch (aac->profile) {
        case 17:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
            temp = batom_bitstream_bits(&bs,2);
            if (temp == 2 || temp == 3)
            {
                batom_bitstream_drop(&bs);
            }
            break;

        default:
            break;
    }

    if (aac->profile != 5)
    {
        temp = batom_bitstream_bits(&bs,11);
        if (temp == 0x2b7)
        {
            temp = batom_bitstream_bits(&bs,5);
            if (temp == 0x5)
            { 
                sbr = batom_bitstream_bit(&bs);
                if (sbr)
                {
                    aac->profile  = temp;
                    aac->sampling_frequency_index = batom_bitstream_bits(&bs,4);

                    if (aac->sampling_frequency_index == 0x0F)
                    {
                        BDBG_WRN(("bmedia_info_probe_aac_info: AudioSpecificConfig not supported samplingFrequencyIndex %#x, try basic parse", aac->sampling_frequency_index));
                        goto basic_parse;
                    }
                }
            }
        }
    }

    return true;

basic_parse:
    /* Table 1.8 . Syntax of AudioSpecificConfig , ISO/IEC 14496-3 MPEG4 Part-3, page 3 */
    batom_cursor_rollback(cursor,&check_point);
    byte = batom_cursor_uint16_be(cursor);
    if(byte==BATOM_EOF) {
        return false;
    }
    aac->profile = B_GET_BITS(byte, 15, 11);
    aac->sampling_frequency_index = B_GET_BITS(byte, 10, 7);
    if(aac->sampling_frequency_index ==0x0F) {
        BDBG_WRN(("bmedia_info_probe_aac_info: AudioSpecificConfig not supported samplingFrequencyIndex %#x", aac->sampling_frequency_index));
        return false;
    }
    aac->channel_configuration = B_GET_BITS(byte, 6, 3);
    
    return true;
}
