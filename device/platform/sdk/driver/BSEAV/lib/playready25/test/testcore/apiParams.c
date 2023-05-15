/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include <drmcommon.h>
#include <drmerror.h>
#include <drmmathsafe.h>
#include <apiParams.h>

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

ENTER_PKTEST_NAMESPACE_CODE;

extern DRM_RESULT DRM_CALL GetValueFromLoopData(
    __in const                 DRM_DWORD     f_iLoopLevel,
    __in const                 DRM_VOID     *f_prgLoopEntries,
    __in const                 DRM_CHAR     *f_pchToken,
    __out_bcount(f_cchLength)  DRM_CHAR     *f_pchConfigVar,
    __in const                 DRM_DWORD     f_cchLength );


#define MAX_PARAM_SIZE 1024


char* TrimSpaceLeft(__inout_z_opt char *s)
{
    if (s) {
        while (*s && isspace((unsigned char)*s))
            s++;
    }
    return s;
}

char* TrimSpace(__inout_z_opt char *s)
{
    char *pEnd;
    s = TrimSpaceLeft(s);
    if (s) {
        pEnd = s + strlen(s);
        while (pEnd > s && isspace((unsigned char)*(pEnd - 1)))
            pEnd--;
        *pEnd = '\0';
    }
    return s;
}

int DRM_CALL ParseTCID( __in_z char* szLine, long *pTCID, __deref_opt_out_z_opt char **ppTCTitle)
{
    char *pTitle;

    if (!isdigit((unsigned char)*szLine))
        return 0;

    if (pTCID)
        *pTCID = atol(szLine);

    if (ppTCTitle) {
        pTitle = strchr(szLine, ':');
        *ppTCTitle = pTitle? TrimSpace(pTitle + 1) : NULL;
    }
    return 1;
}

#define TCTITLE_DRT_PREFIX "DRT:"
#define TCTITLE_BVT_PREFIX "BVT:"
#define TCTITLE_FUNC_PREFIX "FUNC:"
#define TCTITLE_PERF_PREFIX "PERF:"
#define TCTITLE_FUZZ_PREFIX "FUZZ:"
#define TCTITLE_DISABLED_PREFIX "DISABLED:"

DRM_RESULT ParseTCType(__in_z const char *ppTCTitle, ENUM_TESTCASE_TYPE *peTCType)
{
    DRM_RESULT  dr           = DRM_SUCCESS;
    const char *pAfterPrefix = NULL;

    ChkArg( peTCType != NULL );

    *peTCType = eUnknown;

    if( ppTCTitle == NULL )
    {
        goto ErrorExit;
    }

    if( strncmp( ppTCTitle, TCTITLE_BVT_PREFIX, strlen( TCTITLE_BVT_PREFIX) ) == 0 )
    {
        *peTCType = eBVT;
        pAfterPrefix = ppTCTitle + strlen( TCTITLE_BVT_PREFIX);
    }
    else if( strncmp( ppTCTitle, TCTITLE_FUNC_PREFIX, strlen( TCTITLE_FUNC_PREFIX) ) == 0 )
    {
        *peTCType = eFunc;
        pAfterPrefix = ppTCTitle + strlen( TCTITLE_FUNC_PREFIX);
    }
    else if( strncmp( ppTCTitle, TCTITLE_DRT_PREFIX, strlen( TCTITLE_DRT_PREFIX) ) == 0 )
    {
        *peTCType = eDRT;
        pAfterPrefix = ppTCTitle + strlen( TCTITLE_DRT_PREFIX);
    }
    else if( strncmp( ppTCTitle, TCTITLE_FUZZ_PREFIX, strlen( TCTITLE_FUZZ_PREFIX) ) == 0 )
    {
        *peTCType = eFuzz;
        pAfterPrefix = ppTCTitle + strlen( TCTITLE_FUZZ_PREFIX);
    }
    else if( strncmp( ppTCTitle, TCTITLE_PERF_PREFIX, strlen( TCTITLE_PERF_PREFIX) ) == 0 )
    {
        *peTCType = ePerf;
        pAfterPrefix = ppTCTitle + strlen( TCTITLE_PERF_PREFIX);
    };

    if( pAfterPrefix != NULL && ( strncmp( pAfterPrefix, TCTITLE_DISABLED_PREFIX, strlen( TCTITLE_DISABLED_PREFIX) ) == 0 ) )
    {
        *peTCType = eDisabled;
    }


ErrorExit:
    return dr;
}

char* StringDup( __in_z const char *pStr)
{
    char *pOut = NULL;
    DRM_DWORD len;
    len = (DRM_DWORD)strlen(pStr) + 1;
    pOut = (char*)Oem_MemAlloc(len);
    if ( pOut != NULL )
    {
        MEMCPY(pOut, pStr, len);
    }
    return pOut;
}

