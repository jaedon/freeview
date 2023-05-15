/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_NDTMANAGER_INTERNAL_C
#include <drmtypes.h>
#include <ndttypes.h>
#include <ndtcontextsizes.h>
#include <ndtmanager_internal.h>
#include <ndtconstants.h>
#include <ndtasf_internal.h>
#include <ndtreg.h>
#include <ndtnetmsg.h>
#include <wmdrmndresults.h>
#include <oemdevstore.h>
#include <drmdevstore.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;


static DRM_RESULT _DecryptNonce(
    __in const   DRM_ID * const f_pSeed,
    __inout      DRM_ID        *f_pNonce
)
{
    DRM_RESULT   dr   = DRM_SUCCESS;
    DRM_AES_KEY *pKey = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__DecryptNonce );

    pKey = WmdrmNetGetRegistrationKey( f_pSeed, FALSE );

    if (pKey == NULL)
    {
        TRACE(("DecryptNonce : Failed to get content encryption key\n"));
        ChkDR( DRM_E_FAIL );
    }

    ChkDR( Oem_Aes_EcbDecryptData( pKey, (DRM_BYTE*)f_pNonce, DRM_ID_SIZE ) );

ErrorExit:
    if( pKey != NULL )
    {
        Oem_Aes_ZeroKey( pKey );
        SAFE_OEM_FREE( pKey );
    }
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    _NdtManagerFreeRegistrationSession
**
** Synopsis:    Releases any resources held by a registered receiver.
**
** Arguments:   [f_pRegSessionContext] -- The registration session context
**
** Returns:     None
**
**********************************************************************/
static DRM_VOID _NdtManagerFreeRegistrationSession(
    __inout NDTMGR_INTERNAL_REGISTRATIONSESSION_CONTEXT *f_pRegSessionContext
)
{
    if (f_pRegSessionContext)
    {
        f_pRegSessionContext->state = REG_STATE_INIT;
        SAFE_OEM_FREE(f_pRegSessionContext->DeviceID.pbBlob);
        SAFE_OEM_FREE(f_pRegSessionContext->pbProximityDetectionAddress );
        if ( NULL != f_pRegSessionContext->hDevice )
        {
            Oem_DeviceStore_FreeDevice( f_pRegSessionContext->hDevice );
            f_pRegSessionContext->hDevice = NULL;
        }

        /* Clear the certificate info. */
        OEM_SECURE_ZERO_MEMORY( f_pRegSessionContext, SIZEOF( NDTMGR_INTERNAL_REGISTRATIONSESSION_CONTEXT ) );
    }
}


WMDRMND_RESULT DRM_NDT_DRToWMDRMNDError(
    __in const DRM_RESULT f_dr
)
{
    WMDRMND_RESULT wr = WMDRMND_E_TRANSMITTERFAILURE;
    DRM_RESULT     dr = f_dr;   /* used in default clause macro */

    switch (dr)
    {
    case DRM_SUCCESS:
        wr = WMDRMND_SUCCESS;
        break;
    case DRM_E_INVALID_DEVICE_CERTIFICATE:
        wr = WMDRMND_E_INVALIDCERTIFICATE;
        break;
    case DRM_E_CERTIFICATE_REVOKED:
        wr = WMDRMND_E_CERTIFICATEREVOKED;
        break;
    case DRM_E_LICENSE_NOT_FOUND:
        wr = WMDRMND_E_LICENSEUNAVAILABLE;
        break;
    case DRM_E_ND_DEVICE_LIMIT_REACHED:
        wr = WMDRMND_E_DEVICELIMITREACHED;
        break;
    case DRM_E_ND_UNABLE_TO_VERIFY_PROXIMITY:
        wr = WMDRMND_E_UNABLETOVERIFYPROXIMITY;
        break;
    case DRM_E_DEVICE_NOT_REGISTERED:
        wr = WMDRMND_E_MUSTREGISTER;
        break;
    case DRM_E_ND_MUST_REVALIDATE:
        wr = WMDRMND_E_MUSTREVALIDATE;
        break;
    case DRM_E_ND_INVALID_PROXIMITY_RESPONSE:
        wr = WMDRMND_E_INVALIDPROXIMITYRESPONSE;
        break;
    case DRM_E_ND_INVALID_SESSION:
        wr = WMDRMND_E_INVALIDSESSION;
        break;
    case DRM_E_ND_BAD_REQUEST:
        wr = WMDRMND_E_BADREQUEST;
        break;

    default:
        TRACE_IF_FAILED(("Cannot map DRM_RESULT to WMDRMND Error code \n"));
        wr = WMDRMND_E_TRANSMITTERFAILURE;
    }

    return wr;
}



