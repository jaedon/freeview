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
 * $brcm_Workfile: baob_acquire.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/22/12 4:43p $
 * 
 * Module Description:
 * 
 * Revision History:
 * 
 * $brcm_Log: /magnum/portinginterface/aob/7552/baob_acquire.c $
 * 
 * Hydra_Software_Devel/1   6/22/12 4:43p farshidf
 * SW7425-2983: Remove symbolic links and flatten out ADS and AOB
 * directory structure
 * 
 * 24   3/13/12 5:03p farshidf
 * SW3128-1: reset status lock at power down
 * 
 * 23   2/15/12 6:50p farshidf
 * SW3128-1: merge to main
 * 
 * Fw_Integration_Devel/7   2/15/12 6:41p farshidf
 * SW3128-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V4_0_AOB_DEV/1   2/15/12 5:15p dorothyl
 * SW3128-1 : bug fix
 * 
 * Fw_Integration_Devel/6   2/9/12 12:14p farshidf
 * SW3128-1: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/9   2/8/12 4:10p dorothyl
 * SW3128-1 : oob fec bug
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/8   2/8/12 2:16p dorothyl
 * SW3128-1 : OOB BERT
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/7   2/3/12 1:56p dorothyl
 * SW3128-1 : clean up comments
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/6   1/6/12 1:24p dorothyl
 * SW3128-1 : add nyq option
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/5   12/14/11 11:14a mpovich
 * SW3128-1: Incorporate changes from Dorothy Lew on 12/14/2011, 11:00 am.
 * 
 * 18   12/13/11 5:25p mpovich
 * SW3128-1: Merge to main branch.
 * 
 * Fw_Integration_Devel/3   12/13/11 5:25p mpovich
 * SW3128-1: Merge to integ. branch.
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/4   12/13/11 5:21p mpovich
 * SW3128-1: Take Dorothy's OOB changes on 12/12/2011.
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/3   12/13/11 4:50p mpovich
 * SW3128-1: Take Dorothy's OOB changes on 12/12/2011.
 * 
 * 17   12/13/11 1:21p mpovich
 * SW3128-1: Merge to the main branch.
 * 
 * Fw_Integration_Devel/2   12/13/11 12:26p mpovich
 * SW3128-69: Add lock/unlock IRQs and status for Out of Band.
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/2   12/13/11 12:25p mpovich
 * SW3128-69: Add lock/unlock IRQs and status for Out of Band.
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/SW3128-69/2   12/13/11 12:09p mpovich
 * SW3128-69: Add lock/unlock IRQs and status for Out of Band.
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/SW3128-69/1   12/12/11 9:38p mpovich
 * SW3128-69: Support for lock/unlock Host IRQs for Out of Band.
 * 
 * 16   11/22/11 4:55p farshidf
 * SW3461-99: compile fix
 * 
 * 15   11/22/11 4:36p farshidf
 * SW3461-99: merge to main
 * 
 * Fw_Integration_Devel/1   11/22/11 3:22p farshidf
 * SW3461-99: merge to integ
 * 
 * Fw_Integration_Devel/1   11/22/11 3:22p farshidf
 * SW3461-99: merge to integ
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/1   11/22/11 10:08a mpovich
 * SW3128-77: Merge to dev. branch.
 * 
 * Fw_Integration_Devel/AP_V3_0_AOB_DEV/SW3128-71/1   11/21/11 4:43p mpovich
 * SW3128-77: Fix AOB RAM Power down setting (change value written from
 *  0x7 to 0x3 [is 2 bit field]).
 * 
 * 13   4/18/11 5:32p farshidf
 * SW3128-1: update to make it work with host chip
 * 
 * 12   4/12/11 11:47a farshidf
 * SW3128-1: fix warning
 * 
 * 11   4/5/11 3:21p farshidf
 * SW3461-1: merge  main
 * 
 * AOB_3128_1/13   4/5/11 10:54a dorothyl
 * SW3128-1: OOB UPDATE
 * 
 * AOB_3128_1/12   3/31/11 11:32a dorothyl
 * SW3128-1: oob status fix
 * 
 * AOB_3128_1/11   3/29/11 12:05p dorothyl
 * SW3128-1: oob
 * 
 * AOB_3128_1/10   3/29/11 10:39a dorothyl
 * SW3128-1: oob bert
 * 
 * AOB_3128_1/9   3/28/11 2:33p dorothyl
 * SW3128 : OOB FIXED
 * 
 * AOB_3128_1/8   3/25/11 11:26a dorothyl
 * SW3128-1: fix OOB
 * 
 * AOB_3128_1/7   3/23/11 12:49p mpovich
 * SW3128-1: Pick up HAB related updates.
 * 
 * AOB_3128_1/6   3/23/11 12:42p dorothyl
 * sw3128-1: oob change
 * 
 * 6   3/22/11 5:03p mpovich
 * SW3128-1: Add latest AOB driver changes.
 * 
 * AOB_3128_1/5   3/21/11 6:30p farshidf
 * SW3128-1: add the IRQ for lock and lost of lock
 * 
 * AOB_3128_1/4   3/21/11 6:25p farshidf
 * SW3461-1: update naming
 * 
 * AOB_3128_1/3   3/18/11 4:37p cbrooks
 * sw3128-1:Added Status for OOB
 * 
 * AOB_3128_1/2   3/17/11 8:38p cbrooks
 * sw3128-1:Worked on AOB status
 * 
 * AOB_3128_1/1   3/17/11 6:16p cbrooks
 * sw3128-1:Added OOB code
 * 
 * 5   3/11/11 3:49p farshidf
 * SW3128-1: latest chnages from Charlie
 * 
 * 4   3/9/11 5:21p farshidf
 * SW3128-1: fix build issue
 * 
 * 3   3/8/11 3:10p farshidf
 * SW3128-1: add the OOb chnages
 * 
 * 2   3/8/11 2:45p cbrooks
 * sw3128-1:NEW CODE
 * 
 *****************************************************************************/


