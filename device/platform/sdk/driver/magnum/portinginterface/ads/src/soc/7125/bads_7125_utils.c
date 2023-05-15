/***************************************************************************
 *     Copyright (c) 2005-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bads_7125_utils.c $
 * $brcm_Revision: Hydra_Software_Devel/9 $
 * $brcm_Date: 6/2/11 3:04p $
 *
 * [File Description:]
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/ads/7125/bads_7125_utils.c $
 * 
 * Hydra_Software_Devel/9   6/2/11 3:04p mward
 * SW7125-905:  Don't attempt to power down already powered-down frontend.
 * 
 * Hydra_Software_Devel/8   12/2/10 5:39p mward
 * SW7125-728:  Add Annex C QAM64 and QAM256 support same as Annex A
 * except Nyquist alpha = 12% instead of 15%.
 * 
 * Hydra_Software_Devel/7   10/20/10 4:49p mward
 * SW7125-513:  Adding PowerSave to status.
 * 
 * Hydra_Software_Devel/6   10/15/10 1:40p mward
 * SW7125-513:  Fix reacquisition.  Give only one lock status callback for
 * each change, fix multiple callbacks per lost lock.
 * 
 * Hydra_Software_Devel/5   10/13/10 5:51p mward
 * SW7125-513:  Add power down/up functions.
 * 
 * Hydra_Software_Devel/4   7/16/10 2:14p mward
 * SW7125-513: Add back last 7550 history to show origin version.
 * 
 * Hydra_Software_Devel/3   7/16/10 1:29p mward
 * SW7125-513: 7125 main PLL is in US_TOP, not DS_PLL.
 * 
 * Hydra_Software_Devel/2   7/13/10 2:04p mward
 * SW7125-513:  Update macros for multi-channel.
 * 
 * Hydra_Software_Devel/1   7/12/10 2:54p mward
 * SW7125-513:  Add host control of 7125 internal DS and tuner.  Copy and
 * adapt 7550 implementation.
 * 
 * Hydra_Software_Devel/10   5/18/10 11:58a farshidf
 * SW7550-440: merge to main branch
 * 
 ***************************************************************************/

#include "bstd.h"
#include "bads.h"
#include "bads_priv.h"
#include "bads_7125_priv.h"
#include "bads_7125_acquire.h"
#include "bmth.h"

/*registers needed for the functions in this file*/
#include "bchp_ds_0.h"
#include "bchp_ds_1.h"
#include "bchp_ds_eq_0.h" 
#include "bchp_ds_eq_1.h" 
#include "bchp_ds_tuner_chnz_0.h"
#include "bchp_ds_tuner_chnz_1.h"
#include "bchp_us_top.h"
#include "bchp_int.h"
#include "bchp_vcxo_ctl_misc.h"

BDBG_MODULE(bads_7125_utils);

/***************************************************************************
 * DS_0.power_down_DS0_receiver, DS_1.power_down_DS1_receiver
 ***************************************************************************/
BERR_Code BADS_7125_P_PowerDn(BADS_ChannelHandle hChannel)
{
	BERR_Code retCode;
    BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
    BADS_7125_Handle hImplDev = (BADS_7125_Handle) hDev->pImpl;
    BADS_7125_ChannelHandle h = (BADS_7125_ChannelHandle)(hChannel->pImpl);
    unsigned int chn = h->chnNo;

    BREG_Handle hReg = hImplDev->hRegister;
    uint32_t reg;

#if (BCHP_VER>=BCHP_VER_C0)
	if (!BADS_7125_P_ReadField(hReg, VCXO_CTL_MISC_DOCSIS_CTRL, POWERDOWN, 0))
	{ /* if whole frontend is not already powered off */
		if (chn==0)
		{
			/*DS_0.power_down_DS0_receiver*/
			reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_CLK_DS0);
			reg &= ~(
					BCHP_MASK(US_TOP_PLL_CLK_DS0, PWRUP_CMLBUF) |
					BCHP_MASK(US_TOP_PLL_CLK_DS0, PWRUP) |
					BCHP_MASK(US_TOP_PLL_CLK_DS0, EN_CLK)
					);
			reg |=
					BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS0, PWRUP_CMLBUF,  0 ) |
					BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS0, PWRUP,  0 ) |
					BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS0, EN_CLK,  0 )
			;
			BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_DS0, reg);
		
			reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
			reg &= ~(
					BCHP_MASK(INT_CLKCONTROL, DS0_CLK_EN)
					);
			reg |=
					BCHP_FIELD_DATA(INT_CLKCONTROL, DS0_CLK_EN,  0 )
			;
			BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);
		}
		else
		{
			/*power_down_DS1_receiver*/
			reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_CLK_DS1);
			reg &= ~(
					BCHP_MASK(US_TOP_PLL_CLK_DS1, PWRUP_CMLBUF) |
					BCHP_MASK(US_TOP_PLL_CLK_DS1, PWRUP) |
					BCHP_MASK(US_TOP_PLL_CLK_DS1, EN_CLK)
					);
			reg |=
					BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS1, PWRUP_CMLBUF,  0 ) |
					BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS1, PWRUP,  0 ) |
					BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS1, EN_CLK,  0 )
			;
			BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_DS1, reg);
		
			reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
			reg &= ~(
					BCHP_MASK(INT_CLKCONTROL, DS1_CLK_EN)
					);
			reg |=
					BCHP_FIELD_DATA(INT_CLKCONTROL, DS1_CLK_EN,  0 )
			;
			BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);
		}
		h->pAcquireParam->AcqState = BADS_Acq_Inactive;
		h->pAcquireParam->DoneFirstTimeFlag = BADS_OffOn_eOff;
		h->pStatus->PowerSave = true;
	}
#endif

  retCode = BERR_SUCCESS;
  return retCode;
}

/***************************************************************************
 * DS_0.power_up_DS0_receiver, DS_1.power_up_DS1_receiver
 ***************************************************************************/
BERR_Code BADS_7125_P_PowerUp(BADS_ChannelHandle hChannel)
{
    BERR_Code retCode;
    BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
    BADS_7125_Handle hImplDev = (BADS_7125_Handle) hDev->pImpl;
    BADS_7125_ChannelHandle h = (BADS_7125_ChannelHandle)(hChannel->pImpl);
    unsigned int chn = h->chnNo;

    BREG_Handle hReg = hImplDev->hRegister;
    uint32_t reg;

    BSTD_UNUSED(hChannel);

#if (BCHP_VER>=BCHP_VER_C0)
    if (chn==0)
    {
		/*DS_0.power_up_DS0_receiver*/
		reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_CLK_DS0);
		reg &= ~(
				BCHP_MASK(US_TOP_PLL_CLK_DS0, PWRUP_CMLBUF) |
				BCHP_MASK(US_TOP_PLL_CLK_DS0, PWRUP) |
				BCHP_MASK(US_TOP_PLL_CLK_DS0, EN_CLK)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS0, PWRUP_CMLBUF,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS0, PWRUP,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS0, EN_CLK,  1 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_DS0, reg);
	
		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, DS0_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, DS0_CLK_EN,  1 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);    
    }
    else
    {
		/*DS_1.power_up_DS1_receiver*/
		reg = BREG_Read32(hReg, BCHP_US_TOP_PLL_CLK_DS1);
		reg &= ~(
				BCHP_MASK(US_TOP_PLL_CLK_DS1, PWRUP_CMLBUF) |
				BCHP_MASK(US_TOP_PLL_CLK_DS1, PWRUP) |
				BCHP_MASK(US_TOP_PLL_CLK_DS1, EN_CLK)
				);
		reg |=
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS1, PWRUP_CMLBUF,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS1, PWRUP,  1 ) |
				BCHP_FIELD_DATA(US_TOP_PLL_CLK_DS1, EN_CLK,  1 )
		;
		BREG_Write32(hReg, BCHP_US_TOP_PLL_CLK_DS1, reg);
	
		reg = BREG_Read32(hReg, BCHP_INT_CLKCONTROL);
		reg &= ~(
				BCHP_MASK(INT_CLKCONTROL, DS1_CLK_EN)
				);
		reg |=
				BCHP_FIELD_DATA(INT_CLKCONTROL, DS1_CLK_EN,  1 )
		;
		BREG_Write32(hReg, BCHP_INT_CLKCONTROL, reg);    
    }
    h->pStatus->PowerSave = false;
