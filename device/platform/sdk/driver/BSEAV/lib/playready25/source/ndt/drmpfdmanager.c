/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMPFDMANAGER_C
#include <ndttypes.h>
#include <drmpfdmanager.h>
#include <ndtconstants.h>
#include <ndtlicense.h>
#include <ndtreg.h>
#include <pfddrmmtpextensions.h>
#include <ndtnetmsg.h>
#include <oemaes.h>
#include <drmviewrights.h>
#include <oemndt.h>
#include <drmpfd.h>
#include <drmndtasf.h>
#include <ndtmanager_internal.h>
#include <drmprofile.h>
#include <drmlastinclude.h>


#define VERIFY_REGISTRATION_TRANSACTION_ID( id ) \
    if ( (pContext->dwRegistrationTransactionID == 0 )   \
      || (pContext->dwRegistrationTransactionID != id ) ) \
    { \
        dr = DRM_E_ND_INVALID_SESSION; \
        goto ErrorExit; \
    }


ENTER_PK_NAMESPACE_CODE;

static DRM_VOID _PackDWORDSInDRMID(
    __out      DRM_ID    *pId,
    __in const DRM_DWORD  dw1,
    __in const DRM_DWORD  dw2,
    __in const DRM_DWORD  dw3,
    __in const DRM_DWORD  dw4
)
{
    PUT_BYTE( pId->rgb,  0, (DRM_BYTE)( dw1 >> 24) );
    PUT_BYTE( pId->rgb,  1, (DRM_BYTE)( dw1 >> 16) );
    PUT_BYTE( pId->rgb,  2, (DRM_BYTE)( dw1 >> 8) );
    PUT_BYTE( pId->rgb,  3, (DRM_BYTE)( dw1 ) );

    PUT_BYTE( pId->rgb,  4, (DRM_BYTE)( dw2 >> 24) );
    PUT_BYTE( pId->rgb,  5, (DRM_BYTE)( dw2 >> 16) );
    PUT_BYTE( pId->rgb,  6, (DRM_BYTE)( dw2 >> 8) );
    PUT_BYTE( pId->rgb,  7, (DRM_BYTE)( dw2 ) );

    PUT_BYTE( pId->rgb,  8, (DRM_BYTE)( dw3 >> 24) );
    PUT_BYTE( pId->rgb,  9, (DRM_BYTE)( dw3 >> 16) );
    PUT_BYTE( pId->rgb, 10, (DRM_BYTE)( dw3 >> 8) );
    PUT_BYTE( pId->rgb, 11, (DRM_BYTE)( dw3 ) );

    PUT_BYTE( pId->rgb, 12, (DRM_BYTE)( dw4 >> 24) );
    PUT_BYTE( pId->rgb, 13, (DRM_BYTE)( dw4 >> 16) );
    PUT_BYTE( pId->rgb, 14, (DRM_BYTE)( dw4 >> 8) );
    PUT_BYTE( pId->rgb, 15, (DRM_BYTE)( dw4 ) );
}

/****************************************************************************/
static DRM_VOID _UnpackDWORDSFromDRMID(
    __in  const DRM_ID * const  pId,
    __out DRM_DWORD            *pdw1,
    __out DRM_DWORD            *pdw2,
    __out DRM_DWORD            *pdw3,
    __out DRM_DWORD            *pdw4
)
{
  *pdw1 = GET_BYTE(pId->rgb,  0);
  *pdw1 <<= 8;
  *pdw1 += GET_BYTE(pId->rgb, 1);
  *pdw1 <<= 8;
  *pdw1 += GET_BYTE(pId->rgb, 2);
  *pdw1 <<= 8;
  *pdw1 += GET_BYTE(pId->rgb, 3);

  *pdw2 = GET_BYTE(pId->rgb,  4);
  *pdw2 <<= 8;
  *pdw2 += GET_BYTE(pId->rgb, 5);
  *pdw2 <<= 8;
  *pdw2 += GET_BYTE(pId->rgb, 6);
  *pdw2 <<= 8;
  *pdw2 += GET_BYTE(pId->rgb, 7);

  *pdw3 = GET_BYTE(pId->rgb,  8);
  *pdw3 <<= 8;
  *pdw3 += GET_BYTE(pId->rgb, 9);
  *pdw3 <<= 8;
  *pdw3 += GET_BYTE(pId->rgb, 10);
  *pdw3 <<= 8;
  *pdw3 += GET_BYTE(pId->rgb, 11);

  *pdw4 = GET_BYTE(pId->rgb,  12);
  *pdw4 <<= 8;
  *pdw4 += GET_BYTE(pId->rgb, 13);
  *pdw4 <<= 8;
  *pdw4 += GET_BYTE(pId->rgb, 14);
  *pdw4 <<= 8;
  *pdw4 += GET_BYTE(pId->rgb, 15);

}


/**********************************************************************
**
** Function:    Drm_Pfd_GetMTPStatusFromDRMRESULT
**
** Synopsis:    Translates a DRM_RESULT into an MTP response code.
**
** Arguments:   [f_hrRes] -- The DRM_RESULT to translate
**
** Returns:     The corresponding MTP response code corresponding to the given DRM_RESULT
**
**********************************************************************/
DRM_API DRM_WORD DRM_CALL Drm_Pfd_GetMTPStatusFromDRMRESULT(
    __in const    DRM_RESULT f_drRes
)
{
    DRM_WORD wResult;

    switch (f_drRes)
    {
    case DRM_SUCCESS:
         wResult = 0x2001; /* MTP_RESPONSECODE_OK */
         break;

    case DRM_E_INVALID_DEVICE_CERTIFICATE:
        wResult = MTP_RESPONSECODE_WMDRMND_INVALID_CERTIFICATE;
        break;

    case DRM_E_CERTIFICATE_REVOKED:
        wResult = MTP_RESPONSECODE_WMDRMND_CERTIFICATE_REVOKED;
        break;

    case DRM_E_LICENSE_NOT_FOUND:
    case DRM_E_DEVICE_SECURITY_LEVEL_TOO_LOW:
        wResult = MTP_RESPONSECODE_WMDRMND_LICENSE_UNAVAILABLE;
        break;

    case DRM_E_ND_DEVICE_LIMIT_REACHED:
        wResult = MTP_RESPONSECODE_WMDRMND_DEVICE_LIMIT_REACHED;
        break;

    case DRM_E_ND_UNABLE_TO_VERIFY_PROXIMITY:
        wResult = MTP_RESPONSECODE_WMDRMND_UNABLE_TO_VERIFY_PROXIMITY;
        break;

    case DRM_E_DEVICE_NOT_REGISTERED:
        wResult = MTP_RESPONSECODE_WMDRMND_MUST_REGISTER;
        break;

    case DRM_E_ND_MUST_REVALIDATE:
        wResult = MTP_RESPONSECODE_WMDRMND_MUST_REVALIDATE;
        break;

    case DRM_E_ND_INVALID_PROXIMITY_RESPONSE:
        wResult = MTP_RESPONSECODE_WMDRMND_INVALID_PROXIMITY_RESPONSE;
        break;

    case DRM_E_ND_INVALID_SESSION:
        wResult = MTP_RESPONSECODE_AAVT_INVALID_MEDIA_SESSION_ID;
        break;

    case DRM_E_FILEOPEN:
        wResult = MTP_RESPONSECODE_WMDRMND_UNABLE_TO_OPEN_FILE;
        break;

    case DRM_E_ND_BAD_REQUEST:
        wResult = MTP_RESPONSECODE_WMDRMND_BAD_REQUEST;
        break;

    case DRM_E_NOMORE:
        wResult = MTP_RESPONSECODE_AAVT_NO_MORE_DATA;
        break;

    case DRM_E_ND_MEDIA_SESSION_LIMIT_REACHED:
        wResult = MTP_RESPONSECODE_AAVT_MEDIA_SESSION_LIMIT_REACHED;
        break;

    case DRM_E_DEVICE_ALREADY_REGISTERED:
    default:
        wResult = MTP_RESPONSECODE_WMDRMND_TRANSMITTER_FAILURE;
        break;
    }

    return wResult;
}


