/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>
#include <drmcertcache.h>
#include <drmmanager.h>
#include <drmsecureclock.h>
#include <drmsyncstore.h>
#include <drmlicreason.h>
#include <drmxmlparser.h>
#include <drmopcodes.h>
#include <drmutf.h>
#include <drmembedding.h>
#include <devicedevcert.h>
#include <drmsymopt.h>
#include <drmrevocation.h>
#include <drmcontract.h>
#include <drmxmlhash.h>
#include <drmsoapxmlutility.h>
#include <drmapprevocation.h>
#include <drmdevicerevocation.h>
#include <drmwmdrmnet.h>
#include <drmapppolicy.h>
#include <drmmeterapi.h>
#include <drmhdsblockheadercache.h>
#include <drmmanagerimpl.h>
#include <drmcleanstore.h>
#include <drmperformance.h>
#include <drmwmdrm.h>
#include <drmactivationimp.h>
#include <oemkeyfile.h>
#include <oemeccp160.h>
#include <drmmanagerconstants.h>

ENTER_PK_NAMESPACE_CODE;

DRM_VOID drmapp_casserts( DRM_VOID )
{
    /* If any of these compile time asserts fire it is because size of internal contexts don't match. */
    DRMSIZEASSERT( SIZEOF(DRM_DECRYPT_CONTEXT), SIZEOF( DRM_CIPHER_CONTEXT ) );
    DRMSIZEASSERT( SIZEOF(DRM_APP_CONTEXT), SIZEOF( DRM_APP_CONTEXT_INTERNAL ) );
}

DRM_API DRM_VOID DRM_CALL _FreeDrmAppInternalContexts(
    IN DRM_APP_CONTEXT_INTERNAL *f_poAppContext )
{
    if ( f_poAppContext != NULL )
    {
        if ( f_poAppContext->fSecStoreGlobalContextOpen )
        {
            DRM_SST_CloseKey( &f_poAppContext->oSecStoreGlobalContext, &f_poAppContext->oDatastoreHDS, NULL, NULL  );

            f_poAppContext->fSecStoreGlobalContextOpen = FALSE;
        }
    }
}

/*
** Create the Deivce store/
*/
static DRM_RESULT DRM_CALL _CreateDeviceStore(
    IN       DRM_VOID        *f_pOEMContext,
    IN const DRM_WCHAR       *f_pwszDeviceStoreName,
    IN       DRM_DST         *f_pDatastore )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD  dwHDSInitSizeKB = 0;
    DRM_DWORD  dwHDSBlockSize = 0;
    DRM_DWORD  cbLen = DRMCRT_wcslen( f_pwszDeviceStoreName ) * SIZEOF( DRM_WCHAR );

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC__CreateDeviceStore );

    ChkArg( f_pDatastore != NULL );

    ChkDR( Oem_Hds_GetParams( f_pOEMContext,
                              f_pwszDeviceStoreName,
                              &dwHDSInitSizeKB,
                              NULL,
                              &dwHDSBlockSize ) );

    dr = DRM_DST_CreateStore( f_pOEMContext,
                              ( DRM_BYTE * )f_pwszDeviceStoreName,
                              cbLen,
                              dwHDSBlockSize,
                              eDRM_DSTBLKNUM_DWORD,
                              TRUE,
                              dwHDSInitSizeKB,
                              f_pDatastore );

    if ( dr == DRM_E_FILEWRITEERROR )
    {
        /*
        **  Some other process created the store concurrently. We can consider
        **  this operation as successful as store has been created. In the worst
        **  subsequent call to Open store will fail.
        */
        dr = DRM_SUCCESS;
    }

    ChkDR( dr );

ErrorExit:

    if ( DRM_FAILED( dr ) )
    {
        TRACE( ( "_CreateDeviceStore( ): Device store '%S' cannot be created.\n",
               f_pwszDeviceStoreName ) );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _IsEmptyV41Header
**
** Synopsis: Function that checks whether the WRMHEADER string in
**           appplication context is an empty V4.1 header.
**
** Arguments:
**
** [f_poAppContextInternal] -- Pointer to a DRM App context initialized via
**                             Drm_Initialize.
** [f_pfEmptyV41Header]     -- Out parameter to contain the result.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_BOOL _IsEmptyV41Header(
    IN  DRM_APP_CONTEXT_INTERNAL  *f_poAppContextInternal,
    OUT DRM_BOOL                  *f_pfEmptyV41Header )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poAppContextInternal != NULL );
    ChkArg( f_pfEmptyV41Header != NULL );

    *f_pfEmptyV41Header = FALSE;

    /* An empty V4.1 header contains the header string identical to g_dstrEmptyWRMHeaderV4_1. */
    if(   f_poAppContextInternal->eHeaderInContext == DRM_CSP_V4_1_HEADER
       && f_poAppContextInternal->cbHeaderData == ( g_dstrEmptyWRMHeaderV4_1.cchString * SIZEOF( DRM_WCHAR ) )
       && 0 == MEMCMP( f_poAppContextInternal->pwszHeaderData, g_dstrEmptyWRMHeaderV4_1.pwszString, f_poAppContextInternal->cbHeaderData ) )
    {
        *f_pfEmptyV41Header = TRUE;
    }

ErrorExit:
    return dr;
}

/* This function is used by functions in other files. */
DRM_API DRM_RESULT DRM_CALL _SetupLicEvalObjectToShare(
    IN DRM_APP_CONTEXT_INTERNAL *f_poAppContextInternal )
{
    DRM_RESULT  dr                                 = DRM_SUCCESS;
    DRM_BYTE    bBBVer[ __CB_DECL( VERSION_LEN ) ] = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC__SetupLicEvalObjectToShare );

    ZEROMEM( &f_poAppContextInternal->oLicEvalContext, SIZEOF( DRM_LICEVAL_CONTEXT ) );

    MEMCPY( bBBVer,
            f_poAppContextInternal->oBlackBoxContext.cachedCertValues.m_BBCompatibilityCert.pk.version,
            VERSION_LEN );

    ChkDR( DRM_UTL_GetVersionAsString( bBBVer, f_poAppContextInternal->wszBBVerString ) );

    f_poAppContextInternal->oLicEvalContext.dstrBBVer.pwszString = f_poAppContextInternal->wszBBVerString;

    DRM_STR_StringCchLengthW( f_poAppContextInternal->wszBBVerString, VER_STRING_MAX_LEN, &f_poAppContextInternal->oLicEvalContext.dstrBBVer.cchString );

    f_poAppContextInternal->oLicEvalContext.dstrDRMVer = g_dstrReqTagPlayReadyClientVersionData;

    INIT_DRM_STRING( f_poAppContextInternal->oLicEvalContext.dstrContentHeader );

    if (   f_poAppContextInternal->eHeaderInContext == DRM_CSP_V2_HEADER
       ||  f_poAppContextInternal->eHeaderInContext == DRM_CSP_V2_4_HEADER
       ||  f_poAppContextInternal->eHeaderInContext == DRM_CSP_V4_HEADER
       ||  f_poAppContextInternal->eHeaderInContext == DRM_CSP_V4_1_HEADER
       ||  f_poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ )
    {
        DRM_BOOL fEmptyV41Header = FALSE;

        /* An empty V4.1 WRMHEADER doesn't have a KID in it. */
        ChkDR( _IsEmptyV41Header( f_poAppContextInternal, &fEmptyV41Header ) );
        if( fEmptyV41Header )
        {
            /* Set the KID to NULL GUID */
            MEMSET( &f_poAppContextInternal->KID, 0, SIZEOF( DRM_KID ) );
        }
        else
        {
            DRM_CONST_STRING dstrKID = EMPTY_DRM_STRING;

            DSTR_FROM_PB( &f_poAppContextInternal->oLicEvalContext.dstrContentHeader,
                           f_poAppContextInternal->pwszHeaderData,
                           f_poAppContextInternal->cbHeaderData );

            ChkDR( DRM_HDR_GetAttribute( &f_poAppContextInternal->oLicEvalContext.dstrContentHeader,
                                          NULL,
                                          DRM_HEADER_ATTRIB_KID,
                                         &dstrKID,
                                          0 ) );

            ChkDR( DRM_UTL_DecodeKID( &dstrKID, &f_poAppContextInternal->KID ) );
        }
    }
    else if ( f_poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID )
    {
        DRM_CONST_STRING dstrKID = EMPTY_DRM_STRING;

        DSTR_FROM_PB( &dstrKID,
                       f_poAppContextInternal->pwszHeaderData,
                       f_poAppContextInternal->cbHeaderData );

        ChkDR( DRM_UTL_DecodeKID( &dstrKID, &f_poAppContextInternal->KID ) );
    }
    else if ( f_poAppContextInternal->eHeaderInContext == DRM_CSP_V1_HEADER )
    {
        DRM_WCHAR         wszKID[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) ] = { 0 };
        DRM_CONST_STRING  dstrKID                                         = EMPTY_DRM_STRING;
        DRM_STRING_WINDOW dstrwKID                                        = EMPTY_DRM_STRING_WINDOW;
        DRM_V1Header     *pV1Header                                       = ( DRM_V1Header * )f_poAppContextInternal->pbDRMHeaderData;

        /* Make sure the size of encoded KID is no more than CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ). */
        dstrwKID.m_ichMin          = 0;
        dstrwKID.m_ichMaxExclusive = ( pV1Header->cbKeyID <= CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) ) ?
                                       pV1Header->cbKeyID :
                                       CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );

        dstrKID.cchString  = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );
        dstrKID.pwszString = wszKID;

        /* Convert to UTF-16 (DRM string). */
        ChkDR( DRM_STR_UTF8toDSTR_ByWindow( ( DRM_CHAR * )( pV1Header->pbKeyID ), &dstrwKID, ( DRM_STRING * )&dstrKID ) );

        ChkDR( DRM_UTL_DecodeKID( &dstrKID, &f_poAppContextInternal->KID ) );
    }
    else if ( f_poAppContextInternal->eHeaderInContext == DRM_CSP_KID )
    {
        DRM_CONST_STRING dstrKID = EMPTY_DRM_STRING;

        DSTR_FROM_PB( &dstrKID,
                       f_poAppContextInternal->pwszHeaderData,
                       f_poAppContextInternal->cbHeaderData );

        ChkDR( DRM_UTL_DecodeKID( &dstrKID, &f_poAppContextInternal->KID ) );
    }
    else
    {
        /* No content header is given. */
        ZEROMEM( f_poAppContextInternal->KID.rgb, SIZEOF( f_poAppContextInternal->KID.rgb ) );
    }

    /* Result is first DRM_SHA1_DIGEST_LEN bytes of pbDRMLicense. */
    ChkDR( DRM_SST_CreateGlobalStorePassword( f_poAppContextInternal->pOEMContext, f_poAppContextInternal->pbDRMLicense,
                                              ( DRM_BYTE * )&f_poAppContextInternal->oBlackBoxContext ) );

    /* Open the global key. */
    ChkDR( DRM_SST_OpenKeyTokens( &f_poAppContextInternal->oSecStoreGlobalContext,
                                   ( DRM_ID * )&g_rgbSecStoreGlobalName,
                                   NULL,
                                   f_poAppContextInternal->pbDRMLicense,
                                   DRM_SECURE_STORE_OPEN_ALWAYS,
                                   SECURE_STORE_GLOBAL_DATA,
                                  &f_poAppContextInternal->oDatastoreHDS ) );

    f_poAppContextInternal->fSecStoreGlobalContextOpen  =  TRUE;

    f_poAppContextInternal->oLicEvalContext.dwFlags     =  LICEVAL_OPEN_CHECK_SETTINGS;
    f_poAppContextInternal->oLicEvalContext.pcontextBBX = &f_poAppContextInternal->oBlackBoxContext;

    if (  f_poAppContextInternal->eHeaderInContext == DRM_CSP_V2_HEADER
       || f_poAppContextInternal->eHeaderInContext == DRM_CSP_V2_4_HEADER
       || f_poAppContextInternal->eHeaderInContext == DRM_CSP_V4_HEADER
       || f_poAppContextInternal->eHeaderInContext == DRM_CSP_V4_1_HEADER
       || f_poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ )
    {
        DSTR_FROM_PB( &f_poAppContextInternal->oLicEvalContext.dstrContentHeader,
                       f_poAppContextInternal->pwszHeaderData,
                       f_poAppContextInternal->cbHeaderData );
    }
    else
    {
        /* Either V1 header or no header is given. */
        INIT_DRM_STRING( f_poAppContextInternal->oLicEvalContext.dstrContentHeader );
    }

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        MEMCPY( &f_poAppContextInternal->oLicEvalContext.idCRLsCurrent, &f_poAppContextInternal->idCRLsCurrent, SIZEOF( DRM_REVLIST_ID_VALUES ) );
        f_poAppContextInternal->oLicEvalContext.idCRLsCurrent.fUpdatedRevocationList = FALSE;
        f_poAppContextInternal->oLicEvalContext.pContextRev                          = &f_poAppContextInternal->oRevContext;
        f_poAppContextInternal->oLicEvalContext.pbRevocationBuffer                   =  f_poAppContextInternal->pbRevocationBuffer;
        f_poAppContextInternal->oLicEvalContext.cbRevocationBuffer                   =  f_poAppContextInternal->cbRevocationBuffer;
    }

    f_poAppContextInternal->oLicEvalContext.pDatastore         = &f_poAppContextInternal->oDatastoreHDS;
    f_poAppContextInternal->oLicEvalContext.pcontextSSTGlobal  = &f_poAppContextInternal->oSecStoreGlobalContext;
    f_poAppContextInternal->oLicEvalContext.pcontextSSTLicense = &f_poAppContextInternal->rgcontextSST[0];

    if( DRM_CERTCACHE_IsCertCacheSupported()
     || DRM_REVOCATION_IsRevocationSupported() )
    {
        f_poAppContextInternal->oLicEvalContext.pcontextSSTRevocation = &f_poAppContextInternal->contextSSTCertCache;
    }
    else
    {
        f_poAppContextInternal->oLicEvalContext.pcontextSSTRevocation = NULL;
    }

    f_poAppContextInternal->oLicEvalContext.pcontextSync = &f_poAppContextInternal->contextSync;

    f_poAppContextInternal->oLicEvalContext.fGlobalSecStoreWritable         = FALSE;
    f_poAppContextInternal->oLicEvalContext.fLegacyAppInfoValid             = TRUE;
    f_poAppContextInternal->oLicEvalContext.certinfoAppLegacy.appSec        = f_poAppContextInternal->oBlackBoxContext.cachedCertValues.appSec;
    f_poAppContextInternal->oLicEvalContext.certinfoAppLegacy.appcd_subject = f_poAppContextInternal->oBlackBoxContext.cachedCertValues.appcd_subject;
    f_poAppContextInternal->oLicEvalContext.fAppInfoValid                   = TRUE;
    f_poAppContextInternal->oLicEvalContext.certinfoApp.appSec              = f_poAppContextInternal->oBlackBoxContext.cachedBCertValues.dwGroupSecurityLevel;
    f_poAppContextInternal->oLicEvalContext.certinfoApp.appcd_subject       = 0;
    f_poAppContextInternal->oLicEvalContext.certinfoSDK.appSec              = 3000;
    f_poAppContextInternal->oLicEvalContext.certinfoSDK.appcd_subject       = 0;

    f_poAppContextInternal->oLicEvalContext.fSSecureTimeDataLoaded    = FALSE;
    f_poAppContextInternal->oLicEvalContext.fSecureTimeDataValid      = FALSE;
    f_poAppContextInternal->oLicEvalContext.fInGracePeriod            = FALSE;
    f_poAppContextInternal->oLicEvalContext.u64GPStartTime            = DRM_UI64( 0 );

    f_poAppContextInternal->oLicEvalContext.pcontextSync              = &( f_poAppContextInternal->contextSync );

    if( DRM_METERING_IsMeteringSupported() )
    {
        f_poAppContextInternal->oLicEvalContext.pcontextMeterCert         = &( f_poAppContextInternal->oMeterCertContext );
    }

    /*
    ** Defaults to XML license structures until XMR is fully supported.
    */
    f_poAppContextInternal->oLicEvalContext.plicenseXMR               = &( f_poAppContextInternal->licenseXMR[ 0 ] );
    f_poAppContextInternal->oLicEvalContext.fLicenseIsXMR             = FALSE;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

static DRM_RESULT _PrepareForClientInfo(
    IN DRM_APP_CONTEXT_INTERNAL *f_poAppContextInternal )
{
    DRM_RESULT       dr        = DRM_SUCCESS;
    DRM_CONST_STRING dstrValue = EMPTY_DRM_STRING;

    /* Get the device certificate. */
    DSTR_FROM_PB( &f_poAppContextInternal->licensechallenge.dstrDeviceCert,
                   f_poAppContextInternal->pbDRMLicense,
                   f_poAppContextInternal->cbDRMLicense );

    ChkDR( DRM_DDC_GetXMLDeviceCertificate(
                f_poAppContextInternal->pOEMContext,
                &f_poAppContextInternal->oKeyFileContext,
                ( DRM_STRING * )&f_poAppContextInternal->licensechallenge.dstrDeviceCert,
                0,
                &f_poAppContextInternal->oBlackBoxContext.CryptoContext ) );

    /* Get the security level. */
    ChkDR ( DRM_DCP_GetAttribute(  NULL,
                                  &f_poAppContextInternal->licensechallenge.dstrDeviceCert,
                                   DRM_DEVCERT_GROUPSECURITYLEVEL,
                                   NULL,
                                  &dstrValue ) );

    ChkDR ( DRMCRT_wcsntol( dstrValue.pwszString,
                            dstrValue.cchString,
                            ( DRM_LONG * )&f_poAppContextInternal->licensechallenge.levelAppSecurity ) );

    /* Get the subject id. */
    ChkDR ( DRM_DCP_GetAttribute(  NULL,
                                  &f_poAppContextInternal->licensechallenge.dstrDeviceCert,
                                   DRM_DEVCERT_SUBJECTID,
                                   NULL,
                                  &dstrValue ) );

    ChkDR ( DRMCRT_wcsntol( dstrValue.pwszString,
                            dstrValue.cchString,
                            ( DRM_LONG * )&f_poAppContextInternal->licensechallenge.idSubject ) );

    ChkDR( DRM_BBX_GetClientId( &f_poAppContextInternal->licensechallenge.clientid,
                                &f_poAppContextInternal->oBlackBoxContext ) );

    f_poAppContextInternal->licensechallenge.pwszBBVer   = f_poAppContextInternal->oBlackBoxContext.cachedCertValues.wszLegacyVersion;
    f_poAppContextInternal->licensechallenge.pcontextBBX = &f_poAppContextInternal->oBlackBoxContext;

ErrorExit:

    return dr;

}

/**********************************************************************
**
** Function:    _LoadHeaderFromSyncStore
**
** Synopsis:    This function loads a content header (V2/V2.4/V4) from
**              the sync store.
**              If there is only a KID specified in the DRM app context.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_SYNC_ENTRYNOTFOUND if content header
**                          cannot be found.
**
**********************************************************************/
static DRM_RESULT _LoadHeaderFromSyncStore(
    IN DRM_APP_CONTEXT *f_poAppContext )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_DWORD                 cchHeader            = DRM_MAX_HEADERDATASIZE / SIZEOF( DRM_WCHAR );
    DRM_STRING                dstrKID              = EMPTY_DRM_STRING;
    DRM_KID                   oKID                 = { 0 };
    DRM_CONST_STRING          dstrHeader           = EMPTY_DRM_STRING;
    DRM_CONTENT_SET_PROPERTY  eHeaderType          = DRM_CSP_HEADER_NOT_SET;
    eDRM_HEADER_VERSION       eHeaderVersion       = DRM_HEADER_VERSION_UNKNOWN;

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        ChkArg( f_poAppContext != NULL );
        ChkArg( poAppContextInternal->fStoreOpened );
        ChkArg( poAppContextInternal->eHeaderInContext == DRM_CSP_KID );

        /* We were given a KID only, and we need to lookup the header for this KID. */
        dstrKID.pwszString = ( DRM_WCHAR * )poAppContextInternal->pbDRMHeaderData;
        dstrKID.cchString  = poAppContextInternal->cbHeaderData / SIZEOF( DRM_WCHAR );

        ChkDR( DRM_UTL_DecodeKID( ( DRM_CONST_STRING * )&dstrKID, &oKID ) );

        ChkDR( DRM_SNC_GetSyncStoreEntry( &poAppContextInternal->contextSync,
                                          &oKID,
                                           NULL,
                                           dstrKID.pwszString,
                                          &cchHeader ) );

        if ( cchHeader == 0 )
        {
            /* There was no content header in the sync store. */
            ChkDR( DRM_E_SYNC_ENTRYNOTFOUND );
        }

        /* We've now got a full context header. */
        poAppContextInternal->cbHeaderData = cchHeader * SIZEOF( DRM_WCHAR );

        /* Let's check the header's version */
        dstrHeader.pwszString = ( DRM_WCHAR * )poAppContextInternal->pbDRMHeaderData;
        dstrHeader.cchString = cchHeader;

        ChkDR( DRM_HDR_GetHeaderVersion( &dstrHeader, &eHeaderVersion ) );

        /*
        ** Convert the Version Enum to the CSP value
        */
        switch( eHeaderVersion )
        {
        case DRM_HEADER_VERSION_2:
            eHeaderType = DRM_CSP_V2_HEADER;
            break;
        case DRM_HEADER_VERSION_2_4:
            eHeaderType = DRM_CSP_V2_4_HEADER;
            break;
        case DRM_HEADER_VERSION_4:
            eHeaderType = DRM_CSP_V4_HEADER;
            break;
        case DRM_HEADER_VERSION_4_1:
            eHeaderType = DRM_CSP_V4_1_HEADER;
            break;
        default:
            /* Assert for supported header formats */
            DRMASSERT( !"Unsupported Header Type" );
        }

        poAppContextInternal->eHeaderInContext  = eHeaderType;
        poAppContextInternal->pwszHeaderData    = ( DRM_WCHAR * )dstrHeader.pwszString;
        poAppContextInternal->pbEmbeddedStore   = NULL;
        poAppContextInternal->cbEmbeddedStore   = 0;
        poAppContextInternal->cbPROData         = 0;
        poAppContextInternal->cbTotalHeaderData = poAppContextInternal->cbHeaderData;
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _IsV2LicenseResponse
**
** Synopsis: Function that checks whether a license response is a V2
**           license response.
**
** Arguments:
**
** [f_pbResponse]           -- Pointer to a buffer that contains the
**                             response from the license server.
** [f_cbResponse]           -- Size ( number of bytes ) of the response
**                             from the license server.
**
** Returns:                 TRUE if it is a V2 response.
**                          FALSE if it is not a V2 response.
**
**********************************************************************/
static DRM_BOOL _IsV2LicenseResponse(
    IN DRM_BYTE *f_pbResponse,
    IN DRM_DWORD f_cbResponse )
{
    DRM_RESULT    dr                = DRM_SUCCESS;
    DRM_SUBSTRING dasstrLicenseList = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrLicenseData = EMPTY_DRM_SUBSTRING;

    ChkArg( f_pbResponse != NULL && f_cbResponse > 0 );

    /*
    ** If the response does not begin with a '<' character, treat it as
    ** a V2 response, since V2 response may be an error message that is
    ** not XML at all.
    */
    if ( f_pbResponse[ 0 ] == '<' )
    {
        dasstrLicenseData.m_ich = 0;
        dasstrLicenseData.m_cch = f_cbResponse;

        ChkDR( DRM_XML_GetNodeA(  ( DRM_CHAR * )f_pbResponse,
                                 &dasstrLicenseData,
                                 &g_adstrLicenseRespTag,
                                  NULL,
                                  NULL,
                                  0,
                                  NULL,
                                 &dasstrLicenseList ) );
    }

ErrorExit:

    return DRM_SUCCEEDED( dr );
}

/**********************************************************************
**
** Function:    _InitOpaqueBufferPointers
**
** Synopsis:    Sets DRM_APP_CONTEXT_INTERNAL pointers for the OpaqueBuffer
**
** Arguments:
**
** [f_poAppContextInternal] -- DRM Application context
** [f_pbOpaqueBuffer]       -- OpaqueBuffer for header and license
** [f_cbOpaqueBuffer]       -- Size of OpaqueBuffer pointed to by f_pbOpaqueBuffer
**
**********************************************************************/
static DRM_VOID _InitOpaqueBufferPointers(
    __in                            DRM_APP_CONTEXT_INTERNAL    *f_poAppContextInternal,
    __in_bcount( f_cbOpaqueBuffer ) DRM_BYTE                    *f_pbOpaqueBuffer,
    __in                            DRM_DWORD                    f_cbOpaqueBuffer )
{
    DRMASSERT( f_poAppContextInternal != NULL );
    f_poAppContextInternal->pbOpaqueBuffer = f_pbOpaqueBuffer;
    f_poAppContextInternal->cbOpaqueBuffer = f_cbOpaqueBuffer;
    f_poAppContextInternal->pbDRMHeaderData = f_pbOpaqueBuffer;
    f_poAppContextInternal->cbDRMHeaderData = f_cbOpaqueBuffer - DWORD_ALIGNED_SIZE( DRM_MAX_LICENSESIZE );
    f_poAppContextInternal->pbDRMLicense = f_pbOpaqueBuffer + f_cbOpaqueBuffer - DWORD_ALIGNED_SIZE( DRM_MAX_LICENSESIZE );
    f_poAppContextInternal->cbDRMLicense = DWORD_ALIGNED_SIZE( DRM_MAX_LICENSESIZE );
}

/**********************************************************************
**
** Function:    Drm_Initialize
**
** Synopsis:    Initializes DRM. The context that is initialized after
**              this call can be used in later calls to most DRM functions
**              to perform a varety of DRM related tasks.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_pOEMContext]          -- Optional pointer to OEM specific data
** [f_pbOpaqueBuffer]       -- OpaqueBuffer for header and license of minimum size MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE
** [f_cbOpaqueBuffer]       -- Size of OpaqueBuffer pointed to by f_pbOpaqueBuffer
** [f_pdstrDeviceStoreName] -- Path DRM should use as the current store.
**
** Returns:                 DRM_SUCCESS on success or failed with.
**                          DRM_E_NEEDDEVCERTINDIV if the device needs to be
**                          individualized.
**                          DRM_E_INVALIDARG if any parameter is incorrect
**                          or other errors found in drmresults.h.
**
** Notes:                   The f_poAppContext should be uninitialized with
**                          a call to Drm_Uninitialize.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Initialize(
    __in                                    DRM_APP_CONTEXT     *f_poAppContext,
    __in_opt                                DRM_VOID            *f_pOEMContext,
    __in_bcount( f_cbOpaqueBuffer )         DRM_BYTE            *f_pbOpaqueBuffer,
    __in                                    DRM_DWORD            f_cbOpaqueBuffer,
    __in                              const DRM_CONST_STRING    *f_pdstrDeviceStoreName )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_DWORD                 dwHDSGrowSizeKB      = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Initialize );
    DRM_PERF_LogMicrosecondCount( eDRM_PERF_LogTag_Enter, eDRM_PERF_LogFunction_Drm_Initialize );

    ChkArg( f_poAppContext != NULL );

    /*
    ** Before proceeding, zero out everything. This will ensure cleanup on error
    ** proceeds properly.
    */
    ZEROMEM( f_poAppContext, SIZEOF( DRM_APP_CONTEXT ) );

    poAppContextInternal->fStoreOpened = FALSE;

    poAppContextInternal->pOEMContext = f_pOEMContext;

    poAppContextInternal->fResponseSignatureChecked = FALSE;
    poAppContextInternal->dwLastResponseProtocol    = DRM_SOAPXML_PROTOCOL_UNKNOWN;

    /*
    ** Now continue with error checking.
    */
    ChkDRMString( f_pdstrDeviceStoreName );

    /* Copy Device Store file name in DRM App Context. */
    if ( f_pdstrDeviceStoreName->cchString > DRM_MAX_PATH )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    /*
    ** Although for perf it would be ideal to not flush the cache here and instead rely
    ** on the caller to know when the cache may become invalid (ie because the HDS file
    ** has been switched over to a different one) it is safer to just force the caches
    ** to be flushed.
    */
    Drm_ClearAllCaches();

    /*
    ** Initializes pointer to OEM context in BIG_NUM context.
    ** Pointer to BIG_NUM is passed to all crypto and bignum functions
    ** Thus OEM context becames available to all  bignum, in particular to
    ** "random_bytes" function in bignum code.
    **
    */
    ZEROMEM( &poAppContextInternal->oBlackBoxContext.CryptoContext, SIZEOF( DRM_CRYPTO_CONTEXT ) );
    OEM_ECC_PKInit( &poAppContextInternal->oBlackBoxContext.CryptoContext.rgbCryptoContext, f_pOEMContext );

    poAppContextInternal->oBlackBoxContext.pOEMContext = f_pOEMContext;

    ChkDR( DRM_KF_Initialize( f_pOEMContext, &poAppContextInternal->oKeyFileContext ) );

    poAppContextInternal->oBlackBoxContext.pKeyFileContext = &poAppContextInternal->oKeyFileContext;

    /*
    ** Initialize OpaqueBuffer
    */
    ChkArg( f_pbOpaqueBuffer != NULL );
    ChkArg( f_cbOpaqueBuffer >= MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE );
    DRMASSERT( MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE >= DWORD_ALIGNED_SIZE( DRM_MAX_LICENSESIZE ) );
    ZEROMEM( f_pbOpaqueBuffer, f_cbOpaqueBuffer );
    _InitOpaqueBufferPointers( poAppContextInternal, f_pbOpaqueBuffer, f_cbOpaqueBuffer );

    /*
    ** Initialize all device certificates
    ** If activation is supported this call can fail and return DRM_E_ACTIVATION_REQUIRED
    */
    dr = DRM_DDC_Certificates_Initialize( f_poAppContext, f_pOEMContext );
    if( dr == DRM_E_ACTIVATION_REQUIRED )
    {
        /*
        ** If activation is required there are 2 possibilities.
        ** In the future when reactivation is supported we should bubble up the
        ** error code so existing keysfiles will be sent to the activation service.
        **
        ** For now, the machine state is unusable so we need to clean everything up
        ** before returning activation error codes.
        ** NOTE: If we are unable to cleanup these files we are in an hard fail state
        **
        */

        {
            OEM_FILEHDL hFile = OEM_INVALID_HANDLE_VALUE;
            if( OEM_INVALID_HANDLE_VALUE != ( hFile = Oem_File_Open(
                NULL,
                f_pdstrDeviceStoreName->pwszString,
                OEM_GENERIC_READ,
                OEM_FILE_SHARE_READ|OEM_FILE_SHARE_WRITE,
                OEM_OPEN_EXISTING,
                0 ) ) )
            {
                (void)Oem_File_Close( hFile );
                if( !Oem_File_Delete( f_pdstrDeviceStoreName->pwszString ) )
                {
                    ChkDR( DRM_E_FILEWRITEERROR );
                }
            }
        }
        ChkDR( Oem_Keyfile_Delete( ) );
        ChkDR( DRM_E_ACTIVATION_REQUIRED );
    }
    ChkDR( dr );

    /*
    ** Complete initialization of black box.
    */

    poAppContextInternal->oBlackBoxContext.pDomainStoreContext = NULL;
    DRMASSERT( g_dstrReqTagPlayReadyClientVersionData.cchString < NO_OF(poAppContextInternal->szClientVersion) );
    DRM_UTL_DemoteUNICODEtoASCII(
        g_dstrReqTagPlayReadyClientVersionData.pwszString,
        poAppContextInternal->szClientVersion,
        NO_OF(poAppContextInternal->szClientVersion) );
    DRMASSERT( poAppContextInternal->szClientVersion[NO_OF(poAppContextInternal->szClientVersion)-1] == '\0' );
    DRMASSERT( DRMCRT_strlen(poAppContextInternal->szClientVersion) == g_dstrReqTagPlayReadyClientVersionData.cchString );
    poAppContextInternal->oClientInfo.m_dastrClientVersion.pszString = poAppContextInternal->szClientVersion;
    poAppContextInternal->oClientInfo.m_dastrClientVersion.cchString = g_dstrReqTagPlayReadyClientVersionData.cchString;
    poAppContextInternal->oBlackBoxContext.pClientInfo = &poAppContextInternal->oClientInfo;
    poAppContextInternal->oBlackBoxContext.fInited = TRUE;
    poAppContextInternal->eHeaderInContext = DRM_CSP_HEADER_NOT_SET;
    poAppContextInternal->pwszHeaderData   = ( DRM_WCHAR * )poAppContextInternal->pbDRMHeaderData;
    poAppContextInternal->cbHeaderData     = 0;
    poAppContextInternal->pbEmbeddedStore  = NULL;
    poAppContextInternal->cbEmbeddedStore  = 0;

    ChkDR( DRM_STR_StringCchCopyNW( ( DRM_WCHAR * )poAppContextInternal->pbDRMLicense,
                                    poAppContextInternal->cbDRMLicense / SIZEOF( DRM_WCHAR ),
                                    f_pdstrDeviceStoreName->pwszString,
                                    f_pdstrDeviceStoreName->cchString ) );

    /* Initialize device store. Open if not yet opened. Create if not yet created. */
    ChkDR( DRM_DST_Init( eDRM_DST_HDS,
                         ( DRM_DST_CONTEXT * )&poAppContextInternal->oContextHDS,
                         SIZEOF( poAppContextInternal->oContextHDS ),
                        &poAppContextInternal->oDatastoreHDS ) );

    ChkDR( Oem_Hds_GetParams( f_pOEMContext,
                              ( DRM_WCHAR * )poAppContextInternal->pbDRMLicense,
                              NULL,
                              &dwHDSGrowSizeKB,
                              NULL ) );

    dr = DRM_DST_OpenStore( f_pOEMContext,
                            poAppContextInternal->pbDRMLicense,
                            poAppContextInternal->cbDRMLicense,
                            dwHDSGrowSizeKB,
                            &poAppContextInternal->oDatastoreHDS );

    if ( dr == DRM_E_FILENOTFOUND )   /* Store does not exist. */
    {
        ChkDR( _CreateDeviceStore(  f_pOEMContext,
                                    ( DRM_WCHAR * )poAppContextInternal->pbDRMLicense,
                                   &( poAppContextInternal->oDatastoreHDS ) ) );

        dr = DRM_DST_OpenStore( f_pOEMContext,
                                poAppContextInternal->pbDRMLicense,
                                poAppContextInternal->cbDRMLicense,
                                dwHDSGrowSizeKB,
                                &poAppContextInternal->oDatastoreHDS );
    }

    ChkDR( dr );

    poAppContextInternal->fStoreOpened = TRUE;

    /*
    ** Open both the XML and XMR stores.
    */
    ChkDR( DRM_LST_Open( &poAppContextInternal->oLicStoreContextXML,
                         &poAppContextInternal->oDatastoreHDS,
                          eDRM_LICENSE_STORE_XML ) );

    poAppContextInternal->fLicStoreOpenXML = TRUE;

    ChkDR( DRM_LST_Open( &poAppContextInternal->oLicStoreContextXMR,
                         &poAppContextInternal->oDatastoreHDS,
                          eDRM_LICENSE_STORE_XMR ) );

    poAppContextInternal->fLicStoreOpenXMR = TRUE;

    /*
    ** Initialize the embedded store. This is only opened during embedding
    */
    ChkDR( DRM_DST_Init( eDRM_DST_EMBEDDED,
                         ( DRM_DST_CONTEXT * )&poAppContextInternal->oContextEST,
                         SIZEOF( poAppContextInternal->oContextEST ),
                         &poAppContextInternal->oDatastoreEST ) );

    ChkDR( DRM_DOMST_OpenStore( &poAppContextInternal->oDatastoreHDS,
                                &poAppContextInternal->oDomainStoreContext ) );

    poAppContextInternal->oBlackBoxContext.pDomainStoreContext = &poAppContextInternal->oDomainStoreContext;

    ChkDR( DRM_NST_Open( f_pOEMContext,
                         poAppContextInternal->rgbNSTBuffer,
                         SIZEOF( poAppContextInternal->rgbNSTBuffer ),
                         &poAppContextInternal->oLicStoreContextNST,
                         &poAppContextInternal->oNonceStoreContext ) );

    ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

    /* Explicitly make the global secure store writable. */
    poAppContextInternal->oLicEvalContext.fGlobalSecStoreWritable = TRUE;

    if ( DRM_CLK_IsSecureClockSupported()
      && poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags & DRM_FEATURE_SECURE_CLOCK )
    {
        ChkDR( DRM_CLK_CheckSecureClock( f_pOEMContext, poAppContextInternal ) );
    }

    if ( poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags & DRM_FEATURE_ANTI_ROLLBACK_CLOCK )
    {
        ChkDR( DRM_LIC_CheckClockRollback(
                &poAppContextInternal->oLicEvalContext,
                &poAppContextInternal->oLicStoreContextXML,
                &poAppContextInternal->oLicStoreContextXMR,
                &poAppContextInternal->oLicEnum[0],
                 poAppContextInternal->pbOpaqueBuffer + __CB_DECL( poAppContextInternal->cbTotalHeaderData ),
                 poAppContextInternal->cbOpaqueBuffer - poAppContextInternal->cbTotalHeaderData,
                &poAppContextInternal->oDatastoreHDS,
                 poAppContextInternal->pbDRMLicense,
                 poAppContextInternal->cbDRMLicense ) );
    }

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        ChkDR( DRM_SNC_OpenStore( &poAppContextInternal->oDatastoreHDS, &poAppContextInternal->contextSync ) );
    }

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        ChkDR( DRM_RVS_InitRevocationStore( &poAppContextInternal->oDatastoreHDS,
                                            &poAppContextInternal->contextSSTCertCache,
                                            &poAppContextInternal->oBlackBoxContext,
                                            &poAppContextInternal->oRevContext ) );
    }

    if( DRM_METERING_IsMeteringSupported() )
    {
        ChkDR( DRM_MCS_OpenStore( &poAppContextInternal->oDatastoreHDS,
                                   poAppContextInternal->pbDRMLicense,
                                   DRM_MAX_LICENSESIZE,
                                  &poAppContextInternal->oMeterCertContext ) );
    }

ErrorExit:

    _FreeDrmAppInternalContexts( poAppContextInternal );

    if ( DRM_FAILED( dr ) && poAppContextInternal != NULL )
    {
        Drm_Uninitialize( f_poAppContext );
    }

    ChkECC( ECC_Drm_Initialize, dr );

    DRM_PERF_LogMicrosecondCount( eDRM_PERF_LogTag_Exit, eDRM_PERF_LogFunction_Drm_Initialize );
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    Drm_ResizeOpaqueBuffer
**
** Synopsis:    Allows the caller to resize the OpaqueBuffer, for example
**              after receiving DRM_E_BUFFERTOOSMALL from Drm_Content_SetProperty or Drm_Reader_Bind
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_pbOpaqueBuffer]       -- OpaqueBuffer for header and license of minimum size MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE
** [f_cbOpaqueBuffer]       -- Size of OpaqueBuffer pointed to by f_pbOpaqueBuffer
**                             The new buffer must be LARGER than the original buffer.
**
** Returns:                 DRM_SUCCESS on success or failed with.
**                          DRM_E_INVALIDARG if any parameter is incorrect.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_ResizeOpaqueBuffer(
    __in                                    DRM_APP_CONTEXT     *f_poAppContext,
    __in_bcount( f_cbOpaqueBuffer )         DRM_BYTE            *f_pbOpaqueBuffer,
    __in                                    DRM_DWORD            f_cbOpaqueBuffer )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_pbOpaqueBuffer  != NULL );
    ChkArg( poAppContextInternal->cbOpaqueBuffer >= MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE );
    ChkArg( f_cbOpaqueBuffer > poAppContextInternal->cbOpaqueBuffer );

    ZEROMEM( f_pbOpaqueBuffer, f_cbOpaqueBuffer );

    /* Copy any existing data into the larger buffer */
    MEMCPY( f_pbOpaqueBuffer, poAppContextInternal->pbOpaqueBuffer, poAppContextInternal->cbOpaqueBuffer );

    _InitOpaqueBufferPointers( poAppContextInternal, f_pbOpaqueBuffer, f_cbOpaqueBuffer );

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    Drm_GetOpaqueBuffer
**
** Synopsis:    Allows the caller to retrieve the OpaqueBuffer
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_ppbOpaqueBuffer]      -- OpaqueBuffer set when calling Drm_Initialize or Drm_ResizeOpaqueBuffer
**
** Returns:                 DRM_SUCCESS on success or failed with.
**                          DRM_E_INVALIDARG if any parameter is incorrect.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_GetOpaqueBuffer(
    __in                                    DRM_APP_CONTEXT     *f_poAppContext,
    __out                                   DRM_BYTE           **f_ppbOpaqueBuffer )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext    != NULL );
    ChkArg( f_ppbOpaqueBuffer != NULL );

    *f_ppbOpaqueBuffer = poAppContextInternal->pbOpaqueBuffer;

