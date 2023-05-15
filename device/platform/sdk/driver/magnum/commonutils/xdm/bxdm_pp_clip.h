/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_clip.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/16/10 10:51a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_clip.h $
 * 
 * Hydra_Software_Devel/1   2/16/10 10:51a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#ifndef bxdm_pp_CLIP_H__
#define bxdm_pp_CLIP_H__

#ifdef __cplusplus
extern "C" {
#endif

#if 0
}
#endif

void BXDM_PPCLIP_P_Reset(
   BXDM_PictureProvider_Handle hXdmPP
   );

bool BXDM_PPCLIP_P_ClipTimeTSMHandler(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_P_Picture_Context* pstPicture,
   bool bEvaluateActualPts,
   BXDM_PictureProvider_TSMResult *peTsmState
   );

void BXDM_PPCLIP_P_ClipTimeTSMTransitionHandler(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_P_Picture_Context* pstPicture,
   BXDM_PictureProvider_TSMResult *peTsmState
   );

void BXDM_PPCLIP_P_ClipTimeQueueTransitionHandler(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstPicCntxt
   );

void BXDM_PPCLIP_P_ClipTimeTrickModeTransitionHandler(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState
   );

void BXDM_PPCLIP_P_ClipTimeCallbackTriggerHandler(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstPicture
   );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef bxdm_pp_CLIP_H__ */