void FreeAPIParams(TAPIParams *pParams)
{
    int i;
    for (i = 0; i < pParams->m_lParamCount; i++)
        SAFE_OEM_FREE(pParams->m_szParams[i]);

    SAFE_OEM_FREE(pParams->m_szParams);
    SAFE_OEM_FREE(pParams->m_szAPIName);
    memset(pParams, 0, sizeof(TAPIParams));
}

DRM_RESULT TranslateHr(const char* s)
{
    DRM_RESULT         dr         = DRM_E_FAIL;
    DRM_RESULT         drExpected = DRM_SUCCESS;
    char       const  *pszEnd     = NULL;
    DRM_DWORD          cch        = 0;

    DRMASSERT( NULL != s );
    /*
    ** first see if there is a space or comment character
    ** remaining in the line.
    */

    pszEnd = s;
    while( pszEnd  != NULL
        && *pszEnd != '\0'
        && !isspace( ( unsigned char ) *pszEnd ) )
    {
        pszEnd++;
    }

    /*
    ** Now look for the error in the list.
    ** This will parse the numeric error if given
    ** numerically.
    */
    dr = DRM_DWordPtrToDWord( (DRM_DWORD_PTR)( (DRM_DWORD_PTR)pszEnd - (DRM_DWORD_PTR)s ), &cch );
    if( DRM_SUCCEEDED( dr ) )
    {
        dr = DRM_ERR_GetErrorCodeFromStringA( s, cch, &drExpected );
    }

    DRMASSERT( DRM_SUCCEEDED(dr) );

    return drExpected;
}

typedef enum {T_FUNCTION_NAME, T_PARAMETER, T_EXP_VALUE} EToken;

static char* GetToken( __in_z_opt char *pLine, EToken *pToken)
{
    static EToken tokenNext = T_FUNCTION_NAME;
    static char *pNext = NULL;
    char *pOut, *p, *q;
    if ( NULL != pLine )
    {
        pNext = pLine;
        tokenNext = T_FUNCTION_NAME;
    }

    if ( NULL == pNext || '\0' == *pNext )
    {
        return NULL;
    }

    pOut = pNext = TrimSpaceLeft( pNext );
    *pToken = tokenNext;

    switch (tokenNext)
    {
    case T_FUNCTION_NAME:
        p = strchr(pOut, '(');
        q = strchr(pOut, '=');
        if ( NULL != p
          && ( NULL == q || q > p ) )
        {
            *p = 0;
            pNext = p + 1;
            tokenNext = T_PARAMETER;
        }
        else if ( NULL != q
               && ( NULL == p || p > q))
        {
            *q = 0;
            pNext = q + 1;
            tokenNext = T_EXP_VALUE;
        }
        else
        {
            pNext = NULL;
        }
        break;
    case T_PARAMETER:
        if (*pOut == '"')  /* Search the ending quote for the literal string */
        {
            pOut++;
            p = pOut; q = pOut;
            while ( '\0' != *q && *q != '"' )
            {
                if ( *q == '\\' ) /* '\' is the escape char */
                {
                    q++;
                }
                if ( '\0' == *q )
                {
                    break;
                }
                *p++ = *q++;
            }
            pNext = ( '\0' != *q )? q + 1 : NULL;
            *p = '\0';
        }
        if ( pNext != NULL )
        {
            p = strchr( pNext, ',' );
            q = strchr( pNext, ')' );

            /*
            ** if we can't find a ')' we have an error.
            */
            if( q == NULL )
            {
                DRMASSERT( FALSE );
                return NULL;
            }
            if ( NULL != p && q > p )
            {
                *p = '\0';
                pNext = p + 1;
            }
            else if ( NULL == p || p > q )
            {
                *q = '\0';
                q = strchr(q + 1, '=');
                pNext = ( NULL != q ) ? q + 1 : NULL;
                tokenNext = T_EXP_VALUE;
            }
        }
        break;
    case T_EXP_VALUE:
        pNext = NULL;
        break;
    }
    return TrimSpace( pOut );
}

static long _GetParamCount( __in_z const char *pLine)
{
    long count = 0;
    EToken token;
    char *s = NULL;

    if ( pLine != NULL && strlen( pLine ) > 0 )
    {
        s = StringDup(pLine);

        if (GetToken(s, &token)) {
            do {
                if (token == T_PARAMETER)
                    count++;
            } while (GetToken(NULL, &token));
        }

        SAFE_OEM_FREE(s);
    }

    return count;
}


