/*************************************************************************
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
* $brcm_Workfile: btnr_init.c $
* $brcm_Revision: 34 $
* $brcm_Date: 9/7/12 12:03p $
*
* [File Description:]
*
* Revision History:
*
 * $brcm_Log: /AP/ctfe/core/tnr/btnr_init.c $
 * 
 * 34   9/7/12 12:03p farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 *  structure
 * 
 * Fw_Integration_Devel/20   9/7/12 12:02p farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 *  structure
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/2   9/7/12 12:01p farshidf
 * SW7425-3765: Remove symbolic links and flatten out TNR directory
 *  structure
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/1   5/7/12 6:31p farshidf
 * SW3472-2: Make the tuner multi channel
 * 
 * Fw_Integration_Devel/18   3/23/12 2:51p farshidf
 * SW3128-125: FW version 4.6
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/4   3/12/12 6:09p farshidf
 * SW3462-6: merge top Dev branch
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/SW3462-6/2   2/29/12 5:27p farshidf
 * SW3461-165 : Chip version clean up & fix tuner for 3462
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/SW3462-6/1   2/28/12 6:04p mpovich
 * SW3462-6: Rebase with SW3462-3 dev. branch.  Important Note:  Element 4
 *  changes of V3_0 TNR Dev not picked up.
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/SW3462-3/1   2/28/12 10:08a jputnam
 * SW3462-5: Temporary change to force use of TNR_CORE_V_1_1 for 3462A0
 *  until proper version methodology is adopted in the tuner driver
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/4   1/23/12 5:28p cbrooks
 * sw3461-1:added new breakpoints for the ACI filters
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/1   2/9/12 11:48a cbrooks
 * sw3461-1:fixed baud rate droppouts
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/4   1/23/12 5:28p cbrooks
 * sw3461-1:added new breakpoints for the ACI filters
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/3   1/18/12 10:11a jputnam
 * SW3461-88: Update UFE AGC thresholds for better compromise between LTE
 *  and impulse noise performance
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/2   12/26/11 8:49p shchang
 * SW3461-120: modify AGC threshold settings for DVB-T LTE scenarios
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/1   9/26/11 5:31p mbsingh
 * SW3461-1: Incorporate correct register writes for B0
 * 
 * Fw_Integration_Devel/12   9/22/11 4:44p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/3   9/21/11 4:14p farshidf
 * SW3461-1: update files for B0
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/2   9/7/11 11:31a jputnam
 * SW3461-1: Added new UFE filter coefficients for SmartTune
 *  FreqPlanAlternate
 * 
 * 25   8/26/11 3:45p farshidf
 * SW3461-1: merge to main
 * 
 * Fw_Integration_Devel/10   8/26/11 3:38p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/1   8/24/11 4:51p mbsingh
 * bcm3461-1: Removing Terrestrial Init() to save code size. This function
 *  is not required.
 * 
 * Fw_Integration_Devel/8   8/2/11 6:39p farshidf
 * SW3461-1: fix compile issue
 * 
 * Fw_Integration_Devel/7   8/2/11 6:20p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/7   8/2/11 6:11p farshidf
 * SW3461-1: update the tuner structure
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/6   7/29/11 1:59p jputnam
 * SW3461-1: Added AGC control for external low-IF tuner
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/5   7/27/11 1:12p farshidf
 * SW3461-1: make it maganum compatible
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/4   7/14/11 5:35p cbrooks
 * sw3461-1:removed print statements
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/3   7/14/11 5:21p cbrooks
 * sw3461-1:added callback support for gains
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/2   7/7/11 4:47p cbrooks
 * SW3461-1:working on callbacak functions and status
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/1   7/1/11 3:19p cbrooks
 * sw3461-1:added ADS callback support
 * 
 * Fw_Integration_Devel/2   7/19/11 7:43p farshidf
 * SW3461-1: compile fix
 * 
 * Fw_Integration_Devel/1   6/29/11 12:39p farshidf
 * SW3461-13: merge to integration branch
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/2   6/22/11 6:52p cbrooks
 * sw3461-1:fixed bug
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/1   6/22/11 6:47p cbrooks
 * sw3461-1:Added Callback Support
 * 
 * 20   6/12/11 12:41p farshidf
 * SW3461-1: clean up
 * 
 * 19   6/12/11 12:35p farshidf
 * SW3461-1: code clean up
 * 
 * 18   6/9/11 6:40p mpovich
 * SW3461-1: Merge Ver 0.4 Integ. onto main branch.
 * 
 * SW_System_4_Integ_Test/2   6/9/11 4:48p farshidf
 * SW3461-1: sync up with 7552 code
* 
* 17   5/20/11 6:43a mpovich
* SW3461-1: rename UFE (BUFE) module to TNR (BTNR).
* 
* TNR_3461_1/1   5/19/11 5:12p mpovich
* SW3461-1: Change BUFE module prefix to BTNR
* 
* 16   5/3/11 1:40p mpovich
* SW3461-1: Merge to main of intermediate label, FW_3461_A0_05022011_1100
*  (05/02/2011 Pace build).
* 
* TNR_3461_1/5   5/2/11 6:13p lukose
* SW3461-1: Added sensitivity improvements in TerrestrialInit function
*
* 15   4/19/11 3:50p farshidf
* SW3461-1: merge main
* 
* TNR_3461_1/4   4/13/11 7:41p cbrooks
* sw3461-1:added reset to fix ds core code
*
* TNR_3461_1/3   3/22/11 4:02p jputnam
* SW3461-1: Change AGC back-offs from 10dB to 15dB to slightly reduce
*  sprinkle in high-QAM constellations
*
* TNR_3461_1/2   3/18/11 4:07p farshidf
* SW3461-1: merge  main
*
* 13   3/18/11 4:06p farshidf
* SW3461-1: merge to main
*
* TNR_3461_1/1   3/12/11 2:58p cbrooks
* sw3461-1added hanli changes to init.c
*
* 9   3/9/11 7:21p cbrooks
* sw3461-1:added tnr programming code
*
* 8   3/3/11 12:40p farshidf
* SW3461-1: update the power seq
*
* 7   3/2/11 4:58p mpovich
* SW3461-1: Fix TNR struct compiler bugs.  Add HAB related updates for T2
*  and for TNR.
*
* Rom_Devel_3461/1   3/2/11 4:21p mpovich
* SW3461-1: Fix TNR struct compiler bugs.  Add HAB related updates for T2
*  and for TNR.
*
* 6   2/28/11 3:38p cbrooks
* SW3461-1:new code for tuner
*
* 5   2/24/11 3:18p farshidf
* SW3461-1: update the code for tuner
*
* 4   2/24/11 11:34a farshidf
* SW3461-1: add the tuner code
*
*
***************************************************************************/
#include "bstd.h"
#include "bkni.h"
#include "bmth.h"
#include "btmr.h"
#ifndef LEAP_BASED_CODE
#include "btnr.h"
#include "bdbg.h"
#include "btnr_priv.h"
#include "btnr_3x7xib_priv.h"
#else
#include "btnr_api.h"
#endif

