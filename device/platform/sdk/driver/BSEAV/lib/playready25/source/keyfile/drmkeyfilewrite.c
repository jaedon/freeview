/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMKEYFILEWRITE_C
#include <drmkeyfile.h>
#include <drmblackbox.h>
#include <oemeccp256.h>
#include <oemkeyfile.h>
#include <drmactivation.h>
#include <drmdeviceassets.h>
#include <drmmathsafe.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;


#if DRM_KEYFILE_VERSION == 2
    static const DRM_DWORD g_KF_VERSION = KEYFILE_VERSION_2;
#else
    static const DRM_DWORD g_KF_VERSION = KEYFILE_VERSION_3;
#endif

/*
** Returns an error specific to keyfile are ainstead of a generic out-of-memory error
** thrown by a stack allocator.
*/
#define TranslateKeyfileOOMError(_dr) \
    ( ( _dr == DRM_E_OUTOFMEMORY ) ? DRM_E_KEYFILE_TOO_LARGE : _dr )

/*
** Note: When this function is successfully executed, the key file buffer is in the machine byte order.
*/
static DRM_RESULT DRMFORCEINLINE DRM_CALL _LoadKeyfile(
    __inout DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    __in    DRM_BOOL               f_fCheckSig )
{
    return DRM_KF_Parse(
        f_pKeyFile->pOEMContext,
        f_pKeyFile->rgbParserStack,
        SIZEOF( f_pKeyFile->rgbParserStack ),
        f_pKeyFile->pbKeyfileBuffer,
        f_pKeyFile->cbKeyfileBuffer,
        f_fCheckSig,
        &f_pKeyFile->keyfile );
}

