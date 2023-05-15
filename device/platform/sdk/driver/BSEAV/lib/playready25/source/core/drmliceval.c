/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMLICEVAL_C
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmliceval.h>
#include <drmheaderparser.h>
#include <drmsecurestore.h>
#include <drmxmlparser.h>
#include <drmlicreason.h>
#include <oemaes.h>
#include <oem.h>
#include <drmdomainstore.h>
#include <drmapppolicy.h>
#include <drmantirollbackclock.h>
#include <drmsymopt.h>
#include <drmcontentrevocation.h>
#include <drmmove.h>
#include <drmthumbnail.h>
#include <drmrevocation.h>
#include <drmapprevocation.h>
#include <drmsecureclock.h>
#include <drmwmdrm.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

typedef struct __tagDRM_STRING_AND_GUID
{
    const DRM_CONST_STRING    *pdstrString;
    const DRM_GUID            *pdguidGuid;
} DRM_STRING_TO_GUID;

/******************************************************************************
**
** Action String and GUID pairs for conversion into each other
**
******************************************************************************/
static const DRM_STRING_TO_GUID g_ActionStringAndGuidConversion [] =
{
    { &g_dstrWMDRM_RIGHT_PLAYBACK,               &DRM_ACTION_PLAY               },
    { &g_dstrWMDRM_RIGHT_COPY,                   &DRM_ACTION_COPY               },
    { &g_dstrWMDRM_RIGHT_CREATE_THUMBNAIL_IMAGE, &DRM_ACTION_CREATE_THUMBNAIL   },
    { &g_dstrWMDRM_RIGHT_COLLABORATIVE_PLAY,     &DRM_ACTION_COLLABORATIVE_PLAY },
    { &g_dstrWMDRM_RIGHT_COPY_TO_CD,             &DRM_ACTION_COPY_TO_CD         },
};

/******************************************************************************
**
** Function: _ConvertActionStringToActionGUIDAndRightID
**
** Synopsis: Looks up a table to convert Action string into Action GUID.
**           Maps any string that it could not find to a Zero GUID (including empty string)
**           If string not found in the lookup table - tries to convert action string
**           into a RightID DWORD.
**
**
** Arguments:
**      [f_pdstrAction]           : Specifies Action string
**      [f_pdguidAction]          : Returns Action GUID
**      [f_pdwRightID]            : Returns Right ID
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or improperly initialized
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _ConvertActionStringToActionGUIDAndRightID(
    __in     DRM_CONST_STRING const    *f_pdstrAction,
    __inout  DRM_GUID                  *f_pdguidAction,
    __inout  DRM_DWORD                 *f_pdwRightID ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT DRM_CALL _ConvertActionStringToActionGUIDAndRightID(
    __in     DRM_CONST_STRING const    *f_pdstrAction,
    __inout  DRM_GUID                  *f_pdguidAction,
    __inout  DRM_DWORD                 *f_pdwRightID )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT              dr                  = DRM_SUCCESS;
    DRM_DWORD               dwCount             = 0;
    DRM_BOOL                fFound              = FALSE;
    DRM_GUID                dguidZero           = EMPTY_DRM_GUID;
    DRM_LONG                lRightID            = 0;

    ChkArg( NULL != f_pdstrAction );
    ChkArg( NULL != f_pdguidAction );
    ChkArg( NULL != f_pdwRightID );

    *f_pdwRightID = 0;

    /*
    ** Anything that we would not be able to map becomes a zero GUID
    */
    MEMCPY( f_pdguidAction, &dguidZero, SIZEOF(DRM_GUID) );

    if ( 0 < f_pdstrAction->cchString )
    {
        for ( dwCount = 0 ; dwCount < NO_OF( g_ActionStringAndGuidConversion ) && !fFound ; ++dwCount )
        {
            if ( DRM_UTL_DSTRStringsEqual( f_pdstrAction, g_ActionStringAndGuidConversion[ dwCount ].pdstrString ) )
            {
                MEMCPY( f_pdguidAction, g_ActionStringAndGuidConversion[ dwCount ].pdguidGuid, SIZEOF(DRM_GUID) );
                fFound = TRUE;
            }
        }

        /*
        ** Not found in the lookup table? Try to convert the Action string into a Right ID
        */
        if ( !fFound )
        {
            if( DRM_SUCCEEDED( DRMCRT_wcsntol( f_pdstrAction->pwszString, f_pdstrAction->cchString, &lRightID ) ) && ( lRightID > 0 ) )
            {
                *f_pdwRightID = (DRM_DWORD)lRightID;
            }
        }
    }

ErrorExit:

    return dr;
}


/******************************************************************************
**
** Function: DRM_LEVL_SearchQualifierGuidInEnablerContainer
**
** Synopsis: Searches the Qualifier GUID in Enabler Container.
**           If not found - does depth-first search in other containers
**           (i.e. checks nested containers first)
**           Uses recursion.
**
** Arguments:
**      [f_pdguidActionQualifier] : Specifies action qualifier GUID for DRM_LICENSE_EVAL_ACTION operation
**      [f_pEnablerContainer]     : Specifies container to search in.
**      [f_wContainerType]        : Specifies the expected container type for license verification
**      [f_wObjectType]           : Specifies the expected object type for license verification
**      [f_fFound]                : Returns TRUE if the GUID was found in any object of any container
**                                  False otherwise.
**      [f_ppEnablerContainerFound] : Returns the container containing the object with the specified GUID
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or improperly initialized
**      DRM_E_INVALID_LICENSE
**          The license has Unknown container or object of unexpected type
**          (in place of Enabler Container or Object of a proper type)
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LEVL_SearchQualifierGuidInEnablerContainer(
    __in        DRM_GUID const                    *f_pdguidActionQualifier,
    __in        DRM_XMR_UNKNOWN_CONTAINER const   *f_pEnablerContainer,
    __in        DRM_WORD const                     f_wContainerType,
    __in        DRM_WORD const                     f_wObjectType,
    __inout     DRM_BOOL                          *f_pfFound,
    __inout_opt DRM_XMR_UNKNOWN_CONTAINER const  **f_ppEnablerContainerFound )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT                    dr                  = DRM_SUCCESS;
    const DRM_XMR_UNKNOWN_OBJECT *pObject             = NULL;

    ChkArg( NULL != f_pdguidActionQualifier
         && NULL != f_pEnablerContainer
         && NULL != f_pfFound );

    /*
    ** If the enabler container isn't valid, there's no way we'll
    ** find any specific enabler under it.
    */
    if( f_pEnablerContainer->fValid )
    {
        /*
        ** Check that we have a container of the proper type
        */
        ChkBOOL( f_wContainerType == f_pEnablerContainer->wType, DRM_E_INVALID_LICENSE );

        /*
        ** First, check Enabler Objects in this container
        */
        pObject = f_pEnablerContainer->pObject;
        while ( NULL != pObject )
        {
            /*
            ** Check that we have a object of the proper type
            ** Don't fail if we don't because it could be an enabler-specific restriction,
            ** instead just move on to the next object
            */
            if( pObject->fValid && ( f_wObjectType == pObject->wType ) )
            {
                DRM_GUID    dguidFromObject = EMPTY_DRM_GUID;

                /*
                ** Check that the object contains a GUID
                */
                ChkBOOL( SIZEOF(DRM_GUID) == (pObject->cbData), DRM_E_INVALID_LICENSE );

                DRM_BYT_CopyBytes(
                    &dguidFromObject,
                    0,
                    pObject->pbBuffer,
                    pObject->ibData,
                    SIZEOF(DRM_GUID) );
                if ( IDENTICAL_GUIDS( &dguidFromObject, f_pdguidActionQualifier ) )
                {
                    *f_pfFound = TRUE;
                    if( f_ppEnablerContainerFound != NULL )
                    {
                        *f_ppEnablerContainerFound = f_pEnablerContainer;
                    }
                    break;
                }
            }
            pObject = pObject->pNext;
        }

        /*
        ** Did not find? Check child containers (that is, we are going depth-first container-wise)
        */
        if ( !(*f_pfFound) && NULL != f_pEnablerContainer->pUnkChildcontainer )
        {
            ChkDR( DRM_LEVL_SearchQualifierGuidInEnablerContainer(
                                            f_pdguidActionQualifier,
                                            f_pEnablerContainer->pUnkChildcontainer,
                                            f_wContainerType,
                                            f_wObjectType,
                                            f_pfFound,
                                            f_ppEnablerContainerFound ) );
        }

        /*
        ** Still did not find? Check linked containers
        */
        if ( !(*f_pfFound) && NULL != f_pEnablerContainer->pNext )
        {
            ChkDR( DRM_LEVL_SearchQualifierGuidInEnablerContainer(
                                            f_pdguidActionQualifier,
                                            f_pEnablerContainer->pNext,
                                            f_wContainerType,
                                            f_wObjectType,
                                            f_pfFound,
                                            f_ppEnablerContainerFound ) );
        }
    }

ErrorExit:
    return dr;
}

/******************************************************************************
 **
 ** Function: _DRM_LEVL_PerformCallbackInEnablerContainer
 **
 ** Synopsis: Searches for specific enabler objects in Enabler Container and
 **           perform callback if needed.
 **           If not found - does depth-first search in other containers
 **           (i.e. checks nested containers first)
 **           Uses recursion.
 **
 ** Arguments:
 **      [f_pcontextLicEval]       : Specifies the liceval context to use.
 **      [f_wRightID]              : Right ID to be performed.
 **      [f_pEnablerContainer]     : Specifies container to search in.
 **      [f_wContainerType]        : Specifies the expected container type for license verification
 **      [f_wObjectType]           : Specifies the expected object type for license verification
 **      [f_pfnPolicyCallback]     : Callback function for output restrictions.
 **      [f_pv]                    : Void pass-through parameter for callback.
 **      [f_pfFound]               : Pointer to a variable to receive whether at least one matching enabler is found.
 **                                  Note: this parameter should be set to FALSE when this function is called
 **                                  for the first time.
 **      [f_pfSuccess]             : Pointer to a variable to receive the aggregated enabler callback result:
 **                                  If at least one enabler callback succeeds, the result is true,
 **                                  otherwise the result is false.
 **                                  Note: this parameter should be set to FALSE when this function is called
 **                                  for the first time.
 **
 ** Returns:
 **      DRM_SUCCESS
 **          Success
 **      DRM_E_INVALIDARG
 **          One of the parameters is NULL or improperly initialized
 **      DRM_E_INVALID_LICENSE
 **          The license has Unknown container or object of unexpected type
 **          (in place of Enabler Container or Object of a proper type)
 **
 ******************************************************************************/
