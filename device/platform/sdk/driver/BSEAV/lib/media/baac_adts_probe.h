/***************************************************************************
 *     Copyright (c) 2007 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: baac_adts_probe.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 11/16/10 5:52p $
 *
 * Module Description:
 *
 * BMedia library, Probe of AAC audio with ADTS encapsulation
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/baac_adts_probe.h $
 * 
 * 2   11/16/10 5:52p vsilyaev
 * SWBLURAY-22285: Merged AAC ES player
 * 
 * fix_bdvd_v3.0_SWBLURAY-22285/1   11/16/10 3:28p ialauder
 * Add support for AAC elementary streams.
 * 
 * 1   7/23/07 7:13p vsilyaev
 * PR 32813: AAC ADTS probe
 * 
 *******************************************************************************/
#ifndef _BAAC_ADTS_PROBE_H__
#define _BAAC_ADTS_PROBE_H__

#include "bmedia_probe_es.h"

#ifdef __cplusplus
extern "C"
{
#endif
	
#define B_AAC_ADTS_N_FRAMES	3
#define B_AAC_ADTS_SYNC	0xFFF
#define B_AAC_ADTS_SYNC_LEN	2

extern const bmedia_probe_es_desc baac_adts_probe;

size_t b_aac_adts_probe_parse_header(batom_cursor *cursor, bmedia_probe_audio *info, uint32_t sync_word);

#ifdef __cplusplus
}
#endif


#endif /* _BAAC_ADTS_PROBE_H__ */