#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"
#include "btmr.h"
#ifdef LEAP_BASED_CODE
#include "baob_api.h"
#include "bchp_tm.h"
#include "bchp_leap_ctrl.h"
#else
BDBG_MODULE(baob_acquire);
#include "baob.h"
#endif
#include "baob_struct.h"
#include "baob_acquire.h"
#include "baob_status.h"
#include "baob_utils.h"
#include "baob_priv.h"
#include "bchp_ds_topm.h"
#include "bchp_oob.h"




/**************************************************************************
* BAOB_P_PowerUp()is used to power up the ADS core, it should be followed 
* by an initialization of the OOB core using BAOB_P_Init()								 
***************************************************************************/
BERR_Code BAOB_P_PowerUp(BAOB_3x7x_Handle h)
{
	BERR_Code retCode = BERR_SUCCESS;
	BDBG_MSG(("BAOB_P_PowerUp() "));
#ifdef LEAP_BASED_CODE
		BREG_WriteField(h->hRegister, TM_MEM_CTRL, PSM_RAM_OB, 0x2);
		BKNI_Delay(20);
		BREG_WriteField(h->hRegister, TM_MEM_CTRL, PSM_RAM_OB, 0x0);
#endif
		BREG_Write32(h->hRegister, BCHP_OOB_CTRL2, 0x9b0300);
		/*--DDL BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, 0x7fff0010);  */
		BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, 0xff7f1f00);
		BREG_Write32(h->hRegister, BCHP_OOB_CTRL2, 0x9b0300);
	    BREG_Write32(h->hRegister, BCHP_OOB_CTRL3, 0x320012cc);
		/*h->pAcqParam->BAOB_Local_Params.CTRL3 =  0x320012cc;
		h->pAcqParam->BAOB_Local_Params.BERSRC = 3;*/
		/*BREG_Write32(h->hRegister, BCHP_OOB_CTRL3, 0x11dc);*/
		BREG_Write32(h->hRegister, BCHP_OOB_CTRL4, 0x3060f);
		BREG_Write32(h->hRegister, BCHP_OOB_CTRL5, 0x400002);
	  h->PowerStatus = BAOB_ePower_On;

	return retCode;
}


