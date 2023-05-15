/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_DRMNDTASF_C
#include <drmxmlparser.h>
#include <ndtcontextsizes.h>
#include <ndtconstants.h>
#include <drmndtmanager.h>
#include <ndtlicense.h>
#include <drmndtasf.h>
#include <ndtasf_internal.h>
#include <ndtmanager_internal.h>
#include <ndtasfdrm.h>
#include <drmdevstore.h>
#include <asfsample.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

#define ChkDeliveryMode(m) if(pContSessContext->asfContext.ASFDeliveryMode != m) \
    { \
        ChkDR( DRM_E_FAIL ); \
    }


/**********************************************************************
**
** Function:    Drm_Ndt_Asf_Init
**
** Synopsis: Initializes an ASF content session with the file name and
**           delivery mode.  The file path must be a valid ASF file.
**           The mux'ed ASF delivery is used over protocols like HTTP
**           and MTP whereas the sample mode is used for RTSP.
**
** Arguments:   [f_pOEMContext]     -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext]  -- The NDT DRM manager context
**              [f_dstrFilePath]    -- The file path
**              [f_ASFDeliveryMode] -- The delivery mode
**              [f_pAppContext]     -- An initialized app context (i.e. one
**                                     already used in a Drm_Initialize call)
**                                     to use for accessing protected content.
**                                     This context may *not* be the same used
**                                     in Drm_Ndt_Initialize and must be
**                                     different for each CONTENT session.
**                                     This app context can be uninitialized
**                                     after Drm_Ndt_Asf_Close is used to close
**                                     the session.
**              [f_fEncryptClear]   -- Whether or not clear content should be encrypted
**              [f_pidSession]      -- The ID of the session that is created
**              [f_pdwFlags]        -- Flags returned to the caller
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**              Indicates via flags parameter whether the content is
**              WMDRM protected or not and whether a license is
**              available to play the content.
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_Asf_Init (
    __in_opt   DRM_VOID                 *f_pOEMContext,
    __in       NDTMGR_CONTEXT           *f_pNdtMgrContext,
    __in const DRM_STRING                f_dstrFilePath,
    __in const NDT_ASFDELIVERY_MODE      f_ASFDeliveryMode,
    __in       DRM_APP_CONTEXT          *f_pAppContext,
    __in const DRM_BOOL                  f_fEncryptClear,
    __out      DRM_ID                   *f_pidSession,
    __out      DRM_DWORD                *f_pdwFlags
)
{
    DRM_RESULT                              dr               = DRM_SUCCESS;
    NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *pContSessContext = NULL;
    NDT_ASF_CONTEXT                        *pAsfContext      = NULL;
    OEM_FILEHDL                             hInFile          = OEM_INVALID_HANDLE_VALUE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_Init );

    ChkArg( f_pNdtMgrContext != NULL
            && f_pAppContext != NULL
            && f_pidSession != NULL
            && f_pdwFlags != NULL );

    ChkDRMString( &f_dstrFilePath );

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        DRM_BYTE  *pbRevocationBuffer = NULL;
        DRM_DWORD  cbRevocationBuffer;
        ChkDR( Drm_Revocation_GetBuffer( f_pAppContext,
                                         &pbRevocationBuffer,
                                         &cbRevocationBuffer ) );

        if (( pbRevocationBuffer == NULL ) ||
            ( cbRevocationBuffer < REVOCATION_BUFFER_SIZE ))
        {
            ChkDR( DRM_E_REVOCATION_BUFFER_TOO_SMALL );
        }
    }

    /* Re-initialize in case init is being called after an earlier failure */
    ChkDR( Drm_Reinitialize( f_pAppContext ) );

    ChkDR( NDTMGR_Internal_CreateSession( f_pOEMContext,
                                          f_pNdtMgrContext,
                                          NDTMGR_SESSION_TYPE_CONTENT,
                                          f_pAppContext,
                                          (NDTMGR_INTERNAL_SESSION_CONTEXT **) &pContSessContext ) );

    pAsfContext = &pContSessContext->asfContext;

    hInFile = Oem_File_Open( f_pOEMContext,
                             f_dstrFilePath.pwszString,
                             OEM_GENERIC_READ,
                             OEM_FILE_SHARE_READ,
                             OEM_OPEN_EXISTING,
                             OEM_ATTRIBUTE_NORMAL );

    if( hInFile == OEM_INVALID_HANDLE_VALUE )
    {
        ChkDR( DRM_E_FILEOPEN );
    }

    ChkDR( NDTMGR_ASF_Internal_Initialize( pAsfContext, hInFile ) );

    if ( NDT_ASFDELIVERY_MODE_SAMPLES != f_ASFDeliveryMode
         &&  NDT_ASFDELIVERY_MODE_MUXED != f_ASFDeliveryMode )
    {
        ChkDR( DRM_E_INVALIDARG );
    }
    pAsfContext->ASFDeliveryMode = f_ASFDeliveryMode;

    dr = NDTMGR_ASF_Internal_CheckFileDRM( pContSessContext, f_fEncryptClear );
    if( dr != DRM_E_LICENSE_NOT_FOUND && dr != DRM_E_LICENSE_EXPIRED && dr != DRM_E_RIGHTS_NOT_AVAILABLE && DRM_FAILED( dr ) )
    {
        ChkDR( dr );
    }
    dr = DRM_SUCCESS;

    /* Generate f_pdwFlags return from values in the ASF context */

    if(pAsfContext->dwDRMStatus == NDT_DRM_STATUS_PROTECTED)
    {
        *f_pdwFlags |= NDTMGR_ASF_WMDRMPROTECTED;
    }
    if(pAsfContext->dwLicenseStatus == NDT_DRM_LICENSE_ALLOWS_PLAYBACK)
    {
        *f_pdwFlags |= NDTMGR_ASF_LICENSE_PLAYAVAILABLE;
    }

    MEMCPY( f_pidSession, &pContSessContext->sessionContext.idSession,
            DRM_ID_SIZE );

    pContSessContext->mediaSessionState = MEDIA_STATE_INITED;

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        if( pContSessContext )
        {
            Drm_Ndt_Session_Close( f_pNdtMgrContext,
                                   pContSessContext->sessionContext.idSession,
                                   NULL );
        }
        if( pAsfContext && OEM_INVALID_HANDLE_VALUE != hInFile )
        {
            (void) Oem_File_Close( hInFile );
        }
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    Drm_Ndt_Asf_InitTranscryption
**
** Synopsis:    This function informs the NDT Manager that transcryption is to
**              be done on the ASF file. This call must be made immediately
**              after the Drm_Ndt_Asf_Init call for protected content.
**
** Arguments:   [f_pOEMContext]        -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext]     -- A handle to the Windows Media DRM for
**                                        Network Devices Transmitter (NDT)
**                                        manager context that will be used for
**                                        initialization.
**              [f_idSession]          -- The session identifier (ID) that will
**                                        be used for transcryption.
**              [f_LicenseChallenge]   -- The license challenge data.
**              [f_pKID]               -- A pointer to the Key identifier (KID)
**                                        data that will be used for
**                                        transcryption.
**              [f_pbLicenseResponse]  -- A pointer to the license response data.
**              [f_pcbLicenseResponse] -- On input, contains the maximum length
**                                        of the f_pbLicenseResponseData buffer.
**                                        On output, contains the length of the
**                                        license response data buffer.
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_Asf_InitTranscryption(
    __in_opt   DRM_VOID                 *f_pOEMContext,
    __in       NDTMGR_CONTEXT           *f_pNdtMgrContext,
    __in const DRM_ID                    f_idSession,
    __in const DRM_BYTEBLOB              f_LicenseChallenge,
    __out      DRM_KID                  *f_pKID,
    __out_bcount_opt(*f_pcbLicenseResponse) DRM_BYTE *f_pbLicenseResponse,
    __inout    DRM_DWORD                *f_pcbLicenseResponse
)
{
    DRM_RESULT                              dr = DRM_SUCCESS;
    NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *pContSessContext = NULL;
    WMDRMNET_POLICY                        *pPolicy = NULL;
    DRM_DWORD                               cbPolicy = 0;
    NDT_LICENSE_MODE                        LicenseMode = NDT_LICENSE_MODE_SIMPLE;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_InitTranscryption );

    ChkArg( f_pNdtMgrContext != NULL
        && f_pKID != NULL
        && f_pcbLicenseResponse != NULL
        && (*f_pcbLicenseResponse == 0 || f_pbLicenseResponse != NULL) );

    ChkDR( NDTMGR_Internal_FindSession
               ( f_pNdtMgrContext,
                 f_idSession,
                 NDTMGR_SESSION_TYPE_CONTENT,
                 (NDTMGR_INTERNAL_SESSION_CONTEXT **)&pContSessContext ) );

    pContSessContext->asfContext.fProtectedContentPlayback = TRUE;

    /* Ensure we are transmitting protected content and have license. */
    if( ! ( pContSessContext->asfContext.dwDRMStatus &&
            pContSessContext->asfContext.dwLicenseStatus ) )
    {
        ChkDR( DRM_E_LICENSE_NOT_FOUND );
    }

    if( pContSessContext->asfContext.ASFDeliveryMode == NDT_ASFDELIVERY_MODE_MUXED )
    {
        LicenseMode = NDT_LICENSE_MODE_SIMPLE;

        ChkDR( DRM_NDT_License_CreateDefaultPolicy( pContSessContext, &pPolicy, &cbPolicy ) );
    }
    else if( NDT_ASFDELIVERY_MODE_SAMPLES == pContSessContext->asfContext.ASFDeliveryMode )
    {
        LicenseMode = NDT_LICENSE_MODE_CHAINED;
    }

    ChkDR( Drm_Ndt_License_ProcessChallenge( f_pOEMContext,
                                             f_pNdtMgrContext,
                                             f_idSession,
                                             f_LicenseChallenge,
                                             LicenseMode,
                                             pPolicy,
                                             f_pKID,
                                             f_pbLicenseResponse,
                                             f_pcbLicenseResponse ) );

    ChkDR( NDTMGR_ASF_Internal_InitializeTranscryption( &pContSessContext->asfContext,
                                                        f_pNdtMgrContext,
                                                        f_idSession,
                                                        (DRM_BYTE *) f_pKID,
                                                        SIZEOF(DRM_KID) ) );

