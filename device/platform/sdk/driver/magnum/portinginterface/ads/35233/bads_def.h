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
 * $brcm_Workfile: bads_def.h $
 * $brcm_Revision: 45 $
 * $brcm_Date: 10/2/12 7:00p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AP/ctfe/core/ads/bads_def.h $
 * 
 * 45   10/2/12 7:00p farshidf
 * SW3128-231: FW version 5.7.0.0
 * 
 * Fw_Integration_Devel/20   10/2/12 6:32p farshidf
 * SW3128-231: FW version 5.7.0.0
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/7   9/27/12 4:20p cbrooks
 * sw3128-227:Added Edefine in defs.h option for equalizer size, default
 *  is 36/36 w/18 overlap, option i n 18/24 w/6 overlap
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/6   8/17/12 4:23p cbrooks
 * sw3128-1:Code Optimization/Clean up
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/5   8/6/12 6:34p cbrooks
 * sw3128-1:Code Cleanup
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/4   7/26/12 11:32a farshidf
 * SW7584-41: initial clean up
 * 
 * 42   7/17/12 5:45p farshidf
 * SW3128-183: fix CableLabs OpenCable ATP 4.1.11 Combined Distortion Test
 *  (Burst Noise)
 * 
 * Fw_Integration_Devel/17   7/17/12 5:44p farshidf
 * SW3128-183: fix CableLabs OpenCable ATP 4.1.11 Combined Distortion Test
 *  (Burst Noise)
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/3   6/15/12 4:45p cbrooks
 * sw3128-1:readded 1-7 MBaud
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/2   6/12/12 7:00p cbrooks
 * sw3128-1:Lower Baud Rate is 3.0 MBaud
 * 
 * Fw_Integration_Devel/AP_V5_0_ADS_DEV/1   4/27/12 5:22p farshidf
 * SW3462-19: merge to 5.0 branch
 * 
 * Fw_Integration_Devel/AP_V4_0_ADS_DEV/SW3462-6/1   2/28/12 6:02p mpovich
 * SW3462-6: Rebase with SW3462-3 dev. branch.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/SW3462-3/1   2/27/12 5:03p mpovich
 * SW3462-3: 3462 initial baseline development and initial HAB support.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/SW3462-2/2   1/30/12 5:46p mpovich
 * SW3462-2: Support for 3462.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/SW3462-2/1   1/12/12 5:59p mpovich
 * SW3462-2: Support for 3462.
 * 
 * Fw_Integration_Devel/AP_V4_0_ADS_DEV/3   2/22/12 2:08p farshidf
 * SW7552-217: Enable Burst mode for DS only for china based on compile
 *  flag
 * 
 * Fw_Integration_Devel/AP_V4_0_ADS_DEV/2   2/21/12 12:03p cbrooks
 * sw3128-1:disabled Burst Mode for Annex A
 * 
 * Fw_Integration_Devel/AP_V4_0_ADS_DEV/1   2/20/12 5:40p cbrooks
 * sw3128-1:Added Burst Noise Enable for Annex A China
 * 
 * Fw_Integration_Devel/13   12/15/11 12:23p farshidf
 * SW3461-118: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/7   12/12/11 4:14p cbrooks
 * sw3128-1:added new coeffs for FOI timing loop
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/6   11/21/11 6:55p mpovich
 * SW3128-71: Support for a single, common 3128 chip family F/W binary.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/SW3128-71/1   11/17/11 6:58p mpovich
 * SW3128-71: Support for common 3128 family chip F/W.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/5   11/17/11 6:27p mpovich
 * SW3128-1: Revert INIT_BBS_UNUSED_FLAG5 to ,
 *  BADS_Internal_Params_eDisable.
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/4   10/31/11 10:50a cbrooks
 * sw3128-1:added FOI timing loop
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/3   10/31/11 9:39a thayashi
 * Changed CWC PLL BW
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/2   10/24/11 2:29p cbrooks
 * sw3128-1:made Taks -1.75 MHz AnnexB CWC only for chips with WFE
 * 
 * Fw_Integration_Devel/AP_V3_0_ADS_DEV/1   10/21/11 6:16p cbrooks
 * sw3128-1:added Taks -1.75 MHz CWC spur cancellation for AnnexB
 * 
 * Fw_Integration_Devel/8   10/12/11 10:40a farshidf
 * SW3128-54: merge to Integ branch
 * 
 * Fw_Integration_Devel/AP_V2_0_ADS_DEV/1   10/6/11 11:45a farshidf
 * SW3128-1: add support for 3128 C0
 * 
 * 31   8/17/11 8:00p farshidf
 * SW3461-1: enable INIT_BBS_UNUSED_FLAG6
 * 
 * 30   8/12/11 3:13p farshidf
 * SW3461-1: merge to main
 * 
 * Fw_Integration_Devel/6   8/12/11 2:10p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/16   7/29/11 11:25a cbrooks
 * sw3128-1:increased stuck fec timeout
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/15   7/28/11 6:44p cbrooks
 * sw3128-1:Added auto reacquision count attemps for each mode
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/14   7/28/11 4:40p cbrooks
 * sw3128-1:minor revisions for clarity
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/13   7/27/11 7:58p cbrooks
 * sw3128-1:disabled acquisition test
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/12   7/25/11 5:49p farshidf
 * SW3128-1: correction chip ID
 * 
 * Fw_Integration_Devel/5   7/25/11 5:48p farshidf
 * SW3128-1: correction chip ID
 * 
 * 29   7/26/11 6:24p farshidf
 * SW3128-1: remove 7552 callback
 * 
 * 28   7/26/11 2:23p farshidf
 * SWDTV-7869: enable the FFT IRQ for 35233
 * 
 * 27   7/25/11 5:48p farshidf
 * SW3128-1: correction of chip ID
 * 
 * 26   7/25/11 10:34a farshidf
 * SW3128-1: merge to main
 * 
 * Fw_Integration_Devel/4   7/25/11 10:19a farshidf
 * SW3128-1: merge to integ
 * 
 * Fw_Integration_Devel/3   7/20/11 9:49a farshidf
 * SW3128-1: B0 support
 * 
 * Fw_Integration_Devel/2   7/8/11 11:34a farshidf
 * SW3128-11: merge to integ
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/11   7/22/11 4:51p cbrooks
 * sw3128-1:Added SLow Scan for RFI interference
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/10   7/18/11 6:37p cbrooks
 * sw3128-1:temporarily make slow mode same as fast mode
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/9   7/18/11 6:09p cbrooks
 * sw3128-1:Added Baseband timing loop
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/8   7/18/11 9:59a farshidf
 * SW3128-28: compile fix
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/7   7/15/11 6:27p farshidf
 * SWDTV-7869: fix compile error
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/6   7/15/11 6:23p farshidf
 * SWDTV-7869: add the new flag for B0 chip
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/5   7/15/11 5:40p cbrooks
 * sw3128-1:added slow acquire for RFI support
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/4   7/14/11 5:25p cbrooks
 * sw3128-1:enabled callback in 3461
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/3   7/1/11 1:10p cbrooks
 * sw3128-1:disabled callback in 3461
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/2   7/1/11 12:54p cbrooks
 * sw3128-1:added callback for 3461
 * 
 * Fw_Integration_Devel/AP_V0_6_ADS_DEV/1   6/30/11 6:01p cbrooks
 * sw3128-1:added retry capability
 * 
 * Fw_Integration_Devel/1   6/29/11 12:38p farshidf
 * SW3461-13: merge to integration branch
 * 
 * Fw_Integration_Devel/Ads_Fw_Devel_Rc04/2   6/22/11 6:46p cbrooks
 * sw3128-1:disabled callback for 3461
 * 
 * Fw_Integration_Devel/Ads_Fw_Devel_Rc04/1   6/22/11 5:32p cbrooks
 * sw3128-1:callback spport for 3461
 * 
 * 21   6/9/11 6:15p mpovich
 * SW3461-1: Merge Ver 0.4 Integ. onto main branch.
 * 
 * SW_System_4_Integ_Test/4   6/9/11 2:16p mpovich
 * SW3461-1: Rebase with main branch.
 * 
 * 20   6/7/11 6:02p farshidf
 * SW3128-1: add 3123 support
 * 
 * 19   6/7/11 3:15p farshidf
 * SW3128-1: merge to main
 * 
 * SW_System_4_Integ_Test/3   6/7/11 1:50p farshidf
 * SW3128-1: sync up with backend
 * 
 * SW_System_4_Integ_Test/2   6/7/11 11:05a farshidf
 * SW3128-1: merge to integration branch
 * 
 * Ads_Fw_Devel_3/5   6/7/11 10:56a cbrooks
 * sw3128-1:added3461/ 7552 CWC
 * 
 * Ads_Fw_Devel_3/4   6/6/11 7:50p cbrooks
 * sw3128-1:Added 3461 CWC spur
 * 
 * Ads_Fw_Devel_3/3   6/1/11 12:08p cbrooks
 * sw3128-1:new defs
 * 
 * Ads_Fw_Devel_3/1   5/27/11 12:28p cbrooks
 * sw3128-1:Added IMC and CWC
 * 
 * 18   5/18/11 3:43p farshidf
 * SW3128-1: compile fix
 * 
 * 17   5/18/11 3:42p farshidf
 * SW3128-1: disable callback for 3461
 * 
 * 16   5/10/11 3:13p farshidf
 * SW3128-1: merge main
 * 
 * ADS_3128_3/6   5/7/11 3:53p cbrooks
 * sw3128-1:clean up
 * 
 * ADS_3128_3/5   5/7/11 3:47p cbrooks
 * sw3128-1:corrected freq offset
 * 
 * ADS_3128_3/4   5/5/11 8:14p cbrooks
 * sw3128-1:Cleanup Code
 * 
 * ADS_3128_3/3   5/1/11 3:45p cbrooks
 * sw3128-1:New Code
 * 
 * ADS_3128_3/2   5/1/11 3:30p cbrooks
 * sw3128-1:Cleaned up Channel Scan Code
 * 
 * ADS_3128_3/1   4/28/11 1:08p cbrooks
 * sw3128-1:New Code for scan
 * 
 * 14   4/26/11 6:50p farshidf
 * SW3128-1: merge main
 * 
 * ADS_3128_2/5   4/26/11 5:09p farshidf
 * SW3128-1: merge main
 * 
 * 13   4/15/11 5:16p farshidf
 * SW3128-1: update from charlie
 * 
 * 12   4/15/11 4:39p farshidf
 * SW3128-1: merge main
 * 
 * ADS_3128_2/3   4/13/11 5:04p cbrooks
 * sw3128-1:new cwc code
 * 
 * ADS_3128_2/2   4/11/11 8:31p cbrooks
 * SW3128-1:Added CWC code
 * 
 * ADS_3128_2/1   4/11/11 12:46p cbrooks
 * sw3128-1:New CWC code
 * 
 * 11   3/24/11 4:15p farshidf
 * SW3128-1: add support for 3124
 * 
 * 10   3/3/11 11:01a farshidf
 * SW3128-1: clean up
 * 
 * 8   2/28/11 5:57p cbrooks
 * sw3128-1:Changed acqwords to internal_params
 * 
 * 7   2/9/11 10:50a cbrooks
 * SW3128-1:LIC EST CODE
 * 
 * 6   1/31/11 7:50p cbrooks
 * sw3128-1:new code
 * 
 * 5   1/30/11 6:35p cbrooks
 * sw3128-1:FFT Code
 * 
 * 4   1/28/11 4:00p farshidf
 * SW3461-1: update
 * 
 * 3   1/28/11 3:45p farshidf
 * SW3128-1: adapt the files to 3461
 * 
 * 2   1/26/11 4:13p farshidf
 * SW3128-1: clean up
 *
 ***************************************************************************/
