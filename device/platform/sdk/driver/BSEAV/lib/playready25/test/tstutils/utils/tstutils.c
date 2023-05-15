/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#if WINCE_TEST
static char *_pszBaseDir = "\\windows\\drmtest\\";
#endif

#include <tstutils.h>
#include <drmcrt.h>
#include <tclib.h>
#include <tOEMIMP.h>
#include <drmkeyfile.h>
#include <tstkeyfile.h>
#include <devicedevcert.h>
#include <drmbcertparser.h>

/*
** Module Definitions includes
*/
#include <drmwmdrm.h>
#include <drmsyncstore.h>
#include <drmmetering.h>
#include <drmdomainapi.h>
#include <drmactivation.h>
#include <drmprnd.h>
#include <drmmathsafe.h>
#include <drmmanager.h>
#include <drmdeviceassets.h>
#include <drmwmdrmnet.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

/* This global in oemtime.c allows an offset to be added to the system clock */
ENTER_PK_NAMESPACE_CODE;
extern DRM_EXPORT_VAR DRM_LONG g_lOEMTimeOffset;
extern DRM_EXPORT_VAR DRM_BOOL g_fOEMTimeOffsetIsAbsolute;
EXIT_PK_NAMESPACE_CODE;

ENTER_PKTEST_NAMESPACE_CODE;

#define MAX_ASF_HEADER_SIZE      30000
#define MAX_TYPE_DATA_SIZE       1024
#define OBJ_SIZE_COMMON          ( SIZEOF( DRM_GUID ) + SIZEOF( DRM_INT64 ) )
#define ROOT_OBJ_SIZE            ( OBJ_SIZE_COMMON + 6 )
#define DEFAULT_FORMAT_DATA_SIZE 4

typedef enum
{
    STREAM_TYPE_UNKNOWN = 0,
    STREAM_TYPE_AUDIO,
    STREAM_TYPE_VIDEO,
    STREAM_TYPE_SCRIPT
} ASF_STREAM_TYPE;

/* Constants used for ASF to PMF Conversion */
const DRM_GUID ASFStreamPropertiesObject =
    { 0xb7dc0791, 0xa9b7, 0x11cf, { 0x8e, 0xe6, 0x00, 0xc0, 0x0c, 0x20, 0x53, 0x65 } };

const DRM_GUID ASFPlayReadyObject =
    { 0x9A04F079, 0x9840, 0x4286, { 0xAB,0x92,0xE6,0x5B,0xE0,0x88,0x5F,0x95 } };

const DRM_GUID ASFContentEncryptionObject =
    { 0x2211B3FB, 0xBD23, 0x11D2, { 0xB4, 0xB7, 0x00, 0xA0, 0xC9, 0x55, 0xFC, 0x6E } };

const DRM_GUID ASFContentEncryptionObjectEx =
    { 0x298AE614, 0x2622, 0x4C17, { 0xB9, 0x35, 0xDA, 0xE0, 0x7E, 0xE9, 0x28, 0x9C } };

const DRM_GUID ASFDigitalSignatureObject =
    { 0x2211B3FC, 0xBD23, 0x11D2, { 0xB4, 0xB7, 0x00, 0xA0, 0xC9, 0x55, 0xFC, 0x6E } };

const DRM_GUID ASFStreamTypeAudioMedia =
    { 0xf8699e40, 0x5b4d, 0x11cf, { 0xa8, 0xfd, 0x00, 0x80, 0x5f, 0x5c, 0x44, 0x2b } };

const DRM_GUID ASFStreamTypeVideoMedia =
    { 0xbc19efc0, 0x5b4d, 0x11cf, { 0xa8, 0xfd, 0x00, 0x80, 0x5f, 0x5c, 0x44, 0x2b } };

const DRM_GUID ASFStreamTypeCommandMedia =
    { 0x59dacfc0, 0x59e6, 0x11d0, { 0xa3, 0xac, 0x00, 0xa0, 0xc9, 0x03, 0x48, 0xf6 } };

const DRM_GUID ASFStreamTypePlayReadyEncryptedCommandMedia =
    { 0x8683973A, 0x6639, 0x463A, { 0xab, 0xd7, 0x64, 0xf1, 0xce, 0x3e, 0xea, 0xe0 } };

const DRM_GUID ASFStreamTypeBinaryMedia =
    { 0x3AFB65E2, 0x47EF, 0x40F2, { 0xAC, 0x2C, 0x70, 0xA9, 0x0D, 0x71, 0xD3, 0x43 } };


/*********************************************************
**
** Function: TST_UTL_CompareSystemTime
**
** Synopsis: Compares two DRMSYSTEMTIME variables.
**
** Arguments:
**     f_pLeftTime         -- Pointer to the left side of the comparison
**     f_pRightTime        -- Pointer to the left side of the comparison
**
** Returns
**     0                   -- *f_pLeftTime == *f_pRightTime
**     -1                  -- *f_pLeftTime <  *f_pRightTime
**     1                   -- *f_pLeftTime >  *f_pRightTime
**
*********************************************************/
DRM_LONG DRM_CALL TST_UTL_CompareSystemTime(
    __in const DRMSYSTEMTIME *f_pLeftTime,
    __in const DRMSYSTEMTIME *f_pRightTime )
{
    DRMFILETIME ftLeft  = {0};
    DRMFILETIME ftRight = {0};
    DRM_UINT64  uiLeft  = {0};
    DRM_UINT64  uiRight = {0};

    Oem_Clock_SystemTimeToFileTime( f_pLeftTime,  &ftLeft );
    Oem_Clock_SystemTimeToFileTime( f_pRightTime, &ftRight );

    FILETIME_TO_UI64( ftLeft,  uiLeft );
    FILETIME_TO_UI64( ftRight, uiRight );

    if( DRM_UI64Eql( uiLeft, uiRight ) )
    {
        return 0;
    }
    if( DRM_UI64Les( uiLeft, uiRight ) )
    {
        return -1;
    }
    /*
    ** If we get here, left must be greater.
    */
    return 1;
}

/*******************************************************************************
**
** Function: TST_UTL_FixPathSeparatorA
**
** Synopsis: Replaces Windows path separators with the system path separator.
**
** Arguments:
**  f_pszPath -- Path to fix
**
** Returns     DRM_VOID
**
*******************************************************************************/
DRM_VOID DRM_CALL TST_UTL_FixPathSeparatorA( __inout_z DRM_CHAR f_pszPath[DRM_MAX_PATH])
{
    DRM_CHAR chWinSep = '\\';

    if ( f_pszPath == NULL
        || chWinSep  == g_chPathSeparator )
    {
        return;
    }
    while ( *f_pszPath != g_wchNull )
    {
        if ( *f_pszPath == chWinSep )
        {
            *f_pszPath = g_chPathSeparator;
        }
        f_pszPath++;
    }
}

/*******************************************************************************
**
** Function: TST_UTL_FixPathSeparatorW
**
** Synopsis: Replaces Windows path separators with the system path separator.
**
** Arguments:
**  f_pwszPath -- Path to fix
**
** Returns     DRM_VOID
**
*******************************************************************************/
DRM_VOID DRM_CALL TST_UTL_FixPathSeparatorW( __inout_z DRM_WCHAR f_pwszPath[DRM_MAX_PATH])
{
    DRM_WCHAR wchWinSep = ONE_WCHAR( '\\', '\0' );

    if ( f_pwszPath == NULL
        || wchWinSep  == g_wchPathSeparator )
    {
        return;
    }
    while ( *f_pwszPath != g_wchNull )
    {
        if ( *f_pwszPath == wchWinSep )
        {
            *f_pwszPath = g_wchPathSeparator;
        }
        f_pwszPath++;
    }
}

/*********************************************************
**
** Function: TST_UTL_MakeExplicitPathString
**
** Synopsis: This function concatenates the folder and file strings.
** Always null terminates the string.
**
** Arguments:
**     f_pszDir         -- Folder containing the file to open. May be NULL
**     f_pszFileName    -- Relative path to the file
**     f_pdstrResult    -- pointer to a DRM_CONST_STRING to store the path. Memory for the string
**                         will be malloced. The caller is responsible for memory management.
**
** Returns
**     DRM_SUCCESS              -- Always unless DRM_E_TEST_INVALIDARG
**     DRM_E_TEST_INVALIDARG    -- If any argument is invalid
**     DRM_E_OUTOFMEMORY        -- Malloc failed
**
*********************************************************/
DRM_RESULT DRM_CALL TST_UTL_MakeExplicitPathString( __in_z const DRM_CHAR *f_pszDir, __in_z const DRM_CHAR *f_pszFileName, __out_ecount(1) DRM_CONST_STRING *f_pdstrResult )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    char            *pszFullPath  = NULL;
    DRM_DWORD        cchFullPath  = 0;

    ChkArg( f_pszDir != NULL );
    ChkArg( f_pszFileName != NULL );
    ChkArg( f_pdstrResult != NULL );

    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( f_pszDir ) + DRMCRT_strlen( f_pszFileName ) + 1, &cchFullPath ) );;
    ChkMem( pszFullPath = (char *)Oem_MemAlloc( cchFullPath ) );
    MEMSET( pszFullPath, 0, cchFullPath );

    ChkDR( DRM_STR_StringCchCopyA( pszFullPath, cchFullPath, f_pszDir ) );
    ChkDR( DRM_STR_StringCchCatA( pszFullPath, cchFullPath, f_pszFileName ) );

    ChkDR( MakeDRMString( f_pdstrResult, pszFullPath ) );

ErrorExit:
    SAFE_OEM_FREE( pszFullPath );
    return dr;
}

/*********************************************************
**
** Function: TST_UTL_MakePathString
**
** Synopsis: This function concatenates the folder and file strings given
** with _pszBaseDir. This Function should be used to resolve
** relative paths before trying to open files. Always null terminates the string.
**
** Arguments:
**     f_pszFolder         -- Folder containing the file to open. May be NULL
**     f_pszFile           -- Relative path to the file
**     f_pdstrPath         -- pointer to a DRM_CONST_STRING to store the path. Memory for the string
**                            will be malloced. The caller is responsible for memory management.
**
** Returns
**     DRM_SUCCESS              -- Always unless DRM_E_TEST_INVALIDARG
**     DRM_E_TEST_INVALIDARG    -- If any argument is invalid
**     DRM_E_OUTOFMEMORY        -- Malloc failed
**
*********************************************************/
DRM_RESULT DRM_CALL TST_UTL_MakePathString( __in_z_opt const DRM_CHAR         *f_pszFolder,
                                            __in_z const     DRM_CHAR         *f_pszFile,
                                            __out_ecount(1)  DRM_CONST_STRING *f_pdstrPath)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cchTemp = 0;
    DRM_CHAR  *pszTemp = NULL;

    ChkDR( TST_UTL_MakePathA( f_pszFolder, f_pszFile, &pszTemp, &cchTemp, FALSE ) );

    ChkDR( MakeDRMString( f_pdstrPath, pszTemp ) );

ErrorExit:
    SAFE_OEM_FREE( pszTemp );
    return dr;
}

/*********************************************************
**
** Function: _MakePathW
**
** Synopsis: Wide-Char Version of TST_UTL_MakePathA -- only used in LoadTestFileW -- DO NOT USE.
**           This function will disappear after refactoring LoadTestFile and LoadTestFileW!!!
**           Use TST_UTL_MakePathString if you need a wide character path.
**
** This function concatenates the folder and file strings given
** with _pszBaseDir.
**
** Always null terminates the string.
**
** Arguments:
**     f_pwszFolder        -- Folder containing the file to open. May be NULL
**     f_pwszFile          -- Relative path to the file
**     f_ppwszPath         -- pointer to pointer to Output buffer.
**                               if it points to NULL, the buffer will be inited by function.
**                               Caller must free
**     f_pcchPath          -- On input, contains the size in characters of f_pwszPath.
**                            On output, contains the number of Characters written to f_pwszPath, not counting the terminator.
**     f_fFuzzPath         -- boolean indicating if the global fuzz path should be used.  If false, this uses the global file path.
**
** Returns
**     DRM_SUCCESS              -- Always unless DRM_E_TEST_INVALIDARG
**     DRM_E_TEST_INVALIDARG    -- If any argument is invalid
**     DRM_E_OUTOFMEMORY        -- Malloc failed
**
*********************************************************/
static DRM_RESULT DRM_CALL _MakePathW( __in_z_opt const                 DRM_WCHAR *f_pwszFolder,
                                      __in_z const                      DRM_WCHAR *f_pwszFile,
                                      __deref_out_ecount_z(*f_pcchPath) DRM_WCHAR **f_ppwszPath,
                                      __inout_ecount(1)                 DRM_DWORD *f_pcchPath,
                                      DRM_BOOL f_fFuzzPath )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  cchStored = 0;
    DRM_DWORD  cchNeeded = 1; /* need at least one for terminator */
    DRM_DWORD  cchFile   = 0;
    DRM_DWORD  cchFolder = 0;
    DRM_WCHAR *pwszPath  = NULL;
    DRM_CONST_STRING dstrTemp = EMPTY_DRM_STRING;

    ChkArg( f_pwszFile != NULL );
    ChkArg( f_pcchPath != NULL );

    cchFile = ( DRM_DWORD ) DRMCRT_wcslen( f_pwszFile );
    cchNeeded += cchFile;

    if ( f_fFuzzPath )
    {
        dstrTemp.pwszString = g_dstrFuzzPath.pwszString;
        dstrTemp.cchString = g_dstrFuzzPath.cchString;
    }
    else
    {
        if ( f_pwszFolder != NULL )
        {
            cchFolder = ( DRM_DWORD ) DRMCRT_wcslen( f_pwszFolder );
            cchNeeded += cchFolder + 1; /* plus one for trailing "\" */
        }
        dstrTemp.pwszString = g_dstrFilePath.pwszString;
        dstrTemp.cchString = g_dstrFilePath.cchString;
    }
    cchNeeded += dstrTemp.cchString;

#if WINCE_TEST
    ChkArg( cchNeeded <= MAX_PATH + 1 );
#endif

    if ( *f_ppwszPath == NULL )
    {
        ChkMem( *f_ppwszPath = ( DRM_WCHAR * ) Oem_MemAlloc( cchNeeded * SIZEOF( DRM_WCHAR ) ) );
        *f_pcchPath = cchNeeded;
    }
    else
    {
        ChkArg( cchNeeded <= *f_pcchPath );
    }
    pwszPath = *f_ppwszPath;

    ChkArg( cchStored + dstrTemp.cchString < *f_pcchPath );

    DRM_BYT_CopyBytes( pwszPath, cchStored, dstrTemp.pwszString, 0, dstrTemp.cchString * SIZEOF(DRM_WCHAR) );
    cchStored += dstrTemp.cchString;

    if ( !f_fFuzzPath && f_pwszFolder != NULL )
    {
        ChkArg( cchStored + cchFolder < *f_pcchPath );

        DRM_BYT_CopyBytes( pwszPath, cchStored, f_pwszFolder, 0, cchFolder * SIZEOF(DRM_WCHAR) );
        cchStored += cchFolder;
        pwszPath[ cchStored ] = g_wchPathSeparator;
        cchStored++;
    }

    ChkArg( cchStored + cchFile < *f_pcchPath );

    DRM_BYT_CopyBytes( pwszPath, cchStored, f_pwszFile, 0, SIZEOF( DRM_WCHAR ) * cchFile );
    cchStored += cchFile;
    pwszPath[ cchStored ] = ONE_WCHAR('\0', '\0');

    TST_UTL_FixPathSeparatorW(pwszPath);

    *f_pcchPath = cchStored;

