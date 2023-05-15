/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMNDR_C
#include <ndtnetmsg.h>
#include <drmndr.h>
#include <drmxmr.h>
#include <ndtreg.h>
#include <oemrsaoaeppss.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/*****************************************************************************
** Function:    Drm_Ndr_Registration_GenerateChallenge
**
** Synopsis:    Gets all of the data needed for the WMDRMNET registration message.
**
** Arguments:   [f_pOEMContext]    : A pointer to the OEM context; may be NULL
**              [f_ppbMessage]     : Returns the registration message
**              [f_pcbMessageSize] : Returns the size (in bytes) of the registration message
**
** Returns:     DRM_SUCCESS - on success
**              or other return code from a function called within
**
** Notes:       The returned buffer must be freed using OEM_free.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndr_Registration_GenerateChallenge(
    __in_opt DRM_VOID                                    *f_pOEMContext,
    __deref_out_bcount_full(*f_pcbMessageSize) DRM_BYTE **f_ppbMessage,
    __out DRM_DWORD                                      *f_pcbMessageSize
)
{
    DRM_RESULT  dr                  = DRM_SUCCESS;
    DRM_BYTE   *pbDeviceCertificate = NULL;
    DRM_DWORD   cbDeviceCertificate = 0;
    DRM_ID      idSerialNumber      = {0};
    DRM_DWORD   cbSerialNumber      = DRM_ID_SIZE;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_Registration_GenerateChallenge );

    /*
    ** Get the Device certificate stored in the hardware
    */
    dr = Oem_Device_GetCert( f_pOEMContext,
                             DRM_DCT_NETRECEIVER,
                             pbDeviceCertificate,
                             &cbDeviceCertificate );
    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbDeviceCertificate = (DRM_BYTE *)Oem_MemAlloc( cbDeviceCertificate ) );
        ChkDR( Oem_Device_GetCert( f_pOEMContext,
                                   DRM_DCT_NETRECEIVER,
                                   pbDeviceCertificate,
                                   &cbDeviceCertificate ) );
    }
    else
    {
        goto ErrorExit;
    }

    /*
    ** Get the Serial Number stored in the hardware
    */
    ChkDR( Oem_Device_GetUniqueId( f_pOEMContext,
                                    DRM_DIT_PLAYREADY_ID,
                                    (DRM_BYTE *)&idSerialNumber,
                                    &cbSerialNumber ));

    /*
    ** Build a registration message
    */
    ChkDR( WmdrmNetPackRegistrationMessage( &idSerialNumber,
                                            pbDeviceCertificate,
                                            cbDeviceCertificate,
                                            f_ppbMessage,
                                            f_pcbMessageSize ) );

