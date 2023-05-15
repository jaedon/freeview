
/***************************************************************************
 *     (c)2005-2012 Broadcom Corporation
 *
 *  This program is the proprietary software of Broadcom Corporation and/or its licensors,
 *  and may only be used, duplicated, modified or distributed pursuant to the terms and
 *  conditions of a separate, written license agreement executed between you and Broadcom
 *  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 *  no license (express or implied), right to use, or waiver of any kind with respect to the
 *  Software, and Broadcom expressly reserves all rights in and to the Software and all
 *  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 *  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 *  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
 *
 *  Except as expressly set forth in the Authorized License,
 *
 *  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 *  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 *  and to use this information only in connection with your use of Broadcom integrated circuit products.
 *
 *  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
 *  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
 *  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO
 *  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 *  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 *  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 *  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 *  USE OR PERFORMANCE OF THE SOFTWARE.
 *
 *  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
 *  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR
 *  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 *  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 *  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 *  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 *  ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: btnr_tune_ofdm.c $
 * $brcm_Revision: 9 $
 * $brcm_Date: 10/11/12 5:35p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /AP/ctfe/core/tnr/btnr_tune_ofdm.c $
 * 
 * 9   10/11/12 5:35p farshidf
 * SW3461-287: Create Release 5.3.0.0
 * 
 * Fw_Integration_Devel/9   10/11/12 5:35p farshidf
 * SW3461-287: Create Release 5.3.0.0
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/10   10/11/12 3:43p shchang
 * SW3461-286: remove dithering off to avoid slow AGC response
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/9   10/9/12 5:09p farshidf
 * SW3461-279: Tuner Low-power firmware
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/8   9/27/12 9:45a shchang
 * SW3461-208: completely separate cable/terrestrail settings
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/7   9/12/12 1:22p farshidf
 * SW7563-14: remove LNA access
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/6   9/10/12 12:06p farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 *  structure
 * 
 * 4   9/7/12 12:05p farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 *  structure
 * 
 * Fw_Integration_Devel/4   9/7/12 12:04p farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 *  structure
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/4   9/7/12 12:01p farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 *  structure
 * 
 * 3   8/29/12 1:39p farshidf
 * SW7552-336: re-structure TNR folder to point to AP tnr code
 * 
 * Fw_Integration_Devel/3   8/29/12 1:38p farshidf
 * SW7552-336: re-structure TNR folder to point to AP tnr code
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/3   8/29/12 1:38p farshidf
 * SW7552-336: re-structure TNR folder to point to AP tnr code
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/2   8/29/12 1:16p farshidf
 * SW7552-336: re-structure TNR folder to point to AP tnr code
 * 
 * 1   8/29/12 11:45a farshidf
 * SW7552-336: merge to main
 * 
 * Fw_Integration_Devel/1   8/29/12 11:45a farshidf
 * SW7552-336: merge to main
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/1   8/17/12 3:54p farshidf
 * SW3461-260: Sperate the Ter and cable tuner code base
 * 
 ***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "btmr.h"
#ifndef LEAP_BASED_CODE
#include "btnr.h"
#include "bdbg.h"
#include "btnr_priv.h"
#include "btnr_3x7xib_priv.h"
#endif
#include "btnr_global_clk.h"
#include "btnr_struct.h"
#ifdef  LEAP_BASED_CODE
#include "btnr_api.h"
#endif
#include "btnr_tune.h"
#include "bmth.h"
#if ((BCHP_CHIP == 3461) || (BCHP_CHIP == 3462) || (BCHP_CHIP == 3472))
#include "bchp_tm.h"
#endif
#if (BCHP_CHIP == 7552)
#include "bchp_gio_aon.h"
#endif
#include "bchp_ufe_afe.h"
#include "bchp_sdadc.h"
#ifndef LEAP_BASED_CODE
BDBG_MODULE(btnr_tune_ofdm);
#define POWER2_24 16777216
#define POWER2_3 8
#define POWER2_16 65536
#define POWER2_29 536870912
#define POWER2_27 134217728
#endif
#include "bchp_ufe.h"



#if ((BCHP_CHIP == 3472) || ((BCHP_CHIP == 3461) && (BCHP_VER == BCHP_VER_B0)) || (BCHP_CHIP == 3462) || (BCHP_CHIP == 7552))
#define LOW_POWER_MODE
#endif
#ifdef  LOW_POWER_MODE
#define ENABLE_LNA_AGC_CYCLE
/*LNA*/
#define LNA_BIAS_SENSITIVITY 11
#define LNA_BIAS_ACI 1
/*LNA SF*/
#define LNASF_BIAS_SENSITIVITY 0
#define LNASF_BIAS_ACI 1
/*RFVGA*/
#define RFVGA_BIAS_SENSITIVITY 2
#define RFVGA_BIAS_ACI 4
/*RFFIL*/
#define TRKFIL_BIAS_SENSITIVITY 5
#define TRKFIL_BIAS_ACI 4
/*RFVGA RDEG*/
#define RFVGA_RDEG_SENSITIVITY 7
#define RFVGA_RDEG_ACI 1
/*MXR*/
#define MXR_BIAS_SENSITIVITY 0
#define MXR_BIAS_ACI 0
/*FGA*/
#define FGA_BIAS_SENSITIVITY 2
#define FGA_BIAS_ACI 4
/*LPF*/
#define LPF_BIAS_SENSITIVITY 2
#define LPF_BIAS_ACI 2
#else
/*LNA*/
#define LNA_BIAS_SENSITIVITY 11
#define LNA_BIAS_ACI 1
/*LNA SF*/
#define LNASF_BIAS_SENSITIVITY 4
#define LNASF_BIAS_ACI 4
/*RFVGA*/
#define RFVGA_BIAS_SENSITIVITY 12
#define RFVGA_BIAS_ACI 12
/*RFFIL*/
#define TRKFIL_BIAS_SENSITIVITY 10
#define TRKFIL_BIAS_ACI 10
/*RFVGA RDEG*/
#define RFVGA_RDEG_SENSITIVITY 7
#define RFVGA_RDEG_ACI 3
/*MXR*/
#define MXR_BIAS_SENSITIVITY 6
#define MXR_BIAS_ACI 6
/*FGA*/
#define FGA_BIAS_SENSITIVITY 5
#define FGA_BIAS_ACI 5
/*LPF*/
#define LPF_BIAS_SENSITIVITY 4
#define LPF_BIAS_ACI 4
#endif