#include "bchp_ufe.h"
#include "bchp_ufe_misc.h"
#include "bchp_ufe_misc2.h"
#include "bchp_ufe_saw.h"
#include "btnr_struct.h"

#include "btnr_init.h"
#include "btnr_tune.h"
#include "btnr_global_clk.h"
#include "bchp_ufe_afe.h"

#ifndef LEAP_BASED_CODE
BDBG_MODULE(btnr_init);
#define POWER2_31 2147483648UL
#define POWER2_16 65536
#define LOG10_POWER2_9_X5120 13871
#define Twos_Complement32(x) ((uint32_t)((x ^ 0xFFFFFFFF) + 1))
#endif

#define BCHP_UFE_MISC_CTRL_CLKGEN_DATA_RESET_SHIFT     BCHP_UFE_MISC_CTRL_DATA_RESET_SHIFT
#define BCHP_UFE_MISC_CTRL_CLKGEN_DATA_RESET_MASK  (BCHP_UFE_MISC_CTRL_DATA_RESET_MASK | BCHP_UFE_MISC_CTRL_CLKGEN_RESET_MASK)

#if 0
/******************************************************************************
*  BTNR_3x7x_P_TerrestrialInit
******************************************************************************/
BERR_Code BTNR_P_TerrestrialInit(BTNR_3x7x_ChnHandle hTnr)
{



/*	BREG_Write32(hTnr->hRegister, BCHP_UFE_MISC2_CLK_RESET, 0x00000000 ); */

	BREG_Write32(hTnr->hRegister, BCHP_UFE_CTRL,            0x001B000A );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_BYP,             0x00001004 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_RST,             0xC0000FFF );        /* reset the clock gen after BYP/CIC programming */
	BREG_Write32(hTnr->hRegister, BCHP_UFE_RST,             0x40000FFF );        /* clear clock gen reset, but hold datapath reset*/
	BREG_Write32(hTnr->hRegister, BCHP_UFE_DCO_CTRL,        0x00000008 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC1,            0x0000000D );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC2,            0x0000000B );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC3,            0x00000009 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC1_THRESH,     0x0001030e );        /* 15dB back-off dec2hex(num2tc(10^(-15/10),2,21,0))*/
	BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC2_THRESH,     0x0001030e );        /* 15dB back-off dec2hex(num2tc(10^(-15/10),2,21,0))*/
	BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC3_THRESH,     0x0001030e );        /* 15dB back-off dec2hex(num2tc(10^(-15/10),2,21,0))*/
	BREG_Write32(hTnr->hRegister, BCHP_UFE_IQIMB_AMP_CTRL,  0x00000007 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_IQIMB_PHS_CTRL,  0x00000007 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF0,      0x00000076 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF1,      0x000000D6 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF2,      0x0007FF7B );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF3,      0x0007FF2A );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF4,      0x0000018A );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF5,      0x0000004B );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF6,      0x0007FD32 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF7,      0x000001A9 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF8,      0x0000032C );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF9,      0x0007FB02 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF10,     0x0007FF06 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF11,     0x0000082C );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF12,     0x0007FB27 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF13,     0x0007F7F4 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF14,     0x00000D12 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF15,     0x00000149 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF16,     0x0007ECD0 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF17,     0x00000CF2 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF18,     0x00001067 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF19,     0x0007E1B1 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF20,     0x000000F6 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF21,     0x000028A2 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF22,     0x0007DF25 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF23,     0x0007E192 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF24,     0x000045EC );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF25,     0x0007F389 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF26,     0x0007A36C );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF27,     0x000060BC );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF28,     0x0000444C );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF29,     0x000709F6 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF30,     0x000070D3 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF31,     0x0003FFFF );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF32,     0x00000000 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_RST,             0x40000000 );        /* clear clock gen reset, but hold datapath reset */
	BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC1_LF,         0x06000000 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC2_LF,         0x02000000 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC3_LF,         0x02000000 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_IQIMB_AMP_LF,    0x00000000 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_IQIMB_PHS_LF,    0x00000000 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_FRZ,             0x00000000 );
	BREG_WriteField(hTnr->hRegister,UFE_CTRL, VID_QUANT,	0x2);					/* reprogram VID_QUANT*/
	BREG_Write32(hTnr->hRegister, BCHP_UFE_RST,             0x00000000 );        /* clear any remaining data reset*/
	BREG_WriteField(hTnr->hRegister,UFE_MISC_CTRL, BYP_HRC,	0x1);				/* set hard-bypass bit*/
	BREG_WriteField(hTnr->hRegister,UFE_MISC_CTRL, CLKGEN_DATA_RESET,	0x0);	/* clear data/clkgen reset*/

	BREG_WriteField(hTnr->hRegister, UFE_AFE_TNR0_MXR_01, i_MIXER_low_I_mode, 0x1);  /*0: high linearity mode , 1: low power mode,Reset value: 0x0*/
	BREG_WriteField(hTnr->hRegister, UFE_AFE_TNR0_MXR_03, i_MIXER_bias_ctrl, 0x0);   /*mixer bias current control, Reset value: 0x6*/
	BREG_WriteField(hTnr->hRegister, UFE_AFE_TNR0_RFVGA_01, i_RFVGA_ctrl_rdeg, 0x7); /*RFVGA control for degeneration resistors on all Gm stages thus changing the Max Gain*/
		return BERR_SUCCESS;

}