DRM_RESULT DRM_CALL GetWMDRMNETRevocationEntries(
    __in_bcount(f_cbCRL) DRM_BYTE             *f_pbCRL,
    __in const           DRM_DWORD             f_cbCRL,
    __out                DRM_DWORD            *f_pcEntries,
    __out                WMDRMNET_CRL_ENTRY  **f_ppEntries
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD dwVersion;
    DRM_DWORD cEntries;
    DRM_BYTE *pEntries = NULL;
    WMDRMNET_MESSAGE_DESCRIPTOR *MessageDescriptor = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_GetWMDRMNETRevocationEntries );

    ChkDR( WmdrmNetAllocateMessageDescriptor( f_pbCRL, f_cbCRL, &MessageDescriptor ) );

    if( !WmdrmNetUnpackDword( MessageDescriptor, &dwVersion ) )
    {
        TRACE(( "GetWMDRMNETRevocationEntries: CRL too short (Version)\n" ));
        ChkDR( DRM_E_FAIL );
    }

    if( !WmdrmNetUnpackDword( MessageDescriptor, &cEntries ) )
    {
        TRACE(( "GetWMDRMNETRevocationEntries: CRL too short (Entry Count)\n" ));
        ChkDR( DRM_E_FAIL );
    }

    if( !WmdrmNetUnpackOctets(MessageDescriptor, &pEntries, cEntries * WMDRMNET_CRL_ENTRY_SIZE) )
    {
        TRACE(("GetWMDRMNETRevocationEntries: CRL too short (Entries)\n"));
        ChkDR( DRM_E_FAIL );
    }

    *f_pcEntries = cEntries;
    *f_ppEntries = (WMDRMNET_CRL_ENTRY *)pEntries;
    pEntries = NULL;

ErrorExit:
    SAFE_OEM_FREE(pEntries);
    if(MessageDescriptor != NULL)
    {
        /*
        ** Pass in FALSE here because we do not parse the certificate that follows the CRL
        */
        WmdrmNetFreeMessageDescriptor( MessageDescriptor, FALSE );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/****************************************************************************/
DRM_RESULT GetWMDRMNDRevocationList(
    __in  NDTMGR_CONTEXT_INTERNAL                 *f_pContext,
    __deref_out_bcount(*f_pcbCRLBuffer) DRM_BYTE **f_ppbCRLBuffer,
    __out DRM_DWORD                               *f_pcbCRLBuffer,
    __out DRM_DWORD                               *f_pdwCRLVersion
)
{
    DRM_RESULT        dr = DRM_SUCCESS;
    DRM_DWORD         dwCRLVersion = 0;
    DRM_DWORD         cbCRLBuffer = 0;
    DRM_BYTE         *pbCRLBuffer = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_GetWMDRMNDRevocationList );

    ChkArg( f_pContext && f_ppbCRLBuffer && f_pcbCRLBuffer );

    cbCRLBuffer = 0;

    dr = Drm_Revocation_GetList( f_pContext->pAppContext,
                                 DRM_REVOCATION_TYPE_WMDRM_ND,
                                 NULL,
                                 &cbCRLBuffer,
                                 &dwCRLVersion );

    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbCRLBuffer = (DRM_BYTE*)Oem_MemAlloc(cbCRLBuffer) );
        dr = Drm_Revocation_GetList( f_pContext->pAppContext,
                                     DRM_REVOCATION_TYPE_WMDRM_ND,
                                     pbCRLBuffer,
                                     &cbCRLBuffer,
                                     &dwCRLVersion );
    }
    ChkDR( dr );

    if ( 0 == cbCRLBuffer )
    {
        *f_pcbCRLBuffer = 0;
        goto ErrorExit;
    }

    *f_ppbCRLBuffer = pbCRLBuffer;
    *f_pcbCRLBuffer = cbCRLBuffer;
    pbCRLBuffer = NULL;

    if( f_pdwCRLVersion )
    {
        *f_pdwCRLVersion = dwCRLVersion;
    }

