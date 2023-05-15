/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMNDTMANAGER_C
#include <ndttypes.h>
#include <ndtcontextsizes.h>
#include <drmndtmanager.h>
#include <ndtmanager_internal.h>
#include <ndtreg.h>
#include <ndtlicense.h>
#include <ndtnetmsg.h>
#include <oemdevstore.h>
#include <drmdevstore.h>
#include <ndtconstants.h>
#include <oemaes.h>
#include <ndtrevguids.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

DRM_DEFINE_GUID( GUID_WMDRMRevocationTypeApp,
                 0x90a37313, 0x0ecf, 0x4caa, 0xa9, 0x06, 0xb1, 0x88, 0xf6, 0x12, 0x93, 0x00 );

DRM_DEFINE_GUID( GUID_WMDRMRevocationTypeWmdrmpd,
                 0x3129e375, 0xceb0, 0x47d5, 0x9c, 0xca, 0x9d, 0xb7, 0x4c, 0xfd, 0x43, 0x32 );

DRM_DEFINE_GUID( GUID_WMDRMRevocationTypeWmdrmnd,
                 0xcd75e604, 0x543d, 0x4a9c, 0x9f, 0x09, 0xfe, 0x6d, 0x24, 0xe8, 0xbf, 0x90 );

DRM_DEFINE_GUID( GUID_WMDRMRevocationTypeRiv,
                 0xccde5a55, 0xa688, 0x4405, 0xa8, 0x8b, 0xd1, 0x3f, 0x90, 0xd5, 0xba, 0x3e );

ENTER_PK_NAMESPACE_CODE;

/**********************************************************************
**
** Function:    Drm_Ndt_Initialize
**
** Synopsis:    Initializes the NDT manager.  The context initialized with this
**              function is then used in subsequent NDT manager calls.
**
** Arguments:   [f_pNdtMgrContext]  -- The NDT DRM manager context
**              [f_pAppContext]     -- An initialized app context (i.e. one
**                                     already used in a Drm_Initialize call).
**                                     It is used to provide an oem context
**                                     pointer, an HDS, and revocation support.
**                                     It may not be freed until
**                                     Drm_Ndt_Uninitialize is called. Note
**                                     that a revocation buffer of at least
**                                     REVOCATION_BUFFER_SIZE must already have
**                                     been set.
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_Initialize(
    __out    NDTMGR_CONTEXT             *f_pNdtMgrContext,
    __in     DRM_APP_CONTEXT            *f_pAppContext
)
{
    DRM_RESULT               dr = DRM_SUCCESS;
    NDTMGR_CONTEXT_INTERNAL *pContext = (NDTMGR_CONTEXT_INTERNAL*) f_pNdtMgrContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_Initialize );

    ChkArg( f_pNdtMgrContext != NULL );
    ZEROMEM( pContext, SIZEOF( NDTMGR_CONTEXT ) );

    pContext->pAppContext = f_pAppContext;

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        DRM_BYTE                *pbRevocationBuffer = NULL;
        DRM_DWORD                cbRevocationBuffer;
        ChkDR( Drm_Revocation_GetBuffer( pContext->pAppContext,
                                         &pbRevocationBuffer,
                                         &cbRevocationBuffer ) );

        if (( pbRevocationBuffer == NULL ) ||
            ( cbRevocationBuffer < REVOCATION_BUFFER_SIZE ))
        {
            ChkDR( DRM_E_REVOCATION_BUFFER_TOO_SMALL );
        }
    }

    ChkDR( Oem_DeviceStore_Initialize( &pContext->deviceStoreContext,
                                       (OEM_DEVICESTORE_INITDATA *) pContext->pAppContext ) );

    ChkDR( Oem_DeviceStore_Initialize( &pContext->upperLayerDeviceStoreContext,
                                       (OEM_DEVICESTORE_INITDATA *) pContext->pAppContext ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
**
** Function:    Drm_Ndt_Uninitialize
**
** Synopsis:    Releases any resources held by the NDT manager.
**
** Arguments:   [f_pNdtMgrContext] -- The NDT manager context initialized by
**                                    Drm_Ndt_Initialize. After this call,
**                                    the app context passed to
**                                    Drm_Ndt_Initialize will no longer be used.
**
** Returns:     None.
**
**********************************************************************/
DRM_API DRM_VOID DRM_CALL Drm_Ndt_Uninitialize(
    __in     NDTMGR_CONTEXT             *f_pNdtMgrContext
)
{
    DRM_RESULT                       dr = DRM_SUCCESS;
    NDTMGR_CONTEXT_INTERNAL         *pContext = (NDTMGR_CONTEXT_INTERNAL*) f_pNdtMgrContext;

    ChkArg( f_pNdtMgrContext != NULL );

    if ( NULL != pContext->certificateInfo.pDevicePublicKey )
    {
        OEM_RSA_ZeroPublicKey_2048BIT( pContext->certificateInfo.pDevicePublicKey );
        SAFE_OEM_FREE( pContext->certificateInfo.pDevicePublicKey );
    }
    ZEROMEM( &pContext->certificateInfo, SIZEOF(pContext->certificateInfo) );

    Oem_DeviceStore_Uninitialize( &pContext->deviceStoreContext );
    Oem_DeviceStore_Uninitialize( &pContext->upperLayerDeviceStoreContext );

    ZEROMEM( pContext, SIZEOF( NDTMGR_CONTEXT_INTERNAL ) );

ErrorExit:
    TRACE_IF_FAILED(("Drm_Ndt_Uninitialize failed 0x%lx\n",dr));

    return;
}


/**********************************************************************
**
** Function:    Drm_Ndt_Session_Create
**
** Synopsis:    Creates an NDT session
**
** Arguments:   [f_pOEMContext]    -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext] -- The NDT manager context initialized by Drm_Ndt_Initialize
**              [f_eSessionType]   -- Type of session to create
**              [f_pAppContext]    -- For CONTENT sessions, supplies a bound
**                                    app context for retrieving license data.
**                                    This context may *not* be the same used
**                                    in Drm_Ndt_Initialize and must be
**                                    different for each CONTENT session. It
**                                    can be uninitialized after
**                                    Drm_Ndt_Session_Close is called.
**              [f_pID]            -- Pointer to receive session ID
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_Session_Create(
    __in_opt    DRM_VOID                *f_pOEMContext,
    __in        NDTMGR_CONTEXT          *f_pNdtMgrContext,
    __in const  NDTMGR_SESSION_TYPE      f_eSessionType,
    __in_opt    DRM_APP_CONTEXT         *f_pAppContext,
    __out       DRM_ID                  *f_pID
)
{
    DRM_RESULT                       dr = DRM_SUCCESS;
    NDTMGR_INTERNAL_SESSION_CONTEXT *pSessionContext = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_Session_Create );

    ChkArg( f_pNdtMgrContext != NULL );
    ChkArg( (f_eSessionType == NDTMGR_SESSION_TYPE_REGISTRATION) ==
            (f_pAppContext == NULL) );

    ChkDR( NDTMGR_Internal_CreateSession( f_pOEMContext,
                                          f_pNdtMgrContext,
                                          f_eSessionType,
                                          f_pAppContext,
                                          &pSessionContext ) );

    MEMCPY( f_pID, &pSessionContext->idSession, DRM_ID_SIZE );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    Drm_Ndt_Session_Close
**
** Synopsis:    Releases any resources held by an NDT session
**
** Arguments:   [f_pNdtMgrContext] -- The NDT manager context initialized by Drm_Ndt_Initialize
**              [f_idSession]      -- The ID of the NDT session to close
**              [f_ppAppContext]   -- Optional pointer that receives the app context used to create the session
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_Session_Close(
    __in        NDTMGR_CONTEXT          *f_pNdtMgrContext,
    __in const  DRM_ID                   f_idSession,
    __out_opt   DRM_APP_CONTEXT        **f_ppAppContext
)
{
    DRM_RESULT                              dr = DRM_SUCCESS;
    NDTMGR_CONTEXT_INTERNAL                *pContext = (NDTMGR_CONTEXT_INTERNAL *) f_pNdtMgrContext;
    DRM_APP_CONTEXT                        *pAppContext = NULL;
    NDTMGR_INTERNAL_SESSION_CONTEXT        *pSession;
    NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *pContentSession;
    DRM_BYTE                               *pbDeletedDeviceID = NULL;
    DRM_DWORD                               cbDeletedDeviceID = 0;
    DRM_BYTE                               *pbOtherDeviceID = NULL;
    DRM_DWORD                               cbOtherDeviceID = 0;
    DRM_BOOL                                fFound = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_Session_Close );

    ChkArg( f_pNdtMgrContext != NULL );

    /* Look up session to delete */
    dr = NDTMGR_Internal_FindSession( f_pNdtMgrContext, f_idSession, NDTMGR_SESSION_TYPE_CONTENT, &pSession );
    if(dr == DRM_E_ND_INVALID_SESSION)
    {
        dr = NDTMGR_Internal_FindSession( f_pNdtMgrContext, f_idSession, NDTMGR_SESSION_TYPE_REGISTRATION, &pSession );
    }
    ChkDR( dr );

    /* If it is a content session, record the device id for later comparison */
    if ( NDTMGR_SESSION_TYPE_CONTENT == pSession->sessionType )
    {
        pContentSession = (NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *) pSession;
        if ( NULL != pContentSession->hDevice )
        {
            dr = Oem_DeviceStore_GetDeviceID(pContentSession->hDevice, pbDeletedDeviceID, &cbDeletedDeviceID);
            if ( DRM_E_BUFFERTOOSMALL == dr )
            {
                pbDeletedDeviceID = (DRM_BYTE *) Oem_MemAlloc(cbDeletedDeviceID);
                ChkMem( pbDeletedDeviceID );
                dr = Oem_DeviceStore_GetDeviceID(pContentSession->hDevice, pbDeletedDeviceID, &cbDeletedDeviceID);
            }
            ChkDR( dr );
        }

        pAppContext = pContentSession->pAppContext;
    }

    /* Remove the session */
    ChkDR( NDTMGR_Internal_RemoveSession( f_pNdtMgrContext, pSession ) );

    if ( f_ppAppContext != NULL )
    {
        *f_ppAppContext = pAppContext;
    }

    if( NULL != pbDeletedDeviceID )
    {
        /* Determine of the global opened count must be decremented.
         * To do this, see if any other sessions have the same DeviceID.
         * if no sessions have the same DeviceID, then decrement the
         * count
         */
        for( pSession = pContext->pFirstSession; pSession; pSession = pSession->pNextContext )
        {
            if ( NDTMGR_SESSION_TYPE_CONTENT == pSession->sessionType )
            {
                pContentSession = (NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *) pSession;
                if ( NULL != pContentSession->hDevice )
                {
                    dr = Oem_DeviceStore_GetDeviceID(pContentSession->hDevice, pbOtherDeviceID, &cbOtherDeviceID);
                    if ( DRM_E_BUFFERTOOSMALL == dr )
                    {
                        pbOtherDeviceID = (DRM_BYTE *) Oem_MemAlloc(cbOtherDeviceID);
                        ChkMem( pbOtherDeviceID );
                        dr = Oem_DeviceStore_GetDeviceID(pContentSession->hDevice, pbOtherDeviceID, &cbOtherDeviceID);
                    }
                    ChkDR( dr );

                    if ( cbDeletedDeviceID == cbOtherDeviceID
                         && 0 == MEMCMP(pbDeletedDeviceID, pbOtherDeviceID, cbDeletedDeviceID) )
                    {
                        fFound = TRUE;
                        break;
                    }
                    SAFE_OEM_FREE(pbOtherDeviceID);
                }
            }
        }
        if ( FALSE == fFound )
        {
            pContext->dwOpenedCount--;
            TRACE_IF_FAILED(("Global Open Count decremented\n"));
        }
    }