/**********************************************************************
**
** Function:    Drm_Pfd_Initialize
**
** Synopsis:    Initializes the PFD manager.  The context initialized with this function
**              is then used in subsequent PFD manager calls.
**
** Arguments:   [f_pPfdMgrContext]       -- The PFD DRM manager context
**              [f_pOEMContext]          -- Pointer to the OEM context; may be NULL
**              [f_pdstrDeviceStoreName] -- Name to use for the device
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Pfd_Initialize(
    __out          PFDMGR_CONTEXT   *f_pPfdMgrContext,
    __in_opt       DRM_VOID         *f_pOEMContext,
    __in     const DRM_CONST_STRING *f_pdstrDeviceStoreName )
{
    DRM_RESULT dr = DRM_SUCCESS;
    PFDMGR_CONTEXT_INTERNAL *pContext = (PFDMGR_CONTEXT_INTERNAL*) f_pPfdMgrContext;
    DRM_BOOL fAppContextInited = FALSE;
    DRM_BYTE *pbRevocationBuffer = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Initialize );

    ChkArg( f_pPfdMgrContext != NULL );

    ZEROMEM( f_pPfdMgrContext, SIZEOF( PFDMGR_CONTEXT ) );

    ChkDR( DRM_PtrList_Initialize( &pContext->ContentSessions, PFD_MAX_CONTENT_SESSIONS ) );

    ChkDR( Drm_Initialize( &pContext->AppContext,
                           f_pOEMContext,
                           pContext->rgbDrmBuffer,
                           SIZEOF( pContext->rgbDrmBuffer ),
                           f_pdstrDeviceStoreName ) );
    fAppContextInited = TRUE;

    ChkMem( pbRevocationBuffer = (DRM_BYTE *)Oem_MemAlloc( REVOCATION_BUFFER_SIZE ) );

    ChkDR( Drm_Revocation_SetBuffer( &pContext->AppContext,
                                     pbRevocationBuffer,
                                     REVOCATION_BUFFER_SIZE ) );

    ChkDR( Drm_Ndt_Initialize(&pContext->ndtMgrContext,
                              &pContext->AppContext) );

    pbRevocationBuffer = NULL;

ErrorExit:

    TRACE_IF_FAILED(("Drm_Pfd_Initialize failed 0x%lx\n",dr));

    if ( DRM_FAILED(dr) && fAppContextInited )
    {
        Drm_Uninitialize( &pContext->AppContext );
        SAFE_OEM_FREE( pbRevocationBuffer );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
**
** Function:    Drm_Pfd_Uninitialize
**
** Synopsis:    Releases any resources held by the PFD DRM manager.
**
** Arguments:   [f_pPfdMgrContext] -- The PFD manager context initialized by Drm_Pfd_Initialize
**
** Returns:     None.
**
**********************************************************************/
DRM_API DRM_VOID DRM_CALL Drm_Pfd_Uninitialize(
    __in     PFDMGR_CONTEXT *f_pPfdMgrContext
)
{
    PFDMGR_CONTEXT_INTERNAL *pContext = (PFDMGR_CONTEXT_INTERNAL*) f_pPfdMgrContext;
    PFDMGR_CONTENT_SESSION_CONTEXT_INTERNAL *pContentSession = NULL;
    DRM_APP_CONTEXT *pAppContext = NULL;
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  i = 0;
    DRM_BYTE  *pbRevocationBuffer = NULL;
    DRM_DWORD  cbRevocationBuffer = 0;

    ChkArg( f_pPfdMgrContext );

    for ( i = 0; i < pContext->ContentSessions.dwCurrentSize; i++ )
    {
        ChkDR( DRM_PtrList_GetByIndex( &pContext->ContentSessions, i, (DRM_VOID **)&pContentSession ) );

        SAFE_OEM_FREE( pContentSession->pbLicenseResponseMsg );

        if( pContentSession->hRawFile != OEM_INVALID_HANDLE_VALUE )
        {
            Oem_File_Close( pContentSession->hRawFile );
            pContentSession->hRawFile = OEM_INVALID_HANDLE_VALUE;
        }
        (DRM_VOID) Drm_Ndt_Asf_Close( &pContext->ndtMgrContext,
                                      pContentSession->idContentSession,
                                      &pAppContext );

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
    DRM_PtrList_Uninitialize( &pContext->ContentSessions );

    if( MEMCMP( g_idNULLSessionID.rgb, pContext->idRegistrationSession.rgb, DRM_ID_SIZE ) != 0 )
    {
        (DRM_VOID)Drm_Ndt_Session_Close( &pContext->ndtMgrContext, pContext->idRegistrationSession, NULL );
    }

    Drm_Ndt_Uninitialize( &pContext->ndtMgrContext );

    dr = Drm_Revocation_GetBuffer( &pContext->AppContext,
                                   &pbRevocationBuffer,
                                   &cbRevocationBuffer );

    Drm_Uninitialize( &pContext->AppContext );

    if( DRM_SUCCEEDED( dr ) )
    {
        SAFE_OEM_FREE( pbRevocationBuffer );
    }

    SAFE_OEM_FREE( pContext->pbRegistrationResponseMsg );
    SAFE_OEM_FREE( pContext->pbCurrentRegistrationRequestMsg );

    ZEROMEM( pContext, SIZEOF(PFDMGR_CONTEXT) );

ErrorExit:

    TRACE_IF_FAILED(("Drm_Pfd_Uninitialize failed 0x%lx\n",dr));
    return;
}


/**********************************************************************
**
** Function:    Drm_Pfd_Registration_ProcessRequest
**
** Synopsis:    Processes an incoming WMDRM-ND Registration Request message from a receiver.
**              The message's certificate is examined to see if it is valid.  If so, then a
**              transaction ID is generated and passed back for the receiver to use in future
**              registration calls.
**
** Arguments:   [f_pOEMContext]                  -- Pointer to the OEM context; may be NULL
**              [f_pPfdMgrContext]               -- The PFD manager context initialized by Drm_Pfd_Initialize
**              [f_pbRegistrationRequestMsg]     -- Array of bytes that contains WMDRM-ND Registration Request message
**              [f_cbRegistrationRequestMsg]     -- Count of bytes in f_pbRegistrationRequestMsg
**              [f_pdwRegistrationTransactionID] -- Transaction ID generated by this function for this registration process
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_ND_BAD_REQUEST if the message can't be parsed or
**              DRM_E_INVALID_DEVICE_CERTIFICATE if the certificate in the registration message is invalid
**
** Notes:       If any other device is registered or in the process of registering when this function
**              is called then its registration will be invalidated and all future calls from that device
**              will fail.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Pfd_Registration_ProcessRequest(
    __in_opt   DRM_VOID       *f_pOEMContext,
    __inout    PFDMGR_CONTEXT *f_pPfdMgrContext,
    __in_bcount(f_cbRegistrationRequestMsg) DRM_BYTE *f_pbRegistrationRequestMsg,
    __in const DRM_DWORD       f_cbRegistrationRequestMsg,
    __out      DRM_DWORD      *f_pdwRegistrationTransactionID
)
{
    DRM_RESULT               dr       = DRM_SUCCESS;
    PFDMGR_CONTEXT_INTERNAL *pContext = (PFDMGR_CONTEXT_INTERNAL*) f_pPfdMgrContext;
    NDTMGR_CONTEXT          *pNDTContext           = NULL;
    OEM_DEVICESTORE_CONTEXT *pOEMDevStoreContext   = NULL;
    DRM_BYTEBLOB             registrationRequest   = {NULL, 0};
    DRM_DWORD                cbResponse            = 0;
    DRM_ID                   idSession             = { 0 };
    OEM_DEVICE_HANDLE        hDeviceHandle         = (OEM_DEVICE_HANDLE) NULL;
    DRM_BYTEBLOB             blobDeviceCertificate = {NULL, 0};
    DRM_ID                   idDeviceSerialNumber  = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Registration_ProcessRequest );

    ChkArg( f_pPfdMgrContext               != NULL
         && f_pbRegistrationRequestMsg     != NULL
         && f_pdwRegistrationTransactionID != NULL );

    pNDTContext = &pContext->ndtMgrContext;

    ChkBOOL( pNDTContext != NULL, DRM_E_ND_INVALID_CONTEXT );

    pOEMDevStoreContext = &( (NDTMGR_CONTEXT_INTERNAL*)pNDTContext )->upperLayerDeviceStoreContext;

    ChkBOOL( pOEMDevStoreContext != NULL, DRM_E_ND_INVALID_CONTEXT );

    registrationRequest.pbBlob = f_pbRegistrationRequestMsg;
    registrationRequest.cbBlob = f_cbRegistrationRequestMsg;
    /* Create our registration session. */
    if( MEMCMP( g_idNULLSessionID.rgb, pContext->idRegistrationSession.rgb, DRM_ID_SIZE ) != 0 )
    {
        (void) Drm_Ndt_Session_Close( pNDTContext,
                                      pContext->idRegistrationSession,
                                      NULL );
        ZEROMEM( pContext->idRegistrationSession.rgb, DRM_ID_SIZE );
        pContext->dwRegistrationTransactionID = 0;
    }
    ChkDR( Drm_Ndt_Session_Create( f_pOEMContext,
                                   pNDTContext,
                                   NDTMGR_SESSION_TYPE_REGISTRATION,
                                   NULL,
                                   &idSession ) );

    if ( pContext->cbCurrentRegistrationRequestMsg == f_cbRegistrationRequestMsg
         && NULL != pContext->pbCurrentRegistrationRequestMsg
         && 0 == MEMCMP(pContext->pbCurrentRegistrationRequestMsg,
                        f_pbRegistrationRequestMsg,
                        f_cbRegistrationRequestMsg) )
    {
        /* The existing device is re-registering */
    }
    else if ( NULL == pContext->pbCurrentRegistrationRequestMsg )
    {
        /* No device has registered since the pfd context was initialized
         * or there was a registration failure
         */
    }
    else
    {
        /* There is a new device trying to register over the top of
         * an old device.
         * Need to:
         *    free up any open content sessions
         */

        _PfdDrmManagerFreeAllContentSessions( pContext );
    }

    dr = Drm_Ndt_Registration_ProcessRequest( f_pOEMContext,
                                              pNDTContext,
                                              idSession,
                                              registrationRequest,
                                              NULL,
                                              &cbResponse );
    if ( DRM_E_ND_DEVICE_LIMIT_REACHED == dr )
    {
        /* The device store filled up, get rid of a device before
         * registering a new one
         */
        ChkDR( DRM_DeviceStore_GetFirstDevice( pOEMDevStoreContext,
                                               &hDeviceHandle ) );
        dr = DRM_DeviceStore_GetDeviceInfo( hDeviceHandle,
                                            blobDeviceCertificate.pbBlob,
                                            &blobDeviceCertificate.cbBlob,
                                            &idDeviceSerialNumber );
        if ( DRM_E_BUFFERTOOSMALL == dr )
        {
            blobDeviceCertificate.pbBlob = (DRM_BYTE *) Oem_MemAlloc(blobDeviceCertificate.cbBlob);
            ChkMem(blobDeviceCertificate.pbBlob);
            dr = DRM_DeviceStore_GetDeviceInfo( hDeviceHandle,
                                                blobDeviceCertificate.pbBlob,
                                                &blobDeviceCertificate.cbBlob,
                                                &idDeviceSerialNumber );
        }
        ChkDR(dr);

        ChkDR( DRM_DeviceStore_UnRegisterDevice( pOEMDevStoreContext,
                                                 blobDeviceCertificate,
                                                 &idDeviceSerialNumber) );
        cbResponse = 0;
        dr = Drm_Ndt_Registration_ProcessRequest( f_pOEMContext,
                                                  pNDTContext,
                                                  idSession,
                                                  registrationRequest,
                                                  NULL,
                                                  &cbResponse);
    }

    if ( dr != DRM_E_BUFFERTOOSMALL && DRM_FAILED(dr) )
    {
        ChkDR( dr );
    }
    SAFE_OEM_FREE( pContext->pbRegistrationResponseMsg );
    ChkMem( pContext->pbRegistrationResponseMsg = (DRM_BYTE*)Oem_MemAlloc( cbResponse ) );
    ChkDR( Drm_Ndt_Registration_ProcessRequest( f_pOEMContext,
                                                pNDTContext,
                                                idSession,
                                                registrationRequest,
                                                pContext->pbRegistrationResponseMsg,
                                                &cbResponse ) );
    pContext->cbRegistrationResponseMsg = cbResponse;
    MEMCPY(&pContext->idRegistrationSession, &idSession, DRM_ID_SIZE);
    ZEROMEM( &idSession, DRM_ID_SIZE );
    do
    {
        Oem_Random_GetBytes( f_pOEMContext, (DRM_BYTE *) &pContext->dwRegistrationTransactionID, SIZEOF(DRM_DWORD));
    } while ( 0 == pContext->dwRegistrationTransactionID );
    *f_pdwRegistrationTransactionID = pContext->dwRegistrationTransactionID;

    /* Save registration information so new device registration can be
     * differentiated from a device re-registering
     */
    SAFE_OEM_FREE( pContext->pbCurrentRegistrationRequestMsg );
    pContext->pbCurrentRegistrationRequestMsg = (DRM_BYTE *) Oem_MemAlloc(f_cbRegistrationRequestMsg);
    ChkMem( pContext->pbCurrentRegistrationRequestMsg );
    MEMCPY( pContext->pbCurrentRegistrationRequestMsg, f_pbRegistrationRequestMsg, f_cbRegistrationRequestMsg);
    pContext->cbCurrentRegistrationRequestMsg = f_cbRegistrationRequestMsg;

ErrorExit:
    if(DRM_FAILED(dr) && pContext != NULL)
    {
        SAFE_OEM_FREE( pContext->pbRegistrationResponseMsg );
        SAFE_OEM_FREE( pContext->pbCurrentRegistrationRequestMsg );
        pContext->cbCurrentRegistrationRequestMsg = 0;
    }

    if( pContext != NULL && MEMCMP( g_idNULLSessionID.rgb, &idSession, DRM_ID_SIZE ) != 0 )
    {
        (DRM_VOID)Drm_Ndt_Session_Close( &pContext->ndtMgrContext, idSession, NULL );
    }

    SAFE_OEM_FREE( blobDeviceCertificate.pbBlob );
    if( hDeviceHandle != NULL )
    {
        DRM_DeviceStore_FreeDevice( hDeviceHandle );
    }

    TRACE_IF_FAILED(("Drm_Pfd_Registration_ProcessRequest failed 0x%lx\n",dr));

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    Drm_Pfd_Registration_GenerateResponse
**
** Synopsis:    Generates and returns the WMDRM-ND Registration Response message for the current transaction.
**
** Arguments:   [f_pPfdMgrContext]              -- The PFD manager context initialized by Drm_Pfd_Initialize
**              [f_dwRegistrationTransactionID] -- Registration transaction ID assigned in the Drm_Pfd_Registration_ProcessRequest call
**              [f_pbRegistrationResponseMsg]   -- User-allocated array of bytes that will hold the WMDRM-ND Registration Response message
**              [f_pcbRegistrationResponseMsg]  -- Count of bytes in f_pbRegistrationResponseMsg
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_ND_INVALID_SESSION if the transaction ID is invalid or
**              DRM_E_DEVICE_NOT_REGISTERED if the session has timed out or
**              DRM_E_BUFFERTOOSMALL if there's not enough bytes in f_pbRegistrationResponseMsg to hold the message
**
** Notes:       On return f_cbRegistrationResponseMsg will either hold the length of the Registration Response message or
**              the number of bytes needed to hold the message if the buffer is too small.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Pfd_Registration_GenerateResponse(
    __inout                                     const PFDMGR_CONTEXT *f_pPfdMgrContext,
    __in                                        const DRM_DWORD       f_dwRegistrationTransactionID,
    __out_bcount(*f_pcbRegistrationResponseMsg)       DRM_BYTE       *f_pbRegistrationResponseMsg,
    __inout                                           DRM_DWORD      *f_pcbRegistrationResponseMsg )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    const PFDMGR_CONTEXT_INTERNAL *pContext = (const PFDMGR_CONTEXT_INTERNAL*) f_pPfdMgrContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Registration_GenerateResponse );

    ChkArg( f_pPfdMgrContext             != NULL
         && f_pcbRegistrationResponseMsg != NULL
         && (*f_pcbRegistrationResponseMsg == 0
         || f_pbRegistrationResponseMsg != NULL) );

    /* Verify that this transaction ID is valid. */
    VERIFY_REGISTRATION_TRANSACTION_ID( f_dwRegistrationTransactionID );

    if(*f_pcbRegistrationResponseMsg < pContext->cbRegistrationResponseMsg)
    {
        *f_pcbRegistrationResponseMsg = pContext->cbRegistrationResponseMsg;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }
    MEMCPY(f_pbRegistrationResponseMsg, pContext->pbRegistrationResponseMsg, pContext->cbRegistrationResponseMsg);
    *f_pcbRegistrationResponseMsg = pContext->cbRegistrationResponseMsg;