#ifndef BADS_DEF_H__
#define BADS_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 *  BADS CORE Versions
 ****************************************************************************/
#define BADS_P_BCHP_DS_CORE_V(MAJOR,MINOR) ((MAJOR*10)+MINOR)

#define BADS_P_BCHP_DS_CORE_V_9_1 (BADS_P_BCHP_DS_CORE_V(9,1))
#define BADS_P_BCHP_DS_CORE_V_9_2 (BADS_P_BCHP_DS_CORE_V(9,2))
#define BADS_P_BCHP_DS_CORE_V_9_3 (BADS_P_BCHP_DS_CORE_V(9,3))
#define BADS_P_BCHP_DS_CORE_V_9_4 (BADS_P_BCHP_DS_CORE_V(9,4))
#define BADS_P_BCHP_DS_CORE_V_9_5 (BADS_P_BCHP_DS_CORE_V(9,5))
#define BADS_P_BCHP_DS_CORE_V_9_6 (BADS_P_BCHP_DS_CORE_V(9,6))

/***************************************************************************
 *  BADS CORE Defines
 ****************************************************************************/
#if (BCHP_VER == BCHP_VER_A0)
  #if ((BCHP_FAMILY==3462))
    #define BADS_P_BCHP_DS_CORE_VER (BADS_P_BCHP_DS_CORE_V_9_5)
  #elif ((BCHP_FAMILY==7584))
    #define BADS_P_BCHP_DS_CORE_VER (BADS_P_BCHP_DS_CORE_V_9_6)
  #endif
