/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMNDCERTBUILDER_C
#include <drmxmlbuilder.h>
#include <drmxmlbuildera.h>
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Ignore prefast warning about possible const parameters for this entire file.")

#include <drmxmlrsakeys.h>
#include <oemrsa.h>
#include <drmndcertbuilder.h>
#include <drmxmlsigconstants.h>
#include <drmwmdrmnet.h>
#include <drmndcertconstants.h>
#include <drmprofile.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_CreateDigitalSignature(
    __inout                           _XMBContextA                  *f_pXMLContext,
    __in                        const DRM_ANSI_CONST_STRING         *f_pdastrRSAPubKeyModulusB64,
    __in                        const DRM_ANSI_CONST_STRING         *f_pdastrRSAPubKeyExponentB64,
    __in                        const DRM_RSA_PRIVATE_KEY_2048BIT   *f_pRSAPrivKey,
    __in                        const DRM_DWORD                      f_cchSHA,
    __in_ecount( f_cchSHA )     const DRM_CHAR                      *f_pchSHA,
    __in                        const DRM_DWORD                      f_cchRSA,
    __in_ecount_opt( f_cchRSA ) const DRM_CHAR                      *f_pchRSA,
    __in                              DRM_DWORD                      f_dwSignType,
    __in                              DRM_DWORD                      f_dwFlags,
    __in                              DRM_CRYPTO_CONTEXT            *f_pCryptoCtx )
{
    UNREFERENCED_PARAMETER( f_pXMLContext );
    UNREFERENCED_PARAMETER( f_pdastrRSAPubKeyModulusB64 );
    UNREFERENCED_PARAMETER( f_pdastrRSAPubKeyExponentB64 );
    UNREFERENCED_PARAMETER( f_pRSAPrivKey );
    UNREFERENCED_PARAMETER( f_cchSHA );
    UNREFERENCED_PARAMETER( f_pchSHA );
    UNREFERENCED_PARAMETER( f_cchRSA );
    UNREFERENCED_PARAMETER( f_pchRSA );
    UNREFERENCED_PARAMETER( f_dwSignType );
    UNREFERENCED_PARAMETER( f_dwFlags );
    UNREFERENCED_PARAMETER( f_pCryptoCtx );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_CreateCertificateInit(
  __inout       _XMBContextA               *f_pXMLContextA,
  __in     enum DRM_CERTIFICATE_TYPE        f_eCertType,
  __in    const DRM_RSA_PUBLIC_KEY_2048BIT *f_pKeyPublic,
  __in    const DRM_CRYPTO_CONTEXT         *f_pCryptoCtx  )
{
    UNREFERENCED_PARAMETER( f_pXMLContextA );
    UNREFERENCED_PARAMETER( f_eCertType );
    UNREFERENCED_PARAMETER( f_pKeyPublic );
    UNREFERENCED_PARAMETER( f_pCryptoCtx );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_CreateCertificateAddTemplateData(
  __inout       _XMBContextA               *f_pXMLContextA,
  __in    const DRM_ANSI_CONST_STRING      *f_pdastrTemplate )
{
    UNREFERENCED_PARAMETER( f_pXMLContextA );
    UNREFERENCED_PARAMETER( f_pdastrTemplate );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_CreateCertificateSignAndClose(
  __inout       _XMBContextA                  *f_pXMLContextA,
  __in    const DRM_ANSI_CONST_STRING         *f_pdastrRSAPubKeyModulusB64,
  __in    const DRM_ANSI_CONST_STRING         *f_pdastrRSAPubKeyExponentB64,
  __in    const DRM_RSA_PRIVATE_KEY_2048BIT   *f_pRSAPrivKey,
  __in          DRM_CRYPTO_CONTEXT            *f_pCryptoCtx  )
{
    UNREFERENCED_PARAMETER( f_pXMLContextA );
    UNREFERENCED_PARAMETER( f_pdastrRSAPubKeyModulusB64 );
    UNREFERENCED_PARAMETER( f_pdastrRSAPubKeyExponentB64 );
    UNREFERENCED_PARAMETER( f_pRSAPrivKey );
    UNREFERENCED_PARAMETER( f_pCryptoCtx );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_CreateCertificate(
  __inout       _XMBContextA                *f_pXMLContextA,
  __in    enum  DRM_CERTIFICATE_TYPE         f_eCertType,
  __in    const DRM_RSA_PUBLIC_KEY_2048BIT  *f_pKeyPublic,
  __in    const DRM_ANSI_CONST_STRING       *f_pdastrRSAPubKeyModulusB64,
  __in    const DRM_ANSI_CONST_STRING       *f_pdastrRSAPubKeyExponentB64,
  __in    const DRM_RSA_PRIVATE_KEY_2048BIT *f_pRSAPrivKey,
  __in    const DRM_ANSI_CONST_STRING       *f_pdastrTemplate,
  __in          DRM_CRYPTO_CONTEXT          *f_pCryptoCtx  )
{
    UNREFERENCED_PARAMETER( f_pXMLContextA );
    UNREFERENCED_PARAMETER( f_eCertType );
    UNREFERENCED_PARAMETER( f_pKeyPublic );
    UNREFERENCED_PARAMETER( f_pdastrRSAPubKeyModulusB64 );
    UNREFERENCED_PARAMETER( f_pdastrRSAPubKeyExponentB64 );
    UNREFERENCED_PARAMETER( f_pRSAPrivKey );
    UNREFERENCED_PARAMETER( f_pdastrTemplate );
    UNREFERENCED_PARAMETER( f_pCryptoCtx );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_AddAttribute(
  __inout                      _XMBContextA             *f_pXMLContext,
  __in                         DRM_WMDRM_CERT_ATTRIBUTE  f_eCertAttribute,
  __in_ecount(f_cEntries)const DRM_ANSI_CONST_STRING    *f_rgpdastrValue     [],
  __in                         DRM_DWORD                 f_cEntries )
{
    UNREFERENCED_PARAMETER( f_pXMLContext );
    UNREFERENCED_PARAMETER( f_eCertAttribute );
    UNREFERENCED_PARAMETER( f_rgpdastrValue );
    UNREFERENCED_PARAMETER( f_cEntries );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_AddManufacturerNodes(
  __inout                           _XMBContextA           *f_pXMLContext,
  __in_opt                    const DRM_ANSI_CONST_STRING  *f_pdastrNamespace,
  __in_ecount(f_cEntries)     const DRM_ANSI_CONST_STRING **f_rgpdastrTag,
  __in_ecount_opt(f_cEntries) const DRM_ANSI_CONST_STRING **f_rgpdastrAttrLabel,
  __in_ecount_opt(f_cEntries) const DRM_ANSI_CONST_STRING **f_rgpdastrAttrValue,
  __in_ecount(f_cEntries)     const DRM_ANSI_CONST_STRING **f_rgpdastrValue,
  __in                        const DRM_DWORD               f_cEntries )
{
    UNREFERENCED_PARAMETER( f_pXMLContext );
    UNREFERENCED_PARAMETER( f_pdastrNamespace );
    UNREFERENCED_PARAMETER( f_rgpdastrTag );
    UNREFERENCED_PARAMETER( f_rgpdastrAttrLabel );
    UNREFERENCED_PARAMETER( f_rgpdastrAttrValue );
    UNREFERENCED_PARAMETER( f_rgpdastrValue );
    UNREFERENCED_PARAMETER( f_cEntries );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_SignCertificate(
    __inout       _XMBContextA                  *f_pXMLContext,
    __in          DRM_BOOL                       f_fWMDRMNET,
    __in    const DRM_ANSI_CONST_STRING         *f_pdastrRSAPubKeyModulusB64,
    __in    const DRM_ANSI_CONST_STRING         *f_pdastrRSAPubKeyExponentB64,
    __in    const DRM_RSA_PRIVATE_KEY_2048BIT   *f_pRSAPrivKey,
    __in          DRM_CRYPTO_CONTEXT            *f_pCryptoCtx  )
{
    UNREFERENCED_PARAMETER( f_pXMLContext );
    UNREFERENCED_PARAMETER( f_fWMDRMNET );
    UNREFERENCED_PARAMETER( f_pdastrRSAPubKeyModulusB64 );
    UNREFERENCED_PARAMETER( f_pdastrRSAPubKeyExponentB64 );
    UNREFERENCED_PARAMETER( f_pRSAPrivKey );
    UNREFERENCED_PARAMETER( f_pCryptoCtx );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_ND_CB_CreateCertificateFile(
    __inout        _XMBContextA                *f_pXMLContext,
    __in     enum  DRM_CERTIFICATE_TYPE         f_oCertType,
    __in     const DRM_RSA_PUBLIC_KEY_2048BIT  *f_pKeyPublic,
    __in     const DRM_ANSI_CONST_STRING       *f_pdastrRSAPubKeyModulusB64,
    __in     const DRM_ANSI_CONST_STRING       *f_pdastrRSAPubKeyExponentB64,
    __in     const DRM_RSA_PRIVATE_KEY_2048BIT *f_pRSAPrivKey,
    __in     const DRM_ANSI_CONST_STRING       *f_pdastrTemplate,
    __in_opt const DRM_ANSI_CONST_STRING       *f_pdastrCerts,
    __in           DRM_CRYPTO_CONTEXT          *f_pCryptoCtx )
{
    UNREFERENCED_PARAMETER( f_pXMLContext );
    UNREFERENCED_PARAMETER( f_oCertType );
    UNREFERENCED_PARAMETER( f_pKeyPublic );
    UNREFERENCED_PARAMETER( f_pdastrRSAPubKeyModulusB64 );
    UNREFERENCED_PARAMETER( f_pdastrRSAPubKeyExponentB64 );
    UNREFERENCED_PARAMETER( f_pRSAPrivKey );
    UNREFERENCED_PARAMETER( f_pdastrTemplate );
    UNREFERENCED_PARAMETER( f_pdastrCerts );
    UNREFERENCED_PARAMETER( f_pCryptoCtx );

    return DRM_E_NOTIMPL;
}

PREFAST_POP /* ignore prefast warning about possible const parameters for this entire file  */

EXIT_PK_NAMESPACE_CODE;
