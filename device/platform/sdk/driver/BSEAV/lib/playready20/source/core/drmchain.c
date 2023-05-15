/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmrevocation.h>
#include <drmchain.h>
#include <drmsecureclock.h>
#include <drmxmlparser.h>
#include <drmlicreason.h>
#include <drmdatastore.h>
#include <drmantirollbackclock.h>
#include <drmsymopt.h>
#include <drmcontentrevocation.h>
#include <drmcopyopl.h>
#include <drmdeletedstore.h>
#include <drmxmlhash.h>
#include <drmwmdrmnet.h>
#include <drmapprevocation.h>
#include <drmdevicerevocation.h>
#include <drmapppolicy.h>
#include <drmmetering.h>
#include <drmwmdrm.h>
#include <drmsyncstore.h>

ENTER_PK_NAMESPACE_CODE;

/******************************************************************************
**
** Function :   _PerformActions
**
** Synopsis :   The license is selectable -- Can I perform the actions on it?
**              Loop through all the request actions and ask if we can do them
**              If we can do them all the report them all
**
** Arguments :
**          [f_pFFLicense]              : Find first license structure
**          [f_pstack]                  : Stack allocator for temporary allocations
**          [f_rgpdstrRights]           : Array of rights strings to query for
**          [f_cRights]                 : Count of rights in f_rgpdstrRights
**          [f_pDatastore]              : Specifies the DST
**          [f_pfnPolicyCallback]       : Callback function for output restrictions
**          [f_pv]                      : Void pass-through parameter for callback
**
** Returns :
**          DRM_SUCCESS
**              Success
**          DRM_E_INVALIDARG
**              A required parameter was NULL or improperly initialized
**          DRM_E_RIGHTSNOTAVAILABLE
**              A selected right was not found
**
******************************************************************************/
static DRM_RESULT _PerformActions(
    IN OUT   DRM_FFLICENSE               *f_pFFLicense,
    IN OUT   DRM_STACK_ALLOCATOR_CONTEXT *f_pstack,
    IN const DRM_CONST_STRING            *f_rgpdstrRights[], /* Array of DRM_CONST_STRING pointers */
    IN       DRM_DWORD                    f_cRights,
    IN       DRM_DST                     *f_pDatastore,
    IN       DRMPFNPOLICYCALLBACK         f_pfnPolicyCallback,
    IN const DRM_VOID                    *f_pv )
{
    DRM_RESULT       dr          = DRM_SUCCESS;
    DRM_DWORD        iRight      = f_cRights;
    DRM_BOOL         fExisted    = FALSE;
    DRM_BOOL         fEvalResult = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMCHAIN, PERF_FUNC__PerformActions );

    ChkArg( f_pFFLicense != NULL );
    ChkArg( f_pDatastore != NULL );
    f_pFFLicense->fDomainBoundRootRequired = FALSE;

    for( ; iRight > 0; )
    {
        fExisted    = FALSE;
        fEvalResult = FALSE;

        iRight--;
        ChkDR( DRM_LEVL_PerformOperations( f_pFFLicense->pLicEval,
                                           DRM_LICENSE_EVAL_ACTION,
                                           DRM_LICENSE_EVAL_CAN_DO_OPERATION,
                                           f_rgpdstrRights[iRight],
                                          &fEvalResult,
                                          &fExisted,
                                           f_pDatastore,
                                           f_pfnPolicyCallback,
                                           f_pv ) );

        ChkBOOL( fEvalResult, DRM_E_RIGHTSNOTAVAILABLE );

        f_pFFLicense->fDomainBoundRootRequired =
            f_pFFLicense->fDomainBoundRootRequired
         || f_pFFLicense->pLicEval->fDomainBoundRootRequired;

        /*
        ** Check if there are any output levels on this right
        */
        if ( fExisted )
        {
            /*
            ** XMR License
            */
            if ( f_pFFLicense->pLicEval->fLicenseIsXMR )
            {
                /*
                ** Play right
                */
                if ( DRM_UTL_DSTRStringsEqual( f_rgpdstrRights[iRight],
                                             &g_dstrWMDRM_RIGHT_PLAYBACK )
                 && XMR_IS_PLAY_VALID( f_pFFLicense->pLicEval->plicenseXMR ) )
                {
                    /*
                    ** No need to obtain default OPL data if no callback function was supplied
                    */
                    if ( f_pfnPolicyCallback == NULL )
                    {
                        /*
                        ** There were output restrictions and no callback function was supplied
                        */
                        ChkBOOL( !XMR_IS_OPL_VALID( f_pFFLicense->pLicEval->plicenseXMR )
                              && !XMR_IS_EXPLICITANALOGVIDEOPROTECTION_VALID( f_pFFLicense->pLicEval->plicenseXMR ),
                                 DRM_E_NO_OPL_CALLBACK );
                    }
                    /*
                    ** Obtain OPL data - will obtain default data if license has no OPLs
                    */
                    else if ( XMR_IS_OPL_VALID(f_pFFLicense->pLicEval->plicenseXMR)
                           || XMR_IS_EXPLICITANALOGVIDEOPROTECTION_VALID(f_pFFLicense->pLicEval->plicenseXMR)
                           || XMR_IS_EXPLICITDIGITALAUDIOPROTECTION_VALID(f_pFFLicense->pLicEval->plicenseXMR) )
                    {
                        f_pstack->pbStack = f_pFFLicense->rgPlayOpl[f_pFFLicense->dwChainDepth].rgbPlayOPLBuffer;
                        f_pstack->cbStack = SIZEOF( f_pFFLicense->rgPlayOpl[f_pFFLicense->dwChainDepth].rgbPlayOPLBuffer );

                        ChkDR( DRM_LICOPL_ProcessPlayOutputLevelDataXMR( &f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerPlaybackPolicies,
                                                                         &f_pFFLicense->rgPlayOpl[f_pFFLicense->dwChainDepth].oplPlay,
                                                                          f_pstack ) );
                        f_pFFLicense->rgfPlayOplValid[f_pFFLicense->dwChainDepth] = TRUE;
                    }
                }
                /*
                ** Copy right
                */
                else if ( DRM_UTL_DSTRStringsEqual( f_rgpdstrRights[iRight], &g_dstrWMDRM_RIGHT_COPY )
                      && XMR_IS_COPY_VALID( f_pFFLicense->pLicEval->plicenseXMR ) )
                {
                    /*
                    ** No need to obtain default OPL data if no callback function was supplied
                    */
                    if ( f_pfnPolicyCallback == NULL )
                    {
                        /*
                        ** There were output restrictions and no callback function was supplied
                        */
                        ChkBOOL( !XMR_IS_COPYPROTECTIONLEVEL_VALID( f_pFFLicense->pLicEval->plicenseXMR ),
                                 DRM_E_NO_OPL_CALLBACK );
                    }
                    /*
                    ** Obtain OPL data - will obtain default data if license has no OPLs
                    */
                    else if( XMR_IS_COPYPROTECTIONLEVEL_VALID(f_pFFLicense->pLicEval->plicenseXMR) )
                    {
                        if( DRM_COPYOPL_IsCopyOplSupported() )
                        {
                            f_pstack->pbStack = f_pFFLicense->rgCopyOpl[f_pFFLicense->dwChainDepth].rgbCopyOPLBuffer;
                            f_pstack->cbStack = SIZEOF( f_pFFLicense->rgCopyOpl[f_pFFLicense->dwChainDepth].rgbCopyOPLBuffer );

                            ChkDR( DRM_COPYOPL_ProcessCopyOutputLevelDataXMR(
                                &f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerCopyPolicies,
                                &f_pFFLicense->rgCopyOpl[f_pFFLicense->dwChainDepth].oplCopy,
                                f_pstack ) );
                            f_pFFLicense->rgfCopyOplValid[f_pFFLicense->dwChainDepth] = TRUE;
                        }
                        else
                        {
                            /*
                            ** We don't support COPY OPLs but the license has COPY OPLs
                            */
                            ChkDR( DRM_E_RIGHTSNOTAVAILABLE );
                        }
                    }
                }
            }
            /*
            ** If we got this far, then this is an XML license with OPLs
            */
            else
            {
                ChkDR( DRM_WMDRM_LIC_PerformActionsOPLCallbacks(
                    f_pFFLicense,
                    f_pstack,
                    f_rgpdstrRights,
                    iRight,
                    f_pfnPolicyCallback ) );
            }
        }
    } /* end for rights */

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
**
** Function :   _ValidateRevocationVersions
**
** Synopsis :   This will check the revocation list versions in the license versus the current
**              list versions to ensure they are adequate.  The current version numbers should
**              be pre-cached for this check
**
** Parameters:
**      [f_pFFLicense] : Specifies the find first license structure to use
**      [f_pDatastore] : Specifies the data store.
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument was NULL or invalid
**      DRM_E_LICEVAL_REQUIRED_REVOCATION_LIST_NOT_AVAILABLE
**          Unable to retrieve the revocation list
**      DRM_E_RIV_TOO_SMALL
**          The Riv was retrieved but was too small
**
******************************************************************************/
static DRM_RESULT _ValidateRevocationVersions(
    IN OUT DRM_FFLICENSE    *f_pFFLicense,
    IN     DRM_DST          *f_pDatastore )
{
    DRM_RESULT       dr                    = DRM_SUCCESS;
    DRM_DWORD        idCRLWMDRMAppRequired = 0;
    DRM_DWORD        idCRLDeviceRequired   = 0;
    DRM_DWORD        idCRLWMDRMNETRequired = 0;
    DRM_DWORD        dwRevInfo             = 0;
    DRM_DWORD        dwXMRVersion          = XMR_VERSION_INVALID;
    DRM_BOOL         fUseV2RIV             = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMCHAIN, PERF_FUNC__ValidateRevocationVersions );

    ChkArg( f_pFFLicense           != NULL );
    ChkArg( f_pFFLicense->pLicEval != NULL );
    ChkArg( f_pDatastore           != NULL );

    /*
    ** Extract required versions from an XML license
    */
    if ( !f_pFFLicense->pLicEval->fLicenseIsXMR )
    {
        ChkDR( DRM_WMDRM_LIC_ExtractRevocationVersions(
            f_pFFLicense,
            &idCRLWMDRMAppRequired,
            &idCRLDeviceRequired,
            &idCRLWMDRMNETRequired,
            &dwRevInfo ) );
    }

    /*
    ** Extract required versions from an XMR license
    */
    else
    {
        ChkArg( f_pFFLicense->pLicEval->plicenseXMR != NULL );

        dwXMRVersion = f_pFFLicense->pLicEval->plicenseXMR->dwVersion;

        ChkBOOL(
            dwXMRVersion == XMR_VERSION_1
         || dwXMRVersion == XMR_VERSION_3, DRM_E_INVALIDLICENSE);

        if ( XMR_IS_MINIMUM_ENVIRONMENT_VALID( f_pFFLicense->pLicEval->plicenseXMR ) )
        {
                if (dwXMRVersion == XMR_VERSION_1)
                {
                    idCRLWMDRMAppRequired  = f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerGlobalPolicies.MinimumEnvironment.dwMinimumAppRevocationListVersion;
                    idCRLDeviceRequired = f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerGlobalPolicies.MinimumEnvironment.dwMinimumDeviceRevocationListVersion;
                }
                else
                {
                    /*
                     * For V2+ XMR, we do not support the minimum environment objects, so validate
                     * that the versions in this object are zero if the object is valid.
                     * Note that it can be valid since the minimum security level is still
                     * honored.
                    */
                    ChkBOOL( f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerGlobalPolicies.MinimumEnvironment.dwMinimumAppRevocationListVersion == 0,
                        DRM_E_INVALIDLICENSE);

                    ChkBOOL( f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerGlobalPolicies.MinimumEnvironment.dwMinimumDeviceRevocationListVersion == 0,
                        DRM_E_INVALIDLICENSE);

                    /* Use the V2 revinfo */
                    fUseV2RIV = TRUE;
                }
        }

        if ( XMR_IS_REVOCATION_INFORMATION_VERSION_VALID( f_pFFLicense->pLicEval->plicenseXMR ) )
        {
            dwRevInfo = f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerGlobalPolicies.RevocationInformationVersion.dwValue;
        }
    }

    /*
    ** Compare the required values to the cached values
    */

    /*
    ** Although we're failing here, the caller should update
    ** the revocation version cache and try again. This is in
    ** case there have been any updates to the revocation lists.
    */
    ChkBOOL( idCRLWMDRMAppRequired <= f_pFFLicense->pLicEval->idCRLsCurrent.wmdrmapp
          && idCRLDeviceRequired   <= f_pFFLicense->pLicEval->idCRLsCurrent.device
          && idCRLWMDRMNETRequired <= f_pFFLicense->pLicEval->idCRLsCurrent.wmdrmnet,
             DRM_E_LICEVAL_REQUIRED_REVOCATION_LIST_NOT_AVAILABLE );

    /*
    ** Ensure the dwLicRevInfoVer field has the highest RIV needed so far. This
    ** value is returned to callers asking which RIV is required.
    */
    if ( dwRevInfo > f_pFFLicense->dwLicRevInfoVer )
    {
        f_pFFLicense->dwLicRevInfoVer = dwRevInfo;
    }

    /*
    ** Test the license RIV against the currently stored RIV. If the best CRL
    ** set we have is too old, return DRM_E_RIV_TOO_SMALL.
    */
    if ( fUseV2RIV )
    {
        ChkBOOL( dwRevInfo <= f_pFFLicense->pLicEval->idCRLsCurrent.riv2,
                 DRM_E_RIV_TOO_SMALL );
    }
    /*
    ** We only want to check the V1 RIV if we support one of the V1 revocation types
    */
    else if( DRM_REVOCATION_IsV1RIVSupported() )
    {
        ChkBOOL( dwRevInfo <= f_pFFLicense->pLicEval->idCRLsCurrent.riv,
                 DRM_E_RIV_TOO_SMALL );
    }

    dr = DRM_SUCCESS;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
