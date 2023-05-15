/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmkeyfile.h>
#include <drmblackbox.h>
#include <oemeccp256.h>
#include <oemkeyfile.h>
#include <drmactivation.h>
#include <drmdeviceassets.h>
#include <devicedevcert.h>
#include <tstkeyfile.h>

ENTER_PKTEST_NAMESPACE_CODE;

static const DRM_WORD g_KF_PLATFORM_VERSION = KEYFILE_PLATFORM_PK;

/*******************************************************************************
** Function:    TST_KF_Open
**
** Synopsis:    Opens the keyfile on disk
**
**              [f_pKeyFile]     Keyfile context
**              [f_dwAccessMode] The access mode to use when opening the file
**                               eg, OEM_GENERIC_READ, OEM_GENERIC_READ | OEM_GENERIC_WRITE
**
** Returns:
**              DRM_SUCCESS              The function completed successfully.
**              DRM_E_INVALIDARG         One or more arguments were invalid
**              DRM_E_KEYFILE_NOT_INITED The KeyFile context was not initialized prior to use.
**              DRM_E_FILE_READ_ERROR      An Error occured reading the file.
**
*******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL TST_KF_Open(
    __inout  DRM_KEYFILE_CONTEXT *f_pKeyFile,
    __in     DRM_DWORD            f_fCreateIfNotExists,
    __in     DRM_DWORD            f_dwAccessMode )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        ChkDR( DRM_KF_Open( f_pKeyFile, f_fCreateIfNotExists, f_dwAccessMode ) );
    }
    else
    {
        ChkArg( f_pKeyFile != NULL );
        ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
        ChkBOOL( f_pKeyFile->hKeyfile == OEM_INVALID_HANDLE_VALUE, DRM_E_FILE_READ_ERROR );

        f_pKeyFile->hKeyfile = Oem_Keyfile_Open( f_pKeyFile->pOEMContext,
            f_dwAccessMode,
            OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
            f_fCreateIfNotExists ? (DRM_DWORD)OEM_OPEN_ALWAYS : (DRM_DWORD)OEM_OPEN_EXISTING,
            OEM_ATTRIBUTE_NORMAL );

        ChkBOOL( f_pKeyFile->hKeyfile != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN );

        /*
        ** Now lock the whole file.  If the file is being opened for write
        ** then lock it for exclusive access.  This will force us to
        ** wait for all other threads that may have opened the file
        ** for read to close the file.
        **
        ** If the file is not being opened for write then lock it for
        ** shared access to allow multiple readers.
        */
        if ( ! Oem_File_Lock( f_pKeyFile->hKeyfile,
            f_dwAccessMode & OEM_GENERIC_WRITE ? TRUE : FALSE,
            0,
            0xffffffff,
            TRUE ) )
        {
            /*
            ** Could not lock the file, so close it and return an error.
            */
            Oem_Keyfile_Close( f_pKeyFile->hKeyfile );
            ChkDR( DRM_E_FILEOPEN );
        }
    }

ErrorExit:
    return dr;
}

