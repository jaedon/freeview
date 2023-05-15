/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/******************************************************************************
**
** This file defines a minimal set of key derivation functions for inside the secure crypto core.
** OEMs should not need to change this file in any way (except to get it to compile properly).
** If the device supports a secure crypto core, this file should compile ONLY for that core.
**
*******************************************************************************
*/

#define DRM_BUILDING_OEMDERIVEDKEYS_C

#include <oemaes.h>
#include <oemderivedkeys.h>

#include <drmerr.h>
#include <drmlicense.h>
#include <drmderivedkey.h>

#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

static DRM_BYTE g_rgbMagicConstantLeft[ DRM_AES_KEYSIZE_128 ] = {
    0xf0, 0x61, 0x4d, 0xb6, 0x0f, 0xca, 0x3f, 0xbb,
    0x0f, 0x0d, 0x20, 0xf5, 0x61, 0xad, 0xec, 0x81 };

static DRM_BYTE g_rgbMagicConstantRight[ DRM_AES_KEYSIZE_128 ] = {
    0x3e, 0xf4, 0x88, 0xf3, 0x37, 0xe2, 0xda, 0xc6,
    0x62, 0x14, 0x9a, 0xdb, 0x82, 0x7e, 0x97, 0x6d };

static DRM_BYTE g_rgbMagicConstantZero[ DRM_AES_KEYSIZE_128 ] = {
    0x7e, 0xe9, 0xed, 0x4a, 0xf7, 0x73, 0x22, 0x4f,
    0x00, 0xb8, 0xea, 0x7e, 0xfb, 0x02, 0x7c, 0xbb };


/*********************************************************************
**
** Function: _VerifyChecksum2
**
** Synopsis: Function that verifies the checksum of the key that is used
**           to decrypt the leaf content key.
**
** Arguments:
**
** [f_pbKey]                -- Pointer to the AES 128 key to be verified.
** [f_pKID]                 -- Checksum KID.
** [f_pbChecksumData]       -- Checksum data.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_CH_BAD_KEY if checksum verification fails.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _VerifyChecksum2(
    __in_bcount( DRM_AES_KEYSIZE_128 )          const DRM_BYTE                      *f_pbKey,
    __in                                        const DRM_KID                       *f_pKID,
    __in_ecount( DRM_AES_CK_CHECKSUM_LENGTH )   const DRM_BYTE                      *f_pbChecksumData )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_AES_KEY oAesKey;                        /* Don't initialize key material */
    DRM_BYTE    rgbBlock[ SIZEOF( DRM_KID ) ];  /* Don't initialize key material */

    ChkDR( Oem_Aes_SetKey( f_pbKey, &oAesKey ) );

    OEM_SECURE_MEMCPY( rgbBlock, f_pKID->rgb, SIZEOF( DRM_KID ) );

    ChkDR( Oem_Aes_EcbEncryptData( &oAesKey, rgbBlock, SIZEOF( DRM_KID ) ) );

    ChkBOOL( OEM_SECURE_ARE_EQUAL( f_pbChecksumData, rgbBlock, DRM_AES_CK_CHECKSUM_LENGTH ), DRM_E_CH_BAD_KEY );

ErrorExit:
    Oem_Aes_ZeroKey( &oAesKey );
    return dr;
}


