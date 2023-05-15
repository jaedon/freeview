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
 * $brcm_Workfile: bads_struct.h $
 * $brcm_Revision: 31 $
 * $brcm_Date: 10/2/12 7:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AP/ctfe/core/ads/bads_struct.h $
 * 
 * 31   10/2/12 7:00p farshidf
 * SW3128-231: FW version 5.7.0.0
 * 
 * Fw_Integration_Devel/10   10/2/12 6:33p farshidf
 * SW3128-231: FW version 5.7.0.0
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/6   9/27/12 4:20p cbrooks
 * sw3128-227:Added Edefine in defs.h option for equalizer size, default
 *  is 36/36 w/18 overlap, option i n 18/24 w/6 overlap
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/5   9/14/12 10:11a mpovich
 * SW3128-207:  3128 ADS cleanup and removal of unused HAB commands (per
 *  CBrooks).
 * 
 * Fw_Integration_Devel/8   8/30/12 2:19p farshidf
 * SW3128-213: 3128 Fw release 5.6
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/4   8/27/12 11:57a cbrooks
 * sw3128-1:code cleanup
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/3   8/23/12 6:10p mpovich
 * SW3128-207: Remove use of driver local parameters from API level
 *  module.
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/2   8/23/12 3:43p cbrooks
 * sw3128-1:removed ReSync_Count
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/1   8/17/12 4:23p cbrooks
 * sw3128-1:Code Optimization/Clean up
 * 
 * Fw_Integration_Devel/7   10/12/11 10:40a farshidf
 * SW3128-54: merge to Integ branch
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/3   10/10/11 2:36p farshidf
 * SW7552-134: compile correction
 * 
 * Fw_Integration_Devel/6   8/24/11 11:59a farshidf
 * SW3461-38: merge to integ
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/2   8/23/11 4:49p mpovich
 * SW3128-24: Merge latest of SW3128-24 to devel. branch.
 * 
 * Fw_Integration_Devel/SW3128-24/7   8/23/11 4:30p mpovich
 * SW3128-24: Rebase with 2.0 devel. branch.
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/1   8/18/11 1:34p mpovich
 * SW3128-24: Merge Scan feature support into the devel. branch.
 * 
 * Fw_Integration_Devel/5   8/16/11 3:59p farshidf
 * SW3461-1: merge to integ
 * 
 * 26   8/16/11 3:58p farshidf
 * SW3461-1: magnum compatible
 * 
 * 25   8/12/11 3:13p farshidf
 * SW3461-1: merge to main
 * 
 * Fw_Integration_Devel/3   8/12/11 2:11p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/SW3128-24/6   8/17/11 10:13a mpovich
 * SW3128-24: Update from SW3128-32.
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/SW3128-32/2   8/16/11 7:35p mpovich
 * SW3128-32: Assign Scan Status params inside BADS_P_Acquire().
 * 
 * Fw_Integration_Devel/SW3128-24/5   8/16/11 6:39p mpovich
 * SW3128-24: Pick up SW3128-32, "Get Scan Status" HAB command
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/SW3128-32/1   8/16/11 9:59a mpovich
 * SW3128-32: Add Downstream Get Scan Status HAB command.
 * 
 * Fw_Integration_Devel/SW3128-24/4   8/11/11 5:30p mpovich
 * SW3128-24: Rebase to latest from devel. branch.
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/3   8/11/11 10:21a mpovich
 * SW3128-31: Ensure ADS channel status may be obtained concurrently.
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/SW3128-31/1   8/11/11 10:10a mpovich
 * SW3128-31: Ensure channel status may be obtained concurrently.
 * 
 * Fw_Integration_Devel/SW3128-24/3   8/1/11 6:32p mpovich
 * SW3128-24: Rebase with ADS_DEV branch.
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/2   7/27/11 7:58p cbrooks
 * sw3128-1:Added early exit
 * 
 * Fw_Integration_Devel/SW3128-24/2   7/25/11 2:10p mpovich
 * SW3128-24: Rebase with updates from ADS devel. branch.
 * 
 * Fw_Integration_Devel/SW3128-24/1   7/25/11 10:14a mpovich
 * SW3128-24: Assert initial "unlock" IRQ for DS channel that is tuned for
 *  (WFE), tuned (WFE)/acquired, or acquired w/o a preceding tune.  Add
 *  "System" events for WFE tune, ADS channel being tuned, ADS acquire,
 *  etc.
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/1   7/22/11 4:52p cbrooks
 * sw3128-1:Added SLow Scan for RFI interference
 * 
 * Fw_Integration_Devel/1   6/29/11 12:38p farshidf
 * SW3461-13: merge to integration branch
 * 
 * Fw_Integration_Devel/Ads_Fw_Devel_Rc04/2   6/28/11 6:49p cbrooks
 * sw3128-1:added main tap
 * 
 * Fw_Integration_Devel/Ads_Fw_Devel_Rc04/1   6/21/11 5:18p cbrooks
 * sw3128-1:Added racquire count
 * 
 * 22   6/9/11 6:16p mpovich
 * SW3461-1: Merge Ver 0.4 Integ. onto main branch.
 * 
 * SW_System_4_Integ_Test/3   6/9/11 2:17p mpovich
 * SW3461-1: Rebase with main branch.
 * 
 * 21   6/9/11 12:55p farshidf
 * SW3128-1: do not report lock until acquie
 * 
 * 20   6/7/11 3:16p farshidf
 * SW3128-1: merge to main
 * 
 * SW_System_4_Integ_Test/2   6/7/11 1:51p farshidf
 * SW3128-1: sync up with backend
 * 
 * SW_System_4_Integ_Test/1   6/6/11 2:05p mpovich
 * SW3461-1: Integ. test all development branches together.
 * 
 * Ads_Fw_Devel_3/1   5/27/11 3:27p cbrooks
 * sw3128-1:Added second timer for Mark
 * 
 * 19   5/11/11 5:03p farshidf
 * SW3128-1: merge main
 * 
 * Hydra_Software_Devel/SWDTV-7035/5   5/10/11 5:54p farshidf
 * SWDTV-7035: put back teh NExus falg
 * 
 * Hydra_Software_Devel/SWDTV-7035/4   5/10/11 5:50p cbrooks
 * swdtv-7035:BBS support
 * 
 * 17   5/10/11 4:01p farshidf
 * SW3128-1: update teh structure
 * 
 * 16   5/10/11 3:17p farshidf
 * SW3128-1: merge main
 * 
 * ADS_3128_3/5   5/11/11 4:23p cbrooks
 * sw3128-1:merging 330 changes
 * 
 * ADS_3128_3/4   5/10/11 3:16p farshidf
 * SW3128-1: remove the nexus flag
 * 
 * ADS_3128_3/3   5/10/11 11:54a farshidf
 * SW3128-1: update th enum
 * 
 * ADS_3128_3/2   5/10/11 11:00a cbrooks
 * sw3128-1:Changed status
 * 
 * ADS_3128_3/1   5/1/11 3:30p cbrooks
 * sw3128-1:Cleaned up Channel Scan Code
 * 
 * 15   4/26/11 6:51p farshidf
 * SW3128-1: merge main
 * 
 * ADS_3128_2/4   4/26/11 5:51p farshidf
 * SW3128: compile fix
 * 
 * ADS_3128_2/3   4/26/11 5:12p farshidf
 * SW3128-1: merge main
 * 
 * 14   4/26/11 4:36p farshidf
 * SW3128-1: update the acquire param matching verion 0.9 of the HAb doc
 * 
 * 13   4/26/11 4:25p farshidf
 * SW3128-1: update the acquire param per request
 * 
 * 12   4/25/11 10:23a farshidf
 * SW3461-1: sync up with 35330
 * 
 * Hydra_Software_Devel/1   4/20/11 12:37p farshidf
 * SWDTV-6190: add intial DS files
 * 
 * 11   4/19/11 9:56p farshidf
 * SW3128-1: set an IRQ for each acquire
 * 
 * 10   4/15/11 4:56p farshidf
 * SW3128-1: compile fix
 * 
 * 9   4/15/11 4:38p farshidf
 * SW3128-1: merge main
 * 
 * ADS_3128_2/1   4/11/11 8:31p cbrooks
 * SW3128-1:Added CWC code
 * 
 * 7   3/23/11 10:57a farshidf
 * SW3128-1: add new element
 * 
 * 6   3/18/11 4:32p farshidf
 * SW3461-1: merge  main
 * 
 * ADS_3128_1/1   3/17/11 12:30p cbrooks
 * sw3128-1:Added BERT resync warning
 * 
 * 5   2/28/11 5:58p cbrooks
 * sw3128-1:Changed acqwords to internal_params
 * 
 * 4   2/21/11 8:55p cbrooks
 * sw3128-1:new coe
 * 
 * 3   2/3/11 6:34p farshidf
 * SW3128-1: make AGF 16bit int
 * 
 * 2   1/28/11 3:36p cbrooks
 * sw3128-1:new file
 * 
 * 1   1/26/11 4:38p farshidf
 * SW3128-1: new file
 * 
 ***************************************************************************/