/* This function re-builds the keyfile data and optionally adds or updates the supplied cert or key into the
** format during building. If the cert type and/or key type is specified as INVALID, then the parameter is ignored.
** Note: When this function is successfully executed, the key file buffer is in the network byte order.
*/
static DRM_RESULT DRM_CALL _UpdateKeyfile(
    __inout                               DRM_KEYFILE_CONTEXT             *f_pKeyFile,
    __in                                  DRM_KEYFILE_CERT_TYPE            f_eCertType,
    __in_bcount_opt(f_cbCertData)   const DRM_BYTE                        *f_pbCertData,
    __in                                  DRM_DWORD                        f_cbCertData,
    __in                                  DRM_BOOL                         f_fUpdateSecureStoreKey,
    __in                                  DRM_KEYFILE_ASYMMETRIC_KEY_TYPE  f_eAsymmKeyType,
    __in_bcount_opt(f_cbPubKey)     const DRM_BYTE                        *f_pbPubKey,
    __in                                  DRM_DWORD                        f_cbPubKey,
    __in_opt                              DRM_ENCRYPTED_KEY               *f_pKey,
    __in_opt                              DRM_DEVICE_CERT_PARSER_CACHE    *f_pCertParserCache )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    KF_KEYPAIR_V2_OBJECT *pKeyPair = NULL;
    KF_CERTIFICATE_CHAIN_OBJECT_V2 *pCertChain = NULL;
    DRM_DWORD  cbKeyfile = SIZEOF( f_pKeyFile->rgbKeyfileBuffer );
    KF_KEYPAIR_V2_OBJECT keyPair = {0};
    KF_CERTIFICATE_CHAIN_OBJECT_V2 certChain = {0};
    KF_SECURE_STORE_KEY_V2_OBJECT secureStoreKey = {0};
    KF_CERT_PARSER_CACHE_OBJECT certParserCache = {0};

    ChkDR( DRM_KF_Start( f_pKeyFile->rgbBuilderStack, SIZEOF( f_pKeyFile->rgbBuilderStack ), g_KF_VERSION, NULL, &f_pKeyFile->builderContext ) );

    /* Add platform object */
    ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_PLATFORM_OBJECT, &f_pKeyFile->keyfile.Platform ) );

    /* Add all the keypairs in the keyhistory */
    pKeyPair = f_pKeyFile->keyfile.KeyHistoryV2.pKeyPairHeadv2;
    while( pKeyPair != NULL && pKeyPair->fValid )
    {
        if( f_eAsymmKeyType != KF_ASYMM_KEY_TYPE_INVALID
         && f_pbPubKey != NULL
         && f_cbPubKey == pKeyPair->xbbaPublicKeyData.cbData
         && pKeyPair->wCipherType == f_eAsymmKeyType
         && DRM_BYT_CompareBytes( pKeyPair->xbbaPublicKeyData.pbDataBuffer, pKeyPair->xbbaPublicKeyData.iData, f_pbPubKey, 0, f_cbPubKey ) == 0 )
        {
            /* The key to be added already exists, we just need to update it */

            ChkArg( f_pKey != NULL );

            pKeyPair->wKeySize = f_pKey->wKeySize;
            pKeyPair->wKeypairDecryptionMethod = f_pKey->wKeypairDecryptionMethod;
            pKeyPair->wReserved = f_pKey->wReserved;
            pKeyPair->guidKeypairDecryptionKeyID = f_pKey->guidKeypairDecryptionKeyID;

            /* Currently no support for expanding the size of keys */
            ChkFAIL( f_pKey->cbEncryptedKey == pKeyPair->xbbaEncryptedPrivateKey.cbData );
            DRM_BYT_CopyBytes( pKeyPair->xbbaEncryptedPrivateKey.pbDataBuffer, pKeyPair->xbbaEncryptedPrivateKey.iData, f_pKey->pbEncryptedKey, 0, f_pKey->cbEncryptedKey );

            /* Initialization vector size is constant */
            ChkFAIL( pKeyPair->xbbaInitializationVector.cbData == SIZEOF( f_pKey->rgbInitializationVector ) );
            DRM_BYT_CopyBytes( pKeyPair->xbbaInitializationVector.pbDataBuffer, pKeyPair->xbbaInitializationVector.iData, f_pKey->rgbInitializationVector, 0, SIZEOF( f_pKey->rgbInitializationVector ) );


            /* Set key to be added to 0 so we won't add it again later */
            f_eAsymmKeyType = KF_ASYMM_KEY_TYPE_INVALID;
        }

        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_KEYPAIR_OBJECT_2, pKeyPair ) );

        pKeyPair = pKeyPair->pNext;
    }


    if( f_eAsymmKeyType != KF_ASYMM_KEY_TYPE_INVALID )
    {
        /* New keypair is to be added to the key history */

        ChkArg( f_pbPubKey != NULL );
        ChkArg( f_pKey != NULL );

        keyPair.fValid = TRUE;
        ChkDR( DRM_DWordToWord( f_eAsymmKeyType, &keyPair.wCipherType ) );
        keyPair.wKeySize = f_pKey->wKeySize;
        keyPair.wKeypairDecryptionMethod = f_pKey->wKeypairDecryptionMethod;
        keyPair.wReserved = f_pKey->wReserved;
        keyPair.guidKeypairDecryptionKeyID = f_pKey->guidKeypairDecryptionKeyID;

        keyPair.xbbaPublicKeyData.fValid = TRUE;
        keyPair.xbbaPublicKeyData.cbData = f_cbPubKey;
        keyPair.xbbaPublicKeyData.iData = 0;
        keyPair.xbbaPublicKeyData.pbDataBuffer = (DRM_BYTE*)f_pbPubKey;

        keyPair.xbbaEncryptedPrivateKey.fValid = TRUE;
        keyPair.xbbaEncryptedPrivateKey.cbData = f_pKey->cbEncryptedKey;
        keyPair.xbbaEncryptedPrivateKey.iData = 0;
        keyPair.xbbaEncryptedPrivateKey.pbDataBuffer = f_pKey->pbEncryptedKey;

        keyPair.xbbaInitializationVector.fValid = TRUE;
        keyPair.xbbaInitializationVector.cbData = SIZEOF( f_pKey->rgbInitializationVector );
        keyPair.xbbaInitializationVector.iData = 0;
        keyPair.xbbaInitializationVector.pbDataBuffer = (DRM_BYTE*)f_pKey->rgbInitializationVector;


        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_KEYPAIR_OBJECT_2, &keyPair ) );
    }

    pCertChain = f_pKeyFile->keyfile.pCertChainV2;
    while( pCertChain != NULL && pCertChain->fValid )
    {
        if( f_eCertType != KF_CERT_TYPE_INVALID
            && (DRM_KEYFILE_CERT_TYPE)pCertChain->dwCertificateType == f_eCertType )
        {
            /* Update the cert */

            ChkArg( f_pbCertData != NULL );
            ChkArg( f_cbCertData > 0 );

            pCertChain->xbbaCertificateChain.pbDataBuffer = (DRM_BYTE*)f_pbCertData;
            pCertChain->xbbaCertificateChain.iData = 0;
            pCertChain->xbbaCertificateChain.cbData = f_cbCertData;

            /* Clear out cert type so we won't add it again */
            f_eCertType = KF_CERT_TYPE_INVALID;
        }

        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_CERTIFICATE_CHAIN_OBJECT_2, pCertChain ) );
        pCertChain = pCertChain->pNext;
    }

    if( f_eCertType != KF_CERT_TYPE_INVALID )
    {
        ChkArg( f_pbCertData != NULL );
        ChkArg( f_cbCertData > 0 );

        certChain.fValid = TRUE;
        certChain.dwCertificateType = f_eCertType;
        certChain.xbbaCertificateChain.pbDataBuffer = (DRM_BYTE*)f_pbCertData;
        certChain.xbbaCertificateChain.iData = 0;
        certChain.xbbaCertificateChain.cbData = f_cbCertData;
        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_CERTIFICATE_CHAIN_OBJECT_2, &certChain ) );
    }

    if( f_fUpdateSecureStoreKey )
    {
        ChkArg( f_pKey != NULL );
        ChkArg( f_pbPubKey == NULL );
        ChkArg( f_cbPubKey == 0 );

        secureStoreKey.fValid = TRUE;
        secureStoreKey.wCipherType = KF_SYMM_KEY_TYPE_AES;
        secureStoreKey.wKeySize = f_pKey->wKeySize;
        secureStoreKey.wKeypairDecryptionMethod = f_pKey->wKeypairDecryptionMethod;
        secureStoreKey.wReserved = f_pKey->wReserved;
        secureStoreKey.guidKeypairDecryptionKeyID = f_pKey->guidKeypairDecryptionKeyID;

        secureStoreKey.xbbaEncryptedSecureStoreKey.fValid = TRUE;
        secureStoreKey.xbbaEncryptedSecureStoreKey.cbData = f_pKey->cbEncryptedKey;
        secureStoreKey.xbbaEncryptedSecureStoreKey.iData = 0;
        secureStoreKey.xbbaEncryptedSecureStoreKey.pbDataBuffer = f_pKey->pbEncryptedKey;

        secureStoreKey.xbbaInitializationVector.fValid = TRUE;
        secureStoreKey.xbbaInitializationVector.cbData = SIZEOF( f_pKey->rgbInitializationVector );
        secureStoreKey.xbbaInitializationVector.iData = 0;
        secureStoreKey.xbbaInitializationVector.pbDataBuffer = (DRM_BYTE*)f_pKey->rgbInitializationVector;


        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_SECURE_STORE_KEY_OBJECT_2, &secureStoreKey ) );
    }
    else if( f_pKeyFile->keyfile.SecureStoreKeyV2.fValid )
    {
        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_SECURE_STORE_KEY_OBJECT_2, &f_pKeyFile->keyfile.SecureStoreKeyV2 ) );
    }

    if ( f_pCertParserCache != NULL )
    {
        certParserCache.fValid = TRUE;
        certParserCache.wVersion = DRM_CERT_PARSER_CACHE_VERSION_1;

        certParserCache.xbbaHash.fValid = TRUE;
        certParserCache.xbbaHash.cbData = DRM_SHA1_DIGEST_LEN;
        certParserCache.xbbaHash.iData = 0;
        certParserCache.xbbaHash.pbDataBuffer = f_pCertParserCache->m_rgbSHAHash;

        certParserCache.wlData.fValid = TRUE;
        certParserCache.wlData.cWORDs = SIZEOF( f_pCertParserCache->m_rgbPacked ) / SIZEOF( DRM_WORD );
        certParserCache.wlData.pwordBuffer = ( DRM_BYTE * )( f_pCertParserCache->m_rgbPacked );
        certParserCache.wlData.iWords = 0;

        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_CERT_PARSER_CACHE_OBJECT, &certParserCache ) );
    }
    else if( f_pKeyFile->keyfile.CertParserCache.fValid )
    {
        ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_CERT_PARSER_CACHE_OBJECT, &f_pKeyFile->keyfile.CertParserCache ) );
    }

    f_pKeyFile->keyfile.Signature.fValid = TRUE;
    f_pKeyFile->keyfile.Signature.wSignatureType = KEYFILE_SIGNATURE_TYPE_OMAC_1;
    f_pKeyFile->keyfile.Signature.xbbaSignatureData.fValid = TRUE;
    f_pKeyFile->keyfile.Signature.xbbaSignatureData.cbData = DRM_AES_BLOCKLEN;
    f_pKeyFile->keyfile.Signature.xbbaSignatureData.iData = 0;
    f_pKeyFile->keyfile.Signature.xbbaSignatureData.pbDataBuffer = f_pKeyFile->rgbKeyfileBuffer; /* Dummy data, will be overwritten during StoreToFile */
    f_pKeyFile->keyfile.Signature.xbbaSigningCertificate.fValid = TRUE;
    f_pKeyFile->keyfile.Signature.xbbaSigningCertificate.cbData = SIZEOF( DRM_GUID );
    f_pKeyFile->keyfile.Signature.xbbaSigningCertificate.iData = 0;
    f_pKeyFile->keyfile.Signature.xbbaSigningCertificate.pbDataBuffer = f_pKeyFile->rgbKeyfileBuffer; /* Dummy data, will be overwritten during StoreToFile */

    ChkDR( DRM_XB_AddEntry( &f_pKeyFile->builderContext, KF_ENTRY_TYPE_SIGNATURE_OBJECT, &f_pKeyFile->keyfile.Signature ) );

    ChkDR( DRM_XB_FinishFormat( &f_pKeyFile->builderContext, f_pKeyFile->rgbKeyfileBuffer, &cbKeyfile ) );
    f_pKeyFile->cbKeyfileBuffer = cbKeyfile;

    /* We need to move the keyfile data to the end of the buffer such that next time we update
    ** keyfile data we won't be stomping on the previous keyfile data while constructing the new keyfile data
    */
    f_pKeyFile->pbKeyfileBuffer = (f_pKeyFile->rgbKeyfileBuffer + SIZEOF( f_pKeyFile->rgbKeyfileBuffer ));
    ChkOverflow( f_pKeyFile->pbKeyfileBuffer, f_pKeyFile->rgbKeyfileBuffer );
    f_pKeyFile->pbKeyfileBuffer -= cbKeyfile;

    /* Since DRM_XB_FinishFormat would have failed if cbKeyfile was larger than the keyfile buffer, an
    ** integer underflow should not be possible in the preceding line
    */
    DRM_BYT_MoveBytes( f_pKeyFile->pbKeyfileBuffer, 0, f_pKeyFile->rgbKeyfileBuffer, 0, cbKeyfile );

