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
 * $brcm_Workfile: baob_utils.c $
 * $brcm_Revision: 17 $
 * $brcm_Date: 2/9/12 12:45p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /AP/ctfe/core/aob/baob_utils.c $
 * 
 * 17   2/9/12 12:45p farshidf
 * SW3128-1: merge to main
 * 
 * Fw_Integration_Devel/6   2/9/12 12:19p farshidf
 * SW3128-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/8   2/8/12 4:11p dorothyl
 * SW3128-1 : bug in fec
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/7   2/8/12 2:05p dorothyl
 * SW3128-1: oob bert
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/6   2/3/12 1:56p dorothyl
 * SW3128-1 : clean up comments
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/5   1/5/12 9:41a dorothyl
 * SW3128-1: oob fix
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/4   12/14/11 2:13p mpovich
 * SW3128-1: Add changes per Dorothy Lew on 12/14/2011, 2pm.
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/3   12/14/11 2:03p mpovich
 * SW3128-1: Merge to Fw Integ. and to the Sys. devl. branch.
 * 
 * Fw_Integration_Devel/3   12/14/11 2:02p mpovich
 * SW3128-1: Merge to Fw Integ. and to the Sys. devl. branch.
 * 
 * 14   12/14/11 11:27a farshidf
 * SW7552-170: fix for 7552
 * 
 * 13   12/13/11 5:06p mpovich
 * SW3128-1: Merge to Fw devel. branch.
 * 
 * Fw_Integration_Devel/2   12/13/11 5:05p mpovich
 * SW3128-1: Merge to Fw devel. branch.
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/2   12/13/11 4:50p mpovich
 * SW3128-1: Take Dorothy's OOB changes on 12/12/2011.
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/1   12/13/11 12:27p mpovich
 * SW3128-69: Add lock/unlock IRQs and status for Out of Band.
 * 
 * 12   12/13/11 4:41p mpovich
 * SW3128-1: merge to main branch.
 * 
 * Fw_Integration_Devel/1   12/13/11 12:13p mpovich
 * SW3128-69: Add lock/unlock IRQs and status for Out of Band.
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/SW3128-69/1   12/12/11 9:38p mpovich
 * SW3128-69: Support for lock/unlock Host IRQs for Out of Band.
 * 
 * 11   11/9/11 4:39p farshidf
 * SW7552-139: adapt the code for magnum support
 * 
 * 9   4/18/11 5:33p farshidf
 * SW3128-1: update to make it work with host chip
 * 
 * 8   4/12/11 11:47a farshidf
 * SW3128-1: fix warning
 * 
 * 7   4/5/11 3:24p farshidf
 * SW3461-1: merge  main
 * 
 * AOB_3128_1/9   4/5/11 10:54a dorothyl
 * SW3128-1: OOB UPDATE
 * 
 * AOB_3128_1/8   3/31/11 11:32a dorothyl
 * SW3128-1: oob status fix
 * 
 * AOB_3128_1/7   3/28/11 2:34p dorothyl
 * SW3128-1: OOB FIX
 * 
 * AOB_3128_1/6   3/25/11 11:29a dorothyl
 * SW3128-1: fix oob
 * 
 * 4   3/22/11 5:05p mpovich
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
 * 3   3/11/11 3:58p farshidf
 * SW3128-1: compile fix
 * 
 * 2   3/11/11 3:49p farshidf
 * SW3128-1: latest chnages from Charlie
 * 
 *****************************************************************************/


#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "btmr.h"
#include "bmth.h"

#ifdef LEAP_BASED_CODE
#include "bwfe_global_clk.h"
#include "baob_api.h"
#include "bchp_leap_ctrl.h"
#else
#include "baob.h"
BDBG_MODULE(baob_utils);
#define POWER2_24 16777216
#define POWER2_26 67108864
#define POWER2_18 262144
#define Twos_Complement32(x) ((uint32_t)((x ^ 0xFFFFFFFF) + 1))
#endif
#include "baob_struct.h"
#include "baob_acquire.h"
#include "baob_status.h"
#include "baob_utils.h"
#include "baob_priv.h"
#include "bchp_oob.h"




