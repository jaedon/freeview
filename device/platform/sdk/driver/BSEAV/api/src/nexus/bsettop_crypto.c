/***************************************************************************
 *     Copyright (c) 2006-2009, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: bsettop_crypto.c $
 * $brcm_Revision: 6 $
 * $brcm_Date: 12/9/09 12:03p $
 *
 * Module Description:
 *
 * Revision History:
 *
 * $brcm_Log: /BSEAV/api/src/nexus/bsettop_crypto.c $
 * 
 * 6   12/9/09 12:03p gmohile
 * SW7408-1 : Add 7408 support
 * 
 * 5   6/15/09 5:08p jtna
 * PR43001: add support for TTS+encryption record/playback
 * 
 * 4   12/24/08 11:56a jgarrett
 * PR 50703: Allowing for security module to be absent
 * 
 * 3   8/13/08 12:56p mphillip
 * PR40027: Fix return code from bcrypto_process
 * 
 * 2   8/6/08 1:47p mphillip
 * PR40027: Add basic block modes
 * 
 * 1   10/15/07 2:35p erickson
 * PR36068: initial
 * 
 *
 *
 ***************************************************************************/

#include "bsettop_impl.h"

#if NEXUS_HAS_DMA
#include "nexus_memory.h"
#include "nexus_dma.h"

BDBG_MODULE(crypto);

typedef struct bcrypto {
    bcrypto_settings        settings;
    NEXUS_KeySlotHandle keyHandle;

    bool    key_loaded;
    unsigned int key_slot;

} bcrypto;

/* B_CRYPTO_N_CRYPTS controls the number of bcrypto structures available to 
   bcrypto_open (and thus the number of possible concurrent crypto sessions).

   This should be enough for reasonable cryptographic use.  Even for 3-way 
   PVR, you have 1 for encrypting and 2 for decrypting per channel (9 total 
   used).  This can be safely increased, it just results in an array of 
   struct bcrypto of this size.
 */
#define B_CRYPTO_N_CRYPTS (32)
static struct bcrypto g_crypto[B_CRYPTO_N_CRYPTS];


void bcrypto_p_init(void)
{
    BKNI_Memset(g_crypto, 0, sizeof(g_crypto));
}

void bcrypto_p_shutdown(void)
{
    unsigned i;
    for(i=0;i<B_CRYPTO_N_CRYPTS;i++) {
        if (g_crypto[i].keyHandle) {
            bcrypto_close(&g_crypto[i]);
        }
    }
}

void bcrypto_settings_init(bcrypto_settings *settings)
{
    BKNI_Memset(settings, 0, sizeof(bcrypto_settings));
}

bcrypto_t bcrypto_open(const bcrypto_settings *settings)
{
    unsigned i;

    BDBG_ASSERT(settings);

    for(i=0;i<B_CRYPTO_N_CRYPTS;i++) {
        if (!g_crypto[i].keyHandle) {
            g_crypto[i].settings = *settings;
            return &g_crypto[i];
        }
    }

    BSETTOP_ERROR(berr_not_available);
    return NULL;
}

bresult bcrypto_status(bcrypto_t crypto, bcrypto_status_t *status)
{
    BDBG_ASSERT(crypto);
    if (status) {
        switch(crypto->settings.encryption.type) {
        case bencryption_type_des:
        case bencryption_type_3des:
        case bencryption_type_aes:
        case bencryption_type_dvb:
        case bencryption_type_rc4:
        case bencryption_type_c2:
            status->matching_output_size = true;
            status->fixed_output_size = false;
            status->fixed_output_length = 0;
            break;
        case bencryption_type_md5:
            status->matching_output_size = false;
            status->fixed_output_size = true;
            status->fixed_output_length = 16;
            break;
        case bencryption_type_sha1:
            status->matching_output_size = false;
            status->fixed_output_size = true;
            status->fixed_output_length = 20;
            break;
        /* add RSA/DSA later, output is dependent on key size */
        default:
            return BSETTOP_ERROR(berr_not_available);
        }
        return b_ok;
    } else {
        return BSETTOP_ERROR(berr_invalid_parameter);
    }
}