ErrorExit:
    return dr;
}


/*********************************************************
**
** Function: TST_UTL_MakePathA
**
** Synopsis: This function concatenates the folder and file strings given
** with _pszBaseDir.
**
** Always null terminates the string.
**
** Arguments:
**     f_pszFolder        -- Folder containing the file to open. May be NULL
**     f_pszFile          -- Relative path to the file
**     f_ppszPath         -- pointer to pointer to Output buffer.
**                               if it points to NULL, the buffer will be inited by function.
**                               Caller must free
**     f_pcchPath         -- On input, contains the size in characters of f_pwszPath.
**                           On output, contains the number of Characters written to f_pwszPath, not counting the terminator.
**     f_fFuzzPath         -- boolean indicating if the global fuzz path should be used.  If false, this uses the global file path.
**
** Returns
**     DRM_SUCCESS              -- Always unless DRM_E_TEST_INVALIDARG
**     DRM_E_TEST_INVALIDARG    -- If any argument is invalid
**
*********************************************************/
DRM_RESULT DRM_CALL TST_UTL_MakePathA( __in_z_opt const                  DRM_CHAR   *f_pszFolder,
                                       __in_z const                      DRM_CHAR   *f_pszFile,
                                       __deref_out_ecount_z(*f_pcchPath) DRM_CHAR  **f_ppszPath,
                                       __inout_ecount(1)                 DRM_DWORD  *f_pcchPath,
                                                                         DRM_BOOL    f_fFuzzPath )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_DWORD  cchStored  = 0;
    DRM_DWORD  cchNeeded  = 1; /* need at least one for terminator */
    DRM_DWORD  cchFile    = 0;
    DRM_DWORD  cchFolder  = 0;
    DRM_DWORD  cchBaseDir = 0;
    DRM_CHAR  *pszPath    = NULL;
    DRM_CHAR   rgchBaseDir[256] = {0};

    ChkArg( f_pszFile  != NULL );
    ChkArg( f_pcchPath != NULL );

    cchFile = ( DRM_DWORD ) strlen( f_pszFile );
    cchNeeded += cchFile;

    if ( f_fFuzzPath )
    {
        DRM_UTL_DemoteUNICODEtoASCII( g_dstrFuzzPath.pwszString, rgchBaseDir, SIZEOF( rgchBaseDir ) - 1 );
        cchNeeded += g_dstrFuzzPath.cchString;
        cchBaseDir = g_dstrFuzzPath.cchString;
    }
    else
    {
        if ( f_pszFolder != NULL )
        {
            cchFolder = ( DRM_DWORD ) strlen( f_pszFolder );
            cchNeeded += cchFolder;
            if ( cchFolder                >  0
              && f_pszFolder[cchFolder-1] != g_chPathSeparator )
            {
                cchNeeded++;
            }
        }
        DRM_UTL_DemoteUNICODEtoASCII( g_dstrFilePath.pwszString, rgchBaseDir, SIZEOF( rgchBaseDir ) - 2 );
        cchNeeded += g_dstrFilePath.cchString;
        cchBaseDir = g_dstrFilePath.cchString;
        if ( cchBaseDir > 0
            && rgchBaseDir[cchBaseDir - 1] != g_chPathSeparator )
        {
            rgchBaseDir[cchBaseDir] = g_chPathSeparator;
            cchNeeded++;
            cchBaseDir++;
            rgchBaseDir[cchBaseDir] = '\0';
        }
    }


#if WINCE_TEST
    ChkArg( cchNeeded <= MAX_PATH + 1 );
#endif

    if ( *f_ppszPath == NULL )
    {
        ChkMem( *f_ppszPath = ( DRM_CHAR * ) Oem_MemAlloc( cchNeeded * SIZEOF( DRM_CHAR ) ) );
        ZEROMEM( *f_ppszPath, cchNeeded * SIZEOF( DRM_CHAR ) );
        *f_pcchPath = cchNeeded;
    }
    else
    {
        ChkArg( cchNeeded <= *f_pcchPath );
    }
    pszPath = *f_ppszPath;

    DRM_BYT_CopyBytes( pszPath, cchStored, rgchBaseDir, 0, cchBaseDir * SIZEOF(DRM_CHAR) );
    cchStored += cchBaseDir;
    if ( f_pszFolder != NULL )
    {
        DRM_BYT_CopyBytes( pszPath, cchStored, f_pszFolder, 0, cchFolder * SIZEOF(DRM_CHAR) );
        cchStored += cchFolder;
        if ( cchFolder                >  0
            && f_pszFolder[cchFolder-1] != g_chPathSeparator )
        {
            PUT_CHAR( pszPath, cchStored, (DRM_BYTE)g_chPathSeparator );
            cchStored++;
        }
    }
    DRM_BYT_CopyBytes( pszPath, cchStored, f_pszFile, 0, SIZEOF( DRM_CHAR ) * cchFile );
    cchStored +=  cchFile;
    PUT_CHAR( pszPath, cchStored, '\0' );

    TST_UTL_FixPathSeparatorA(pszPath);

    *f_pcchPath = cchStored;

ErrorExit:
    return dr;
}


DRM_RESULT DRM_CALL MakeDRMString(DRM_CONST_STRING *pwszStr, const char *szStr)
{
    DRM_RESULT       dr            = DRM_SUCCESS;

    if ( szStr )
    {
        pwszStr->cchString = (DRM_DWORD)TST_UTL_mbstowcs(NULL, szStr, (DRM_DWORD)strlen(szStr));
        ChkMem( pwszStr->pwszString = ( const DRM_WCHAR* )Oem_MemAlloc( ( pwszStr->cchString + 1 ) * SIZEOF( DRM_WCHAR ) ) );
        ZEROMEM( pwszStr->pwszString, ( pwszStr->cchString + 1 ) * SIZEOF( DRM_WCHAR ) );
        TST_UTL_mbstowcs( (DRM_WCHAR *)pwszStr->pwszString, szStr, pwszStr->cchString + 1 );
    }
    else
    {
        MEMSET(pwszStr, 0, SIZEOF(DRM_CONST_STRING));
    }

ErrorExit:
    return dr;
}


/**************************************************************************************************
** Function:    TST_UTL_mbstowcs
**
** Synopsis:    Converts a C string to a Wide Char string
**
** Arguments:   f_pwszDest  - pointer to a target wide char string.
**                            If this is NULL, the function returns the length of the source string.
**              f_pszSource - pointer to a source C string, must not be NULL.
**              f_cchLength - number of characters to copy.
**
**************************************************************************************************/
DRM_DWORD DRM_CALL TST_UTL_mbstowcs( __inout_opt                    DRM_WCHAR *f_pwszDest,
                                     __in_ecount(f_cchLength) const DRM_CHAR  *f_pszSource,
                                     __in                           DRM_DWORD  f_cchLength )
{
    DRM_SUBSTRING dsstrWorker = EMPTY_DRM_SUBSTRING;
    DRM_STRING    dstrDest    = EMPTY_DRM_STRING;

    if ( NULL == f_pwszDest )
    {
        return (DRM_DWORD)strlen( f_pszSource );
    }

    dsstrWorker.m_cch   = f_cchLength;
    dstrDest.pwszString = f_pwszDest;
    dstrDest.cchString  = f_cchLength;

    DRM_UTL_PromoteASCIItoUNICODE(f_pszSource, &dsstrWorker, &dstrDest);
    return dstrDest.cchString;
}

int DRM_CALL CompDRMString(const DRM_CONST_STRING *pwszStr, const char *szStr)
{
    DRM_RESULT       dr   = DRM_SUCCESS;
    int              iRet = 0;
    DRM_CONST_STRING wsz;

    ChkDR( MakeDRMString(&wsz, szStr) );
    iRet = DRMCRT_wcsncmp(pwszStr->pwszString, wsz.pwszString, min(pwszStr->cchString, wsz.cchString));
    if (!iRet && pwszStr->cchString != wsz.cchString)
        iRet = (pwszStr->cchString > wsz.cchString)? 1: -1;

ErrorExit:
    DRMASSERT( dr == DRM_SUCCESS );
    FREEDRMSTRING( wsz );
    return iRet;
}

int DRM_CALL CompWideString(const DRM_WCHAR *pwszStr, const char *szStr)
{
    DRM_CONST_STRING wsz;
    wsz.pwszString = pwszStr;
    wsz.cchString = ( DRM_DWORD ) DRMCRT_wcslen(pwszStr);
    return CompDRMString(&wsz, szStr);
}

DRM_BOOL DRM_CALL LoadTestFileW(const DRM_WCHAR* pwszFolder, const DRM_WCHAR* pwszPath, DRM_BYTE **ppBuffer, DRM_DWORD *pSize)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL fRet = FALSE;
    OEM_FILEHDL hFile = OEM_INVALID_HANDLE_VALUE;
    DRM_WCHAR wcfuzzpath[256] = { 0 }; /* _MAX_PATH not ansi */
    DRM_WCHAR wcfilepath[256] = { 0 }; /* _MAX_PATH not ansi */
    DRM_WCHAR wcOriginalPath[256] = { 0 };
    DRM_WCHAR *pwcfuzzpath = &wcfuzzpath[0];
    DRM_WCHAR *pwcfilepath = &wcfilepath[0];
    DRM_DWORD cbfuzzRead = 0;
    DRM_DWORD cbfileRead = 0;
    DRM_DWORD cbOriginalPathLength = 0;
    DRM_DWORD cbRead = 0;
#if DBG
    DRM_CHAR  pzfuzzPath[256] = { 0 }; /* _MAX_PATH ansi */
    DRM_CHAR  pzfilePath[256] = { 0 }; /* _MAX_PATH ansi */
#endif

    ChkArg( pwszPath != NULL );
    ChkArg( ppBuffer != NULL );
    ChkArg( pSize != NULL );

    cbfuzzRead = NO_OF( wcfuzzpath );
    cbfileRead = NO_OF( wcfilepath );
    if ( DRM_FAILED( _MakePathW( pwszFolder, pwszPath, &pwcfuzzpath, &cbfuzzRead, TRUE ) ) )
    {
        fRet = FALSE;
        goto ErrorExit;
    }
    if ( DRM_FAILED( _MakePathW( pwszFolder, pwszPath, &pwcfilepath, &cbfileRead, FALSE ) ) )
    {
        fRet = FALSE;
        goto ErrorExit;
    }
    *pSize = 0;
    *ppBuffer = 0;

#if DBG
    /*
    ** Get ASCII file name for tracing in case of error
    */
    DRM_UTL_DemoteUNICODEtoASCII( pwcfuzzpath, pzfuzzPath, SIZEOF( pzfuzzPath ) - 1 );
    DRM_UTL_DemoteUNICODEtoASCII( pwcfilepath, pzfilePath, SIZEOF( pzfilePath ) - 1 );
#endif

    hFile = Oem_File_Open(NULL, wcfuzzpath, OEM_GENERIC_READ, OEM_FILE_SHARE_READ, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL);
    if ( OEM_INVALID_HANDLE_VALUE == hFile )
    {
        hFile = Oem_File_Open(NULL, wcfilepath, OEM_GENERIC_READ, OEM_FILE_SHARE_READ, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL);
        if ( OEM_INVALID_HANDLE_VALUE == hFile
            && pwszFolder == NULL )
        {
            /*
            ** Just try using original file path - it could have been a full path in the first place!
            */
            cbOriginalPathLength = ( DRM_DWORD ) (DRMCRT_wcslen( pwszPath ) * SIZEOF( DRM_WCHAR ));
            DRM_BYT_CopyBytes( wcOriginalPath, 0, pwszPath, 0, cbOriginalPathLength );
            hFile = Oem_File_Open( NULL, wcOriginalPath, OEM_GENERIC_READ, OEM_FILE_SHARE_READ, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL);
        }
        if ( OEM_INVALID_HANDLE_VALUE == hFile )
        {
            TRACE(("LoadTestFileW - File %s open failed\n", pzfuzzPath ));
            TRACE(("LoadTestFileW - File %s open failed\n", pzfilePath ));
            goto ErrorExit;
        }
    }

    if (!Oem_File_GetSize(hFile, pSize))
    {
        TRACE(("LoadTestFileW - Get Size on file %s failed\n", pzfilePath ));
        goto ErrorExit;
    }

    ChkMem( *ppBuffer = (DRM_BYTE*) Oem_MemAlloc( *pSize + SIZEOF(DRM_WCHAR) ) );

    MEMSET(*ppBuffer, 0, SIZEOF(DRM_WCHAR) + *pSize);

     if(!Oem_File_Read(hFile, *ppBuffer, *pSize, &cbRead) || cbRead != *pSize)
     {
         TRACE(("LoadTestFileW - Load of data from file %s failed, bytes read %d\n", pzfilePath, cbRead ));
         goto ErrorExit;
     }
    fRet = TRUE;
ErrorExit:
    if (hFile != OEM_INVALID_HANDLE_VALUE)
        Oem_File_Close(hFile);
    return fRet;
}

DRM_BOOL DRM_CALL LoadTestFile(const DRM_CHAR* pszFolder, const DRM_CHAR* pszPath, DRM_BYTE **ppBuffer, DRM_DWORD *pSize)
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_CONST_STRING dstrFolder = {0}, dstrPath = {0};
    DRM_BOOL         fRet       = FALSE;
    ChkDR( MakeDRMString(&dstrFolder, pszFolder) );
    ChkDR( MakeDRMString(&dstrPath, pszPath) );

    fRet = LoadTestFileW(dstrFolder.pwszString, dstrPath.pwszString, ppBuffer, pSize);
ErrorExit:
    DRMASSERT( dr == DRM_SUCCESS );
    FREEDRMSTRING( dstrFolder );
    FREEDRMSTRING( dstrPath );
    return fRet;


}

