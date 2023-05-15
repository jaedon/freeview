/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmperformance.h>
#include <drmutilitieslite.h>
#include <oemcommon.h>
#include <drmint64.h>

ENTER_PK_NAMESPACE_CODE;

static DRM_CHAR *s_rgpszDRM_PERF_LogFunctionString[] = {
    "Drm_Initialize",
    "Drm_Reader_Bind",
    "Drm_Reader_Commit",
    "Drm_Reader_InitDecrypt",
    "Drm_Reader_Decrypt",
    "Drm_StoreMgmt_CleanupStore",
};

#define DRM_PERF_MAX_SCOPE_ID_LENGTH    31

typedef struct
{
    eDRM_PERF_LogTag       eLogTag;
    DRM_CHAR               rgcScopeID[ DRM_PERF_MAX_SCOPE_ID_LENGTH + 1 ];
    DRM_DWORD              dwMicrosecondCount;
} DRM_PERF_LogEntry;

static DRM_PERF_LogEntry     *s_poDRM_PERF_MemoryBuffer          = NULL;

static DRM_DWORD              s_dwDRM_PERF_MaxBufferEntries      = 0;

static DRM_DWORD              s_dwDRM_PERF_CurrentIndex          = 0;

static DRM_BOOL               s_fDRM_PERF_BufferWrappedAround    = FALSE;

static DRM_BOOL               s_fDRM_PERF_CollectData            = FALSE;

static DRM_CHAR              *s_rgpszDRM_PERF_LogTagString[]     = { "Enter", "Exit ", "-----" };

static const DRM_UINT64       s_qwDRM_PERF_MicrosecondsPerSecond = DRM_UI64LITERAL( 0, 1000000 );