ErrorExit:
    return dr;
}



/**********************************************************************
**
** Function:    Drm_Uninitialize
**
** Synopsis:    Allows DRM to shutdown any allocated resources.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
**
** Returns:                 N/A
**
**********************************************************************/
DRM_API DRM_VOID DRM_CALL Drm_Uninitialize(
    IN DRM_APP_CONTEXT *f_poAppContext )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Uninitialize );

    ChkArg( f_poAppContext != NULL );

    if ( poAppContextInternal->fBindThenCommit )
    {
        poAppContextInternal->fBindThenCommit = FALSE;
    }

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        ( DRM_VOID )DRM_SNC_CloseStore( &poAppContextInternal->contextSync );
    }

    if( DRM_METERING_IsMeteringSupported() )
    {
        ( DRM_VOID )DRM_MCS_CloseStore( &poAppContextInternal->oMeterCertContext );
    }

    if ( poAppContextInternal->fLicStoreOpenXML )
    {
        ( DRM_VOID )DRM_LST_Close( &poAppContextInternal->oLicStoreContextXML );
        poAppContextInternal->fLicStoreOpenXML = FALSE;
    }
    if ( poAppContextInternal->fLicStoreOpenXMR )
    {
        ( DRM_VOID )DRM_LST_Close( &poAppContextInternal->oLicStoreContextXMR );
        poAppContextInternal->fLicStoreOpenXMR = FALSE;
    }

    ( DRM_VOID )DRM_DOMST_CloseStore( &poAppContextInternal->oDomainStoreContext );

    ( DRM_VOID )DRM_NST_Close( &poAppContextInternal->oNonceStoreContext );

    if ( poAppContextInternal->fStoreOpened )
    {
        ( DRM_VOID )DRM_DST_CloseStore( &poAppContextInternal->oDatastoreHDS );
    }

    ( DRM_VOID )DRM_DST_Uninit( &poAppContextInternal->oDatastoreHDS );
    ( DRM_VOID )DRM_DST_Uninit( &poAppContextInternal->oDatastoreEST );

    ( DRM_VOID )DRM_KF_Uninitialize( &poAppContextInternal->oKeyFileContext );

    ( DRM_VOID )DRM_XMLHASH_Cleanup();

    ZEROMEM( &poAppContextInternal->oDatastoreHDS,
              SIZEOF( poAppContextInternal->oDatastoreHDS ) );
    ZEROMEM( &poAppContextInternal->oContextHDS,
              SIZEOF( poAppContextInternal->oContextHDS ) );
    ZEROMEM( &poAppContextInternal->oDatastoreEST,
              SIZEOF( poAppContextInternal->oDatastoreEST ) );
    ZEROMEM( &poAppContextInternal->oContextEST,
              SIZEOF( poAppContextInternal->oContextEST ) );

ErrorExit:

    ChkECC( ECC_Drm_Uninitialize, dr );

    DRM_PROFILING_LEAVE_SCOPE;

    return;
}

/**********************************************************************
**
** Function:    Drm_Reinitialize
**
** Synopsis:    Cleans up the header information stored in the DRM context.
**              Useful when loading a mix of content with v1 and v2 headers.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the argument is invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Reinitialize(
    IN DRM_APP_CONTEXT  *f_poAppContext )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext != NULL );

    poAppContextInternal->eHeaderInContext          = DRM_CSP_HEADER_NOT_SET;
    poAppContextInternal->fCanBind                  = FALSE;
    poAppContextInternal->fResponseSignatureChecked = FALSE;
    poAppContextInternal->dwLastResponseProtocol    = DRM_SOAPXML_PROTOCOL_UNKNOWN;

    ZEROMEM( &poAppContextInternal->KID,            SIZEOF( poAppContextInternal->KID ) );
    ZEROMEM( poAppContextInternal->oBindInfo,       SIZEOF( poAppContextInternal->oBindInfo ) );
    ZEROMEM( poAppContextInternal->licenseXMR,      SIZEOF( poAppContextInternal->licenseXMR ) );

    poAppContextInternal->dwChainDepth              = 0;

    ZEROMEM( poAppContextInternal->pbDRMHeaderData, poAppContextInternal->cbDRMHeaderData );

    poAppContextInternal->pwszHeaderData            = NULL;
    poAppContextInternal->cbHeaderData              = 0;
    poAppContextInternal->pbEmbeddedStore           = NULL;
    poAppContextInternal->cbEmbeddedStore           = 0;
    poAppContextInternal->cbPROData                 = 0;
    poAppContextInternal->cbTotalHeaderData         = 0;

    poAppContextInternal->fBindThenCommit           = FALSE;

    /* Close and reopen the nonce (in-memory) license store to make sure it is empty */
    dr = DRM_NST_Close( &poAppContextInternal->oNonceStoreContext );
    DRMASSERT( DRM_SUCCEEDED( dr ) );
    dr = DRM_SUCCESS;
    ZEROMEM( poAppContextInternal->rgbNSTBuffer, SIZEOF( poAppContextInternal->rgbNSTBuffer ) );
    dr = DRM_NST_Open( poAppContextInternal->pOEMContext,
                       poAppContextInternal->rgbNSTBuffer,
                       SIZEOF( poAppContextInternal->rgbNSTBuffer ),
                       &poAppContextInternal->oLicStoreContextNST,
                       &poAppContextInternal->oNonceStoreContext );
    DRMASSERT( DRM_SUCCEEDED( dr ) );
    dr = DRM_SUCCESS;

ErrorExit:
    ChkECC( ECC_Drm_Reinitialize, dr );
    return dr;
}

/**********************************************************************
**
** Function:    Drm_Revocation_SetBuffer
**
** Synopsis:    Passes in a buffer to be used by revocation code.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_pbRevocationBuffer]   -- Pointer to a buffer to be set as revocation
**                             buffer.
** [f_cbRevocationBuffer]   -- Size of the buffer(in bytes) to be set as
**                             revocation buffer.
**
** Returns:                    DRM_SUCCESS on success.
**                             DRM_E_INVALIDARG if a parameter is incorrect.
**                             Other errors found in drmresults.h.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Revocation_SetBuffer(
    __inout                                 DRM_APP_CONTEXT *f_poAppContext,
    __in_bcount_opt( f_cbRevocationBuffer ) DRM_BYTE        *f_pbRevocationBuffer,
    __in                                    DRM_DWORD        f_cbRevocationBuffer )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Revocation_SetBuffer );

    ChkArg( f_poAppContext != NULL );

    if ( f_pbRevocationBuffer == NULL )
    {
        ChkArg( f_cbRevocationBuffer == 0 );

        /*
        ** If we're zeroing out the revocation buffer, just free the internal
        ** context associated with it and exit.
        */
        poAppContextInternal->pbRevocationBuffer = f_pbRevocationBuffer;
        poAppContextInternal->cbRevocationBuffer = f_cbRevocationBuffer;
    }
    else
    {
        /* Result is first DRM_SHA1_DIGEST_LEN bytes of pbDRMLicense. */
        ChkDR( DRM_SST_CreateGlobalStorePassword( poAppContextInternal->pOEMContext, poAppContextInternal->pbDRMLicense,
                                                  ( DRM_BYTE * ) &poAppContextInternal->oBlackBoxContext ) );

        /* Open the global key. */
        ChkDR( DRM_SST_OpenKeyTokens( &poAppContextInternal->oSecStoreGlobalContext,
                                       ( DRM_ID * )&g_rgbSecStoreGlobalName,
                                       NULL,
                                       poAppContextInternal->pbDRMLicense,
                                       DRM_SECURE_STORE_OPEN_ALWAYS,
                                       SECURE_STORE_GLOBAL_DATA,
                                      &poAppContextInternal->oDatastoreHDS ) );

        poAppContextInternal->fSecStoreGlobalContextOpen         = TRUE;
        poAppContextInternal->pbRevocationBuffer                 = f_pbRevocationBuffer;
        poAppContextInternal->cbRevocationBuffer                 = f_cbRevocationBuffer;
        poAppContextInternal->oLicEvalContext.pbRevocationBuffer = f_pbRevocationBuffer;
        poAppContextInternal->oLicEvalContext.cbRevocationBuffer = f_cbRevocationBuffer;

        /*
        ** Load revocation versions from the DST and cache them for later use by
        ** Drm_Reader_Bind().
        */
        ChkDR( DRM_RVK_UpdateRevocationVersionsCache( &poAppContextInternal->oRevContext,
                                                      &poAppContextInternal->oBlackBoxContext.CryptoContext,
                                                      &poAppContextInternal->idCRLsCurrent,
                                                       poAppContextInternal->pbRevocationBuffer,
                                                       poAppContextInternal->cbRevocationBuffer,
                                                       NULL ) );
    }

ErrorExit:

    _FreeDrmAppInternalContexts( poAppContextInternal );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
**
** Function:    Drm_Revocation_GetBuffer
**
** Synopsis:    Gets the buffer used by revocation code.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_ppbRevocationBuffer]  -- Pointer to a pointer that will point to the
**                             returned revocation buffer.
** [f_pcbRevocationBuffer]  -- Pointer to a variable to receive the size of
**                             the returned revocation buffer in bytes.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if a parameter is incorrect.
**                          Other errors found in drmresults.h.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Revocation_GetBuffer(
    __inout                                      DRM_APP_CONTEXT  *f_poAppContext,
    __deref_out_bcount( *f_pcbRevocationBuffer ) DRM_BYTE        **f_ppbRevocationBuffer,
    __out                                        DRM_DWORD        *f_pcbRevocationBuffer )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg(  f_poAppContext != NULL
          && f_ppbRevocationBuffer != NULL
          && f_pcbRevocationBuffer != NULL );

    *f_ppbRevocationBuffer = poAppContextInternal->pbRevocationBuffer;
    *f_pcbRevocationBuffer = poAppContextInternal->cbRevocationBuffer;

ErrorExit:
    return dr;
}

/**********************************************************************
**
** Function:    Drm_Revocation_StoreRevListArray
**
** Synopsis:    Stores new revocation lists and rev info blobs.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_cRevocationLists]     -- Number of revocation lists and rev info blobs.
** [f_pRevocationLists]     -- One or more revocation lists and rev info blobs.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if a parameter is incorrect.
**                          Other errors found in drmresults.h.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Revocation_StoreRevListArray(
    __inout                           DRM_APP_CONTEXT *f_poAppContext,
    __in                              DRM_DWORD        f_cRevocationLists,
    __in_ecount( f_cRevocationLists ) DRM_RVK_LIST    *f_pRevocationLists )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg(  poAppContextInternal != NULL
          && f_pRevocationLists != NULL
          && f_cRevocationLists > 0 );

    ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

    ChkDR( DRM_RVK_StoreRevocationLists( &poAppContextInternal->oBlackBoxContext.CryptoContext,
                                         &poAppContextInternal->oRevContext,
                                          f_cRevocationLists,
                                          f_pRevocationLists,
                                         &poAppContextInternal->oLicEvalContext.appcert,
                                          poAppContextInternal->pbRevocationBuffer,
                                          poAppContextInternal->cbRevocationBuffer,
                                         &poAppContextInternal->idCRLsCurrent ) );

ErrorExit:

    _FreeDrmAppInternalContexts( poAppContextInternal );

    return dr;
}

/**********************************************************************
**
** Function:    Drm_Revocation_StorePackage
**
** Synopsis:    Stores the revocation package XML.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_pchPackage]           -- Pointer to a UTF8/ASCII buffer that contains the
**                             revocation package XML.
** [f_cchPackage]           -- Number of characters of the revocation package XML.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if a parameter is incorrect.
**                          Other errors found in drmresults.h.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Revocation_StorePackage(
    __inout                           DRM_APP_CONTEXT *f_poAppContext,
    __in_ecount( f_cchPackage ) const DRM_CHAR        *f_pchPackage,
    __in                              DRM_DWORD        f_cchPackage )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_LICRESP_CONTEXT_V3    oLicRespContext      = {0};
    DRM_ANSI_CONST_STRING     dastrResponse        = EMPTY_DRM_STRING;
    DRM_DWORD                 dwRevListProcessed   = 0;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( poAppContextInternal != NULL );
    ChkArg( f_pchPackage != NULL );
    ChkArg( f_cchPackage > 0 );

    ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

    oLicRespContext.m_poBBXContext        = &poAppContextInternal->oBlackBoxContext;
    oLicRespContext.m_poRevStoreContext   = &poAppContextInternal->oRevContext;
    oLicRespContext.m_poLicEvalContext    = &poAppContextInternal->oLicEvalContext;

    dastrResponse.pszString = f_pchPackage;
    dastrResponse.cchString = f_cchPackage;

    ChkDR( DRM_LA_ProcessRevocationPackage( &oLicRespContext,
                                            &dastrResponse,
                                            &dwRevListProcessed ) );

    if ( poAppContextInternal->oLicEvalContext.idCRLsCurrent.fUpdatedRevocationList )
    {
        MEMCPY( &poAppContextInternal->idCRLsCurrent,
                &poAppContextInternal->oLicEvalContext.idCRLsCurrent,
                SIZEOF( DRM_REVLIST_ID_VALUES ) );

        /*
        ** No need to clear the fUpdatedRevocationList flag - the lic eval
        ** object will be zeroed out before being populated again prior to
        ** this call.
        */
    }

ErrorExit:

    _FreeDrmAppInternalContexts( poAppContextInternal );

    return dr;
}

#if defined(_PREFAST_)
#pragma prefast(push)
#pragma prefast(disable:6326) /* Temporarily disable comparing a constant with a constant warning. */
#endif
/*********************************************************************
**
** Function: _AutoDetectHeader
**
** Synopsis: Check the header content and makes sophisticated guess
**           about the header type
**
** Arguments:
**
** [f_pbPropertyData]       -- Pointer to a buffer that contains the
**                             header content.
** [f_cbPropertyData]       -- Size ( number of bytes ) of the header
** [f_eProperty]            -- Header type or DRM_CSP_HEADER_NOT_SET
**                             if header type is not detected
**
** Returns:                 DRM_SUCCESS if drm header type was successfully
**                          detected.
**                          DRM_S_FALSE if drm header still unknown.
**                          DRM_E_INVALIDARG if incorrected parameters were
**                          specified.
**
**********************************************************************/
static DRM_RESULT _AutoDetectHeader(
    IN const DRM_BYTE                 *f_pbPropertyData,
    IN       DRM_DWORD                 f_cbPropertyData,
    OUT      DRM_CONTENT_SET_PROPERTY *f_peProperty )
{
    DRM_RESULT          dr                            = DRM_S_FALSE;
    DRM_CONST_STRING    dstrHeader                    = EMPTY_DRM_STRING;
    eDRM_HEADER_VERSION eHeaderVersion                = DRM_HEADER_VERSION_UNKNOWN;
    DRM_CSP_PLAYREADY_OBJ_WITH_KID_DATA *pPROWithKID  = NULL;

    ChkArg( f_pbPropertyData != NULL );
    ChkBOOL( f_cbPropertyData > 0, DRM_E_CH_INVALID_HEADER );
    ChkArg( f_peProperty != NULL );

    *f_peProperty = DRM_CSP_HEADER_NOT_SET;


    /* Validation for KID type header */
    if ( f_cbPropertyData == CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) * SIZEOF( DRM_WCHAR ) )
    {
        if( CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) * SIZEOF( DRM_WCHAR ) == SIZEOF( DRM_CSP_V1_HEADER_DATA ) )
        {
            /* The V1 struct is the same size as a V2 KID, this can happen on 64 bit systems */
            DRM_DWORD i;
            DRM_RESULT dr2 = DRM_SUCCESS;
            DRM_KID    kid;
            DRM_CONST_STRING dstrKID = EMPTY_DRM_STRING;

            for( i = 1; i < f_cbPropertyData; i += 2 )
            {
                if( GET_BYTE( f_pbPropertyData, i ) != 0 )
                {
                    /* KIDs should not have anything but 0 as their second Unicode bytes,
                    ** so it must be a V1 header
                    */
                    *f_peProperty = DRM_CSP_V1_HEADER;
                    dr = DRM_SUCCESS;
                    goto ErrorExit;
                }
            }

            dstrKID.pwszString = (DRM_WCHAR*)f_pbPropertyData;
            dstrKID.cchString = f_cbPropertyData / SIZEOF( DRM_WCHAR );
            dr2 = DRM_UTL_DecodeKID( &dstrKID, &kid );
            if( DRM_FAILED( dr2 ) )
            {
                /* Invalid KID,
                ** so it must be a V1 header
                */
                *f_peProperty = DRM_CSP_V1_HEADER;
                dr = DRM_SUCCESS;
                goto ErrorExit;
            }
        }

        *f_peProperty = DRM_CSP_KID;

        dr = DRM_SUCCESS;

        goto ErrorExit;
    }

    /* Validation for V1 type headers */
    /* Assumption: size of V1 header is to small for V2/V2.4/V4 headers */
    if ( f_cbPropertyData == SIZEOF( DRM_CSP_V1_HEADER_DATA ) )
    {
        *f_peProperty = DRM_CSP_V1_HEADER;

        dr = DRM_SUCCESS;

        goto ErrorExit;
    }

    /* Validation for PlayReady object */
    if ( DRM_PRO_IsValidObject( f_pbPropertyData, f_cbPropertyData ) == DRM_SUCCESS )
    {
        *f_peProperty = DRM_CSP_PLAYREADY_OBJ;

        dr = DRM_SUCCESS;

        goto ErrorExit;
    }

    /* Validation for PlayReady object with a separate KID */
    if ( f_cbPropertyData == SIZEOF( DRM_CSP_PLAYREADY_OBJ_WITH_KID_DATA ) )
    {
        pPROWithKID = (DRM_CSP_PLAYREADY_OBJ_WITH_KID_DATA*) f_pbPropertyData;
        if ( pPROWithKID->cbKeyID == CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) * SIZEOF( DRM_WCHAR )
          && DRM_PRO_IsValidObject( pPROWithKID->pbHeaderData, pPROWithKID->cbHeaderData ) )
        {
            *f_peProperty = DRM_CSP_PLAYREADY_OBJ_WITH_KID;

            dr = DRM_SUCCESS;

            goto ErrorExit;
        }
    }
    /* Validation for V2, V2.4 and V4 type headers */
    if ( f_cbPropertyData % SIZEOF( DRM_WCHAR ) == 0 )
    {
        /* Check if first 2 8-bit units are little endian BOM (byte order mark) */
        if ( IS_LITTLE_ENDIAN_UTF16_BOM( f_pbPropertyData ) )
        {
            dstrHeader.pwszString = ( DRM_WCHAR * ) f_pbPropertyData;

            /* Skipping UTF-16 bits BOM */
            dstrHeader.pwszString++;
            dstrHeader.cchString = f_cbPropertyData / SIZEOF( DRM_WCHAR ) - 1;
        }
        else
        {
            /* Big endian BOM is not expected here, real data are expected */
            dstrHeader.pwszString = ( DRM_WCHAR *) f_pbPropertyData;
            dstrHeader.cchString  = f_cbPropertyData / SIZEOF( DRM_WCHAR );
        }

        ChkDR( DRM_HDR_GetHeaderVersion( &dstrHeader, &eHeaderVersion ) );
        switch( eHeaderVersion )
        {
        case DRM_HEADER_VERSION_2:
            *f_peProperty = DRM_CSP_V2_HEADER;
            break;
        case DRM_HEADER_VERSION_2_4:
            *f_peProperty = DRM_CSP_V2_4_HEADER;
            break;
        case DRM_HEADER_VERSION_4:
            *f_peProperty = DRM_CSP_V4_HEADER;
            break;
        case DRM_HEADER_VERSION_4_1:
            *f_peProperty = DRM_CSP_V4_1_HEADER;
            break;
        default:
            dr = DRM_E_CH_UNSUPPORTED_VERSION;
            break;
        }
    }

ErrorExit:

    return dr;
}
#if defined(_PREFAST_)
#pragma prefast(pop)
#endif

/*********************************************************************
**
** Function: _PrepareViewRights
**
** Synopsis: Function that initializes a view rights context contained
**           in an initialized DRM manager context. The view rights
**           context is going to be used to retrieve license state data.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _PrepareViewRights(
    IN DRM_APP_CONTEXT *f_poAppContext )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_VIEW_RIGHTS_CONTEXT  *poViewRightsContext  = NULL;
    DRM_DWORD                 i                    = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__PrepareViewRights );

    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->fStoreOpened );

    poViewRightsContext = &poAppContextInternal->oViewRightsContext;

    /* Explicitly zeroed the view right context. */
    ZEROMEM( poViewRightsContext, SIZEOF( DRM_VIEW_RIGHTS_CONTEXT ) );

    poViewRightsContext->pBBContext = &poAppContextInternal->oBlackBoxContext;

    /* Initialize the pointers of license enumerator contexts in the view rights context. */
    for ( i = 0; i < DRM_MAX_LICENSE_CHAIN_DEPTH; i++ )
    {
        poViewRightsContext->rgpLicQueryContext[ i ] = &poAppContextInternal->oLicEnum[ i ];
    }

    poViewRightsContext->pbGlobalSecStore  = &poAppContextInternal->oSecStoreGlobalContext;

    poViewRightsContext->pbLIDSecStore     = &poAppContextInternal->rgcontextSST[ 0 ];

    poViewRightsContext->pbLicenseStoreXML = &poAppContextInternal->oLicStoreContextXML;

    poViewRightsContext->pbLicenseStoreXMR = &poAppContextInternal->oLicStoreContextXMR;

    /* Initialize the pointer of license evaluation context in the view rights context. */
    poViewRightsContext->pLicEval          = &poAppContextInternal->oLicEvalContext;

    /* Initialize the pointer of scratch buffer in the view rights context. */
    poViewRightsContext->pbBuffer          =  poAppContextInternal->pbOpaqueBuffer
                                              + __CB_DECL( poAppContextInternal->cbTotalHeaderData );

    /* Initialize the size ( number of bytes ) of scratch buffer in the view rights context. */
    poViewRightsContext->cbBuffer          =  poAppContextInternal->cbOpaqueBuffer
                                              - poAppContextInternal->cbTotalHeaderData;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    Drm_Content_SetProperty
**
** Synopsis:    Sets content properties.  This allows the DRM runtime to
**              have enough information to attempt to perform DRM tasks
**              that related to specific peices of content, such as Bind
**              and GetLicenseStateData.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_eProperty]            -- The property to set. Must be a value from the
**                              DRM_CONTENT_SET_PROPERTY enum.
** [f_pbPropertyData]       -- Pointer to the property-specific data. See below.
** [f_cbPropertyData]       -- Count of bytes in the property-specific data.
**
** The following are the valid properties, and the respective property-specific data:
** Property                       Property-Specific Data.
** DRM_CSP_AUTODETECT_HEADER      One of the header specified below, autodetection of header
**                                type is required.
** DRM_CSP_PLAYREADY_OBJ          Byte array with the contents of the PlayReady object
** DRM_CSP_V4_HEADER              Byte array with the contents of the V4 header (unicode XML)
** DRM_CSP_V4_1_HEADER            Byte array with the contents of the V4.1 header (unicode XML)
**                                If this is an empty V4.1 header, f_pbPropertyData has to be NULL
**                                and f_cbPropertyData has to be zero.
** DRM_CSP_V2_4_HEADER            Byte array with the contents of the V2.4 header (unicode XML)
** DRM_CSP_V2_HEADER              Byte array with the contents of the V2 header (unicode XML)
** DRM_CSP_KID                    Wide character array specifying the 24-character KeyID
** DRM_CSP_V1_HEADER              Pointer to a valid DRM_CSP_V1_HEADER_DATA structure
** DRM_CSP_PLAYREADY_OBJ_WITH_KID Pointer to a valid DRM_CSP_PLAYREADY_OBJ_WITH_KID_DATA structure
** DRM_CSP_HEADER_COMPONENTS      Pointer to a valid DRM_CSP_HEADER_COMPONENTS_DATA structure
**
** Note:                    This function allows to change type of V2.4 content header to V2 header
**                          For this change f_eProperty should be set to DRM_CSP_V2_HEADER,
**                          f_pbPropertyData to NULL and f_cbPropertyData to 0 (zero)
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the buffer passed
**                          to Drm_Initialize was too small.  The caller
**                          can use Drm_ResizeOpaqueBuffer to increase the
**                          size of the buffer in this scenario.
**                          For f_eProperty==DRM_CSP_PLAYREADY_OBJ_WITH_KID,
**                          the new buffer size should be at least this size:
**                          DWORD_ALIGNED_SIZE( DRM_MAX_LICENSESIZE )
**                              + ((DRM_CSP_PLAYREADY_OBJ_WITH_KID_DATA*)f_pbPropertyData)->cbKeyID
**                              + ((DRM_CSP_PLAYREADY_OBJ_WITH_KID_DATA*)f_pbPropertyData)->cbHeaderData
**                          For f_eProperty==DRM_CSP_HEADER_COMPONENTS,
**                          the new buffer size should be at least this size:
**                          DWORD_ALIGNED_SIZE( DRM_MAX_LICENSESIZE )
**                              + size of new XML header
**                          For other values of f_eProperty,
**                          the new buffer size should be at least this size:
**                          DWORD_ALIGNED_SIZE( DRM_MAX_LICENSESIZE )
**                              + f_cbPropertyData
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Content_SetProperty(
    IN                                        DRM_APP_CONTEXT          *f_poAppContext,
    IN                                        DRM_CONTENT_SET_PROPERTY  f_eProperty,
    __in_bcount_opt( f_cbPropertyData ) const DRM_BYTE                 *f_pbPropertyData,
    IN                                        DRM_DWORD                 f_cbPropertyData )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_CONTENT_SET_PROPERTY  eProperty            = DRM_CSP_HEADER_NOT_SET;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext != NULL );

    ChkArg( f_eProperty == DRM_CSP_PLAYREADY_OBJ
         || f_eProperty == DRM_CSP_V4_HEADER
         || f_eProperty == DRM_CSP_V4_1_HEADER
         || f_eProperty == DRM_CSP_V2_4_HEADER
         || f_eProperty == DRM_CSP_V2_HEADER
         || f_eProperty == DRM_CSP_KID
         || f_eProperty == DRM_CSP_V1_HEADER
         || f_eProperty == DRM_CSP_AUTODETECT_HEADER
         || f_eProperty == DRM_CSP_PLAYREADY_OBJ_WITH_KID
         || f_eProperty == DRM_CSP_HEADER_COMPONENTS );

    /* Check if user trying to change content header */
    if (  f_pbPropertyData == NULL
       && poAppContextInternal->eHeaderInContext != DRM_CSP_HEADER_NOT_SET )
    {
        /* Check if possible to change header type: V2.4->V2 */
        if ( f_eProperty != poAppContextInternal->eHeaderInContext )
        {
            ChkBOOL(  f_eProperty == DRM_CSP_V2_HEADER
                   && poAppContextInternal->eHeaderInContext == DRM_CSP_V2_4_HEADER, DRM_E_CH_INCOMPATIBLE_HEADER_TYPE );
            poAppContextInternal->eHeaderInContext = DRM_CSP_V2_HEADER;
        }
        goto ErrorExit;
    }

    if (  f_eProperty == DRM_CSP_V4_1_HEADER
       && f_pbPropertyData == NULL
       && f_cbPropertyData == 0 )
    {
        /* Use the predefined empty V4.1 header string. */
        f_pbPropertyData = (const DRM_BYTE *)g_dstrEmptyWRMHeaderV4_1.pwszString;
        f_cbPropertyData = g_dstrEmptyWRMHeaderV4_1.cchString * SIZEOF( DRM_WCHAR );
    }

    /* From this point pbPropertyData cannot be NULL or empty */
    ChkArg( f_pbPropertyData != NULL );
    ChkArg( f_cbPropertyData > 0 );

    /* Reset these every time: Only a full PlayReady object can initialize these */
    poAppContextInternal->pbEmbeddedStore = NULL;
    poAppContextInternal->cbEmbeddedStore = 0;
    poAppContextInternal->cbPROData       = 0;

    if ( f_eProperty == DRM_CSP_AUTODETECT_HEADER )
    {
        ChkDR( _AutoDetectHeader( f_pbPropertyData, f_cbPropertyData, &eProperty ) );

        if ( eProperty == DRM_CSP_HEADER_NOT_SET )
        {
            ChkDR( DRM_E_CH_INVALID_HEADER );
        }
    }
    else
    {
        eProperty = f_eProperty;
    }

    if ( poAppContextInternal->eHeaderInContext == DRM_CSP_V1_HEADER )
    {
        /* Header V1 can be upgraded only */
        ChkBOOL( eProperty != DRM_CSP_V1_HEADER, DRM_E_HEADER_ALREADY_SET );
    }
    else
    {
        /* All headers except V1 cannot be changed, use Drm_Reinitialize */
        ChkBOOL( poAppContextInternal->eHeaderInContext == DRM_CSP_HEADER_NOT_SET, DRM_E_HEADER_ALREADY_SET );
    }

    if ( eProperty == DRM_CSP_PLAYREADY_OBJ )
    {
        ChkBOOL( poAppContextInternal->cbDRMHeaderData >= f_cbPropertyData,
                    DRM_E_BUFFERTOOSMALL );
        MEMCPY( poAppContextInternal->pbDRMHeaderData,
                f_pbPropertyData,
                f_cbPropertyData );
        poAppContextInternal->cbPROData = f_cbPropertyData;
        poAppContextInternal->cbTotalHeaderData = f_cbPropertyData;

        dr = DRM_PRO_GetRecord( poAppContextInternal->pbDRMHeaderData,
                                f_cbPropertyData,
                                PLAYREADY_WRMHEADER,
                                (DRM_BYTE **)&poAppContextInternal->pwszHeaderData,
                                &poAppContextInternal->cbHeaderData );
        if( dr == DRM_E_RECORD_NOT_FOUND )
        {
            /* PRO without WRM Header can only be set with a corresonding KID (use DRM_CSP_PLAYREADY_OBJ_WITH_KID) */
            dr = DRM_E_CH_INVALID_HEADER;
        }
        ChkDR( dr );

        if ( poAppContextInternal->fBindThenCommit )
        {
            poAppContextInternal->fBindThenCommit = FALSE;
        }

        /*
        ** Now load the embedded license store, if it is there
        */
        dr = DRM_PRO_GetRecord( poAppContextInternal->pbDRMHeaderData,
                                f_cbPropertyData,
                                PLAYREADY_EMBEDDED_LICENSE_STORE,
                                &poAppContextInternal->pbEmbeddedStore,
                                &poAppContextInternal->cbEmbeddedStore );
        if ( dr == DRM_E_RECORD_NOT_FOUND )
        {
            poAppContextInternal->pbEmbeddedStore = NULL;
            poAppContextInternal->cbEmbeddedStore = 0;
            dr = DRM_SUCCESS;
        }
        else
        {
            ChkDR( dr );
        }
    }
    else if (  eProperty == DRM_CSP_V2_HEADER
            || eProperty == DRM_CSP_V2_4_HEADER
            || eProperty == DRM_CSP_V4_HEADER
            || eProperty == DRM_CSP_V4_1_HEADER )
    {
        DRM_DWORD cchHeader = 0;

        /* The header is Unicode, therefore its size must be even */
        ChkBOOL( f_cbPropertyData % SIZEOF(DRM_WCHAR) == 0, DRM_E_CH_INVALID_HEADER );

        /* Make sure that there is enough data to process BOM (byte order mark). */
        ChkBOOL( f_cbPropertyData > 2, DRM_E_CH_INVALID_HEADER );

        if ( poAppContextInternal->fBindThenCommit )
        {
            poAppContextInternal->fBindThenCommit = FALSE;
        }

        ChkBOOL( poAppContextInternal->cbDRMHeaderData >= f_cbPropertyData,
                    DRM_E_BUFFERTOOSMALL );

        /* Have to fail if first 2 8-bit units indicates big endian BOM (byte order mark). */
        /* PK accepts input in little endian format only. */
        ChkBOOL( !IS_BIG_ENDIAN_UTF16_BOM( f_pbPropertyData ), DRM_E_CH_INVALID_HEADER );

        /* Check if first 2 8-bit units are little endian BOM (byte order mark) */
        if ( IS_LITTLE_ENDIAN_UTF16_BOM( f_pbPropertyData ) )
        {
            /* Copy header without UTF-16 BOM. */
            DRM_BYT_CopyBytes( poAppContextInternal->pbDRMHeaderData,
                                0,
                                f_pbPropertyData,
                                SIZEOF( DRM_WCHAR ), f_cbPropertyData - SIZEOF( DRM_WCHAR ) );

            poAppContextInternal->cbHeaderData = f_cbPropertyData - SIZEOF( DRM_WCHAR );
        }
        else
        {
            MEMCPY( poAppContextInternal->pbDRMHeaderData,
                    f_pbPropertyData,
                    f_cbPropertyData );

            poAppContextInternal->cbHeaderData = f_cbPropertyData;
        }

        cchHeader = poAppContextInternal->cbHeaderData / SIZEOF( DRM_WCHAR );

        if ( ( ( DRM_WCHAR * )( poAppContextInternal->pbDRMHeaderData ) )[ cchHeader - 1 ] == g_wchNull )
        {
            /* Remove the NULL terminating character if it is there. */
            poAppContextInternal->cbHeaderData -= SIZEOF( DRM_WCHAR );
        }
    }
    else if ( eProperty == DRM_CSP_KID )
    {
        ChkArg( f_cbPropertyData == CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) * SIZEOF( DRM_WCHAR ) );

        if ( poAppContextInternal->fBindThenCommit )
        {
            poAppContextInternal->fBindThenCommit = FALSE;
        }

        ChkBOOL( poAppContextInternal->cbDRMHeaderData >= f_cbPropertyData,
                    DRM_E_BUFFERTOOSMALL );

        MEMCPY( poAppContextInternal->pbDRMHeaderData,
                f_pbPropertyData,
                f_cbPropertyData );

        poAppContextInternal->cbHeaderData = f_cbPropertyData;
    }
    else if ( eProperty == DRM_CSP_V1_HEADER )
    {
        DRM_CSP_V1_HEADER_DATA *pHeaderData = ( DRM_CSP_V1_HEADER_DATA * )f_pbPropertyData;
        DRM_V1Header *pV1Header = NULL;

        if ( poAppContextInternal->fBindThenCommit )
        {
            poAppContextInternal->fBindThenCommit = FALSE;
        }

        ChkArg( f_cbPropertyData == SIZEOF( DRM_CSP_V1_HEADER_DATA ) );
        ChkArg( pHeaderData->pbKeyID != NULL && pHeaderData->cbKeyID > 0 );
        ChkArg( pHeaderData->pbSecretData != NULL && pHeaderData->cbSecretData > 0 );
        ChkArg( pHeaderData->pbURL != NULL && pHeaderData->cbURL > 0 );
        ChkBOOL( poAppContextInternal->cbDRMHeaderData >= pHeaderData->cbURL + pHeaderData->cbKeyID + pHeaderData->cbSecretData + SIZEOF( DRM_V1Header ),
                    DRM_E_BUFFERTOOSMALL );

        pV1Header = ( DRM_V1Header * )poAppContextInternal->pbDRMHeaderData;

        pV1Header->cbKeyID = 4 * ( pHeaderData->cbKeyID / 4 );  /* KeyID length must be mutilple of 4 bytes. */
        pV1Header->pbKeyID = pV1Header->bBuffer;

        MEMCPY( pV1Header->pbKeyID, pHeaderData->pbKeyID, pHeaderData->cbKeyID );

        pV1Header->cbSecretData = pHeaderData->cbSecretData;

        /* For 16-bit addressing the key ID must lie on a word boundary. */
        DRMASSERT( pHeaderData->cbKeyID % CB_NATIVE_BYTE == 0 );

        pV1Header->pbSecretData = &( pV1Header->bBuffer[ __CB_DECL( pHeaderData->cbKeyID ) ] );

        MEMCPY( pV1Header->pbSecretData, pHeaderData->pbSecretData, pHeaderData->cbSecretData );

        pV1Header->cbURL = pHeaderData->cbURL;

        /* For 16-bit addressing the URL must lie on a word boundary. */
        DRMASSERT( ( pHeaderData->cbKeyID + pHeaderData->cbSecretData ) % CB_NATIVE_BYTE == 0 );

        pV1Header->pbURL = &( pV1Header->bBuffer[ __CB_DECL( pHeaderData->cbKeyID + pHeaderData->cbSecretData ) ] );

        MEMCPY( pV1Header->pbURL, pHeaderData->pbURL, pHeaderData->cbURL );

        poAppContextInternal->cbHeaderData = pHeaderData->cbURL +
                                                pHeaderData->cbKeyID +
                                                pHeaderData->cbSecretData +
                                                SIZEOF( DRM_V1Header );

    }
    else if ( eProperty == DRM_CSP_PLAYREADY_OBJ_WITH_KID )
    {
        DRM_CSP_PLAYREADY_OBJ_WITH_KID_DATA *pHeaderData = ( DRM_CSP_PLAYREADY_OBJ_WITH_KID_DATA*) f_pbPropertyData;
        ChkArg( f_cbPropertyData == SIZEOF( DRM_CSP_PLAYREADY_OBJ_WITH_KID_DATA ) );

        ChkArg( pHeaderData->pbKeyID != NULL
                && pHeaderData->cbKeyID == CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) * SIZEOF( DRM_WCHAR ) );
        ChkArg( pHeaderData->pbHeaderData != NULL && pHeaderData->cbHeaderData > 0 );

        ChkBOOL( poAppContextInternal->cbDRMHeaderData >= pHeaderData->cbHeaderData + pHeaderData->cbKeyID,
                    DRM_E_BUFFERTOOSMALL );

        if ( poAppContextInternal->fBindThenCommit )
        {
            poAppContextInternal->fBindThenCommit = FALSE;
        }

        MEMCPY( poAppContextInternal->pbDRMHeaderData,
                pHeaderData->pbKeyID,
                pHeaderData->cbKeyID );

        MEMCPY( &(poAppContextInternal->pbDRMHeaderData[pHeaderData->cbKeyID]),
                pHeaderData->pbHeaderData,
                pHeaderData->cbHeaderData );

        poAppContextInternal->cbHeaderData = pHeaderData->cbKeyID;
        poAppContextInternal->cbPROData    = pHeaderData->cbHeaderData + pHeaderData->cbKeyID;

        /*
        ** Now load the embedded license store, if it is there
        */
        dr = DRM_PRO_GetRecord( &(poAppContextInternal->pbDRMHeaderData[pHeaderData->cbKeyID]),
                                pHeaderData->cbHeaderData,
                                PLAYREADY_EMBEDDED_LICENSE_STORE,
                                &poAppContextInternal->pbEmbeddedStore,
                                &poAppContextInternal->cbEmbeddedStore );
        if ( dr == DRM_E_RECORD_NOT_FOUND )
        {
            poAppContextInternal->pbEmbeddedStore = NULL;
            poAppContextInternal->cbEmbeddedStore = 0;
            dr = DRM_SUCCESS;
        }
        else
        {
            ChkDR( dr );
        }
    }
    else if ( eProperty == DRM_CSP_HEADER_COMPONENTS )
    {
        DRM_DWORD cchHeader     = 0;
        DRM_DWORD cbHeaderData  = 0;
        DRM_CSP_HEADER_COMPONENTS_DATA *pHeaderData = ( DRM_CSP_HEADER_COMPONENTS_DATA*) f_pbPropertyData;

        ChkArg( f_cbPropertyData == SIZEOF( DRM_CSP_HEADER_COMPONENTS_DATA ) );

        if ( poAppContextInternal->fBindThenCommit )
        {
            poAppContextInternal->fBindThenCommit = FALSE;
        }

        ChkArg( pHeaderData->eHeaderVersion == DRM_HEADER_VERSION_4
             || pHeaderData->eHeaderVersion == DRM_HEADER_VERSION_4_1 );

        /*
        ** Convert the header to a standard header.
        ** This will fail with DRM_E_BUFFTOOSMALL if poAppContextInternal->pbDRMHeaderData
        ** is not big enough, so we can simply fail with that error too in that case.
        */
        cbHeaderData = poAppContextInternal->cbDRMHeaderData;
        ChkDR( DRM_PRO_CreateRMHeader(
            pHeaderData->eHeaderVersion,
            pHeaderData->eCipherType,
            &pHeaderData->dstrKID,
            &pHeaderData->dstrCheckSum,
            &pHeaderData->dstrPlayReadySilentURL,
            &pHeaderData->dstrPlayReadyNonSilentURL,
            &pHeaderData->dstrServiceID,
            &pHeaderData->dstrCustomAttributesXML,
            &pHeaderData->dstrDecryptorSetup,
            poAppContextInternal->pbDRMHeaderData,
            &cbHeaderData ) );
        ChkBOOL( cbHeaderData % SIZEOF(DRM_WCHAR) == 0, DRM_E_CH_INVALID_HEADER );
        poAppContextInternal->cbHeaderData = cbHeaderData;
        cchHeader = poAppContextInternal->cbHeaderData / SIZEOF( DRM_WCHAR );
        if ( ( ( DRM_WCHAR * )( poAppContextInternal->pbDRMHeaderData ) )[ cchHeader - 1 ] == g_wchNull )
        {
            /* Remove the NULL terminating character if it is there. */
            poAppContextInternal->cbHeaderData -= SIZEOF( DRM_WCHAR );
        }

        if( pHeaderData->eHeaderVersion == DRM_HEADER_VERSION_4 )
        {
            eProperty = DRM_CSP_V4_HEADER;
        }
        else if( pHeaderData->eHeaderVersion == DRM_HEADER_VERSION_4_1 )
        {
            eProperty = DRM_CSP_V4_1_HEADER;
        }
        else
        {
            AssertChkArg( FALSE ); /* Should never be reached (checked previously) */
        }
    }

    poAppContextInternal->eHeaderInContext = eProperty;

    if ( eProperty != DRM_CSP_PLAYREADY_OBJ )
    {
        poAppContextInternal->pwszHeaderData = ( DRM_WCHAR * )poAppContextInternal->pbDRMHeaderData;
        if ( eProperty != DRM_CSP_PLAYREADY_OBJ_WITH_KID )
        {
            poAppContextInternal->cbTotalHeaderData = poAppContextInternal->cbHeaderData;
        }
        else
        {
            poAppContextInternal->cbTotalHeaderData = poAppContextInternal->cbPROData;
        }
    }
    else
    {
        poAppContextInternal->cbTotalHeaderData = poAppContextInternal->cbPROData;
    }

