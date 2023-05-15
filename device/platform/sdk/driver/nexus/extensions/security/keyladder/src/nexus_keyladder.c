/***************************************************************************
 *     (c)2007-2012 Broadcom Corporation
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
 * $brcm_Workfile: nexus_keyladder.c $
 * $brcm_Revision: 11 $
 * $brcm_Date: 5/9/12 1:12p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /nexus/extensions/security/keyladder/7422/src/nexus_keyladder.c $
 * 
 * 11   5/9/12 1:12p atruong
 * SWSECURITY-53: Key ladder code maintenance - Disabling PKL code for Ax
 *  chips
 * 
 * 10   4/2/12 1:26p atruong
 * SW7425-2807:Add support for BSP PKL hardware
 * 
 * 9   3/28/12 2:51p atruong
 * SW7425-2749: Fix for Coverity Defect ID 37113
 * 
 * 8   3/6/12 10:03a atruong
 * SWSECURITY-113: add NEXUS_SecurityAlgorithm_eAesCounter alias
 * 
 * 7   3/5/12 1:58p atruong
 * SWSECURITY-113: Secure RSA2 Support for 40-nm Platforms
 * 
 * 5   11/8/11 11:45a atruong
 * SWSECURITY-53: Code maintenace - 40-nm Zeus 2.0 support
 * 
 * 4   10/17/11 5:48p atruong
 * SWSECURITY-69: Support for 40-nm B0 Zeus 2.0 platforms
 * 
 * 3   10/13/11 9:16a atruong
 * SWSECURITY-69: BSP Zeus 2.0 FW Support
 * 
 * 1   1/28/11 7:08p atruong
 * SW7422-235: Enable Nexus Security extension modules for 97422
 *
 ******************************************************************************/


#include "nexus_security_module.h"
#include "nexus_keyladder.h"
#include "nexus_security_datatypes.h"
#include "nexus_security.h"
#include "priv/nexus_security_priv.h"
#include "priv/nexus_core.h"

#include "bhsm.h"
#include "bsp_s_commands.h"
#include "bsp_s_misc.h"
#include "bhsm_misc.h"
#include "bsp_s_hw.h"
#include "bsp_s_keycommon.h"
#include "bhsm_keyladder.h"
#include "bhsm_otpmsp.h"
#include "bhsm_user_cmds.h"
#include "bsp_s_hmac_sha1.h"



BDBG_MODULE(nexus_security);




void NEXUS_Security_GetDefaultSessionKeySettings(
    NEXUS_SecurityEncryptedSessionKey  *pSettings    
    )
{

    if (pSettings)
    {
        BKNI_Memset(pSettings, 0, sizeof(*pSettings));
		pSettings->keyGenCmdID     = NEXUS_SecurityKeyGenCmdID_eKeyGen;
		pSettings->keyladderType   = NEXUS_SecurityKeyladderType_e3Des;
#if HSM_IS_ASKM_40NM_ZEUS_2_0
		pSettings->keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eFWKL;
		pSettings->bkeyGenBlocked  = false;
		pSettings->rootKeySwap     = false;
#endif
		pSettings->bRouteKey       = false;
		pSettings->bSwapAESKey     = false;
	
    }

}


void NEXUS_Security_GetDefaultControlWordSettings(
    NEXUS_SecurityEncryptedControlWord  *pSettings  
    )
{

	if (pSettings)
	{
		BKNI_Memset(pSettings, 0, sizeof(*pSettings));
		pSettings->keyGenCmdID	   = NEXUS_SecurityKeyGenCmdID_eKeyGen;
		pSettings->keyladderType   = NEXUS_SecurityKeyladderType_e3Des;
#if HSM_IS_ASKM_40NM_ZEUS_2_0
		pSettings->keyLadderSelect = NEXUS_SecurityKeyLadderSelect_eFWKL;
		pSettings->bkeyGenBlocked  = false;
		pSettings->rootKeySwap	   = false;
		pSettings->sc01Polarity[NEXUS_SecurityPacketType_eGlobal] =
				NEXUS_SecurityAlgorithmScPolarity_eClear;
		pSettings->sc01Polarity[NEXUS_SecurityPacketType_eRestricted] =
				NEXUS_SecurityAlgorithmScPolarity_eClear;
#endif
		pSettings->bRouteKey	   = false;
		pSettings->bSwapAESKey	   = false;
	
	}

}







