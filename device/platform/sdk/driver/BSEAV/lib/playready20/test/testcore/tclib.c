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

#include "tclib.h"
#include "apiparams.h"
#include "logger.h"
#include "tstutils.h"
#include "tOEMImp.h"

ENTER_PKTEST_NAMESPACE_CODE

#define DRM_TEST_MAX_ARG_NAME_VALUE_PAIRS 10

#define NO_LOG                      "-nolog"
#define QUIET                       "-q"
#define RUN_BVT                     "-bvt"
#define RUN_FUNC                    "-func"
#define RUN_DRT                     "-drt"
#define RUN_PERF                    "-perf"
#define SPECIFY_DRM_PATH            "-drmpath"
#define SPECIFY_FUZZED_FILE_PATH    "-fuzzpath"
#define SPECIFY_TEST_FILE_PATH      "-filepath"
#define NO_PROMPT                   "-noprompt"
#define NAME_VALUE_PAIR             "-namevalue"
#define ENDOFLOOP                   "EndOfLoop"
#define BEGIN_REPEAT_LOOP           "BeginRepeatLoop"
#define LOGCOMMENT                  "LogComment"
#define BEGINIF                     "If"
#define BEGINELSE                   "Else"
#define BEGINELSEIF                 "ElseIf"
#define ENDIF                       "EndIf"

#define BVT_END_LINE    "BVT ends here"
#define FUNC_END_LINE   "FUNC ends here"
#define DRT_END_LINE    "DRT ends here"

#define DRM_E_WIN32_ERROR_TOO_MANY_CMDS       ((DRM_RESULT)0x80070038L)
typedef enum ELOOPTYPE
{
    ELOOPTYPE_INVALID           = 0,
    ELOOPTYPE_TIME              = 1,
    ELOOPTYPE_REPEAT            = 2,
    ELOOPTYPE_FOREACHITEMINFILE = 3,
};

static const DRM_WCHAR g_rgwchEmptyPath [] =
{
    ONE_WCHAR('\0', '\0')
};

DRM_CONST_STRING        g_dstrFuzzPath   = CREATE_DRM_STRING( g_rgwchEmptyPath );
extern DRM_CONST_STRING g_dstrFilePath; /* By default test files should be in the data path */

#if SIXTEEN_BIT_ADDRESSING
#define tGETTCFILELIST(x1,x2,x3)  tGetTCFileList_pfd(x1,x2,x3)
#define tFREEFILENAMELIST(x) tFreeFileNameList_pfd(x)
#else
#define tGETTCFILELIST(x1,x2,x3)  tGetTCFileList(x1,x2,x3)
#define tFREEFILENAMELIST(x) tFreeFileNameList(x)
#endif

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
        Log( "ExamineDR", "%s(%#X) encountered.", DRM_ERR_GetErrorNameFromCode( lDRVal, NULL ), lDRVal );
        DRMASSERT(FALSE);
    }
}

/*
** Perf - timing info 
*/
DRM_UINT64 g_uiStartTime = DRM_UI64LITERAL( 0, 0 );
DRM_UINT64 g_uiDiffTime  = DRM_UI64LITERAL( 0, 0 );

static TModMapEntry DRM_CALL _GetModuleData( const DRM_CHAR *f_szModuleName, TModMapEntry *f_rgoModMap )
{
    int i = 0;
    while (f_rgoModMap[i].m_szModuleName && strcmp(f_rgoModMap[i].m_szModuleName, f_szModuleName))
        i++;
    
    return f_rgoModMap[i];
}