ErrorExit:

    ChkECC( ECC_Drm_Content_SetProperty, dr );

    return dr;
}

/**********************************************************************
**
** Function:    Drm_Content_UpdateEmbeddedStore
**
** Synopsis:    Searches for suitable licenses in the store and embeds
**              them, if found, into the PlayReady object buffer.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
**
** Returns:                 DRM_SUCCESS      - Successfully embedded a license.
**                          DRM_S_FALSE      - Did not update the embedded store, but no error occurred.
**                          DRM_E_INVALIDARG - An argument is invalid or improperly initialized.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Content_UpdateEmbeddedStore(
    __in    DRM_APP_CONTEXT     *f_poAppContext )
{
    DRM_RESULT                  dr                       = DRM_S_FALSE;
    DRM_APP_CONTEXT_INTERNAL   *poAppContextInternal     = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BOOL                    fModifiedEST             = FALSE;
    DRM_BOOL                    fESTOpened               = FALSE;
    DRM_CONST_STRING            dstrKID                  = EMPTY_DRM_STRING;
    DRM_KID                     oKID                     = { 0 };
    DRM_BYTE                    rgbLicEvalBuffer[ __CB_DECL( SIZEOF( DRM_LICEVAL_CONTEXT ) ) ] = { 0 };
    DRM_DWORD                   cbScratchBuffer          = 0;
    DRM_BYTE                    pbKID[CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) * SIZEOF( DRM_WCHAR ) ]= { 0 };
    DRM_DWORD                   cbKID                    = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) * SIZEOF( DRM_WCHAR );

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Content_UpdateEmbeddedStore );

    ChkArg( f_poAppContext != NULL );

    /*
    ** Verify that a PlayReady Object with an embedded store record
    ** has been set using Drm_Content_SetProperty.
    */
    ChkBOOL( poAppContextInternal->eHeaderInContext  != DRM_CSP_HEADER_NOT_SET
          && poAppContextInternal->cbTotalHeaderData >  0, DRM_E_HEADER_NOT_SET );
    ChkBOOL( (    poAppContextInternal->eHeaderInContext  == DRM_CSP_PLAYREADY_OBJ
               || poAppContextInternal->eHeaderInContext  == DRM_CSP_PLAYREADY_OBJ_WITH_KID )
          && poAppContextInternal->cbEmbeddedStore   >  0
          && poAppContextInternal->pbEmbeddedStore   != NULL, DRM_E_CH_INCOMPATIBLE_HEADER_TYPE );

    /*
    ** Retrieve the KID from the Header
    */
    ChkDR( Drm_Content_GetProperty( f_poAppContext, DRM_CGP_HEADER_KID, pbKID, &cbKID) );

    DSTR_FROM_PB( &dstrKID, pbKID, cbKID );

    ChkDR( DRM_UTL_DecodeKID( &dstrKID, &poAppContextInternal->KID ) );
    MEMCPY( &oKID, &poAppContextInternal->KID, SIZEOF( DRM_KID ) );

    /*
    ** Set up enumeration over embedded store and HDS (XMR) store
    */

    /*
    ** Get the embedded license store from the PlayReady Object
    */
    if ( DRM_FAILED( DRM_DST_OpenStore( poAppContextInternal->pOEMContext,
                                        poAppContextInternal->pbEmbeddedStore,
                                        poAppContextInternal->cbEmbeddedStore,
                                        0,
                                        &poAppContextInternal->oDatastoreEST ) ) )
    {
        /*
        ** Create the store if it doesn't already exist
        */
        ChkDR( DRM_DST_CreateStore( poAppContextInternal->pOEMContext,
                                    poAppContextInternal->pbEmbeddedStore,
                                    poAppContextInternal->cbEmbeddedStore,
                                    0,
                                    eDRM_DSTBLKNUM_DWORD,
                                    FALSE,
                                    0,
                                    &poAppContextInternal->oDatastoreEST ) );

        fModifiedEST = TRUE; /* Created store should be re-written to file */
    }
    fESTOpened = TRUE;
    ChkDR( DRM_LST_Open( &poAppContextInternal->oLicStoreContextEmbedded,
                         &poAppContextInternal->oDatastoreEST,
                         eDRM_LICENSE_STORE_XMR ) );

    ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

    cbScratchBuffer = poAppContextInternal->cbOpaqueBuffer - poAppContextInternal->cbTotalHeaderData;

    ChkDR( DRM_EMB_UpdateEmbeddedStore( &poAppContextInternal->oLicStoreContextEmbedded,
                                        &poAppContextInternal->oLicStoreContextXMR,
                                        &poAppContextInternal->oDomainStoreContext,
                                        &poAppContextInternal->oLicEvalContext,
                                        rgbLicEvalBuffer,
                                        poAppContextInternal->pbOpaqueBuffer + __CB_DECL( poAppContextInternal->cbTotalHeaderData ),
                                        &cbScratchBuffer,
                                        &oKID,
                                        &fModifiedEST ) );

ErrorExit:
    if ( poAppContextInternal != NULL )
    {
        if ( poAppContextInternal->oLicStoreContextEmbedded.fInited )
        {
            DRM_LST_Close( &poAppContextInternal->oLicStoreContextEmbedded );
        }
        if ( fESTOpened )
        {
            DRM_DST_CloseStore( &poAppContextInternal->oDatastoreEST );
        }
    }
    /*
    ** Return DRM_S_FALSE if no updates were made, or DRM_SUCCESS if updates were made.
    */
    if ( DRM_SUCCEEDED( dr ) )
    {
        dr = fModifiedEST? DRM_SUCCESS : DRM_S_FALSE;
    }

    DRM_PROFILING_LEAVE_SCOPE;
    ChkECC( ECC_Drm_Content_UpdateEmbeddedStore, dr );

    return dr;
}

/**********************************************************************
**
** Function:    Drm_Content_UpdateEmbeddedStore_Commit
**
** Synopsis:    Performs actions that may need to happen after the
**              embedded store is written to the file.
**
**              Should always be called after retrieving the header
**              from the DRM_APP_CONTEXT and written to the file.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
**
** Returns:                 DRM_SUCCESS      - Successfully completed the transaction.
**                          DRM_E_INVALIDARG - An argument is invalid or improperly initialized.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Content_UpdateEmbeddedStore_Commit(
    __in    DRM_APP_CONTEXT     *f_poAppContext )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Content_UpdateEmbeddedStore_Commit );

    ChkArg( f_poAppContext );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;
    ChkECC( ECC_Drm_Content_UpdateEmbeddedStore_Commit, dr );
    return dr;
}

static DRM_RESULT _GetUplinkState(
    IN const DRM_CONST_STRING        *f_pdstrAction,
    IN       DRM_DWORD                f_iKID,
    IN OUT   DRM_LICENSE_CHALLENGE   *f_plicensechallenge,
    IN       DRM_VIEW_RIGHTS_CONTEXT *f_pcontextVR,
    IN       DRM_DST                 *f_pDatastore )
{
    DRM_RESULT              dr                 = DRM_SUCCESS;
    const DRM_CONST_STRING *rgpdstrRights[ 1 ] = { NULL };

    rgpdstrRights[ 0 ] = f_pdstrAction;

    ChkDR( DRM_UTL_DecodeKID( &f_plicensechallenge->rgdstrUplinkKID[ f_iKID ],
                              &f_pcontextVR->KID ) );

    ChkDR( DRM_VIEWRIGHTS_GetLicenseAggregateData(  rgpdstrRights,
                                                   &f_plicensechallenge->rglicensestatedata[ f_iKID ],
                                                    1,
                                                    f_pcontextVR,
                                                    f_pDatastore,
                                                    FALSE,
                                                    DRM_ASD_AGGREGATE_SIMPLE_AND_LEAF_LICENSES,
                                                    NULL,
                                                    NULL ) );

ErrorExit:

    return dr;
}

/**********************************************************************
**
** Function:    Drm_SyncListItem_InitEnum
**
** Synopsis:    Initializes an enumeration of entries in the license
**              sync store.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_SyncListItem_InitEnum(
    IN DRM_APP_CONTEXT *f_poAppContext )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_SyncListItem_InitEnum );

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        ChkArg( f_poAppContext != NULL );

        ChkDR( _PrepareViewRights( f_poAppContext ) );

        ChkDR( DRM_SNC_InitEnum( &poAppContextInternal->contextSync,
                                 &poAppContextInternal->contextSyncEnum ) );
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        ChkECC( ECC_Drm_SyncListItem_InitEnum, dr );
    }

    return dr;
}

/**********************************************************************
**
** Function:    Drm_SyncListItem_EnumNext
**
** Synopsis:    Enumerates the next license sync entry in the store.
**              Must call Drm_SyncListItem_InitEnum prior to calling
**              this function.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_cMaxCount]            -- Maximum remaining playcount before license
**                             expiration to use evaluating licenses with
**                             count-based expirations. Use
**                             DRM_SYNC_IGNORE_THRESHOLD_PARAMETER (0xFFFFFFFF)
**                             to ignore this parameter and always sync
**                             count-based licenses.
** [f_cMaxHours]            -- Maximum number of hours before license
**                             expiration to use evaluating licenses with
**                             date-based expirations. Use
**                             DRM_SYNC_IGNORE_THRESHOLD_PARAMETER (0xFFFFFFFF)
**                             to ignore this parameter and always sync
**                             date-based licenses.
** [f_pKID]                 -- Optional buffer to hold the binary KID of the
**                             currently enumerated entry.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_SyncListItem_EnumNext(
    IN  DRM_APP_CONTEXT *f_poAppContext,
    IN  DRM_DWORD        f_cMaxCount,
    IN  DRM_DWORD        f_cMaxHours,
    OUT DRM_KID         *f_pKID )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_DST_SLOT_HINT         slotHint             = { 0 };
    DRM_KID                   kid                  = { 0 };
    DRM_LICENSE_STATE_DATA    lsd                  = { 0 };
    DRM_UINT64                ui64ThreshholdTime   = DRM_UI64LITERAL( 0, 0 );

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_SyncListItem_EnumNext );

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        ChkArg ( f_poAppContext != NULL );

        do
        {
            ChkDR( DRM_SNC_EnumNext( &poAppContextInternal->contextSyncEnum, &kid, &slotHint ) );

            ChkDR( DRM_SNC_GetStoreDataForKID( &poAppContextInternal->contextSync,
                                               &kid,
                                               &slotHint,
                                               &lsd,
                                                NULL,
                                                NULL ) );

            /*
            ** If this KID has count-based licenses we must refresh its data from the
            ** license store; date-based data will be updated along with licenses.
            */
            if ( DRM_SNC_IsStoreRefreshRequired( poAppContextInternal->pOEMContext, &lsd ) )
            {
                ZEROMEM( &lsd, SIZEOF( lsd ) );

                MEMCPY( &poAppContextInternal->oViewRightsContext.KID, &kid, SIZEOF( DRM_KID ) );

                /* If this fails, we will want to sync. */
                if ( DRM_SUCCEEDED( DRM_SNC_GetLicenseDataForKID( &poAppContextInternal->oDatastoreHDS,
                                                                  &poAppContextInternal->oViewRightsContext,
                                                                  &lsd ) ) )
                {
                    ChkDR( DRM_SNC_SetStoreDataForKID( &poAppContextInternal->contextSync,
                                                       &kid,
                                                       &slotHint,
                                                       &lsd,
                                                        NULL,
                                                        FALSE ) );
                }
            }

            /* Determine if this requires a sync. */
            if ( DRM_SNC_IsSyncRequired(  poAppContextInternal->pOEMContext,
                                          f_cMaxCount,
                                          f_cMaxHours,
                                         &lsd,
                                         &ui64ThreshholdTime ) )
            {
                DRM_DWORD cch = poAppContextInternal->cbDRMHeaderData / SIZEOF( DRM_WCHAR );

                if ( poAppContextInternal->fBindThenCommit )
                {
                    poAppContextInternal->fBindThenCommit = FALSE;
                }

                if ( f_pKID != NULL )
                {
                    MEMCPY( f_pKID, kid.rgb, SIZEOF( DRM_KID ) );
                }

                ChkDR( DRM_B64_EncodeW(  kid.rgb, SIZEOF( kid.rgb ),
                                         ( DRM_WCHAR* )poAppContextInternal->pbDRMHeaderData,
                                        &cch,
                                         0 ) );

                poAppContextInternal->cbHeaderData     = cch * SIZEOF( DRM_WCHAR );
                poAppContextInternal->eHeaderInContext = DRM_CSP_KID;

                break;
            }
        } while( TRUE ); /* Keep looping until either we run out of entries or find one. */
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

static DRM_RESULT _GenerateLicenseChallengeV2(
    __in                                    DRM_APP_CONTEXT   *f_poAppContext,
    __in const                              DRM_CONST_STRING **f_ppdstrRights, /* Array of DRM_CONST_STRING pointers */
    __in                                    DRM_DWORD          f_cRights,
    __out_ecount_opt( *f_pcchSilentURL )    DRM_CHAR          *f_pchSilentURL,
    __inout                                 DRM_DWORD         *f_pcchSilentURL,
    __out_ecount_opt( *f_pcchNonSilentURL ) DRM_CHAR          *f_pchNonSilentURL,
    __inout                                 DRM_DWORD         *f_pcchNonSilentURL,
    __out_ecount_opt( *f_pcchChallenge )    DRM_CHAR          *f_pszChallenge,
    __inout                                 DRM_DWORD         *f_pcchChallenge )
{
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_CONST_STRING          dstrLAINFO           = EMPTY_DRM_STRING;
    DRM_DWORD                 cbChallenge          = 0;
    DRM_DWORD                 cbRequired           = 0;
    DRM_BOOL                  fTooSmall            = FALSE;
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_BOOL                  fInit                = FALSE;

    ChkArg( f_poAppContext != NULL && f_pcchChallenge != NULL );

    ChkArg(  ( f_cRights  > 0 && f_ppdstrRights != NULL )
          || ( f_cRights == 0 && f_ppdstrRights == NULL ) );

    if ( poAppContextInternal->fBindThenCommit )
    {
        poAppContextInternal->fBindThenCommit = FALSE;
    }

    if ( poAppContextInternal->eHeaderInContext == DRM_CSP_V1_HEADER )
    {
        ChkDR ( DRM_E_V1_NOT_SUPPORTED );  /* V1 Lic Acq is not supported. */
    }

    ZEROMEM( &poAppContextInternal->licensechallenge, SIZEOF( DRM_LICENSE_CHALLENGE ) );

    /* Did they only pass in a KID? We need a full header for license acquisition. */
    if ( poAppContextInternal->eHeaderInContext == DRM_CSP_KID )
    {
        if( DRM_SNC_IsLicenseSyncSupported() )
        {
            ChkDR( _LoadHeaderFromSyncStore( f_poAppContext ) );
        }
        else
        {
            ChkDR( DRM_E_NOTIMPL );
        }
    }

    if ( poAppContextInternal->eHeaderInContext == DRM_CSP_V2_HEADER )
    {

        DSTR_FROM_PB( &poAppContextInternal->licensechallenge.dstrHeader,
                       poAppContextInternal->pbDRMHeaderData,
                       poAppContextInternal->cbHeaderData );

        dr = DRM_HDR_GetAttribute( &poAppContextInternal->licensechallenge.dstrHeader,
                                    NULL,
                                    DRM_HEADER_ATTRIB_LAINFO,
                                   &dstrLAINFO,
                                    0 );

        if ( DRM_FAILED( dr ) )
        {
            /* Make sure this string is empty. */
            ChkDR( DRM_SUCCESS );  /* This sets 'dr' and keeps the PREfast tool happy. */

            dstrLAINFO.cchString  = 0;
            dstrLAINFO.pwszString = NULL;
        }

        poAppContextInternal->oViewRightsContext.pBBContext = &poAppContextInternal->oBlackBoxContext;

        for ( cbRequired = 0; cbRequired < DRM_MAX_LICENSE_CHAIN_DEPTH; cbRequired++ )
        {
            poAppContextInternal->oViewRightsContext.rgpLicQueryContext[ cbRequired ] = &poAppContextInternal->oLicEnum[ cbRequired ];
        }

        poAppContextInternal->oViewRightsContext.pbGlobalSecStore  = &poAppContextInternal->oSecStoreGlobalContext;
        poAppContextInternal->oViewRightsContext.pbLIDSecStore     = &poAppContextInternal->rgcontextSST[ 0 ];
        poAppContextInternal->oViewRightsContext.pbLicenseStoreXML = &poAppContextInternal->oLicStoreContextXML;
        poAppContextInternal->oViewRightsContext.pLicEval          = &poAppContextInternal->oLicEvalContext;
        poAppContextInternal->oViewRightsContext.pbBuffer          =  poAppContextInternal->pbOpaqueBuffer
                                                                      + __CB_DECL( poAppContextInternal->cbHeaderData );
        poAppContextInternal->oViewRightsContext.cbBuffer          =  poAppContextInternal->cbOpaqueBuffer
                                                                      - poAppContextInternal->cbHeaderData;

        /* Start at index 1. If we actually find an uplink index 0 will hold the data for the KID of the header itself. */
        poAppContextInternal->licensechallenge.cUplinks = 1;

        while ( DRM_SUCCEEDED( DRM_HDR_GetUplink( &poAppContextInternal->licensechallenge.dstrHeader,
                                                   poAppContextInternal->licensechallenge.cUplinks - 1,
                                                  &poAppContextInternal->licensechallenge.rgdstrUplinkKID[ poAppContextInternal->licensechallenge.cUplinks ] ) ) )
        {
            if ( !fInit )
            {
                ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

                fInit = TRUE;
            }

            ChkDR( _GetUplinkState(  f_ppdstrRights[ 0 ],
                                     poAppContextInternal->licensechallenge.cUplinks,
                                    &poAppContextInternal->licensechallenge,
                                    &poAppContextInternal->oViewRightsContext,
                                    &poAppContextInternal->oDatastoreHDS ) );

            if ( ++poAppContextInternal->licensechallenge.cUplinks >= NO_OF( poAppContextInternal->licensechallenge.rgdstrUplinkKID ) )
            {
                break;
            }
        }

        if ( fInit )
        {
            /*
            ** fInit is TRUE if we processed an UPLINK so we know they are there.
            ** Now add ASD for the KID in the header.
            */
            ChkDR( DRM_HDR_GetAttribute( &poAppContextInternal->licensechallenge.dstrHeader,
                                          NULL,
                                          DRM_HEADER_ATTRIB_KID,
                                         &poAppContextInternal->licensechallenge.rgdstrUplinkKID[ 0 ],
                                          0 ) );

            ChkDR( _GetUplinkState(  f_ppdstrRights[ 0 ],
                                     0,
                                    &poAppContextInternal->licensechallenge,
                                    &poAppContextInternal->oViewRightsContext,
                                    &poAppContextInternal->oDatastoreHDS ) );
        }
        else
        {
            poAppContextInternal->licensechallenge.cUplinks = 0;
        }
    }

    /* Check for the URL. If user asked for it and we dont have it, return an error. */
    if ( dstrLAINFO.cchString == 0 && f_pcchSilentURL != NULL )
    {
        ChkDR( DRM_E_NO_URL );
    }

    /* Check for the URL. If user asked for it and we dont have it, return an error. */
    if ( dstrLAINFO.cchString == 0 && f_pcchNonSilentURL != NULL )
    {
        ChkDR( DRM_E_NO_URL );
    }

    /*
    ** It will prepare context for Client info node.
    ** Devcert will loaded into the license buffer.
    */
    ChkDR( _PrepareForClientInfo( poAppContextInternal ) );

    poAppContextInternal->licensechallenge.ppdstrRights = f_ppdstrRights;
    poAppContextInternal->licensechallenge.cRights      = f_cRights;

    /* Request data now complete and cached in plicensechallenge; query for the required sizes. */
    dr = DRM_LA_CreateChallenge( &poAppContextInternal->licensechallenge,
                                  NULL,
                                 &cbChallenge );

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );

        ChkDR( DRM_SUCCESS );  /* This sets 'dr' and keeps the PREfast tool happy. */
    }

    cbRequired = cbChallenge;

    if (  f_pszChallenge == NULL
       || *f_pcchChallenge < ( cbRequired + SIZEOF( DRM_CHAR ) ) )
    {
        fTooSmall = TRUE;
    }

    if (  f_pcchSilentURL != NULL
       && *f_pcchSilentURL < ( dstrLAINFO.cchString + SIZEOF( DRM_CHAR ) ) )
    {
        fTooSmall = TRUE;
    }

    if (  f_pcchNonSilentURL != NULL
       && *f_pcchNonSilentURL < ( dstrLAINFO.cchString + SIZEOF( DRM_CHAR ) ) )
    {
        fTooSmall = TRUE;
    }

    if ( f_pcchSilentURL != NULL &&
         f_pchSilentURL  == NULL )
    {
        fTooSmall = TRUE;
    }

    if ( f_pcchNonSilentURL != NULL &&
         f_pchNonSilentURL  == NULL )
    {
        fTooSmall = TRUE;
    }

    /* If any of the above conditionals evaluate TRUE,
       return the required size(s). */
    if ( fTooSmall != FALSE )
    {
        if ( f_pcchSilentURL != NULL )
        {
            *f_pcchSilentURL = dstrLAINFO.cchString + SIZEOF( DRM_CHAR );
        }

        if ( f_pcchNonSilentURL != NULL )
        {
            *f_pcchNonSilentURL = dstrLAINFO.cchString + SIZEOF( DRM_CHAR );
        }

        *f_pcchChallenge = cbRequired + SIZEOF( DRM_CHAR );

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /* Challenge buffer holds ANSI chars, no need to align it. */
    ChkDR( DRM_LA_CreateChallenge( &poAppContextInternal->licensechallenge,
                                    f_pszChallenge,
                                   &cbChallenge ) );

    if ( f_pchSilentURL != NULL )
    {
        /* We will be appending a NULL later, so we need to reserve that space. */
        ( *f_pcchSilentURL )--;

        ChkDR( DRM_UTL_XMLDecodeUA( dstrLAINFO.pwszString,
                                    dstrLAINFO.cchString,
                                    f_pchSilentURL,
                                    f_pcchSilentURL ) );

        /* We're now guaranteed to have this extra space for the NULL. */
        f_pchSilentURL[ ( *f_pcchSilentURL )++ ] = 0;
    }

    if ( f_pchNonSilentURL != NULL )
    {
        /* We will be appending a NULL later, so we need to reserve that space. */
        ( *f_pcchNonSilentURL )--;

        ChkDR( DRM_UTL_XMLDecodeUA( dstrLAINFO.pwszString,
                                    dstrLAINFO.cchString,
                                    f_pchNonSilentURL,
                                    f_pcchNonSilentURL ) );

        /* We're now guaranteed to have this extra space for the NULL. */
        f_pchNonSilentURL[ ( *f_pcchNonSilentURL )++ ] = 0;
    }

    *f_pcchChallenge = cbChallenge;

    PUT_CHAR( f_pszChallenge, *f_pcchChallenge, '\0' );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: _GetLicenseState
**
** Synopsis: Function that retrieves the license state data for a specific
**           license KID based upon a set of requested rights ( actions ).
**
** Arguments:
**
** [f_rgpdstrRights]        -- An array of DRM string pointers. Each DRM
**                             string defines a specific right ( action ) to
**                             evaluate.
** [f_cRights]              -- Number of elements in the array mentioned
**                             above.
** [f_pdstrKID]             -- Pointer to a DRM string that defines the KID
**                             of the license.
** [f_poViewRightsContext]  -- Pointer to an initialized view rights context.
** [f_poDatastore]           -- Pointer to an initialized DST context.
** [f_poLicStateData]       -- Pointer to a structure that receives the
**                             license state data.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _GetLicenseState(
    IN const DRM_CONST_STRING        *f_rgpdstrRights[],
    IN       DRM_DWORD                f_cRights,
    IN       DRM_STRING              *f_pdstrKID,
    IN       DRM_VIEW_RIGHTS_CONTEXT *f_poViewRightsContext,
    IN       DRM_DST                 *f_poDatastore,
    OUT      DRM_LICENSE_STATE_DATA  *f_poLicStateData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__GetLicenseState );

    ChkArg( f_rgpdstrRights != NULL );
    ChkArg( f_cRights > 0 && f_cRights <= DRM_MAX_RIGHTS_REQUESTED );
    ChkDRMString( f_pdstrKID );
    ChkArg( f_poViewRightsContext != NULL );
    ChkArg( f_poDatastore != NULL );
    ChkArg( f_poLicStateData != NULL );

    ChkDR( DRM_UTL_DecodeKID(  ( const DRM_CONST_STRING * )f_pdstrKID,
                              &f_poViewRightsContext->KID ) );

    /* Collect the license state data for a specific KID with a set of rights ( actions ). */
    ChkDR( DRM_VIEWRIGHTS_GetLicenseAggregateData( f_rgpdstrRights,
                                                   f_poLicStateData,
                                                   f_cRights,
                                                   f_poViewRightsContext,
                                                   f_poDatastore,
                                                   FALSE,
                                                   DRM_ASD_AGGREGATE_SIMPLE_AND_LEAF_LICENSES,
                                                   NULL,
                                                   NULL ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _PrepareUplinks
**
** Synopsis: Function that prepares the uplink information for a specific
**           KID to be sent to the license server. The KID is in the
**           content header that is stored in a structure that has all
**           the information needed to build a license acquisition
**           challenge XML.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize.
** [f_poLicChlgContext]     -- Pointer to a structure that contains all
**                             the information needed to build the license
**                             acquisition challenge XML.
** [f_rgpdstrRights]        -- An array of DRM string pointers. Each DRM
**                             string defines a specific right ( action ) to
**                             evaluate.
** [f_cRights]              -- Number of elements in the array mentioned
**                             above.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _PrepareUplinks(
    IN       DRM_APP_CONTEXT        *f_poAppContext,
    IN       DRM_LICCHLG_CONTEXT_V3 *f_poLicChlgContext,
    IN const DRM_CONST_STRING       *f_rgpdstrRights[],
    IN       DRM_DWORD               f_cRights )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_DWORD                 i                    = 0;
    DRM_BOOL                  fUplinkAvailable     = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__PrepareUplinks );

    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->fStoreOpened );
    ChkArg( f_poLicChlgContext != NULL );
    ChkArg( f_rgpdstrRights != NULL );
    ChkArg( f_cRights > 0 && f_cRights <= DRM_MAX_RIGHTS_REQUESTED );

    /* Prepare for the view right context. */
    ChkDR( _PrepareViewRights( f_poAppContext ) );

    f_poLicChlgContext->m_cUplinks = 0;

    for ( ; ; )
    {
        i = f_poLicChlgContext->m_cUplinks;

        /*
        ** Try to get the uplink KID of a specific level ( defined by i ) of the KID
        ** in the content header.
        */
        dr = DRM_HDR_GetUplink( &f_poLicChlgContext->m_dstrContentHeader,
                                 i,
                                &f_poLicChlgContext->m_rgoUplinks[ i ].m_dstrKID );

        if ( DRM_FAILED( dr ) )
        {
            /* There is no uplink KID of a specific uplink level. */
            dr = DRM_SUCCESS;

            break;
        }

        if ( !fUplinkAvailable )
        {
            /* First time need to initialize the license evaluation context. */
            ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

            /* Record the fact that there is at least one uplink KID found. */
            fUplinkAvailable = TRUE;
        }

        /* Collect raw license state data of the uplink KID just found. */
        ChkDR( _GetLicenseState(  f_rgpdstrRights,
                                  f_cRights,
                                  ( DRM_STRING * )&f_poLicChlgContext->m_rgoUplinks[ i ].m_dstrKID,
                                 &poAppContextInternal->oViewRightsContext,
                                 &poAppContextInternal->oDatastoreHDS,
                                 &f_poLicChlgContext->m_rgoUplinks[ i ].m_oStateData[ 0 ] ) );

        /* Make sure the total level of uplink KIDs does not exceed the maximum. */
        if ( i + 1 >= NO_OF( f_poLicChlgContext->m_rgoUplinks ) )
        {
            break;
        }

        f_poLicChlgContext->m_cUplinks++;
    }

    /*
    ** If an uplink KID was not found in the header, search licenses in the XMR store
    */
    if ( !fUplinkAvailable )
    {
        DRM_XMR_LICENSE            *pXmrLicense = NULL;
        DRM_XMR_LICENSE             oXmrLicense = { 0 };
        DRM_KID                     oKIDLeaf    = { 0 };
        DRM_CONST_STRING            dstrKID     = { 0 };
        DRM_DST_SLOT_HINT           oSlotHint   = { 0 };
        DRM_LID                     oLIDLeaf    = { 0 };
        DRM_DWORD                   cbLicense   = 0;
        DRM_STACK_ALLOCATOR_CONTEXT oStack      = { 0 };

        /*
        ** Make sure the proper KID is loaded from the header
        */
        ChkDR( DRM_HDR_GetAttribute( &f_poLicChlgContext->m_dstrContentHeader,
                                     NULL,
                                     DRM_HEADER_ATTRIB_KID,
                                     &dstrKID,
                                     0 ) );

        ChkDR( DRM_UTL_DecodeKID( &dstrKID, &oKIDLeaf ) );

        /*
        ** Using this as a temporary stack to store and unpack the XMR license.
        ** This is immediately reused afterwards by the challenge generation code
        */
        oStack.pbStack = poAppContextInternal->pbDRMLicense;
        oStack.cbStack = poAppContextInternal->cbDRMLicense;

        /*
        ** Search for a matching license in the license store
        */

        /*
        ** Enumerate licenses matching the current KID
        */
        ChkDR( DRM_LST_InitEnum( &poAppContextInternal->oLicStoreContextXMR,
                                 &oKIDLeaf,
                                 TRUE,
                                 &poAppContextInternal->oLicEnum[0] ) );

        dr = DRM_LST_EnumNext( &poAppContextInternal->oLicEnum[0],
                               &oKIDLeaf,
                               &oLIDLeaf,
                               &oSlotHint,
                               &cbLicense );

        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;
        }
        else
        {
            DRM_BYTE *pbLicense = NULL;

            ChkDR( dr );

            ChkDR( DRM_STK_Alloc( &oStack,
                                  cbLicense,
                                  ( DRM_VOID ** )&pbLicense ) );

            /*
            ** Load and unpack the first matching license
            */
            ChkDR( DRM_LST_GetLicense( &poAppContextInternal->oLicStoreContextXMR,
                                       &oKIDLeaf,
                                       &oLIDLeaf,
                                       &oSlotHint,
                                       pbLicense,
                                       &cbLicense ) );

            ChkDR( DRM_XMR_UnpackLicense( pbLicense,
                                          cbLicense,
                                          &oStack,
                                          &oXmrLicense ) );

            pXmrLicense = &oXmrLicense;
        }

        /*
        ** Uplink found (not from content header): Set up uplink array
        */
        if ( pXmrLicense != NULL && XMR_IS_UPLINK_KID_VALID( pXmrLicense ) )
        {
            /*
            ** Copy the KID from the license
            */

             DRM_BYT_CopyBytes( &poAppContextInternal->oViewRightsContext.KID,
                                0,
                                pXmrLicense->containerOuter.containerKeys.UplinkKid.pbguidUplinkKID,
                                pXmrLicense->containerOuter.containerKeys.UplinkKid.iguidUplinkKID,
                                SIZEOF( DRM_KID ) );

            f_poLicChlgContext->m_rgoUplinks[f_poLicChlgContext->m_cUplinks].m_dstrKID.cchString = SIZEOF( f_poLicChlgContext->m_rgwchUplinkKID );
            f_poLicChlgContext->m_rgoUplinks[f_poLicChlgContext->m_cUplinks].m_dstrKID.pwszString = f_poLicChlgContext->m_rgwchUplinkKID;

            ChkDR( DRM_B64_EncodeW( ( DRM_BYTE * )&poAppContextInternal->oViewRightsContext.KID,
                                     SIZEOF( DRM_KID ),
                                     f_poLicChlgContext->m_rgwchUplinkKID,
                                     &f_poLicChlgContext->m_rgoUplinks[f_poLicChlgContext->m_cUplinks].m_dstrKID.cchString,
                                     0 ) );

            /*
            ** First time need to initialize the license evaluation context.
            */
            ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

            /*
            ** Collect raw license state data of the uplink KID just found.
            */
            ChkDR( DRM_VIEWRIGHTS_GetLicenseAggregateData(  f_rgpdstrRights,
                                                           &f_poLicChlgContext->m_rgoUplinks[f_poLicChlgContext->m_cUplinks].m_oStateData[ 0 ],
                                                            f_cRights,
                                                           &poAppContextInternal->oViewRightsContext,
                                                           &poAppContextInternal->oDatastoreHDS,
                                                            FALSE,
                                                            DRM_ASD_AGGREGATE_SIMPLE_AND_LEAF_LICENSES,
                                                            NULL,
                                                            NULL ) );

            f_poLicChlgContext->m_cUplinks++;
            /*
            ** Otherwise, assume that the license is correct but not a chained license - stop looking for uplinks
            */
        }
    }

