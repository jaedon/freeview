/***************************************************************************
 *     Copyright (c) 2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpcrlib_rap.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 2/3/06 3:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/pcrlib/7038/bpcrlib_rap.h $
 * 
 * Hydra_Software_Devel/2   2/3/06 3:50p vsilyaev
 * PR 18642: Update PCRlibrary to support playback of constant bitrate
 * streams
 * 
 * Hydra_Software_Devel/PR18642/1   1/19/06 6:16p vsilyaev
 * PR18642: Use PTS offset to control buffer depth in the decoders
 * 
 * Hydra_Software_Devel/1   7/21/05 12:36p vsilyaev
 * PR 15967: Added raptor STC interface
 * 
 ***************************************************************************/
#ifndef BPCRLIB_RAP_H_
#define BPCRLIB_RAP_H_

#include "brap.h"

#ifndef BCHP_7411_VER
#include "bxpt_rave.h"
#endif


typedef struct {
	BRAP_ChannelHandle dec;
#ifdef BCHP_7411_VER
	BRAP_TRANS_ChannelHandle trans;
#else
	BXPT_RaveCx_Handle rave;
#endif
} BPCRlib_Rap_Decoder;

extern const BPCRlib_StcDecIface BPCRlib_Audio_Rap;

#endif /* BPCRLIB_XVD_H_ */