DRM_BOOL DRM_CALL SaveToFileW(const DRM_WCHAR* pwszPath, DRM_BYTE *pbData, DRM_DWORD cbData)
{
    DRM_BOOL fRet = FALSE;
    OEM_FILEHDL hFile = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD cbWritten;

    if (!(pwszPath && pbData))
        goto ErrorExit;

    hFile = Oem_File_Open(NULL, pwszPath, OEM_GENERIC_WRITE, OEM_FILE_SHARE_NONE, OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_NORMAL);
    if (hFile == OEM_INVALID_HANDLE_VALUE)
    {
        /*
        ** Try to open a hidden file, then fail is this still doesn't work
        */
        hFile = Oem_File_Open(NULL, pwszPath, OEM_GENERIC_WRITE, OEM_FILE_SHARE_NONE, OEM_CREATE_ALWAYS, OEM_ATTRIBUTE_HIDDEN );
        if (hFile == OEM_INVALID_HANDLE_VALUE)
            goto ErrorExit;
    }
    if (!Oem_File_Write(hFile, pbData, cbData, &cbWritten) || cbWritten != cbData)
        goto ErrorExit;
    fRet = TRUE;
ErrorExit:
    if (hFile != OEM_INVALID_HANDLE_VALUE)
        Oem_File_Close(hFile);
    return fRet;
}


#if WINCE_TEST
/*replace c remove function with win32 */
DRM_LONG remove(const char* filename)
{
    DRM_WCHAR wszHdsName[256]={0};
    TST_UTL_mbstowcs(wszHdsName, filename, strlen(filename)+1);
    if ( 0 == DeleteFileW( wszHdsName ) )
    {
        return GetLastError();
    }
    else
    {
        /*
        ** C remove function returns 0 on success
        */
        return 0;
    }
}

/*replace c clock with win32. Return millseconds */
DRM_DWORD MyClock()
{
   return (DRM_DWORD)GetTickCount();
}

#endif


DRM_BOOL DRM_CALL IsDigitalOnlyString(const char *p)
{
    if(!p)
        return FALSE;
    if (*p == '-')
        p++;
    while(*p && isdigit((unsigned char)*p))
        p++;
    return !(*p);
}

/*********************************************************
**
** Function: StringToBool
**
** Synopsis: This takes a C-string which contains a string
** version of a boolean and converts it into a DRM_BOOL.
**
** Arguments:
**     psBoolAsString      -- The C-string to convert to a DRM_BOOL
**     pdStringAsBool      -- On output, this contains the DRM_BOOL
**                         value which matches the input C-string
**
** Returns
**     DRM_SUCCESS         -- On successful parsinga and translation
**     DRM_E_INVALIDARG    -- If any argument is invalid
**
*********************************************************/
DRM_RESULT DRM_CALL StringToBool(const char *psBoolAsString, DRM_BOOL* pbStringAsBool)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrBool = EMPTY_DRM_STRING;

    ChkArg((psBoolAsString != NULL) && (pbStringAsBool != NULL) );

    ChkDR( MakeDRMString(&dstrBool, psBoolAsString) );

    /* Check for TRUE */
    if ( (0 == CompDRMString(&dstrBool, "true")) ||
         (0 == CompDRMString(&dstrBool, "True")) ||
         (0 == CompDRMString(&dstrBool, "TRUE")) ||
         (0 == CompDRMString(&dstrBool, "T")) ||
         (0 == CompDRMString(&dstrBool, "t")) ||
         (0 == CompDRMString(&dstrBool, "1")) )
    {
        *pbStringAsBool = TRUE;
    }
    /* Check For FALSE */
    else if ( (0 == CompDRMString(&dstrBool, "false")) ||
         (0 == CompDRMString(&dstrBool, "False")) ||
         (0 == CompDRMString(&dstrBool, "FALSE")) ||
         (0 == CompDRMString(&dstrBool, "F")) ||
         (0 == CompDRMString(&dstrBool, "f")) ||
         (0 == CompDRMString(&dstrBool, "0")) )
    {
        *pbStringAsBool = FALSE;
    }
    else
    {
        dr = DRM_E_FAIL;
    }

ErrorExit:
    /* free memory allocated for DRM_CONST_STRING dstrBOOL in MakeDRMString */
    FREEDRMSTRING( dstrBool );
    return dr;
}

/*********************************************************
**
** Function: TST_UTL_AtoUI64
**
** Synopsis: Converts an ASCII string to a UI64
**           The string has the format: "###-###" where the first set of
**           digits is treated as the high dword and the second set is treated
**           as the low DWORD. If '-' is not found then the whole thing is treated
**           as the low dword and the high dword is 0
**
** Arguments:
**     f_pszString       -- string to convert
**     f_cchString       -- length in characters of the string to convert
**     f_pui64Value      -- Pointer to a DRM_UINT64 structure to hold the output.
**
** Returns
**      DRM_E_TEST_INVALIDARG       -- an arg to the function was bad.
**      DRM_E_ARITHMETIC_OVERFLOW   -- DRMCRT_AtoDWORD hit an overflow converting the digits
**      DRM_SUCCESS                 -- Everything was fine.
**
*********************************************************/
DRM_RESULT DRM_CALL TST_UTL_AtoUI64(
    __in_ecount( f_cchString ) const DRM_CHAR   *f_pszString,
    __in                             DRM_DWORD   f_cchString,
    __out_ecount( 1 )                DRM_UINT64 *f_pui64Value )
{
    DRM_RESULT       dr      = DRM_SUCCESS;
    const DRM_CHAR  *pszTemp = NULL;
    DRM_DWORD        ichTemp = 0;
    DRM_DWORD        dwHigh  = 0;
    DRM_DWORD        dwLow   = 0;

    ChkArg( f_pui64Value != NULL );
    ChkArg( f_pszString  != NULL );
    pszTemp = f_pszString;

    while( ichTemp < f_cchString && *pszTemp != '-' )
    {
        pszTemp++;
        ichTemp++;
    }

    if ( ichTemp < f_cchString && *pszTemp == '-' )
    {
        pszTemp++;
        ChkDR( DRMCRT_AtoDWORD( f_pszString,
                                ichTemp,
                                ( ichTemp > 2 && ( f_pszString[1] == 'X' || f_pszString[1] == 'x' ) ) ? (DRM_DWORD)16 : (DRM_DWORD)10,
                                &dwHigh ) );

        ichTemp = f_cchString - ichTemp - 1;
        ChkDR( DRMCRT_AtoDWORD( pszTemp,
                                ichTemp,
                                ( ichTemp > 2 && ( pszTemp[1] == 'X' || pszTemp[1] == 'x' ) ) ? (DRM_DWORD)16 : (DRM_DWORD)10,
                                &dwLow ) );
    }
    else
    {
        ChkDR( DRMCRT_AtoDWORD( f_pszString,
                                f_cchString,
                                ( f_cchString > 2 && ( f_pszString[1] == 'X' || f_pszString[1] == 'x' ) ) ? (DRM_DWORD)16 : (DRM_DWORD)10,
                                &dwLow ) );
    }

    *f_pui64Value = DRM_UI64HL(dwHigh, dwLow);

ErrorExit:
    return dr;
}

/*********************************************************
**
** Function: TST_UTL_ByteValueToASCII
**
** Synopsis: Converts a Byte to an ASCII Char
**
** Arguments:
**     f_bVal       -- byte value to convert
**
** Returns
**     The character representation of the byte.
**
*********************************************************/
DRM_CHAR DRM_CALL TST_UTL_ByteValueToASCII( DRM_BYTE f_bVal )
{
    DRM_CHAR chReturnValue = '0';

    /* Do not have ASCII representation for values more that 36. Return '0' as default */
    if ( f_bVal >= 36 )
    {
        goto ErrorExit;
    }

    /* Base 10 digit */
    if ( f_bVal < 10 )
    {
        chReturnValue = '0' + f_bVal;
    }
    else
    {
        /* Hex Symbol */
        chReturnValue = 'A' - 10 + f_bVal;
    }

ErrorExit :
    return chReturnValue;
}

/*********************************************************
**
** Function: TST_UTL_NumberToAnsiString
**
** Synopsis: Converts a DWORD to an ASCII string
**
** Arguments:
**      f_dwValue       -- value to convert
**      f_dwRadix       -- Radix to use for conversion
**      f_iWidth        -- Width of the result.
**                         The output string will be padded on the left up to the width
**                         set to 0 to use up to the maximum buffer and not pad left.
**      f_cbString      -- the size of the buffer including the null character
**      f_pszString     -- Output buffer.
**
** Returns
**     The number of characters converted.
**
*********************************************************/
DRM_DWORD DRM_CALL TST_UTL_NumberToAnsiString(
    __in    DRM_DWORD   f_dwValue,
    __in    DRM_DWORD   f_dwRadix,
    __in    DRM_DWORD   f_iWidth,
    __in    DRM_DWORD   f_cbString,
    __out_bcount_z(f_cbString) DRM_CHAR   *f_pszString )
{
    DRM_DWORD  iString = 0;
    DRM_DWORD  dwTemp  = f_dwValue;
    DRM_DWORD  iWidth  = 0;
    DRM_BOOL   fFill   = FALSE;

    if ( f_pszString == NULL )
    {
        goto ErrorExit;
    }

    if ( f_iWidth > 0 && f_iWidth < f_cbString )
    {
        iWidth = f_iWidth;
        fFill = TRUE;
    }
    else
    {
        iWidth = f_cbString - 1;
    }
    while( iString < iWidth )
    {
        PUT_CHAR( f_pszString, iString, (DRM_BYTE)TST_UTL_ByteValueToASCII( ( DRM_BYTE ) ( dwTemp % f_dwRadix ) ) );
        dwTemp /= f_dwRadix;
        iString++;
        if ( dwTemp == 0 )
        {
            break;
        }
    }

    if ( fFill )
    {
        while ( iString < iWidth )
        {
            PUT_CHAR( f_pszString, iString, '0' );
            iString++;
        }
    }

    PUT_CHAR( f_pszString, iString, '\0' );

    iWidth = iString;

    if ( iWidth == 0 )
    {
        goto ErrorExit;
    }

    iString = 0;

    while ( iString <= ( iWidth - 1 ) / 2 )
    {
        DRM_CHAR temp = (DRM_CHAR)GET_CHAR(f_pszString, iString);
        PUT_CHAR( f_pszString, iString, GET_CHAR(f_pszString, iWidth - 1 - iString ) );
        PUT_CHAR( f_pszString, iWidth - iString - 1, (DRM_BYTE)temp);
        iString++;
    }


ErrorExit:
    return iWidth;
}

/*******************************************************************************
** Function:    TST_UTL_StringCchCopyNA
**
** Synopsis:    Safe String Copy Function. similar to Win32's StringCchCopyNA.
**              This will always NULL terminate the string.
**
**              [f_pszDst]       Pointer to a buffer that recieves the resulting
**                               character string.
**              [f_cchDstSize]   The size of the output buffer.
**              [f_pszSrc]       Null terminated string to copy
**              [f_cchMaxCount]  The maximum number of characters to write.
**
** Returns:
**              DRM_E_TEST_INVALIDARG  One or more arguments were invalid
**              DRM_E_BUFFERTOOSMALL   The output buffer was too small and contains a
**                                     null-terminated, truncated version of the Src.
**
*********************************************************************************/
DRM_RESULT DRM_CALL TST_UTL_StringCchCopyNA(
    __out_ecount_z(f_cchDstSize)DRM_CHAR * f_pszDst,
    __in DRM_DWORD f_cchDstSize,
    __in_z_opt const DRM_CHAR * f_pszSrc,
    __in DRM_DWORD f_cchMaxCount)
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  cchLength  = 0;
    DRM_DWORD  iCurrent  = 0;
    DRM_CHAR   chCurrent = '\0';

    ChkArg( f_pszDst != NULL );
    ChkArg( f_pszSrc != NULL );

    cchLength = min( f_cchDstSize - 1, f_cchMaxCount );
    ChkArg( cchLength > 0 );
    chCurrent = (DRM_CHAR)GET_CHAR( f_pszSrc, iCurrent );
    while( iCurrent < cchLength && chCurrent != '\0' )
    {
        PUT_CHAR( f_pszDst, iCurrent, (DRM_BYTE)chCurrent );
        iCurrent++;
        chCurrent = (DRM_CHAR)GET_CHAR( f_pszSrc, iCurrent );
    }

    PUT_CHAR( f_pszDst, iCurrent, '\0' );

    /*
    ** make sure we return DRM_E_BUFFERTOOSMALL if truncated
    */
    ChkBOOL( cchLength == f_cchMaxCount , DRM_E_BUFFERTOOSMALL );

ErrorExit:
    return dr;

}

static DRM_BOOL _IsOffsetClockSupported()
{
#if LINUX_BUILD
    return TRUE;
#elif DRM_TEST_USE_OFFSET_CLOCK
    return TRUE;
#else
    return FALSE;
#endif
}

static DRM_LONG g_lTstUtilsClockOffset = 0;

DRM_VOID DRM_CALL tResetSystemTimeOffset()
{
    /* Reset all system time offset variables to defaults */
    if( _IsOffsetClockSupported() )
    {
        g_lOEMTimeOffset = 0;
        g_fOEMTimeOffsetIsAbsolute = FALSE;
    }
    else
    {
        tChangeSystemTime( -g_lTstUtilsClockOffset );
    }
}

DRM_VOID DRM_CALL tSetFlagOEMTimeOffsetIsAbsolute( DRM_BOOL f_fOEMTimeOffsetIsAbsolute )
{
    if( _IsOffsetClockSupported() )
    {
        g_fOEMTimeOffsetIsAbsolute = f_fOEMTimeOffsetIsAbsolute;
    }
}

/*offset time in seconds */
DRM_RESULT DRM_CALL tChangeSystemTime(long loffset)
{
    DRM_RESULT dr = DRM_SUCCESS;

    if( _IsOffsetClockSupported() )
    {
        if (loffset == 0)
        {
            goto ErrorExit;
        }
        g_lOEMTimeOffset += loffset;
    }
    else
    {
        DRMSYSTEMTIME tSysTime;
        DRMFILETIME   mcFileTime;
        DRM_UINT64    u64;

        if (!loffset)
            goto ErrorExit;

        Oem_Clock_GetSystemTimeAsFileTime(NULL, &mcFileTime);

        FILETIME_TO_UI64(mcFileTime, u64);
        if(loffset > 0)
        {
            u64 = DRM_UI64Add(u64, DRM_UI64Mul(DRM_UI64(loffset), DRM_UI64(FILETIME_TO_SECONDS)));
        }
        else
        {
            u64 = DRM_UI64Sub(u64, DRM_UI64Mul(DRM_UI64(-loffset), DRM_UI64(FILETIME_TO_SECONDS)));
        }
        UI64_TO_FILETIME(u64, mcFileTime);

        ChkArg(Oem_Clock_FileTimeToSystemTime(&mcFileTime, &tSysTime));
        Oem_Clock_SetSystemTime(NULL, &tSysTime);

        g_lTstUtilsClockOffset += loffset;

    }
ErrorExit:
    return dr;
}

#define AVERAGE_SECONDS_PER_YEAR 31556926
DRM_RESULT DRM_CALL tChangeYearOnMachineClock(DRM_WORD wYear)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRMSYSTEMTIME ost = {0};

    Oem_Clock_GetSystemTime(NULL, &ost);
    ChkDR( tChangeSystemTime( (wYear - ost.wYear) * AVERAGE_SECONDS_PER_YEAR ) );