ErrorExit:

    if( NULL != poAppContextInternal
     && NULL != poAppContextInternal->pbDRMLicense )
    {
        ZEROMEM( poAppContextInternal->pbDRMLicense,
                 poAppContextInternal->cbDRMLicense );
    }
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _PrepareLicenseChallenge
**
** Synopsis: Function that prepares for a structure that contains all
**           the information needed to build a license acquisition
**           challenge XML.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize.
** [f_rgpdstrRights]        -- An array of DRM string pointers. Each DRM
**                             string defines a specific right ( action ) to
**                             evaluate.
** [f_cRights]              -- Number of elements in the array mentioned
**                             above.
** [f_pchCustomData]        -- Pointer to a buffer that contains the custom data
**                             to be sent to the server. It may be NULL.
** [f_cchCustomData]        -- Size ( in characters ) of the custom data buffer.
**                             It must be 0 if f_pchCustomData is NULL.
** [f_poLicChlgContext]       -- Pointer to a structure that contains all
**                             the information needed to build the license
**                             acquisition challenge XML.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _PrepareLicenseChallenge(
    IN       DRM_APP_CONTEXT        *f_poAppContext,
    IN const DRM_CONST_STRING       *f_rgpdstrRights[],
    IN       DRM_DWORD               f_cRights,
    IN const DRM_CHAR               *f_pchCustomData,
    IN       DRM_DWORD               f_cchCustomData,
    OUT      DRM_LICCHLG_CONTEXT_V3 *f_poLicChlgContext )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BB_CONTEXT           *poBBXContext         = NULL;
    DRM_BOOL                  fEmptyV41Header      = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__PrepareLicenseChallenge );

    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->fStoreOpened );
    ChkArg( f_rgpdstrRights != NULL );
    ChkArg( f_cRights > 0 && f_cRights <= DRM_MAX_RIGHTS_REQUESTED );
    ChkArg( f_pchCustomData == NULL || f_cchCustomData > 0 );
    ChkArg( f_poLicChlgContext != NULL );

    poBBXContext = &poAppContextInternal->oBlackBoxContext;

    ChkArg( poBBXContext != NULL );
    ChkArg( poBBXContext->fInited );

    /* Prepare the custom data. */
    f_poLicChlgContext->m_dastrCustomData.pszString = f_pchCustomData;
    f_poLicChlgContext->m_dastrCustomData.cchString = f_cchCustomData;

    /* Prepare the right ( action ) list. */
    f_poLicChlgContext->m_ppdstrRights = f_rgpdstrRights;
    f_poLicChlgContext->m_cRights      = f_cRights;

    /* Prepare the content header. */
    DSTR_FROM_PB( &f_poLicChlgContext->m_dstrContentHeader,
                   poAppContextInternal->pwszHeaderData,
                   poAppContextInternal->cbHeaderData );

    /*
    ** Prepare for revocation version information. It is obtained from the cached
    ** values in the passed in DRM manager context.
    */
    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        DRM_DWORD iCRL = 0;
        ChkFAIL( iCRL < MAX_REVLIST_INFO );
        f_poLicChlgContext->m_rgoRevListInfo[ iCRL ].m_poGUID    = &g_guidRevocationTypePlayReadySilverLightRuntime;
        f_poLicChlgContext->m_rgoRevListInfo[ iCRL++ ].m_dwVersion =  poAppContextInternal->idCRLsCurrent.runtime;

        if( DRM_APPREVOCATION_IsAppRevocationSupported() )
        {
            ChkFAIL( iCRL < MAX_REVLIST_INFO );
            f_poLicChlgContext->m_rgoRevListInfo[ iCRL ].m_poGUID    = &g_guidRevocationTypePlayReadySilverLightApplication;
            f_poLicChlgContext->m_rgoRevListInfo[ iCRL++ ].m_dwVersion =  poAppContextInternal->idCRLsCurrent.playreadyapp;
        }

        if( DRM_WMDRMNET_IsWmdrmnetSupported() )
        {
            ChkFAIL( iCRL < MAX_REVLIST_INFO );
            f_poLicChlgContext->m_rgoRevListInfo[ iCRL ].m_poGUID    = &g_guidRevocationTypeWMDRMNET;
            f_poLicChlgContext->m_rgoRevListInfo[ iCRL++ ].m_dwVersion =  poAppContextInternal->idCRLsCurrent.wmdrmnet;
        }

        ChkFAIL( iCRL < MAX_REVLIST_INFO );
        f_poLicChlgContext->m_rgoRevListInfo[ iCRL ].m_poGUID    = &g_guidRevocationTypeRevInfo2;
        f_poLicChlgContext->m_rgoRevListInfo[ iCRL++ ].m_dwVersion =  poAppContextInternal->idCRLsCurrent.riv2;
    }

    /*
    ** Prepare for the uplink information.  Skip this function call if it is an
    ** empty V4.1 header because it doesn't have uplink and KID in it.
    */
    ChkDR( _IsEmptyV41Header( poAppContextInternal, &fEmptyV41Header ) );
    if( !fEmptyV41Header && DRM_VIEWRIGHTS_IsViewRightsSupported() )
    {
        ChkDR( _PrepareUplinks( f_poAppContext,
                                f_poLicChlgContext,
                                f_rgpdstrRights,
                                f_cRights ) );
    }

    f_poLicChlgContext->m_oStackAllocatorContext.pbStack = poAppContextInternal->pbDRMLicense;
    f_poLicChlgContext->m_oStackAllocatorContext.cbStack = poAppContextInternal->cbDRMLicense;

    /* Prepare the key information used for XML encryption/signature. */
    ChkDR( DRM_SOAPXML_InitXMLKey(  poAppContextInternal->pOEMContext,
                                   &f_poLicChlgContext->m_oXMLKey,
                                   &poBBXContext->CryptoContext ) );

    /*
    ** DRM App context should have already opened the domain store during
    ** its initialization.
    */
    f_poLicChlgContext->m_poDomainStoreContext = &poAppContextInternal->oDomainStoreContext;

    f_poLicChlgContext->m_poBBXContext         = poBBXContext;

    f_poLicChlgContext->m_poOEMContext         = poAppContextInternal->pOEMContext;

    ChkDR( DRM_NST_GenerateNonce( &poAppContextInternal->oNonceStoreContext,
                                  &f_poLicChlgContext->m_oNonce ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _GenerateLicenseChallengeV3
**
** Synopsis: Function that generates a license acquisition challenge XML to
**           be sent to the license server using the V3 protocol.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize.
** [f_rgpdstrRights]        -- An array of DRM string pointers. Each DRM
**                             string defines a specific right ( action ) to
**                             evaluate.
** [f_cRights]              -- Number of elements in the array mentioned
**                             above.
** [f_poDomainID]           -- Pointer to a domain ID specifying which domain
**                             certificates to include in the license challenge.
**                             Some of its fields (service ID, account ID, or
**                             revision) may be blank. Only the domain certificates
**                             matching the non-blank values ( currently only
**                             service ID) in this structure will be included
**                             in the license challenge. It can be NULL.
** [f_pchCustomData]        -- Pointer to a buffer that contains the custom data
**                             to be sent to the server. It may be NULL.
** [f_cchCustomData]        -- Size ( in characterss ) of the custom data buffer.
**                             It must be 0 if f_pchCustomData is NULL.
** [f_pchSilentURL]         -- Pointer to a buffer that receives the parsed out
**                             silent license acquisition URL. If caller does not want
**                             URL to be parsed out, this parameter should be NULL
**                             and f_pcchSilentURL should be NULL too.
** [f_pcchSilentURL]        -- Pointer to a variable that contains the size
**                             ( number of characters ) of the buffer that is used
**                             to store the silent license acquisition URL, and stores
**                             the actual size ( number of characters ) of the
**                             parsed out silent license acquisition URL. If caller does
**                             not want URL to be parsed out, this parameter should
**                             be NULL and f_pchSilentURL should be NULL too.
** [f_pchNonSilentURL]      -- Pointer to a buffer that receives the parsed out
**                             non silent license acquisition URL. If caller does not want
**                             URL to be parsed out, this parameter should be NULL
**                             and f_pcchNonSilentURL should be NULL too.
** [f_pcchNonSilentURL]     -- Pointer to a variable that contains the size
**                             ( number of characters ) of the buffer that is used
**                             to store the non silent license acquisition URL, and stores
**                             the actual size ( number of characters ) of the
**                             parsed out non silent license acquisition URL. If caller does
**                             not want URL to be parsed out, this parameter should
**                             be NULL and f_pchNonSilentURL should be NULL too.
** [f_pbChallenge]          -- Pointer to a buffer to receive a generated challenge
**                             byte stream. If it is NULL, the function returns an
**                             error and the size of the required buffer is stored
**                             in *f_pcbChallenge.
** [f_pcbChallenge]         -- Pointer to a variable that contains the size ( in bytes
**                             of the challenge buffer during input and receives the
**                             size ( in bytes ) of the challenge buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_NOTIMPL if only KID if specified in DRM manager
**                          context and license sync is not turned on.
**                          DRM_E_BUFFERTOOSMALL if the buffers to retrieve
**                          the URL or challenge data are not big enough.
**
**********************************************************************/
static DRM_RESULT _GenerateLicenseChallengeV3(
    IN                                            DRM_APP_CONTEXT  *f_poAppContext,
    __in_ecount( f_cRights )                const DRM_CONST_STRING *f_rgpdstrRights[],
    IN                                            DRM_DWORD         f_cRights,
    IN                                      const DRM_DOMAIN_ID    *f_poDomainID,
    __in_ecount_opt( f_cchCustomData )      const DRM_CHAR         *f_pchCustomData,
    IN                                            DRM_DWORD         f_cchCustomData,
    __out_ecount_opt( *f_pcchSilentURL )          DRM_CHAR         *f_pchSilentURL,
    IN OUT                                        DRM_DWORD        *f_pcchSilentURL,
    __out_ecount_opt( *f_pcchNonSilentURL )       DRM_CHAR         *f_pchNonSilentURL,
    IN OUT                                        DRM_DWORD        *f_pcchNonSilentURL,
    __out_bcount_opt( *f_pcbChallenge )           DRM_BYTE         *f_pbChallenge,
    IN OUT                                        DRM_DWORD        *f_pcbChallenge )
{
    DRM_RESULT                dr                                        = DRM_SUCCESS;
    DRM_RESULT                dr1                                       = DRM_SUCCESS;
    DRM_RESULT                dr2                                       = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal                      = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_CONST_STRING          dstrContentHeader                         = EMPTY_DRM_STRING;
    DRM_CONST_STRING          dstrServiceID                             = EMPTY_DRM_STRING;
    DRM_DWORD                 cbServiceID                               = 0;
    DRM_LICCHLG_CONTEXT_V3    oLicChlgContext                           = { 0 };
    const DRM_CONST_STRING   *rgpdstrRights[ DRM_MAX_RIGHTS_REQUESTED ] = { &g_dstrWMDRM_RIGHT_PLAYBACK };
    DRM_DWORD                 cRights                                   = 1;
    DRM_DWORD                 i                                         = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__GenerateLicenseChallengeV3 );

    UNREFERENCED_PARAMETER(dstrServiceID);
    UNREFERENCED_PARAMETER(cbServiceID);
    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->fStoreOpened );
    ChkArg( f_cRights <= DRM_MAX_RIGHTS_REQUESTED );
    ChkArg(  ( f_rgpdstrRights != NULL && f_cRights > 0 )
          || ( f_rgpdstrRights == NULL && f_cRights == 0 ) );
    ChkArg( f_pchCustomData == NULL || f_cchCustomData > 0 );
    ChkArg( f_pcchSilentURL != NULL || f_pchSilentURL == NULL );
    ChkArg( f_pchSilentURL == NULL || *f_pcchSilentURL > 0 );
    ChkArg( f_pcchNonSilentURL != NULL || f_pchNonSilentURL == NULL );
    ChkArg( f_pchNonSilentURL == NULL || *f_pcchNonSilentURL > 0 );
    ChkArg( f_pcbChallenge != NULL );
    ChkArg( f_pbChallenge == NULL || *f_pcbChallenge > 0 );

    ZEROMEM( &oLicChlgContext, SIZEOF( DRM_LICCHLG_CONTEXT_V3 ) );

    /*
    ** If user provides a list of rights, then use what the user provides.
    ** Otherwise use the default right list that contains only "Play".
    */
    if ( f_cRights > 0 )
    {
        DRMASSERT( f_cRights <= DRM_MAX_RIGHTS_REQUESTED );

        for ( i = 0; i < f_cRights; i++ )
        {
            rgpdstrRights[ i ] = f_rgpdstrRights[ i ];
        }

        cRights = f_cRights;
    }

    if (    poAppContextInternal->eHeaderInContext == DRM_CSP_KID
         || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID )
    {
        if( DRM_SNC_IsLicenseSyncSupported() )
        {
            ChkDR( _LoadHeaderFromSyncStore( f_poAppContext ) );
        }
        else
        {
            ChkDR( DRM_E_NOTIMPL );
        }
    }

    /*
    ** Clear the previous response fields in the app context.
    */
    poAppContextInternal->fResponseSignatureChecked = FALSE;
    poAppContextInternal->dwLastResponseProtocol    = DRM_SOAPXML_PROTOCOL_UNKNOWN;

    DSTR_FROM_PB( &dstrContentHeader,
                   poAppContextInternal->pwszHeaderData,
                   poAppContextInternal->cbHeaderData );

    if (  f_poDomainID == NULL
       || DRM_SOAPXML_IsBlankGUID( ( DRM_GUID * )&f_poDomainID->m_oServiceID ) )
    {
        ChkDR( DRM_LA_ParseServiceID( &dstrContentHeader,
                                      &oLicChlgContext.m_oServiceID ) );
    }
    else
    {
        MEMCPY( &oLicChlgContext.m_oServiceID,
                &f_poDomainID->m_oServiceID,
                 SIZEOF( DRM_GUID ) );
    }

    if ( f_pcchSilentURL != NULL )
    {
        /*
        ** Extract silent license acquisition URL only if the caller intends
        ** to do so by providing a non-NULL f_pcchSilentURL.
        */
        dr1 = DRM_LA_ParseLicenseAcquisitionURL( &dstrContentHeader,
                                                  TRUE,
                                                  f_pchSilentURL,
                                                  f_pcchSilentURL );

        if ( DRM_FAILED( dr1 ) && dr1 != DRM_E_BUFFERTOOSMALL )
        {
            /*
            ** If error happens and the error code is not DRM_E_BUFFERTOOSMALL,
            ** exit immediately. Otherwise exit after the required size of the
            ** challenge buffer gets calculated.
            */
            dr = dr1;

            ChkDR( dr );
        }
    }

    if ( f_pcchNonSilentURL != NULL )
    {
        /*
        ** Extract non silent license acquisition URL only if the caller intends
        ** to do so by providing a non-NULL f_pcchNonSilentURL.
        */
        dr2 = DRM_LA_ParseLicenseAcquisitionURL( &dstrContentHeader,
                                                  FALSE,
                                                  f_pchNonSilentURL,
                                                  f_pcchNonSilentURL );

        if ( DRM_FAILED( dr2 ) && dr2 != DRM_E_BUFFERTOOSMALL )
        {
            /*
            ** If error happens and the error code is not DRM_E_BUFFERTOOSMALL,
            ** exit immediately. Otherwise exit after the required size of the
            ** challenge buffer gets calculated.
            */
            dr = dr2;

            ChkDR( dr );
        }
    }

    /*
    ** Prepare for all the information needed for the license
    ** acquisition challenge XML.
    */
    ChkDR( _PrepareLicenseChallenge(  f_poAppContext,
                                      rgpdstrRights,
                                      cRights,
                                      f_pchCustomData,
                                      f_cchCustomData,
                                     &oLicChlgContext ) );

    ChkDR( DRM_LA_GenerateChallengeV3( &oLicChlgContext,
                                        f_pbChallenge,
                                        f_pcbChallenge ) );

    if ( dr1 == DRM_E_BUFFERTOOSMALL || dr2 == DRM_E_BUFFERTOOSMALL )
    {
        /*
        ** If the buffer for challenge is big enough but the buffer for URL
        ** is not, exit with error.
        */
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    Drm_LicenseAcq_GenerateChallenge
**
** Synopsis:    Using the current DRM header set by SetProperty DRM will
**              generate a liecense challenge that can be sent to a
**              license server using V2 or V3 protocol. The license server
**              generally issues a response that should be passed to
**              Drm_LicenseAcq_ProcessResponse.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_rgpdstrRights]        -- An array of DRM string pointers. Each DRM
**                             string defines a specific right ( action ) to
**                             evaluate.
** [f_cRights]              -- Number of elements in the array mentioned
**                             above.
** [f_poDomainID]           -- Pointer to a domain ID that is used by V3 license
**                             protocol. Some of its fields can be empty. It is
**                             ignored for V2 license protocol.
** [f_pchCustomData]        -- Pointer to a buffer that contains custom data
**                             to be sent to the server. It may be NULL.
** [f_cchCustomData]        -- Pointer to a variable that contains the size
**                             ( number of characters ) of the custom data.
** [f_pchSilentURL]         -- Pointer to a buffer that contains silent license
**                             acquisition URL that is parsed out from the content
**                             header. If it is NULL, the function returns an error
**                             and the size of the required buffer is stored in
**                             *f_pcchSilentURL.
** [f_pcchSilentURL]        -- Pointer to a variable that contains the size ( in
**                             characters of the silent license acquisition URL
**                             buffer during input and receives the actual size
**                             ( in characters ) of the silent license acquisition URL
**                             during output.
** [f_pchNonSilentURL]      -- Pointer to a buffer that contains non silent license
**                             acquisition URL that is parsed out from the content
**                             header. If it is NULL, the function returns an error
**                             and the size of the required buffer is stored in
**                             *f_pcchNonSilentURL.
** [f_pcchNonSilentURL]     -- Pointer to a variable that contains the size ( in
**                             characters of the non silent license acquisition
**                             URL buffer during input and receives the actual size
**                             ( in characters ) of the non silent license acquisition URL
**                             during output.
** [f_pbChallenge]          -- Pointer to a buffer to receive a generated challenge
**                             byte stream. If it is NULL, the function returns an
**                             error and the size of the required buffer is stored
**                             in *f_pcbChallenge.
** [f_pcbChallenge]         -- Pointer to a variable that contains the size ( in bytes
**                             of the challenge buffer during input and receives the
**                             size ( in bytes ) of the challenge buffer during output.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_V1_NOT_SUPPORTED if a V1 content header was only set.
**                          DRM_E_HEADER_NOT_SET if no content header was set.
**                          DRM_E_BUFFERTOOSMALL if the input URL buffer or challenge
**                          buffer were too small.
**                          Other error codes as defined in drmresults.h.
**
** Notes:                   Drm_Content_SetProperty with content header must be called
**                          prior to this function. Challenge is for the license of the
**                          content header stored in f_poAppConext.
**                          On return from this function the character count parameters
**                          will be updated with the number of characters copied to the
**                          buffer if successful or the required length of the buffer
**                          if the current buffer size is too small.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_LicenseAcq_GenerateChallenge(
    IN                                            DRM_APP_CONTEXT  *f_poAppContext,
    __in_ecount( f_cRights )                const DRM_CONST_STRING *f_rgpdstrRights[],
    IN                                            DRM_DWORD         f_cRights,
    IN                                      const DRM_DOMAIN_ID    *f_poDomainID,
    __in_ecount_opt( f_cchCustomData )      const DRM_CHAR         *f_pchCustomData,
    IN                                            DRM_DWORD         f_cchCustomData,
    __out_ecount_opt( *f_pcchSilentURL )          DRM_CHAR         *f_pchSilentURL,
    IN OUT                                        DRM_DWORD        *f_pcchSilentURL,
    __out_ecount_opt( *f_pcchNonSilentURL )       DRM_CHAR         *f_pchNonSilentURL,
    IN OUT                                        DRM_DWORD        *f_pcchNonSilentURL,
    __out_bcount_opt( *f_pcbChallenge )           DRM_BYTE         *f_pbChallenge,
    IN OUT                                        DRM_DWORD        *f_pcbChallenge )
{
    DRM_RESULT                dr        = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_LicenseAcq_GenerateChallenge );

    ChkArg( f_poAppContext != NULL );

    if ( DRM_SNC_IsLicenseSyncSupported() && poAppContextInternal->eHeaderInContext == DRM_CSP_KID )
    {
        /* Try to load the header from the sync store if it exists. */
        (DRM_VOID) _LoadHeaderFromSyncStore( f_poAppContext );
    }

    if (  poAppContextInternal->eHeaderInContext == DRM_CSP_V1_HEADER
       || poAppContextInternal->eHeaderInContext == DRM_CSP_V2_HEADER
       || poAppContextInternal->eHeaderInContext == DRM_CSP_KID )
    {
        ChkDR( _GenerateLicenseChallengeV2( f_poAppContext,
                                            f_rgpdstrRights,
                                            f_cRights,
                                            f_pchSilentURL,
                                            f_pcchSilentURL,
                                            f_pchNonSilentURL,
                                            f_pcchNonSilentURL,
                                            ( DRM_CHAR * )f_pbChallenge,
                                            f_pcbChallenge ) );
    }
    else if (  poAppContextInternal->eHeaderInContext == DRM_CSP_V4_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_V4_1_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_V2_4_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ )
    {
        ChkDR( _GenerateLicenseChallengeV3( f_poAppContext,
                                            f_rgpdstrRights,
                                            f_cRights,
                                            f_poDomainID,
                                            f_pchCustomData,
                                            f_cchCustomData,
                                            f_pchSilentURL,
                                            f_pcchSilentURL,
                                            f_pchNonSilentURL,
                                            f_pcchNonSilentURL,
                                            f_pbChallenge,
                                            f_pcbChallenge ) );
    }
    else
    {
        ChkDR( DRM_E_HEADER_NOT_SET );
    }

ErrorExit:

    _FreeDrmAppInternalContexts( ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext );

    DRM_PROFILING_LEAVE_SCOPE;

    ChkECC( ECC_Drm_LicenseAcq_GenerateChallenge, dr );

    return dr;
}

/******************************************************************************
**
** Function:    Drm_Device_GetProperty
**
** Synopsis:    Retrieves clients properties.  Available properties are defined
**              in the enum DRM_DEVICE_GET_PROPERTY.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_eProperty]            -- The property to retrieve.  Must be a value from
**                             the DRM_DEVICE_GET_PROPERTY enum.
** [f_pbProperties]         -- Buffer to receive the property value. The format
**                             of the data depends upon the property.
** [f_pcbProperties]        -- Size of output buffer; If this is too small,
**                             it is updated with the required size.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_HEADER_NOT_SET if no content header was set.
**                          DRM_E_BUFFERTOOSMALL - output buffer too small.
**                          Other error codes as defined in drmresults.h.
**
** Notes :                  DRM_DGP_CLIENT_INFO is UNICODE blob which is NOT
**                          NULL terminated.
**                          DRM_DGP_DRM_VERSION is UNICODE blob which is NOT
**                          NULL terminated.
**                          DRM_DGP_SECURITY_VERSION is UNICODE blob which is
**                          NOT NULL terminated.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Device_GetProperty(
    IN                                  DRM_APP_CONTEXT         *f_poAppContext,
    IN                                  DRM_DEVICE_GET_PROPERTY  f_eProperty,
     __out_bcount_opt( *f_pcbProperty ) DRM_BYTE                *f_pbProperty,
    IN OUT                              DRM_DWORD               *f_pcbProperty )
{
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_DWORD                 cchEncoded           = 0;
    DRM_CONST_STRING          dstrScratch          = EMPTY_DRM_STRING;
    DRM_DWORD                 cbMinRequired        = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Device_GetProperty );

    ChkArg(  f_poAppContext != NULL
          && f_pcbProperty != NULL );

    if ( poAppContextInternal->fBindThenCommit )
    {
        poAppContextInternal->fBindThenCommit = FALSE;
    }

    switch( f_eProperty )
    {
        case DRM_DGP_CLIENT_INFO:
            ZEROMEM( &poAppContextInternal->licensechallenge, SIZEOF( DRM_LICENSE_CHALLENGE ) );

            cchEncoded = SIZEOF( poAppContextInternal->licensechallenge.rgbStack );

            /*It will prepare context for Client info node.*/
            ChkDR( _PrepareForClientInfo( poAppContextInternal ) );

            dstrScratch.pwszString = poAppContextInternal->licensechallenge.pwszBBVer;
            ChkDR( DRM_STR_StringCchLengthW( poAppContextInternal->licensechallenge.pwszBBVer, VER_STRING_MAX_LEN, &dstrScratch.cchString ) );

            dr = DRM_LA_GetClientInfo( &poAppContextInternal->licensechallenge.clientid,
                                        poAppContextInternal->licensechallenge.rgbStack,
                                       &cchEncoded,
                                        ( DRM_CONST_STRING * )&g_dstrReqTagWMDRMPDClientVersionData,
                                       &dstrScratch,
                                        poAppContextInternal->licensechallenge.levelAppSecurity,
                                        poAppContextInternal->licensechallenge.idSubject,
                                        0,
                                        NULL,
                                       &poAppContextInternal->licensechallenge.dstrDeviceCert,
                                        NULL,
                                       &cbMinRequired,
                                       &poAppContextInternal->oBlackBoxContext.CryptoContext,
                                        poAppContextInternal->pOEMContext,
                                        TRUE,
                                        NULL );

            if ( dr != DRM_E_BUFFERTOOSMALL )
            {
                ChkDR( dr );

                ChkDR( DRM_SUCCESS );  /* This sets 'dr' and keeps the PREfast tool happy. */
            }

            if ( f_pbProperty == NULL ||
                 *f_pcbProperty < cbMinRequired )
            {
                *f_pcbProperty = cbMinRequired;

                ChkDR( DRM_E_BUFFERTOOSMALL );
            }

            cchEncoded = SIZEOF( poAppContextInternal->licensechallenge.rgbStack );

            ChkDR( DRM_LA_GetClientInfo( &poAppContextInternal->licensechallenge.clientid,
                                          poAppContextInternal->licensechallenge.rgbStack,
                                         &cchEncoded,
                                          ( DRM_CONST_STRING * )&g_dstrReqTagWMDRMPDClientVersionData,
                                         &dstrScratch,
                                          poAppContextInternal->licensechallenge.levelAppSecurity,
                                          poAppContextInternal->licensechallenge.idSubject,
                                          0,
                                          NULL,
                                         &poAppContextInternal->licensechallenge.dstrDeviceCert,
                                          ( _XMBContext* )f_pbProperty,
                                          f_pcbProperty,
                                         &poAppContextInternal->oBlackBoxContext.CryptoContext,
                                          poAppContextInternal->pOEMContext,
                                          TRUE,
                                          NULL ) );
            break;

        case DRM_DGP_PLAYREADY_VERSION:
            cbMinRequired = CB_DSTR( &g_dstrReqTagPlayReadyClientVersionData );

            if (  f_pbProperty == NULL
               || *f_pcbProperty < cbMinRequired )
            {
                *f_pcbProperty = cbMinRequired;

                ChkDR( DRM_E_BUFFERTOOSMALL );
            }

            MEMCPY( f_pbProperty, PB_DSTR( &g_dstrReqTagPlayReadyClientVersionData ), cbMinRequired );

            *f_pcbProperty = cbMinRequired;
            break;

        case DRM_DGP_WMDRMPD_VERSION:
            cbMinRequired = CB_DSTR( &g_dstrReqTagWMDRMPDClientVersionData );

            if (  f_pbProperty == NULL
               || *f_pcbProperty < cbMinRequired )
            {
                *f_pcbProperty = cbMinRequired;

                ChkDR( DRM_E_BUFFERTOOSMALL );
            }

            MEMCPY( f_pbProperty, PB_DSTR( &g_dstrReqTagWMDRMPDClientVersionData ), cbMinRequired );

            *f_pcbProperty = cbMinRequired;
            break;

        case DRM_DGP_SECURITY_VERSION:
            ChkDR( DRM_STR_StringCchLengthW(  poAppContextInternal->oBlackBoxContext.cachedCertValues.wszLegacyVersion,
                                              VER_STRING_MAX_LEN,
                                             &cbMinRequired ) );
            cbMinRequired *= SIZEOF( DRM_WCHAR );

            if (  f_pbProperty == NULL
               || *f_pcbProperty < cbMinRequired )
            {
                *f_pcbProperty = cbMinRequired;

                ChkDR( DRM_E_BUFFERTOOSMALL );
            }

            MEMCPY( f_pbProperty,
                    poAppContextInternal->oBlackBoxContext.cachedCertValues.wszLegacyVersion,
                    cbMinRequired );

            *f_pcbProperty = cbMinRequired;
            break;

        default:
            ChkDR( DRM_E_UNKNOWN_DEVICE_PROPERTY );
    }

ErrorExit:

    ChkECC( ECC_Drm_Device_GetProperty, dr );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
** Function:    Drm_Content_GetProperty
**
** Synopsis:    Retrieves content properties.  Available properties are defined
**              in the enum DRM_CONTENT_GET_PROPERTY.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_eProperty]            -- The property to retrieve.  Must be a value from
**                             the DRM_CONTENT_GET_PROPERTY enum.
** [f_pbProperties]         -- Buffer to receive the property value. The format
**                             of the data depends upon the property.
** [f_pcbProperties]        -- Size of output buffer; If this is too small,
**                             it is updated with the required size.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_HEADER_NOT_SET if no content header was set.
**                          DRM_E_CH_INCOMPATIBLE_HEADER_TYPE if a PlayReady object is requested
**                              but a different header type is set.
**                          DRM_E_BUFFERTOOSMALL - output buffer too small.
**                          Other error codes as defined in drmresults.h.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Content_GetProperty(
    __inout                                DRM_APP_CONTEXT          *f_poAppContext,
    __in                                   DRM_CONTENT_GET_PROPERTY  f_eProperty,
    __out_bcount_opt( *f_pcbPropertyData ) DRM_BYTE                 *f_pbPropertyData,
    __inout                                DRM_DWORD                *f_pcbPropertyData )
{
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_DWORD                 cbMinRequired        = 0;
    DRM_DWORD                 cbHeaderOffset       = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Content_GetProperty );

    ChkArg(  f_poAppContext    != NULL
          && f_pcbPropertyData != NULL );

    if ( poAppContextInternal->fBindThenCommit )
    {
        poAppContextInternal->fBindThenCommit = FALSE;
    }

    switch( f_eProperty )
    {
        case DRM_CGP_HEADER_KID:
            if (  poAppContextInternal->eHeaderInContext == DRM_CSP_V2_HEADER
               || poAppContextInternal->eHeaderInContext == DRM_CSP_V2_4_HEADER
               || poAppContextInternal->eHeaderInContext == DRM_CSP_V4_HEADER
               || poAppContextInternal->eHeaderInContext == DRM_CSP_V4_1_HEADER
               || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ )
            {
                DRM_CONST_STRING dstrKID = EMPTY_DRM_STRING;
                ChkBOOL( poAppContextInternal->pwszHeaderData != NULL, DRM_E_HEADER_NOT_SET );

                DSTR_FROM_PB( &poAppContextInternal->oLicEvalContext.dstrContentHeader,
                               poAppContextInternal->pwszHeaderData,
                               poAppContextInternal->cbHeaderData );

                ChkDR( DRM_HDR_GetAttribute( &poAppContextInternal->oLicEvalContext.dstrContentHeader,
                                              NULL,
                                              DRM_HEADER_ATTRIB_KID,
                                             &dstrKID,
                                              0 ) );

                cbMinRequired = CB_DSTR( &dstrKID );

                if (  f_pbPropertyData == NULL
                   || *f_pcbPropertyData < cbMinRequired )
                {
                    *f_pcbPropertyData = cbMinRequired;

                    ChkDR( DRM_E_BUFFERTOOSMALL );
                }

                MEMCPY( f_pbPropertyData, PB_DSTR( &dstrKID ), cbMinRequired );

                *f_pcbPropertyData = cbMinRequired;

            }
            else if ( poAppContextInternal->eHeaderInContext == DRM_CSP_V1_HEADER )
            {
                DRM_V1Header *pV1Header = (DRM_V1Header*)poAppContextInternal->pbDRMHeaderData ;

                DRMASSERT( pV1Header != NULL );

                cbMinRequired = pV1Header->cbKeyID;

                if (  f_pbPropertyData == NULL
                   || *f_pcbPropertyData < cbMinRequired )
                {
                    *f_pcbPropertyData = cbMinRequired;

                    ChkDR( DRM_E_BUFFERTOOSMALL );
                }

                MEMCPY( f_pbPropertyData, pV1Header->pbKeyID, cbMinRequired );

                *f_pcbPropertyData = cbMinRequired;
            }
            else if ( poAppContextInternal->eHeaderInContext == DRM_CSP_KID
                   || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID )
            {
                cbMinRequired = poAppContextInternal->cbHeaderData;

                if (  f_pbPropertyData == NULL
                   || *f_pcbPropertyData < cbMinRequired )
                {
                    *f_pcbPropertyData = cbMinRequired;

                    ChkDR( DRM_E_BUFFERTOOSMALL );
                }

                MEMCPY( f_pbPropertyData, poAppContextInternal->pbDRMHeaderData, cbMinRequired );

                *f_pcbPropertyData = cbMinRequired;
            }
            else
            {
                ChkDR( DRM_E_HEADER_NOT_SET );
            }
            break;

        case DRM_CGP_HEADER_TYPE:
            /* Return the type of header that's in the DRM manager context. */
            if ( *f_pcbPropertyData < SIZEOF( poAppContextInternal->eHeaderInContext ) )
            {
                *f_pcbPropertyData = SIZEOF( poAppContextInternal->eHeaderInContext );

                ChkDR( DRM_E_BUFFERTOOSMALL );
            }

            MEMCPY(  f_pbPropertyData,
                    &poAppContextInternal->eHeaderInContext,
                     SIZEOF( poAppContextInternal->eHeaderInContext ) );

            *f_pcbPropertyData = SIZEOF( poAppContextInternal->eHeaderInContext );
            break;

        case DRM_CGP_HEADER:
            /*
            ** Return the header data from the DRM manager context.
            */
            ChkBOOL( poAppContextInternal->eHeaderInContext != DRM_CSP_HEADER_NOT_SET, DRM_E_HEADER_NOT_SET );

            if ( *f_pcbPropertyData < poAppContextInternal->cbHeaderData )
            {
                *f_pcbPropertyData = poAppContextInternal->cbHeaderData;

                ChkDR( DRM_E_BUFFERTOOSMALL );
            }

            ChkArg( f_pbPropertyData != NULL );
            MEMCPY( f_pbPropertyData,
                    poAppContextInternal->pwszHeaderData,
                    poAppContextInternal->cbHeaderData );

            *f_pcbPropertyData = poAppContextInternal->cbHeaderData;
            break;

        case DRM_CGP_PLAYREADY_OBJ:
            /*
            ** Return the PlayReady Object from the DRM manager context
            */
            ChkBOOL( poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ
                  || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID,
                     DRM_E_CH_INCOMPATIBLE_HEADER_TYPE );


            if ( poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID )
            {
                cbHeaderOffset = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) * SIZEOF( DRM_WCHAR );
            }

            if ( *f_pcbPropertyData < poAppContextInternal->cbTotalHeaderData - cbHeaderOffset )
            {
                *f_pcbPropertyData = poAppContextInternal->cbTotalHeaderData - cbHeaderOffset ;

                ChkDR( DRM_E_BUFFERTOOSMALL );
            }

            MEMCPY( f_pbPropertyData,
                   &poAppContextInternal->pbDRMHeaderData[cbHeaderOffset],
                    poAppContextInternal->cbTotalHeaderData - cbHeaderOffset  );

            *f_pcbPropertyData = poAppContextInternal->cbTotalHeaderData - cbHeaderOffset ;

            break;

        case DRM_CGP_CIPHER_TYPE:
            /*
            ** Return the content cipher type from the DRM manager context
            */
            ChkBOOL(    poAppContextInternal->eHeaderInContext == DRM_CSP_V4_HEADER
                     || poAppContextInternal->eHeaderInContext == DRM_CSP_V4_1_HEADER
                     || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ
                     || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID,
                     DRM_E_CH_INCOMPATIBLE_HEADER_TYPE );

            if ( *f_pcbPropertyData < SIZEOF( DRM_SUPPORTED_CIPHERS ) )
            {
                *f_pcbPropertyData = SIZEOF( DRM_SUPPORTED_CIPHERS );

                ChkDR( DRM_E_BUFFERTOOSMALL );
            }

            if (    poAppContextInternal->eHeaderInContext == DRM_CSP_V4_HEADER
                 || poAppContextInternal->eHeaderInContext == DRM_CSP_V4_1_HEADER )
            {
                ChkDR( DRM_PRO_GetCipherTypeFromPlayReadyHeader(
                                              poAppContextInternal->pbDRMHeaderData,
                                              poAppContextInternal->cbTotalHeaderData,
                                              ( DRM_SUPPORTED_CIPHERS * )f_pbPropertyData ) );
            }
            else if ( poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ )
            {
                ChkDR( DRM_PRO_GetCipherType( poAppContextInternal->pbDRMHeaderData,
                                              poAppContextInternal->cbTotalHeaderData,
                                              ( DRM_SUPPORTED_CIPHERS * )f_pbPropertyData ) );
            }
            else if ( poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID )
            {
                cbHeaderOffset = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) * SIZEOF( DRM_WCHAR );
                ChkDR( DRM_PRO_GetCipherType( &poAppContextInternal->pbDRMHeaderData[cbHeaderOffset],
                                               poAppContextInternal->cbTotalHeaderData - cbHeaderOffset,
                                              ( DRM_SUPPORTED_CIPHERS * )f_pbPropertyData ) );
            }
            else
            {
                DRMASSERT( FALSE );
            }

            break;

        case DRM_CGP_DECRYPTORSETUP:
        {
            ChkBOOL(    poAppContextInternal->eHeaderInContext == DRM_CSP_V4_1_HEADER
                     || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ
                     || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID,
                     DRM_E_CH_INCOMPATIBLE_HEADER_TYPE );


            if ( *f_pcbPropertyData < SIZEOF( eDRM_DECRYPTORSETUP_TYPE ) )
            {
                *f_pcbPropertyData = SIZEOF( eDRM_DECRYPTORSETUP_TYPE );

                ChkDR( DRM_E_BUFFERTOOSMALL );
            }

            if ( poAppContextInternal->eHeaderInContext == DRM_CSP_V4_1_HEADER )
            {
                ChkDR( DRM_PRO_GetDecryptorSetupTypeFromPlayReadyHeader(
                      poAppContextInternal->pbDRMHeaderData,
                      poAppContextInternal->cbTotalHeaderData,
                    ( eDRM_DECRYPTORSETUP_TYPE * )f_pbPropertyData ) );
            }
            else if ( poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ )
            {
                ChkDR( DRM_PRO_GetDecryptorSetupType(
                      poAppContextInternal->pbDRMHeaderData,
                      poAppContextInternal->cbTotalHeaderData,
                    ( eDRM_DECRYPTORSETUP_TYPE * )f_pbPropertyData ) );
            }
            else if ( poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID )
            {
                cbHeaderOffset = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) * SIZEOF( DRM_WCHAR );
                ChkDR( DRM_PRO_GetDecryptorSetupType(
                     &poAppContextInternal->pbDRMHeaderData[cbHeaderOffset],
                      poAppContextInternal->cbTotalHeaderData - cbHeaderOffset,
                    ( eDRM_DECRYPTORSETUP_TYPE * )f_pbPropertyData ) );
            }
            else
            {
                ChkDR( DRM_E_CH_INCOMPATIBLE_HEADER_TYPE );
            }

            break;
        }
        default:
            ChkArg( FALSE );
    }

ErrorExit:

    ChkECC( ECC_Drm_Content_GetProperty, dr );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    _ProcessLicenseResponseV2
