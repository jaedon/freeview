/***************************************************************************
 *     Copyright (c) 2002-2012, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 *
 * $brcm_Workfile: prdy_decryptor.c $
 * $brcm_Revision: 10 $
 * $brcm_Date: 10/1/12 10:36a $
 *
 * Module Description:
 *
 * Playready library
 * 
 * Revision History:
 *
 * $brcm_Log: /BSEAV/lib/playready/prdy_decryptor.c $
 * 
 * 10   10/1/12 10:36a hugost
 * SW7425-4009: Add new decryption function for Smooth Streaming
 * 
 * 9   7/17/12 4:13p piyushg
 * SW7425-3281: Add playready kernel mode support and nexus drm
 * implementation.
 * 
 * 8   6/5/12 3:02p hugost
 * SW7425-3169: Modified playready to use new version of common crypto
 * library
 * 
 * 7   11/10/11 6:57a hugost
 * SW7425-1625: Set Nexus_SecurityKeyType to NEXUS_SecurityKeyType_eOdd.
 * 
 * 6   11/1/11 9:51a hugost
 * SW7425-1625: Added MP4 fragment support
 * 
 * 5   10/25/11 3:18p piyushg
 * SW7425-1607: Added file header to track revision history.
 ***************************************************************************/

#include "bstd.h"
#include "bkni.h"
#include "bkni_multi.h"

#include <drmcommon.h>
#include <drmtypes.h>
#include <drmcipher.h>
#include <prdy_decryptor.h>
#include <drmlicense.h>
#include <drmdevcert.h>
#include <drmblackbox.h>
#include <drmhds.h>
#include <drmsecurestore.h>
#include <drmlicstore.h>
#include <drmliceval.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>

#include "biovec.h"
#include "prdy_core.h"
#include "prdy_priv.h"

#include "../asf/basf_parser.h"

#include "nexus_memory.h"
#ifndef __KERNEL__
#include <stdio.h>
#endif

BDBG_MODULE(bdrm_content_decryptor);

/* local defines */
#define PAYLOAD_BUFFER_SIZE     (2048 * 1024) /* Media obj can be a bit big in aes ctr mode */
#define BIOVEC_BIG_NUM          (1024*1024*64)   
#define DRM_CRYPT_PAYLOAD_MIN   (15)
#define WMDRM_EKL_SIZE (80)

/* debug trace fw macro */
#define BDBG_MSG_TRACE(x)       BDBG_MSG(x)

/* local forward decls */
typedef DRM_RESULT (* WMDRM_DEC_FP)(bdrm_decryptor_t hDecryptor, uint8_t *pdata, size_t len, uint64_t* repl_seq_num);
static DRM_RESULT do_decrypt_sw (bdrm_decryptor_t hDecryptor, uint8_t *pdata, size_t len, uint64_t* repl_seq_num);

#ifndef PRDY_USE_SW_DECRYPTION
static DRM_RESULT do_decrypt_hw (bdrm_decryptor_t hDecryptor, uint8_t *pdata, size_t len, uint64_t* repl_seq_num);
#endif

extern bdrm_err bdrm_get_decrypt_context (bdrm_t drm, DRM_DECRYPT_CONTEXT** ctx);

/* sets decryptor magic */
void 
bdrm_decryptor_set_magic (bdrm_decryptor_t dcrypt, unsigned int magic) 
{ 
    dcrypt->magic = magic; 
}

/* retr decryptor magic */
static unsigned int 
bdrm_decryptor_get_magic (bdrm_decryptor_t dcrypt) 
{ 
    return dcrypt->magic; 
}

/* retr decryptor validity */
unsigned int 
bdrm_decryptor_valid (bdrm_decryptor_t dcrypt) 
{ 
    return (bdrm_decryptor_get_magic(dcrypt) == BDRM_DCRYPT_MAGIC); 
}

