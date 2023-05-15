/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#include <drmcommon.h>
#include <drmcmdlnpars.h>
#include <drmtoolsconstants.h>
#include <drmtrace.h>
#include <oemtraceimpl.h>

/* Maximum number of characters of a line to be written out. */
#define MAX_LINE_BUFFER_SIZE 2048

DRM_DWORD g_dwScopeLevel = 0;

DRM_CHAR g_rgchLineBuffer[ MAX_LINE_BUFFER_SIZE + 1 ] = { 0 };

/*********************************************************************
**
** Function: ShowMenu
**
** Synopsis: Function that shows how the application is to be used.
**
** Arguments:               N/A
**
** Returns:                 N/A
**
**********************************************************************/
DRM_VOID DRM_CALL ShowMenu( DRM_VOID )
{
    printf( "Usage:\nTraceTool.exe -i:<trace log file name> -o:<output file name>\n" );
}

#if DRM_SUPPORT_TRACING

/*********************************************************************
**
** Function: IsBlankLine
**
** Synopsis: Function that checks whether the passed in string is made up
**           of blank characters only. A blank character is a character
**           that is treated as a space by isspace() C functions.
**
** Arguments:
**
** [f_szData]               -- Pointer to the NULL terminated string to be
**                             checked.
**
** Returns:                 TRUE if the string is made up of blank characters
**                          only. FALSE otherwise.
**
**********************************************************************/
DRM_RESULT DRM_CALL IsBlankLine(
    __in_z DRM_CHAR *f_pszData )
{
    DRM_BOOL fResult = TRUE;
    DRM_CHAR *ptr = f_pszData;

    if ( f_pszData != NULL )
    {
        while ( *ptr != 0 )
        {
            if ( !isspace( ( DRM_BYTE )*ptr ) )
            {
                fResult = FALSE;

                break;
            }

            ptr++;
        }
    }

    return fResult;
}

