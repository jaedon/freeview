/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>

#ifdef WINCE_TEST

#include <drmerr.h>
#include <drmcmdlnpars.h>

#ifdef __cplusplus
extern "C" {
#endif

DRM_LONG DRM_CALL DRM_Main( __in DRM_LONG argc, __in_ecount(argc) DRM_WCHAR *argv[] )
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);
    return DRM_E_NOTIMPL;
}

#ifdef __cplusplus
}
#endif

#else   /* WINCE_TEST */

#include <windows.h>

#include <drmerr.h>
#include <oemcommon.h>
#include <drmcmdlnpars.h>
#include <tOEMIMP.h>
#include <PKTestNameSpace.h>

HANDLE  g_hMap      = NULL;
BYTE   *g_pbMap     = NULL;
HANDLE  g_hEvent1   = NULL;
HANDLE  g_hEvent2   = NULL;

ENTER_PKTEST_NAMESPACE_CODE;

extern DRM_RESULT TST_DRM_AsyncCmdProc_ProcessMessage(
    __in                           DRM_DWORD    f_dwMessageType,
    __in_ecount(f_cbMessage) const DRM_BYTE    *f_pbMessage,
    __in                           DRM_DWORD    f_cbMessage );

DRM_RESULT TST_DRM_AsyncCmdProc_WriteMessage(
    __in                           DRM_DWORD    f_dwMessageType,
    __in_ecount(f_cbMessage) const DRM_BYTE    *f_pbMessage,
    __in                           DRM_DWORD    f_cbMessage )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRM_DWORD   cbDataWrite = 0;

    /* Write message size */
    cbDataWrite = f_cbMessage + SIZEOF(DRM_DWORD);
    MEMCPY( g_pbMap,                       &cbDataWrite,     SIZEOF(DRM_DWORD) );

    /* Write message type */
    MEMCPY( g_pbMap + SIZEOF(DRM_DWORD),   &f_dwMessageType, SIZEOF(DRM_DWORD) );
    if( f_cbMessage > 0 )
    {
        /* Write message data */
        MEMCPY( g_pbMap + 2*SIZEOF(DRM_DWORD), f_pbMessage, f_cbMessage );
    }

    /* Signal process that we have written data to shared memory */
    if( !SetEvent( g_hEvent1 ) )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        ChkDR( dr );
        ChkDR( E_UNEXPECTED );
    }

ErrorExit:
    return dr;
}

static DRM_RESULT _TST_DRM_Main_Worker()
{
    DRM_RESULT        dr                = DRM_SUCCESS;
    DRM_BOOL          fRunning          = TRUE;
    DRM_DWORD         cbData            = 0;
    DRM_BYTE         *pbDataAlloc       = NULL;
    DRM_DWORD         cbDataAlloc       = 0;
    DRM_DWORD         dwMessageType     = 0;
    const DRM_BYTE   *pbMessage         = NULL;
    const DRM_WCHAR   rgwchMapName[]    = DRMTEST_AsyncShellCommand_Map_Name;
    const DRM_WCHAR   rgwchEvent1Name[] = DRMTEST_AsyncShellCommand_Event1_Name;
    const DRM_WCHAR   rgwchEvent2Name[] = DRMTEST_AsyncShellCommand_Event2_Name;

    /* Initialize events */
    g_hEvent1 = CreateEventW( NULL, TRUE, FALSE, rgwchEvent1Name );
    if( g_hEvent1 == INVALID_HANDLE_VALUE )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        ChkDR( dr );
        ChkDR( E_UNEXPECTED );
    }
    g_hEvent2 = CreateEventW( NULL, TRUE, FALSE, rgwchEvent2Name );
    if( g_hEvent2 == INVALID_HANDLE_VALUE )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        ChkDR( dr );
        ChkDR( E_UNEXPECTED );
    }

    /* Initialize shared memory */
    g_hMap = OpenFileMappingW(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        rgwchMapName );
    if( g_hMap == NULL )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        ChkDR( dr );
        ChkDR( E_UNEXPECTED );
    }
    g_pbMap = (BYTE*) MapViewOfFile(
        g_hMap,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        DRMTEST_AsyncShellCommand_Map_Size );
    if( g_pbMap == NULL )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        ChkDR( dr );
        ChkDR( E_UNEXPECTED );
    }

    /* Signal process that we are initialized */
    if( !SetEvent( g_hEvent1 ) )
    {
        dr = HRESULT_FROM_WIN32( GetLastError() );
        ChkDR( dr );
        ChkDR( E_UNEXPECTED );
    }

    printf( "Process Ready!\n" );

    while( fRunning )
    {
        DRM_DWORD dwWaitResult = 0;

        /* Wait for process to signal us that it has written data to shared memory */
        dwWaitResult = WaitForSingleObject( g_hEvent2, ASYNC_SHELL_COMMAND_DEFAULT_PING_TIME * ASYNC_SHELL_COMMAND_WAIT_PINGS );
        ChkBOOL( dwWaitResult == WAIT_OBJECT_0, HRESULT_FROM_WIN32( ERROR_TIMEOUT ) );

        /* Read message size */
        MEMCPY( &cbData, g_pbMap, SIZEOF(DRM_DWORD) );

        ChkBOOL( cbData >= SIZEOF(DRM_DWORD), DRM_E_FILE_READ_ERROR );

        /* Read message type */
        MEMCPY( &dwMessageType, g_pbMap + SIZEOF(DRM_DWORD), SIZEOF(DRM_DWORD) );

        cbData -= SIZEOF(DRM_DWORD);

        if( cbData > 0 )
        {
            /* Reuse already allocated space if large enough */
            if( cbData > cbDataAlloc )
            {
                /* Otherwise allocate larger space */
                SAFE_OEM_FREE( pbDataAlloc );
                cbDataAlloc = 0;
                ChkMem( pbDataAlloc = (DRM_BYTE*)Oem_MemAlloc( cbData ) );
                cbDataAlloc = cbData;
            }

            /* Read message data */
            MEMCPY( pbDataAlloc, g_pbMap + 2*SIZEOF(DRM_DWORD), cbData );
            pbMessage = pbDataAlloc;
        }
        else
        {
            pbMessage = NULL;
            cbData    = 0;
        }

        switch( dwMessageType )
        {
        case ASYNC_SHELL_COMMAND_MESSAGE_TYPE_HALT_PROCESS:
            ChkDR( TST_DRM_AsyncCmdProc_ProcessMessage( 0, pbMessage, cbData ) );
            fRunning = FALSE;
            break;
        default:
            ChkDR( TST_DRM_AsyncCmdProc_ProcessMessage( dwMessageType, pbMessage, cbData ) );
            break;
        }
    }

ErrorExit:
    SAFE_OEM_FREE( pbDataAlloc );
    if( g_pbMap != NULL )
    {
        UnmapViewOfFile( g_pbMap );
    }
    if( g_hMap != NULL )
    {
        CloseHandle( g_hMap );
    }
    if( g_hEvent1 != NULL )
    {
        CloseHandle( g_hEvent1 );
    }
    if( g_hEvent2 != NULL )
    {
        CloseHandle( g_hEvent2 );
    }

    printf( "_TST_DRM_Main_Worker returns 0x%x\n", dr );
    fflush(stdout);
    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE;

USING_PKTEST_NAMESPACE;

#ifdef __cplusplus
extern "C" {
#endif

DRM_LONG DRM_CALL DRM_Main( __in DRM_LONG argc, __in_ecount(argc) DRM_WCHAR *argv[] )
{
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);
    return _TST_DRM_Main_Worker();
}

#ifdef __cplusplus
}
#endif

#endif   /* WINCE_TEST */