#endif
/******************************************************************************
*  BTNR_P_TnrMiniInit
******************************************************************************/
BERR_Code BTNR_P_TnrMiniInit(BTNR_3x7x_ChnHandle hTnr)
{
	/*These are scan parameters that should be reset*/
	hTnr->pTunerParams->BTNR_Local_Params.RF_Offset   = 0;    
	hTnr->pTunerParams->BTNR_Local_Params.Symbol_Rate = 0;

	BREG_WriteField(hTnr->hRegister, UFE_RST, DATA_RESET, 0x1 );        /* clear any remaining data reset*/
	BREG_WriteField(hTnr->hRegister, UFE_RST, DATA_RESET, 0x0 );        /* clear any remaining data reset*/

		return BERR_SUCCESS;
}

/******************************************************************************
*  BTNR_P_TnrInit
******************************************************************************/
BERR_Code BTNR_P_TnrInit(BTNR_3x7x_ChnHandle hTnr)
{
	/*These are scan parameters that should be reset*/
	hTnr->pTunerParams->BTNR_Local_Params.RF_Offset   = 0;    
	hTnr->pTunerParams->BTNR_Local_Params.Symbol_Rate = 0;

#if 0

	if (hTnr->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eCable)
	{
//#if 0
		BTNR_P_CableInit(hTnr);
//#endif
	}
	else
	{
		BTNR_P_TerrestrialInit(hTnr);
	}
#endif


/*Try me and remove above*/
		BTNR_P_Program_TNR(hTnr);

	return BERR_SUCCESS;
}



