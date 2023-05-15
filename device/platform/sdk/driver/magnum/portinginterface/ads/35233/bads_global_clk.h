/******************************************************************************
 *    (c)2010-2011 Broadcom Corporation
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
 * $brcm_Workfile: bads_global_clk.h $
 * $brcm_Revision: Hydra_Software_Devel/6 $
 * $brcm_Date: 7/19/11 8:00p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /magnum/portinginterface/ads/7552/bads_global_clk.h $
 * 
 * Hydra_Software_Devel/6   7/19/11 8:00p farshidf
 * SW7552-60: add new flag
 * 
 * Hydra_Software_Devel/5   6/1/11 11:43a farshidf
 * SW7552-36: clean up the code
 * 
 * Hydra_Software_Devel/4   5/27/11 10:50a farshidf
 * SW7552-36: compiel fix
 * 
 * Hydra_Software_Devel/3   5/10/11 11:11p farshidf
 * SWDTV-7035: merge main
 * 
 * Hydra_Software_Devel/SWDTV-7035/2   5/10/11 6:30p cbrooks
 * SWDTV-7035:Fixed MIPS timer
 * 
 * Hydra_Software_Devel/SWDTV-7035/1   5/10/11 11:53a farshidf
 * SWDTV-7035: new DS code from charlie
 * 
 * Hydra_Software_Devel/2   4/27/11 12:32a farshidf
 * SWDTV-6190: code cleanup
 * 
 * Hydra_Software_Devel/1   4/20/11 12:38p farshidf
 * SWDTV-6190: add intial DS files
 * 
 *****************************************************************************/

#ifndef _BADS_GLOBAL_CLK_H__
#define _BADS_GLOBAL_CLK_H__

#ifdef __cplusplus
extern "C" {
#endif

#define GAIN_OFFSET -3000

/*The following offsets need to be set board to board, this set was done with the BCM93128V10_00 without triplexer*/
/*Gain offsets for board to get channel power, values are offsets in db*256*/
#define GAIN_OFFSET_BELOW_150MHZ   12*256  /*Calibrate at 100 MHz and 0 dbmv*/
#define GAIN_OFFSET_150MHZ_250MHZ  13*256  /*Calibrate at 200 MHz and 0 dbmv*/
#define GAIN_OFFSET_250MHZ_350MHZ  15*256  /*Calibrate at 300 MHz and 0 dbmv*/
#define GAIN_OFFSET_350MHZ_450MHZ  15*256  /*Calibrate at 400 MHz and 0 dbmv*/
#define GAIN_OFFSET_450MHZ_550MHZ  14*256  /*Calibrate at 500 MHz and 0 dbmv*/
#define GAIN_OFFSET_550MHZ_650MHZ  14*256  /*Calibrate at 600 MHz and 0 dbmv*/
#define GAIN_OFFSET_650MHZ_750MHZ  14*256  /*Calibrate at 700 MHz and 0 dbmv*/
#define GAIN_OFFSET_750MHZ_850MHZ  13*256  /*Calibrate at 800 MHz and 0 dbmv*/
#define GAIN_OFFSET_ABOVE_850MHZ   12*256  /*Calibrate at 900 MHz and 0 dbmv*/

/*This was 27000000 API for the 330 needs to be in us*/
/*#define MIPS_TIMER 27000000*/
#define MIPS_TIMER 1000000

#define F_HS		         15000000UL /*14625000UL*/
#define F_1S                 30000000UL /*29250000UL*/


#define SPUR_TBL_SIZE 1
static const uint32_t SPUR_TBL[SPUR_TBL_SIZE] =
{
0
};


#ifdef __cplusplus
}
#endif

#endif