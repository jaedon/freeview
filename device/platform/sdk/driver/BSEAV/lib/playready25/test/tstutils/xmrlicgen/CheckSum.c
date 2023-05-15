/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include "stdafx.h"

#include <PKTestNameSpace.h>

USING_PK_NAMESPACE
USING_PKTEST_NAMESPACE

#include <oemaes.h>
#include <oemrsa.h>

ENTER_PKTEST_NAMESPACE_CODE
/**
    name@ CalculateCheckSum
    descr@ Calculcates the check sum of a content key
    param@ pbData - Pointer to the content key
    param@ cbData - length of the content key
    param@ pCheckSum - pointer to buffer to put the check sum
    param@ cbCheckSum - The length of check sum buffer.
    param@ pKID - Optional.  If specified, it's the KID for the content key and indicates we should use the new AES-based checksum algorithm.  Otherwise, we use the legacy SHA-based checksum algorithm.
    return@ a valid DRM_RESULT code.
*/
DRM_RESULT DRM_CALL CalculateCheckSum(
    __in  const DRM_BYTE  *pbData,
    __in        DRM_DWORD  cbData,
    __out       DRM_BYTE  *pbCheckSum,
    __in        DRM_DWORD  cbCheckSum,
    __in  const DRM_KID   *pKID )
{
    DRM_RESULT dr=DRM_SUCCESS;
    DRM_SHA1_CONTEXT  shaVal = {0};
    DRM_BYTE res[DRM_SHA1_DIGEST_LEN+1] = {0};
    DRM_AES_KEY aesTotallyInsecureContentKey = {0};
    DRM_BYTE bKeyBuffer[SIZEOF(DRM_AES_KEY)] = {0};
    DRM_BYTE bAESBlock[DRM_AES_BLOCKLEN] = {0};
    int i = 0;

    ChkArg(pbData);
    ChkArg(pbCheckSum);
    if( pKID == NULL )
    {
        ChkArg(cbData <= DRM_SHA1_DIGEST_LEN);
        ChkArg(cbCheckSum <= DRM_SHA1_DIGEST_LEN+1);

        memset(res, 0, DRM_SHA1_DIGEST_LEN+1);
        memcpy(res, pbData, cbData);
        for (i = 0; i < SHA_ITERATIONS; i++)
        {
            DRM_SHA1_Init(&shaVal);
            DRM_SHA1_Update(res, DRM_SHA1_DIGEST_LEN+1, &shaVal);
            DRM_SHA1_Finalize(&shaVal, res);
        }
        memcpy(pbCheckSum, res, cbCheckSum);
    }
    else
    {

        ChkArg( cbCheckSum == DRM_AES_CK_CHECKSUM_LENGTH );
        ChkArg( cbData <= SIZEOF(bKeyBuffer) );

        /* New algorithm: Encrypt KID with Content Key == checksum */
        memcpy( bAESBlock, pKID, SIZEOF( DRM_KID ) );
        memcpy( bKeyBuffer, pbData, cbData );

        ChkDR( Oem_Aes_SetKey( bKeyBuffer, &aesTotallyInsecureContentKey ) );

        ChkDR( Oem_Aes_EcbEncryptData(
            &aesTotallyInsecureContentKey,
            bAESBlock,
            DRM_AES_BLOCKLEN ) );

        memcpy(pbCheckSum, bAESBlock, cbCheckSum);
    }

ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE