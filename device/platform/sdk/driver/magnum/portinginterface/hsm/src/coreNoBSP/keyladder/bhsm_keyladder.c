/***************************************************************************
 *     (c)2007-2008 Broadcom Corporation
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
 * $brcm_Workfile: bhsm_keyladder.c $
 * $brcm_Revision: Hydra_Software_Devel/3 $
 * $brcm_Date: 9/28/12 3:55p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /magnum/portinginterface/hsm/src/coreNoBSP/keyladder/bhsm_keyladder.c $
 * 
 * Hydra_Software_Devel/3   9/28/12 3:55p atruong
 * SWSECURITY-175: Symbolic link removal for 7408 and 7550 HSM PI
 * 
 * Hydra_Software_Devel/2   2/23/11 3:38p haoboy
 * SW7550-692:added support for M2M ,  AES keyladder
 * 
 * Hydra_Software_Devel/1   1/21/11 10:00a haoboy
 * SW7550-666:initial version
 * 
 * Hydra_Software_Devel/20   11/26/10 4:59p jhaberf
 * SW35230-13: Added 35125 DTV chip support
 * 
 * Hydra_Software_Devel/19   9/29/10 11:29a qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/18   9/24/10 2:17p qcheng
 * SW7550-419 : 7550's BHSM_FreeCAKeySlot appears to have a wrong API
 * 
 * Hydra_Software_Devel/17   6/23/10 3:29p cdisc
 * SW35230-420: CA core bringup
 * 
 * Hydra_Software_Devel/16   6/18/10 2:43p cdisc
 * SW35230-352: M2M and partial key ladder support (enabling M2M for 35230
 * only)
 * 
 * Hydra_Software_Devel/15   6/11/10 10:02a qcheng
 * SW7550-427 : Coverity - fix the 7550 "magnum_pi_hsm" module specific
 * coverity defects
 * 
 * Hydra_Software_Devel/13   6/10/10 11:28a qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/12   6/10/10 11:05a qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/11   6/2/10 2:32p cdisc
 * SW35230-51:   Magnum HSM PI needs to build for the 35230
 * 
 * Hydra_Software_Devel/10   5/25/10 6:29p qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/8   5/11/10 7:18p qcheng
 * SW7550-419 : 7550's BHSM_FreeCAKeySlot appears to have a wrong API
 * 
 * Hydra_Software_Devel/5   4/5/10 5:39p qcheng
 * SW7550-353: PVR encryption feature is required for 7540 production with
 * PVR function
 * 
 * Hydra_Software_Devel/4   4/2/10 7:32p qcheng
 * SW7550-353: PVR encryption feature is required for 7540 production with
 * PVR function
 * 
 * Hydra_Software_Devel/3   3/22/10 7:05p qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/2   3/4/10 11:39a qcheng
 * SW7408-82 : Add security support for 7408 A0
 * 
 * Hydra_Software_Devel/1   12/4/09 5:43p qcheng
 * SW7550-102 : Add Security support for 7550
 *
 **************************************************************************/

#include "bstd.h" 
#include "bkni.h"
#include "bhsm_keyladder.h" 
#if ( BCHP_CHIP == 7550)
#include "xpt_security_key_table.h"
#include "chip_secure_7550.h"
#elif ( BCHP_CHIP == 35230) || ( BCHP_CHIP == 35125 ) || ( BCHP_CHIP == 35233 )
#include "xpt_security_key_table.h"
#include "chip_secure_35230.h"
#include "mmscram_key_table.h"
#elif ( BCHP_CHIP == 7408)
#include "mmscram_key_table.h"
#include "xpt_mmscram.h"
#endif

BDBG_MODULE(BHSM);

/*#define BDBG_MSG(x) printf x  */
/*#define BDBG_MSG(x)  */

BERR_Code BHSM_ConfigCaAlgorithm(
	BHSM_Handle				in_handle,
	BHSM_ConfigAlgorithmIO_t		*inp_configAlgorithmIO
);

BERR_Code BHSM_ConfigM2MAlgorithm(
	BHSM_Handle				in_handle,
	BHSM_ConfigAlgorithmIO_t		*inp_configAlgorithmIO
);


BERR_Code BHSM_ConfigAlgorithm(
	BHSM_Handle				in_handle,
	BHSM_ConfigAlgorithmIO_t		*inp_configAlgorithmIO
)
{
	switch (inp_configAlgorithmIO->ekeyDestBlckType ) {
		case  BHSM_KeyDestBlockType_eCA :
		case  BHSM_KeyDestBlockType_eCPS :
		case  BHSM_KeyDestBlockType_eCPD :
			return BHSM_ConfigCaAlgorithm(in_handle,inp_configAlgorithmIO);
			
		case BHSM_KeyDestBlockType_eM2M : 
			return BHSM_ConfigM2MAlgorithm(in_handle,inp_configAlgorithmIO);
			
		default: 
			BDBG_ERR(("keyDestBlckType=  %d is not invalid key address!\n ", inp_configAlgorithmIO->ekeyDestBlckType));
			return BHSM_STATUS_INPUT_PARM_ERR;
	}
}