**
** Function :   _GetLicenseInfoAndSetup
**
** Synopsis :   Load the license in memory and setup
**
** Parameters:
**      [f_pFFLicense]    : Specifies the find first license structure to use
**      [f_rgpdstrRights] : Specifies the list of rights
**      [f_cRights]       : Number of elements in f_rgpdstrRights
**      [f_pDatastore]    : Specifies the DST
**
** Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument was NULL or invalid
**      DRM_E_XMR_REQUIRED_OBJECT_MISSING
**          A required object was missing for an XMR license
**      DRM_E_INVALIDLICENSE
**          An error occurred while parsing the license
**      DRM_E_POLICY_METERING_DISABLED
**          There is a MID but metering is disabled
**
******************************************************************************/
static DRM_RESULT _GetLicenseInfoAndSetup(
    __in_opt DRM_VOID *f_pOEMContext,
    IN OUT   DRM_FFLICENSE    *f_pFFLicense,
    IN const DRM_CONST_STRING *f_rgpdstrRights[], /* Array of DRM_CONST_STRING pointers */
    IN       DRM_DWORD         f_cRights,
    IN       DRM_DST          *f_pDatastore )
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_DWORD  cbLicSize  = 0;
    DRM_BYTE  *pbLicense  = NULL;
    DRM_BYTE  *pbTemp     = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMCHAIN, PERF_FUNC__GetLicenseInfoAndSetup );

    ChkArg( f_pFFLicense != NULL );
    ChkArg( f_pDatastore != NULL );
    ChkArg( f_pFFLicense->pBindingInfo != NULL );

    ChkDR( DRM_STK_PreAlloc( f_pFFLicense->pstack, &cbLicSize, (DRM_VOID**)&pbLicense ) );
    ChkDR( DRM_LST_GetLicense( f_pFFLicense->rgpLicStoreCurrent[f_pFFLicense->dwChainDepth],
                              &f_pFFLicense->rgkid[f_pFFLicense->dwChainDepth],
                              &f_pFFLicense->rglid[f_pFFLicense->dwChainDepth],
                              &( f_pFFLicense->rgslotHint[f_pFFLicense->dwChainDepth] ),
                               pbLicense,
                              &cbLicSize ) );
    ChkDR( DRM_STK_Alloc( f_pFFLicense->pstack, cbLicSize, (DRM_VOID**)&pbTemp ) );
    DRMASSERT( pbTemp == pbLicense );

    /*
    ** XMR License
    */
    if ( f_pFFLicense->pBindingInfo[0].m_fXMRLicense )
    {
        f_pFFLicense->pLicenseXMR[f_pFFLicense->dwChainDepth].pbXMRLic = pbLicense;
        f_pFFLicense->pLicenseXMR[f_pFFLicense->dwChainDepth].cbXMRLic = cbLicSize;
        /*
        ** Unpack the license into the correct chained license slot
        */

        f_pFFLicense->pLicEval->plicenseXMR = &f_pFFLicense->pLicenseXMR[f_pFFLicense->dwChainDepth];

        ChkDR( DRM_XMR_UnpackLicense( pbLicense,
                                      cbLicSize,
                                      f_pFFLicense->pstack,
                                      f_pFFLicense->pLicEval->plicenseXMR ) );

        /*
        ** Ensure the minimum objects are in the license
        */

        PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_REDUNDANTTEST, "Ignore prefast warning about redundant checks.")

        ChkBOOL( XMR_IS_RID_VALID( f_pFFLicense->pLicEval->plicenseXMR )
              && XMR_IS_SIGNATURE_VALID  ( f_pFFLicense->pLicEval->plicenseXMR )
              && XMR_IS_CONTENT_KEY_VALID( f_pFFLicense->pLicEval->plicenseXMR ),
                 DRM_E_XMR_REQUIRED_OBJECT_MISSING );
        PREFAST_POP

        f_pFFLicense->pLicEval->fLicenseIsXMR = TRUE;

        if ( f_pFFLicense->fSkipAESContent
          && f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.ContentKey.wSymmetricCipherType == XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR )
        {
            ChkDR( DRM_E_LICENSEMISMATCH );   /* we are told to ignore this type of license. */
        }

        /*
        ** Parse the KID and LID from the license
        */
        DRM_BYT_CopyBytes( f_pFFLicense->pLicEval->LID.rgb,
                           0,
                           f_pFFLicense->pLicEval->plicenseXMR->pbRightsIdBuffer,
                           f_pFFLicense->pLicEval->plicenseXMR->iRightsId,
                           SIZEOF( f_pFFLicense->pLicEval->LID.rgb ) );
        DRM_BYT_CopyBytes( f_pFFLicense->pLicEval->KID.rgb,
                           0,
                           f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.ContentKey.pbguidKeyID,
                           f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.ContentKey.iguidKeyID,
                           SIZEOF( f_pFFLicense->pLicEval->KID.rgb ) );

        /*
        ** If we didn't get the RID and LID before, get it now
        */
        if ( f_pFFLicense->dwChainDepth == 0 )
        {
            DRM_BYT_CopyBytes( &f_pFFLicense->rglid[f_pFFLicense->dwChainDepth],
                               0,
                               f_pFFLicense->pLicEval->LID.rgb,
                               0,
                               SIZEOF( f_pFFLicense->pLicEval->LID.rgb ) );
            DRM_BYT_CopyBytes( &f_pFFLicense->rgkid[f_pFFLicense->dwChainDepth],
                               0,
                               f_pFFLicense->pLicEval->KID.rgb,
                               0,
                               SIZEOF( f_pFFLicense->pLicEval->KID.rgb ) );
            ZEROMEM( &f_pFFLicense->rgslotHint[f_pFFLicense->dwChainDepth], SIZEOF( DRM_DST_SLOT_HINT ) );
        }

        if ( XMR_IS_METERING_VALID( f_pFFLicense->pLicEval->plicenseXMR ) )
        {
            ChkBOOL( ( f_pFFLicense->dwPolicyFlags & DRM_POLICY_FLAG_METERING_DISABLED_ENTERPRISE ) == 0,
                     DRM_E_POLICY_METERING_DISABLED );

            if( DRM_METERING_IsMeteringSupported() )
            {
                f_pFFLicense->rgfHasMetering[f_pFFLicense->dwChainDepth] = TRUE;
                DRM_BYT_CopyBytes( &f_pFFLicense->rgmid[f_pFFLicense->dwChainDepth],
                                    0,
                                    f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerGlobalPolicies.Metering.pguidBuffer,
                                    f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerGlobalPolicies.Metering.iGuid,
                                    SIZEOF( DRM_GUID ) );
            }
        }

        /*
        ** Set up XMR binding context
        */
        f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_fXMRLicense                                 = TRUE;
        f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_BindingInfo.m_XMR.m_pxmrContentKey          = &f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.ContentKey;
        f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_BindingInfo.m_XMR.m_pxmrOptimizedContentKey = &f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.OptimizedContentKey;
        f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_BindingInfo.m_XMR.m_pxmrDeviceKey           = &f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.DeviceKey;
        f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_BindingInfo.m_XMR.m_pxmrDeviceEccKey        = &f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.ECCKey;
        f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_BindingInfo.m_XMR.m_pxmrDomainID            = &f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerGlobalPolicies.DomainID;
        f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_BindingInfo.m_XMR.m_pxmrSignature           = &f_pFFLicense->pLicEval->plicenseXMR->containerOuter.signature;
        f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_BindingInfo.m_XMR.m_pxmrLicense             =  f_pFFLicense->pLicEval->plicenseXMR;

        /*
        ** assign checksum from license to bindinginfo buffer
        */
        if ( f_pFFLicense->dwChainDepth != DRM_MAX_LICENSE_CHAIN_DEPTH - 1 )
        {
            if ( XMR_IS_UPLINK_KID_VALID( f_pFFLicense->pLicEval->plicenseXMR ) )
            {
                ChkBOOL( f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkKid.cbChainedCheckSum <= DRM_MAX_CHECKSUM,
                         DRM_E_BUFFERTOOSMALL );
                DRM_BYT_CopyBytes( f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth + 1].m_oChecksum.m_rgbChecksumData,
                                   0,
                                   f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkKid.pbChainedCheckSumBuffer,
                                   f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkKid.iChainedCheckSum,
                                   f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkKid.cbChainedCheckSum );
                f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth + 1].m_oChecksum.m_cbChecksum
                    = f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkKid.cbChainedCheckSum;
                f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth + 1].m_oChecksum.m_fIgnoreChecksum = FALSE;
                f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth + 1].m_oChecksum.m_pbV1KID = NULL;
                DRM_BYT_CopyBytes( f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth + 1].m_oChecksum.m_KID.rgb,
                                   0,
                                   f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkKid.pbguidUplinkKID,
                                   f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkKid.iguidUplinkKID,
                                   SIZEOF( DRM_KID ) );
            }
            else if ( XMR_IS_UPLINKX_VALID( f_pFFLicense->pLicEval->plicenseXMR ) )
            {
                ChkBOOL( f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkX.cbCheckSum <= DRM_MAX_CHECKSUM,
                         DRM_E_BUFFERTOOSMALL );
                DRM_BYT_CopyBytes( f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth + 1].m_oChecksum.m_rgbChecksumData,
                                   0,
                                   f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkX.pbCheckSumBuffer,
                                   f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkX.iCheckSum,
                                   f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkX.cbCheckSum );
                f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth + 1].m_oChecksum.m_cbChecksum
                    = f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkX.cbCheckSum;
                f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth + 1].m_oChecksum.m_fIgnoreChecksum = FALSE;
                f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth + 1].m_oChecksum.m_pbV1KID = NULL;

                /* Use leaf KID to verify checksum. */
                DRM_BYT_CopyBytes( f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth + 1].m_oChecksum.m_KID.rgb,
                                   0,
                                   f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.ContentKey.pbguidKeyID,
                                   f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.ContentKey.iguidKeyID,
                                   SIZEOF( DRM_KID ) );

                ChkDR( DRM_STK_Alloc( f_pFFLicense->pstack,
                                      f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkX.cEntries * DRM_AES_KEYSIZE_128,
                                      ( DRM_VOID ** )&f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkX.pbKey ) );
            }
        }
    }
    /*
    ** XML License
    */
    else
    {
        DSTR_FROM_PB( &( f_pFFLicense->pLicEval->dstrContentLicense ), pbLicense, cbLicSize );

        ChkDR( DRM_WMDRM_LIC_GetLicenseInfoAndSetup(
            f_pFFLicense,
            f_rgpdstrRights,
            f_cRights ) );
    }

    if ( f_pFFLicense->fSecStoreOpen[f_pFFLicense->dwChainDepth] )
    {
        f_pFFLicense->fSecStoreOpen[f_pFFLicense->dwChainDepth] = FALSE;
        ZEROMEM( &( f_pFFLicense->pSecStoreLicense[f_pFFLicense->dwChainDepth] ),
                  SIZEOF( DRM_SECSTORE_CONTEXT ) );
    }

    /*
    ** Never fail here for secure clock stuff
    ** DRM_LIC_LoadSecureTimeData returns DRM_VOID
    */
    DRM_LIC_LoadSecureTimeData( f_pOEMContext, f_pFFLicense, f_pDatastore );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