DRM_RESULT StartTestCase(long lTCID, const char *szTCName, ENUM_TESTCASE_TYPE f_eType, TModMapEntry *f_rgoModMap )
{
    DRM_RESULT   dr           = DRM_SUCCESS;
    TModMapEntry oModMapEntry = EMPTY_MOD_MAP_ENTRY;
    DRM_DWORD    iModMapEntry = 0;

#if DBG
    g_fAssertThrown = FALSE;
#endif
    LogStart(lTCID, f_eType, szTCName);
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
        if ( DRM_FAILED( dr = oModMapEntry.m_pfnPreTestCase( lTCID, szTCName ) ) ) 
        {
            char *szError         = NULL;
            
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

DRM_RESULT EndTestCase(long lTCID, DRM_BOOL fPass, const char *szTCName, const char*szTrace, TModMapEntry *f_rgoModMap )
{
    DRM_RESULT   dr           = DRM_SUCCESS;
    TModMapEntry oModMapEntry = EMPTY_MOD_MAP_ENTRY;
    DRM_DWORD    iModMapEntry = 0;

    STOP_TIMER( g_uiStartTime, g_uiDiffTime );

    if( f_rgoModMap == NULL )
    {
        Log( "Error", "Module map is null in EndTestCase.");
        fPass = FALSE;
        goto ErrorExit;
    }

    oModMapEntry = f_rgoModMap[iModMapEntry];

    while( oModMapEntry.m_pfnPostTestCase != NULL )
    {
        if (DRM_FAILED(dr = oModMapEntry.m_pfnPostTestCase(lTCID, szTCName))) {
            char *szError         = NULL;

            szError = DRM_ERR_GetErrorNameFromCode( dr, NULL );
            Log("Trace", "PostTestCase for module %s failed with %s(%#X).", oModMapEntry.m_szModuleName, szError, dr);
            fPass = FALSE;
        }
        iModMapEntry++;
        oModMapEntry = f_rgoModMap[ iModMapEntry ];
    }

#if DBG
    if( g_fAssertThrown )
    {
        Log("Trace", "Test case failed due to assert being thrown.");
        fPass = FALSE;
    }
#endif
    DRM_TST_FreeVarArray();
ErrorExit:
    LogEnd((int)fPass, g_uiDiffTime, szTCName, szTrace);
    return fPass? DRM_SUCCESS: DRM_E_FAIL;
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
DRM_RESULT DRM_CALL RunAPIInModule( const char *szAPIName, 
                                    const char *szModuleName,
                                    long lParams, 
                                    __in_ecount(lParams) char **pParams, 
                                    TModMapEntry *f_poModuleMap )
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
DRM_VOID DRM_CALL RefTestTraceCallback(  DRM_CHAR* f_pszFormat, va_list f_ArgList )
{
    if ( f_pszFormat != NULL ) 
    {
        vLog( "DevTrace", f_pszFormat, f_ArgList );
    }
}

DRM_VOID DRM_CALL RefTestAssertCallback(
    IN DRM_BOOL  fAssert, 
    IN DRM_CHAR *assertcmd, 
    IN DRM_CHAR *file, 
    IN DRM_LONG  line)
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
#endif
        }
    }
} 
#endif

DRM_RESULT DRM_CALL LogTestAPI( TAPIParams APIParams )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_CHAR  *pszParams = NULL;
    DRM_SIZE_T cchParams = 0;

