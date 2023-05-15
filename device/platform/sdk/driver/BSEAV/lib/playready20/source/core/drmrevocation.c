/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmcommon.h>

#include <drmutilities.h>
#include <oempkcrypto.h>
#include <oemcipher.h>
#include <oemsha1.h>
#include <drmrevocation.h>
#include <drmxmlparser.h>
#include <drmcrt.h>
#include <byteorder.h>
#include <drmint64.h>
#include <drmcert.h>
#include <drmbase64.h>
#include <drmcertparser.h>
#include <drmbcertparser.h>
#include <drmbcrl.h>
#include <oem.h>
#include <drmblobcache.h>

/* keypair for device exclusion; the private key is cloaked and must be uncloaked to compile */

#include <drmpubkeydeviceexclusion.h>

/* pubkey for app revocation */

#include <drmpubkeyrevocation.h>

#include <drmapprevocation.h>
#include <drmcontentrevocation.h>
#include <drmdevicerevocation.h>
#include <drmmetercertrevocation.h>
#include <drmwmdrmnet.h>
#include <drmmodelrevocation.h>
#include <drmrevocationconstants.h>

ENTER_PK_NAMESPACE_CODE;

PUBKEY g_pubkeyRevocation =
{
    0x02, 0x14, 0xEF, 0x99, 0xD4, 0x80, 0x0C, 0x96,
    0x5E, 0xB4, 0x57, 0x5C, 0xE7, 0x16, 0x67, 0x60,
    0x40, 0x4E, 0x6D, 0x23, 0x30, 0x42, 0xBB, 0xBA,
    0x68, 0x35, 0x42, 0x30, 0xA1, 0xB6, 0x10, 0x95,
    0x4C, 0x40, 0xF9, 0xAA, 0x10, 0x85, 0xE5, 0x70
};

DRM_API DRM_NO_INLINE DRM_BOOL DRM_CALL DRM_REVOCATION_IsRevocationSupported(DRM_VOID)
{
    /*
    ** PlayReady revocation can not be disabled.
    ** If at some point PlayReady revocation CAN be disabled,
    ** then TRUE would be replaced with something like
    ** "DRM_PLAYREADYREVOCATION_IsPlayReadyRevocationSupported".
    ** However, there are many places in the code where
    ** DRM_REVOCATION_IsRevocationSupported is used to
    ** mean "DRM_PLAYREADYREVOCATION_IsPlayReadyRevocationSupported"
    ** and other places in the code where a check for revocation
    ** being enabled isn't performed at all.  Thus, creating support
    ** for a PK that does not support PlayReady reovcation is
    ** non-trivial.
    */
    return TRUE
        || DRM_APPREVOCATION_IsAppRevocationSupported()
        || DRM_CONTENTREVOCATION_IsContentRevocationSupported()
        || DRM_DEVICEREVOCATION_IsDeviceRevocationSupported()
        || DRM_METERCERTREVOCATION_IsMeterCertRevocationSupported()
        || DRM_WMDRMNET_IsWmdrmnetSupported()
        || DRM_MODELREVOCATION_IsModelRevocationSupported();
}

DRM_API DRM_BOOL DRM_CALL DRM_REVOCATION_IsV1RIVSupported(DRM_VOID)
{
    return DRM_APPREVOCATION_IsAppRevocationSupported()
        || DRM_DEVICEREVOCATION_IsDeviceRevocationSupported()
        || DRM_WMDRMNET_IsWmdrmnetSupported();
}

DRM_API DRM_BOOL DRM_CALL DRM_REVOCATION_AreAppAndDeviceRevocationSupported(DRM_VOID)
{
    return DRM_APPREVOCATION_IsAppRevocationSupported()
        && DRM_DEVICEREVOCATION_IsDeviceRevocationSupported();
}

DRM_API DRM_BOOL DRM_CALL DRM_REVOCATION_AreAppAndWmdrmnetRevocationSupported(DRM_VOID)
{
    return DRM_APPREVOCATION_IsAppRevocationSupported()
        && DRM_WMDRMNET_IsWmdrmnetSupported();
}

DRM_API DRM_BOOL DRM_CALL DRM_REVOCATION_AreDeviceAndWmdrmnetRevocationSupported(DRM_VOID)
{
    return DRM_DEVICEREVOCATION_IsDeviceRevocationSupported()
        && DRM_WMDRMNET_IsWmdrmnetSupported();
}

DRM_API DRM_BOOL DRM_CALL DRM_REVOCATION_AreAppAndDeviceAndWmdrmnetRevocationSupported(DRM_VOID)
{
    return DRM_APPREVOCATION_IsAppRevocationSupported()
        && DRM_DEVICEREVOCATION_IsDeviceRevocationSupported()
        && DRM_WMDRMNET_IsWmdrmnetSupported();
}

static DRM_NO_INLINE DRM_RESULT _GetPlayReadyRevocationList(
    __in                                           DRM_CRYPTO_CONTEXT          *f_pcontextCRYP,
    __in                                           DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in                                     const DRM_GUID                    *f_pguidRevocationType,
    __out_bcount_opt( *f_pcbRevocationData ) const DRM_BYTE                    *f_pbRevocationData,
    __inout                                        DRM_DWORD                   *f_pcbRevocationData,
    __out                                          DRM_DWORD                   *f_pdwVersion) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _GetPlayReadyRevocationList(
    __in                                           DRM_CRYPTO_CONTEXT          *f_pcontextCRYP,
    __in                                           DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in                                     const DRM_GUID                    *f_pguidRevocationType,
    __out_bcount_opt( *f_pcbRevocationData ) const DRM_BYTE                    *f_pbRevocationData,
    __inout                                        DRM_DWORD                   *f_pcbRevocationData,
    __out                                          DRM_DWORD                   *f_pdwVersion)
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT       dr          = DRM_SUCCESS;
    DRM_BYTE        *pbAligned   = NULL;
    DRM_DWORD        cbAligned   = 0;
    DRM_BCRL_Signed  oBCRLSigned = { 0 };
    _RevocationStoreContext *pRevStore = (_RevocationStoreContext*)f_pContextRev;

    ChkArg( f_pcontextCRYP      != NULL
         && f_pContextRev       != NULL
         && f_pcbRevocationData != NULL
         && f_pdwVersion        != NULL );

    *f_pdwVersion = 0;

    /*
        The call to DRM_UTL_EnsureDataAlignment
        re-adjust alightment of buffer and may place revocation
        information with offset to f_pcbRevocationData
        The calling code would need to detect the same condition.
        It would be more reasonable to request the f_pcbRevocationData
        to be already aligned buffer
        and return error if buffer is not aligned.
        Currently we use the same logic as appears througth the code.
    */

    if (f_pbRevocationData != NULL)
    {
        ChkDR( DRM_UTL_EnsureDataAlignment(
            f_pbRevocationData,
            *f_pcbRevocationData,
            &pbAligned,
            &cbAligned,
            SIZEOF (DRM_WCHAR),
            NULL ) );

        DRMASSERT(pbAligned != NULL);
    }

    dr = DRM_RVS_GetRevocationData(
        f_pContextRev,
        f_pguidRevocationType,
        pbAligned,
        &cbAligned );

    if ( DRM_FAILED( dr ) )
    {
        if ( DRM_E_DSTSLOTNOTFOUND == dr )
        {
            cbAligned = 0;
            dr = DRM_SUCCESS;
        }
        goto ErrorExit;
    }

    AssertChkArg( pbAligned != NULL );

    ChkDR( DRM_BCrl_ParseCrl(
        pbAligned,
        cbAligned,
        &oBCRLSigned,
        (const PUBKEY_P256 *) &g_rgbMSPlayReadyRootIssuerPubKey,
        f_pcontextCRYP, pRevStore->pContextBB, pRevStore->pContextSST, pRevStore->pDatastore ) );

    *f_pdwVersion = oBCRLSigned.Crl.dwVersion;

ErrorExit:
    if( f_pcbRevocationData != NULL )
    {
        *f_pcbRevocationData = cbAligned;
    }
    return dr;
}


/******************************************************************************
**
** Function :   DRM_RVK_UpdateRevocationVersionsCache
**
** Synopsis :   Loads the revinfo and CRL versions from the secure store
**              and caches them in the DRM_APP_CONTEXT for use when
**              evaluting licenses with revocation requirements
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_UpdateRevocationVersionsCache(
 __inout                            DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
 __in                               DRM_CRYPTO_CONTEXT          *f_pContextCrypto,
 __inout                            DRM_REVLIST_ID_VALUES       *f_pRevListIDs,
 __out_bcount(f_cbRevocationBuffer) DRM_BYTE                    *f_pbRevocationBuffer,
 __in                               DRM_DWORD                    f_cbRevocationBuffer,
 __out                              DRM_BOOL                    *f_pfUpdated)
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT       dr             = DRM_SUCCESS;
    DRM_DWORD        cbBuffer       = 0;
    DRM_RLVI         RLVI           = {0};
    DRM_RLVI         RLVI2          = {0};
    DRM_DWORD        dwVersion      = 0;
    DRM_BOOL         fUpdated       = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_UpdateRevocationVersionsCache );

    ChkArg( f_pContextRev != NULL
        &&  f_pRevListIDs != NULL
        &&  f_pbRevocationBuffer != NULL);

    cbBuffer = f_cbRevocationBuffer;

    /* Update the revocation info version V1 */
    ChkDR( DRM_RVK_GetCurrentRevocationInfo( f_pContextRev,
                                             &g_guidRevocationTypeRevInfo,
                                             f_pbRevocationBuffer,
                                             &cbBuffer,
                                             &RLVI,
                                             f_pContextCrypto ) );

    if( f_pRevListIDs->riv != RLVI.head.dwRIV )
    {
        fUpdated = TRUE;
        f_pRevListIDs->riv = RLVI.head.dwRIV;
    }

    /* Update the revocation info version V2 */
    cbBuffer = f_cbRevocationBuffer;
    ChkDR( DRM_RVK_GetCurrentRevocationInfo( f_pContextRev,
                                             &g_guidRevocationTypeRevInfo2,
                                             f_pbRevocationBuffer,
                                             &cbBuffer,
                                             &RLVI2,
                                             f_pContextCrypto ) );

    if( f_pRevListIDs->riv2 != RLVI2.head.dwRIV )
    {
        fUpdated = TRUE;
        f_pRevListIDs->riv2 = RLVI2.head.dwRIV;
    }

    if( DRM_DEVICEREVOCATION_IsDeviceRevocationSupported() )
    {
        /* Update the WMDRM device CRL version */
        dwVersion = 0;
        cbBuffer = f_cbRevocationBuffer;
        ChkDR( DRM_RVK_GetDeviceRevocationList( f_pContextCrypto,
                                                f_pContextRev,
                                                f_pbRevocationBuffer,
                                                &cbBuffer,
                                                &dwVersion) );


        if( f_pRevListIDs->device != dwVersion )
        {
            fUpdated = TRUE;
            f_pRevListIDs->device = dwVersion;
        }
    }

    if( DRM_APPREVOCATION_IsAppRevocationSupported() )
    {
        /* Get the playready app crl */
        dwVersion = 0;
        cbBuffer = f_cbRevocationBuffer;
        ChkDR(_GetPlayReadyRevocationList( f_pContextCrypto,
                                           f_pContextRev,
                                           &g_guidRevocationTypePlayReadySilverLightApplication,
                                           f_pbRevocationBuffer,
                                           &cbBuffer,
                                           &dwVersion ) );
        if (f_pRevListIDs->playreadyapp != dwVersion)
        {
            fUpdated = TRUE;
            f_pRevListIDs->playreadyapp = dwVersion;
        }

        /* Get the wmdrm app crl */
        dwVersion = 0;
        cbBuffer = f_cbRevocationBuffer;
        ChkDR( DRM_RVK_GetCurrentAppRevocationList( f_pContextRev,
                                                    f_pbRevocationBuffer,
                                                    &cbBuffer,
                                                    &dwVersion ) );

        if (f_pRevListIDs->wmdrmapp != dwVersion)
        {
            fUpdated = TRUE;
            f_pRevListIDs->wmdrmapp = dwVersion;
        }
    }

    dwVersion = 0;
    cbBuffer = f_cbRevocationBuffer;
    ChkDR(_GetPlayReadyRevocationList( f_pContextCrypto,
                                       f_pContextRev,
                                       &g_guidRevocationTypePlayReadySilverLightRuntime,
                                       f_pbRevocationBuffer,
                                       &cbBuffer,
                                       &dwVersion ) );

    if (f_pRevListIDs->runtime != dwVersion)
    {
        fUpdated = TRUE;
        f_pRevListIDs->runtime = dwVersion;
    }

    if( DRM_WMDRMNET_IsWmdrmnetSupported() )
    {
        dwVersion = 0;
        cbBuffer = f_cbRevocationBuffer;
        /* Update the WMDRMNET CRL version */
        ChkDR( DRM_RVK_GetWMDRMNETList( f_pContextCrypto,
                                        f_pContextRev,
                                        f_pbRevocationBuffer,
                                        &cbBuffer,
                                        &dwVersion) );


        if( f_pRevListIDs->wmdrmnet != dwVersion )
        {
            fUpdated = TRUE;
            f_pRevListIDs->wmdrmnet = dwVersion;
        }
    }