/*******************************************************************************************************************
 * BTNR_P_Ods_TunerSetAGCTOP()  This routine sets the tuner AGC TOP
 ******************************************************************************************************************/
 void BTNR_P_Ods_TunerSetAGCTOP(BTNR_3x7x_ChnHandle h)
{
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_00, pd_thresh, 0xC);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_03, RF_Ios_PRG, 0x8);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_03, BB1_Ios_PRG, 0x7);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFAGC_03, BB2_Ios_PRG, 0x7);	
	BDBG_MSG(("BTNR_P_Ods_TunerSetAGCTOP() Complete"));
}

/*******************************************************************************************
 *BTNR_P_Ods_LNAAGCCycle()		This routine keeps LNA AGC and 6 Bit ADC enabled for a small duty 
 *cycle to save power. LNA AGC is slower than RF AGC so slower reaction time is not an issue. 
 *The PI will call the BTNR_P_LNAAGCCycle() every Xmsec (presently 100ms), based on a timer
 *interrupt 
 *******************************************************************************************/
BERR_Code BTNR_P_Ods_LNAAGCCycle(BTNR_3x7x_ChnHandle h)
{
	BERR_Code retCode = BERR_SUCCESS;

#ifdef ENABLE_LNA_AGC_CYCLE
	/*Reset WBADC & UnFreeze LNAAGC */
	BTNR_P_TunerSetADC6B(h);
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_03, AGC_frz, 0x0);

	/* --- Sleep X (To control the duty cycle) 20% --- */
	BKNI_Delay(5000);
	BKNI_Sleep(90);
	BKNI_Delay(5000);

	/* --- Freeze LNA --- */
	/*Freeze LNA AGC  , Power Down 6-bit ADC & PHYPLL ch1*/
	BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_03, AGC_frz, 0x1);
	BKNI_Delay(20); /*20 usec*/
	BTNR_P_TunerSetADC6BDOWN(h);
