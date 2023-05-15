/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_BLACKBOX_C

#include <drmblackboxtypes.h>
#include <blackboxinternal.h>
#include <blackboxlegacy.h>
#include <blackboxslk.h>
#include <blackboxhal.h>
#include <blackboxdevcerts.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <oempkcrypto.h>
#include <oemcipher.h>
#include <oemsha1.h>
#include <drmmanager.h>
#include <drmblackbox.h>
#include <oemaes.h>
#include <oemshahmac.h>
#include <drmxmrbuilder.h>
#include <drmdomainapi.h>
#include <drmdomainkeyxmrparser.h>
#include <drmdomainstore.h>
#include <oem.h>
#include <drmkeyfile.h>
#include <oemkeyfile.h>
#include <drmbcertbuilder.h>
#include <drmbcertparser.h>
#include <drmderivedkey.h>
#include <oemkeyfile.h>
#include <drmwmdrm.h>
#include <oemeccp160.h>
#include <drmprofile.h>
#include <drmconstants.h>
#include <drmlastinclude.h>
#ifdef TEE
#include <tee.h>
#endif


ENTER_PK_NAMESPACE_CODE;

#define HASH_RESIDUAL_LEN       6

static DRM_RESULT _InitializeHandleCaching(
    __inout_ecount( 1 )             DRM_BB_CONTEXT                       *f_pBbxContext,
    __inout_ecount( 1 )             DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCache,
    __in_bcount(f_cbCertData) const DRM_BYTE                             *f_pbCertData,
    __in                      const DRM_DWORD                             f_cbCertData,
    __in                            DRM_DWORD                             f_dwChainLength,
    __in                            DRM_DWORD                             f_bOffset,
    __out_ecount( 1 )               OEM_CRYPTO_HANDLE                    *f_phCertChainPubKey );

static DRM_RESULT _UnwrapPublicKeyHandle(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT         *f_pBbxContext,
    __in                                    OEM_CRYPTO_HANDLE       f_hWrappingKey,
    __in_bcount( f_cbWrappedKeyData ) const DRM_BYTE               *f_pbCertificate,
    __in                                    DRM_DWORD               f_cbCertificate,
    __in                                    DRM_DWORD               f_iKeyInCert,
    __out_ecount( 1 )                       DRM_PUBLIC_KEY_CONTEXT *f_pKeyContext );

static DRM_VOID _CompleteHandleCaching(
    __inout_ecount_opt(1)           DRM_BB_CONTEXT                       *f_pBbxContext,
    __inout_ecount_opt(1)           DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCache,
    __in                            DRM_RESULT                            f_drCachingResult,
    __inout_ecount_opt(1)           OEM_CRYPTO_HANDLE                    *f_phCertChainPubKey );

static DRM_VOID _CleanupCache(
    __inout_ecount_opt(1)           DRM_BB_CONTEXT                       *f_pBbxContext,
    __inout_ecount_opt(1)           DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCache );

static DRM_BB_KEYHANDLE_CALLBACKS g_HandleCallbacks = { 0 };

static DRMFORCEINLINE DRM_VOID _compile_time_asserts( DRM_VOID )
{
    DRMCASSERT( SIZEOF( DRM_DECRYPT_CONTEXT ) == SIZEOF( DRM_CIPHER_CONTEXT ) );
}

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_BBX_IsHandleCachingSupported(DRM_VOID)
{
    return _Hal_IsHandleCachingSupported();
}

DRM_API DRM_RESULT DRM_CALL DRM_BBX_Initialize(
    __in_opt            DRM_VOID           *f_pOEMContext,
    __inout_ecount( 1 ) DRM_BB_CONTEXT     *f_pcontextBBX )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_Initialize);

    AssertChkArg( !f_pcontextBBX->fInited );

    /*
    ** Initialize the callback structure
    */
    g_HandleCallbacks.pfInitializeCaching     = _InitializeHandleCaching;
    g_HandleCallbacks.pfUnwrapPublicKeyHandle = _UnwrapPublicKeyHandle;
    g_HandleCallbacks.pfCompleteCaching       = _CompleteHandleCaching;
    g_HandleCallbacks.pfCleanupCache          = _CleanupCache;

    ChkArg( f_pcontextBBX != NULL );
    _compile_time_asserts();

    ZEROMEM( &f_pcontextBBX->CryptoContext, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    f_pcontextBBX->pOEMContext = f_pOEMContext;

    f_pcontextBBX->pDomainStoreContext = NULL;
    f_pcontextBBX->prgOpaqueKeyMap     = NULL;

    ChkDR( _Hal_Initialize( f_pcontextBBX ) );

    f_pcontextBBX->prgOpaqueCallbackTable = &g_HandleCallbacks;

    f_pcontextBBX->fInited = TRUE;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_BBX_TestDeviceKeyPair(
    __inout_ecount( 1 )  DRM_BB_CONTEXT     *f_pcontextBBX )
{
    DRM_RESULT dr      = DRM_SUCCESS;

#if DBG
    DRM_BOOL   fInited = FALSE;

    ChkArg( f_pcontextBBX != NULL );

    fInited = f_pcontextBBX->fInited;

    if( DRM_WMDRM_IsWMDRMSupported()
     && f_pcontextBBX->pKeyFileContext != NULL )
    {
        f_pcontextBBX->fInited = FALSE;
        ChkDR(_TestDeviceKeyPair(f_pcontextBBX));
    }

ErrorExit:

    if( f_pcontextBBX != NULL )
    {
        f_pcontextBBX->fInited = fInited;
    }
#endif

    return dr;
}


DRM_API DRM_RESULT DRM_CALL DRM_BBX_Shutdown( __inout_ecount( 1 ) DRM_BB_CONTEXT *f_pcontextBBX )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pcontextBBX != NULL );

    if( f_pcontextBBX->fInited )
    {
        _CleanupCache( f_pcontextBBX, &f_pcontextBBX->cachedBCertValues );

        dr = _Hal_Deinitialize( f_pcontextBBX );

        ZEROMEM( f_pcontextBBX, SIZEOF( DRM_BB_CONTEXT ) );
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:    DRM_BBX_GetKeyHandleCallbacks
**
** Synopsis:    Return the key handle callback table, if implemented.
**
** Arguments:
**
** [f_pBbxContext]:   The blackbox context.
**
** Returns:     NULL or a valid KeyHandleCallback table.
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_BB_KEYHANDLE_CALLBACKS* DRM_CALL DRM_BBX_GetKeyHandleCallbacks(
    __inout_ecount( 1 )  DRM_BB_CONTEXT *f_pcontextBBX )
{
    DRM_BB_KEYHANDLE_CALLBACKS *pCallbacks = NULL;

    DRMASSERT( f_pcontextBBX != NULL );

    if( f_pcontextBBX != NULL )
    {
        pCallbacks = ( DRM_BB_KEYHANDLE_CALLBACKS* )f_pcontextBBX->prgOpaqueCallbackTable;
    }

    return pCallbacks;
}


/*
    Test whether or not a key is valid and can be used to decrypt
    content.
*/
DRM_API DRM_BOOL DRM_CALL DRM_BBX_CanBind(
    __in_ecount( f_dwChainLength ) const DRM_BINDING_INFO *f_pBindingInfo,
    __in                                 DRM_DWORD         f_dwChainLength,
    __inout_ecount( 1 )                  DRM_BB_CONTEXT   *f_pcontextBBX,
    __out_opt                            DRM_RESULT       *f_pDr )
{
    DRM_BYTE          rgbKey [DRM_AES_KEYSIZE_128] = { 0 };   /* 16 BYTES */
    DRM_DWORD         f_cbKey     = 0;
    DRM_RESULT        dr          = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE hContentKey = OEM_CRYPTO_HANDLE_INVALID;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_CanBind );

    ChkArg( f_pBindingInfo != NULL );

    if ( f_pBindingInfo[0].m_fXMRLicense )
    {
        dr = _DecryptContentKeyXMR( f_pBindingInfo,
                                    f_dwChainLength,
                                    f_pcontextBBX,
                                    TRUE,
                                    &hContentKey,
                                    NULL );
    }
    else
    {
        dr = _DecryptContentKeyXML( f_pBindingInfo,
                                    f_dwChainLength,
                                    f_pcontextBBX,
                                    TRUE, /* Ignore CK checksum on CanBind */
                                    TRUE, /* Ignore CK signature on CanBind */
                                    DRM_AES_KEYSIZE_128,
                                    rgbKey,
                                   &f_cbKey,
                                    NULL );
    }
    if ( f_pDr != NULL )
    {
        *f_pDr = dr;
    }

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_pcontextBBX, &hContentKey );
    DRM_PROFILING_LEAVE_SCOPE;
    return DRM_SUCCEEDED( dr );
}


/*
** Note that this API must be called in the context of the keyfile context already open
** and locked for writing.
*/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_SetupSecureStoreKey(
    __inout_ecount( 1 ) DRM_BB_CONTEXT *f_pBlackBoxContext,
    __in_opt            DRM_VOID       *f_pOEMContext )
{
    DRM_RESULT        dr         = DRM_SUCCESS;
    DRM_ENCRYPTED_KEY oEncKey    = { 0 };
    OEM_CRYPTO_HANDLE hStoreKey  = OEM_CRYPTO_HANDLE_INVALID;

    ChkArg( f_pBlackBoxContext != NULL );

    /*
    ** First check if we have the secure store key loaded in the keyfile cache already to avoid having to lock
    ** the keyfile with a call to DRM_KF_Open
    */
    dr = DRM_KF_GetPrivateKey( f_pBlackBoxContext->pKeyFileContext,
                               eKF_KEY_SECURE_STORE,
                               NULL,
                               0,
                               &oEncKey );

    if( dr == DRM_E_KEYFILE_PRIVATE_KEY_NOT_FOUND )
    {
        DRM_ENCRYPTED_KEY oEncryptedKey = { 0 };
        DRM_BYTE rgbScratch[KF_MAX_ENCRYPTION_BUFFER] = {0};

        /*
        ** No secure store key yet, need to create and store one. The DRM_KF_Open function will
        ** lock the keyfile exclusively so that nobody else should be trying to add one at the same time
        */
        ChkDR( _Hal_GenerateKey( f_pBlackBoxContext,
                                 OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE,
                                 0,
                                 &hStoreKey ) );

        oEncryptedKey.pbEncryptedKey = rgbScratch;
        oEncryptedKey.cbEncryptedKey = SIZEOF(rgbScratch);

        ChkDR( DRM_BBX_KF_EncryptKey( f_pBlackBoxContext,
                                      hStoreKey,
                                      &oEncryptedKey ) );

        ChkDR( DRM_KF_SetPrivateKey( f_pBlackBoxContext->pKeyFileContext,
                                     eKF_KEY_SECURE_STORE,
                                     NULL,
                                     0,
                                     &oEncryptedKey ) );
    }
    else
    {
        ChkDR( dr );
    }

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_pBlackBoxContext, &hStoreKey );
    return dr;
}