ErrorExit:

    if ( fUpdated && ( f_pRevListIDs != NULL ) )
    {
        f_pRevListIDs->fUpdatedRevocationList = TRUE;
    }

    if( f_pfUpdated != NULL )
    {
        *f_pfUpdated = fUpdated;
    }

    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        dr = DRM_E_REVOCATION_BUFFERTOOSMALL;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/*****************************************************************************
** Function: _ExtractRevocationList
**
** Synopsis: calls app revocation operation
**
** [f_pdstrLicenseXML]      -- entire license response XML
** [f_pbBuffer]             -- buffer
** [f_pcbBuffer]            -- pointer to buffer size
** [f_pdstrRevocationType]  -- the revocation attribute type
** [f_pdstrAttributeData]   -- on success contains the substring for the given
**                             revocation type
*****************************************************************************/
static DRM_RESULT _ExtractRevocationList(
    IN       DRM_ANSI_CONST_STRING *f_pdastrLicenseResponse,
    IN const DRM_ANSI_CONST_STRING *f_pdastrRevocationType,
    OUT      DRM_SUBSTRING         *f_pdasstrAttributeData)
{
    DRM_RESULT    dr                    = DRM_SUCCESS;
    DRM_SUBSTRING dasstrLicenseResponse = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrList            = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC__ExtractRevocationList );

    ChkArg(f_pdastrLicenseResponse     != NULL
        && f_pdastrRevocationType       != NULL
        && f_pdasstrAttributeData       != NULL);

    /* get the uppermost license list */

    dasstrLicenseResponse.m_ich = 0;
    dasstrLicenseResponse.m_cch = f_pdastrLicenseResponse->cchString;

    ChkDR( DRM_XML_GetNodeA( f_pdastrLicenseResponse->pszString,
                             &dasstrLicenseResponse,
                             &g_adstrLicenseRespTag,
                             NULL,
                             NULL,
                             0,
                             NULL,
                             &dasstrList ) );

    /* get the node for this revocation type */

    ChkDR( DRM_XML_GetNodeA( f_pdastrLicenseResponse->pszString,
                             &dasstrList,
                             &g_dastrTagRevocation,
                             &g_dastrAttributeType,
                             f_pdastrRevocationType,
                             0,
                             NULL,
                             f_pdasstrAttributeData ) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*****************************************************************************
** Function: DRM_RVK_IsWrappedCRL
**
** Synopsis: Checks if the CRL is wrapped with <DATA> tag or not. Used to find
**           out if the WMDRMNET CRL is with wrappers.
**
** [f_pdstrRevocationList]  -- Revocation list
*****************************************************************************/
DRM_NO_INLINE DRM_BOOL DRM_RVK_IsWrappedCRL(
    IN DRM_CONST_STRING *f_pdstrRevocationList )
{
    DRM_RESULT       dr       = DRM_SUCCESS;
    DRM_CONST_STRING dstrData = EMPTY_DRM_STRING;

    /*Check if the revocation list contains <DATA> tag */
    dr = DRM_XML_GetNode( f_pdstrRevocationList,
                          &g_dstrTagData,
                          NULL,
                          NULL,
                          0,
                          NULL,
                          &dstrData );
    if ( dr == DRM_E_XMLNOTFOUND )
    {
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

/******************************************************************************
**
** Function :   DRM_RVK_VerifyRevocationList
**
** Synopsis :   Verify signature on the revocation list
**
** Arguments :  f_pContextRev   - Revocation store context (optional)
**              f_pcontextCRYP  - Crypto context
**              f_pdstrList     - Revocation list
**              f_ppubkey       - Public key for verifying signature on
**                                revocation list
**              f_pidSerial     - Serial number on revocation list
**
** Returns :
**
** Notes :
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_VerifyRevocationList(
    __in_opt DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in     DRM_CRYPTO_CONTEXT          *f_pcontextCRYP,
    __in     DRM_CONST_STRING            *f_pdstrList,
    __in const PUBKEY                    *f_ppubkey,
    __out    DRM_DWORD                   *f_pidSerial )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrListData      = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrListSignature = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrNodeHashAlg   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrNodeSignAlg   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDataHashAlg   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDataSignature = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDataSignAlg   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDataIndex     = EMPTY_DRM_STRING;
    _RevocationStoreContext *pRevStoreContextInternal = ( _RevocationStoreContext * )f_pContextRev;
    DRM_BOOL         fVerified         = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_VerifyRevocationList );

    /* verify the signature attributes and get the value */

    ChkDR(DRM_XML_GetNode(f_pdstrList,        &g_dstrTagSignature, NULL, NULL, 0,  NULL,            &dstrListSignature));
    ChkDR(DRM_XML_GetNode(&dstrListSignature, &g_dstrTagHashAlg,   NULL, NULL, 0, &dstrNodeHashAlg,  NULL));
    ChkDR(DRM_XML_GetNode(&dstrListSignature, &g_dstrTagSignAlg,   NULL, NULL, 0, &dstrNodeSignAlg,  NULL));
    ChkDR(DRM_XML_GetNode(&dstrListSignature, &g_dstrTagValue,     NULL, NULL, 0,  NULL,            &dstrDataSignature));

    ChkDR(DRM_XML_GetNodeAttribute(&dstrNodeHashAlg, &g_dstrAttributeType, &dstrDataHashAlg));
    ChkDR(DRM_XML_GetNodeAttribute(&dstrNodeSignAlg, &g_dstrAttributeType, &dstrDataSignAlg));

    ChkFAIL(DRM_UTL_DSTRStringsEqual(&dstrDataHashAlg, &g_dstrSHA));
    ChkFAIL(DRM_UTL_DSTRStringsEqual(&dstrDataSignAlg, &g_dstrMSDRM));

    ChkDR( DRM_BLOBCACHE_Verify( f_pContextRev == NULL ? NULL : pRevStoreContextInternal->pContextBB,
                                 f_pContextRev == NULL ? NULL : pRevStoreContextInternal->pContextSST,
                                 f_pContextRev == NULL ? NULL : pRevStoreContextInternal->pDatastore,
                                 PB_DSTR( f_pdstrList ),
                                 CB_DSTR( f_pdstrList ),
                                 PB_DSTR( &dstrDataSignature ),
                                 CB_DSTR( &dstrDataSignature ),
                                 &fVerified ) );

    if ( fVerified )
    {
        ChkDR( DRM_XML_GetSubNode( f_pdstrList,
                                   &g_dstrTagData,
                                   NULL,
                                   NULL,
                                   0,
                                   NULL,
                                   &dstrListData,
                                   0 ) );
    }
    else
    {
        /* verify the signature against the <DATA> section */

        ChkDR( DRM_XML_GetAndVerifyNode( f_pdstrList,
                                         &g_dstrTagData,
                                         NULL,
                                         NULL,
                                         0,
                                         f_pcontextCRYP,
                                         f_ppubkey,
                                         TRUE,
                                         &dstrDataSignature,
                                         NULL,
                                         &dstrListData ) );

        if( f_pContextRev != NULL )
        {
            ChkDR( DRM_BLOBCACHE_Update( pRevStoreContextInternal->pContextBB,
                                         pRevStoreContextInternal->pContextSST,
                                         pRevStoreContextInternal->pDatastore,
                                         PB_DSTR( f_pdstrList ),
                                         CB_DSTR( f_pdstrList ),
                                         PB_DSTR( &dstrDataSignature ),
                                         CB_DSTR( &dstrDataSignature ) ) );
        }
    }

    /* verification successful; get the index (version) of this list */

    ChkDR( DRM_XML_GetNode( &dstrListData,            /*    <INDEX> */
                            &g_dstrTagIndex,
                            NULL,
                            NULL,
                            0,
                            NULL,
                            &dstrDataIndex ) );

    ChkDR( DRMCRT_wcsntol( dstrDataIndex.pwszString,
                    dstrDataIndex.cchString,
                    (DRM_LONG *) f_pidSerial ) );

    if( DRM_WMDRMNET_IsWmdrmnetSupported()
     && MEMCMP( f_ppubkey, &g_pubkeyWMDRMNDRevocation, SIZEOF( *f_ppubkey ) ) == 0 )
    {
        DRM_CONST_STRING dstrTemplate = EMPTY_DRM_STRING;

        ChkDR( DRM_XML_GetSubNode(  f_pdstrList,
                                   &g_dstrTemplate,
                                    NULL,
                                    NULL,
                                    0,
                                    NULL,
                                   &dstrTemplate,
                                    1 ) );

        /*
        ** If this is a WMDRMNET revocation list use the OEM function to
        ** validate the internal representation of the list
        */

        ChkDR( DRM_RVK_VerifyWMDRMNETRevocationList( f_pContextRev,
                                                     &dstrTemplate,
                                                     f_pidSerial,
                                                     f_pcontextCRYP ) );

    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
** Function :   DRM_RVK_GetSSTRevocationList
**
** Synopsis :   Get and verify revocation list from Secure store
**
** Arguments :
**
** Returns :
**
** Notes :
**
******************************************************************************/
DRM_NO_INLINE DRM_RESULT DRM_RVK_GetSSTRevocationList(
    __in                       const DRM_GUID                    *f_pGUID,
    __in                             DRM_CRYPTO_CONTEXT          *f_pcontextCRYP,
    __in                             DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in                       const PUBKEY                      *f_ppubkey,
    __out_bcount(*f_pcbBuffer) const DRM_BYTE                    *f_pbBuffer,
    __inout                          DRM_DWORD                   *f_pcbBuffer,
    __out                            DRM_DWORD                   *f_pidSerial )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr            = DRM_SUCCESS;
    DRM_BYTE  *pbAligned     = NULL;
    DRM_DWORD  cbAligned     = 0;
    DRM_CONST_STRING dstrXML = EMPTY_DRM_STRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_GetSSTRevocationList );

    ChkArg(f_pGUID         != NULL
       &&  f_pcontextCRYP != NULL
       &&  f_pContextRev  != NULL
       &&  f_ppubkey      != NULL
       &&  f_pcbBuffer    != NULL);

    *f_pidSerial = DRM_NO_PREVIOUS_CRL;

    /* get the current SST image */

    if (f_pbBuffer != NULL)
    {
        ChkDR( DRM_UTL_EnsureDataAlignment( f_pbBuffer,
                                            *f_pcbBuffer,
                                            &pbAligned,
                                            &cbAligned,
                                            SIZEOF (DRM_WCHAR),
                                            NULL ) );
    }

    dr = DRM_RVS_GetRevocationData( f_pContextRev,
                          f_pGUID,
                          pbAligned,
                          &cbAligned );

    *f_pcbBuffer = cbAligned;

    /* includes the DRM_E_BUFFERTOOSMALL case */

    ChkDR(dr);

    DSTR_FROM_PB( &dstrXML, pbAligned, cbAligned );

    /* verify the signature and extract the index of this exclusion list */
    if ( DRM_RVK_IsWrappedCRL( &dstrXML ) )
    {
        ChkDR( DRM_RVK_VerifyRevocationList( f_pContextRev,
                                             f_pcontextCRYP,
                                             &dstrXML,
                                             f_ppubkey,
                                             f_pidSerial ) );
    }
    else
    {
        ChkDR( DRM_RVK_VerifyBinaryWMDRMNETSignature(
            f_pContextRev,
            pbAligned,
            cbAligned,
            f_pidSerial,
            f_pcontextCRYP) );
    }


ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


static DRM_NO_INLINE DRM_RESULT _SetPlayReadyRevocationList(
    __in                                    DRM_CRYPTO_CONTEXT          *f_pcontextCrypto,
    __in                                    DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in                              const DRM_GUID                    *f_pguidRevocationType,
    __in_bcount(f_cbRevocationList)   const DRM_BYTE                    *f_pbRevocationList,
    __in                                    DRM_DWORD                    f_cbRevocationList,
    __in_bcount(f_cbRevocationBuffer) const DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                    DRM_DWORD                    f_cbRevocationBuffer,
    __out_opt                               DRM_BOOL                    *f_pfUpdated,
    __out_opt                               DRM_DWORD                   *f_pdwVersion ) DRM_NO_INLINE_ATTRIBUTE;

static DRM_NO_INLINE DRM_RESULT _SetPlayReadyRevocationList(
    __in                                    DRM_CRYPTO_CONTEXT          *f_pcontextCrypto,
    __in                                    DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in                              const DRM_GUID                    *f_pguidRevocationType,
    __in_bcount(f_cbRevocationList)   const DRM_BYTE                    *f_pbRevocationList,
    __in                                    DRM_DWORD                    f_cbRevocationList,
    __in_bcount(f_cbRevocationBuffer) const DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                    DRM_DWORD                    f_cbRevocationBuffer,
    __out_opt                               DRM_BOOL                    *f_pfUpdated,
    __out_opt                               DRM_DWORD                   *f_pdwVersion )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT      dr              = DRM_SUCCESS;
    DRM_BCRL_Signed oBCRLSigned     = { 0 };
    DRM_DWORD       dwVersionOld    = 0;
    DRM_DWORD       dwCRLBufferSize = f_cbRevocationBuffer;
    _RevocationStoreContext *pRevStore = (_RevocationStoreContext*)f_pContextRev;

    ChkArg(   f_pcontextCrypto     != NULL
           && f_pContextRev        != NULL
           && f_pbRevocationList   != NULL
           && f_pbRevocationBuffer != NULL
           && f_cbRevocationBuffer  > 0
           && f_cbRevocationList    > 0 );

    if( f_pfUpdated != NULL )
    {
        *f_pfUpdated = FALSE;
    }
    if (f_pdwVersion != NULL)
    {
        *f_pdwVersion = 0;
    }

    ChkDR( DRM_BCrl_ParseCrl(
        f_pbRevocationList,
        f_cbRevocationList,
        &oBCRLSigned,
        (const PUBKEY_P256*)g_rgbMSPlayReadyRootIssuerPubKey,
        f_pcontextCrypto, pRevStore->pContextBB, pRevStore->pContextSST, pRevStore->pDatastore));

    if ( DRM_MODELREVOCATION_IsModelRevocationSupported( ) )
    {
        if(  0 == MEMCMP( f_pguidRevocationType, &g_guidRevocationTypePlayReadySilverLightRuntime, SIZEOF( DRM_GUID ) ) )
        {
            /* Check if our model is revoked, we shouldn't store the CRL if we are */
            ChkDR( DRM_MODELREVOCATION_CheckIfPlayReadyModelRevoked( ((_RevocationStoreContext*)f_pContextRev)->pContextBB->pOEMContext,
                                                   &oBCRLSigned.Crl ) );
        }
    }

    dr = _GetPlayReadyRevocationList(f_pcontextCrypto,
                                            f_pContextRev,
                                            f_pguidRevocationType,
                                            f_pbRevocationBuffer,
                                            &dwCRLBufferSize,
                                            &dwVersionOld);
    if ( dr == DRM_E_DSTSLOTNOTFOUND
        || ( ( dwVersionOld < oBCRLSigned.Crl.dwVersion ) && DRM_SUCCEEDED( dr ) ) )
    {
        /* Store the list as a wrapped CRL */
        ChkDR( DRM_RVS_StoreRevocationData( f_pContextRev,
                               f_pguidRevocationType,
                               f_pbRevocationList,
                               f_cbRevocationList ) );
        if( f_pfUpdated != NULL )
        {
            *f_pfUpdated = TRUE;
        }
        if (f_pdwVersion != NULL)
        {
            *f_pdwVersion = oBCRLSigned.Crl.dwVersion;
        }
    }
    else
    {
        ChkDR( dr );

        /* CRL did not get updated, so set it to the old version. */
        if (f_pdwVersion)
        {
            *f_pdwVersion = dwVersionOld;
        }

    }



ErrorExit:
    return dr;

}

typedef struct _tagRevDispatchEntry
{
    DRM_BOOL (DRM_CALL *pfnIsSupported)(DRM_VOID);
    DRM_REVOCATION_TYPE_ENUM     eType;
    const DRM_ANSI_CONST_STRING *pdastrType;
    const DRM_ANSI_CONST_STRING *pdastrTypeGuid;
    const DRM_GUID              *pguid;
    const PUBKEY                *ppubkey;
} REVDISPATCHENTRY;

/* dispatch table for various revocation types */
/* note: remove this conditional and all like it if any revocation types are entered
         feature-independently
         likewise, extend this conditional and all like it if this list is extended with other
         feature-dependent revocation types */

/* if any of revocation types is defined */
REVDISPATCHENTRY g_arevDispatch [] =
{
    { DRM_APPREVOCATION_IsAppRevocationSupported,       DRM_REVOCATION_TYPE_WMDRM_APP,      &g_dastrAppRevocation,          &g_dastrRevocationGuidApp,      &g_guidRevocationTypeApp,       &g_pubkeyRevocation         },
    { DRM_DEVICEREVOCATION_IsDeviceRevocationSupported, DRM_REVOCATION_TYPE_WMDRM_DEVICE,   &g_dastrDeviceRevocation,       &g_dastrRevocationGuidDevice,   &g_guidRevocationTypeDevice,    &g_pubkeyDeviceRevocation   },
    { DRM_WMDRMNET_IsWmdrmnetSupported,                 DRM_REVOCATION_TYPE_WMDRM_ND,       &g_dastrWMDRMNET_Revocation,    &g_dastrRevocationGuidWMDRMNET, &g_guidRevocationTypeWMDRMNET,  &g_pubkeyWMDRMNDRevocation  },
};

    /*
    // The CRL has the following format:
    //
    // +--------------------+------------------------------------------+
    // | SECTIONS           | FIELDS                                   |
    // +--------------------+------------------------------------------+
    // | Header             | 32-bit CRL Version                       |
    // |                    +------------------------------------------+
    // |                    | 32-bit Number of Entries                 |
    // +--------------------+------------------------------------------+
    // | Revocation Entries | Multiple 160-bit Revocation Entries      |
    // +--------------------+------------------------------------------+
    // | Certificate        | 32-bit Certificate Length                |
    // |                    +------------------------------------------+
    // |                    | Variable Length Certificate              |
    // +--------------------+------------------------------------------+
    // | Signature          | 8-bit Signature Type                     |
    // |                    +------------------------------------------+
    // |                    | 16-bit Signature Length                  |
    // |                    +------------------------------------------+
    // |                    | Variable Length Signature                |
    // +--------------------+------------------------------------------+
    */

/******************************************************************************
**
** Function :   DRM_RVK_VerifyRevocationInfo
**
** Synopsis :   Verify signature on the revocation info
**
** Arguments :  f_pbRevInfo  - rev info buffer (already b64 decoded)
**              f_pcbRevInfo - size of rev info buffer
**              [f_pCryptoCtx]          -- Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
** Returns :
**
** Notes :
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_VerifyRevocationInfo(
    __in_bcount( f_cbRevInfo ) DRM_BYTE           *f_pbRevInfo,
    __in                       DRM_DWORD           f_cbRevInfo,
    __out                      DRM_RLVI           *f_pRLVI,
    __in                       DRM_CRYPTO_CONTEXT *f_pCryptoCtx,
    __in_opt                   DRM_BB_CONTEXT     *f_pcontextBBX,
    __in_opt                   DRM_SECSTORE_CONTEXT *f_pcontextSST,
    __in_opt                   DRM_DST            *f_pDatastore )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT      dr                = DRM_SUCCESS;
    DRM_DWORD       cbRevInfo         = f_cbRevInfo;
    DRM_DWORD       cbSignedBytes     = 0;
    DRM_DWORD       cbEntries         = 0;
    DRM_DWORD       dwRevInfo         = 0;
    DRM_SUBSTRING   dasstrCertificate = EMPTY_DRM_SUBSTRING;
#if SIXTEEN_BIT_ADDRESSING
    DRM_BYTE        rgbSignature[ __CB_DECL( max( RLVI_SIGNATURE_SIZE_1, RLVI_SIGNATURE_SIZE_2 ) ) ];
#endif
    DRM_WORD        cbSignature = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_VerifyRevocationInfo );

    ChkArg( f_pbRevInfo != NULL
          && f_pRLVI     != NULL );
    ChkArg( SIZEOF( DRM_RLVI_HEAD ) <= f_cbRevInfo );
    ChkArg( f_pCryptoCtx    != NULL );

    MEMSET( f_pRLVI, 0, SIZEOF( DRM_RLVI ) );

    /* ID */
    NETWORKBYTES_TO_DWORD( f_pRLVI->head.dwID, f_pbRevInfo, dwRevInfo );
    ChkArg( RLVI_MAGIC_NUM_V1 == f_pRLVI->head.dwID || RLVI_MAGIC_NUM_V2 == f_pRLVI->head.dwID );
    dwRevInfo += SIZEOF( DRM_DWORD );

    /* Length */
    NETWORKBYTES_TO_DWORD( f_pRLVI->head.cbSignedBytes, f_pbRevInfo, dwRevInfo );
    ChkArg( SIZEOF( DRM_RLVI_HEAD ) <= f_pRLVI->head.cbSignedBytes );
    dwRevInfo += SIZEOF( DRM_DWORD );

    /* Format Version */
    f_pRLVI->head.bFormatVersion = GET_BYTE( f_pbRevInfo, dwRevInfo );
    ChkArg( (RLVI_FORMAT_VERSION_V1 == f_pRLVI->head.bFormatVersion && f_pRLVI->head.dwID == RLVI_MAGIC_NUM_V1 )
        || ( RLVI_FORMAT_VERSION_V2 == f_pRLVI->head.bFormatVersion && f_pRLVI->head.dwID == RLVI_MAGIC_NUM_V2 ) );
    dwRevInfo += SIZEOF(DRM_BYTE) / CB_NATIVE_BYTE;

    /* Reserved */
    f_pRLVI->head.bReserved[0] = GET_BYTE( f_pbRevInfo, dwRevInfo );
    dwRevInfo += SIZEOF(DRM_BYTE) / CB_NATIVE_BYTE;
    f_pRLVI->head.bReserved[1] = GET_BYTE( f_pbRevInfo, dwRevInfo );
    dwRevInfo += SIZEOF(DRM_BYTE) / CB_NATIVE_BYTE;
    f_pRLVI->head.bReserved[2] = GET_BYTE( f_pbRevInfo, dwRevInfo );
    dwRevInfo += SIZEOF(DRM_BYTE) / CB_NATIVE_BYTE;

    /* Sequence Nubmer */
    NETWORKBYTES_TO_DWORD( f_pRLVI->head.dwRIV, f_pbRevInfo, dwRevInfo );
    dwRevInfo += SIZEOF( DRM_DWORD );

    /* Issued Time: Stored in little endian order for V1 and big endian for V2 */
    if( RLVI_MAGIC_NUM_V1 == f_pRLVI->head.dwID )
    {
        LITTLEENDIAN_BYTES_TO_DWORD( f_pRLVI->head.ftIssuedTime.dwLowDateTime, f_pbRevInfo, dwRevInfo );
        dwRevInfo += SIZEOF( DRM_DWORD );
        LITTLEENDIAN_BYTES_TO_DWORD( f_pRLVI->head.ftIssuedTime.dwHighDateTime, f_pbRevInfo, dwRevInfo );
        dwRevInfo += SIZEOF( DRM_DWORD );
    }
    else if( RLVI_MAGIC_NUM_V2 == f_pRLVI->head.dwID )
    {
        DRM_UINT64 ui64IssueTime = DRM_UI64LITERAL(0,0);

        NETWORKBYTES_TO_QWORD( ui64IssueTime, f_pbRevInfo, dwRevInfo );
        UI64_TO_FILETIME(ui64IssueTime, f_pRLVI->head.ftIssuedTime);
        dwRevInfo += SIZEOF( DRM_UINT64 );
    }
    else
    {
        ChkDR( DRM_E_INVALID_REVOCATION_LIST );
    }

    /* Entry Count */
    NETWORKBYTES_TO_DWORD( f_pRLVI->head.dwRecordCount, f_pbRevInfo, dwRevInfo );
    dwRevInfo += SIZEOF( DRM_DWORD );

    /* Entries */
    cbEntries = SIZEOF( DRM_RLVI_RECORD ) * f_pRLVI->head.dwRecordCount;
    ChkArg( f_cbRevInfo - dwRevInfo >= cbEntries + ( SIZEOF(DRM_BYTE) / CB_NATIVE_BYTE ) + SIZEOF( DRM_DWORD ) );
    f_pRLVI->ibEntries = dwRevInfo;
    dwRevInfo += cbEntries;

    cbSignedBytes = dwRevInfo;

    /* Signature Type */
    f_pRLVI->signature.bSignatureType = GET_BYTE( f_pbRevInfo, dwRevInfo );
    dwRevInfo += SIZEOF(DRM_BYTE) / CB_NATIVE_BYTE;
    if( f_pRLVI->signature.bSignatureType == RLVI_SIGNATURE_TYPE_1 )
    {
        ChkArg( RLVI_SIGNATURE_SIZE_1 + SIZEOF(DRM_DWORD) <= (cbRevInfo - dwRevInfo) );
        cbSignature = RLVI_SIGNATURE_SIZE_1;
    }
    else if( f_pRLVI->signature.bSignatureType == RLVI_SIGNATURE_TYPE_2 )
    {
        NETWORKBYTES_TO_WORD( cbSignature, f_pbRevInfo, dwRevInfo );
        dwRevInfo += SIZEOF( DRM_WORD );
        ChkArg( cbSignature <= cbRevInfo - dwRevInfo );
    }
    else
    {
        ChkDR( DRM_E_INVALID_REVOCATION_LIST );
    }

    /* Signature */
    f_pRLVI->signature.ibSignature = dwRevInfo;
#if SIXTEEN_BIT_ADDRESSING
    DRM_BYT_CopyBytes( rgbSignature, 0, f_pbRevInfo, dwRevInfo, cbSignature );
#endif
    dwRevInfo += cbSignature;


    /*
    ** Certificate Chain Length.
    ** For RLVI_SIGNATURE_TYPE_1 the size is written before the certificate.
    ** For RLVI_SIGNATURE_TYPE_2 the certificate is PlayReady cert and size is not written.
    */
    if ( f_pRLVI->signature.bSignatureType == RLVI_SIGNATURE_TYPE_1 )
    {
        NETWORKBYTES_TO_DWORD( f_pRLVI->certchain.cbCertChain, f_pbRevInfo, dwRevInfo );
        dwRevInfo += SIZEOF( DRM_DWORD );
    }
    else if( f_pRLVI->signature.bSignatureType == RLVI_SIGNATURE_TYPE_2 )
    {
        f_pRLVI->certchain.cbCertChain = f_cbRevInfo - dwRevInfo;
    }
    else
    {
        ChkDR( DRM_E_INVALID_REVOCATION_LIST );
    }

    /* Certificate Chain */
    ChkArg( f_cbRevInfo - dwRevInfo >= f_pRLVI->certchain.cbCertChain );
    f_pRLVI->certchain.ibCertChain = dwRevInfo;
    dwRevInfo += f_pRLVI->certchain.cbCertChain;

    /* Verify Signature */

    dasstrCertificate.m_ich = f_pRLVI->certchain.ibCertChain;
    dasstrCertificate.m_cch = f_pRLVI->certchain.cbCertChain;

#if SIXTEEN_BIT_ADDRESSING
    if( f_pRLVI->head.bFormatVersion == RLVI_FORMAT_VERSION_V1 )
    {
        ChkDR( DRM_RVK_VerifyCRLSignature(
            f_pcontextBBX,
            f_pcontextSST,
            f_pDatastore,
            f_pbRevInfo,
            cbSignedBytes,
            rgbSignature,
            cbSignature,
            dasstrCertificate,
            f_pCryptoCtx ) );
    }
    else if( f_pRLVI->head.bFormatVersion == RLVI_FORMAT_VERSION_V2 )
    {
        ChkDR( DRM_BCrl_VerifySignature(f_pbRevInfo,
                                        cbSignedBytes,
                                        rgbSignature,
                                        cbSignature,
                                        f_pbRevInfo + dasstrCertificate.m_ich,
                                        dasstrCertificate.m_cch,
                                        (const PUBKEY_P256*)&g_rgbMSPlayReadyRootIssuerPubKey,
                                        f_pCryptoCtx, f_pcontextBBX, f_pcontextSST, f_pDatastore ) );
    }
#else
    if( f_pRLVI->head.bFormatVersion == RLVI_FORMAT_VERSION_V1 )
    {
        ChkDR( DRM_RVK_VerifyCRLSignature(
            f_pcontextBBX,
            f_pcontextSST,
            f_pDatastore,
            f_pbRevInfo,
            cbSignedBytes,
            f_pbRevInfo + f_pRLVI->signature.ibSignature,
            cbSignature,
            dasstrCertificate,
            f_pCryptoCtx ) );
    }
    else if( f_pRLVI->head.bFormatVersion == RLVI_FORMAT_VERSION_V2 )
    {
        ChkDR( DRM_BCrl_VerifySignature(f_pbRevInfo,
                                        cbSignedBytes,
                                        f_pbRevInfo + f_pRLVI->signature.ibSignature,
                                        cbSignature,
                                        f_pbRevInfo + dasstrCertificate.m_ich,
                                        dasstrCertificate.m_cch,
                                        (const PUBKEY_P256*)&g_rgbMSPlayReadyRootIssuerPubKey,
                                        f_pCryptoCtx, f_pcontextBBX, f_pcontextSST, f_pDatastore ) );
    }
#endif

 ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*****************************************************************************
** Function: DRM_RVK_GetCurrentRevocationInfo
**
** Synopsis: gets the current device exclusion list from the secure store
**
** Arguments:
** [f_pContextRev]       -- revocation store context
** [f_pbBuffer]          -- output buffer for exclusion list; NULL to request
**                          required size
** [f_pcbBuffer]         -- DRM_DWORD to hold max buffer size on in, bytes actually
**                          used on out
** [f_pCryptoCtx]          -- Pointer to DRM_CRYPTO_CONTEXT used for temporary data in crypto code.
*****************************************************************************/

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_GetCurrentRevocationInfo(
    __in       DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in const DRM_GUID                    *f_pguidRevocationType,
    __out_bcount(*f_pcbBuffer) DRM_BYTE    *f_pbBuffer,
    __inout    DRM_DWORD                   *f_pcbBuffer,
    __out      DRM_RLVI                    *f_pRLVI,
    __in       DRM_CRYPTO_CONTEXT          *f_pCryptoCtx )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;
    _RevocationStoreContext *pRevStore = (_RevocationStoreContext*)f_pContextRev;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_GetCurrentRevocationInfo );

    ChkArg( f_pRLVI       != NULL
         && f_pContextRev != NULL
         && f_pCryptoCtx  != NULL );

    if ( ( 0 != MEMCMP(f_pguidRevocationType, &g_guidRevocationTypeRevInfo,  SIZEOF( DRM_GUID ) ) )
      && ( 0 != MEMCMP(f_pguidRevocationType, &g_guidRevocationTypeRevInfo2, SIZEOF( DRM_GUID ) ) ) )
    {
        ChkDR( DRM_E_REVOCATION_GUID_NOT_RECOGNIZED );
    }

    dr = DRM_RVS_GetRevocationData( f_pContextRev,
                                    f_pguidRevocationType,
                                    f_pbBuffer,
                                    f_pcbBuffer);

    if ( dr == DRM_E_FILENOTFOUND
      || dr == DRM_E_DSTSLOTNOTFOUND )
    {
        /* if there's no revocation info current stored, fake out the RIV so the new info can be stored */
        MEMSET( f_pRLVI, 0, SIZEOF( DRM_RLVI ) );
        f_pRLVI->head.dwRIV = 0;
        *f_pcbBuffer = 0;
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }
    ChkDR(dr);

    /* decode the revocation info */

    ChkDR( DRM_RVK_VerifyRevocationInfo( f_pbBuffer,
                                        *f_pcbBuffer,
                                         f_pRLVI,
                                         f_pCryptoCtx,
                                         pRevStore->pContextBB,
                                         pRevStore->pContextSST,
                                         pRevStore->pDatastore) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/*
Note: this function only works for legacy wmdrm app certs and will only support
pre-playready types of crls and rev info.
*/
DRM_API DRM_RESULT DRM_RVK_StoreRevocationLists(
    IN     DRM_CRYPTO_CONTEXT    *f_pContextCrypto,
    IN     DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    IN     DRM_DWORD              f_cRevocationLists,
    IN     DRM_RVK_LIST          *f_pRevocationLists,
    IN     APPCERT               *f_pAppcert,
    IN     DRM_BYTE              *f_pbRevocationBuffer,
    IN     DRM_DWORD              f_cbRevocationBuffer,
    IN OUT DRM_REVLIST_ID_VALUES *f_pRevList )
{
    DRM_RESULT    dr = DRM_SUCCESS;
    DRM_DWORD     i = 0;
    DRM_BOOL      fUpdated = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_StoreRevocationLists );

    ChkArg( f_cRevocationLists > 0
     && f_pRevocationLists != NULL
     && f_pbRevocationBuffer != NULL
     && f_cbRevocationBuffer > 0
     && f_pContextCrypto != NULL
     && f_pContextRev  != NULL );
    ChkArg( f_pRevList != NULL );

    /* Process the CRLs first */
    for ( i = 0; i < f_cRevocationLists; i++ )
    {
        if( f_pRevocationLists[i].cbRevocationList == 0 )
        {
            /* Ignore zero length revocation lists */
            continue;
        }

        if ( DRM_APPREVOCATION_IsAppRevocationSupported()
         && ( 0 == MEMCMP( &(f_pRevocationLists[i].guidRevocationList), &g_guidRevocationTypeApp, SIZEOF( DRM_GUID ) ) ) )
        {
            DRM_DWORD   cbBuffer                = 0;
            DRM_DWORD   crlVersion              = f_pRevList->wmdrmapp;
            DRM_BOOL    fUpdatedRevocationList  = FALSE;

            /* Ensure latest app crl version is up to date */
            cbBuffer = f_cbRevocationBuffer;
            ChkDR( DRM_RVK_GetCurrentAppRevocationList(f_pContextRev,
                                                       f_pbRevocationBuffer,
                                                      &cbBuffer,
                                                      &crlVersion ) );
            if( crlVersion > f_pRevList->wmdrmapp
             || f_pRevList->wmdrmapp == DRM_APP_REVOCATION_VERSION_NONE )
            {
                f_pRevList->wmdrmapp = crlVersion;
                fUpdated = TRUE;
            }

            ChkDR( DRM_RVK_UpdateAppRevocationListDecoded( f_pContextCrypto,
                        f_pContextRev,
                        f_pRevocationLists[i].pbRevocationList,
                        f_pRevocationLists[i].cbRevocationList,
                        f_pAppcert,
                        f_pRevList->wmdrmapp,
                       &fUpdatedRevocationList,
                       NULL ) );
            if( fUpdatedRevocationList )
            {
                fUpdated = TRUE;
            }

            continue;
        }

        if ( DRM_DEVICEREVOCATION_IsDeviceRevocationSupported()
          && ( 0 == MEMCMP( &(f_pRevocationLists[i].guidRevocationList), &g_guidRevocationTypeDevice, SIZEOF( DRM_GUID ) ) ) )
        {
            DRM_BOOL    fUpdatedRevocationList  = FALSE;
            ChkDR( DRM_RVK_UpdateRevocationListDecoded(
                f_pContextCrypto,
                f_pContextRev,
                f_pRevocationLists[i].pbRevocationList,
                f_pRevocationLists[i].cbRevocationList,
                f_pbRevocationBuffer,
                f_cbRevocationBuffer,
                &g_guidRevocationTypeDevice,
                &g_pubkeyDeviceRevocation,
                &fUpdatedRevocationList ) );
            if( fUpdatedRevocationList )
            {
                fUpdated = TRUE;
            }
            continue;
        }

        if ( DRM_WMDRMNET_IsWmdrmnetSupported()
          && ( 0 == MEMCMP( &(f_pRevocationLists[i].guidRevocationList), &g_guidRevocationTypeWMDRMNET, SIZEOF( DRM_GUID ) ) ) )
        {
            DRM_BOOL    fUpdatedRevocationList  = FALSE;
            ChkDR( DRM_RVK_UpdateWMDRMNETRevocationListDecoded(
                                          f_pContextCrypto,
                                          f_pContextRev,
                                          f_pRevocationLists[i].pbRevocationList,
                                          f_pRevocationLists[i].cbRevocationList,
                                          f_pbRevocationBuffer,
                                          f_cbRevocationBuffer,
                                         &fUpdatedRevocationList,
                                          NULL) );
            if( fUpdatedRevocationList )
            {
                fUpdated = TRUE;
            }
            continue;
        }
    }

    /* Next process the rev infos */
    for ( i = 0; i < f_cRevocationLists; i++ )
    {
        if( f_pRevocationLists[i].cbRevocationList == 0 )
        {
            /* Ignore zero length revocation infos */
            continue;
        }

        if ( 0 == MEMCMP(&(f_pRevocationLists[i].guidRevocationList), &g_guidRevocationTypeRevInfo, SIZEOF(DRM_GUID)) )
        {
            /* To be symmetric with DRM_MGR_GetRevInfo, we accept base64 encoded revinfos */

            DRM_STRING dstrRevInfo = EMPTY_DRM_STRING;
            DRM_BOOL   fUpdatedRevInfo = FALSE;

            dstrRevInfo.pwszString = (DRM_WCHAR*)f_pRevocationLists[i].pbRevocationList;
            dstrRevInfo.cchString = f_pRevocationLists[i].cbRevocationList / SIZEOF( DRM_WCHAR );

            ChkDR( DRM_B64_DecodeW( (DRM_CONST_STRING *)&dstrRevInfo,
                            &f_pRevocationLists[i].cbRevocationList,
                            NULL,
                            DRM_BASE64_DECODE_IN_PLACE ) );

            ChkDR( DRM_RVK_StoreRevInfo( f_pContextCrypto,
                                         f_pContextRev,
                                        &(f_pRevocationLists[i].guidRevocationList),
                                         f_pRevocationLists[i].pbRevocationList,
                                         f_pRevocationLists[i].cbRevocationList,
                                         f_pbRevocationBuffer,
                                         f_cbRevocationBuffer,
                                         f_pRevList,
                                         &fUpdatedRevInfo) );

            if ( fUpdatedRevInfo )
            {
                /* DRM_RVK_StoreRevInfo will fully update passed in RevLists */
                fUpdated = FALSE;
            }

            continue;
        }
    }

ErrorExit:

    if ( fUpdated )
    {
        DRM_RVK_UpdateRevocationVersionsCache( f_pContextRev,
                                               f_pContextCrypto,
                                               f_pRevList,
                                               f_pbRevocationBuffer,
                                               f_cbRevocationBuffer,
                                               NULL );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_FindEntryInRevInfo(
    __in                           DRM_RLVI    *f_prlvi,
    __in_bcount(f_cbRevInfo) const DRM_BYTE    *f_pbRevInfo,
    __in                           DRM_DWORD    f_cbRevInfo,
    __in                     const DRM_GUID    *f_pguidEntry,
    __out                          DRM_DWORD   *f_pdwVersion )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT  dr      = DRM_SUCCESS;
    DRM_BOOL    fFound  = FALSE;
    DRM_DWORD   iRev    = 0;

    ChkArg( f_pdwVersion != NULL );
    ChkArg( f_pbRevInfo != NULL );
    ChkArg( f_prlvi != NULL );
    ChkArg( f_cbRevInfo > 0 );
    ChkArg( f_pguidEntry != NULL );

    *f_pdwVersion = DRM_NO_PREVIOUS_CRL;

    /* Check buffer is big enough */
    ChkBOOL( f_cbRevInfo >= f_prlvi->ibEntries + ( f_prlvi->head.dwRecordCount * SIZEOF( DRM_RLVI_RECORD ) ), DRM_E_INVALIDREVINFO );

    /* Find the entry in the rev info corresponding to the supplied GUID */
    for ( iRev = 0; iRev < f_prlvi->head.dwRecordCount; iRev++ )
    {
        DRM_INT64  qwrlviVersion;
        DRM_DWORD  dwrlviVersion = 0;
        DRM_GUID   rlviGUID;

        /* Extract the CRL GUID from the iRev-th DRM_RLVI_RECORD in the revinfo */
        DRM_BYT_CopyBytes( &rlviGUID,
                        0,
                        f_pbRevInfo,
                        f_prlvi->ibEntries + ( iRev * SIZEOF( DRM_RLVI_RECORD ) ), /* Add ibEntries offset to the offset of the current record */
                        SIZEOF(DRM_GUID) );

        /* Correct endianness */
        FIX_ENDIAN_DWORD( rlviGUID.Data1 );
        FIX_ENDIAN_WORD( rlviGUID.Data2 );
        FIX_ENDIAN_WORD( rlviGUID.Data3 );

        /* Extract the CRL version from the iRev-th DRM_RLVI_RECORD in the revinfo */
        NETWORKBYTES_TO_QWORD( qwrlviVersion,
                        f_pbRevInfo,
                        f_prlvi->ibEntries + ( iRev * SIZEOF( DRM_RLVI_RECORD ) ) + SIZEOF( DRM_GUID ) );  /* Add ibEntries offset to the offset of the current record and skip the GUID */
        dwrlviVersion = DRM_I64ToUI32(qwrlviVersion);

        if ( 0 == MEMCMP(&rlviGUID, f_pguidEntry, SIZEOF(DRM_GUID)) )
        {
            fFound = TRUE;
            *f_pdwVersion = dwrlviVersion;
            break;
        }
    }
ErrorExit:
    if( DRM_SUCCEEDED( dr ) )
    {
        if( fFound )
        {
            dr = DRM_SUCCESS;
        }
        else
        {
            dr = DRM_S_FALSE;
        }
    }
    return dr;
}

DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_VerifyRevInfoSafe(
    __in                               DRM_CRYPTO_CONTEXT          *f_pContextCrypto,
    __in                               DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    __in const                         DRM_GUID                    *f_pguidRevocationType,
    __inout_bcount(f_cbRevocationInfo) DRM_BYTE                    *f_pbRevocationInfo,
    __in                               DRM_DWORD                    f_cbRevocationInfo,
    __in                               DRM_BYTE                    *f_pbRevocationBuffer,
    __in                               DRM_DWORD                    f_cbRevocationBuffer,
    __inout                            DRM_REVLIST_ID_VALUES       *f_pRevList,
    __out                              DRM_RLVI                    *f_pnewRLVI,
    __out                              DRM_BOOL                    *f_pfRIV2,
    __out                              DRM_BOOL                    *f_pfVerified )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT     dr = DRM_SUCCESS;
    DRM_DWORD      cbTmp = 0;
    DRM_DWORD      crlVersion = 0;
    DRM_DWORD      dwWMDRMAppCRLVersion = 0;
    DRM_DWORD      dwPlayReadyAppCRLVersion = 0;
    DRM_DWORD      dwWMDRMNETCRLVersion = 0;
    DRM_DWORD      dwDeviceCRLVersion = 0;
    DRM_DWORD      dwRuntimeCRLVersion = 0;
    _RevocationStoreContext *pRevStore = (_RevocationStoreContext*)f_pContextRev;
    DRM_RLVI       newRLVI;

    ChkArg( f_pbRevocationInfo != NULL
     && f_cbRevocationInfo != 0
     && f_pbRevocationBuffer != NULL
     && f_cbRevocationBuffer > 0
     && f_pContextRev != NULL
     && f_pfVerified != NULL
     && f_pguidRevocationType != NULL );

    if( f_pnewRLVI == NULL )
    {
        f_pnewRLVI = &newRLVI;
    }
    MEMSET( f_pnewRLVI, 0, SIZEOF( DRM_RLVI ) );

    *f_pfVerified = FALSE;

    if ( 0 == MEMCMP(f_pguidRevocationType, &g_guidRevocationTypeRevInfo2, SIZEOF( DRM_GUID ) ) )
    {
        if( f_pfRIV2 != NULL )
        {
            *f_pfRIV2 = TRUE;
        }
    }
    else if ( 0 != MEMCMP(f_pguidRevocationType, &g_guidRevocationTypeRevInfo,  SIZEOF( DRM_GUID ) ) )
    {
        ChkDR( DRM_E_REVOCATION_GUID_NOT_RECOGNIZED );
    }

    /* Verify the one from the license response is a good one */
    ChkDR( DRM_RVK_VerifyRevocationInfo( f_pbRevocationInfo,
                                         f_cbRevocationInfo,
                                         f_pnewRLVI,
                                         f_pContextCrypto,
                                         pRevStore->pContextBB,
                                         pRevStore->pContextSST,
                                         pRevStore->pDatastore) );

    /*
    ** Parse the RevInfo structure currently loaded into the revocation buffer
    ** and retrieve the version numbers for each possible CRL. The FindEntry
    ** function will return DRM_NO_PREVIOUS_CRL for CRLs the RIV has no comment
    ** on.
    */

    if( DRM_APPREVOCATION_IsAppRevocationSupported() )
    {
        ChkDR( DRM_RVK_FindEntryInRevInfo( f_pnewRLVI, f_pbRevocationInfo, f_cbRevocationInfo, &g_guidRevocationTypeApp, &dwWMDRMAppCRLVersion) );
        ChkDR( DRM_RVK_FindEntryInRevInfo( f_pnewRLVI, f_pbRevocationInfo, f_cbRevocationInfo, &g_guidRevocationTypePlayReadySilverLightApplication, &dwPlayReadyAppCRLVersion) );
    }

    if( DRM_DEVICEREVOCATION_IsDeviceRevocationSupported() )
    {
        ChkDR( DRM_RVK_FindEntryInRevInfo( f_pnewRLVI, f_pbRevocationInfo, f_cbRevocationInfo, &g_guidRevocationTypeDevice, &dwDeviceCRLVersion) );
    }

    if( DRM_WMDRMNET_IsWmdrmnetSupported() )
    {
        ChkDR( DRM_RVK_FindEntryInRevInfo( f_pnewRLVI, f_pbRevocationInfo, f_cbRevocationInfo, &g_guidRevocationTypeWMDRMNET, &dwWMDRMNETCRLVersion) );
    }

    ChkDR( DRM_RVK_FindEntryInRevInfo( f_pnewRLVI, f_pbRevocationInfo, f_cbRevocationInfo, &g_guidRevocationTypePlayReadySilverLightRuntime, &dwRuntimeCRLVersion) );

    /*
    ** For each CRL located above, find the corresponding version in the HDS.
    ** The version in the HDS ***must*** be equal or newer than the version
    ** in the RIV. We can't store the RIV if this is not true.
    */

    if( DRM_APPREVOCATION_IsAppRevocationSupported() )
    {
        if ( dwWMDRMAppCRLVersion != DRM_NO_PREVIOUS_CRL )
        {
            cbTmp = f_cbRevocationBuffer;
            ChkDR( DRM_RVK_GetCurrentAppRevocationList( f_pContextRev,
                                                        f_pbRevocationBuffer,
                                                        &cbTmp,
                                                        &crlVersion) );

            if( f_pRevList != NULL )
            {
                if( f_pRevList->wmdrmapp != crlVersion )
                {
                    f_pRevList->wmdrmapp = crlVersion;
                    f_pRevList->fUpdatedRevocationList = TRUE;
                }
            }

            if ( crlVersion < dwWMDRMAppCRLVersion )
            {
                /* Not verified - its required CRLs are not all available! */
                goto ErrorExit;
            }
        }

        if ( dwPlayReadyAppCRLVersion != DRM_NO_PREVIOUS_CRL )
        {
            cbTmp = f_cbRevocationBuffer;
            ChkDR(_GetPlayReadyRevocationList( f_pContextCrypto,
                                               f_pContextRev,
                                               &g_guidRevocationTypePlayReadySilverLightApplication,
                                               f_pbRevocationBuffer,
                                               &cbTmp,
                                               &crlVersion));
            if( f_pRevList != NULL )
            {
                if( f_pRevList->playreadyapp != crlVersion )
                {
                    f_pRevList->playreadyapp = crlVersion;
                    f_pRevList->fUpdatedRevocationList = TRUE;
                }
            }

            if ( crlVersion < dwPlayReadyAppCRLVersion )
            {
                /* Not verified - its required CRLs are not all available! */
                goto ErrorExit;
            }
        }
    }

    if ( DRM_DEVICEREVOCATION_IsDeviceRevocationSupported()
      && dwDeviceCRLVersion != DRM_NO_PREVIOUS_CRL )
    {
        cbTmp = f_cbRevocationBuffer;
        ChkDR( DRM_RVK_GetDeviceRevocationList( f_pContextCrypto,
                                                f_pContextRev,
                                                f_pbRevocationBuffer,
                                                &cbTmp,
                                                &crlVersion) );

        if( f_pRevList != NULL )
        {
            if( f_pRevList->device != crlVersion )
            {
                f_pRevList->device = crlVersion;
                f_pRevList->fUpdatedRevocationList = TRUE;
            }
        }

        if ( crlVersion < dwDeviceCRLVersion )
        {
            /* Not verified - its required CRLs are not all available! */
            goto ErrorExit;
        }
    }

    if ( DRM_WMDRMNET_IsWmdrmnetSupported()
      && dwWMDRMNETCRLVersion != DRM_NO_PREVIOUS_CRL )
    {
        cbTmp = f_cbRevocationBuffer;
        ChkDR( DRM_RVK_GetWMDRMNETList( f_pContextCrypto,
                                        f_pContextRev,
                                        f_pbRevocationBuffer,
                                        &cbTmp,
                                        &crlVersion) );
        if( f_pRevList != NULL )
        {
            if( f_pRevList->wmdrmnet != crlVersion )
            {
                f_pRevList->wmdrmnet = crlVersion;
                f_pRevList->fUpdatedRevocationList = TRUE;
            }
        }

        if ( crlVersion < dwWMDRMNETCRLVersion )
        {
            /* Not verified - its required CRLs are not all available! */
            goto ErrorExit;
        }
    }

    if ( dwRuntimeCRLVersion != DRM_NO_PREVIOUS_CRL )
    {
        cbTmp = f_cbRevocationBuffer;
        ChkDR(_GetPlayReadyRevocationList( f_pContextCrypto,
                                           f_pContextRev,
                                           &g_guidRevocationTypePlayReadySilverLightRuntime,
                                           f_pbRevocationBuffer,
                                           &cbTmp,
                                           &crlVersion ));
        if( f_pRevList != NULL )
        {
            if( f_pRevList->runtime != crlVersion )
            {
                f_pRevList->runtime = crlVersion;
                f_pRevList->fUpdatedRevocationList = TRUE;
            }
        }

        if ( crlVersion < dwRuntimeCRLVersion )
        {
            /* Not verified - its required CRLs are not all available! */
            goto ErrorExit;
        }

    }

    /*
     *  dr could still be S_FALSE from a prior call to FindEntryInRevInfo.
     *  Set it to success here.
     */
    dr = DRM_SUCCESS;

    /* Verified - its required CRLs are all available. */
    *f_pfVerified = TRUE;

ErrorExit:
    return dr;
}


DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_StoreRevInfo (
    IN     DRM_CRYPTO_CONTEXT          *f_pContextCrypto,
    IN     DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    IN     const DRM_GUID              *f_pguidRevocationType,
    IN     DRM_BYTE                    *f_pbRevocationInfo,
    IN     DRM_DWORD                    f_cbRevocationInfo,
    IN     DRM_BYTE                    *f_pbRevocationBuffer,
    IN     DRM_DWORD                    f_cbRevocationBuffer,
    IN OUT DRM_REVLIST_ID_VALUES       *f_pRevList,
       OUT DRM_BOOL                    *f_pfWasStored )
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT      dr          = DRM_SUCCESS;
    DRM_RLVI        currentRLVI;
    DRM_RLVI        newRLVI;
    DRM_DWORD       cbTmp       = 0;
    DRM_BOOL        fRIV2       = FALSE;
    DRM_BOOL        fVerified   = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_StoreRevInfo );

    MEMSET( &newRLVI,       0, SIZEOF( DRM_RLVI ) );
    MEMSET( &currentRLVI,   0, SIZEOF( DRM_RLVI ) );

    if( f_pfWasStored != NULL )
    {
        *f_pfWasStored = FALSE;
    }

    ChkArg(
        f_pbRevocationInfo      != NULL
     && f_cbRevocationInfo      != 0
     && f_pbRevocationBuffer    != NULL
     && f_cbRevocationBuffer    > 0
     && f_pContextRev           != NULL );

    ChkDR( DRM_RVK_VerifyRevInfoSafe(
        f_pContextCrypto,
        f_pContextRev,
        f_pguidRevocationType,
        f_pbRevocationInfo,
        f_cbRevocationInfo,
        f_pbRevocationBuffer,
        f_cbRevocationBuffer,
        f_pRevList,
        &newRLVI,
        &fRIV2,
        &fVerified ) );

    if( !fVerified )
    {
        /* Don't update the HDS's RevInfo - its required CRLs are not all available! */
        goto ErrorExit;
    }

    cbTmp = f_cbRevocationBuffer;
    ChkDR( DRM_RVK_GetCurrentRevocationInfo( f_pContextRev,
                                             f_pguidRevocationType,
                                             f_pbRevocationBuffer,
                                             &cbTmp,
                                             &currentRLVI,
                                             f_pContextCrypto ) );

    /* If each stored version matches the corresponding rlvi version, then store the rlvi */
    if ( newRLVI.head.dwRIV > currentRLVI.head.dwRIV )
    {
        /* Storing binary revocation info */
        ChkDR( DRM_RVS_StoreRevocationData( f_pContextRev,
                                            f_pguidRevocationType,
                                            f_pbRevocationInfo,
                                            f_cbRevocationInfo ) );
        if (f_pRevList != NULL)
        {
            if( fRIV2 )
            {
                f_pRevList->riv2 = newRLVI.head.dwRIV;
            }
            else
            {
                f_pRevList->riv = newRLVI.head.dwRIV;
            }
            f_pRevList->fUpdatedRevocationList = TRUE;
        }

        if( f_pfWasStored != NULL )
        {
            *f_pfWasStored = TRUE;
        }
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;

}

