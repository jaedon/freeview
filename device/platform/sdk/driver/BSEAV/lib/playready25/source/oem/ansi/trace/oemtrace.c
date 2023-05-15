/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_OEMTRACE_C
#include <stdio.h>
#include <stdarg.h>

#include <drmerr.h>
#include <drmbytemanip.h>
#include <drmtrace.h>
#include <oem.h>
#include <drmlastinclude.h>

#if DRM_SUPPORT_TRACING

ENTER_PK_NAMESPACE_CODE;

/* Default trace log file name: L"trace.dat" */
static const DRM_WCHAR g_rgwchTraceLog[] = { ONE_WCHAR( 't', '\0' ), ONE_WCHAR( 'r', '\0' ), ONE_WCHAR( 'a', '\0' ), ONE_WCHAR( 'c', '\0' ),
                                             ONE_WCHAR( 'e', '\0' ), ONE_WCHAR( '.', '\0' ), ONE_WCHAR( 'd', '\0' ), ONE_WCHAR( 'a', '\0' ),
                                             ONE_WCHAR( 't', '\0' ), ONE_WCHAR( '\0', '\0' ) };

/* Global trace context */
static DRM_TRACE_GLOBAL_CONTEXT g_oTraceContext =
{
    /* Current global trace level. */
    TRACE_FATAL,

    /* Default component trace levels. */
    { TRACE_NONE },

    /*
    ** Init flag to indicate whether the
    ** data structure has been initialized.
    */
    FALSE,

    /*
    ** OEM data field to allow OEM to store its
    ** specific information (such as a trace log
    ** file pointer).
    */
    NULL,

    /*
    ** Flag indicating whether is currently in
    ** Oem_Trace_Enter/LeaveScope() API. This flag
    ** is used to prevent recursive entering
    ** of Oem_Trace_Enter/LeaveScope().
    */
    FALSE,

    /*
    ** Flag indicating whether is currently in
    ** Oem_Trace_Start API. This flag
    ** is used to prevent recursive entering
    ** of Oem_Trace_Start.
    */
    FALSE,

    /* Bit mask of trace channels. */
    TRACE_CONSOLE_CHANNEL
};

#if !TARGET_LITTLE_ENDIAN

/*********************************************************************
**
** Function: _TraceTimeToLittleEndian
**
** Synopsis: Converts the TraceTime datastructure members to little endian.
**
**
** Arguments:               Pointer to time data.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT _TraceTimeToLittleEndian( __inout DRMSYSTEMTIME *f_time )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_time != NULL );

    f_time->wYear         = FLIP_WORD_BYTES( f_time->wYear );
    f_time->wMonth        = FLIP_WORD_BYTES( f_time->wMonth );
    f_time->wDayOfWeek    = FLIP_WORD_BYTES( f_time->wDayOfWeek );
    f_time->wDay          = FLIP_WORD_BYTES( f_time->wDay );
    f_time->wHour         = FLIP_WORD_BYTES( f_time->wHour );
    f_time->wMinute       = FLIP_WORD_BYTES( f_time->wMinute );
    f_time->wSecond       = FLIP_WORD_BYTES( f_time->wSecond );
    f_time->wMilliseconds = FLIP_WORD_BYTES( f_time->wMilliseconds );

ErrorExit:
    return dr;
}

#endif /*!TARGET_LITTLE_ENDIAN*/

/*********************************************************************
**
** Function: Oem_Trace_GetGlobalContext
**
** Synopsis: API that returns a pointer to the global trace context
**           data structure.
**
** Arguments:               N/A
**
** Returns:                 Pointer to the global trace context.
**
**********************************************************************/
DRM_API DRM_TRACE_GLOBAL_CONTEXT * DRM_CALL Oem_Trace_GetGlobalContext( DRM_VOID )
{
    return &g_oTraceContext;
}

