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
 * $brcm_Revision: Hydra_Software_Devel/15 $
 * $brcm_Date: 8/10/12 3:28p $
 *
 * Module Description:
 *   Private module for Macrovision support. Do not include
 * bvdc_nomacrovision_priv.c, if this file is included in your project.
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/vdc/src/orthogonalvec/bvdc_macrovision_priv.c $
 * 
 * Hydra_Software_Devel/15   8/10/12 3:28p pntruong
 * SW7435-276: Updated NTSC/480p to 720x480i/720x480p respectively.
 * Added support 482i/483p with new enums.
 *
 * Hydra_Software_Devel/14   6/29/12 12:28p darnstein
 * HW7552-167: Support the VAMP on/off bit in CPC array for custom
 * Macrovision.
 *
 * Hydra_Software_Devel/13   3/8/12 12:05p yuxiaz
 * SW7425-2546: Fixed warning msg.
 *
 * Hydra_Software_Devel/12   1/11/12 3:07p darnstein
 * SW7125-1124: merge DCS Hybrid+ to main branch. DCS almost ready for
 * production.
 *
 * Hydra_Software_Devel/11   11/18/11 4:21p darnstein
 * SW7425-1658: prevent possible sync reduction for 480P and 576P. I did
 * this mainly to prevent confusion.
 *
 * Hydra_Software_Devel/SW7125-1124/2   12/1/11 4:22p darnstein
 * SW7125-1124: remove unnecessary functions.
 *
 * Hydra_Software_Devel/SW7125-1124/1   11/10/11 7:09p darnstein
 * SW7125-1124: partial implementation of DCS Hybrid+.
 *
 * Hydra_Software_Devel/9   11/4/11 3:49p darnstein
 * SW7425-1658: program the new register VF_n_NEG_SYNC_AMPLITUDE_EXTN.
 *
 * Hydra_Software_Devel/8   8/26/10 5:29p tdo
 * SW7422-57: Add simple vdc support
 *
 * Hydra_Software_Devel/7   5/7/10 7:23p albertl
 * SW7125-364: Changed dirty bits to use union structure to avoid type-pun
 * warnings
 *
 * Hydra_Software_Devel/6   10/23/09 6:49p darnstein
 * SW7405-3132,SW7405-3133,HW7413-238,SW7340-45: Updates for PAL-M, PAL-N,
 * and Macrovision. Fix small error in NTSC/Macrovision_TEST0?.
 *
 * Hydra_Software_Devel/5   6/17/09 6:30p darnstein
 * PR44592,PR54152,PR55140: remove some obsolete symbols.
 *
 * Hydra_Software_Devel/4   6/2/09 3:22p darnstein
 * PR48884: More resistance to invalid user inputs.
 *
 * Hydra_Software_Devel/3   3/16/09 10:57p tdo
 * PR45785, PR45789: Merge from MCVP branch
 *
 * Hydra_Software_Devel/7420_mcvp/4   3/13/09 5:52p darnstein
 * PR51527: Macrovision working now, but it reloads microcode for each
 * change. So more work remains.
 *
 * Hydra_Software_Devel/7420_mcvp/3   3/11/09 3:31p darnstein
 * PR51527: Disable Macrovision API functions for now. They cause bad
 * video timing.
 *
 * Hydra_Software_Devel/7420_mcvp/2   3/10/09 12:30p darnstein
 * PR51527: Macrovision API functions set bTiming dirty bit.
 *
 * Hydra_Software_Devel/7420_mcvp/1   3/5/09 9:13p darnstein
 * PR51527: Eliminate unused state variables eVbiPath and eVecPath. The
 * DCS files compile now, although they need a lot of work to acutally
 * function.
 *
 * Hydra_Software_Devel/2   12/17/08 4:40p rpan
 * PR45804: Return valid neg vsync value.
 *
 * Hydra_Software_Devel/1   11/4/08 9:56a tdo
 * PR45804: Implement and Bringup Orthorgonal VEC
 *
 ***************************************************************************/

