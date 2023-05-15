/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmcommon.h>
#include <drmsoapxmlconstants.h>
#include <drmlicacqv3constants.h>
#include <drmmanager.h>
#include <drmxmrbuilder.h>
#include <drmkeygenerator.h>
#include <drmbcertparser.h>
#include <drmhdrbuilder.h>
#include <oemrsaoaeppss.h>
#include <tclib.h>
#include <TestVariableArray.h>
#include <licgenxmr.h>
#include <drmsoapxmlutility.h>

ENTER_PKTEST_NAMESPACE_CODE

static const DRM_BYTE g_rgbRSAModulus1024[__CB_DECL( DRM_RSA_CB_MODULUS( eDRM_RSA_1024 ) )] =
{
    0x97, 0x91, 0x29, 0x85,
    0xDD, 0x1F, 0x9E, 0x45,
    0xE3, 0x7B, 0x19, 0x31,
    0x1B, 0x3C, 0xC0, 0xA9,
    0xB5, 0xE9, 0xE9, 0x38,
    0x34, 0xB0, 0xA3, 0x16,
    0x03, 0x3B, 0x9B, 0xA3,
    0x13, 0xA2, 0xC9, 0x7D,
    0x37, 0x70, 0xC4, 0xB5,
    0xF3, 0x19, 0x84, 0x7A,
    0x9C, 0x07, 0x8D, 0x0B,
    0xD9, 0x32, 0x6A, 0x4E,
    0xA5, 0x12, 0x45, 0xEF,
    0x63, 0x57, 0x75, 0xB1,
    0x30, 0x99, 0x76, 0x1A,
    0x3D, 0xA3, 0xB7, 0xE9,
    0xE6, 0xFB, 0xA5, 0x31,
    0xAD, 0xA4, 0xF8, 0xCD,
    0xAD, 0xBD, 0xB6, 0x02,
    0xAF, 0xE3, 0x41, 0x9F,
    0x9C, 0x57, 0xB7, 0x59,
    0x7F, 0x00, 0x6E, 0xD9,
    0x45, 0x1D, 0x86, 0x15,
    0x15, 0x80, 0x8F, 0xCD,
    0xC0, 0xE3, 0xDB, 0xEC,
    0xE4, 0x74, 0xE6, 0x5C,
    0x00, 0x11, 0x31, 0xC2,
    0x74, 0xCD, 0xAF, 0x42,
    0x51, 0xE3, 0xE9, 0x21,
    0x24, 0x4A, 0x4F, 0x3B,
    0x59, 0x0D, 0x45, 0x6D,
    0x70, 0xCA, 0x56, 0x43
};

/*
** Constant strings used to build V3 license response.
*/
static DRM_ANSI_CONST_STRING g_dastrAcquireLicenseResponseAttribName  = CREATE_DRM_ANSI_STRING( "xmlns" );
static DRM_ANSI_CONST_STRING g_dastrAcquireLicenseResponseAttribValue = CREATE_DRM_ANSI_STRING( "http://schemas.microsoft.com/DRM/2007/03/protocols" );

static DRM_ANSI_CONST_STRING g_dastrResponseTag                       = CREATE_DRM_ANSI_STRING( "Response" );
static DRM_ANSI_CONST_STRING g_dastrResponseAttribName                = CREATE_DRM_ANSI_STRING( "xmlns" );
static DRM_ANSI_CONST_STRING g_dastrResponseAttribValue               = CREATE_DRM_ANSI_STRING( "http://schemas.microsoft.com/DRM/2007/03/protocols/messages" );

static DRM_ANSI_CONST_STRING g_dastrSLKTag                            = CREATE_DRM_ANSI_STRING( "SLK" );
static DRM_ANSI_CONST_STRING g_dastrSLKIDTag                          = CREATE_DRM_ANSI_STRING( "SLKID" );
static DRM_ANSI_CONST_STRING g_dastrSLKDATATag                        = CREATE_DRM_ANSI_STRING( "SLKDATA" );

static DRM_ANSI_CONST_STRING g_dastrLicenseResponseTag                = CREATE_DRM_ANSI_STRING( "LicenseResponse" );
static DRM_ANSI_CONST_STRING g_dastrLicenseResponseAttrib1Name        = CREATE_DRM_ANSI_STRING( "xmlns" );
static DRM_ANSI_CONST_STRING g_dastrLicenseResponseAttrib1Value       = CREATE_DRM_ANSI_STRING( "http://schemas.microsoft.com/DRM/2007/03/protocols" );
static DRM_ANSI_CONST_STRING g_dastrLicenseResponseAttrib2Name        = CREATE_DRM_ANSI_STRING( "Id" );
static DRM_ANSI_CONST_STRING g_dastrLicenseResponseAttrib2Value       = CREATE_DRM_ANSI_STRING( "SignedData" );

static DRM_ANSI_CONST_STRING g_dastrLicensesTag                       = CREATE_DRM_ANSI_STRING( "Licenses" );

static DRM_ANSI_CONST_STRING g_dastrRevInfoTag                       = CREATE_DRM_ANSI_STRING( "RevInfo" );

static DRM_ANSI_CONST_STRING g_dastrAcknowledgementTag                = CREATE_DRM_ANSI_STRING( "Acknowledgement" );

/******************************************************************************
**
** Function :   _SignXmrLicense
**
** Synopsis :   This is implementation of DRM_XMR_CB_SIGNATURE callback function
**
** Arguments :  f_pvSignatureContext  - Context needed by caller to retrieve
**                                      appropriate integrity key
**              f_pbLicense           - Pointer to license buffer
**              f_cbLicense           - Size of license buffer
**              f_pbSignatureBuffer   - Pointer to output signature buffer
**              f_cbSignatureBuffer   - Size of signature buffer
**              f_ibSignatureBuffer   - Offset (index) in signature buffer
**                                      where to start signature writing
**
** Returns :    DRM_SUCCESS if signing was successful
**
******************************************************************************/
static DRM_RESULT DRM_CALL _SignXmrLicense(
    __in                                const DRM_VOID  *f_pvSignatureContext,
    __in_bcount( f_cbLicense )          const DRM_BYTE  *f_pbLicense,
    __in                                const DRM_DWORD  f_cbLicense,
    __out_bcount( f_cbSignatureBuffer )
                                              DRM_BYTE  *f_pbSignatureBuffer,
    __in                                const DRM_DWORD  f_cbSignatureBuffer,
    __in                                const DRM_DWORD  f_ibSignatureBuffer )
{
    DRM_RESULT  dr     = DRM_SUCCESS;
    DRM_AES_KEY aesKey = {0};
    DRM_BYTE    rgbSignature[__CB_DECL(XMR_AES_OMAC_SIGNATURE_LENGTH)];

    /*
    ** Sanity check the arguments
    */
    DRMASSERT( f_pvSignatureContext != NULL );
    DRMASSERT( f_pbSignatureBuffer  != NULL );
    ChkArg( f_cbSignatureBuffer - f_ibSignatureBuffer == XMR_AES_OMAC_SIGNATURE_LENGTH );
    ChkArg( f_pbLicense          != NULL );
    ChkArg( f_cbLicense          > 0 );
    ChkArg( f_pbLicense          == f_pbSignatureBuffer );

    /*
    ** Sign the license with integrity key
    */
    ChkDR( Oem_Aes_SetKey((DRM_BYTE*)f_pvSignatureContext, &aesKey) );
    ChkDR( Oem_Omac1_Sign(&aesKey, f_pbLicense, 0, f_cbLicense, rgbSignature ) );
    DRM_BYT_CopyBytes(f_pbSignatureBuffer, f_ibSignatureBuffer, rgbSignature, 0, XMR_AES_OMAC_SIGNATURE_LENGTH);

ErrorExit:
    return dr;
}

static DRM_RESULT _CreateChecksum(
    __in                            const DRM_KID         *f_pKID,
    __in_bcount( f_cbKey )          const DRM_BYTE        *f_pbKey,
    __in                                  DRM_DWORD        f_cbKey,
    __out_bcount( DRM_AES_CK_CHECKSUM_LENGTH ) DRM_BYTE    f_rgbChecksum[__CB_DECL( DRM_AES_CK_CHECKSUM_LENGTH )] )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_BYTE rgbBlock[__CB_DECL(SIZEOF( DRM_KID ))] = {0};
    DRM_AES_KEY aesKey = {0};

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_BLACKBOX, PERF_FUNC_VerifyChecksum);

    ChkArg( f_pKID != NULL );

    if ( f_pbKey == NULL
      || f_cbKey != DRM_AES_KEYSIZE_128 )
    {
        ChkDR( DRM_E_CH_BAD_KEY );
    }

    ChkDR( Oem_Aes_SetKey( f_pbKey, &aesKey ) );
    MEMCPY( rgbBlock, f_pKID->rgb, SIZEOF( DRM_KID ) );
    dr = Oem_Aes_EcbEncryptData(&aesKey, rgbBlock, SIZEOF( DRM_KID ) );
    Oem_Aes_ZeroKey( &aesKey );
    ChkDR( dr );
    MEMCPY( f_rgbChecksum, rgbBlock, DRM_AES_CK_CHECKSUM_LENGTH );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*********************************************************************
