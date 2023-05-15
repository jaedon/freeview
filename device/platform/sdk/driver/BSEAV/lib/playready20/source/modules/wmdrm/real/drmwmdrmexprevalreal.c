/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmwmdrm.h>
#include <drmutilitieslite.h>

ENTER_PK_NAMESPACE_CODE;

/* Operators we don't care to support anymore: */
/* ++, --, %, unary +, unary - */
/* maybe ! */

/* Supported functions */
/* exists, dateadd, datediff, datepart, min, max, long */
/* string, versioncompare, datetime, index, length, deletelicense */

static DRM_RESULT DRM_CALL ExtractStringToken( DRM_CONST_STRING* pdstrExpression, TOKEN* pToken );
static DRM_RESULT DRM_CALL ExtractSymbolToken( DRM_CONST_STRING* pdstrExpression, TOKEN* pToken );

static DRM_RESULT PerformOperation(
    TOKEN                       *pOperator,
    DRM_WMDRM_EXPR_EVAL_CONTEXT *pEEContext);

/* Stub'd functions */
static DRM_NO_INLINE DRM_RESULT EvalVariable(
    TOKEN                       *pToken,
    DRM_WMDRM_EXPR_EVAL_CONTEXT *pEEContext)
{
    TOKEN Token;
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( pEEContext->GetVariable( &(pToken->val.stringValue), &Token, pEEContext->pvOpaqueData ) );
    MEMCPY( pToken, &Token, SIZEOF( TOKEN ) );

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_BOOL IsOperand(DRM_EXPR_TOKEN_TYPE TokenType)
{
    return (
        TokenType == TOKEN_VARIABLE ||
        TokenType == TOKEN_STRING   ||
        TokenType == TOKEN_LONG     ||
        TokenType == TOKEN_DATETIME
        );
}

static DRM_BOOL IsPreUnaryOp(DRM_EXPR_TOKEN_TYPE TokenType)
{
    return (
        TokenType == TOKEN_PLUS     ||
        TokenType == TOKEN_MINUS    ||
        TokenType == TOKEN_NOT      ||
        TokenType == TOKEN_PREINCR  ||
        TokenType == TOKEN_PREDECR  ||
        TokenType == TOKEN_FUNCTION
        );
}

static DRM_NO_INLINE DRM_BOOL IsBinaryOp( DRM_EXPR_TOKEN_TYPE TokenType)
{
    return (
        TokenType == TOKEN_ADD     ||
        TokenType == TOKEN_SUB     ||
        TokenType == TOKEN_MUL     ||
        TokenType == TOKEN_DIV     ||
        TokenType == TOKEN_MOD     ||
        TokenType == TOKEN_ASSIGN  ||
        TokenType == TOKEN_LESS    ||
        TokenType == TOKEN_LESSEQ  ||
        TokenType == TOKEN_GREAT   ||
        TokenType == TOKEN_GREATEQ ||
        TokenType == TOKEN_NOTEQ   ||
        TokenType == TOKEN_EQ      ||
        TokenType == TOKEN_AND     ||
        TokenType == TOKEN_OR      ||
        TokenType == TOKEN_IF      ||
        TokenType == TOKEN_COLON   ||
        TokenType == TOKEN_COMMA   ||
        TokenType == TOKEN_SEMICOLON
        );
}

static DRM_NO_INLINE DRM_RESULT DRM_CALL ExtractStringToken(
    DRM_CONST_STRING *pdstrExpression,
    TOKEN            *pToken )
{
    DRM_RESULT dr = DRM_SUCCESS;
    const DRM_WCHAR* pwcStart = NULL;

    pwcStart = pdstrExpression->pwszString;       /* Now we are at the start of a token. */
    /* Loop until matching one is found. Don't forget escape symbol \. */
    /* First we need to keep track of the count to allocate space.         */
    while( pdstrExpression->cchString && *(pdstrExpression->pwszString) != g_wchQuote && *(pdstrExpression->pwszString) != g_wchNewLine )
    {

        if (*(pdstrExpression->pwszString) == g_wchBackSlash && pdstrExpression->cchString > 1)
        {
            /* Escape character followed by a non-null character. */
            pdstrExpression->pwszString += 2;
            pdstrExpression->cchString  -= 2;
        }
        else
        {
            pdstrExpression->pwszString++;
            pdstrExpression->cchString --;
        }
    }
    if ( *(pdstrExpression->pwszString) != g_wchQuote)
    {
        dr = CPRMEXP_STRING_UNTERMINATED;
        goto ErrorExit;
    }

    /* Skip the ending quote.          */
    pToken->val.stringValue.pwszString = pwcStart;
    pToken->val.stringValue.cchString = (DRM_DWORD) ( pdstrExpression->pwszString - pwcStart );

    pdstrExpression->pwszString++;
    pdstrExpression->cchString--;

ErrorExit:
    return dr;

}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_EXPR_ExtractLongToken(
    DRM_CONST_STRING *pdstrExpression,
    TOKEN            *pToken )
{
    DRM_RESULT  dr           = DRM_SUCCESS;
    DRM_DWORD   cchValue     = 0;

    pToken->TokenType = TOKEN_LONG;
    dr = DRM_UTL_StringToNumber( pdstrExpression, &pToken->val.lValue, &cchValue );
    DRMASSERT( cchValue <= pdstrExpression->cchString );
    pdstrExpression->cchString  -= cchValue;
    pdstrExpression->pwszString += cchValue;

    if( dr == DRM_E_INVALIDARG )
    {
        dr = CPRMEXP_INVALID_CONSTANT;
    }
    else if( dr == DRM_E_ARITHMETIC_OVERFLOW )
    {
        dr = CPRMEXP_OVERFLOW;
    }
    ChkDR( dr );

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT DRM_CALL ExtractSymbolToken(
    DRM_CONST_STRING *pdstrExpression,
    TOKEN            *pToken )
{
    const DRM_WCHAR* pwcStart    = NULL;
    DRM_RESULT dr = DRM_SUCCESS;

    pwcStart = pdstrExpression->pwszString;
    while ( pdstrExpression->cchString > 0 &&
            ( DRMCRT_iswalpha( *(pdstrExpression->pwszString) ) ||
            DRMCRT_iswdigit( *(pdstrExpression->pwszString) ) ||
            *(pdstrExpression->pwszString)== g_wchPeriod ||
            *(pdstrExpression->pwszString)== g_wchUnderscore) )
    {
        pdstrExpression->pwszString++;
        pdstrExpression->cchString--;
    }

    pToken->val.stringValue.pwszString = pwcStart;
    pToken->val.stringValue.cchString = (DRM_DWORD) ( pdstrExpression->pwszString - pwcStart );

    /* Check if it is a function symbol or a variable */
    /* Function symbol must follow (. Look ahead for (.     */
    while ( pdstrExpression->cchString > 0 &&
            DRMCRT_iswspace(*(pdstrExpression->pwszString ) ) )
    {
        pdstrExpression->pwszString++;
        pdstrExpression->cchString--;
    }
    if ( pdstrExpression->cchString > 0 && *(pdstrExpression->pwszString) == g_wchOpenParen )
    {
        if (DRM_WMDRM_EXPRFUNC_IsValidFunction(&(pToken->val.stringValue), &( pToken->val.fnValue ) ) )
        {
            pToken->TokenType = TOKEN_FUNCTION;
        }
        else
        {
            dr = CPRMEXP_INVALID_FUNCTION;
        }
    }
    else
    {
        pToken->TokenType = TOKEN_VARIABLE;
    }

    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_EXPR_ExtractDateToken(
    DRM_CONST_STRING *pdstrExpression,
    TOKEN            *pToken)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_UINT64 ui64DateTime;

    ChkArg( pToken != NULL );
    dr = DRM_UTL_GetZuluTimeFromStringAsUI64( pdstrExpression, &ui64DateTime );
    if( dr == DRM_E_INVALIDARG )
    {
        dr = CPRMEXP_INVALID_CONSTANT;
    }
    ChkDR( dr );
    pToken->TokenType       = TOKEN_DATETIME;
    pToken->val.u64DateTime = ui64DateTime;

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT GetToken(
    DRM_CONST_STRING    *pdstrExpression,
    TOKEN               *pToken,
    DRM_EXPR_TOKEN_TYPE *psLastToken)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BOOL fFound;

    /* Skip whitespace */
    while ( pdstrExpression->cchString && DRMCRT_iswspace(*(pdstrExpression->pwszString)))
    {
        pdstrExpression->pwszString++;
        pdstrExpression->cchString--;
    }

    if ( pdstrExpression->cchString == 0 )
    {
        /* We are at the end of buffer. No more args. */
        dr = DRM_E_NOMORE;
        goto ErrorExit;
    }

    fFound = TRUE;
    if( *(pdstrExpression->pwszString) == g_wchOpenParen )
    {
        (*pToken).TokenType = TOKEN_OPEN;
    }
    else if( *(pdstrExpression->pwszString) == g_wchCloseParen )
    {
        (*pToken).TokenType = TOKEN_CLOSE;
    }
    else if( *(pdstrExpression->pwszString) == g_wchComma )
    {
        (*pToken).TokenType = TOKEN_COMMA;
    }
    else if( *(pdstrExpression->pwszString) == g_wchAsterisk)
    {
        (*pToken).TokenType = TOKEN_MUL;
    }
    else if( *(pdstrExpression->pwszString) == g_wchForwardSlash )
    {
        (*pToken).TokenType = TOKEN_DIV;
    }
    else if( *(pdstrExpression->pwszString) == g_wchPercent )
    {
        (*pToken).TokenType = TOKEN_MOD;
    }
    else if( *(pdstrExpression->pwszString) == g_wchColon )
    {
        (*pToken).TokenType = TOKEN_COLON;
    }
    else if( *(pdstrExpression->pwszString) == g_wchQuestionMark )
    {
        (*pToken).TokenType = TOKEN_IF;
    }
    else if( *(pdstrExpression->pwszString) == g_wchSemiColon )
    {
        (*pToken).TokenType = TOKEN_SEMICOLON;
    }
    else if( *(pdstrExpression->pwszString) == g_wchExclamationMark )
    {
        if( pdstrExpression->cchString > 1 &&
            pdstrExpression->pwszString[1] == g_wchEqual )
        {
            pdstrExpression->pwszString++;
            pdstrExpression->cchString--;
            (*pToken).TokenType = TOKEN_NOTEQ;
        }
        else
        {
            (*pToken).TokenType = TOKEN_NOT;
        }
    }
    else if( *(pdstrExpression->pwszString) == g_wchEqual )
    {
        if( pdstrExpression->cchString > 1 &&
            pdstrExpression->pwszString[1] == g_wchEqual )
        {
            pdstrExpression->pwszString++;
            pdstrExpression->cchString--;
            (*pToken).TokenType = TOKEN_EQ;
        }
        else
        {
            (*pToken).TokenType = TOKEN_ASSIGN;
        }
    }
    else if( *(pdstrExpression->pwszString) == g_wchLessThan )
    {
        if( pdstrExpression->cchString > 1 &&
            pdstrExpression->pwszString[1] == g_wchEqual )
        {
            pdstrExpression->pwszString++;
            pdstrExpression->cchString--;
            (*pToken).TokenType = TOKEN_LESSEQ;
        }
        else
        {
            (*pToken).TokenType = TOKEN_LESS;
        }
    }
    else if( *(pdstrExpression->pwszString) == g_wchGreaterThan )
    {
        if( pdstrExpression->cchString > 1 &&
            pdstrExpression->pwszString[1] == g_wchEqual )
        {
            pdstrExpression->pwszString++;
            pdstrExpression->cchString--;
            (*pToken).TokenType = TOKEN_GREATEQ;
        }
        else
        {
            (*pToken).TokenType = TOKEN_GREAT;
        }
    }
    else if( *(pdstrExpression->pwszString) == g_wchAmpersand )
    {
        if( pdstrExpression->cchString > 1 &&
            pdstrExpression->pwszString[1] == g_wchAmpersand )
        {
            pdstrExpression->pwszString++;
            pdstrExpression->cchString--;
            (*pToken).TokenType = TOKEN_AND;
        }
        else
        {
            (*pToken).TokenType = TOKEN_UNKNOWN;
        }
    }
    else if( *(pdstrExpression->pwszString) == g_wchPipe )
    {
        if( pdstrExpression->cchString > 1 &&
            pdstrExpression->pwszString[1] == g_wchPipe )
        {
            pdstrExpression->pwszString++;
            pdstrExpression->cchString--;
            (*pToken).TokenType = TOKEN_OR;
        }
        else
        {
            (*pToken).TokenType = TOKEN_UNKNOWN;
        }
    }
    else if( *(pdstrExpression->pwszString) == g_wchPlus )
    {
        if( pdstrExpression->cchString > 1 &&
            pdstrExpression->pwszString[1] == g_wchPlus )
        {
            /* It is an increment operator. Determine if it is pre or post. */
            pdstrExpression->pwszString++;
            pdstrExpression->cchString--;
            if (*psLastToken == TOKEN_VARIABLE || *psLastToken == TOKEN_CLOSE)
            {
                (*pToken).TokenType = TOKEN_POSTINCR;
            }
            else
            {
                (*pToken).TokenType = TOKEN_PREINCR;
            }
        }
        else if (*psLastToken == TOKEN_LONG || *psLastToken == TOKEN_STRING || *psLastToken == TOKEN_DATETIME ||
            *psLastToken == TOKEN_VARIABLE || *psLastToken == TOKEN_CLOSE ||
            *psLastToken == TOKEN_POSTINCR || *psLastToken == TOKEN_POSTDECR)
        {
            (*pToken).TokenType = TOKEN_ADD;    /* Binary operator */
        }
        else
        {
            (*pToken).TokenType = TOKEN_PLUS; /*Unary operator */
        }
    }
    else if( *(pdstrExpression->pwszString) == g_wchMinus )
    {
        if( pdstrExpression->cchString > 1 &&
            pdstrExpression->pwszString[1] == g_wchMinus )
        {
            /* It is an increment operator. Determine if it is pre or post. */
            pdstrExpression->pwszString++;
            pdstrExpression->cchString--;
            if (*psLastToken == TOKEN_VARIABLE || *psLastToken == TOKEN_CLOSE)
            {
                (*pToken).TokenType = TOKEN_POSTDECR;
            }
            else
            {
                (*pToken).TokenType = TOKEN_PREDECR;
            }
        }
        else if (*psLastToken == TOKEN_LONG || *psLastToken == TOKEN_STRING || *psLastToken == TOKEN_DATETIME ||
            *psLastToken == TOKEN_VARIABLE || *psLastToken == TOKEN_CLOSE ||
            *psLastToken == TOKEN_POSTINCR || *psLastToken == TOKEN_POSTDECR)
        {
            (*pToken).TokenType = TOKEN_SUB;    /* Binary operator */
        }
        else
        {
            (*pToken).TokenType = TOKEN_MINUS; /*Unary operator */
        }
    }
    else
    {
        fFound = FALSE;
    }

    if( fFound )
    {
        pdstrExpression->pwszString++;
        pdstrExpression->cchString--;
        goto ErrorExit;
    }

    /* We have not identified this token yet -- it's obviously not an operator.  Let's see if we can figure
    it out */

    if (*(pdstrExpression->pwszString) == g_wchQuote)
    {
        /* String Constant */
        pdstrExpression->pwszString++;
        pdstrExpression->cchString--;
        dr = ExtractStringToken( pdstrExpression, pToken );
        (*pToken).TokenType = TOKEN_STRING;
    }
    else if (DRMCRT_iswdigit(*(pdstrExpression->pwszString)))
    {
        (*pToken).TokenType = TOKEN_LONG;
        dr = DRM_WMDRM_EXPR_ExtractLongToken( pdstrExpression, pToken );
    }
    else if (DRMCRT_iswalpha(*(pdstrExpression->pwszString)))
    {
        /* This could be either a function or a variable */
        dr = ExtractSymbolToken( pdstrExpression, pToken );
    }
    else if (*(pdstrExpression->pwszString) == g_wchPound)
    {
        dr = DRM_WMDRM_EXPR_ExtractDateToken( pdstrExpression, pToken);
        (*pToken).TokenType = TOKEN_DATETIME;
    }
    else
    {
        /* This must be an invalid character. */
        dr = CPRMEXP_INVALID_TOKEN;
        pdstrExpression->pwszString++;
        pdstrExpression->cchString--;
    }

ErrorExit:
    *psLastToken = (DRM_EXPR_TOKEN_TYPE)((*pToken).TokenType);
    return (dr);
}


static DRM_NO_INLINE DRM_LONG InputPrecedence(DRM_EXPR_TOKEN_TYPE tokenType)
{
    switch(tokenType)
    {
    case TOKEN_OPEN:
        return(1000);
    case TOKEN_CLOSE:
        return(0);
    case TOKEN_FUNCTION:
        return(300);
    case TOKEN_POSTINCR:
    case TOKEN_POSTDECR:
        return(220);
    case TOKEN_NOT:
    case TOKEN_PREINCR:
    case TOKEN_PREDECR:
    case TOKEN_PLUS:
    case TOKEN_MINUS:
        return(200);
    case TOKEN_MUL:
    case TOKEN_DIV:
    case TOKEN_MOD:
        return(180);
    case TOKEN_ADD:
    case TOKEN_SUB:
        return(160);
    case TOKEN_LESS:
    case TOKEN_GREAT:
    case TOKEN_LESSEQ:
    case TOKEN_GREATEQ:
        return(140);
    case TOKEN_EQ:
    case TOKEN_NOTEQ:
        return(120);
    case TOKEN_AND:
        return(100);
    case TOKEN_OR:
        return(80);
    case TOKEN_IF:
        return(60);
    case TOKEN_COLON:
        return(25);
    case TOKEN_ASSIGN:
        return(50);
    case TOKEN_SEMICOLON:
        return(20);
    case TOKEN_COMMA:
        return(10);
    default:
        break;
    }

    DRMASSERT(!"Should not happen: unexpected token");
    return (-1000); /* Should not happen. */
}

static DRM_NO_INLINE DRM_LONG StackPrecedence(DRM_EXPR_TOKEN_TYPE tokenType)
{

    switch(tokenType)
    {
    case TOKEN_OPEN:
        return(-1);
    case TOKEN_CLOSE:
        return(0);
    case TOKEN_FUNCTION:
        return(300);
    case TOKEN_POSTINCR:
    case TOKEN_POSTDECR:
        return(220);
    case TOKEN_NOT:
    case TOKEN_PREINCR:
    case TOKEN_PREDECR:
    case TOKEN_PLUS:
    case TOKEN_MINUS:
        return(190);
    case TOKEN_MUL:
    case TOKEN_DIV:
    case TOKEN_MOD:
        return(180);
    case TOKEN_ADD:
    case TOKEN_SUB:
        return(160);
    case TOKEN_LESS:
    case TOKEN_GREAT:
    case TOKEN_LESSEQ:
    case TOKEN_GREATEQ:
        return(140);
    case TOKEN_EQ:
    case TOKEN_NOTEQ:
        return(120);
    case TOKEN_AND:
    case TOKEN_ANDSKIP:
        return(100);
    case TOKEN_OR:
    case TOKEN_ORSKIP:
        return(80);
    case TOKEN_IF:
    case TOKEN_IFSKIP:
        return(22);
    case TOKEN_COLON:
    case TOKEN_COLONSKIP:
        return(25);
    case TOKEN_ASSIGN:
        return(30);
    case TOKEN_SEMICOLON:
        return(20);
    case TOKEN_COMMA:
        return(10);
    default:
        break;
    }
    DRMASSERT(!"Should not happen: unexpected token");
    return(-1000); /* Should not happen. */
}

static DRM_NO_INLINE DRM_RESULT TokenStackPush(
    TOKEN      rgTokenStack[],
    DRM_DWORD *pdwStackHead,
    TOKEN     *pNewToken )
{
    if( *pdwStackHead == DRM_EXPR_EVAL_MAX_TOKEN_STACK_SIZE )
    {
        /* The stack is full.  Fail */
        return DRM_E_OUTOFMEMORY;
    }
    MEMCPY( &(rgTokenStack[*pdwStackHead]), pNewToken, SIZEOF( TOKEN ) );
    *pdwStackHead += 1;
    return DRM_SUCCESS;
}

static DRM_NO_INLINE DRM_RESULT TokenStackPop(
    TOKEN      rgTokenStack[],
    DRM_DWORD *pdwStackHead,
    TOKEN     *pNewToken )
{
    if( *pdwStackHead == 0 )
    {
        return CPRMEXP_MISSING_OPERAND;
    }
    *pdwStackHead -= 1;
    MEMCPY( pNewToken, &(rgTokenStack[*pdwStackHead]), SIZEOF( TOKEN ) );
    return DRM_SUCCESS;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_EXPR_EvaluateExpression(
    IN const DRM_CONST_STRING            *pdstrExpression,
    IN OUT   DRM_WMDRM_EXPR_EVAL_CONTEXT *pEEContext,
       OUT   TOKEN                       *ptResult )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrExpression = EMPTY_DRM_STRING;
    TOKEN Token, newToken;
    DRM_EXPR_TOKEN_TYPE sLastToken = TOKEN_UNKNOWN;
    DRM_LONG lRank = 0;        /* Rank will tell us if we have operands to gether without operators or operators that are stranded */
    DRM_LONG lInputPrecedence = 0;

    ChkDRMString( pdstrExpression );
    ChkArg( pEEContext && ptResult );

    ZEROMEM( &(pEEContext->ExprEvalData), SIZEOF( pEEContext->ExprEvalData ) );
    ASSIGN_DRM_STRING( dstrExpression, *pdstrExpression );
    for( dr = GetToken( &dstrExpression, &Token, &sLastToken ); DRM_SUCCEEDED( dr ); dr = GetToken( &dstrExpression, &Token, &sLastToken ) )
    {
        if (Token.TokenType == TOKEN_UNKNOWN)
        {
            dr = CPRMEXP_INVALID_TOKEN;
            goto ErrorExit;
        }

        if (IsOperand((DRM_EXPR_TOKEN_TYPE)Token.TokenType))
        {
            lRank++;
        }
        else if (IsBinaryOp((DRM_EXPR_TOKEN_TYPE)Token.TokenType))
        {
            lRank--;
        }
        /* Now check the rank. It should always be 0 or 1 */
        if (lRank != 0 && lRank != 1)
        {
            dr = CPRMEXP_ISOLATED_OPERAND_OR_OPERATOR;
            goto ErrorExit;
        }

        /* If it is an operand, push onto the operand stack. */
        if (IsOperand((DRM_EXPR_TOKEN_TYPE)Token.TokenType))
        {
            ChkDR( TokenStackPush( pEEContext->ExprEvalData.rgOperandTokenStack, &(pEEContext->ExprEvalData.dwOperandStack), &Token ) );
            ZEROMEM( &Token, SIZEOF( Token ) );
            continue;
        }
        /* If it is ), then pop all operators of higher precedence in stack and perform operation. */
        if (Token.TokenType == TOKEN_CLOSE)
        {
            lInputPrecedence = InputPrecedence(TOKEN_CLOSE);
            while ( 0 < pEEContext->ExprEvalData.dwOperatorStack &&
                StackPrecedence((DRM_EXPR_TOKEN_TYPE)pEEContext->ExprEvalData.rgOperatorTokenStack[pEEContext->ExprEvalData.dwOperatorStack - 1].TokenType) >= lInputPrecedence)    /* Loop through operators in the stack and perform the operations if their precedence is right */
            {

                ChkDR( TokenStackPop( pEEContext->ExprEvalData.rgOperatorTokenStack, &( pEEContext->ExprEvalData.dwOperatorStack ), &newToken ) );
                ChkDR( PerformOperation(&newToken, pEEContext ) );
                /* &pEEContext->ExprEvalData.rgOperandTokenStack, &pEEContext->ExprEvalData.rgOperatorTokenStack, &fShortCircuit, pEEContext->pvOpaqueData); */
                if (newToken.TokenType == TOKEN_FUNCTION)
                {
                    /* It was a function call. Rand must be 0 or 1. It will be 0 if it was a function call with 0 args.  */
                    /* We need to set rank to 1. */
                    if (DRM_WMDRM_EXPRFUNC_Arity((DRM_EXPR_FUNCTION_TYPE)newToken.val.fnValue) == 0)
                    {
                        lRank++;
                    }
                }
            }
            if (0 == pEEContext->ExprEvalData.dwOperatorStack ||
                pEEContext->ExprEvalData.rgOperatorTokenStack[pEEContext->ExprEvalData.dwOperatorStack - 1].TokenType != TOKEN_OPEN)
            {
                dr = CPRMEXP_UNMATCHED;
                goto ErrorExit;
            }

            /* Pop and discard the token TOPEN. Here the stack must be non-empty and has TOPEN. */
            ChkDR( TokenStackPop( pEEContext->ExprEvalData.rgOperatorTokenStack, &( pEEContext->ExprEvalData.dwOperatorStack ), &newToken ) );

            /* Check if the ) we are processing corresponds to the one closing a function call. If so, execute the function. */
            if ( 0 != pEEContext->ExprEvalData.dwOperatorStack)
            {
                if (pEEContext->ExprEvalData.rgOperatorTokenStack[pEEContext->ExprEvalData.dwOperatorStack - 1].TokenType == TOKEN_FUNCTION)
                {
                    ChkDR( TokenStackPop( pEEContext->ExprEvalData.rgOperatorTokenStack, &( pEEContext->ExprEvalData.dwOperatorStack ), &newToken ) );
                    dr = PerformOperation(&newToken, pEEContext );
                     /*&pEEContext->ExprEvalData.rgOperandTokenStack, &pEEContext->ExprEvalData.rgOperatorTokenStack, &fShortCircuit, pEEContext->pvOpaqueData); */
                    if( DRM_FAILED( dr ) )
                    {
                        goto ErrorExit;
                    }
                    /* It was a function call. Rank must be 0 or 1. It will be 0 if it was a function call with 0 args.  */
                    /* We need to set rank to 1. */
                    if (DRM_WMDRM_EXPRFUNC_Arity((DRM_EXPR_FUNCTION_TYPE)newToken.val.fnValue) == 0)
                    {
                        lRank++;
                    }
                }
            }
            ZEROMEM( &Token, SIZEOF( Token ) );
            continue;
        }

        /* It must be an operator or TOKEN_OPEN. Pop all operators of higher precedence and perform op.                 */
        lInputPrecedence = InputPrecedence((DRM_EXPR_TOKEN_TYPE)Token.TokenType);
        while (0 != pEEContext->ExprEvalData.dwOperatorStack &&
            StackPrecedence((DRM_EXPR_TOKEN_TYPE)pEEContext->ExprEvalData.rgOperatorTokenStack[pEEContext->ExprEvalData.dwOperatorStack - 1].TokenType) >= lInputPrecedence)
        {
            ChkDR( TokenStackPop( pEEContext->ExprEvalData.rgOperatorTokenStack, &( pEEContext->ExprEvalData.dwOperatorStack ), &newToken ) );
            dr = PerformOperation(&newToken, pEEContext );
            if( DRM_FAILED( dr ) )
            {
                goto ErrorExit;
            }
            /* The stackOp cannot be a function symbol here. No need to check for it like in other places. */
        }

        /* Push the new operator onto the stack. Do not delete the token as it lives in stack. */
        /* Take care of && || ? and :.  */
        if ( pEEContext->ExprEvalData.fShortCircuit )
        {
            /* We are in skip mode in this section. */
            /* Simply push the corresponding token. */
            switch(Token.TokenType)
            {
            case TOKEN_AND:
                Token.TokenType = TOKEN_ANDSKIP;
                break;
            case TOKEN_OR:
                Token.TokenType = TOKEN_ORSKIP;
                break;
            case TOKEN_IF:
                Token.TokenType = TOKEN_IFSKIP;
                break;
            case TOKEN_COLON:
                if ( 0 == pEEContext->ExprEvalData.dwOperatorStack ||
                    ( pEEContext->ExprEvalData.rgOperatorTokenStack[pEEContext->ExprEvalData.dwOperatorStack - 1].TokenType != TOKEN_IF &&
                    pEEContext->ExprEvalData.rgOperatorTokenStack[pEEContext->ExprEvalData.dwOperatorStack - 1].TokenType != TOKEN_IFSKIP
                    )
                    )
                {
                    dr = CPRMEXP_UNMATCHED;
                    goto ErrorExit;
                }
                if (pEEContext->ExprEvalData.rgOperatorTokenStack[pEEContext->ExprEvalData.dwOperatorStack - 1].TokenType == TOKEN_IF)
                {
                    pEEContext->ExprEvalData.fShortCircuit = FALSE; /* Then part skipping is over. We are at the begining of else part. Turn eval on. */
                    Token.TokenType = TOKEN_COLON;
                }
                else
                {
                    Token.TokenType = TOKEN_COLONSKIP;    /* The matching ? was found in skip mode. */
                }
            }
        }
        else
        {
            /* We are in evaluation mode in this section. */
            if (Token.TokenType == TOKEN_AND ||
                Token.TokenType == TOKEN_OR  ||
                Token.TokenType == TOKEN_IF)
            {
                /* Pop an operand and evaluate before proceeding. This is for shortcut or conditional evaluation. */
                dr = TokenStackPop( pEEContext->ExprEvalData.rgOperandTokenStack, &(pEEContext->ExprEvalData.dwOperandStack), &newToken );
                if ( DRM_FAILED( dr ) )
                {
                    dr = CPRMEXP_MISSING_OPERAND;
                    goto ErrorExit;
                }
                if ( newToken.TokenType == TOKEN_VARIABLE)
                {
                    dr = EvalVariable(&newToken, pEEContext);    /* This can change the type of NODE after evaluation */
                    if( DRM_FAILED( dr ) )
                    {
                        goto ErrorExit;
                    }
                }
                if ( newToken.TokenType != TOKEN_LONG )
                {
                    dr = CPRMEXP_WRONG_TYPE_OPERAND;
                    goto ErrorExit;
                }
                if ( ( Token.TokenType == TOKEN_AND && newToken.val.lValue == 0 ) ||
                     ( Token.TokenType == TOKEN_OR  && newToken.val.lValue == 1  ) ||
                     ( Token.TokenType == TOKEN_IF  && newToken.val.lValue == 0  ) )
                {
                    pEEContext->ExprEvalData.fShortCircuit = TRUE;
                }
                ChkDR( TokenStackPush( pEEContext->ExprEvalData.rgOperandTokenStack, &( pEEContext->ExprEvalData.dwOperandStack ), &newToken) );
            }
            else if (Token.TokenType == TOKEN_COLON)
            {
                /* It is ':'. The stack must have matching ?.  */
                if (0 == pEEContext->ExprEvalData.dwOperatorStack ||
                    pEEContext->ExprEvalData.rgOperatorTokenStack[pEEContext->ExprEvalData.dwOperatorStack - 1].TokenType != TOKEN_IF)
                {
                    dr = CPRMEXP_UNMATCHED;
                    goto ErrorExit;
                }
                pEEContext->ExprEvalData.fShortCircuit = TRUE; /* fShortCircuit is FALSE currently. Which means we got then part. We need to skip else part.                 */
            }
        }

        ChkDR( TokenStackPush(pEEContext->ExprEvalData.rgOperatorTokenStack, &( pEEContext->ExprEvalData.dwOperatorStack ), &Token) );

        if (Token.TokenType == TOKEN_FUNCTION)
        {
            /* Push TOKEN_FUNCTIONSYNC token into the operand stack.             */
            ZEROMEM( &Token, SIZEOF( TOKEN ) );
            Token.TokenType = TOKEN_FUNCTIONSYNC;
            ChkDR( TokenStackPush( pEEContext->ExprEvalData.rgOperandTokenStack, &( pEEContext->ExprEvalData.dwOperandStack ), &Token) );
        }
        else if (IsPreUnaryOp((DRM_EXPR_TOKEN_TYPE)Token.TokenType))
        {
            /* Push TUNARYSYNC token into the operand stack. */
            ZEROMEM( &Token, SIZEOF( TOKEN ) );
            Token.TokenType = TOKEN_UNARYSYNC;
            ChkDR( TokenStackPush( pEEContext->ExprEvalData.rgOperandTokenStack, &( pEEContext->ExprEvalData.dwOperandStack ), &Token) );
        }
        ZEROMEM( &Token, SIZEOF( Token ) );
    } /* for loop */


    if (dr == DRM_E_NOMORE)
    {
        /* We have a valid expression. Clean up stack and return the result. */
        ChkDR(DRM_SUCCESS); /* This sets 'dr' and keeps the PREfast tool happy. */
        while (0 != pEEContext->ExprEvalData.dwOperatorStack )
        {
            ChkDR( TokenStackPop( pEEContext->ExprEvalData.rgOperatorTokenStack, &(pEEContext->ExprEvalData.dwOperatorStack), &Token) );
            ChkDR( PerformOperation(&Token, pEEContext ) );
            /* ->ExprEvalData.rgOperandTokenStack, &pEEContext->ExprEvalData.rgOperatorTokenStack, &fShortCircuit, pEEContext->pvOpaqueData)); */
            if ( Token.TokenType == TOKEN_FUNCTION )
            {
                /* It was a function call. Rand must be 0 or 1. It will be 0 if it was a function call with 0 args.  */
                /* We need to set rank to 1. */
                if (DRM_WMDRM_EXPRFUNC_Arity((DRM_EXPR_FUNCTION_TYPE)Token.val.fnValue) == 0)
                {
                    lRank++;
                }
            }
        }
        if ( 0 == pEEContext->ExprEvalData.dwOperandStack )
        {
            dr = CPRMEXP_NO_OPERANDS_IN_EXPRESSION;
            goto ErrorExit;
        }

        ChkDR( TokenStackPop( pEEContext->ExprEvalData.rgOperandTokenStack, &( pEEContext->ExprEvalData.dwOperandStack ), &Token) );

        if ( 0 != pEEContext->ExprEvalData.dwOperandStack )
        {
            dr = CPRMEXP_TOO_MANY_OPERANDS;
            goto ErrorExit;
        }

        /* At the end, lRank should be 1 for a valid expression. */
        if (lRank != 1)
        {
            dr = CPRMEXP_ISOLATED_OPERAND_OR_OPERATOR;
            goto ErrorExit;
        }

        /* At the end if the token is a variable, evaluate it and return that value */
        if (Token.TokenType == TOKEN_VARIABLE)
        {
            ChkDR( EvalVariable( &Token, pEEContext ) );
        }

        /* Examine the value of the node at this point.         */
        MEMCPY( ptResult, &Token, SIZEOF( TOKEN ) );
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function: DRM_WMDRM_EXPR_LEVL_EvaluateExpression
**
** Synopsis: Evaluate an expression
**
** Arguments:
**      [f_pcontextLicEval] : Specifies the liceval context to use
**      [f_pdstrExpression] : Specifies the expression to evaluate
**      [f_pfValue]         : Returns the output of the expression
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL
**      CPRMEXP_RETRIEVAL_FAILURE
**          Failred to retrieve date/time data
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_EXPR_LEVL_EvaluateExpression(
    IN OUT   DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    IN const DRM_CONST_STRING    *f_pdstrExpression,
    OUT      DRM_BOOL            *f_pfValue )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    TOKEN      tResult = {0};

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMLICEVAL, PERF_FUNC_DRM_WMDRM_EXPR_LEVL_EvaluateExpression );

    ChkArg( f_pContextLicEval != NULL );
    ChkArg( f_pfValue         != NULL );
    ChkDRMString( f_pdstrExpression );

    f_pContextLicEval->contextEXPR.GetVariable          = DRM_WMDRM_EXPR_GlobalGetVariable;
    f_pContextLicEval->contextEXPR.SetVariable          = DRM_WMDRM_EXPR_GlobalSetVariable;
    f_pContextLicEval->contextEXPR.pvOpaqueData         = f_pContextLicEval;

    ChkDR( DRM_WMDRM_EXPR_EvaluateExpression( f_pdstrExpression,
                                       &( f_pContextLicEval->contextEXPR ),
                                       &tResult ) );
    if ( tResult.TokenType == TOKEN_LONG )
    {
        *f_pfValue = ( tResult.val.lValue != 0 );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/* PerformOperation & EvalVariable ( this is a big one with the different types of variables we might have. ) */

/* Peform the operation passed in the token. Do not delete the token. */
/* Stack of operands gets affected. Stack of operators may also get affected. */
/* This function can change operand stack, operator stack, state of fShortCircuit, will also need access to the expression string. */
DRM_NO_INLINE DRM_RESULT PerformOperation(TOKEN *pOperator, DRM_WMDRM_EXPR_EVAL_CONTEXT* pEEContext)
{
    DRM_RESULT dr = DRM_SUCCESS;/*vinceyu:replaced CPRMEXP_NOERROR */
    TOKEN Operand1;
    TOKEN Operand2;
    TOKEN OperandResult;
    DRM_INT64 i64Value;

    ZEROMEM( &OperandResult, SIZEOF( TOKEN ) );
    OperandResult.TokenType = TOKEN_LONG;
    switch(pOperator->TokenType)
    {
    case TOKEN_PLUS:
    case TOKEN_MINUS:
    case TOKEN_NOT:
        /* Unary operator. Pop one operand, perform operation, and push result back. */
        /* We should have TUNARYSYNC followed by operand. */
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand1 ) );
        if (Operand1.TokenType == TOKEN_UNARYSYNC || Operand1.TokenType == TOKEN_FUNCTIONSYNC)
        {
            dr = CPRMEXP_MISSING_OPERAND;
            goto ErrorExit;
        }

        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand2 ) );
        if (Operand2.TokenType != TOKEN_UNARYSYNC)
        {
            dr = CPRMEXP_UNKNOWN_PARSE_ERROR; /* This should not really happen. */
            goto ErrorExit;
        }

        if (!pEEContext->ExprEvalData.fShortCircuit)
        {
            /* Do the actual evaluation */
            if (Operand1.TokenType == TOKEN_VARIABLE)
            {
                ChkDR( EvalVariable( &Operand1, pEEContext) );
            }
            if ( Operand1.TokenType != TOKEN_LONG)
            {
                dr = CPRMEXP_WRONG_TYPE_OPERAND;
                goto ErrorExit;
            }

            OperandResult.val.lValue = Operand1.val.lValue;
            if (pOperator->TokenType == TOKEN_MINUS)
            {
                i64Value = DRM_I64Sub(DRM_I64(0), DRM_I64(OperandResult.val.lValue) );

                if ( DRM_I64Les( DRM_I64(LONG_MAX), i64Value ) )
                {
                    dr = CPRMEXP_OVERFLOW;
                    goto ErrorExit;
                }
                else if ( DRM_I64Les( i64Value, DRM_I64(LONG_MIN)) )
                {
                    dr = CPRMEXP_UNDERFLOW;
                    goto ErrorExit;
                }
                OperandResult.val.lValue = DRM_I64ToUI32(i64Value);
            }
            else if (pOperator->TokenType == TOKEN_NOT)
            {
                OperandResult.val.lValue = !(OperandResult.val.lValue != 0);
            }
        }
        ChkDR( TokenStackPush(pEEContext->ExprEvalData.rgOperandTokenStack, &( pEEContext->ExprEvalData.dwOperandStack ), &OperandResult) );
        break;
    case TOKEN_ADD:
    case TOKEN_SUB:
    case TOKEN_MUL:
    case TOKEN_DIV:
    case TOKEN_MOD:
        /* Binary arithmetic operators. TADD can also be used for strings. */
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand2 ) );
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand1 ) );

        if (!pEEContext->ExprEvalData.fShortCircuit)
        {
            /* Perform the operation on the two operands and produce result. */
            if (Operand1.TokenType == TOKEN_VARIABLE)
            {
                ChkDR(EvalVariable(&Operand1, pEEContext));
            }
            if (Operand2.TokenType == TOKEN_VARIABLE)
            {
                ChkDR(EvalVariable(&Operand2, pEEContext));
            }
            if ( pOperator->TokenType != TOKEN_ADD &&
                ( Operand2.TokenType != TOKEN_LONG || Operand1.TokenType != TOKEN_LONG) )
            {
                dr = CPRMEXP_WRONG_TYPE_OPERAND;
                goto ErrorExit;
            }
            else if (pOperator->TokenType == TOKEN_ADD &&
                !(
                (Operand2.TokenType == TOKEN_LONG && Operand1.TokenType == TOKEN_LONG) ||
                (Operand2.TokenType == TOKEN_STRING && Operand1.TokenType == TOKEN_STRING)
                )
                )

            {
                dr = CPRMEXP_WRONG_TYPE_OPERAND;
                goto ErrorExit;
            }
            switch(pOperator->TokenType)
            {
            case TOKEN_ADD:
                if (Operand2.TokenType == TOKEN_LONG)
                {
                    i64Value = DRM_I64Add( DRM_I64(Operand1.val.lValue), DRM_I64(Operand2.val.lValue) );

                    if ( DRM_I64Les( DRM_I64(LONG_MAX), i64Value ) )
                    {
                        dr = CPRMEXP_OVERFLOW;
                        goto ErrorExit;
                    }
                    else if ( DRM_I64Les( i64Value, DRM_I64(LONG_MIN) ) )
                    {
                        dr = CPRMEXP_UNDERFLOW;
                        goto ErrorExit;
                    }
                    OperandResult.val.lValue = DRM_I64ToUI32(i64Value);
                }
                else
                {
                    dr = CPRMEXP_WRONG_TYPE_OPERAND;
                    goto ErrorExit;
                }

                break;
            case TOKEN_SUB:
                i64Value = DRM_I64Sub( DRM_I64(Operand1.val.lValue), DRM_I64(Operand2.val.lValue) );
                if ( DRM_I64Les( DRM_I64(LONG_MAX), i64Value ) )
                {
                    dr = CPRMEXP_OVERFLOW;
                    goto ErrorExit;
                }
                else if ( DRM_I64Les( i64Value, DRM_I64(LONG_MIN) ) )
                {
                    dr = CPRMEXP_UNDERFLOW;
                    goto ErrorExit;
                }
                OperandResult.val.lValue = DRM_I64ToUI32(i64Value);
                break;
            case TOKEN_MUL:
                i64Value = DRM_I64Mul( DRM_I64(Operand1.val.lValue), DRM_I64(Operand2.val.lValue) );
                if ( DRM_I64Les( DRM_I64(LONG_MAX), i64Value ) )
                {
                    dr = CPRMEXP_OVERFLOW;
                    goto ErrorExit;
                }
                else if ( DRM_I64Les( i64Value, DRM_I64(LONG_MIN) ) )
                {
                    dr = CPRMEXP_UNDERFLOW;
                    goto ErrorExit;
                }
                OperandResult.val.lValue = DRM_I64ToUI32(i64Value);
                break;
            case TOKEN_DIV:
                if (Operand2.val.lValue == 0)
                {
                    dr = CPRMEXP_OVERFLOW;
                    goto ErrorExit;
                }
                OperandResult.val.lValue = Operand1.val.lValue / Operand2.val.lValue;
                break;
            case TOKEN_MOD:
                if (Operand2.val.lValue == 0)
                {
                    dr = CPRMEXP_OVERFLOW;
                    goto ErrorExit;
                }
                OperandResult.val.lValue = Operand1.val.lValue % Operand2.val.lValue;
            }
        }
        ChkDR( TokenStackPush(pEEContext->ExprEvalData.rgOperandTokenStack, &( pEEContext->ExprEvalData.dwOperandStack ), &OperandResult) );
        break;
    case TOKEN_PREINCR:
    case TOKEN_POSTINCR:
    case TOKEN_PREDECR:
    case TOKEN_POSTDECR:
        /* Unary increment or decrement operator. */
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand1 ) );

        if (pOperator->TokenType == TOKEN_PREINCR || pOperator->TokenType == TOKEN_PREDECR)
        {
            if (pOperator->TokenType == TOKEN_UNARYSYNC || pOperator->TokenType == TOKEN_FUNCTIONSYNC)
            {
                dr = CPRMEXP_MISSING_OPERAND;
                goto ErrorExit;
            }
            ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand2 ) );
            if (Operand2.TokenType != TOKEN_UNARYSYNC)
            {
                dr = CPRMEXP_UNKNOWN_PARSE_ERROR;
                goto ErrorExit;
            }
        }

        if (!pEEContext->ExprEvalData.fShortCircuit)
        {
            /* The operand must be a varialble that can be set. */
            if (Operand1.TokenType != TOKEN_VARIABLE)
            {
                dr = CPRMEXP_VARIABLE_EXPECTED;
                goto ErrorExit;
            }

            ZEROMEM( &Operand2, SIZEOF( TOKEN ) );

            ChkDR( pEEContext->GetVariable( &(Operand1.val.stringValue), &Operand2, pEEContext->pvOpaqueData ) );
            if (Operand2.TokenType != TOKEN_LONG)
            {
                dr = CPRMEXP_WRONG_TYPE_OPERAND;
                goto ErrorExit;
            }
            OperandResult.val.lValue = Operand2.val.lValue; /* For preincr or predecr. */
            /* Perform the incr or decr operation on the variable. */
            if (pOperator->TokenType == TOKEN_PREINCR || pOperator->TokenType == TOKEN_POSTINCR)
            {
                i64Value = DRM_I64Add( DRM_I64(Operand2.val.lValue), DRM_I64(1) );
            }
            else
            {
                i64Value = DRM_I64Sub( DRM_I64(Operand2.val.lValue), DRM_I64(1) );
            }
            if ( DRM_I64Les( DRM_I64(LONG_MAX), i64Value ) )
            {
                dr = CPRMEXP_OVERFLOW;
                goto ErrorExit;
            }
            else if ( DRM_I64Les( i64Value, DRM_I64(LONG_MIN) ) )
            {
                dr = CPRMEXP_UNDERFLOW;
                goto ErrorExit;
            }
            Operand2.val.lValue = DRM_I64ToUI32(i64Value);
            if (pOperator->TokenType == TOKEN_PREINCR || pOperator->TokenType == TOKEN_PREDECR)
            {
                OperandResult.val.lValue = Operand2.val.lValue;
            }
            ChkDR( pEEContext->SetVariable( &(Operand1.val.stringValue), &Operand2, NULL, pEEContext->pvOpaqueData) );
        }
        ChkDR( TokenStackPush(pEEContext->ExprEvalData.rgOperandTokenStack, &( pEEContext->ExprEvalData.dwOperandStack ), &OperandResult) );
        break;
    case TOKEN_ASSIGN:
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand2 ) );  /* The right side operand of = */
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand1 ) );  /* The left side operand (i.e variable) of = */

        /* The operand must be a varialble that can be set. */
        if (Operand1.TokenType != TOKEN_VARIABLE)
        {
            dr = CPRMEXP_VARIABLE_EXPECTED;
            goto ErrorExit;
        }
        if (!pEEContext->ExprEvalData.fShortCircuit)
        {
            if (Operand2.TokenType == TOKEN_VARIABLE)
            {
                ChkDR(EvalVariable(&Operand2, pEEContext));
            }

            ZEROMEM( &OperandResult, SIZEOF( TOKEN ) );
            ChkDR( pEEContext->SetVariable( &(Operand1.val.stringValue), &Operand2, &OperandResult, pEEContext->pvOpaqueData ) );
        }
        ChkDR( TokenStackPush(pEEContext->ExprEvalData.rgOperandTokenStack, &( pEEContext->ExprEvalData.dwOperandStack ), &OperandResult) );
        break;
    case TOKEN_LESS:
    case TOKEN_GREAT:
    case TOKEN_LESSEQ:
    case TOKEN_GREATEQ:
    case TOKEN_NOTEQ:
    case TOKEN_EQ:
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand2 ) );  /* The right side operand of comparison */
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand1 ) );  /* The left side operand of comparison */
        if (!pEEContext->ExprEvalData.fShortCircuit)
        {
            /* Perform the operation on the two operands and produce result. */
            if (Operand1.TokenType == TOKEN_VARIABLE)
            {
                ChkDR(EvalVariable(&Operand1, pEEContext));
            }
            if (Operand2.TokenType == TOKEN_VARIABLE)
            {
                ChkDR(EvalVariable(&Operand2, pEEContext));
            }
            if (Operand1.TokenType != Operand2.TokenType)
            {
                dr = CPRMEXP_WRONG_TYPE_OPERAND;
                goto ErrorExit;
            }
            switch(pOperator->TokenType)
            {
            case TOKEN_LESS:
                if (Operand1.TokenType == TOKEN_LONG)
                {
                    OperandResult.val.lValue = Operand1.val.lValue < Operand2.val.lValue;
                }
                else if (Operand2.TokenType == TOKEN_STRING)
                {
                    OperandResult.val.lValue =  DRMCRT_wcsncmp( Operand1.val.stringValue.pwszString,
                                                         Operand2.val.stringValue.pwszString,
                                                         min( Operand1.val.stringValue.cchString, Operand2.val.stringValue.cchString) ) < 0;
                }
                else
                {
                    OperandResult.val.lValue = DRM_UI64Les( Operand1.val.u64DateTime, Operand2.val.u64DateTime );
                }
                break;
            case TOKEN_GREAT:
                if (Operand1.TokenType == TOKEN_LONG)
                {
                    OperandResult.val.lValue = Operand1.val.lValue > Operand2.val.lValue;
                }
                else if (Operand2.TokenType == TOKEN_STRING)
                {
                    OperandResult.val.lValue =  DRMCRT_wcsncmp( Operand1.val.stringValue.pwszString,
                                                         Operand2.val.stringValue.pwszString,
                                                         min( Operand1.val.stringValue.cchString, Operand2.val.stringValue.cchString) ) > 0;
                }
                else
                {
                    OperandResult.val.lValue = DRM_UI64Les( Operand2.val.u64DateTime, Operand1.val.u64DateTime );
                }
                break;
            case TOKEN_LESSEQ:
                if (Operand1.TokenType == TOKEN_LONG)
                {
                    OperandResult.val.lValue = Operand1.val.lValue <= Operand2.val.lValue;
                }
                else if (Operand2.TokenType == TOKEN_STRING)
                {
                    OperandResult.val.lValue =  DRMCRT_wcsncmp( Operand1.val.stringValue.pwszString,
                                                         Operand2.val.stringValue.pwszString,
                                                         min( Operand1.val.stringValue.cchString, Operand2.val.stringValue.cchString) ) <= 0;
                }
                else
                {
                    OperandResult.val.lValue =   DRM_UI64Les( Operand1.val.u64DateTime, Operand2.val.u64DateTime )
                                              || DRM_UI64Eql( Operand1.val.u64DateTime, Operand2.val.u64DateTime );
                }
                break;
            case TOKEN_GREATEQ:
                if (Operand1.TokenType == TOKEN_LONG)
                {
                    OperandResult.val.lValue = Operand1.val.lValue >= Operand2.val.lValue;
                }
                else if (Operand2.TokenType == TOKEN_STRING)
                {
                    OperandResult.val.lValue =  DRMCRT_wcsncmp( Operand1.val.stringValue.pwszString,
                                                         Operand2.val.stringValue.pwszString,
                                                         min( Operand1.val.stringValue.cchString, Operand2.val.stringValue.cchString) ) >= 0;
                }
                else
                {
                    OperandResult.val.lValue =   DRM_UI64Les( Operand2.val.u64DateTime, Operand1.val.u64DateTime )
                                              || DRM_UI64Eql( Operand1.val.u64DateTime, Operand2.val.u64DateTime );
                }
                break;
            case TOKEN_EQ:
                if (Operand1.TokenType == TOKEN_LONG)
                {
                    OperandResult.val.lValue = Operand1.val.lValue == Operand2.val.lValue;
                }
                else if (Operand2.TokenType == TOKEN_STRING)
                {
                    OperandResult.val.lValue =  DRMCRT_wcsncmp( Operand1.val.stringValue.pwszString,
                                                         Operand2.val.stringValue.pwszString,
                                                         min( Operand1.val.stringValue.cchString, Operand2.val.stringValue.cchString) ) == 0;
                }
                else
                {
                    OperandResult.val.lValue =  DRM_UI64Eql( Operand1.val.u64DateTime, Operand2.val.u64DateTime );
                }
                break;
            case TOKEN_NOTEQ:
                if (Operand1.TokenType == TOKEN_LONG)
                {
                    OperandResult.val.lValue = Operand1.val.lValue != Operand2.val.lValue;
                }
                else if (Operand2.TokenType == TOKEN_STRING)
                {
                    OperandResult.val.lValue =  DRMCRT_wcsncmp( Operand1.val.stringValue.pwszString,
                                                         Operand2.val.stringValue.pwszString,
                                                         min( Operand1.val.stringValue.cchString, Operand2.val.stringValue.cchString) ) != 0;
                }
                else
                {
                    OperandResult.val.lValue =  !DRM_UI64Eql( Operand1.val.u64DateTime, Operand2.val.u64DateTime );
                }
                break;
            }
        }
        ChkDR( TokenStackPush(pEEContext->ExprEvalData.rgOperandTokenStack, &( pEEContext->ExprEvalData.dwOperandStack ), &OperandResult) );
        break;
    case TOKEN_COMMA:
        /* It can occur only inside a function argument list.  */
        /* We define comma to be a no-op. Thus 2,3 will end up 2 3 on operand stack. */
        /* A comma has the lowest precedence just above ; and hence the expression */
        /* before comma is evaluated before proceeding. */
        /* Just make sure there are at least 2 operands for so we know they exist. */
        if( pEEContext->ExprEvalData.dwOperandStack < 2 )
        {
            ChkDR( CPRMEXP_MISSING_OPERAND );
        }
        break;
    case TOKEN_FUNCTION:
        {
            /* Pop the number from arity stack to determine actual number of arguments. */
            DRM_DWORD dwIndex;
            DRM_LONG  lNumArgs = 0, lExpectedArgs = DRM_WMDRM_EXPRFUNC_Arity((DRM_EXPR_FUNCTION_TYPE)pOperator->val.fnValue);

            /* Search the stack array for the TOKEN_FUNCTIONSYNC token.               */
            for( dwIndex = pEEContext->ExprEvalData.dwOperandStack; dwIndex > 0; dwIndex-- )
            {
                if ( pEEContext->ExprEvalData.rgOperandTokenStack[dwIndex-1].TokenType == TOKEN_UNARYSYNC)
                {
                    dr = CPRMEXP_INCORRECT_NUM_ARGS;
                    goto ErrorExit;
                }

                if( pEEContext->ExprEvalData.rgOperandTokenStack[dwIndex-1].TokenType == TOKEN_FUNCTIONSYNC )
                {
                    /* We got all the arguments. Exit */
                    break;
                }

                lNumArgs++;
            }
            if (lNumArgs != lExpectedArgs)
            {
                /* Mismatch in # of expected and actual arguments. */
                dr = CPRMEXP_INCORRECT_NUM_ARGS;
                goto ErrorExit;
            }

            if (!pEEContext->ExprEvalData.fShortCircuit)
            {
                dr = DRM_WMDRM_EXPRFUNC_FunctionGetValue(pOperator, lNumArgs, &(pEEContext->ExprEvalData.rgOperandTokenStack[dwIndex]), &OperandResult, pEEContext );
                if ( DRM_FAILED( dr ) )
                {
                    goto ErrorExit; /* dr should indicate the reason for the failure. */
                }
            }
            /* Now pop all of the operands off the stack. */
            /* We could short circuit the stack logic and just overwrite the OperandStack counter variable */
            /* pEEContext->ExprEvalData.dwOperandStack -= (lNumArgs + 1); */
            for( ;lNumArgs > 0; lNumArgs-- )
            {
                ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand2 ) );
            }
            /* All function arguments have been poped.  Now pop the TOKEN_FUNCTIONSYNC */
            ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand2 ) );

            ChkDR( TokenStackPush(pEEContext->ExprEvalData.rgOperandTokenStack, &( pEEContext->ExprEvalData.dwOperandStack ), &OperandResult) );
        break;
        }
    case TOKEN_AND:
    case TOKEN_OR:
        /* we turn the mode back on. */
        pEEContext->ExprEvalData.fShortCircuit = FALSE;
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand2 ) );  /* The right side operand of comparison */
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand1 ) );  /* The left side operand of comparison */
        if (Operand1.TokenType != TOKEN_LONG)
        {
            dr = CPRMEXP_WRONG_TYPE_OPERAND;
            goto ErrorExit;
        }
        if (pOperator->TokenType == TOKEN_AND && Operand1.val.lValue == 0)
        {
            OperandResult.val.lValue = 0;
        }
        else if (pOperator->TokenType == TOKEN_OR && Operand1.val.lValue != 0)
        {
            OperandResult.val.lValue = 1;
        }
        else
        {
            if (Operand2.TokenType == TOKEN_VARIABLE)
            {
                ChkDR(EvalVariable(&Operand2, pEEContext));
            }
            if (Operand2.TokenType != TOKEN_LONG)
            {
                dr = CPRMEXP_WRONG_TYPE_OPERAND;
                goto ErrorExit;
            }
            OperandResult.val.lValue = Operand2.val.lValue != 0;
        }
        ChkDR( TokenStackPush(pEEContext->ExprEvalData.rgOperandTokenStack, &( pEEContext->ExprEvalData.dwOperandStack ), &OperandResult) );
        break;
    case TOKEN_ANDSKIP:
    case TOKEN_ORSKIP:
        /* We pop two operands. We should be in skip mode now. We don't turn it on. */
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand2 ) );  /* The right side operand of comparison */
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand1 ) );  /* The left side operand of comparison */

        /* Popping both operands verify that there are two operands on stack. */
        /* We simply ignore the operands. We don't check the type for skipped section. */
        /* We can't check the type anyway without evaluating it and we don't want to evaluate. */
        /* For example, 1 || securestate.playcount. We don't know the type of variable until */
        /* evaluated. */
        ChkDR( TokenStackPush(pEEContext->ExprEvalData.rgOperandTokenStack, &( pEEContext->ExprEvalData.dwOperandStack ), &Operand1) );
        break;
    case TOKEN_COLONSKIP:
        /* We need matchig TIF */
        if (pEEContext->ExprEvalData.dwOperatorStack == 0
         || pEEContext->ExprEvalData.rgOperatorTokenStack [pEEContext->ExprEvalData.dwOperatorStack - 1].TokenType != TOKEN_IFSKIP)
        {
            dr = CPRMEXP_UNMATCHED;
            goto ErrorExit;
        }
        /* Discard the operator */
        ChkDR(TokenStackPop (pEEContext->ExprEvalData.rgOperatorTokenStack,&(pEEContext->ExprEvalData.dwOperatorStack), &Operand1 ) );

        ChkDR(TokenStackPop (pEEContext->ExprEvalData.rgOperandTokenStack, &(pEEContext->ExprEvalData.dwOperandStack), &OperandResult ) );
        ChkDR(TokenStackPop (pEEContext->ExprEvalData.rgOperandTokenStack, &(pEEContext->ExprEvalData.dwOperandStack), &Operand2 ) );
        ChkDR(TokenStackPop (pEEContext->ExprEvalData.rgOperandTokenStack, &(pEEContext->ExprEvalData.dwOperandStack), &Operand1 ) );
        ChkDR(TokenStackPush(pEEContext->ExprEvalData.rgOperandTokenStack, &(pEEContext->ExprEvalData.dwOperandStack), &Operand1) );
        break;
    case TOKEN_IFSKIP:
    case TOKEN_IF:
        dr = CPRMEXP_UNMATCHED;
        break;
    case TOKEN_COLON:
        /* We need matchig TOKEN_IF */
        if (0 == pEEContext->ExprEvalData.dwOperatorStack ||
            pEEContext->ExprEvalData.rgOperatorTokenStack[pEEContext->ExprEvalData.dwOperatorStack - 1].TokenType != TOKEN_IF)
        {
            dr = CPRMEXP_UNMATCHED;
            goto ErrorExit;
        }
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperatorTokenStack,&(pEEContext->ExprEvalData.dwOperatorStack), &Operand1 ) );
        /* We pop three operands and do the right thing. */
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &OperandResult ) );
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand2 ) );
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand1 ) );

        /* Operand1 should be an integer. Other two can be any type. */
        if (Operand1.TokenType == TOKEN_VARIABLE)
        {
            ChkDR(EvalVariable(&Operand1, pEEContext));
        }
        if (Operand1.TokenType != TOKEN_LONG)
        {
            dr = CPRMEXP_WRONG_TYPE_OPERAND;
            goto ErrorExit;
        }

        if (Operand1.val.lValue == 0)
        {
            /* Use the else part, which is oprnd3. */
            ChkDR( TokenStackPush(pEEContext->ExprEvalData.rgOperandTokenStack, &( pEEContext->ExprEvalData.dwOperandStack ), &OperandResult) );
        }
        else
        {
            ChkDR( TokenStackPush(pEEContext->ExprEvalData.rgOperandTokenStack, &( pEEContext->ExprEvalData.dwOperandStack ), &Operand2) );
        }
        pEEContext->ExprEvalData.fShortCircuit = FALSE; /* We are getting out of conditional expression. Turn back evaluation on. */
        break;
    case TOKEN_SEMICOLON:
        /* Pop the two operands and push the second operand back. */
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand2 ) );
        ChkDR( TokenStackPop(pEEContext->ExprEvalData.rgOperandTokenStack,&(pEEContext->ExprEvalData.dwOperandStack), &Operand1 ) );
        ChkDR( TokenStackPush(pEEContext->ExprEvalData.rgOperandTokenStack, &( pEEContext->ExprEvalData.dwOperandStack ), &Operand2) );
        break;
    case TOKEN_OPEN:
        dr = CPRMEXP_UNMATCHED;
        break;
    default:
        ;
    }

ErrorExit:
    return(dr);
}

EXIT_PK_NAMESPACE_CODE;

