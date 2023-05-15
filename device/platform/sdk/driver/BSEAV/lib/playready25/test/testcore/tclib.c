/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <drmcommon.h>
#include <drmerror.h>
#include <drmallstrings.h>
#include <drmxmlparser.h>
#include <drmcompiler.h>
#include <drmmathsafe.h>
#include <tclib.h>
#include <apiParams.h>
#include <logger.h>
#include <tstutils.h>
#include <tOEMIMP.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PKTEST_NAMESPACE_CODE;

#define DRM_TEST_MAX_ARG_NAME_VALUE_PAIRS 10

#define NO_LOG                      "-nolog"
#define QUIET                       "-q"
#define RUN_BVT                     "-bvt"
#define RUN_FUNC                    "-func"
#define RUN_DRT                     "-drt"
#define RUN_PERF                    "-perf"
#define RUN_FUZZ                    "-fuzz"
#define SPECIFY_DRM_PATH            "-drmpath"
#define SPECIFY_FUZZED_FILE_PATH    "-fuzzpath"
#define SPECIFY_TEST_FILE_PATH      "-filepath"
#define NO_PROMPT                   "-noprompt"
#define NAME_VALUE_PAIR             "-namevalue"
#define LOGCOMMENT                  "LogComment"

#define BVT_END_LINE                "BVT ends here"
#define FUNC_END_LINE               "FUNC ends here"
#define DRT_END_LINE                "DRT ends here"

#define DRM_E_WIN32_ERROR_TOO_MANY_CMDS       ((DRM_RESULT)0x80070038L)


/*
** ----------------------------------------------------------------------------
** |    BEGIN LOOPING DEFINITIONS
** ----------------------------------------------------------------------------
*/
#define ENDOFLOOP          "EndOfLoop"
#define BEGIN_REPEAT_LOOP  "BeginRepeatLoop"
#define FOREACHITEMINFILE  "ForEachItemInFile"
#define MAX_NESTED_LOOPS   10
#define MAX_LFD_LINE_SIZE  512

typedef enum _ELOOPTYPE
{
    ELOOPTYPE_INVALID           = 0,
    ELOOPTYPE_TIME              = 1,
    ELOOPTYPE_REPEAT            = 2,
    ELOOPTYPE_FOREACHITEMINFILE = 3
} ELOOPTYPE;

typedef enum _ELOOPFILETYPE
{
    ELOOPFILETYPE_INVALID = 0,
    ELOOPFILETYPE_XML     = 1,
    ELOOPFILETYPE_TEXT    = 2
} ELOOPFILETYPE;

struct _LoopFileData
{
    ELOOPFILETYPE  eType;
    OEM_FILEHDL    hFile;
    DRM_CHAR       rgchFileLine[ MAX_LFD_LINE_SIZE ];
    DRM_CHAR      *pszXML;
    DRM_DWORD      iXMLNode;
    DRM_SUBSTRING  dsstrCurrentNode;
};

struct _LoopEntry
{
    long                  lBeginOfLoopFilePosition;
    long                  lCurrentLoop;
    enum _ELOOPTYPE       eLoopType;
    struct _LoopFileData  LFD;
};
/*
** ----------------------------------------------------------------------------
** |    END LOOPING DEFINITIONS
** ----------------------------------------------------------------------------
*/


/*
** ----------------------------------------------------------------------------
** |    BEGIN IF DEFINITIONS
** ----------------------------------------------------------------------------
*/
#define SKIPTESTIF      "SkipTestIf"
#define BEGINIF         "If"
#define BEGINELSE       "Else"
#define BEGINELSEIF     "ElseIf"
#define ENDIF           "EndIf"
#define MAX_NESTED_IFS  10

struct _IfLevelData
{
    DRM_BOOL fInIf;
    DRM_BOOL fInElse;
    DRM_BOOL fSkipLines;
    DRM_BOOL fConditionMet;
};
/*
** ----------------------------------------------------------------------------
** |    END IF DEFINITIONS
** ----------------------------------------------------------------------------
*/


static const DRM_WCHAR g_rgwchEmptyPath [] =
{
    ONE_WCHAR('\0', '\0')
};

DRM_CONST_STRING        g_dstrFuzzPath   = CREATE_DRM_STRING( g_rgwchEmptyPath );
extern DRM_CONST_STRING g_dstrFilePath; /* By default test files should be in the data path */

#define tGETTCFILELIST(x1,x2,x3)  tGetTCFileList(x1,x2,x3)
#define tFREEFILENAMELIST(x) tFreeFileNameList(x)

#if WINCE_TEST
    #define SUPPORTED_TEST_PLATFORMS {".tc.", ".ce.", NULL}
#elif PC_TEST
    #define SUPPORTED_TEST_PLATFORMS {".tc.", ".pc.", NULL}
#else
    /*
    ** assume PK_TEST is defined
    */
    #define SUPPORTED_TEST_PLATFORMS {".tc.", ".pk.", NULL}
#endif

#if DBG
DRM_BOOL g_fAssertThrown = FALSE;
#endif

DRM_RESULT g_drExamine = DRM_SUCCESS;
DRM_BOOL   g_fExamineDR = FALSE;

/*
** Hook for ChkDR to break when a specified result is found.
*/
void TST_ExamineDR( unsigned long lDRVal )
{
    if( g_fExamineDR && (DRM_RESULT) lDRVal == g_drExamine )
    {
        Log( "ExamineDR", "%s(%#X) encountered.", DRM_ERR_GetErrorNameFromCode( (DRM_RESULT)lDRVal, NULL ), lDRVal );
        DRMASSERT(FALSE);
    }
}

/*
** Perf - timing info
*/
DRM_UINT64 g_uiStartTime = DRM_UI64LITERAL( 0, 0 );
DRM_UINT64 g_uiDiffTime  = DRM_UI64LITERAL( 0, 0 );

static TModMapEntry DRM_CALL _GetModuleData(
    const DRM_CHAR     *f_szModuleName,
    const TModMapEntry *f_rgoModMap )
{
    int i = 0;
    while (f_rgoModMap[i].m_szModuleName && strcmp(f_rgoModMap[i].m_szModuleName, f_szModuleName))
        i++;

    return f_rgoModMap[i];
}

DRM_RESULT StartTestCase(
          long                lTCID,
    const char               *szTCName,
          ENUM_TESTCASE_TYPE  f_eType,
    const TModMapEntry       *f_rgoModMap )
{
    DRM_RESULT   dr           = DRM_SUCCESS;
    TModMapEntry oModMapEntry = EMPTY_MOD_MAP_ENTRY;
    DRM_DWORD    iModMapEntry = 0;

#if DBG
    g_fAssertThrown = FALSE;
#endif

    LogStart(lTCID, f_eType, szTCName);

    TST_OEM_LeakClear();

    if( f_rgoModMap == NULL )
    {
        Log( "Error", "Module map is null in StartTestCase.");
        dr = DRM_E_TEST_INVALIDARG;
        goto ErrorExit;
    }

    DRM_TST_FreeVarArray();
    oModMapEntry = f_rgoModMap[ iModMapEntry ];
    while( oModMapEntry.m_pfnPreTestCase != NULL )
    {
        Log("Info", "Calling PreTestCase for module %s", oModMapEntry.m_szModuleName);
        if ( DRM_FAILED( dr = oModMapEntry.m_pfnPreTestCase( lTCID, szTCName ) ) )
        {
            const char *szError         = NULL;

            szError = DRM_ERR_GetErrorNameFromCode( dr, NULL );
            Log("Trace", "PreTestCase for module %s failed with %s(%#X).", oModMapEntry.m_szModuleName, szError, dr);
            goto ErrorExit;
        }
        iModMapEntry++;
        oModMapEntry = f_rgoModMap[ iModMapEntry ];
    }
    START_TIMER( g_uiStartTime );
ErrorExit:
    return dr;
}


static DRM_RESULT _EndTestCase(
    __in       const DRM_DWORD            f_dwTCID,
    __in             enum _EDISPOSITION   f_eDisposition,
    __in_z     const DRM_CHAR            *f_szTCName,
    __in_z_opt const DRM_CHAR            *f_szTrace,
    __in       const TModMapEntry        *f_rgoModMap )
{
    DRM_RESULT   dr           = DRM_SUCCESS;
    TModMapEntry oModMapEntry = EMPTY_MOD_MAP_ENTRY;
    DRM_LONG     iModMapEntry = 0;

    STOP_TIMER( g_uiStartTime, g_uiDiffTime );

    if( f_rgoModMap == NULL )
    {
        Log( "Error", "Module map is null in EndTestCase." );
        f_eDisposition = EDISPOSITION_FAILED;
    }
    else
    {
        oModMapEntry = f_rgoModMap[ iModMapEntry ];

        /*
        ** Find the end
        */
        while ( oModMapEntry.m_pfnPostTestCase != NULL )
        {
            iModMapEntry++;
            oModMapEntry = f_rgoModMap[ iModMapEntry ];
        }

        /*
        ** Call PostTestCase in reverse order
        */
        if( iModMapEntry != 0 )
        {
            iModMapEntry--;
            oModMapEntry = f_rgoModMap[ iModMapEntry ];

            while( iModMapEntry >= 0 )
            {
                Log("Info", "Calling PostTestCase for module %s", oModMapEntry.m_szModuleName);
                if ( DRM_FAILED( dr = oModMapEntry.m_pfnPostTestCase( (long)f_dwTCID, f_szTCName ) ) )
                {
                    const DRM_CHAR *szError = DRM_ERR_GetErrorNameFromCode( dr, NULL );
                    Log( "Trace", "PostTestCase for module %s failed with %s(%#X).", oModMapEntry.m_szModuleName, szError, dr );
                    f_eDisposition = EDISPOSITION_FAILED;
                }
                iModMapEntry--;
                oModMapEntry = f_rgoModMap[ iModMapEntry ];
            }
        }
    }

    DRM_TST_FreeVarArray();
    TST_OEM_LeakScan();

#if DBG
    if( g_fAssertThrown )
    {
        Log("Trace", "Test case failed due to assert being thrown.");
        f_eDisposition = EDISPOSITION_FAILED;
    }
#endif

    LogEnd( f_eDisposition, g_uiDiffTime, f_dwTCID, f_szTCName, f_szTrace );
    return ( f_eDisposition == EDISPOSITION_FAILED )? DRM_E_FAIL : DRM_SUCCESS;
}


