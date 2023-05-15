/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/******************************************************************************
**
** This file implements the interface in oemhal.h (refer to comments in oemhal.h).
** It implements the secure-core side of the interface between the DRM blackbox and the secure crypto core.
** OEMs should not need to change this file in any way (except to get it to compile properly).
** If the device supports a secure crypto core, this file should compile ONLY for that core.
**
*******************************************************************************
*/

#define DRM_BUILDING_OEMHAL_C

#include <oemhal.h>
#include <oemkeyfile.h>
#include <oemsparsexmrparser.h>
#include <oemhaldevcerts.h>
#include <oemhaloemimpl.h>
#include <oemderivedkeys.h>

#include <drmmathsafe.h>
#include <drmconstants.h>

#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*
** We block rebinding to any certificate that does not have the PRND encrypt key usage and the PRND Receiver feature.
** However, we can only block this when secure HAL is enabled because the symmetric optimizations feature also
** requires that a license be resigned (specifically: after a symmetrically optimized content key is added to it).
*/
#if DRM_SUPPORT_SECUREOEMHAL
#define OEM_HAL_BCERT_ADDITIONAL_KEYUSAGE_REQUIRED_FOR_REBINDING   DRM_BCERT_KEYUSAGE_PRND_ENCRYPT_KEY
#else   /* DRM_SUPPORT_SECUREOEMHAL */
#define OEM_HAL_BCERT_ADDITIONAL_KEYUSAGE_REQUIRED_FOR_REBINDING   DRM_BCERT_KEYUSAGE_ENCRYPT_KEY
#endif  /* DRM_SUPPORT_SECUREOEMHAL */

#if DRM_SUPPORT_SECUREOEMHAL

/*
** When using the secure HAL, we require the passed in crypto context to be NULL and allocate our
** own to avoid leakage of key material through a structure to which the caller may have access.
*/

#define DECLARE_LOCAL_CRYPTO_CONTEXT()  DRM_CRYPTO_CONTEXT  *pCryptoContext = NULL
#define ALLOC_LOCAL_CRYPTO_CONTEXT()    do {                                                            \
    ChkArg( f_pCryptoContext == NULL );                                                                 \
    ChkMem( pCryptoContext = ( DRM_CRYPTO_CONTEXT* )Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );     \
	OEM_SECURE_ZERO_MEMORY( pCryptoContext, SIZEOF( *pCryptoContext ) );	\
} while(FALSE)
#define FREE_LOCAL_CRYPTO_CONTEXT() do {                                        \
    if( pCryptoContext != NULL )                                                \
    {                                                                           \
        OEM_SECURE_ZERO_MEMORY( pCryptoContext, SIZEOF( *pCryptoContext ) );    \
    }                                                                           \
    SAFE_OEM_FREE( pCryptoContext );                                            \
} while(FALSE)
#define USE_LOCAL_CRYPTO_CONTEXT        pCryptoContext

#else   /* DRM_SUPPORT_SECUREOEMHAL */

/* When NOT using the secure HAL, we use the passed in crypto context to save memory. */

#define DECLARE_LOCAL_CRYPTO_CONTEXT()
#define ALLOC_LOCAL_CRYPTO_CONTEXT()    ChkPtr( f_pCryptoContext )
#define FREE_LOCAL_CRYPTO_CONTEXT() do {                                            \
    if( f_pCryptoContext != NULL )                                                  \
    {                                                                               \
        OEM_SECURE_ZERO_MEMORY( f_pCryptoContext, SIZEOF( *f_pCryptoContext ) );    \
    }                                                                               \
} while(FALSE)
#define USE_LOCAL_CRYPTO_CONTEXT        f_pCryptoContext

#endif  /* DRM_SUPPORT_SECUREOEMHAL */

static const PUBKEY_P256* _GetRootPublicKey()
{
    return ( const PUBKEY_P256* )&g_rgbMSPlayReadyRootIssuerPubKey;
}

/*
** Functions defining whether OEM HAL device certificate validation is supported
**
** OEM_HAL_IsHalDevCertValidationSupported should return TRUE if the OEM HAL requires
** full cert-chain validation of device certificates, including Revocation Info validation,
** below the OEM_HAL API.
*/
DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL OEM_HAL_IsHalDevCertValidationSupported(DRM_VOID)
{
    return TRUE;
}

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL OEM_HAL_IsHalDevCertValidationUnsupported(DRM_VOID)
{
    return FALSE;
}

/*********************************************************************
**
** Function imlementations
**
**********************************************************************/