ErrorExit:
    return TranslateKeyfileOOMError(dr);
}

/*******************************************************************************
** Function:    DRM_KF_StoreToFile
**
** Synopsis:    Stores and signs the cached keyfile data to disk
**
**              [f_pKeyFile]     Keyfile context
**
** Returns:
**              DRM_SUCCESS              The function completed successfully.
**              DRM_E_INVALIDARG         One or more arguments were invalid
**              DRM_E_KEYFILE_NOT_INITED The KeyFile context was not initialized prior to use.
**
*******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_KF_StoreToFile(
    __inout   DRM_KEYFILE_CONTEXT           *f_pKeyFile )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  iObject  = 0;
    DRM_DWORD  cbObject = 0;
    DRM_GUID   guidAESDecryptionKeyID;
    DRM_DWORD iSignatureFromObjectStart = 0;
    DRM_DWORD iCertificateFromObjectStart = 0;

    ChkBOOL( DRM_DEVICEASSETS_IsDeviceAssetsSupported(), DRM_E_NOTIMPL );

    ChkArg( f_pKeyFile != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->cbKeyfileBuffer > 0 );
    ChkArg( f_pKeyFile->hKeyfile != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_pKeyFile->fLoaded );

    if( DRM_ACTIVATION_IsActivationSupported( ) )
    {
        /* If this device is using activation the flow would be a call to DRM_KF_LoadFromMemory
        ** followed by a call to StoreToFile.  In this case the keyfile is already signed
        ** and ready to write to disk.
        */

    }
    else
    {
        /* In the non activation case we need to rebuild the keyfile including signature
        ** before we can write to disk
        */

        /* Convert the data in the internal key file buffer to network byte order. */
        ChkDR( _UpdateKeyfile( f_pKeyFile, KF_CERT_TYPE_INVALID, NULL, 0, FALSE, KF_ASYMM_KEY_TYPE_INVALID, NULL, 0, NULL, NULL ) );

        /* Offset of the signature and certificate data into the signature object */
        iSignatureFromObjectStart = XB_BASE_OBJECT_LENGTH
            + SIZEOF(DRM_DWORD)
            + f_pKeyFile->keyfile.Signature.xbbaSigningCertificate.cbData
            + SIZEOF( DRM_WORD )
            + SIZEOF( DRM_WORD );

        iCertificateFromObjectStart = XB_BASE_OBJECT_LENGTH + SIZEOF(DRM_DWORD);

        /* Caculcate the signature */
        ChkDR( DRM_XB_FindObject(
                KF_ENTRY_TYPE_SIGNATURE_OBJECT,
                &(g_kfFormatDescription[ADJUST_VERSION_TO_INDEX(g_KF_VERSION)]),
                f_pKeyFile->pbKeyfileBuffer,
                f_pKeyFile->cbKeyfileBuffer,
                &iObject,
                &cbObject ) );

        ChkArg( iObject + max( iSignatureFromObjectStart, iCertificateFromObjectStart ) < f_pKeyFile->cbKeyfileBuffer );
        ChkOverflow( f_pKeyFile->pbKeyfileBuffer + iSignatureFromObjectStart, f_pKeyFile->pbKeyfileBuffer );
        ChkOverflow( f_pKeyFile->pbKeyfileBuffer + iSignatureFromObjectStart + iObject, f_pKeyFile->pbKeyfileBuffer + iSignatureFromObjectStart );

        ChkDR( DRM_BBX_KF_CalculateOmac( f_pKeyFile->pOEMContext,
            &guidAESDecryptionKeyID,
            TRUE,
            f_pKeyFile->pbKeyfileBuffer,
            iObject,
            f_pKeyFile->pbKeyfileBuffer + iObject + iSignatureFromObjectStart ) );
        ChkOverflow( f_pKeyFile->pbKeyfileBuffer + iCertificateFromObjectStart, f_pKeyFile->pbKeyfileBuffer );
        ChkOverflow( f_pKeyFile->pbKeyfileBuffer + iCertificateFromObjectStart + iObject, f_pKeyFile->pbKeyfileBuffer + iCertificateFromObjectStart );
        MEMCPY( f_pKeyFile->pbKeyfileBuffer + iObject + iCertificateFromObjectStart, &guidAESDecryptionKeyID, SIZEOF( DRM_GUID ) );
    }

    /* Write the keyfile data to file */
    ChkDR( Oem_Keyfile_StoreToFile( f_pKeyFile ) );

    /* Convert the data in the internal key file buffer to machine byte order. */
    ChkDR( _LoadKeyfile( f_pKeyFile, FALSE ) );