#ifndef _BADS_STRUCT_H__
#define _BADS_STRUCT_H__

#ifdef __cplusplus
extern "C" {
#endif


/**************************************************************************************** 
 * BADS HAB Command Structures  
 ****************************************************************************************/

typedef enum BADS_Acquire_Params_Annex_s
{
	BADS_Acquire_Params_Annex_eAnnexA=0,
	BADS_Acquire_Params_Annex_eAnnexB=1
}BADS_Acquire_Params_Annex_t;

typedef enum BADS_Acquire_Params_BPS_s
{
	BADS_Acquire_Params_BPS_eQam16=0,
	BADS_Acquire_Params_BPS_eQam32=1,
	BADS_Acquire_Params_BPS_eQam64=2,
	BADS_Acquire_Params_BPS_eQam128=3,
	BADS_Acquire_Params_BPS_eQam256=4,
	BADS_Acquire_Params_BPS_eQam512=5,
	BADS_Acquire_Params_BPS_eQam1024=6
}BADS_Acquire_Params_BPS_t;

typedef enum BADS_Acquire_Params_OffOn_s
{
  BADS_Acquire_Params_eDisable=0, 
  BADS_Acquire_Params_eEnable=1
}BADS_Acquire_Params_OffOn_t;

typedef enum BADS_AcquireStartMode
{
  BADS_AcquireStartMode_Acquire = 1, 
  BADS_AcquireStartMode_ResetStatus = 2
} BADS_AcquireStartMode_t;

typedef enum BADS_Acquire_AcqType_s
{
	BADS_Acquire_Params_AcqType_eAuto=0,
	BADS_Acquire_Params_AcqType_eFast=1,
	BADS_Acquire_Params_AcqType_eSlow=2,
	BADS_Acquire_Params_AcqType_eScan=3
}BADS_Acquire_AcqType_t;

#define BADS_NexusStatusMode_EnableStatusForNexus 1

typedef struct BADS_Acquire_Params_s
{	
	volatile BADS_AcquireStartMode_t	AcquireStartMode;      /*Added for PI*/
	uint32_t							NexusStatusMode;       /*Added for PI*/
	BADS_Acquire_Params_OffOn_t         Acquire_After_Tune_te;
	BADS_Acquire_AcqType_t				AcqType_te;
	BADS_Acquire_Params_OffOn_t			Auto_te;
	BADS_Acquire_Params_OffOn_t			IS_te;
	BADS_Acquire_Params_Annex_t			Annex_te;	
	BADS_Acquire_Params_BPS_t			Qam_Mode_te;
	uint32_t					        Carrier_Range_u32;	
	uint32_t							AnnexA_Sym_Rate_u32;
}BADS_Acquire_Params_t;

typedef enum BADS_Scan_Params_OffOn_s
{
  BADS_Scan_Params_eDisable=0, 
  BADS_Scan_Params_eEnable=1
}BADS_Scan_Params_OffOn_t;

typedef struct BADS_Scan_Params_s
{
	BADS_Scan_Params_OffOn_t		AI_te;
	BADS_Scan_Params_OffOn_t		QM_te;
	BADS_Scan_Params_OffOn_t		CO_te;
	BADS_Scan_Params_OffOn_t		TO_te;
	BADS_Scan_Params_OffOn_t		B1024_te;
	BADS_Scan_Params_OffOn_t		B256_te;
	BADS_Scan_Params_OffOn_t		B64_te;
	BADS_Scan_Params_OffOn_t		A1024_te;
	BADS_Scan_Params_OffOn_t		A512_te;
	BADS_Scan_Params_OffOn_t		A256_te;
	BADS_Scan_Params_OffOn_t		A128_te;
	BADS_Scan_Params_OffOn_t		A64_te;
	BADS_Scan_Params_OffOn_t		A32_te;
	BADS_Scan_Params_OffOn_t		A16_te;
	uint32_t						Carrier_Search_u32;
	uint32_t						Upper_Baud_Search_u32;
	uint32_t						Lower_Baud_Search_u32;
}BADS_Scan_Params_t;

/*Put all local variables that need to be global here
 *These values need to be initialized BADS_P_Initialize()*/

/*This structure is also used as an entry into a LUT so don't change*/
typedef enum BADS_Local_Params_Annex_s
{
	BADS_Local_Params_Annex_eAnnexA = 0,
	BADS_Local_Params_Annex_eAnnexB = 1
}BADS_Local_Params_Annex_t;

/*This structure is also used as an entry into a LUT so don't change*/
typedef enum BADS_Local_Params_QAM_s
{
	BADS_Local_Params_QAM_eQam16 = 0,
	BADS_Local_Params_QAM_eQam32 = 1,
	BADS_Local_Params_QAM_eQam64 = 2,
	BADS_Local_Params_QAM_eQam128 = 3,
	BADS_Local_Params_QAM_eQam256 = 4,
	BADS_Local_Params_QAM_eQam512 = 5,
	BADS_Local_Params_QAM_eQam1024 = 6
}BADS_Local_Params_QAM_t;

typedef enum BADS_Local_Params_FECSpectrum_s
{
	BADS_Local_Params_FECSpectrum_eNotInverted=0,
	BADS_Local_Params_FECSpectrum_eInverted=1,
	BADS_Local_Params_FECSpectrum_eNotInvertedAutoInvert=2,
	BADS_Local_Params_FECSpectrum_eInvertedAutoInvert=3
}BADS_Local_Params_FECSpectrum_t;

typedef enum BADS_Local_Params_OffOn_s
{
  BADS_Local_Params_eDisable=0, 
  BADS_Local_Params_eEnable=1
}BADS_Local_Params_OffOn_t;

typedef enum BADS_Local_Params_AcqType_s
{
	BADS_Local_Params_AcqType_eFastAcquire=0,
	BADS_Local_Params_AcqType_eSlowAcquire=1,
	BADS_Local_Params_AcqType_eScan=2,
	BADS_Local_Params_AcqType_eSlowAcquireScan=3
}BADS_Local_Params_AcqType_t;

typedef enum BADS_Local_Params_AcqStatus_s
{
	BADS_Local_Params_AcqStatus_eNoPower=0,
	BADS_Local_Params_AcqStatus_eNoTiming=1,
	BADS_Local_Params_AcqStatus_eNoCarrier=2,
	BADS_Local_Params_AcqStatus_eNoFECLock=3,
	BADS_Local_Params_AcqStatus_eLockedFast=4,
	BADS_Local_Params_AcqStatus_eLockedSlow=5,
	BADS_Local_Params_AcqStatus_eEarlyExit=6
}BADS_Local_Params_AcqStatus_t;

typedef struct BADS_Local_Params_s                    
{
	BADS_Local_Params_AcqType_t     AcqType_te;
	BADS_Local_Params_AcqStatus_t   AcqStatus_te;
	uint32_t                        Carrier_Search_u32;
	uint32_t                        Upper_Baud_Search_u32;
	uint32_t                        Lower_Baud_Search_u32;
	bool                            Q1024A_b;
	bool                            Q512A_b;
	bool                            Q256A_b;
	bool                            Q256B_b;
	bool                            Q64A_b;
	bool                            Q64B_b;	
	bool                            Q16A_b;
	bool                            Q128A_b;
	bool                            Q32A_b;
	bool                            Q1024B_b;
	bool                            IS_b;
	bool                            Flip_Spectrum_b;
	BADS_Local_Params_OffOn_t       DoneFirstTimeFlag_te;					
	BADS_Local_Params_Annex_t       Annex_te;
	BADS_Local_Params_QAM_t         QAM_te;
	uint32_t                        Old_UERC1_u32;
	uint32_t                        Old_NBERC1_u32;
	uint32_t                        Old_CBERC1_u32;
	uint32_t                        BadBlockCount_u32;
	uint32_t                        StuckFECCount_u32;
	BADS_Local_Params_OffOn_t       TestLockFlag_te;
	BADS_Local_Params_FECSpectrum_t FECSpectrum_te;
	uint32_t                        ElapsedTime_u32;
	uint32_t                        TotalTime_u32;
	uint32_t						SampleRate_u32;
	uint32_t						VIDRate_u32;
	bool                            EarlyExit;    /* (this should stay local to bads local params) */
}BADS_Local_Params_t;

/*Put all variables that BBS needs to change here
 *These values need to be initialized by the BADS_P_Initialize()*/

typedef enum BADS_Internal_Params_OffOn_s
{
  BADS_Internal_Params_eDisable=0, 
  BADS_Internal_Params_eEnable=1
}BADS_Internal_Params_OffOn_t;

typedef struct BADS_Internal_Params_s
{
	BADS_Internal_Params_OffOn_t			Acquisition_Test_te;
	BADS_Internal_Params_OffOn_t			BigEQ_te;
	BADS_Internal_Params_OffOn_t			CWC_te;
	BADS_Internal_Params_OffOn_t			CFL_te;
	BADS_Internal_Params_OffOn_t			DDAGC_te;
	BADS_Internal_Params_OffOn_t			IMC_te;
	BADS_Internal_Params_OffOn_t			Video_Carrier_Annex_B_te;
	BADS_Internal_Params_OffOn_t			Dual_Scan_te;
	BADS_Internal_Params_OffOn_t			FOI_Timing_te;
	BADS_Internal_Params_OffOn_t			Callback_Enable_te;
	uint32_t								Timing_Scan_Threshold_u32;
	uint32_t								Carrier_Scan_Threshold_u32;
}BADS_Internal_Params_t;

/*This is the main structure by the ADS acquisition functions*/
typedef struct BADS_3x7x_AcqParams_s
{
	BADS_Acquire_Params_t   BADS_Acquire_Params;
	BADS_Scan_Params_t      BADS_Scan_Params;
	BADS_Internal_Params_t  BADS_Internal_Params;
	BADS_Local_Params_t     BADS_Local_Params;
}BADS_3x7x_AcqParams_t;
 
/***************************************************************/
/***************************************************************/

typedef enum BADS_3x7x_ChnLockStatus_UnlockLock_s
{
  BADS_3x7x_ChnLockStatus_eUnlock=0, 
  BADS_3x7x_ChnLockStatus_eLock=1
}BADS_3x7x_ChnLockStatus_UnlockLock_t;

typedef struct BADS_3x7x_ChnLockStatus_s
{
	BADS_3x7x_ChnLockStatus_UnlockLock_t  QLK_te;
	BADS_3x7x_ChnLockStatus_UnlockLock_t  FLK_te;
    uint32_t                              NoSignal_b;
	uint32_t							  SNR_u32;
	uint32_t							  SNRAVG_u32;
	uint32_t							  ReAck_Count_u32;
	uint32_t							  ReAcquire_Count_u32;
}BADS_3x7x_ChnLockStatus_t;

/***************************************************************/
/***************************************************************/
typedef enum BADS_3x7x_ChnStatus_AcqType_s
{
	BADS_3x7x_ChnStatus_AcqType_eFastAcquire=0,
	BADS_3x7x_ChnStatus_AcqType_eSlowAcquire=1,
	BADS_3x7x_ChnStatus_AcqType_eScan=2,
	BADS_3x7x_ChnStatus_AcqType_eSlowAcquireScan=3
}BADS_3x7x_ChnStatus_AcqType_t;

typedef enum BADS_3x7x_ChnStatus_AcqStatus_s
{
	BADS_3x7x_ChnStatus_AcqStatus_eNoPower=0,
	BADS_3x7x_ChnStatus_AcqStatus_eNoTiming=1,
	BADS_3x7x_ChnStatus_AcqStatus_eNoCarrier=2,
	BADS_3x7x_ChnStatus_AcqStatus_eNoFECLock=3,
	BADS_3x7x_ChnStatus_AcqStatus_eLockedFast=4,
	BADS_3x7x_ChnStatus_AcqStatus_eLockedSlow=5,
	BADS_3x7x_ChnStatus_AcqStatus_eEarlyExit=6
}BADS_3x7x_ChnStatus_AcqStatus_t;

typedef enum BADS_3x7x_ChnStatus_Annex_s
{
	BADS_3x7x_ChnStatus_Annex_eAnnexA=0,
	BADS_3x7x_ChnStatus_Annex_eAnnexB=1 
}BADS_3x7x_ChnStatus_Annex_t;

typedef enum BADS_3x7x_ChnStatus_BPS_s
{
	BADS_3x7x_ChnStatus_BPS_eQam16=0,
	BADS_3x7x_ChnStatus_BPS_eQam32=1,
	BADS_3x7x_ChnStatus_BPS_eQam64=2,
	BADS_3x7x_ChnStatus_BPS_eQam128=3,
	BADS_3x7x_ChnStatus_BPS_eQam256=4,
	BADS_3x7x_ChnStatus_BPS_eQam512=5,
	BADS_3x7x_ChnStatus_BPS_eQam1024=6
}BADS_3x7x_ChnStatus_BPS_t;

typedef enum BADS_3x7x_ChnStatus_UnlockLock_s
{
  BADS_3x7x_ChnStatus_eUnlock=0, 
  BADS_3x7x_ChnStatus_eLock=1
}BADS_3x7x_ChnStatus_UnlockLock_t;

typedef enum BADS_3x7x_ChnStatus_OffOn_s
{
  BADS_3x7x_ChnStatus_eDisable=0, 
  BADS_3x7x_ChnStatus_eEnable=1
}BADS_3x7x_ChnStatus_OffOn_t;

typedef struct BADS_3x7x_ChnStatus_s
{
	BADS_3x7x_ChnStatus_OffOn_t			Auto_te;
	BADS_3x7x_ChnStatus_UnlockLock_t	FLK_te; 
	BADS_3x7x_ChnStatus_UnlockLock_t	QLK_te; 
	BADS_3x7x_ChnStatus_AcqType_t	    AcqType_te; 
    BADS_3x7x_ChnStatus_AcqStatus_t     AcqStatus_te;
	BADS_3x7x_ChnStatus_OffOn_t			AI_te;
	BADS_3x7x_ChnStatus_Annex_t			AB_te;
	BADS_3x7x_ChnStatus_BPS_t			BPS_te;
	BADS_3x7x_ChnStatus_OffOn_t			B1024_te;
	BADS_3x7x_ChnStatus_OffOn_t			B256_te;
	BADS_3x7x_ChnStatus_OffOn_t			B64_te;
	BADS_3x7x_ChnStatus_OffOn_t			A1024_te;
	BADS_3x7x_ChnStatus_OffOn_t			A512_te;
	BADS_3x7x_ChnStatus_OffOn_t			A256_te;
	BADS_3x7x_ChnStatus_OffOn_t			A128_te;
	BADS_3x7x_ChnStatus_OffOn_t			A64_te;
	BADS_3x7x_ChnStatus_OffOn_t			A32_te;
	BADS_3x7x_ChnStatus_OffOn_t			A16_te;
	BADS_3x7x_ChnStatus_OffOn_t     	SI_te; 
	uint32_t                            Interleaver_u32;
	uint32_t							ReAck_Count_u32;
	uint32_t							ReSync_Count_u32;
	uint32_t							AcquisitionTime1_u32;
	uint32_t							AcquisitionTime2_u32;
	uint32_t							SNR_db_u32;
	uint32_t							SNRAVG_db_u32;
	int32_t								EstChannelPower_dbm_i32;					
	int32_t								FrontEndGain_db_i32;
	int32_t								AGCB_Gain_db_i32;
	int32_t								EQ_Gain_db_i32;
	uint32_t							Carrier_Scan_u32;
	int32_t								Carrier_Error_i32;
	int32_t								Symbol_Error_i32;
	int32_t								Phase_Error_i32;
	uint32_t							FEC_Corr_RS_Bits_u32;
	uint32_t							FEC_Corr_RS_Blocks_u32;
	uint32_t							FEC_UCorr_RS_Blocks_u32;
	uint32_t							FEC_Clean_RS_Blocks_u32;
	uint32_t							PRE_FEC_BERT_u32;
	uint32_t							BERT_u32;
	uint32_t                            RF_Frequency_u32;
	int32_t	                            RF_Offset_i32;
	uint32_t                            Upper_Symbol_Scan_u32;
	uint32_t                            Lower_Symbol_Scan_u32;
	uint32_t							Symbol_Rate_u32;
	int32_t								EQ_Main_Tap_i32;
}BADS_3x7x_ChnStatus_t;

/* Structures for ScanStatus*/
typedef enum BADS_3x7x_ChnScanStatus_AcqStatus_s
{
      BADS_3x7x_ChnScanStatus_AcqStatus_eNoPower=0,
      BADS_3x7x_ChnScanStatus_AcqStatus_eNoTiming=1,
      BADS_3x7x_ChnScanStatus_AcqStatus_eNoCarrier=2,
      BADS_3x7x_ChnScanStatus_AcqStatus_eNoFECLock=3,
      BADS_3x7x_ChnScanStatus_AcqStatus_eLockedFast=4,
      BADS_3x7x_ChnScanStatus_AcqStatus_eLockedSlow=5,
      BADS_3x7x_ChnScanStatus_AcqStatus_eEarlyExit=6
}BADS_3x7x_ChnScanStatus_AcqStatus_t;

typedef enum BADS_3x7x_ChnScanStatus_BPS_s
{
      BADS_3x7x_ChnScanStatus_BPS_eQam16=0,
      BADS_3x7x_ChnScanStatus_BPS_eQam32=1,
      BADS_3x7x_ChnScanStatus_BPS_eQam64=2,
      BADS_3x7x_ChnScanStatus_BPS_eQam128=3,
      BADS_3x7x_ChnScanStatus_BPS_eQam256=4,
      BADS_3x7x_ChnScanStatus_BPS_eQam512=5,
      BADS_3x7x_ChnScanStatus_BPS_eQam1024=6
}BADS_3x7x_ChnScanStatus_BPS_t;

typedef enum BADS_3x7x_ChnScanStatus_Annex_s
{
      BADS_3x7x_ChnScanStatus_Annex_eAnnexA=0,
      BADS_3x7x_ChnScanStatus_Annex_eAnnexB=1 
}BADS_3x7x_ChnScanStatus_Annex_t;

typedef enum BADS_3x7x_ChnScanStatus_Interleaver_s
{
      BADS_3x7x_ChnScanStatus_Interleaver_eI128_J1=0,
      BADS_3x7x_ChnScanStatus_Interleaver_eI128_J2=1,
      BADS_3x7x_ChnScanStatus_Interleaver_eI128_J3=2,
      BADS_3x7x_ChnScanStatus_Interleaver_eI128_J4=3,
      BADS_3x7x_ChnScanStatus_Interleaver_eI64_J2=4,
      BADS_3x7x_ChnScanStatus_Interleaver_eI32_J4=5,
      BADS_3x7x_ChnScanStatus_Interleaver_eI16_J8=6,
      BADS_3x7x_ChnScanStatus_Interleaver_eI8_J16=7,
      BADS_3x7x_ChnScanStatus_Interleaver_eI4_J32=8,
      BADS_3x7x_ChnScanStatus_Interleaver_eI2_J64=9,
      BADS_3x7x_ChnScanStatus_Interleaver_eI1_J128=10,
      BADS_3x7x_ChnScanStatus_Interleaver_eI12_J17=11,
      BADS_3x7x_ChnScanStatus_Interleaver_eUnsupported=12
}BADS_3x7x_ChnScanStatus_Interleaver_t;

typedef enum BADS_3x7x_ChnScanStatus_Spectrum_s
{
      BADS_3x7x_ChnScanStatus_Spectrum_eNormal=0,
      BADS_3x7x_ChnScanStatus_Spectrum_eFlipped=1
}BADS_3x7x_ChnScanStatus_Spectrum_t;

typedef enum BADS_3x7x_ChnScanStatus_OffOn_s
{
      BADS_3x7x_ChnScanStatus_eDisable=0, 
      BADS_3x7x_ChnScanStatus_eEnable=1 
}BADS_3x7x_ChnScanStatus_OffOn_t;

typedef enum BADS_3x7x_ChnScanStatus_UnlockLock_s
{
      BADS_3x7x_ChnScanStatus_eUnlock=0, 
      BADS_3x7x_ChnScanStatus_eLock=1
}BADS_3x7x_ChnScanStatus_UnlockLock_t;

typedef enum BADS_3x7x_ChnScanStatus_AcqType_s
{
	BADS_3x7x_ChnScanStatus_AcqType_eAuto=0,
	BADS_3x7x_ChnScanStatus_AcqType_eFast=1,
	BADS_3x7x_ChnScanStatus_AcqType_eSlow=2,
	BADS_3x7x_ChnScanStatus_AcqType_eScan=3
}BADS_3x7x_ChnScanStatus_AcqType_t;

typedef struct BADS_3x7x_ChnScanStatus_s
{
      BADS_3x7x_ChnScanStatus_AcqStatus_t   AcqStatus_te;
      BADS_3x7x_ChnScanStatus_Spectrum_t    Spectrum_te;
      BADS_3x7x_ChnScanStatus_BPS_t         BPS_te;
      BADS_3x7x_ChnScanStatus_Annex_t       AB_te;
      BADS_3x7x_ChnScanStatus_Interleaver_t Interleaver_te;
      int32_t                               RF_Offset_i32;
      uint32_t                              Symbol_Rate_u32;

      /* Extra scan status param info */
      BADS_3x7x_ChnScanStatus_OffOn_t	    Auto_te;
      BADS_3x7x_ChnScanStatus_UnlockLock_t	FLK_te;  
      BADS_3x7x_ChnScanStatus_UnlockLock_t	QLK_te;  
      BADS_3x7x_ChnScanStatus_AcqType_t	    AcqType_te;

}BADS_3x7x_ChnScanStatus_t;
 

#ifdef __cplusplus
}
#endif

#endif