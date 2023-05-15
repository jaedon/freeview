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
#include "drmcommon.h"
#include "drmerror.h"
#include "apiparams.h"

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

int DRM_CALL ParseTCID( __in_z char* szLine, long *pTCID, __deref_out_z_opt char **ppTCTitle)
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

DRM_RESULT ParseTCType(__in_z char *ppTCTitle, ENUM_TESTCASE_TYPE *peTCType)
{
    DRM_RESULT dr = DRM_SUCCESS;
    
    ChkArg( peTCType != NULL );

    *peTCType = eUnknown;

    if( ppTCTitle == NULL )
    {
        goto ErrorExit;
    }

    if( strncmp( ppTCTitle, TCTITLE_BVT_PREFIX, strlen( TCTITLE_BVT_PREFIX) ) == 0 )
    {
        *peTCType = eBVT;
    }
    else if( strncmp( ppTCTitle, TCTITLE_FUNC_PREFIX, strlen( TCTITLE_FUNC_PREFIX) ) == 0 )
    {
        *peTCType = eFunc;
    }
    else if( strncmp( ppTCTitle, TCTITLE_DRT_PREFIX, strlen( TCTITLE_DRT_PREFIX) ) == 0 )
    {
        *peTCType = eDRT;
    }
    else if( strncmp( ppTCTitle, TCTITLE_PERF_PREFIX, strlen( TCTITLE_PERF_PREFIX) ) == 0 )
    {
        *peTCType = ePerf;
    }
    
ErrorExit:
    return dr;
}

char* StringDup( __in_z char *pStr)
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
    DRM_RESULT  dr         = DRM_E_FAIL;
    DRM_RESULT  drExpected = DRM_SUCCESS;
    char       *pszEnd     = NULL;

    DRMASSERT( NULL != s );
    /*
    ** first see if there is a space or comment character
    ** remaining in the line.
    */

    pszEnd = (char *) s;
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
    dr = DRM_ERR_GetErrorCodeFromStringA( s, ( pszEnd - s ), &drExpected );

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

static long GetParamCount( __in_z char *pLine)
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
*/
int DRM_CALL ParseAPI( __in_z char *pLine, TAPIParams *pParams )
{
    EToken token;
    long lParamCount;
    char *pCurToken;

    FreeAPIParams(pParams);

    lParamCount = GetParamCount(pLine);
    pParams->m_drExpected = DRM_SUCCESS;
    pParams->m_fExcept = FALSE;
    pParams->m_lParamCount = 0;
    if (lParamCount) {
        pParams->m_szParams = (char**)Oem_MemAlloc(sizeof(char*) * lParamCount);
        memset(pParams->m_szParams, 0, sizeof(char*) * lParamCount);
    }

    pCurToken = GetToken(pLine, &token);
    if (pCurToken) {
        do {
            switch (token) {
                case T_FUNCTION_NAME:
                    pParams->m_szAPIName = StringDup(pCurToken);
                    break;
                case T_EXP_VALUE:
                    if (*pCurToken == '!') {
                        pParams->m_fExcept = TRUE;
                        pCurToken++;
                    } else if (*pCurToken == '^') {
                        pParams->m_fSkipIfThisError = TRUE;
                        pCurToken++;
                        if (*pCurToken != '?')
                        {
                            pParams->m_dwSkipNumberOfLines = atol(pCurToken);
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
                            pParams->m_dwSkipNumberOfLines = APIPARAMS_SKIP_TO_END;
                        }
                        pCurToken++;
                    }
                    pCurToken = TrimSpace( pCurToken );
                    pParams->m_drExpected = TranslateHr(pCurToken);
                    break;
                case T_PARAMETER:
                    if (pParams->m_lParamCount < lParamCount) {
                        if (!strcmp(pCurToken, "NULL")) /* if the parameter is "NULL", then set the value to NULL */
                            pParams->m_szParams[pParams->m_lParamCount] = NULL;

                        else if (!strcmp(pCurToken, "EMPTY")) /* if the parameter is "EMPTY", then set the value to an empty string */
                            pParams->m_szParams[pParams->m_lParamCount] = StringDup("");
                        
                        else
                            pParams->m_szParams[pParams->m_lParamCount] = StringDup(pCurToken);
                        
                        pParams->m_lParamCount++;
                    }
                    break;
            }
            pCurToken = GetToken( NULL, &token );
        } while ( pCurToken != NULL );
    }

    return (pParams->m_szAPIName != NULL);
}

