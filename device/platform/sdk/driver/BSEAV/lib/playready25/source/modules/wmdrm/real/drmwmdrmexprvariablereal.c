/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMWMDRMEXPRVARIABLEREAL_C
#include <drmwmdrm.h>
#include <drmutilitieslite.h>
#include <drmsecureclock.h>
#include <drmwmdrmconstants.h>
#include <drmconstants.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_NONCONST_PARAM, "The array of function pointers means these must all have identical signatures")

static DRM_RESULT DRM_CALL _VariableDRMGetorSet       (const DRM_CONST_STRING* pdstrToken, TOKEN* pNewValue, TOKEN* pResult, DRM_VOID* pvOpaqueData, DRM_BOOL fSet);
static DRM_RESULT DRM_CALL _VariableDRMKGetorSet      (const DRM_CONST_STRING* pdstrToken, TOKEN* pNewValue, TOKEN* pResult, DRM_VOID* pvOpaqueData, DRM_BOOL fSet);
static DRM_RESULT DRM_CALL _VariableMachineGetorSet   (const DRM_CONST_STRING* pdstrToken, TOKEN* pNewValue, TOKEN* pResult, DRM_VOID* pvOpaqueData, DRM_BOOL fSet);
static DRM_RESULT DRM_CALL _VariableAPPGetorSet       (const DRM_CONST_STRING* pdstrToken, TOKEN* pNewValue, TOKEN* pResult, DRM_VOID* pvOpaqueData, DRM_BOOL fSet);
static DRM_RESULT DRM_CALL _VariableSecStateGetorSet  (const DRM_CONST_STRING* pdstrToken, TOKEN* pNewValue, TOKEN* pResult, DRM_VOID* pvOpaqueData, DRM_BOOL fSet);
static DRM_RESULT DRM_CALL _VariableLicenseGetorSet   (const DRM_CONST_STRING* pdstrToken, TOKEN* pNewValue, TOKEN* pResult, DRM_VOID* pvOpaqueData, DRM_BOOL fSet);
static DRM_RESULT DRM_CALL _VariableContentGetorSet   (const DRM_CONST_STRING* pdstrToken, TOKEN* pNewValue, TOKEN* pResult, DRM_VOID* pvOpaqueData, DRM_BOOL fSet);
static DRM_RESULT DRM_CALL _VariableDeviceGetorSet    (const DRM_CONST_STRING* pdstrToken, TOKEN* pNewValue, TOKEN* pResult, DRM_VOID* pvOpaqueData, DRM_BOOL fSet);

typedef DRM_RESULT  ( DRM_CALL *pfnGetorSetVariable)(const DRM_CONST_STRING* pdstrToken, TOKEN* pNewValue, TOKEN* pResult, DRM_VOID* pvOpaqueData, DRM_BOOL fSet);

typedef struct tagDRM_VARIABLE_NAMESPACES
{
    const DRM_CONST_STRING *pdstrNamespace;
    pfnGetorSetVariable GetorSetVariable;
} DRM_VARIABLE_NAMESPACES;