/*  Used for computing the secure store password. */
DRM_API DRM_RESULT DRM_CALL DRM_BBX_HashValue(
    __in_bcount( f_cbValue )  const     DRM_BYTE       *f_pbValue,
    __in                                DRM_DWORD       f_cbValue,
    __in_bcount( DRM_SHA1_DIGEST_LEN )  DRM_BYTE        f_rgbHashValue[DRM_SHA1_DIGEST_LEN],
    __inout_ecount( 1 )                 DRM_BB_CONTEXT *f_pcontextBBX )
{
    DRM_RESULT        dr          = DRM_SUCCESS;
    DRM_ENCRYPTED_KEY oEncKey     = { 0 };
    OEM_CRYPTO_HANDLE hKeyFileKey = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE hSigningKey = OEM_CRYPTO_HANDLE_INVALID;
    DRM_DWORD         cbHashValue = DRM_SHA1_DIGEST_LEN;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_HashValue);

    ChkArg( f_pbValue      != NULL
         && f_cbValue       > 0
         && f_rgbHashValue != NULL
         && f_pcontextBBX  != NULL
         && f_pcontextBBX->fInited );

    ChkDR( DRM_KF_GetPrivateKey( f_pcontextBBX->pKeyFileContext,
                                 eKF_KEY_SECURE_STORE,
                                 NULL,
                                 0,
                                 &oEncKey ) );

    ChkDR( _Hal_GetPreloadedKey( f_pcontextBBX,
                                 OEM_HAL_KEY_TYPE_AES_KEYFILE,
                                 &oEncKey.guidKeypairDecryptionKeyID,
                                 &hKeyFileKey ) );

    ChkDR( _Hal_UnwrapKey( f_pcontextBBX,
                           OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE,
                           hKeyFileKey,
                           ( DRM_BYTE* )&oEncKey,
                           SIZEOF( oEncKey ),
                           NULL,
                           0,
                           oEncKey.pbEncryptedKey,
                           oEncKey.cbEncryptedKey,
                           &hSigningKey) );

    /*
    ** AES 128 only generates 16 bytes of signature, the 20 bytes hash will
    ** be padded with trailing 0s.
    */
    OEM_SECURE_ZERO_MEMORY( f_rgbHashValue, DRM_SHA1_DIGEST_LEN );
    DRMCASSERT( DRM_AES_BLOCKLEN <= DRM_SHA1_DIGEST_LEN );

    ChkDR( _Hal_CreateOMAC1Signature( f_pcontextBBX,
                                      f_pbValue,
                                      f_cbValue,
                                      f_rgbHashValue,
                                      &cbHashValue,
                                      hSigningKey ) );

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_pcontextBBX, &hKeyFileKey );
    DRM_BBX_SafeReleaseKey( f_pcontextBBX, &hSigningKey );
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   DRM_BBX_CipherKeySetup
**
** Synopsis :   Set up the content key data ( decryption or encryption )
**
** Arguments :
**          [f_pBindingInfo]     : Binding information
**          [f_dwChainLength]    : Chain length
**          [f_pCipherContext]   : Cipher context to set up
**          [f_pcontextBBX]      : Blackbox content
**
** Returns :
**          DRM_SUCCESS
**              Success
**          DRM_E_INVALIDARG
**              A required parameter was NULL or improperly initialized
**          DRM_E_CH_BAD_KEY
**              The checksum over the content key failed
**          DRM_E_UNSUPPORTED_ALGORITHM
**              The symmetric encryption type specified in f_pBindingInfo is
**              unsupported or unrecognized
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_CipherKeySetup(
    __in_ecount( f_dwChainLength ) const DRM_BINDING_INFO    *f_pBindingInfo,
    __in                                 DRM_DWORD            f_dwChainLength,
    __out_ecount( 1 )                    DRM_CIPHER_CONTEXT  *f_pCipherContext,
    __inout_ecount( 1 )                  DRM_BB_CONTEXT      *f_pcontextBBX )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_BYTE   rgbKey[DRM_AES_KEYSIZE_128] = { 0 };
    DRM_DWORD  cbKey = SIZEOF( rgbKey );
    DRM_SUPPORTED_CIPHERS eCipherType = eDRM_RC4_CIPHER;
    OEM_CRYPTO_HANDLE     hContentKey = OEM_CRYPTO_HANDLE_INVALID;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_CipherKeySetup );

    ChkArg( f_pBindingInfo   != NULL
         && f_pCipherContext != NULL
         && f_pcontextBBX    != NULL
         && f_pcontextBBX->fInited );

    if ( f_pBindingInfo[0].m_fXMRLicense )
    {


        ChkDR( _DecryptContentKeyXMR( f_pBindingInfo,
                                      f_dwChainLength,
                                      f_pcontextBBX,
                                      FALSE,
                                      &hContentKey,
                                      NULL ) );

        if ( f_pBindingInfo[0].m_BindingInfo.m_XMR.m_pxmrContentKey->wSymmetricCipherType
             == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR )
        {
            eCipherType = eDRM_AES_COUNTER_CIPHER;

            ChkDR( DRM_BBX_CPHR_InitWithKeyHandle( f_pcontextBBX, f_pCipherContext, eCipherType, hContentKey ) );
            /*/DRM_BBX_GetContentKey(f_pCipherContext, hContentKey);*/
        }
        else if ( f_pBindingInfo[0].m_BindingInfo.m_XMR.m_pxmrContentKey->wSymmetricCipherType
                  == XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER ||
                  f_pBindingInfo[0].m_BindingInfo.m_XMR.m_pxmrContentKey->wSymmetricCipherType
                  == XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL )
        {
            /*
            ** Extract the legacy key into rgbKey
            */
            ChkDR( _Hal_WrapKey( f_pcontextBBX,
                                 hContentKey,
                                 OEM_CRYPTO_HANDLE_INVALID,
                                 rgbKey,
                                 &cbKey ) );

            ChkDR( DRM_CPHR_Init( f_pCipherContext, eCipherType, cbKey, rgbKey ) );
        }
        else
        {
            ChkDR( DRM_E_UNSUPPORTED_ALGORITHM );
        }
    }
    else
    {


        ChkDR( _DecryptContentKeyXML( f_pBindingInfo,
                                      f_dwChainLength,
                                      f_pcontextBBX,
                                      FALSE, /* Enforce CK checksum on Bind */
                                      f_pBindingInfo->m_BindingInfo.m_XML.m_fCKSigVerified,
                                      SIZEOF( rgbKey ),
                                      rgbKey,
                                      &cbKey,
                                      NULL ) );

        ChkDR( DRM_CPHR_Init( f_pCipherContext, eCipherType, cbKey, rgbKey ) );
    }

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_pcontextBBX, &hContentKey );
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}
#if 0
//Added For Get Content Key
DRM_API DRM_RESULT DRM_CALL DRM_BBX_GetContentKey(
    __out_ecount( 1 )                    DRM_CIPHER_CONTEXT  *f_pCipherContext,
    __in_ecount( 1 )            	     OEM_CRYPTO_HANDLE   *f_phKey)
{
    DRM_RESULT dr = DRM_SUCCESS;

    _Hal_GetContentKey(( DRM_BB_CONTEXT* )f_pCipherContext->pOpaqueBbx,
    					f_pCipherContext);

ErrorExit:
    return dr;
}
#endif

