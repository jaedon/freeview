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
#include <stdio.h>

#include "bstd.h"
#include "bdbg.h"
#include "bkni.h"

ENTER_PK_NAMESPACE_CODE;
#ifdef PRDY_ROBUSTNESS
#include "./inc/oem_brcm.h"

#define BDVD_PLAYREADY_MAX_BIN_SIZE   (BDVD_SIZE_IN_BYTES_8K)

BDBG_MODULE(oem_key_region);

static uint8_t prdyBinFile[BDVD_PLAYREADY_MAX_BIN_SIZE];  


/**********************************************************************
** Function:    _ReadBinFile
** Synopsis:    Read all the necessary information needed to recover and decrypt playready keys from 
** the file system
** Arguments:   [pKi] Pointer to sturcture containing the keys data.
** Arguments:   [drm] DRM type to read from flash.
** Returns:     RM_SUCCESS when success
** Notes:
***********************************************************************/
DRM_RESULT _ReadBinFile(oem_ctx_t *pOemCtx)
{
    DRM_RESULT  dr     = DRM_SUCCESS;
    OEM_FILEHDL hfp    = OEM_INVALID_HANDLE_VALUE;
    uint32_t    DRM_DataType;
    DRM_DWORD   cbRead = 0;
    DRM_DWORD   cbFile = 0;
    DRM_SHA_CONTEXT sha_ctx;
    DRM_BYTE shaOut [ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ] = {0};
    DRM_WCHAR *f_pwszPath;
    int ii = 0;

    uint8_t *p;
    bdvd_playready_bin_header_t   *pPrBinHeader;
    bdvd_playready_bin_key_data_t *pPrBinData;

    bdvd_wmdrm_srm_header_t       *pWmdrmSrmHeader;
    bdvd_wmdrm_srm_key_data_t     *pWmdrmSrmData;

    uint32_t encTemplateSize;

    uint32_t group_cert_offset = sizeof(bdvd_playready_bin_header_t) + sizeof(bdvd_playready_bin_key_data_t);
    uint8_t *pSignature = (uint8_t *)Oem_MemHeapAlloc((DRM_DWORD)BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES);

    ChkMem(pSignature);
    if(pOemCtx == NULL)
        ChkDR(DRM_E_INVALIDARG);

    f_pwszPath = (DRM_WCHAR *) bdrm_get_pr_bin_fname();

    hfp = Oem_File_Open( NULL,
                        f_pwszPath, 
                        OEM_GENERIC_READ, 
                        OEM_FILE_SHARE_READ, 
                        OEM_OPEN_EXISTING, 
                        OEM_ATTRIBUTE_HIDDEN );

    if ( hfp != OEM_INVALID_HANDLE_VALUE )
    {
        if ( !Oem_File_GetSize( hfp, &cbFile ) )
        {
            BDBG_ERR(("%s - Oem_File_GetSize() failed", __FUNCTION__));
            ChkDR( DRM_E_FILEREADERROR );
        }

        if(cbFile > BDVD_PLAYREADY_MAX_BIN_SIZE){
            BDBG_ERR(("Playready bin file is too big to fit in the pre-allocated buffer!\n"));
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

        if ( !Oem_File_SetFilePointer( hfp, 0, OEM_FILE_BEGIN, NULL ) )
        {
            BDBG_ERR(("%s - Oem_File_SetFilePointer() failed", __FUNCTION__));
            ChkDR( DRM_E_FILESEEKERROR );
        }

        if (!Oem_File_Read( hfp, prdyBinFile, cbFile, &cbRead ) || cbRead != cbFile )
        {
            BDBG_ERR(("%s - Oem_File_Read() failed", __FUNCTION__));
            ChkDR( DRM_E_FILEREADERROR );
        }
    
        pPrBinHeader = (bdvd_playready_bin_header_t *) prdyBinFile;
        pPrBinData   = (bdvd_playready_bin_key_data_t *)(prdyBinFile + sizeof(bdvd_playready_bin_header_t));

        // Data is in big endian in flash. Convert it to native processor.
        DRM_DataType = GET_UINT32_FROM_BUF((uint8_t *)pPrBinHeader);

        BDBG_MSG(("%s -DRM_DataType %x\n", __FUNCTION__, DRM_DataType));

        // Validate key region info.
        if(DRM_DataType != BDVD_DRM_DATA_PLAYREADY_BIN){
            BDBG_ERR(("%s -DRM_DataType != BDVD_DRM_DATA_PLAYREADY_BIN", __FUNCTION__));
            ChkDR(DRM_E_PRIVKEYREADERROR);
        }

        pOemCtx->pr_ki.info.custKeySelect = pPrBinHeader->cust_key;
        pOemCtx->pr_ki.info.keyVarLow     = pPrBinHeader->var_lo;
        pOemCtx->pr_ki.info.keyVarHigh    = pPrBinHeader->var_hi;

        MEMCPY(pOemCtx->pr_ki.info.procInForKey3, pPrBinData->SerialNumber, BDVD_DRM_SERIAL_NUMBER_LENGTH);
        MEMCPY(pOemCtx->pr_ki.serial_nb, pPrBinData->SerialNumber, BDVD_DRM_SERIAL_NUMBER_LENGTH);
        MEMCPY(&pOemCtx->pr_ki.data, &pPrBinData->playready_data, sizeof(bdvd_playready_data_t));

        pOemCtx->zgroupCertSize = GET_UINT32_FROM_BUF(&pPrBinData->DeviceCertificateTemplateSize);

        // Data is in big endian in flash. Convert it to native processor.
        DRM_DataType = GET_UINT32_FROM_BUF(&pOemCtx->pr_ki.data);
        if(DRM_DataType != BDVD_DRM_DATA_PLAYREADY){
            BDBG_ERR(("%s -DRM_DataType != BDVD_DRM_DATA_PLAYREADY", __FUNCTION__));
            ChkDR(DRM_E_PRIVKEYREADERROR);
        }

        MEMCPY(pSignature, &prdyBinFile[group_cert_offset + pOemCtx->zgroupCertSize], BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES);
         // Signature is encrypted, decrypt it.
        dr = _Crypto_AesEcb(pOemCtx->commonCryptoHandle, CommonCrypto_eCustKey, &pOemCtx->pr_ki.info, pSignature,
                            BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES, pPrBinData->playready_data.key, NEXUS_SecurityOperation_eDecrypt);
        ChkDR(dr);

        // Validate DRM bin file signature.
        dr = DRM_SHA_Init(&sha_ctx, eDRM_SHA_1);
        if(dr == DRM_SUCCESS){
            // We must skip over the header to compute the hash. The header is not taking into accound in the hash computations.
            dr = DRM_SHA_Update(&prdyBinFile[sizeof(bdvd_playready_bin_header_t)], sizeof(bdvd_playready_bin_key_data_t) + pOemCtx->zgroupCertSize, &sha_ctx);
            if(dr == DRM_SUCCESS){
                dr = DRM_SHA_Finalize(&sha_ctx, 20, shaOut);
                ChkDR(dr);
            }
            else{
                ChkDR(DRM_E_CRYPTO_FAILED);
            }
        }

        if(BKNI_Memcmp(shaOut, pSignature, 20) == 0){
            pOemCtx->pr_keys_valid = true;
            printf("%s - Playready Signature Verification SUCCEEDED\n!!!", __FUNCTION__);
        }
        else {
            printf("%s - Playready Signature Verification Failed\n!!!", __FUNCTION__);
            ChkDR(DRM_E_CRYPTO_FAILED);
        }
        
        pOemCtx->pZGroupCert = &prdyBinFile[group_cert_offset];

        // Now, Process WMDRM region 
        pWmdrmSrmHeader = &prdyBinFile[group_cert_offset + pOemCtx->zgroupCertSize + BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES];
        p = pWmdrmSrmHeader;

        pWmdrmSrmData   = &prdyBinFile[group_cert_offset + pOemCtx->zgroupCertSize + BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES + sizeof(bdvd_wmdrm_srm_header_t)];

        // Data is in big endian in flash. Convert it to native processor.
        DRM_DataType = GET_UINT32_FROM_BUF((uint8_t *)pWmdrmSrmHeader);

        //Validate key region info.
        if(DRM_DataType != BDVD_DRM_DATA_WMDRM_SRM){
            BDBG_ERR(("%s -DRM_DataType != BDVD_DRM_DATA_WMDRM_SRM", __FUNCTION__));
            ChkDR(DRM_E_PRIVKEYREADERROR);
        }

        pOemCtx->wmdrm_ki.info.custKeySelect = pPrBinHeader->cust_key;
        pOemCtx->wmdrm_ki.info.keyVarLow     = pPrBinHeader->var_lo;
        pOemCtx->wmdrm_ki.info.keyVarHigh    = pPrBinHeader->var_hi;

        MEMCPY(pOemCtx->wmdrm_ki.info.procInForKey3, pWmdrmSrmData->SerialNumber, BDVD_DRM_SERIAL_NUMBER_LENGTH);
        MEMCPY(pOemCtx->wmdrm_ki.serial_nb, pWmdrmSrmData->SerialNumber, BDVD_DRM_SERIAL_NUMBER_LENGTH);
        MEMCPY(&pOemCtx->wmdrm_ki.data, &pWmdrmSrmData->wmdrm_data, sizeof(bdvd_wmdrm_pd_data_t));

        pOemCtx->devTemplateSize = GET_UINT32_FROM_BUF(&pWmdrmSrmHeader->reserved);
        if(pOemCtx->devTemplateSize % 16 != 0){
            encTemplateSize = pOemCtx->devTemplateSize + (16 - (pOemCtx->devTemplateSize % 16));
        }
        else {
            encTemplateSize = pOemCtx->devTemplateSize;
        }


        // Data is in big endian in flash. Convert it to native processor.
        DRM_DataType = GET_UINT32_FROM_BUF(&pOemCtx->wmdrm_ki.data);
       
        if(DRM_DataType != BDVD_DRM_DATA_WMDRM_PD){
            BDBG_ERR(("%s -DRM_DataType != BDVD_DRM_DATA_WMDRM_PD", __FUNCTION__));
            ChkDR(DRM_E_PRIVKEYREADERROR);
        }

        MEMCPY(pSignature, &prdyBinFile[cbRead - BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES], BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES);


        // Signature is encrypted, decrypt it.
        dr = _Crypto_AesEcb(pOemCtx->commonCryptoHandle, CommonCrypto_eCustKey, &pOemCtx->wmdrm_ki.info, pSignature,
                            BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES, pWmdrmSrmData->wmdrm_data.key, NEXUS_SecurityOperation_eDecrypt);
        ChkDR(dr);

        // Validate DRM bin file signature.
        dr = DRM_SHA_Init(&sha_ctx, eDRM_SHA_1);
        if(dr == DRM_SUCCESS){
            // We must skip over the header to compute the hash. The header is not taking into accound in the hash computations.
            // Start hashing from the beginning of the SRM data all the way down to the signature.
            dr = DRM_SHA_Update((uint8_t *)pWmdrmSrmData, sizeof(bdvd_wmdrm_srm_key_data_t) + encTemplateSize, &sha_ctx);
            if(dr == DRM_SUCCESS){
                dr = DRM_SHA_Finalize(&sha_ctx, 20, shaOut);
                ChkDR(dr);
            }
            else{
                ChkDR(DRM_E_CRYPTO_FAILED);
            }
        }

        if(BKNI_Memcmp(shaOut, pSignature, 20) == 0){
            pOemCtx->wmdrm_keys_valid = true;
            printf("%s - WMDRM Signature Verification SUCCEEDED\n!!!", __FUNCTION__);
        }
        else {
            printf("%s - WMDRM Signature Verification Failed\n!!!", __FUNCTION__);
            ChkDR(DRM_E_CRYPTO_FAILED);
        }
        pOemCtx->pDevTemplate  =  &prdyBinFile[group_cert_offset + pOemCtx->zgroupCertSize + BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES + sizeof(bdvd_wmdrm_srm_header_t) + sizeof(bdvd_wmdrm_srm_key_data_t)];
    }
    else
    {
        BDBG_ERR(("%s - Playready bin file not found", __FUNCTION__));
        ChkDR( DRM_E_FILENOTFOUND );
    }

ErrorExit:
    if(pSignature) Oem_MemHeapFree(pSignature);
    
    if( hfp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hfp );
    }
    BDBG_MSG(("%s - exiting dr %x", __FUNCTION__, dr));
    return dr;
}

#endif
EXIT_PK_NAMESPACE_CODE;
