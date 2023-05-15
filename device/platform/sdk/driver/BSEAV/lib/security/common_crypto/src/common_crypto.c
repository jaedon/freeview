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
 * $brcm_Workfile: common_crypto.c $
 * $brcm_Revision: 5 $
 * $brcm_Date: 7/18/12 12:51p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/security/common_crypto/src/common_crypto.c $
 * 
 * 5   7/18/12 12:51p cdisc
 * SW7425-3487: adding more flexibility to common crypto
 * 
 * 4   6/21/12 11:47a hugost
 * SW7425-3300: Defaulted keySlotType to odd in
 * CommonCrypto_GetDefaultKeySettings().
 * 
 * 3   5/15/12 12:58p cdisc
 * SW7125-1289: updating to support new common crypto
 * 
 * 2   5/10/12 6:06p ajutras
 * SW7125-1288: Add support for Comcast key and certificate protection
 * 
 * 1   4/24/12 11:10a cdisc
 * SW7425-2923: adding common_crypto dir and sources
 * 
 * SW7425-2317/2   4/12/12 9:37a hugost
 * Added missing header file
 * 
 * SW7425-2317/1   4/9/12 10:22a hugost
 * Source file.
 * 
 * SW7425-2717/1   3/23/12 10:44a hugost
 * Nexus common crypto.
 * 
 * 
 *
 **************************************************************************/
#if (BCHP_CHIP == 7125)
#include <string.h>
#endif

#include "common_crypto.h"

#include "nexus_keyladder.h"
#include "bkni.h"

BDBG_MODULE(common_crypto);

typedef struct CommonCrypto
{
    BDBG_OBJECT(CommonCrypto)

    NEXUS_DmaHandle dmaHandle; /* Dma handle*/
    CommonCryptoSettings settings;

}CommonCrypto;

static NEXUS_Error CommonCrypto_LoadClearKey_priv(NEXUS_KeySlotHandle keySlot,
                                                  const uint8_t  *pKey,
                                                  uint32_t  keySize,
                                                  NEXUS_SecurityKeyType keySlotType,
                                                  NEXUS_SecurityKeyIVType keyIvType);

static NEXUS_Error CommonCrypto_LoadCipheredKey_priv(NEXUS_KeySlotHandle keySlot,
                                                     NEXUS_SecurityRootKeySrc keySrc,
                                                     NEXUS_SecurityKeyType keySlotType,
                                                     const CommonCryptoKeyLadderSettings   *pKeyLadderInfo);


void CommonCrypto_GetDefaultSettings(
    CommonCryptoSettings *pSettings
    )
{
    BDBG_ASSERT(pSettings != NULL);
    BKNI_Memset(&pSettings->dmaSettings, 0, sizeof(NEXUS_DmaSettings));
}

void CommonCrypto_GetDefaultCipheredKeySettings(
    CommonCryptoCipheredKeySettings *pSettings    /* [out] default settings */
    )
{
    BDBG_ASSERT(pSettings != NULL);
    BKNI_Memset(pSettings, 0, sizeof(CommonCryptoCipheredKeySettings));
    CommonCrypto_GetDefaultKeyLadderSettings(&pSettings->settings);
    pSettings->keySlotType = NEXUS_SecurityKeyType_eOdd;
}


void CommonCrypto_GetDefaultClearKeySettings(
    CommonCryptoClearKeySettings *pSettings    /* [out] default settings */
    )
{
    BDBG_ASSERT(pSettings != NULL);
    BKNI_Memset(pSettings, 0, sizeof(CommonCryptoClearKeySettings));
    pSettings->keySlotType = NEXUS_SecurityKeyType_eOdd;
}


void CommonCrypto_GetDefaultKeyConfigSettings(
    CommonCryptoKeyConfigSettings *pSettings    /* [out] default settings */
    )
{
    BDBG_ASSERT(pSettings != NULL);
    BKNI_Memset(pSettings, 0, sizeof(CommonCryptoKeyConfigSettings));
    pSettings->settings.keySlotType = NEXUS_SecurityKeyType_eOdd;
}

void CommonCrypto_GetDefaultKeySettings(
    CommonCryptoKeySettings *pSettings,    /* [out] default settings */
    CommonCryptoKeySrc keySrc
    )
{
    BDBG_ASSERT(pSettings != NULL);
    BKNI_Memset(pSettings, 0, sizeof(CommonCryptoKeySettings));
    pSettings->alg.keySlotType = NEXUS_SecurityKeyType_eOdd;
    if(keySrc != CommonCrypto_eClearKey){
    	CommonCrypto_GetDefaultKeyLadderSettings((CommonCryptoKeyLadderSettings*)&pSettings->src);
    }
}