**
** Synopsis:    Function that processes a response from a license server
**              using the V2 license protocol.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_pfnCallback]          -- Pointer to a callback function which is called
**                             for each license in response. It can be NULL
**                             and used in V2 protocol only.
** [f_pvCallbackContext]    -- User context returned in callback function.
**                             It can be NULL and used in V2 protocol only.
** [f_pbResponse]           -- Pointer to a response blob from a license server.
** [f_cbResponse]           -- Count of bytes in pbResponse.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT _ProcessLicenseResponseV2(
    IN DRM_APP_CONTEXT           *f_poAppContext,
    IN pfnLicenseResponseCallback f_pfnCallback,
    IN DRM_VOID                  *f_pvCallbackContext,
    IN DRM_BYTE                  *f_pbResponse,
    IN DRM_DWORD                  f_cbResponse )
{
    DRM_RESULT                dr                   =  DRM_SUCCESS;
    DRM_DWORD                 i                    =  0;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal =  ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_VIEW_RIGHTS_CONTEXT  *pcontextViewRights   = &poAppContextInternal->oViewRightsContext;

    ChkArg( f_poAppContext != NULL );

    if ( !poAppContextInternal->fStoreOpened )
    {
        ChkDR( DRM_E_DRMNOTINIT );
    }

    if ( poAppContextInternal->fBindThenCommit )
    {
        poAppContextInternal->fBindThenCommit = FALSE;
    }

    ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

    /* Explicitly zeroed the view right context. */
    ZEROMEM( pcontextViewRights, SIZEOF( DRM_VIEW_RIGHTS_CONTEXT ) );

    pcontextViewRights->pBBContext = &poAppContextInternal->oBlackBoxContext;

    for ( i = 0; i < DRM_MAX_LICENSE_CHAIN_DEPTH; i++ )
    {
        pcontextViewRights->rgpLicQueryContext[ i ] = &poAppContextInternal->oLicEnum[ i ];
    }

    pcontextViewRights->pbGlobalSecStore  = &poAppContextInternal->oSecStoreGlobalContext;
    pcontextViewRights->pbLIDSecStore     = &poAppContextInternal->rgcontextSST[ 0 ];
    pcontextViewRights->pbLicenseStoreXML = &poAppContextInternal->oLicStoreContextXML;
    pcontextViewRights->pLicEval          = &poAppContextInternal->oLicEvalContext;
    pcontextViewRights->pbBuffer          =  poAppContextInternal->pbOpaqueBuffer
                                             + __CB_DECL( poAppContextInternal->cbTotalHeaderData );
    pcontextViewRights->cbBuffer          =  poAppContextInternal->cbOpaqueBuffer
                                             -  poAppContextInternal->cbTotalHeaderData;

    if( DRM_APPREVOCATION_IsAppRevocationSupported() )
    {
        poAppContextInternal->oLicEvalContext.idCRLsCurrent.wmdrmapp = DRM_APP_REVOCATION_VERSION_NONE;
        poAppContextInternal->oLicEvalContext.idCRLsCurrent.fUpdatedRevocationList = FALSE;

        dr = Drm_Revocation_GetList(  (DRM_APP_CONTEXT*)poAppContextInternal,
                                                DRM_REVOCATION_TYPE_WMDRM_APP,
                                                NULL,
                                                NULL,
                                               &poAppContextInternal->oLicEvalContext.idCRLsCurrent.wmdrmapp );
        if ( dr == DRM_E_DSTSLOTNOTFOUND )
        {
            ChkDR( DRM_SUCCESS );  /* This sets 'dr' and keeps the PREfast tool happy. */
        }
        else
        {
            ChkDR( dr );
        }
    }

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        DRM_ANSI_CONST_STRING dastrRevocationInfoData = EMPTY_DRM_STRING;

        DASTR_FROM_PB( &dastrRevocationInfoData, f_pbResponse, f_cbResponse );

        ChkDR( DRM_RVK_ProcessRevocationInfo( &poAppContextInternal->oBlackBoxContext.CryptoContext,
                                              &poAppContextInternal->oRevContext,
                                              &dastrRevocationInfoData,
                                               poAppContextInternal->pbRevocationBuffer,
                                               poAppContextInternal->cbRevocationBuffer,
                                              &poAppContextInternal->oLicEvalContext.appcert,
                                              &poAppContextInternal->oLicEvalContext.idCRLsCurrent ) );

        if ( poAppContextInternal->oLicEvalContext.idCRLsCurrent.fUpdatedRevocationList )
        {
            MEMCPY( &poAppContextInternal->idCRLsCurrent,
                    &poAppContextInternal->oLicEvalContext.idCRLsCurrent,
                    SIZEOF( DRM_REVLIST_ID_VALUES ) );

            /*
            ** No need to clear the fUpdatedRevocationList flag - the lic eval
            ** object will be zeroed out before being populated again prior to
            ** this call.
            */
        }
    }

    dr = DRM_WMDRM_LA_ProcessResponse(  f_pbResponse,
                                        f_cbResponse,
                                       &poAppContextInternal->oLicEvalContext,
                                       &poAppContextInternal->oLicStoreContextXML,
                                       &poAppContextInternal->oLicStoreContextXMR,
                                       &poAppContextInternal->oLicEnum[0],
                                       &poAppContextInternal->oFFLicense.inclusionListCallback,
                                        f_pfnCallback,
                                        f_pvCallbackContext,
                                       &poAppContextInternal->oDatastoreHDS,
                                        pcontextViewRights,
                                        poAppContextInternal->pbDRMLicense,
                                        DRM_SNC_IsLicenseSyncSupported() ? &poAppContextInternal->contextSync : NULL,
                                        NULL );

ErrorExit:

    _FreeDrmAppInternalContexts( poAppContextInternal );

    return dr;
}

/*********************************************************************
**
** Function: _ExtractDataFromLicenseResponse
**
** Synopsis: Function that extracts, processes and stores information
**           received from the server. There are three types of
**           data from the server: licenses, revocation information
**           and transaction ID.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize.
** [f_dwFlags]              -- Flags of DRM_PROCESS_LIC_RESPONSE_FLAG
** [f_pdastrResponse]       -- Pointer to a DRM UTF8/ANSI string that contains
**                             the server response.
** [f_pbTransactionID]      -- Pointer to a buffer that is used to receive
**                             the extracted transaction ID.
** [f_pcbTransactionID]     -- Pointer to a variable that receives the size
**                             (number of bytes) of the extracted transaction ID.
** [f_poLicAcks]            -- Array of structures that reveives the result of
**                             license processing and storage.
** [f_pcLicAcks]            -- Pointer to a variable that:
**                             On input, contains the maximum number of acks that
**                             can be used in f_poLicAcks.
**                             On output, receives the total number
**                             of elements used in the array mentioned above.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the passed in DRM string to
**                          store the transaction ID is not NULL but is not big
**                          enough.
**                          DRM_E_LRB_INVALID_REVOCATION_PACKAGE if XML structure
**                          of response is invalid
**                          DRM_E_LICACQ_TOO_MANY_LICENSES if *f_pcLicAcks is not large enough.
**
**********************************************************************/
static DRM_RESULT _ExtractDataFromLicenseResponse(
    __in                              DRM_APP_CONTEXT               *f_poAppContext,
    __in                              DRM_PROCESS_LIC_RESPONSE_FLAG  f_dwFlags,
    __in                              DRM_ANSI_CONST_STRING         *f_pdastrResponse,
    __out_bcount(*f_pcbTransactionID) DRM_BYTE                      *f_pbTransactionID,
    __out                             DRM_DWORD                     *f_pcbTransactionID,
    __out_ecount(*f_pcLicAcks)        DRM_LICENSE_ACK               *f_poLicAcks,
    __inout                           DRM_DWORD                     *f_pcLicAcks )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_LICRESP_CONTEXT_V3    oLicRespContext      = { 0 };
    DRM_BOOL                  fEmptyV41Header      = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__ExtractDataFromLicenseResponse );

    ChkArg( f_poAppContext != NULL );
    ChkDRMANSIString( f_pdastrResponse );
    ChkArg( f_pbTransactionID != NULL );
    ChkArg( f_pcbTransactionID != NULL );
    ChkArg( f_poLicAcks != NULL );
    ChkArg( f_pcLicAcks != NULL );
    ChkArg( f_dwFlags == DRM_PROCESS_LIC_RESPONSE_NO_FLAGS
         || 0 != ( f_dwFlags & DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED ) );

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        /*
        ** Prepare the license evaluation context for information
        ** needed to update the sync store. For example, the content
        ** header.
        */
        ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

        /*
        ** Prepare for the view rights context used to store content
        ** header to the sync store.
        */
        ChkDR( _PrepareViewRights( f_poAppContext ) );
    }

    /* Prepare the license response context data structure. */
    oLicRespContext.m_pbLicenseBuffer     = poAppContextInternal->pbDRMLicense;
    oLicRespContext.m_cbLicenseBufferSize = poAppContextInternal->cbDRMLicense;
    oLicRespContext.m_poDatastore         = &poAppContextInternal->oDatastoreHDS;
    oLicRespContext.m_poLicEvalContext    = &poAppContextInternal->oLicEvalContext;
    oLicRespContext.m_poLicStoreContext   = &poAppContextInternal->oLicStoreContextXMR;
    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        oLicRespContext.m_poSyncContext       = &poAppContextInternal->contextSync;
    }
    oLicRespContext.m_poViewRightsContext = &poAppContextInternal->oViewRightsContext;
    oLicRespContext.m_poBBXContext        = &poAppContextInternal->oBlackBoxContext;
    oLicRespContext.m_poSecStoreGlobalContext = &poAppContextInternal->oSecStoreGlobalContext;

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        oLicRespContext.m_poRevStoreContext   = &poAppContextInternal->oRevContext;
        oLicRespContext.m_pbRevocationBuffer  = poAppContextInternal->pbRevocationBuffer;
        oLicRespContext.m_cbRevocationBuffer  = poAppContextInternal->cbRevocationBuffer;
    }

    oLicRespContext.m_poNonceStoreContext = &poAppContextInternal->oNonceStoreContext;
    oLicRespContext.m_eLicResponseFlag    = (DRM_PROCESS_LIC_RESPONSE_FLAG)f_dwFlags;

    /* Extract and process all the licenses in the server response. */
    dr = DRM_LA_ProcessResponseV3( &oLicRespContext,
                                   f_pdastrResponse,
                                   f_pbTransactionID,
                                   f_pcbTransactionID,
                                   f_poLicAcks,
                                   f_pcLicAcks );

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        DRM_DWORD cProcessed = 0;

        /* Ignore the error if there are no licenses in the response */
        if( DRM_E_XMLNOTFOUND != dr )
        {
            ChkDR( dr );
        }

        /* Process any revocation information in the server response. */
        ChkDR( DRM_LA_ProcessRevocationPackage( &oLicRespContext,
                                                 f_pdastrResponse,
                                                &cProcessed ) );

        if( cProcessed > 0 )
        {
            /*
            ** Make sure the revocation version values cached in DRM manager get
            ** refreshed. No need to clear the fUpdatedRevocationList flag - the
            ** lic eval object will be zeroed out before being populated again
            ** prior to this call.
            */
            MEMCPY( &poAppContextInternal->idCRLsCurrent,
                    &poAppContextInternal->oLicEvalContext.idCRLsCurrent,
                    SIZEOF( DRM_REVLIST_ID_VALUES ) );
        }
    }
    else
    {
        ChkDR( dr );
    }

    /* Update the KID in APP context, it was skipped in setup eval object because it wasn't available at the time. */
    ChkDR( _IsEmptyV41Header( poAppContextInternal, &fEmptyV41Header ) );
    if( fEmptyV41Header )
    {
        MEMCPY(
            &poAppContextInternal->KID,
            &oLicRespContext.m_poLicEvalContext->KID,
            SIZEOF( DRM_KID ) );
    }

ErrorExit:

    _FreeDrmAppInternalContexts( poAppContextInternal );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*********************************************************************
**
** Function: _ProcessLicenseResponseV3
**
** Synopsis: Function that processes a license acquisition challenge
**           response from the license server using the V3 protocol.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize.
** [f_dwFlags]              -- Flags of DRM_PROCESS_LIC_RESPONSE_FLAG
** [f_pbResponse]           -- Pointer to a buffer that contains the response
**                             response from the server.
** [f_cbResponse]           -- Size ( number of bytes ) of the response from the
**                             server. It must be bigger than 0.
** [f_poLicenseResponse]    -- Point to a data structure to reveive the result
**                             of license processing response.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_S_MORE_DATA if custom data is available from
**                          the server.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_XMLNOTFOUND if expected XML node is not found.
**                          DRM_E_SOAPXML_XML_FORMAT if SOAP/XML error occurs.
**                          DRM_E_LICACQ_TOO_MANY_LICENSES if f_poLicenseResponse->m_cMaxAcks
**                          is non-zero but not large enough for the number of licenses in
**                          the response OR if f_poLicenseResponse->m_cMaxAcks is 0 and
**                          there are more than DRM_MAX_LICENSE_ACK licenses in the response.
**
**********************************************************************/
static DRM_RESULT _ProcessLicenseResponseV3(
    IN                          DRM_APP_CONTEXT                *f_poAppContext,
    IN                          DRM_PROCESS_LIC_RESPONSE_FLAG   f_dwFlags,
    __in_bcount( f_cbResponse ) DRM_BYTE                       *f_pbResponse,
    IN                          DRM_DWORD                       f_cbResponse,
    OUT                         DRM_LICENSE_RESPONSE           *f_poLicenseResponse )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_RESULT                dr2                  = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_ANSI_CONST_STRING     dastrResponse        = EMPTY_DRM_STRING;
    DRM_SUBSTRING             dasstrResponse       = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING             dasstrData           = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING             dasstrSignature      = EMPTY_DRM_SUBSTRING;
    DRM_BOOL                  fOverwriteResult     = FALSE;
    DRM_DWORD                 cAcks                = 0;
    DRM_LICENSE_ACK          *pAcks                = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_LICACQV3, PERF_FUNC__ProcessLicenseResponseV3 );

    UNREFERENCED_PARAMETER(dasstrData);
    UNREFERENCED_PARAMETER(dasstrSignature);
    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->fStoreOpened );
    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );
    ChkArg( f_poLicenseResponse != NULL );
    ChkArg( ( f_poLicenseResponse->m_pAcks == NULL ) == ( f_poLicenseResponse->m_cMaxAcks == 0 ) );
    fOverwriteResult = TRUE;

    DASTR_FROM_PB( &dastrResponse, f_pbResponse, f_cbResponse );

    dasstrResponse.m_ich = 0;
    dasstrResponse.m_cch = dastrResponse.cchString;

#if DRM_USE_SOAP

    ChkDR( DRM_SOAPXML_SkipSOAPHeaders(  ( DRM_CHAR * )dastrResponse.pszString,
                                        &dasstrResponse ) );
#endif

    ChkDR( DRM_LA_CheckForCustomData( f_pbResponse,
                                      f_cbResponse ) );
    if ( dr == DRM_S_MORE_DATA )
    {
        dr2 = DRM_S_MORE_DATA;
    }

    f_poLicenseResponse->m_cbTransactionID = DRM_MAX_TRANSACTION_ID;

    cAcks = ( f_poLicenseResponse->m_cMaxAcks == 0 ) ? DRM_MAX_LICENSE_ACK : f_poLicenseResponse->m_cMaxAcks;
    pAcks = ( f_poLicenseResponse->m_pAcks == NULL ) ? f_poLicenseResponse->m_rgoAcks : f_poLicenseResponse->m_pAcks;

    /* Extract license data from the license response. */
    dr =   _ExtractDataFromLicenseResponse(  f_poAppContext,
                                             f_dwFlags,
                                            &dastrResponse,
                                             f_poLicenseResponse->m_rgbTransactionID,
                                            &f_poLicenseResponse->m_cbTransactionID,
                                             pAcks,
                                            &cAcks );
    if( DRM_SUCCEEDED( dr ) || dr == DRM_E_LICACQ_TOO_MANY_LICENSES )
    {
        f_poLicenseResponse->m_cAcks = cAcks;
    }
    ChkDR( dr );

ErrorExit:

    /*
    **  DRM_E_LRB_INVALID_REVOCATION_PACKAGE means that response is valid XML document
    **  but structure of this document is invalid for license response
     */
    if ( dr == DRM_E_REVOCATION_INVALID_PACKAGE )
    {
        /* Let use error returned by DRM_SOAPXML_ParseStatusCodeAndChooseBest */
        dr = DRM_SUCCESS;
    }
    else if ( DRM_SUCCEEDED( dr ) && dr2 == DRM_S_MORE_DATA )
    {
        dr = DRM_S_MORE_DATA;
    }

    dr = DRM_SOAPXML_ParseStatusCodeAndChooseBest(f_pbResponse,
                                                  f_cbResponse,
                                                  dr,
                                                  (fOverwriteResult && f_poLicenseResponse != NULL) ? &f_poLicenseResponse->m_dwResult : NULL );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/**********************************************************************
**
** Function:    Drm_LicenseAcq_ProcessResponse
**
** Synopsis:    API that processes a response from a license server.
**              Usually this is a series of licenses that will
**              ultimately be stored in the device license store. This
**              API can handle both V2 and V3 license protocol.
**
** Arguments:
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_dwFlags]              -- Flags of DRM_PROCESS_LIC_RESPONSE_FLAG
** [f_pfnCallback]          -- Pointer to a callback function which is called
**                             for each license in response. It can be NULL
**                             and used in V2 protocol only.
** [f_pvCallbackContext]    -- User context returned in callback function.
**                             It can be NULL and used in V2 protocol only.
** [f_pbResponse]           -- Pointer to a response blob from a license server.
** [f_cbResponse]           -- Count of bytes in pbResponse.
** [f_poLicenseResponse]    -- Point to a data structure to reveive the result
**                             of license processing response. One of its
**                             members tells whether the response is a V2 or
**                             V3 or unknown response. Other fields are used
**                             by V3 protocol only and should be ignored in
**                             V2 protocol.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_LICACQ_TOO_MANY_LICENSES if f_poLicenseResponse->m_cMaxAcks
**                          is non-zero but not large enough for the number of licenses in
**                          the response OR if f_poLicenseResponse->m_cMaxAcks is 0 and
**                          there are more than DRM_MAX_LICENSE_ACK licenses in the response.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_LicenseAcq_ProcessResponse(
    IN                          DRM_APP_CONTEXT                *f_poAppContext,
    IN                          DRM_PROCESS_LIC_RESPONSE_FLAG   f_dwFlags,
    IN                          pfnLicenseResponseCallback      f_pfnCallback,
    IN                          DRM_VOID                       *f_pvCallbackContext,
    __in_bcount( f_cbResponse ) DRM_BYTE                       *f_pbResponse,
    IN                          DRM_DWORD                       f_cbResponse,
    OUT                         DRM_LICENSE_RESPONSE           *f_poLicenseResponse )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_LicenseAcq_ProcessResponse );

    ChkArg( f_poLicenseResponse != NULL );

    if ( _IsV2LicenseResponse( f_pbResponse, f_cbResponse ) )
    {
        f_poLicenseResponse->m_eType = eV2Protocol;

        ChkDR( _ProcessLicenseResponseV2( f_poAppContext,
                                          f_pfnCallback,
                                          f_pvCallbackContext,
                                          f_pbResponse,
                                          f_cbResponse ) );
    }
    else
    {
        f_poLicenseResponse->m_eType = eV3Protocol;

        ChkDR( _ProcessLicenseResponseV3( f_poAppContext,
                                          f_dwFlags,
                                          f_pbResponse,
                                          f_cbResponse,
                                          f_poLicenseResponse ) );
    }

ErrorExit:

    if ( DRM_FAILED( dr ) && f_poLicenseResponse != NULL )
    {
        f_poLicenseResponse->m_eType = eUnknownProtocol;
    }

    _FreeDrmAppInternalContexts( ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext );

    DRM_PROFILING_LEAVE_SCOPE;

    ChkECC( ECC_Drm_LicenseAcq_ProcessResponse, dr );

    return dr;
}

typedef struct __tagBindCommit
{
    DRM_BYTE                rgbPassword[ __CB_DECL( DRM_SHA1_DIGEST_LEN ) ];

    const DRM_CONST_STRING *rgpdstrActions[ DRM_MAX_ACTIONS ];

    DRM_DWORD               cActions;
} DRM_BIND_COMMIT_DATA;

/*
** Helper function: Updates symm opts for XML licenses at chain depth f_iChainDepth.
** Preconditions:
**    - License data is loaded into App Context license buffer; Bind operations were successful.
**    - Caller has checked that license is XML and is not yet verified with attrib.
** Postconditions:
**    - Overwrites license buffer in app context with updated license.
*/
static DRM_RESULT _UpdateSymmOptsForLicense( __in    DRM_DWORD                 f_iChainDepth,
                                             __inout DRM_APP_CONTEXT_INTERNAL *f_poAppContextInternal,
                                             __in    DRM_DWORD                 f_cbLicense )
{
    DRM_RESULT       dr                 = DRM_SUCCESS;
    DRM_DWORD        dwPriority         = 0;
    DRM_CONST_STRING dstrPriority       = EMPTY_DRM_STRING;
    DRM_STRING       dstrUpdatedLicense = EMPTY_DRM_STRING;

    DRMASSERT( f_poAppContextInternal != NULL );
    DRMASSERT( f_iChainDepth < DRM_MAX_LICENSE_CHAIN_DEPTH );
    DRMASSERT( f_cbLicense > 0 && f_cbLicense < DRM_MAX_LICENSESIZE );

    DSTR_FROM_PB( &( dstrUpdatedLicense ), f_poAppContextInternal->pbDRMLicense, f_cbLicense );
    ChkDR( DRM_SYMOPT_LIC_UpdateLicenseWithSymmetricData( &dstrUpdatedLicense,
                                                          DRM_MAX_LICENSESIZE,
                                                          f_poAppContextInternal->oFFLicense.pLicEval,
                                                          TRUE,
                                                          NULL ) );

    /*
    ** Read license priority before writing back to store
    */
    ChkDR( DRM_WMDRM_LIC_GetAttribute( ( DRM_CONST_STRING * )&dstrUpdatedLicense,
                                        NULL,
                                        DRM_LICENSE_ATTRIB_PRIORITY,
                                        NULL,
                                       &dstrPriority,
                                        0 ) );
    ChkDR( DRMCRT_wcsntol( dstrPriority.pwszString,
                           dstrPriority.cchString,
             ( DRM_LONG * )&dwPriority ) );

    /*
    ** Write the license back to the store if possible
    */
    ChkDR( DRM_LST_UpdateLicense( &f_poAppContextInternal->oLicStoreContextXML,
                                  f_cbLicense,
                                  f_poAppContextInternal->pbDRMLicense,
                                  &( f_poAppContextInternal->oFFLicense.rgkid[f_iChainDepth] ),
                                  &( f_poAppContextInternal->oFFLicense.rglid[f_iChainDepth] ),
                                  dwPriority,
                                  &( f_poAppContextInternal->oFFLicense.rgslotHint[f_iChainDepth] ) ) );

ErrorExit:
    return dr;
}

/*****************************************************************************
** Function: Drm_Reader_Bind
**
** Synopsis: Ask DRM to a complete search for a license that can be used to
**           render the content matching the header set in a call to
**           Drm_Content_SetProperty().
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_rgpdstrRights]        -- An array of strings representing rights that
**                             the caller would like to access the content with.
** [f_cRights]              -- Count of the pointers in the rgpdstrRights array.
** [f_pfnPolicyCallback]    -- A pointer to a callback function that provides
**                             information to the caller about output restrictions
**                             in licenses.
** [f_pv]                   -- A void pointer that will be passed through to the
**                             callback function.
** [f_pcontextDCRY]         -- A context that may be used to decrypt data.
**
** Returns:     DRM_SUCCESS on success.
**              DRM_E_INVALIDARG if and of the arguments are invalid.
**              DRM_E_HEADER_NOT_SET if Drm_Content_SetProperty haven't been
**              called with a content header.
**              DRM_E_LICENSENOTFOUND if a usuable license could not be found.
**              DRM_E_LICENSEEXPIRED if the only available license was expired.
**              DRM_E_CH_BAD_KEY if the checksum over the content key failed
**              DRM_E_LIC_KEY_DECODE_FAILURE if an error occurred during content
**              key decryption.
**              DRM_E_DRMNOTINIT if the DRM_APP_CONTEXT was not initialized.
**              Other return code as defined in drmresults.h.
**              DRM_E_BUFFERTOOSMALL if the buffer passed
**              to Drm_Initialize was too small.  The caller
**              can use Drm_ResizeOpaqueBuffer to increase the
**              size of the buffer in this scenario.
**              The caller should double the size of the buffer
**              for the call to Drm_ResizeOpaqueBuffer because
**              there is no way to know the size of the buffer
**              required in advance.
**
** Notes:       If Drm_Reader_Bind fails with DRM_E_LICENSENOTFOUND the caller
**              should call Drm_LicenseAcq_GenerateChallenge() and attempt to
**              acquire a license if that functionality is available to the device.
**              If success the caller should call Drm_Reader_Commit to get a
**              decryption context.
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Reader_Bind(
    IN                             DRM_APP_CONTEXT      *f_poAppContext,
    __in_ecount( f_cRights ) const DRM_CONST_STRING     *f_rgpdstrRights[],
    IN                             DRM_DWORD             f_cRights,
    IN                             DRMPFNPOLICYCALLBACK  f_pfnPolicyCallback,
    IN const                       DRM_VOID             *f_pv,
    OUT                            DRM_DECRYPT_CONTEXT  *f_pcontextDCRY )
{
    DRM_RESULT dr            = DRM_SUCCESS;
    DRM_RESULT dr2           = DRM_SUCCESS;
    DRM_DWORD  cbLicContext  = 0;
    DRM_BOOL   fBound        = FALSE;
    DRM_BOOL   fESTOpened    = FALSE;
    DRM_BB_CHECKSUM_CONTEXT *pChecksum = NULL;
    DRM_BOOL   fPrioritized  = FALSE;
    DRM_BIND_COMMIT_DATA *pbcd   = NULL;
    DRM_DWORD             iRight = 0;

    DRM_APP_CONTEXT_INTERNAL    *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_STACK_ALLOCATOR_CONTEXT  stack                = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Reader_Bind );
    DRM_PERF_LogMicrosecondCount( eDRM_PERF_LogTag_Enter, eDRM_PERF_LogFunction_Drm_Reader_Bind );

    ChkArg( f_poAppContext != NULL );

    ChkArg(  ( f_rgpdstrRights != NULL && f_cRights  > 0 && f_cRights <= DRM_MAX_ACTIONS )
          || ( f_rgpdstrRights == NULL && f_cRights == 0 ) );

    if ( poAppContextInternal->fBindThenCommit )
    {
        poAppContextInternal->fBindThenCommit = FALSE;
    }

    ChkBOOL( poAppContextInternal->fStoreOpened, DRM_E_DRMNOTINIT );

    for ( cbLicContext = 0; cbLicContext < f_cRights; cbLicContext++ )
    {
        ChkDRMString( f_rgpdstrRights[ cbLicContext ] );
    }

    ChkBOOL(  poAppContextInternal->cbHeaderData != 0
           && poAppContextInternal->eHeaderInContext != DRM_CSP_HEADER_NOT_SET, DRM_E_HEADER_NOT_SET );

    ChkArg( f_pcontextDCRY != NULL );

    poAppContextInternal->dwChainDepth = 0;
    poAppContextInternal->dwSourceid   = 0;

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        poAppContextInternal->dwLicRevInfoVer = 0;
    }

    /*
    ** Zero out the FFLicense structure before performing any initialization
    */
    ZEROMEM( &poAppContextInternal->oFFLicense, SIZEOF( poAppContextInternal->oFFLicense ) );

    /*
    ** v2, v2.4, v4, and v4.1 headers are unicode, # of bytes must be a even number.
    */
    DRMASSERT(   (    poAppContextInternal->eHeaderInContext != DRM_CSP_V2_HEADER
                   && poAppContextInternal->eHeaderInContext != DRM_CSP_V2_4_HEADER
                   && poAppContextInternal->eHeaderInContext != DRM_CSP_V4_HEADER
                   && poAppContextInternal->eHeaderInContext != DRM_CSP_V4_1_HEADER
                   && poAppContextInternal->eHeaderInContext != DRM_CSP_PLAYREADY_OBJ
                   && poAppContextInternal->eHeaderInContext != DRM_CSP_PLAYREADY_OBJ_WITH_KID)
               || !ISODD( poAppContextInternal->cbHeaderData ) );

    /*
    ** There are maximum DRM_LICSTORE_MAX_STORES (3) license stores being used
    ** for search at any time.
    **
    ** Set up the array of license stores to search with the following order:
    ** 1. Nonce store (if the header in context is a KID or V2_4 or V4 or V4_1 or PlayReady object).
    ** 2. Embedded store (if the header in context is a PlayReady object).
    ** 3. XMR store (if the header in context is a V1 or KID or V2_4 or V4 or V4_1 or PlayReady object).
    ** 4. XML store (if the header in context is a V1 or V2 or KID or V2_4 or V4 or V4_1).
    */
    if (   (   poAppContextInternal->eHeaderInContext == DRM_CSP_KID
            || poAppContextInternal->eHeaderInContext == DRM_CSP_V2_4_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_V4_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_V4_1_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ
            || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID )
        && poAppContextInternal->oFFLicense.cLicStores < DRM_LICSTORE_MAX_STORES )
    {
        poAppContextInternal->oFFLicense.rgpLicStores[poAppContextInternal->oFFLicense.cLicStores++] = &poAppContextInternal->oLicStoreContextNST;
    }

    if (   (   poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ
            || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID )
        && poAppContextInternal->pbEmbeddedStore  != NULL
        && poAppContextInternal->cbEmbeddedStore  != 0
        && poAppContextInternal->oFFLicense.cLicStores < DRM_LICSTORE_MAX_STORES
        && DRM_SUCCEEDED( DRM_DST_OpenStore( poAppContextInternal->pOEMContext,
                                             poAppContextInternal->pbEmbeddedStore,
                                             poAppContextInternal->cbEmbeddedStore,
                                             0,
                                             &poAppContextInternal->oDatastoreEST ) ) )
    {
        poAppContextInternal->oFFLicense.rgpLicStores[poAppContextInternal->oFFLicense.cLicStores++] = &poAppContextInternal->oLicStoreContextEmbedded;

        fESTOpened = TRUE;

        ChkDR( DRM_LST_Open( &poAppContextInternal->oLicStoreContextEmbedded,
                             &poAppContextInternal->oDatastoreEST,
                             eDRM_LICENSE_STORE_XMR ) );
    }

    if (   (   poAppContextInternal->eHeaderInContext == DRM_CSP_V1_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_KID
            || poAppContextInternal->eHeaderInContext == DRM_CSP_V2_4_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_V4_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_V4_1_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ
            || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID )
        && poAppContextInternal->oFFLicense.cLicStores < DRM_LICSTORE_MAX_STORES )
    {
        poAppContextInternal->oFFLicense.rgpLicStores[poAppContextInternal->oFFLicense.cLicStores++] = &poAppContextInternal->oLicStoreContextXMR;
    }

    if (   (   poAppContextInternal->eHeaderInContext == DRM_CSP_V1_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_V2_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_KID
            || poAppContextInternal->eHeaderInContext == DRM_CSP_V2_4_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_V4_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_V4_1_HEADER )
        && poAppContextInternal->oFFLicense.cLicStores < DRM_LICSTORE_MAX_STORES )
    {
        poAppContextInternal->oFFLicense.rgpLicStores[poAppContextInternal->oFFLicense.cLicStores++] = &poAppContextInternal->oLicStoreContextXML;

        fPrioritized = TRUE; /* Should do a prioritized search if we have the XML Store */
    }

    /* For V2 headers search the XMR store AFTER XML since it's more likely to use XML */
    if ( poAppContextInternal->eHeaderInContext == DRM_CSP_V2_HEADER
      && poAppContextInternal->oFFLicense.cLicStores < DRM_LICSTORE_MAX_STORES )
    {
        poAppContextInternal->oFFLicense.rgpLicStores[poAppContextInternal->oFFLicense.cLicStores++] = &poAppContextInternal->oLicStoreContextXMR;
    }

    ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

    /*
    ** Set up Checksum object from current header
    */
    poAppContextInternal->oFFLicense.pV1Header = NULL;

    pChecksum = &poAppContextInternal->oBindInfo[0].m_oChecksum;
    pChecksum->m_fIgnoreChecksum = TRUE;
    pChecksum->m_pbV1KID         = NULL;
    pChecksum->m_cbChecksum      = 0;

    if (  poAppContextInternal->eHeaderInContext == DRM_CSP_V2_HEADER
       || poAppContextInternal->eHeaderInContext == DRM_CSP_V2_4_HEADER
       || poAppContextInternal->eHeaderInContext == DRM_CSP_V4_HEADER
       || poAppContextInternal->eHeaderInContext == DRM_CSP_V4_1_HEADER
       || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ )
    {
        DRM_CONST_STRING dstrCheckSum = EMPTY_DRM_STRING;

        /*
        ** Extract the checksum from the header.
        ** It's optional, so if it doesn't exist simply ignore it.
        ** (pChecksum->m_fIgnoreChecksum is already set to TRUE.)
        */
        if( DRM_SUCCEEDED( DRM_HDR_GetAttribute( &poAppContextInternal->oLicEvalContext.dstrContentHeader,
                                                  NULL,
                                                  DRM_HEADER_ATTRIB_CHECKSUM,
                                                 &dstrCheckSum,
                                                  0 ) ) )
        {

            /*
            ** If the following base64 decode fails because the buffer isn't big enough then it
            ** isn't a valid checksum anyway.
            */
            pChecksum->m_cbChecksum = DRM_MAX_CHECKSUM;
            pChecksum->m_fIgnoreChecksum = FALSE;

            ZEROMEM( pChecksum->m_rgbChecksumData, DRM_MAX_CHECKSUM );

            ChkBOOL( DRM_SUCCEEDED( DRM_B64_DecodeW( &dstrCheckSum,
                                                     &pChecksum->m_cbChecksum,
                                                      pChecksum->m_rgbChecksumData,
                                                      0 ) ), DRM_E_CH_INVALID_CHECKSUM );

            ChkBOOL( pChecksum->m_cbChecksum == CHECKSUM_LENGTH      /* Legacy checksum algorithm */
                  || pChecksum->m_cbChecksum == DRM_AES_CK_CHECKSUM_LENGTH    /* New PlayReady algorithm */
                  || pChecksum->m_cbChecksum == SIZEOF( DRM_KID ),   /* New PlayReady algorithm */
                     DRM_E_CH_INVALID_CHECKSUM );

            MEMCPY( pChecksum->m_KID.rgb, poAppContextInternal->KID.rgb, SIZEOF( DRM_KID ) );
        }
    }
    else if ( poAppContextInternal->eHeaderInContext == DRM_CSP_V1_HEADER )
    {
        poAppContextInternal->oFFLicense.pV1Header = ( DRM_V1Header * )poAppContextInternal->pbDRMHeaderData;

        ChkBOOL( poAppContextInternal->oFFLicense.pV1Header->cbSecretData <= DRM_MAX_CHECKSUM,
                 DRM_E_BUFFERTOOSMALL );

        MEMCPY( pChecksum->m_rgbChecksumData,
                poAppContextInternal->oFFLicense.pV1Header->pbSecretData,
                poAppContextInternal->oFFLicense.pV1Header->cbSecretData );
        pChecksum->m_cbChecksum = poAppContextInternal->oFFLicense.pV1Header->cbSecretData;
        ChkBOOL( poAppContextInternal->oFFLicense.pV1Header->cbKeyID == LEGACY_KID_B64ENC_LEN, DRM_E_CH_INVALID_CHECKSUM );
        pChecksum->m_pbV1KID = poAppContextInternal->oFFLicense.pV1Header->pbKeyID;
        pChecksum->m_fIgnoreChecksum = FALSE;
    }

    /*
    ** Fill the FindFirstLicense structure.
    */

    poAppContextInternal->oFFLicense.pLicEval         = &poAppContextInternal->oLicEvalContext;
    poAppContextInternal->oFFLicense.pstack           = &stack;
    poAppContextInternal->oFFLicense.pLicStoreEnum    =  poAppContextInternal->oLicEnumMultiple;
    poAppContextInternal->oFFLicense.pbBuffer         =  poAppContextInternal->pbOpaqueBuffer
                                                         + __CB_DECL( poAppContextInternal->cbTotalHeaderData );
    poAppContextInternal->oFFLicense.cbBuffer         =  poAppContextInternal->cbOpaqueBuffer
                                                         - poAppContextInternal->cbTotalHeaderData;
    poAppContextInternal->oFFLicense.pBindingInfo     =  poAppContextInternal->oBindInfo;
    poAppContextInternal->oFFLicense.pSecStoreLicense =  poAppContextInternal->rgcontextSST;
    poAppContextInternal->oFFLicense.dwPolicyFlags    =  0;

    /*
    ** When just testing to see if we could bind if we wanted to, we dont't need to report actions.
    ** (This is set by the IsAllowed function).
    */
    if ( poAppContextInternal->fCanBind )
    {
        poAppContextInternal->oFFLicense.fCanBind = TRUE;
    }

    ChkDR( DRM_UTL_EnsureDataAlignment(  poAppContextInternal->oFFLicense.pbBuffer,
                                         poAppContextInternal->oFFLicense.cbBuffer,
                                        &poAppContextInternal->oFFLicense.pbBuffer,
                                        &poAppContextInternal->oFFLicense.cbBuffer,
                                         SIZEOF( DRM_DWORD_PTR ),
                                         NULL ) );

    /*
    ** Initialize the stack
    */
    stack.nStackTop = 0;

    /*
    ** Note: The byte count is computed this way to avoid 16-bit issues.
    */
    stack.cbStack = poAppContextInternal->cbOpaqueBuffer
                    - __CB_DECL( poAppContextInternal->cbTotalHeaderData ) * CB_NATIVE_BYTE;

    stack.pbStack = poAppContextInternal->pbOpaqueBuffer
                    + __CB_DECL( poAppContextInternal->cbTotalHeaderData );

    /*
    ** Query for licenses based on KID
    ** Note: poAppContextInternal->KID should be set up at this point.
    */
    ChkDR( DRM_LST_InitEnumMultiple(  poAppContextInternal->oFFLicense.rgpLicStores,
                                      poAppContextInternal->oFFLicense.cLicStores,
                                     &poAppContextInternal->KID,
                                      fPrioritized,
                                     &poAppContextInternal->oLicEnumMultiple[ 0 ] ) );
    while ( !fBound )
    {
        dr = DRM_LST_EnumNextMultiple( &poAppContextInternal->oLicEnumMultiple[ 0 ],
                                       &poAppContextInternal->oFFLicense.rgkid[ 0 ],
                                       &poAppContextInternal->oFFLicense.rglid[ 0 ],
                                       &poAppContextInternal->oFFLicense.rgpLicStoreCurrent[ 0 ],
                                       &( poAppContextInternal->oFFLicense.rgslotHint[ 0 ] ),
                                        NULL );

        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_E_LICENSENOTFOUND;

            break;
        }

        ChkDR( dr );

        /*
        ** XML license search (HDS only)
        */
        if ( poAppContextInternal->oFFLicense.rgpLicStoreCurrent[ 0 ] == &poAppContextInternal->oLicStoreContextXML )
        {
            poAppContextInternal->oFFLicense.pBindingInfo[0].m_fXMRLicense = FALSE;
            poAppContextInternal->oFFLicense.pLicenseXMR =  NULL;
            poAppContextInternal->oFFLicense.pLicEval->fLicenseFromHDS = TRUE;
        }
        /*
        ** XMR license search (HDS or embedded store)
        */
        else
        {
            poAppContextInternal->oFFLicense.pBindingInfo[0].m_fXMRLicense = TRUE;
            poAppContextInternal->oFFLicense.pLicenseXMR = poAppContextInternal->licenseXMR;
            poAppContextInternal->oFFLicense.pLicEval->fLicenseFromHDS =
                ( poAppContextInternal->oFFLicense.rgpLicStoreCurrent[ 0 ] == &poAppContextInternal->oLicStoreContextXMR );
        }

        if (  DRM_CLK_IsSecureClockSupported()
           && ( poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags & DRM_FEATURE_SECURE_CLOCK )
           && !poAppContextInternal->fClockSet )
        {
           poAppContextInternal->oLicEvalContext.fIgnoreTimeBoundLicense = TRUE;
        }

        if (  ( poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags & DRM_FEATURE_SECURE_CLOCK ) == 0
           && ( poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags & DRM_FEATURE_ANTI_ROLLBACK_CLOCK ) == 0 )
        {
           poAppContextInternal->oLicEvalContext.fIgnoreTimeBoundLicense = TRUE;
        }

        /*
        ** Make Sure we are evaluating a new chain.
        */
        poAppContextInternal->oFFLicense.dwChainDepth = 0;

        dr = DRM_LIC_CompleteLicenseChain( poAppContextInternal->pOEMContext, &poAppContextInternal->oFFLicense,
                                            f_rgpdstrRights,
                                            f_cRights,
                                           &poAppContextInternal->dwChainDepth,
                                           &poAppContextInternal->oDatastoreHDS,
                                            f_pfnPolicyCallback,
                                            f_pv );

        if( DRM_REVOCATION_IsRevocationSupported() )
        {
            /*
            ** If the call failed due to revocation, we should make sure the
            ** revocation version cache in the DRM_APP_CONTEXT is up to date.
            ** If there have been any changes we should try again.
            */
            if (  dr == DRM_E_RIV_TOO_SMALL
               || dr == DRM_E_LICEVAL_REQUIRED_REVOCATION_LIST_NOT_AVAILABLE )
            {
                DRM_BOOL revocationUpdated = FALSE;

                DRM_RVK_UpdateRevocationVersionsCache( &poAppContextInternal->oRevContext,
                                                       &poAppContextInternal->oBlackBoxContext.CryptoContext,
                                                       &poAppContextInternal->idCRLsCurrent,
                                                        poAppContextInternal->pbRevocationBuffer,
                                                        poAppContextInternal->cbRevocationBuffer,
                                                       &revocationUpdated );

                if ( revocationUpdated )
                {
                    /*
                    ** This can occur if the HDS was updated by another app
                    ** context, especially in ND scenarios.
                    */
                    MEMCPY( &poAppContextInternal->oLicEvalContext.idCRLsCurrent,
                            &poAppContextInternal->idCRLsCurrent,
                            SIZEOF( DRM_REVLIST_ID_VALUES ) );

                    dr = DRM_LIC_CompleteLicenseChain( poAppContextInternal->pOEMContext, &poAppContextInternal->oFFLicense,
                                                        f_rgpdstrRights,
                                                        f_cRights,
                                                       &poAppContextInternal->dwChainDepth,
                                                       &poAppContextInternal->oDatastoreHDS,
                                                        f_pfnPolicyCallback,
                                                        f_pv );
                }
            }
        }

        /*
        ** If we are only checking whether we can bind, no need to report actions
        */
        if ( poAppContextInternal->oFFLicense.fCanBind )
        {
            /*
            ** If bind succeeds, exit the function, otherwise try the next license
            ** if exists.
            */
            if ( DRM_SUCCEEDED( dr ) )
            {
                goto ErrorExit;
            }
            else
            {
                dr2 = dr;
                continue;
            }
        }

        if ( DRM_SUCCEEDED( dr ) )
        {
            /*
            ** update the dwSourceid and dwLicRevInfoVer in DRM_APP_CONTEXT_INTERNAL from
            ** DRM_FFLICENSE.
            */
            poAppContextInternal->dwSourceid      = poAppContextInternal->oFFLicense.dwSourceid;

            if( DRM_REVOCATION_IsRevocationSupported() )
            {
                poAppContextInternal->dwLicRevInfoVer = poAppContextInternal->oFFLicense.dwLicRevInfoVer;
            }

            /*
            ** A license was found.  The license chain has a depth of dwChainDepth.
            ** Send all the binding info to the blackbox.
            */
            dr = DRM_APPP_CipherKeySetup( poAppContextInternal->oFFLicense.pLicenseXMR,
                                          poAppContextInternal->oFFLicense.pBindingInfo,
                                          poAppContextInternal->dwChainDepth,
                                          ( DRM_CIPHER_CONTEXT * )f_pcontextDCRY,
                                          &poAppContextInternal->oBlackBoxContext );

            if ( DRM_SUCCEEDED( dr ) )
            {
                dr = DRM_LIC_ReportActions( &poAppContextInternal->oFFLicense,
                                             f_rgpdstrRights,
                                             f_cRights,
                                             poAppContextInternal->dwChainDepth,
                                            &poAppContextInternal->oDatastoreHDS,
                                             f_pfnPolicyCallback,
                                             f_pv );

                if ( DRM_SUCCEEDED( dr ) )
                {
                    if( DRM_SYMOPT_IsSymOptSupported() )
                    {
                        /*
                        ** If XML license is not fully optimized, do so now (CK sig has been verified by DRM_BBX_CipherKeySetup.
                        ** Will need to re-write license to store.
                        ** Do not fail Drm_Reader_Bind on symm opt failure
                        ** The topmost license is still loaded into the AppContext buffer
                        */
                        DRM_DWORD iChainDepth2   = poAppContextInternal->dwChainDepth;
                        DRM_DWORD cbLicense      = poAppContextInternal->oFFLicense.pLicEval->dstrContentLicense.cchString;
                        DRM_BOOL  fLicenseLoaded = TRUE;

                        do
                        {
                            iChainDepth2--;

                            if ( !poAppContextInternal->oBindInfo[iChainDepth2].m_fXMRLicense
                                && !poAppContextInternal->oBindInfo[iChainDepth2].m_BindingInfo.m_XML.m_fCKSigVerified )
                            {
                                if ( !fLicenseLoaded )
                                {
                                    cbLicense = DRM_MAX_LICENSESIZE;
                                    fLicenseLoaded = DRM_SUCCEEDED( DRM_LST_GetLicense( &poAppContextInternal->oLicStoreContextXML,
                                                                                        &poAppContextInternal->oFFLicense.rgkid[iChainDepth2],
                                                                                        &poAppContextInternal->oFFLicense.rglid[iChainDepth2],
                                                                                        &poAppContextInternal->oFFLicense.rgslotHint[iChainDepth2],
                                                                                            poAppContextInternal->pbDRMLicense,
                                                                                        &cbLicense ) );
                                }

                                if ( fLicenseLoaded )
                                {
                                    ( DRM_VOID )_UpdateSymmOptsForLicense( iChainDepth2,
                                                                            poAppContextInternal,
                                                                            cbLicense );
                                }
                            }
                            fLicenseLoaded = FALSE; /* Additional licenses in the chain must be re-loaded into the buffer */
                        } while ( iChainDepth2 > 0 );
                    }

                    fBound = TRUE;
                }
            }

            if ( DRM_FAILED( dr ) )
            {
                /* Don't overwite any cached error code. */
                if ( DRM_SUCCEEDED( dr2 ) )
                {
                    dr2 = dr;
                }
            }
        }
        else
        {
            if ( poAppContextInternal->oLicEvalContext.lReasonForFail == LR_LICENSE_CLOCK_NOT_SET )
            {
                dr2 = DRM_E_CLK_NOT_SET;
            }
            else if( dr == DRM_E_UPLINKLICENSENOTFOUND )  /* DRM_E_UPLINKLICENSENOTFOUND supercedes DRM_E_LICENSEEXPIRED or DRM_E_RIGHTSNOTAVAILABLE. */
            {
                if( DRM_SUCCEEDED( dr2 ) )  /* Don't overwite any cached error code. */
                {
                    dr2 = DRM_E_UPLINKLICENSENOTFOUND;
                }
            }
            else if ( poAppContextInternal->oLicEvalContext.lReasonForFail == LR_LICENSE_EXPIRED )
            {
                dr2 = DRM_E_LICENSEEXPIRED;
            }
            else if ( poAppContextInternal->oLicEvalContext.lReasonForFail == LR_LICENSE_NOTENABLED )
            {
                dr2 = DRM_E_RIGHTSNOTAVAILABLE;
            }
            else if ( poAppContextInternal->oLicEvalContext.lReasonForFail == LR_LICENSE_EXTENSIBLE_RIGHT_NOT_FOUND )
            {
                dr2 = DRM_E_RIGHTSNOTAVAILABLE;
            }
            else if ( DRM_SNC_IsLicenseSyncSupported()
                   && ( dr == DRM_E_LICENSENOTFOUND
                     || dr == DRM_E_RIGHTSNOTAVAILABLE )
                   && DRM_SUCCEEDED( dr2 ) ) /* Don't overwite any cached error code. */
            {
                /*
                ** Generic case where we just couldn't find a license.
                ** check the sync store to see if we ever had a license.
                */
                DRM_BOOL   fExpired = TRUE;
                DRM_DWORD  i        = 0;
                DRM_RESULT drOrig   = dr;

                for ( i = 0; i < poAppContextInternal->dwChainDepth; i++ )
                {
                    ChkDR( DRM_SNC_DidLicenseExpire( &poAppContextInternal->contextSync,
                                                      NULL,
                                                     &poAppContextInternal->oDatastoreHDS,
                                                     &poAppContextInternal->oFFLicense.rgkid[ i ],
                                                     &fExpired ) );

                    if ( fExpired )
                    {
                        dr2 = DRM_E_LICENSEEXPIRED;
                        break;
                    }
                    else if ( drOrig == DRM_E_RIGHTSNOTAVAILABLE )
                    {
                        dr2 = DRM_E_RIGHTSNOTAVAILABLE;
                    }
                }
            }
            else if ( DRM_SUCCEEDED( dr2 ) ) /* Don't overwite any cached error code. */
            {
                dr2 = dr;
            }
        }
    }

    if( DRM_METERING_IsMeteringSupported() )
    {
        /*
        ** Cache the actions/rights to pass to the Commit call.
        */
        pbcd = ( DRM_BIND_COMMIT_DATA * ) poAppContextInternal->pbDRMLicense;

        pbcd->cActions = f_cRights;

        for ( iRight = 0; iRight < f_cRights; iRight++ )
        {
            pbcd->rgpdstrActions[ iRight ] = f_rgpdstrRights[ iRight ];
        }
    }

    poAppContextInternal->fBindThenCommit = TRUE;

    if ( dr == DRM_E_LICENSENOTFOUND )
    {
        if ( DRM_FAILED( dr2 ) )
        {
            ChkDR( dr2 );
        }
        else if( DRM_SNC_IsLicenseSyncSupported() )
        {
            /*
            ** Generic case where we just couldn't find a license.
            ** check the sync store to see if we ever had a license.
            */
            DRM_BOOL fExpired = TRUE;

            ChkDR( DRM_SNC_DidLicenseExpire( &poAppContextInternal->contextSync,
                                              NULL,
                                             &poAppContextInternal->oDatastoreHDS,
                                             &poAppContextInternal->KID,
                                             &fExpired ) );

            ChkBOOL( !fExpired, DRM_E_LICENSEEXPIRED );

        }

        ChkDR( DRM_E_LICENSENOTFOUND );
    }

    if( DRM_METERING_IsMeteringSupported() )
    {
        /* Cache the actions/rights to pass to the Commit call. */

        pbcd = ( DRM_BIND_COMMIT_DATA * )poAppContextInternal->pbDRMLicense;

        pbcd->cActions = f_cRights;

        for ( iRight = 0; iRight < f_cRights; iRight++ )
        {
            pbcd->rgpdstrActions[ iRight ] = f_rgpdstrRights[ iRight ];
        }
    }

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        DRM_DWORD        iChainDepth       = 0;
        DRM_CONST_STRING dstrContentHeader = EMPTY_DRM_STRING;

        /* Update the content headers in the sync store for the licenses we just bound to. */
        DSTR_FROM_PB( &dstrContentHeader,
                       poAppContextInternal->pwszHeaderData,
                       poAppContextInternal->cbHeaderData );

        iChainDepth = poAppContextInternal->dwChainDepth;

        while ( iChainDepth > 0 )
        {
            DRM_BOOL dr1 = DRM_SUCCESS;
            iChainDepth--;

            if ( ! XMR_IS_REMOVAL_DATE_VALID( &poAppContextInternal->licenseXMR[ iChainDepth ] ) )
            {
                /* Ignore errors in free builds as they're not critical, but ASSERT in chk builds. */
                dr1 = DRM_SNC_UpdateKID( &poAppContextInternal->contextSync,
                                         NULL,
                                         &poAppContextInternal->oFFLicense.rgkid[ iChainDepth ],
                                         &dstrContentHeader,
                                         DRM_SYNC_UPDATE_AGGREGATE );
                DRMASSERT( DRM_SUCCEEDED( dr1 ) );
            }
        }
    }