ErrorExit:
    TRACE_IF_FAILED(("Drm_Pfd_Registration_GenerateResponse failed 0x%lx\n",dr));

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    Drm_Pfd_Proximity_GenerateChallenge
**
** Synopsis:    Initiates the proximity challenge phase of registration.
**              A 128-bit nonce is generated and returned through
**              the four f_pdwChallenge DWORD pointers in big-endian format,
**              with f_pdwChallenge1 holding the highest-order
**              DWORD and f_pdwChallenge4 holding the lowest-order DWORD.
**
** Arguments:   [f_pOEMContext]                 -- Pointer to the OEM context; may be NULL
**              [f_pPfdMgrContext]              -- The PFD manager context initialized by Drm_Pfd_Initialize
**              [f_dwRegistrationTransactionID] -- Registration transaction ID assigned in the Drm_Pfd_Registration_ProcessRequest call
**              [f_pdwChallenge1]               -- Nonce DWORD 1.  Highest-order DWORD.
**              [f_pdwChallenge2]               -- Nonce DWORD 2
**              [f_pdwChallenge3]               -- Nonce DWORD 3
**              [f_pdwChallenge4]               -- Nonce DWORD 4.  Lowest-order DWORD.
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_ND_INVALID_SESSION if the transaction ID is invalid or
**              DRM_E_DEVICE_NOT_REGISTERED if the session has timed out
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Pfd_Proximity_GenerateChallenge(
    __in_opt   DRM_VOID       *f_pOEMContext,
    __inout    PFDMGR_CONTEXT *f_pPfdMgrContext,
    __in const DRM_DWORD       f_dwRegistrationTransactionID,
    __out      DRM_DWORD      *f_pdwChallenge1,
    __out      DRM_DWORD      *f_pdwChallenge2,
    __out      DRM_DWORD      *f_pdwChallenge3,
    __out      DRM_DWORD      *f_pdwChallenge4
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    PFDMGR_CONTEXT_INTERNAL *pContext = (PFDMGR_CONTEXT_INTERNAL*) f_pPfdMgrContext;
    DRM_ID idChallenge;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Proximity_GenerateChallenge );

    ChkArg( f_pPfdMgrContext != NULL
         && f_pdwChallenge1  != NULL
         && f_pdwChallenge2  != NULL
         && f_pdwChallenge3  != NULL
         && f_pdwChallenge4  != NULL );

    /* Verify that this transaction ID is valid. */
    VERIFY_REGISTRATION_TRANSACTION_ID( f_dwRegistrationTransactionID );

    ChkDR( DRM_NDT_Proximity_GetChallenge( f_pOEMContext,
                                           &pContext->ndtMgrContext,
                                           pContext->idRegistrationSession,
                                           &idChallenge) );

    _UnpackDWORDSFromDRMID( &idChallenge,
                           f_pdwChallenge1,
                           f_pdwChallenge2,
                           f_pdwChallenge3,
                           f_pdwChallenge4 );