/*********************************************************************
**
** Function: Oem_Trace_Info
**
** Synopsis: API that writes the result string defined by the input format
**           string and varied number of parameters into the trace log.
**
** Arguments:
**
** [f_pszFormat]            -- Pointer to format string that can use varied
**                             number of parameters.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FILExxxERROR when various file I/O errors occur.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Trace_Info(
    __in DRM_CHAR *f_pszFormat,
    ... )
{
    DRM_RESULT dr = DRM_SUCCESS;
    va_list oList;
    TRACE_PACKET oPacket;
    DRM_DWORD cbWritten = 0;
    DRM_DWORD cchData   = 0;
    DRM_TRACE_GLOBAL_CONTEXT *poTraceContext = Oem_Trace_GetGlobalContext();
    OEM_FILEHDL fp = OEM_INVALID_HANDLE_VALUE;

    ChkArg( poTraceContext != NULL );

    if ( !poTraceContext->m_fInit )
    {
        ChkDR( Oem_Trace_Start( NULL, NULL ) );
    }

    fp = poTraceContext->m_pvOEMData;
    ChkDR( poTraceContext->m_fInit &&
           fp != NULL &&
           fp != OEM_INVALID_HANDLE_VALUE );

    ZEROMEM( &oPacket, SIZEOF( TRACE_PACKET ) );

    Oem_Clock_GetSystemTime(NULL, &oPacket.m_oHeader.m_tmTraceTime );

    va_start( oList, f_pszFormat );

    vsprintf_s( oPacket.m_oBody.m_oString.m_rgchData,
                TRACE_MAX_STRING_BUFFER,
                f_pszFormat,
                oList );
    cchData = DRMCRT_strlen( oPacket.m_oBody.m_oString.m_rgchData );

#if TARGET_LITTLE_ENDIAN
    oPacket.m_oHeader.m_eType = TRACE_STRING_PACKET;
    oPacket.m_oBody.m_oString.m_cchData = cchData;
#else
    oPacket.m_oHeader.m_eType = (eTracePacketType) FLIP_DWORD_BYTES( TRACE_STRING_PACKET );
    oPacket.m_oBody.m_oString.m_cchData = FLIP_DWORD_BYTES( cchData );
    ChkDR( _TraceTimeToLittleEndian( &oPacket.m_oHeader.m_tmTraceTime ) );
#endif

    if ( poTraceContext->m_dwChannelMask & TRACE_CONSOLE_CHANNEL )
    {
        printf( "%s\n", oPacket.m_oBody.m_oString.m_rgchData );
    }

    va_end( oList );

    if ( poTraceContext->m_dwChannelMask & TRACE_FILE_CHANNEL )
    {
        /* Write the packet header and the actual number of characters. */
        ChkBOOL( Oem_File_Write( fp,
                                 &oPacket,
                                 SIZEOF( TRACE_HEADER ) +
                                 SIZEOF( DRM_DWORD ) +
                                 cchData,
                                 &cbWritten ), DRM_E_FAIL );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: Oem_Trace_ID