/************************************************
** Function: RunAPI
**
** Synopsis: Finds the API the list of APIs used and runs it
**
** Arguments:
**           [szAPIName]    -- the string name of the API to execute
**           [lParams]      -- the count of parameters to the API.
**           [pParams]      -- array of parameters for the API
**           [apiMap]       -- Pointer to an array of API map entries.
**
** Returns:
**           DRM_E_TEST_NOTIMPL -- if the API was not found
**           Whatever the API itsef returns.
**
***************************************************/
DRM_RESULT DRM_CALL RunAPI( const char *szAPIName,
                            long lParams,
                            __in_ecount(lParams) char **pParams,
                            const TAPIMapEntry *apiMap )
{
    int i = 0;
    while (apiMap[i].m_szAPI && strcmp(apiMap[i].m_szAPI, szAPIName))
        i++;

    if (apiMap[i].m_szAPI)
        return (apiMap[i].m_pAPI)(lParams, pParams);
    else
        return DRM_E_TEST_NOTIMPL;
}

/************************************************
** Function: RunAPIInModule
**
** Synopsis: Finds the API the list of modules used and runs it
**           If szModuleName is not NULL then it only looks at that module
**
** Arguments:
**           [szAPIName]    -- the string name of the API to execute
**           [szModuleName] -- The string name of the module containing the API.
**                             This can be NULL, in which case all modules will be searched.
**           [lParams]      -- the count of parameters to the API.
**           [pParams]      -- array of parameters for the API
**           [f_poModuleMap]-- Pointer to an array of module map entries.
**
** Returns:
**           DRM_E_TEST_NOTIMPL -- if the API was not found
**           Whatever the API itsef returns.
**
***************************************************/
static DRM_RESULT DRM_CALL RunAPIInModule(               const char          *szAPIName,
                                                         const char          *szModuleName,
                                                               long           lParams,
                                          __in_ecount(lParams) char         **pParams,
                                                         const TModMapEntry  *f_poModuleMap )
{
    DRM_RESULT   dr           = DRM_E_TEST_NOTIMPL;
    TModMapEntry oModMapEntry = EMPTY_MOD_MAP_ENTRY;
    DRM_DWORD    iModMapEntry = 0;
    ChkArg( f_poModuleMap != NULL );
    ChkArg( szAPIName     != NULL );

    if ( szModuleName != NULL )
    {
        oModMapEntry = _GetModuleData( szModuleName, f_poModuleMap );
        dr = RunAPI( szAPIName, lParams, pParams, oModMapEntry.m_oApiMap );
    }
    else
    {
        oModMapEntry = f_poModuleMap[iModMapEntry];
        while( oModMapEntry.m_oApiMap != NULL && dr == DRM_E_TEST_NOTIMPL )
        {
            dr = RunAPI( szAPIName, lParams, pParams, oModMapEntry.m_oApiMap );
            iModMapEntry++;
            oModMapEntry = f_poModuleMap[ iModMapEntry ];
        }
    }
ErrorExit:
    return dr;
}

#if DBG
DRM_VOID DRM_CALL RefTestTraceCallback( const DRM_CHAR* f_pszFormat, DRM_VA_LIST f_ArgList )
{
    if ( f_pszFormat != NULL )
    {
        vLog( "DevTrace", f_pszFormat, (va_list)f_ArgList );
    }
}

DRM_VOID DRM_CALL RefTestAssertCallback(
    __in       DRM_BOOL  fAssert,
    __in const DRM_CHAR *assertcmd,
    __in const DRM_CHAR *file,
    __in       DRM_LONG  line)
{
    if(!fAssert)
    {
        Oem_Debug_Trace("DRMASSERT %s failed at %s(%d)\r\n", assertcmd, file, line);
        g_fAssertThrown = TRUE;

        /*
        ** If there's a debugger attached, then it's safe to break
        */
        if( _IsDebuggerAttached() )
        {
#if _M_IX86
            __asm int 3
#endif /* _M_IX86 */
        }
    }
}
#endif /* DBG */

DRM_RESULT DRM_CALL LogTestAPI( TAPIParams APIParams )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_CHAR  *pszParams = NULL;
    DRM_DWORD  cchParams = 0;

#define SIZEOF_COMMA  2
#define SIZEOF_NULL   6
#define SIZEOF_PARENS 4
    DRM_LONG iParam = 0;

    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( APIParams.m_szAPIName ) + SIZEOF_PARENS, &cchParams ) );
    for( iParam = 0; iParam < APIParams.m_lParamCount; iParam++ )
    {
        if( APIParams.m_szParams[iParam] == NULL )
        {
            cchParams += SIZEOF_NULL;
        }
        else
        {
            DRM_DWORD cch = 0;
            ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( APIParams.m_szParams[iParam] ) + SIZEOF_COMMA, &cch ) );
            cchParams += cch;
        }
    }
    ChkMem( pszParams = ( DRM_CHAR * )Oem_MemAlloc( cchParams ) );
    ZEROMEM( pszParams, cchParams );
    DRM_STR_StringCchCatA( pszParams, cchParams, APIParams.m_szAPIName );
    DRM_STR_StringCchCatA( pszParams, cchParams, "(" );
    for( iParam = 0; iParam < APIParams.m_lParamCount; iParam++ )
    {
        if( iParam == 0 )
        {
            DRM_STR_StringCchCatA( pszParams, cchParams, " " );
        }
        else
        {
            DRM_STR_StringCchCatA( pszParams, cchParams, ", " );
        }
        if( APIParams.m_szParams[iParam] == NULL )
        {
            DRM_STR_StringCchCatA( pszParams, cchParams, "NULL" );
        }
        else
        {
            DRM_STR_StringCchCatA( pszParams, cchParams, APIParams.m_szParams[iParam] );
        }
    }
    DRM_STR_StringCchCatA( pszParams, cchParams, " )" );
    Log( "Running_API", pszParams );
#undef SIZEOF_COMMA
#undef SIZEOF_NULL
#undef SIZEOF_PARENS
ErrorExit:
    SAFE_OEM_FREE( pszParams );
    return dr;
}