ErrorExit:
    TRACE_IF_FAILED(("Drm_Pfd_Proximity_GenerateChallenge failed 0x%lx\n",dr));
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    Drm_Pfd_Proximity_ProcessResponse
**
** Synopsis:    Concludes the proximity challenge phase of registration.
**              The 128-bit encrypted nonce from the receiver
**              is decrypted and compared against the original nonce.
**              If the numbers match and the response was received
**              within 7 milliseconds of the challenge then the registration process is successful.
**
** Arguments:   [f_pOEMContext]                 -- Pointer to the OEM context; may be NULL
**              [f_pPfdMgrContext]              -- The PFD manager context initialized by Drm_Pfd_Initialize
**              [f_dwRegistrationTransactionID] -- Registration transaction ID assigned in the Drm_Pfd_Registration_ProcessRequest call
**              [f_dwEncryptedChallenge1]       -- Encrypted nonce DWORD 1.  Highest-order DWORD.
**              [f_dwEncryptedChallenge2]       -- Encrypted nonce DWORD 2
**              [f_dwEncryptedChallenge3]       -- Encrypted nonce DWORD 3
**              [f_dwEncryptedChallenge4]       -- Encrypted nonce DWORD 4.  Lowest-order DWORD.
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_ND_INVALID_SESSION if the transaction ID is invalid or
**              DRM_E_DEVICE_NOT_REGISTERED if the session has timed out or
**              DRM_E_ND_UNABLE_TO_VERIFY_PROXIMITY if the proximity verification failed
**              DRM_E_ND_BAD_REQUEST if the state is incorrect
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Pfd_Proximity_ProcessResponse(
    __in_opt   DRM_VOID       *f_pOEMContext,
    __inout    PFDMGR_CONTEXT *f_pPfdMgrContext,
    __in const DRM_DWORD       f_dwRegistrationTransactionID,
    __in const DRM_DWORD       f_dwEncryptedChallenge1,
    __in const DRM_DWORD       f_dwEncryptedChallenge2,
    __in const DRM_DWORD       f_dwEncryptedChallenge3,
    __in const DRM_DWORD       f_dwEncryptedChallenge4
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_ID          idNonce;
    PFDMGR_CONTEXT_INTERNAL *pContext = (PFDMGR_CONTEXT_INTERNAL*) f_pPfdMgrContext;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Proximity_ProcessResponse );

    ChkArg( f_pPfdMgrContext != NULL );

    /* Verify that this transaction ID is valid. */
    VERIFY_REGISTRATION_TRANSACTION_ID( f_dwRegistrationTransactionID );

    /* Decrypt and match nonce. */
    _PackDWORDSInDRMID( &idNonce,
                        f_dwEncryptedChallenge1,
                        f_dwEncryptedChallenge2,
                        f_dwEncryptedChallenge3,
                        f_dwEncryptedChallenge4 );

    ChkDR( DRM_NDT_Proximity_VerifyEncryptedChallenge( f_pOEMContext,
                                                       &pContext->ndtMgrContext,
                                                       pContext->idRegistrationSession,
                                                       idNonce) );