ErrorExit:

    SAFE_OEM_FREE( pbCRLBuffer );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************/
DRM_VOID LicenseFreeKey(
  __in NDT_LICENSE_CONTEXT *f_pLicContext
)
{
    if ( NULL != f_pLicContext )
    {
        if ( NULL != f_pLicContext->pAESKeyContentEncryption )
        {
            Oem_Aes_ZeroKey(f_pLicContext->pAESKeyContentEncryption);
            SAFE_OEM_FREE( f_pLicContext->pAESKeyContentEncryption );
        }
    }
}

/**********************************************************************
**
** Function:    _NdtManagerFreeContentSession
**
** Synopsis:    Releases any resources held by an open DRM content transfer session.
**
** Arguments:   [f_pContentSesContext] -- The content session context
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**              DRM_E_FAIL if the resources couldn't be released
**
**********************************************************************/
static DRM_RESULT _NdtManagerFreeContentSession(
    __inout NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *f_pContentSesContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    NDT_ASF_CONTEXT *pAsfContext = NULL;
    NDT_LICENSE_CONTEXT *pLicenseContext = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__NdtManagerFreeContentSession );

    ChkArg( f_pContentSesContext != NULL );

    f_pContentSesContext->mediaSessionState  = MEDIA_STATE_CLOSE;
    (void) Oem_DeviceStore_FreeDevice( f_pContentSesContext->hDevice );
    f_pContentSesContext->hDevice = NULL;

    pAsfContext    = &(f_pContentSesContext->asfContext);

    switch ( f_pContentSesContext->licenseMode )
    {
    case NDT_LICENSE_MODE_SIMPLE:
        pLicenseContext = (NDT_LICENSE_CONTEXT *) f_pContentSesContext->pLicenseContext;
        LicenseFreeKey(pLicenseContext);
        break;
    case NDT_LICENSE_MODE_CHAINED:
        if ( NULL != f_pContentSesContext->pLicenseContext )
        {
            pLicenseContext = &(((NDT_CHAINED_LICENSE_CONTEXT *) f_pContentSesContext->pLicenseContext)->rootLicenseContext);
            LicenseFreeKey(pLicenseContext);
            pLicenseContext = &(((NDT_CHAINED_LICENSE_CONTEXT *) f_pContentSesContext->pLicenseContext)->leafLicenseContext);
            LicenseFreeKey(pLicenseContext);
        }
        break;
    }
    SAFE_OEM_FREE(f_pContentSesContext->pLicenseContext);

    NDTMGR_ASF_Internal_Uninitialize( pAsfContext );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