static DRM_BOOL _GetConfigParam(
    __in  const DRM_CHAR    *f_pchCurrentToken,
    __out       DRM_CHAR   **f_pchNewVar,
    __in  const DRM_DWORD    f_nLoopLevel,
    __in  const DRM_VOID    *f_prgLoopEntries )
{
    DRM_RESULT    dr                          = DRM_E_NOT_FOUND;
    DRM_CHAR     *pVar                        = NULL;
    DRM_DWORD     iNewVar                     = 0;
    DRM_CHAR     *pToken                      = (DRM_CHAR *)f_pchCurrentToken;
    DRM_CHAR     *pConfigVar                  = NULL;
    DRM_BOOL      fRC                         = FALSE;
    DRM_CHAR      rgszValue[ MAX_PARAM_SIZE ] = { 0 };
    DRM_DWORD     cchValue                    = 0;

#if defined ( _X86_ )
    DRM_CHAR  const *pszArch   = "x86";
#else
    DRM_CHAR  const *pszArch   = "amd64";
#endif

#if defined (DBG)
    DRM_CHAR  const *pszFlavor = "chk";
#else
    DRM_CHAR  const *pszFlavor = "fre";
#endif


    ChkArg( f_pchCurrentToken != NULL );
    ChkArg( f_pchNewVar       != NULL );
    ChkArg( *f_pchNewVar      == NULL );

    ChkMem( pConfigVar = (DRM_CHAR *)Oem_MemAlloc( MAX_PARAM_SIZE ) );
    MEMSET( pConfigVar, '\0', MAX_PARAM_SIZE );

    while ( *pToken != '\0' )
    {
        /*
        ** Search for '%' in the token, for example: "c:\playready\%Param0%"
        */
        while ( *pToken != '%' && *pToken != '\0' && iNewVar < MAX_PARAM_SIZE )
        {
            pConfigVar[iNewVar] = *pToken;
            pToken++;
            iNewVar++;
        }

        if ( *pToken == '\0' || iNewVar == MAX_PARAM_SIZE )
        {
            break;
        }

        pVar = strchr( pToken + 1, '%' );
        if( pVar == NULL )
        {
            /*
            ** no ending %.  copy rest of string
            */
            while ( *pToken != '\0' && iNewVar < MAX_PARAM_SIZE )
            {
                pConfigVar[iNewVar] = *pToken;
                pToken++;
                iNewVar++;
            }
            break;
        }

        *pVar = '\0';
        pVar++; /* This now points after the token */
        pToken++;

        if( 0 == DRMCRT_strncmp( pToken, "Arch", 4 ) )
        {
            ChkDR( DRM_STR_StringCchCopyA( rgszValue, MAX_PARAM_SIZE, pszArch ) );
        }
        else if( 0 == DRMCRT_strncmp( pToken, "Flavor", 6 ) )
        {
            ChkDR( DRM_STR_StringCchCopyA( rgszValue, MAX_PARAM_SIZE, pszFlavor ) );
        }
        else if( f_prgLoopEntries != NULL )
        {
            dr = GetValueFromLoopData( f_nLoopLevel, f_prgLoopEntries, pToken, rgszValue, MAX_PARAM_SIZE );
            if( DRM_FAILED( dr ) && ( dr != DRM_E_NOT_FOUND ) )
            {
                ChkDR( dr );
            }
        }

        if( DRM_FAILED( dr ) )
        {
            /*
            ** config data not found.  Try rest of string
            */
            pToken = pVar;
            dr = DRM_SUCCESS;
            continue;
        }

        ChkDR( DRM_SizeTToDWord( DRMCRT_strlen( rgszValue ), &cchValue ) );
        DRM_BYT_CopyBytes( pConfigVar, iNewVar, rgszValue, 0, MIN( cchValue,( MAX_PARAM_SIZE - iNewVar ) ) );

        pToken = pVar;
    }

    *f_pchNewVar = pConfigVar;
    if ( pConfigVar != NULL )
    {
        fRC = TRUE;
    }
    pConfigVar = NULL;

ErrorExit:
    SAFE_OEM_FREE( pConfigVar );
    if ( DRM_FAILED( dr ) )
    {
        fRC = FALSE;
    }
    return fRC;

}


