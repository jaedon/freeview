/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once
#include <PRNDSession.h>
#include <drmptrlist.h>

#define PRND_SESSION_COUNT_MAX 1000

typedef struct tagPRND_SESSION_ENTRY
{
    CPRNDSession    *pSession;
    DRM_ID           sessionID;
} PRND_SESSION_ENTRY;

class CPRNDSessionManager
{

public:
    static HRESULT CPRNDSessionManager::GetInstance( CPRNDSessionManager **ppSessionManager );
    static DWORD WINAPI CPRNDSessionManager::ProcessPRNDSessionMessagesCallback( __in LPVOID lpParam );
    static DWORD WINAPI CPRNDSessionManager::ProcessMediaStreamingRequestsCallback( __in LPVOID lpParam );

    CPRNDSessionManager();
    virtual ~CPRNDSessionManager();

    HRESULT CPRNDSessionManager::GetPRNDSession(
                                CPRNDSession  **ppSession  );

    HRESULT CPRNDSessionManager::GetPRNDSessionFromSessionID(
                                __in const DRM_ID         &sessionID,
                                __out      CPRNDSession  **ppSession  );

    HRESULT CPRNDSessionManager::AdoptSession( __in CPRNDSession *pSession );

private:
           PTR_LIST             m_ptrlistSessions;
    static CPRNDSessionManager *s_pSessionManager;
    static CDRMLock             s_lock;
};