BERR_Code BAOB_P_PowerDn(BAOB_3x7x_Handle h)
{
	BERR_Code retCode = BERR_SUCCESS;
	BDBG_MSG(("BAOB_P_PowerDn() "));
#ifdef LEAP_BASED_CODE
	BREG_WriteField(h->hRegister,TM_MEM_CTRL, PSM_RAM_OB, 0x3);
	BREG_WriteField(h->hRegister,OOB_CTRL2, OBPWR, 0x1);
#else
	h->PowerStatus = BAOB_ePower_Off;
	h->pStatus->RLK = BAOB_Status_eUnlock;
	h->pStatus->FLK = BAOB_Status_eUnlock;
	BSTD_UNUSED(h);
#endif	
	return retCode;
}


BERR_Code BAOB_P_Init(BAOB_3x7x_Handle h)
{
	BERR_Code retCode = BERR_SUCCESS;
    BSTD_UNUSED(h);
	BDBG_MSG(("BAOB_P_Init() "));
	
	return retCode;
}

BERR_Code BAOB_P_Get_LockStatus(BAOB_3x7x_Handle h)
{
	BERR_Code retCode = BERR_SUCCESS;
	BSTD_UNUSED(h);
	BDBG_MSG(("BAOB_P_Get_LockStatus() "));

	return retCode;
}