static NEXUS_Error NEXUS_Security_GetHsmDestBlkType(
    NEXUS_KeySlotHandle                      keyHandle,
	NEXUS_SecurityAlgorithmConfigDestination dest, 
	BCMD_KeyDestBlockType_e                  *pType
	)
{
    switch (keyHandle->cryptoEngine) 
	{
	    case NEXUS_SecurityEngine_eCa:
	        *pType = BCMD_KeyDestBlockType_eCA;
	        break;
	    case NEXUS_SecurityEngine_eM2m:
	        *pType = BCMD_KeyDestBlockType_eMem2Mem;
	        break;
	    case NEXUS_SecurityEngine_eCaCp:
			if (dest == NEXUS_SecurityAlgorithmConfigDestination_eCa)
				*pType = BCMD_KeyDestBlockType_eCA;
			else if (dest == NEXUS_SecurityAlgorithmConfigDestination_eCpd)
				*pType = BCMD_KeyDestBlockType_eCPDescrambler;
			else if (dest == NEXUS_SecurityAlgorithmConfigDestination_eCps)
				*pType = BCMD_KeyDestBlockType_eCPScrambler;
	        break;
	    case NEXUS_SecurityEngine_eCp:
			if (dest == NEXUS_SecurityAlgorithmConfigDestination_eCpd)
				*pType = BCMD_KeyDestBlockType_eCPDescrambler;
			else if (dest == NEXUS_SecurityAlgorithmConfigDestination_eCps)
				*pType = BCMD_KeyDestBlockType_eCPScrambler;

	        break;
	    case NEXUS_SecurityEngine_eRmx:
	        BDBG_ERR(("Remux is not supported on 40nm HSM"));
	        *pType = BCMD_KeyDestBlockType_eCA;
	        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
	    default:
	        /* There is no meaningful default, error. */
	        return BERR_TRACE(NEXUS_NOT_SUPPORTED);
	        break;
    }

    return NEXUS_SUCCESS;

}



static NEXUS_Error NEXUS_Security_GetHsmDestEntrykType(
	NEXUS_SecurityKeyType   keytype, 
	BCMD_KeyDestEntryType_e *pType
	)
{
    switch (keytype) 
	{
	    case NEXUS_SecurityKeyType_eEven:
	        *pType = BCMD_KeyDestEntryType_eEvenKey;
	        break;
	    case NEXUS_SecurityKeyType_eOdd:
	        *pType = BCMD_KeyDestEntryType_eOddKey;
	        break;
	    case NEXUS_SecurityKeyType_eClear:
	        *pType = BCMD_KeyDestEntryType_eReserved2;
	        break;
	    default:
	        *pType = BCMD_KeyDestEntryType_eOddKey;
	        break;
    }
    return NEXUS_SUCCESS;
}

static NEXUS_Error NEXUS_Security_GetHsmDestIVType(
	NEXUS_SecurityKeyIVType keyIVtype, 
	BCMD_KeyDestIVType_e    *pType
	)
{
    switch (keyIVtype) 
	{
	    case NEXUS_SecurityKeyIVType_eNoIV:
	        *pType = BCMD_KeyDestIVType_eNoIV;
	        break;
	    case NEXUS_SecurityKeyIVType_eIV:
	        *pType = BCMD_KeyDestIVType_eIV;
	        break;
	    case NEXUS_SecurityKeyIVType_eAesShortIV:
	        *pType = BCMD_KeyDestIVType_eAesShortIV;
	        break;
	    default:
	        *pType = BCMD_KeyDestIVType_eNoIV;
	        break;
    }
    return NEXUS_SUCCESS;
}


