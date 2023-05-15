/***************************************************************************
 *     Copyright (c) 2007-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg1_probe.h $
 * $brcm_Revision: 4 $
 * $brcm_Date: 1/22/09 9:08a $
 *
 * Module Description:
 *
 * MPEG1 systems stream probe
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg1_probe.h $
 * 
 * 4   1/22/09 9:08a gmohile
 * PR 50725 : Fix typo
 * 
 * 3   1/20/09 4:20p gmohile
 * PR 50725 : Seperate mpeg1 pts parsing
 * 
 * 2   7/25/07 1:50p vsilyaev
 * PR 32813: Separated public and internal API of bmedia_probe
 * 
 * 1   7/21/07 9:51p vsilyaev
 * PR 32813: MPEG1 probe module
 * 
 * 
 *******************************************************************************/
#ifndef __BMPEG1_PROBE_H__
#define __BMPEG1_PROBE_H__


#include "bmedia_probe_impl.h"
#ifdef __cplusplus
extern "C"
{
#endif

extern const bmedia_probe_format_desc bmpeg1_probe;

bmedia_timestamp_parser_t bmpeg1_pts_parser_create(uint16_t stream_id, size_t packet_len);

#ifdef __cplusplus
}
#endif


#endif  /* __BMPEG1_PROBE_H__ */

