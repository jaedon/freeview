/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

#include <stdio.h>
#include <Logger.h>

class CFileLogger
{

public:
    static HRESULT Create( const char *pszLogFile );
    static void CFileLogger::Close();

    void CFileLogger::LogMessage( const char *szFormat, va_list vaList );
    void CFileLogger::LogRowsOfRawBytes( BYTE *pbData, DWORD cbDataSize, bool bDisplayChars );
    void CFileLogger::CloseInternal();
    
    static CFileLogger *s_fileLogger ;

private:
    CFileLogger();
    virtual ~CFileLogger();
    HRESULT CFileLogger::Initialize( const char *pszLogFile );
    void CFileLogger::DisplayOneLineOfRawBytes( BYTE *pbBuffer, DWORD dwByteCountToDisplay, bool bDisplayChars );
    
    FILE           *m_fpLogFile;
    static CDRMLock s_lock;
    
};