/******************************************************************************
*  BTNR_P_Program_TNR()
******************************************************************************/
void BTNR_P_Program_TNR(BTNR_3x7x_ChnHandle hTnr)
{
	bool HRC_Flag;
	bool Terr_Flag;
	bool Real_Flag;
	uint32_t IF_Freq;

	HRC_Flag  = (hTnr->pTunerParams->BTNR_Internal_Params.HRC_Enable == BTNR_Internal_Params_eEnable) ? true : false;
  Terr_Flag = (hTnr->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eTerrestrial) ? true : false;
	Real_Flag = (hTnr->pTunerParams->BTNR_Internal_Params.SDADC_Input == BTNR_Internal_Params_SDADC_Input_eExtReal) ? true : false;
	IF_Freq   = hTnr->pTunerParams->BTNR_Internal_Params.IF_Freq;

	if (HRC_Flag == true)
	{
		BDBG_ERR(("ERROR!!!  HRC IS UNSUPPORTED in "));
	}
#if 0
	//BREG_WriteField(hTnr->hRegister, UFE_CTRL, INPUT_FMT, 0);
	//BREG_WriteField(hTnr->hRegister, UFE_CTRL, INPUT_EDGE, 0);
	//BREG_WriteField(hTnr->hRegister, UFE_CTRL, IQ_SWAP, 0);
	//BREG_WriteField(hTnr->hRegister, UFE_CTRL, NEGATE_I, 0);
	//BREG_WriteField(hTnr->hRegister, UFE_CTRL, NEGATE_Q, 0);
	//BREG_WriteField(hTnr->hRegister, UFE_CTRL, SPINV_FRONT, 0);
	//BREG_WriteField(hTnr->hRegister, UFE_CTRL, SPINV_BACK, 0);
#endif
	/*HRC is bypassed*/
	BREG_WriteField(hTnr->hRegister, UFE_CTRL, USE_EXT_VID_FREQ, 0);
  BREG_WriteField(hTnr->hRegister, UFE_MISC_CTRL, BYP_HRC, 0x1);

	/*CIC is not bypassed*/
	BREG_WriteField(hTnr->hRegister, UFE_BYP, CIC, 0);

/*replaced for callback function
	if (IF_Freq == 0)
	{
		BREG_WriteField(hTnr->hRegister, UFE_BYP, BACK_MIX, 1);
	}
	else
	{
		BREG_WriteField(hTnr->hRegister, UFE_BYP, BACK_MIX, 0);
		BTNR_P_Program_Back_DDFS(hTnr);
	}


	if (Real_Flag == true)
	{
		BREG_WriteField(hTnr->hRegister, UFE_CTRL, ZERO_Q, 1);
		BREG_WriteField(hTnr->hRegister, UFE_CTRL, LO_IF, 1);
		BREG_WriteField(hTnr->hRegister, UFE_BYP, IQ_PHS, 1);
		BREG_WriteField(hTnr->hRegister, UFE_BYP, IQ_AMP, 1);

	}
	else
	{
		BREG_WriteField(hTnr->hRegister, UFE_CTRL, ZERO_Q, 0);
		BREG_WriteField(hTnr->hRegister, UFE_CTRL, LO_IF, 0);
		BREG_WriteField(hTnr->hRegister, UFE_BYP, IQ_PHS, 0);
		BREG_WriteField(hTnr->hRegister, UFE_BYP, IQ_AMP, 0);
		BTNR_P_Program_Back_DDFS(hTnr);
	}
*/
	if (Real_Flag == true)
	{
		BREG_WriteField(hTnr->hRegister, UFE_CTRL, ZERO_Q, 1);
		BREG_WriteField(hTnr->hRegister, UFE_CTRL, LO_IF, 1);
		BREG_WriteField(hTnr->hRegister, UFE_BYP, IQ_PHS, 1);
		BREG_WriteField(hTnr->hRegister, UFE_BYP, IQ_AMP, 1);
#if (BTNR_P_BCHP_TNR_CORE_VER == BTNR_P_BCHP_CORE_V_1_0)  /* TO FIX */
    /* Setup AGC control for DTV Module w/NuTune FK1605 */
	  BREG_WriteField(hTnr->hRegister, UFE_MISC_CTRL, AAGC_DIN_SEL, 1);          /* select CIC output to AAGC */
#else
	/* Setup AGC control for DTV Module w/NuTune FK1605 */
	  BREG_WriteField(hTnr->hRegister, UFE_MISC_CTRL_SEL, AAGC_DIN_SEL, 1);          /* select CIC output to AAGC */	
#endif
 	  /* BREG_Write32(hTnr->hRegister, BCHP_UFE_MISC_AGC_THRES, 0x00016B54); */  /* THRES = dec2hex(floor(10^(-9/20)*2^18)), back off 9dB, 2.18 format */   
 	  BREG_Write32(hTnr->hRegister, BCHP_UFE_MISC_AGC_THRES, 0x0000103a);        /* THRES = dec2hex(floor(10^(-36/20)*2^18)), back off 36dB, 2.18 format - WHY????*/ 
 	  BREG_Write32(hTnr->hRegister, BCHP_UFE_MISC_AGC_CTRL, 0x00140100);         /* BW = 2^-20, inv threshold compare, unfreeze */
 	  BREG_Write32(hTnr->hRegister, BCHP_UFE_MISC_AGC_INT_UT, 0x00000000);       /* Upper threshold corresponds to 0.6V */
 	  BREG_Write32(hTnr->hRegister, BCHP_UFE_MISC_AGC_INT_LT, 0xb0000000);       /* Lower threshold corresponds to 1.6V */
		BREG_WriteField(hTnr->hRegister, UFE_CTRL, SPINV_BACK, 1); 	     
	}
	else
	{
		BREG_WriteField(hTnr->hRegister, UFE_CTRL, ZERO_Q, 0);  
		BREG_WriteField(hTnr->hRegister, UFE_CTRL, LO_IF, 0);
		BREG_WriteField(hTnr->hRegister, UFE_BYP, IQ_PHS, 0);
		BREG_WriteField(hTnr->hRegister, UFE_BYP, IQ_AMP, 0);
	}

	/*always use back mixer*/
	BREG_WriteField(hTnr->hRegister, UFE_BYP, BACK_MIX, 0);
	BTNR_P_Program_Back_DDFS(hTnr);

	BREG_Write32(hTnr->hRegister, BCHP_UFE_RST,             0xC0000FFF );        /* reset the clock gen after BYP/CIC programming*/
	BREG_Write32(hTnr->hRegister, BCHP_UFE_RST,             0x40000FFF );        /* clear clock gen reset, but hold datapath reset*/
	BREG_Write32(hTnr->hRegister, BCHP_UFE_DCO_CTRL,        0x00000008 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC1,            0x0000000D );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC2,            0x0000000B );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC3,            0x00000009 );
	switch (hTnr->pTunerParams->BTNR_Acquire_Params.Application)
	{
    case BTNR_TunerApplicationMode_eTerrestrial:
		BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC1_THRESH,     0x0000830e );        /* 18dB back-off dec2hex(num2tc(10^(-15/10),2,21,0))*/
		BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC2_THRESH,     0x0000830e );        /* 18dB back-off dec2hex(num2tc(10^(-15/10),2,21,0))*/
		BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC3_THRESH,     0x0001030e );        /* 15dB back-off dec2hex(num2tc(10^(-15/10),2,21,0))*/
	break;
    case BTNR_TunerApplicationMode_eCable:		
		BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC1_THRESH,     0x0001030e );        /* 15dB back-off dec2hex(num2tc(10^(-15/10),2,21,0))*/
		BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC2_THRESH,     0x0001030e );        /* 15dB back-off dec2hex(num2tc(10^(-15/10),2,21,0))*/
		BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC3_THRESH,     0x0001030e );        /* 15dB back-off dec2hex(num2tc(10^(-15/10),2,21,0))*/	
	break;
	default:
		BDBG_ERR(("ERROR!!! Invalid h->pTunerParams->BTNR_Acquire_Params.Application, value received is %d",hTnr->pTunerParams->BTNR_Acquire_Params.Application));
		/*retCode = BERR_INVALID_PARAMETER;*/
		/*goto bottom of function to return error code*/
		/*goto something_bad_happened;*/
	}			
	BREG_Write32(hTnr->hRegister, BCHP_UFE_IQIMB_AMP_CTRL,  0x00000007 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_IQIMB_PHS_CTRL,  0x00000007 );

  BTNR_P_Program_VID_CIC_HB_SAW(hTnr);

	BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC1_LF,         0x06000000 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC2_LF,         0x02000000 );
	/*BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC3_LF,         0x02000000 );*/
	BREG_Write32(hTnr->hRegister, BCHP_UFE_AGC3_LF,         0x20000000 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_IQIMB_AMP_LF,    0x00000000 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_IQIMB_PHS_LF,    0x00000000 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_FRZ,             0x00000000 );
	BREG_Write32(hTnr->hRegister, BCHP_UFE_RST,             0x00000000 );        /* clear any remaining data reset*/
	BREG_WriteField(hTnr->hRegister, UFE_MISC_CTRL, CLKGEN_DATA_RESET, 0x3); /* set data/clkgen reset*/
	BREG_WriteField(hTnr->hRegister, UFE_MISC_CTRL, CLKGEN_DATA_RESET, 0x0); /* clear data/clkgen reset*/
}

