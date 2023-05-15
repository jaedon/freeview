/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bvdc_macrovision_priv.c $
 * $brcm_Revision: Hydra_Software_Devel/65 $
 * $brcm_Date: 8/10/12 3:27p $
 *
 * Module Description:
 *   Private module for Macrovision support. Do not include
 * bvdc_nomacrovision_priv.c, if this file is included in your project.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/modularvec/bvdc_macrovision_priv.c $
 * 
 * Hydra_Software_Devel/65   8/10/12 3:27p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/64   10/22/09 4:25p darnstein
 * SW7405-3132,SW7405-3133,HW7413-238: Updates for PAL-M, PAL-N, and
 * Macrovision.
 *
 * Hydra_Software_Devel/63   6/4/09 12:08p darnstein
 * PR44592,PR54152,PR55140: update analog and digital VEC IT microcode for
 * PAL-G and similar formats. Update corresponding microcontroller start
 * addresses too.
 *
 * Hydra_Software_Devel/62   12/17/08 6:21p darnstein
 * PR50075: For PAL Macrovision TYPE1, TYPE2, TYPE3: enable pseudosync
 * pulses via PCL_0.NEGSYNC_AND_TERM_3=LINE_D, NOT
 * PCL_2.SEC_NEG_SYNC_ENABLE. Per Nutan Raj.
 *
 * Hydra_Software_Devel/61   3/10/08 9:13p darnstein
 * PR25971: first try at DCS for 7400
 *
 * Hydra_Software_Devel/60   10/30/07 7:21p pntruong
 * PR34239: Allow dynamically loading of vec custom timing.
 *
 * Hydra_Software_Devel/59   10/5/07 5:45p darnstein
 * PR35195: these are the same changes I made on the dedicated 7401
 * branch. Three new Macrovision types for PAL.
 *
 * Hydra_Software_Devel/58   6/30/06 5:44p hongtaoz
 * PR22081, PR20395, PR22062, PR22246: added SECAM Macrovision support;
 * fixed PAL_M dvi microcode; fixed PAL_M sub-carrier frequency setting;
 * PAL_NC should use the same microcode as normal PAL; fixed SECAM and
 * various PAL's csc matrices according to BT.470-6 spec; changed SRC
 * filter setting to beneifit higher freq response for HD output on
 * PRIM_VEC; fixed PAL sync tip setting for non-Macrovision build;
 *
 * Hydra_Software_Devel/57   6/23/06 6:33p hongtaoz
 * PR22246: some VEC csc, sync, offset values are incorrect;
 *
 * Hydra_Software_Devel/56   5/19/06 1:47p pntruong
 * PR21560: Global symbols without BVDC prefix in the BVDC module.  Reduce
 * unnecessary global exported symbols.  Tighten const params.
 *
 * Hydra_Software_Devel/55   5/1/06 12:03p hongtaoz
 * PR20403: added triple display Macrovision support for7400;
 *
 * Hydra_Software_Devel/54   4/4/06 1:49p hongtaoz
 * PR20403: add triple displays for 7400;
 *
 * Hydra_Software_Devel/53   3/24/06 1:35p hongtaoz
 * PR19204: new PAL settings of TDAC and sync levels;
 *
 * Hydra_Software_Devel/52   11/29/05 4:58p hongtaoz
 * PR16298, PR16781, PR17804, PR18025, PR18232: implemented 704-sample
 * NTSC analog output; added two more Macrovision control modes; added
 * RGB SCART mode (G channel w/o sync) DAC config for Macrovision
 * validation test;
 *
 * Hydra_Software_Devel/50   9/28/05 6:48p hongtaoz
 * PR16459: fixed PAL macrovision for the new format table-driven code;
 *
 * Hydra_Software_Devel/49   9/22/05 3:55p albertl
 * PR16459:  Changed table lookups to use BVDC_P_Get functions.  Cleaned
 * up switch code and renamed pTable back to pulTable.
 *
 * Hydra_Software_Devel/48   9/22/05 11:02a pntruong
 * PR16459: Fixed macrovision build errors.
 *
 * Hydra_Software_Devel/47   9/21/05 6:27p albertl
 * PR16459:  Display format code restructured, moved to
 * bvdc_displayfmt_priv.c and bvdc_displayfmt.h, and changed to be  table
 * driven  to allow ease of adding new formats.
 *
 * Hydra_Software_Devel/46   9/12/05 2:24p hongtaoz
 * PR16822, PR15749: updated PAL microcode; fixed Macrovision
 * certification control process;
 *
 * Hydra_Software_Devel/45   8/18/05 1:16p pntruong
 * PR15757, PR16391, PR16411, PR12519, PR14791, PR15535, PR15206, PR15778:
 * Improved bandwidth for cropping/scaler/capture/playback.   Unified
 * window shutdown sequence for destroy, reconfigure mad/scaler, and/or
 * reconfigure result of source changes.  And miscellances fixes from
 * above PRs.
 *
 * Hydra_Software_Devel/XVDPhase1/XVDPhase1_merge/1   8/15/05 12:06p hongtaoz
 * PR15749:  added Macrovision support for new test process, i.e. toggling
 * of certain CPC control bits;
 *
 * Hydra_Software_Devel/44   8/14/05 1:16p hongtaoz
 * PR15749: added Macrovision support for new test process, i.e. toggling
 * of certain CPC control bits;
 *
 * Hydra_Software_Devel/43   6/23/05 6:13p hongtaoz
 * PR14720: added Macrovision support for B2;
 *
 * Hydra_Software_Devel/42   4/29/05 10:42a hongtaoz
 * PR15037: AGC process always starts from minimum amplitude when change
 * MV types;
 *
 * Hydra_Software_Devel/41   3/17/05 7:07p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/40   3/17/05 6:39p pntruong
 * PR14494: Add preliminary software support to 3560 A0.
 *
 * Hydra_Software_Devel/39   1/21/05 6:19p hongtaoz
 * PR13887: fixing 576p Macrovision microcode; fixed 480p and PAL
 * Macrovision switch problem; fixed C0 Macrovision RGB output problem;
 *
 * Hydra_Software_Devel/38   12/29/04 2:41p hongtaoz
 * PR13177: added RGBHV output support;
 *
 * Hydra_Software_Devel/37   12/8/04 4:50p hongtaoz
 * PR13206: use vec color matrices to compensate the truncation/rounding
 * errors introduced from different precision compositor csc matrices;
 * re-organized vec matrices for DAC/DVI/656 outputs;
 *
 ***************************************************************************/