ErrorExit:
    if( pPolicy )
    {
        DRM_NDT_License_FreePolicy(pPolicy);
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}



/**********************************************************************
**
** Function:    Drm_Ndt_Asf_GetHeader
**
** Synopsis:    This function retrieves the Advanced Streaming Format (ASF)
**              header associated with the content session.
**
** Arguments:   [f_pOEMContext]      -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext]   -- The NDT DRM manager context
**              [f_idSession]        -- The content session ID
**              [f_pbHeader]         -- Output byte buffer for header
**              [f_pcbHeader]        -- Size of buffer
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_Asf_GetHeader(
    __in_opt   DRM_VOID                 *f_pOEMContext,
    __in       NDTMGR_CONTEXT           *f_pNdtMgrContext,
    __in const DRM_ID                    f_idSession,
    __out_bcount(*f_pcbHeader) DRM_BYTE *f_pbHeader,
    __inout    DRM_DWORD                *f_pcbHeader
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *pContSessContext;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_GetHeader );

    ChkArg( f_pNdtMgrContext != NULL
            && f_pcbHeader != NULL
            && (*f_pcbHeader == 0 || f_pbHeader != NULL) );

    ChkDR( NDTMGR_Internal_FindSession
               ( f_pNdtMgrContext,
                 f_idSession,
                 NDTMGR_SESSION_TYPE_CONTENT,
                 (NDTMGR_INTERNAL_SESSION_CONTEXT **) &pContSessContext ) );

    /* Ensure we are in sample mode and the content session is in
       the proper state */
    ChkDeliveryMode( NDT_ASFDELIVERY_MODE_SAMPLES );

    if(!( ( pContSessContext->mediaSessionState == MEDIA_STATE_INITED )
       || ( pContSessContext->mediaSessionState == MEDIA_STATE_SENDING_DATA )
       || ( pContSessContext->mediaSessionState == MEDIA_STATE_LICENSE_READY ) ) )
    {
        ChkDR( DRM_E_FAIL );
    }

    /* Validate device and commit license if we're doing protected playback */
    if ( pContSessContext->asfContext.fProtectedContentPlayback )
    {
        if ( !DRM_DeviceStore_IsDeviceValid( f_pOEMContext, pContSessContext->hDevice ) )
        {
            ChkDR( DRM_E_ND_MUST_REVALIDATE );
        }

        if(!pContSessContext->asfContext.fCommitted)
        {
            ChkDR( Drm_Reader_Commit( pContSessContext->pAppContext, NULL, NULL ) );
            pContSessContext->asfContext.fCommitted = TRUE;
        }
    }

    ChkDR( NDTMGR_ASF_Internal_GetHeader(&pContSessContext->asfContext,
                                         f_pbHeader,
                                         f_pcbHeader) );

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    Drm_Ndt_Asf_GenerateLeafLicenseResponse
**
** Synopsis:    This function uses the session context to generate a leaf
**              license for Advanced Streaming Format (ASF) content.
**
** Arguments:   [f_pOEMContext]      -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext]   -- The NDT DRM manager context
**              [f_idSession]        -- The content session ID
**              [f_pKID]             -- The KID
**              [f_pbLeafLicenseResponse] -- Response buffer
**              [f_pcbLeafLicenseResponse] -- Size of response buffer
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_Asf_GenerateLeafLicenseResponse(
    __in_opt   DRM_VOID                 *f_pOEMContext,
    __in       NDTMGR_CONTEXT           *f_pNdtMgrContext,
    __in const DRM_ID                    f_idSession,
    __out_bcount(*f_pcbLeafLicenseResponse) DRM_BYTE *f_pbLeafLicenseResponse,
    __inout    DRM_DWORD                *f_pcbLeafLicenseResponse
)
{
    DRM_RESULT                              dr = DRM_SUCCESS;
    NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *pContSessContext = NULL;
    WMDRMNET_POLICY                        *pPolicy = NULL;
    DRM_DWORD                               cbPolicy = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_GenerateLeafLicenseResponse );

    ChkArg( f_pNdtMgrContext != NULL
        && f_pcbLeafLicenseResponse != NULL
        && (*f_pcbLeafLicenseResponse == 0
        || f_pbLeafLicenseResponse != NULL) );

    ChkDR( NDTMGR_Internal_FindSession
               ( f_pNdtMgrContext,
                 f_idSession,
                 NDTMGR_SESSION_TYPE_CONTENT,
                 (NDTMGR_INTERNAL_SESSION_CONTEXT **)&pContSessContext ) );

    ChkDeliveryMode ( NDT_ASFDELIVERY_MODE_SAMPLES );

    ChkDR( DRM_NDT_License_CreateDefaultPolicy( pContSessContext, &pPolicy, &cbPolicy ) );

    ChkDR( Drm_Ndt_License_GenerateLeafResponse( f_pOEMContext,
                                                 f_pNdtMgrContext,
                                                 f_idSession,
                                                 pPolicy,
                                                 f_pbLeafLicenseResponse,
                                                 f_pcbLeafLicenseResponse ) );