ErrorExit:
    return dr;
}

/*****************************************************************************
** Function: TST_UTL_LogLicenseStateData
**
** Synopsis: writes a license state data struct to the log.
**           adapted from DRMTOOLS_PrintLicenseStateData
**
** Arguments:
**           IN  *szOutputPrefix    -- prefix for each line printed
**           IN  *pDRMLicenseStateData -- License state data to be printed
** Returns:
**           DRM_SUCCESS           -- On success
**           DRM_E_INVALIDARG      -- if any argument is invalid
**
*****************************************************************************/
DRM_RESULT DRM_CALL TST_UTL_LogLicenseStateData( const DRM_CHAR *szOutputPrefix, const DRM_LICENSE_STATE_DATA *pDRMLicenseStateData )
{
    DRM_RESULT dr           = DRM_SUCCESS;
    DRM_DWORD dwCountIndex  = 0;
    DRM_DWORD dwDateIndex   = 0;

    ChkArg( szOutputPrefix      != NULL
        && pDRMLicenseStateData != NULL );

    /*
    ** Print the type of right given by aggregating all the licenses for the content
    */
    Log( "",  "%sDRM_LICENSE_DATA.dwStreamId: %ld", szOutputPrefix, pDRMLicenseStateData->dwStreamId );
    switch( pDRMLicenseStateData->dwCategory )
    {
    case DRM_LICENSE_STATE_NORIGHT: /* No rights to perform this action */
        Log( "",  "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_NORIGHT", szOutputPrefix );
        break;

    case DRM_LICENSE_STATE_UNLIM: /* Unlimited rights to perform this action */
        Log( "",  "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_UNLIM", szOutputPrefix );
        break;

    case DRM_LICENSE_STATE_COUNT: /* Action may only be performed a certain number of times */
        Log( "",  "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_COUNT", szOutputPrefix );
        break;

    case DRM_LICENSE_STATE_FROM: /* Action cannot be performed until a specific date */
        Log( "",  "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_FROM", szOutputPrefix );
        break;

    case DRM_LICENSE_STATE_UNTIL: /* Action cannot be performed after a certain date */
        Log( "",  "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_UNTIL", szOutputPrefix );
        break;

    case DRM_LICENSE_STATE_FROM_UNTIL: /* Action can only be performed within a specific range of dates */
        Log( "",  "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_FROM_UNTIL", szOutputPrefix );
        break;

    case DRM_LICENSE_STATE_COUNT_FROM: /* Action can only be performed a certain number of times, starting from a specific date */
        Log( "",  "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_COUNT_FROM", szOutputPrefix );
        break;

    case DRM_LICENSE_STATE_COUNT_UNTIL: /* Action can be performed a certain number of times until a specific date */
        Log( "",  "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_COUNT_UNTIL", szOutputPrefix );
        break;

    case DRM_LICENSE_STATE_COUNT_FROM_UNTIL: /* Action can only be performed a certain number of times, and only is a specific range of dates */
        Log( "",  "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_COUNT_FROM_UNTIL", szOutputPrefix );
        break;

    case DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE: /* License restrictions don't occur until after the first use */
        Log( "",  "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_EXPIRATION_AFTER_FIRSTUSE", szOutputPrefix );
        break;

    case DRM_LICENSE_STATE_FORCE_SYNC:
        Log( "",  "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_FORCE_SYNC", szOutputPrefix );
        break;

    case DRM_LICENSE_STATE_NOT_FOUND:
        Log( "",  "%sDRM_LICENSE_DATA.dwCategory: DRM_LICENSE_STATE_NOT_FOUND", szOutputPrefix );
        break;

    default:
        Log( "",  "%sDRM_LICENSE_DATA.dwCategory: Unknown! - %d", szOutputPrefix, pDRMLicenseStateData->dwCategory );
    }

    /*
    ** If count limited, print the number of times the action can be performed
    */
    if ( pDRMLicenseStateData->dwNumCounts != 0 )
    {
        for( dwCountIndex = 0; dwCountIndex < pDRMLicenseStateData->dwNumCounts; dwCountIndex++ )
        {
            Log( "",  "%sDRM_LICENSE_DATA.dwCount:  %ld", szOutputPrefix, pDRMLicenseStateData->dwCount[ dwCountIndex ] );
        }
    }

    /*
    ** If the action is date limited, print the date restriction(s)
    */
    if ( pDRMLicenseStateData->dwNumDates != 0 )
    {
        for( dwDateIndex = 0; dwDateIndex < pDRMLicenseStateData->dwNumDates; dwDateIndex++ )
        {
            DRMSYSTEMTIME  st;
            Oem_Clock_FileTimeToSystemTime( &(pDRMLicenseStateData->datetime[ dwDateIndex ]), &st );
            Log( "",  "%sDRM_LICENSE_DATA.datetime:  On %04d/%02d/%02d at %02d:%02d:%02d.%03d",
                szOutputPrefix,
                st.wYear,
                st.wMonth,
                st.wDay,
                st.wHour,
                st.wMinute,
                st.wSecond,
                st.wMilliseconds);
        }
    }

    /*
    * If the aggregate license data cannot easily be represented, the "vague" value will be set.
    * This will happen when two or more licenses with different right types (like COUNT and
    * and FROM_UNTIL) make it impossible to represent the information simply.  For instance,
    * if license 1 allows 5 plays and license 2 allows unlimited playback during the current
    * month, then the user is guaranteed at least 5 plays, but possibly an unlimited number,
    * if done within the current month.
    */
    Log( "",  "%sDRM_LICENSE_DATA.dwVague: %ld ", szOutputPrefix, pDRMLicenseStateData->dwVague );

ErrorExit:
    return dr;
}

/****************************************************************************************
**
**  TST_UTL_CopyFileToFolder
**
**  Copies the given file, assuming in a location relative to the current path, to the specied folder.
**  The folder is a full path.
**
**  [f_pszFolder] -- The path to the folder to which to copy the file
**  [f_pwszFile]  -- relative path to the file to copy
**
****************************************************************************************/
DRM_RESULT DRM_CALL TST_UTL_CopyFileToFolder( __in const DRM_CHAR *f_pszFolder, __in const DRM_WCHAR *f_pwszFile )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WCHAR  wszDest[256] = { 0 };
    DRM_DWORD  cchDest      = 256;
    DRM_DWORD  cchLen       = 0;
    DRM_CONST_STRING dstrDest     = { 0 };
    DRM_CONST_STRING dstrSrc      = { 0 };

    ChkArg( DRMCRT_strlen( f_pszFolder ) + 1  <= NO_OF( wszDest ) );

    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( f_pszFolder ) + 1, &cchLen ) );
    cchLen = TST_UTL_mbstowcs( wszDest, f_pszFolder, cchLen );
    ChkBOOL( cchLen < cchDest , DRM_E_BUFFERTOOSMALL );
    if ( GET_CHAR( wszDest, cchLen - 1 ) != g_wchPathSeparator )
    {
        DRM_BYT_CopyBytes( wszDest, cchLen, &g_wchPathSeparator, 0, SIZEOF(g_wchPathSeparator) );
        cchLen++;
    }
    ChkDR( DRM_STR_StringCchCatW( wszDest, cchDest, f_pwszFile ) );
    ChkDR( DRM_SizeTToDWord( DRMCRT_wcslen( wszDest ), &cchLen ) );

    dstrDest.pwszString = wszDest;
    dstrDest.cchString = cchLen;

    ChkDR( _MakePathW( NULL, f_pwszFile, ( DRM_WCHAR ** )&(dstrSrc.pwszString), &(dstrSrc.cchString), FALSE ) );

    ChkDR( TST_OEM_CopyFile(  dstrSrc, dstrDest ) );

ErrorExit:
    FREEDRMSTRING( dstrSrc );
    return dr;
}

/*********************************************************************
**
** Function: TST_UTL_BinaryFileCompare
**
** Synopsis: This function binary compares two files
**
** Arguments:
**
** [f_pszFolder1]  -- Optional. The folder name for the first file.
** [f_pszPath1]    -- The file path for the first file.
** [f_pszFolder2]  -- Optional. The folder name for the second file.
** [f_pszPath2]    -- The file path for the first file.
**
** Returns:
**          DRM_SUCCESS                  -- On success if files are identical
**          DRM_E_TEST_UNEXPECTED_OUTPUT -- If files are different
**          DRM_INVALIDARG               -- If any arguments are invalid (NULL pointers)
**
**********************************************************************/

DRM_RESULT DRM_CALL TST_UTL_BinaryFileCompare(
    __in const DRM_CHAR* f_pszFolder1,
    __in const DRM_CHAR* f_pszPath1,
    __in const DRM_CHAR* f_pszFolder2,
    __in const DRM_CHAR* f_pszPath2 )
{
    DRM_RESULT  dr        = DRM_SUCCESS;
    DRM_DWORD   cbFile1   = 0;
    DRM_DWORD   cbFile2   = 0;
    DRM_BYTE   *pbBuffer1 = NULL;
    DRM_BYTE   *pbBuffer2 = NULL;

    ChkArg( f_pszPath1 != NULL );
    ChkArg( f_pszPath2 != NULL );

    ChkBOOL( LoadTestFile( f_pszFolder1, f_pszPath1, &pbBuffer1, &cbFile1 ), DRM_E_FILE_READ_ERROR );
    ChkBOOL( LoadTestFile( f_pszFolder2, f_pszPath2, &pbBuffer2, &cbFile2 ), DRM_E_FILE_READ_ERROR );

    ChkBOOL( cbFile1 == cbFile2, DRM_E_TEST_UNEXPECTED_OUTPUT );
    ChkBOOL( DRMCRT_memcmp( pbBuffer1, pbBuffer2, cbFile1 ) == 0, DRM_E_TEST_UNEXPECTED_OUTPUT );

ErrorExit:
    SAFE_OEM_FREE( pbBuffer1 );
    SAFE_OEM_FREE( pbBuffer2 );

    return dr;
}

/*****************************************************************************
** Function: TST_UTL_SaveDeviceStore
**
** Synopsis: Copies the existing test DeviceStore/DeviceCert to the specified
             destination. It is the responsibility of the caller to make sure
             that the devicestore/cert is not currently being used.
             (possibly by calling drm_uninitialize before calling this function)
**
** Arguments:
**           IN  *f_pszDeviceStore -- pointer to a string holding the name of
                                      destination DeviceStore
**           IN  *f_pszDeviceCert  -- pointer to a string holding the name of
                                      destination DeviceCert
**Returns:
**          DRM_SUCCESS           -- On success
*****************************************************************************/
DRM_RESULT DRM_CALL TST_UTL_SaveDeviceStore(DRM_CONST_STRING f_dstrDevStore, DRM_CONST_STRING f_dstrDevCert)
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_CONST_STRING dstrSource   = {0};
    DRM_CONST_STRING dstrDestCert = {0};
    DRM_CONST_STRING dstrDestStore = {0};
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

    ChkArg((f_dstrDevStore.pwszString != NULL) || (f_dstrDevCert.pwszString != NULL));
    ChkArg(g_dstrDrmPath.cchString > 0);


    if(f_dstrDevCert.pwszString != NULL && f_dstrDevCert.cchString > 0)
    {
        ChkDR( tGetDeviceCertPathname(wszPathName, &dstrSource) );
        dstrDestCert.cchString = f_dstrDevCert.cchString + g_dstrDrmPath.cchString + 1;
        ChkMem(dstrDestCert.pwszString = (DRM_WCHAR *) Oem_MemAlloc( (dstrDestCert.cchString) * SIZEOF(DRM_WCHAR)));
        ChkDR(DRM_STR_StringCchCopyNW((DRM_WCHAR *)dstrDestCert.pwszString, dstrDestCert.cchString,g_dstrDrmPath.pwszString, g_dstrDrmPath.cchString));
        ChkDR(DRM_STR_StringCchCatNW((DRM_WCHAR *)dstrDestCert.pwszString,dstrDestCert.cchString,f_dstrDevCert.pwszString, f_dstrDevCert.cchString));

        ChkDR(TST_OEM_CopyFile(dstrSource, dstrDestCert));
    }

    if(f_dstrDevStore.pwszString != NULL && f_dstrDevStore.cchString > 0)
    {
        ChkDR( tGetDeviceStorePathname(wszPathName, &dstrSource) );
        dstrDestStore.cchString = f_dstrDevStore.cchString + g_dstrDrmPath.cchString + 1;
        ChkMem(dstrDestStore.pwszString = (DRM_WCHAR *) Oem_MemAlloc((dstrDestStore.cchString) * SIZEOF(DRM_WCHAR)));
        ChkDR(DRM_STR_StringCchCopyNW((DRM_WCHAR *)dstrDestStore.pwszString, dstrDestStore.cchString,g_dstrDrmPath.pwszString, g_dstrDrmPath.cchString));
        ChkDR(DRM_STR_StringCchCatNW((DRM_WCHAR *)dstrDestStore.pwszString,dstrDestStore.cchString,f_dstrDevStore.pwszString, f_dstrDevStore.cchString));

        ChkDR(TST_OEM_CopyFile(dstrSource,dstrDestStore));
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function: TST_UTL_RestoreDeviceStore
**
** Synopsis: Deletes the existing test DeviceStore/DeviceCert and copies over
**           the ones provided as input. It is the responsibility of the caller
             to make sure that the devicestore/cert is not currently being used
             (possibly by calling drm_uninitialize before calling this function)
**
** Arguments:
**           IN  *f_pszDeviceStore -- pointer to a string holding the name of
                                      DeviceStore to be restored
**           IN  *f_pszDeviceCert  -- pointer to a string holding the name of
                                      DeviceCert to be restored
**Returns:
**          DRM_SUCCESS           -- On success
*****************************************************************************/
DRM_RESULT DRM_CALL TST_UTL_RestoreDeviceStore(DRM_CONST_STRING f_dstrDevStore, DRM_CONST_STRING f_dstrDevCert)
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_CONST_STRING dstrDest     = {0};
    DRM_CONST_STRING dstrDevCert = {0};
    DRM_CONST_STRING dstrDevStore = {0};
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

    ChkArg((f_dstrDevStore.pwszString != NULL) || (f_dstrDevCert.pwszString != NULL));
    ChkArg(g_dstrDrmPath.cchString > 0);

    if(f_dstrDevCert.pwszString != NULL && f_dstrDevCert.cchString > 0)
    {
        RemoveDRMFile(RMFILE_DEVCERT);
        ChkDR( tGetDeviceCertPathname(wszPathName, &dstrDest) );

        dstrDevCert.cchString = f_dstrDevCert.cchString + g_dstrDrmPath.cchString + 1;
        ChkMem(dstrDevCert.pwszString = (DRM_WCHAR *) Oem_MemAlloc((dstrDevCert.cchString) * SIZEOF(DRM_WCHAR)));
        ChkDR(DRM_STR_StringCchCopyNW((DRM_WCHAR *)dstrDevCert.pwszString, dstrDevCert.cchString,g_dstrDrmPath.pwszString, g_dstrDrmPath.cchString));
        ChkDR(DRM_STR_StringCchCatNW((DRM_WCHAR *)dstrDevCert.pwszString,dstrDevCert.cchString,f_dstrDevCert.pwszString, f_dstrDevCert.cchString));

        ChkDR(TST_OEM_CopyFile(dstrDevCert, dstrDest));
    }

    if(f_dstrDevStore.pwszString  != NULL && f_dstrDevStore.cchString > 0)
    {
        RemoveDRMFile(RMFILE_STORE);
        ChkDR( tGetDeviceStorePathname(wszPathName, &dstrDest) );

        dstrDevStore.cchString = f_dstrDevStore.cchString + g_dstrDrmPath.cchString + 1;
        ChkMem(dstrDevStore.pwszString = (DRM_WCHAR *) Oem_MemAlloc((dstrDevStore.cchString) * SIZEOF(DRM_WCHAR)));
        ChkDR(DRM_STR_StringCchCopyNW((DRM_WCHAR *)dstrDevStore.pwszString, dstrDevStore.cchString,g_dstrDrmPath.pwszString, g_dstrDrmPath.cchString));
        ChkDR(DRM_STR_StringCchCatNW((DRM_WCHAR *)dstrDevStore.pwszString,dstrDevStore.cchString,f_dstrDevStore.pwszString, f_dstrDevStore.cchString));

        ChkDR(TST_OEM_CopyFile(dstrDevStore, dstrDest));
    }

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function: _WriteObjectToFile
**
** Synopsis: Writes a block of data (header object) to an output file.
**
** Arguments:   f_pbObj       : Pointer to data to be written.
**              f_cbObj       : Number of bytes to write.
**              f_hOutputFile : Handle to output file.
**
**Returns:
**          DRM_SUCCESS           -- On success
*****************************************************************************/
static DRM_RESULT _WriteObjectToFile(
    __in DRM_BYTE *f_pbObj,
    __in DRM_DWORD f_cbObj,
    __in OEM_FILEHDL f_hOutputFile )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  cbWritten = 0;

    if ( !Oem_File_Write( f_hOutputFile, (DRM_VOID *)f_pbObj, f_cbObj, &cbWritten )
        || cbWritten != f_cbObj )
    {
        ChkDR( DRM_E_FILE_WRITE_ERROR );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function: _GetNextObject
**
** Synopsis: Obtains the next object from the header provided.
**
** Arguments:   f_pbHeader : Pointer to header data.
**              f_cbHeader : Size of the remaining header data.
**              f_pcbObj   : Pointer to object read from header. [output]
**
**Returns:
**          DRM_SUCCESS           -- On success
*****************************************************************************/
static DRM_BYTE *_GetNextObject(
    __in_bcount( f_cbHeader ) DRM_BYTE  *f_pbHeader,
    __in                      DRM_DWORD  f_cbHeader,
    __out                     DRM_DWORD *f_pcbObj )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(f_pbHeader != NULL);
    ChkArg(f_pcbObj != NULL);

    *f_pcbObj = 0;

    if ( f_cbHeader < OBJ_SIZE_COMMON )
    {
        return NULL;
    }


    DRM_BYT_CopyBytes( f_pcbObj, 0, f_pbHeader, SIZEOF( DRM_GUID ), SIZEOF(DRM_DWORD) );

ErrorExit:
    return f_pbHeader;
}

/*****************************************************************************
** Function: _GrabV2Header
**
** Synopsis: Returns the size & pointer to the V2 header in the object.
**
** Arguments:   f_pbObj       : Pointer to the object.
**              f_cbObj       : Size of the object.
**              f_ppbV2Header : Pointer to the V2 header. [output]
**              f_cbV2Header  : Size of the V2 header. [output]
**
**Returns:
**          DRM_SUCCESS           -- On success
*****************************************************************************/
static DRM_RESULT _GrabV2Header(
    __in_bcount(f_cbObj)  DRM_BYTE  *f_pbObj,
    __in                  DRM_DWORD  f_cbObj,
    __out                 DRM_BYTE  **f_ppbV2Header,
    __out                 DRM_DWORD *f_cbV2Header )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pbObj != NULL && f_cbObj > 0 );
    ChkArg( f_ppbV2Header != NULL );
    ChkArg( f_cbV2Header != NULL );

    DRM_BYT_CopyBytes( f_cbV2Header, 0, f_pbObj, OBJ_SIZE_COMMON, SIZEOF(DRM_DWORD) );

    /*
    ** Size of the V2 header.
    */
    *f_ppbV2Header = f_pbObj + OBJ_SIZE_COMMON + SIZEOF( DRM_DWORD );