/*
    Parse an API line in the text file to the TAPIParams struct.
    All these are valid api lines:
    API_Name
    API_Name = ExpValue
    API_Name(param)
    API_Name(param)=ExpValue
    API_Name(param1, param2, param3)=ExpValue
    API_Name("Param(v1, v2)", param2)
    API_Name(param1, "Param(\"string\")", param2)
    API_Name(param1, "param is backslash\\")
    API_Name(%Param1%) // Param1 will be looked for in a configuration file and if found will be used
*/
int DRM_CALL ParseAPI(
    __in_z     DRM_CHAR    *f_pLine,
    __inout    TAPIParams  *f_pParams,
    __in const DRM_DWORD    f_nLoopLevel,
    __in const DRM_VOID    *f_prgLoopEntries )
{
    DRM_RESULT    dr                  = DRM_SUCCESS;
    EToken        token               = T_FUNCTION_NAME;
    long          lParamCount         = 0;
    DRM_CHAR     *pCurToken           = NULL;
    DRM_CHAR     *pchActualParam      = NULL;
    DRM_CHAR     *pchCurrentTokenCopy = NULL;

    FreeAPIParams( f_pParams );

    lParamCount = _GetParamCount( f_pLine );
    f_pParams->m_drExpected = DRM_SUCCESS;
    f_pParams->m_fExcept = FALSE;
    f_pParams->m_lParamCount = 0;
    if ( lParamCount > 0 )
    {
        ChkMem( f_pParams->m_szParams = (char**)Oem_MemAlloc(sizeof(char*) * lParamCount) );
        MEMSET( f_pParams->m_szParams, 0, sizeof(char*) * lParamCount );
    }

    pCurToken = GetToken( f_pLine, &token );
    if ( pCurToken != NULL )
    {
        do
        {
            switch ( token )
            {
                case T_FUNCTION_NAME:
                    ChkMem( f_pParams->m_szAPIName = StringDup( pCurToken ) );
                    break;

                case T_EXP_VALUE:
                    if (*pCurToken == '!')
                    {
                        f_pParams->m_fExcept = TRUE;
                        pCurToken++;
                    }
                    else if (*pCurToken == '^')
                    {
                        f_pParams->m_fSkipIfThisError = TRUE;
                        pCurToken++;
                        if (*pCurToken != '?')
                        {
                            f_pParams->m_dwSkipNumberOfLines = (DRM_DWORD)atol(pCurToken);
                            pCurToken = strchr(pCurToken, '?');
                            if( pCurToken == NULL )
                            {
                                /* Missing the ? tag */
                                DRMASSERT( FALSE );
                                break;
                            }
                        }
                        else
                        {
                            f_pParams->m_dwSkipNumberOfLines = APIPARAMS_SKIP_TO_END;
                        }
                        pCurToken++;
                    }

                    ChkMem( pchCurrentTokenCopy = StringDup( pCurToken ) );
                    if ( _GetConfigParam( pchCurrentTokenCopy, &pchActualParam, f_nLoopLevel, f_prgLoopEntries ) )
                    {
                        f_pParams->m_drExpected = TranslateHr( pchActualParam );
                    }
                    else
                    {
                        pCurToken = TrimSpace( pCurToken );
                        f_pParams->m_drExpected = TranslateHr( pCurToken );
                    }
                    SAFE_OEM_FREE( pchActualParam );
                    SAFE_OEM_FREE( pchCurrentTokenCopy );
                    break;

                case T_PARAMETER:
                    if ( f_pParams->m_lParamCount < lParamCount )
                    {
                        if (!strcmp(pCurToken, "NULL")) /* if the parameter is "NULL", then set the value to NULL */
                        {
                            f_pParams->m_szParams[ f_pParams->m_lParamCount ] = NULL;
                        }
                        else if (!strcmp(pCurToken, "EMPTY")) /* if the parameter is "EMPTY", then set the value to an empty string */
                        {
                            ChkMem( f_pParams->m_szParams[ f_pParams->m_lParamCount ] = StringDup("\0") );
                        }
                        else
                        {
                            ChkMem( pchCurrentTokenCopy = StringDup( pCurToken ) );
                            if ( _GetConfigParam( pchCurrentTokenCopy, &pchActualParam, f_nLoopLevel, f_prgLoopEntries ) )
                            {
                                /*
                                ** Replace with the new parameter from the config file
                                */
                                ChkMem( f_pParams->m_szParams[ f_pParams->m_lParamCount ] = StringDup( pchActualParam ) );
                            }
                            else
                            {
                                /*
                                ** Use existing parameter from the function call itself
                                */
                                ChkMem( f_pParams->m_szParams[ f_pParams->m_lParamCount ] = StringDup( pCurToken ) );
                            }
                            SAFE_OEM_FREE( pchActualParam );
                            SAFE_OEM_FREE( pchCurrentTokenCopy );
                        }

                        f_pParams->m_lParamCount++;
                    }
                    break;
            }
            pCurToken = GetToken( NULL, &token );
        } while ( pCurToken != NULL );
    }

ErrorExit:
    SAFE_OEM_FREE( pchActualParam );
    SAFE_OEM_FREE( pchCurrentTokenCopy );
    if( DRM_FAILED( dr ) )
    {
        return 0;
    }
    else
    {
        return ( f_pParams->m_szAPIName != NULL );
    }
}

EXIT_PKTEST_NAMESPACE_CODE

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