/******************************************************************************
*  BTNR_P_Program_VID_CIC_HB_SAW()
******************************************************************************/
void BTNR_P_Program_VID_CIC_HB_SAW(BTNR_3x7x_ChnHandle hTnr)
{
	bool Terr_Flag;
	bool QAM_Flag;
	bool SmartTune_Flag;
	uint8_t  Vid_Quant;
	uint16_t Vid_Div;
	uint8_t  CIC, HB;
	uint8_t  CoeffSet, Index;

	Terr_Flag = (hTnr->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eTerrestrial) ? true : false;
	QAM_Flag = (hTnr->pTunerParams->BTNR_Acquire_Params.Standard == BTNR_Standard_eQAM) ? true : false;
  SmartTune_Flag = (hTnr->pTunerParams->BTNR_Local_Params.SmartTune == BTNR_Local_Params_SmartTune_FreqPlanA) ? true : false;
  
	/*Program VID_QUANT and VID_DIV*/
	/*These values need to match what is in btnr_global_clk.h*/
	Vid_Quant  = (Terr_Flag == true) ? 0x2 : 0x4;
	Vid_Div    = (Terr_Flag == true) ? 0xA : 0x24;

	/*Tables are defined for Terrestrial 8 MHz, 7 MHz, 6 MHz, 5 MHz and 1.7 MHz*/
	/*For Cable 8 MHz and 7 MHz use the same table, and 6 MHz, 5 MHz and 1.7 MHz use the same table*/
	switch (hTnr->pTunerParams->BTNR_Acquire_Params.LPF_Bandwidth)
	{
		case BTNR_LPF_Bandwidth_e8MHz :
			CIC			 = (Terr_Flag == true) ? 3 : 3;
			HB			 = (Terr_Flag == true) ? 2 : 2;
			CoeffSet = (Terr_Flag == true) ? ((SmartTune_Flag == true) ? 0 : 0) : 0;
			break;
		case BTNR_LPF_Bandwidth_e7MHz :
			CIC			 = (Terr_Flag == true) ? 3 : 3;
			HB			 = (Terr_Flag == true) ? 2 : 2;
			CoeffSet = (Terr_Flag == true) ? ((SmartTune_Flag == true) ? 1 : 1) : 2;
			break;
		case BTNR_LPF_Bandwidth_e6MHz :
			CIC			 = (Terr_Flag == true) ? 3 : 3;
			HB			 = (Terr_Flag == true) ? 2 : 2;
			CoeffSet = (Terr_Flag == true) ? ((SmartTune_Flag == true) ? 2 : 2) : 3;
			break;
		case BTNR_LPF_Bandwidth_e5MHz :
			CIC			 = (Terr_Flag == true) ? 0 : 0;
			HB			 = (Terr_Flag == true) ? 0 : 0;
			CoeffSet = (Terr_Flag == true) ? ((SmartTune_Flag == true) ? 3 : 3) : 4;
			break;
		case BTNR_LPF_Bandwidth_e1_7MHz :
			CIC			 = (Terr_Flag == true) ? 3 : 3;
			HB			 = (Terr_Flag == true) ? 0 : 0;
			CoeffSet = (Terr_Flag == true) ? ((SmartTune_Flag == true) ? 4 : 4) : 8;
			break;
		case BTNR_LPF_Bandwidth_eVariable :
			CIC			 = (Terr_Flag == true) ? 3 : 3;
			HB			 = (Terr_Flag == true) ? 2 : 2;
			CoeffSet = (Terr_Flag == true) ? ((SmartTune_Flag == true) ? 0 : 0) : 0;
		if ((hTnr->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth > MAX_LPF_VARIABLE_BW) ||
			  (hTnr->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth < MIN_LPF_VARIABLE_BW))
		{
			BDBG_ERR(("ERROR!!!  VARIABLE LPF BANDWIDTH UNSUPPORTED SETTING TO 8 MHZ for Terrestrial and 8 MHz for Cable"));
			BDBG_ERR(("ERROR!!! Invalid Tuner_Variable_BW in BTNR_P_Write_SAW() , Value is %d", hTnr->pTunerParams->BTNR_Acquire_Params.LPF_Variable_Bandwidth));
		}
		break;
	default :
		CIC			 = (Terr_Flag == true) ? 3 : 3;
		HB			 = (Terr_Flag == true) ? 2 : 2;
		CoeffSet = (Terr_Flag == true) ? ((SmartTune_Flag == true) ? 0 : 0) : 0;
		BDBG_ERR(("ERROR!!! Invalid Tuner_BW selected in BTNR_P_Write_SAW() , Value is %d", hTnr->pTunerParams->BTNR_Acquire_Params.LPF_Bandwidth));
		break;
	}

	/*Override Filters if symbol rate is not 0, means function is called by scan and QAM standard*/
	if ((hTnr->pTunerParams->BTNR_Local_Params.Symbol_Rate > 0) && (QAM_Flag == true))
	{
		if (hTnr->pTunerParams->BTNR_Local_Params.Symbol_Rate > 6058796)  /*6250000->5830000->6058796*/
		{
			CIC			 = 3;
			HB			 = 2;
			CoeffSet = 0;
		}
		else if (hTnr->pTunerParams->BTNR_Local_Params.Symbol_Rate > 5028631)   /*5360000->5000000->5028631*/
		{
			CIC			 = 3;
			HB			 = 2;
			CoeffSet = 2;		
		}
		else if (hTnr->pTunerParams->BTNR_Local_Params.Symbol_Rate > 4173624)   /*4460000->4170000->4173624*/
		{
			CIC			 = 3;
			HB			 = 2;
			CoeffSet = 3;		
		}
		else if (hTnr->pTunerParams->BTNR_Local_Params.Symbol_Rate > 3463992)		/*3580000->3330000->3463992)*/
		{
			CIC			 = 0;
			HB			 = 0;
			CoeffSet = 4;		
		}
		else if (hTnr->pTunerParams->BTNR_Local_Params.Symbol_Rate > 2539117)		/*2700000->2500000->2539117)*/
		{
			CIC			 = 3;
			HB			 = 1;
			CoeffSet = 5;		
		}
		else if (hTnr->pTunerParams->BTNR_Local_Params.Symbol_Rate > 1749080)  /*1800000->1670000->1749080*/
		{
			CIC			 = 1;
			HB			 = 0;
			CoeffSet = 6;		
		}
		else if (hTnr->pTunerParams->BTNR_Local_Params.Symbol_Rate > 1544728) /*1500000->1500000->1544728*/
		{
			CIC			 = 2; 
			HB			 = 0;
			CoeffSet = 7;		
		}
		else
		{
			CIC			 = 3;
			HB			 = 0;
			CoeffSet = 8;	
		}
	}


	/*Write VID, CIC and HB values*/
	BREG_WriteField(hTnr->hRegister, UFE_CTRL, VID_QUANT, Vid_Quant);
	BREG_WriteField(hTnr->hRegister, UFE_CTRL, VID_DIV, Vid_Div);
	BREG_WriteField(hTnr->hRegister, UFE_CTRL, CIC_DEC_RATIO, CIC);
	BREG_WriteField(hTnr->hRegister, UFE_BYP, HB, HB);

	/*Write SAW Coeffs*/
	if (CoeffSet > (BTNR_SAW_TABLE_SIZE-1))
	{
		BDBG_ERR(("ERROR!!! Reading outside of table size in  BWFE_P_HAB_Read_LIC_EQ_DCO()"));
	}

	for (Index=0;Index<BTNR_SAW_COEFF_SIZE;Index++)
	{
	  if (Terr_Flag)
		  if (SmartTune_Flag)
			  BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF0+(4*Index), BTNR_SAW_Table_FreqPlanAlternate[CoeffSet][Index]);	  
		  else
			  BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF0+(4*Index), BTNR_SAW_Table_FreqPlanDefault[CoeffSet][Index]);
		else
		  BREG_Write32(hTnr->hRegister, BCHP_UFE_SAW_COEFF0+(4*Index), BTNR_SAW_Table[CoeffSet][Index]);			  
	}

	/*Reset clock gen*/
	BREG_WriteField(hTnr->hRegister, UFE_MISC_CTRL, CLKGEN_RESET, 1);
	BREG_WriteField(hTnr->hRegister, UFE_MISC_CTRL, CLKGEN_RESET, 0);
}