**
** Function:    NDTMGR_Internal_FindSession
**
** Synopsis:    Given a SessionID, finds the session in the NDTMGR
**              Context and returns a pointer to it.
**
** Arguments:   [f_pNdtMgrContext]   -- The NDT manager context
**              [f_idSession]        -- Session ID to look for
**              [f_SessionType       -- Session type to look for
**              [f_ppSessionContext] -- for returning a pointer to
**                                      the session
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**              DRM_E_ND_INVALID_SESSION if the session is not found
**
**********************************************************************/
DRM_RESULT NDTMGR_Internal_FindSession(
    __in        NDTMGR_CONTEXT                      *f_pNdtMgrContext,
    __in const  DRM_ID                               f_idSession,
    __in const  NDTMGR_SESSION_TYPE                  f_SessionType,
    __out       NDTMGR_INTERNAL_SESSION_CONTEXT    **f_ppSessionContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    NDTMGR_CONTEXT_INTERNAL *pContext = (NDTMGR_CONTEXT_INTERNAL*) f_pNdtMgrContext;
    NDTMGR_INTERNAL_SESSION_CONTEXT *pSession = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_NDTMGR_Internal_FindSession );

    ChkArg( f_pNdtMgrContext != NULL && f_ppSessionContext != NULL );

    for(pSession = pContext->pFirstSession; pSession; pSession = pSession->pNextContext)
    {
        if ( 0 == MEMCMP(&pSession->idSession, &f_idSession, DRM_ID_SIZE) )
        {
            break;
        }
    }
    if(NULL == pSession || f_SessionType != pSession->sessionType)
    {
        ChkDR( DRM_E_ND_INVALID_SESSION );
    }
    *f_ppSessionContext = pSession;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
**
** Function:    NDTMGR_Internal_CreateSession
**
** Synopsis:    Given a Session Type, creats a new session in the NDTMGR
**              Context and returns a pointer to it.
**
** Arguments:   [f_pOEMContext]      -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext]   -- The NDT manager context
**              [f_sessionType]      -- Session Type to Create
**              [f_pAppContext]      -- For CONTENT sessions, supplies a bound
**                                      app context for retrieving license data.
**                                      This context may *not* be the same used
**                                      in Drm_Ndt_Initialize and must be
**                                      different for each CONTENT session. It
**                                      can be uninitialized after
**                                      Drm_Ndt_Session_Close is called.
**              [f_ppSessionContext] -- for returning a pointer to the session
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**              DRM_E_ND_INVALID_SESSION if the session is not found
**
**********************************************************************/
DRM_RESULT NDTMGR_Internal_CreateSession(
    __in_opt    DRM_VOID                            *f_pOEMContext,
    __in        NDTMGR_CONTEXT                      *f_pNdtMgrContext,
    __in const  NDTMGR_SESSION_TYPE                  f_sessionType,
    __in_opt    DRM_APP_CONTEXT                     *f_pAppContext,
    __out       NDTMGR_INTERNAL_SESSION_CONTEXT    **f_ppSessionContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    NDTMGR_CONTEXT_INTERNAL *pContext = (NDTMGR_CONTEXT_INTERNAL*) f_pNdtMgrContext;
    NDTMGR_INTERNAL_SESSION_CONTEXT *pSession = NULL;
    NDTMGR_INTERNAL_SESSION_CONTEXT *pTmpSession = NULL;
    const NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *pContentSession;
    DRM_BOOL fAlreadyExists;
    DRM_DWORD cbSession = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_NDTMGR_Internal_CreateSession );

    ChkArg( f_pNdtMgrContext != NULL && f_ppSessionContext != NULL );
    ChkArg( f_sessionType == NDTMGR_SESSION_TYPE_REGISTRATION ||
            f_pAppContext != NULL );

    switch(f_sessionType)
    {
    case NDTMGR_SESSION_TYPE_REGISTRATION:
        cbSession = SIZEOF( NDTMGR_INTERNAL_REGISTRATIONSESSION_CONTEXT );
        break;
    case NDTMGR_SESSION_TYPE_CONTENT:
        /* Ensure the app context isn't already being used */
        if ( f_pAppContext == pContext->pAppContext)
        {
            ChkDR( DRM_E_INVALIDARG );
        }

        for( pSession = pContext->pFirstSession;
             pSession != NULL;
             pSession = pSession->pNextContext )
        {
            if ( pSession->sessionType == NDTMGR_SESSION_TYPE_CONTENT)
            {
                pContentSession = (const NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *) pSession;

                if ( f_pAppContext == pContentSession->pAppContext )
                {
                    ChkDR( DRM_E_INVALIDARG );
                }
            }
        }

        cbSession = SIZEOF( NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT );
        break;
    default:
        ChkDR( DRM_E_INVALIDARG );
    }

    ChkMem( pSession = (NDTMGR_INTERNAL_SESSION_CONTEXT*)Oem_MemAlloc( cbSession ) );

    /* Initialize the structure */
    ZEROMEM( pSession, cbSession );
    pSession->sessionType = f_sessionType;
    switch(f_sessionType)
    {
    case NDTMGR_SESSION_TYPE_REGISTRATION:
        ((NDTMGR_INTERNAL_REGISTRATIONSESSION_CONTEXT *) pSession)->wResult = WMDRMND_E_MUSTREVALIDATE;
        break;
    case NDTMGR_SESSION_TYPE_CONTENT:
        ((NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *) pSession)->asfContext.hInFile = OEM_INVALID_HANDLE_VALUE;
        ((NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *) pSession)->pAppContext = f_pAppContext;
        break;
    default:
        ChkDR( DRM_E_INVALIDARG );
    }

    /* Ensure session ID is unique */
    do
    {
        Oem_Random_GetBytes( f_pOEMContext, (DRM_BYTE *) &pSession->idSession, DRM_ID_SIZE );

        fAlreadyExists = ( ( DRM_SUCCESS == NDTMGR_Internal_FindSession(f_pNdtMgrContext,
                                                                      pSession->idSession,
                                                                      NDTMGR_SESSION_TYPE_REGISTRATION,
                                                                      &pTmpSession) ) ||
                            ( DRM_SUCCESS == NDTMGR_Internal_FindSession(f_pNdtMgrContext,
                                                                      pSession->idSession,
                                                                      NDTMGR_SESSION_TYPE_CONTENT,
                                                                      &pTmpSession) ) );
    } while ( 0 == MEMCMP(&pSession->idSession, &g_idNULLSessionID, DRM_ID_SIZE)
              || fAlreadyExists );

    /* Add the session into the list of sessions */
    if(pContext->pFirstSession != NULL)
    {
        pContext->pFirstSession->pPrevContext = pSession;
    }
    pSession->pNextContext = pContext->pFirstSession;
    pSession->pPrevContext = NULL;
    pContext->pFirstSession = pSession;

    *f_ppSessionContext = pSession;

    pSession = NULL;

