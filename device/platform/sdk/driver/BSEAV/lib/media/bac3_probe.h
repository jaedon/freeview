/***************************************************************************
 *     Copyright (c) 2007 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bac3_probe.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 12/2/08 11:20a $
 *
 * Module Description:
 *
 * BMedia library, AC3 elementary stream probe
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bac3_probe.h $
 * 
 * 2   12/2/08 11:20a vsilyaev
 * PR 49834: Addec code to read bsid and print extra codec information
 * 
 * 1   7/20/07 12:52p vsilyaev
 * PR 32813: Added elementary stream probes
 * 
 * 
 *******************************************************************************/
#ifndef _BAC3_PROBE_H__
#define _BAC3_PROBE_H__

#include "bmedia_probe_es.h"

#ifdef __cplusplus
extern "C"
{
#endif
	
extern const bmedia_probe_es_desc bac3_probe;
typedef struct bmedia_probe_ac3_audio {
    bool valid;
    baudio_format codec; /* type of audio codec in the extended information */
} bmedia_probe_ac3_audio;

#ifdef __cplusplus
}
#endif


#endif /* _BMEDIA_PROBE_ES_H__ */