#include "bstd.h"          /* standard types */
#include "bvdc_display_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_macrovision.h"
#include "bchp_prim_vf.h"

BDBG_MODULE(BVDC_DISP);

/* number of VECs */
#if BVDC_P_SUPPORT_TER_VEC
#define BVDC_P_VEC_PATH_COUNT                  (3)
#elif BVDC_P_SUPPORT_SEC_VEC
#define BVDC_P_VEC_PATH_COUNT                  (2)
#else
#define BVDC_P_VEC_PATH_COUNT                  (1)
#endif

/* mode control flags */
#define BVDC_P_MACROVISION_CPC0_BP_ON          (1)
#define BVDC_P_MACROVISION_CPC0_PSAGC_ON       (2)

#define BVDC_P_MACROVISION_CPC1_HAMP_ON        (2)
#define BVDC_P_MACROVISION_CPC1_CYCL_ON        (4)
#define BVDC_P_MACROVISION_CPC1_CS_ON          (8)

#define BVDC_P_MACROVISION_CPS5_RGB_ON         (4)

/* AGC amplitude (mv) relative to blank level */
#define BVDC_P_AGC_AMPLITUDE_NTSC_MAX          (813)
#define BVDC_P_AGC_AMPLITUDE_NTSC_MIN          (0)
#define BVDC_P_AGC_AMPLITUDE_480p_MAX          (740)
#define BVDC_P_AGC_AMPLITUDE_480p_MIN          (0)
#define BVDC_P_AGC_AMPLITUDE_PAL_MAX           (790)
#define BVDC_P_AGC_AMPLITUDE_PAL_PULSATION_HI  (450)
#define BVDC_P_AGC_AMPLITUDE_PAL_PULSATION_LOW (100)


#define BVDC_P_IT_TABLE_MV_SIZE \
	((BCHP_PRIM_IT_PCL_5 - BCHP_PRIM_IT_ADDR_0_3)/sizeof(uint32_t)+1)

/***************************************************************************
 * Statics
 ***************************************************************************/
static uint8_t sCpcTable[BVDC_P_MAX_DISPLAY_COUNT][BVDC_CPC_COUNT];
static uint8_t sCpsTable[BVDC_P_MAX_DISPLAY_COUNT][BVDC_CPS_COUNT];

/*************************************************************************
 *  {secret}
 *  Calculates and returns the Agc pulse amplitude value.
 *  Ramping will be achieved with 25 sampling levels. Min 16 required.
 *
 *  SEC_VF_POS_SYNC_VALUES_VALUE0: 1 unit = 286 mV / (240 - 16) = 1.277 mV
 *
 *  NTSC/NTSC_J/PAL_M: min(0)..max(638 = 813 mv / 1.277 mV/unit)
 *  level 2 - 720 fields (max)
 *  level 3 - 150 fields total, 6 per sampling (max..min)
 *  level 0 - 180 fields (min)
 *  level 1 - 150 fields total, 6 per sampling (min..max)
 *
 *  PAL/625: min(78 = 100mV / 1.277 mV/unit)..max(619 = 790 mv / 1.277 mV/unit)
 *  level 2 - 600 fields (max)
 *  level 3 - 124 fields, 2 per sampling (max..min)
 *  level 0 - 614 fields (min)
 *  level 1 - 126 fields, 2 per sampling (min..max)
 *
 *  480p/576p: min(0)..max(579 = 740 mv / 1.277 mV/unit)
 *  level 2 - 780 frames (max)
 *  level 3 - 150 frames, 6 per sampling (max..min)
 *  level 0 - 180 frames (min)
 *  level 1 - 150 frames, 6 per sampling (min..max)
 *************************************************************************/