**
** Function: _CalcV3LicenseResponseCharCount
**
** Synopsis: Function that calculates the character count of a license
**           acquisition response wrapped in a SOAP packet.
**
** Arguments:
**
** [f_rgdastrXMRLic]        -- An array of DRM ANSI strings that contain
**                             B64 encoded XMR licenses.
** [f_rgdastrXMRLic]        -- Count of XMR licenses in the array
**                             mentioned above.
** [f_pdastrRevInfo]        -- Pointer to a DRM ANSI string that contains
**                             a the revocation info. It is optional.
** [f_pdastrCustomData]     -- Pointer to a DRM ANSI string that contains
**                             a custom data string. It is optional.
** [f_pdastrTransactionID]  -- Pointer to a DRM ANSI string that contains
**                             a transaction ID GUID string. It is optional.
** [f_fUseSlk]              -- Do we need to use a SLK for the response.
**
** Returns:                 A long integer value that is the character
**                          count of license acquisition response wrapped
**                          in a SOAP packet..
**
**********************************************************************/
static DRM_DWORD _CalcV3LicenseResponseCharCount(
    IN DRM_ANSI_CONST_STRING  f_rgdastrXMRLic[],
    IN DRM_DWORD              f_cXMRLic,
    IN DRM_ANSI_CONST_STRING *f_pdastrRevInfo,
    IN DRM_ANSI_CONST_STRING *f_pdastrCustomData,
    IN DRM_ANSI_CONST_STRING *f_pdastrTransactionID,
    IN DRM_BOOL               f_fUseSlk )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i = 0;
    DRM_DWORD cchSignatureXMLCount = 0;
    DRM_DWORD cchCount = 0;

    ChkArg( f_rgdastrXMRLic != NULL && f_cXMRLic > 0 );

    cchCount += g_dastrXMLRootTag.cchString;

    cchCount += DRM_SOAPXML_CalcSOAPHeaderCharCount();

    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrAcquireLicenseResponseTag.cchString,
                                              0,
                                              g_dastrAcquireLicenseResponseAttribName.cchString,
                                              g_dastrAcquireLicenseResponseAttribValue.cchString );

    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrAcquireLicenseResultTag.cchString,
                                              0,
                                              0,
                                              0 );

    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrResponseTag.cchString,
                                              0,
                                              g_dastrResponseAttribName.cchString,
                                              g_dastrResponseAttribValue.cchString );

    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicenseResponseTag.cchString,
                                              0,
                                              g_dastrLicenseResponseAttrib1Name.cchString,
                                              g_dastrLicenseResponseAttrib1Value.cchString );

    cchCount += DRM_XMB_RequiredCharsForTagA( 0,
                                              0,
                                              g_dastrLicenseResponseAttrib2Name.cchString,
                                              g_dastrLicenseResponseAttrib2Value.cchString );

    if( f_fUseSlk )
    {
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrSLKTag.cchString,
                                                  0,
                                                  0,
                                                  0 );

        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrSLKIDTag.cchString,
                                                  0,
                                                  0,
                                                  0 );

        cchCount += SLK_ID_SIZE_IN_CHARS;


        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrSLKDATATag.cchString,
                                                  0,
                                                  0,
                                                  0 );

        cchCount += SLK_DATA_SIZE_IN_CHARS;
    }

    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqVersionTag.cchString,
                                              g_dastrLicAcqVersionValue.cchString,
                                              0,
                                              0 );

    cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicensesTag.cchString,
                                              0,
                                              0,
                                              0 );

    for ( i = 0; i < f_cXMRLic; i++ )
    {
        ChkDRMANSIString( &f_rgdastrXMRLic[ i ] );

        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAckLicenseTag.cchString,
                                                  f_rgdastrXMRLic[ i ].cchString,
                                                  0,
                                                  0 );
    }

    if ( f_pdastrCustomData != NULL )
    {
        ChkDRMANSIString( f_pdastrCustomData );

        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAcqCustomDataTag.cchString,
                                                  f_pdastrCustomData->cchString,
                                                  0,
                                                  0 );
    }

    if ( f_pdastrTransactionID != NULL )
    {
        ChkDRMANSIString( f_pdastrTransactionID );

        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrAcknowledgementTag.cchString,
                                                  0,
                                                  0,
                                                  0 );

        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrLicAckTransactionIDTag.cchString,
                                                  f_pdastrTransactionID->cchString,
                                                  0,
                                                  0 );
    }


    /*
    ** If there is revocation info need to be added to the response
    */
    if( f_pdastrRevInfo != NULL )
    {
        ChkDRMANSIString( f_pdastrRevInfo );
        cchCount += DRM_XMB_RequiredCharsForTagA( g_dastrRevInfoTag.cchString,
                                                  f_pdastrRevInfo->cchString,
                                                  0,
                                                  0 );
    }


    /*
    ** Add the character count of the signature.
    */
    ChkDR( DRM_XMLSIG_GetSignatureNodeCharCount( &cchSignatureXMLCount ) );

    cchCount += cchSignatureXMLCount;

ErrorExit:

    return cchCount;
}

/******************************************************************************
**
** Function:    Test_ReallocHBContext
**
** Synopsis:    Reallocated header context. The caller should make sure that
                ppbHBContext is not NULL.
**
** Arguments:
**              [ppbHBContext]  --  reference to header context
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_ReallocHBContext( DRM_BYTE **ppbHBContext )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD  cbContext    = 0;
    DRM_BYTE  *pbNewContext = NULL;

    ChkDR(DRM_HB_GetContextSize(*ppbHBContext, &cbContext));  /* get current size */
    ChkMem(pbNewContext = (DRM_BYTE*)Oem_MemAlloc(2 * cbContext));  /* alloc double of the current size */
    ZEROMEM( pbNewContext, 2 * cbContext );
    ChkDR(DRM_HB_ReallocHeader(*ppbHBContext,    2 * cbContext, pbNewContext));
    Oem_MemFree(*ppbHBContext);
    *ppbHBContext = pbNewContext;

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: Test_BuildV3LicenseResponse
**
** Synopsis: Function that constructs a license acquisition response
**           wrapped in a SOAP packet.
**
** Arguments:
**
** [f_poBBXContext]         -- Pointer to a black box context.
** [f_rgdastrXMRLic]        -- An array of DRM ANSI strings that contain
**                             B64 encoded XMR licenses.
** [f_rgdastrXMRLic]        -- Count of XMR licenses in the array
**                             mentioned above.
** [f_pdastrRevInfo]        -- Pointer to a DRM ANSI string that contains
**                             a the revocation info. It is optional.
** [f_pdastrCustomData]     -- Pointer to a DRM ANSI string that contains
**                             a custom data string. It is optional.
** [f_pdastrTransactionID]  -- Pointer to a DRM ANSI string that contains
**                             a transaction ID GUID string. It is optional.
** [f_pbXMRLicResp]         -- Pointer to a buffer to receive a generated response
**                             byte stream. If it is NULL, the function returns
**                             an error and the size of the required buffer is
**                             stored in *f_pcbXMRLicResp.
** [f_pcbXMRLicResp]        -- Pointer to a variable that contains the size (in bytes
**                             of the response buffer during input and receives the
**                             size (in bytes) of the response buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer to retrieve
**                          the data is not big enough.
**
**********************************************************************/
DRM_RESULT DRM_CALL Test_BuildV3LicenseResponse(
    IN DRM_BB_CONTEXT        *f_poBBXContext,
    IN DRM_ANSI_CONST_STRING  f_rgdastrXMRLic[],
    IN DRM_DWORD              f_cXMRLic,
    IN DRM_ANSI_CONST_STRING *f_pdastrRevInfo,
    IN DRM_ANSI_CONST_STRING *f_pdastrCustomData,
    IN DRM_ANSI_CONST_STRING *f_pdastrTransactionID,
    OUT DRM_BYTE             *f_pbXMRLicResp,
    IN OUT DRM_DWORD         *f_pcbXMRLicResp )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchXMLContext = 0;
    _XMBContextA *poXMLContext = NULL;
    DRM_DWORD cbXMLContext = 0;
    DRM_CHAR *pchXML = NULL;
    DRM_SUBSTRING dasstrXML = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_STRING dastrXML = EMPTY_DRM_STRING;
    DRM_SUBSTRING dasstrSignedNode = EMPTY_DRM_SUBSTRING;
    DRM_DWORD i = 0;
    DRM_BOOL  fUseSlk = FALSE;
    DRM_BOOL *pfUseSlk = &fUseSlk;


    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_rgdastrXMRLic != NULL && f_cXMRLic > 0 );
    ChkArg( f_pcbXMRLicResp != NULL );
    ChkArg( f_pbXMRLicResp == NULL ||
            *f_pcbXMRLicResp > 0 );

    if( DRM_SUCCEEDED( DRM_TST_GetTestVar( RESPONSE_USE_SLK, (DRM_VOID **) &pfUseSlk, NULL ) ) )
    {
        Log("", "picked up the Use Slk global Flag. State is : %s", *pfUseSlk ? "TRUE" : "FALSE" );
    }

    cchXMLContext = _CalcV3LicenseResponseCharCount( f_rgdastrXMRLic,
                                                     f_cXMRLic,
                                                     f_pdastrRevInfo,
                                                     f_pdastrCustomData,
                                                     f_pdastrTransactionID,
                                                    *pfUseSlk );

    /*
    ** Add space used by XML builder context.
    */
    cchXMLContext += g_cbXMLBuilderMinimumA;

    /*
    ** Add one more DRM_DWORD to ensure the size of buffer is
    ** big enough even after re-alignment.
    */
    cchXMLContext += SIZEOF( DRM_DWORD );

    if ( f_pbXMRLicResp == NULL || *f_pcbXMRLicResp < cchXMLContext )
    {
        *f_pcbXMRLicResp = cchXMLContext;

        /*
        ** Not using ChkDR here to avoid TRACE spews
        */
        dr = DRM_E_BUFFERTOOSMALL;
        goto ErrorExit;
    }

    ChkDR( DRM_SOAPXML_PrepareXMLContext( f_pbXMRLicResp,
                                          cchXMLContext,
                                          &poXMLContext,
                                          &cbXMLContext ) );

    ChkDR( DRM_XMB_GetXMLBaseA( poXMLContext, &pchXML ) );

    ChkDR( DRM_SOAPXML_BuildSOAPHeaderXML( poXMLContext, cbXMLContext ) );

    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrAcquireLicenseResponseTag,
                              NULL,
                              &g_dastrAcquireLicenseResponseAttribName,
                              &g_dastrAcquireLicenseResponseAttribValue,
                              wttOpen ) );

    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrAcquireLicenseResultTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrResponseTag,
                              NULL,
                              &g_dastrResponseAttribName,
                              &g_dastrResponseAttribValue,
                              wttOpen ) );

    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrLicenseResponseTag,
                              NULL,
                              &g_dastrLicenseResponseAttrib1Name,
                              &g_dastrLicenseResponseAttrib1Value,
                              wttOpen ) );

    ChkDR( DRM_XMB_AddAttributeA( poXMLContext,
                                  &g_dastrLicenseResponseAttrib2Name,
                                  &g_dastrLicenseResponseAttrib2Value ) );
    if ( *pfUseSlk )
    {
        DRM_ANSI_STRING  dastrSlkID   = EMPTY_DRM_STRING;
        DRM_ANSI_STRING  dastrSlkData = EMPTY_DRM_STRING;

        ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                                 &g_dastrSLKTag,
                                  NULL,
                                  NULL,
                                  NULL,
                                  wttOpen ) );

        MAP_TO_GLOBAL_TEST_VAR( dastrSlkID.pszString , RESPONSE_V3_SLK_ID );

        dastrSlkID.cchString = DRMCRT_strlen( dastrSlkID.pszString );

        ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                                 &g_dastrSLKIDTag,
                                  (DRM_ANSI_CONST_STRING *) &dastrSlkID,
                                  NULL,
                                  NULL,
                                  wttClosed ) );

        MAP_TO_GLOBAL_TEST_VAR( dastrSlkData.pszString, RESPONSE_V3_SLK_ENCRYPTED );

        dastrSlkData.cchString = DRMCRT_strlen( dastrSlkData.pszString );

        ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                                 &g_dastrSLKDATATag,
                                  (DRM_ANSI_CONST_STRING *) &dastrSlkData,
                                  NULL,
                                  NULL,
                                  wttClosed ) );


        ChkDR( DRM_XMB_CloseCurrNodeA( poXMLContext, NULL ) );
    }

    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrLicAcqVersionTag,
                              &g_dastrLicAcqVersionValue,
                              NULL,
                              NULL,
                              wttClosed ) );

    ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                              &g_dastrLicensesTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    for ( i = 0; i < f_cXMRLic; i++ )
    {
        ChkDRMANSIString( &f_rgdastrXMRLic[ i ] );

        ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                                  &g_dastrLicAckLicenseTag,
                                  &f_rgdastrXMRLic[ i ],
                                  NULL,
                                  NULL,
                                  wttClosed ) );
    }

    ChkDR( DRM_XMB_CloseCurrNodeA( poXMLContext, NULL ) );

    /*
    ** If there is revocation data to be added to the response:
    */
    if( f_pdastrRevInfo != NULL )
    {
        ChkDRMANSIString(f_pdastrRevInfo);
        ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                                  &g_dastrRevInfoTag,
                                  f_pdastrRevInfo,
                                  NULL,
                                  NULL,
                                  wttClosed ) );
    }

    if ( f_pdastrCustomData != NULL )
    {
        ChkDRMANSIString( f_pdastrCustomData );

        ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                                  &g_dastrLicAcqCustomDataTag,
                                  f_pdastrCustomData,
                                  NULL,
                                  NULL,
                                  wttClosed ) );
    }

    if ( f_pdastrTransactionID != NULL )
    {
        ChkDRMANSIString( f_pdastrTransactionID );

        ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                                  &g_dastrAcknowledgementTag,
                                  NULL,
                                  NULL,
                                  NULL,
                                  wttOpen ) );

        ChkDR( DRM_XMB_WriteTagA( poXMLContext,
                                  &g_dastrLicAckTransactionIDTag,
                                  f_pdastrTransactionID,
                                  NULL,
                                  NULL,
                                  wttClosed ) );

        ChkDR( DRM_XMB_CloseCurrNodeA( poXMLContext, NULL ) );
    }

    ChkDR( DRM_XMB_CloseCurrNodeA( poXMLContext, &dasstrSignedNode ) );

    ChkDR( DRM_XMB_CloseDocumentA( poXMLContext, &dasstrXML ) );

    dastrXML.pszString = pchXML;

    dastrXML.cchString = dasstrXML.m_cch;

    ChkDR( DRM_SOAPXML_PrependXMLRoot( &dastrXML ) );

    ChkBOOL( dastrXML.cchString <= *f_pcbXMRLicResp,
             DRM_E_BUFFERTOOSMALL );

    MEMCPY( f_pbXMRLicResp, dastrXML.pszString, dastrXML.cchString );

    /*
    ** Return the actual size (number of bytes) of the generated response.
    */
    *f_pcbXMRLicResp = dastrXML.cchString;

