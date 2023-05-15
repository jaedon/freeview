/***************************************************************************
 *	   Copyright (c) 2003-2010, Broadcom Corporation
 *	   All Rights Reserved
 *	   Confidential Property of Broadcom Corporation
 *
 *	THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *	AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *	EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_cm3d_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/8 $
 * $brcm_Date: 4/7/10 11:27a $
 *
 * Module Description:
 *
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_cm3d_priv.h $
 * 
 * Hydra_Software_Devel/8   4/7/10 11:27a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/7   4/5/10 3:59p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/6   5/13/09 12:32a pntruong
 * PR51108: Coverity Defect ID:13602 OVERRUN_STATIC.
 *
 * Hydra_Software_Devel/5   12/3/08 5:04p jessem
 * PR 48992: Added Chroma Histogram API support.
 *
 * Hydra_Software_Devel/4   7/9/08 6:12p pntruong
 * PR44306: CM3D and CMP are tied together need to reprogram CM3D when CMP
 * is reset.
 *
 * Hydra_Software_Devel/3   5/12/08 3:00p jessem
 * PR 39335: Removed bInitial parameter from BVDC_P_Cm3d_BuildRul_isr.
 *
 * Hydra_Software_Devel/2   3/31/08 3:18p jessem
 * PR 39335: Modified implementation.
 *
 * Hydra_Software_Devel/1   3/31/08 2:19p jessem
 * PR 39335: Initial version
 *
 *
 ***************************************************************************/
#ifndef BVDC_CM3D_PRIV_H__
#define BVDC_CM3D_PRIV_H__

#include "bstd.h"
#include "bdbg.h"

#include "bvdc_common_priv.h"

#ifdef __cplusplus
	 extern "C" {
#endif

#define BVDC_P_CM3D_FORMAT_POINTS           2 /* Cr/Cb or Hue/Sat */
#define BVDC_P_MAX_CHIST_HUE_ANGLE      0x2CF /* 359.5 degrees in u9.1 format */

typedef struct
{
	uint32_t    aulRegionConfig[BVDC_MAX_CM3D_REGIONS * BVDC_P_CM3D_FORMAT_POINTS];
	uint32_t    aulPwl[BVDC_MAX_CM3D_REGIONS * BVDC_MAX_CM3D_PWL * BVDC_MAX_CM3D_PWL_POINTS];
	uint32_t    aulPwlInput[BVDC_MAX_CM3D_REGIONS];
	uint32_t    aulGainAdj[BVDC_MAX_CM3D_REGIONS * BVDC_MAX_CM3D_GAIN_CONFIGS];
	uint32_t    aulGainOutput[BVDC_MAX_CM3D_REGIONS];
} BVDC_P_Cm3dRegionSettings;

typedef struct
{
	uint32_t    ulRegionCfg;
	uint32_t    aulPwl[BVDC_MAX_CM3D_OVERLAPPED_PWL * BVDC_MAX_CM3D_OVERLAPPED_PWL_POINTS];
} BVDC_P_Cm3dOvlpRegionSettings;


/***************************************************************************
 * CM3D private data structures
 ***************************************************************************/
typedef struct BVDC_P_Cm3dContext
{
	BDBG_OBJECT(BVDC_C3D)

	/* private fields. */
	BVDC_P_WindowId 			       eId;
	BREG_Handle 				       hReg;
	bool						       bInitial;

	/* indicates if a particular region is enabled or not */
	bool                               abRegionEnable[BVDC_MAX_CM3D_REGIONS];
	bool                               abOvlpRegionEnable[BVDC_MAX_CM3D_OVERLAPPED_REGIONS];

	/* copy of user provided region info */
	BVDC_Cm3dRegion                    astRegion[BVDC_MAX_CM3D_REGIONS];
	BVDC_Cm3dOverlappedRegion          astOvlpRegion[BVDC_MAX_CM3D_OVERLAPPED_REGIONS];

	/* Histogram data */
	BVDC_ChromaStatus                  stChromaHistData;
	BVDC_ChromaHistType                eChromaHistType;
} BVDC_P_Cm3dContext;

BERR_Code BVDC_P_Cm3d_Create
	( BVDC_P_Cm3d_Handle 		      *phCm3d,
	  const BVDC_P_WindowId 		   eWinId,
	  const BREG_Handle 			   hReg );

void BVDC_P_Cm3d_Destroy
	( BVDC_P_Cm3d_Handle 			   hCm3d );

void BVDC_P_Cm3d_Init
	( const BVDC_P_Cm3d_Handle		   hCm3d );

BERR_Code BVDC_P_Cm3d_ValidateRegion
	( const BVDC_Cm3dRegion           *pRegion,
	  uint32_t                         ulRegionId );

BERR_Code BVDC_P_Cm3d_ValidateOverlappedRegion
	( const BVDC_Cm3dOverlappedRegion *pRegion,
	  uint32_t                         ulRegionId );

void BVDC_P_Cm3dStoreRegionInfo
	( BVDC_Window_Handle               hWindow,
 	  const BVDC_Cm3dRegion           *pstRegion,
 	  uint32_t                         ulRegionId );

void BVDC_P_Cm3dStoreOverlappedRegionInfo
	( BVDC_Window_Handle               hWindow,
 	  const BVDC_Cm3dOverlappedRegion *pstRegion,
 	  uint32_t                         ulRegionId );

void BVDC_P_Cm3d_BuildRul_isr
	( const BVDC_Window_Handle         hWindow,
	  BVDC_P_ListInfo                 *pList,
	  bool                             bInitial );

void BVDC_P_Cm3d_GetChromaHistogramData
	( const BVDC_P_Cm3d_Handle		   hCm3d,
	  BVDC_ChromaStatus               *pStatus );
#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_CM3D_PRIV_H__*/

/* End of file. */