static NEXUS_Error NEXUS_Security_GetKeyLadderType(
	NEXUS_SecurityKeyladderType klType, 
	BCMD_KeyLadderType_e        *pType
	)
{
    switch (klType) 
	{
	    case NEXUS_SecurityKeyladderType_e1Des:
	        *pType = BCMD_KeyLadderType_e1DES;
	        break;
	    case NEXUS_SecurityKeyladderType_e3Des:
	        *pType = BCMD_KeyLadderType_e3DESABA;
	        break;
	    case NEXUS_SecurityKeyladderType_eAes128:
	        *pType = BCMD_KeyLadderType_eAES128;
	        break;
#if HSM_IS_ASKM_40NM_ZEUS_2_0
#if NEXUS_SUPPORT_HDDTA
		case NEXUS_SecurityKeyladderType_ePKSM:
			*pType = BCMD_KeyLadderType_ePKSM;
			break;
#endif
#endif
	    default:
	        *pType = BCMD_KeyLadderType_e3DESABA;
	        break;
    }
    return NEXUS_SUCCESS;
}


static NEXUS_Error NEXUS_Security_GetRootKeySource(
	NEXUS_SecurityRootKeySrc    ksType, 
	BCMD_RootKeySrc_e           *pType
	)
{
    switch (ksType)
    {
	    case NEXUS_SecurityRootKeySrc_eOtpKeyA:
	        *pType = BCMD_RootKeySrc_eOTPKeya;
	        break;
	    case NEXUS_SecurityRootKeySrc_eOtpKeyB:
	        *pType = BCMD_RootKeySrc_eOTPKeyb;
	        break;
	    case NEXUS_SecurityRootKeySrc_eOtpKeyC:
	        *pType = BCMD_RootKeySrc_eOTPKeyc;
	        break;
		case NEXUS_SecurityRootKeySrc_eOtpKeyD:
			*pType = BCMD_RootKeySrc_eOTPKeyd;
			break;
		case NEXUS_SecurityRootKeySrc_eOtpKeyE:
			*pType = BCMD_RootKeySrc_eOTPKeye;
			break;
		case NEXUS_SecurityRootKeySrc_eOtpKeyF:
			*pType = BCMD_RootKeySrc_eOTPKeyf;
			break;
	    case NEXUS_SecurityRootKeySrc_eCuskey:
	    default:
	        *pType = BCMD_RootKeySrc_eCusKey;
	        break;
    }

    return NEXUS_SUCCESS;
}


static NEXUS_Error NEXUS_Security_GetSwizzleType(
	NEXUS_SecuritySwizzleType   swType, 
	BCMD_SwizzleType_e          *pType
	)
{
    switch (swType) 
	{
	    case NEXUS_SecuritySwizzleType_eNone:
	        *pType = BCMD_SwizzleType_eNoSwizzle;
	        break;
	    case NEXUS_SecuritySwizzleType_eSwizzle1:
	        *pType = BCMD_SwizzleType_eSwizzle1;
	        break;
	    case NEXUS_SecuritySwizzleType_eSwizzle0:
	        *pType = BCMD_SwizzleType_eSwizzle0;
	        break;
	    default:
	        *pType = BCMD_SwizzleType_eNoSwizzle;
	        break;
    }
    return NEXUS_SUCCESS;
}


#if HSM_IS_ASKM_40NM_ZEUS_2_0