static DRM_RESULT _RemapEvaluationErrors( __in DRM_RESULT drOrig, __in DRM_RESULT drDefault )
{
    /*
    ** If the error code is any of the specific errors in this if block,
    ** let it be explicitly returned.
    */
    if (   drOrig == DRM_E_DOMAIN_BIND_LICENSE
        || drOrig == DRM_E_UNABLE_TO_RESOLVE_LOCATION_TREE
        || drOrig == DRM_E_UPLINKLICENSENOTFOUND )
    {
        return drOrig;
    }
    else
    {
        /* Otherwise, return the default (generic) error */
        return drDefault;
    }
}

/******************************************************************************
**
** Function :   _ProcessEndOfChain
**
** Synopsis :   Process end of the license chain and return
**
** Parameters:
**      [f_pFFLicense]        : Specifies the find first license structure to use
**      [f_pfnPolicyCallback] : Specifies the OPL callback
**      [f_pv]                : Opaque data to pass to f_pfnPolicyCallback
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument was NULL or invalid
**      DRM_E_LIC_KEY_DECODE_FAILURE
**          Unable to decode key data from the license
**      DRM_E_BUFFERTOOSMALL
**          The internal license buffer is too small
**
******************************************************************************/
static DRM_RESULT _ProcessEndOfChain(
    IN OUT   DRM_FFLICENSE               *f_pFFLicense,
    IN       DRMPFNPOLICYCALLBACK         f_pfnPolicyCallback,
    IN const DRM_VOID                    *f_pv )
{
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  i         = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMCHAIN, PERF_FUNC__ProcessEndOfChain );

    ChkArg( f_pFFLicense != NULL );
    ChkArg( f_pFFLicense->pLicEval != NULL );

    if ( !DRM_APPP_CanBind( f_pFFLicense->pLicEval->plicenseXMR,
                            f_pFFLicense->pBindingInfo,
                            f_pFFLicense->dwChainDepth + 1,
                            f_pFFLicense->pLicEval->pcontextBBX,
                            &dr ) )
    {
        ChkDR( _RemapEvaluationErrors( dr, DRM_E_LIC_KEY_DECODE_FAILURE ) );
    }

    if ( f_pfnPolicyCallback != NULL )
    {
        /*
        **  Perform an inclusion list callback, but only if at least one inclusion list is valid
        */
        for( i = 0; i < DRM_MAX_INCLUSION_GUIDS; i++ )
        {
            if( f_pFFLicense->inclusionListCallback.rgfInclusionListValid[i][f_pFFLicense->dwChainDepth] )
            {
                /*
                ** At least one inclusion list is valid, so we need to make the callback
                */
                f_pFFLicense->inclusionListCallback.dwChainDepth = f_pFFLicense->dwChainDepth;
                ChkDR( f_pfnPolicyCallback( &f_pFFLicense->inclusionListCallback, DRM_INCLUSION_LIST_CALLBACK, f_pv ) );
                break; /* make sure we only callback once */
            }
        }

        for( i = f_pFFLicense->dwChainDepth + 1; i > 0; )
        {
            /*
            ** Find and return the root most OPL for the license chain.
            */
            i--;

            if ( f_pFFLicense->rgfPlayOplValid[i] )
            {
                ChkDR( f_pfnPolicyCallback( &f_pFFLicense->rgPlayOpl[i].oplPlay, DRM_PLAY_OPL_CALLBACK, f_pv ) );
                break;
            }
        }

        if( DRM_COPYOPL_IsCopyOplSupported() )
        {
            for( i = f_pFFLicense->dwChainDepth + 1; i > 0; )
            {
                i--;
                /*
                ** Find and return the root most OPL for the license chain.
                */
                if ( f_pFFLicense->rgfCopyOplValid[i] )
                {
                    ChkDR( f_pfnPolicyCallback( &f_pFFLicense->rgCopyOpl[i].oplCopy, DRM_COPY_OPL_CALLBACK, f_pv ) );
                    break;
                }
            }
        }
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/*****************************************************************************
** Function: _GetSourceIDFromLicense
**
** Synopsis: Checks of the <SOURCEID> tag in the XML license and updates the
**           the dwSourceid in DRM_FFLICENSE if found.
**           Source ID should be specified only in leaf license. We will ignore
**           the sourceID if specified in the root license.
** Arguments:
**           IN f_pFFLicense   -- DRM_FFLICENSE the license
** Returns:
**           DRM_SUCCESS           -- On success
*****************************************************************************/
static DRM_RESULT _GetSourceIDFromLicense(
    IN DRM_FFLICENSE *f_pFFLicense )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pFFLicense != NULL );
    ChkArg( f_pFFLicense->pLicEval != NULL );
    ChkArg( f_pFFLicense->pLicEval->dstrContentLicense.pwszString != NULL
         || ( f_pFFLicense->pLicEval->fLicenseIsXMR
           && f_pFFLicense->pLicEval->plicenseXMR != NULL ) );

    f_pFFLicense->dwSourceid = 0;

    if ( f_pFFLicense->pLicEval->fLicenseIsXMR )
    {
        if ( f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerGlobalPolicies.SourceID.fValid )
        {
            f_pFFLicense->dwSourceid = f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerGlobalPolicies.SourceID.dwValue;
        }
    }
    else
    {
        ChkDR( DRM_WMDRM_LIC_GetSourceIDFromLicense( f_pFFLicense ) );
    }

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function : DRM_LIC_CompleteLicenseChain
**
** Synopsis : Complete a license chain and return
**
** Parameters:
**      [f_pFFLicense]              : Specifies the find first license structure to use
**      [f_rgpdstrRights]           : Specifies the set of rights strings
**      [f_cRights]                 : Specifies the number of elements in f_rgpdstrRights
**      [f_pdwChainDepth]           : Returns the current chain depth
**      [f_pDatastore]              : Specifies the primary Data Store (used for metering, Secure Store, etc.)
**      [f_pfnPolicyCallback]       : Specifies the OPL callback
**      [f_pv]                      : Opaque data to pass to f_pfnPolicyCallback
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument was NULL or invalid
**      DRM_E_RIGHTSNOTAVAILABLE
**          Unable to parse a right from the license
**      DRM_E_LICENSEEXPIRED
**          License is expired
**      DRM_E_V1_LICENSE_CHAIN_NOT_SUPPORTED
**          V1 content containing chained licenses is not supported
**      DRM_E_LICENSENOTFOUND
**          License not found at maximum chain depth
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_LIC_CompleteLicenseChain(
    __in_opt DRM_VOID *f_pOEMContext,
    IN OUT   DRM_FFLICENSE               *f_pFFLicense,
    IN const DRM_CONST_STRING            *f_rgpdstrRights[], /* Array of DRM_CONST_STRING pointers */
    IN       DRM_DWORD                    f_cRights,
       OUT   DRM_DWORD                   *f_pdwChainDepth,
    IN       DRM_DST                     *f_pDatastore,
    IN       DRMPFNPOLICYCALLBACK         f_pfnPolicyCallback,
    IN const DRM_VOID                    *f_pv )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_BOOL         fEvalResult      = FALSE;
    DRM_DWORD        i                = 0;
    DRM_KID          oChainKID        = { 0 };
    DRM_BOOL         fChained         = FALSE;
    DRM_STACK_ALLOCATOR_CONTEXT oStack = { 0, 0, NULL };
    DRM_DWORD        nStackTop        = ULONG_MAX;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMCHAIN, PERF_FUNC_DRM_LIC_CompleteLicenseChain );

    DRMCASSERT( DRM_MAX_LICENSE_CHAIN_DEPTH > 0 );

    ChkArg( f_pFFLicense                           != NULL );
    ChkArg( f_pFFLicense->rgpLicStoreCurrent       != NULL );
    ChkArg( f_pFFLicense->rgpLicStores             != NULL );
    ChkArg( f_pFFLicense->cLicStores                > 0 );
    ChkArg( f_pFFLicense->cLicStores               <= DRM_LICSTORE_MAX_STORES );
    ChkArg( f_pFFLicense->pbBuffer                 != NULL );
    ChkArg( f_pFFLicense->cbBuffer                  > 0 );
    ChkArg( f_pFFLicense->pBindingInfo             != NULL );
    ChkArg( f_pFFLicense->pLicStoreEnum            != NULL );
    ChkArg( f_pdwChainDepth                        != NULL );
    ChkArg( f_pDatastore                           != NULL );
    ChkArg( f_cRights                              <= DRM_MAX_ACTIONS );
    ChkArg( f_pFFLicense->pLicEval                 != NULL );
    ChkArg( f_pFFLicense->pLicEval->pcontextBBX    != NULL );

    f_pFFLicense->pLicEval->fDeleteLicense = FALSE;
    *f_pdwChainDepth = 0;

    ChkBOOL( f_pFFLicense->dwChainDepth < DRM_MAX_LICENSE_CHAIN_DEPTH, DRM_E_LIC_CHAIN_TOO_DEEP );

    if ( f_pFFLicense->dwChainDepth == 0 )
    {
        /*
        ** Only Re-init the stack on the first iteration
        */
        ZEROMEM( f_pFFLicense->pstack, SIZEOF( DRM_STACK_ALLOCATOR_CONTEXT ) );
        ChkDR( DRM_STK_Init( f_pFFLicense->pstack, f_pFFLicense->pbBuffer, f_pFFLicense->cbBuffer ) );

        for( i = 0; i < DRM_MAX_LICENSE_CHAIN_DEPTH; i++ )
        {
            f_pFFLicense->rgfCopyOplValid[i] = FALSE;
            f_pFFLicense->rgfPlayOplValid[i] = FALSE;
        }
        if( DRM_WMDRMNET_IsWmdrmnetSupported() )
        {
            f_pFFLicense->dwMinimumRequiredAppSec = 0;
        }
        f_pFFLicense->fBlockNDT = FALSE;
    }

    /* Cache the current stack top in case it needs to be explicitly restored. */
    nStackTop = f_pFFLicense->pstack->nStackTop;

    if( DRM_METERING_IsMeteringSupported() )
    {
        f_pFFLicense->rgfHasMetering [f_pFFLicense->dwChainDepth] = FALSE;
    }

    ChkDR( _GetLicenseInfoAndSetup( f_pOEMContext, f_pFFLicense, f_rgpdstrRights, f_cRights, f_pDatastore ) );

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        /*
        ** Compare license RIV and CRL versions,
        ** then return DRM_E_RIV_TOO_SMALL
        ** or DRM_E_LICEVAL_REQUIRED_REVOCATION_LIST_NOT_AVAILABLE error
        */
        ChkDR( _ValidateRevocationVersions( f_pFFLicense, f_pDatastore ) );
    }

    /*
    ** Check if the license has source id >0 and update the dwSourceid in
    ** DRM_FFLICENSE. The source ID should be specified only in leaf license.
    */
    if ( f_pFFLicense->dwChainDepth == 0 )
    {
        ChkDR( _GetSourceIDFromLicense( f_pFFLicense ) );
    }

    f_pFFLicense->pLicEval->dwFlags              = LICEVAL_OPEN_CHECK_SETTINGS;

    /*
    ** The PC is currently using this function to handle temp XMR licenses
    ** which can not be persisted.  Once that is no longer the case, we can remove
    ** all the code #if'd in by this #if.  PS #2762.
    */
    if( f_pFFLicense->pSecStoreLicense != NULL )
    {
        /*
        ** Talk to blackbox to get the secure store password for this license
        */
        ChkDR( DRM_SST_CreateLicenseStatePassword( &f_pFFLicense->pLicEval->LID,
                                                    f_pFFLicense->rgbLicensePassword,
                                                    f_pFFLicense->pLicEval->pcontextBBX ) );

        ChkDR( DRM_SST_OpenKeyTokens( &( f_pFFLicense->pSecStoreLicense[f_pFFLicense->dwChainDepth] ),
                                      &f_pFFLicense->pLicEval->LID,
                                       NULL,
                                       f_pFFLicense->rgbLicensePassword,
                                       0,
                                       SECURE_STORE_LICENSE_DATA,
                                       f_pDatastore ) );

        f_pFFLicense->fSecStoreOpen[f_pFFLicense->dwChainDepth] = TRUE;

        f_pFFLicense->pLicEval->pcontextSSTLicense      = &( f_pFFLicense->pSecStoreLicense[f_pFFLicense->dwChainDepth] );
    }
    else
    {
        f_pFFLicense->pLicEval->pcontextSSTLicense = NULL;
        f_pFFLicense->pLicEval->dwFlags |= LICEVAL_IGNORE_SECURESTORE_ENTRY;
    }

    f_pFFLicense->pLicEval->fGlobalSecStoreWritable = FALSE;

    f_pFFLicense->pLicEval->fReserved            = 0;
    if( DRM_CONTENTREVOCATION_IsContentRevocationSupported() )
    {
        f_pFFLicense->pLicEval->dwChainDepth         = f_pFFLicense->dwChainDepth;
    }
    f_pFFLicense->pLicEval->pDatastore           = f_pDatastore;
    f_pFFLicense->pLicEval->eTimeBasedState      = LICEVAL_NOREFERENECE;
    f_pFFLicense->pLicEval->fDeleteLicense       = FALSE;

    ChkDR( DRM_LEVL_PerformOperations( f_pFFLicense->pLicEval,
                                       DRM_LICENSE_EVAL_SELECT,
                                       DRM_LICENSE_EVAL_CAN_DO_OPERATION,
                                       NULL,
                                      &fEvalResult,
                                       NULL,
                                       f_pDatastore,
                                       f_pfnPolicyCallback,
                                       f_pv ) );
    if ( !fEvalResult )
    {
        /*
        **  We need to make sure that the reason for failure is set correctly.
        **  This is a workaround to compensate for the fact that v9
        **  licenses do not set the reason correctly sometimes.
        */
        if ( f_pFFLicense->pLicEval->lReasonForFail == 0 )
        {
            /*
            ** No reason given by license. Try parsing to get the reason, if we can.
            */
            ( void ) DRM_WMDRM_LEVL_GetLicenseReasonForUnusable( f_pFFLicense->pLicEval,
                                                                &f_pFFLicense->pLicEval->lReasonForFail );
        }
        dr = DRM_E_RIGHTSNOTAVAILABLE;
        /*
        ** If the reason for license not usable is because license expired
        ** OR if the license is "deleting" itself, then delete the license
        */
        if ( f_pFFLicense->pLicEval->lReasonForFail == LR_LICENSE_EXPIRED
          || f_pFFLicense->pLicEval->fDeleteLicense )
        {
            /*
            ** License has expired, delete it
            */
            ( void )DRM_LST_DeleteLicenseAndSaveDate( f_pFFLicense->rgpLicStoreCurrent[f_pFFLicense->dwChainDepth],
                                                     &f_pFFLicense->pLicEval->KID,
                                                     &f_pFFLicense->pLicEval->LID,
                                                      NULL,
                                                      DRM_DELETEDSTORE_IsDeletedStoreSupported());

            if( DRM_SNC_IsLicenseSyncSupported() )
            {
                ( void )DRM_SNC_UpdateKID( f_pFFLicense->pLicEval->pcontextSync,
                                           NULL,
                                          &f_pFFLicense->pLicEval->KID,
                                           NULL,
                                           DRM_SYNC_UPDATE_FORCE_SYNC );
            }

            /*
            ** If we're deleting the license, it more or less indicates that the license expired. Some
            ** licenses don't properly set the "reason for fail", so if they never set any reason code
            ** we should just message that it expired.
            */
            if ( f_pFFLicense->pLicEval->lReasonForFail == LR_LICENSE_EXPIRED
             || f_pFFLicense->pLicEval->lReasonForFail == 0 )
            {
                dr = DRM_E_LICENSEEXPIRED;
            }
        }
        goto ErrorExit;
    }

    ChkDR( DRM_LIC_CheckGracePeriod( f_pFFLicense ) );

    /*
    **  Get the inclusion list for this license
    */
    if ( f_pFFLicense->pLicEval->fLicenseIsXMR )
    {
        if ( f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerGlobalPolicies.InclusionList.fValid )
        {
            ChkDR( DRM_XMR_PopulateInclusionList( &f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerGlobalPolicies.InclusionList,
                                                   f_pFFLicense->inclusionListCallback.rgInclusionList,
                                                   f_pFFLicense->inclusionListCallback.rgfInclusionListValid,
                                                   f_pFFLicense->dwChainDepth ) );
        }

        if( XMR_IS_UPLINKX_VALID( f_pFFLicense->pLicEval->plicenseXMR )
         || XMR_IS_AUX_KEY_VALID( f_pFFLicense->pLicEval->plicenseXMR ) )
        {
            /*
            ** Scalable licenses should not be used for NDT.
            */
            f_pFFLicense->fBlockNDT = TRUE;
        }
    }
    else
    {
        ChkDR( DRM_WMDRM_LIC_GetInclusionList( &f_pFFLicense->pLicEval->dstrContentLicense,
                                                f_pFFLicense->inclusionListCallback.rgInclusionList,
                                                f_pFFLicense->inclusionListCallback.rgfInclusionListValid,
                                                f_pFFLicense->dwChainDepth ) );
    }

    dr = _PerformActions( f_pFFLicense,
                         &oStack,
                          f_rgpdstrRights,
                          f_cRights,
                          f_pDatastore,
                          f_pfnPolicyCallback,
                          f_pv );

    if( DRM_SNC_IsLicenseSyncSupported() && dr == DRM_E_LICENSEEXPIRED )
    {
        /* An extensible restriction callback may have given this error code to indicate
        ** that the right has expired and needs to be refreshed.
        */
        ( void )DRM_SNC_UpdateKID( f_pFFLicense->pLicEval->pcontextSync,
                                   NULL,
                                  &f_pFFLicense->pLicEval->KID,
                                   NULL,
                                   DRM_SYNC_UPDATE_FORCE_SYNC );
    }

    ChkDR( dr );

    if( DRM_WMDRMNET_IsWmdrmnetSupported() )
    {
        DRM_DWORD dwMinimumRequiredAppSec = 0;

        if ( f_pFFLicense->pLicEval->fLicenseIsXMR )
        {
            dwMinimumRequiredAppSec = f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerGlobalPolicies.MinimumEnvironment.wMinimumSecurityLevel;
        }
        else
        {
            ChkDR( DRM_WMDRM_LEVL_ParseV2License( &f_pFFLicense->pLicEval->dstrContentLicense,
                                                  &g_dstrDRM_LS_APPSEC_ATTR,
                                                  &dwMinimumRequiredAppSec,
                                                   NULL ) );
        }

        if ( dwMinimumRequiredAppSec > f_pFFLicense->dwMinimumRequiredAppSec )
        {
            f_pFFLicense->dwMinimumRequiredAppSec = dwMinimumRequiredAppSec;
        }
    }

    /*
    ** Get the KID for the chained license
    */
    if ( f_pFFLicense->pLicEval->fLicenseIsXMR
      && XMR_IS_UPLINK_KID_VALID( f_pFFLicense->pLicEval->plicenseXMR ) )
    {
        DRM_BYT_CopyBytes( oChainKID.rgb,
                           0,
                           f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkKid.pbguidUplinkKID,
                           f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkKid.iguidUplinkKID,
                           SIZEOF( oChainKID.rgb ) );
        fChained = TRUE;
    }
    else if ( f_pFFLicense->pLicEval->fLicenseIsXMR
      && XMR_IS_UPLINKX_VALID( f_pFFLicense->pLicEval->plicenseXMR ) )
    {
        DRM_BYT_CopyBytes( oChainKID.rgb,
                           0,
                           f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkX.pbguidUplinkKID,
                           f_pFFLicense->pLicEval->plicenseXMR->containerOuter.containerKeys.UplinkX.iguidUplinkKID,
                           SIZEOF( oChainKID.rgb ) );
        fChained = TRUE;
    }
    else if ( f_pFFLicense->pLicEval->rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_CHAINEDKID].cchString != 0 )
    {
        fChained = TRUE;
        ChkDR( DRM_UTL_DecodeKID( &( f_pFFLicense->pLicEval->rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_CHAINEDKID] ),
                                  &oChainKID ) );
    }

    /*
    ** We don't support chained licenses from V1 content
    */
    ChkBOOL( !fChained || f_pFFLicense->pV1Header == NULL, DRM_E_V1_LICENSE_CHAIN_NOT_SUPPORTED );

    if ( fChained )
    {
        DRM_DWORD drCached = DRM_E_UPLINKLICENSENOTFOUND;

        /*
        ** We're at the maximum chain depth that we support
        */
        ChkBOOL( f_pFFLicense->dwChainDepth + 1 < DRM_MAX_LICENSE_CHAIN_DEPTH, DRM_E_LICENSENOTFOUND );

        /*
        ** Open an enumerator for the KID, increment the chain depth and recurse
        ** Don't recurse if the dependent kid is a kid already in the chain
        */
        ChkDR( DRM_LST_InitEnumMultiple( f_pFFLicense->rgpLicStores,
                                         f_pFFLicense->cLicStores,
                                         &oChainKID,
                                         TRUE,
                                         &( f_pFFLicense->pLicStoreEnum[f_pFFLicense->dwChainDepth+1] ) ) );

        while( TRUE )
        {
            ChkBOOL( f_pFFLicense->dwChainDepth + 1 < DRM_MAX_LICENSE_CHAIN_DEPTH, DRM_E_LICENSENOTFOUND );

            dr = DRM_LST_EnumNextMultiple( &( f_pFFLicense->pLicStoreEnum[f_pFFLicense->dwChainDepth + 1] ),
                                           &f_pFFLicense->rgkid[f_pFFLicense->dwChainDepth + 1],
                                           &f_pFFLicense->rglid[f_pFFLicense->dwChainDepth + 1],
                                           &f_pFFLicense->rgpLicStoreCurrent[f_pFFLicense->dwChainDepth + 1],
                                           &( f_pFFLicense->rgslotHint[f_pFFLicense->dwChainDepth + 1] ),
                                            NULL );
            if ( dr == DRM_E_NOMORE )
            {
                /*
                ** If uplink KID was not found in the license store, make sure to set
                ** the proper chain depth and uplink KID anyway
                ** We also should add the uplink KID to the synclist to request a license the next sync.
                */
                if ( f_pFFLicense->pLicStoreEnum[f_pFFLicense->dwChainDepth + 1].eMode == eDRM_LICENSE_STORE_MODE_NONE )
                {
                    f_pFFLicense->dwChainDepth++;
                    DRM_BYT_CopyBytes( f_pFFLicense->rgkid[f_pFFLicense->dwChainDepth].rgb,
                                       0,
                                       oChainKID.rgb,
                                       0,
                                       SIZEOF( f_pFFLicense->rgkid[f_pFFLicense->dwChainDepth].rgb ) );

                    if( DRM_SNC_IsLicenseSyncSupported() )
                    {
                        /*
                        ** If there is no entry for the root KID in the sync store then we should add one.
                        ** Intenionally ignoring the errors here.
                        */
                        if ( DRM_E_SYNC_ENTRYNOTFOUND == DRM_SNC_GetSyncStoreEntry(
                                                             f_pFFLicense->pLicEval->pcontextSync,
                                                            &oChainKID,
                                                             NULL,
                                                             NULL,
                                                             NULL) )
                        {
                            ( void )DRM_SNC_UpdateKID( f_pFFLicense->pLicEval->pcontextSync,
                                                       NULL,
                                                      &oChainKID,
                                                       NULL,
                                                       DRM_SYNC_UPDATE_NO_LICENSE );
                        }
                    }
                }
                ChkDR( drCached );
            }

            ChkDR( dr );

            f_pFFLicense->dwChainDepth++;
            dr = DRM_LIC_CompleteLicenseChain( f_pOEMContext,
                                               f_pFFLicense,
                                               f_rgpdstrRights,
                                               f_cRights,
                                               f_pdwChainDepth,
                                               f_pDatastore,
                                               f_pfnPolicyCallback,
                                               f_pv );
            f_pFFLicense->dwChainDepth--;

            if( DRM_SUCCEEDED( dr ) )
            {
                goto ErrorExit;
            }
            drCached = _RemapEvaluationErrors( dr, drCached );
        }
    }
    else
    {
        if( f_pFFLicense->fDomainBoundRootRequired )
        {
            /*
            ** The license evaluator for some license in the chain indicated
            **  that the root license MUST be domain-bound in order for
            **  chain evaluation to succeed.  Verify this here.
            */
            ChkBOOL(
                ( f_pFFLicense->pLicEval->plicenseXMR != NULL )
             && XMR_IS_DOMAIN_ID_VALID( f_pFFLicense->pLicEval->plicenseXMR ),
                DRM_E_RIGHTSNOTAVAILABLE );
        }

        /*
        ** We can use this license.  Now try to set up a decryptor.
        ** Only setup a decryptor if this is either a simple license ( no chain ),
        ** or the last link in the chain
        */
        if ( !f_pFFLicense->fCanBind )
        {
            ChkDR( _ProcessEndOfChain( f_pFFLicense, f_pfnPolicyCallback, f_pv ) );
        }
    }

    *f_pdwChainDepth = f_pFFLicense->dwChainDepth + 1;