ErrorExit:

    DRM_XMLHASH_Stop();

    if ( poAppContextInternal != NULL )
    {
        if ( poAppContextInternal->oLicStoreContextEmbedded.fInited )
        {
            DRM_LST_Close( &poAppContextInternal->oLicStoreContextEmbedded );
        }
        if ( fESTOpened )
        {
            DRM_DST_CloseStore( &poAppContextInternal->oDatastoreEST );
        }
    }

    if ( DRM_FAILED( dr ) )
    {
        if ( dr2 == DRM_E_CLK_NOT_SET )
        {
            if ( poAppContextInternal->oBlackBoxContext.cachedCertValues.dwFeatureFlags & DRM_FEATURE_SECURE_CLOCK )
            {
                dr = DRM_E_CLK_NOT_SET;
            }
            else
            {
                dr = DRM_E_NO_CLK_SUPPORTED;
            }
        }
        else if ( DRM_FAILED( dr2 ) &&
                  dr == DRM_E_LICENSENOTFOUND )
        {
            dr = dr2;
        }
    }

    _FreeDrmAppInternalContexts( poAppContextInternal );

    ChkECC( ECC_Drm_Reader_Bind, dr );

    DRM_PERF_LogMicrosecondCount( eDRM_PERF_LogTag_Exit, eDRM_PERF_LogFunction_Drm_Reader_Bind );
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    Drm_License_GetProperty
**
** Synopsis:    Gets properties for bound licenses.  Must be called after
**              a successful bind via Drm_Reader_Bind() and prior to
**              Drm_Reader_Commit.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_eProperty]            -- The property to get. Must be a value from the
**                             DRM_LICENSE_GET_PROPERTY enum.
** [f_pbExtraData]          -- Pointer to property-specific input/output data.
**                             See below.
** [f_pcbExtraData]         -- Pointer to the size in bytes of the data buffer
**                             f_pbExtraData points to.
** [f_pdwOutputData]        -- Pointer to property-specific output data. See below.
**
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          DRM_E_BUFFERTOOSMALL if the f_pbExtraData input/output
**                          buffer is too small.
**                          DRM_E_LICENSENOTBOUND if license hasn't been bound
**                          by calling Drm_Reader_Bind() successfully.
**                          Any return code as defined in drmrestults.h.
**
** Notes:
**
** The following are the valid properties, and the respective property-specific data:
** Property                    f_pbExtraData                  f_pdwOutputData
** --------------------------  ----------------------------   ------------------------------
** DRM_LGP_MIN_APP_SEC         unused, must be NULL           The minimum app security level.
** DRM_LGP_SOURCE_ID           unused, must be NULL           The Source ID.
** DRM_LGP_REVINFO_VERSION     unused, must be NULL           The RevInfo version.
** DRM_LGP_MID                 DRM_LGP_MID_EXTRA_DATA         The number of MIDs set in the
**                             OUT: one or two MIDs           ExtraData struct.
** DRM_LGP_HAS_INCLUSION_GUID  DRM_LGP_HAS_INCLUSION_GUID_EXTRA_DATA    Bool: 0 if the GUID
**                             IN : the GUID to query         was not found, non-zero if it was.
** DRM_LGP_BLOCK_NDT           unused, must be NULL           Bool: non-zero if the license should be blocked for NDT
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_License_GetProperty(
    IN                                          DRM_APP_CONTEXT           *f_poAppContext,
    IN                                          DRM_LICENSE_GET_PROPERTY   f_eProperty,
    __inout_bcount_opt( *f_pcbExtraData ) const DRM_BYTE                  *f_pbExtraData,
    IN                                    const DRM_DWORD                 *f_pcbExtraData,
    OUT                                         DRM_DWORD                 *f_pdwOutputData )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRM_APP,PERF_FUNC_Drm_License_GetProperty);

    ChkArg( f_poAppContext != NULL  );
    ChkArg(  f_eProperty == DRM_LGP_MIN_APP_SEC
          || f_eProperty == DRM_LGP_SOURCE_ID
          || f_eProperty == DRM_LGP_REVINFO_VERSION
          || f_eProperty == DRM_LGP_MID
          || f_eProperty == DRM_LGP_HAS_INCLUSION_GUID
          || f_eProperty == DRM_LGP_BLOCK_NDT );

    if ( !poAppContextInternal->fBindThenCommit )
    {
        ChkDR( DRM_E_LICENSENOTBOUND );
    }

    if ( f_eProperty == DRM_LGP_MIN_APP_SEC )
    {
        ChkBOOL( DRM_WMDRMNET_IsWmdrmnetSupported(), DRM_E_NOTIMPL );
        ChkArg( f_pdwOutputData != NULL );
        *f_pdwOutputData = poAppContextInternal->oFFLicense.dwMinimumRequiredAppSec;
    }
    else if ( f_eProperty == DRM_LGP_SOURCE_ID )
    {
        ChkArg( f_pdwOutputData != NULL );

        *f_pdwOutputData = poAppContextInternal->dwSourceid;
    }
    else if ( f_eProperty == DRM_LGP_REVINFO_VERSION )
    {
        ChkBOOL( DRM_REVOCATION_IsRevocationSupported(), DRM_E_NOTIMPL );
        ChkArg( f_pdwOutputData != NULL );
        *f_pdwOutputData = poAppContextInternal->dwLicRevInfoVer;
    }
    else if ( f_eProperty == DRM_LGP_MID )
    {
        if( DRM_METERING_IsMeteringSupported() )
        {
            DRM_DWORD iLicense = 0;
            DRM_DWORD cMIDsFound = 0;
            DRM_LGP_MID_EXTRA_DATA *pMidExtraData = ( DRM_LGP_MID_EXTRA_DATA * )f_pbExtraData;

            ChkArg(  f_pdwOutputData != NULL
                  && f_pbExtraData != NULL
                  && f_pcbExtraData != NULL
                  && *f_pcbExtraData == SIZEOF(DRM_LGP_MID_EXTRA_DATA ) );

            for ( iLicense = 0; iLicense < poAppContextInternal->dwChainDepth; iLicense++ )
            {
                if ( poAppContextInternal->oFFLicense.rgfHasMetering[ iLicense ] )
                {
                    DRM_DWORD cchMID = CCH_BASE64_EQUIV( SIZEOF( DRM_MID ) );

                    ChkBOOL( *f_pcbExtraData >= CCH_BASE64_EQUIV( SIZEOF( DRM_MID ) ) * ( cMIDsFound + 1 ), DRM_E_INVALIDARG );

                    /* Note there will always be room in the input array for all the MIDs in the license chain. */
                    ChkDR( DRM_B64_EncodeA(  ( DRM_BYTE * )&poAppContextInternal->oFFLicense.rgmid[ iLicense ],
                                             SIZEOF( DRM_MID ),
                                            &pMidExtraData->szMID[ cMIDsFound ][ 0 ],
                                            &cchMID,
                                             0 ) );

                    cMIDsFound++;
                }
            }

            *f_pdwOutputData = cMIDsFound;
        }
        else
        {
            ChkDR( DRM_E_NOTIMPL );
        }
    }
    else if ( f_eProperty == DRM_LGP_HAS_INCLUSION_GUID )
    {
        DRM_DWORD iInclGuid = 0;
        DRM_LGP_HAS_INCLUSION_GUID_EXTRA_DATA *pInclGuidExtraData = ( DRM_LGP_HAS_INCLUSION_GUID_EXTRA_DATA * )f_pbExtraData;

        ChkArg(  f_pdwOutputData != NULL
              && f_pbExtraData != NULL
              && f_pcbExtraData != NULL
              && *f_pcbExtraData == SIZEOF( DRM_LGP_HAS_INCLUSION_GUID_EXTRA_DATA ) );

        *f_pdwOutputData = FALSE;

        for ( iInclGuid = 0; iInclGuid < DRM_MAX_INCLUSION_GUIDS; iInclGuid++ )
        {
            DRM_DWORD iDepth = 0;
            DRM_BOOL fIsInclGuidValid = TRUE;

            DRMASSERT( poAppContextInternal->dwChainDepth <= DRM_MAX_LICENSE_CHAIN_DEPTH );

            for ( iDepth = 0; iDepth < poAppContextInternal->dwChainDepth; iDepth++ )
            {
                /*
                ** For chained licenses, the inclusion GUID should be checked at all levels
                ** and used if and only if it appears at all levels.
                */
                if ( !poAppContextInternal->oFFLicense.inclusionListCallback.rgfInclusionListValid[ iInclGuid ][ iDepth ] )
                {
                    fIsInclGuidValid = FALSE;

                    break;
                }
            }

            if ( fIsInclGuidValid )
            {
                if ( MEMCMP( &poAppContextInternal->oFFLicense.inclusionListCallback.rgInclusionList[ iInclGuid ],
                             &pInclGuidExtraData->guid,
                              SIZEOF( DRM_GUID ) ) == 0 )
                {
                    *f_pdwOutputData = TRUE;
                    break;
                }
            }
        }
    }
    else if ( f_eProperty == DRM_LGP_BLOCK_NDT )
    {
        ChkArg( f_pdwOutputData != NULL );
        *f_pdwOutputData = poAppContextInternal->oFFLicense.fBlockNDT;
    }

ErrorExit:

    ChkECC( ECC_Drm_License_GetProperty, dr );
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    Drm_Reader_Commit
**
** Synopsis:    Commits all metering and secure store transactions to
**              the store file.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_pfnPolicyCallback]    -- Pointer to a callback function that provides
**                             information to the caller about output
**                             restrictions in licenses.
** [f_pvCallbackData]       -- A void pointer that will be passed through
**                             to the callback function.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguement is invalid.
**                          Any other return code as defined in drmresults.h.
**
** Notes:       If this function fails the app ( which may already be decrypting
**              and playing back content ) should stop all content rendering.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Reader_Commit(
    IN       DRM_APP_CONTEXT      *f_poAppContext,
    IN       DRMPFNPOLICYCALLBACK  f_pfnPolicyCallback,
    IN const DRM_VOID             *f_pvCallbackData )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_BIND_COMMIT_DATA     *pbcd                 = NULL;
    DRM_CONST_STRING          dstrContentHeader    = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Reader_Commit );
    DRM_PERF_LogMicrosecondCount( eDRM_PERF_LogTag_Enter, eDRM_PERF_LogFunction_Drm_Reader_Commit );

    ChkArg( f_poAppContext != NULL );
    ChkArg( poAppContextInternal->fBindThenCommit );

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        DSTR_FROM_PB( &dstrContentHeader,
                       poAppContextInternal->pwszHeaderData,
                       poAppContextInternal->cbHeaderData );
    }

    if( DRM_METERING_IsMeteringSupported() )
    {
        pbcd = ( DRM_BIND_COMMIT_DATA * )poAppContextInternal->pbDRMLicense;
    }

    if ( !poAppContextInternal->fStoreOpened )
    {
        ChkDR( DRM_E_DRMNOTINIT );
    }

    if ( poAppContextInternal->dwChainDepth == 0 )
    {
        ChkDR( DRM_E_LICENSENOTBOUND );
    }

    for ( ; poAppContextInternal->dwChainDepth > 0; )
    {
        /* Shift from cardinal to ordinal. */

        poAppContextInternal->dwChainDepth--;

        ChkDR( DRM_SST_CloseKey( &poAppContextInternal->rgcontextSST[ poAppContextInternal->dwChainDepth ],
                                 &poAppContextInternal->oDatastoreHDS,
                                  f_pfnPolicyCallback,
                                  f_pvCallbackData ) );

        /* If DRM_SST_CloseKey didn't update any state data for the license it returns
        ** DRM_S_FALSE, and we shouldn't have to clear the state data in the sync store
        ** since it wouldn't have been affected.
        */
        if ( DRM_SNC_IsLicenseSyncSupported() && dr == DRM_SUCCESS )
        {
            /* Ignore errors in free builds as they're not critical, but ASSERT in chk builds. */
            dr = DRM_SNC_UpdateKID( &poAppContextInternal->contextSync,
                                    NULL,
                                   &poAppContextInternal->oFFLicense.rgkid[ poAppContextInternal->dwChainDepth ],
                                   &dstrContentHeader,
                                    DRM_SYNC_UPDATE_FORCE_SYNC );
            DRMASSERT( DRM_SUCCEEDED( dr ) );
            dr = DRM_SUCCESS;
        }
        else if ( dr == DRM_S_FALSE )
        {
            /* Clear the result code if DRM_SST_CloseKey returned DRM_S_FALSE, the caller
            ** to Commit shouldn't need to know.
            */
            ChkDR( DRM_SUCCESS );
        }

        if( DRM_METERING_IsMeteringSupported() )
        {
            if ( poAppContextInternal->oFFLicense.rgfHasMetering[ poAppContextInternal->dwChainDepth ] )
            {
                DRM_CONST_STRING dstrLAINFO = EMPTY_DRM_STRING;
                DRM_CONST_STRING dstrHeader = EMPTY_DRM_STRING;

                ChkDR( DRM_METERING_UpdateData( f_poAppContext,
                                                pbcd->rgpdstrActions,
                                                pbcd->cActions ) );

                if ( poAppContextInternal->eHeaderInContext != DRM_CSP_KID )
                {
                    /*
                    ** The license is to be metered, we need to add an entry in our meter cert store.
                    ** We also need to try to get the LAINFO from the content header.
                    */
                    DSTR_FROM_PB( &dstrHeader,
                                   poAppContextInternal->pwszHeaderData,
                                   poAppContextInternal->cbHeaderData );

                    if ( DRM_SUCCEEDED( DRM_HDR_GetAttribute( &dstrHeader,
                                                               NULL,
                                                               DRM_HEADER_ATTRIB_LICENSE_ACQ_V4_URL,
                                                              &dstrLAINFO,
                                                               0 ) ) )
                    {
                        DRM_RESULT dr1 = DRM_SUCCESS;

                        /* Update the meter cert store with the MID and LAINFO. */
                        dr1 = DRM_MCS_UpdateMeterCert( &poAppContextInternal->oMeterCertContext,
                                                                             &dstrLAINFO,
                                                                              FALSE,
                                                                              NULL,
                                                                              0,
                                                                             &poAppContextInternal->oFFLicense.rgmid[ poAppContextInternal->dwChainDepth ] );
                        DRMASSERT( DRM_SUCCEEDED(dr1));

                    }
                    else if ( DRM_SUCCEEDED( DRM_HDR_GetAttribute( &dstrHeader,
                                                                    NULL,
                                                                    DRM_HEADER_ATTRIB_LAINFO,
                                                                   &dstrLAINFO,
                                                                    0 ) ) )
                    {
                        DRM_RESULT dr1 = DRM_SUCCESS;

                        /* Update the meter cert store with the MID and LAINFO */
                        dr1 = DRM_MCS_UpdateMeterCert( &poAppContextInternal->oMeterCertContext,
                                                                             &dstrLAINFO,
                                                                              TRUE,
                                                                              NULL,
                                                                              0,
                                                                             &poAppContextInternal->oFFLicense.rgmid[ poAppContextInternal->dwChainDepth ] );
                        DRMASSERT( DRM_SUCCEEDED(dr1));
                    }
                }
            }
        }
    }

ErrorExit:

    if ( poAppContextInternal != NULL )
    {
        poAppContextInternal->fBindThenCommit = FALSE;
        poAppContextInternal->dwChainDepth    = 0;
    }

    ChkECC( ECC_Drm_Reader_Commit, dr );

    DRM_PERF_LogMicrosecondCount( eDRM_PERF_LogTag_Exit, eDRM_PERF_LogFunction_Drm_Reader_Commit );
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
** Function:    Drm_Reader_InitDecrypt
**
** Synopsis:    Initializes state for phased decryption.
**
** Arguments:
**
** [f_pDecryptContext]      -- Pointer to an initialized DRM_DECRYPT_CONTEXT
**                             structure that contains the state data of the current
**                             DRM session.
** [f_pbLast15]             -- Pointer to the last 15 bytes of the payload. If the
**                             payload is less than 15 bytes, it is a pointer to the
**                             beginning of the payload.
** [f_cbData]               -- Byte count of the payload.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguement is invalid.
**                          Any other return code as defined in drmresults.h.
**                          DRM_E_UNSUPPORTEDALGORITHM if non WMDRM cipher
**
** Note:                    For AES packaged content this call is not required.
**                          For Cocktail packaged content, the caller will need to
**                          set a pointer to the last 15 bytes of the payload to be
**                          decrypted.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Reader_InitDecrypt(
    __inout_ecount( 1 )                                                DRM_DECRYPT_CONTEXT *f_pDecryptContext,
    __in_bcount_opt( min(DRM_CPHR_CB_FOR_INITDECRYPT,f_cbData) ) const DRM_BYTE            *f_pbLast15,
    __in                                                               DRM_DWORD            f_cbData )
{
    DRM_RESULT          dr             = DRM_SUCCESS;
    DRM_CIPHER_CONTEXT *pCipherContext = ( DRM_CIPHER_CONTEXT * )f_pDecryptContext;

    DRM_PERF_LogMicrosecondCount( eDRM_PERF_LogTag_Enter, eDRM_PERF_LogFunction_Drm_Reader_InitDecrypt );
    
    ChkArg( pCipherContext != NULL );

    /* if RC4 is uses then buffer cannot be NULL */
    ChkArg( pCipherContext->eCipherType != eDRM_RC4_CIPHER
         || f_pbLast15 != NULL );

    ChkBOOL( pCipherContext->fInited, DRM_E_CIPHER_NOTINITIALIZED );

    if ( pCipherContext->eCipherType == eDRM_RC4_CIPHER )
    {
        ChkDR( DRM_BBX_CPHR_InitDecrypt( pCipherContext,
                                     f_pbLast15,
                                     f_cbData ) );
    }
    else
    {
        ChkBOOL( pCipherContext->eCipherType == eDRM_AES_COUNTER_CIPHER,
                 DRM_E_UNSUPPORTEDALGORITHM );
    }

ErrorExit:

    ChkECC( ECC_Drm_Reader_InitDecrypt, dr );

    DRM_PERF_LogMicrosecondCount( eDRM_PERF_LogTag_Exit, eDRM_PERF_LogFunction_Drm_Reader_InitDecrypt );
    
    return dr;
}

/******************************************************************************
**
** Function:    Drm_Reader_Decrypt
**
** Synopsis:    Decrypts part of the encrypted content.
**
** Arguments:
**
** [f_pDecryptContext]      -- Pointer to a DRM_DECRYPT_CONTEXT structure
**                             that was returned from a successful call to
**                             Drm_Reader_Bind().
** [f_pCtrContext]          -- Sets and maintains the initialization vector and
**                             counter for AES CTR mode. Must be specified for AES
**                             decryption, and must be NULL for Cocktail decryption.
** [f_pbData]               -- Pointer to a buffer that contains data to be decrypted.
** [f_cbData]               -- Byte count of the data to be decrypted.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguement is invalid.
**                          Any other return code as defined in drmresults.h.
**
** Notes :                  Content is decrypted in place. Clear output is the same
**                          length as the encrypted input.
**
**                          IMPORTANT: (COCKTAIL ONLY)
**                          Phased decrypt should be called on segments of
**                          encrypted data sequentially, i.e. if encrypted data is divided
**                          four segments, the order of decryption should be segment 1,
**                          followed by 2, 3, 4.
**                          To remove complexity we do not handle the case where
**                          a packet is split somewhere in the middle of the last 15 bytes.
**                          The caller is responsible to ensure that the last 15 bytes never
**                          straddle multiple calls to Decrypt.
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Reader_Decrypt(
    __in_ecount( 1 )           DRM_DECRYPT_CONTEXT          *f_pDecryptContext,
    __inout_opt                DRM_AES_COUNTER_MODE_CONTEXT *f_pCtrContext,
    __inout_bcount( f_cbData ) DRM_BYTE                     *f_pbData,
    __in                       DRM_DWORD                     f_cbData )
{
    DRM_RESULT          dr             = DRM_SUCCESS;
    DRM_CIPHER_CONTEXT *pCipherContext = ( DRM_CIPHER_CONTEXT * )f_pDecryptContext;

    DRM_PERF_LogMicrosecondCount( eDRM_PERF_LogTag_Enter, eDRM_PERF_LogFunction_Drm_Reader_Decrypt );
    
    dr = DRM_BBX_CPHR_Decrypt( pCipherContext,
                           f_pCtrContext,
                           f_cbData,
                           f_pbData );

    ChkECC( ECC_Drm_Reader_Decrypt, dr );

    DRM_PERF_LogMicrosecondCount( eDRM_PERF_LogTag_Exit, eDRM_PERF_LogFunction_Drm_Reader_Decrypt );
    
    return dr;
}

/**********************************************************************
**
** Function:    Drm_LicenseQuery_GetState
**
** Synopsis:    Retrieves data about the usability of content.  This is
**              data that is aggregated across all licenses that match
**              the header specified in Drm_Content_SetProperty().
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_rgpdstrRights]        -- An array of strings representing rights that
**                             the caller would like license state
**                             information about.
** [f_rgStateData]          -- Array of DRM_LICENSE_STATE_DATA structures
**                             that will hold the results of the queires.
** [f_cRightsQueried]       -- Number of elements in the f_rgpdstrRights array
**                             and the f_rgStateData array.
** [f_pfnPolicyCallback]    -- Optional callback function to process extensible
**                             restrictions found in the license.
** [f_pv]                   -- Optional callback specific data to be supplied
**                             to the callback.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any of the arguments are invalid.
**                          DRM_E_HEADER_NOT_SET if a header has not been set.
**                          Any other return codes as defined in drmresults.h.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_LicenseQuery_GetState(
    IN                                    DRM_APP_CONTEXT        *f_poAppContext,
    __in_ecount( f_cRightsQueried ) const DRM_CONST_STRING       *f_rgpdstrRights[],
    IN                                    DRM_DWORD               f_cRightsQueried,
    __out_ecount( f_cRightsQueried )      DRM_LICENSE_STATE_DATA  f_rgStateData[],
    IN                                    DRMPFNPOLICYCALLBACK    f_pfnPolicyCallback,
    IN const                              DRM_VOID               *f_pv )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_DWORD                 i                    = 0;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_LicenseQuery_GetState );

    ChkArg(  f_rgpdstrRights != NULL
          && f_rgStateData != NULL
          && f_cRightsQueried > 0
          && f_poAppContext != NULL );

    if ( poAppContextInternal->fBindThenCommit )
    {
        poAppContextInternal->fBindThenCommit = FALSE;
    }

    if ( !poAppContextInternal->fStoreOpened )
    {
        ChkDR( DRM_E_DRMNOTINIT );
    }
    if ( poAppContextInternal->eHeaderInContext == DRM_CSP_HEADER_NOT_SET )
    {
        ChkDR( DRM_E_HEADER_NOT_SET );  /* No v1 or V2 header is setup. */
    }

    ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

    /* Setup the stuff that GetProperties needs. */
    MEMCPY( &poAppContextInternal->oViewRightsContext.KID,
            &poAppContextInternal->KID,
            SIZEOF( DRM_KID ) );

    poAppContextInternal->oViewRightsContext.pBBContext = &poAppContextInternal->oBlackBoxContext;

    for ( i = 0; i < DRM_MAX_LICENSE_CHAIN_DEPTH; i++ )
    {
        poAppContextInternal->oViewRightsContext.rgpLicQueryContext[ i ] = &poAppContextInternal->oLicEnum[ i ];
    }

    poAppContextInternal->oViewRightsContext.pbGlobalSecStore  = &poAppContextInternal->oSecStoreGlobalContext;
    poAppContextInternal->oViewRightsContext.pbLIDSecStore     = &poAppContextInternal->rgcontextSST[0];
    poAppContextInternal->oViewRightsContext.pbLicenseStoreXML = &poAppContextInternal->oLicStoreContextXML;
    poAppContextInternal->oViewRightsContext.pbLicenseStoreXMR = &poAppContextInternal->oLicStoreContextXMR;
    poAppContextInternal->oViewRightsContext.pLicEval          = &poAppContextInternal->oLicEvalContext;
    poAppContextInternal->oViewRightsContext.pbBuffer          =  poAppContextInternal->pbOpaqueBuffer
                                                                  + __CB_DECL( poAppContextInternal->cbTotalHeaderData );
    poAppContextInternal->oViewRightsContext.cbBuffer          =  poAppContextInternal->cbOpaqueBuffer
                                                                  - poAppContextInternal->cbTotalHeaderData;

    ChkDR( DRM_VIEWRIGHTS_GetLicenseAggregateData(  f_rgpdstrRights,
                                                    f_rgStateData,
                                                    f_cRightsQueried,
                                                   &poAppContextInternal->oViewRightsContext,
                                                   &poAppContextInternal->oDatastoreHDS,
                                                    FALSE,
                                                    DRM_ASD_AGGREGATE_ROOT_LICENSES,
                                                    f_pfnPolicyCallback,
                                                    f_pv ) );

    /* Explicitly zeroed the view right context. */
    ZEROMEM( &poAppContextInternal->oViewRightsContext, SIZEOF( DRM_VIEW_RIGHTS_CONTEXT ) );

ErrorExit:

    _FreeDrmAppInternalContexts( poAppContextInternal );

    DRM_PROFILING_LEAVE_SCOPE;

    ChkECC( ECC_Drm_LicenseQuery_GetState, dr );

    return dr;
}

/**********************************************************************
**
** Function:    Drm_LicenseAcq_GetContentHeader
**
** Synopsis:    Retrieves a content header from the sync store by KID.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_pkid]                 -- KID of the content header to retreive.
** [f_pwszContentHeader]    -- Buffer to hold returned content header.
** [f_pcchContentHeader]    -- Size of the buffer in wide characters.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any of the parameters are invalid.
**                          Any other return codes as defined in drmresults.h.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_LicenseAcq_GetContentHeader(
    IN                                       DRM_APP_CONTEXT *f_poAppContext,
    IN                                       DRM_KID         *f_pkid,
    __out_ecount_opt( *f_pcchContentHeader ) DRM_WCHAR       *f_pwszContentHeader,
    IN OUT                                   DRM_DWORD       *f_pcchContentHeader )
{
    DRM_DWORD                 dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        ChkArg(   poAppContextInternal != NULL
               && f_pkid != NULL
               && f_pcchContentHeader != NULL );

        if ( !poAppContextInternal->fStoreOpened )
        {
            ChkDR( DRM_E_DRMNOTINIT );
        }

        ChkDR( DRM_SNC_GetSyncStoreEntry( &poAppContextInternal->contextSync,
                                           f_pkid,
                                           NULL,
                                           f_pwszContentHeader,
                                           f_pcchContentHeader ) );
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        ChkECC( ECC_Drm_LicenseAcq_GetContentHeader, dr );
    }

    return dr;
}

/**********************************************************************
**
** Function:    Drm_Sync_GenerateChallenge
**
** Synopsis:    Creates a license synchronization challenge in multiple
**              session.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_cMaxCount]            -- Maximum remaining playcount before license
**                             expiration to use evaluating licenses with
**                             count-based expirations. Use
**                             DRM_SYNC_IGNORE_THRESHOLD_PARAMETER (0xFFFFFFFF)
**                             to ignore this parameter and always sync
**                             count-based licenses.
** [f_cMaxHours]            -- Maximum number of hours before license
**                             expiration to use evaluating licenses with
**                             date-based expirations. Use
**                             DRM_SYNC_IGNORE_THRESHOLD_PARAMETER (0xFFFFFFFF)
**                             to ignore this parameter and always sync
**                             date-based licenses.
** [f_iKIDStart]            -- Starting index of KID in the sync store to be included
**                             in the challenge.
** [f_cKIDs]                -- Maximum number KIDs to be included in the challenge.
**                             The actual number of KIDs included may be smaller than
**                             this value.
** [f_piKIDNext]            -- Pointer to a DWORD to receive index of KID in sync store
**                             to be used as the starting index of the next challenge.
** [f_pcKIDs]               -- Pointer to a DWORD to receive the actual number of KIDs
**                             included in the challenge.
** [f_pbData]               -- User-allocated buffer to be used as output buffer.
** [f_pcbData]              -- Size of the user allocated buffer.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          DRM_E_BUFFERTOOSMALL if the output buffer is too small.
**
** Notes:                   To get the minimum size required, first call with
**                          f_pbChallenge=NULL and *f_pcbChallenge=0. This will
**                          return DRM_E_BUFFERTOOSMALL and *f_pcbChallenge will
**                          contain the required buffer size.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Sync_GenerateChallenge(
    IN                             DRM_APP_CONTEXT *f_poAppContext,
    IN                             DRM_DWORD        f_cMaxCount,
    IN                             DRM_DWORD        f_cMaxHours,
    IN                             DRM_DWORD        f_iKIDStart,
    IN                             DRM_DWORD        f_cKIDs,
    OUT                            DRM_DWORD       *f_piKIDNext,
    OUT                            DRM_DWORD       *f_pcKIDs,
    __out_bcount_opt( *f_pcbData ) DRM_BYTE        *f_pbData,
    IN OUT                         DRM_DWORD       *f_pcbData )
{
    DRM_APP_CONTEXT_INTERNAL   *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_VIEW_RIGHTS_CONTEXT    *pcontextViewRights   = NULL;
    DRM_STACK_ALLOCATOR_CONTEXT contextStack         = { 0 };
    DRM_DST                    *pDatastore           = NULL;
    DRM_RESULT                  dr                   = DRM_SUCCESS;
    DRM_DWORD                   cbStack              = 0;
    DRM_DWORD                   cbViewRights         = 0;
    DRM_DWORD                   i                    = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Sync_GenerateChallenge );

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        ChkArg(  poAppContextInternal != NULL );
        ChkArg(  f_pcbData != NULL );
        ChkArg(  *f_pcbData >= SIZEOF( _XMBContext ) || f_pbData == NULL );

        if ( poAppContextInternal->fBindThenCommit )
        {
            poAppContextInternal->fBindThenCommit = FALSE;
        }

        /* Caller must do Drm_Initialize() before Drm_Sync_GenerateChallenge(). */
        if ( !poAppContextInternal->fStoreOpened )
        {
            ChkDR( DRM_E_DRMNOTINIT );
        }

        pDatastore         = &poAppContextInternal->oDatastoreHDS;
        pcontextViewRights = &poAppContextInternal->oViewRightsContext;

        /*
        ** Neither view rights nor GenerateSyncChallenge need a
        ** lot of stack; split this one between both.
        ** | rgbLicense .... |
        **   ^ stack
        **          ^ viewrights
        **
        */
        cbStack = SIZEOF( DRM_SYNC_CONTEXT ) +
                  SIZEOF( DRM_SYNC_ENUM_CONTEXT ) +
                  20;

        cbViewRights = poAppContextInternal->cbDRMLicense -
                       cbStack;

        contextStack.pbStack = poAppContextInternal->pbDRMLicense;
        contextStack.cbStack = cbStack;

        pcontextViewRights->pbBuffer = contextStack.pbStack +
                                       __CB_DECL( contextStack.cbStack );
        pcontextViewRights->cbBuffer = cbViewRights;

        /* Set up view rights context for license store query. */
        pcontextViewRights->pBBContext = &poAppContextInternal->oBlackBoxContext;

        for ( i = 0; i < DRM_MAX_LICENSE_CHAIN_DEPTH; i++ )
        {
            pcontextViewRights->rgpLicQueryContext[ i ] = &poAppContextInternal->oLicEnum[ i ];
        }

        pcontextViewRights->pbGlobalSecStore  = &poAppContextInternal->oSecStoreGlobalContext;
        pcontextViewRights->pbLIDSecStore     = &poAppContextInternal->rgcontextSST[ 0 ];
        pcontextViewRights->pbLicenseStoreXML = &poAppContextInternal->oLicStoreContextXML;
        pcontextViewRights->pLicEval          = &poAppContextInternal->oLicEvalContext;

        dr = DRM_SNC_GenerateSyncChallenge( &poAppContextInternal->contextSync,
                                             pcontextViewRights,
                                             pDatastore,
                                            &contextStack,
                                             f_cMaxCount,
                                             f_cMaxHours,
                                             f_iKIDStart,
                                             f_cKIDs,
                                             f_piKIDNext,
                                             f_pcKIDs,
                                             ( _XMBContext * )f_pbData,
                                             f_pcbData );
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:

    ChkECC( ECC_Drm_Sync_GenerateChallenge, dr );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    Drm_SecureClock_ProcessResponse
