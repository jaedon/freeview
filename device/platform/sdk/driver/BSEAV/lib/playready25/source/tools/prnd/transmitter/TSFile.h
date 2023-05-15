/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#pragma once
#include <Locks.h>

class CTSFile
{

public:
    CTSFile::CTSFile( const std::wstring wstrFile );
    virtual CTSFile::~CTSFile();

    HRESULT CTSFile::GetPacketCount( LONGLONG  *pllPacketCount );
    HRESULT CTSFile::GetPacket( LONGLONG  llPacketNumber, std::vector<BYTE> &vbPacket  );
    HRESULT CTSFile::GetFileSize( LONGLONG  *pllFileSize );

private:
    HRESULT CTSFile::FileOpened();
    HRESULT CTSFile::ReadPacketCount();

    LONGLONG        m_llPacketCount;
    std::wstring    m_wstrFile;
    HANDLE          m_hTSFile;

    CDRMLock        m_lock;
};


/////////////////////////////////////////////////////////////////////////////////////
class CTSFileWriter
{

public:
    CTSFileWriter::CTSFileWriter( const std::wstring wstrFile );
    virtual CTSFileWriter::~CTSFileWriter();

    HRESULT CTSFileWriter::WritePacket( std::vector<BYTE> &vbPacket, bool bFlush  );
    HRESULT CTSFileWriter::GetFileSize( LONGLONG  *pllFileSize );

private:
    HRESULT CTSFileWriter::FileOpened();

    std::wstring    m_wstrFile;
    HANDLE          m_hTSFile;

    CDRMLock        m_lock;
};
