/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmutf.h>
#include <UTFTestAPI.h>
#include <drmmathsafe.h>

ENTER_PKTEST_NAMESPACE_CODE;

/***************************************************************************
**
** Function: TestDRM_UTF8_VerifyBytes
**
** Synopsis: Function uses various byte sequences to check if it's valid UTF-8
**
** Arguments:
**
** [argc]       -- Total number of arguments.
** [argv]       -- Pointer to a array of arguments.
**
** argv[0]      -- file name with binary data
** argv[1]      -- test data size in bytes, ignore if set into 0
** argv[2]      -- position to begin reading
** argv[3]      -- positing to end reading, if argv[1] == 0 then read everything to the end
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
***************************************************************************/

DRM_RESULT DRM_CALL TestDRM_UTF8_VerifyBytes(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_BYTE   *pbData = NULL;
    DRM_DWORD  cbTemp  = 0;
    DRM_DWORD  cbData  = 0;
    DRM_DWORD  cbStart = 0;
    DRM_DWORD  cbEnd   = 0;

    ChkArg( argc == 4 );
    if ( !IsDigitalOnlyString( argv[1] )
        || !IsDigitalOnlyString( argv[2] )
        || !IsDigitalOnlyString( argv[3] ) )
    {
        Log( "Trace", "\t\tTestDRM_DOM_VerifyUTF8 error: arguments #2,3,4 must be numbers." );
        ChkDR(DRM_E_FAIL);
    }

    if ( !LoadTestFile( "data", argv[0], &pbData, &cbData ) )
    {
        Log( "Trace", "\t\tTestDRM_DOM_VerifyUTF8 error: failed to load file data\\%s", argv[0] );
        ChkDR( DRM_E_FAIL );
    }

    cbTemp = (DRM_DWORD)atol(argv[1]);
    if( cbTemp > 0 )
    {
        cbData = cbTemp;
    }

    cbStart = (DRM_DWORD)atol(argv[2]);

    if( cbTemp > 0 )
    {
        cbEnd = (DRM_DWORD)atol(argv[3]);
    }
    else
    {
        cbEnd = cbData - 1;
    }

    ChkDR( DRM_UTF8_VerifyBytes( pbData, cbData, cbStart, cbEnd ) );

ErrorExit:
    SAFE_OEM_FREE( pbData );
    return dr;
}

