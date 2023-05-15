/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmutilitieslite.h>
#include <drmxmlparser.h>
#include <drmxmlsigconstants.h>
#include <drmxmlsig.h>

ENTER_PK_NAMESPACE_CODE;

/*********************************************************************
**
** Function: _VerifySHA256Hash
**
** Synopsis: Function that verifies a SHA-256 hash of the passed in
**           data.
**
** Arguments:
**
** [f_pszBase]              -- base string, all substrings
**                             are operating on it.
** [f_pdasstrData]          -- Pointer to substring with data against
**                             which a SHA-256 hash is verified.
** [f_rgbHash]              -- Pointer to a buffer that contains a SHA-256
**                             hash to be verified.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _VerifySHA256Hash(
    __in_ecount(f_pdasstrData->m_ich+f_pdasstrData->m_cch)
                                             const DRM_CHAR      *f_pszBase,
    __in                                     const DRM_SUBSTRING *f_pdasstrData,
    __in_bcount(SHA256_DIGEST_SIZE_IN_BYTES) const DRM_BYTE       f_rgbHash[ __CB_DECL( SHA256_DIGEST_SIZE_IN_BYTES ) ] )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_SHA_CONTEXT oSHAContext = { 0 };
    DRM_BYTE rgbCalculatedHash[ __CB_DECL( SHA256_DIGEST_SIZE_IN_BYTES ) ] = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_XMLSIG, PERF_FUNC__VerifySHA256Hash );

    ChkArg( f_pszBase != NULL );
    ChkArg( f_pdasstrData != NULL );
    ChkArg( f_pdasstrData->m_cch > 0 );
    ChkArg( f_rgbHash != NULL );

    ChkDR( DRM_SHA_Init( &oSHAContext, eDRM_SHA_256 ) );

    ChkDR( DRM_SHA_Update( ( DRM_BYTE * )( f_pszBase + __CB_DECL( f_pdasstrData->m_ich ) ),
                           f_pdasstrData->m_cch,
                           &oSHAContext ) );

    ChkDR( DRM_SHA_Finalize( &oSHAContext,
                             SHA256_DIGEST_SIZE_IN_BYTES,
                             rgbCalculatedHash ) );

    if ( MEMCMP( rgbCalculatedHash, f_rgbHash, SHA256_DIGEST_SIZE_IN_BYTES ) != 0 )
    {
        ChkDR( DRM_E_XMLSIG_SHA_VERIFY_FAILURE );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _GetCipherDataNodeCharCount
**
** Synopsis: Function that calculates the number of characters
**           in a <CipherData> XML node.
**
** Arguments:
** [f_cbData]               -- Number of bytes of data to be
**                             ciphered.
**
** Returns:                 A long integer value that is number of
**                          characters in a <CipherData> XML segment.
**
** Note:                    We allow cipher data of a zero length.
**
**********************************************************************/
static DRM_NO_INLINE DRM_DWORD DRM_CALL _GetCipherDataNodeCharCount(
    __in const DRM_DWORD f_cbData )
{
    DRM_DWORD cchSize = 0;

    /* <CipherData> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigCipherDataTag.cchString,
                                             0,
                                             0,
                                             0 );

    /* <CipherValue> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigCipherValueTag.cchString,
                                             0,
                                             0,
                                             0 );

    cchSize += CCH_BASE64_EQUIV( f_cbData );

    return cchSize;
}

/*********************************************************************
**
** Function: _BuildCipherDataNode
**
** Synopsis: Function that constructs a cipher data XML
**           segment of the passed in data.
**
**           The cipher data XML segment should have a root node
**           <CipherData>, and should have one subnode <CipherVaue>
**           under it. The <Cipher Value> node contains the passed in
**           data.
**
** Arguments:
**
** [f_poXMLContextA]        -- Pointer to a XML builder context, ANSI version.
** [f_pbData]               -- Pointer to data to be put in the
**                             <CipherValue> node. If this data is encrypted
**                             encryption should already be applied.
**                             If f_pbData is NULL, the function assumes
**                             the data to be B64 encoded is already
**                             stored at the right place in the buffer of the
**                             XML builder context.
** [f_cbData]               -- Size of the data to be B64 encoded.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildCipherDataNode(
    __inout _XMBContextA                     *f_poXMLContextA,
    __in_bcount_opt(f_cbData) const DRM_BYTE *f_pbData,
    __in                      const DRM_DWORD f_cbData )
{
    DRM_RESULT       dr        = DRM_SUCCESS;
    DRM_BYTE        *pbData    = NULL;
    const DRM_BYTE  *pbDataIn  = NULL;
    DRM_BYTE        *pbDataOut = NULL;
    DRM_DWORD        cchData   = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_XMLSIG, PERF_FUNC__BuildCipherDataNode );

    ChkArg( f_poXMLContextA != NULL );
    ChkArg( f_cbData > 0 );

    /*
    ** Add and open <CipherData> node.
    **
    ** Output:
    ** <CipherData>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigCipherDataTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add and open <CipherValue> node.
    **
    ** Output:
    ** <CipherData>
    **      <CipherValue>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigCipherValueTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    {
        ChkDR( DRM_XMB_GetCurrentBufferPointerA( f_poXMLContextA,
                                                 FALSE,
                                                 ( DRM_CHAR ** )&pbData ) );

        DRMASSERT( pbData != NULL );

        if ( f_pbData == NULL )
        {
            /* B64 encoding in place. */
            pbDataIn = pbData;
            pbDataOut = pbData;
        }
        else
        {
            pbDataIn = (DRM_BYTE*) f_pbData;
            pbDataOut = pbData;
        }

        cchData = CCH_BASE64_EQUIV( f_cbData );

        ChkDR( DRM_B64_EncodeA( pbDataIn,
                                f_cbData,
                                ( DRM_CHAR * )pbDataOut,
                                &cchData,
                                0 ) );

        ChkDR( DRM_XMB_ShiftCurrentPointerA( f_poXMLContextA, cchData ) );
    }

    /*
    ** Close <CipherValue> node.
    **
    ** Output:
    ** <CipherData>
    **      <CipherValue>
    **      ...
    **      </CipherValue>
    ** </CipherData>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

    /*
    ** Close <CipherData> node.
    **
    ** Output:
    ** <CipherData>
    ** ...
    ** </CipherData>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/************************************************************************************
**
** Function: _GetSignedInfoNodeCharCount
**
** Synopsis: Function that calculates the number of characters in a <SignedInfo>
**           XML segment.
**
** Arguments:               N/A
**
** Returns:                 A long integer value that is number of characters in
**                          a <SignedInfo> XML segment.
**
************************************************************************************/
static DRM_NO_INLINE DRM_DWORD _GetSignedInfoNodeCharCount( DRM_VOID )
{
    DRM_DWORD cchSize = 0;

    /* <SignedInfo> */
    cchSize = DRM_XMB_RequiredCharsForTagA( g_dastrSigSignedInfoTag.cchString,
                                            0,
                                            g_dastrSigSignedInfoAttribName.cchString,
                                            g_dastrSigSignedInfoAttribValue.cchString );

    /* <CanonicalizationMethod> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigCanonicalizationMethodTag.cchString,
                                             0,
                                             g_dastrSigCanonicalizationMethodAttribName.cchString,
                                             g_dastrSigCanonicalizationMethodAttribValue.cchString );

    /* <SignatureMethod> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigSignatureMethodTag.cchString,
                                             0,
                                             g_dastrSigSignatureMethodAttribName.cchString,
                                             g_dastrSigSignatureMethodAttribValue.cchString );

    /* <Reference> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigReferenceTag.cchString,
                                             0,
                                             g_dastrSigReferenceAttribName.cchString,
                                             g_dastrSigReferenceAttribValue.cchString );

    /* <DigestMethod> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigDigestMethodTag.cchString,
                                             0,
                                             g_dastrSigDigestMethodAttribName.cchString,
                                             g_dastrSigDigestMethodAttribValue.cchString );

    /* Add the B64-ed size of SHA-256 hash. */
    cchSize += CCH_BASE64_EQUIV( SHA256_DIGEST_SIZE_IN_BYTES );

    /* <DigestValue> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigDigestValueTag.cchString,
                                             0,
                                             g_dastrSigDigestMethodAttribName.cchString,
                                             g_dastrSigDigestMethodAttribValue.cchString );

    return cchSize;
}

/************************************************************************************
**
** Function: _GetSignatureValueNodeCharCount
**
** Synopsis: Function that calculates the number of characters in a <SignatureValue>
**           XML segment.
**
** Arguments:               N/A
**
** Returns:                 A long integer value that is number of characters in
**                          a <SignatureValue> XML segment.
**
** Note:                    Assumes that ECC-256 key is used to sign data.
**
************************************************************************************/
static DRM_DWORD _GetSignatureValueNodeCharCount( DRM_VOID )
{
    DRM_DWORD cchSize = 0;

    /* <SignatureValue> */
    cchSize = DRM_XMB_RequiredCharsForTagA( g_dastrSigSignatureValueTag.cchString,
                                            0,
                                            0,
                                            0 );

    /* Add the B64-ed size of ECC-256 signature. */
    cchSize += CCH_BASE64_EQUIV( ECDSA_P256_SIGNATURE_SIZE_IN_BYTES );

    return cchSize;
}

/*********************************************************************
**
** Function: _GetPublicKeyInfoNodeWithNameCharCount
**
** Synopsis: Function that calculates the number of characters in a
**           <KeyInfo> tag for pre-shared ECC-256 public key: e.g.
**           <KeyInfo><KeyName>WMRMServer</KeyName></KeyInfo>
**
** Arguments:
**
** [f_pdastrID]             -- Pointer to a DRM ANSI String that specifies the
**                             ID of a pre-shared ECC-256 public key.
**
** Returns:                 A long integer value that is number of characters
**                          in <KeyInfo> XML segment.
**
**********************************************************************/
static DRM_NO_INLINE DRM_DWORD _GetPublicKeyInfoNodeWithNameCharCount(
    __in const DRM_ANSI_CONST_STRING *f_pdastrID )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchSize = 0;

    ChkDRMANSIString( f_pdastrID );

    /* <KeyInfo> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigKeyInfoTag.cchString,
                                             0,
                                             g_dastrSigKeyInfoAttribName.cchString,
                                             g_dastrSigKeyInfoAttribValue.cchString );

    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigKeyNameTag.cchString,
                                             f_pdastrID->cchString,
                                             0,
                                             0 );

ErrorExit:

    return cchSize;
}

/*********************************************************************
**
** Function: _BuildPublicKeyInfoNodeWithName
**
** Synopsis: Function that constructs KeyInfo for ECC-256 public key
**           which is built into PK code: e.g.
**           <KeyInfo><KeyName>WMRMServer</KeyName></KeyInfo>
**
** Arguments:
**
** [f_poXMLContextA]         -- Pointer to ANSI XML builder context.
** [f_pdastrID]              -- Pointer to a DRM ANSI string that specifies the
**                              ID of a pre-shared ECC-256 public key.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _BuildPublicKeyInfoNodeWithName(
    __inout       _XMBContextA          *f_poXMLContextA,
    __in    const DRM_ANSI_CONST_STRING *f_pdastrID )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_XMLSIG, PERF_FUNC__BuildPublicKeyInfoNodeWithName );

    ChkArg( f_poXMLContextA != NULL );

    /*
    ** KeyName is always "WMRMServer" or "Metering" or "Client".
    */
    if ( !DRM_UTL_DASTRStringsEqual( f_pdastrID, &g_dastrSigKeyInfoKeyName1 ) &&
         !DRM_UTL_DASTRStringsEqual( f_pdastrID, &g_dastrSigKeyInfoKeyName2 ) &&
         !DRM_UTL_DASTRStringsEqual( f_pdastrID, &g_dastrSigKeyInfoKeyName3 ) )
    {
        ChkDR( DRM_E_XMLSIG_PUBLIC_KEY_ID );
    }

    /*
    ** Add <KeyInfo> node.
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigKeyInfoTag,
                              NULL,
                              &g_dastrSigKeyInfoAttribName,
                              &g_dastrSigKeyInfoAttribValue,
                              wttOpen ) );
    /*
    ** Add <KeyName> node.
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigKeyNameTag,
                              f_pdastrID,
                              NULL,
                              NULL,
                              wttOpen ) );

    /* Close <KeyName> */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

    /* Close <KeyInfo> */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _ExtractXMLPublicKeyByName
**
** Synopsis: Function that extracts a ECC-256 public key from a XML segment.
**           The XML segment should be in the form of
**           <KeyInfo><KeyName>name</KeyName></KeyInfo>.
**           The name of the root node can be any string.
**
** Arguments:
**
** [f_pszBase]              -- base XML data string, all substrings
**                             are operating on it.
** [f_pdasstrXML]           -- Pointer to a substring that contains a
**                             XML segment whose root node contains a
**                             <KeyInfo> subnode where an ECC-256 public key
**                             is stored.
** [f_pdastrID]             -- Pointer to ANSI string that specifies the
**                             ID of a pre-shared ECC-256 public key.
** [f_poECC256PubKey]       -- Pointer to an ECC-256 public key structure that
**                             will receive the extracted ECC-256 public key.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_XMLSIG_PUBLIC_KEY_ID if the passed
**                          in ID is unknown.
**
** Note: "KeyByName" functions take key name as parameter so that
**       in future we could support more pre-shared key names,
**       but right now there is only "WMRMServer".
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ExtractPublicKeyByName(
    __in_ecount(f_pdasstrXML->m_ich+f_pdasstrXML->m_cch) const DRM_CHAR              *f_pszBase,
    __in                                                 const DRM_SUBSTRING         *f_pdasstrXML,
    __in                                                 const DRM_ANSI_CONST_STRING *f_pdastrID,
    __out                                                      PUBKEY_P256           *f_poECC256PubKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrKeyNameValue = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_XMLSIG, PERF_FUNC__ExtractPublicKeyByName );

    ChkArg( f_pszBase != NULL );
    ChkArg( f_pdasstrXML != NULL );
    ChkArg( f_pdasstrXML->m_cch > 0 );
    ChkDRMANSIString( f_pdastrID );
    ChkArg( f_poECC256PubKey != NULL );

    /*
    ** pre-shared ECC-256 public key is always "WMRMServer" or "Client".
    */
    if ( !DRM_UTL_DASTRStringsEqual( f_pdastrID, &g_dastrSigKeyInfoKeyName1 ) &&
         !DRM_UTL_DASTRStringsEqual( f_pdastrID, &g_dastrSigKeyInfoKeyName2 ))
    {
        ChkDR( DRM_E_XMLSIG_PUBLIC_KEY_ID );
    }

    /*
    ** Extract <KeyName> and read the name of the key
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pszBase,
                                      f_pdasstrXML,
                                      &g_dastrSigKeyNameTag,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrKeyNameValue,
                                      g_chForwardSlash ) );

    /*
    ** Verify the key name and return it.
    */
    if ( DRM_UTL_DASSTRStringsEqual( f_pszBase, &dasstrKeyNameValue, f_pdastrID ) )
    {
        if ( DRM_UTL_DASTRStringsEqual( f_pdastrID, &g_dastrSigKeyInfoKeyName1 ) )
        {
            MEMCPY( ( DRM_BYTE * )f_poECC256PubKey,
                    ( DRM_BYTE * )&g_oWMRMECC256PubKey,
                    SIZEOF( PUBKEY_P256 ) );
        }
        else
        {
            /* If the key name is "Client", set the returned public key to be all 0. */
            OEM_ECC_ZeroPublicKey_P256( f_poECC256PubKey );
        }
    }
    else
    {
        ChkDR( DRM_E_XMLSIG_PUBLIC_KEY_ID );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _GetECC256PublicKeyInfoNodeCharCount
**
** Synopsis: Function that calculates the number of characters in
**           a <KeyInfo> XML segment with ECC256 key in the following format:
**              <KeyInfo>
**                  <KeyValue>
**                      <ECCKeyValue>
**                          <PublicKey>
**                              ...
**                          </PublicKey>
**                      </ECCKeyValue>
**                  </KeyValue>
**              </KeyInfo>
**
** Arguments:               N/A
**
** Returns:                 A long integer value that is number of characters
**                          in the <KeyInfo> XML segment.
**
**********************************************************************/
static DRM_NO_INLINE DRM_DWORD _GetECC256PublicKeyInfoNodeCharCount( DRM_VOID )
{
    DRM_DWORD cchSize = 0;

    /* <KeyInfo> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigKeyInfoTag.cchString,
                                             0,
                                             g_dastrSigKeyInfoAttribName.cchString,
                                             g_dastrSigKeyInfoAttribValue.cchString );

    /* <KeyValue> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigKeyValueTag.cchString,
                                             0,
                                             0,
                                             0 );

    /* <ECCKeyValue> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigECCKeyValueTag.cchString,
                                             0,
                                             0,
                                             0 );

    /* <PublicKey> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigPublicKeyTag.cchString,
                                             0,
                                             0,
                                             0 );

    /* Add the B64-ed size of public key. */
    cchSize += CCH_BASE64_EQUIV( SIZEOF( PUBKEY_P256) );

    return cchSize;
}

/*********************************************************************
**
** Function: DRM_XMLSIG_BuildECC256PublicKeyInfoNode
**
** Synopsis: Function that constructs a public key XML segment
**           for ECC-256 public key.
**
**           The public key XML segment should have a root node
**           <KeyInfo>, then <KeyValue> node under it.
**           Under <KeyValue> there is a <ECCKeyValue><PublicKey>
**           node that contains the B64 encoded ECC-256 public key.
**
** Arguments:
**
** [f_poXMLContextA]        -- Pointer to ANSI XML builder context.
** [f_poECC256PubKey]       -- Pointer to a PUBKEY_P256 structure that
**                             is going to be encoded in the generated
**                             XML segment.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMLSIG_BuildECC256PublicKeyInfoNode(
    __inout       _XMBContextA *f_poXMLContextA,
    __in    const PUBKEY_P256  *f_poECC256PubKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR rgchScratch[ CCH_BASE64_EQUIV( SIZEOF( PUBKEY_P256 ) ) ];
    DRM_DWORD cchScratch = 0;
    DRM_ANSI_STRING dastrScratch = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_XMLSIG, PERF_FUNC__BuildECC256PublicKeyInfoNode );

    ChkArg( f_poXMLContextA != NULL );
    ChkArg( f_poECC256PubKey != NULL );

    dastrScratch.pszString = rgchScratch;

    /*
    ** Add and open <KeyInfo> node.
    **
    ** Output:
    ** <KeyInfo>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigKeyInfoTag,
                              NULL,
                              &g_dastrSigKeyInfoAttribName,
                              &g_dastrSigKeyInfoAttribValue,
                              wttOpen ) );

    /*
    ** Add and open <KeyValue> node.
    **
    ** Output:
    ** <KeyInfo>
    **      <KeyValue>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigKeyValueTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add and open <ECCKeyValue> node.
    **
    ** Output:
    ** <KeyInfo>
    **      <KeyValue>
    **          <ECCKeyValue>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigECCKeyValueTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    /*
    ** Add and open <PublicKey> node.
    **
    ** Output:
    ** <KeyInfo>
    **      <KeyValue>
    **          <ECCKeyValue>
    **              <PublicKey>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigPublicKeyTag,
                              NULL,
                              NULL,
                              NULL,
                              wttOpen ) );

    cchScratch = CCH_BASE64_EQUIV( SIZEOF( PUBKEY_P256 ) );

    ChkDR( DRM_B64_EncodeA( ( DRM_BYTE * )f_poECC256PubKey->m_rgbPubkey,
                            SIZEOF( PUBKEY_P256 ),
                            rgchScratch,
                            &cchScratch,
                            0 ) );

    DRMASSERT( cchScratch == CCH_BASE64_EQUIV( SIZEOF( PUBKEY_P256 ) ) );

    dastrScratch.cchString = cchScratch;

    ChkDR( DRM_XMB_AddDataA( f_poXMLContextA,
                            ( DRM_ANSI_CONST_STRING* )&dastrScratch ) );

    /*
    ** Close <PublicKey> node.
    **
    ** Output:
    ** <KeyInfo>
    **      <KeyValue>
    **          <ECCKeyValue>
    **              <PublicKey>
    **              ...
    **              </PublicKey>
    **          </ECCKeyValue>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

    /*
    ** Close <ECCKeyValue> node.
    **
    ** Output:
    ** <KeyInfo>
    **      <KeyValue>
    **          <ECCKeyValue>
    **          ...
    **          </ECCKeyValue>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

    /*
    ** Close <KeyValue> node.
    **
    ** Output:
    ** <KeyInfo>
    **      <KeyValue>
    **      ...
    **      </KeyValue>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

    /*
    ** Close <KeyInfo> node.
    **
    ** Output:
    ** <KeyInfo>
    ** ...
    ** </KeyInfo>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _ExtractECC256PublicKey
**
** Synopsis: Function that extracts an ECC-256 public key from a XML segment.
**           The XML segment should be in the form of
**           <KeyInfo>/<KeyValue>/<ECCKeyValue>/<PublicKey>.
**           Under the <KeyInfo>/<KeyValue>/<ECCKeyValue>/<PublicKey> node,
**           there is a B64 encoded ECC-256 public key.
**
** Arguments:
**
** [f_pszBase]              -- base XML data string, all substrings
**                             are operating on it.
** [f_pdasstrXML]           -- Pointer to a substring that contains a
**                             XML segment whose root node contains a
**                             <KeyInfo> subnode where an ECC-256 public
**                             key is stored.
** [f_poECC256PubKey]       -- Pointer to an ECC-256 public key that is used
**                             to receive the extracted ECC-256 public key.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_XMLNOTFOUND if XML tags are not found
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _ExtractECC256PublicKey(
    __in_ecount(f_pdasstrXML->m_ich+f_pdasstrXML->m_cch) const DRM_CHAR      *f_pszBase,
    __in                                                 const DRM_SUBSTRING *f_pdasstrXML,
    __out                                                      PUBKEY_P256   *f_poECC256PubKey )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrPublicKey  = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cbSize = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_XMLSIG, PERF_FUNC__ExtractECC256PublicKey );

    ChkArg( f_pszBase != NULL );
    ChkArg( f_pdasstrXML != NULL );
    ChkArg( f_pdasstrXML->m_cch > 0 );
    ChkArg( f_poECC256PubKey != NULL );

    /*
    ** Extract the public key from the passed in root node.
    */
    if ( DRM_FAILED( DRM_XML_GetSubNodeByPathA( f_pszBase,
                                                f_pdasstrXML,
                                                &g_dastrSigPublicKeyPath,
                                                NULL,
                                                NULL,
                                                NULL,
                                                &dasstrPublicKey,
                                                g_chForwardSlash ) ) )
    {
        ChkDR( DRM_E_XMLNOTFOUND );
    }

    cbSize = SIZEOF( PUBKEY_P256 );

    ChkDR( DRM_B64_DecodeA( f_pszBase,
                            &dasstrPublicKey,
                            &cbSize,
                            f_poECC256PubKey->m_rgbPubkey,
                            0 ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _GetEncryptedDataNodePrefixCharCount
**
** Synopsis: Function that calculates the prefix portion of the
**           <EncryptedData> node. The prefix portion includes
**           all characters immediately before the actual data to be
**           B64 encoded.
**
** Arguments:
** [f_poXMLKey]             -- Pointer to a DRM_XML_KEY that contains a
**                             information of how to build a <KeyInfo>
**                             XML segment.
** [f_pcchSize]             -- Pointer to a variable to receive the
**                             calculated character count.
**
** Returns:
**                          DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_NO_INLINE DRM_RESULT _GetEncryptedDataNodePrefixCharCount(
    __in  const DRM_XML_KEY *f_poXMLKey,
    __out       DRM_DWORD   *f_pcchSize )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchSize = 0;

    ChkArg( f_poXMLKey != NULL );
    ChkArg( f_poXMLKey->m_eKeyInfoType == DRM_XML_KEYINFO_ENCRYPTED );
    ChkArg( f_pcchSize != NULL );

    /* <EncryptedData Type="..."> */
    cchSize += DRM_XMB_RequiredCharsForTagNoCloseA( g_dastrSigEncryptedDataTag.cchString,
                                                    0,
                                                    g_dastrSigEncryptedDataAttrib1Name.cchString,
                                                    g_dastrSigEncryptedDataAttrib1Value.cchString );

    cchSize += DRM_XMB_RequiredCharsForAttributeA( g_dastrSigEncryptedDataAttrib2Name.cchString,
                                                   g_dastrSigEncryptedDataAttrib2Value.cchString );

    /* <EncryptionMethod Algorithm="../xmlenc#aes128cbc"> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigEncryptionMethodTag.cchString,
                                             0,
                                             g_dastrSigEncryptionMethodAttrib1Name.cchString,
                                             g_dastrSigEncryptionMethodAttrib1Value.cchString );

    /* <KeyInfo> */
    ChkDR( DRM_XMLSIG_GetEncryptedKeyInfoNodeCharCount( f_poXMLKey, f_pcchSize ) );

    /* <CipherData> */
    cchSize += DRM_XMB_RequiredCharsForTagNoCloseA( g_dastrSigCipherDataTag.cchString,
                                                    0,
                                                    0,
                                                    0 );

    /* <CipherValue> */
    cchSize += DRM_XMB_RequiredCharsForTagNoCloseA( g_dastrSigCipherValueTag.cchString,
                                                    0,
                                                    0,
                                                    0 );

    *f_pcchSize += cchSize;

ErrorExit:

    return dr;
}


/*********************************************************************
**
** Function: DRM_XMLSIG_ExtractCipherData
**
** Synopsis: Function that extracts a cipher data buffer contained in
**           a XML segment and applies B64 decoder.
**           This function does not decrypt cipher data.
**           The XML segment should be in the form of
**           <...>/<CipherData>/<CipherValue>, and the name of the root
**           node can be any string.
**
** Arguments:
**
** [f_pszBase]              -- base XML data string for f_pdasstrXML
**                             and other substrings.
** [f_pdasstrXML]           -- Pointer to a substring that contains a
**                             XML segment whose root node contains a
**                             <CipherData> subnode.
** [f_ppbCipherData]        -- Pointer to a pointer to a buffer that
**                             receives the B64 decoded cipher data
**                             contained in the XML segment.
** [f_pcbCipherData]        -- Pointer to a variable to receive the size
**                             (number of bytes) of the B64 decoded cipher
**                             data contained in the XML segment.
** [f_pdasstrCipherData]    -- Pointer to a variable to receive the a
**                             substring that cover the encoded cipher data.
**                             It is the caller's responsiblity to cover
**                             the the substring with space (' ') to
**                             ensure the whole XML string is still valid
**                             for future parsing.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMLSIG_ExtractCipherData(
    __inout_ecount(f_pdasstrXML->m_ich+f_pdasstrXML->m_cch)
                    DRM_CHAR      *f_pszBase,
    __in      const DRM_SUBSTRING *f_pdasstrXML,
    __deref_out_bcount( *f_pcbCipherData )
                    DRM_BYTE     **f_ppbCipherData,
    __out           DRM_DWORD     *f_pcbCipherData,
    __out_opt       DRM_SUBSTRING *f_pdasstrCipherData )
{
    DRM_RESULT    dr               = DRM_SUCCESS;
    DRM_SUBSTRING dasstrCipherData = EMPTY_DRM_SUBSTRING;
    DRM_DWORD     cchExtra         = 0;
    DRM_BYTE     *pbCipherData     = NULL;
    DRM_DWORD     cbCipherData     = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_XMLSIG, PERF_FUNC_DRM_XMLSIG_ExtractCipherData );

    ChkArg( f_pszBase != NULL );
    ChkArg( f_pdasstrXML != NULL );
    ChkArg( f_pdasstrXML->m_cch > 0 );
    ChkArg( f_ppbCipherData != NULL );
    ChkArg( f_pcbCipherData != NULL );

    /*
    ** Extract <CipherValue> node from the passed in root node.
    ** It contains the cipher data string.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pszBase,
                                      f_pdasstrXML,
                                      &g_dastrSigCipherValuePath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrCipherData,
                                      g_chForwardSlash ) );

    if ( f_pdasstrCipherData != NULL )
    {
        *f_pdasstrCipherData = dasstrCipherData;
    }

    cbCipherData = dasstrCipherData.m_cch;

    if ( cbCipherData > 0 )
    {
        ChkDR( DRM_B64_DecodeA( f_pszBase,
                                &dasstrCipherData,
                                &cbCipherData,
                                NULL,
                                DRM_BASE64_DECODE_IN_PLACE ) );

        /* Make sure that size of decoded buffer is in proper range and satisfy OACR */
        ChkBOOL( dasstrCipherData.m_ich + cbCipherData <= f_pdasstrXML->m_ich + f_pdasstrXML->m_cch, DRM_E_BUFFER_BOUNDS_EXCEEDED );

        if ( dasstrCipherData.m_ich % SIZEOF( DRM_DWORD ) != 0 )
        {
            /* If the encoded string is not beginning at an aligned address,
            ** shift it to make aligned.
            */
            cchExtra = SIZEOF( DRM_DWORD ) - ( dasstrCipherData.m_ich % SIZEOF( DRM_DWORD ) );

            /* *f_ppbCipherData will point to an aligned address. */
            pbCipherData = ( DRM_BYTE * )f_pszBase + __CB_DECL( dasstrCipherData.m_ich + cchExtra );

            /* validation of buffer size */
            ChkBOOL( f_pdasstrXML->m_ich+f_pdasstrXML->m_cch >= __CB_DECL( dasstrCipherData.m_ich + cchExtra ) + cbCipherData,
                     DRM_E_BUFFERTOOSMALL );
            DRM_BYT_MoveBytes( pbCipherData,
                               0,
                               ( DRM_BYTE * )f_pszBase,
                               dasstrCipherData.m_ich,
                               cbCipherData );
        }
        else
        {
            /* The encoded string is beginning at an aligned address. */
            pbCipherData = ( DRM_BYTE * )f_pszBase + __CB_DECL( dasstrCipherData.m_ich );
        }
    }
    else
    {
        /* If <CipherValue> does not have any content, something is wrong. */
        ChkDR( DRM_E_XMLSIG_FORMAT );
    }

    *f_ppbCipherData = pbCipherData;
    *f_pcbCipherData = cbCipherData;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/*********************************************************************
**
** Function: DRM_XMLSIG_GetSimpleECC256KeyInfoNodeCharCount
**
** Synopsis: Function that calculates the number of characters in a <KeyInfo>
**           XML segment. Size depends on wether we write a pre-shared ECC-256
**           key pair name or a full ECC-256 public key.
**
** Arguments:
** [f_eECC256KeyFormat]     -- ECC-256 key format
** [f_pcchSize]             -- Pointer to a variable to be incremented
**                             by a result of calculations, cannot be NULL,
**                             and a variable should be initialized.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMLSIG_GetSimpleECC256KeyInfoNodeCharCount(
    __in    const DRM_XML_ECC256_KEY_FORMAT f_eECC256KeyFormat,
    __out         DRM_DWORD                *f_pcchSize )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchSize = 0;

    ChkArg( f_pcchSize != NULL );
    ChkArg( f_eECC256KeyFormat == DRM_XML_ECC256_SERVER_PUBLIC_KEY_ID ||
            f_eECC256KeyFormat == DRM_XML_ECC256_CLIENT_PUBLIC_KEY_ID ||
            f_eECC256KeyFormat == DRM_XML_ECC256_METERING_PUBLIC_KEY_ID ||
            f_eECC256KeyFormat == DRM_XML_ECC256_PUBLIC_KEY_DATA );

    if ( f_eECC256KeyFormat == DRM_XML_ECC256_SERVER_PUBLIC_KEY_ID )
    {
        cchSize = _GetPublicKeyInfoNodeWithNameCharCount(
            ( DRM_ANSI_CONST_STRING * )&g_dastrSigKeyInfoKeyName1
            );
    }
    else if ( f_eECC256KeyFormat == DRM_XML_ECC256_CLIENT_PUBLIC_KEY_ID )
    {
        cchSize = _GetPublicKeyInfoNodeWithNameCharCount(
            ( DRM_ANSI_CONST_STRING * )&g_dastrSigKeyInfoKeyName2
            );
    }
    else if ( f_eECC256KeyFormat == DRM_XML_ECC256_METERING_PUBLIC_KEY_ID )
    {
        cchSize = _GetPublicKeyInfoNodeWithNameCharCount(
            ( DRM_ANSI_CONST_STRING * )&g_dastrSigKeyInfoKeyName3
            );
    }
    else if ( f_eECC256KeyFormat == DRM_XML_ECC256_PUBLIC_KEY_DATA )
    {
        cchSize = _GetECC256PublicKeyInfoNodeCharCount();
    }
    else
    {
        ChkDR( DRM_E_XMLSIG_INVALID_KEY_FORMAT );
    }

ErrorExit:

    *f_pcchSize += cchSize;

    return dr;
}

/*********************************************************************
**
** Function: DRM_XMLSIG_BuildSimpleECC256KeyInfoNode
**
** Synopsis: Function that constructs KeyInfo XML node for a given
**           ECC-256 public key. It supports two formats:
**           either there is <KeyValue> node with a full ECC-256 public
**           key or a reference to a pre-shared ECC-256 public key.
**
** Arguments:
**
** [f_poXMLContextA]        -- Pointer to ANSI XML builder context.
** [f_poECC256PubKey]       -- Pointer to a PUBKEY_P256 structure that
**                             is going to be encoded in the generated
**                             XML segment.
** [f_eECC256KeyFormat]     -- ECC-256 key info format.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note:  If key format is DRM_XML_ECC256_PUBLIC_KEY_ID then the value of
**        ECC0256 public key does not matter, but we still check it for NULL.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMLSIG_BuildSimpleECC256KeyInfoNode(
    __inout       _XMBContextA             *f_poXMLContextA,
    __in    const PUBKEY_P256              *f_poECC256PubKey,
    __in    const DRM_XML_ECC256_KEY_FORMAT f_eECC256KeyFormat )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poXMLContextA != NULL );
    ChkArg( f_poECC256PubKey != NULL );
    ChkArg( f_eECC256KeyFormat != DRM_XML_ECC256_INVALID_KEY );

    if ( f_eECC256KeyFormat == DRM_XML_ECC256_SERVER_PUBLIC_KEY_ID )
    {
        ChkDR( _BuildPublicKeyInfoNodeWithName( f_poXMLContextA,
                    ( DRM_ANSI_CONST_STRING * )&g_dastrSigKeyInfoKeyName1 ) );
    }
    else if ( f_eECC256KeyFormat == DRM_XML_ECC256_CLIENT_PUBLIC_KEY_ID )
    {
        ChkDR( _BuildPublicKeyInfoNodeWithName( f_poXMLContextA,
                    ( DRM_ANSI_CONST_STRING * )&g_dastrSigKeyInfoKeyName2 ) );
    }
    else if ( f_eECC256KeyFormat == DRM_XML_ECC256_METERING_PUBLIC_KEY_ID )
    {
        ChkDR( _BuildPublicKeyInfoNodeWithName( f_poXMLContextA,
                    ( DRM_ANSI_CONST_STRING * )&g_dastrSigKeyInfoKeyName3 ) );
    }
    else if ( f_eECC256KeyFormat == DRM_XML_ECC256_PUBLIC_KEY_DATA )
    {
        ChkDR( DRM_XMLSIG_BuildECC256PublicKeyInfoNode( f_poXMLContextA, f_poECC256PubKey) );
    }
    else
    {
        ChkDR( DRM_E_XMLSIG_INVALID_KEY_FORMAT );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_XMLSIG_ExtractSimpleECC256Key
**
** Synopsis: Function that extracts an ECC-256 public key from a XML
**           segment that it is correctly-formed <KeyInfo>. The XML
**           segment may contain ECC-256 key name or ECC-256 key
**           parameters.
**           Function does not know the exact format of <KeyInfo>
**           and attempts to find ECC-256 key parameters first,
**           then if failed looks for a pre-shared ECC-256 key name.
**
** Arguments:
**
** [f_pszBase]              -- base XML data string for f_pdasstrXML.
** [f_pdasstrXML]           -- Pointer to a substring that contains a
**                             XML segment whose root node contains a
**                             <KeyInfo> subnode where an ECC-256 public
**                             key is stored.
** [f_poECC256PubKey]       -- Pointer to an ECC-256 public key that is
**                             used to receive the extracted ECC-256
**                             public key.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMLSIG_ExtractSimpleECC256Key(
    __in_ecount( f_pdasstrXML->m_ich + f_pdasstrXML->m_cch ) const DRM_CHAR      *f_pszBase,
    __in                                                     const DRM_SUBSTRING *f_pdasstrXML,
    __out                                                          PUBKEY_P256   *f_poECC256PubKey )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pszBase != NULL );
    ChkArg( f_pdasstrXML != NULL );
    ChkArg( f_pdasstrXML->m_cch > 0 );
    ChkArg( f_poECC256PubKey != NULL );

    dr = _ExtractECC256PublicKey( f_pszBase,
                                  f_pdasstrXML,
                                  f_poECC256PubKey );
    /*
    ** XML error means that the key may be stored in a different format.
    */
    if ( dr == DRM_E_XMLNOTFOUND )
    {
        dr = DRM_SUCCESS;

        /*
        ** Extract a pre-shared ECC-256 public key.
        */
        if ( DRM_FAILED( _ExtractPublicKeyByName( f_pszBase,
                                                  f_pdasstrXML,
                                                  ( DRM_ANSI_CONST_STRING * ) &g_dastrSigKeyInfoKeyName1,
                                                  f_poECC256PubKey ) ) )
        {
            ChkDR( _ExtractPublicKeyByName( f_pszBase,
                                            f_pdasstrXML,
                                            ( DRM_ANSI_CONST_STRING * ) &g_dastrSigKeyInfoKeyName2,
                                            f_poECC256PubKey ) );
        }
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_XMLSIG_GetEncryptedKeyInfoNodeCharCount
**
** Synopsis: Function that calculates the number of characters in <KeyInfo>
**           XML segment that contains AES key encrypted with ECC-256 key.
**
** Arguments:
** [f_poXMLKey]             -- Pointer to a DRM_XML_KEY that contains
**                             information of a key to be put in the
**                             <KeyInfo> XML segment.
** [f_pcchSize]             -- Pointer to a variable to be incremented
**                             by a result of calculations, cannot be NULL,
**                             and a variable should be initialized.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note:    Keys values inside DRM_XML_KEY are not needed for calculations.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMLSIG_GetEncryptedKeyInfoNodeCharCount(
    __in    const DRM_XML_KEY *f_poXMLKey,
    __inout       DRM_DWORD   *f_pcchSize )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchSize = 0;

    ChkArg( f_poXMLKey != NULL );
    ChkArg( f_poXMLKey->m_eKeyInfoType == DRM_XML_KEYINFO_ENCRYPTED );
    ChkArg( f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_SERVER_PUBLIC_KEY_ID ||
            f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_CLIENT_PUBLIC_KEY_ID ||
            f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_METERING_PUBLIC_KEY_ID ||
            f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_PUBLIC_KEY_DATA );
    ChkArg( f_pcchSize != NULL );

    /* <KeyInfo> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigKeyInfoTag.cchString,
                                             0,
                                             g_dastrSigKeyInfoAttribName.cchString,
                                             g_dastrSigKeyInfoAttribValue.cchString );

    /* <EncryptedKey> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigEncryptedKeyTag.cchString,
                                             0,
                                             g_dastrSigEncryptedKeyAttribName.cchString,
                                             g_dastrSigEncryptedKeyAttribValue.cchString );

    /* <EncryptionMethod Algorithm="..."> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigEncryptionMethodTag.cchString,
                                             0,
                                             g_dastrSigEncryptionMethodAttrib2Name.cchString,
                                             g_dastrSigEncryptionMethodAttrib2Value.cchString );

    ChkDR( DRM_XMLSIG_GetSimpleECC256KeyInfoNodeCharCount( f_poXMLKey->m_eKeyFormat, f_pcchSize ) );

    cchSize += _GetCipherDataNodeCharCount( SIZEOF( _AES_KEY ) );

ErrorExit:

    *f_pcchSize += cchSize;

    return dr;
}

/*********************************************************************************
** Function: DRM_XMLSIG_BuildEncryptedKeyInfoNode
**
** Synopsis: Function constructs a complex <KeyInfo> XML node
**           that contains an AES key encrypted with ECC-256 algorithm
**           together with ECC-256 public key used to encrypt it:
**
**           <KeyInfo xmlns="...">
**               <EncryptedKey>
**                   <EncryptionMethod Algorithm="rsa"/>
**                   <KeyInfo>
**                      (ECC-256 KeyInfo)
**                   </KeyInfo>
**                   <CipherData>
**                       (cypher data: encrypted AES key)
**                   </CipherData>
**               </EncryptedKey>
**           </KeyInfo>
**
** Arguments:
**
** [f_poXMLContextA]        -- Pointer to ANSI XML builder context.
** [f_poXMLKey]             -- Pointer to a DRM_XML_KEY that contains
**                             information of a key to be put in the
**                             <KeyInfo> XML segment.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note: DRM_XML_KEY structure should already contain encrypted AES key data
** because this API will not encrypt the AES key itself.
**
*********************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMLSIG_BuildEncryptedKeyInfoNode(
    __inout       _XMBContextA *f_poXMLContextA,
    __in    const DRM_XML_KEY  *f_poXMLKey )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_XMLSIG, PERF_FUNC_DRM_XMLSIG_BuildEncryptedKeyInfoNode );

    ChkArg( f_poXMLContextA != NULL );
    ChkArg( f_poXMLKey != NULL );
    ChkArg( f_poXMLKey->m_eKeyInfoType == DRM_XML_KEYINFO_ENCRYPTED );
    ChkArg( f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_SERVER_PUBLIC_KEY_ID ||
            f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_METERING_PUBLIC_KEY_ID ||
            f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_PUBLIC_KEY_DATA );

    /*
    ** Add and open <KeyInfo> node.
    **
    ** Output:
    ** <KeyInfo>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigKeyInfoTag,
                              NULL,
                              &g_dastrSigKeyInfoAttribName,
                              &g_dastrSigKeyInfoAttribValue,
                              wttOpen ) );

    /*
    ** Add and open <EncryptedKey> node.
    **
    ** Output:
    ** <KeyInfo ... >
    **      <EncryptedKey>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigEncryptedKeyTag,
                              NULL,
                              &g_dastrSigEncryptedKeyAttribName,
                              &g_dastrSigEncryptedKeyAttribValue,
                              wttOpen ) );

    /*
    ** Add and close <EncryptionMethod> node.
    **
    ** Output:
    ** <KeyInfo ... >
    **      <EncryptedKey ... >
    **          <EncryptionMethod/>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigEncryptionMethodTag,
                              NULL,
                              &g_dastrSigEncryptionMethodAttrib2Name,
                              &g_dastrSigEncryptionMethodAttrib2Value,
                              wttClosed ) );

    /*
    ** Build <KeyInfo> node.
    **
    ** Output:
    ** <KeyInfo ... >
    ** ...
    **      <EncryptedKey ... >
    **      ...
    **          <KeyInfo>...</KeyInfo>
    */
    ChkDR( DRM_XMLSIG_BuildSimpleECC256KeyInfoNode( f_poXMLContextA,
                                                    &f_poXMLKey->m_oECC256PubKey,
                                                    f_poXMLKey->m_eKeyFormat ) );

    /*
    ** Build <CipherData> node.
    **
    ** Output:
    ** <KeyInfo ... >
    ** ...
    **      <EncryptedKey ... >
    **      ...
    **          <CypherData>...</CipherData>
    */
    ChkDR( _BuildCipherDataNode( f_poXMLContextA,
                                 f_poXMLKey->m_oAESKey.m_rgbCipherAESKey,
                                 SIZEOF( _AES_KEY  ) ) );

    /*
    ** Close <EncryptedKey> node.
    **
    ** Output:
    ** <KeyInfo ...>
    ** ...
    **      <EncryptedKey>
    **      ...
    **      </EncryptedKey>
    ** </KeyInfo>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

    /*
    ** Close <KeyInfo> node.
    **
    ** Output:
    ** <KeyInfo ...>
    ** ...
    ** </KeyInfo>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, NULL ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/************************************************************************************
**
** Function: DRM_XMLSIG_GetSignatureNodeCharCount
**
** Synopsis: Function that calculates the number of characters in <Signature>
**           XML segment.
**
** Arguments:
**
** [f_pcchSize]             -- Pointer to a variable to be incremented
**                             by a result of calculations, cannot be NULL,
**                             and a variable should be initialized.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note:                    Supports only DRM_XML_ECC256_PUBLIC_KEY_DATA format.
**
************************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMLSIG_GetSignatureNodeCharCount(
    __inout DRM_DWORD *f_pcchSize )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchSize = 0;

    ChkArg( f_pcchSize != NULL );

    /* <Signature xmlns="..."> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigRootTag.cchString,
                                             0,
                                             g_dastrSigRootAttribName.cchString,
                                             g_dastrSigRootAttribValue.cchString );

    /* Add the size of a SignedInfo XML segment. */
    cchSize += _GetSignedInfoNodeCharCount();

    /* size of a SignatureValue XML segment. */
    cchSize += _GetSignatureValueNodeCharCount();

    /* size of a public key XML segment. */
    cchSize += _GetECC256PublicKeyInfoNodeCharCount();

ErrorExit:

    *f_pcchSize += cchSize;

    return dr;
}

/*********************************************************************
**
** Function: DRM_XMLSIG_VerifySignature
**
** Synopsis: Function that verifies the integrity of the data. First an
**           ECC-256 signature is calculated for the whole <SignedInfo>
**           node and is compared against an ECC-256 signature contained
**           in the <SignatureValue> node. If it is OK, a SHA-256 hash
**           contained in the <DigestValue> node is retrieved and is
**           compared against the calculated SHA-256 hash of the passed
**           in data for integrity verification.
**
** Arguments:
**
** [f_pszDataBase]           -- Base string for f_pdasstrData
** [f_pdasstrData]           -- Pointer to a substring that whose signature
**                              is going to be verified against a
**                              signature contained in the signature XML
**                              segment.
** [f_pszSignatureBase]      -- base string for f_pdasstrSignature
**
** [f_pdasstrSignature]      -- Pointer to a substring that contains the
**                              signature XML segment in the form of
**                              <Signature>...</Signature>.
** [f_poECC256PubKey]        -- Pointer to ECC256 public key thay used if <KeyInfo> public key not found
** [f_pCryptoCtx]            -- Pointer to Crypto Context used for temporary data in crypto code.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_XMLSIG_SHA_HASH_SIZE if the size of hash is
**                          unexpected.
**                          DRM_E_XMLSIG_ECDSA_SIGNATURE_SIZE if the size of
**                          ECDSA signature is unexpected.
**                          DRM_E_XMLSIG_SHA_VERIFY_FAILURE if the SHA-256
**                          hash verfication failed.
**                          DRM_E_XMLSIG_ECDSA_VERIFY_FAILURE of ECDSA
**                          signature verification failed.
**                          DRM_E_XMLNOTFOUND if expected node cannot be found.
**
** Note: If <KeyInfo> node cannot be found, assume caller wants to use WMRM
**       server public key.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMLSIG_VerifySignature(
    __in_ecount(f_pdasstrData->m_ich+f_pdasstrData->m_cch)           const DRM_CHAR             *f_pszDataBase,
    __in                                                             const DRM_SUBSTRING        *f_pdasstrData,
    __in_ecount(f_pdasstrSignature->m_ich+f_pdasstrSignature->m_cch) const DRM_CHAR             *f_pszSignatureBase,
    __in                                                             const DRM_SUBSTRING        *f_pdasstrSignature,
    __in                                                             const PUBKEY_P256          *f_poECC256PubKey,
    __inout                                                                DRM_CRYPTO_CONTEXT   *f_pCryptoCtx )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrSignedInfo = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrKeyInfo = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrHashValue = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrSignatureValue = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cbSize = 0;
    DRM_BYTE rgbHash[ __CB_DECL( SHA256_DIGEST_SIZE_IN_BYTES ) ] = { 0 };
    DRM_BYTE rgbECC256Signature[ __CB_DECL( ECDSA_P256_SIGNATURE_SIZE_IN_BYTES ) ] = { 0 };
    PUBKEY_P256 oECC256PubKey = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_XMLSIG, PERF_FUNC_DRM_XMLSIG_VerifySignature );

    ChkArg( f_pszDataBase != NULL );
    ChkArg( f_pdasstrData != NULL );
    ChkArg( f_pdasstrData->m_cch > 0 );
    ChkArg( f_pszSignatureBase != NULL );
    ChkArg( f_pdasstrSignature != NULL );
    ChkArg( f_pdasstrSignature->m_cch > 0 );
    ChkArg( f_poECC256PubKey != NULL );
    ChkArg( f_pCryptoCtx != NULL );

    /*
    ** Extract <SignedInfo> node from <Signature> node.
    ** The integrity of the node will be verified using an ECC-256
    ** signature as illustrated below.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pszSignatureBase,
                                      f_pdasstrSignature,
                                      &g_dastrSigSignedInfoPath,
                                      NULL,
                                      NULL,
                                      &dasstrSignedInfo,
                                      NULL,
                                      g_chForwardSlash ) );

    ChkBOOL( dasstrSignedInfo.m_ich + dasstrSignedInfo.m_cch <=
             f_pdasstrSignature->m_ich + f_pdasstrSignature->m_cch,
             DRM_E_XMLSIG_ECDSA_VERIFY_FAILURE );

    /*
    ** Extract <DigestValue> node from <SignInfo>/<Reference> node.
    ** It contains a SHA-256 hash of the passed in data.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pszSignatureBase,
                                      f_pdasstrSignature,
                                      &g_dastrSigHashPath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrHashValue,
                                      g_chForwardSlash ) );

    cbSize = SHA256_DIGEST_SIZE_IN_BYTES;

    ChkDR( DRM_B64_DecodeA( f_pszSignatureBase,
                            &dasstrHashValue,
                            &cbSize,
                            rgbHash,
                            0 ) );

    ChkBOOL( cbSize == SHA256_DIGEST_SIZE_IN_BYTES, DRM_E_XMLSIG_SHA_HASH_SIZE );

    /*
    ** Extract a public key from the <KeyInfo> node. The public key
    ** is used to verify the ECC-256 signature of the <SignedInfo> node.
    */
    dr = DRM_XML_GetSubNodeByPathA( f_pszSignatureBase,
                                    f_pdasstrSignature,
                                    &g_dastrSigKeyInfoPath,
                                    NULL,
                                    NULL,
                                    &dasstrKeyInfo,
                                    NULL,
                                    g_chForwardSlash );

    if ( DRM_FAILED( dr ) )
    {
        /*
        ** If <KeyInfo> node cannot be found, assume caller wants to use supplied public key.
        */
        MEMCPY( &oECC256PubKey, f_poECC256PubKey, SIZEOF( PUBKEY_P256 ) );
    }
    else
    {
        ChkDR( DRM_XMLSIG_ExtractSimpleECC256Key( f_pszSignatureBase,
                                                  &dasstrKeyInfo,
                                                  &oECC256PubKey ) );
    }

    /*
    ** Extract <SignatureValue> node from <Signature> node.
    ** It contains an ECC-256 signature of the <SignInfo> node.
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pszSignatureBase,
                                      f_pdasstrSignature,
                                      &g_dastrSigSignatureValuePath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrSignatureValue,
                                      g_chForwardSlash ) );

    cbSize = ECDSA_P256_SIGNATURE_SIZE_IN_BYTES;

    ChkDR( DRM_B64_DecodeA( f_pszSignatureBase,
                            &dasstrSignatureValue,
                            &cbSize,
                            rgbECC256Signature,
                            0 ) );

    ChkBOOL( cbSize == ECDSA_P256_SIGNATURE_SIZE_IN_BYTES,
             DRM_E_XMLSIG_ECDSA_SIGNATURE_SIZE );

    /* Verify the ECC-256 signature of the <SignedInfo> node. */
    if ( DRM_FAILED( OEM_ECDSA_Verify_P256( ( DRM_BYTE * )( f_pszSignatureBase + __CB_DECL( dasstrSignedInfo.m_ich ) ),
                                  dasstrSignedInfo.m_cch,
                                  &oECC256PubKey,
                                  (SIGNATURE_P256*)&rgbECC256Signature,
                                  (struct bigctx_t *)&f_pCryptoCtx->rgbCryptoContext ) ) )
    {
        ChkDR( DRM_E_XMLSIG_ECDSA_VERIFY_FAILURE );
    }

    /*
    ** If the integrity of <SignedInfo> is OK, verify the SHA-256 hash of the
    ** passed in data against the SHA-1 hash stored in <DigestValue> node.
    */
    ChkDR( _VerifySHA256Hash( f_pszDataBase,
                              f_pdasstrData,
                              rgbHash ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: DRM_XMLSIG_GetXMLEncryptedDataNodeCharCount
**
** Synopsis: Function that calculates the number of characters in
**           <EncryptedData> XML segment.
**
** Arguments:
** [f_poXMLKey]             -- Pointer to a DRM_XML_KEY that contains a key
**                             used to encrypt the data.
** [f_cbData]               -- Size (number of bytes) of data to be encrypted.
**
** [f_pcchSize]             -- Pointer to a variable to receive the result
**                             of calculations, cannot be NULL.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
** Note:    The function does not encrypt anything during calculations.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMLSIG_GetEncryptedDataNodeCharCount(
    __in  const DRM_XML_KEY *f_poXMLKey,
    __in  const DRM_DWORD    f_cbData,
    __out       DRM_DWORD   *f_pcchSize )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchSize = 0;

    ChkArg( f_poXMLKey != NULL );
    ChkArg( f_poXMLKey->m_eKeyInfoType == DRM_XML_KEYINFO_ENCRYPTED );
    ChkArg( f_pcchSize != NULL );

    *f_pcchSize = 0;

    /* <EncryptedData Type="..."> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigEncryptedDataTag.cchString,
                                             0,
                                             g_dastrSigEncryptedDataAttrib1Name.cchString,
                                             g_dastrSigEncryptedDataAttrib1Value.cchString );

    cchSize += DRM_XMB_RequiredCharsForTagA( 0,
                                             0,
                                             g_dastrSigEncryptedDataAttrib2Name.cchString,
                                             g_dastrSigEncryptedDataAttrib2Value.cchString );

    /* <EncryptionMethod Algorithm="../xmlenc#aes128cbc"> */
    cchSize += DRM_XMB_RequiredCharsForTagA( g_dastrSigEncryptionMethodTag.cchString,
                                             0,
                                             g_dastrSigEncryptionMethodAttrib1Name.cchString,
                                             g_dastrSigEncryptionMethodAttrib1Value.cchString );

    /* <KeyInfo> */
    ChkDR( DRM_XMLSIG_GetEncryptedKeyInfoNodeCharCount( f_poXMLKey, f_pcchSize ) );

    /* <CipherData> */
    cchSize += _GetCipherDataNodeCharCount( f_cbData );

    *f_pcchSize += cchSize;

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_XMLSIG_BuildEncryptedDataNode
**
** Synopsis: Function that constructs an encrypted data XML segment of
**           the passed in key information and data.
**
**           The cipher data XML segment should have a root node
**           <EncryptedData>, and should have three subnodes under it:
**           <EncryptionMethod> contains the method used to encrypt the
**           data. <KeyInfo> contains information of how to find a way
**           to decrypt the data. <CipherData> contains the encrypted
**           data.
**
** Arguments:
**
** [f_poXMLContextA]        -- Pointer to ANSI XML builder context.
** [f_poXMLKey]             -- Pointer to a DRM_XML_KEY that contains a
**                             information of how to build a <KeyInfo>
**                             XML segment.
** [f_cbData]               -- Size (number of bytes) of data to be put
**                             into the <CipherValue> node. The data should
**                             have already been stored at the position
**                             pointed by the current buffer pointer in the
**                             XML builder context, and the data should have
**                             already been encrypted.
** [f_pdasstrEncryptedData] -- Pointer to a DRM substring to receive the result
**                             of the whole <EncryptedData> XML segment.
**                             The substring operates on XML builder's buffer.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_XMLSIG_BuildEncryptedDataNode(
    __inout       _XMBContextA  *f_poXMLContextA,
    __in    const DRM_XML_KEY   *f_poXMLKey,
    __in    const DRM_DWORD      f_cbData,
    __out         DRM_SUBSTRING *f_pdasstrEncryptedData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cchShift = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_XMLSIG, PERF_FUNC_DRM_XMLSIG_BuildEncryptedDataNode );

    ChkArg( f_poXMLContextA != NULL );
    ChkArg( f_poXMLKey != NULL );
    ChkArg( f_poXMLKey->m_eKeyInfoType == DRM_XML_KEYINFO_ENCRYPTED );
    /* Encrypt either using a preshared server key or a full ECC-256 public key. */
    ChkArg( f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_SERVER_PUBLIC_KEY_ID ||
            f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_METERING_PUBLIC_KEY_ID ||
            f_poXMLKey->m_eKeyFormat == DRM_XML_ECC256_PUBLIC_KEY_DATA );
    ChkArg( f_cbData > 0 );

    ChkDR( _GetEncryptedDataNodePrefixCharCount( f_poXMLKey, &cchShift ) );

    ChkDR( DRM_XMB_ShiftDataFromCurrentPositionA( f_poXMLContextA,
                                                  cchShift,
                                                  f_cbData ) );

    /*
    ** Add and open <EncryptedData> node.
    **
    ** Output:
    ** <EncryptedData ... >
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigEncryptedDataTag,
                              NULL,
                              &g_dastrSigEncryptedDataAttrib1Name,
                              &g_dastrSigEncryptedDataAttrib1Value,
                              wttOpen ) );

    ChkDR( DRM_XMB_AddAttributeA( f_poXMLContextA,
                                  &g_dastrSigEncryptedDataAttrib2Name,
                                  &g_dastrSigEncryptedDataAttrib2Value ) );

    /*
    ** Add and close <EncryptionMethod> node.
    **
    ** Output:
    ** <EncryptedData>
    **      <EncryptionMethod/>
    */
    ChkDR( DRM_XMB_WriteTagA( f_poXMLContextA,
                              &g_dastrSigEncryptionMethodTag,
                              NULL,
                              &g_dastrSigEncryptionMethodAttrib1Name,
                              &g_dastrSigEncryptionMethodAttrib1Value,
                              wttClosed ) );

    /*
    ** Build <KeyInfo> node.
    **
    ** Output:
    ** <EncryptedData>
    ** ...
    **      <KeyInfo ... >
    **      ...
    **      </KeyInfo>
    */
    ChkDR( DRM_XMLSIG_BuildEncryptedKeyInfoNode( f_poXMLContextA, f_poXMLKey ) );

    /*
    ** Build <CipherData> node.
    **
    ** Output:
    ** <EncryptedDataData>
    ** ...
    **      <CipherData>...</CipherData>
    */
    ChkDR( _BuildCipherDataNode( f_poXMLContextA, NULL, f_cbData ) );

    /*
    ** Close <EncryptedData> node.
    **
    ** Output:
    ** <EncryptedData>
    ** ...
    ** </EncryptedData>
    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_poXMLContextA, f_pdasstrEncryptedData ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