static uint32_t BVDC_P_GetAgc
(
	BVDC_P_DisplayContext  *pDisplay,
	const BFMT_VideoInfo *pFmtInfo,
	uint32_t      **ppRul,
	BVDC_P_VecPath  eVecPath
)
{
	uint32_t          ulMax;
	uint32_t          ulMin;
	uint32_t          ulAndMask;
	uint32_t          ulOrMask;
	uint32_t          ulLevel=0;
	const uint32_t*   pFramesTbl=NULL;
	const uint32_t    aulFramesTbl_NTSC[] = { 180, 150, 720, 150  };
	const uint32_t    aulFramesTbl_480p[] = { 180, 150, 780, 150 };
	const uint32_t    aulFramesTbl_PAL[]  = { 614, 126, 600, 124  };
	const uint32_t    ulPulsationFrames = 56; /* 1.12 seconds of PAL video */
	/* Store data for Primary & Secondary Vecs.
	   Bypass Vec does not support Macrovision.
	   index = [BVDC_P_eVecPrimary, BVDC_P_eVecSecondary] */
	static uint32_t   ulStep[BVDC_P_VEC_PATH_COUNT] = { 0, };
	static uint32_t   ulState[BVDC_P_VEC_PATH_COUNT] = { 0, };
	static uint32_t   ulFrames[BVDC_P_VEC_PATH_COUNT] = {0,};
	static bool       bPulsationStateX[BVDC_P_VEC_PATH_COUNT] = { false, };

	/* restart AGC process at the following condition */
	if(pDisplay->stCurInfo.eMacrovisionType != pDisplay->stNewInfo.eMacrovisionType)
	{
		ulStep[eVecPath] = 0;
		ulState[eVecPath] = 0;
		ulFrames[eVecPath] = 0;
		bPulsationStateX[eVecPath] = false;
	}
	if (VIDEO_FORMAT_IS_525_LINES(pFmtInfo->eVideoFmt))
	{
		ulMax = BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_AGC_AMPLITUDE_NTSC_MAX);
		ulMin = BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_AGC_AMPLITUDE_NTSC_MIN);
		pFramesTbl = aulFramesTbl_NTSC;
	}
	else if (VIDEO_FORMAT_IS_ED(pFmtInfo->eVideoFmt))
	{
		ulMax = BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_AGC_AMPLITUDE_480p_MAX);
		ulMin = BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_AGC_AMPLITUDE_480p_MIN);
		pFramesTbl = aulFramesTbl_480p;
	}
	else /* PAL/625 */
	{
		ulMax = BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_AGC_AMPLITUDE_PAL_MAX);
		ulMin = BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_AGC_AMPLITUDE_PAL_PULSATION_LOW);
		pFramesTbl = aulFramesTbl_PAL;
	}

	/* support CPC[1:0] = 0xE1 */
	if(!pDisplay->stNewInfo.stN0Bits.bPsAgc)
	{
		return 0;
	}

	/* if N0 indicates static AGC, return max level */
	if(!pDisplay->stNewInfo.stN0Bits.bCycl)
	{
		return ulMax;
	}

	++ulFrames[eVecPath];

	switch(ulState[eVecPath])
	{
	case 2: /* maximum level */
		ulLevel = ulMax;
		break;
	case 3: /* ramp down */
		ulLevel = ulMax - (ulMax - ulMin) * ulFrames[eVecPath] / pFramesTbl[ulState[eVecPath]];
		break;
	case 0: /* minimum level */
		ulLevel = ulMin;

		/* PAL pulsation mode */
		if(VIDEO_FORMAT_IS_625_LINES(pFmtInfo->eVideoFmt))
		{
			const uint32_t *pulTable;
			uint32_t ulStack23Offset = (BCHP_PRIM_IT_STACK_reg_2_3 - BCHP_PRIM_IT_ADDR_0_3)/sizeof(uint32_t);

			pulTable = BVDC_P_GetItTableMv_isr(&pDisplay->stNewInfo);

			if(++ulStep[eVecPath] >= ulPulsationFrames)
			{
				ulStep[eVecPath] = 0;
				bPulsationStateX[eVecPath] ^= true;
			}
			if(bPulsationStateX[eVecPath])
			{
				/* state "X" = 450mV / 1.277 mV/unit */
				ulLevel = BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_AGC_AMPLITUDE_PAL_PULSATION_HI);

				/* RMW Stack #3 of IT block to have lines 8 ~ 13 and 320 ~ 325 to
				   have AGC pulses.
				   Note: IT stack #3 bit 0 corresponds to line 7. */
				ulAndMask = ~BCHP_MASK(PRIM_IT_STACK_reg_2_3, REG_3);
				ulOrMask = BCHP_FIELD_DATA(PRIM_IT_STACK_reg_2_3, REG_3, 0x7E &
						BVDC_P_GET_FIELD(pulTable[ulStack23Offset], PRIM_IT_STACK_reg_2_3, REG_3));
				BVDC_P_RD_MOD_WR_RUL(*ppRul, ulAndMask, ulOrMask, \
					BCHP_PRIM_IT_STACK_reg_2_3 + pDisplay->lItOffset);
			}
			else
			{	/* state "Y" */
				/* RMW Stack #3 of IT block to have lines 8 ~ 16 and 320 ~ 328 to
				   have AGC pulses. */
				ulAndMask = ~BCHP_MASK(PRIM_IT_STACK_reg_2_3, REG_3);
				ulOrMask = BCHP_FIELD_DATA(PRIM_IT_STACK_reg_2_3, REG_3,
						BVDC_P_GET_FIELD(pulTable[ulStack23Offset], PRIM_IT_STACK_reg_2_3, REG_3));
				BVDC_P_RD_MOD_WR_RUL(*ppRul, ulAndMask, ulOrMask, \
					BCHP_PRIM_IT_STACK_reg_2_3 + pDisplay->lItOffset);
			}
			/*BDBG_MSG(("PAL step #%d: state %d", ulStep[eVecPath], bPulsationStateX[eVecPath]));*/
		}
		break;
	case 1: /* ramp up */
		ulLevel = ulMin + (ulMax - ulMin) * ulFrames[eVecPath] / pFramesTbl[ulState[eVecPath]];
		break;
	}
	/*BDBG_MSG(("ulState = %d, Frame #%d, level = %d", ulState[eVecPath], ulFrames[eVecPath], ulLevel));*/
	if(ulFrames[eVecPath] >= pFramesTbl[ulState[eVecPath]])
	{
		ulFrames[eVecPath] = 0;
		ulStep[eVecPath] = 0;
		bPulsationStateX[eVecPath] = false;
		ulState[eVecPath] = (ulState[eVecPath] + 1) % 4;
	}

	return ulLevel;
}

