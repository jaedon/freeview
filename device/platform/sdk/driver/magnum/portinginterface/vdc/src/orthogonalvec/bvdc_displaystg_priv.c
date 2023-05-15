/***************************************************************************
 *     Copyright (c) 2011-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_displaystg_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/32 $
 * $brcm_Date: 8/30/12 1:13p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/orthogonalvec/bvdc_displaystg_priv.c $
 * 
 * Hydra_Software_Devel/32   8/30/12 1:13p hongtaoz
 * SW7435-319: disabled SM_ENABLE for now;
 * 
 * Hydra_Software_Devel/31   8/29/12 1:10p vanessah
 * SW7425-3359: merge fnrt change to main
 * 
 * Hydra_Software_Devel/30   8/28/12 3:27p vanessah
 * SW7435-335: temporary fix 7435b0 VDC PI compiling error due to VCE
 * unavailability
 * 
 * Hydra_Software_Devel/29   8/17/12 2:11p vanessah
 * SW7445-8: get the correct interrupt for 7445
 * 
 * Hydra_Software_Devel/28   8/15/12 6:23p tdo
 * SW7445-8: Create Appframework emulation environment for 7445 A0
 *
 * Hydra_Software_Devel/27   8/8/12 4:22p vanessah
 * SW7425-3704: add gfx xcode bchannelchange support
 *
 * Hydra_Software_Devel/26   7/27/12 3:45p vanessah
 * SW7425-3150: adding Bypass Video Processing support
 *
 * Hydra_Software_Devel/25   5/14/12 11:00a vanessah
 * SW7425-3040: program stg timer setting according ignore picture
 *
 * Hydra_Software_Devel/24   5/1/12 3:04p vanessah
 * SW7425-2643: add bar data support
 *
 * Hydra_Software_Devel/23   4/12/12 4:10p vanessah
 * SW7425-2831: add NRT mode definition
 *
 * Hydra_Software_Devel/22   4/6/12 8:16p vanessah
 * SW7425-2831: change polarity due to ignore frame flag
 *
 * Hydra_Software_Devel/21   4/6/12 7:59p vanessah
 * SW7425-2831: change polarity due to ignore frame flag
 *
 * Hydra_Software_Devel/20   3/6/12 6:19p vanessah
 * SW7435-9: change display debug msg
 *
 * Hydra_Software_Devel/19   3/1/12 5:16p vanessah
 * SW7435-9: fix Mbox address error for STG2/3
 *
 * Hydra_Software_Devel/18   2/28/12 12:50p vanessah
 * SW7435-9: update MBox and ViCE ARCSS interrupt address
 *
 * Hydra_Software_Devel/17   2/23/12 11:28a vanessah
 * SW7425-1783: freeze picture id only when nrt ignore picture
 *
 * Hydra_Software_Devel/16   1/30/12 4:07p vanessah
 * SW7425-2239: add  bChannelChange in VCE Mbox
 *
 * Hydra_Software_Devel/15   1/26/12 12:06p vanessah
 * SW7435-9: definition of STG version
 *
 * Hydra_Software_Devel/14   1/24/12 4:38p vanessah
 * SW7435-9: add support NRT on STG2/3
 *
 * Hydra_Software_Devel/13   1/20/12 6:30p vanessah
 * SW7435-9: add STG2/3 support
 *
 * Hydra_Software_Devel/12   1/16/12 11:08a vanessah
 * SW7425-2084: add bLast flag to VCE Mbox
 *
 * Hydra_Software_Devel/11   1/13/12 5:31p vanessah
 * SW7425-2175: add longer RUL delay for NRT transcode
 *
 * Hydra_Software_Devel/10   1/4/12 12:48p vanessah
 * SW7425-9: fix STG compile error for 7435
 *
 * Hydra_Software_Devel/9   1/4/12 11:39a vanessah
 * SW7425-2051: bStallStc propogation change, use CPU write instead of RUL
 *
 * Hydra_Software_Devel/8   12/5/11 5:05p vanessah
 * SW7425-1783: fix master mode stgid error
 *
 * Hydra_Software_Devel/7   12/5/11 3:41p hongtaoz
 * SW7425-1291: SW7425-1783: use MBOX rev ID instead of DCCM rev ID and
 * CASSERT instead of runtime ASSERT;
 *
 * Hydra_Software_Devel/6   12/5/11 2:05p pntruong
 * SW7425-1783: Added the log tag as well.
 *
 ***************************************************************************/
#include "bstd.h"
#include "bdbg.h"
#include "bfmt.h"

#include "bchp.h"
#include "bvdc_compositor_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_common_priv.h"
#include "bvdc_display_priv.h"
#include "bavc_vce_mbox.h"

#if (BVDC_P_SUPPORT_STG)

/* ---------------------------------------------
 * STG version
 * --------------------------------------------- */
/*
 *  7425 A0/A1
 */
#define BVDC_P_STG_VER_1                     (1)

/*
 *  7425 B0/B1/B2
 */
#define BVDC_P_STG_VER_2                     (2)

/*
 *  7435
 */
#define BVDC_P_STG_VER_3                     (3)

/*
 *  7445
 */
#define BVDC_P_STG_VER_4                     (4)

