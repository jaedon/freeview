/***************************************************************************
 *     Copyright (c) 2007-2012, Broadcom Corporation
 *     All Righpes Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg2pes_probe.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 4/30/12 1:37p $
 *
 * Module Description:
 *
 * MPEG-2 PES probe 
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg2pes_probe.h $
 * 
 * 5   4/30/12 1:37p vsilyaev
 * SW7405-4929: Removed 'no-op' intentification/parsing of subtitle
 * streams
 * 
 * 4   4/20/12 2:27p vsilyaev
 * SW7405-4929: Added intentification/parsing of subtitle streams
 * 
 * 3   1/13/09 3:45p gmohile
 * PR 50725 : Add duration probe for PES and VOB streams
 * 
 * 2   7/25/07 1:50p vsilyaev
 * PR 32813: Separated public and internal API of bmedia_probe
 * 
 * 1   7/24/07 6:04p vsilyaev
 * PR 32813: MPEG-2 PES probe
 * 
 * 
 *******************************************************************************/
#ifndef __BMPEG2PES_PROBE_H__
#define __BMPEG2PES_PROBE_H__


#include "bmedia_probe_impl.h"
#ifdef __cplusplus
extern "C"
{
#endif

#define BMPEG2PES_PROBE_ID       0x9002
extern const bmedia_probe_format_desc bmpeg2pes_probe;

bmedia_timestamp_parser_t bmpeg2pes_pts_parser_create(uint16_t stream_id, size_t packet_len);

#ifdef __cplusplus
}
#endif


#endif  /* __BMPEG2PES_PROBE_H__ */