ErrorExit:
    TRACE_IF_FAILED(("Drm_Pfd_Proximity_ProcessResponse failed 0x%lx\n",dr));
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    Drm_Pfd_License_ProcessRequest
**
** Synopsis:    Processes a license request for a media session established in the Drm_Pfd_Aavt_OpenMediaSession
**              call.
**
** Arguments:   [f_pOEMContext]         -- Pointer to the OEM context; may be NULL
**              [f_pPfdMgrContext]      -- The PFD manager context initialized by Drm_Pfd_Initialize
**              [f_dwSessionID]         -- The ID for this media session
**              [f_pbLicenseRequestMsg] -- Array of bytes that contains WMDRM-ND License Request message
**              [f_cbLicenseRequestMsg] -- Count of bytes in f_pbLicenseRequestMsg
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_ND_INVALID_SESSION if the media session ID is invalid or
**              DRM_E_ND_MUST_REVALIDATE if the registration session has timed out or
**              DRM_E_ND_BAD_REQUEST if the message can't be parsed or the requested action isn't available
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Pfd_License_ProcessRequest(
    __in_opt                               DRM_VOID       *f_pOEMContext,
    __inout                                PFDMGR_CONTEXT *f_pPfdMgrContext,
    __in const                             DRM_DWORD       f_dwSessionID,
    __in_bcount(f_cbLicenseRequestMsg)     DRM_BYTE       *f_pbLicenseRequestMsg,
    __in const                             DRM_DWORD       f_cbLicenseRequestMsg
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    PFDMGR_CONTEXT_INTERNAL *pContext = (PFDMGR_CONTEXT_INTERNAL*) f_pPfdMgrContext;
    PFDMGR_CONTENT_SESSION_CONTEXT_INTERNAL *pContentSession = NULL;
    DRM_BYTEBLOB licenseChallenge;
    DRM_KID kid;
    DRM_BYTE *pbLicenseResponse = NULL;
    DRM_DWORD cbLicenseResponse = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_License_ProcessRequest );

    ChkArg( f_pPfdMgrContext      != NULL
         && f_pbLicenseRequestMsg != NULL );

    ChkDR( _PfdDrmManagerGetSession( pContext, f_dwSessionID, &pContentSession ) );

    licenseChallenge.pbBlob = f_pbLicenseRequestMsg;
    licenseChallenge.cbBlob = f_cbLicenseRequestMsg;

    dr = Drm_Ndt_Asf_InitTranscryption( f_pOEMContext,
                                        &pContext->ndtMgrContext,
                                        pContentSession->idContentSession,
                                        licenseChallenge,
                                        &kid,
                                        NULL,
                                        &cbLicenseResponse );
    if( dr != DRM_E_BUFFERTOOSMALL && DRM_FAILED( dr ) )
    {
        ChkDR( dr );
    }

    pbLicenseResponse = (DRM_BYTE*)Oem_MemAlloc( cbLicenseResponse );
    ChkDR( Drm_Ndt_Asf_InitTranscryption( f_pOEMContext,
                                          &pContext->ndtMgrContext,
                                          pContentSession->idContentSession,
                                          licenseChallenge,
                                          &kid,
                                          pbLicenseResponse,
                                          &cbLicenseResponse ) );

    pContentSession->pbLicenseResponseMsg = pbLicenseResponse;
    pbLicenseResponse = NULL;
    pContentSession->cbLicenseResponseMsg = cbLicenseResponse;

