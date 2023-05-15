/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_NDTLICENSE_C
#include <ndttypes.h>
#include <drmndtmanager.h>
#include <ndtconstants.h>
#include <ndtlicense.h>
#include <drmxmrbuilder.h>
#include <oemrsaoaeppss.h>
#include <oemaes.h>
#include <oemndt.h>
#include <ndtcontextsizes.h>
#include <drmxmlparser.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

/******************************************************************************
** Function :   _OutputLevelsCallback
**
** Synopsis :   Callback Function that provides information to the caller about
**              output restrictions in licenses.  Used when calling DRM_Mgr_Bind
**              during the _RetrieveLicense call in the WMDRM-NDT SDK.  The
*               callback returns information protection levels for playing content
**              from the license when binding.
**
** Arguments :  [f_pvOutputLevelsData]  : Pointer to licenses output restrictions information
**              [f_dwCallbackType]      : Type of callback (PLAY or COPY), we only support PLAY
**              [f_pv]                  : miscellaneous pointer passed from Drm_Reader_Bind
**
** Returns:     DRM_SUCCESS always
**
******************************************************************************/
static DRM_RESULT DRM_CALL _OutputLevelsCallback(
    __in const DRM_VOID                 *f_pvOutputLevelsData,
    __in       DRM_POLICY_CALLBACK_TYPE  f_dwCallbackType,
    __in const DRM_VOID                 *f_pv
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__OutputLevelsCallback );

    switch( f_dwCallbackType )
    {
    case DRM_PLAY_OPL_CALLBACK:
        {
            DRM_PLAY_OPL *pOPL = (DRM_PLAY_OPL *) f_pvOutputLevelsData;
            NDT_ASF_CONTEXT *pAsfContext = (NDT_ASF_CONTEXT *) f_pv;
            MEMCPY( &pAsfContext->playOPLs.minOPL, &pOPL->minOPL, SIZEOF(pOPL->minOPL) );
            pAsfContext->fOPLValid = TRUE;
        }
        break;
    case DRM_INCLUSION_LIST_CALLBACK:
        {
            DRM_INCLUSION_LIST_CALLBACK_STRUCT *pInclusionList = (DRM_INCLUSION_LIST_CALLBACK_STRUCT *) f_pvOutputLevelsData;
            NDT_ASF_CONTEXT *pAsfContext = (NDT_ASF_CONTEXT *) f_pv;
            DRM_DWORD i;
            DRM_DWORD cGuids = 0;
            DRM_DWORD dwOffset = 0;
            DRM_BYTE *pbGuidBuffer = NULL;

            for(i = 0; i < DRM_MAX_INCLUSION_GUIDS; i++)
            {
                if(pInclusionList->rgfInclusionListValid[i][pInclusionList->dwChainDepth])
                {
                    cGuids++;
                }
            }
            if(cGuids > 0)
            {
                ChkMem( pbGuidBuffer = (DRM_BYTE*)Oem_MemAlloc( cGuids * SIZEOF( DRM_GUID ) ) );
                for(i = 0; i < DRM_MAX_INCLUSION_GUIDS; i++)
                {
                    if(pInclusionList->rgfInclusionListValid[i][pInclusionList->dwChainDepth])
                   {
                        DRM_BYT_CopyBytes( pbGuidBuffer, dwOffset, (DRM_BYTE *)&pInclusionList->rgInclusionList[i], 0, SIZEOF(DRM_GUID) );
                        dwOffset += SIZEOF(DRM_GUID );
                    }
                }
            }
            pAsfContext->InclusionList.cGUIDs = cGuids;
            pAsfContext->InclusionList.pguidBuffer = pbGuidBuffer;
            pAsfContext->fInclusionListValid = TRUE;
        }
        break;
    }

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
** Function :   _RetrieveLicense
**
** Synopsis :   Aggregates calls into WMDRM-PD SDK to DRM_MGR_GetLicenseData,
**              DRM_MGR_GetLicenseState, and DRM_MGR_Bind to the license,
**              collecting license information in preparation to send to the
**              WMDRM-ND receiver.
**
** Arguments :  [f_pNdtContentSessionContext] : The content session context
**              [f_pdstrAction]               : action being performed to test the license against.
**                                              WMDRMND only supports "PLAY"
**
** Returns:     DRM_SUCCESS on success
**              DRM_xxxx depending on failure
**
******************************************************************************/
static DRM_RESULT _RetrieveLicense(
    __in       NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT * const f_pNdtContentSessionContext,
    __in const DRM_CONST_STRING                              *f_pdstrAction
)
{
    DRM_RESULT                      dr          = DRM_SUCCESS;
    WMDRMNET_POLICY_TRANSCRYPTPLAY  policyTranscryptPlay;
    DRM_DWORD                       fBlockNDT   = FALSE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__RetrieveLicense );

    ZEROMEM( &policyTranscryptPlay, SIZEOF( policyTranscryptPlay ) );

    ChkArg( f_pdstrAction != NULL );

    /* The playback right was already verified at license request time. So here
     * we're just making sure nothing has changed as we only support the
     * playback right at this point.
     */
    DRMASSERT( DRM_UTL_DSTRStringsEqual( f_pdstrAction,
                                        &g_dstrDRM_RIGHT_PLAYBACK ) );

    if (!DRM_UTL_DSTRStringsEqual( f_pdstrAction, &g_dstrWMDRM_RIGHT_PLAYBACK ))
    {
        ChkDR(DRM_E_INVALID_RIGHT);
    }

    dr = Drm_Reader_Bind( f_pNdtContentSessionContext->pAppContext,
                          &f_pdstrAction,
                          1,
                          _OutputLevelsCallback,
                          &f_pNdtContentSessionContext->asfContext,
                          &f_pNdtContentSessionContext->asfContext.decryptContext );

    if( DRM_FAILED(dr) )
    {
        /* License may not be available */
        f_pNdtContentSessionContext->asfContext.dwLicenseStatus = NDT_DRM_LICENSE_NOT_AVAILABLE;
        TRACE(("Drm_Reader_Bind: failed 0x%x, license may not be available\n",dr));
        goto ErrorExit;
    }

    ChkDR( Drm_License_GetProperty( f_pNdtContentSessionContext->pAppContext,
                                    DRM_LGP_BLOCK_NDT,
                                    NULL,
                                    NULL,
                                    &fBlockNDT ) );
    if( fBlockNDT )
    {
        /* Cannot use the bound license */
        dr = DRM_E_RIGHTS_NOT_AVAILABLE;
        f_pNdtContentSessionContext->asfContext.dwLicenseStatus = NDT_DRM_LICENSE_NOT_AVAILABLE;
        TRACE(("Drm_Reader_Bind: failed 0x%x, license is explicitly blocked from being used for NDT\n",dr));
        goto ErrorExit;
    }

    f_pNdtContentSessionContext->asfContext.dwLicenseStatus = NDT_DRM_LICENSE_ALLOWS_PLAYBACK;