#include "bstd.h"          /* standard types */
#include "bvdc_display_priv.h"
#include "bvdc_displayfmt_priv.h"
#include "bvdc_macrovision.h"
#include "bchp_vf_0.h"

BDBG_MODULE(BVDC_DISP);

/* number of IT blocks */
/* TODO: refine this */
/*
#if BVDC_P_SUPPORT_TER_VEC
#define BVDC_P_VEC_PATH_COUNT                  (3)
#elif BVDC_P_SUPPORT_SEC_VEC
#define BVDC_P_VEC_PATH_COUNT                  (2)
#else
#define BVDC_P_VEC_PATH_COUNT                  (1)
#endif
*/
#define BVDC_P_IT_COUNT (3)

/* mode control flags */
#define BVDC_P_MACROVISION_CPC0_BP_ON          (1)
#define BVDC_P_MACROVISION_CPC0_PSAGC_ON       (2)

#define BVDC_P_MACROVISION_CPC1_VAMP_ON        (1)
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
	uint32_t      **ppRul
)
{
	uint32_t          ulMax;
	uint32_t          ulMin;
	uint32_t          ulAndMask;
	uint32_t          ulOrMask;
	uint32_t          ulIt;
	uint32_t          ulItOffset;
	uint32_t          ulLevel=0;
	const uint32_t*   pFramesTbl=NULL;
	const uint32_t    aulFramesTbl_NTSC[] = { 180, 150, 720, 150  };
	const uint32_t    aulFramesTbl_480p[] = { 180, 150, 780, 150 };
	const uint32_t    aulFramesTbl_PAL[]  = { 614, 126, 600, 124  };
	const uint32_t    ulPulsationFrames = 56; /* 1.12 seconds of PAL video */
	/* Store data for Primary & Secondary Vecs.
	   Bypass Vec does not support Macrovision.
	   index = [BVDC_P_eVecPrimary, BVDC_P_eVecSecondary] */
	static uint32_t   ulStep[BVDC_P_IT_COUNT] = { 0, };
	static uint32_t   ulState[BVDC_P_IT_COUNT] = { 0, };
	static uint32_t   ulFrames[BVDC_P_IT_COUNT] = {0,};
	static bool       bPulsationStateX[BVDC_P_IT_COUNT] = { false, };

	switch(pDisplay->eMasterTg)
	{
	case BVDC_DisplayTg_ePrimIt:
	case BVDC_DisplayTg_eSecIt:
	case BVDC_DisplayTg_eTertIt:
		ulIt = pDisplay->stAnlgChan_0.ulIt;
		BDBG_ASSERT(ulIt != BVDC_P_HW_ID_INVALID);
		ulItOffset = ulIt * BVDC_P_VEC_IT_SIZE;
		break;
	default:
		ulIt = 0;
		ulItOffset = 0;
		BDBG_ASSERT(0);
		break;
	}

	/* restart AGC process at the following condition */
	if(pDisplay->stCurInfo.eMacrovisionType != pDisplay->stNewInfo.eMacrovisionType)
	{
		ulStep[ulIt] = 0;
		ulState[ulIt] = 0;
		ulFrames[ulIt] = 0;
		bPulsationStateX[ulIt] = false;
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

	++ulFrames[ulIt];

	switch(ulState[ulIt])
	{
	case 2: /* maximum level */
		ulLevel = ulMax;
		break;
	case 3: /* ramp down */
		ulLevel = ulMax - (ulMax - ulMin) * ulFrames[ulIt] / pFramesTbl[ulState[ulIt]];
		break;
	case 0: /* minimum level */
		ulLevel = ulMin;

		/* PAL pulsation mode */
		if(VIDEO_FORMAT_IS_625_LINES(pFmtInfo->eVideoFmt))
		{
			const uint32_t *pulTable;
			uint32_t ulStack23Offset = (BCHP_IT_0_STACK_reg_2_3 - BCHP_IT_0_ADDR_0_3)/sizeof(uint32_t);

			pulTable = BVDC_P_GetItTableMv_isr(&pDisplay->stNewInfo);
			BDBG_ASSERT (pulTable);

			if(++ulStep[ulIt] >= ulPulsationFrames)
			{
				ulStep[ulIt] = 0;
				bPulsationStateX[ulIt] ^= true;
			}
			if(bPulsationStateX[ulIt])
			{
				/* state "X" = 450mV / 1.277 mV/unit */
				ulLevel = BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_AGC_AMPLITUDE_PAL_PULSATION_HI);

				/* RMW Stack #3 of IT block to have lines 8 ~ 13 and 320 ~ 325 to
				   have AGC pulses.
				   Note: IT stack #3 bit 0 corresponds to line 7. */
				ulAndMask = ~BCHP_MASK(IT_0_STACK_reg_2_3, REG_3);
				ulOrMask = BCHP_FIELD_DATA(IT_0_STACK_reg_2_3, REG_3, 0x7E &
						BVDC_P_GET_FIELD(pulTable[ulStack23Offset], IT_0_STACK_reg_2_3, REG_3));
				BVDC_P_RD_MOD_WR_RUL(*ppRul, ulAndMask, ulOrMask, \
					BCHP_IT_0_STACK_reg_2_3 + ulItOffset);
			}
			else
			{	/* state "Y" */
				/* RMW Stack #3 of IT block to have lines 8 ~ 16 and 320 ~ 328 to
				   have AGC pulses. */
				ulAndMask = ~BCHP_MASK(IT_0_STACK_reg_2_3, REG_3);
				ulOrMask = BCHP_FIELD_DATA(IT_0_STACK_reg_2_3, REG_3,
						BVDC_P_GET_FIELD(pulTable[ulStack23Offset], IT_0_STACK_reg_2_3, REG_3));
				BVDC_P_RD_MOD_WR_RUL(*ppRul, ulAndMask, ulOrMask, \
					BCHP_IT_0_STACK_reg_2_3 + ulItOffset);
			}
			/*BDBG_MSG(("PAL step #%d: state %d", ulStep[ulIt], bPulsationStateX[ulIt]));*/
		}
		break;
	case 1: /* ramp up */
		ulLevel = ulMin + (ulMax - ulMin) * ulFrames[ulIt] / pFramesTbl[ulState[ulIt]];
		break;
	}
	/*BDBG_MSG(("ulState = %d, Frame #%d, level = %d", ulState[ulIt], ulFrames[ulIt], ulLevel));*/
	if(ulFrames[ulIt] >= pFramesTbl[ulState[ulIt]])
	{
		ulFrames[ulIt] = 0;
		ulStep[ulIt] = 0;
		bPulsationStateX[ulIt] = false;
		ulState[ulIt] = (ulState[ulIt] + 1) % 4;
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
	uint32_t              **ppulRul
)
{
	uint32_t     ulPosSync=0;
	uint32_t     ulAgc=0;
	BVDC_P_DisplayInfo *pDispInfo = &pDisplay->stCurInfo;

	if (pDispInfo->eMacrovisionType != BVDC_MacrovisionType_eNoProtection)
	{
		/* Get Agc value */
		ulAgc = BVDC_P_GetAgc(pDisplay, pDispInfo->pFmtInfo, ppulRul);

		if((BVDC_P_Output_eYQI   == pDispInfo->eAnlg_0_OutputColorSpace) ||
		   (BVDC_P_Output_eYQI_M == pDispInfo->eAnlg_0_OutputColorSpace) ||
		   (BVDC_P_Output_eYUV_N == pDispInfo->eAnlg_0_OutputColorSpace))
		{
			ulPosSync =
				BCHP_FIELD_DATA(VF_0_POS_SYNC_VALUES, VALUE1,
					BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_NTSC_PEAK_WHITE_LEVEL)) |
				BCHP_FIELD_DATA(VF_0_POS_SYNC_VALUES, VALUE0, ulAgc);
		}
		else
		{
			ulPosSync =
				BCHP_FIELD_DATA(VF_0_POS_SYNC_VALUES, VALUE1,
					BVDC_P_POS_SYNC_AMPLITUDE_VALUE(BVDC_P_DAC_OUTPUT_PEAK_WHITE_LEVEL)) |
				BCHP_FIELD_DATA(VF_0_POS_SYNC_VALUES, VALUE0, ulAgc);
		}
	}

	return ulPosSync;
}