#if (BVDC_P_NUM_SHARED_STG >=4)
#include "bchp_vice2_arcss_ess_dccm_0_0.h"
#include "bchp_vice2_arcss_ess_dccm_0_1.h"
#include "bchp_vice2_arcss_ess_p1_intr2_0_0.h"
#include "bchp_vice2_arcss_ess_p1_intr2_0_1.h"
#else
#include "bchp_vice2_arcss_ess_dccm_0.h"
#include "bchp_vice2_arcss_ess_p1_intr2_0.h"
#endif

BDBG_MODULE(BVDC_DISP);
BDBG_FILE_MODULE(BVDC_DISP_STG);
/*************************************************************************
*  {private}
* BVDC_P_Display_EnableSTGTriggers_isr
*  Re-enable trigger after vec reset.
**************************************************************************/
void BVDC_P_Display_EnableSTGTriggers_isr
( BVDC_Display_Handle              hDisplay,
 bool                             bEnable )
{
	uint32_t ulStgCtrl;
	uint32_t ulRegOffset = 0;

	ulRegOffset = hDisplay->ulStgRegOffset;

	ulStgCtrl   = BREG_Read32_isr(hDisplay->hVdc->hRegister,
		BCHP_VIDEO_ENC_STG_0_CONTROL + ulRegOffset);

	BDBG_MSG(("Display %d %s STG %d trigger: %s", hDisplay->eId, bEnable?"enables":"disables",
		(uint32_t) (hDisplay->eMasterTg - BVDC_DisplayTg_eStg0),
		hDisplay->stCurInfo.bStgNonRealTime? "NRT":"RT"));
	if(!bEnable)
	{
		/*SLAVE_MODE=0, EOP_TRIG_ENABLE=0, TIMER_TRIG_ENABLE=0, SM_ENABLE=0.*/
		ulStgCtrl &= ~(
			BCHP_VIDEO_ENC_STG_0_CONTROL_SLAVE_MODE_MASK |
			BCHP_VIDEO_ENC_STG_0_CONTROL_TIMER_TRIG_ENABLE_MASK |
			BCHP_VIDEO_ENC_STG_0_CONTROL_SM_ENABLE_MASK |
			BCHP_VIDEO_ENC_STG_0_CONTROL_EOP_TRIG_ENABLE_MASK |
			BCHP_VIDEO_ENC_STG_0_CONTROL_HOST_ARM_ENABLE_MASK);

		BREG_Write32(hDisplay->hVdc->hRegister,
			BCHP_VIDEO_ENC_STG_0_CONTROL + ulRegOffset, ulStgCtrl);
	}
	else
	{
		/* non-real time mode, .EOP trigger*/
		if(hDisplay->stStgChan.bStgNonRealTime)
		{

			/* SLAVE_MODE=0, TIMER_TRIG_ENABLE=0, SM_ENABLE=0.*/
			ulStgCtrl &= ~(
				BCHP_MASK(VIDEO_ENC_STG_0_CONTROL, SLAVE_MODE       ) |
				BCHP_MASK(VIDEO_ENC_STG_0_CONTROL, TIMER_TRIG_ENABLE) |
				BCHP_MASK(VIDEO_ENC_STG_0_CONTROL, SM_ENABLE));

			/* EOP_TRIG_ENABLE=1,*/
			ulStgCtrl |=
				BCHP_FIELD_ENUM(VIDEO_ENC_STG_0_CONTROL, EOP_TRIG_ENABLE, ENABLE) |
				BCHP_FIELD_ENUM(VIDEO_ENC_STG_0_CONTROL, HOST_ARM_ENABLE, ENABLE);

			BREG_Write32(hDisplay->hVdc->hRegister,
				BCHP_VIDEO_ENC_STG_0_CONTROL + ulRegOffset, ulStgCtrl);

		}
		/* real time mode */
		else
		{

			/*STG Master Mode Timer trigger */
			/*SLAVE_MODE=0, EOP_TRIG_ENABLE=0, */
			ulStgCtrl &= ~(
				BCHP_MASK(VIDEO_ENC_STG_0_CONTROL, SLAVE_MODE) |
				BCHP_MASK(VIDEO_ENC_STG_0_CONTROL, HOST_ARM_ENABLE) |
				BCHP_MASK(VIDEO_ENC_STG_0_CONTROL, EOP_TRIG_ENABLE));

			/*TIMER_TRIG_ENABLE=1, SM_ENABLE=1.
			  SW7435-319: SM_ENABLE seemed to cause BVB interface isue at VIP. Disable it for now! */
			ulStgCtrl |=
				BCHP_FIELD_ENUM(VIDEO_ENC_STG_0_CONTROL, TIMER_TRIG_ENABLE, ENABLE) |
				BCHP_FIELD_ENUM(VIDEO_ENC_STG_0_CONTROL, SM_ENABLE, DISABLE);

			BREG_Write32(hDisplay->hVdc->hRegister,
				BCHP_VIDEO_ENC_STG_0_CONTROL + ulRegOffset, ulStgCtrl);
		}
	}
}

void BVDC_P_ConnectStgSrc
	(BVDC_Display_Handle             hDisplay,
	 BVDC_P_ListInfo                *pList )
{
	uint32_t ulCmpSrc = 0;
	uint32_t ulRegOffset = 0;

#if (BVDC_P_SUPPORT_STG > 1)
	/* Master mode */
	if(BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg))
		ulRegOffset = (hDisplay->eMasterTg - BVDC_DisplayTg_eStg0) * sizeof(uint32_t);
	else /* Slave mode */
		ulRegOffset = hDisplay->stStgChan.ulStg * sizeof(uint32_t);