bdrm_err 
bdrm_decryptor_assign (bdrm_decryptor_t hDecryptor, bdrm_t drm)
{
    bdrm_err ret = bdrm_err_fail;
    DRM_DECRYPT_CONTEXT* dcrypt_ctx = NULL;

    bdrm_get_decrypt_context(drm, &dcrypt_ctx);

    /* 
    ** Decrypting the content key and calling DRM_CPHR_Init() will be done
    ** by DRM_MGR_Bind(). The bound DRM_CIPHER_CONTEXT will contain the 
    ** initialized structs we need for DRM_CPHR_InitDecrypt() below. 
    ** DRM_MGR_Bind() casts DRM_DECRYPT_CONTEXT to DRM_CIPHER_CONTEXT,
    ** so that's where the initialized structs wind up. We'll make our own
    ** copy for our use, since we're running outside the BlackBox module.
    */
    if (dcrypt_ctx != NULL) {
        BKNI_Memcpy((void *)&hDecryptor->MsDecryptCtx, 
                    (void *)dcrypt_ctx, 
                    sizeof(DRM_DECRYPT_CONTEXT));
        ret = bdrm_err_ok;
    }

    return (ret);
}

bdrm_err 
bdrm_decryptor_set_encr_state (bdrm_decryptor_t hDecryptor, bdrm_encr_state asf_encr)
{
    bdrm_err ret = bdrm_err_fail;
    
    if(hDecryptor != NULL){
        hDecryptor->asf_encr = asf_encr;
        ret = bdrm_err_ok;
    }
    return (ret);
}

bdrm_err 
bdrm_decryptor_create (bdrm_decryptor_t* hDecryptor)
{
    bdrm_err ret = bdrm_err_fail;
    bdrm_decryptor *DecCntx = BKNI_Malloc(sizeof(bdrm_decryptor));

#ifndef PRDY_USE_SW_DECRYPTION
    NEXUS_SecurityKeySlotSettings  keySlotSettings;
    CommonCryptoSettings           cryptoSettings;
    CommonCryptoKeyConfigSettings  algSettings;
#endif

    BDBG_ASSERT(DecCntx); 
    BKNI_Memset(DecCntx, 0, sizeof(bdrm_decryptor));
    if ((DecCntx != NULL) && (hDecryptor != NULL)) {
        /* dyn-alloc payload buffer */

        DecCntx->dyn.buf = (uint8_t *)Oem_MemHeapAlloc(PAYLOAD_BUFFER_SIZE); 
        if(DecCntx->dyn.buf == NULL) goto ErrorExit;

        DecCntx->dyn.ekl_buf = (uint8_t *)Oem_MemHeapAlloc(WMDRM_EKL_SIZE); 
        if(DecCntx->dyn.ekl_buf == NULL) goto ErrorExit;

        DecCntx->dyn.size = PAYLOAD_BUFFER_SIZE;
        BDBG_ASSERT(DecCntx->dyn.buf != NULL);

        DecCntx->dyn.aes_ctr_padding = (uint8_t *)Oem_MemHeapAlloc(16); 
        if(DecCntx->dyn.aes_ctr_padding == NULL) goto ErrorExit;

#ifndef PRDY_USE_SW_DECRYPTION

        CommonCrypto_GetDefaultSettings(&cryptoSettings);
        DecCntx->cryptoHandle = CommonCrypto_Open(&cryptoSettings);
        if(DecCntx->cryptoHandle == NULL) goto ErrorExit;

         /* Allocate key slot for AES Counter mode */
        NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
        keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

        DecCntx->keyCtr = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
        if(DecCntx->keyCtr == NULL) 
        {
            BDBG_ERR(("%s - Failure to allocate key slot for keyCtr\n", __FUNCTION__));
            goto ErrorExit;
        }

        CommonCrypto_GetDefaultKeyConfigSettings(&algSettings);
        algSettings.keySlot = DecCntx->keyCtr;
        algSettings.settings.opType = NEXUS_SecurityOperation_eDecrypt;
        algSettings.settings.algType = NEXUS_SecurityAlgorithm_eAes128;
        algSettings.settings.algVariant = NEXUS_SecurityAlgorithmVariant_eCounter;
        algSettings.settings.termMode = NEXUS_SecurityTerminationMode_eClear;
        algSettings.settings.enableExtKey = true;
        algSettings.settings.enableExtIv = true;
        algSettings.settings.aesCounterSize = NEXUS_SecurityAesCounterSize_e64Bits;


        /* Configure key slot for AES Counter mode */
        if(CommonCrypto_LoadKeyConfig( DecCntx->cryptoHandle,
                                            &algSettings) != NEXUS_SUCCESS)
        {
            BDBG_ERR(("%s - CommonCrypto_ConfigAlg failed aes ctr\n", __FUNCTION__));
            goto ErrorExit;
        }


        /* Allocate key slot for WMDRM */
        NEXUS_Security_GetDefaultKeySlotSettings(&keySlotSettings);
        keySlotSettings.keySlotEngine = NEXUS_SecurityEngine_eM2m;

        DecCntx->keyWmdrm = NEXUS_Security_AllocateKeySlot(&keySlotSettings);
        if(DecCntx->keyWmdrm == NULL) 
        {
            BDBG_ERR(("%s - Failure to allocate key slot for keyWmdrm\n", __FUNCTION__));
            goto ErrorExit;
        }


        CommonCrypto_GetDefaultKeyConfigSettings(&algSettings);
        algSettings.keySlot = DecCntx->keyWmdrm;
        algSettings.settings.opType = NEXUS_SecurityOperation_eDecrypt;
        algSettings.settings.algType = NEXUS_SecurityAlgorithm_eWMDrmPd;
        algSettings.settings.algVariant = NEXUS_SecurityAlgorithmVariant_eEcb;
        algSettings.settings.termMode = NEXUS_SecurityTerminationMode_eClear;
        algSettings.settings.enableExtKey = true;
        algSettings.settings.enableExtIv = true;

        /* Configure key slot for WMDRM */
        if(CommonCrypto_LoadKeyConfig( DecCntx->cryptoHandle,
                                            &algSettings) != NEXUS_SUCCESS)
        {
            BDBG_ERR(("%s - CommonCrypto_ConfigAlg failed wmdrm\n", __FUNCTION__));
            goto ErrorExit;
        }
#endif

        *hDecryptor = DecCntx;
        ret = bdrm_err_ok;
    }

    return (ret);

ErrorExit:
#ifndef PRDY_USE_SW_DECRYPTION
    if(DecCntx->keyCtr) NEXUS_Security_FreeKeySlot(DecCntx->keyCtr);
    if(DecCntx->keyWmdrm) NEXUS_Security_FreeKeySlot(DecCntx->keyWmdrm);
    if(DecCntx->cryptoHandle) CommonCrypto_Close(DecCntx->cryptoHandle);
#endif
    if(DecCntx->dyn.buf != NULL) Oem_MemHeapFree(DecCntx->dyn.buf);
    if(DecCntx->dyn.ekl_buf != NULL)  Oem_MemHeapFree(DecCntx->dyn.ekl_buf);
    if(DecCntx->dyn.aes_ctr_padding != NULL)  Oem_MemHeapFree(DecCntx->dyn.aes_ctr_padding);
    if(DecCntx != NULL) BKNI_Free(DecCntx);

    return (ret);
}