#elif (BCHP_VER == BCHP_VER_B0)
  #if ((BCHP_CHIP==7552) || (BCHP_FAMILY==3461) || (BCHP_FAMILY==3462))
    #define BADS_P_BCHP_DS_CORE_VER (BADS_P_BCHP_DS_CORE_V_9_2)
  #endif
#elif (BCHP_VER == BCHP_VER_C0)
  #if ((BCHP_FAMILY==3128))
    #define BADS_P_BCHP_DS_CORE_VER (BADS_P_BCHP_DS_CORE_V_9_3)
  #endif
#endif

#ifndef BADS_P_BCHP_DS_CORE_VER
  #error DS core NOT DEFINED in ADS PI
#endif

/***************************************************************************
 *  BADS define statements
 ****************************************************************************/
#define PRINT_DEBUG 0				  /*Print debug messages to the UART*/
#define ADS_INTERNAL_ERROR_CHECKING 3 /*0 no error checking, 1 check divide by 0, 2 ADD check ranges, 3 ADD check results*/

#define TIMING_SCAN_FFT_TIMEOUT 7    /*Timing Scan FFT Interrupt timeout in mS*/
#define CARRIER_SCAN_FFT_TIMEOUT 11  /*Carrier Scan FFT Interrupt timeout in mS*/
#define NUM_TIMING_FFTS 3			 /*number of timing FFT's to perform 1,2, or 3*/
#define NUM_CARRIER_FFTS 3			 /*number of carrier FFT's to perform 1,2, or 3*/