#else
	BSTD_UNUSED(h);
#endif
	/*goto label to return error code if something bad happened above*/
	return retCode;
}

/*******************************************************************************************************************
 * BTNR_P_Ods_TunerBiasSelect()  This routine sets the tuner bias mode based AGC reading
 ******************************************************************************************************************/
void BTNR_P_Ods_TunerBiasSelect(BTNR_3x7x_ChnHandle h)
{
	BTNR_P_TunerAGCReadBack(h);
	h->pTunerStatus->Tuner_Change_Settings = 0;
	switch (h->pTunerParams->BTNR_RF_Input_Mode)
	{
	case BTNR_3x7x_TunerRfInputMode_eInternalLna:
		/* dithering on/off */
/*		if ((h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eSensitivity) &&
		    (h->pTunerStatus->Tuner_RFVGA_Gain_Code == 0xFFFFFFFF) && (h->pTunerStatus->Tuner_LNA_Gain_Code == 0x1D)) {h->pTunerStatus->RFAGC_dither = 0;}
		else {h->pTunerStatus->RFAGC_dither = 1;}	
		BTNR_P_TunerSetDither(h);*/
		/* bais settings */
		if ((h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eSensitivity) &&
		   ((h->pTunerStatus->Tuner_RFVGA_Gain_Code <= 0x60000000) || (h->pTunerStatus->Tuner_LNA_Gain_Code != 0x1D)))
		{
			h->pTunerParams->BTNR_Internal_Params.Bias_Select = BTNR_Internal_Params_TunerBias_eACI;
			h->pTunerStatus->Tuner_Change_Settings = 1;
		}
		else if ((h->pTunerParams->BTNR_Internal_Params.Bias_Select != BTNR_Internal_Params_TunerBias_eSensitivity) &&
			    ((h->pTunerStatus->Tuner_RFVGA_Gain_Code == 0xFFFFFFFF) && (h->pTunerStatus->Tuner_LNA_Gain_Code == 0x1D)))
		{
			h->pTunerParams->BTNR_Internal_Params.Bias_Select = BTNR_Internal_Params_TunerBias_eSensitivity;
			h->pTunerStatus->Tuner_Change_Settings = 1;
		}		
	break;
	case BTNR_3x7x_TunerRfInputMode_eExternalLna:
		/* dithering on/off */
/*		if ((h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eSensitivity) &&
			(h->pTunerStatus->Tuner_RFVGA_Gain_Code == 0xFFFFFFFF))	{h->pTunerStatus->RFAGC_dither = 0;}		
		else {h->pTunerStatus->RFAGC_dither = 1;}
		BTNR_P_TunerSetDither(h);*/
		/* bias settings */
		if ((h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eSensitivity) &&
		    (h->pTunerStatus->Tuner_RFVGA_Gain_Code <= 0x60000000)) 
		{
			h->pTunerParams->BTNR_Internal_Params.Bias_Select = BTNR_Internal_Params_TunerBias_eACI;
			h->pTunerStatus->Tuner_Change_Settings = 1;
		}	
		else if ((h->pTunerParams->BTNR_Internal_Params.Bias_Select != BTNR_Internal_Params_TunerBias_eSensitivity) &&
			     (h->pTunerStatus->Tuner_RFVGA_Gain_Code == 0xFFFFFFFF))
		{
			h->pTunerParams->BTNR_Internal_Params.Bias_Select = BTNR_Internal_Params_TunerBias_eSensitivity;
			h->pTunerStatus->Tuner_Change_Settings = 1;
		}			
	break;	
	case BTNR_3x7x_TunerRfInputMode_InternalLna_Daisy:
		if ((h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eSensitivity) &&
			(h->pTunerStatus->Tuner_LNA_Gain_Code != 0x1D)) 
		{
			h->pTunerParams->BTNR_Internal_Params.Bias_Select = BTNR_Internal_Params_TunerBias_eACI;
			h->pTunerStatus->Tuner_Change_Settings = 1;
		}			
		else if ((h->pTunerParams->BTNR_Internal_Params.Bias_Select != BTNR_Internal_Params_TunerBias_eSensitivity) &&
                 (h->pTunerStatus->Tuner_LNA_Gain_Code == 0x1D))
		{
			h->pTunerParams->BTNR_Internal_Params.Bias_Select = BTNR_Internal_Params_TunerBias_eSensitivity;
			h->pTunerStatus->Tuner_Change_Settings = 1;
		}			
	break;		
	case BTNR_3x7x_TunerRfInputMode_eStandardIf:
	case BTNR_3x7x_TunerRfInputMode_eLowIf:
	case BTNR_3x7x_TunerRfInputMode_eBaseband:
	case BTNR_3x7x_TunerRfInputMode_eOff:
	break;
	default:
		BDBG_ERR(("ERROR!!! Invalid h->pTunerParams->BTNR_RF_Input_Mode, value received is %d",h->pTunerParams->BTNR_RF_Input_Mode));
	}
}

