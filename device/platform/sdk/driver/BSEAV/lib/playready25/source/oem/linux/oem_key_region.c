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

#include "drm_metadata.h"

#define GET_UINT32_FROM_BUF(pBuf) \
    (((uint32_t)(((uint8_t*)(pBuf))[0]) << 24) | \
     ((uint32_t)(((uint8_t*)(pBuf))[1]) << 16) | \
     ((uint32_t)(((uint8_t*)(pBuf))[2]) << 8)  | \
     ((uint8_t *)(pBuf))[3])

#define SET_UINT32_IN_BUF(pBuf, val) \
{   *pBuf       = (uint8_t)((val & 0xFF000000) >> 24);  \
    *(pBuf + 1) = (uint8_t)((val & 0xFF0000) >> 16);    \
    *(pBuf + 2) = (uint8_t)((val & 0xFF00) >> 8);       \
    *(pBuf + 3) = (uint8_t)((val & 0xFF));			    \
}

ENTER_PK_NAMESPACE_CODE;
#ifdef PRDY_ROBUSTNESS

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
DRM_RESULT _ReadBinFile(DRM_VOID *f_pOEMContext)
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
    if(f_pOEMContext == NULL)
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
            printf("%s - Oem_File_GetSize() failed\n", __FUNCTION__);
            ChkDR( DRM_E_FILE_READ_ERROR );
        }

        if(cbFile > BDVD_PLAYREADY_MAX_BIN_SIZE){
            printf("Playready bin file is too big to fit in the pre-allocated buffer!\n");
            ChkDR(DRM_E_BUFFERTOOSMALL);
        }

        if ( !Oem_File_SetFilePointer( hfp, 0, OEM_FILE_BEGIN, NULL ) )
        {
            printf("%s - Oem_File_SetFilePointer() failed\n", __FUNCTION__);
            ChkDR( DRM_E_FILE_SEEK_ERROR );
        }

        if (!Oem_File_Read( hfp, prdyBinFile, cbFile, &cbRead ) || cbRead != cbFile )
        {
            printf("%s - Oem_File_Read() failed\n", __FUNCTION__);
            ChkDR( DRM_E_FILE_READ_ERROR );
        }
    
        pPrBinHeader = (bdvd_playready_bin_header_t *) prdyBinFile;
        pPrBinData   = (bdvd_playready_bin_key_data_t *)(prdyBinFile + sizeof(bdvd_playready_bin_header_t));

        // Data is in big endian in flash. Convert it to native processor.
        DRM_DataType = GET_UINT32_FROM_BUF((uint8_t *)pPrBinHeader);

        printf("%s -DRM_DataType %x\n", __FUNCTION__, DRM_DataType);

        // Validate key region info.
        if(DRM_DataType != BDVD_DRM_DATA_PLAYREADY_BIN){
            printf("%s -DRM_DataType != BDVD_DRM_DATA_PLAYREADY_BIN\n", __FUNCTION__);
            ChkDR(DRM_E_PRIVKEY_READ_ERROR);
        }

		BCM_SetZGroupCertSize(f_pOEMContext, GET_UINT32_FROM_BUF(&pPrBinData->DeviceCertificateTemplateSize));

		dr = BCM_ReadBinFilePrdy(f_pOEMContext,
							pPrBinHeader->cust_key,
							pPrBinHeader->var_lo,
							pPrBinHeader->var_hi,
							pPrBinData->SerialNumber,
							(void *)&pPrBinData->playready_data,
							sizeof(bdvd_playready_data_t),
							(void *)&pPrBinData->DeviceCertificateTemplateSize,
							pSignature,
							prdyBinFile,
							group_cert_offset,
							pPrBinData->playready_data.key);
        ChkDR(dr);

        // Validate DRM bin file signature.
        dr = DRM_SHA_Init(&sha_ctx, eDRM_SHA_1);
        if(dr == DRM_SUCCESS){
            // We must skip over the header to compute the hash. The header is not taking into accound in the hash computations.
            dr = DRM_SHA_Update(&prdyBinFile[sizeof(bdvd_playready_bin_header_t)], sizeof(bdvd_playready_bin_key_data_t) + BCM_GetZGroupCertSize(f_pOEMContext), &sha_ctx);
            if(dr == DRM_SUCCESS){
                dr = DRM_SHA_Finalize(&sha_ctx, 20, shaOut);
                ChkDR(dr);
            }
            else{
                ChkDR(DRM_E_CRYPTO_FAILED);
            }
        }

        if(BKNI_Memcmp(shaOut, pSignature, 20) == 0){
            BCM_Setpr_keys_valid(f_pOEMContext, TRUE);
            printf("%s - Playready Signature Verification SUCCEEDED !!!\n", __FUNCTION__);
        }
        else {
            printf("%s - Playready Signature Verification Failed !!!\n", __FUNCTION__);
            ChkDR(DRM_E_CRYPTO_FAILED);
        }
        
		BCM_SetZGroupCert(f_pOEMContext, &prdyBinFile[group_cert_offset]);

        // Now, Process WMDRM region 
        pWmdrmSrmHeader = &prdyBinFile[group_cert_offset + BCM_GetZGroupCertSize(f_pOEMContext) + BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES];
        p = pWmdrmSrmHeader;

        pWmdrmSrmData   = &prdyBinFile[group_cert_offset + BCM_GetZGroupCertSize(f_pOEMContext) + BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES + sizeof(bdvd_wmdrm_srm_header_t)];

        // Data is in big endian in flash. Convert it to native processor.
        DRM_DataType = GET_UINT32_FROM_BUF((uint8_t *)pWmdrmSrmHeader);

        //Validate key region info.
        if(DRM_DataType != BDVD_DRM_DATA_WMDRM_SRM){
            printf("%s -DRM_DataType != BDVD_DRM_DATA_WMDRM_SRM\n", __FUNCTION__);
            ChkDR(DRM_E_PRIVKEY_READ_ERROR);
        }

		dr = BCM_ReadBinFileWmdrm(f_pOEMContext,
							pPrBinHeader->cust_key,
							pPrBinHeader->var_lo,
							pPrBinHeader->var_hi,
							pWmdrmSrmData->SerialNumber,
							(void *)&pWmdrmSrmData->wmdrm_data,
							sizeof(bdvd_wmdrm_pd_data_t),
							(void *)&pWmdrmSrmHeader->reserved,
							pSignature,
							prdyBinFile,
							(unsigned long)cbRead,
							pWmdrmSrmData->wmdrm_data.key,
							&encTemplateSize);
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
			BCM_Setwmdrm_keys_valid(f_pOEMContext, TRUE);
            printf("%s - WMDRM Signature Verification SUCCEEDED !!!\n", __FUNCTION__);
        }
        else {
            printf("%s - WMDRM Signature Verification Failed !!!\n", __FUNCTION__);
            ChkDR(DRM_E_CRYPTO_FAILED);
        }
		BCM_SetDevTemplate(f_pOEMContext, &prdyBinFile[group_cert_offset + BCM_GetZGroupCertSize(f_pOEMContext) + BDVD_PLAYREADY_BIN_ENC_SIGN_SIZE_IN_BYTES + sizeof(bdvd_wmdrm_srm_header_t) + sizeof(bdvd_wmdrm_srm_key_data_t)]);
    }
    else
    {
        printf("%s - Playready bin file not found\n", __FUNCTION__);
        ChkDR( DRM_E_FILENOTFOUND );
    }

ErrorExit:
    if(pSignature) Oem_MemHeapFree(pSignature);
    
    if( hfp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hfp );
    }
    printf("%s - exiting dr %x\n", __FUNCTION__, dr);
    return dr;
}

#endif
EXIT_PK_NAMESPACE_CODE;