/*****************************************************************************
** Function: DRM_RVK_ProcessRevocationInfo
**
** Synopsis:
** processes the revocation info struct from the license response and updates
** the revinfo and revocations stored on the device if necessary
**
** Arguments:
** [f_pDrmContext]          -- DRM context
** [f_pdastrLicenseResponse]-- license response XML string
*****************************************************************************/
/*
Note: this function only works for legacy wmdrm app certs and will only support
pre-playready types of crls and rev info.
*/

DRM_API DRM_RESULT DRM_CALL DRM_RVK_ProcessRevocationInfo (
    IN     DRM_CRYPTO_CONTEXT          *f_pContextCrypto,
    IN     DRM_REVOCATIONSTORE_CONTEXT *f_pContextRev,
    IN     DRM_ANSI_CONST_STRING       *f_pdastrLicenseResponse,
    IN     DRM_BYTE                    *f_pbRevocationBuffer,
    IN     DRM_DWORD                    f_cbRevocationBuffer,
    IN     APPCERT                     *f_pAppcert,
    IN OUT DRM_REVLIST_ID_VALUES       *f_pRevList )
{
    DRM_RESULT     dr                    = DRM_SUCCESS;
    DRM_BYTE      *pbLicenseResponse     = 0;
    DRM_SUBSTRING  dasstrLicenseResponse = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING  dasstrDataWithoutTags = EMPTY_DRM_SUBSTRING;
    DRM_BYTE      *pbNewRevocationInfo   = NULL;
    DRM_DWORD      cbNewRevocationInfo   = 0;
    DRM_DWORD      iRev                  = 0 ;
    DRM_SUBSTRING  dasstrRevocationList  = EMPTY_DRM_SUBSTRING;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_ProcessRevocationInfo );

    ChkArg( f_pdastrLicenseResponse            != NULL
         && f_pdastrLicenseResponse->pszString != NULL
         && f_pdastrLicenseResponse->cchString != 0
         && f_pRevList != NULL
         && f_pContextCrypto != NULL
         && f_pContextRev  != NULL );

    pbLicenseResponse = (DRM_BYTE*)f_pdastrLicenseResponse->pszString;
    dasstrLicenseResponse.m_cch = f_pdastrLicenseResponse->cchString;

    /* foreach revocation type we care about, get it out of the license response, and store if the version is newer */
    for (iRev = 0; iRev < NO_OF(g_arevDispatch); iRev++)
    {
        if( g_arevDispatch[iRev].pfnIsSupported() )
        {
            dr = _ExtractRevocationList( f_pdastrLicenseResponse,
                                         g_arevDispatch [iRev].pdastrTypeGuid,
                                         &dasstrRevocationList );

            if ( dr == DRM_E_XMLNOTFOUND )
            {
                dr = _ExtractRevocationList( f_pdastrLicenseResponse,
                                             g_arevDispatch [iRev].pdastrType,
                                             &dasstrRevocationList );
                if ( dr == DRM_E_XMLNOTFOUND )
                {
                    ChkDR(DRM_SUCCESS);  /* This sets 'dr' and keeps the PREfast tool happy. */
                    continue;
                }
            }
            ChkDR(dr);

            switch ( g_arevDispatch[iRev].eType )
            {
            case DRM_REVOCATION_TYPE_WMDRM_APP:
                ChkDR( DRM_RVK_UpdateAppRevocationListA( f_pContextCrypto,
                                                         f_pContextRev,
                                                         f_pbRevocationBuffer,
                                                         f_cbRevocationBuffer,
                                              (DRM_CHAR*)f_pdastrLicenseResponse->pszString,
                                                        &dasstrRevocationList,
                                                         f_pAppcert,
                                                         f_pRevList->wmdrmapp,
                                                        &f_pRevList->fUpdatedRevocationList ) );
                break;

            case DRM_REVOCATION_TYPE_WMDRM_DEVICE:
                ChkDR( DRM_RVK_UpdateRevocationList( f_pContextCrypto,
                                                     f_pContextRev,
                                          (DRM_CHAR*)f_pdastrLicenseResponse->pszString,
                                                    &dasstrRevocationList,
                                                     f_pbRevocationBuffer,
                                                     f_cbRevocationBuffer,
                                                     g_arevDispatch[iRev].pguid,
                                                     g_arevDispatch[iRev].ppubkey ) );
                break;
            case DRM_REVOCATION_TYPE_WMDRM_ND:
                ChkDR( DRM_RVK_UpdateWMDRMNETRevocationList(
                                              f_pContextCrypto,
                                              f_pContextRev,
                                   (DRM_CHAR*)f_pdastrLicenseResponse->pszString,
                                             &dasstrRevocationList,
                                              f_pbRevocationBuffer,
                                              f_cbRevocationBuffer,
                                              g_arevDispatch[iRev].ppubkey ) );
                break;
            default:
                ChkDR(DRM_E_INVALIDARG);
                break;
            }
        }
    }

    /*
      Process the revocation info struct from the license response after the
      CRLs in the license response if present are processed.
    */

    dr = DRM_XML_GetSubNodeA((DRM_CHAR*)pbLicenseResponse,
                            &dasstrLicenseResponse,
                            &g_dastrTagRevocationInfo,
                             NULL,
                             NULL,
                             0,
                             NULL,
                            &dasstrDataWithoutTags,
                             1);
    if ( DRM_E_XMLNOTFOUND == dr )
    {
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }
    ChkDR(dr);

     /* pbOffset will point within the base64 revocation list we were passed,
       such that after base64 decoding the end of the decoded data will co-incide with
       the end of the buffer we were given, minus one byte.

       This is because we are giving the B64 decoder overlapping memory for source and desitination,
       and we can't use the DECODE_IN_PLACE flag because this is ANSI where the offset may not coincide
       with a word boundary as required on 16-bit platforms.
     */

    pbNewRevocationInfo = pbLicenseResponse
                        + __CB_DECL( dasstrDataWithoutTags.m_ich + dasstrDataWithoutTags.m_cch - CB_BASE64_DECODE( dasstrDataWithoutTags.m_cch ) - 1 )
                        + ( ( dasstrDataWithoutTags.m_ich + dasstrDataWithoutTags.m_cch - CB_BASE64_DECODE( dasstrDataWithoutTags.m_cch ) ) % CB_NATIVE_BYTE );

    cbNewRevocationInfo = CB_BASE64_DECODE( dasstrDataWithoutTags.m_cch ) + 1 - ( ( dasstrDataWithoutTags.m_ich + dasstrDataWithoutTags.m_cch - CB_BASE64_DECODE( dasstrDataWithoutTags.m_cch ) ) % CB_NATIVE_BYTE );


    /* decode the XML for the new revocation info in place */
    ChkDR( DRM_B64_DecodeA( (const DRM_CHAR*)pbLicenseResponse,
                           &dasstrDataWithoutTags,
                           &cbNewRevocationInfo,
                            pbNewRevocationInfo,
                            0 ) );

    ChkDR( DRM_RVK_StoreRevInfo( f_pContextCrypto,
                            f_pContextRev,
                           &g_guidRevocationTypeRevInfo,
                            pbNewRevocationInfo,
                            cbNewRevocationInfo,
                            f_pbRevocationBuffer,
                            f_cbRevocationBuffer,
                            f_pRevList,
                            NULL ) );