static const DRM_VARIABLE_NAMESPACES g_VariableNamespaces[] =
{
    { &g_dstrDRM_PREFIX,          _VariableDRMGetorSet                },
    { &g_dstrDRMK_PREFIX,         _VariableDRMKGetorSet               },
    { &g_dstrMACHINE_PREFIX,      _VariableMachineGetorSet            },
    { &g_dstrAPP_PREFIX,          _VariableAPPGetorSet                },
    { &g_dstrSECSTATE_PREFIX,     _VariableSecStateGetorSet           },
    { &g_dstrLICENSE_PREFIX,      _VariableLicenseGetorSet            },
    { &g_dstrCONTENT_PREFIX,      _VariableContentGetorSet            },
    { &g_dstrDEVICE_PREFIX,       _VariableDeviceGetorSet             },
    { &g_dstrSECURETIME_PREFIX,   DRM_CLK_VariableSecureTimeGetorSet },

#if DRM_SUPPORT_SST_REDUNANCY
    { &g_dstrSSTREDUNDANCY_PREFIX, VariableSSTRedundancyGetorSet     },
#endif

};

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_EXPR_GlobalSetVariable(
    const DRM_CONST_STRING *pdstrToken,
    const TOKEN            *pNewValue,
          TOKEN            *pResult,
          DRM_VOID         *pvOpaqueData)
{
    DRM_DWORD i,j;

    /* Find the period first and use that to  */
    for( j=0; j<pdstrToken->cchString; j++ )
    {
        if( pdstrToken->pwszString[j] == g_wchPeriod )
        {
            j++;
            break;
        }
    }
    if( j < pdstrToken->cchString )
    {
        DRM_CONST_STRING dstrToken = EMPTY_DRM_STRING;
        dstrToken.pwszString = pdstrToken->pwszString + j;
        dstrToken.cchString  = pdstrToken->cchString  - j;

            for( i = 0; i < NO_OF( g_VariableNamespaces ); i++ )
        {
            if( j == g_VariableNamespaces[i].pdstrNamespace->cchString &&
                0 == DRMCRT_wcsncmp(pdstrToken->pwszString, g_VariableNamespaces[i].pdstrNamespace->pwszString, j) )
            {
                return g_VariableNamespaces[i].GetorSetVariable((const DRM_CONST_STRING *)&dstrToken, (TOKEN *)pNewValue, pResult, pvOpaqueData, TRUE ) ;
            }
        }
    }
    return DRM_E_CPRMEXP_INVALID_VARIABLE;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_WMDRM_EXPR_GlobalGetVariable(
    const DRM_CONST_STRING  *pdstrToken,
    TOKEN                   *pResult,
    DRM_VOID                *pvOpaqueData )
{
    DRM_DWORD i,j;

    /* Find the g_wchPeriod first and use that to  */
    for( j=0; j<pdstrToken->cchString; j++ )
    {
        if( pdstrToken->pwszString[j] == g_wchPeriod )
        {
            j++;
            break;
        }
    }
    if( j < pdstrToken->cchString )
    {
        DRM_CONST_STRING dstrToken;
        dstrToken.pwszString = pdstrToken->pwszString + j;
        dstrToken.cchString = pdstrToken->cchString - j;
        for( i = 0; i < NO_OF( g_VariableNamespaces ); i++ )
        {
            if( j == g_VariableNamespaces[i].pdstrNamespace->cchString &&
                0 == DRMCRT_wcsncmp(pdstrToken->pwszString, g_VariableNamespaces[i].pdstrNamespace->pwszString, j) )
            {
                return g_VariableNamespaces[i].GetorSetVariable(&dstrToken, NULL, pResult, pvOpaqueData, FALSE ) ;
            }
        }
    }
    return DRM_E_CPRMEXP_INVALID_VARIABLE;
}


static DRM_NO_INLINE DRM_RESULT DRM_CALL _VariableDRMKGetorSet(
    const DRM_CONST_STRING  *pdstrToken,
    TOKEN                   *pNewValue,
    TOKEN                   *pResult,
    DRM_VOID                *pvOpaqueData,
    DRM_BOOL                 fSet )
{
    DRM_LICEVAL_CONTEXT* pLicEval = (DRM_LICEVAL_CONTEXT*) pvOpaqueData;

    if ( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarVersion, pdstrToken ) )
    {
        if( fSet )
        {
            return DRM_E_CPRMEXP_UPDATE_UNSUPPORTED;
        }
        if( pLicEval->dstrDRMKVer.pwszString && pLicEval->dstrDRMKVer.cchString )
        {
            pResult->TokenType = TOKEN_STRING;
            pResult->val.stringValue.pwszString = pLicEval->dstrDRMKVer.pwszString;
            pResult->val.stringValue.cchString = pLicEval->dstrDRMKVer.cchString;
        }
        else
        {
            return DRM_E_CPRMEXP_RETRIEVAL_FAILURE;
        }
    }
    else if ( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarParameter, pdstrToken ) )
    {
        if( fSet )
        {
            if( pNewValue->TokenType != TOKEN_STRING )
            {
                return DRM_E_CPRMEXP_WRONG_TYPE_OPERAND;
            }
            pLicEval->wszDRMKParam.pwszString = pNewValue->val.stringValue.pwszString;
            pLicEval->wszDRMKParam.cchString  = pNewValue->val.stringValue.cchString;
        }
        else if( NULL == pLicEval->wszDRMKParam.pwszString || 0 == pLicEval->wszDRMKParam.cchString )
        {
            return DRM_E_CPRMEXP_RETRIEVAL_FAILURE;
        }

        pResult->TokenType = TOKEN_STRING;
        pResult->val.stringValue.pwszString = pLicEval->wszDRMKParam.pwszString;
        pResult->val.stringValue.cchString = pLicEval->wszDRMKParam.cchString;
    }
    else
    {
        return DRM_E_CPRMEXP_INVALID_VARIABLE;
    }

    return DRM_SUCCESS;
}