BERR_Code BHSM_ConfigCaAlgorithm(
	BHSM_Handle				in_handle,
	BHSM_ConfigAlgorithmIO_t		*inp_configAlgorithmIO
)
{
#if ( BCHP_CHIP !=7408)
	unsigned int keyDest;
	uint32_t regVal;
	uint32_t	globalRegVal, caRegVal, cpdRegVal,cpsRegVal;
	BHSM_P_ProcessKeyIO_t processKeyIO;


	switch (inp_configAlgorithmIO->eKeyType){
		case BHSM_KeyType_eOddKey:
			keyDest = BHSM_KeyAddr_eOddKeyMode1+ inp_configAlgorithmIO->unKeySlot*BHSM_CA_KEYSLOT_STEPSIZE;
			break;

		case BHSM_KeyType_eEvenKey:
			keyDest = BHSM_KeyAddr_eEvenKeyMode1+ inp_configAlgorithmIO->unKeySlot*BHSM_CA_KEYSLOT_STEPSIZE;
			break;

		case BHSM_KeyType_eClearKey:
			keyDest = BHSM_KeyAddr_eClearKeyMode1+ inp_configAlgorithmIO->unKeySlot*BHSM_CA_KEYSLOT_STEPSIZE;
			break;

		default:
			BDBG_ERR(("KeyType = %d is not supported !\n", inp_configAlgorithmIO->eKeyType));	
			return BHSM_STATUS_INPUT_PARM_ERR ;
	}


	globalRegVal = 0;
	regVal = 0;
	
	globalRegVal =	
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.bEncryptBeforeRave << XPT_SECURITY_KEY_TABLE_GLOBAL_KEY_MODE_ENCRYPT_BEFORE_RAVE_SHIFT ) & XPT_SECURITY_KEY_TABLE_GLOBAL_KEY_MODE_ENCRYPT_BEFORE_RAVE_MASK) |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.bGlobalRegOverwrite << XPT_SECURITY_KEY_TABLE_GLOBAL_KEY_MODE_GLOBAL_REGION_OVERRIDE_SHIFT) & XPT_SECURITY_KEY_TABLE_GLOBAL_KEY_MODE_GLOBAL_REGION_OVERRIDE_MASK) |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.bRestrictDropPktCtrl <<  XPT_SECURITY_KEY_TABLE_GLOBAL_KEY_MODE_RESTR_DROP_PACKET_SHIFT) & XPT_SECURITY_KEY_TABLE_GLOBAL_KEY_MODE_RESTR_DROP_PACKET_MASK)| 
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.bGlobalDropPktCtrl << XPT_SECURITY_KEY_TABLE_GLOBAL_KEY_MODE_GLOBAL_DROP_PACKET_SHIFT) & XPT_SECURITY_KEY_TABLE_GLOBAL_KEY_MODE_GLOBAL_DROP_PACKET_MASK) |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.bAtscMod <<  XPT_SECURITY_KEY_TABLE_GLOBAL_KEY_MODE_ATSC_MOD_SHIFT) & XPT_SECURITY_KEY_TABLE_GLOBAL_KEY_MODE_ATSC_MOD_MASK ) |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.bAtscScrambleCtrl << XPT_SECURITY_KEY_TABLE_GLOBAL_KEY_MODE_ATSC_SCRAMBLE_CTRL_SHIFT) & XPT_SECURITY_KEY_TABLE_GLOBAL_KEY_MODE_ATSC_SCRAMBLE_CTRL_MASK) |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.ucMulti2KeySelect << XPT_SECURITY_KEY_TABLE_GLOBAL_KEY_MODE_MULTI2_SYSKEY_SEL_SHIFT) & XPT_SECURITY_KEY_TABLE_GLOBAL_KEY_MODE_MULTI2_SYSKEY_SEL_MASK) ;
			
	regVal =
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.uScValue << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_SC_VALUE_SHIFT) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_SC_VALUE_MASK) |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.bGlobalScMod <<  XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_GLOBAL_SC_MOD_SHIFT) &  XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_GLOBAL_SC_MOD_MASK) |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.bRestrictScMod << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_RESTR_SC_MOD_SHIFT ) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_RESTR_SC_MOD_MASK) |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.bGlobalEnable << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_GLOBAL_EN_SHIFT) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_GLOBAL_EN_MASK) |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.bRestrictEnable << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_RESTR_EN_SHIFT) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_RESTR_EN_MASK) |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.bMscSel << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_MSC_SEL_SHIFT ) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_MSC_SEL_MASK) |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.solitaryMode << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_SOLITARY_TERM_SEL_SHIFT ) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_SOLITARY_TERM_SEL_MASK) |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.IVMode << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_IV_MODE_SEL_SHIFT ) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_IV_MODE_SEL_MASK) |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.terminationMode << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_TERM_MODE_SEL_SHIFT) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_TERM_MODE_SEL_MASK) |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.cipherDVBMode << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_NOT_DVB_CIPHER_MODE_SEL_SHIFT) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_NOT_DVB_CIPHER_MODE_SEL_MASK) |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.algorithm << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_ALG_SEL_SHIFT  ) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_ALG_SEL_MASK)  |
		((inp_configAlgorithmIO->cryptoAlg.cacpCryptAlg.eKeyOffset << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_KEY_OFFSET_SHIFT  ) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_KEY_OFFSET_MASK)  ;

	switch (inp_configAlgorithmIO->ekeyDestBlckType ) {
		case BHSM_KeyDestBlockType_eCA:
			caRegVal = regVal ;
			cpsRegVal = in_handle->aCaCpKeySlotInfo[inp_configAlgorithmIO->unKeySlot].aKeySlotAlgorithm[inp_configAlgorithmIO->eKeyType].ulCPSControlBits;
			cpdRegVal = in_handle->aCaCpKeySlotInfo[inp_configAlgorithmIO->unKeySlot].aKeySlotAlgorithm[inp_configAlgorithmIO->eKeyType].ulCPDControlBits;
			break;

		case BHSM_KeyDestBlockType_eCPS:
			caRegVal = in_handle->aCaCpKeySlotInfo[inp_configAlgorithmIO->unKeySlot].aKeySlotAlgorithm[inp_configAlgorithmIO->eKeyType].ulCAControlBits;
			cpsRegVal = regVal;
			cpdRegVal = in_handle->aCaCpKeySlotInfo[inp_configAlgorithmIO->unKeySlot].aKeySlotAlgorithm[inp_configAlgorithmIO->eKeyType].ulCPDControlBits;
			break;

		case BHSM_KeyDestBlockType_eCPD:
			caRegVal = in_handle->aCaCpKeySlotInfo[inp_configAlgorithmIO->unKeySlot].aKeySlotAlgorithm[inp_configAlgorithmIO->eKeyType].ulCAControlBits;
			cpsRegVal = in_handle->aCaCpKeySlotInfo[inp_configAlgorithmIO->unKeySlot].aKeySlotAlgorithm[inp_configAlgorithmIO->eKeyType].ulCPSControlBits;
			cpdRegVal = regVal;
			break;

		default:
			BDBG_ERR(("keyDestBlckType=  %d is not invalid key address!\n ", inp_configAlgorithmIO->ekeyDestBlckType));
			return BHSM_STATUS_INPUT_PARM_ERR;
	}

	processKeyIO.eKeyLadderModule = BHSM_KeyLadderModule_eSecurity ;
	processKeyIO.eKeyProcess = BHSM_KeyGenProcess_eCwDirectSW;
	processKeyIO.unCustKey = 0 ;
	processKeyIO.unKeyVar = 0;
	processKeyIO.procInHigh = caRegVal;
	processKeyIO.procInLow = globalRegVal;
	processKeyIO.eOperation = BHSM_Operation_eCA;
	processKeyIO.unKeyDest = keyDest ; /* keyDest + 0  is for 1st 64 bit for odd/even/clear key mode( General/CA ) */	

	BDBG_MSG(("%s - Configure Algorithem to key Destiantion = 0x%x !\n", __FUNCTION__, processKeyIO.unKeyDest ));	 
	
	check(BHSM_P_ProcessKey( in_handle , &processKeyIO ));

	
	processKeyIO.procInHigh = cpdRegVal;
	processKeyIO.procInLow = cpsRegVal;
	processKeyIO.unKeyDest = keyDest + 1 ; /* keyDest + 1  is for 1st 64 bit for odd/even/clear key mode( CPS/CPD ) */	
	BDBG_MSG(("%s - Configure Algorithem to key Destiantion = 0x%x !\n", __FUNCTION__, processKeyIO.unKeyDest));	 
	
	check(BHSM_P_ProcessKey( in_handle , &processKeyIO ));

	in_handle->aCaCpKeySlotInfo[inp_configAlgorithmIO->unKeySlot].aKeySlotAlgorithm[inp_configAlgorithmIO->eKeyType].ulGlobalControlBits = globalRegVal ;
	in_handle->aCaCpKeySlotInfo[inp_configAlgorithmIO->unKeySlot].aKeySlotAlgorithm[inp_configAlgorithmIO->eKeyType].ulCAControlBits = caRegVal ;
	in_handle->aCaCpKeySlotInfo[inp_configAlgorithmIO->unKeySlot].aKeySlotAlgorithm[inp_configAlgorithmIO->eKeyType].ulCPSControlBits = cpsRegVal ;
	in_handle->aCaCpKeySlotInfo[inp_configAlgorithmIO->unKeySlot].aKeySlotAlgorithm[inp_configAlgorithmIO->eKeyType].ulCPDControlBits = cpdRegVal ;

	return BERR_SUCCESS;