ErrorExit:

    /*
    ** Update the cached revocation version numbers. Do so regardless of the
    ** overall result of the update.
    */
    DRM_RVK_UpdateRevocationVersionsCache( f_pContextRev,
                                           f_pContextCrypto,
                                           f_pRevList,
                                           f_pbRevocationBuffer,
                                           f_cbRevocationBuffer,
                                           NULL );

    if( ( dasstrDataWithoutTags.m_cch != 0 )
        && ( f_pdastrLicenseResponse != NULL )
        && ( f_pdastrLicenseResponse->pszString != NULL ))
    {
        /* Since we decoded in-place, we need to clean up otherwise the XML parsing may
         * not work properly later on. So we have to overwrite our binary with spaces.
         *
         * NOTE: This means that we CANNOT process the same revocation list in the same
         * license response buffer twice since we're overwriting the buffer we were given
         */
        DRM_BYT_SetBytes( (DRM_BYTE*)f_pdastrLicenseResponse->pszString, dasstrDataWithoutTags.m_ich, dasstrDataWithoutTags.m_cch, ' ');
    }

    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        dr = DRM_E_REVOCATION_BUFFERTOOSMALL;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
**  Function    :   DRM_RVK_GetCRL
**
**  Synopsis    :   This function returns the CRL corresponding to the GUID
**
**  Arguments   :   IN      f_guidRevocationType : The GUID for Revocation Type
**                  OUT     f_pbCRL              : contains the CRL if retrieved
**                  IN OUT  f_pcbCRL             : size of CRL in bytes
**                  OUT     f_pqwCRLVersion      : CRL version
**                  IN      f_pcontextBBX        : Blackbox context
**                  IN      f_pcontextHDS        : HDS context
**                  IN      f_rgbPassword        : Revocation store password
**
**  Returns     :   DRM_SUCCESS            :If the CRL was successfully retrieved
**              :   DRM_E_BUFFER_TOO_SMALL :If the size of f_pbCRL is less than the
**                                          the size of CRL.
**                  DRM_E_FILENOTFOUND     :If no CRL was found in the store.
**
******************************************************************************/
DRM_API DRM_NO_INLINE DRM_RESULT DRM_CALL DRM_RVK_GetCRL(
    IN     DRM_CRYPTO_CONTEXT           *f_pContextCrypto,
    IN     DRM_REVOCATIONSTORE_CONTEXT  *f_pContextRev,
    IN     const DRM_GUID               *f_pguidRevocationType,
    __out_bcount_opt( *f_pcbCRL ) DRM_BYTE *f_pbCRL,
    IN OUT DRM_DWORD                    *f_pcbCRL,
       OUT DRM_DWORD                    *f_pdwCRLVersion)
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr        = DRM_SUCCESS;
    DRM_DWORD  dwVersion = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_GetCRL );

    ChkArg(f_pcbCRL        != NULL
        && f_pContextRev   != NULL);

    if ( DRM_WMDRMNET_IsWmdrmnetSupported()
      && ( 0 == MEMCMP( f_pguidRevocationType, &g_guidRevocationTypeWMDRMNET, SIZEOF( DRM_GUID ) ) ) )
    {
        ChkDR(DRM_RVK_GetWMDRMNETList(f_pContextCrypto,
                                      f_pContextRev,
                                      f_pbCRL,
                                      f_pcbCRL,
                                     &dwVersion));
    }
    else if ( DRM_DEVICEREVOCATION_IsDeviceRevocationSupported()
      && ( 0 == MEMCMP( f_pguidRevocationType, &g_guidRevocationTypeDevice, SIZEOF( DRM_GUID ) ) ) )
    {
        ChkDR(DRM_RVK_GetDeviceRevocationList( f_pContextCrypto,
                                               f_pContextRev,
                                               f_pbCRL,
                                               f_pcbCRL,
                                               &dwVersion ) );
    }
    else if ( DRM_APPREVOCATION_IsAppRevocationSupported()
     && ( 0 == MEMCMP( f_pguidRevocationType, &g_guidRevocationTypeApp, SIZEOF( DRM_GUID ) ) ) )
    {
        ChkDR(DRM_RVK_GetCurrentAppRevocationList( f_pContextRev,
                                                   f_pbCRL,
                                                   f_pcbCRL,
                                                   &dwVersion ) );
    }
    else if ( ( 0 == MEMCMP( f_pguidRevocationType, &g_guidRevocationTypePlayReadySilverLightRuntime,     SIZEOF( DRM_GUID ) ) )
           || ( 0 == MEMCMP( f_pguidRevocationType, &g_guidRevocationTypePlayReadySilverLightApplication, SIZEOF( DRM_GUID ) ) ) )
    {
        ChkDR(_GetPlayReadyRevocationList(f_pContextCrypto,
                                                 f_pContextRev,
                                                 f_pguidRevocationType,
                                                 f_pbCRL,
                                                 f_pcbCRL,
                                                &dwVersion));
    }
    else
    /* REV_INFO and REV_INFO v2 */
    if ( ( 0 == MEMCMP(f_pguidRevocationType, &g_guidRevocationTypeRevInfo,  SIZEOF( DRM_GUID ) ) )
      || ( 0 == MEMCMP(f_pguidRevocationType, &g_guidRevocationTypeRevInfo2, SIZEOF( DRM_GUID ) ) ) )
    {
        DRM_RLVI RLVI;
        ChkDR( DRM_RVK_GetCurrentRevocationInfo( f_pContextRev,
                                                 f_pguidRevocationType,
                                                 f_pbCRL,
                                                 f_pcbCRL,
                                                &RLVI,
                                                 f_pContextCrypto ) );
        dwVersion = RLVI.head.dwRIV;
    }
    else
    {
        /*
        ** Error on unrecognized CRLs since they weren't stored (see comment in DRM_RVK_SetCRL).
        */
        ChkDR( DRM_E_REVOCATION_GUID_NOT_RECOGNIZED );
    }

    if( f_pdwCRLVersion != NULL )
    {
        *f_pdwCRLVersion = dwVersion;
    }