/*************************************************************************
 *  {secret}
 *	Returns SEC_VF_POS_SYNC_VALUES value
 *	The amplitude of the AGC pulses varies with time. The amplitude ramping
 *  must be achieved using a minimum of 16 sampling levels, equally spaced
 *  10%. Same values are used for SVideo/CVBS/component.
 **************************************************************************/
uint32_t BVDC_P_GetPosSyncValue
(
	BVDC_P_DisplayContext  *pDisplay,
	uint32_t              **ppulRul,
	BVDC_P_VecPath          eVecPath
)
{
	uint32_t     ulPosSync=0;
	uint32_t     ulAgc=0;
	BVDC_P_DisplayInfo *pDispInfo = &pDisplay->stCurInfo;

	if (pDispInfo->eMacrovisionType != BVDC_MacrovisionType_eNoProtection)
	{
		/* Get Agc value */
		ulAgc = BVDC_P_GetAgc(pDisplay, pDispInfo->pFmtInfo, ppulRul, eVecPath);

		if((BVDC_P_Output_eYQI   == pDispInfo->eOutputColorSpace) ||
		   (BVDC_P_Output_eYQI_M == pDispInfo->eOutputColorSpace) ||
		   (BVDC_P_Output_eYUV_N == pDispInfo->eOutputColorSpace))
		{
			ulPosSync =
				BCHP_FIELD_DATA(PRIM_VF_POS_SYNC_VALUES, VALUE1,
					BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_NTSC_PEAK_WHITE_LEVEL)) |
				BCHP_FIELD_DATA(PRIM_VF_POS_SYNC_VALUES, VALUE0, ulAgc);
		}
		else
		{
			ulPosSync =
				BCHP_FIELD_DATA(PRIM_VF_POS_SYNC_VALUES, VALUE1,
					BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_PEAK_WHITE_LEVEL)) |
				BCHP_FIELD_DATA(PRIM_VF_POS_SYNC_VALUES, VALUE0, ulAgc);
		}
	}

	return ulPosSync;
}

/*************************************************************************
 *  {secret}
 *	Returns SEC_CO_VF_POS_SYNC_VALUES value
 *	For macrovision, it is the same as the above function.
 **************************************************************************/
uint32_t BVDC_P_GetCoPosSyncValue
(
	BVDC_P_DisplayContext  *pDisplay,
	uint32_t              **ppulRul,
	BVDC_P_VecPath          eVecPath
)
{
	return BVDC_P_GetPosSyncValue (pDisplay, ppulRul, eVecPath);
}

/*************************************************************************
 *  {secret}
 *	Macrovision type change shall not cause flash on screen.
 **************************************************************************/
