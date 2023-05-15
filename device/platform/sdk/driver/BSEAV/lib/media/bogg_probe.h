/***************************************************************************
 *     Copyright (c) 2011 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bogg_probe.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 8/25/11 6:17p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bogg_probe.h $
 * 
 * 1   8/25/11 6:17p vsilyaev
 * SWDTV-8388: Added OGG support
 * 
 * 1   7/11/08 3:57p vsilyaev
 * PR 31188: Added probe module for the OGG files
 * 
 *******************************************************************************/
#ifndef _BOGG_PROBE_H__
#define _BOGG_PROBE_H__

#include "bmedia_probe_impl.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bogg_probe_stream {
      bmedia_probe_stream media;
} bogg_probe_stream;

typedef struct bogg_probe_track {
      bmedia_probe_track media;
} bogg_probe_track;

extern const bmedia_probe_format_desc bogg_probe;

#ifdef __cplusplus
}
#endif


#endif /* _BOGG_PROBE_H__ */

