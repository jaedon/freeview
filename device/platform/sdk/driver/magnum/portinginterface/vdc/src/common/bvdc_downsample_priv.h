/***************************************************************************
 *     Copyright (c) 2003-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_downsample_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/4 $
 * $brcm_Date: 4/7/10 11:29a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_downsample_priv.h $
 * 
 * Hydra_Software_Devel/4   4/7/10 11:29a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/3   4/5/10 4:05p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/2   10/30/07 7:17p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/1   12/18/06 11:42p pntruong
 * PR22577: Merged back to mainline.
 *
 * Hydra_Software_Devel/Refsw_Devel_3563/1   11/15/06 6:17p albertl
 * PR24587:  Added downsample source for 7440.
 *
 ***************************************************************************/
#ifndef BVDC_DOWNSAMPLE_PRIV_H__
#define BVDC_DOWNSAMPLE_PRIV_H__

#include "bvdc.h"
#include "bvdc_common_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Defines
 ***************************************************************************/

/***************************************************************************
 * Vdec Context
 ***************************************************************************/
typedef enum BVDC_P_DownSampleId
{
	BVDC_P_DownSampleId_eDs0 = 0,
	BVDC_P_DownSampleId_eDs1

} BVDC_P_DownSampleId;


typedef struct BVDC_P_DownSampleContext
{
	BDBG_OBJECT(BVDC_DWS)

	BREG_Handle                    hReg;
	BVDC_Source_Handle             hSource;

	BVDC_P_DownSampleId            eId;

	uint32_t                       ulOffset;

	/* Downstream status. */
	bool                           bVideoDetected;

	/* cache value of the registers */
	uint32_t                       ulBeModeReg;
	uint32_t                       ulFeSyncReg;
	uint32_t                       ulFeSyncDetectReg;

	/* Input Format Auto detection. */
	uint32_t                       ulVideoLostCnt;
	uint32_t                       ulCurAutoFmt;
	const BFMT_VideoInfo          *pPrevFmtInfo;

	BVDC_Compositor_Handle         hSrcCompositor;

} BVDC_P_DownSampleContext;


/***************************************************************************
 * Private function prototypes
 ***************************************************************************/
#if (BVDC_P_SUPPORT_DOWNSAMPLE)
BERR_Code BVDC_P_DownSample_Create
	( BVDC_P_DownSample_Handle        *phDownSample,
	  BVDC_P_DownSampleId              eDownSampleId,
	  BREG_Handle                      hReg,
	  BVDC_Source_Handle               hSource );

void BVDC_P_DownSample_Destroy
	( BVDC_P_DownSample_Handle         hDownSample );

void BVDC_P_DownSample_Init
	( BVDC_P_DownSample_Handle         hDownSample );

void BVDC_P_DownSample_Bringup_isr
	( const BVDC_P_DownSample_Handle   hDownSample );

void BVDC_P_DownSample_BuildRul_isr
	( const BVDC_P_DownSample_Handle   hDownSample,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldId );

void BVDC_P_DownSample_UpdateStatus_isr
	( BVDC_P_DownSample_Handle         hDownSample );

void BVDC_P_DownSample_GetStatus_isr
	( const BVDC_P_DownSample_Handle   hDownSample,
	  bool                            *pbVideoDetected );

void BVDC_P_DownSample_Disconnect
	( BVDC_P_DownSample_Handle         hDownSample );

BERR_Code BVDC_P_DownSample_SetVideoFormat
	( BVDC_P_DownSample_Handle         hDownSample,
	  const BFMT_VideoInfo            *pFmtInfo );

#else
#define BVDC_P_DownSample_Create(phDownSample, eDownSampleId, hReg, hSource)        BDBG_ASSERT(0)
#define BVDC_P_DownSample_Destroy(hDownSample)                                      BDBG_ASSERT(0)
#define BVDC_P_DownSample_Init(hDownSample)                                         BDBG_ASSERT(0)
#define BVDC_P_DownSample_Bringup_isr(hDownSample )                                 BDBG_ASSERT(0)
#define BVDC_P_DownSample_BuildRul_isr(hDownSample, pList, eFieldId )               BDBG_ASSERT(0)
#define BVDC_P_DownSample_UpdateStatus_isr(hDownSample)                             BDBG_ASSERT(0)
#define BVDC_P_DownSample_GetStatus_isr(hDownSample, pbVideoDetected)               BDBG_ASSERT(0)
#define BVDC_P_DownSample_Disconnect(hDownSample)                                   BDBG_ASSERT(0)
#define BVDC_P_DownSample_SetVideoFormat(hDownSample, pFmtInfo)                     BDBG_ASSERT(0)
#endif


#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_DOWNSAMPLE_PRIV_H__ */
/* End of file. */