/* decryptor buffer factory, builds flat buffers from cursors and sends them
   up for descrambling 
*/
void 
bdrm_decryptor_packet_filter (bdrm_decryptor_t hDecryptor, size_t media_obj_length,
    batom_cursor *payload, const struct basf_payload_info *info)
{    
    struct batom_checkpoint start;
    unsigned char* ps;      /* source buffer */
    unsigned char* pdb;     /* pointer to debug */
    
    size_t size0 = 0; 
    static int run = 0; 
    unsigned int off = 0, copy = 0;
    unsigned pos = 0;
    uint64_t repl_seq_num;
    WMDRM_DEC_FP do_decrypt_fp;
    BSTD_UNUSED(run);
    BSTD_UNUSED(size0);
    
    if (hDecryptor == NULL) {
        BDBG_MSG(("no active decryptor"));
        return;
    }

    /* save cursor position on entry */
    batom_cursor_save(payload, &start);
    BKNI_Memcpy(&repl_seq_num, hDecryptor->repl_data, BDRMND_SAMPLE_ID_LEN);

    /* make sure the linear buffer can hold the payload */
    if(media_obj_length > hDecryptor->dyn.size) {
        Oem_MemHeapFree(hDecryptor->dyn.buf);
        hDecryptor->dyn.buf = Oem_MemHeapAlloc(media_obj_length);
        if(hDecryptor->dyn.buf == NULL) {
            BDBG_WRN(("realloc-ed to %d failed", media_obj_length));
            return;
        }

        hDecryptor->dyn.size = media_obj_length;
        BDBG_WRN(("realloc-ed to %d", media_obj_length));
    }

    size0 = media_obj_length;

    batom_cursor_copy(payload, hDecryptor->dyn.buf, media_obj_length);
    pdb = hDecryptor->dyn.buf;

#ifdef PRDY_USE_SW_DECRYPTION
    do_decrypt_fp = do_decrypt_sw;
#else
    if(hDecryptor->asf_encr == bdrm_encr_wmdrm)
        do_decrypt_fp = (size0 < 16) ? do_decrypt_sw :  do_decrypt_hw;
    else 
        do_decrypt_fp = do_decrypt_hw;
#endif

    if ((hDecryptor->dyn.buf != NULL) && (DRM_SUCCESS == do_decrypt_fp( hDecryptor, 
                    hDecryptor->dyn.buf, media_obj_length, &repl_seq_num))) {

        BDBG_MSG(("o[0]=%.2X o[1]=%.2X o[2]=%.2X o[-2]=%.2X o[-1]=%.2X o[-0]=%.2X len=%d size0=%d payload=%d", 
                    ((uint8_t*)pdb)[0], ((uint8_t*)pdb)[1], ((uint8_t*)pdb)[2],
                    ((uint8_t*)pdb)[size0-3], ((uint8_t*)pdb)[size0-2], 
                    ((uint8_t*)pdb)[size0-1],
                    info->payload_length, size0, run));        

        batom_cursor_rollback( payload, &start ); 

        ps = hDecryptor->dyn.buf;
        pos = 0; run++; 

        /* copy decrypted data back into app */
        if (payload->left >= (int)media_obj_length) {           /* direct copy */
            BDBG_MSG(("pld->left(%d) > info->pld_len(%d)", payload->left, media_obj_length));
            BKNI_Memcpy((uint8_t*)payload->cursor, ps, media_obj_length);
        } else {
            BDBG_MSG(("pld->left(%d) < info->pld_len(%d) (straddle)", payload->left, media_obj_length));            
            for (off=0; pos < media_obj_length; batom_cursor_skip(payload, payload->left+off)) {    
                /* straddle copy */
                if(BATOM_IS_EOF(payload)) {
                    BDBG_ASSERT(0); break;                          /* shouldn't happen */
                }
                BDBG_MSG(("straddle copy: %d pos=%d", payload->left, pos));
                if (pos+payload->left+off <= media_obj_length) {
                    copy = payload->left+off;               
                } else {
                    copy = media_obj_length-pos;
                }
                BKNI_Memcpy((uint8_t*)payload->cursor-off, ps, copy);
                ps += copy;
                pos += copy;
                off = 1;                
            }
        }

    } /* if (hDecryptor->dyn.buf ... */

    batom_cursor_rollback(payload, &start);

    return;
}