ErrorExit:
    return dr;
}

/*******************************************************************************
** Function:    DRM_KF_SetCertificate
**
** Synopsis:    Sets a certificate in the keyfile data cache
**
**              [f_pKeyFile]    Keyfile context
**              [f_eCertType]   Type of certificate to
**                              set
**              [f_pbCertData]  Certificate data to be
**                              set
**              [f_cbCertData]  Size in bytes of the certificate
**                              data
**
** Returns:
**              DRM_SUCCESS              The function completed successfully.
**              DRM_E_INVALIDARG         One or more arguments were invalid.
**              DRM_E_KEYFILE_NOT_INITED The KeyFile context was not initialized prior to use.
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_KF_SetCertificate(
    __in                            DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    __in                            DRM_KF_CERT_TYPE       f_eCertType,
    __in_bcount(f_cbCertData) const DRM_BYTE              *f_pbCertData,
    __in                            DRM_DWORD              f_cbCertData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_KEYFILE_CERT_TYPE eFormatCertTypeToSet = KF_CERT_TYPE_INVALID;

    ChkBOOL( DRM_DEVICEASSETS_IsDeviceAssetsSupported(), DRM_E_NOTIMPL );

    if( DRM_ACTIVATION_IsActivationSupported( ) )
    {
        ChkDR( DRM_E_KEYFILE_UPDATE_NOT_ALLOWED );
    }

    ChkArg( f_pKeyFile != NULL );
    ChkArg( f_pbCertData != NULL );
    ChkArg( f_cbCertData != 0 );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->fLoaded );

    /* Perform an explicit mapping.  This ensures that the API can change without regaurd to the
    ** core format defines
    */
    eFormatCertTypeToSet = MapApiCertTypeToFormatCertType( f_eCertType );
    ChkArg( eFormatCertTypeToSet != KF_CERT_TYPE_INVALID );

    ChkDR( _UpdateKeyfile( f_pKeyFile, eFormatCertTypeToSet, f_pbCertData, f_cbCertData, FALSE, KF_ASYMM_KEY_TYPE_INVALID, NULL, 0, NULL, NULL ) );

    /* Load the keyfile format back into the context */
    ChkDR( _LoadKeyfile( f_pKeyFile, FALSE ) );