static BCMD_XptM2MSecCryptoAlg_e NEXUS_Security_MapNexusAlgorithmToHsm(NEXUS_SecurityAlgorithm algorithm)
{
    BCMD_XptM2MSecCryptoAlg_e rvAlgorithm = algorithm;

    switch (algorithm)
    {
	    case NEXUS_SecurityAlgorithm_eMulti2:
	        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eMulti2;
	        break;
	    case NEXUS_SecurityAlgorithm_eDes:
	        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eDes;
	        break;
	    case NEXUS_SecurityAlgorithm_e3DesAba:
	        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_e3DesAba;
	        break;
	    case NEXUS_SecurityAlgorithm_e3DesAbc:
	        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_e3DesAbc;
	        break;
	    case NEXUS_SecurityAlgorithm_eAes:
	        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eAes128;
	        break;
	    case NEXUS_SecurityAlgorithm_eAes192:
	        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eAes192;
	        break;
	    case NEXUS_SecurityAlgorithm_eC2:
	        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eC2;
	        break;
	    case NEXUS_SecurityAlgorithm_eCss:
	        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eCss;
	        break;
	    case NEXUS_SecurityAlgorithm_eM6Ke:
	        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eM6KE;
	        break;
	    case NEXUS_SecurityAlgorithm_eM6:
	        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eM6;
	        break;
	    case NEXUS_SecurityAlgorithm_eWMDrmPd:
	        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eWMDrmPd;
	        break;

	    case NEXUS_SecurityAlgorithm_eDvbCsa3:
	        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eDVBCSA3;
	        break;
	    case NEXUS_SecurityAlgorithm_eAesCounter:
#if HSM_IS_ASKM_40NM_ZEUS_2_5
		rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eAesCounter0;
#else
		rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eASF;
#endif
		break;
	    case NEXUS_SecurityAlgorithm_eMSMultiSwapMac:
	        rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eMSMULTISWAPMAC;
	        break;
#if HSM_IS_ASKM_40NM_ZEUS_2_0
        case NEXUS_SecurityAlgorithm_eAsa:
            rvAlgorithm = BCMD_XptM2MSecCryptoAlg_eASA;
            break;
#endif

    /* The _eReservedX values should pass the literal value X into HSM, 
     * allowing direct control of custom modes
     * Macro trickery to avoid copy/paste errors */
#define NEXUS_REMAP_RESERVED(VAL) \
	    case NEXUS_SecurityAlgorithm_eReserved##VAL : \
	        rvAlgorithm = (BCMD_XptM2MSecCryptoAlg_e) VAL ; \
	        break;
	        NEXUS_REMAP_RESERVED(0)
	        NEXUS_REMAP_RESERVED(1)
	        NEXUS_REMAP_RESERVED(2)
	        NEXUS_REMAP_RESERVED(3)
	        NEXUS_REMAP_RESERVED(4)
	        NEXUS_REMAP_RESERVED(5)
	        NEXUS_REMAP_RESERVED(6)
	        NEXUS_REMAP_RESERVED(7)
	        NEXUS_REMAP_RESERVED(8)
#undef NEXUS_REMAP_RESERVED
	    default:
	        break;
    }

    return rvAlgorithm;
}

#endif