ErrorExit:

    return dr;
}

#if defined(_PREFAST_)
#pragma prefast(push)
#pragma prefast(disable:6262) /* Temporarily disable stack limit check. */
#endif

/*****************************************************************************
** Function:    Test_CreateXMRLicense
**
** Synopsis:
**
** Arguments:   [f_ppbLicense]    -- Buffer to receive the created license.
**              [f_pcbLicense]    -- Pointer to a DWORD to reeive the size of the License.
**              [f_fLog]          -- If True the value of each member of argv is written to the log.
**              [argc]            -- number of entries in argv
**              [argv]            -- array of string to set the policy. specific members are detailed below.
**
**              argv[0] : KID (may be RANDOM or PREGENERATEDRANDOM)
**              argv[1] : LID (may be RANDOM)
**              argv[2] : key seed
**              argv[3] : optional. priority
**              argv[4] : optional. uplink KID
**              argv[5] : optional. play count (NULL to have no play count but still play right, OMIT to omit the right altogether)
**              argv[6] : optional. metering ID
**              argv[7] : optional. expire after first use (number of seconds)
**              argv[8] : optional. expire on store (number of seconds)
**              argv[9] : optional. absolute expiry end date (number of seconds from now)
**              argv[10]: optional. grace period
**              argv[11]: optional. generation number or OMIT to omit the object, default is generation 1
**              argv[12]: optional. security level (default is 150)
**              argv[13]: optional. copy count (NULL to have no copy count but still copy right, OMIT to omit the right altogether)
**              argv[14]: optional. absolute expiry begin date (number of seconds from now)
**              argv[15]: optional. specifies the XMR_SYMMETRIC_ENCRYPTION_TYPE for the content key object (AES_128_CTR, AES_128_ECB or COCKTAIL, defaults to AES_128_CTR).
**              argv[16]: optional. explicitly defines the embedding behavior (IGNORE, COPY, MOVE)
**              argv[17]: optional. B64 AccountID for embedding. The most recent domain cert stored for that domain will be used to create the license.  If no domain cert is available, use a random domain key and set the revision to the current time
**              argv[18]: optional. analog video OPL
**              argv[19]: optional. compressed digital audio OPL
**              argv[20]: optional. uncompressed digital audio OPL
**              argv[21]: optional. compressed digital video OPL
**              argv[22]: optional. uncompressed digital video OPL
**              argv[23]: optional. Source ID
**              argv[24]: optional. B64 Domain key value.  When making domain-bound licenses, this key will override that of the actual domain cert.  This will also overwrite the domain revision with the current time if argv[25] is not used.
**              argv[25]: optional. Domain revision.  When making domain-bound licenses, this will override the revision of the actual domain cert.
**              argv[26]: optional. IssueDate (number of seconds from now, +/-). This defaults to now. use OMIT to leave it out.
**              argv[27]: optional. Restricted Source ID is set if this is not NULL
**              argv[28]: optional. B64 Serial Number
**              argv[29]: optional. B64 Extended Analog Video Protection GUID
**              argv[30]: optional. Extended alalog video protection configuration.
**              argv[31]: optional. XMR Version. Integer {1,2,3}. Defaults to 3.
**              argv[32]: optional. CANNOT_PERSIST Restriction. Is set to TRUE if not NULL
**              argv[33]: optional. CANNOT_BIND Restriction. Is set to TRUE if not NULL
**              argv[34]: optional. Boolean indicating that global extended right should be used.
**              argv[35]: optional. Boolean indicating that global extended restriction should be used.
**              argv[36]: optional. If not NULL, this indicates that Playlist Burn right is enabled.
**              argv[37]: optional. RIV version
**              argv[38]: optional. Test custom data
**
** Returns:     DRM_SUCCESS                  - on success
**
** Note:
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_CreateXMRLicense( __deref_out_ecount( *f_pcbLicense ) DRM_BYTE **f_ppbLicense, __out DRM_DWORD *f_pcbLicense, __in DRM_BOOL f_fLog, long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT                                  dr = DRM_SUCCESS;
    DRM_BYTE                                    rgbKey[ DRM_AES_KEYSIZE_128 * 2 ] = { 0 };
    DRM_DWORD                                   dwVersion = XMR_VERSION_3;
    DRM_BYTE                                    rgbXMRBuilder[1536];
    DRM_DWORD                                   cbXMRBuilder = SIZEOF( rgbXMRBuilder );
    DRM_ID                                     *pLID = NULL;
    DRM_ID                                      drmidKID;
    DRM_BOOL                                    fSymmOpt       = FALSE;
    DRM_BOOL                                   *pfSymmOpt      = &fSymmOpt;
    DRM_BOOL                                    fUseSlk        = TRUE;
    DRM_BOOL                                   *pfSharedUseSlk = &fUseSlk;
    DRM_AES_KEY                                 oAesSymmOptKey = { 0 };
    DRM_WORD                                    wOptKeyEncryptionCipherType = XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_INVALID;
    DRM_XMR_OPTIMIZED_CONTENT_KEY               optContentKey;
    DRM_XMR_CONTENT_KEY                         contentKey;
    DRM_XMR_ECC_DEVICE_KEY                      deviceKey;
    DRM_XMR_DEVICE_KEY                          rsaDeviceKey;
    DRM_XMR_GENERATION_NUMBER                   xmrGenerationNumber;
    DRM_XMR_BUILDER_CONTEXT                     xmrBuilder;
    DRM_XMR_MINIMUM_ENVIRONMENT                 xmrMinEnv;
    DRM_XMR_METERING                            xmrMID;
    DRM_XMR_PRIORITY                            xmrPriority;
    DRM_XMR_ISSUEDATE                           xmrIssueDate;
    DRM_XMR_EXPIRATION_AFTER_FIRSTUSE           xmrExpirationAfterFirstUse;
    DRM_XMR_EXPIRATION_AFTER_FIRSTSTORE         xmrExpirationOnStore;
    DRM_XMR_EXPIRATION                          xmrExpiration;
    DRM_XMR_GRACE_PERIOD                        xmrGracePeriod;
    DRM_XMR_EMBEDDING_BEHAVIOR                  xmrEmbeddingBehavior;
    DRM_XMR_RIGHTS                              xmrGlobalRights = { 0 };
    DRM_XMR_PLAYBACK_RIGHTS                     xmrPlay = {0};
    DRM_XMR_COPY_RIGHTS                         xmrCopy = {0};
    DRM_XMR_PLAYLIST_BURN_RIGHTS                xmrPLB = {0};
    DRM_XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS    xmrOPLs = {0};
    DRM_XMR_SOURCEID                            xmrSourceID = {0};
    DRM_XMR_RESTRICTED_SOURCEID                 xmrRestrictedSourceID = {0};
    DRM_XMR_REVOCATION_INFORMATION_VERSION      xmrRIV = {0};
    DRM_XMR_SERIAL_NUMBER                       xmrSerialNum = {0};
    DRM_CONST_STRING                            dstrSerialNum = EMPTY_DRM_STRING;
    DRM_XMR_VIDEO_OUTPUT_CONFIGURATION_LIST     xmrVideoOutConfigList = {0};
    DRM_CONST_STRING                            dstrExtendedOPLGUID = EMPTY_DRM_STRING;
    DRM_GUID                                    oExtendedOPLGUID = { 0 };
    DRM_APP_CONTEXT_INTERNAL                   *pAppContext = NULL;
    DRM_BYTE                                    rgbCryptoBuffer[max( SIZEOF( DRM_CRYPTO_CONTEXT ), SIZEOF( DRM_SHA_CONTEXT ) )] = {0};
    DRM_BYTE                                    rgbEncKey[max( ECC_P256_CIPHERTEXT_SIZE_IN_BYTES + DRM_AES_KEYSIZE_128, DRM_AES_KEYSIZE_128*2 )];
    DRM_DWORD                                   cbEncKey = 0;
    DRM_BYTE                                    rgbOptEncKey[ DRM_AES_KEYSIZE_128*2 ] = {0};
    DRM_DWORD                                   cbOptEncKey = 0;
    DRM_BOOL                                    fKIDAlloc = FALSE;
    DRM_CONST_STRING                            dstrKID = EMPTY_DRM_STRING;
    DRM_CONST_STRING                            dstrLID = EMPTY_DRM_STRING;
    DRM_CONST_STRING                            dstrSeed = EMPTY_DRM_STRING;
    DRM_CONST_STRING                            dstrAccountID = EMPTY_DRM_STRING;
    DRM_CONST_STRING                            dstrDomainKey = EMPTY_DRM_STRING;
    DRM_CONST_STRING                            dstrContentKey = EMPTY_DRM_STRING;
    enum XMR_ASYMMETRIC_ENCRYPTION_TYPE         eKeyEncryptionCipherType;
    enum XMR_SYMMETRIC_ENCRYPTION_TYPE          eSymmetricEncryptionType = XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR;
    DRM_XMR_UPLINK_KID                          xmrUplink = {0};
    DRM_BYTE                                    rgbChainedChecksum[DRM_AES_CK_CHECKSUM_LENGTH] = {0};
    DRM_ID                                      drmidUplinkKID = {0};
    DRM_CONST_STRING                            dstrUplinkKID = EMPTY_DRM_STRING;
    DRM_ID                                      drmidMID;
    DRM_CHAR                                   *pstrRandomKeyID = NULL;
    DRM_CHAR                                   *pstrLicenseID;
    DRM_BYTE                                   *pbDomainCert = NULL;
    DRM_DWORD                                   cbDomainCert = 0;
    DRM_GUID                                    oAccountID = { 0 };
    DRM_BOOL                                    fDomainBound = FALSE;
    DRM_BYTE                                   *prgbAesSharedKey = NULL;
    DRM_BYTE                                   *prgbCocktailSharedKey = NULL;
    DRM_DWORD                                   cbBuffer      = 0;
    DRM_BCERT_CHAIN_HEADER                      ChainHeader      = { 0 };
    DRM_BCERT_CERTIFICATE                       oDomainCert      = { 0 };
    DRM_GUID                                    oServiceID       = { 0 };
    DRM_DWORD                                   bCurrOffset      = 0;
    DRM_XMR_DOMAIN_ID                           oXmrDomainID     = { 0 };
    DRM_DWORD                                   cbSize           = SIZEOF( DRM_GUID );
    PUBKEY_P256                                 oDomainPubKey    = { 0 };
    PRIVKEY_P256                                oDomainPrivKey   = { 0 };
    DRM_DWORD                                   dwDomainRevision = 0;
    DRMFILETIME                                 ft               = { 0 };
    DRM_LONG                                    iArg             = 0;
    DRM_RSA_PUBLIC_KEY                          oRsaPubKey       = { 0 };
    DRM_WORD                                    wObjectType      = 0;
    DRM_BYTE                                   *pbScratchKeySeed = NULL;
    DRM_TEST_XPOL_DATA                         *poXRightData            = NULL;
    DRM_TEST_XPOL_DATA                         *poXRestrictionData      = NULL;
    DRM_BOOL                                    fUseXpol                = FALSE;
    DRM_BYTE                                   *pExtendedRestriction    = NULL;
    DRM_BCERT_PUBKEY                            oTmpPubKey       = { 0 };


    ChkArg( f_ppbLicense != NULL );
    ChkArg( f_pcbLicense != NULL );
    ChkArg( argc >= 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    oDomainCert.KeyInfo.rgoKeys[0].pValue = &oTmpPubKey;

    GET_SHARED_APP_CONTEXT( pAppContext );

    MAP_TO_GLOBAL_TEST_VAR( prgbAesSharedKey, AES_KEY_VAR );
    MAP_TO_GLOBAL_TEST_VAR( prgbCocktailSharedKey, RC4_KEY_VAR );
    MAP_TO_GLOBAL_TEST_VAR( poXRightData, RESPONSE_XPOL_RIGHT_VAR );
    MAP_TO_GLOBAL_TEST_VAR( poXRestrictionData, RESPONSE_XPOL_RESTRICTION_VAR );

    if( f_fLog )
    {
        Log( "Trace", "\t_CreateXMRLicense Args (%d)", argc );
        for( iArg = 0; iArg < argc; iArg++ )
        {
            Log( "Trace", "\t\tArg[%d]: %s", iArg, argv[iArg] );
        }
    }

    /*
    ** Check if a version was specified.
    */
    if ( argc > 31 && argv[31] != NULL  )
    {
        DRM_DWORD dwInternalVerion = strtoul( argv[31], NULL, 0 );
        switch( dwInternalVerion )
        {
        case 1:
            dwVersion = XMR_VERSION_1;
            break;
        case 2:
            dwVersion = XMR_VERSION_2;
            break;
        case 3:
        default:
            dwVersion = XMR_VERSION_3;
            break;
        }
    }
    /*
    ** Pick up key encryption type if it is specified
    */
    if ( argc > 15 && argv[15] != NULL )
    {
        if ( strncmp( argv[15], "AES_128_ECB", 11 ) == 0 )
        {
            eSymmetricEncryptionType = XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB;
        }
        else if ( strncmp( argv[15], "COCKTAIL", 8 ) == 0 )
        {
            eSymmetricEncryptionType = XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL;
        }
        else
        {
            ChkArg( strncmp( argv[15], "AES_128_CTR", 11 ) == 0 );
        }
    }

    /*
    ** Check if we should symmetrically optimize the Key.
    */
    if ( DRM_FAILED( DRM_TST_GetTestVar( RESPONSE_SYMM_OPT, ( DRM_VOID ** ) &pfSymmOpt, NULL ) ) )
    {
        *pfSymmOpt = FALSE;
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pfSharedUseSlk, RESPONSE_USE_SLK );
    }

    /*
    ** Get The Symmetric Encryption Key if needed
    */
    if ( *pfSymmOpt )
    {
        DRM_BYTE  *pbKeyData  = NULL;
        DRM_DWORD *pcbKeyData = NULL;

        MAP_TO_GLOBAL_TEST_VAR_EX( pbKeyData, pcbKeyData, RESPONSE_V3_SLK );

        ChkBOOL( *pcbKeyData == DRM_AES_KEYSIZE_128, DRM_E_TEST_UNEXPECTED_OUTPUT );

        ChkDR( Oem_Aes_SetKey( pbKeyData, &oAesSymmOptKey ) );
        wOptKeyEncryptionCipherType = XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB_SLK;
    }

    /*
    ** Get shared random KID if it exists
    */
    if ( DRM_FAILED( DRM_TST_GetTestVar( RANDOM_KID_VAR, ( DRM_VOID ** ) &pstrRandomKeyID, NULL ) ) )
    {
        pstrRandomKeyID = NULL;
    }

    /*
    ** Pick up KID
    */
    if( strncmp( argv[0], "PREGENERATEDRANDOM", 18 ) == 0 && pstrRandomKeyID != NULL)
    {
        ChkDR( MakeDRMString( &dstrKID, pstrRandomKeyID ) );
    }
    else if( strncmp( argv[0], "RANDOM", 6 ) == 0 )
    {
        if ( DRM_FAILED( DRM_TST_GetTestVar( CONTENT_ID_VAR, ( DRM_VOID ** )&dstrKID.pwszString, NULL ) ) )
        {
            ChkMem( dstrKID.pwszString = ( DRM_WCHAR * )Oem_MemAlloc( ( CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + 1 ) * SIZEOF( DRM_WCHAR ) ) );
            fKIDAlloc = TRUE;
        }
        ZEROMEM( ( DRM_BYTE * )dstrKID.pwszString, ( CCH_BASE64_EQUIV(SIZEOF(DRM_KID)) + 1 ) * SIZEOF( DRM_WCHAR ) );
        dstrKID.cchString = CCH_BASE64_EQUIV(SIZEOF(DRM_KID)) + 1;
        DRM_KG_GenerateKeyID( NULL, (DRM_WCHAR*)dstrKID.pwszString, &dstrKID.cchString );
    }
    else
    {
        ChkDR( MakeDRMString( &dstrKID, argv[0] ) );
    }
    ChkDR( DRM_UTL_DecodeKID( &dstrKID, &drmidKID ) );


    /*
    ** Pick up LID
    */
    if( strncmp( argv[1], "RANDOM", 6 ) == 0 )
    {
        MAP_TO_GLOBAL_TEST_VAR( pLID, LICENSE_ID_VAR );
        ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE*)pLID, SIZEOF( DRM_GUID ) ) );
    }
    else if( strncmp( argv[1], "NONCE", 6 ) == 0 )
    {
        DRM_FAILED( DRM_TST_GetTestVar( LICENSE_NONCE_VAR, ( DRM_VOID ** ) &pstrLicenseID, NULL ));
        ChkDR( MakeDRMString( &dstrLID, pstrLicenseID ) );
        MAP_TO_GLOBAL_TEST_VAR( pLID, LICENSE_ID_VAR );
        ChkDR( DRM_UTL_DecodeKID( &dstrLID, (DRM_KID*)pLID ) );
    }
    else
    {
        MAP_TO_GLOBAL_TEST_VAR( pLID, LICENSE_ID_VAR );
        ChkDR( MakeDRMString( &dstrLID, argv[1] ) );
        if( dstrLID.cchString == DRM_GUID_STRING_LEN )
        {
            ChkDR( DRM_UTL_StringToGuid( &dstrLID, (DRM_GUID*)pLID ) );
        }
        else
        {
            ChkDR( DRM_UTL_DecodeKID( &dstrLID, (DRM_KID*)pLID ) );
        }
    }

    /*
    ** Pick up seed
    */
    ChkDR( MakeDRMString( &dstrSeed, argv[2] ) );

    /*
    ** Allocate scratch buffer used in the content key generation functions.
    */