**
** Synopsis: API that writes the result string defined by the ID of a
**           format string and varied number of parameters into the trace
**           log. Currently maximum 3 paramaters can be used, and the
**           parameeters can only be of DWORD type.
**
** Arguments:
**
** [f_dwFormatID]           -- DWORD value of the ID of the format string.
** [f_dr]                   -- Status code associated with the format string.
** [f_cParams]              -- Total number of effective parameters. Must
**                             be within the range of [0,3].
** [f_dwParam1]             -- First optional DWORD parameter.
** [f_dwParam1]             -- Second optional DWORD parameter.
** [f_dwParam1]             -- Third optional DWORD parameter.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FILExxxERROR when various file I/O errors occur.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Trace_ID(
    __in DRM_DWORD f_dwFormatID,
    __in DRM_RESULT f_dr,
    __in DRM_DWORD f_cParams,
    __in DRM_DWORD f_dwParam1,
    __in DRM_DWORD f_dwParam2,
    __in DRM_DWORD f_dwParam3 )
{
    DRM_RESULT dr = DRM_SUCCESS;
    TRACE_PACKET oPacket;
    DRM_DWORD cbWritten = 0;
    DRM_TRACE_GLOBAL_CONTEXT *poTraceContext = Oem_Trace_GetGlobalContext();
    OEM_FILEHDL fp = OEM_INVALID_HANDLE_VALUE;

    if ( f_dwFormatID == INVALID_TRACE_ID )
    {
        goto ErrorExit;
    }

    ChkArg( poTraceContext != NULL );

    if ( !poTraceContext->m_fInit )
    {
        ChkDR( Oem_Trace_Start( NULL, NULL ) );
    }

    fp = poTraceContext->m_pvOEMData;
    ChkDR( poTraceContext->m_fInit &&
           fp != NULL &&
           fp != OEM_INVALID_HANDLE_VALUE );

    ZEROMEM( &oPacket, SIZEOF( TRACE_PACKET ) );

    Oem_Clock_GetSystemTime(NULL, &oPacket.m_oHeader.m_tmTraceTime );

#if TARGET_LITTLE_ENDIAN
    oPacket.m_oHeader.m_eType = TRACE_STRING_ID_PACKET;
    oPacket.m_oBody.m_oStringID.m_dwFormatID = f_dwFormatID;
    oPacket.m_oBody.m_oStringID.m_dr = f_dr;
    oPacket.m_oBody.m_oStringID.m_cParams = f_cParams;
    oPacket.m_oBody.m_oStringID.m_dwParams[ 0 ] = f_dwParam1;
    oPacket.m_oBody.m_oStringID.m_dwParams[ 1 ] = f_dwParam2;
    oPacket.m_oBody.m_oStringID.m_dwParams[ 2 ] = f_dwParam3;
#else
    oPacket.m_oHeader.m_eType = (eTracePacketType) FLIP_DWORD_BYTES( TRACE_STRING_ID_PACKET );
    oPacket.m_oBody.m_oStringID.m_dwFormatID = FLIP_DWORD_BYTES( f_dwFormatID );
    oPacket.m_oBody.m_oStringID.m_dr = FLIP_DWORD_BYTES( f_dr );
    oPacket.m_oBody.m_oStringID.m_cParams = FLIP_DWORD_BYTES( f_cParams );
    oPacket.m_oBody.m_oStringID.m_dwParams[ 0 ] = FLIP_DWORD_BYTES( f_dwParam1 );
    oPacket.m_oBody.m_oStringID.m_dwParams[ 1 ] = FLIP_DWORD_BYTES( f_dwParam2 );
    oPacket.m_oBody.m_oStringID.m_dwParams[ 2 ] = FLIP_DWORD_BYTES( f_dwParam3 );
    ChkDR( _TraceTimeToLittleEndian( &oPacket.m_oHeader.m_tmTraceTime ) );
#endif

    if ( poTraceContext->m_dwChannelMask & TRACE_FILE_CHANNEL )
    {
        ChkBOOL( Oem_File_Write( fp,
                                 &oPacket,
                                 SIZEOF( TRACE_HEADER ) +  SIZEOF( TRACE_STRING_ID_DATA ),
                                 &cbWritten ), DRM_E_FAIL );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: Oem_Trace_Enter_Scope
**
** Synopsis: API that writes the event of entering a scope into the trace
**           log.
**
** Arguments:
**
** [f_dwScopeID]            -- DWORD value of the scope ID of the scope being
**                             entered.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FILExxxERROR when various file I/O errors occur.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Trace_Enter_Scope(
    __in DRM_DWORD f_dwScopeID )
{
    DRM_RESULT dr = DRM_SUCCESS;
    TRACE_PACKET oPacket;
    DRM_DWORD cbWritten = 0;
    DRM_TRACE_GLOBAL_CONTEXT *poTraceContext = Oem_Trace_GetGlobalContext();
    OEM_FILEHDL fp = OEM_INVALID_HANDLE_VALUE;

    ChkArg( poTraceContext != NULL );

    /*
    ** If the caller is already in the middle of entering/leaving a scope,
    ** just exit the function to prevent stack overflow.
    */
    if ( poTraceContext-> m_fEnterScope )
    {
        return dr;
    }

    poTraceContext-> m_fEnterScope = TRUE;

    /*
    ** Scope entry/leave tracing is only enabled when
    ** the global tracing level is set equal to or above
    ** TRACE_INFO.
    */
    if ( poTraceContext->m_eTraceLevel < TRACE_INFO )
    {
        goto ErrorExit;
    }

    if ( !poTraceContext->m_fInit )
    {
        ChkDR( Oem_Trace_Start( NULL, NULL ) );
    }

    fp = poTraceContext->m_pvOEMData;
    ChkDR( poTraceContext->m_fInit &&
           fp != NULL &&
           fp != OEM_INVALID_HANDLE_VALUE );

    ZEROMEM( &oPacket, SIZEOF( TRACE_PACKET ) );

    Oem_Clock_GetSystemTime(NULL, &oPacket.m_oHeader.m_tmTraceTime );

#if TARGET_LITTLE_ENDIAN
    oPacket.m_oHeader.m_eType = TRACE_EVENT_PACKET;
    oPacket.m_oHeader.m_dwScopeID = f_dwScopeID;
    oPacket.m_oBody.m_oEvent.m_dwEventID = TRACE_ENTER_SCOPE;
#else
    oPacket.m_oHeader.m_eType = (eTracePacketType) FLIP_DWORD_BYTES( TRACE_EVENT_PACKET );
    oPacket.m_oHeader.m_dwScopeID = FLIP_DWORD_BYTES( f_dwScopeID );
    oPacket.m_oBody.m_oEvent.m_dwEventID = FLIP_DWORD_BYTES( TRACE_ENTER_SCOPE );
    ChkDR( _TraceTimeToLittleEndian( &oPacket.m_oHeader.m_tmTraceTime ) );
#endif

    if ( poTraceContext->m_dwChannelMask & TRACE_FILE_CHANNEL )
    {
        ChkBOOL( Oem_File_Write( fp,
                                 &oPacket,
                                 SIZEOF( TRACE_HEADER ) +  SIZEOF( TRACE_EVENT_DATA ),
                                 &cbWritten ), DRM_E_FAIL );
    }

ErrorExit:

    if ( poTraceContext != NULL )
    {
        poTraceContext-> m_fEnterScope = FALSE;
    }

    return dr;
}

/*********************************************************************
**
** Function: Oem_Trace_Leave_Scope
**
** Synopsis: API that writes the event of leaving a scope into the trace
**           log.
**
** Arguments:
**
** [f_dwScopeID]            -- DWORD value of the scope ID of the scope being
**                             left.
** [f_dr]                   -- Status code when the scope is being left.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FILExxxERROR when various file I/O errors occur.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Trace_Leave_Scope(
    __in DRM_DWORD f_dwScopeID,
    __in DRM_RESULT f_dr )
{
    DRM_RESULT dr = DRM_SUCCESS;
    TRACE_PACKET oPacket;
    DRM_DWORD cbWritten = 0;
    DRM_TRACE_GLOBAL_CONTEXT *poTraceContext = Oem_Trace_GetGlobalContext();
    OEM_FILEHDL fp = OEM_INVALID_HANDLE_VALUE;

    ChkArg( poTraceContext != NULL );

    /*
    ** If the caller is already in the middle of entering/leaving a scope,
    ** just exit the function to prevent stack overflow.
    */
    if ( poTraceContext-> m_fEnterScope )
    {
        return dr;
    }

    poTraceContext-> m_fEnterScope = TRUE;

    /*
    ** Scope entry/leave tracing is only enabled when
    ** the global tracing level is set equal to or above
    ** TRACE_INFO.
    */
    if ( poTraceContext->m_eTraceLevel < TRACE_INFO )
    {
        goto ErrorExit;
    }

    if ( !poTraceContext->m_fInit )
    {
        ChkDR( Oem_Trace_Start( NULL, NULL ) );
    }

    fp = poTraceContext->m_pvOEMData;
    ChkDR( poTraceContext->m_fInit &&
           fp != NULL &&
           fp != OEM_INVALID_HANDLE_VALUE );

    ZEROMEM( &oPacket, SIZEOF( TRACE_PACKET ) );

    Oem_Clock_GetSystemTime(NULL, &oPacket.m_oHeader.m_tmTraceTime );

#if TARGET_LITTLE_ENDIAN
    oPacket.m_oHeader.m_eType = TRACE_EVENT_PACKET;
    oPacket.m_oHeader.m_dwScopeID = f_dwScopeID;
    oPacket.m_oBody.m_oEvent.m_dwEventID = TRACE_LEAVE_SCOPE;
#else
    oPacket.m_oHeader.m_eType = (eTracePacketType) FLIP_DWORD_BYTES( TRACE_EVENT_PACKET );
    oPacket.m_oHeader.m_dwScopeID = FLIP_DWORD_BYTES( f_dwScopeID );
    oPacket.m_oBody.m_oEvent.m_dwEventID = FLIP_DWORD_BYTES( TRACE_LEAVE_SCOPE );
    ChkDR( _TraceTimeToLittleEndian( &oPacket.m_oHeader.m_tmTraceTime ) );
#endif

    if ( poTraceContext->m_dwChannelMask & TRACE_FILE_CHANNEL )
    {
        ChkBOOL( Oem_File_Write( fp,
                                 &oPacket,
                                 SIZEOF( TRACE_HEADER ) +  SIZEOF( TRACE_EVENT_DATA ),
                                 &cbWritten ), DRM_E_FAIL );
    }

ErrorExit:

    if ( poTraceContext != NULL )
    {
        poTraceContext-> m_fEnterScope = FALSE;
    }

    return dr;
}

/*********************************************************************
**
** Function: Oem_Trace_Status_Code
**
** Synopsis: API that writes the event of reporting a status code into
**           the trace log.
**
** Arguments:
**
** [f_dwEventID]            -- DWORD value of the event ID associated with
**                             the reported stats code.
** [f_dr]                   -- Status code to be reported.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FILExxxERROR when various file I/O errors occur.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Trace_Status_Code(
    __in DRM_DWORD f_dwEventID,
    __in DRM_RESULT f_dr )
{
    DRM_RESULT dr = DRM_SUCCESS;
    TRACE_PACKET oPacket;
    DRM_DWORD cbWritten = 0;
    DRM_TRACE_GLOBAL_CONTEXT *poTraceContext = Oem_Trace_GetGlobalContext();
    OEM_FILEHDL fp = OEM_INVALID_HANDLE_VALUE;

    ChkArg( poTraceContext != NULL );

    if ( !poTraceContext->m_fInit )
    {
        ChkDR( Oem_Trace_Start( NULL, NULL ) );
    }

    fp = poTraceContext->m_pvOEMData;
    ChkDR( poTraceContext->m_fInit &&
           fp != NULL &&
           fp != OEM_INVALID_HANDLE_VALUE );

    ZEROMEM( &oPacket, SIZEOF( TRACE_PACKET ) );

    Oem_Clock_GetSystemTime(NULL, &oPacket.m_oHeader.m_tmTraceTime );

#if TARGET_LITTLE_ENDIAN
    oPacket.m_oHeader.m_eType = TRACE_EVENT_PACKET;
    oPacket.m_oBody.m_oEvent.m_dwEventID = f_dwEventID;
#else
    oPacket.m_oHeader.m_eType = (eTracePacketType) FLIP_DWORD_BYTES( TRACE_EVENT_PACKET );
    oPacket.m_oBody.m_oEvent.m_dwEventID = FLIP_DWORD_BYTES( f_dwEventID );
    ChkDR( _TraceTimeToLittleEndian( &oPacket.m_oHeader.m_tmTraceTime ) );
#endif



    if ( poTraceContext->m_dwChannelMask & TRACE_FILE_CHANNEL )
    {
        ChkBOOL( Oem_File_Write( fp,
                                 &oPacket,
                                 SIZEOF( TRACE_HEADER ) +  SIZEOF( TRACE_EVENT_DATA ),
                                 &cbWritten ), DRM_E_FAIL );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: Oem_Trace_Start
**
** Synopsis: Function that initializes the global trace provider context.
**           This function must be called before all other trace APIs are
**           used.
**
** Arguments:
**
** [f_poOEMContext]         -- Pointer to an OEM provided context that can be NULL.
** [f_pdstrTraceLog]        -- Pointer to a DRM string that contains the file name
**                             of the trace log. If it is NULL, the default trace log
**                             file name defined in g_rgwchTraceLog will be used.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FILExxxERROR when various file I/O error occurs.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Trace_Start(
    __in_opt DRM_VOID *f_poOEMContext,
    __in_opt DRM_CONST_STRING *f_pdstrTraceLog )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_TRACE_GLOBAL_CONTEXT *poTraceContext = Oem_Trace_GetGlobalContext();
    OEM_FILEHDL fp = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD i = 0;

    ChkArg( poTraceContext != NULL );

    if ( poTraceContext->m_fInit )
    {
        goto ErrorExit;
    }

    if( poTraceContext->m_fStartTrace )
    {
        goto ErrorExit;
    }

    poTraceContext->m_fStartTrace = TRUE;

    /* Try to open the existing trace log file in append mode. */
    fp = Oem_File_Open( f_poOEMContext,
                        ( NULL == f_pdstrTraceLog ? g_rgwchTraceLog : f_pdstrTraceLog->pwszString ),
                        OEM_GENERIC_WRITE,
                        OEM_FILE_SHARE_NONE,
                        ( NULL == f_pdstrTraceLog ? OEM_OPEN_ALWAYS : OEM_CREATE_ALWAYS ),
                        OEM_ATTRIBUTE_NORMAL );

    if ( fp == OEM_INVALID_HANDLE_VALUE )
    {
        /* If there is no existing trace log file, create a new one. */
        if ( f_pdstrTraceLog == NULL )
        {
            fp = Oem_File_Open( f_poOEMContext,
                                ( NULL == f_pdstrTraceLog ? g_rgwchTraceLog : f_pdstrTraceLog->pwszString ),
                                OEM_GENERIC_WRITE,
                                OEM_FILE_SHARE_NONE,
                                OEM_CREATE_NEW,
                                OEM_ATTRIBUTE_NORMAL );
        }

        ChkBOOL( fp != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN );
    }
    else
    {
        if ( !Oem_File_SetFilePointer( fp, 0, OEM_FILE_END, NULL ) )
        {
            ChkDR( DRM_E_FILE_SEEK_ERROR );
        }
    }

    ZEROMEM( poTraceContext, SIZEOF( DRM_TRACE_GLOBAL_CONTEXT ) );

    /* Initialize all default trace levels to TRACE_NONE. */
    poTraceContext->m_eTraceLevel = TRACE_NONE;

    for ( i = 0; i < MAX_TRACE_COMPONENT; i++ )
    {
        poTraceContext->m_rgeComponentTraceLevels[ i ] = TRACE_NONE;
    }

    /* Store the trace log file pointer. */
    poTraceContext->m_pvOEMData = fp;

    poTraceContext->m_dwChannelMask = TRACE_CONSOLE_CHANNEL;

    poTraceContext->m_fInit = TRUE;

    poTraceContext->m_fStartTrace = FALSE;

ErrorExit:
    if ( DRM_FAILED( dr ) && poTraceContext != NULL )
    {
        poTraceContext->m_fStartTrace = FALSE;
    }

    return dr;
}

/*********************************************************************
**
** Function: Oem_Trace_Stop
**
** Synopsis: Function that deinitializes the global trace provider context.
**           No trace APIs should be used after this function is called.
**
** Arguments:               N/A
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FILExxxERROR when various file I/O errors occur.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Oem_Trace_Stop( DRM_VOID )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_TRACE_GLOBAL_CONTEXT *poTraceContext = Oem_Trace_GetGlobalContext();

    ChkArg( poTraceContext != NULL );

    if ( !poTraceContext->m_fInit )
    {
        goto ErrorExit;
    }

    ChkArg( poTraceContext->m_pvOEMData != NULL &&
            poTraceContext->m_pvOEMData != OEM_INVALID_HANDLE_VALUE );

    Oem_File_Close( ( OEM_FILEHDL )poTraceContext->m_pvOEMData );

    poTraceContext->m_pvOEMData = NULL;

    poTraceContext->m_fInit = FALSE;

ErrorExit:

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

#endif  /* DRM_SUPPORT_TRACING */