#define PRE_NYQUIST_FILTER_BW_1MHZ 60 /*Bandwidth for stepping carrier loop during baud/carrier search: This is in KHz and is referenced to 1 MBaud and must be >= 8*/
#define MAX_PHASE_ERROR 5000		  /*maximum phase loop offset to declare carrier lock*/
#define SNR_LEAKY_AVG 512		      /*leaky averager rate for average SNR computation*/
	 
/***************************************************************************************************************
*Acquisition parameters
 ***************************************************************************************************************/
/*Define number of times to retry if not locked and we are in Auto Acquire mode is enabled for different AcqTypes*/
#define NUM_RETRIES_IF_AUTOACQUIRE_AND_AUTO 2
#define NUM_RETRIES_IF_AUTOACQUIRE_AND_FAST 0
#define NUM_RETRIES_IF_AUTOACQUIRE_AND_SLOW 0
#define NUM_RETRIES_IF_AUTOACQUIRE_AND_SCAN 0

/*Number of fast acquires before transitioning to slow acquire if Auto Acquire mode is enabled and if acquisition type is AUTO*/
/*Number of slow acquires before transitioning to slow scan acquire if Auto Acquire mode is enabled and if acquisition type is AUTO*/
/*Slow scan acquire is the union of the acquire params and the scan params*/
#define NUM_FAST_ACQUIRES 1     
#define NUM_SLOW_ACQUIRES 1

/*Number of ms for AGCB to converge, this is baud based but we don't know the baud rate*/
#define AGCB_CONVERGENCE_TIME_MS 3       

 /*Number of baud samples for TL to converge, 5000 = 1 ms for Baud rate of 5 MBAUD*/
#define TL_TIME_ACQ_BAUD_SAMPLES  10000   
#define TL_TIME_TRK_BAUD_SAMPLES   5000  

/*Number of baud samples for different EQ modes to converge, 5000 = 1 ms for Baud rate of 5 MBAUD*/
#define CMA_TIME_BLIND1_BAUD_SAMPLES 30000 
#define CMA_TIME_BLIND2_BAUD_SAMPLES 20000  
#define CMA_TIME_LOCKED1_BAUD_SAMPLES 30000  
#define CMA_TIME_LOCKED2_BAUD_SAMPLES 20000  
#define CMA_TIME_FAST_TRIM_BAUD_SAMPLES 10000   
#define LMS_TIME_FAST_TRIM_BAUD_SAMPLES  10000  
#define CMA_TIME_SLOW_TRIM1_BAUD_SAMPLES 768000    
#define LMS_TIME_SLOW_TRIM1_BAUD_SAMPLES 256000    
#define CMA_TIME_SLOW_TRIM2_BAUD_SAMPLES 48000    
#define LMS_TIME_SLOW_TRIM2_BAUD_SAMPLES 16000  