/*******************************************************************************************************************
 * BTNR_P_Ods_TunerSetBias()  This routine sets the tuner bias
 ******************************************************************************************************************/
void BTNR_P_Ods_TunerSetBias(BTNR_3x7x_ChnHandle h)
{
	switch (h->pTunerParams->BTNR_Internal_Params.Bias_Select)
	{
	case BTNR_Internal_Params_TunerBias_eSensitivity:
#if (BTNR_P_BCHP_TNR_CORE_VER >= BTNR_P_BCHP_CORE_V_1_1)
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_SPARE_01, i_LNA_ctrl_bias, LNA_BIAS_SENSITIVITY);
#endif
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNA_01, i_LNA_sf, LNASF_BIAS_SENSITIVITY);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_bias, RFVGA_BIAS_SENSITIVITY);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_rdeg, RFVGA_RDEG_SENSITIVITY);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_BUF_I_ctrl, TRKFIL_BIAS_SENSITIVITY);	
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_03, i_MIXER_bias_ctrl, MXR_BIAS_SENSITIVITY);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_02, i_FGA_bias_ctrl, FGA_BIAS_SENSITIVITY);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LPF_01, i_LPF_bias, LPF_BIAS_SENSITIVITY);
		BREG_WriteField(h->hRegister, SDADC_CTRL_SYS0, i_ctl_adc_gain, 0x0);		
	break;
	case BTNR_Internal_Params_TunerBias_eACI:	
#if (BTNR_P_BCHP_TNR_CORE_VER >= BTNR_P_BCHP_CORE_V_1_1)
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_SPARE_01, i_LNA_ctrl_bias, LNA_BIAS_ACI);
#endif
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNA_01, i_LNA_sf, LNASF_BIAS_ACI);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_bias, RFVGA_BIAS_ACI);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_rdeg, RFVGA_RDEG_ACI);	
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_BUF_I_ctrl, TRKFIL_BIAS_ACI);	
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_03, i_MIXER_bias_ctrl, MXR_BIAS_ACI);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_02, i_FGA_bias_ctrl, FGA_BIAS_ACI);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LPF_01, i_LPF_bias, LPF_BIAS_ACI);		
		BREG_WriteField(h->hRegister, SDADC_CTRL_SYS0, i_ctl_adc_gain, 0x1);		
	break;
	default:
		BDBG_ERR(("ERROR!!! Invalid h->pTunerParams->BTNR_Internal_Params.Bias_Select, value received is %d",h->pTunerParams->BTNR_Internal_Params.Bias_Select));
	}	
	BDBG_MSG(("BTNR_P_Ods_TunerSetBias() Complete"));		
}

/*******************************************************************************************************************
 * BTNR_P_Ods_TunerSetTilt()  This routine sets the tuner tilt
 ******************************************************************************************************************/
void BTNR_P_Ods_TunerSetTilt(BTNR_3x7x_ChnHandle h)
{
	switch (h->pTunerParams->BTNR_Internal_Params.Bias_Select)
	{
	case BTNR_Internal_Params_TunerBias_eSensitivity:
	case BTNR_Internal_Params_TunerBias_eACI:
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNA_01, i_LNA_tilt, 0x0);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_tilt, 0x0);	
	break;
	default:
		BDBG_ERR(("ERROR!!! Invalid h->pTunerParams->BTNR_Internal_Params.Bias_Select, value received is %d",h->pTunerParams->BTNR_Internal_Params.Bias_Select));
	}
	BDBG_MSG(("BTNR_P_Ods_TunerSetTilt() Complete"));		
}