#else
 	BSTD_UNUSED(in_handle);
 	BSTD_UNUSED(inp_configAlgorithmIO);

	BDBG_ERR(("BCHP_CHIP= %d do not support CA \n", BCHP_CHIP));
	return BHSM_STATUS_NO_HW_SUPPORT ;
#endif

}

BERR_Code BHSM_ConfigM2MAlgorithm(
	BHSM_Handle				in_handle,
	BHSM_ConfigAlgorithmIO_t		*inp_configAlgorithmIO
)
{
#if ( BCHP_CHIP != 7550)
	unsigned int keyDest;
	uint32_t regVal1,regVal2;
	BHSM_P_ProcessKeyIO_t processKeyIO;

	if (inp_configAlgorithmIO->ekeyDestBlckType != BHSM_KeyDestBlockType_eM2M ) {
		BDBG_ERR(("keyDestBlckType=  %d is not invalid key address!\n ", inp_configAlgorithmIO->ekeyDestBlckType));
		return BHSM_STATUS_INPUT_PARM_ERR;
	}

	processKeyIO.eKeyLadderModule = BHSM_KeyLadderModule_eMmscram;

	keyDest = BHSM_KeyAddr_eM2MKeyMode + inp_configAlgorithmIO->unKeySlot*BHSM_M2M_KEYSLOT_STEPSIZE;
	
	regVal1 =	((inp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.ucAuthCtrl  << XPT_MMSCRAM_KEY_TABLE_KEY_MODE_ENCRYPTION_MODE_SHIFT) & XPT_MMSCRAM_KEY_TABLE_KEY_MODE_ENCRYPTION_MODE_MASK) |
			((inp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.solitaryMode << XPT_MMSCRAM_KEY_TABLE_KEY_MODE_SOLITARY_TERM_SEL_SHIFT ) & XPT_MMSCRAM_KEY_TABLE_KEY_MODE_SOLITARY_TERM_SEL_MASK) |
			((inp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.IVMode << XPT_MMSCRAM_KEY_TABLE_KEY_MODE_IV_MODE_SEL_SHIFT ) & XPT_MMSCRAM_KEY_TABLE_KEY_MODE_IV_MODE_SEL_MASK) |
			((inp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.ucAESCounterKeyMode << XPT_MMSCRAM_KEY_TABLE_KEY_MODE_COUNTER_COUNTER_MODE_SEL_SHIFT) & XPT_MMSCRAM_KEY_TABLE_KEY_MODE_COUNTER_COUNTER_MODE_SEL_MASK) |
			((inp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.cipherDVBMode << XPT_MMSCRAM_KEY_TABLE_KEY_MODE_CIPHER_MODE_SEL_SHIFT) & XPT_MMSCRAM_KEY_TABLE_KEY_MODE_CIPHER_MODE_SEL_MASK ) |
			((inp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.m2mSecAlg << XPT_MMSCRAM_KEY_TABLE_KEY_MODE_ALG_SEL_SHIFT  ) & XPT_MMSCRAM_KEY_TABLE_KEY_MODE_ALG_SEL_MASK)  |
			((inp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.eKeyOffset << XPT_MMSCRAM_KEY_TABLE_KEY_MODE_KEY_OFFSET_SHIFT  ) & XPT_MMSCRAM_KEY_TABLE_KEY_MODE_KEY_OFFSET_MASK)  ;

	regVal2 =	((inp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.bEnableTimestamp << XPT_MMSCRAM_KEY_TABLE_KEY_MODE_HI_TIMESTSMP_EN_SHIFT ) & XPT_MMSCRAM_KEY_TABLE_KEY_MODE_HI_TIMESTSMP_EN_MASK) |
			((inp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.bDisallow_RR<< XPT_MMSCRAM_KEY_TABLE_KEY_MODE_HI_DISALLOW_RR_SHIFT) & XPT_MMSCRAM_KEY_TABLE_KEY_MODE_HI_DISALLOW_RR_MASK) |
			((inp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.bDisallow_RG <<  XPT_MMSCRAM_KEY_TABLE_KEY_MODE_HI_DISALLOW_RG_SHIFT) & XPT_MMSCRAM_KEY_TABLE_KEY_MODE_HI_DISALLOW_RG_MASK)| 
			((inp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.bDisallow_GR << XPT_MMSCRAM_KEY_TABLE_KEY_MODE_HI_DISALLOW_GR_SHIFT) & XPT_MMSCRAM_KEY_TABLE_KEY_MODE_HI_DISALLOW_GR_MASK) |
			((inp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.bDisallow_GG <<  XPT_MMSCRAM_KEY_TABLE_KEY_MODE_HI_DISALLOW_GG_SHIFT) & XPT_MMSCRAM_KEY_TABLE_KEY_MODE_HI_DISALLOW_GG_MASK ) |
			((inp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.bUseExtIV << XPT_MMSCRAM_KEY_TABLE_KEY_MODE_HI_EXT_IV_EN_SHIFT ) & XPT_MMSCRAM_KEY_TABLE_KEY_MODE_HI_EXT_IV_EN_MASK ) |
			((inp_configAlgorithmIO->cryptoAlg.m2mCryptAlg.bUseExtKey << XPT_MMSCRAM_KEY_TABLE_KEY_MODE_HI_EXT_KEY_EN_SHIFT) & XPT_MMSCRAM_KEY_TABLE_KEY_MODE_HI_EXT_KEY_EN_MASK) ;

	processKeyIO.eKeyProcess = BHSM_KeyGenProcess_eCwDirectSW;
	processKeyIO.unCustKey = 0 ;
	processKeyIO.unKeyVar = 0;
	processKeyIO.procInHigh = regVal2;
	processKeyIO.procInLow = regVal1;
	processKeyIO.eOperation = BHSM_Operation_eCP;
	processKeyIO.unKeyDest = keyDest ; 	
	
	check(BHSM_P_ProcessKey( in_handle , &processKeyIO ));

	in_handle->aM2MKeySlotInfo[inp_configAlgorithmIO->unKeySlot].aKeySlotAlgorithm.ulGlobalControlBits = regVal2 ;
	in_handle->aM2MKeySlotInfo[inp_configAlgorithmIO->unKeySlot].aKeySlotAlgorithm.ulM2MControlBits = regVal1 ;
	
	return BERR_SUCCESS;
#else	
 	BSTD_UNUSED(in_handle);
 	BSTD_UNUSED(inp_configAlgorithmIO);

	BDBG_ERR(("BCHP_CHIP= %d do not support M2M DMA \n", BCHP_CHIP));
	return BHSM_STATUS_NO_HW_SUPPORT ;
#endif

}