/*********************************************************************
**
** Function: _DecryptContentKey_Derived
**
** Synopsis: Function that decryptes a content key using all derived keys
**           of locations in the UplinkX plus the seconary key in the Aux Key.
**
** Arguments:
**
** [f_pbSecondaryKey]       -- Pointer to an encrypted secondary key.
** [f_poUplinkX]            -- Pointer the UplinkX entry array in the leaf license.
** [f_pKID]                 -- Pointer to KID used for checksum validation.
**                             If this pointer is NULL, checksum will not be verified.
**                             If this pointer is NULL, f_pbChecksumData must be NULL.
**                             If this pointer is not NULL, f_pbChecksumData must be not NULL.
** [f_poChecksum]           -- Pointer to a checksum data used to make sure the
**                             key used to decrypt the content key is valid.
**                             If this pointer is NULL, checksum will not be verified.
** [f_pbKey]                -- Pointer to a buffer that contains the encrypted
**                             content key during input, and the decrypted content
**                             key during output. The size of the key is
**                             DRM_AES_KEYSIZE_128_X2 since it includes both CI and CK.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT _DecryptContentKey_Derived(
    __in_bcount( DRM_AES_KEYSIZE_128 )              const DRM_BYTE                      *f_pbSecondaryKey,
    __in                                            const DRM_XMR_UPLINKX               *f_poUplinkX,
    __in_opt                                        const DRM_KID                       *f_pKID,
    __in_ecount_opt( DRM_AES_CK_CHECKSUM_LENGTH )   const DRM_BYTE                      *f_pbChecksumData,
    __inout_bcount( DRM_AES_KEYSIZE_128_X2 )              DRM_BYTE                      *f_pbKey )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_AES_KEY aesKey; /* Don't initialize key material */

    DRMCASSERT( DRM_AES_KEYSIZE_128_X2 == ( DRM_AES_KEYSIZE_128 * 2 ) );

    ChkPtr( f_pbSecondaryKey );
    ChkPtr( f_poUplinkX );
    ChkPtr( f_poUplinkX->pbKey );
    ChkPtr( f_pbKey );
    ChkArg( ( f_pKID == NULL ) == ( f_pbChecksumData == NULL ) );

    /* Semi-decrypt the leaf content key using the combined middle (key tree) key. */
    ChkDR( Oem_Aes_SetKey( f_poUplinkX->pbKey,
                           &aesKey ) );
    ChkDR( Oem_Aes_EcbEncryptData( &aesKey,
                                   f_pbKey,
                                   DRM_AES_KEYSIZE_128_X2 ) );

    /* Decrypt the semi decrypted leaf content key using the clear secondary key. */
    ChkDR( Oem_Aes_SetKey( f_pbSecondaryKey, &aesKey ) );
    ChkDR( Oem_Aes_EcbEncryptData( &aesKey,
                                   f_pbKey,
                                   DRM_AES_KEYSIZE_128_X2 ) );

    if( f_pbChecksumData != NULL )
    {
        ChkDR( _VerifyChecksum2( f_pbKey + DRM_AES_KEYSIZE_128, f_pKID, f_pbChecksumData ) );
    }

ErrorExit:
    Oem_Aes_ZeroKey( &aesKey );
    return dr;
}





/*********************************************************************
**
** Function: _CalcContent_Key_Prime
**
** Synopsis: Function that calculates the AES key that is used to encrypt
**           the derived tree keys. The AES key is derived from the root
**           content key using the following formula:
**           K' = AES128_ENCRYPT_BLOCK(K, K^MCZero)
**
** Arguments:
**
** [f_pbContentKey]         -- Pointer to the root content key.
**                             child of the current key.
** [f_pbContentKeyPrime]    -- Pointer to a buffer to receive the calculated
**                             AES key.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _CalcContent_Key_Prime(
    __in_bcount( DRM_AES_KEYSIZE_128 )  const DRM_BYTE *f_pbContentKey,
    __out_bcount( DRM_AES_KEYSIZE_128 )       DRM_BYTE *f_pbContentKeyPrime )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_DWORD   i  = 0;
    DRM_AES_KEY oAesKey;    /* Don't initialize key material */

    OEM_SECURE_MEMCPY( f_pbContentKeyPrime, f_pbContentKey, DRM_AES_KEYSIZE_128 );

    for( i = 0; i < DRM_AES_KEYSIZE_128; i++ )
    {
        f_pbContentKeyPrime[ i ] ^= g_rgbMagicConstantZero[ i ];
    }

    ChkDR( Oem_Aes_SetKey( f_pbContentKey, &oAesKey ) );

    ChkDR( Oem_Aes_EcbEncryptData( &oAesKey, f_pbContentKeyPrime, DRM_AES_KEYSIZE_128 ) );

ErrorExit:
    Oem_Aes_ZeroKey( &oAesKey );
    return dr;
}

