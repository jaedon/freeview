/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmbase64.h>
#include <OEMTestAPI.h>
#include <drmhdsblockheadercache.h>
#include <drmmathsafe.h>
#include <oemhal.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PKTEST_NAMESPACE_CODE;

TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY g_rgOemKeyTypes[] = {
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_INVALID ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_ECC_P256_CONTENT_ENCRYPT ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_ECC_P256_DOMAIN_ENCRYPT ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_ECC_P256_MESSAGE_ENCRYPT ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_ECC_P256_SIGN ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_ECC_P256_PUBLIC ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_ECC_P256_MODEL_SIGN ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_AES_ECB_ROOT ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_AES_CTR_CONTENT ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_AES_ECB_MESSAGE_ENCRYPT ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_AES_ECB_DOMAIN_PROTECT ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_AES_OMAC1_SECURE_STORE ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_AES_KEYFILE ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_CONTENT ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_AES_GENERIC_KEY_ROOT ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_AES_ECB_NEW_LICENSE_ROOT ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_AES_CTR_NEW_LICENSE_CONTENT ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_LEGACY ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_AES_SLK ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_NUM_VALID_TYPES ),
    CREATE_TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY( OEM_HAL_KEY_TYPE_UNDEFINED )
};

/* Use a local path for testing  */
static const DRM_WCHAR g_rgwchTestDrmPath [] =
{
    ONE_WCHAR('.', '\0'),  ONE_WCHAR('\\', '\0'), ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),
    ONE_WCHAR('s', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('w', '\0'),  ONE_WCHAR('m', '\0'),
    ONE_WCHAR('d', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('m', '\0'),
    ONE_WCHAR('d', '\0'),  ONE_WCHAR('\\', '\0'), ONE_WCHAR('\0', '\0')
};

/* This is used for testing OEM Block Header Cache functions.   */
/* If a valid handle is required, set this value in PreTestCase */
OEM_FILEHDL    g_hHDSFileHandle = OEM_INVALID_HANDLE_VALUE;

/*********************************************************
**
** Function: TestOEMSystemTimeFileTimeConversion
**
** Synopsis: This test the functions Oem_Clock_SystemTimeToFileTime
** and Oem_Clock_FileTimeToSystemTime to ensure conversion doesn't
** drop data.
**
** Arguments:
**  argv[0] -- Time to use. Valid values are "CURRENT", "RANDOM", or "FIXED". (optional, default is "FIXED")
**  argv[1] -- System Year (optional, default for "FIXED" is 2000)
**  argv[2] -- System Month (optional, default for "FIXED" is 2 (February))
**  argv[3] -- System Day Of The Week (optional, default for "FIXED" is 2 (Tuesday))
**  argv[4] -- System Day (optional, default for "FIXED" is 29)
**  argv[5] -- System Hour (optional, default for "FIXED" is 20)
**  argv[6] -- System Minute (optional, default for "FIXED" is 9)
**  argv[7] -- System Second (optional, default for "FIXED" is 42)
**  argv[8] -- System Millisecond (optional, default for "FIXED" is 13)
**
**  NOTE: If specifying a fixed time, either use all default values for arguments 1-8
**        or specify all values.
**        If using current or random time, arguments 1-8 are ignored
*********************************************************/
DRM_RESULT DRM_CALL TestOEMSystemTimeFileTimeConversion(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr                = DRM_E_TEST_INCOMPLETE;
    DRMSYSTEMTIME    systimeTestTime   = {2000, 2, 2, 29, 20, 9, 42, 13};
    DRMSYSTEMTIME    systimeVerifyTime = {0};
    DRMFILETIME      filetimeTestTime  = {0};


    /* Set the test system time */
    if ( ( argc > 0 ) && ( argv[0] != NULL ) )
    {
        if ( strncmp( argv[0], "CURRENT", 7 ) == 0 )
        {
            Oem_Clock_GetSystemTime( NULL, &systimeTestTime );
            Log("Trace", "\t\tUsing Current Time: (weekday %d) %d/%d/%d at %d:%d:%d:%d", systimeTestTime.wDayOfWeek, systimeTestTime.wMonth, systimeTestTime.wDay, systimeTestTime.wYear, systimeTestTime.wHour, systimeTestTime.wMinute, systimeTestTime.wSecond, systimeTestTime.wMilliseconds);
        }
        else if ( strncmp( argv[0], "RANDOM", 6 ) == 0 )
        {
            ChkDR( Oem_Random_GetBytes( NULL, (DRM_BYTE *)&systimeTestTime, SIZEOF( DRMSYSTEMTIME ) ) );
            Log("Trace", "\t\tUsing Random Time: (weekday %d) %d/%d/%d at %d:%d:%d:%d", systimeTestTime.wDayOfWeek, systimeTestTime.wMonth, systimeTestTime.wDay, systimeTestTime.wYear, systimeTestTime.wHour, systimeTestTime.wMinute, systimeTestTime.wSecond, systimeTestTime.wMilliseconds);
        }
        else if ( strncmp( argv[0], "FIXED", 5 ) == 0 )
        {
            if ( argc > 1 )
            {
                ChkArg( argc >= 9 );
                systimeTestTime.wYear         = (DRM_WORD)strtoul( argv[1], (char **)NULL, 0 );
                systimeTestTime.wMonth        = (DRM_WORD)strtoul( argv[2], (char **)NULL, 0 );
                systimeTestTime.wDayOfWeek    = (DRM_WORD)strtoul( argv[3], (char **)NULL, 0 );
                systimeTestTime.wDay          = (DRM_WORD)strtoul( argv[4], (char **)NULL, 0 );
                systimeTestTime.wHour         = (DRM_WORD)strtoul( argv[5], (char **)NULL, 0 );
                systimeTestTime.wMinute       = (DRM_WORD)strtoul( argv[6], (char **)NULL, 0 );
                systimeTestTime.wSecond       = (DRM_WORD)strtoul( argv[7], (char **)NULL, 0 );
                systimeTestTime.wMilliseconds = (DRM_WORD)strtoul( argv[8], (char **)NULL, 0 );
            }
            Log("Trace", "\t\tUsing Fixed Time: (weekday %d) %d/%d/%d at %d:%d:%d:%d", systimeTestTime.wDayOfWeek, systimeTestTime.wMonth, systimeTestTime.wDay, systimeTestTime.wYear, systimeTestTime.wHour, systimeTestTime.wMinute, systimeTestTime.wSecond, systimeTestTime.wMilliseconds);
        }
        else
        {
            /* First argument is unknown */
            ChkDR(DRM_E_NOTIMPL);
        }
    }

    /* Convert the test system time to file time */
    ChkDR( Oem_Clock_SystemTimeToFileTime ( &systimeTestTime, &filetimeTestTime ) );
    Log("Trace", "\t\tIn file time this is (low:high) %d:%d", filetimeTestTime.dwLowDateTime, filetimeTestTime.dwHighDateTime );

    /* Convert the file time back to system time */
    ChkDR( Oem_Clock_FileTimeToSystemTime ( &filetimeTestTime, &systimeVerifyTime ) );
    Log("Trace", "\t\tBack in system time this is: (weekday %d) %d/%d/%d at %d:%d:%d:%d", systimeTestTime.wDayOfWeek, systimeTestTime.wMonth, systimeTestTime.wDay, systimeTestTime.wYear, systimeTestTime.wHour, systimeTestTime.wMinute, systimeTestTime.wSecond, systimeTestTime.wMilliseconds);

    /* Check the converted system time to the original system time */
    ChkBOOL( MEMCMP( &systimeTestTime, &systimeVerifyTime, SIZEOF( DRMSYSTEMTIME ) ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Test case passed, return success */
    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}


/*********************************************************
**
** Function: TestOEM_SECURE_ZERO_MEMORY
**
** Synopsis: This test the macro OEM_SECURE_ZERO_MEMORY
** to ensure properly zeros out memory
**
** Arguments: none
**
** NOTE: This doesn't necessarily check that the compilier does
**       not optimize away the OEM_SECURE_ZERO_MEMORY.
**
*********************************************************/
DRM_RESULT DRM_CALL TestOEM_SECURE_ZERO_MEMORY(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr                 = DRM_E_TEST_INCOMPLETE;
    DRM_BYTE         bZeroBuffer[100]   = {0};
    DRM_BYTE         bNonZeroedBuffer[100] = {1};
    DRM_DWORD        cbBuffer           = 100;

    OEM_SECURE_ZERO_MEMORY( bNonZeroedBuffer, cbBuffer );

    ChkBOOL( MEMCMP( bZeroBuffer, bNonZeroedBuffer, cbBuffer ) == 0, DRM_E_FAIL );

    /* Test case passed, return success */
    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*********************************************************
**
** Function: TestOEMStringCchCopyN
**
** Synopsis: This test the function DRM_STR_StringCchCopyNW
** to ensure properly checks buffers and copies strings
**
** Arguments:
**  argv[0] -- String to copy
**  argv[1] -- Size of destination buffer in characters
**
** NOTE: If the specified size of the destination buffer is
**       less than the number of characters in the the
**       specified string + 1 (for NULL terminator), this
**       test function expects DRM_STR_StringCchCopyNW to return
**       DRM_E_BUFFERTOOSMALL and output a truncated, NULL-
**       terminated string in the destination.
**
*********************************************************/
DRM_RESULT DRM_CALL TestOEMStringCchCopyN(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr                = DRM_E_TEST_INCOMPLETE;
    DRM_CONST_STRING dstrSource        = EMPTY_DRM_STRING;
    DRM_WCHAR        *pwszDest         = NULL;
    DRM_DWORD        cchDest           = 0;
    DRM_DWORD        cbDest            = 0;
    DRM_DWORD        cbSource          = 0;

    ChkArg( argc >= 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkDR( MakeDRMString( &dstrSource, argv[0] ) );
    cchDest = strtoul( argv[1], NULL, 0 );

    /* Allocate memory for destination buffer */
    ChkOverflow( cchDest * SIZEOF( DRM_WCHAR ), cchDest );
    cbDest = cchDest * SIZEOF( DRM_WCHAR );
    ChkOverflow( dstrSource.cchString * SIZEOF( DRM_WCHAR ), dstrSource.cchString );
    cbSource = dstrSource.cchString * SIZEOF( DRM_WCHAR );

    ChkMem( pwszDest = ( DRM_WCHAR * )Oem_MemAlloc( cbDest ) );

    dr = DRM_STR_StringCchCopyNW( pwszDest, cchDest, dstrSource.pwszString, dstrSource.cchString );

    /* Verify output */
    if ( cchDest <= dstrSource.cchString )
    {
        ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, DRM_E_TEST_UNEXPECTED_OUTPUT );
        ChkOverflow( cbDest, cbDest - SIZEOF( DRM_WCHAR ) );
        ChkBOOL( MEMCMP( pwszDest, dstrSource.pwszString, cbDest - SIZEOF( DRM_WCHAR ) ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }
    else
    {
        ChkDR( dr );
        ChkBOOL( MEMCMP( pwszDest, dstrSource.pwszString, cbSource ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    /* Test case passed, return success */
    dr = DRM_SUCCESS;

ErrorExit:
    FREEDRMSTRING( dstrSource );
    SAFE_OEM_FREE( pwszDest );
    return dr;
}

/*********************************************************
**
** Function: TestOEMGetFileSize
**
** Synopsis: This tests the function Oem_File_GetSize
** to ensure properly measure file sizes
**
** Arguments:
**  argv[0] -- File name
**  argv[1] -- Expected size in bytes
**
**
*********************************************************/
DRM_RESULT DRM_CALL TestOEMGetFileSize(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr                = DRM_E_TEST_INCOMPLETE;
    OEM_FILEHDL      hFile             = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD        cbFileExpected    = 0;
    DRM_DWORD        cbFileActual      = 0;
    DRM_CONST_STRING dstrFileName      = EMPTY_DRM_STRING;

    ChkArg( argc >= 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    cbFileExpected = strtoul( argv[1], NULL, 0 );

    /* Open the file */
    ChkDR( TST_UTL_MakePathString( NULL,
                                   argv[0],
                                  &dstrFileName) );

    hFile = Oem_File_Open( NULL,
                          dstrFileName.pwszString,
                          OEM_GENERIC_READ,
                          OEM_FILE_SHARE_READ,
                          OEM_OPEN_EXISTING,
                          OEM_ATTRIBUTE_NORMAL );
    ChkBOOL( OEM_INVALID_HANDLE_VALUE != hFile, DRM_E_FILENOTFOUND);

    Log("Trace", "\t\tFile opened successfully");

    /* Get file size */
    ChkBOOL( Oem_File_GetSize(hFile, &cbFileActual), DRM_E_FILE_SEEK_ERROR );

    /* Compare file size to expected size */
    ChkBOOL( cbFileActual == cbFileExpected, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Test case passed, return success */
    dr = DRM_SUCCESS;

ErrorExit:
    FREEDRMSTRING( dstrFileName );
    if( hFile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hFile );
    }
    return dr;
}

/*********************************************************
**
** Function: TestOEMBlockHeaderCache
**
** Synopsis: This tests the functions Oem_Hds_CheckBlockHeaderCache
** and Oem_Hds_UpdateBlockHeaderCache
**
** Arguments:
**  argv[0] -- Block number to test on (default = 1)
**  argv[1] -- Block header metadata to use (default = 1234)
**  argv[2] -- Block header data to use (default = "bh", this is converted to wchar string and then stored in a byte buffer)
**  argv[3] -- Number of bytes to write to cache (default = 4)
**  argv[4] -- Number of times to update the cache (default = 2)
**
** NOTE: If a valid HDS file handle is required because of multiple
**       HDS, the PreTestCase function should be modified to set
**       g_hHDSFileHandle to a valid HDS file handle.
**
*********************************************************/
DRM_RESULT DRM_CALL TestOEMBlockHeaderCache(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr                      = DRM_E_TEST_INCOMPLETE;
    DRM_DWORD        dwBlockNumber           = 1;
    DRM_DWORD        dwBlockHeaderMetadata   = 1234;
    DRM_DWORD        cbBytesToWrite          = 4;
    DRM_DWORD        dwNumUpdates            = 2;
    DRM_CONST_STRING dstrBlockHeaderData     = EMPTY_DRM_STRING;
    DRM_DWORD        i                       = 0;
    DRM_BYTE         *pbExpectedBlockHeader  = NULL;
    DRM_BYTE         *pbOutputBlockHeader    = NULL;
    DRM_DWORD        dwOutputMetadata        = 0;

    ChkBOOL( DRM_HBHC_IsHdsBlockHeaderCacheSupported(), DRM_E_NOTIMPL );

    if ( argc > 0 && argv[0] != NULL)
    {
        dwBlockNumber = strtoul( argv[0], NULL, 0 );
    }
    if ( argc > 1 && argv[1] != NULL)
    {
        dwBlockHeaderMetadata = strtoul( argv[1], NULL, 0 );
    }
    if ( argc > 2 )
    {
        if ( argv[2] != NULL )
        {
            ChkDR( MakeDRMString(&dstrBlockHeaderData, argv[2]) );
        }
    }
    else
    {
        ChkDR( MakeDRMString(&dstrBlockHeaderData, "bh") );
    }
    if ( argc > 3 && argv[3] != NULL)
    {
        cbBytesToWrite = strtoul( argv[3], NULL, 0 );
    }
    if ( argc > 4 && argv[4] != NULL)
    {
        dwNumUpdates = strtoul( argv[4], NULL, 0 );
    }

    /* Allocate memory for expected block header and output buffer */
    ChkMem( pbExpectedBlockHeader = (DRM_BYTE*)Oem_MemAlloc( CALC_MAXBLOCKHEADERLEN ) );
    ChkMem( pbOutputBlockHeader   = (DRM_BYTE*)Oem_MemAlloc( CALC_MAXBLOCKHEADERLEN ) );

    /* Clear cache */
    Oem_Hds_ClearBlockHeaderCache();

    /* Update Cache and Check */
    for ( i = 0; i < dwNumUpdates; i++ )
    {
        ChkDR( Oem_Hds_UpdateBlockHeaderCache( dwBlockNumber,
                                           g_hHDSFileHandle,
                                           dwBlockHeaderMetadata,
                                           (DRM_BYTE*)dstrBlockHeaderData.pwszString,
                                           (i + 1) * cbBytesToWrite / dwNumUpdates ) );
        Log("Trace", "\t\tBlock Header Cache: Block number %d updated.", dwBlockNumber);
        ChkDR( Oem_Hds_CheckBlockHeaderCache(  dwBlockNumber,
                                           g_hHDSFileHandle,
                                           &dwOutputMetadata,
                                           pbOutputBlockHeader,
                                           (i + 1) * cbBytesToWrite / dwNumUpdates ) );
        Log("Trace", "\t\tBlock Header Cache: Block number %d has been found in the cache.", dwBlockNumber);

        /* Verify that input metadata matches output metadata */
        ChkBOOL ( dwOutputMetadata == dwBlockHeaderMetadata, DRM_E_FAIL );

        /* Update expected header data */
        MEMCPY( pbExpectedBlockHeader,
                (DRM_BYTE*)dstrBlockHeaderData.pwszString,
                min( (i + 1) * cbBytesToWrite / dwNumUpdates, CALC_MAXBLOCKHEADERLEN ) );

        /*Verify that expecter header data matches output header data */
        ChkBOOL( MEMCMP( pbExpectedBlockHeader, pbOutputBlockHeader, min( (i + 1) * cbBytesToWrite / dwNumUpdates, CALC_MAXBLOCKHEADERLEN ) ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );

        Log("Trace", "\t\tBlock Header Cache: Block number %d cahe value verified.", dwBlockNumber);
    }

    /* Test passed, return success */
    dr = DRM_SUCCESS;

ErrorExit:
    FREEDRMSTRING( dstrBlockHeaderData );
    SAFE_OEM_FREE( pbExpectedBlockHeader );
    SAFE_OEM_FREE( pbOutputBlockHeader );
    return dr;
}

/*********************************************************
**
** Function: TestOEMClearBlockHeaderCache
**
** Synopsis: This tests the function Oem_Hds_ClearBlockHeaderCache
** to ensure properly clears the header cache
**
** Arguments: none
**
** NOTE: If a valid HDS file handle is required because of multiple
**       HDS, the PreTestCase function should be modified to set
**       g_hHDSFileHandle to a valid HDS file handle.
**
*********************************************************/
DRM_RESULT DRM_CALL TestOEMClearBlockHeaderCache(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr                = DRM_E_TEST_INCOMPLETE;

    ChkBOOL( DRM_HBHC_IsHdsBlockHeaderCacheSupported(), DRM_E_NOTIMPL );

    /* Cache metadata for block number 1 */
    ChkDR( Oem_Hds_UpdateBlockHeaderCache( 1, g_hHDSFileHandle, 1234, NULL, 0 ) );

    /* Clear the block cache */
    Oem_Hds_ClearBlockHeaderCache();

    /* Check that block number 1 is not in cache */
    ChkBOOL( Oem_Hds_CheckBlockHeaderCache( 1, g_hHDSFileHandle, NULL, NULL, 0 ) == DRM_E_DST_BLOCK_CACHE_MISS, DRM_E_TEST_UNEXPECTED_OUTPUT );

    /* Test case passed, return success */
    dr = DRM_SUCCESS;

ErrorExit:
    return dr;
}

/*********************************************************
**
** Function: TestOEMMemRealloc
**
** Synopsis: This test the function Oem_MemRealloc to verify that is retains the data in the original buffer
**
** Arguments:
**  argv[0] -- Original buffer size (decimal dword)
**  argv[1] -- Reallocated buffer size (decimal dword)
**
**  NOTE: If specifying a fixed time, either use all default values for arguments 1-8
**        or specify all values.
**        If using current or random time, arguments 1-8 are ignored
*********************************************************/
DRM_RESULT DRM_CALL TestOEMMemRealloc(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr                = DRM_E_TEST_INCOMPLETE;
    DRM_BYTE         *pbReallocBuffer  = NULL;
    DRM_BYTE         *pbControlBuffer  = NULL;
    DRM_DWORD        cbOriginal        = 0;
    DRM_DWORD        cbRealloced       = 0;
    DRM_DWORD        cch               = 0;

    ChkArg( argc >= 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
    ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &cbOriginal ) );
    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[1]), &cch ) );
    ChkDR( DRMCRT_AtoDWORD( argv[1], cch, 10, &cbRealloced ) );

    /* Allocate the control buffer and the buffer which is to be realloced */
    if ( cbOriginal > 0 )
    {
        ChkMem( pbReallocBuffer = (DRM_BYTE*)Oem_MemAlloc( cbOriginal ) );
        ChkMem( pbControlBuffer = (DRM_BYTE*)Oem_MemAlloc( cbOriginal ) );

        /* Fill both buffers with data */
        DRMCRT_memset( pbReallocBuffer, 'a', cbOriginal );
        DRMCRT_memset( pbControlBuffer, 'a', cbOriginal );

        /* Make sure both buffers match before reallocing */
        ChkBOOL( 0 == DRMCRT_memcmp( pbControlBuffer, pbReallocBuffer, cbOriginal ), DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    /* ReAlloc one buffer */
    if ( cbRealloced > 0 )
    {
        ChkMem( pbReallocBuffer = (DRM_BYTE*)Oem_MemRealloc( (DRM_VOID*)pbReallocBuffer, cbRealloced ) );

        if ( cbOriginal > 0 )
        {
            /* Compare realloced buffer with control buffer */
            ChkBOOL( 0 == DRMCRT_memcmp( pbControlBuffer, pbReallocBuffer, min( cbOriginal, cbRealloced ) ), DRM_E_TEST_UNEXPECTED_OUTPUT );
        }
    }
    else
    {
        pbReallocBuffer = (DRM_BYTE*)Oem_MemRealloc( (DRM_VOID*)pbReallocBuffer, cbRealloced );
        ChkBOOL( pbReallocBuffer == NULL , DRM_E_TEST_UNEXPECTED_OUTPUT );
    }

    /* At this point the test has passed */
    dr = DRM_SUCCESS;

ErrorExit:
    SAFE_OEM_FREE( pbControlBuffer );
    SAFE_OEM_FREE( pbReallocBuffer );
    return dr;
}

/*********************************************************
**
** Function: TestOEMDeviceVerifyNonFakeId
**
** Synopsis: Test the function Oem_DeviceGetUniqueId and verify that
**           the returned hardware ID is not hardcoded.
**           This test only makes sense for WinCE and WM7
**
** Arguments:
**           None needed
**
*********************************************************/
DRM_RESULT DRM_CALL TestOEMDeviceVerifyNonFakeId(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT  dr                  = DRM_SUCCESS;
    DRM_ID      idSerialNumber      = {0};
    DRM_DWORD   cbSerialNumber      = DRM_ID_SIZE;

    DRM_BYTE g_rgbHWID[DRM_ID_SIZE] = {0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A};

    /*
    ** Get the Serial Number stored in the hardware
    */
    ChkDR(Oem_Device_GetUniqueId( NULL,
                                 DRM_DIT_PLAYREADY_ID,
                                 (DRM_BYTE *)&idSerialNumber,
                                 &cbSerialNumber ));
    Log("Trace", "\t\tDevice Id length is %d", cbSerialNumber);

    ChkBOOL(MEMCMP(idSerialNumber.rgb, g_rgbHWID, cbSerialNumber) != 0, DRM_E_TEST_UNEXPECTED_OUTPUT);

ErrorExit:
    return dr;
}

/*********************************************************
**
** Function: GetOemKeyMapDataByType
**
** Synopsis: Look up the Hal Key Type data in g_rgOemKeyTypes
**           based on the Key Type Value.
**
** Arguments:
**           [f_eType]      - OEM_HAL_KEY_TYPE to look up
**           [f_ppMapEntry] - Output reference to the key map entry.
**
*********************************************************/
DRM_RESULT DRM_CALL GetOemKeyMapDataByType( __in OEM_HAL_KEY_TYPE f_eType, __out TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY **f_ppMapEntry )
{
    DRM_RESULT dr = DRM_E_NOMORE;
    DRM_DWORD  i  = 0;
    ChkArg( f_ppMapEntry != NULL );

    /*
    ** If this fails, you need to update the key map array with the correct types
    */
    DRMCASSERT( OEM_HAL_KEY_TYPE_NUM_VALID_TYPES == 18 );

    for( i = 0; i < NO_OF( g_rgOemKeyTypes ); i++ )
    {
        if( f_eType == g_rgOemKeyTypes[i].eType )
        {
            *f_ppMapEntry = &g_rgOemKeyTypes[i];
            dr = DRM_SUCCESS;
            break;
        }
    }

ErrorExit:
    return dr;
}

/*********************************************************
**
** Function: GetOemKeyMapDataByName
**
** Synopsis: Look up the Hal Key Type data in g_rgOemKeyTypes
**           based on the Key Type Name.
**
** Arguments:
**           [f_pszType]    - OEM_HAL_KEY_TYPE to look up
**           [f_ppMapEntry] - Output reference to the key map entry.
**
*********************************************************/
DRM_RESULT DRM_CALL GetOemKeyMapDataByName( __in const DRM_CHAR *f_pszType, __out TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY **f_ppMapEntry )
{
    DRM_RESULT dr = DRM_E_NOMORE;
    DRM_DWORD  i  = 0;
    ChkArg( f_ppMapEntry != NULL );
    ChkArg( f_pszType != NULL );

    /*
    ** If this fails, you need to update the key map array with the correct types
    */
    DRMCASSERT( OEM_HAL_KEY_TYPE_NUM_VALID_TYPES == 18 );

    for( i = 0; i < NO_OF( g_rgOemKeyTypes ); i++ )
    {
        if( DRMCRT_strncmp( f_pszType, g_rgOemKeyTypes[i].pszType, DRMCRT_strlen( f_pszType ) ) == 0 )
        {
            *f_ppMapEntry = &g_rgOemKeyTypes[i];
            dr = DRM_SUCCESS;
            break;
        }
    }

ErrorExit:
    return dr;
}

/*********************************************************
**
** Function: TestOEMHalAllocateKey
**
** Synopsis: Invoke Oem_Hal_AllocateRegister() with the
**           OEM_HAL_KEY_TYPE specified in argv[0].
**
** Arguments:
**  argv[0] -- OEM_HAL_KEY_TYPE to use.
**
*********************************************************/
DRM_RESULT DRM_CALL TestOEMHalAllocateKey(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT                       dr       = DRM_SUCCESS;
    TEST_OEM_HAL_KEY_TYPE_MAP_ENTRY *pKeyMap  = NULL;
    OEM_HAL_KEY_REGISTER_INDEX       keyIndex = OEM_HAL_KEY_REGISTER_INDEX_INVALID;

    ChkDR( GetOemKeyMapDataByName( argv[0], &pKeyMap ) );

    ChkDR( Oem_Hal_AllocateRegister( pKeyMap->eType, &keyIndex) );

ErrorExit:
    if( keyIndex != OEM_HAL_KEY_REGISTER_INDEX_INVALID && pKeyMap != NULL )
    {
        Oem_Hal_FreeRegister( pKeyMap->eType, keyIndex );
    }
    return dr;
}

PRAGMA_DBG_OPTIMIZATION_OFF;

#define LOG_TestOEM_SECURE_ARE_EQUAL_FAILURE( lhs, rhs, cb, rgch, cch, strwhich ) do {   \
    cch = SIZEOF(rgch);                                                               \
    Log( "Trace", "" );                                                               \
    Log( "Trace", "\t%s Compare Failure - LHS:", strwhich );                          \
    LogBlobB64( rgch, cch, lhs, cb );                                                 \
    Log( "Trace", "\t%s Compare Failure - RHS:", strwhich );                          \
    LogBlobB64( rgch, cch, rhs, cb );                                                 \
} while(FALSE)

DRM_RESULT DRM_CALL TestOEM_SECURE_ARE_EQUAL(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr                                           = DRM_SUCCESS;
    DRM_BYTE   rgbLHS[128]                                  = {0};
    DRM_BYTE   rgbRHS[SIZEOF(rgbLHS)]                       = {0};
    DRM_DWORD  rgbSizes[]                                   = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 23, 32, 64, 89, 100, 128 };
    DRM_DWORD  cSizes                                       = SIZEOF(rgbSizes) / SIZEOF(rgbSizes[0]);
    DRM_DWORD  iByte                                        = 0;
    DRM_DWORD  iSize                                        = 0;
    DRM_CHAR   rgchLog[CCH_BASE64_EQUIV(SIZEOF(rgbLHS))+1]  = {0};
    DRM_DWORD  cchLog                                       = 0;
    DRM_BOOL   fPassed                                      = TRUE;

    /*
    ** We are testing as follows.
    ** 1. For each byte (iByte) out of 128 bytes (size of rgbLHS)
    **    A. Randomly populate the LHS array (so we'll randomly populate it 128 times)
    **    B. Copy the LHS array to the RHS array (so they match).
    **    C. For each possible size in rgbSizes (rgbSizes[iSize])
    **       I. Confirm that the arrays are the same by comparing size (rgbSizes[iSize]) from C (above) bytes.      (i.e. we're effectively operating on an array of size rgbSizes[iSize])
    **       II. If the byte iByte from 1 (above) is smaller than the size (rgbSizes[iSize]) from C (above)
    **           Z. Corrupt the iByte'th byte of LHS by adding a random value between 1-255 to it
    **           ZZ. Verify the the arrays are not the same by comparing (rgbSizes[iSize]) from C (above) bytes.    (i.e. we're effectively operating on an array of size rgbSizes[iSize])
    **           ZZZ. Restore the LHS.
    **           ZZZZ. Repeat Z-ZZZ for RHS.
    ** What does this effectively cover?
    **  For each size in rgbSizes, create a random array of that size, make sure it compares as equal.
    **  Then, for each byte in that random array
    **        corrupt it in LHS (make sure it compares as not-equal) and restore from corruption
    **        corrupt it in RHS (make sure it compares as not-equal) and restore from corruption
    */

    for( iByte = 0; iByte < SIZEOF( rgbLHS ); iByte++ )
    {
        ChkDR( Oem_Random_GetBytes( NULL, rgbLHS, SIZEOF( rgbLHS ) ) );
        MEMCPY( rgbRHS, rgbLHS, SIZEOF( rgbLHS ) );
        for( iSize = 0; iSize < cSizes; iSize++ )
        {
            DRM_DWORD cbCompare = rgbSizes[iSize];
            DRMASSERT( iSize <= cbCompare );

            /* Verify that the two match when this many bytes are compared */
            if( !OEM_SECURE_ARE_EQUAL( rgbLHS, rgbRHS, cbCompare ) )
            {
                LOG_TestOEM_SECURE_ARE_EQUAL_FAILURE( rgbLHS, rgbRHS, cbCompare, rgchLog, cchLog, "First" );
                fPassed = FALSE;
            }

            if( iByte < cbCompare )
            {
                DRM_BYTE bAdd = 0;

                /* Generate a random value in the range 1-255 */
                ChkDR( Oem_Random_GetBytes( NULL, &bAdd, SIZEOF( bAdd ) ) );
                bAdd %= 254;
                bAdd++;

                /* Randomly corrupt the iByte'th value on the LHS, verify it no longer matches */
                rgbLHS[iByte] += bAdd; /* Overflow can occur and is desirable when it does */
                if( OEM_SECURE_ARE_EQUAL( rgbLHS, rgbRHS, cbCompare ) )
                {
                    LOG_TestOEM_SECURE_ARE_EQUAL_FAILURE( rgbLHS, rgbRHS, cbCompare, rgchLog, cchLog, "Second" );
                    fPassed = FALSE;
                }

                /* Restore the iByte'th value on the LHS */
                rgbLHS[iByte] -= bAdd; /* Underflow can occur and is desirable when it does */

                /* Randomly corrupt the iByte'th value on the RHS, verify it no longer matches */
                rgbRHS[iByte] += bAdd; /* Overflow can occur and is desirable when it does */
                if( OEM_SECURE_ARE_EQUAL( rgbLHS, rgbRHS, cbCompare ) )
                {
                    LOG_TestOEM_SECURE_ARE_EQUAL_FAILURE( rgbLHS, rgbRHS, cbCompare, rgchLog, cchLog, "Third" );
                    fPassed = FALSE;
                }

                /* Restore the iByte'th value on the RHS */
                rgbRHS[iByte] -= bAdd; /* Underflow can occur and is desirable when it does */
            }
        }
    }

    ChkBOOL( fPassed, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    return dr;
}

PRAGMA_DBG_OPTIMIZATION_ON;


DRM_RESULT DRM_CALL MODULE_PRETESTCASE(OEM)(long lTCID, const char *strTCName)
{
    DRM_InitAllStrings();
/* If HDS handle is required by OEM block header caching functions, set g_hHDSFileHandle here */

    Oem_Hal_Initialize();

    return DRM_SUCCESS;
}


DRM_RESULT DRM_CALL MODULE_POSTTESTCASE(OEM)(long lTCID, const char *strTCName)
{
    Oem_Hal_Deinitialize();
    return DRM_SUCCESS;
}



BEGIN_TEST_API_LIST( OEM )
    API_ENTRY(TestOEMSystemTimeFileTimeConversion)
    API_ENTRY(TestOEM_SECURE_ZERO_MEMORY)
    API_ENTRY(TestOEMStringCchCopyN)
    API_ENTRY(TestOEMGetFileSize)
    API_ENTRY(TestOEMMemRealloc)
    API_ENTRY(TestOEMDeviceVerifyNonFakeId)
    API_ENTRY(TestOEMBlockHeaderCache)
    API_ENTRY(TestOEMClearBlockHeaderCache)
    API_ENTRY(TestOEMHalAllocateKey)
    API_ENTRY(TestOEM_SECURE_ARE_EQUAL)
END_TEST_API_LIST

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP; /* __WARNING_USE_WIDE_API */

