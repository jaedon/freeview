/***************************************************************************
 *     Copyright (c) 2007-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg4_util.h $
 * $brcm_Revision: 10 $
 * $brcm_Date: 10/18/10 1:46p $
 *
 * Module Description:
 *
 * MPEG4 stream parsing 
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg4_util.h $
 * 
 * 10   10/18/10 1:46p vsilyaev
 * SWBLURAY-22500: Merged support for MPEG audio
 * 
 * fix_bdvd_v3.0_SWBLURAY-22500/1   10/14/10 3:45p ialauder
 * Add support for MPEG audio in MP4 container
 * 
 * 9   12/1/09 3:13p vsilyaev
 * SW7405-3513: Merged changes to probe SBR information in the AAC streams
 * 
 * fix_bdvd_v3.0_issue17634/1   11/30/09 1:41p ialauder
 * AAC-HE support
 * 
 * 8   3/4/09 3:31p vsilyaev
 * PR 52434: MPEG4 parsing functions should be compiled with bmedia_player
 * 
 * 7   3/2/09 3:05p vsilyaev
 * PR 52434: Post merge updates/unification of parsing video data
 * 
 * 6   3/2/09 10:27a vsilyaev
 * PR 52434: Merge XViD and VC1 AP support in MKV
 * 
 * fix_bdvd_v1.42_pr12986/2   2/27/09 4:53p gooslin
 * Merge to the latest bmedia code
 * 
 * 5   2/10/09 3:22p gmohile
 * PR 51975 : support AAC Audio ISO IEC 13818-7
 * 
 * fix_bdvd_v1.42_pr12986/1   2/20/09 5:51p gooslin
 * Add mpeg4 frame detection
 * 
 * 4   10/16/08 6:32p vsilyaev
 * PR 47995: Added control over ID's used for media->PES conversion
 * 
 * 3   10/15/07 5:36p jtna
 * PR35124: LIB-Coverity (CID 3048): OVERRUN_STATIC
 * 
 * 2   5/30/07 5:18p vsilyaev
 * PR 31697: Added support for MPEG4Part2 video
 * 
 * 1   5/9/07 3:04p vsilyaev
 * PR 28631: MPEG4 system parser
 * 
 * 
 *******************************************************************************/
#ifndef _BMPEG4_UTIL_H__
#define _BMPEG4_UTIL_H__

#include "bioatom.h"
#include "bmedia_util.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* 7.2.6.5 Semantics, Table 5. objectTypeIndication Values ISO/IEC 14496-1 MPEG4 Part-1, page 35 */
#define BMPEG4_Audio_ISO_IEC_14496_3	0x40
#define BMPEG4_Audio_ISO_IEC_13818_7	0x67
#define BMPEG4_Video_ISO_IEC_14496_2	0x20
#define BMPEG4_Audio_ISO_IEC_11172_3    0x6B

typedef struct bmpeg4_es_descriptor {
	uint16_t ES_ID;
	uint8_t objectTypeIndication;
	uint8_t	streamType;
	union {
		struct {
			uint8_t audioObjectType;
			uint8_t samplingFrequencyIndex;
			uint8_t channelConfiguration;
		} iso_14496_3;
		struct {
			size_t header_size;
			uint8_t header[128];	/* changed from 64 to 128 to accomodate PR35214: LIB-Coverity (CID 3048): OVERRUN_STATIC */
		} iso_14496_2;
	} decoder;
} bmpeg4_es_descriptor;


bool
bmpeg4_parse_es_descriptor(batom_cursor *cursor, bmpeg4_es_descriptor *descriptor);

bool
bmedia_info_probe_aac_info(batom_cursor *cursor, bmedia_info_aac *aac);
#ifdef __cplusplus
}
#endif


#endif /* _BMPEG4_UTIL_H__ */