ErrorExit:
    if( DRM_FAILED( dr ) )
    {
        /*
        ** If this function has failed, make sure the decryption context is released
        */
        Drm_Reader_Close( &f_pNdtContentSessionContext->asfContext.decryptContext );
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



DRM_VOID DRM_CALL DRM_NDT_License_FreePolicy(
    __out WMDRMNET_POLICY *f_pPolicy
)
{
    WMDRMNET_POLICY_TRANSCRYPTPLAY  *pPolicyTranscryptPlay = NULL;

    DRMASSERT( f_pPolicy != NULL );
    __analysis_assume( f_pPolicy != NULL );

    if ( f_pPolicy->ePolicyType == WMDRMNET_POLICY_TYPE_TRANSCRYPTPLAY )
    {
        pPolicyTranscryptPlay = (WMDRMNET_POLICY_TRANSCRYPTPLAY *) f_pPolicy->pbPolicy;
        SAFE_OEM_FREE( pPolicyTranscryptPlay->globals.InclusionList.pguidBuffer );
        SAFE_OEM_FREE( f_pPolicy );
    }
}



DRM_RESULT DRM_CALL DRM_NDT_License_CreateDefaultPolicy(
    __in  NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT  * const f_pContSessContext,
    __deref_out_bcount(*f_pcbPolicy) WMDRMNET_POLICY    **f_ppPolicy,
    __out DRM_DWORD                               * const f_pcbPolicy
)
{
    DRM_RESULT                       dr = DRM_SUCCESS;
    DRM_DWORD                        cbPolicy = 0;
    DRM_DWORD                        dwMinAppSec = 0;
    DRM_DWORD                        dwRiv = 0;
    WMDRMNET_POLICY                 *pPolicy = NULL;
    WMDRMNET_POLICY_TRANSCRYPTPLAY  *pPolicyTranscryptPlay = NULL;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_DRM_NDT_License_CreateDefaultPolicy );

    ChkArg( f_pContSessContext != NULL );
    ChkArg( f_ppPolicy         != NULL );
    ChkArg( f_pcbPolicy        != NULL );

    /* Create policy and populate policy */
    cbPolicy = SIZEOF( WMDRMNET_POLICY ) +
               SIZEOF( WMDRMNET_POLICY_TRANSCRYPTPLAY );
    pPolicy = (WMDRMNET_POLICY*)Oem_MemAlloc( cbPolicy );
    ChkMem( pPolicy );
    ZEROMEM( pPolicy, cbPolicy );

    pPolicyTranscryptPlay = (WMDRMNET_POLICY_TRANSCRYPTPLAY *) pPolicy->pbPolicy;
    pPolicy->ePolicyType = WMDRMNET_POLICY_TYPE_TRANSCRYPTPLAY;

    /* Get License Min App Security */
    if ( f_pContSessContext->asfContext.fEncryptClear )
    {
        dr = OEM_NDT_GetWMDRMNETMinAppSec( &dwMinAppSec );
    }
    else
    {
        dr = Drm_License_GetProperty( f_pContSessContext->pAppContext,
                                      DRM_LGP_MIN_APP_SEC,
                                      NULL,
                                      NULL,
                                      &dwMinAppSec );
    }
    if( DRM_FAILED(dr) )
    {
        TRACE(("Get Min App Security Level failed 0x%x\n",dr));
        ChkDR(dr);
    }
    pPolicyTranscryptPlay->globals.MinimumEnvironment.wMinimumSecurityLevel = (DRM_WORD) dwMinAppSec;

    /* Get OPLs */
    if ( TRUE == f_pContSessContext->asfContext.fOPLValid )
    {
        MEMCPY( &pPolicyTranscryptPlay->playOPLs.minOPL,
                &f_pContSessContext->asfContext.playOPLs.minOPL,
                SIZEOF( f_pContSessContext->asfContext.playOPLs.minOPL ) );
    }
    else
    {
        pPolicyTranscryptPlay->playOPLs.minOPL.wAnalogVideo = (DRM_WORD) 100;
        pPolicyTranscryptPlay->playOPLs.minOPL.wCompressedDigitalAudio = (DRM_WORD) 100;
        pPolicyTranscryptPlay->playOPLs.minOPL.wCompressedDigitalVideo = (DRM_WORD) 100;
        pPolicyTranscryptPlay->playOPLs.minOPL.wUncompressedDigitalAudio = (DRM_WORD) 100;
        pPolicyTranscryptPlay->playOPLs.minOPL.wUncompressedDigitalVideo = (DRM_WORD) 100;
    }

    /* Get Source ID */
    if ( f_pContSessContext->asfContext.fEncryptClear )
    {
        ChkDR( OEM_NDT_GetSourceID( &pPolicyTranscryptPlay->globals.dwSourceID ) );
    }
    else
    {
        ChkDR( Drm_License_GetProperty(
                   f_pContSessContext->pAppContext,
                   DRM_LGP_SOURCE_ID,
                   NULL,
                   NULL,
                   &pPolicyTranscryptPlay->globals.dwSourceID ) );
    }

    /* Get Inclusion List */
    if( f_pContSessContext->asfContext.fInclusionListValid && f_pContSessContext->asfContext.InclusionList.cGUIDs > 0)
    {
        ChkMem( pPolicyTranscryptPlay->globals.InclusionList.pguidBuffer = (DRM_BYTE*)Oem_MemAlloc( SIZEOF(DRM_GUID) * f_pContSessContext->asfContext.InclusionList.cGUIDs ) );
        MEMCPY( pPolicyTranscryptPlay->globals.InclusionList.pguidBuffer,
                f_pContSessContext->asfContext.InclusionList.pguidBuffer,
                SIZEOF(DRM_GUID) * f_pContSessContext->asfContext.InclusionList.cGUIDs );
        pPolicyTranscryptPlay->globals.InclusionList.cGUIDs = f_pContSessContext->asfContext.InclusionList.cGUIDs;
    }

    /* Get Revocation Information Version */
    if ( f_pContSessContext->asfContext.fEncryptClear )
    {
        ChkDR( OEM_NDT_GetLicenseRevInfoVersion( &dwRiv ) );
    }
    else
    {
        ChkDR( Drm_License_GetProperty( f_pContSessContext->pAppContext,
                                        DRM_LGP_REVINFO_VERSION,
                                        NULL,
                                        NULL,
                                        &dwRiv ) );
    }
    pPolicyTranscryptPlay->globals.dwRiv = dwRiv;

    *f_ppPolicy = pPolicy;
    pPolicy = NULL;
    *f_pcbPolicy = cbPolicy;

ErrorExit:
    SAFE_OEM_FREE(pPolicy);
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



DRM_RESULT DRM_CALL DRM_NDT_License_SetRootPolicy(
    __in  const NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT  * const pContentSession,
    __out       WMDRMNET_POLICY                         * const pPolicy
)
{
    WMDRMNET_POLICY_TRANSCRYPTPLAY *pPolicyTranscryptPlay = NULL;
    DRM_RESULT dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_NDT_License_SetRootPolicy );

    ChkArg( pContentSession != NULL );
    ChkArg( pPolicy         != NULL );

    ZEROMEM( pPolicy, SIZEOF( WMDRMNET_POLICY_TRANSCRYPTPLAY ) );

    pPolicy->ePolicyType = WMDRMNET_POLICY_TYPE_TRANSCRYPTPLAY;

    pPolicyTranscryptPlay = (WMDRMNET_POLICY_TRANSCRYPTPLAY *)pPolicy->pbPolicy;
    pPolicyTranscryptPlay->globals.MinimumEnvironment.wMinimumSecurityLevel = (DRM_WORD)pContentSession->dwReceiverSecurityLevel;
    pPolicyTranscryptPlay->globals.MinimumEnvironment.dwMinimumAppRevocationListVersion = 0;
    pPolicyTranscryptPlay->globals.MinimumEnvironment.dwMinimumDeviceRevocationListVersion = 0;

    pPolicyTranscryptPlay->playOPLs.minOPL.wCompressedDigitalVideo   = 0;
    pPolicyTranscryptPlay->playOPLs.minOPL.wUncompressedDigitalVideo = 0;
    pPolicyTranscryptPlay->playOPLs.minOPL.wAnalogVideo              = 0;
    pPolicyTranscryptPlay->playOPLs.minOPL.wCompressedDigitalAudio   = 0;
    pPolicyTranscryptPlay->playOPLs.minOPL.wUncompressedDigitalAudio = 0;

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


DRM_RESULT DRM_CALL DRM_NDT_License_GenerateKeys(
    __in_opt        DRM_VOID                               *f_pOEMContext,
    __in     const  NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *f_pNdtContentSessionContext,
    __inout         NDT_LICENSE_CONTEXT                    *f_pLicContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_DRM_NDT_License_GenerateKeys );

    ChkArg( f_pNdtContentSessionContext != NULL );
    ChkArg( f_pLicContext               != NULL );

    /* Generate a new KID */
    Oem_Random_GetBytes( f_pOEMContext, (DRM_BYTE*)&f_pLicContext->idKID, SIZEOF(DRM_ID) );

    /* Generate a new LID/rights ID */
    Oem_Random_GetBytes( f_pOEMContext, (DRM_BYTE*)&f_pLicContext->idRightsID, SIZEOF(DRM_ID) );

    /* Generate new content encryption and integrity keys */
    Oem_Random_GetBytes( f_pOEMContext, f_pLicContext->rgbKeyMaterial, 2 * DRM_AES_KEYSIZE_128 );

    f_pLicContext->pbKeyContentEncryption = (DRM_BYTE*)f_pLicContext->rgbKeyMaterial;
    f_pLicContext->pbKeyContentIntegrity = (DRM_BYTE*)(f_pLicContext->rgbKeyMaterial + __CB_DECL(DRM_AES_KEYSIZE_128));

    ChkMem( f_pLicContext->pAESKeyContentEncryption = (DRM_AES_KEY*)Oem_MemAlloc( SIZEOF( DRM_AES_KEY ) ) );
    ChkDR( Oem_Aes_SetKey(f_pLicContext->pbKeyContentEncryption, f_pLicContext->pAESKeyContentEncryption ) );

ErrorExit:
    TRACE_IF_FAILED(("DRM_NDT_License_GenerateKeys 0x%x\n", dr));
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/******************************************************************************
** Function :   NDT_RetrieveLicenseDataV1
**
** Synopsis:    Places License data in License Context in DRM manager context from
**              license store keyed off ASF Encryption Object V1 Header
**
** Arguments :  [f_pNdtContentSessionContext] -- The content session context
**              [f_pbKeyID] -- KeyID byte pointer array
**              [f_cbKeyID] -- size of KeyID byte pointer array in bytes
**              [f_pbSecretData] -- Secret Data byte pointer array
**              [f_cbSecretData] -- size of Secret Data byte pointer array in bytes
**              [f_pbURL] -- URL byte pointer array
**              [f_cbURL] -- size of URL byte pointer array in bytes
**              [f_pdstrAction] -- type of usage requested by receiver, currently, only "Play" is supported.
**
** Returns:     DRM_SUCCESS on success
**              DRM_xxxx depending on failure
**
******************************************************************************/
DRM_RESULT DRM_CALL NDT_RetrieveLicenseDataV1(
    __in NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT        * const f_pNdtContentSessionContext,
    __in_bcount(f_cbKeyID)      const DRM_BYTE         * const f_pbKeyID,
    __in                        const DRM_DWORD                f_cbKeyID,
    __in_bcount(f_cbSecretData) const DRM_BYTE         * const f_pbSecretData,
    __in                        const DRM_DWORD                f_cbSecretData,
    __in_bcount(f_cbURL)        const DRM_BYTE         * const f_pbURL,
    __in                        const DRM_DWORD                f_cbURL,
    __in                        const DRM_CONST_STRING * const f_pdstrAction
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CSP_V1_HEADER_DATA cspv1Header = {0};


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_NDT_RetrieveLicenseDataV1 );

    ChkArg( f_pbKeyID      != NULL );
    ChkArg( f_pbSecretData != NULL );
    ChkArg( f_pbURL        != NULL );

    cspv1Header.pbKeyID = f_pbKeyID;
    cspv1Header.cbKeyID = f_cbKeyID;
    cspv1Header.pbSecretData = f_pbSecretData;
    cspv1Header.cbSecretData = f_cbSecretData;
    cspv1Header.pbURL = f_pbURL;
    cspv1Header.cbURL = f_cbURL;

    ChkDR(Drm_Content_SetProperty( f_pNdtContentSessionContext->pAppContext,
                                   DRM_CSP_V1_HEADER,
                                   (DRM_BYTE*)&cspv1Header,
                                   SIZEOF( cspv1Header )));

    if(DRM_FAILED(dr))
    {
        TRACE_IF_FAILED(("DRM_Content_SetProperty failed 0x%x\n",dr));
        ChkDR(dr);
    }

    ChkDR( _RetrieveLicense( f_pNdtContentSessionContext, f_pdstrAction ) );

ErrorExit:
    TRACE_IF_FAILED(("NDT_RetrieveLicenseDataV1 failed 0x%x\n",dr));
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}