bresult bcrypto_process(bcrypto_t crypto, uint8_t  *in_buffer, unsigned long in_buffer_length, uint8_t  *out_buffer, unsigned long out_buffer_length)
{
    NEXUS_DmaJobBlockSettings dmaJobBlockSettings;
    NEXUS_DmaJobSettings dmaJobSettings;
    NEXUS_DmaJobHandle dmaJobHandle;
    bresult rc = b_ok;
    NEXUS_Error nrc;
    BDBG_ASSERT(crypto);
    switch(crypto->settings.encryption.type) {
    case bencryption_type_des:
    case bencryption_type_3des:
    case bencryption_type_aes:

        if (!in_buffer || !out_buffer || (in_buffer_length != out_buffer_length) || 
                (in_buffer_length == 0) || 
                ((crypto->settings.operation != bcrypto_operation_encrypt) && (crypto->settings.operation != bcrypto_operation_decrypt)))
            return BSETTOP_ERROR(berr_invalid_parameter);

        if (!crypto->keyHandle) {
            crypto->keyHandle = b_keyslot_m2m_allocate(&(crypto->settings.encryption),crypto->settings.operation == bcrypto_operation_encrypt, false);
            if (!crypto->keyHandle) {
                BDBG_ERR(("Error allocating keyslot!"));
                return BSETTOP_ERROR(berr_external_error);
            }
        }

        NEXUS_DmaJob_GetDefaultSettings(&dmaJobSettings);
        dmaJobSettings.numBlocks = 1;
        dmaJobSettings.keySlot = crypto->keyHandle;
        switch(crypto->settings.data_format) {
        case bcrypto_data_format_ts:
            dmaJobSettings.dataFormat = NEXUS_DmaDataFormat_eMpeg;
            break;
        case bcrypto_data_format_dss:
            dmaJobSettings.dataFormat = NEXUS_DmaDataFormat_eDss;
            break;
        default:
            dmaJobSettings.dataFormat = NEXUS_DmaDataFormat_eBlock;
        }
        dmaJobHandle = NEXUS_DmaJob_Create(g_dma.hDma,&dmaJobSettings);
        if (!dmaJobHandle) {
            BSETTOP_ERROR(berr_external_error);
        }
        NEXUS_DmaJob_GetDefaultBlockSettings(&dmaJobBlockSettings);
        dmaJobBlockSettings.pSrcAddr = in_buffer;
        dmaJobBlockSettings.blockSize = in_buffer_length;
        dmaJobBlockSettings.pDestAddr = out_buffer;
        dmaJobBlockSettings.cached = true;
        dmaJobBlockSettings.resetCrypto = true;
        dmaJobBlockSettings.scatterGatherCryptoStart = true;
        dmaJobBlockSettings.scatterGatherCryptoEnd = true;

        nrc = NEXUS_DmaJob_ProcessBlocks(dmaJobHandle, &dmaJobBlockSettings, 1);
        if (nrc == NEXUS_DMA_QUEUED || nrc == NEXUS_SUCCESS)
            rc = b_ok;
        {
            NEXUS_DmaJobStatus status;
            NEXUS_DmaJob_GetStatus(dmaJobHandle, &status);
            while (status.currentState != NEXUS_DmaJobState_eComplete) {
                BKNI_Sleep(1);
                NEXUS_DmaJob_GetStatus(dmaJobHandle, &status);
            }
        }
        NEXUS_DmaJob_Destroy(dmaJobHandle);
        break;
    default:
        return BSETTOP_ERROR(berr_not_available);
    }
    return rc;
}

void bcrypto_close(bcrypto_t crypto)
{
    if (crypto) {
        if (crypto->keyHandle) {
            b_keyslot_m2m_free(crypto->keyHandle);
            crypto->keyHandle = NULL;
        }
        BKNI_Memset(crypto, 0, sizeof(struct bcrypto));
    }
}

uint8_t *bcrypto_alloc(unsigned buffer_length)
{
    void *rp = NULL;
    NEXUS_MemoryAllocationSettings memSettings;
    NEXUS_Memory_GetDefaultAllocationSettings(&memSettings);
    if (NEXUS_Memory_Allocate(buffer_length, NULL, &rp)) {
        return NULL;
    }
    return (uint8_t *)rp;
#if 0
    if (b_root.dma.dma && b_root.dma.dma_mem) {
        return BMEM_AllocAligned(b_root.transport.xpt->hMemory, buffer_length, 2, 0);
    }
#endif
}

void bcrypto_free(uint8_t *buffer)
{
    if (buffer)
        NEXUS_Memory_Free(buffer);
#if 0
    BMEM_Free(b_root.transport.xpt->hMemory, buffer);
#endif
}

#else


void bcrypto_settings_init(bcrypto_settings *settings)
{
    BSTD_UNUSED(settings);
}


bcrypto_t bcrypto_open(const bcrypto_settings *settings)
{
    BSTD_UNUSED(settings);
    return NULL;
}

bresult bcrypto_status(bcrypto_t crypto, bcrypto_status_t *status)
{
    BSTD_UNUSED(crypto);
    BSTD_UNUSED(status);
    return 0;
}

bresult bcrypto_process(bcrypto_t crypto, uint8_t  *in_buffer, unsigned long in_buffer_length, uint8_t  *out_buffer, unsigned long out_buffer_length)
{
    BSTD_UNUSED(crypto);
    BSTD_UNUSED(in_buffer);
    BSTD_UNUSED(in_buffer_length);
    BSTD_UNUSED(out_buffer);
    BSTD_UNUSED(out_buffer_length);
    return 0;
}

void bcrypto_close(bcrypto_t crypto)
{
    BSTD_UNUSED(crypto);
}

uint8_t *bcrypto_alloc(unsigned buffer_length)
{
    BSTD_UNUSED(buffer_length);
    return NULL;
}
    
void bcrypto_free(uint8_t *buffer)
{
    BSTD_UNUSED(buffer);
}

#endif