#endif

  retCode = BERR_SUCCESS;
  return retCode;
}

/************************************************************************************************
 * BADS_7125_P_Range_Check()	 This routine checks that parameters from the PI/BBS are in range
 ************************************************************************************************/
void BADS_7125_P_Range_Check(BADS_7125_ChannelHandle h)
{
	/*The structure passed from the PI is c combined QAM/FEC data type, it is simpler fo rthe eacquisition functions to have them seperate*/
	/*Map the h->pAcquireParam->ModulationType to h->pAcquireParam->Annex and h->pAcquireParam->QAM*/
	switch (h->pAcquireParam->ModulationType)
	{
		case BADS_ModulationType_eAnnexAQam16   : 
			h->pAcquireParam->Annex = BADS_Annex_eAnnexA; 
			h->pAcquireParam->QAM = BADS_QAM_eQam16;   
			break;
		case BADS_ModulationType_eAnnexAQam32   : 
			h->pAcquireParam->Annex = BADS_Annex_eAnnexA; 
			h->pAcquireParam->QAM = BADS_QAM_eQam32;   
			break;
		case BADS_ModulationType_eAnnexAQam64   : 
			h->pAcquireParam->Annex = BADS_Annex_eAnnexA; 
			h->pAcquireParam->QAM = BADS_QAM_eQam64;   
			break;
		case BADS_ModulationType_eAnnexAQam128  : 
			h->pAcquireParam->Annex = BADS_Annex_eAnnexA; 
			h->pAcquireParam->QAM = BADS_QAM_eQam128;  
			break;
		case BADS_ModulationType_eAnnexAQam256  : 
			h->pAcquireParam->Annex = BADS_Annex_eAnnexA; 
			h->pAcquireParam->QAM = BADS_QAM_eQam256;  
			break;
		case BADS_ModulationType_eAnnexAQam512  : 
			h->pAcquireParam->Annex = BADS_Annex_eAnnexA; 
			h->pAcquireParam->QAM = BADS_QAM_eQam512;  
			break;
		case BADS_ModulationType_eAnnexAQam1024 : 
			h->pAcquireParam->Annex = BADS_Annex_eAnnexA; 
			h->pAcquireParam->QAM = BADS_QAM_eQam1024; 
			break;
		case BADS_ModulationType_eAnnexAQam2048 :          
			BDBG_WRN(("WARNING!!!!! Unsupported ModulationType from PI, received value is %d",h->pAcquireParam->ModulationType));			  
			break;
		case BADS_ModulationType_eAnnexAQam4096 :          
			BDBG_WRN(("WARNING!!!!! Unsupported ModulationType from PI, received value is %d",h->pAcquireParam->ModulationType));			  
			break;
		case BADS_ModulationType_eAnnexBQam16   :          
			BDBG_WRN(("WARNING!!!!! Unsupported ModulationType from PI, received value is %d",h->pAcquireParam->ModulationType));				  
			break;
		case BADS_ModulationType_eAnnexBQam32   :          
			BDBG_WRN(("WARNING!!!!! Unsupported ModulationType from PI, received value is %d",h->pAcquireParam->ModulationType));			  
			break;
		case BADS_ModulationType_eAnnexBQam64   : 
			h->pAcquireParam->Annex = BADS_Annex_eAnnexB; 
			h->pAcquireParam->QAM = BADS_QAM_eQam64;   
			break;
		case BADS_ModulationType_eAnnexBQam128  :          
			BDBG_WRN(("WARNING!!!!! Unsupported ModulationType from PI, received value is %d",h->pAcquireParam->ModulationType));			  
			break;
		case BADS_ModulationType_eAnnexBQam256  : 
			h->pAcquireParam->Annex = BADS_Annex_eAnnexB; 
			h->pAcquireParam->QAM = BADS_QAM_eQam256; 
			break;
		case BADS_ModulationType_eAnnexBQam512  :          
			BDBG_WRN(("WARNING!!!!! Unsupported ModulationType from PI, received value is %d",h->pAcquireParam->ModulationType));			  
			break;
		case BADS_ModulationType_eAnnexBQam1024 : 
			h->pAcquireParam->Annex = BADS_Annex_eAnnexB; 
			h->pAcquireParam->QAM = BADS_QAM_eQam1024; 
			break;
		case BADS_ModulationType_eAnnexBQam2048 :          
			BDBG_WRN(("WARNING!!!!! Unsupported ModulationType from PI, received value is %d",h->pAcquireParam->ModulationType));			  
			break;
		case BADS_ModulationType_eAnnexBQam4096 :          
			BDBG_WRN(("WARNING!!!!! Unsupported ModulationType from PI, received value is %d",h->pAcquireParam->ModulationType));			  
			break;
        case BADS_ModulationType_eAnnexCQam16   :          
            BDBG_WRN(("WARNING!!!!! Unsupported ModulationType from PI, received value is %d",h->pAcquireParam->ModulationType));				  
            break;
        case BADS_ModulationType_eAnnexCQam32   :          
            BDBG_WRN(("WARNING!!!!! Unsupported ModulationType from PI, received value is %d",h->pAcquireParam->ModulationType));			  
            break;
        case BADS_ModulationType_eAnnexCQam64   : 
            h->pAcquireParam->Annex = BADS_Annex_eAnnexC; 
            h->pAcquireParam->QAM = BADS_QAM_eQam64;   
            break;
        case BADS_ModulationType_eAnnexCQam128  :          
            BDBG_WRN(("WARNING!!!!! Unsupported ModulationType from PI, received value is %d",h->pAcquireParam->ModulationType));			  
            break;
        case BADS_ModulationType_eAnnexCQam256  : 
            h->pAcquireParam->Annex = BADS_Annex_eAnnexC; 
            h->pAcquireParam->QAM = BADS_QAM_eQam256; 
            break;
        case BADS_ModulationType_eAnnexCQam512  :          
            BDBG_WRN(("WARNING!!!!! Unsupported ModulationType from PI, received value is %d",h->pAcquireParam->ModulationType));			  
            break;
        case BADS_ModulationType_eAnnexCQam1024 : 
            BDBG_WRN(("WARNING!!!!! Unsupported ModulationType from PI, received value is %d",h->pAcquireParam->ModulationType));			  
            break;
        case BADS_ModulationType_eAnnexCQam2048 :          
            BDBG_WRN(("WARNING!!!!! Unsupported ModulationType from PI, received value is %d",h->pAcquireParam->ModulationType));			  
            break;
        case BADS_ModulationType_eAnnexCQam4096 :          
            BDBG_WRN(("WARNING!!!!! Unsupported ModulationType from PI, received value is %d",h->pAcquireParam->ModulationType));			  
            break;
		default : 
			BDBG_ERR(("ERROR!!!!! Undefined ModulationType from PI, value received is %d",h->pAcquireParam->ModulationType));
	}

	/*Display the received Parameters from the PI, the BDBG_MSG messages are supressed at compile time*/
	switch(h->pAcquireParam->ModulationType)
	{
		case BADS_ModulationType_eAnnexAQam16 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexAQam16"));    
			break;
		case BADS_ModulationType_eAnnexAQam32 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexAQam32"));    
			break;
		case BADS_ModulationType_eAnnexAQam64 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexAQam64"));    
			break;
		case BADS_ModulationType_eAnnexAQam128 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexAQam128"));    
			break;
		case BADS_ModulationType_eAnnexAQam256 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexAQam256"));    
			break;
		case BADS_ModulationType_eAnnexAQam512 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexAQam512"));    
			break;
		case BADS_ModulationType_eAnnexAQam1024 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexAQam1024"));    
			break;
		case BADS_ModulationType_eAnnexAQam2048 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexAQam2048"));    
			break;
		case BADS_ModulationType_eAnnexAQam4096 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexAQam4096"));    
			break;
		case BADS_ModulationType_eAnnexBQam16 :
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexBQam16"));    
			break;
		case BADS_ModulationType_eAnnexBQam32 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexBQam32"));    
			break;
		case BADS_ModulationType_eAnnexBQam64 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexBQam64"));    
			break;
		case BADS_ModulationType_eAnnexBQam128 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexBQam128"));    
			break;
		case BADS_ModulationType_eAnnexBQam256 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexBQam256"));    
			break;
		case BADS_ModulationType_eAnnexBQam512 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexBQam512"));    
			break;
		case BADS_ModulationType_eAnnexBQam1024 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexBQam1024"));    
			break;
		case BADS_ModulationType_eAnnexBQam2048 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexBQam2048"));    
			break;
		case BADS_ModulationType_eAnnexBQam4096 :   
			BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexBQam4096"));    
			break;
        case BADS_ModulationType_eAnnexCQam16 :
            BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexCQam16"));    
            break;
        case BADS_ModulationType_eAnnexCQam32 :   
            BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexCQam32"));    
            break;
        case BADS_ModulationType_eAnnexCQam64 :   
            BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexCQam64"));    
            break;
        case BADS_ModulationType_eAnnexCQam128 :   
            BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexCQam128"));    
            break;
        case BADS_ModulationType_eAnnexCQam256 :   
            BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexCQam256"));    
            break;
        case BADS_ModulationType_eAnnexCQam512 :   
            BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexCQam512"));    
            break;
        case BADS_ModulationType_eAnnexCQam1024 :   
            BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexCQam1024"));    
            break;
        case BADS_ModulationType_eAnnexCQam2048 :   
            BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexCQam2048"));    
            break;
        case BADS_ModulationType_eAnnexCQam4096 :   
            BDBG_MSG(("h->pAcquireParam->ModulationType = BADS_ModulationType_eAnnexCQam4096"));    
            break;
		default : 
			BDBG_ERR(("ERROR!!! UNDEFINED ModulationType, value received is %d",h->pAcquireParam->ModulationType));
	}

	switch(h->pAcquireParam->InvertSpectrum)
	{
		case BADS_InvertSpectrum_eNoInverted: 
			BDBG_MSG(("h->pAcquireParam->InvertSpectrum Set To Non-Inverted"));   
			break;
		case BADS_InvertSpectrum_eInverted:   
			BDBG_MSG(("h->pAcquireParam->InvertSpectrum Set To Inverted"));		
			break;
		default : 
			BDBG_ERR(("ERROR!!! UNDEFINED h->pAcquireParam->InvertSpectrum, value received is %d",h->pAcquireParam->InvertSpectrum));
	}

	switch(h->pAcquireParam->spectrum)
	{
		case BADS_SpectrumMode_eAuto: 
			BDBG_MSG(("h->pAcquireParam->spectrum Set To Auto"));			
			break;
		case BADS_SpectrumMode_eManual:   
			BDBG_MSG(("h->pAcquireParam->spectrum Set To Manual"));		
			break;
		default : 
			BDBG_ERR(("ERROR!!! UNDEFINED h->pAcquireParam->spectrum, value received is %d",h->pAcquireParam->spectrum));
	}

	if (h->pAcquireParam->IfFrequency > 0)
	{
		BDBG_MSG(("WARNING!!!! IfFrequency Not Set for Internal Tuner Mode, h->pAcquireParam->IfFrequency =  %d Hz ",h->pAcquireParam->IfFrequency)); 
	}
	else
	{
		BDBG_MSG(("h->pAcquireParam->IfFrequency =  %d Hz",h->pAcquireParam->IfFrequency)); 
	}

	switch(h->pAcquireParam->EnableDpm)
	{
		case BADS_DpmMode_Disabled : 
			BDBG_MSG(("h->pAcquireParam->EnableDpm Set to Dpm Disabled"));   
			break;
		case BADS_DpmMode_Enabled :  
			BDBG_MSG(("h->pAcquireParam->EnableDpm Set to Dpm Enabled"));   
			break;
		default : 
			BDBG_ERR(("ERROR!!! UNDEFINED h->pAcquireParam->EnableDpm, value received is %d",h->pAcquireParam->EnableDpm));
	}

	/*Only check the Symbol Rate in Annex A Mode */
	if (h->pAcquireParam->Annex == BADS_Annex_eAnnexA)
	{
		if (h->pAcquireParam->SymbolRate == 0)
		{
			BDBG_ERR(("ERROR!!!! h->pAcquireParam->SymbolRate =  %d ",h->pAcquireParam->SymbolRate));
		
		}
		else if ((h->pAcquireParam->SymbolRate < MIN_BAUD_RATE) || (h->pAcquireParam->SymbolRate > MAX_BAUD_RATE))
		{
			 BDBG_WRN(("WARNING!!!! Unsupported h->pAcquireParam->SymbolRate =  %d ",h->pAcquireParam->SymbolRate)); 
		}
		else
		{
			BDBG_MSG(("h->pAcquireParam->SymbolRate =  %d ",h->pAcquireParam->SymbolRate)); 
		}
	}

	switch(h->pAcquireParam->AutoAcquireMode)
	{
		case 0 : 
			BDBG_MSG(("h->pAcquireParam->AutoAcquireMode Set to Off"));   
			break;
		case 1 :  
			BDBG_MSG(("h->pAcquireParam->AutoAcquireMode Set to On"));   
			break;
		default : 
			BDBG_ERR(("ERROR!!! UNDEFINED h->pAcquireParam->AutoAcquireMode, value received is %d",h->pAcquireParam->AutoAcquireMode));
	}

	switch(h->pAcquireParam->fastAcquire)
	{
		case BADS_eSlowAcquire: 
			BDBG_MSG(("h->pAcquireParam->FastAcquire Set To Slow Acquisition, Check Timing and Qam64/256 if AnnexB"));   
			break;
		case BADS_eFastAcquire:   
			BDBG_MSG(("h->pAcquireParam->FastAcquire Set To Fast Acquisition, Do Not Check Timing or Qam64/256 if AnnexB"));		
			break;
		default : 
			BDBG_ERR(("ERROR!!! UNDEFINED h->pAcquireParam->BADS_eFastAcquire, value received is %d",h->pAcquireParam->fastAcquire));
	}

	/*Display the predefined parameteres set in acquire.h*/
	switch((BADS_OffOn_t)h->pAcquireParam->BBS_Burst_mode)
	{
		case BADS_OffOn_eOff : 
			BDBG_MSG(("h->pAcquireParam->BBS_Burst_mode Set to Burst_mode Off"));  
			break;
		case BADS_OffOn_eOn :  
			BDBG_MSG(("h->pAcquireParam->BBS_Burst_mode Set to Burst_mode On"));   
			break;
		default : 
			BDBG_ERR(("ERROR!!! UNDEFINED h->pAcquireParam->BBS_Burst_mode, value received is %d",(BADS_OffOn_t)h->pAcquireParam->BBS_Burst_mode));
	}
	
	/*The CWC frequencies are not range checked*/
	switch((BADS_OffOn_t)h->pAcquireParam->BBS_CWC_Enable)
	{
		case BADS_OffOn_eOff : 
			BDBG_MSG(("h->pAcquireParam->BBS_CWC_Enable Set to CWC Disabled"));   
			break;
		case BADS_OffOn_eOn : 
			BDBG_MSG(("h->pAcquireParam->BBS_CWC_Enable Set to CWC Enabled"));   
			BDBG_MSG(("h->pAcquireParam->BBS_CWC1_Freq = %d Hz",(int32_t)h->pAcquireParam->BBS_CWC1_Freq));  
			BDBG_MSG(("h->pAcquireParam->BBS_CWC2_Freq = %d Hz",(int32_t)h->pAcquireParam->BBS_CWC2_Freq));  
			BDBG_MSG(("h->pAcquireParam->BBS_CWC3_Freq = %d Hz",(int32_t)h->pAcquireParam->BBS_CWC3_Freq));  
			BDBG_MSG(("h->pAcquireParam->BBS_CWC4_Freq = %d Hz",(int32_t)h->pAcquireParam->BBS_CWC4_Freq));  
			break;
		default : 
			BDBG_ERR(("ERROR!!! UNDEFINED h->pAcquireParam->BBS_CWC_Enable, value received is %d",(BADS_OffOn_t)h->pAcquireParam->BBS_CWC_Enable));
	}	

	switch((BADS_OffOn_t)h->pAcquireParam->BBS_IQIMB_on)
	{
		case BADS_OffOn_eOff : 
			BDBG_MSG(("h->pAcquireParam->BBS_IQIMB_on Set to Off")); 
			break;
		case BADS_OffOn_eOn : 
			BDBG_MSG(("h->pAcquireParam->BBS_IQIMB_on Set to  On"));   
			break;
		default : 
			BDBG_ERR(("ERROR!!! UNDEFINED h->pAcquireParam->BBS_IQIMB_on, value received is %d",(BADS_OffOn_t)h->pAcquireParam->BBS_IQIMB_on));
	}
	
	switch((BADS_OffOn_t)h->pAcquireParam->BBS_CFL_on)
	{
		case BADS_OffOn_eOff : 
			BDBG_MSG(("h->pAcquireParam->BBS_CFL_on Set to Off")); 
			break;
		case BADS_OffOn_eOn : 
			BDBG_MSG(("h->pAcquireParam->BBS_CFL_on Set to On"));   
			break;
		default : 
			BDBG_ERR(("ERROR!!! UNDEFINED h->pAcquireParam->BBS_CFL_on, value received is %d",(BADS_OffOn_t)h->pAcquireParam->BBS_CFL_on));
	}
	
	switch((BADS_SweepDirection_t)h->pAcquireParam->BBS_sweep_dir)
	{
		case BADS_SweepDirection_eNeg2Pos : 
			BDBG_MSG(("h->pAcquireParam->BBS_sweep_dir Set to Sweep up from neg freq"));   
			break;
		case BADS_SweepDirection_ePos2Neg : 
			BDBG_MSG(("h->pAcquireParam->BBS_sweep_dir Set to Sweep down from pos freq")); 
			break;
		default : 
			BDBG_ERR(("ERROR!!! UNDEFINED h->pAcquireParam->BBS_sweep_dir, value received is %d",(BADS_SweepDirection_t)h->pAcquireParam->BBS_sweep_dir));
	}
	
	
	switch((BADS_OffOn_t)h->pAcquireParam->BBS_DDAGC_on)
	{
		case BADS_OffOn_eOff : 
			BDBG_MSG(("h->pAcquireParam->BBS_DDAGC_on Set to Off")); 
			break;
		case BADS_OffOn_eOn : 
			BDBG_MSG(("h->pAcquireParam->BBS_DDAGC_on Set to On"));   
			break;
		default : 
			BDBG_ERR(("ERROR!!! UNDEFINED h->pAcquireParam->BBS_DDAGC_o, value received is %d",(BADS_OffOn_t)h->pAcquireParam->BBS_DDAGC_on));
	}

	switch((BADS_OffOn_t)h->pAcquireParam->BBS_IMC_on)
	{
		case BADS_OffOn_eOff : 
			BDBG_MSG(("h->pAcquireParam->BBS_IMC_on Set to Off")); 
			break;
		case BADS_OffOn_eOn : 
			BDBG_MSG(("h->pAcquireParam->BBS_IMC_on Set to On"));   
			break;
		default : 
			BDBG_ERR(("ERROR!!! UNDEFINED IMC value received is %d",(BADS_OffOn_t)h->pAcquireParam->BBS_IMC_on));
	}
	
	switch((BADS_OffOn_t)h->pAcquireParam->BBS_IQPHS_on)
	{
		case BADS_OffOn_eOff : 
			BDBG_MSG(("h->pAcquireParam->BBS_IQPHS_on Set to Off")); 
			break;
		case BADS_OffOn_eOn : 
			BDBG_MSG(("h->pAcquireParam->BBS_IQPHS_on Set to On"));   
			break;
		default : 
			BDBG_ERR(("ERROR!!! UNDEFINED h->pAcquireParam->BBS_IQPHS_on, value received is %d",(BADS_OffOn_t)h->pAcquireParam->BBS_IQPHS_on));
	}	

	BDBG_MSG(("BADS_7125_P_Range_Check() Complete\n"));
}

