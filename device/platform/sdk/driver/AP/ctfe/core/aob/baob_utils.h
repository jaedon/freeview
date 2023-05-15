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
 * $brcm_Workfile: baob_utils.h $
 * $brcm_Revision: 8 $
 * $brcm_Date: 2/15/12 6:51p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AP/ctfe/core/aob/baob_utils.h $
 * 
 * 8   2/15/12 6:51p farshidf
 * SW3128-1: merge to main
 * 
 * Fw_Integration_Devel/2   2/15/12 6:41p farshidf
 * SW3128-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V4_0_AOB_DEV/1   2/15/12 5:16p dorothyl
 * SW3128-1: bug fix --
 * 
 * Fw_Integration_Devel/1   2/9/12 12:20p farshidf
 * SW3128-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/2   2/8/12 4:11p dorothyl
 * SW3128-1 : bug in fec
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/1   2/8/12 2:17p dorothyl
 * SW3128-1: OOB BERT
 * 
 * 6   4/5/11 3:27p farshidf
 * SW3461-1: merge  main
 * 
 * AOB_3128_1/6   3/31/11 11:32a dorothyl
 * SW3128-1: oob status fix
 * 
 * AOB_3128_1/5   3/23/11 1:06p mpovich
 * SW3128-1: Rebase AOB from main branch
 * 
 * 4   3/22/11 5:06p mpovich
 * SW3128-1: Add latest AOB driver changes.
 * 
 * AOB_3128_1/4   3/21/11 6:26p farshidf
 * SW3461-1: update naming
 * 
 * AOB_3128_1/3   3/17/11 8:49p cbrooks
 * sw4128-1:removed Range_Check
 * 
 * AOB_3128_1/2   3/17/11 8:39p cbrooks
 * sw3128-1:Worked on AOB status
 * 
 * AOB_3128_1/1   3/17/11 6:16p cbrooks
 * sw3128-1:Added OOB code
 * 
 * 3   3/11/11 3:49p farshidf
 * SW3128-1: latest chnages from Charlie
 * 
 * 2   3/8/11 2:46p cbrooks
 * sw3128-1:new OOB code
 * 
 * 1   2/23/11 7:09p cbrooks
 * sw3128-1:new code
 * 
 *****************************************************************************/
#ifndef BAOB_UTILS_H__
#define BAOB_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif



typedef enum BW_Sel_s
{
  BW_Sel_eAcquisition_BW, 
  BW_Sel_eTracking_BW
}BW_Sel_t;

typedef struct BAOB_PhaseLoopBW_Tables_s
{
uint8_t Lin_PhaseLoopBw;
uint8_t Int_PhaseLoopBw;
}BAOB_PhaseLoopBW_Tables_t; 

static const BAOB_PhaseLoopBW_Tables_t BAOB_PhaseLoopBw_Table[3][3][2] = 
{
	/* AcqLowBW     TrkLowBW        AcqMedBW      TrkMedBW        AcqHighBW    TrkHighBW*/
	/* Lin   Int    Lin   Int       Lin   Int     Lin   Int       Lin   Int    Lin   Int*/
	{{{0xfc, 0xdc}, {0xfe, 0xff}}, {{0xfc, 0xdc}, {0xfd, 0xfc}}, {{0xfc, 0xdc}, {0xfc, 0xf5}}}, /*DVS_178*/
	{{{0xF9, 0xC1}, {0xfe, 0xff}}, {{0xF9, 0xC1}, {0xfc, 0xf9}}, {{0xF9, 0xC1}, {0xf9, 0xc1}}}, /*DVS_167_GradeA*/
	{{{0xF9, 0xC1}, {0xfe, 0xff}}, {{0xF9, 0xC1}, {0xfc, 0xf9}}, {{0xF9, 0xC1}, {0xf9, 0xc1}}}  /*DVS_167_GradeB*/
};	

typedef struct BAOB_TimingLoopBW_Tables_s
{
uint32_t Lin_TimingLoopBw;
uint32_t Int_TimingLoopBw;
}BAOB_TimingLoopBW_Tables_t;

static const BAOB_TimingLoopBW_Tables_t BAOB_TimingLoopBW_Table[3][2] = 
{
	/*  AcqLowBW    AcqLowBW      TrkLowBW     TrkLowBW*/
	/*     Lin         Int           Lin          Int  */
	{{0x0a4cf800, 0x02ad3600}, {0x020f6400, 0x00183693}}, /*DVS_178*/
	{{0x0db2f700, 0x042ea46d}, {0x0db2f700, 0x042ea46d}}, /*DVS_167_GradeA*/
	{{0x0db2f700, 0x042ea46d}, {0x0db2f700, 0x042ea46d}}  /*DVS_167_GradeB*/
};	

typedef struct BAOB_FEC_Tables_s
{
uint32_t STFECL;
uint32_t STFECH;
}BAOB_FEC_Tables_t;

/*DVS_178
 *acq: allow 2 of 16 to get in,    
 *trk: allow 7 of 13 bad MPEG headers
 *trk: allow 8 of 48 bad RS blocks
 *
 *DVS_167
 *acq: allow 2 of 16 to get in then
 *trk: allow 7 of 13 bad MPEG headers
 *trk: allow 8 of 48 bad RS blocks
 *
 *For TESTING
 *acq: allow 2 of 16 to get in
 *trk: allow 13 of 13 bad MPEG hdr
 *trk: allow 48 of 48 bad RS blcks
 */
static const BAOB_FEC_Tables_t BAOB_FEC_Table[4] = 
{
  /* STFECL      STFECH*/
	{0x08c7f200, 0x07300000}, /*DVS_178*/
	{0x05532100, 0x0B070000}, /*DVS_167*/
 /* {0x08c7f200, 0x07300000},*/ /*DVS_178*/
 	{0x31fFF200, 0x07300000}, /*for BERT tests - Annex B*/
	{0x31fff200, 0x03300000}, /*for BERT tests - Annex A*/
};	

/*****************************************************************************
 * ADS Function Prototypes Used by PI or Local 
 *****************************************************************************/


/*****************************************************************************
 * ADS Function Prototypes Used Local 
 *****************************************************************************/
void     BAOB_P_Set_CFL_Frequency(BAOB_3x7x_Handle hChn, int32_t CFL_Frequency);
int32_t  BAOB_P_Get_CFL_FrequencyError(BAOB_3x7x_Handle hChn);
void     BAOB_P_Set_CFL_BW(BAOB_3x7x_Handle h, BW_Sel_t BW_Sel);
uint32_t BAOB_P_Get_SymbolRate(BAOB_3x7x_Handle hChn);
void     BAOB_P_Set_TL_Frequency(BAOB_3x7x_Handle h, uint32_t Symbol_Rate);
int32_t  BAOB_P_Get_TL_FrequencyError(BAOB_3x7x_Handle h);
void     BAOB_P_Set_TL_BW(BAOB_3x7x_Handle h, BW_Sel_t BW_Sel);
void     BAOB_P_Set_FEC(BAOB_3x7x_Handle h);
void     BAOB_P_Set_SNR(BAOB_3x7x_Handle h);

#ifdef __cplusplus
}
#endif

#endif