ErrorExit:
    SAFE_OEM_FREE(pbDeletedDeviceID);
    SAFE_OEM_FREE(pbOtherDeviceID);

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
**
** Function:    Drm_Ndt_Registration_ProcessRequest
**
** Synopsis:    Use the session ID to locate the registration session
**              and verify that the state of the session is correct
**              for processing this registration request.
**              Parse the registration request to extract the
**              certificate and serial number.
**              Verify the certificate.
**              Check if the device is registered and if so get the
**              device handle.
**              If the device is not registered already, invoke the
**              OEM_NDT_AllowNewDeviceRegistration. If this API indicates
**              that the device should be registered, register the new
**              device and get the device handle.
**              Generate the registration response message.
**
** Arguments:   [f_pOEMContext]             -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext]          -- The NDT manager context
**              [f_idRegistrationSession]   -- Session ID
**              [f_RegistrationRequest]     -- request message
**              [f_pbRegistrationResponse]  -- caller allocated buffer
**              [f_pcbRegistrationResponse] -- on input: size of buffer
**                                             on output size of message
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_ND_BAD_REQUEST if the message can't be parsed or
**              DRM_E_INVALID_DEVICE_CERTIFICATE if the certificate in the registration message is invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_Registration_ProcessRequest(
    __in_opt DRM_VOID                   *f_pOEMContext,
    __in     NDTMGR_CONTEXT             *f_pNdtMgrContext,
    __in     DRM_ID                      f_idRegistrationSession,
    __in     DRM_BYTEBLOB                f_RegistrationRequest,
    __out_bcount_opt(*f_pcbRegistrationResponse) DRM_BYTE *f_pbRegistrationResponse,
    __inout  DRM_DWORD                  *f_pcbRegistrationResponse
)
{
    DRM_RESULT                                   dr = DRM_SUCCESS;
    DRM_ID                                       idSerial;
    DRM_BYTE                                    *pbDeviceCertificate = NULL;
    DRM_DWORD                                    cbDeviceCertificate;
    DRM_SUBSTRING                                dasstrLeafCertificate;
    NDT_CERTIFICATE_INFO                         certificateInfo = CERTIFICATE_INFO_INITIALIZER;
    NDTMGR_CONTEXT_INTERNAL                     *pContext = (NDTMGR_CONTEXT_INTERNAL*) f_pNdtMgrContext;
    DRM_BYTEBLOB                                 DeviceID = {NULL, 0};
    NDTMGR_INTERNAL_REGISTRATIONSESSION_CONTEXT *pRegSessionContext = NULL;
    DRM_WORD                                     cbAddress = 0;
    DRM_SHA_CONTEXT                              contextSHA;
    DRM_BYTE                                     rgbReceiverIdentityHash[__CB_DECL(DRM_SHA1_DIGEST_LEN)];
    DRM_BOOL                                     fBypassValidation = FALSE;
    DRM_DWORD                                    dwCRLVersion = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_Registration_ProcessRequest );

    ChkArg( f_pNdtMgrContext               != NULL
         && f_RegistrationRequest.pbBlob   != NULL
         && f_pcbRegistrationResponse      != NULL
         && (*f_pcbRegistrationResponse == 0
             || f_pbRegistrationResponse != NULL) );

    /* Use the Session ID to locate the registration session and verify the state
     */
    ChkDR( NDTMGR_Internal_FindSession( f_pNdtMgrContext, f_idRegistrationSession, NDTMGR_SESSION_TYPE_REGISTRATION, (NDTMGR_INTERNAL_SESSION_CONTEXT **)&pRegSessionContext ) );
    ChkBOOL( REG_STATE_INIT == pRegSessionContext->state, DRM_E_DEVICE_ALREADY_REGISTERED );

    /* Parse the registration request to extract the certificate and serial number
     */
    if (DRM_FAILED( WmdrmNetUnpackRegistrationRequestMessage(
                f_RegistrationRequest.pbBlob,
                f_RegistrationRequest.cbBlob,
                &idSerial,
                &pbDeviceCertificate,
                &cbDeviceCertificate ) ) )
    {
        /* Either DRM_E_ND_INVALID_MESSAGE, DRM_E_ND_INVALID_MESSAGE_VERSION, or DRM_E_ND_INVALID_MESSAGE_TYPE. */
        TRACE_IF_FAILED(("Drm_Ndt_Registration_ProcessRequest: Invalid Registration Request Message: 0x%lx\n",dr));
        ChkDR( DRM_E_ND_BAD_REQUEST );
    }

    /* Verify the certificate.  Verfication includes checking the WMDRM-ND
     * revocation list.
     */

    /*
    ** We skip the certificate validation if we receive a certificate/serial number
    ** combo we already know about.  To determine if we already know about it,
    ** a SHA-1 hash of the serial number concatenated with the certificate is taken.
    */

    DRM_SHA_Init    ( &contextSHA, eDRM_SHA_1 );
    DRM_SHA_Update  ( (const DRM_BYTE *) &idSerial, DRM_ID_SIZE, &contextSHA );
    DRM_SHA_Update  ( pbDeviceCertificate, cbDeviceCertificate, &contextSHA );
    DRM_SHA_Finalize( &contextSHA, DRM_SHA1_DIGEST_LEN, rgbReceiverIdentityHash );

    if ( 0 == MEMCMP( rgbReceiverIdentityHash, pContext->certificateInfo.rgbReceiverIdentityHash, DRM_SHA1_DIGEST_LEN ) )
    {

        ChkDR( Drm_Revocation_GetList( pContext->pAppContext,
                                       DRM_REVOCATION_TYPE_WMDRM_ND,
                                       NULL,
                                       NULL,
                                       &dwCRLVersion ) );

        if( pContext->dwCRLVersion == dwCRLVersion )
        {
            MEMCPY( &certificateInfo, &pContext->certificateInfo, SIZEOF(certificateInfo) );
            fBypassValidation = TRUE;
        }
    }

    if ( FALSE == fBypassValidation )
    {
        dr = NDT_ValidateCertificate(pContext,
                                     (const DRM_CHAR*)pbDeviceCertificate,
                                     cbDeviceCertificate,
                                     &dasstrLeafCertificate,
                                     &certificateInfo,
                                     NULL,
                                     NULL,
                                     &dwCRLVersion);
        if ( DRM_E_CERTIFICATE_REVOKED == dr )
        {
            ChkDR(dr);
        }
        else if ( DRM_FAILED(dr) )
        {
            /* Registration request failed, so send back the appropriate error code. */
            TRACE_IF_FAILED(("Drm_Ndt_Registration_ProcessRequest: Cert Validation failed 0x%lx\n",dr));
            ChkDR( DRM_E_INVALID_DEVICE_CERTIFICATE );
        }
        pContext->dwCRLVersion = dwCRLVersion;
    }

    /* Check if the device is registered.  The Device ID for the device store
     * is the serial number concatenated with the certificate.
     */
    ChkOverflow( DRM_ID_SIZE + cbDeviceCertificate, cbDeviceCertificate );
    DeviceID.cbBlob = DRM_ID_SIZE + cbDeviceCertificate;
    DeviceID.pbBlob = (DRM_BYTE *) Oem_MemAlloc(DeviceID.cbBlob);
    ChkMem( DeviceID.pbBlob );
    DRM_BYT_CopyBytes(DeviceID.pbBlob, 0, &idSerial, 0, DRM_ID_SIZE);
    DRM_BYT_CopyBytes(DeviceID.pbBlob, DRM_ID_SIZE, pbDeviceCertificate, 0, cbDeviceCertificate);

    /* Make sure we don't leak hDevice if this is the second of a two-pass call. */
    if( pRegSessionContext->hDevice != NULL )
    {
        Oem_DeviceStore_FreeDevice( pRegSessionContext->hDevice );
        pRegSessionContext->hDevice = NULL;
    }

    /* If GetDeviceByID is successful, the device is already registered */
    dr = Oem_DeviceStore_GetDeviceByID ( &pContext->deviceStoreContext, DeviceID, &pRegSessionContext->hDevice );
    if ( dr == DRM_E_DEVICE_NOT_REGISTERED )
    {
        DRM_BYTEBLOB blobCertificate;
        DRM_BYTEBLOB blobSerial;

        blobCertificate.pbBlob = pbDeviceCertificate;
        blobCertificate.cbBlob = cbDeviceCertificate;
        blobSerial.pbBlob = (DRM_BYTE *) &idSerial;
        blobSerial.cbBlob = DRM_ID_SIZE;
        ChkDR( Oem_Ndt_AllowNewDeviceRegistration(blobCertificate, blobSerial) );
        ChkDR( Oem_DeviceStore_RegisterDevice( &pContext->deviceStoreContext, DeviceID, &pRegSessionContext->hDevice ) );
    }
    ChkDR( dr );


    /* Registration passed, update context
     */
    pRegSessionContext->idSerial = idSerial;
    Oem_Random_GetBytes( f_pOEMContext,
                         pRegSessionContext->idSeed.rgb,
                         DRM_ID_SIZE );

    dr = Oem_Ndt_GetProximityDetectionAddress( NULL, &cbAddress );
    if( DRM_E_BUFFERTOOSMALL == dr )
    {
        SAFE_OEM_FREE(pRegSessionContext->pbProximityDetectionAddress);
        pRegSessionContext->pbProximityDetectionAddress = (DRM_BYTE*)Oem_MemAlloc( cbAddress );
        ChkMem(pRegSessionContext->pbProximityDetectionAddress);
        ChkDR( Oem_Ndt_GetProximityDetectionAddress( pRegSessionContext->pbProximityDetectionAddress, &cbAddress) );
        pRegSessionContext->cbProximityDetectionAddress = cbAddress;
    }

    ChkDR( dr );

    /* Construct the registration response message
     */
    ChkDR( WmdrmNetPackRegistrationResponseMessage(&idSerial,
                                                   certificateInfo.pDevicePublicKey,
                                                   certificateInfo.dwPubKeyModulusSize,
                                                   &pRegSessionContext->sessionContext.idSession,
                                                   &pRegSessionContext->idSeed,
                                                   pRegSessionContext->pbProximityDetectionAddress,
                                                   pRegSessionContext->cbProximityDetectionAddress,
                                                   f_pbRegistrationResponse,
                                                   f_pcbRegistrationResponse) );

    SAFE_OEM_FREE(pRegSessionContext->pbProximityDetectionAddress);
    pRegSessionContext->cbProximityDetectionAddress = 0;

    /* Success */
    Oem_Clock_GetSystemTimeAsFileTime( f_pOEMContext, &pRegSessionContext->ftRegistrationStartTime );

    pRegSessionContext->state = REG_STATE_PROCESSED_REGISTRATION_REQUEST;

    if ( FALSE == fBypassValidation )
    {
        MEMCPY( certificateInfo.rgbReceiverIdentityHash, rgbReceiverIdentityHash, DRM_SHA1_DIGEST_LEN );
        if ( NULL != pContext->certificateInfo.pDevicePublicKey )
        {
            OEM_SECURE_ZERO_MEMORY( pContext->certificateInfo.pDevicePublicKey, SIZEOF( DRM_RSA_PUBLIC_KEY_2048BIT ) );
            SAFE_OEM_FREE( pContext->certificateInfo.pDevicePublicKey );
        }
        MEMCPY( &pContext->certificateInfo, &certificateInfo, SIZEOF(certificateInfo) );
        certificateInfo.pDevicePublicKey = NULL;
    }

    pRegSessionContext->DeviceID = DeviceID;
    DeviceID.pbBlob = NULL;

