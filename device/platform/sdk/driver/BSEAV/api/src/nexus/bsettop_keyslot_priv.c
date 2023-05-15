/***************************************************************************
 *     Copyright (c) 2004-2011, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_keyslot_priv.c $
 * $brcm_Revision: 16 $
 * $brcm_Date: 10/20/11 4:34p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_keyslot_priv.c $
 * 
 * 16   10/20/11 4:34p mphillip
 * SW7346-481: 7346/7425 B0 support exposed a lack of
 * NEXUS_Security_GetDefaultClearKey calls
 * 
 * 15   5/12/11 3:47p jtna
 * SW7550-739: replace all instances of 'NEXUS_HAS_DMA &&
 * NEXUS_HAS_SECURITY' with 'NEXUS_ENCRYPTED_DVR_WITH_M2M'. replace some
 * instances of 'NEXUS_HAS_DMA' with 'NEXUS_NUM_DMA_CHANNELS'
 * 
 * 14   4/15/11 3:14p jtna
 * SW7422-420: replace deprecated NEXUS_DmaJob_Start() with
 * NEXUS_DmaJob_ProcessBlocks()
 * 
 * 13   6/10/10 2:45p mphillip
 * SW7550-380: Enable non-DMA PVR encryption for 7550 and similar
 * platforms
 * 
 * 12   6/3/10 5:11p mward
 * SW7400-2786:  For DivX DRM under Nexus on 7400/7403, pass a DMA handle
 * for use by bdrm_memory_crypt().
 * 
 * 11   12/9/09 12:03p gmohile
 * SW7408-1 : Add 7408 support
 * 
 * 10   8/25/09 12:36p erickson
 * SW7420-228: fix warnings
 *
 * 9   7/10/09 6:18p atruong
 * PR55169: NEXUS/HSM PI support for 7420A1
 *
 * 8   6/15/09 5:07p jtna
 * PR43001: add support for TTS+encryption record/playback
 *
 * 7   3/31/09 7:31p katrep
 * PR53733: MSDRM_PD_HWDECRYPT support,moved msdrm pd compile to settop
 * api from nexus
 *
 * 6   3/9/09 6:59p nickh
 * PR52996: Fix compilation errors when NEXUS_HAS_SECURITY is disabled
 *
 * 5   2/26/09 2:53p erickson
 * PR49584: add a dma-based memory bandwidth stress test feature
 *
 * 4   12/24/08 11:54a mphillip
 * PR49607: Allow keyslot to be configured based on keyladder options
 *
 * 3   8/13/08 12:58p mphillip
 * PR40027: Add CBC support to keyslot allocation
 *
 * 2   7/3/08 12:21p erickson
 * PR39978: allow security to be optional
 *
 * 1   6/26/08 6:22p mphillip
 * PR42901: Encrypted PVR playback support
 *
 *
 *******************************************************************************/

#include "bsettop_impl.h"

#if NEXUS_HAS_DMA
#include "nexus_dma.h"
#endif
#include "nexus_memory.h"
#if NEXUS_HAS_SECURITY
#include "nexus_security.h"
#if NEXUS_HAS_KEYLADDER_SUPPORT
#include "nexus_keyladder.h"
#endif
#endif
#if B_HAS_MSDRM_PD && MSDRM_PD_HWDECRYPT
#include "drmcore.h"
#endif

#if B_HAS_DIVX_DRM && (BCHP_CHIP==7400) || (BCHP_CHIP==7403)
#include "bdrm_decrypt.h"
#endif

#if NEXUS_HAS_DMA
struct bdma g_dma = {NULL};
#endif

BDBG_MODULE(dma);

void bdma_p_init()
{
#if NEXUS_HAS_DMA
    NEXUS_DmaSettings dmaSettings;

    NEXUS_Dma_GetDefaultSettings(&dmaSettings);
    dmaSettings.endianMode = NEXUS_DmaEndianMode_eLittle; /* TODO: endianness */
    dmaSettings.swapMode = NEXUS_DmaSwapMode_eNone;

    g_dma.hDma = NEXUS_Dma_Open(0, &dmaSettings);
    if (g_dma.hDma == NULL) {
        BDBG_ERR(("Unable to open DMA"));
        return;
    }
#if B_HAS_MSDRM_PD && MSDRM_PD_HWDECRYPT
    bdrm_set_dma_handle(g_dma.hDma);
#endif
#if B_HAS_DIVX_DRM && (BCHP_CHIP==7400) || (BCHP_CHIP==7403)
	bdrm_divx_set_dma_handle(g_dma.hDma);
#endif
#endif    
}