/*******************************************************************************************************************
 * BTNR_P_Ods_TunerSetRFFILSelect()  This routine sets the tuner RFFIL mode
 ******************************************************************************************************************/
void BTNR_P_Ods_TunerSetRFFILSelect(BTNR_3x7x_ChnHandle h)
{
	if (h->pTunerParams->BTNR_Internal_Params.LNA_Enable == BTNR_Internal_Params_eDisable)
	{
#ifdef LEAP_BASED_CODE
#if EXT_LNA_ENABLE
		BLNA_P_Init_LNA(h->hTnr->pLna);																/* Initialize external LNA BCM3406 */
	    BLNA_P_Set_LNA_Boost(h->hTnr->pLna);
#endif
#endif	
	}
	switch (h->pTunerParams->BTNR_Internal_Params.Bias_Select)
	{
	case BTNR_Internal_Params_TunerBias_eSensitivity:
		if (h->pTunerParams->BTNR_Acquire_Params.RF_Freq > 143160000)
		{
			/*Use MoCA Trap*/
			h->pTunerParams->BTNR_Internal_Params.RFFIL_Select = BTNR_Internal_Params_TunerRFFIL_eMOCATRAP;
		}
		else 
		{
			/*Use Tracking Filter Bypass*/
			h->pTunerParams->BTNR_Internal_Params.RFFIL_Select = BTNR_Internal_Params_TunerRFFIL_eTRKBYPASS;
		}
	break;
	case BTNR_Internal_Params_TunerBias_eACI:
		if (h->pTunerParams->BTNR_Acquire_Params.RF_Freq > 334020000)
		{
			/*Use MOCA Trap*/
			h->pTunerParams->BTNR_Internal_Params.RFFIL_Select = BTNR_Internal_Params_TunerRFFIL_eMOCATRAP;
		}
		else
		{
			/*Use Tracking Filter*/
			h->pTunerParams->BTNR_Internal_Params.RFFIL_Select = BTNR_Internal_Params_TunerRFFIL_eTRKFIL;
		}
	break;
	default:
		BDBG_ERR(("ERROR!!! Invalid h->pTunerParams->BTNR_Internal_Params.Bias_Select, value received is %d",h->pTunerParams->BTNR_Internal_Params.Bias_Select));
	}	
	BDBG_MSG(("BTNR_P_Ods_TunerSetRFFILSelect() Complete"));
}


/*******************************************************************************************************************
 * BTNR_P_Ods_TunerSetBiasSmooth()  This routine sets the tuner transition smoothly
 ******************************************************************************************************************/
