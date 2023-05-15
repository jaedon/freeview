/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_rsbuf.h $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 6/18/12 10:53a $
 *
 * Porting interface code for the rate smoothing buffer block in the 
 * transport core. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/include/bxpt_rsbuf.h $
 * 
 * Hydra_Software_Devel/8   6/18/12 10:53a gmullen
 * SW7425-2914: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2914/1   6/11/12 10:59a gmullen
 * SW7425-2914: Moved and updated files to new home
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

#ifndef BXPT_RS_BUFFER_H__
#define BXPT_RS_BUFFER_H__

#include "bxpt.h"

#ifdef __cplusplus
extern "C"{
#endif

#if BXPT_HAS_FIXED_RSBUF_CONFIG
	/* Newer chips set the RS config through the defaults struct at BXPT_Open() */
#else

/*= Module Overview *********************************************************

Overview

Usage / Sample Code

***************************************************************************/

/***************************************************************************
Summary:
Configure the given input band rate-smoothing buffers peak rate.

Description:
Set the peak input rate for the rate-smoothing buffer. Each input band has its
own buffer; this call allows each buffer to support a unique peak rate. The 
caller specifies the peak rate in Mbps. 

Returns:
    BERR_SUCCESS                - Defaults loaded into struct.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_RsBuf_SetBufferSize					
****************************************************************************/
BERR_Code BXPT_RsBuf_SetBandDataRate(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum,				/* [in] Which input band parser to configure */
	unsigned long PeakRate,			/* [in] Max data rate (in bps) the band will handle. */
    unsigned PacketLen              /* [in] Packet size ,130 for dss and 188 for mpeg */
	);

/***************************************************************************
Summary:
Configure the given input band rate-smoothing buffer's size.

Description:
Set the DRAM buffer size for the rate-smoothing buffer.  Each input band has 
its own buffer; this call allows customizing each buffer's size. The DRAM 
buffer size is specified in bytes, but must be a multiple of 256; the size 
will be rounded down to the nearest multiple otherwise.

The buffer is allocated internally from the system heap. It will be freed 
when the XPT porting interface is closed.

Returns:
    BERR_SUCCESS                - Defaults loaded into struct.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:	
BXPT_RsBuf_SetBandDataRate				
****************************************************************************/
BERR_Code BXPT_RsBuf_SetBufferSize(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum,				/* [in] Which input band parser to configure */
	unsigned long BufferSize		/* [in] Buffer size in bytes */
	);

/***************************************************************************
Summary:
Configure the given Playback channel rate-smoothing buffers peak rate.

Description:
Set the peak input rate for the rate-smoothing buffer. Each playback channel
has its own buffer; this call allows each buffer to support a unique peak 
rate. The caller specifies the peak rate in Mbps. 

Returns:
    BERR_SUCCESS                - Defaults loaded into struct.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:
BXPT_RsBuf_SetPlaybackBufferSize					
****************************************************************************/
BERR_Code BXPT_RsBuf_SetPlaybackDataRate(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned PbNum,					/* [in] Which playback channel to configure */
	unsigned long PeakRate			/* [in] Max data rate (in bps) the band will handle. */
	);

/***************************************************************************
Summary:
Configure the given Playback channels rate-smoothing buffer's size.

Description:
Set the DRAM buffer size for the rate-smoothing buffer.  Each Playback channel has 
its own buffer; this call allows customizing each buffer's size. The DRAM 
buffer size is specified in bytes, but must be a multiple of 256; the size 
will be rounded down to the nearest multiple otherwise.

The buffer is allocated internally from the system heap. It will be freed 
when the XPT porting interface is closed.

Returns:
    BERR_SUCCESS                - Defaults loaded into struct.
    BERR_INVALID_PARAMETER      - Bad input parameter
 
See Also:	
BXPT_RsBuf_SetPlaybackDataRate				
****************************************************************************/
BERR_Code BXPT_RsBuf_SetPlaybackBufferSize(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum,				/* [in] Which input band parser to configure */
	unsigned long BufferSize		/* [in] Buffer size in bytes */
	);


/*
** These functions are called internally from BXPT_Open() and BXPT_Close(). 
** Users should NOT uses these functions directly.
*/

#define BXPT_P_PLAYBACK_ENABLE_BIT_OFFSET		( 8 )

BERR_Code BXPT_P_RsBuf_Init(
	BXPT_Handle hXpt, 	   	   	/* [in] Handle for this transport */
    const BXPT_DramBufferCfg *Cfg
	);

BERR_Code BXPT_P_RsBuf_Shutdown(
	BXPT_Handle hXpt 	   	   	/* [in] Handle for this transport */
	);

bool BXPT_P_RsBuf_IsBufferEnabled( 
	BXPT_Handle hXpt, 
	unsigned BandNum 
	);

unsigned long BXPT_P_RsBuf_GetBlockout(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum				/* [in] Which input band parser to configure */
	);

BERR_Code BXPT_P_RsBuf_SetBlockout(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum,             /* [in] Packet size ,130 for dss and 188 for mpeg */
    unsigned long NewBO
	);


BERR_Code BXPT_P_RsBuf_PlaybackSetBlockout(
	BXPT_Handle hXpt, 	   			/* [in] Handle for this transport */
	unsigned BandNum,             /* [in] Packet size ,130 for dss and 188 for mpeg */
    unsigned long NewBO
	);

uint32_t BXPT_P_RsBuf_ComputeBlockOut( 
	unsigned long PeakRate,			/* [in] Max data rate (in bps) the band will handle. */
    unsigned PacketLen             /* [in] Packet size ,130 for dss and 188 for mpeg */
    );

#endif /* BXPT_HAS_FIXED_RSBUF_CONFIG */

#ifdef __cplusplus
}
#endif

#endif	/* BXPT_RS_BUFFER_H__ */