DRM_RESULT DRM_CALL RunTestAPI( TAPIParams    APIParams,
                                DRM_BOOL     *pfFail,
                                DRM_BOOL     *pfSkipLines,
                                DRM_DWORD    *pdwLinesToSkip,
                                DRM_RESULT   *pdrOut,
                                __out_bcount_opt(cchMaxTraceMessage) DRM_CHAR  *pszTraceMessage,
                                DRM_DWORD     cchMaxTraceMessage,
                                DRM_BOOL      fNoSuccessLog,
                          const TModMapEntry *poModuleMap )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_RESULT drActual  = DRM_SUCCESS;

    ChkArg( pfFail != NULL );

    *pfFail = FALSE;
    if( pfSkipLines != NULL )
    {
        *pfSkipLines = FALSE;
    }
    if( pdwLinesToSkip != NULL )
    {
        *pdwLinesToSkip = 0;
    }

    drActual = RunAPIInModule(APIParams.m_szAPIName, NULL, APIParams.m_lParamCount, APIParams.m_szParams, poModuleMap);

    if( pszTraceMessage != NULL )
    {
        memset(pszTraceMessage, '\0', cchMaxTraceMessage);
    }
    /*
    ** api returns a value which is explicitly rejected.
    */
    if (APIParams.m_fExcept && (drActual == APIParams.m_drExpected))
    {
        const char *szMessageStart  = "API_Failure: api returns a value which is explicitly rejected. API ";
        const char *szMessageMiddle = " unexpectedly returns ";
        char        szTemp[33]      = {0};
        const char *szError         = NULL;
        DRM_DWORD   cchCopied       = 0;
        DRM_DWORD   cchCopy         = 0;

        szError = DRM_ERR_GetErrorNameFromCode( drActual, NULL );

        Log( "API_Failure", "API %s unexpectedly returns %s(%#X).", APIParams.m_szAPIName, szError, drActual );

        if( pszTraceMessage != NULL )
        {
            /*
            ** Build the Trace Message avoiding sprintf
            */
            cchCopy =( DRM_DWORD )  min( strlen( szMessageStart ), cchMaxTraceMessage - 1 );
            DRM_BYT_CopyBytes( pszTraceMessage, cchCopied, szMessageStart, 0, cchCopy );
            cchCopied += cchCopy;
            ChkOverflow( cchMaxTraceMessage, cchMaxTraceMessage - cchCopied);
            ChkOverflow( cchMaxTraceMessage - cchCopied, cchMaxTraceMessage - cchCopied - 1);
            cchCopy = ( DRM_DWORD ) min( strlen( APIParams.m_szAPIName ), cchMaxTraceMessage - cchCopied - 1 );
            DRM_BYT_CopyBytes( pszTraceMessage, cchCopied, APIParams.m_szAPIName, 0, cchCopy );
            cchCopied += cchCopy;

            ChkOverflow( cchMaxTraceMessage, cchMaxTraceMessage - cchCopied);
            ChkOverflow( cchMaxTraceMessage - cchCopied, cchMaxTraceMessage - cchCopied - 1);
            cchCopy = ( DRM_DWORD ) min( strlen( szMessageMiddle ), cchMaxTraceMessage - cchCopied - 1 );
            DRM_BYT_CopyBytes( pszTraceMessage, cchCopied, szMessageMiddle, 0, cchCopy );
            cchCopied += cchCopy;

            ChkOverflow( cchMaxTraceMessage, cchMaxTraceMessage - cchCopied);
            ChkOverflow( cchMaxTraceMessage - cchCopied, cchMaxTraceMessage - cchCopied - 1);
            cchCopy = ( DRM_DWORD ) min( strlen( szError ), cchMaxTraceMessage - cchCopied - 1 );
            DRM_BYT_CopyBytes( pszTraceMessage, cchCopied, szError, 0, cchCopy );
            cchCopied += cchCopy;

            ChkBOOL( cchCopied + 3 < cchMaxTraceMessage, DRM_E_FAIL )
            PUT_CHAR( pszTraceMessage, cchCopied, '(' );
            cchCopied++;
            PUT_CHAR( pszTraceMessage, cchCopied, '0' );
            cchCopied++;
            PUT_CHAR( pszTraceMessage, cchCopied, 'x' );
            cchCopied++;

            ZEROMEM( szTemp, SIZEOF( szTemp ) );
            cchCopy = TST_UTL_NumberToAnsiString( (DRM_DWORD)drActual, 16, 0, SIZEOF( szTemp ), &szTemp[0] );
            ChkBOOL( cchCopied + cchCopy < cchMaxTraceMessage, DRM_E_FAIL );
            DRM_BYT_CopyBytes( pszTraceMessage, cchCopied, szTemp, 0, cchCopy );
            cchCopied += cchCopy;

            ChkBOOL( cchCopied + 3 < cchMaxTraceMessage, DRM_E_FAIL );
            PUT_CHAR( pszTraceMessage, cchCopied, ')' );
            cchCopied++;
            PUT_CHAR( pszTraceMessage, cchCopied, '.' );
            cchCopied++;
            PUT_CHAR( pszTraceMessage, cchCopied, '\0' );
        }
        *pfFail = TRUE;

    /*
    ** api returns a value which does not match the expected.
    */
    }
    else if (APIParams.m_fSkipIfThisError && drActual == APIParams.m_drExpected)
    {
        /*
        ** We're ending the test case early
        */
        if( pfSkipLines != NULL )
        {
            *pfSkipLines = TRUE;
        }
        if( pdwLinesToSkip != NULL )
        {
            *pdwLinesToSkip = APIParams.m_dwSkipNumberOfLines;
        }
    }
    else if (!APIParams.m_fExcept && (!APIParams.m_fSkipIfThisError || DRM_FAILED(dr)) && drActual != APIParams.m_drExpected)
    {
        const char *szMessageStart   = "API_Failure: API ";
        const char *szMessageMiddle1 = " returns ";
        const char *szMessageMiddle2 = "). Expecting ";
        char        szTemp[33]       = {0};
        const char *szExpectedError  = NULL;
        const char *szActualError    = NULL;

        DRM_DWORD cchCopied    = 0;
        DRM_DWORD cchCopy      = 0;

        szExpectedError = DRM_ERR_GetErrorNameFromCode( APIParams.m_drExpected, NULL );
        szActualError   = DRM_ERR_GetErrorNameFromCode( drActual, NULL );

        Log("API_Failure", "API %s returns %s(%#X). Expecting %s(%#X).", APIParams.m_szAPIName, szActualError, drActual, szExpectedError, APIParams.m_drExpected);

        if( pszTraceMessage != NULL )
        {
            /*
            ** Build the Trace Message avoiding sprintf
            */
            cchCopy = ( DRM_DWORD ) min( strlen( szMessageStart ), cchMaxTraceMessage - 1 );
            DRM_BYT_CopyBytes( pszTraceMessage, cchCopied, szMessageStart, 0, cchCopy );
            cchCopied += cchCopy;
            ChkOverflow( cchMaxTraceMessage, cchMaxTraceMessage - cchCopied);
            ChkOverflow( cchMaxTraceMessage - cchCopied, cchMaxTraceMessage - cchCopied - 1);
            cchCopy = ( DRM_DWORD ) min( strlen( APIParams.m_szAPIName ), cchMaxTraceMessage - cchCopied - 1 );
            DRM_BYT_CopyBytes( pszTraceMessage, cchCopied, APIParams.m_szAPIName, 0, cchCopy );
            cchCopied += cchCopy;

            ChkOverflow( cchMaxTraceMessage, cchMaxTraceMessage - cchCopied);
            ChkOverflow( cchMaxTraceMessage - cchCopied, cchMaxTraceMessage - cchCopied - 1);
            cchCopy = ( DRM_DWORD ) min( strlen( szMessageMiddle1 ), cchMaxTraceMessage - cchCopied - 1 );
            DRM_BYT_CopyBytes( pszTraceMessage, cchCopied, szMessageMiddle1, 0, cchCopy );
            cchCopied += cchCopy;

            ChkOverflow( cchMaxTraceMessage, cchMaxTraceMessage - cchCopied);
            ChkOverflow( cchMaxTraceMessage - cchCopied, cchMaxTraceMessage - cchCopied - 1);
            cchCopy = ( DRM_DWORD ) min( strlen( szActualError ), cchMaxTraceMessage - cchCopied - 1 );
            DRM_BYT_CopyBytes( pszTraceMessage, cchCopied, szActualError, 0, cchCopy );
            cchCopied += cchCopy;

            ChkBOOL( cchCopied + 3 < cchMaxTraceMessage, DRM_E_FAIL )
            PUT_CHAR( pszTraceMessage, cchCopied, '(' );
            cchCopied++;
            PUT_CHAR( pszTraceMessage, cchCopied, '0' );
            cchCopied++;
            PUT_CHAR( pszTraceMessage, cchCopied, 'x' );
            cchCopied++;

            ZEROMEM( szTemp, SIZEOF( szTemp ) );
            cchCopy = TST_UTL_NumberToAnsiString( (DRM_DWORD)drActual, 16, 0, SIZEOF( szTemp ), &szTemp[0] );
            ChkBOOL( cchCopied + cchCopy < cchMaxTraceMessage, DRM_E_FAIL );
            DRM_BYT_CopyBytes( pszTraceMessage, cchCopied, szTemp, 0, cchCopy );
            cchCopied += cchCopy;

            ChkOverflow( cchMaxTraceMessage, cchMaxTraceMessage - cchCopied);
            ChkOverflow( cchMaxTraceMessage - cchCopied, cchMaxTraceMessage - cchCopied - 1);
            cchCopy = ( DRM_DWORD ) min( strlen( szMessageMiddle2 ), cchMaxTraceMessage - cchCopied - 1 );
            DRM_BYT_CopyBytes( pszTraceMessage, cchCopied, szMessageMiddle2, 0, cchCopy );
            cchCopied += cchCopy;

            ChkOverflow( cchMaxTraceMessage, cchMaxTraceMessage - cchCopied);
            ChkOverflow( cchMaxTraceMessage - cchCopied, cchMaxTraceMessage - cchCopied - 1);
            cchCopy = ( DRM_DWORD ) min( strlen( szExpectedError ), cchMaxTraceMessage - cchCopied - 1 );
            DRM_BYT_CopyBytes( pszTraceMessage, cchCopied, szExpectedError, 0, cchCopy );
            cchCopied += cchCopy;

            ChkBOOL( cchCopied + 3 < cchMaxTraceMessage, DRM_E_FAIL )
            PUT_CHAR( pszTraceMessage, cchCopied, '(' );
            cchCopied++;
            PUT_CHAR( pszTraceMessage, cchCopied, '0' );
            cchCopied++;
            PUT_CHAR( pszTraceMessage, cchCopied, 'x' );
            cchCopied++;

            ZEROMEM( szTemp, SIZEOF( szTemp ) );
            cchCopy = TST_UTL_NumberToAnsiString( (DRM_DWORD)APIParams.m_drExpected, 16, 0, SIZEOF( szTemp ), &szTemp[0] );
            ChkBOOL( cchCopied + cchCopy < cchMaxTraceMessage, DRM_E_FAIL );
            DRM_BYT_CopyBytes( pszTraceMessage, cchCopied, szTemp, 0, cchCopy );
            cchCopied += cchCopy;

            ChkBOOL( cchCopied + 3 < cchMaxTraceMessage, DRM_E_FAIL );
            PUT_CHAR( pszTraceMessage, cchCopied, ')' );
            cchCopied++;
            PUT_CHAR( pszTraceMessage, cchCopied, '.' );
            cchCopied++;
            PUT_CHAR( pszTraceMessage, cchCopied, '\0' );
        }
        *pfFail = TRUE;
    }

    if( pdrOut != NULL )
    {
        *pdrOut = drActual;
    }

ErrorExit:
    return dr;
}


/*
** ----------------------------------------------------------------------------
** |    BEGIN LOOPING FUNCTIONS
** ----------------------------------------------------------------------------
*/

static DRM_VOID DRM_CALL _CloseLoopFile (
    __in struct _LoopFileData    *f_pLFD )
{
    if ( f_pLFD != NULL )
    {
        if ( f_pLFD->hFile != OEM_INVALID_HANDLE_VALUE )
        {
            Oem_File_Close( f_pLFD->hFile );
            f_pLFD->hFile = OEM_INVALID_HANDLE_VALUE;
        }
    }
}


static DRM_RESULT DRM_CALL _GetNextItemInLoopFile (
    __in struct _LoopFileData    *f_pLFD )
{
    DRM_RESULT            dr            = DRM_SUCCESS;
    DRM_CHAR      const  *pchRead       = NULL;
    DRM_DWORD             cchLength     = 0;
    DRM_SUBSTRING         dsstrSS       = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dsstrTag      = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dsstrNodeData = EMPTY_DRM_SUBSTRING;


    ChkArg( f_pLFD != NULL );

    if ( f_pLFD->eType == ELOOPFILETYPE_XML  )
    {
        ChkBOOL( f_pLFD->pszXML != NULL, DRM_E_TEST_FILE_NOT_OPEN );
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( f_pLFD->pszXML ), &dsstrSS.m_cch ) );
        dsstrSS.m_ich = 0;
        f_pLFD->iXMLNode++;

        ChkDR( DRM_XML_EnumNextNodeA( f_pLFD->pszXML, &dsstrSS, 0, &dsstrTag, NULL, &dsstrNodeData, NULL, NULL ) );
        dr = DRM_XML_EnumNextNodeA( f_pLFD->pszXML, &dsstrNodeData, f_pLFD->iXMLNode, &dsstrTag, &(f_pLFD->dsstrCurrentNode), NULL, NULL, NULL );
        if ( ( dr == DRM_E_NO_XML_OPEN_TAG ) || ( dr == DRM_E_XMLNOTFOUND ) )
        {
            ChkDR( DRM_E_NOMORE );
        }
        ChkDR( dr );
    }
    else if ( f_pLFD->eType == ELOOPFILETYPE_TEXT )
    {
        ChkBOOL( f_pLFD->hFile != OEM_INVALID_HANDLE_VALUE, DRM_E_FILENOTFOUND );
        MEMSET( f_pLFD->rgchFileLine, '\0', MAX_LFD_LINE_SIZE );
        pchRead = fgets( f_pLFD->rgchFileLine, MAX_LFD_LINE_SIZE, (FILE *)f_pLFD->hFile );
        if( pchRead == NULL )
        {
            if( feof( (FILE *)f_pLFD->hFile ) )
            {
                ChkDR( DRM_E_NOMORE );
            }
            ChkDR( DRM_E_FILE_READ_ERROR );
        }
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( f_pLFD->rgchFileLine ) - 1, &cchLength ) );
        if ( f_pLFD->rgchFileLine[ cchLength ] == '\n' )
        {
            f_pLFD->rgchFileLine[ cchLength ] = '\0';
        }
    }
    else
    {
        ChkDR( DRM_E_TEST_UNSUPPORTED_FILE_FORMAT );
    }

ErrorExit:
    return dr;
}


