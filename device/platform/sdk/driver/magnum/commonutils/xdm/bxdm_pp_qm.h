/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_qm.h $
 * $brcm_Revision: Hydra_Software_Devel/5 $
 * $brcm_Date: 2/16/12 9:29a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_qm.h $
 * 
 * Hydra_Software_Devel/5   2/16/12 9:29a btosi
 * SW7425-2255: bind the PTS offset to the picture when it is selected for
 * display
 * 
 * Hydra_Software_Devel/4   4/21/11 1:53p btosi
 * SW7405-4736: moved BXDM_PPQM_P_PrintUnifiedPicture to bxdm_pp_dbg.c,
 * tweaked the BXDM_PPQM message
 * 
 * Hydra_Software_Devel/3   10/20/10 11:47a btosi
 * SW7405-4804: deliver the SEI messages for pictures that are dropped
 * 
 * Hydra_Software_Devel/2   9/23/10 3:09p btosi
 * SW7405-4736: add support for a XDM instance ID to help debug multi-
 * channel issues
 * 
 * Hydra_Software_Devel/1   2/16/10 10:51a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#ifndef bxdm_pp_QM_H__
#define bxdm_pp_QM_H__

#include "bxdm_pp.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
** "public" queue management funcrtions.
*/
bool BXDM_PPQM_P_PeekAtNextPicture(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstNextPicCntxt,
   BXDM_PictureProvider_P_Picture_Context * pstPrevPicCntxt,
   BXDM_PictureProvider_P_Picture_Context * pstSelectedPicCntxt
   );

void BXDM_PPQM_P_GetNextPicture(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstNextPicCntxt
   );

void BXDM_PPQM_P_PromotePictureToDisplayStatus(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState * pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstPictureUnderEvaluation
   );

void BXDM_PPQM_P_ReleasePicture(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_Picture_Context * pstPicCntxt
   );

void BXDM_PPQM_P_ReleasePictureExt(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_LocalState* pLocalState,
   BXDM_PictureProvider_P_Picture_Context * pstPicCntxt
   );

void BXDM_PPQM_P_InvalidatePictureContext(
   BXDM_PictureProvider_P_Picture_Context * pstPicCntxt
   );

void BXDM_PPQM_P_GetHwPcrOffset(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   uint32_t * puiPcrOffset
   );

void BXDM_PPQM_P_GetCookedPcrOffset(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   uint32_t * puiPcrOffset
   );

void BXDM_PPQM_P_GetPtsOffset(
   BXDM_PictureProvider_Handle hXdmPP,
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   uint32_t * puiPtsOffset
   );

void BXDM_PPQM_P_SetPtsWithFrac(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   uint32_t    uiIndex,
   BXDM_PPFP_P_DataType stPTS
   );

void BXDM_PPQM_P_GetPtsWithFrac(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   uint32_t    uiIndex,
   BXDM_PPFP_P_DataType *pstPTS
   );

void BXDM_PPQM_P_GetPts(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   uint32_t    uiIndex,
   uint32_t * puiPts
   );

void BXDM_PPQM_P_GetPtsUnfiltered(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   uint32_t    uiIndex,
   uint32_t * puiPts
   );

void BXDM_PPQM_P_GetLastPts(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   uint32_t * puiPts
   );

void BXDM_PPQM_P_GetPredictedPts(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   uint32_t * puiPts
   );

void BXDM_PPQM_P_GetPredictedPtsWithFrac(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   BXDM_PPFP_P_DataType *pstPTS
   );

void BXDM_PPQM_P_SetPredictedPtsWithFrac(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_P_PTSIndex ePTSIndex,
   BXDM_PPFP_P_DataType stPTS
   );

void BXDM_PPQM_P_GetPtsType(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_PTSType * pePtsType
   );

void BXDM_PPQM_P_GetPtsTypeUnfiltered(
   BXDM_PictureProvider_P_Picture_Context * pstPicture,
   BXDM_PictureProvider_PTSType * pePtsType
   );

void BXDM_PPQM_P_SetElementPolarity(
   BXDM_PictureProvider_P_Picture_Context *pstPicture,
   uint32_t uiElementIndex,
   BAVC_Polarity ePolarity);

void BXDM_PPQM_P_GetElementPolarity(
   BXDM_PictureProvider_P_Picture_Context *pstPicture,
   uint32_t uiElementIndex,
   BAVC_Polarity *pePolarity);

BXDM_PictureProvider_P_Picture_Context * BXDM_PPQM_P_GetFirstPictureContext(
   BXDM_PictureProvider_Handle hXdmPP
   );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef bxdm_pp_QM_H__ */

