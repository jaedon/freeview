/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

/// <summary>
/// Contains information about a PRND Session
/// like ID and last registration time
/// </summary>
class PRNDSessionInfo
{
public:

    PRNDSessionInfo()
    {
        Clear();
    }

    void Clear()
    {
        m_drmSessionID = EMPTY_DRM_ID;

        m_lastRegistrationTime.dwHighDateTime = 0;
        m_lastRegistrationTime.dwLowDateTime = 0;
    }

    bool IsEmpty() const
    {
        return memcmp( &m_drmSessionID, &EMPTY_DRM_ID, sizeof( EMPTY_DRM_ID ) ) == 0;
    }

    const DRM_ID& SessionID() const
    {
        return m_drmSessionID;
    }

    DRMFILETIME LastRegistrationTime() const
    {
        return m_lastRegistrationTime;
    }

    void SetSessionID( __in const DRM_ID &drmSessionID, __in DRMFILETIME currentTime )
    {
        m_drmSessionID = drmSessionID;
        m_lastRegistrationTime = currentTime;
    }

    DRM_INT64 GetSecondsUntilExpiration( __in DRMFILETIME currentTime ) const;

private:

    DRM_ID      m_drmSessionID;
    DRMFILETIME m_lastRegistrationTime;
};

/// <summary>
/// Executes PRND actions bound to a session context
/// </summary>
class PRNDReceiverSession
{
public:

    PRNDReceiverSession();

    ~PRNDReceiverSession()
    {
        Close();
    }

    HRESULT Open(
                __in const DRM_ID  &sessionID,
                __in       LPCWSTR  pszDataStoreFilePath,
                __in       LPCWSTR  pszRevocationPackagePath
                );

    void Close();

    const DRM_ID& SessionID() const
    {
        return m_drmSessionID;
    }

    HRESULT GenerateRegistrationRequest(
        __inout CPRNDMemHolder &message
        );

    HRESULT GenerateProximityStart(
        __inout CPRNDMemHolder &message
        );

    HRESULT GenerateFetchLicense(
        __in const DRM_GUID       &requestedAction,
        __in const DRM_KID        &contentID,
        __inout    CPRNDMemHolder &message
        );

    HRESULT ProcessRegistrationResponse(
        __in_bcount( cb ) const void *pvMsg,
        __in                    ULONG cbMsg
        );

    HRESULT ProcessProximityChallenge(
        __in_bcount( cb ) const void           *pvMsg,
        __in                    ULONG           cbMsg,
        __inout                 CPRNDMemHolder &response
        );

    HRESULT ProcessProximityResult(
        __in_bcount( cb ) const void *pvMsg,
        __in                    ULONG cbMsg );

    HRESULT ProcessLicenseResponse(
        __in_bcount( cb ) const void           *pvMsg,
        __in                    ULONG           cbMsg,
        __out                   DRM_ID         &customDataTypeID,
        __inout                 CPRNDMemHolder &customData
        );

    static bool IsEmpty( const DRM_ID &sessionID );

private:

    struct LICENSE_RESPONSE_CATCHER
    {
        DRM_ID         customDataTypeID;
        CPRNDMemHolder customData;
    };

    bool                      m_fDrmIsInitialized;
    bool                      m_fReceiverSessionIsInitialized;
    DRM_APP_CONTEXT           m_oAppContext;
    DRM_BYTE                  m_opaqueBuffer[MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE];
    DRM_PRND_RECEIVER_CONTEXT m_oReceiverContext;
    DRM_ID                    m_drmSessionID;
    CPRNDMemHolder            m_revocationBuffer;
    LICENSE_RESPONSE_CATCHER *m_pCurrentLicenseResponseCatcher;

    HRESULT _InitPrndSession(
                __in LPCWSTR pszDataStoreFilePath,
                __in LPCWSTR pszRevocationPackagePath
                );

    static HRESULT DRM_CALL _LicenseResponseCallback(
                __inout_ecount_opt( 1 )               DRM_VOID                   *pvDataCallbackContext,
                __in_opt                              DRM_PRND_CERTIFICATE_DATA  *pCertificateData,
                __in_opt                        const DRM_ID                     *pCustomDataTypeID,
                __in_bcount_opt( cbCustomData ) const DRM_BYTE                   *pbCustomData,
                __in                                  DRM_DWORD                   cbCustomData
                );
};