/*********************************************************************
**
** Function: _CalcNextKey
**
** Synopsis: Function that calculates the next key value based on the
**           current key value and whether the new key is the left or the
**           right child of the current key.
**
** Arguments:
**
** [f_fDeriveRight]         -- Flag indicating whether the new key is the right
**                             child of the current key.
** [f_pbKey]                -- Pointer to a buffer that contains the current key
**                             value during input and the new key value during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _CalcNextKey(
    __in                                    DRM_BOOL  f_fDeriveRight,
    __inout_bcount( DRM_AES_KEYSIZE_128 )   DRM_BYTE *f_pbKey )
{
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_AES_KEY oAesKey;                            /* Don't initialize key material */
    DRM_BYTE    rgbResult[ DRM_AES_KEYSIZE_128 ];   /* Don't initialize key material */

    ChkDR( Oem_Aes_SetKey( f_pbKey, &oAesKey ) );

    if( f_fDeriveRight )
    {
        OEM_SECURE_MEMCPY( rgbResult, g_rgbMagicConstantRight, DRM_AES_KEYSIZE_128 );
    }
    else
    {
        OEM_SECURE_MEMCPY( rgbResult, g_rgbMagicConstantLeft, DRM_AES_KEYSIZE_128 );
    }

    ChkDR( Oem_Aes_EcbEncryptData( &oAesKey, rgbResult, DRM_AES_KEYSIZE_128 ) );

    OEM_SECURE_MEMCPY( f_pbKey, rgbResult, DRM_AES_KEYSIZE_128 );

ErrorExit:
    Oem_Aes_ZeroKey( &oAesKey );
    OEM_SECURE_ZERO_MEMORY( rgbResult, SIZEOF( rgbResult ) );
    return dr;
}


