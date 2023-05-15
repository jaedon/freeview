/***************************************************************************
 *     Copyright (c) 2003-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_7550_status.c $
 * $brcm_Revision: Hydra_Software_Devel/1 $
 * $brcm_Date: 6/22/12 12:46p $
 *
 * Module Description:
 *
 * Revision History:  $
 *
 * $brcm_Log: /magnum/portinginterface/ads/src/soc/7550/bads_7550_status.c $
 * 
 * Hydra_Software_Devel/1   6/22/12 12:46p farshidf
 * SW7425-2983: add 7550 ADS code
 * 
 * Hydra_Software_Devel/17   10/28/10 5:57p farshidf
 * SW7550-607: merge
 * 
 * Hydra_Software_Devel/ADS_7550_40_11/1   10/28/10 3:03p cbrooks
 * Added the Jam phase loop to from loop and changed leaf from FFE to DFE
 * 
 * Hydra_Software_Devel/13   5/18/10 11:57a farshidf
 * SW7550-440: merge to main branch
 * 
 * Hydra_Software_Devel/SW7550-236/1   4/21/10 10:28p pinglong
 * SW7550-236: It's not right for some item for status page of brutus.
 * 
 * Hydra_Software_Devel/12   4/14/10 4:50p farshidf
 * SW7550-38: merge teh ADS branch 9
 * 
 * Hydra_Software_Devel/ADS_7550_40_9/2   4/14/10 3:30p cbrooks
 * fixed agc
 * 
 * Hydra_Software_Devel/ADS_7550_40_9/1   4/12/10 7:22p cbrooks
 * fixed agc to report in percent
 * 
 * Hydra_Software_Devel/11   4/5/10 5:08p farshidf
 * SW7550-38: check-in charlie's changes
 * 
 * Hydra_Software_Devel/10   3/19/10 3:43p farshidf
 * SW7550-38: merge from brnach ADS-8
 * 
 * Hydra_Software_Devel/ADS_7550_40_8/2   3/18/10 3:06p cbrooks
 * Made some fixes to lock detect
 * 
 * Hydra_Software_Devel/9   1/8/10 4:50p farshidf
 * SW7550-38: merge ADs branch 6
 * 
 * Hydra_Software_Devel/ADS_7550_40_6/2   1/8/10 4:47p cbrooks
 * tuner agc work
 * 
 * Hydra_Software_Devel/ADS_7550_40_6/1   1/5/10 5:48p cbrooks
 * Assed PI Status
 * 
 * Hydra_Software_Devel/8   1/5/10 1:02p farshidf
 * SW7550-38: merge from ADS branch 5
 * 
 * Hydra_Software_Devel/ADS_7550_40_5/1   1/5/10 12:44p cbrooks
 * Big Qam Changes
 * 
 * Hydra_Software_Devel/6   12/16/09 3:57p farshidf
 * SW7550-38: merge the ADS branch 3
 * 
 * Hydra_Software_Devel/ADS_7550_40_3/1   12/10/09 8:28p cbrooks
 * Fixed a reset bug in the FEC counter
 * 
 * Hydra_Software_Devel/5   12/10/09 1:05p farshidf
 * SW7550-38: warning fix
 * 
 * Hydra_Software_Devel/4   12/9/09 3:37p farshidf
 * SW7550-38: merge branch ADS 2
 * 
 * Hydra_Software_Devel/ADS_7550_40_2/3   12/9/09 3:20p cbrooks
 * Fixed Auto  Acquire Counter
 * 
 * Hydra_Software_Devel/ADS_7550_40_2/2   12/9/09 2:54p cbrooks
 * First Pass at the QAM Acquisition Script (still lots of fixing needed)
 * 
 * Hydra_Software_Devel/ADS_7550_40_2/1   12/4/09 6:20p cbrooks
 * for farshid
 * 
 * Hydra_Software_Devel/1   12/3/09 6:45p farshidf
 * SW7550-38: inital ADS code for 7550
 * 
 ***************************************************************************/


#include "bstd.h"
#include "bads.h"
#include "bads_priv.h"
#include "bads_7550_api.h"
#include "bads_7550_acquire.h"
#include "bmth.h"