CommonCryptoHandle CommonCrypto_Open(
    const CommonCryptoSettings *pSettings
    )
{
    NEXUS_Error rc;
    CommonCryptoHandle  pCrypto;
    CommonCryptoSettings defaultSettings;

    if (!pSettings) {
        CommonCrypto_GetDefaultSettings(&defaultSettings);
        pSettings = &defaultSettings;
    }

    pCrypto = BKNI_Malloc(sizeof(*pCrypto));
    if (!pCrypto) {
        rc = BERR_TRACE(NEXUS_OUT_OF_SYSTEM_MEMORY);
        return NULL;
    }

    BKNI_Memset(pCrypto, 0, sizeof(*pCrypto));

    /* DMA is virtualized, hence it can be opened multiple times */
    pCrypto->dmaHandle = NEXUS_Dma_Open(0, &pSettings->dmaSettings);
    if(pCrypto->dmaHandle == NULL)
    {
        rc = BERR_TRACE(NEXUS_NOT_AVAILABLE);
        BKNI_Free(pCrypto);
        return NULL;
    }

    BKNI_Memcpy(&pCrypto->settings, pSettings, sizeof(*pSettings));

    return pCrypto;
}

void CommonCrypto_Close(
    CommonCryptoHandle handle
    )
{

    NEXUS_Dma_Close(handle->dmaHandle);

    BKNI_Free(handle);
}

/***************************************************************************
Summary:
Get default key ladder operation settings.

Description:
This function is used to initialize a CommonCryptoKeyLadderOperationStruct structure
with default settings used during the key3 and key4 generation.

***************************************************************************/
void CommonCrypto_GetDefaultKeyLadderSettings(
		CommonCryptoKeyLadderSettings *pSettings)
{
	BDBG_MSG(("%s - Entered function", __FUNCTION__));
	pSettings->overwriteKeyLadderOperation = false;
	pSettings->overwriteVKLSettings = false;
	pSettings->KeyLadderOpStruct.SessionKeyOperation = NEXUS_SecurityOperation_eDecrypt;
	pSettings->KeyLadderOpStruct.SessionKeyOperationKey2 = NEXUS_SecurityOperation_eEncrypt;
	pSettings->KeyLadderOpStruct.ControlWordKeyOperation = NEXUS_SecurityOperation_eDecrypt;
	pSettings->VirtualKeyLadderSettings.CustSubMode = NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;
	pSettings->VirtualKeyLadderSettings.VklValue = NEXUS_SecurityVirtualKeyladderID_eVKL6;
	BDBG_MSG(("%s - Exiting function", __FUNCTION__));
	return;
}



void CommonCrypto_GetSettings(
    CommonCryptoHandle handle,
    CommonCryptoSettings *pSettings
    )
{
    BDBG_ASSERT(pSettings != NULL);

    *pSettings = handle->settings;
    return;
}

NEXUS_Error CommonCrypto_SetSettings(
    CommonCryptoHandle handle,
    const CommonCryptoSettings *pSettings
    )
{
    NEXUS_Error rc;

    BDBG_ASSERT(pSettings != NULL);

    rc =  NEXUS_Dma_SetSettings(handle->dmaHandle, &pSettings->dmaSettings);

    return rc;
}


void CommonCrypto_GetDefaultJobSettings(
    CommonCryptoJobSettings *pSettings /* [out] */
    )
{
    BDBG_ASSERT(NULL != pSettings);
    BKNI_Memset(pSettings, 0, sizeof(*pSettings));
    pSettings->dataFormat = NEXUS_DmaDataFormat_eBlock;

    return;
}