/***************************************************************************
**
** Function: TestDRM_UTF_RoundtripForSymbolUTF8
**
** Synopsis: Performs UTF8->UTF16->UTF32->UTF16->UTF8 conversion
**
** Arguments:
**
** [argc]       -- Total number of arguments.
** [argv]       -- Pointer to a array of arguments.
**
** argv[0]      -- UTF8 symbols sequence (hex value)
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_FAIL if the conversion returns unexpected result
**              DRM_E_TEST_UNEXPECTED_OUTPUT if result does not match original value.
**              UTF errors (0X8004CA00+) if UTF conversion failed
***************************************************************************/
DRM_RESULT DRM_CALL TestDRM_UTF_RoundtripForSymbolUTF8(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT  dr                 = DRM_SUCCESS;
    DRM_BYTE    rgbUTF8data[]      = { 0, 0, 0, 0, 0, };
    DRM_WCHAR   rgwchUTF16data[]   = { 0, 0, 0, 0, 0, };
    DRM_WCHAR32 rgwch32UTF32data[] = { 0, 0, 0, 0, 0, };
    DRM_DWORD   dwValue            = 0;
    DRM_DWORD   dwSize             = 0;
    DRM_DWORD   dwSize2            = 0;
    DRM_DWORD   dwInputSize        = 0;

    ChkArg( argc == 1 );
    dwValue    = strtoul( argv[0], NULL, 16 );

    ZEROMEM(rgwch32UTF32data, MAX_BUFFER_SIZE*SIZEOF(DRM_WCHAR32) );
    ZEROMEM(rgwchUTF16data,   MAX_BUFFER_SIZE*SIZEOF(DRM_WCHAR) );
    ZEROMEM(rgbUTF8data,      MAX_BUFFER_SIZE*SIZEOF(DRM_BYTE) );


    ChkDR( DRM_DWordToByte( ( dwValue & 0xFF000000 ) >> 24, &rgbUTF8data[0] ) );
    ChkDR( DRM_DWordToByte( ( dwValue & 0x00FF0000 ) >> 16, &rgbUTF8data[1] ) );
    ChkDR( DRM_DWordToByte( ( dwValue & 0x0000FF00 ) >>  8, &rgbUTF8data[2] ) );
    ChkDR( DRM_DWordToByte( ( dwValue & 0x000000FF ) >>  0, &rgbUTF8data[3] ) );
    dwInputSize = 4;

    dr = DRM_STR_UTF8toUTF16( (DRM_CHAR*)rgbUTF8data, 0, dwInputSize, NULL, &dwSize);
    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = DRM_E_FAIL;
        }
        ChkDR(dr);
    }
    dwSize2 = MAX_BUFFER_SIZE;
    ChkDR( DRM_STR_UTF8toUTF16( (DRM_CHAR*)rgbUTF8data, 0, dwInputSize, rgwchUTF16data, &dwSize2 ) );
    ChkBOOL( dwSize == dwSize2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwInputSize = dwSize - 1; /* -1 - excluding NULL terminator */

    dr = DRM_STR_UTF16toUTF32( rgwchUTF16data, dwInputSize, NULL, &dwSize);
    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = DRM_E_FAIL;
        }
        ChkDR(dr);
    }
    dwSize2 = MAX_BUFFER_SIZE;
    ChkDR( DRM_STR_UTF16toUTF32( rgwchUTF16data, dwInputSize, rgwch32UTF32data, &dwSize2 ) );
    ChkBOOL( dwSize == dwSize2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwInputSize = dwSize - 1; /* -1 - excluding NULL terminator */

    dr = DRM_STR_UTF32toUTF16( rgwch32UTF32data, dwInputSize, NULL, &dwSize);
    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = DRM_E_FAIL;
        }
        ChkDR(dr);
    }
    dwSize2 = MAX_BUFFER_SIZE;
    ChkDR( DRM_STR_UTF32toUTF16( rgwch32UTF32data, dwInputSize, rgwchUTF16data, &dwSize2 ) );
    ChkBOOL( dwSize == dwSize2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    dwInputSize = dwSize - 1; /* -1 - excluding NULL terminator */

    dr = DRM_STR_UTF16toUTF8( rgwchUTF16data, dwInputSize, NULL, 0, &dwSize);
    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = DRM_E_FAIL;
        }
        ChkDR(dr);
    }
    dwSize2 = MAX_BUFFER_SIZE;
    ChkDR( DRM_STR_UTF16toUTF8( rgwchUTF16data, dwInputSize, (DRM_CHAR*)rgbUTF8data, 0, &dwSize2 ) );
    ChkBOOL( dwSize == dwSize2, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkBOOL( rgbUTF8data[0] == ( dwValue & 0xff000000 ) >> 24
          && rgbUTF8data[1] == ( dwValue & 0x00ff0000 ) >> 16
          && rgbUTF8data[2] == ( dwValue & 0x0000ff00 ) >>  8
          && rgbUTF8data[3] == ( dwValue & 0x000000ff ), DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    return dr;
}

/***************************************************************************
**
** Function: TestDRM_UTF_RoundtripForSymbolUTF32
**
** Synopsis: Performs UTF32->UTF16->UTF8->UTF16->UTF32 conversion
**
** Arguments:
**
** [argc]       -- Total number of arguments.
** [argv]       -- Pointer to a array of arguments.
**
** argv[0]      -- Range of UTF32 symbols
** argv[1]      -- for conversion (inclusive hex value)
** argv[2]      -- Expected conversion result, hex value
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_FAIL if the conversion returns unexpected result
**              DRM_E_TEST_UNEXPECTED_OUTPUT if result does not match original value.
**              UTF errors (0X8004CA00+) if UTF conversion failed
***************************************************************************/
DRM_RESULT DRM_CALL TestDRM_UTF_RoundtripForSymbolUTF32(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT  dr                 = DRM_SUCCESS;
    DRM_RESULT  drExpected         = DRM_SUCCESS;
    DRM_BYTE    rgbUTF8data[]      = { 0, 0, 0, 0, 0 };
    DRM_WCHAR   rgwchUTF16data[]   = { 0, 0, 0, 0, 0 };
    DRM_WCHAR32 rgwch32UTF32data[] = { 0, 0, 0, 0, 0 };
    DRM_DWORD   dwStartValue       = 0;
    DRM_DWORD   dwEndValue         = 0;
    DRM_DWORD   dw                 = 0,
                dw2                = 0 ;
    DRM_DWORD   dwSize             = 0;
    DRM_DWORD   dwSize2            = 0;
    DRM_DWORD   dwInputSize        = 0;

    ChkArg( argc == 3 );
    dwStartValue = strtoul( argv[0], NULL, 16 );
    dwEndValue   = strtoul( argv[1], NULL, 16 );
    drExpected   = (DRM_RESULT) strtoul( argv[2], NULL, 16 );

    for ( dw=dwStartValue; dw<=dwEndValue; dw++ )
    {
        ZEROMEM(rgwch32UTF32data, MAX_BUFFER_SIZE*SIZEOF(DRM_WCHAR32) );
        ZEROMEM(rgwchUTF16data,   MAX_BUFFER_SIZE*SIZEOF(DRM_WCHAR) );
        ZEROMEM(rgbUTF8data,      MAX_BUFFER_SIZE*SIZEOF(DRM_BYTE) );
        rgwch32UTF32data[0] = dw;
        FIX_ENDIAN_DWORD( rgwch32UTF32data[0] );   /* Input is expected in little-endian which is not necessarily the same as native byte order. */
        dwInputSize = 1;

        dr = DRM_STR_UTF32toUTF16( rgwch32UTF32data, dwInputSize, NULL, &dwSize );
        if( dr != DRM_E_BUFFERTOOSMALL )
        {
            if ( DRM_FAILED(dr) )
            {
                 ChkBOOL( dr == drExpected, dr )
                 dr = DRM_SUCCESS;
                 continue;
            }
        }
        dwSize2 = MAX_BUFFER_SIZE;
        dr = DRM_STR_UTF32toUTF16( rgwch32UTF32data, dwInputSize, rgwchUTF16data, &dwSize2 );
        if ( DRM_FAILED(dr) )
        {
            ChkBOOL( dr == drExpected, dr )
            dr = DRM_SUCCESS;
            continue;
        }
        ChkBOOL( dwSize == dwSize2, DRM_E_TEST_UNEXPECTED_OUTPUT );
        dwInputSize = dwSize - 1; /* -1 - excluding NULL terminator */

        dr = DRM_STR_UTF16toUTF8( rgwchUTF16data, dwInputSize, NULL, 0, &dwSize );
        if( dr != DRM_E_BUFFERTOOSMALL )
        {
            if ( DRM_FAILED(dr) )
            {
                 ChkBOOL( dr == drExpected, dr )
                 dr = DRM_SUCCESS;
                 continue;
            }
        }
        dwSize2 = MAX_BUFFER_SIZE;
        dr = DRM_STR_UTF16toUTF8( rgwchUTF16data, dwInputSize, (DRM_CHAR*)rgbUTF8data, 0, &dwSize2 );
        if ( DRM_FAILED(dr) )
        {
            ChkBOOL( dr == drExpected, dr )
            dr = DRM_SUCCESS;
            continue;
        }
        ChkBOOL( dwSize == dwSize2, DRM_E_TEST_UNEXPECTED_OUTPUT );
        dwInputSize = dwSize - 1; /* -1 - excluding NULL terminator */

        dr = DRM_STR_UTF8toUTF16( (DRM_CHAR*)rgbUTF8data, 0, dwInputSize, NULL, &dwSize);
        if( dr != DRM_E_BUFFERTOOSMALL )
        {
            if ( DRM_FAILED(dr) )
            {
                 ChkBOOL( dr == drExpected, dr )
                 dr = DRM_SUCCESS;
                 continue;
            }
        }
        dwSize2 = MAX_BUFFER_SIZE;
        dr = DRM_STR_UTF8toUTF16( (DRM_CHAR*)rgbUTF8data, 0, dwInputSize, rgwchUTF16data, &dwSize2 );
        if ( DRM_FAILED(dr) )
        {
            ChkBOOL( dr == drExpected, dr )
            dr = DRM_SUCCESS;
            continue;
        }
        ChkBOOL( dwSize == dwSize2, DRM_E_TEST_UNEXPECTED_OUTPUT );
        dwInputSize = dwSize - 1; /* -1 - excluding NULL terminator */

        dr = DRM_STR_UTF16toUTF32( rgwchUTF16data, dwInputSize, NULL, &dwSize );
        if( dr != DRM_E_BUFFERTOOSMALL )
        {
            if ( DRM_FAILED(dr) )
            {
                 ChkBOOL( dr == drExpected, dr )
                 dr = DRM_SUCCESS;
                 continue;
            }
        }
        dwSize2 = MAX_BUFFER_SIZE;
        dr = DRM_STR_UTF16toUTF32( rgwchUTF16data, dwInputSize, rgwch32UTF32data, &dwSize2 );
        if ( DRM_FAILED(dr) )
        {
            ChkBOOL( dr == drExpected, dr )
            dr = DRM_SUCCESS;
            continue;
        }
        ChkBOOL( dwSize == dwSize2, DRM_E_TEST_UNEXPECTED_OUTPUT );

        ChkBOOL( dr == drExpected, dr );

        FIX_ENDIAN_DWORD( rgwch32UTF32data[0] );   /* Convert back to native byte order (if needed) for final comparison. */
        ChkBOOL( rgwch32UTF32data[0] == dw, DRM_E_TEST_UNEXPECTED_OUTPUT );
        for ( dw2= 1; dw2 < MAX_BUFFER_SIZE; dw2++ )
        {
           ChkBOOL( rgwch32UTF32data[dw2] == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }
ErrorExit:
    return dr;
}

/***************************************************************************
**
** Function: TestDRM_UTF_FileUTF8toUTF16
**
** Synopsis: Performs UTF8->UTF16 file conversion
**
** Arguments:
**
** [argc]       -- Total number of arguments.
** [argv]       -- Pointer to a array of arguments.
**
** argv[0]      -- UTF8 file folder
** argv[1]      -- UTF8 file name
** argv[2]      -- UTF16 file path and name
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_FAIL if the conversion returns unexpected result
**              DRM_E_TEST_UNEXPECTED_OUTPUT if result does not match original value.
**              UTF errors (0X8004CA00+) if UTF conversion failed
***************************************************************************/
DRM_RESULT DRM_CALL TestDRM_UTF_FileUTF8toUTF16(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr              = DRM_SUCCESS;
    DRM_DWORD        dwSizeUTF8      = 0;
    DRM_BYTE        *pbBufferUTF8    = NULL;
    DRM_DWORD        dwSizeUTF16     = 0;
    DRM_WCHAR       *pwchBufferUTF16 = NULL;
    DRM_DWORD        dwSizeTemp      = 0;
    DRM_CONST_STRING dstrOutFilename = EMPTY_DRM_STRING;

    ChkArg( argc == 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    ChkBOOL( LoadTestFile( argv[0], argv[1], &pbBufferUTF8, &dwSizeUTF8 ), DRM_E_FILE_READ_ERROR );

    dr = DRM_STR_UTF8toUTF16( (const DRM_CHAR*)pbBufferUTF8, 0, dwSizeUTF8, NULL, &dwSizeUTF16 );
    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = DRM_E_FAIL;
        }
        ChkDR(dr);
    }

    ChkMem( pwchBufferUTF16 = (DRM_WCHAR*) Oem_MemAlloc( dwSizeUTF16 * SIZEOF(DRM_WCHAR) ) );
    dwSizeTemp = dwSizeUTF16;
    ChkDR( DRM_STR_UTF8toUTF16( (DRM_CHAR*)pbBufferUTF8, 0, dwSizeUTF8, pwchBufferUTF16, &dwSizeTemp ) );
    ChkBOOL( dwSizeUTF16 == dwSizeTemp, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( TST_UTL_MakePathString( NULL, argv[2], &dstrOutFilename ) );
    /* Save the UTF16 string without NULL terminator*/
    ChkBOOL( SaveToFileW( dstrOutFilename.pwszString, (DRM_BYTE *)pwchBufferUTF16, (dwSizeUTF16-1) * SIZEOF(DRM_WCHAR) ), DRM_E_FILE_WRITE_ERROR );

ErrorExit:
    SAFE_OEM_FREE( pbBufferUTF8 );
    SAFE_OEM_FREE( pwchBufferUTF16 );
    FREEDRMSTRING( dstrOutFilename );
    return dr;
}

/***************************************************************************
**
** Function: TestDRM_UTF_FileUTF16toUTF8
**
** Synopsis: Performs UTF16->UTF8 file conversion
**
** Arguments:
**
** [argc]       -- Total number of arguments.
** [argv]       -- Pointer to a array of arguments.
**
** argv[0]      -- UTF16 file folder
** argv[1]      -- UTF16 file name
** argv[2]      -- UTF8 file path and name
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_FAIL if the conversion returns unexpected result
**              DRM_E_TEST_UNEXPECTED_OUTPUT if result does not match original value.
**              UTF errors (0X8004CA00+) if UTF conversion failed
***************************************************************************/
DRM_RESULT DRM_CALL TestDRM_UTF_FileUTF16toUTF8(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr              = DRM_SUCCESS;
    DRM_DWORD        dwSizeUTF8      = 0;
    DRM_BYTE        *pbBufferUTF8    = NULL;
    DRM_DWORD        dwSizeUTF16     = 0;
    DRM_WCHAR       *pwchBufferUTF16 = NULL;
    DRM_DWORD        dwSizeTemp      = 0;
    DRM_CONST_STRING dstrOutFilename = EMPTY_DRM_STRING;

    ChkArg( argc == 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    ChkBOOL( LoadTestFile( argv[0], argv[1], (DRM_BYTE**)&pwchBufferUTF16, &dwSizeUTF16 ), DRM_E_FILE_READ_ERROR );
    ChkBOOL( dwSizeUTF16 % SIZEOF(DRM_WCHAR) == 0, DRM_E_FAIL );
    dwSizeUTF16 /= SIZEOF(DRM_WCHAR);

    dr = DRM_STR_UTF16toUTF8( pwchBufferUTF16, dwSizeUTF16, NULL, 0, &dwSizeUTF8 );
    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = DRM_E_FAIL;
        }
        ChkDR(dr);
    }

    ChkMem( pbBufferUTF8 = (DRM_BYTE*) Oem_MemAlloc( dwSizeUTF8 ) );
    dwSizeTemp = dwSizeUTF8;
    ChkDR( DRM_STR_UTF16toUTF8( pwchBufferUTF16, dwSizeUTF16, (DRM_CHAR*)pbBufferUTF8, 0, &dwSizeTemp ) );
    ChkBOOL( dwSizeUTF8 == dwSizeTemp, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( TST_UTL_MakePathString( NULL, argv[2], &dstrOutFilename ) );
    /* Save the UTF8 string without NULL terminator*/
    ChkBOOL( SaveToFileW( dstrOutFilename.pwszString, pbBufferUTF8, dwSizeUTF8-1 ), DRM_E_FILE_WRITE_ERROR );

ErrorExit:
    SAFE_OEM_FREE( pbBufferUTF8 );
    SAFE_OEM_FREE( pwchBufferUTF16 );
    FREEDRMSTRING( dstrOutFilename );
    return dr;
}

/***************************************************************************
**
** Function: TestDRM_UTF_FileUTF16toUTF32
**
** Synopsis: Performs UTF16->UTF32 file conversion
**
** Arguments:
**
** [argc]       -- Total number of arguments.
** [argv]       -- Pointer to a array of arguments.
**
** argv[0]      -- UTF16 file folder
** argv[1]      -- UTF16 file name
** argv[2]      -- UTF32 file path and name
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_FAIL if the conversion returns unexpected result
**              DRM_E_TEST_UNEXPECTED_OUTPUT if result does not match original value.
**              UTF errors (0X8004CA00+) if UTF conversion failed
***************************************************************************/
DRM_RESULT DRM_CALL TestDRM_UTF_FileUTF16toUTF32(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr                = DRM_SUCCESS;
    DRM_DWORD        dwSizeUTF16       = 0;
    DRM_WCHAR       *pwchBufferUTF16   = NULL;
    DRM_DWORD        dwSizeUTF32       = 0;
    DRM_WCHAR32     *pwch32BufferUTF32 = NULL;
    DRM_DWORD        dwSizeTemp        = 0;
    DRM_CONST_STRING dstrOutFilename   = EMPTY_DRM_STRING;

    ChkArg( argc == 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    ChkBOOL( LoadTestFile( argv[0], argv[1], (DRM_BYTE**)&pwchBufferUTF16, &dwSizeUTF16 ), DRM_E_FILE_READ_ERROR );
    ChkBOOL( dwSizeUTF16 % SIZEOF(DRM_WCHAR) == 0, DRM_E_FAIL );
    dwSizeUTF16 /= SIZEOF(DRM_WCHAR);

    dr = DRM_STR_UTF16toUTF32( pwchBufferUTF16, dwSizeUTF16, NULL, &dwSizeUTF32 );
    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = DRM_E_FAIL;
        }
        ChkDR(dr);
    }

    ChkMem( pwch32BufferUTF32 = (DRM_WCHAR32*) Oem_MemAlloc( dwSizeUTF32 * SIZEOF(DRM_WCHAR32) ) );
    dwSizeTemp = dwSizeUTF32;
    ChkDR( DRM_STR_UTF16toUTF32( pwchBufferUTF16, dwSizeUTF16, pwch32BufferUTF32, &dwSizeTemp ) );
    ChkBOOL( dwSizeUTF32 == dwSizeTemp, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( TST_UTL_MakePathString( NULL, argv[2], &dstrOutFilename ) );
    /* Save the UTF32 string without NULL terminator*/
    ChkBOOL( SaveToFileW( dstrOutFilename.pwszString, (DRM_BYTE*)pwch32BufferUTF32, (dwSizeUTF32-1) * SIZEOF(DRM_WCHAR32) ), DRM_E_FILE_WRITE_ERROR );

ErrorExit:
    SAFE_OEM_FREE( pwchBufferUTF16 );
    SAFE_OEM_FREE( pwch32BufferUTF32 );
    FREEDRMSTRING( dstrOutFilename );
    return dr;
}

/***************************************************************************
**
** Function: TestDRM_UTF_FileUTF32toUTF16
**
** Synopsis: Performs UTF32->UTF16 file conversion
**
** Arguments:
**
** [argc]       -- Total number of arguments.
** [argv]       -- Pointer to a array of arguments.
**
** argv[0]      -- UTF32 file folder
** argv[1]      -- UTF32 file name
** argv[2]      -- UTF16 file path and name
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_FAIL if the conversion returns unexpected result
**              DRM_E_TEST_UNEXPECTED_OUTPUT if result does not match original value.
**              UTF errors (0X8004CA00+) if UTF conversion failed
***************************************************************************/
DRM_RESULT DRM_CALL TestDRM_UTF_FileUTF32toUTF16(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr                = DRM_SUCCESS;
    DRM_DWORD        dwSizeUTF16       = 0;
    DRM_WCHAR       *pwchBufferUTF16   = NULL;
    DRM_DWORD        dwSizeUTF32       = 0;
    DRM_WCHAR32     *pwch32BufferUTF32 = NULL;
    DRM_DWORD        dwSizeTemp        = 0;
    DRM_CONST_STRING dstrOutFilename   = EMPTY_DRM_STRING;

    ChkArg( argc == 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    ChkBOOL( LoadTestFile( argv[0], argv[1], (DRM_BYTE**)&pwch32BufferUTF32, &dwSizeUTF32 ), DRM_E_FILE_READ_ERROR );
    ChkBOOL( dwSizeUTF32 % SIZEOF(DRM_WCHAR32) == 0, DRM_E_FAIL );
    dwSizeUTF32 /= SIZEOF(DRM_WCHAR32);

    dr = DRM_STR_UTF32toUTF16( pwch32BufferUTF32, dwSizeUTF32, NULL, &dwSizeUTF16 );
    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        if( DRM_SUCCEEDED( dr ) )
        {
            dr = DRM_E_FAIL;
        }
        ChkDR(dr);
    }

    ChkMem( pwchBufferUTF16 = (DRM_WCHAR*) Oem_MemAlloc( dwSizeUTF16 * SIZEOF(DRM_WCHAR) ) );
    dwSizeTemp = dwSizeUTF16;
    ChkDR( DRM_STR_UTF32toUTF16( pwch32BufferUTF32, dwSizeUTF32, pwchBufferUTF16, &dwSizeTemp ) );
    ChkBOOL( dwSizeUTF16 == dwSizeTemp, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( TST_UTL_MakePathString( NULL, argv[2], &dstrOutFilename ) );
    /* Save the UTF16 string without NULL terminator*/
    ChkBOOL( SaveToFileW( dstrOutFilename.pwszString, (DRM_BYTE*)pwchBufferUTF16, (dwSizeUTF16-1) * SIZEOF(DRM_WCHAR) ), DRM_E_FILE_WRITE_ERROR );

ErrorExit:
    SAFE_OEM_FREE( pwchBufferUTF16 );
    SAFE_OEM_FREE( pwch32BufferUTF32 );
    FREEDRMSTRING( dstrOutFilename );
    return dr;
}

/***************************************************************************
**
** Function: TestDRM_UTF_FileASCIItoUTF16
**
** Synopsis: Performs ASCII->UTF16 file conversion in place
**
** Arguments:
**
** [argc]       -- Total number of arguments.
** [argv]       -- Pointer to a array of arguments.
**
** argv[0]      -- ASCII file folder
** argv[1]      -- ASCII file name
** argv[2]      -- UTF16 file path and name
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_TEST_UNEXPECTED_OUTPUT if the conversion returns unexpected result
**              UTF errors (0X8004CA00+) if UTF conversion failed
***************************************************************************/
DRM_RESULT DRM_CALL TestDRM_UTF_FileASCIItoUTF16(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr              = DRM_SUCCESS;
    DRM_DWORD        dwSize          = 0;
    DRM_BYTE        *pbBuffer        = NULL;
    DRM_CONST_STRING dstrOutFilename = EMPTY_DRM_STRING;
    DRM_STRING       dstrUTF16_1     = EMPTY_DRM_STRING;
    DRM_STRING       dstrUTF16_2     = EMPTY_DRM_STRING;
    DRM_SUBSTRING    dasstr          = EMPTY_DRM_SUBSTRING;

    ChkArg( argc == 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    ChkBOOL( LoadTestFile( argv[0], argv[1], &pbBuffer, &dwSize ), DRM_E_FILE_READ_ERROR );

    ChkMem( dstrUTF16_1.pwszString = (DRM_WCHAR*) Oem_MemAlloc( dwSize * SIZEOF(DRM_WCHAR) ) );
    dstrUTF16_1.cchString = dwSize;
    DRMCRT_memcpy( (DRM_BYTE*)dstrUTF16_1.pwszString, pbBuffer, dwSize );
    ChkDR( DRM_STR_ASCIItoDSTR( (DRM_CHAR*)dstrUTF16_1.pwszString, dstrUTF16_1.cchString, &dstrUTF16_1 ) );

    /* And once more for for another method */
    ChkMem( dstrUTF16_2.pwszString = (DRM_WCHAR*) Oem_MemAlloc( dwSize * SIZEOF(DRM_WCHAR) ) );
    dstrUTF16_2.cchString = dwSize;
    DRMCRT_memcpy( (DRM_BYTE*)dstrUTF16_2.pwszString, pbBuffer, dwSize );
    dasstr.m_ich = 0;
    dasstr.m_cch = dwSize;
    ChkDR( DRM_STR_ASCII_SUBSTRtoDSTR( (DRM_CHAR*)dstrUTF16_2.pwszString, &dasstr, &dstrUTF16_2 ) );

    /* Buffers has to be identical */
    ChkBOOL( DRMCRT_memcmp( dstrUTF16_1.pwszString, dstrUTF16_2.pwszString, dwSize )==0, DRM_E_TEST_UNEXPECTED_OUTPUT );

    ChkDR( TST_UTL_MakePathString( NULL, argv[2], &dstrOutFilename ) );
    /* Save the UTF16 string without NULL terminator*/
    ChkBOOL( SaveToFileW( dstrOutFilename.pwszString, (DRM_BYTE *)dstrUTF16_1.pwszString, dwSize * SIZEOF(DRM_WCHAR) ), DRM_E_FILE_WRITE_ERROR );

ErrorExit:
    SAFE_OEM_FREE( pbBuffer );
    FREEDRMSTRING( dstrUTF16_1 );
    FREEDRMSTRING( dstrUTF16_2 );
    FREEDRMSTRING( dstrOutFilename );
    return dr;
}

/***************************************************************************
**
** Function: TestDRM_UTF_FileUTF16toASCII
**
** Synopsis: Performs UTF16->ASCII file conversion in place
**           Note: UTF16 file must contain ASCII symbols only
**
** Arguments:
**
** [argc]       -- Total number of arguments.
** [argv]       -- Pointer to a array of arguments.
**
** argv[0]      -- UTF16 file folder
** argv[1]      -- UTF16 file name
** argv[2]      -- ASCII file path and name
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_TEST_UNEXPECTED_OUTPUT if the conversion returns unexpected result
**              UTF errors (0X8004CA00+) if UTF conversion failed
***************************************************************************/
DRM_RESULT DRM_CALL TestDRM_UTF_FileUTF16toASCII(
    __in long argc,
    __in_ecount(argc) char **argv )
{
    DRM_RESULT       dr              = DRM_SUCCESS;
    DRM_DWORD        dwSize          = 0;
    DRM_BYTE        *pbBuffer1       = NULL;
    DRM_CONST_STRING dstrOutFilename = EMPTY_DRM_STRING;

    ChkArg( argc == 3 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[2] != NULL );

    ChkBOOL( LoadTestFile( argv[0], argv[1], (DRM_BYTE**)&pbBuffer1, &dwSize ), DRM_E_FILE_READ_ERROR );
    ChkBOOL( dwSize % SIZEOF(DRM_WCHAR) == 0, DRM_E_FILE_READ_ERROR );
    dwSize /= SIZEOF(DRM_WCHAR);

    ChkDR( DRM_STR_UTF16toASCII( (DRM_WCHAR*)pbBuffer1, dwSize, (DRM_CHAR*)pbBuffer1, dwSize * SIZEOF(DRM_WCHAR) ) );

    ChkDR( TST_UTL_MakePathString( NULL, argv[2], &dstrOutFilename ) );
    /* Save the ASCII string */
    ChkBOOL( SaveToFileW( dstrOutFilename.pwszString, pbBuffer1, dwSize ), DRM_E_FILE_WRITE_ERROR );

ErrorExit:
    SAFE_OEM_FREE( pbBuffer1 );
    FREEDRMSTRING( dstrOutFilename );
    return dr;
}

IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE_FOR_MODULE( UTF )

BEGIN_TEST_API_LIST( UTF )
    API_ENTRY(TestDRM_UTF8_VerifyBytes)
    API_ENTRY(TestDRM_UTF_RoundtripForSymbolUTF8)
    API_ENTRY(TestDRM_UTF_RoundtripForSymbolUTF32)
    API_ENTRY(TestDRM_UTF_FileUTF8toUTF16)
    API_ENTRY(TestDRM_UTF_FileUTF16toUTF8)
    API_ENTRY(TestDRM_UTF_FileUTF32toUTF16)
    API_ENTRY(TestDRM_UTF_FileUTF16toUTF32)
    API_ENTRY(TestDRM_UTF_FileASCIItoUTF16)
    API_ENTRY(TestDRM_UTF_FileUTF16toASCII)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE



