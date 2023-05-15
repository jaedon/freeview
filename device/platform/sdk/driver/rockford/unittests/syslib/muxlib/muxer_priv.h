/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: muxer_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 8/13/12 12:28p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /rockford/unittests/syslib/muxlib/muxer_priv.h $
 * 
 * Hydra_Software_Devel/4   8/13/12 12:28p nilesh
 * SW7425-3362: Merged FNRT support
 * 
 * Hydra_Software_Devel/SW7425-3362/1   7/5/12 1:54p nilesh
 * SW7425-3362: Added FNRTlib support
 * 
 * Hydra_Software_Devel/3   10/3/11 5:07p nilesh
 * SW7425-891: Added dual simul encoder support
 * 
 * Hydra_Software_Devel/2   5/11/11 12:01p nilesh
 * SW7425-447: Added ASF Mux support
 * 
 * Hydra_Software_Devel/1   4/4/11 12:12p nilesh
 * SW7425-73: Merged MP4 Mux support
 * 
 * Hydra_Software_Devel/mp4_mux_devel/1   3/30/11 2:00a nilesh
 * SW7425-73: Re-factored muxer to work with different containers
 *
 ***************************************************************************/

#ifndef MUXER_PRIV_H_
#define MUXER_PRIV_H_

#include "bmuxlib.h"
#include "bmuxlib_fnrt.h"

#ifdef __cplusplus
extern "C" {
#if 0
}
#endif
#endif

#define MUX_NUM_INSTANCES 2

typedef void (*BTST_Muxer_Function)(void *pMuxContext);
typedef void* (*BTST_Muxer_Function_OpenMux)(BTST_P_Context *pContext);
typedef void (*BTST_Muxer_Function_StartMux)(void *pMuxContext, void *pEncoderOutputContext);
typedef BERR_Code (*BTST_Muxer_Function_DoMux)(void *pMuxContext, BMUXlib_DoMux_Status *pstStatus);
typedef void (*BTST_Muxer_Function_StartCapture)(void *pMuxContext, char* szFilename);

typedef enum BTST_Muxer_Type
{
   BTST_Muxer_Type_eTS = 0,
   BTST_Muxer_Type_eMP4,
   BTST_Muxer_Type_eASF,

   BTST_Muxer_Type_eMax
} BTST_Muxer_Type;

typedef struct BTST_Muxer_Info
{
      BTST_TaskHandle           hMuxerTask;
      unsigned                  uiEnableMuxerHandler;
      bool                      bEnableCaptureMuxOutputHandler[MUX_NUM_INSTANCES];
      BKNI_EventHandle          hFinishedEvent[MUX_NUM_INSTANCES];

      /* Function Pointers */
      struct
      {
            BTST_Muxer_Type eMuxerType;
            void *pContext;

            BTST_Muxer_Function_OpenMux fOpenMux;
            BTST_Muxer_Function fConfigMux;
            BTST_Muxer_Function_StartMux fStartMux;
            BTST_Muxer_Function fFinishMux;
            BTST_Muxer_Function fStopMux;
            BTST_Muxer_Function fCloseMux;
            BTST_Muxer_Function_DoMux fDoMux;
            BTST_Muxer_Function_StartCapture fStartCapture;
            BTST_Muxer_Function fStopCapture;

            BMUXlib_FNRT_Handle hMuxFNRT;
      } muxer_interface[MUX_NUM_INSTANCES];
      unsigned uiNumInstances;
} BTST_Muxer_Info;

#ifdef __cplusplus
}
#endif

#endif /* MUXER_PRIV_H_ */
