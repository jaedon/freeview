/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmxmlbuilder.h>
#include <drmxmlbuildera.h>
#include <drmxmlrsakeys.h>
#include <oemrsa.h>
#include <drmndcertbuilder.h>
#include <drmxmlsigconstants.h>
#include <drmwmdrmnet.h>
#include <drmndcertconstants.h>

ENTER_PK_NAMESPACE_CODE;

/******************************************************************************
**
** Function :   _AddDSigPublicKeyNode
**
** Synopsis :   Write a <c:PublicKey> node with the ds: namespace prefix
**              to ASCII XML context
**
** Arguments :  [f_pXMLCtxA]      - pointer to ASCII XML context
**              [f_pKeyRSAPublic] - pointer to public key data structure
**
** Returns :    DRM_SUCCESS - if public key node was added successfully
**
** Notes:       Format:
**                   <c:PublicKey>
**                      <ds:KeyValue>
**                          <ds:RSAKeyValue>
**                              <ds:Modulus> ... </ds:Modulus>
**                              <ds:Exponent> ... </ds:Exponent>
**                          </ds:RSAKeyValue>
**                      </ds:KeyValue>
**                   </c:PublicKey>
******************************************************************************/
static DRM_RESULT _AddDSigPublicKeyNode(
   __inout       _XMBContextA       *f_pXMLCtxA,
   __in    const DRM_RSA_PUBLIC_KEY *f_pKeyRSAPublic )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_BYTE   rgbModulus[DRM_RSA_CB_MODULUS_MAX] = { 0 };
    DRM_DWORD  cbModulus                          = SIZEOF( rgbModulus );
    DRM_BYTE   rgbExponent[DRM_RSA_CB_PUBEXP]     = { 0 };
    DRM_DWORD  cbExponent                         = SIZEOF( rgbExponent );
    DRM_CHAR   rgbWorkBuffer[CCH_BASE64_EQUIV(DRM_RSA_CB_MODULUS_MAX)] = { 0 };

    DRM_ANSI_CONST_STRING         dastrModulus   = EMPTY_DRM_STRING;
    DRM_SUBSTRING                 dasstrModulus  = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING                 dasstrExponent = EMPTY_DRM_SUBSTRING;
    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength     = eDRM_RSA_UNKNOWN;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC__AddDSigPublicKeyNode );

    DRMASSERT( f_pXMLCtxA != NULL );
    DRMASSERT( f_pKeyRSAPublic != NULL );

    dastrModulus.cchString = CCH_BASE64_EQUIV(DRM_RSA_CB_MODULUS_MAX);
    dastrModulus.pszString = &rgbWorkBuffer[0];

    ChkDR( OEM_RSA_ParsePublicKey(  f_pKeyRSAPublic,
                                   &eKeyLength,
                                   &cbExponent,
                                    rgbExponent,
                                   &cbModulus,
                                    rgbModulus ) );

    ChkArg( SIZEOF( rgbModulus ) >= cbModulus );

    ChkDR( DRM_B64_EncodeA( rgbModulus,
                            cbModulus,
                            (DRM_CHAR *) dastrModulus.pszString,
                            &dastrModulus.cchString,
                            0 ) );

    dasstrModulus.m_ich = 0;
    dasstrModulus.m_cch = dastrModulus.cchString;

    dasstrExponent.m_ich = 0;
    dasstrExponent.m_cch = g_dastrWMDRMCertExponent.cchString;

    ChkDR( DRM_XMB_WriteTagA( f_pXMLCtxA, &g_dastrTagWMDRMCertPublicKey, NULL, NULL, NULL, wttOpen ) );
    ChkDR( DRM_XMB_WriteTagA( f_pXMLCtxA, &g_dastrTagKeyValue,           NULL, NULL, NULL, wttOpen ) );
    ChkDR( DRM_XMB_WriteTagA( f_pXMLCtxA, &g_dastrTagRSAKeyValue,        NULL, NULL, NULL, wttOpen ) );

    ChkDR( DRM_XMB_WriteTagA( f_pXMLCtxA, &g_dastrTagModulus,  &dastrModulus,             NULL, NULL, wttClosed ) );
    ChkDR( DRM_XMB_WriteTagA( f_pXMLCtxA, &g_dastrTagExponent, &g_dastrWMDRMCertExponent, NULL, NULL, wttClosed ) );

    ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLCtxA, NULL ) );
    ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLCtxA, NULL ) );
    ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLCtxA, NULL ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   DRM_ND_CB_CreateDigitalSignature
