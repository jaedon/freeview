/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

class CCriticalSection
{
public:
    __forceinline
    CCriticalSection( VOID )
    {
        InitializeCriticalSection( &m_cs );
    }

    __forceinline
    ~CCriticalSection( VOID )
    {
        DeleteCriticalSection( &m_cs );
    }

    __forceinline 
    VOID
    Lock( VOID )
    {
        EnterCriticalSection( &m_cs );
    }

    __forceinline 
    VOID Unlock( VOID )
    {
        LeaveCriticalSection( &m_cs );
    }

private:
    CRITICAL_SECTION m_cs;
};


//
// Stack-based automatic lock
//
template <class t_type> class CAutoLock
{
public:
    __forceinline 
    CAutoLock( t_type *pLock )
    :   m_pLock( pLock )
    {
        m_pLock->Lock();
    }

    __forceinline
    ~CAutoLock( void )
    {
        m_pLock->Unlock();
    }

private:
    t_type *    m_pLock;
};

typedef   CCriticalSection              CDRMLock;
typedef   CAutoLock<CCriticalSection>   CDRMAutoLock;