/*****************************************************************************************************************
* BADS_7125_P_ADS_Init_Buffer()	 This routine loads the BADS_7125_P_AcquireParams_t structure with default values
* The only values not initialized are the ones passed from the PI											     
******************************************************************************************************************/
void BADS_7125_P_ADS_Init_Buffer(BADS_7125_ChannelHandle h)
{
	/*Initialize BADS_7125_P_AcquireParams_t Structure*/
	/*PI parameters are not initialized and are never written to*/
	/*these parameters are used locally and for BBS to sent parameters into the ADS functions*/
	h->pAcquireParam->BBS_AcqWord0 =			(uint32_t)BBS_ACQWORD0;		/*Beginning of 17 words of memory for BBS*/ 
	h->pAcquireParam->BBS_AcqWord1 =			(uint32_t)BBS_ACQWORD1;
	h->pAcquireParam->BBS_AcqWord2 =			(uint32_t)BBS_ACQWORD2;
	h->pAcquireParam->BBS_AcqWord3 =			(uint32_t)BBS_ACQWORD3;
	h->pAcquireParam->BBS_Rerun_Init =			(uint32_t)RERUN_INIT;
	h->pAcquireParam->BBS_Burst_mode =			(uint32_t)BURST_MODE;
	h->pAcquireParam->BBS_CWC_Enable =			(uint32_t)CCW_ENABLE;
	h->pAcquireParam->BBS_CWC1_Freq =			(uint32_t)CCW1_FREQUENCY;
	h->pAcquireParam->BBS_CWC2_Freq =			(uint32_t)CCW2_FREQUENCY;
	h->pAcquireParam->BBS_CWC3_Freq =			(uint32_t)CCW3_FREQUENCY;
	h->pAcquireParam->BBS_CWC4_Freq =			(uint32_t)CCW4_FREQUENCY;
	h->pAcquireParam->BBS_IQIMB_on =			(uint32_t)IQ_IMB_ON;
	h->pAcquireParam->BBS_CFL_on =				(uint32_t)CFL_ON;
	h->pAcquireParam->BBS_sweep_dir =			(uint32_t)SWEEP_DIRECTION;
	h->pAcquireParam->BBS_DDAGC_on =			(uint32_t)DDAGC_ON;
	h->pAcquireParam->BBS_IMC_on =				(uint32_t)IMC_ON;
	h->pAcquireParam->BBS_IQPHS_on =			(uint32_t)IQPHS_ON;			/*End of 17 words of memory for BBS*/
	h->pAcquireParam->DoneLoadFlag =			BADS_OffOn_eOn;
	h->pAcquireParam->DoneFirstTimeFlag	=		BADS_OffOn_eOff;
	h->pAcquireParam->Annex	=					BADS_Annex_eAnnexB;
	h->pAcquireParam->QAM =						BADS_QAM_eQam64;
	h->pAcquireParam->TimingCheck =         	BADS_Timing_eNotChecked;
	h->pAcquireParam->FECSpectrum =         	BADS_FECSpectrum_eNotInverted;
	h->pAcquireParam->Old_CBERC1 =				0;
	h->pAcquireParam->Old_UERC1 =				0;
	h->pAcquireParam->Old_NBERC1 =				0;

	BDBG_MSG(("BADS_7125_P_ADS_Init_Buffer() Complete\n"));
}