**
** Synopsis:    Process Secure clock response received from server.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_pbResponse]           -- Response string received from serve.
** [f_cbResponse]           -- Size of f_pbResponse in bytes.
** [f_pResult]              -- Pointer to DRM_RESULT to get error from server
**                             included in response.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          DRM_E_CLK_INVALID_RESPONSE if the response is
**                          corrupted.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_SecureClock_ProcessResponse(
    IN                          DRM_APP_CONTEXT *f_poAppContext,
    __in_bcount( f_cbResponse ) DRM_BYTE        *f_pbResponse,
    IN                          DRM_DWORD        f_cbResponse,
    OUT                         DRM_RESULT      *f_pResult )
{
    return DRM_CLK_Drm_SecureClock_ProcessResponseImpl(
        f_poAppContext,
        f_pbResponse,
        f_cbResponse,
        f_pResult );
}

/**********************************************************************
**
** Function:    Drm_SecureClock_GenerateChallenge
**
** Synopsis:    Creates a Secure Clock challenge.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_pwszUrl]              -- User allocated buffer to get URL in WCHAR.
**                             Null Terminated.
** [f_pcchUrl]              -- Character count of pwszUrl. Can be NULL if
**                             URL is not required.
** [f_pbChallenge]          -- User allocated buffer to be used as output buffer.
** [f_pcbChallenge]         -- Size of f_pbChallenge
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          DRM_E_BUFFERTOOSMALL if the output buffer is too small.
**                          DRM_E_PRIVKEYREADERROR if an error occurred reading the device private key.
**                          DRM_E_DEVCERTREADERROR if an error occurred reading the device certificate.
**                          DRM_E_INVALIDDEVICECERTIFICATETEMPLATE if the specified device certificate
**                                                                 template is invalid.
**
** Notes:                   To get the minimum size required, first call with
**                          f_pbChallenge=NULL and *f_pcbChallenge=0.
**                          This will return DRM_E_BUFFERTOOSMALL and *f_pcchUrl
**                          and *f_pcbChallenge will contain the minimum context size.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_SecureClock_GenerateChallenge(
    IN                                  DRM_APP_CONTEXT *f_poAppContext,
    __out_ecount_opt( *f_pcchURL )      DRM_WCHAR       *f_pwszURL,
    IN OUT                              DRM_DWORD       *f_pcchURL,
    __out_bcount_opt( *f_pcbChallenge ) DRM_BYTE        *f_pbChallenge,
    IN OUT                              DRM_DWORD       *f_pcbChallenge )
{
    return DRM_CLK_Drm_SecureClock_GenerateChallengeImpl(
        f_poAppContext,
        f_pwszURL,
        f_pcchURL,
        f_pbChallenge,
        f_pcbChallenge );
}


/**********************************************************************
**
** Function:    Drm_SecureClock_GetValue
**
** Synopsis:    Get Secure clock state.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_pwszSecureTime]       -- Optional pointer to WCHAR buffer to get Secure time.
** [f_pcchSecureTime]       -- Optional size of buffer in WCHAR.
** [f_pdwFlags]             -- Optional pointer to DWORD.
** [f_pbSecureTimeWithStatus]
**                          -- Optional pointer to BYTE buffer to get Secure.
**                             time and flag status in XML format.
** [f_pcbSecureTimeWithStatus]
**                          -- Optional size of buffer.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          DRM_E_BUFFERTOOSMALL if the output buffer is too small.
**                          DRM_E_CLK_NOT_SUPPORTED if device does not support a secure clock.
**                          DRM_E_PRIVKEYREADERROR if an error occurred reading the device private key.
**                          Other return code as defined in drmresults.h
**
** Notes:                   There are 2 forms in which the secure clock data
**                          is being returned - either in f_poSecureTime and
**                          f_pdwFlags or in f_pbSecureTimeWithStatus. The
**                          second one in in XML format which contains both
**                          information as in f_poSecureTime and f_pdwFlags.
**                          You can choose either format.
**                          To get minimum buffer size, this API should be called
**                          with *f_pcchSecureTime=0 (or *f_pcbSecureTimeWithStatus=0)
**                          This will return DRM_E_BUFFERTOOSMALL and f_pcchSecureTime
**                          and f_pcbSecureTimeWithStatus will point to a value
**                          of minimum context size.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_SecureClock_GetValue(
    IN                                            DRM_APP_CONTEXT *f_poAppContext,
    __in_ecount_opt( *f_pcchSecureTime )          DRM_WCHAR       *f_pwszSecureTime,
    IN OUT                                        DRM_DWORD       *f_pcchSecureTime,
    IN                                            DRM_DWORD       *f_pdwFlags,
    __in_bcount_opt( *f_pcbSecureTimeWithStatus ) DRM_BYTE        *f_pbSecureTimeWithStatus,
    IN OUT                                        DRM_DWORD       *f_pcbSecureTimeWithStatus )
{
    return DRM_CLK_Drm_SecureClock_GetValueImpl(
        f_poAppContext,
        f_pwszSecureTime,
        f_pcchSecureTime,
        f_pdwFlags,
        f_pbSecureTimeWithStatus,
        f_pcbSecureTimeWithStatus );
}

/**********************************************************************
**
** Function:    Drm_StoreMgmt_CleanupStore
**
** Synopsis:    Instructs DRM to delete all unusable licenses from the
**              license store and perform maintaince functions on the
**              data store file.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_dwFlags]              -- Flags to indicate what type of clean operations
**                             to perform.  Valid values are:
**                                DRM_STORE_CLEANUP_ALL
**                             or any combination of:
**                                DRM_STORE_CLEANUP_DELETE_EXPIRED_LICENSES
**                                DRM_STORE_CLEANUP_DELETE_REMOVAL_DATE_LICENSES
** [f_pvCallerData]         -- Opaque context data to be funnelled back to
**                             caller during callback. Could be NULL.
** [f_dwCallbackInterval]   -- % completion after which Callback (if any)
**                             should be invoked.
**                             e.g. if dwCallbackInterval = 5, callback will
**                             be invoked after 5%, 10%, 15%...completion.
** [f_pfnCallback]          -- Optional caller supplied callback function.
**                             It can be NULL.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmrestults.h.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_StoreMgmt_CleanupStore(
    IN       DRM_APP_CONTEXT        *f_poAppContext,
    IN       DRM_DWORD               f_dwFlags,
    IN const DRM_VOID               *f_pvCallerData,
    IN       DRM_DWORD               f_dwCallbackInterval,
    IN       pfnStoreCleanupProgress f_pfnCallback )
{
    DRM_RESULT                dr                   =  DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_StoreMgmt_CleanupStore );
    DRM_PERF_LogMicrosecondCount( eDRM_PERF_LogTag_Enter, eDRM_PERF_LogFunction_Drm_StoreMgmt_CleanupStore );

    ChkArg ( f_poAppContext != NULL );

    ChkBOOL( DRM_CLEANSTORE_IsCleanStoreSupported(), DRM_E_NOTIMPL );

    if ( poAppContextInternal->fBindThenCommit )
    {
        poAppContextInternal->fBindThenCommit = FALSE;
    }

    ChkDR( _SetupLicEvalObjectToShare( poAppContextInternal ) );

    dr = DRM_LST_Clean(  ( DRM_BYTE * )&poAppContextInternal->oLicEvalContext,
                         ( DRM_BYTE * )&poAppContextInternal->oLicStoreContextXML,
                         ( DRM_BYTE * )&poAppContextInternal->oLicStoreContextXMR,
                         &poAppContextInternal->oLicEnum[0],
                         f_dwFlags,
                         poAppContextInternal->pbDRMLicense,
                         DRM_MAX_LICENSESIZE,
                         f_pvCallerData,
                         f_dwCallbackInterval,
                         f_pfnCallback,
                        &poAppContextInternal->oDatastoreHDS );

ErrorExit:

    _FreeDrmAppInternalContexts( poAppContextInternal );

    ChkECC( ECC_Drm_StoreMgmt_CleanupStore, dr );

    DRM_PERF_LogMicrosecondCount( eDRM_PERF_LogTag_Exit, eDRM_PERF_LogFunction_Drm_StoreMgmt_CleanupStore );
    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    Drm_LicenseQuery_IsAllowed
**
** Synopsis:    Gets License state for the KID included in f_pdstrQuery.
**              Currently it tells whether license is usable for Play action.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_pdstrQuery]           -- Query containing KID, Action etc.
** [f_pCategory]            -- Returns DRM_LICENSE_STATE_CATEGORY.
** [f_pfnPolicyCallback]    -- Optional callback to allow processing of
**                             extended restrictions.
** [f_pvCallbackData]       -- Optional pointer that allows caller data
**                             to be sent to the callback.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmrestults.h.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_LicenseQuery_IsAllowed(
    IN       DRM_APP_CONTEXT            *f_poAppContext,
    IN const DRM_CONST_STRING           *f_pdstrQuery,
    IN       DRMPFNPOLICYCALLBACK        f_pfnPolicyCallback,
    IN const DRM_VOID                   *f_pvCallbackData,
    OUT      DRM_LICENSE_STATE_CATEGORY *f_pCategory )
{
    DRM_RESULT                dr                             = DRM_SUCCESS;
    DRM_LONG                  lCanBind                       = 0;
    DRM_CONST_STRING          dstrRoot                       = EMPTY_DRM_STRING;
    DRM_CONST_STRING          dstrData                       = EMPTY_DRM_STRING;
    DRM_CONST_STRING          dstrValue                      = EMPTY_DRM_STRING;
    DRM_DWORD                 iAction                        = 0;
    const DRM_CONST_STRING   *pdstrRights[ DRM_MAX_ACTIONS ] = { 0 };
    DRM_CONST_STRING          dstrRights[ DRM_MAX_ACTIONS ]  = { 0 };
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal           = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_DECRYPT_CONTEXT       oUnusedDecyrptContext          = { 0 };

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_LicenseQuery_IsAllowed );

    ChkArg( f_poAppContext != NULL &&
            f_pCategory != NULL );

    ChkDRMString( f_pdstrQuery );

    /* Init the out parameter. */
    *f_pCategory = DRM_LICENSE_STATE_NORIGHT;

    /*
    ** <LICENSESTATE type = "challenge">
    ** <DATA>
    **    <ACTION>Play</ACTION>
    **    <KID>KID directly from license</KID>
    **    <CANBIND>1</CANBIND>
    ** </DATA>
    ** </LICENSESTATE >
    */
    ChkDR( DRM_XML_GetNode(  f_pdstrQuery,
                            &g_dstrTagLicenseStateRequestRoot,
                             NULL,
                             NULL,
                             0,
                             NULL,
                            &dstrRoot ) );

    ChkDR( DRM_XML_GetNode( &dstrRoot,
                            &g_dstrTagData,
                             NULL,
                             NULL,
                             0,
                             NULL,
                            &dstrData ) );

    ChkDR( DRM_XML_GetNode( &dstrData,
                            &g_dstrTagKID,
                             NULL,
                             NULL,
                             0,
                             NULL,
                            &dstrValue ) );

    if (  CB_DSTR( &dstrValue ) > DRM_MAX_HEADERDATASIZE
       || CB_DSTR( &dstrValue ) == 0 )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    MEMCPY( poAppContextInternal->pbDRMHeaderData,
            PB_DSTR( &dstrValue ),
            CB_DSTR( &dstrValue ) );

    poAppContextInternal->cbHeaderData      = CB_DSTR( &dstrValue );
    poAppContextInternal->cbTotalHeaderData = poAppContextInternal->cbHeaderData;
    poAppContextInternal->eHeaderInContext  = DRM_CSP_KID;
    poAppContextInternal->pwszHeaderData    = ( DRM_WCHAR * )dstrValue.pwszString;
    poAppContextInternal->pbEmbeddedStore   = NULL;
    poAppContextInternal->cbEmbeddedStore   = 0;

    ChkDR( DRM_XML_GetNode( &dstrData,
                            &g_dstrTagCanBind,
                             NULL,
                             NULL,
                             0,
                             NULL,
                            &dstrValue ) );

    ChkDR( DRMCRT_wcsntol( dstrValue.pwszString, dstrValue.cchString, &lCanBind ) );

    if ( lCanBind != 1 )
    {
        ChkDR( DRM_E_INVALIDARG );
    }

    do
    {
        dr = DRM_XML_GetNode( &dstrData,
                              &g_dstrTagAction,
                               NULL,
                               NULL,
                               iAction,
                               NULL,
                              &dstrValue );

        if ( dr == DRM_E_XMLNOTFOUND )
        {
            break;
        }

        ChkDR( dr );

        ChkArg( iAction < DRM_MAX_ACTIONS );

        dstrRights[ iAction ]  = dstrValue;
        pdstrRights[ iAction ] = &dstrRights[ iAction ];
        iAction++;
    } while( TRUE );

    ChkArg( iAction > 0 );

    /* Tell Bind to not bother reporting actions. */
    poAppContextInternal->fCanBind = TRUE;

    dr = Drm_Reader_Bind( f_poAppContext,
                          pdstrRights,
                          iAction,
                          f_pfnPolicyCallback,
                          f_pvCallbackData,
                          &oUnusedDecyrptContext );

    if ( DRM_FAILED( dr ) )
    {
        if ( dr == DRM_E_CLK_NOT_SET )
        {
            *f_pCategory = DRM_LICENSE_STATE_UNLIM;
        }

        dr = DRM_SUCCESS;

        goto ErrorExit;
    }

    *f_pCategory = DRM_LICENSE_STATE_UNLIM;

ErrorExit:
    OEM_SECURE_ZERO_MEMORY( &oUnusedDecyrptContext, SIZEOF( DRM_DECRYPT_CONTEXT ) );
    if ( poAppContextInternal != NULL )
    {
        poAppContextInternal->fCanBind = FALSE;
    }

    ChkECC( ECC_Drm_LicenseQuery_IsAllowed, dr );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

static DRM_RESULT _DeleteLicenses(
    IN  DRM_APP_CONTEXT_INTERNAL *f_poAppContextInternal,
    IN  DRM_CONST_STRING         *f_pdstrLGPubKey,
    IN  DRM_KID                  *f_pKID,
    OUT DRM_DWORD                *f_pcLicDeleted )
{
    DRM_RESULT       dr           = DRM_SUCCESS;
    DRM_DWORD        cbLicSize    = 0;
    DRM_CONST_STRING dstrLicense  = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrLGPubkey = EMPTY_DRM_STRING;

    ChkArg(  f_poAppContextInternal != NULL
          && f_pdstrLGPubKey != NULL
          && f_pKID != NULL
          && f_pcLicDeleted != NULL );

    *f_pcLicDeleted = 0;

    dr = DRM_LST_InitEnum( &f_poAppContextInternal->oLicStoreContextXML,
                            f_pKID,
                            FALSE,
                           &f_poAppContextInternal->oLicEnum[ 0 ] );

    if ( DRM_SUCCEEDED( dr ) )
    {
        while( TRUE )
        {
            DRM_DST_SLOT_HINT slotHint;

            dr = DRM_LST_EnumNext( &f_poAppContextInternal->oLicEnum[ 0 ],
                                    NULL,
                                   &f_poAppContextInternal->oFFLicense.rglid[ 0 ],
                                   &slotHint,
                                   &cbLicSize );

            if ( dr == DRM_E_NOMORE )
            {
                dr = DRM_SUCCESS;

                goto ErrorExit;
            }

            ChkDR( dr );

            if ( cbLicSize > f_poAppContextInternal->cbDRMLicense )
            {
                /* Can't read the license. Just continue. */
                continue;
            }

            ChkDRContinue( DRM_LST_GetLicense( &f_poAppContextInternal->oLicStoreContextXML,
                                                f_pKID,
                                               &f_poAppContextInternal->oFFLicense.rglid[ 0 ],
                                               &slotHint,
                                                f_poAppContextInternal->pbDRMLicense,
                                               &cbLicSize ) );

            DSTR_FROM_PB( &dstrLicense, f_poAppContextInternal->pbDRMLicense, cbLicSize );

            ChkDRContinue( DRM_WMDRM_LIC_GetAttribute( &dstrLicense,
                                                       &g_dstrTagRevocationPubKey,
                                                        DRM_LICENSE_ATTRIB_META_ATTRIBUTE,
                                                        NULL,
                                                       &dstrLGPubkey,
                                                        0 ) );

            if ( DRM_UTL_DSTRStringsEqual( &dstrLGPubkey, f_pdstrLGPubKey ) )
            {
                dr = DRM_LST_EnumDelete( &f_poAppContextInternal->oLicEnum[ 0 ] );

                if ( DRM_SUCCEEDED( dr ) )
                {
                    ( *f_pcLicDeleted )++;
                }

                if( DRM_SNC_IsLicenseSyncSupported() )
                {
                    ( DRM_VOID )DRM_SNC_DeleteKID( &f_poAppContextInternal->contextSync, f_pKID );
                }
            }
        }
    }

ErrorExit:

    return dr;
}


/*
<RESPONSE type="dlrb">
<DATA>
    <VERSION value="verionval"/>
    <LGPUBKEY value="lgapubkeyval"/>
    <KID value="kid1"/>
    <KID value="kid2"/>
    <!-- ...-->
</DATA>
<SIGNATURE>
    <HASHALGORITH type="SHA"/>
    <SIGNALGORITHM type="MSDRM"/>
    <VALUE>...</VALUE>
</SIGNATURE>
</RESPONSE>
*/
static DRM_RESULT _ProcessLicenseRevocationList(
    IN  DRM_APP_CONTEXT_INTERNAL *f_poAppContextInternal,
    IN  DRM_DWORD                 f_iKids,
    IN  DRM_DWORD                 f_cKids,
    IN  DRM_BYTE                 *f_pbList,
    IN  DRM_DWORD                 f_cbList,
    OUT DRM_DWORD                *f_piKids,
    OUT DRM_DWORD                *f_pcLicDeleted )
{
    DRM_DWORD        iKids      = 0;
    DRM_DWORD        cbBuffer   = 0;
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_CONST_STRING dstrRoot   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrData   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrPubKey = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrSign   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrBlob   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrValue  = EMPTY_DRM_STRING;

    ChkArg(  f_poAppContextInternal != NULL
          && f_pbList != NULL
          && f_piKids != NULL
          && f_pcLicDeleted != NULL
          && f_cbList > 0 );

    *f_piKids       = 0;
    *f_pcLicDeleted = 0;

    if ( f_cKids == 0 )
    {
        goto ErrorExit;
    }

    DSTR_FROM_PB( &dstrBlob, f_pbList, f_cbList );

    ChkDR( DRM_XML_GetNode( &dstrBlob,
                            &g_dstrTagDeviceLicenseRevocationRoot,
                             NULL,
                             NULL,
                             0,
                             NULL,
                            &dstrRoot ) );

    ChkDR( DRM_XML_GetNode( &dstrRoot,
                            &g_dstrTagData,
                             NULL,
                             NULL,
                             0,
                             NULL,
                            &dstrData ) );

    ChkDR( DRM_XML_GetNode( &dstrData,
                            &g_dstrTagRevocationPubKey,
                             NULL,
                             NULL,
                             0,
                            &dstrValue,
                             NULL ) );

    ChkDR( DRM_XML_GetNodeAttribute( &dstrValue, &g_dstrLabelValue, &dstrPubKey ) );

    /* Check the signature first. */
    ChkDR( DRM_XML_GetNode( &dstrRoot,
                            &g_dstrTagSignature,
                             NULL,
                             NULL,
                             0,
                             NULL,
                            &dstrSign ) );

    ChkDR( DRM_XML_GetNode( &dstrSign,
                            &g_dstrTagValue,
                             NULL,
                             NULL,
                             0,
                             NULL,
                            &dstrValue ) );

    /* Base64 decode Pubkey and this signature. */
    cbBuffer = SIZEOF( PUBKEY );

    ChkDR( DRM_B64_DecodeW( &dstrPubKey,
                            &cbBuffer,
                             ( DRM_BYTE * )&f_poAppContextInternal->oBlackBoxContext.CryptoContext.pubKey,
                             0 ) );

    cbBuffer = SIZEOF( f_poAppContextInternal->oBlackBoxContext.CryptoContext.signature );

    ChkDR( DRM_B64_DecodeW( &dstrValue,
                            &cbBuffer,
                             f_poAppContextInternal->oBlackBoxContext.CryptoContext.signature, 0 ) );

    ChkDR( DRM_XML_GetNode( &dstrRoot,
                            &g_dstrTagData,
                             NULL,
                             NULL,
                             0,
                            &dstrValue,
                             NULL ) );

    ChkBOOL( DRM_SUCCEEDED( DRM_PK_Verify( f_poAppContextInternal->oBlackBoxContext.CryptoContext.rgbCryptoContext,
                                           eDRM_ECC_P160,
                                           DRM_ECC160_PUBLIC_KEY_LEN,
                                           ( const DRM_BYTE * )&f_poAppContextInternal->oBlackBoxContext.CryptoContext.pubKey,
                                           CB_DSTR( &dstrValue ),
                                           PB_DSTR( &dstrValue ),
                                           DRM_ECC160_SIGNATURE_LEN,
                                           f_poAppContextInternal->oBlackBoxContext.CryptoContext.signature ) ),
             DRM_E_INVALID_LICENSE_REVOCATION_LIST_SIGNATURE );

    for ( iKids = f_iKids; iKids < f_iKids + f_cKids; iKids++, ( *f_piKids )++ )
    {
        DRM_DWORD nDeleted = 0;

        /* Reuse dstrBlob. */

        dr = DRM_XML_GetNode( &dstrData,
                              &g_dstrTagKID,
                               NULL,
                               NULL,
                               iKids,
                              &dstrBlob,
                               NULL );

        if ( DRM_SUCCEEDED( dr ) )
        {

             /* Call function to delete licenses. */
            ChkDRContinue( DRM_XML_GetNodeAttribute( &dstrBlob, &g_dstrLabelValue, &dstrValue ) );

            ChkDRContinue( DRM_UTL_DecodeKID( &dstrValue, &f_poAppContextInternal->KID ) );

            ( DRM_VOID )_DeleteLicenses(  f_poAppContextInternal,
                                         &dstrPubKey,
                                         &f_poAppContextInternal->KID,
                                         &nDeleted );

            *f_pcLicDeleted += nDeleted;
        }
        else
        {
            if ( dr == DRM_E_XMLNOTFOUND )
            {
                /* No more KIDS found. */
                dr = DRM_SUCCESS;

                break;
            }

            ChkDR( dr );  /* Other errors. */
        }
    }

ErrorExit:

    return dr;
}

/*****************************************************************************
** Function:    Drm_Revocation_GetList
**
** Synopsis:    Open the secure store and get the version number and buffer
**              containing the revocation list currently stored based upon the
**              type of revocation list specified.
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_eRevType]             -- Type of Revocation List and Version to retrieve.
** [f_pbBuffer]             -- Pointer to a user-allocated buffer to hold the
**                             revocation list. If NULL is specified, the
**                             required buffer size is returned.
** [f_pcbBuffer]            -- Size of f_pbBuffer, in bytes, returns size of
**                             revocation list.
** [f_pqwVersion]           -- Returns version of revocation list.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          DRM_E_NOTIMPL if the revocation type is unknown.
**                          DRM_E_PRIVKEYREADERROR if BB context not initialized.
**                          DRM_E_SLOTNOTFOUND if revocation list doesn't exist
**                          in secure store.
**                          DRM_E_BUFFERTOOSMALL if *f_pcbBuffer smaller than
**                          required buffer size to return revocation list.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Revocation_GetList(
    __inout                          DRM_APP_CONTEXT          *f_poAppContext,
    __in                             DRM_REVOCATION_TYPE_ENUM  f_eRevType,
    __out_bcount_opt( *f_pcbBuffer ) DRM_BYTE                 *f_pbBuffer,
    __inout_opt                      DRM_DWORD                *f_pcbBuffer,
    __out_opt                        DRM_DWORD                *f_pdwVersion )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_DWORD                 cbRevocationBuffer   = 0;
    const DRM_GUID           *pguidRevocationType  = NULL;

    ChkArg(  f_poAppContext != NULL );

    ChkArg( poAppContextInternal != NULL );

    cbRevocationBuffer = poAppContextInternal->cbRevocationBuffer;

    switch( f_eRevType )
    {
        case DRM_REVOCATION_TYPE_WMDRM_APP:
            ChkBOOL( DRM_APPREVOCATION_IsAppRevocationSupported(), DRM_E_NOTIMPL );
            pguidRevocationType = &g_guidRevocationTypeApp;
            break;

        case DRM_REVOCATION_TYPE_WMDRM_DEVICE:
            ChkBOOL( DRM_DEVICEREVOCATION_IsDeviceRevocationSupported(), DRM_E_NOTIMPL );
            pguidRevocationType = &g_guidRevocationTypeDevice;
            break;

        case DRM_REVOCATION_TYPE_WMDRM_ND:
            ChkBOOL( DRM_WMDRMNET_IsWmdrmnetSupported(), DRM_E_NOTIMPL );
            pguidRevocationType = &g_guidRevocationTypeWMDRMNET;
            break;

        case DRM_REVOCATION_TYPE_PLAYREADY_APP:
            pguidRevocationType = &g_guidRevocationTypePlayReadySilverLightApplication;
            break;

        case DRM_REVOCATION_TYPE_PLAYREADY_RUNTIME:
            pguidRevocationType = &g_guidRevocationTypePlayReadySilverLightRuntime;
            break;

        case DRM_REVOCATION_TYPE_WMDRM_REVINFO:
            pguidRevocationType = &g_guidRevocationTypeRevInfo;
            break;

        case DRM_REVOCATION_TYPE_PLAYREADY_REVINFO2:
            pguidRevocationType = &g_guidRevocationTypeRevInfo2;
            break;

        default:
            ChkDR( DRM_E_INVALIDARG );
            break;
    }

    if( f_pcbBuffer == NULL
     || f_eRevType == DRM_REVOCATION_TYPE_WMDRM_REVINFO )
    {
        /* They're only interested in the version, use the revocation buffer,
        ** or they're asking for a legacy revinfo V1 which we should B64 encode
        ** before returning
        */
        dr = DRM_RVK_GetCRL(&poAppContextInternal->oBlackBoxContext.CryptoContext,
                              &poAppContextInternal->oRevContext,
                               pguidRevocationType,
                               poAppContextInternal->pbRevocationBuffer,
                              &cbRevocationBuffer,
                               f_pdwVersion);

        if( dr == DRM_E_BUFFERTOOSMALL )
        {
            dr = DRM_E_REVOCATION_BUFFERTOOSMALL;
        }
        ChkDR( dr );

        if( f_eRevType == DRM_REVOCATION_TYPE_WMDRM_REVINFO
         && f_pcbBuffer != NULL )
        {
            if ( cbRevocationBuffer > 0 )
            {
                /* Base64 encode expects the number of unicode characters in the buffer, not bytes. */
                *f_pcbBuffer /= SIZEOF( DRM_WCHAR );

                dr = DRM_B64_EncodeW( poAppContextInternal->pbRevocationBuffer,
                                      cbRevocationBuffer,
                                      ( DRM_WCHAR * )f_pbBuffer,
                                      f_pcbBuffer,
                                      0 );

                *f_pcbBuffer *= SIZEOF( DRM_WCHAR );

                ChkDR( dr );
            }
            else
            {
                *f_pcbBuffer = 0;
            }
        }
    }
    else
    {
        ChkDR( DRM_RVK_GetCRL(&poAppContextInternal->oBlackBoxContext.CryptoContext,
                              &poAppContextInternal->oRevContext,
                               pguidRevocationType,
                               f_pbBuffer,
                               f_pcbBuffer,
                               f_pdwVersion) );
    }

ErrorExit:
    return dr;
}


/**********************************************************************
**
** Function:    Drm_StoreMgmt_DeleteLicenses
**
** Synopsis:    Deletes all licenses with a given KID, and removes them.
**              from the sync list to prevent them from being automatically
**              reacquired.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_pdcstrKID]            -- Optional KID of licenses to delete, if NULL
**                             then the KID must have been passed in via
**                             a content header (ie Drm_Content_SetProperty()).
** [f_pcLicDeleted]         -- Optional DWORD to receive the number of
**                             licenses deleted.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_StoreMgmt_DeleteLicenses(
    IN  DRM_APP_CONTEXT  *f_poAppContext,
    IN  DRM_CONST_STRING *f_pdcstrKID,
    OUT DRM_DWORD        *f_pcLicDeleted )
{
    DRM_RESULT                  dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL   *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_DWORD                   cbLicSize            = 0;
    DRM_DWORD                   i                    = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_StoreMgmt_DeleteLicenses );

    /* They need to either supply a KID as an argument, or have supplied a header. */
    ChkArg(  f_poAppContext != NULL
          && ( f_pdcstrKID != NULL
          || ( poAppContextInternal->cbHeaderData > 0
          &&   ( poAppContextInternal->eHeaderInContext == DRM_CSP_V2_HEADER
          ||     poAppContextInternal->eHeaderInContext == DRM_CSP_V2_4_HEADER
          ||     poAppContextInternal->eHeaderInContext == DRM_CSP_V4_HEADER
          ||     poAppContextInternal->eHeaderInContext == DRM_CSP_V4_1_HEADER
          ||     poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ
          ||     poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID
          ||     poAppContextInternal->eHeaderInContext == DRM_CSP_KID
          ||     poAppContextInternal->eHeaderInContext == DRM_CSP_V1_HEADER ) ) ) );

    if ( f_pcLicDeleted != NULL )
    {
        *f_pcLicDeleted = 0;
    }

    if ( f_pdcstrKID != NULL )
    {
        ChkDR( DRM_UTL_DecodeKID( f_pdcstrKID, &poAppContextInternal->KID ) );
    }
    else if (  poAppContextInternal->eHeaderInContext == DRM_CSP_V2_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_V2_4_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_V4_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_V4_1_HEADER
            || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ )
    {
        DRM_CONST_STRING dstrKID = EMPTY_DRM_STRING;

        DSTR_FROM_PB( &poAppContextInternal->oLicEvalContext.dstrContentHeader,
                       poAppContextInternal->pwszHeaderData,
                       poAppContextInternal->cbHeaderData );

        ChkDR( DRM_HDR_GetAttribute( &poAppContextInternal->oLicEvalContext.dstrContentHeader,
                                      NULL,
                                      DRM_HEADER_ATTRIB_KID,
                                     &dstrKID,
                                      0 ) );

        ChkDR( DRM_UTL_DecodeKID( &dstrKID, &poAppContextInternal->KID ) );
    }
    else if ( poAppContextInternal->eHeaderInContext == DRM_CSP_V1_HEADER )
    {
        DRM_WCHAR         wszKID[ CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) ] = { 0 };
        DRM_CONST_STRING  dstrKID                                         = EMPTY_DRM_STRING;
        DRM_STRING_WINDOW dstrwKID                                        = EMPTY_DRM_STRING_WINDOW;
        DRM_V1Header     *pV1Header                                       = ( DRM_V1Header * )poAppContextInternal->pbDRMHeaderData;

        /* Make sure the size of encoded KID is no more than CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ). */
        dstrwKID.m_ichMin = 0;
        dstrwKID.m_ichMaxExclusive = ( pV1Header->cbKeyID <= CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) ) )?
                                       pV1Header->cbKeyID :
                                       CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );

        dstrKID.cchString = CCH_BASE64_EQUIV( SIZEOF( DRM_KID ) );
        dstrKID.pwszString = wszKID;

        /* Convert to UTF-16 (DRM string). */
        ChkDR( DRM_STR_UTF8toDSTR_ByWindow( ( DRM_CHAR * )( pV1Header->pbKeyID ), &dstrwKID, ( DRM_STRING * )&dstrKID ) );
        ChkDR( DRM_UTL_DecodeKID( &dstrKID, &poAppContextInternal->KID ) );
    }
    else if ( poAppContextInternal->eHeaderInContext == DRM_CSP_KID
           || poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ_WITH_KID )
    {
        DRM_CONST_STRING dstrKID = EMPTY_DRM_STRING;
        DSTR_FROM_PB( &dstrKID,
                       poAppContextInternal->pwszHeaderData,
                       poAppContextInternal->cbHeaderData );

        ChkDR( DRM_UTL_DecodeKID( &dstrKID, &poAppContextInternal->KID ) );
    }

    for ( i = 0; i < 2; i++ )
    {
        if ( i == 0 )
        {
            dr = DRM_LST_InitEnum( &poAppContextInternal->oLicStoreContextXMR,
                                   &poAppContextInternal->KID,
                                    FALSE,
                                   &poAppContextInternal->oLicEnum[ 0 ] );
        }
        else
        {
            dr = DRM_LST_InitEnum( &poAppContextInternal->oLicStoreContextXML,
                                   &poAppContextInternal->KID,
                                    FALSE,
                                   &poAppContextInternal->oLicEnum[ 0 ] );
        }

        if ( DRM_SUCCEEDED( dr ) )
        {
            while ( dr != DRM_E_NOMORE )
            {
                DRM_DST_SLOT_HINT slotHint = { 0 };

                dr = DRM_LST_EnumNext( &poAppContextInternal->oLicEnum[ 0 ],
                                        NULL,
                                       &poAppContextInternal->oFFLicense.rglid[ 0 ],
                                       &slotHint,
                                       &cbLicSize );

                if ( dr != DRM_E_NOMORE )
                {
                    ChkDR( dr );

                    dr = DRM_LST_EnumDelete( &poAppContextInternal->oLicEnum[ 0 ] );

                    if (  DRM_SUCCEEDED( dr )
                       && f_pcLicDeleted != NULL )
                    {
                        ( *f_pcLicDeleted )++;
                    }

                    if( DRM_SNC_IsLicenseSyncSupported() )
                    {
                        ( DRM_VOID )DRM_SNC_DeleteKID( &poAppContextInternal->contextSync,
                                                       &poAppContextInternal->KID );
                    }
                }
            }

        }
    }

ErrorExit:

    if ( dr == DRM_E_NOMORE )
    {
        dr = DRM_SUCCESS;
    }

    ChkECC( ECC_Drm_StoreMgmt_DeleteLicenses, dr );

    DRM_PROFILING_LEAVE_SCOPE

    return dr;
}

/**********************************************************************
**
** Function:    Drm_ClearAllCaches
**
** Synopsis:    This function will clear any globally cached data. It
**              should be called when the DST, devcert, or private key may
**              have changed  (ie if the device is formatted). The caches
**              to be cleared are generally used to improve performance.
**
** Arguments:
**              N/A
**
** Returns:     N/A
**
**********************************************************************/
DRM_API DRM_VOID DRM_CALL Drm_ClearAllCaches( DRM_VOID )
{
    if( DRM_HBHC_IsHdsBlockHeaderCacheSupported() )
    {
        Oem_Hds_ClearBlockHeaderCache( );
    }
}

