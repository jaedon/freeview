/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: muxer_ts.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/3/11 5:07p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/syslib/muxlib/muxer_ts.h $
 * 
 * Hydra_Software_Devel/2   10/3/11 5:07p nilesh
 * SW7425-891: Added dual simul encoder support
 * 
 * Hydra_Software_Devel/1   4/4/11 12:12p nilesh
 * SW7425-73: Merged MP4 Mux support
 * 
 * Hydra_Software_Devel/mp4_mux_devel/2   3/30/11 2:26a nilesh
 * SW7425-73: Moved include
 * 
 * Hydra_Software_Devel/mp4_mux_devel/1   3/30/11 2:01a nilesh
 * SW7425-73: Re-factored muxer to work with different containers
 *
 ***************************************************************************/

#ifndef MUXER_TS_H_
#define MUXER_TS_H_

/* test include */
#include "commands.h"

#include "bmuxlib.h"

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

void* BTST_TS_OpenMux
   (BTST_P_Context *pContext);

void BTST_TS_ConfigMux
   (void *pMuxContext);

void BTST_TS_StartMux
   (
      void *pMuxContext,
      void *pOutputContext
   );

BERR_Code
BTST_TS_DoMux
   (
   void *pMuxContext,
   BMUXlib_DoMux_Status *pstStatus
   );

void
BTST_TS_StartCapture
   (
   void *pMuxContext,
   char* szFilename
   );

void
BTST_TS_StopCapture
   (
   void *pMuxContext
   );

void BTST_TS_FinishMux
   (void *pMuxContext);

void BTST_TS_StopMux
   (void *pMuxContext);

void BTST_TS_CloseMux
   (void *pMuxContext);

#ifdef __cplusplus
}
#endif

#endif /* MUXER_TS_H_ */