BERR_Code BVDC_P_ChangeMvType
(
	BVDC_P_DisplayContext  *pDisplay,
	uint32_t              **ppulRul
)
{
	const uint32_t *pulTable;
	uint32_t ulAddr46, ulPcl4;
	uint32_t ulAddr46Offset = (BCHP_PRIM_IT_ADDR_4_6 - BCHP_PRIM_IT_ADDR_0_3)/sizeof(uint32_t);
	uint32_t ulPcl4Offset = (BCHP_PRIM_IT_PCL_4 - BCHP_PRIM_IT_ADDR_0_3)/sizeof(uint32_t);
	uint32_t ulVecOffset = pDisplay->lItOffset;
	BVDC_P_DisplayInfo *pNewInfo = &(pDisplay->stNewInfo);

	if(!VIDEO_FORMAT_SUPPORTS_MACROVISION(pNewInfo->pFmtInfo->eVideoFmt))
	{
		return BERR_SUCCESS;
	}

	pulTable = BVDC_P_GetItTableMv_isr(pNewInfo);

	*(*ppulRul)++ = BRDC_OP_IMMS_TO_REGS(BVDC_P_IT_TABLE_MV_SIZE);
	*(*ppulRul)++ = BRDC_REGISTER(BCHP_PRIM_IT_ADDR_0_3 + ulVecOffset);
	/* Setup IT_ADDR_0_3 to IT_PCL_5 */
	BKNI_Memcpy((void*)(*ppulRul), (void*)pulTable,
	            BVDC_P_IT_TABLE_MV_SIZE * sizeof(uint32_t));

	/* if toggle N0 control bits for CS(MC4/5) or BP(MC6) */
	ulAddr46 = pulTable[ulAddr46Offset];
	ulPcl4   = pulTable[ulPcl4Offset];
	if(!pNewInfo->stN0Bits.bBp)
	{
		ulPcl4 &= ~(BCHP_MASK(PRIM_IT_PCL_4, PSB_AND_TERM_2));
		ulPcl4 |= BCHP_FIELD_ENUM(PRIM_IT_PCL_4, PSB_AND_TERM_2, ZERO);
		*(*ppulRul + ulPcl4Offset) = ulPcl4;
	}

	/* PAL toggling CS only for Test01/02 types, which requires format switch;
	   try to not change PAL MC's address to avoid timing violation; */
	if(!pNewInfo->stN0Bits.bCs && VIDEO_FORMAT_IS_525_LINES(pNewInfo->pFmtInfo->eVideoFmt))
	{
		ulAddr46 &= ~(
			BCHP_MASK(PRIM_IT_ADDR_4_6, MC_4_START_ADDR) |
			BCHP_MASK(PRIM_IT_ADDR_4_6, MC_5_START_ADDR));
		ulAddr46 |= (
			BCHP_FIELD_DATA(PRIM_IT_ADDR_4_6, MC_4_START_ADDR, 0xfd) |
			BCHP_FIELD_DATA(PRIM_IT_ADDR_4_6, MC_5_START_ADDR, 0xfd));

		*(*ppulRul + ulAddr46Offset) = ulAddr46;
	}

	/* move pointer after the adjustment; */
	*ppulRul = (*ppulRul) + BVDC_P_IT_TABLE_MV_SIZE;

	/* update tg_config */
	*(*ppulRul)++ = BRDC_OP_IMM_TO_REG();
	*(*ppulRul)++ = BRDC_REGISTER(BCHP_PRIM_IT_TG_CONFIG + ulVecOffset);
	*(*ppulRul)++ = BVDC_P_GetItConfigMv_isr(pNewInfo);

	return BERR_SUCCESS;
}

/*************************************************************************
 *  {secret}
 *	Returns SEC_VF_NEG_SYNC_VALUES value
 *	The amplitude of the neg sync pulses reduces with MV for NTSC/PAL.
 **************************************************************************/
