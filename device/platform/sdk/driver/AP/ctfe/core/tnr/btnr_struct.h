/******************************************************************************
 *    (c)2011-2012 Broadcom Corporation
 * 
 * This program is the proprietary software of Broadcom Corporation and/or its licensors,
 * and may only be used, duplicated, modified or distributed pursuant to the terms and
 * conditions of a separate, written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
 * no license (express or implied), right to use, or waiver of any kind with respect to the
 * Software, and Broadcom expressly reserves all rights in and to the Software and all
 * intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
 * HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
 * NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization, constitutes the valuable trade
 * secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
 * AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
 * WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
 * THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
 * USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
 * LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
 * EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
 * ANY LIMITED REMEDY.
 *
 * $brcm_Workfile: btnr_struct.h $
 * $brcm_Revision: 33 $
 * $brcm_Date: 10/18/12 6:38p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AP/ctfe/core/tnr/btnr_struct.h $
 * 
 * 33   10/18/12 6:38p farshidf
 * SW3461-287: create 5.3.1.1
 * 
 * Fw_Integration_Devel/16   10/18/12 6:37p farshidf
 * SW3461-287: create 5.3.1.1
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/10   10/18/12 6:36p farshidf
 * SW3461-208: Start AGCMonitir after RFinput mode has been set
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/9   10/10/12 3:13p farshidf
 * SW3461-260: power up clean up
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/8   10/9/12 5:09p farshidf
 * SW3461-279: Tuner Low-power firmware
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/7   9/27/12 9:46a shchang
 * SW3461-208: completely separate cable/terrestrail settings
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/6   6/14/12 2:33p mbsingh
 * SW3461-195: Addition of Gain API to tuner - Adding new firmware
 *  structure for gain API
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/5   6/11/12 1:43p shchang
 * SW3462-31: fix 99MHz unlock issue due to 100MHz spur
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/4   6/4/12 9:23a shchang
 * SW3462-31: fix performance issue below 130MHz
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/3   5/21/12 6:58p farshidf
 * SW3461-1: sync up with branch 4.0
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/2   5/7/12 11:57a shchang
 * SW3461-120: 1. add LO index variable.
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/1   5/2/12 3:08p shchang
 * SW3461-120: add RFAGC_dither parameter
 * 
 * Fw_Integration_Devel/10   4/17/12 5:28p farshidf
 * SW3461-186: merge to integ
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/1   4/17/12 7:20a shchang
 * SW3461-184: tuner gain smooth transition
 * 
 * Fw_Integration_Devel/9   1/20/12 6:31p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/2   1/11/12 12:01p farshidf
 * SW3461-129: add the Internal Lna Daisy mode
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/1   1/11/12 11:05a farshidf
 * SW3461-129 : add new power mode Internal LNA daisy out
 * 
 * Fw_Integration_Devel/8   12/15/11 6:01p farshidf
 * SW3461-118: merge to Integ
 * 
 * 25   12/15/11 5:59p farshidf
 * SW3461-118: update the tuner to work on B0 and B1
 * 
 * 24   11/8/11 6:12p farshidf
 * SW7552-144: add loopthr,daisy,dpm
 * 
 * 23   10/24/11 3:25p farshidf
 * SW7552-134: add the new flag to store BBS address
 * 
 * 22   10/12/11 5:23p farshidf
 * SW7552-134: fixi 7552 issue
 * 
 * 21   8/24/11 12:33p farshidf
 * SW3461-38: merge to main
 * 
 * Fw_Integration_Devel/7   8/24/11 12:08p farshidf
 * SW3461-38: merge to integ
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/3   8/23/11 4:50p mpovich
 * SW3128-24: Merge latest of SW3128-24 to devel. branch.
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/2   8/22/11 12:44p jputnam
 * SW3461-1: Added SmartTune
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/SW3128-24/3   8/19/11 5:12p mpovich
 * SW3128-24: Rebase with Ver 2.0 devel. branch.
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/1   8/18/11 2:40p mpovich
 * SW3128-24: Merge Scan feature support into the devel. branch.
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/SW3128-24/2   8/5/11 1:08a mpovich
 * SW3128-24: Rebase with latest dev branch.
 * 
 * Fw_Integration_Devel/5   8/2/11 6:21p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/3   8/2/11 6:11p farshidf
 * SW3461-1: update the tuner structure
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/SW3128-24/1   8/1/11 7:37p mpovich
 * SW3128-24: Add "Initiate Tune after Setting Tune Params" flag to TNR
 *  Tune Params.
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/2   7/7/11 4:48p cbrooks
 * sw3461-1:added callback support
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/1   7/1/11 3:20p cbrooks
 * sw3461-1:added ADS callback support
 * 
 * Fw_Integration_Devel/3   7/20/11 11:30a farshidf
 * SW3461-1: add leap flag
 * 
 * Fw_Integration_Devel/2   7/19/11 8:27p farshidf
 * SW3461-1: fix BBS for 7552
 * 
 * Fw_Integration_Devel/1   6/29/11 12:39p farshidf
 * SW3461-13: merge to integration branch
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/1   6/22/11 6:48p cbrooks
 * sw3461-1:added callback support
 * 
 * 17   6/12/11 10:59a farshidf
 * SW3461-1: remove warning for 7552
 * 
 * 16   6/9/11 6:56p farshidf
 * SW3128-1: fix the mode
 * 
 * 15   6/9/11 4:56p farshidf
 * SW3461-1: 7552 compatiable
 * 
 * 14   6/9/11 4:48p farshidf
 * SW3461-1: sync up with 7552 code
 * 
 * 13   6/2/11 6:57p mpovich
 * SW3461-1: TNR RF Input Mode and TNR Power HAB commands.  Change T2
 *  Acquire Params.  Fix THD Status extraneous bytes sent by LEAP.
 * 
 * SW_System_HAB_Devel_4/1   6/2/11 5:18p mpovich
 * SW3461-1: Add "RF Input Mode" HAB command and remove "Tuner ower mode"
 *  from Tuner Power HAB command.
 * 
 * 12   5/20/11 6:43a mpovich
 * SW3461-1: rename UFE (BUFE) module to TNR (BTNR).
 * 
 * TNR_3461_1/1   5/19/11 5:15p mpovich
 * SW3461-1: Change BUFE module prefix to BTNR
 * 
 * 11   3/9/11 9:02p cbrooks
 * sw3461-1:Added LNA level and RFVGA level to status
 * 
 * 9   3/8/11 8:40p cbrooks
 * sw3461-1:new code
 * 
 * 8   3/8/11 8:04p mpovich
 * SW3461-1: Add "Get Power Status" function.
 * 
 * 7   3/8/11 3:42p farshidf
 * SW3461-1: add the tuner power up
 * 
 * 6   3/8/11 2:43p cbrooks
 * sw3461-1:new code
 * 
 * 4   3/6/11 5:58p cbrooks
 * SW3461-1:New TNR Code
 * 
 * 3   3/2/11 4:59p mpovich
 * SW3461-1: Fix TNR struct compiler bugs.  Add HAB related updates for T2
 *  and for TNR.
 * 
 * Rom_Devel_3461/1   3/2/11 4:25p mpovich
 * SW3461-1: Fix TNR struct compiler bugs.  Add HAB related updates for T2
 *  and for TNR.
 * 
 * 2   3/1/11 12:59p cbrooks
 * sw3461-1:new code
 * 
 * 1   2/28/11 3:55p cbrooks
 * sw3461-1:new code
 * 
 *****************************************************************************/

