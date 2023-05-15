/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_vtsm.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 3/23/10 10:51a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_vtsm.h $
 * 
 * Hydra_Software_Devel/2   3/23/10 10:51a nilesh
 * SW7405-4072: Fixed issue with frame advance occurring continuously
 * 
 * Hydra_Software_Devel/1   2/16/10 10:52a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#ifndef bxdm_pp_VTSM_H__
#define bxdm_pp_VTSM_H__

#include "bxdm_pp.h"


#ifdef __cplusplus
extern "C" {
#endif

#if 0
}
#endif

typedef struct BXDM_PPVTSM_P_State
{
   BXDM_PPFP_P_DataType stVirtualSTC;
   bool bVirtualPTSInitialized;
   bool bTrickModeTransition;
   BXDM_PictureProvider_FrameAdvanceMode eFrameAdvanceMode;
} BXDM_PPVTSM_P_State;

void BXDM_PPVTSM_P_VirtualStcSet(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   );

void BXDM_PPVTSM_P_VirtualStcIncrement(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   );

void BXDM_PPVTSM_P_VirtualPtsInterpolate(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_P_Picture_Context* pstPicture,
   BXDM_PictureProvider_P_Picture_Context* pstPrevPicture,
   BXDM_PictureProvider_P_Picture_Context* pstSelectedPicture
   );

void BXDM_PPVTSM_P_VirtualStcGet(
   const BXDM_PictureProvider_Handle hXdmPP,
   uint32_t* puiStc
   );

void BXDM_PPVTSM_P_ClipTimeTrickModeTransitionHandler(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState
   );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef bxdm_pp_VTSM_H__ */