/*registers needed for the functions in this file*/
#include "bchp_ds.h" 
#include "bchp_ds_eq.h" 
#include "bchp_ds_tuner_chnz.h"

BDBG_MODULE(bads_7550_status);

/************************************************************************************************
 * BADS_Soc_P_Reset_Status()	 This routine resets the status parameters 
 ************************************************************************************************/
BERR_Code BADS_Soc_P_Reset_Status(BADS_ChannelHandle hChannel)
{
	BERR_Code retCode;
	BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
	BADS_Soc_Handle hImplDev = (BADS_Soc_Handle) hDev->pImpl;
	BADS_Soc_ChannelHandle  h = (BADS_Soc_ChannelHandle) (hChannel->pImpl); 

	/*Reset parts of the BADS_7550_P_Status_t structure*/	
	h->pStatus->SNR = 0;
	h->pStatus->SNRAVG = 0;
	h->pStatus->BerPreRS = 0;
	h->pStatus->BerPostRS = 0;
	h->pStatus->BERTMode = 0;
	h->pStatus->DS_CERC2 = 0;
	h->pStatus->DS_NBERC2 = 0;
	h->pStatus->DS_UERC2 = 0;
	h->pStatus->DS_CBERC2 = 0;
	h->pStatus->FEC_Corr_RS_Blocks = 0;
	h->pStatus->FEC_UCorr_RS_Blocks = 0;
	h->pStatus->FEC_Clean_RS_Blocks = 0;
	h->pStatus->ReAcquireCount = 0;
	h->pStatus->ReSyncCount = 0;
	h->pStatus->pCount = 0;
	h->pStatus->TimingStatus = 0;
	h->pStatus->SpectrumStatus = 0;
	h->pStatus->InterleaverDepth = 0;	
	h->pStatus->IsSpectrumInverted = 0;
	h->pStatus->IfFrequency = 0;
	h->pStatus->ModulationType = BADS_ModulationType_eAnnexBQam64;
	h->pStatus->AGF_Level = 0;
	h->pStatus->AGF2_Level = 0;
	h->pStatus->FEC_Corr_Bits = 0;

	/*Reset Error Counters in chip ONLY for the second set, the first set is reset by the BADS_Soc_P_Get_LockStatus()*/
	BREG_Write32(hImplDev->hRegister, BCHP_DS_TPFEC, 0x00001F00);

	/*ADS Status Reset Complete*/
	retCode = BERR_SUCCESS;
	return retCode;
}

/************************************************************************************************
 * BADS_Soc_P_Get_Status()	 This routine gets the status parameters 
 ************************************************************************************************/