static DRM_NO_INLINE DRM_RESULT DRM_CALL _VariableDRMGetorSet(
    const DRM_CONST_STRING  *pdstrToken,
    TOKEN                   *pNewValue,
    TOKEN                   *pResult,
    DRM_VOID                *pvOpaqueData,
    DRM_BOOL                 fSet )
{
    DRM_LICEVAL_CONTEXT* pLicEval = (DRM_LICEVAL_CONTEXT*) pvOpaqueData;

    if( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarReason, pdstrToken ) )
    {
        if( !fSet )
        {
            return DRM_E_CPRMEXP_RETRIEVAL_FAILURE;
        }
        if( pNewValue->TokenType != TOKEN_LONG )
        {
            return DRM_E_CPRMEXP_WRONG_TYPE_OPERAND;
        }

        pResult->TokenType = TOKEN_LONG;
        pLicEval->lReasonForFail = pResult->val.lValue = pNewValue->val.lValue;
    }
    else if ( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarVersion, pdstrToken ) )
    {
        if( fSet )
        {
            return DRM_E_CPRMEXP_UPDATE_UNSUPPORTED;
        }
        if( pLicEval->dstrDRMVer.pwszString && pLicEval->dstrDRMVer.cchString )
        {
            pResult->TokenType = TOKEN_STRING;
            pResult->val.stringValue.pwszString = pLicEval->dstrDRMVer.pwszString;
            pResult->val.stringValue.cchString = pLicEval->dstrDRMVer.cchString;
        }
        else
        {
            return DRM_E_CPRMEXP_RETRIEVAL_FAILURE;
        }

    }
    else if ( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarBBMSDRMVersion, pdstrToken ) )
    {
        if( fSet )
        {
            return DRM_E_CPRMEXP_UPDATE_UNSUPPORTED;
        }
        if( pLicEval->dstrBBVer.pwszString && pLicEval->dstrBBVer.cchString )
        {
            pResult->TokenType = TOKEN_STRING;
            pResult->val.stringValue.pwszString = pLicEval->dstrBBVer.pwszString;
            pResult->val.stringValue.cchString = pLicEval->dstrBBVer.cchString;
        }
        else
        {
            return DRM_E_CPRMEXP_RETRIEVAL_FAILURE;
        }
    }
    else
    {
        return DRM_E_CPRMEXP_INVALID_VARIABLE;
    }

    return DRM_SUCCESS;
}

