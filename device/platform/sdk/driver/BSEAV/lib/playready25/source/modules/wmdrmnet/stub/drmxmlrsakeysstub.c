/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMXMLRSAKEYS_C
#include <oemrsa.h>
#include <drmxmlbuilder.h>
#include <drmxmlbuildera.h>
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM_25004, "Ignore prefast warning about possible const parameters for this entire file.")

#include <drmxmlrsakeys.h>
#include <drmutf.h>
#include <drmutilities.h>
#include <drmxmlsigconstants.h>
#include <drmndcertconstants.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;


DRM_API DRM_RESULT DRM_CALL DRM_XML_RSA_WritePublicKeyNodeA(
   __inout        _XMBContextA                  *f_pXMLCtxA,
   __in_opt const DRM_ANSI_CONST_STRING         *f_pdastrWrapperTag,
   __in     const DRM_RSA_PUBLIC_KEY_2048BIT    *f_pKeyRSAPublic )
{
    UNREFERENCED_PARAMETER( f_pXMLCtxA );
    UNREFERENCED_PARAMETER( f_pdastrWrapperTag );
    UNREFERENCED_PARAMETER( f_pKeyRSAPublic );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_XML_RSA_WritePublicKeyNode(
   __inout        _XMBContext                *f_pXMLCtx,
   __in_opt const DRM_CONST_STRING           *f_pdstrWrapperTag,
   __in     const DRM_RSA_PUBLIC_KEY_2048BIT *f_pKeyRSAPublic )
{
    UNREFERENCED_PARAMETER( f_pXMLCtx );
    UNREFERENCED_PARAMETER( f_pdstrWrapperTag );
    UNREFERENCED_PARAMETER( f_pKeyRSAPublic );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_XML_RSA_ParseBase64PublicKey(
    __in    const DRM_CONST_STRING           *f_dstrPubKeyModulus,
    __in    const DRM_CONST_STRING           *f_dstrPubKeyExponent,
    __inout       DRM_RSA_PUBLIC_KEY_2048BIT *f_pKeyRSAPublic )
{
    UNREFERENCED_PARAMETER( f_dstrPubKeyModulus );
    UNREFERENCED_PARAMETER( f_dstrPubKeyExponent );
    UNREFERENCED_PARAMETER( f_pKeyRSAPublic );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_XML_RSA_ParseBase64PublicKeyA(
    __in    const DRM_ANSI_CONST_STRING         *f_dastrPubKeyModulus,
    __in    const DRM_SUBSTRING                 *f_dasstrPubKeyModulus,
    __in    const DRM_ANSI_CONST_STRING         *f_dastrPubKeyExponent,
    __in    const DRM_SUBSTRING                 *f_dasstrPubKeyExponent,
    __inout       DRM_RSA_PUBLIC_KEY_2048BIT    *f_pKeyRSAPublic )
{
    UNREFERENCED_PARAMETER( f_dastrPubKeyModulus );
    UNREFERENCED_PARAMETER( f_dasstrPubKeyModulus );
    UNREFERENCED_PARAMETER( f_dastrPubKeyExponent );
    UNREFERENCED_PARAMETER( f_dasstrPubKeyExponent );
    UNREFERENCED_PARAMETER( f_pKeyRSAPublic );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_XML_RSA_WritePrivateKeyNode(
    __inout        _XMBContext                 *f_pXMLCtx,
    __in_opt const DRM_CONST_STRING            *f_pdstrWrapperTag,
    __in     const DRM_RSA_PRIVATE_KEY_2048BIT *f_pKeyRSAPrivate )
{
    UNREFERENCED_PARAMETER( f_pXMLCtx );
    UNREFERENCED_PARAMETER( f_pdstrWrapperTag );
    UNREFERENCED_PARAMETER( f_pKeyRSAPrivate );

    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_XML_RSA_ParseBase64PrivateKey(
    __in    const DRM_CONST_STRING            *f_dstrPrivKeyPrime0,
    __in    const DRM_CONST_STRING            *f_dstrPrivKeyPrime1,
    __in    const DRM_CONST_STRING            *f_dstrPrivKeyCRTExponent0,
    __in    const DRM_CONST_STRING            *f_dstrPrivKeyCRTExponent1,
    __in    const DRM_CONST_STRING            *f_dstrPrivKeyIQMP,
    __inout       DRM_RSA_PRIVATE_KEY_2048BIT *f_pKeyRSAPrivate )
{
    UNREFERENCED_PARAMETER( f_dstrPrivKeyPrime0 );
    UNREFERENCED_PARAMETER( f_dstrPrivKeyPrime1 );
    UNREFERENCED_PARAMETER( f_dstrPrivKeyCRTExponent0 );
    UNREFERENCED_PARAMETER( f_dstrPrivKeyCRTExponent1 );
    UNREFERENCED_PARAMETER( f_dstrPrivKeyIQMP );
    UNREFERENCED_PARAMETER( f_pKeyRSAPrivate );

    return DRM_E_NOTIMPL;
}

PREFAST_POP /* ignore prefast warning about possible const parameters for this entire file  */

EXIT_PK_NAMESPACE_CODE;