#endif


	BDBG_ASSERT(true== hDisplay->hVdc->pFeatures->abAvailCmp[hDisplay->hCompositor->eId]);


	/* Connect STG to source */
	ulCmpSrc = BCHP_VEC_CFG_STG_0_SOURCE_SOURCE_S_0 +
		(hDisplay->hCompositor->eId - BVDC_CompositorId_eCompositor0);

	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_STG_0_SOURCE + ulRegOffset);
	*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VEC_CFG_STG_0_SOURCE, SOURCE, ulCmpSrc);

	return;
}

void BVDC_P_TearDownStgChan
	  (BVDC_Display_Handle             hDisplay,
	   BVDC_P_ListInfo                *pList )
{
	uint32_t  ulRegOffset = 0;

#if (BVDC_P_SUPPORT_STG > 1)
	/* Master mode */
	if(BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg))
		ulRegOffset = (hDisplay->eMasterTg - BVDC_DisplayTg_eStg0) * sizeof(uint32_t);
	else /* Slave mode */
		ulRegOffset = hDisplay->stStgChan.ulStg * sizeof(uint32_t);
#else
	BSTD_UNUSED(hDisplay);
#endif
	/* Disable STG source */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_STG_0_SOURCE + ulRegOffset);
	*pList->pulCurrent++ =
		BCHP_FIELD_ENUM(VEC_CFG_STG_0_SOURCE, SOURCE, DISABLE);

	return;
}

void BVDC_P_SetupStg_isr
	  (uint32_t                       ulRegOffset,
	   BVDC_P_ListInfo               *pList )
{
	/* Reset STG Module*/
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_STG_0 + ulRegOffset);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_STG_0, INIT, 1);
	/* unreset STG Module*/
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_SW_INIT_STG_0 + ulRegOffset);
	*pList->pulCurrent++ = BCHP_FIELD_DATA(VEC_CFG_SW_INIT_STG_0, INIT, 0);

	return;
}

/*************************************************************************
 *  {secret}
 *	BVDC_P_STG_Build_RM_isr
 **************************************************************************/
void BVDC_P_STG_Build_RM_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList  )
{
	/* RM pairs*/
	BVDC_P_DisplayInfo *pCurInfo = &hDisplay->stCurInfo;
	uint32_t           ulFrameTicks =
	    (uint32_t)27000000*BFMT_FREQ_FACTOR/pCurInfo->pFmtInfo->ulVertFreq;
	uint32_t  ulRegOffset = 0;

	ulRegOffset = hDisplay->ulStgRegOffset;

	if(hDisplay->hCompositor->ulDelayCnt)
		return;

	BDBG_ENTER(BVDC_P_STG_Build_RM_isr);

	pCurInfo->ulVertFreq = pCurInfo->pFmtInfo->ulVertFreq;
	if((pCurInfo->bMultiRateAllow) && (!pCurInfo->bFullRate))
	{
		ulFrameTicks = ulFrameTicks*1001/1000;
		pCurInfo->ulVertFreq = pCurInfo->pFmtInfo->ulVertFreq * 1000/1001;
	}
	if(pCurInfo->ulVertFreq != pCurInfo->stRateInfo.ulVertRefreshRate)
	{
		pCurInfo->stRateInfo.ulVertRefreshRate = pCurInfo->ulVertFreq;
		hDisplay->bRateManagerUpdated = true;
	}

	/* --- Setup RM --- */
	*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
	*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_STG_0_FRAME_SIZE + ulRegOffset);
	*pList->pulCurrent++ = ulFrameTicks;

	BDBG_LEAVE(BVDC_P_STG_Build_RM_isr);

	return;
}

void BVDC_P_ProgramStgChan
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	BVDC_P_DisplayInfo *pCurInfo = &hDisplay->stCurInfo;
	const BFMT_VideoInfo		*pFmtInfo;
	uint32_t                    ulStgRegOffset = 0, ulRegOffset=0, ulSrc, ulStgId;

	ulStgRegOffset = hDisplay->ulStgRegOffset;

	ulStgId = hDisplay->stStgChan.ulStg; /* slave mode */

	/* Master mode */
	if(BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg))
		ulStgId = hDisplay->eMasterTg - BVDC_DisplayTg_eStg0;

	BDBG_ENTER(BVDC_P_ProgramStgChan);

	if (!hDisplay->hCompositor->ulDelayCnt)
		pFmtInfo = pCurInfo->pFmtInfo;
	else
		pFmtInfo = hDisplay->pPrevFmtInfo;

	if(hDisplay->stStgChan.bEnable)
	{
		/* Dimension */
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_STG_0_BVB_SIZE + ulStgRegOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VIDEO_ENC_STG_0_BVB_SIZE, HORIZONTAL, pFmtInfo->ulDigitalWidth) |
			BCHP_FIELD_DATA(VIDEO_ENC_STG_0_BVB_SIZE, VERTICAL,   pFmtInfo->ulDigitalHeight>>(pFmtInfo->bInterlaced));

		/* interlace or progressive */
		/* disable both timer and EOP triggers */
		/* TODO: HOST_ARM enable for non-real-time mode */

		/* Frame rate:
		 * to make sure 32-bit math not to overflow, cancel one '0'
		 * TODO: support 1000/1001 frame rate tracking */
		BVDC_P_STG_Build_RM_isr(hDisplay, pList);

		/* TODO: STC_CONTROL for non-real-time*/
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_STG_0_STC_CONTROL + ulStgRegOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(VIDEO_ENC_STG_0_STC_CONTROL, STC_FLAG_ENABLE, ENABLE);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_STG_0_CONTROL + ulStgRegOffset);
		*pList->pulCurrent++ = /* disable both timer and EOP triggers */
