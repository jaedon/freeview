/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <oemrsa.h>
#include <drmxmlbuilder.h>
#include <drmxmlbuildera.h>
#include <drmxmlrsakeys.h>
#include <drmutf.h>
#include <drmutilities.h>
#include <drmxmlsigconstants.h>
#include <drmndcertconstants.h>

ENTER_PK_NAMESPACE;

DRM_API DRM_RESULT DRM_CALL _BigEndianBytesToDigits(
    __in_ecount( BITS_TO_BYTES( f_cBitsData ) ) const DRM_BYTE  *f_pbData,
    __in const                                        DRM_DWORD  f_cBitsData,
    __out_ecount( BITS_TO_DIGITS( f_cBitsData ) )     digit_t   *f_pDigits
    );

DRM_API DRM_RESULT DRM_CALL _DigitsToBigEndianBytes(
    __in_ecount( BITS_TO_DIGITS( f_cBitsData ) ) const digit_t   *f_pDigits,
    __in                                               DRM_DWORD  f_cBitsData,
    __out_ecount( BITS_TO_BYTES( f_cBitsData ) )       DRM_BYTE  *f_pbData
    );

EXIT_PK_NAMESPACE;

ENTER_PK_NAMESPACE_CODE;

/******************************************************************************
**
** Function :   _ConvertB64DStrToASCII
**
** Synopsis :   Converts base64 encoded drm string into ASCII string
**              This function allocates memory for ASCII string, it is
**              responsibility of the caller to release allocated memory
**
** Arguments :  [f_pdstr]  - pointer to UTF-16 base64 encoded string
**              [f_pdastr] - pointer to empty ASCII string
**
** Returns :    DRM_SUCCESS - if conversion done successfully
**
******************************************************************************/
static DRM_RESULT _ConvertB64DStrToASCII(
    __in    const DRM_CONST_STRING *f_pdstr,
    __inout       DRM_ANSI_STRING  *f_pdastr )
{
    DRM_RESULT    dr     = DRM_SUCCESS;
    DRM_SUBSTRING dasstr = EMPTY_DRM_SUBSTRING;

    ChkDRMString( f_pdstr );
    ChkArg( f_pdastr != NULL );

    ChkDRMap( DRM_STR_DSTRtoUTF8( (const DRM_STRING*)f_pdstr,
                                  NULL,
                                  &f_pdastr->cchString ),
              DRM_E_BUFFERTOOSMALL, DRM_SUCCESS );

    dasstr.m_ich = 0;
    dasstr.m_cch = f_pdastr->cchString;

    ChkMem( f_pdastr->pszString = (DRM_CHAR*)Oem_MemAlloc( f_pdastr->cchString * SIZEOF(DRM_CHAR) ) );
    ChkDR( DRM_STR_DSTRtoUTF8( (const DRM_STRING*)f_pdstr,
                               f_pdastr->pszString,
                               &f_pdastr->cchString ) );

    ChkArg( f_pdastr->cchString == dasstr.m_cch );

    ChkDR( DRM_B64_DecodeA( f_pdastr->pszString,
                            &dasstr,
                            &f_pdastr->cchString,
                            (DRM_BYTE*)f_pdastr->pszString,
                            DRM_BASE64_DECODE_IN_PLACE ) );
ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        SAFE_OEM_FREE( f_pdastr->pszString );
    }
    return dr;
}