static void NEXUS_Security_InitGenerateRouteKey(
    BHSM_GenerateRouteKeyIO_t   * generateRouteKeyPtr,
    BCMD_KeyLadderType_e klType,    BCMD_RootKeySrc_e rootKsrc,
    BCMD_SwizzleType_e swizzleType, unsigned char kLow, unsigned char kLowV, unsigned char kHigh, unsigned char kHighV,
    BCMD_KeyRamBuf_e kDest,
    BCMD_KeySize_e kSize, bool routeFlag, BCMD_KeyDestEntryType_e oddEvenEntry, unsigned int slotNum,
    bool useCustLow,
        bool useCustHigh,
        bool operateCrption,
        BCMD_KeyDestBlockType_e blockType
    )
{
    BKNI_Memset(generateRouteKeyPtr, 0, sizeof(BHSM_GenerateRouteKeyIO_t));

    generateRouteKeyPtr->keyLadderType = klType; /* mostly fixed to BCMD_KeyLadderType_e3DESABA */
    generateRouteKeyPtr->rootKeySrc = rootKsrc;
    generateRouteKeyPtr->ucSwizzle1Index = 0;         /*fixed */

    generateRouteKeyPtr->swizzleType = swizzleType; /* valid only if rootkeysrc is custkey */
    if (swizzleType == BCMD_SwizzleType_eSwizzle1){
        generateRouteKeyPtr->ucSwizzle1Index = kLow;   /* swizzle1 needs a valid index, re-define kLow*/
    }else if (swizzleType == BCMD_SwizzleType_eSwizzle0) {
        generateRouteKeyPtr->ucCustKeyLow = kLow;
        generateRouteKeyPtr->ucKeyVarLow= kLowV;
        generateRouteKeyPtr->ucCustKeyHigh = kHigh;
        generateRouteKeyPtr->ucKeyVarHigh= kHighV;

    } else{}
    generateRouteKeyPtr->bUseCustKeyLowDecrypt   =  useCustLow;
    generateRouteKeyPtr->bUseCustKeyHighDecrypt  = useCustHigh;

	generateRouteKeyPtr->keyLayer                = kDest;
	generateRouteKeyPtr->bIsKeyLadder3DESEncrypt = operateCrption; 

    /*BKNI_Memcpy(generateRouteKeyPtr->aucKeyData, aucKeyData[0], 24); */

    generateRouteKeyPtr->keySize= kSize;

    generateRouteKeyPtr->bIsRouteKeyRequired = routeFlag;
    generateRouteKeyPtr->keyDestBlckType = blockType;       /*fixed */
    generateRouteKeyPtr->keyDestEntryType = oddEvenEntry;
    generateRouteKeyPtr->caKeySlotType = 0;                  /*no matter for m2m */
    generateRouteKeyPtr->unKeySlotNum =  slotNum;

}

#if HSM_IS_ASKM_40NM_ZEUS_2_0
static int g_scValues[NEXUS_SecurityAlgorithmScPolarity_eMax] = { 0, 1, 2, 3 };
#endif