/******************************************************************************************** 
 *BAOB_P_Set_CFL_Frequency() determines the value to program in the front mixer	                 
	*Only 0 is supported for now                                                                     
 ********************************************************************************************/
void BAOB_P_Set_CFL_Frequency(BAOB_3x7x_Handle h, int32_t CFL_Frequency)
{
	/*Detect if CFL_Frequency is NON zero*/
	if (CFL_Frequency != 0)
	{
				BDBG_ERR(("CFL_Frequency is not 0 in BADS_P_Set_CFL_Frequency()"));
	}

	/*Program the  Carrier Loop Frequency Control Word*/
	BREG_Write32(h->hRegister, BCHP_OOB_STDRI, 0);
}

/******************************************************************************************** 
 *BAOB_P_Get_CFL_FrequencyError() determin front mixer freqency error                                                                
 ********************************************************************************************/
int32_t BAOB_P_Get_CFL_FrequencyError(BAOB_3x7x_Handle h)
{
	bool RegIsNegative;
	uint32_t ReadReg;
	int32_t CFL_Error;
	uint32_t	ulMultA, ulMultB, ulNrmHi, ulNrmLo, ulDivisor;

	/*Read the Carrier Loop Frequency Integrator*/
	/*Carrier Offset = LDDRI/(2^24) * F_HS*/
	ReadReg = BREG_Read32(h->hRegister, BCHP_OOB_LDDRI);
	RegIsNegative=false;
	/*This is a 24 bit 2's complement register MSB justified, if negative, clamp, sign extend and twos complement*/
	ReadReg = ReadReg/256;
	if ((ReadReg & 0x00800000) != 0)
	{
		RegIsNegative = true;
		ReadReg = ReadReg | 0xFF000000;	/*sign extend*/
		ReadReg = (ReadReg == 0xFF800000) ? Twos_Complement32(0xFF800001) : Twos_Complement32(ReadReg);
	}

	ulMultA = ReadReg;
	ulMultB = F_HS;/*POWER2_24;*/
	ulDivisor = POWER2_24;/*F_HS;*/
	/*BDBG_ERR(("%x, freq err=%d",ReadReg,ReadReg/POWER2_24));*/
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);

	/*If result should be negative, take twos complement of output*/
	ulNrmLo = (RegIsNegative == true) ? Twos_Complement32(ulNrmLo) : ulNrmLo;
	
	CFL_Error = (int32_t)ulNrmLo;
	return	CFL_Error;
}

/******************************************************************************************** 
 *BAOB_P_Set_CFL_BW() sets BW for carrier recovery loop                                                                  
 ********************************************************************************************/