ErrorExit:

    SAFE_OEM_FREE( pSession );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
**
** Function:    NDTMGR_Internal_RemoveSession
**
** Synopsis:    Given a Session Context, removes the session from the
**              NDTMGR Context and frees memory associated with the
**              session.
**
** Arguments:   [f_pNdtMgrContext]   -- The NDT manager context
**              [f_pSessionContext]  -- for returning a pointer to
**                                      the session
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**              DRM_E_ND_INVALID_SESSION if the session is not found
**
**********************************************************************/
DRM_RESULT NDTMGR_Internal_RemoveSession(
    __in     NDTMGR_CONTEXT                      *f_pNdtMgrContext,
    __in     NDTMGR_INTERNAL_SESSION_CONTEXT     *f_pSessionContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    NDTMGR_CONTEXT_INTERNAL *pContext = (NDTMGR_CONTEXT_INTERNAL*) f_pNdtMgrContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_NDTMGR_Internal_RemoveSession );

    ChkArg( f_pNdtMgrContext != NULL && f_pSessionContext != NULL);

    if(NULL == f_pSessionContext->pPrevContext)
    {
        pContext->pFirstSession = f_pSessionContext->pNextContext;
    }
    else
    {
        f_pSessionContext->pPrevContext->pNextContext = f_pSessionContext->pNextContext;
    }
    if(NULL != f_pSessionContext->pNextContext)
    {
        f_pSessionContext->pNextContext->pPrevContext = f_pSessionContext->pPrevContext;
    }

    /* Do session type-specific stuff here */
    switch(f_pSessionContext->sessionType)
    {
    case NDTMGR_SESSION_TYPE_REGISTRATION:
         _NdtManagerFreeRegistrationSession( (NDTMGR_INTERNAL_REGISTRATIONSESSION_CONTEXT *) f_pSessionContext );
        break;
    case NDTMGR_SESSION_TYPE_CONTENT:
        (void) _NdtManagerFreeContentSession( (NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *) f_pSessionContext );
        break;
    }

    Oem_MemFree(f_pSessionContext);
    f_pSessionContext = NULL;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