ErrorExit:
    if(pPolicy)
    {
        DRM_NDT_License_FreePolicy(pPolicy);
    }

    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}



/**********************************************************************
**
** Function:    Drm_Ndt_Asf_Seek
**
** Synopsis:    This function sets the playback position for an Advanced
**              Streaming Format (ASF) stream.
**
** Arguments:   [f_pOEMContext]    -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext] -- The NDT DRM manager context
**              [f_idSession]      -- The content session ID
**              [f_qwSeekTimeNS]   -- Seek Time in 100-nanosecond units
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_Asf_Seek (
    __in_opt   DRM_VOID                 *f_pOEMContext,
    __in       NDTMGR_CONTEXT           *f_pNdtMgrContext,
    __in const DRM_ID                    f_idSession,
    __in const DRM_UINT64                f_qwSeekTimeNS
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *pContSessContext = NULL;
    DRM_DWORD dwSeekTimeMS = 0;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_Seek );

    ChkArg( f_pNdtMgrContext != NULL );

    ChkDR( NDTMGR_Internal_FindSession
               ( f_pNdtMgrContext,
                 f_idSession,
                 NDTMGR_SESSION_TYPE_CONTENT,
                 (NDTMGR_INTERNAL_SESSION_CONTEXT **)&pContSessContext ) );

    /* Convert input time to proper units for old seek call */
    dwSeekTimeMS = DRM_UI64Low32
                       ( DRM_UI64Div( f_qwSeekTimeNS,
                                      DRM_UI64( MS_TO_HNS_MULTIPLIER ) ) );