BERR_Code BHSM_LoadCwIv(
	BHSM_Handle				in_handle,
	BHSM_LoadCwIvIO_t		*inp_loadCwIvIO
)
{
	unsigned int keyDest;
	BHSM_P_ProcessKeyIO_t processKeyIO;
	unsigned  int  keySlotStepSize;

	if ( inp_loadCwIvIO->eKeyAddr < BHSM_KeyAddr_eM2MIva || inp_loadCwIvIO->eKeyAddr >= BHSM_KeyAddr_eMaxNumber ) {
		BDBG_ERR(("Key address =  %d is invalid key address!\n ", inp_loadCwIvIO->eKeyAddr));
		return BHSM_STATUS_INPUT_PARM_ERR;
	}

	if ( inp_loadCwIvIO->unKeySize != 8 && inp_loadCwIvIO->unKeySize != 16 ) {
		BDBG_ERR(("Keyladder Key size =  %d is invalid !\n ", inp_loadCwIvIO->eKeyAddr));
		return BHSM_STATUS_INPUT_PARM_ERR;
	}	
		
	if (inp_loadCwIvIO->eKeyLadderModule ==  BHSM_KeyLadderModule_eSecurity)	
		keySlotStepSize = BHSM_CA_KEYSLOT_STEPSIZE ;
	else if (inp_loadCwIvIO->eKeyLadderModule ==  BHSM_KeyLadderModule_eMmscram)
		keySlotStepSize = BHSM_M2M_KEYSLOT_STEPSIZE ;
	else
		return BHSM_STATUS_INPUT_PARM_ERR;

				
	keyDest = inp_loadCwIvIO->eKeyAddr + inp_loadCwIvIO->unKeySlot*keySlotStepSize;
		
	processKeyIO.eKeyProcess = BHSM_KeyGenProcess_eCwDirectSW;
	processKeyIO.unCustKey = 0 ;
	processKeyIO.unKeyVar = 0;
	processKeyIO.eKeyLadderModule = inp_loadCwIvIO->eKeyLadderModule;
	processKeyIO.eOperation = inp_loadCwIvIO->eOperation;
	processKeyIO.unKeyDest = keyDest ; 
	
	if ( (inp_loadCwIvIO->unKeySize == 16)) {		
		processKeyIO.procInHigh  = 	( inp_loadCwIvIO->aucCwIv[8] << 24) |
							 	( inp_loadCwIvIO->aucCwIv[9] << 16) |
							 	( inp_loadCwIvIO->aucCwIv[10] << 8) |
							 	( inp_loadCwIvIO->aucCwIv[11]  ) ;
	
		processKeyIO.procInLow  = 	( inp_loadCwIvIO->aucCwIv[12] << 24) |
							 	( inp_loadCwIvIO->aucCwIv[13] << 16) |
							 	( inp_loadCwIvIO->aucCwIv[14] << 8) |
							 	( inp_loadCwIvIO->aucCwIv[15]  ) ;

		BDBG_MSG(("Load CW or IV to key Destiantion = 0x%x eOperation= %d !\n", processKeyIO.unKeyDest,inp_loadCwIvIO->eOperation));	
		
		check(BHSM_P_ProcessKey( in_handle , &processKeyIO ));
		
		processKeyIO.unKeyDest ++;
	}

	
	processKeyIO.procInHigh  = 	( inp_loadCwIvIO->aucCwIv[0] << 24) |
						 	( inp_loadCwIvIO->aucCwIv[1] << 16) |
						 	( inp_loadCwIvIO->aucCwIv[2] << 8) |
						 	( inp_loadCwIvIO->aucCwIv[3]  ) ;

	processKeyIO.procInLow  = 	( inp_loadCwIvIO->aucCwIv[4] << 24) |
						 	( inp_loadCwIvIO->aucCwIv[5] << 16) |
						 	( inp_loadCwIvIO->aucCwIv[6] << 8) |
						 	( inp_loadCwIvIO->aucCwIv[7]  ) ;

	BDBG_MSG(("Load CW or IV to key Destiantion = 0x%x eOperation= %d !\n", processKeyIO.unKeyDest,inp_loadCwIvIO->eOperation));	

	check(BHSM_P_ProcessKey( in_handle , &processKeyIO ));


	return BERR_SUCCESS;
}

