/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxpt_xcbuf_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 3/8/12 3:41p $
 *
 * Porting interface code for the transport client buffer block. 
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/xpt/base2/bxpt_xcbuf_priv.h $
 * 
 * Hydra_Software_Devel/3   3/8/12 3:41p gmullen
 * SW7425-2497: Merged to Hydra
 * 
 * Hydra_Software_Devel/SW7425-2497/1   3/5/12 4:30p gmullen
 * SW7425-2497: Checks for errors in data pipeline
 * 
 * Hydra_Software_Devel/2   12/6/10 10:01a gmullen
 * SW7422-117: Disable pausing from XC buffer to flush data on playback
 * stopping
 * 
 * Hydra_Software_Devel/1   10/25/10 2:09p gmullen
 * SW7425-15: Moved srcs to base2 folder
 * 
 * Hydra_Software_Devel/1   10/25/10 9:12a gmullen
 * SW7425-15: Updated headers and added stub versions of C files
 * 
 * 
 * 
 ***************************************************************************/

#ifndef BXPT_XCBUFF_PRIV_H__
#define BXPT_XCBUFF_PRIV_H__

#include "bxpt.h"

#ifdef __cplusplus
extern "C"{
#endif


/*
** These functions are called internally from BXPT_Open() and BXPT_Close(). 
** Users should NOT uses these functions directly.
*/

BERR_Code BXPT_P_XcBuf_Init(
    BXPT_Handle hXpt,           /* [in] Handle for this transport */
    const BXPT_BandWidthConfig *BandwidthConfig
    );

BERR_Code BXPT_P_XcBuf_Shutdown(
    BXPT_Handle hXpt            /* [in] Handle for this transport */
    );

void BXPT_XcBuf_P_EnablePlaybackPausing( 
    BXPT_Handle hXpt, 
    unsigned PbChannelNum,
    bool PauseEn
    );

BERR_Code BXPT_P_XcBuf_ReportOverflows( BXPT_Handle hXpt );

#ifdef __cplusplus
}
#endif

#endif /* BXPT_XCBUFF_PRIV_H__ */