ErrorExit:

    return dr;
}

/*****************************************************************************
** Function: _ConvertV2ToPlayReady
**
** Synopsis: Converts a V2 WMDRM header into a Playready object.
**
** Arguments:   f_pbV2Header         : Pointer to the V2 header.
**              f_cbV2Header         : Size of the V2 header.
**              f_pwchSilentURL      : Pointer to the Silent LA URL.
**              f_cchSilentURL       : Size of the Silent LA URL.
**              f_pwchNonSilentURL   : Pointer to the Non-Silent LA URL.
**              f_cchNonSilentURL    : Size of the Non-Silent LA URL.
**              f_pwchDSID           : Pointer to the DSID.
**              f_cchDSID            : Size of the DSID.
**              f_ppbPlayReadyObject : Pointer to the PlayReady object created. [output]
**              f_pcbPlayReadyObject : Size of the PlayReady object created. [output]
**
**Returns:
**          DRM_SUCCESS           -- On success
*****************************************************************************/
static DRM_RESULT _ConvertV2ToPlayReady(
    __in_bcount(f_cbV2Header)               const DRM_BYTE  *f_pbV2Header,
    __in                                          DRM_DWORD  f_cbV2Header,
    __in_ecount_nz_opt( f_cchSilentURL ) const    DRM_WCHAR *f_pwchSilentURL,
    __in const                                    DRM_DWORD  f_cchSilentURL,
    __in_ecount_nz_opt( f_cchNonSilentURL ) const DRM_WCHAR *f_pwchNonSilentURL,
    __in const                                    DRM_DWORD  f_cchNonSilentURL,
    __in_ecount_nz_opt( f_cchDSID ) const         DRM_WCHAR *f_pwchDSID,
    __in const                                    DRM_DWORD  f_cchDSID,
    __out                                         DRM_BYTE  **f_ppbPlayReadyObject,
    __inout                                       DRM_DWORD *f_pcbPlayReadyObject )
{
    DRM_RESULT dr = DRM_SUCCESS;

    dr = Drm_PlayReadyObject_ConvertFromWmdrmHeader(
                                         f_pbV2Header,
                                         f_cbV2Header,
                                         f_pwchSilentURL,
                                         f_cchSilentURL,
                                         f_pwchNonSilentURL,
                                         f_cchNonSilentURL,
                                         f_pwchDSID,
                                         f_cchDSID,
                                         TRUE,
                                         NULL,
                                         0,
                                         NULL,
                                         f_pcbPlayReadyObject );

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( *f_ppbPlayReadyObject = (DRM_BYTE *) Oem_MemAlloc( *f_pcbPlayReadyObject ) );

        ChkDR( Drm_PlayReadyObject_ConvertFromWmdrmHeader(
                                               f_pbV2Header,
                                               f_cbV2Header,
                                               f_pwchSilentURL,
                                               f_cchSilentURL,
                                               f_pwchNonSilentURL,
                                               f_cchNonSilentURL,
                                               f_pwchDSID,
                                               f_cchDSID,
                                               TRUE,
                                               NULL,
                                               0,
                                               *f_ppbPlayReadyObject,
                                               f_pcbPlayReadyObject ) );
    }
    else
    {
        ChkDR( dr );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function: _WritePlayReadyObject
**
** Synopsis: Writes the PlayReady object into an output file.
**
** Arguments:   f_pbPlayReadyObj : Pointer to the PlayReady object to be written.
**              f_cbPlayReadyObj : Size of the PlayReady object to be written.
**              f_hOutputFile    : Handle to the output file.
**              f_pcbCurrHeader  : Size of the current header object. [output]
**
**Returns:
**          DRM_SUCCESS           -- On success
*****************************************************************************/
static DRM_RESULT _WritePlayReadyObject(
    __in    DRM_BYTE    *f_pbPlayReadyObj,
    __in    DRM_DWORD    f_cbPlayReadyObj,
    __in    OEM_FILEHDL  f_hOutputFile,
    __inout DRM_DWORD   *f_pcbCurrHeader )
{
    DRM_BYTE rgbPlayReadySystemId[] = { 0x10, 0x70, 0x63, 0xF4, 0xC3, 0x03, 0xCD, 0x42,
                                        0xB9, 0x32, 0xB4, 0x8A, 0xDF, 0x3A, 0x6A, 0x54 };
    DRM_DWORD  dwSystemVersion      = 1;
    DRM_RESULT dr                   = DRM_SUCCESS;
    DRM_INT64  cbSize               = DRM_I64LITERAL(0,0);

    ChkArg( f_pbPlayReadyObj != NULL );
    ChkArg( f_pcbCurrHeader != NULL );

    cbSize = DRM_I64( SIZEOF( DRM_GUID )
                    + SIZEOF( DRM_INT64 )
                    + SIZEOF( DRM_GUID )
                    + ( 2 * SIZEOF( DRM_DWORD ) )
                    + f_cbPlayReadyObj );

    ChkDR( _WriteObjectToFile( ( DRM_BYTE * )&ASFPlayReadyObject, SIZEOF( DRM_GUID ), f_hOutputFile ) );
    ChkDR( _WriteObjectToFile( ( DRM_BYTE * )&cbSize, SIZEOF( DRM_INT64 ), f_hOutputFile ) );
    ChkDR( _WriteObjectToFile( rgbPlayReadySystemId, SIZEOF( rgbPlayReadySystemId ), f_hOutputFile ) );
    ChkDR( _WriteObjectToFile( ( DRM_BYTE * )&dwSystemVersion, SIZEOF( DRM_DWORD ), f_hOutputFile ) );
    ChkDR( _WriteObjectToFile( ( DRM_BYTE * )&f_cbPlayReadyObj, SIZEOF( DRM_DWORD ), f_hOutputFile ) );
    ChkDR( _WriteObjectToFile( f_pbPlayReadyObj, f_cbPlayReadyObj, f_hOutputFile ) );

    ( *f_pcbCurrHeader ) += DRM_I64ToUI32( cbSize );

ErrorExit:

    return dr;
}

/*****************************************************************************
** Function: _WriteStreamPropertyObject
**
** Synopsis: Writes the stream properties object into an output file.
**
** Arguments:   f_pbObj         : Pointer to the Stream properties object.
**              f_cbObj         : Size of the stream properties object.
**              f_hOutputFile   : Handle to the output file.
**              f_pcbCurrHeader : Size of the current header object. [output]
**
**Returns:
**          DRM_SUCCESS           -- On success
*****************************************************************************/
static DRM_RESULT _WriteStreamPropertyObject(
    __in_bcount (f_cbObj ) const DRM_BYTE    *f_pbObj,
    __in                         DRM_DWORD    f_cbObj,
    __in                         OEM_FILEHDL  f_hOutputFile,
    __inout                      DRM_DWORD   *f_pcbCurrHeader )
{
    DRM_RESULT      dr                                = DRM_SUCCESS;
    ASF_STREAM_TYPE eStreamType                       = STREAM_TYPE_UNKNOWN;
    DRM_BYTE        rgbTypeData[ MAX_TYPE_DATA_SIZE ] = {0};
    DRM_DWORD       cbTypeData                        = 0;
    DRM_DWORD       cbECC                             = 0;
    DRM_DWORD       cbExtra                           = 0;
    DRM_BYTE        *pbNewObj                         = NULL;
    DRM_DWORD       cbNewObj                          = 0;
    DRM_DWORD       dwHeaderOffset                    = SIZEOF( DRM_GUID ) +
                                                        SIZEOF( DRM_INT64 );
    DRM_DWORD       dwCommonOffset                    = dwHeaderOffset +
                                                        SIZEOF( DRM_GUID ) +
                                                        SIZEOF( DRM_GUID ) +
                                                        SIZEOF( DRM_INT64 );
    DRM_DWORD       dwTypeDataLenOffset               = dwCommonOffset;
    DRM_DWORD       dwECCLenOffset                    = dwTypeDataLenOffset +
                                                        SIZEOF( DRM_DWORD );
    DRM_DWORD       dwFlagOffset                      = dwECCLenOffset +
                                                        SIZEOF( DRM_DWORD );
    DRM_DWORD       dwTypeDataOffset                  = dwECCLenOffset +
                                                        SIZEOF( DRM_DWORD ) +
                                                        SIZEOF( DRM_WORD ) +
                                                        SIZEOF( DRM_DWORD );
    DRM_DWORD       dwTempVariable                    = 0;
    DRM_WORD        wTempVariable                     = 0;

    ChkArg( f_pbObj != NULL && f_cbObj > 0 );
    ChkArg( f_pcbCurrHeader != NULL );

    /*
    ** Figure out the type of stream(s) from the header
    */
    if ( MEMCMP( f_pbObj + OBJ_SIZE_COMMON, &ASFStreamTypeAudioMedia, SIZEOF( DRM_GUID ) ) == 0 )
    {
        eStreamType = STREAM_TYPE_AUDIO;
    }
    else if ( MEMCMP( f_pbObj + OBJ_SIZE_COMMON, &ASFStreamTypeVideoMedia, SIZEOF( DRM_GUID ) ) == 0 )
    {
        eStreamType = STREAM_TYPE_VIDEO;
    }
    else if ( MEMCMP( f_pbObj + OBJ_SIZE_COMMON, &ASFStreamTypeCommandMedia, SIZEOF( DRM_GUID ) ) == 0 )
    {
        eStreamType = STREAM_TYPE_SCRIPT;
    }
    else
    {
        ChkDR( DRM_E_ASF_BAD_ASF_HEADER );
    }

    DRM_BYT_CopyBytes( &cbTypeData, 0, f_pbObj, dwTypeDataLenOffset, SIZEOF(DRM_DWORD) );
    DRM_BYT_CopyBytes( &cbECC, 0, f_pbObj, dwECCLenOffset, SIZEOF(DRM_DWORD) );

    ChkBOOL( cbTypeData <= MAX_TYPE_DATA_SIZE, DRM_E_FAIL );

    MEMCPY( rgbTypeData, f_pbObj + dwTypeDataOffset, cbTypeData );

    if ( eStreamType == STREAM_TYPE_AUDIO )
    {
        /*
        ** Calculate the size of the type specific data field
        */
        DRM_DWORD dwSizeOffset = SIZEOF( DRM_WORD ) +
                                 SIZEOF( DRM_WORD ) +
                                 SIZEOF( DRM_DWORD ) +
                                 SIZEOF( DRM_DWORD ) +
                                 SIZEOF( DRM_WORD ) +
                                 SIZEOF( DRM_WORD );

        /*
        ** Size of any extra format information appended to the end of the type specific data field
        */
        cbExtra = SIZEOF( DRM_WORD );

        MEMCPY( rgbTypeData + cbTypeData, rgbTypeData, cbExtra );

        /*
        ** Set the Codec ID/Format Tag field
        */
        *( ( DRM_WORD * )rgbTypeData ) = 0x5052;

        DRM_BYT_CopyBytes( &dwTempVariable, 0, rgbTypeData, dwSizeOffset, SIZEOF(DRM_DWORD) );
        dwTempVariable += cbExtra;
        DRM_BYT_CopyBytes( rgbTypeData, dwSizeOffset, &dwTempVariable, 0, SIZEOF(DRM_DWORD) );
    }
    else if ( eStreamType == STREAM_TYPE_VIDEO )
    {
        /*
        ** Calculate the size of the type specific data field
        */
        DRM_DWORD dwSize1Offset = SIZEOF( DRM_DWORD ) +
                                  SIZEOF( DRM_DWORD ) +
                                  1;
        DRM_DWORD dwSize2Offset = dwSize1Offset +
                                  SIZEOF( DRM_WORD );
        DRM_DWORD dwCodecOffset = dwSize2Offset +
                                  4 * SIZEOF( DRM_DWORD );

        /*
        ** Size of any extra format information appended to the end of the type specific data field
        */
        cbExtra = SIZEOF( DRM_DWORD );

        MEMCPY( rgbTypeData + cbTypeData, rgbTypeData + dwCodecOffset, cbExtra );

        /*
        ** Set the biCompression fielf to 'PRDY'
        */
        MEMCPY( rgbTypeData + dwCodecOffset, "PRDY", cbExtra );

        DRM_BYT_CopyBytes( &dwTempVariable, 0, rgbTypeData, dwSize1Offset, SIZEOF(DRM_DWORD) );
        dwTempVariable += cbExtra;
        DRM_BYT_CopyBytes( rgbTypeData, dwSize1Offset, &dwTempVariable, 0, SIZEOF(DRM_DWORD) );
        DRM_BYT_CopyBytes( &dwTempVariable, 0, rgbTypeData, dwSize2Offset, SIZEOF(DRM_DWORD) );
        dwTempVariable += cbExtra;
        DRM_BYT_CopyBytes( rgbTypeData, dwSize2Offset, &dwTempVariable, 0, SIZEOF(DRM_DWORD) );
    }
    else if ( eStreamType == STREAM_TYPE_SCRIPT )
    {
        /*
        ** Calculate the offset for the additional type specific data to be added
        ** to the Stream Properties object.
        */
        DRM_DWORD dwFormatTypeOffset     = SIZEOF( DRM_GUID ) +
                                           SIZEOF( DRM_GUID ) +
                                           3 * SIZEOF( DRM_DWORD );

        DRM_DWORD dwFormatDataSizeOffset = dwFormatTypeOffset +
                                           SIZEOF( DRM_GUID );

        DRMASSERT( cbTypeData == 0 );

        cbExtra = dwFormatDataSizeOffset + SIZEOF( DRM_DWORD ) + DEFAULT_FORMAT_DATA_SIZE;

        ChkBOOL( cbExtra <= MAX_TYPE_DATA_SIZE, DRM_E_FAIL );

        ZEROMEM( rgbTypeData, cbExtra );

        /*
        ** Change the Stream type from ASF_Command_Media to ASF_BINARY_MEDIA.
        */
        MEMCPY( rgbTypeData, &ASFStreamTypeBinaryMedia, SIZEOF( DRM_GUID ) );

        /*
        ** Add the format type to be ASFStreamTypePlayReadyEncryptedCommandMedia.
        */
        MEMCPY( rgbTypeData + dwFormatTypeOffset,
                &ASFStreamTypePlayReadyEncryptedCommandMedia,
                SIZEOF( DRM_GUID ) );

        dwTempVariable = DEFAULT_FORMAT_DATA_SIZE;
        DRM_BYT_CopyBytes( rgbTypeData, dwFormatDataSizeOffset, &dwTempVariable, 0, SIZEOF(DRM_DWORD) );
    }

    ChkMem( pbNewObj = (DRM_BYTE *) Oem_MemAlloc( f_cbObj + cbExtra ) );

    cbNewObj = f_cbObj + cbExtra;

    MEMCPY( pbNewObj, f_pbObj, f_cbObj );

    if ( eStreamType == STREAM_TYPE_SCRIPT )
    {
        MEMCPY( pbNewObj + dwHeaderOffset,
                &ASFStreamTypeBinaryMedia,
                SIZEOF( DRM_GUID ) );
   }

   DRM_BYT_CopyBytes( &dwTempVariable, 0, pbNewObj, dwTypeDataLenOffset, SIZEOF(DRM_DWORD) );
   dwTempVariable += cbExtra;
   DRM_BYT_CopyBytes( pbNewObj, dwTypeDataLenOffset, &dwTempVariable, 0, SIZEOF(DRM_DWORD) );

   /*
   ** Clear the "Encrypted" bit.
   */
   DRM_BYT_CopyBytes( &wTempVariable, 0, pbNewObj, dwFlagOffset, SIZEOF(DRM_WORD) );

   /*
   ** Prevent endian mismatch.
   */
   FIX_ENDIAN_WORD( wTempVariable );
   wTempVariable &= 0x7f;
   FIX_ENDIAN_WORD( wTempVariable );

   DRM_BYT_CopyBytes( pbNewObj, dwFlagOffset, &wTempVariable, 0, SIZEOF(DRM_WORD) );

   if ( cbECC > 0 )
   {
       MEMMOVE( pbNewObj + dwTypeDataOffset + cbTypeData + cbExtra,
                pbNewObj + dwTypeDataOffset + cbTypeData,
                cbECC );
   }

   MEMCPY( pbNewObj + dwTypeDataOffset,
           rgbTypeData,
           cbTypeData + cbExtra );

   DRM_BYT_CopyBytes( &dwTempVariable, 0, pbNewObj, SIZEOF( DRM_GUID ), SIZEOF(DRM_DWORD) );
   dwTempVariable += cbExtra;
   DRM_BYT_CopyBytes( pbNewObj, SIZEOF( DRM_GUID ), &dwTempVariable, 0, SIZEOF(DRM_DWORD) );

   ChkDR( _WriteObjectToFile( pbNewObj, cbNewObj, f_hOutputFile ) );

   ( *f_pcbCurrHeader ) += cbNewObj;

ErrorExit:

    SAFE_OEM_FREE( pbNewObj );

    return dr;
}

/*****************************************************************************
** Function: _ConvertHeader
**
** Synopsis: Converts a WMDRM header into a Playready object.
**
** Arguments:   f_pbHeader          : Pointer to the V2 header.
**              f_cbHeader          : Size of the V2 header.
**              f_hOutputFile       : Handle to the output file.
**              f_pdstrSilentURL    : Pointer to the Silent LA URL.
**              f_pdstrNonSilentURL : Pointer to the Non-Silent LA URL.
**              f_pdstrDSID         : Pointer to the DSID.
**              f_pcbCurrHeader     : Size of the current header object. [output]
**              f_pdwObjCount       : Count of the number of objects. [output]
**
**Returns:
**          DRM_SUCCESS           -- On success
*****************************************************************************/
static DRM_RESULT _ConvertHeader(
    __in_bcount( f_cbHeader ) DRM_BYTE         *f_pbHeader,
    __in                      DRM_DWORD         f_cbHeader,
    __in                      OEM_FILEHDL       f_hOutputFile,
    __in                const DRM_CONST_STRING *f_pdstrSilentURL,
    __in                const DRM_CONST_STRING *f_pdstrNonSilentURL,
    __in                const DRM_CONST_STRING *f_pdstrDSID,
    __inout                   DRM_DWORD        *f_pcbCurrHeader,
    __inout                   DRM_DWORD        *f_pdwObjCount )
{
    DRM_RESULT   dr                 = DRM_SUCCESS;
    DRM_BYTE    *pbHeader           = f_pbHeader;
    DRM_DWORD    cbHeader           = f_cbHeader;
    DRM_BYTE    *pbObj              = NULL;
    DRM_DWORD    cbObj              = 0;
    DRM_BYTE    *pbPlayReadyObject  = NULL;

    ChkArg( f_pbHeader != NULL && f_cbHeader > 0 );
    ChkArg( f_pdstrSilentURL != NULL );
    ChkArg( f_pdstrNonSilentURL != NULL );
    ChkArg( f_pdstrDSID != NULL );
    ChkArg( f_pcbCurrHeader != NULL );
    ChkArg( f_pdwObjCount != NULL );

    while ( ( pbObj = _GetNextObject( pbHeader, cbHeader, &cbObj ) ) != NULL )
    {
        if ( MEMCMP( pbObj, &ASFContentEncryptionObjectEx, SIZEOF( DRM_GUID ) ) == 0 )
        {
            DRM_BYTE *pbV2Header = NULL;
            DRM_DWORD cbV2Header = 0;
            DRM_DWORD cbPlayReadyObject = 0;

            ChkDR( _GrabV2Header( pbObj, cbObj, &pbV2Header, &cbV2Header ) );

            SAFE_OEM_FREE( pbPlayReadyObject );
            ChkDR( _ConvertV2ToPlayReady( pbV2Header,
                                          cbV2Header,
                                          f_pdstrSilentURL->pwszString,
                                          f_pdstrSilentURL->cchString,
                                          f_pdstrNonSilentURL->pwszString,
                                          f_pdstrNonSilentURL->cchString,
                                          f_pdstrDSID->pwszString,
                                          f_pdstrDSID->cchString,
                                          &pbPlayReadyObject,
                                          &cbPlayReadyObject ) );

            ( *f_pcbCurrHeader ) -= cbObj;

            ChkDR( _WritePlayReadyObject( pbPlayReadyObject,
                                          cbPlayReadyObject,
                                          f_hOutputFile,
                                          f_pcbCurrHeader ) );
        }
        else if ( MEMCMP( pbObj, &ASFContentEncryptionObject, SIZEOF( DRM_GUID ) ) == 0 )
        {
            ( *f_pcbCurrHeader ) -= cbObj;
            ( *f_pdwObjCount )--;
        }
        else if ( MEMCMP( pbObj, &ASFStreamPropertiesObject, SIZEOF( DRM_GUID ) ) == 0 )
        {
            ( *f_pcbCurrHeader ) -= cbObj;

            ChkDR( _WriteStreamPropertyObject( pbObj,
                                               cbObj,
                                               f_hOutputFile,
                                               f_pcbCurrHeader ) );
        }
        else if ( MEMCMP( pbObj, &ASFDigitalSignatureObject, SIZEOF( DRM_GUID ) ) == 0 )
        {
            ( *f_pcbCurrHeader ) -= cbObj;
            ( *f_pdwObjCount )--;
        }
        else
        {
            ChkDR( _WriteObjectToFile( pbObj, cbObj, f_hOutputFile ) );
        }

        pbHeader += cbObj;
        cbHeader -= cbObj;
    }

ErrorExit:
    SAFE_OEM_FREE( pbPlayReadyObject );
    return dr;
}

/*****************************************************************************
** Function: _ConvertASFHeader
**
** Synopsis: Converts an ASF header into a PlayReady object.
**
** Arguments:   f_pdstrInput        : Pointer to the input file.
**              f_pdstrOutput       : Pointer to the output file.
**              f_pdstrSilentURL    : Pointer to the Silent LA URL.
**              f_pdstrNonSilentURL : Pointer to the Non-Silent LA URL.
**              f_pdstrDSID         : Pointer to the DSID.
**              f_pcbOldHeader      : Size of the old ASF header. [output]
**              f_pcbNewHeader      : Size of the new PlayReady object. [output]
**              f_pdwNewObjCount    : Count of the number of new objects. [output]
**
**Returns:
**          DRM_SUCCESS           -- On success
*****************************************************************************/
static DRM_RESULT _ConvertASFHeader(
    __in  const DRM_CONST_STRING *f_pdstrInput,
    __in  const DRM_CONST_STRING *f_pdstrOutput,
    __in  const DRM_CONST_STRING *f_pdstrSilentURL,
    __in  const DRM_CONST_STRING *f_pdstrNonSilentURL,
    __in  const DRM_CONST_STRING *f_pdstrDSID,
    __out       DRM_DWORD        *f_pcbOldHeader,
    __out       DRM_DWORD        *f_pcbNewHeader,
    __out       DRM_DWORD        *f_pdwNewObjCount )
{
    DRM_RESULT  dr          = DRM_SUCCESS;
    OEM_FILEHDL hInputFile  = OEM_INVALID_HANDLE_VALUE;
    OEM_FILEHDL hOutputFile = OEM_INVALID_HANDLE_VALUE;
    DRM_BYTE    *pbHeader   = NULL;
    DRM_DWORD   cbHeader    = 0;
    DRM_DWORD   dwObjCount  = 0;
    DRM_DWORD   cbInputFile = 0;
    DRM_DWORD   cbRead      = 0;

    ChkDRMString ( f_pdstrInput );
    ChkDRMString ( f_pdstrOutput );
    ChkDRMString ( f_pdstrSilentURL );
    ChkArg( f_pcbOldHeader != NULL );
    ChkArg( f_pcbNewHeader != NULL );
    ChkArg( f_pdwNewObjCount != NULL );

    /*
    ** Open Input file for reading
    */
    hInputFile = Oem_File_Open( NULL,
                                f_pdstrInput->pwszString,
                                OEM_GENERIC_READ,
                                OEM_FILE_SHARE_READ,
                                OEM_OPEN_EXISTING,
                                OEM_ATTRIBUTE_NORMAL );

    ChkBOOL( hInputFile != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN );

    if ( !Oem_File_GetSize( hInputFile, &cbInputFile ) )
    {
        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    /*
    ** Verify that file is larger than max asf header size
    */
    ChkArg( MAX_ASF_HEADER_SIZE < cbInputFile );

    ChkMem( pbHeader = (DRM_BYTE *) Oem_MemAlloc( MAX_ASF_HEADER_SIZE ) );

    if ( !Oem_File_Read( hInputFile, pbHeader, MAX_ASF_HEADER_SIZE, &cbRead )
     ||   cbRead != MAX_ASF_HEADER_SIZE )
    {
        ChkDR( DRM_E_FILE_READ_ERROR );
    }

    ChkBOOL( pbHeader != NULL, DRM_E_FAIL );

    /*
    ** Get ASF header information
    */
    cbHeader = (DRM_DWORD) *(const DRM_WCHAR*)( pbHeader + SIZEOF( DRM_GUID ) );
    dwObjCount = (DRM_DWORD) *(const DRM_WCHAR*)( pbHeader + SIZEOF( DRM_GUID ) + SIZEOF( DRM_INT64 ) );

    *f_pcbOldHeader = cbHeader;

    /*
    ** Open Output File for writing
    */
    hOutputFile = Oem_File_Open( NULL,
                                 f_pdstrOutput->pwszString,
                                 OEM_GENERIC_WRITE,
                                 OEM_FILE_SHARE_READ,
                                 OEM_CREATE_ALWAYS,
                                 OEM_ATTRIBUTE_NORMAL );

    ChkBOOL( hOutputFile != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN );

    ChkDR( _WriteObjectToFile( pbHeader, ROOT_OBJ_SIZE, hOutputFile ) );

    ChkBOOL( cbHeader > ROOT_OBJ_SIZE, DRM_E_FAIL );

    *f_pcbNewHeader = cbHeader;
    *f_pdwNewObjCount = dwObjCount;

    ChkDR( _ConvertHeader( pbHeader + ROOT_OBJ_SIZE,
                           cbHeader - ROOT_OBJ_SIZE,
                           hOutputFile,
                           f_pdstrSilentURL,
                           f_pdstrNonSilentURL,
                           f_pdstrDSID,
                           f_pcbNewHeader,
                           f_pdwNewObjCount ) );

ErrorExit:

    SAFE_OEM_FREE( pbHeader );

    if ( hInputFile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hInputFile );
    }

    if ( hOutputFile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hOutputFile );
    }

    return dr;
}