/******************************************************************************
**
** Function :   _ConvertDigitArrayToBigEndianB64EncodedWideString
**
** Synopsis :   Converts array of digit_t to big endian byte array
**              and then, base64 encodes it into UTF-16 string
**
** Arguments :  [f_pDigits]  - pointer to array of digit_t
**              [f_cDigit]   - length of f_pDigits array in digits
**              [f_pchData]  - location of UTF-16 base64 encoded big endian
**                             representation of input array
**              [f_pcchData] - location of length of f_pchData string
**
** Returns :    DRM_SUCCESS - if conversion was successful
**
** Notes:       Max length in digits of f_pDigits array must not exceed
**              DRM_RSA_CB_MODULUS_MAX
**
******************************************************************************/
static DRM_RESULT _ConvertDigitArrayToBigEndianB64EncodedWideString(
    __in_ecount( f_cDigit )     const digit_t   *f_pDigits,
    __in                        const DRM_DWORD  f_cDigit,
    __out_ecount( *f_pcchData )       DRM_WCHAR *f_pchData,
    __out                             DRM_DWORD *f_pcchData
    )
{
    DRM_RESULT dr = DRM_SUCCESS;
    /* Reserve buffer that sufficient for RSA encoding */
    DRM_BYTE  rgbBuffer [__CB_DECL( DRM_RSA_CB_MODULUS_MAX ) ] = { 0 };
    DRM_DWORD cbitBuffer = 0;

    AssertChkArg( f_pDigits != NULL );
    AssertChkArg( f_cDigit > 0 );
    AssertChkArg( f_pchData != NULL );
    AssertChkArg( f_pcchData != NULL );

    ChkOverflow( f_cDigit * DWORD_BITS, f_cDigit );

    /* digit_t is defined as DRM_DWORD */
    cbitBuffer = f_cDigit * DWORD_BITS;

    AssertChkArg( BITS_TO_BYTES( cbitBuffer ) <= DRM_RSA_CB_MODULUS_MAX );

    ChkDR( _DigitsToBigEndianBytes( f_pDigits,
                                    cbitBuffer,
                                    rgbBuffer ) );

    ChkDR( DRM_B64_EncodeW( rgbBuffer,
                            cbitBuffer / BITS_PER_BYTE,
                            f_pchData,
                            f_pcchData,
                            0 ) );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   _ConvertBigEndianB64EncodedWideStringToDigitArray
**
** Synopsis :   Converts base64 encoded wide character string representing
**              big endian byte array of digit_t  array
**
** Arguments :  [f_pwchData] - pointer to UTF-16 string representing
**                             base64 encoded big endian byte array
**              [f_cchData]  - length of f_pwchData string
**              [f_pDigits]  - location of digit_t array
**              [f_pcDigit]  - pointer to length of f_pDigits buffer in digit_t,
**                             after successful conversion it will have length f_pDigits array
**
** Returns :    DRM_SUCCESS - if conversion was successful
**
** Notes:       Max length in digits of f_pDigits array must not exceed
**              DRM_RSA_CB_MODULUS_MAX
**
******************************************************************************/
static DRM_RESULT _ConvertBigEndianB64EncodedWideStringToDigitArray(
    __in_ecount( f_cchData )   const DRM_WCHAR  *f_pwchData,
    __in                       const DRM_DWORD   f_cchData,
    __out_ecount( *f_pcDigit )       digit_t    *f_pDigits,
    __inout                          DRM_DWORD  *f_pcDigit
    )
{
    DRM_RESULT        dr             = DRM_SUCCESS;
    DRM_CONST_STRING  dstrTemp       = EMPTY_DRM_STRING;
    DRM_DWORD         cbDecodedArray = 0;

    DRMASSERT( f_pwchData != NULL );
    DRMASSERT( f_cchData > 0 );
    DRMASSERT( f_pDigits != NULL );
    DRMASSERT( f_pcDigit != NULL );

    dstrTemp.pwszString = f_pwchData;
    dstrTemp.cchString = f_cchData;

    /* Verify required buffer size in advance */
    ChkBOOL( *f_pcDigit * SIZEOF(digit_t) >= CB_BASE64_DECODE( f_cchData ) * SIZEOF(DRM_BYTE),
             DRM_E_BUFFERTOOSMALL );

    ChkDR( DRM_B64_DecodeW( &dstrTemp,
                            &cbDecodedArray,
                            NULL,
                            DRM_BASE64_DECODE_IN_PLACE ) );

    ChkBOOL( *f_pcDigit >= cbDecodedArray, DRM_E_BUFFERTOOSMALL );
    ChkDR( _BigEndianBytesToDigits( (DRM_BYTE*)dstrTemp.pwszString,
                                    cbDecodedArray * BITS_PER_BYTE,
                                    f_pDigits ) );

    *f_pcDigit = cbDecodedArray * SIZEOF(DRM_BYTE) / SIZEOF(digit_t);

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   _IsDefaultRSAPubExpIsUsed
**
** Synopsis :   Detects if default RSA public exponent is used
**
** Arguments :  [f_pKeyRSAPublic]    - pointer to public key data structure
**
** Returns :    TRUE - if default RSA public exponent is used
**
** Notes:       Used to optimize base64 encode form of RSA public exponent
******************************************************************************/
static DRM_BOOL _IsDefaultRSAPubExpIsUsed(
    __in  const DRM_RSA_PUBLIC_KEY *f_pKeyRSAPublic
    )
{
    INTERNAL_DRM_RSA_PUBLIC_KEY *pRSAPubKeyInternal = (INTERNAL_DRM_RSA_PUBLIC_KEY*)f_pKeyRSAPublic;

    DRMASSERT( f_pKeyRSAPublic != NULL );

    return pRSAPubKeyInternal->rgdPubExp[0] == DRM_RSA_DEFAULT_PUBEXP_DWORD;
}

/******************************************************************************
**
** Function :   DRM_XML_RSA_WritePublicKeyNodeA
**
** Synopsis :   Write a RSA public key node with the ds: namespace prefix
**              to ASCII XML context
**
** Arguments :  [f_pXMLCtxA]         - pointer to ASCII XML context
**              [f_pdastrWrapperTag] - pointer to optional wrapper tag
**              [f_pKeyRSAPublic]    - pointer to public key data structure
**
** Returns :    DRM_SUCCESS - if public key node was added successfully
**
** Notes:       Format:
**                     <Optional Wrapper Tag>
**                          <Modulus>  ... </Modulus>
**                          <Exponent> ... </Exponent>
**                     </Optional Wrapper Tag>
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XML_RSA_WritePublicKeyNodeA(
   __inout        _XMBContextA          *f_pXMLCtxA,
   __in_opt const DRM_ANSI_CONST_STRING *f_pdastrWrapperTag,
   __in     const DRM_RSA_PUBLIC_KEY    *f_pKeyRSAPublic )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_BYTE   rgbModulus [__CB_DECL( DRM_RSA_CB_MODULUS_MAX )] = { 0 };
    DRM_BYTE   rgbExponent[__CB_DECL( DRM_RSA_CB_PUBEXP )]      = { 0 };
    DRM_DWORD  cbModulus  = DRM_RSA_CB_MODULUS_MAX;
    DRM_DWORD  cbExponent = DRM_RSA_CB_PUBEXP;

    DRM_CHAR   rgchModulusB64[__CB_DECL( CCH_BASE64_EQUIV( DRM_RSA_CB_MODULUS_MAX ) )] = { 0 };
    DRM_CHAR   rgchExponentB64[__CB_DECL( CCH_BASE64_EQUIV( DRM_RSA_CB_PUBEXP ) )] = { 0 };
    DRM_ANSI_CONST_STRING dastrModulus  = EMPTY_DRM_STRING;
    DRM_ANSI_CONST_STRING dastrExponent = EMPTY_DRM_STRING;

    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength = eDRM_RSA_UNKNOWN;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_XML_RSA_WritePublicKeyNodeA );

    ChkDR( f_pXMLCtxA != NULL );
    ChkDR( f_pKeyRSAPublic != NULL );

    dastrModulus.cchString = CCH_BASE64_EQUIV( DRM_RSA_CB_MODULUS_MAX );
    dastrModulus.pszString = rgchModulusB64;

    dastrExponent.cchString = CCH_BASE64_EQUIV( DRM_RSA_CB_PUBEXP );
    dastrExponent.pszString = rgchExponentB64;

    ChkDR( OEM_RSA_ParsePublicKey(  f_pKeyRSAPublic,
                                   &eKeyLength,
                                   &cbExponent,
                                    rgbExponent,
                                   &cbModulus,
                                    rgbModulus ) );

    ChkDR( DRM_B64_EncodeA( rgbModulus,
                            cbModulus,
                            (DRM_CHAR *) dastrModulus.pszString,
                            &dastrModulus.cchString,
                            0 ) );

    if( _IsDefaultRSAPubExpIsUsed( f_pKeyRSAPublic ) )
    {
        /* Optimize for standard form of RSA public exponent */
        dastrExponent.pszString = g_dastrWMDRMCertExponent.pszString;
        dastrExponent.cchString = g_dastrWMDRMCertExponent.cchString;;
    }
    else
    {
        ChkDR( DRM_B64_EncodeA( rgbExponent,
                                cbExponent,
                                (DRM_CHAR *) dastrExponent.pszString,
                                &dastrExponent.cchString,
                                0 ) );
    }

    if ( f_pdastrWrapperTag != NULL )
    {
        ChkDRMANSIString( f_pdastrWrapperTag );
        ChkDR( DRM_XMB_WriteTagA( f_pXMLCtxA, f_pdastrWrapperTag, NULL, NULL, NULL, wttOpen ) );
    }

    ChkDR( DRM_XMB_WriteTagA( f_pXMLCtxA, &g_dastrTagModulus,  &dastrModulus,   NULL, NULL, wttClosed ) );
    ChkDR( DRM_XMB_WriteTagA( f_pXMLCtxA, &g_dastrTagExponent, &dastrExponent,  NULL, NULL, wttClosed ) );

    if ( f_pdastrWrapperTag != NULL )
    {
        ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLCtxA, NULL ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XML_RSA_WritePublicKeyNode
**
** Synopsis :   Write a RSA public key node with the ds: namespace prefix
**              to UTF-16 XML context
**
** Arguments :  [f_pXMLCtx]         - pointer to UTF16 XML context
**              [f_pdstrWrapperTag] - pointer to optional wrapper tag
**              [f_pKeyRSAPublic]   - pointer to public key data structure
**
** Returns :    DRM_SUCCESS - if public key node was added successfully
**
** Notes:       Format:
**                     <Optional Wrapper Tag>
**                          <Modulus>  ... </Modulus>
**                          <Exponent> ... </Exponent>
**                     </Optional Wrapper Tag>
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XML_RSA_WritePublicKeyNode(
   __inout        _XMBContext        *f_pXMLCtx,
   __in_opt const DRM_CONST_STRING   *f_pdstrWrapperTag,
   __in     const DRM_RSA_PUBLIC_KEY *f_pKeyRSAPublic )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_BYTE   rgbModulus [__CB_DECL( DRM_RSA_CB_MODULUS_MAX )] = { 0 };
    DRM_BYTE   rgbExponent[__CB_DECL( DRM_RSA_CB_PUBEXP )]      = { 0 };
    DRM_DWORD  cbModulus  = DRM_RSA_CB_MODULUS_MAX;
    DRM_DWORD  cbExponent = DRM_RSA_CB_PUBEXP;

    DRM_WCHAR  rgwchModulusB64[__CB_DECL( CCH_BASE64_EQUIV( DRM_RSA_CB_MODULUS_MAX ) )] = { 0 };
    DRM_WCHAR  rgwchExponentB64[__CB_DECL( CCH_BASE64_EQUIV( DRM_RSA_CB_PUBEXP ) )] = { 0 };
    DRM_CONST_STRING dstrModulus  = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrExponent = EMPTY_DRM_STRING;

    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength = eDRM_RSA_UNKNOWN;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_XML_RSA_WritePublicKeyNode );

    ChkDR( f_pXMLCtx != NULL );
    ChkDR( f_pKeyRSAPublic != NULL );

    dstrModulus.cchString = CCH_BASE64_EQUIV( DRM_RSA_CB_MODULUS_MAX );
    dstrModulus.pwszString = rgwchModulusB64;

    dstrExponent.cchString = CCH_BASE64_EQUIV( DRM_RSA_CB_PUBEXP );
    dstrExponent.pwszString = rgwchExponentB64;

    ChkDR( OEM_RSA_ParsePublicKey(  f_pKeyRSAPublic,
                                   &eKeyLength,
                                   &cbExponent,
                                    rgbExponent,
                                   &cbModulus,
                                    rgbModulus ) );

    ChkDR( DRM_B64_EncodeW( rgbModulus,
                            cbModulus,
                            (DRM_WCHAR *) dstrModulus.pwszString,
                            &dstrModulus.cchString,
                            0 ) );

    if( _IsDefaultRSAPubExpIsUsed( f_pKeyRSAPublic ) )
    {
        /* Optimize for standard form of RSA public exponent */
        dstrExponent.pwszString = g_dstrWMDRMCertExponent.pwszString;
        dstrExponent.cchString = g_dstrWMDRMCertExponent.cchString;
    }
    else
    {
        ChkDR( DRM_B64_EncodeW( rgbExponent,
                                cbExponent,
                                (DRM_WCHAR *) dstrExponent.pwszString,
                                &dstrExponent.cchString,
                                0 ) );
    }

    if ( f_pdstrWrapperTag != NULL )
    {
        ChkDRMString( f_pdstrWrapperTag );
        ChkDR( DRM_XMB_WriteTag( f_pXMLCtx, f_pdstrWrapperTag, NULL, NULL, NULL, wttOpen ) );
    }
     ChkDR( DRM_XMB_WriteTag( f_pXMLCtx, &g_dstrTagModulus,  &dstrModulus,   NULL, NULL, wttClosed ) );
     ChkDR( DRM_XMB_WriteTag( f_pXMLCtx, &g_dstrTagExponent, &dstrExponent,  NULL, NULL, wttClosed ) );

    if ( f_pdstrWrapperTag != NULL )
    {
         ChkDR( DRM_XMB_CloseCurrNode( f_pXMLCtx, NULL ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XML_RSA_ParseBase64PublicKey
**
** Synopsis :   Parse UTF-16 base64 encoded data to get the RSA public key
**
** Arguments :  [f_dstrPubKeyModulus]  - pointer to UTF-16 base64 encoded RSA public key modulus
**              [f_dstrPubKeyExponent] - pointer to UTF-16 base64 encoded RSA public key exponent
**              [f_pKeyRSAPublic] - pointer to public key data structure
**
** Returns :    DRM_SUCCESS - if public key node was parsed successfully
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XML_RSA_ParseBase64PublicKey(
    __in    const DRM_CONST_STRING   *f_dstrPubKeyModulus,
    __in    const DRM_CONST_STRING   *f_dstrPubKeyExponent,
    __inout       DRM_RSA_PUBLIC_KEY *f_pKeyRSAPublic )
{
    DRM_RESULT dr             = DRM_SUCCESS;
    DRM_BOOL   fDefaultPubExp = FALSE;
    DRM_ANSI_STRING dastrPubKeyModulus  = EMPTY_DRM_STRING;
    DRM_ANSI_STRING dastrPubKeyExponent = EMPTY_DRM_STRING;
    DRM_RSA_SUPPORTED_KEY_LENGTHS eRSAKeyLength = eDRM_RSA_UNKNOWN;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_XML_RSA_ParseBase64PublicKey );

    ChkDRMString( f_dstrPubKeyModulus );
    ChkDRMString( f_dstrPubKeyExponent );
    ChkArg( f_pKeyRSAPublic != NULL );

    /* Check if default public exponent is used */
    fDefaultPubExp = f_dstrPubKeyExponent->cchString == g_dstrWMDRMCertExponent.cchString &&
                     ( MEMCMP( f_dstrPubKeyExponent->pwszString,
                               g_dstrWMDRMCertExponent.pwszString,
                               g_dstrWMDRMCertExponent.cchString*SIZEOF(DRM_WCHAR) ) == 0 );

    ChkDR( _ConvertB64DStrToASCII( f_dstrPubKeyModulus,  &dastrPubKeyModulus  ) );
    if ( !fDefaultPubExp )
    {
        ChkDR( _ConvertB64DStrToASCII( f_dstrPubKeyExponent, &dastrPubKeyExponent ) );
    }
    ChkDR( OEM_RSA_GetKeyLengthFromModulusLength( dastrPubKeyModulus.cchString, &eRSAKeyLength ) );

    ChkDR( OEM_RSA_ZeroPublicKey( f_pKeyRSAPublic ) );
    if( fDefaultPubExp )
    {
        ChkDR( OEM_RSA_SetPublicKey( eRSAKeyLength,
                                     DRM_RSA_CB_PUBEXP,
                                     rgbDRM_RSA_DEFAULT_PUBEXP,
                                     dastrPubKeyModulus.cchString,
                                     (DRM_BYTE*)dastrPubKeyModulus.pszString,
                                     f_pKeyRSAPublic ) );
    }
    else
    {
        ChkDR( OEM_RSA_SetPublicKey( eRSAKeyLength,
                                     dastrPubKeyExponent.cchString,
                                     (DRM_BYTE*)dastrPubKeyExponent.pszString,
                                     dastrPubKeyModulus.cchString,
                                     (DRM_BYTE*)dastrPubKeyModulus.pszString,
                                     f_pKeyRSAPublic ) );
    }
ErrorExit:
    SAFE_OEM_FREE( dastrPubKeyModulus.pszString );
    SAFE_OEM_FREE( dastrPubKeyExponent.pszString );
    if ( DRM_FAILED( dr ) && f_pKeyRSAPublic != NULL )
    {
        OEM_RSA_ZeroPublicKey( f_pKeyRSAPublic );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XML_RSA_ParseBase64PublicKeyA
**
** Synopsis :   Parse UTF-8 base64 encoded data to get the RSA public key
**
** Arguments :  [f_dastrPubKeyModulus]  - pointer to buffer
**                                        containing UTF-8 base64 encoded RSA public key modulus.
**              [f_dasstrPubKeyModulus] - location of UTF-8 base64 encoded RSA public key modulus.
**              [f_dastrPubKeyExponent] - pointer to buffer
**                                        containing UTF-8 base64 encoded RSA public key exponent.
**              [f_dasstrPubKeyExponent]- location of UTF-8 base64 encoded RSA public key exponent.
**              [f_pKeyRSAPublic] - pointer to public key data structure.
**
** Returns :    DRM_SUCCESS - if public key node was parsed successfully .
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XML_RSA_ParseBase64PublicKeyA(
    __in    const DRM_ANSI_CONST_STRING *f_dastrPubKeyModulus,
    __in    const DRM_SUBSTRING         *f_dasstrPubKeyModulus,
    __in    const DRM_ANSI_CONST_STRING *f_dastrPubKeyExponent,
    __in    const DRM_SUBSTRING         *f_dasstrPubKeyExponent,
    __inout       DRM_RSA_PUBLIC_KEY    *f_pKeyRSAPublic )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL   fDefaultPubExp = FALSE;
    DRM_BYTE   rgbModulus [ __CB_DECL( DRM_RSA_CB_MODULUS_MAX ) ] = { 0 };
    DRM_BYTE   rgbExponent[ __CB_DECL( DRM_RSA_CB_PUBEXP ) ]      = { 0 };
    DRM_DWORD  cchModulus  = DRM_RSA_CB_MODULUS_MAX;
    DRM_DWORD  cchExponent = DRM_RSA_CB_PUBEXP;
    DRM_RSA_SUPPORTED_KEY_LENGTHS eRSAKeyLength = eDRM_RSA_UNKNOWN;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_XML_RSA_ParseBase64PublicKeyA );

    ChkDRMANSIString( f_dastrPubKeyModulus );
    ChkArg( f_dasstrPubKeyModulus != NULL );
    ChkDRMANSIString( f_dastrPubKeyExponent );
    ChkArg( f_dasstrPubKeyExponent != NULL );
    ChkArg( f_pKeyRSAPublic != NULL );

    ZEROMEM( rgbModulus, DRM_RSA_CB_MODULUS_MAX );
    ZEROMEM( rgbExponent, DRM_RSA_CB_PUBEXP );

    /* Check if default public exponent is used */
    fDefaultPubExp = f_dasstrPubKeyExponent->m_cch == g_dastrWMDRMCertExponent.cchString &&
                     ( MEMCMP( f_dastrPubKeyExponent->pszString + f_dasstrPubKeyExponent->m_ich,
                               g_dastrWMDRMCertExponent.pszString,
                               g_dastrWMDRMCertExponent.cchString*SIZEOF(DRM_CHAR) ) == 0 );

    ChkDR( DRM_B64_DecodeA( f_dastrPubKeyModulus->pszString,
                            f_dasstrPubKeyModulus,
                           &cchModulus,
                            rgbModulus,
                            0 ) );

    if( !fDefaultPubExp )
    {
        ChkDR( DRM_B64_DecodeA( f_dastrPubKeyExponent->pszString,
                                f_dasstrPubKeyExponent,
                               &cchExponent,
                                rgbExponent,
                                0 ) );
    }
    ChkDR( OEM_RSA_GetKeyLengthFromModulusLength( cchModulus, &eRSAKeyLength ) );

    ChkDR( OEM_RSA_ZeroPublicKey( f_pKeyRSAPublic ) );
    if( fDefaultPubExp )
    {
        ChkDR( OEM_RSA_SetPublicKey( eRSAKeyLength,
                                     DRM_RSA_CB_PUBEXP,
                                     rgbDRM_RSA_DEFAULT_PUBEXP,
                                     DRM_RSA_CB_MODULUS( eRSAKeyLength ),
                                     rgbModulus,
                                     f_pKeyRSAPublic ) );
    }
    else
    {
        ChkDR( OEM_RSA_SetPublicKey( eRSAKeyLength,
                                     DRM_RSA_CB_PUBEXP,
                                     rgbExponent,
                                     DRM_RSA_CB_MODULUS( eRSAKeyLength ),
                                     rgbModulus,
                                     f_pKeyRSAPublic ) );
    }
ErrorExit:
    if ( DRM_FAILED( dr ) && f_pKeyRSAPublic != NULL )
    {
        OEM_RSA_ZeroPublicKey( f_pKeyRSAPublic );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XML_RSA_WritePrivateKeyNode
**
** Synopsis :   Write a <PrivateKey> node to ASCII XML context
**
** Arguments :  [f_pXMLCtxA]        - pointer to ASCII XML context
**              [f_pdstrWrapperTag] - pointer to optional wrapper tag
**              [f_pKeyRSAPrivate]  - pointer to public key data structure
**
** Returns :    DRM_SUCCESS - if private key node was written successfully
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XML_RSA_WritePrivateKeyNode(
    __inout        _XMBContext         *f_pXMLCtx,
    __in_opt const DRM_CONST_STRING    *f_pdstrWrapperTag,
    __in     const DRM_RSA_PRIVATE_KEY *f_pKeyRSAPrivate )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_WCHAR  rgwchPrime0B64      [__CB_DECL( CCH_BASE64_EQUIV( DRM_RSA_CB_PRIME_MAX ) )]   = { 0 };
    DRM_WCHAR  rgwchPrime1B64      [__CB_DECL( CCH_BASE64_EQUIV( DRM_RSA_CB_PRIME_MAX ) )]   = { 0 };
    DRM_WCHAR  rgwchCRTExponent0B64[__CB_DECL( CCH_BASE64_EQUIV( DRM_RSA_CB_PRIVEXP_MAX ) )] = { 0 };
    DRM_WCHAR  rgwchCRTExponent1B64[__CB_DECL( CCH_BASE64_EQUIV( DRM_RSA_CB_PRIVEXP_MAX ) )] = { 0 };
    DRM_WCHAR  rgwchIQMPB64        [__CB_DECL( CCH_BASE64_EQUIV( DRM_RSA_CB_MODULUS_MAX ) )] = { 0 };

    DRM_CONST_STRING dstrPrime0       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrime1       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrCRTExponent0 = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrCRTExponent1 = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrIQMP         = EMPTY_DRM_STRING;
    INTERNAL_DRM_RSA_PRIVATE_KEY *pPvtKey = ( INTERNAL_DRM_RSA_PRIVATE_KEY * )f_pKeyRSAPrivate;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_XML_RSA_WritePrivateKeyNode );

    ChkDR( f_pXMLCtx != NULL );
    ChkDR( f_pKeyRSAPrivate != NULL );

    dstrPrime0.cchString        = CCH_BASE64_EQUIV( DRM_RSA_CB_PRIME_MAX );
    dstrPrime0.pwszString       = rgwchPrime0B64;
    dstrPrime1.cchString        = CCH_BASE64_EQUIV( DRM_RSA_CB_PRIME_MAX );
    dstrPrime1.pwszString       = rgwchPrime1B64;
    dstrCRTExponent0.cchString  = CCH_BASE64_EQUIV( DRM_RSA_CB_PRIME_MAX );
    dstrCRTExponent0.pwszString = rgwchCRTExponent0B64;
    dstrCRTExponent1.cchString  = CCH_BASE64_EQUIV( DRM_RSA_CB_PRIME_MAX );
    dstrCRTExponent1.pwszString = rgwchCRTExponent1B64;
    dstrIQMP.cchString          = CCH_BASE64_EQUIV( DRM_RSA_CB_PRIME_MAX );
    dstrIQMP.pwszString         = rgwchIQMPB64;

    ChkDR( _ConvertDigitArrayToBigEndianB64EncodedWideString( pPvtKey->rgdPrimes[0],
                                                              pPvtKey->cdPrimes[0],
                                                              (DRM_WCHAR *) dstrPrime0.pwszString,
                                                              &dstrPrime0.cchString ) );

    ChkDR( _ConvertDigitArrayToBigEndianB64EncodedWideString( pPvtKey->rgdPrimes[1],
                                                              pPvtKey->cdPrimes[1],
                                                              (DRM_WCHAR *) dstrPrime1.pwszString,
                                                              &dstrPrime1.cchString ) );

    ChkDR( _ConvertDigitArrayToBigEndianB64EncodedWideString( pPvtKey->rgdCRTExponents[0],
                                                              pPvtKey->cdPrimes[0],
                                                              (DRM_WCHAR *) dstrCRTExponent0.pwszString,
                                                              &dstrCRTExponent0.cchString ) );

    ChkDR( _ConvertDigitArrayToBigEndianB64EncodedWideString( pPvtKey->rgdCRTExponents[1],
                                                              pPvtKey->cdPrimes[1],
                                                              (DRM_WCHAR *) dstrCRTExponent1.pwszString,
                                                              &dstrCRTExponent1.cchString ) );

    ChkDR( _ConvertDigitArrayToBigEndianB64EncodedWideString( pPvtKey->rgdIQMP,
                                                              pPvtKey->cdPrimes[0] + pPvtKey->cdPrimes[1],
                                                              (DRM_WCHAR *) dstrIQMP.pwszString,
                                                              &dstrIQMP.cchString ) );

    if ( f_pdstrWrapperTag != NULL )
    {
        ChkDRMString( f_pdstrWrapperTag );
        ChkDR( DRM_XMB_WriteTag( f_pXMLCtx, f_pdstrWrapperTag, NULL, NULL, NULL, wttOpen ) );
    }
    ChkDR( DRM_XMB_WriteTag( f_pXMLCtx, &g_dstrTagPrime0,       &dstrPrime0,       NULL, NULL, wttClosed ) );
    ChkDR( DRM_XMB_WriteTag( f_pXMLCtx, &g_dstrTagPrime1,       &dstrPrime1,       NULL, NULL, wttClosed ) );
    ChkDR( DRM_XMB_WriteTag( f_pXMLCtx, &g_dstrTagCRTExponent0, &dstrCRTExponent0, NULL, NULL, wttClosed ) );
    ChkDR( DRM_XMB_WriteTag( f_pXMLCtx, &g_dstrTagCRTExponent1, &dstrCRTExponent1, NULL, NULL, wttClosed ) );
    ChkDR( DRM_XMB_WriteTag( f_pXMLCtx, &g_dstrTagIQMP,         &dstrIQMP,         NULL, NULL, wttClosed ) );

    if ( f_pdstrWrapperTag != NULL )
    {
         ChkDR( DRM_XMB_CloseCurrNode( f_pXMLCtx, NULL ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   DRM_XML_RSA_ParseBase64PrivateKey
**
** Synopsis :   Parse UTF-16 base64 encoded data to get the RSA public key
**
** Arguments :  [f_dstrPrivKeyPrime0]       - pointer to UTF-16 base64 encoded RSA private key prime0
**              [f_dstrPrivKeyPrime1]       - pointer to UTF-16 base64 encoded RSA private key prime1
**              [f_dstrPrivKeyCRTExponent0] - pointer to UTF-16 base64 encoded RSA private key CRTExponent0
**              [f_dstrPrivKeyCRTExponent1] - pointer to UTF-16 base64 encoded RSA private key CRTExponent1
**              [f_dstrPrivKeyIQMP]         - pointer to UTF-16 base64 encoded RSA private key IQMP
**
** Returns :    DRM_SUCCESS - if private key node was parsed successfully
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_XML_RSA_ParseBase64PrivateKey(
    __in    const DRM_CONST_STRING    *f_dstrPrivKeyPrime0,
    __in    const DRM_CONST_STRING    *f_dstrPrivKeyPrime1,
    __in    const DRM_CONST_STRING    *f_dstrPrivKeyCRTExponent0,
    __in    const DRM_CONST_STRING    *f_dstrPrivKeyCRTExponent1,
    __in    const DRM_CONST_STRING    *f_dstrPrivKeyIQMP,
    __inout       DRM_RSA_PRIVATE_KEY *f_pKeyRSAPrivate )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_DWORD  dwTempSize = 0;
    INTERNAL_DRM_RSA_PRIVATE_KEY *pPrivKey = ( INTERNAL_DRM_RSA_PRIVATE_KEY * )f_pKeyRSAPrivate;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_XML_RSA_ParseBase64PrivateKey );

    ChkDRMString( f_dstrPrivKeyPrime0 );
    ChkDRMString( f_dstrPrivKeyPrime1 );
    ChkDRMString( f_dstrPrivKeyCRTExponent0 );
    ChkDRMString( f_dstrPrivKeyCRTExponent1 );
    ChkDRMString( f_dstrPrivKeyIQMP );
    ChkDR( f_pKeyRSAPrivate != NULL );

    ChkDR( OEM_RSA_ZeroPrivateKey( f_pKeyRSAPrivate ) );

    pPrivKey->cdPrimes[0] = CDIGITS_MODULUS_MAX * SIZEOF(digit_t);
    ChkDR( _ConvertBigEndianB64EncodedWideStringToDigitArray( f_dstrPrivKeyPrime0->pwszString,
                                                              f_dstrPrivKeyPrime0->cchString,
                                                              pPrivKey->rgdPrimes[0],
                                                              &pPrivKey->cdPrimes[0] ) );

    pPrivKey->cdPrimes[1] = CDIGITS_MODULUS_MAX * SIZEOF(digit_t);
    ChkDR( _ConvertBigEndianB64EncodedWideStringToDigitArray( f_dstrPrivKeyPrime1->pwszString,
                                                              f_dstrPrivKeyPrime1->cchString,
                                                              pPrivKey->rgdPrimes[1],
                                                              &pPrivKey->cdPrimes[1] ) );

    dwTempSize = CDIGITS_MODULUS_MAX * SIZEOF(digit_t);
    ChkDR( _ConvertBigEndianB64EncodedWideStringToDigitArray( f_dstrPrivKeyCRTExponent0->pwszString,
                                                              f_dstrPrivKeyCRTExponent0->cchString,
                                                              pPrivKey->rgdCRTExponents[0],
                                                              &dwTempSize) );

    dwTempSize = CDIGITS_MODULUS_MAX * SIZEOF(digit_t);
    ChkDR( _ConvertBigEndianB64EncodedWideStringToDigitArray( f_dstrPrivKeyCRTExponent1->pwszString,
                                                              f_dstrPrivKeyCRTExponent1->cchString,
                                                              pPrivKey->rgdCRTExponents[1],
                                                              &dwTempSize) );

    dwTempSize = CDIGITS_MODULUS_MAX * SIZEOF(digit_t);
    ChkDR( _ConvertBigEndianB64EncodedWideStringToDigitArray( f_dstrPrivKeyIQMP->pwszString,
                                                              f_dstrPrivKeyIQMP->cchString,
                                                              pPrivKey->rgdIQMP,
                                                              &dwTempSize) );

    /* Length of modulus is the same as sum of length of private key primes */
    ChkDR( OEM_RSA_GetKeyLengthFromModulusLength( ( pPrivKey->cdPrimes[0] + pPrivKey->cdPrimes[1] ) * SIZEOF(digit_t),
                                                  &pPrivKey->eKeyLength ) );
ErrorExit:
    if ( DRM_FAILED( dr ) && f_pKeyRSAPrivate != NULL )
    {
        OEM_RSA_ZeroPrivateKey( f_pKeyRSAPrivate );
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


EXIT_PK_NAMESPACE_CODE;