/******************************************************************************
**
** Function :   NDT_RetrieveLicenseDataV2
**
** Synopsis:    Places License data in License Context in DRM manager context from
**              license store keyed off ASF Encryption Object V2 Header
**
** Arguments:   [f_pNdtContentSessionContext] -- The content session context
**              [f_pbContentEncryptionObject] -- ASF Encryption Header byte pointer array
**              [f_cbContentEncryptionObject] -- size of ASF Encryption Header byte pointer array in bytes
**              [f_pdstrAction] -- type of usage requested by receiver, currently, only "Play" is supported.
**
** Returns:     DRM_SUCCESS on success
**              DRM_xxxx depending on failure
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL NDT_RetrieveLicenseDataV2(
    __in       NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT        * const f_pNdtContentSessionContext,
    __in_bcount(f_cbContentEncryptionObject) const DRM_BYTE  * const f_pbContentEncryptionObject,
    __in const DRM_DWORD                                             f_cbContentEncryptionObject,
    __in const DRM_CONST_STRING                              * const f_pdstrAction
)
{
    DRM_RESULT dr = DRM_SUCCESS;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_NDT_RetrieveLicenseDataV2 );

    ChkArg( f_pbContentEncryptionObject != NULL );

    dr = Drm_Content_SetProperty( f_pNdtContentSessionContext->pAppContext,
                                  DRM_CSP_AUTODETECT_HEADER,
                                  f_pbContentEncryptionObject,
                                  f_cbContentEncryptionObject );
    if(DRM_FAILED(dr))
    {
        TRACE_IF_FAILED(("Drm_Content_SetProperty failed 0x%x\n",dr));
        ChkDR(dr);
    }

    ChkDR( _RetrieveLicense( f_pNdtContentSessionContext, f_pdstrAction ) );

ErrorExit:
    TRACE_IF_FAILED(("NDT_RetrieveLicenseDataV2 failed 0x%x\n",dr));

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



/******************************************************************************
**
** Function : NDT_RetrieveLicenseDataPR
**
** Synopsis:  Places License data in License Context in DRM manager context from
**            license store keyed off ASF PlayReady header object.
**
** Arguments: [f_pNdtContentSessionContext] -- The content session context
**            [f_pbPlayReadyObject]         -- PlayReady Object byte pointer array
**            [f_cbPlayReadyObject]         -- size of PlayReady Object byte pointer array (in bytes)
**            [f_pdstrAction]               -- type of usage requested by receiver, currently, only "Play" is supported.
**
** Returns:   DRM_SUCCESS on success
**            DRM_xxxx depending on failure
**
******************************************************************************/
DRM_RESULT DRM_CALL NDT_RetrieveLicenseDataPR(
    __in       NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT * const f_pNdtContentSessionContext,
    __in_bcount(f_cbPlayReadyObject) const DRM_BYTE   * const f_pbPlayReadyObject,
    __in const DRM_DWORD                                      f_cbPlayReadyObject,
    __in const DRM_CONST_STRING                       * const f_pdstrAction
)
{
    DRM_RESULT dr = DRM_SUCCESS;


    ChkArg( f_pNdtContentSessionContext != NULL );
    ChkArg( f_pbPlayReadyObject         != NULL );
    ChkArg( f_pdstrAction               != NULL );

    dr = Drm_Content_SetProperty( f_pNdtContentSessionContext->pAppContext,
                                  DRM_CSP_PLAYREADY_OBJ,
                                  f_pbPlayReadyObject,
                                  f_cbPlayReadyObject );
    if( DRM_FAILED( dr ) )
    {
        TRACE(( "Drm_Content_SetProperty failed 0x%x\n", dr ));
        ChkDR( dr );
    }

    ChkDR( _RetrieveLicense( f_pNdtContentSessionContext, f_pdstrAction ) );

ErrorExit:
    TRACE_IF_FAILED(( "NDT_RetrieveLicenseDataPR failed 0x%x\n", dr ));
    return dr;
}