#if (BVDC_P_SUPPORT_STG_VER > BVDC_P_STG_VER_1)
			BCHP_FIELD_ENUM(VIDEO_ENC_STG_0_CONTROL, TRIG_MODE, AND) |
#endif
			BCHP_FIELD_DATA(VIDEO_ENC_STG_0_CONTROL, TIMEBASE_SEL, pCurInfo->eTimeBase) |
			BCHP_FIELD_DATA(VIDEO_ENC_STG_0_CONTROL, SLAVE_MODE, !BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg)) |
			BCHP_FIELD_DATA(VIDEO_ENC_STG_0_CONTROL, HOST_ARM_ENABLE, pCurInfo->bStgNonRealTime) |
			BCHP_FIELD_DATA(VIDEO_ENC_STG_0_CONTROL, TIMER_TRIG_ENABLE, !pCurInfo->bStgNonRealTime || hDisplay->hCompositor->bIgnorePicture) |
			BCHP_FIELD_DATA(VIDEO_ENC_STG_0_CONTROL, EOP_TRIG_ENABLE, pCurInfo->bStgNonRealTime) |
			BCHP_FIELD_DATA(VIDEO_ENC_STG_0_CONTROL, SCAN_MODE, pCurInfo->pFmtInfo->bInterlaced);

		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_STG_0_DOWN_SAMP + ulStgRegOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_DATA(VIDEO_ENC_STG_0_DOWN_SAMP, DERING_EN,   !hDisplay->stCurInfo.bBypassVideoProcess) | /* 1 if filter is FILTER_MODE == FILTER3 */
			BCHP_FIELD_ENUM(VIDEO_ENC_STG_0_DOWN_SAMP, FILTER_MODE, FILTER3);
#if (BVDC_P_ORTHOGONAL_VEC_VER >= BVDC_P_ORTHOGONAL_VEC_VER_1)
		if(pCurInfo->bStgNonRealTime){
			ulRegOffset = (hDisplay->hCompositor->eId - BVDC_CompositorId_eCompositor0) * sizeof(uint32_t);
			ulSrc       = BCHP_VEC_CFG_TRIGGER_SEL_0_SRC_STG_0 + ulStgId;

			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VEC_CFG_TRIGGER_SEL_0 + ulRegOffset);
			*pList->pulCurrent++ = BCHP_MASK(VEC_CFG_TRIGGER_SEL_0, OVERRIDE) |
				ulSrc;
		}
#else
		BSTD_UNUSED(ulRegOffset);
		BSTD_UNUSED(ulSrc);
#endif
	}
	BDBG_LEAVE(BVDC_P_ProgramStgChan);
	return;
}

void BVDC_P_ProgrameStgMBox_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )
{
	BVDC_P_DisplayInfo              *pCurInfo = &hDisplay->stCurInfo;
	BVDC_P_DisplayStgChan           *pstChan;
	const BFMT_VideoInfo            *pFmtInfo;
	bool                            bChannelChange = true;
	uint32_t                        ulRegOffset = 0, ulStgId = 0, ulMboxAddr = 0;
	uint32_t                        ulViCE2CoreIdx = 0;
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
	uint32_t                       *pulMboxBase = 0, ulCount = 0;

	BDBG_CASSERT(3 == BAVC_VICE_BVN2VICE_MAJORREVISION_ID);
#else
	BDBG_CASSERT(2 == BAVC_VICE_BVN2VICE_MAJORREVISION_ID);
#endif

	pstChan     = &hDisplay->stStgChan;
	ulRegOffset = hDisplay->ulStgRegOffset;

	/* slave mode */
	ulStgId = pstChan->ulStg;
	/* Master mode */
	if(BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg))
		ulStgId = hDisplay->eMasterTg - BVDC_DisplayTg_eStg0;
	/* one vice core supports two channels */
	ulViCE2CoreIdx = ulStgId >>1;


#ifdef BCHP_VICE2_ARCSS_ESS_DCCM_0_0_DATAi_ARRAY_BASE
	ulMboxAddr = (ulViCE2CoreIdx ? BCHP_VICE2_ARCSS_ESS_DCCM_0_1_DATAi_ARRAY_BASE :
		BCHP_VICE2_ARCSS_ESS_DCCM_0_0_DATAi_ARRAY_BASE)
#else
	ulMboxAddr = BCHP_VICE2_ARCSS_ESS_DCCM_0_DATAi_ARRAY_BASE