NEXUS_Error CommonCrypto_DmaXfer(
    CommonCryptoHandle handle,
    const CommonCryptoJobSettings *pJobSettings,
    const NEXUS_DmaJobBlockSettings *pBlkSettings,
    unsigned nBlocks
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    
    NEXUS_DmaJobSettings jobSettings;
    NEXUS_DmaJobHandle dmaJob = NULL;
    NEXUS_DmaJobStatus jobStatus;

    BDBG_ASSERT(pBlkSettings != NULL);
    BDBG_ASSERT(pJobSettings != NULL);

    BDBG_MSG(("CommonCrypto_DmaXfer: Enter\n"));
    BDBG_MSG(("%s - Entered function using keySlot '%p'  nBlocks = '%u'", __FUNCTION__, pJobSettings->keySlot, nBlocks));

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks                   = nBlocks;
    jobSettings.keySlot                     = pJobSettings->keySlot;
    jobSettings.dataFormat                  = pJobSettings->dataFormat;
    jobSettings.completionCallback.callback = NULL;

    dmaJob = NEXUS_DmaJob_Create(handle->dmaHandle, &jobSettings);
    if(dmaJob == NULL){
        BDBG_ERR(("%s - NEXUS_DmaJob_Create failed\n", __FUNCTION__));
        return BERR_TRACE(NEXUS_NOT_AVAILABLE);
    }

    /* We can save one context switch by calling NEXUS_DmaJob_ProcessBlocks_priv instead of NEXUS_DmaJob_ProcessBlocks. */
    rc = NEXUS_DmaJob_ProcessBlocks(dmaJob, (NEXUS_DmaJobBlockSettings *)pBlkSettings, nBlocks);
    if(rc != NEXUS_SUCCESS){
        if(rc == NEXUS_DMA_QUEUED){
            for(;;)
            {
                rc = NEXUS_DmaJob_GetStatus(dmaJob, &jobStatus);
                if(rc != NEXUS_SUCCESS) {
                    BDBG_ERR(("%s - NEXUS_DmaJob_ProcessBlocks failed, rc = %d\n", __FUNCTION__, rc));
                    goto errorExit;
                }

                if(jobStatus.currentState == NEXUS_DmaJobState_eComplete )
                {
                    break;
                }
                BKNI_Delay(1);
            }

         }
         else {
             BDBG_ERR(("%s - NEXUS_DmaJob_ProcessBlocks failed, rc = %d\n", __FUNCTION__, rc));
             goto errorExit;
         }
     }


errorExit:
    if(dmaJob != NULL) NEXUS_DmaJob_Destroy(dmaJob);

    BDBG_MSG(("%s - Exiting function", __FUNCTION__));    
    return rc;
}