static DRM_NO_INLINE DRM_RESULT DRM_CALL _DRM_LEVL_PerformCallbackInEnablerContainer(
    __inout     DRM_LICEVAL_CONTEXT               *f_pContextLicEval,
    __in        DRM_WORD                           f_wRightID,
    __in        DRM_XMR_UNKNOWN_CONTAINER const   *f_pEnablerContainer,
    __in        DRM_WORD const                     f_wContainerType,
    __in        DRM_WORD const                     f_wObjectType,
    __in        DRMPFNPOLICYCALLBACK               f_pfnPolicyCallback,
    __in_opt    DRM_VOID const                    *f_pv,
    __inout     DRM_BOOL                          *f_pfFound,
    __inout     DRM_BOOL                          *f_pfSuccess)
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT                    dr                  = DRM_SUCCESS;
    const DRM_XMR_UNKNOWN_OBJECT *pObject             = NULL;

    ChkArg(   NULL != f_pContextLicEval
           && NULL != f_pEnablerContainer
           && NULL != f_pfnPolicyCallback
           && NULL != f_pfFound
           && NULL != f_pfSuccess );

    /*
     ** If the enabler container isn't valid, there's no way we'll
     ** find any specific enabler under it.
     */
    if( f_pEnablerContainer->fValid )
    {
        /*
         ** Check that we have a container of the proper type
         */
        ChkBOOL( f_wContainerType == f_pEnablerContainer->wType, DRM_E_INVALID_LICENSE );

        /*
         ** First, check Enabler Objects in this container
         */
        pObject = f_pEnablerContainer->pObject;
        while ( NULL != pObject )
        {
            /*
             ** Check that we have a object of the proper type.
             */
            if( pObject->fValid && ( f_wObjectType == pObject->wType ) )
            {
                DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT oCallback = {0};
                DRM_RESULT drCallback = DRM_SUCCESS;

                /*
                 ** Check that the object contains a GUID
                 */
                ChkBOOL( SIZEOF(DRM_GUID) == (pObject->cbData), DRM_E_INVALID_LICENSE );

                *f_pfFound = TRUE;

                oCallback.wRightID = f_wRightID;
                oCallback.pRestriction = ( DRM_XMR_UNKNOWN_OBJECT * )pObject;
                oCallback.pXMRLicense = f_pContextLicEval->plicenseXMR;
                oCallback.pContextSST = f_pContextLicEval->pcontextSSTLicense;

                drCallback = f_pfnPolicyCallback( &oCallback, DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK, f_pv );
                if ( DRM_SUCCEEDED( drCallback ) )
                {
                    *f_pfSuccess = TRUE;
                }
            }
            pObject = pObject->pNext;
        }

        /*
         ** Check child containers (that is, we are going depth-first container-wise)
         */
        if ( NULL != f_pEnablerContainer->pUnkChildcontainer )
        {
            ChkDR( _DRM_LEVL_PerformCallbackInEnablerContainer( f_pContextLicEval,
                                                               f_wRightID,
                                                               f_pEnablerContainer->pUnkChildcontainer,
                                                               f_wContainerType,
                                                               f_wObjectType,
                                                               f_pfnPolicyCallback,
                                                               f_pv,
                                                               f_pfFound,
                                                               f_pfSuccess ) );
        }

        /*
         ** Check linked containers
         */
        if ( NULL != f_pEnablerContainer->pNext )
        {
            ChkDR( _DRM_LEVL_PerformCallbackInEnablerContainer( f_pContextLicEval,
                                                               f_wRightID,
                                                               f_pEnablerContainer->pNext,
                                                               f_wContainerType,
                                                               f_wObjectType,
                                                               f_pfnPolicyCallback,
                                                               f_pv,
                                                               f_pfFound,
                                                               f_pfSuccess ) );
        }
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function: _CheckCount
**
** Synopsis: Check the count for a given right on an XMR license
**
** Arguments:
**      [f_pcontextSST]        : Specifies the secure store context
**      [f_pdstrRight]         : Specifies the right string to check
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or improperly initialized
**      DRM_E_LICENSE_EXPIRED
**          The license has expired
**      LICEVAL_INVALID_LICENSE
**          The specified license is invalid
**      DRM_E_CPRMEXP_RETRIEVAL_FAILURE
**          Unable to retrieve the count data
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _CheckCount(
    __inout       DRM_SECSTORE_CONTEXT *f_pcontextSST,
    __in          DRM_DWORD             f_dwOriginalCountFromLicense,
    __in    const DRM_CONST_STRING     *f_pdstrRight ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _CheckCount(
    __inout       DRM_SECSTORE_CONTEXT *f_pcontextSST,
    __in          DRM_DWORD             f_dwOriginalCountFromLicense,
    __in    const DRM_CONST_STRING     *f_pdstrRight )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr    = DRM_SUCCESS;
    TOKEN      token = { 0 };

    ChkBOOL( f_pcontextSST != NULL, DRM_E_INVALID_LICENSE );
    ChkDR( DRM_SST_GetTokenValue( f_pcontextSST, f_pdstrRight, &token ) );
    ChkBOOL( token.TokenType == TOKEN_LONG, DRM_E_CPRMEXP_RETRIEVAL_FAILURE );
    ChkBOOL( token.val.lValue > 0, DRM_E_LICENSE_EXPIRED );

    /* This check is to ensure that if a secure state entry was maliciously created with
       false data, we detect it at runtime.  A more actionable error isn't necessary as
       this case can only happen if someone has deliberately tampered with the system */
    ChkBOOL( token.val.lValue <= (DRM_LONG)f_dwOriginalCountFromLicense, DRM_E_INVALID_LICENSE );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function: _DecrementCount
**
** Synopsis: Decrement the count for a given right on an XMR license
**
** Arguments:
**      [f_pcontextSST]        : Specifies the secure store context
**      [f_pdstrRight]         : Specifies the right string to check
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or improperly initialized
**      DRM_E_CPRMEXP_RETRIEVAL_FAILURE
**          Unable to retrieve the count data
**
******************************************************************************/
static DRM_NO_INLINE DRM_RESULT _DecrementCount(
    __inout       DRM_SECSTORE_CONTEXT *f_pcontextSST,
    __in    const DRM_CONST_STRING     *f_pdstrRight ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _DecrementCount(
    __inout       DRM_SECSTORE_CONTEXT *f_pcontextSST,
    __in    const DRM_CONST_STRING     *f_pdstrRight )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr    = DRM_SUCCESS;
    TOKEN      token = { 0 };

    ChkDR( DRM_SST_GetTokenValue( f_pcontextSST, f_pdstrRight, &token ) );
    ChkBOOL( token.TokenType == TOKEN_LONG, DRM_E_CPRMEXP_RETRIEVAL_FAILURE );
    token.val.lValue--;
    ChkDR( DRM_SST_SetTokenValue( f_pcontextSST, f_pdstrRight, &token ) );

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function: _IsAllowedSourceID
**
** Synopsis: Verifies whether the application is allowed access to the source
**           of the current content
**
** Arguments:
**      [f_pContextLicEval] : Specifies the license context
**
** Returns:
**      TRUE
**          The application is allowed access to the source
**      FALSE
**          The application is not allowed access to the source
**
******************************************************************************/
static DRM_NO_INLINE DRM_BOOL _IsAllowedSourceID(
    __in const DRM_LICEVAL_CONTEXT *f_pContextLicEval ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_BOOL _IsAllowedSourceID(
    __in const DRM_LICEVAL_CONTEXT *f_pContextLicEval )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_LONG  cAllowedSourceIDs = 0;
    DRM_BOOL  fReturn           = FALSE;

    if ( f_pContextLicEval == NULL )
    {
        return FALSE;
    }

    if ( f_pContextLicEval->fAllowAllSourceIDs )
    {
        fReturn = TRUE;
    }

    cAllowedSourceIDs = (DRM_LONG)f_pContextLicEval->cAllowedSourceIDs;
    while ( cAllowedSourceIDs > 0 && !fReturn )
    {
        cAllowedSourceIDs--;

        if ( f_pContextLicEval->plicenseXMR->containerOuter.containerGlobalPolicies.SourceID.dwValue
          == f_pContextLicEval->rgdwAllowedSourceIDs[cAllowedSourceIDs] )
        {
            fReturn = TRUE;
        }
    }

    return fReturn;
}

static DRM_NO_INLINE DRM_RESULT DRM_CALL _InvokeExtensibleRestrictionsCallbacks(
    __inout  DRM_LICEVAL_CONTEXT    *f_pContextLicEval,
    __in     DRM_XMR_UNKNOWN_OBJECT *f_pExtendedRestrictions,
    __in     DRM_WORD                f_wRightID,
    __in_opt DRMPFNPOLICYCALLBACK    f_pfnPolicyCallback,
    __in_opt DRM_VOID const         *f_pv ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT DRM_CALL _InvokeExtensibleRestrictionsCallbacks(
    __inout  DRM_LICEVAL_CONTEXT    *f_pContextLicEval,
    __in     DRM_XMR_UNKNOWN_OBJECT *f_pExtendedRestrictions,
    __in     DRM_WORD                f_wRightID,
    __in_opt DRMPFNPOLICYCALLBACK    f_pfnPolicyCallback,
    __in_opt DRM_VOID const         *f_pv )
{
    DRM_RESULT              dr               = DRM_SUCCESS;
    const DRM_XMR_LICENSE  *pXMR             = NULL;
    DRM_XMR_UNKNOWN_OBJECT *pExtRestrictLoop = NULL;
    CLAW_AUTO_RANDOM_CIPHER

    ChkArg(f_pContextLicEval != NULL );
    ChkArg(f_pExtendedRestrictions != NULL );

    pXMR = f_pContextLicEval->plicenseXMR;
    pExtRestrictLoop = f_pExtendedRestrictions;

    ChkDR( DRM_APPP_RequireExecutePolicy( f_wRightID,
                                          &pXMR->containerOuter.containerGlobalPolicies,
                                          pExtRestrictLoop,
                                          f_pContextLicEval ) );

    while( pExtRestrictLoop != NULL
        && pExtRestrictLoop->fValid )
    {
        if( ( pExtRestrictLoop->wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE ) != 0 )
        {
            /*
            ** This is likely an extensible rights container.
            ** Need to pass it up to the app for them to parse.
            */

            dr = DRM_E_EXTENDED_RESTRICTION_NOT_UNDERSTOOD;
            if( f_pfnPolicyCallback != NULL )
            {
                DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT oCallback = {0};

                oCallback.wRightID = f_wRightID;
                oCallback.pRestriction = pExtRestrictLoop;
                oCallback.pXMRLicense = f_pContextLicEval->plicenseXMR;
                oCallback.pContextSST = f_pContextLicEval->pcontextSSTLicense;

                dr = f_pfnPolicyCallback( &oCallback, DRM_EXTENDED_RESTRICTION_CONDITION_CALLBACK, f_pv );
            }

            /*
            ** If either there was no callback specified or if the callback responded
            ** with DRM_E_EXTENDED_RESTRICTION_NOT_UNDERSTOOD, then the restriction was not processed
            */
            if( dr == DRM_E_EXTENDED_RESTRICTION_NOT_UNDERSTOOD || dr == DRM_E_NOTIMPL )
            {
                if( ( pExtRestrictLoop->wFlags & XMR_FLAGS_MUST_UNDERSTAND ) != 0 )
                {
                    TRACE(("Extensible restrction marked as must understand, but couldn't. Callback: 0x%X", f_pfnPolicyCallback ));
                    ChkDR( DRM_E_EXTENDED_RESTRICTION_NOT_UNDERSTOOD );
                }
                else
                {
                    /*
                    ** Callback didn't understand the restriction, but they didn't have to.
                    */
                    dr = DRM_SUCCESS;
                }
            }
            else if( dr == DRM_E_LICENSE_EXPIRED )
            {
                f_pContextLicEval->lReasonForFail = LR_LICENSE_EXPIRED;
            }
            ChkDR( dr );

        }

        pExtRestrictLoop = pExtRestrictLoop->pNext;
    }

ErrorExit:
    return dr;
}


/******************************************************************************
**
** Function: DRM_LEVL_PerformOperationsXMR
**
** Synopsis: Do an operation on an XMR license
**
** Arguments:
**      [f_pcontextLicEval] : Specifies the liceval context to use
**      [f_eOperation]      : Specifies the operation to be perfomed
**      [f_eOperationState] :
**      [f_pdguidAction]    : Specifies action GUID for DRM_LICENSE_EVAL_ACTION operation
**      [f_pdguidActionQualifier] : Specifies action qualifier GUID for DRM_LICENSE_EVAL_ACTION operation
**      [f_dwRightID]       : Specifies optional right ID
**      [f_pfPerform]       : Returns whether the action has been performed
**      [f_pfActionExisted] : Returns whether the action existed
**      [f_pDatastore]      : Specifies the DST context
**      [f_pfnPolicyCallback] : Callback function for output restrictions
**      [f_pv]              : Void pass-through parameter for callback
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or improperly initialized
**      DRM_E_LICEVAL_LICENSE_NOT_SUPPLIED
**          The license content string is NULL
**      DRM_E_LICEVAL_INVALID_LICENSE
**          The specified license is invalid
**      DRM_E_LICEVAL_LICENSE_REVOKED
**          The specified license has been revoked
**      DRM_E_LICEVAL_KID_MISMATCH
**          KID mismatch with header
**      DRM_E_CH_UNABLE_TO_VERIFY
**          Unable to verify the signature of the content header
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LEVL_PerformOperationsXMR(
    __inout     DRM_LICEVAL_CONTEXT          *f_pContextLicEval,
    __in        eDRM_LICEVAL_OPERATIONS       f_eOperation,
    __in        eDRM_LICEVAL_OPERATION_STATE  f_eOperationState,
    __in_opt    DRM_GUID const               *f_pdguidAction,
    __in_opt    DRM_GUID const               *f_pdguidActionQualifier,
    __in        DRM_DWORD const               f_dwRightID,
    __out_opt   DRM_BOOL                     *f_pfPerform,
    __out_opt   DRM_BOOL                     *f_pfActionExisted,
    __in        DRM_DST const                *f_pDatastore,
    __in_opt    DRMPFNPOLICYCALLBACK          f_pfnPolicyCallback,
    __in_opt    DRM_VOID const               *f_pv )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT              dr                 = DRM_SUCCESS;
    DRM_DWORD               dwTemp             = XMR_UNLIMITED;
    DRM_WORD                wTemp              = 0;
    DRM_BOOL                fCondition         = TRUE;
    DRM_BOOL                fRestricted        = FALSE;
    DRMFILETIME             ftXMR              = {0};
    DRMFILETIME             ftCurrent          = {0};
    DRM_UINT64              ui64Current        = DRM_UI64LITERAL( 0, 0 );
    DRM_UINT64              ui64XMR            = DRM_UI64LITERAL( 0, 0 );
    DRM_XMR_LICENSE        *pXMR               = NULL;
    TOKEN                   token              = { 0 };
    DRM_BOOL                fDisabled          = FALSE;
    const DRM_CONST_STRING *pdstrCountString   = NULL;
    DRM_XMR_UNKNOWN_CONTAINER *pExtendedRight                   = NULL;
    DRM_XMR_UNKNOWN_OBJECT    *pExtendedRestrictions            = NULL;
    DRM_XMR_UNKNOWN_OBJECT    *pExtendedRestrictionsFromEnabler = NULL;
    DRM_BOOL                   fReturnStoreError                = FALSE;
    DRM_BOOL                   fLicenseRequiresSST              = TRUE;
    DRM_BOOL                   fIgnoreSecureStoreEntry          = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMLICEVAL, PERF_FUNC_DRM_LEVL_PerformOperationsXMR );

    ChkArg( f_pContextLicEval != NULL );
    ChkArg( f_pDatastore      != NULL );

    ChkBOOL( f_pContextLicEval->plicenseXMR != NULL, DRM_E_LICEVAL_LICENSE_NOT_SUPPLIED );
    ChkArg( f_pContextLicEval->plicenseXMR->containerOuter.fValid );

    /* Ignore SST for PRND licenses.  Tx will handle anti-clock. */
    fIgnoreSecureStoreEntry = ( ( ( f_pContextLicEval->dwFlags & LICEVAL_IGNORE_SECURESTORE_ENTRY ) != 0 ) || f_pContextLicEval->fLicenseFromPRND );
    f_pContextLicEval->fDomainBoundRootRequired = FALSE;
    f_pContextLicEval->fMoveListCheckRequired   = FALSE;
    f_pContextLicEval->ui64RealTimeExpiration   = DRM_UI64HL(MAX_UNSIGNED_TYPE(DRM_DWORD),MAX_UNSIGNED_TYPE(DRM_DWORD));

    pXMR         = f_pContextLicEval->plicenseXMR;

    switch( f_eOperationState )
    {
    case DRM_LICENSE_EVAL_CAN_DO_OPERATION:
        fCondition = TRUE;
        break;
    case DRM_LICENSE_EVAL_DONE_WITH_OPERATION:
        fCondition = FALSE;
        break;
    default:
        ChkArg( FALSE );
    }

    if ( f_pfPerform != NULL )
    {
        *f_pfPerform = FALSE;
    }
    else
    {
        ChkArg( !fCondition );
    }

    Oem_Clock_GetSystemTimeAsFileTime( f_pContextLicEval->pcontextBBX->pOEMContext, &ftCurrent );
    FILETIME_TO_UI64( ftCurrent, ui64Current );

    ChkDR( DRM_XMR_RequiresSST( pXMR, &fLicenseRequiresSST ) );
    fLicenseRequiresSST = fLicenseRequiresSST && !f_pContextLicEval->fLicenseFromPRND;  /* Do not check for "deleted" SST attribute on PRND licenses. It will never be set. */
    if ( f_pContextLicEval->pcontextSSTLicense != NULL && fLicenseRequiresSST )
    {
        ChkDR( DRM_SST_GetTokenValue( f_pContextLicEval->pcontextSSTLicense,
                                     &g_dstrDRM_LS_DELETED_ATTR,
                                     &token ) );

        /*
        ** Value should be 0.  If not fail because the license has been deleted before
        */
        ChkBOOL( token.val.lValue == 0, DRM_E_INVALID_LICENSE );
    }

    switch( f_eOperation )
    {
        case DRM_LICENSE_EVAL_SELECT:
        {
            ChkBOOL( !XMR_IS_RIGHTS_VALID( pXMR )
                  || !( ( pXMR->containerOuter.containerGlobalPolicies.Rights.wValue & XMR_RIGHTS_CANNOT_BIND_LICENSE ) == XMR_RIGHTS_CANNOT_BIND_LICENSE ),
                     DRM_E_XMR_LICENSE_NOT_BINDABLE );

            /*
            ** During select we check to see if the move list should be searched.
            */
            if( DRM_MOVE_IsMoveSupported() && XMR_IS_MOVEENABLER_VALID( pXMR ) )
            {
                f_pContextLicEval->fMoveListCheckRequired = TRUE;
            }

            if( XMR_IS_GLOBAL_POLICIES_VALID( pXMR ) )
            {
                if( XMR_HAS_TIME_BASED_RESTRICTIONS( pXMR ) )
                {
                    /*
                    **  If the license is timebound and flag is set to ignore time bound licenses
                    ** ( for e.g. if device clock is not set ), return Clock not set
                    */
                    if( ( ( f_pContextLicEval->pcontextSSTLicense == NULL ) && !fIgnoreSecureStoreEntry )
                     || ( f_pContextLicEval->fIgnoreTimeBoundLicense && XMR_IS_EXPIRATION_DATE_VALID( pXMR ) ) )
                    {
                        f_pContextLicEval->lReasonForFail = LR_LICENSE_CLOCK_NOT_SET;
                        ChkDR( DRM_E_CPRMEXP_CLOCK_REQUIRED );
                    }
                    if( f_pContextLicEval->eTimeBasedState != LICEVAL_GRACEPERIODREFERENCED )
                    {
                        f_pContextLicEval->eTimeBasedState = LICEVAL_MACHINEDATETIMEREFERENCED;
                    }
                }

                if( XMR_IS_EXPIRATION_AFTER_FIRST_USE_VALID( pXMR )
                 && f_pContextLicEval->pcontextSSTLicense != NULL )
                {
                    /* Ignore expire on first play for PRND leaf license and error out on root or simple license */
                    if( f_pContextLicEval->fLicenseFromPRND )
                    {
                        if( !XMR_IS_UPLINK_KID_VALID( pXMR ) )
                        {
                            ChkDR( DRM_E_LICEVAL_INVALID_PRND_LICENSE );
                        }
                    }
                    else
                    {
                        ZEROMEM( &token, SIZEOF( token ) );
                        ChkDR( DRM_SST_GetTokenValue( f_pContextLicEval->pcontextSSTLicense,
                                                     &g_dstrDRM_LS_FIRSTUSE_ATTR,
                                                     &token ) );
                        if( dr != DRM_S_FALSE )
                        {
                            ChkBOOL( token.TokenType == TOKEN_DATETIME, DRM_E_CPRMEXP_INVALID_TOKEN );

                            if ( !DRM_UI64Les( ui64Current, token.val.u64DateTime ) )
                            {
                                if( DRM_I64ToUI32( DRM_I64Div( DRM_UI2I64( DRM_UI64Sub( ui64Current, token.val.u64DateTime ) ), DRM_I64( C_TICS_PER_SECOND )) ) >= pXMR->containerOuter.containerGlobalPolicies.ExpirationAfterUse.dwValue )
                                {
                                    f_pContextLicEval->lReasonForFail = LR_LICENSE_EXPIRED;
                                    goto ErrorExit;
                                }
                                else if( XMR_IS_REAL_TIME_EXPIRATION_VALID( pXMR ) )
                                {
                                    DRM_UINT64 ui64rtexp = DRM_UI64Mul( DRM_UI64Add( DRM_UI64Div( ui64Current,
                                                                                                  DRM_UI64( C_TICS_PER_SECOND ) ),
                                                                                     DRM_UI64( pXMR->containerOuter.containerGlobalPolicies.ExpirationAfterUse.dwValue ) ),
                                                                        DRM_UI64( C_TICS_PER_SECOND ) );

                                    /* Save most restrictive ui64RealTimeExpiration */
                                    if( DRM_UI64Les( ui64Current, ui64rtexp )
                                     && DRM_UI64Les( ui64rtexp,   f_pContextLicEval->ui64RealTimeExpiration ) )
                                    {
                                        f_pContextLicEval->ui64RealTimeExpiration = ui64rtexp;
                                    }
                                }
                            }
                            else
                            {
                                f_pContextLicEval->lReasonForFail = LR_LICENSE_TIME_CHECK_FAILURE;
                                goto ErrorExit;
                            }
                        }
                    }
                }

                if( XMR_IS_EXPIRATION_AFTER_STORE_VALID( pXMR )
                 && f_pContextLicEval->pcontextSSTLicense != NULL )
                {
                    ZEROMEM( &token, SIZEOF( token ) );
                    ChkDR( DRM_SST_GetTokenValue( f_pContextLicEval->pcontextSSTLicense,
                                                 &g_dstrDRM_LS_FIRSTSTORE_ATTR,
                                                 &token ) );
                    if( dr == DRM_S_FALSE )
                    {
                        /*
                        ** Token not found isn't acceptable
                        */
                        ChkDR( DRM_E_CPRMEXP_RETRIEVAL_FAILURE );
                    }
                    else
                    {
                        ChkBOOL( token.TokenType == TOKEN_DATETIME, DRM_E_CPRMEXP_INVALID_TOKEN );

                        if ( !DRM_UI64Les( ui64Current, token.val.u64DateTime ) )
                        {
                            if( DRM_I64ToUI32( DRM_I64Div( DRM_UI2I64( DRM_UI64Sub( ui64Current, token.val.u64DateTime ) ), DRM_I64( C_TICS_PER_SECOND )) ) >= pXMR->containerOuter.containerGlobalPolicies.ExpirationAfterStore.dwValue )
                            {
                                f_pContextLicEval->lReasonForFail = LR_LICENSE_EXPIRED;
                                goto ErrorExit;
                            }
                        }
                        else
                        {
                            f_pContextLicEval->lReasonForFail = LR_LICENSE_TIME_CHECK_FAILURE;
                            goto ErrorExit;
                        }
                    }
                }

                if( XMR_IS_EXPIRATION_VALID( pXMR ) )
                {
                    if( pXMR->containerOuter.containerGlobalPolicies.Expiration.dwBeginDate != 0 )
                    {
                        CREATE_FILE_TIME( pXMR->containerOuter.containerGlobalPolicies.Expiration.dwBeginDate, ftXMR );
                        FILETIME_TO_UI64( ftXMR, ui64XMR );

                        if( !( DRM_UI64Les( ui64XMR, ui64Current )
                            || DRM_UI64Eql( ui64XMR, ui64Current ) ) )
                        {
                            f_pContextLicEval->lReasonForFail = LR_LICENSE_NOTENABLED;
                            goto ErrorExit;
                        }
                    }
                    if( pXMR->containerOuter.containerGlobalPolicies.Expiration.dwEndDate != XMR_UNLIMITED )
                    {
                        CREATE_FILE_TIME( pXMR->containerOuter.containerGlobalPolicies.Expiration.dwEndDate, ftXMR );
                        FILETIME_TO_UI64( ftXMR, ui64XMR );

                        if( !DRM_UI64Les( ui64Current, ui64XMR ) )
                        {
                            f_pContextLicEval->lReasonForFail = LR_LICENSE_EXPIRED;
                            goto ErrorExit;
                        }
                        else if( XMR_IS_REAL_TIME_EXPIRATION_VALID( pXMR ) )
                        {
                            /* Save most restrictive ui64RealTimeExpiration */
                            if( DRM_UI64Les( ui64XMR, f_pContextLicEval->ui64RealTimeExpiration ) )
                            {
                                f_pContextLicEval->ui64RealTimeExpiration = ui64XMR;
                            }
                        }
                    }
                }

                fDisabled = FALSE;
                ChkDR( DRM_ARCLK_LEVL_DisableOnRollback( f_pContextLicEval, pXMR, &fDisabled ) );
                if( fDisabled )
                {
                    /*
                    ** f_pContextLicEval->lReasonForFail is set by DRM_ARCLK_LEVL_DisableOnRollback
                    */
                    goto ErrorExit;
                }

                {
                    DRM_BOOL fGracePeriodExceeded = FALSE;
                    ChkDR( DRM_LEVL_CheckGracePeriodXMR(
                        f_pContextLicEval,
                        ui64Current,
                        pXMR,
                        &fGracePeriodExceeded ) );
                    if( fGracePeriodExceeded )
                    {
                        TRACE(("Grace period of %d seconds exceeded.", pXMR->containerOuter.containerGlobalPolicies.GracePeriod.dwValue));
                        f_pContextLicEval->lReasonForFail = LR_LICENSE_TIME_CHECK_FAILURE;
                        goto ErrorExit;
                    }
                }

                if( XMR_IS_MINIMUM_ENVIRONMENT_VALID( pXMR ) )
                {
                    if( pXMR->containerOuter.containerGlobalPolicies.MinimumEnvironment.wMinimumSecurityLevel > f_pContextLicEval->certinfoSDK.appSec
                     || ( ( f_pContextLicEval->fLegacyAppInfoValid
                         && pXMR->dwVersion == XMR_VERSION_1
                         && pXMR->containerOuter.containerGlobalPolicies.MinimumEnvironment.wMinimumSecurityLevel > f_pContextLicEval->certinfoAppLegacy.appSec )
                       || ( f_pContextLicEval->fAppInfoValid
                         && pXMR->dwVersion != XMR_VERSION_1
                         && pXMR->containerOuter.containerGlobalPolicies.MinimumEnvironment.wMinimumSecurityLevel > f_pContextLicEval->certinfoApp.appSec ) ) )
                    {
                        f_pContextLicEval->lReasonForFail = LR_LICENSE_APPSECLOW;
                        goto ErrorExit;
                    }

                    if( DRM_APPREVOCATION_IsAppRevocationSupported() )
                    {
                        if (pXMR->dwVersion == XMR_VERSION_1)
                        {
                            if( pXMR->containerOuter.containerGlobalPolicies.MinimumEnvironment.dwMinimumAppRevocationListVersion > f_pContextLicEval->idCRLsCurrent.wmdrmapp)
                            {
                                f_pContextLicEval->lReasonForFail = LR_LICENSE_CONTENT_REVOKED;
                                goto ErrorExit;
                            }
                        }
                        else
                        {
                            /*
                             * For V2+ XMR, we do not support the minimum environment objects, so validate
                             * that the versions in this object are zero if the object is valid.
                             * Note that it can be valid since the minimum security level is still
                             * honored.
                            */
                            ChkBOOL( pXMR->containerOuter.containerGlobalPolicies.MinimumEnvironment.dwMinimumAppRevocationListVersion == 0,
                                DRM_E_INVALID_LICENSE);

                            ChkBOOL( pXMR->containerOuter.containerGlobalPolicies.MinimumEnvironment.dwMinimumDeviceRevocationListVersion == 0,
                                DRM_E_INVALID_LICENSE);
                        }
                    }
                }

                if( XMR_IS_DOMAIN_ID_VALID( pXMR ) && f_pContextLicEval->fLicenseFromPRND )
                {
                    ChkDR( DRM_E_LICEVAL_INVALID_PRND_LICENSE );
                }
            }
        }
        break;

        case DRM_LICENSE_EVAL_DELETE:

            /*
            ** Update secure store to add Deleted to the entry
            */
            AssertChkArg( !f_pContextLicEval->fLicenseFromPRND );   /* Invalid to call for PRND licenses. */
            token.val.lValue = 1;
            token.TokenType  = TOKEN_LONG;
            ChkDR( DRM_SST_SetTokenValue( f_pContextLicEval->pcontextSSTLicense, &g_dstrDRM_LS_DELETED_ATTR, &token ) );

            break;
        case DRM_LICENSE_EVAL_STORE:
            /*
            ** Return a store error if we have a valid yet non-bindable license.
            */
            AssertChkArg( !f_pContextLicEval->fLicenseFromPRND );   /* Invalid to call for PRND licenses. */
            wTemp = pXMR->containerOuter.containerGlobalPolicies.Rights.wValue;

            fReturnStoreError =
                 XMR_IS_RIGHTS_VALID( pXMR )
              && ( ( wTemp & XMR_RIGHTS_CANNOT_BIND_LICENSE ) == XMR_RIGHTS_CANNOT_BIND_LICENSE );

            ChkBOOL( !fReturnStoreError, DRM_E_FAILED_TO_STORE_LICENSE );

            if( XMR_IS_EXPIRATION_VALID( pXMR ) )
            {
                if( pXMR->containerOuter.containerGlobalPolicies.Expiration.dwEndDate != XMR_UNLIMITED )
                {
                    CREATE_FILE_TIME( pXMR->containerOuter.containerGlobalPolicies.Expiration.dwEndDate, ftXMR );
                    FILETIME_TO_UI64( ftXMR, ui64XMR );

                    if( !DRM_UI64Les( ui64Current, ui64XMR ) )
                    {
                        f_pContextLicEval->lReasonForFail = LR_LICENSE_EXPIRED;
                        goto ErrorExit;
                    }
                }
            }

            ChkDR( DRM_ARCLK_LEVL_SaveLicenseStoreTime( f_pContextLicEval, pXMR ) );

            if( XMR_IS_EXPIRATION_AFTER_STORE_VALID( pXMR ) )
            {
                ZEROMEM( &token, SIZEOF( token ) );
                FILETIME_TO_UI64( ftCurrent, token.val.u64DateTime );
                token.TokenType = TOKEN_DATETIME;
                ChkDR( DRM_SST_SetTokenValue( f_pContextLicEval->pcontextSSTLicense,
                                             &g_dstrDRM_LS_FIRSTSTORE_ATTR,
                                             &token ) );
            }

            if( XMR_IS_PLAY_VALID( pXMR ) )
            {
                if( XMR_IS_PLAYCOUNT_VALID( pXMR ) )
                {
                    ZEROMEM( &token, SIZEOF( token ) );
                    token.TokenType  = TOKEN_LONG;
                    token.val.lValue = (DRM_LONG)pXMR->containerOuter.containerPlaybackPolicies.PlayCount.dwValue;
                    ChkDR( DRM_SST_SetTokenValue( f_pContextLicEval->pcontextSSTLicense,
                                                 &g_dstrDRM_LS_PLAYCOUNT_ATTR,
                                                 &token ) );
                }
            }

            if( XMR_IS_COPY_VALID( pXMR ) )
            {
                if( XMR_IS_COPYCOUNT_VALID( pXMR ) )
                {
                    ZEROMEM( &token, SIZEOF( token ) );
                    token.TokenType  = TOKEN_LONG;
                    token.val.lValue = (DRM_LONG)pXMR->containerOuter.containerCopyPolicies.CopyCount.dwValue;
                    ChkDR( DRM_SST_SetTokenValue( f_pContextLicEval->pcontextSSTLicense,
                                                 &g_dstrDRM_LS_COPYCOUNT_ATTR,
                                                 &token ) );
                }
            }

            if( pXMR->containerOuter.containerUnknown.fValid )
            {
                const DRM_XMR_UNKNOWN_CONTAINER *pContainer = &pXMR->containerOuter.containerUnknown;

                while( pContainer != NULL
                    && pContainer->fValid )
                {
                    ChkDR( DRM_APPP_AllowExecutePolicy( &pXMR->containerOuter.containerGlobalPolicies,
                                                        pContainer,
                                                        f_pContextLicEval ) );

                    pContainer = pContainer->pNext;
                }
            }

            if( XMR_IS_REMOVAL_DATE_VALID( pXMR ) )
            {
                /*
                ** If the removal date is valid then no secure restrictions are allowed.
                */
                if ( fLicenseRequiresSST )
                {
                    f_pContextLicEval->lReasonForFail = LR_LICENSE_STORE_NOT_ALLOWED;
                    goto ErrorExit;
                }
            }

            break;

        case DRM_LICENSE_EVAL_REPORTCLOCK_ROLLBACK:
            ChkArg( DRM_ARCLK_IsAntirollbackClockSupported() );
            AssertChkArg( !f_pContextLicEval->fLicenseFromPRND );   /* Invalid to call for PRND licenses. */

            ChkArg( !fCondition );

            ChkDR( DRM_ARCLK_LEVL_ReportClockRollback( f_pContextLicEval, pXMR ) );

            break;

        case DRM_LICENSE_EVAL_ACTION:
            {
                DRM_WORD wRightID = 0;
                DRM_GUID dguidEmpty = EMPTY_DRM_GUID;

                ChkArg( NULL != f_pdguidAction );

                if( fCondition
                 && XMR_IS_RIGHTS_VALID( pXMR )
                 && ( ( pXMR->containerOuter.containerGlobalPolicies.Rights.wValue & XMR_RIGHTS_CANNOT_PERSIST ) == XMR_RIGHTS_CANNOT_PERSIST )
                 && ( f_pContextLicEval->fLicenseFromHDS ) )
                {
                    /*
                    ** A cannot persist license should not have been read from persisted storage.
                    */
                    goto ErrorExit;
                }

                /*
                **  Check for Allowed SourceIDs
                */
                if ( ( !IDENTICAL_GUIDS( f_pdguidAction, &dguidEmpty ) || (0 < f_dwRightID))
                  && XMR_IS_RESTRICTED_SOURCEID_VALID( pXMR )
                  && !_IsAllowedSourceID( f_pContextLicEval ) )
                {
                    fRestricted = TRUE;
                }

                /*
                ** First assume that the action exists
                */
                if( f_pfActionExisted != NULL )
                {
                    *f_pfActionExisted = TRUE;
                }

                if( IDENTICAL_GUIDS( f_pdguidAction, &DRM_ACTION_PLAY ) )
                {
                    wRightID = XMR_OBJECT_TYPE_PLAYBACK_POLICY_CONTAINER;

                    if( XMR_IS_PLAY_VALID( f_pContextLicEval->plicenseXMR ) )
                    {
                        if( XMR_IS_PLAYCOUNT_VALID( f_pContextLicEval->plicenseXMR ) )
                        {
                            dwTemp = f_pContextLicEval->plicenseXMR->containerOuter.containerPlaybackPolicies.PlayCount.dwValue;
                            pdstrCountString = &g_dstrDRM_LS_PLAYCOUNT_ATTR;
                        }

                        /*
                        ** We will be trampling this, make sure it hasn't already been set
                        */
                        DRMASSERT( pExtendedRestrictions == NULL );
                        pExtendedRestrictions = pXMR->containerOuter.containerPlaybackPolicies.pUnknownObjects;

                        /*
                        ** Check if we need to verify the Action Qualifier GUID as well
                        */
                        if ( NULL != f_pdguidActionQualifier )
                        {
                            DRM_BOOL    fFound = FALSE;
                            const DRM_XMR_UNKNOWN_CONTAINER *pEnablerContainerFound = NULL;

                            /*
                            ** This will check the first container; if not found - will go depth-first into other containers
                            */
                            ChkDR( DRM_LEVL_SearchQualifierGuidInEnablerContainer(
                                                            f_pdguidActionQualifier,
                                                           &pXMR->containerOuter.containerPlaybackPolicies.UnknownContainer,
                                                            XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER,
                                                            XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT,
                                                           &fFound,
                                                           &pEnablerContainerFound ) );

                            /*
                            ** Enabler GUID not found in the license? Just fail.
                            */
                            if ( !fFound )
                            {
                                goto ErrorExit;
                            }

                            /*
                            ** See if we have extended restrictions in enabler container
                            */
                            DRMASSERT( pExtendedRestrictionsFromEnabler == NULL );
                            pExtendedRestrictionsFromEnabler = pEnablerContainerFound->pObject;
                        }
                        else if ( pXMR->containerOuter.containerPlaybackPolicies.UnknownContainer.fValid &&
                                  f_pfnPolicyCallback != NULL )
                        {
                            DRM_BOOL fFound = FALSE;
                            DRM_BOOL fSuccess = FALSE;

                            ChkDR( _DRM_LEVL_PerformCallbackInEnablerContainer( f_pContextLicEval,
                                                                               wRightID,
                                                                              &pXMR->containerOuter.containerPlaybackPolicies.UnknownContainer,
                                                                               XMR_OBJECT_TYPE_PLAY_ENABLER_CONTAINER,
                                                                               XMR_OBJECT_TYPE_PLAY_ENABLER_OBJECT,
                                                                               f_pfnPolicyCallback,
                                                                               f_pv,
                                                                               &fFound,
                                                                               &fSuccess ) );
                            if ( fFound && !fSuccess )
                            {
                                /*
                                ** Found at least one matching enabler and the aggregated callback result is a failure,
                                ** just fail.
                                */
                                goto ErrorExit;
                            }
                        }
                    }
                    else
                    {
                        /*
                        ** Right doesn't exist.  Just fail
                        */
                        goto ErrorExit;
                    }
                }
                else if( IDENTICAL_GUIDS( f_pdguidAction, &DRM_ACTION_COLLABORATIVE_PLAY ) )
                {
                    /*
                    ** It's possible that XMR_RIGHTS_COLLABORATIVE_PLAY right doesn't exist.
                    ** However, PDRM licenses that supported PLAY always supported
                    ** COLLABORATIVE_PLAY too. So if this license supports play AND it was
                    ** converted from PDRM, then we allow use of the basic PLAY right.
                    ** Note: We can NOT simply change the V1->V3 conversion logic to add the
                    ** COLLABORATIVE_PLAY rights to PDRM licenses because we've already shipped
                    ** code that does this conversion without adding that right.
                    ** Without this evaluation code handling this situation, people will lose
                    **  their COLLABORATIVE_PLAY rights on those licenses.
                    */

                    PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_REDUNDANTTEST, "Ignore prefast warning about redundant checks.")
                    if( ( XMR_IS_RIGHTS_VALID( pXMR ) && ( XMR_RIGHTS_COLLABORATIVE_PLAY == ( XMR_RIGHTS_COLLABORATIVE_PLAY & pXMR->containerOuter.containerGlobalPolicies.Rights.wValue ) ) )
                     || ( XMR_IS_PLAY_VALID( pXMR ) && XMR_IS_SOURCEID_VALID( pXMR ) && pXMR->containerOuter.containerGlobalPolicies.SourceID.dwValue == XMR_SOURCEID_PDRM ) )
                    {
                        if( XMR_IS_PLAYCOUNT_VALID( pXMR ) )
                        {
                            dwTemp = pXMR->containerOuter.containerPlaybackPolicies.PlayCount.dwValue;
                            pdstrCountString = &g_dstrDRM_LS_PLAYCOUNT_ATTR;
                        }
                    }
                    else
                    {
                        /*
                        ** Right doesn't exist.  Just fail
                        */
                        goto ErrorExit;
                    }
                    PREFAST_POP
                }
                else if( IDENTICAL_GUIDS( f_pdguidAction, &DRM_ACTION_COPY ) )
                {
                    DRM_BOOL fDomainAllowsAction    = FALSE;

                    /*
                    ** Domain only allows Copy To PC and Copy To Device
                    ** If caller didn't ask for a specific Copy right,
                    **  we also allow it here.
                    */
                    DRM_BOOL fDomainCanAllowAction  =
                        ( ( f_pdguidActionQualifier == NULL )
                         || IDENTICAL_GUIDS( f_pdguidActionQualifier, &DRM_ACTION_COPY_TO_PC )
                         || IDENTICAL_GUIDS( f_pdguidActionQualifier, &DRM_ACTION_COPY_TO_DEVICE ) );

                    /*
                    ** If the license is domain bound, then copy is simply allowed
                    ** It is the caller's responsibility to verify that the target
                    **  is joined to the domain, and we hand back the domain
                    **  information to allow them to make that check
                    */
                    if( XMR_IS_DOMAIN_ID_VALID( pXMR ) )
                    {
                        if( fDomainCanAllowAction )
                        {
                            /*
                            ** Give the license's domain ID back to the caller
                            */
                            const DRM_XMR_DOMAIN_ID *pLicenseDomainId =
                                &pXMR->containerOuter.containerGlobalPolicies.DomainID;

                            fDomainAllowsAction = TRUE;

                            if( pLicenseDomainId->cbAccountID == SIZEOF( DRM_GUID ) )
                            {
                                f_pContextLicEval->fCopyDestDomainAccountIDRequired = TRUE;
                                DRM_BYT_CopyBytes(
                                    &f_pContextLicEval->guidCopyDestDomainAccountIDRequired,
                                    0,
                                    pLicenseDomainId->pbAccountID,
                                    pLicenseDomainId->ibAccountID,
                                    pLicenseDomainId->cbAccountID );
                                f_pContextLicEval->dwCopyDestDomainRevisionRequired = pLicenseDomainId->dwRevision;
                            }
                        }
                    }

                    if( !fDomainAllowsAction )
                    {
                        /*
                        ** Use the copy right, if present
                        */

                        wRightID = XMR_OBJECT_TYPE_COPY_POLICY_CONTAINER;

                        /*
                        ** Check for Copy ( to device ) rights
                        */
                        if( XMR_IS_COPY_VALID( pXMR ) )
                        {
                            /*
                            ** Check the counts
                            */
                            if( XMR_IS_COPYCOUNT_VALID( pXMR ) )
                            {
                                if( pXMR->containerOuter.containerCopyPolicies.CopyCount.dwValue == 0 )
                                {
                                    /*
                                    ** Regardless of the move enabler or the current count, if the original count (which is in the license)
                                    ** is 0 then copy is never allowed, and checking current counts (whether local or on a service) shouldn't be
                                    ** attempted.  This is a valid license configuration for move scenarios.  Just fail.
                                    */
                                    goto ErrorExit;
                                }
                                if( DRM_MOVE_IsMoveSupported() && XMR_IS_MOVEENABLER_VALID( pXMR ) )
                                {
                                    /*
                                    ** If the move enabler exists the caller needs to enforce
                                    ** the move enabler security level.  In this case counts
                                    ** are not local, but enforced by the server.
                                    */
                                }
                                else
                                {
                                    dwTemp = pXMR->containerOuter.containerCopyPolicies.CopyCount.dwValue;
                                    pdstrCountString = &g_dstrDRM_LS_COPYCOUNT_ATTR;
                                }
                            }

                            /*
                            ** We will be trampling this, make sure it hasn't already been set
                            */
                            DRMASSERT( pExtendedRestrictions == NULL );
                            pExtendedRestrictions = pXMR->containerOuter.containerCopyPolicies.pUnknownObjects;

                            /*
                            ** Check if we need to verify the Action Qualifier GUID as well
                            */
                            if( NULL != f_pdguidActionQualifier )
                            {
                                DRM_BOOL    fFound = FALSE;
                                const DRM_XMR_UNKNOWN_CONTAINER *pEnablerContainerFound = NULL;

                                /*
                                ** This will check the first container; if not found - will go depth-first into other containers
                                */
                                ChkDR( DRM_LEVL_SearchQualifierGuidInEnablerContainer(
                                                                f_pdguidActionQualifier,
                                                               &pXMR->containerOuter.containerCopyPolicies.UnknownContainer,
                                                                XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER,
                                                                XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT,
                                                               &fFound,
                                                               &pEnablerContainerFound ) );

                                /*
                                ** Enabler GUID not found in the license? Just fail.
                                */
                                if ( !fFound )
                                {
                                    goto ErrorExit;
                                }

                                /*
                                ** See if we have extended restrictions in enabler container
                                */
                                DRMASSERT( pExtendedRestrictionsFromEnabler == NULL );
                                pExtendedRestrictionsFromEnabler = pEnablerContainerFound->pObject;

                            }
                            else if ( pXMR->containerOuter.containerCopyPolicies.UnknownContainer.fValid &&
                                      f_pfnPolicyCallback != NULL )
                            {
                                DRM_BOOL fFound = FALSE;
                                DRM_BOOL fSuccess = FALSE;

                                ChkDR( _DRM_LEVL_PerformCallbackInEnablerContainer( f_pContextLicEval,
                                                                                   wRightID,
                                                                                  &pXMR->containerOuter.containerCopyPolicies.UnknownContainer,
                                                                                   XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER,
                                                                                   XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT,
                                                                                   f_pfnPolicyCallback,
                                                                                   f_pv,
                                                                                   &fFound,
                                                                                   &fSuccess ) );

                                if ( fFound && !fSuccess )
                                {
                                    /*
                                    ** Found at least one matching enabler and the aggregated callback result is a failure,
                                    ** just fail.
                                    */
                                    goto ErrorExit;
                                }
                            }
                        }
                        else
                        {
                            /*
                            ** Copy right not available.
                            ** If this is a LEAF license, that might be OK
                            **  if (and only if) the root is domain-bound
                            **  AND the domain can allow the copy action.
                            ** See comments regarding this flag in drmliceval.h
                            **  for more information.
                            */
                            if( XMR_IS_UPLINK_KID_VALID( pXMR )
                             && fDomainCanAllowAction )
                            {
                                f_pContextLicEval->fDomainBoundRootRequired = TRUE;
                            }
                            else
                            {
                                /*
                                ** Right doesn't exist.  Just fail.
                                */
                                goto ErrorExit;
                            }
                        }
                    }
                }
                else if( DRM_MOVE_IsMoveSupported() && IDENTICAL_GUIDS( f_pdguidAction, &DRM_ACTION_MOVE ) )
                {
                    if( !XMR_IS_MOVEENABLER_VALID( pXMR ) )
                    {
                        /*
                        **  The move enabler doesn't exist.  Just fail.
                        */
                        goto ErrorExit;
                    }

                    /*
                    ** We will be trampling this, make sure it hasn't already been set
                    */
                    DRMASSERT( pExtendedRestrictions == NULL );
                    pExtendedRestrictions = pXMR->containerOuter.containerCopyPolicies.pUnknownObjects;

                    /*
                    ** Check if we need to verify the Action Qualifier GUID as well
                    ** For move we search the copy enabler container.
                    */
                    if( NULL != f_pdguidActionQualifier )
                    {
                        DRM_BOOL                   fFound = FALSE;
                        const DRM_XMR_UNKNOWN_CONTAINER *pEnablerContainerFound = NULL;

                        /*
                        ** This will check the first container; if not found - will go depth-first into other containers
                        */
                        ChkDR( DRM_LEVL_SearchQualifierGuidInEnablerContainer(
                                                        f_pdguidActionQualifier,
                                                       &pXMR->containerOuter.containerCopyPolicies.UnknownContainer,
                                                        XMR_OBJECT_TYPE_COPY_ENABLER_CONTAINER,
                                                        XMR_OBJECT_TYPE_COPY_ENABLER_OBJECT,
                                                       &fFound,
                                                       &pEnablerContainerFound ) );

                        /*
                        ** Enabler GUID not found in the license? Just fail.
                        */
                        if ( !fFound )
                        {
                            goto ErrorExit;
                        }

                        /*
                        ** See if we have extended restrictions in enabler container
                        */
                        DRMASSERT( pExtendedRestrictionsFromEnabler == NULL );
                        pExtendedRestrictionsFromEnabler = pEnablerContainerFound->pObject;
                    }
                    else
                    {
                        /*
                        ** Enabler GUID not provided. Just fail.
                        */
                        goto ErrorExit;
                    }
                }
                else if( DRM_THUMBNAIL_IsThumbnailSupported()
                      && IDENTICAL_GUIDS( f_pdguidAction, &DRM_ACTION_CREATE_THUMBNAIL ) )
                {
                    if( XMR_IS_THUMBNAIL_VALID( pXMR ) )
                    {
                        dwTemp           = XMR_UNLIMITED;
                        pdstrCountString = &g_dstrDRM_LS_PLAYCOUNT_ATTR;
                    }
                    else
                    {
                        /*
                        ** Right doesn't exist.  Just fail
                        */
                        goto ErrorExit;
                    }
                }
                else
                {
                    if( pXMR->containerOuter.containerUnknown.fValid )
                    {
                        /* Check if the Right ID they gave is a 16-bit number; if so, it would be the
                        ** object type of a container that may be an extensible right
                        */
                        if( f_dwRightID > 0 && f_dwRightID <= MAX_UNSIGNED_TYPE( DRM_WORD ) )
                        {
                            DRM_XMR_UNKNOWN_CONTAINER *pUnknownContainer = &pXMR->containerOuter.containerUnknown;

                            /*
                            ** Seems to be a valid object type
                            */

                            while( pUnknownContainer != NULL
                                && pUnknownContainer->fValid )
                            {
                                if( pUnknownContainer->wType == f_dwRightID )
                                {
                                    /*
                                    ** We found a match
                                    */
                                    DRMASSERT( pExtendedRestrictions == NULL );
                                    pExtendedRight = pUnknownContainer;
                                    pExtendedRestrictions = pExtendedRight->pObject;
                                    break;
                                }

                                pUnknownContainer = pUnknownContainer->pNext;
                            }
                        }

                        wRightID = (DRM_WORD)f_dwRightID;
                    }

                    /*
                    ** If no extended right was found then we couldn't find it
                    */
                    if( pExtendedRight == NULL )
                    {
                        if( f_pfActionExisted != NULL )
                        {
                            /*
                            **  Extensible Right was not found.
                            */
                            f_pContextLicEval->lReasonForFail = LR_LICENSE_EXTENSIBLE_RIGHT_NOT_FOUND;
                            *f_pfActionExisted = FALSE;
                        }
                        dr = DRM_SUCCESS;
                        goto ErrorExit;
                    }
                }

                if ( fRestricted )
                {
                    /*
                    ** Either a caller provided a callback function to tell whether this restricted SourceID is allowed
                    ** or license with this SourceID is not usable.
                    */
                    DRM_RESTRICTED_SOURCEID_CALLBACK_STRUCT oCallback = {0};
                    DRM_RESULT drTemp = DRM_SUCCESS;
                    if( f_pfnPolicyCallback == NULL )
                    {
                        f_pContextLicEval->lReasonForFail = LR_LICENSE_RESTRICTED_SOURCE;
                        goto ErrorExit;
                    }

                    oCallback.dwSourceID = f_pContextLicEval->plicenseXMR->containerOuter.containerGlobalPolicies.SourceID.dwValue;
                    drTemp = f_pfnPolicyCallback( &oCallback, DRM_RESTRICTED_SOURCEID_CALLBACK, f_pv );
                    if ( DRM_FAILED( drTemp ) )
                    {
                        TRACE(("Source with ID 0x%X is not allowed by a caller. Callback: 0x%X, error: 0x%X", oCallback.dwSourceID, f_pfnPolicyCallback, drTemp ));
                        f_pContextLicEval->lReasonForFail = LR_LICENSE_RESTRICTED_SOURCE;
                        goto ErrorExit;
                    }
                }

                /*
                ** It was a right we understand and it existed.  Check or update counts
                */
                if( fCondition )
                {
                    if( dwTemp != XMR_UNLIMITED )
                    {
                        dr = _CheckCount( f_pContextLicEval->pcontextSSTLicense,
                                          dwTemp,
                                          pdstrCountString );
                        if( dr == DRM_E_LICENSE_EXPIRED )
                        {
                              if( XMR_IS_DOMAIN_ID_VALID(pXMR)
                                  && DRM_UTL_DSTRStringsEqual(pdstrCountString, &g_dstrDRM_LS_COPYCOUNT_ATTR ) )
                              {
                                  /*
                                  ** If we're attempting copy, and we're all out of copy counts,
                                  ** and the license is domain bound, we could avoid using Copy by joining the device to the domain
                                  */
                                  ChkDR(  DRM_E_DEVICE_DOMAIN_JOIN_REQUIRED );
                              }
                              else
                              {

                                  f_pContextLicEval->lReasonForFail = LR_LICENSE_EXPIRED;
                                  dr = DRM_SUCCESS;
                                  goto ErrorExit;
                              }
                        }
                        ChkDR( dr );
                    }

                    if ( pExtendedRestrictions != NULL )
                    {
                        ChkDR( _InvokeExtensibleRestrictionsCallbacks( f_pContextLicEval,
                                                                       pExtendedRestrictions,
                                                                       wRightID,
                                                                       f_pfnPolicyCallback,
                                                                       f_pv ) );
                    }

                    if ( pExtendedRestrictionsFromEnabler != NULL )
                    {
                        ChkDR( _InvokeExtensibleRestrictionsCallbacks( f_pContextLicEval,
                                                                       pExtendedRestrictionsFromEnabler,
                                                                       wRightID,
                                                                       f_pfnPolicyCallback,
                                                                       f_pv ) );
                    }
                }
                else
                {
                    if( dwTemp != XMR_UNLIMITED )
                    {
                        ChkDR( _DecrementCount( f_pContextLicEval->pcontextSSTLicense,
                                                pdstrCountString ) );
                    }

                    /*
                    ** Update expire after first use
                    */
                    if( ( ( pXMR->dwVersion == XMR_VERSION_1 )
                       || IDENTICAL_GUIDS( f_pdguidAction, &DRM_ACTION_PLAY ) )
                     && XMR_IS_EXPIRATION_AFTER_FIRST_USE_VALID( pXMR ) )
                    {
                        /* Ignore expire on first play for PRND leaf license and error out on root or simple license */
                        if( f_pContextLicEval->fLicenseFromPRND )
                        {
                            if( !XMR_IS_UPLINK_KID_VALID( pXMR ) )
                            {
                                ChkDR( DRM_E_LICEVAL_INVALID_PRND_LICENSE );
                            }
                        }
                        else
                        {
                            ChkDR( DRM_SST_GetTokenValue( f_pContextLicEval->pcontextSSTLicense,
                                                         &g_dstrDRM_LS_FIRSTUSE_ATTR,
                                                         &token ) );
                            if( dr == DRM_S_FALSE )
                            {
                                ZEROMEM( &token, SIZEOF( token ) );
                                FILETIME_TO_UI64( ftCurrent, token.val.u64DateTime );
                                token.TokenType = TOKEN_DATETIME;
                                ChkDR( DRM_SST_SetTokenValue( f_pContextLicEval->pcontextSSTLicense,
                                                             &g_dstrDRM_LS_FIRSTUSE_ATTR,
                                                             &token ) );
                            }
                        }
                    }

                    if( pExtendedRestrictions != NULL
                     && f_pfnPolicyCallback != NULL )
                    {
                        DRM_XMR_UNKNOWN_OBJECT *pExtRestrictLoop = pExtendedRestrictions;

                        while( pExtRestrictLoop != NULL
                            && pExtRestrictLoop->fValid )
                        {
                            if( ( pExtRestrictLoop->wFlags & XMR_FLAGS_ALLOW_EXTERNAL_PARSE ) != 0 )
                            {
                                DRM_EXTENDED_RESTRICTION_CALLBACK_STRUCT oCallback = {0};

                                /*
                                ** This is likley an extensible rights container.
                                ** Need to pass it up to the app for them to parse.
                                */
                                oCallback.wRightID = wRightID;
                                oCallback.pRestriction = pExtRestrictLoop;
                                oCallback.pXMRLicense = f_pContextLicEval->plicenseXMR;
                                oCallback.pContextSST = f_pContextLicEval->pcontextSSTLicense;

                                dr = f_pfnPolicyCallback( &oCallback, DRM_EXTENDED_RESTRICTION_ACTION_CALLBACK, f_pv );
                                if( dr == DRM_E_EXTENDED_RESTRICTION_NOT_UNDERSTOOD || dr == DRM_E_NOTIMPL )
                                {
                                    if( ( pExtRestrictLoop->wFlags & XMR_FLAGS_MUST_UNDERSTAND ) != 0 )
                                    {
                                        TRACE(("Extensible restrction marked as must understand, but couldn't. Callback: 0x%X", f_pfnPolicyCallback ));
                                        ChkDR( DRM_E_EXTENDED_RESTRICTION_NOT_UNDERSTOOD );
                                    }
                                    else
                                    {
                                        /*
                                        ** Callback didn't understand the restriction, but they didn't have to.
                                        */
                                        ChkDR( DRM_SUCCESS );
                                    }
                                }
                            }
                            pExtRestrictLoop = pExtRestrictLoop->pNext;
                        }
                    }
                }
            }
            break;
        default:
            ChkDR( DRM_E_INVALIDARG );
    }


    if( f_pfPerform != NULL )
    {
        *f_pfPerform = TRUE;
    }

    dr = DRM_SUCCESS;

ErrorExit:
    if ( dr == DRM_S_FALSE )
    {
        dr = DRM_SUCCESS;
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/******************************************************************************
**
** Function: DRM_LEVL_PerformOperations
**
** Synopsis: Do an operation on a license
**
** Arguments:
**      [f_pcontextLicEval] : Specifies the liceval context to use
**      [f_eOperation]      : Specifies the operation to be perfomed
**      [f_eOperationState] :
**      [f_pdstrAction]     : Specifies action data for DRM_LICENSE_EVAL_ACTION operation
**      [f_pfPerform]       : Returns whether the action has been performed
**      [f_pfActionExisted] : Returns whether the action existed
**      [f_pDatastore]      : Specifies the DST context
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or improperly initialized
**      LICEVAL_LICENSE_NOT_SUPPLIED
**          The license content string is NULL
**      LICEVAL_INVALID_LICENSE
**          The specified license is invalid
**      LICEVAL_LICENSE_REVOKED
**          The specified license has been revoked
**      LICEVAL_KID_MISMATCH
**          KID mismatch with header
**      DRM_E_CH_UNABLE_TO_VERIFY
**          Unable to verify the signature of the content header
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LEVL_PerformOperations(
    __inout         DRM_LICEVAL_CONTEXT          *f_pContextLicEval,
    __in            eDRM_LICEVAL_OPERATIONS       f_eOperation,
    __in            eDRM_LICEVAL_OPERATION_STATE  f_eOperationState,
    __in_opt  const DRM_CONST_STRING             *f_pdstrAction,     /* Only required if DRM_LICENSE_EVAL_ACTION is passed as eOperation */
    __out_opt       DRM_BOOL                     *f_pfPerform,
    __out_opt       DRM_BOOL                     *f_pfActionExisted,
    __in      const DRM_DST                      *f_pDatastore,
    __in_opt        DRMPFNPOLICYCALLBACK          f_pfnPolicyCallback,
    __in_opt  const DRM_VOID                     *f_pv )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT  dr = DRM_SUCCESS;
    DRM_BOOL    fPerformActionEval = FALSE;

    ChkArg( f_pContextLicEval != NULL );

    if( DRM_LICENSE_EVAL_ACTION == f_eOperation )
    {
        fPerformActionEval = TRUE;
    }

    if( fPerformActionEval )
    {
        ChkArg( f_pdstrAction != NULL );

        if( DRM_UTL_DSTRStringsEqual( f_pdstrAction, &g_dstrWMDRM_RIGHT_NONE ) )
        {
            /*
            **  If the action is none, we always want to allow the action as long as
            **  a license exists
            */
            ChkArg( (f_pfPerform != NULL) && (f_pfActionExisted != NULL) );
            *f_pfPerform       = TRUE;
            *f_pfActionExisted = TRUE;
            goto ErrorExit;
        }
    }

    if ( f_pContextLicEval->fLicenseIsXMR )
    {
        DRM_GUID        dguidAction = EMPTY_DRM_GUID;
        DRM_DWORD       dwRightID = 0;

        if ( fPerformActionEval )
        {
            /*
            ** Convert Action string to Action GUID, using lookup table;
            ** extract Right ID from Action string if there is one
            */
            ChkDR( _ConvertActionStringToActionGUIDAndRightID( f_pdstrAction, &dguidAction, &dwRightID ) );
        }

        ChkDR( DRM_LEVL_PerformOperationsXMR( f_pContextLicEval,
                                      f_eOperation,
                                      f_eOperationState,
                                      fPerformActionEval ? &dguidAction : NULL,
                                      NULL,         /* f_pdguidActionQualifier */
                                      dwRightID,
                                      f_pfPerform,
                                      f_pfActionExisted,
                                      f_pDatastore,
                                      f_pfnPolicyCallback,
                                      f_pv ) );
    }
    else
    {
        f_pContextLicEval->ui64RealTimeExpiration = DRM_UI64HL(MAX_UNSIGNED_TYPE(DRM_DWORD),MAX_UNSIGNED_TYPE(DRM_DWORD));
        ChkDR( DRM_WMDRM_LEVL_PerformOperationsXML( f_pContextLicEval,
                                                    f_eOperation,
                                                    f_eOperationState,
                                                    f_pdstrAction,
                                                    f_pfPerform,
                                                    f_pfActionExisted,
                                                    f_pDatastore ) );
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function: DRM_LEVL_IsLicenseReadyForDeletion
**
** Synopsis: Checks if this license has expired and is ready to be deleted
**
** Arguments:
**      [f_pcontextLicEval] : Specifies the liceval context to check
**      [f_pfDelete]        : Returns whether or not the license can be deleted
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL or improperly initialized
**
** Notes:
**      For now this function is mostly a CUT&PASTE
**      job from DRM_LEVL_PerformOperations, with a lot of
**      duplicate code. The common sections should be factored
**      out into a separate subroutine
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LEVL_IsLicenseReadyForDeletion(
    __inout DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __out   DRM_BOOL            *f_pfDelete )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT        dr         = DRM_SUCCESS;
    DRM_BOOL          fResult    = FALSE;

    /*
    ** The other pointers in the context structure *may* not be
    ** needed so don't fail just yet for those
    */
    ChkArg( f_pContextLicEval              != NULL );
    ChkArg( f_pfDelete                     != NULL );
    ChkArg( f_pContextLicEval->pcontextBBX != NULL );

    /*
    **  Clear output
    */
    *f_pfDelete = FALSE;
    f_pContextLicEval->fDeleteLicense = FALSE;

    MEMCPY( &f_pContextLicEval->contextEXPR.KID,
            &f_pContextLicEval->KID,
             SIZEOF( DRM_KID ) );
    MEMCPY( &f_pContextLicEval->contextEXPR.LID,
            &f_pContextLicEval->LID,
             SIZEOF( DRM_LID ) );

    /*
    ** Is license selectable
    */
    ChkDR( DRM_LEVL_PerformOperations( f_pContextLicEval,
                                       DRM_LICENSE_EVAL_SELECT,
                                       DRM_LICENSE_EVAL_CAN_DO_OPERATION,
                                       NULL,
                                      &fResult,
                                       NULL,
                                       f_pContextLicEval->pDatastore,
                                       NULL,
                                       NULL ) );
    if( !fResult )
    {
        /*
        **  We need to make sure that the reason for failure is set correctly.
        **  This is a workaround to compensate for the fact that v9
        **  licenses do not set the reason correctly sometimes.
        */
        if( f_pContextLicEval->lReasonForFail == 0 )
        {
            /* No reason given by license. Try parsing to get the reason, if we can. */
            (void) DRM_WMDRM_LEVL_GetLicenseReasonForUnusable( f_pContextLicEval,
                                                              &f_pContextLicEval->lReasonForFail );
        }
        /*
        ** If the reason for license not usable is because license expired, then
        ** set *pfDelete
        */
        if( f_pContextLicEval->lReasonForFail == LR_LICENSE_EXPIRED
         || f_pContextLicEval->fDeleteLicense )
        {
            /*
            ** License cannot be selected because it is expired
            */
            *f_pfDelete = TRUE;
            goto ErrorExit;
        }
    }

    if ( f_pContextLicEval->fLicenseIsXMR )
    {
        DRM_DWORD i = 0;
        const DRM_CONST_STRING *rgpdstrKnownRights[] = {
            &g_dstrWMDRM_RIGHT_PLAYBACK,
            &g_dstrWMDRM_RIGHT_COLLABORATIVE_PLAY,
            &g_dstrWMDRM_RIGHT_COPY };

        DRM_BOOL rgbSupportedRights[] = {
            TRUE,
            TRUE,
            TRUE };

        if( f_pContextLicEval->plicenseXMR->containerOuter.containerUnknown.fValid )
        {
            /*
            ** There is at least one extensible right in the license, so we can't safely delete it
            */
            goto ErrorExit;
        }

        for( i = 0; i < NO_OF( rgpdstrKnownRights );i++ )
        {
            if( rgbSupportedRights[i] )
            {
                dr = DRM_LEVL_PerformOperations( f_pContextLicEval,
                                                 DRM_LICENSE_EVAL_ACTION,
                                                 DRM_LICENSE_EVAL_CAN_DO_OPERATION,
                                                 rgpdstrKnownRights[i],
                                                &fResult,
                                                 NULL,
                                                 f_pContextLicEval->pDatastore,
                                                 NULL,
                                                 NULL );

                /*
                ** Since we do not have a policy callback function, if the license has a must understand
                ** extensible restriction, DRM_LEVL_PerformOperations will return a
                ** DRM_E_EXTENDED_RESTRICTION_NOT_UNDERSTOOD error.  Without the policy callback function,
                ** there is no way to understand the restriction so we can't delete the license but
                ** we also do not want to return an error.
                */
                if ( dr == DRM_E_EXTENDED_RESTRICTION_NOT_UNDERSTOOD )
                {
                    /*
                    ** There is at least one restriction that isn't understood, so can't delete the license.
                    ** Reset the status to success and set fResult to TRUE so that the license will not
                    ** be deleted.
                    */
                    dr = DRM_SUCCESS;
                    fResult = TRUE;
                }

                ChkDR( dr );

                if( fResult
                 || ( f_pContextLicEval->lReasonForFail != LR_LICENSE_EXPIRED && !f_pContextLicEval->fDeleteLicense ) )
                {
                    /*
                    ** At least one right isn't expired, so can't delete the license
                    */
                    goto ErrorExit;
                }
            }
        }

        /*
        ** No non-expired rights exist in the license, so it's safe to delete it
        */
        *f_pfDelete = TRUE;
    }

ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function: DRM_LEVL_GetMachineDatetime
**
** Synopsis: Get the "machine.datetime" for the clock
**           If anti-rollback clock is supported, this includes
**           using the value in the Secure Store - see
**           DRM_ARCLK_LEVL_OverrideMachineDateTimeWithSavedValue for details
**
** Arguments:
**      [f_pcontextLicEval]      : Specifies the liceval context to use
**      [f_pui64MachineDateTime] : Returns the machine date/time
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the parameters is NULL
**      DRM_E_CPRMEXP_CLOCK_REQUIRED
**          f_pContextLicEval->fIgnoreTimeBoundLicense is set
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_LEVL_GetMachineDatetime(
    __inout     DRM_LICEVAL_CONTEXT *f_pContextLicEval,
    __out       DRM_UINT64          *f_pui64MachineDateTime )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT  dr          = DRM_SUCCESS;
    DRMFILETIME filetime    = {0};

    ChkArg( f_pContextLicEval      != NULL );
    ChkArg( f_pui64MachineDateTime != NULL );

    if( f_pContextLicEval->fIgnoreTimeBoundLicense )
    {
        f_pContextLicEval->lReasonForFail = LR_LICENSE_CLOCK_NOT_SET;
        ChkDR( DRM_E_CPRMEXP_CLOCK_REQUIRED );
    }

    if( f_pContextLicEval->eTimeBasedState != LICEVAL_GRACEPERIODREFERENCED )
    {
        f_pContextLicEval->eTimeBasedState = LICEVAL_MACHINEDATETIMEREFERENCED;
    }

    Oem_Clock_GetSystemTimeAsFileTime( f_pContextLicEval->pcontextBBX->pOEMContext, &filetime );
    FILETIME_TO_UI64( filetime, *f_pui64MachineDateTime );

    ChkDR( DRM_ARCLK_LEVL_OverrideMachineDateTimeWithSavedValue( f_pContextLicEval, f_pui64MachineDateTime ) );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