BERR_Code BHSM_ConfigPidKeyPointerTable (
	BHSM_Handle							in_handle,
	BHSM_ConfigPidKeyPointerTableIO_t		*inp_configPidKeyPointerTableIO
)
{
#if ( BCHP_CHIP != 7408)
	uint32_t reg;
	unsigned int keyAddress;

	BSTD_UNUSED(in_handle);

	if (( inp_configPidKeyPointerTableIO->unKeySlotNum !=  (unsigned int)BHSM_SLOT_NUM_INIT_VAL) &&
	    ( inp_configPidKeyPointerTableIO->unKeySlotNum >= BHSM_MaxCaCpKeySlot ))
		return BHSM_STATUS_INPUT_PARM_ERR;

	if ( inp_configPidKeyPointerTableIO->unPidChannel >= BHSM_MaxPidChannelNum )
		return BHSM_STATUS_INPUT_PARM_ERR;

	keyAddress = inp_configPidKeyPointerTableIO->unKeySlotNum * BHSM_CA_KEYSLOT_STEPSIZE;
	if ( inp_configPidKeyPointerTableIO->unKeySlotNum ==  (unsigned int)BHSM_SLOT_NUM_INIT_VAL) 
	{
		in_handle->aunPidChannelToCaCpKeySlotNum[inp_configPidKeyPointerTableIO->unPidChannel] = (unsigned int)BHSM_SLOT_NUM_INIT_VAL;
		reg = XPT_SECURITY_KEY_POINTER0;
	}
	else
	{
	reg = XPT_SECURITY_KEY_POINTER0 + inp_configPidKeyPointerTableIO->unPidChannel * ( XPT_SECURITY_KEY_POINTER1 - XPT_SECURITY_KEY_POINTER0);
		in_handle->aunPidChannelToCaCpKeySlotNum[inp_configPidKeyPointerTableIO->unPidChannel] = inp_configPidKeyPointerTableIO->unKeySlotNum ; 

	}

	BDBG_MSG(("Connect Pid Channel 0x%x to keyslot 0x%x \n",inp_configPidKeyPointerTableIO->unPidChannel,inp_configPidKeyPointerTableIO->unKeySlotNum));	
	BREG_Write32 (in_handle ->regHandle, reg, keyAddress );
	BDBG_MSG(("XPT_SECURITY_KEY_POINTER%d(0x%08x) = 0x%08x\n", inp_configPidKeyPointerTableIO->unPidChannel,reg,keyAddress ));	

	/*in_handle->aunPidChannelToCaCpKeySlotNum[inp_configPidKeyPointerTableIO->unPidChannel] = inp_configPidKeyPointerTableIO->unKeySlotNum ; 
*/
	return BERR_SUCCESS;
#else
 	BSTD_UNUSED(in_handle);
 	BSTD_UNUSED(inp_configPidKeyPointerTableIO);

	BDBG_ERR(("BCHP_CHIP= %d do not support CA \n", BCHP_CHIP));
	return BHSM_STATUS_NO_HW_SUPPORT ;
#endif
	
}

