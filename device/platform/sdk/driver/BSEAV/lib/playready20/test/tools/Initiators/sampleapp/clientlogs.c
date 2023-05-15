/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <drmwindowsenv.h>
#include <clientlogs.h>
#include <stdio.h>
#include <stdarg.h>

#define LOG_PACKETS 1

#define MAX_DRM_STRING_MSG_SIZE        1024

const DRM_WCHAR  g_rgwchPacketFile   [] = { ONE_WCHAR('c', '\0'), ONE_WCHAR(':', '\0'), ONE_WCHAR('\\', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\\', '\0'), ONE_WCHAR('P', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('c', '\0'), ONE_WCHAR('k', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('L', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('g', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('x', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\0', '\0') }; /* "c:\\InitiatorContent\\PacketLog.txt" */
const DRM_CHAR  g_rgchPacketSeparator       [] = "\r\n\r\n\r\n";
const DRM_CHAR  g_rgchChallenge             [] = "Challenge \r\n";
const DRM_CHAR  g_rgchResponse              [] = "Response \r\n";

/*********************************************************************
**
** Function: LogMessage
**
** Synopsis: Function that writes information to a standard output.
**
** Arguments:
**
** [f_pszMessage]           -- Pointer to a format string.
**
** Returns:                 N/A
**
**********************************************************************/
DRM_API DRM_VOID DRM_CALL LogMessage(
    IN DRM_CHAR *f_pszMessage, ... )
{
    va_list va;

    va_start( va, f_pszMessage );

    vprintf( f_pszMessage, va );

    va_end( va );

/*  printf( "\n" ); */
}



DRM_API DRM_RESULT DRM_CALL LogDrmString(
    DRM_CONST_STRING *f_dstrMsg )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WCHAR rgwchMsg[ MAX_DRM_STRING_MSG_SIZE + 1 ] = { 0 };

    ChkArg( f_dstrMsg != NULL );

    DRM_BYT_CopyBytes(rgwchMsg, 0, f_dstrMsg->pwszString, 0, min( MAX_DRM_STRING_MSG_SIZE, f_dstrMsg->cchString ) * SIZEOF( DRM_WCHAR ));

    printf( "%S", rgwchMsg );
    
ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: LogPacket
**
** Synopsis: Function that logs a packet into a file. If the file exists,
**           its content will be overwritten.
**
** Arguments:
**
** [f_fChallenge]           -- Flag indicating whether the packet is a
**                             challenge or response packet.
** [f_pbPacket]             -- Pointer to a buffer that contains the packet.
** [f_cbPacket]             -- Size of the packet.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL if file I/O error happens.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL LogPacket(
    IN DRM_BOOL f_fChallenge,
    IN DRM_BYTE *f_pbPacket,
    IN DRM_DWORD f_cbPacket )
{
    DRM_RESULT dr = DRM_SUCCESS;
    
#if LOG_PACKETS
    /* Packet counts used as part of the file name of the packet logs. */
    static DRM_DWORD s_dwChallengePacketCount = 1;
    static DRM_DWORD s_dwResponsePacketCount = 1;
    static DRM_BOOL  s_fAppend = FALSE;
    
    OEM_FILEHDL fp = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD cbWritten = 0;
    
    ChkArg( f_pbPacket != NULL && f_cbPacket > 0 );
    
    fp = Oem_File_Open( NULL,
                        ( const DRM_WCHAR * ) g_rgwchPacketFile,
                        OEM_GENERIC_WRITE,
                        OEM_FILE_SHARE_NONE,
                        s_fAppend ? OEM_OPEN_EXISTING : OEM_CREATE_ALWAYS,
                        OEM_ATTRIBUTE_NORMAL );

    if ( fp == OEM_INVALID_HANDLE_VALUE )
    {
        LogMessage( "LogPacket: Failed to write file %S.",
                    g_rgwchPacketFile );

        ChkDR( DRM_E_FAIL );
    }

    if ( s_fAppend )
    {
        /*
        ** Move to the end of the file and write a couple of new lines
        */
        ChkBOOL( Oem_File_SetFilePointer( fp, 0, OEM_FILE_END, NULL ), DRM_E_FAIL );
        ChkBOOL( Oem_File_Write( fp, ( DRM_BYTE * ) g_rgchPacketSeparator, SIZEOF(g_rgchPacketSeparator), &cbWritten ), DRM_E_FAIL );
    }

    if ( f_fChallenge )
    {
        ChkBOOL( Oem_File_Write( fp, ( DRM_BYTE * ) g_rgchChallenge, SIZEOF(g_rgchChallenge), &cbWritten ), DRM_E_FAIL );
    }
    else
    {
        ChkBOOL( Oem_File_Write( fp, ( DRM_BYTE * ) g_rgchResponse, SIZEOF(g_rgchResponse), &cbWritten ), DRM_E_FAIL );
    }


    if ( !Oem_File_Write( fp, f_pbPacket, f_cbPacket, &cbWritten ) ||
         cbWritten != f_cbPacket )
    {
        ChkDR( DRM_E_FAIL );
    }
    /*
    ** set the static flag so that the next call appends to the file.
    */
    if ( !s_fAppend )
    {
        s_fAppend = TRUE;
    }
ErrorExit:
    
    if ( fp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( fp );
    }
    
#endif  /* LOG_PACKETS */
    
    return dr;
}