/***************************************************************************
* BADS_7125_P_2560log10() This function calculated 256*10log10(x)
***************************************************************************/
uint32_t BADS_7125_P_2560log10(uint32_t x)
{
  int32_t  x1;
  int32_t  x2;
  int32_t  x3;
  int32_t  x4;

  if (x == 0)
    return 0;

  x1 = 30;
  while (!((x >> x1) & 1))
    x1 = x1-1;
  x1 = x1+1;

  x2 = (int32_t)((x << 8) >> x1);
  x3 = -24381739 + x2*(62348 + (x2 << 7));
  x4 = 5907991 + (x2 << 16);

  return (unsigned)((770*(x3/(x4 >> 8) + (x1 << 8))) >> 8);
}



/***********************************************************************************
*BADS_7125_P_Set_TimingFrequency() determines the value to program in the VID	
************************************************************************************/
void BADS_7125_P_Set_TimingFrequency(BADS_ChannelHandle hChannel, uint32_t SymbolRate, uint32_t SampleRate, uint8_t FilterFactor, uint8_t ARFFactor)
{
	BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
	BADS_7125_Handle hImplDev = (BADS_7125_Handle) hDev->pImpl;
	BADS_7125_ChannelHandle h = (BADS_7125_ChannelHandle)(hChannel->pImpl);
	unsigned int chn = h->chnNo;
	
	/*Local Variables used for multipliers and dividers in math calculations*/
	uint32_t	ulMultA, ulMultB, ulNrmHi, ulNrmLo, ulDivisor;

	/****************************************************************************************************/
	/*Calculate the Timing Loop Frequency Control Word a 24 bit 2's complement number*/
	/*The NCO value depends on the Symbol Rate, SampleRate, ARF, FilterFactor, and Timing Loop NCO value*/
	/*TLFOS = 2^25*FilterFactor*SymbolRate/VID_SampleRate: VID_SampleRate=SampleRate/(ARF*FilterFactor)*/
	/****************************************************************************************************/

	ulMultA = (POWER2_25*FilterFactor*ARFFactor);
	ulMultB = SymbolRate;
	ulDivisor = SampleRate;
	if (ulDivisor == 0)
	{
		BDBG_ERR(("divide by 0 in BADS_7125_P_Set_TimingFrequency() while calculating Timing Loop Frequency Control Word"));
	}
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);
	if ((ulNrmHi != 0) || (ulNrmLo > 0x007FFFFF))
	{
		BDBG_WRN(("64to32 division is not in a 24 twos complement range while calculating Timing Loop Frequency Control Word"));
	}

	/*Program the Timing Loop Frequency Control Word*/
	BADS_7125_P_Write32(hImplDev->hRegister, BCHP_DS_0_TLFOS, ulNrmLo, chn);
}