ErrorExit:
    SAFE_OEM_FREE( pbDeviceCertificate );
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    Drm_Ndr_Registration_ProcessResponse
**
** Synopsis:    Processes a received RegistrationResponse message.
**
** Arguments:   [f_pOEMContext]                       : A pointer to the OEM context; may be NULL
**              [f_pProximityContext]                 : The context of the operation
**              [f_pbRegistrationResponseMessage]     : The registration response message
**              [f_cbRegistrationResponseMessageSize] : The size (in bytes) of the registration
**                                                      response message
**
** Returns:     DRM_SUCCESS              - on success
**              DRM_E_ND_INVALID_MESSAGE - if the serial numbers do not match
**              or other return code from a function called within
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndr_Registration_ProcessResponse(
    __in_opt   DRM_VOID                                       *f_pOEMContext,
    __in       DRM_ND_PROXIMITY_CONTEXT                       *f_pProximityContext,
    __in_bcount(f_cbRegistrationResponseMessageSize) DRM_BYTE *f_pbRegistrationResponseMessage,
    __in const DRM_DWORD                                       f_cbRegistrationResponseMessageSize,
    __in       DRM_CRYPTO_CONTEXT                             *f_pCryptoCtx
)
{
    DRM_RESULT                   dr                    = DRM_SUCCESS;
    DRM_ID                       idSeed                = {0};
    DRM_ID                       idSerialNumber        = {0};
    DRM_DWORD                    cbSerialNumber        = DRM_ID_SIZE;
    DRM_ID                       idMessageSerialNumber = {0};
    DRM_BYTE                    *pbAddress             = NULL;
    DRM_RSA_PRIVATE_KEY_2048BIT  oPrivateKey           = {0};
    DRM_DWORD                    cbPrivateKey          = SIZEOF( DRM_RSA_PRIVATE_KEY_2048BIT );


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_Registration_ProcessResponse );

    ChkDR( Oem_Device_GetKey( f_pOEMContext,
                              DRM_DKT_PLAYREADY_NDR,
                              (DRM_BYTE*) &oPrivateKey,
                              &cbPrivateKey ) );

    /*
    ** Unpack the message
    */
    dr = WmdrmNetUnpackRegistrationResponseMessage( &oPrivateKey,
                                                    f_pbRegistrationResponseMessage,
                                                    f_cbRegistrationResponseMessageSize,
                                                    &idSeed,
                                                    &idMessageSerialNumber,
                                                    &f_pProximityContext->SessionId,
                                                    &pbAddress,
                                                    &f_pProximityContext->AddressSize,
                                                    f_pCryptoCtx );

    if ( dr != DRM_SUCCESS)
    {
        TRACE(( "Receiver: Cannot unpack RegistrationResponse message %lx\n", dr ));
        goto ErrorExit;
    }

    /*
    ** Ensure the Serial Number matches that of this device
    */
    ChkDR( Oem_Device_GetUniqueId( f_pOEMContext,
                                   DRM_DIT_PLAYREADY_ID,
                                   (DRM_BYTE *)&idSerialNumber,
                                   &cbSerialNumber ));

    if ( MEMCMP( &idSerialNumber, &idMessageSerialNumber, cbSerialNumber ) != 0 )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "Receiver: RegistrationResponseMessage contains invalid Serial Number\n" ));
        goto ErrorExit;
    }

    /*
    ** Copy the address to the context
    */
    f_pProximityContext->Address = (DRM_BYTE *)Oem_MemAlloc( f_pProximityContext->AddressSize + (DRM_DWORD)1 );
    ChkMem( f_pProximityContext->Address );

    ChkArg( pbAddress != NULL );
    MEMCPY( f_pProximityContext->Address, pbAddress, f_pProximityContext->AddressSize );
    f_pProximityContext->Address[f_pProximityContext->AddressSize] = 0;
    TRACE(( "Receiver: Respond to address '%s'.\n", f_pProximityContext->Address ));

    /*
    ** Validate that the Address is one this device recognizes
    **
    */

    /*
    ** Compute the registration content encryption key
    */
    f_pProximityContext->ProximityContentEncryptionKey = WmdrmNetGetRegistrationKey( &idSeed, FALSE );
    ChkMem( f_pProximityContext->ProximityContentEncryptionKey );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    OEM_RSA_ZeroPrivateKey_2048BIT( &oPrivateKey );
    return dr;
}



/*****************************************************************************
** Function:    Drm_Ndr_License_GenerateChallenge
**
** Synopsis:    Gets all of the data needed for the WMDRMNET License request message.
**
** Arguments:   [f_pOEMContext]    : A pointer to the OEM context; may be NULL
**              [f_pRightsId]      : Returns the Rights ID for this request
**              [f_ppbMessage]     : Returns the License request message
**              [f_pcbMessageSize] : Returns the size (in bytes) of the license request message
**
** Returns:     DRM_SUCCESS - on success
**              or other return code from a function called within
**
** Notes:       The returned buffer must be freed using OEM_free.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndr_License_GenerateChallenge(
    __in_opt DRM_VOID                                     *f_pOEMContext,
    __out DRM_ID                                          *f_pRightsId,
    __deref_out_bcount_full(*f_pcbMessageSize) DRM_BYTE  **f_ppbMessage,
    __out DRM_DWORD                                       *f_pcbMessageSize
)
{
    DRM_RESULT  dr                  = DRM_SUCCESS;
    DRM_BYTE   *pbDeviceCertificate = NULL;
    DRM_DWORD   cbDeviceCertificate = 0;
    DRM_ID      idSerialNumber      = {0};
    DRM_DWORD   cbSerialNumber      = DRM_ID_SIZE;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_License_GenerateChallenge );

    /*
    ** Get the Device certificate stored in the hardware
    */
    dr = Oem_Device_GetCert( f_pOEMContext,
                             DRM_DCT_NETRECEIVER,
                             pbDeviceCertificate,
                             &cbDeviceCertificate );

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbDeviceCertificate = (DRM_BYTE *)Oem_MemAlloc( cbDeviceCertificate ) );
        ChkDR( Oem_Device_GetCert( f_pOEMContext,
                                   DRM_DCT_NETRECEIVER,
                                   pbDeviceCertificate,
                                   &cbDeviceCertificate ) );
    }
    else
    {
        goto ErrorExit;
    }

    /*
    ** Get the Serial Number stored in the hardware
    */
    ChkDR( Oem_Device_GetUniqueId( f_pOEMContext,
                                   DRM_DIT_PLAYREADY_ID,
                                   (DRM_BYTE *)&idSerialNumber,
                                   &cbSerialNumber ));

    /*
    ** Compute a random rights id
    */
    ChkDR( Oem_Random_GetBytes( f_pOEMContext, (DRM_BYTE *)f_pRightsId, sizeof( DRM_ID ) ) );

    /*
    ** Build a License request message
    */
    dr = WmdrmNetPackLicenseRequestMessage( f_pRightsId,
                                            &idSerialNumber,
                                            pbDeviceCertificate,
                                            cbDeviceCertificate,
                                            (DRM_BYTE *)WMDRMNET_PLAY_ACTION,
                                            WMDRMNET_PLAY_ACTION_SIZE,
                                            f_ppbMessage,
                                            f_pcbMessageSize );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    SAFE_OEM_FREE( pbDeviceCertificate );
    return dr;
}



