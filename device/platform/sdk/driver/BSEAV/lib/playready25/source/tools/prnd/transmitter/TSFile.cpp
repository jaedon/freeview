/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdafx.h>
#include <TSFile.h>

using namespace std;

CTSFile::CTSFile( const wstring wstrFile )
{
    m_wstrFile      = wstrFile;
    m_llPacketCount = 0;
    m_hTSFile       = INVALID_HANDLE_VALUE;
}

CTSFile::~CTSFile()
{
    CLOSE_HANDLE( m_hTSFile );
}

HRESULT CTSFile::FileOpened()
{
    HRESULT      dr = S_OK;
    CDRMAutoLock lock( &m_lock );

    if( m_hTSFile != INVALID_HANDLE_VALUE )
    {
        goto ErrorExit;
    }

    m_hTSFile = CreateFileW(
                        m_wstrFile.c_str(),
                        GENERIC_READ,
                        FILE_SHARE_READ,        
                        NULL,                    // default security attributes
                        OPEN_EXISTING,
                        0,                       // default attributes
                        NULL );                   // no template file
    if( m_hTSFile == INVALID_HANDLE_VALUE )
    {
        ChkDR( E_FAIL );
    }

ErrorExit:
    return dr;
}

HRESULT CTSFile::ReadPacketCount()
{
    HRESULT       dr = S_OK;
    CDRMAutoLock  lock( &m_lock );
    LARGE_INTEGER liFileSize = {0};

    if( m_llPacketCount == 0 )
    {
        ChkDR( FileOpened() );
        if( !GetFileSizeEx( m_hTSFile, &liFileSize ) )
        {
            dr = HRESULT_FROM_WIN32( GetLastError() );
            goto ErrorExit;
        }
        
        m_llPacketCount    =  liFileSize.QuadPart / TS_PACKET_SIZE ;
        if( liFileSize.QuadPart % TS_PACKET_SIZE > 0 )
        {
            m_llPacketCount++;
        }
    }

ErrorExit:
    return dr;
}

HRESULT CTSFile::GetPacketCount( LONGLONG  *pllPacketCount )
{
    HRESULT       dr = S_OK;
    CDRMAutoLock  lock( &m_lock );

    ChkArg( pllPacketCount != nullptr );
    if( m_llPacketCount == 0 )
    {
        ChkDR( ReadPacketCount() );
    }

    *pllPacketCount = m_llPacketCount;

ErrorExit:
    return dr;
}

HRESULT CTSFile::GetFileSize( LONGLONG  *pllFileSize )
{
    HRESULT         dr          = S_OK;
    LARGE_INTEGER   liFileSize  = {0};
    CDRMAutoLock    lock( &m_lock );

    ChkArg( pllFileSize != nullptr );
    ChkDR( FileOpened() );
    if( !GetFileSizeEx( m_hTSFile, &liFileSize ) )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        goto ErrorExit;
    }

    *pllFileSize = liFileSize.QuadPart;

ErrorExit:
    return dr;
}

HRESULT CTSFile::GetPacket( LONGLONG  llPacketNumber, vector<BYTE> &vbPacket  )
{
    HRESULT       dr          = S_OK;
    DWORD         dwBytesRead = 0;
    LARGE_INTEGER liFilePos   = {0};
    CDRMAutoLock  lock( &m_lock );

    ChkDR( FileOpened() );
    ChkDR( ReadPacketCount() );
    ChkArg( llPacketNumber < m_llPacketCount );  

    vbPacket.resize( TS_PACKET_SIZE, 0 );
    liFilePos.QuadPart = llPacketNumber * TS_PACKET_SIZE;
    if( !SetFilePointerEx( m_hTSFile, liFilePos, NULL, FILE_BEGIN ) )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        goto ErrorExit;
    }

    /*
    ** The construction &vbPacket[0] to access the data in a vector works with the MS version of STL but
    ** is not guaranteed to work. You should check if this works on your implementation.
    */
#if !defined( _MSC_VER )
#error Double-Check the way your STL accesses Vector Data
#endif
    if( !ReadFile( m_hTSFile, ( char* )&vbPacket[0], TS_PACKET_SIZE, &dwBytesRead, NULL ) )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        goto ErrorExit;
    }
    vbPacket.resize( dwBytesRead, 0 );
    
ErrorExit:
    return dr;
}


//////////////////////////////////////////////////////////////////////////////////////////////

CTSFileWriter::CTSFileWriter( const wstring wstrFile )
{
    m_wstrFile      = wstrFile;
    m_hTSFile       = INVALID_HANDLE_VALUE;
}

CTSFileWriter::~CTSFileWriter()
{
    CLOSE_HANDLE( m_hTSFile );
}

HRESULT CTSFileWriter::FileOpened()
{
    HRESULT      dr = S_OK;
    CDRMAutoLock lock( &m_lock );

    if( m_hTSFile != INVALID_HANDLE_VALUE )
    {
        goto ErrorExit;
    }

    m_hTSFile = CreateFileW(
                        m_wstrFile.c_str(),
                        GENERIC_WRITE,
                        0,        
                        NULL,                    // default security attributes
                        CREATE_ALWAYS,
                        0,                       // default attributes
                        NULL );                   // no template file
    if( m_hTSFile == INVALID_HANDLE_VALUE )
    {
        ChkDR( E_FAIL );
    }

ErrorExit:
    return dr;
}

HRESULT CTSFileWriter::GetFileSize( LONGLONG  *pllFileSize )
{
    HRESULT         dr          = S_OK;
    LARGE_INTEGER   liFileSize  = {0};
    CDRMAutoLock    lock( &m_lock );

    ChkArg( pllFileSize != nullptr );
    ChkDR( FileOpened() );
    if( !GetFileSizeEx( m_hTSFile, &liFileSize ) )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        goto ErrorExit;
    }

    *pllFileSize = liFileSize.QuadPart;

ErrorExit:
    return dr;
}

HRESULT CTSFileWriter::WritePacket( vector<BYTE> &vbPacket,  bool bFlush )
{
    HRESULT dr              = S_OK;
    DWORD   dwBytesWritten  = 0;
    
    CDRMAutoLock  lock( &m_lock );

    if( vbPacket.empty() )
    {
        goto ErrorExit;
    }
    
    ChkDR( FileOpened() );

    /*
    ** The construction &vbPacket[0] to access the data in a vector works with the MS version of STL but
    ** is not guaranteed to work. You should check if this works on your implementation.
    */
#if !defined( _MSC_VER )
#error Double-Check the way your STL accesses Vector Data
#endif
    if( !WriteFile( m_hTSFile, ( char* )&vbPacket[0], ( DWORD ) vbPacket.size(), &dwBytesWritten, NULL ) )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        goto ErrorExit;
    }

    if( vbPacket.size() != dwBytesWritten )
    {
        dr = E_UNEXPECTED;
        goto ErrorExit;
    }

    if( bFlush )
    {
        if( !FlushFileBuffers( m_hTSFile ) )
        {
            dr = HRESULT_FROM_WIN32( GetLastError() );
            goto ErrorExit;
        }
    }
    
ErrorExit:
    return dr;
}