/***********************************************************************************
*BADS_7125_P_Set_IfFrequency() determines the value to program in the front mixer	
************************************************************************************/
void BADS_7125_P_Set_IfFrequency(BADS_ChannelHandle hChannel, uint32_t IfFrequency, uint32_t SampleRate)
{
	BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
	BADS_7125_Handle hImplDev = (BADS_7125_Handle) hDev->pImpl;
	BADS_7125_ChannelHandle h = (BADS_7125_ChannelHandle)(hChannel->pImpl);
	unsigned int chn = h->chnNo;
	
	/*Local Variables*/
	uint8_t  RegIsNegative;
	uint32_t ReadReg;

	/*Local Variables used for multipliers and dividers in math calculations*/
	uint32_t	ulMultA, ulMultB, ulNrmHi, ulNrmLo, ulDivisor;
	
	/****************************************************************************************************/
	/*Calculate the Carrier Loop Frequency Control Word a 16 bit 2's complement number*/
	/*The NCO value depends on the SampleRate, IF Frequency a 2's complement number passed as uint32_t*/
	/*CFLFOS = 2^16*IF/SampleRate*/
	/****************************************************************************************************/
	ReadReg = IfFrequency;

	/*Detect if it is negative*/
	RegIsNegative = 0;
	if ((ReadReg & 0x80000000) != 0)
	{
		RegIsNegative = 1;
		if (ReadReg == 0x80000000)
		{
			ReadReg = 0x80000001; /*clamp value*/
		}
		ReadReg = ReadReg ^ 0xFFFFFFFF;	/*invert all bits*/
		ReadReg = ReadReg + 1;			/* add 1 */
	}
	
	ulMultA = POWER2_16;
	ulMultB = ReadReg;
	ulDivisor = SampleRate;
	if (ulDivisor == 0)
	{
		BDBG_ERR(("divide by 0 in BADS_7125_P_Set_IfFrequency() while calculating Carrier Loop Frequency Control Word"));
	}
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);
	if ((ulNrmHi != 0) || (ulNrmLo > 0x00007FFF))
	{
		BDBG_WRN(("64to32 division is not in a 15 bit unsigned int range"));
	}

	if (RegIsNegative == 1)
	{
		ulNrmLo = ulNrmLo ^ 0xFFFFFFFF;	/*invert all bits*/
		ulNrmLo = ulNrmLo + 1;			/* add 1 */
	}
	
	/*Mask to 16 bits*/
	ulNrmLo = ulNrmLo & 0x0000FFFF;

	/*Program the  Carrier Loop Frequency Control Word*/
	BADS_7125_P_WriteField(hImplDev->hRegister, DS_0_CFLFOS, CFLOFFSET, ulNrmLo, chn);
	/**********************************************************************/
	/**********************************************************************/
	/**********************************************************************/
}

/***********************************************************************************
*BADS_7125_P_Get_SampleRate() determines the Sample Rate programmed in the ADS core	
************************************************************************************/
uint32_t BADS_7125_P_Get_SampleRate(BADS_ChannelHandle hChannel)	
{
	BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
	BADS_7125_Handle hImplDev = (BADS_7125_Handle) hDev->pImpl;

	/*used to calculate sample rate and symbol rate*/
	uint32_t SampleRate, P2Div, P1Div, NDivInt, NDivFrac, MxDiv;     /*To Calculate Sample Rate*/

	/*SampleRate*/
	/*MAIN_PLL_REFERENCE_FREQUENCY is defined in the acquire.h*/
	/* F = (p2div/p1div)*(ndiv_int+ndiv_frac)/mxdiv*54MHz = (1/2)*(78+0)/18*MAIN_PLL_REFERENCE_FREQUENCY = 117MHz*/
	/* The ADC is Ping Pong so 117 MHz is 58.5 MSPS I and Q*/

	NDivInt = BADS_7125_P_ReadField(hImplDev->hRegister, US_TOP_PLL_DIV1, NDIV_INT, 0);
	P2Div = 1;
 	P1Div = BADS_7125_P_ReadField(hImplDev->hRegister, US_TOP_PLL_DIV1, P1DIV, 0);
 	NDivFrac = BADS_7125_P_ReadField(hImplDev->hRegister, US_TOP_PLL_DIV2, NDIV_FRAC, 0);
 	MxDiv = BADS_7125_P_ReadField(hImplDev->hRegister, US_TOP_PLL_CLK_DS_AFE0,  DIV, 0);

	/*Check for 0 values in divisor*/
	if ((P1Div == 0) || (MxDiv == 0))
	{
		BDBG_ERR(("ERROR!!! divide by 0 in BADS_7125_P_Get_SampleRate()"));
	}
	SampleRate = (MAIN_PLL_REFERENCE_FREQUENCY*P2Div*(NDivInt+NDivFrac))/(2*P1Div*MxDiv); /*divide by 2 is from ping pong*/

	BDBG_MSG(("%s() returns %u",__FUNCTION__,SampleRate));
	return SampleRate;
}

/****************************************************************************************
*BADS_7125_P_Get_ARFFactor() determines the decimation from the ARF in the ADS core	
*This is used to program the VID and to find out how the VID is programmed                                           
****************************************************************************************/

uint8_t BADS_7125_P_Get_ARFFactor(BADS_ChannelHandle hChannel)
{
	BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
	BADS_7125_Handle hImplDev = (BADS_7125_Handle) hDev->pImpl;
	BADS_7125_ChannelHandle h = (BADS_7125_ChannelHandle)(hChannel->pImpl);
	unsigned int chn = h->chnNo;
	
	uint8_t ARFFactor;
	
	if (BADS_7125_P_ReadField(hImplDev->hRegister, DS_0_GBL, EN_ARF, chn) == 1) /*Arf is enabled*/
	{
		ARFFactor = 2; /*divide by 2 due to the ARF enabled*/
	}
	else
	{
		ARFFactor = 1; /*divide by 1 due to the ARF bypassed*/
	}
	return ARFFactor;
}

/****************************************************************************************
*BADS_7125_P_Get_FilterFactor() determines the Filter Factor programmed in the ADS core	
*This is used to program the VID and to find out how the VID is programmed                                           
****************************************************************************************/
uint8_t BADS_7125_P_Get_FilterFactor(BADS_ChannelHandle hChannel)	
{
	BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
	BADS_7125_Handle hImplDev = (BADS_7125_Handle) hDev->pImpl;
	BADS_7125_ChannelHandle h = (BADS_7125_ChannelHandle)(hChannel->pImpl);
	unsigned int chn = h->chnNo;

	
	uint8_t FilterFactor, BypA, BypB;
	
	/*SymbolRate*/
	/*Filter Factor Calculation*/
	/*Filter		BYPHA		BYPHB		BYPHC		    HC_SELECT	FilterFactor*/
	/*Half-Band 		1      		1      		0     		10 or 11          	1	*/
	/*Third-Band 		1      		1  	 		0        		01             	1	*/
	/*Quarter-Band 		1      		1      		0        		00             	1	*/
	/*Sixth-Band 		1      		0      		0        		01            	2	*/
	/*Eighth-Band 		1      		0      		0        		00             	2	*/
	/*Twelfth-Band 		0      		0      		0        		01             	4	*/
	/*Sixteenth-Band 	0     		0      		0        		00             	4	*/
	BypA = BADS_7125_P_ReadField(hImplDev->hRegister, DS_TUNER_CHNZ_0_FE, BYPHA, chn);
	BypB = BADS_7125_P_ReadField(hImplDev->hRegister, DS_TUNER_CHNZ_0_FE, BYPHB, chn);	
	switch (BypA + BypB)
	{
		case 0 : FilterFactor = 4; break;
		case 1 : FilterFactor = 2; break;
		case 2 : FilterFactor = 1; break;
		default: FilterFactor = 1; BDBG_WRN(("WARNING!!! Unsupported FilterFactor in BADS_7125_P_Get_Status()")); break;
	}	
	return FilterFactor;
}	

/****************************************************************************************
*BADS_7125_P_Get_SymbolRate() determines the symbol rate in the VID            
****************************************************************************************/
uint32_t BADS_7125_P_Get_SymbolRate(BADS_ChannelHandle hChannel, uint8_t ARFFactor, uint8_t FilterFactor, uint32_t SampleRate)
{	
	BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
	BADS_7125_Handle hImplDev = (BADS_7125_Handle) hDev->pImpl;
	BADS_7125_ChannelHandle h = (BADS_7125_ChannelHandle)(hChannel->pImpl);
	unsigned int chn = h->chnNo;
	
	/*Local Variables*/
	uint32_t ReadReg;
	uint32_t VID_SampleRate, SymbolRate;
	
	/*Local Variables used for multipliers and dividers in math calculations*/
	uint32_t	ulMultA, ulMultB, ulNrmHi, ulNrmLo, ulDivisor;

	/*This is an integer division, ARFFactor = 1,2 and FilterFactor is 1,2,4*/
	/*Check for division by 0                                               */
	if (ARFFactor*FilterFactor == 0)
	{
		BDBG_ERR(("divide by 0 in BADS_7125_P_Get_SymbolRate() while calculating VID_SampleRate"));
	}	
	VID_SampleRate = SampleRate/(ARFFactor*FilterFactor);

	/*Get the timing loop NCO value*/
	/*this is an unsigned multiply*/
	ReadReg = BADS_7125_P_ReadField(hImplDev->hRegister, DS_0_TLFOS, TLOFFSET, chn);

	ulMultA = ReadReg;
	ulMultB = VID_SampleRate;
	ulDivisor = POWER2_25;
	if (ulDivisor == 0)
	{
		BDBG_ERR(("divide by 0 in BADS_7125_P_Get_SymbolRate()"));
	}
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);
	if ((ulNrmHi != 0) || (ulNrmLo > 0x7FFFFFFF))
	{
		BDBG_WRN(("64to32 division is not in a 31 bit unsigned int range"));
	}

	SymbolRate = ulNrmLo;
	return SymbolRate;
}