#pragma prefast(push)
#pragma prefast(disable:22022) /* ignore arithmetic overflow warning, already handled */
    ChkOverflow( dstrSeed.cchString * SIZEOF(DRM_WCHAR), dstrSeed.cchString );
    ChkMem( pbScratchKeySeed = (DRM_BYTE *)Oem_MemAlloc( dstrSeed.cchString * SIZEOF(DRM_WCHAR) ) );
    ZEROMEM( pbScratchKeySeed, dstrSeed.cchString * SIZEOF(DRM_WCHAR) );
#pragma prefast(pop)


    /*
    ** Generate content key
    */
    if ( eSymmetricEncryptionType == XMR_SYMMETRIC_ENCRYPTION_TYPE_COCKTAIL )
    {
        dstrContentKey.cchString = CCH_BASE64_EQUIV( DRM_COCKTAIL_PADDED_KEYLEN ) + 1;
        ChkMem( dstrContentKey.pwszString = (DRM_WCHAR*)Oem_MemAlloc( dstrContentKey.cchString * SIZEOF( DRM_WCHAR ) ) );
        ZEROMEM( ( DRM_BYTE * )dstrContentKey.pwszString, dstrContentKey.cchString * SIZEOF( DRM_WCHAR ) );
        ( ( DRM_WCHAR * )dstrContentKey.pwszString )[CCH_BASE64_EQUIV( DRM_COCKTAIL_PADDED_KEYLEN )] = ONE_WCHAR( '\0', '\0' );  /* NULL Terminate the string, need to cast away the const. */

        ChkDR( DRM_KG_GenerateContentKeyB64( dstrSeed.pwszString,
                                            dstrSeed.cchString,
                                            dstrKID.pwszString,
                                            dstrKID.cchString,
                                            eDRM_RC4_CIPHER,
                                            DRMCIPHERKEYLEN_RC4,
                                            ( DRM_WCHAR * )dstrContentKey.pwszString,
                                            &dstrContentKey.cchString,
                                            pbScratchKeySeed,
                                            dstrSeed.cchString * SIZEOF(DRM_WCHAR)) );

        /* Modify Content key to adhere to Cocktail specs */
        cbBuffer = DRM_COCKTAIL_PADDED_KEYLEN;
        ChkDR( DRM_B64_DecodeW( &dstrContentKey, &cbBuffer, &rgbKey[DRM_COCKTAIL_PADDED_KEYLEN], 0 ) );
        ChkBOOL( cbBuffer == DRMCIPHERKEYLEN_RC4, DRM_E_TEST_UNEXPECTED_OUTPUT );

        /* Save the key for use in Encryption */
        MEMCPY( prgbCocktailSharedKey, &rgbKey[DRM_COCKTAIL_PADDED_KEYLEN], DRMCIPHERKEYLEN_RC4 );

        /* Fill random bytes after Cocktail Key */
        Oem_Random_GetBytes( NULL, &rgbKey[DRM_COCKTAIL_PADDED_KEYLEN + DRMCIPHERKEYLEN_RC4], DRM_COCKTAIL_PADDED_KEYLEN - DRMCIPHERKEYLEN_RC4);
    }
    else
    {
        dstrContentKey.cchString = CCH_BASE64_EQUIV( DRM_AES_KEYSIZE_128 ) + 1;
        ChkMem( dstrContentKey.pwszString = (DRM_WCHAR*)Oem_MemAlloc( dstrContentKey.cchString * SIZEOF( DRM_WCHAR ) ) );
        ZEROMEM( ( DRM_BYTE * )dstrContentKey.pwszString, dstrContentKey.cchString * SIZEOF( DRM_WCHAR ) );
        ( ( DRM_WCHAR * )dstrContentKey.pwszString )[CCH_BASE64_EQUIV( DRM_AES_KEYSIZE_128 )] = ONE_WCHAR( '\0', '\0' );  /* NULL Terminate the string, need to cast away the const. */

        ChkDR( DRM_KG_GenerateContentKeyB64( dstrSeed.pwszString,
                                            dstrSeed.cchString,
                                            dstrKID.pwszString,
                                            dstrKID.cchString,
                                            eDRM_AES_COUNTER_CIPHER,
                                            DRM_AES_KEYSIZE_128,
                                            ( DRM_WCHAR * )dstrContentKey.pwszString,
                                            &dstrContentKey.cchString,
                                            pbScratchKeySeed,
                                            dstrSeed.cchString * SIZEOF(DRM_WCHAR)) );

        cbBuffer = SIZEOF( rgbKey ) - DRM_AES_KEYSIZE_128;
        ChkDR( DRM_B64_DecodeW( &dstrContentKey, &cbBuffer, &rgbKey[DRM_AES_KEYSIZE_128], 0 ) );
        ChkArg( cbBuffer == DRM_AES_KEYSIZE_128 );
    }

    /*
    ** Save the key for use in Encryption.
    */
    MEMCPY( prgbAesSharedKey, &rgbKey[DRM_AES_KEYSIZE_128], DRM_AES_KEYSIZE_128 );

    ChkDR( OEM_ECC_GenerateHMACKey_P256( (PLAINTEXT_P256 *)rgbKey, (struct bigctx_t *)rgbCryptoBuffer ) );

    ChkDR( DRM_XMR_StartLicense( rgbXMRBuilder,
                                 cbXMRBuilder,
                                 pLID,
                                 dwVersion,
                                &xmrBuilder ) );

    ChkArg( pAppContext->oBlackBoxContext.fInited );

    if( argc > 4 && argv[4] != NULL && strcmp( argv[4], "ROOT" ) == 0 )
    {
        eSymmetricEncryptionType = XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB;
    }

    if( argc > 4
     && argv[4] != NULL
     && strcmp( argv[4], "ROOT" ) != 0 )
    {
        /*
        ** We were given an uplink KID, so this is a leaf license
        */
        DRM_AES_KEY oAesUplinkKey = {0};

        ChkDR( MakeDRMString( &dstrUplinkKID, argv[4] ) );
        ChkDR( DRM_UTL_DecodeKID( &dstrUplinkKID, &drmidUplinkKID ) );

        dstrContentKey.cchString = CCH_BASE64_EQUIV( DRM_AES_KEYSIZE_128 ) + 1;
        ChkDR( DRM_KG_GenerateContentKeyB64( dstrSeed.pwszString,
                                          dstrSeed.cchString,
                                          dstrUplinkKID.pwszString,
                                          dstrUplinkKID.cchString,
                                          eDRM_AES_COUNTER_CIPHER,
                                          DRM_AES_KEYSIZE_128,
                              (DRM_WCHAR*)dstrContentKey.pwszString,
                                         &dstrContentKey.cchString,
                                          pbScratchKeySeed,
                                          dstrSeed.cchString * SIZEOF(DRM_WCHAR)) );

        cbBuffer = SIZEOF( rgbEncKey );
        ChkDR( DRM_B64_DecodeW( &dstrContentKey, &cbBuffer, rgbEncKey, 0 ) );
        ChkArg( cbBuffer == DRM_AES_KEYSIZE_128 );

        /*
        ** Compute the checksum
        */
        ChkDR( _CreateChecksum(&drmidUplinkKID,
                                rgbEncKey,
                                DRM_AES_KEYSIZE_128,
                                rgbChainedChecksum ) );

        /*
        ** Encrypt content key with uplink key.
        */
        ChkDR( Oem_Aes_SetKey( rgbEncKey, &oAesUplinkKey ) );
        MEMCPY( rgbEncKey, rgbKey, DRM_AES_KEYSIZE_128 * 2 );

        ChkDR( Oem_Aes_EcbEncryptData( &oAesUplinkKey,
                        rgbEncKey,
                        DRM_AES_KEYSIZE_128 * 2 ) );

        cbEncKey = DRM_AES_KEYSIZE_128 * 2;

        /*
        ** V3+ licenses use Uplink KID 2 object, not Uplink KID object
        */
        wObjectType = ( ( dwVersion < XMR_VERSION_3 ) ? XMR_OBJECT_TYPE_UPLINK_KID_OBJECT : XMR_OBJECT_TYPE_UPLINK_KID_2_OBJECT );
        ChkDR( DRM_XMR_InitializeObject( wObjectType, &xmrUplink ) );
        xmrUplink.pbguidUplinkKID = drmidUplinkKID.rgb;
        xmrUplink.iguidUplinkKID = 0;
        xmrUplink.pbChainedCheckSumBuffer = rgbChainedChecksum;
        xmrUplink.cbChainedCheckSum = SIZEOF( rgbChainedChecksum );
        xmrUplink.iChainedCheckSum = 0;
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, wObjectType, &xmrUplink ) );

        eKeyEncryptionCipherType = XMR_ASYMMETRIC_ENCRYPTION_TYPE_CHAINED_LICENSE;
        fUseSlk = FALSE;
    }
    else if ( argc > 17 && argv[17] != NULL ) /* This is Domain Bound */
    {
        DRM_DWORD                cbDomainKeyRecord = SIZEOF( DRM_BB_DOMAIN_KEY_RECORD );
        DRM_BB_DOMAIN_KEY_RECORD oDomainKeyRecord  = {0};
        DRM_BOOL                 fHavePrivKey      = FALSE;

        /*
        ** Get the account ID
        */
        cbSize           = SIZEOF( DRM_GUID );
        ChkDR( MakeDRMString( &dstrAccountID, argv[17] ) );
        ChkDR( DRM_B64_DecodeW( &dstrAccountID, &cbSize, (DRM_BYTE*)&oAccountID, 0 ) );


        dr = DRM_DOMST_GetCert( &pAppContext->oDomainStoreContext,
                                &oAccountID,
                                &oServiceID,
                                NULL,
                                &cbDomainCert );
        if ( dr == DRM_E_BUFFERTOOSMALL )
        {
            /*
            ** Use the existing domain cert
            */
            ChkMem( pbDomainCert = ( DRM_BYTE * ) Oem_MemAlloc( cbDomainCert ) );
            ZEROMEM( pbDomainCert, cbDomainCert );

            ChkDR( DRM_DOMST_GetCert( &pAppContext->oDomainStoreContext,
                                      &oAccountID,
                                      &oServiceID,
                                      pbDomainCert,
                                      &cbDomainCert ) );

            /*
            ** Skip the chain header to get to device certificate
            */
            ChkDR( DRM_BCert_GetChainHeader( pbDomainCert,
                                             cbDomainCert,
                                             &bCurrOffset,
                                             &ChainHeader ) );

            /*
            ** Load domain certificate. The domain certificate is the first in the chain.
            */
            ChkDR( DRM_BCert_GetCertificate( pbDomainCert,
                                             cbDomainCert,
                                             &bCurrOffset,
                                             &oDomainCert,
                                             DRM_BCERT_CERTTYPE_DOMAIN ) );

            MEMCPY( (DRM_BYTE*)&oDomainPubKey, oDomainCert.KeyInfo.rgoKeys[0].pValue->m_rgbPubkey, SIZEOF( PUBKEY_P256 ) );
            dwDomainRevision = oDomainCert.Type.DomainInfo.dwRevision;
        }
        else if ( dr == DRM_E_DOMAIN_STORE_GET_DATA )
        {
            /*
            ** No such domain cert is available.  Use a random key
            */
            ChkDR( OEM_ECC_GenKeyPair_P256( &oDomainPubKey, &oDomainPrivKey, (struct bigctx_t *)rgbCryptoBuffer ) );

            /*
            ** Set revision as the current time
            */
            Oem_Clock_GetSystemTimeAsFileTime( NULL, &ft );
            FILE_TIME_TO_DWORD( ft, dwDomainRevision );
            fHavePrivKey = TRUE;
        }
        else
        {
            /*
            ** Unexpected error code was returned by DRM_DOMST_GetCert
            */
            Log( "", "DRM_DOMST_GetCert unexpectedly returned %#X\n", dr );
            ChkDR( DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
        /*
        ** Check for specified domain key
        */
        if ( argc > 24 && argv[24] != NULL )
        {
            cbSize = SIZEOF( PUBKEY_P256 );
            ChkDR( MakeDRMString( &dstrDomainKey, argv[24] ) );
            ChkDR( DRM_B64_DecodeW( &dstrDomainKey, &cbSize, (DRM_BYTE*)&oDomainPubKey, 0 ) );
            /*
            ** Set revision as the current time
            */
            Oem_Clock_GetSystemTimeAsFileTime( NULL, &ft );
            FILE_TIME_TO_DWORD( ft, dwDomainRevision );

        }

        /*
        ** Check for specified revision
        */
        if ( argc > 25 && argv[25] != NULL )
        {
            ChkDR( DRMCRT_AtoDWORD( argv[25], (DRM_DWORD)DRMCRT_strlen( argv[25] ), 10, &dwDomainRevision ) );
        }

        /*
        ** Try to get the private key for the revision. It's okay to not get it.
        */
        if( !fHavePrivKey )
        {
            dr = DRM_DOMST_GetKey( &pAppContext->oDomainStoreContext,
                                   &oAccountID,
                                   dwDomainRevision,
                                   (DRM_BYTE*)&oDomainKeyRecord,
                                   &cbDomainKeyRecord );
            if( DRM_SUCCEEDED( dr ) )
            {
                ChkBOOL( oDomainKeyRecord.dwVersion == DOMAIN_KEY_RECORD_VERSION, DRM_E_DOMAIN_STORE_INVALID_KEY_RECORD );
                ChkBOOL( cbDomainKeyRecord == SIZEOF( DRM_BB_DOMAIN_KEY_RECORD ), DRM_E_DOMAIN_STORE_INVALID_KEY_RECORD );

                /*
                ** Domain Key is encrypted to the machine.
                */
                ChkDR( DRM_BBX_SymmetricBind( &pAppContext->oBlackBoxContext,
                                              FALSE,
                                              FALSE,
                                              oDomainKeyRecord.oDomainPrivKeyEncrypted.m_rgbPrivkey,
                                              NULL,
                                              SIZEOF( PRIVKEY_P256 ) ) );

                DRM_BYT_CopyBytes( oDomainPrivKey.m_rgbPrivkey,
                                   0,
                                   oDomainKeyRecord.oDomainPrivKeyEncrypted.m_rgbPrivkey,
                                   0,
                                   SIZEOF( PRIVKEY_P256 ) );

                fHavePrivKey = TRUE;
            }
        }

        ChkDR( OEM_ECC_Encrypt_P256( ( PUBKEY_P256 * )&oDomainPubKey,
                                     (PLAINTEXT_P256 *)rgbKey,
                                     (CIPHERTEXT_P256 *)rgbEncKey,
                                     (struct bigctx_t *)rgbCryptoBuffer ) );

        cbEncKey = SIZEOF( CIPHERTEXT_P256 );

        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT, &deviceKey ) );

        deviceKey.fValid          = TRUE;
        deviceKey.wEccCurveType   = XMR_ECC_CURVE_TYPE_P256;
        deviceKey.pbKeyData       = oDomainCert.KeyInfo.rgoKeys[0].pValue->m_rgbPubkey;
        deviceKey.cbKeyData       = SIZEOF( PUBKEY_P256 );
        deviceKey.iKeyData        = 0;

        if ( dstrDomainKey.pwszString == NULL )
        {
            ChkBOOL( DRM_B64_EncodeW( deviceKey.pbKeyData, deviceKey.cbKeyData, ( DRM_WCHAR* )dstrDomainKey.pwszString, &dstrDomainKey.cchString, 0 ), DRM_E_BUFFERTOOSMALL );
            ChkOverflow( dstrDomainKey.cchString * SIZEOF( DRM_WCHAR ), dstrDomainKey.cchString );
            ChkOverflow( dstrDomainKey.cchString * SIZEOF( DRM_WCHAR ) + SIZEOF( DRM_WCHAR ),  dstrDomainKey.cchString );
            ChkMem( dstrDomainKey.pwszString = (DRM_WCHAR*)Oem_MemAlloc( dstrDomainKey.cchString * SIZEOF( DRM_WCHAR ) + SIZEOF( DRM_WCHAR ) ) );
            ZEROMEM( (DRM_BYTE*)( dstrDomainKey.pwszString ), dstrDomainKey.cchString * SIZEOF( DRM_WCHAR ) + SIZEOF( DRM_WCHAR ) );
            ChkDR( DRM_B64_EncodeW( deviceKey.pbKeyData, deviceKey.cbKeyData, ( DRM_WCHAR* )dstrDomainKey.pwszString, &dstrDomainKey.cchString, 0 ) );
        }
        if( f_fLog )
        {
            Log( "", "\t_CreateXMRLicense: Using domain account %S\n", dstrAccountID.pwszString );
            Log( "", "\t_CreateXMRLicense: Using domain key %S\n", dstrDomainKey.pwszString );
            Log( "", "\t_CreateXMRLicense: Using domain revision %d", dr );
        }

        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT,      &deviceKey ) );

        eKeyEncryptionCipherType = XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256;

        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_DOMAIN_ID_OBJECT, &oXmrDomainID ) );

        /* Set domain account ID and revision. Service ID is not embedded into license */
        oXmrDomainID.pbAccountID = (DRM_BYTE *)&oAccountID;
        oXmrDomainID.ibAccountID = 0;
        oXmrDomainID.cbAccountID = SIZEOF( DRM_GUID );
        oXmrDomainID.dwRevision  = dwDomainRevision;

        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_DOMAIN_ID_OBJECT, &oXmrDomainID ) );

        /*
        ** If we have the domain priv key, then create the Aes Symmetric Key
        */
        if ( fHavePrivKey )
        {
            DRM_AES_KEY oAesTempKey                           = {0};
            DRM_BYTE    rgbConstantBlock[DRM_AES_KEYSIZE_128] = {0};
            DRM_BYTE    rgbSymmOptKey[DRM_AES_KEYSIZE_128]    = {0};

            ChkDR( Oem_Aes_SetKey(  oDomainPrivKey.m_rgbPrivkey,
                                    &oAesTempKey ) );

            ChkDR( Oem_Omac1_Sign( &oAesTempKey,
                                    rgbConstantBlock,
                                    0,
                                    SIZEOF(rgbConstantBlock),
                                    rgbSymmOptKey ) );

            ChkDR( Oem_Aes_SetKey(  rgbSymmOptKey,
                                   &oAesSymmOptKey ) );

            wOptKeyEncryptionCipherType = XMR_SYMMETRIC_KEY_ENCRYPTION_TYPE_AES_128_ECB;
            fUseSlk = FALSE;
        }

        fDomainBound = TRUE;
    }
    else if( dwVersion == XMR_VERSION_1 )
    {
        /*
        ** If it's XMR V1 we need to try to add an RSA Key
        */
        ChkDR( OEM_RSA_SetPublicKey(  eDRM_RSA_1024,
                                      SIZEOF( rgbDRM_RSA_DEFAULT_PUBEXP ),
                                      rgbDRM_RSA_DEFAULT_PUBEXP,
                                      SIZEOF( g_rgbRSAModulus1024 ),
                                      g_rgbRSAModulus1024,
                                     &oRsaPubKey ) );

        cbEncKey = SIZEOF( rgbEncKey );
        ChkDR( OEM_RSA_OaepEncrypt( NULL,
                                   &oRsaPubKey,
                                    DRM_AES_KEYSIZE_128,
                                    prgbAesSharedKey,
                                    &cbEncKey,
                                    rgbEncKey,
                                    (DRM_CRYPTO_CONTEXT *)rgbCryptoBuffer ) );

        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT, &rsaDeviceKey ) );

        rsaDeviceKey.pbModulusBuffer = (DRM_BYTE *)g_rgbRSAModulus1024;
        rsaDeviceKey.iModulus        = 0;
        rsaDeviceKey.cbModulus       = SIZEOF( g_rgbRSAModulus1024 );
        rsaDeviceKey.fValid          = TRUE;

        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT,      &rsaDeviceKey ) );

        eKeyEncryptionCipherType = XMR_ASYMMETRIC_ENCRYPTION_TYPE_RSA_1024;
    }
    else
    {
        ChkDR( OEM_ECC_Encrypt_P256( &pAppContext->oBlackBoxContext.cachedBCertValues.DeviceKeyEncrypt.PublicKey,
                                        (PLAINTEXT_P256 *)rgbKey,
                                        (CIPHERTEXT_P256 *)rgbEncKey,
                                        (struct bigctx_t *)rgbCryptoBuffer ) );

        cbEncKey = SIZEOF( CIPHERTEXT_P256 );

        eKeyEncryptionCipherType = XMR_ASYMMETRIC_ENCRYPTION_TYPE_ECC_256;

        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT, &deviceKey ) );

        deviceKey.fValid          = TRUE;
        deviceKey.wEccCurveType   = XMR_ECC_CURVE_TYPE_P256;
        deviceKey.pbKeyData       = (DRM_BYTE*)&pAppContext->oBlackBoxContext.cachedBCertValues.DeviceKeyEncrypt.PublicKey;
        deviceKey.cbKeyData       = SIZEOF( PUBKEY_P256 );
        deviceKey.iKeyData        = 0;

        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_ECC_DEVICE_KEY_OBJECT,      &deviceKey ) );

        fUseSlk = TRUE;
    }

    /*
    ** Set up and add the encrypted content key to the license
    */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT,    &contentKey ) );

    contentKey.pbguidKeyID              = drmidKID.rgb;
    contentKey.iguidKeyID               = 0;
    contentKey.wSymmetricCipherType     = (DRM_WORD)eSymmetricEncryptionType;
    contentKey.wKeyEncryptionCipherType = (DRM_WORD)eKeyEncryptionCipherType;
    contentKey.pbEncryptedKeyBuffer     = rgbEncKey;
    contentKey.cbEncryptedKey           = (DRM_WORD)cbEncKey;
    contentKey.iEncryptedKey            = 0;

    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT,         &contentKey ) );

    /*
    ** Add the Optimized Content Key Object if Needed (Ignores Leaves)
    */
    if ( *pfSymmOpt && eKeyEncryptionCipherType != XMR_ASYMMETRIC_ENCRYPTION_TYPE_CHAINED_LICENSE )
    {

        MEMCPY( rgbOptEncKey, rgbKey, DRM_AES_KEYSIZE_128 * 2 );
        ChkDR( Oem_Aes_EcbEncryptData( &oAesSymmOptKey,
                                       rgbOptEncKey,
                                       DRM_AES_KEYSIZE_128 * 2 ) );
        cbOptEncKey = DRM_AES_KEYSIZE_128 * 2;

        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_OPTIMIZED_CONTENT_KEY_OBJECT,    &optContentKey ) );

        optContentKey.wKeyEncryptionCipherType = wOptKeyEncryptionCipherType;
        optContentKey.pbEncryptedKeyBuffer     = rgbOptEncKey;
        optContentKey.cbEncryptedKey           = (DRM_WORD)cbOptEncKey;
        optContentKey.iEncryptedKey            = 0;

        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_OPTIMIZED_CONTENT_KEY_OBJECT, &optContentKey ) );

        if( fUseSlk )
        {
            *pfSharedUseSlk = TRUE;
        }
    }

    /*
    ** Add priority
    */
    if ( argc > 3
      && argv[3] != NULL )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_PRIORITY_OBJECT, &xmrPriority ) );
        xmrPriority.dwValue = strtoul( argv[3], NULL, 10 );
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_PRIORITY_OBJECT, &xmrPriority ) );
    }

    /*
    ** Set the security level
    ** V3+ licenses use security level object, not min env object
    */
    wObjectType = ( ( dwVersion < XMR_VERSION_3 ) ? XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT : XMR_OBJECT_TYPE_SECURITY_LEVEL );
    ChkDR( DRM_XMR_InitializeObject( wObjectType, &xmrMinEnv ) );
    xmrMinEnv.fValid                = TRUE;
    if ( argc > 12
      && argv[12] != NULL )
    {
        xmrMinEnv.wMinimumSecurityLevel = (DRM_WORD)strtoul( argv[12], NULL, 10 );
    }
    else
    {
        xmrMinEnv.wMinimumSecurityLevel = (DRM_WORD)150;
    }
    ChkDR( DRM_XMR_AddObject( &xmrBuilder, wObjectType, &xmrMinEnv ) );

    /*
    ** Playback policy
    */
    if ( argc <= 5
      || argv[5] == NULL
      || strcmp( argv[5], "OMIT" ) != 0 )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER, &xmrPlay ) );
        xmrPlay.fValid            = TRUE;

        /*
        ** Set playcount
        */
        if ( argc > 5
          && argv[5] != NULL )
        {
            xmrPlay.PlayCount.fValid  = TRUE;
            xmrPlay.PlayCount.dwValue = strtoul( argv[5], NULL, 10 );
        }

        /*
        ** Set Explicit Analog Video Output Protection
        */
        if ( argc > 29
          && argv[29] != NULL )
        {
            ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_EXPLICIT_ANALOG_VIDEO_OUTPUT_PROTECTION_CONTAINER, &xmrPlay.containerExplicitAnalogVideoProtection ) );
            xmrVideoOutConfigList.Config.fValid = TRUE;

            cbSize           = SIZEOF( DRM_GUID );

            /*
            ** Read extended analog video guid
            */
            ChkDR( MakeDRMString( &dstrExtendedOPLGUID, argv[29] ) );
            ChkDR( DRM_B64_DecodeW( &dstrExtendedOPLGUID, &cbSize, (DRM_BYTE*)&oExtendedOPLGUID, 0 ) );
            xmrVideoOutConfigList.Config.pguidBuffer = (DRM_BYTE*)&oExtendedOPLGUID;
            xmrVideoOutConfigList.Config.iGuid = 0;

            /*
            ** Read extended analog video config buffer
            */
            if ( argc > 30
              && argv[30] != NULL )
            {
                DRM_DWORD dwConfigData = 0;

                ChkArg( IsDigitalOnlyString( argv[30] ) );

                dwConfigData = (DRM_DWORD) strtoul( argv[30], NULL, 10 );

                xmrVideoOutConfigList.Config.cbConfigData  = SIZEOF(DRM_DWORD);
                ChkMem( xmrVideoOutConfigList.Config.pbConfigDataBuffer = (DRM_BYTE*)Oem_MemAlloc( xmrVideoOutConfigList.Config.cbConfigData ) );
                ZEROMEM( xmrVideoOutConfigList.Config.pbConfigDataBuffer, xmrVideoOutConfigList.Config.cbConfigData );
                MEMCPY( xmrVideoOutConfigList.Config.pbConfigDataBuffer, (DRM_BYTE*)&dwConfigData, xmrVideoOutConfigList.Config.cbConfigData );
            }
            xmrVideoOutConfigList.pNext = NULL;
            xmrPlay.containerExplicitAnalogVideoProtection.plistOutputConfiguration = &xmrVideoOutConfigList;
            xmrPlay.containerExplicitAnalogVideoProtection.cOutputProtectionIDs = 1;
        }

        ChkDR( DRM_XMR_AddObject( &xmrBuilder,
            XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER,
            &xmrPlay ) );
    }

    /*
    ** Copy policy
    */
    if ( argc <= 13
      || argv[13] == NULL
      || strcmp( argv[13], "OMIT" ) != 0 )
    {
        /* V3+ licenses use Copy Policy 2, not Copy Policy 1 */
        wObjectType = ( ( dwVersion < XMR_VERSION_3 ) ? XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER : XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER );
        ChkDR( DRM_XMR_InitializeObject( wObjectType, &xmrCopy ) );
        xmrCopy.fValid            = TRUE;
        if ( argc > 13
          && argv[13] != NULL )
        {
            xmrCopy.CopyCount.fValid  = TRUE;
            xmrCopy.CopyCount.dwValue = strtoul( argv[13], NULL, 10 );
        }

        ChkDR( DRM_XMR_AddObject( &xmrBuilder,
            wObjectType,
            &xmrCopy ) );

        /* XMR requires Enablers under Copy Container 2 */
        if( wObjectType == XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER )
        {
            /* Current test cases only care about Copy To Device, so just use that */
            ChkDR( DRM_XMR_AddUnknownObject(
                &xmrBuilder,
                XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER,
                TRUE,
                XMR_OBJECT_TYPE_COPY_POLICY_2_CONTAINER,
                XMR_FLAGS_CONTAINER,
                NULL,
                0 ) );

            ChkDR( DRM_XMR_AddUnknownObject(
                &xmrBuilder,
                XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT,
                TRUE,
                XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER,
                0,
                (DRM_BYTE*)&DRM_ACTION_COPY_TO_DEVICE,
                sizeof(DRM_GUID) ) );
        }
    }

    /*
    ** Insert Generation Number object
    ** User cannot control the value through SDK
    */
    if ( argc > 11
      && argv[11] != NULL
      && strcmp( argv[11], "OMIT" ) != 0 )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_GENERATION_NUMBER_OBJECT, &xmrGenerationNumber ) );
        if ( argc > 11
          && argv[11] != NULL )
        {
            xmrGenerationNumber.dwValue = strtoul( argv[11], NULL, 10 );
        }
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_GENERATION_NUMBER_OBJECT, &xmrGenerationNumber ) );
    }

    /*
    ** Add metering ID
    */
    if ( argc > 6
      && argv[6] != NULL )
    {
        DRM_CONST_STRING dstrMID = EMPTY_DRM_STRING;

        ChkDR( MakeDRMString( &dstrMID, argv[6] ) );
        ChkDR( DRM_UTL_DecodeKID( &dstrMID, &drmidMID ) );
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_METERING_OBJECT, &xmrMID ) );
        xmrMID.pguidBuffer = drmidMID.rgb;
        xmrMID.iGuid = 0;
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_METERING_OBJECT, &xmrMID ) );
    }

    /*
    * Set ExpirationAfterFirstUse (if not unlimited)
    */
    if ( argc > 7
      && argv[7] != NULL )
    {
        /* V3+ licenses use Expire After First Play, not Expire After First Use */
        wObjectType = ( ( dwVersion < XMR_VERSION_3 ) ? XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTUSE_OBJECT : XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTPLAY_OBJECT );
        ChkDR( DRM_XMR_InitializeObject( wObjectType, &xmrExpirationAfterFirstUse ) );
        xmrExpirationAfterFirstUse.dwValue = strtoul( argv[7], NULL, 10 );
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, wObjectType, &xmrExpirationAfterFirstUse ) );
    }

    /*
    ** Set ExpirationOnStore (if not unlimited)
    */
    if ( argc > 8
      && argv[8] != NULL )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTSTORE_OBJECT, &xmrExpirationOnStore ) );
        xmrExpirationOnStore.dwValue = strtoul( argv[8], NULL, 10 );
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_EXPIRATION_AFTER_FIRSTSTORE_OBJECT, &xmrExpirationOnStore ) );
    }

    /*
    ** Set Absolute expiry (if not unlimited)
    */
    if (   ( argc > 9
          && argv[9] != NULL )
        || ( argc > 14
          && argv[14] != NULL ) )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_EXPIRATION_OBJECT, &xmrExpiration ) );

        Oem_Clock_GetSystemTimeAsFileTime( NULL, &ft );

        if( argc > 9
         && argv[9] != NULL )
        {
            FILE_TIME_TO_DWORD( ft, xmrExpiration.dwEndDate );
            xmrExpiration.dwEndDate += strtol( argv[9], NULL, 10 );
        }
        else
        {
            xmrExpiration.dwEndDate = XMR_UNLIMITED;
        }

        if( argc > 14
         && argv[14] != NULL )
        {
            FILE_TIME_TO_DWORD( ft, xmrExpiration.dwBeginDate );
            xmrExpiration.dwBeginDate += strtol( argv[14], NULL, 10 );
        }
        else
        {
            xmrExpiration.dwBeginDate = 0;
        }

        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_EXPIRATION_OBJECT, &xmrExpiration ) );
    }

    /*
    ** Set grace period
    */
    if ( argc > 10
      && argv[10] != NULL )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_GRACE_PERIOD_OBJECT, &xmrGracePeriod ) );
        xmrGracePeriod.dwValue = strtoul( argv[10], NULL, 10 );
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_GRACE_PERIOD_OBJECT, &xmrGracePeriod ) );
    }


    /*
    ** Set any OPLs
    */

    if ( argc > 18 )
    {

        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT, &xmrOPLs ) );
        xmrOPLs.fValid            = FALSE;

        if( argv[18] != NULL )
        {
            xmrOPLs.wAnalogVideo = strtoul( argv[18], NULL, 10 );
            xmrOPLs.fValid            = TRUE;
        }

        if ( argc > 19 && argv[19] != NULL )
        {
            xmrOPLs.wCompressedDigitalAudio = strtoul( argv[19], NULL, 10 );
            xmrOPLs.fValid            = TRUE;
        }

        if ( argc > 20 && argv[20] != NULL )
        {
            xmrOPLs.wUncompressedDigitalAudio = strtoul( argv[20], NULL, 10 );
            xmrOPLs.fValid            = TRUE;
        }

        if ( argc > 21 && argv[21] != NULL )
        {
            xmrOPLs.wCompressedDigitalVideo = strtoul( argv[21], NULL, 10 );
            xmrOPLs.fValid            = TRUE;
        }

        if ( argc > 22 && argv[22] != NULL )
        {
            xmrOPLs.wUncompressedDigitalVideo = strtoul( argv[22], NULL, 10 );
            xmrOPLs.fValid            = TRUE;
        }

        if( xmrOPLs.fValid )
        {
            ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT, &xmrOPLs ) );
        }

    }


    /*
    ** Set SourceID
    */
    if ( argc > 23
      && argv[23] != NULL )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SOURCEID_OBJECT, &xmrSourceID ) );
        xmrSourceID.dwValue = strtoul( argv[23], NULL, 10 );
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_SOURCEID_OBJECT, &xmrSourceID ) );
    }

    /*
    ** Set Restricted Source ID
    */
    if ( argc > 27
      && argv[27] != NULL )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT, &xmrRestrictedSourceID ) );
        xmrRestrictedSourceID.fValid = TRUE;
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_RESTRICTED_SOURCEID_OBJECT, &xmrRestrictedSourceID ) );
    }

    /*
    ** Set Serial Number
    */
    if ( argc > 28
      && argv[28] != NULL )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT, &xmrSerialNum ) );
        ChkDR( MakeDRMString( &dstrSerialNum, argv[28] ) );
        ChkBOOL( DRM_B64_DecodeW( &dstrSerialNum, &xmrSerialNum.cbData, xmrSerialNum.pbDataBuffer, 0 ), DRM_E_BUFFERTOOSMALL );
        ChkMem( xmrSerialNum.pbDataBuffer = (DRM_BYTE*)Oem_MemAlloc( xmrSerialNum.cbData ) );
        ZEROMEM( xmrSerialNum.pbDataBuffer, xmrSerialNum.cbData );
        ChkDR( DRM_B64_DecodeW( &dstrSerialNum, &xmrSerialNum.cbData, xmrSerialNum.pbDataBuffer, 0 ) );
        xmrSerialNum.iData = 0;
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT, &xmrSerialNum ) );
    }

    /*
    ** Add Embedding Behavior
    ** Add a Default behavior if its XMR_VERSION_3
    */
    if( dwVersion == XMR_VERSION_3
     || ( argc > 16
       && argv[16] != NULL ) )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT, &xmrEmbeddingBehavior ) );
        if ( argc > 16
          && argv[16] != NULL )
        {
            if ( strcmp( argv[16], "IGNORE" ) == 0 )
            {
                xmrEmbeddingBehavior.wValue = XMR_EMBEDDING_BEHAVIOR_IGNORE;
            }
            else if ( strcmp( argv[16], "COPY" ) == 0 )
            {
                xmrEmbeddingBehavior.wValue = XMR_EMBEDDING_BEHAVIOR_COPY;
            }
            else if ( strcmp( argv[16], "MOVE" ) == 0 )
            {
                xmrEmbeddingBehavior.wValue = XMR_EMBEDDING_BEHAVIOR_MOVE;
            }
        }
        else
        {
            if ( eKeyEncryptionCipherType == XMR_ASYMMETRIC_ENCRYPTION_TYPE_CHAINED_LICENSE
              || fDomainBound )
            {
                xmrEmbeddingBehavior.wValue = XMR_EMBEDDING_BEHAVIOR_MOVE;
            }
            else
            {
                xmrEmbeddingBehavior.wValue = XMR_EMBEDDING_BEHAVIOR_IGNORE;
            }
        }
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_EMBEDDING_BEHAVIOR_OBJECT, &xmrEmbeddingBehavior ) );
    }

    /*
    ** Set IssueDate
    */
    if (  argc     <= 26
       || argv[26] == NULL
       || strcmp( argv[26], "OMIT" ) != 0 )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_ISSUEDATE_OBJECT, &xmrIssueDate ) );

        Oem_Clock_GetSystemTimeAsFileTime( NULL, &ft );

        FILE_TIME_TO_DWORD( ft, xmrIssueDate.dwValue );

        if( argc > 26
         && argv[26] != NULL )
        {
            xmrIssueDate.dwValue += strtol( argv[26], NULL, 10 );
        }

        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_ISSUEDATE_OBJECT, &xmrIssueDate ) );
    }

    /*
    ** Add the Rights Object.
    */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SETTINGS_OBJECT, &xmrGlobalRights ) );
    xmrGlobalRights.wValue = 0; /* Initially nothing is set. */

    /*
    ** Set Cannot Persist.
    */
    if ( argc     >  32
      && argv[32] != NULL )
    {
        xmrGlobalRights.wValue |= XMR_RIGHTS_CANNOT_PERSIST;
    }

    /*
    ** Set Cannot Bind.
    */
    if ( argc     >  33
      && argv[33] != NULL )
    {
        xmrGlobalRights.wValue |= XMR_RIGHTS_CANNOT_BIND_LICENSE;
    }

    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_SETTINGS_OBJECT, &xmrGlobalRights ) );

    if ( argc > 36 && argv[36] != NULL )
    {
        /*
        ** Add Playlist Burn object
        */
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER, &xmrPLB ) );

        xmrPLB.fValid = TRUE;
        ChkDR( DRM_XMR_AddObject( &xmrBuilder,
                                   XMR_OBJECT_TYPE_ALLOW_PLAYLISTBURN_POLICY_CONTAINER,
                                  &xmrPLB ) );
    }

    if ( argc > 34 && argv[34] != NULL )
    {
        ChkDR( StringToBool( argv[34], &fUseXpol ) );
        if ( fUseXpol )
        {
            Log( "", "\t_CreateXMRLicense: Using extended right %d", poXRightData->wObjectType );
            ChkDR( DRM_XMR_AddUnknownObject( &xmrBuilder,
                                             poXRightData->wObjectType,
                                             poXRightData->fDuplicateAllowed,
                                             poXRightData->wParent,
                                             poXRightData->wFlags,
                                             NULL,
                                             poXRightData->cbAllData ) );
        }
    }

    if ( argc > 35 && argv[35] != NULL )
    {
        ChkDR( StringToBool( argv[35], &fUseXpol ) );
        if ( fUseXpol )
        {
            Log( "", "\t_CreateXMRLicense: Using extended restriction %d", poXRestrictionData->wObjectType );
            ChkDR( DRM_XMR_CreateExtensibleRestriction( &xmrBuilder,
                                                        poXRestrictionData->pbXPolData,
                                                        poXRestrictionData->cbXPolData,
                                                        poXRestrictionData->cStateDateCountorBinary,
                                                        poXRestrictionData->fSecureStateType,
                                                        poXRestrictionData->pStateDateCountorBinary,
                                                        poXRestrictionData->cStateDateCountorBinary,
                                                        &pExtendedRestriction,
                                                        &(poXRestrictionData->cbAllData ) ) );
            ChkDR( DRM_XMR_AddUnknownObject( &xmrBuilder,
                                             poXRestrictionData->wObjectType,
                                             poXRestrictionData->fDuplicateAllowed,
                                             poXRestrictionData->wParent,
                                             poXRestrictionData->wFlags,
                                             pExtendedRestriction,
                                             poXRestrictionData->cbAllData ) );
        }
    }

    /*
    ** For V3 licenses, include any required objects.
    */
    if( dwVersion == XMR_VERSION_3 )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT, &xmrRIV ) );
        xmrRIV.fValid  = TRUE;
        /*
        ** Set RIV Version.
        */
        if( argc > 37 && argv[37] != NULL )
        {
            xmrRIV.dwValue = strtoul( argv[37], NULL, 0 );
        }
        else
        {
            xmrRIV.dwValue = 0;
        }

        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT, &xmrRIV ) );
    }

    *f_pcbLicense = 0;
    dr = DRM_XMR_FinishLicense( &xmrBuilder,
                                rgbKey,
                                _SignXmrLicense,
                                NULL,
                                f_pcbLicense );

    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
    }

    ChkMem( *f_ppbLicense = (DRM_BYTE*)Oem_MemAlloc( CCH_BASE64_EQUIV(*f_pcbLicense) + 1 ) );
    ZEROMEM( *f_ppbLicense, CCH_BASE64_EQUIV(*f_pcbLicense) + 1 );
    ChkDR( DRM_XMR_FinishLicense( &xmrBuilder,
                                  rgbKey,
                                  _SignXmrLicense,
                                  *f_ppbLicense,
                                  f_pcbLicense ) );