uint32_t BVDC_P_GetNegSyncValue
(
	BVDC_P_DisplayInfo     *pDispInfo,
	BVDC_P_Output           eOutputColorSpace
)
{
	uint32_t ulValue;

	if((pDispInfo->eMacrovisionType == BVDC_MacrovisionType_eNoProtection) ||
		(!pDispInfo->stN0Bits.bHamp))
	{/* 525/60 CVBS/Svideo-Y outputs use 714/286 picture/sync ratio;
	    525/60 YPbPr/RGB and 625/50 outputs use 700/300 picture/sync ratio */
		if((BVDC_P_Output_eYQI   == eOutputColorSpace) ||
		   (BVDC_P_Output_eYQI_M == eOutputColorSpace) ||
		   (BVDC_P_Output_eYUV_N == eOutputColorSpace))
		{
			ulValue =
				BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE2,
					BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_NTSC_SYNC_LEVEL)) | /* 286 mv */
				BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE1,
					BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_NTSC_SYNC_LEVEL)) | /* 286 mv */
				BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE0, 16);
		}
		else
		{
			ulValue = /* 700mV/300mV pix/sync ratio */
				(BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE2,
					BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL)) | /* 300 mv */
				 BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE1,
				 	BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL)) | /* 300 mv */
				 BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE0, 16));
		}
	}
	else if(BVDC_P_Output_eRGB == eOutputColorSpace)
	{
		ulValue = /* 700mV/300mV pix/sync ratio */
			(BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE2,
				BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL)) | /* 300 mv */
			 BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE1,
			 	BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL)) | /* 300 mv */
			 BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE0, 16));
	}
	else
	{
		if((BVDC_P_Output_eYQI   == eOutputColorSpace) ||
		   (BVDC_P_Output_eYQI_M == eOutputColorSpace) ||
		   (BVDC_P_Output_eYUV_N == eOutputColorSpace))
		{/* 525/60 NTSC/NTSC_J/PAL_N CVBS/Svideo-Y macrovision outputs */
			ulValue =
				BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE2,
					BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_NTSC_SYNC_LEVEL*3/4)) | /* 214 mv */
				BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE1,
					BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_NTSC_SYNC_LEVEL)) | /* 286 mv */
				BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE0, 16);
		}
		else if(VIDEO_FORMAT_IS_625_LINES(pDispInfo->pFmtInfo->eVideoFmt))
		{/* 625/50 macrovision outputs */
			ulValue =
				BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE2,
					BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL*4/5)) | /* 240 mv */
				BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE1,
					BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL)) | /* 300 mv */
				BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE0, 16);
		}
		else
		{/* 525/60/YPbPr/RGB macrovision outputs */
			ulValue =
				BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE2,
					BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL*3/4)) | /* 225 mv */
				BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE1,
					BVDC_P_NEG_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_SYNC_LEVEL)) | /* 300 mv */
				BCHP_FIELD_DATA(PRIM_VF_NEG_SYNC_VALUES, VALUE0, 16);
		}
	}
	return ulValue;
}


/*************************************************************************
 *  {public}
 *  This function is to set the private macrovision table.
 **************************************************************************/
/*************************************************************************
 *	BVDC_Display_SetMacrovisionTable
 *************************************************************************/
BERR_Code BVDC_Display_SetMacrovisionTable
(
	BVDC_Display_Handle              hDisplay,
	const BVDC_CpcTable              pCpcTable,
	const BVDC_CpsTable              pCpsTable
)
{
	int i;

	BDBG_ENTER(("BVDC_Display_SetMacrovisionTable\n"));

	if ((!hDisplay) || (!pCpcTable) || (!pCpsTable))
	{
		BDBG_ERR(("Invalid parameter\n"));
		return BERR_TRACE(BERR_INVALID_PARAMETER);
	}

	/* Bypass path does not support macrovision */
	if (hDisplay->bIsBypass)
	{
		return BVDC_ERR_INVALID_MACROVISON_PATH;
	}

	for(i = 0; i < BVDC_CPC_COUNT; i++ )
	{
		sCpcTable[hDisplay->eId][i] = pCpcTable[i];
	}
	for(i = 0; i < BVDC_CPS_COUNT; i++ )
	{
		sCpsTable[hDisplay->eId][i] = pCpsTable[i];
	}

	BDBG_LEAVE(("BVDC_Display_SetMacrovisionTable\n"));
	return BERR_SUCCESS;
}

/*************************************************************************
 *  {secret}
 *  This function is to validate the macrovision settings. For customized type,
 *  it'll translate the CPC/CPS tables settings into a known type, otherwise
 *  return eror.
 **************************************************************************/
