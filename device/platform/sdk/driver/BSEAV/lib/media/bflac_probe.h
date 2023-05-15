/***************************************************************************
 *     Copyright (c) 2011 Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bflac_probe.h $
 * $brcm_Revision: 1 $
 * $brcm_Date: 11/11/11 5:22p $
 *
 * Module Description:
 *
 * BMedia library, stream probe module
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bflac_probe.h $
 * 
 * 1   11/11/11 5:22p vsilyaev
 * SW7425-1752: FLAC file format support
 * 
 *******************************************************************************/
#ifndef _BFLAC_PROBE_H__
#define _BFLAC_PROBE_H__

#include "bmedia_probe_impl.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bflac_probe_stream {
      bmedia_probe_stream media;
} bflac_probe_stream;

typedef struct bflac_probe_track {
      bmedia_probe_track media;
} bflac_probe_track;

extern const bmedia_probe_format_desc bflac_probe;

#ifdef __cplusplus
}
#endif


#endif /* _BFLAC_PROBE_H__ */

