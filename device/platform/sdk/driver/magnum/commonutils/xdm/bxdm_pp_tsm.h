/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_tsm.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/16/10 10:51a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_tsm.h $
 * 
 * Hydra_Software_Devel/1   2/16/10 10:51a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#ifndef bxdm_pp_TSM_H__
#define bxdm_pp_TSM_H__

#include "bxdm_pp.h"

#ifdef __cplusplus
extern "C" {
#endif

#if 0
}
#endif

void BXDM_PPTSM_P_PtsCalculateParameters (
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState *pLocalState,
   BXDM_PictureProvider_P_Picture_Context* pstPicture
   );

void BXDM_PPTSM_P_PtsInterpolate (
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_P_Picture_Context* pstPicture,
   BXDM_PictureProvider_P_Picture_Context* pstPrevPicture
   );

void BXDM_PPTSM_P_EvaluateTsmState(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_P_Picture_Context* pstPicture
   );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef bxdm_pp_TSM_H__ */
/* End of File */
