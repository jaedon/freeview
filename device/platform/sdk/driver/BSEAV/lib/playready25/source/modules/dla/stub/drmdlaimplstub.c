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
PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "Changing parameters to const to satisfy warning would make them not match the real interface.")

DRM_API DRM_RESULT DRM_CALL DRM_WMDRMDLA_CreateChallenge(
    __in                               DRM_LICENSE_CHALLENGE *f_plicensechallenge,
    __out_ecount_opt(*f_pcchChallenge) DRM_CHAR              *f_pszChallenge,
    __inout                            DRM_DWORD             *f_pcchChallenge )
{
    UNREFERENCED_PARAMETER( f_plicensechallenge );
    UNREFERENCED_PARAMETER( f_pszChallenge );
    UNREFERENCED_PARAMETER( f_pcchChallenge );
    return DRM_E_NOTIMPL;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRMDLA_GetClientInfo(
    __in                                 CLIENTID               *f_pclientid,
    __in_bcount_opt( *f_pcbStack )       DRM_BYTE               *f_pbStack,
    __inout                              DRM_DWORD              *f_pcbStack,
    __in                           const DRM_CONST_STRING       *f_pdstrClientVersionData,
    __in                           const DRM_CONST_STRING       *f_pdstrSecurityVersionData,
    __in                                 DRM_DWORD               f_AppSecurityData,
    __in                                 DRM_DWORD               f_SubjectId1,
    __in                                 DRM_DWORD               f_SubjectId2,
    __in_opt                       const DRM_CONST_STRING       *f_pdstrDRMKVersion,
    __in_opt                       const DRM_CONST_STRING       *f_pdstrDevcert,
    __inout_opt                          _XMBContext            *f_pbXML,
    __inout                              DRM_DWORD              *f_pcbXML,
    __in                                 DRM_CRYPTO_CONTEXT     *f_contextCRYP,
    __in                                 DRM_VOID               *f_pOEMContext,
    __in                                 DRM_BOOL                f_fCreateNewDoc,
    __in_opt                       const DRM_CONST_STRING       *f_pdstrRevInfo )
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

PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */
PREFAST_POP /* Changing parameters to const to satisfy warning would make them not match the real interface. */

EXIT_PK_NAMESPACE_CODE;