/*********************************************************************
**
** Function: _TryDeriveKey
**
** Synopsis: Function that tries to derive the key of a given location
**           using Aux Key entries from the stack that is used to store
**           the intermediate keys from pervious derivations. This
**           function may generate new stack entries during its derivation
**           process.
**
** Arguments:
**
** [f_dwLocation]           -- The location whose key is to be derived.
** [f_poKeyStack]           -- A stack of Aux Key entries that is used to stored the
**                             intermediate keys during key derivation.
** [f_pdwStackTop]          -- Pointer to a value that indicates the current top of
**                             the stack mentioned above.
** [f_pbKey]                -- Pointer to a buffer to store the derived key of the location.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_S_FALSE if the location cannot be derived from stack entries.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _TryDeriveKey(
    __in                                DRM_DWORD              f_dwLocation,
    __inout                             DRM_XMR_AUX_KEY_ENTRY *f_poKeyStack,
    __inout                             DRM_DWORD             *f_pdwStackTop,
    __out_bcount( DRM_AES_KEYSIZE_128 ) DRM_BYTE              *f_pbKey )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  dwSBMParent  = 0;
    DRM_BOOL   fDeriveRight = FALSE;

    if( !IsAncestor( f_poKeyStack[ *f_pdwStackTop - 1 ].dwLocation,
                     f_dwLocation ) )
    {
        dr = DRM_S_FALSE;
    }
    else
    {
        while( f_poKeyStack[ *f_pdwStackTop - 1 ].dwLocation != f_dwLocation )
        {
            dwSBMParent = SBM( f_poKeyStack[ *f_pdwStackTop - 1 ].dwLocation );

            fDeriveRight = ( f_dwLocation & dwSBMParent ) != 0;
            OEM_SECURE_MEMCPY( f_poKeyStack[ *f_pdwStackTop ].rgbKey,
                               f_poKeyStack[ *f_pdwStackTop - 1 ].rgbKey,
                               DRM_AES_KEYSIZE_128 );

            ChkDR( _CalcNextKey( fDeriveRight,  f_poKeyStack[ *f_pdwStackTop ].rgbKey ) );

            f_poKeyStack[ *f_pdwStackTop ].dwLocation =
                        ( ( f_poKeyStack[ *f_pdwStackTop - 1 ].dwLocation & ( ~dwSBMParent ) |
                          ( f_dwLocation & dwSBMParent ) ) |
                        ( dwSBMParent >> 1 ) );

            ( *f_pdwStackTop )++;

            ChkBOOL( *f_pdwStackTop < MAX_KEY_STACK, DRM_E_UNABLE_TO_RESOLVE_LOCATION_TREE );
        }

        OEM_SECURE_MEMCPY( f_pbKey,
                           f_poKeyStack[ *f_pdwStackTop - 1 ].rgbKey,
                           DRM_AES_KEYSIZE_128 );
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _FindNextAuxEntry
**
** Synopsis: Function that bisectionally searches the Aux Key entry
**           array to find an Aux Key entry that can be used to derive
**           a given location.
**
** Arguments:
**
** [f_dwLocation]           -- The location that is used to search for an
**                             Aux Key entry to derive from.
** [f_poAuxKey]             -- Pointer to the Aux Key entry array in the root license.
** [f_pdwAuxKeyIndex]       -- Pointer to the beginning index of Aux Key entry to
**                          -- use in the Aux Key entry array. It may be updated
**                             when the function is returned.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_S_FALSE if no matching is found.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _FindNextAuxEntry(
    __in          DRM_DWORD        f_dwLocation,
    __in    const DRM_XMR_AUX_KEY *f_poAuxKey,
    __inout       DRM_DWORD       *f_pdwAuxKeyIndex )
{
    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_BOOL   fDone = FALSE;
    DRM_LONG   nLeft, nRight, nMiddle, nStart;  /* Initialized before usage below */
    DRM_DWORD  dwLocationToTry;                 /* Initialized before usage below */

    AssertChkArg( f_poAuxKey       != NULL );
    AssertChkArg( f_pdwAuxKeyIndex != NULL );

    if( f_poAuxKey->cEntries == 0
     || *f_pdwAuxKeyIndex    >= f_poAuxKey->cEntries )
    {
        dr = DRM_S_FALSE;
    }
    else
    {
        nStart = nLeft = ( DRM_LONG )( *f_pdwAuxKeyIndex );
        nRight = f_poAuxKey->cEntries - 1;
      
        while( !fDone )
        {
            nMiddle = ( nLeft + nRight ) / 2;

            NETWORKBYTES_TO_DWORD( dwLocationToTry, &f_poAuxKey->pEntries[ nMiddle ].dwLocation, 0 );
            if( IsAncestor( dwLocationToTry, f_dwLocation ) )
            {
                *f_pdwAuxKeyIndex = (DRM_DWORD)nMiddle;
                fDone = TRUE;   /* Found the index */
            }
            else
            {
                if( dwLocationToTry > f_dwLocation )
                {
                    nRight = nMiddle - 1;
                }
                else
                {
                    nLeft = nMiddle + 1;
                }
                
                if( nLeft  >= f_poAuxKey->cEntries
                 || nRight <  nStart
                 || nLeft  >  nRight )
                {
                    dr    = DRM_S_FALSE;
                    fDone = TRUE;   /* Index not be found */
                }
            }
        }
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _CalcOneDerivedKey
**
** Synopsis: Function that calculates the key of an UplinkX entry from
**           the Aux Key entries in the root license.
**
** Arguments:
**
** [f_pbContentKeyPrime]    -- Pointer to the clear AES 128 content key prime
**                             used to decrypt the key in the Aux Key entry.
** [f_poAuxKey]             -- Pointer to the Aux Key entry array in the root license.
** [f_poUplinkX]            -- Pointer the UplinkX entry array in the leaf license.
** [f_dwUplinkXIdx]         -- The index of the UplinkX entry whose key is to
**                             be calculated.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_UNABLE_TO_RESOLVE_LOCATION_TREE if a key cannot be derived.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note:                    The Aux entries in the root license are sorted in their
**                          location values (ascending).
**
**********************************************************************/
static DRM_RESULT _CalcOneDerivedKey(
    __in_bcount( DRM_AES_KEYSIZE_128 )  const DRM_BYTE              *f_pbContentKeyPrime,
    __in                                const DRM_XMR_AUX_KEY       *f_poAuxKey,
    __inout                                   DRM_XMR_UPLINKX       *f_poUplinkX,
    __in                                      DRM_DWORD              f_dwUplinkXIdx )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_XMR_AUX_KEY_ENTRY rgoKeyStack[ MAX_KEY_STACK ]; /* Don't initialize key material */
    DRM_DWORD dwStackTop    = 0;
    DRM_DWORD dwAuxKeyIndex = 0;
    DRM_BOOL  fFound        = FALSE;
    DRM_DWORD dwLocation    = 0;
    DRM_BYTE  rgbNextKey[DRM_AES_KEYSIZE_128];  /* Don't initialize key material */

    ChkArg( f_poUplinkX->pbKey != NULL );

    /* Need to covert the location to machine byte order before being used. */
    NETWORKBYTES_TO_DWORD( dwLocation, &f_poUplinkX->pdwLocation[ f_dwUplinkXIdx ], 0 );

    while( !fFound )
    {
        while( dwStackTop > 0 && !fFound )
        {
            ChkDR( _TryDeriveKey( dwLocation,
                                  rgoKeyStack,
                                  &dwStackTop,
                                  rgbNextKey ) );

            if( dr == DRM_SUCCESS )
            {
                fFound = TRUE;
            }
            else
            {
                dwStackTop--;
            }
        }

        if( fFound )
        {
            /*
            ** The key is derived. XOR it to the accumulated key buffer
            */
            DRM_DWORD i = 0;

            for ( i = 0; i < DRM_AES_KEYSIZE_128; i++ )
            {
                f_poUplinkX->pbKey[ i ] ^= rgbNextKey[i];
            }
        }
        else
        {
            ChkBOOL( dwStackTop == 0, DRM_E_UNABLE_TO_RESOLVE_LOCATION_TREE );

            ChkDR( _FindNextAuxEntry( dwLocation, f_poAuxKey, &dwAuxKeyIndex ) );

            if( dr == DRM_S_FALSE )
            {
                /* No match. */
                TRACE( ( "Fail to derive key for location: %d", dwLocation ) );
                ChkDR( DRM_E_UNABLE_TO_RESOLVE_LOCATION_TREE );
            }
            else
            {
                DRM_AES_KEY oAesKey;  /* Don't initialize key material */

                DRM_DWORD dwLocationInternal = 0;

                OEM_SECURE_MEMCPY( &rgoKeyStack[ dwStackTop ],
                                   &f_poAuxKey->pEntries[ dwAuxKeyIndex ],
                                   SIZEOF( DRM_XMR_AUX_KEY_ENTRY ) );

                NETWORKBYTES_TO_DWORD( dwLocationInternal, &rgoKeyStack[ dwStackTop ].dwLocation, 0 );
                rgoKeyStack[ dwStackTop ].dwLocation = dwLocationInternal;

                ChkDR( Oem_Aes_SetKey( f_pbContentKeyPrime, &oAesKey ) );

                ChkDR( Oem_Aes_EcbEncryptData( &oAesKey,
                                               rgoKeyStack[ dwStackTop ].rgbKey,
                                               DRM_AES_KEYSIZE_128 ) );

                dwAuxKeyIndex++;
                dwStackTop++;
            }
        }
    }

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( rgoKeyStack, SIZEOF( rgoKeyStack ) );
    OEM_SECURE_ZERO_MEMORY( rgbNextKey,  SIZEOF( rgbNextKey ) );
    return dr;
}


