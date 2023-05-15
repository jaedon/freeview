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
 * $brcm_Workfile: bthd_def.h $
 * $brcm_Revision: 5 $
 * $brcm_Date: 9/10/12 10:27p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /AP/ctfe/core/thd/bthd_def.h $
 * 
 * 5   9/10/12 10:27p mpovich
 * SW3472-6: Merge to main branch.
 * 
 * Fw_Integration_Devel/5   9/10/12 10:25p mpovich
 * SW3472-6: Merge to Fw Integ. branch.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/4   8/21/12 5:13p mpovich
 * SW3472-6: Merge to 5_0 Dev branch.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/2   7/10/12 5:44p mpovich
 * SW3472-6: Add support for channelization of TM_SYS_PLL registers.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/SW3472-6/1   7/10/12 5:40p mpovich
 * SW3472-6: Add support for channelization of TM_SYS_PLL registers.
 * 
 * 4   6/26/12 6:36p mpovich
 * SW3472-3: Merge initial 3472 a0 baseline.
 * 
 * Fw_Integration_Devel/4   6/26/12 5:42p mpovich
 * SW3472-3: Merge initial 3472 a0 baseline.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/3   6/26/12 1:35p mpovich
 * SW3472-3: Initial 3472 a0 baseline.  Define distinct IRQ IDs for use in
 *  registering THD L1 IRQs.  Initial support for powering THD channels.
 *  Commonize compiler definition used to enable ISDBT support.
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/2   6/22/12 11:14a mpovich
 * SW3472-3: Alter macro to support core revisison IDs greater than 99
 *  (decimal).  Add THD core ID for 3472 a0.
 * 
 * 3   5/24/12 11:27a farshidf
 * SW3461-1: merge to main
 * 
 * Fw_Integration_Devel/3   5/24/12 11:26a farshidf
 * SW3461-1: merge to main
 * 
 * Fw_Integration_Devel/AP_V5_0_THD_DEV/1   5/22/12 5:19p farshidf
 * SW3461-211: make Isdbt to build on 5 branch for 7552
 * 
 * Fw_Integration_Devel/2   3/23/12 2:44p farshidf
 * SW3128-125: FW version 4.6
 * 
 * Fw_Integration_Devel/AP_V4_0_THD_DEV/3   3/13/12 4:20p farshidf
 * SW3461-1: make smart tune a compile option
 * 
 * Fw_Integration_Devel/AP_V4_0_THD_DEV/1   3/13/12 3:31p farshidf
 * SW3462-6: merge to dev
 * 
 * Fw_Integration_Devel/AP_V4_0_THD_DEV/SW3462-6/2   2/29/12 5:26p farshidf
 * SW3461-165 : Chip version clean up & fix tuner for 3462
 * 
 * Fw_Integration_Devel/AP_V4_0_THD_DEV/SW3462-6/1   2/28/12 6:04p mpovich
 * SW3462-6: Rebase with SW3462-3 dev. branch.
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/SW3462-3/1   2/28/12 3:46p jputnam
 * SW3462-5: Update defines for BCM3462
 * 
 * Fw_Integration_Devel/1   10/13/11 6:58p farshidf
 * SW3461-64: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_THD_DEV/1   9/26/11 11:29a jputnam
 * SW3461-1: Initial version
 *
 ***************************************************************************/
#ifndef BTHD_DEF_H__
#define BTHD_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BTHD_P_BCHP_CORE_V(MAJOR,MINOR) (((MAJOR)<<8)+MINOR)

#define BTHD_P_BCHP_CORE_V_4_0 (BTHD_P_BCHP_CORE_V(4,0))
#define BTHD_P_BCHP_CORE_V_5_0 (BTHD_P_BCHP_CORE_V(5,0))
#define BTHD_P_BCHP_CORE_V_5_1 (BTHD_P_BCHP_CORE_V(5,1))
#define BTHD_P_BCHP_CORE_V_5_21 (BTHD_P_BCHP_CORE_V(5,0x21))

/***************************************************************************
 *  BTHD CORE Defines
 ****************************************************************************/
#if (BCHP_VER == BCHP_VER_A0)
	#if ((BCHP_CHIP==7552) || (BCHP_CHIP==35233) || (BCHP_CHIP==3461))
	  #define BTHD_P_BCHP_THD_CORE_VER     BTHD_P_BCHP_CORE_V_4_0
      #define BTHD_P_BCHP_THD_NUM_CORES   (1)
      #define BTHD_P_BCHP_THD_MULTI_CORE_OFFSET (0x0)
      #define BTHD_P_BCHP_THD_MULTI_CORE_TM_SYS_PLL_OFFSET (0x0)
	#elif (BCHP_CHIP==3462)
	  #define BTHD_P_BCHP_THD_CORE_VER     BTHD_P_BCHP_CORE_V_5_1
      #define BTHD_P_BCHP_THD_NUM_CORES   (1)     
      #define BTHD_P_BCHP_THD_MULTI_CORE_OFFSET (0x0)
      #define BTHD_P_BCHP_THD_MULTI_CORE_TM_SYS_PLL_OFFSET (0x0)
	#elif (BCHP_FAMILY == 3472)
      #define BTHD_P_BCHP_THD_CORE_VER   BTHD_P_BCHP_CORE_V_5_21
      #define BTHD_P_BCHP_THD_NUM_CORES   (2)     
      #define BTHD_P_BCHP_THD_MULTI_CORE_OFFSET (0x20000)
      #define BTHD_P_BCHP_THD_MULTI_CORE_TM_SYS_PLL_OFFSET (0x4)
	#endif
#elif (BCHP_VER == BCHP_VER_B0)
	#if ((BCHP_CHIP==7552) || (BCHP_CHIP==3461))
	  #define BTHD_P_BCHP_THD_CORE_VER     BTHD_P_BCHP_CORE_V_5_1
      #define BTHD_P_BCHP_THD_NUM_CORES   (1)     
      #define BTHD_P_BCHP_THD_MULTI_CORE_OFFSET (0x0)
      #define BTHD_P_BCHP_THD_MULTI_CORE_TM_SYS_PLL_OFFSET (0x0)
	#endif
#endif

#if ((BCHP_CHIP==7552) || (BCHP_CHIP==3461) || (BCHP_CHIP==3462))
	#define SmartNotchEnabled
#endif

#ifdef SMART_TUNE_ENABLED
	#define SmartTuneEnabled
#endif


#ifndef BTHD_P_BCHP_THD_CORE_VER
  #error THD core NOT DEFINED in THD PI
#endif

#if (BCHP_CHIP==7552)   /* Requires for any PI with single Channel support */
#define BTHD_3x7x_P_ChnHandle BTHD_3x7x_P_Handle
#define BTHD_3x7x_ChnHandle BTHD_3x7x_Handle
#endif


#ifdef __cplusplus
}
#endif

#endif