BERR_Code BVDC_P_ValidateMacrovision
(
	BVDC_P_DisplayContext   *pDisplay
)
{
	BDBG_ENTER(BVDC_P_ValidateMacrovision);

	/* Check for valid Macrovision support */
	if ((pDisplay->stNewInfo.eMacrovisionType != BVDC_MacrovisionType_eNoProtection) &&
	    ((!VIDEO_FORMAT_SUPPORTS_MACROVISION(pDisplay->stNewInfo.pFmtInfo->eVideoFmt)) ||
	     (pDisplay->stNewInfo.eOutputColorSpace == BVDC_P_Output_eHsync)))
	{
		return BERR_TRACE(BVDC_ERR_INVALID_MACROVISON_MODE);
	}

	pDisplay->bMvChange = (pDisplay->stNewInfo.eMacrovisionType !=
		pDisplay->stCurInfo.eMacrovisionType) ||
		(pDisplay->ePrevMvType == BVDC_MacrovisionType_eCustomized);

	/* store previous user set type */
	pDisplay->ePrevMvType = pDisplay->stNewInfo.eMacrovisionType;

	if(pDisplay->stNewInfo.eMacrovisionType != BVDC_MacrovisionType_eCustomized)
	{
		goto MACROVISION_VALIDATION_DONE;
	}

	/* check if Back Porch is ON */
	if(sCpcTable[pDisplay->eId][0] & BVDC_P_MACROVISION_CPC0_BP_ON)
	{
		pDisplay->stNewInfo.stN0Bits.bBp = true;
		BDBG_MSG(("Turn on Back Porch"));
	}

	/* check if PS/AGC is ON */
	if(sCpcTable[pDisplay->eId][0] & BVDC_P_MACROVISION_CPC0_PSAGC_ON)
	{
		pDisplay->stNewInfo.stN0Bits.bPsAgc = true;
		BDBG_MSG(("Turn on PS/AGC"));
	}

	/* check if Hamp reduction is ON */
	if(sCpcTable[pDisplay->eId][1] & BVDC_P_MACROVISION_CPC1_HAMP_ON)
	{
		pDisplay->stNewInfo.stN0Bits.bHamp = true;
		BDBG_MSG(("Turn on Hamp"));
	}

	/* check if AGC Cyclic variation is ON */
	if(sCpcTable[pDisplay->eId][1] & BVDC_P_MACROVISION_CPC1_CYCL_ON)
	{
		pDisplay->stNewInfo.stN0Bits.bCycl = true;
		BDBG_MSG(("Turn on CYCL"));
	}

	/* check if Color Stripes are ON */
	if(sCpcTable[pDisplay->eId][1] & BVDC_P_MACROVISION_CPC1_CS_ON)
	{
		pDisplay->stNewInfo.stN0Bits.bCs = true;
		BDBG_MSG(("Turn on Color-Stripes"));
	}

	/* check if RGB are ON */
	if(sCpsTable[pDisplay->eId][5] & BVDC_P_MACROVISION_CPS5_RGB_ON)
	{
		pDisplay->stNewInfo.stN0Bits.bRgb = true;
		BDBG_MSG(("Turn on RGB process"));
	}

	/* Currently, we only look at CPC[1:0] and CPS[8] to determine what MV types
	   are used; for AGC only and AGC + 2/4, we also look at CPS[5] bit 2 to decide
	   if RGB channel should have MV signals. */
	if((sCpcTable[pDisplay->eId][0] == 0) && (sCpcTable[pDisplay->eId][1] == 0))
	{
		pDisplay->stNewInfo.eMacrovisionType =
			BVDC_MacrovisionType_eNoProtection;
	}
	else
	{
		switch(sCpsTable[pDisplay->eId][8])
		{
		case 6: /* NTSC 4 Lines */
			/* check if RGB channels have MV signal */
			if(pDisplay->stNewInfo.stN0Bits.bRgb)
			{
				pDisplay->stNewInfo.eMacrovisionType =
					BVDC_MacrovisionType_eType3_Rgb;
			}
			else
			{
				pDisplay->stNewInfo.eMacrovisionType =
					BVDC_MacrovisionType_eType3;
			}
			break;
		case 0xC: /* NTSC 2 Lines */
			/* check if RGB channels have MV signal */
			if(pDisplay->stNewInfo.stN0Bits.bRgb)
			{
				pDisplay->stNewInfo.eMacrovisionType =
					BVDC_MacrovisionType_eType2_Rgb;
			}
			else
			{
				pDisplay->stNewInfo.eMacrovisionType =
					BVDC_MacrovisionType_eType2;
			}
			break;
		case 7: /* NTSC Test 01 */
			/* TODO: figure out what to put here */
			if(VIDEO_FORMAT_IS_NTSC(pDisplay->stNewInfo.pFmtInfo->eVideoFmt))
			{
				pDisplay->stNewInfo.eMacrovisionType =
					BVDC_MacrovisionType_eTest01;
			}
			else
			{
				return BERR_TRACE(BVDC_ERR_INVALID_MACROVISON_MODE);
			}
			break;
		case 3: /* NTSC Test 02 */
			/* TODO: figure out what to put here */
			if(VIDEO_FORMAT_IS_NTSC(pDisplay->stNewInfo.pFmtInfo->eVideoFmt))
			{
				pDisplay->stNewInfo.eMacrovisionType =
					BVDC_MacrovisionType_eTest02;
			}
			else
			{
				return BERR_TRACE(BVDC_ERR_INVALID_MACROVISON_MODE);
			}
			break;
		case 0xB: /* PAL Test 02 */
			if(VIDEO_FORMAT_IS_PAL(pDisplay->stNewInfo.pFmtInfo->eVideoFmt))
			{
				pDisplay->stNewInfo.eMacrovisionType =
					BVDC_MacrovisionType_eTest02;
			}
			else
			{
				return BERR_TRACE(BVDC_ERR_INVALID_MACROVISON_MODE);
			}
			break;
		default:
			if(VIDEO_FORMAT_IS_PAL(pDisplay->stNewInfo.pFmtInfo->eVideoFmt))
			{
				/* PAL Test 01 */
				if(pDisplay->stNewInfo.stN0Bits.bCs)
				{
					pDisplay->stNewInfo.eMacrovisionType =
						BVDC_MacrovisionType_eTest01;
				}
				else /* AGC only */
				{
					/* check if RGB channels have MV signal */
					if(pDisplay->stNewInfo.stN0Bits.bRgb)
					{
						pDisplay->stNewInfo.eMacrovisionType =
							BVDC_MacrovisionType_eType1_Rgb;
					}
					else
					{
						pDisplay->stNewInfo.eMacrovisionType =
							BVDC_MacrovisionType_eType1;
					}
				}
			}
			else/* NTSC Agc Only */
			{
				/* check if RGB channels have MV signal */
				if(pDisplay->stNewInfo.stN0Bits.bRgb)
				{
					pDisplay->stNewInfo.eMacrovisionType =
						BVDC_MacrovisionType_eType1_Rgb;
				}
				else
				{
					pDisplay->stNewInfo.eMacrovisionType =
						BVDC_MacrovisionType_eType1;
				}
			}
		}
	}
MACROVISION_VALIDATION_DONE:
#if BVDC_P_REV_7038_A0_B0_B1
	/* this is a hardware restriction on pre-C0 chips */
	if(!BVDC_P_MACROVISION_ON_RGB(pDisplay->stNewInfo.eMacrovisionType) &&
		(pDisplay->stNewInfo.eMacrovisionType != BVDC_MacrovisionType_eNoProtection))
	{
		/* only the secondary vec could encounter the problem */
		if((pDisplay->stNewInfo.uiNumDacsOn > 3) &&
		   (pDisplay->stNewInfo.eCoOutputColorSpace == BVDC_P_Output_eRGB))
		{
			BDBG_ERR(("B0 and B1 don't support simultaneous RGB and CVBS|SVideo outputs from the same display if Macrovision is enabled."));
			return BERR_TRACE(BVDC_ERR_INVALID_MACROVISON_MODE);
		}
	}
#endif
	BDBG_LEAVE(BVDC_P_ValidateMacrovision);
	return BERR_SUCCESS;
}