BERR_Code BHSM_ConfigMulti2(	
	BHSM_Handle				in_handle,
	BHSM_ConfigMulti2IO_t		*inp_configMulti2IO
)	
{
#if ( BCHP_CHIP != 7408)
	uint32_t reg,regVal;
	int i;

	BSTD_UNUSED(in_handle);

	switch (inp_configMulti2IO->ucSysKeyDest)
	{
		case BHSM_Muti2KeyDest_eKey0:
			BREG_Write32 (in_handle ->regHandle, XPT_SECURITY_MULTI2_RC0 , inp_configMulti2IO->ucMulti2RndCnt);
			BDBG_MSG((" Program RC0: 0x%08x = 0x%08x \n ", XPT_SECURITY_MULTI2_RC0,  inp_configMulti2IO->ucMulti2RndCnt));
			reg = XPT_SECURITY_MULTI2_SK0_0;
			break;
			
		case BHSM_Muti2KeyDest_eKey1:
			BREG_Write32 (in_handle ->regHandle, XPT_SECURITY_MULTI2_RC1 , inp_configMulti2IO->ucMulti2RndCnt);
			BDBG_MSG((" Program RC1: 0x%08x = 0x%08x \n ", XPT_SECURITY_MULTI2_RC1,  inp_configMulti2IO->ucMulti2RndCnt));
			reg = XPT_SECURITY_MULTI2_SK1_0 ;
			break;

		default:
			BDBG_ERR(("Multi2 Keydest = %d is not supported in 7002 !\n ", inp_configMulti2IO->ucSysKeyDest ));
			return BHSM_STATUS_INPUT_PARM_ERR;
			
	}

	for ( i = 0 ; i < 8 ; i++ )
	{
		regVal = 	( inp_configMulti2IO->aucSysKey[ i*4] << 24) |
			 	( inp_configMulti2IO->aucSysKey[ i*4 + 1] << 16) |
			 	( inp_configMulti2IO->aucSysKey[ i*4 + 2] << 8) |
			 	( inp_configMulti2IO->aucSysKey[ i*4 + 3]  ) ;
		BREG_Write32 (in_handle ->regHandle, reg + i*4 , regVal );
		BDBG_MSG((" 0x%08x = 0x%08x \n ", reg + i*4,  regVal));
	}		

	return BERR_SUCCESS;
#else
 	BSTD_UNUSED(in_handle);
 	BSTD_UNUSED(inp_configMulti2IO);

	BDBG_ERR(("BCHP_CHIP= %d do not support CA \n", BCHP_CHIP));
	return BHSM_STATUS_NO_HW_SUPPORT ;
#endif
		
}