void bdma_p_uninit()
{
#if NEXUS_HAS_DMA
    if (g_dma.hDma) {
        NEXUS_Dma_Close(g_dma.hDma);
        g_dma.hDma = NULL;
    }
#if B_HAS_MSDRM_PD && MSDRM_PD_HWDECRYPT
    bdrm_set_dma_handle(NULL);
#endif
#if B_HAS_DIVX_DRM && (BCHP_CHIP==7400) || (BCHP_CHIP==7403)
	bdrm_divx_set_dma_handle(NULL);
#endif
#endif
}

NEXUS_KeySlotHandle b_keyslot_m2m_allocate(const bencryption_params *encryption, bool encrypt, bool timestamp)
{
#if NEXUS_HAS_SECURITY
    NEXUS_Error rc;
    NEXUS_KeySlotHandle pKeySlot = NULL;
    NEXUS_SecurityKeySlotSettings keySettings;
    NEXUS_SecurityAlgorithmSettings algSettings;
    NEXUS_SecurityClearKey key;

    NEXUS_Security_GetDefaultAlgorithmSettings(&algSettings);
    switch (encryption->type) {
    case bencryption_type_aes:
        BDBG_MSG(("b_keyslot_m2m_allocate: Creating AES keyslot"));
        algSettings.algorithm = NEXUS_SecurityAlgorithm_eAes;
        break;
    case bencryption_type_des:
        BDBG_MSG(("b_keyslot_m2m_allocate: Creating DES keyslot"));
        algSettings.algorithm = NEXUS_SecurityAlgorithm_eDes;
        break;
    case bencryption_type_3des:
        BDBG_MSG(("b_keyslot_m2m_allocate: Creating 3DES keyslot"));
        algSettings.algorithm = NEXUS_SecurityAlgorithm_e3DesAba;
        break;
    default:
        BDBG_MSG(("b_keyslot_m2m_allocate: Unsupported encryption type: %d",encryption->type));
        goto error;
    }

    NEXUS_Security_GetDefaultKeySlotSettings(&keySettings);
#if NEXUS_NUM_DMA_CHANNELS
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;
#else
    keySettings.keySlotEngine = NEXUS_SecurityEngine_eCp;
#endif
    pKeySlot = NEXUS_Security_AllocateKeySlot(&keySettings);
    if (!pKeySlot) {
        BDBG_ERR(("b_keyslot_m2m_allocate: Unable to allocate m2m keyslot"));
        goto error;
    }

    algSettings.algorithmVar = encryption->blockmode == bcrypto_blockmode_cbc ? NEXUS_SecurityAlgorithmVariant_eCbc : NEXUS_SecurityAlgorithmVariant_eEcb;
    algSettings.terminationMode = NEXUS_SecurityTerminationMode_eClear;
#if NEXUS_NUM_DMA_CHANNELS
    algSettings.operation = encrypt ? NEXUS_SecurityOperation_eEncrypt : NEXUS_SecurityOperation_eDecrypt;
#else
    algSettings.dest = encrypt ? NEXUS_SecurityAlgorithmConfigDestination_eCps : NEXUS_SecurityAlgorithmConfigDestination_eCpd;
#endif
    algSettings.enableTimestamps = timestamp;
    rc = NEXUS_Security_ConfigAlgorithm(pKeySlot, &algSettings);
    if (rc != 0) {
        BDBG_ERR(("b_keyslot_m2m_allocate: Unable to configure m2m algorithm"));
        goto error_keyslot;
    }

    if (encryption->key_ladder) {
#if NEXUS_HAS_KEYLADDER_SUPPORT
        bcrypto_keyladder_data *keyladder_data = (bcrypto_keyladder_data *)encryption->long_key;
        NEXUS_SecurityEncryptedSessionKey key3;
        NEXUS_SecurityEncryptedControlWord key4;

        if (encryption->key_length != 8*sizeof(bcrypto_keyladder_data)) {
            BDBG_ERR(("b_keyslot_m2m_allocate: Invalid keyladder_data size"));
            goto error_keyslot;
        }
        if (!keyladder_data) {
            BDBG_ERR(("b_keyslot_m2m_allocate: No keyladder data provided!"));
            goto error_keyslot;
        }

        if (keyladder_data->keyladder_key != bcrypto_keyladder_key_4 && keyladder_data->keyladder_key != bcrypto_keyladder_key_5) {
            BDBG_ERR(("b_keyslot_m2m_allocate: Unsupported keyladder key"));
            goto error_keyslot;
        }

        key3.keyladderID = NEXUS_SecurityKeyladderID_eA;
        key3.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
        switch (keyladder_data->rootkey_src) {
        case bcrypto_rootkey_cust_key:
            key3.rootKeySrc = NEXUS_SecurityRootKeySrc_eCuskey;
            break;
        case bcrypto_rootkey_otp_a:
            key3.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyA;
            break;
        case bcrypto_rootkey_otp_b:
            key3.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyB;
            break;
        case bcrypto_rootkey_otp_c:
            key3.rootKeySrc = NEXUS_SecurityRootKeySrc_eOtpKeyC;
            break;
        default:
            BDBG_ERR(("b_keyslot_m2m_allocate: Unsupported keyladder root key source"));
            goto error_keyslot;
        }
        switch (keyladder_data->swizzle) {
        case bcrypto_swizzle_none:
            key3.swizzleType = NEXUS_SecuritySwizzleType_eNone;
            break;
        case bcrypto_swizzle_0:
            key3.swizzleType = NEXUS_SecuritySwizzleType_eSwizzle0;
            break;
        case bcrypto_swizzle_1:
        default:
            BDBG_ERR(("b_keyslot_m2m_allocate: Unsupported swizzle"));
            goto error_keyslot;
        }
        key3.keyEntryType = NEXUS_SecurityKeyType_eOdd;
        key3.operation = keyladder_data->gen_key_decrypt ? NEXUS_SecurityOperation_eDecrypt : NEXUS_SecurityOperation_eEncrypt;
        key3.cusKeyL = keyladder_data->key_cust_low;
        key3.cusKeyH = keyladder_data->key_cust_high;
        key3.cusKeyVarL = keyladder_data->key_var_low;
        key3.cusKeyVarH = keyladder_data->key_var_high;
        BKNI_Memcpy((void *)(key3.keyData),
                    (void *)(keyladder_data->odd_in_3),
                    24);
        key3.bRouteKey = false;

        if (NEXUS_Security_GenerateSessionKey(pKeySlot, &key3) != 0) {
            BDBG_ERR(("b_keyslot_m2m_allocate: Generate key3 failed"));
            goto error_keyslot;
        }

        key4.keyladderID = NEXUS_SecurityKeyladderID_eA;
        key4.keyladderType = NEXUS_SecurityKeyladderType_e3Des;
        key4.keySize = 16;
        key4.keyEntryType = NEXUS_SecurityKeyType_eOdd;
        key4.operation = keyladder_data->gen_key_decrypt ? NEXUS_SecurityOperation_eDecrypt : NEXUS_SecurityOperation_eEncrypt;
        key4.bRouteKey = keyladder_data->keyladder_key == bcrypto_keyladder_key_4 ? true : false;
        BKNI_Memcpy((void *)(key4.keyData),
                    (void *)(keyladder_data->odd_in_4),
                    24);

        if (NEXUS_Security_GenerateControlWord(pKeySlot, &key4) != 0) {
            BDBG_ERR(("b_keyslot_m2m_allocate: Generate key4 failed"));
            goto error_keyslot;
        }

        if (keyladder_data->keyladder_key == bcrypto_keyladder_key_5) {
            /* Route Key 5 */
            BDBG_ERR(("key5 ***NYI***"));
        }
#else
        BDBG_ERR(("b_keyslot_m2m_allocate: Keyladder support is not enabled"));
        goto error_keyslot;
#endif
    } else {
        NEXUS_Security_GetDefaultClearKey(&key);
        key.keySize = encryption->key_length/8;
        key.keyEntryType = NEXUS_SecurityKeyType_eOdd; /* clear key */
        BKNI_Memcpy(key.keyData,encryption->key,key.keySize);
        if (NEXUS_Security_LoadClearKey(pKeySlot, &key) != 0) {
            BDBG_ERR(("b_keyslot_m2m_allocate: Unable to load m2m key data"));
            goto error_keyslot;
        }
    }

    if (encryption->blockmode == bcrypto_blockmode_cbc) {
        key.keyEntryType = NEXUS_SecurityKeyType_eIv;
        if (encryption->type == bencryption_type_aes) {
            key.keySize = 16;
            BKNI_Memcpy(key.keyData,encryption->iv,key.keySize);
        } else {
            key.keySize = 16; /* We have to lie for alignment reasons */

            /* This also means that the IV needs to be loaded in the second 8-byte
             * block of the data, not the first, and the first part needs to be
             * zeroed. */
            BKNI_Memcpy(&key.keyData[8],encryption->iv,8);
            BKNI_Memset(key.keyData,0,8);
        }
        if (NEXUS_Security_LoadClearKey(pKeySlot, &key) != 0) {
            BDBG_ERR(("b_keyslot_m2m_allocate: Unable to load m2m IV data"));
            b_keyslot_m2m_free(pKeySlot);
            return NULL;
        }
    }

    BDBG_MSG(("b_keyslot_m2m_allocateSuccessfully created keyslot"));
    return pKeySlot;
#else
    BSTD_UNUSED(encryption);
    BSTD_UNUSED(encrypt);
    BSTD_UNUSED(timestamp);
    BSETTOP_ERROR(berr_not_supported);
    return NULL;
#endif
#if NEXUS_HAS_SECURITY
error_keyslot:
    b_keyslot_m2m_free(pKeySlot);
error:
#endif
    return NULL;
}

