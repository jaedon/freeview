/**@@@+++@@@@******************************************************************
**
** Microsoft PlayReady
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <oemcommon.h>
#include <drmtypes.h>
#include <drmutilities.h>

/*#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"*/
#ifndef HUMAX_PLATFORM_BASE
#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"
#endif

ENTER_PK_NAMESPACE_CODE;
#ifdef PRDY_ROBUSTNESS

#include "common_crypto.h"
#include "../common/inc/oem_brcm.h"

DRM_RESULT _Crypto_AesEcb(
    __in     CommonCryptoHandle commonCryptoHandle,
    __in     CommonCryptoKeySrc keySrc,
    __in     CommonCryptoKeyLadderSettings *pKeyLadderSettings,
    __inout  uint8_t *pBuf,
    __inout  uint32_t buf_size,
    __in     DRM_BYTE *pEncKey, 
    __in     NEXUS_SecurityOperation opType
)
{
    DRM_RESULT  dr = DRM_SUCCESS;
    NEXUS_SecurityKeySlotSettings keySlotSettings;
    NEXUS_KeySlotHandle keyHandle = NULL;
    CommonCryptoKeySettings keySettings;
    NEXUS_DmaJobBlockSettings blk;
    CommonCryptoJobSettings jobSettings;

    ChkArg( pKeyLadderSettings != NULL
         && pBuf  != NULL
         && buf_size != 0);

    ChkArg( !(pEncKey  == NULL && keySrc == CommonCrypto_eCustKey));

    if(keySrc == CommonCrypto_eCustKey)
        BKNI_Memcpy(pKeyLadderSettings->procInForKey4, pEncKey, 16);

    NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
    keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
    
    keyHandle = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
    if(keyHandle == NULL) 
    {
        ChkDR(DRM_E_CRYPTO_FAILED);
    }

    CommonCrypto_GetDefaultKeySettings(&keySettings, keySrc);
    keySettings.keySrc  = keySrc;
    keySettings.keySlot = keyHandle;
    BKNI_Memcpy(&keySettings.src.keyLadderInfo, pKeyLadderSettings, sizeof(keySettings.src.keyLadderInfo));
	
	if(keySrc != CommonCrypto_eClearKey){
    	CommonCrypto_GetDefaultKeyLadderSettings((CommonCryptoKeyLadderSettings*)&keySettings.src.keyLadderInfo);
    }
    keySettings.alg.opType            = opType;
    keySettings.alg.algType           = NEXUS_SecurityAlgorithm_eAes;
    keySettings.alg.algVariant        = NEXUS_SecurityAlgorithmVariant_eEcb;
    keySettings.alg.termMode          = NEXUS_SecurityTerminationMode_eClear;
    keySettings.alg.enableExtKey      = false;
    keySettings.alg.enableExtIv       = false;
    keySettings.alg.aesCounterSize    = 0;

    if(CommonCrypto_SetupKey(commonCryptoHandle, &keySettings) != NEXUS_SUCCESS)
    {
        ChkDR(DRM_E_CRYPTO_FAILED);
    }

    NEXUS_DmaJob_GetDefaultBlockSettings(&blk);
    blk.pSrcAddr =  pBuf;
    blk.pDestAddr = pBuf;
    blk.blockSize = buf_size;
    blk.resetCrypto = true;
    blk.scatterGatherCryptoStart = true;
    blk.scatterGatherCryptoEnd = true;

    CommonCrypto_GetDefaultJobSettings(&jobSettings);
    jobSettings.keySlot = keyHandle;

    if(CommonCrypto_DmaXfer(commonCryptoHandle, &jobSettings, &blk, 1) != NEXUS_SUCCESS)
    {
        ChkDR(DRM_E_CRYPTO_FAILED);
    }

ErrorExit:
    if(keyHandle != NULL)
    {
        NEXUS_Security_FreeKeySlot(keyHandle);
    }

    return dr;
}
#endif
EXIT_PK_NAMESPACE_CODE;