void BAOB_P_Set_CFL_BW(BAOB_3x7x_Handle h, BW_Sel_t BW_Sel)
{
	
	uint8_t  BPS_Index, PLBW_Index;
	uint32_t BW_Reg32L, BW_Reg32I;

	switch (h->pAcqParam->BAOB_Acquire_Params.AA)
	{
		case BAOB_Acquire_Params_BPS_eDVS178:
			BPS_Index = 0;
			break;
		case BAOB_Acquire_Params_BPS_eDVS167:
			if(h->pAcqParam->BAOB_Acquire_Params.BPS==BAOB_Acquire_Params_BPS_eDVS_167_GradeA) 
			{
				BPS_Index = 1;
			}
			else
			{
				BPS_Index = 2;
			}		
			break;
		default:
			BPS_Index = 0;
			BDBG_ERR(("UNKNOWN BAOB_Acquire_Params.AA in BAOB_P_Set_CFL_BW()"));
			break;
	}

#if TRUE /*currently fix it to medium*/
	PLBW_Index = 1; 
#else
	switch (h->pAcqParam->BAOB_Acquire_Params.PLBW)
	{
		case BAOB_Acquire_Params_PLBW_eLow:
			PLBW_Index = 0;
			break;
		case BAOB_Acquire_Params_PLBW_eMed:
			PLBW_Index = 1;
			break;
		case BAOB_Acquire_Params_PLBW_eHigh:
			PLBW_Index = 2;
			break;
		default:
			PLBW_Index = 0;
			BDBG_ERR(("UNKNOWN BAOB_Acquire_Params.PLBW in BAOB_P_Set_CFL_BW()"));
			break;
	}
#endif

	/*Select Acquisition or Tracking BW*/
	if (BW_Sel == BW_Sel_eAcquisition_BW)
	{
		BW_Reg32L = BAOB_PhaseLoopBw_Table[BPS_Index][PLBW_Index][0].Lin_PhaseLoopBw;
		BW_Reg32I = BAOB_PhaseLoopBw_Table[BPS_Index][PLBW_Index][0].Int_PhaseLoopBw;
	}
	else
	{
		BW_Reg32L = BAOB_PhaseLoopBw_Table[BPS_Index][PLBW_Index][1].Lin_PhaseLoopBw;
		BW_Reg32I = BAOB_PhaseLoopBw_Table[BPS_Index][PLBW_Index][1].Int_PhaseLoopBw;
	}

	/*shift to 24 MSB's*/
	BW_Reg32L = (BW_Reg32L << 24) & 0xFF000000;
	BW_Reg32I = (BW_Reg32I << 24) & 0xFF000000;


	/*Program the Phase Loop BW Word*/
	BREG_Write32(h->hRegister, BCHP_OOB_STDRLC, BW_Reg32L);	
	BREG_Write32(h->hRegister, BCHP_OOB_STDRIC, BW_Reg32I);
}


/**************************************************************************************************** 
 *BAOB_P_Set_TL_Frequency() determines the value to program in the VID	                             
 *Calculate the Timing Loop Frequency Control Word a 24 bit 2's complement number                    
 *STBFOS = (4*2^24*BaudRate)/F_HS  or STBFOS = (2^26*BaudRate/F_HS)                                      
 ****************************************************************************************************/
void BAOB_P_Set_TL_Frequency(BAOB_3x7x_Handle h, uint32_t Symbol_Rate)
{

	uint32_t	ulMultA, ulMultB, ulNrmHi, ulNrmLo, ulDivisor;
	
	/*Check to make sure symbol rate is in range*/
	if ((Symbol_Rate < 100000) || (Symbol_Rate >= F_HS/4))
	{
		BDBG_ERR(("SymbolRate out of range in BAOB_P_Set_TL_Frequency()"));
	}

  ulMultA = POWER2_26;
	ulMultB = Symbol_Rate;
	ulDivisor = F_HS;
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);

	/*shift to 24 MSB's*/
	ulNrmLo = (ulNrmLo << 8) & 0xFFFFFF00;

	/*Program the Timing Loop Frequency Control Word*/
	BREG_Write32(h->hRegister, BCHP_OOB_STBFOS, ulNrmLo);	

}

/**************************************************************************************************** 
 *BAOB_P_Get_SymbolRate() determines the value in the VID	                             
 *Calculate the Timing Loop Frequency Control Word a 24 bit 2's complement number                    
 *STBFOS = (4*2^24*BaudRate)/F_HS  or STBFOS = (2^26*BaudRate/F_HS)     BaudRate =  STBFOS*F_HS/2^26                                 
 ****************************************************************************************************/
uint32_t BAOB_P_Get_SymbolRate(BAOB_3x7x_Handle h)
{
	uint32_t  ReadReg;
	uint32_t  ulMultA, ulMultB, ulNrmHi, ulNrmLo, ulDivisor;
	
	/*Read the Timing Loop Frequency Integrator*/
	ReadReg = BREG_Read32(h->hRegister, BCHP_OOB_STBFOS);
	ReadReg = ReadReg/256;
	 /*BDBG_MSG(("symbolrate = %d", ReadReg));*/
    ulMultA = ReadReg;
	ulMultB = F_HS;
	ulDivisor = POWER2_26;
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);
	
	return ulNrmLo;
}

/******************************************************************************************** 
 *BAOB_P_Get_TL_FrequencyError() determine fromt mixer freqency error                                                                
 ********************************************************************************************/
