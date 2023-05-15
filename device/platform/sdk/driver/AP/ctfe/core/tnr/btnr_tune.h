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
 * $brcm_Workfile: btnr_tune.h $
 * $brcm_Revision: 31 $
 * $brcm_Date: 10/18/12 6:01p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /AP/ctfe/core/tnr/btnr_tune.h $
 * 
 * 31   10/18/12 6:01p farshidf
 * SW3461-287: create 5.3.1.1
 * 
 * Fw_Integration_Devel/15   10/18/12 5:20p farshidf
 * SW3461-287: Create Release 5.3.1.1
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/16   10/13/12 3:50p mbsingh
 * SW3461-160: Fix type in a gain table name
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/15   10/13/12 2:40p mbsingh
 * SW3461-160: Converting from 1D arrays to 2D for cleaner implementation
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/14   10/13/12 10:32a mbsingh
 * SW3461-260: Adding the new gain code look up tables
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/13   10/9/12 5:09p farshidf
 * SW3461-279: Tuner Low-power firmware
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/12   9/27/12 9:46a shchang
 * SW3461-208: completely separate cable/terrestrail settings
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/11   9/4/12 6:37p shchang
 * SW3461-208: 1. change input clock of tuner PLL to CML 2. add low-power
 *  settings to 3461B0/B1/B2
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/10   8/29/12 11:22a shchang
 * SW3461-208: 1. change tuner/SDADC common-mode selection to LPF 2.
 *  change RFAGC clock from 1350MHz to 1080MHz 3. improve ACI performance
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/9   8/17/12 3:54p farshidf
 * SW3461-260: Sperate the Ter and cable tuner code base
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/8   7/28/12 9:42a shchang
 * SW3461-208: 1). power/input mode de-coupling 2). SDADC calibration for
 *  3462/72 3). low-power mode 4). shift gear on LNA AGC 5). bypass mode
 *  threshold adjustment during tune
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/7   7/3/12 4:11p farshidf
 * SW3461-120: remove  warning/compile issue
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/6   6/14/12 2:56p mbsingh
 * SW3461-195: Adding driver functions to support gain api - Adding new
 *  function prototype to calculate RFOutGains
 * 
 * 25   6/13/12 10:57a farshidf
 * SW3461-1: merge
 * 
 * Fw_Integration_Devel/9   6/13/12 10:57a farshidf
 * SW3461-1: merge
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/5   6/13/12 10:54a farshidf
 * SW3461-1: remove warning
 * 
 * 24   5/24/12 11:16a farshidf
 * SW3461-1: merge to main
 * 
 * Fw_Integration_Devel/8   5/24/12 11:14a farshidf
 * SW3461-1: merge to main
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/4   5/23/12 5:08p farshidf
 * SW3461-1: update the tuner status
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/3   5/21/12 6:19p farshidf
 * SW3461-213: Make the Tuner Branch 5.0 to compile for 7552
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/2   5/7/12 6:31p farshidf
 * SW3472-2: Make the tuner multi channel
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/1   5/7/12 11:56a shchang
 * SW3461-120: modify DPM LO table.
 * 
 * Fw_Integration_Devel/6   3/23/12 2:46p farshidf
 * SW3128-125: FW version 4.6
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/1   3/12/12 6:27p farshidf
 * SW3462-6: merge to dev
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/SW3462-6/1   2/29/12 5:34p farshidf
 * Sw3461-165: clean up
 * 
 * Fw_Integration_Devel/5   10/14/11 12:58a farshidf
 * SW3461-64: fix warning
 * 
 * 20   10/10/11 3:21p farshidf
 * SW7552-134: compile fix
 * 
 * 19   9/16/11 10:06a farshidf
 * SW3461-1: merge to main
 * 
 * Fw_Integration_Devel/4   9/16/11 9:57a farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V2_0_TNR_DEV/1   9/14/11 5:21p shchang
 * SW3461-1: change frequency plan
 * 
 * 18   7/20/11 11:37a farshidf
 * SW7552-60: mereg the latest tuner code
 * 
 * Fw_Integration_Devel/2   7/19/11 7:02p farshidf
 * SW3461-28: merge to main
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/3   7/19/11 7:01p farshidf
 * SW3461-28: compile fix
 * 
 * Fw_Integration_Devel/1   6/29/11 12:38p farshidf
 * SW3461-13: merge to integ V0.5_rc0
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/2   6/29/11 12:32p shchang
 * SW3461-1: replace 6-phase with 8-phase mixer
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/1   6/16/11 6:05p shchang
 * SW3461-1: add DPM feature
 * 
 * 17   6/9/11 7:03p farshidf
 * SW3461-1: merge to main
 * 
 * SW_System_4_Integ_Test/3   6/9/11 7:02p farshidf
 * SW3461-1: changes from Dave
 * 
 * 15   5/20/11 6:44a mpovich
 * SW3461-1: rename UFE (BUFE) module to TNR (BTNR).
 * 
 * TNR_3461_1/1   5/19/11 5:17p mpovich
 * SW3461-1: Change BUFE module prefix to BTNR
 * 
 * 14   3/25/11 6:32p mpovich
 * SW3128-1: Merge latest from TNR branch to main branch.
 * 
 * TNR_3461_1/5   3/18/11 4:12p farshidf
 * SW3461-1: merge  main
 * 
 * 13   3/16/11 10:22a farshidf
 * SW3461-1: merge internal LNA code
 * 
 * TNR_3461_1/4   3/16/11 8:13a jputnam
 * SW3461-1: Change default to internal LNA
 * 
 * TNR_3461_1/3   3/15/11 5:56p mbsingh
 * SW3461-1: Fixed the frequency tables. Now all frequencies above 143.16
 *  work
 * 
 * TNR_3461_1/2   3/14/11 8:16p lukose
 * SW3461-1: fix the freq table
 * 
 * TNR_3461_1/1   3/14/11 7:42p lukose
 * SW3461-1: Update freq. table values with correct values
 * 
 * 11   3/10/11 5:35p cbrooks
 * sw3461-1: New Code
 * 
 * 10   3/9/11 9:02p cbrooks
 * sw3461-1:Added LNA level and RFVGA level to status
 * 
 * 9   3/8/11 8:26p cbrooks
 * sw3461-1:new code
 * 
 * 7   3/7/11 9:11p cbrooks
 * sw3461-1:New Code
 * 
 * 6   3/6/11 6:36p cbrooks
 * sw3461-1:new code
 * 
 * 4   3/6/11 5:58p cbrooks
 * SW3461-1:New TNR Code
 * 
 * 3   3/2/11 5:28p farshidf
 * SW3461-1: remove the warning
 * 
 * 2   3/1/11 12:59p cbrooks
 * sw3461-1:new code
 * 
 * 1   2/24/11 11:27a farshidf
 * SW3461-1: add the initial Tuner code from Dave
 * 
 ***************************************************************************/

