/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>

#include <string>

#include <Utilities.h>

#include <PRNDMemHolder.h>
#include <PRNDReceiverSession.h>


////////////////////////////////////////////////////////////////////////////////
//
// PRNDSessionInfo implementation
//
////////////////////////////////////////////////////////////////////////////////

DRM_INT64 PRNDSessionInfo::GetSecondsUntilExpiration( __in DRMFILETIME currentTime ) const
{
    static const DRM_UINT64 PRND_MAX_REGISTRATION_AGE_IN_TICKS = DRM_UI64Mul( DRM_UI64Mul( DRM_UI64( 48 ), DRM_UI64( 3600 ) ), DRM_UI64( C_TICS_PER_SECOND ) );

    if( IsEmpty() )
    {
        // Invalid sessions are not fresh
        return DRM_I64(-1);
    }
    else
    {
        // Compute the duration left for the session in ticks

        DRM_UINT64 ullExpirationTime;
        DRM_UINT64 ullCurrentTime;

        FILETIME_TO_UI64( currentTime, ullCurrentTime );
        FILETIME_TO_UI64( m_lastRegistrationTime, ullExpirationTime );

        ullExpirationTime = DRM_UI64Add( ullExpirationTime, PRND_MAX_REGISTRATION_AGE_IN_TICKS );

        // get difference rounded to seconds.
        DRM_INT64 diff = DRM_UI2I64( DRM_UI64Div( DRM_UI64Sub( ullExpirationTime, ullCurrentTime ), DRM_UI64( C_TICS_PER_SECOND ) ) );

        return diff;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
// PRNDReceiverSession implementation
//
////////////////////////////////////////////////////////////////////////////////


PRNDReceiverSession::PRNDReceiverSession()
    : m_fDrmIsInitialized( false )
    , m_fReceiverSessionIsInitialized( false )
    , m_pCurrentLicenseResponseCatcher( nullptr )
{
    memset( &m_oAppContext, 0, sizeof( m_oAppContext ) );
    memset( &m_opaqueBuffer, 0, sizeof( m_opaqueBuffer ) );
    memset( &m_oReceiverContext, 0, sizeof( m_oReceiverContext ) );
    memset( &m_drmSessionID, 0, sizeof( m_drmSessionID ) );
}

HRESULT PRNDReceiverSession::Open(
    __in const DRM_ID  &sessionID,
    __in       LPCWSTR  pszDataStoreFilePath,
    __in       LPCWSTR  pszRevocationPackagePath
    )
{
    HRESULT dr = S_OK;

    m_drmSessionID = sessionID;

    ChkDR( _InitPrndSession( pszDataStoreFilePath, pszRevocationPackagePath ) );

ErrorExit:

    if( FAILED( dr ) )
    {
        Close();
    }

    return dr;
}

void PRNDReceiverSession::Close()
{
    if( m_fReceiverSessionIsInitialized )
    {
        Drm_Prnd_Receiver_EndSession( &m_oAppContext );
    }

    if( m_fDrmIsInitialized )
    {
        Drm_Uninitialize( &m_oAppContext );
    }

    m_fReceiverSessionIsInitialized = false;
    m_fDrmIsInitialized = false;
}

HRESULT PRNDReceiverSession::GenerateRegistrationRequest( __inout CPRNDMemHolder &message )
{
    HRESULT    dr    = S_OK;
    DRM_BYTE  *pbMsg = nullptr;
    DRM_DWORD  cbMsg = 0;

    bool fExistingSession = !IsEmpty( m_drmSessionID );

    dr = Drm_Prnd_Receiver_RegistrationRequest_Generate(
                &m_oAppContext,
                fExistingSession ? &m_drmSessionID : nullptr,
                nullptr,
                nullptr,
                0,
                DRM_PRND_FLAG_NO_FLAGS,
                &pbMsg,
                &cbMsg );

    message.Attach( pbMsg, cbMsg );

    ChkDR( dr );

ErrorExit:

    return dr;
}

HRESULT PRNDReceiverSession::GenerateProximityStart( __inout CPRNDMemHolder &message )
{
    HRESULT    dr    = S_OK;
    DRM_BYTE  *pbMsg = nullptr;
    DRM_DWORD  cbMsg = 0;

    dr = Drm_Prnd_Receiver_ProximityDetectionStart_Generate(
                &m_oAppContext,
                DRM_PRND_FLAG_NO_FLAGS,
                &pbMsg,
                &cbMsg );

    message.Attach( pbMsg, cbMsg );

    ChkDR( dr );

ErrorExit:

    return dr;
}

HRESULT PRNDReceiverSession::GenerateFetchLicense(
    __in const DRM_GUID       &requestedAction,
    __in const DRM_KID        &contentID,
    __inout    CPRNDMemHolder &message )
{
    HRESULT    dr    = S_OK;
    DRM_BYTE  *pbMsg = nullptr;
    DRM_DWORD  cbMsg = 0;

    dr = Drm_Prnd_Receiver_LicenseRequest_Generate(
            &m_oAppContext,
            &requestedAction,
            NULL,
            DRM_PRND_CONTENT_IDENTIFIER_TYPE_KID,
            contentID.rgb,
            sizeof( contentID.rgb ),
            nullptr,
            nullptr,
            0,
            DRM_PRND_FLAG_NO_FLAGS,
            &pbMsg,
            &cbMsg );

    message.Attach( pbMsg, cbMsg );

    ChkDR( dr );

ErrorExit:

    return dr;
}

HRESULT PRNDReceiverSession::ProcessRegistrationResponse(
    __in_bcount( cb ) const void  *pvMsg,
    __in                    ULONG  cbMsg )
{
    HRESULT                           dr         = S_OK;
    DRM_DWORD                         dwFlagsOut = 0;
    HRESULT                           drError    = S_OK;
    DRM_BYTE                         *pbWeakTransmitterProximityDetectionChannel = nullptr;
    DRM_DWORD                         cbTransmitterProximityDetectionChannel     = 0;
    DRM_PRND_MESSAGE_TYPE             eType;
    DRM_PRND_PROXIMITY_DETECTION_TYPE eProximityType;
    
    ChkDR( Drm_Prnd_GetMessageType(
                reinterpret_cast<const DRM_BYTE*>( pvMsg ),
                cbMsg,
                &eType ) );

    switch( eType )
    {
    case DRM_PRND_MESSAGE_TYPE_REGISTRATION_RESPONSE:

        ChkDR( Drm_Prnd_Receiver_RegistrationResponse_Process(
                    &m_oAppContext,
                    reinterpret_cast<const DRM_BYTE*>( pvMsg ),
                    cbMsg,
                    nullptr,
                    nullptr,
                    &m_drmSessionID,
                    &eProximityType,
                    &pbWeakTransmitterProximityDetectionChannel,
                    &cbTransmitterProximityDetectionChannel,
                    &dwFlagsOut ) );
        break;

    case DRM_PRND_MESSAGE_TYPE_REGISTRATION_ERROR:

        ChkDR( Drm_Prnd_Receiver_RegistrationError_Process(
                    &m_oAppContext,
                    reinterpret_cast<const DRM_BYTE*>( pvMsg ),
                    cbMsg,
                    &drError,
                    &dwFlagsOut ) );

        DRMASSERT( FAILED( drError ) );

        ChkDR( drError );

        break;

    default:

        ChkDR( DRM_E_PRND_MESSAGE_INVALID );

        break;
    }

ErrorExit:

    return dr;
}

HRESULT PRNDReceiverSession::ProcessProximityChallenge(
    __in_bcount( cb ) const void           *pvMsg,
    __in                    ULONG           cbMsg,
    __inout                 CPRNDMemHolder &response
    )
{
    HRESULT    dr         = S_OK;
    DRM_DWORD  dwFlagsOut = 0;
    DRM_BYTE  *pbResponse = nullptr;
    DRM_DWORD  cbResponse = 0;

    dr = Drm_Prnd_Receiver_ProximityDetectionChallenge_Process(
                &m_oAppContext,
                reinterpret_cast<const DRM_BYTE*>( pvMsg ),
                cbMsg,
                DRM_PRND_FLAG_NO_FLAGS,
                &pbResponse,
                &cbResponse,
                &dwFlagsOut );

    response.Attach( pbResponse, cbResponse );

    ChkDR( dr );

ErrorExit:

    return dr;
}

HRESULT PRNDReceiverSession::ProcessProximityResult(
    __in_bcount( cb ) const void  *pvMsg,
    __in                    ULONG  cbMsg
    )
{
    HRESULT dr         = S_OK;
    HRESULT drResult   = S_OK;
    DWORD   dwOutFlags = 0;

    ChkDR( Drm_Prnd_Receiver_ProximityDetectionResult_Process(
                &m_oAppContext,
                reinterpret_cast<const DRM_BYTE*>( pvMsg ),
                cbMsg,
                &drResult,
                &dwOutFlags ) );

    ChkDR( drResult );

ErrorExit:

    return dr;
}

HRESULT PRNDReceiverSession::ProcessLicenseResponse(
    __in_bcount( cb ) const void           *pvMsg,
    __in                    ULONG           cbMsg,
    __out                   DRM_ID         &customDataTypeID,
    __inout                 CPRNDMemHolder &customData
    )
{
    HRESULT                  dr         = S_OK;
    DRM_DWORD                dwFlagsOut = 0;
    HRESULT                  drError    = S_OK;
    DRM_PRND_MESSAGE_TYPE    eType;
    LICENSE_RESPONSE_CATCHER catcher;

    ChkDR( Drm_Prnd_GetMessageType( reinterpret_cast<const DRM_BYTE*>( pvMsg ), cbMsg, &eType ) );

    switch( eType )
    {
    case DRM_PRND_MESSAGE_TYPE_LICENSE_TRANSMIT:

        m_pCurrentLicenseResponseCatcher = &catcher;

        dr = Drm_Prnd_Receiver_LicenseTransmit_Process( &m_oAppContext,
                      reinterpret_cast<const DRM_BYTE*>( pvMsg ),
                                                         cbMsg,
                                                         _LicenseResponseCallback,
                                                         this,
                                                        &dwFlagsOut );

        m_pCurrentLicenseResponseCatcher = nullptr;

        ChkDR( dr );

        customDataTypeID = catcher.customDataTypeID;
        customData.TakeFrom( catcher.customData );

        break;

    case DRM_PRND_MESSAGE_TYPE_LICENSE_ERROR:

        ChkDR( Drm_Prnd_Receiver_LicenseError_Process( &m_oAppContext,
                     reinterpret_cast<const DRM_BYTE*>( pvMsg ),
                                                        cbMsg,
                                                       &drError,
                                                       &dwFlagsOut ) );

        DRMASSERT( FAILED( drError ) );

        ChkDR( drError );

        break;

    default:

        ChkDR( DRM_E_PRND_MESSAGE_INVALID );

        break;
    }

ErrorExit:

    return dr;
}

/*static*/ bool PRNDReceiverSession::IsEmpty( const DRM_ID &sessionID )
{
    return( memcmp( sessionID.rgb, EMPTY_DRM_ID.rgb, sizeof( sessionID.rgb ) ) == 0 );
}

HRESULT PRNDReceiverSession::_InitPrndSession(
    __in LPCWSTR pszDataStoreFilePath,
    __in LPCWSTR pszRevocationPackagePath
    )
{
    HRESULT          dr                = S_OK;
    DRM_CONST_STRING dstrDataStoreFile = {0};
    std::string      strRevocationInfo;

    AssertChkArg( pszDataStoreFilePath != nullptr );
    AssertChkArg( pszRevocationPackagePath != nullptr );

    //
    // Initialize DRM
    //

    ChkDR( m_revocationBuffer.Alloc( REVOCATION_BUFFER_SIZE ) );
    memset( m_revocationBuffer.Data(), 0, m_revocationBuffer.Size() );

    ChkBOOL( !m_fDrmIsInitialized && !m_fReceiverSessionIsInitialized, E_NOT_VALID_STATE );

    dstrDataStoreFile.pwszString = WStr_As_DRMWCHAR( pszDataStoreFilePath );
    dstrDataStoreFile.cchString = ( DRM_DWORD ) wcslen( pszDataStoreFilePath );

    ChkDR( Drm_Initialize( &m_oAppContext, 
                            NULL, 
                            m_opaqueBuffer,
                            sizeof( m_opaqueBuffer ),
                           &dstrDataStoreFile ) );

    m_fDrmIsInitialized = true;

    ChkDR( Drm_Revocation_SetBuffer( &m_oAppContext,
                                      m_revocationBuffer.Data(), 
                                      m_revocationBuffer.Size() ) );

    ChkDR( FileReadAllText( pszRevocationPackagePath, strRevocationInfo ) );
    ChkDR( Drm_Revocation_StorePackage( &m_oAppContext, 
                                         strRevocationInfo.c_str(),
                           ( DRM_DWORD ) strRevocationInfo.size() ) );

    ChkDR( Drm_Prnd_Receiver_BeginSession( &m_oAppContext, &m_oReceiverContext ) );

    m_fReceiverSessionIsInitialized = true;

ErrorExit:

    return dr;
}

/*static*/ HRESULT DRM_CALL PRNDReceiverSession::_LicenseResponseCallback(
    __inout_ecount_opt( 1 )               DRM_VOID                  *pvDataCallbackContext,
    __in_opt                              DRM_PRND_CERTIFICATE_DATA *pCertificateData,
    __in_opt                        const DRM_ID                    *pCustomDataTypeID,
    __in_bcount_opt( cbCustomData ) const DRM_BYTE                  *pbCustomData,
    __in                                  DRM_DWORD                  cbCustomData
    )
{
    HRESULT              dr    = S_OK;
    PRNDReceiverSession *pThis = reinterpret_cast<PRNDReceiverSession*>( pvDataCallbackContext );

    AssertChkArg( pThis != nullptr );
    AssertChkArg( pCustomDataTypeID != nullptr );

    ChkBOOL( pThis->m_pCurrentLicenseResponseCatcher != nullptr, E_NOT_VALID_STATE );

    pThis->m_pCurrentLicenseResponseCatcher->customDataTypeID = *pCustomDataTypeID;

    ChkDR( pThis->m_pCurrentLicenseResponseCatcher->customData.CopyFrom( pbCustomData, cbCustomData ) );

ErrorExit:

    return dr;
}