ErrorExit:
    if ( dr == DRM_E_FILENOTFOUND )
    {
        /*
        **  when we dont' have CRL in the secure store. We shouldn't return an error; instead return S_FALSE
        */
        if( f_pdwCRLVersion != NULL )
        {
            *f_pdwCRLVersion = 0;
        }
        *f_pcbCRL = 0;

        dr = DRM_S_FALSE;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

#if DRM_SUPPORT_3RDPARTY_REVOCATION
/******************************************************************************
**
**  Function    :   _VerifyOEMGUID
**
**  Synopsis    :   This function verifies that the GUID inside the OEM wrapper is
**                  the same as the one passed in as the identifying GUID.
**
**  Arguments   :   IN f_guidRevocationType : The GUID for Revocation Type
**                  IN f_pbCRL              : contains the CRL
**                  IN f_cbCRL              : size of CRL in bytes
**
**  Returns     :   DRM_SUCCESS            :If the CRL was successfully retrieved
**              :   DRM_E_INVALIDARG       :If the GUIDs are not equal
**                  Other Error codes can be returned.
******************************************************************************/
static DRM_RESULT _VerifyOEMGUID(
    IN DRM_GUID  f_guidRevocationType,
    IN DRM_BYTE *f_pbCRL,
    IN DRM_DWORD f_cbCRL)
{
    DRM_RESULT        dr           = DRM_SUCCESS;
    DRM_CONST_STRING dstrCRL      = EMPTY_DRM_STRING;
    DRM_GUID         guid         = { 0 };

    DSTR_FROM_PB(&dstrCRL, f_pbCRL, f_cbCRL);

    ChkDR(DRM_RVK_GetOEMRevocationListGUID(&dstrCRL, (DRM_GUID *) &guid));

    ChkArg(IDENTICAL_GUIDS(&guid, &f_guidRevocationType));

ErrorExit:
    return dr;
}
#endif

/******************************************************************************
**
**  Function    :   DRM_RVK_SetCRL
**
**  Synopsis    :   This function stores the CRL in the HDS corresponding to the GUID.
**
**  Arguments   :   IN      f_guidRevocationType : The GUID for Revocation Type
**                  IN      f_pbCRL              : contains the CRL to be stored
**                  IN      f_pcbCRL             : size of CRL in bytes
**                  IN      f_pcontextBBX        : Blackbox context
**                  IN      f_pcontextHDS        : HDS context
**                  IN      f_rgbPassword        : Revocation store password
**                  OUT     f_pfUpdated          : The flag indicates the CRL
**                                                 was updated in the HDS or not.
**
**  Returns     :   DRM_SUCCESS            :If the CRL was successfully stored
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_RVK_SetCRL(
    IN     DRM_CRYPTO_CONTEXT           *f_pContextCrypto,
    IN     DRM_REVOCATIONSTORE_CONTEXT  *f_pContextRev,
    IN     const DRM_GUID               *f_pguidRevocationType,
    __in_bcount( f_cbCRL ) DRM_BYTE *f_pbCRL,
    IN     DRM_DWORD                     f_cbCRL,
    __in_bcount( f_cbRevocationBuffer ) DRM_BYTE *f_pbRevocationBuffer,
    IN     DRM_DWORD                     f_cbRevocationBuffer,
    IN OUT DRM_BOOL                     *f_pfUpdated)
{
    CLAW_AUTO_RANDOM_CIPHER
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_REVOCATION, PERF_FUNC_DRM_RVK_SetCRL );

    ChkArg( f_pfUpdated != NULL );
    *f_pfUpdated = FALSE;

    /* WMDRMNET Revocation Lists */
    if ( DRM_WMDRMNET_IsWmdrmnetSupported()
      && ( 0 == MEMCMP( f_pguidRevocationType, &g_guidRevocationTypeWMDRMNET, SIZEOF( DRM_GUID ) ) ) )
    {
        ChkDR(DRM_RVK_UpdateWMDRMNETRevocationListDecoded(f_pContextCrypto,
                                                          f_pContextRev,
                                                          f_pbCRL,
                                                          f_cbCRL,
                                                          f_pbRevocationBuffer,
                                                          f_cbRevocationBuffer,
                                                          f_pfUpdated,
                                                          NULL));
    }
    else if ( DRM_DEVICEREVOCATION_IsDeviceRevocationSupported() /* WMDRM Device Revocation Lists */
           && ( 0 == MEMCMP(f_pguidRevocationType, &g_guidRevocationTypeDevice, SIZEOF( DRM_GUID ) ) ) )
    {
        ChkDR(DRM_RVK_UpdateRevocationListDecoded(
            f_pContextCrypto,
            f_pContextRev,
            f_pbCRL,
            f_cbCRL,
            f_pbRevocationBuffer,
            f_cbRevocationBuffer,
            &g_guidRevocationTypeDevice,
            &g_pubkeyDeviceRevocation,
            f_pfUpdated ) );
    }
    else if ( DRM_APPREVOCATION_IsAppRevocationSupported() /* App Revocation Lists */
          &&  ( 0 == MEMCMP(f_pguidRevocationType, &g_guidRevocationTypeApp, SIZEOF( DRM_GUID ) ) ) )
    {
        DRM_CONST_STRING dstr = EMPTY_DRM_STRING;

        DSTR_FROM_PB( &dstr, f_pbCRL, f_cbCRL );

        ChkDR(DRM_RVK_UpdateAppRevocationListW(f_pContextCrypto,
                                               f_pContextRev,
                                               f_pbRevocationBuffer,
                                               f_cbRevocationBuffer,
                                               &dstr,
                                               NULL,
                                               0,
                                               f_pfUpdated,
                                               NULL) );
    }
    else if ( ( 0 == MEMCMP( f_pguidRevocationType, &g_guidRevocationTypePlayReadySilverLightRuntime,     SIZEOF( DRM_GUID ) ) )
           || ( 0 == MEMCMP( f_pguidRevocationType, &g_guidRevocationTypePlayReadySilverLightApplication, SIZEOF( DRM_GUID ) ) ) )
    {
        ChkDR(_SetPlayReadyRevocationList(f_pContextCrypto,
                                          f_pContextRev,
                                          f_pguidRevocationType,
                                          f_pbCRL,
                                          f_cbCRL,
                                          f_pbRevocationBuffer,
                                          f_cbRevocationBuffer,
                                          f_pfUpdated,
                                          NULL));
    }
    else
    /* REV_INFO and REV_INFO v2 */
    if ( ( 0 == MEMCMP(f_pguidRevocationType, &g_guidRevocationTypeRevInfo,  SIZEOF( DRM_GUID ) ) )
      || ( 0 == MEMCMP(f_pguidRevocationType, &g_guidRevocationTypeRevInfo2, SIZEOF( DRM_GUID ) ) ) )
    {
        ChkDR(DRM_RVK_StoreRevInfo(f_pContextCrypto,
                                   f_pContextRev,
                                   f_pguidRevocationType,
                                   f_pbCRL,
                                   f_cbCRL,
                                   f_pbRevocationBuffer,
                                   f_cbRevocationBuffer,
                                   NULL,
                                   f_pfUpdated));
    }
    else
    {
        /*
        ** Do not store unrecognized CRLs.
        */
        dr = DRM_SUCCESS;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************
**
**  Function    :   DRM_RVK_VerifySigningCert
**
**  Synopsis    :   This function verifys the server's signing certificate chain
**                  and checks against the playready CRL for revocation.
**
**
**  Arguments   :   IN      f_pdastrResponse            : Pointer to a DRM UTF8/ANSI string that
**                                                        contains a domain join response XML
**                                                        received from the server.
**                  IN      f_pdasstrResponse           : Pointer to a sub string that defines the
**                                                        domain join response string ( pre-B64 decoded )
**                                                        within the server response buffer.
**                  IN      f_pdastrCertNodePath        : XMR node path for the signing cert.
**                  IN      f_pbBCertBuffer             : Pointer to a buffer to store BCert.
**                  IN      f_cbBcertBuffer             : Size of the buffer(in bytes) of f_pbBCertBuffer.
**                  IN      f_pbRevocationBuffer        : Pointer to a buffer to be set as revocation
**                                                        buffer.
**                  IN      f_cbRevocationBuffer        : Size of the buffer(in bytes) to be set as
**                                                        revocation buffer.
**                  IN      f_pBBXContext               : Black box context
**                  IN      f_pSecStoreGlobalContext    : Secure store context
**                  IN      f_pDatastore                : Data store context
**                  IN      f_pRevStoreContext          : REV store context
**
**  Returns     :   DRM_SUCCESS            :If the CRL was successfully stored
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_RVK_VerifySigningCert(
    __in                                DRM_ANSI_CONST_STRING       *f_pdastrResponse,
    __in                                DRM_SUBSTRING               *f_pdasstrResponse,
    __in                         const  DRM_ANSI_CONST_STRING       *f_pdastrCertNodePath,
    __in_bcount( f_cbBcertBuffer )      DRM_BYTE                    *f_pbBCertBuffer,
    __in                                DRM_DWORD                    f_cbBcertBuffer,
    __in_bcount( f_cbRevocationBuffer ) DRM_BYTE                    *f_pbRevocationBuffer,
    __in                                DRM_DWORD                    f_cbRevocationBuffer,
    __in                                DRM_BB_CONTEXT              *f_pBBXContext,
    __in                                DRM_SECSTORE_CONTEXT        *f_pSecStoreGlobalContext,
    __in                                DRM_DST                     *f_pDatastore,
    __in                                DRM_REVOCATIONSTORE_CONTEXT *f_pRevStoreContext )
{
    DRM_RESULT                      dr                          = DRM_SUCCESS;
    DRM_SUBSTRING                   dasstrB64CertificateData    = EMPTY_DRM_SUBSTRING;
    DRMFILETIME                     ftCurrentTime               = {0};
    DRM_BCERT_VERIFICATIONCONTEXT   oVerificationContext        = {0};
    DRM_DWORD                       cbCert                      = 0;

    ChkDRMANSIString( f_pdastrResponse );
    ChkArg( f_pdasstrResponse != NULL );
    ChkArg( f_pdastrCertNodePath != NULL );
    ChkArg( f_pbBCertBuffer != NULL );
    ChkArg( f_cbBcertBuffer > 0 );
    ChkArg( f_pbRevocationBuffer != NULL );
    ChkArg( f_cbRevocationBuffer > 0 );
    ChkArg( f_pBBXContext != NULL );
    ChkArg( f_pSecStoreGlobalContext != NULL );
    ChkArg( f_pDatastore != NULL );
    ChkArg( f_pRevStoreContext != NULL );

    /*
    ** Extract signing certificate chain node from f_pdastrCertNodePath
    */
    ChkDR( DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                      f_pdasstrResponse,
                                      f_pdastrCertNodePath,
                                      NULL,
                                      NULL,
                                      NULL,
                                      &dasstrB64CertificateData,
                                      g_chForwardSlash ) );

    cbCert = f_cbBcertBuffer;
    ChkDR( DRM_B64_DecodeA( f_pdastrResponse->pszString,
                            &dasstrB64CertificateData,
                            &cbCert,
                            f_pbBCertBuffer,
                            0 ) );

    /*
    ** Verifying certificate chain if it's trusted.
    */
    Oem_Clock_GetSystemTimeAsFileTime( NULL, &ftCurrentTime );
    ChkDR( DRM_BCert_InitVerificationContext( &ftCurrentTime,
                                              (const PUBKEY_P256*) g_rgbMSPlayReadyRootIssuerPubKey,
                                              DRM_BCERT_CERTTYPE_UNKNOWN,
                                              &f_pBBXContext->CryptoContext,
                                              TRUE,     /* Verify signatures */
                                              FALSE,
                                              NULL,
                                              0,
                                              FALSE,    /* do not collect errors, just stop parsing */
                                              NULL,
                                              NULL,
                                              NULL,
                                              &oVerificationContext ) );
    ChkDR( DRM_BCert_ParseCertificateChain( f_pbBCertBuffer,
                                            cbCert,
                                            &oVerificationContext ) );

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        DRM_DWORD cbCRL = 0;

        /*
        ** Check signing certificate for revocation.
        */

        /* Retrieves CRL, then check if revoked */
        cbCRL = f_cbRevocationBuffer;
        dr = DRM_RVS_GetRevocationData( f_pRevStoreContext,
                                        &g_guidRevocationTypePlayReadySilverLightRuntime,
                                        f_pbRevocationBuffer,
                                        &cbCRL );

        if ( dr == DRM_SUCCESS )
        {
            ChkDR( DRM_BCrl_CheckIfPlayReadyCertRevoked( f_pbBCertBuffer,
                                                         cbCert,
                                                         f_pbRevocationBuffer,
                                                         cbCRL,
                                                         NULL,
                                                         NULL,
                                                         &f_pBBXContext->CryptoContext,
                                                         f_pBBXContext,
                                                         f_pSecStoreGlobalContext,
                                                         f_pDatastore ) );
        }
        else
        {
            if( dr == DRM_E_DSTSLOTNOTFOUND )
            {
                /* Slot not found means there's just no CRL available which is fine */
                dr = DRM_SUCCESS;
            }

            ChkDR( dr );
        }
    }

ErrorExit:

    return dr;
}

EXIT_PK_NAMESPACE_CODE;