static DRM_RESULT DRM_CALL _GetFirstItemInLoopFile (
    __in    const DRM_CHAR              *f_pszFileName,
    __inout       struct _LoopFileData  *f_pLFD )
{
    DRM_RESULT          dr           = DRM_SUCCESS;
    DRM_CONST_STRING    dstrFileName = EMPTY_DRM_STRING;
    DRM_DWORD           cbFile       = 0;
    DRM_DWORD           cbRead       = 0;
    DRM_DWORD           cchFileName  = 0;


    ChkArg( f_pszFileName != NULL );
    ChkArg( f_pLFD != NULL );

    ChkDR( MakeDRMString( &dstrFileName, f_pszFileName ) );

    ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( f_pszFileName ), &cchFileName ) );
    if ( DRMCRT_strnstr( f_pszFileName, cchFileName, ".xml", 4 ) != NULL  )
    {
        f_pLFD->eType = ELOOPFILETYPE_XML;
        f_pLFD->iXMLNode = MAX_UNSIGNED_TYPE(DRM_DWORD);
        f_pLFD->hFile = Oem_File_Open( NULL, dstrFileName.pwszString, OEM_GENERIC_READ, OEM_FILE_SHARE_READ, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL );
        if ( f_pLFD->hFile != OEM_INVALID_HANDLE_VALUE )
        {
            if ( Oem_File_GetSize( f_pLFD->hFile, &cbFile ) )
            {
                ChkMem( f_pLFD->pszXML = (DRM_CHAR *)Oem_MemAlloc( cbFile + 1 ) );
                ZEROMEM( f_pLFD->pszXML, ( cbFile + 1 ) );
                if ( !Oem_File_Read( f_pLFD->hFile, f_pLFD->pszXML, cbFile, &cbRead ) )
                {
                    Oem_MemFree( f_pLFD->pszXML );
                    f_pLFD->pszXML = NULL;
                    ChkDR( DRM_E_TEST_FILE_LOAD_ERROR );
                }
            }
            else
            {
                ChkDR( DRM_E_FILEIO_ERROR );
            }
            _CloseLoopFile( f_pLFD );
        }
        else
        {
            ChkDR( DRM_E_FILENOTFOUND );
        }
    }
    else if ( DRMCRT_strnstr( f_pszFileName, cchFileName, ".txt", 4 ) != NULL )
    {
        f_pLFD->eType = ELOOPFILETYPE_TEXT;
        f_pLFD->hFile = Oem_File_Open( NULL, dstrFileName.pwszString, OEM_GENERIC_READ, OEM_FILE_SHARE_READ, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL );
    }
    else
    {
        ChkDR( DRM_E_TEST_UNSUPPORTED_FILE_FORMAT );
    }

    ChkDR( _GetNextItemInLoopFile( f_pLFD ) );

ErrorExit:
    FREEDRMSTRING( dstrFileName );
    if ( DRM_FAILED( dr ) )
    {
        _CloseLoopFile( f_pLFD );
    }
    return dr;
}


static DRM_RESULT DRM_CALL _GetValueFromLoopData(
    __in const                  struct _LoopFileData  *f_pLFD,
    __in const                  DRM_CHAR              *f_pchToken,
    __out_bcount(f_cchLength)   DRM_CHAR              *f_pchConfigVar,
    __in const                  DRM_DWORD              f_cchLength )
{
    DRM_RESULT             dr                  = DRM_SUCCESS;
    DRM_ANSI_CONST_STRING  dacstrAttributeName = EMPTY_DRM_STRING;
    DRM_SUBSTRING          dsstrAttributeValue = EMPTY_DRM_SUBSTRING;


    ChkArg( f_pchToken       != NULL );
    ChkArg( f_pchConfigVar   != NULL );

    ChkBOOL( f_pLFD != NULL, DRM_E_NOT_FOUND );

    if ( f_pLFD->eType == ELOOPFILETYPE_XML  )
    {
        dacstrAttributeName.pszString = f_pchToken;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( f_pchToken ), &dacstrAttributeName.cchString ) );
        dr = DRM_XML_GetNodeAttributeA( f_pLFD->pszXML, &(f_pLFD->dsstrCurrentNode), &dacstrAttributeName, &dsstrAttributeValue );
        if ( dr == DRM_E_INVALID_XML_TAG )
        {
            /*
            ** support searching in node body when/if people start making XML files that way
            */
            ChkDR( DRM_E_NOT_FOUND );
        }
        ChkDR( dr );
        MEMCPY( f_pchConfigVar, f_pLFD->pszXML + dsstrAttributeValue.m_ich, MIN( f_cchLength, dsstrAttributeValue.m_cch ) );
    }
    else if ( f_pLFD->eType == ELOOPFILETYPE_TEXT )
    {
        const DRM_CHAR  *pchParam = NULL;
        const DRM_CHAR  *pchComma = NULL;
        const DRM_CHAR  *pchEqual = NULL;

        pchParam = f_pLFD->rgchFileLine;
        while ( ( pchParam = DRMCRT_strnstr( pchParam, MAX_LFD_LINE_SIZE, f_pchToken, MAX_LFD_LINE_SIZE ) ) != NULL )
        {
            pchEqual = DRMCRT_strnstr( pchParam, MAX_LFD_LINE_SIZE, "=", 1 );
            if ( pchEqual == NULL )
            {
                pchParam++;
                continue;
            }
            pchParam = pchEqual + 1;
            pchComma = DRMCRT_strnstr( pchParam, MAX_LFD_LINE_SIZE, ",", 1 );
            if ( pchComma != NULL )
            {
                DRM_DWORD cch = 0;
                ChkDR( DRM_DWordPtrToDWord( (DRM_DWORD_PTR)( (DRM_DWORD_PTR)pchComma - (DRM_DWORD_PTR)pchParam ), &cch ) );
                ChkDR( DRM_STR_StringCchCopyNA( f_pchConfigVar, f_cchLength, pchParam, cch ) );
                goto ErrorExit;
            }
            else
            {
                ChkDR( DRM_STR_StringCchCopyA( f_pchConfigVar, f_cchLength, pchParam ) );
                goto ErrorExit;
            }
        }

        pchParam = f_pLFD->rgchFileLine;
        if ( DRMCRT_strncmp( f_pchToken, "Param0", 6 ) == 0 )
        {
            ChkDR( DRM_STR_StringCchCopyA( f_pchConfigVar, f_cchLength, pchParam ) );
            goto ErrorExit;
        }

        ChkDR( DRM_E_NOT_FOUND );
    }
    else
    {
        ChkDR( DRM_E_TEST_UNSUPPORTED_FILE_FORMAT );
    }

ErrorExit:
    return dr;
}


DRM_RESULT DRM_CALL GetValueFromLoopData(
    __in const                 DRM_DWORD     f_iLoopLevel,
    __in const                 DRM_VOID     *f_prgLoopEntries,
    __in const                 DRM_CHAR     *f_pchToken,
    __out_bcount(f_cchLength)  DRM_CHAR     *f_pchConfigVar,
    __in const                 DRM_DWORD     f_cchLength )
{
    DRM_RESULT                dr             = DRM_SUCCESS;
    long                      iLevel         = -1;
    struct _LoopEntry const  *prgLoopEntries = (struct _LoopEntry const *)f_prgLoopEntries;


    ChkArg( f_prgLoopEntries != NULL );
    ChkArg( f_pchToken       != NULL );
    ChkArg( f_pchConfigVar   != NULL );

    for( iLevel = (long)f_iLoopLevel; iLevel > -1; iLevel-- )
    {
        dr = _GetValueFromLoopData( &(prgLoopEntries[ iLevel ].LFD), f_pchToken, f_pchConfigVar, f_cchLength );
        if( dr == DRM_SUCCESS )
        {
            break;
        }
        else if( dr == DRM_E_NOT_FOUND )
        {
            continue;
        }
        ChkDR( dr );
    }

ErrorExit:
    return dr;
}

/*
** ----------------------------------------------------------------------------
** |    END LOOPING FUNCTIONS
** ----------------------------------------------------------------------------
*/

/*
** ----------------------------------------------------------------------------
** |    BEGIN CONDITIONAL EXECUTION FUNCTIONS
** ----------------------------------------------------------------------------
*/
static DRM_RESULT DRM_CALL __CheckIFCondition(
    __in struct _IfLevelData    *f_prgIfData,
    __in        DRM_DWORD        f_iIfLevel,
    __in        TAPIParams      *f_pAPIParams )
{
    DRM_RESULT dr = DRM_SUCCESS;


    if ( f_prgIfData[ f_iIfLevel ].fConditionMet == FALSE )
    {
        ChkDR( TST_UTL_IsConditionTrue( &(f_prgIfData[ f_iIfLevel ].fConditionMet),
                                        f_pAPIParams->m_lParamCount,
                                        (const char **)f_pAPIParams->m_szParams ) );
        if ( f_prgIfData[ f_iIfLevel ].fConditionMet == TRUE )
        {
            f_prgIfData[ f_iIfLevel ].fSkipLines = FALSE;
            Log( "******", "If : One or more of the conditions were met" );
        }
    }
    else
    {
        f_prgIfData[ f_iIfLevel ].fSkipLines = TRUE;
    }

ErrorExit:
    return dr;
}


static DRM_RESULT DRM_CALL _BeginIF (
    __in struct _IfLevelData    *f_prgIfData,
    __in        DRM_DWORD       *f_piIfLevel,
    __in        TAPIParams      *f_pAPIParams )
{
    DRM_RESULT dr = DRM_SUCCESS;


    ChkDR( LogTestAPI( *f_pAPIParams ) );
    (*f_piIfLevel)++;
    ChkBOOL( *f_piIfLevel < MAX_NESTED_IFS, DRM_E_WIN32_ERROR_TOO_MANY_CMDS );

    f_prgIfData[ *f_piIfLevel ].fInIf         = TRUE;
    f_prgIfData[ *f_piIfLevel ].fInElse       = FALSE;
    f_prgIfData[ *f_piIfLevel ].fSkipLines    = TRUE;
    f_prgIfData[ *f_piIfLevel ].fConditionMet = FALSE;

    if ( f_prgIfData[ *f_piIfLevel - 1 ].fSkipLines == FALSE )  /* outer level not skipping lines */
    {
        ChkDR( __CheckIFCondition( f_prgIfData, *f_piIfLevel, f_pAPIParams ) );
    }

ErrorExit:
    return dr;
}