ErrorExit:

    if ( DRM_FAILED( dr )
     && f_pFFLicense != NULL
     && f_pdwChainDepth != NULL )
    {
        *f_pdwChainDepth = max( *f_pdwChainDepth, f_pFFLicense->dwChainDepth + 1 );
    }

    /*
    ** If the function fails and a stack buffer has been allocated for the license(
    ** allocated in _GetLicenseInfoAndSetup() at the beginning of the function),
    ** the stack buffer has to be released. Otherwise future stack allocation for the
    ** next candidate license may fail with DRM_E_BUFFERTOOSMALL.
    */
    if ( DRM_FAILED( dr )
     && f_pFFLicense != NULL
     && f_pFFLicense->pstack != NULL
     && nStackTop != ULONG_MAX )
    {
        f_pFFLicense->pstack->nStackTop = nStackTop;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*****************************************************************************
** Function : DRM_LIC_ReportActions
**
** Synopsis : Report actions and update metering info
**
** Parameters:
**      [f_pFFLicense]    : Specifies the find first license structure to use
**      [f_rgpdstrRights] : Specifies the set of rights strings
**      [f_cRights]       : Specifies the number of elements in f_rgpdstrRights
**      [f_dwChainDepth]  : Specifies the current chain depth
**      [f_pDatastore]    : Specifies the DST
**      [f_pfnPolicyCallback] : Callback function for output restrictions
**      [f_pv]                      : Void pass-through parameter for callback
**
** Returns :
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          A required argument was NULL or invalid
**      DRM_E_RIGHTSNOTAVAILABLE
**          Unable to parse a right from the license
**      DRM_E_LICENSENOTFOUND
**          License not found
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_LIC_ReportActions(
    IN OUT   DRM_FFLICENSE                *f_pFFLicense,
    IN const DRM_CONST_STRING             *f_rgpdstrRights[], /* Array of DRM_CONST_STRING pointers */
    IN       DRM_DWORD                     f_cRights,
    IN       DRM_DWORD                     f_dwChainDepth,
    IN       DRM_DST                      *f_pDatastore,
    IN       DRMPFNPOLICYCALLBACK          f_pfnPolicyCallback,
    IN const DRM_VOID                     *f_pv )
{
    DRM_RESULT dr          = DRM_SUCCESS;
    DRM_DWORD  cbLicSize   = 0;
    DRM_DWORD  iCount      = 0;
    DRM_DWORD  iRights     = 0;
    DRM_BOOL   fEvalResult = FALSE;
    DRM_BOOL   fExisted    = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMCHAIN, PERF_FUNC_DRM_LIC_ReportActions );

    ChkArg( f_pFFLicense != NULL );

    /*
    ** Report actions and update metering info
    */
    for( ; iCount < f_dwChainDepth ; iCount++ )
    {
        f_pFFLicense->rgfDeleteLicense[iCount] = FALSE;

        /*
        ** Read the licenses from the license store again.  It will need
        **  refreshing if it was a chained license
        */
        if ( f_pFFLicense->pLicenseXMR == NULL )
        {
            cbLicSize = f_pFFLicense->cbBuffer;
            ChkDR( DRM_LST_GetLicense( f_pFFLicense->rgpLicStoreCurrent[iCount],
                                      &f_pFFLicense->rgkid[iCount],
                                      &f_pFFLicense->rglid[iCount],
                                      &( f_pFFLicense->rgslotHint[iCount] ),
                                       f_pFFLicense->pbBuffer,
                                      &cbLicSize ) );

            if( DRM_XMLHASH_IsXmlHashSupported() )
            {
                ChkDR( DRM_XMLHASH_Start());
            }

            DSTR_FROM_PB( &( f_pFFLicense->pLicEval->dstrContentLicense ),
                            f_pFFLicense->pbBuffer,
                            cbLicSize );
            f_pFFLicense->pLicEval->fUseCachedAttribs = FALSE;

            /*
            **  Load the necessary License Attribs
            */
            ChkDR( DRM_WMDRM_LIC_LoadLicenseAttributes( f_rgpdstrRights, f_cRights, f_pFFLicense->pLicEval ) );
        }

        if ( NULL == f_pFFLicense->pSecStoreLicense )
        {
            f_pFFLicense->pLicEval->pcontextSSTLicense  = NULL;
        }
        else
        {
            f_pFFLicense->pLicEval->pcontextSSTLicense  = &( f_pFFLicense->pSecStoreLicense[iCount] );
        }

        f_pFFLicense->pLicEval->fGlobalSecStoreWritable = FALSE;
        f_pFFLicense->pLicEval->fReserved               = 0;
        if( DRM_CONTENTREVOCATION_IsContentRevocationSupported() )
        {
            f_pFFLicense->pLicEval->dwChainDepth            = iCount;
        }
        if ( f_pFFLicense->pLicenseXMR != NULL )
        {
            f_pFFLicense->pLicEval->plicenseXMR   = &f_pFFLicense->pLicenseXMR[iCount];
            f_pFFLicense->pLicEval->fLicenseIsXMR =  TRUE;
        }
        else
        {
            f_pFFLicense->pLicEval->fLicenseIsXMR = FALSE;
        }

        f_pFFLicense->pLicEval->fDeleteLicense = FALSE;
        iRights = f_cRights;
        for( ; iRights > 0; )
        {
            fEvalResult = FALSE;
            fExisted    = FALSE;

            iRights--;
            ChkDR( DRM_LEVL_PerformOperations( f_pFFLicense->pLicEval,
                                               DRM_LICENSE_EVAL_ACTION,
                                               DRM_LICENSE_EVAL_DONE_WITH_OPERATION,
                                               f_rgpdstrRights[iRights],
                                              &fEvalResult,
                                              &fExisted,
                                               f_pDatastore,
                                               f_pfnPolicyCallback,
                                               f_pv ) );

            ChkBOOL( !fExisted || fEvalResult, DRM_E_RIGHTSNOTAVAILABLE );
        }
        f_pFFLicense->rgfDeleteLicense[iCount] = f_pFFLicense->pLicEval->fDeleteLicense;
    }
ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_LIC_CheckClockRollback(
    IN DRM_LICEVAL_CONTEXT                      *f_pLicEval,
    IN DRM_LICSTORE_CONTEXT                     *f_pLicStoreXML,
    IN DRM_LICSTORE_CONTEXT                     *f_pLicStoreXMR,
    IN DRM_LICSTOREENUM_CONTEXT                 *f_pLicEnum,
    IN DRM_BYTE                                 *f_pbBuffer,
    IN DRM_DWORD                                 f_cbBuffer,
    IN DRM_DST                                  *f_pDatastore,
    __in_bcount( f_cbStackBuffer ) DRM_BYTE     *f_pbStackBuffer,
    IN DRM_DWORD                                 f_cbStackBuffer )
{
    DRM_RESULT       dr       = DRM_SUCCESS;
    DRM_STACK_ALLOCATOR_CONTEXT oStack             = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMCHAIN, PERF_FUNC_DRM_LIC_CheckClockRollback );

    ChkDR( DRM_STK_Init( &oStack,
                         f_pbStackBuffer,
                         f_cbStackBuffer ) );

    ChkDR( DRM_ARCLK_LIC_CheckClockRollback(
        f_pLicEval,
        f_pLicStoreXML,
        f_pLicStoreXMR,
        f_pLicEnum,
        f_pbBuffer,
        f_cbBuffer,
        f_pDatastore,
        &oStack ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*********************************************************************
**
**  Function:  _ProcessExtensionsXMR
**
**  Synopsis:  Process all extensions from an XMR analog video and digital
               audio protection structures
**
**  Arguments:
**     [f_pxmrVideoRestrictions] : Structure containing all video restrictions
**     [f_pxmrAudioRestrictions] : Structure containing all audio restrictions
**     [f_pPlayOPL]              : Play OPL struct that will be filled in
**     [f_pstack]                : Stack allocator context that will be used to allocate
**                                 room for the extensions
**  Returns:
**      DRM_SUCCESS
**          Success
**      DRM_E_INVALIDARG
**          One of the arguments was NULL
**      DRM_E_ARITHMETICOVERFLOW
**          An arithmetic overflow occurred during processing
**
**  Notes: Data may be allocated from the stack context.  This data will be returned in the
**         f_pPlayOPL.vopi.rgVop and f_pPlayOPL.aopi.rgAop pointers.
**
*********************************************************************/
static DRM_RESULT _ProcessExtensionsXMR(
    IN const DRM_XMR_EXPLICIT_ANALOG_VIDEO_PROTECTION   *f_pxmrVideoRestrictions,
    IN const DRM_XMR_EXPLICIT_DIGITAL_AUDIO_PROTECTION  *f_pxmrAudioRestrictions,
    IN OUT   DRM_PLAY_OPL_EX2                           *f_pPlayOPL,
    IN       DRM_STACK_ALLOCATOR_CONTEXT                *f_pstack )
{
    DRM_RESULT dr               = DRM_SUCCESS;
    DRM_WORD   iExtension       = 0;
    DRM_WORD   cVideoExtensions = 0;
    DRM_DWORD  cVop             = 0;
    DRM_WORD   cAudioExtensions = 0;
    DRM_DWORD  cAop             = 0;
    DRM_VIDEO_OUTPUT_PROTECTION_EX          *pVop                          = NULL;
    DRM_VIDEO_OUTPUT_PROTECTION_EX          *pAop                          = NULL;
    DRM_XMR_VIDEO_OUTPUT_CONFIGURATION_LIST *plistVideoOutputConfiguration = NULL;
    DRM_XMR_VIDEO_OUTPUT_CONFIGURATION_LIST *plistAudioOutputConfiguration = NULL;

    MEMSET( &f_pPlayOPL->vopi, 0, SIZEOF(f_pPlayOPL->vopi) );
    MEMSET( &f_pPlayOPL->aopi, 0, SIZEOF(f_pPlayOPL->aopi) );

    if( f_pxmrVideoRestrictions->fValid )
    {
        ChkDR( DRM_UTL_MultiplyDWORDs( f_pxmrVideoRestrictions->cOutputProtectionIDs,
                                   SIZEOF( DRM_VIDEO_OUTPUT_PROTECTION_EX ),
                                      &cVop ) );

        ChkDR( DRM_STK_Alloc( f_pstack, cVop, ( DRM_VOID** )&pVop ) );
        ZEROMEM( pVop, cVop );

        /*
        ** Populate the data structure with data from the license
        */
        for( plistVideoOutputConfiguration = f_pxmrVideoRestrictions->plistOutputConfiguration;
             iExtension < f_pxmrVideoRestrictions->cOutputProtectionIDs
          && plistVideoOutputConfiguration != NULL;
             iExtension++, plistVideoOutputConfiguration = plistVideoOutputConfiguration->pNext )
        {
            DRM_BYT_CopyBytes( &pVop[iExtension].guidId,
                                0,
                                plistVideoOutputConfiguration->Config.pguidBuffer,
                                plistVideoOutputConfiguration->Config.iGuid,
                                SIZEOF( pVop[iExtension].guidId ) );

            DRM_BYT_CopyBytes( &pVop[iExtension].dwConfigData,
                                0,
                                plistVideoOutputConfiguration->Config.pbConfigDataBuffer,
                                plistVideoOutputConfiguration->Config.iConfigData,
                                plistVideoOutputConfiguration->Config.cbConfigData );

            pVop[iExtension].dwVersion = VER_DRM_VIDEO_OUTPUT_PROTECTION;

            cVideoExtensions++;
        }

        f_pPlayOPL->vopi.cEntries = cVideoExtensions;
        f_pPlayOPL->vopi.rgVop    = pVop;
        pVop = NULL;
    }

    if( f_pxmrAudioRestrictions->fValid )
    {
        ChkDR( DRM_UTL_MultiplyDWORDs( f_pxmrAudioRestrictions->cOutputProtectionIDs,
                                   SIZEOF( DRM_AUDIO_OUTPUT_PROTECTION_EX ),
                                      &cAop ) );

        ChkDR( DRM_STK_Alloc( f_pstack, cAop, ( DRM_VOID** )&pAop ) );
        ZEROMEM( pAop, cAop );

        /*
        ** Populate the data structure with data from the license
        */
        for( plistAudioOutputConfiguration = f_pxmrAudioRestrictions->plistOutputConfiguration,
             iExtension = 0;
             iExtension < f_pxmrAudioRestrictions->cOutputProtectionIDs
          && plistAudioOutputConfiguration != NULL;
             iExtension++, plistAudioOutputConfiguration = plistAudioOutputConfiguration->pNext )
        {
            DRM_BYT_CopyBytes( &pAop[iExtension].guidId,
                                0,
                                plistAudioOutputConfiguration->Config.pguidBuffer,
                                plistAudioOutputConfiguration->Config.iGuid,
                                SIZEOF( pAop[iExtension].guidId ) );


            DRM_BYT_CopyBytes( &pAop[iExtension].dwConfigData,
                                0,
                                plistAudioOutputConfiguration->Config.pbConfigDataBuffer,
                                plistAudioOutputConfiguration->Config.iConfigData,
                                plistAudioOutputConfiguration->Config.cbConfigData );

            pAop[iExtension].dwVersion = VER_DRM_VIDEO_AUDIO_OUTPUT_PROTECTION;

            cAudioExtensions++;
        }

        f_pPlayOPL->aopi.cEntries = cAudioExtensions;
        f_pPlayOPL->aopi.rgAop    = pAop;
        pAop = NULL;
    }

ErrorExit:
    SAFE_STK_FREE( f_pstack, pVop );
    SAFE_STK_FREE( f_pstack, pAop );
    return dr;
}

/*********************************************************************
**
**  Function:  DRM_LICOPL_ProcessPlayOutputLevelDataXMR
**
**  Synopsis:  Process a Playback OPL for XMR
**
**  Arguments:
**     [f_pxmrPlaybackRights] : XMR Playback rights structure to parse the OPL from
**     [f_pPlayOPL]           : Playback OPL struct that will be populated with output level data
**     [f_pstack]             : Stack allocator context that will be used to allocate room for IDs
**
** Returns:
**      DRM_SUCCESS:
**          Success
**      DRM_E_INVALIDARG:
**          One of the arguments was NULL or improperly initialized
**
**  Notes:  Caller should provide a clean, unused stack context.
**          Caller shouldn't free data directly from stack context.
**          Instead just throw entire stack context away when finished with the OPL structures.
**
*********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_LICOPL_ProcessPlayOutputLevelDataXMR(
    IN const DRM_XMR_PLAYBACK_RIGHTS      *f_pxmrPlaybackRights,
    IN OUT   DRM_PLAY_OPL_EX2             *f_pPlayOPL,
    IN       DRM_STACK_ALLOCATOR_CONTEXT  *f_pstack )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pPlayOPL          != NULL );
    ChkArg( f_pstack            != NULL );
    ChkArg( f_pstack->pbStack   != NULL );
    ChkArg( f_pstack->cbStack    > 0 );
    ChkArg( f_pxmrPlaybackRights!= NULL );

    ZEROMEM( f_pPlayOPL, SIZEOF( *f_pPlayOPL ) );

    if( f_pxmrPlaybackRights->opl.fValid )
    {
        f_pPlayOPL->minOPL.wCompressedDigitalVideo   = f_pxmrPlaybackRights->opl.wCompressedDigitalVideo;
        f_pPlayOPL->minOPL.wUncompressedDigitalVideo = f_pxmrPlaybackRights->opl.wUncompressedDigitalVideo;
        f_pPlayOPL->minOPL.wAnalogVideo              = f_pxmrPlaybackRights->opl.wAnalogVideo;
        f_pPlayOPL->minOPL.wCompressedDigitalAudio   = f_pxmrPlaybackRights->opl.wCompressedDigitalAudio;
        f_pPlayOPL->minOPL.wUncompressedDigitalAudio = f_pxmrPlaybackRights->opl.wUncompressedDigitalAudio;
    }

    ChkDR( _ProcessExtensionsXMR( &f_pxmrPlaybackRights->containerExplicitAnalogVideoProtection,
                                  &f_pxmrPlaybackRights->containerExplicitDigitalAudioProtection,
                                   f_pPlayOPL,
                                   f_pstack ) );

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

