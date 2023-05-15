/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#pragma once

extern int string_split( const std::string &text, const std::string &separators, std::vector<std::string> &words );
extern int wstring_split( const std::wstring &text, const std::wstring &separators, std::vector<std::wstring> &words );

extern std::string string_trim( const std::string &s, const char *trimArray );
extern std::string string_tolower( const std::string &ss );
extern std::string string_toupper( const std::string &ss );

extern std::wstring string_to_wstring( const std::string &s );
extern std::string wstring_to_string( const std::wstring &s );

extern std::string append_backslash_if_not_exist( const std::string &s );

extern bool string_compare_case_insensitive( std::string s1,  std::string s2 );
extern bool string_starts_with( const std::string &str, const std::string &with );
extern bool string_ends_with( const std::string &str, const std::string &with );

extern std::vector<BYTE> hexstring_to_bytes( const std::string &s );
extern void find_and_replace_substring( std::string &strSource, const std::string &strFind, const std::string &strReplace );

template< typename T > 
std::string integer_to_hexstring( T i ) 
{ 
    std::stringstream stream; 
    
    stream << std::hex << i; 
    return stream.str(); 
} 


extern HRESULT Base16Encode(
    __in_bcount( cbDecodedData )  BYTE    *pbDecodedData,
    __in                          DWORD    cbDecodedData,
    __deref_out_z                 char   **ppszEncodedData );

extern DWORD GetCurrentTimeInSeconds();
extern void string_URLDecode( std::string &strUrl );