void b_keyslot_m2m_free(NEXUS_KeySlotHandle pHandle)
{
#if NEXUS_HAS_SECURITY
    if (pHandle) {
        BDBG_MSG(("b_keyslot_m2m_free: Freeing keyslot"));
        NEXUS_Security_FreeKeySlot(pHandle);
    }
#else
    BSTD_UNUSED(pHandle);
#endif
}

/***************
* PR 49584: The following is stress-test code which soaks up all memory bandwidth using M2M DMA
* export mem_stress=y to activate this feature.
**/

#if NEXUS_HAS_DMA
static struct {
    NEXUS_DmaJobHandle job;
    uint64_t totalCopied;
    void *mem;
    unsigned size;
    unsigned lastGbCopied;
} g_memStress;

static void bdma_p_mem_stress_callback(void *context, int param)
{
    NEXUS_DmaJobBlockSettings blockSettings;
    NEXUS_Error rc;

    BSTD_UNUSED(context);
    if (param) {
        unsigned gbCopied;
        g_memStress.totalCopied += g_memStress.size;
        gbCopied = (unsigned)(g_memStress.totalCopied / 1024 / 1024 / 1024);
        if (g_memStress.lastGbCopied != gbCopied) {
            BDBG_WRN(("mem_stress %d GB copied", gbCopied));
            g_memStress.lastGbCopied = gbCopied;
        }
    }

    NEXUS_DmaJob_GetDefaultBlockSettings(&blockSettings);
    blockSettings.pSrcAddr = g_memStress.mem;
    blockSettings.pDestAddr = g_memStress.mem;
    blockSettings.blockSize = g_memStress.size;
    NEXUS_DmaJob_SetBlockSettings(g_memStress.job, 0, &blockSettings);

    rc = NEXUS_DmaJob_ProcessBlocks(g_memStress.job, &blockSettings, 1);
    if (rc!=NEXUS_SUCCESS && rc!=NEXUS_DMA_QUEUED) rc = BERR_TRACE(rc);
}

void bdma_p_mem_stress(void)
{
    NEXUS_DmaJobSettings jobSettings;
    NEXUS_Error rc;

    NEXUS_DmaJob_GetDefaultSettings(&jobSettings);
    jobSettings.numBlocks = 1;
    jobSettings.completionCallback.callback = bdma_p_mem_stress_callback;
    jobSettings.completionCallback.param = 1;
    g_memStress.job = NEXUS_DmaJob_Create(g_dma.hDma, &jobSettings);

    g_memStress.size = 10 * 1024 * 1024;
    rc = NEXUS_Memory_Allocate(g_memStress.size, NULL, &g_memStress.mem);
    if (rc) {
        rc = BERR_TRACE(rc);
        return;
    }

    /* kickstart the callback, then let it run forever */
    bdma_p_mem_stress_callback(NULL, 0);
}
#endif