#if DRM_SUPPORT_AUDIO_ONLY == 1
    ChkDR( NDTMGR_ASF_Internal_SetTimePosition( f_pOEMContext,
                                                &pContSessContext->asfContext,
                                                dwSeekTimeMS ) );
#else
    if ( pContSessContext->asfContext.fAudioOnly && 1 == pContSessContext->asfContext.dwNoOfStreams )
    {
        ChkDR( NDTMGR_ASF_Internal_SetTimePosition( f_pOEMContext,
                                                    &pContSessContext->asfContext,
                                                    dwSeekTimeMS ) );
    }
    else
    {
        /* Seeking is not currently supported for video files. */
        ChkDR( DRM_E_NOTIMPL );
    }
#endif /* DRM_SUPPORT_AUDIO_ONLY */

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/**********************************************************************
**
** Function:    Drm_Ndt_Asf_ReadData
**
** Synopsis:    This function retrieves multiplexed data for an Advanced
**              Streaming Format (ASF) stream.
**
** Arguments:   [f_pOEMContext]      -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext]   -- The NDT DRM manager context
**              [f_idSession]        -- The content session ID
**              [f_pbData]           -- Data buffer
**              [f_pcbData]          -- Size of data buffer
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_Asf_ReadData(
    __in_opt   DRM_VOID                 *f_pOEMContext,
    __in       NDTMGR_CONTEXT           *f_pNdtMgrContext,
    __in const DRM_ID                    f_idSession,
    __out_bcount(*f_pcbData) DRM_BYTE   *f_pbData,
    __inout    DRM_DWORD                *f_pcbData
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *pContSessContext = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_ReadData );

    ChkArg( f_pNdtMgrContext != NULL
        && f_pcbData != NULL
        && (*f_pcbData == 0 || f_pbData != NULL) );

    ChkDR( NDTMGR_Internal_FindSession
               ( f_pNdtMgrContext,
                 f_idSession,
                 NDTMGR_SESSION_TYPE_CONTENT,
                 (NDTMGR_INTERNAL_SESSION_CONTEXT **)&pContSessContext ) );

    ChkDeliveryMode( NDT_ASFDELIVERY_MODE_MUXED )

    if ( pContSessContext->asfContext.fProtectedContentPlayback )
    {
        if( ! ( ( pContSessContext->mediaSessionState == MEDIA_STATE_SENDING_DATA )
            || ( pContSessContext->mediaSessionState == MEDIA_STATE_LICENSE_READY ) ) )
        {
            ChkDR( DRM_E_FAIL );
        }

        if ( !DRM_DeviceStore_IsDeviceValid( f_pOEMContext, pContSessContext->hDevice ) )
        {
            ChkDR( DRM_E_ND_MUST_REVALIDATE );
        }

        if(!pContSessContext->asfContext.fCommitted)
        {
            ChkDR( Drm_Reader_Commit( pContSessContext->pAppContext, NULL, NULL ) );
            pContSessContext->asfContext.fCommitted = TRUE;
        }
    }
    else if ( !( ( pContSessContext->mediaSessionState == MEDIA_STATE_INITED )
            || ( pContSessContext->mediaSessionState == MEDIA_STATE_SENDING_DATA )
            || ( pContSessContext->mediaSessionState == MEDIA_STATE_LICENSE_READY ) ) )
    {
        ChkDR( DRM_E_FAIL );
    }

    pContSessContext->mediaSessionState = MEDIA_STATE_SENDING_DATA;

#if DRM_SUPPORT_AUDIO_ONLY == 1
    if ( pContSessContext->asfContext.fAudioOnly && 1 == pContSessContext->asfContext.dwNoOfStreams )
    {
        ChkDR( NDTMGR_ASF_Internal_ReadData( f_pOEMContext,
                                             &pContSessContext->asfContext,
                                             f_pbData,
                                             f_pcbData ) );
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }
#else
    ChkDR( NDTMGR_ASF_Internal_ReadData( f_pOEMContext,
                                         &pContSessContext->asfContext,
                                         f_pbData,
                                         f_pcbData ) );

#endif /* DRM_SUPPORT_AUDIO_ONLY */

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;

}

