/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: muxer_mp4.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 11/10/11 12:16p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/syslib/muxlib/muxer_mp4.h $
 * 
 * Hydra_Software_Devel/3   11/10/11 12:16p nilesh
 * SW7425-1691: File mux output is now open/closed at start/stop capture
 * instead of open/close
 * 
 * Hydra_Software_Devel/2   10/3/11 5:07p nilesh
 * SW7425-891: Added dual simul encoder support
 * 
 * Hydra_Software_Devel/1   4/4/11 12:12p nilesh
 * SW7425-73: Merged MP4 Mux support
 * 
 * Hydra_Software_Devel/mp4_mux_devel/1   3/30/11 3:10p nilesh
 * SW7425-73: Added MP4 support
 *
 ***************************************************************************/

#ifndef MUXER_MP4_H_
#define MUXER_MP4_H_

/* test include */
#include "commands.h"

#include "bmuxlib.h"

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

void* BTST_MP4_OpenMux
   (BTST_P_Context *pContext);

void BTST_MP4_StartMux
   (
      void* pMuxContext,
      void *pOutputContext
   );

BERR_Code
BTST_MP4_DoMux(
         void* pMuxContext,
         BMUXlib_DoMux_Status *pstStatus
   );

void
BTST_MP4_StartCapture(
         void* pMuxContext,
         char* szFilename
         );

void
BTST_MP4_StopCapture(
         void* pMuxContext
         );

void BTST_MP4_FinishMux
   (void* pMuxContext);

void BTST_MP4_StopMux
   (void* pMuxContext);

void BTST_MP4_CloseMux
   (void* pMuxContext);

#ifdef __cplusplus
}
#endif

#endif /* MUXER_MP4_H_ */