/*******************************************************************************
** Function:    TST_KF_LoadFromFile
**
** Synopsis:    Loads, parses, and verifies the signature on the keyfile data from disk
**
**              [f_pKeyFile]     Keyfile context
**              [f_fValidateSignature] If TRUE, the keyfile signature will be validated
**
** Returns:
**              DRM_SUCCESS              The function completed successfully.
**              DRM_E_INVALIDARG         One or more arguments were invalid
**              DRM_E_KEYFILE_TOO_LARGE  The keyfile is too large for the buffer in the context
**              DRM_E_KEYFILE_NOT_INITED The KeyFile context was not initialized prior to use.
**
*******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL TST_KF_LoadFromFile(
    __inout   DRM_KEYFILE_CONTEXT        *f_pKeyFile,
    __in      DRM_BOOL                    f_fValidateSignature )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        ChkDR( DRM_KF_LoadFromFile( f_pKeyFile, f_fValidateSignature ) );
    }
    else
    {
        DRM_DWORD  cbKeyfileLen = 0;

        ChkArg( f_pKeyFile != NULL );
        ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
        ChkArg( f_pKeyFile->hKeyfile != OEM_INVALID_HANDLE_VALUE );

        ChkBOOL( Oem_File_GetSize( f_pKeyFile->hKeyfile, &cbKeyfileLen ), DRM_E_FILE_READ_ERROR );
        ChkBOOL( cbKeyfileLen <= SIZEOF( f_pKeyFile->rgbKeyfileBuffer ), DRM_E_KEYFILE_TOO_LARGE );

        f_pKeyFile->cbKeyfileBuffer = cbKeyfileLen;
        if( cbKeyfileLen == 0 )
        {
            /* We just created a new keyfile on disk, need to set the fixed values in the context for when it's
            ** stored back
            */

            ZEROMEM( &f_pKeyFile->keyfile, SIZEOF( f_pKeyFile->keyfile ) );

            f_pKeyFile->keyfile.Platform.fValid = TRUE;
            f_pKeyFile->keyfile.Platform.wPlatform = g_KF_PLATFORM_VERSION;
            f_pKeyFile->pbKeyfileBuffer = f_pKeyFile->rgbKeyfileBuffer;
        }
        else
        {
            /* We need to load the keyfile data at the end of the buffer such that next time we update
            ** keyfile data we won't be stomping on the previous keyfile data while constructing the new keyfile data.
            ** Leave some space (DRM_AES_BLOCKLEN (16 bytes)) in the end to allow padding of keyfile on some platforms.
            */
            f_pKeyFile->pbKeyfileBuffer = (f_pKeyFile->rgbKeyfileBuffer + SIZEOF( f_pKeyFile->rgbKeyfileBuffer ));
            ChkOverflow( f_pKeyFile->pbKeyfileBuffer, f_pKeyFile->rgbKeyfileBuffer );
            f_pKeyFile->pbKeyfileBuffer -= ( cbKeyfileLen + DRM_AES_BLOCKLEN );
            ChkOverflow( f_pKeyFile->pbKeyfileBuffer, f_pKeyFile->rgbKeyfileBuffer );
            /* Since DRM_E_KEYFILE_TOO_LARGE check would have failed if cbKeyfileLen was larger than the keyfile buffer, an
            ** integer underflow should not be possible in the preceding line
            */

            ChkDR( Oem_Keyfile_LoadFromFile( f_pKeyFile ) );

            ChkDR( DRM_KF_Parse( f_pKeyFile->pOEMContext,
                                 f_pKeyFile->rgbParserStack,
                                 SIZEOF( f_pKeyFile->rgbParserStack ),
                                 f_pKeyFile->pbKeyfileBuffer,
                                 f_pKeyFile->cbKeyfileBuffer,
                                 TRUE,
                                &f_pKeyFile->keyfile ) );
        }

        f_pKeyFile->fLoaded = TRUE;
    }

ErrorExit:
    return dr;
}