**
** Synopsis:    Create an XML digital signature in either PlayReady-ND or standard
**              format.  This call can only be used in an XML construction
**              already in progress using the DRM_XMB API and it must be
**              called while the current insertion point is at peer level with
**              the <Data> node
**
** Arguments :  [f_pXMLContext]                - the XML context currently in use
**              [f_pdastrRSAPubKeyModulusB64]  - pointer to signing base64 encoded RSA public key modulus
**              [f_pdastrRSAPubKeyExponentB64] - pointer to signing base64 encoded RSA public key exponent
**              [f_pRSAPrivKey]                - pointer to signing RSA private key
**              [f_cchSHA]                     - size of SHA hash
**              [f_pchSHA]                     - the base64 encoded unkeyed SHA hash of the <Data>
**                                               node of the XML entity
**              [f_cchRSA]                     - size of RSA signature
**              [f_pchRSA]                     - an RSA signature of the <Data> node;
**                                               this should be NULL if the f_fWMDRMNET
**                                               parameter is DRM_DSIG_TYPE_STANDARD
**              [f_dwSignType]                 - specifies which type of signature to
**                                               write, either PlayReady-ND or standard.
**                                               Use the manifest constants
**                                               DRM_DSIG_TYPE_STANDARD and
**                                               DRM_DSIG_TYPE_WMDRMNET
**              [f_dwFlags]                    - indicates if signature is provided by caller
**                                               Use constant DRM_CDS_USE_SIGNATURE_PARAMETER
**              [f_pCryptoCtx]                 - pointer to crypto context
**
** Returns :    DRM_SUCCESS if digital signature was created successfully
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_CreateDigitalSignature(
    __inout                           _XMBContextA        *f_pXMLContext,
    __in                        const DRM_ANSI_CONST_STRING *f_pdastrRSAPubKeyModulusB64,
    __in                        const DRM_ANSI_CONST_STRING *f_pdastrRSAPubKeyExponentB64,
    __in                        const DRM_RSA_PRIVATE_KEY   *f_pRSAPrivKey,
    __in                        const DRM_DWORD              f_cchSHA,
    __in_ecount( f_cchSHA )     const DRM_CHAR              *f_pchSHA,
    __in                        const DRM_DWORD              f_cchRSA,
    __in_ecount_opt( f_cchRSA ) const DRM_CHAR              *f_pchRSA,
    __in                              DRM_DWORD              f_dwSignType,
    __in                              DRM_DWORD              f_dwFlags,
    __in                              DRM_CRYPTO_CONTEXT    *f_pCryptoCtx )
{
    DRM_RESULT    dr             = DRM_SUCCESS;

    DRM_RSA_SUPPORTED_KEY_LENGTHS eKeyLength = eDRM_RSA_UNKNOWN;
    DRM_ANSI_CONST_STRING         dastr      = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_ND_CB_CreateDigitalSignature );

    ChkArg( f_pXMLContext != NULL );
    ChkArg( f_pCryptoCtx  != NULL );
    ChkDRMANSIString( f_pdastrRSAPubKeyModulusB64 );
    ChkDRMANSIString( f_pdastrRSAPubKeyExponentB64 );
    ChkArg( f_pRSAPrivKey != NULL );
    ChkArg( f_pchSHA      != NULL );

    ChkBOOL( f_cchSHA >= DRM_SHA1_DIGEST_LEN, DRM_E_BUFFERTOOSMALL );

    /* if the signature is to be provided by the caller make sure it has been passed */
    if ( ( f_dwFlags & DRM_CDS_USE_SIGNATURE_PARAMETER ) == 0 )
    {
        ChkArg( ( f_dwSignType == DRM_DSIG_TYPE_WMDRMNET
               && f_pchRSA    != NULL)
             || ( f_dwSignType == DRM_DSIG_TYPE_STANDARD
               && f_pchRSA    == NULL ) );
    }
    else
    {
        /* we're generating the signature; make sure the buffer is large enough. */
        ChkArg( f_pchRSA != NULL );
        ChkBOOL( f_cchRSA >= CCH_SIGNATURE_RSA( (DRM_DWORD)eKeyLength ), DRM_E_BUFFERTOOSMALL );
    }

    ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagSignature,        NULL, &g_dastrSigRootAttribName,  &g_dastrSigRootAttribValue, wttOpen ) );
    ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagSignedInfo,       NULL,  NULL,                       NULL,              wttOpen ) );
    ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagCanonicalization, NULL, &g_dastrAttributeAlgorithm, &g_dastrURIC14N,    wttClosed ) );

    if ( f_dwSignType == DRM_DSIG_TYPE_WMDRMNET )
    {
        ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagSignatureMethod,  NULL, &g_dastrAttributeAlgorithm, &g_dastrURIRSASHA1_Old, wttClosed ) );
    }
    else
    {
        ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagSignatureMethod,  NULL, &g_dastrAttributeAlgorithm, &g_dastrURIRSASHA1, wttClosed ) );
    }

    /*      <Reference>
    **          <Transforms>
    **              <Transform Algorithm="http://www.microsoft.com/DRM/CERT/v2/Data"/>
    **              <Transform Algorithm="http://www.w3.org/TR/2001/REC-xml-c14n-20010315"/>
    **          </Transforms>
    **          <DigestMethod Algorithm=" http://www.w3.org/2000/09/xmldsig#sha1"/>
    **          <DigestValue>izldaYbmY8ziCfrL980l4VOIHmo=</DigestValue>
    **      </Reference>
    */

    ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagReference,  NULL,  NULL, NULL, wttOpen ) );
    ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagTransforms, NULL,  NULL, NULL, wttOpen ) );

    if ( f_dwSignType == DRM_DSIG_TYPE_WMDRMNET )
    {
        ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagTransform, NULL, &g_dastrAttributeAlgorithm, &g_dastrURITransformMSCert_Old, wttClosed ) );
    }
    else
    {
        ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagTransform, NULL, &g_dastrAttributeAlgorithm, &g_dastrURITransformMSCert, wttClosed ) );
    }

    ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagTransform, NULL, &g_dastrAttributeAlgorithm, &g_dastrURITransformC14N, wttClosed ) );
    ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLContext, NULL ) ); /*       </Transforms> */

    ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagDigestMethod, NULL, &g_dastrAttributeAlgorithm, &g_dastrURIDSigSHA1, wttClosed ) );

    dastr.pszString = f_pchSHA;
    dastr.cchString = CCH_BASE64_EQUIV( DRM_SHA1_DIGEST_LEN );

    ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagDigestValue, &dastr, NULL, NULL, wttClosed ) );

    ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLContext, NULL ) ); /*     </Reference> */

    /* close the <SignedInfo> node and sign it with the RSA private key; write the signature of as a peer node to <SignedInfo> */
    if ( f_dwSignType == DRM_DSIG_TYPE_WMDRMNET )
    {
        /* Cardea-style signing: use a caller-provided signature of the data section */
        ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLContext, NULL)); /*     </SignedInfo> */

        /* <SignatureValue> */
        dastr.pszString = f_pchRSA;
        dastr.cchString = f_cchRSA;

        ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagSignatureValue, &dastr, NULL, NULL, wttClosed ) );
    }
    else
    {
        if ( ( f_dwFlags & DRM_CDS_USE_SIGNATURE_PARAMETER ) == 0 )
        {
            DRM_DWORD     cbSignature    = CB_SIGNATURE_RSA( eDRM_RSA_MAXIMUM_KEY_LENGTH );
            DRM_BYTE      rgbSignature  [CB_SIGNATURE_RSA( eDRM_RSA_MAXIMUM_KEY_LENGTH )]  = { 0 };
            DRM_CHAR      rgchSignature [CCH_SIGNATURE_RSA( eDRM_RSA_MAXIMUM_KEY_LENGTH )] = { 0 };

            ChkBOOL( DRM_WMDRMNET_IsWmdrmnetSupported(), DRM_E_NOTIMPL );

            /* standard signing with an available private key */
            dastr.pszString = &rgchSignature[0];
            dastr.cchString = CCH_SIGNATURE_RSA( eKeyLength );

            ChkDR( DRM_XMB_RSASignAndCloseCurrNodeA( f_pXMLContext,
                                                     f_pRSAPrivKey,
                                                     TRUE,
                                                    &cbSignature,
                                                     &rgbSignature[0],
                                                     &rgchSignature[0],
                                                    &dastr.cchString,
                                                     NULL,
                                                     f_pCryptoCtx ) );

            /* <SignatureValue> */
            ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagSignatureValue, &dastr, NULL, NULL, wttClosed ) );
        }
        else
        {
            /* standard signing with a provided signature: exported from a key-management system e.g. the blackbox */
            dastr.pszString = f_pchRSA;
            dastr.cchString = f_cchRSA;

            ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLContext, NULL ) ); /*     </SignedInfo> */

            /* <SignatureValue> */
            ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagSignatureValue, &dastr, NULL, NULL, wttClosed ) );
        }
    }

    /*  <KeyInfo>
    **      <KeyValue>
    **          <RSAKeyValue>
    **              <Modulus>x6O/cYFbvAQe ? Vz7w==</Modulus>
    **              <Exponent>AQAB</Exponent>
    **          </RSAKeyValue>
    **      </KeyValue>
    **  </KeyInfo>
    */
    ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagKeyInfo,     NULL, NULL, NULL, wttOpen ) );
    ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagKeyValue,    NULL, NULL, NULL, wttOpen ) );
    ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagRSAKeyValue, NULL, NULL, NULL, wttOpen ) );

    ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagModulus, f_pdastrRSAPubKeyModulusB64,  NULL, NULL, wttClosed ) );

    ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagExponent, f_pdastrRSAPubKeyExponentB64, NULL, NULL, wttClosed ) );

    ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLContext, NULL ) ); /*       </RSAKeyValue> */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLContext, NULL ) ); /*       </KeyValue>    */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLContext, NULL ) ); /*     </KeyInfo>       */

    ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLContext, NULL ) ); /*   </Signature> */

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   DRM_ND_CB_CreateCertificateInit
**
** Synopsis :   This method starts creation of ND certificated certificate.
**              It does not add template ( like manufacturer's and custom ) data
**              and does not sign certificate.
**              Appropriate methods: DRM_ND_CB_CreateCertificateAddTemplateData
**              and DRM_ND_CB_CreateCertificateSignAndClose must be called to complete
**              certificate creation.
**
** Arguments :  [f_pXMLContextA]           - pointer to ASCII XML context in use
**              [f_eCertType]              - type of certificate
**              [f_pKeyPublic]             - pointer to client's RSA key
**              [f_pCryptoCtx]             - pointer to crypto context
**
** Returns :    DRM_SUCCESS if certificate was created successfully
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_CreateCertificateInit(
  __inout      _XMBContextA               *f_pXMLContextA,
  __in    enum DRM_CERTIFICATE_TYPE        f_eCertType,
  __in         DRM_RSA_PUBLIC_KEY         *f_pKeyPublic,
  __in         DRM_CRYPTO_CONTEXT         *f_pCryptoCtx  )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_ND_CB_CreateCertificateInit );

    /* argument checks */
    ChkArg( f_pXMLContextA != NULL );
    ChkArg( f_pKeyPublic != NULL );
    ChkArg( f_pCryptoCtx != NULL );

    ChkArg( f_eCertType == certtypeRoot
         || f_eCertType == certtypeCertAuthority
         || f_eCertType == certtypeBlackBox
         || f_eCertType == certtypeBlackBoxVersion
         || f_eCertType == certtypeMachine
         || f_eCertType == certtypeIndivBox
         || f_eCertType == certtypeExport
         || f_eCertType == certtypeExportIncl
         || f_eCertType == certtypeDevice
         || f_eCertType == certtypeSampleProtection );

    /* <Certificate c:Version="2.0" xmlns:c=http://www.microsoft.com/DRM/2004/02/cert type="Machine"> */
    ChkDR( DRM_XMB_WriteTagA( f_pXMLContextA, &g_dastrTagWMDRMCertificate, NULL, NULL, NULL, wttOpen ) );

    ChkDR( DRM_XMB_AddAttributeA( f_pXMLContextA, &g_dastrAttributeVersionWMDRM, &g_dastrVersionWMDRM ) );
    ChkDR( DRM_XMB_AddAttributeA( f_pXMLContextA, &g_dastrPrefixMicrosoftCert,   &g_dastrURITransformMSCertColl ) );

    /* Open the <Data> tag and add the namespaces;
       <Data
        xmlns:c=http://www.microsoft.com/DRM/2004/02/cert
        xmlns:m=http://www.microsoft.com/DRM/2004/02/mslp>
    */
    ChkDR( DRM_XMB_WriteTagA    ( f_pXMLContextA, &g_dastrTagWMDRMData, NULL, NULL, NULL, wttOpen ) );
    ChkDR( DRM_XMB_AddAttributeA( f_pXMLContextA, &g_dastrPrefixMicrosoftCert, &g_dastrURITransformMSCertColl ) );
    ChkDR( DRM_XMB_AddAttributeA( f_pXMLContextA, &g_dastrRSAPrefixMSLP, &g_dastrRSAURIMSLP ) );

    /* <SecurityVersion> */
    if ( f_eCertType == certtypeExport
      || f_eCertType == certtypeExportIncl
      || f_eCertType == certtypeBlackBoxVersion
      || f_eCertType == certtypeIndivBox )
    {
        ChkDR( DRM_XMB_WriteTagA( f_pXMLContextA, &g_dastrTagWMDRMCertSecurityVersion, &g_dastrSecurityVersion, NULL,  NULL, wttClosed ) );
    }


    /* add the <PublicKey> node */
    ChkDR( _AddDSigPublicKeyNode( f_pXMLContextA, f_pKeyPublic ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   DRM_ND_CB_CreateCertificateAddTemplateData
**
** Synopsis :   This optional method adds template data to certificate, like
**              manufacturer's and customer data
**
** Arguments :  [f_pXMLContextA]           - pointer to ASCII XML context in use
**              [f_pdastrTemplate]         - pointer to xml tamplate that contains
**
** Returns :    DRM_SUCCESS if certificate was created successfully
**
** Note :       This method must be called after call of DRM_ND_CB_CreateCertificateInit
**              method
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_CreateCertificateAddTemplateData(
  __inout      _XMBContextA               *f_pXMLContextA,
  __in         DRM_ANSI_CONST_STRING      *f_pdastrTemplate )
{
    DRM_RESULT            dr                = DRM_SUCCESS;
    DRM_SUBSTRING         dasstrData        = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrTemplate    = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING dastrTemplateData = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_ND_CB_CreateCertificateAddTemplateData );

    ChkArg( f_pdastrTemplate != NULL );

    dasstrTemplate.m_cch = f_pdastrTemplate->cchString;

    /* Get information from unsigned template */
    ChkDR( DRM_XML_GetNodeA( f_pdastrTemplate->pszString,
                             &dasstrTemplate,
                             &g_dastrTagUnsignedTemplate,
                             NULL,
                             NULL,
                             0,
                             NULL,
                             &dasstrData ) );
    dastrTemplateData.pszString = f_pdastrTemplate->pszString + dasstrData.m_ich;
    dastrTemplateData.cchString = dasstrData.m_cch;
    ChkDR( DRM_XMB_AppendNodeA( f_pXMLContextA, &dastrTemplateData ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   DRM_ND_CB_CreateCertificateSignAndClose
**
** Synopsis :   Final Step for creation of ND certificate - sign the certificate and
**              close </c:Certificate> node
**
** Arguments :  [f_pXMLContextA]               - pointer to ASCII XML context in use
**              [f_pdastrRSAPubKeyModulusB64]  - pointer to signing base64 encoded RSA public key modulus
**              [f_pdastrRSAPubKeyExponentB64] - pointer to signing base64 encoded RSA public key exponent
**              [f_pRSAPrivKey]                - pointer to signing RSA private key
**              [f_pCryptoCtx]                 - pointer to crypto context
**
** Returns :    DRM_SUCCESS if certificate was created successfully
**
** Note :       This method must be called after call of DRM_ND_CB_CreateCertificateInit
**              method
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_CreateCertificateSignAndClose(
  __inout       _XMBContextA          *f_pXMLContextA,
  __in    const DRM_ANSI_CONST_STRING *f_pdastrRSAPubKeyModulusB64,
  __in    const DRM_ANSI_CONST_STRING *f_pdastrRSAPubKeyExponentB64,
  __in    const DRM_RSA_PRIVATE_KEY   *f_pRSAPrivKey,
  __in          DRM_CRYPTO_CONTEXT    *f_pCryptoCtx  )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_ND_CB_CreateCertificateSignAndClose );

    ChkArg( f_pXMLContextA != NULL );
    ChkDRMANSIString( f_pdastrRSAPubKeyModulusB64 );
    ChkDRMANSIString( f_pdastrRSAPubKeyExponentB64 );
    ChkArg( f_pRSAPrivKey != NULL );
    ChkArg( f_pCryptoCtx != NULL );

    /* </c:Data> tag  will be closed at signing */
    ChkDR( DRM_ND_CB_SignCertificate( f_pXMLContextA,
                                      TRUE,
                                      f_pdastrRSAPubKeyModulusB64,
                                      f_pdastrRSAPubKeyExponentB64,
                                      f_pRSAPrivKey,
                                      f_pCryptoCtx ) );

    ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLContextA, NULL ) ); /* </c:Certificate> */

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/******************************************************************************
**
** Function :   DRM_ND_CB_CreateCertificate
**
** Synopsis :   Created XML section for certificate, with specified attribute set
**              and manufacturer's data
**
** Arguments :  [f_pXMLContextA]               - pointer to ASCII XML context in use
**              [f_eCertType]                  - type of certificate
**              [f_pKeyPublic]                 - pointer to client's RSA key
**              [f_pdastrRSAPubKeyModulusB64]  - pointer to signing base64 encoded RSA public key modulus
**              [f_pdastrRSAPubKeyExponentB64] - pointer to signing base64 encoded RSA public key exponent
**              [f_pRSAPrivKey]                - pointer to signing RSA private key
**              [f_pdastrTemplate]             - pointer to xml tamplate that contains
**                                               data for <data> section of certificate
**              [f_pCryptoCtx]                 - pointer to crypto context
**
** Returns :    DRM_SUCCESS if certificate was created successfully
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_CreateCertificate(
  __inout       _XMBContextA          *f_pXMLContextA,
  __in    enum  DRM_CERTIFICATE_TYPE   f_eCertType,
  __in          DRM_RSA_PUBLIC_KEY    *f_pKeyPublic,
  __in    const DRM_ANSI_CONST_STRING *f_pdastrRSAPubKeyModulusB64,
  __in    const DRM_ANSI_CONST_STRING *f_pdastrRSAPubKeyExponentB64,
  __in    const DRM_RSA_PRIVATE_KEY   *f_pRSAPrivKey,
  __in          DRM_ANSI_CONST_STRING *f_pdastrTemplate,
  __in          DRM_CRYPTO_CONTEXT    *f_pCryptoCtx  )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_ND_CB_CreateCertificate );

    ChkArg( f_pXMLContextA != NULL );
    ChkArg( f_pKeyPublic != NULL );
    ChkDRMANSIString( f_pdastrRSAPubKeyModulusB64 );
    ChkDRMANSIString( f_pdastrRSAPubKeyExponentB64 );
    ChkArg( f_pRSAPrivKey != NULL );
    ChkArg( f_pdastrTemplate != NULL );
    ChkArg( f_pCryptoCtx != NULL );

    ChkDR( DRM_ND_CB_CreateCertificateInit( f_pXMLContextA,
                                            f_eCertType,
                                            f_pKeyPublic,
                                            f_pCryptoCtx ) );

    ChkDR( DRM_ND_CB_CreateCertificateAddTemplateData( f_pXMLContextA,
                                                       f_pdastrTemplate ) );

    ChkDR( DRM_ND_CB_CreateCertificateSignAndClose( f_pXMLContextA,
                                                    f_pdastrRSAPubKeyModulusB64,
                                                    f_pdastrRSAPubKeyExponentB64,
                                                    f_pRSAPrivKey,
                                                    f_pCryptoCtx ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   DRM_ND_CB_AddAttribute
**
** Synopsis :   Adds certificate attribute to ASCII XML context
**
** Arguments :  [f_pXMLContext]    - pointer to ASCII XML context in use
**              [f_eCertAttribute] - type of attribute to add
**              [f_rgpdastrValue]  - attribute list of values
**              [f_cEntries]       - number of attribute values
**
** Returns :    DRM_SUCCESS if attribute was added successfully
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_AddAttribute(
  __inout                      _XMBContextA             *f_pXMLContext,
  __in                         DRM_WMDRM_CERT_ATTRIBUTE  f_eCertAttribute,
  __in_ecount(f_cEntries)const DRM_ANSI_CONST_STRING    *f_rgpdastrValue     [],
  __in                         DRM_DWORD                 f_cEntries )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  iEntry = 0;
    const DRM_ANSI_CONST_STRING *pdastrTagValue  = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_ND_CB_AddAttribute );

    ChkArg( f_pXMLContext != NULL );
    ChkArg( f_cEntries > 0 );
    ChkArg( f_rgpdastrValue != NULL);

    /* perform final arg checks and match implicit tags */
    switch( f_eCertAttribute )
    {
    case DRM_WMDRM_CERT_ATTR_SECURITY_LEVEL:
        ChkArg( f_cEntries == 1 );
        ChkArg( f_rgpdastrValue != NULL );

        pdastrTagValue = &g_dastrTagWMDRMCertSecurityLevel;
        break;

    case DRM_WMDRM_CERT_ATTR_KEY_USAGE:
        ChkArg( f_cEntries >= 1 );
        ChkArg( f_rgpdastrValue != NULL );
        break;

    case DRM_WMDRM_CERT_ATTR_FEATURES:
        ChkArg( f_cEntries >= 1 );
        ChkArg( f_rgpdastrValue != NULL );

        pdastrTagValue = &g_dastrTagWMDRMCertFeatures;
        break;

    default:
        ChkDR( DRM_E_INVALIDARG );
    }

    if ( f_eCertAttribute == DRM_WMDRM_CERT_ATTR_KEY_USAGE )
    {
        /*
        ** <c:KeyUsage>
        **    <c:SignCertificate>1</c:SignCertificate>
        **    <c:SignCRL>1</c:SignCRL>
        **    <c:EncryptKey>1</c:EncryptKey>
        ** </c:KeyUsage>
        */
        ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagWMDRMCertKeyUsage, NULL, NULL, NULL, wttOpen));

        for (iEntry = 0; iEntry < f_cEntries; iEntry++)
        {
            ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, f_rgpdastrValue [iEntry], &g_dastrOne, NULL, NULL, wttClosed ) );
        }

        ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLContext, NULL ) );
    }
    else if ( f_eCertAttribute == DRM_WMDRM_CERT_ATTR_FEATURES )
    {
        /*
        ** <c:Features>
        **    <m:WMDRMTransmitter>1</m:WMDRMTransmitter>
        ** </c:Features>
        */
        ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagWMDRMCertFeatures, NULL, NULL, NULL, wttOpen ) );

        for ( iEntry = 0; iEntry < f_cEntries; iEntry++ )
        {
            ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, f_rgpdastrValue [iEntry], &g_dastrOne, NULL, NULL, wttClosed ) );
        }

        ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLContext, NULL ) );
    }
    else
    {
        for (iEntry = 0; iEntry < f_cEntries; iEntry++)
        {
            ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, pdastrTagValue, f_rgpdastrValue [iEntry], NULL, NULL, wttOpen ) );
        }

        ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLContext, NULL ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   DRM_ND_CB_AddManufacturerNodes
