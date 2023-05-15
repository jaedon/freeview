/******************************************************************************
 *    (c)2010-2012 Broadcom Corporation
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
 * $brcm_Workfile: btnr_global_clk.h $
 * $brcm_Revision: 23 $
 * $brcm_Date: 10/11/12 4:52p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AP/ctfe/core/tnr/btnr_global_clk.h $
 * 
 * 23   10/11/12 4:52p farshidf
 * SW3461-287: Create Release 5.3.0.0
 * 
 * Fw_Integration_Devel/10   10/11/12 3:53p farshidf
 * SW3461-287: Create Release 5.3.0.0
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/2   10/5/12 3:07p mpovich
 * SW3128-87: Merge to dev branch (fix BTMR ReadTimer function to return
 *  value in microseconds).
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/SW3128-87/1   10/5/12 11:58a mpovich
 * SW3128-87: Encapsulate BTMR max timer period (MIPS_TIMER) inside BTMR
 *  module.  Modify BTMR_ReadTimer to return timer in microseconds (not in
 *  clock counts).
 * 
 * Fw_Integration_Devel/AP_V5_0_TNR_DEV/1   10/5/12 11:50a farshidf
 * SW3461-1: compile fix
 * 
 * Fw_Integration_Devel/8   7/3/12 12:23p farshidf
 * SW3461-1: clean up
 * 
 * Fw_Integration_Devel/5   11/4/11 11:38a farshidf
 * SW3461-82: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_TNR_DEV/1   10/24/11 5:02p cbrooks
 * sw3461-1:Added 0 Hz CWC
 * 
 * Fw_Integration_Devel/3   8/12/11 2:11p farshidf
 * SW3461-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/4   7/27/11 7:05p cbrooks
 * sw3461-1:removed frequency dependent gain offsets
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/3   7/27/11 4:19p cbrooks
 * sw3461-1:added 540 MHz cwc
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/2   7/14/11 5:21p cbrooks
 * sw3461-1:added callback support for gains
 * 
 * Fw_Integration_Devel/AP_V0_6_TNR_DEV/1   7/1/11 3:19p cbrooks
 * sw3461-1:added callback support
 * 
 * Fw_Integration_Devel/1   6/29/11 12:39p farshidf
 * SW3461-13: merge to integration branch
 * 
 * Fw_Integration_Devel/Tnr_Fw_Devel_Rc05/1   6/20/11 3:06p cbrooks
 * sw3461-1:Added GAIN_OFFSET for channel power
 * 
 * 14   6/12/11 11:01a farshidf
 * SW3461-1: remove warning for 7552
 * 
 * 13   6/12/11 10:57a farshidf
 * SW3461-1: remove warning for 7552
 * 
 * 12   6/9/11 6:39p mpovich
 * SW3461-1: Merge Ver 0.4 Integ. onto main branch.
 * 
 * SW_System_4_Integ_Test/1   6/9/11 12:26p mpovich
 * SW3461-1: Integrate latest from all devel. branches.  Convert end of
 *  line comments per coding standard.
 * 
 * Tnr_Fw_Devel_4/1   6/6/11 7:48p cbrooks
 * sw3461-1:Added Spur Table
 * 
 * 11   5/20/11 6:42a mpovich
 * SW3461-1: rename UFE (BUFE) module to TNR (BTNR).
 * 
 * TNR_3461_1/1   5/19/11 5:12p mpovich
 * SW3461-1: Change BUFE module prefix to BTNR
 * 
 * 10   5/3/11 1:39p mpovich
 * SW3461-1: Merge to main of intermediate label, FW_3461_A0_05022011_1100
 *  (05/02/2011 Pace build).
 * 
 * TNR_3461_1/3   4/26/11 8:40p cbrooks
 * sw3461-1:added spur table
 * 
 * 9   4/26/11 8:22p farshidf
 * SW3461-1: add spur table
 * 
 * 8   3/25/11 6:31p mpovich
 * SW3128-1: Merge latest from TNR branch to main branch.
 * 
 * TNR_3461_1/2   3/24/11 11:53a jputnam
 * SW3461-1: Change TNR clock to 540MHz for cable mode
 * 
 * 7   3/18/11 4:11p farshidf
 * SW3461-1: merge  main
 * 
 * TNR_3461_1/1   3/16/11 8:42p cbrooks
 * sw3461-1:added MIPS timer
 * 
 * 6   3/10/11 5:07p cbrooks
 * sw3461-1:new code
 * 
 * 5   3/9/11 7:21p cbrooks
 * sw3461-1:Addef TNR programming code
 * 
 * 4   3/8/11 8:25p cbrooks
 * sw3461-1:new code
 * 
 * 3   3/8/11 2:42p cbrooks
 * sw3461-1:new code
 * 
 * 2   3/7/11 9:10p cbrooks
 * sw3128-1:new code
 * 
 * 1   2/2/11 9:49p farshidf
 * SW3461-1: new file
 * 
 * 1   1/26/11 4:24p farshidf
 * SW3128-1: new file
 * 
 *****************************************************************************/

#ifndef _BTNR_GLOBAL_CLK_H__
#define _BTNR_GLOBAL_CLK_H__

#ifdef __cplusplus
extern "C" {
#endif

/*The following offsets need to be set board to board, this set was done with the BCM93461DC1_01*/
/*Gain offsets for board to get channel power, values are offsets in db*256*/
#define GAIN_OFFSET  20*256  /*Calibrate at 500 MHz and 0 dbmv*/


#define REF_FREQ 54000000

/*Choose 1 REFPLL Rate*/
/*0 for 1080000000, 1 for 1350000000*/
#define REFPLL_FREQ_SEL 0

/*Assumes REF_FREQ = 54000000 and that
 *BTNR_P_TunerInit() makes the #define true*/
#if (REFPLL_FREQ_SEL == 0)
#define REFPLL_FREQ    1080000000UL
#endif
#if (REFPLL_FREQ_SEL == 1)
#define REFPLL_FREQ    1350000000UL
#endif

/*It is assumed that the BTNR_P_TunerInit()
 * and BTNR_P_Program_CIC_HB_SAW()
 *makes the following #defines true       */
#define TERR_PHYPLL1_FREQ  2700000000UL    
#define TERR_PHYPLL2_FREQ  1350000000UL            
#define TERR_PHYPLL3_FREQ   225000000UL              
#define TERR_PHYPLL4_FREQ   100000000UL              
#define TERR_PHYPLL5_FREQ    10546875UL  					 
#define TERR_PHYPLL6_FREQ   540000000UL
#define F_TER                54000000UL  
#define CABLE_PHYPLL1_FREQ 2632500000UL 
#define CABLE_PHYPLL2_FREQ 1316250000UL            
#define CABLE_PHYPLL3_FREQ  219375000UL              
#define CABLE_PHYPLL4_FREQ   97500000UL              
#define CABLE_PHYPLL5_FREQ   10283203UL                                          
#define CABLE_PHYPLL6_FREQ  540000000UL /* 526500000UL */
#define F_HS		             15000000UL /* 14625000UL */
#define F_1S                 30000000UL /* 29250000UL */


#define REF_FREQ_D128 421875
#define DPM_FREQ_MAX 1012600000
#define DPM_FREQ_MIN 31640625

#ifdef LEAP_BASED_CODE
#define SPUR_TBL_SIZE 11 
static uint32_t SPUR_TBL_u32[SPUR_TBL_SIZE] =
{
 0, 54000000, 675000000,  99900000, 432000000,
810000000, 864000000, 486000000, 594000000,
648000000, 540000000
};
#endif
#ifdef __cplusplus
}
#endif

#endif

 