/**********************************************************************
**
** Function:    Drm_ProcessCommand
**
** Synopsis:    This function processes a operation command received from a
**              host.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_dwOperationCode]      -- The unique identifier for an operation command.
** [f_dwRequestArgument1]   -- A parameter for the command. It is dependent
**                             on the operation code in f_dwOperationCode.
** [f_dwRequestArgument2]   -- A parameter for the command. It is dependent
**                             on the operation code in f_dwOperationCode.
** [f_dwRequestArgument3]   -- A parameter for the command. It is dependent
**                             on the operation code in f_dwOperationCode.
** [f_dwRequestArgument4]   -- A parameter for the command. It is dependent
**                             on the operation code in f_dwOperationCode.
** [f_pbRequestData]        -- Pointer to a buffer that contains an XML string
**                             whose format is dependent on the operation code.
** [f_dwRequestDataLength]  -- Size of the buffer that contains an XML string
**                             whose format is dependent on the operation code.
** [f_pdwResponseResult1]   -- Pointer to a DRM_DWORD corresponding to an output
**                             parameter dependent on the operation code.
** [f_pdwResponseResult2]   -- Pointer to a DRM_DWORD corresponding to an output
**                             parameter dependent on the operation code.
** [f_pdwResponseResult3]   -- Pointer to a DRM_DWORD corresponding to an output
**                             parameter dependent on the operation code.
** [f_pdwResponseResult4]   -- Pointer to a DRM_DWORD corresponding to an output
**                             parameter dependent on the operation code.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_ProcessCommand(
    IN                                   DRM_APP_CONTEXT *f_poAppContext,
    IN                                   DRM_DWORD        f_dwOperationCode,
    IN                                   DRM_DWORD        f_dwRequestArgument1,
    IN                                   DRM_DWORD        f_dwRequestArgument2,
    IN                                   DRM_DWORD        f_dwRequestArgument3,
    IN                                   DRM_DWORD        f_dwRequestArgument4,
    __in_bcount( f_dwRequestDataLength ) DRM_BYTE        *f_pbRequestData,
    IN                                   DRM_DWORD        f_dwRequestDataLength,
    OUT                                  DRM_DWORD       *f_pdwResponseResult1,
    OUT                                  DRM_DWORD       *f_pdwResponseResult2,
    OUT                                  DRM_DWORD       *f_pdwResponseResult3,
    OUT                                  DRM_DWORD       *f_pdwResponseResult4 )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;

    ChkArg( f_poAppContext != NULL );

    switch ( f_dwOperationCode )
    {
        case OpGetVersion:
           ChkArg(  f_pdwResponseResult1 != NULL
                 && f_pdwResponseResult2 != NULL );

            *f_pdwResponseResult1 = g_dwWMDRMPDVersionMajor;
            *f_pdwResponseResult2 = g_dwWMDRMPDVersionMinor;
            *f_pdwResponseResult3 = g_dwWMDRMPDVersionBuild;
            *f_pdwResponseResult4 = g_dwWMDRMPDVersionRevision;

            dr = DRM_SUCCESS;

            break;

        case OpProcessLicenseRevocationList:
            ChkDR( _ProcessLicenseRevocationList( poAppContextInternal,
                                                  f_dwRequestArgument1,
                                                  f_dwRequestArgument2,
                                                  f_pbRequestData,
                                                  f_dwRequestDataLength,
                                                  f_pdwResponseResult1,
                                                  f_pdwResponseResult2 ) );
            break;

        default:
            ChkDR( DRM_E_INVALIDARG );
    }

ErrorExit:

    ChkECC( ECC_Drm_ProcessCommand, dr );

    return dr;
}

/**********************************************************************
**
** Function:    Drm_ProcessCommand
**
** Synopsis:    This function processes a request for data received from a
**              host.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_dwOperationCode]      -- The unique identifier for an operation command.
** [f_dwRequestArgument1]   -- A parameter for the command. It is dependent
**                             on the operation code in f_dwOperationCode.
** [f_dwRequestArgument2]   -- A parameter for the command. It is dependent
**                             on the operation code in f_dwOperationCode.
** [f_dwRequestArgument3]   -- A parameter for the command. It is dependent
**                             on the operation code in f_dwOperationCode.
** [f_dwRequestArgument4]   -- A parameter for the command. It is dependent
**                             on the operation code in f_dwOperationCode.
** [f_pdwResponseResult1]   -- Pointer to a DRM_DWORD corresponding to an output
**                             parameter dependent on the operation code.
** [f_pdwResponseResult2]   -- Pointer to a DRM_DWORD corresponding to an output
**                             parameter dependent on the operation code.
** [f_pdwResponseResult3]   -- Pointer to a DRM_DWORD corresponding to an output
**                             parameter dependent on the operation code.
** [f_pdwResponseResult4]   -- Pointer to a DRM_DWORD corresponding to an output
**                             parameter dependent on the operation code.
** [f_pbResponseData]       -- Pointer to a buffer that contains the requested
**                             data, which is dependent upon the operation code.
** [f_pdwResponseDataLength]-- Pointer to a DRM_DWORD containing a count of
**                             bytes in f_pbResponseData.
**
** Returns:                 DRM_E_NOTIMPL which means it is not implemented.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_ProcessRequest(
    IN                                       DRM_APP_CONTEXT *f_poAppContext,
    IN                                       DRM_DWORD        f_dwOperationCode,
    IN                                       DRM_DWORD        f_dwRequestArgument1,
    IN                                       DRM_DWORD        f_dwRequestArgument2,
    IN                                       DRM_DWORD        f_dwRequestArgument3,
    IN                                       DRM_DWORD        f_dwRequestArgument4,
    OUT                                      DRM_DWORD       *f_pdwResponseResult1,
    OUT                                      DRM_DWORD       *f_pdwResponseResult2,
    OUT                                      DRM_DWORD       *f_pdwResponseResult3,
    OUT                                      DRM_DWORD       *f_pdwResponseResult4,
    __out_bcount( *f_pdwResponseDataLength ) DRM_BYTE        *f_pbResponseData,
    OUT                                      DRM_DWORD       *f_pdwResponseDataLength )
{
    DRM_RESULT dr = DRM_E_NOTIMPL;

    ChkECC( ECC_Drm_ProcessRequest, dr );

    return dr;
}

/**********************************************************************
**
** Function:    _LoadEnvelopeHeader
**
** Synopsis:    Loads the header data from an envelope file into a
**              DRM_ENVELOPE_HEADER structure.
**              The seek pointer will be updated to the end of the header
**              data.
**
** Arguments:
**
**      [f_oFileHandle]      - Open file handle to the envelope file
**      [f_poEnvelopeHeader] - Returns the initialized envelope header data.
**
**********************************************************************/
static DRM_RESULT _LoadEnvelopeHeader(
    __inout OEM_FILEHDL           f_oFileHandle,
    __out   DRM_ENVELOPE_HEADER  *f_poEnvelopeHeader )
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_DWORD  cbRead = 0;
    /* Temporary buffer to read the envelope header into. Due to slack in the struct, the
    ** actual envelope header could potentially be smaller than SIZEOF( DRM_ENVELOPE_HEADER )
    ** but should never be larger
    */
    DRM_BYTE rgbHeaderBuffer[SIZEOF( DRM_ENVELOPE_HEADER )] = {0};

    /*
    ** Seek to the beginning of the file.
    */
    ChkBOOL( Oem_File_SetFilePointer( f_oFileHandle,
                                      0,
                                      OEM_FILE_BEGIN,
                                      NULL ), DRM_E_FILESEEKERROR );

    ChkBOOL( Oem_File_Read( f_oFileHandle,
                            rgbHeaderBuffer,
                            DRM_ENVELOPE_MINIMUM_HEADER_SIZE,
                           &cbRead ), DRM_E_FILEREADERROR );

    ChkBOOL( cbRead == DRM_ENVELOPE_MINIMUM_HEADER_SIZE, DRM_E_FILEREADERROR );

    /*
    ** Load the header from the byte buffer to the struct. Data isn't read directly into the
    ** struct with ReadFile due to the potential for different packing on varying compilers
    */
    cbRead = 0;
    LITTLEENDIAN_BYTES_TO_DWORD( f_poEnvelopeHeader->dwFileSignature, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_DWORD );

    LITTLEENDIAN_BYTES_TO_DWORD( f_poEnvelopeHeader->cbHeaderSize, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_DWORD );

    LITTLEENDIAN_BYTES_TO_DWORD( f_poEnvelopeHeader->dwFileDataOffset, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_DWORD );

    LITTLEENDIAN_BYTES_TO_WORD( f_poEnvelopeHeader->wFormatVersion, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_WORD );

    LITTLEENDIAN_BYTES_TO_WORD( f_poEnvelopeHeader->wCompatibleVersion, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_WORD );

    LITTLEENDIAN_BYTES_TO_DWORD( f_poEnvelopeHeader->dwCipherType, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_DWORD );

    DRM_BYT_CopyBytes( f_poEnvelopeHeader->rgbCipherData, 0, rgbHeaderBuffer, cbRead, DRM_ENVELOPE_CIPHER_DATA_SIZE );
    cbRead += DRM_ENVELOPE_CIPHER_DATA_SIZE;

    LITTLEENDIAN_BYTES_TO_WORD( f_poEnvelopeHeader->cbOriginalFilename, rgbHeaderBuffer, cbRead );
    cbRead += SIZEOF( DRM_WORD );

    LITTLEENDIAN_BYTES_TO_DWORD( f_poEnvelopeHeader->cbDrmHeaderLen, rgbHeaderBuffer, cbRead );

ErrorExit:
    return dr;
}

/*****************************************************************************
**
** Function:    Drm_Envelope_Open
**
** Synopsis:    Opens a file within the envelope archive.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_pOEMContext]          -- Optional pointer to OEM specific data.
** [f_pwszFilename]         -- Filename to open.
** [f_pEnvFile]             -- Returns an initialize enveloped file context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Envelope_Open(
    IN           DRM_APP_CONTEXT            *f_poAppContext,
    IN           DRM_VOID                   *f_pOEMContext,
    __in_z const DRM_WCHAR                  *f_pwszFilename,
    OUT          DRM_ENVELOPED_FILE_CONTEXT *f_pEnvFile )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_DWORD                 cbRead               = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Envelope_Open );

    ChkArg( f_pwszFilename != NULL );
    ChkArg( f_pEnvFile != NULL );
    ChkArg( f_poAppContext != NULL );

    ZEROMEM( f_pEnvFile, SIZEOF( DRM_ENVELOPED_FILE_CONTEXT ) );

    f_pEnvFile->pEnvFileHandle = Oem_File_Open( f_pOEMContext,
                                                f_pwszFilename,
                                                OEM_GENERIC_READ,
                                                OEM_FILE_SHARE_READ,
                                                OEM_OPEN_EXISTING,
                                                OEM_ATTRIBUTE_NORMAL );

    ChkBOOL( f_pEnvFile->pEnvFileHandle != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN );

    /*
    ** Read the file header.
    */
    ChkDR( _LoadEnvelopeHeader( f_pEnvFile->pEnvFileHandle, &f_pEnvFile->oEnvHeader ) );

    /*
    ** Check the file signature, make sure reported header size is sane, and check that the version is OK.
    */
    ChkBOOL( f_pEnvFile->oEnvHeader.dwFileSignature == DRM_ENVELOPE_FILE_SIGNATURE, DRM_E_ENVELOPE_CORRUPT );
    ChkBOOL( f_pEnvFile->oEnvHeader.cbHeaderSize >= DRM_ENVELOPE_MINIMUM_HEADER_SIZE, DRM_E_ENVELOPE_CORRUPT );
    ChkBOOL( f_pEnvFile->oEnvHeader.wCompatibleVersion <= DRM_ENVELOPE_CURRENT_FORMAT_VERSION, DRM_E_ENVELOPE_FILE_NOT_COMPATIBLE );
    ChkBOOL( f_pEnvFile->oEnvHeader.dwFileDataOffset >= f_pEnvFile->oEnvHeader.cbHeaderSize, DRM_E_ENVELOPE_CORRUPT );

    /*
    ** Check that the header length is valid.
    */
    ChkBOOL( f_pEnvFile->oEnvHeader.cbDrmHeaderLen <= poAppContextInternal->cbDRMHeaderData, DRM_E_BUFFERTOOSMALL );
    ChkBOOL( f_pEnvFile->oEnvHeader.cbDrmHeaderLen % SIZEOF( DRM_WCHAR ) == 0, DRM_E_ENVELOPE_CORRUPT );

    /*
    ** Skip over the original filename.
    */
    ChkBOOL( Oem_File_SetFilePointer( f_pEnvFile->pEnvFileHandle,
                                      f_pEnvFile->oEnvHeader.cbOriginalFilename,
                                      OEM_FILE_CURRENT,
                                      NULL ), DRM_E_FILESEEKERROR );


    /*
    ** Read the RM content header itself.
    */
    ChkBOOL( Oem_File_Read(  f_pEnvFile->pEnvFileHandle,
                             poAppContextInternal->pbDRMHeaderData,
                             f_pEnvFile->oEnvHeader.cbDrmHeaderLen,
                            &cbRead ), DRM_E_FILEREADERROR );

    ChkBOOL( f_pEnvFile->oEnvHeader.cbDrmHeaderLen == cbRead, DRM_E_FILEREADERROR );

    poAppContextInternal->eHeaderInContext = DRM_CSP_HEADER_NOT_SET;
    ChkDRMap( Drm_Content_SetProperty( f_poAppContext,
                               DRM_CSP_AUTODETECT_HEADER,
                               poAppContextInternal->pbDRMHeaderData,
                               f_pEnvFile->oEnvHeader.cbDrmHeaderLen ), DRM_E_INVALIDARG, DRM_E_CH_INVALID_HEADER );

    /*
    ** Parse the cipher intiailization values.
    */
    switch ( f_pEnvFile->oEnvHeader.dwCipherType )
    {
        case eDRM_AES_COUNTER_CIPHER:
            /* Copy the initial AES counter */
            MEMCPY( ( DRM_BYTE * )&f_pEnvFile->qwInitialCipherCounter,
                    f_pEnvFile->oEnvHeader.rgbCipherData,
                    SIZEOF( DRM_UINT64 ) );

            FIX_ENDIAN_QWORD( f_pEnvFile->qwInitialCipherCounter );
            break;

        case eDRM_RC4_CIPHER:
            break;

        default:
            ChkDR( DRM_E_ENVELOPE_FILE_NOT_COMPATIBLE );
    }

    f_pEnvFile->dwFileDataStart = f_pEnvFile->oEnvHeader.dwFileDataOffset;

    /*
    ** Seek to the beginning of the file data.
    */
    if ( f_pEnvFile->dwFileDataStart > f_pEnvFile->oEnvHeader.cbHeaderSize )
    {
        ChkBOOL( Oem_File_SetFilePointer( f_pEnvFile->pEnvFileHandle,
                                          f_pEnvFile->dwFileDataStart,
                                          OEM_FILE_BEGIN, NULL ), DRM_E_FILESEEKERROR );
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    ChkECC( ECC_Drm_Envelope_Open, dr );

    return dr;
}

/*****************************************************************************
**
** Function:    Drm_Envelope_WritePlayReadyObject
**
** Synopsis:    Re-writes the PlayReady Object back to an open envelope file.
**
** Arguments:
**
** [f_poAppContext]         -- Opaque DRM Application context initialized
**                             by a call to Drm_Initialize().
** [f_pOEMContext]          -- Optional pointer to OEM specific data.
** [f_pwszFilename]         -- Filename to open.
** [f_pEnvFile]             -- The initialized enveloped file context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
** Notes:                   If DRM_SUPPORT_LOCKING is not enabled and there
**                          are other open envelope contexts the FILE I/O behavior
**                          may have bad results. Be careful when using this function
**                          if file locking is not supported.
**
*****************************************************************************/
DRM_RESULT DRM_API DRM_CALL Drm_Envelope_WritePlayReadyObject(
    __in         DRM_APP_CONTEXT            *f_poAppContext,
    __inout_opt  DRM_VOID                   *f_pOEMContext,
    __in_z const DRM_WCHAR                  *f_pwszFilename,
    __inout      DRM_ENVELOPED_FILE_CONTEXT *f_pEnvFile )
{
    DRM_RESULT                dr                   = DRM_SUCCESS;
    DRM_ENVELOPE_HEADER       oCompareHeader       = { 0 };
    DRM_APP_CONTEXT_INTERNAL *poAppContextInternal = ( DRM_APP_CONTEXT_INTERNAL * )f_poAppContext;
    DRM_DWORD                 cbWritten            = 0;
    DRM_BOOL                  fReadPtrOpen         = TRUE;
    DRM_BOOL                  fWritePtrOpen        = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Envelope_WritePlayReadyObject );

    ChkArg( f_pwszFilename != NULL );
    ChkArg( f_pEnvFile != NULL );
    ChkArg( f_poAppContext != NULL );

    ChkBOOL( poAppContextInternal->eHeaderInContext != DRM_CSP_HEADER_NOT_SET, DRM_E_HEADER_NOT_SET );
    ChkBOOL( poAppContextInternal->eHeaderInContext == DRM_CSP_PLAYREADY_OBJ,  DRM_E_CH_INCOMPATIBLE_HEADER_TYPE );

    /*
    ** Close the read handle to the envelope file and reopen for write access
    */
    fReadPtrOpen = FALSE;
    ChkDR( Drm_Envelope_Close( f_pEnvFile ) );

    /*
    ** Open a handle to the envelope file for read/write access
    */
    f_pEnvFile->pEnvFileHandle = Oem_File_Open( f_pOEMContext,
                                                f_pwszFilename,
                                                OEM_GENERIC_READ | OEM_GENERIC_WRITE,
                                                OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
                                                OEM_OPEN_EXISTING,
                                                OEM_ATTRIBUTE_NORMAL );
    ChkBOOL( f_pEnvFile->pEnvFileHandle != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN );
    fWritePtrOpen = TRUE;

    /*
    ** Read the file header
    */
    ChkDR( _LoadEnvelopeHeader( f_pEnvFile->pEnvFileHandle,
                                &oCompareHeader ) );

    /*
    ** Compare header values to expected
    */
    ChkBOOL( MEMCMP( &oCompareHeader, &f_pEnvFile->oEnvHeader, SIZEOF( DRM_ENVELOPE_HEADER ) ) == 0,
             DRM_E_ENVELOPE_CORRUPT );

    /*
    ** Skip over the original filename.
    */
    ChkBOOL( Oem_File_SetFilePointer( f_pEnvFile->pEnvFileHandle,
                                      oCompareHeader.cbOriginalFilename,
                                      OEM_FILE_CURRENT,
                                      NULL ), DRM_E_FILESEEKERROR );

    /*
    ** Write over the PlayReady Object.
    */
    ChkBOOL( poAppContextInternal->cbPROData == oCompareHeader.cbDrmHeaderLen, DRM_E_FILEREADERROR );
    ChkBOOL( Oem_File_Write( f_pEnvFile->pEnvFileHandle,
                             poAppContextInternal->pbDRMHeaderData,
                             poAppContextInternal->cbPROData,
                            &cbWritten ), DRM_E_FILEWRITEERROR );
    ChkBOOL( cbWritten == poAppContextInternal->cbPROData, DRM_E_FILEWRITEERROR );

    /*
    ** Close the write pointer and reopen the envelope
    */
    fWritePtrOpen = FALSE;
    ChkDR( Oem_File_Close( f_pEnvFile->pEnvFileHandle ) );

    fReadPtrOpen = TRUE;
    ChkDR( Drm_Envelope_Open( f_poAppContext, f_pOEMContext, f_pwszFilename, f_pEnvFile ) );

    /*
    ** Compare header values to expected
    */
    ChkBOOL( MEMCMP( &oCompareHeader, &f_pEnvFile->oEnvHeader, SIZEOF( DRM_ENVELOPE_HEADER ) ) == 0,
             DRM_E_ENVELOPE_CORRUPT );

ErrorExit:
    /*
    ** On failure: Attempt to close any open write handles and reopen the envelope file for read
    */
    if ( DRM_FAILED( dr ) && f_pEnvFile != NULL )
    {
        if ( fWritePtrOpen )
        {
            Oem_File_Close( f_pEnvFile->pEnvFileHandle );
        }
        if ( !fReadPtrOpen )
        {
            Drm_Envelope_Open( f_poAppContext, f_pOEMContext, f_pwszFilename, f_pEnvFile );
        }
    }

    DRM_PROFILING_LEAVE_SCOPE;
    ChkECC( ECC_Drm_Envelope_WritePlayReadyObject, dr );

    return dr;
}

/*****************************************************************************
**
** Function:    Drm_Envelope_DuplicateFileContext
**
** Synopsis:    This function allows an application to duplicate an envelope
**              file handle. This may be useful if the application is
**              concurrently accessing multiple seperate parts of the file
**              at the same time.
**
** Arguments:
**
** [f_pOEMContext]          -- Optional pointer to OEM specific data.
** [f_pwszFilename]         -- Filename to re-open.
** [f_pSourceHandle]        -- Pointer to an already initialized envelope
**                             file context.
** [f_pNewHandle]           -- Returns a new duplicate enveloped file context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Envelope_DuplicateFileContext(
    IN           DRM_VOID                   *f_pOEMContext,
    __in_z const DRM_WCHAR                  *f_pwszFilename,
    IN           DRM_ENVELOPED_FILE_CONTEXT *f_pSourceHandle,
    OUT          DRM_ENVELOPED_FILE_CONTEXT *f_pNewHandle )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pwszFilename != NULL );
    ChkArg( f_pSourceHandle != NULL );
    ChkArg( f_pNewHandle != NULL );
    ChkArg( f_pSourceHandle->pEnvFileHandle != OEM_INVALID_HANDLE_VALUE);
    ChkArg( f_pSourceHandle->dwFileDataStart != 0 );

    /* Copy the context data. */
    MEMCPY( f_pNewHandle, f_pSourceHandle, SIZEOF( DRM_ENVELOPED_FILE_CONTEXT ) );

    /* Open a new OS file handle. */
    f_pNewHandle->pEnvFileHandle = Oem_File_Open( f_pOEMContext,
                                                  f_pwszFilename,
                                                  OEM_GENERIC_READ,
                                                  OEM_FILE_SHARE_READ,
                                                  OEM_OPEN_EXISTING,
                                                  OEM_ATTRIBUTE_NORMAL );

    ChkBOOL( f_pNewHandle->pEnvFileHandle != OEM_INVALID_HANDLE_VALUE, DRM_E_FILEOPEN );

    /* Seek over the header data. */
    ChkBOOL( Oem_File_SetFilePointer( f_pNewHandle->pEnvFileHandle,
                                      f_pNewHandle->dwFileDataStart,
                                      OEM_FILE_BEGIN,
                                      NULL ), DRM_E_FILESEEKERROR );

ErrorExit:

    ChkECC( ECC_Drm_Envelope_DuplicateFileContext, dr );

    return dr;
}

/*****************************************************************************
**
** Function:    Drm_Envelope_InitializeRead
**
** Synopsis:    Specifies the decrypt context for the given enveloped file.
**
** Arguments:
**
** [f_pEnvFile]             -- Initialized enveloped file context.
** [f_pDecrypt]             -- An initialized decrypt context, or NULL to clear the
**                             currently associated context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any return code as defined in drmresults.h.
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Envelope_InitializeRead(
    IN DRM_ENVELOPED_FILE_CONTEXT *f_pEnvFile,
    IN DRM_DECRYPT_CONTEXT        *f_pDecrypt )
{
    DRM_RESULT dr                                       = DRM_SUCCESS;
    DRM_BYTE   rgbLast15[ DRM_CPHR_CB_FOR_INITDECRYPT ] = { 0 };
    DRM_DWORD  dwFileSize                               = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Envelope_InitializeRead );

    ChkArg( f_pEnvFile != NULL );
    ChkArg( f_pDecrypt != NULL );
    ChkArg( f_pEnvFile->pEnvFileHandle != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_pEnvFile->dwFileDataStart != 0 );

    MEMCPY( ( DRM_BYTE * )&f_pEnvFile->oDecrypt, ( DRM_BYTE * )f_pDecrypt, SIZEOF( DRM_CIPHER_CONTEXT ) );

    ChkDR( Drm_Envelope_GetSize( f_pEnvFile, &dwFileSize ) );

    /*
    ** For RC4 we need to grab the last 15 bytes.
    */
    if ( f_pEnvFile->oDecrypt.eCipherType == eDRM_RC4_CIPHER )
    {
        DRM_DWORD cbLenToRead      = DRM_CPHR_CB_FOR_INITDECRYPT;
        DRM_DWORD cbRead           = 0;
        DRM_DWORD dwOldSeekPointer = 0;

        if ( dwFileSize < cbLenToRead )
        {
            cbLenToRead = dwFileSize;
        }

        /* Save the seek pointer. */
        ChkBOOL( Oem_File_SetFilePointer(  f_pEnvFile->pEnvFileHandle,
                                           0,
                                           OEM_FILE_CURRENT,
                                          &dwOldSeekPointer ), DRM_E_FILESEEKERROR );

        /* Go to the end and grab the last 15 bytes. */
        ChkBOOL( Oem_File_SetFilePointer( f_pEnvFile->pEnvFileHandle,
                                          -DRM_CPHR_CB_FOR_INITDECRYPT,
                                          OEM_FILE_END,
                                          NULL ), DRM_E_FILESEEKERROR );

        ChkBOOL( Oem_File_Read(  f_pEnvFile->pEnvFileHandle,
                                 rgbLast15,
                                 cbLenToRead,
                                &cbRead ), DRM_E_FILEREADERROR );

        ChkBOOL( cbLenToRead == cbRead, DRM_E_FILEREADERROR );

        /* Restore the seek pointer. */
        ChkBOOL( Oem_File_SetFilePointer( f_pEnvFile->pEnvFileHandle,
                                          dwOldSeekPointer,
                                          OEM_FILE_BEGIN, NULL ), DRM_E_FILESEEKERROR );
    }
    else
    {
        ChkBOOL( f_pEnvFile->oDecrypt.eCipherType == eDRM_AES_COUNTER_CIPHER,
                 DRM_E_UNSUPPORTEDALGORITHM );
    }

    ChkDR( DRM_BBX_CPHR_InitDecrypt( &f_pEnvFile->oDecrypt, rgbLast15, dwFileSize ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    ChkECC( ECC_Drm_Envelope_InitializeRead, dr );

    return dr;
}

/*****************************************************************************
**
** Function:    Drm_Envelope_Close
**
** Synopsis:    Closes the enveloped file and all associated resources.
**
** Arguments:
**
** [f_pEnvFile]             -- Initialized enveloped file context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Envelope_Close(
    IN DRM_ENVELOPED_FILE_CONTEXT *f_pEnvFile )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Envelope_Close );

    ChkArg( f_pEnvFile != NULL );
    ChkArg( f_pEnvFile->pEnvFileHandle != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_pEnvFile->dwFileDataStart != 0 );

    ChkBOOL( Oem_File_Close( f_pEnvFile->pEnvFileHandle ), DRM_E_FILEOPEN );

    f_pEnvFile->pEnvFileHandle = OEM_INVALID_HANDLE_VALUE;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    ChkECC( ECC_Drm_Envelope_Close, dr );

    return dr;
}

/*****************************************************************************
**
** Function:    Drm_Envelope_GetSize
**
** Synopsis:    Returns the size of the enveloped file.
**
** Arguments:
**
** [f_pEnvFile]             -- Initialized enveloped file context.
** [f_pcbFileSize]          -- Returns the file size in bytes.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Envelope_GetSize(
    IN  DRM_ENVELOPED_FILE_CONTEXT *f_pEnvFile,
    OUT DRM_DWORD                  *f_pcbFileSize )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Envelope_GetSize );

    ChkArg( f_pEnvFile != NULL );
    ChkArg( f_pcbFileSize != NULL );
    ChkArg( f_pEnvFile->pEnvFileHandle != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_pEnvFile->dwFileDataStart != 0 );

    ChkBOOL( Oem_File_GetSize( f_pEnvFile->pEnvFileHandle, f_pcbFileSize ), DRM_E_FILESEEKERROR );

    /* Subtract off the size of our header. */
    ChkBOOL( *f_pcbFileSize >= f_pEnvFile->dwFileDataStart, DRM_E_ENVELOPE_CORRUPT );

    *f_pcbFileSize -= f_pEnvFile->dwFileDataStart;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    ChkECC( ECC_Drm_Envelope_GetSize, dr );

    return dr;
}

/*****************************************************************************
**
** Function:    Drm_Envelope_Read
**
** Synopsis:    Reads data from the enveloped file.
**
** Arguments:
**
** [f_pEnvFile]             -- Initialized enveloped file context.
** [f_pbBuffer]             -- Buffer to receive the file data.
** [f_cbToRead]             -- Number of bytes to read.
** [f_pcbBytesToRead]       -- Number of bytes actually read.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          DRM_E_BUFFERTOOSMALL if *f_pcbBuffer smaller than
**                          required buffer size.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Envelope_Read(
    IN                                                DRM_ENVELOPED_FILE_CONTEXT *f_pEnvFile,
     __out_ecount_part( f_cbToRead, *f_pcbBytesRead ) DRM_BYTE                   *f_pbBuffer,
    IN                                                DRM_DWORD                   f_cbToRead,
    OUT                                               DRM_DWORD                  *f_pcbBytesRead )
{
    DRM_RESULT                    dr          = DRM_SUCCESS;
    DRM_AES_COUNTER_MODE_CONTEXT  ctrContext  = { 0 };
    DRM_AES_COUNTER_MODE_CONTEXT *pCtrContext = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Envelope_Read );

    ChkArg( f_pEnvFile != NULL );
    ChkArg( f_pcbBytesRead != NULL );
    ChkArg( f_pbBuffer != NULL );
    ChkArg( f_pEnvFile->pEnvFileHandle != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_pEnvFile->dwFileDataStart != 0 );

    *f_pcbBytesRead = 0;

    /* For AES counter mode get the file offset. */
    if ( f_pEnvFile->oDecrypt.eCipherType == eDRM_AES_COUNTER_CIPHER )
    {
        DRM_DWORD dwFilePosition = 0;

        ChkBOOL( Oem_File_SetFilePointer(  f_pEnvFile->pEnvFileHandle,
                                           0,
                                           OEM_FILE_CURRENT,
                                          &dwFilePosition ), DRM_E_FILESEEKERROR );

        dwFilePosition -= f_pEnvFile->dwFileDataStart;

        ctrContext.qwBlockOffset          = DRM_UI64( dwFilePosition / DRM_AES_BLOCKLEN );
        ctrContext.bByteOffset            = ( DRM_BYTE )( dwFilePosition % DRM_AES_BLOCKLEN );
        ctrContext.qwInitializationVector = f_pEnvFile->qwInitialCipherCounter;
        pCtrContext                       = &ctrContext;
    }

    ChkBOOL( Oem_File_Read( f_pEnvFile->pEnvFileHandle,
                            f_pbBuffer,
                            f_cbToRead,
                            f_pcbBytesRead ), DRM_E_FILEREADERROR );

    ChkBOOL( *f_pcbBytesRead <= f_cbToRead, DRM_E_FILEREADERROR );

    if ( *f_pcbBytesRead > 0 )
    {
        /* Decrypt it. */
        ChkDR( DRM_BBX_CPHR_Decrypt( &f_pEnvFile->oDecrypt, pCtrContext,*f_pcbBytesRead,  f_pbBuffer ) );
    }

ErrorExit:

    if ( DRM_FAILED( dr ) && f_pcbBytesRead != NULL )
    {
        *f_pcbBytesRead = 0;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    ChkECC( ECC_Drm_Envelope_Read, dr );

    return dr;
}

/*****************************************************************************
**
** Function:    Drm_Envelope_Seek
**
** Synopsis:    Seeks in the enveloped file.
**
** Arguments:
**
** [f_pEnvFile]             -- Initialized enveloped file context.
** [f_lDistanceToMove]      -- Specifies the number of bytes to seek.
** [f_dwMoveMethod]         -- Specifies what the distance to move is relative to.
** [f_pdwNewFilePointer]    -- Optional pointer to receive the new current
**                             location in the file.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          DRM_E_BUFFERTOOSMALL if *f_pcbBuffer smaller than
**                          required buffer size.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Envelope_Seek(
    IN  DRM_ENVELOPED_FILE_CONTEXT *f_pEnvFile,
    IN  DRM_LONG                    f_lDistanceToMove,
    IN  DRM_DWORD                   f_dwMoveMethod,
    OUT DRM_DWORD                  *f_pdwNewFilePointer )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_DRM_APP, PERF_FUNC_Drm_Envelope_Seek );

    ChkArg( f_pEnvFile != NULL );
    ChkArg( f_pEnvFile->pEnvFileHandle != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_pEnvFile->dwFileDataStart != 0 );

    /* Seeking in an RC4 encrypted envelope file is not supported. */
    ChkBOOL( f_pEnvFile->oDecrypt.eCipherType != eDRM_RC4_CIPHER, DRM_E_NOTIMPL );

    /* Add the file header size when moving relative to the start of the file. */
    if ( f_dwMoveMethod == OEM_FILE_BEGIN )
    {
        ChkArg( f_lDistanceToMove >= 0 );

        f_lDistanceToMove += f_pEnvFile->dwFileDataStart;
    }
    else if ( f_dwMoveMethod == OEM_FILE_END )
    {
        ChkArg( f_lDistanceToMove <= 0 );
    }

    ChkBOOL( Oem_File_SetFilePointer( f_pEnvFile->pEnvFileHandle,
                                      f_lDistanceToMove,
                                      f_dwMoveMethod,
                                      f_pdwNewFilePointer ), DRM_E_FILESEEKERROR );

    if ( f_pdwNewFilePointer != NULL )
    {
        *f_pdwNewFilePointer -= f_pEnvFile->dwFileDataStart;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    ChkECC( ECC_Drm_Envelope_Seek, dr );

    return dr;
}

/*****************************************************************************
**
** Function:    Drm_Envelope_GetOriginalFilename
**
** Synopsis:    Returns the original filename of the envelope from its header.
**
** Arguments:
**
** [f_pEnvFile]             -- Initialized enveloped file context.
** [f_pwszOriginalFilename] -- Buffer to receive the original filename.
** [f_pcchOriginalFilename] -- On input provides the size in wide characters
**                             of the buffer, on output returns the number
**                             of characters returned.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**                          DRM_E_BUFFERTOOSMALL if *f_pcbBuffer smaller than
                            required buffer size.
**                          Any other return code as defined in drmresults.h.
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Envelope_GetOriginalFilename(
    IN                                          DRM_ENVELOPED_FILE_CONTEXT *f_pEnvFile,
    __out_ecount_opt( *f_pcchOriginalFilename ) DRM_WCHAR                  *f_pwszOriginalFilename,
    IN OUT                                      DRM_DWORD                  *f_pcchOriginalFilename )
{
    DRM_RESULT dr                    = DRM_SUCCESS;
    DRM_DWORD  dwOriginalFilePointer = 0;
    DRM_DWORD  cbRead                = 0;

    ChkArg( f_pEnvFile != NULL );
    ChkArg( f_pcchOriginalFilename != NULL );
    ChkArg( f_pEnvFile->pEnvFileHandle != OEM_INVALID_HANDLE_VALUE );
    ChkArg( f_pEnvFile->dwFileDataStart != 0 );

    if ( *f_pcchOriginalFilename < ( f_pEnvFile->oEnvHeader.cbOriginalFilename / SIZEOF( DRM_WCHAR ) ) )
    {
        *f_pcchOriginalFilename = f_pEnvFile->oEnvHeader.cbOriginalFilename / SIZEOF( DRM_WCHAR );

        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    ChkArg( f_pwszOriginalFilename != NULL );

    ChkBOOL( Oem_File_SetFilePointer(  f_pEnvFile->pEnvFileHandle,
                                       0,
                                       OEM_FILE_CURRENT,
                                      &dwOriginalFilePointer ), DRM_E_FILESEEKERROR );

    ChkBOOL( Oem_File_SetFilePointer( f_pEnvFile->pEnvFileHandle,
                                      DRM_ENVELOPE_MINIMUM_HEADER_SIZE,
                                      OEM_FILE_BEGIN, NULL ), DRM_E_FILESEEKERROR );

    ChkBOOL( Oem_File_Read(  f_pEnvFile->pEnvFileHandle,
                             f_pwszOriginalFilename,
                             f_pEnvFile->oEnvHeader.cbOriginalFilename,
                            &cbRead ) &&
             cbRead == f_pEnvFile->oEnvHeader.cbOriginalFilename, DRM_E_FILEREADERROR );

    ChkBOOL( Oem_File_SetFilePointer( f_pEnvFile->pEnvFileHandle,
                                      dwOriginalFilePointer,
                                      OEM_FILE_BEGIN, NULL ), DRM_E_FILESEEKERROR );

ErrorExit:

    ChkECC( ECC_Drm_Envelope_GetOriginalFileName, dr );

    return dr;
}

DRM_API DRM_RESULT Drm_Platform_Initialize( DRM_VOID )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_XMLHASH_Init() );
    ChkDR( Oem_Platform_Init() );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT Drm_Platform_Uninitialize( DRM_VOID )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkDR( DRM_XMLHASH_UnInit() );
    ChkDR( Oem_Platform_UnInit() );

ErrorExit:
    return dr;
}

DRM_API DRM_RESULT DRM_CALL Drm_Activation_ProcessResponseGenerateChallenge(
    __inout_bcount( ACTIVATION_CONTEXT_SIZE )  DRM_BYTE        *f_pbActivationCtx,
    __in_bcount_opt( f_cbResponse )            DRM_BYTE        *f_pbResponse,
    __in_opt                                   DRM_DWORD        f_cbResponse,
    __out_bcount_opt( *f_pcbChallenge )        DRM_BYTE        *f_pbChallenge,
    __inout                                    DRM_DWORD       *f_pcbChallenge,
    __out_ecount_opt( *f_pcchUrl )             DRM_CHAR        *f_pszUrl,
    __inout                                    DRM_DWORD       *f_pcchUrl,
    __out                                      DRM_RESULT      *f_pResult )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_ACTIVATION,
                               PERF_FUNC_Drm_Activation_ProcessResponseGenerateChallenge );

    ChkArg( f_pbActivationCtx   != NULL );
    ChkArg( f_pbResponse        != NULL );
    ChkArg( f_cbResponse        != 0    );
    ChkArg( f_pcbChallenge      != NULL );
    ChkArg( f_pcchUrl           != NULL );
    ChkArg( f_pResult           != NULL );
    *f_pResult = DRM_SUCCESS;

    ChkDR( DRM_ACT_ProcessResponseGenerateChallenge(
        f_pbActivationCtx,
        f_pbResponse,
        f_cbResponse,
        f_pbChallenge,
        f_pcbChallenge,
        f_pszUrl,
        f_pcchUrl,
        f_pResult ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

/*****************************************************************************
**
** Function:    Drm_GetContextSizes
**
** Synopsis:    This function returns the sizes of DRM_APP_CONTEXT, the minimum
**              opaque buffer size and the size of the decrypt context
**
** Arguments:
**
** [f_pdwAppContextSize]       -- Returns the Playready App context size
** [f_pdwMinOpaqueBufferSize]  -- Returns the opaque buffer size.
** [f_pdwDecryptContextSize]   -- Returns the size of decrypt context,
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_GetContextSizes(
    __out DRM_DWORD *f_pdwAppContextSize,
    __out DRM_DWORD *f_pdwOpaqueBufferSize,
    __out DRM_DWORD *f_pdwDecryptContextSize)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pdwAppContextSize != NULL );
    ChkArg( f_pdwOpaqueBufferSize != NULL );
    ChkArg( f_pdwDecryptContextSize != NULL );

    *f_pdwAppContextSize     = SIZEOF(DRM_APP_CONTEXT);
    *f_pdwOpaqueBufferSize   = MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE;
    *f_pdwDecryptContextSize = SIZEOF(DRM_DECRYPT_CONTEXT);

ErrorExit:
    return dr;
}


/*****************************************************************************
**
** Function:    Drm_Reader_CloneDecryptContext
**
** Synopsis:    This function clones a decrypt context so that the caller can
**              decrypt multiple streams in parallel or reinitialize a DRM_APP_CONTEXT
**              structure without destroying the decrypt context while content decryption
**              is still in progress.
**              The caller is expected to allocate the memory for f_pDecryptContextOut
**              before calling this function.
**
** Arguments:
**
** [f_pDecryptContextIn]   -- The Playready Decrypt context that needs to be cloned
** [f_pDecryptContextOut]  -- The cloned decrypt context.
** 
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if any parameter is invalid.
**
*****************************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Reader_CloneDecryptContext(
    __in_ecount(1)  DRM_DECRYPT_CONTEXT *f_pDecryptContextIn,
    __out_ecount(1) DRM_DECRYPT_CONTEXT *f_pDecryptContextOut)
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg(f_pDecryptContextIn  != NULL);
    ChkArg(f_pDecryptContextOut != NULL);

    MEMCPY(f_pDecryptContextOut, f_pDecryptContextIn, SIZEOF(DRM_DECRYPT_CONTEXT));
    
ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;