#endif
		+ ((ulStgId &1) ?
		BAVC_VICE_MBOX_OFFSET_BVN2VICE_DATA_1_START :
		BAVC_VICE_MBOX_OFFSET_BVN2VICE_DATA_0_START);

	/*MBox struture programing*/
	if(hDisplay->stStgChan.bEnable)
	{

		pstChan->ulMBoxAddr = ulMboxAddr;
		bChannelChange = hDisplay->hCompositor->bChannelChange && hDisplay->hCompositor->bGfxChannelChange;

		if (hDisplay->hCompositor->ulDelayCnt)
		{
			pFmtInfo = hDisplay->pPrevFmtInfo;
			hDisplay->hCompositor->ulDelayCnt--;
		}
		else
			pFmtInfo = pCurInfo->pFmtInfo;
		/* Picture Id freeze only when NRT && bIgnorePicture */
		hDisplay->hCompositor->ulPicId += (!hDisplay->hCompositor->bIgnorePicture) || (!pCurInfo->bStgNonRealTime);
		/* MBox structure DW 0: BVB Pic Size*/
		*pList->pulCurrent++ = BRDC_OP_IMMS_TO_REGS(BAVC_VICE_BVN2VICE_DATA_SIZE);
		*pList->pulCurrent++ = BRDC_REGISTER(ulMboxAddr);
#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
		pulMboxBase = pList->pulCurrent;
#endif
		*pList->pulCurrent++ =
			BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_00_BVB_PIC_SIZE, H_SIZE, pFmtInfo->ulDigitalWidth) |
			BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_00_BVB_PIC_SIZE, V_SIZE, pFmtInfo->ulDigitalHeight>>(pFmtInfo->bInterlaced));

		/* MBox structure DW 1: Sample aspect ratio*/
		/*
		*pList->pulCurrent++ =
		BCHP_FIELD_DATA(MBOX_SAMPLE_ASPECT_RATIO, H_SIZE, hDisplay->hCompositor->iSampleAspectRatioX) |
		BCHP_FIELD_DATA(MBOX_SAMPLE_ASPECT_RATIO, V_SIZE, hDisplay->hCompositor->iSampleAspectRatioY);
		*/
		*pList->pulCurrent++ = hDisplay->hCompositor->ulStgPxlAspRatio_x_y;

		/* MBox structure DW 2 Picture Info*/
		*pList->pulCurrent++ =
			BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_02_PIC_INFO, FRAME_RATE,   pCurInfo->ulVertFreq                     ) |
			BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_02_PIC_INFO, SRC_PIC_TYPE, hDisplay->hCompositor->ePictureType      ) |
			BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_02_PIC_INFO, POLARITY,     eFieldPolarity                           ) |
			BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_02_PIC_INFO, REPEAT,       hDisplay->hCompositor->bPictureRepeatFlag) |
			BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_02_PIC_INFO, LAST,         hDisplay->hCompositor->bLast             ) |
			BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_02_PIC_INFO, CHANNELCHANGE,bChannelChange                           ) |
			BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_02_PIC_INFO, IGNORE,       hDisplay->hCompositor->bIgnorePicture    ) |
			BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_02_PIC_INFO, ACTIVEFORMATDATA, hDisplay->hCompositor->bValidAfd     ) |
			BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_02_PIC_INFO, ACTIVEFORMATDATAMODE, hDisplay->hCompositor->ulAfd     );

		BDBG_MODULE_MSG(BVDC_DISP_STG,("STG display%d stg_id %d mbox %8x:", hDisplay->eId, ulStgId, ulMboxAddr));
		BDBG_MODULE_MSG(BVDC_DISP_STG,("%dx%d%s%d", pFmtInfo->ulDigitalWidth, pFmtInfo->ulDigitalHeight,
			pFmtInfo->bInterlaced? "i" : "p", pCurInfo->ulVertFreq));
		BDBG_MODULE_MSG(BVDC_DISP_STG,("ignore? %d", hDisplay->hCompositor->bIgnorePicture));
		BDBG_MODULE_MSG(BVDC_DISP_STG,("stall STC? %d", hDisplay->hCompositor->bStallStc));
		BDBG_MODULE_MSG(BVDC_DISP_STG,("repeat? %d", hDisplay->hCompositor->bPictureRepeatFlag));
		BDBG_MODULE_MSG(BVDC_DISP_STG,("Last? %d",   hDisplay->hCompositor->bLast));
		BDBG_MODULE_MSG(BVDC_DISP_STG,("ChannelChange? %d gfx %d video %d",   bChannelChange, hDisplay->hCompositor->bGfxChannelChange, hDisplay->hCompositor->bChannelChange));
		BDBG_MODULE_MSG(BVDC_DISP_STG,("PicId %d DecodePicId %d type %d, pol %d, asp ratio 0x%x, origPTS 0x%x, ",
			hDisplay->hCompositor->ulPicId, hDisplay->hCompositor->ulDecodePictureId,
			hDisplay->hCompositor->ePictureType, eFieldPolarity,
			hDisplay->hCompositor->ulStgPxlAspRatio_x_y, hDisplay->hCompositor->ulOrigPTS));
		BDBG_MODULE_MSG(BVDC_DISP_STG, ("bAfd? %d AfdMode %4d BarDataType %d Offset left %4x right %4x", hDisplay->hCompositor->bValidAfd, 
			hDisplay->hCompositor->ulAfd, hDisplay->hCompositor->eBarDataType, hDisplay->hCompositor->ulTopLeftBarValue, hDisplay->hCompositor->ulBotRightBarValue));
		BDBG_MODULE_MSG(BVDC_DISP_STG,("chunkId %u, EOC?%d, preCharge?%d, bLast?%d\n",
			hDisplay->hCompositor->ulChunkId, hDisplay->hCompositor->bEndofChunk, hDisplay->hCompositor->bPreChargePicture, hDisplay->hCompositor->bLast));

		/* MBox structure DW 3: Original PTS*/
		*pList->pulCurrent++ = BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_03_ORIGINAL_PTS, VAL, hDisplay->hCompositor->ulOrigPTS);

		/* MBox structure DW 4 Picture Id*/
		*pList->pulCurrent++ = BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_04_STG_PICTURE_ID, VAL, hDisplay->hCompositor->ulPicId);

		/* MBox structure DW 5 Bar Data */
		*pList->pulCurrent++ =
			BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_05_BARDATA_INFO, BARDATATYPE,      hDisplay->hCompositor->eBarDataType      ) |
			BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_05_BARDATA_INFO, TOPLEFTBARVALUE,  hDisplay->hCompositor->ulTopLeftBarValue ) |
			BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_05_BARDATA_INFO, BOTRIGHTBARVALUE, hDisplay->hCompositor->ulBotRightBarValue);

      /* MBox structure DW 6 FNRT */
      *pList->pulCurrent++ =
         BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_06_FNRT_INFO, ENDOFCHUNK, hDisplay->hCompositor->bEndofChunk            ) |
         BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_06_FNRT_INFO, PRECHARGEPICTURE, hDisplay->hCompositor->bPreChargePicture) |
         BAVC_FIELD_DATA(VICE_BVN2VICE_DWORD_06_FNRT_INFO, CHUNKID, hDisplay->hCompositor->ulChunkId                 );