/*Number of samples to wait when switching in the CPL integrator leak and phase accummulator leak*/
/*FEC sync timeout in ms for AnnexA and Annex B*/
#define ANNEXA_FEC_LOCK_TIMEOUT	20    
#define ANNEXB_FEC_LOCK_TIMEOUT 40    

/*****************************************************************************************************************
*Lock Detector Values
*The PI polls the BADS_P_Get_LockStatus function
*Lock is declared IF the number of clean blocks detected in since the previous call is >= NUM_CLEAN_BLOCKS_TO_LOCK 
*if this condition is not met then 
*Lock is declared if there have NOT been more then NUM_BAD_BLOCK_TO_UNLOCK bad blocks have accumulated 
*if this condition is not met then 
*Unlock is declared if there have been more then NUM_BAD_BLOCK_TO_UNLOCK bad blocks have accumulated
*****************************************************************************************************************/
#define NUM_CLEAN_BLOCKS_TO_LOCK 1
#define NUM_BAD_BLOCK_TO_UNLOCK 1000
#define STUCK_FEC_RESET_COUNT 10

/*Acquire Parameter Ranges*/
#define MAX_CARRIER_RANGE 1000000
#define MIN_BAUD_RATE 1000000
#define MAX_BAUD_RATE 7300000

/*Scan Parameter Ranges*/
#define MAX_CARRIER_SCAN 1000000
#define MIN_BAUD_SCAN 1000000
#define MAX_BAUD_SCAN 7300000

/*AnnexB Baud Ranges*/
#define Q64_ANNEXB_SYMBOL_RATE 5056941
#define MAX_Q64_ANNEXB_SYMBOL_RATE 5081941
#define MIN_Q64_ANNEXB_SYMBOL_RATE 5031941
#define Q256_Q1024_ANNEXB_SYMBOL_RATE 5360537
#define MAX_Q256_Q1024_ANNEXB_SYMBOL_RATE 5385537
#define MIN_Q256_Q1024_ANNEXB_SYMBOL_RATE 5335537

/*What CWC parameters to use if INIT_BBS_CWC==BADS_Internal_Params_eEnable*/
/*the CWC will use parameters from the UFE or the WFE to know what RF frequency the toned are at*/
/*See global_clk.h files and the BADS_P_Set_CWC_Auto() funtion for more information*/
#define CWC_MODE1	2	/*0 disable, 1 for Non-tracking mode (CWC_LFC1 - CWC_LFC4 will be set to 0), 2 for AFC mode, 3 for PLL mode*/
#define CWC_MODE2	3		        
#define CWC_MODE3	3		
#define CWC_MODE4	3
#define CWC_AFC_ACQ_BW 0x12800320
#define CWC_AFC_TRK_BW 0x03200160 
#define CWC_AFC_MU 2
#define CWC_AFC_LEAK 0
#define CWC_PLL_ACQ_BW 0x00040064
#define CWC_PLL_TRK_BW 0x00040064
#define CWC_PLL_MU 2
#define CWC_PLL_LEAK 0

/******************************************************************************************
*THE CWC HAS SOME STRANGE PROGRAMMING DO NOT CHANGE THE DEFINITIONS BELOW
*The chip has a bitwise definition for the AFC/PLL mode but no bitwise RDB representation
*******************************************************************************************/
#define CWC_ACQ_LFC1 ((CWC_MODE1 == 3) ? CWC_PLL_ACQ_BW : CWC_AFC_ACQ_BW) 
#define CWC_ACQ_LFC2 ((CWC_MODE2 == 3) ? CWC_PLL_ACQ_BW : CWC_AFC_ACQ_BW)
#define CWC_ACQ_LFC3 ((CWC_MODE3 == 3) ? CWC_PLL_ACQ_BW : CWC_AFC_ACQ_BW) 
#define CWC_ACQ_LFC4 ((CWC_MODE4 == 3) ? CWC_PLL_ACQ_BW : CWC_AFC_ACQ_BW) 
#define CWC_TRK_LFC1 ((CWC_MODE1 == 3) ? CWC_PLL_TRK_BW : CWC_AFC_TRK_BW) 
#define CWC_TRK_LFC2 ((CWC_MODE2 == 3) ? CWC_PLL_TRK_BW : CWC_AFC_TRK_BW)
#define CWC_TRK_LFC3 ((CWC_MODE3 == 3) ? CWC_PLL_TRK_BW : CWC_AFC_TRK_BW) 
#define CWC_TRK_LFC4 ((CWC_MODE4 == 3) ? CWC_PLL_TRK_BW : CWC_AFC_TRK_BW) 
#define CWC_MU1 ((CWC_MODE1 == 3) ? CWC_PLL_MU : CWC_AFC_MU) 
#define CWC_MU2 ((CWC_MODE2 == 3) ? CWC_PLL_MU : CWC_AFC_MU)
#define CWC_MU3 ((CWC_MODE3 == 3) ? CWC_PLL_MU : CWC_AFC_MU) 
#define CWC_MU4 ((CWC_MODE4 == 3) ? CWC_PLL_MU : CWC_AFC_MU)
#define CWC_LK1 ((CWC_MODE1 == 3) ? CWC_PLL_LEAK : CWC_AFC_LEAK) 
#define CWC_LK2 ((CWC_MODE2 == 3) ? CWC_PLL_LEAK : CWC_AFC_LEAK)
#define CWC_LK3 ((CWC_MODE3 == 3) ? CWC_PLL_LEAK : CWC_AFC_LEAK) 
#define CWC_LK4 ((CWC_MODE4 == 3) ? CWC_PLL_LEAK : CWC_AFC_LEAK)
#define CWC_PLL ((CWC_MODE1 == 3) ? 1 : 0) | ((CWC_MODE2 == 3) ? 1 : 0)<<1 | ((CWC_MODE3 == 3) ? 1 : 0)<<2 | ((CWC_MODE4 == 3) ? 1 : 0)<<3

/**************************************************************************************
*Initial Values for the BADS_Internal_Params structure
***************************************************************************************/
#define INIT_BIG_EQ					 BADS_Internal_Params_eEnable      /*Enable/Disable Big Equalizer: Big EQ is 36/36 taps w/18 overlap taps, Big EQ is 18/24 taps w/6 overlap taps */
#define INIT_CWC					 BADS_Internal_Params_eEnable      /*Enable/Disable CWC cancellers*/
#define INIT_CFL                     BADS_Internal_Params_eEnable      /*Enable/Disable front carrier frequency loop*/
#define INIT_DDAGC                   BADS_Internal_Params_eEnable      /*Enable/Disable DD_AGC*/
#define INIT_IMC                     BADS_Internal_Params_eDisable     /*Enable/Disable IMC circuit if chip has an IMC*/
#define INIT_ACQUISITION_TEST        BADS_Internal_Params_eDisable     /*Enable/Disable internal acquisition percentage test with SPARE register*/

#if (BCHP_FAMILY==3128) || (BCHP_FAMILY==7584)
	#define INIT_VIDEO_CANCELLATION_ANNEX_B            BADS_Internal_Params_eEnable  /*Enable/Disable CWC at RF Freq - 1.75 MHz, the Tak special for WFE*/
#else
	#define INIT_VIDEO_CANCELLATION_ANNEX_B            BADS_Internal_Params_eDisable  
#endif

#define INIT_DUAL_SCAN            BADS_Internal_Params_eEnable /*Enable/Disable Dual Scan, Disable mean one fast scan, Enable for one fast scan then one slow scan*/
#define INIT_FOI_TIMING           BADS_Internal_Params_eEnable /*Enable/Disable FOI Timing Detector: Disable mean transition tracker, Enable for FOI*/
#define INIT_CALLBACK_ENABLE      BADS_Internal_Params_eEnable /*Enable/Disable front end callback function*/

#define INIT_TIMING_SCAN_THRESHOLD   0x3000			           /*FFT Timing Scan Threshold: only 17 bits in chip*/
#define INIT_CARRIER_SCAN_THRESHOLD  0x3000                    /*FFT Carrier Scan Threshold: only 17 bits in chip*/

#ifdef __cplusplus
}
#endif

#endif