**
** Function:    DRM_NDT_Proximity_VerifyEncryptedChallenge
**
** Synopsis:    This call will verify that the input encrypted challenge
**              matches the challenge (from the context), encrypted using
**              the key derived from the seed sent in the registration
**              response (see spec (1) for details). If there is a match,
**              the function will check if the proximity RTT time is less
**              than 7 ms. If it is, then it will invoke
**              DRM_DeviceStore_MarkDeviceAsValid.
**              This call must be used by NDTMGR_ProcessProximityResponseMsg
**              to verify that the encrypted challenge in the proximity
**              response message is correct.
**
** Arguments:   [f_pOEMContext]           -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext]        -- The NDT manager context
**              [f_idRegistrationSession] -- Registration ID
**              [f_EncryptedChallenge]    -- Encrypted
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_ND_INVALID_SESSION if the transaction ID is invalid or
**              DRM_E_DEVICE_NOT_REGISTERED if the session has timed out
**
**********************************************************************/
DRM_RESULT DRM_NDT_Proximity_VerifyEncryptedChallenge(
    __in_opt   DRM_VOID                 *f_pOEMContext,
    __in       NDTMGR_CONTEXT           *f_pNdtMgrContext,
    __in const DRM_ID                    f_idRegistrationSession,
    __in       DRM_ID                    f_EncryptedChallenge
)
{
    DRM_RESULT                                   dr = DRM_SUCCESS;
    NDTMGR_INTERNAL_REGISTRATIONSESSION_CONTEXT *pRegistrationContext = NULL;
    DRM_DWORD                                    dwProximityTime;
    DRM_DWORD                                    dwNow;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_NDT_Proximity_VerifyEncryptedChallenge );

    ChkArg( f_pNdtMgrContext );

    /* Complete proximity measurement */
    dwNow = Oem_Clock_GetTickCount( f_pOEMContext );

    ChkDR( NDTMGR_Internal_FindSession(f_pNdtMgrContext,
                                       f_idRegistrationSession,
                                       NDTMGR_SESSION_TYPE_REGISTRATION,
                                       (NDTMGR_INTERNAL_SESSION_CONTEXT **) &pRegistrationContext) );

    /* Verify state for that registration transaction ID */
    if ( pRegistrationContext->state < REG_STATE_PROXIMITY_CHALLENGE_SENT
         || pRegistrationContext->state == REG_STATE_DONE )
    {
        TRACE(( "DRM_NDT_Proximity_VerifyEncryptedChallenge: invalid state %d\n", pRegistrationContext->state ));
        ChkDR( DRM_E_DEVICE_NOT_REGISTERED );
    }

    ChkDR( _DecryptNonce( &pRegistrationContext->idSeed, &f_EncryptedChallenge ) );

    if ( MEMCMP( &(pRegistrationContext->idNonce), &f_EncryptedChallenge, DRM_ID_SIZE ) != 0 )
    {
        /* If the nonce is wrong then restart the registration process. */
        TRACE(( "DRM_NDT_Proximity_VerifyEncryptedChallenge: nonce challenge failed\n" ));
        pRegistrationContext->wResult = DRM_NDT_DRToWMDRMNDError( DRM_E_ND_INVALID_PROXIMITY_RESPONSE );
        pRegistrationContext->state = REG_STATE_DONE;
        ChkDR( DRM_E_ND_INVALID_PROXIMITY_RESPONSE );
    }

    /* Calculate proximity measurement and store it if lower the previous measurements. */
    dwProximityTime = dwNow - pRegistrationContext->dwProximityStartTime;

    /* Verify proximity measurement is lower than threshold. */
    if ( PROXIMITY_RTT_THRESHOLD_MS < dwProximityTime )
    {
        TRACE(( "DRM_NDT_Proximity_VerifyEncryptedChallenge: proximity threshold exceeded: %ld ms\n", dwProximityTime ));
        pRegistrationContext->state = REG_STATE_PROCESSED_REGISTRATION_REQUEST;
        ChkDR( DRM_E_ND_UNABLE_TO_VERIFY_PROXIMITY );
    }

    /* Update context. */
    ChkDR( DRM_DeviceStore_MarkDeviceAsValid( f_pOEMContext, pRegistrationContext->hDevice ) );

    pRegistrationContext->state = REG_STATE_PROXIMITY_VERIFIED;

 ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    DRM_NDT_Proximity_GetChallenge
