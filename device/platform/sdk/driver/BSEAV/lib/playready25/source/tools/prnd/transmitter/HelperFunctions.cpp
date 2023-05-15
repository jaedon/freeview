/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <HelperFunctions.h>

using namespace std;


template <typename T>
int split_t( const T &text, const T &separators, vector<T> &words )
{
    string::size_type textLen       = text.length();
    string::size_type startIndex    = text.find_first_not_of( separators, 0 );
    
    while( ( startIndex != string::npos ) && ( startIndex < textLen ) )
    {
        string::size_type stopIndex = text.find_first_of( separators, startIndex );

        if( stopIndex == string::npos )
        {
            stopIndex = textLen;
        }

        words.push_back( text.substr( startIndex, stopIndex - startIndex ) );
        
        startIndex = text.find_first_not_of( separators, stopIndex + 1 );
    }
    
    return ( int )words.size();
}

int string_split( const string &text, const string &separators, vector<string> &words )
{
    return split_t<string>( text, separators, words );
}

int wstring_split( const wstring &text, const wstring &separators, vector<wstring> &words )
{
    return split_t<wstring>( text, separators, words );
}

string string_trim( const string &s, const char *trimArray )
{
    if( s.empty() )
    {
        return s;
    }
    
    string::size_type beginIndex    = s.find_first_not_of( trimArray );
    string::size_type endIndex      = s.find_last_not_of( trimArray );
    if( beginIndex == string::npos )
    {
        return "";
    }
    
    return string( s, beginIndex, endIndex - beginIndex + 1 );
}

string string_tolower( const string &s )
{
    string str;

    std::transform( s.begin(), s.end(), str.begin(), tolower );
    return str;
}

string string_toupper( const string &s )
{
    string str;
    
    std::transform( s.begin(), s.end(), str.begin(), toupper );
    return str;
}

bool string_compare_case_insensitive( string s1,  string s2 )
{
    s1 = string_trim( s1, " " );
    s2 = string_trim( s2, " " );
    
    transform( s1.begin(), s1.end(), s1.begin(), toupper );
    transform( s2.begin(), s2.end(), s2.begin(), toupper );
    
    return s1 == s2;
}

bool string_starts_with( const string &str, const string &with )
{
    if( str.length() < with.length() )
    {
        return false;
    }
    else
    {
        string s( str, 0, with.length() );
        return string_compare_case_insensitive( s, with );
    }
}

bool string_ends_with( const string &str, const string &with )
{
    if( str.length() < with.length() )
    {
        return false;
    }
    else
    {
        string s( str, str.length() - with.length(), with.length() );
        return string_compare_case_insensitive( s, with );
    }
    
}

std::wstring string_to_wstring( const std::string &s )
{
    std::wstring temp( s.length(),L' ' );
    
    std::copy( s.begin(), s.end(), temp.begin());
    return temp; 
}

std::string wstring_to_string( const std::wstring &s )
{
    std::string temp( s.length(), ' ' );
    
    std::copy( s.begin(), s.end(), temp.begin() );
    return temp; 
}

std::string append_backslash_if_not_exist( const std::string &s )
{
    string str( s );
    
    if( str.empty() )
    {
        return str;
    }

    if( str.back() != '\\' )
    {
        str += '\\';
    }
    
    return str;
}

vector<BYTE> hexstring_to_bytes( const string &s )
{
    size_t          i = 0;
    vector<BYTE>    vb;

    if( s.empty() )
    {
        return vb;
    }
    
    while( i < s.size()-1 )
    {
        string strHexByte( s, i, 2 );
        
        BYTE byte = ( BYTE )std::stoi( strHexByte, nullptr, 16 );
        vb.push_back( byte );

        i += 2;
    }

    return vb;
}

void find_and_replace_substring( std::string &strSource, const std::string &strFind, const std::string &strReplace )
{
    std::string::size_type pos = 0;

    while( ( pos = strSource.find( strFind, pos ) ) != std::string::npos )
    {
        strSource.replace( pos, strFind.length(), strReplace );
        pos += strReplace.length();
    }
    
}