NEXUS_Error CommonCrypto_P_LoadKeyConfig(
    CommonCryptoHandle handle,
    NEXUS_KeySlotHandle keySlot,
    const CommonCryptoAlgorithmSettings *pSettings
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_SecurityAlgorithmSettings     nexusConfig;

    if(handle->settings.dmaSettings.coreType != NEXUS_DmaCoreType_eSharf){

        /* Set up key */
        NEXUS_Security_GetDefaultAlgorithmSettings(&nexusConfig);

        /* Config Algrithm */
        nexusConfig.algorithm           = pSettings->algType;
        nexusConfig.algorithmVar        = pSettings->algVariant;
        nexusConfig.operation           = pSettings->opType;
        nexusConfig.keyDestEntryType    = pSettings->keySlotType;
        nexusConfig.terminationMode     = pSettings->termMode;
        nexusConfig.enableExtKey        = pSettings->enableExtKey;
        nexusConfig.enableExtIv         = pSettings->enableExtIv; 
        nexusConfig.aesCounterSize      = pSettings->aesCounterSize;

#if (NEXUS_SECURITY_HAS_ASKM == 1)
        nexusConfig.key2Select          = NEXUS_SecurityKey2Select_eReserved1;
        nexusConfig.caVendorID          = 0x1234;/* Verify vendor ID with security team */
        nexusConfig.askmModuleID        = NEXUS_SecurityAskmModuleID_eModuleID_4;
#endif

        /* Configure the keyslot for the particular operation (except when using Sharf Engine) */
        rc = NEXUS_Security_ConfigAlgorithm(keySlot, &nexusConfig);
        if(rc != NEXUS_SUCCESS){
            return BERR_TRACE(rc);
        }

    }
    else 
    {
        rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

    return rc;
}

NEXUS_Error CommonCrypto_LoadKeyConfig(
    CommonCryptoHandle handle,
    const CommonCryptoKeyConfigSettings *pSettings
    )
{
    NEXUS_Error rc;

    BDBG_ASSERT(pSettings != NULL);
    BDBG_ASSERT(pSettings->keySlot != NULL);

    rc = CommonCrypto_P_LoadKeyConfig(handle, pSettings->keySlot, &pSettings->settings);

    return rc;
}



NEXUS_Error CommonCrypto_P_LoadClearKeyIv(
    CommonCryptoHandle handle,
    NEXUS_KeySlotHandle keySlot,
    NEXUS_SecurityKeyType keySlotType,
    const CommonCryptoKeyIvSettings *pKeyIvStruct
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    
    if(pKeyIvStruct->keySize > 0 )
    {
        rc = CommonCrypto_LoadClearKey_priv(keySlot,
                                pKeyIvStruct->key,
                                pKeyIvStruct->keySize, 
                                keySlotType, 
                                NEXUS_SecurityKeyIVType_eNoIV);
        if(rc != NEXUS_SUCCESS){
            return BERR_TRACE(rc);
        } 
    }

    if(handle->settings.dmaSettings.coreType != NEXUS_DmaCoreType_eSharf){
        if(pKeyIvStruct->ivSize > 0)
        {
            rc = CommonCrypto_LoadClearKey_priv(keySlot,
                                    pKeyIvStruct->iv,
                                    pKeyIvStruct->ivSize, 
                                    keySlotType,
                                    NEXUS_SecurityKeyIVType_eIV);
            if(rc != NEXUS_SUCCESS){
                return BERR_TRACE(rc);
            } 
        }
    }
    else {
        /* When using the Sharf, the IV must be specified in dma descriptor */
        rc = BERR_TRACE(NEXUS_NOT_SUPPORTED);
    }

   return rc;
}

NEXUS_Error CommonCrypto_LoadClearKeyIv(
    CommonCryptoHandle handle,
    const CommonCryptoClearKeySettings *pSettings
    )
{
    NEXUS_Error rc;

    BDBG_ASSERT(pSettings != NULL);
    BDBG_ASSERT(pSettings->keySlot != NULL);

    rc = CommonCrypto_P_LoadClearKeyIv(handle, pSettings->keySlot, pSettings->keySlotType, &pSettings->settings);

    return rc;
}

NEXUS_Error CommonCrypto_P_LoadCipheredKey(
    NEXUS_KeySlotHandle keySlot,
    CommonCryptoKeySrc keySrc,
    NEXUS_SecurityKeyType keySlotType,
    const CommonCryptoKeyLadderSettings *pKeyLadderInfo
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_SecurityRootKeySrc  rootKeySrc;

    switch(keySrc)
    {
        case CommonCrypto_eCustKey:
            rootKeySrc = NEXUS_SecurityRootKeySrc_eCuskey;
            break;
        case CommonCrypto_eOtpKey:
            rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA;
            break;
        default:
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            break;
    }

    rc = CommonCrypto_LoadCipheredKey_priv(keySlot, rootKeySrc, keySlotType, pKeyLadderInfo);

    return rc; 
}

NEXUS_Error CommonCrypto_LoadCipheredKey(
    CommonCryptoHandle handle,
    const CommonCryptoCipheredKeySettings *pSettings
    )
{
    NEXUS_Error rc;

    BDBG_ASSERT(pSettings != NULL);
    BDBG_ASSERT(pSettings->keySlot != NULL);
    BSTD_UNUSED(handle);

    rc = CommonCrypto_P_LoadCipheredKey(pSettings->keySlot, pSettings->keySrc, pSettings->keySlotType, &pSettings->settings);

    return rc;
}

NEXUS_Error CommonCrypto_SetupKey(
    CommonCryptoHandle handle,
    const CommonCryptoKeySettings *pSettings
    )
{
    NEXUS_Error rc  = NEXUS_SUCCESS;

    BDBG_ASSERT(pSettings != NULL);
    BDBG_ASSERT(pSettings->keySlot != NULL);

    BDBG_MSG(("%s - Entered function", __FUNCTION__));

    rc = CommonCrypto_P_LoadKeyConfig(handle,
                            pSettings->keySlot, 
                            &pSettings->alg);
    if(rc != NEXUS_SUCCESS) {
        return BERR_TRACE(rc);
    }

     switch(pSettings->keySrc){
        case CommonCrypto_eClearKey:

            rc = CommonCrypto_P_LoadClearKeyIv(handle,
                            pSettings->keySlot,
                            pSettings->alg.keySlotType,
                            &pSettings->src.keyIvInfo);
            break;
        case CommonCrypto_eCustKey:
        case CommonCrypto_eOtpKey:
            rc = CommonCrypto_P_LoadCipheredKey(
                            pSettings->keySlot,
                            pSettings->keySrc,
                            pSettings->alg.keySlotType,
                            &pSettings->src.keyLadderInfo);
            break;
        default:
            return BERR_TRACE(NEXUS_INVALID_PARAMETER);
            break;
    }

    BDBG_MSG(("%s - Exiting function", __FUNCTION__));
    return rc;
}

static NEXUS_Error CommonCrypto_LoadClearKey_priv(
    NEXUS_KeySlotHandle keySlot,
    const uint8_t *pKey, 
    uint32_t keySize, 
    NEXUS_SecurityKeyType keySlotType,
    NEXUS_SecurityKeyIVType keyIvType
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
    NEXUS_SecurityClearKey swKey;

    BDBG_ASSERT(pKey != NULL);
    BDBG_ASSERT(keySlot != NULL);

    BDBG_MSG(("%s - Loading sw key or iv... (keyIvType = %u) keySlotType = %u", __FUNCTION__, keyIvType, keySlotType));

    if(keySize > sizeof(swKey.keyData)){
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);
    }

    if(keySize > COMMON_CRYPTO_KEY_SIZE)
        return BERR_TRACE(NEXUS_INVALID_PARAMETER);

    NEXUS_Security_GetDefaultClearKey(&swKey);

    swKey.keyEntryType = keySlotType;
    swKey.keySize      = keySize;
    swKey.keyIVType    = keyIvType;

    BKNI_Memcpy(swKey.keyData, pKey, keySize);

    rc = NEXUS_Security_LoadClearKey(keySlot, &swKey);
    if(rc != NEXUS_SUCCESS)
    {
        return BERR_TRACE(rc);
    }

    return rc;
}