void bdrm_decryptor_destroy_se(bdrm_decryptor_t hDecryptor)
{
    if(hDecryptor != NULL){
        BKNI_Memset(hDecryptor->se, 0, sizeof(hDecryptor->se));
    }

    return;
}

void 
bdrm_decryptor_destroy (bdrm_decryptor_t hDecryptor)
{    
    BDBG_ASSERT(hDecryptor); 
    hDecryptor->dyn.size = 0;
    
    /* free dynalloc-ed mem */
    Oem_MemHeapFree(hDecryptor->dyn.buf);
    Oem_MemHeapFree(hDecryptor->dyn.ekl_buf);
    Oem_MemHeapFree(hDecryptor->dyn.aes_ctr_padding);
    bdrm_decryptor_destroy_se(hDecryptor);
#ifndef PRDY_USE_SW_DECRYPTION
    if(hDecryptor->keyWmdrm) NEXUS_Security_FreeKeySlot(hDecryptor->keyWmdrm);
    if(hDecryptor->keyCtr) NEXUS_Security_FreeKeySlot(hDecryptor->keyCtr);
    if(hDecryptor->cryptoHandle) CommonCrypto_Close(hDecryptor->cryptoHandle);
#endif

    BKNI_Free(hDecryptor);
    BDBG_MSG(("decryptor destroyed\n"));

}