void string_URLDecode( std::string &strUrl )
{
    find_and_replace_substring( strUrl, "%26", "&" );
    find_and_replace_substring( strUrl, "%20", " " );

    find_and_replace_substring( strUrl, "%3D", "=" );
    find_and_replace_substring( strUrl, "%3A", ":" );
    find_and_replace_substring( strUrl, "%2F", "/" );
}

HRESULT HexFromChar(
    __in  char  ch, 
    __out BYTE &b )
{
    HRESULT hr = S_OK;
  
    if( '0' <= ch && ch <= '9' )
    {
        b = ( BYTE )( ch - '0' );
    }
    else if( 'a' <= ch && ch <= 'f' )
    {
        b = ( BYTE )( ( 10 + ch - 'a' ) );
    }
    else if( 'A' <= ch && ch <= 'F' )
    {
        b = ( BYTE )( ( 10 + ch - 'A' ) );
    } 
    else
    {
        hr = E_FAIL;
    }
    return hr;
}

void HexToChar(
    __in  BYTE  b,
    __out char &ch )
{
    if( b >= 10 )
    {
        ch = b - 10 + 'A';
    }
    else
    {
        ch = b + '0';
    }
}

HRESULT Base16Decode(
    __in_z                         char  *pszEncodedData,
    __deref_out_bcount( *pcbData ) BYTE **ppbDecodedData,
    __out                          DWORD *pcbDecodedData )
{
    HRESULT hr     = S_OK;
    char   *ptrSrc = pszEncodedData;
    BYTE   *pbDest;
    int     n;

    if( pszEncodedData == NULL
     || ppbDecodedData == NULL
     || pcbDecodedData == NULL )
    {
        hr = E_INVALIDARG;
        goto ErrorExit;
    }

    n = ( int ) strlen( pszEncodedData );
    // Make sure the number of characters is an even number.
    if( n % 2 != 0 )
    {
        hr = E_INVALIDARG;
        goto ErrorExit;
    }

    *pcbDecodedData = ( DWORD )( n / 2 );
    *ppbDecodedData = new BYTE[*pcbDecodedData];
    if( *ppbDecodedData == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto ErrorExit;
    }
    pbDest = *ppbDecodedData;

    for( DWORD i = 0; i < *pcbDecodedData; i++ )
    {
        BYTE b1, b2;
        
        hr = HexFromChar( *ptrSrc++, b1 );
        if( FAILED( hr ) )
        {
            goto ErrorExit;
        }
        
        hr = HexFromChar( *ptrSrc++, b2 );
        if( FAILED( hr ) )
        {
            goto ErrorExit;
        }

        *pbDest++ = ( BYTE ) ( ( b1 << 4 ) | b2 );
    }    
    
ErrorExit:
    return hr;
}

HRESULT Base16Encode(
    __in_bcount( cbDecodedData )  BYTE    *pbDecodedData,
    __in                          DWORD    cbDecodedData,
    __deref_out_z                 char   **ppszEncodedData )
{
    HRESULT hr             = S_OK;
    char   *pszEncodedData = NULL;

    if( pbDecodedData   == NULL
     || cbDecodedData   == 0
     || ppszEncodedData == NULL )
    {
        hr = E_INVALIDARG;
        goto ErrorExit;
    }

    pszEncodedData = new char[cbDecodedData * 2 + 1];
    if( pszEncodedData == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto ErrorExit;
    }

    for( DWORD i = 0; i < cbDecodedData; i++ )
    {
        HexToChar( ( BYTE ) ( pbDecodedData[i] >> 4 ), pszEncodedData[i*2] );
        HexToChar( ( BYTE ) ( pbDecodedData[i] & 0x0F ), pszEncodedData[i*2+1] );
    }

    pszEncodedData[cbDecodedData * 2] = 0;

    *ppszEncodedData = pszEncodedData;
    
ErrorExit:
    return hr;
}

DWORD GetCurrentTimeInSeconds()
{
    DRMFILETIME ftCurrentTime   = { 0 };
    DRM_DWORD   dwCurrentTime   = 0;
    
    Oem_Clock_GetSystemTimeAsFileTime( NULL, &ftCurrentTime );
    FILE_TIME_TO_DWORD( ftCurrentTime, dwCurrentTime );
    return dwCurrentTime;
}

