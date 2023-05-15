/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMWMDRMLICCHAINREAL_C
#include <drmwmdrm.h>
#include <drmsymopt.h>
#include <drmxmlparser.h>
#include <drmcopyopl.h>
#include <drmapprevocation.h>
#include <drmrevocation.h>
#include <drmwmdrmnet.h>
#include <drmdevicerevocation.h>
#include <drmxmlhash.h>
#include <drmmetering.h>
#include <drmwmdrmconstants.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/******************************************************************************
**
** Function :   _ParseLicenseAttributes
**
** Synopsis :   Parse the license for frequently used attributes
**
** Arguments :  f_pdstrLicense  - License string
**              f_rgpdstrEvents - Array of events to parse
**              f_cEvents       - Count of events to parse
**              f_rgdstrCachedAttribs - Cached attribs
**              f_rgCachedEvents- Cached events
**
** Returns :
**
** Notes :      We can parse the license more optimally if needed.
**
******************************************************************************/
static DRM_RESULT _ParseLicenseAttributes(
    __in  const DRM_CONST_STRING    *f_pdstrLicense,
    __in  const DRM_CONST_STRING    *f_rgpdstrEvents[],
    __in        DRM_DWORD            f_cEvents,
    __out       DRM_CONST_STRING     f_rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_LAST],
    __out       DRM_CACHED_EVENT     f_rgCachedEvents[DRM_MAX_ACTIONS] )
{
    DRM_RESULT          dr          = DRM_SUCCESS;
    DRM_CONST_STRING    dstrLIData  = EMPTY_DRM_STRING;
    DRM_DWORD           iEvents     = 0;

    /*
    **  If need be we can try to do better here by doing a single scan of the XML
    */
    ChkDR( DRM_WMDRM_LIC_GetAttribute( f_pdstrLicense,
                                       NULL,
                                       DRM_LICENSE_ATTRIB_KID,
                                       &dstrLIData,
                                       &f_rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_KID],
                                       0 ) );

    ChkDR( DRM_WMDRM_LIC_GetAttribute( f_pdstrLicense,
                                       NULL,
                                       DRM_LICENSE_ATTRIB_LID,
                                       &dstrLIData,
                                       &f_rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_LID],
                                       0 ) );

    dr = DRM_WMDRM_LIC_GetAttribute( f_pdstrLicense,
                                     NULL,
                                     DRM_LICENSE_ATTRIB_CONTENTPUBKEY,
                                     &dstrLIData,
                                     &f_rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_CONTENTPUBKEY],
                                     0 );
    if ( DRM_FAILED( dr ) )
    {
        INIT_DRM_STRING( f_rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_CONTENTPUBKEY] );
        ChkDR( DRM_SUCCESS );  /* This sets 'dr' and keeps the PREfast tool happy. */
    }

    dr = DRM_WMDRM_LIC_GetAttribute( f_pdstrLicense,
                                     NULL,
                                     DRM_LICENSE_ATTRIB_METERING_ID,
                                     &dstrLIData,
                                     &f_rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_METERING_ID],
                                     0 );
    if ( DRM_FAILED( dr ) )
    {
        INIT_DRM_STRING( f_rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_METERING_ID] );
        ChkDR( DRM_SUCCESS );  /* This sets 'dr' and keeps the PREfast tool happy. */
    }

    dr = DRM_WMDRM_LIC_GetAttribute( f_pdstrLicense,
                                     NULL,
                                     DRM_LICENSE_ATTRIB_CHAINEDKID,
                                     &dstrLIData,
                                     &f_rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_CHAINEDKID],
                                     0 );
    if ( DRM_FAILED( dr ) )
    {
        INIT_DRM_STRING( f_rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_CHAINEDKID] );
        dr = DRM_SUCCESS;
    }

    for ( iEvents = 0; ( iEvents < f_cEvents ) && ( iEvents < DRM_MAX_ACTIONS ); iEvents++ )
    {
        ASSIGN_DRM_STRING( f_rgCachedEvents[iEvents].dstrType, *( f_rgpdstrEvents[iEvents] ) );
        dr = DRM_WMDRM_LIC_GetEvent( f_pdstrLicense,
                                     &g_dstrLicEvalOnAction,
                                     f_rgpdstrEvents[iEvents],
                                     &dstrLIData,
                                     &f_rgCachedEvents[iEvents].dstrCondition,
                                     &f_rgCachedEvents[iEvents].dstrAction,
                                     &f_rgCachedEvents[iEvents].dstrRestriction );
        if ( DRM_FAILED( dr ) )
        {
            INIT_DRM_STRING( f_rgCachedEvents[iEvents].dstrCondition );
            INIT_DRM_STRING( f_rgCachedEvents[iEvents].dstrAction );
            INIT_DRM_STRING( f_rgCachedEvents[iEvents].dstrRestriction );
            dr = DRM_SUCCESS;
        }
    }