/******************************************************************************
** Function :   _DRM_XMR_RootSignatureCallback
**
** Synopsis :   Signs the XMR license to be sent to the WMDRM-ND receiver using
**              the WMDRM-ND receiver's RSA-1024 public key sent during the
**              registration process in the WMDRM-ND receiver's device certificate
**              This is implementation of DRM_XMR_CB_SIGNATURE callback function
**
** Arguments :  [f_pvSignatureContext]    : Context needed by caller to retrieve
**                                          appropriate integrity key
**              [f_pbData]                : Data to be signed
**              [f_cbData]                : Length of pbData
**              [f_pbSignatureBuffer]     : Buffer in which to put the signature
**              [f_cbSignatureBuffer]     : Length of signature buffer
**              [f_ibSignatureBuffer]     : Index in buffer at which signature should be put
**
** Returns:     DRM_SUCCESS on success
**              DRM_xxxx depending on failure
**
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL _DRM_XMR_RootSignatureCallback(
    __in                                const DRM_VOID  *f_pvSignatureContext,
    __in_bcount(f_cbData)               const DRM_BYTE  *f_pbData,
    __in                                const DRM_DWORD  f_cbData,
    __out_bcount( f_cbSignatureBuffer )       DRM_BYTE  *f_pbSignatureBuffer,
    __in                                const DRM_DWORD  f_cbSignatureBuffer,
    __in                                const DRM_DWORD  f_ibSignatureBuffer
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *pContentSesContext = (NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT*) f_pvSignatureContext;
    const NDT_LICENSE_CONTEXT *pLicContext = NULL;
    DRM_AES_KEY *pKey = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__DRM_XMR_RootSignatureCallback );

    DRMASSERT( NULL != pContentSesContext->pLicenseContext );
    __analysis_assume( pContentSesContext->pLicenseContext != NULL );
    switch( pContentSesContext->licenseMode )
    {
    case NDT_LICENSE_MODE_SIMPLE:
        pLicContext = (const NDT_LICENSE_CONTEXT *) pContentSesContext->pLicenseContext;
        break;
    case NDT_LICENSE_MODE_CHAINED:
        pLicContext = &(((NDT_CHAINED_LICENSE_CONTEXT *) pContentSesContext->pLicenseContext)->rootLicenseContext);
        break;
    default:
        ChkDR( DRM_E_NOTIMPL );
    }
    ChkArg( NULL != pLicContext );
    ChkArg( f_cbSignatureBuffer - f_ibSignatureBuffer == DRM_AES_BLOCKLEN );

    ChkMem( pKey = (DRM_AES_KEY*)Oem_MemAlloc( SIZEOF( DRM_AES_KEY ) ) );
    ChkDR( Oem_Aes_SetKey(pLicContext->pbKeyContentIntegrity, pKey ) );

#if SIXTEEN_BIT_ADDRESSING
    if( f_ibSignatureBuffer % CB_NATIVE_BYTE != 0 )
    {
        DRM_BYTE rgbSignature[__CB_DECL( DRM_AES_BLOCKLEN )];

        ChkDR( Oem_Omac1_Sign( pKey,
                               f_pbData,
                               0,
                               f_cbData,
                               rgbSignature ) );
        DRM_BYT_CopyBytes( f_pbSignatureBuffer,
                           f_ibSignatureBuffer,
                           rgbSignature,
                           0,
                           SIZEOF( rgbSignature ) - f_ibSignatureBuffer );
    }
    else
    {
#endif /* SIXTEEN_BIT_ADDRESSING */
        ChkDR( Oem_Omac1_Sign( pKey,
                               f_pbData,
                               0,
                               f_cbData,
                               f_pbSignatureBuffer + __CB_DECL(f_ibSignatureBuffer) ) );
#if SIXTEEN_BIT_ADDRESSING
    }
