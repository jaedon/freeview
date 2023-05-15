/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMXMLSIGBUILDER_C
#include <drmutilitieslite.h>
#include <drmxmlparser.h>
#include <drmxmlsigconstants.h>
#include <drmxmlsig.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

/*********************************************************************
**
** Function: _CalcSHA256Hash
**
** Synopsis: Function that generates a SHA-256 hash of the passed in data.
**
** Arguments:
**
** [f_pdastrData]           -- Pointer to a DRM ANSI string for which a
**                             SHA-256 hash is generated.
** [f_rgbHash]              -- Pointer to a buffer that receives a generated
**                             SHA-256 hash.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _CalcSHA256Hash(
    __in                                const DRM_ANSI_CONST_STRING *f_pdastrData,
    __out_bcount(SHA256_DIGEST_SIZE_IN_BYTES) DRM_BYTE               f_rgbHash[ __CB_DECL( SHA256_DIGEST_SIZE_IN_BYTES ) ] )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_SHA_CONTEXT oSHAContext = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_XMLSIG, PERF_FUNC__CalcSHA256Hash );

    ChkDRMANSIString( f_pdastrData );
    ChkArg( f_rgbHash != NULL );

    /* Generate a SHA-1 hash of the passed in data. */
    ChkDR( DRM_SHA_Init( &oSHAContext, eDRM_SHA_256 ) );

    ChkDR( DRM_SHA_Update( PB_DASTR( f_pdastrData ),
                           CB_DASTR( f_pdastrData ),
                           &oSHAContext ) );

    ChkDR( DRM_SHA_Finalize( &oSHAContext,
                             SHA256_DIGEST_SIZE_IN_BYTES,
                             f_rgbHash ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _BuildSignedInfoNode
**
** Synopsis: Function that constructs <SignedInfo> node.
**           This information will be used to calculate a signature value.
**
** Arguments:
**
** [f_poXMLContextA]        -- Pointer to a XML builder context, ANSI version.
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_pdastrData]           -- Pointer to a ANSI string with data
**                             to calculate SHA-256 digest.
** [f_dasstrSignedInfo]     -- Pointer to a substring with <SignedInfo> tag
**                             which is used in signature value calculation.
**                             Substring operates on XMLBuilder's buffer
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
*********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildSignedInfoNode(
    __inout       _XMBContextA          *f_poXMLContextA,
    __in    const DRM_BB_CONTEXT        *f_poBBXContext,
    __in    const DRM_ANSI_CONST_STRING *f_pdastrData,
    __out         DRM_SUBSTRING         *f_dasstrSignedInfo )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE rgbHash[ __CB_DECL( SHA256_DIGEST_SIZE_IN_BYTES ) ] = { 0 };
    DRM_CHAR rgchScratch[ CCH_BASE64_EQUIV( SHA256_DIGEST_SIZE_IN_BYTES ) ] = { 0 };
    DRM_DWORD cchScratch = 0;
    DRM_ANSI_STRING dastrScratch = {0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_XMLSIG, PERF_FUNC__BuildSignedInfoNode );

    ChkArg( f_poXMLContextA != NULL );
    ChkArg( f_poBBXContext != NULL );
    ChkDRMANSIString( f_pdastrData );

    ChkDR( _CalcSHA256Hash( f_pdastrData, rgbHash ) );

    DRMCASSERT( SIZEOF( rgchScratch ) >=
                CCH_BASE64_EQUIV( SHA256_DIGEST_SIZE_IN_BYTES ) * SIZEOF ( DRM_CHAR ) );

    /*
    ** Add and open <SignedInfo> node.
    **
    ** Output:
    **      <SignedInfo>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigSignedInfoTag,
                              NULL,
                              &g_dastrSigSignedInfoAttribName,
                              &g_dastrSigSignedInfoAttribValue,
                              wttOpen ) );

    /*
    ** Add and close <CanonicalizationMethod> node.
    **
    ** Output:
    **      <SignedInfo>
    **          <CanonicalizationMethod></CanonicalizationMethod>
    **
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigCanonicalizationMethodTag,
                              NULL,
                              &g_dastrSigCanonicalizationMethodAttribName,
                              &g_dastrSigCanonicalizationMethodAttribValue,
                              wttOpen ) );
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

    /*
    ** Add and close <SignatureMethod> node.
    **
    ** Output:
    **      <SignedInfo>
    **      ...
    **          <SignatureMethod></SignatureMethod>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigSignatureMethodTag,
                              NULL,
                              &g_dastrSigSignatureMethodAttribName,
                              &g_dastrSigSignatureMethodAttribValue,
                              wttOpen ) );

    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

    /*
    ** Add and open <Reference> node.
    **
    ** Output:
    **      <SignedInfo>
    **      ...
    **          <Reference URI="#SignedData">
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigReferenceTag,
                              NULL,
                              &g_dastrSigReferenceAttribName,
                              &g_dastrSigReferenceAttribValue,
                              wttOpen ) );

    /*
    ** Add and close <DigestMethod> node.
    **
    ** Output:
    **      <SignedInfo>
    **      ...
    **          <Reference>
    **              <DigestMethod>...</DigestMethod>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigDigestMethodTag,
                              NULL,
                              &g_dastrSigDigestMethodAttribName,
                              &g_dastrSigDigestMethodAttribValue,
                              wttOpen ) );

    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

    /*
    ** Add and open <DigestValue> node.
    **
    ** Output:
    **      <SignedInfo>
    **      ...
    **          <Reference>
    **          ...
    **              <DigestValue>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigDigestValueTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    cchScratch = CCH_BASE64_EQUIV( SHA256_DIGEST_SIZE_IN_BYTES );

    ChkDR( DRM_B64_EncodeA( rgbHash,
                            SHA256_DIGEST_SIZE_IN_BYTES,
                            rgchScratch,
                            &cchScratch,
                            0 ) );

    dastrScratch.pszString = rgchScratch;
    dastrScratch.cchString = cchScratch;

    ChkDR( DRM_XMB_AddDataA( f_poXMLContextA,
                            ( DRM_ANSI_CONST_STRING* )&dastrScratch ) );

    /*
    ** Close <DigestValue> node.
    **
    ** Output:
    **      <SignedInfo>
    **      ...
    **          <Reference>
    ***             ...
    **              <DigestValue></DigestValue>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

    /*
    ** Close <Reference> node.
    **
    ** Output:
    **      <SignedInfo>
    **          ...
    **          <Reference>
    **          ...
    **          </Reference>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

    /*
    ** Close <SignedInfo> node.
    **
    ** Output:
    **      <SignedInfo>
    **      ...
    **      </SignedInfo>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, f_dasstrSignedInfo ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _BuildSignatureValueNode
**
** Synopsis: Function that constructs <SignatureValue> node
**
** Arguments:
**
** [f_poXMLContextA]        -- Pointer to ANSI XML builder context.
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_dastrSignedInfo]      -- Pointer to ANSI string with data being signed.
** [f_poECC256PubKey]       -- Pointer to an ECC-256 public key that is used
**                             to receive the extracted ECC-256 public key.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note:                    ECC-256 key is always in explicit format.
**
*********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildSignatureValueNode(
    __inout       _XMBContextA          *f_poXMLContextA,
    __inout       DRM_BB_CONTEXT        *f_poBBXContext,
    __in    const DRM_ANSI_CONST_STRING *f_dastrSignedInfo,
    __out         PUBKEY_P256           *f_poECC256PubKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE rgbECC256Signature[ __CB_DECL( ECDSA_P256_SIGNATURE_SIZE_IN_BYTES ) ] = { 0 };
    /*
    ** The size of the scratch buffer is the B64-ed size of an ECC-256 signature.
    ** Since the scratch buffer is also used to process ECC-256 public key,
    ** it assumes the size of an ECC-256 signature is enough to accomodate the
    ** size of all other types of data being processed.
    */
    DRM_CHAR rgchScratch[ CCH_BASE64_EQUIV( ECDSA_P256_SIGNATURE_SIZE_IN_BYTES ) ];
    DRM_DWORD cchScratch = 0;
    DRM_ANSI_STRING dastrScratch = {0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_XMLSIG, PERF_FUNC__BuildSignatureValueNode );

    /*
    ** Enforce the assumtpion rule on the size of the scratch buffer.
    */
    DRMCASSERT( SIZEOF( rgchScratch ) >=
                CCH_BASE64_EQUIV( SIZEOF( PUBKEY_P256 ) ) * SIZEOF( DRM_CHAR ) );

    ChkArg( f_poXMLContextA != NULL );
    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_poECC256PubKey != NULL );
    ChkDRMANSIString( f_dastrSignedInfo );

    /*
    ** Point the scratch string to the scratch buffer.
    ** Although the size of the scratch string can change, its starting location is
    ** always the same.
    */
    dastrScratch.pszString = rgchScratch;

    ChkDR( DRM_BBX_ECC256_Sign( f_poBBXContext,
                                PB_DASTR(f_dastrSignedInfo),
                                CB_DASTR(f_dastrSignedInfo),
                                f_poECC256PubKey,
                                rgbECC256Signature ) );

    /* Add and open <SignatureValue> node.
    /*
    ** Output:
    ** <Signature ... >
    ** ...
    **  <SignatureValue>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigSignatureValueTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    cchScratch = CCH_BASE64_EQUIV( ECDSA_P256_SIGNATURE_SIZE_IN_BYTES );

    ChkDR( DRM_B64_EncodeA( rgbECC256Signature,
                            ECDSA_P256_SIGNATURE_SIZE_IN_BYTES,
                            rgchScratch,
                            &cchScratch,
                            0 ) );

    dastrScratch.cchString = cchScratch;

    ChkDR( DRM_XMB_AddDataA( f_poXMLContextA,
                            ( DRM_ANSI_CONST_STRING* )&dastrScratch ) );

    /* Close <SignatureValue> node. */
    /*
    ** Output:
    ** <Signature ... >
    ** ...
    **      <SignatureValue>...</SignatureValue>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_XMLSIG_BuildSignatureNode
**
** Synopsis: Function that constructs a signature XML segment of the
**           passed in data.
**
**           The signature XML segment should have a root node
**           <Signature>, and should have three subnodes under it:
**           <SignedInfo> that contains a SHA-256 hash of the passed in
**           data, <SignatureValue> that contains an ECC-256 signature
**           over the whole <SignedInfo> node, and <KeyInfo> that contains
**           a public key to verify the ECC-256 signature. Each subnode
**           has its own XML structure.
**
** Arguments:
**
** [f_poXMLContextA]        -- Pointer to ANSI XML builder context.
** [f_poBBXContext]         -- Pointer to a DRM_BB_CONTEXT context.
** [f_pszBase]              -- base XML data string for f_pdasstrData
**                             and other substrings.
** [f_pdasstrData]          -- Pointer to a non-empty substring
**                             for which the hash contained
**                             in the signature XML segment is calculated.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note:        Works only with DRM_XML_ECC256_PUBLIC_KEY_DATA format.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMLSIG_BuildSignatureNode(
    __inout       _XMBContextA   *f_poXMLContextA,
    __inout       DRM_BB_CONTEXT *f_poBBXContext,
    __in_ecount( f_pdasstrData->m_ich + f_pdasstrData->m_cch )
            const DRM_CHAR       *f_pszBase,
    __in    const DRM_SUBSTRING  *f_pdasstrData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrSignedInfo = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING dastrSignedInfo = EMPTY_DRM_STRING;
    DRM_ANSI_CONST_STRING dastrData = EMPTY_DRM_STRING;
    PUBKEY_P256 oECC256PubKey = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_XMLSIG, PERF_FUNC_DRM_XMLSIG_BuildSignatureNode );

    ChkArg( f_poXMLContextA != NULL );
    ChkArg( f_poBBXContext != NULL );
    ChkArg( f_pszBase != NULL );
    ChkArg( f_pdasstrData != NULL );
    ChkArg( f_pdasstrData->m_cch > 0 );

    /*
    ** Add and open <Signature> node.
    **
    ** Output:
    ** <Signature ... >
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigRootTag,
                              NULL,
                              &g_dastrSigRootAttribName,
                              &g_dastrSigRootAttribValue,
                              wttOpen ) );

    /*
    ** Write <SignedInfo>...</SignedInfo>
    */
    dastrData.pszString = f_pszBase + __CB_DECL( f_pdasstrData->m_ich );
    dastrData.cchString = f_pdasstrData->m_cch;

    ChkDR( _BuildSignedInfoNode( f_poXMLContextA,
                                 f_poBBXContext,
                                 &dastrData,
                                 &dasstrSignedInfo ) );

    /*
    ** Write <SignatureValue>...</SignatureValue>
    */
    dastrSignedInfo.pszString = f_poXMLContextA->rgchXML + dasstrSignedInfo.m_ich;
    dastrSignedInfo.cchString = dasstrSignedInfo.m_cch;

    ChkDR( _BuildSignatureValueNode( f_poXMLContextA,
                                     f_poBBXContext,
                                     &dastrSignedInfo,
                                     &oECC256PubKey ) );

    /*
    ** Build <KeyInfo> node.
    **
    ** Output:
    ** <Signature ... >
    ** ...
    **      <KeyInfo>
    **      ...
    **      </KeyInfo>
    */
    ChkDR( DRM_XMLSIG_BuildECC256PublicKeyInfoNode( f_poXMLContextA, &oECC256PubKey ) );

    /*
    ** Close <Signature> node.
    **
    ** Output:
    ** <Signature ... >
    ** ...
    ** </Signature>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

EXIT_PK_NAMESPACE_CODE;
