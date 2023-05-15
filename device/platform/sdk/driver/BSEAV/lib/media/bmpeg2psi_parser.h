/***************************************************************************
 *     Copyright (c) 2007-2008, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bmpeg2psi_parser.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 6/30/08 12:11p $
 *
 * Module Description:
 *
 * MPEG-2 PSI Parser/Demux library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/media/bmpeg2psi_parser.h $
 * 
 * 2   6/30/08 12:11p vsilyaev
 * PR 32813: Removed artificial dependency in the SettopApi
 * 
 * 1   7/17/07 4:18p vsilyaev
 * PR 32813: MPEG-2 TS/PES/PSI parser
 * 
 * 
 *******************************************************************************/
#ifndef __BMPEG2PSI_PARSER_H__
#define __BMPEG2PSI_PARSER_H__


#include "bmpeg2ts_parser.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct bmpeg2psi_parser {
	batom_accum_t accum;
	size_t section_length;
	size_t section_left;
	bool hold_enable;
	uint8_t data[16];
    struct {
	    uint8_t mask[16];	/* Bits to be ignored. See Description. */
	    uint8_t coef[16];	/* Bits to be included. See Description. */
	    uint8_t excl[16];	/* Bits to be excluded. Doesn't apply to Venom2 platforms. See Description. */
    } filter;
	void *section_cnxt;
	void (*section)(void *section_cnxt, batom_t atom);
} bmpeg2psi_parser;

int bmpeg2psi_parser_init(batom_factory_t factory, bmpeg2psi_parser *psi);
void bmpeg2psi_parser_shutdown(bmpeg2psi_parser *psi);
bmpeg2ts_parser_action bmpeg2psi_parser_feed(bmpeg2psi_parser *psi, unsigned ts_flags, batom_accum_t src, batom_cursor *payload_start, size_t len);
void bmpeg2psi_parser_flush(bmpeg2psi_parser *psi);

#ifdef __cplusplus
}
#endif


#endif  /* __BMPEG2PES_PARSER_H__ */