BERR_Code BAOB_P_Acquire(BAOB_3x7x_Handle h)
{
	int32_t  CFL_Frequency;
	uint32_t Symbol_Rate ;
	uint8_t  AutoInversionFlag, FEC_LockFlag;
	uint32_t ReadReg;

	/* BDBG_MSG(("BAOB_P_Acquire() ")); */

	h->pStatus->Reack_Count++;

		/* this a bug in A0 where the status needs to clear the first time */
    BREG_Write32(h->hRegister, BCHP_OOB_IRQCLR2A, 0xffffffff); 
	BREG_Write32(h->hRegister, BCHP_OOB_IRQMST2A, 0xffffffff); 

	/* was part of the oob_reset bbs routine*/
 	BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, 0xff7f1f00); /*frz DAGC,FFE,DFE,DRL,BRL,AGL; rst FEC,FFE,DFE,SNR,DRL,BRL,AGL*/


	BREG_Write32(h->hRegister, BCHP_OOB_CONFIG, 0xF0C00000); /*no byp; FFE/DFE 2^-11*/
	BREG_Write32(h->hRegister, BCHP_OOB_CTRL4, 0x00030603); /*frz DR2; rst FLI,DCC*/
	BREG_Write32(h->hRegister, BCHP_OOB_CTRL2, 0x00628300);  /*rst slicer off, spec inv off; 2 taps, main F1*/

	/*Program CFL_Frequency*/
	CFL_Frequency = BAOB_IF_FREQUENCY;
	BAOB_P_Set_CFL_Frequency(h, BAOB_IF_FREQUENCY);

	BAOB_P_Set_CFL_BW(h, BW_Sel_eAcquisition_BW);

	/*Program TL_Frequency*/
	switch (h->pAcqParam->BAOB_Acquire_Params.AA)
	{
	case BAOB_Acquire_Params_BPS_eDVS178: 
		Symbol_Rate = 1024000;
 		break;
	case BAOB_Acquire_Params_BPS_eDVS167:
		if(h->pAcqParam->BAOB_Acquire_Params.BPS==BAOB_Acquire_Params_BPS_eDVS_167_GradeA)
			Symbol_Rate = 772000;
		else
			Symbol_Rate = 1544000;
		break;
	default :
		Symbol_Rate = 0;
		BDBG_ERR(("UNKNOWN BAOB_Acquire_Params.BPS in BAOB_P_Acquire()"));
	}
 
	BAOB_P_Set_TL_Frequency(h, Symbol_Rate);

	/*Program FEC*/
	BAOB_P_Set_FEC(h);

   
	BAOB_P_Set_TL_BW(h, BW_Sel_eAcquisition_BW);

	/*Spectral Inversion control*/
	if (h->pAcqParam->BAOB_Acquire_Params.IS == BAOB_Acquire_Params_eDisable) 
	{
		BREG_WriteField(h->hRegister, OOB_CTRL2, OFMT, 0);
	}
	else 
	{
		BREG_WriteField(h->hRegister, OOB_CTRL2, OFMT, 1);
	}
  	/*AGC/PGA setup and control*/

   if (h->pAcqParam->BAOB_Acquire_Params.BYP == BAOB_Acquire_Params_eEnable)                      /*Bypass FEC in "CC" mode*/
	{
		if (CLOCK_INVERT == 1) 
		{
			BREG_Write32(h->hRegister, BCHP_OOB_CTRL5, 0x3d400002);
		}
		else
		{
			BREG_Write32(h->hRegister, BCHP_OOB_CTRL5, 0x39400002);  
		}       
	}
	else
	{
		if (CLOCK_INVERT == 1) 
		{
			BREG_WriteField(h->hRegister, OOB_CTRL2, INVCK, 1);
		}
		else
		{
			BREG_WriteField(h->hRegister, OOB_CTRL2, INVCK, 0);  
		}
		BREG_Write32(h->hRegister, BCHP_OOB_CTRL5, 0x38400002);  /*PGA normal operation; halfband/nyq filt gain, FEC output control*/
	}

 	/*Set BERT Source*/
	/*Need to add a shadow  hack since this register can not be read back*/
	/*BDBG_ERR(( "BERSRC = %32x", h->pAcqParam->BAOB_Acquire_Params.BERSRC));*/
   	switch (h->pAcqParam->BAOB_Acquire_Params.BERSRC)
	{
	case BAOB_Acquire_Params_BERT_Source_eI:
		BREG_WriteField(h->hRegister, OOB_CTRL3, BERSRC, 0x0);
		/*BDBG_ERR(( "BEFORE WRITE (i) CTRL3 = %32x", h->pAcqParam->BAOB_Local_Params.CTRL3));*/
		/*h->pAcqParam->BAOB_Local_Params.CTRL3 = h->pAcqParam->BAOB_Local_Params.CTRL3 & 0xcfffffff;*/
		break;
	case BAOB_Acquire_Params_BERT_Source_eQ:
		BREG_WriteField(h->hRegister, OOB_CTRL3, BERSRC, 0x1);
		/*BDBG_ERR(( "BEFORE WRITE (q) CTRL3 = %32x", h->pAcqParam->BAOB_Local_Params.CTRL3));*/
		/*h->pAcqParam->BAOB_Local_Params.CTRL3 = h->pAcqParam->BAOB_Local_Params.CTRL3 & 0xdfffffff; 
		h->pAcqParam->BAOB_Local_Params.CTRL3 = h->pAcqParam->BAOB_Local_Params.CTRL3 | 0x10000000;  */
			break;
	case BAOB_Acquire_Params_BERT_Source_eIQ:
		BREG_WriteField(h->hRegister, OOB_CTRL3, BERSRC, 0x2);
		/*BDBG_ERR(( "BEFORE WRITE (iq) CTRL3 = %32x", h->pAcqParam->BAOB_Local_Params.CTRL3));*/
		/*h->pAcqParam->BAOB_Local_Params.CTRL3 = h->pAcqParam->BAOB_Local_Params.CTRL3 | 0x20000000; 
		h->pAcqParam->BAOB_Local_Params.CTRL3 = h->pAcqParam->BAOB_Local_Params.CTRL3 & 0xefffffff;  */
		break;
	case BAOB_Acquire_Params_BERT_Source_eFEC:
		BREG_WriteField(h->hRegister, OOB_CTRL3, BERSRC, 0x3);
		/*BDBG_ERR(( "BEFORE WRITE(fec) == CTRL3 = %32x", h->pAcqParam->BAOB_Local_Params.CTRL3));*/
		/*h->pAcqParam->BAOB_Local_Params.CTRL3 = h->pAcqParam->BAOB_Local_Params.CTRL3 | 0x30000000; 
 		h->pAcqParam->BAOB_Local_Params.BERSRC = BAOB_Local_Params_BERT_Source_eFEC;	*/
 		break;
	default :
		BREG_WriteField(h->hRegister, OOB_CTRL3, BERSRC, 0x3);
		/*h->pAcqParam->BAOB_Local_Params.CTRL3 = h->pAcqParam->BAOB_Local_Params.CTRL3 & 0xcfffffff; */
		/*BDBG_ERR(("UNKNOWN BAOB_Acquire_Params.BERSRC in BAOB_P_Acquire()"));*/
	}

	/*BREG_Write32(h->hRegister, BCHP_OOB_CTRL3, h->pAcqParam->BAOB_Local_Params.CTRL3);*/
	/*BDBG_ERR(( "AFTERWRITE CTRL3=%32x", h->pAcqParam->BAOB_Local_Params.CTRL3));*/
	
	/*set SNR thresholds*/
	BAOB_P_Set_SNR(h);


	/*Enable BERT Source*/
   /*	h->pAcqParam->BAOB_Local_Params.CTRL3 = h->pAcqParam->BAOB_Local_Params.CTRL3 | 0x02000000; 
	BREG_Write32(h->hRegister, BCHP_OOB_CTRL3, h->pAcqParam->BAOB_Local_Params.CTRL3);*/
	BREG_WriteField(h->hRegister, OOB_CTRL3, BERCEN, 0x1);
	
	BREG_Write32(h->hRegister, BCHP_OOB_STBRI, 0);  /*initialize baud integrator*/
	BREG_Write32(h->hRegister, BCHP_OOB_STDRI, 0);  /*initialize (CFL) DR1 integrator*/
	BREG_Write32(h->hRegister, BCHP_OOB_STD2I, 0);  /*initialize (CPL) DR2 integrator*/
	BREG_Write32(h->hRegister, BCHP_OOB_STATHR, 0x06000000);  /*A/D loading and AGC loop threshold*/
	BREG_Write32(h->hRegister, BCHP_OOB_BYP, 0x1);  /*bypass the dagc*/

	BREG_Write32(h->hRegister, BCHP_OOB_CTRL6, 0x05040011); /*enable self clearing of FEC counter after reading*/
	BREG_Write32(h->hRegister, BCHP_OOB_STBERTT, 0xf0000500); 

  	/*BEGIN ACQUISITION*/

	
	/*BDBG_MSG(( "OOB BPS=%32x", h->pAcqParam->BAOB_Acquire_Params.BPS));*/

	if(h->pAcqParam->BAOB_Acquire_Params.AA==BAOB_Acquire_Params_BPS_eDVS178)/*if annex B*/
	{
		BREG_Write32(h->hRegister, BCHP_OOB_STDRSP, 0x00a10000);  /* window set and freq det enabled*/
	}
	else /*annex A*/
	{
		BREG_Write32(h->hRegister, BCHP_OOB_STDRSP, 0x00a00000);  /* window set and freq det enabled*/	
	}

  /*AGC*/
	ReadReg = BREG_Read32(h->hRegister, BCHP_OOB_CTRL1); 
	ReadReg = ReadReg & 0x003eff00;
	ReadReg = ReadReg | 0x003e0f00;/*do not overwrite the aci/nyq filter selection*/
	BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, ReadReg);  /*release AGC and main tap*/
		/*BDBG_MSG(( "OOB_CTRL1 written with %x",ReadReg));*/

	if (DAGC_FREEZE == 1)
	{	
		/*BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, 0x00be1f00); release AGC and main tap, DAGC still frozen*/
		ReadReg = BREG_Read32(h->hRegister, BCHP_OOB_CTRL1); 
		ReadReg = ReadReg & 0x00beff00;
		ReadReg = ReadReg | 0x00be0f00;/*do not overwrite the aci/nyq filter selection*/
		BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, ReadReg);  
	}
	else
	{
		/*BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, 0x003e1f00);  release AGC and main tap and DAGC */
		ReadReg = BREG_Read32(h->hRegister, BCHP_OOB_CTRL1); 
		ReadReg = ReadReg & 0x003eff00;
		ReadReg = ReadReg | 0x003e0f00;/*do not overwrite the aci/nyq filter selection*/
		BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, ReadReg);  
	}

	BKNI_Sleep(AGC_WAIT_TIME);					/*AGC settling time*/
  
	/*baud/timing*/
	BREG_Write32(h->hRegister, BCHP_OOB_STBEN, 0x01000000);  /*baud loop enable */
	if (DAGC_FREEZE == 1)
	{	
		/*BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, 0x00bc1f00);  release baud, AGC, and main tap, DAGC still frozen*/
		ReadReg = BREG_Read32(h->hRegister, BCHP_OOB_CTRL1); 
		ReadReg = ReadReg & 0x00bcff00;
		ReadReg = ReadReg | 0x00bc0f00;/*do not overwrite the aci/nyq filter selection*/
		BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, ReadReg);  
	}
	else
	{
		/*BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, 0x003c1f00);  release baud, AGC, and main tap, DAGC running */
		ReadReg = BREG_Read32(h->hRegister, BCHP_OOB_CTRL1); 
		ReadReg = ReadReg & 0x003cff00;
		ReadReg = ReadReg | 0x003c0f00;/*do not overwrite the aci/nyq filter selection*/
		BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, ReadReg); 

	}
	BKNI_Sleep(BAUD_WAIT_TIME);					/*baud loop convergence time*/
   
  /*FEC*/
	if (DAGC_FREEZE == 1)
	{	
		/*BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, 0x40bc1f00);  FEC reset after FEC setup and after baud acquired, DAGC still frozen*/
		ReadReg = BREG_Read32(h->hRegister, BCHP_OOB_CTRL1); 
		ReadReg = ReadReg & 0x40bcff00;
		ReadReg = ReadReg | 0x40bc0f00;/*do not overwrite the aci/nyq filter selection*/
		BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, ReadReg);  
	}
	else
	{
		/*BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, 0x403c1f00);  FEC reset after FEC setup and after baud acquired, DAGC running*/
		ReadReg = BREG_Read32(h->hRegister, BCHP_OOB_CTRL1); 
		ReadReg = ReadReg & 0x403cff00;
		ReadReg = ReadReg | 0x403c0f00;/*do not overwrite the aci/nyq filter selection*/
		BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, ReadReg);  

	}
   BKNI_Sleep(CARRIER_WAIT_TIME);					/*CFL convergence time*/
 
  /*FEC reset to take in new data*/
	if (DAGC_FREEZE == 1)
	{	
		/*BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, 0x00a01f00);  release CFL, baud, AGC, main tap, DFE, FFE,jianh do not release eq, DAGC still frozen*/
		ReadReg = BREG_Read32(h->hRegister, BCHP_OOB_CTRL1); 
		ReadReg = ReadReg & 0x00a0ff00;
		ReadReg = ReadReg | 0x00a00f00;/*do not overwrite the aci/nyq filter selection*/
		BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, ReadReg); 

	}
	else
	{
		/*BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, 0x00201f00);  release CFL, baud, AGC, main tap, DFE, FFE,jianh do not release eq, DAGC running*/
		ReadReg = BREG_Read32(h->hRegister, BCHP_OOB_CTRL1); 
		ReadReg = ReadReg & 0x0020ff00;
		ReadReg = ReadReg | 0x00200f00;/*do not overwrite the aci/nyq filter selection*/
		BREG_Write32(h->hRegister, BCHP_OOB_CTRL1, ReadReg);  
}
	BKNI_Sleep(CARRIER_WAIT_TIME);					/*CFL convergence time*/
  
   	/*start tracking*/
   BAOB_P_Set_TL_BW(h, BW_Sel_eTracking_BW);
  BAOB_P_Set_CFL_BW(h, BW_Sel_eTracking_BW);
     
   	BREG_Write32(h->hRegister, BCHP_OOB_STFLTH, 0x0D060000); 
 
 	/*Do not wait for FEC or auto invert spectrum in Cable Card Mode*/
	if (h->pAcqParam->BAOB_Acquire_Params.BYP == BAOB_Acquire_Params_eEnable)
	{
		BKNI_Sleep(CABLECARD_WAIT_TIME);
	}
	else
	{
		/*settling time for FEC to declare/maintain lock*/
		if (h->pAcqParam->BAOB_Acquire_Params.AA == BAOB_Acquire_Params_BPS_eDVS178)
		{
			BKNI_Sleep(ANNEXB_FEC_WAIT_TIME);					
		}
		else
		{
			BKNI_Sleep(ANNEXA_FEC_WAIT_TIME);
		}

		FEC_LockFlag = BREG_ReadField(h->hRegister, OOB_IRQST,	FEC_IS); 
		
		/*Spectral Inversion Auto control*/
		/*Only used when not in Cable Card mode*/
		if ((h->pAcqParam->BAOB_Acquire_Params.AI == BAOB_Acquire_Params_eEnable) && (FEC_LockFlag == 0))
		{
			AutoInversionFlag = BREG_ReadField(h->hRegister, OOB_CTRL2, OFMT);
			AutoInversionFlag = AutoInversionFlag ^ 1;
			BREG_WriteField(h->hRegister, OOB_CTRL2, OFMT, AutoInversionFlag);
			/*settling time for FEC to declare/maintain lock*/
			if (h->pAcqParam->BAOB_Acquire_Params.AA == BAOB_Acquire_Params_BPS_eDVS178)
			{
				BKNI_Sleep(ANNEXB_FEC_WAIT_TIME);					
			}
			else
			{
				BKNI_Sleep(ANNEXA_FEC_WAIT_TIME);
			}
		}
	}

	/*Setup Nyq filters*/
	/*h->pAcqParam->BAOB_Acquire_Params.Nyq=BAOB_Acquire_Params_Nyq_ehalf_raised;	*/  

	if(h->pAcqParam->BAOB_Acquire_Params.Nyq == BAOB_Acquire_Params_Nyq_ehalf_rtraised)
	{
		/*root raised cosine with 0.5 rolloff*/
		BREG_WriteField(h->hRegister, OOB_CTRL1, NYQ_SEL, 0);
		BDBG_MSG(("root raised cosine %d",h->pAcqParam->BAOB_Acquire_Params.Nyq )); 
	}
	else if(h->pAcqParam->BAOB_Acquire_Params.Nyq == BAOB_Acquire_Params_Nyq_ehalf_raised)
	{
		/*raised cosine with 0.5 rolloff*/
		BREG_WriteField(h->hRegister, OOB_CTRL1, NYQ_SEL, 1); 
		BDBG_MSG(("raised cosine %d",h->pAcqParam->BAOB_Acquire_Params.Nyq ));
	}
	else 
	{
		/*root raised cosine with 0.3 rolloff*/
		BREG_WriteField(h->hRegister, OOB_CTRL1, NYQ_SEL, 2); 
	}

	

	/*setup ACI filters*/
	if(Symbol_Rate == 1024000)
	{	
		BDBG_ERR(("Setting filters for 1.024"));
		BREG_WriteField(h->hRegister, OOB_BYP, ACI, 0); 	
		
		BREG_WriteField(h->hRegister, OOB_CTRL1, ACI_SEL, 2);
	}
	else if(Symbol_Rate == 772000)
	{	
		BDBG_ERR(("Setting filters for 0.772"));

		BREG_WriteField(h->hRegister, OOB_BYP, ACI, 0); 
		BREG_WriteField(h->hRegister, OOB_CTRL1, ACI_SEL, 1);
	}
	else if(Symbol_Rate == 1544000)
	{	
		BDBG_ERR(("Setting filters for 1.544"));
		BREG_WriteField(h->hRegister, OOB_BYP, ACI, 0); 
		BREG_WriteField(h->hRegister, OOB_CTRL1, ACI_SEL, 3);
	}


#if 0 	 
	BDBG_ERR(("OOB symbol rate = %d", Symbol_Rate));
		BREG_WriteField(h->hRegister, OOB_BYP, ACI, 0);
		BREG_WriteField(h->hRegister, OOB_CTRL1, NYQ_SEL, 0); 
		BREG_WriteField(h->hRegister, OOB_CTRL1, ACI_SEL, 0);
#endif

#if 0
	/* this a bug in A0 where the status needs to clear the first time */
	BREG_Write32(h->hRegister, BCHP_OOB_IRQMST2A, 0xffffffff); 

    if (h->pAcqParam->BAOB_Acquire_Params.BYP == BAOB_Acquire_Params_eEnable)
	{
		AobL2_EnableIsr(BCHP_INT_ID_AOB_SNR_IS_INTR);
		AobL2_EnableIsr(BCHP_INT_ID_AOB_SNR_LS_INTR);
	}
	else
	{
		AobL2_EnableIsr(BCHP_INT_ID_AOB_FEC_IS_INTR);
		AobL2_EnableIsr(BCHP_INT_ID_AOB_FEC_LS_INTR);
	}
#endif
		return BERR_SUCCESS;
}