BERR_Code BADS_Soc_P_Get_Status(BADS_ChannelHandle hChannel)
{
	BERR_Code retCode;
	BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
	BADS_Soc_Handle hImplDev = (BADS_Soc_Handle) hDev->pImpl;
  BADS_Soc_ChannelHandle h = (BADS_Soc_ChannelHandle)(hChannel->pImpl);

	/*Local Variables*/
	uint32_t ReadReg;
	uint8_t FilterFactor, ARFFactor;			      
	uint32_t SampleRate;
		
	/*Local Variables used for multipliers and dividers in math calculations*/
	uint32_t ulMultA, ulMultB, ulDivisor, ulNrmHi, ulNrmLo;


	/**************************************************/
	/*Determine the Auto Acquisition Mode             */
	/**************************************************/
	
	h->pStatus->AutoAcquireMode = h->pAcquireParam->AutoAcquireMode;

	/**************************************************/
	/*Determine the FEC and QAM Modes                 */
	/**************************************************/
	/*Get the FEC type, Interleaver Dept, Spectral Inversion State*/
	ReadReg = BREG_Read32(hImplDev->hRegister, BCHP_DS_FECU);
	if ((ReadReg & 0x00A00000) == 0x00000000)
	{
		h->pStatus->FecMode = 0;	/*Annex A Mode*/
		h->pStatus->InterleaverDepth = (ReadReg & 0x000F0000);
		ReadReg = BREG_Read32(hImplDev->hRegister, BCHP_DS_FECL);
		if ((ReadReg & 0x00000008) == 0x00000000)
		{
			h->pStatus->IsSpectrumInverted = 0;
		}
			else
		{
			h->pStatus->IsSpectrumInverted = 1;

		}
	}
	else if ((ReadReg & 0x00800000) == 0x00800000)	
	{
		h->pStatus->FecMode = 1;	/*Annex B Mode*/
		h->pStatus->InterleaverDepth = BADS_Soc_P_ReadField(hImplDev->hRegister, DS_FEC, IDS);
		ReadReg = BREG_Read32(hImplDev->hRegister, BCHP_DS_FECL);
		if ((ReadReg & 0x00000008) == 0x00000000)
		{
			h->pStatus->IsSpectrumInverted = 1;
		}
			else
		{
			h->pStatus->IsSpectrumInverted = 0;

		}
	}
	else
	{
		BDBG_WRN(("ERROR!!! UNSUPPORTED OR UNDEFINED FEC_MODE"));
	}
	
	/*Get the QAM Mode from the bits/symbol from the FEC*/
	ReadReg = BREG_Read32(hImplDev->hRegister, BCHP_DS_FECL);
	ReadReg = (ReadReg & 0x000000F0)>>4;
	switch (ReadReg)
	{
		case 3: h->pStatus->QamMode = (uint32_t)BADS_QAM_eQam16; break;			/*16 QAM Mode*/
		case 4: h->pStatus->QamMode = (uint32_t)BADS_QAM_eQam32; break;			/*32 QAM Mode*/
		case 5: h->pStatus->QamMode = (uint32_t)BADS_QAM_eQam64; break;			/*64 QAM Mode*/
		case 6: h->pStatus->QamMode = (uint32_t)BADS_QAM_eQam128; break;		/*128 QAM Mode*/
		case 7:	h->pStatus->QamMode = (uint32_t)BADS_QAM_eQam256; break;		/*256 QAM Mode*/
		case 8:	h->pStatus->QamMode = (uint32_t)BADS_QAM_eQam512; break;		/*512 QAM Mode*/
		case 9:	h->pStatus->QamMode = (uint32_t)BADS_QAM_eQam1024; break;		/*1024 QAM Mode*/
		default: BDBG_WRN(("WARNING!!! UNSUPPORTED OR UNDEFINED QAM_MODE")); break;
	}

	/*Determine the Combined QAM/FEC mode for Nexus*/
	if (h->pStatus->FecMode == 0) /*Annex A Mode*/
	{
		switch (ReadReg)
		{
			case 3: h->pStatus->ModulationType = BADS_ModulationType_eAnnexAQam16; break;
			case 4: h->pStatus->ModulationType = BADS_ModulationType_eAnnexAQam32; break;
			case 5: h->pStatus->ModulationType = BADS_ModulationType_eAnnexAQam64; break;
			case 6: h->pStatus->ModulationType = BADS_ModulationType_eAnnexAQam128; break;
			case 7: h->pStatus->ModulationType = BADS_ModulationType_eAnnexAQam256; break;
			case 8: h->pStatus->ModulationType = BADS_ModulationType_eAnnexAQam512; break;
			case 9: h->pStatus->ModulationType = BADS_ModulationType_eAnnexAQam1024; break;
			default: BDBG_WRN(("WARNING!!! UNSUPPORTED OR UNDEFINED QAM_MODE")); break;

		}
	}
	if (h->pStatus->FecMode == 1) /*Annex B Mode*/
	{
		switch (ReadReg)
		{
			case 5: h->pStatus->ModulationType = BADS_ModulationType_eAnnexBQam64; break;  
			case 7: h->pStatus->ModulationType = BADS_ModulationType_eAnnexBQam256; break;
			case 9: h->pStatus->ModulationType = BADS_ModulationType_eAnnexBQam1024; break;
			default: BDBG_WRN(("WARNING!!! UNSUPPORTED OR UNDEFINED QAM/FEC MODE")); break;

		}
	}
	/**************************************************/
	/*Determine QAM and FEC Lock Status               */
	/*This is done in the BADS_Soc_P_Get_LockStatus()*/
	/**************************************************/
	
	/*Call the Lock Status Function to update the state of h->pStatus->QamStatus and h->pStatus->FecStatus*/
	/*BADS_Soc_P_Get_LockStatus(hChannel);
	h->pStatus->QamStatus
	h->pStatus->FecStatus
	*/

	/*******************************************/
	/*Determine the VID_Sample and Symbol Rates*/
	/*******************************************/

	/*Get the Sample Rate*/
	SampleRate = BADS_75xx_P_Get_SampleRate(hChannel);
	h->pStatus->SampleRate = SampleRate; 
	
	/*Get the Symbol Rate which depends on the SampleRate, ARF, FilterFactor, and timing loop NCO value*/
	ARFFactor = BADS_75xx_P_Get_ARFFactor(hChannel);
	FilterFactor = BADS_75xx_P_Get_FilterFactor(hChannel);

	h->pStatus->SymbolRate = BADS_7550_P_Get_SymbolRate(hChannel, ARFFactor, FilterFactor, SampleRate);

	/****************************************************************************************************/
	/*Calculate the IF Frequency */
	/****************************************************************************************************/
	
	h->pStatus->IfFrequency = (int32_t)BADS_7550_P_Get_IfFrequency(hChannel, SampleRate, ARFFactor);

	/***************************************/
	/*Determine The AGF and AGCB gains     */
	/*Also need then in 1/10 pf a percent  */
	/***************************************/

	/*AGF Gain and Level Calculation*/
	/*AGFVAL is 24-bit 5.19 unsigned number*/
	/*  db = 20*log10(AGFVAL/2^19)-20*log10(2)*/
	/*  256*db = 5120*log10(AGFVAL)-5120*log10(2^19)-5120*log10(2)*/ 
	/*AGF_Level is AGFVAL*1000/2^24 for 1/10 percent*/
	ReadReg = BADS_Soc_P_ReadField(hImplDev->hRegister, DS_TUNER_CHNZ_AGFI, AGFVAL);
	h->pStatus->AGF = (BADS_7550_P_2560log10(ReadReg)<<1)-30825;  /*BBS will divide by 256 to get dB*/
	
	/*The max gain of AGF is 24 db, scale to 0-48 and clamp the lower limit to 0*/
	ReadReg = (uint32_t)((int32_t) h->pStatus->AGF + 6144);
	if ((ReadReg & 0x80000000) != 0)
	{
		ReadReg = 0;
	}

	ulMultA = ReadReg;
	ulMultB = 1000;
	ulDivisor = 12288;
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);
	if ((ulNrmHi != 0))
	{
		BDBG_WRN(("64to32 division is not in a 32 bit unsigned int range"));
	}

	/*clamp to be 0 to 1000*/
	if ((ulNrmLo > 1000))
	{
		ulNrmLo = 1000;
	}
	
	h->pStatus->AGF_Level = ulNrmLo;

	/*AGF2 Gain and Level Calculation*/
	/*AGCBVAL is 32-bit 7.25 unsigned number*/
	/*  db = 20*log10(AGCBVAL/2^25)-20*log10(2)*/
	/*  256*db = 5120*log10(AGCBVAL)-5120*log10(2^25)-5120*log10(2)*/ 
	/*AGF_Level is AGFVAL*1000/2^32 or AGFVAL*125/2^24 for 1/10 percent*/
	ReadReg = BADS_Soc_P_ReadField(hImplDev->hRegister, DS_AGCBI, AGCBVAL);
	h->pStatus->AGF2 = (BADS_7550_P_2560log10(ReadReg)<<1)-40073;  /*BBS will divide by 256 to get dB*/
	
	/*The max gain of AGF is 36 db, scale to 0-72 and clamp the lower limit to 0*/
	ReadReg = (uint32_t)((int32_t) h->pStatus->AGF2 + 9216);
	if ((ReadReg & 0x80000000) !=0)
	{
		ReadReg = 0;
	}

	ulMultA = ReadReg;
	ulMultB = 1000;
	ulDivisor = 18432;
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);
	if ((ulNrmHi != 0))
	{
		BDBG_WRN(("64to32 division is not in a 32 bit unsigned int range"));
	}
	
	/*clamp to be 0 to 1000*/
	if ((ulNrmLo > 1000))
	{
		ulNrmLo = 1000;
	}
	h->pStatus->AGF2_Level = ulNrmLo;

	/****************************************************/
	/*Determine SNR and Average SNR                      */
	/*This is done in the BADS_Soc_P_Get_LockStatus()  */
	/****************************************************/

	/*h->pStatus->SNR = 0;    */ /*BBS will divide by 256 to get dB*/
	/*h->pStatus->SNRAVG = 0;  */ /*BBS will divide by 256 to get dB*/

	/*************************************************************************************************/
	/*Get Counter Values of FEC so BBS can get determine BerPreRS and BerPostRS  and counter values  */
	/*************************************************************************************************/

	ReadReg = BADS_Soc_P_ReadField(hImplDev->hRegister, DS_CERC2, CERCCNTVAL);
	h->pStatus->DS_CERC2 = ReadReg;
	h->pStatus->FEC_Corr_Bits = ReadReg;
	ReadReg = BADS_Soc_P_ReadField(hImplDev->hRegister, DS_NBERC2, NBERCCNTVAL);
	h->pStatus->DS_NBERC2 = ReadReg;
	h->pStatus->FEC_Clean_RS_Blocks = ReadReg;
	ReadReg = BADS_Soc_P_ReadField(hImplDev->hRegister, DS_UERC2, UERCCNTVAL);
	h->pStatus->DS_UERC2 = ReadReg;
	h->pStatus->FEC_UCorr_RS_Blocks = ReadReg;
	ReadReg = BADS_Soc_P_ReadField(hImplDev->hRegister, DS_CBERC2, CBERCCNTVAL);
	h->pStatus->DS_CBERC2 = ReadReg;
	h->pStatus->FEC_Corr_RS_Blocks = ReadReg;
	

	/***************************************/
	/*Determine The Timing Frequency Error*/
	/***************************************/

	h->pStatus->TimingFrequencyError = (uint32_t)BADS_75xx_P_Get_VIDTimingError(hChannel, ARFFactor, FilterFactor, h->pStatus->SampleRate);

	/***************************************/
	/*Determine The Carrier Frequency Error*/
	/***************************************/

	h->pStatus->CarrierFrequencyError = (uint32_t)BADS_75xx_P_Get_CarrierFrequencyError(hChannel, h->pStatus->SampleRate);
	
	/***********************************/
	/*Determine The Carrier Phase Error*/
	/***********************************/
	
	h->pStatus->CarrierPhaseError = (uint32_t)BADS_75xx_P_Get_CarrierPhaseError(hChannel, h->pStatus->SymbolRate);
	
	/***********************************/
	/*Get the FEC Error Counters       */
	/***********************************/
	/*
	ReadReg = BADS_Soc_P_ReadField(hImplDev->hRegister, DS_CBERC1, CBERCCNTVAL);
	h->pStatus->FEC_Corr_RS_Blocks = ReadReg;

	ReadReg = BADS_Soc_P_ReadField(hImplDev->hRegister, DS_UERC1, UERCCNTVAL);
	h->pStatus->FEC_UCorr_RS_Blocks = ReadReg;

	ReadReg = BADS_Soc_P_ReadField(hImplDev->hRegister, DS_NBERC1, NBERCCNTVAL);
	h->pStatus->FEC_Clean_RS_Blocks = ReadReg;
	*/
	/***********************************/
	/*Get the Channel Power			   */
	/***********************************/	
	 h->pStatus->ChannelPower = 0;  /*BBS will divide by 256 to get dB*/

	/**********************************************/
	/*Get the ReAcquisitionCount	              */
	/*This is done in by the autoacquire in the PI*/
	/**********************************************/

	/*the h->pStatus->ReAcquireCount parameter is incremented by BADS_Soc_P_Get_LockStatus()*/
  /*the h->pStatus->ReSyncCount parameter is incremented by BADS_Soc_P_Get_LockStatus()*/
	/*the h->pStatus->pCount parameter is incremented by BADS_Soc_P_Get_LockStatus()*/

	/*************************************************************/
	/*TimngLock, AnnexB 64/256 Qam detect and Spectral Inversion */
	/*This is done in the BADS_Soc_P_Get_LockStatus()           */
	/*************************************************************/	
	/*h->pStatus->TimingStatus = 0;    */
	/*h->pStatus->SpectrumStatus = 0;  */

	/*ADS Status Complete*/
	retCode = BERR_SUCCESS;
	return retCode;
}

