/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>

#include <strsafe.h>
#include <string>

#include <Utilities.h>

//////////////////////////////////////////////////77////////////////////////////
//
// String utilities implementation
//
////////////////////////////////////////////////////////////////////////////////

LPCWSTR SkipSpaces( __in_opt LPCWSTR pszIn )
{
    if( pszIn != nullptr )
    {
        while( iswspace( *pszIn ) )
        {
            ++pszIn;
        }
    }

    return pszIn;
}

LPCWSTR SkipNonSpaces(
    __in_opt    LPCWSTR       pszIn,
    __inout_opt std::wstring *pstrOut )
{
    LPCWSTR pszMark = nullptr;

    if( pszIn != nullptr )
    {
        pszMark = pszIn;

        while( *pszIn != L'\0' && !iswspace( *pszIn ) )
        {
            ++pszIn;
        }
    }

    if( pstrOut != nullptr )
    {
        if( pszIn != nullptr )
        {
            pstrOut->assign( pszMark, pszIn );
        }
        else
        {
            pstrOut->clear();
        }
    }

    return pszIn;
}

LPCWSTR SkipUnsignedDecimalInt(
    __in_opt           LPCWSTR pszIn,
    __out_opt unsigned int    *pValue
    )
{
    unsigned int acc = 0;

    if( pszIn != nullptr )
    {
        bool fFoundDigits = false;

        while( iswdigit( *pszIn ) )
        {
            acc *= 10;
            acc += ( *pszIn - L'0' );
            ++pszIn;
            fFoundDigits = true;
        }

        if( !fFoundDigits )
        {
            pszIn = nullptr;
        }
    }

    if( pValue != nullptr )
    {
        *pValue = acc;
    }

    return pszIn;
}


LPCWSTR SkipCommandToken(
    __in_opt LPCWSTR pszIn,
    __in_opt LPCWSTR pszCommand
    )
{
    if( pszCommand == nullptr )
    {
        pszIn = nullptr;
    }

    if( pszIn != nullptr )
    {
        while( *pszCommand != L'\0' && *pszIn == *pszCommand )
        {
            ++pszIn;
            ++pszCommand;
        }

        if( *pszCommand != L'\0' )
        {
            return nullptr; // input string is shorter than token
        }

        if( *pszIn != L'\0' && !iswspace( *pszIn ) )
        {
            return nullptr; // input token is longer
        }
    }

    return pszIn;
}

LPCWSTR SkipPrefix(
    __in_opt LPCWSTR pszIn,
    __in_opt LPCWSTR pszPrefix
    )
{
    if( pszPrefix == nullptr )
    {
        pszIn = nullptr;
    }

    if( pszIn != nullptr )
    {
        while( *pszPrefix != L'\0' && *pszIn == *pszPrefix )
        {
            ++pszIn;
            ++pszPrefix;
        }

        if( *pszPrefix != L'\0' )
        {
            // No match, the input string is shorter than the prefix
            pszIn = nullptr;
        }
    }

    return pszIn;
}

HRESULT FileReadAllText(
    __in    LPCWSTR      pszFilePath,
    __inout std::string &strText
    )
{
    HRESULT dr            = S_OK;
    FILE*   pFile         = nullptr;
    errno_t errn          = 0;
    CHAR    szLineBuffer[1024];
    ULONG   cbGranularity = 0;

    strText.clear();

    //
    // Open the file for reading
    //

    errn = _wfopen_s( &pFile, pszFilePath, L"r" );

    ChkBOOL( errn != ENOENT, HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND ) );
    ChkBOOL( errn != EACCES, HRESULT_FROM_WIN32( ERROR_ACCESS_DENIED ) );
    ChkBOOL( errn == 0, E_FAIL );
    ChkBOOL( pFile != nullptr, E_UNEXPECTED );

    //
    // Get the file size to use as estimation for the string's growth
    // ( to reduce reallocation )
    //

    ChkBOOL( fseek( pFile, 0L, SEEK_END ) == 0, E_UNEXPECTED );

    cbGranularity = static_cast<ULONG>( ftell( pFile ) ) / 4;

    ChkBOOL( fseek( pFile, 0L, SEEK_SET ) == 0, E_UNEXPECTED );

    while( fgets( szLineBuffer, ARRAYSIZE( szLineBuffer ), pFile ) != nullptr )
    {
        if( strText.capacity() - strText.size() < cbGranularity )
        {
            strText.reserve( strText.size() + cbGranularity );
        }

        strText += szLineBuffer;
    }

    //
    // Ensure the loop reached the end of the file
    //

    errn = ferror( pFile );

    ChkBOOL( errn != EIO, HRESULT_FROM_WIN32( ERROR_IO_DEVICE ) );
    ChkBOOL( errn == 0, E_FAIL );
    ChkBOOL( feof( pFile ) != 0, E_UNEXPECTED );

ErrorExit:

    if( pFile != nullptr )
    {
        fclose( pFile );
    }

    return dr;
}

//////////////////////////////////////////////////77////////////////////////////
//
// DRMIDString implementation
//
////////////////////////////////////////////////////////////////////////////////

DRMIDString::DRMIDString( __in const DRM_ID &id )
{
    ( void )StringCchPrintfW(
                m_buffer,
                ARRAYSIZE( m_buffer ),
                L"%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x",
                id.rgb[0],
                id.rgb[1],
                id.rgb[2],
                id.rgb[3],
                id.rgb[4],
                id.rgb[5],
                id.rgb[6],
                id.rgb[7],
                id.rgb[8],
                id.rgb[9],
                id.rgb[10],
                id.rgb[11],
                id.rgb[12],
                id.rgb[13],
                id.rgb[14],
                id.rgb[15],
                id.rgb[16] );
}