ErrorExit:
    SAFE_OEM_FREE( pbLicenseResponse );
    if (DRM_FAILED( dr ))
    {
        TRACE_IF_FAILED(("Drm_Pfd_License_ProcessRequest failed 0x%lx\n",dr));
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    Drm_Pfd_License_GenerateResponse
**
** Synopsis:    Generates and returns a License Response message used to authorize content playback.
**
** Arguments:   [f_pPfdMgrContext]        -- The PFD manager context initialized by Drm_Pfd_Initialize
**              [f_dwSessionID]           -- The ID for this media session
**              [f_pbLicenseResponseMsg]  -- Array of bytes that contains WMDRM-ND License Response message
**              [f_pcbLicenseResponseMsg] -- Count of bytes in f_pbLicenseResponseMsg
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_ND_INVALID_SESSION if the media session ID is invalid or
**              DRM_E_ND_MUST_REVALIDATE if the registration session has timed out or
**              DRM_E_BUFFERTOOSMALL if there's not enough bytes in f_pbLicenseResponseMsg to hold the message
**
** Notes:       On return f_cbLicenseResponseMsg will either hold the length of the Registration Response message or
**              the number of bytes needed to hold the message if the buffer is too small.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Pfd_License_GenerateResponse(
    __inout                                const PFDMGR_CONTEXT *f_pPfdMgrContext,
    __in                                   const DRM_DWORD       f_dwSessionID,
    __out_bcount(*f_pcbLicenseResponseMsg)       DRM_BYTE       *f_pbLicenseResponseMsg,
    __inout                                      DRM_DWORD      *f_pcbLicenseResponseMsg )
{
    DRM_RESULT dr = DRM_SUCCESS;
    const PFDMGR_CONTEXT_INTERNAL *pContext = (const PFDMGR_CONTEXT_INTERNAL*) f_pPfdMgrContext;
    PFDMGR_CONTENT_SESSION_CONTEXT_INTERNAL *pContentSession = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_License_GenerateResponse );

    ChkArg( f_pPfdMgrContext        != NULL
         && f_pcbLicenseResponseMsg != NULL
         && (f_pbLicenseResponseMsg  != NULL || *f_pcbLicenseResponseMsg == 0)
    );

    ChkDR( _PfdDrmManagerGetSession( pContext, f_dwSessionID, &pContentSession ) );

    if( NULL == pContentSession->pbLicenseResponseMsg )
    {
        ChkDR( DRM_E_ND_BAD_REQUEST );
    }

    if(*f_pcbLicenseResponseMsg < pContentSession->cbLicenseResponseMsg)
    {
        *f_pcbLicenseResponseMsg = pContentSession->cbLicenseResponseMsg;
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    MEMCPY( f_pbLicenseResponseMsg, pContentSession->pbLicenseResponseMsg, pContentSession->cbLicenseResponseMsg );
    *f_pcbLicenseResponseMsg = pContentSession->cbLicenseResponseMsg;
    SAFE_OEM_FREE( pContentSession->pbLicenseResponseMsg );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    Drm_Pfd_Aavt_OpenMediaSession
**
** Synopsis:    Begins a media transfer session for the requested content.  The license information for the file is
**              extracted and returned to the caller.
**
** Arguments:   [f_pOEMContext]            -- Pointer to the OEM context; may be NULL
**              [f_pPfdMgrContext]         -- The PFD DRM manager context initialized by Drm_Pfd_Initialize
**              [f_pdstrDeviceStoreName]   -- Name to use for the device
**              [f_pwszFileName]           -- Wide character string that contains the name of the file to transfer
**              [f_wFileType]              -- PTP/MTP object format code for the file
**              [f_dwForceNewSession]      -- If 1, then a new session will be always be created, terminating any current session in progress
**              [f_fEncryptClear]          -- Whether or not clear content should be encrypted
**              [f_pdwSessionID]           -- Pointer to a DWORD that will return the ID for this media session
**              [f_pdwControlCapabilities] -- Pointer to a DWORD that will return the control capabilities supported by the device
**              [f_pdwDRMStatus]           -- Pointer to a DWORD that will return the DRM status for the requested content
**              [f_pdwLicenseStatus]       -- Pointer to a DWORD that will return information about the content's license
**              [f_pdwDRMDeliveryOptions]  -- Pointer to a DWORD that will return which media delivery mechanisms are available
**                                            for this content.
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_FILEOPEN if unable to open the file or
**              DRM_E_MEDIA_SESSION_LIMIT_REACHED if another session is open and f_dwForceNewSession is 0 or
**              DRM_E_FAIL on general file failure
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Pfd_Aavt_OpenMediaSession(
    __in_opt   DRM_VOID         *f_pOEMContext,
    __inout    PFDMGR_CONTEXT   *f_pPfdMgrContext,
    __in const DRM_CONST_STRING *f_pdstrDeviceStoreName,
    __in       DRM_WCHAR        *f_pwszFileName,
    __in const DRM_WORD          f_wFileType,
    __in const DRM_DWORD         f_dwForceNewSession,
    __in const DRM_BOOL          f_fEncryptClear,
    __out      DRM_DWORD        *f_pdwSessionID,
    __out      DRM_DWORD        *f_pdwControlCapabilities,
    __out      DRM_DWORD        *f_pdwDRMStatus,
    __out      DRM_DWORD        *f_pdwLicenseStatus,
    __out      DRM_DWORD        *f_pdwDRMDeliveryOptions )
{
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_STRING dstrFilePath;
    PFDMGR_CONTEXT_INTERNAL *pContext = NULL;
    PFDMGR_CONTENT_SESSION_CONTEXT_INTERNAL *pContentSession = NULL;
    PFDMGR_CONTENT_SESSION_CONTEXT_INTERNAL *pJunk = NULL;
    DRM_ID idContentSession;
    DRM_DWORD dwSessionID = 0;
    DRM_DWORD dwFlags = 0;
    DRM_BOOL fAppContextInited = FALSE;
    DRM_BYTE *pbRevocationBuffer = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Aavt_OpenMediaSession );

    ChkArg( f_pPfdMgrContext         != NULL
         && f_pwszFileName           != NULL
         && f_pdwSessionID           != NULL
         && f_pdwControlCapabilities != NULL
         && f_pdwDRMStatus           != NULL
         && f_pdwLicenseStatus       != NULL
         && f_pdwDRMDeliveryOptions  != NULL );

    pContext = (PFDMGR_CONTEXT_INTERNAL*) f_pPfdMgrContext;

    dstrFilePath.pwszString = f_pwszFileName;
    dstrFilePath.cchString = (DRM_DWORD) DRMCRT_wcslen(f_pwszFileName);

    if( MEMCMP( g_idNULLSessionID.rgb, pContext->idRegistrationSession.rgb, DRM_ID_SIZE ) != 0 )
    {
        ChkDR( Drm_Ndt_Session_Close( &pContext->ndtMgrContext, pContext->idRegistrationSession, NULL ) );
        ZEROMEM( pContext->idRegistrationSession.rgb, DRM_ID_SIZE );
        pContext->dwRegistrationTransactionID = 0;
    }

    /* Are we at the session limit? */
    if ( PFD_MAX_CONTENT_SESSIONS == pContext->ContentSessions.dwCurrentSize )
    {
        /* Should we force a new session? */
        if (f_dwForceNewSession)
        {
            /* Close the handle and start a new session. */
            PFDMGR_CONTENT_SESSION_CONTEXT_INTERNAL *pContentSessionToClose = NULL;

            ChkDR( DRM_PtrList_GetByIndex( &pContext->ContentSessions, 0, (DRM_VOID **)&pContentSessionToClose ) );
            _PfdDrmManagerFreeContentSession( pContext, pContentSessionToClose->dwSessionID );
        }
        else
        {
            /* Return an error. */
            ChkDR( DRM_E_ND_MEDIA_SESSION_LIMIT_REACHED );
        }
    }

    pContentSession = (PFDMGR_CONTENT_SESSION_CONTEXT_INTERNAL *) Oem_MemAlloc( SIZEOF( PFDMGR_CONTENT_SESSION_CONTEXT_INTERNAL ) );
    ChkMem( pContentSession );
    ZEROMEM( pContentSession,  SIZEOF( PFDMGR_CONTENT_SESSION_CONTEXT_INTERNAL ) );
    pContentSession->hRawFile = OEM_INVALID_HANDLE_VALUE;

#if DRM_SUPPORT_AUDIO_ONLY == 0
    if( ( f_wFileType == WMDRMND_FORMATCODE_ASF )
        || ( f_wFileType == WMDRMND_FORMATCODE_WMA )
        || ( f_wFileType == WMDRMND_FORMATCODE_WMV )
        || ( f_wFileType == WMDRMND_FORMATCODE_UNDEFINED_VIDEO ) )
#else
    if( ( f_wFileType == WMDRMND_FORMATCODE_ASF )
        || ( f_wFileType == WMDRMND_FORMATCODE_WMA ) )
#endif /* DRM_SUPPORT_AUDIO_ONLY */

    {
        ChkDR( Drm_Initialize( &pContentSession->AppContext,
                               f_pOEMContext,
                               pContentSession->rgbDrmBuffer,
                               SIZEOF( pContentSession->rgbDrmBuffer ),
                               f_pdstrDeviceStoreName ) );

        fAppContextInited = TRUE;

        ChkMem( pbRevocationBuffer = (DRM_BYTE *)Oem_MemAlloc( REVOCATION_BUFFER_SIZE ) );

        ChkDR( Drm_Revocation_SetBuffer( &pContentSession->AppContext,
                                         pbRevocationBuffer,
                                         REVOCATION_BUFFER_SIZE ) );

        ChkDR( Drm_Ndt_Asf_Init( f_pOEMContext,
                                 &pContext->ndtMgrContext,
                                 dstrFilePath,
                                 NDT_ASFDELIVERY_MODE_MUXED,
                                 &pContentSession->AppContext,
                                 f_fEncryptClear,
                                 &idContentSession,
                                 &dwFlags ) );

        pbRevocationBuffer = NULL;

        pContentSession->idContentSession = idContentSession;

#if DRM_SUPPORT_AUDIO_ONLY
        *f_pdwControlCapabilities = TRUE;
#else
        /* Seeking is not currently supported for video files. */
        *f_pdwControlCapabilities = FALSE;
#endif /* DRM_SUPPORT_AUDIO_ONLY */

        if(dwFlags & NDTMGR_ASF_WMDRMPROTECTED)
        {
            *f_pdwDRMStatus           = TRUE;
        }
        else
        {
            *f_pdwDRMStatus           = FALSE;
        }

        if(dwFlags & NDTMGR_ASF_LICENSE_PLAYAVAILABLE)
        {
            *f_pdwLicenseStatus       = TRUE;
            *f_pdwDRMDeliveryOptions  = TRUE;
        }
        else
        {
            *f_pdwLicenseStatus       = FALSE;
            *f_pdwDRMDeliveryOptions  = FALSE;
        }
    }
    else
    {
        pContentSession->hRawFile = Oem_File_Open( f_pOEMContext,
                                                   f_pwszFileName,
                                                   OEM_GENERIC_READ,
                                                   OEM_FILE_SHARE_READ,
                                                   OEM_OPEN_EXISTING,
                                                   OEM_ATTRIBUTE_NORMAL );
        if( pContentSession->hRawFile == OEM_INVALID_HANDLE_VALUE )
        {
            ChkDR( DRM_E_FILEOPEN );
        }

        *f_pdwControlCapabilities = FALSE;
        *f_pdwDRMStatus = FALSE;
        *f_pdwLicenseStatus = FALSE;
        *f_pdwDRMDeliveryOptions = FALSE;
    }


    /*
     * Randomly assign the session ID and make sure it's not an invalid value and doesn't collide with any
     * previous session IDs.
     */
    do
    {
        Oem_Random_GetBytes( f_pOEMContext, (DRM_BYTE*) &dwSessionID, SIZEOF( DRM_DWORD ) );
    } while ( (dwSessionID == 0)
              || (dwSessionID == MAX_UNSIGNED_TYPE( DRM_DWORD ))
              || (DRM_SUCCESS == _PfdDrmManagerGetSession( pContext, dwSessionID, &pJunk)) );
    *f_pdwSessionID = dwSessionID;
    pContentSession->dwSessionID = dwSessionID;

    ChkDR( DRM_PtrList_AddTail( &pContext->ContentSessions, pContentSession ) );
    pContentSession = NULL;

ErrorExit:

    if ( DRM_FAILED( dr ) )
    {
        (DRM_VOID) _PfdDrmManagerFreeContentSession( pContext, dwSessionID );

        if ( fAppContextInited )
        {
            Drm_Uninitialize( &pContentSession->AppContext );
            SAFE_OEM_FREE( pbRevocationBuffer );
        }
        SAFE_OEM_FREE( pContentSession );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
**
** Function:    Drm_Pfd_Aavt_CloseMediaSession
**
** Synopsis:    Closes the media transfer session and releases any asssociated resources.
**
** Arguments:   [f_pPfdMgrContext] -- The PFD DRM manager context initialized by Drm_Pfd_Initialize
**              [f_dwSessionID]    -- The media transfer session ID assigned by Drm_Pfd_Aavt_OpenMediaSession
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_ND_INVALID_SESSION if the given session ID is not valid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Pfd_Aavt_CloseMediaSession(
    __inout    PFDMGR_CONTEXT   *f_pPfdMgrContext,
    __in const DRM_DWORD         f_dwSessionID
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    PFDMGR_CONTEXT_INTERNAL *pContext = (PFDMGR_CONTEXT_INTERNAL*) f_pPfdMgrContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Aavt_CloseMediaSession );

    ChkArg( f_pPfdMgrContext != NULL );

    ChkDR( _PfdDrmManagerFreeContentSession( pContext, f_dwSessionID ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
**
** Function:    Drm_Pfd_Aavt_GetNextDataBlock
**
** Synopsis:    Reads the next block of data from the currently open media transfer session.
**
** Arguments:   [f_pOEMContext]      -- Pointer to the OEM context; may be NULL
**              [f_pPfdMgrContext]   -- The PFD DRM manager context initialized by Drm_Pfd_Initialize
**              [f_dwSessionID]      -- The media transfer session ID assigned by Drm_Pfd_Aavt_OpenMediaSession
**              [f_pdwEndOfData]     -- Will be set to 1 if the end-of-file is reached, 0 otherwise
**              [f_pbNextDataBlock]  -- User-allocated array of bytes that will hold the data read
**              [f_pcbNextDataBlock] -- Count of bytes to read into f_pcbNextDataBlock; on return this will contain the
**                                      number of bytes read
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_ND_INVALID_SESSION if the given session ID is not valid or
**              DRM_E_LICENSE_NOT_FOUND if a license must be aquired to deliver this content or
**              DRM_E_FILE_READ_ERROR if an error occurred reading the file or
**              DRM_E_NOMORE if the end-of-file is reached
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Pfd_Aavt_GetNextDataBlock(
    __in_opt   DRM_VOID       *f_pOEMContext,
    __inout    PFDMGR_CONTEXT *f_pPfdMgrContext,
    __in const DRM_DWORD       f_dwSessionID,
    __out      DRM_DWORD      *f_pdwEndOfData,
    __out_bcount(*f_pcbNextDataBlock) DRM_BYTE *f_pbNextDataBlock,
    __out      DRM_DWORD      *f_pcbNextDataBlock
)
{
    DRM_RESULT                               dr              = DRM_SUCCESS;
    PFDMGR_CONTEXT_INTERNAL                 *pContext        = (PFDMGR_CONTEXT_INTERNAL*) f_pPfdMgrContext;
    PFDMGR_CONTENT_SESSION_CONTEXT_INTERNAL *pContentSession = NULL;
    DRM_DWORD                                cbWanted        = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Aavt_GetNextDataBlock );

    ChkArg( f_pPfdMgrContext   != NULL );
    ChkArg( f_pcbNextDataBlock != NULL );
    ChkArg( f_pbNextDataBlock  != NULL || *f_pcbNextDataBlock == 0 );
    ChkArg( f_pdwEndOfData     != NULL );

    ChkDR( _PfdDrmManagerGetSession( pContext, f_dwSessionID, &pContentSession ) );

    *f_pdwEndOfData = 0;
    if ( pContentSession->hRawFile != OEM_INVALID_HANDLE_VALUE )
    {
        cbWanted = *f_pcbNextDataBlock;
        if ( !Oem_File_Read( pContentSession->hRawFile, (DRM_VOID*)f_pbNextDataBlock, *f_pcbNextDataBlock, f_pcbNextDataBlock ) )
        {
            ChkDR( DRM_E_FILE_READ_ERROR );
        }

        /*
        ** Check EOF, which is affirmed if the number of bytes read is less than what we wanted
        */
        *f_pdwEndOfData = ( *f_pcbNextDataBlock < cbWanted ) ? (DRM_DWORD)1 : (DRM_DWORD)0;
    }
    else
    {
        dr = Drm_Ndt_Asf_ReadData( f_pOEMContext,
                                   &pContext->ndtMgrContext,
                                   pContentSession->idContentSession,
                                   f_pbNextDataBlock,
                                   f_pcbNextDataBlock );
        *f_pdwEndOfData = ( dr == DRM_E_NOMORE ? (DRM_DWORD)1 : (DRM_DWORD)0);
        ChkDR( dr );
    }

    if ( *f_pdwEndOfData == 1 )
    {
        ChkDR( DRM_E_NOMORE );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
**
** Function:    Drm_Pfd_Aavt_SetCurrentTimePosition
**
** Synopsis:    Seeks to the specified time position in the file
**
** Arguments:   [f_pOEMContext]                -- Pointer to the OEM context; may be NULL
**              [f_pPfdMgrContext]             -- The PFD DRM manager context initialized by Drm_Pfd_Initialize
**              [f_dwSessionID]                -- The media transfer session ID assigned by Drm_Pfd_Aavt_OpenMediaSession
**              [f_dwTimeOffsetInMilliseconds] -- Time to seek to in milliseconds
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid or
**              DRM_E_ND_INVALID_SESSION if the given session ID is not valid or
**              DRM_E_FAIL if the time position was not able to be set
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Pfd_Aavt_SetCurrentTimePosition(
    __in_opt   DRM_VOID       *f_pOEMContext,
    __inout    PFDMGR_CONTEXT *f_pPfdMgrContext,
    __in const DRM_DWORD       f_dwSessionID,
    __in const DRM_DWORD       f_dwTimeOffsetInMilliseconds
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    PFDMGR_CONTEXT_INTERNAL *pContext = (PFDMGR_CONTEXT_INTERNAL*) f_pPfdMgrContext;
    PFDMGR_CONTENT_SESSION_CONTEXT_INTERNAL *pContentSession = NULL;
    DRM_UINT64 qwSeekTimeNS = DRM_UI64LITERAL( 0, 0 );

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_Drm_Pfd_Aavt_SetCurrentTimePosition );

    ChkArg( f_pPfdMgrContext != NULL );

    ChkDR( _PfdDrmManagerGetSession( pContext, f_dwSessionID, &pContentSession ) );

    if( pContentSession->hRawFile != OEM_INVALID_HANDLE_VALUE )
    {
        ChkDR( DRM_E_ND_BAD_REQUEST );
    }

    /* Convert time offset to correct units for new NDT call */
    qwSeekTimeNS = DRM_UI64Mul( DRM_UI64( MS_TO_HNS_MULTIPLIER ),
                                DRM_UI64( f_dwTimeOffsetInMilliseconds ) );
    if( DRM_FAILED( Drm_Ndt_Asf_Seek( f_pOEMContext,
                                       &pContext->ndtMgrContext,
                                       pContentSession->idContentSession,
                                       qwSeekTimeNS ) ) )
    {
        ChkDR( DRM_E_ND_BAD_REQUEST );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