ErrorExit:
    return dr;
}


/*******************************************************************************
** Function:    DRM_KF_SetPrivateKey
**
** Synopsis:    Set an encrypted private key into the keyfile cache
**
**              [f_pKeyFile]     Keyfile context
**              [f_eKeyType]  Cipher type of the supplied encrypted private
**                               key
**              [f_pbPubKey]     Public key corresponding to the supplied privat
**                               e key
**              [f_cbPubKey]     Size in bytes of the public
**                               key
**              [f_pKey]         Struct containing the encrypted private
**                               key
**
** Returns:
**              DRM_SUCCESS              The function completed successfully.
**              DRM_E_INVALIDARG         One or more arguments were invalid
**              DRM_E_KEYFILE_NOT_INITED The KeyFile context was not initialized prior to use.
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_KF_SetPrivateKey(
    __in                              DRM_KEYFILE_CONTEXT   *f_pKeyFile,
    __in                              DRM_KF_KEY_TYPE        f_eKeyType,
    __in_bcount_opt(f_cbPubKey) const DRM_BYTE              *f_pbPubKey,
    __in                              DRM_DWORD              f_cbPubKey,
    __in                              DRM_ENCRYPTED_KEY     *f_pKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_KEYFILE_ASYMMETRIC_KEY_TYPE eAsymmKeyType = KF_ASYMM_KEY_TYPE_INVALID;
    DRM_BOOL fUpdateSecureStoreKey = FALSE;

    ChkBOOL( DRM_DEVICEASSETS_IsDeviceAssetsSupported(), DRM_E_NOTIMPL );

    if( DRM_ACTIVATION_IsActivationSupported( ) )
    {
        ChkDR( DRM_E_KEYFILE_UPDATE_NOT_ALLOWED );
    }

    ChkArg( f_pKeyFile != NULL );
    ChkArg( f_pKey != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->fLoaded );

    if( eKF_KEY_SECURE_STORE == f_eKeyType )
    {
        fUpdateSecureStoreKey = TRUE;
    }
    else
    {
        eAsymmKeyType = MapApiKeyTypeToFormatKeyType( f_eKeyType );

        ChkArg( eAsymmKeyType != KF_ASYMM_KEY_TYPE_INVALID );
    }

    ChkDR( _UpdateKeyfile(
        f_pKeyFile,
        KF_CERT_TYPE_INVALID,
        NULL,
        0,
        fUpdateSecureStoreKey,
        eAsymmKeyType,
        f_pbPubKey,
        f_cbPubKey,
        f_pKey,
        NULL ) );

    /* Load the keyfile format back into the context */
    ChkDR( _LoadKeyfile( f_pKeyFile, FALSE ) );