/*******************************************************************************
** Function:    TST_KF_GetCertificate
**
** Synopsis:    Retreives the certificate of the specified type from the keyfile
**
**              [f_pKeyFile]     Keyfile context
**              [f_eCertType]    Type of certificate to
**                               load
**              [f_ppbCertData]  Pointer to a buffer to receive a pointer to
**                               the certificate
**              [f_pcbCertData]  Pointer to a DWORD to receive the size in bytes
**                               of the certificate
**
** Returns:
**              DRM_SUCCESS                                The function complete
**                                                         d successfully.
**              DRM_E_INVALIDARG                           One or more arguments
**                                                         were invalid
**              DRM_E_KEYFILE_NOT_INITED                   The KeyFile context was
**                                                         not initialized prior to use.
**              DRM_E_KEYFILE_CERTIFICATE_CHAIN_NOT_FOUND  The certificate of
**                                                         the specified type
**                                                         was not found
**
********************************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: TST_KEYFILE_CONTEXT* should not be const.")
DRM_API DRM_RESULT DRM_CALL TST_KF_GetCertificate(
    __in                            DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    __in                            DRM_KF_CERT_TYPE       f_eCertType,
    __out_ecount(*f_pcbCertData)    DRM_BYTE             **f_ppbCertData,
    __out_ecount(1)                 DRM_DWORD             *f_pcbCertData )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        ChkDR( DRM_KF_GetCertificate( f_pKeyFile, f_eCertType, f_ppbCertData, f_pcbCertData ) );
    }
    else
    {
        KF_CERTIFICATE_CHAIN_OBJECT_V2 *pCertChain            = NULL;
        DRM_KEYFILE_CERT_TYPE           eFormatCertTypeToFind = KF_CERT_TYPE_INVALID;

        ChkArg( f_pKeyFile    != NULL );
        ChkArg( f_ppbCertData != NULL );
        ChkArg( f_pcbCertData != 0 );

        ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
        ChkArg( f_pKeyFile->fLoaded );

        /* Perform an explicit mapping.  This ensures that the API can change without regaurd to the
        ** core format defines
        */
        eFormatCertTypeToFind = MapApiCertTypeToFormatCertType( f_eCertType );
        ChkArg( eFormatCertTypeToFind != KF_CERT_TYPE_INVALID );

        pCertChain = f_pKeyFile->keyfile.pCertChainV2;
        while( pCertChain != NULL && pCertChain->fValid )
        {
            if( (DRM_KEYFILE_CERT_TYPE)pCertChain->dwCertificateType == eFormatCertTypeToFind )
            {
                /* Found the cert */
                *f_ppbCertData = pCertChain->xbbaCertificateChain.pbDataBuffer + pCertChain->xbbaCertificateChain.iData;
                *f_pcbCertData = pCertChain->xbbaCertificateChain.cbData;

                goto ErrorExit;
            }
            pCertChain = pCertChain->pNext;
        }

        ChkDR( DRM_E_KEYFILE_CERTIFICATE_CHAIN_NOT_FOUND );
    }
ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT _FindAsymmetricKeyPair(
    __in                      const DRM_KEYFILE_CONTEXT             *f_pKeyFile,
    __in                            DRM_KEYFILE_ASYMMETRIC_KEY_TYPE  f_eAsymmKeyType,
    __in_bcount(f_cbPubKey)   const DRM_BYTE                        *f_pbPubKey,
    __in                            DRM_DWORD                        f_cbPubKey,
    __out                           DRM_ENCRYPTED_KEY               *f_pKey ) DRM_NO_INLINE_ATTRIBUTE;