int32_t BAOB_P_Get_TL_FrequencyError(BAOB_3x7x_Handle h)
{
	bool RegIsNegative;
	uint32_t ReadReg;
	int32_t TL_Error;
	uint32_t	ulMultA, ulMultB, ulNrmHi, ulNrmLo, ulDivisor;
	
	RegIsNegative = false;

	/*Read the Timing Loop Frequency Integrator*/
	/*Timing Offset = LDBRFO/(2^26) * F_HS - symbol rate*/
	ReadReg = BREG_Read32(h->hRegister, BCHP_OOB_LDBRFO);

	/*This is a 32 bit 2's complement register, if negative, clamp, sign extend and twos complement*/
	ReadReg = ReadReg/256;
	if ((ReadReg & 0x00800000) != 0)
	{
		RegIsNegative = true;
		BDBG_ERR(("IS Negative %d", RegIsNegative));
		ReadReg = ReadReg | 0xFF000000;	/*sign extend*/
		ReadReg = (ReadReg == 0xff800000) ? Twos_Complement32(0xff800001) : Twos_Complement32(ReadReg);
	}

	ulMultA = POWER2_18;   /*supposed to be 2^18*/
	ulMultB = F_HS;
	ulDivisor = ReadReg;
	/*BDBG_MSG(("%x, integrator err %d %x",ReadReg,(2^18*ReadReg/F_HS),(2^18*ReadReg/F_HS)));*/
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);

	/* BDBG_MSG(("final = %x", ulNrmLo));*/
	/*If result should be negative, take twos complement of output*/
	ulNrmLo = (RegIsNegative == true) ? Twos_Complement32(ulNrmLo) : ulNrmLo;
	
	 /*BDBG_MSG(("err = %d %d", ulNrmLo, BAOB_P_Get_SymbolRate(h)));*/
	TL_Error = (int32_t)ulNrmLo;
	return	TL_Error;
}


/**************************************************************************************************** 
 *BAOB_P_Set_TL_BW() determines the loop bandwidth to program in the VID	                             
 ****************************************************************************************************/
 void BAOB_P_Set_TL_BW(BAOB_3x7x_Handle h, BW_Sel_t BW_Sel)
 {
	uint8_t  BPS_Index;
  uint32_t BW_Reg32L, BW_Reg32I;

	switch (h->pAcqParam->BAOB_Acquire_Params.AA)
	{
		case BAOB_Acquire_Params_BPS_eDVS178:
			BPS_Index = 0;
			break;
		case BAOB_Acquire_Params_BPS_eDVS167:
			if(h->pAcqParam->BAOB_Acquire_Params.BPS==BAOB_Acquire_Params_BPS_eDVS_167_GradeB)
			{
				BPS_Index = 1;		
			}
			else
			{
				BPS_Index = 2;
			}
			break;
		default:
			BPS_Index = 0;
			BDBG_ERR(("UNKNOWN BAOB_Acquire_Params.AA in BAOB_P_Set_TL_BW()"));
	}

	/*Select Acquisition or Tracking BW*/
	if (BW_Sel == BW_Sel_eAcquisition_BW)
	{
		BW_Reg32L = BAOB_TimingLoopBW_Table[BPS_Index][0].Lin_TimingLoopBw;
		BW_Reg32I = BAOB_TimingLoopBW_Table[BPS_Index][0].Int_TimingLoopBw;
	}
	else
	{
		BW_Reg32L = BAOB_TimingLoopBW_Table[BPS_Index][1].Lin_TimingLoopBw;
		BW_Reg32I = BAOB_TimingLoopBW_Table[BPS_Index][1].Int_TimingLoopBw;
	}

	/*Program the Phase Loop BW Word*/
	BREG_Write32(h->hRegister, BCHP_OOB_STBRLC, BW_Reg32L);	
	BREG_Write32(h->hRegister, BCHP_OOB_STBRIC, BW_Reg32I);
}

/**************************************************************************************************** 
 *BAOB_P_Set_FEC() determines the FEC programming	                             
 ****************************************************************************************************/
 void BAOB_P_Set_FEC(BAOB_3x7x_Handle h)
 {
	 uint8_t  AA_Index;
   uint32_t BW_Reg32H, BW_Reg32L;
	 
	 switch (h->pAcqParam->BAOB_Acquire_Params.BPS) /*FEC*/
	{
		case BAOB_Acquire_Params_BPS_eDVS178:
			AA_Index = 0;
			break;
		case BAOB_Acquire_Params_BPS_eDVS_167_GradeA:
			AA_Index = 1;		
			break;
		case BAOB_Acquire_Params_BPS_eDVS_167_GradeB:
			AA_Index = 1;		
			break;
		case BAOB_Acquire_Params_BPS_eBERT_TEST_MODE:
			if(h->pAcqParam->BAOB_Acquire_Params.AA==BAOB_Acquire_Params_BPS_eDVS167)
			{	 
				AA_Index = 3;
			}
			else
			{
				AA_Index = 2;
			}
			break;
		default:
			/*AA_Index = 0;*/
			AA_Index = 2;
			BDBG_ERR(("UNKNOWN BAOB_Acquire_Params.BPS in BAOB_P_Set_FEC()"));
	}
	

	 BDBG_MSG(("BPS= %d, AA Index = %d", h->pAcqParam->BAOB_Acquire_Params.BPS,AA_Index));
	/*Select FEC parameters*/
	BW_Reg32L = BAOB_FEC_Table[AA_Index].STFECL;
	BW_Reg32H = BAOB_FEC_Table[AA_Index].STFECH;

	/*Program the FEC*/
	BREG_Write32(h->hRegister, BCHP_OOB_STFECL, BW_Reg32L);	
	BREG_Write32(h->hRegister, BCHP_OOB_STFECH, BW_Reg32H);
 
 }	

 /**************************************************************************************************** 
 *BAOB_P_Set_SNR() determines the SNR threshold programming	                             
 ****************************************************************************************************/
 void BAOB_P_Set_SNR(BAOB_3x7x_Handle h)
 {
	 uint32_t ReadReg;

	/*SNR estimator setup and control*/
	/*set SNR estimator convergence time to roughly 10 ms*/
	BREG_Write32(h->hRegister, BCHP_OOB_STSNRC, 0x03000000);

	/*set STSNRC to indicate this is to set the SNR Low threshold*/
	/*This is hardcoded to SNR = 10 db, STSNRT = 10.0 ^ ((124.47 - SNR) / 20.0)*/
	ReadReg = BREG_Read32(h->hRegister, BCHP_OOB_STSNRC);	
	ReadReg = ReadReg & 0xFBFFFFFF;
	BREG_Write32(h->hRegister, BCHP_OOB_STSNRC, ReadReg);	
	BREG_Write32(h->hRegister, BCHP_OOB_STSNRT, 0x08129E00);	
	
	/*next write to stsnrt is low error thres, 10 ms conv*/
	/*next write to stsnrt is low error thres, 10 ms conv*/
	BREG_Write32(h->hRegister, BCHP_OOB_STSNRC, 0x07000000);
	
	/*set STSNRC to indicate this is to set the SNR High threshold*/
	/*STSNRT = 10.0 ^ ((124.47 - SNR) / 20.0)*/

	ReadReg = BREG_Read32(h->hRegister, BCHP_OOB_STSNRC);	
	ReadReg = ReadReg | 0x04000000;
	BREG_Write32(h->hRegister, BCHP_OOB_STSNRC, ReadReg);	
	/*BREG_Write32(h->hRegister, BCHP_OOB_STSNRT, 0x00826500);*/	
	/*BREG_Write32(h->hRegister, BCHP_OOB_STSNRT, 0x00535000); 37dB*/
	/*BREG_Write32(h->hRegister, BCHP_OOB_STSNRT, 0x003A3F00); 41dB*/
	/*BREG_Write32(h->hRegister, BCHP_OOB_STSNRT, 0x001d3100); 47dB*/
	  BREG_Write32(h->hRegister, BCHP_OOB_STSNRT, 0x0014aa00); /*50dB*/



 }
