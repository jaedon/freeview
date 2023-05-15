/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>

#include <ndtnetmsg.h>
#include <drmndr.h>
#include <drmxmlrsakeys.h>
#include <drmndcertbuilder.h>
#include <drmxmlsigconstants.h>
#include <drmwmdrmnet.h>
#include <drmwmdrmnetconstants.h>

ENTER_PK_NAMESPACE_CODE;

/* Set limitation to XML builder stack size: no more than 40 tags */
#define XML_BUILDER_STACK_SIZE (40*SIZEOF(DRM_DWORD))

/******************************************************************************
**
** Function :   _VerifyRSACertCollectionOnDevice
**
** Synopsis :   Verifies ND certificate collection
**
** Arguments :  [f_pdastrCertCollection]  - pointer to the OEM context; may be NULL
**              [f_pCryptoCtx]            - pointer to the DRM crypto context
**              [f_pdastrLeafCertificate] - pointer to DRM UTF-8 string that will contain
**                                          leaf certificate inside of supplied cert collection.
**                                          Optional.
**              [f_pdastrPubkeyMachine]   - pointer to DRM UTF-8 string that will contain
**                                          base64 encode RSA public key inside of leaf certificate.
**                                          Optional.
** Returns :    DRM_SUCCESS if verification succeed
**
******************************************************************************/
static DRM_RESULT _VerifyRSACertCollectionOnDevice(
    __in        DRM_ANSI_CONST_STRING *f_pdastrCertCollection,
    __in        DRM_CRYPTO_CONTEXT    *f_pCryptoCtx,
    __inout_opt DRM_ANSI_CONST_STRING *f_pdastrLeafCertificate,
    __inout_opt DRM_ANSI_CONST_STRING *f_pdastrPubkeyMachine )
{
    DRM_RESULT    dr                    = DRM_SUCCESS;
    DRM_SUBSTRING dasstrCollection      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrLeafCertificate = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrPubkeyMachine   = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC__VerifyRSACertCollectionOnDevice );

    ChkArg( f_pdastrCertCollection != NULL );
    ChkArg( f_pCryptoCtx!= NULL );

    dasstrCollection.m_cch = f_pdastrCertCollection->cchString;

    ChkDR( DRM_WCP_VerifyCertificateCollection( f_pdastrCertCollection->pszString,
                                                &dasstrCollection,
                                                &g_dastrMSNDRootPubKeyB64,
                                                NULL,
                                                0,
                                                &dasstrLeafCertificate,
                                                &dasstrPubkeyMachine,
                                                f_pCryptoCtx ) );

   if ( f_pdastrLeafCertificate != NULL )
   {
       f_pdastrLeafCertificate->pszString = f_pdastrCertCollection->pszString + dasstrLeafCertificate.m_ich;
       f_pdastrLeafCertificate->cchString = dasstrLeafCertificate.m_cch;
   }

   if ( f_pdastrPubkeyMachine != NULL )
   {
       f_pdastrPubkeyMachine->pszString = f_pdastrCertCollection->pszString + dasstrPubkeyMachine.m_ich;
       f_pdastrPubkeyMachine->cchString = dasstrPubkeyMachine.m_cch;
   }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   _GetModelRSAPrivKeyOnDevice