/******************************************************************************
*  BTNR_P_Program_Back_DDFS()
******************************************************************************/
void BTNR_P_Program_Back_DDFS(BTNR_3x7x_ChnHandle hTnr)
{
	bool     Terr_Flag;
	bool		 QAM_Flag;
	uint8_t  CIC, HB;
	uint32_t DDFS_Input_Freq;
	uint32_t ulMultA, ulMultB, ulNrmHi, ulNrmLo, ulDivisor;
	bool  IsNegative = false;
	int32_t DDFS_Freq;

	Terr_Flag = (hTnr->pTunerParams->BTNR_Acquire_Params.Application == BTNR_TunerApplicationMode_eTerrestrial) ? true : false;
	QAM_Flag = (hTnr->pTunerParams->BTNR_Acquire_Params.Standard == BTNR_Standard_eQAM) ? true : false;

	CIC = BREG_ReadField(hTnr->hRegister, UFE_CTRL, CIC_DEC_RATIO);
	HB  = BREG_ReadField(hTnr->hRegister, UFE_BYP, HB);

	/*translate values for calculating DDFS FCW*/
	CIC =  CIC+2;
	HB  = (hTnr->pTunerParams->BTNR_Internal_Params.SDADC_Input == BTNR_Internal_Params_SDADC_Input_eExtReal)	? (16>>HB) : (32>>HB);
	if (CIC*HB == 0)
	{
			BDBG_ERR(("ERROR!!!  Divide by 0 in BTNR_P_Write_Back_DDFS()"));
	}

	/*FCW = F*2^32/DDFS_Input_Freq if is positive and FCW = (DDFS_Input_Freq-F)*2^32/DDFS_Input_Freq*/
	DDFS_Input_Freq = (Terr_Flag == true) ? (TERR_PHYPLL6_FREQ/(CIC*HB)): (CABLE_PHYPLL6_FREQ/(CIC*HB));
	
	/*find the desired DDFS Frequency*/
	/*for some reason the mixer DDFS is negative*/
	DDFS_Freq = -1*(int32_t)hTnr->pTunerParams->BTNR_Internal_Params.IF_Freq;
	if (QAM_Flag == true)
	{
		DDFS_Freq = DDFS_Freq - hTnr->pTunerParams->BTNR_Local_Params.RF_Offset;
	}
	
	/*Detect if DDFS_Freq is negative*/
	if (DDFS_Freq < 0)
	{
		IsNegative = true;
		DDFS_Freq = (DDFS_Freq == (int32_t)0x80000000) ? -1 * (DDFS_Freq + 1) : -1 * DDFS_Freq;
	}

	ulMultA = 2*(uint32_t)DDFS_Freq;
	ulMultB = POWER2_31;
	ulDivisor = DDFS_Input_Freq;
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);
	if (ulNrmHi != 0x00000000)
	{
     BDBG_ERR(("DDFS is outside of the 32 bit range in BTNR_P_Write_Back_DDFS()"));
	}

	/*If result should be negative, take twos complement of output*/
	ulNrmLo = (IsNegative == true) ? Twos_Complement32(ulNrmLo) : ulNrmLo;

	/*Write DDFS value*/
	BREG_Write32(hTnr->hRegister, BCHP_UFE_BMIX_FCW, ulNrmLo);

}