/*********************************************************************
**
** Function: _CalculateKeys_Derived
**
** Synopsis: Function that calculates all UplinkX keys from the Aux Key
**           entries in the root license.
**
** Arguments:
**
** [f_pbRootContentKey]     -- Pointer to the clear AES 128 root content key
**                             used to decrypt the key in the Aux Key entry.
** [f_poAuxKey]             -- Pointer to the Aux Key entry array in the root license.
** [f_poUplinkX]            -- Pointer the UplinkX entry array in the leaf license.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note:                    The Aux entries in the root license are sorted in their
**                          location values (ascending).
**
**********************************************************************/
DRM_RESULT _CalculateKeys_Derived(
    __in_bcount( DRM_AES_KEYSIZE_128 )  const DRM_BYTE              *f_pbRootContentKey,
    __in                                const DRM_XMR_AUX_KEY       *f_poAuxKey,
    __inout                                   DRM_XMR_UPLINKX       *f_poUplinkX )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_DWORD  iEntry   = 0;
    DRM_BYTE   rgbContentKeyPrime[ DRM_AES_KEYSIZE_128 ];   /* Don't initialize key material */

    ChkArg( f_pbRootContentKey != NULL );
    ChkArg( f_poAuxKey != NULL && f_poUplinkX != NULL );

    ChkDR( _CalcContent_Key_Prime( f_pbRootContentKey, rgbContentKeyPrime ) );

    for( iEntry = 0; iEntry < f_poUplinkX->cEntries; iEntry++ )
    {
        ChkDR( _CalcOneDerivedKey( rgbContentKeyPrime, f_poAuxKey, f_poUplinkX, iEntry ) );
    }

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( rgbContentKeyPrime, SIZEOF( rgbContentKeyPrime ) );
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

