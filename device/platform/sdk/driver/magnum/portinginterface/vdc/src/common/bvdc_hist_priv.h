/***************************************************************************
 *     Copyright (c) 2004-2010, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_hist_priv.h $
 * $brcm_Revision: Hydra_Software_Devel/11 $
 * $brcm_Date: 6/18/10 4:20p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/7038/bvdc_hist_priv.h $
 * 
 * Hydra_Software_Devel/11   6/18/10 4:20p rpan
 * SW7400-2763: New BVN resource management.
 * 
 * Hydra_Software_Devel/10   4/7/10 11:31a tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic.  Rename TLA
 *
 * Hydra_Software_Devel/9   4/5/10 4:08p tdo
 * SW3548-2814: Improvements to VDC ulBlackMagic
 *
 * Hydra_Software_Devel/8   1/16/09 4:20p tdo
 * PR46529, PR51020: Use correct current writer node to store histogram
 * and completely freeze APL when source mute
 *
 * Hydra_Software_Devel/7   1/15/09 1:15p tdo
 * PR46529, PR51020: Implement const delay for histogram for dynamic
 * contrast use, and avoid histogram update when source freezed
 *
 * Hydra_Software_Devel/6   7/30/08 11:35a tdo
 * PR45181: Implement New Dynamic Contrast Algorithm
 *
 * Hydra_Software_Devel/5   4/2/08 4:31p tdo
 * PR39333: Correctly programming HIST core
 *
 * Hydra_Software_Devel/4   3/24/08 4:29p tdo
 * PR39333: Passing Hist handle instead of its pointer to follow the
 * standard.  Using the interlace flag from source instead of compositor
 * when setting the size.
 *
 * Hydra_Software_Devel/3   3/21/08 5:08p tdo
 * PR39333: More Hist implementation for 3548
 *
 * Hydra_Software_Devel/2   3/21/08 12:10p tdo
 * PR39333: More implementation of HIST block
 *
 * Hydra_Software_Devel/1   3/19/08 5:29p tdo
 * PR39333: Implement HIST core
 *
 ***************************************************************************/
#ifndef BVDC_HIST_PRIV_H__
#define BVDC_HIST_PRIV_H__

#include "breg_mem.h"      /* Chip register access (memory mapped). */
#include "bvdc_common_priv.h"
#include "bvdc_subrul_priv.h"
#include "bvdc_window_priv.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * {private}
 *
 */
#define BVDC_P_SUPPORT_HIST_VER_0                            (0) /* other */
#define BVDC_P_SUPPORT_HIST_VER_1                            (1) /* 7400, 3563, 7325, 7335 */
#define BVDC_P_SUPPORT_HIST_VER_2                            (2) /* 3548, 3556 */

/*-------------------------------------------------------------------------
 * macro used by Histogram sub-module
 */

#define BVDC_P_Hist_MuxAddr(hHist)   (BCHP_VNET_B_HISTOGRAM_0_SRC + (hHist)->eId * sizeof(uint32_t))

#define BVDC_P_Hist_SetVnet_isr(hHist, ulSrcMuxValue, eVnetPatchMode) \
   BVDC_P_SubRul_SetVnet_isr(&((hHist)->SubRul), ulSrcMuxValue, eVnetPatchMode)
#define BVDC_P_Hist_UnsetVnet_isr(hHist) \
   BVDC_P_SubRul_UnsetVnet_isr(&((hHist)->SubRul))

/*
 * This structure contains the actual HW value representing the number of
 * histogram bins and the actual number of histogram bins
 */
typedef struct BVDC_P_Hist_NumBins
{
	uint32_t                        ulHwNumBin;
	uint32_t                        ulHistSize;
} BVDC_P_Hist_NumBins;

/*-------------------------------------------------------------------------
 * Histogram main context
 */
typedef struct BVDC_P_HistContext
{
	BDBG_OBJECT(BVDC_HST)

	/* Hist Id */
	BVDC_P_HistId                    eId;
	uint32_t                         ulRegOffset;

	/* static info from creating */
	BREG_Handle                      hRegister;

	/* Which window it connect to */
	BVDC_Window_Handle               hWindow;

	/* sub-struct to manage vnet and rul build opreations */
	BVDC_P_SubRulContext             SubRul;

	bool                             bInitial;

	/* Histogram data */
	BVDC_LumaStatus                  stHistData;
	uint32_t                         ulHistSize;

	/* Freezed histogram data */
	BVDC_LumaStatus                  stFreezedHistData;

} BVDC_P_HistContext;


/***************************************************************************
 * private functions
***************************************************************************/
/***************************************************************************
 * {private}
 *
 * BVDC_P_Hist_Create
 *
 * called by BVDC_Open only
 */
BERR_Code BVDC_P_Hist_Create
	( BVDC_P_Hist_Handle               *phHist,
	  BVDC_P_HistId                     eHistId,
	  BREG_Handle                       hRegister,
	  BVDC_P_Resource_Handle            hResource );

/***************************************************************************
 * {private}
 *
 * BVDC_P_Hist_Destroy
 *
 * called by BVDC_Close only
 */
BERR_Code BVDC_P_Hist_Destroy
	( BVDC_P_Hist_Handle                hHist );

/***************************************************************************
 * {private}
 *
 * BVDC_P_Hist_AcquireConnect
 *
 * It is called by BVDC_Window_Validate after changing from diabling Hist to
 * enabling Hist.
 */
BERR_Code BVDC_P_Hist_AcquireConnect
	( BVDC_P_Hist_Handle                hHist,
	  BVDC_Window_Handle                hWindow);

/***************************************************************************
 * {private}
 *
 * BVDC_P_Hist_ReleaseConnect_isr
 *
 * It is called after window decided that Histogram is no-longer used by HW in
 * its vnet mode (i.e. it is really shut down and teared off from vnet).
 */
BERR_Code BVDC_P_Hist_ReleaseConnect_isr
	( BVDC_P_Hist_Handle               *phHist );

/***************************************************************************
 * {private}
 *
 * BVDC_P_Hist_BuildRul_isr
 *
 * called by BVDC_Window_BuildRul_isr at every src vsync. It builds RUL for
 * histogram HW module.
 */
void BVDC_P_Hist_BuildRul_isr
	( BVDC_P_Hist_Handle                hHist,
	  BVDC_P_ListInfo                  *pList,
	  BVDC_P_State                      eVnetState,
	  BVDC_P_PicComRulInfo             *pPicComRulInfo );

/***************************************************************************
 * {private}
 *
 * BVDC_P_Hist_UpdateHistData_isr
 *
 * called by BVDC_Window_Writer_isr at every src vsync. It samples the
 * Histogram registers.
 */
void BVDC_P_Hist_UpdateHistData_isr
	( BVDC_P_Hist_Handle                hHist );

/***************************************************************************
 * {private}
 *
 * BVDC_P_Hist_GetHistogramData
 *
 * called by BVDC_Window_GetLumaStatus. It returns the histogram data
 * collected by the HIST block.
 */
void BVDC_P_Hist_GetHistogramData
	( const BVDC_Window_Handle          hWindow,
	  BVDC_LumaStatus                  *pLumaStatus );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef BVDC_HIST_PRIV_H__ */
/* End of file. */