/*************************************************************************
 *  {secret}
 *	Returns SEC_VF_NEG_SYNC_VALUES value
 *	The amplitude of the neg sync pulses reduces with MV for NTSC/PAL.
 **************************************************************************/
void BVDC_P_GetNegSyncValue
	( BVDC_P_DisplayInfo              *pDispInfo,
	  BVDC_P_Output                    eOutputColorSpace,
	  uint32_t*                        ulRegVal,
	  uint32_t*                        ulRegValEx)
{
	bool bHamp, bVamp;
	uint32_t ulValue1, ulValue2;
	uint32_t ulValue;
	uint32_t ulValueEx;
	static const uint32_t ulValue0 = 16;

	/* Without Macrovision, values depend only on colorspace. */
	/* 525/60 CVBS/Svideo-Y outputs use 714/286 picture/sync ratio;
	   525/60 YPbPr/RGB and 625/50 outputs use 700/300 picture/sync ratio */
	if((BVDC_P_Output_eYQI	 == eOutputColorSpace) ||
	   (BVDC_P_Output_eYQI_M == eOutputColorSpace) ||
	   (BVDC_P_Output_eYUV_N == eOutputColorSpace))
	{
		ulValue1 = BVDC_P_DAC_OUTPUT_NTSC_SYNC_LEVEL; /* 286 mv */
	}
	else
	{
		ulValue1 = BVDC_P_DAC_OUTPUT_SYNC_LEVEL;      /* 300 mv */
	}
	ulValue2 = ulValue1;

	/* Refine the sync reduction Macrovision features */
	bHamp = bVamp = (
		(pDispInfo->eMacrovisionType != BVDC_MacrovisionType_eNoProtection) &&
		VIDEO_FORMAT_IS_SD (pDispInfo->pFmtInfo->eVideoFmt) &&
		(BVDC_P_Output_eRGB != eOutputColorSpace));
	bHamp = bHamp && pDispInfo->stN0Bits.bHamp;
	bVamp = bVamp && pDispInfo->stN0Bits.bVamp;

	/* Now we can apply sync reduction Macrovision features */
	if (VIDEO_FORMAT_IS_625_LINES(pDispInfo->pFmtInfo->eVideoFmt))
	{
		/* PAL reduction is 4/5 */
		if (bHamp)
		{
			ulValue2 *= 4;
			ulValue2 /= 5;
		}
		if (bVamp)
		{
			ulValue1 *= 4;
			ulValue1 /= 5;
		}
	}
	else
	{
		/* NTSC reduction is 3/4 */
		if (bHamp)
		{
			ulValue2 *= 3;
			ulValue2 /= 4;
		}
		if (bVamp)
		{
			ulValue1 *= 3;
			ulValue1 /= 4;
		}
	}

	/* Convert from voltage to register bits */
	ulValue1 = BVDC_P_NEG_SYNC_AMPLITUDE_VALUE (ulValue1);
	ulValue2 = BVDC_P_NEG_SYNC_AMPLITUDE_VALUE (ulValue2);

	/* Format for hardware registers */
#if (BVDC_P_SUPPORT_VEC_VF_VER < 2)
	ulValue =
		BCHP_FIELD_DATA(VF_0_NEG_SYNC_VALUES, VALUE2, ulValue2) |
		BCHP_FIELD_DATA(VF_0_NEG_SYNC_VALUES, VALUE1, ulValue1) |
		BCHP_FIELD_DATA(VF_0_NEG_SYNC_VALUES, VALUE0, ulValue0) ;
	ulValueEx = 0;
#else
	ulValue =
		BCHP_FIELD_DATA(VF_0_NEG_SYNC_VALUES, VALUE1, ulValue1) |
		BCHP_FIELD_DATA(VF_0_NEG_SYNC_VALUES, VALUE0, ulValue0) ;
	ulValueEx =
		BCHP_FIELD_DATA(VF_0_NEG_SYNC_AMPLITUDE_EXTN, VALUE2, ulValue2);
#endif

	/* Return computed values */
	*ulRegVal   = ulValue;
	*ulRegValEx = ulValueEx;
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

	BDBG_ENTER(("BVDC_Display_SetMacrovisionTable"));

	if ((!hDisplay) || (!pCpcTable) || (!pCpsTable))
	{
		BDBG_ERR(("Invalid parameter"));
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

	/* Activate the VEC timing event handler. This will do the real work. */
	hDisplay->stNewInfo.stDirty.stBits.bTiming = BVDC_P_DIRTY;
	hDisplay->stNewInfo.stDirty.stBits.bDacSetting = BVDC_P_DIRTY;

	BDBG_LEAVE(("BVDC_Display_SetMacrovisionTable"));
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
	     (pDisplay->stNewInfo.eAnlg_0_OutputColorSpace == BVDC_P_Output_eHsync)))
	{
		return BERR_TRACE(BVDC_ERR_INVALID_MACROVISON_MODE);
	}

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

	/* check if Vamp reduction is ON */
	if(sCpcTable[pDisplay->eId][1] & BVDC_P_MACROVISION_CPC1_VAMP_ON)
	{
		pDisplay->stNewInfo.stN0Bits.bVamp = true;
		BDBG_MSG(("Turn on Vamp"));
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
	uint32_t ulIt;
	uint32_t ulItOffset;

	BDBG_ENTER(BVDC_P_Vec_Build_SyncPCL_isr);

	switch(pDisplay->eMasterTg)
	{
	case BVDC_DisplayTg_ePrimIt:
	case BVDC_DisplayTg_eSecIt:
	case BVDC_DisplayTg_eTertIt:
		ulIt = pDisplay->stAnlgChan_0.ulIt;
		BDBG_ASSERT(ulIt != BVDC_P_HW_ID_INVALID);
		ulItOffset = ulIt * BVDC_P_VEC_IT_SIZE;
		break;
	default:
		ulIt = 0;
		ulItOffset = 0;
		BDBG_ASSERT(0);
		break;
	}

	/* PR13177: For DTV application, RGB + Hsync are Dac outputs, Vsync is
	Ext digital output. */
	if(BVDC_P_Display_FindDac(pDisplay, BVDC_DacOutput_eHsync))
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_IT_0_PCL_0 + ulItOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(IT_0_PCL_0, VBI_DATA_ACTIVE_ENABLE,         ON) |
			BCHP_FIELD_DATA(IT_0_PCL_0, VBI_DATA_ACTIVE_MUX_SELECT ,    3) |
#if BVDC_P_SUPPORT_EXT_SYNC_PCL_0
			BCHP_FIELD_ENUM(IT_0_PCL_0, EXT_VSYNC_ENABLE,               ON) |
			BCHP_FIELD_DATA(IT_0_PCL_0, EXT_VSYNC_MUX_SELECT,           0) |
			BCHP_FIELD_ENUM(IT_0_PCL_0, EXT_HSYNC_ENABLE,               ON) |
			BCHP_FIELD_DATA(IT_0_PCL_0, EXT_HSYNC_MUX_SELECT,           0) |
#endif
			BCHP_FIELD_ENUM(IT_0_PCL_0, NEGSYNC_AND_TERM_4 ,            ZERO) |
			BCHP_FIELD_ENUM(IT_0_PCL_0, NEGSYNC_AND_TERM_3 ,            ZERO) |
			BCHP_FIELD_ENUM(IT_0_PCL_0, NEGSYNC_AND_TERM_2 ,            ZERO) |
			BCHP_FIELD_ENUM(IT_0_PCL_0, NEGSYNC_AND_TERM_1 ,            ZERO) |
			BCHP_FIELD_ENUM(IT_0_PCL_0, NEGSYNC_AND_TERM_0 ,            ONE) | /* take Hsync only */
			BCHP_FIELD_DATA(IT_0_PCL_0, NEGSYNC_MUX_E_SELECT,           0) |
			BCHP_FIELD_DATA(IT_0_PCL_0, NEGSYNC_MUX_D_SELECT,           0) |
			BCHP_FIELD_DATA(IT_0_PCL_0, NEGSYNC_MUX_4_SELECT,           0) |
			BCHP_FIELD_DATA(IT_0_PCL_0, NEGSYNC_MUX_3_SELECT,           0);
	}
	else /* normal setting */
	{
		*pList->pulCurrent++ = BRDC_OP_IMM_TO_REG();
		*pList->pulCurrent++ = BRDC_REGISTER(BCHP_IT_0_PCL_0 + ulItOffset);
		*pList->pulCurrent++ =
			BCHP_FIELD_ENUM(IT_0_PCL_0, VBI_DATA_ACTIVE_ENABLE,         ON) |
			BCHP_FIELD_DATA(IT_0_PCL_0, VBI_DATA_ACTIVE_MUX_SELECT ,    3) |
#if BVDC_P_SUPPORT_EXT_SYNC_PCL_0
			BCHP_FIELD_ENUM(IT_0_PCL_0, EXT_VSYNC_ENABLE,               ON) |
			BCHP_FIELD_DATA(IT_0_PCL_0, EXT_VSYNC_MUX_SELECT,           0) |
			BCHP_FIELD_ENUM(IT_0_PCL_0, EXT_HSYNC_ENABLE,               ON) |
			BCHP_FIELD_DATA(IT_0_PCL_0, EXT_HSYNC_MUX_SELECT,           0) |
#endif
			BCHP_FIELD_ENUM(IT_0_PCL_0, NEGSYNC_AND_TERM_4 ,            ZERO) |
			BCHP_FIELD_ENUM(IT_0_PCL_0, NEGSYNC_AND_TERM_3 ,            LINE_D) |
			BCHP_FIELD_ENUM(IT_0_PCL_0, NEGSYNC_AND_TERM_2 ,            LINE_A) |
			BCHP_FIELD_ENUM(IT_0_PCL_0, NEGSYNC_AND_TERM_1 ,            LINE_B) |
			BCHP_FIELD_ENUM(IT_0_PCL_0, NEGSYNC_AND_TERM_0 ,            LINE_C) |
			BCHP_FIELD_DATA(IT_0_PCL_0, NEGSYNC_MUX_E_SELECT,           0) |
			BCHP_FIELD_DATA(IT_0_PCL_0, NEGSYNC_MUX_D_SELECT,           3) |
			BCHP_FIELD_DATA(IT_0_PCL_0, NEGSYNC_MUX_4_SELECT,           0) |
			BCHP_FIELD_DATA(IT_0_PCL_0, NEGSYNC_MUX_3_SELECT,           2);
	}

	BDBG_LEAVE(BVDC_P_Vec_Build_SyncPCL_isr);
	return;
}

uint32_t BVDC_P_Display_Modify_SYNC_TRANS_0 (
	BVDC_P_DisplayInfo               *pNewInfo,
	uint32_t                         ulVfSTRegVal)
{
	BSTD_UNUSED (pNewInfo);

	return ulVfSTRegVal;
}

/* End of File */