#ifndef PRDY_USE_SW_DECRYPTION
DRM_RESULT 
do_decrypt_hw (bdrm_decryptor_t hDecryptor, uint8_t *pdata, size_t len, uint64_t* repl_seq_num)
{
    DRM_RESULT dr=DRM_SUCCESS;
    uint32_t cbClear;
    DRM_CIPHER_CONTEXT *cipher_ctx;
    uint32_t ekl_size = 0;
    NEXUS_Error rc;

    NEXUS_KeySlotHandle  keyHandle = 0;
    CommonCryptoJobSettings jobSettings;

#undef COMPARE_SW_HW_DEC
#ifdef COMPARE_SW_HW_DEC
    static int dumped = 0;
    uint8_t *tmp;
    uint64_t repl_seq_num2 = *repl_seq_num;
    tmp = BKNI_Malloc(len);

    memcpy(tmp, pdata, len);
    do_decrypt_sw(hDecryptor, tmp, len, &repl_seq_num2);
#endif

    if (len < DRM_CRYPT_PAYLOAD_MIN) {
        BDBG_MSG(( "do_decrypt(): Payload is small (%d)", len )); 
    }

    /* 
    ** Decrypting the content key and calling DRM_CPHR_Init() will be done
    ** by DRM_MGR_Bind(). The bound DRM_CIPHER_CONTEXT will contain the 
    ** initialized structs we need for DRM_CPHR_InitDecrypt() below. 
    ** DRM_MGR_Bind() casts DRM_MANAGER_DECRYPT_CONTEXT to DRM_CIPHER_CONTEXT,
    ** so that's where the initialized structs wind up. We'll make our own
    ** copy for our use, since we're running outside the BlackBox module.
    */
    cipher_ctx = (DRM_CIPHER_CONTEXT*)(hDecryptor->MsDecryptCtx.rgbBuffer);
    if (!cipher_ctx->fInited)
    {
        /* When the cipher is not initialized, it implies that the stream we are playing is not encrypted. 
           Hence there is no need to start a dma transfer. We can just feed the source buffer back to the application. */
        return dr;
    }

    if(hDecryptor->asf_encr == bdrm_encr_aes_ctr){
   
        BKNI_Memcpy(hDecryptor->dyn.ekl_buf,   cipher_ctx->rgbContentKey+8, 8);	/* Copy key.  H and L need to be swapped */
	    BKNI_Memcpy(hDecryptor->dyn.ekl_buf+8, cipher_ctx->rgbContentKey, 8);	

        BKNI_Memset(&hDecryptor->dyn.ekl_buf[16], 0, 8);           
        BKNI_Memcpy(&hDecryptor->dyn.ekl_buf[24], repl_seq_num, 8);
    
        ekl_size = AES_EKL_SIZE;
      
        keyHandle = hDecryptor->keyCtr;
    }
    else if(hDecryptor->asf_encr == bdrm_encr_wmdrm){

        uint8_t *pEkl= hDecryptor->dyn.ekl_buf;
        DRM_BYTE *last_15_bytes;
        
        if (len < DRM_CRYPT_PAYLOAD_MIN) {
            last_15_bytes = (DRM_BYTE*)pdata;
        } else {
            last_15_bytes = (DRM_BYTE*)pdata + (len - DRM_CRYPT_PAYLOAD_MIN);
        }

        /*8 byte DES key*/
        BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.shaOut[12],8);
        pEkl+=8;

        /*8 byte RC4 content key (the one at the end of of the ASF paylaod */
        cbClear = len % 8;  /*Find Number of clear bytes*/
        BKNI_Memcpy(pEkl, &last_15_bytes[15-8-cbClear],8);
        pEkl+=8;

        /*8 byte D1 */
        /*D1 = "9.	From the 64 bytes obtained in step 4, use the last 8 bytes to XOR with the result of the DES encryption in step 8."*/
        BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.desS2[0],8);
        pEkl+=8;

        /*8 byte D0 */
        /*D0 = "From the 64 bytes obtained in step 4, the 8 bytes following the 48 bytes MAC key are used to XOR the RC4 content key obtained in step 5."*/
        BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.desS1[0],8);
        pEkl+=8;

        /*48 byte MAC key */
        BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.a1, 4);
        pEkl+=4;
        BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.b1, 4);
        pEkl+=4;
        BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.c1, 4);
        pEkl+=4;
        BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.d1, 4);
        pEkl+=4;
        BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.e1, 4);
        pEkl+=4;
        BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.f1, 4);
        pEkl+=4;
        BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.a2, 4);
        pEkl+=4;
        BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.b2, 4);
        pEkl+=4;
        BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.c2, 4);
        pEkl+=4;
        BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.d2, 4);
        pEkl+=4;
        BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.e2, 4);
        pEkl+=4;
        BKNI_Memcpy(pEkl, &cipher_ctx->cipher.cipherRC4.mackey.f2, 4);

        keyHandle = hDecryptor->keyWmdrm;
        ekl_size = WMDRM_EKL_SIZE;
    }

    NEXUS_DmaJob_GetDefaultBlockSettings(&hDecryptor->blks[0]);
    hDecryptor->blks[0].pSrcAddr                 = hDecryptor->dyn.ekl_buf;
    hDecryptor->blks[0].pDestAddr                = hDecryptor->dyn.ekl_buf;
    hDecryptor->blks[0].blockSize                = ekl_size;
    hDecryptor->blks[0].cached                   = true;
    hDecryptor->blks[0].resetCrypto              = true;
    hDecryptor->blks[0].scatterGatherCryptoStart = true;
    hDecryptor->blks[0].scatterGatherCryptoEnd   = false;

    NEXUS_DmaJob_GetDefaultBlockSettings(&hDecryptor->blks[1]);
    hDecryptor->blks[1].pSrcAddr                 = pdata;
    hDecryptor->blks[1].pDestAddr                = pdata;
    hDecryptor->blks[1].blockSize                = len;
    hDecryptor->blks[1].cached                   = true;
    hDecryptor->blks[1].resetCrypto              = true;
    hDecryptor->blks[1].scatterGatherCryptoStart = false;
    hDecryptor->blks[1].scatterGatherCryptoEnd   = true;

    CommonCrypto_GetDefaultJobSettings(&jobSettings);
    jobSettings.keySlot = keyHandle;
    
    rc = CommonCrypto_DmaXfer(hDecryptor->cryptoHandle, &jobSettings, hDecryptor->blks, 2);
    if(rc != NEXUS_SUCCESS)
    {
        BDBG_ERR(("%s - CommonCrypto_DmaXfer failed rc %x\n", __FUNCTION__, rc));
    }

