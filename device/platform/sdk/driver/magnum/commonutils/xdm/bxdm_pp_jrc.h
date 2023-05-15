/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_jrc.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/16/10 10:51a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_jrc.h $
 * 
 * Hydra_Software_Devel/1   2/16/10 10:51a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#ifndef bxdm_pp_JRC_H_
#define bxdm_pp_JRC_H_

#include "bxdm_pp_fp.h"

#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif

typedef struct BXDM_PPJRC_P_Context *BXDM_PPJRC_P_Handle;

typedef struct BXDM_PPJRC_P_Settings
{
   uint32_t uiQueueDepth;
   uint32_t uiJitterLowerThreshold;
   uint32_t uiJitterUpperThreshold;
} BXDM_PPJRC_P_Settings;

BERR_Code BXDM_PPJRC_P_GetDefaultSettings(
   BXDM_PPJRC_P_Settings *pstJrcSettings
   );

/* Create the XVD JRC Handle */
BERR_Code BXDM_PPJRC_P_Create(
   BXDM_PPJRC_P_Handle *phJrc,
   const BXDM_PPJRC_P_Settings *pJrcSettings
   );

/* Destroy the XVD JRC Handle */
BERR_Code BXDM_PPJRC_P_Destroy(
   BXDM_PPJRC_P_Handle hJrc
   );

BERR_Code BXDM_PPJRC_P_Reset(
   BXDM_PPJRC_P_Handle hJrc
   );

BERR_Code BXDM_PPJRC_P_AddValue(
   BXDM_PPJRC_P_Handle hJrc,
   uint32_t uiOriginalValue,
   const BXDM_PPFP_P_DataType *pstExpectedDelta,
   uint32_t  uiStepCount,
   uint32_t *puiJitterCorrectedValue
   );

BERR_Code BXDM_PPJRC_P_GetLowerThreshold(
   BXDM_PPJRC_P_Handle hJrc,
   uint32_t *puiLowerThreshold
   );

BERR_Code BXDM_PPJRC_P_SetLowerThreshold(
   BXDM_PPJRC_P_Handle hJrc,
   uint32_t uiLowerThreshold
   );

BERR_Code BXDM_PPJRC_P_GetUpperThreshold(
   BXDM_PPJRC_P_Handle hJrc,
   uint32_t *puiUpperThreshold
   );

BERR_Code BXDM_PPJRC_P_SetUpperThreshold(
   BXDM_PPJRC_P_Handle hJrc,
   uint32_t uiUpperThreshold
   );

#ifdef __cplusplus
}
#endif

#endif /* bxdm_pp_JRC_H_ */