**
** Synopsis:    Initiates the proximity challenge phase of registration.
**              A 128-bit nonce is generated and returned through
**              the f_pChallenge pointer
**
** Arguments:   [f_pOEMContext]           -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext]        -- The NDT manager context
**              [f_idRegistrationSession] -- Registration transaction ID
**              [f_pChallenge]            -- Nonce
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_ND_INVALID_SESSION if the transaction ID is invalid or
**              DRM_E_DEVICE_NOT_REGISTERED if the session has timed out
**
**********************************************************************/
DRM_RESULT DRM_NDT_Proximity_GetChallenge(
    __in_opt   DRM_VOID                 *f_pOEMContext,
    __in       NDTMGR_CONTEXT           *f_pNdtMgrContext,
    __in const DRM_ID                    f_idRegistrationSession,
    __out      DRM_ID                   *f_pChallenge
)
{
    DRM_RESULT                                   dr = DRM_SUCCESS;
    NDTMGR_INTERNAL_REGISTRATIONSESSION_CONTEXT *pRegistrationContext = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_NDT_Proximity_GetChallenge );

    ChkArg( f_pNdtMgrContext
            && f_pChallenge );

    ChkDR( NDTMGR_Internal_FindSession(f_pNdtMgrContext,
                                       f_idRegistrationSession,
                                       NDTMGR_SESSION_TYPE_REGISTRATION,
                                       (NDTMGR_INTERNAL_SESSION_CONTEXT **) &pRegistrationContext) );

    /* Check for timeout */
    ChkBOOL( !DRM_DeviceStore_HasTimedOut( f_pOEMContext,
                                           &pRegistrationContext->ftRegistrationStartTime,
                                           NDT_REGISTRATION_TIMEOUT ), DRM_E_DEVICE_NOT_REGISTERED );

    /* Verify state for that registration transaction ID (is the receiver registered yet?) */
    if ( pRegistrationContext->state < REG_STATE_PROCESSED_REGISTRATION_REQUEST
         || pRegistrationContext->state == REG_STATE_DONE )
    {
        TRACE_IF_FAILED(("DRM_NDT_Proximity_GetChallenge: invalid state %d\n",pRegistrationContext->state));
        ChkDR( DRM_E_DEVICE_NOT_REGISTERED );
    }

    /* Generate Nonce and store it for later comparison */
    ChkDR( Oem_Random_GetBytes( f_pOEMContext,
                                (DRM_BYTE *) &pRegistrationContext->idNonce,
                                SIZEOF( pRegistrationContext->idNonce ) ) );

    MEMCPY(f_pChallenge, &pRegistrationContext->idNonce, DRM_ID_SIZE);

    /* Update context */
    pRegistrationContext->state = REG_STATE_PROXIMITY_CHALLENGE_SENT;

    /* Start proximity measurement */
    pRegistrationContext->dwProximityStartTime = Oem_Clock_GetTickCount( f_pOEMContext );

 ErrorExit:
    TRACE_IF_FAILED(("DRM_NDT_Proximity_GetChallenge failed 0x%lx\n",dr));
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

