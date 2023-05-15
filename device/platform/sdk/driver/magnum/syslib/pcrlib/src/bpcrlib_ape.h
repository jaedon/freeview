/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bpcrlib_ape.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 5/17/11 1:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/syslib/pcrlib/7038/bpcrlib_ape.h $
 * 
 * Hydra_Software_Devel/1   5/17/11 1:20p jgarrett
 * SW7425-604: Adding APE support to pcrlib
 * 
 ***************************************************************************/
#ifndef BPCRLIB_APE_H_
#define BPCRLIB_APE_H_

#include "bape.h"
#include "bxpt_rave.h"

typedef struct {
	BAPE_DecoderHandle dec;
	BXPT_RaveCx_Handle rave;
} BPCRlib_Ape_Decoder;

extern const BPCRlib_StcDecIface BPCRlib_Audio_Ape;

#endif /* BPCRLIB_APE_H_ */