/****************************************************************************************
*BADS_7125_P_Get_IfFrequency() determines the Frequency in the front mixer            
****************************************************************************************/
int32_t BADS_7125_P_Get_IfFrequency(BADS_ChannelHandle hChannel, uint32_t SampleRate, uint8_t ARFFactor)
{
	/****************************************************************************************************/
	/*Calculate the IF Frequency */
	/*IF = SampleRate*CFLFOS/2^16*/
	/****************************************************************************************************/

	BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
	BADS_7125_Handle hImplDev = (BADS_7125_Handle) hDev->pImpl;
	BADS_7125_ChannelHandle h = (BADS_7125_ChannelHandle)(hChannel->pImpl);
	unsigned int chn = h->chnNo;
	
	/*Local Variables*/
	uint8_t  RegIsNegative;
	uint32_t ReadReg;
	uint32_t IfFrequency;
	
	/*Local Variables used for multipliers and dividers in math calculations*/
	uint32_t	ulMultA, ulMultB, ulNrmHi, ulNrmLo, ulDivisor;
	
	ReadReg = BADS_7125_P_ReadField(hImplDev->hRegister, DS_0_CFLFOS, CFLOFFSET, chn);

	/*Detect if it is negative, this is a 16 bit 2's complement register*/
	RegIsNegative = 0;
	if ((ReadReg & 0x00008000) != 0)
	{
		RegIsNegative = 1;
		if (ReadReg == 0x00008000)
		{
			ReadReg = 0xFFFF8001; /*clamp value*/
		}
		ReadReg = ReadReg | 0xFFFF8000; /*sign extend*/
		ReadReg = ReadReg ^ 0xFFFFFFFF;	/*invert all bits*/
		ReadReg = ReadReg + 1;			/* add 1 */
	}
	
	ulMultA = SampleRate/ARFFactor;
	ulMultB = ReadReg;
	ulDivisor = POWER2_16;

	if (ulDivisor == 0)
	{
		BDBG_ERR(("divide by 0 in BADS_7125_P_Get_IfFrequency() while calculating IF Frequency"));
	}
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);
	if ((ulNrmHi != 0) || (ulNrmLo > 0x7FFFFFFF))
	{
		BDBG_WRN(("64to32 division is not in a 31 bit unsigned int range"));
	}

	if (RegIsNegative == 1)
	{
		ulNrmLo = ulNrmLo ^ 0xFFFFFFFF;	/*invert all bits*/
		ulNrmLo = ulNrmLo + 1;			/* add 1 */
	}
	
	IfFrequency = (int32_t)ulNrmLo;
	return IfFrequency;
}

/****************************************************************************************
*BADS_7125_P_Get_VIDTimingError() determines the VID Timing Error                                          
****************************************************************************************/
int32_t BADS_7125_P_Get_VIDTimingError(BADS_ChannelHandle hChannel, uint8_t ARFFactor, uint8_t FilterFactor, uint32_t SampleRate)	
{	
/*For QAM (with loop filter output gain set to 1.0) the offset in Hertz is: (1/8)*(TLI/2^31)*(SampleRate/FilterFactor) in Hertz*/
/*For QAM (with loop filter output gain set to 2^-6) the offset in Hertz is: (1/512)*(TLI/2^31)*(SampleRate/FilterFactor) in Hertz*/

	BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
	BADS_7125_Handle hImplDev = (BADS_7125_Handle) hDev->pImpl;
	BADS_7125_ChannelHandle h = (BADS_7125_ChannelHandle)(hChannel->pImpl);
	unsigned int chn = h->chnNo;
	
	/*Local Variables*/
	uint8_t  RegIsNegative;
	uint32_t ReadReg, TL_Gain;
	uint32_t VID_SampleRate, TimingFrequencyError;
	
	/*Local Variables used for multipliers and dividers in math calculations*/
	uint32_t	ulMultA, ulMultB, ulNrmHi, ulNrmLo, ulDivisor;

	/*This is an integer division, ARFFactor = 1,2 and FilterFactor is 1,2,4*/
	/*Check for division by 0                                               */
	if (ARFFactor*FilterFactor == 0)
	{
		BDBG_ERR(("divide by 0 in BADS_7125_P_Get_SymbolRate() while calculating VID_SampleRate"));
	}	
	VID_SampleRate = SampleRate/(ARFFactor*FilterFactor);

	TL_Gain = BADS_7125_P_ReadField(hImplDev->hRegister, DS_0_TLC, TLLFOUTGAIN, chn);
	ReadReg = BADS_7125_P_ReadField(hImplDev->hRegister, DS_0_TLI, TLVAL, chn);

	/*Detect if it is negative, this is a 32 bit 2's complement register*/
	RegIsNegative = 0;
	if ((ReadReg & 0x80000000) != 0)
	{
		RegIsNegative = 1;
		if (ReadReg == 0x80000000)
		{
			ReadReg = 0x80000001; /*clamp value*/
		}
		ReadReg = ReadReg ^ 0xFFFFFFFF;	/*invert all bits*/
		ReadReg = ReadReg + 1;			/* add 1 */
	}

	ulMultA = ReadReg;
	ulMultB = VID_SampleRate;
	ulDivisor = POWER2_31;
	if (ulDivisor == 0)
	{
		BDBG_ERR(("divide by 0 in BADS_7125_P_Get_VIDTimingError() while calculating TimingFrequencyError"));
	}
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);
	if ((ulNrmHi != 0) || (ulNrmLo > 0x7FFFFFFF))
	{
		BDBG_WRN(("64to32 division is not in a 31 bit unsigned int range"));
	}
	
	if (TL_Gain == 0)
	{
		ulNrmLo = ulNrmLo>>3;		/*loop filter output gain set to 1*/
	}
	else
	{
		ulNrmLo = ulNrmLo>>9;		/*loop filter output gain set to 2^-6*/
	}		 
	
	if (RegIsNegative == 1)
	{
		ulNrmLo = ulNrmLo ^ 0xFFFFFFFF;	/*invert all bits*/
		ulNrmLo = ulNrmLo + 1;			/* add 1 */
	}
	
	TimingFrequencyError = (int32_t)ulNrmLo;
	
	return TimingFrequencyError;
}

/****************************************************************************************
*BADS_7125_P_CarrierFrequencyError() determines the Front Loop Frequency Error                                          
****************************************************************************************/
int32_t BADS_7125_P_Get_CarrierFrequencyError(BADS_ChannelHandle hChannel, uint32_t SampleRate)
{
	BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
	BADS_7125_Handle hImplDev = (BADS_7125_Handle) hDev->pImpl;
	BADS_7125_ChannelHandle h = (BADS_7125_ChannelHandle)(hChannel->pImpl);
	unsigned int chn = h->chnNo;
	
	/*Local Variables*/
	uint8_t  RegIsNegative;
	uint32_t ReadReg;
	uint32_t CarrierFrequencyError;

	/*Local Variables used for multipliers and dividers in math calculations*/
	uint32_t	ulMultA, ulMultB, ulNrmHi, ulNrmLo, ulDivisor;

	ReadReg = BADS_7125_P_ReadField(hImplDev->hRegister, DS_0_CFLI, CFLVAL, chn);
	/*Detect if it is negative, this is a 32 bit 2's complement register*/
	/*FreqError = SampleRate*RegValue/((2^31)*32)*/
/*FreqError = (RegValue>>5)*SampleRate/(2^31)*/
	RegIsNegative = 0;
	if ((ReadReg & 0x80000000) != 0)
	{
		RegIsNegative = 1;
		if (ReadReg == 0x80000000)
		{
			ReadReg = 0x80000001; /*clamp value*/
		}
		ReadReg = ReadReg ^ 0xFFFFFFFF;	/*invert all bits*/
		ReadReg = ReadReg + 1;			/* add 1 */
	}

	ulMultA = (ReadReg>>5);
	ulMultB = SampleRate;
	ulDivisor = POWER2_31;
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);
	if ((ulNrmHi != 0) || (ulNrmLo > 0x7FFFFFFF))
	{
		BDBG_WRN(("64to32 division is not in a 31 bit unsigned int range"));
	}

	if (RegIsNegative == 1)
	{
		ulNrmLo = ulNrmLo ^ 0xFFFFFFFF;	/*invert all bits*/
		ulNrmLo = ulNrmLo + 1;			/* add 1 */
	}

	CarrierFrequencyError = (int32_t)ulNrmLo;
	return CarrierFrequencyError;
}