static DRM_NO_INLINE DRM_RESULT DRM_CALL _VariableMachineGetorSet(
    const DRM_CONST_STRING  *pdstrToken,
    TOKEN                   *pNewValue,
    TOKEN                   *pResult,
    DRM_VOID                *pvOpaqueData,
    DRM_BOOL                 fSet )
{
    DRM_RESULT           dr       = DRM_SUCCESS;
    DRM_LICEVAL_CONTEXT *pLicEval = (DRM_LICEVAL_CONTEXT*) pvOpaqueData;

    if( fSet )
    {
        ChkDR( DRM_E_CPRMEXP_UPDATE_UNSUPPORTED );
    }

    if ( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarDateTime, pdstrToken ) )
    {
        /* Use a temp local because the result of &(pResult->val.u64DateTime) may be unaligned on some platforms */
        DRM_UINT64 u64Temp = DRM_UI64LITERAL( 0, 0 );
        pResult->TokenType = TOKEN_DATETIME;
        ChkDR( DRM_LEVL_GetMachineDatetime( pLicEval, &u64Temp ) );
        pResult->val.u64DateTime = u64Temp;
    }
    else
    {
        ChkDR( DRM_E_CPRMEXP_INVALID_VARIABLE );
    }

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT DRM_CALL _VariableAPPGetorSet(
    const DRM_CONST_STRING  *pdstrToken,
    TOKEN                   *pNewValue,
    TOKEN                   *pResult,
    DRM_VOID                *pvOpaqueData,
    DRM_BOOL                 fSet )
{
    DRM_RESULT                  dr       = DRM_SUCCESS;
    const DRM_LICEVAL_CONTEXT  *pLicEval = (const DRM_LICEVAL_CONTEXT*) pvOpaqueData;

    pResult->TokenType = TOKEN_LONG;

    if( fSet )
    {
        /* Set not allowed */
        dr = DRM_E_CPRMEXP_UPDATE_UNSUPPORTED;
    }
    else if( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarCount, pdstrToken ) )
    {
        if( pLicEval->fLegacyAppInfoValid )
        {
            pResult->val.lValue = 2;
        }
        else
        {
            pResult->val.lValue = 1;
        }
    }
    else if( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarMinSecLevel, pdstrToken ) )
    {
        pResult->val.lValue = (DRM_LONG)pLicEval->certinfoSDK.appSec;
        if( pLicEval->fLegacyAppInfoValid )
        {
            if( pLicEval->certinfoAppLegacy.appSec < pLicEval->certinfoSDK.appSec )
            {
                pResult->val.lValue = (DRM_LONG)pLicEval->certinfoAppLegacy.appSec;
            }
        }
    }
    else if( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarSecLevel, pdstrToken ) )
    {
        pResult->val.lValue = (DRM_LONG)pLicEval->certinfoSDK.appSec;
    }
    else if( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarAppSecLevel, pdstrToken ) )
    {
        if( pLicEval->fLegacyAppInfoValid )
        {
            pResult->val.lValue = (DRM_LONG)pLicEval->certinfoAppLegacy.appSec;
        }
        else
        {
            pResult->val.lValue = (DRM_LONG)pLicEval->certinfoSDK.appSec;
        }
    }
    else if( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarSubjID, pdstrToken ) )
    {
        pResult->val.lValue = (DRM_LONG)pLicEval->certinfoSDK.appcd_subject;
    }
    else if( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarAppSubjID, pdstrToken ) )
    {
        if( pLicEval->fLegacyAppInfoValid )
        {
            pResult->val.lValue = (DRM_LONG)pLicEval->certinfoAppLegacy.appcd_subject;
        }
        else
        {
            pResult->val.lValue = (DRM_LONG)pLicEval->certinfoSDK.appcd_subject;
        }
    }
    else
    {
        dr  = DRM_E_CPRMEXP_RETRIEVAL_FAILURE;
    }

    return dr;
}