ErrorExit:
    return dr;
}

/*******************************************************************************
** Function:    DRM_KF_SetCertParserCache
**
** Synopsis:    Set the device cert parser cache BLOB into the key file cache.
**
**              [f_pKeyFile]     Keyfile context
**              [f_pCertParserCache]
**                               Pointer to a device cert parser cache BLOB.
**
** Returns:
**              DRM_SUCCESS              The function completed successfully.
**              DRM_E_INVALIDARG         One or more arguments were invalid
**              DRM_E_KEYFILE_NOT_INITED The KeyFile context was not initialized prior to use.
**
*******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_KF_SetCertParserCache(
    __in  DRM_KEYFILE_CONTEXT          *f_pKeyFile,
    __in  DRM_DEVICE_CERT_PARSER_CACHE *f_pCertParserCache )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkBOOL( DRM_DEVICEASSETS_IsDeviceAssetsSupported(), DRM_E_NOTIMPL );

    if( DRM_ACTIVATION_IsActivationSupported( ) )
    {
        ChkDR( DRM_E_KEYFILE_UPDATE_NOT_ALLOWED );
    }

    ChkArg( f_pKeyFile != NULL );
    ChkBOOL( f_pKeyFile->fInited, DRM_E_KEYFILE_NOT_INITED );
    ChkArg( f_pKeyFile->fLoaded );
    ChkArg( f_pCertParserCache != NULL );

    ChkDR( _UpdateKeyfile( f_pKeyFile, KF_CERT_TYPE_INVALID, NULL, 0, FALSE, KF_ASYMM_KEY_TYPE_INVALID, NULL, 0, NULL, f_pCertParserCache ) );

    /* Load the keyfile format back into the context */
    ChkDR( _LoadKeyfile( f_pKeyFile, FALSE ) );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

