/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/*
** drmaes.c
**
** Summary:
**  This file implements AES signing and encryption of variable length data.
**  It assumes the existence of routines to encrypt a block sized buffer
*/

#define DRM_BUILDING_OEMAESMULTI_C
#include <oemaes.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmtypes.h>
#include <byteorder.h>
#include <oem.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*********************************************************************************************
** Function:  _XOR
**
** Synopsis:  Byte-by-byte XOR of two buffers using two indices.
**
** Arguments: [f_pbLHS]  : Pointer to the left-hand side
**            [f_ibLHS]  : Index into the LHS to start XOR at
**            [f_pbRHS]  : Pointer to the right-hand side
**            [f_ibRHS]  : Index into the RHS to start XOR at
**            [f_cb]     : Byte count to XOR
**
** Returns:   DRM_VOID
**********************************************************************************************/
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_COUNT_REQUIRED_FOR_WRITABLE_BUFFER_25057, "f_pbLHS length defined by f_ibLHS and f_cb" )
static DRMFORCEINLINE DRM_VOID _XOR(
    __inout_bcount( f_ibLHS + f_cb )    DRM_BYTE   *f_pbLHS,
    __in                                DRM_DWORD   f_ibLHS,
    __in_bcount( f_ibRHS + f_cb ) const DRM_BYTE   *f_pbRHS,
    __in                                DRM_DWORD   f_ibRHS,
    __in                                DRM_DWORD   f_cb )
PREFAST_POP
{
    DRM_DWORD i = 0;

    for(  ; i < f_cb; i++ )
    {
        PUT_BYTE( f_pbLHS,
                  i + f_ibLHS,
                  (DRM_BYTE)( GET_BYTE( f_pbLHS, i + f_ibLHS ) ^ GET_BYTE( f_pbRHS, i + f_ibRHS ) ) );
    }
}


/*********************************************************************************************
** Function:  Oem_Aes_CtrProcessData
**
** Synopsis:  Does AES Counter-Mode encryption or decryption on a buffer of data
**
** Arguments: [f_pKey]      : The AES secret key used to encrypt or decrypt buffer
**            [f_pbData]    : The buffer to encrypt or decrypt ( in place )
**            [f_cbData]    : The number of bytes to encrypt or decrypt
**            [f_pCtrContext] : Contains the initialization vector and offset data. Will be updated
**
** Returns:   DRM_SUCCESS
**              Success
**            DRM_E_INVALIDARG
**              One of the pointers was NULL, or the byte count is 0, or neither the
**              block ID or offset are 0 ( one must == 0 ).
**            DRM_E_CRYPTO_FAILED
**              The encrypt/decrypt operation failed
**********************************************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Aes_CtrProcessData(
    __in_ecount( 1 )            const DRM_AES_KEY                  *f_pKey,
    __inout_bcount( f_cbData )        DRM_BYTE                     *f_pbData,
    __in                              DRM_DWORD                     f_cbData,
    __inout_ecount( 1 )               DRM_AES_COUNTER_MODE_CONTEXT *f_pCtrContext )
{
    /* TODO: TFS: see bug 213254. For now, we protect this function*/
#if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_WP8
    CLAW_AUTO_RANDOM_CIPHER