static DRM_NO_INLINE DRM_RESULT DRM_CALL _VariableSecStateGetorSet(
    const DRM_CONST_STRING  *pdstrToken,
    TOKEN                   *pNewValue,
    TOKEN                   *pResult,
    DRM_VOID                *pvOpaqueData,
    DRM_BOOL                 fSet )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_LICEVAL_CONTEXT  *pLicEval  = (DRM_LICEVAL_CONTEXT*) pvOpaqueData;
    DRM_SECSTORE_CONTEXT *pbContext = NULL;
    DRM_CONST_STRING      dstrToken = EMPTY_DRM_STRING;

    dstrToken.pwszString = pdstrToken->pwszString;
    dstrToken.cchString  = pdstrToken->cchString;

    if ( fSet && pLicEval->fIsReadOnlyMode )
    {
        goto ErrorExit; /* ignore setting the secstore if fIsReadOnlyMode is set */
    }

    if( g_dstrExprVarGlobal.cchString < dstrToken.cchString &&
        DRMCRT_wcsncmp( g_dstrExprVarGlobal.pwszString, dstrToken.pwszString, g_dstrExprVarGlobal.cchString ) == 0 )
    {
        /* Move the pointer past the globalstate string */
        dstrToken.pwszString += g_dstrExprVarGlobal.cchString;
        dstrToken.cchString  -= g_dstrExprVarGlobal.cchString;


        /* Look for both savedatetime and saveddatetime due to a difference in older WMRM versions */
        if( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarSaveDateTime, &dstrToken ) )
        {
            dstrToken.pwszString = g_dstrExprVarSavedDateTime.pwszString;
            dstrToken.cchString = g_dstrExprVarSavedDateTime.cchString;
        }

        /* It is a global state variable.  If this is a set call we have to check to see if set is available right now */
        if( fSet )
        {
            if( !pLicEval->fGlobalSecStoreWritable )
            {
                dr = DRM_E_CPRMEXP_UPDATE_FAILURE;
                goto ErrorExit;
            }
            else if( ( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarSavedDateTime, &dstrToken ) )&&
                    pNewValue->TokenType != TOKEN_DATETIME )
            {
                /* saveddatetime MUST be a DATE token */
                dr = DRM_E_CPRMEXP_WRONG_TYPE_OPERAND;
                goto ErrorExit;
            }
        }
        pbContext = pLicEval->pcontextSSTGlobal;
    }
    else if( DRM_CLK_IsSecureClockSupported()
          && g_dstrSECURETIME_PREFIX.cchString < dstrToken.cchString
          && DRMCRT_wcsncmp( g_dstrSECURETIME_PREFIX.pwszString, dstrToken.pwszString, g_dstrSECURETIME_PREFIX.cchString ) == 0 )
    {
        /* Move the pointer past the securetime string */
        dstrToken.pwszString += g_dstrSECURETIME_PREFIX.cchString;
        dstrToken.cchString  -= g_dstrSECURETIME_PREFIX.cchString;
        dr = DRM_CLK_VariableSecureTimeGetorSet( &dstrToken, pNewValue, pResult, pvOpaqueData, fSet);
        goto ErrorExit;
    }
    else
    {
        pbContext = pLicEval->pcontextSSTLicense;
    }

    if( NULL == pbContext )
    {
        dr = DRM_E_CPRMEXP_RETRIEVAL_FAILURE;
    }
    else if( fSet )
    {
        TOKEN tempToken;

        if( pNewValue->TokenType == TOKEN_STRING )
        {
            /* String types can not be assigned to secure store variables */
            dr = DRM_E_CPRMEXP_WRONG_TYPE_OPERAND;
            goto ErrorExit;
        }
        MEMCPY( &tempToken, pNewValue, SIZEOF( TOKEN ) );

        if( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarDRMReason, &dstrToken ) )
        {
            pLicEval->lReasonForFail = pNewValue->val.lValue;
        }
        else
        {
            dr = DRM_SST_SetTokenValue( pbContext, &dstrToken, &tempToken);
            if( DRM_FAILED( dr ) )
            {
                dr = DRM_E_CPRMEXP_UPDATE_FAILURE;
                goto ErrorExit;
            }
        }
        if( pResult )
        {
            MEMCPY( pResult, pNewValue, SIZEOF( TOKEN ) );
        }
    }
    else
    {
        dr = DRM_SST_GetTokenValue( pbContext, &dstrToken, pResult);
        if( dr == DRM_S_FALSE )
        {
            dr = DRM_E_CPRMEXP_RETRIEVAL_FAILURE;
        }
        ChkDR( dr );
    }

ErrorExit:
    return dr;
}