**
** Synopsis :   Adds manufacturer data to ASCII XML context
**
** Arguments :  [f_pXMLContext]       - pointer to ASCII XML context in use
**              [f_pdastrNamespace]   - XML namespace prefix
**              [f_rgpdastrTag]       - pointer to list of tags
**              [f_rgpdastrAttrLabel] - pointer to optional list of labels
**              [f_rgpdastrAttrValue] - pointer to optional list of values
**              [f_rgpdastrValue]     - pointer to list of values
**              [f_cEntries]          - number of entries
**
** Returns :    DRM_SUCCESS if manufacturer's nodes were successfully added
**              to XML context
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_AddManufacturerNodes(
  __inout                           _XMBContextA           *f_pXMLContext,
  __in_opt                    const DRM_ANSI_CONST_STRING  *f_pdastrNamespace,
  __in_ecount(f_cEntries)     const DRM_ANSI_CONST_STRING **f_rgpdastrTag,
  __in_ecount_opt(f_cEntries) const DRM_ANSI_CONST_STRING **f_rgpdastrAttrLabel,
  __in_ecount_opt(f_cEntries) const DRM_ANSI_CONST_STRING **f_rgpdastrAttrValue,
  __in_ecount(f_cEntries)     const DRM_ANSI_CONST_STRING **f_rgpdastrValue,
  __in                        const DRM_DWORD               f_cEntries )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  iEntry    = 0;
    DRM_ANSI_CONST_STRING dastrManufacturerBare =
    {
        "ManufacturerName",
        16
    };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_ND_CB_AddManufacturerNodes );

    ChkArg( f_pXMLContext  != NULL );

    ChkArg( f_rgpdastrTag != NULL );
    ChkArg( f_rgpdastrValue != NULL );
    ChkArg( f_cEntries > 0 );

    /* first tag must be <ManufacturerName> or <c:ManufacturerName> */
    ChkArg( DRM_UTL_DASTRStringsEqual( f_rgpdastrTag [0], &dastrManufacturerBare )
         || DRM_UTL_DASTRStringsEqual( f_rgpdastrTag [0], &g_dastrTagManufacturerName ) );

    /* <ManufacturerData> */
    if ( f_pdastrNamespace != NULL )
    {
        ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagManufacturerData, NULL, &g_dastrPrefixManufacturer, f_pdastrNamespace, wttOpen ) );
    }
    else
    {
        ChkDR( DRM_XMB_WriteTagA( f_pXMLContext, &g_dastrTagManufacturerData, NULL,  NULL, NULL, wttOpen ) );
    }

    for ( iEntry = 0; iEntry < f_cEntries; iEntry++ )
    {
        if ( f_rgpdastrAttrLabel != NULL
          && f_rgpdastrAttrValue != NULL )
        {
            ChkArg( ( f_rgpdastrAttrLabel [iEntry] != NULL
                   && f_rgpdastrAttrValue [iEntry] != NULL )
                   || ( f_rgpdastrAttrLabel [iEntry] == NULL
                   && f_rgpdastrAttrValue [iEntry] == NULL ) );
        }

        ChkDR( DRM_XMB_WriteTagA( f_pXMLContext,
                                  iEntry == 0 ? &g_dastrTagManufacturerName : f_rgpdastrTag [iEntry],
                                  f_rgpdastrValue[iEntry],
                                  ((f_rgpdastrAttrLabel == NULL) ? NULL : f_rgpdastrAttrLabel [iEntry]),
                                  ((f_rgpdastrAttrValue == NULL) ? NULL : f_rgpdastrAttrValue [iEntry]),
                                  wttClosed));
    }

    /* </ManufacturerData> */
    ChkDR( DRM_XMB_CloseCurrNodeA( f_pXMLContext, NULL ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   DRM_ND_CB_SignCertificate
**
** Synopsis :   Signs certificate and adds signature section to XML context
**
** Arguments :  [f_pXMLContext] - pointer to ASCII XML context in use
**              [f_fWMDRMNET]   -
**              [f_pdastrRSAPubKeyModulusB64]  - pointer to signing base64 encoded RSA public key modulus
**              [f_pdastrRSAPubKeyExponentB64] - pointer to signing base64 encoded RSA public key exponent
**              [f_pRSAPrivKey]                - pointer to signing RSA private key
**              [f_pCryptoCtx]                 - pointer to crypto context
**
** Returns :    DRM_SUCCESS if certificated was signed successfully
**
** Note:
**    <Signature xmlns="http://www.w3.org/2000/09/xmldsig#">
**        <SignedInfo>
**            <CanonicalizationMethod Algorithm="http://www.w3.org/TR/2001/REC-xml-c14n-20010315"/>
**            <SignatureMethod Algorithm=" http://www.w3.org/2000/09/xmldsig#rsa-sha1"/>
**            <Reference>
**                <Transforms>
**                    <Transform Algorithm="http://www.microsoft.com/DRM/CERT/v2/Data"/>
**                    <Transform Algorithm="http://www.w3.org/TR/2001/REC-xml-c14n-20010315"/>
**                </Transforms>
**                <DigestMethod Algorithm=" http://www.w3.org/2000/09/xmldsig#sha1"/>
**                <DigestValue>izldaYbmY8ziCfrL980l4VOIHmo=</DigestValue>
**            </Reference>
**        </SignedInfo>
**        <SignatureValue>jFfZ1RHu1eJ ? +Bq2g==</SignatureValue>
**        <KeyInfo>
**            <KeyValue>
**                <RSAKeyValue>
**                    <Modulus>x6O/cYFbvAQe ? Vz7w==</Modulus>
**                    <Exponent>AQAB</Exponent>
**                </RSAKeyValue>
**            </KeyValue>
**        </KeyInfo>
**    </Signature>
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_SignCertificate(
    __inout       _XMBContextA          *f_pXMLContext,
    __in          DRM_BOOL               f_fWMDRMNET,
    __in    const DRM_ANSI_CONST_STRING *f_pdastrRSAPubKeyModulusB64,
    __in    const DRM_ANSI_CONST_STRING *f_pdastrRSAPubKeyExponentB64,
    __in    const DRM_RSA_PRIVATE_KEY   *f_pRSAPrivKey,
    __in          DRM_CRYPTO_CONTEXT    *f_pCryptoCtx  )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_BYTE   rgbSHA  [__CB_DECL(DRM_SHA1_DIGEST_LEN)]        = { 0 };
    DRM_CHAR   rgchSHA [__CB_DECL(CCH_BASE64_EQUIV(DRM_SHA1_DIGEST_LEN))] = { 0 };
    DRM_DWORD  cchSHA = SIZEOF( rgchSHA );
    DRM_SHA1_CONTEXT    contextSHA   = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_ND_CB_SignCertificate );

    ChkArg( f_pXMLContext != NULL );
    ChkArg( f_pCryptoCtx != NULL );
    ChkDRMANSIString( f_pdastrRSAPubKeyModulusB64 );
    ChkDRMANSIString( f_pdastrRSAPubKeyExponentB64 );
    ChkArg( f_pRSAPrivKey != NULL );

    if ( f_fWMDRMNET == DRM_DSIG_TYPE_WMDRMNET )
    {
        DRM_BYTE   rgbRSA  [__CB_DECL(CB_SIGNATURE_RSA( eDRM_RSA_MAXIMUM_KEY_LENGTH ))]      = { 0 };
        DRM_DWORD  cbRSA = SIZEOF( rgbRSA );
        DRM_CHAR   rgchRSA [__CB_DECL(CCH_SIGNATURE_RSA( eDRM_RSA_MAXIMUM_KEY_LENGTH ))]     = { 0 };
        DRM_DWORD  cchRSA = SIZEOF( rgchRSA );

        ChkBOOL( DRM_WMDRMNET_IsWmdrmnetSupported(), DRM_E_NOTIMPL );

        ChkDR( DRM_XMB_HashAndRSASignAndCloseCurrNodeA( f_pXMLContext, /* </Data> */
                                                        &contextSHA,
                                                        f_pRSAPrivKey,
                                                        TRUE,
                                                        rgbSHA,
                                                        rgchSHA,
                                                        &cbRSA,
                                                        rgbRSA,
                                                        &cchRSA,
                                                        rgchRSA,
                                                        NULL,
                                                        f_pCryptoCtx ) );

        /* write a WMDRMNET-style XML DSIG node using the RSA signature of the Data node */
        ChkDR( DRM_ND_CB_CreateDigitalSignature( f_pXMLContext,
                                                 f_pdastrRSAPubKeyModulusB64,
                                                 f_pdastrRSAPubKeyExponentB64,
                                                 f_pRSAPrivKey,
                                                 cchSHA,
                                                 rgchSHA,
                                                 cchRSA,
                                                 rgchRSA,
                                                 DRM_DSIG_TYPE_WMDRMNET,
                                                 0,
                                                 f_pCryptoCtx ) );
    }
    else
    {
        ChkDR( DRM_XMB_HashAndCloseCurrNodeA( f_pXMLContext, /* </Data> */
                                              &contextSHA,
                                              TRUE,
                                              rgbSHA,
                                              rgchSHA,
                                              NULL ) );

        /* write a standard W3C XML DSIG node */
        ChkDR( DRM_ND_CB_CreateDigitalSignature( f_pXMLContext,
                                                 f_pdastrRSAPubKeyModulusB64,
                                                 f_pdastrRSAPubKeyExponentB64,
                                                 f_pRSAPrivKey,
                                                 cchSHA,
                                                 rgchSHA,
                                                 0,
                                                 NULL,
                                                 DRM_DSIG_TYPE_STANDARD,
                                                 0,
                                                 f_pCryptoCtx ) );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/******************************************************************************
**
** Function :   DRM_ND_CB_CreateCertificateFile
**
** Synopsis :   Generate PlayReady-ND certificate collection
**
** Arguments :  [f_pXMLContext]                - pointer to ASCII XML context in use
**              [f_oCertType]                  - type of certificate to create
**              [f_pKeyPublic]                 - pointer manufacturer public key structure
**              [f_pdastrRSAPubKeyModulusB64]  - pointer to signing base64 encoded RSA public key modulus
**              [f_pdastrRSAPubKeyExponentB64] - pointer to signing base64 encoded RSA public key exponent
**              [f_pRSAPrivKey]                - pointer to signing RSA private key
**              [f_pdastrTemplate]             - template that contains xml data for <data> section
**              [f_rgdastrCerts]               - pointer to certificate collection
**              [f_pCryptoCtx]                 - pointer to crypto context
**
** Returns :    DRM_SUCCESS if certificate collection was created succefffully
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_CreateCertificateFile(
 __inout       _XMBContextA          *f_pXMLContext,
 __in    enum  DRM_CERTIFICATE_TYPE   f_oCertType,
 __in          DRM_RSA_PUBLIC_KEY    *f_pKeyPublic,
 __in    const DRM_ANSI_CONST_STRING *f_pdastrRSAPubKeyModulusB64,
 __in    const DRM_ANSI_CONST_STRING *f_pdastrRSAPubKeyExponentB64,
 __in    const DRM_RSA_PRIVATE_KEY   *f_pRSAPrivKey,
 __in          DRM_ANSI_CONST_STRING *f_pdastrTemplate,
 __in_opt      DRM_ANSI_CONST_STRING *f_pdastrCerts,
 __in          DRM_CRYPTO_CONTEXT    *f_pCryptoCtx )
{
    DRM_RESULT    dr     = DRM_SUCCESS;
    DRM_SUBSTRING dasstr = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_ND_CB_CreateCertificateFile );

    ChkArg( f_pXMLContext != NULL );
    ChkArg( f_pCryptoCtx != NULL );
    ChkArg( f_pKeyPublic != NULL );
    ChkDRMANSIString( f_pdastrRSAPubKeyModulusB64 );
    ChkDRMANSIString( f_pdastrRSAPubKeyExponentB64 );
    ChkArg( f_pRSAPrivKey != NULL );
    ChkArg( f_pdastrTemplate != NULL );

    ChkArg( f_oCertType == certtypeRoot
         || f_oCertType == certtypeCertAuthority
         || f_oCertType == certtypeBlackBox
         || f_oCertType == certtypeBlackBoxVersion
         || f_oCertType == certtypeIndivBox
         || f_oCertType == certtypeExport
         || f_oCertType == certtypeExportIncl
         || f_oCertType == certtypeSampleProtection );

    /*
    ** <c:CertificateCollection
    **     xmlns="http://www.w3.org/2000/09/xmldsig#"
    **     xmlns:c="http://schemas.microsoft.com/DRM/2004/02/cert"
    **     c:Version="2.0">
    */
    ChkDR( DRM_XMB_CreateDocumentA( f_pXMLContext->cbContext, (DRM_BYTE*)f_pXMLContext, &g_dastrTagCertificateCollection ) );
    ChkDR( DRM_XMB_AddAttributeA( f_pXMLContext, &g_dastrSigRootAttribName,     &g_dastrSigRootAttribValue ) );
    ChkDR( DRM_XMB_AddAttributeA( f_pXMLContext, &g_dastrPrefixMicrosoftCert,   &g_dastrURITransformMSCertColl ) );
    ChkDR( DRM_XMB_AddAttributeA( f_pXMLContext, &g_dastrAttributeVersionWMDRM, &g_dastrVersionWMDRM ) );

    ChkDR( DRM_ND_CB_CreateCertificate( f_pXMLContext,
                                        f_oCertType,
                                        f_pKeyPublic,
                                        f_pdastrRSAPubKeyModulusB64,
                                        f_pdastrRSAPubKeyExponentB64,
                                        f_pRSAPrivKey,
                                        f_pdastrTemplate,
                                        f_pCryptoCtx ) );

    if ( f_pdastrCerts != NULL )
    {
        ChkDR( DRM_XMB_AppendNodeA( f_pXMLContext, f_pdastrCerts ) );
    }

    ChkDR( DRM_XMB_CloseDocumentA( f_pXMLContext, &dasstr)); /* </c:CertificateCollection> */
    f_pXMLContext->cchBuffer = dasstr.m_cch; /* adjusting size */

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


EXIT_PK_NAMESPACE_CODE;