ErrorExit:
    SAFE_OEM_FREE( pbDeviceCertificate );
    SAFE_OEM_FREE( DeviceID.pbBlob );

    if ( TRUE == fBypassValidation )
    {
        /* If validation was bypassed, the public key was copied into ndt manager
         * context, and it will handle freeing this
         */
        certificateInfo.pDevicePublicKey = NULL;
    }

    if ( NULL != certificateInfo.pDevicePublicKey )
    {
        OEM_SECURE_ZERO_MEMORY( certificateInfo.pDevicePublicKey, SIZEOF( DRM_RSA_PUBLIC_KEY_2048BIT ) );
        SAFE_OEM_FREE( certificateInfo.pDevicePublicKey );
    }

    if ( DRM_E_BUFFERTOOSMALL != dr )
    {
        TRACE_IF_FAILED(("Drm_Ndt_Registration_ProcessRequest failed 0x%lx\n", dr));
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



/**********************************************************************
**
** Function:    Drm_Ndt_Proximity_ProcessStartMsg
**
** Synopsis:    Parse the Proximity Start message to get the session ID
**              and locate the registration session context.
**              Generate the challenge message using
**              NDTMGR_GetProximityChallenge
**
** Arguments:   [f_pOEMContext]              -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext]           -- The NDT manager context
**              [f_ProximityStartMsg]        -- Start Message
**              [f_pbProximityChallengeMsg]  -- caller-allocated buffer
**              [f_pcbProximityChallengeMsg] -- on input: size of buffer
**                                              on output: size of message
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_ND_INVALID_SESSION if the transaction ID is invalid or
**              DRM_E_DEVICE_NOT_REGISTERED if the session has timed out
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_Proximity_ProcessStartMsg (
    __in_opt DRM_VOID                   *f_pOEMContext,
    __in     NDTMGR_CONTEXT             *f_pNdtMgrContext,
    __in     DRM_BYTEBLOB                f_ProximityStartMsg,
    __out_bcount(*f_pcbProximityChallengeMsg) DRM_BYTE *f_pbProximityChallengeMsg,
    __inout  DRM_DWORD                  *f_pcbProximityChallengeMsg
)
{
    DRM_RESULT                                   dr = DRM_SUCCESS;
    NDTMGR_INTERNAL_REGISTRATIONSESSION_CONTEXT *pRegistrationContext = NULL;
    DRM_ID idSession;
    DRM_ID idChallenge;
    DRM_BYTE *pbChallengeMsg = NULL;
    DRM_DWORD cbChallengeMsg = 0;
    DRM_BYTE *pbResultMsg = NULL;
    DRM_DWORD cbResultMsg = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_Proximity_ProcessStartMsg );

    ChkArg( f_pNdtMgrContext
            && f_ProximityStartMsg.pbBlob
            && f_pcbProximityChallengeMsg );

    ChkDR( WmdrmNetUnpackProximityStartMessage(f_ProximityStartMsg.pbBlob,
                                               f_ProximityStartMsg.cbBlob,
                                               &idSession) );

    ChkDR( NDTMGR_Internal_FindSession(f_pNdtMgrContext,
                                       idSession,
                                       NDTMGR_SESSION_TYPE_REGISTRATION,
                                       (NDTMGR_INTERNAL_SESSION_CONTEXT **) &pRegistrationContext) );

    /* Verify state for that registration transaction ID (is the receiver registered yet?) */
    if ( pRegistrationContext->state < REG_STATE_PROCESSED_REGISTRATION_REQUEST )
    {
        TRACE_IF_FAILED(("Drm_Ndt_Proximity_ProcessStartMsg: invalid state %d\n",pRegistrationContext->state));
        ChkDR( DRM_E_DEVICE_NOT_REGISTERED );
    }

    if ( pRegistrationContext->state == REG_STATE_DONE )
    {
        /* If the Proximity Detection procedure already succeeded, immediately
         * reply with a Proximity Result message after the transmitter receives
         * the new Proximity Start message from the receiver
         */

        if ( *f_pcbProximityChallengeMsg < WMDRMNET_PROXIMITY_RESULT_MESSAGE_SIZE )
        {
            *f_pcbProximityChallengeMsg = WMDRMNET_PROXIMITY_RESULT_MESSAGE_SIZE;
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }

        ChkDR( WmdrmNetPackProximityResultMessage(&idSession,
                                                  pRegistrationContext->wResult,
                                                  &pbResultMsg,
                                                  &cbResultMsg) );

        if ( cbResultMsg > *f_pcbProximityChallengeMsg )
        {
            ChkDR( DRM_E_BUFFERTOOSMALL );
        }

        MEMCPY(f_pbProximityChallengeMsg, pbResultMsg, cbResultMsg);
        *f_pcbProximityChallengeMsg = cbResultMsg;

        goto ErrorExit;
    }

    ChkDR( DRM_NDT_Proximity_GetChallenge( f_pOEMContext,
                                           f_pNdtMgrContext,
                                           idSession,
                                           &idChallenge) );

    ChkDR( WmdrmNetPackProximityChallengeMessage(pRegistrationContext->bSequenceNumber,
                                                 &idSession,
                                                 &idChallenge,
                                                 &pbChallengeMsg,
                                                 &cbChallengeMsg) );

    if ( *f_pcbProximityChallengeMsg >= cbChallengeMsg )
    {
        *f_pcbProximityChallengeMsg = cbChallengeMsg;
        ChkArg( f_pbProximityChallengeMsg );
        MEMCPY(f_pbProximityChallengeMsg, pbChallengeMsg, cbChallengeMsg);
    }
    else
    {
        *f_pcbProximityChallengeMsg = cbChallengeMsg;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    pRegistrationContext->bSequenceNumber++;

 ErrorExit:
    SAFE_OEM_FREE(pbChallengeMsg);
    SAFE_OEM_FREE(pbResultMsg);
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    Drm_Ndt_Proximity_ProcessResponseMsg
**
** Synopsis:    Parse the Proximity Response message to get the session
**              ID and locate the registration session context.
**              Invoke NDTMGR_VerifyProximityEncryptedChallenge using
**              the encrypted challenge and session ID from the proximity
**              response message.
**              Map the returned result to the WMDRM-ND Status code and
**              compose the proximity result message
**
** Arguments:   [f_pOEMContext]           -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext]        -- The NDT manager context
**              [f_ProximityResponseMsg]  -- Proximity Response Message
**              [f_pbProximityResultMsg]  -- caller allocated buffer
**              [f_pcbProximiryResultMsg] -- on input: size of buffer
**                                           on output: size of message
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_ND_INVALID_SESSION if the transaction ID is invalid or
**              DRM_E_DEVICE_NOT_REGISTERED if the session has timed out
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_Proximity_ProcessResponseMsg (
    __in_opt DRM_VOID                   *f_pOEMContext,
    __in     NDTMGR_CONTEXT             *f_pNdtMgrContext,
    __in     DRM_BYTEBLOB                f_ProximityResponseMsg,
    __out_bcount(*f_pcbProximityResultMsg) DRM_BYTE *f_pbProximityResultMsg,
    __inout  DRM_DWORD                  *f_pcbProximityResultMsg
)
{
    DRM_RESULT                                   dr = DRM_SUCCESS;
    DRM_RESULT                                   drProximity;
    DRM_BYTE                                     bSequence;
    DRM_ID                                       idSession;
    DRM_ID                                       idNonce;
    NDTMGR_INTERNAL_REGISTRATIONSESSION_CONTEXT *pRegSessionContext = NULL;
    DRM_BYTE                                    *pbResultMsg = NULL;
    DRM_DWORD                                    cbResultMsg;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_Proximity_ProcessResponseMsg );

    ChkArg( f_pNdtMgrContext
            && f_ProximityResponseMsg.pbBlob
            && f_pcbProximityResultMsg
            && (*f_pcbProximityResultMsg == 0 || f_pbProximityResultMsg != NULL) );


    if ( *f_pcbProximityResultMsg < WMDRMNET_PROXIMITY_RESULT_MESSAGE_SIZE )
    {
        *f_pcbProximityResultMsg = WMDRMNET_PROXIMITY_RESULT_MESSAGE_SIZE;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /* Parse the Proximity Response message to get the session ID and locate the registration context */
    ChkDR( WmdrmNetUnpackProximityResponseMessage(f_ProximityResponseMsg.pbBlob,
                                                  f_ProximityResponseMsg.cbBlob,
                                                  &bSequence,
                                                  &idSession,
                                                  &idNonce) );

    ChkDR( NDTMGR_Internal_FindSession( f_pNdtMgrContext, idSession, NDTMGR_SESSION_TYPE_REGISTRATION, (NDTMGR_INTERNAL_SESSION_CONTEXT **)&pRegSessionContext ) );

    /* Verify state for that registration transaction ID (is the receiver registered yet?) */
    if ( pRegSessionContext->state < REG_STATE_PROCESSED_REGISTRATION_REQUEST
         || pRegSessionContext->state == REG_STATE_DONE )
    {
        TRACE_IF_FAILED(("Drm_Ndt_Proximity_ProcessResponseMsg: invalid state %d\n", pRegSessionContext->state));
        ChkDR( DRM_E_DEVICE_NOT_REGISTERED );
    }

    /*
    ** Invoke the DRM_NDT_Proximity_VerifyEncryptedChallenge function using the encrypted
    ** challenge and session ID from the proximity response message.
    */
    drProximity =  DRM_NDT_Proximity_VerifyEncryptedChallenge( f_pOEMContext,
                                                               f_pNdtMgrContext,
                                                               idSession,
                                                               idNonce);

    /* Map the returned result to the WMDRM-ND Status code */
    pRegSessionContext->wResult = (DRM_WORD) DRM_NDT_DRToWMDRMNDError( drProximity );

    /* compose the proximity result message */
    ChkDR( WmdrmNetPackProximityResultMessage(&idSession,
                                              pRegSessionContext->wResult,
                                              &pbResultMsg,
                                              &cbResultMsg) );

    if ( cbResultMsg > *f_pcbProximityResultMsg )
    {
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    MEMCPY(f_pbProximityResultMsg, pbResultMsg, cbResultMsg);
    *f_pcbProximityResultMsg = cbResultMsg;

    if ( DRM_E_ND_INVALID_PROXIMITY_RESPONSE == drProximity
         || DRM_SUCCESS == drProximity )
    {
        pRegSessionContext->state = REG_STATE_DONE;
    }

 ErrorExit:
    SAFE_OEM_FREE(pbResultMsg);

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



static DRM_RESULT _GetDeviceHandle(
    __in                                NDTMGR_CONTEXT       *f_pNdtMgrContext,
    __in_bcount(f_cbDeviceCertificate) const DRM_BYTE * const f_pbDeviceCertificate,
    __in const                          DRM_DWORD             f_cbDeviceCertificate,
    __in const                          DRM_ID                f_idSerial,
    __out                               OEM_DEVICE_HANDLE    *f_pDeviceHandle)
{
    DRM_RESULT               dr = DRM_SUCCESS;
    DRM_BYTEBLOB             DeviceID = {NULL, 0};
    NDTMGR_CONTEXT_INTERNAL *pContext = (NDTMGR_CONTEXT_INTERNAL*) f_pNdtMgrContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__GetDeviceHandle );

    ChkArg( f_pNdtMgrContext
            && f_pbDeviceCertificate
            && f_pDeviceHandle );

    /* Device ID is the serial number concatenated with the certificate */
    ChkOverflow( DRM_ID_SIZE + f_cbDeviceCertificate, f_cbDeviceCertificate );
    DeviceID.cbBlob = DRM_ID_SIZE + f_cbDeviceCertificate;
    DeviceID.pbBlob = (DRM_BYTE *) Oem_MemAlloc(DeviceID.cbBlob);
    ChkMem( DeviceID.pbBlob );
    DRM_BYT_CopyBytes(DeviceID.pbBlob, 0, f_idSerial.rgb, 0, DRM_ID_SIZE);
    DRM_BYT_CopyBytes(DeviceID.pbBlob, DRM_ID_SIZE, f_pbDeviceCertificate, 0, f_cbDeviceCertificate);

    ChkDR( Oem_DeviceStore_GetDeviceByID( &pContext->deviceStoreContext,
                                          DeviceID,
                                          f_pDeviceHandle ) );

 ErrorExit:
    SAFE_OEM_FREE( DeviceID.pbBlob );
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    Drm_Ndt_License_ProcessChallenge
**
** Synopsis:    This call must do the following:
**              Parses the License challenge message to extract the
**              device certificate and serial number.
**              If the device is not registered, fail
**              Check if the device is valid, using DRM_DeviceStore_IsDeviceValid
**              Check if the device is already opened. To do this, iterate
**              the list of active contexts, locate the content retrieval
**              sessions, get the stored device handle and using that compare
**              the certificate and serial number against the current values.
**              If the two match then the device is already opened and the
**              global opened count need not be incremented. If none of the
**              opened sessions have the same device cert and serial number,
**              then the global count must be incremented.
**              Ensure that incrementing the global count does not exceed the
**              maximum limit for the opened devices (viz 10). If it does,
**              then fail the call.  The chained mode, the policy to be used
**              for root licenses is in the WMDRM-ND spec (see (1) ).
**
** Arguments:   [f_pOEMContext]        -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext]     -- manager context
**              [f_dwSessionID]        -- session id
**              [f_LicenseChallenge]   -- license challenge message
**              [f_LicenseMode]        -- license mode
**              [f_pPolicy]            -- policy (for root license if chained mode)
**              [f_pKID]               -- key id
**              [f_pbLicenseResponse]  -- caller allocated buffer
**              [f_pcbLicenseResponse] -- on input, the size of the buffer
**                                        on output, number of bytes used
**
** Returns:     DRM_E_BUFFERTOOSMALL -- if buffer size is not sufficient; in this case
**                                      *f_pcbLicenseResponse is set to the required size
**              DRM_SUCCESS          -- if successful
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_License_ProcessChallenge(
    __in_opt   DRM_VOID                 *f_pOEMContext,
    __in       NDTMGR_CONTEXT           *f_pNdtMgrContext,
    __in const DRM_ID                    f_idSession,
    __in const DRM_BYTEBLOB              f_LicenseChallenge,
    __in const NDT_LICENSE_MODE          f_LicenseMode,
    __in       WMDRMNET_POLICY          *f_pPolicy,
    __out      DRM_KID                  *f_pKID,
    __out_bcount(*f_pcbLicenseResponse) DRM_BYTE *f_pbLicenseResponse,
    __inout    DRM_DWORD                *f_pcbLicenseResponse
)
{
    DRM_RESULT                                   dr = DRM_SUCCESS;
    DRM_ID                                       idSerial;
    DRM_BYTE                                    *pbDeviceCertificate = NULL;
    DRM_DWORD                                    cbDeviceCertificate;
    DRM_BYTE                                    *pbAction = NULL;
    DRM_WORD                                     cbAction;
    NDTMGR_INTERNAL_SESSION_CONTEXT             *pSession = NULL;
    NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT      *pContentSession = NULL;
    NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT      *pOtherContentSession = NULL;
    DRM_BOOL                                     fFound = FALSE;
    DRM_DWORD                                    cbOtherDeviceID = 0;
    DRM_BYTE                                    *pbOtherDeviceID = NULL;
    DRM_DWORD                                    cbNewDeviceID = 0;
    DRM_BYTE                                    *pbNewDeviceID = NULL;
    NDTMGR_CONTEXT_INTERNAL                     *pContext = (NDTMGR_CONTEXT_INTERNAL*) f_pNdtMgrContext;
    DRM_BYTE                                    *pbCRL = NULL;
    DRM_DWORD                                    cbCRL = 0;
    DRM_DWORD                                    dwCRLVersion;
    NDT_LICENSE_CONTEXT                         *pLicContext = NULL;
    NDT_CERTIFICATE_INFO                         certificateInfo = CERTIFICATE_INFO_INITIALIZER;
    DRM_SUBSTRING                                dasstrLeafCertificate;
    DRM_DWORD                                    cbXMR = 0;
    DRM_BYTE                                    *pbXMR = NULL;
    DRM_BYTE                                    *pbMessage = NULL;
    DRM_DWORD                                    cbMessage = 0;
    DRM_SUBSTRING                                dastrAction = EMPTY_DRM_SUBSTRING;
    WMDRMNET_POLICY                             *pPolicy = NULL;
    WMDRMNET_POLICY                             *pFreePolicy = NULL;
    const WMDRMNET_POLICY_TRANSCRYPTPLAY        *pPolicyTranscryptPlay = NULL;
    DRM_SHA_CONTEXT                              contextSHA;
    DRM_BYTE                                     rgbReceiverIdentityHash[__CB_DECL(DRM_SHA1_DIGEST_LEN)];
    DRM_BOOL                                     fBypassValidation = FALSE;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_License_ProcessChallenge );

    ChkArg(NULL != f_pNdtMgrContext
           && NULL != f_LicenseChallenge.pbBlob
           && NULL != f_pKID
           && NULL != f_pcbLicenseResponse
           && ( NDT_LICENSE_MODE_CHAINED == f_LicenseMode ||
                NDT_LICENSE_MODE_SIMPLE == f_LicenseMode ) );

    /* Policy must only be specified in case of simple license mode */
    if( NDT_LICENSE_MODE_SIMPLE == f_LicenseMode )
    {
        ChkArg( NULL != f_pPolicy &&
               WMDRMNET_POLICY_TYPE_TRANSCRYPTPLAY == f_pPolicy->ePolicyType );
        pPolicy = f_pPolicy;
    }
    else
    {
        ChkArg( f_pPolicy == NULL );

        pFreePolicy = (WMDRMNET_POLICY *)Oem_MemAlloc(SIZEOF(WMDRMNET_POLICY) + SIZEOF(WMDRMNET_POLICY_TRANSCRYPTPLAY));
        pPolicy = pFreePolicy;
    }
    pPolicyTranscryptPlay = (const WMDRMNET_POLICY_TRANSCRYPTPLAY *)pPolicy->pbPolicy;
    /* Use the session ID to locate the content retrieval session and
     * verify that the state of the session is correct for processing this
     * license challenge
     */
    ChkDR( NDTMGR_Internal_FindSession( f_pNdtMgrContext,
                                        f_idSession,
                                        NDTMGR_SESSION_TYPE_CONTENT,
                                        &pSession ) );
    ChkBOOL( NDTMGR_SESSION_TYPE_CONTENT == pSession->sessionType,
             DRM_E_INVALIDARG );
    pContentSession = (NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *) pSession;

    /* Set up the license context pointer */
    pContentSession->licenseMode = f_LicenseMode;
    switch (pContentSession->licenseMode)
    {
    case NDT_LICENSE_MODE_SIMPLE:
        pContentSession->pLicenseContext = Oem_MemAlloc( SIZEOF( NDT_LICENSE_CONTEXT ) );
        ChkMem(pContentSession->pLicenseContext);
        ZEROMEM( (DRM_BYTE *) pContentSession->pLicenseContext,
                 SIZEOF( NDT_LICENSE_CONTEXT ) );
        pLicContext = (NDT_LICENSE_CONTEXT *) pContentSession->pLicenseContext;
        break;
    case NDT_LICENSE_MODE_CHAINED:
        pContentSession->pLicenseContext = Oem_MemAlloc( SIZEOF( NDT_CHAINED_LICENSE_CONTEXT ) );
        ChkMem(pContentSession->pLicenseContext);
        ZEROMEM( (DRM_BYTE *) pContentSession->pLicenseContext,
                 SIZEOF( NDT_CHAINED_LICENSE_CONTEXT ) );
        pLicContext = &( ( ( NDT_CHAINED_LICENSE_CONTEXT *)
            pContentSession->pLicenseContext )->rootLicenseContext );
        break;
    default:
        DRMASSERT(0);
    }

    ChkDR( DRM_NDT_License_GenerateKeys( f_pOEMContext, pContentSession, pLicContext ) );


    /* Parse the License challenge message to extract the device certificate
     *  and serial number
     */
    dr = WmdrmNetUnpackLicenseRequestMessage(f_LicenseChallenge.pbBlob,
                                             f_LicenseChallenge.cbBlob,
                                             &pLicContext->idRightsID,
                                             &idSerial,
                                             &pbDeviceCertificate,
                                             &cbDeviceCertificate,
                                             &pbAction,
                                             &cbAction,
                                             &pLicContext->dwClientCRLVersion);
    if ( DRM_FAILED(dr) )
    {
        /* Either DRM_E_ND_INVALID_MESSAGE, DRM_E_ND_INVALID_MESSAGE_VERSION,
           or DRM_E_ND_INVALID_MESSAGE_TYPE. */
        TRACE_IF_FAILED(("Drm_Ndt_License_ProcessChallenge: invalid message 0x%lx\n",dr));
        ChkDR( DRM_E_ND_BAD_REQUEST );
    }

    /* Use the OEM device store APIs to get a device handle. If the device is
     *  not registered, fail
     */
    dr = _GetDeviceHandle( f_pNdtMgrContext,
                           pbDeviceCertificate,
                           cbDeviceCertificate,
                           idSerial,
                           &pContentSession->hDevice );
    if ( DRM_FAILED( dr ) )
    {
        ChkDR( DRM_E_DEVICE_NOT_REGISTERED );
    }

    /* Check if the device is valid */
    if ( !DRM_DeviceStore_IsDeviceValid( f_pOEMContext, pContentSession->hDevice ) )
    {
        ChkDR( DRM_E_ND_MUST_REVALIDATE );
    }

    dastrAction.m_cch = cbAction;
    if( !DRM_UTL_DASSTRStrings_CI_Equal( (const DRM_CHAR*)pbAction, &dastrAction, &g_dastrDRM_RIGHT_PLAYBACK ) )
    {
        TRACE_IF_FAILED(("Drm_Ndt_License_ProcessChallenge: action isn't \"Play\"\n"));
        ChkDR( DRM_E_ND_BAD_REQUEST );
    }

    /* Verify the certificate.  Verification includes checking the WMDRM-ND
     * revocation list
     */

    /*
    ** We skip the certificate validation if we receive a certificate/serial number
    ** combo we already know about.  To determine if we already know about it,
    ** a SHA-1 hash of the serial number concatenated with the certificate is taken.
    */
    DRM_SHA_Init    ( &contextSHA, eDRM_SHA_1 );
    DRM_SHA_Update  ( (const DRM_BYTE *) &idSerial, DRM_ID_SIZE, &contextSHA );
    DRM_SHA_Update  ( pbDeviceCertificate, cbDeviceCertificate, &contextSHA );
    DRM_SHA_Finalize( &contextSHA, DRM_SHA1_DIGEST_LEN, rgbReceiverIdentityHash );

    /*
    ** Any app context is acceptable here as this function does not cache the
    ** CRL version *and* since master context fields like dwOpenedCount are
    ** modified, an exclusive lock must be held around this call in an MT
    ** environment anyway.
    */
    ChkDR( Drm_Revocation_GetList( pContext->pAppContext,
                                   DRM_REVOCATION_TYPE_WMDRM_ND,
                                   NULL,
                                   NULL,
                                   &dwCRLVersion ) );
    if ( 0 == MEMCMP( rgbReceiverIdentityHash, pContext->certificateInfo.rgbReceiverIdentityHash, DRM_SHA1_DIGEST_LEN ) )
    {
        if( dwCRLVersion == pContext->dwCRLVersion )
        {
            TRACE_IF_FAILED(("Setting fBypassValidation to TRUE\n"));
            fBypassValidation = TRUE;
        }
    }

    if ( TRUE == fBypassValidation )
    {
        MEMCPY(&certificateInfo, &pContext->certificateInfo, SIZEOF(certificateInfo));
    }
    else
    {
        dr = NDT_ValidateCertificate((NDTMGR_CONTEXT_INTERNAL *) f_pNdtMgrContext,
                                     (const DRM_CHAR*)pbDeviceCertificate,
                                     cbDeviceCertificate,
                                     &dasstrLeafCertificate,
                                     &certificateInfo,
                                     &pbCRL,
                                     &cbCRL,
                                     &dwCRLVersion);
        if ( DRM_E_CERTIFICATE_REVOKED == dr )
        {
            ChkDR(dr);
        }
        else if ( DRM_FAILED(dr) )
        {
            /* Registration request failed, so send back the appropriate error code. */
            TRACE_IF_FAILED(("Drm_Ndt_License_ProcessChallenge: Cert Validation failed 0x%lx\n",dr));
            ChkDR( DRM_E_INVALID_DEVICE_CERTIFICATE );
        }
        pContext->dwCRLVersion = dwCRLVersion;
    }

    /* Check if the device is already opened.
     * To do this, iterate the list of active contexts, locate the content
     * retrieval sessions, get the store device handle and using that
     * compare the cerificate and serial number against the current values.
     * Skip over the current content session being operated on by this
     * function.
     * If the certificate and serial number of another session matches the
     * current session, then the device is already opened and the global
     * opened count need not be incremented.
     * If none of the opened sessions have the same device cert and serial
     * number, then the global count must be incremented
     */
    cbNewDeviceID = 0;
    dr = Oem_DeviceStore_GetDeviceID(pContentSession->hDevice, pbNewDeviceID, &cbNewDeviceID);
    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        pbNewDeviceID = (DRM_BYTE *) Oem_MemAlloc(cbNewDeviceID);
        ChkMem( pbNewDeviceID );
        dr = Oem_DeviceStore_GetDeviceID(pContentSession->hDevice, pbNewDeviceID, &cbNewDeviceID);
    }
    ChkDR( dr );

    for( pSession = pContext->pFirstSession; pSession; pSession = pSession->pNextContext )
    {
        if ( pSession->sessionType == NDTMGR_SESSION_TYPE_CONTENT
             && MEMCMP(&pSession->idSession, &f_idSession, DRM_ID_SIZE) != 0 )
        {
            pOtherContentSession = (NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *) pSession;

            if ( pOtherContentSession->hDevice == NULL )
            {
                continue;
            }

            cbOtherDeviceID = 0;
            dr = Oem_DeviceStore_GetDeviceID( pOtherContentSession->hDevice,
                                              pbOtherDeviceID,
                                              &cbOtherDeviceID );
            if ( DRM_E_BUFFERTOOSMALL == dr )
            {
                SAFE_OEM_FREE( pbOtherDeviceID );
                pbOtherDeviceID = (DRM_BYTE *) Oem_MemAlloc( cbOtherDeviceID );
                ChkMem( pbOtherDeviceID );

                dr = Oem_DeviceStore_GetDeviceID( pOtherContentSession->hDevice,
                                                  pbOtherDeviceID,
                                                  &cbOtherDeviceID );
            }
            if ( DRM_SUCCESS != dr )
            {
                ChkDR( DRM_SUCCESS );
                continue;
            }

            if( cbOtherDeviceID == cbNewDeviceID
                && 0 == MEMCMP(pbNewDeviceID, pbOtherDeviceID, cbOtherDeviceID) )
            {
                /* The device is already opened, no need to increment global
                 * opened count.
                 */
                fFound = TRUE;
                break;
            }

        }
    }

    if ( NDT_LICENSE_MODE_CHAINED == f_LicenseMode )
    {
        ChkDR( DRM_NDT_License_SetRootPolicy( pContentSession, pPolicy ) );
    }

    /* Verify security level of incoming policy */
    pContentSession->dwReceiverSecurityLevel = certificateInfo.dwSecurityLevel;
    pContentSession->dwReceiverWMDRMNDFeatures = certificateInfo.dwWMDRMNDFeatures;

    if ( (DRM_DWORD) pPolicyTranscryptPlay->globals.MinimumEnvironment.wMinimumSecurityLevel > pContentSession->dwReceiverSecurityLevel )
    {
        ChkDR( DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );
    }
    pLicContext->wMinimumSecurityLevel = pPolicyTranscryptPlay->globals.MinimumEnvironment.wMinimumSecurityLevel;

    /* Build the license */
    if( NDT_LICENSE_MODE_CHAINED == f_LicenseMode )
    {
        ChkDR( NDT_License_BuildRootXMR( f_pNdtMgrContext, pContentSession, pPolicy, idSerial, &certificateInfo, &pbXMR, &cbXMR ) );
    }
    else
    {
        ChkDR( NDT_License_BuildSimpleXMR( f_pNdtMgrContext, pContentSession, f_pPolicy, idSerial, &certificateInfo, &pbXMR, &cbXMR ) );
    }

    /* Get Current WMDRM-ND Revocation List if registered device is a transmitter */
    if( NDT_FEATURE_TRANSMITTER & certificateInfo.dwWMDRMNDFeatures )
    {
        if( pLicContext->dwClientCRLVersion >= dwCRLVersion )
        {
            /* the client is up-to-date, we don't need to send down the CRL */
            cbCRL = 0;
        }
    }
    else
    {
        /* No need to send CRL to receivers */
        SAFE_OEM_FREE(pbCRL);
        cbCRL = 0;
    }

    ChkDR( WmdrmNetPackLicenseResponseMessage( pbXMR, cbXMR, pbCRL, cbCRL, &pbMessage, &cbMessage ) );

    /*
     * Make sure we have enough space to write out the license response message.
     */

    if ( (NULL == f_pbLicenseResponse)
      || (*f_pcbLicenseResponse < cbMessage) )
    {
        dr = DRM_E_BUFFERTOOSMALL;
    }

    *f_pcbLicenseResponse = cbMessage;
    ChkDR( dr );

    /* It's pretty much a sure thing now, go ahead and increment the count */
    if ( !fFound )
    {
        /* The device is not already opened, increment global opened count */
        if ( pContext->dwOpenedCount >= MAXIMUM_LIMIT_FOR_OPENED_DEVICES )
        {
            TRACE_IF_FAILED(("Global Open Exceed Count Failed\n"));
            ChkDR( DRM_E_ND_MEDIA_SESSION_LIMIT_REACHED );
        }
        pContext->dwOpenedCount++;
        TRACE_IF_FAILED(("Global Open Count incremented\n"));
    }

    ChkArg( f_pbLicenseResponse );
    MEMCPY( f_pbLicenseResponse, pbMessage, cbMessage );
    MEMCPY( f_pKID, &pLicContext->idKID, SIZEOF(DRM_KID) );

    if ( NDT_LICENSE_MODE_CHAINED == f_LicenseMode )
    {
        pContentSession->mediaSessionState = MEDIA_STATE_LICENSE_READY_FOR_LEAF;
    }
    else if ( NDT_LICENSE_MODE_SIMPLE == f_LicenseMode )
    {
        pContentSession->mediaSessionState = MEDIA_STATE_LICENSE_READY;
    }
    else
    {
        ChkDR(DRM_E_NOTIMPL);
    }

 ErrorExit:

    if ( DRM_FAILED( dr ) )
    {
        if ( NULL != pContentSession )
        {
            pContentSession->mediaSessionState = MEDIA_STATE_CLOSE;
            LicenseFreeKey( pLicContext );
            SAFE_OEM_FREE(pContentSession->pLicenseContext);
            if (  NULL != pContentSession->hDevice )
            {
                Oem_DeviceStore_FreeDevice( pContentSession->hDevice );
                pContentSession->hDevice = NULL;
            }
        }
    }

    SAFE_OEM_FREE(pbAction);
    SAFE_OEM_FREE(pbDeviceCertificate);
    SAFE_OEM_FREE(pbNewDeviceID);
    SAFE_OEM_FREE(pbCRL);
    SAFE_OEM_FREE(pbXMR);
    SAFE_OEM_FREE(pbMessage);
    SAFE_OEM_FREE(pFreePolicy);
    SAFE_OEM_FREE( pbOtherDeviceID );

    if ( FALSE == fBypassValidation )
    {
        if ( NULL != certificateInfo.pDevicePublicKey )
        {
            OEM_RSA_ZeroPublicKey_2048BIT( certificateInfo.pDevicePublicKey );
            SAFE_OEM_FREE( certificateInfo.pDevicePublicKey );
        }
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



/**********************************************************************
**
** Function:    Drm_Ndt_License_GenerateLeafResponse
**
** Synopsis:    This call must do the following:
**              Locate the session context
**              Generate the leaf license using the indicated policy.
**
** Arguments:   [f_pOEMContext]        -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext]     -- manager context
**              [f_idSession]          -- session id
**              [f_pPolicy]            -- policy
**              [f_pKID]               -- key id
**              [f_pbLicenseResponse]  -- caller allocated buffer
**              [f_pcbLicenseResponse] -- on input, the size of the buffer
**                                        on output, number of bytes used
**
** Returns:     DRM_E_BUFFERTOOSMALL -- if buffer size is not sufficient; in this case
**                                      *f_pcbLicenseResponse is set to the required size
**              DRM_SUCCESS          -- if successful
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_License_GenerateLeafResponse(
    __in_opt   DRM_VOID                 *f_pOEMContext,
    __in       NDTMGR_CONTEXT           *f_pNdtMgrContext,
    __in const DRM_ID                    f_idSession,
    __in       WMDRMNET_POLICY          *f_pPolicy,
    __out_bcount(*f_pcbLicenseResponse) DRM_BYTE *f_pbLicenseResponse,
    __inout    DRM_DWORD                *f_pcbLicenseResponse
)
{
    DRM_RESULT                                   dr = DRM_SUCCESS;
    NDTMGR_CONTEXT_INTERNAL                     *pContext = (NDTMGR_CONTEXT_INTERNAL*) f_pNdtMgrContext;
    NDTMGR_INTERNAL_SESSION_CONTEXT             *pSession = NULL;
    NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT      *pContentSession = NULL;
    const WMDRMNET_POLICY_TRANSCRYPTPLAY        *pPolicyTranscryptPlay = NULL;
    NDT_LICENSE_CONTEXT                         *pLicContext = NULL;
    DRM_DWORD                                    cbXMR = 0;
    DRM_BYTE                                    *pbXMR = NULL;
    DRM_DWORD                                    cbCRL = 0;
    DRM_BYTE                                    *pbCRL = NULL;
    DRM_DWORD                                    dwCRLVersion = 0;
    DRM_DWORD                                    cbMessage = 0;
    DRM_BYTE                                    *pbMessage = NULL;
    DRM_ID                                       idSerial;
    DRM_BYTE                                    *pbDeviceID = NULL;
    DRM_DWORD                                    cbDeviceID = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_License_GenerateLeafResponse );

    ChkArg( f_pNdtMgrContext
            && f_pPolicy
            && f_pcbLicenseResponse
            && (*f_pcbLicenseResponse == 0 || NULL != f_pbLicenseResponse)
            && f_pPolicy->ePolicyType == WMDRMNET_POLICY_TYPE_TRANSCRYPTPLAY );

    /* Use the session ID to locate the content retrieval session and
     * verify that the state of the session is correct for processing this
     * message
     */
    ChkDR( NDTMGR_Internal_FindSession(f_pNdtMgrContext, f_idSession, NDTMGR_SESSION_TYPE_CONTENT, &pSession) );
    ChkBOOL( NDTMGR_SESSION_TYPE_CONTENT == pSession->sessionType, DRM_E_INVALIDARG );
    pContentSession = (NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *) pSession;

    /* Check if the device is valid */
    if (  NULL == pContentSession->hDevice )
    {
        ChkDR( DRM_E_DEVICE_NOT_REGISTERED );
    }
    if ( !DRM_DeviceStore_IsDeviceValid( f_pOEMContext, pContentSession->hDevice ) )
    {
        ChkDR( DRM_E_ND_MUST_REVALIDATE );
    }

    /* Make sure license mode is chained */
    ChkBOOL( NDT_LICENSE_MODE_CHAINED == pContentSession->licenseMode, DRM_E_FAIL );

    ChkBOOL( ( MEDIA_STATE_LICENSE_READY_FOR_LEAF == pContentSession->mediaSessionState
         || MEDIA_STATE_LICENSE_READY == pContentSession->mediaSessionState), DRM_E_FAIL );

    /* Verify security level of incoming policy */
    pPolicyTranscryptPlay = (const WMDRMNET_POLICY_TRANSCRYPTPLAY *) f_pPolicy->pbPolicy;
    if ( (DRM_DWORD) pPolicyTranscryptPlay->globals.MinimumEnvironment.wMinimumSecurityLevel > pContentSession->dwReceiverSecurityLevel )
    {
        ChkDR( DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );
    }
    if ( pPolicyTranscryptPlay->globals.MinimumEnvironment.wMinimumSecurityLevel > ((NDT_CHAINED_LICENSE_CONTEXT *)pContentSession->pLicenseContext)->rootLicenseContext.wMinimumSecurityLevel )
    {
        ChkDR( DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW );
    }

    dr = Oem_DeviceStore_GetDeviceID( pContentSession->hDevice,
                                      pbDeviceID,
                                      &cbDeviceID );
    if ( DRM_E_BUFFERTOOSMALL == dr )
    {
        pbDeviceID = (DRM_BYTE *) Oem_MemAlloc(cbDeviceID);
        ChkMem(pbDeviceID);
        dr = Oem_DeviceStore_GetDeviceID( pContentSession->hDevice,
                                          pbDeviceID,
                                          &cbDeviceID );
    }
    ChkDR(dr);
    DRM_BYT_CopyBytes(&idSerial, 0, pbDeviceID, 0, DRM_ID_SIZE);

    /* Generate new keys */
    pLicContext = &((NDT_CHAINED_LICENSE_CONTEXT *) pContentSession->pLicenseContext)->leafLicenseContext;
    LicenseFreeKey( pLicContext );
    ChkDR( DRM_NDT_License_GenerateKeys( f_pOEMContext, pContentSession, pLicContext ) );
    pLicContext->wMinimumSecurityLevel = pPolicyTranscryptPlay->globals.MinimumEnvironment.wMinimumSecurityLevel;

    /* Build the license */
    ChkDR( NDT_License_BuildLeafXMR( f_pNdtMgrContext, pContentSession, f_pPolicy, idSerial, &pbXMR, &cbXMR ) );

    if ( NDT_FEATURE_TRANSMITTER & pContentSession->dwReceiverWMDRMNDFeatures )
    {
        dr = GetWMDRMNDRevocationList( pContext, &pbCRL, &cbCRL, &dwCRLVersion );
        if( DRM_FAILED(dr) )
        {
            TRACE_IF_FAILED(("Get Device CRL failed 0x%lx\n",dr));
            ChkDR( dr );
        }
    }

    ChkDR( WmdrmNetPackLicenseResponseMessage( pbXMR, cbXMR, pbCRL, cbCRL, &pbMessage, &cbMessage ) );

    /*
     * Make sure we have enough space to write out the license response message.
     */

    if ( *f_pcbLicenseResponse < cbMessage )
    {
        dr = DRM_E_BUFFERTOOSMALL;
    }

    *f_pcbLicenseResponse = cbMessage;
    ChkDR( dr );

    MEMCPY(f_pbLicenseResponse, pbMessage, cbMessage);

    /* All done */
    pContentSession->mediaSessionState = MEDIA_STATE_LICENSE_READY;

 ErrorExit:

    if ( DRM_FAILED(dr) )
    {
        if ( NULL != pContentSession )
        {
            if( NULL != pLicContext )
            {
                LicenseFreeKey( pLicContext );
            }
        }
    }

    SAFE_OEM_FREE(pbCRL);
    SAFE_OEM_FREE(pbMessage);
    SAFE_OEM_FREE(pbXMR);
    SAFE_OEM_FREE(pbDeviceID);

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



static DRM_RESULT _DrmRevocationTypeEnumFromGUID(
    __in const DRM_GUID                  * const f_pGuid,
    __out      DRM_REVOCATION_TYPE_ENUM  * const f_peRevType
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    if(DRM_IsEqualGUID( &GUID_WMDRMRevocationTypeApp, f_pGuid ) )
    {
        *f_peRevType = DRM_REVOCATION_TYPE_WMDRM_APP;
    }
    else if(DRM_IsEqualGUID( &GUID_WMDRMRevocationTypeWmdrmpd, f_pGuid ) )
    {
        *f_peRevType = DRM_REVOCATION_TYPE_WMDRM_DEVICE;
    }
    else if(DRM_IsEqualGUID( &GUID_WMDRMRevocationTypeWmdrmnd, f_pGuid ) )
    {
        *f_peRevType = DRM_REVOCATION_TYPE_WMDRM_ND;
    }
    else
    {
        ChkDR( DRM_E_INVALIDARG );
    }

ErrorExit:
    return dr;
}



static DRM_RESULT _GUIDFromDrmRevocationTypeEnum(
    __in const DRM_REVOCATION_TYPE_ENUM    eRevType,
    __out      DRM_GUID                  **ppGuid
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__GUIDFromDrmRevocationTypeEnum );

    switch(eRevType)
    {
        case DRM_REVOCATION_TYPE_WMDRM_APP:
            *ppGuid = (DRM_GUID *)&GUID_WMDRMRevocationTypeApp;
            break;
        case DRM_REVOCATION_TYPE_WMDRM_DEVICE:
            *ppGuid = (DRM_GUID *)&GUID_WMDRMRevocationTypeWmdrmpd;
            break;
        case DRM_REVOCATION_TYPE_WMDRM_ND:
            *ppGuid = (DRM_GUID *)&GUID_WMDRMRevocationTypeWmdrmnd;
            break;
        default:
            ChkDR( DRM_E_INVALIDARG );
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    Drm_Ndt_RevocationList_GenerateChallenge
**
** Synopsis:    Generates a revocation list request message for processing
**              by another distinct transmitter.  Used to initiate
**              revocation list propagation between transmitters.
**
** Arguments:   [f_pNdtMgrContext]           -- manager context
**              [f_eRevocationList]          -- enumerant describing the
**                                              nature of the request
**              [f_pbRevocationListRequest]  -- caller allocated buffer
**              [f_pcbRevocationListRequest] -- on input, the size of the
**                                              buffer on output, number of
**                                              bytes used
**
** Returns:     DRM_E_BUFFERTOOSMALL -- if buffer size is not
**                                      sufficient; in this case
**                                      *f_pcbRevocationListRequest is set
**                                      to the required size
**              DRM_SUCCESS          -- if successful
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_RevocationList_GenerateChallenge(
    __in const NDTMGR_CONTEXT           *f_pNdtMgrContext,
    __in const DRM_REVOCATION_LIST_TYPE  f_eRevocationList,
    __out_bcount_opt(*f_pcbRevocationListRequest) DRM_BYTE *f_pbRevocationListRequest,
    __inout    DRM_DWORD                *f_pcbRevocationListRequest
)
{
    DRM_DWORD cguidsRevocationLists = 0;
    DRM_GUID guidsRevocationLists[DRM_REVOCATION_LIST_NET + 1];
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_RevocationList_GenerateChallenge );

    ChkArg(f_pNdtMgrContext != NULL);

    switch(f_eRevocationList)
    {
    case DRM_REVOCATION_LIST_ALL:
    case DRM_REVOCATION_LIST_RIV:
    case DRM_REVOCATION_LIST_APPS:
    case DRM_REVOCATION_LIST_DEVICE:
    case DRM_REVOCATION_LIST_NET:
        break;
    default:
        ChkDR( DRM_E_INVALIDARG );
    }

    if(DRM_REVOCATION_LIST_RIV == f_eRevocationList ||
       DRM_REVOCATION_LIST_ALL == f_eRevocationList)
    {
        MEMCPY( &guidsRevocationLists[cguidsRevocationLists],
                &GUID_WMDRMRevocationTypeRiv,
                DRM_ID_SIZE );
        cguidsRevocationLists++;
    }

    if(DRM_REVOCATION_LIST_APPS == f_eRevocationList ||
       DRM_REVOCATION_LIST_ALL == f_eRevocationList)
    {
        MEMCPY( &guidsRevocationLists[cguidsRevocationLists],
                &GUID_WMDRMRevocationTypeApp,
                DRM_ID_SIZE );
        cguidsRevocationLists++;
    }

    if(DRM_REVOCATION_LIST_DEVICE == f_eRevocationList ||
       DRM_REVOCATION_LIST_ALL == f_eRevocationList)
    {
        MEMCPY( &guidsRevocationLists[cguidsRevocationLists],
                &GUID_WMDRMRevocationTypeWmdrmpd,
                DRM_ID_SIZE );
        cguidsRevocationLists++;
    }

    if(DRM_REVOCATION_LIST_NET == f_eRevocationList ||
       DRM_REVOCATION_LIST_ALL == f_eRevocationList)
    {
        MEMCPY( &guidsRevocationLists[cguidsRevocationLists],
                &GUID_WMDRMRevocationTypeWmdrmnd,
                DRM_ID_SIZE );
        cguidsRevocationLists++;
    }

    ChkDR( WmdrmNetPackRevocationListRequestMessage(
               cguidsRevocationLists,
               guidsRevocationLists,
               f_pbRevocationListRequest,
               f_pcbRevocationListRequest ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    Drm_Ndt_RevocationList_ProcessRequest
**
** Synopsis:    Process a revocation list request message generated by either
**              another transmitter or a receiver.  Used to propagate
**              revocation lists back to the requestor.
**
** Arguments:   [f_pNdtMgrContext]           -- manager context
**              [f_RevocationListRequest]    -- blob containing request msg
**              [f_pbRevocationListResponse] -- caller allocated buffer to
**                                              contain response
**              [f_pcbRevocationListResponse] -- on input, size of response
**                                               buffer
**                                               on output, size of message
**
** Returns:     DRM_E_BUFFERTOOSMALL -- if buffer size is not
**                                      sufficient; in this case
**                                      *f_pcbRevocationListResponse is set
**                                      to the required size
**              DRM_SUCCESS          -- if successful
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_RevocationList_ProcessRequest(
    __in       NDTMGR_CONTEXT          *f_pNdtMgrContext,
    __in const DRM_BYTEBLOB             f_RevocationListRequest,
    __out_bcount_opt(*f_pcbRevocationListResponse) DRM_BYTE *f_pbRevocationListResponse,
    __inout    DRM_DWORD               *f_pcbRevocationListResponse
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    NDTMGR_CONTEXT_INTERNAL *pContext = NULL;
    DRM_GUID *pguidsRevocationLists = NULL;
    DRM_DWORD cguidsRevocationLists = 0;
    DRM_DWORD dwRevocationList = 0;
    DRM_GUID *pGuid = NULL;
    DRM_DWORD i = 0;
    DRM_REVOCATION_TYPE_ENUM eRevType;
    DRM_DWORD cbTotalCRL = 0;
    DRM_DWORD cRevocationLists = 0;
    DRM_RVK_LIST *pRevocationLists = NULL;
    DRM_RVK_LIST *pRevocationList = NULL;
    DRM_DWORD cbRevocationLists = 0;
    DRM_DWORD cbRevocationListResponse = 0;

    /* WMDRMND is currently the highest enumerant.  If a revocation type
       is added change the following values.  We add 2 because the enumeration
       is zero-based (+1) and we also reserve the first element for
       the RIV (+1) since it is a special case. */

    DRM_DWORD rgbcbRevocationList[DRM_REVOCATION_TYPE_WMDRM_ND + 2];
    DRM_DWORD rgbfRevocationList[DRM_REVOCATION_TYPE_WMDRM_ND + 2];

    /* Function logic begins.  Check arguments and initalize variables. */

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_RevocationList_ProcessRequest );

    ChkArg( f_pNdtMgrContext != NULL &&
            f_RevocationListRequest.pbBlob != NULL &&
            f_pcbRevocationListResponse != NULL &&
            ( *f_pcbRevocationListResponse == 0 ||
              f_pbRevocationListResponse != NULL ) );

    pContext = (NDTMGR_CONTEXT_INTERNAL *)f_pNdtMgrContext;
    ZEROMEM( rgbcbRevocationList, SIZEOF( rgbcbRevocationList ) );
    ZEROMEM( rgbfRevocationList,  SIZEOF( rgbfRevocationList  ) );

    if(DRM_FAILED( dr = WmdrmNetUnpackRevocationListRequestMessage
               ( f_RevocationListRequest.pbBlob,
                 f_RevocationListRequest.cbBlob,
                 &cguidsRevocationLists,
                 &pguidsRevocationLists ) ) )
    {
        /* Either DRM_E_ND_INVALID_MESSAGE, DRM_E_ND_INVALID_MESSAGE_VERSION, or
           DRM_E_ND_INVALID_MESSAGE_TYPE. */
        TRACE_IF_FAILED(("Drm_Ndt_RevocationList_ProcessRequest: "
               "Invalid Revocation List Response Message: 0x%lx\n", dr));
        ChkDR( DRM_E_ND_BAD_REQUEST );
    }

    /* Initially do a retreival of the size of all requested blobs,
       so we can throw a DRM_E_BUFFERTOOSMALL if necessary.  We save
       the buffer sizes needed for when we do the actual retreival
       of the blobs.  We use an array of booleans to track which blobs
       have been requested.  This is necessary because we need to return
       information for all requested blobs, even if they are zero-length. */

    for(i = 0; i < cguidsRevocationLists; i++)
    {
        /* RIV is a special case */
        if( DRM_IsEqualGUID( &GUID_WMDRMRevocationTypeRiv,
                             &pguidsRevocationLists[i] ) )
        {
            /* Set our index variable, used at the end of the for loop. */
            dwRevocationList = 0;

            if(rgbfRevocationList[dwRevocationList])
            {
                /* We've already seen and processed a request for the RIV,
                   so ignore duplicates. */
                continue;
            }

            rgbfRevocationList[0] = TRUE;
            dr = Drm_Revocation_GetList( pContext->pAppContext,
                                         DRM_REVOCATION_TYPE_WMDRM_REVINFO,
                                         NULL,
                                         &rgbcbRevocationList[0],
                                         NULL );

            if(dr != DRM_E_BUFFERTOOSMALL)
            {
                ChkDR( dr );
            }
        }
        else
        {
            /* Handle standard revocation lists. */
            ChkDR( _DrmRevocationTypeEnumFromGUID( &pguidsRevocationLists[i],
                                                   &eRevType ));

            /* Index variable is enumerant + 1 since first element is RIV */
            dwRevocationList = (DRM_DWORD)eRevType + 1;

            ChkBOOL( dwRevocationList < DRM_REVOCATION_TYPE_WMDRM_ND + 2, DRM_E_BUFFERTOOSMALL );

            if(rgbfRevocationList[dwRevocationList])
            {
                /* We've already seen and processed a request for this type,
                   so ignore duplicates. */
                continue;
            }

            rgbfRevocationList[dwRevocationList] = TRUE;
            dr = Drm_Revocation_GetList(
                     pContext->pAppContext,
                     eRevType,
                     NULL,
                     &rgbcbRevocationList[dwRevocationList],
                     NULL );
            if(dr != DRM_E_BUFFERTOOSMALL && dr != DRM_E_NOTIMPL)
            {
                ChkDR( dr );
            }
        }

        /* Add any requested CRL to to the total byte count, increment
           element count. */
        cbTotalCRL += rgbcbRevocationList[dwRevocationList];
        cRevocationLists++;
    }

    /* Ensure we have a large enough buffer */

    cbRevocationListResponse =
      WmdrmNetFixedMessageSize(WMDRMNET_REVOCATION_LIST_RESPONSE_MESSAGE_TYPE)+
        cbTotalCRL + cRevocationLists * (SIZEOF(DRM_GUID) + SIZEOF(DRM_DWORD));

    if(cbRevocationListResponse > *f_pcbRevocationListResponse)
    {
        *f_pcbRevocationListResponse = cbRevocationListResponse;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    if(cRevocationLists)
    {
        /* Now we can actually allocate storage for our list and the CRLs
           themselves.  Once they're all retreived we can generate
           our response message. */

        cbRevocationLists = SIZEOF( DRM_RVK_LIST ) * cRevocationLists ;
        pRevocationLists = (DRM_RVK_LIST *) Oem_MemAlloc( cbRevocationLists );
        ChkMem( pRevocationLists );

        ZEROMEM( pRevocationLists, cbRevocationLists );

        pRevocationList = pRevocationLists;

        /* First deal with the RIV if necessary */
        if( rgbfRevocationList[0] && rgbcbRevocationList[0] )
        {
            pRevocationList->pbRevocationList = (DRM_BYTE*)
                Oem_MemAlloc( rgbcbRevocationList[0] );
            ChkMem( pRevocationList->pbRevocationList );

            ChkDR( Drm_Revocation_GetList( pContext->pAppContext,
                                           DRM_REVOCATION_TYPE_WMDRM_REVINFO,
                                           pRevocationList->pbRevocationList,
                                           &rgbcbRevocationList[0],
                                           NULL ) );

            pRevocationList->cbRevocationList = rgbcbRevocationList[0];
        }
        if( rgbfRevocationList[0] )
        {
            MEMCPY( &pRevocationList->guidRevocationList,
                    &GUID_WMDRMRevocationTypeRiv,
                    DRM_ID_SIZE );
            pRevocationList++;
        }

        /* Subsequently process any CRLs requested */
        for( i = 1; i < NO_OF(rgbcbRevocationList); i++)
        {
            if(! rgbfRevocationList[i] )
            {
                continue;
            }

            /* Convert index value to enumerant and retreive guid */
            eRevType = (DRM_REVOCATION_TYPE_ENUM) (i - 1);
            ChkDR( _GUIDFromDrmRevocationTypeEnum( eRevType, &pGuid ) );

            /* Only allocate and retreive list if non-zero.  Note we still
               return an entry in the response in any case. */

            if(rgbcbRevocationList[i])
            {
                pRevocationList->pbRevocationList = (DRM_BYTE*)
                    Oem_MemAlloc( rgbcbRevocationList[i] );
                ChkMem( pRevocationList->pbRevocationList );

                ChkDR( Drm_Revocation_GetList(
                           pContext->pAppContext,
                           eRevType,
                           pRevocationList->pbRevocationList,
                           &rgbcbRevocationList[i],
                           NULL ) );

                pRevocationList->cbRevocationList = rgbcbRevocationList[i];
            }

            MEMCPY( &pRevocationList->guidRevocationList,
                    pGuid, DRM_ID_SIZE );
            pRevocationList++;
        }
    }

    /* Generate response message. */

    ChkDR( WmdrmNetPackRevocationListResponseMessage
               ( cRevocationLists,
                 pRevocationLists,
                 f_pbRevocationListResponse,
                 f_pcbRevocationListResponse ) );
ErrorExit:

    if( pRevocationLists )
    {
        for(i = 0; i < cRevocationLists; i++)
        {
            SAFE_OEM_FREE( pRevocationLists[i].pbRevocationList );
        }
        Oem_MemFree( pRevocationLists );
    }
    SAFE_OEM_FREE( pguidsRevocationLists );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



/**********************************************************************
**
** Function:    Drm_Ndt_RevocationList_ProcessResponse
**
** Synopsis:    Process a revocation list response message generated by
**              another transmitter.  This puts the revocation lists
**              and/or RIV contained in the response into the secure store.
**
** Arguments:   [f_pNdtMgrContext]           -- manager context
**              [f_RevocationListResponse]   -- blob containing response msg
**
** Returns:     DRM_SUCCESS -- if successful
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_RevocationList_ProcessResponse(
    __in       NDTMGR_CONTEXT          *f_pNdtMgrContext,
    __in const DRM_BYTEBLOB             f_RevocationListResponse
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    NDTMGR_CONTEXT_INTERNAL *pContext;
    DRM_DWORD cRevocationLists = 0;
    DRM_RVK_LIST *pRevocationLists = NULL;
    DRM_DWORD i = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Ndt_RevocationList_ProcessResponse );

    ChkArg( f_pNdtMgrContext != NULL );

    pContext = (NDTMGR_CONTEXT_INTERNAL *)f_pNdtMgrContext;

    dr = WmdrmNetUnpackRevocationListResponseMessage(
             f_RevocationListResponse.pbBlob,
             f_RevocationListResponse.cbBlob,
             &cRevocationLists,
             NULL );
    if( dr != DRM_E_BUFFERTOOSMALL && DRM_FAILED( dr ) )
    {
        ChkDR( DRM_E_ND_BAD_REQUEST );
    }
    if(cRevocationLists == 0)
    {
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }

    ChkMem( pRevocationLists = (DRM_RVK_LIST*) Oem_MemAlloc( cRevocationLists * SIZEOF( DRM_RVK_LIST ) ) );
    ZEROMEM( pRevocationLists, cRevocationLists * SIZEOF( DRM_RVK_LIST ) );

    ChkDR( WmdrmNetUnpackRevocationListResponseMessage(
               f_RevocationListResponse.pbBlob,
               f_RevocationListResponse.cbBlob,
               &cRevocationLists,
               pRevocationLists ) );

    dr = Drm_Revocation_StoreRevListArray( pContext->pAppContext,
                                           cRevocationLists,
                                           pRevocationLists );
    if(DRM_FAILED(dr))
    {
        TRACE_IF_FAILED(("Drm_Ndt_RevocationList_ProcessResponse: Drm_Revocation_StoreRevListArray failed: 0x%lx\n",dr));
        ChkDR( dr );
    }

ErrorExit:
    if( pRevocationLists != NULL )
    {
        for(i = 0; i < cRevocationLists; i++)
        {
            SAFE_OEM_FREE( pRevocationLists[i].pbRevocationList );
        }
        SAFE_OEM_FREE( pRevocationLists );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