#ifndef _BTNR_STRUCT_H__
#define _BTNR_STRUCT_H__

#if __cplusplus
extern "C" {
#endif

/******************************************
 *BTNR_3x7x_Tuner_Power_Mode_t definition 
 *****************************************/
typedef enum BTNR_Tuner_Power_Mode_s
{
BTNR_Tuner_Power_Mode_eOff        = 0,
BTNR_Tuner_Power_Mode_eMini_Power = 1,
BTNR_Tuner_Power_Mode_eUHF_Power = 2,
BTNR_Tuner_Power_Mode_eVHF_Power = 3,
BTNR_Tuner_Power_Mode_eUHF_VHF_Power = 4,
BTNR_Tuner_Power_Mode_eLna_Daisy_Power = 5,
BTNR_Tuner_Power_Mode_eOn        = 0xFFU
}BTNR_Tuner_Power_Mode_t;

typedef struct BTNR_3x7x_Tuner_Power_Mode_s
{
	BTNR_Tuner_Power_Mode_t Tuner_Power_Mode;
}BTNR_3x7x_Tuner_Power_Mode_t;

/******************************************
 *BTNR_3x7x_TuneType_t definition 
 *****************************************/
typedef enum BTNR_TuneType_s
{
  BTNR_TuneType_eInitTune = 1,
  BTNR_TuneType_eTune = 2,
  BTNR_TuneType_eMiniTune = 3
} BTNR_TuneType_t;

typedef struct BTNR_3x7x_TuneType_s
{
	BTNR_TuneType_t   TuneType;
} BTNR_3x7x_TuneType_t;

/******************************************
 *BTNR_3x7x_Acquire_Params_t definition 
 *****************************************/
typedef enum BTNR_TunerApplicationMode_s
{
  BTNR_TunerApplicationMode_eCable = 1,
  BTNR_TunerApplicationMode_eTerrestrial = 2
} BTNR_TunerApplicationMode_t;

typedef enum BTNR_LPF_Bandwidth_s
{
  BTNR_LPF_Bandwidth_e8MHz = 1,
  BTNR_LPF_Bandwidth_e7MHz = 2,
  BTNR_LPF_Bandwidth_e6MHz = 3,
  BTNR_LPF_Bandwidth_e5MHz = 4,
  BTNR_LPF_Bandwidth_e1_7MHz = 5,
	BTNR_LPF_Bandwidth_eVariable = 6			
} BTNR_LPF_Bandwidth_t;

typedef enum BTNR_Standard_s
{
  BTNR_Standard_eDVBT = 1,
  BTNR_Standard_eISDBT = 2,
  BTNR_Standard_eQAM = 3,
  BTNR_Standard_eT2 =4
} BTNR_Standard_t;

typedef struct BTNR_Acquire_Params_s
{
	BTNR_TunerApplicationMode_t   Application;
	BTNR_Standard_t           Standard;
	BTNR_LPF_Bandwidth_t	  LPF_Bandwidth;
	uint32_t				  LPF_Variable_Bandwidth;	
	uint32_t				  RF_Freq;

    uint32_t                  Tune_After_Set_Params;  
} BTNR_3x7x_Acquire_Params_t;
 

/******************************************
 *BTNR_DPM_Params_t definition 
 *****************************************/
typedef enum BTNR_DPM_Target_s
{
  BTNR_DPM_Target_eDaisyUHF=0,
  BTNR_DPM_Target_eRFVGA=1,
  BTNR_DPM_Target_eDaisyUHF_RFVGA=2
}BTNR_DPM_Target_t;

typedef enum BTNR_DPM_Params_OffOn_s
{
  BTNR_DPM_Params_eDisable=0,
  BTNR_DPM_Params_eEnable=1
}BTNR_DPM_Params_OffOn_t;

typedef struct BTNR_3x7x_DPM_Params_s
{
	BTNR_DPM_Params_OffOn_t   DPM_Enable;
	BTNR_DPM_Target_t         DPM_Target;
	uint32_t				  DPM_Freq;
} BTNR_3x7x_DPM_Params_t;

/******************************************
 *BTNR_3x7x_LoopThru_Params_t definition 
 *****************************************/
typedef enum BTNR_LoopThru_Source_s
{
  BTNR_LoopThru_Source_eVHF=0,
  BTNR_LoopThru_Source_eDAC=1,
  BTNR_LoopThru_Source_eVHF_DAC=2
}BTNR_LoopThru_Source_t;

typedef enum BTNR_LoopThru_Params_OffOn_s
{
  BTNR_LoopThru_Params_eDisable=0,
  BTNR_LoopThru_Params_eEnable=1
}BTNR_LoopThru_Params_OffOn_t;

typedef struct BTNR_3x7x_LoopThru_Params_s
{
	BTNR_LoopThru_Params_OffOn_t LoopThru_Enable;
	BTNR_LoopThru_Source_t       LoopThru_Source;
} BTNR_3x7x_LoopThru_Params_t;

/******************************************
 *BTNR_3x7x_Daisy_Params_t definition 
 *****************************************/
typedef enum BTNR_Daisy_Source_s
{
  BTNR_Daisy_Source_eUHF=0,
  BTNR_Daisy_Source_eVHF=1,
  BTNR_Daisy_Source_eUHF_VHF=2
}BTNR_Daisy_Source_t;

typedef enum BTNR_Daisy_Params_OffOn_s
{
  BTNR_Daisy_Params_eDisable=0,
  BTNR_Daisy_Params_eEnable=1
}BTNR_Daisy_Params_OffOn_t;

typedef struct BTNR_3x7x_Daisy_Params_s
{
	BTNR_Daisy_Params_OffOn_t Daisy_Enable;
	BTNR_Daisy_Source_t       Daisy_Source;
} BTNR_3x7x_Daisy_Params_t;

/******************************************
 *BTNR_3x7x_Gain_Params_t definition 
 *****************************************/
typedef enum BTNR_Gain_Params_OffOn_s
{
  BTNR_Gain_Params_eDisable=0,
  BTNR_Gain_Params_eEnable=1
}BTNR_Gain_Params_OffOn_t;

typedef struct BTNR_3x7x_Gain_Params_s
{
	int32_t						ExternalGain_Total;			/*1/256 dBm [15:0]*/
	int32_t						ExternalGain_Bypassable;	/*1/256 dBm [15:0]*/
	int32_t						InternalGain_To_LT;			/*1/256 dBm [15:0]*/
	int32_t						InternalGain_To_Daisy;		/*1/256 dBm [15:0]*/
	uint32_t					Frequency;					/*Hz [31:0]*/
	BTNR_Gain_Params_OffOn_t	Gain_Bypassed;				/*Bool*/
}BTNR_3x7x_Gain_Params_t;

/****************************************************
 *BTNR_3x7x_Local_Params_t definition: must be initialized  
 ***************************************************/
typedef enum BTNR_3x7x_TunerRfInputMode
{
  BTNR_3x7x_TunerRfInputMode_eOff = 0, /* Tuner Rf is off. */
  BTNR_3x7x_TunerRfInputMode_eExternalLna=1,  /* Tuner Rf input through UHF path. This Rf path does not use internal LNA. */
  BTNR_3x7x_TunerRfInputMode_eInternalLna=2, /* Tuner Rf input through UHF path. This Rf path does uses internal LNA. */
  BTNR_3x7x_TunerRfInputMode_eStandardIf=3,  /* 44 MHz or 36 MHz */
  BTNR_3x7x_TunerRfInputMode_eLowIf=4,  /*4 MHz to 5 MHz. */
  BTNR_3x7x_TunerRfInputMode_eBaseband=5,
  BTNR_3x7x_TunerRfInputMode_InternalLna_Daisy=6
}BTNR_3x7x_TunerRfInputMode_t;

 typedef enum BTNR_Local_Params_SmartTune_s
{
  BTNR_Local_Params_SmartTune_FreqPlanDefault=0,
  BTNR_Local_Params_SmartTune_FreqPlanA=1
}BTNR_Local_Params_SmartTune_t;

typedef struct BTNR_3x7x_Local_Params_s
{	
	uint32_t								TunerCapCntl;
	int32_t									RF_Offset;	  /*This is set by scan*/
	uint32_t								Symbol_Rate;	/*This is set by scan*/
	int32_t									Total_Mix_After_ADC;
	int16_t									PostADC_Gain_x256db;
#ifndef LEAP_BASED_CODE
	BTNR_TunerApplicationMode_t					TunerApplication;
	BTNR_RfInputMode						RfInputMode;
	BTNR_Tuner_Power_Mode_t					Tuner_Power_Mode;
	uint32_t								TunerBBSaddress;
#endif
	BTNR_Local_Params_SmartTune_t   SmartTune;	
	uint32_t								RevId;
	uint32_t								TuneComple;
}BTNR_3x7x_Local_Params_t;

/****************************************************
 *BTNR_3x7x_Internal_Params_t definition: must be initialized 
 ***************************************************/
typedef enum BTNR_Internal_Params_SDADC_Input_s
{
	BTNR_Internal_Params_SDADC_Input_eTuner=0,
	BTNR_Internal_Params_SDADC_Input_eTuner_wTestOut=1,
	BTNR_Internal_Params_SDADC_Input_eExtReal=2,
	BTNR_Internal_Params_SDADC_Input_eExtIQ=3
} BTNR_Internal_Params_SDADC_Input_t;

typedef enum BTNR_Internal_Params_TunerRFFIL_s
{
  BTNR_Internal_Params_TunerRFFIL_eTRKFIL=0,
  BTNR_Internal_Params_TunerRFFIL_eMOCATRAP=1,
  BTNR_Internal_Params_TunerRFFIL_eTRKBYPASS=2
}BTNR_Internal_Params_TunerRFFIL_t;

typedef enum BTNR_Internal_Params_OffOn_s
{
  BTNR_Internal_Params_eDisable=0,
  BTNR_Internal_Params_eEnable=1
}BTNR_Internal_Params_OffOn_t;

typedef enum BTNR_Internal_Params_TunerBias_s
{
	BTNR_Internal_Params_TunerBias_eSensitivity=0,
	BTNR_Internal_Params_TunerBias_eACI=1
} BTNR_Internal_Params_TunerBias_t;

typedef struct BTNR_3x7x_Internal_Params_s
{	
	BTNR_Internal_Params_OffOn_t       LNA_Enable;
	BTNR_Internal_Params_SDADC_Input_t SDADC_Input;
	BTNR_Internal_Params_TunerRFFIL_t  RFFIL_Select;
	BTNR_Internal_Params_OffOn_t       HRC_Enable;
	BTNR_Internal_Params_TunerBias_t   Bias_Select;
	uint32_t						   IF_Freq;
	bool						       RFInputModeComplete;
}BTNR_3x7x_Internal_Params_t;

typedef enum BTNR_BBSConnectMode_s
{
  BTNR_BBSConnectMode_Tune = 1, 
  BTNR_BBSConnectMode_ResetStatus = 2,
  BTNR_BBSConnectMode_EnableStatus = 4,
  BTNR_BBSConnectMode_RfMode = 8,
  BTNR_BBSConnectMode_EnableLoop = 16,
  BTNR_BBSConnectMode_EnableDaisy = 32,
  BTNR_BBSConnectMode_EnableDPM = 64
} BTNR_BBSConnectMode_t;

/****************************************************
 *BTNR_3x7x_BBS_Params_t definition: 
 ***************************************************/
typedef struct BTNR_3x7x_BBS_Params_t
{	
	uint32_t							StartSturctureAddress;
	volatile BTNR_BBSConnectMode_t		BBSConnectMode;
	BTNR_TunerApplicationMode_t			Application;
	BTNR_Standard_t						Standard;
	BTNR_LPF_Bandwidth_t				LPF_Bandwidth;
    BTNR_3x7x_TunerRfInputMode_t		BTNR_RF_Input_Mode; 
	BTNR_3x7x_LoopThru_Params_t			BTNR_LoopThru_Params;
	BTNR_3x7x_Daisy_Params_t			BTNR_Daisy_Params;
	BTNR_3x7x_DPM_Params_t				 BTNR_DPM_Params;
}BTNR_3x7x_BBS_Params_t;

/****************************************************
 *BTNR_3x7x_TuneParams_t definition: 
 ***************************************************/
/*This is the main structure by the tuning functions*/
typedef struct BTNR_3x7x_TuneParams_s
{
#ifndef LEAP_BASED_CODE
	BTNR_3x7x_BBS_Params_t		 BTNR_BBS_Params;
#endif
	BTNR_3x7x_Tuner_Power_Mode_t BTNR_TunePowerMode;
	BTNR_3x7x_TuneType_t         BTNR_TuneType;
	BTNR_3x7x_Acquire_Params_t   BTNR_Acquire_Params;
	BTNR_3x7x_DPM_Params_t       BTNR_DPM_Params;
	BTNR_3x7x_LoopThru_Params_t  BTNR_LoopThru_Params;
	BTNR_3x7x_Daisy_Params_t     BTNR_Daisy_Params;
	BTNR_3x7x_Gain_Params_t		 BTNR_Gain_Params;
    BTNR_3x7x_TunerRfInputMode_t BTNR_RF_Input_Mode; 
	BTNR_3x7x_Internal_Params_t  BTNR_Internal_Params;
	BTNR_3x7x_Local_Params_t     BTNR_Local_Params;
}BTNR_3x7x_TuneParams_t;

typedef enum BTNR_PowerStatus_s
{
    BTNR_ePower_Off = 0,
    BTNR_ePower_On  = 1,
    BTNR_ePower_Unknown = 0xFF
}  BTNR_PowerStatus_t;

typedef enum BADS_3x7x_Status_UnlockLock_s
{
  BTNR_Status_eUnlock=0, 
  BTNR_Status_eLock=1
}BTNR_3x7x_Status_UnlockLock_t;

/****************************************************
 *BTNR_3x7x_TuneStatus_t definition:
 ***************************************************/
/*This is the main structure by the tuning status*/
typedef struct BTNR_3x7x_TuneStatus_s
{
  uint32_t      Lock;
	BTNR_3x7x_Status_UnlockLock_t		Tuner_Ref_Lock_Status;
	BTNR_3x7x_Status_UnlockLock_t		Tuner_Phy_Lock_Status;
	BTNR_3x7x_Status_UnlockLock_t		Tuner_Mixer_Lock_Status;
	BTNR_PowerStatus_t                  PowerStatus;
	uint32_t                            Tuner_Ref_Freq;
	uint32_t                            Tuner_RefPll_Freq;
	uint32_t                            Tuner_PhyPll1_Freq;
	uint32_t                            Tuner_PhyPll2_Freq;
	uint32_t                            Tuner_PhyPll3_Freq;
	uint32_t                            Tuner_PhyPll4_Freq;
	uint32_t                            Tuner_PhyPll5_Freq;
	uint32_t                            Tuner_PhyPll6_Freq;
	uint32_t							Tuner_LNA_Gain_Code;
	uint32_t							Tuner_RFVGA_Gain_Code;
	uint32_t							Tuner_RF_Freq;
	int16_t								Tuner_PreADC_Gain_x256db;
	int16_t								External_Gain_x256db;
	int8_t								Tuner_RFFIL_band;
	int8_t								Tuner_RFFIL_tune;
	int8_t								RFAGC_indirect_addr;
	uint32_t							RFAGC_indirect_data;
	uint8_t								Tuner_Change_Settings;
	uint8_t								LO_index;
	uint8_t								RFAGC_dither;
	uint8_t								External_FGLNA_Mode;	
	uint8_t								AGC_BW;	
	uint8_t								DCO_ON;	
} BTNR_3x7x_TuneStatus_t;

#ifdef __cplusplus
}
#endif

#endif /* _BTNR_STRUCT_H__ */