#endif /* SIXTEEN_BIT_ADDRESSING */
ErrorExit:
    TRACE_IF_FAILED(("_DRM_XMR_RootSignatureCallback failed 0x%x\n",dr));

    if(pKey != NULL)
    {
        Oem_Aes_ZeroKey(pKey);
        SAFE_OEM_FREE(pKey);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
** Function :   _DRM_XMR_LeafSignatureCallback
**
** Synopsis :   Signs the XMR license to be sent to the WMDRM-ND receiver using
**              the WMDRM-ND receiver's RSA-1024 public key sent during the
**              registration process in the WMDRM-ND receiver's device certificate
**
** Arguments :  [f_pvSignatureContext]    : Context needed by caller to retrieve
**                                          appropriate integrity key
**              [f_pbData]                : Data to be signed
**              [f_cbData]                : Lentgth of pbData
**              [f_pbSignatureBuffer]     : Buffer in which to put the signature
**              [f_cbSignatureBuffer]     : Length of signature buffer
**              [f_ibSignatureBuffer]      : Index in buffer at which signature should
**                                        be put
**
** Returns:     DRM_SUCCESS on success
**              DRM_xxxx depending on failure
**
******************************************************************************/
static DRM_RESULT DRM_CALL _DRM_XMR_LeafSignatureCallback(
    __in                                const DRM_VOID  *f_pvSignatureContext,
    __in_bcount( f_cbData )             const DRM_BYTE  *f_pbData,
    __in                                const DRM_DWORD  f_cbData,
    __out_bcount( f_cbSignatureBuffer )       DRM_BYTE  *f_pbSignatureBuffer,
    __in                                const DRM_DWORD  f_cbSignatureBuffer,
    __in                                const DRM_DWORD  f_ibSignatureBuffer
)
{
    DRM_RESULT           dr          = DRM_SUCCESS;
    const NDT_LICENSE_CONTEXT *pLicContext = NULL;
    DRM_AES_KEY         *pKey        = NULL;
    NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *pContentSesContext = (NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT*) f_pvSignatureContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC__DRM_XMR_LeafSignatureCallback );

    ChkArg( f_cbSignatureBuffer - f_ibSignatureBuffer == DRM_AES_BLOCKLEN );

    DRMASSERT( NDT_LICENSE_MODE_CHAINED == pContentSesContext->licenseMode );
    DRMASSERT( NULL != pContentSesContext->pLicenseContext );
    __analysis_assume( pContentSesContext->pLicenseContext != NULL );
    pLicContext = &(((NDT_CHAINED_LICENSE_CONTEXT *) pContentSesContext->pLicenseContext)->leafLicenseContext);

    ChkMem( pKey = (DRM_AES_KEY*)Oem_MemAlloc( SIZEOF( DRM_AES_KEY ) ) );
    ChkDR( Oem_Aes_SetKey(pLicContext->pbKeyContentIntegrity, pKey ) );

#if SIXTEEN_BIT_ADDRESSING
    if( f_ibSignatureBuffer % CB_NATIVE_BYTE != 0 )
    {
        DRM_BYTE rgbSignature[__CB_DECL( DRM_AES_BLOCKLEN )];

        ChkDR( Oem_Omac1_Sign( pKey,
                               f_pbData,
                               0,
                               f_cbData,
                               rgbSignature ) );
        DRM_BYT_CopyBytes( f_pbSignatureBuffer, f_ibSignatureBuffer, rgbSignature, 0, SIZEOF( rgbSignature ) );
    }
    else
    {
#endif /* SIXTEEN_BIT_ADDRESSING */
        ChkDR( Oem_Omac1_Sign( pKey,
                               f_pbData,
                               0,
                               f_cbData,
                               f_pbSignatureBuffer + __CB_DECL(f_ibSignatureBuffer) ) );
#if SIXTEEN_BIT_ADDRESSING
    }
#endif /* SIXTEEN_BIT_ADDRESSING */
ErrorExit:
    TRACE_IF_FAILED(("_DRM_XMR_LeafSignatureCallback failed 0x%x\n",dr));

    if(pKey != NULL)
    {
        Oem_Aes_ZeroKey(pKey);
        SAFE_OEM_FREE(pKey);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
** Function :   NDT_License_BuildSimpleXMR
**
** Synopsis:    Builds an XMR License from the License data read from NDT_RetrieveLicenseData
**              which must be called first
**
** Arguments:   [f_pNdtMgrContext]            -- The NDT DRM manager context
**              [f_pNdtContentSessionContext] -- Content session context
**              [f_pPolicy]                   -- Policy
**              [f_idSerial]                  -- Serial number
**              [f_pCertificateInfo]          -- certificate info
**              [f_ppbXMR]                    -- XMR License in a byte array
**              [f_pcbXMR]                    -- size of XMR License
**
** Returns:     DRM_SUCCESS on success
**              DRM_xxxx depending on failure
**
******************************************************************************/
DRM_RESULT DRM_CALL NDT_License_BuildSimpleXMR(
    __in const  NDTMGR_CONTEXT                          * const f_pNdtMgrContext,
    __in const  NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT  * const f_pNdtContentSessionContext,
    __in const  WMDRMNET_POLICY                         * const f_pPolicy,
    __in const  DRM_ID                                          f_idSerial,
    __in const  NDT_CERTIFICATE_INFO                    * const f_pCertificateInfo,
    __deref_out_bcount(*f_pcbXMR) DRM_BYTE                    **f_ppbXMR,
    __out       DRM_DWORD                                      *f_pcbXMR
)
{
    DRM_RESULT                      dr = DRM_SUCCESS;
    const NDTMGR_CONTEXT_INTERNAL  *pContext = (const NDTMGR_CONTEXT_INTERNAL*) f_pNdtMgrContext;
    NDT_LICENSE_CONTEXT            *pLicContext;
    DRM_BYTE                        rgbXMRBuilder[__CB_DECL(1024)];
    DRM_DWORD                       cbXMRBuilder = SIZEOF( rgbXMRBuilder );
    DRM_XMR_BUILDER_CONTEXT         xmrBuilder;
    DRM_XMR_SOURCEID                xmr_sourceID;
    DRM_XMR_CONTENT_KEY             contentKey;
    DRM_XMR_DEVICE_KEY              deviceKey;
    DRM_XMR_MINIMUM_ENVIRONMENT     xmrMinEnv;
    DRM_XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS xmrMinOpls;
    DRM_XMR_SERIAL_NUMBER           xmrSerialNumber;
    DRM_XMR_INCLUSION_LIST          xmrInclusionList;
    DRM_XMR_REVOCATION_INFORMATION_VERSION xmrRiv;
    DRM_DWORD                       cbKeyMaterial = 0;
    DRM_BYTE                        rgbKeyMaterial[DRM_RSA_CB_MODULUS_MAX_2048BIT] = {0};
    DRM_BYTE                       *pbXMR = NULL;
    DRM_DWORD                       cbXMR;
    DRM_XMR_RIGHTS                  Settings;
    WMDRMNET_POLICY_TRANSCRYPTPLAY *pPolicyTranscryptPlay;
    DRM_CRYPTO_CONTEXT             *poCrypto = NULL;
    DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT   eDeviceKeyLength = eDRM_RSA_KEY_LENGTH_UNKNOWN_2048BIT;
    DRM_DWORD                       cbDeviceExponent = SIZEOF( deviceKey.dwExponent );
    DRM_BYTE                        rgbDeviceModulus[DRM_RSA_CB_MODULUS_MAX_2048BIT] = {0};
    DRM_DWORD                       cbDeviceModulus = SIZEOF( rgbDeviceModulus );

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_NDT_License_BuildSimpleXMR );

    ChkMem( poCrypto = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( poCrypto, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    ChkArg( f_pNdtMgrContext && f_pCertificateInfo && f_ppbXMR && f_pcbXMR );
    ChkArg( f_pPolicy &&
            f_pPolicy->ePolicyType == WMDRMNET_POLICY_TYPE_TRANSCRYPTPLAY );

    DRMASSERT( NULL != f_pNdtContentSessionContext->pLicenseContext );
    __analysis_assume( f_pNdtContentSessionContext->pLicenseContext != NULL );
    DRMASSERT( NDT_LICENSE_MODE_SIMPLE == f_pNdtContentSessionContext->licenseMode );

    pLicContext = (NDT_LICENSE_CONTEXT *) f_pNdtContentSessionContext->pLicenseContext;

    pPolicyTranscryptPlay = (WMDRMNET_POLICY_TRANSCRYPTPLAY *)f_pPolicy->pbPolicy;

    ChkDR( DRM_XMR_StartLicense( rgbXMRBuilder, cbXMRBuilder, &pLicContext->idRightsID, XMR_VERSION_1, &xmrBuilder ) );

    /*
     * Add the device key object so we can tell which pubkey this content is bound to
     */

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT, &deviceKey ) );

    ChkDR( OEM_RSA_ParsePublicKey_2048BIT(       f_pCertificateInfo->pDevicePublicKey,
                                                &eDeviceKeyLength,
                                                &cbDeviceExponent,
                                   ( DRM_BYTE* )&deviceKey.dwExponent,
                                                &cbDeviceModulus,
                                                 rgbDeviceModulus ) );
    deviceKey.pbModulusBuffer = rgbDeviceModulus;
    deviceKey.fValid    = TRUE;
    deviceKey.iModulus  = 0;
    deviceKey.cbModulus = ( DRM_WORD )cbDeviceModulus;

    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT, &deviceKey ) );

    /*
     * Add the settings object
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SETTINGS_OBJECT, &Settings ) );
    Settings.fValid = TRUE;
    Settings.wValue = XMR_SETTINGS_FLAG_CANNOT_PERSIST;
    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_SETTINGS_OBJECT, &Settings ) );

    /*
     *  Add the SourceID from the WMDRM-PD license to the XMR license
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SOURCEID_OBJECT, &xmr_sourceID ) );
    xmr_sourceID.dwValue = pPolicyTranscryptPlay->globals.dwSourceID;
    xmr_sourceID.fValid = TRUE;
    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_SOURCEID_OBJECT, &xmr_sourceID ) );

    /*
     *  Add the inclusion list.
     */
    if ( 0 != pPolicyTranscryptPlay->globals.InclusionList.cGUIDs )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT, &xmrInclusionList ) );
        xmrInclusionList.cGUIDs      = pPolicyTranscryptPlay->globals.InclusionList.cGUIDs;
        xmrInclusionList.pguidBuffer = pPolicyTranscryptPlay->globals.InclusionList.pguidBuffer;
        xmrInclusionList.iGuids      = 0;
        xmrInclusionList.fValid      = TRUE;
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT, &xmrInclusionList ) );
    }

    /*
     * Add the Riv
     */
    if ( 0 != pPolicyTranscryptPlay->globals.dwRiv )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT, &xmrRiv ) );
        xmrRiv.dwValue = pPolicyTranscryptPlay->globals.dwRiv;
        xmrRiv.fValid  = TRUE;
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_OBJECT, &xmrRiv ) );
    }

    /*
     * Add the encrypted content key to the license
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT, &contentKey ) );

    /*
     * Root licenses use RSA for the Key Encryption Cipher Type
     */
    cbKeyMaterial = SIZEOF( rgbKeyMaterial );
    OEM_RSA_OaepEncrypt_2048BIT(  NULL,
                                  f_pCertificateInfo->pDevicePublicKey,
                                  SIZEOF( pLicContext->rgbKeyMaterial ),
                                  pLicContext->rgbKeyMaterial,
                                 &cbKeyMaterial,
                                  rgbKeyMaterial,
                                  poCrypto );

    contentKey.fValid                   = TRUE;
    contentKey.pbguidKeyID              = (DRM_BYTE*)&pLicContext->idKID;
    contentKey.iguidKeyID               = 0;
    contentKey.wSymmetricCipherType     = XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR;
    contentKey.wKeyEncryptionCipherType = XMR_ASYMMETRIC_ENCRYPTION_TYPE_RSA_1024;
    contentKey.cbEncryptedKey           = ( DRM_WORD )cbKeyMaterial;
    contentKey.pbEncryptedKeyBuffer     = rgbKeyMaterial;
    contentKey.iEncryptedKey            = 0;

    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT, &contentKey ) );

    /*
     *  Add minimum security level for the content plus the minimum application and device
     *  revocation list versions.
     */

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT, &xmrMinEnv ) );

    xmrMinEnv.fValid                = TRUE;
    if(f_pPolicy->ePolicyType == WMDRMNET_POLICY_TYPE_TRANSCRYPTPLAY)
    {
        xmrMinEnv.wMinimumSecurityLevel = pPolicyTranscryptPlay->globals.MinimumEnvironment.wMinimumSecurityLevel;
        xmrMinEnv.dwMinimumAppRevocationListVersion = pPolicyTranscryptPlay->globals.MinimumEnvironment.dwMinimumAppRevocationListVersion;
        xmrMinEnv.dwMinimumDeviceRevocationListVersion = pPolicyTranscryptPlay->globals.MinimumEnvironment.dwMinimumDeviceRevocationListVersion;
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT, &xmrMinEnv ) );

    /*
     *  Add the serial number
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT, &xmrSerialNumber ) );
    xmrSerialNumber.fValid = TRUE;
    xmrSerialNumber.pbDataBuffer = (DRM_BYTE *)&f_idSerial;
    xmrSerialNumber.cbData = SIZEOF(DRM_ID);
    xmrSerialNumber.iData = 0;
    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT, &xmrSerialNumber ) );

    /*
     *  Add playback policy object
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT, &xmrMinOpls ) );

    xmrMinOpls.fValid = TRUE;
    xmrMinOpls.wAnalogVideo = pPolicyTranscryptPlay->playOPLs.minOPL.wAnalogVideo;
    xmrMinOpls.wCompressedDigitalAudio = pPolicyTranscryptPlay->playOPLs.minOPL.wCompressedDigitalAudio;
    xmrMinOpls.wCompressedDigitalVideo = pPolicyTranscryptPlay->playOPLs.minOPL.wCompressedDigitalVideo;
    xmrMinOpls.wUncompressedDigitalAudio = pPolicyTranscryptPlay->playOPLs.minOPL.wUncompressedDigitalAudio;
    xmrMinOpls.wUncompressedDigitalVideo = pPolicyTranscryptPlay->playOPLs.minOPL.wUncompressedDigitalVideo;

    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT, &xmrMinOpls ) );

    cbXMR = 0;
    dr = DRM_XMR_FinishLicense( &xmrBuilder,
                                 pContext,
                                 _DRM_XMR_RootSignatureCallback,
                                 NULL,
                                &cbXMR );
    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
    }

    ChkMem( pbXMR = (DRM_BYTE*)Oem_MemAlloc(cbXMR) );

    ChkDR( DRM_XMR_FinishLicense( &xmrBuilder,
                                  f_pNdtContentSessionContext,
                                   _DRM_XMR_RootSignatureCallback,
                                   pbXMR,
                                  &cbXMR ) );

    *f_ppbXMR = pbXMR;
    *f_pcbXMR = cbXMR;
    pbXMR = NULL;

ErrorExit:
    SAFE_OEM_FREE( poCrypto );
    TRACE_IF_FAILED(("NDT_BuildXMRLicense failed 0x%x\n",dr));

    SAFE_OEM_FREE( pbXMR );

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
** Function :   NDT_License_BuildRootXMR
**
** Synopsis:    Builds an XMR License from the License data read from NDT_RetrieveLicenseData
**              which must be called first
**
** Arguments:   [f_pNdtMgrContext]            -- The NDT DRM manager context
**              [f_pNdtContentSessionContext] -- Content session context
**              [f_pPolicy]                   -- Policy
**              [f_idSerial]                  -- Serial number
**              [f_pCertificateInfo]          -- certificate info
**              [f_ppbXMR]                    -- XMR License in a byte array
**              [f_pcbXMR]                    -- size of XMR License
**
** Returns:     DRM_SUCCESS on success
**              DRM_xxxx depending on failure
**
******************************************************************************/
DRM_RESULT DRM_CALL NDT_License_BuildRootXMR(
    __in const  NDTMGR_CONTEXT                          * const f_pNdtMgrContext,
    __in const  NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT  * const f_pNdtContentSessionContext,
    __in const  WMDRMNET_POLICY                         * const f_pPolicy,
    __in const  DRM_ID                                          f_idSerial,
    __in const  NDT_CERTIFICATE_INFO                    * const f_pCertificateInfo,
    __deref_out_bcount(*f_pcbXMR) DRM_BYTE                    **f_ppbXMR,
    __out       DRM_DWORD                                      *f_pcbXMR
)
{
    DRM_RESULT                      dr = DRM_SUCCESS;
    const NDTMGR_CONTEXT_INTERNAL  *pContext = (const NDTMGR_CONTEXT_INTERNAL*) f_pNdtMgrContext;
    NDT_LICENSE_CONTEXT            *pLicContext;
    DRM_BYTE                        rgbXMRBuilder[__CB_DECL(1024)];
    DRM_DWORD                       cbXMRBuilder = SIZEOF( rgbXMRBuilder );
    DRM_XMR_BUILDER_CONTEXT         xmrBuilder;
    DRM_XMR_CONTENT_KEY             contentKey;
    DRM_XMR_DEVICE_KEY              deviceKey;
    DRM_XMR_MINIMUM_ENVIRONMENT     xmrMinEnv;
    DRM_XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS xmrMinOpls;
    DRM_XMR_SERIAL_NUMBER           xmrSerialNumber;
    DRM_DWORD                       cbKeyMaterial = 0;
    DRM_BYTE                        rgbKeyMaterial[__CB_DECL(DRM_RSA_CB_MODULUS_MAX_2048BIT)] = {0};
    DRM_BYTE                       *pbXMR = NULL;
    DRM_DWORD                       cbXMR;
    DRM_XMR_RIGHTS                  Settings;
    const WMDRMNET_POLICY_TRANSCRYPTPLAY *pPolicyTranscryptPlay;
    DRM_CRYPTO_CONTEXT             *poCryptoCtx = NULL;
    DRM_RSA_SUPPORTED_KEY_LENGTHS_2048BIT   eDeviceKeyLength = eDRM_RSA_KEY_LENGTH_UNKNOWN_2048BIT;
    DRM_DWORD                       cbDeviceExponent = SIZEOF( deviceKey.dwExponent );
    DRM_BYTE                        rgbDeviceModulus[__CB_DECL(DRM_RSA_CB_MODULUS_MAX_2048BIT)] = {0};
    DRM_DWORD                       cbDeviceModulus = SIZEOF( rgbDeviceModulus );

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_NDT_License_BuildRootXMR );

    ChkArg( f_pNdtMgrContext && f_pCertificateInfo && f_ppbXMR && f_pcbXMR );
    ChkArg( f_pPolicy &&
            f_pPolicy->ePolicyType == WMDRMNET_POLICY_TYPE_TRANSCRYPTPLAY );

    DRMASSERT( NULL != f_pNdtContentSessionContext->pLicenseContext );
    __analysis_assume( f_pNdtContentSessionContext->pLicenseContext != NULL );
    DRMASSERT( NDT_LICENSE_MODE_CHAINED ==
               f_pNdtContentSessionContext->licenseMode );

    ChkMem( poCryptoCtx = (DRM_CRYPTO_CONTEXT*)Oem_MemAlloc( SIZEOF( DRM_CRYPTO_CONTEXT ) ) );
    ZEROMEM( poCryptoCtx, SIZEOF( DRM_CRYPTO_CONTEXT ) );

    pLicContext     = &(((NDT_CHAINED_LICENSE_CONTEXT *)
        f_pNdtContentSessionContext->pLicenseContext)->rootLicenseContext);

    pPolicyTranscryptPlay = (const WMDRMNET_POLICY_TRANSCRYPTPLAY *)
                             f_pPolicy->pbPolicy;

    ChkDR( DRM_XMR_StartLicense( rgbXMRBuilder,
                                 cbXMRBuilder,
                                 &pLicContext->idRightsID,
                                 XMR_VERSION_1,
                                 &xmrBuilder ) );

    /*
     * Add the encrypted content key to the license
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT,
                                     &contentKey ) );

    /*
     * Root licenses use RSA for the Key Encryption Cipher Type
     */
    cbKeyMaterial = SIZEOF( rgbKeyMaterial );
    OEM_RSA_OaepEncrypt_2048BIT(  NULL,
                                  f_pCertificateInfo->pDevicePublicKey,
                                  SIZEOF( pLicContext->rgbKeyMaterial ),
                                  pLicContext->rgbKeyMaterial,
                                 &cbKeyMaterial,
                                  rgbKeyMaterial,
                                  poCryptoCtx );

    contentKey.fValid                   = TRUE;
    contentKey.pbguidKeyID              = (DRM_BYTE*)&pLicContext->idKID;
    contentKey.iguidKeyID               = 0;
    contentKey.wSymmetricCipherType     =
        XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_ECB;
    contentKey.wKeyEncryptionCipherType =
        XMR_ASYMMETRIC_ENCRYPTION_TYPE_RSA_1024;
    contentKey.cbEncryptedKey           = ( DRM_WORD )cbKeyMaterial;
    contentKey.pbEncryptedKeyBuffer     = rgbKeyMaterial;
    contentKey.iEncryptedKey            = 0;

    ChkDR( DRM_XMR_AddObject( &xmrBuilder,
                              XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT,
                              &contentKey ) );

    /*
     * Add the device key object so we can tell which pubkey this content is bound to
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT,
                                     &deviceKey ) );

    ChkDR( OEM_RSA_ParsePublicKey_2048BIT(       f_pCertificateInfo->pDevicePublicKey,
                                                &eDeviceKeyLength,
                                                &cbDeviceExponent,
                                   ( DRM_BYTE* )&deviceKey.dwExponent,
                                                &cbDeviceModulus,
                                                 rgbDeviceModulus ) );
    deviceKey.pbModulusBuffer = rgbDeviceModulus;
    deviceKey.fValid    = TRUE;
    deviceKey.iModulus  = 0;
    deviceKey.cbModulus = ( DRM_WORD )cbDeviceModulus;

    ChkDR( DRM_XMR_AddObject( &xmrBuilder,
                              XMR_OBJECT_TYPE_RSA_DEVICE_KEY_OBJECT,
                              &deviceKey ) );

    /*
     *  Add playback policy object
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT, &xmrMinOpls ) );

    xmrMinOpls.fValid = TRUE;
    xmrMinOpls.wAnalogVideo =
        pPolicyTranscryptPlay->playOPLs.minOPL.wAnalogVideo;

    xmrMinOpls.wCompressedDigitalAudio =
        pPolicyTranscryptPlay->playOPLs.minOPL.wCompressedDigitalAudio;

    xmrMinOpls.wCompressedDigitalVideo =
        pPolicyTranscryptPlay->playOPLs.minOPL.wCompressedDigitalVideo;

    xmrMinOpls.wUncompressedDigitalAudio =
        pPolicyTranscryptPlay->playOPLs.minOPL.wUncompressedDigitalAudio;

    xmrMinOpls.wUncompressedDigitalVideo =
        pPolicyTranscryptPlay->playOPLs.minOPL.wUncompressedDigitalVideo;

    ChkDR( DRM_XMR_AddObject( &xmrBuilder,
                              XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT,
                              &xmrMinOpls ) );

    /*
     *  Add minimum security level for the content plus the minimum application and device
     *  revocation list versions.
     */

    ChkDR( DRM_XMR_InitializeObject(
               XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT,
               &xmrMinEnv ) );

    xmrMinEnv.fValid                = TRUE;
    if(f_pPolicy->ePolicyType == WMDRMNET_POLICY_TYPE_TRANSCRYPTPLAY)
    {
        xmrMinEnv.wMinimumSecurityLevel = pPolicyTranscryptPlay
            ->globals.MinimumEnvironment.wMinimumSecurityLevel;
        xmrMinEnv.dwMinimumAppRevocationListVersion = pPolicyTranscryptPlay
            ->globals.MinimumEnvironment.dwMinimumAppRevocationListVersion;
        xmrMinEnv.dwMinimumDeviceRevocationListVersion = pPolicyTranscryptPlay
            ->globals.MinimumEnvironment.dwMinimumDeviceRevocationListVersion;
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

    ChkDR( DRM_XMR_AddObject( &xmrBuilder,
                              XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT,
                              &xmrMinEnv ) );

    /*
     * Add the settings object
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SETTINGS_OBJECT,
                                     &Settings ) );
    Settings.fValid = TRUE;
    Settings.wValue = XMR_SETTINGS_FLAG_CANNOT_PERSIST;
    ChkDR( DRM_XMR_AddObject( &xmrBuilder,
                              XMR_OBJECT_TYPE_SETTINGS_OBJECT,
                              &Settings ) );

    /*
     *  Add the serial number
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT,
                                     &xmrSerialNumber ) );
    xmrSerialNumber.fValid = TRUE;
    xmrSerialNumber.pbDataBuffer = (DRM_BYTE *)&f_idSerial;
    xmrSerialNumber.cbData = SIZEOF(DRM_ID);
    xmrSerialNumber.iData = 0;
    ChkDR( DRM_XMR_AddObject( &xmrBuilder,
                              XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT,
                              &xmrSerialNumber ) );

    cbXMR = 0;
    dr = DRM_XMR_FinishLicense( &xmrBuilder,
                                 pContext,
                                 _DRM_XMR_RootSignatureCallback,
                                 NULL,
                                &cbXMR );
    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
    }

    ChkMem( pbXMR = (DRM_BYTE*)Oem_MemAlloc(cbXMR) );

    ChkDR( DRM_XMR_FinishLicense( &xmrBuilder,
                                  f_pNdtContentSessionContext,
                                   _DRM_XMR_RootSignatureCallback,
                                   pbXMR,
                                  &cbXMR ) );

    *f_ppbXMR = pbXMR;
    *f_pcbXMR = cbXMR;
    pbXMR = NULL;

ErrorExit:
    SAFE_OEM_FREE( poCryptoCtx );
    TRACE_IF_FAILED(("NDT_BuildXMRLicense failed 0x%x\n",dr));

    if(pbXMR != NULL)
    {
        Oem_MemFree(pbXMR);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


/******************************************************************************
** Function :   NDT_License_BuildLeafXMR
**
** Synopsis:    Builds an XMR License from the License data read from NDT_RetrieveLicenseData
**              which must be called first
**
** Arguments:   [f_pNdtMgrContext]            -- The NDT DRM manager context
**              [f_pNdtContentSessionContext] -- Content session context
**              [f_pPolicy]                   -- Policy
**              [f_ppbXMR]                    -- XMR License in a byte array
**              [f_pcbXMR]                    -- size of XMR License
**
** Returns:     DRM_SUCCESS on success
**              DRM_xxxx depending on failure
**
******************************************************************************/
DRM_RESULT DRM_CALL NDT_License_BuildLeafXMR(
    __in const  NDTMGR_CONTEXT                          * const f_pNdtMgrContext,
    __in const  NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT  * const f_pNdtContentSessionContext,
    __in const  WMDRMNET_POLICY                         * const f_pPolicy,
    __in const  DRM_ID                                          f_idSerial,
    __deref_out_bcount(*f_pcbXMR) DRM_BYTE                    **f_ppbXMR,
    __out       DRM_DWORD                                      *f_pcbXMR
)
{
    DRM_RESULT                      dr = DRM_SUCCESS;
    const NDTMGR_CONTEXT_INTERNAL  *pContext = NULL;
    NDT_LICENSE_CONTEXT            *pLicContext;
    NDT_LICENSE_CONTEXT            *pRootLicContext;
    DRM_XMR_SOURCEID                xmr_sourceID;
    DRM_BYTE                        rgbXMRBuilder[__CB_DECL(1024)];
    DRM_DWORD                       cbXMRBuilder = SIZEOF( rgbXMRBuilder );
    DRM_XMR_BUILDER_CONTEXT         xmrBuilder;
    DRM_XMR_CONTENT_KEY             contentKey;
    DRM_XMR_UPLINK_KID              uplinkKID;
    DRM_XMR_MINIMUM_ENVIRONMENT     xmrMinEnv;
    DRM_XMR_MINIMUM_OUTPUT_PROTECTION_LEVELS xmrMinOpls;
    DRM_XMR_SERIAL_NUMBER           xmrSerialNumber;
    DRM_XMR_INCLUSION_LIST          xmrInclusionList;
    DRM_XMR_REVOCATION_INFORMATION_VERSION xmrRiv;
    DRM_BYTE                       *pbKeyMaterial = NULL;
    DRM_DWORD                       cbKeyMaterial;
    DRM_BYTE                       *pbXMR = NULL;
    DRM_DWORD                       cbXMR;
    DRM_XMR_RIGHTS                  Settings;
    WMDRMNET_POLICY_TRANSCRYPTPLAY *pPolicyTranscryptPlay;
    DRM_BYTE                        rgbCheckSum[__CB_DECL(DRM_SHA1_DIGEST_LEN + 1)];
    DRM_SHA_CONTEXT                 contextSHA;
    DRM_DWORD                       i;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT, PERF_FUNC_NDT_License_BuildLeafXMR );

    ChkArg( f_pNdtMgrContext && f_ppbXMR && f_pcbXMR );
    ChkArg( f_pPolicy &&
            f_pPolicy->ePolicyType == WMDRMNET_POLICY_TYPE_TRANSCRYPTPLAY );

    pContext = (const NDTMGR_CONTEXT_INTERNAL*) f_pNdtMgrContext;

    DRMASSERT( NULL != f_pNdtContentSessionContext->pLicenseContext );
    __analysis_assume( f_pNdtContentSessionContext->pLicenseContext != NULL );
    DRMASSERT( NDT_LICENSE_MODE_CHAINED == f_pNdtContentSessionContext->licenseMode );

    pRootLicContext = &(((NDT_CHAINED_LICENSE_CONTEXT *) f_pNdtContentSessionContext->pLicenseContext)->rootLicenseContext);
    pLicContext     = &(((NDT_CHAINED_LICENSE_CONTEXT *) f_pNdtContentSessionContext->pLicenseContext)->leafLicenseContext);

    pPolicyTranscryptPlay = (WMDRMNET_POLICY_TRANSCRYPTPLAY *)f_pPolicy->pbPolicy;

    ChkDR( DRM_XMR_StartLicense( rgbXMRBuilder, cbXMRBuilder, &pLicContext->idRightsID, XMR_VERSION_1, &xmrBuilder ) );

    /*
     * Add the encrypted content key to the license
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT, &contentKey ) );

    /*
     * Leaf licenses use AES for the Key Encryption Cipher Type
     */

    cbKeyMaterial = DRM_AES_KEYSIZE_128 * 2;
    pbKeyMaterial = (DRM_BYTE *) Oem_MemAlloc( cbKeyMaterial );
    ChkMem( pbKeyMaterial );
    MEMCPY( pbKeyMaterial, pLicContext->rgbKeyMaterial, cbKeyMaterial );

    DRMASSERT( NULL != ((NDT_CHAINED_LICENSE_CONTEXT *) f_pNdtContentSessionContext->pLicenseContext)->rootLicenseContext.pAESKeyContentEncryption );
    __analysis_assume( ((NDT_CHAINED_LICENSE_CONTEXT *) f_pNdtContentSessionContext->pLicenseContext)->rootLicenseContext.pAESKeyContentEncryption != NULL );
    ChkDR( Oem_Aes_EcbEncryptData(
               ((NDT_CHAINED_LICENSE_CONTEXT *) f_pNdtContentSessionContext->pLicenseContext)->rootLicenseContext.pAESKeyContentEncryption,
               pbKeyMaterial,
               cbKeyMaterial ) );

    contentKey.fValid                   = TRUE;
    contentKey.pbguidKeyID              = (DRM_BYTE *) &pLicContext->idKID;
    contentKey.iguidKeyID               = 0;
    contentKey.wSymmetricCipherType     = XMR_SYMMETRIC_ENCRYPTION_TYPE_AES_128_CTR;
    contentKey.wKeyEncryptionCipherType = XMR_ASYMMETRIC_ENCRYPTION_TYPE_CHAINED_LICENSE;
    contentKey.cbEncryptedKey           = (DRM_WORD) cbKeyMaterial;
    contentKey.pbEncryptedKeyBuffer     = pbKeyMaterial;
    contentKey.iEncryptedKey            = 0;

    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_CONTENT_KEY_OBJECT, &contentKey ) );

    /*
     * Add Uplink KID to License
     */

    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_UPLINK_KID_OBJECT, &uplinkKID ) );
    uplinkKID.fValid                  = TRUE;
    uplinkKID.pbguidUplinkKID         = (DRM_BYTE *) &pRootLicContext->idKID;
    uplinkKID.iguidUplinkKID          = 0;
    uplinkKID.cbChainedCheckSum       = DRM_XMR_CHAINED_CHECKSUM_LENGTH;

    ZEROMEM( rgbCheckSum, SIZEOF(rgbCheckSum) );
    MEMCPY( rgbCheckSum, pRootLicContext->pbKeyContentEncryption, DRM_AES_KEYSIZE_128 );
    for (i = 0; i < DRM_XMR_CHAINED_CHECKSUM_SHA_ITERATIONS; i++)
    {
        DRM_SHA_Init( &contextSHA, eDRM_SHA_1 );
        DRM_SHA_Update( rgbCheckSum,
                        DRM_SHA1_DIGEST_LEN + 1,
                        &contextSHA );
        DRM_SHA_Finalize( &contextSHA, DRM_SHA1_DIGEST_LEN, rgbCheckSum );
    }

    uplinkKID.pbChainedCheckSumBuffer = rgbCheckSum;
    uplinkKID.iChainedCheckSum        = 0;
    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_UPLINK_KID_OBJECT, &uplinkKID ) );

    /*
     *  Add playback policy object
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT, &xmrMinOpls ) );

    xmrMinOpls.fValid = TRUE;
    xmrMinOpls.wAnalogVideo = pPolicyTranscryptPlay->playOPLs.minOPL.wAnalogVideo;
    xmrMinOpls.wCompressedDigitalAudio = pPolicyTranscryptPlay->playOPLs.minOPL.wCompressedDigitalAudio;
    xmrMinOpls.wCompressedDigitalVideo = pPolicyTranscryptPlay->playOPLs.minOPL.wCompressedDigitalVideo;
    xmrMinOpls.wUncompressedDigitalAudio = pPolicyTranscryptPlay->playOPLs.minOPL.wUncompressedDigitalAudio;
    xmrMinOpls.wUncompressedDigitalVideo = pPolicyTranscryptPlay->playOPLs.minOPL.wUncompressedDigitalVideo;

    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_OUTPUT_PROTECTION_OBJECT, &xmrMinOpls ) );

    /*
     *  Add minimum security level for the content plus the minimum application and device
     *  revocation list versions.
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT, &xmrMinEnv ) );

    xmrMinEnv.fValid                = TRUE;
    if(f_pPolicy->ePolicyType == WMDRMNET_POLICY_TYPE_TRANSCRYPTPLAY)
    {
        xmrMinEnv.wMinimumSecurityLevel = pPolicyTranscryptPlay->globals.MinimumEnvironment.wMinimumSecurityLevel;
        xmrMinEnv.dwMinimumAppRevocationListVersion = pPolicyTranscryptPlay->globals.MinimumEnvironment.dwMinimumAppRevocationListVersion;
        xmrMinEnv.dwMinimumDeviceRevocationListVersion = pPolicyTranscryptPlay->globals.MinimumEnvironment.dwMinimumDeviceRevocationListVersion;
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }

    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_MINIMUM_ENVIRONMENT_OBJECT, &xmrMinEnv ) );

    /*
     * Add the settings object
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SETTINGS_OBJECT, &Settings ) );
    Settings.fValid = TRUE;
    Settings.wValue = XMR_SETTINGS_FLAG_CANNOT_PERSIST;
    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_SETTINGS_OBJECT, &Settings ) );

    /*
     *  Add the serial number
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT, &xmrSerialNumber ) );
    xmrSerialNumber.fValid = TRUE;
    xmrSerialNumber.pbDataBuffer = (DRM_BYTE *)&f_idSerial;
    xmrSerialNumber.cbData = SIZEOF(DRM_ID);
    xmrSerialNumber.iData = 0;
    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_SERIAL_NUMBER_OBJECT, &xmrSerialNumber ) );

    /*
     *  Add the SourceID from the WMDRM-PD license to the XMR license
     */
    ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_SOURCEID_OBJECT, &xmr_sourceID ) );
    xmr_sourceID.dwValue = pPolicyTranscryptPlay->globals.dwSourceID;
    xmr_sourceID.fValid = TRUE;
    ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_SOURCEID_OBJECT, &xmr_sourceID ) );

    /*
     *  Add the inclusion list.
     */
    if ( 0 != pPolicyTranscryptPlay->globals.InclusionList.cGUIDs )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT, &xmrInclusionList ) );
        xmrInclusionList.cGUIDs      = pPolicyTranscryptPlay->globals.InclusionList.cGUIDs;
        xmrInclusionList.pguidBuffer = pPolicyTranscryptPlay->globals.InclusionList.pguidBuffer;
        xmrInclusionList.iGuids      = 0;
        xmrInclusionList.fValid      = TRUE;
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_INCLUSION_LIST_OBJECT, &xmrInclusionList ) );
    }

    /*
     * Add the Riv
     */
    if ( 0 != pPolicyTranscryptPlay->globals.dwRiv )
    {
        ChkDR( DRM_XMR_InitializeObject( XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_2_OBJECT, &xmrRiv ) );
        xmrRiv.dwValue = pPolicyTranscryptPlay->globals.dwRiv;
        xmrRiv.fValid  = TRUE;
        ChkDR( DRM_XMR_AddObject( &xmrBuilder, XMR_OBJECT_TYPE_REVOCATION_INFORMATION_VERSION_OBJECT, &xmrRiv ) );
    }

    cbXMR = 0;
    dr = DRM_XMR_FinishLicense( &xmrBuilder,
                                 pContext,
                                 _DRM_XMR_LeafSignatureCallback,
                                 NULL,
                                &cbXMR );
    if( dr != DRM_E_BUFFERTOOSMALL )
    {
        ChkDR( dr );
    }

    ChkMem( pbXMR = (DRM_BYTE*)Oem_MemAlloc(cbXMR) );

    ChkDR( DRM_XMR_FinishLicense( &xmrBuilder,
                                  f_pNdtContentSessionContext,
                                   _DRM_XMR_LeafSignatureCallback,
                                   pbXMR,
                                  &cbXMR ) );

    *f_ppbXMR = pbXMR;
    *f_pcbXMR = cbXMR;
    pbXMR = NULL;

ErrorExit:
    TRACE_IF_FAILED(("NDT_BuildXMRLicense failed 0x%x\n",dr));

    if(pbXMR != NULL)
    {
        Oem_MemFree(pbXMR);
    }

    if(pbKeyMaterial != NULL)
    {
        Oem_MemFree(pbKeyMaterial);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}


EXIT_PK_NAMESPACE_CODE;