/**********************************************************************
**
** Function:    Drm_Ndt_Asf_ReadSample
**
** Synopsis:    This function gets raw Advanced Streaming Format (ASF) samples
**              with some associated sample information.
**
** Arguments:   [f_pOEMContext]      -- Pointer to the OEM context; may be NULL
**              [f_pNdtMgrContext]   -- The NDT DRM manager context
**              [f_idSession]        -- The content session ID
**              [f_pSampleInfo]      -- Sample information
**              [f_pbSampleData]     -- Sample sample data buffer
**              [f_pcbSampleData]    -- Size of sample data buffer
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_Asf_ReadSample(
    __in_opt   DRM_VOID                 *f_pOEMContext,
    __in       NDTMGR_CONTEXT           *f_pNdtMgrContext,
    __in const DRM_ID                    f_idSession,
    __out      NDT_SAMPLEINFO           *f_pSampleInfo,
    __out_bcount(*f_pcbSampleData) DRM_BYTE *f_pbSampleData,
    __inout    DRM_DWORD                *f_pcbSampleData
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *pContSessContext = NULL;
    ASF_SAMPLE *pSample = NULL;
    DRM_DWORD   cbSampleDataOut = 0;


    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_ReadSample );

    ChkArg( f_pNdtMgrContext != NULL
        && f_pSampleInfo != NULL
        && f_pcbSampleData != NULL
        && (*f_pcbSampleData == 0 || f_pbSampleData != NULL) );

    /*
    ** Remeber how much room is in the buffer passed into this function.
    ** We'll need to compare against this below before doing a copy.
    */
    cbSampleDataOut = *f_pcbSampleData;

    ChkDR( NDTMGR_Internal_FindSession
               ( f_pNdtMgrContext,
                 f_idSession,
                 NDTMGR_SESSION_TYPE_CONTENT,
                 (NDTMGR_INTERNAL_SESSION_CONTEXT **)&pContSessContext ) );

    ChkDeliveryMode( NDT_ASFDELIVERY_MODE_SAMPLES );

    if ( pContSessContext->asfContext.fProtectedContentPlayback )
    {
        if( ! ( ( pContSessContext->mediaSessionState == MEDIA_STATE_SENDING_DATA )
            || ( pContSessContext->mediaSessionState == MEDIA_STATE_LICENSE_READY ) ) )
        {
            ChkDR( DRM_E_FAIL );
        }

        if ( !DRM_DeviceStore_IsDeviceValid( f_pOEMContext, pContSessContext->hDevice ) )
        {
            ChkDR( DRM_E_ND_MUST_REVALIDATE );
        }
    }
    pContSessContext->mediaSessionState = MEDIA_STATE_SENDING_DATA;