void BTNR_P_Ods_TunerSetBiasSmooth(BTNR_3x7x_ChnHandle h)
{
	uint8_t BIAS, READ_BIAS;
	/*LNA*/
	#if (BTNR_P_BCHP_TNR_CORE_VER >= BTNR_P_BCHP_CORE_V_1_1)
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eSensitivity)	{BIAS = LNA_BIAS_SENSITIVITY;}
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eACI)	{BIAS = LNA_BIAS_ACI;}
	READ_BIAS = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_SPARE_01, i_LNA_ctrl_bias);
	while (READ_BIAS != BIAS)
	{
		if (READ_BIAS > BIAS) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_SPARE_01, i_LNA_ctrl_bias, (READ_BIAS -= 1)); BTNR_P_TunerSleep(1);} 
		if (READ_BIAS < BIAS) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_SPARE_01, i_LNA_ctrl_bias, (READ_BIAS += 1)); BTNR_P_TunerSleep(1);} 
	}
	#endif
	/*LNA SF*/
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eSensitivity)	{BIAS = LNASF_BIAS_SENSITIVITY;}
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eACI)	{BIAS = LNASF_BIAS_ACI;}
	READ_BIAS = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_LNA_01, i_LNA_sf);
	while (READ_BIAS != BIAS)
	{
		if (READ_BIAS > BIAS) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNA_01, i_LNA_sf, (READ_BIAS -= 1)); BTNR_P_TunerSleep(1);} 
		if (READ_BIAS < BIAS) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNA_01, i_LNA_sf, (READ_BIAS += 1)); BTNR_P_TunerSleep(1);} 
	}	
	/*RFVGA*/
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eSensitivity)	{BIAS = RFVGA_BIAS_SENSITIVITY;}
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eACI)	{BIAS = RFVGA_BIAS_ACI;}
	READ_BIAS = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_bias);
	while (READ_BIAS != BIAS)
	{
		if (READ_BIAS > BIAS) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_bias, (READ_BIAS -= 1)); BTNR_P_TunerSleep(1);} 
		if (READ_BIAS < BIAS) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_bias, (READ_BIAS += 1)); BTNR_P_TunerSleep(1);} 
	}	
	/*RFFIL*/
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eSensitivity)	{BIAS = TRKFIL_BIAS_SENSITIVITY;}
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eACI)	{BIAS = TRKFIL_BIAS_ACI;}
	READ_BIAS = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_BUF_I_ctrl);	
	while (READ_BIAS != BIAS)
	{
		if (READ_BIAS > BIAS) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_BUF_I_ctrl, (READ_BIAS -= 1)); BTNR_P_TunerSleep(1);} 
		if (READ_BIAS < BIAS) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFFIL_01, i_TRKFIL_BUF_I_ctrl, (READ_BIAS += 1)); BTNR_P_TunerSleep(1);} 
	}		
	/*MXR*/
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eSensitivity)	{BIAS = MXR_BIAS_SENSITIVITY;}
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eACI)	{BIAS = MXR_BIAS_ACI;}
	READ_BIAS = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_MXR_03, i_MIXER_bias_ctrl);	
	while (READ_BIAS != BIAS)
	{
		if (READ_BIAS > BIAS) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_03, i_MIXER_bias_ctrl, (READ_BIAS -= 1)); BTNR_P_TunerSleep(1);} 
		if (READ_BIAS < BIAS) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_03, i_MIXER_bias_ctrl, (READ_BIAS += 1)); BTNR_P_TunerSleep(1);} 
	}	
	/*FGA*/
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eSensitivity)	{BIAS = FGA_BIAS_SENSITIVITY;}
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eACI)	{BIAS = FGA_BIAS_ACI;}
	READ_BIAS = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_MXR_02, i_FGA_bias_ctrl);	
	while (READ_BIAS != BIAS)
	{
		if (READ_BIAS > BIAS) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_02, i_FGA_bias_ctrl, (READ_BIAS -= 1)); BTNR_P_TunerSleep(1);} 
		if (READ_BIAS < BIAS) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_MXR_02, i_FGA_bias_ctrl, (READ_BIAS += 1)); BTNR_P_TunerSleep(1);} 
	}	
	/*LPF*/
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eSensitivity)	{BIAS = LPF_BIAS_SENSITIVITY;}
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eACI)	{BIAS = LPF_BIAS_ACI;}
	READ_BIAS = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_LPF_01, i_LPF_bias);	
	while (READ_BIAS != BIAS)
	{
		if (READ_BIAS > BIAS) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LPF_01, i_LPF_bias, (READ_BIAS -= 1)); BTNR_P_TunerSleep(1);} 
		if (READ_BIAS < BIAS) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LPF_01, i_LPF_bias, (READ_BIAS += 1)); BTNR_P_TunerSleep(1);} 
	}		
	h->pTunerStatus->Tuner_Change_Settings = 2;
}

/*******************************************************************************************************************
 * BTNR_P_Ods_TunerSetRdegSmooth()  This routine sets the tuner transition smoothly
 ******************************************************************************************************************/
void BTNR_P_Ods_TunerSetRdegSmooth(BTNR_3x7x_ChnHandle h)
{
	uint8_t RFVGA_RDEG, RDEG;
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eSensitivity)	{RDEG = RFVGA_RDEG_SENSITIVITY;}
	if (h->pTunerParams->BTNR_Internal_Params.Bias_Select == BTNR_Internal_Params_TunerBias_eACI)	{RDEG = RFVGA_RDEG_ACI;}
	RFVGA_RDEG = BREG_ReadField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_rdeg);
	BDBG_MSG(("RFVGA_RDEG = %d",RFVGA_RDEG));
	if (RFVGA_RDEG != RDEG)
	{
		if (RFVGA_RDEG > RDEG) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_rdeg, (RFVGA_RDEG -= 1));}
		if (RFVGA_RDEG < RDEG) {BREG_WriteField(h->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_rdeg, (RFVGA_RDEG += 1));}
	}
	else {h->pTunerStatus->Tuner_Change_Settings = 0;}
}