static NEXUS_Error CommonCrypto_LoadCipheredKey_priv(
    NEXUS_KeySlotHandle keySlot,
    NEXUS_SecurityRootKeySrc keySrc,
    NEXUS_SecurityKeyType keySlotType,
    const CommonCryptoKeyLadderSettings *pKeyLadderInfo
    )
{
    NEXUS_Error rc = NEXUS_SUCCESS;
       
    NEXUS_SecurityEncryptedSessionKey encryptedSessionkey;
    NEXUS_SecurityEncryptedControlWord encrytedCW;
    
    BDBG_ASSERT(pKeyLadderInfo != NULL);
    BDBG_ASSERT(keySlot != NULL);

#if (BCHP_CHIP == 7125)

    #warning "Using memset to initialize NEXUS_SecurityEncryptedSessionKey structure because NEXUS_Security_GetDefaultSessionKeySettings is not available on this chip."
    #warning "Using memset to initialize NEXUS_SecurityEncryptedControlWord structure because NEXUS_Security_GetDefaultControlWordSettings is not available on this chip."

    memset(&encryptedSessionkey, 0, sizeof(encryptedSessionkey));
    memset(&encrytedCW,          0, sizeof(encrytedCW));
#else
    NEXUS_Security_GetDefaultSessionKeySettings(&encryptedSessionkey);
    NEXUS_Security_GetDefaultControlWordSettings(&encrytedCW);
#endif

    BDBG_MSG(("%s - Entered function", __FUNCTION__));
    encryptedSessionkey.keyladderID     = NEXUS_SecurityKeyladderID_eA;
    encryptedSessionkey.keyladderType   = NEXUS_SecurityKeyladderType_e3Des;
    encryptedSessionkey.swizzleType     = NEXUS_SecuritySwizzleType_eSwizzle0;

    encryptedSessionkey.rootKeySrc      = keySrc;
    encryptedSessionkey.bRouteKey       = false;
    encryptedSessionkey.operation       = pKeyLadderInfo->KeyLadderOpStruct.SessionKeyOperation;/* Default: NEXUS_SecurityOperation_eDecrypt;*/
    encryptedSessionkey.operationKey2   = pKeyLadderInfo->KeyLadderOpStruct.SessionKeyOperationKey2;/* Default: NEXUS_SecurityOperation_eEncrypt;*/
    encryptedSessionkey.keyEntryType    = keySlotType;

    if(keySrc == NEXUS_SecurityRootKeySrc_eCuskey)
    {
        BDBG_MSG(("%s - Setting CustKey values", __FUNCTION__));
        encryptedSessionkey.cusKeyL         = pKeyLadderInfo->custKeySelect;
        encryptedSessionkey.cusKeyH         = pKeyLadderInfo->custKeySelect;
        encryptedSessionkey.cusKeyVarL      = pKeyLadderInfo->keyVarLow;
        encryptedSessionkey.cusKeyVarH      = pKeyLadderInfo->keyVarHigh;
    }
    BKNI_Memcpy(encryptedSessionkey.keyData, pKeyLadderInfo->procInForKey3, sizeof(pKeyLadderInfo->procInForKey3));

#if (NEXUS_SECURITY_HAS_ASKM == 1)
#if (BCHP_CHIP != 7125)
    encryptedSessionkey.keyGenCmdID = NEXUS_SecurityKeyGenCmdID_eKeyGen;
    encryptedSessionkey.sessionKeyOp =  NEXUS_SecuritySessionKeyOp_eNoProcess;
    encryptedSessionkey.bASKMMode = false;
    encryptedSessionkey.bSwapAESKey = false;
    encryptedSessionkey.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
    BDBG_MSG(("%s - VKL Session key (40nm) *****************", __FUNCTION__));
#endif
    encryptedSessionkey.custSubMode = pKeyLadderInfo->VirtualKeyLadderSettings.CustSubMode;/* Default: NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;*/
    BDBG_MSG(("%s - VKL Session key *****************", __FUNCTION__));
    encryptedSessionkey.virtualKeyLadderID =  pKeyLadderInfo->VirtualKeyLadderSettings.VklValue;/* Default: NEXUS_SecurityVirtualKeyladderID_eVKL6;*/
#endif /*NEXUS_SECURITY_HAS_ASKM*/
    encryptedSessionkey.keyMode = NEXUS_SecurityKeyMode_eRegular;

    BDBG_MSG(("%s - Calling 'NEXUS_Security_GenerateSessionKey'", __FUNCTION__));
    rc = NEXUS_Security_GenerateSessionKey(keySlot, &encryptedSessionkey);
    if(rc == NEXUS_SUCCESS){
        /* Load CW */
        encrytedCW.keyladderID = NEXUS_SecurityKeyladderID_eA;
        encrytedCW.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
        encrytedCW.keySize = sizeof(pKeyLadderInfo->procInForKey4); 
        encrytedCW.keyEntryType = keySlotType;
        encrytedCW.operation = pKeyLadderInfo->KeyLadderOpStruct.ControlWordKeyOperation;/* Default: NEXUS_SecurityOperation_eDecrypt;*/
        encrytedCW.bRouteKey = true;

        BKNI_Memcpy(encrytedCW.keyData, pKeyLadderInfo->procInForKey4, sizeof(pKeyLadderInfo->procInForKey4));

#if (NEXUS_SECURITY_HAS_ASKM == 1)
#if (BCHP_CHIP != 7125)
        encrytedCW.keyDestIVType = NEXUS_SecurityKeyIVType_eNoIV;
        encrytedCW.keyGenCmdID = NEXUS_SecurityKeyGenCmdID_eKeyGen;
        encrytedCW.bSwapAESKey = false;
        BDBG_MSG(("%s - VKL Control Word (40nm) *****************", __FUNCTION__));
#endif
        encrytedCW.custSubMode = pKeyLadderInfo->VirtualKeyLadderSettings.CustSubMode;/* Default: NEXUS_SecurityCustomerSubMode_eGeneric_CP_128_4;*/
        BDBG_MSG(("%s - VKL Control Word *****************", __FUNCTION__));
        encrytedCW.virtualKeyLadderID = pKeyLadderInfo->VirtualKeyLadderSettings.VklValue;/* Default: NEXUS_SecurityVirtualKeyladderID_eVKL6;*/
#endif /*NEXUS_SECURITY_HAS_ASKM*/
        encrytedCW.keyMode = NEXUS_SecurityKeyMode_eRegular;
        rc = NEXUS_Security_GenerateControlWord(keySlot, &encrytedCW);
        if(rc != NEXUS_SUCCESS)
        {
            BDBG_ERR(("%s - Error generating Control Word (key4)", __FUNCTION__));
        }
    }
    else
    {
        BDBG_ERR(("%s - Error generating Session key (key3)", __FUNCTION__));
    }

    BDBG_MSG(("%s - Exiting function", __FUNCTION__));
    return rc;
}