BERR_Code BHSM_AllocateCAKeySlot(	
	BHSM_Handle						in_handle,
/* 	unsigned int pid_channel, */
	unsigned int *unKeySlot
)
{
#if ( BCHP_CHIP != 7408)
	unsigned int i;

	BSTD_UNUSED(in_handle);

#if 0
	for(i=0; i < BHSM_MaxCaCpKeySlot; i++){
#else		
	for(i=1; i < BHSM_MaxCaCpKeySlot; i++){
#endif		
		if(!in_handle ->aCaCpKeySlotInfo[i].bIsUsed) break;
	}

	if(i== BHSM_MaxCaCpKeySlot){
		*unKeySlot = -1;

		BDBG_ERR(("CA key slot is not allocated"));
		return BHSM_STATUS_FAILED;
	}
	
	*unKeySlot = i ;
	in_handle ->aCaCpKeySlotInfo[i].bIsUsed = true;

	BDBG_MSG((" find CA keyslot = %d !\n", *unKeySlot ));

	return BERR_SUCCESS;

#else
 	BSTD_UNUSED(in_handle);
 	BSTD_UNUSED(unKeySlot);

	BDBG_ERR(("BCHP_CHIP= %d do not support CA \n", BCHP_CHIP));
	return BHSM_STATUS_NO_HW_SUPPORT ;
#endif


}

BERR_Code BHSM_LocateCAKeySlotAssigned(
	BHSM_Handle						in_handle,
	unsigned int pid_channel,
	unsigned int *unKeySlot
)
{
#if ( BCHP_CHIP != 7408)

	BSTD_UNUSED(in_handle);

	if(pid_channel >= BHSM_MaxPidChannelNum)
		return BHSM_STATUS_INPUT_PARM_ERR; 

	if( in_handle ->aunPidChannelToCaCpKeySlotNum[pid_channel] == (unsigned int)BHSM_SLOT_NUM_INIT_VAL){ 
		BDBG_MSG(("CA key slot is not assigned at pid channel %d\n", pid_channel));
		*unKeySlot = (unsigned int) BHSM_MaxCaCpKeySlot;
		return BHSM_STATUS_FAILED;
	}
	
	*unKeySlot = in_handle ->aunPidChannelToCaCpKeySlotNum[pid_channel]; 
	return BERR_SUCCESS;

#else
 	BSTD_UNUSED(in_handle);
 	BSTD_UNUSED(pid_channel);
 	BSTD_UNUSED(unKeySlot);

	BDBG_ERR(("BCHP_CHIP= %d do not support CA \n", BCHP_CHIP));
	return BHSM_STATUS_NO_HW_SUPPORT ;
#endif

}

 BERR_Code BHSM_FreeCAKeySlot(
	BHSM_Handle						in_handle,
	unsigned int	unKeySlot 
)
{
#if ( BCHP_CHIP != 7408)
	unsigned int i, value1= 0, value2 = 0;
        BHSM_ConfigPidKeyPointerTableIO_t configPidKeyPointerTableIO;
	BHSM_P_ProcessKeyIO_t processKeyIO;

	for(i = 0 ; i < BHSM_MaxPidChannelNum; i++  ) {
		if ( unKeySlot == in_handle ->aunPidChannelToCaCpKeySlotNum[i]) {
			configPidKeyPointerTableIO.unPidChannel = i ;
			configPidKeyPointerTableIO.unKeySlotNum = (unsigned int)BHSM_SLOT_NUM_INIT_VAL; /*BHSM_KEY_POINTOR_INIT_VAL;*/
			check( BHSM_ConfigPidKeyPointerTable( in_handle,&configPidKeyPointerTableIO));
		}	
	}
	
	/*Write all 0 to  Odd/Even/Clear KeyMode*/
	processKeyIO.eKeyLadderModule = BHSM_KeyLadderModule_eSecurity ;
	processKeyIO.eKeyProcess = BHSM_KeyGenProcess_eCwDirectSW;
	processKeyIO.unCustKey = 0 ;
	processKeyIO.unKeyVar = 0;
	processKeyIO.procInHigh = 0;
	processKeyIO.procInLow = 0;
	processKeyIO.eOperation = BHSM_Operation_eCA;

	for ( i = BHSM_KeyAddr_eOddKeyMode1 ; i <= BHSM_KeyAddr_eClearKeyMode2; i++ )
	{
		processKeyIO.unKeyDest = i + unKeySlot*BHSM_CA_KEYSLOT_STEPSIZE;
		BDBG_MSG(("%s - Configure Algorithem to key Destination = 0x%x !\n", __FUNCTION__, processKeyIO.unKeyDest ));	 
	
		check(BHSM_P_ProcessKey( in_handle , &processKeyIO ));
	}
		
	for ( i = BHSM_KeyType_eOddKey ; i < BHSM_KeyType_eMaxNumber; i++ ) 
	{
		in_handle->aCaCpKeySlotInfo[unKeySlot].aKeySlotAlgorithm[i].ulGlobalControlBits = 0 ;

		switch (i)
		{
			case BHSM_KeyType_eOddKey:
				value1 = BHSM_KeyAddr_eCaKeyOdd1;
                                value2 = BHSM_KeyAddr_eCpKeyOdd1;
				break;
				
			case BHSM_KeyType_eEvenKey:
				value1 = BHSM_KeyAddr_eCaKeyEven1;
				value2 = BHSM_KeyAddr_eCpKeyEven1;
				break;
				
			case BHSM_KeyType_eClearKey:
				value1 = BHSM_KeyAddr_eCaKeyOdd1;
				value2 = BHSM_KeyAddr_eCpKeyOdd1;
				break;
				
			default:
				break;
		}
	
		in_handle->aCaCpKeySlotInfo[unKeySlot].aKeySlotAlgorithm[i].ulCAControlBits = 
						( value1 << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_KEY_OFFSET_SHIFT  ) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_KEY_OFFSET_MASK ;

		in_handle->aCaCpKeySlotInfo[unKeySlot].aKeySlotAlgorithm[i].ulCPSControlBits = 	
		((XPT_SECURITY_KEY_TABLE_ALG_SEL_3DESABA << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_ALG_SEL_SHIFT  ) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_ALG_SEL_MASK)  |
		(( value2 << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_KEY_OFFSET_SHIFT  ) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_KEY_OFFSET_MASK) ;

		in_handle->aCaCpKeySlotInfo[unKeySlot].aKeySlotAlgorithm[i].ulCPDControlBits = 
		  ((XPT_SECURITY_KEY_TABLE_ALG_SEL_3DESABA << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_ALG_SEL_SHIFT  ) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_ALG_SEL_MASK)  |
		(	( value2 << XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_KEY_OFFSET_SHIFT  ) & XPT_SECURITY_KEY_TABLE_MODULE_KEY_MODE_KEY_OFFSET_MASK );
	}

	in_handle ->aCaCpKeySlotInfo[unKeySlot].bIsUsed=false;
	
	return BERR_SUCCESS;

#else
 	BSTD_UNUSED(in_handle);
 	BSTD_UNUSED(unKeySlot);

	BDBG_ERR(("BCHP_CHIP= %d do not support CA \n", BCHP_CHIP));
	return BHSM_STATUS_NO_HW_SUPPORT ;
#endif

}


BERR_Code BHSM_AllocateM2MKeySlot (
		BHSM_Handle					in_handle,
		unsigned int					*outp_unKeySlotNum
)
{
#if ( BCHP_CHIP != 7550)
	BERR_Code errCode = BERR_SUCCESS;
	unsigned int i;

	BDBG_ENTER(BHSM_AllocateM2MKeySlot);
	BDBG_ASSERT( in_handle );

	*outp_unKeySlotNum = (unsigned int)BHSM_SLOT_NUM_INIT_VAL;	
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_handle->m2mKeySlotMutexLock));
	
	/* Search for vacant key slot */
	for (i=0;	i< BHSM_MaxM2MKeySlot ; i++) {
		
		if (in_handle->aM2MKeySlotInfo[i].bIsUsed== false) {
			/* Found */
			in_handle->aM2MKeySlotInfo[i].bIsUsed= true;
			*outp_unKeySlotNum = i;
			break;
		}
	}
	
	/* Return if there is no vacant key slot or key slot type mismatched */
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, (i == BHSM_MaxM2MKeySlot));

	BDBG_MSG(("BHSM_AllocateM2MKeySlot  outp_unKeySlotNum = %d\n", *outp_unKeySlotNum));

	