/*********************************************************************
**
** Function: _InitializeUnwrapContext
**
** Synopsis: Initialize the unwrap context, verifying that the
**           unwrap arguments specify a valid combination
**
** Arguments:
**
** [f_eKeyType]        : The type of key to be unwrapped.
** [f_indexKey]        : The index of the register in which to place the key.
** [f_eWrappingKeyType]: The type of the wrapping key.
** [f_indexWrappingKey]: The index of the wrapping key.
** [f_cbWrappedKeyData]: The size of the supplied key data.
** [f_pbParameterData] : Pointer to any additional parameter data.
** [f_cbParameterData] : The size of any parameter data.
** [f_pUnwrapContext]  : The unwrap context to initialize
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_INVALIDARG if a parameter is invalid.
**             DRM_E_OEMHAL_INVALID_PARAMETER
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE
**             DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT
**
** Notes:      None.
**
**********************************************************************/
static DRM_RESULT _InitializeUnwrapContext(
    __in                                       OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                                       OEM_HAL_KEY_REGISTER_INDEX  f_indexKey,
    __in                                       OEM_HAL_KEY_TYPE            f_eWrappingKeyType,
    __in                                       OEM_HAL_KEY_REGISTER_INDEX  f_indexWrappingKey,
    __in                                       DRM_DWORD                   f_cbWrappedKeyData,
    __in_bcount_opt( f_cbParameterData ) const DRM_BYTE                   *f_pbParameterData,
    __in                                       DRM_DWORD                   f_cbParameterData,
    __out_ecount( 1 )                          OEM_HAL_UNWRAP_CONTEXT     *f_pUnwrapContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkPtr( f_pUnwrapContext );

    OEM_SECURE_ZERO_MEMORY( f_pUnwrapContext, SIZEOF( OEM_HAL_UNWRAP_CONTEXT ) );

    /*
    ** Zero the register that is to be the target key
    */
    Oem_HalOemImpl_ZeroKeyRegister( f_eKeyType, f_indexKey );

    /*
    ** This switch statement contains all the valid combinations of key/unwrapping key pairs
    */
    switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_AES_KEYFILE:
            /*
            ** Load device content key from key file.  Device private content encryption key decrypted from keyfile using aes keyfile key.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
            /*
            ** Load domain key from hash data store. Key is encrypted with a symmetric key derived from device ECC key.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        case OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT:
            /*
            ** Unwrap an ECC256 private domain key in two halves using the {CI|CK} Domain Protect key pair to do an AES-ECB decrypt on one half each.  Scenario: Process Domain Join Response.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_AES_KEYFILE:
            /*
            ** Load device message key from key file.  Device private message encryption key decrypted from keyfile using aes keyfile key.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_ECC_P256_SIGN:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_AES_KEYFILE:
            /*
            ** Load device signing key from key file.  Device private signing key decrypted from keyfile using aes keyfile key.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
            /*
            ** Load domain-key-protecting transient key from domain join response.  The protecting key is encrypted with the device content key.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
            /*
            ** Root license content key decrypt with my ecc priv content key.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:
            /*
            ** Root license content key decrypt with domain ecc priv key.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT:
            /*
            ** Local root license key caching to avoid running out of registers.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
            /*
            ** Simple license content key decrypted with my ecc privkey.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:
            /*
            ** Simple license content key decrypted with domain ecc privkey.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:
            /*
            ** Leaf license content key decrypted with root license content key.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        case OEM_HAL_KEY_TYPE_AES_SLK:
            /*
            ** Local simple license key decrypted with SLK.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT:
            /*
            ** Local leaf/simple license key caching to avoid running out of registers.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT:
            /*
            ** MI/MK decrypted with my ecc private message encrypt key.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_LEGACY:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
            /*
            ** Cocktail content key decrypt with my ecc priv content key.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:
            /*
            ** Cocktail content key decrypt with domain ecc priv key.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:
            /*
            ** Cocktail content key decrypt with root license key.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_AES_KEYFILE:
            /*
            ** Secure store key read from key file decrypted with aes keyfile key.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC:
            /*
            ** Load a public key into a register.
            ** Will verify that the target key's certificate is valid and not revoked.
            ** The wrapping key is the parent certificate's pubkey (or root pubkey).
            ** Will verify that the wrapping key is allowed to sign the target key.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, NULL, 0 ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_AES_SLK:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
            /*
            ** SLK decrypt with a symmetric key derived from my ecc priv content key.
            */
            ChkDR( Oem_HalOemImpl_InitializeUnwrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pUnwrapContext, f_pbParameterData, f_cbParameterData ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    default:
        ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _PublicKeyUsageContains
**
** Synopsis:    Returns TRUE if the usage field contains a particular value.
**
** Arguments:   [f_dwKeyUsageField] : The key usage field
**              [f_dwUsageRequired] : The key usage to check against
**                                    NOTE: f_dwUsageRequired *MUST* be only a single key usage.
**
** Returns:     TRUE      - Usage field contains the specified usage
**              FALSE     - Usage field doesn't contain the specified usage
**
** Notes:       None.
**
******************************************************************************/
static DRM_FRE_INLINE DRM_BOOL _PublicKeyUsageContains(
    __in DRM_DWORD f_dwKeyUsageField,
    __in DRM_DWORD f_dwUsageRequired )
{
    if( (DRM_DWORD)( BCERT_KEYUSAGE_BIT( f_dwUsageRequired ) & f_dwKeyUsageField ) == (DRM_DWORD)BCERT_KEYUSAGE_BIT( f_dwUsageRequired ) )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static DRM_FRE_INLINE DRM_RESULT _ValidatePublicKeyForWrapping(
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexWrappingKey,
    __in                                            DRM_DWORD                            f_dwKeyUsageRequired,
    __in                                            DRM_DWORD                            f_dwFullCertKeyUsageRequired ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_RESULT _ValidatePublicKeyForWrapping(
    __in                                            OEM_HAL_KEY_REGISTER_INDEX           f_indexWrappingKey,
    __in                                            DRM_DWORD                            f_dwKeyUsageRequired,
    __in                                            DRM_DWORD                            f_dwFullCertKeyUsageRequired )
{
    DRM_RESULT  dr                          = DRM_SUCCESS;
    DRM_DWORD   dwKeyUsageBitmap            = 0;
    DRM_DWORD   dwFullCertKeyUsageBitmap    = 0;

    ChkDR( Oem_HalOemImpl_GetEcc256PublicKeyUsageBitmap( f_indexWrappingKey, &dwKeyUsageBitmap, &dwFullCertKeyUsageBitmap ) );

    /*
    ** Only allow public keys with the specified usage bit(s) to be used for wrapping
    ** Only allow certificates that contain key(s) that include the specified additional usage bit(s)
    */
    ChkBOOL( _PublicKeyUsageContains( dwKeyUsageBitmap,                            f_dwKeyUsageRequired ),         DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    ChkBOOL( _PublicKeyUsageContains( dwKeyUsageBitmap | dwFullCertKeyUsageBitmap, f_dwFullCertKeyUsageRequired ), DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );

    /*
    ** Make sure that the wrapping key is valid
    */
    if( OEM_HAL_IsHalDevCertValidationSupported() )
    {
        ChkDR( _CheckValidPublicKey( f_indexWrappingKey ) );
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _InitializeWrapContext
**
** Synopsis: Initialize the wrap context, verifying that the
**           wrap arguments specify a valid combination
**
** Arguments:
**
** [f_eKeyType]        : The type of key to be wrapped.
** [f_indexKey]        : The index of the register containing the key.
** [f_eWrappingKeyType]: The type of the wrapping key.
** [f_indexWrappingKey]: The index of the wrapping key.
** [f_pWrapContext]    : The wrap context to initialize
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_INVALIDARG if a parameter is invalid.
**             DRM_E_OEMHAL_INVALID_PARAMETER
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE
**             DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT
**
** Notes:      None.
**
**********************************************************************/
static DRM_RESULT _InitializeWrapContext(
    __in              OEM_HAL_KEY_TYPE            f_eKeyType,
    __in              OEM_HAL_KEY_REGISTER_INDEX  f_indexKey,
    __in              OEM_HAL_KEY_TYPE            f_eWrappingKeyType,
    __in              OEM_HAL_KEY_REGISTER_INDEX  f_indexWrappingKey,
    __out_ecount( 1 ) OEM_HAL_WRAP_CONTEXT       *f_pWrapContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkPtr( f_pWrapContext );

    OEM_SECURE_ZERO_MEMORY( f_pWrapContext, SIZEOF( OEM_HAL_WRAP_CONTEXT ) );

    /*
    ** This switch statement contains all the valid combinations of key/wrapping key pairs
    */
    switch( f_eKeyType )
    {
    case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_AES_KEYFILE:
            /*
            ** Current or historical ECC device content privkey written to keyfile.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        case OEM_HAL_KEY_TYPE_INVALID:
            /*
            ** Current ECC device content pubkey available in clear for writing to the certificate during certificate generation.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
            /*
            ** Write domain key to hash data store.  Key is encrypted with a symmetric key derived from device ECC key.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        case OEM_HAL_KEY_TYPE_INVALID:
            /*
            ** Load domain public key (pubkey).
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_AES_KEYFILE:
            /*
            ** Current ECC device message privkey written to keyfile.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        case OEM_HAL_KEY_TYPE_INVALID:
            /*
            ** Current ECC device message pubkey available in clear for writing to the certificate during certificate generation.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_ECC_P256_SIGN:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_AES_KEYFILE:
            /*
            ** Current ECC device sign privkey written to keyfile.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        case OEM_HAL_KEY_TYPE_INVALID:
            /*
            ** Current ECC device signing pubkey available in clear for writing to the certificate during certificate generation.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
            /*
            ** Symmetrically optimize root license during license acquisition.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:
            /*
            ** Symmetrically optimize root domain license during license acquisition.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        case OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC:
            /*
            ** Only allow public keys with the 'ENCRYPT_KEY' usage to encrypt other keys
            ** Target certificate must also have the additional key usage defined above in order to do rebinding
            ** (refer to comments near OEM_HAL_BCERT_ADDITIONAL_KEYUSAGE_REQUIRED_FOR_REBINDING definition for details)
            */
            ChkDR( _ValidatePublicKeyForWrapping( f_indexWrappingKey, DRM_BCERT_KEYUSAGE_ENCRYPT_KEY, OEM_HAL_BCERT_ADDITIONAL_KEYUSAGE_REQUIRED_FOR_REBINDING ) );

            /*
            ** Rebind root license to remote public key.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC:
            /*
            ** Only allow public keys with the 'ENCRYPT_KEY' usage to encrypt other keys
            ** Target certificate must also have a key with usage 'PRND_ENCRYPT_KEY' (e.g. this blocks generating a license bound to a domain certificate)
            */
            ChkDR( _ValidatePublicKeyForWrapping( f_indexWrappingKey, DRM_BCERT_KEYUSAGE_ENCRYPT_KEY, DRM_BCERT_KEYUSAGE_PRND_ENCRYPT_KEY ) );

            /*
            ** Generate local-bound or remote-bound root license content key.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT:
            /*
            ** Local root license key caching to avoid running out of registers.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
            /*
            ** Symmetrically optimize simple license during license acquisition.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:
            /*
            ** Symmetrically optimize simple domain license during license acquisition.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        case OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC:
            /*
            ** Only allow public keys with the 'ENCRYPT_KEY' usage to encrypt other keys
            ** Target certificate must also have the additional key usage defined above in order to do rebinding
            ** (refer to comments near OEM_HAL_BCERT_ADDITIONAL_KEYUSAGE_REQUIRED_FOR_REBINDING definition for details)
            */
            ChkDR( _ValidatePublicKeyForWrapping( f_indexWrappingKey, DRM_BCERT_KEYUSAGE_ENCRYPT_KEY, OEM_HAL_BCERT_ADDITIONAL_KEYUSAGE_REQUIRED_FOR_REBINDING ) );

            /*
            ** Rebind simple license to remote public key.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC:
            /*
            ** Only allow public keys with the 'ENCRYPT_KEY' usage to encrypt other keys
            ** Target certificate must also have a key with usage 'PRND_ENCRYPT_KEY' (e.g. this blocks generating a license bound to a domain certificate)
            */
            ChkDR( _ValidatePublicKeyForWrapping( f_indexWrappingKey, DRM_BCERT_KEYUSAGE_ENCRYPT_KEY, DRM_BCERT_KEYUSAGE_PRND_ENCRYPT_KEY ) );

            /*
            ** Generate local-bound or remote-bound simple license content key.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:
            /*
            ** Bind generated leaf license content key to generated root license content key.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT:
            /*
            ** Local leaf/simple license key caching to avoid running out of registers.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC:
            /*
            ** Only allow public keys with the 'PRND_ENCRYPT_KEY' usage to encrypt the message encryption keys
            */
            ChkDR( _ValidatePublicKeyForWrapping( f_indexWrappingKey, DRM_BCERT_KEYUSAGE_PRND_ENCRYPT_KEY, DRM_BCERT_KEYUSAGE_PRND_ENCRYPT_KEY ) );

            /*
            ** Generate and encrypt MI/MK with target device pubkey.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_AES_KEYFILE:
            /*
            ** Secure store key written to keyfile.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_LEGACY:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
            /*
            ** Cocktail key re-encrypted with a symmetric key derived from my ecc priv content key in client-side symmetric optimizations during lacq.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        case OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT:
            /*
            ** Cocktail domain-bound key re-encrypted with a symmetric key derived from my ecc priv content key in client-side symmetric optimizations during lacq.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        case OEM_HAL_KEY_TYPE_INVALID:
            /*
            ** Cocktail key leaves HAL in the clear.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    case OEM_HAL_KEY_TYPE_AES_SLK:
        switch( f_eWrappingKeyType )
        {
        case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
            /*
            ** SLK creation encrypted with my ecc priv content key.
            */
            ChkDR( Oem_HalOemImpl_InitializeWrapContext( f_eKeyType, f_indexKey, f_eWrappingKeyType, f_indexWrappingKey, f_pWrapContext ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT );
        }

        break;

    default:
        ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_AllocateRegister
**
** Synopsis: This function reserves a free register of the requested type (if one is available), and returns its index.
**
** Arguments:
**
** [f_eKeyType]           : The type of key register to be allocated.
** [f_pKeyRegisterIndex]  : A pointer to a location into which the index of the allocated register will be placed.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OEMHAL_OUT_OF_KEY_REGISTERS
**             DRM_E_INVALIDARG
**             DRM_E_POINTER
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_AllocateRegister(
    __in              OEM_HAL_KEY_TYPE            f_eKeyType,
    __out_ecount( 1 ) OEM_HAL_KEY_REGISTER_INDEX *f_pKeyRegisterIndex )
{
    DRM_RESULT          dr          = DRM_SUCCESS;
    OEM_HAL_KEY_TYPE    eKeyType    = f_eKeyType;   /* TOC/TOU */

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkDR( Oem_HalOemImpl_AllocateKeyRegister( eKeyType, FALSE, f_pKeyRegisterIndex ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_FreeRegister
**
** Synopsis: This function frees a register of the requested type that was previously allocated.
**
** [f_eKeyType]         : The type of key register to be freed.
** [f_keyRegisterIndex] : The index of the allocated register to be freed.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_INVALIDARG
**
** Notes:      If the register was not previously allocated the operation returns DRM_E_OEMHAL_NO_KEY.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_FreeRegister(
    __in OEM_HAL_KEY_TYPE           f_eKeyType,
    __in OEM_HAL_KEY_REGISTER_INDEX f_keyRegisterIndex )
{
    DRM_RESULT                  dr                  = DRM_SUCCESS;
    OEM_HAL_KEY_TYPE            eKeyType            = f_eKeyType;           /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX  keyRegisterIndex    = f_keyRegisterIndex;   /* TOC/TOU */

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkDR( Oem_HalOemImpl_FreeRegister( eKeyType, keyRegisterIndex ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_RegisterCount
**
** Synopsis: This function returns information about available registers
**
** Arguments:
**
** [f_eKeyType]               : The type of key register to be counted.
** [f_pdwTotalRegisters]      : The total number of registers of this type.
** [f_pdwAllocatedRegisters]  : The number of allocated registers of this type.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_INVALIDARG
**             DRM_E_POINTER
**
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_RegisterCount(
    __in            OEM_HAL_KEY_TYPE     f_eKeyType,
    __out_ecount(1) DRM_DWORD           *f_pdwTotalRegisters,
    __out_ecount(1) DRM_DWORD           *f_pdwAllocatedRegisters )
{
    DRM_RESULT          dr          = DRM_SUCCESS;
    OEM_HAL_KEY_TYPE    eKeyType    = f_eKeyType;   /* TOC/TOU */

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkDR( Oem_HalOemImpl_RegisterCount( eKeyType, f_pdwTotalRegisters, f_pdwAllocatedRegisters ) );

ErrorExit:
    return dr;
}

static DRM_FRE_INLINE DRM_RESULT _GetDefaultKeyId(
    __in              OEM_HAL_KEY_TYPE  f_eType,
    __out_ecount( 1 ) DRM_GUID         *f_pKeyId ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_RESULT _GetDefaultKeyId(
    __in              OEM_HAL_KEY_TYPE  f_eType,
    __out_ecount( 1 ) DRM_GUID         *f_pKeyId )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkPtr( f_pKeyId );

    if( f_eType == OEM_HAL_KEY_TYPE_AES_KEYFILE )
    {
        ChkDR( Oem_Keyfile_GetCurrentKFAesKeyId( f_pKeyId ) );
    }
    else if( f_eType != OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN
          && f_eType != OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC )
    {
        ChkDR( DRM_E_OEMHAL_NO_KEY );
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_GetPreloadedIndex
**
** Synopsis: Return the index of a preloaded key with the specified key ID.
**
** Arguments:
**
** [f_pOEMContext] : The OEM context.
** [f_eKeyType]    : The type of the key to return.
** [f_pbKeyId]     : The unique ID of the preloaded key.
** [f_cbKeyId]     : The unique ID length.
** [f_pIndexKey]   : The register index of the preloaded key.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_INVALIDARG if f_eType or f_cbKeyId aren't valid values
**             DRM_E_POINTER if f_pbKeyId or f_indexKey are NULL
**             DRM_E_OEMHAL_NO_KEY if the key ID isn't recognised
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_GetPreloadedIndex(
    __in                           DRM_VOID                   *f_pOEMContext,
    __in                           OEM_HAL_KEY_TYPE            f_eKeyType,
    __in_bcount( f_cbKeyId ) const DRM_BYTE                   *f_pbKeyId,
    __in                           DRM_DWORD                   f_cbKeyId,
    __out_ecount( 1 )              OEM_HAL_KEY_REGISTER_INDEX *f_pIndexKey )
{
    DRM_RESULT                  dr          = DRM_SUCCESS;
    DRM_GUID                    defaultId   = EMPTY_DRM_GUID;
    DRM_GUID                   *pId         = &defaultId;
    OEM_HAL_KEY_REGISTER_INDEX  keyIndex    = OEM_HAL_KEY_REGISTER_INDEX_INVALID;
    DRM_DWORD                   cbKeyBuffer = 0;
    OEM_HAL_KEY_TYPE            eKeyType    = f_eKeyType;   /* TOC/TOU */
    DRM_BYTE                   *pbKeyId     = NULL;         /* TOC/TOU */
    DRM_DWORD                   cbKeyId     = f_cbKeyId;    /* TOC/TOU */

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkPtr( f_pbKeyId );
    ChkArg( cbKeyId == SIZEOF( DRM_GUID ) );
    ChkPtr( f_pIndexKey );

    ChkMem( pbKeyId = ( DRM_BYTE* )Oem_MemAlloc( cbKeyId ) );
    OEM_SECURE_MEMCPY( pbKeyId, f_pbKeyId, cbKeyId );

    *f_pIndexKey = OEM_HAL_KEY_REGISTER_INDEX_INVALID;

    /*
    ** Use the passed in KeyID buffer, which may be all zeroes but may not be NULL
    */
    pId = ( DRM_GUID* )pbKeyId;

    /*
    ** See if a non-zero ID has been passed in, in which case skip the default lookup
    */
    if( OEM_SECURE_ARE_EQUAL( &defaultId, pId, SIZEOF( DRM_GUID ) ) )
    {
        ChkDR( _GetDefaultKeyId( eKeyType, pId ) );
    }

    /*
    ** See whether this key is already loaded
    */
    ChkDR( Oem_HalOemImpl_LookupPreloadedKey( eKeyType, pId, f_pIndexKey ) );

    /*
    ** If not, try to load it
    */
    if( *f_pIndexKey == OEM_HAL_KEY_REGISTER_INDEX_INVALID )
    {
        ChkDR( Oem_HalOemImpl_VerifyPreloadedKeyInfoSlotIsAvailable() );

        switch( eKeyType )
        {
        case OEM_HAL_KEY_TYPE_AES_KEYFILE:
            ChkDR( Oem_HalOemImpl_AllocateKeyRegister( eKeyType, TRUE, &keyIndex ) );

            Oem_HalOemImpl_ZeroKeyRegister( eKeyType, keyIndex );

            {
                DRM_AES_KEY oKey;
                ChkDR( Oem_Keyfile_GetKFAesKey( pId, &oKey ) );
                dr = Oem_HalOemImpl_SetAesKeyfileExpandedKey( keyIndex, &oKey );
                OEM_SECURE_ZERO_MEMORY( &oKey, SIZEOF( oKey ) );
                ChkDR( dr );
            }
            break;

        case OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN:
            ChkDR( Oem_HalOemImpl_AllocateKeyRegister( eKeyType, TRUE, &keyIndex ) );

            Oem_HalOemImpl_ZeroKeyRegister( eKeyType, keyIndex );

            /*
            ** Return the Device Model Key
            */
            ChkBOOL( OEM_SECURE_ARE_EQUAL( &defaultId, pId, SIZEOF( DRM_GUID ) ), DRM_E_OEMHAL_NO_KEY );

            {
                PRIVKEY_P256 privKey;
                cbKeyBuffer = SIZEOF( PRIVKEY_P256 );
                ChkDR( Oem_Device_GetKey( f_pOEMContext, DRM_DKT_PLAYREADY_MODEL, (DRM_BYTE*)&privKey, &cbKeyBuffer ) );
                dr = Oem_HalOemImpl_SetModelSigningKey( keyIndex, &privKey );
                OEM_SECURE_ZERO_MEMORY( &privKey, SIZEOF( privKey ) );
                ChkDR( dr );
            }
            break;

        case OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC:
            ChkDR( Oem_HalOemImpl_AllocateKeyRegister( eKeyType, TRUE, &keyIndex ) );

            Oem_HalOemImpl_ZeroKeyRegister( eKeyType, keyIndex );

            /*
            ** Return the Root public key
            */
            ChkBOOL( OEM_SECURE_ARE_EQUAL( &defaultId, pId, SIZEOF( DRM_GUID ) ), DRM_E_OEMHAL_NO_KEY );

            ChkDR( Oem_HalOemImpl_SetEcc256PublicKeyRoot(
                keyIndex,
                _GetRootPublicKey(),
                BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ISSUER_ALL ) ) );
            break;

        default:
            ChkDR( DRM_E_OEMHAL_NO_KEY );
        }

        /*
        ** Save the key info for subsequent attempts to get this key
        */
        ChkDR( Oem_HalOemImpl_SavePreloadedKeyInfo( eKeyType, pId, keyIndex ) );

        *f_pIndexKey = keyIndex;
        keyIndex     = OEM_HAL_KEY_REGISTER_INDEX_INVALID;
    }

ErrorExit:
    /*
    ** If there's been a failure, make sure that the register isn't left allocated
    */
    if( keyIndex != OEM_HAL_KEY_REGISTER_INDEX_INVALID )
    {
        (DRM_VOID)Oem_Hal_FreeRegister( eKeyType, keyIndex );
    }

    SAFE_OEM_FREE( pbKeyId );

    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_Initialize
**
** Synopsis: Initializes the crypto core.  This operation should be called only once per boot:
**           subsequent calls must result in a no-op.
**           When this function is called, the crypto core must load its private keys and verify
**           their integrity. If there is a failure in the integrity check then the core must return
**           an error, and not carry out any further operations.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OUTOFMEMORY
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_Initialize()
{
    return Oem_HalOemImpl_Initialize();
}

/*********************************************************************
**
** Function: Oem_Hal_Deinitialize
**
** Synopsis: This function deinitializes the cryptocore, leaving it unable to used further. This should be done
**           only as part of an orderly shutdown of the platform.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_NOT_INITIALIZED
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_Deinitialize()
{
    return Oem_HalOemImpl_Deinitialize();
}

/*********************************************************************
**
** Function: Oem_Hal_VerifyMessageSignature
**
** Synopsis: This function verifies the asymmetric signature of the supplied message using the validated
**           public key contained in f_indexIntegrityKey.
**
** Arguments:
**
** [f_pbMessage]            : The message whose signature is to be verified.
** [f_cbMessage]            : The length (in bytes) of the message.
** [f_pbSignature]          : The signature to be verified.
** [f_cbSignature]          : The length (in  bytes) of the signature.
** [f_eHashType]            : The hash algorithm to be used.
** [f_eSignatureScheme]     : The signature scheme (including padding type) to be used.
** [f_indexIntegrityKey]    : The index of the key to be used for verification of the message.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_BAD_KEY_REGISTER_INDEX
**             DRM_E_OEMHAL_BAD_MESSAGE_LENGTH
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_BUFFER_TOO_LARGE
**             DRM_E_BUFFERTOOSMALL
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OUTOFMEMORY
**             DRM_E_OEMHAL_SIGNATURE_CHECK_FAILED
**             DRM_E_OEMHAL_UNSUPPORTED_HASH_TYPE
**             DRM_E_OEMHAL_UNSUPPORTED_SIGNATURE_SCHEME
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_VerifyMessageSignature(
    __in_bcount( f_cbMessage )   const DRM_BYTE                  *f_pbMessage,
    __in                               DRM_DWORD                  f_cbMessage,
    __in_bcount( f_cbSignature ) const DRM_BYTE                  *f_pbSignature,
    __in                               DRM_DWORD                  f_cbSignature,
    __in                               OEM_HAL_HASH_TYPE          f_eHashType,
    __in                               OEM_HAL_SIGNATURE_SCHEME   f_eSignatureScheme,
    __in                               OEM_HAL_KEY_REGISTER_INDEX f_indexIntegrityKey )
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_BYTE                    *pbMessage          = NULL;                     /* TOC/TOU */
    DRM_DWORD                    cbMessage          = f_cbMessage;              /* TOC/TOU */
    DRM_BYTE                    *pbSignature        = NULL;                     /* TOC/TOU */
    DRM_DWORD                    cbSignature        = f_cbSignature;            /* TOC/TOU */
    OEM_HAL_HASH_TYPE            eHashType          = f_eHashType;              /* TOC/TOU */
    OEM_HAL_SIGNATURE_SCHEME     eSignatureScheme   = f_eSignatureScheme;       /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX   indexIntegrityKey  = f_indexIntegrityKey;      /* TOC/TOU */

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    if( f_pbMessage != NULL && cbMessage > 0 )
    {
        ChkMem( pbMessage = ( DRM_BYTE* )Oem_MemAlloc( cbMessage ) );
        OEM_SECURE_MEMCPY( pbMessage, f_pbMessage, cbMessage );
    }
    if( f_pbSignature != NULL && f_cbSignature > 0 )
    {
        ChkMem( pbSignature = ( DRM_BYTE* )Oem_MemAlloc( cbSignature ) );
        OEM_SECURE_MEMCPY( pbSignature, f_pbSignature, cbSignature );
    }

    ChkDR( Oem_HalOemImpl_VerifyMessageSignature( pbMessage, cbMessage, pbSignature, cbSignature, eHashType, eSignatureScheme, indexIntegrityKey ) );

ErrorExit:
    SAFE_OEM_FREE( pbMessage );
    SAFE_OEM_FREE( pbSignature );
    return dr;
}

static DRM_RESULT _Hal_WrapKey(
    __in                                 DRM_VOID                   *f_pOEMContext,
    __inout_ecount( 1 )                  DRM_CRYPTO_CONTEXT         *f_pCryptoContext,
    __in                                 OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                                 OEM_HAL_KEY_REGISTER_INDEX  f_indexKey,
    __in                                 OEM_HAL_KEY_TYPE            f_eWrappingKeyType,
    __in                                 OEM_HAL_KEY_REGISTER_INDEX  f_indexWrappingKey,
    __out_bcount_opt( *f_pcbWrappedKey ) DRM_BYTE                   *f_pbWrappedKey,
    __inout_ecount(1)                    DRM_DWORD                  *f_pcbWrappedKey )
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    OEM_HAL_WRAP_CONTEXT         wrapContext        = OEM_HAL_WRAP_CONTEXT_INVALID;
    OEM_HAL_CRYPTO_TYPE          eEncryptionType    = eOEM_HAL_CRYPTO_TYPE_INVALID;

    /*
    ** Some wrap operations allow using an invalid key to pass a key out in the clear.
    ** _InitializeWrapContext will ensure that this only happens for permitted
    ** key types
    */
    if( f_eWrappingKeyType != OEM_HAL_KEY_TYPE_INVALID )
    {
        ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( f_eWrappingKeyType, f_indexWrappingKey ) );
    }

    /*
    ** Initialize the wrap Context, verifying that this is a valid combination of keys
    */
    ChkDR( _InitializeWrapContext( f_eKeyType,
                                   f_indexKey,
                                   f_eWrappingKeyType,
                                   f_indexWrappingKey,
                                   &wrapContext ) );

    ChkDR( Oem_HalOemImpl_WrapGetEncryptionType( &wrapContext, &eEncryptionType ) );

    switch( eEncryptionType )
    {
    case eOEM_HAL_CRYPTO_TYPE_DERIVED_SYMMETRIC_KEY:
        ChkDR( Oem_HalOemImpl_WrapWithSymmetricCipher( &wrapContext, f_pbWrappedKey, f_pcbWrappedKey ) );
        break;

    case eOEM_HAL_CRYPTO_TYPE_PASSTHROUGH:
        ChkDR( Oem_HalOemImpl_WrapWithPassThrough( &wrapContext, f_pbWrappedKey, f_pcbWrappedKey ) );
        break;

    case eOEM_HAL_CRYPTO_TYPE_AES_CBC:
        ChkArg( *f_pcbWrappedKey == SIZEOF( DRM_ENCRYPTED_KEY ) );
        ChkDR( Oem_HalOemImpl_WrapEncryptedKey( f_pOEMContext, f_eKeyType, &wrapContext, ( DRM_ENCRYPTED_KEY* )f_pbWrappedKey ) );
        break;

    case eOEM_HAL_CRYPTO_TYPE_AES_ECB:
        ChkDR( Oem_HalOemImpl_WrapWithAesEcb( &wrapContext, f_pbWrappedKey, f_pcbWrappedKey ) );
        break;

    case eOEM_HAL_CRYPTO_TYPE_ECC256_PUBLIC_KEY:
        ChkDR( Oem_HalOemImpl_WrapWithPublicKey( f_pCryptoContext, &wrapContext, f_pbWrappedKey, f_pcbWrappedKey ) );
        break;

    default:
        /*
        ** Should never reach here - set return code to DRM_E_LOGICERR
        */
        DRMASSERT( FALSE );
        ChkDR( DRM_E_LOGICERR );
        break;
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_SignWithPrivateKey
**
** Synopsis:    This function creates a digital signature for a message/cert.
**              If the signature scheme is OEM_HAL_SIGNATURE_SCHEME_RSASSA_PKCS1_v1_5
**              the signature is created using NonAV_Pk. If the scheme is
**              OEM_HAL_SIGNATURE_SCHEME_ECDSA_P256 the signature is created
**              using the device's PlayReady signing key OR the device's
**              model certificate private signing key (for cert generation).
**              If the signature buffer passed in is null or if the buffer
**              is too small to store the signature, the actual buffer
**              length needed is returned in signatureLength.
**              Note that OEM_HAL_SIGNATURE_SCHEME_RSASSA_PKCS1_v1_5 is not
**              supported in the default PK implementation.
**
** Arguments:
**
** [f_pOEMContext]          : The OEM context.
** [f_pCryptoContext]       : The crypto context.
** [f_pbMessage]            : The message to be signed.
**                            If the model certificate private key is being used,
**                            this must contain an unsigned certificate where the
**                            public keys in that certificate were generated inside
**                            the HAL with appropriate key usages.
** [f_cbMessage]            : The length (in bytes) of the message.
** [f_pbSignature]          : The buffer that is to hold the signature.
** [f_pcbSignature]         : A pointer to a location holding the length (in bytes) of
**                            the signature buffer. If the buffer is too small to contain the signature, the function returns
**                            an error indicating that, and updates the location with the buffer size required.
** [f_eHashType]            : The hash algorithm to be used.
** [f_eSignatureScheme]     : The signature scheme (including padding type) to be used.
** [f_eKeyType]             : The type of the signing key.
** [f_indexIntegrityKey]    : The index of the signing key.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_BAD_MESSAGE_LENGTH
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_BUFFER_TOO_LARGE
**             DRM_E_BUFFERTOOSMALL
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OUTOFMEMORY
**             DRM_E_OEMHAL_UNSUPPORTED_HASH_TYPE
**             DRM_E_OEMHAL_UNSUPPORTED_SIGNATURE_SCHEME
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_SignWithPrivateKey(
    __in                                DRM_VOID                  *f_pOEMContext,
    __inout_ecount_opt( 1 )             DRM_CRYPTO_CONTEXT        *f_pCryptoContext,
    __in_bcount( f_cbMessage )    const DRM_BYTE                  *f_pbMessage,
    __in                                DRM_DWORD                  f_cbMessage,
    __out_bcount_opt( *f_pcbSignature ) DRM_BYTE                  *f_pbSignature,
    __inout                             DRM_DWORD                 *f_pcbSignature,
    __in                                OEM_HAL_HASH_TYPE          f_eHashType,
    __in                                OEM_HAL_SIGNATURE_SCHEME   f_eSignatureScheme,
    __in                                OEM_HAL_KEY_TYPE           f_eKeyType,
    __in                                OEM_HAL_KEY_REGISTER_INDEX f_indexIntegrityKey )
{
    DRM_RESULT                   dr                                                 = DRM_SUCCESS;
    DRM_BYTE                    *pbExpandedMessage                                  = NULL;
    DRM_DWORD                    dwRequestedSecurityLevelForLeafDeviceCertificate   = 0;
    DRM_BYTE                    *pbMessage                                          = NULL;                 /* TOC/TOU */
    DRM_DWORD                    cbMessage                                          = f_cbMessage;          /* TOC/TOU */
    DRM_DWORD                    cbSignature                                        = 0;                    /* TOC/TOU */
    OEM_HAL_HASH_TYPE            eHashType                                          = f_eHashType;          /* TOC/TOU */
    OEM_HAL_SIGNATURE_SCHEME     eSignatureScheme                                   = f_eSignatureScheme;   /* TOC/TOU */
    OEM_HAL_KEY_TYPE             eKeyType                                           = f_eKeyType;           /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX   indexIntegrityKey                                  = f_indexIntegrityKey;  /* TOC/TOU */
    DECLARE_LOCAL_CRYPTO_CONTEXT();


    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkPtr( f_pbMessage );
    ChkArg( cbMessage > 0 );
    ChkPtr( f_pcbSignature );
    cbSignature = *f_pcbSignature;

    ALLOC_LOCAL_CRYPTO_CONTEXT();


    ChkMem( pbMessage = ( DRM_BYTE* )Oem_MemAlloc( cbMessage ) );
    OEM_SECURE_MEMCPY( pbMessage, f_pbMessage, cbMessage );


    if( !Oem_HalOemImpl_CanSkipLeafCertificateValidityCheckDuringSignOperation()
     && eKeyType == OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN )
    {
        DRM_DWORD                       cbExpandedMessage       = 0;
        DRM_BYTE                       *pbOffset                = NULL;
        DRM_BCERT_VERIFICATIONCONTEXT   oVerificationContext    = {0};
        DRM_BCERT_MINIMALDATA           oMinCertData            = {0};
        DRM_DWORD                       iKey                    = 0;
        DRM_DWORD                       dwUnused                = 0;

        /*
        ** We are signing data with the model certificate private signing key.
        ** We should not allow the caller to sign arbitrary data with this key.
        ** We should not allow the caller to sign a certificate that contains a public
        **  key for which they have the corresponding private key.
        ** We should only be signing a valid certificate that contains public keys that we generated.
        ** Therefore, we will parse the data passed in as a certificate and verify its public keys.
        */

        /*
        ** 1. The certificate passed in is incomplete - it lacks the signature object.
        ** We have to add a structurally valid signature object (without actually having the signature)
        ** before we try to parse the certificate or the certificate parser will fail.
        */

        ChkDR( DRM_DWordAdd( cbMessage, DRM_BCERT_SIGNATURE_OBJECT_LEN, &cbExpandedMessage ) );
        ChkMem( pbExpandedMessage = (DRM_BYTE*)Oem_MemAlloc( cbExpandedMessage ) );

        OEM_SECURE_MEMCPY( pbExpandedMessage, pbMessage, cbMessage );
        pbOffset = &pbExpandedMessage[cbMessage];

        OEM_SECURE_ZERO_MEMORY( pbOffset, DRM_BCERT_SIGNATURE_OBJECT_LEN );
        WORD_TO_NETWORKBYTES( pbOffset, 0, DRM_BCERT_OBJFLAG_MUST_UNDERSTAND );
        pbOffset += SIZEOF(DRM_WORD);
        WORD_TO_NETWORKBYTES( pbOffset, 0, DRM_BCERT_OBJTYPE_SIGNATURE );
        pbOffset += SIZEOF(DRM_WORD);
        DWORD_TO_NETWORKBYTES( pbOffset, 0, DRM_BCERT_SIGNATURE_OBJECT_LEN );
        pbOffset += SIZEOF(DRM_DWORD);
        WORD_TO_NETWORKBYTES( pbOffset, 0, DRM_BCERT_SIGNATURE_TYPE_P256 );
        pbOffset += SIZEOF(DRM_WORD);
        WORD_TO_NETWORKBYTES( pbOffset, 0, SIZEOF( SIGNATURE_P256 ) );
        pbOffset += SIZEOF(DRM_WORD);
        pbOffset += SIZEOF(SIGNATURE_P256);     /* Skip signature data */
        DWORD_TO_NETWORKBYTES( pbOffset, 0, SIZEOF( PUBKEY_P256 ) * CHAR_BIT );
        pbOffset += SIZEOF(DRM_DWORD);
        pbOffset += SIZEOF(PUBKEY_P256);        /* Skip signing key */
        DRMASSERT( pbOffset == pbExpandedMessage + cbExpandedMessage );


        /*
        ** 2. Parse the public keys out of the certificate.  If certificate parsing fails, we will fail.
        */
        ChkDR( DRM_BCert_InitVerificationContext(
            NULL,
            NULL,
            0,
            NULL,
            FALSE,
            TRUE,
            NULL,
            0,
            FALSE,
            NULL,
            NULL,
            NULL,
            &oVerificationContext ) );


        ChkDR( DRM_BCert_ParseRawCertificate(
            pbExpandedMessage,
            cbExpandedMessage,
            &dwUnused,
            NULL,
            &oMinCertData,
            &oVerificationContext ) );


        /*
        ** Oem_HalOemImpl_SignWithPrivateKey is going to verify that the certificate we are
        ** signing does not have a higher security level than this device's security level
        ** (as defined by OEM_KEY_GENERATION_SECURITY_LEVEL in oemhaloemimpl.c).
        ** This has the pleasant side-effect of FORCING the OEM to properly change
        ** the OEM_KEY_GENERATION_SECURITY_LEVEL value to 2000 in oemhaloemimpl.c if they ever
        ** want to playback SL 2000 content on their device because they won't even be able
        ** to generate a leaf device certificate with the appropriate security level otherwise.
        */
        dwRequestedSecurityLevelForLeafDeviceCertificate = oMinCertData.oBasicInfo.dwSecurityLevel;

        AssertChkArg( oMinCertData.oKeyInfo.dwNumKeys <= DRM_BCERT_MAX_KEY_USAGES );


        /*
        ** 3. For each key in the certificate...
        */
        if( !Oem_HalOemImpl_CanSkipLeafCertificateKeysValidityCheckDuringSignOperation() )
        {

            for( iKey = 0; iKey < oMinCertData.oKeyInfo.dwNumKeys; iKey++ )
            {
                const DRM_BCERT_CERTKEY     *pCertKey               = &oMinCertData.oKeyInfo.rgoKeys[iKey];
                OEM_HAL_KEY_TYPE             eKeyTypeLocal          = OEM_HAL_KEY_TYPE_INVALID;
                DRM_DWORD                    dwTotalRegisters       = 0;
                OEM_HAL_KEY_REGISTER_INDEX   iKeyRegister           = 0;
                DRM_BOOL                     fKeyVerified           = FALSE;

                /*
                ** 4. Verify that the next key's key usage is valid for a leaf certificate.
                ** If any invalid key usage is specified, we will fail.
                */
                switch( pCertKey->dwUsageSet )
                {
                case BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_SIGN ):

                    eKeyTypeLocal = OEM_HAL_KEY_TYPE_ECC_P256_SIGN;
                    break;
                case BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_ENCRYPT_KEY ):

                    eKeyTypeLocal = OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT;
                    break;
                case BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_PRND_ENCRYPT_KEY ):

                    eKeyTypeLocal = OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT;
                    break;
                default:

                    ChkDR( DRM_E_BCERT_INVALID_KEY_USAGE );
                    break;
                }


                ChkDR( Oem_HalOemImpl_RegisterCount( eKeyTypeLocal, &dwTotalRegisters, NULL ) );

                /*
                ** 5. Attempt to find the next key in the list of public keys we have of that type.
                */
                for( iKeyRegister = 0; !fKeyVerified && iKeyRegister < (OEM_HAL_KEY_REGISTER_INDEX)dwTotalRegisters; iKeyRegister++ )
                {
                    if( DRM_SUCCEEDED( Oem_HalOemImpl_VerifyValidKeyRegister( eKeyTypeLocal, iKeyRegister ) ) )
                    {
                        PUBKEY_P256        oGeneratedPubKey  = {0};
                        DRM_DWORD          cbGeneratedPubKey = SIZEOF(oGeneratedPubKey);
                        const PUBKEY_P256 *pCertPubkey       = oMinCertData.oKeyInfo.rgoKeys[iKey].pValue;


                        /*
                        ** Note: If the key register is valid for a public key of a valid type,
                        ** then we should be able to export it.  This function could fail if we
                        ** only have the private key out of the key pair for this index, but a
                        ** later index might have the public key.
                        */
                        dr = _Hal_WrapKey(
                            f_pOEMContext,
                            USE_LOCAL_CRYPTO_CONTEXT,
                            eKeyTypeLocal,
                            iKeyRegister,
                            OEM_HAL_KEY_TYPE_INVALID,
                            0,
                            (DRM_BYTE*)&oGeneratedPubKey,
                            &cbGeneratedPubKey );

                        if( dr == DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT )
                        {
                            dr = DRM_SUCCESS;
                            continue;
                        }
                        ChkDR( dr );
                        AssertChkArg( cbGeneratedPubKey == SIZEOF( oGeneratedPubKey ) );


                        if( OEM_SECURE_ARE_EQUAL( &oGeneratedPubKey, pCertPubkey, SIZEOF( oGeneratedPubKey ) ) )
                        {

                            /* We found it. */
                            fKeyVerified = TRUE;
                        }
                    }
                }

                /*
                ** 6. If we didn't find the next key, then the caller is attempting to have us sign a certificate with keys
                **  that we didn't generate or with keys we did generate but that are of the wrong type, so we will fail.
                */
                ChkBOOL( fKeyVerified, DRM_E_INVALID_DEVICE_CERTIFICATE );

            }
        }
    }


    ChkDR( Oem_HalOemImpl_SignWithPrivateKey(
        USE_LOCAL_CRYPTO_CONTEXT,
        pbMessage,
        cbMessage,
        f_pbSignature,
        &cbSignature,
        dwRequestedSecurityLevelForLeafDeviceCertificate,   /* Note: This value is ignored by the callee unless eKeyType == OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN */
        eHashType,
        eSignatureScheme,
        eKeyType,
        indexIntegrityKey ) );


ErrorExit:

    FREE_LOCAL_CRYPTO_CONTEXT();
    if( f_pcbSignature != NULL )
    {
        *f_pcbSignature = cbSignature;
    }
    SAFE_OEM_FREE( pbExpandedMessage );
    SAFE_OEM_FREE( pbMessage );
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_VerifyOMAC1Signature
**
** Synopsis: This function verifies the signature of the supplied data blob using the AES-OMAC1 algorithm
**           with the indicated key.
**
** Arguments:
**
** [f_pbData]       : The data blob whose signature is to be verified.
** [f_cbData]       : The length (in bytes) of the data blob.
** [f_pbSignature]  : The signature to be verified.
** [f_cbSignature]  : The length of the signature.
** [f_eKeyType]     : The type of the key to be used for the AES OMAC1 signature validation.
** [f_pIndexKey]    : The index of the key to be used for the AES OMAC1 signature validation.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_BAD_KEY_REGISTER_INDEX
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_BUFFER_TOO_LARGE
**             DRM_E_BUFFERTOOSMALL
**             DRM_E_OEMHAL_NO_KEY_IN_REGISTER
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OUTOFMEMORY
**             DRM_E_OEMHAL_SIGNATURE_CHECK_FAILED
**             DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_VerifyOMAC1Signature(
    __in_bcount( f_cbData )      const DRM_BYTE                   *f_pbData,
    __in                               DRM_DWORD                   f_cbData,
    __in_bcount( f_cbSignature ) const DRM_BYTE                   *f_pbSignature,
    __in                               DRM_DWORD                   f_cbSignature,
    __in                               OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                               OEM_HAL_KEY_REGISTER_INDEX  f_indexKey )
{
    DRM_RESULT                   dr             = DRM_SUCCESS;
    DRM_BYTE                    *pbData         = NULL;             /* TOC/TOU */
    DRM_BYTE                    *pbSignature    = NULL;             /* TOC/TOU */
    DRM_DWORD                    cbData         = f_cbData;         /* TOC/TOU */
    DRM_DWORD                    cbSignature    = f_cbSignature;    /* TOC/TOU */
    OEM_HAL_KEY_TYPE             eKeyType       = f_eKeyType;       /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX   indexKey       = f_indexKey;       /* TOC/TOU */

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkPtr( f_pbData );
    ChkArg( cbData > 0 );
    ChkPtr( f_pbSignature );
    ChkArg( cbSignature > 0 );

    ChkMem( pbData = ( DRM_BYTE* )Oem_MemAlloc( cbData ) );
    OEM_SECURE_MEMCPY( pbData, f_pbData, cbData );
    ChkMem( pbSignature = ( DRM_BYTE* )Oem_MemAlloc( cbSignature ) );
    OEM_SECURE_MEMCPY( pbSignature, f_pbSignature, cbSignature );

    ChkDR( Oem_HalOemImpl_VerifyOMAC1Signature( pbData, cbData, pbSignature, cbSignature, eKeyType, indexKey ) );

ErrorExit:
    SAFE_OEM_FREE( pbData );
    SAFE_OEM_FREE( pbSignature );
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_CreateOMAC1Signature
**
** Synopsis: This function creates the signature of the supplied data blob using the AES-OMAC1 algorithm
**           with the indicated key. If the signature buffer passed in is null or if the buffer is too
**           small to store the signature, the actual buffer length needed is returned in f_pcbSignature.
**
** Arguments:
**
** [f_pbData]       : The data blob whose signature is to be verified.
** [f_cbData]       : The length (in bytes) of the data blob.
** [f_pbSignature]  : The signature to be verified.
** [f_pcbSignature] : A pointer to a location holding the length (in bytes)
**                    of the signature buffer. If the buffer is too small to contain the signature, the function
**                    returns an error indicating that, and updates the location with the buffer size required.
** [f_eKeyType]     : The type of the key to be used for the AES OMAC1 signature creation.
** [f_indexKey]     : The index of the key to be used for the AES OMAC1 signature creation.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_BAD_KEY_REGISTER_INDEX
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_BUFFER_TOO_LARGE
**             DRM_E_BUFFERTOOSMALL
**             DRM_E_OEMHAL_NO_KEY_IN_REGISTER
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OUTOFMEMORY
**             DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_CreateOMAC1Signature(
    __in_bcount( f_cbData )        const DRM_BYTE                   *f_pbData,
    __in                                 DRM_DWORD                   f_cbData,
    __out_bcount_opt( *f_pcbSignature )  DRM_BYTE                   *f_pbSignature,
    __inout                              DRM_DWORD                  *f_pcbSignature,
    __in                                 OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                                 OEM_HAL_KEY_REGISTER_INDEX  f_indexKey )
{
    DRM_RESULT                   dr                         = DRM_SUCCESS;
    DRM_XMR_LICENSE              oLicense                   = {0};
    const DRM_DWORD              icbOuter                   = XMR_HEADER_LENGTH + 2 * SIZEOF( DRM_WORD );
    DRM_DWORD                    cbOuter                    = 0;
    const DRM_DWORD              cbSig                      = XMR_BASE_OBJECT_LENGTH + XMR_AES_OMAC_SIGNATURE_LENGTH + 2 * SIZEOF( DRM_WORD );
    DRM_DWORD                    cbOuterNoSig               = 0;
    DRM_BYTE                    *pbData                     = NULL;         /* TOC/TOU */
    DRM_DWORD                    cbData                     = f_cbData;     /* TOC/TOU */
    DRM_DWORD                    cbSignature                = 0;            /* TOC/TOU */
    OEM_HAL_KEY_TYPE             eKeyType                   = f_eKeyType;   /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX   indexKey                   = f_indexKey;   /* TOC/TOU */
    DRM_DWORD                    dwRequestedSecurityLevel   = 0;
    DRM_DWORD                    dwRequestedRIV             = 0;

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkPtr( f_pbData );
    ChkArg( cbData > 0 );
    ChkPtr( f_pcbSignature );
    cbSignature = *f_pcbSignature;

    ChkMem( pbData = ( DRM_BYTE* )Oem_MemAlloc( cbData ) );
    OEM_SECURE_MEMCPY( pbData, f_pbData, cbData );

    switch( eKeyType )
    {
    case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:
    case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:
    case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:
    case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:

        /*
        ** We are going to parse pbData as a license.  It MUST be a license for these key types.
        **
        ** However, when this function is called:
        ** A. The buffer we are passed to sign (pbData, cbData) does not include the signature object.
        ** B. The outer container object inside that buffer (pbData[icbOuter]) has a size which includes the signature object.
        ** Thus, OEM_XMR_SparseUnpackLicense will fail because the outer container size (pbData[icbOuter])
        **  plus license header size (fixed) is larger than the entire buffer (cbData).
        ** Specifically, it will be too large by exactly the size of the siganture object (cbSig).
        ** So, we:
        ** 1. Read the outer container size from (cbOuter = pbData[icbOuter])  (checking for overflow)
        ** 2. Calculate the correct outer container size without signature by subtracting signature size (cbOuterNoSig = cbOuter - cbSig)
        ** 3. Overwrite the outer container size (pbData[icbOuter] = cbOuterNoSig).
        ** 4. Parse the license with OEM_XMR_SparseUnpackLicense.
        ** 5. Re-overwrite the outer container size with its previous value.  The original value is what's required at signing time below.
        */
        ChkBOOL( cbData > icbOuter + SIZEOF( DRM_DWORD ), DRM_E_INVALID_LICENSE );
        NETWORKBYTES_TO_DWORD( cbOuter, pbData, icbOuter );
        ChkBOOL( cbOuter > cbSig, DRM_E_INVALID_LICENSE );
        cbOuterNoSig = cbOuter - cbSig;

        DWORD_TO_NETWORKBYTES( pbData, icbOuter, cbOuterNoSig );
        ChkDR( OEM_XMR_SparseUnpackLicense( pbData, cbData, &oLicense ) );
        DWORD_TO_NETWORKBYTES( pbData, icbOuter, cbOuter );

        ChkBOOL( XMR_IS_CONTENT_KEY_VALID( &oLicense ), DRM_E_INVALID_LICENSE );
        ChkBOOL( XMR_IS_SECURITY_LEVEL_VALID( &oLicense ), DRM_E_INVALID_LICENSE );
        ChkBOOL( XMR_IS_REVOCATION_INFORMATION_VERSION_VALID( &oLicense ), DRM_E_INVALID_LICENSE );
        dwRequestedSecurityLevel = oLicense.containerOuter.containerGlobalPolicies.MinimumEnvironment.wMinimumSecurityLevel;
        dwRequestedRIV           = oLicense.containerOuter.containerGlobalPolicies.RevocationInformationVersion.dwValue;

        break;
    default:
        /* No license parsing required. */
        break;
    }

    /*
    ** One reason we parsed the license above is to block license signing if the license has the wrong key type.
    ** Otherwise, a root key could be repurposed as leaf key and then misused to decrypt the leaf content keys into clear!
    */
    switch( eKeyType )
    {
    case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:
    case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:
        ChkBOOL( oLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB, DRM_E_UNSUPPORTED_ALGORITHM );
        break;
    case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:
    case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:
        ChkBOOL( oLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR, DRM_E_UNSUPPORTED_ALGORITHM );
        break;
    default:
        /* No additional verification required. */
        break;
    }

#if DRM_SUPPORT_SECUREOEMHAL

    /*
    ** One reason we parsed the license above is to block license signing for non-in-memory-only licenses during rebind.
    ** Otherwise, a license could be rebound for PRND streaming as a persisted license and then saved on the target
    ** machine (i.e. PRND receiver) even after the PRND session has ended, thus allowing unlimited play on the target.
    ** However, we can only block this when secure HAL is enabled because the symmetric optimizations feature also
    ** requires that a license be resigned (specifically: after a symmetrically optimized content key is added to it).
    */
    switch( eKeyType )
    {
    case OEM_HAL_KEY_TYPE_AES_ECB_ROOT:
    case OEM_HAL_KEY_TYPE_AES_CTR_CONTENT:
        ChkBOOL( XMR_IS_CANNOT_PERSIST_LICENSE( &oLicense ), DRM_E_LICGEN_PERSISTENT_REMOTE_LICENSE );
        break;
    default:
        /* No additional verification required. */
        break;
    }

#endif /* DRM_SUPPORT_SECUREOEMHAL */

    ChkDR( Oem_HalOemImpl_CreateOMAC1Signature( pbData, cbData, f_pbSignature, &cbSignature, eKeyType, indexKey, dwRequestedSecurityLevel, dwRequestedRIV ) );

ErrorExit:
    if( f_pcbSignature != NULL )
    {
        *f_pcbSignature = cbSignature;
    }
    SAFE_OEM_FREE( pbData );
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_EncryptDataBlock
**
** Synopsis: This function encrypts a single 16-byte block with a message encryption or root key.
**
** Arguments:
**
** [f_pbBlock]                  : The data block to be encrypted.
** [f_cbBlock]                  : The length (in bytes) of the data block. This must be 16 bytes long.
** [f_pbEncryptedBlock]         : Buffer to output the encrypted block.
** [f_pcbEncryptedBlock]        : A pointer to a location holding the length (in bytes)
**                                of the encrypted block buffer. If the buffer is too small the function
**                                returns an error indicating that, and updates the location with the buffer size required.
** [f_eKeyType]                 : The type of the key to be used when encrypting.
** [f_indexMessageEncryptKey]   : The index of the key to be used when encrypting.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_BAD_KEY_REGISTER_INDEX
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_BUFFER_TOO_LARGE
**             DRM_E_BUFFERTOOSMALL
**             DRM_E_OEMHAL_NO_KEY_IN_REGISTER
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE
**             DRM_E_OUTOFMEMORY
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_EncryptDataBlock(
    __in_bcount( f_cbData )             const DRM_BYTE                   *f_pbBlock,
    __in                                      DRM_DWORD                   f_cbBlock,
    __out_bcount_opt( *f_pcbEncryptedBlock )  DRM_BYTE                   *f_pbEncryptedBlock,
    __inout                                   DRM_DWORD                  *f_pcbEncryptedBlock,
    __in                                      OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                                      OEM_HAL_KEY_REGISTER_INDEX  f_indexMessageEncryptKey )
{
    OEM_HAL_KEY_TYPE             eKeyType               = f_eKeyType;               /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX   indexMessageEncryptKey = f_indexMessageEncryptKey; /* TOC/TOU */

    /*
    ** Performance: Do not make a copy f_pbBlock / f_cbBlock.  No TOC/TOU attack exists for this data.
    ** If modified by caller after being passed in, garbage data will come out for the encrypted data.
    */

    return Oem_HalOemImpl_EncryptDataBlock( f_pbBlock, f_cbBlock, f_pbEncryptedBlock, f_pcbEncryptedBlock, eKeyType, indexMessageEncryptKey );
}

/*********************************************************************
**
** Function: Oem_Hal_IsDevelopmentPlatform
**
** Synopsis: Returns an indication of whether the platform is a development, as opposed to production,
**           version.
**
** Returns:    TRUE:  Platform is development.
**             FALSE: Platform is production.
**
**********************************************************************/
DRM_API DRM_BOOL DRM_CALL Oem_Hal_IsDevelopmentPlatform()
{
#if DBG
    return TRUE;
#else
    return FALSE;
#endif
}

/*********************************************************************
**
** Function: _UnshuffleContentKey
**
** Synopsis: Function that unshuffles a content key in the form of CI||CK
**           to the original value.
**           Note: X||Y = x0y0x1y1...xnyn where xi and yi are ith byte
**           value of X and Y.
**
** Arguments:
**
** [f_pbContentKey] : Pointer to a content key that is shuffled.
**                    The same buffer receives the content key unshuffled.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_FRE_INLINE DRM_RESULT _UnshuffleContentKey(
    __inout_bcount( DRM_AES_KEYSIZE_128_X2 ) DRM_BYTE *f_pbContentKey ) DRM_FRE_INLINE_ATTRIBUTE;
static DRM_FRE_INLINE DRM_RESULT _UnshuffleContentKey(
    __inout_bcount( DRM_AES_KEYSIZE_128_X2 ) DRM_BYTE *f_pbContentKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE   rgbTmp[ DRM_AES_KEYSIZE_128_X2 ];    /* Performance: No reason to initialize. */
    DRM_DWORD  i = 0;
    DRM_DWORD  j = 0;

    DRMCASSERT( DRM_AES_KEYSIZE_128_X2 == ( DRM_AES_KEYSIZE_128 * 2 ) );

    ChkPtr( f_pbContentKey );

    for( i = 0; i < DRM_AES_KEYSIZE_128; i++ )
    {
        rgbTmp[ i ] = f_pbContentKey[ j++ ];
        rgbTmp[ i + DRM_AES_KEYSIZE_128 ] = f_pbContentKey[ j++ ];
    }

    OEM_SECURE_MEMCPY( f_pbContentKey, rgbTmp, DRM_AES_KEYSIZE_128_X2 );

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( rgbTmp, SIZEOF( rgbTmp ) );
    return dr;
}

/*  Verify the checksum on a content key. */
static DRM_RESULT _VerifyChecksum(
    __in_bcount( f_cbKey ) const DRM_BYTE                *f_pbKey,
    __in                         DRM_DWORD                f_cbKey,
    __in_ecount( 1 )       const DRM_BB_CHECKSUM_CONTEXT *f_pChecksum,
    __inout_ecount( 1 )          DRM_CRYPTO_CONTEXT      *f_pCryptoContext )
{
    DRM_RESULT          dr             = DRM_SUCCESS;
    DRM_AES_KEY         aesKey         = {0};
    DRM_CIPHER_CONTEXT *pCipherContext = NULL;
    DRM_BYTE            rgbTemp[max( LEGACY_KID_B64ENC_LEN, DRM_SHA1_DIGEST_LEN + 1 )] = {0};

    ChkPtr( f_pChecksum      );
    ChkPtr( f_pCryptoContext );

    ChkBOOL( f_pbKey != NULL, DRM_E_CH_BAD_KEY );
    ChkBOOL( f_cbKey != 0, DRM_E_CH_BAD_KEY );

    OEM_SECURE_ZERO_MEMORY( rgbTemp, SIZEOF( rgbTemp ) );

    if( f_pChecksum->m_fIgnoreChecksum )
    {
        goto ErrorExit;
    }

    if( f_pChecksum->m_pbV1KID != NULL )
    {
        pCipherContext = ( DRM_CIPHER_CONTEXT * )( f_pCryptoContext->rgbCryptoContext );

        ChkDR( DRM_CPHR_Init( pCipherContext, eDRM_RC4_CIPHER, f_cbKey,f_pbKey ) );

        /*
        ** Verify the secret data:
        ** The secret is the encrypted blob of the base64 encoded KID with the content key
        */
        ChkBOOL( f_pChecksum->m_cbChecksum >= LEGACY_KID_B64ENC_LEN, DRM_E_CH_BAD_KEY );

        DRM_BYT_CopyBytes( rgbTemp,
                           0,
                           f_pChecksum->m_rgbChecksumData,
                           LEGACY_KID_B64ENC_LEN - DRM_CPHR_CB_FOR_INITDECRYPT,
                           DRM_CPHR_CB_FOR_INITDECRYPT );

        ChkDR( DRM_CPHR_InitDecrypt( pCipherContext,
                                     rgbTemp,
                                     LEGACY_KID_B64ENC_LEN ) );

        OEM_SECURE_MEMCPY( rgbTemp, f_pChecksum->m_rgbChecksumData, LEGACY_KID_B64ENC_LEN );
        ChkDR( DRM_CPHR_Decrypt( pCipherContext, NULL, LEGACY_KID_B64ENC_LEN, rgbTemp ) );

        ChkBOOL( OEM_SECURE_ARE_EQUAL( f_pChecksum->m_pbV1KID, rgbTemp, LEGACY_KID_B64ENC_LEN ), DRM_E_CH_BAD_KEY );
    }
    else if( f_pChecksum->m_cbChecksum == DRM_AES_CK_CHECKSUM_LENGTH )
    {
        DRM_BYTE rgbBlock[SIZEOF( DRM_KID )] = {0};

        ChkArg( f_cbKey == DRM_AES_KEYSIZE_128 );

        ChkDR( Oem_Aes_SetKey( f_pbKey, &aesKey ) );
        OEM_SECURE_MEMCPY( rgbBlock, f_pChecksum->m_KID.rgb, SIZEOF( DRM_KID ) );
        dr = Oem_Aes_EcbEncryptData(&aesKey, rgbBlock, SIZEOF( DRM_KID ) );
        Oem_Aes_ZeroKey( &aesKey );
        ChkDR( dr );
        ChkBOOL( OEM_SECURE_ARE_EQUAL( f_pChecksum->m_rgbChecksumData, rgbBlock, DRM_AES_CK_CHECKSUM_LENGTH ), DRM_E_CH_BAD_KEY );
    }
    else
    {
        DRM_LONG         lCount  = 0;
        DRM_SHA_CONTEXT *pShaVal = ( DRM_SHA_CONTEXT * )( f_pCryptoContext->rgbCryptoContext );

        /* Run SHA on key SHA_ITERATIONS times. */
        /* First copy key into rgbTemp. */
        if( f_cbKey > SIZEOF( rgbTemp ) )
        {
            f_cbKey = SIZEOF( rgbTemp ); /* Make sure we don't exceed buffer capacity in rgbTemp. */
        }

        OEM_SECURE_MEMCPY( rgbTemp, f_pbKey, f_cbKey );

        for( lCount = 0; lCount < SHA_ITERATIONS; lCount++ )
        {
            ChkDR( DRM_SHA_Init( pShaVal, eDRM_SHA_1 ) );
            ChkDR( DRM_SHA_Update( rgbTemp, DRM_SHA1_DIGEST_LEN + 1, pShaVal ) );
            ChkDR( DRM_SHA_Finalize( pShaVal, DRM_SHA1_DIGEST_LEN, rgbTemp ) );
        }

        ChkBOOL( OEM_SECURE_ARE_EQUAL( rgbTemp, f_pChecksum->m_rgbChecksumData, CHECKSUM_LENGTH ), DRM_E_CH_BAD_KEY );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function:    _UnwrapXmrLicenseEcc256
**
** Synopsis:    Decrypts a key using data stored in a certificate. Uses either
**              a ECC256 private key, or a symmetric key derived from this
**
** Arguments:   [f_pCryptoContext]  : The crypto context
**              [f_pUnwrapContext]  : A pointer to an initialized unwrap context
**              [f_pBindingInfo]    : A structure containing encrypted key data
**              [f_fIgnoreChecksum] : Set to TRUE if checksum should be ignored
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_KEYFILE_UNKNOWN_DECRYPTION_METHOD - if invalid decryption method
**              if specified in the f_pcbEncryptedKey struct.
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _UnwrapXmrLicenseEcc256(
    __inout_ecount( 1 )       DRM_CRYPTO_CONTEXT      *f_pCryptoContext,
    __in                      OEM_HAL_UNWRAP_CONTEXT  *f_pUnwrapContext,
    __in                const DRM_XMR_UNWRAP_INFO     *f_pBindingInfo,
    __in                      DRM_BOOL                 f_fIgnoreChecksum )
{
    DRM_RESULT                   dr                                             = DRM_SUCCESS;
    DRM_BYTE                     rgbDecKeys[ECC_P256_PLAINTEXT_SIZE_IN_BYTES]   = {0};
    DRM_BYTE                     rgbEncKeys[ECC_P256_CIPHERTEXT_SIZE_IN_BYTES]  = {0};
    DRM_DWORD                    cbDecKeys                                      = ECC_P256_PLAINTEXT_SIZE_IN_BYTES;
    DRM_BOOL                     fSymmBound                                     = FALSE;
    DRM_DWORD                    cbKey                                          = 0;
    DRM_DWORD                    dwKeyOffset                                    = 0;
    DRM_BOOL                     fDerivedRoot                                   = FALSE;
    DRM_XMR_LICENSE              trustedLicense                                 = {0};
    OEM_HAL_CRYPTO_TYPE          eDecryptionType                                = eOEM_HAL_CRYPTO_TYPE_INVALID;
    OEM_HAL_KEYREGISTER_TYPE     eWrappingKeyType                               = eOEM_HAL_KEYREGISTER_TYPE_INVALID;
    OEM_HAL_KEYREGISTER_TYPE     eTargetKeyType                                 = eOEM_HAL_KEYREGISTER_TYPE_INVALID;

    ChkPtr( f_pCryptoContext );
    ChkPtr( f_pUnwrapContext );
    ChkPtr( f_pBindingInfo );
    ChkPtr( f_pBindingInfo->pbXMRLic );
    ChkPtr( f_pBindingInfo->poChecksum );
    ChkArg( f_pBindingInfo->cbXMRLic > 0 );

    /*
    ** The unwrap context should be initialized, run a few checks anyway
    */
    ChkDR( Oem_HalOemImpl_UnwrapContextGetMetaData( f_pUnwrapContext, NULL, &eDecryptionType, &eWrappingKeyType, &eTargetKeyType ) );
    ChkArg( eWrappingKeyType == eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR );
    ChkArg( eTargetKeyType   == eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR
         || eTargetKeyType   == eOEM_HAL_KEYREGISTER_TYPE_AES128_ROOT_PAIR
         || eTargetKeyType   == eOEM_HAL_KEYREGISTER_TYPE_LEGACY );
    ChkArg( eDecryptionType  == eOEM_HAL_CRYPTO_TYPE_CHAINED_LICENSE_ECC256
         || eDecryptionType  == eOEM_HAL_CRYPTO_TYPE_SIMPLE_LICENSE_ECC256 );

    /*
    ** Locally parse the XMR license for the elements needed to unpack its keys
    */
    ChkDR( OEM_XMR_SparseUnpackLicense( f_pBindingInfo->pbXMRLic,
                                        f_pBindingInfo->cbXMRLic,
                                        &trustedLicense ) );
    ChkBOOL( XMR_IS_CONTENT_KEY_VALID( &trustedLicense ), DRM_E_INVALID_LICENSE );
    ChkBOOL( XMR_IS_SECURITY_LEVEL_VALID( &trustedLicense ), DRM_E_INVALID_LICENSE );
    ChkBOOL( XMR_IS_REVOCATION_INFORMATION_VERSION_VALID( &trustedLicense ), DRM_E_INVALID_LICENSE );

    /*
    ** Check valid license type and carry out and type-specific checks and initialization
    */
    switch( eTargetKeyType )
    {
    case eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR:
        ChkBOOL( trustedLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType
                == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR,
                 DRM_E_UNSUPPORTED_ALGORITHM );

        break;
    case eOEM_HAL_KEYREGISTER_TYPE_AES128_ROOT_PAIR:
        ChkBOOL( trustedLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType
                == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB,
                 DRM_E_UNSUPPORTED_ALGORITHM );

        fDerivedRoot = XMR_IS_AUX_KEY_VALID( &trustedLicense );
        break;

    case eOEM_HAL_KEYREGISTER_TYPE_LEGACY:
        /* No-op.  eTargetKeyType is valid but there is no additional work to do here. */
        break;

    default:
        ChkDR( DRM_E_INVALIDARG );
        break;
    }

    /*
    ** Get the encrypted content key & integrity key:
    ** If there is an optimized key, try to use that
    */
    if( trustedLicense.containerOuter.containerKeys.OptimizedContentKey.fValid
     && trustedLicense.containerOuter.containerKeys.OptimizedContentKey.wKeyEncryptionCipherType
     == XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB
     && trustedLicense.containerOuter.containerKeys.OptimizedContentKey.cbEncryptedKey
     <= SIZEOF( rgbEncKeys ) )
    {
        DRM_BYT_CopyBytes( rgbEncKeys,
                           0,
                           trustedLicense.containerOuter.containerKeys.OptimizedContentKey.pbEncryptedKeyBuffer,
                           trustedLicense.containerOuter.containerKeys.OptimizedContentKey.iEncryptedKey,
                           trustedLicense.containerOuter.containerKeys.OptimizedContentKey.cbEncryptedKey );

        cbDecKeys = trustedLicense.containerOuter.containerKeys.OptimizedContentKey.cbEncryptedKey;

        if( DRM_SUCCEEDED( Oem_HalOemImpl_WrapOrUnwrapWithSymmetricCipherRaw( FALSE,
                                                                              f_pUnwrapContext,
                                                                              NULL,
                                                                              rgbEncKeys,
                                                                              rgbDecKeys,
                                                                              cbDecKeys ) ) )
        {
            fSymmBound = TRUE;
        }
    }

    /*
    ** Fall back to asymmetric binding if symmetric binding failed
    */
    if( !fSymmBound )
    {
        /*
        ** Need to make sure the encrypted content key buffer has at least
        ** ECC_P256_CIPHERTEXT_SIZE_IN_BYTES bytes (the size of rgbEncKeys)
        ** that can be decrypted via ECC-256.
        */
        ChkBOOL( trustedLicense.containerOuter.containerKeys.ContentKey.cbEncryptedKey >= SIZEOF( rgbEncKeys ), DRM_E_CH_BAD_KEY );

        DRM_BYT_CopyBytes( rgbEncKeys,
                           0,
                           trustedLicense.containerOuter.containerKeys.ContentKey.pbEncryptedKeyBuffer,
                           trustedLicense.containerOuter.containerKeys.ContentKey.iEncryptedKey,
                           ECC_P256_CIPHERTEXT_SIZE_IN_BYTES );

        ChkDR( Oem_HalOemImpl_UnwrapXmrAesKeyPairWithEcc256PrivKey( f_pCryptoContext, f_pUnwrapContext, rgbEncKeys, rgbDecKeys ) );

        if( fDerivedRoot )
        {
            ChkDR( _UnshuffleContentKey( rgbDecKeys ) );
        }
    }

    /*
    ** Verify the signature of the license buffer
    */
    if( trustedLicense.containerOuter.signature.wType == XMR_SIGNATURE_TYPE_AES_128_OMAC )
    {
        DRM_AES_KEY aesKey;

        ChkBOOL( trustedLicense.containerOuter.signature.cbSignature == DRM_AES_BLOCKLEN, DRM_E_INVALID_SIGNATURE );

        ChkDR( Oem_Aes_SetKey( rgbDecKeys, &aesKey ) );
        dr = Oem_Omac1_Verify( &aesKey,
                               trustedLicense.pbSignedDataBuffer,
                               trustedLicense.iSignedData,
                               trustedLicense.cbSignedData,
                               trustedLicense.containerOuter.signature.pbSignatureBuffer,
                               trustedLicense.containerOuter.signature.iSignature );

        Oem_Aes_ZeroKey( &aesKey );
        ChkDR( dr );
    }
    else
    {
        ChkDR( DRM_E_UNSUPPORTED_ALGORITHM );
    }

    /*
    ** Get the size of the key for running the checksum and
    ** copy the key material to the specified key register
    */


    if( eTargetKeyType != eOEM_HAL_KEYREGISTER_TYPE_LEGACY )
    {
        /*
        ** First, make sure that the licence key type matches that which has been requested
        */


        if( eTargetKeyType == eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR )
        {
            ChkBOOL( trustedLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType
                     == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR,
                     DRM_E_UNSUPPORTED_ALGORITHM );
        }
        else if( eTargetKeyType   == eOEM_HAL_KEYREGISTER_TYPE_AES128_ROOT_PAIR )
        {
            ChkBOOL( trustedLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType
                     == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB,
                     DRM_E_UNSUPPORTED_ALGORITHM );
        }
        else
        {
            ChkDR( DRM_E_LOGICERR );
        }

        dwKeyOffset = DRM_AES_KEYSIZE_128;
        cbKey       = DRM_AES_KEYSIZE_128;

        ChkBOOL( ( ( trustedLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType
                  == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR )
                || ( trustedLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType
                  == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB ) ),
                 DRM_E_UNSUPPORTED_ALGORITHM );

        ChkDR( Oem_HalOemImpl_UnwrapXmrAesKeyPair(
            f_pUnwrapContext,
            rgbDecKeys,
            trustedLicense.containerOuter.containerGlobalPolicies.MinimumEnvironment.wMinimumSecurityLevel,
            trustedLicense.containerOuter.containerGlobalPolicies.RevocationInformationVersion.dwValue ) );

        if( fDerivedRoot )
        {
            ChkDR( Oem_HalOemImpl_UnwrapAuxKeyMaterial( f_pUnwrapContext, &trustedLicense, f_pBindingInfo->poChecksum->m_rgbChecksumData, &f_pBindingInfo->poChecksum->m_KID ) );
        }
    }
    else
    {
        /*
        ** Make sure that the licence key type matches that which has been requested
        */


        ChkBOOL( ( trustedLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType
                == XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER
                || trustedLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType
                == XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL ),
                 DRM_E_UNSUPPORTED_ALGORITHM );

        if( trustedLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER )
        {
            /*
            ** Don't checksum the size byte
            */
            dwKeyOffset = DRM_AES_KEYSIZE_128 + 1;
            cbKey       = GET_BYTE( rgbDecKeys, DRM_AES_KEYSIZE_128 );

            ChkBOOL( cbKey == DRMCIPHERKEYLEN_RC4
                  || cbKey == DRMCIPHERKEYLEN_RC4_PDRM, DRM_E_LIC_KEY_DECODE_FAILURE );
        }
        else
        {
            dwKeyOffset = DRM_AES_KEYSIZE_128;
            cbKey       = DRMCIPHERKEYLEN_RC4;
        }

        ChkDR( Oem_HalOemImpl_UnwrapXmrLegacyKey( f_pUnwrapContext,
                                                  trustedLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType,
                                                  rgbDecKeys,
                                                  cbKey + dwKeyOffset ) );
    }

    /*
    ** Verify the checksum (over content key only).
    ** Only do so for a non chained license.
    ** For a chained license verifying root's checksum is not necessary
    ** because if it fails at root it would fail at leaf also. So it can be skipped.
    */


    if( eDecryptionType == eOEM_HAL_CRYPTO_TYPE_SIMPLE_LICENSE_ECC256
     && !f_fIgnoreChecksum
     && !fDerivedRoot )
    {


        ChkDR( _VerifyChecksum( &rgbDecKeys[dwKeyOffset],
                                cbKey,
                                f_pBindingInfo->poChecksum,
                                f_pCryptoContext ) );
    }

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( rgbEncKeys, ECC_P256_PLAINTEXT_SIZE_IN_BYTES );
    OEM_SECURE_ZERO_MEMORY( rgbDecKeys, ECC_P256_PLAINTEXT_SIZE_IN_BYTES );
    return dr;
}

/*****************************************************************************
** Function:    _UnwrapXmrLicenseAesEcb
**
** Synopsis:    Decrypts a key using data stored in a certificate. Uses an
**              AES ECB key
**
** Arguments:   [f_pCryptoContext]  : The crypto context
**              [f_pUnwrapContext]  : A pointer to an initialized unwrap context
**              [f_pBindingInfo]    : A structure containing encrypted key data
**              [f_fIgnoreChecksum] : Set to TRUE if checksum should be ignored
**
** Returns:     DRM_SUCCESS          - on success
**              DRM_E_KEYFILE_UNKNOWN_DECRYPTION_METHOD - if invalid decryption method
**              if specified in the f_pcbEncryptedKey struct.
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _UnwrapXmrLicenseAesEcb(
    __inout_ecount( 1 )       DRM_CRYPTO_CONTEXT      *f_pCryptoContext,
    __in                      OEM_HAL_UNWRAP_CONTEXT  *f_pUnwrapContext,
    __in                const DRM_XMR_UNWRAP_INFO     *f_pBindingInfo,
    __in                      DRM_BOOL                 f_fIgnoreChecksum )
{
    DRM_RESULT                  dr                                              = DRM_SUCCESS;
    DRM_BYTE                    rgbDecKeys[ECC_P256_PLAINTEXT_SIZE_IN_BYTES]    = {0};
    DRM_BYTE                    rgbEncKey[DRM_AES_KEYSIZE_128]                  = {0};
    DRM_DWORD                   cbKey                                           = 0;
    DRM_DWORD                   dwKeyOffset                                     = 0;
    DRM_BYTE                    rgbMiddleKey[DRM_AES_KEYSIZE_128]               = {0};
    DRM_BOOL                    fDerivedLeaf                                    = FALSE;
    DRM_XMR_LICENSE             trustedLicense                                  = {0};
    DRM_XMR_AUX_KEY             auxKey                                          = {0};
    DRM_BYTE                    rgbSecondaryKey[DRM_AES_KEYSIZE_128]            = {0};
    DRM_KID                     oKID                                            = {0};
    DRM_BYTE                    rgbChecksumData[DRM_AES_CK_CHECKSUM_LENGTH]     = {0};
    OEM_HAL_CRYPTO_TYPE         eDecryptionType                                 = eOEM_HAL_CRYPTO_TYPE_INVALID;
    OEM_HAL_KEYREGISTER_TYPE    eWrappingKeyType                                = eOEM_HAL_KEYREGISTER_TYPE_INVALID;
    OEM_HAL_KEYREGISTER_TYPE    eTargetKeyType                                  = eOEM_HAL_KEYREGISTER_TYPE_INVALID;

    ChkPtr( f_pCryptoContext );
    ChkPtr( f_pUnwrapContext );
    ChkPtr( f_pBindingInfo );
    ChkPtr( f_pBindingInfo->pbXMRLic );
    ChkPtr( f_pBindingInfo->poChecksum );
    ChkArg( f_pBindingInfo->cbXMRLic > 0 );

    /*
    ** The unwrap context should be initialized, run a few checks anyway
    */
    ChkDR( Oem_HalOemImpl_UnwrapContextGetMetaData( f_pUnwrapContext, NULL, &eDecryptionType, &eWrappingKeyType, &eTargetKeyType ) );
    ChkArg( eWrappingKeyType == eOEM_HAL_KEYREGISTER_TYPE_AES128_ROOT_PAIR );
    ChkArg( eTargetKeyType   == eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR
         || eTargetKeyType   == eOEM_HAL_KEYREGISTER_TYPE_LEGACY );
    ChkArg( eDecryptionType  == eOEM_HAL_CRYPTO_TYPE_AES_ECB );

    OEM_SECURE_ZERO_MEMORY( rgbMiddleKey, SIZEOF( rgbMiddleKey ) );

    /*
    ** Locally parse the XMR license for the elements needed to unpack its keys
    */
    ChkDR( OEM_XMR_SparseUnpackLicense( f_pBindingInfo->pbXMRLic, f_pBindingInfo->cbXMRLic, &trustedLicense ) );

    fDerivedLeaf = XMR_IS_UPLINKX_VALID( &trustedLicense );

    if( fDerivedLeaf )
    {
        DRM_XMR_KEY_MATERIAL *pXmrContainerKeyLeaf   = NULL;

        pXmrContainerKeyLeaf = &trustedLicense.containerOuter.containerKeys;

        /*
        ** Use rgbEncKey to temporarily store the clear root CK
        */
        DRMCASSERT( SIZEOF( rgbEncKey ) >= DRM_AES_KEYSIZE_128 );

        ChkDR( Oem_HalOemImpl_UnwrapGetRootCK( f_pUnwrapContext, rgbEncKey ) );

        pXmrContainerKeyLeaf->UplinkX.pbKey = rgbMiddleKey;

        ChkDR( Oem_HalOemImpl_UnwrapGetDerivedAuxKey( f_pUnwrapContext, &auxKey ) );
        ChkDR( _CalculateKeys_Derived( rgbEncKey, &auxKey, &pXmrContainerKeyLeaf->UplinkX ) );

        DRM_BYT_CopyBytes( rgbDecKeys,
                           0,
                           trustedLicense.containerOuter.containerKeys.ContentKey.pbEncryptedKeyBuffer,
                           trustedLicense.containerOuter.containerKeys.ContentKey.iEncryptedKey,
                           DRM_AES_KEYSIZE_128_X2 );

        ChkDR( Oem_HalOemImpl_UnwrapGetKeyDerivationData( f_pUnwrapContext,
                                                          rgbSecondaryKey,
                                                          f_fIgnoreChecksum ? NULL : &oKID,
                                                          f_fIgnoreChecksum ? NULL : rgbChecksumData ) );

        ChkDR( _DecryptContentKey_Derived( rgbSecondaryKey,
                                           &pXmrContainerKeyLeaf->UplinkX,
                                           f_fIgnoreChecksum ? NULL : &oKID,
                                           f_fIgnoreChecksum ? NULL : rgbChecksumData,
                                           rgbDecKeys ) );

        pXmrContainerKeyLeaf->UplinkX.pbKey = NULL;
    }
    else
    {
        DRM_BYT_CopyBytes( rgbDecKeys,
                           0,
                           trustedLicense.containerOuter.containerKeys.ContentKey.pbEncryptedKeyBuffer,
                           trustedLicense.containerOuter.containerKeys.ContentKey.iEncryptedKey,
                           DRM_AES_KEYSIZE_128_X2 );

        ChkDR( Oem_HalOemImpl_UnwrapLeafContentKey( f_pUnwrapContext, rgbDecKeys ) );
    }

    /*
    ** Get the size of the key for running the checksum and
    ** copy the key material to the specified key register
    */
    if( eTargetKeyType == eOEM_HAL_KEYREGISTER_TYPE_AES128_PAIR )
    {
        dwKeyOffset = DRM_AES_KEYSIZE_128;
        cbKey       = DRM_AES_KEYSIZE_128;

        ChkBOOL( ( trustedLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType
                == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR ),
                 DRM_E_UNSUPPORTED_ALGORITHM );

        ChkDR( Oem_HalOemImpl_UnwrapXmrAesKeyPair(
            f_pUnwrapContext,
            rgbDecKeys,
            trustedLicense.containerOuter.containerGlobalPolicies.MinimumEnvironment.wMinimumSecurityLevel,
            trustedLicense.containerOuter.containerGlobalPolicies.RevocationInformationVersion.dwValue ) );
    }
    else if( eTargetKeyType == eOEM_HAL_KEYREGISTER_TYPE_LEGACY )
    {
        ChkBOOL( ( trustedLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType
                == XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER
                || trustedLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType
                == XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL ),
                 DRM_E_UNSUPPORTED_ALGORITHM );

        if( trustedLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_RC4_CIPHER )
        {
            /*
            ** Don't checksum the size byte
            */
            dwKeyOffset = DRM_AES_KEYSIZE_128 + 1;
            cbKey       = GET_BYTE( rgbDecKeys, DRM_AES_KEYSIZE_128 );

            ChkBOOL( cbKey == DRMCIPHERKEYLEN_RC4
                  || cbKey == DRMCIPHERKEYLEN_RC4_PDRM, DRM_E_LIC_KEY_DECODE_FAILURE );
        }
        else
        {
            dwKeyOffset = DRM_AES_KEYSIZE_128;
            cbKey       = DRMCIPHERKEYLEN_RC4;
        }

        ChkDR( Oem_HalOemImpl_UnwrapXmrLegacyKey( f_pUnwrapContext,
                                                  trustedLicense.containerOuter.containerKeys.ContentKey.wSymmetricCipherType,
                                                  rgbDecKeys,
                                                  cbKey + dwKeyOffset ) );
    }
    else
    {
        ChkDR( DRM_E_UNSUPPORTED_ALGORITHM );
    }

    /*
    ** Verify the checksum (over content key only).
    ** Only do so for non derived key based leaf license
    ** since derived key based leaf license verifies checksum
    ** inside of _DecryptContentKey_Derived().
    */
    if( !f_fIgnoreChecksum && !fDerivedLeaf )
    {


        ChkDR( _VerifyChecksum( &rgbDecKeys[DRM_AES_KEYSIZE_128],
                                cbKey,
                                f_pBindingInfo->poChecksum,
                                f_pCryptoContext ) );
    }

    /*
    ** Verify the signature of the license buffer
    */
    if( trustedLicense.containerOuter.signature.wType == XMR_SIGNATURE_TYPE_AES_128_OMAC )
    {
        DRM_AES_KEY aesKey = {0};

        ChkBOOL( trustedLicense.containerOuter.signature.cbSignature == DRM_AES_BLOCKLEN,
                 DRM_E_INVALID_SIGNATURE );


        ChkDR( Oem_Aes_SetKey(rgbDecKeys, &aesKey) );
        dr = Oem_Omac1_Verify( &aesKey,
                               trustedLicense.pbSignedDataBuffer,
                               trustedLicense.iSignedData,
                               trustedLicense.cbSignedData,
                               trustedLicense.containerOuter.signature.pbSignatureBuffer,
                               trustedLicense.containerOuter.signature.iSignature );
        Oem_Aes_ZeroKey(&aesKey);
        ChkDR( dr );
    }
    else
    {
        ChkDR( DRM_E_UNSUPPORTED_ALGORITHM );
    }

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( rgbDecKeys,      SIZEOF( rgbDecKeys ) );
    OEM_SECURE_ZERO_MEMORY( rgbEncKey,       SIZEOF( rgbEncKey ) );
    OEM_SECURE_ZERO_MEMORY( rgbMiddleKey,    SIZEOF( rgbMiddleKey ) );
    OEM_SECURE_ZERO_MEMORY( &auxKey,         SIZEOF( auxKey ) );
    OEM_SECURE_ZERO_MEMORY( rgbSecondaryKey, SIZEOF( rgbSecondaryKey ) );
    return dr;
}

/*****************************************************************************
** Function:    _UnwrapEcc256PublicKey
**
** Synopsis:    Loads a public key into a register with validation
**
** Arguments:   [f_pCryptoContext]   : A pointer to a crypto context
**              [f_pUnwrapContext]   : A pointer to an initialized unwrap context
**              [f_pbWrappedKeyData] : A buffer containing key data
**              [f_cbWrappedKeyData] : The length of the data in f_pbWrappedKeyData
**              [f_dwKeyIndex]       : Index of the key within the certificate to unwrap
**
** Returns:     DRM_SUCCESS                - on success
**              DRM_E_OEMHAL_BAD_ENCODING  - the key data isn't valid
**
** Notes:       None.
**
******************************************************************************/
static DRM_RESULT _UnwrapEcc256PublicKey(
    __inout_ecount( 1 )                     DRM_CRYPTO_CONTEXT          *f_pCryptoContext,
    __in                                    OEM_HAL_UNWRAP_CONTEXT      *f_pUnwrapContext,
    __in_bcount( f_cbWrappedKeyData ) const DRM_BYTE                    *f_pbWrappedKeyData,
    __in                                    DRM_DWORD                    f_cbWrappedKeyData,
    __in                                    DRM_DWORD                    f_dwKeyIndex )
{
    DRM_RESULT                        dr                        = DRM_SUCCESS;
    DRM_BCERT_CERTIFICATE             certificate               = {0};
    DRM_DWORD                         dwOffset                  = 0;
    OEM_HAL_PUBKEY_VERIFICATION_STATE eVerificationState        = eOEM_HAL_PUBKEY_NOT_VERIFIED;
    OEM_HAL_KEYREGISTER_TYPE          eWrappingKeyType          = eOEM_HAL_KEYREGISTER_TYPE_INVALID;
    OEM_HAL_KEYREGISTER_TYPE          eTargetKeyType            = eOEM_HAL_KEYREGISTER_TYPE_INVALID;
    PUBKEY_P256                       oKey                      = {0};
    DRM_DWORD                         dwSecurityLevel           = 0;
    DRM_DWORD                         dwKeyUsageBitmap          = 0;
    DRM_BOOL                          fRootKey                  = FALSE;
    DRM_DWORD                         idx                       = 0;
    DRM_DWORD                         dwFullCertKeyUsageBitmap  = 0;
    DRM_BOOL                          fReceiverFeature          = FALSE;

    ChkDR( Oem_HalOemImpl_UnwrapContextGetMetaData( f_pUnwrapContext, NULL, NULL, &eWrappingKeyType, &eTargetKeyType ) );
    ChkPtr( f_pbWrappedKeyData );
    ChkArg( eTargetKeyType   == eOEM_HAL_KEYREGISTER_TYPE_P256_PUBLIC );
    ChkArg( eWrappingKeyType == eOEM_HAL_KEYREGISTER_TYPE_P256_PUBLIC );
    ChkDR( Oem_HalOemImpl_UnwrapGetWrappingEcc256PublicKeyData( f_pUnwrapContext, &oKey, &dwSecurityLevel, &dwKeyUsageBitmap, &fRootKey ) );

    /*
    ** Parse the certificate to extract the required information
    */
    ChkDR( DRM_BCert_GetCertificate( f_pbWrappedKeyData,
                                     f_cbWrappedKeyData,
                                     &dwOffset,
                                     &certificate,
                                     DRM_BCERT_CERTTYPE_UNKNOWN ) );

    /*
    ** Check that basic parameters/values are valid
    */
    ChkArg( f_dwKeyIndex                           <  certificate.KeyInfo.dwNumKeys );
    ChkArg( certificate.Header.cbCertificate       <= f_cbWrappedKeyData );
    ChkArg( certificate.Header.cbCertificateSigned <  certificate.Header.cbCertificate );
    ChkArg( certificate.Header.cbCertificateSigned >  0 );

    /*
    ** Check the unwrapping key matches that specified in the certificate
    */
    ChkArg( OEM_SECURE_ARE_EQUAL( &oKey, certificate.SignatureInfo.pIssuerKey, SIZEOF( PUBKEY_P256 ) ) );

    /*
    ** Check that the signature on this certificate is valid
    */
    ChkDR( OEM_ECDSA_Verify_P256(
            f_pbWrappedKeyData,
            certificate.Header.cbCertificateSigned,
            &oKey,
            (SIGNATURE_P256 *)certificate.SignatureInfo.SignatureData.pValue,
            (struct bigctx_t *)f_pCryptoContext->rgbCryptoContext ) );

    /*
    ** Check that the parent key is allowed to sign this key
    */
    ChkDR( DRM_BCert_VerifyChildUsage( certificate.KeyInfo.rgoKeys[f_dwKeyIndex].dwUsageSet, dwKeyUsageBitmap ) );

    /*
    ** If this is not a CRL chain, then check that this certificate has not been revoked
    */
    if( !_PublicKeyUsageContains( certificate.KeyInfo.rgoKeys[f_dwKeyIndex].dwUsageSet, DRM_BCERT_KEYUSAGE_SIGN_CRL )
     && !_PublicKeyUsageContains( dwKeyUsageBitmap,                                     DRM_BCERT_KEYUSAGE_ISSUER_CRL ) )
    {
        if( OEM_HAL_IsHalDevCertValidationSupported() )
        {
            ChkDR( _CheckCertificateRevocation( &certificate.BasicInfo.DigestValue, 1, &eVerificationState ) );

            if( eVerificationState == eOEM_HAL_PUBKEY_REVOKED )
            {
                ChkDR( DRM_E_CERTIFICATE_REVOKED );
            }
        }
    }

    /*
    ** Check that the signing key has a greater than or equal security level to the target key
    */
    if( !fRootKey )
    {
        ChkArg( dwSecurityLevel >= certificate.BasicInfo.dwSecurityLevel );
    }

    /*
    ** Store all key usages for all keys in this certificate
    */
    for( idx = 0; idx < certificate.KeyInfo.dwNumKeys; idx++ )
    {
        dwFullCertKeyUsageBitmap |= certificate.KeyInfo.rgoKeys[idx].dwUsageSet;
    }

    /*
    ** Remove the PRND key usage if the certificate is not a PRND receiver
    ** Binary certificates may have the PRND key without actually being a receiver
    ** Also remove the PRND key usage if the certificate is not PC or Device,
    ** as other certificates can't actually be PRND receivers.
    */
    fReceiverFeature = FALSE;
    for( idx = 0; idx < certificate.FeatureInfo.dwNumFeatureEntries && !fReceiverFeature; idx++ )
    {
        if( certificate.FeatureInfo.rgdwFeatureSet[ idx ] == DRM_BCERT_FEATURE_RECEIVER )
        {
            fReceiverFeature = TRUE;
        }
    }
    switch( certificate.BasicInfo.dwType )
    {
    case DRM_BCERT_CERTTYPE_PC:         __fallthrough;
    case DRM_BCERT_CERTTYPE_DEVICE:
        /* no-op */
        break;
    default:
        fReceiverFeature = FALSE;
        break;
    }
    if( !fReceiverFeature )
    {
        dwFullCertKeyUsageBitmap &= ~( BCERT_KEYUSAGE_BIT( DRM_BCERT_KEYUSAGE_PRND_ENCRYPT_KEY ) );
    }

    /*
    ** The certificate is valid - extract the key and related information
    */
    ChkDR( Oem_HalOemImpl_UnwrapSetTargetEcc256PublicKeyData( f_pUnwrapContext,
                                                              certificate.KeyInfo.rgoKeys[f_dwKeyIndex].pValue,
                                                              certificate.BasicInfo.dwSecurityLevel,
                                                              certificate.KeyInfo.rgoKeys[f_dwKeyIndex].dwUsageSet,
                                                              dwFullCertKeyUsageBitmap,
                                                              &certificate.BasicInfo.DigestValue,
                                                              eVerificationState ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_UnwrapKey
**
** Synopsis: This function unwraps a key, wrapped using another key, and loads it into a key register.
**           NOTE: after this function is successfully called, a call must be made to Oem_Hal_FreeRegister
**           to free resources associated with the key
**
** Arguments:
**
** [f_pCryptoContext]   : The crypto context.
** [f_eKeyType]         : The type of key to be unwrapped.
** [f_indexKey]         : The index of the register in which to place the key.
** [f_eWrappingKeyType] : The type of the wrapping key.
** [f_indexWrappingKey] : The index of the wrapping key.
** [f_pbWrappedKeyData] : A pointer to the buffer containing the data required
**                        to unpack the wrapped key (this is key dependent).
** [f_cbWrappedKeyData] : The length (in bytes) of the wrappedKey data.
** [f_pbParameterData]  : Parameter data associated with the particular unwrap type.
** [f_cbParameterData]  : Size of parameter data.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_INVALID_PARAMETER
**             DRM_E_OEMHAL_BAD_KEY_REGISTER_INDEX
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE
**             DRM_E_OEMHAL_UNSUPPORTED_KEY_LENGTH
**             DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_UnwrapKey(
    __inout_ecount_opt( 1 )                    DRM_CRYPTO_CONTEXT         *f_pCryptoContext,
    __in                                       OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                                       OEM_HAL_KEY_REGISTER_INDEX  f_indexKey,
    __in                                       OEM_HAL_KEY_TYPE            f_eWrappingKeyType,
    __in                                       OEM_HAL_KEY_REGISTER_INDEX  f_indexWrappingKey,
    __in_bcount( f_cbWrappedKeyData )    const DRM_BYTE                   *f_pbWrappedKeyData,
    __in                                       DRM_DWORD                   f_cbWrappedKeyData,
    __in_bcount_opt( f_cbParameterData ) const DRM_BYTE                   *f_pbParameterData,
    __in                                       DRM_DWORD                   f_cbParameterData )
{
    DRM_RESULT                   dr                         = DRM_SUCCESS;
    OEM_HAL_UNWRAP_CONTEXT       unwrapContext              = OEM_HAL_UNWRAP_CONTEXT_INVALID;
    OEM_HAL_KEYDATA_TYPE         eKeyDataType               = eOEM_HAL_KEYDATA_TYPE_INVALID;
    OEM_HAL_CRYPTO_TYPE          eDecryptionType            = eOEM_HAL_CRYPTO_TYPE_INVALID;
    OEM_HAL_KEYREGISTER_TYPE     eWrappingKeyRegisterType   = eOEM_HAL_KEYREGISTER_TYPE_INVALID;
    OEM_HAL_KEY_TYPE             eKeyType                   = f_eKeyType;           /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX   indexKey                   = f_indexKey;           /* TOC/TOU */
    OEM_HAL_KEY_TYPE             eWrappingKeyType           = f_eWrappingKeyType;   /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX   indexWrappingKey           = f_indexWrappingKey;   /* TOC/TOU */
    DRM_BYTE                    *pbWrappedKeyData           = NULL;                 /* TOC/TOU */
    DRM_DWORD                    cbWrappedKeyData           = f_cbWrappedKeyData;   /* TOC/TOU */
    DRM_BYTE                    *pbParameterData            = NULL;                 /* TOC/TOU */
    DRM_DWORD                    cbParameterData            = f_cbParameterData;    /* TOC/TOU */

    DECLARE_LOCAL_CRYPTO_CONTEXT();

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( eKeyType, indexKey ) );
    ChkPtr( f_pbWrappedKeyData );

    ALLOC_LOCAL_CRYPTO_CONTEXT();

    ChkMem( pbWrappedKeyData = ( DRM_BYTE* )Oem_MemAlloc( cbWrappedKeyData ) );
    OEM_SECURE_MEMCPY( pbWrappedKeyData, f_pbWrappedKeyData, cbWrappedKeyData );
    if( f_pbParameterData != NULL && cbParameterData > 0 )
    {
        ChkMem( pbParameterData = ( DRM_BYTE* )Oem_MemAlloc( cbParameterData ) );
        OEM_SECURE_MEMCPY( pbParameterData, f_pbParameterData, cbParameterData );
    }

    /*
    ** Some wrap operations allow using an invalid key to pass the a out in the clear.
    ** _InitializeUnwrapContext will ensure that this only happens for permitted
    ** key types
    */
    if( eWrappingKeyType != OEM_HAL_KEY_TYPE_INVALID )
    {
        ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( eWrappingKeyType, indexWrappingKey ) );
    }

    /*
    ** Initialize the Unwrap Context, verifying that this is a valid combination of keys
    */
    ChkDR( _InitializeUnwrapContext( eKeyType,
                                     indexKey,
                                     eWrappingKeyType,
                                     indexWrappingKey,
                                     cbWrappedKeyData,
                                     pbParameterData,
                                     cbParameterData,
                                     &unwrapContext ) );

    ChkDR( Oem_HalOemImpl_UnwrapContextGetMetaData( &unwrapContext, &eKeyDataType, &eDecryptionType, &eWrappingKeyRegisterType, NULL ) );


    switch( eKeyDataType )
    {
    case eOEM_HAL_KEYDATA_TYPE_DRM_ENCRYPTED_KEY:
        ChkArg( cbWrappedKeyData == SIZEOF( DRM_ENCRYPTED_KEY ) );
        ChkDR( Oem_HalOemImpl_UnwrapEncryptedKey( eKeyType, &unwrapContext, ( DRM_ENCRYPTED_KEY* )pbWrappedKeyData ) );
        break;

    case eOEM_HAL_KEYDATA_TYPE_ENCRYPTED_BYTES:
        switch( eDecryptionType )
        {
        case eOEM_HAL_CRYPTO_TYPE_DERIVED_SYMMETRIC_KEY:
            ChkDR( Oem_HalOemImpl_UnwrapWithSymmetricCipher( &unwrapContext, pbWrappedKeyData, cbWrappedKeyData ) );
            break;

        case eOEM_HAL_CRYPTO_TYPE_AES_KEY_PAIR:
            ChkDR( Oem_HalOemImpl_UnwrapAesKeyPairWithEcc256PrivKey( USE_LOCAL_CRYPTO_CONTEXT, &unwrapContext, pbWrappedKeyData, cbWrappedKeyData ) );
            break;

        case eOEM_HAL_CRYPTO_TYPE_TWO_STAGE_ECB:
            ChkDR( Oem_HalOemImpl_UnwrapEcc256WithAesKeyPair( &unwrapContext, pbWrappedKeyData, cbWrappedKeyData ) );
            break;

        case eOEM_HAL_CRYPTO_TYPE_AES_PADDED_KEY:
            ChkDR( Oem_HalOemImpl_UnwrapAesPaddedKeyWithEcc256PrivKey( USE_LOCAL_CRYPTO_CONTEXT, &unwrapContext, pbWrappedKeyData, cbWrappedKeyData ) );
            break;

        case eOEM_HAL_CRYPTO_TYPE_AES_ECB:
            ChkDR( Oem_HalOemImpl_UnwrapAesKeyWithAesEcb( &unwrapContext, pbWrappedKeyData, cbWrappedKeyData ) );
            break;

        default:
            /*
            ** Should never reach here - set return code to DRM_E_LOGICERR
            */
            DRMASSERT( FALSE );
            ChkDR( DRM_E_LOGICERR );
            break;
        }
        break;

    case eOEM_HAL_KEYDATA_TYPE_XMR_LICENSE:
        ChkArg( cbWrappedKeyData == SIZEOF( DRM_XMR_UNWRAP_INFO ) );
        ChkArg( cbParameterData  == SIZEOF( DRM_BOOL ) );
        ChkPtr( pbParameterData );


        switch( eWrappingKeyRegisterType )
        {
        case eOEM_HAL_KEYREGISTER_TYPE_P256_KEYPAIR:
            ChkDR( _UnwrapXmrLicenseEcc256( USE_LOCAL_CRYPTO_CONTEXT,
                                            &unwrapContext,
                                            ( DRM_XMR_UNWRAP_INFO* )pbWrappedKeyData,
                                            *( ( DRM_BOOL* )pbParameterData ) ) );
            break;

        case eOEM_HAL_KEYREGISTER_TYPE_AES128_ROOT_PAIR:
            ChkDR( _UnwrapXmrLicenseAesEcb( USE_LOCAL_CRYPTO_CONTEXT,
                                            &unwrapContext,
                                            ( DRM_XMR_UNWRAP_INFO* )pbWrappedKeyData,
                                            *( ( DRM_BOOL* )pbParameterData ) ) );
            break;

        default:
            /*
            ** Should never reach here - set return code to DRM_E_LOGICERR
            */
            DRMASSERT( FALSE );
            ChkDR( DRM_E_LOGICERR );
            break;
        }
        break;

    case eOEM_HAL_KEYDATA_TYPE_XMR_CERT:
        ChkPtr( pbParameterData );
        ChkArg( cbParameterData == SIZEOF( DRM_DWORD ) );

        ChkDR( _UnwrapEcc256PublicKey( USE_LOCAL_CRYPTO_CONTEXT,
                                       &unwrapContext,
                                       pbWrappedKeyData,
                                       cbWrappedKeyData,
                                       *( ( DRM_DWORD* )pbParameterData ) ) );
        break;

    default:
        /*
        ** Should never reach here - set return code to DRM_E_LOGICERR
        */
        DRMASSERT( FALSE );
        ChkDR( DRM_E_LOGICERR );
        break;
    }















ErrorExit:
    FREE_LOCAL_CRYPTO_CONTEXT();
    SAFE_OEM_FREE( pbWrappedKeyData );
    SAFE_OEM_FREE( pbParameterData );
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_WrapKey
**
** Synopsis: This function wraps a key (in a key register) with a wrapping key (typically also in a key
**           register) and places the wrapped key in a buffer.
**
** Arguments:
**
** [f_pOEMContext]      : The OEM context.
** [f_pCryptoContext]   : The crypto context.
** [f_eKeyType]         : The type of the key to be wrapped.
** [f_indexKey]         : The index of the key to be wrapped.
** [f_eWrappingKeyType] : The type of the wrapping key.
** [f_indexWrappingKey] : The index of the wrapping key.
** [f_pbWrappedKey]     : A pointer to the buffer to contain the wrapped key.
**                        If the buffer is too small to contain the wrapped key, the function returns an error
**                        indicating that, and updates the location with the buffer size required.
** [f_pcbWrappedKey]    : A pointer to a location holding the length (in bytes)
**                        of the wrappedKey buffer. On input, this parameter holds the length of the wrappedKey buffer.
**                        On output, this parameter indicates the number of bytes written to the wrappedKey buffer.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_INVALID_PARAMETER
**             DRM_E_OEMHAL_BAD_KEY_REGISTER_INDEX
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE
**             DRM_E_OEMHAL_UNSUPPORTED_KEY_WRAPPING_FORMAT
**             DRM_E_BUFFERTOOSMALL
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_WrapKey(
    __in                                 DRM_VOID                   *f_pOEMContext,
    __inout_ecount_opt( 1 )              DRM_CRYPTO_CONTEXT         *f_pCryptoContext,
    __in                                 OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                                 OEM_HAL_KEY_REGISTER_INDEX  f_indexKey,
    __in                                 OEM_HAL_KEY_TYPE            f_eWrappingKeyType,
    __in                                 OEM_HAL_KEY_REGISTER_INDEX  f_indexWrappingKey,
    __out_bcount_opt( *f_pcbWrappedKey ) DRM_BYTE                   *f_pbWrappedKey,
    __inout_ecount(1)                    DRM_DWORD                  *f_pcbWrappedKey )
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    OEM_HAL_KEY_TYPE             eKeyType           = f_eKeyType;           /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX   indexKey           = f_indexKey;           /* TOC/TOU */
    OEM_HAL_KEY_TYPE             eWrappingKeyType   = f_eWrappingKeyType;   /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX   indexWrappingKey   = f_indexWrappingKey;   /* TOC/TOU */
    DRM_DWORD                    cbWrappedKey       = 0;                    /* TOC/TOU */
    DECLARE_LOCAL_CRYPTO_CONTEXT();

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( eKeyType, indexKey ) );
    ChkPtr( f_pcbWrappedKey );
    cbWrappedKey = *f_pcbWrappedKey;

    ALLOC_LOCAL_CRYPTO_CONTEXT();

    ChkDR( _Hal_WrapKey(
        f_pOEMContext,
        USE_LOCAL_CRYPTO_CONTEXT,
        eKeyType,
        indexKey,
        eWrappingKeyType,
        indexWrappingKey,
        f_pbWrappedKey,
        &cbWrappedKey ) );

ErrorExit:
    FREE_LOCAL_CRYPTO_CONTEXT();
    if( f_pcbWrappedKey != NULL )
    {
        *f_pcbWrappedKey = cbWrappedKey;
    }
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_GenerateKey
**
** Synopsis: This function generates a random key of a given type and places it into a key register.
**           NOTE: after this function is successfully called, a call must be made to Oem_Hal_FreeRegister
**           to free resources associated with the key
**
** Arguments:
**
** [f_pOEMContext]              : The OEM context.
** [f_pCryptoContext]           : The crypto context.
** [f_eKeyType]                 : The type of key to be generated.
** [f_indexKey]                 : The key index to fill with the newly generated key.
** [f_dwRequestedSecurityLevel] : The requested security level of the key.
**                                Ignored unless f_eKeyType == OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT
**                                            || f_eKeyType == OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_INVALID_PARAMETER
**             DRM_E_OEMHAL_BAD_KEY_REGISTER_INDEX
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_GenerateKey(
    __in                    DRM_VOID                  *f_pOEMContext,
    __inout_ecount_opt( 1 ) DRM_CRYPTO_CONTEXT        *f_pCryptoContext,
    __in                    OEM_HAL_KEY_TYPE           f_eKeyType,
    __in                    OEM_HAL_KEY_REGISTER_INDEX f_indexKey,
    __in                    DRM_DWORD                  f_dwRequestedSecurityLevel )
{
    DRM_RESULT                   dr                         = DRM_SUCCESS;
    OEM_HAL_KEY_TYPE             eKeyType                   = f_eKeyType;                   /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX   indexKey                   = f_indexKey;                   /* TOC/TOU */
    DRM_DWORD                    dwRequestedSecurityLevel   = f_dwRequestedSecurityLevel;   /* TOC/TOU */

    DECLARE_LOCAL_CRYPTO_CONTEXT();

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkDR( Oem_HalOemImpl_VerifyValidKeyRegister( eKeyType, indexKey ) );

    ALLOC_LOCAL_CRYPTO_CONTEXT();

    Oem_HalOemImpl_ZeroKeyRegister( eKeyType, indexKey );

    switch( eKeyType )
    {
    case OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT:
    case OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT:
    case OEM_HAL_KEY_TYPE_ECC_P256_SIGN:
        ChkDR( Oem_HalOemImpl_GenerateEcc256KeyPair( USE_LOCAL_CRYPTO_CONTEXT, eKeyType, indexKey ) );
        break;

    case OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT:
    case OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT:
        ChkDR( Oem_HalOemImpl_GenerateAes128NewLicenseKeyPair( f_pOEMContext, USE_LOCAL_CRYPTO_CONTEXT, eKeyType, indexKey, dwRequestedSecurityLevel ) );
        break;

    case OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT:
        ChkDR( Oem_HalOemImpl_GenerateAes128KeyPair( f_pOEMContext, USE_LOCAL_CRYPTO_CONTEXT, eKeyType, indexKey ) );
        break;

    case OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE:
    case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT:
    case OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT:
        ChkDR( Oem_HalOemImpl_GenerateAes128Key( f_pOEMContext, eKeyType, indexKey ) );
        break;

    default:
        ChkDR( DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );
    }

ErrorExit:
    FREE_LOCAL_CRYPTO_CONTEXT();
    return dr;
}

#if 0
//Added For Get Content Key
DRM_API DRM_RESULT DRM_CALL Oem_Hal_GetContentKey(
    __in_ecount( 1 )           DRM_CIPHER_CONTEXT           *f_pCipherContext,
    __in                       OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                       OEM_HAL_KEY_REGISTER_INDEX  f_indexContentKey)
{
    OEM_HAL_KEY_TYPE            eKeyType                = f_eKeyType;               /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX  indexContentKey         = f_indexContentKey;        /* TOC/TOU */

	Oem_HalOemImpl_GetContentKey(f_pCipherContext, eKeyType, indexContentKey);
}
#endif
/*********************************************************************
**
** Function: Oem_Hal_DecryptContent
**
** Synopsis: Decrypts PlayReady content using AES in counter mode with the key in the indicated content key
**           register.
**
** Arguments:
**
** [f_pbData]                   : A pointer to a buffer to be decrypted in place
** [f_cbData]                   : The length of the buffer pointed to by f_pbData
** [f_eKeyType]                 : The type of the content key to use for decryption of the source data.
** [f_indexContentKey]          : The register index of the content key to use for decryption of the source data.
** [f_qwInitializationVector]   : A 64 bit integer specifying the IV of the sample to
**                                be decrypted and decoded.  This IV is used to initialize the upper 64 bits of the 128 bit
**                                counter used when decrypting the sample.  (The lower 64 bits of the counter are initialized
**                                according to the sampleByteOffset field as described below).
** [f_qwSampleByteOffset]       : A 64 bit integer specifying the byte offset (from the
**                                start of the sample) of the first byte of the payload. This value, divided by 16, is used to
**                                initialize the lower half of the 128 bit counter used when decrypting the sample. If the
**                                sampleByteOffset modulo 16 has a value N != 0, then the first N bytes of counter mode keystream
**                                must be discarded before XORing it with the encrypted payload. (Equivalently, N arbitrary bytes
**                                can be prepended to the encrypted payload and the first N bytes of the plaintext discarded).
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_BAD_KEY_REGISTER_INDEX
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_BUFFER_TOO_LARGE
**             DRM_E_BUFFERTOOSMALL
**             DRM_E_OEMHAL_CONTENT_BLACKED_OUT
**             DRM_E_OEMHAL_NO_KEY_IN_REGISTER
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OUTOFMEMORY
**
** Notes:      Decrypt content using the content key register index provided.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_DecryptContent(
    __inout_bcount( f_cbData ) DRM_BYTE                   *f_pbData,
    __in                       DRM_DWORD                   f_cbData,
    __in                       OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                       OEM_HAL_KEY_REGISTER_INDEX  f_indexContentKey,
    __in                       DRM_UINT64                  f_qwInitializationVector,
    __in                       DRM_UINT64                  f_qwSampleByteOffset )
{
    DRM_RESULT                  dr                      = DRM_SUCCESS;
    OEM_HAL_KEY_TYPE            eKeyType                = f_eKeyType;               /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX  indexContentKey         = f_indexContentKey;        /* TOC/TOU */
    DRM_UINT64                  qwInitializationVector  = f_qwInitializationVector; /* TOC/TOU */
    DRM_UINT64                  qwSampleByteOffset      = f_qwSampleByteOffset;     /* TOC/TOU */

    /*
    ** Performance: Do not make a copy f_pbData / f_cbData.  No TOC/TOU attack exists for this data.
    ** If modified by caller after being passed in, garbage data will come out for the decrypted data.
    */

    ChkDR( Oem_HalOemImpl_DecryptContent( f_pbData, f_cbData, eKeyType, indexContentKey, qwInitializationVector, qwSampleByteOffset ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_DecryptContentOpaque
**
** Synopsis: Decrypts PlayReady content using AES in counter mode with the key in the indicated content key
**           register.
**
** Arguments:
**
** [f_hInBuf]                   : An opaque handle to an OEM memory structure containing input samples to be decrypted.
** [f_hOutBuf]                  : An opaque handle to an OEM memory structure containing output samples in the clear.
** [f_cbData]                   : The length of the buffer pointed to by f_hInBuf and f_hOutBuf.
** [f_eKeyType]                 : The type of the content key to use for decryption of the source data.
** [f_indexContentKey]          : The register index of the content key to use for decryption of the source data.
** [f_qwInitializationVector]   : A 64 bit integer specifying the IV of the sample to
**                                be decrypted and decoded.  This IV is used to initialize the upper 64 bits of the 128 bit
**                                counter used when decrypting the sample.  (The lower 64 bits of the counter are initialized
**                                according to the sampleByteOffset field as described below).
** [f_qwSampleByteOffset]       : A 64 bit integer specifying the byte offset (from the
**                                start of the sample) of the first byte of the payload. This value, divided by 16, is used to
**                                initialize the lower half of the 128 bit counter used when decrypting the sample. If the
**                                sampleByteOffset modulo 16 has a value N != 0, then the first N bytes of counter mode keystream
**                                must be discarded before XORing it with the encrypted payload. (Equivalently, N arbitrary bytes
**                                can be prepended to the encrypted payload and the first N bytes of the plaintext discarded).
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_BAD_KEY_REGISTER_INDEX
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_BUFFER_TOO_LARGE
**             DRM_E_BUFFERTOOSMALL
**             DRM_E_OEMHAL_CONTENT_BLACKED_OUT
**             DRM_E_OEMHAL_NO_KEY_IN_REGISTER
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OUTOFMEMORY
**
** Notes:      Decrypt content using the content key register index provided.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_DecryptContentOpaque(
    __in                       OEM_OPAQUE_BUFFER_HANDLE    f_hInBuf,
    __in                       OEM_OPAQUE_BUFFER_HANDLE    f_hOutBuf,
    __in                       DRM_DWORD                   f_cbData,
    __in                       OEM_HAL_KEY_TYPE            f_eKeyType,
    __in                       OEM_HAL_KEY_REGISTER_INDEX  f_indexContentKey,
    __in                       DRM_UINT64                  f_qwInitializationVector,
    __in                       DRM_UINT64                  f_qwSampleByteOffset )
{
    DRM_RESULT                  dr                      = DRM_SUCCESS;
    DRM_DWORD                   cbData                  = f_cbData;                 /* TOC/TOU */
    OEM_HAL_KEY_TYPE            eKeyType                = f_eKeyType;               /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX  indexContentKey         = f_indexContentKey;        /* TOC/TOU */
    DRM_UINT64                  qwInitializationVector  = f_qwInitializationVector; /* TOC/TOU */
    DRM_UINT64                  qwSampleByteOffset      = f_qwSampleByteOffset;     /* TOC/TOU */

    /*
    ** Performance: Do not make a copy f_hInBuf / f_hOutBuf.  No TOC/TOU attack exists for this data.
    ** If modified by caller after being passed in, garbage data will come out for the encrypted data.
    */

    ChkDR( Oem_HalOemImpl_DecryptContentOpaque( f_hInBuf, f_hOutBuf, cbData, eKeyType, indexContentKey, qwInitializationVector, qwSampleByteOffset ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_CreateSampleEncryptionContext
**
** Synopsis: Creates a sample encryption context that can, after it is has been populated appropriately,
**           designate the input for a series of encryption operations covering one or several samples.
**           The sample encryption context itself is held in secure memory, and cannot be directly
**           manipulated by application code.
**
** Arguments:
**
** [f_eKeyType]                 : The type of the content key to be used for encryption of the source data.
** [f_indexKey]                 : The register index of the content key to be used for encryption of the source data.
** [f_eSampleEncryptionMode]    : the encryption mode requested for this context. This mode
**                                may not be available according to policy, in which case
**                                DRM_E_OEMHAL_SAMPLE_ENCRYPTION_MODE_NOT_PERMITTED will be returned (see Remarks).
** [f_phEncryptionContext]      : A pointer to a handle which will be filled with a reference to the newly created sample encryption context.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_BAD_KEY_REGISTER_INDEX
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OUTOFMEMORY
**             DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE
**             DRM_E_OEMHAL_SAMPLE_ENCRYPTION_MODE_NOT_PERMITTED
**
** Notes:      Following its creation, a sample encryption context can be populated with references to the
**             data to be encrypted (and with certain other metadata governing the encryption process) by a
**             secure data source, such as a hardware transcoder. This maintains the secure data path.
**             The crypto HAL implementation will generate or derive a suitable 64-bit sample id for each
**             sample. For the first sample this will be random. For subsequent samples, this will be
**             derived by incrementing the previous one. At most 2**20 samples can be encrypted using a
**             single context.
**             The detailed layout of the context in secure memory is platform-dependent, and is a matter
**             for bilateral agreement between the implementors of the crypto HAL and the secure data source.
**             However it is expected that the context will contain at least the information in the
**             OEM_HAL_ENCRYPTION_CONTEXT_INTERNAL structure.
**             The encryption mode requested in the requestedMode parameter may not be available according
**             to the policy enforced by the crypto HAL implementation for a particular platform. It is
**             expected that such restrictions will be made known to the application developers affected.
**             Here are some examples of policy restricting the use of certain modes:
**                OEM_HAL_SAMPLE_ENCRYPTION_MODE_PASSTHRU is only permitted if the source data
**                   (e.g. before transcoding) was unencrypted;
**                OEM_HAL_SAMPLE_ENCRYPTION_MODE_FULL is permitted unless adherence to one of the
**                   subsample encryption standards is mandated;
**                OEM_HAL_SAMPLE_ENCRYPTION_MODE_SUBSAMPLE_CENC is permitted only for PlayReady
**                   encryption, and only if CFF is not required;
**                OEM_HAL_SAMPLE_ENCRYPTION_MODE_SUBSAMPLE_CFF is permitted only for PlayReady encryption.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_CreateSampleEncryptionContext(
    __in            OEM_HAL_KEY_TYPE                f_eKeyType,
    __in            OEM_HAL_KEY_REGISTER_INDEX      f_indexKey,
    __in            OEM_HAL_SAMPLE_ENCRYPTION_MODE  f_eSampleEncryptionMode,
    __out_ecount(1) OEM_ENCRYPTION_HANDLE          *f_phEncryptionContext )
{
    DRM_RESULT                       dr                     = DRM_SUCCESS;
    OEM_HAL_KEY_TYPE                 eKeyType               = f_eKeyType;               /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX       indexKey               = f_indexKey;               /* TOC/TOU */
    OEM_HAL_SAMPLE_ENCRYPTION_MODE   eSampleEncryptionMode  = f_eSampleEncryptionMode;  /* TOC/TOU */

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkDR( Oem_HalOemImpl_CreateSampleEncryptionContext( eKeyType, indexKey, eSampleEncryptionMode, f_phEncryptionContext ) );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_EncryptSampleData
**
** Synopsis: Encrypts sample data and places the output in the supplied buffer(s). Encryption is carried out
** using the data and policy in the sample encryption context whose handle is supplied.
**
** Arguments:
**
** [f_pOEMContext]              : The OEM context
** [f_hEncryptionContext]       : The handle to an active and valid sample encryption context.
** [f_pqwInitializationVector]  : Pointer to a variable to populate with the
**                                initialization vector. If this is NULL the old
**                                IV will be used
** [f_pMetadata]                : A pointer to a caller-allocated OEM_HAL_SAMPLE_METADATA
**                                structure in which the function will place the metadata for the sample.
** [f_hInBuf]                   : An opaque handle to an OEM memory structure containing input samples.
**                                Either this param or f_pbData should be specified; not both.
**                                If this is supplied, f_hOutBuf must also be supplied.
**                                In either case, f_cbData is required.
** [f_hOutBuf]                  : An opaque handle to an OEM memory structure to receive input samples.
**                                Either this param or f_pbData should be specified; not both.
**                                If this is supplied, f_hInBuf must also be supplied.
**                                In either case, f_cbData is required.
** [f_cbData]                   : The length of the buffer pointed to by f_pbData OR by both f_hInBuf and f_hOutBuf.
** [f_pbData]                   : A pointer to a buffer to be encrypted in place.
**                                Either this param or the f_hInBuf/f_hOutBuf pair should be specified; not both.
**                                In either case, f_cbData is required.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OUTOFMEMORY
**             DRM_E_OEMHAL_SAMPLE_ENCRYPTION_CONTEXT_UNPOPULATED
**             DRM_E_OEMHAL_SAMPLE_ENCRYPTION_CONTEXT_INVALID
**             DRM_E_OEMHAL_SAMPLE_ENCRYPTION_CONTEXT_OVERUSED
**
** Notes:
**             1. The caller must know the exact length of encrypted data available and must
**                supply buffer(s) with sufficient room available to hold that data.
**                If the caller needs to probe f_hInBuf to determine the value to pass for f_cbData,
**                the caller must do so using an OEM-specific function outside the scope of this function.
**             2. After all of the data associated with a sample has been encrypted, the context reverts
**                to the unpopulated state. When the next sample is available, the context is populated
**                once more.
**             3. This function will fail if an attempt is made to encrypt more than 2**20 samples with
**                a single context.
**             4. For convenience of the programmer, the metadata can alternatively be obtained using a
**                separate call (Oem_Hal_QuerySampleMetadata).
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_EncryptSampleData(
    __in                           DRM_VOID                 *f_pOEMContext,
    __in                           OEM_ENCRYPTION_HANDLE     f_hEncryptionContext,
    __out_ecount_opt(1)            DRM_UINT64               *f_pqwInitializationVector,
    __out_ecount_opt(1)            OEM_HAL_SAMPLE_METADATA  *f_pMetadata,
    __in_opt                       OEM_OPAQUE_BUFFER_HANDLE  f_hInBuf,
    __in_opt                       OEM_OPAQUE_BUFFER_HANDLE  f_hOutBuf,
    __in                           DRM_DWORD                 f_cbData,
    __inout_bcount_opt( f_cbData ) DRM_BYTE                 *f_pbData )
{
    DRM_DWORD cbData = f_cbData;    /* TOC/TOU */

    /*
    ** Performance: Do not make a copy f_hEncryptionContext / f_hInBuf / f_hOutBuf / f_pbData.  No TOC/TOU attack exists for this data.
    ** If modified by caller after being passed in, garbage data will come out for the encrypted data.
    */

    return Oem_HalOemImpl_EncryptSampleData( f_pOEMContext, f_hEncryptionContext, f_pqwInitializationVector, f_pMetadata, f_hInBuf, f_hOutBuf, cbData, f_pbData );
}

/*********************************************************************
**
** Function: Oem_Hal_QuerySampleMetadata
**
** Synopsis: Returns metadata corresponding to the sample associated with the context. This function can
**           only be performed successfully after the context has been populated with the sample data.
**
** Arguments:
**
** [f_hEncryptionContext]   : The handle to an active and valid sample encryption context.
** [f_pMetadata]            : A pointer to a caller-allocated
**                            OEM_HAL_SAMPLE_METADATA structure in which the function will place the metadata for
**                            the sample.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_OUTOFMEMORY
**             DRM_E_BUFFERTOOSMALL
**             DRM_E_OEMHAL_SAMPLE_ENCRYPTION_CONTEXT_UNPOPULATED
**             DRM_E_OEMHAL_SAMPLE_ENCRYPTION_CONTEXT_INVALID
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_QuerySampleMetadata(
    __in            OEM_ENCRYPTION_HANDLE    f_hEncryptionContext,
    __out_ecount(1) OEM_HAL_SAMPLE_METADATA *f_pMetadata )
{
    /*
    ** Performance: Do not make a copy f_hEncryptionContext.  No TOC/TOU attack exists for this data.
    ** If modified by caller after being passed in, garbage data will come out for the metadata.
    */

    return Oem_HalOemImpl_QuerySampleMetadata( f_hEncryptionContext, f_pMetadata );
}

/*********************************************************************
**
** Function: Oem_Hal_DestroySampleEncryptionContext
**
** Synopsis: This function destroys a sample encryption context that was previously created using
**           Oem_Hal_CreateSampleEncryptionContext.
**
** Arguments:
**
** [f_hEncryptionContext] : The handle of the sample encryption context to be destroyed.
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_NOT_INITIALIZED
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_DestroySampleEncryptionContext(
    __in OEM_ENCRYPTION_HANDLE f_hEncryptionContext )
{
    /*
    ** Do not make a copy f_hEncryptionContext.  No TOC/TOU attack exists for this data.
    ** If modified by caller after being passed in, it's the equivalent of being modified before being passed in.
    */

    return Oem_HalOemImpl_DestroySampleEncryptionContext( f_hEncryptionContext );
}

static DRM_RESULT DRM_FRE_INLINE _ValidatePublicKeysAgainstCRL() DRM_FRE_INLINE_ATTRIBUTE;
static DRM_RESULT DRM_FRE_INLINE _ValidatePublicKeysAgainstCRL()
{
    DRM_RESULT                  dr      = DRM_SUCCESS;

    if( OEM_HAL_IsHalDevCertValidationSupported() )
    {
        OEM_HAL_KEY_REGISTER_INDEX  iKeys   = 0;
        DRM_DWORD                   cKeys   = 0;

        ChkDR( Oem_HalOemImpl_RegisterCount( OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC, &cKeys, NULL ) );

        /* Check every key to see if any of its digests are revoked. */
        for( iKeys = 0; iKeys < (OEM_HAL_KEY_REGISTER_INDEX)cKeys; iKeys++ )
        {
            DRM_SHA256_Digest                   digests[DRM_BCERT_MAX_CERTS_PER_CHAIN];
            DRM_DWORD                           cDigests = 0;
            OEM_HAL_PUBKEY_VERIFICATION_STATE   state    = eOEM_HAL_PUBKEY_NOT_VERIFIED;

            /* Only check revocation for valid key registers */
            if( DRM_SUCCEEDED( Oem_HalOemImpl_VerifyValidKeyRegister( OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC, iKeys ) ) )
            {
                ChkDR( Oem_HalOemImpl_GetEcc256PublicKeyCertDigests( iKeys, digests, &cDigests ) );
                if( cDigests > 0 )
                {
                    ChkDR( _CheckCertificateRevocation( digests, cDigests, &state ) );
                    ChkDR( Oem_HalOemImpl_SetEcc256PublicKeyVerificationState( iKeys, state ) );
                }
            }
        }
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_LoadPlayReadyRevocationInfo
**
** Synopsis: This function loads a signed revocation info blob. Any public keys currently in use
** are invalidated pending re-validation against a CRL submitted with Oem_Hal_LoadPlayReadyCRL
**
** Arguments:
**
** [f_pCryptoContext]   : Pointer to a crypto context
** [f_pbRevocationInfo] : Pointer to the Revocation Info BLOB
** [f_cbRevocationInfo] : Length in bytes of the Revocation Info BLOB
** [f_indexSigningKey]  : Index of the public key used to sign the revocation info
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_INVALIDARG
**             DRM_E_INVALID_REVOCATION_LIST
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_LoadPlayReadyRevocationInfo(
    __inout_ecount_opt( 1 )                 DRM_CRYPTO_CONTEXT         *f_pCryptoContext,
    __in_bcount( f_cbRevocationInfo ) const DRM_BYTE                   *f_pbRevocationInfo,
    __in                                    DRM_DWORD                   f_cbRevocationInfo,
    __in                                    OEM_HAL_KEY_REGISTER_INDEX  f_indexSigningKey )
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DWORD                    dwKeyUsageBitmap   = 0;
    DRM_BYTE                    *pbRevocationInfo   = NULL;                 /* TOC/TOU */
    DRM_DWORD                    cbRevocationInfo   = f_cbRevocationInfo;   /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX   indexSigningKey    = f_indexSigningKey;    /* TOC/TOU */

    DECLARE_LOCAL_CRYPTO_CONTEXT();

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkArg( f_pbRevocationInfo != NULL );
    ChkArg( cbRevocationInfo   >  0 );

    ALLOC_LOCAL_CRYPTO_CONTEXT();

    ChkMem( pbRevocationInfo = ( DRM_BYTE* )Oem_MemAlloc( cbRevocationInfo ) );
    OEM_SECURE_MEMCPY( pbRevocationInfo, f_pbRevocationInfo, cbRevocationInfo );

    /* Only a pubkey with usage DRM_BCERT_KEYUSAGE_SIGN_CRL can sign a RevInfo. */
    ChkDR( Oem_HalOemImpl_GetEcc256PublicKeyUsageBitmap( indexSigningKey, &dwKeyUsageBitmap, NULL ) );
    ChkBOOL( _PublicKeyUsageContains( dwKeyUsageBitmap, DRM_BCERT_KEYUSAGE_SIGN_CRL ), DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );

    ChkDR( _ProcessRevocationInfo( USE_LOCAL_CRYPTO_CONTEXT, pbRevocationInfo, cbRevocationInfo, indexSigningKey ) );

    ChkDR( _ValidatePublicKeysAgainstCRL() );

ErrorExit:
    FREE_LOCAL_CRYPTO_CONTEXT();
    SAFE_OEM_FREE( pbRevocationInfo );
    return dr;
}

/*********************************************************************
**
** Function: Oem_Hal_LoadPlayReadyCrl
**
** Synopsis: This function loads a signed CRL (cert revocation list) blob. Any public keys
**           are re-validated against the CRL
**
** Arguments:
**
** [f_pCryptoContext]   : Pointer to a crypto context
** [f_pbCRL]            : Pointer to the CRL BLOB
** [f_cbCRL]            : Length in bytes of the CRL BLOB
** [f_indexSigningKey]  : Index of the public key used to sign the CRL
**
** Returns:    DRM_SUCCESS on success.
**             DRM_E_OEMHAL_BAD_POINTER
**             DRM_E_OEMHAL_NOT_INITIALIZED
**             DRM_E_INVALIDARG
**             DRM_E_INVALID_REVOCATION_LIST
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Hal_LoadPlayReadyCrl(
    __inout_ecount_opt( 1 )      DRM_CRYPTO_CONTEXT         *f_pCryptoContext,
    __in_bcount( f_cbCRL ) const DRM_BYTE                   *f_pbCRL,
    __in                         DRM_DWORD                   f_cbCRL,
    __in                         OEM_HAL_KEY_REGISTER_INDEX  f_indexSigningKey )
{
    DRM_RESULT                   dr                 = DRM_SUCCESS;
    DRM_DWORD                    dwKeyUsageBitmap   = 0;
    DRM_BYTE                    *pbCRL              = NULL;                 /* TOC/TOU */
    DRM_DWORD                    cbCRL              = f_cbCRL;              /* TOC/TOU */
    OEM_HAL_KEY_REGISTER_INDEX   indexSigningKey    = f_indexSigningKey;    /* TOC/TOU */

    DECLARE_LOCAL_CRYPTO_CONTEXT();

    ChkDR( Oem_HalOemImpl_VerifyHalInitialized() );
    ChkArg( f_pbCRL != NULL );
    ChkArg( cbCRL   >  0 );

    ALLOC_LOCAL_CRYPTO_CONTEXT();

    ChkMem( pbCRL = ( DRM_BYTE* )Oem_MemAlloc( cbCRL ) );
    OEM_SECURE_MEMCPY( pbCRL, f_pbCRL, cbCRL );

    /* Only a pubkey with usage DRM_BCERT_KEYUSAGE_SIGN_CRL can sign a CRL. */
    ChkDR( Oem_HalOemImpl_GetEcc256PublicKeyUsageBitmap( indexSigningKey, &dwKeyUsageBitmap, NULL ) );
    ChkBOOL( _PublicKeyUsageContains( dwKeyUsageBitmap, DRM_BCERT_KEYUSAGE_SIGN_CRL ), DRM_E_OEMHAL_UNSUPPORTED_KEY_TYPE );

    ChkDR( _ProcessCRL( USE_LOCAL_CRYPTO_CONTEXT, pbCRL, cbCRL, indexSigningKey ) );

    ChkDR( _ValidatePublicKeysAgainstCRL() );

ErrorExit:
    FREE_LOCAL_CRYPTO_CONTEXT();
    SAFE_OEM_FREE( pbCRL );
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