NEXUS_Error NEXUS_Security_GenerateSessionKey(
    NEXUS_KeySlotHandle keyHandle,
    const NEXUS_SecurityEncryptedSessionKey *pSessionKey
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BCMD_KeyDestBlockType_e blockType = BCMD_KeyDestBlockType_eCA;
    BCMD_KeyDestEntryType_e entryType = BCMD_KeyDestEntryType_eOddKey;
	BCMD_KeyDestIVType_e    ivType    = BCMD_KeyDestIVType_eIV;
    BHSM_GenerateRouteKeyIO_t   generateRouteKeyIO;
    bool bDecrypt;
    bool bKey2Decrypt;
    BCMD_RootKeySrc_e rootkeySrc = BCMD_RootKeySrc_eCusKey;
    BCMD_KeyRamBuf_e keyladderID;
    BHSM_Handle hHsm;
	BCMD_KeyLadderType_e    keyLadderType;
	BCMD_SwizzleType_e      swizzleType;


    NEXUS_Security_GetHsm_priv(&hHsm);
    if (!hHsm) 
		return NEXUS_INVALID_PARAMETER;

	keyladderID = BCMD_KeyRamBuf_eKey3;


    NEXUS_Security_GetHsmDestBlkType (keyHandle, pSessionKey->dest, &blockType);
    NEXUS_Security_GetHsmDestEntrykType(pSessionKey->keyEntryType, &entryType);
	NEXUS_Security_GetHsmDestIVType(pSessionKey->keyDestIVType, &ivType);
    bDecrypt = (pSessionKey->operation==NEXUS_SecurityOperation_eDecrypt)? false: true;
    bKey2Decrypt = (pSessionKey->operationKey2==NEXUS_SecurityOperation_eDecrypt)? true: false;

	NEXUS_Security_GetKeyLadderType(pSessionKey->keyladderType, &keyLadderType);
	NEXUS_Security_GetRootKeySource(pSessionKey->rootKeySrc, &rootkeySrc);
	NEXUS_Security_GetSwizzleType(pSessionKey->swizzleType, &swizzleType);
    NEXUS_Security_InitGenerateRouteKey( &generateRouteKeyIO,
                keyLadderType,
                rootkeySrc,
                swizzleType,
                pSessionKey->cusKeyL, pSessionKey->cusKeyVarL, pSessionKey->cusKeyH, pSessionKey->cusKeyVarH,
                keyladderID,
                BCMD_KeySize_e128,
                pSessionKey->bRouteKey,
                entryType,
                keyHandle->keySlotNumber,
                bKey2Decrypt,
                bKey2Decrypt,
                bDecrypt,
                blockType);

#if HSM_IS_ASKM_40NM_ZEUS_2_0
	generateRouteKeyIO.keyLadderSelect              = pSessionKey->keyLadderSelect;
	generateRouteKeyIO.bASKM3DesKLRootKeySwapEnable = pSessionKey->rootKeySwap;
	generateRouteKeyIO.keyGenMode                   = pSessionKey->bkeyGenBlocked;

	/* For Hardware Key Ladder */
	generateRouteKeyIO.hwklLength                   = (BCMD_HwKeyLadderLength_e)pSessionKey->hwklLen;
	generateRouteKeyIO.hwklDestAlg                  = NEXUS_Security_MapNexusAlgorithmToHsm(pSessionKey->hwklDestAlg);
    BKNI_Memset(generateRouteKeyIO.activationCode, 0, NEXUS_SECURITY_KL_ACTCODE_SIZE);
    BKNI_Memcpy(generateRouteKeyIO.activationCode, pSessionKey->actCode, NEXUS_SECURITY_KL_ACTCODE_SIZE);

#endif

#if defined(HUMAX_PLATFORM_BASE)
	if (pSessionKey->virtualKeyLadderID == NEXUS_SecurityVirtualKeyladderID_eMax)
		generateRouteKeyIO.virtualKeyLadderID = BHSM_MAX_VKL;
	else
		generateRouteKeyIO.virtualKeyLadderID = pSessionKey->virtualKeyLadderID;
#else
	generateRouteKeyIO.virtualKeyLadderID = pSessionKey->virtualKeyLadderID;
#endif		
	generateRouteKeyIO.customerSubMode    = pSessionKey->custSubMode;
	generateRouteKeyIO.keyMode            = pSessionKey->keyMode;
    generateRouteKeyIO.ucKeyDataLen       = 16;
    BKNI_Memset(generateRouteKeyIO.aucKeyData, 0, 16);
    BKNI_Memcpy(generateRouteKeyIO.aucKeyData, pSessionKey->keyData, 16);

    generateRouteKeyIO.unKeySlotNum     = keyHandle->keySlotNumber;
    generateRouteKeyIO.caKeySlotType    = keyHandle->keyslotType;
	generateRouteKeyIO.bASKMModeEnabled = pSessionKey->bASKMMode;
	generateRouteKeyIO.bSwapAesKey      = pSessionKey->bSwapAESKey;
	generateRouteKeyIO.key3Op           = pSessionKey->sessionKeyOp;
	generateRouteKeyIO.keyDestIVType    = ivType;
	generateRouteKeyIO.subCmdID         = pSessionKey->keyGenCmdID;
	
    rc= BHSM_GenerateRouteKey (hHsm, &generateRouteKeyIO);
    if (rc != 0) {
        BDBG_ERR(("Generate route Key 3 failed\n"));
        return NEXUS_INVALID_PARAMETER;
    }

    return NEXUS_SUCCESS;
}