#if defined(HUMAX_PLATFORM_BASE) && defined(CONFIG_BRCM_PATCH_TRANSCODING_32BIT_TIMESTAMP) /*Timestamp*/
		/* MBox structure zero-fill at end to be scalable for expansion in future; */
		ulCount = pList->pulCurrent - pulMboxBase;
		while(ulCount++ < BAVC_VICE_BVN2VICE_DATA_SIZE) {
			*pList->pulCurrent++ = 0;
		}
#endif
		/*Write to the register
		VICE2_ARCSS_ESS_P1_INTR2_0_ARC_P1_SET */

#if(BVDC_P_SUPPORT_STG_VER >=BVDC_P_STG_VER_3)
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
				*pList->pulCurrent++ = (ulViCE2CoreIdx)?
					BRDC_REGISTER(BCHP_VICE2_ARCSS_ESS_P1_INTR2_0_1_ARC_P1_SET):
					BRDC_REGISTER(BCHP_VICE2_ARCSS_ESS_P1_INTR2_0_0_ARC_P1_SET);
#if(BVDC_P_SUPPORT_STG_VER < BVDC_P_STG_VER_4)
			*pList->pulCurrent++ =	(ulStgId&1)?
			BCHP_MASK(VICE2_ARCSS_ESS_P1_INTR2_0_0_ARC_P1_SET, ARCESS_SOFT0_01_INTR) :
			BCHP_MASK(VICE2_ARCSS_ESS_P1_INTR2_0_0_ARC_P1_SET, ARCESS_SOFT0_00_INTR);
#else
		*pList->pulCurrent++ =	(ulStgId&1)?
		BCHP_MASK(VICE2_ARCSS_ESS_P1_INTR2_0_0_ARC_P1_SET, VIP1_INTR) :
		BCHP_MASK(VICE2_ARCSS_ESS_P1_INTR2_0_0_ARC_P1_SET, VIP0_INTR);
#endif
#else
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VICE2_ARCSS_ESS_P1_INTR2_0_ARC_P1_SET);
		*pList->pulCurrent++ =  ulStgId?
			BCHP_MASK(VICE2_ARCSS_ESS_P1_INTR2_0_ARC_P1_SET, ARCESS_SOFT0_01_INTR) :
			BCHP_MASK(VICE2_ARCSS_ESS_P1_INTR2_0_ARC_P1_SET, ARCESS_SOFT0_00_INTR);
#endif
	}

	/* NRT toggle trigger mode: if ignore frame, use combinational timer trigger to slow down (assume NRT faster than RT);
	   else resume EOP only trigger;
	   TODO: clarify combination trigger behavior; use timer or eop one at a time for now; */
	if(pCurInfo->bStgNonRealTime)
	{
		uint32_t ulStgCtrl;

		/* disable both timer and EOP triggers */
		ulStgCtrl = (
#if (BVDC_P_SUPPORT_STG_VER > BVDC_P_STG_VER_1)
			BCHP_FIELD_ENUM(VIDEO_ENC_STG_0_CONTROL, TRIG_MODE, AND) |
#endif
			BCHP_FIELD_DATA(VIDEO_ENC_STG_0_CONTROL, TIMEBASE_SEL,      pCurInfo->eTimeBase) |
			BCHP_FIELD_DATA(VIDEO_ENC_STG_0_CONTROL, SLAVE_MODE,       !BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg)) |
			BCHP_FIELD_ENUM(VIDEO_ENC_STG_0_CONTROL, HOST_ARM_ENABLE,   ENABLE) |
			BCHP_FIELD_DATA(VIDEO_ENC_STG_0_CONTROL, TIMER_TRIG_ENABLE, hDisplay->hCompositor->bIgnorePicture) |
#if (BVDC_P_SUPPORT_STG_VER == BVDC_P_STG_VER_1)
			BCHP_FIELD_DATA(VIDEO_ENC_STG_0_CONTROL, EOP_TRIG_ENABLE,  !hDisplay->hCompositor->bIgnorePicture) |
#else
			BCHP_FIELD_ENUM(VIDEO_ENC_STG_0_CONTROL, EOP_TRIG_ENABLE,  ENABLE) |
#endif
			BCHP_FIELD_DATA(VIDEO_ENC_STG_0_CONTROL, SCAN_MODE,         pCurInfo->pFmtInfo->bInterlaced));

		/* @@@How to handle B0 And/or logic for ignore picture*/
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_STG_0_CONTROL + ulRegOffset);
		*pList->pulCurrent++ = ulStgCtrl;