static DRM_RESULT DRM_CALL _BeginELSE (
    __in         struct _IfLevelData    *f_prgIfData,
    __in const   DRM_DWORD               f_iIfLevel )
{
    DRM_RESULT dr = DRM_SUCCESS;


    Log( "******", "Else, level %d", f_iIfLevel );
    ChkBOOL( f_prgIfData[ f_iIfLevel ].fInIf   == TRUE,  DRM_E_FAIL );
    ChkBOOL( f_prgIfData[ f_iIfLevel ].fInElse == FALSE, DRM_E_FAIL );

    if ( f_prgIfData[ f_iIfLevel - 1 ].fSkipLines == FALSE )  /* outer level not skipping lines */
    {
        f_prgIfData[ f_iIfLevel ].fSkipLines = f_prgIfData[ f_iIfLevel ].fConditionMet;
        f_prgIfData[ f_iIfLevel ].fInElse = TRUE;
    }

ErrorExit:
    return dr;
}


static DRM_RESULT DRM_CALL _BeginELSEIF (
    __in struct _IfLevelData    *f_prgIfData,
    __in        DRM_DWORD        f_iIfLevel,
    __in        TAPIParams      *f_pAPIParams )
{
    DRM_RESULT dr = DRM_SUCCESS;


    ChkDR( LogTestAPI( *f_pAPIParams ) );
    ChkBOOL( f_prgIfData[ f_iIfLevel ].fInIf   == TRUE,  DRM_E_FAIL );
    ChkBOOL( f_prgIfData[ f_iIfLevel ].fInElse == FALSE, DRM_E_FAIL );

    if ( f_prgIfData[ f_iIfLevel - 1 ].fSkipLines == FALSE )  /* outer level not skipping lines */
    {
        ChkDR( __CheckIFCondition( f_prgIfData, f_iIfLevel, f_pAPIParams ) );
    }

ErrorExit:
    return dr;
}


static DRM_RESULT DRM_CALL _EndIF (
    __in struct _IfLevelData    *f_prgIfData,
    __in        DRM_DWORD       *f_piIfLevel )
{
    DRM_RESULT dr = DRM_SUCCESS;


    Log( "******", "EndIf, level %d", *f_piIfLevel );
    ChkBOOL( f_prgIfData[ *f_piIfLevel ].fInIf == TRUE,  DRM_E_FAIL );

    f_prgIfData[ *f_piIfLevel ].fInIf         = FALSE;
    f_prgIfData[ *f_piIfLevel ].fInElse       = FALSE;
    f_prgIfData[ *f_piIfLevel ].fSkipLines    = FALSE;
    f_prgIfData[ *f_piIfLevel ].fConditionMet = FALSE;

    (*f_piIfLevel)--;

ErrorExit:
    return dr;
}


static DRM_VOID DRM_CALL _ResetIFHeirarchy (
    __in struct _IfLevelData    *f_prgIfData,
    __in        DRM_DWORD       *f_piIfLevel )
{
    DRM_DWORD   i = 0;

    for ( i = 0; i < MAX_NESTED_IFS; i++ )
    {
        f_prgIfData[ i ].fInIf         = FALSE;
        f_prgIfData[ i ].fInElse       = FALSE;
        f_prgIfData[ i ].fSkipLines    = FALSE;
        f_prgIfData[ i ].fConditionMet = FALSE;
    }
    *f_piIfLevel = 0;
}
/*
** ----------------------------------------------------------------------------
** |    END CONDITIONAL EXECUTION FUNCTIONS
** ----------------------------------------------------------------------------
*/

static DRM_VOID _CleanupBeforeEndTestCase(
    TAPIParams *pParams,
    struct _LoopEntry rgLoopEntries[MAX_NESTED_LOOPS] )
{
    DRM_DWORD idx = 0;
    FreeAPIParams( pParams );
    for( idx = 0; idx < MAX_NESTED_LOOPS; idx++ )
    {
        SAFE_OEM_FREE( rgLoopEntries[idx].LFD.pszXML );
    }
}