//  LogBlob( "_CreateXMRLicense", "XMR License:", *f_ppbLicense, *f_pcbLicense );

ErrorExit:
    FREEDRMSTRING( dstrContentKey );
    if ( fKIDAlloc )
    {
        FREEDRMSTRING( dstrKID );
    }
    FREEDRMSTRING( dstrLID );
    FREEDRMSTRING( dstrAccountID );
    FREEDRMSTRING( dstrSerialNum );
    FREEDRMSTRING( dstrExtendedOPLGUID );
    SAFE_OEM_FREE( xmrVideoOutConfigList.Config.pbConfigDataBuffer );
    FREEDRMSTRING( dstrDomainKey );
    FREEDRMSTRING( dstrSeed );
    SAFE_OEM_FREE( pbDomainCert );
    SAFE_OEM_FREE( pbScratchKeySeed );

    return dr;
}

#if defined(_PREFAST_)
#pragma prefast(pop)
#endif


/******************************************************************************
**
** Function :   _Test_InitLicGenXMR
**
** Synopsis :   Initialize some global variables used by the license generation code.
**
** Arguments :  N/A
**
** Returns :    DRM_SUCCESS if signing was successful
**
******************************************************************************/
DRM_RESULT DRM_CALL Test_InitLicGenXMR()
{
    DRM_RESULT                dr                 = DRM_SUCCESS;
    DRM_BYTE                 *prgbRC4Key         = NULL;
    DRM_BYTE                 *prgbAESKey         = NULL;
    DRM_BYTE                  rgbDefaultRC4Key[DRMCIPHERKEYLEN_RC4] = { 0x0D, 0x17, 0x52, 0x8C, 0x80, 0x19, 0x56 };
    DRM_BYTE                  rgbDefaultAESKey[DRM_AES_KEYSIZE_128] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    DRM_TEST_XPOL_DATA       *poXRightData       = NULL;
    DRM_TEST_XPOL_DATA       *poXRestrictionData = NULL;
    DRM_BYTE                 *pdrmidKID          = NULL;
    DRM_BYTE                 *pbNonce            = NULL;
    DRM_BYTE                 *pdrmidLID          = NULL;

    if ( DRM_FAILED( DRM_TST_GetTestVar( AES_KEY_VAR, ( DRM_VOID ** ) &prgbAESKey, NULL ) ) )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( prgbAESKey, AES_KEY_VAR, DRM_AES_KEYSIZE_128 * SIZEOF( DRM_BYTE ) );
        MEMCPY( prgbAESKey, rgbDefaultAESKey, DRM_AES_KEYSIZE_128 * SIZEOF( DRM_BYTE ) );
    }

    if ( DRM_FAILED( DRM_TST_GetTestVar( RC4_KEY_VAR, ( DRM_VOID ** ) &prgbRC4Key, NULL ) ) )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( prgbRC4Key, RC4_KEY_VAR, DRMCIPHERKEYLEN_RC4 * SIZEOF( DRM_BYTE ) );
        MEMCPY( prgbRC4Key, rgbDefaultRC4Key, DRMCIPHERKEYLEN_RC4 * SIZEOF( DRM_BYTE ) );
    }

    if ( DRM_FAILED( DRM_TST_GetTestVar( CONTENT_ID_VAR, ( DRM_VOID ** ) &pdrmidKID, NULL ) ) )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pdrmidKID, CONTENT_ID_VAR, ( CCH_BASE64_EQUIV(SIZEOF(DRM_KID)) + 1 ) * SIZEOF( DRM_WCHAR ) );
        ZEROMEM( pdrmidKID, ( CCH_BASE64_EQUIV(SIZEOF(DRM_KID)) + 1 ) * SIZEOF( DRM_WCHAR ) );
    }

    if ( DRM_FAILED( DRM_TST_GetTestVar( LICENSE_NONCE_VAR, ( DRM_VOID ** ) &pbNonce, NULL ) ) )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pbNonce, LICENSE_NONCE_VAR, CCH_BASE64_EQUIV( SIZEOF( DRM_KID) ) + 1 );
        ZEROMEM( pbNonce, CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) + 1 );
    }

    if ( DRM_FAILED( DRM_TST_GetTestVar( LICENSE_ID_VAR, ( DRM_VOID ** ) &pdrmidLID, NULL ) ) )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pdrmidLID, LICENSE_ID_VAR, SIZEOF( DRM_GUID ) + 1 );
        ZEROMEM( pdrmidLID, SIZEOF( DRM_GUID ) + 1 );
    }

    if ( DRM_FAILED( DRM_TST_GetTestVar( RESPONSE_XPOL_RIGHT_VAR, ( DRM_VOID ** ) &poXRightData, NULL ) ) )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( poXRightData, RESPONSE_XPOL_RIGHT_VAR, SIZEOF( DRM_TEST_XPOL_DATA ) );
        ZEROMEM( poXRightData, SIZEOF( DRM_TEST_XPOL_DATA ) );
    }

    if ( DRM_FAILED( DRM_TST_GetTestVar( RESPONSE_XPOL_RESTRICTION_VAR, ( DRM_VOID ** ) &poXRestrictionData, NULL ) ) )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( poXRestrictionData, RESPONSE_XPOL_RESTRICTION_VAR, SIZEOF( DRM_TEST_XPOL_DATA ) );
        ZEROMEM( poXRestrictionData, SIZEOF( DRM_TEST_XPOL_DATA ) );
    }

    if ( DRM_FAILED( DRM_TST_GetTestVar( RANDOM_KID_VAR, ( DRM_VOID ** ) &pdrmidKID, NULL ) ) )
    {
        MAP_TO_GLOBAL_TEST_VAR_Alloc( pdrmidKID, RANDOM_KID_VAR, CCH_BASE64_EQUIV(SIZEOF(DRM_KID)) + 1 );
        ZEROMEM( pdrmidKID, CCH_BASE64_EQUIV(SIZEOF(DRM_KID)) + 1  );
    }

ErrorExit:
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE

