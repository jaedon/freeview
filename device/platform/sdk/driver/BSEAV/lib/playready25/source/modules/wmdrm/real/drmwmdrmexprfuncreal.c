/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMWMDRMEXPRFUNCREAL_C
#include <drmwmdrm.h>
#include <drmutilitieslite.h>
#include <drmwmdrmconstants.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/* Put the function names in a table with arity values */
typedef struct tagDRM_EXPR_FUNCTIONS
{
    const DRM_CONST_STRING *pdstrFunction;
    DRM_LONG Arity;
    DRM_EXPR_FUNCTION_TYPE FnType;
} DRM_EXPR_FUNCTIONS;

static const DRM_EXPR_FUNCTIONS g_FncsArity[] =
{
    { &g_dstrExprFuncDateAdd,        3, FN_DATEADD        },
    { &g_dstrExprFuncDateDiff,       3, FN_DATEDIFF       },
    { &g_dstrExprFuncMin,            2, FN_MIN            },
    { &g_dstrExprFuncMax,            2, FN_MAX            },
    { &g_dstrExprFuncDatePart,       2, FN_DATEPART       },
    { &g_dstrExprFuncVersionCompare, 2, FN_VERSIONCOMPARE },
    { &g_dstrExprFuncDeleteLicense,  0, FN_DELETELICENSE  },
    { &g_dstrExprFuncExists,         1, FN_EXISTS         },
};

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_WMDRM_EXPRFUNC_IsValidFunction(
    __in const DRM_CONST_STRING *pStringToken,
    __in       DRM_DWORD        *pfnType )
{
    DRM_DWORD i;
    for( i = 0; i < NO_OF( g_FncsArity ); i++ )
    {
        if( DRM_UTL_DSTRStringsEqual( pStringToken, g_FncsArity[i].pdstrFunction ) )
        {
            *pfnType = g_FncsArity[i].FnType;
            return TRUE;
        }
    }
    return FALSE;
}


DRM_API DRM_NO_INLINE DRM_LONG DRM_CALL DRM_WMDRM_EXPRFUNC_Arity(DRM_EXPR_FUNCTION_TYPE fnType)
{
    DRM_DWORD i;
    for( i = 0; i < NO_OF( g_FncsArity ); i++ )
    {
        if( g_FncsArity[i].FnType == fnType )
        {
            return g_FncsArity[i].Arity;
        }
    }
    /* We shouldn't get here -- if we did then this wasn't a valid function.
    Callers should call DRM_WMDRM_EXPRFUNC_IsValidFunction to validate the name first */
    /* ASSERT( 0 ) */
    return -1;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_EXPRFUNC_FunctionGetValue(
    __in const TOKEN                       *pOperator,
    __in       DRM_LONG                     cArgs,
    __in       TOKEN                        rgArgs[],
    __out      TOKEN                       *pOperandResult,
    __in       DRM_WMDRM_EXPR_EVAL_CONTEXT *pEEContext )
{
    DRM_RESULT           dr              = DRM_SUCCESS;
    DRM_LONG             lArity          = 0;
    DRMSYSTEMTIME        date;
    DRMFILETIME          filetime;
    DRM_LICEVAL_CONTEXT *pcontextLicEval = (DRM_LICEVAL_CONTEXT *) pEEContext->pvOpaqueData;
    DRM_LONG             lCounter        = 0;
    DRM_INT64            i64Temp;

    DRMASSERT( pOperator->TokenType == TOKEN_FUNCTION );
    lArity = DRM_WMDRM_EXPRFUNC_Arity((DRM_EXPR_FUNCTION_TYPE)pOperator->val.fnValue);
    if (lArity != cArgs)
    {
        dr = DRM_E_CPRMEXP_INCORRECT_NUM_ARGS;
        goto ErrorExit;
    }

    if( pOperator->val.fnValue != FN_EXISTS )
    {
        /* For all functions other than FN_EXISTS we have to ensure that all varibles used actually do exist */
        /* Evaluate arguments that are variables before applying the function. */
        for (lCounter = 0; lCounter < lArity; lCounter++)
        {
            if (rgArgs[lCounter].TokenType == TOKEN_VARIABLE)
            {
                /* Get the variable and replace current rgArgs with it. */
                TOKEN Token = { TOKEN_VARIABLE };
                ChkDR( pEEContext->GetVariable( &(rgArgs[lCounter].val.stringValue), &(Token), pEEContext->pvOpaqueData ) );
                rgArgs[lCounter] = Token;
            }
        }
    }

    /* Everything looks ok. Now, write code for each function. */
    switch( pOperator->val.fnValue )
    {
    case FN_EXISTS:
        {
            TOKEN Token = { TOKEN_VARIABLE };
            if (rgArgs[lCounter].TokenType != TOKEN_VARIABLE)
            {
                dr = DRM_E_CPRMEXP_VARIABLE_EXPECTED;
                goto ErrorExit;
            }
            pOperandResult->TokenType = TOKEN_LONG;
            if( DRM_FAILED( pEEContext->GetVariable( &(rgArgs[lCounter].val.stringValue), &(Token), pEEContext->pvOpaqueData ) ) )
            {
                pOperandResult->val.lValue = 0;
            }
            else
            {
                pOperandResult->val.lValue = 1;
            }
        }
        dr = DRM_SUCCESS;
        break;
    case FN_DATEADD:
        if (rgArgs[0].TokenType != TOKEN_STRING || rgArgs[1].TokenType != TOKEN_LONG ||
            rgArgs[2].TokenType != TOKEN_DATETIME)
        {
            dr = DRM_E_CPRMEXP_WRONG_TYPE_OPERAND;
            goto ErrorExit;
        }
        pOperandResult->TokenType = TOKEN_DATETIME;
        if( rgArgs[0].val.stringValue.cchString > 1 )
        {
            dr = DRM_E_CPRMEXP_INVALID_ARGUMENT;
            goto ErrorExit;
        }
        if( rgArgs[0].val.stringValue.pwszString[0] == g_wchd )
        {
            i64Temp = DRM_I64(10000000);
            i64Temp = DRM_I64Mul( i64Temp, DRM_I64( 24uL * 60uL * 60uL ) );
        }
        else if( rgArgs[0].val.stringValue.pwszString[0] == g_wchh )
        {
            i64Temp = DRM_I64(10000000);
            i64Temp = DRM_I64Mul( i64Temp, DRM_I64( 60 * 60 ) );
        }
        else if( rgArgs[0].val.stringValue.pwszString[0] == g_wchn )
        {
            i64Temp = DRM_I64(10000000);
            i64Temp = DRM_I64Mul( i64Temp, DRM_I64( 60 ) );
        }
        else if( rgArgs[0].val.stringValue.pwszString[0] == g_wchs )
        {
            i64Temp = DRM_I64( 10000000 );
        }
        else
        {
            dr = DRM_E_CPRMEXP_INVALID_ARGUMENT;
            goto ErrorExit;
        }
        i64Temp = DRM_I64Mul( i64Temp, DRM_I64(rgArgs[1].val.lValue) );
        pOperandResult->val.u64DateTime = DRM_UI64Add( *(DRM_UINT64*)&i64Temp, rgArgs[2].val.u64DateTime );
        break;
    case FN_DATEDIFF:
        {
            DRM_UINT64  u64AbsDiff;
            DRM_LONG lSign;

            if ( rgArgs[0].TokenType != TOKEN_STRING
              || rgArgs[1].TokenType != TOKEN_DATETIME
              || rgArgs[2].TokenType != TOKEN_DATETIME )
            {
                dr = DRM_E_CPRMEXP_WRONG_TYPE_OPERAND;
                goto ErrorExit;
            }
            if ( DRM_UI64Les( rgArgs[1].val.u64DateTime, rgArgs[2].val.u64DateTime ) )
            {
                u64AbsDiff = DRM_UI64Sub( rgArgs[2].val.u64DateTime, rgArgs[1].val.u64DateTime );
                lSign      = 1;
            }
            else
            {
                u64AbsDiff = DRM_UI64Sub( rgArgs[1].val.u64DateTime, rgArgs[2].val.u64DateTime );
                lSign      = -1;
            }
            pOperandResult->TokenType = TOKEN_LONG;
            if( rgArgs[0].val.stringValue.cchString > 1 )
            {
                dr = DRM_E_CPRMEXP_INVALID_ARGUMENT;
                goto ErrorExit;
            }
            if( rgArgs[0].val.stringValue.pwszString[0] == g_wchd )
            {
                i64Temp = DRM_I64( 10000000 );
                i64Temp = DRM_I64Mul( i64Temp, DRM_I64( 24uL * 60uL * 60uL ) );
            }
            else if( rgArgs[0].val.stringValue.pwszString[0] == g_wchh )
            {
                i64Temp = DRM_I64( 10000000 );
                i64Temp = DRM_I64Mul( i64Temp, DRM_I64( 60 * 60 ) );
            }
            else if( rgArgs[0].val.stringValue.pwszString[0] == g_wchn )
            {
                i64Temp = DRM_I64( 10000000 );
                i64Temp = DRM_I64Mul( i64Temp, DRM_I64( 60 ) );
            }
            else if( rgArgs[0].val.stringValue.pwszString[0] == g_wchs )
            {
                i64Temp = DRM_I64( 10000000 );
            }
            else
            {
                dr = DRM_E_CPRMEXP_INVALID_ARGUMENT;
                goto ErrorExit;
            }
            pOperandResult->val.lValue = lSign * (DRM_LONG) DRM_I64ToUI32( DRM_I64Div( DRM_UI2I64( u64AbsDiff) , i64Temp) );
            break;
        }
    case FN_MIN:
        if (rgArgs[0].TokenType != TOKEN_LONG || rgArgs[1].TokenType != TOKEN_LONG)
        {
            dr = DRM_E_CPRMEXP_WRONG_TYPE_OPERAND;
            goto ErrorExit;
        }
        pOperandResult->TokenType = TOKEN_LONG;
        pOperandResult->val.lValue = min(rgArgs[0].val.lValue, rgArgs[1].val.lValue);
        break;
    case FN_MAX:
        if (rgArgs[0].TokenType != TOKEN_LONG || rgArgs[1].TokenType != TOKEN_LONG)
        {
            dr = DRM_E_CPRMEXP_WRONG_TYPE_OPERAND;
            goto ErrorExit;
        }
        pOperandResult->TokenType = TOKEN_LONG;
        pOperandResult->val.lValue = max(rgArgs[0].val.lValue, rgArgs[1].val.lValue);
        break;
    case FN_DATEPART:
        if (rgArgs[0].TokenType != TOKEN_STRING || rgArgs[1].TokenType != TOKEN_DATETIME)
        {
            dr = DRM_E_CPRMEXP_WRONG_TYPE_OPERAND;
            goto ErrorExit;
        }
        pOperandResult->TokenType = TOKEN_LONG;
        UI64_TO_FILETIME( rgArgs[1].val.u64DateTime, filetime );
        if (Oem_Clock_FileTimeToSystemTime(&filetime, &date))
        {
            if( rgArgs[0].val.stringValue.cchString > 1 )
            {
                dr = DRM_E_CPRMEXP_INVALID_ARGUMENT;
                goto ErrorExit;
            }
            if( rgArgs[0].val.stringValue.pwszString[0] == g_wchy )
            {
                pOperandResult->val.lValue = date.wYear;
            }
            else if( rgArgs[0].val.stringValue.pwszString[0] == g_wchm )
            {
                pOperandResult->val.lValue = date.wMonth;
            }
            else if( rgArgs[0].val.stringValue.pwszString[0] == g_wchd )
            {
                pOperandResult->val.lValue = date.wDay;
            }
            else if( rgArgs[0].val.stringValue.pwszString[0] == g_wchh )
            {
                pOperandResult->val.lValue = date.wHour;
            }
            else if( rgArgs[0].val.stringValue.pwszString[0] == g_wchn )
            {
                pOperandResult->val.lValue = date.wMinute;
            }
            else if( rgArgs[0].val.stringValue.pwszString[0] == g_wchs )
            {
                pOperandResult->val.lValue = date.wSecond;
            }
            else
            {
                dr = DRM_E_CPRMEXP_INVALID_ARGUMENT;
                goto ErrorExit;
            }
        }
        else
        {
            dr = DRM_E_CPRMEXP_INVALID_CONSTANT;
            goto ErrorExit;
        }
        break;
    case FN_VERSIONCOMPARE:
        {
            DRM_UINT64 ver1, ver2;
            DRM_INT64  i64;
            DRM_WORD rgwVersion[VERSION_LEN];

            if (rgArgs[0].TokenType != TOKEN_STRING || rgArgs[1].TokenType != TOKEN_STRING)
            {
                dr = DRM_E_CPRMEXP_WRONG_TYPE_OPERAND;
                goto ErrorExit;
            }
            pOperandResult->TokenType = TOKEN_LONG;

            ChkDR( DRM_UTL_GetVersionFromString(rgArgs[0].val.stringValue.pwszString, rgArgs[0].val.stringValue.cchString, rgwVersion) );
            i64 = DRM_I64Asgn( (((DRM_DWORD)rgwVersion[0]) << 16) | rgwVersion[1],
                           (((DRM_DWORD)rgwVersion[2]) << 16) | rgwVersion[3] );

            MEMCPY( &ver1, &i64, SIZEOF( DRM_INT64 ) );

            ChkDR( DRM_UTL_GetVersionFromString(rgArgs[1].val.stringValue.pwszString, rgArgs[1].val.stringValue.cchString, rgwVersion) );
            i64 = DRM_I64Asgn( (((DRM_DWORD)rgwVersion[0]) << 16) | rgwVersion[1],
                           (((DRM_DWORD)rgwVersion[2]) << 16) | rgwVersion[3] );

            MEMCPY( &ver2, &i64, SIZEOF( DRM_INT64 ) );

            if ( DRM_UI64Les( ver1, ver2 ) )
            {
                pOperandResult->val.lValue = -1;
            }
            else if ( DRM_UI64Eql( ver1, ver2 ) )
            {
                pOperandResult->val.lValue = 0;
            }
            else
            {
                pOperandResult->val.lValue = 1;
            }
        }
        break;
    case FN_DELETELICENSE:

        if ( pcontextLicEval == NULL )
        {
            dr = DRM_E_CPRMEXP_INVALID_CONTEXT;
            goto ErrorExit;
        }

        pOperandResult->TokenType = TOKEN_LONG;

        /* Just return success and set the delayed delete flag. */
        pOperandResult->val.lValue = 1;
        pcontextLicEval->fDeleteLicense = TRUE;
        break;
    default:
        dr = DRM_E_CPRMEXP_UNSUPPORTED_FUNCTION; /* This case will happen if we recognize a function name but not support it. */
    }

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

