/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdafx.h>
#include <drmperformance.h>
#include <PerformanceTestAPI.h>
#include <drmmathsafe.h>

USING_PKTEST_NAMESPACE;

ENTER_PKTEST_NAMESPACE_CODE;


/******************************************************************************
**
** Function:  TestPerfAllocateLogBuffer
**
** Synopsis:  Allocates the circular memory buffer for performance recording.
**
** Arguments: [argc]   : Number of arguments
**            [argv 0] : How many entries the circular buffer should contain
**
** Returns:   DRM_RESULT
**
******************************************************************************/
DRM_RESULT DRM_CALL TestPerfAllocateLogBuffer(long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT  dr                = DRM_SUCCESS;
    DRM_DWORD   dwNumberOfEntries = 0;
    DRM_DWORD   cch               = 0;

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
    ChkDR( DRMCRT_AtoDWORD( argv[0], cch, 10, &dwNumberOfEntries ) );
    ChkDR( DRM_PERF_AllocateLogBuffer( dwNumberOfEntries ) );

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function:  TestPerfFreeLogBuffer
**
** Synopsis:  Frees the circular memory buffer for performance recording.
**
** Arguments: [argc] : Number of arguments
**            [argv] : Ignored
**
** Returns:   DRM_SUCCESS
**
******************************************************************************/
DRM_RESULT DRM_CALL TestPerfFreeLogBuffer(long argc, __in_ecount( argc ) char **argv )
{
    DRM_PERF_FreeLogBuffer();
    return DRM_SUCCESS;
}

/******************************************************************************
**
** Function:  TestPerfDumpLogBuffer
**
** Synopsis:  Dumps the circular memory buffer into the specified file.
**
** Arguments: [argc]   : Number of arguments
**            [argv 0] : The full path to the file in which to dump the buffer
**
** Returns:   DRM_RESULT
**
******************************************************************************/
DRM_RESULT DRM_CALL TestPerfDumpLogBuffer(long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr           = DRM_SUCCESS;

    ChkArg( argc >= 1 );
    ChkArg( argv[0] != NULL );

    ChkDR( DRM_PERF_DumpLogBuffer( argv[0] ) );

#if DRM_BUILD_PROFILE==DRM_BUILD_PROFILE_XBOX

    ChkDR( TST_UTL_CopyFileFromDRMDirectoryA( argv[0] ) );

#endif

ErrorExit:

    return dr;
}


/******************************************************************************
**
** Function:  TestPerfOn
**
** Synopsis:  Turn on performance recording.
**
** Arguments: [argc] : Number of arguments
**            [argv] : Ignored
**
** Returns:   DRM_RESULT:
**                DRM_SUCCESS,           if everything is OK
**                DRM_E_BUFFERTOOSMALL,  if the memory buffer has not previously been allocated
**
******************************************************************************/
DRM_RESULT DRM_CALL TestPerfOn(long argc, __in_ecount( argc ) char **argv )
{
    return( DRM_PERF_On() );
}


/******************************************************************************
**
** Function:  TestPerfOff
**
** Synopsis:  Turn off performance recording.
**
** Arguments: [argc] : Number of arguments
**            [argv] : Ignored
**
** Returns:   DRM_SUCCESS
**
******************************************************************************/
DRM_RESULT DRM_CALL TestPerfOff(long argc, __in_ecount( argc ) char **argv )
{
    DRM_PERF_Off();
    return DRM_SUCCESS;
}


/******************************************************************************
**
** Function:  TestPerfLogSeparator
**
** Synopsis:  Record a separator in the log file, useful for delineating loop iterations.
**
** Arguments: [argc] : Number of arguments
**            [argv] : Ignored
**
** Returns:   DRM_RESULT:
**                DRM_SUCCESS,           if everything is OK
**                DRM_E_FAIL,            if perf collection is not turned on.
**                DRM_E_BUFFERTOOSMALL,  if the memory buffer has not previously been allocated
**
******************************************************************************/
DRM_RESULT DRM_CALL TestPerfLogSeparator(long argc, __in_ecount( argc ) char **argv )
{
    return DRM_PERF_LogSeparator();
}

extern DRM_RESULT DRM_CALL Test_DRM_AES_Cipher(long argc, __in_ecount( argc ) char **argv );


IMPLEMENT_DEFAULT_PRETESTCASE_FOR_MODULE(Performance);

DRM_RESULT DRM_CALL MODULE_POSTTESTCASE( Performance )(
    __in long lTCID,
    __in const char *strTCName )
{
    DRM_PERF_Off();
    DRM_PERF_FreeLogBuffer();
    return DRM_SUCCESS;
}


BEGIN_TEST_API_LIST( Performance )
    API_ENTRY( TestPerfAllocateLogBuffer )
    API_ENTRY( TestPerfFreeLogBuffer )
    API_ENTRY( TestPerfDumpLogBuffer )
    API_ENTRY( TestPerfOn )
    API_ENTRY( TestPerfOff )
    API_ENTRY( TestPerfLogSeparator )
    API_ENTRY( Test_DRM_AES_Cipher )
END_TEST_API_LIST


EXIT_PKTEST_NAMESPACE_CODE