/*****************************************************************************
** Function:    Drm_Ndr_License_ProcessResponse
**
** Synopsis:    Processes a received License Response message.
**
** Arguments:   [f_pOEMContext]                  : A pointer to the OEM context; may be NULL
**              [f_pProximityContext]            : The context of the operation
**              [f_pbLicenseResponseMessage]     : The License response message
**              [f_cbLicenseResponseMessageSize] : The size (in bytes) of the license response message
**              [f_pStack]                       : Pointer to a stack allocator context
**              [f_pfnPolicyCallback]            : Pointer to callback function for output restrictions; may be NULL
**              [f_pv]                           : Pointer to void pass-through parameter for callback; may be NULL
**              [f_pcontextDCRY]                 : Pointer to a decryption context
**              [f_pCryptoCtx]                   : Pointer to a crypto context
**
** Returns:     DRM_SUCCESS - on success
**              DRM_E_ND_INVALID_MESSAGE - if there is a serial number or rights ID mismatch
**              or other return code from a function called within
**
** Notes:       This function only works for 1024-bit RSA private keys.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndr_License_ProcessResponse(
    __in_opt       DRM_VOID                              *f_pOEMContext,
    __in           DRM_ND_PROXIMITY_CONTEXT              *f_pProximityContext,
    __in_bcount(f_cbLicenseResponseMessageSize) DRM_BYTE *f_pbLicenseResponseMessage,
    __in const     DRM_DWORD                              f_cbLicenseResponseMessageSize,
    __in           DRM_STACK_ALLOCATOR_CONTEXT           *f_pStack,
    __in_opt       DRMPFNPOLICYCALLBACK                   f_pfnPolicyCallback,
    __in_opt const DRM_VOID                              *f_pv,
    __out          DRM_DECRYPT_CONTEXT                   *f_pcontextDCRY,
    __in           DRM_CRYPTO_CONTEXT                    *f_pCryptoCtx
)
{
    DRM_RESULT                            dr                = DRM_SUCCESS;
    DRM_ID                                idSerialNumber    = {0};
    DRM_DWORD                             cbSerialNumber    = DRM_ID_SIZE;
    DRM_BYTE                             *pbLicense         = NULL;
    DRM_DWORD                             cbLicense         = 0;
    DRM_XMR_LICENSE                       XMRLicense        = {0};
    DRM_RSA_PRIVATE_KEY_2048BIT           oPrivateKey       = {0};
    DRM_DWORD                             cbDevicePvtKey    = SIZEOF( DRM_RSA_PRIVATE_KEY_2048BIT );
    DRM_BYTE                              rgbPlaintext[ __CB_DECL( DRM_RSA_CB_MAX_PLAINTEXT_MAX_2048BIT ) ] = { 0 };
    DRM_DWORD                             cbPlaintext       = 0;  /* set appropriately below */
    DRM_DWORD                             iGUID             = 0;
    DRM_INCLUSION_LIST_CALLBACK_STRUCT    ilcs              = {0};
    DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT eKeyLength        = eDRM_RSA_KEY_LENGTH_UNKNOWN_2048BIT;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_License_ProcessResponse );

    /*
    ** Unpack the License response message
    */
    dr = WmdrmNetUnpackLicenseResponseMessage( f_pbLicenseResponseMessage,
                                               f_cbLicenseResponseMessageSize,
                                               &pbLicense,
                                               &cbLicense );

    if ( dr != DRM_SUCCESS )
    {
        TRACE(( "Receiver: Cannot process the License response message. %lx\n", dr ));
        goto ErrorExit;
    }

    /*
    ** Parse the License
    */
    TRACE(( "Receiver: License received: %ld\n", cbLicense ));

    ChkArg( pbLicense != NULL );
    dr = DRM_XMR_UnpackLicense( pbLicense, cbLicense, f_pStack, &XMRLicense );
    if ( dr != DRM_SUCCESS )
    {
        TRACE(( "Receiver: Cannot unpack XMR License. %lx\n", dr ));
        goto ErrorExit;
    }

    TRACE(( "Receiver: License unpacked\n" ));

    /*
    ** Verify that the Serial Number matches
    */
    ChkDR( Oem_Device_GetUniqueId( f_pOEMContext,
                                   DRM_DIT_PLAYREADY_ID,
                                   (DRM_BYTE *)&idSerialNumber,
                                   &cbSerialNumber ));

    if ( MEMCMP( &idSerialNumber,
                  XMRLicense.containerOuter.containerGlobalPolicies.SerialNumber.pbDataBuffer +
                    XMRLicense.containerOuter.containerGlobalPolicies.SerialNumber.iData,
                  cbSerialNumber ) != 0 )
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "Receiver: XMR License contains invalid Serial Number\n" ));
        goto ErrorExit;
    }

    /*
    ** Verify that the Rights ID matches
    */
    if ( MEMCMP( &(f_pProximityContext->RightsId),
                  XMRLicense.pbRightsIdBuffer + XMRLicense.iRightsId,
                  sizeof(f_pProximityContext->RightsId) ) != 0)
    {
        dr = DRM_E_ND_INVALID_MESSAGE;
        TRACE(( "Receiver: XMR License contains invalid Rights ID\n" ));
        goto ErrorExit;
    }

    /*
    ** Decrypt the Content Key and initialize the decryption context passed in
    */
    ChkDR( Oem_Device_GetKey( f_pOEMContext,
                               DRM_DKT_PLAYREADY_NDR,
                               (DRM_BYTE *)&oPrivateKey,
                               &cbDevicePvtKey ) );

    ChkDR( OEM_RSA_GetPrivateKeyLength_2048BIT( &oPrivateKey, &eKeyLength ) );
    ChkBOOL( eKeyLength <= eDRM_RSA_MAXIMUM_KEY_LENGTH_2048BIT, DRM_E_PRIVKEY_READ_ERROR );

    cbPlaintext = DRM_RSA_CB_MAX_PLAINTEXT( (DRM_DWORD)eKeyLength );

    ChkDR( OEM_RSA_OaepDecrypt_2048BIT( &oPrivateKey,
                                        XMRLicense.containerOuter.containerKeys.ContentKey.cbEncryptedKey,
                                        XMRLicense.containerOuter.containerKeys.ContentKey.pbEncryptedKeyBuffer /* license  */
                                            + XMRLicense.containerOuter.containerKeys.ContentKey.iEncryptedKey, /* + offset */
                                        &cbPlaintext,
                                        rgbPlaintext,
                                        f_pCryptoCtx ) );

    /*
    ** Within the plaintext buffer, the content key is followed by the content integrity key.
    ** Each must be of length DRM_AES_KEYSIZE_128 bytes.
    */
    ChkBOOL( cbPlaintext == DRM_AES_KEYSIZE_128 * 2, DRM_E_RSA_DECRYPTION_ERROR );
    ChkDR( DRM_CPHR_Init( (DRM_CIPHER_CONTEXT *)f_pcontextDCRY,
                           eDRM_AES_COUNTER_CIPHER,
                           DRM_AES_KEYSIZE_128,
                           rgbPlaintext ) );  /* the content key */

    /*
    ** Callbacks
    */
    if ( f_pfnPolicyCallback != NULL )
    {
        /*
        ** Enforce playback policy parameters from the parsed license
        */
        ChkDR( f_pfnPolicyCallback( &(XMRLicense.containerOuter.containerPlaybackPolicies),
                                     DRM_PLAY_OPL_CALLBACK,
                                     f_pv ) );

        /*
        ** Enforce inclusion-list GUID parameters from the parsed license
        */
        ilcs.dwChainDepth = 1;
        for ( iGUID = 0;
              ( ( iGUID < XMRLicense.containerOuter.containerGlobalPolicies.InclusionList.cGUIDs )
                 && ( iGUID < DRM_MAX_INCLUSION_GUIDS ) );
              iGUID++ )
        {
            ilcs.rgfInclusionListValid[iGUID][ilcs.dwChainDepth] = XMRLicense.containerOuter.containerGlobalPolicies.InclusionList.fValid;
            MEMCPY( (DRM_BYTE *)&(ilcs.rgInclusionList[iGUID]),
                    XMRLicense.containerOuter.containerGlobalPolicies.InclusionList.pguidBuffer + ( iGUID * SIZEOF( DRM_GUID ) ),
                    SIZEOF( DRM_GUID ) );
        }

        ChkDR( f_pfnPolicyCallback( &ilcs, DRM_INCLUSION_LIST_CALLBACK, f_pv ) );
    }


ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    OEM_RSA_ZeroPrivateKey_2048BIT( &oPrivateKey );
    return dr;
}