/******************************************************************************
**
** Function:  _writeDWORDtoFile
**
** Synopsis:  Helper function to write the specified DWORD into the specified file.
**            Arguments have been checked prior to calling this function.
**
** Arguments: [f_dwValue] : The DWORD to write
**            [f_hFile]   : The handle to the file to write to
**
** Returns:   DRM_RESULT:
**                DRM_SUCCESS,           if everything is OK
**                DRM_E_FILEWRITEERROR,  if we cannot succefully write to the file
**
******************************************************************************/
static DRM_RESULT _writeDWORDtoFile(
    __in const DRM_DWORD     f_dwValue,
    __in const OEM_FILEHDL   f_hFile )
{
    DRM_RESULT    dr                                                   = DRM_SUCCESS;
    DRM_CHAR      rgcDWordASCII[ DRM_MAX_CCH_BASE10_DWORD_STRING + 1 ] = {0};
    DRM_DWORD     cchDWordASCII                                        = 0;
    DRM_DWORD     cbWritten                                            = 0;

    ChkDR( DRM_UTL_NumberToStringA( f_dwValue, rgcDWordASCII, SIZEOF( rgcDWordASCII ), 0, 10, &cchDWordASCII ) );
    ChkBOOL( Oem_File_Write( f_hFile, rgcDWordASCII, cchDWordASCII, &cbWritten ), DRM_E_FILEWRITEERROR );
    ChkBOOL( cbWritten == cchDWordASCII, DRM_E_FILEWRITEERROR );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function:  _writeStringtoFile
**
** Synopsis:  Helper function to write the specified string into the specified file.
**            Arguments have been checked prior to calling this function.
**
** Arguments: [f_pszValue] : The string to write
**            [f_hFile]    : The handle to the file to write to
**
** Returns:   DRM_RESULT:
**                DRM_SUCCESS,           if everything is OK
**                DRM_E_FILEWRITEERROR,  if we cannot succefully write to the file
**
******************************************************************************/
static DRM_RESULT _writeStringtoFile(
    __in       DRM_CHAR     *f_pszValue,
    __in const OEM_FILEHDL   f_hFile )
{
    DRM_RESULT    dr         = DRM_SUCCESS;
    DRM_DWORD     cbWritten  = 0;

    ChkBOOL( Oem_File_Write( f_hFile, f_pszValue, DRMCRT_strlen( f_pszValue ), &cbWritten ), DRM_E_FILEWRITEERROR );
    ChkBOOL( cbWritten == DRMCRT_strlen( f_pszValue ), DRM_E_FILEWRITEERROR );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function:  DRM_PERF_AllocateLogBuffer
**
** Synopsis:  Allocates the circular memory buffer for performance recording.
**
** Arguments: [f_dwNumberOfEntries] : How many entries the circular buffer should contain
**
** Returns:   DRM_RESULT:
**                DRM_SUCCESS,       if everything is OK
**                DRM_E_INVALIDARG,  if the paramater is not greater than 0
**                DRM_E_FAIL,        if the memory buffer has already been allocated
**                DRM_E_OUTOFMEMORY, if the memory allocation fails
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_PERF_AllocateLogBuffer(
    __in const DRM_DWORD      f_dwNumberOfEntries )
{
    DRM_RESULT    dr = DRM_SUCCESS;

    ChkArg( f_dwNumberOfEntries > 0 );
    ChkBOOL( s_poDRM_PERF_MemoryBuffer == NULL, DRM_E_FAIL );

    ChkMem( s_poDRM_PERF_MemoryBuffer = ( DRM_PERF_LogEntry* )Oem_MemAlloc( f_dwNumberOfEntries * SIZEOF( DRM_PERF_LogEntry ) ) );

    MEMSET( ( DRM_BYTE * )s_poDRM_PERF_MemoryBuffer, 0, f_dwNumberOfEntries * SIZEOF( DRM_PERF_LogEntry ) );

    s_dwDRM_PERF_MaxBufferEntries = f_dwNumberOfEntries;
    s_dwDRM_PERF_CurrentIndex = 0;
    s_fDRM_PERF_BufferWrappedAround = FALSE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function:  DRM_PERF_FreeLogBuffer
**
** Synopsis:  Frees the circular memory buffer for performance recording.
**
** Arguments: none
**
** Returns:   void
**
******************************************************************************/
DRM_API DRM_VOID DRM_CALL DRM_PERF_FreeLogBuffer()
{
    SAFE_OEM_FREE( s_poDRM_PERF_MemoryBuffer );
    s_fDRM_PERF_CollectData = FALSE;
}

/******************************************************************************
**
** Function:  DRM_PERF_DumpLogBuffer
**
** Synopsis:  Dumps the circular memory buffer into the specified file.  The location of the file
**            will be the directory in which the HDS is located.
**
** Arguments: [f_pszFilename] : The name the file in which to dump the buffer.
**
** Returns:   DRM_RESULT:
**                DRM_SUCCESS,           if everything is OK
**                DRM_E_INVALIDARG,      if the file path string is NULL
**                DRM_E_BUFFERTOOSMALL,  if the memory buffer has not previously been allocated
**                DRM_E_FILEOPEN,        if we cannot open the parameter for writing
**                DRM_E_FILEWRITEERROR,  if we cannot succefully write to the file
**                DRM_E_OUTOFMEMORY,     if any memory allocation fails
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_PERF_DumpLogBuffer(
    __in_z const DRM_CHAR     *f_pszFilename )
{
    DRM_RESULT      dr            = DRM_SUCCESS;
    OEM_FILEHDL     hFile         = OEM_INVALID_HANDLE_VALUE;
    DRM_STRING      dstrFileName  = EMPTY_DRM_STRING;
    DRM_SUBSTRING   dsstrTemp     = EMPTY_DRM_SUBSTRING;
    DRM_WCHAR      *pwszFullPath  = NULL;
    DRM_DWORD       dwIndex       = 0;

    ChkArg( f_pszFilename != NULL );
    ChkBOOL( s_poDRM_PERF_MemoryBuffer != NULL, DRM_E_BUFFERTOOSMALL );

    dstrFileName.cchString  = ( DRM_DWORD )DRMCRT_strlen( f_pszFilename ) + 1;
    ChkMem( dstrFileName.pwszString = (DRM_WCHAR *)Oem_MemAlloc( dstrFileName.cchString * SIZEOF( DRM_WCHAR ) ) );
    MEMSET( dstrFileName.pwszString, 0, ( dstrFileName.cchString * SIZEOF( DRM_WCHAR ) ) );
    dsstrTemp.m_cch = dstrFileName.cchString - 1;
    dsstrTemp.m_ich = 0;
    DRM_UTL_PromoteASCIItoUNICODE( f_pszFilename, &dsstrTemp, &dstrFileName );

    ChkDR( _GetDRMFullPathName( &pwszFullPath, (DRM_CONST_STRING *)&dstrFileName, HDSPATH ) );   /* Allocates memory */

    hFile = Oem_File_Open( NULL, pwszFullPath, OEM_GENERIC_WRITE, OEM_FILE_SHARE_NONE, OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_NORMAL );
    ChkBOOL( hFile != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN );

    if ( s_fDRM_PERF_BufferWrappedAround )
    {
        dwIndex = s_dwDRM_PERF_CurrentIndex;
    }

    do
    {
        ChkDR( _writeDWORDtoFile( dwIndex, hFile ) );
        ChkDR( _writeStringtoFile( ", ", hFile ) );

        if ( s_poDRM_PERF_MemoryBuffer[ dwIndex ].dwMicrosecondCount == 0 )
        {
            ChkDR( _writeStringtoFile( "empty - the end \r\n", hFile ) );
            break;
        }

        ChkDR( _writeStringtoFile( s_rgpszDRM_PERF_LogTagString[ s_poDRM_PERF_MemoryBuffer[ dwIndex ].eLogTag ], hFile ) );

        if ( s_poDRM_PERF_MemoryBuffer[ dwIndex ].eLogTag == eDRM_PERF_LogTag_Separator )
        {
            ChkDR( _writeStringtoFile( " \r\n", hFile ) );
        }
        else
        {
            ChkDR( _writeStringtoFile( ", ", hFile ) );
            ChkDR( _writeDWORDtoFile( s_poDRM_PERF_MemoryBuffer[ dwIndex ].dwMicrosecondCount, hFile ) );
            ChkDR( _writeStringtoFile( ", ", hFile ) );

            ChkDR( _writeStringtoFile( s_poDRM_PERF_MemoryBuffer[ dwIndex ].rgcScopeID, hFile ) );
            ChkDR( _writeStringtoFile( " \r\n", hFile ) );
        }

        if ( ++dwIndex == s_dwDRM_PERF_MaxBufferEntries )
        {
            dwIndex = 0;
        }
    } while ( dwIndex != s_dwDRM_PERF_CurrentIndex );

ErrorExit:
    SAFE_OEM_FREE( dstrFileName.pwszString );
    SAFE_OEM_FREE( pwszFullPath );
    Oem_File_Close( hFile );
    return dr;
}

/******************************************************************************
**
** Function:  DRM_PERF_On
**
** Synopsis:  Turn on performance recording if the memory buffer has been previously allocated.
**
** Arguments: none
**
** Returns:   DRM_RESULT:
**                DRM_SUCCESS,           if everything is OK
**                DRM_E_BUFFERTOOSMALL,  if the memory buffer has not previously been allocated
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_PERF_On()
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkBOOL( s_poDRM_PERF_MemoryBuffer != NULL, DRM_E_BUFFERTOOSMALL );
    s_fDRM_PERF_CollectData = TRUE;

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function:  DRM_PERF_Off
**
** Synopsis:  Turn off performance recording.
**
** Arguments: none
**
** Returns:   void
**
******************************************************************************/
DRM_API DRM_VOID DRM_CALL DRM_PERF_Off()
{
    s_fDRM_PERF_CollectData = FALSE;
}

/******************************************************************************
**
** Function:  DRM_PERF_LogMicrosecondCount
**
** Synopsis:  Record the the current microsecond count against the specified parameters.
**
** Arguments: [f_eLogTag]       : Enum value indicating either Enter or Exit
**            [f_eLogFunction]  : Index into function string array to associate with the microsecond count
**
** Returns:   DRM_VOID - never cause callers to fail if performance tracing fails
**
******************************************************************************/
DRM_API DRM_VOID DRM_CALL DRM_PERF_LogMicrosecondCount(
    __in   const      eDRM_PERF_LogTag      f_eLogTag,
    __in   const      eDRM_PERF_LogFunction f_eLogFunction )
{
    DRM_RESULT dr                 = DRM_SUCCESS;
    DRM_DWORD  dwMicrosecondCount = 0;

    if ( s_fDRM_PERF_CollectData )
    {
        /*
        ** For increased accuracy, get the microsecond count before doing anything else.
        */
        dwMicrosecondCount = ( DRM_UI64Low32( DRM_UI64Div( DRM_UI64Mul( Oem_Clock_QueryPerformanceCounter( NULL ), s_qwDRM_PERF_MicrosecondsPerSecond ), Oem_Clock_QueryPerformanceFrequency( NULL ) ) ) );

        ChkArg( ((DRM_DWORD)f_eLogFunction) < NO_OF( s_rgpszDRM_PERF_LogFunctionString ) );
        ChkArg( f_eLogTag != eDRM_PERF_LogTag_Separator );
        ChkBOOL( s_poDRM_PERF_MemoryBuffer != NULL, DRM_E_BUFFERTOOSMALL );

        s_poDRM_PERF_MemoryBuffer[ s_dwDRM_PERF_CurrentIndex ].eLogTag = f_eLogTag;
        MEMCPY(s_poDRM_PERF_MemoryBuffer[ s_dwDRM_PERF_CurrentIndex ].rgcScopeID, s_rgpszDRM_PERF_LogFunctionString[ (DRM_DWORD)f_eLogFunction ], DRM_PERF_MAX_SCOPE_ID_LENGTH );
        s_poDRM_PERF_MemoryBuffer[ s_dwDRM_PERF_CurrentIndex ].dwMicrosecondCount = dwMicrosecondCount;

        if ( ++s_dwDRM_PERF_CurrentIndex == s_dwDRM_PERF_MaxBufferEntries )
        {
            s_dwDRM_PERF_CurrentIndex = 0;
            s_fDRM_PERF_BufferWrappedAround = TRUE;
        }
    }

ErrorExit:
    return;
}

/******************************************************************************
**
** Function:  DRM_PERF_LogSeparator
**
** Synopsis:  Record a separator in the log file, useful for delineating loop iterations.
**
** Arguments: none
**
** Returns:
**                DRM_SUCCESS,           if everything is OK
**                DRM_E_FAIL,            if perf collection is not turned on.
**                DRM_E_BUFFERTOOSMALL,  if the memory buffer has not previously been allocated
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_PERF_LogSeparator()
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkBOOL( s_fDRM_PERF_CollectData, DRM_E_FAIL );
    ChkBOOL( s_poDRM_PERF_MemoryBuffer != NULL, DRM_E_BUFFERTOOSMALL );

    s_poDRM_PERF_MemoryBuffer[ s_dwDRM_PERF_CurrentIndex ].eLogTag = eDRM_PERF_LogTag_Separator;
    s_poDRM_PERF_MemoryBuffer[ s_dwDRM_PERF_CurrentIndex ].dwMicrosecondCount = 1;

    if ( ++s_dwDRM_PERF_CurrentIndex == s_dwDRM_PERF_MaxBufferEntries )
    {
        s_dwDRM_PERF_CurrentIndex = 0;
    }

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