static DRM_NO_INLINE DRM_RESULT _FindAsymmetricKeyPair(
    __in                      const DRM_KEYFILE_CONTEXT             *f_pKeyFile,
    __in                            DRM_KEYFILE_ASYMMETRIC_KEY_TYPE  f_eAsymmKeyType,
    __in_bcount(f_cbPubKey)   const DRM_BYTE                        *f_pbPubKey,
    __in                            DRM_DWORD                        f_cbPubKey,
    __out                           DRM_ENCRYPTED_KEY               *f_pKey )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    KF_KEYPAIR_V2_OBJECT *pKeyPairV2 = NULL;
    KF_KEYPAIR_V3_OBJECT *pKeyPairV3 = NULL;

    ChkArg( f_pbPubKey != NULL );

    pKeyPairV3 = f_pKeyFile->keyfile.KeyHistoryV2.pKeyPairHeadv3;
    while( pKeyPairV3 != NULL && pKeyPairV3->fValid )
    {
        if( f_cbPubKey == pKeyPairV3->xbbaPublicKeyData.cbData
           && pKeyPairV3->wCipherType == f_eAsymmKeyType
           && OEM_SECURE_ARE_EQUAL(pKeyPairV3->xbbaPublicKeyData.pbDataBuffer + pKeyPairV3->xbbaPublicKeyData.iData, f_pbPubKey, f_cbPubKey ) )
        {
            /* Found the key */
            f_pKey->guidKeypairDecryptionKeyID = pKeyPairV3->guidKeypairDecryptionKeyID;
            f_pKey->wKeypairDecryptionMethod = pKeyPairV3->wKeypairDecryptionMethod;
            f_pKey->wKeySize = pKeyPairV3->wKeySize;
            f_pKey->wReserved = pKeyPairV3->wReserved;

            f_pKey->cbEncryptedKey = pKeyPairV3->xbbaEncryptedPrivateKey.cbData;
            f_pKey->pbEncryptedKey = pKeyPairV3->xbbaEncryptedPrivateKey.pbDataBuffer + pKeyPairV3->xbbaEncryptedPrivateKey.iData;

            /* Initialization vector size is constant */
            ChkFAIL( SIZEOF( pKeyPairV3->qwInitializationVector ) <= SIZEOF( f_pKey->rgbInitializationVector ) );
            DRM_BYT_CopyBytes( f_pKey->rgbInitializationVector, 0, &pKeyPairV3->qwInitializationVector, 0, SIZEOF( pKeyPairV3->qwInitializationVector ) );

            goto ErrorExit;
        }
        pKeyPairV3 = pKeyPairV3->pNext;
    }

    pKeyPairV2 = f_pKeyFile->keyfile.KeyHistoryV2.pKeyPairHeadv2;

    while( pKeyPairV2 != NULL && pKeyPairV2->fValid )
    {
        if( f_cbPubKey == pKeyPairV2->xbbaPublicKeyData.cbData
            && pKeyPairV2->wCipherType == f_eAsymmKeyType
            && OEM_SECURE_ARE_EQUAL( pKeyPairV2->xbbaPublicKeyData.pbDataBuffer + pKeyPairV2->xbbaPublicKeyData.iData, f_pbPubKey, f_cbPubKey ) )
        {
            /* Found the key */
            f_pKey->guidKeypairDecryptionKeyID = pKeyPairV2->guidKeypairDecryptionKeyID;
            f_pKey->wKeypairDecryptionMethod = pKeyPairV2->wKeypairDecryptionMethod;
            f_pKey->wKeySize = pKeyPairV2->wKeySize;
            f_pKey->wReserved = pKeyPairV2->wReserved;

            f_pKey->cbEncryptedKey = pKeyPairV2->xbbaEncryptedPrivateKey.cbData;
            f_pKey->pbEncryptedKey = pKeyPairV2->xbbaEncryptedPrivateKey.pbDataBuffer + pKeyPairV2->xbbaEncryptedPrivateKey.iData;

            /* Initialization vector size is constant */
            ChkFAIL( pKeyPairV2->xbbaInitializationVector.cbData == SIZEOF( f_pKey->rgbInitializationVector ) );
            DRM_BYT_CopyBytes( f_pKey->rgbInitializationVector, 0, pKeyPairV2->xbbaInitializationVector.pbDataBuffer, pKeyPairV2->xbbaInitializationVector.iData, SIZEOF( f_pKey->rgbInitializationVector ) );

            goto ErrorExit;
        }
        pKeyPairV2 = pKeyPairV2->pNext;
    }

    ChkDR( DRM_E_KEYFILE_PRIVATE_KEY_NOT_FOUND );
ErrorExit:
    return dr;
}


