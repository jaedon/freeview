/***************************************************************************
 *     Copyright (c) 2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: btune.h $
 * $brcm_Revision: 2 $
 * $brcm_Date: 2/2/06 5:04p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/utils/common/btune.h $
 * 
 * 2   2/2/06 5:04p vsilyaev
 * PR 19233:Use common function for tune. Added diseqc_tone option
 * 
 * 1   11/30/05 5:02p vsilyaev
 * PR 18183: Common tune functions
 * 
 *
 ***************************************************************************/

#ifndef __TUNE_H_
#define __TUNE_H_

typedef struct btune_state {
	bstream_t stream;
	btuner_t tuner;
} btune_state;

/* bsettop.h shall be inclided prior to this point */

void btune_mpeg_init(bstream_mpeg *mpeg);
bstream_t btune_tune(unsigned channel, bstream_mpeg *mpeg, bool encode_analog, btune_state *state);
void btune_teardown(btune_state *state);

#endif  /* __TUNE_H_ */

