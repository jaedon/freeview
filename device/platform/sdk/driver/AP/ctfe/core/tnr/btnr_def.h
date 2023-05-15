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
 * $brcm_Workfile: btnr_def.h $
 * $brcm_Revision: 
 * $brcm_Date: 8/29/12 11:54a $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AP/ctfe/core/tnr/btnr_def.h $
 * 
 * 6   8/29/12 11:54a farshidf
 * SW7552-336: merge to main
 * 
 * Fw_Integration_Devel/6   8/29/12 11:53a farshidf
 * SW7552-336: merge to main
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/5   8/21/12 5:13p mpovich
 * SW3472-6: Merge to 5_0 Dev branch.
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/4   8/15/12 6:47p farshidf
 * SW7563-14: remove external LNA calls
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/SW3472-6/1   7/10/12 5:10p mpovich
 * SW3472-6: Channelize writes to the TM_SYS_PLL tuner registers (inside
 *  smart tune).
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/3   6/28/12 2:45p mpovich
 * SW3472-6: Fix 3461's TNR core version ID assignment made for 3472
 *  change.
 * 
 * 4   6/26/12 6:37p mpovich
 * SW3472-3: Merge initial 3472 a0 baseline.
 * 
 * Fw_Integration_Devel/4   6/26/12 5:42p mpovich
 * SW3472-3: Merge to 3472 a0 initial baseline to integ. branch.
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/2   6/26/12 1:12p mpovich
 * SW3472-3: Initial 3472 a0 baseline.  Add initial support for
 *  initializing and powering dual core TNR channels.  Fix initialization
 *  and assignment of multiple channel handles.
 * 
 * 3   5/24/12 11:14a farshidf
 * SW3461-1: merge to main
 * 
 * Fw_Integration_Devel/3   5/24/12 11:13a farshidf
 * SW3461-1: merge to main
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/1   5/21/12 6:20p farshidf
 * SW3461-213: Make the Tuner Branch 5.0 to compile for 7552
 * 
 * Fw_Integration_Devel/2   3/23/12 2:50p farshidf
 * SW3128-125: FW version 4.6
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/5   3/13/12 4:21p farshidf
 * SW3461-1: make smart tune a compile option
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/4   3/13/12 4:13p farshidf
 * SW3461-1: smart tune only for 3461/3462
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/3   3/13/12 3:57p farshidf
 * SW3461-1: clean up
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/2   3/13/12 3:53p farshidf
 * SW3461-1: clean up btnr_def.h
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/1   3/12/12 6:26p farshidf
 * SW3462-6: merge to Dev
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/SW3462-6/3   2/29/12 5:56p farshidf
 * SW3461-165: fix warning
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/SW3462-6/2   2/29/12 5:26p farshidf
 * SW3461-165 : Chip version clean up & fix tuner for 3462
 * 
 * Fw_Integration_Devel/AP_V4_0_TNR_DEV/SW3462-6/1   2/28/12 6:04p mpovich
 * SW3462-6: Rebase with SW3462-3 dev. branch.
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/SW3462-3/1   2/28/12 10:08a jputnam
 * SW3462-5: Temporary change to force use of TNR_CORE_V_1_1 for 3462A0
 *  until proper version methodology is adopted in the tuner driver
 * 
 * Fw_Integration_Devel/1   10/13/11 6:58p farshidf
 * SW3461-64: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/1   9/29/11 11:38a farshidf
 * SW3461-1: add the btnr_def.h

 ***************************************************************************/
#ifndef BTNR_DEF_H__
#define BTNR_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BTNR_P_BCHP_CORE_V(MAJOR,MINOR) (((MAJOR)<<8)+MINOR)

#define BTNR_P_BCHP_CORE_V_1_0 (BTNR_P_BCHP_CORE_V(1,0))
#define BTNR_P_BCHP_CORE_V_1_1 (BTNR_P_BCHP_CORE_V(1,1))
#define BTNR_P_BCHP_CORE_V_1_2 (BTNR_P_BCHP_CORE_V(1,2))
#define BTNR_P_BCHP_CORE_V_1_3 (BTNR_P_BCHP_CORE_V(1,3))
#define BTNR_P_BCHP_CORE_V_1_4 (BTNR_P_BCHP_CORE_V(1,4))


#define EXT_LNA_ENABLE 0

/***************************************************************************
 *  BTNR CORE Defines
 ****************************************************************************/
#if (BCHP_VER == BCHP_VER_A0)
	#if ((BCHP_CHIP==7552) || (BCHP_CHIP==35233) || (BCHP_CHIP==3461))
  	  #define BTNR_P_BCHP_TNR_CORE_VER     BTNR_P_BCHP_CORE_V_1_0
      #define BTNR_P_BCHP_TNR_NUM_CORES   (1)     
      #define BTNR_P_BCHP_TNR_MULTI_CORE_OFFSET (0x0)
      #define BTNR_P_BCHP_TNR_MULTI_CORE_TM_SYS_PLL_OFFSET (0x0)
	#elif (BCHP_CHIP==3462)
	  #define BTNR_P_BCHP_TNR_CORE_VER     BTNR_P_BCHP_CORE_V_1_2
      #define BTNR_P_BCHP_TNR_NUM_CORES   (1)     
      #define BTNR_P_BCHP_TNR_MULTI_CORE_OFFSET (0x0)
      #define BTNR_P_BCHP_TNR_MULTI_CORE_TM_SYS_PLL_OFFSET (0x0)
	#elif (BCHP_CHIP==3472)
	  #define BTNR_P_BCHP_TNR_CORE_VER     BTNR_P_BCHP_CORE_V_1_3
      #define BTNR_P_BCHP_TNR_NUM_CORES   (2)     
      #define BTNR_P_BCHP_TNR_MULTI_CORE_OFFSET (0x20000)
      #define BTNR_P_BCHP_TNR_MULTI_CORE_TM_SYS_PLL_OFFSET (0x4)
	#elif (BCHP_CHIP==7563)
	  #define BTNR_P_BCHP_TNR_CORE_VER     BTNR_P_BCHP_CORE_V_1_4
      #define BTNR_P_BCHP_TNR_NUM_CORES   (1)     
      #define BTNR_P_BCHP_TNR_MULTI_CORE_OFFSET (0x0)
      #define BTNR_P_BCHP_TNR_MULTI_CORE_TM_SYS_PLL_OFFSET (0x0)
	#endif
#elif (BCHP_VER == BCHP_VER_B0)
	#if ((BCHP_CHIP==7552) || (BCHP_CHIP==3461))
	  #define BTNR_P_BCHP_TNR_CORE_VER     BTNR_P_BCHP_CORE_V_1_1
      #define BTNR_P_BCHP_TNR_NUM_CORES   (1)     
      #define BTNR_P_BCHP_TNR_MULTI_CORE_OFFSET (0x0)
      #define BTNR_P_BCHP_TNR_MULTI_CORE_TM_SYS_PLL_OFFSET (0x0)
	#endif
#endif


#ifdef SMART_TUNE_ENABLED
	#define SmartTuneEnabled
#endif


#ifndef BTNR_P_BCHP_TNR_CORE_VER
  #error TNR core NOT DEFINED in TNR PI
#endif
 
#if (BCHP_CHIP==7552)   /* Requires for any PI with single Channel support */
#define BTNR_3x7x_ChnHandle BTNR_3x7x_Handle
#define BTNR_P_3x7x_ChnHandle BTNR_P_3x7x_Handle
#endif

#ifdef __cplusplus
}
#endif

#endif