#ifdef COMPARE_SW_HW_DEC
       
    if(memcmp(tmp, pdata, len) != 0)
    {
        int ii;
        printf("MISMATCH!, len %d\n", len);
        for(ii = 0; ii < len; ii++){
            if(tmp[ii] != pdata[ii]){
                printf("offset ii %d\n", ii);
            }

        }
    }
    BKNI_Free(tmp);
#endif
    
    return dr;
}
#endif

DRM_RESULT 
do_decrypt_sw (bdrm_decryptor_t hDecryptor, uint8_t *pdata, size_t len, uint64_t* repl_seq_num)
{
    DRM_BYTE *last_15_bytes;
    DRM_RESULT dr;
    DRM_AES_COUNTER_MODE_CONTEXT f_pCtrContext;
    uint8_t *nonce;

    if (len < DRM_CRYPT_PAYLOAD_MIN) {
        BDBG_MSG(( "do_decrypt(): Payload is small (%d)", len )); 
    }

    nonce = (uint8_t *)repl_seq_num;
    NETWORKBYTES_TO_QWORD(f_pCtrContext.qwInitializationVector, nonce, 0);

    if (len < DRM_CRYPT_PAYLOAD_MIN) {
        last_15_bytes = (DRM_BYTE*)pdata;
    } else {
        last_15_bytes = (DRM_BYTE*)pdata + (len - DRM_CRYPT_PAYLOAD_MIN);
    }

    dr = Drm_Reader_InitDecrypt( (DRM_DECRYPT_CONTEXT *)(hDecryptor->MsDecryptCtx.rgbBuffer), last_15_bytes, len); 
    if (dr == DRM_SUCCESS) {
		f_pCtrContext.bByteOffset = 0;
		f_pCtrContext.qwBlockOffset = (uint64_t)0;
        NETWORKBYTES_TO_QWORD(f_pCtrContext.qwInitializationVector, nonce, 0);

#if 0
    {
        FILE *res;
        uint32_t res_size, flen;
        char fname1[80] = "/mnt/nfs/views/captured_enc.wmv";

        res = fopen(fname1, "ab");
        res_size = fwrite(pdata, 1, len, res);
        fclose(res);
    }
#endif

        dr = Drm_Reader_Decrypt( (DRM_DECRYPT_CONTEXT *)(hDecryptor->MsDecryptCtx.rgbBuffer), &f_pCtrContext, pdata, len); 
        if(dr != 0){
            BDBG_ERR(("Drm_Reader_Decrypt error %x\n", dr));
        }
#if 0
    {
        FILE *res;
        uint32_t res_size, flen;
        char fname1[80] = "/mnt/nfs/views/captured_dec.wmv";

        res = fopen(fname1, "ab");
        res_size = fwrite(pdata, 1, len, res);
        fclose(res);
    }
#endif


    } else { 
        BDBG_MSG(("InitDecrypt failed = 0x%X", dr));
    }

    return dr;
}