**
** Synopsis :   Gets Model RSA private key from file and parses it
**
** Arguments :  [f_pOEMContext]  - pointer to the OEM context; may be NULL
**              [f_pbBuffer]     - pointer to byte array for loading RSA private key file
**              [f_cbBuffer]     - size of byte array
**              [f_pCryptoCtx]   - pointer to the DRM crypto context
**              [f_poRSAPrivKey] - pointer to result RSA private key data structure
**
** Returns :    DRM_SUCCESS if RSA private key was successfully parsed
**
******************************************************************************/
static DRM_RESULT _GetModelRSAPrivKeyOnDevice(
    __in_opt                DRM_VOID              *f_pOEMContext,
    __in_bcount(f_cbBuffer) DRM_BYTE              *f_pbBuffer,
    __in                    DRM_DWORD              f_cbBuffer,
    __in                    DRM_CRYPTO_CONTEXT    *f_pCryptoCtx,
    __out                   DRM_RSA_PRIVATE_KEY   *f_poRSAPrivKey )
{
    DRM_RESULT       dr                      = DRM_SUCCESS;
    DRM_CONST_STRING dstrPrivKeyData         = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKeyPrime0       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKeyPrime1       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKeyCRTExponent0 = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKeyCRTExponent1 = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPrivKeyIQMP         = EMPTY_DRM_STRING;
    DRM_DWORD        cbKeyFileSize           = f_cbBuffer;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC__GetModelRSAPrivKeyOnDevice );

    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_pCryptoCtx != NULL );
    ChkArg( f_poRSAPrivKey != NULL );

    ChkDR(  Oem_Device_GetKey( f_pOEMContext,
                               DRM_DKT_PLAYREADY_NDR_MODEL,
                               f_pbBuffer,
                               &cbKeyFileSize ) );

    dstrPrivKeyData.pwszString = (DRM_WCHAR*)f_pbBuffer;
    dstrPrivKeyData.cchString = cbKeyFileSize / SIZEOF(DRM_WCHAR);

    ChkDR( DRM_XML_GetSubNodeByPath( &dstrPrivKeyData,
                                     &g_dstrXPathPrime0,
                                     NULL,
                                     NULL,
                                     NULL,
                                     &dstrPrivKeyPrime0,
                                     g_wchForwardSlash ) );

    ChkDR( DRM_XML_GetSubNodeByPath( &dstrPrivKeyData,
                                     &g_dstrXPathPrime1,
                                     NULL,
                                     NULL,
                                     NULL,
                                     &dstrPrivKeyPrime1,
                                     g_wchForwardSlash ) );

    ChkDR( DRM_XML_GetSubNodeByPath( &dstrPrivKeyData,
                                     &g_dstrXPathCRTExponent0,
                                     NULL,
                                     NULL,
                                     NULL,
                                     &dstrPrivKeyCRTExponent0,
                                     g_wchForwardSlash ) );

    ChkDR( DRM_XML_GetSubNodeByPath( &dstrPrivKeyData,
                                     &g_dstrXPathCRTExponent1,
                                     NULL,
                                     NULL,
                                     NULL,
                                     &dstrPrivKeyCRTExponent1,
                                     g_wchForwardSlash ) );

    ChkDR( DRM_XML_GetSubNodeByPath( &dstrPrivKeyData,
                                     &g_dstrXPathIQMP,
                                     NULL,
                                     NULL,
                                     NULL,
                                     &dstrPrivKeyIQMP,
                                     g_wchForwardSlash ) );

    /*
    **  Parse XML to get the RSA-1024 private key
    */
    ChkDR( DRM_XML_RSA_ParseBase64PrivateKey( &dstrPrivKeyPrime0,
                                              &dstrPrivKeyPrime1,
                                              &dstrPrivKeyCRTExponent0,
                                              &dstrPrivKeyCRTExponent1,
                                              &dstrPrivKeyIQMP,
                                              f_poRSAPrivKey ) );
ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/******************************************************************************
**
** Function :   _UpdateDeviceCertificateProperties
**
** Synopsis :   Updates device certificate data properties
**
** Arguments :  [f_pXMLContextA]           - pointer to XML building context
**              [f_pdastrModelCertificate] - pointer to DRM ANSI string with model
**                                           certificate
**
** Returns :    DRM_SUCCESS if certificate was successfully updated
**
******************************************************************************/
static DRM_RESULT _UpdateDeviceCertificateProperties(
    __inout _XMBContextA          *f_pXMLContextA,
    __in    DRM_ANSI_CONST_STRING *f_pdastrModelCertificate )
{
    DRM_RESULT             dr                 = DRM_SUCCESS;
    DRM_SUBSTRING          dasstrTempData     = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING          dasstrDataSection  = EMPTY_DRM_SUBSTRING;
    DRM_ANSI_CONST_STRING  dastrSecurityLevel = EMPTY_DRM_STRING;
    DRM_ANSI_CONST_STRING  dastrManufData     = EMPTY_DRM_STRING;
    const DRM_ANSI_CONST_STRING *rgpdastrValues[2] = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC__UpdateDeviceCertificateProperties );

    ChkArg( f_pXMLContextA != NULL );
    ChkDRMANSIString( f_pdastrModelCertificate );

    /* Get data section from model certificate */
    {
        dasstrTempData.m_cch = f_pdastrModelCertificate->cchString;

        ChkDR( DRM_XML_GetNodeA( f_pdastrModelCertificate->pszString,
                                 &dasstrTempData,
                                 &g_dastrTagCertificateCollection,
                                 NULL,
                                 NULL,
                                 0,
                                 NULL,
                                 &dasstrTempData ) );

        ChkDR( DRM_XML_GetNodeA( f_pdastrModelCertificate->pszString,
                                 &dasstrTempData,
                                 &g_dastrTagWMDRMCertificate,
                                 NULL,
                                 NULL,
                                 0,
                                 NULL,
                                 &dasstrTempData ) );

        ChkDR( DRM_XML_GetNodeA( f_pdastrModelCertificate->pszString,
                                 &dasstrTempData,
                                 &g_dastrTagWMDRMData,
                                 NULL,
                                 NULL,
                                 0,
                                 NULL,
                                 &dasstrDataSection ) );

        /* Copy security level from model certificate */
        ChkDR( DRM_XML_GetNodeA( f_pdastrModelCertificate->pszString,
                                 &dasstrDataSection,
                                 &g_dastrTagWMDRMCertSecurityLevel,
                                 NULL,
                                 NULL,
                                 0,
                                 NULL,
                                 &dasstrTempData ) );
    }

    dastrSecurityLevel.pszString = f_pdastrModelCertificate->pszString + dasstrTempData.m_ich;
    dastrSecurityLevel.cchString = dasstrTempData.m_cch;
    rgpdastrValues[0] = &dastrSecurityLevel;
    ChkDR( DRM_ND_CB_AddAttribute( f_pXMLContextA,
                                    DRM_WMDRM_CERT_ATTR_SECURITY_LEVEL,
                                    rgpdastrValues,
                                    1 ) );

    /* Set appropriate key usage for device ND cert */
    rgpdastrValues[0] = &g_dastrKeyUsageEncryptKey;
    ChkDR( DRM_ND_CB_AddAttribute( f_pXMLContextA,
                                   DRM_WMDRM_CERT_ATTR_KEY_USAGE,
                                   rgpdastrValues,
                                   1 ) );

    /* Set transmitter and receiver features */
    rgpdastrValues[0] = &g_dastrFeaturesWMDRMTransmitter;
    rgpdastrValues[1] = &g_dastrFeaturesWMDRMReceiver;
    ChkDR( DRM_ND_CB_AddAttribute( f_pXMLContextA,
                                   DRM_WMDRM_CERT_ATTR_FEATURES,
                                   rgpdastrValues,
                                   2 ) );

    /* Copy manufacturer data from data sction of model certificate */
    dasstrTempData.m_cch = f_pdastrModelCertificate->cchString;

    ChkDR( DRM_XML_GetNodeA( f_pdastrModelCertificate->pszString,
                             &dasstrDataSection,
                             &g_dastrTagManufacturerData,
                             NULL,
                             NULL,
                             0,
                             &dasstrTempData,
                             NULL ) );

    dastrManufData.pszString = f_pdastrModelCertificate->pszString + dasstrTempData.m_ich;
    dastrManufData.cchString = dasstrTempData.m_cch;
    ChkDR( DRM_XMB_AppendNodeA( f_pXMLContextA, &dastrManufData ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function :   DRM_NDCert_CreateDeviceCertificate
**
** Synopsis :   Generates PlayReady-ND device certificate and verifies correctness
**
** Arguments :  [f_pOEMContext]       - pointer to the OEM context; may be NULL
**              [f_pdastrBuffer]      - pointer to ND model cert, if creation is successful
**                                      contains pointer to ND device cert
**                                      cch contains size of certificate, buffer size is larger
**              [f_cchBuffer]          - size of f_pdastrBuffer
**              [f_poRSAModelPrivKey] - pointer to model/group private key
**              [f_poRSADevicePubKey] - pointer to device public key
**              [f_pCryptoCtx]        - pointer to the DRM crypto context
**
** Returns :    DRM_SUCCESS if certificate was successfully created and verified
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_NDCert_CreateDeviceCertificate(
  __in_opt DRM_VOID            *f_pOEMContext,
  __inout  DRM_ANSI_STRING     *f_pdastrBuffer,
  __in     DRM_DWORD            f_cchBuffer,
  __in     DRM_RSA_PRIVATE_KEY *f_poRSAModelPrivKey,
  __in     DRM_RSA_PUBLIC_KEY  *f_poRSADevicePubKey,
  __in     DRM_CRYPTO_CONTEXT  *f_pCryptoCtx  )
{
    DRM_RESULT             dr                    = DRM_SUCCESS;
    _XMBContextA          *pXMLContextA          = NULL;
    DRM_DWORD              cbDeviceCertSize      = 0;
    DRM_DWORD              cbBytesToMove         = 0;
    DRM_ANSI_CONST_STRING  dastrModelCert            = EMPTY_DRM_STRING;
    DRM_ANSI_CONST_STRING  dastrRSAPubKeyModulusB64  = EMPTY_DRM_STRING;
    DRM_ANSI_CONST_STRING  dastrModelCertCollection  = EMPTY_DRM_STRING;
    DRM_SUBSTRING          dasstrModelCertCollection = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_NDCert_CreateDeviceCertificate );

    ChkDRMANSIString( f_pdastrBuffer );
    ChkArg( f_cchBuffer >= f_pdastrBuffer->cchString );
    ChkArg( f_pCryptoCtx != NULL );

    /* Lets move model certificate to the end of the buffer */
    ChkOverflow( f_pdastrBuffer->pszString + f_cchBuffer - f_pdastrBuffer->cchString,
                 f_pdastrBuffer->pszString );
    dastrModelCert.pszString = f_pdastrBuffer->pszString + f_cchBuffer - f_pdastrBuffer->cchString;
    dastrModelCert.cchString = f_pdastrBuffer->cchString;
    MEMMOVE( (DRM_CHAR*) dastrModelCert.pszString,
             f_pdastrBuffer->pszString,
             dastrModelCert.cchString );

    ChkDR( _VerifyRSACertCollectionOnDevice( &dastrModelCert,
                                             f_pCryptoCtx,
                                             NULL,
                                             &dastrRSAPubKeyModulusB64 ) );

    /*
    ** Get certificate collection from model certificate
    */
    {
        DRM_SUBSTRING dasstrModelCert = EMPTY_DRM_SUBSTRING;

        dasstrModelCert.m_cch = dastrModelCert.cchString;
        ChkDR( DRM_XML_GetNodeA( dastrModelCert.pszString,
                                 &dasstrModelCert,
                                 &g_dastrTagCertificateCollection,
                                 NULL,
                                 NULL,
                                 0,
                                 NULL,
                                &dasstrModelCertCollection ) );
        dastrModelCertCollection.pszString = dastrModelCert.pszString + dasstrModelCertCollection.m_ich;
        dastrModelCertCollection.cchString = dasstrModelCertCollection.m_cch;
    }

   /*
   ** Begin building cert collection: set available size for device certificate.
   ** XML builder is using top of the buffer to keep it's own stack data.
   */
    ChkBOOL( f_cchBuffer > dastrModelCert.cchString, DRM_E_BUFFERTOOSMALL );
    cbDeviceCertSize = f_cchBuffer - dastrModelCert.cchString - XML_BUILDER_STACK_SIZE;

    pXMLContextA = (_XMBContextA*)f_pdastrBuffer->pszString;

    /*
    ** <c:CertificateCollection
    **     xmlns="http://www.w3.org/2000/09/xmldsig#"
    **     xmlns:c="http://schemas.microsoft.com/DRM/2004/02/cert"
    **     c:Version="2.0">
    */
    ChkDR( DRM_XMB_CreateDocumentA( cbDeviceCertSize,
                                    (DRM_BYTE*)pXMLContextA,
                                    &g_dastrTagCertificateCollection ) );
    ChkDR( DRM_XMB_AddAttributeA( pXMLContextA, &g_dastrSigRootAttribName,     &g_dastrSigRootAttribValue ) );
    ChkDR( DRM_XMB_AddAttributeA( pXMLContextA, &g_dastrPrefixMicrosoftCert,   &g_dastrURITransformMSCertColl ) );
    ChkDR( DRM_XMB_AddAttributeA( pXMLContextA, &g_dastrAttributeVersionWMDRM, &g_dastrVersionWMDRM ) );

    ChkDR( DRM_ND_CB_CreateCertificateInit( pXMLContextA,
                                            certtypeDevice,
                                            f_poRSADevicePubKey,
                                            f_pCryptoCtx ) );

    ChkDR( _UpdateDeviceCertificateProperties( pXMLContextA, &dastrModelCert ) );

    ChkDR( DRM_ND_CB_CreateCertificateSignAndClose( pXMLContextA,
                                                    &dastrRSAPubKeyModulusB64,
                                                    &g_dastrWMDRMCertExponent,
                                                    f_poRSAModelPrivKey,
                                                    f_pCryptoCtx ) );

    /* Check for data "overwrite" */
    ChkOverflow( pXMLContextA->rgchXML + pXMLContextA->ichNextString, pXMLContextA->rgchXML );
    ChkBOOL( dastrModelCertCollection.pszString > pXMLContextA->rgchXML + pXMLContextA->ichNextString,
             DRM_E_BUFFERTOOSMALL );

    /*
    **   Move model certificate collection and closing tag to right place.
    **   DRM_XMB_Close is not used (to close XML) because pointers inside XML builder header are invalid
    **   after MEMMOVE
    */
    cbBytesToMove = dastrModelCert.cchString - (DRM_DWORD)( dastrModelCertCollection.pszString - dastrModelCert.pszString );
    MEMMOVE( pXMLContextA->rgchXML + pXMLContextA->ichNextString,
             dastrModelCertCollection.pszString,
             cbBytesToMove );

    /* Remove _XMBContextA header from f_pdastrBuffer */
    cbBytesToMove += pXMLContextA->ichNextString;
    MEMMOVE( (DRM_CHAR*) f_pdastrBuffer->pszString,
             &pXMLContextA->rgchXML[0],
             cbBytesToMove );
    f_pdastrBuffer->pszString[cbBytesToMove] = 0;

    f_pdastrBuffer->cchString = cbBytesToMove;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/* =============================================================================
** Function:    DRM_NDCert_GetDeviceCertificate
** Synopsis:    Obtains device certificate: check if device certificate exists and valid;
**              if not - gets model certificate and builds device certificate
** Arguments:   [f_pOEMContext]  -- A pointer to the OEM Context; may be NULL
**              [f_pdastrNDCert] -- A pointer to a DRM ANSI string to contain the XML ND device certificate
**              [f_dwFlags]      -- Behavioral flags
**              [f_pCryptoCtx]   -- A pointer to the crypto context
** Returns:     DRM_SUCCESS on success
** Notes:
** =============================================================================
*/
DRM_API DRM_RESULT DRM_CALL DRM_NDCert_GetDeviceCertificate(
    __in_opt       DRM_VOID            *f_pOEMContext,
    __inout        DRM_ANSI_STRING     *f_pdastrBuffer,
    __in    const  DRM_DWORD            f_dwFlags,
    __inout        DRM_CRYPTO_CONTEXT  *f_pCryptoCtx
)
{
    DRM_RESULT dr               = DRM_SUCCESS;
    DRM_DWORD  cbDeviceCert     = 0;
    DRM_DWORD  cbOrigBuffer     = 0;
    DRM_DWORD  cbRSAPubKeySize  = DRM_RSA_CB_PUBKEY;
    DRM_DWORD  cbRSAPrivKeySize = DRM_RSA_CB_PRIVKEY;

    DRM_RSA_PRIVATE_KEY oRSADevicePrivKey = { 0 };
    DRM_RSA_PUBLIC_KEY  oRSADevicePubKey  = { 0 };
    DRM_RSA_PRIVATE_KEY oRSAModelPrivKey  = { 0 };

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_NDCERT, PERF_FUNC_DRM_NDCert_GetDeviceCertificate);

    ChkArg( f_pCryptoCtx != NULL);
    ChkDRMANSIString( f_pdastrBuffer );

    cbOrigBuffer = f_pdastrBuffer->cchString*SIZEOF(DRM_CHAR);
    cbDeviceCert = cbOrigBuffer;

    /*First try to get ND device certificate*/
    dr = Oem_Device_GetCert( f_pOEMContext,
                             DRM_DCT_NETRECEIVER,
                             (DRM_BYTE*)f_pdastrBuffer->pszString,
                             &cbDeviceCert );
    if( DRM_SUCCEEDED( dr ) )
    {
        f_pdastrBuffer->cchString = cbDeviceCert;

        /* return if flag not set */
        ChkBOOL( f_dwFlags & DRM_DCP_VERIFY_DEVCERT, dr );

        dr = _VerifyRSACertCollectionOnDevice( (DRM_ANSI_CONST_STRING*) f_pdastrBuffer,
                                               f_pCryptoCtx,
                                               NULL,
                                               NULL );
        /* return if verification if successful, continue otherwise */
        ChkBOOL ( DRM_FAILED(dr), dr );

    }

    /* if we reach here, the devcert is no good and we need to create a new one from template */
    ChkBOOL( f_dwFlags & DRM_DCP_CREATE_DEVCERT_IF_NOT_EXISTING, DRM_E_DEVCERTREADERROR );

    ZEROMEM( f_pdastrBuffer->pszString, cbOrigBuffer );
    ChkDR( _GetModelRSAPrivKeyOnDevice( f_pOEMContext,
                                        (DRM_BYTE*)f_pdastrBuffer->pszString,
                                        cbOrigBuffer,
                                        f_pCryptoCtx,
                                        &oRSAModelPrivKey ) );

    ZEROMEM( f_pdastrBuffer->pszString, cbOrigBuffer );

    /*
    ** Get ND model cert and check the size. If size of Model cert is
    ** greater than MAX_DEVICE_CERT_TEMPLATE_SIZE, return error
    */
    f_pdastrBuffer->cchString = min( MAX_DEVICE_CERT_TEMPLATE_SIZE, cbOrigBuffer );
    ZEROMEM( f_pdastrBuffer->pszString, cbOrigBuffer );

    {
        /* Create device private/public key pair */
        ChkDR( DRM_PK_GenKeyPair( f_pCryptoCtx,
                                  eDRM_RSA,
                                  eDRM_RSA_1024,
                                  &cbRSAPubKeySize,
                                  (DRM_BYTE*) &oRSADevicePubKey,
                                  &cbRSAPrivKeySize,
                                  (DRM_BYTE*) &oRSADevicePrivKey ) );

        /* Let's save device private key (in binary form) - key is not needed for now */
        ChkDR( Oem_Device_SetKey( f_pOEMContext,
                                  DRM_DKT_PLAYREADY_NDR,
                                  (DRM_BYTE*) &oRSADevicePrivKey,
                                  cbRSAPrivKeySize ) );
    }

    {
        /* Load model certificate */
        f_pdastrBuffer->cchString = cbOrigBuffer;
        dr = Oem_Device_GetCert( f_pOEMContext,
                                 DRM_DCT_NETRECEIVER_TEMPLATE,
                                 (DRM_BYTE*) f_pdastrBuffer->pszString,
                                 &f_pdastrBuffer->cchString );

        ChkBOOL( dr != DRM_E_BUFFERTOOSMALL, DRM_E_INVALIDDEVICECERTIFICATETEMPLATE );
    }

    /* Certificate didn't exist or not valid. Let's create one. */
    ChkDR( DRM_NDCert_CreateDeviceCertificate( f_pOEMContext,
                                               f_pdastrBuffer,
                                               cbOrigBuffer,
                                               &oRSAModelPrivKey,
                                               &oRSADevicePubKey,
                                               f_pCryptoCtx  ) );

    /* save the ND device certificate on device */
    ChkDR( Oem_Device_SetCert( f_pOEMContext,
                               DRM_DCT_NETRECEIVER,
                               (DRM_BYTE*) f_pdastrBuffer->pszString,
                               f_pdastrBuffer->cchString*SIZEOF(DRM_CHAR) ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;

}

/*****************************************************************************
** Function:    DRM_NDCert_Certificate_Initialize
**
** Synopsis:    Performs all of the tasks necessary to begin using a ND device certificate chain.
**
** Arguments:   [f_pContext]    : A pointer to the DRM application context
**              [f_pOEMContext] : A pointer to the OEM Context; may be NULL
**
** Returns:     DRM_SUCCESS      - on success
**              DRM_E_INVALIDARG - if the application context pointer is NULL
**              or other return code from a function called within
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_NDCert_Certificate_Initialize(
    __in     DRM_APP_CONTEXT  *f_pAppContext,
    __in_opt DRM_VOID         *f_pOEMContext
)
{
    DRM_RESULT                 dr              = DRM_SUCCESS;
    DRM_ANSI_STRING            dastrDeviceCert = EMPTY_DRM_STRING;
    DRM_APP_CONTEXT_INTERNAL  *pAppContext        = (DRM_APP_CONTEXT_INTERNAL *)f_pAppContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDCERT, PERF_FUNC_DRM_NDCert_Certificate_Initialize );

    ChkArg( pAppContext != NULL );

    DASTR_FROM_PB( &dastrDeviceCert,
                   pAppContext->pbDRMLicense,
                   pAppContext->cbDRMLicense );

    /*
    **  Get device certificate;
    **  Crypto validation for device certificate is switched off,
    **  to enable it add DRM_DCP_VERIFY_DEVCERT to behavioral flag
    */
    ChkDR( DRM_NDCert_GetDeviceCertificate( f_pOEMContext,
                                            &dastrDeviceCert,
                                            DRM_DCP_CREATE_DEVCERT_IF_NOT_EXISTING,
                                            &pAppContext->oBlackBoxContext.CryptoContext ) );

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