/*****************************************************************************
** Function: _ConvertASFData
**
** Synopsis: Converts the data portion of a WMDRM file into a Playready file.
**
** Arguments:   f_pdstrInput    : Filename of V2 WMDRM file to be converted.
**              f_pdstrOutput   : Filename of expected PlayReady file after conversion.
**              f_cbOldHeader   : Size of the old ASF header.
**              f_cbNewHeader   : Size of the new PlayReady object.
**              f_dwNewObjCount : Count of the number of objects.
**
**Returns:
**          DRM_SUCCESS           -- On success
*****************************************************************************/
static DRM_RESULT _ConvertASFData(
    __in const DRM_CONST_STRING *f_pdstrInput,
    __in const DRM_CONST_STRING *f_pdstrOutput,
    __in       DRM_DWORD         f_cbOldHeader,
    __in       DRM_DWORD         f_cbNewHeader,
    __in       DRM_DWORD         f_dwNewObjCount )
{
    DRM_RESULT  dr                              = DRM_SUCCESS;
    OEM_FILEHDL hInputFile                      = NULL;
    OEM_FILEHDL hOutputFile                     = NULL;
    DRM_DWORD   cbRead                          = 0;
    DRM_DWORD   cbWritten                       = 0;
    DRM_BYTE    rgbBuffer[ MAX_TYPE_DATA_SIZE ] = { 0 };

    ChkDRMString( f_pdstrInput );
    ChkDRMString( f_pdstrOutput );

    /*
    ** Open Input file for reading
    */
    hInputFile = Oem_File_Open( NULL,
                                f_pdstrInput->pwszString,
                                OEM_GENERIC_READ,
                                OEM_FILE_SHARE_READ,
                                OEM_OPEN_EXISTING,
                                OEM_ATTRIBUTE_NORMAL );

    ChkBOOL( hInputFile != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN );

    /*
    ** Open Output File for writing
    */
    hOutputFile = Oem_File_Open( NULL,
                                 f_pdstrOutput->pwszString,
                                 OEM_GENERIC_WRITE,
                                 OEM_FILE_SHARE_READ,
                                 OEM_OPEN_EXISTING,
                                 OEM_ATTRIBUTE_NORMAL );

    ChkBOOL( hOutputFile != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN );

    Oem_File_SetFilePointer( hOutputFile, SIZEOF( DRM_GUID ), OEM_FILE_BEGIN, NULL );
    ChkDR( _WriteObjectToFile( ( DRM_BYTE * )&f_cbNewHeader, SIZEOF( DRM_DWORD ), hOutputFile ) );

    Oem_File_SetFilePointer( hOutputFile, SIZEOF( DRM_DWORD ), OEM_FILE_CURRENT, NULL );
    ChkDR( _WriteObjectToFile( ( DRM_BYTE * )&f_dwNewObjCount, SIZEOF( DRM_DWORD ), hOutputFile ) );

    /*
    ** Set the input and output file pointers to the end of the old and new header blocks
    ** respectively to copy the remaining data
    */
    Oem_File_SetFilePointer( hInputFile, (DRM_LONG)f_cbOldHeader, OEM_FILE_BEGIN, NULL );
    Oem_File_SetFilePointer( hOutputFile, (DRM_LONG)f_cbNewHeader, OEM_FILE_BEGIN, NULL );

    while ( TRUE )
    {
        if ( !Oem_File_Read( hInputFile, rgbBuffer, SIZEOF( rgbBuffer ), &cbRead ) )
        {
            ChkDR( DRM_E_FILE_READ_ERROR );
        }

        if ( cbRead == 0 )
        {
            break;
        }

        if ( !Oem_File_Write( hOutputFile, rgbBuffer, cbRead, &cbWritten ) ||
            cbWritten != cbRead )
        {
            ChkDR( DRM_E_FILE_WRITE_ERROR );
        }
    }

ErrorExit:

    if ( hInputFile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hInputFile );
    }

    if ( hOutputFile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hOutputFile );
    }

    return dr;
}