/******************************************************************************
**
** Function :   DRM_BBX_CPHR_InitDecrypt
**
** Synopsis :   Initializes state for phased decryption, calls into drmcipher.c
**
** Arguments :
**      f_pCipherContext : Cipher context initialized with DRM_CPHR_Init
**      f_pbLast15 : Pointer to the last 15 bytes of the ciphertext. If the
**                   ciphertext is less than 15 bytes, it is a pointer to the
**                   beginning of the ciphertext.
**      f_cbData   : # of bytes in ENTIRE ciphertext
**
** Returns :
**      DRM_SUCCESS
**          success
**      DRM_E_INVALIDARG
**          f_pContext is null, or f_pbLast15 is null when cipher type is RC4
**      DRM_E_CIPHER_NOT_INITIALIZED
**          f_pContext was not appropriately initialized
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_CPHR_InitDecrypt(
    __inout_ecount( 1 )                                                DRM_CIPHER_CONTEXT *f_pCipherContext,
    __in_bcount_opt( min(DRM_CPHR_CB_FOR_INITDECRYPT,f_cbData) ) const DRM_BYTE           *f_pbLast15,
    __in                                                               DRM_DWORD           f_cbData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pCipherContext != NULL );

    ChkDR( DRM_CPHR_InitDecrypt( f_pCipherContext,
                                 f_pbLast15,
                                 f_cbData ) );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_BBX_CPHR_Decrypt
**
** Synopsis :   Decrypts part of the ciphertext. This function must be called
**              after DRM_BBX_CPHR_InitDecrypt has been successful, calls into drmcipher.c
**
** Arguments :
**     f_pCipherContext     : Cipher context initialized with DRM_CPHR_Init
**                      followed by DRM_CPHR_InitDecrypt
**     f_pCtrContext  : Sets and maintains the
**                      initialization vector and counter
**                      for AES CTR mode. Must be
**                      specified for AES decryption, and
**                      ignored for Cocktail.
**     f_cbData       :  Length of pbData in bytes
**     f_pbData       :  Array of bytes to decrypt
**
**  Returns:
**      DRM_SUCCESS
**          success
**      DRM_E_INVALIDARG
**          pbData or pContextCipher is null
**      DRM_E_CIPHER_NOT_INITIALIZED
**          pContextCipher was not appropriately initialized
**      DRM_E_DECRYPT_NOT_INITIALIZED
**          pContextCipher was not appropriately initialized for decryption (Cocktail only)
**      DRM_E_CRYPTO_FAILED
**          The encrypt/decrypt operation failed
**      DRM_E_ARITHMETIC_OVERFLOW
**          An overflow/underflow occurred while processing the data
**
** Notes :      IMPORTANT: (COCKTAIL ONLY)
**              Phased decrypt should be called on segments of
**              encrypted data sequentially, i.e. if encrypted data is divided
**              four segments, the order of decryption should be segment 1,
**              followed by 2, 3, 4.
**              To remove complexity we do not handle the case where
**              a packet is split somewhere in the middle of the last 15 bytes.
**              The caller is responsible to ensure that the last 15 bytes never
**              straddle multiple calls to Decrypt.
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_CPHR_Decrypt(
    __in_ecount( 1 )            DRM_CIPHER_CONTEXT           *f_pCipherContext,
    __inout_opt                 DRM_AES_COUNTER_MODE_CONTEXT *f_pCtrContext,
    __in                        DRM_DWORD                     f_cbData,
    __inout_bcount( f_cbData )  DRM_BYTE                     *f_pbData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pCipherContext != NULL );

    if( f_pCipherContext->eCipherType              == eDRM_AES_COUNTER_CIPHER
     && f_pCipherContext->cipher.cipherAES.hAesKey != OEM_CRYPTO_HANDLE_INVALID )
    {
        ChkArg( f_pCipherContext->pOpaqueBbx != NULL );

        ChkDR( _Hal_DecryptContent( ( DRM_BB_CONTEXT* )f_pCipherContext->pOpaqueBbx,
                                    f_pCipherContext,
                                    f_pCtrContext,
                                    f_cbData,
                                    f_pbData ) );
    }
    else
    {
        ChkDR( DRM_CPHR_Decrypt( f_pCipherContext,
                                 f_pCtrContext,
                                 f_cbData,
                                 f_pbData ) );
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_BBX_CPHR_DecryptOpaque(
    __in_ecount( 1 )            DRM_CIPHER_CONTEXT           *f_pCipherContext,
    __inout                     DRM_AES_COUNTER_MODE_CONTEXT *f_pCtrContext,
    __in                        OEM_OPAQUE_BUFFER_HANDLE      f_hInBuf,
    __in                        OEM_OPAQUE_BUFFER_HANDLE      f_hOutBuf,
    __in                        DRM_DWORD                     f_cbData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkPtr( f_pCipherContext );
    ChkPtr( f_pCipherContext->pOpaqueBbx );

    ChkArg( f_pCipherContext->eCipherType == eDRM_AES_COUNTER_CIPHER );
    ChkArg( f_pCipherContext->cipher.cipherAES.hAesKey != OEM_CRYPTO_HANDLE_INVALID );

    ChkDR( _Hal_DecryptContentOpaque( ( DRM_BB_CONTEXT* )f_pCipherContext->pOpaqueBbx,
                                      f_pCipherContext,
                                      f_pCtrContext,
                                      f_hInBuf,
                                      f_hOutBuf,
                                      f_cbData ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
**  Function:  DRM_BBX_CPHR_InitWithKeyHandle
**
**  Synopsis:  Initializes a content cipher context.  This must be done prior to using
**             DRM_CPHR_Encrypt or DRM_BBX_CPHR_Decrypt, calls into drmcipher.c.
**
**  Arguments:
**     f_pContext     :  The cipher context to be initialized
**     f_eCipherType  :  The type of cipher to be used
**     f_hContKey     :  The content key as a HAL key handle
**
**  Returns:
**      DRM_SUCCESS
**          success
**      DRM_E_INVALIDARG
**          one of the arguments was NULL or out of range
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_CPHR_InitWithKeyHandle(
    __inout_ecount( 1 )                DRM_BB_CONTEXT        *f_pcontextBBX,
    __out_ecount(1)                    DRM_CIPHER_CONTEXT    *f_pContext,
    __in                               DRM_SUPPORTED_CIPHERS  f_eCipherType,
    __in                               OEM_CRYPTO_HANDLE      f_hContKey )
{
    DRM_RESULT        dr             = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE hCryptoContKey = OEM_CRYPTO_HANDLE_INVALID;

    ChkArg( NULL != f_pContext );

    /*
    ** Create a duplicate of the key handle, so that it lasts
    ** the lifespan of the cipher context
    */
    ChkDR( _Hal_DuplicateKey( f_pcontextBBX,
                              f_hContKey,
                              &hCryptoContKey ) );

    ChkDR( DRM_CPHR_InitWithKeyHandle( f_pcontextBBX,
                                       f_pContext,
                                       f_eCipherType,
                                       hCryptoContKey ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
**  Function:  DRM_BBX_CPHR_Uninitialize
**
**  Synopsis:  Uninitializes a content cipher context.  This must be done when the
**             cipher context is no longer needed, in order to release any
**             associated resources
**
**  Arguments:
**     f_pContext     :  The cipher context to be uninitialized
**
*********************************************************************/
DRM_API DRM_VOID DRM_CALL DRM_BBX_CPHR_Uninitialize(
    __inout_ecount(1) DRM_CIPHER_CONTEXT *f_pContext )
{
    if( f_pContext != NULL )
    {
        if( f_pContext->fInited                  == TRUE
         && f_pContext->eCipherType              == eDRM_AES_COUNTER_CIPHER
         && f_pContext->cipher.cipherAES.hAesKey != OEM_CRYPTO_HANDLE_INVALID )
        {
            DRMASSERT( f_pContext->pOpaqueBbx != NULL );

            DRM_BBX_SafeReleaseKey( ( DRM_BB_CONTEXT* )f_pContext->pOpaqueBbx,
                                    &f_pContext->cipher.cipherAES.hAesKey );
        }

        OEM_SECURE_ZERO_MEMORY( f_pContext,
                                SIZEOF( DRM_CIPHER_CONTEXT ) );
    }
}


/*********************************************************************
**
**  Function:  DRM_BBX_CPHR_Duplicate
**
**  Synopsis:  Duplicates a content cipher context, managing the
**             references to any associated resources
**
**  Arguments:
**     f_pContext           :  The cipher context to be duplicated
**     f_pDuplicateContext  :  Returns an initialized duplicate
**
**  Returns:
**      DRM_SUCCESS
**          success
**      DRM_E_INVALIDARG
**          one of the arguments was NULL or out of range
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_CPHR_Duplicate(
    __inout_ecount(1) DRM_CIPHER_CONTEXT *f_pContext,
    __out_ecount(1)   DRM_CIPHER_CONTEXT *f_pDuplicateContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pContext                   != NULL );
    ChkArg( f_pDuplicateContext          != NULL );

    /*
    ** If the decrypt context being duplicated to is initialized, close it before overwriting
    */
    DRM_BBX_CPHR_Uninitialize( f_pDuplicateContext );

    OEM_SECURE_MEMCPY( ( DRM_BYTE * )f_pDuplicateContext,
                       ( DRM_BYTE * )f_pContext,
                       SIZEOF( DRM_CIPHER_CONTEXT ) );

    /*
    ** If the cipher context contains a key handle, duplicate this, to
    ** correctly increment the handle's reference count
    */
    if( f_pContext->fInited                  == TRUE
     && f_pContext->eCipherType              == eDRM_AES_COUNTER_CIPHER
     && f_pContext->cipher.cipherAES.hAesKey != OEM_CRYPTO_HANDLE_INVALID )
    {
        ChkArg( f_pContext->pOpaqueBbx != NULL );

        ChkDR( _Hal_DuplicateKey( ( DRM_BB_CONTEXT* )f_pContext->pOpaqueBbx,
                                  f_pContext->cipher.cipherAES.hAesKey,
                                  &f_pDuplicateContext->cipher.cipherAES.hAesKey ) );
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
**  Function:  _UpdateXMRWithSymmKey
**
**  Synopsis:  Takes an XMR license (bound to an asymmetric key or symmetric SLK),
**             a content integrity key (clear), and a new content and integrity key (re-encrypted symmetrically),
**             and updates the license with the symmetric key and new signature.
**             This function expects that the XMR license already contains an optimized content key object
**
**  Arguments:
**     [f_hCIKey]          : A handle to a key with a content integrity component
**     [f_rgbCIKey]        : Buffer containing the clear CI key
**     [f_rgbEncryptedKeys]: Buffer containing the re-encrypted content and CI keys
**     [f_pXmrLicense]     : Pointer to the license object
**
** Notes: only one of f_hCIKey or f_rgbCIKey may be used. If both
**        contain valid values, this function will return DRM_E_INVALIDARG
**
*********************************************************************/
DRM_RESULT DRM_CALL _UpdateXMRWithSymmKey(
    __inout_ecount( 1 )                          DRM_BB_CONTEXT   *f_pcontextBBX,
    __in                                         OEM_CRYPTO_HANDLE f_hCIKey,
    __in_bcount_opt( DRM_AES_KEYSIZE_128 ) const DRM_BYTE         *f_rgbCIKey,
    __in_bcount( DRM_AES_KEYSIZE_128 * 2 ) const DRM_BYTE          f_rgbEncryptedKeys[DRM_AES_KEYSIZE_128 * 2],
    __inout_ecount( 1 )                          DRM_XMR_LICENSE  *f_pXmrLicense )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_BYTE    rgbSignature[XMR_AES_OMAC_SIGNATURE_LENGTH];
    DRM_DWORD   cbSignature = SIZEOF( rgbSignature );
    DRM_AES_KEY aesKey; /* Do not zero-init key material */

    ChkArg( f_pXmrLicense != NULL );
    ChkArg( f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.fValid );
    ChkArg( f_hCIKey == OEM_CRYPTO_HANDLE_INVALID || f_rgbCIKey == NULL );

    ChkBOOL( f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.cbEncryptedKey == DRM_AES_BLOCKLEN * 2,
             DRM_E_INVALID_LICENSE );
    DRM_BYT_CopyBytes( f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.pbEncryptedKeyBuffer,
                       f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.iEncryptedKey,
                       f_rgbEncryptedKeys,
                       0,
                       DRM_AES_BLOCKLEN * 2 );

    f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.wKeyEncryptionCipherType = XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB;

    /*
    ** Copy the new encryption type to the license buffer
    */
    ChkBOOL( f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.iEncryptedKey > 2 * SIZEOF( DRM_WORD ),
             DRM_E_INVALID_LICENSE );
    WORD_TO_NETWORKBYTES( f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.pbEncryptedKeyBuffer,
                          f_pXmrLicense->containerOuter.containerKeys.OptimizedContentKey.iEncryptedKey - SIZEOF( DRM_WORD ) * 2,
                          XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB );

    if( f_hCIKey != OEM_CRYPTO_HANDLE_INVALID )
    {
        ChkDR( _Hal_CreateOMAC1Signature( f_pcontextBBX,
                                          &f_pXmrLicense->pbSignedDataBuffer[f_pXmrLicense->iSignedData],
                                          f_pXmrLicense->cbSignedData,
                                          rgbSignature,
                                          &cbSignature,
                                          f_hCIKey ) );
    }
    else
    {


        ChkDR( Oem_Aes_SetKey(f_rgbCIKey, &aesKey) );

        ChkDR( Oem_Omac1_Sign(&aesKey, f_pXmrLicense->pbSignedDataBuffer, f_pXmrLicense->iSignedData, f_pXmrLicense->cbSignedData, rgbSignature ) );
    }

    DRM_BYT_CopyBytes( f_pXmrLicense->containerOuter.signature.pbSignatureBuffer,
                       f_pXmrLicense->containerOuter.signature.iSignature,
                       rgbSignature,
                       0,
                       XMR_AES_OMAC_SIGNATURE_LENGTH );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( &aesKey, SIZEOF( aesKey ) );
    return dr;
}


/*********************************************************************
**
**  Function:  DRM_BBX_AsymmetricToSymmetricKeyXMR
**
**  Synopsis:  Takes an asymmetrically encrypted license and symmetrically
**             rebinds it to the current blackbox. Adds the optimized content key object
**             if it doesn't already exist, then updates the signature object in-place.
**
**  Arguments:
**     [f_pcontextBBX]      : Pointer to an initialized blackbox context
**     [f_pbindinfoEx]      : Pointer to the binding info of the license that is being optmized
**     [f_pbLicense]        : Pointer to the license bytes
**     [f_cbLicense]        : Length of the entire license buffer
**     [f_pcbLicenseUsed]   : Pointer to the length of the license buffer used.
**     [f_pStack]           : Pointer to stack allocator context used to unpack license
**     [f_pcbSymmKey]       : Length of the resultant symmetric content key
**
**  Notes:  The content key will be reencrypted and returned in the same buffer.
**          The new length of the symmetric key is returned in f_pcbSymmKey.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_AsymmetricToSymmetricKeyXMR(
    __inout_ecount( 1 )               DRM_BB_CONTEXT                *f_pcontextBBX,
    __inout_ecount_opt( 1 )           DRM_BINDING_INFO              *f_pbindinfo,
    __inout_bcount( f_cbLicense )     DRM_BYTE                      *f_pbLicense,
    __in                              DRM_DWORD                      f_cbLicense,
    __inout_ecount( 1 )               DRM_DWORD                     *f_pcbLicenseUsed,
    __inout_ecount( 1 )               DRM_STACK_ALLOCATOR_CONTEXT   *f_pStack,
    __out_ecount( 1 )                 DRM_DWORD                     *f_pcbSymmKey )
{
    DRM_RESULT   dr      = DRM_SUCCESS;
    DRM_BYTE     rgbKey[ECC_P256_PLAINTEXT_SIZE_IN_BYTES] = { 0 };
    DRM_DWORD    cbKey   = SIZEOF( rgbKey );
    DRM_XMR_BINDING_INFO *pXmrBindInfo = NULL;
    OEM_CRYPTO_HANDLE hPrivKey         = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE hContentKey      = OEM_CRYPTO_HANDLE_INVALID;

    ChkArg( f_pcontextBBX    != NULL );
    ChkArg( f_pcbSymmKey     != NULL );
    ChkArg( f_pcbLicenseUsed != NULL );
    ChkArg( f_pbindinfo      != NULL );
    ChkArg( f_pStack         != NULL );

    pXmrBindInfo = &f_pbindinfo->m_BindingInfo.m_XMR;

    ChkArg( f_pbindinfo->m_BindingInfo.m_XMR.m_pxmrContentKey != NULL );
    ChkArg( f_pcontextBBX->fInited );
    ChkArg( pXmrBindInfo->m_pxmrContentKey->wKeyEncryptionCipherType
         == XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256
         || pXmrBindInfo->m_pxmrContentKey->wKeyEncryptionCipherType
         == XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256_WITH_KZ
          ); /* Must be an asymmetric encryption */

    ChkDR( _DecryptContentKeyXMR( f_pbindinfo,
                                  1,
                                  f_pcontextBBX,
                                  TRUE,
                                  &hContentKey,
                                  &hPrivKey ) );

    ChkDR( _Hal_WrapKey( f_pcontextBBX,
                         hContentKey,
                         hPrivKey,
                         rgbKey,
                         &cbKey ) );

    *f_pcbSymmKey = cbKey;

    if( pXmrBindInfo->m_pxmrOptimizedContentKey == NULL
     || !pXmrBindInfo->m_pxmrOptimizedContentKey->fValid )
    {
        /*
        ** Insert Optimized content key directly after regular content key
        */
        DRM_DWORD   dwInsertionPoint    = pXmrBindInfo->m_pxmrLicense->containerOuter.containerKeys.ContentKey.iguidKeyID
                                              - XMR_BASE_OBJECT_LENGTH;

        ChkOverflow( pXmrBindInfo->m_pxmrLicense->containerOuter.containerKeys.ContentKey.iguidKeyID, dwInsertionPoint );

        /*
        ** Expand license to accommodate symmetric key and update size
        */
        ChkDR( DRM_XMR_UpdateLicenseSizeForSymmKey(  f_pbLicense,
                                                     f_cbLicense,
                                                     f_pcbLicenseUsed,
                                                    &dwInsertionPoint ) );

        /*
        ** Unpack license structure again in order to readjust pointers which have changed
        */
        f_pStack->nStackTop = 0; /* Reset top of stack */
        ZEROMEM( pXmrBindInfo->m_pxmrLicense, SIZEOF( *pXmrBindInfo->m_pxmrLicense ) );
        ChkDR( DRM_XMR_UnpackLicense( f_pbLicense,
                                     *f_pcbLicenseUsed,
                                      f_pStack,
                                      pXmrBindInfo->m_pxmrLicense ) );
        /*
        ** Reset pointers in binding info structure
        */
        pXmrBindInfo->m_pxmrContentKey          = &pXmrBindInfo->m_pxmrLicense->containerOuter.containerKeys.ContentKey;
        pXmrBindInfo->m_pxmrOptimizedContentKey = &pXmrBindInfo->m_pxmrLicense->containerOuter.containerKeys.OptimizedContentKey;
        pXmrBindInfo->m_pxmrSignature           = &pXmrBindInfo->m_pxmrLicense->containerOuter.signature;
        pXmrBindInfo->m_pxmrDomainID            = &pXmrBindInfo->m_pxmrLicense->containerOuter.containerGlobalPolicies.DomainID;
    }

    ChkDR( _UpdateXMRWithSymmKey( f_pcontextBBX,
                                  hContentKey,
                                  NULL,
                                  rgbKey,
                                  pXmrBindInfo->m_pxmrLicense ) );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( rgbKey, SIZEOF( rgbKey ) );
    DRM_BBX_SafeReleaseKey( f_pcontextBBX, &hPrivKey );
    DRM_BBX_SafeReleaseKey( f_pcontextBBX, &hContentKey );
    return dr;
}


/*********************************************************************
**
** Function: DRM_BBX_ECC256_Decrypt
**
** Synopsis: Function that does ECC-256 decryption of the passed in key pair
**           data, returning a DOMAIN_PROTECT key handle, using the device's
**           ECC-256 private key, whose matching ECC-256 public key is the
**           passed in ECC-256 public key.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_pbData]               -- Pointer to a buffer that contains the
**                             key pair data to be decrypted.
** [f_cbData]               -- Variable that contains the size
**                             (number of bytes) of the buffer to be
**                             decrypted.
** [f_poECC256PubKey]       -- Pointer to a device ECC-256 public key whose
**                             matching device ECC-256 private key should be
**                             used to decrypt the passed in data.
**                             If its content is all 0, caller assumes to
**                             use the current device private key to decrypt
**                             the data without checking whether
**                             *f_poECC256PubKey is the same as the current
**                             device public key.
** [f_hKeyPair]             -- Handle to the decrypted DOMAIN_PROTECT key pair.
**                             this handle must be released after use
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_CRYPTO_PUBLIC_KEY_NOT_MATCH if the
**                          passed in ECC-256 public key does not match any
**                          device public key.
**
** Note:                    The caller ensures the passed in buffer is big
**                          enough to receive the decrypted data.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_ECC256_Decrypt(
    __inout_ecount( 1 )                DRM_BB_CONTEXT    *f_poBBXContext,
    __in_bcount( f_cbData )      const DRM_BYTE          *f_pbData,
    __in                               DRM_DWORD          f_cbData,
    __inout_ecount( 1 )          const PUBKEY_P256       *f_poECC256PubKey,
    __out_ecount( 1 )                  OEM_CRYPTO_HANDLE *f_phKeyPair )
{
    DRM_RESULT        dr              = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE hPrivKey        = OEM_CRYPTO_HANDLE_INVALID;

    ChkArg( f_poBBXContext   != NULL );
    ChkArg( f_pbData         != NULL );
    ChkArg( f_cbData         == ECC_P256_CIPHERTEXT_SIZE_IN_BYTES );
    ChkArg( f_poECC256PubKey != NULL );
    ChkArg( f_phKeyPair      != NULL );

    *f_phKeyPair = OEM_CRYPTO_HANDLE_INVALID;

    if(DRM_UTL_IsZeros( (const DRM_BYTE*)f_poECC256PubKey, SIZEOF( PUBKEY_P256 ) ) )
    {
        f_poECC256PubKey = NULL;
    }

    ChkDR( _GetDeviceECC256KeyPair( f_poBBXContext,
                                    f_poECC256PubKey,
                                    NULL,
                                    &hPrivKey,
                                    DRM_BINARY_DEVICE_CERT_KEY_ENCRYPT ) );

    ChkDR( _Hal_UnwrapKey( f_poBBXContext,
                           OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT,
                           hPrivKey,
                           f_pbData,
                           f_cbData,
                           NULL,
                           0,
                           f_pbData,
                           f_cbData,
                           f_phKeyPair ) );

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hPrivKey );
    return dr;
}

/*********************************************************************
**
** Function: DRM_BBX_ECC256_Sign
**
** Synopsis: Function that generates an ECC-256 signature of the passed in
**           data. It also returns an ECC-256 public key used to verify
**           the generated ECC-256 signature.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_pbData]               -- Pointer to a data buffer for which
**                             ECC-256 signature is generated.
** [f_cbData]               -- Size (number of bytes) of a data buffer
**                             for which ECC-256 signature is generated.
** [f_poECC256PubKey]       -- Pointer to receive an ECC-256 public key
**                             whose matching ECC-256 private key is used
**                             to create the signature. The public key
**                             can be used to verify the signature
**                             generated in this function.
** [f_rgbECC256Signature]   -- Pointer to a buffer that receives a generated
**                             ECC-256 signature.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_ECC256_Sign(
    __inout_ecount( 1 )                                DRM_BB_CONTEXT *f_poBBXContext,
    __inout_bcount( f_cbData )                         const DRM_BYTE *f_pbData,
    __in                                               DRM_DWORD       f_cbData,
    __inout_ecount( 1 )                                PUBKEY_P256    *f_poECC256PubKey,
    __out_bcount( ECDSA_P256_SIGNATURE_SIZE_IN_BYTES ) DRM_BYTE        f_rgbECC256Signature[ECDSA_P256_SIGNATURE_SIZE_IN_BYTES] )
{
    DRM_RESULT        dr               = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE hPrivKey         = OEM_CRYPTO_HANDLE_INVALID;
    DRM_DWORD         dwSignatureSize  = ECDSA_P256_SIGNATURE_SIZE_IN_BYTES;

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poBBXContext->fInited );
    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData > 0 );
    ChkArg( f_poECC256PubKey != NULL );
    ChkArg( f_rgbECC256Signature != NULL );

    OEM_ECC_ZeroPublicKey_P256( f_poECC256PubKey );

    ZEROMEM( f_rgbECC256Signature, ECDSA_P256_SIGNATURE_SIZE_IN_BYTES );

    ChkDR( _GetDeviceECC256KeyPair( f_poBBXContext,
                                    NULL,
                                    f_poECC256PubKey,
                                    &hPrivKey,
                                    DRM_BINARY_DEVICE_CERT_KEY_SIGN ) );

    ChkDR( _Hal_SignWithPrivateKey( f_poBBXContext,
                                    f_pbData,
                                    f_cbData,
                                    f_rgbECC256Signature,
                                    &dwSignatureSize,
                                    OEM_HAL_HASH_TYPE_SHA256,
                                    OEM_HAL_SIGNATURE_SCHEME_ECDSA_P256,
                                    hPrivKey ) );

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hPrivKey );
    return dr;
}

#ifdef TEE
DRM_API DRM_RESULT DRM_CALL DRM_BBX_GetSecureStop(
    __inout_ecount( 1 )         DRM_BB_CONTEXT *f_poBBXContext,
    __in_bcount( 16 )           DRM_BYTE       *f_pbSessionID,
    __inout_bcount( f_cbData )  DRM_BYTE       *f_pbData,
    __in                        DRM_DWORD      *f_cbData,
    __out                       TEE_SIGNATURE  *f_sig)
{
    DRM_RESULT        dr               = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE hPrivKey         = OEM_CRYPTO_HANDLE_INVALID;

    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poBBXContext->fInited );
    ChkArg( f_pbSessionID != NULL );
    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData > 0 );
    ChkArg( f_sig != NULL );

    ChkDR( _GetDeviceECC256KeyPair( f_poBBXContext,
                                    NULL,
                                    NULL,
                                    &hPrivKey,
                                    DRM_BINARY_DEVICE_CERT_KEY_SIGN ) );

    ChkDR( TEE_GetSecureStop( f_pbSessionID, ((DRM_BB_KEY_REGISTER_ENTRY *)hPrivKey)->indexKeyRegister, f_pbData, f_cbData, f_sig) );

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hPrivKey );
    return dr;
}
#endif

/*********************************************************************
 *
** Function: DRM_BBX_StoreDomainPrivateKeys
**
** Synopsis:
**           Parse the domain key XMR data and store domain private
**           keys in the store
**
** Arguments:
**  [f_poBBXContext]            -- Pointer to a DRM_BB_CONTEXT context.
**  [f_poDomainStoreContext]    -- Pointer to a DRM_DOMAINSTORE_CONTEXT
**                                 context.
**  [f_pbData]                  -- Pointer to a DRM_BYTE buffer that
**                                 contains the domain key XMR data
**  [f_cbData]                  -- The size of the XMR data
**  [f_poDomainID]              -- Pointer to a DRM_DOMAIN_ID struct
**  [f_poECC256PubKey]          -- Pointer to the ECC256 Public Key that
**                                 is used to encrypt the AES session
**                                 key
**  [f_pdastrCertificate]       -- Pointer a DRM_ANSI_STRING that holds
**                                 the domain certificate
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**
**********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_BBX_StoreDomainPrivateKeys(
    __inout_ecount( 1 )                 DRM_BB_CONTEXT *f_poBBXContext,
    __inout_ecount( 1 )                 DRM_DOMAINSTORE_CONTEXT *f_poDomainStoreContext,
    __inout_bcount( f_cbData )          DRM_BYTE *f_pbData,
    __in                                DRM_DWORD f_cbData,
    __inout_ecount( 1 )                 DRM_DOMAIN_ID *f_poDomainID,
    __in_ecount( 1 )              const PUBKEY_P256 *f_poECC256PubKey,
    __in_ecount( 1 )                    DRM_ANSI_STRING *f_pdastrCertificate )
{
    DRM_RESULT                          dr                      = DRM_SUCCESS;
    DRM_DOMKEYXMR_HEADER                oXMRHeader              = {0};
    DRM_DOMKEYXMR_SESSIONKEY            oXMRSessionKey          = {0};
    DRM_DOMKEYXMR_PRIVKEYCONTAINER      oXMRPrivKeyContainer    = {0};
    DRM_DOMKEYXMR_PRIVKEY               oXMRDomainPrivKey       = {0};
    DRM_BB_DOMAIN_KEY_RECORD            oDomainKeyRecord        = {0};
    DRM_BCERT_CHAIN_HEADER              oChainHeader            = {0};
    DRM_BCERT_CERTIFICATE               oCert                   = {0};
    DRM_DWORD                           bCurrOffset             = 0;
    DRM_DWORD                           bCurrOffsetSaved        = 0;
    DRM_DWORD                           dwOffset                = 0;
    DRM_DWORD                           dwDomainRevisionPrev    = 0;
    DRM_BOOL                            fFirstRevision          = TRUE;
    DRM_BYTE                            rgbPasswordSST[DRM_SHA1_DIGEST_LEN] = {0};
    OEM_CRYPTO_HANDLE                   hDomainProtectKey       = OEM_CRYPTO_HANDLE_INVALID;
    OEM_CRYPTO_HANDLE                   hDomainKey              = OEM_CRYPTO_HANDLE_INVALID;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_BLACKBOX, PERF_FUNC_DRM_BBX_StoreDomainPrivateKeys );

    ChkBOOL( DRM_DOMAIN_IsDomainSupported(), DRM_E_NOTIMPL );
    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poDomainStoreContext != NULL );
    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData > 0 );  /*/ should this be the minumu data length*/
    ChkArg( f_poDomainID != NULL );
    ChkArg( f_poECC256PubKey != NULL );
    ChkDRMANSIString( f_pdastrCertificate );

    /* get domain key xmr header */
    ChkDR( DRM_DOMKEYXMR_GetHeader(f_pbData, f_cbData, &bCurrOffset, &oXMRHeader) );
    ChkBOOL( oXMRHeader.dwConstant == DRM_DOMKEYXMR_HEADER_CONSTANT, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
    ChkBOOL( oXMRHeader.dwVersion  == DRM_DOMKEYXMR_HEADER_VERSION,  DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
    ChkBOOL( bCurrOffset <= f_cbData, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );

    /* get domain key xmr session key */
    ChkDR( DRM_DOMKEYXMR_GetSessionKey( f_pbData, f_cbData, &bCurrOffset, &oXMRSessionKey ) );
    ChkBOOL( oXMRSessionKey.wType           == DRM_DOMKEYXMR_OBJTYPE_SESSIOINKEY,   DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
    ChkBOOL( oXMRSessionKey.wEncryptionType == DRM_DOMKEYXMR_SESSIONKEY_ENCTYPE_ECCP256, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
    ChkBOOL( oXMRSessionKey.dwKeyLength     == ECC_P256_CIPHERTEXT_SIZE_IN_BYTES, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
    ChkBOOL( bCurrOffset <= f_cbData, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );

    /* get domain key xmr privkey container */
    ChkDR( DRM_DOMKEYXMR_GetPrivKeyContainer(f_pbData, f_cbData, &bCurrOffset, &oXMRPrivKeyContainer ) );
    ChkBOOL( oXMRPrivKeyContainer.wType     == DRM_DOMKEYXMR_OBJTYPE_PRIVKEYCONTAINER, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
    ChkBOOL( bCurrOffset <= f_cbData, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );

    /* get domain cert */
    ChkDR( DRM_BCert_GetChainHeader( PB_DASTR( f_pdastrCertificate ),
                                     CB_DASTR( f_pdastrCertificate ),
                                     &dwOffset,
                                     &oChainHeader ) );

    ChkDR( DRM_BCert_GetCertificate( PB_DASTR( f_pdastrCertificate ),
                                     CB_DASTR( f_pdastrCertificate ),
                                     &dwOffset,
                                     &oCert,
                                     DRM_BCERT_CERTTYPE_DOMAIN ) );

    /*
    ** Verify the revision numbers to ensure that:
    ** 1) The revision numbers are in descending order.
    ** 2) The first revision number on the list should be same as the revision number of the domain cert.
    */
    bCurrOffsetSaved = bCurrOffset;
    while( bCurrOffset + DRM_DOMKEYXMR_PRIVKEYOBJ_MIN_LENGTH < f_cbData )
    {
        ChkDR( DRM_DOMKEYXMR_GetPrivkey( f_pbData, f_cbData, &bCurrOffset, &oXMRDomainPrivKey ) );
        ChkBOOL( bCurrOffset <= f_cbData, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
        if( fFirstRevision )
        {
            ChkBOOL( oXMRDomainPrivKey.dwRevision == oCert.Type.DomainInfo.dwRevision, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
            fFirstRevision = FALSE;
        }
        else
        {
            ChkBOOL( dwDomainRevisionPrev > oXMRDomainPrivKey.dwRevision, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
        }
        dwDomainRevisionPrev = oXMRDomainPrivKey.dwRevision;
    }

    /* Decrypt AES session keys to get 2 AES session keys { CI|CK } */
    ChkDR(DRM_BBX_ECC256_Decrypt( f_poBBXContext,
                                  (DRM_BYTE*) oXMRSessionKey.pbData,
                                  oXMRSessionKey.dwKeyLength,
                                  f_poECC256PubKey,
                                  &hDomainProtectKey ) );

    dr = DRM_DOMST_DeleteCert( f_poDomainStoreContext, &f_poDomainID->m_oAccountID );
    if (DRM_E_DOMAIN_STORE_DELETE_DATA == dr)
    {
        /* It is okay if we couldn't find the cert to delete, this may be the first time we
           have joined this domain.
           */
        dr = DRM_SUCCESS;
    }
    ChkDR( dr );

    ChkDR( DRM_DOMST_CreateDomainStorePassword( &f_poDomainID->m_oAccountID,
                                                f_poBBXContext,
                                                rgbPasswordSST ) );

    ChkDR( DRM_DOMST_AddCert( rgbPasswordSST,
                              f_poBBXContext->pOEMContext,
                              f_poDomainStoreContext,
                             &f_poDomainID->m_oAccountID,
                             &f_poDomainID->m_oServiceID,
                              PB_DASTR( f_pdastrCertificate ),
                              CB_DASTR( f_pdastrCertificate ) ) );

    bCurrOffset = bCurrOffsetSaved;
    while( bCurrOffset + DRM_DOMKEYXMR_PRIVKEYOBJ_MIN_LENGTH < f_cbData )
    {
        hDomainKey = OEM_CRYPTO_HANDLE_INVALID;
        ChkDR( DRM_DOMKEYXMR_GetPrivkey( f_pbData, f_cbData, &bCurrOffset, &oXMRDomainPrivKey ) );
        ChkBOOL( oXMRDomainPrivKey.wType == DRM_DOMKEYXMR_OBJTYPE_PRIVKEY,  DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
        ChkBOOL( oXMRDomainPrivKey.wKeyType == DRM_DOMKEYXMR_PRIVKEY_TYPE_ECCP256, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
        ChkBOOL( oXMRDomainPrivKey.dwKeyLength == ECC_P256_PRIVKEY_SIZE_IN_BYTES , DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
        ChkBOOL( oXMRDomainPrivKey.wEncryptionType == DRM_DOMKEYXMR_PRIVKEY_ENCTYPE_MIXED_AESECB, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
        ChkBOOL( oXMRDomainPrivKey.dwKeyLength == ECC_P256_PRIVKEY_SIZE_IN_BYTES, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );
        ChkBOOL( bCurrOffset <= f_cbData, DRM_E_DOMAIN_INVALID_DOMKEYXMR_DATA );

        f_poDomainID->m_dwRevision = oXMRDomainPrivKey.dwRevision;

        ChkDR( _Hal_UnwrapKey( f_poBBXContext,
                               OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT,
                               hDomainProtectKey,
                               oXMRDomainPrivKey.pbData,
                               ECC_P256_PRIVKEY_SIZE_IN_BYTES,
                               NULL,
                               0,
                               oXMRDomainPrivKey.pbData,
                               ECC_P256_PRIVKEY_SIZE_IN_BYTES,
                               &hDomainKey ) );

        ChkDR( _SymmetricWrapKey( f_poBBXContext,
                                  hDomainKey,
                                  SIZEOF( PRIVKEY_P256 ),
                                  (DRM_BYTE*) &oDomainKeyRecord.oDomainPrivKeyEncrypted ) );

        dr =   DRM_DOMST_DeleteKey( f_poDomainStoreContext,
                                   &f_poDomainID->m_oAccountID,
                                   f_poDomainID->m_dwRevision );
        if (DRM_E_DOMAIN_STORE_DELETE_DATA == dr)
        {
            /* It is okay if we couldn't find the key to delete, this may be the first time we
               have seen this revision value.
               */
            dr = DRM_SUCCESS;
        }
        ChkDR( dr );

        DRM_BYT_CopyBytes( &oDomainKeyRecord.oDevicePubKey,
                            0,
                            &f_poBBXContext->cachedBCertValues.DeviceKeyEncrypt.PublicKeyContext.PublicKey,
                            0,
                            SIZEOF( PUBKEY_P256 ) );

        oDomainKeyRecord.dwVersion = DOMAIN_KEY_RECORD_VERSION;

        ChkDR( DRM_DOMST_AddKey( f_poDomainStoreContext,
                                 &f_poDomainID->m_oAccountID,
                                 f_poDomainID->m_dwRevision,
                                 (DRM_BYTE *) &oDomainKeyRecord,
                                 SIZEOF( DRM_BB_DOMAIN_KEY_RECORD ) ) );

        DRM_BBX_SafeReleaseKey( f_poBBXContext, &hDomainKey );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    OEM_SECURE_ZERO_MEMORY( &oDomainKeyRecord, SIZEOF(DRM_BB_DOMAIN_KEY_RECORD));
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hDomainKey );
    DRM_BBX_SafeReleaseKey( f_poBBXContext, &hDomainProtectKey );
    return dr;
}


/*********************************************************************
**
** Function: DRM_BBX_SignWithCertKeys
**
** Synopsis:
**           Sign device cert with the appropriate keys for either P160 or P256 case.
**
** Arguments:
**  [f_pOEMContext]             -- A pointer to the OEM Context; may be NULL
**  [f_pCryptoCtx]              -- A pointer to the crypto context
**  [f_eKeyType]                -- Enum describing what key to
**                                           use for signing.
**  [f_pbPrivKey]                    TEMPORARY: Key to be used in P256 signing
**                                   can be NULL for P160
**  [f_cbData]                  -- Size of data to be signed
**  [f_pbData]                  -- Pointer data to be signed
**  [f_pcbSignature]            -- Returned size of signature
**  [f_pbSignature]             -- Signature created.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_SignWithCertKeys(
    __in_ecount( 1 )                DRM_BB_CONTEXT      *f_pBbxContext,
    __in                            DRM_DEVICE_KEY_TYPE  f_eKeyType,
    __in                            OEM_CRYPTO_HANDLE    f_hPrivKey,  /* Remove once embedded group keys are used */
    __in                            DRM_DWORD            f_cbData,
    __in_bcount( f_cbData ) const   DRM_BYTE            *f_pbData,
    __inout_ecount( 1 )             DRM_DWORD           *f_pcbSignature,
    __out_bcount( *f_pcbSignature ) DRM_BYTE            *f_pbSignature
)
{
    DRM_RESULT  dr              = DRM_SUCCESS;
    PRIVKEY     oSigningKeyP160 = {0};

    ChkArg( NULL != f_pbData );
    ChkArg( NULL != f_pcbSignature );
    ChkArg( NULL != f_pbSignature );
    ChkArg( NULL != f_pBbxContext );


    switch (f_eKeyType)
    {
        case DRM_DKT_WMDRMPD_FALLBACK:
        case DRM_DKT_WMDRMPD_GROUP:
            {
                DRM_DWORD cbSigningKey = SIZEOF( PRIVKEY );


                ChkDR( Oem_Device_GetKey( f_pBbxContext->pOEMContext,
                                          f_eKeyType,
                                          (DRM_BYTE *)&oSigningKeyP160,
                                         &cbSigningKey ) );


                ChkDR( DRM_PK_Sign( f_pBbxContext->CryptoContext.rgbCryptoContext,
                                eDRM_ECC_P160,
                                DRM_ECC160_PRIVATE_KEY_LEN,
                                ( DRM_BYTE * ) &oSigningKeyP160,
                                f_cbData,
                                f_pbData,
                                f_pcbSignature,
                                f_pbSignature ) );
            }
            break;
        case DRM_DKT_PLAYREADY_MODEL:
            {
                /* Remove once embedded group keys are used */
                ChkArg( OEM_CRYPTO_HANDLE_INVALID != f_hPrivKey );


                ChkDR( _Hal_SignWithPrivateKey( f_pBbxContext,
                                                f_pbData,
                                                f_cbData,
                                                f_pbSignature,
                                                f_pcbSignature,
                                                OEM_HAL_HASH_TYPE_SHA256,
                                                OEM_HAL_SIGNATURE_SCHEME_ECDSA_P256,
                                                f_hPrivKey ) );

            }
            break;
        default:

            ChkDR(DRM_E_INVALIDARG);
    }



ErrorExit:

    OEM_ECC_ZeroPrivateKey( &oSigningKeyP160, SIZEOF( oSigningKeyP160 ) );
    return dr;
}


/*********************************************************************
**
** Function: DRM_BBX_GenerateECCKeyPair
**
** Synopsis:
**           Generation of keypairs is an important operation from
**           robustness perspective - private key of is one
**           of the most important robustness assets. Blackbox.c file
**           and oem files are the source files that should be the
**           only ones to be edited to increase the protection of
**           robustness assets.
**           In the given implementation this function just calls
**           a plain one to generate a P256 ECC key pair.
**           It should be called whenever a P256 key pair for
**           encryption/decryption is generated.
**
** Arguments:
**  [f_pBbxContext]             -- A pointer to the black box context
**  [f_eKeyType]                -- the type of key to generate.
**  [f_pcbPublicKey]            -- Size of pubkey buffer
**  [f_pbPublicKey]             -- Pubkey buffer
**  [f_phKeyPair]               -- Returns a handle to the newly created key pair
**
**********************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_BBX_GenerateECCKeyPair(
    __inout                             DRM_BB_CONTEXT    *f_pBbxContext,
    __in                                OEM_HAL_KEY_TYPE   f_eKeyType,
    __inout_ecount( 1 )                 DRM_DWORD         *f_pcbPublicKey,
    __out_bcount( *f_pcbPublicKey )     DRM_BYTE          *f_pbPublicKey,
    __out_ecount( 1 )                   OEM_CRYPTO_HANDLE *f_phKeyPair )
{
    DRM_RESULT        dr       = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE hKeyPair = OEM_CRYPTO_HANDLE_INVALID;

    ChkPtr( f_pBbxContext != NULL );
    ChkPtr( f_pcbPublicKey != NULL );
    ChkPtr( f_phKeyPair );
    ChkArg( *f_pcbPublicKey >= SIZEOF( PUBKEY_P256 ) );

    ChkDR( _Hal_GenerateKey( f_pBbxContext,
                             f_eKeyType,
                             0,
                             &hKeyPair ) );

    /*
    ** Wrap the key pair with a NULL wrapping key to obtain the public
    ** portion of the key in the clear
    */
    ChkDR( _Hal_WrapKey( f_pBbxContext,
                         hKeyPair,
                         OEM_CRYPTO_HANDLE_INVALID,
                         f_pbPublicKey,
                         f_pcbPublicKey ) );

    *f_phKeyPair = hKeyPair;
    hKeyPair     = OEM_CRYPTO_HANDLE_INVALID;

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_pBbxContext, &hKeyPair );
    return dr;
}


/***********************************************************************************
** Function:    DRM_BBX_KF_EncryptKey
**
** Synopsis:    Encrypts data (a key) to be stored in the keyfile.
**
** Arguments:   [f_pBbxContext]   : Pointer to the blackbox context
**              [f_hKey]          : Handle of the key to be encrypted
**              [f_pEncryptedKey] : Structure to be populated with encrypted key
**                                  and metadata about encryption.
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_BUFFERTOOSMALL - if the buffer is NULL or too small
**
**
** Notes:       f_pEncryptedKey->pbEncryptedKey buffer to store encrypted key value
**              is already allocated by a caller.
***********************************************************************************/
DRM_RESULT DRM_API DRM_CALL DRM_BBX_KF_EncryptKey (
    __inout           DRM_BB_CONTEXT    *f_pBbxContext,
    __in              OEM_CRYPTO_HANDLE  f_hKey,
    __out_ecount( 1 ) DRM_ENCRYPTED_KEY *f_pEncryptedKey )
{
    DRM_RESULT        dr             = DRM_SUCCESS;
    OEM_CRYPTO_HANDLE hKeyFileKey    = OEM_CRYPTO_HANDLE_INVALID;
    DRM_DWORD         cbEncryptedKey = SIZEOF( DRM_ENCRYPTED_KEY );
    DRM_GUID          emptyGuid      = EMPTY_DRM_GUID;

    ChkArg( f_pBbxContext   != NULL );
    ChkArg( f_hKey          != OEM_CRYPTO_HANDLE_INVALID );
    ChkArg( f_pEncryptedKey != NULL );

    /*
    ** Use the default keyfile key
    */
    f_pEncryptedKey->guidKeypairDecryptionKeyID = emptyGuid;

    ChkDR( _Hal_GetPreloadedKey( f_pBbxContext,
                                 OEM_HAL_KEY_TYPE_AES_KEYFILE,
                                 &f_pEncryptedKey->guidKeypairDecryptionKeyID,
                                 &hKeyFileKey ) );

    ChkDR( _Hal_WrapKey( f_pBbxContext,
                         f_hKey,
                         hKeyFileKey,
                         ( DRM_BYTE* )f_pEncryptedKey,
                         &cbEncryptedKey ) );

ErrorExit:
    DRM_BBX_SafeReleaseKey( f_pBbxContext, &hKeyFileKey );
    return dr;
}

/***********************************************************************************
** Function:    DRM_BBX_SafeReleaseKey
**
** Synopsis:    Release a key handle if it is valid, and set it to NULL.
**
** Arguments:   [f_phKey]   : Pointer to the key handle to try releasing
**
** Notes:
**
***********************************************************************************/
DRM_API DRM_VOID DRM_CALL DRM_BBX_SafeReleaseKey(
    __inout_ecount( 1 ) DRM_BB_CONTEXT    *f_pBbxContext,
    __inout_ecount( 1 ) OEM_CRYPTO_HANDLE *f_phKey )
{
    if( f_pBbxContext
     && f_phKey
     && *f_phKey != OEM_CRYPTO_HANDLE_INVALID )
    {
        DRM_RESULT dr = _Hal_ReleaseKey( f_pBbxContext,
                                         *f_phKey );

        DRMASSERT( DRM_SUCCEEDED( dr ) );
        UNREFERENCED_PARAMETER( dr ); /* for release builds */

        *f_phKey = OEM_CRYPTO_HANDLE_INVALID;
    }
}

/***********************************************************************************
** Function:    DRM_BBX_SafeDestroySampleEncryptionContext
**
** Synopsis:    Destroy a sample encryption context if it is valid, and set it to NULL.
**
** Arguments:   [f_phEncryptionContext] : Pointer to the context to try destroying,
**                  it gets updated to OEM_ENCRYPTION_HANDLE_INVALID
**
** Notes:
**
***********************************************************************************/
DRM_API DRM_VOID DRM_CALL DRM_BBX_SafeDestroySampleEncryptionContext(
    __inout_ecount( 1 ) OEM_ENCRYPTION_HANDLE *f_phEncryptionContext )
{
    if( f_phEncryptionContext
     && *f_phEncryptionContext != OEM_ENCRYPTION_HANDLE_INVALID )
    {
        DRM_RESULT dr = Oem_Hal_DestroySampleEncryptionContext( *f_phEncryptionContext );

        DRMASSERT( DRM_SUCCEEDED( dr ) );
        UNREFERENCED_PARAMETER( dr ); /* for release builds */

        *f_phEncryptionContext = OEM_ENCRYPTION_HANDLE_INVALID;
    }
}


/*********************************************************************
**
** Function:    DRM_BBX_GetPreloadedKey
**
** Synopsis:    Return the index of a preloaded key with the specified key ID.
**
** Arguments:
**
** [f_pBbxContext]:   The blackbox context.
** [f_eType]:         the type of the key to return.
** [f_pguidKeyId]:    The unique ID of the preloaded key.
** [f_phKey]:         The handle of the preloaded key.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_INVALIDARG if f_eType is invalid.
**              DRM_E_POINTER if f_pbKeyId or f_phKey are NULL.
**
** Notes:      None.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_GetPreloadedKey(
    __inout_ecount( 1 )    DRM_BB_CONTEXT    *f_pBbxContext,
    __in                   OEM_HAL_KEY_TYPE   f_eType,
    __in_ecount( 1 ) const DRM_GUID          *f_pguidKeyId,
    __out_ecount( 1 )      OEM_CRYPTO_HANDLE *f_phKey )
{
    return _Hal_GetPreloadedKey( f_pBbxContext,
                                 f_eType,
                                 f_pguidKeyId,
                                 f_phKey );
}


/*********************************************************************
**
** Function:    DRM_BBX_UnwrapKey
**
** Synopsis:    This function unwraps a key, wrapped using another key,
**              and loads it into a key register.
**
** Arguments:
**
** [f_pBbxContext]:      The blackbox context.
** [f_eKeyType]:         The type of key to be unwrapped.
** [f_hWrappingKey]:     The handle of the wrapping key.
** [f_pbWrappedKeyData]: A pointer to the buffer containing the data required
**                       to unpack the wrapped key (this is key dependent).
** [f_cbWrappedKeyData]: The length (in bytes) of the wrappedKey data.
** [f_pbParameterData]:  Parameter data associated with the particular unwrap type.
** [f_cbParameterData]:  Size of parameter data.
** [f_pbCacheKey]:       Cache key associated with the key handle.
** [f_cbCacheKey]:       Size of f_pbCacheKey in bytes.
** [f_phKey]:            The handle of the unwrapped key.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_HANDLE if f_hWrappingKey is invalid.
**              DRM_E_INVALIDARG if f_eKeyType is invalid.
**              DRM_E_POINTER if f_pbWrappedKeyData or f_phKey are NULL.
**              DRM_E_OEMHAL_CRYPTO_NOT_INITIALIZED
**              DRM_E_OEMHAL_CRYPTO_UNSUPPORTED_KEY_TYPE
**              DRM_E_OEMHAL_CRYPTO_UNSUPPORTED_KEY_LENGTH
**              DRM_E_OEMHAL_CRYPTO_UNSUPPORTED_KEY_WRAPPING_FORMAT
**
** Notes:      after this function is successfully called, a call must be made to
**             _Hal_ReleaseKey to free resources associated with the key.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_UnwrapKey(
    __inout                                 DRM_BB_CONTEXT     *f_pBbxContext,
    __in                                    OEM_HAL_KEY_TYPE    f_eKeyType,
    __in                                    OEM_CRYPTO_HANDLE   f_hWrappingKey,
    __in_bcount( f_cbWrappedKeyData ) const DRM_BYTE           *f_pbWrappedKeyData,
    __in                                    DRM_DWORD           f_cbWrappedKeyData,
    __in_bcount( f_cbParameterData )  const DRM_BYTE           *f_pbParameterData,
    __in                                    DRM_DWORD           f_cbParameterData,
    __in_bcount( f_cbCacheKey )       const DRM_BYTE           *f_pbCacheKey,
    __in                              const DRM_DWORD           f_cbCacheKey,
    __out_ecount( 1 )                       OEM_CRYPTO_HANDLE  *f_phKey)
{
    return _Hal_UnwrapKey( f_pBbxContext,
                           f_eKeyType,
                           f_hWrappingKey,
                           f_pbWrappedKeyData,
                           f_cbWrappedKeyData,
                           f_pbParameterData,
                           f_cbParameterData,
                           f_pbCacheKey,
                           f_cbCacheKey,
                           f_phKey );
}


/*********************************************************************
**
** Function:    DRM_BBX_InitPublicKeyContext
**
** Synopsis:    Initialize a public key context, setting flags to
**              determine whether it needs a key handle.
**
** Arguments:
**
** [f_pBbxContext]:      The blackbox context.
** [f_pKeyContext]:      The public key context.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_INVALIDARG if the supplied data isn't set to zero.
**              DRM_E_POINTER if either argument is NULL.
**
** Notes:      after this function is successfully called, a call must be made to
**             DRM_BBX_CleanupPublicKeyContext or DRM_BBX_TransferPublicKeyContext
**             to free/transfer ownership of resources associated with the context.
**
**********************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Prefast Noise: DRM_BB_CONTEXT should never be const")
DRM_API DRM_RESULT DRM_CALL DRM_BBX_InitPublicKeyContext(
    __inout_ecount(1) DRM_BB_CONTEXT       *f_pBbxContext,
    __out_ecount(1) DRM_PUBLIC_KEY_CONTEXT *f_pKeyContext )
PREFAST_POP /* __WARNING_NONCONST_PARAM_25004 */
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkPtr( f_pBbxContext );
    ChkPtr( f_pKeyContext );

    OEM_SECURE_ZERO_MEMORY( f_pKeyContext,
                            SIZEOF( DRM_PUBLIC_KEY_CONTEXT ) );

    f_pKeyContext->hPublicKey = OEM_CRYPTO_HANDLE_INVALID;

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:    DRM_BBX_CleanupPublicKeyContext
**
** Synopsis:    Cleans up a public key context, releasing any resources
**              assigned to the context.
**
** Arguments:
**
** [f_pBbxContext]:      The blackbox context.
** [f_pKeyContext]:      The public key context.
**
** Notes:      must be called after DRM_BBX_InitPublicKeyContext.
**
**********************************************************************/
DRM_API DRM_VOID DRM_CALL DRM_BBX_CleanupPublicKeyContext(
    __inout_ecount(1) DRM_BB_CONTEXT       *f_pBbxContext,
    __out_ecount(1) DRM_PUBLIC_KEY_CONTEXT *f_pKeyContext )
{
    DRMASSERT( f_pKeyContext != NULL );

    if( f_pKeyContext != NULL )
    {
        if( f_pKeyContext->hPublicKey != OEM_CRYPTO_HANDLE_INVALID )
        {
            DRMASSERT( f_pBbxContext != NULL );

            DRM_BBX_SafeReleaseKey( f_pBbxContext, &f_pKeyContext->hPublicKey );
        }

        OEM_SECURE_ZERO_MEMORY( f_pKeyContext,
                                SIZEOF( DRM_PUBLIC_KEY_CONTEXT ) );
    }
}


/*********************************************************************
**
** Function:    DRM_BBX_TransferPublicKeyContext
**
** Synopsis:    Transfers ownership of resources from one key context
**              to another, clearing up the first key context so that
**              it is no longer valid. After this is called,
**              DRM_BBX_CleanupPublicKeyContext must be called on the
**              destination key context. As ownership is transferred,
**              DRM_BBX_CleanupPublicKeyContext should not be called on
**              the source key context.
**
** Arguments:
**
** [f_pBbxContext]:            The blackbox context.
** [f_pDestinationKeyContext]: The key context to transfer to
** [f_pSourceKeyContext]:      The key context to transfer from
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_INVALIDARG if the supplied data isn't set to zero.
**              DRM_E_POINTER if either argument is NULL.
**
** Notes:
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_TransferPublicKeyContext(
    __inout_ecount(1) DRM_BB_CONTEXT         *f_pBbxContext,
    __out_ecount(1)   DRM_PUBLIC_KEY_CONTEXT *f_pDestinationKeyContext,
    __inout_ecount(1) DRM_PUBLIC_KEY_CONTEXT *f_pSourceKeyContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkPtr( f_pBbxContext );
    ChkPtr( f_pDestinationKeyContext );
    ChkPtr( f_pSourceKeyContext );

    DRM_BBX_CleanupPublicKeyContext( f_pBbxContext,
                                     f_pDestinationKeyContext );

    OEM_SECURE_MEMCPY( f_pDestinationKeyContext,
                       f_pSourceKeyContext,
                       SIZEOF( DRM_PUBLIC_KEY_CONTEXT ) );

    OEM_SECURE_ZERO_MEMORY( f_pSourceKeyContext,
                            SIZEOF( DRM_PUBLIC_KEY_CONTEXT ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:    DRM_BBX_DuplicatePublicKeyContext
**
** Synopsis:    Duplicates ownership of resources from one key context
**              to another, incrementing the reference count on any
**              reference counted resources. After this is called,
**              DRM_BBX_CleanupPublicKeyContext must be called on the
**              destination key context. As ownership is duplicated,
**              DRM_BBX_CleanupPublicKeyContext should still also be
**              called on the source key context.
**
** Arguments:
**
** [f_pBbxContext]:            The blackbox context.
** [f_pDestinationKeyContext]: The key context to transfer to
** [f_pSourceKeyContext]:      The key context to transfer from
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_INVALIDARG if the supplied data isn't set to zero.
**              DRM_E_POINTER if either argument is NULL.
**
** Notes:
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_BBX_DuplicatePublicKeyContext(
    __inout_ecount(1) DRM_BB_CONTEXT         *f_pBbxContext,
    __out_ecount(1)   DRM_PUBLIC_KEY_CONTEXT *f_pDestinationKeyContext,
    __inout_ecount(1) DRM_PUBLIC_KEY_CONTEXT *f_pSourceKeyContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkPtr( f_pBbxContext );
    ChkPtr( f_pDestinationKeyContext );
    ChkPtr( f_pSourceKeyContext );

    DRM_BBX_CleanupPublicKeyContext( f_pBbxContext,
                                     f_pDestinationKeyContext );

    OEM_SECURE_MEMCPY( f_pDestinationKeyContext,
                       f_pSourceKeyContext,
                       SIZEOF( DRM_PUBLIC_KEY_CONTEXT ) );

    if( f_pSourceKeyContext->hPublicKey != OEM_CRYPTO_HANDLE_INVALID )
    {
        ChkDR( _Hal_DuplicateKey( f_pBbxContext,
                                  f_pSourceKeyContext->hPublicKey,
                                  &f_pDestinationKeyContext->hPublicKey ) );
    }

ErrorExit:
    return dr;
}


/*********************************************************************************
** Function:    _InitPropertiesCache
**
** Synopsis:    Initialize a properties cache, setting up flags to determine
**              whether key handles are required for the public key.
**
** Arguments:   [f_pCachedValues]    : pointer to the structure to store cached data from the cert.
**              [f_pBlackboxContext] : pointer to an initialized blackbox context
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG
**
** Note:
**
***********************************************************************************/
static DRMFORCEINLINE DRM_RESULT _InitPropertiesCache(
   __inout           DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCachedValues,
   __inout_ecount(1) DRM_BB_CONTEXT                       *f_pBlackboxContext ) DRM_ALWAYS_INLINE_ATTRIBUTE;
static DRMFORCEINLINE DRM_RESULT _InitPropertiesCache(
   __inout           DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCachedValues,
   __inout_ecount(1) DRM_BB_CONTEXT                       *f_pBlackboxContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pCachedValues    != NULL );
    ChkArg( f_pBlackboxContext != NULL );

    OEM_SECURE_ZERO_MEMORY( f_pCachedValues,
                            SIZEOF( DRM_BINARY_DEVICE_CERT_CACHED_VALUES ) );

    /*
    ** Initialize each of the public keys
    */
    ChkDR( DRM_BBX_InitPublicKeyContext( f_pBlackboxContext,
                                         &f_pCachedValues->DeviceKeySign.PublicKeyContext ) );
    ChkDR( DRM_BBX_InitPublicKeyContext( f_pBlackboxContext,
                                         &f_pCachedValues->DeviceKeyEncrypt.PublicKeyContext ) );
    ChkDR( DRM_BBX_InitPublicKeyContext( f_pBlackboxContext,
                                         &f_pCachedValues->DeviceKeyPrndEncrypt.PublicKeyContext ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:    _InitializeHandleCaching
**
** Synopsis:    Initialize the process of adding key handles to a
**              DRM_BINARY_DEVICE_CERT_CACHED_VALUES struct
**
** Arguments:
**
** [f_pBbxContext]:      : The blackbox context.
** [f_pCache]            : The cache
** [f_pbCertData]        : The cert chain buffer
** [f_cbCertData]        : Length of the cert chain buffer
** [f_dwChainLength]     : The length of the cert chain (num certs)
** [f_bOffset]           : The offset of the first certificate in f_pbCertData
** [f_phCertChainPubKey] : Populated with the signing key of the cert chain
**
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_INVALIDARG if the parameters are incorrect
**
** Notes:
**
**********************************************************************/
static DRM_RESULT _InitializeHandleCaching(
    __inout_ecount( 1 )             DRM_BB_CONTEXT                       *f_pBbxContext,
    __inout_ecount( 1 )             DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCache,
    __in_bcount(f_cbCertData) const DRM_BYTE                             *f_pbCertData,
    __in                      const DRM_DWORD                             f_cbCertData,
    __in                            DRM_DWORD                             f_dwChainLength,
    __in                            DRM_DWORD                             f_bOffset,
    __out_ecount( 1 )               OEM_CRYPTO_HANDLE                    *f_phCertChainPubKey )
{
    DRM_RESULT dr = DRM_E_LOGICERR;

    ChkArg( f_pBbxContext       != NULL );
    ChkArg( f_pCache            != NULL );
    ChkArg( f_pbCertData        != NULL );
    ChkArg( f_phCertChainPubKey != NULL );

    /*
    ** Free any previously cached resources
    */
    _CleanupCache( f_pBbxContext,
                   f_pCache );

    /*
    ** Prepare the cache for populating
    */
    ChkDR( _InitPropertiesCache( f_pCache,
                                 f_pBbxContext ) );

    ChkDR( _ResolveSigningChainKey( f_pBbxContext,
                                    f_pbCertData,
                                    f_cbCertData,
                                    f_dwChainLength,
                                    1, /* resolve the leaf certificate parent signing key */
                                    DRM_BCERT_KEYUSAGE_UNKNOWN,
                                    f_bOffset,
                                    f_phCertChainPubKey ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:    _UnwrapPublicKeyHandle
**
** Synopsis:    This function unwraps a public key, ensuring it is validated
**              via its certificate chain.
**
** Arguments:
**
** [f_pBbxContext]:      The blackbox context.
** [f_hWrappingKey]:     The handle of the signing key.
** [f_pbCertificate]:    A pointer to the buffer containing the certificate
** [f_cbCertificate]:    The length (in bytes) of the certificate data.
** [f_iKeyInCert]:       the index of the key in the certificate
** [f_pKeyContext]:      the key context to add the handle to.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_HANDLE if f_hWrappingKey is invalid.
**              DRM_E_INVALIDARG if f_eKeyType is invalid.
**              DRM_E_POINTER if f_pbWrappedKeyData or f_phKey are NULL.
**              DRM_E_OEMHAL_CRYPTO_NOT_INITIALIZED
**              DRM_E_OEMHAL_CRYPTO_UNSUPPORTED_KEY_TYPE
**              DRM_E_OEMHAL_CRYPTO_UNSUPPORTED_KEY_LENGTH
**              DRM_E_OEMHAL_CRYPTO_UNSUPPORTED_KEY_WRAPPING_FORMAT
**
** Notes:      after this function is successfully called, a call must be made to
**             _Hal_ReleaseKey to free resources associated with the key.
**
**********************************************************************/
static DRM_RESULT _UnwrapPublicKeyHandle(
    __inout_ecount( 1 )                     DRM_BB_CONTEXT         *f_pBbxContext,
    __in                                    OEM_CRYPTO_HANDLE       f_hWrappingKey,
    __in_bcount( f_cbWrappedKeyData ) const DRM_BYTE               *f_pbCertificate,
    __in                                    DRM_DWORD               f_cbCertificate,
    __in                                    DRM_DWORD               f_iKeyInCert,
    __out_ecount( 1 )                       DRM_PUBLIC_KEY_CONTEXT *f_pKeyContext )
{
    DRM_RESULT dr = DRM_E_LOGICERR;

    ChkArg( f_pBbxContext   != NULL );
    ChkArg( f_pbCertificate != NULL );
    ChkArg( f_pKeyContext   != NULL );
    ChkArg( f_hWrappingKey  != OEM_CRYPTO_HANDLE_INVALID );

    ChkDR( DRM_BBX_UnwrapKey( f_pBbxContext,
                              OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC,
                              f_hWrappingKey,
                              f_pbCertificate,
                              f_cbCertificate,
                              ( const DRM_BYTE* )&f_iKeyInCert,
                              SIZEOF( f_iKeyInCert ),
                              f_pKeyContext->PublicKey.m_rgbPubkey,
                              SIZEOF( f_pKeyContext->PublicKey.m_rgbPubkey ),
                              &f_pKeyContext->hPublicKey ) );

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function:    _CompleteHandleCaching
**
** Synopsis:    Cleanup after the process of adding key handles to a
**              DRM_BINARY_DEVICE_CERT_CACHED_VALUES struct
**
** Arguments:
**
** [f_pBbxContext]:      : The blackbox context.
** [f_pCache]            : The cache
** [f_drCachingResult]   : The result of the cache operation
** [f_phCertChainPubKey] : Populated with the signing key of the cert chain
**
**
** Notes:
**        After calling, f_phCertChainPubKey will be freed, and if
**        f_drCachingResult is an error code, the cache handles will also be freed
**
**********************************************************************/
static DRM_VOID _CompleteHandleCaching(
    __inout_ecount_opt(1)           DRM_BB_CONTEXT                       *f_pBbxContext,
    __inout_ecount_opt(1)           DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCache,
    __in                            DRM_RESULT                            f_drCachingResult,
    __inout_ecount_opt(1)           OEM_CRYPTO_HANDLE                    *f_phCertChainPubKey )
{
    /*
    ** This may be called even when some parameters are NULL, so don't enforce them being populated
    */

    if( DRM_FAILED( f_drCachingResult ) )
    {
        if( f_pBbxContext != NULL
         && f_pCache      != NULL )
        {
            _CleanupCache( f_pBbxContext,
                           f_pCache );
        }
    }

    DRM_BBX_SafeReleaseKey( f_pBbxContext, f_phCertChainPubKey );
}


/*********************************************************************
**
** Function:    _CleanupCache
**
** Synopsis:    Cleanup the provided handle cache, releasing any resources
**
** Arguments:
**
** [f_pBbxContext]:      : The blackbox context.
** [f_pCache]            : The cache
**
**
** Notes:
**
**********************************************************************/
static DRM_VOID _CleanupCache(
    __inout_ecount_opt(1) DRM_BB_CONTEXT                       *f_pBbxContext,
    __inout_ecount_opt(1) DRM_BINARY_DEVICE_CERT_CACHED_VALUES *f_pCache )
{
    DRMASSERT( f_pCache    != NULL );
    DRMASSERT( f_pBbxContext != NULL );

    /*
    ** Assume the parameters are correct; but make best effort to cleanup even if
    ** they aren't
    */
    if( f_pCache    != NULL )
    {
        if( f_pBbxContext != NULL )
        {
            /*
            ** Clean up each of the public keys
            */
            DRM_BBX_CleanupPublicKeyContext( f_pBbxContext,
                                             &f_pCache->DeviceKeySign.PublicKeyContext );
            DRM_BBX_CleanupPublicKeyContext( f_pBbxContext,
                                             &f_pCache->DeviceKeyEncrypt.PublicKeyContext );
            DRM_BBX_CleanupPublicKeyContext( f_pBbxContext,
                                             &f_pCache->DeviceKeyPrndEncrypt.PublicKeyContext );
        }

        OEM_SECURE_ZERO_MEMORY( f_pCache,
                                SIZEOF( DRM_BINARY_DEVICE_CERT_CACHED_VALUES ) );
    }
}


EXIT_PK_NAMESPACE_CODE;