int DRM_CALL DoTest
(                           int                   argc,
    __in_ecount(argc) const char                **argv,
                      const TestScriptListEntry  *f_rgoTestScriptList,
                            LPFNWRAPTESTRUN       lpfnWrapTestRun,
                      const TModMapEntry         *f_rgoModuleMap
)
{
    DRM_RESULT dr                      = DRM_E_TEST_INCOMPLETE;
    int        i                       = 0;
    DRM_BOOL   fLoggerEnabled          = TRUE;
    DRM_BOOL   fTCFilePresent          = FALSE;
    DRM_BOOL   fLastTestCase           = FALSE;
    DRM_BOOL   fSkipTestCase           = FALSE;
    DRM_BOOL   fAllowClauseSkipTestIf  = TRUE;
    long       lStartTCID              = 0;
    long       lEndTCID                = 0;
    DRM_DWORD  dwCurrentTCID           = 0;
    long       lNextTCID               = 0;
    char      *pLine                   = NULL;
    char      *pNextTCTitle            = NULL;
    char       szTraceMessage[500]     = { '\0' };
    long       lLoopDepth              = -1;

    struct _LoopEntry rgLoopEntries[MAX_NESTED_LOOPS] = { 0 };
    DRM_TEST_RUN_TYPE eRunWhat = eRunNone;

    /*
    ** These variables need to be released at the end
    */
    FILE        *fTestCase               = NULL;
    char        *szLine                  = NULL;
    char        *pTCTitle                = NULL;
    const char **szTestScriptFiles       = NULL;
    const char  *szSupportedPlatforms[]  = SUPPORTED_TEST_PLATFORMS;
    DRM_WCHAR   *pwszNewFuzzPath         = NULL;
    DRM_WCHAR   *pwszNewFilePath         = NULL;
    DRM_WCHAR   *pwszNewDRMPath          = NULL;
    DRM_CHAR    *pszStdOutLogFile        = NULL;
    DRM_DWORD    cchExeNameWithExtension = 0;
    TAPIParams   APIParams               = {0};

    char *pstrArgNames[DRM_TEST_MAX_ARG_NAME_VALUE_PAIRS]   = {NULL};
    char *pstrArgValues[DRM_TEST_MAX_ARG_NAME_VALUE_PAIRS]  = {NULL};
    long  cNameValuePairs = 0;

    struct _IfLevelData    rgIfData[ MAX_NESTED_IFS ] = { 0 };
    DRM_DWORD              iIfLevel = 0;


#ifndef USE_DRM_HARNESS_LOGGER
    /*
    ** save application name
    */
    extern char g_szExeNameWithExtension[256];

    ChkArg( argc > 0 && argv != NULL );

    memset(g_szExeNameWithExtension, '\0', 256);
    cchExeNameWithExtension = ( DRM_DWORD ) min( NO_OF( g_szExeNameWithExtension ) - 1, strlen( argv[0] ) );
    MEMCPY( g_szExeNameWithExtension, argv[0], cchExeNameWithExtension );
    g_szExeNameWithExtension[ cchExeNameWithExtension ] = '\0';
#endif

    /*
    ** The following optional flags are supported; otherwise it is a test case number:
    **    -nolog to allow disable COM log system;
    **    -bvt run BVT test cases only;
    **    -func run Functional test cases only.
    */
    for( i = 1; i < argc; i++  )
    {
        if ( !strcmp(argv[i], NO_PROMPT ) )
        {
            /*
            ** no action, need to ignore this. Device OEM_Main should handle this directly
            */
            continue;
        }
        else if ( !strcmp( argv[i], NO_LOG ) || !strcmp( argv[i], QUIET ) )
        {
            fLoggerEnabled = FALSE;
        }
        else if ( !strcmp( argv[i], RUN_BVT ) )
        {
            eRunWhat = ( DRM_TEST_RUN_TYPE )(eRunWhat | eRunBVT);
        }
        else if ( !strcmp(argv[i], RUN_FUNC ) )
        {
            eRunWhat = ( DRM_TEST_RUN_TYPE )(eRunWhat | eRunFunc);
        }
        else if ( !strcmp(argv[i], RUN_DRT ) )
        {
            eRunWhat = ( DRM_TEST_RUN_TYPE )(eRunWhat | eRunDRT);
        }
        else if ( !strcmp(argv[i], RUN_PERF ) )
        {
            eRunWhat = ( DRM_TEST_RUN_TYPE )(eRunWhat | eRunPerf);
        }
        else if ( !strcmp(argv[i], RUN_FUZZ ) )
        {
            eRunWhat = ( DRM_TEST_RUN_TYPE )(eRunWhat | eRunFuzz);
        }
        else if ( !strcmp(argv[i], SPECIFY_FUZZED_FILE_PATH ) )
        {
            /*
            ** Override the Fuzz path with that specified in the next argv element
            */
            DRM_SUBSTRING dasstr = EMPTY_DRM_SUBSTRING;
            i++;
            ChkArg( i < argc );

            dasstr.m_ich = 0;
            dasstr.m_cch = ( DRM_DWORD ) strlen( argv[i] );

            ChkMem( pwszNewFuzzPath = (DRM_WCHAR*)Oem_MemAlloc( (dasstr.m_cch + 1) * SIZEOF( DRM_WCHAR ) ) );

            g_dstrFuzzPath.pwszString = pwszNewFuzzPath;
            g_dstrFuzzPath.cchString = dasstr.m_cch;
            pwszNewFuzzPath[dasstr.m_cch] = 0;

            DRM_UTL_PromoteASCIItoUNICODE( argv[i], &dasstr, (DRM_STRING *)&g_dstrFuzzPath );
        }
        else if ( !strcmp(argv[i], SPECIFY_TEST_FILE_PATH ) )
        {
            /*
            ** Override the test file path with that specified in the next argv element
            */
            DRM_SUBSTRING dasstr = EMPTY_DRM_SUBSTRING;
            i++;
            ChkArg( i < argc );

            dasstr.m_ich = 0;
            dasstr.m_cch = ( DRM_DWORD ) strlen( argv[i] );

            ChkMem( pwszNewFilePath = (DRM_WCHAR*)Oem_MemAlloc( (dasstr.m_cch + 1) * SIZEOF( DRM_WCHAR ) ) );

            g_dstrFilePath.pwszString = pwszNewFilePath;
            g_dstrFilePath.cchString = dasstr.m_cch;
            pwszNewFilePath[dasstr.m_cch] = 0;

            DRM_UTL_PromoteASCIItoUNICODE( argv[i], &dasstr, (DRM_STRING *)&g_dstrFilePath );
        }
        else if ( !strcmp(argv[i], SPECIFY_DRM_PATH ) )
        {
            DRM_SUBSTRING dasstr = EMPTY_DRM_SUBSTRING;

            /*
            ** Override the DRM path with that specified in the next argv element
            */
            i++;
            ChkArg( i < argc );

            dasstr.m_ich = 0;
            dasstr.m_cch = ( DRM_DWORD ) strlen( argv[i] );

            ChkMem( pwszNewDRMPath = (DRM_WCHAR*)Oem_MemAlloc( (dasstr.m_cch + 1) * SIZEOF( DRM_WCHAR ) ) );

            g_dstrDrmPath.pwszString = pwszNewDRMPath;
            g_dstrDrmPath.cchString = dasstr.m_cch;
            pwszNewDRMPath[dasstr.m_cch] = 0;

            DRM_UTL_PromoteASCIItoUNICODE( argv[i], &dasstr, (DRM_STRING *)&g_dstrDrmPath );
        }
        else if ( !strcmp(argv[i], NAME_VALUE_PAIR) )
        {
            long cLen = 0;
            const char* pTail = NULL;

            ChkArg( cNameValuePairs < DRM_TEST_MAX_ARG_NAME_VALUE_PAIRS );
            i++;
            ChkArg( i < argc);

            pTail = (const char*) argv[i];

            while(  '\0' != *pTail  )
            {
                if( '=' == *pTail )
                {
                    ChkMem( pstrArgNames[cNameValuePairs] = (char*)Oem_MemAlloc( (cLen+1) * SIZEOF(char) ) );
                    DRM_STR_StringCchCopyNA(pstrArgNames[cNameValuePairs], (DRM_DWORD)(cLen+1), argv[i], (DRM_DWORD)cLen );
                    pstrArgNames[cNameValuePairs][cLen] = 0;

                    pTail++;
                    cLen = ( long ) strlen( pTail );

                    ChkMem( pstrArgValues[cNameValuePairs] = (char*)Oem_MemAlloc( (cLen+1) * SIZEOF(char) ) );
                    DRM_STR_StringCchCopyNA(pstrArgValues[cNameValuePairs], (DRM_DWORD)(cLen+1), pTail, (DRM_DWORD)cLen );
                    pstrArgValues[cNameValuePairs][cLen] = 0;

                    cNameValuePairs++;
                    break;
                }

                pTail++;
                cLen++;
            }

        }
        else /* assume it is a test case number */
        {
            char* pchEnd = NULL;
            lStartTCID = strtol( argv[i], &pchEnd, 0  );
            if( pchEnd != NULL && *pchEnd == '-' && pchEnd+1 != NULL )
            {
                pchEnd++;
                lEndTCID = strtol( pchEnd, NULL, 0 );
            }
            else
            {
                lEndTCID = lStartTCID;
            }
            ChkBOOL( lEndTCID >= lStartTCID, DRM_E_TEST_INVALIDARG );
            eRunWhat = eRunAll;
        }
    }

    /*
    ** Default to running all test cases
    */
    if( eRunWhat == eRunNone )
    {
        eRunWhat = ( DRM_TEST_RUN_TYPE )(eRunBVT | eRunDRT | eRunFunc | eRunFuzz | eRunPerf );
    }

    InitLogger(fLoggerEnabled);
    LogTestInfo(f_rgoTestScriptList, szSupportedPlatforms, eRunWhat);

#if DBG
    Oem_Debug_HookTrace(RefTestTraceCallback);
    Oem_Debug_HookAssert(RefTestAssertCallback);
#endif

    if(lpfnWrapTestRun)
    {
        ChkDR(lpfnWrapTestRun(lStartTCID, TRUE));
    }
    ChkMem(szLine = (char*)Oem_MemAlloc(MAX_LINE_SIZE));

    for( ; f_rgoTestScriptList->m_szTestScriptFileName != NULL; f_rgoTestScriptList++ )
    {
        const char **pTestScriptFile;

        tFREEFILENAMELIST( szTestScriptFiles );

        szTestScriptFiles = NULL;

        if( ( f_rgoTestScriptList->m_pfnIsSupported != NULL )
         && !f_rgoTestScriptList->m_pfnIsSupported() )
        {
            /*
            ** If test script file not supported, continue to the next file in the list
            */
            continue;
        }

        dr = tGETTCFILELIST(f_rgoTestScriptList->m_szTestScriptFileName, szSupportedPlatforms, &szTestScriptFiles);

        if( ( dr != DRM_E_WIN32_FILE_NOT_FOUND ) && ( dr != DRM_E_WIN32_NO_MORE_FILES ) )
        {
             ChkDR( dr );
        }
        else
        {
             /*
             ** If test script file not found, continue to the next file in the list
             */
             continue;
        }

        for ( pTestScriptFile = szTestScriptFiles; *pTestScriptFile; pTestScriptFile++ )
        {
            ENUM_TESTCASE_TYPE eCurrentDefaultType = eBVT;

            printf("\t\t%s\n", *pTestScriptFile );
            fTestCase = fopen( *pTestScriptFile, "rt" );

            if ( !fTestCase )
            {
                continue;
            }

            fTCFilePresent = TRUE;

            while ( !feof( fTestCase ) && fgets(szLine, MAX_LINE_SIZE, fTestCase ) )
            {
                pLine = TrimSpace(szLine);

                /*
                ** comment line starts with ', don't process a comment line or an empty line
                */
                if ( !*pLine || *pLine == '\'' )
                {
                    continue;
                }
                /*
                ** separation line
                */
                if ( *pLine == '-' )
                {
                    if ( strstr( pLine, BVT_END_LINE ) )
                    {
                        /*
                        ** BVT's are done, next default type is Func
                        */
                        eCurrentDefaultType = eFunc;
                    }
                    if ( strstr( pLine, FUNC_END_LINE ) )
                    {
                        /*
                        ** Func's are done, next default type is DRT
                        */
                        eCurrentDefaultType = eDRT;
                    }
                    if ( strstr( pLine, DRT_END_LINE ) )
                    {
                        /*
                        ** DRT's are done, next default type is Perf
                        */
                        eCurrentDefaultType = ePerf;
                    }
                    continue;
                }

                if ( ParseTCID( pLine, &lNextTCID, &pNextTCTitle ) )
                {
                    ENUM_TESTCASE_TYPE eTCType = eUnknown;

                    if ( dwCurrentTCID != 0 )
                    {
                        /*
                        ** Test case CurrentTCID is finished and passed, log it
                        */
                        _CleanupBeforeEndTestCase( &APIParams, rgLoopEntries );
                        dr = _EndTestCase( dwCurrentTCID, EDISPOSITION_PASSED, pTCTitle, NULL, f_rgoModuleMap );
                        dwCurrentTCID = 0;

                        if ( fLastTestCase ) /* We just ran the required test cases, no more left, so leave now. */
                        {
                            goto ErrorExit;
                        }
                    }

                    /*
                    ** If the test case prefix matches the cases we want, then keep going.
                    ** However, if the test case prefix isn't specified, then use the BVT
                    ** end line to determine whether to run the test case.
                    **
                    */
                    ChkDR( ParseTCType( pNextTCTitle, &eTCType ) );
                    if ( eTCType == eUnknown )
                    {
                        eTCType = eCurrentDefaultType;
                    }

                    switch( eTCType )
                    {
                    case eDRT:
                        if( ( eRunWhat & eRunDRT ) == eRunNone )
                        {
                            continue;
                        }
                        break;
                    case eBVT:
                        if( ( eRunWhat & eRunBVT ) == eRunNone )
                        {
                            continue;
                        }
                        break;
                    case eFunc:
                        if( ( eRunWhat & eRunFunc ) == eRunNone )
                        {
                            continue;
                        }
                        break;
                    case ePerf:
                        if( ( eRunWhat & eRunPerf ) == eRunNone )
                        {
                            continue;
                        }
                        break;
                    case eFuzz:
                        if( ( eRunWhat & eRunFuzz ) == eRunNone )
                        {
                            continue;
                        }
                        break;
                    case eDisabled:
                        continue;
                    default:
                        break;
                    }

                    if ( ( lStartTCID == 0 )
                      || ( lStartTCID <= lNextTCID
                        && lEndTCID   >= lNextTCID ) )
                    {
                        /*
                        ** save the test case info and start to run api tests for this test case
                        */
                        dwCurrentTCID = (DRM_DWORD)lNextTCID;
                        SAFE_OEM_FREE( pTCTitle );

                        ChkMem(pTCTitle = StringDup(pNextTCTitle));
                        if( lEndTCID == lNextTCID )
                        {
                            fLastTestCase = TRUE;
                        }
                        if ( DRM_FAILED( dr = StartTestCase( (long)dwCurrentTCID, pTCTitle, eTCType, f_rgoModuleMap ) ) )
                        {
                            _CleanupBeforeEndTestCase( &APIParams, rgLoopEntries );
                            dr = _EndTestCase( dwCurrentTCID, EDISPOSITION_FAILED, pTCTitle, NULL, f_rgoModuleMap );
                            dwCurrentTCID = 0;
                        }
                        else
                        {
                            _ResetIFHeirarchy( rgIfData, &iIfLevel );
                            fSkipTestCase = FALSE;
                            fAllowClauseSkipTestIf = TRUE;
                        }
                    }

                }
                else if ( ( dwCurrentTCID != 0 ) && ( ParseAPI( pLine, &APIParams, (DRM_DWORD)lLoopDepth, rgLoopEntries ) ) )
                {
                    DRM_BOOL fFail = FALSE;
                    DRM_BOOL fEndEarly = FALSE;
                    long iNVP=0;
                    long j=0;

                    for( iNVP=0; iNVP < cNameValuePairs; iNVP++ )
                    {
                       for( j=0; j < APIParams.m_lParamCount ; j++)
                       {
                           if ( NULL != APIParams.m_szParams[j]
                             && NULL != pstrArgNames[iNVP]
                             && 0    == strcmp( APIParams.m_szParams[j], pstrArgNames[iNVP] ) )
                           {
                                long cLen = 0;

                                SAFE_OEM_FREE( APIParams.m_szParams[j] );
                                cLen = ( long ) strlen(pstrArgValues[iNVP]);

                                ChkMem( APIParams.m_szParams[j] = (char*)Oem_MemAlloc( (cLen+1) * SIZEOF(char) ) );
                                DRM_STR_StringCchCopyNA(APIParams.m_szParams[j], (DRM_DWORD)(cLen+1), pstrArgValues[iNVP], (DRM_DWORD)cLen );
                                APIParams.m_szParams[j][cLen] = 0;
                            }
                        }
                    }


                    /*
                    ** ---------------------------------
                    ** |  Begin conditional processing
                    ** ---------------------------------
                    */
                    if ( strcmp( APIParams.m_szAPIName, SKIPTESTIF ) == 0 )
                    {
                        ChkBOOL( fAllowClauseSkipTestIf, DRM_E_FAIL );
                        fAllowClauseSkipTestIf = FALSE;
                        ChkDR( LogTestAPI( APIParams ) );
                        ChkDR( TST_UTL_IsConditionTrue( &fSkipTestCase,
                                                        APIParams.m_lParamCount,
                                                        (const char **)APIParams.m_szParams ) );
                        if ( !fSkipTestCase )
                        {
                            continue;
                        }
                        else
                        {
                            Log( "******", "Skipping Test : One or more of the conditions were met" );
                        }
                    }
                    else if ( strcmp( APIParams.m_szAPIName, BEGINIF ) == 0 )
                    {
                        fAllowClauseSkipTestIf = FALSE;
                        ChkDR( _BeginIF( rgIfData, &iIfLevel, &APIParams ) );
                        continue;
                    }
                    else if( strcmp( APIParams.m_szAPIName, ENDIF ) == 0 )
                    {
                        fAllowClauseSkipTestIf = FALSE;
                        ChkDR( _EndIF( rgIfData, &iIfLevel ) );
                        continue;
                    }
                    else if ( strcmp( APIParams.m_szAPIName, BEGINELSEIF ) == 0 )
                    {
                        fAllowClauseSkipTestIf = FALSE;
                        ChkDR( _BeginELSEIF( rgIfData, iIfLevel, &APIParams ) );
                        continue;
                    }
                    else if ( strcmp( APIParams.m_szAPIName, BEGINELSE ) == 0 )
                    {
                        fAllowClauseSkipTestIf = FALSE;
                        ChkDR( _BeginELSE( rgIfData, iIfLevel ) );
                        continue;
                    }
                    /*
                    ** ---------------------------------
                    ** |  End conditional processing
                    ** ---------------------------------
                    */

                    if ( !fSkipTestCase )
                    {
                        /*
                        ** ---------------------------------
                        ** |  Log the API
                        ** ---------------------------------
                        */
                        if ( rgIfData[ iIfLevel ].fSkipLines )
                        {
                            Log( "******", "Skipping API %s", APIParams.m_szAPIName );
                            continue;
                        }

                        ChkDR( LogTestAPI( APIParams ) );

                        /*
                        ** ---------------------------------
                        ** |  Begin loop processing
                        ** ---------------------------------
                        */
                        if( strcmp( APIParams.m_szAPIName, BEGIN_REPEAT_LOOP ) == 0 )
                        {
                            fAllowClauseSkipTestIf = FALSE;
                            lLoopDepth++;
                            ChkBOOL( lLoopDepth < MAX_NESTED_LOOPS, DRM_E_WIN32_ERROR_TOO_MANY_CMDS );
                            ChkArg( APIParams.m_lParamCount >= 1 );
                            ChkArg( APIParams.m_szParams[0] != NULL );
                            rgLoopEntries[lLoopDepth].lBeginOfLoopFilePosition = ftell( fTestCase );
                            rgLoopEntries[lLoopDepth].lCurrentLoop = strtol( APIParams.m_szParams[0], NULL, 0 );
                            rgLoopEntries[lLoopDepth].eLoopType = ELOOPTYPE_REPEAT;
                            Log( "******", "Loop %d times", rgLoopEntries[lLoopDepth].lCurrentLoop );
                        }
                        else if ( strcmp( APIParams.m_szAPIName, FOREACHITEMINFILE) == 0 )
                        {
                            fAllowClauseSkipTestIf = FALSE;
                            lLoopDepth++;
                            ChkBOOL( lLoopDepth < MAX_NESTED_LOOPS, DRM_E_WIN32_ERROR_TOO_MANY_CMDS );

                            ChkArg( APIParams.m_lParamCount >= 1 );
                            ChkArg( APIParams.m_szParams );
                            ChkArg( APIParams.m_szParams[0] != NULL );

                            ChkDR( _GetFirstItemInLoopFile( APIParams.m_szParams[0], &rgLoopEntries[lLoopDepth].LFD ) );

                            rgLoopEntries[lLoopDepth].eLoopType = ELOOPTYPE_FOREACHITEMINFILE;
                            rgLoopEntries[lLoopDepth].lBeginOfLoopFilePosition = ftell( fTestCase );
                        }
                        else if( strcmp( APIParams.m_szAPIName, ENDOFLOOP ) == 0 )
                        {
                            fAllowClauseSkipTestIf = FALSE;
                            ChkArg( lLoopDepth >= 0 );

                            switch ( rgLoopEntries[lLoopDepth].eLoopType )
                            {
                                case ELOOPTYPE_REPEAT:
                                {
                                    rgLoopEntries[lLoopDepth].lCurrentLoop--;
                                    if( rgLoopEntries[lLoopDepth].lCurrentLoop > 0 )
                                    {
                                        Log( "******", "Loop again %d times", rgLoopEntries[lLoopDepth].lCurrentLoop );
                                        ChkBOOL( fseek( fTestCase, rgLoopEntries[lLoopDepth].lBeginOfLoopFilePosition, SEEK_SET ) == 0, DRM_E_FILE_SEEK_ERROR );
                                    }
                                    else
                                    {
                                        Log( "******", "Exiting loop" );
                                        lLoopDepth--;
                                    }
                                    break;
                                }

                                case ELOOPTYPE_FOREACHITEMINFILE:
                                {
                                    dr = _GetNextItemInLoopFile( &rgLoopEntries[lLoopDepth].LFD );
                                    if( dr == DRM_SUCCESS )
                                    {
                                        Log( "******", "Loop again" );
                                        ChkBOOL( fseek( fTestCase, rgLoopEntries[lLoopDepth].lBeginOfLoopFilePosition, SEEK_SET ) == 0, DRM_E_FILE_SEEK_ERROR );
                                    }
                                    else if( dr == DRM_E_NOMORE )
                                    {
                                        dr = DRM_SUCCESS;
                                        _CloseLoopFile( &rgLoopEntries[lLoopDepth].LFD );
                                        Log( "******", "Exiting loop" );
                                        lLoopDepth--;
                                    }
                                    else
                                    {
                                        ChkDR( dr );
                                    }
                                    break;
                                }

                                default:
                                {
                                    ChkDR( DRM_E_FAIL );
                                    break;
                                }
                            }
                        }
                        /*
                        ** ---------------------------------
                        ** |  End loop processing
                        ** ---------------------------------
                        */

                        else if( strcmp( APIParams.m_szAPIName, LOGCOMMENT ) == 0 )
                        {
                            /*
                            ** do nothing
                            */
                        }
                        else
                        {
                            fAllowClauseSkipTestIf = FALSE;
                            ChkDR( RunTestAPI( APIParams, &fFail, &fEndEarly, NULL, NULL, szTraceMessage, SIZEOF( szTraceMessage ), FALSE, f_rgoModuleMap ) );
                        }
                    }


                    if (  fSkipTestCase || fFail || fEndEarly )
                    {
                        /*
                        ** api test failed or need to end test early
                        */
                        _CleanupBeforeEndTestCase( &APIParams, rgLoopEntries );
                        dr = _EndTestCase( dwCurrentTCID,
                                          ( fSkipTestCase )? EDISPOSITION_SKIPPED : EDISPOSITION_FAILED,
                                          pTCTitle,
                                          ( fFail )? szTraceMessage : NULL,
                                          f_rgoModuleMap );
                        dwCurrentTCID = 0;
                        _ResetIFHeirarchy( rgIfData, &iIfLevel );
                        fSkipTestCase = FALSE;
                        if ( fLastTestCase )
                        {
                            /*
                            ** We just ran the required test case, no more left, so leave now.
                            */
                            goto ErrorExit;
                        }
                    }
                }
            }

            if ( dwCurrentTCID != 0 )
            {
                /*
                ** Test case CurrentTCID is finished and passed, log it
                */
                _CleanupBeforeEndTestCase( &APIParams, rgLoopEntries );
                dr = _EndTestCase( dwCurrentTCID, EDISPOSITION_PASSED, pTCTitle, NULL, f_rgoModuleMap );
                dwCurrentTCID = 0;
                if ( fLastTestCase ) /* We just ran the required test case, no more left, so leave now.*/
                {
                    goto ErrorExit;
                }
            }

            fclose( fTestCase );
            fTestCase = NULL;
        }
    }

    if (!fTCFilePresent)
    {
        printf("Cannot find any test case file.\n");
    }

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        printf( "\nTclib.c::DoTest() - Test execution failure: dr = 0x%X", dr );
    }

    for( i =0; i < cNameValuePairs ; i++ )
    {
        SAFE_OEM_FREE( pstrArgNames[i] );
        SAFE_OEM_FREE( pstrArgValues[i] );
    }

    SAFE_OEM_FREE(pTCTitle);
    SAFE_OEM_FREE( pszStdOutLogFile );
    tFREEFILENAMELIST(szTestScriptFiles);
    _CleanupBeforeEndTestCase( &APIParams, rgLoopEntries );
    SAFE_OEM_FREE(szLine);

    if (fTestCase)
    {
        fclose(fTestCase);
    }
    lpfnWrapTestRun(lEndTCID, FALSE);
    UninitLogger();

    SAFE_OEM_FREE( pwszNewDRMPath );
    SAFE_OEM_FREE( pwszNewFuzzPath );
    SAFE_OEM_FREE( pwszNewFilePath );

    tOEMSignalTestDone();
    return DRM_SUCCEEDED(dr)? 0: -1;
}