/*******************************************************************************************************************
 * BTNR_P_Ods_TunerSetAGCBW()  This routine sets the tuner AGC BW
 ******************************************************************************************************************/
void BTNR_P_Ods_TunerSetAGCBW(BTNR_3x7x_ChnHandle h)
{
	switch (h->pTunerStatus->AGC_BW)
	{
	case 0:	
		/*LNA AGC bandwidth settings*/
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_00, peak_pwr_set_pt, 0x10B66);	
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_01, hi_thresh, 0x42D9);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_01, LNA_Kbw, 14);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_00, win_len, 0x18);

		/*RF AGC bandwidth settings*/
		h->pTunerStatus->RFAGC_indirect_addr = 0x4;
		h->pTunerStatus->RFAGC_indirect_data = 0x10;
		BTNR_P_TunerRFAGCIndirectWrite(h);
			
		h->pTunerStatus->RFAGC_indirect_addr = 0x2;
		h->pTunerStatus->RFAGC_indirect_data = 0x2710;
		BTNR_P_TunerRFAGCIndirectWrite(h);

		h->pTunerStatus->RFAGC_indirect_addr = 0x1;
		h->pTunerStatus->RFAGC_indirect_data = 0x4B354B35;
		BTNR_P_TunerRFAGCIndirectWrite(h);
	break;
	case 1:
		/*LNA AGC bandwidth settings*/
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_00, peak_pwr_set_pt, 0x10B66);	
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_01, hi_thresh, 0x42D9);
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_01, LNA_Kbw, 7);			
		BREG_WriteField(h->hRegister, UFE_AFE_TNR0_LNAAGC_00, win_len, 0x14);
		
		/*RF AGC bandwidth settings*/
		h->pTunerStatus->RFAGC_indirect_addr = 0x4;
		h->pTunerStatus->RFAGC_indirect_data = 0x1A;
		BTNR_P_TunerRFAGCIndirectWrite(h);	

		h->pTunerStatus->RFAGC_indirect_addr = 0x2;
		h->pTunerStatus->RFAGC_indirect_data = 0x2710;
		BTNR_P_TunerRFAGCIndirectWrite(h);

		h->pTunerStatus->RFAGC_indirect_addr = 0x1;
		h->pTunerStatus->RFAGC_indirect_data = 0x4B354B35;
		BTNR_P_TunerRFAGCIndirectWrite(h);
	break;
	default:
		BDBG_ERR(("ERROR!!! Invalid h->pTunerStatus->AGC_BW, value received is %d",h->pTunerStatus->AGC_BW));
	}
}

/*******************************************************************************************************************
 * BTNR_P_Ods_TunerShiftGearAGC()  This routine accelerate the tuner AGC
 ******************************************************************************************************************/
void BTNR_P_Ods_TunerShiftGearAGC(BTNR_3x7x_ChnHandle h)
{
	/*Accelerate AGC bandwidth*/
	h->pTunerStatus->AGC_BW = 1;
	BTNR_P_Ods_TunerSetAGCBW(h);
	BTNR_P_TunerSleep(5);
	/*reduce AGC bandwidth*/
	h->pTunerStatus->AGC_BW = 0;
	BTNR_P_Ods_TunerSetAGCBW(h);	
	BTNR_P_TunerAGCReadBack(h);
	BDBG_MSG(("LNA gain = %d",h->pTunerStatus->Tuner_LNA_Gain_Code));
	BDBG_MSG(("RFVGA gain = %d",(h->pTunerStatus->Tuner_RFVGA_Gain_Code>>16) & 0x0000FFFF));		
	BDBG_MSG(("BTNR_P_Ods_TunerShiftGearAGC() Complete"));
}