/****************************************************************************************
*BADS_7125_P_PhaseError() determines the Phase Loop Frequency Error                                          
****************************************************************************************/
int32_t BADS_7125_P_Get_CarrierPhaseError(BADS_ChannelHandle hChannel, uint32_t SymbolRate)
{
	BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
	BADS_7125_Handle hImplDev = (BADS_7125_Handle) hDev->pImpl;
	BADS_7125_ChannelHandle h = (BADS_7125_ChannelHandle)(hChannel->pImpl);
	unsigned int chn = h->chnNo;
	
	/*Local Variables*/
	uint8_t  RegIsNegative;
	uint32_t ReadReg;
	uint32_t CarrierPhaseError;

	/*Local Variables used for multipliers and dividers in math calculations*/
	uint32_t	ulMultA, ulMultB, ulNrmHi, ulNrmLo, ulDivisor;
	ReadReg = BADS_7125_P_ReadField(hImplDev->hRegister, DS_EQ_0_CPLI, CPLI, chn);
	/*Detect if it is negative, this is a 30 bit 2's complement register*/
	/*phase error is SymbolRate*RegValue/2^30*/
	RegIsNegative = 0;
	if ((ReadReg & 0x20000000) != 0)
	{
		RegIsNegative = 1;
		ReadReg = ReadReg | 0xC0000000;	/*sign extend*/
		ReadReg = ReadReg ^ 0xFFFFFFFF;	/*invert all bits*/
		ReadReg = ReadReg + 1;			/* add 1 */
	}

	ulMultA = ReadReg;
	ulMultB = SymbolRate;
	ulDivisor = POWER2_30;
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);
	if ((ulNrmHi != 0) || (ulNrmLo > 0x7FFFFFFF))
	{
		BDBG_WRN(("64to32 division is not in a 31 bit unsigned int range"));
	}

	if (RegIsNegative == 1)
	{
		ulNrmLo = ulNrmLo ^ 0xFFFFFFFF;	/*invert all bits*/
		ulNrmLo = ulNrmLo + 1;			/* add 1 */
	}

	CarrierPhaseError = (int32_t)ulNrmLo;
	return CarrierPhaseError;
}

/*******************************************************************************************
 * BADS_7125_P_SetDPM()		This routine sets the DPM for the tuner
 *This is a function called by the tuner PI
 *******************************************************************************************/
BERR_Code BADS_7125_P_SetDPM(BADS_ChannelHandle hChannel, uint8_t I_OUTDIV_M6)
{
	BERR_Code retCode;
	BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
	BADS_7125_Handle hImplDev = (BADS_7125_Handle) hDev->pImpl;
	BADS_7125_ChannelHandle h = (BADS_7125_ChannelHandle)(hChannel->pImpl);
	unsigned int chn = h->chnNo;

	/*local variables*/
	int32_t DPM_Freq;
	uint8_t ARFFactor;
	uint32_t DPM_SampleRate;
	uint32_t ulMultA, ulMultB, ulDivisor, ulNrmLo, ulNrmHi;

	/*determine the DPM Frequency*/
	DPM_Freq = 540000000/((I_OUTDIV_M6<<2)+8);

	/*Get the Sample Rate*/
	DPM_SampleRate = BADS_7125_P_Get_SampleRate(hChannel);
	ARFFactor = BADS_7125_P_Get_ARFFactor(hChannel);

	/*This is an integer division, ARFFactor = 1,2*/
	/*Check for division by 0*/
	if (ARFFactor == 1)
	{
		DPM_SampleRate = DPM_SampleRate;    /*no division, ARF bypassed*/
	}	
	else if (ARFFactor == 2)
	{
		DPM_SampleRate = DPM_SampleRate>>1; /*ARF used, divide by 2*/
	}
	else
	{
		BDBG_ERR(("Invalid ARFFactor BADS_7125_P_SetDPM()"));
	}

	ulMultA = POWER2_24;
	ulMultB = DPM_Freq;
	ulDivisor = DPM_SampleRate;
	if (ulDivisor == 0)
	{
		BDBG_ERR(("divide by 0 in BADS_7125_P_SetDPM()"));
	}
	BMTH_HILO_32TO64_Mul(ulMultA, ulMultB, &ulNrmHi, &ulNrmLo);
	BMTH_HILO_64TO64_Div32(ulNrmHi, ulNrmLo, ulDivisor, &ulNrmHi, &ulNrmLo);
	if ((ulNrmHi != 0) || (ulNrmLo > 0x007FFFFF))
	{
		BDBG_WRN(("64to32 division is not in a 23 bit unsigned int range"));
	}

	/*Write the value to the 24 bit twos complement register*/
	BADS_7125_P_WriteField(hImplDev->hRegister, DS_TUNER_CHNZ_0_DPM, DPMFCW, (uint32_t)ulNrmLo, chn); /*FCW*/ 
	BADS_7125_P_WriteField(hImplDev->hRegister, DS_TUNER_CHNZ_0_DPM, DPMNOTCHBWC, 0x04, chn);		/*BW=2^-15*/
	BADS_7125_P_WriteField(hImplDev->hRegister, DS_TUNER_CHNZ_0_DPM, DPMDCBYP, 0, chn);	 			/*unbypass*/
	BADS_7125_P_WriteField(hImplDev->hRegister, DS_TUNER_CHNZ_0_DPM, DPMCMULTBYP, 0, chn);			/*enable entire block*/
	BADS_7125_P_WriteField(hImplDev->hRegister, DS_TUNER_CHNZ_0_DPM, DPMDCRST, 1, chn);				/*reset the DPM integrator*/

	/*Unfreeze the DPM integrator*/
	/*This uses the DS_FRZ and so does the acquisition script*/
	BADS_7125_P_WriteField(hImplDev->hRegister, DS_0_FRZ,	DPMDCFRZ, 0, chn);
	
	/*Setup Complete*/
	BDBG_MSG((" BADS_7125_P_SetDPM() Complete\n"));

	retCode = BERR_SUCCESS;
	return retCode;
}

/*******************************************************************************************
 * BADS_7125_P_ResetDPM()		This routine resets the DPM integrator for the tuner
  *This is a function called by the tuner PI
 *******************************************************************************************/
BERR_Code BADS_7125_P_ResetDPM(BADS_ChannelHandle hChannel)
{
	BERR_Code retCode;
	BADS_Handle hDev = (BADS_Handle)(hChannel->hAds);
	BADS_7125_Handle hImplDev = (BADS_7125_Handle) hDev->pImpl;
	BADS_7125_ChannelHandle h = (BADS_7125_ChannelHandle)(hChannel->pImpl);
	unsigned int chn = h->chnNo;


	/*reset the DPM integrator*/
	BADS_7125_P_WriteField(hImplDev->hRegister, DS_TUNER_CHNZ_0_DPM, DPMDCRST, 1, chn);
	
	/*Reset Complete*/
	BDBG_MSG((" BADS_7125_P_ResetDPM() Complete\n"));

	retCode = BERR_SUCCESS;
  return retCode;
}