DRM_RESULT DRM_CALL DemoteArgumentsToAnsi(DRM_LONG argc, __out_ecount( argc ) DRM_WCHAR **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_LONG   iArg = 0;

    ChkArg( argv != NULL );

    for( iArg = 0; iArg < argc; iArg++ )
    {
        DRM_UTL_DemoteUNICODEtoASCII( argv[iArg], (DRM_CHAR*)argv[iArg], (DRM_DWORD)DRMCRT_wcslen(argv[iArg]) + 1 );
    }

ErrorExit:
    return dr;
}


/********************************************************************************************
** Function:    Test_EnsureAsserted
**
** Synopsis:    Validate that an assert was thrown
**
** Arguments:   none
**
** Returns:     DRM_E_SUCCESS or DRM_E_FAIL
********************************************************************************************/
DRM_RESULT DRM_CALL Test_EnsureAsserted( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

#if DBG
    ChkFAIL( g_fAssertThrown );

    /*
    ** Reset the flag
    */
    g_fAssertThrown = FALSE;
ErrorExit:
#endif
    return dr;
}

/*
** Test_SetRandomSeed
** argv[0]: Seed value: FIXED, RANDOM, the high dword of a qword seed
** argv[1]: The low dword of a qword seed. Required if not using FIXED or RANDOM
*/
DRM_RESULT DRM_CALL Test_SetRandomSeed( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT  dr     = DRM_SUCCESS;
    DRMFILETIME ft     = { 0 };
    DRM_DWORD   dwHigh = 0;
    DRM_DWORD   dwLow  = 0;
    DRM_UINT64  u64    = DEFAULT_RANDOM_SEED;

    ChkArg( argc    >= 1
         && argv[0] != NULL );

    if ( strncmp( argv[0], "FIXED", 5 ) == 0 )
    {
        dwHigh = DRM_UI64High32( u64 );
        dwLow  = DRM_UI64Low32( u64 );
        Log( "Trace", "Setting random seed to default value: ( %X, %X )", dwHigh, dwLow );
    }
    else if ( strncmp( argv[0], "RANDOM", 6 ) == 0 )
    {
        Oem_Clock_GetSystemTimeAsFileTime( NULL, &ft );
        FILETIME_TO_UI64( ft, u64 );
        dwHigh = DRM_UI64High32( u64 );
        dwLow  = DRM_UI64Low32( u64 );
        Log( "Trace", "Setting random seed to file time: ( %X, %X )", dwHigh, dwLow );
    }
    else
    {
        ChkArg( argc    == 2
             && argv[1] != NULL );

        dwHigh = strtoul( argv[0], NULL, 0 );
        dwLow  = strtoul( argv[1], NULL, 0 );
        u64 = DRM_UI64HL( dwHigh, dwLow );
        Log( "Trace", "Setting random seed to: ( %X, %X )", dwHigh, dwLow );
    }
    Oem_Random_SetSeed( NULL, u64 );

ErrorExit:
    return dr;
}