#endif
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_UINT64 qwIVCorrectEndianness;
    DRM_UINT64 rllDataOut[__CB_DECL( DRM_AES_BLOCKLEN )/SIZEOF(DRM_UINT64)];
    DRM_DWORD  cbDataLeft   = f_cbData;
    DRM_DWORD  ibDataOutCur = 0;
    DRM_DWORD  cbDataToUse  = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMAES, PERF_FUNC_Oem_Aes_CtrProcessData );

    ChkArg( f_pbData      != NULL );
    ChkArg( f_cbData      >  0 );
    ChkArg( f_pCtrContext != NULL );
    ChkArg( f_pKey        != NULL );
    ChkArg( f_pCtrContext->bByteOffset <= DRM_AES_BLOCKLEN );

    QWORD_TO_NETWORKBYTES_SECURE( (DRM_BYTE*)&qwIVCorrectEndianness, 0, f_pCtrContext->qwInitializationVector );

    if( f_pCtrContext->bByteOffset > 0 )
    {
        /*
        ** The data is in the middle of a block.  Handle the special case first
        */
        cbDataToUse = (DRM_DWORD)( DRM_AES_BLOCKLEN - f_pCtrContext->bByteOffset );
        cbDataToUse = min( cbDataLeft, cbDataToUse );

        QWORD_TO_NETWORKBYTES_SECURE( (DRM_BYTE*)&( rllDataOut[1] ), 0, f_pCtrContext->qwBlockOffset );
        rllDataOut[0] = qwIVCorrectEndianness;

        dr = Oem_Aes_EncryptOne( f_pKey, (DRM_BYTE*) rllDataOut );
        ChkBOOL( DRM_SUCCEEDED( dr ), DRM_E_CRYPTO_FAILED );

        _XOR( f_pbData, ibDataOutCur, (DRM_BYTE*)rllDataOut, f_pCtrContext->bByteOffset, cbDataToUse );

        ibDataOutCur += cbDataToUse;

        ChkOverflow( cbDataLeft, cbDataLeft - cbDataToUse );
        cbDataLeft -= cbDataToUse;

        /*
        ** If we used all of the bytes in the current block, then the block offset needs to be increased by one.
        */
        if (f_pCtrContext->bByteOffset + cbDataToUse == DRM_AES_BLOCKLEN )
        {
            f_pCtrContext->qwBlockOffset = DRM_UI64Add( f_pCtrContext->qwBlockOffset, DRM_UI64( 1 ) );
        }
    }

    while( cbDataLeft >= DRM_AES_BLOCKLEN )
    {
        QWORD_TO_NETWORKBYTES_SECURE( (DRM_BYTE*)&( rllDataOut[1] ), 0, f_pCtrContext->qwBlockOffset );
        rllDataOut[0] = qwIVCorrectEndianness;

        dr = Oem_Aes_EncryptOne( f_pKey, (DRM_BYTE*)rllDataOut );
        ChkBOOL( DRM_SUCCEEDED( dr ), DRM_E_CRYPTO_FAILED );

        _XOR( f_pbData, ibDataOutCur, (DRM_BYTE*)rllDataOut, 0, DRM_AES_BLOCKLEN );

        ibDataOutCur += DRM_AES_BLOCKLEN;
        cbDataLeft   -= DRM_AES_BLOCKLEN;

        f_pCtrContext->qwBlockOffset = DRM_UI64Add( f_pCtrContext->qwBlockOffset, DRM_UI64( 1 ) );
    }

    if( cbDataLeft > 0 ) /* at this point it is strictly less than DRM_AES_BLOCKLEN */
    {
        cbDataToUse = cbDataLeft; 

        QWORD_TO_NETWORKBYTES_SECURE( (DRM_BYTE*)&( rllDataOut[1] ), 0, f_pCtrContext->qwBlockOffset );
        rllDataOut[0] = qwIVCorrectEndianness;

        dr = Oem_Aes_EncryptOne( f_pKey, (DRM_BYTE*)rllDataOut );
        ChkBOOL( DRM_SUCCEEDED( dr ), DRM_E_CRYPTO_FAILED );

        _XOR( f_pbData, ibDataOutCur, (DRM_BYTE*)rllDataOut, 0, cbDataToUse );
    }


    f_pCtrContext->bByteOffset = ( f_pCtrContext->bByteOffset + f_cbData ) % DRM_AES_BLOCKLEN;
ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*********************************************************************************************
** Function:  Oem_Aes_CbcEncryptData
**
** Synopsis:  Does AES CBC-Mode encryption on a buffer of data
**
** Arguments: [f_pKey]      : The AES secret key used to encrypt the buffer
**            [f_pbData]    : The buffer to encrypt ( in place )
**            [f_cbData]    : The number of bytes to encrypt
**            [f_rgbIV]     : The initialization vector to use for encryption
**
** Returns:   DRM_SUCCESS
**              Success
**            DRM_E_INVALIDARG
**               One of the pointers was NULL, or the byte count is 0
**               or not a multiple of DRM_AES_BLOCKLEN
**            DRM_E_CRYPTO_FAILED
**              The encrypt operation failed
**********************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Oem_Aes_CbcEncryptData(
    __in_ecount( 1 )                const DRM_AES_KEY *f_pKey,
    __inout_bcount( f_cbData )            DRM_BYTE    *f_pbData,
    __in                                  DRM_DWORD    f_cbData,
    __in_bcount( DRM_AES_BLOCKLEN ) const DRM_BYTE     f_rgbIV[__CB_DECL( DRM_AES_BLOCKLEN )] ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Oem_Aes_CbcEncryptData(
    __in_ecount( 1 )                const DRM_AES_KEY *f_pKey,
    __inout_bcount( f_cbData )            DRM_BYTE    *f_pbData,
    __in                                  DRM_DWORD    f_cbData,
    __in_bcount( DRM_AES_BLOCKLEN ) const DRM_BYTE     f_rgbIV[__CB_DECL( DRM_AES_BLOCKLEN )] )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr           =  DRM_SUCCESS;
    DRM_DWORD  cbDataLeft   = f_cbData;
    DRM_DWORD  ibDataOutCur = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMAES, PERF_FUNC_Oem_Aes_CbcEncryptData );

    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData >= DRM_AES_BLOCKLEN );
    ChkArg( f_cbData % DRM_AES_BLOCKLEN ==  0 );

    /*
    ** The first block is a special case: Use the IV to XOR
    */
    _XOR( f_pbData, 0, f_rgbIV, 0, DRM_AES_BLOCKLEN );
    dr = Oem_Aes_EncryptOne( f_pKey, f_pbData );
    ChkBOOL( DRM_SUCCEEDED( dr ), DRM_E_CRYPTO_FAILED );
    cbDataLeft -= DRM_AES_BLOCKLEN;

    while( cbDataLeft > 0 )
    {
        _XOR( f_pbData, ibDataOutCur + DRM_AES_BLOCKLEN, f_pbData, ibDataOutCur, DRM_AES_BLOCKLEN );
        ibDataOutCur += DRM_AES_BLOCKLEN;
        dr = Oem_Aes_EncryptOne( f_pKey, &f_pbData[ibDataOutCur] );
        ChkBOOL( DRM_SUCCEEDED( dr ), DRM_E_CRYPTO_FAILED );
        cbDataLeft -= DRM_AES_BLOCKLEN;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*********************************************************************************************
** Function:  Oem_Aes_CbcDecryptData
**
** Synopsis:  Does AES CBC-Mode decryption on a buffer of data
**
** Arguments: [f_pKey]      : The AES secret key used to decrypt the buffer
**            [f_pbData]    : The buffer to decrypt ( in place )
**            [f_cbData]    : The number of bytes to decrypt
**            [f_rgbIV]     : The initialization vector to use for decryption
**
** Returns:   DRM_SUCCESS
**              Success
**            DRM_E_INVALIDARG
**               One of the pointers was NULL, or the byte count is 0
**               or not a multiple of DRM_AES_BLOCKLEN
**            DRM_E_CRYPTO_FAILED
**              The decrypt operation failed
**********************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Oem_Aes_CbcDecryptData(
    __in_ecount( 1 )                const DRM_AES_KEY *f_pKey,
    __inout_bcount( f_cbData )            DRM_BYTE    *f_pbData,
    __in                                  DRM_DWORD    f_cbData,
    __in_bcount( DRM_AES_BLOCKLEN ) const DRM_BYTE     f_rgbIV[__CB_DECL( DRM_AES_BLOCKLEN )] )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr           =  DRM_SUCCESS;
    DRM_DWORD  ibDataOutCur = 0;
    DRM_DWORD  i            = 0;
    DRM_BYTE   rgbTemp1[__CB_DECL( DRM_AES_BLOCKLEN )] = { 0 };
    DRM_BYTE   rgbTemp2[__CB_DECL( DRM_AES_BLOCKLEN )] = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMAES, PERF_FUNC_Oem_Aes_CbcDecryptData );

    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData >= DRM_AES_BLOCKLEN );
    ChkArg( f_cbData % DRM_AES_BLOCKLEN ==  0 );

    /*
    ** The first block is a special case: Use the IV to XOR
    */
    OEM_SECURE_MEMCPY( rgbTemp1, f_pbData, DRM_AES_BLOCKLEN );
    dr = Oem_Aes_DecryptOne( f_pKey, f_pbData );
    _XOR( f_pbData, 0, f_rgbIV, 0, DRM_AES_BLOCKLEN );
    ChkBOOL( DRM_SUCCEEDED( dr ), DRM_E_CRYPTO_FAILED );
    ibDataOutCur += DRM_AES_BLOCKLEN;

    for( ; ibDataOutCur < f_cbData; i++, ibDataOutCur += DRM_AES_BLOCKLEN )
    {
        OEM_SECURE_MEMCPY( ( i % 2 == 0? rgbTemp2 : rgbTemp1 ), f_pbData + ibDataOutCur, DRM_AES_BLOCKLEN );
        dr = Oem_Aes_DecryptOne( f_pKey, &f_pbData[ibDataOutCur] );
        ChkBOOL( DRM_SUCCEEDED( dr ), DRM_E_CRYPTO_FAILED );
        _XOR( f_pbData, ibDataOutCur, ( i % 2 == 0? rgbTemp1 : rgbTemp2 ), 0, DRM_AES_BLOCKLEN );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*********************************************************************************************
**
** Function:  Oem_Aes_EcbEncryptData
**
** Synopsis:  Does AES ECB-Mode encryption on a buffer of data
**
** Arguments: [f_pKey]      : The AES secret key used to encrypt  buffer
**            [f_pbData]    : The buffer to encrypt ( in place )
**            [f_cbData]    : The number of bytes to encrypt
**
** Returns:   DRM_SUCCESS
**               Success
**            DRM_E_INVALIDARG
**               One of the pointers was NULL, or the byte count is 0
**               or not a multiple of DRM_AES_BLOCKLEN
**********************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Oem_Aes_EcbEncryptData(
    __in_ecount( 1 )                const DRM_AES_KEY *f_pKey,
    __inout_bcount( f_cbData )            DRM_BYTE    *f_pbData,
    __in                                  DRM_DWORD    f_cbData ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Oem_Aes_EcbEncryptData(
    __in_ecount( 1 )                const DRM_AES_KEY *f_pKey,
    __inout_bcount( f_cbData )            DRM_BYTE    *f_pbData,
    __in                                  DRM_DWORD    f_cbData )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  ibData = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMAES, PERF_FUNC_Oem_Aes_EcbEncryptData );

    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData != 0 );
    ChkArg( f_pKey   != NULL );

    ChkArg( f_cbData % DRM_AES_BLOCKLEN == 0 );

    for ( ; ibData < f_cbData; ibData += DRM_AES_BLOCKLEN )
    {
        ChkDR( Oem_Aes_EncryptOne( f_pKey, &( f_pbData[ibData] ) ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*********************************************************************************************
**
** Function:  Oem_Aes_EcbDecryptData
**
** Synopsis:  Does AES ECB-Mode decryption on a buffer of data
**
** Arguments: [f_pKey]      : The AES secret key used to decrypt buffer
**            [f_pbData]    : The buffer to decrypt ( in place )
**            [f_cbData]    : The number of bytes to decrypt
**
** Returns:   DRM_SUCCESS
**               Success
**            DRM_E_INVALIDARG
**               One of the pointers was NULL, or the byte count is 0
**               or not a multiple of DRM_AES_BLOCKLEN
**********************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Oem_Aes_EcbDecryptData(
    __in_ecount( 1 )                const DRM_AES_KEY *f_pKey,
    __inout_bcount( f_cbData )            DRM_BYTE    *f_pbData,
    __in                                  DRM_DWORD    f_cbData ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Oem_Aes_EcbDecryptData(
    __in_ecount( 1 )                const DRM_AES_KEY *f_pKey,
    __inout_bcount( f_cbData )            DRM_BYTE    *f_pbData,
    __in                                  DRM_DWORD    f_cbData )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  ibData = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMAES, PERF_FUNC_Oem_Aes_EcbDecryptData );

    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData != 0 );
    ChkArg( f_pKey   != NULL );

    ChkArg( f_cbData % DRM_AES_BLOCKLEN == 0 );

    for ( ; ibData < f_cbData; ibData += DRM_AES_BLOCKLEN )
    {
        ChkDR( Oem_Aes_DecryptOne( f_pKey, &( f_pbData[ibData] ) ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*********************************************************************************************
**
** Function:  _Omac1_GenerateSignTag
**
** Synopsis:  Computes the OMAC1 sign tag
**
** Arguments: [f_pKey]      : The AES secret key
**            [f_pbData]    : The data to sign
**            [f_cbData]    : The number of bytes to sign
**            [f_rgbLU]     : Computed by _Omac1_GenerateSignInfo
**            [f_rgbLU_1]   : Computed by _Omac1_GenerateSignInfo
**            [f_rgbTag]    : The generated OMAC1 tag
**
** Returns:   DRM_SUCCESS
**              Success
**            DRM_E_INVALIDARG
**              One of the pointers was NULL, or the byte count is 0
**********************************************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _Omac1_GenerateSignTag(
    __in_ecount( 1 )                 const DRM_AES_KEY *f_pKey,
    __in_bcount( f_ibData+f_cbData ) const DRM_BYTE    *f_pbData,
    __in                                   DRM_DWORD    f_ibData,
    __in                                   DRM_DWORD    f_cbData,
    __in_bcount( DRM_AES_BLOCKLEN )  const DRM_BYTE     f_rgbLU  [__CB_DECL( DRM_AES_BLOCKLEN )],
    __in_bcount( DRM_AES_BLOCKLEN )  const DRM_BYTE     f_rgbLU_1[__CB_DECL( DRM_AES_BLOCKLEN )],
    __out_bcount( DRM_AES_BLOCKLEN )       DRM_BYTE     f_rgbTag [__CB_DECL( DRM_AES_BLOCKLEN )] ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT DRM_CALL _Omac1_GenerateSignTag(
    __in_ecount( 1 )                 const DRM_AES_KEY *f_pKey,
    __in_bcount( f_ibData+f_cbData ) const DRM_BYTE    *f_pbData,
    __in                                   DRM_DWORD    f_ibData,
    __in                                   DRM_DWORD    f_cbData,
    __in_bcount( DRM_AES_BLOCKLEN )  const DRM_BYTE     f_rgbLU  [__CB_DECL( DRM_AES_BLOCKLEN )],
    __in_bcount( DRM_AES_BLOCKLEN )  const DRM_BYTE     f_rgbLU_1[__CB_DECL( DRM_AES_BLOCKLEN )],
    __out_bcount( DRM_AES_BLOCKLEN )       DRM_BYTE     f_rgbTag [__CB_DECL( DRM_AES_BLOCKLEN )] )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  ibDataInCur  = 0;
    DRM_BYTE   rgbDataBlock    [__CB_DECL( DRM_AES_BLOCKLEN )] = { 0 };
    DRM_BYTE   rgbLastDataBlock[__CB_DECL( DRM_AES_BLOCKLEN )] = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMAES, PERF_FUNC_Omac1_GenerateSignTag );

    ChkArg( f_pKey   != NULL );
    ChkArg( f_pbData != NULL );
    ChkArg( f_cbData  > 0 );

    do
    {
        if( f_cbData > DRM_AES_BLOCKLEN )
        {
            OEM_SECURE_MEMCPY_IDX(rgbDataBlock, 0, f_pbData, f_ibData + ibDataInCur, DRM_AES_BLOCKLEN);
            DRM_XOR( rgbDataBlock, rgbLastDataBlock, DRM_AES_BLOCKLEN );
            OEM_SECURE_MEMCPY( rgbLastDataBlock, rgbDataBlock, DRM_AES_BLOCKLEN );

            Oem_Aes_EncryptOne( f_pKey, rgbLastDataBlock );

            f_cbData -= DRM_AES_BLOCKLEN;
            ibDataInCur += DRM_AES_BLOCKLEN;
        }
        else
        {
            if( f_cbData == DRM_AES_BLOCKLEN )
            {
                OEM_SECURE_MEMCPY_IDX(rgbDataBlock, 0, f_pbData, f_ibData + ibDataInCur, DRM_AES_BLOCKLEN);
                DRM_XOR( rgbDataBlock, rgbLastDataBlock, DRM_AES_BLOCKLEN );
                DRM_XOR( rgbDataBlock, f_rgbLU, DRM_AES_BLOCKLEN );
            }
            else
            {
                MEMSET( rgbDataBlock, 0, DRM_AES_BLOCKLEN );

                OEM_SECURE_MEMCPY_IDX(rgbDataBlock, 0, f_pbData, f_ibData + ibDataInCur, f_cbData);
                PUT_BYTE( rgbDataBlock, f_cbData, 0x80 );

                DRM_XOR( rgbDataBlock, rgbLastDataBlock, DRM_AES_BLOCKLEN );
                DRM_XOR( rgbDataBlock, f_rgbLU_1, DRM_AES_BLOCKLEN );
            }

            ChkDR( Oem_Aes_EncryptOne( f_pKey, rgbDataBlock ) );

            f_cbData = 0;
        }

    } while( f_cbData > 0 );

    OEM_SECURE_MEMCPY( f_rgbTag, rgbDataBlock, DRM_AES_BLOCKLEN );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*********************************************************************************************
**
** Function:  _Omac1_GenerateSignInfo
**
** Synopsis:  Computes the OMAC1 sign info
**
** Arguments: [f_pKey]      : The AES secret key
**            [f_rgbLU]     : Computed L.u from the OMAC algorithm
**            [f_rgbLU_1]   : Computed L.u2 from the OMAC algorithm
**
** Returns:   DRM_SUCCESS
**              Success
**            DRM_E_INVALIDARG
**              The key pointer was NULL
**********************************************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _Omac1_GenerateSignInfo(
    __in_ecount( 1 )                 const DRM_AES_KEY *f_pKey,
    __out_bcount( DRM_AES_BLOCKLEN )       DRM_BYTE     f_rgbLU  [__CB_DECL( DRM_AES_BLOCKLEN )],
    __out_bcount( DRM_AES_BLOCKLEN )       DRM_BYTE     f_rgbLU_1[__CB_DECL( DRM_AES_BLOCKLEN )] ) DRM_NO_INLINE_ATTRIBUTE;
static DRM_NO_INLINE DRM_RESULT DRM_CALL _Omac1_GenerateSignInfo(
    __in_ecount( 1 )                 const DRM_AES_KEY *f_pKey,
    __out_bcount( DRM_AES_BLOCKLEN )       DRM_BYTE     f_rgbLU  [__CB_DECL( DRM_AES_BLOCKLEN )],
    __out_bcount( DRM_AES_BLOCKLEN )       DRM_BYTE     f_rgbLU_1[__CB_DECL( DRM_AES_BLOCKLEN )] )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr                                       = DRM_SUCCESS;
    DRM_BYTE   rgbBuffer[__CB_DECL( DRM_AES_BLOCKLEN )] = { 0 };
    const DRM_BYTE bLU_ComputationConstant              = 0x87;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMAES, PERF_FUNC_Omac1_GenerateSignInfo );

    ChkArg( f_pKey != NULL );

    ChkDR( Oem_Aes_EncryptOne( f_pKey, rgbBuffer ) );

    /*
    ** Compute L.u from the OMAC algorithm
    */
    ChkDR( DRM_UTL_LShift( rgbBuffer, f_rgbLU, DRM_AES_BLOCKLEN ) );

    if( GET_BYTE( rgbBuffer, 0 ) & 0x80 )
    {
        PUT_BYTE( f_rgbLU, DRM_AES_BLOCKLEN - 1,
                  (DRM_BYTE)( GET_BYTE( f_rgbLU, DRM_AES_BLOCKLEN - 1 ) ^ bLU_ComputationConstant ) );
    }

    /*
    ** Compute L.u2 from the OMAC algorithm ( OMAC1 veriant )
    */
    ChkDR( DRM_UTL_LShift( f_rgbLU, f_rgbLU_1, DRM_AES_BLOCKLEN ) );

    if( GET_BYTE( f_rgbLU, 0 ) & 0x80 )
    {
        PUT_BYTE( f_rgbLU_1, DRM_AES_BLOCKLEN - 1,
                  (DRM_BYTE)( GET_BYTE( f_rgbLU_1, DRM_AES_BLOCKLEN - 1 ) ^ bLU_ComputationConstant ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*********************************************************************************************
**
** Function:  Oem_Omac1_Sign
**
** Synopsis:  Generates a signature using an OMAC of the data and an AES key
**
** Arguments: [f_pKey]      : The AES secret key
**            [f_pbData]    : The data to sign
**            [f_ibData]    : The starting byte offset of the data to sign
**            [f_cbData]    : The number of bytes to sign
**            [f_rgbTag]    : The generated OMAC1 tag
**
** Returns:   DRM_SUCCESS
**              Success
**            DRM_E_INVALIDARG
**              The plain text is too long or one of the pointers is NULL
**********************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Oem_Omac1_Sign(
    __in_ecount( 1 )                       const DRM_AES_KEY *f_pKey,
    __in_bcount( f_ibData + f_cbData )     const DRM_BYTE    *f_pbData,
    __in                                   DRM_DWORD    f_ibData,
    __in                                   DRM_DWORD    f_cbData,
    __out_bcount( DRM_AES_BLOCKLEN )       DRM_BYTE     f_rgbTag[__CB_DECL( DRM_AES_BLOCKLEN )] )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE   rgbLU  [__CB_DECL( DRM_AES_BLOCKLEN )] = { 0 };
    DRM_BYTE   rgbLU_1[__CB_DECL( DRM_AES_BLOCKLEN )] = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMAES, PERF_FUNC_Oem_Omac1_Sign );

    ChkArg( f_pbData != NULL && f_cbData > 0 );
    ChkArg( f_rgbTag != NULL );
    ChkArg( f_pKey   != NULL );

    ChkDR( _Omac1_GenerateSignInfo( f_pKey,
                                    rgbLU,
                                    rgbLU_1 ) );

    ChkDR( _Omac1_GenerateSignTag( f_pKey,
                                   f_pbData,
                                   f_ibData,
                                   f_cbData,
                                   rgbLU,
                                   rgbLU_1,
                                   f_rgbTag ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/*********************************************************************************************
**
** Function:  Oem_Omac1_Verify
**
** Synopsis:  Verifies a signature using an OMAC of the data and an AES key
**
** Arguments: [f_pKey]      : The AES secret key
**            [f_pbData]    : The signed data
**            [f_ibData]    : The starting byte offset of the data to verify
**            [f_cbData]    : The number of bytes signed
**            [f_rgbTag]    : The OMAC1 tag to verify
**
** Returns:   DRM_SUCCESS
**                The specified signature matches the computed signature
**            DRM_E_INVALID_SIGNATURE
**                The specified signature does not match the computed signature
**            DRM_E_INVALIDARG
**              One of the pointers is NULL
**********************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Oem_Omac1_Verify(
    __in_ecount( 1 )                                const DRM_AES_KEY *f_pKey,
    __in_bcount( f_ibData + f_cbData )              const DRM_BYTE    *f_pbData,
    __in                                            DRM_DWORD          f_ibData,
    __in                                            DRM_DWORD          f_cbData,
    __in_bcount( f_ibSignature + DRM_AES_BLOCKLEN ) const DRM_BYTE    *f_pbSignature,
    __in                                            DRM_DWORD          f_ibSignature ) DRM_NO_INLINE_ATTRIBUTE;
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL Oem_Omac1_Verify(
    __in_ecount( 1 )                                const DRM_AES_KEY *f_pKey,
    __in_bcount( f_ibData + f_cbData )              const DRM_BYTE    *f_pbData,
    __in                                            DRM_DWORD          f_ibData,
    __in                                            DRM_DWORD          f_cbData,
    __in_bcount( f_ibSignature + DRM_AES_BLOCKLEN ) const DRM_BYTE    *f_pbSignature,
    __in                                            DRM_DWORD          f_ibSignature )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE   rgbComputedSignature[__CB_DECL( DRM_AES_BLOCKLEN )] = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMAES, PERF_FUNC_Oem_Omac1_Verify );

    ChkArg( f_pKey   != NULL );
    ChkArg( f_pbData != NULL && f_cbData > 0 );

    /*
    ** Compute the signature
    */
    ChkDR( Oem_Omac1_Sign( f_pKey, f_pbData, f_ibData, f_cbData, rgbComputedSignature ) );

    /*
    ** Compare the computed signature to the passed in signature
    */
    ChkBOOL( DRM_BYT_CompareBytes( rgbComputedSignature, 0, f_pbSignature, f_ibSignature, DRM_AES_BLOCKLEN ) == 0, DRM_E_INVALID_SIGNATURE );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


EXIT_PK_NAMESPACE_CODE;