/*****************************************************************************
** Function: TST_UTL_IsConditionTrue
**
** Synopsis: Determines if one or more the conditions are TRUE.  The conditions are OR'd
**
** Arguments:   f_pfConditionTrue   : returns TRUE if conditions are met.
**              f_lConditionCount   : number of conditions
**              f_ppszConditions    : variable number of OR'd conditions. [Wow64 | XP | Vista | Win2K3 | Win2k8 | Win7]
**
** Returns:
**          DRM_SUCCESS           -- On success
*****************************************************************************/
DRM_RESULT TST_UTL_IsConditionTrue( __out DRM_BOOL *f_pfConditionTrue, __in long f_lConditionCount, __in_ecount( f_lConditionCount ) const char **f_ppszConditions )
{
    DRM_RESULT      dr                          = DRM_SUCCESS;
    DRM_BOOL        fNot                        = FALSE;
    DRM_BOOL        fConditionMet               = FALSE;
    DRM_LONG        iCondition                  = 0;
    const DRM_CHAR *pszCondition                = NULL;
    const DRM_CHAR *pszTRUE                     = "TRUE";
    const DRM_CHAR *pszFALSE                    = "FALSE";
    const DRM_CHAR *pszChk                      = "Chk";
    const DRM_CHAR *pszFre                      = "Fre";
    const DRM_CHAR *pszLicSyncSupport           = "LicenseSyncSupported";
    const DRM_CHAR *pszWMDRMSupport             = "WMDRMSupported";
    const DRM_CHAR *pszHandleCachingSupport     = "HandleCachingSupported";
    const DRM_CHAR *pszVIEWRIGHTSSupport        = "VIEWRIGHTSSupported";
    const DRM_CHAR *pszSYMOPTSupport            = "SYMOPTSupported";
    const DRM_CHAR *pszMeteringSupport          = "MeteringSupported";
    const DRM_CHAR *pszDomainSupport            = "DomainSupported";
    const DRM_CHAR *pszEnvelopeSupport          = "EnvelopeSupported";
    const DRM_CHAR *pszActivationSupport        = "ActivationSupported";
    const DRM_CHAR *pszSysTimeChangeSupport     = "SystemTimeChangeSupported";
    const DRM_CHAR *pszPRNDTxRxSupport          = "PRNDTxAndRxSupported";
    const DRM_CHAR *pszPRNDTxSupport            = "PRNDTxSupported";
    const DRM_CHAR *pszPRNDRxSupport            = "PRNDRxSupported";
    const DRM_CHAR *pszPRNDNoAssetTxAvailable   = "PRNDNoAssetTxAvailable";
    const DRM_CHAR *pszBBXLegacySupport         = "BBXLegacySupported";
    const DRM_CHAR *pszDeviceAssetsSupport      = "DeviceAssetsSupported";
    const DRM_CHAR *pszWMDRMNetSupport          = "WMDRMNetSupported";
    const DRM_CHAR *pszSecureHalSupport         = "SecureHalSupported";
    const DRM_CHAR *pszSecureHalPlayOnlySupport = "SecureHalPlayOnlySupported";

    const DRM_CHAR *pszLinux                    = "Linux";
#if defined (DBG)
    DRM_BOOL        fChk                    = TRUE;
#else
    DRM_BOOL        fChk                    = FALSE;
#endif

    ChkArg( f_pfConditionTrue != NULL );
    ChkArg( f_lConditionCount != 0 );
    ChkArg( f_ppszConditions != NULL );

    *f_pfConditionTrue = FALSE;

    for ( iCondition = 0; iCondition < f_lConditionCount; iCondition++ )
    {
        fConditionMet = FALSE;
        fNot = FALSE;

        ChkArg( f_ppszConditions[ iCondition ] != NULL );

        pszCondition = f_ppszConditions[ iCondition ];

        if ( *pszCondition == '!' )
        {
            fNot = TRUE;
            pszCondition++;
        }

        if ( 0 == strcmp( pszCondition, pszTRUE ) )
        {
            fConditionMet = TRUE;
        }
        else if ( 0 == strcmp( pszCondition, pszFALSE ) )
        {
            fConditionMet = FALSE;
        }
        else if ( 0 == strcmp( pszCondition, pszChk ) )
        {
            if ( fChk )
            {
                fConditionMet = TRUE;
            }
        }
        else if ( 0 == strcmp( pszCondition, pszFre ) )
        {
            if ( !fChk )
            {
                fConditionMet = TRUE;
            }
        }
        else if ( 0 == strcmp( pszCondition, pszLicSyncSupport ) )
        {
            fConditionMet = DRM_SNC_IsLicenseSyncSupported();
        }
        else if ( 0 == strcmp( pszCondition, pszWMDRMSupport ) )
        {
            fConditionMet = DRM_WMDRM_IsWMDRMSupported();
        }
        else if ( 0 == strcmp( pszCondition, pszHandleCachingSupport ) )
        {
            fConditionMet = DRM_BBX_IsHandleCachingSupported();
        }
        else if ( 0 == strcmp( pszCondition, pszVIEWRIGHTSSupport ) )
        {
            fConditionMet = DRM_VIEWRIGHTS_IsViewRightsSupported();
        }
        else if ( 0 == strcmp( pszCondition, pszSYMOPTSupport ) )
        {
            fConditionMet = DRM_SYMOPT_IsSymOptSupported();
        }
        else if ( 0 == strcmp( pszCondition, pszMeteringSupport ) )
        {
            fConditionMet = DRM_METERING_IsMeteringSupported();
        }
        else if ( 0 == strcmp( pszCondition, pszDomainSupport ) )
        {
            fConditionMet = DRM_DOMAIN_IsDomainSupported();
        }
        else if ( 0 == strcmp( pszCondition, pszPRNDTxRxSupport ) )
        {
            fConditionMet = DRM_PRND_IsPRNDTransmitterSupported() && DRM_PRND_IsPRNDReceiverSupported();
        }
        else if ( 0 == strcmp( pszCondition, pszPRNDTxSupport ) )
        {
            fConditionMet = DRM_PRND_IsPRNDTransmitterSupported();
        }
        else if ( 0 == strcmp( pszCondition, pszPRNDRxSupport ) )
        {
            fConditionMet = DRM_PRND_IsPRNDReceiverSupported();
        }
        else if ( 0 == strcmp( pszCondition, pszBBXLegacySupport ) )
        {
            fConditionMet = DRM_BBX_IsLegacyCryptoSupported();
        }
        else if ( 0 == strcmp( pszCondition, pszLinux) )
        {
            #if LINUX_BUILD == 1
                fConditionMet = TRUE;
            #else
                fConditionMet = FALSE;
            #endif
        }
        else if ( 0 == strcmp( pszCondition, pszEnvelopeSupport) )
        {
            /*
            ** DRMEnvelope needs to be stubbed, until then, hard code supported / not supported
            */
            #if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_IOS
                fConditionMet = FALSE;
            #else
                fConditionMet = TRUE;
            #endif
        }
        else if ( 0 == strcmp( pszCondition, pszSysTimeChangeSupport ) )
        {
            #if DRM_BUILD_PROFILE == DRM_BUILD_PROFILE_IOS && !DRM_TEST_USE_OFFSET_CLOCK
                fConditionMet = FALSE;
            #else
                fConditionMet = TRUE;
            #endif
        }
        else if( 0 == strcmp( pszCondition, pszActivationSupport ) )
        {
            fConditionMet = DRM_ACTIVATION_IsActivationSupported();
        }
        else if( 0 == strcmp( pszCondition, pszPRNDNoAssetTxAvailable ) )
        {
            /* L"prndnoassettx.exe" */
            DRM_WCHAR rgwchExeName[] = { WCHAR_CAST('p'), WCHAR_CAST('r'), WCHAR_CAST('n'), WCHAR_CAST('d'), WCHAR_CAST('n'), WCHAR_CAST('o'), WCHAR_CAST('a'), WCHAR_CAST('s'), WCHAR_CAST('s'), WCHAR_CAST('e'), WCHAR_CAST('t'), WCHAR_CAST('t'), WCHAR_CAST('x'), WCHAR_CAST('.'), WCHAR_CAST('e'), WCHAR_CAST('x'), WCHAR_CAST('e'), WCHAR_CAST('\0') };
            OEM_FILEHDL hExe = OEM_INVALID_HANDLE_VALUE;
            hExe = Oem_File_Open( NULL, rgwchExeName, OEM_GENERIC_READ, OEM_FILE_SHARE_READ, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL );
            if( hExe != OEM_INVALID_HANDLE_VALUE )
            {
                fConditionMet = TRUE;
                (DRM_VOID)Oem_File_Close( hExe );
            }
        }
        else if( 0 == strcmp( pszCondition, pszDeviceAssetsSupport ) )
        {
            fConditionMet = DRM_DEVICEASSETS_IsDeviceAssetsSupported();
        }
        else if( 0 == strcmp( pszCondition, pszWMDRMNetSupport ) )
        {
            fConditionMet = DRM_WMDRMNET_IsWmdrmnetSupported();
        }
        else if ( 0 == strcmp( pszCondition, pszSecureHalPlayOnlySupport ) )
        {
            #if DRM_SUPPORT_SECUREOEMHAL_PLAY_ONLY
                fConditionMet = TRUE;
            #else
                fConditionMet = FALSE;
            #endif
        }
        else if ( 0 == strcmp( pszCondition, pszSecureHalSupport ) )
        {
            #if DRM_SUPPORT_SECUREOEMHAL
                fConditionMet = TRUE;
            #else
                fConditionMet = FALSE;
            #endif
        }
        else
        {
            /*
            ** Chk Platform Specific stuff last
            */
             ChkDR( TST_OEM_IsConditionTrue( &fConditionMet, pszCondition ) );
        }

        if ( fConditionMet )
        {
            if ( !fNot )
            {
                *f_pfConditionTrue = TRUE;
                Log( "Info", "Condition %s was met", pszCondition );
            }
        }
        else if ( fNot )
        {
            *f_pfConditionTrue = TRUE;
            Log( "Info", "Condition !%s was met", pszCondition );
        }
    }


ErrorExit:
    return dr;
}