/*************************************************************************
 *  {secret}
 *  BVDC_P_Build_SyncPCL_isr
 *  Build the IT PCL_0 setting for h/v sync outputs;
 **************************************************************************/
void BVDC_P_Vec_Build_SyncPCL_isr
	( BVDC_P_DisplayContext           *pDisplay,
	  BVDC_P_ListInfo                 *pList )
{
	BDBG_ENTER(BVDC_P_Vec_Build_SyncPCL_isr);

	/* PR13177: For DTV application, RGB + Hsync are Dac outputs, Vsync is
	Ext digital output. */
	if(BVDC_P_Display_FindDac(pDisplay, BVDC_DacOutput_eHsync))
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_IT_PCL_0 + pDisplay->lItOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, VBI_DATA_ACTIVE_ENABLE,         ON) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, VBI_DATA_ACTIVE_MUX_SELECT ,    3) |
#if BVDC_P_SUPPORT_EXT_SYNC_PCL_0
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, EXT_VSYNC_ENABLE,               ON) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, EXT_VSYNC_MUX_SELECT,           0) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, EXT_HSYNC_ENABLE,               ON) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, EXT_HSYNC_MUX_SELECT,           0) |
#endif
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_4 ,            ZERO) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_3 ,            ZERO) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_2 ,            ZERO) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_1 ,            ZERO) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_0 ,            ONE) | /* take Hsync only */
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_E_SELECT,           0) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_D_SELECT,           0) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_4_SELECT,           0) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_3_SELECT,           0);
	}
	else /* normal setting */
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_PRIM_IT_PCL_0 + pDisplay->lItOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, VBI_DATA_ACTIVE_ENABLE,         ON) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, VBI_DATA_ACTIVE_MUX_SELECT ,    3) |
#if BVDC_P_SUPPORT_EXT_SYNC_PCL_0
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, EXT_VSYNC_ENABLE,               ON) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, EXT_VSYNC_MUX_SELECT,           0) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, EXT_HSYNC_ENABLE,               ON) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, EXT_HSYNC_MUX_SELECT,           0) |
#endif
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_4 ,            ZERO) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_3 ,            LINE_D) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_2 ,            LINE_A) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_1 ,            LINE_B) |
			BCHP_FIELD_ENUM(PRIM_IT_PCL_0, NEGSYNC_AND_TERM_0 ,            LINE_C) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_E_SELECT,           0) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_D_SELECT,           3) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_4_SELECT,           0) |
			BCHP_FIELD_DATA(PRIM_IT_PCL_0, NEGSYNC_MUX_3_SELECT,           2);
	}

	BDBG_LEAVE(BVDC_P_Vec_Build_SyncPCL_isr);
	return;
}

BVDC_P_Prot_Alt BVDC_P_Get_Prot_Alt (void)
{
	return BVDC_P_PROT_ALT_MV;
}


uint32_t BVDC_P_Display_Modify_SYNC_TRANS_0 (
	BVDC_P_DisplayInfo               *pNewInfo,
	uint32_t                         ulVfSTRegVal)
{
	BSTD_UNUSED (pNewInfo);

	return ulVfSTRegVal;
}

/* End of File */