#if DRM_SUPPORT_AUDIO_ONLY == 1
    if ( pContSessContext->asfContext.fAudioOnly && 1 == pContSessContext->asfContext.dwNoOfStreams )
    {
        ChkDR( NDTMGR_ASF_Internal_ReadSample(&pContSessContext->asfContext,
                                              f_pSampleInfo,
                                              &pSample,
                                              f_pcbSampleData) );

        ChkBOOL( cbSampleDataOut >= *f_pcbSampleData, DRM_E_BUFFERTOOSMALL );
        ChkDR( ASFSample_CopyToBuffer(pSample, f_pbSampleData, f_pcbSampleData) );
    }
    else
    {
        ChkDR( DRM_E_NOTIMPL );
    }
#else
    ChkDR( NDTMGR_ASF_Internal_ReadSample(&pContSessContext->asfContext,
                                          f_pSampleInfo,
                                          &pSample,
                                          f_pcbSampleData) );
    ChkBOOL( cbSampleDataOut >= *f_pcbSampleData, DRM_E_BUFFERTOOSMALL );
    ChkDR( ASFSample_CopyToBuffer(pSample, f_pbSampleData, f_pcbSampleData) );
#endif /* DRM_SUPPORT_AUDIO_ONLY */

ErrorExit:
    if ( NULL != pSample )
    {
        ASFSample_Release( pSample );
        pSample = NULL;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;

}