/********************************************************************************************
** Function:    TestCopyFile
**
** Synopsis:    Copies a file from one location to another.
**
** Arguments:   argv[0] : Source path
**              argv[1] : Destination path
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestCopyFile( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_CONST_STRING dstrSource = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDest   = EMPTY_DRM_STRING;

    ChkArg( argc == 2 );

    ChkDR( TST_UTL_MakePathString( NULL, argv[0], &dstrSource ) );
    ChkDR( TST_UTL_MakePathString( NULL, argv[1], &dstrDest ) );

    ChkDR( TST_OEM_CopyFile( dstrSource, dstrDest ) );

ErrorExit:
    FREEDRMSTRING( dstrSource );
    FREEDRMSTRING( dstrDest );
    return dr;
}


static DRM_BOOL DRM_CALL _FileExists(
    __in const DRM_WCHAR   *f_pwszFile
)
{
    OEM_FILEHDL   hFile  = OEM_INVALID_HANDLE_VALUE;
    DRM_BOOL      fRC    = TRUE;


    hFile = Oem_File_Open( NULL, f_pwszFile, OEM_GENERIC_READ, OEM_FILE_SHARE_READ, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL );
    if( hFile == OEM_INVALID_HANDLE_VALUE )
    {
        TRACE( ( "_FileExists failed to open source file %S.", f_pwszFile ) );
        fRC = FALSE;
    }
    Oem_File_Close( hFile );
    return fRC;
}


/********************************************************************************************
** Function:    TestCopyFileIfExists
**
** Synopsis:    Copies a file from one location to another, if the source file exists.
**
** Arguments:   argv[0] : Source path
**              argv[1] : Destination path
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestCopyFileIfExists( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_CONST_STRING dstrSource = EMPTY_DRM_STRING;


    ChkArg( argc == 2 );
    ChkDR( TST_UTL_MakePathString( NULL, argv[0], &dstrSource ) );
    if ( _FileExists( dstrSource.pwszString ) )
    {
        ChkDR( TestCopyFile( argc, argv ) );
    }

ErrorExit:
    FREEDRMSTRING( dstrSource );
    return dr;
}


/********************************************************************************************
** Function:    TestCopyFuzzFile
**
** Synopsis:    Copies a fuzzed file from one default or user-defined location to current folder.
                On a device, the current folder is just the root.
**
** Arguments:   argv[0] : Source path
**              argv[1] : Destination path
**
********************************************************************************************/
DRM_RESULT DRM_CALL TestCopyFuzzFile( long argc, __in_ecount( argc ) char **argv )
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_CONST_STRING dstrSource = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDest   = EMPTY_DRM_STRING;
    DRM_DWORD        cchTemp    = 0;
    DRM_CHAR         *pszTemp   = NULL;

    ChkArg( argc == 2 );

    ChkDR( TST_UTL_MakePathString( NULL, argv[1], &dstrDest ) );

    if( g_dstrFuzzPath.cchString > 1 ) /*if the caller passed in a fuzzed file name*/
    {
        ChkDR( TST_UTL_MakePathA( NULL, "", &pszTemp, &cchTemp, TRUE ) );
        ChkDR( MakeDRMString( &dstrSource, pszTemp ) );
    }
    else /*otherwise, use the file name from dailytesthds.pc.pk.ce.txt*/
    {
        ChkDR( TST_UTL_MakePathString( NULL, argv[0], &dstrSource ) );
    }

    ChkDR( TST_OEM_CopyFile( dstrSource, dstrDest ) );

ErrorExit:
    FREEDRMSTRING( dstrSource );
    FREEDRMSTRING( dstrDest );
    SAFE_OEM_FREE( pszTemp );
    return dr;
}

/*********************************************************
**
** Function: SetResultTrap
**
** Synopsis: Sets an error code to be looked for in ChkDR
**           and sets the Global ExamineDR function to look for it.
**
** Arguments:
**  argv[0] -- Error Code to look for..
**
** Returns     DRM_SUCCESS
**
*********************************************************/
DRM_RESULT DRM_CALL SetResultTrap(long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    g_fExamineDR = FALSE;
    if ( argc > 0 && argv[0] != NULL )
    {
        DRM_DWORD cch = 0;
        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen(argv[0]), &cch ) );
        ChkDR( DRM_ERR_GetErrorCodeFromStringA( argv[0], cch, &g_drExamine ) );
        g_fExamineDR = TRUE;
    }
    SetDbgAnalyzeFunction( TST_ExamineDR );
ErrorExit:
    return dr;
}


/*********************************************************
**
** Function: BinaryCompareFiles
**
** Synopsis: Binary compares 2 files to each other
**
** Arguments:
**  argv[0] -- Optional. folder for the first file
**  argv[1] -- file name for the first file
**  argv[2] -- Optional. folder for the second file
**  argv[3] -- file name for the second file
**
** Returns     DRM_SUCCESS if files are identical
**             DRM_E_TEST_UNEXPECTED_OUTPUT if files are different
**             other DRM_RESULT I\O error otherwise
**
*********************************************************/
DRM_RESULT DRM_CALL BinaryCompareFiles(long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( argc == 4 );
    ChkArg( argv[1] != NULL );
    ChkArg( argv[3] != NULL );

    ChkDR( TST_UTL_BinaryFileCompare( argv[0], argv[1], argv[2], argv[3] ) );

ErrorExit:

    return dr;
}

DRM_RESULT DRM_CALL GenerateRandomData( long argc, __in_ecount(argc) char **argv )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  cbData = 0;
    DRM_BYTE  *pbData = NULL;

    ChkArg( argc    >= 2 );
    ChkArg( argv[0] != NULL );
    ChkArg( argv[1] != NULL );

    cbData = strtoul( argv[1], NULL, 0 );
    MAP_TO_GLOBAL_TEST_VAR_Alloc( pbData, argv[0], cbData );

    ChkDR( Oem_Random_GetBytes( NULL, pbData, cbData ) );

ErrorExit:
    return dr;
}


/*****************************************************************************
** Function:    StartProfiling
** Synopsis:    Call this test API to turn on Profiling if DRM_SUPPORT_PROFIILNG is set to 1
** Arguments:   argv[0]        -- Optional argument - Name of the performance log generaged. Default is
**                                perf.dat
** Returns:     DRM_SUCCESS on success or
**              Or any return code as defined in drmresults.h
*/
DRM_RESULT DRM_CALL StartProfiling(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT       dr             = DRM_SUCCESS;
    DRM_CONST_STRING dstrProfileLog = {NULL,0};

    if(argc != 0)
    {
        ChkArg(argv[0]);
        ChkDR( MakeDRMString(&dstrProfileLog,argv[0]) );
        ChkDR(DRM_Start_Profiling(NULL,&dstrProfileLog));
    }
    else
        ChkDR(DRM_Start_Profiling(NULL,NULL));

ErrorExit:
    FREEDRMSTRING( dstrProfileLog );
    return dr;
}

/*****************************************************************************
** Function:    StopProfiling
** Synopsis:    Call this test API stop profiling Profiling if DRM_SUPPORT_PROFIILNG is set to 1
** Arguments:   N/A
** Returns:     DRM_SUCCESS on success or
**              Or any return code as defined in drmresults.h
*/
DRM_RESULT DRM_CALL StopProfiling(long argc, __in_ecount(argc) char **argv)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_Stop_Profiling();

    return dr;
}

/*
** Define the TestCore APIMap.
** This Module is always included in ModuleMap.
** Any function that should be accessible from
** any script should be placed here.
*/
BEGIN_TEST_API_LIST(TestCore)
    API_ENTRY(Test_SetRandomSeed)
    API_ENTRY(StartProfiling)
    API_ENTRY(StopProfiling)
    API_ENTRY(SwapTestVariables)
    API_ENTRY(StoreStringInTestVariable)
    API_ENTRY(CompareTestVariableToString)
    API_ENTRY(CompareTestVariables)
    API_ENTRY(FreeTestVariable)
    API_ENTRY(ReadFileToTestVariable)
    API_ENTRY(WriteTestVariableToFile)
    API_ENTRY(GenerateRandomData)
    API_ENTRY(TestCopyFile)
    API_ENTRY(TestCopyFileIfExists)
    API_ENTRY(TestCopyFuzzFile)
    API_ENTRY(BinaryCompareFiles)
    API_ENTRY(Test_EnsureAsserted)
    API_ENTRY(SetResultTrap)
END_TEST_API_LIST

IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE_FOR_MODULE(TestCore);

EXIT_PKTEST_NAMESPACE_CODE;

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