ErrorExit:
    return dr;
}

/******************************************************************************
**
** Function :   DRM_WMDRM_LIC_LoadLicenseAttributes
**
** Synopsis :   Load some of those attributes from a license which are
**              frequently used.
**
** Arguments :  rgpdstrRights : Array of pointers to rights requested
**              f_pLicEval    : License eval context
**
** Returns :
**
** Notes :
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_LoadLicenseAttributes(
    __in  const DRM_CONST_STRING    *f_rgpdstrEvents[],
    __in        DRM_DWORD            f_cRights,
    __inout     DRM_LICEVAL_CONTEXT *f_pLicEval )
{
    DRM_RESULT          dr          = DRM_SUCCESS;

    ChkDR( _ParseLicenseAttributes( &f_pLicEval->dstrContentLicense,
                                    f_rgpdstrEvents,
                                    f_cRights,
                                    f_pLicEval->rgdstrCachedAttribs,
                                    f_pLicEval->rgCachedEvents ) );
    f_pLicEval->cCachedEvents = f_cRights;

    /*
    **  Cache the binary LID
    */
    ChkDR( DRM_UTL_StringToLittleEndianGuid( &f_pLicEval->rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_LID],
                                             ( DRM_GUID* ) &( f_pLicEval->LID ) ) );

    /*
    **  Cache the binary KID
    */
    ChkDR( DRM_UTL_DecodeKID( &f_pLicEval->rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_KID],
                              &f_pLicEval->KID ) );

    f_pLicEval->fUseCachedAttribs = TRUE;

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_PerformActionsOPLCallbacks(
    __inout       DRM_FFLICENSE               *f_pFFLicense,
    __inout       DRM_STACK_ALLOCATOR_CONTEXT *f_pstack,
    __in    const DRM_CONST_STRING            *f_rgpdstrRights[],
    __in          DRM_DWORD                    f_iRight,
    __in          DRMPFNPOLICYCALLBACK         f_pfnPolicyCallback )
{
    DRM_RESULT dr = DRM_SUCCESS;

    if ( f_pFFLicense->pLicEval->rgCachedEvents[f_iRight].dstrRestriction.cchString != 0 )
    {
        /*
        ** There were output restrictions and no callback function was supplied
        */
        ChkBOOL( f_pfnPolicyCallback != NULL, DRM_E_NO_OPL_CALLBACK );

        /*
        ** Reset the stack context
        */
        ZEROMEM( f_pstack, SIZEOF( DRM_STACK_ALLOCATOR_CONTEXT ) );

        if ( DRM_UTL_DSTRStringsEqual( f_rgpdstrRights[f_iRight], &g_dstrWMDRM_RIGHT_PLAYBACK ) )
        {
            f_pstack->pbStack = f_pFFLicense->rgPlayOpl[f_pFFLicense->dwChainDepth].rgbPlayOPLBuffer;
            f_pstack->cbStack = SIZEOF( f_pFFLicense->rgPlayOpl[f_pFFLicense->dwChainDepth].rgbPlayOPLBuffer );

            ChkDR( DRM_WMDRM_LICOPL_ProcessPlayOutputLevelData( &f_pFFLicense->pLicEval->rgCachedEvents[f_iRight].dstrRestriction,
                                                                &f_pFFLicense->rgPlayOpl[f_pFFLicense->dwChainDepth].oplPlay,
                                                                f_pstack ) );
            f_pFFLicense->rgfPlayOplValid[f_pFFLicense->dwChainDepth] = TRUE;
        }

        else if ( DRM_UTL_DSTRStringsEqual( f_rgpdstrRights[f_iRight], &g_dstrWMDRM_RIGHT_COPY ) )
        {
            if( DRM_COPYOPL_IsCopyOplSupported() )
            {
                f_pstack->pbStack = f_pFFLicense->rgCopyOpl[f_pFFLicense->dwChainDepth].rgbCopyOPLBuffer;
                f_pstack->cbStack = SIZEOF( f_pFFLicense->rgCopyOpl[f_pFFLicense->dwChainDepth].rgbCopyOPLBuffer );

                ChkDR( DRM_COPYOPL_ProcessCopyOutputLevelData(
                    &f_pFFLicense->pLicEval->rgCachedEvents[f_iRight].dstrRestriction,
                    &f_pFFLicense->rgCopyOpl[f_pFFLicense->dwChainDepth].oplCopy,
                    f_pstack ) );
                f_pFFLicense->rgfCopyOplValid[f_pFFLicense->dwChainDepth] = TRUE;
            }
            else
            {
                ChkDR( DRM_E_RIGHTS_NOT_AVAILABLE );
            }
        }
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_ExtractRevocationVersions(
    __inout DRM_FFLICENSE    *f_pFFLicense,
    __out   DRM_DWORD        *f_pidCRLWMDRMAppRequired,
    __out   DRM_DWORD        *f_pidCRLDeviceRequired,
    __out   DRM_DWORD        *f_pidCRLWMDRMNETRequired,
    __out   DRM_DWORD        *f_pdwRevInfo )
{
    DRM_RESULT       dr          = DRM_SUCCESS;
    DRM_CONST_STRING dstrRevInfo = EMPTY_DRM_STRING;

    if( DRM_APPREVOCATION_IsAppRevocationSupported() )
    {
        *f_pidCRLWMDRMAppRequired = f_pFFLicense->pLicEval->idCRLsCurrent.wmdrmapp;

        ChkDR( DRM_RVK_PerformAppRevocationCheck( &f_pFFLicense->pLicEval->pcontextBBX->CryptoContext,
                                                  f_pFFLicense->pLicEval->pContextRev,
                                                  &f_pFFLicense->pLicEval->dstrContentLicense,
                                                  &f_pFFLicense->pLicEval->appcert,
                                                  f_pFFLicense->pLicEval->pbRevocationBuffer,
                                                  f_pFFLicense->pLicEval->cbRevocationBuffer,
                                                  &f_pFFLicense->pLicEval->idCRLsCurrent ) );
    }

    if( DRM_DEVICEREVOCATION_IsDeviceRevocationSupported() )
    {
        dr = DRM_WMDRM_LIC_GetAttribute( &f_pFFLicense->pLicEval->dstrContentLicense,
                                         &g_dstrDeviceRevocationLicenseTag,
                                         DRM_LICENSE_ATTRIB_REVOCATION,
                                         NULL,
                                         &dstrRevInfo,
                                         0 );
        if ( DRM_SUCCEEDED( dr ) )
        {
            DRM_DWORD dwTemp = 0;
            ChkDR( DRMCRT_wcsntol( dstrRevInfo.pwszString, dstrRevInfo.cchString, ( DRM_LONG * )&dwTemp ) );
            *f_pidCRLDeviceRequired = dwTemp;
        }
        dr = DRM_SUCCESS; /* ignore errors */
    }

    if( DRM_WMDRMNET_IsWmdrmnetSupported() )
    {
        dr = DRM_WMDRM_LIC_GetAttribute( &f_pFFLicense->pLicEval->dstrContentLicense,
                                         &g_dstrWMDRMNETRevocationLicenseTag,
                                         DRM_LICENSE_ATTRIB_REVOCATION,
                                         NULL,
                                         &dstrRevInfo,
                                         0 );
        if ( DRM_SUCCEEDED( dr ) )
        {
            DRM_DWORD dwTemp = 0;
            ChkDR( DRMCRT_wcsntol( dstrRevInfo.pwszString, dstrRevInfo.cchString, ( DRM_LONG * )&dwTemp ) );
            *f_pidCRLWMDRMNETRequired = dwTemp;
        }
        dr = DRM_SUCCESS; /* ignore errors */
    }

    *f_pdwRevInfo = 0;

    if( DRM_REVOCATION_IsV1RIVSupported() )
    {
        dr = DRM_WMDRM_LIC_GetAttribute( &f_pFFLicense->pLicEval->dstrContentLicense,
                                         NULL,
                                         DRM_LICENSE_ATTRIB_REVINFO,
                                         NULL,
                                         &dstrRevInfo,
                                         0 );

        if( dr != DRM_E_XMLNOTFOUND )
        {
            DRM_DWORD dwTemp = 0;
            ChkDR( dr );
            ChkDR( DRMCRT_wcsntol( dstrRevInfo.pwszString, dstrRevInfo.cchString, ( DRM_LONG * )&dwTemp ) );
            *f_pdwRevInfo = dwTemp;
        }
        dr = DRM_SUCCESS; /* ignore DRM_E_XMLNOTFOUND */
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetLicenseInfoAndSetup(
    __inout       DRM_FFLICENSE    *f_pFFLicense,
    __in    const DRM_CONST_STRING *f_rgpdstrRights[],
    __in          DRM_DWORD         f_cRights )
{
    DRM_RESULT           dr             = DRM_SUCCESS;
    DRM_SYMMOPT_STATUS   eSymOptStatus  = DRM_SYMMOPT_STATUS_NONE;
    DRM_BYTE            *pbChecksum     = NULL;
    DRM_DWORD            cbContentKey   = 0;

    f_pFFLicense->pLicEval->fUseCachedAttribs = FALSE;
    f_pFFLicense->pLicEval->fLicenseIsXMR = FALSE;
    f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_fXMRLicense = FALSE;

    if( DRM_XMLHASH_IsXmlHashSupported() )
    {
        ChkDR( DRM_XMLHASH_Start() );
    }

    /*
    **  Load the necessary License Attribs
    */
    ChkDR( DRM_WMDRM_LIC_LoadLicenseAttributes( f_rgpdstrRights, f_cRights, f_pFFLicense->pLicEval ) );

    /*
    ** see if we have a global policy forbidding metering
    */
    if ( ( f_pFFLicense->dwPolicyFlags & DRM_POLICY_FLAG_METERING_DISABLED_ENTERPRISE ) != 0 )
    {
        ChkBOOL( f_pFFLicense->pLicEval->rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_METERING_ID].cchString == 0,
                 DRM_E_POLICY_METERING_DISABLED );
    }
    else
    {
        if( DRM_METERING_IsMeteringSupported() )
        {
            if ( f_pFFLicense->pLicEval->rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_METERING_ID].cchString != 0 )
            {
                DRM_DWORD cbMID = SIZEOF( DRM_MID );

                f_pFFLicense->rgfHasMetering [f_pFFLicense->dwChainDepth] = TRUE;

                ChkDR( DRM_B64_DecodeW( &f_pFFLicense->pLicEval->rgdstrCachedAttribs[eDRM_LIC_CACHED_ATTRIB_METERING_ID],
                                        &cbMID,
                                        ( DRM_BYTE * ) &f_pFFLicense->rgmid[f_pFFLicense->dwChainDepth],
                                        0 ) );
            }
        }
    }

    cbContentKey = SIZEOF( f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_BindingInfo.m_XML.m_rgbContentKey );
    if ( f_pFFLicense->dwChainDepth == DRM_MAX_LICENSE_CHAIN_DEPTH-1 )
    {
        /*
        ** If we are on the last level that a chain could be pass in NULL
        */
        pbChecksum = NULL;
    }
    else
    {
        /*
        ** Otherwise get the checksum for the next license up the chain
        */
        pbChecksum = f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth + 1].m_oChecksum.m_rgbChecksumData;
    }

    ChkDR( DRM_WMDRM_LIC_GetEnablingBits( &( f_pFFLicense->pLicEval->dstrContentLicense ),
                                          0, /* Get the first enabling bit */
                                          &f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_BindingInfo.m_XML.m_dwAlgorithm,
                                          &f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_BindingInfo.m_XML.m_oPublKey,
                                          f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_BindingInfo.m_XML.m_rgbContentKey,
                                          &cbContentKey,
                                          &f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_BindingInfo.m_XML.m_oLsPublKey,
                                          f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_BindingInfo.m_XML.m_rgbSignature,
                                          pbChecksum,
                                          &f_pFFLicense->pLicEval->pcontextBBX->CryptoContext ) );

    f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_BindingInfo.m_XML.m_fCKSigVerified = FALSE;

    /*
    ** Determine the status of symm opt for the end of the chain (leaf or simple).
    */
    ChkDR( DRM_SYMOPT_LIC_CheckSymmOptStatus( &( f_pFFLicense->pLicEval->dstrContentLicense ),
                                              &eSymOptStatus,
                                              NULL,
                                              NULL,
                                              NULL ) );
    if ( eSymOptStatus == DRM_SYMMOPT_STATUS_COMPLETE )
    {
        f_pFFLicense->pBindingInfo[f_pFFLicense->dwChainDepth].m_BindingInfo.m_XML.m_fCKSigVerified = TRUE;
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetSourceIDFromLicense(
    __inout DRM_FFLICENSE *f_pFFLicense )
{
    DRM_RESULT       dr        = DRM_SUCCESS;
    DRM_CONST_STRING dstrValue = EMPTY_DRM_STRING;

    dr = DRM_WMDRM_LIC_GetAttribute( &f_pFFLicense->pLicEval->dstrContentLicense,
                                     NULL,
                                     DRM_LICENSE_ATTRIB_SOURCEID,
                                     NULL,
                                     &dstrValue,
                                     0 );
    if ( DRM_SUCCEEDED( dr ) )
    {
        ChkDR( DRMCRT_WtoDWORD( dstrValue.pwszString,
                                dstrValue.cchString,
                                10, /* base 10 */
                                &f_pFFLicense->dwSourceid,
                                NULL ) );
    }
    else
    {
        dr = DRM_SUCCESS; /* ignore errors */
    }

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LIC_GetInclusionList(
    __in const DRM_CONST_STRING  *f_pdstrLicense,
    __inout    DRM_GUID           f_rgInclusionList[DRM_MAX_INCLUSION_GUIDS],
    __inout    DRM_BOOL           f_rgfGuidValid[DRM_MAX_INCLUSION_GUIDS][DRM_MAX_LICENSE_CHAIN_DEPTH],
    __in       DRM_DWORD          f_dwChainDepth )
{
    DRM_RESULT       dr       = DRM_SUCCESS;
    DRM_CONST_STRING dstrList = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrGuid = EMPTY_DRM_STRING;
    DRM_DWORD        dwGuid    = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRMCHAIN, PERF_FUNC_DRM_WMDRM_LIC_GetInclusionList );

    /*
    ** Set all the valid flags at this chain depth to FALSE for all GUIDs
    */
    for( ; dwGuid < DRM_MAX_INCLUSION_GUIDS; dwGuid++ )
    {
        f_rgfGuidValid[dwGuid][f_dwChainDepth] = FALSE;
    }

    dwGuid = 0;

    ChkDR( DRM_XML_GetSubNodeByPath( f_pdstrLicense, &g_dstrTagDataInclusionList, NULL, NULL, &dstrList, NULL, g_wchForwardSlash ) );

    while( DRM_SUCCEEDED( DRM_XML_GetSubNode( &dstrList, &g_dstrGUID, NULL, NULL, dwGuid, NULL, &dstrGuid, 1 ) ) )
    {
        DRM_GUID guid;

        if ( f_dwChainDepth == 0
         && dwGuid        >= DRM_MAX_INCLUSION_GUIDS )
        {
            /*
            **  If this is a leaf license we have to stop at the Maximum number of guids
            */
            ChkDR( DRM_E_TOO_MANY_INCLUSION_GUIDS );
        }

        /*
        **  Convert the value to a GUID
        */

        if ( DRM_SUCCEEDED( DRM_UTL_StringToLittleEndianGuid(  &dstrGuid, &guid ) ) )
        {

            if ( f_dwChainDepth == 0 )
            {
                /*
                **  For the leaf license we need to populate the output array of GUIDs
                **  and valid list as appropriate
                */
                MEMCPY( &f_rgInclusionList[dwGuid], &guid, SIZEOF( guid ) );
                f_rgfGuidValid[dwGuid][0] = TRUE;
            }
            else
            {
                DRM_DWORD i;

                /*
                **  For non-leaf licenses we should check to see if this guid is in the list and if so is it valid
                **  at the previous chain level.  If it is then it is still valid at the current level.
                */
                for( i = 0; i < DRM_MAX_INCLUSION_GUIDS; i++ )
                {
                    if ( MEMCMP( &f_rgInclusionList[i], &guid, SIZEOF( guid ) ) == 0
                     && f_rgfGuidValid[i][f_dwChainDepth - 1] )
                    {
                        f_rgfGuidValid[i][f_dwChainDepth] = TRUE;
                    }
                }
            }
        }
        dwGuid++;
    }


ErrorExit:

    if ( dr != DRM_E_TOO_MANY_INCLUSION_GUIDS )
    {
        /* Don't fail for any reason other than too many GUIDs */
        dr = DRM_SUCCESS;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