/**********************************************************************
**
** Function:    Drm_Ndt_Asf_Close
**
** Synopsis:    This function frees up the resources associated with an
**              Advanced Streaming Format (ASF) stream.
**
** Arguments:   [f_pNdtMgrContext]   -- The NDT DRM manager context
**              [f_idSession]        -- The content session ID
**              [f_ppAppContext]     -- Optional pointer that receives the app context used to init the stream.
**
** Returns:     DRM_SUCCESS on success or
**              DRM_E_INVALIDARG if any of the arguments are invalid
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL Drm_Ndt_Asf_Close(
    __in       NDTMGR_CONTEXT           *f_pNdtMgrContext,
    __in const DRM_ID                    f_idSession,
    __out_opt  DRM_APP_CONTEXT         **f_ppAppContext
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT *pContSessContext = NULL;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_Drm_Ndt_Asf_Close );

    ChkArg( f_pNdtMgrContext != NULL );

    ChkDR( NDTMGR_Internal_FindSession
               ( f_pNdtMgrContext,
                 f_idSession,
                 NDTMGR_SESSION_TYPE_CONTENT,
                 (NDTMGR_INTERNAL_SESSION_CONTEXT **)&pContSessContext ) );

    if (pContSessContext->asfContext.hInFile != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close( pContSessContext->asfContext.hInFile );
        pContSessContext->asfContext.hInFile = OEM_INVALID_HANDLE_VALUE;
    }

    pContSessContext->asfContext.fProtectedContentPlayback = FALSE;
    pContSessContext->asfContext.fCommitted                = FALSE;

    Drm_Reader_Close( &pContSessContext->asfContext.decryptContext );

    ChkDR( Drm_Ndt_Session_Close ( f_pNdtMgrContext,
                                   f_idSession,
                                   f_ppAppContext ) );

ErrorExit:
    DRM_PROFILING_LEAVE_SCOPE;
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

