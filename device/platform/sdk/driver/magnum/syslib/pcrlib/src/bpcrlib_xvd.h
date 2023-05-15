/***************************************************************************
 *     Copyright (c) 2005, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpcrlib_xvd.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 2/3/06 3:50p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/pcrlib/7038/bpcrlib_xvd.h $
 * 
 * Hydra_Software_Devel/2   2/3/06 3:50p vsilyaev
 * PR 18642: Update PCRlibrary to support playback of constant bitrate
 * streams
 * 
 * Hydra_Software_Devel/1   6/24/05 3:50p vsilyaev
 * PR 15967: 7038 implementation of the audio/video interface.
 * 
 ***************************************************************************/
#ifndef BPCRLIB_XVD_H_
#define BPCRLIB_XVD_H_

#include "bxvd.h"

#ifndef BCHP_7411_VER
#include "bxpt_rave.h"
#endif


extern const BPCRlib_StcDecIface BPCRlib_Video_Xvd;

typedef struct {
	BXVD_ChannelHandle dec;
#ifndef BCHP_7411_VER
	BXPT_RaveCx_Handle rave;
#endif
} BPCRlib_Xvd_Decoder;

#endif /* BPCRLIB_XVD_H_ */



