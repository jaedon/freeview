/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmdla.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_BUFFER_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL DRM_LA_CreateChallenge (
    IN DRM_LICENSE_CHALLENGE *f_plicensechallenge,
    OUT      DRM_CHAR        *f_pszChallenge,
    IN OUT   DRM_DWORD       *f_pcchChallenge)
{
    UNREFERENCED_PARAMETER( f_plicensechallenge );
    UNREFERENCED_PARAMETER( f_pszChallenge );
    UNREFERENCED_PARAMETER( f_pcchChallenge );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_LA_GetClientInfo (
    IN       CLIENTID                   *f_pclientid,
    __in_bcount( *f_pcbStack ) DRM_BYTE *f_pbStack,
    IN OUT   DRM_DWORD                  *f_pcbStack,
    IN       DRM_CONST_STRING           *f_pdstrClientVersionData,
    IN       DRM_CONST_STRING           *f_pdstrSecurityVersionData,
    IN       DRM_DWORD                   f_AppSecurityData,
    IN       DRM_DWORD                   f_SubjectId1,
    IN       DRM_DWORD                   f_SubjectId2,
    IN       DRM_CONST_STRING           *f_pdstrDRMKVersion,
    IN       DRM_CONST_STRING           *f_pdstrDevcert,
    IN OUT   _XMBContext                *f_pbXML,
    IN OUT   DRM_DWORD                  *f_pcbXML,
    IN       DRM_CRYPTO_CONTEXT         *f_contextCRYP,
    IN       DRM_VOID                   *f_pOEMContext,
    IN       DRM_BOOL                    f_fCreateNewDoc,
    IN       DRM_CONST_STRING           *f_pdstrRevInfo)
{
    UNREFERENCED_PARAMETER( f_pclientid );
    UNREFERENCED_PARAMETER( f_pbStack );
    UNREFERENCED_PARAMETER( f_pcbStack );
    UNREFERENCED_PARAMETER( f_pdstrClientVersionData );
    UNREFERENCED_PARAMETER( f_pdstrSecurityVersionData );
    UNREFERENCED_PARAMETER( f_AppSecurityData );
    UNREFERENCED_PARAMETER( f_SubjectId1 );
    UNREFERENCED_PARAMETER( f_SubjectId2 );
    UNREFERENCED_PARAMETER( f_pdstrDRMKVersion );
    UNREFERENCED_PARAMETER( f_pdstrDevcert );
    UNREFERENCED_PARAMETER( f_pbXML );
    UNREFERENCED_PARAMETER( f_pcbXML );
    UNREFERENCED_PARAMETER( f_contextCRYP );
    UNREFERENCED_PARAMETER( f_pOEMContext );
    UNREFERENCED_PARAMETER( f_fCreateNewDoc );
    UNREFERENCED_PARAMETER( f_pdstrRevInfo );
    return DRM_E_NOTIMPL;
}

PREFAST_POP

EXIT_PK_NAMESPACE_CODE;

