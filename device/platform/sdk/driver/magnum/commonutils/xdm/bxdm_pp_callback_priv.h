/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_callback_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/2 $
 * $brcm_Date: 10/20/10 11:47a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_callback_priv.h $
 * 
 * Hydra_Software_Devel/2   10/20/10 11:47a btosi
 * SW7405-4804: deliver the SEI messages for pictures that are dropped
 * 
 * Hydra_Software_Devel/1   2/16/10 10:51a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#ifndef bxdm_pp_CALLBACK_H__
#define bxdm_pp_CALLBACK_H__

#ifdef __cplusplus
extern "C" {
#endif

void BXDM_PPCB_P_EnableAutomaticCallbacks(
   BXDM_PictureProvider_Handle hXdmPP
   );

void BXDM_PPCB_P_ExecuteSingleCallback(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_Callback eCallbackName
   );

void BXDM_PPCB_P_ExecuteSingleCallbackExt(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_Callback eCallbackName
   );

void BXDM_PPCB_P_ExecuteCallbacks(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState
   );

void BXDM_PPCB_P_EvaluateCriteria_IFrame(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstSelectPic
   );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef bxdm_pp_CALLBACK_H__ */

