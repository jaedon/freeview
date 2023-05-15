/***************************************************************************
 *     Copyright (c) 2003-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: muxer_asf.h $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 11/10/11 12:16p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/syslib/muxlib/muxer_asf.h $
 * 
 * Hydra_Software_Devel/3   11/10/11 12:16p nilesh
 * SW7425-1691: File mux output is now open/closed at start/stop capture
 * instead of open/close
 * 
 * Hydra_Software_Devel/2   10/3/11 5:07p nilesh
 * SW7425-891: Added dual simul encoder support
 * 
 * Hydra_Software_Devel/1   5/11/11 12:01p nilesh
 * SW7425-447: Added ASF Mux support
 * 
 ***************************************************************************/

#ifndef MUXER_ASF_H_
#define MUXER_ASF_H_

/* test include */
#include "commands.h"

#include "bmuxlib.h"

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

void* BTST_ASF_OpenMux
   (BTST_P_Context *pContext);

void BTST_ASF_StartMux
   (
      void* pMuxContext,
      void *pOutputContext
   );

BERR_Code
BTST_ASF_DoMux(
         void* pMuxContext,
         BMUXlib_DoMux_Status *pstStatus
   );

void
BTST_ASF_StartCapture(
         void* pMuxContext,
         char* szFilename
         );

void
BTST_ASF_StopCapture(
         void* pMuxContext
         );

void BTST_ASF_FinishMux
   (void* pMuxContext);

void BTST_ASF_StopMux
   (void* pMuxContext);

void BTST_ASF_CloseMux
   (void* pMuxContext);

#ifdef __cplusplus
}
#endif

#endif /* MUXER_ASF_H_ */
