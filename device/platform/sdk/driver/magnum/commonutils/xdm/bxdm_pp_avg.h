/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bxdm_pp_avg.h $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 2/16/10 10:50a $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/commonutils/xdm/bxdm_pp_avg.h $
 * 
 * Hydra_Software_Devel/1   2/16/10 10:50a nilesh
 * SW7405-2993: Initial XDM version
 *
 ***************************************************************************/

#ifndef bxdm_pp_AVG_H__
#define bxdm_pp_AVG_H__

#ifdef __cplusplus
extern "C" {
#endif

#if 0
}
#endif

typedef struct BXDM_PPAVG_P_Context *BXDM_PPAVG_P_Handle;

typedef struct BXDM_PPAVG_P_Settings
{
   uint32_t uiNumSamples;
} BXDM_PPAVG_P_Settings;

BERR_Code 
BXDM_PPAVG_P_GetDefaultSettings(
   BXDM_PPAVG_P_Settings *pstAvgSettings
   );

BERR_Code
BXDM_PPAVG_P_Create(
   BXDM_PPAVG_P_Handle *phAvg,
   const BXDM_PPAVG_P_Settings *pstAvgSettings
   );

BERR_Code
BXDM_PPAVG_P_Destroy(
   BXDM_PPAVG_P_Handle hAvg
   );

BERR_Code
BXDM_PPAVG_P_AddValueFixedPoint(
   BXDM_PPAVG_P_Handle hAvg,   
   const BXDM_PPFP_P_DataType *pstValue
   );

BERR_Code
BXDM_PPAVG_P_AddValueInteger(
   BXDM_PPAVG_P_Handle hAvg,
   int32_t iValue
   );

BERR_Code
BXDM_PPAVG_P_Reset(
   BXDM_PPAVG_P_Handle hAvg
   );

BERR_Code
BXDM_PPAVG_P_GetAverage(
   BXDM_PPAVG_P_Handle hAvg,
   BXDM_PPFP_P_DataType *pstAverage,
   uint32_t *puiNumSamples
   );

#ifdef __cplusplus
}
#endif

#endif /* bxdm_pp_AVG_H__ */