static NEXUS_Error NEXUS_Security_GenerateKey4or5(
    NEXUS_KeySlotHandle keyHandle,
    const NEXUS_SecurityEncryptedControlWord * pCW,
    bool bKey4
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    BCMD_KeyDestBlockType_e blockType = BCMD_KeyDestBlockType_eCA;
    BCMD_KeyDestEntryType_e entryType = BCMD_KeyDestEntryType_eOddKey;
	BCMD_KeyDestIVType_e    ivType    = BCMD_KeyDestIVType_eIV;
    BHSM_GenerateRouteKeyIO_t   generateRouteKeyIO;
    bool bDecrypt;
    BCMD_KeySize_e keySize;
    BCMD_KeyRamBuf_e keyladderID;
    BHSM_Handle hHsm;
    BCMD_RootKeySrc_e rootkeySrc = BCMD_RootKeySrc_eCusKey;
	BCMD_KeyLadderType_e    keyLadderType;


    NEXUS_Security_GetHsm_priv(&hHsm);
    if (!hHsm) 
		return NEXUS_INVALID_PARAMETER;

    if ( pCW->keySize==8 )
        keySize = BCMD_KeySize_e64;
    else if ( pCW->keySize==16 )
        keySize = BCMD_KeySize_e128;
    else
        keySize = BCMD_KeySize_e192;

#if HSM_IS_ASKM_40NM_ZEUS_2_0
	if(pCW->keyLadderSelect == NEXUS_SecurityKeyLadderSelect_ePKL)
		keyladderID = BCMD_KeyRamBuf_ePKL;
	else
	{
		keyladderID = BCMD_KeyRamBuf_eKey4;
        if ( !bKey4 )
            keyladderID ++;
	}
#else
	keyladderID = BCMD_KeyRamBuf_eKey4;
	if ( !bKey4 )
		keyladderID ++;

#endif

    NEXUS_Security_GetHsmDestBlkType (keyHandle, pCW->dest, &blockType);
    NEXUS_Security_GetHsmDestEntrykType(pCW->keyEntryType, &entryType);
	NEXUS_Security_GetHsmDestIVType(pCW->keyDestIVType, &ivType);
    bDecrypt = (pCW->operation==NEXUS_SecurityOperation_eDecrypt)? false: true;
	NEXUS_Security_GetKeyLadderType(pCW->keyladderType, &keyLadderType);
	NEXUS_Security_GetRootKeySource(pCW->rootKeySrc, &rootkeySrc);
    NEXUS_Security_InitGenerateRouteKey( &generateRouteKeyIO,
                keyLadderType,
                0,
                0,
                0, 0, 0, 0,
                keyladderID,
                keySize,
                pCW->bRouteKey,
                entryType,
                keyHandle->keySlotNumber,
                bDecrypt,
                bDecrypt,
                bDecrypt,
                blockType);

	generateRouteKeyIO.rootKeySrc         = rootkeySrc;             /* need this for ASKM keyladder distinction */
#if defined(HUMAX_PLATFORM_BASE)
	if (pCW->virtualKeyLadderID == NEXUS_SecurityVirtualKeyladderID_eMax)
		generateRouteKeyIO.virtualKeyLadderID = BHSM_MAX_VKL;
	else
	generateRouteKeyIO.virtualKeyLadderID = pCW->virtualKeyLadderID;
#else
	generateRouteKeyIO.virtualKeyLadderID = pCW->virtualKeyLadderID;
#endif		
	generateRouteKeyIO.customerSubMode	  = pCW->custSubMode;
	generateRouteKeyIO.keyMode			  = pCW->keyMode;
    generateRouteKeyIO.ucKeyDataLen       = pCW->keySize;
    BKNI_Memset(generateRouteKeyIO.aucKeyData, 0,  pCW->keySize);
    BKNI_Memcpy(generateRouteKeyIO.aucKeyData, pCW->keyData,  pCW->keySize);

#if HSM_IS_ASKM_40NM_ZEUS_2_0
	generateRouteKeyIO.keyLadderSelect				= pCW->keyLadderSelect;
	generateRouteKeyIO.bASKM3DesKLRootKeySwapEnable = pCW->rootKeySwap;
	generateRouteKeyIO.GpipeSC01Val 				= g_scValues[pCW->sc01Polarity[NEXUS_SecurityPacketType_eGlobal]];
	generateRouteKeyIO.RpipeSC01Val 				= g_scValues[pCW->sc01Polarity[NEXUS_SecurityPacketType_eRestricted]];
	generateRouteKeyIO.keyGenMode					= pCW->bkeyGenBlocked;

	/* For Hardware Key Ladder */
	generateRouteKeyIO.hwklLength					= (BCMD_HwKeyLadderLength_e)pCW->hwklLen;
	generateRouteKeyIO.hwklDestAlg					= NEXUS_Security_MapNexusAlgorithmToHsm(pCW->hwklDestAlg);
	BKNI_Memset(generateRouteKeyIO.activationCode, 0, NEXUS_SECURITY_KL_ACTCODE_SIZE);
	BKNI_Memcpy(generateRouteKeyIO.activationCode, pCW->actCode, NEXUS_SECURITY_KL_ACTCODE_SIZE);
	
#endif


    generateRouteKeyIO.unKeySlotNum     = keyHandle->keySlotNumber;
    generateRouteKeyIO.caKeySlotType    = keyHandle->keyslotType;
	generateRouteKeyIO.bASKMModeEnabled = pCW->bASKMMode;
	generateRouteKeyIO.bSwapAesKey      = pCW->bSwapAESKey;
	generateRouteKeyIO.keyDestIVType    = ivType;

#if HSM_IS_ASKM_40NM_ZEUS_2_0
	generateRouteKeyIO.subCmdID         = (BCMD_VKLAssociationQueryFlag_e)pCW->keyGenCmdID;
#else
	generateRouteKeyIO.subCmdID 		= pCW->keyGenCmdID;
#endif

    rc= BHSM_GenerateRouteKey (hHsm, &generateRouteKeyIO);
    if (rc != 0) {
        BDBG_ERR(("Generate route Key 4 or 5 failed\n"));
        return NEXUS_INVALID_PARAMETER;
    }

    return NEXUS_SUCCESS;
}



