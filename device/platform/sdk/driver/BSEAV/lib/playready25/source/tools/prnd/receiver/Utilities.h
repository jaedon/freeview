/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

/// <summary>
/// Safely casts a DRM_WCHAR string to a WCHAR string
/// </summary>
inline const DRM_WCHAR* WStr_As_DRMWCHAR( __in LPCWSTR pszIn )
{
    C_ASSERT( sizeof( *pszIn ) == sizeof( DRM_WCHAR ) );
    return( reinterpret_cast<const DRM_WCHAR*>( pszIn ) );
}

/// <summary>
/// Simplistic conversion of a WCHAR string to a CHAR string
/// </summary>
inline std::string wstring_to_string( __in LPCWSTR pszStr )
{
    if( pszStr != nullptr )
    {
        return std::string( pszStr, pszStr + wcslen( pszStr ) );
    }
    else
    {
        return std::string();
    }
}

/// <summary>
/// Initializes COM for the application with RAII semantics
/// </summary>
class ComContext
{
public:

    ComContext()
    {
        CoInitializeEx( NULL, COINIT_MULTITHREADED );
    }

    ~ComContext()
    {
        CoUninitialize();
    }
};

/// <summary>
/// Skips spaces
/// </summary>
LPCWSTR SkipSpaces( __in_opt LPCWSTR pszIn );

/// <summary>
/// Skips non-spaces
/// <summary>
LPCWSTR SkipNonSpaces(
    __in_opt    LPCWSTR       pszIn,
    __inout_opt std::wstring *pstrOut
    );

/// <summary>
/// Skips decimal digits and optionaly returns the value
/// <summary>
LPCWSTR SkipUnsignedDecimalInt(
    __in_opt  LPCWSTR       pszIn,
    __out_opt unsigned int *pValue
    );

/// <summary>
/// Auxiliary function to check if a string starts with a token
/// </summary>
LPCWSTR SkipPrefix(
    __in_opt LPCWSTR pszIn,
    __in_opt LPCWSTR pszPrefix
    );

// <summary>
// Reads the contents of a file to a string
// </summary>
HRESULT FileReadAllText(
    __in    LPCWSTR      pszFilePath,
    __inout std::string &strText
    );

/// <summary>
/// Checks if a string is a command token
/// </summary>
inline bool StringIsCommand(
    __in_opt LPCWSTR pszIn,
    __in_opt LPCWSTR pszCommand )
{
    pszIn = SkipSpaces( pszIn );
    pszIn = SkipPrefix( pszIn, pszCommand );
    pszIn = SkipSpaces( pszIn );

    return ( pszIn != nullptr ) && ( *pszIn == L'\0' );
}

inline bool StringIsEmpty( __in_opt LPCWSTR pszIn )
{
    pszIn = SkipSpaces( pszIn );

    return ( pszIn != nullptr ) && ( *pszIn == L'\0' );
}

/// <summary>
/// Converts a DRM_ID to string
/// </summary>
class DRMIDString
{
public:

    DRMIDString( __in const DRM_ID& id );

    const LPCWSTR Value() const
    {
        return m_buffer;
    }

private:

    WCHAR m_buffer[ARRAYSIZE( "0000-0000-0000-0000-0000-0000-0000-0000" )];
};