BHSM_P_DONE_LABEL:

	/* Unlock mutex */
	BKNI_ReleaseMutex( in_handle->m2mKeySlotMutexLock);		
	
	BDBG_LEAVE(BHSM_AllocateM2MKeySlot);
	return( errCode );

#else	
 	BSTD_UNUSED(in_handle);
 	BSTD_UNUSED(outp_unKeySlotNum);

	BDBG_ERR(("BCHP_CHIP= %d do not support M2M DMA \n", BCHP_CHIP));
	return BHSM_STATUS_NO_HW_SUPPORT ;
#endif
}

BERR_Code BHSM_FreeM2MKeySlot (
		BHSM_Handle					in_handle,
		unsigned int					in_unKeySlotNum
)
{
#if ( BCHP_CHIP != 7550)
	BERR_Code errCode = BERR_SUCCESS;
	BHSM_P_ProcessKeyIO_t processKeyIO;

	BDBG_ENTER(BHSM_FreeM2MKeySlot);
	BDBG_ASSERT( in_handle );
	BHSM_P_CHECK_ERR_CODE_CONDITION( errCode, BHSM_STATUS_FAILED, 
		(in_handle->ulMagicNumber != BHSM_P_HANDLE_MAGIC_NUMBER ) );	

	BHSM_P_CHECK_ERR_CODE_FUNC(errCode, BKNI_AcquireMutex( in_handle->m2mKeySlotMutexLock));


	BDBG_MSG(("BHSM_FreeM2MKeySlot  key slot = %d\n", in_unKeySlotNum));	
	
	/* Return if the key slot is already empty */
	if (in_handle->aM2MKeySlotInfo[in_unKeySlotNum].bIsUsed == false)  {
		goto BHSM_P_DONE_LABEL;
	}

	/*Write all 0 to  M2M KeyMode*/
	processKeyIO.eKeyLadderModule = BHSM_KeyLadderModule_eMmscram;
	processKeyIO.eKeyProcess = BHSM_KeyGenProcess_eCwDirectSW;
	processKeyIO.unCustKey = 0 ;
	processKeyIO.unKeyVar = 0;
	processKeyIO.procInHigh = 0;
	processKeyIO.procInLow = 0;
	processKeyIO.eOperation = BHSM_Operation_eCP;

	processKeyIO.unKeyDest =  BHSM_KeyAddr_eM2MKeyMode + in_unKeySlotNum*BHSM_M2M_KEYSLOT_STEPSIZE ;
	BDBG_MSG(("%s - Configure Algorithem to key Destination = 0x%x !\n", __FUNCTION__, processKeyIO.unKeyDest ));	 
	
	check(BHSM_P_ProcessKey( in_handle , &processKeyIO ));
		
	in_handle->aM2MKeySlotInfo[in_unKeySlotNum].aKeySlotAlgorithm.ulGlobalControlBits = 0 ;
	in_handle->aM2MKeySlotInfo[in_unKeySlotNum].aKeySlotAlgorithm.ulM2MControlBits = 0 ;

	in_handle->aM2MKeySlotInfo[in_unKeySlotNum].bIsUsed = false;

	
BHSM_P_DONE_LABEL:
	
	/* Unlock mutex */
	BKNI_ReleaseMutex( in_handle->m2mKeySlotMutexLock);			
	
	BDBG_LEAVE(BHSM_FreeM2MKeySlot);
	return( errCode );

#else	
 	BSTD_UNUSED(in_handle);
 	BSTD_UNUSED(in_unKeySlotNum);

	BDBG_ERR(("BCHP_CHIP= %d do not support M2M DMA \n", BCHP_CHIP));
	return BHSM_STATUS_NO_HW_SUPPORT ;
#endif
}