#define SIZEOF_COMMA  2
#define SIZEOF_NULL   6
#define SIZEOF_PARENS 4
    DRM_LONG iParam = 0;

    cchParams = DRMCRT_strlen( APIParams.m_szAPIName ) + SIZEOF_PARENS;
    for( iParam = 0; iParam < APIParams.m_lParamCount; iParam++ )
    {
        if( APIParams.m_szParams[iParam] == NULL )
        {
            cchParams += SIZEOF_NULL;
        }
        else
        {
            cchParams += DRMCRT_strlen( APIParams.m_szParams[iParam] ) + SIZEOF_COMMA;
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
                                TModMapEntry *poModuleMap )
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
        char *szMessageStart  = "API_Failure: api returns a value which is explicitly rejected. API ";
        char *szMessageMiddle = " unexpectedly returns ";
        char  szTemp[33]      = {0};
        char *szError         = NULL;
        DRM_DWORD cchCopied   = 0;
        DRM_DWORD cchCopy     = 0;

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
            cchCopy = TST_UTL_NumberToAnsiString( drActual, 16, 0, SIZEOF( szTemp ), &szTemp[0] );
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
        char *szMessageStart   = "API_Failure: API ";
        char *szMessageMiddle1 = " returns ";
        char *szMessageMiddle2 = "). Expecting ";
        char  szTemp[33]       = {0};
        char *szExpectedError  = NULL;
        char *szActualError    = NULL;

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
            cchCopy = TST_UTL_NumberToAnsiString( drActual, 16, 0, SIZEOF( szTemp ), &szTemp[0] );
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
            cchCopy = TST_UTL_NumberToAnsiString( APIParams.m_drExpected, 16, 0, SIZEOF( szTemp ), &szTemp[0] );
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

#define MAX_NESTED_LOOPS 10
struct _LoopEntry
{
    long           lBeginOfLoopFilePosition;
    long           lCurrentLoop;
    enum ELOOPTYPE eLoopType;
};

int DRM_CALL DoTest
(                           int argc, 
    __in_ecount(argc) const char **argv, 
                      const TestScriptListEntry *f_rgoTestScriptList, 
                            LPFNWRAPTESTRUN lpfnWrapTestRun,
                            TModMapEntry *f_rgoModuleMap
)
{
    DRM_RESULT dr                  = DRM_E_TEST_INCOMPLETE;
    int        i                   = 0;
    DRM_BOOL   fLoggerEnabled      = TRUE;
    DRM_BOOL   fTCFilePresent      = FALSE;
    DRM_BOOL   fLastTestCase       = FALSE;
    long       lStartTCID          = 0;
    long       lEndTCID            = 0;
    long       lCurrentTCID        = 0;
    long       lNextTCID           = 0;
    char      *pLine               = NULL;
    char      *pNextTCTitle        = NULL; 
    char       szTraceMessage[500] = { '\0' };
    long       lLoopDepth          = -1;
    
    struct _LoopEntry rgLoopEntries[MAX_NESTED_LOOPS] = { 0 };
    DRM_TEST_RUN_TYPE eRunWhat     = eRunNone;
    
    
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

    DRM_BOOL fInIf            = FALSE;
    DRM_BOOL fIfConditionMet  = FALSE;
    DRM_BOOL fInElse          = FALSE;
    DRM_BOOL fLookForEndIf    = FALSE;

#ifndef USE_DRM_HARNESS_LOGGER
    /*
    ** save application name
    */
    extern char g_szExeNameWithExtension[256];
    
    ChkArg( argc > 0 && argv != NULL );
    
    memset(g_szExeNameWithExtension, '\0', 256);

/*
** Since command line arguments are not available on 16-bit TI platform, we cannot
** use argv[0] for 16-bit
*/
#if __TMS320C55X__
        ChkDR( DRM_STR_StringCchCopyA( g_szExeNameWithExtension, SIZEOF(g_szExeNameWithExtension), "Console" ) );
#else  
    cchExeNameWithExtension = ( DRM_DWORD ) min( NO_OF( g_szExeNameWithExtension ) - 1, strlen( argv[0] ) );
    MEMCPY( g_szExeNameWithExtension, argv[0], cchExeNameWithExtension );
    g_szExeNameWithExtension[ cchExeNameWithExtension ] = '\0';
#endif /* __TMS320C55X__ */

#endif

    /*
    ** The following optional flags are supported; otherwise it is a test case number:
    **    -nolog to allow disable COM log system;
    **    -bvt run BVT test cases only;
    **    -func run Functional test cases only.
    */
#if __TMS320C55X__
    /*
    ** Since command line arguments are not available, we ignore it
    */
    argc = 0;
#endif

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
            char* pTail = NULL;
            
            ChkArg( cNameValuePairs < DRM_TEST_MAX_ARG_NAME_VALUE_PAIRS );
            i++;
            ChkArg( i < argc);

            pTail = (char*) argv[i];

            while(  '\0' != *pTail  )
            {                   
                if( '=' == *pTail )
                {                  
                    ChkMem( pstrArgNames[cNameValuePairs] = (char*)Oem_MemAlloc( (cLen+1) * SIZEOF(char) ) );
                    DRM_STR_StringCchCopyNA(pstrArgNames[cNameValuePairs], cLen+1, argv[i], cLen );
                    pstrArgNames[cNameValuePairs][cLen] = 0;
            
                    pTail++;
                    cLen = ( DRM_DWORD ) strlen( pTail );

                    ChkMem( pstrArgValues[cNameValuePairs] = (char*)Oem_MemAlloc( (cLen+1) * SIZEOF(char) ) );
                    DRM_STR_StringCchCopyNA(pstrArgValues[cNameValuePairs], cLen+1, pTail, cLen );
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
    ** Default to running all test cases except perf
    */
    if( eRunWhat == eRunNone )
    {
        eRunWhat = ( DRM_TEST_RUN_TYPE )(eRunBVT | eRunDRT | eRunFunc);
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

		printf("f_rgoTestScriptList->m_szTestScriptFileName = %s\n", f_rgoTestScriptList->m_szTestScriptFileName);

        dr = tGETTCFILELIST(f_rgoTestScriptList->m_szTestScriptFileName, szSupportedPlatforms, &szTestScriptFiles);

        if( ( dr != DRM_E_WIN32_FILE_NOT_FOUND ) && ( dr != DRM_E_WIN32_NO_MORE_FILES ) )
        {
        	printf("1 szTestScriptFiles = %s\n", szTestScriptFiles[0]);
			szTestScriptFiles[0] = malloc(265);
			sprintf(szTestScriptFiles[0], "%s", "refbb.pc.pk.ce.txt");
             ChkDR( dr );
        }
        else
        {
             /*
             ** If test script file not found, continue to the next file in the list
             */
             	printf("0\n");
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
                    
                    if ( lCurrentTCID != 0 )
                    { 
                        /*
                        ** Test case CurrentTCID is finished and passed, log it
                        */
                        dr = EndTestCase( lCurrentTCID, TRUE, pTCTitle,NULL, f_rgoModuleMap );
                        lCurrentTCID = 0;

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
                        lCurrentTCID = lNextTCID;
                        SAFE_OEM_FREE( pTCTitle );

                        ChkMem(pTCTitle = StringDup(pNextTCTitle));
                        if( lEndTCID == lNextTCID )
                        {
                            fLastTestCase = TRUE;
                        }
                        if (DRM_FAILED(dr = StartTestCase(lCurrentTCID, pTCTitle, eTCType, f_rgoModuleMap))) {
                            dr = EndTestCase(lCurrentTCID, FALSE, pTCTitle,NULL, f_rgoModuleMap);
                            lCurrentTCID = 0;
                        }
                    }
                
                }
                else if (lCurrentTCID && ParseAPI(pLine, &APIParams)) 
                {
                    DRM_BOOL fFail = FALSE;
                    DRM_BOOL fEndEarly = FALSE;
                    long i=0;
                    long j=0;

                    for( i=0; i < cNameValuePairs; i++ )
                    {
                       for( j=0; j < APIParams.m_lParamCount ; j++)
                       {
                           if ( NULL != APIParams.m_szParams[j] 
                             && NULL != pstrArgNames[i]
                             && 0    == strcmp( APIParams.m_szParams[j], pstrArgNames[i] ) )
                           {
                                long cLen = 0;
                                
                                SAFE_OEM_FREE( APIParams.m_szParams[j] );
                                cLen = ( DRM_DWORD ) strlen(pstrArgValues[i]);
                                
                                ChkMem( APIParams.m_szParams[j] = (char*)Oem_MemAlloc( (cLen+1) * SIZEOF(char) ) );
                                DRM_STR_StringCchCopyNA(APIParams.m_szParams[j], cLen+1, pstrArgValues[i], cLen );
                                APIParams.m_szParams[j][cLen] = 0;                                
                            }
                        }
                    }

                    if ( fInIf )
                    {
                        if( strcmp( APIParams.m_szAPIName, ENDIF ) == 0 )
                        {
                            /*
                            ** do nothing here.  do something elsewhere
                            */
                        }
                        else if( fIfConditionMet )
                        {
                            if( strcmp( APIParams.m_szAPIName, BEGINELSEIF ) == 0 || strcmp( APIParams.m_szAPIName, BEGINELSE ) == 0 )
                            {
                                fLookForEndIf = TRUE;
                            }
                            if( fLookForEndIf )
                            {
                                Log( "******", "Skipping API %s", APIParams.m_szAPIName );
                                continue;
                            }
                        }
                        else if( strcmp( APIParams.m_szAPIName, BEGINELSEIF ) == 0 )
                        {
                            Log( "******",  "ElseIf" );
                            ChkBOOL( fInElse != TRUE, DRM_E_FAIL ); /* elseif must not be in an else block */

                            ChkDR( TST_UTL_IsConditionTrue( &fIfConditionMet, APIParams.m_lParamCount, (const char **)APIParams.m_szParams ) );
                            if ( fIfConditionMet )
                            {
                                Log( "******", "ElseIf : One or more of the conditions were met" );
                            }
                            continue;
                        }
                        else if( strcmp( APIParams.m_szAPIName, BEGINELSE ) == 0 )
                        {
                            Log( "******",  "Else" );
                            ChkBOOL( fInElse != TRUE, DRM_E_FAIL ); /* do not allow nesting of else */
                            fInElse = TRUE;
                            continue;
                        }
                        else if( !fInElse )
                        {
                            Log( "******", "Skipping API %s", APIParams.m_szAPIName );
                            continue;
                        }
                    }

                    ChkDR( LogTestAPI( APIParams ) );
                    if( strcmp( APIParams.m_szAPIName, BEGIN_REPEAT_LOOP ) == 0 )
                    {
                        lLoopDepth++;
                        ChkBOOL( lLoopDepth < MAX_NESTED_LOOPS, DRM_E_WIN32_ERROR_TOO_MANY_CMDS );
                        ChkArg( APIParams.m_lParamCount >= 1 );
                        ChkArg( APIParams.m_szParams[0] != NULL );
                        rgLoopEntries[lLoopDepth].lBeginOfLoopFilePosition = ftell( fTestCase );
                        rgLoopEntries[lLoopDepth].lCurrentLoop = strtol( APIParams.m_szParams[0], NULL, 0 );
                        rgLoopEntries[lLoopDepth].eLoopType = ELOOPTYPE_REPEAT;
                        Log( "******", "Loop %d times", rgLoopEntries[lLoopDepth].lCurrentLoop );
                    }
                    else if( strcmp( APIParams.m_szAPIName, ENDOFLOOP ) == 0 )
                    {
                        ChkArg( lLoopDepth >= 0 );

                        if( rgLoopEntries[lLoopDepth].eLoopType == ELOOPTYPE_REPEAT )
                        {
                            rgLoopEntries[lLoopDepth].lCurrentLoop--;
                            if( rgLoopEntries[lLoopDepth].lCurrentLoop > 0 )
                            {
                                Log( "******", "Loop again %d times", rgLoopEntries[lLoopDepth].lCurrentLoop );
                                fseek( fTestCase, rgLoopEntries[lLoopDepth].lBeginOfLoopFilePosition, SEEK_SET );
                            }
                            else
                            {
                                Log( "******", "Exiting loop" );
                                lLoopDepth--;
                            }
                        }
                        else
                        {
                            ChkDR( DRM_E_FAIL );
                        }
                    }
                    else if( strcmp( APIParams.m_szAPIName, LOGCOMMENT ) == 0 )
                    {
                        // do nothing
                    }
                    else if ( strcmp( APIParams.m_szAPIName, BEGINIF ) == 0 )
                    {
                        ChkBOOL( fInIf != TRUE, DRM_E_FAIL ); // do not allow nesting
                        fInIf = TRUE;

                        ChkDR( TST_UTL_IsConditionTrue( &fIfConditionMet, APIParams.m_lParamCount, (const char **)APIParams.m_szParams ) );
                        if ( fIfConditionMet )
                        {
                            Log( "******", "If : One or more of the conditions were met" );
                        }
                        continue;
                    }
                    else if( strcmp( APIParams.m_szAPIName, ENDIF ) == 0 )
                    {
                        ChkBOOL( fInIf == TRUE, DRM_E_FAIL );   /* endif must be in an if, elseif, or else block */
                        fInElse = FALSE;
                        fIfConditionMet = FALSE;
                        fInIf = FALSE;
                        fLookForEndIf = FALSE;
                        continue;
                    }
                    else if ( strcmp( APIParams.m_szAPIName, BEGINELSEIF ) == 0 )
                    {
                        ChkBOOL( fInIf == TRUE, DRM_E_FAIL );   /* elseif must be in an if or elseif block */
                        ChkBOOL( fInElse != TRUE, DRM_E_FAIL ); /* elseif must not be in an else block */
                        continue;
                    }
                    else if ( strcmp( APIParams.m_szAPIName, BEGINELSE ) == 0 )
                    {
                        ChkBOOL( fInIf == TRUE, DRM_E_FAIL );   /* else must be in an if or elseif block */
                        ChkBOOL( fInElse != TRUE, DRM_E_FAIL ); /* else must not be in an else block */
                        continue;
                    }
                    else
                    {
                        ChkDR( RunTestAPI( APIParams, &fFail, &fEndEarly, NULL, NULL, szTraceMessage, SIZEOF( szTraceMessage ), FALSE, f_rgoModuleMap ) );
                    }

                    if ( fFail || fEndEarly ) 
                    {   
                        /*
                        ** api test failed or need to end test early 
                        */
                        dr = EndTestCase(lCurrentTCID, !fFail, pTCTitle, fFail ? szTraceMessage : NULL, f_rgoModuleMap);
                        lCurrentTCID = 0;
                        fInElse = FALSE;
                        fIfConditionMet = FALSE;
                        fInIf = FALSE;
                        fLookForEndIf = FALSE;
                        if (fLastTestCase) 
                        {
                            /* 
                            ** We just ran the required test case, no more left, so leave now.
                            */
                            goto ErrorExit;
                        }
                    }
                }
            }

            if (lCurrentTCID) 
            {
                /*
                ** Test case CurrentTCID is finished and passed, log it
                */
                dr = EndTestCase(lCurrentTCID, TRUE, pTCTitle, NULL, f_rgoModuleMap);
                lCurrentTCID=0;
                if (fLastTestCase) /* We just ran the required test case, no more left, so leave now.*/
                    goto ErrorExit;
            }

            fclose(fTestCase);
            fTestCase = NULL;
        }
    }

    if (!fTCFilePresent)
        printf("Cannot find any test case file.\n");
ErrorExit:

    for( i =0; i < cNameValuePairs ; i++ )
    {
        SAFE_OEM_FREE( pstrArgNames[i] );
        SAFE_OEM_FREE( pstrArgValues[i] );
    }
    
    SAFE_OEM_FREE(pTCTitle);
    SAFE_OEM_FREE( pszStdOutLogFile );
    tFREEFILENAMELIST(szTestScriptFiles);
    FreeAPIParams(&APIParams);
    SAFE_OEM_FREE(szLine);
    
    if (fTestCase)
        fclose(fTestCase);
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
    	printf("argv[%d] = %s\n", iArg, argv[iArg]);
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

    ChkDR( MakeDRMString( &dstrDest, argv[1] ) );

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
        ChkDR( DRM_ERR_GetErrorCodeFromStringA( argv[0], DRMCRT_strlen(argv[0]), &g_drExamine ) );
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
    API_ENTRY(GenerateRandomData)
    API_ENTRY(TestCopyFile)
    API_ENTRY(TestCopyFuzzFile)
    API_ENTRY(BinaryCompareFiles)
    API_ENTRY(Test_EnsureAsserted)
    API_ENTRY(SetResultTrap)
END_TEST_API_LIST

IMPLEMENT_DEFAULT_PRETESTCASE_AND_POSTTESTCASE_FOR_MODULE(TestCore)

EXIT_PKTEST_NAMESPACE_CODE
