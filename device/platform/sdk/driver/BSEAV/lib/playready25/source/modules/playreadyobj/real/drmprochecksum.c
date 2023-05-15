/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMPROCHECKSUM_C
#include <drmplayreadyobj.h>
#include <drmxmlparser.h>
#include <drmcontract.h>
#include <drmest.h>
#include <drmutilitieslite.h>
#include <drmlicense.h>
#include <drmplayreadyobjconstants.h>
#include <drmbytemanip.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/******************************************************************************
**
** Function:    _drm_CalculateRMHeaderAesChecksum
**
** Synopsis:    A helper function to calculate a checksum for a PR Rights Management Header
**              for the AES-encrypted content.
**
**              Per "PlayReady Format" SPEC:
**              For ALGID value set to AESCTR, 16-byte KeyId is encrytped with 16-byte AES content key using ECB mode.
**              The first 8 bytes of the buffer are extracted and used as a checksum.
**
** Arguments:
**
** [f_pbContentKey]         -- Content key in raw inprotected form.
** [f_cbContentKey]         -- Length of content key. Must be DRM_AES_KEYSIZE_128(16) bytes long.
** [f_pKID]                 -- KID.
** [f_pbChecksum]           -- Checksum returned here in a binary form.
** [f_pcbChecksum]          -- Length of checksum returned here.
**
** Returns:      DRM_SUCCESS          if successful
**               DRM_E_BUFFERTOOSMALL if buffer pointed by f_pRecord is too small or f_pbChecksum is NULL.
**               DRM_E_ fail codes on other failures.
**
******************************************************************************/
static DRM_RESULT _drm_CalculateRMHeaderAesChecksum(
    __in_bcount( f_cbContentKey )       const DRM_BYTE                         *f_pbContentKey,
    __in                                const DRM_DWORD                         f_cbContentKey,
    __in_ecount( 1 )                    const DRM_KID                          *f_pKID,
    __out_bcount_opt( *f_pcbChecksum )        DRM_BYTE                         *f_pbChecksum,
    __inout                                   DRM_DWORD                        *f_pcbChecksum )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_AES_KEY aesKey = {0};
    DRM_BYTE rgbBlock[ SIZEOF( DRM_KID ) ];

    ChkArg( DRM_AES_KEYSIZE_128 == f_cbContentKey &&
                           NULL != f_pbContentKey );

    ChkArg( NULL != f_pKID );
    ChkArg( NULL != f_pcbChecksum );

    if ( NULL == f_pbChecksum )
    {
        *f_pcbChecksum = DRM_AES_CK_CHECKSUM_LENGTH;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }
    else
    {
        ChkArg( *f_pcbChecksum >= DRM_AES_CK_CHECKSUM_LENGTH );
    }

    ChkDR( Oem_Aes_SetKey( f_pbContentKey, &aesKey ) );
    OEM_SECURE_MEMCPY( rgbBlock, f_pKID->rgb, SIZEOF( DRM_KID ) );
    dr = Oem_Aes_EcbEncryptData(&aesKey, rgbBlock, SIZEOF( DRM_KID ) );
    Oem_Aes_ZeroKey( &aesKey );
    ChkDR( dr );

    OEM_SECURE_MEMCPY( f_pbChecksum, rgbBlock, DRM_AES_CK_CHECKSUM_LENGTH );
    *f_pcbChecksum = DRM_AES_CK_CHECKSUM_LENGTH;

ErrorExit:

    return dr;
}