/*****************************************************************************
** Function: TST_UTL_ConvertFileFromWMDRMToPlayReady
**
** Synopsis: Converts a WMDRM file into a Playready file.
**
** Arguments:   f_pdstrInput        : Filename of V2 WMDRM file to be converted.
**              f_pdstrOutput       : Filename of expected PlayReady file after conversion.
**              f_pdstrSilentURL    : Silent LA URL.
**              f_pdstrNonSilentURL : Non silent LA URL.
**              f_pdstrDSID         : Domain service ID.
**
**Returns:
**          DRM_SUCCESS           -- On success
*****************************************************************************/
DRM_RESULT DRM_CALL TST_UTL_ConvertFileFromWMDRMToPlayReady(
    __in const DRM_CONST_STRING *f_pdstrInput,
    __in const DRM_CONST_STRING *f_pdstrOutput,
    __in const DRM_CONST_STRING *f_pdstrSilentURL,
    __in const DRM_CONST_STRING *f_pdstrNonSilentURL,
    __in const DRM_CONST_STRING *f_pdstrDSID )
{
    DRM_RESULT dr            = DRM_SUCCESS;
    DRM_DWORD  cbOldHeader   = 0;
    DRM_DWORD  cbNewHeader   = 0;
    DRM_DWORD  dwNewObjCount = 0;

    ChkDRMString( f_pdstrInput );
    ChkDRMString( f_pdstrOutput );
    ChkDRMString( f_pdstrSilentURL );

    /*
    ** Convert the V2 header into a PlayReady Object
    */
    ChkDR( _ConvertASFHeader( f_pdstrInput,
                              f_pdstrOutput,
                              f_pdstrSilentURL,
                              f_pdstrNonSilentURL,
                              f_pdstrDSID,
                              &cbOldHeader,
                              &cbNewHeader,
                              &dwNewObjCount ) );

    /*
    ** Copy the remaining data into the new PlayReady file
    */
    ChkDR( _ConvertASFData( f_pdstrInput,
                            f_pdstrOutput,
                            cbOldHeader,
                            cbNewHeader,
                            dwNewObjCount ) );

ErrorExit:
    return dr;
}


DRM_RESULT DRM_CALL TST_UTL_SetWMDRMCertCachedValues(
        __inout DRM_BB_CONTEXT *f_poBBContext )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_STRING  dstrDevCert = EMPTY_DRM_STRING;

    ChkArg( f_poBBContext != NULL );

    ChkDR( TST_UTL_GetWMDRMCert( &dstrDevCert,
                                 &f_poBBContext->CryptoContext ) );

    ChkDR( DRM_DCP_LoadPropertiesCache(
                    NULL,
                    (DRM_CONST_STRING*) &dstrDevCert,
                    NULL,
                    &f_poBBContext->CryptoContext,
                    &f_poBBContext->cachedCertValues ) );

ErrorExit:
    FREEDRMSTRING( dstrDevCert );

    return dr;
}


DRM_RESULT DRM_CALL TST_UTL_GetWMDRMCert(
    __inout     DRM_STRING          *f_pdstrDevCert,
    __inout     DRM_CRYPTO_CONTEXT  *f_pcontextCRYP
    )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT *poKeyfileContext = NULL;

    ChkArg(f_pdstrDevCert != NULL);

    ChkMem( poKeyfileContext = (DRM_KEYFILE_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_KEYFILE_CONTEXT) ) );

    ChkDR( DRM_KF_Initialize( NULL, poKeyfileContext ) );
    ChkDR( DRM_KF_Open( poKeyfileContext, TRUE, OEM_GENERIC_READ ) );
    ChkDR( DRM_KF_LoadFromFile( poKeyfileContext, TRUE ) );
    ChkDR( DRM_KF_Close( poKeyfileContext ) );

    f_pdstrDevCert->cchString = MAX_DEVICE_CERT_SIZE / SIZEOF(DRM_WCHAR);
    ChkMem( f_pdstrDevCert->pwszString = (DRM_WCHAR*) Oem_MemAlloc( MAX_DEVICE_CERT_SIZE ) );

    ChkDR( DRM_DDC_GetXMLDeviceCertificate(
                    NULL,
                    poKeyfileContext,
                    f_pdstrDevCert,
                    0,
                    f_pcontextCRYP ) );

ErrorExit:
    if( poKeyfileContext != NULL )
    {
        DRM_KF_Uninitialize( poKeyfileContext );
        SAFE_OEM_FREE( poKeyfileContext );
    }
    if( DRM_FAILED( dr ) && f_pdstrDevCert != NULL )
    {
        SAFE_OEM_FREE( f_pdstrDevCert->pwszString );
    }

    return dr;
}

DRM_RESULT DRM_CALL TST_UTL_GetPlayReadyCert(
    __out_bcount(*f_pcbBinaryDevCertChain) DRM_BYTE            *f_pBinaryDevCertChain,
    __inout                                DRM_DWORD           *f_pcbBinaryDevCertChain,
    __inout                                DRM_CRYPTO_CONTEXT  *f_pCryptoCtx
)
{
    DRM_RESULT           dr               = DRM_SUCCESS;
    DRM_RESULT           dr2              = DRM_SUCCESS;
    DRM_KEYFILE_CONTEXT *poKeyfileContext = NULL;

    ChkArg( f_pBinaryDevCertChain != NULL );
    ChkArg( f_pcbBinaryDevCertChain != NULL );

    ChkMem( poKeyfileContext = (DRM_KEYFILE_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_KEYFILE_CONTEXT) ) );

    ChkDR( DRM_KF_Initialize( NULL, poKeyfileContext ) );

    ChkDR( TST_KF_Open( poKeyfileContext, TRUE, OEM_GENERIC_READ ) );
    ChkDR( TST_KF_LoadFromFile( poKeyfileContext, TRUE ) );
    ChkDR( TST_DDC_GetBinaryDeviceCertificate( NULL,
                                               poKeyfileContext,
                                               f_pBinaryDevCertChain,
                                               f_pcbBinaryDevCertChain,
                                               DRM_DCP_VERIFY_DEVCERT,
                                               f_pCryptoCtx ));

ErrorExit:
    dr2 = DRM_KF_Close( poKeyfileContext );
    if( DRM_SUCCEEDED( dr ) )
    {
        dr = dr2;
    }
    if( poKeyfileContext != NULL )
    {
        DRM_KF_Uninitialize( poKeyfileContext );
        SAFE_OEM_FREE( poKeyfileContext );
    }

    SAFE_OEM_FREE( poKeyfileContext );

    return dr;
}

DRM_RESULT DRM_CALL TST_UTL_SetPlayReadyCertCachedValues(
    __inout DRM_BB_CONTEXT *f_poBBContext
)
{
    DRM_RESULT           dr               = DRM_SUCCESS;
    DRM_RESULT           dr2              = DRM_SUCCESS;
    DRM_BYTE            *pbBuffer         = NULL;
    DRM_DWORD            cbBuffer         = 0;
    DRM_KEYFILE_CONTEXT *poKeyfileContext = NULL;

    ChkArg( f_poBBContext != NULL );

    ChkMem( pbBuffer = ( DRM_BYTE * ) Oem_MemAlloc( __CB_DECL( MAX_DEVICE_CERT_SIZE ) ) );
    cbBuffer = __CB_DECL( MAX_DEVICE_CERT_SIZE );

    ChkMem( poKeyfileContext = (DRM_KEYFILE_CONTEXT*) Oem_MemAlloc( SIZEOF(DRM_KEYFILE_CONTEXT) ) );

    ChkDR( DRM_KF_Initialize( NULL, poKeyfileContext ) );

    ChkDR( TST_KF_Open( poKeyfileContext, TRUE, OEM_GENERIC_READ ) );
    ChkDR( TST_KF_LoadFromFile( poKeyfileContext, TRUE ) );
    ChkDR( TST_DDC_GetBinaryDeviceCertificate( NULL,
                                               poKeyfileContext,
                                               pbBuffer,
                                               &cbBuffer,
                                               DRM_DCP_VERIFY_DEVCERT,
                                               &f_poBBContext->CryptoContext ));

    ChkDR( DRM_BCert_LoadPropertiesCache( pbBuffer,
                                          cbBuffer,
                                          &f_poBBContext->cachedBCertValues,
                                          DRM_BBX_GetKeyHandleCallbacks( f_poBBContext ),
                                          f_poBBContext ) );
ErrorExit:
    dr2 = DRM_KF_Close( poKeyfileContext );
    if( DRM_SUCCEEDED( dr ) )
    {
        dr = dr2;
    }
    if( poKeyfileContext != NULL )
    {
        DRM_KF_Uninitialize( poKeyfileContext );
        SAFE_OEM_FREE( poKeyfileContext );
    }
    SAFE_OEM_FREE( pbBuffer );
    return dr;
}

/*****************************************************************************
** Function: TST_UTL_TryAssignBase64DecodedString
**
** Synopsis: Try to decode a string as base 64; if unsuccessful, set the result
**           as the input string.
**
** Arguments:   f_szSource     : The source ASCII string.
**              f_pcbBuffer    : The number of bytes of the resulting output.
**              f_ppbBuffer    : The output byte buffer.
**
**Returns:
**          DRM_SUCCESS           -- On success
**          DRM_E_*               -- On other error as defined in drmerrors.h
*****************************************************************************/
DRM_RESULT DRM_CALL TST_UTL_TryAssignBase64DecodedString(
    __in_z      DRM_CHAR   *f_szSource,
    __out       DRM_DWORD  *f_pcbBuffer,
    __deref_out DRM_BYTE  **f_ppbBuffer)
{
    DRM_RESULT    dr           = DRM_SUCCESS;
    DRM_DWORD     cbBuffer     = 0;
    DRM_BYTE     *pbBuffer     = NULL;
    DRM_SUBSTRING dasstrBuffer = EMPTY_DRM_SUBSTRING;

    ChkArg( f_szSource  != NULL );
    ChkArg( f_pcbBuffer != NULL );
    ChkArg( f_ppbBuffer != NULL );

    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( f_szSource ), &dasstrBuffer.m_cch ) );
    cbBuffer = CB_BASE64_DECODE( dasstrBuffer.m_cch );
    ChkMem( pbBuffer = (DRM_BYTE*) Oem_MemAlloc( cbBuffer*SIZEOF(DRM_BYTE) ) );
    if ( DRM_SUCCESS == DRM_B64_DecodeA( f_szSource,
                                         &dasstrBuffer,
                                         &cbBuffer,
                                         pbBuffer,
                                         0 ) )
    {
        *f_pcbBuffer = cbBuffer;
        *f_ppbBuffer = pbBuffer;
        pbBuffer = NULL;
    }
    else
    {
        *f_pcbBuffer= dasstrBuffer.m_cch;
        *f_ppbBuffer = (DRM_BYTE*) f_szSource;
    }

ErrorExit:
    SAFE_OEM_FREE(pbBuffer);
    return dr;
}

/*********************************************************
**
** Function: TST_UTL_CopyFileToDRMDirectoryA
**
** Synopsis: Copy a file to the DRM Directory
**
** Arguments:
**  f_pszFile -- File to copy.
**
** Returns     DRM_SUCCESS if the file was copied
**             Other error code mapped from Win32 error code
**
*********************************************************/
DRM_RESULT DRM_CALL TST_UTL_CopyFileToDRMDirectoryA( __in_z const DRM_CHAR *f_pszFile )
{
    DRM_RESULT       dr                                               = DRM_SUCCESS;
    DRM_CHAR         rgszRunDirectory[DRM_MAX_PATH]                   = { 0 };
    DRM_WCHAR        rgwszDRMDirectory[GET_DRM_FILE_NAME_PATH_LENGTH] = { 0 };
    DRM_CONST_STRING dstrSource                                       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrTarget                                       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrFileName                                     = EMPTY_DRM_STRING;

    ChkArg( f_pszFile != NULL );

    ChkDR( MakeDRMString( &dstrFileName, f_pszFile ) );
    ChkDR( TST_OEM_GetTestRunDirectory( rgszRunDirectory ) );

    ChkDR( TST_UTL_MakeExplicitPathString( rgszRunDirectory, f_pszFile, &dstrSource ) );
    ChkDR( tGetDRMFileName( rgwszDRMDirectory, &dstrTarget, dstrFileName.pwszString ) );

    ChkDR( TST_OEM_CopyFile( dstrSource, dstrTarget ) );

ErrorExit:

    FREEDRMSTRING( dstrSource );
    FREEDRMSTRING( dstrFileName );

    return dr;
}

/*********************************************************
**
** Function: TST_UTL_CopyFileFromDRMDirectoryA
**
** Synopsis: Copy a file from the DRM Directory
**
** Arguments:
**  f_pszFile -- File to copy.
**
** Returns     DRM_SUCCESS if the file was copied
**             Other error code mapped from Win32 error code
**
*********************************************************/
DRM_RESULT DRM_CALL TST_UTL_CopyFileFromDRMDirectoryA( __in_z const DRM_CHAR *f_pszFile )
{
    DRM_RESULT       dr                                               = DRM_SUCCESS;
    DRM_CHAR         rgszRunDirectory[DRM_MAX_PATH]                   = { 0 };
    DRM_WCHAR        rgwszDRMDirectory[GET_DRM_FILE_NAME_PATH_LENGTH] = { 0 };
    DRM_CONST_STRING dstrSource                                       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrTarget                                       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrFileName                                     = EMPTY_DRM_STRING;

    ChkArg( f_pszFile != NULL );

    ChkDR( MakeDRMString( &dstrFileName, f_pszFile ) );
    ChkDR( TST_OEM_GetTestRunDirectory( rgszRunDirectory ) );

    ChkDR( TST_UTL_MakeExplicitPathString( rgszRunDirectory, f_pszFile, &dstrTarget ) );
    ChkDR( tGetDRMFileName( rgwszDRMDirectory, &dstrSource, dstrFileName.pwszString ) );

    ChkDR( TST_OEM_CopyFile( dstrSource, dstrTarget ) );

ErrorExit:

    FREEDRMSTRING( dstrTarget );
    FREEDRMSTRING( dstrFileName );

    return dr;
}

EXIT_PKTEST_NAMESPACE_CODE;

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