/*****************************************************************************
** Function:    Drm_NDR_Proximity_GenerateStart
**
** Synopsis:    Generates a proximity start message to send to the transmitter.
**
** Arguments:   [f_pOEMContext]       : A pointer to the OEM context; may be NULL
**              [f_pProximityContext] : The context of the operation
**              [f_ppbMessage]        : Returns the proximity start message
**              [f_pcbMessageSize]    : Returns the size (in bytes) of the message
**
** Returns:     DRM_SUCCESS - on success
**              or other return code from a function called within
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndr_Proximity_GenerateStart(
    __in_opt                                         DRM_VOID                  *f_pOEMContext,
    __in                                       const DRM_ND_PROXIMITY_CONTEXT  *f_pProximityContext,
    __deref_out_bcount_full(*f_pcbMessageSize)       DRM_BYTE                 **f_ppbMessage,
    __out                                            DRM_DWORD                 *f_pcbMessageSize )
{
    DRM_RESULT   dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_Proximity_GenerateStart );

    /*
    ** Build the Proximity Start message
    */
    dr = WmdrmNetPackProximityStartMessage( &f_pProximityContext->SessionId,
                                            f_ppbMessage,
                                            f_pcbMessageSize );

    if ( dr != DRM_SUCCESS )
    {
        TRACE(( "Cannot build proximity start message. %lx\n", dr ));
        goto ErrorExit;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    Drm_Ndr_Proximity_GenerateResponse
**
** Synopsis:    Generates a proximity response message to send to the transmitter.
**
** Arguments:   [f_pOEMContext]       : A pointer to the OEM context; may be NULL
**              [f_pProximityContext] : The context of the operation
**              [f_ppbMessage]        : Returns the proximity response message
**              [f_pcbMessageSize]    : Returns the size (in bytes) of the message
**
** Returns:     DRM_SUCCESS - on success
**              or other return code from a function called within
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndr_Proximity_GenerateResponse(
    __in_opt                                         DRM_VOID                  *f_pOEMContext,
    __in                                       const DRM_ND_PROXIMITY_CONTEXT  *f_pProximityContext,
    __deref_out_bcount_full(*f_pcbMessageSize)       DRM_BYTE                 **f_ppbMessage,
    __out                                            DRM_DWORD                 *f_pcbMessageSize )
{
    DRM_RESULT   dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_Proximity_GenerateResponse );

    /*
    ** Build the Proximity Challenge Response message
    */
    dr = WmdrmNetPackProximityResponseMessage( f_pProximityContext->ProximityContentEncryptionKey,
                                               f_pProximityContext->Sequence,
                                               &f_pProximityContext->SessionId,
                                               &f_pProximityContext->Challenge,
                                               f_ppbMessage,
                                               f_pcbMessageSize );

    if ( dr != DRM_SUCCESS )
    {
        TRACE(( "Cannot build proximity response message. %lx\n", dr ));
        goto ErrorExit;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    Drm_Ndr_Proximity_ProcessChallenge
**
** Synopsis:    Process the proximity challenge message received from the transmitter.
**
** Arguments:   [f_pOEMContext]       : A pointer to the OEM context; may be NULL
**              [f_pProximityContext] : The context of the operation
**              [f_pbMessage]         : The proximity challenge message
**              [f_cbMessageSize]     : The size (in bytes) of the message
**
** Returns:     DRM_SUCCESS - on success
**              or other return code from a function called within
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndr_Proximity_ProcessChallenge(
    __in_opt DRM_VOID                     *f_pOEMContext,
    __in DRM_ND_PROXIMITY_CONTEXT         *f_pProximityContext,
    __in_bcount(f_cbMessageSize) DRM_BYTE *f_pbMessage,
    __in const DRM_DWORD                   f_cbMessageSize
)
{
    DRM_RESULT   dr        = DRM_SUCCESS;
    DRM_ID       idSession = {0};


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_Proximity_ProcessChallenge );

    /*
    ** Process the Proximity Challenge message
    */
    dr = WmdrmNetUnpackProximityChallengeMessage( f_pbMessage,
                                                  f_cbMessageSize,
                                                  &f_pProximityContext->Sequence,
                                                  &idSession,
                                                  &f_pProximityContext->Challenge );

    if ( dr != DRM_SUCCESS )
    {
        TRACE(( "Failed to process the proximity challenge message. %lx\n", dr ));
        goto ErrorExit;
    }

    /*
    ** Ensure the Session ID is correct
    */
    if ( MEMCMP( &idSession, &f_pProximityContext->SessionId, sizeof( idSession ) ) != 0)
    {
        TRACE(( "Proximity challenge message has unexpected session id.\n" ));
        dr = DRM_E_FAIL;
        goto ErrorExit;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    Drm_Ndr_Proximity_ProcessResult
**
** Synopsis:    Process the proximity result message received from the transmitter.
**
** Arguments:   [f_pOEMContext]       : A pointer to the OEM context; may be NULL
**              [f_pProximityContext] : The context of the operation
**              [f_pbMessage]         : The proximity result message
**              [f_cbMessageSize]     : The size (in bytes) of the message
**              [f_wResult]           : The proximity result
**
** Returns:     DRM_SUCCESS - on success
**              or other return code from a function called within
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndr_Proximity_ProcessResult(
    __in_opt DRM_VOID                     *f_pOEMContext,
    __in DRM_ND_PROXIMITY_CONTEXT         *f_pProximityContext,
    __in_bcount(f_cbMessageSize) DRM_BYTE *f_pbMessage,
    __in const DRM_DWORD                   f_cbMessageSize,
    __out      DRM_WORD                   *f_pwResult
)
{
    DRM_RESULT   dr        = DRM_SUCCESS;
    DRM_ID       idSession = {0};


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDR, PERF_FUNC_Drm_Ndr_Proximity_ProcessResult );

    /*
    ** Process the Proximity Result message
    */
    dr = WmdrmNetUnpackProximityResultMessage( f_pbMessage,
                                               f_cbMessageSize,
                                               &idSession,
                                               f_pwResult );

    if ( dr != DRM_SUCCESS )
    {
        TRACE(( "Failed to process the proximity result message. %lx\n", dr ));
        goto ErrorExit;
    }

    /*
    ** Ensure the Session ID is correct
    */
    if ( MEMCMP( &idSession, &f_pProximityContext->SessionId, sizeof( idSession ) ) != 0)
    {
        TRACE(( "Proximity result message has unexpected session id.\n" ));
        dr = DRM_E_FAIL;
        goto ErrorExit;
    }

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/*****************************************************************************
** Function:    Drm_Ndr_GetMessageType
**
** Synopsis:    Obtains the message type of the message received from the transmitter.
**
** Arguments:   [f_pbMessage]     : The message received from the transmitter
**              [f_cbMessageSize] : The size (in bytes) of the message
**
** Returns:     The message type
**
** Notes:       None.
**
******************************************************************************/
DRM_API DRM_BYTE DRM_CALL Drm_Ndr_GetMessageType(
    __in_bcount(f_cbMessageSize) const DRM_BYTE  * const f_pbMessage,
    __in                         const DRM_DWORD         f_cbMessageSize
)
{
    return WmdrmNetGetMessageType( f_pbMessage, f_cbMessageSize );
}

EXIT_PK_NAMESPACE_CODE;

