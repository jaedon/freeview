/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>

#include <PKTestNameSpace.h>

USING_PK_NAMESPACE
USING_PKTEST_NAMESPACE

#include <drmcommon.h>
#include <oemaescommon.h>
#include <drmderivedkey.h>

#define ROOT_LOCATION   0x8000

static DRM_BYTE g_rgbMagicConstantLeft[ DRM_AES_KEYSIZE_128 ] = {
    0xf0, 0x61, 0x4d, 0xb6, 0x0f, 0xca, 0x3f, 0xbb,
    0x0f, 0x0d, 0x20, 0xf5, 0x61, 0xad, 0xec, 0x81 };

static DRM_BYTE g_rgbMagicConstantRight[ DRM_AES_KEYSIZE_128 ] = {
    0x3e, 0xf4, 0x88, 0xf3, 0x37, 0xe2, 0xda, 0xc6,
    0x62, 0x14, 0x9a, 0xdb, 0x82, 0x7e, 0x97, 0x6d };

static DRM_BYTE g_rgbMagicConstantZero[ DRM_AES_KEYSIZE_128 ] = {
    0x7e, 0xe9, 0xed, 0x4a, 0xf7, 0x73, 0x22, 0x4f,
    0x00, 0xb8, 0xea, 0x7e, 0xfb, 0x02, 0x7c, 0xbb };

static DRM_RESULT _CalcNextKey(
    __in                                  DRM_BOOL  f_fDeriveRight,
    __inout_bcount( DRM_AES_KEYSIZE_128 ) DRM_BYTE *f_pbKey )
{
    DRM_RESULT  dr                               = DRM_SUCCESS;
    DRM_AES_KEY oAesKey                          = { 0 };
    DRM_BYTE    rgbResult[ DRM_AES_KEYSIZE_128 ] = { 0 };

    ChkDR( Oem_Aes_SetKey( f_pbKey, &oAesKey ) );

    if ( f_fDeriveRight )
    {
        MEMCPY( rgbResult, g_rgbMagicConstantRight, DRM_AES_KEYSIZE_128 );
    }
    else
    {
        MEMCPY( rgbResult, g_rgbMagicConstantLeft, DRM_AES_KEYSIZE_128 );
    }

    ChkDR( Oem_Aes_EcbEncryptData( &oAesKey, rgbResult, DRM_AES_KEYSIZE_128 ) );

    MEMCPY( f_pbKey, rgbResult, DRM_AES_KEYSIZE_128 );

ErrorExit:
    return dr;
}

DRM_RESULT TSTUTIL_DeriveAuxKey(
    __in                                      DRM_DWORD f_dwLocation,
    __in_bcount( DRM_AES_KEYSIZE_128 )  const DRM_BYTE *f_pbBaseKey,
    __out_bcount( DRM_AES_KEYSIZE_128 )       DRM_BYTE *f_pbKey )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  dwParent     = ROOT_LOCATION;
    DRM_DWORD  dwLPParent   = 0;
    DRM_DWORD  dwSBMParent  = 0;
    DRM_BOOL   fDeriveRight = FALSE;

    ChkArg( f_pbBaseKey != NULL );
    ChkArg( f_pbKey != NULL );
    MEMCPY( f_pbKey, f_pbBaseKey, DRM_AES_KEYSIZE_128 );

    while ( dwParent != f_dwLocation )
    {
        dwLPParent = LP( dwParent );
        dwSBMParent = SBM( dwParent );

        fDeriveRight = ( f_dwLocation & dwSBMParent ) != 0;
        ChkDR( _CalcNextKey( fDeriveRight, f_pbKey ) );

        dwParent = ( dwParent & ( ~dwSBMParent ) |
                   ( f_dwLocation & dwSBMParent ) |
                   ( dwSBMParent >> 1 ) );
    }

ErrorExit:
    return dr;
}

DRM_RESULT TSTUTIL_CalcContentKeyPrime(
    __in_bcount( DRM_AES_KEYSIZE_128 )  const DRM_BYTE *f_pbContentKey,
    __out_bcount( DRM_AES_KEYSIZE_128 )       DRM_BYTE *f_pbContentKeyPrime )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i = 0;
    DRM_AES_KEY oAesKey = { 0 };

    ChkArg( f_pbContentKey != NULL );
    ChkArg( f_pbContentKeyPrime != NULL );

    MEMCPY( f_pbContentKeyPrime, f_pbContentKey, DRM_AES_KEYSIZE_128 );

    for ( i = 0; i < DRM_AES_KEYSIZE_128; i++ )
    {
        f_pbContentKeyPrime[ i ] ^= g_rgbMagicConstantZero[ i ];
    }

    ChkDR( Oem_Aes_SetKey( f_pbContentKey, &oAesKey ) );

    ChkDR( Oem_Aes_EcbEncryptData( &oAesKey, f_pbContentKeyPrime, DRM_AES_KEYSIZE_128 ) );

ErrorExit:
    return dr;
}

DRM_RESULT TSTUTIL_UnshuffleContentKey(
    __inout_bcount( DRM_AES_KEYSIZE_128 * 2 ) DRM_BYTE *f_pbContentKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE rgbTmp[ DRM_AES_KEYSIZE_128 * 2 ];
    DRM_DWORD i, j = 0;

    ChkArg( f_pbContentKey != NULL );

    for ( i = 0; i < DRM_AES_KEYSIZE_128; i++ )
    {
        rgbTmp[ i ] = f_pbContentKey[ j++ ];
        rgbTmp[ i + DRM_AES_KEYSIZE_128 ] = f_pbContentKey[ j++ ];
    }

    MEMCPY( f_pbContentKey, rgbTmp, DRM_AES_KEYSIZE_128 * 2 );

ErrorExit:
    return dr;
}