static DRM_NO_INLINE DRM_RESULT DRM_CALL _VariableLicenseGetorSet(
    const DRM_CONST_STRING  *pdstrToken,
    TOKEN                   *pNewValue,
    TOKEN                   *pResult,
    DRM_VOID                *pvOpaqueData,
    DRM_BOOL                 fSet )
{
    DRM_RESULT                 dr       = DRM_SUCCESS;
    const DRM_LICEVAL_CONTEXT *pLicEval = (const DRM_LICEVAL_CONTEXT*) pvOpaqueData;

    if( fSet )
    {
        /* Set not allowed */
        return DRM_E_CPRMEXP_UPDATE_UNSUPPORTED;
    }
    pResult->TokenType = TOKEN_STRING;
    if( NULL == pLicEval->dstrContentLicense.pwszString ||
        0 == pLicEval->dstrContentLicense.cchString ||
        DRM_FAILED( DRM_WMDRM_LIC_GetAttribute( &(pLicEval->dstrContentLicense), pdstrToken, DRM_LICENSE_ATTRIB_OTHER, NULL, &(pResult->val.stringValue), g_wchPeriod ) )
        )
    {
        dr = DRM_E_CPRMEXP_RETRIEVAL_FAILURE;
    }

    return dr;
}

static DRM_NO_INLINE DRM_RESULT DRM_CALL _VariableContentGetorSet(
    const DRM_CONST_STRING  *pdstrToken,
    TOKEN                   *pNewValue,
    TOKEN                   *pResult,
    DRM_VOID                *pvOpaqueData,
    DRM_BOOL                 fSet )
{
    DRM_RESULT                 dr       = DRM_SUCCESS;
    const DRM_LICEVAL_CONTEXT *pLicEval = (const DRM_LICEVAL_CONTEXT*) pvOpaqueData;

    /* Use content header parsing to extract a property */
    if( fSet )
    {
        return DRM_E_CPRMEXP_UPDATE_UNSUPPORTED;
    }
    pResult->TokenType = TOKEN_STRING;
    if( NULL == pLicEval->dstrContentHeader.pwszString ||
        0 == pLicEval->dstrContentHeader.cchString ||
        DRM_FAILED( DRM_HDR_GetAttribute( &(pLicEval->dstrContentHeader), pdstrToken, DRM_HEADER_ATTRIB_OTHER, &(pResult->val.stringValue), g_wchPeriod ) )
        )
    {
        dr = DRM_E_CPRMEXP_RETRIEVAL_FAILURE;
    }

    return dr;
}

static DRM_NO_INLINE DRM_RESULT DRM_CALL _VariableDeviceGetorSet(
    const DRM_CONST_STRING  *pdstrToken,
    TOKEN                   *pNewValue,
    TOKEN                   *pResult,
    DRM_VOID                *pvOpaqueData,
    DRM_BOOL                 fSet )
{
    /* We are depricating the device namespace */
#if !DRM_SUPPORT_DEVICE_NAMESPACE
    return DRM_E_NOTIMPL;
#else

    DRM_LICEVAL_CONTEXT* pContext = (DRM_LICEVAL_CONTEXT*) pvOpaqueData;

    DRMASSERT( pContext != NULL );
    if( pContext == NULL )
    {
        return DRM_E_INVALIDARG;
    }

    /* device-registered flag */
    if( DRM_UTL_DSTRStringsEqual( &g_dstrExprVarDevReg, pdstrToken ) )
    {
        if( fSet )
        {
            if( pNewValue->TokenType != TOKEN_LONG )
            {
                return DRM_E_CPRMEXP_WRONG_TYPE_OPERAND;
            }
            pContext->fDeviceRegistered = pNewValue->val.lValue;
        }
        pResult->TokenType  = TOKEN_LONG;
        pResult->val.lValue = pContext->fDeviceRegistered;
    }
    else
    {
        return DRM_E_CPRMEXP_RETRIEVAL_FAILURE;
    }

    return DRM_SUCCESS;

#endif

}

PREFAST_POP /* The array of function pointers means these must all have identical signatures */

EXIT_PK_NAMESPACE_CODE;