/******************************************************************************
**
** Function:    _drm_CalculateRMHeaderCocktailChecksum
**
** Synopsis:    A helper function to calculate a checksum for a PR Rights Management Header
**              for the Cocktail-encrypted content.
**
**              Per "PlayReady Format" SPEC:
**              For ALGID value of COCKTAIL, perform the following steps:
**              1.A 21-byte buffer is created.
**              2.The content key is put in the buffer and the rest of the buffer is filled with zeros.
**              3.For five iterations:
**                  a.buffer = SHA-1 (buffer).
**              4.The first 7 bytes of the buffer are extracted and used as a checksum.
**
** Arguments:
**
** [f_pbContentKey]         -- Content key in raw inprotected form.
** [f_cbContentKey]         -- Length of content key. Must be DRMCIPHERKEYLEN_RC4(7) bytes long.
** [f_pbChecksum]           -- Checksum returned here in a binary form.
** [f_pcbChecksum]          -- Length of checksum returned here.
**
** Returns:      DRM_SUCCESS          if successful
**               DRM_E_BUFFERTOOSMALL if buffer pointed by f_pRecord is too small or f_pbChecksum is NULL.
**               DRM_E_ fail codes on other failures.
**
******************************************************************************/
static DRM_RESULT _drm_CalculateRMHeaderCocktailChecksum(
    __in_bcount( f_cbContentKey )       const DRM_BYTE                         *f_pbContentKey,
    __in                                const DRM_DWORD                         f_cbContentKey,
    __out_bcount_opt( *f_pcbChecksum )        DRM_BYTE                         *f_pbChecksum,
    __inout                                   DRM_DWORD                        *f_pcbChecksum )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SHA_CONTEXT shaVal = DRM_SHA_CONTEXT_DEFAULT;
    DRM_BYTE rgbBuffer[ __CB_DECL( DRM_SHA1_DIGEST_LEN + 1 ) ] = { 0 };
    DRM_LONG lCount = 0;

    ChkArg( DRMCIPHERKEYLEN_RC4 == f_cbContentKey &&
                           NULL != f_pbContentKey );

    ChkArg( NULL != f_pcbChecksum );

    DRMASSERT( DRMCIPHERKEYLEN_RC4 < DRM_SHA1_DIGEST_LEN + 1 ); /* Make sure we don't exceed buffer capacity in rgbBuffer. */

    if ( NULL == f_pbChecksum )
    {
        *f_pcbChecksum = CHECKSUM_LENGTH;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }
    else
    {
        ChkArg( *f_pcbChecksum >= CHECKSUM_LENGTH );
    }

    /* First copy key into rgbBuffer. */
    ZEROMEM( rgbBuffer, DRM_SHA1_DIGEST_LEN + 1 );
    OEM_SECURE_MEMCPY( rgbBuffer, f_pbContentKey, f_cbContentKey );

    /* Run SHA on key SHA_ITERATIONS times. */
    for ( lCount = 0 ; lCount < SHA_ITERATIONS ; lCount++ )
    {
        ChkDR( DRM_SHA_Init( &shaVal, eDRM_SHA_1 ) );
        ChkDR( DRM_SHA_Update( rgbBuffer, DRM_SHA1_DIGEST_LEN + 1, &shaVal ) );
        ChkDR( DRM_SHA_Finalize( &shaVal, DRM_SHA1_DIGEST_LEN, rgbBuffer ) );
    }

    /* take first CHECKSUM_LENGTH bytes of rgbBuffer and return to the caller. */
    OEM_SECURE_MEMCPY( f_pbChecksum, rgbBuffer, CHECKSUM_LENGTH );
    *f_pcbChecksum = CHECKSUM_LENGTH;

ErrorExit:

    return dr;
}

/******************************************************************************
**
** Function:    DRM_PRO_CalculateRMHeaderChecksum
**
** Synopsis:    Calculates a checksum for a PR Rights Management Header.
**
** Arguments:
**
** [f_eCipherType]          -- Cypher type (AES, COCKTAIL).
** [f_pbContentKey]         -- Content key in raw inprotected form.
** [f_cbContentKey]         -- Length of content key.
** [f_pKID]                 -- KID.
** [f_pbChecksum]           -- Checksum returned here in a binary form.
** [f_pcbChecksum]          -- Length of checksum returned here.
**
** Returns:      DRM_SUCCESS          if successful
**               DRM_E_BUFFERTOOSMALL if buffer pointed by f_pRecord is too small or f_pbChecksum is NULL.
**               DRM_E_ fail codes on other failures.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_PRO_CalculateRMHeaderChecksum(
    __in                                const DRM_SUPPORTED_CIPHERS             f_eCipherType,
    __in_bcount( f_cbContentKey )       const DRM_BYTE                         *f_pbContentKey,
    __in                                const DRM_DWORD                         f_cbContentKey,
    __in_ecount_opt( 1 )                const DRM_KID                          *f_pKID,
    __inout_bcount_opt( *f_pcbChecksum )      DRM_BYTE                         *f_pbChecksum,
    __in                                      DRM_DWORD                        *f_pcbChecksum )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMPRO, PERF_FUNC_DRM_PRO_GetRMHeaderChecksum );

    switch ( f_eCipherType )
    {
        case eDRM_AES_COUNTER_CIPHER:
            ChkArg( NULL != f_pKID );
            ChkDR( _drm_CalculateRMHeaderAesChecksum( f_pbContentKey, f_cbContentKey, f_pKID, f_pbChecksum, f_pcbChecksum ) );
            break;

        case eDRM_RC4_CIPHER:
            ChkDR( _drm_CalculateRMHeaderCocktailChecksum( f_pbContentKey, f_cbContentKey, f_pbChecksum, f_pcbChecksum ) );
            break;

        default:
            ChkArg( FALSE );
    };

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}
EXIT_PK_NAMESPACE_CODE;
