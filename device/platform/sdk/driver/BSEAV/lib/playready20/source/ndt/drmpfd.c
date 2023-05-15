/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <ndttypes.h>
#include <drmpfdmanager.h>
#include <ndtconstants.h>
#include <ndtcontextsizes.h>
#include <ndtlicense.h>
#include <ndtreg.h>
#include <pfddrmmtpextensions.h>
#include <ndtnetmsg.h>
#include <oemaes.h>
#include <drmmanager.h>
#include <drmviewrights.h>
#include <oemndt.h>
#include <drmndtasf.h>
#include <pfddrmcontextsizes.h>

#include "drmpfd.h"
#include "ndtmanager_internal.h"

ENTER_PK_NAMESPACE_CODE;

/**********************************************************************
**
** Function:    _PfdDrmManagerFreeContentSession
**
** Synopsis:    Releases any resources held by an open DRM content transfer session.
**
** Arguments:   [f_pContext]    - The PFD DRM manager context
**              [f_dwSessionId] - The identifier of the session to be freed
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**              DRM_E_FAIL if the resources couldn't be released
**
**********************************************************************/
DRM_RESULT _PfdDrmManagerFreeContentSession(
    __inout     PFDMGR_CONTEXT_INTERNAL *f_pContext,
    __in const  DRM_DWORD                f_dwSessionId
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    PFDMGR_CONTENT_SESSION_CONTEXT_INTERNAL *pContentSession = NULL;
    DRM_APP_CONTEXT *pAppContext;
    DRM_BOOL fFound = FALSE;
    DRM_DWORD i;
    DRM_BYTE *pbRevocationBuffer = NULL;
    DRM_DWORD cbRevocationBuffer = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__PfdDrmManagerFreeContentSession );

    ChkArg(f_pContext != NULL);

    for ( i = 0; i < f_pContext->ContentSessions.dwCurrentSize; i++ )
    {
        ChkDR( DRM_PtrList_GetByIndex( &f_pContext->ContentSessions, 
                                    i, 
                                   (DRM_VOID **)&pContentSession ) );
        if ( pContentSession->dwSessionID == f_dwSessionId )
        {
            ChkDR( DRM_PtrList_RemoveByIndex( &f_pContext->ContentSessions, 
                                           i, 
                                          (DRM_VOID **)&pContentSession ) );
            fFound = TRUE;
            break;
        }
    }
    
    if ( FALSE == fFound )
    {
        ChkDR( DRM_E_ND_INVALID_SESSION );
    }

    if( pContentSession->hRawFile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( pContentSession->hRawFile );
        pContentSession->hRawFile = OEM_INVALID_HANDLE_VALUE;
    }
    else
    {
        ChkDR( Drm_Ndt_Asf_Close( &f_pContext->ndtMgrContext, 
                                  pContentSession->idContentSession,
                                  &pAppContext ) );

        ZEROMEM( &pContentSession->idContentSession, DRM_ID_SIZE );
        pContentSession->dwSessionID = 0;

        if ( pAppContext != NULL ) 
        {
            dr = Drm_Revocation_GetBuffer( pAppContext,
                                           &pbRevocationBuffer,
                                           &cbRevocationBuffer );

            Drm_Uninitialize( pAppContext );

            if( DRM_SUCCEEDED( dr ) )
            {
                SAFE_OEM_FREE( pbRevocationBuffer );
            }
        }
    }
    SAFE_OEM_FREE( pContentSession );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


DRM_RESULT _PfdDrmManagerFreeAllContentSessions(
    __inout PFDMGR_CONTEXT_INTERNAL *f_pContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__PfdDrmManagerFreeAllContentSessions );

    for ( i = 0; i < f_pContext->ContentSessions.dwCurrentSize; i++ )
    {
        PFDMGR_CONTENT_SESSION_CONTEXT_INTERNAL *pContentSessionToFree = NULL;
        
        ChkDR( DRM_PtrList_GetByIndex( &f_pContext->ContentSessions, i, (DRM_VOID **)&pContentSessionToFree ) );
        _PfdDrmManagerFreeContentSession( f_pContext,
                                          pContentSessionToFree->dwSessionID );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


DRM_RESULT _PfdDrmManagerGetSession(
    __inout     PFDMGR_CONTEXT_INTERNAL                  *f_pContext,
    __in const  DRM_DWORD                                 f_dwSessionId,
    __out       PFDMGR_CONTENT_SESSION_CONTEXT_INTERNAL **f_pContentSession
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    PFDMGR_CONTENT_SESSION_CONTEXT_INTERNAL *pContentSession = NULL;
    DRM_BOOL fFound = FALSE;
    DRM_DWORD i;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__PfdDrmManagerGetSession );

    ChkArg(f_pContext != NULL);
    ChkArg(f_pContentSession != NULL);

    for ( i = 0; i < f_pContext->ContentSessions.dwCurrentSize; i++ )
    {
        ChkDR( DRM_PtrList_GetByIndex( &f_pContext->ContentSessions, i, (DRM_VOID **)&pContentSession ) );
        if ( pContentSession->dwSessionID == f_dwSessionId )
        {
            fFound = TRUE;
            break;
        }
    }
    
    if ( FALSE == fFound )
    {
        ChkDR( DRM_E_ND_INVALID_SESSION );
    }

    *f_pContentSession = pContentSession;

 ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;
    