NEXUS_Error NEXUS_Security_PklInit(void)
{

#if HSM_IS_ASKM_40NM_ZEUS_2_0
    NEXUS_Error rc = NEXUS_SUCCESS;
    BHSM_PklCmdIO_t pklInitIO;
    BHSM_ProgramOtpPatSeqIO_t progOtpPatSeqIO;
    BHSM_Handle hHsm;

    NEXUS_Security_GetHsm_priv(&hHsm);
    if (!hHsm) 
		return NEXUS_INVALID_PARAMETER;

    pklInitIO.pklSubCmdId = BCMD_PKL_SubCmd_ePklInit;

	
    rc= BHSM_PklInit (hHsm, &pklInitIO);
    if (rc != 0) {
        BDBG_ERR(("PKL Init failed\n"));
        return NEXUS_INVALID_PARAMETER;
    }

    BKNI_Memset(&progOtpPatSeqIO, 0, sizeof(BHSM_ProgramOtpPatSeqIO_t));

     rc = BHSM_ProgramOTPPatternSequence(hHsm, &progOtpPatSeqIO);	
	 
    if (rc != 0) {
	     rc = BHSM_ProgramOTPPatternSequence(hHsm, &progOtpPatSeqIO);
		 
	    if (rc != 0) {
	        BDBG_ERR(("Pattern Command failed\n"));
	        return NEXUS_INVALID_PARAMETER;
    	}	

    }	

#endif	
    return NEXUS_SUCCESS;
}



NEXUS_Error NEXUS_Security_GenerateControlWord(
    NEXUS_KeySlotHandle keyHandle,
    const NEXUS_SecurityEncryptedControlWord * pCW
    )
{
    return NEXUS_Security_GenerateKey4or5 (keyHandle, pCW, true);
}

NEXUS_Error NEXUS_Security_GenerateKey5(
    NEXUS_KeySlotHandle keyHandle,
    const NEXUS_SecurityEncryptedControlWord * pCW
    )
{
    return NEXUS_Security_GenerateKey4or5 (keyHandle, pCW, false);
}