void BTNR_P_InitStatus(BTNR_3x7x_ChnHandle hTnr)
{
	bool     Terr_Flag;
	uint8_t  CIC, HB;
	uint32_t ReadReg;
	uint32_t DDFS_Input_Freq;
	int32_t  Freq;
	bool RegIsNegative = false;
	uint32_t ulMultA, ulMultB, ulNrmHi, ulNrmLo, ulDivisor;
	

	CIC = BREG_ReadField(hTnr->hRegister, UFE_CTRL, CIC_DEC_RATIO);
	HB  = BREG_ReadField(hTnr->hRegister, UFE_BYP, HB);

	/*translate values for calculating DDFS FCW*/
	CIC =  CIC+2;
	HB  = (hTnr->pTunerParams->BTNR_Internal_Params.SDADC_Input == BTNR_Internal_Params_SDADC_Input_eExtReal)	? (16>>HB) : (32>>HB);
	if (CIC*HB == 0)
	{
			BDBG_ERR(("ERROR!!!  Divide by 0 in BTNR_P_Write_Back_DDFS()"));
	}

	/*F = FCW*DDFS_Input_Freq/2^32 if is positive and F = DDFS_Input_Freq - FCW*DDFS_Input_Freq/2^32 if negative*/
	DDFS_Input_Freq = (Terr_Flag == true) ? (TERR_PHYPLL6_FREQ/(CIC*HB)): (CABLE_PHYPLL6_FREQ/(CIC*HB));

	/*Read DDFS value*/
	ReadReg = BREG_Read32(hTnr->hRegister, BCHP_UFE_BMIX_FCW);

	/*Detect if DDFS_Freq is negative*/
	if ((ReadReg & 0x80000000) != 0)
	{
		RegIsNegative = true;
		ReadReg = (ReadReg == 0x80000000) ? Twos_Complement32(0x80000001) : Twos_Complement32(ReadReg);
	}

	ulMultA = ReadReg;
	ulMultB = DDFS_Input_Freq/2;
	ulDivisor = POWER2_31;
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);
	if (ulNrmHi != 0x00000000)
	{
     BDBG_ERR(("F is outside of the 32 bit range in BTNR_P_Write_Back_DDFS()"));
	}

	/*for some reason the mixer DDFS is negative*/
	Freq = (RegIsNegative == true) ? (int32_t)ulNrmLo : -1*(int32_t)ulNrmLo;

	/*Assign value*/
	hTnr->pTunerParams->BTNR_Local_Params.Total_Mix_After_ADC = Freq;


	/*this is the sum of the 3 AGC's in 3461 UFE*/
	/*AGC1 AGC2 AGC3 Gain and Level Calculation*/
	/*AGCF is 32-bit 7.25 unsigned number*/
	/*Reset value is 0x02000000 which is a gain of 1*/
	/*  db = 20*log10(AGC1/2^25)*/
	/*  256*db = 5120*log10(AGC1/2^16)-5120*log10(2^9)*/ 
	ReadReg = BREG_Read32(hTnr->hRegister, BCHP_UFE_AGC1_LF); 
	ReadReg = 2*BMTH_2560log10(ReadReg/POWER2_16) - LOG10_POWER2_9_X5120;
	ReadReg = ReadReg & 0x0000FFFF;
	hTnr->pTunerParams->BTNR_Local_Params.PostADC_Gain_x256db = (int16_t)ReadReg;  /*BBS will divide by 256 to get dB*/

	ReadReg = BREG_Read32(hTnr->hRegister, BCHP_UFE_AGC2_LF); 
	ReadReg = 2*BMTH_2560log10(ReadReg/POWER2_16) - LOG10_POWER2_9_X5120;
	ReadReg = ReadReg & 0x0000FFFF;
	hTnr->pTunerParams->BTNR_Local_Params.PostADC_Gain_x256db += (int16_t)ReadReg;  /*BBS will divide by 256 to get dB*/

	ReadReg = BREG_Read32(hTnr->hRegister, BCHP_UFE_AGC3_LF); 
	ReadReg = 2*BMTH_2560log10(ReadReg/POWER2_16) - LOG10_POWER2_9_X5120;
	ReadReg = ReadReg & 0x0000FFFF;

	/*Assign value*/
	hTnr->pTunerParams->BTNR_Local_Params.PostADC_Gain_x256db += (int16_t)ReadReg;  /*BBS will divide by 256 to get dB*/

}