#ifndef _BTNR_TUNE_H__
#define _BTNR_TUNE_H__           

#ifdef __cplusplus
extern "C" {
#endif

#define NEW_GAIN_CODE

#define  	BTNR_ENABLE_HW_AUTO_TUNE 0 /*Enable Software Cap Select*/
#define  	BTNR_ENABLE_SDADC_CAL    0 /*Enable SDADC Calibration*/


#define 	REF_PLL_LOCK_TIMEOUT_MS 1 /*Ref PLL Lock timeout in ms [0 255]*/
#define 	PHY_PLL_LOCK_TIMEOUT_MS	1 /*Phy PLL Lock timeout in ms*[0 255]*/

#define   MAX_LPF_VARIABLE_BW 10000000 
#define   MIN_LPF_VARIABLE_BW 1000000

#define FGLNA_BYPASS_THRESHOLD_HIGH 5
#define FGLNA_BYPASS_THRESHOLD_LOW 27

/*Initial Values for the BTNR_Internal_Params structure*/
/*Warning!! these can be overwritten by BBS*/
#define INIT_BBS_LNA_ENABLE        BTNR_Internal_Params_eEnable              /*BTNR_Internal_Params_eDisable or BTNR_Internal_Params_eEnable*/
#define INIT_BBS_SDADC_INPUT       BTNR_Internal_Params_SDADC_Input_eTuner   /*BTNR_Internal_Params_SDADC_Input_eTuner, BTNR_Internal_Params_SDADC_Input_eTuner_wTestOut, 
											                                                         BTNR_Internal_Params_SDADC_Input_eExtReal or BTNR_Internal_Params_SDADC_Input_eExtIQ*/	      
#define INIT_BBS_RFFIL_SELECT      BTNR_Internal_Params_TunerRFFIL_eMOCATRAP /*BTNR_Internal_Params_TunerRFFIL_eMOCATRAP or 
																																		           BTNR_Internal_Params_TunerRFFIL_eTRKFIL,*/
#define INIT_BBS_HRC_ENABLE        BTNR_Internal_Params_eDisable             /*BTNR_Internal_Params_eDisable or BTNR_Internal_Params_eEnable*/
#define INIT_BBS_IF_FREQ           0                                         /*uint32_t*/
/*******************************************************************************************************
* declare table for LPF parameters  *********************
********************************************************************************************************/

/*This table is in 100 KHz resolution*/
/*The BTNR_P_TunerSetRFFIL() function assumes this is 105 elements*/
#define LPF_TABLE_SIZE 71
static const uint16_t LPF_Selection_Table[LPF_TABLE_SIZE] = 
{
	33, 35, 37, 38, 41, 42, 45, 47, 50, 52, 55,
	34, 35, 36, 37, 38, 39, 40, 42, 43, 45, 47, 49, 51, 53, 54, 55, 57, 58, 61, 63,
	30, 30, 31, 32, 33, 33, 33, 34, 34, 34, 35, 35, 36, 37, 38, 39, 40, 41, 42, 42, 
	43, 44, 44, 44, 44, 45, 46, 46, 46, 46, 48, 49, 50, 51, 51, 54, 54, 58, 58, 62
};

/*******************************************************************************************************
*declare table to get FGA_RC parameters, both fixed value and variable
********************************************************************************************************/
#define FGA_RC_CTRL_LOWG_8MHz   0xA7  /*[0 255]*/
#define FGA_RC_CTRL_HIGHG_8MHz  0xC9  /*[0 255]*/
#define IFLPF_BW_SEL_8MHz       0x13  /*[0 63]*/
#define IFLPF_WBW_SEL_8MHz         0  /*0 normal: 1 wideband*/

#define FGA_RC_CTRL_LOWG_7MHz   0x96   
#define FGA_RC_CTRL_HIGHG_7MHz  0xBD  
#define IFLPF_BW_SEL_7MHz       0x0F
#define IFLPF_WBW_SEL_7MHz         0

#define FGA_RC_CTRL_LOWG_6MHz   0x7F   
#define FGA_RC_CTRL_HIGHG_6MHz  0xAD  
#define IFLPF_BW_SEL_6MHz       0x0C
#define IFLPF_WBW_SEL_6MHz         0

#define FGA_RC_CTRL_LOWG_5MHz   0x62   
#define FGA_RC_CTRL_HIGHG_5MHz  0x97  
#define IFLPF_BW_SEL_5MHz       0x09
#define IFLPF_WBW_SEL_5MHz         0

#define FGA_RC_CTRL_LOWG_1_7MHz   0x00   
#define FGA_RC_CTRL_HIGHG_1_7MHz  0x00  
#define IFLPF_BW_SEL_1_7MHz       0x00
#define IFLPF_WBW_SEL_1_7MHz         0


#define  FGA_RC_Table_Size 2
#define  FGA_RC_Num_Tables 11
typedef struct FGA_RC_Selection_Elements_s
{
	uint8_t  FGA_RC_CNTL;
}FGA_RC_Selection_Elements_t;

/* declare table to get FGA_RC parameters */
static const FGA_RC_Selection_Elements_t FGA_RC_Selection_Table[FGA_RC_Num_Tables][FGA_RC_Table_Size] = 
{
   {{0},{0}},		  /* <2MHz */
   {{0},{84}},		/* 2MHz */
   {{95},{150}},	/* 3MHz */ 
   {{143},{184}},	/* 4MHz */
   {{171},{205}},	/* 5MHz */
   {{190},{217}},	/* 6MHz */
   {{203},{227}},	/* 7MHz */
   {{214},{235}},	/* 8MHz */
   {{221},{240}},	/* 9MHz */
   {{228},{245}},	/* 10MHz */
   {{255},{255}}	/* >10MHz */
};


/*******************************************************************************************************
*declare tables for tuner LO and DDFS
********************************************************************************************************/

/*The BTNR_P_TunerSetFreq() function assumes this is 14 elements*/
#define BTNR_TUNER_LO_TABLE_SIZE 14
static const uint32_t Tuner_LO_Freq_Table[BTNR_TUNER_LO_TABLE_SIZE] =
{
  44790000UL,
  67180000UL,
  89580000UL,
  91110000UL,
  119440000UL,
  143160000UL,
  179160000UL, 
  200400000UL, 
  238880000UL, 
  334020000UL, 
  358330000UL, 
  537500000UL, 
  716660000UL, 
 1075000000UL  
 };

/* tunerLoDivider          used in FCW calculation 
 * fb_divn               = TNR_AFE_TNR0_03[22:15]  
 * i_MXR_SR6p8p12p16p    = TNR_AFE_TNR0_MXR_01[15:14]
 * i_MIXER_sel_div_ratio = TNR_AFE_TNR0_MXR_01[13:12]
 * i_MIXER_sel           = TNR_AFE_TNR0_MXR_01[5:4]
 * i_MIXER_HRM_mode      = TNR_AFE_TNR0_MXR_01[2]
 * i_MIXER_sel_MUX0p6p8p = TNR_AFE_TNR0_MXR_01[1:0]
 * lP_fbdivn_1p0         = TNR_AFE_TNR0_MXRPLL_03[22:15]
 * lP_div23_sel_1p0      = TNR_AFE_TNR0_MXRPLL_03[14]*/

typedef struct Tuner_LO_Tables_s
{
	uint8_t M_Factor              ;
	uint8_t i_MXR_SR6p8p12p16p    ;
	uint8_t i_MIXER_sel_div_ratio ;
	uint8_t i_MIXER_sel           ;
	uint8_t i_MIXER_HRM_mode      ;
	uint8_t i_MIXER_sel_MUX0p6p8p ;
	uint8_t lP_fbdivn_1p0         ;
	uint8_t lP_div23_sel_1p0      ;
}Tuner_VCO_Tables_t; 

static const Tuner_VCO_Tables_t Tuner_LO_Table[BTNR_TUNER_LO_TABLE_SIZE] =
{
  {0x40, 0x3, 0x2, 0x3, 0x1, 0x0, 0x10, 0x1},  /*  44790000UL*/
  {0x40, 0x3, 0x2, 0x3, 0x1, 0x0, 0x18, 0x0},  /*  67180000UL*/
  {0x20, 0x3, 0x1, 0x3, 0x1, 0x0, 0x10, 0x1},  /*  89580000UL*/
  {0x20, 0x3, 0x1, 0x3, 0x1, 0x0, 0x18, 0x0},  /*  91110000UL*/
  {0x18, 0x2, 0x1, 0x2, 0x1, 0x0, 0x10, 0x1},  /* 119440000UL*/
  {0x18, 0x2, 0x1, 0x2, 0x1, 0x0, 0x18, 0x0},  /* 143160000UL*/
  {0x10, 0x1, 0x1, 0x1, 0x1, 0x3, 0x10, 0x1},  /* 179160000UL*/
  {0x10, 0x1, 0x1, 0x1, 0x1, 0x3, 0x18, 0x0},  /* 200400000UL*/
  {0x0C, 0x0, 0x1, 0x1, 0x1, 0x2, 0x10, 0x1},  /* 238880000UL*/
  {0x0C, 0x0, 0x1, 0x1, 0x1, 0x2, 0x18, 0x0},  /* 334020000UL*/
  {0x08, 0x0, 0x3, 0x1, 0x0, 0x1, 0x10, 0x1},  /* 358330000UL*/
  {0x08, 0x0, 0x3, 0x1, 0x0, 0x1, 0x18, 0x0},  /* 537500000UL*/
  {0x04, 0x0, 0x2, 0x1, 0x0, 0x1, 0x10, 0x1},  /* 716660000UL*/
  {0x04, 0x0, 0x2, 0x1, 0x0, 0x1, 0x18, 0x0}   /*1075000000UL*/
};

/*******************************************************************************************************
*declare tables for DPM LO and DDFS
********************************************************************************************************/

/*The BTNR_P_TunerSetFreq() function assumes this is 12 elements*/
#define BTNR_DPM_LO_TABLE_SIZE 12
static const uint32_t DPM_LO_Freq_Table[BTNR_DPM_LO_TABLE_SIZE] =
{
  44790000UL,
  63300000UL,
  89580000UL,
  134400000UL,
  179160000UL, 
  200400000UL, 
  238880000UL, 
  334020000UL, 
  358330000UL, 
  507500000UL,
  716660000UL,
 1075000000UL 
 };

/* tunerLoDivider          used in FCW calculation 
 * fb_divn               = TNR_AFE_TNR0_03[22:15]  
 * i_MXR_SR6p8p12p16p    = TNR_AFE_TNR0_MXR_01[15:14]
 * i_MIXER_sel_div_ratio = TNR_AFE_TNR0_MXR_01[13:12]
 * i_MIXER_sel           = TNR_AFE_TNR0_MXR_01[5:4]
 * i_MIXER_HRM_mode      = TNR_AFE_TNR0_MXR_01[2]
 * i_MIXER_sel_MUX0p6p8p = TNR_AFE_TNR0_MXR_01[1:0]
 * lP_fbdivn_1p0         = TNR_AFE_TNR0_MXRPLL_03[22:15]
 * lP_div23_sel_1p0      = TNR_AFE_TNR0_MXRPLL_03[14]
 * logen_PreSel          = TNR_AFE_TNR0_DPM_01[22:20]
 * logen_two             = TNR_AFE_TNR0_DPM_01[6]
 * logen_six             = TNR_AFE_TNR0_DPM_01[4]*/

typedef struct DPM_LO_Tables_s
{
uint8_t M_Factor              ; 
uint8_t i_MXR_SR6p8p12p16p    ; 
uint8_t i_MIXER_sel_div_ratio ; 
uint8_t i_MIXER_sel           ; 
uint8_t i_MIXER_HRM_mode      ; 
uint8_t i_MIXER_sel_MUX0p6p8p ; 
uint8_t lP_fbdivn_1p0         ; 
uint8_t lP_div23_sel_1p0      ; 
}DPM_VCO_Tables_t; 

static const DPM_VCO_Tables_t DPM_LO_Table[BTNR_DPM_LO_TABLE_SIZE] =
{
  {0x40, 0x1, 0x3, 0x1, 0x1, 0x3, 0x10, 0x1},  /*  44790000UL*/
  {0x40, 0x1, 0x3, 0x1, 0x1, 0x3, 0x18, 0x0},  /*  63300000UL*/
  {0x20, 0x1, 0x2, 0x1, 0x1, 0x3, 0x10, 0x1},  /*  89580000UL*/
  {0x20, 0x1, 0x2, 0x1, 0x1, 0x3, 0x18, 0x0},  /* 134400000UL*/
  {0x10, 0x1, 0x1, 0x1, 0x1, 0x3, 0x10, 0x1},  /* 179160000UL*/
  {0x10, 0x1, 0x1, 0x1, 0x1, 0x3, 0x18, 0x0},  /* 200400000UL*/
  {0x0C, 0x0, 0x1, 0x1, 0x1, 0x2, 0x10, 0x1},  /* 238880000UL*/
  {0x0C, 0x0, 0x1, 0x1, 0x1, 0x2, 0x18, 0x0},  /* 334020000UL*/
  {0x08, 0x0, 0x3, 0x1, 0x0, 0x1, 0x10, 0x1},  /* 358330000UL*/
  {0x08, 0x0, 0x3, 0x1, 0x0, 0x1, 0x18, 0x0},  /* 507500000UL*/
  {0x04, 0x0, 0x2, 0x1, 0x0, 0x1, 0x10, 0x1},  /* 716660000UL*/
  {0x04, 0x0, 0x2, 0x1, 0x0, 0x1, 0x18, 0x0},  /*1075000000UL*/
};
/*******************************************************************************************************
*declare tables for DPM 
********************************************************************************************************/

/*The BTNR_P_TunerSetFreq() function assumes this is 12 elements*/
#define BTNR_DPM_TABLE_SIZE 12
static const uint32_t DPM_Freq_Table[BTNR_DPM_TABLE_SIZE] =
{
  44790000UL,
  63300000UL,
  89580000UL,
  134400000UL,
  179160000UL, 
  200400000UL, 
  238880000UL, 
  334020000UL, 
  358330000UL, 
  507500000UL,
  716660000UL,
 1075000000UL 
 };

/* logen_PreSel          = TNR_AFE_TNR0_DPM_01[22:20]
 * logen_two             = TNR_AFE_TNR0_DPM_01[6]
 * logen_six             = TNR_AFE_TNR0_DPM_01[4]*/

typedef struct DPM_Tables_s
{
uint8_t logen_PreSel          ; 
uint8_t logen_two             ; 
uint8_t logen_six             ; 
}DPM_Tables_t; 


static const DPM_Tables_t DPM_Table[BTNR_DPM_TABLE_SIZE] =

{
  {0x3, 0x0, 0x0},  /*  44790000UL*/
  {0x3, 0x0, 0x0},  /*  63300000UL*/
  {0x2, 0x0, 0x0},  /*  89580000UL*/
  {0x2, 0x0, 0x0},  /* 134400000UL*/
  {0x1, 0x0, 0x0},  /* 179160000UL*/
  {0x1, 0x0, 0x0},  /* 200400000UL*/
  {0x4, 0x0, 0x0},  /* 238880000UL*/
  {0x4, 0x0, 0x0},  /* 334020000UL*/
  {0x0, 0x0, 0x0},  /* 358330000UL*/
  {0x0, 0x0, 0x0},  /* 507500000UL*/
  {0x1, 0x1, 0x0},  /* 716660000UL*/
  {0x1, 0x1, 0x0},  /*1075000000UL*/
};

/*******************************************************************************************************
*declare tables for Gain Caculations 
********************************************************************************************************/
#ifdef NEW_GAIN_CODE

static const int16_t LNA_Ctrl_Bias[16] = {-1160, -935, -775, -634, -525, -433, -227, -168, -123, -78, -37, 0, 0, 0, 0, 0};
static const int16_t RFVGA_Ctrl_Rdeg[8] = {-1196, -831, -577, -398, -263, -159, -81, 0};
static const int16_t RFVGA_Ctrl_Bias[16] = {-463, -208, 0, 152, 281, 384, 481, 557, 620, 679, 727, 781, 825, 861, 895, 925};
static const int16_t TRKFIL_Buf_I_Ctrl[16] = {-8375, -926, -346, -155, -57, -1, 36, 61, 0, 34, 60, 78, 90, 100, 98, 111};
static const int16_t MIXER_Bias_Ctrl[16] = {0, 48, 88, 114, 148, 176, 195, 213, 228, 231, 234, 228, 216, 200, 166, 153};

static const int16_t LNAcode[8][30] = 
{	{-7552, -7248, -6905, -6562, -6203, -5817, -5438, -5033, -5871, -5412, -5095, -4775, -4450, -4153, -3858, -3605, -3313, -2978, -2944, -2686, -2440, -2176, -1900, -1593, -1337, -1078, -822, -528, -267, 0},
	{-7524, -7227, -6905, -6569, -6208, -5827, -5458, -5049, -5835, -5392, -5085, -4767, -4455, -4158, -3869, -3618, -3331, -2996, -2947, -2694, -2445, -2176, -1892, -1577, -1319, -1060, -804, -518, -259, 0},
	{-7535, -7227, -6928, -6598, -6257, -5878, -5497, -5100, -5801, -5387, -5097, -4788, -4486, -4194, -3912, -3666, -3382, -3049, -2970, -2742, -2486, -2205, -1903, -1562, -1304, -1045, -789, -502, -251, 0},
	{-7488, -7235, -6928, -6618, -6280, -5917, -5556, -5156, -5740, -5364, -5087, -4800, -4506, -4227, -3943, -3705, -3423, -3101, -2978, -2768, -2512, -2225, -1910, -1552, -1291, -1035, -776, -489, -244, 0},
	{-7427, -7197, -6907, -6618, -6296, -5945, -5586, -5202, -5648, -5310, -5056, -4785, -4501, -4235, -3961, -3728, -3456, -3134, -2962, -2745, -2494, -2210, -1895, -1529, -1270, -1012, -756, -474, -239, 0},
	{-7368, -7151, -6900, -6621, -6313, -5978, -5632, -5256, -5551, -5254, -5028, -4777, -4509, -4250, -3989, -3764, -3497, -3185, -2955, -2714, -2473, -2189, -1872, -1496, -1237, -984, -730, -456, -226, 0 },
	{-7312, -7151, -6905, -6644, -6360, -6029, -5696, -5333, -5466, -5210, -5008, -4780, -4527, -4283, -4032, -3812, -3559, -3247, -2962, -2706, -2471, -2189, -1869, -1480, -1219, -968, -715, -443, -221, 0 },
	{-7271, -7128, -6918, -6669, -6388, -6088, -5760, -5407, -5384, -5167, -4990, -4777, -4544, -4319, -4076, -3864, -3615, -3318, -2960, -2676, -2453, -2182, -1859, -1465, -1206, -958, -704, -433, -216, 0 } 
}; /*[Freq:100Mhz to 800Mhz][Lna_Gain_Code]*/

static const int16_t RFVGAcode[8][17] = 
{	{-4886, -4885, -4888, -4851, -4425, -3554, -2561, -1816, -1529, -1351, -1098, -709, -262, -50, -10, -10, 0},
	{-5293, -5293, -5290, -5254, -4738, -3760, -2681, -1897, -1599, -1414, -1155, -749, -292, -70, -25, -1, 0},
	{-5553, -5554, -5557, -5507, -4930, -3874, -2747, -1942, -1639, -1451, -1186, -773, -308, -79, -29, -1, 0},
	{-5745, -5743, -5747, -5694, -5068, -3960, -2796, -1977, -1670, -1479, -1210, -793, -321, -87, -31, 0, 0},
	{-5898, -5894, -5899, -5836, -5175, -4028, -2837, -2007, -1695, -1505, -1233, -813, -335, -96, -38, 0, 0},
	{-6007, -6008, -6007, -5943, -5261, -4072, -2862, -2022, -1711, -1520, -1247, -823, -344, -99, -39, 0, 0},
	{-6101, -6100, -6100, -6040, -5319, -4111, -2888, -2042, -1727, -1534, -1262, -834, -351, -103, -40, -1, 0},
	{-6174, -6174, -6176, -6107, -5374, -4141, -2906, -2055, -1740, -1545, -1271, -842, -357, -109, -41, 2, 0},
}; /*[Rdeg][RFVGA_Gain_Code: in 4096 steps]*/


static const int16_t FREQ_Rsp_Moca[8] = {-315, -131, -87, 107, 83, 24, 0, -120};
static const int16_t FREQ_Rsp_Rffil[8] = {-315, -487, -361, -303, -485, 24, 0, -120};

#endif


/*****************************************************************************
 * TNR Function Prototypes Used by PI or Local 
 *****************************************************************************/
BERR_Code BTNR_P_TunerSetInputMode(BTNR_3x7x_ChnHandle h);
BERR_Code BTNR_P_TunerStatusReset(BTNR_3x7x_ChnHandle h);
BERR_Code BTNR_P_TunerStatus(BTNR_3x7x_ChnHandle h);
BERR_Code BTNR_P_TunerInit(BTNR_3x7x_ChnHandle h);
BERR_Code BTNR_P_TunerTune(BTNR_3x7x_ChnHandle h);
BERR_Code BTNR_P_DPM_Control(BTNR_3x7x_ChnHandle h);
BERR_Code BTNR_P_LoopThru_Control(BTNR_3x7x_ChnHandle h);
BERR_Code BTNR_P_Daisy_Control(BTNR_3x7x_ChnHandle h);
BERR_Code BTNR_P_Calculate_RFout_Gains(BTNR_3x7x_ChnHandle h);

/*****************************************************************************
 * TNR Function Prototypes Used Local 
 *****************************************************************************/
void BTNR_P_TunerSetDither(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerSetRFFIL(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerSetFGA_IFLPF(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerSetFreq(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerSearchCap(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerSetADC6B(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerSetLNAAGC(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerSetRFAGC(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerSetDCO(BTNR_3x7x_ChnHandle h);
void BTNR_P_CalFlashSDADC(BTNR_3x7x_ChnHandle h);
void BTNR_P_DPMSetFreq(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerBypassRFAGC(BTNR_3x7x_ChnHandle h);
BERR_Code BTNR_P_Tuner_PowerUpPLL(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerSetSignalPath(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerAGCMonitor(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerSetDCODOWN(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerAGCReadBack(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerSleep(uint8_t x);
void BTNR_P_TunerRFAGCIndirectWrite(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerRFAGCIndirectRead(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerSetExternalFGLNASwitch(BTNR_3x7x_ChnHandle h);
void BTNR_P_TunerSetADC6BDOWN(BTNR_3x7x_ChnHandle h);


#ifdef BTNR_ENABLE_SOFTWARE_TUNE
BERR_Code BTNR_P_TunerSearchCap(BTNR_3x7x_ChnHandle h);
BERR_Code BTNR_P_TunerSetCapCntlLoopParams(BTNR_3x7x_ChnHandle h);
BERR_Code BTNR_P_TunerSetCapCntl(BTNR_3x7x_ChnHandle h);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _BTNR_TUNE_H__ */


