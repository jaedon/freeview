/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_rsbuf_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/8/12 3:41p $
 *
 * Porting interface code for the rate smoothing buffer block in the 
 * transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/base2/bxpt_rsbuf_priv.h $
 * 
 * Hydra_Software_Devel/2   3/8/12 3:41p gmullen
 * SW7425-2497: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2497/1   3/5/12 4:30p gmullen
 * SW7425-2497: Checks for errors in data pipeline
 * 
 * Hydra_Software_Devel/1   10/25/10 2:09p gmullen
 * SW7425-15: Moved srcs to base2 folder
 * 
 * Hydra_Software_Devel/1   10/25/10 9:12a gmullen
 * SW7425-15: Updated headers and added stub versions of C files
 * 
 * Hydra_Software_Devel/1   10/8/10 2:58p gmullen
 * SW7425-15: Added header files
 * 
 * Hydra_Software_Devel/7   4/16/09 7:20p piyushg
 * PR53728: Each of the XC buffer clients (Remux, RAVE, and the Mesg
 * filters) would get a separate API to set
 * their input bitrates. That API would take a client handle and the ID of
 * the parser band. The XPT PI would
 * then walk backwards through the pipeline, increasing only the bitrates
 * for the XC and RS buffers that it
 * needs to.
 * 
 * Hydra_Software_Devel/6   5/7/08 10:47a gmullen
 * PR42443: Removed call to change band rates when changing parser mode.
 * 
 * Hydra_Software_Devel/5   7/20/06 5:30p katrep
 * PR22365: Added better mangement of XC buffer client BW for playback.XC
 * buf for PB is given max BW & BW throtle is done at PB engine.Spare BW
 * enabllag is used to provide more BW for PB if available.
 * 
 * Hydra_Software_Devel/4   7/12/06 5:54p gmullen
 * PR21042: XC and RS buffers can be customized to reduce total memory
 * needs.
 * 
 * Hydra_Software_Devel/3   4/13/06 4:02p gmullen
 * PR20914: Ported over Prashant's fix from 7401A0
 * 
 * Hydra_Software_Devel/2   2/1/06 10:19a gmullen
 * PR18998: Fixed overflow issue in RAVE ITB/CDB, added support for PB
 * channels.
 * 
 * Hydra_Software_Devel/2   7/15/05 9:04a gmullen
 * PR15309: Inita i
 * 
 * Hydra_Software_Devel/1   5/13/05 1:27p gmullen
 * PR15309: First draft of XPT PI.
 * 
 ***************************************************************************/

#ifndef BXPT_RS_BUFFER_PRIV_H__
#define BXPT_RS_BUFFER_PRIV_H__

#include "bxpt.h"

#ifdef __cplusplus
extern "C"{
#endif


/*
** These functions are called internally from BXPT_Open() and BXPT_Close(). 
** Users should NOT uses these functions directly.
*/

BERR_Code BXPT_P_RsBuf_Init(
	BXPT_Handle hXpt, 	   	   	/* [in] Handle for this transport */
    const BXPT_BandWidthConfig *BandwidthConfig
	);

BERR_Code BXPT_P_RsBuf_Shutdown(
	BXPT_Handle hXpt 	   	   	/* [in] Handle for this transport */
	);

BERR_Code BXPT_P_RsBuf_ReportOverflows( BXPT_Handle hXpt );

#ifdef __cplusplus
}
#endif

#endif	/* BXPT_RS_BUFFER_H__ */


