/***************************************************************************
 *     Copyright (c) 2007-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bflv_probe.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 7/11/08 3:57p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/flv/bflv_probe.h $
 * 
 * 1   7/11/08 3:57p vsilyaev
 * PR 31188: Added probe module for the FLV files
 * 
 *******************************************************************************/
#ifndef _BFLV_PROBE_H__
#define _BFLV_PROBE_H__

#include "bmedia_probe_impl.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bflv_probe_stream {
	  bmedia_probe_stream media;
} bflv_probe_stream;

typedef struct bflv_probe_track {
	  bmedia_probe_track media;
} bflv_probe_track;
		
extern const bmedia_probe_format_desc bflv_probe;

#ifdef __cplusplus
}
#endif


#endif /* _BFLV_PROBE_H__ */

