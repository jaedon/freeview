/***************************************************************************
 *	   Copyright (c) 2009, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: botf_itb_feeder.h $
 * $brcm_Revision: 3 $
 * $brcm_Date: 7/13/09 12:36p $
 *
 * Module Description:
 *
 * Start Code Parser module for the on the fly PVR
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/otfpvr/botf_itb_feeder.h $
 * 
 * 3   7/13/09 12:36p vsilyaev
 * PR 56736: Use common functions to get depth of RAVE buffers
 * 
 * 2   7/8/09 11:46a vsilyaev
 * PR 55989: Added handling of special EOS marker
 * 
 * 1   3/31/09 7:28p vsilyaev
 * PR 50887: OTFPVR support for the Nexus video decoder
 *
 ***************************************************************************/

#ifndef BOTF_OTF_FEEDER_H_
#define BOTF_OTF_FEEDER_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct botf_itb_feeder_tag *botf_itb_feeder;  

/* this function is used to create ITB feeder */
botf_itb_feeder
botf_itb_feeder_create(
    const BOTF_ParserPtrs *IPParserPtrs
);

void 
botf_itb_feeder_destroy(
	botf_itb_feeder feeder /* instance of the scv parser */
);


/* copies SVP ITB entries into the decoder ITB entries */
void *botf_itb_feeder_copy(botf_itb_feeder feeder, void *destn, const void *src);

/* this function is used to denote end of the logical packet */
void botf_itb_feeder_checkpoint(botf_itb_feeder feeder);

unsigned botf_itb_feeder_get_free(const BOTF_ParserPtrs *parserPtrs);
unsigned botf_itb_feeder_get_occupied(const BOTF_ParserPtrs *parserPtrs);

#ifdef __cplusplus
}
#endif

#endif /* BOTF_OTF_FEEDER_H_ */