/*******************************************************************************
** Function:    DRM_KF_GetPrivateKey
**
** Synopsis:    Retreives an encrypted private key from the keyfile given the
**              corresponding public
**              key
**
**              [f_pKeyFile]     Keyfile context
**              [f_eKeyType]  Cipher type of the private key to retreive
**              [f_pbPubKey]     Public key corresponding to the private key
**                               desired
**              [f_cbPubKey]     Size in bytes of the public
**                               key
**              [f_pKey]         Pointer to a struct to receive the encrypted
**                               private key
**                               data
**
** Returns:
**              DRM_SUCCESS                          The function completed
**                                                   successfully.
**              DRM_E_INVALIDARG                     One or more arguments were
**                                                   invalid
**              DRM_E_KEYFILE_NOT_INITED             The KeyFile context was
**                                                   not initialized prior to use.
**              DRM_E_KEYFILE_PRIVATE_KEY_NOT_FOUND  A private key with the
**                                                   specified constraints
**                                                   could not be found
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_NO_INLINE DRM_CALL TST_KF_GetPrivateKey(
    __in                        const DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    __in                              DRM_KF_KEY_TYPE        f_eKeyType,
    __in_bcount_opt(f_cbPubKey) const DRM_BYTE              *f_pbPubKey,
    __in                              DRM_DWORD              f_cbPubKey,
    __out_ecount(1)                   DRM_ENCRYPTED_KEY     *f_pKey )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pKeyFile != NULL );
    ChkArg( f_pKey != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->fLoaded );

    if( eKF_KEY_SECURE_STORE == f_eKeyType )
    {
        if( f_pKeyFile->keyfile.SecureStoreKeyV3.fValid )
        {
            f_pKey->guidKeypairDecryptionKeyID = f_pKeyFile->keyfile.SecureStoreKeyV3.guidKeypairDecryptionKeyID;
            f_pKey->wKeypairDecryptionMethod = f_pKeyFile->keyfile.SecureStoreKeyV3.wKeypairDecryptionMethod;
            f_pKey->wKeySize = f_pKeyFile->keyfile.SecureStoreKeyV3.wKeySize;
            f_pKey->wReserved = f_pKeyFile->keyfile.SecureStoreKeyV3.wReserved;

            f_pKey->cbEncryptedKey = f_pKeyFile->keyfile.SecureStoreKeyV3.xbbaEncryptedSecureStoreKey.cbData;
            f_pKey->pbEncryptedKey = f_pKeyFile->keyfile.SecureStoreKeyV3.xbbaEncryptedSecureStoreKey.pbDataBuffer + f_pKeyFile->keyfile.SecureStoreKeyV3.xbbaEncryptedSecureStoreKey.iData;

            /* Initialization vector size is constant */
            ChkBOOL( SIZEOF( f_pKeyFile->keyfile.SecureStoreKeyV3.qwInitializationVector ) <= SIZEOF( f_pKey->rgbInitializationVector ), DRM_E_KEYFILE_PRIVATE_KEY_NOT_FOUND );
            OEM_SECURE_MEMCPY( f_pKey->rgbInitializationVector, &f_pKeyFile->keyfile.SecureStoreKeyV3.qwInitializationVector, SIZEOF( f_pKeyFile->keyfile.SecureStoreKeyV3.qwInitializationVector ) );
        }
        else
        {
            ChkBOOL( f_pKeyFile->keyfile.SecureStoreKeyV2.fValid, DRM_E_KEYFILE_PRIVATE_KEY_NOT_FOUND );

            f_pKey->guidKeypairDecryptionKeyID = f_pKeyFile->keyfile.SecureStoreKeyV2.guidKeypairDecryptionKeyID;
            f_pKey->wKeypairDecryptionMethod = f_pKeyFile->keyfile.SecureStoreKeyV2.wKeypairDecryptionMethod;
            f_pKey->wKeySize = f_pKeyFile->keyfile.SecureStoreKeyV2.wKeySize;
            f_pKey->wReserved = f_pKeyFile->keyfile.SecureStoreKeyV2.wReserved;

            f_pKey->cbEncryptedKey = f_pKeyFile->keyfile.SecureStoreKeyV2.xbbaEncryptedSecureStoreKey.cbData;
            f_pKey->pbEncryptedKey = f_pKeyFile->keyfile.SecureStoreKeyV2.xbbaEncryptedSecureStoreKey.pbDataBuffer + f_pKeyFile->keyfile.SecureStoreKeyV2.xbbaEncryptedSecureStoreKey.iData;

            /* Initialization vector size is constant */
            ChkBOOL( f_pKeyFile->keyfile.SecureStoreKeyV2.xbbaInitializationVector.cbData == SIZEOF( f_pKey->rgbInitializationVector ), DRM_E_KEYFILE_PRIVATE_KEY_NOT_FOUND );
            OEM_SECURE_MEMCPY( f_pKey->rgbInitializationVector, f_pKeyFile->keyfile.SecureStoreKeyV2.xbbaInitializationVector.pbDataBuffer + f_pKeyFile->keyfile.SecureStoreKeyV2.xbbaInitializationVector.iData, SIZEOF( f_pKey->rgbInitializationVector ) );
        }
    }
    else
    {
        DRM_KEYFILE_ASYMMETRIC_KEY_TYPE eAsymmKeyType = MapApiKeyTypeToFormatKeyType( f_eKeyType );

        ChkArg( eAsymmKeyType != KF_ASYMM_KEY_TYPE_INVALID );
        ChkArg( f_pbPubKey != NULL );

        ChkDR( _FindAsymmetricKeyPair( f_pKeyFile, eAsymmKeyType, f_pbPubKey, f_cbPubKey, f_pKey ) );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    TST_DDC_GetBinaryDeviceCertificate
**
** Synopsis:    Gets the Device's binary certificate chain, creating the leaf node if necessary.
**
** Arguments:   [f_pOEMContext]           : A pointer to the OEM Context
**              [f_dwFlags]               : Behavioral flags
**              [f_pBinaryDevCertChain]   : A pointer to a bcert chain buffer to fill in
**              [f_pcbBinaryDevCertChain] : The size of the bcert chain buffer; on input
**                                          should be enough to fill both device and model certs in it.
**                                          On return will indicate the size on device cert chain.
**              [f_dwFlags]               : DRM_DCP_VERIFY_DEVCERT - verify signature
**              [f_pCryptoCtx]            : Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
**
** Returns:     DRM_SUCCESS                            - on success
**              DRM_E_INVALIDARG                       - if any required pointer parameter is NULL
**              or other return code from a function called within
**              DRM_E_KEYFILE_CERTIFICATE_ISSUER_KEY_MISMATCH -
**              certificate issuer key does not match public key of
**              a model level certificate in PlayReady.dll**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL TST_DDC_GetBinaryDeviceCertificate(
    __in_opt                               DRM_VOID            *f_pOEMContext,
    __in                                   DRM_KEYFILE_CONTEXT *f_poKeyFileContext,
    __out_bcount(*f_pcbBinaryDevCertChain) DRM_BYTE            *f_pBinaryDevCertChain,
    __inout                                DRM_DWORD           *f_pcbBinaryDevCertChain,
    __in                                   const DRM_DWORD      f_dwFlags,
    __inout                                DRM_CRYPTO_CONTEXT  *f_pCryptoCtx
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( DRM_DEVICEASSETS_IsDeviceAssetsSupported() )
    {
        ChkDR( DRM_DDC_GetBinaryDeviceCertificate( f_pOEMContext,
                                                   f_poKeyFileContext,
                                                   f_pBinaryDevCertChain,
                                                   f_pcbBinaryDevCertChain,
                                                   f_dwFlags,
                                                   f_pCryptoCtx ) );
    }
    else
    {
        DRM_BYTE  *pbBinaryDeviceCert = NULL;
        DRM_DWORD  cbBinaryDeviceCert = 0;

        ChkArg( f_pcbBinaryDevCertChain != NULL );
        ChkArg( f_pCryptoCtx            != NULL );

        /*
        ** First, try to get an existing device certificate
        */
        ChkDR( TST_KF_GetCertificate ( f_poKeyFileContext,
                                       eKF_CERT_TYPE_PLAYREADY,
                                      &pbBinaryDeviceCert,
                                      &cbBinaryDeviceCert ) );

        /*
        ** Can return dev cert size right away if that's what we wanted.
        */
        if ( f_pBinaryDevCertChain == NULL )
        {
            *f_pcbBinaryDevCertChain = cbBinaryDeviceCert;
            ChkDR( DRM_E_BUFFERTOOSMALL ); /* the caller code still expects this error */
        }
        else
        {
            if( *f_pcbBinaryDevCertChain < cbBinaryDeviceCert )
            {
                *f_pcbBinaryDevCertChain = cbBinaryDeviceCert;
                ChkDR( DRM_E_BUFFERTOOSMALL );
            }
        }

        MEMCPY( f_pBinaryDevCertChain, pbBinaryDeviceCert, cbBinaryDeviceCert );
        *f_pcbBinaryDevCertChain = cbBinaryDeviceCert;
    }

 ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE;