/*********************************************************************
**
** Function: PrintLine
**
** Synopsis: Function that writes a single line to the output file and the
**           standard output.
**
** Arguments:
**
** [f_Outfp]                -- Opened file handle of the output file.
** [f_szData]               -- Pointer to the NULL terminated string to be
**                             written out.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FILEWRITEERROR if file write error occurs.
**
**********************************************************************/
DRM_RESULT DRM_CALL PrintLine(
    __in OEM_FILEHDL f_Outfp,
    __in_z DRM_CHAR *f_pszData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbWritten = 0;

    ChkArg( f_Outfp != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_pszData != NULL );

    printf( "%s", f_pszData );

    ChkBOOL( Oem_File_Write( f_Outfp,
                             f_pszData,
                             DRMCRT_strlen( f_pszData ),
                             &cbWritten ), DRM_E_FILEWRITEERROR );

    ChkBOOL( cbWritten == DRMCRT_strlen( f_pszData ), DRM_E_FILEWRITEERROR );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: PrintMessage
**
** Synopsis: Function that writes information to the output file and the
**           standard output.
**
** Arguments:
**
** [f_Outfp]                -- Opened file handle of the output file.
** [f_ptmTraceTime]         -- Pointer to the time associated with the trace
**                             information.
** [f_pszMessage]           -- Pointer to a format string.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL PrintMessage(
    __in OEM_FILEHDL f_Outfp,
    __in DRMSYSTEMTIME *f_ptmTraceTime,
    __in DRM_CHAR *f_pszFormat,
    ...)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i = 0;
    va_list va;

    ChkArg( f_Outfp != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_ptmTraceTime != NULL );

    for ( i = 0; i < g_dwScopeLevel; i ++ )
    {
        sprintf_s( g_rgchLineBuffer, MAX_LINE_BUFFER_SIZE, "---" );

        ChkDR( PrintLine( f_Outfp, g_rgchLineBuffer ) );
    }

    va_start( va, f_pszFormat );

    vsprintf_s( g_rgchLineBuffer, MAX_LINE_BUFFER_SIZE, f_pszFormat, va );

    ChkDR( PrintLine( f_Outfp, g_rgchLineBuffer ) );

    va_end( va );
    
    sprintf_s( g_rgchLineBuffer,
               MAX_LINE_BUFFER_SIZE,
               " (%hu-%02hu-%02hu %02hu:%02hu:%02hu)",
               f_ptmTraceTime->wYear, 
               f_ptmTraceTime->wMonth, 
               f_ptmTraceTime->wDay, 
               f_ptmTraceTime->wHour, 
               f_ptmTraceTime->wMinute, 
               f_ptmTraceTime->wSecond );

    ChkDR( PrintLine( f_Outfp, g_rgchLineBuffer ) );

    sprintf_s( g_rgchLineBuffer, MAX_LINE_BUFFER_SIZE, "\r\n" );

    ChkDR( PrintLine( f_Outfp, g_rgchLineBuffer ) );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: FindScopeNameByID
**
** Synopsis: Function that looks up the string name associated with a
**           scope ID using the global lookup table g_Scopes.
**
** Arguments:
**
** [f_dwScopeID]            -- Scope ID to be looked up.
**
** Returns:                 Pointer to the string name of the scope ID if
**                          found. Otherwise returns NULL.
**
**********************************************************************/
DRM_CHAR * DRM_CALL FindScopeNameByID(
    __in DRM_DWORD f_dwScopeID )
{
    DRM_DWORD i = 0;

    for ( i = 0; g_Scopes[ i ].m_pszScopeName != NULL; i++ )
    {
        if ( g_Scopes[ i ].m_dwScopeID == f_dwScopeID )
        {
            return g_Scopes[ i ].m_pszScopeName;
        }
    }

    return NULL;
}

/*********************************************************************
**
** Function: DumpStringPacket
**
** Synopsis: Function that displays the content of a string packet.
**
** Arguments:
**
** [f_Infp]                 -- Opened file handle of the input trace
**                             log file.
** [f_Outfp]                -- Opened file handle of the output file.
** [f_poHeader]             -- Pointer to the header object that has been
**                             read by the caller function.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL DumpStringPacket(
    __in OEM_FILEHDL f_Infp,
    __in OEM_FILEHDL f_Outfp,
    __in TRACE_HEADER *f_poHeader )
{
    DRM_RESULT dr = DRM_SUCCESS;
    TRACE_STRING_DATA oData = { 0 };
    DRM_DWORD cbRead = 0;

    ChkArg( f_Infp != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_Outfp != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_poHeader != NULL );

    ChkBOOL( Oem_File_Read( f_Infp,
                            &oData.m_cchData,
                            SIZEOF( DRM_DWORD ),
                            &cbRead ), DRM_E_FILEREADERROR );

    DRMASSERT( cbRead == SIZEOF( DRM_DWORD ) );

    ChkBOOL( oData.m_cchData <= TRACE_MAX_STRING_BUFFER,
             DRM_E_FILEREADERROR );

    ChkBOOL( Oem_File_Read( f_Infp,
                            &oData.m_rgchData,
                            oData.m_cchData,
                            &cbRead), DRM_E_FILEREADERROR );

    DRMASSERT( cbRead == oData.m_cchData );

    if ( !IsBlankLine( oData.m_rgchData ) )
    {
        ChkDR( PrintMessage( f_Outfp,
                             &f_poHeader->m_tmTraceTime,
                             "%s",
                             oData.m_rgchData ) );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DumpStringIDPacket
**
** Synopsis: Function that displays the content of a string ID packet.
**
** Arguments:
**
** [f_Infp]                 -- Opened file handle of the input trace
**                             log file.
** [f_Outfp]                -- Opened file handle of the output file.
** [f_poHeader]             -- Pointer to the header object that has been
**                             read by the caller function.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL DumpStringIDPacket(
    __in OEM_FILEHDL f_Infp,
    __in OEM_FILEHDL f_Outfp,
    __in TRACE_HEADER *f_poHeader )
{
    DRM_RESULT dr = DRM_SUCCESS;
    TRACE_STRING_ID_DATA oData = { 0 };
    DRM_DWORD cbRead = 0;
    DRM_DWORD i = 0;

    ChkArg( f_Infp != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_Outfp != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_poHeader != NULL );

    ChkBOOL( Oem_File_Read( f_Infp,
                            &oData,
                            SIZEOF( TRACE_STRING_ID_DATA ),
                            &cbRead ), DRM_E_FILEREADERROR );

    PrintMessage( f_Outfp,
                  &f_poHeader->m_tmTraceTime,
                  "String ID %d ( status code %08X ) with %d paramaters:",
                  oData.m_dwFormatID,
                  oData.m_dr,
                  oData.m_cParams );

    for ( i = 0; i < oData.m_cParams; i++ )
    {
        PrintMessage( f_Outfp,
                      &f_poHeader->m_tmTraceTime,
                      " Paramater %d: %08X",
                      i + 1,
                      oData.m_dwParams[ i ] );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DumpEventPacket
**
** Synopsis: Function that displays the content of an event packet.
**
** Arguments:
**
** [f_Infp]                 -- Opened file handle of the input trace
**                             log file.
** [f_Outfp]                -- Opened file handle of the output file.
** [f_poHeader]             -- Pointer to the header object that has been
**                             read by the caller function.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL DumpEventPacket(
    __in OEM_FILEHDL f_Infp,
    __in OEM_FILEHDL f_Outfp,
    __in TRACE_HEADER *f_poHeader )
{
    DRM_RESULT dr = DRM_SUCCESS;
    TRACE_EVENT_DATA oData = { 0 };
    DRM_DWORD cbRead = 0;

    ChkArg( f_Infp != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_Outfp != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_poHeader != NULL );

    ChkBOOL( Oem_File_Read( f_Infp,
                            &oData,
                            SIZEOF( TRACE_EVENT_DATA ),
                            &cbRead ), DRM_E_FILEREADERROR );
    
    DRMASSERT( cbRead == SIZEOF( TRACE_EVENT_DATA ) );

    switch ( oData.m_dwEventID )
    {
        case TRACE_ENTER_SCOPE:
                                g_dwScopeLevel++;

                                PrintMessage( f_Outfp,
                                              &f_poHeader->m_tmTraceTime,
                                              "Entering scope: %s.",
                                              FindScopeNameByID( f_poHeader-> m_dwScopeID ) );
                                break;

        case TRACE_LEAVE_SCOPE:
                                PrintMessage( f_Outfp,
                                              &f_poHeader->m_tmTraceTime,
                                              "Leaving scope: %s with status %08X.",
                                              FindScopeNameByID( f_poHeader-> m_dwScopeID ),
                                              oData.m_dr );

                                g_dwScopeLevel--;
                                break;

        default:                
                                PrintMessage( f_Outfp,
                                              &f_poHeader->m_tmTraceTime,
                                              "Event ID: %X with status %08X.",
                                              oData.m_dwEventID,
                                              oData.m_dr );
                                break;
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DumpTraceLog
**
** Synopsis: Function that displays the content of the trace log file.
**
** Arguments:
**
** [f_pdstrTraceLogFile]    -- Pointer to a DRM string that contains
**                             the name of the trace log file.
** [f_pdstrOutputFile]     -- Pointer to a DRM string that contains
**                             the name of the output file.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FILEOPEN if either input or output file
**                          cannot be opened.
**                          DRM_E_FAIL if unexpected state occurrs.
**
**********************************************************************/
DRM_RESULT DRM_CALL DumpTraceLog(
    IN DRM_CONST_STRING *f_pdstrTraceLogFile,
    IN DRM_CONST_STRING *f_pdstrOutputFile )
{
    DRM_RESULT dr = DRM_SUCCESS;
    OEM_FILEHDL Infp = OEM_INVALID_HANDLE_VALUE;
    OEM_FILEHDL Outfp = OEM_INVALID_HANDLE_VALUE;
    TRACE_HEADER oHeader;
    DRM_DWORD cbRead = 0;

    ChkDRMString( f_pdstrTraceLogFile );
    ChkDRMString( f_pdstrOutputFile );

    Infp = Oem_File_Open( NULL,
                          f_pdstrTraceLogFile->pwszString,
                          OEM_GENERIC_READ,
                          OEM_FILE_SHARE_READ,
                          OEM_OPEN_EXISTING,
                          OEM_ATTRIBUTE_NORMAL );

    if ( Infp == OEM_INVALID_HANDLE_VALUE )
    {
        printf( "Failed to open file %S for read.", f_pdstrTraceLogFile->pwszString );

        ChkDR( DRM_E_FILEOPEN );
    }

    Outfp = Oem_File_Open( NULL,
                          f_pdstrOutputFile->pwszString,
                          OEM_GENERIC_WRITE,
                          OEM_FILE_SHARE_NONE,
                          OEM_CREATE_ALWAYS,
                          OEM_ATTRIBUTE_NORMAL );

    if ( Outfp == OEM_INVALID_HANDLE_VALUE )
    {
        printf( "Failed to open file %S for write.", f_pdstrOutputFile->pwszString );

        ChkDR( DRM_E_FILEOPEN );
    }

    for ( ; ; )
    {
        ZEROMEM( &oHeader, SIZEOF( TRACE_HEADER ) );

        ChkBOOL( Oem_File_Read( Infp,
                                &oHeader,
                                SIZEOF( TRACE_HEADER ),
                                &cbRead ), DRM_E_FILEREADERROR );

        if ( cbRead < SIZEOF( TRACE_HEADER ) )
        {
            /* End of the file has been reached. */
            break;
        }

        switch ( oHeader.m_eType )
        {
            case TRACE_STRING_PACKET:   ChkDR( DumpStringPacket( Infp, Outfp, &oHeader ) );
                                        break;

            case TRACE_STRING_ID_PACKET:
                                        ChkDR( DumpStringIDPacket( Infp, Outfp, &oHeader ) );
                                        break;

            case TRACE_EVENT_PACKET:
                                        ChkDR( DumpEventPacket( Infp, Outfp, &oHeader ) );
                                        break;

            default:                    printf( " Unknow packet type %02X.\n", oHeader.m_eType );
                                        ChkDR( DRM_E_FAIL );
                                        break;
        }
    }

ErrorExit:

    if ( Outfp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( Outfp );
    }

    if ( Infp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( Infp );
    }

    return dr;
}

#endif  /* DRM_SUPPORT_TRACING */

/*********************************************************************
**
** Function: DRM_Main
**
** Synopsis: The main function.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- An array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
DRM_LONG DRM_CALL DRM_Main(
    IN DRM_LONG argc,
    IN DRM_WCHAR *argv[] )
{
    DRM_RESULT dr = DRM_SUCCESS;

#if DRM_SUPPORT_TRACING

    DRM_LONG i = 0;
    DRM_WCHAR wchOption = 0;
    DRM_STRING dstrParam = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrTraceLogFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrOutputFile = EMPTY_DRM_STRING;

    if ( argc < 3 )
    {
        goto ShowMenu;
    }

    for ( i = 1; i < argc; i++ )
    {
        if ( !DRM_CMD_ParseCmdLine( argv[ i ],
                                    &wchOption,
                                    ( DRM_CONST_STRING * )&dstrParam,
                                    NULL ) )
        {
            goto ShowMenu;
        }

        wchOption = DRMCRT_towlower( wchOption );

        switch ( wchOption )
        {
            case WCHAR_CAST( 'i' ):
                dstrTraceLogFile.pwszString = dstrParam.pwszString;
                dstrTraceLogFile.cchString  = dstrParam.cchString;
                break;

            case WCHAR_CAST( 'o' ):
                dstrOutputFile.pwszString = dstrParam.pwszString;
                dstrOutputFile.cchString  = dstrParam.cchString;
                break;

            default:
                    goto ShowMenu;
                    break;
        }
    }

    ChkDR( DumpTraceLog( &dstrTraceLogFile, &dstrOutputFile ) );

#else
    
    printf( "\nFailure: TraceTool is compiled with DRM_SUPPORT_TRACING flag turned off.\n" );
    
    ChkDR( DRM_E_NOTIMPL );
    
#endif  /* DRM_SUPPORT_TRACING */

ErrorExit:

    return dr;

#if DRM_SUPPORT_TRACING

ShowMenu:

    ShowMenu();

    return dr; 

#endif  /* DRM_SUPPORT_TRACING */
}