#if (BVDC_P_SUPPORT_STG_VER == BVDC_P_STG_VER_1)
		/* Kludge: en/disable timer mode in same RUL seems to avoid extra timer
		 * trigger when switch to EOP mode! */
		if(hDisplay->hCompositor->bIgnorePicture)
		{
			ulStgCtrl &= ~(
				BCHP_MASK(VIDEO_ENC_STG_0_CONTROL, TIMER_TRIG_ENABLE));

			*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
			*pList->pulCurrent++ = BRDC_REGISTER(BCHP_VIDEO_ENC_STG_0_CONTROL + ulRegOffset);
			*pList->pulCurrent++ = ulStgCtrl;
		}
#endif

		BREG_Write32_isr(hDisplay->hVdc->hRegister, BCHP_VIDEO_ENC_STG_0_STC_CONTROL + ulRegOffset,
			BCHP_FIELD_DATA(VIDEO_ENC_STG_0_STC_CONTROL, STC_FLAG_ENABLE, !hDisplay->hCompositor->bStallStc));

		/* used for NRT mode transcode: default true to freeze STC and not get encoded */
		hDisplay->hCompositor->bIgnorePicture = true;
		hDisplay->hCompositor->bStallStc      = true;
		hDisplay->hCompositor->ulOrigPTS      = 0;
	}

	return;
}

void BVDC_P_ResetStgChanInfo
	(BVDC_P_DisplayStgChan            *pstStgChan)
{
	pstStgChan->bStgNonRealTime = false;
	pstStgChan->bEnable         = false;
	pstStgChan->ulMBoxAddr      = 0;
	pstStgChan->ulStg           = BVDC_P_HW_ID_INVALID;

	return;
}

BERR_Code BVDC_P_AllocStgChanResources
	( BVDC_P_Resource_Handle           hResource,
	  BVDC_DisplayId                   eDisplayId,
	  BVDC_P_DisplayStgChan            *pstStgChan)
{
	BERR_Code err;

	err = BVDC_P_Resource_AcquireHwId_isr(hResource, BVDC_P_ResourceType_eStg, 0, eDisplayId, &pstStgChan->ulStg, false);

	if (err)
	{
		BDBG_ERR(("No STG block available for display %d. Check hardware capability.", eDisplayId));
	}

	return BERR_TRACE(err);
}

void BVDC_P_FreeStgChanResources
	( BVDC_P_Resource_Handle           hResource,
	  BVDC_P_DisplayStgChan            *pstStgChan)
{
	if (pstStgChan->ulStg!= BVDC_P_HW_ID_INVALID)
	{
		BVDC_P_Resource_ReleaseHwId_isr(hResource, BVDC_P_ResourceType_eStg, pstStgChan->ulStg);
		pstStgChan->ulStg = BVDC_P_HW_ID_INVALID;
	}

	return;
}

BERR_Code BVDC_P_Display_Validate_Stg_Setting
	( BVDC_Display_Handle              hDisplay )
{
	BVDC_P_DisplayInfo *pNewInfo;
 	BERR_Code err;
	bool      bStgSlave;
	pNewInfo = &hDisplay->stNewInfo;

#if !BVDC_P_SUPPORT_SEAMLESS_ATTACH
	hDisplay->stStgChan.bEnable = pNewInfo->bEnableStg;
	bStgSlave = (!BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg)) &&
				(true == hDisplay->stStgChan.bEnable);

	/* Non Real time can not running @ slave mode */
	if( (true == pNewInfo->bStgNonRealTime) && bStgSlave ) {
		BDBG_ERR(("Slave mode cannot running on Non-real time mode!"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* allocate the stg chan for slave mode */
	if(bStgSlave) {
		err = BVDC_P_AllocStgChanResources(hDisplay->hCompositor->hVdc->hResource, hDisplay->eId, &hDisplay->stStgChan);
		if(BERR_SUCCESS !=err)
			return BERR_TRACE(err);

#if (BVDC_P_SUPPORT_STG > 1)
		hDisplay->ulStgRegOffset = (hDisplay->stStgChan.ulStg) * (BCHP_VIDEO_ENC_STG_1_REG_START - BCHP_VIDEO_ENC_STG_0_REG_START);
#endif
	}
#else
	BSTD_UNUSED(err);
	BSTD_UNUSED(bStgSlave);
#endif

	return BERR_SUCCESS;
}


void BVDC_P_Display_Copy_Stg_Setting
	( BVDC_Display_Handle              hDisplay )
{
	hDisplay->stCurInfo.bEnableStg = hDisplay->stNewInfo.bEnableStg;
	hDisplay->stCurInfo.bStgNonRealTime = hDisplay->stNewInfo.bStgNonRealTime;
	hDisplay->stCurInfo.bBypassVideoProcess = hDisplay->stNewInfo.bBypassVideoProcess;

	if (hDisplay->stCurInfo.bEnableStg)
	{
#if BVDC_P_SUPPORT_SEAMLESS_ATTACH
		/* Reset the Stg slave attachment process state */
		hDisplay->eStgSlaveState = BVDC_P_Slave_eInactive;
#else
		hDisplay->eStgState = BVDC_P_DisplayResource_eCreate;
#endif
	}
	else
	{
		/* Tear down the Stg channel */
		hDisplay->eStgState = BVDC_P_DisplayResource_eDestroy;
	}

	return;
}

void BVDC_P_Display_Apply_Stg_Setting
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList,
	  BAVC_Polarity                    eFieldPolarity )

{
	BVDC_P_DisplayInfo *pCurInfo;
	BVDC_P_DisplayStgChan *pstChan;

	BSTD_UNUSED(eFieldPolarity);

	pCurInfo = &hDisplay->stCurInfo;
	pstChan = &hDisplay->stStgChan;

	if (pCurInfo->bEnableStg)
	{
#if BVDC_P_SUPPORT_SEAMLESS_ATTACH
		switch (hDisplay->eStgSlaveState)
		{
			case BVDC_P_Slave_eInactive:
				/* Acquire Stg core */
				if (BVDC_P_AllocStgChanResources(hDisplay->hVdc->hResource,
					  hDisplay->eId, pstChan, hDisplay->hCompositor->eId))
				{
					BDBG_ERR(("No Stg available "));
					return;
				}

				hDisplay->stStgChan.bEnable = true;

				BVDC_P_ProgramStgChan(hDisplay, pstChan, pList);

				hDisplay->eStgSlaveState = BVDC_P_Slave_eEnable;
				break;

			case BVDC_P_Slave_eEnable:
				BVDC_P_ConnectStgSrc(hDisplay, pstChan, pList);
				hDisplay->eStgSlaveState = BVDC_P_Slave_eConnectSrc;
				break;

			case BVDC_P_Slave_eConnectSrc:
			default:
				hDisplay->eStgSlaveState = BVDC_P_Slave_eAttached;
				hDisplay->stCurInfo.stDirty.stBits.bStgEnable = BVDC_P_CLEAN;
				break;
		}
#else
		BVDC_P_TearDownStgChan(hDisplay, pList);

		/*
		 * 2) Acquire necessary resources and set up the new path
		 */
		BDBG_MSG(("BVDC_P_Display_Apply_VideoFormat Display %d allocates resource for STG chan", hDisplay->eId));
		BVDC_P_SetupStg_isr(hDisplay->ulStgRegOffset, pList);
		BVDC_P_ConnectStgSrc(hDisplay, pList);

		/*
		 * 3) Program the modules to the new format
		 */
		BVDC_P_ProgramStgChan(hDisplay, pList);
		/* When Stg is in slave mode, a format switch has to be done. Even
		 * if the RUL somehow didn't get exeucted, our re-do mechanism will keep
		 * trying. So it is safe to move the state to eActive directly.
		 */
		hDisplay->eStgState = BVDC_P_DisplayResource_eActive;
		hDisplay->stCurInfo.stDirty.stBits.bStgEnable = BVDC_P_CLEAN;
#endif
	}
	else
	{
		/* Reset Stg core, disconnect source.
		 * If Stg is the master timing generator, will this automatically
		 * shut down all the slave paths?
		 */
		switch (hDisplay->eStgState)
		{
			case BVDC_P_DisplayResource_eDestroy:
				BVDC_P_TearDownStgChan(hDisplay, pList);
				hDisplay->stStgChan.bEnable = false;
				hDisplay->eStgState = BVDC_P_DisplayResource_eShuttingdown;
				break;

			case BVDC_P_DisplayResource_eShuttingdown:
				if (pList->bLastExecuted)
				{
					BVDC_P_FreeStgChanResources(hDisplay->hVdc->hResource, pstChan);
					hDisplay->eStgState = BVDC_P_DisplayResource_eInactive;
					hDisplay->stCurInfo.stDirty.stBits.bStgEnable = BVDC_P_CLEAN;
				}
				else
				{
					/* Re-build the teardown RUL */
					BVDC_P_TearDownStgChan(hDisplay, pList);
				}
				break;

			default:
				hDisplay->stCurInfo.stDirty.stBits.bStgEnable = BVDC_P_CLEAN;
				break;
		}
	}

	return;
}



#if (BVDC_P_STG_RUL_DELAY_WORKAROUND)
void BVDC_P_STG_DelayRUL_isr
	( BVDC_Display_Handle              hDisplay,
	  BVDC_P_ListInfo                 *pList)
{
	if (BVDC_P_DISPLAY_USED_STG(hDisplay->eMasterTg) &&
		hDisplay->stCurInfo.bStgNonRealTime )
	{
		/* experiment value */
		*pList->pulCurrent++ = BRDC_OP_REG_TO_REG(400);
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_RDC_timer_data);
		*pList->pulCurrent++ = BRDC_REGISTER(hDisplay->ulScratchDummyAddr);
	}
}
#endif
#endif

