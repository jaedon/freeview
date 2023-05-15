/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_NDTASFDRM_C
#include <drmresults.h>
#include <ndtcontextsizes.h>
#include <drmndtmanager.h>
#include <ndtlicense.h>
#include <ndtasf_internal.h>
#include <ndtasfdrm.h>
#include <ndtconstants.h>
#include <drmplayreadyobj.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;


/*
** Transcrypting from RC4/COCKTAIL or clear to ND AES; additional bytes
** needed for the sample ID size (WORD) + the sample ID itself (QWORD)
*/
#define ADD_SAMPLEID_BYTES   ( SIZEOF( DRM_WORD ) + SIZEOF( DRM_UINT64 ) )


DRM_RESULT NDTMGR_ASF_Internal_CheckFileDRM(
    __inout     NDTMGR_INTERNAL_CONTENTSESSION_CONTEXT  * const f_pContSessContext,
    __in const  DRM_BOOL                                        f_fEncryptClear
)
{
    DRM_RESULT      dr      = DRM_SUCCESS;
    DRM_BOOL        fHaveV1 = FALSE;
    DRM_BOOL        fHaveV2 = FALSE;
    DRM_BOOL        fHavePR = FALSE;
    DRM_BOOL        fBound  = FALSE;
    ASF_FILE_INFO   AsfFileInfo;
    DRM_RESULT      drSaved = DRM_SUCCESS;

    DRM_PROFILING_ENTER_SCOPE( PERF_MOD_NDT_ASF, PERF_FUNC_NDTMGR_ASF_Internal_CheckFileDRM );

    /* Initialize file info structure */
    ZEROMEM( &AsfFileInfo, SIZEOF( ASF_FILE_INFO ) );

    /* Start by assuming clear content. */
    f_pContSessContext->asfContext.dwDRMStatus     = NDT_DRM_STATUS_UNPROTECTED;
    f_pContSessContext->asfContext.dwLicenseStatus = NDT_DRM_LICENSE_NOT_AVAILABLE;
    f_pContSessContext->asfContext.dwAesPayloadExtensionAdditionalBytes = ADD_SAMPLEID_BYTES;
    ZEROMEM( &f_pContSessContext->asfContext.rgdwSampleIDRepDataOffset, MAXSTREAMS * SIZEOF( DRM_DWORD ) );;

    /* Get encryption and stream information from the file. If it's not an ASF file, it will fail. */
    ChkDR( NDTMGR_ASF_Internal_GetFileInfo( &f_pContSessContext->asfContext, &AsfFileInfo ) );

    /* If we have an PlayReady Object, use it. Otherwise, fall back first
     * to the Extended Content Encryption Object (if available)
     */
    if ( AsfFileInfo.pPlayReadyObj != NULL )
    {
        const DRM_BYTE  *pbPRData = AsfFileInfo.pPlayReadyObj->pbData;
        DRM_DWORD        cbPRData = 0;

        fHavePR = TRUE;

        f_pContSessContext->asfContext.fIsPlayReady = TRUE;

        /*
        ** The size of the raw PlayReady object data should be at least the size of a GUID (system id) plus
        ** version and size DWORDs.
        */

        ChkBOOL( AsfFileInfo.pPlayReadyObj->cbData >= SIZEOF( DRM_GUID ) + SIZEOF( DRM_DWORD ) * 2,
                 DRM_E_ASF_BAD_ASF_HEADER );

        ChkDR( DRM_PRO_GetCipherType( AsfFileInfo.pPlayReadyObj->pbData +
                                      SIZEOF( DRM_GUID ) + SIZEOF( DRM_DWORD ) * 2,
                                      AsfFileInfo.pPlayReadyObj->cbData -
                                      SIZEOF( DRM_GUID ) - SIZEOF( DRM_DWORD ) * 2,
                                      &f_pContSessContext->asfContext.eCipherType ) );

        DRMASSERT( f_pContSessContext->asfContext.eCipherType == eDRM_RC4_CIPHER ||
                   f_pContSessContext->asfContext.eCipherType == eDRM_AES_COUNTER_CIPHER );

        if ( f_pContSessContext->asfContext.eCipherType == eDRM_RC4_CIPHER )
        {
            f_pContSessContext->asfContext.dwAesPayloadExtensionAdditionalBytes = ADD_SAMPLEID_BYTES;
        }
        else if ( f_pContSessContext->asfContext.eCipherType == eDRM_AES_COUNTER_CIPHER )
        {
            f_pContSessContext->asfContext.dwAesPayloadExtensionAdditionalBytes = 0;
        }

        /* Ensure that we can adjust the pbPRData pointer safely below */
        ChkBOOL( AsfFileInfo.pPlayReadyObj->cbData > SIZEOF(DRM_GUID) + SIZEOF(DRM_DWORD) + SIZEOF(DRM_DWORD),
                 DRM_E_BUFFERTOOSMALL );

        pbPRData += SIZEOF( DRM_GUID );       /* skip over the System ID */
        pbPRData += SIZEOF( DRM_DWORD );      /* skip over the System Version */
        BYTES_TO_DWORD( cbPRData, pbPRData ); /* extract the data length */
        pbPRData += SIZEOF( DRM_DWORD );      /* skip over the data length */

        /* Ensure that cbPRData is valid */
        ChkBOOL( AsfFileInfo.pPlayReadyObj->cbData >= cbPRData + SIZEOF(DRM_GUID) + SIZEOF(DRM_DWORD) + SIZEOF(DRM_DWORD),
                 DRM_E_BUFFERTOOSMALL );

        /* Use the PlayReady Object data to retrieve license information. */
        dr = NDT_RetrieveLicenseDataPR( f_pContSessContext,
                                        pbPRData,
                                        cbPRData,
                                        &g_dstrDRM_RIGHT_PLAYBACK );

        /* If the license isn't found for this header, then keep processing. */
        /* Otherwise, break on any other errors. */
        if ( ( dr != DRM_E_LICENSE_NOT_FOUND )
          && ( dr != DRM_E_RIGHTS_NOT_AVAILABLE )
          && ( dr != DRM_E_LICENSE_EXPIRED )
          && ( dr != DRM_E_CLK_NOT_SET )
          && ( dr != DRM_E_NO_CLK_SUPPORTED ) )
        {
            ChkDR( dr );

            fBound = TRUE;
        }
        drSaved = dr;
    }

    /* If we have an Extended Content Encryption Object, use it. Otherwise, fall back
     * to the Content Encryption Object (if available)
     */
    if ( AsfFileInfo.pContentEncObjExt != NULL && ! fBound )
    {
        fHaveV2 = TRUE;
        f_pContSessContext->asfContext.eCipherType = eDRM_RC4_CIPHER;
        f_pContSessContext->asfContext.dwAesPayloadExtensionAdditionalBytes = ADD_SAMPLEID_BYTES;

        f_pContSessContext->asfContext.fIsPlayReady = FALSE;

        /*
        ** We could end up here if an above step set the content header but
        ** failed to bind. As such, we need to clear the old header.
        */
        ChkDR( Drm_Reinitialize( f_pContSessContext->pAppContext ) );

        /* Use the extended content encryption data to retrieve license information. */
        dr = NDT_RetrieveLicenseDataV2( f_pContSessContext,
                                        AsfFileInfo.pContentEncObjExt->pbData,
                                        AsfFileInfo.pContentEncObjExt->cbData,
                                        &g_dstrDRM_RIGHT_PLAYBACK );

        /* If the license isn't found for this header, then keep processing. */
        /* Otherwise, break on any other errors. */
        if ( ( dr != DRM_E_LICENSE_NOT_FOUND ) &&
             ( dr != DRM_E_RIGHTS_NOT_AVAILABLE ) &&
             ( dr != DRM_E_LICENSE_EXPIRED ) &&
             ( dr != DRM_E_CLK_NOT_SET ) &&
             ( dr != DRM_E_NO_CLK_SUPPORTED ) )
        {
            ChkDR( dr );

            fBound = TRUE;
        }
        drSaved = dr;
    }

    if ( AsfFileInfo.pContentEncObj != NULL && ! fBound )
    {
        fHaveV1 = TRUE;
        f_pContSessContext->asfContext.eCipherType = eDRM_RC4_CIPHER;
        f_pContSessContext->asfContext.dwAesPayloadExtensionAdditionalBytes = ADD_SAMPLEID_BYTES;

        f_pContSessContext->asfContext.fIsPlayReady = FALSE;

        /*
        ** We could end up here if an above step set the content header but
        ** failed to bind. As such, we need to clear the old header.
        */
        ChkDR( Drm_Reinitialize( f_pContSessContext->pAppContext ) );

        /* Use the content encryption data to retrieve license information. */
        dr = NDT_RetrieveLicenseDataV1( f_pContSessContext,
                                        AsfFileInfo.pContentEncObj->pbKeyID,
                                        AsfFileInfo.pContentEncObj->cbKeyID,
                                        AsfFileInfo.pContentEncObj->pbSecretData,
                                        AsfFileInfo.pContentEncObj->cbSecretData,
                                        AsfFileInfo.pContentEncObj->pbLicenseURL,
                                        AsfFileInfo.pContentEncObj->cbLicenseURL,
                                        &g_dstrDRM_RIGHT_PLAYBACK );

        if( dr == DRM_E_V1_LICENSE_CHAIN_NOT_SUPPORTED
         && DRM_FAILED( drSaved ) )
        {
            /*
            ** If we fell back from a PlayReady or V2 content header
            ** and failed here because the V1 header doesn't support
            ** license chains, propogate the error we originally got.
            */
            dr = drSaved;
        }

        /* If the license isn't found for this header, then keep processing. */
        /* Otherwise, break on any other errors. */
        if ( ( dr != DRM_E_LICENSE_NOT_FOUND ) &&
             ( dr != DRM_E_RIGHTS_NOT_AVAILABLE ) &&
             ( dr != DRM_E_LICENSE_EXPIRED ) &&
             ( dr != DRM_E_CLK_NOT_SET ) &&
             ( dr != DRM_E_NO_CLK_SUPPORTED ) )
        {
            ChkDR( dr );

            fBound = TRUE;
        }
    }

    if ( fHaveV1 || fHaveV2 || fHavePR )
    {
        /* The content is encrypted, so set the appropriate parameters. */
        f_pContSessContext->asfContext.dwDRMStatus   = NDT_DRM_STATUS_PROTECTED;
        f_pContSessContext->asfContext.fEncryptClear = FALSE;

        if ( !fBound )
        {
            f_pContSessContext->asfContext.dwLicenseStatus = DRM_LICENSE_STATE_NORIGHT;
        }
    }

    else if ( f_fEncryptClear )
    {
        ChkDR( OEM_NDT_GetLicenseOPLs( &f_pContSessContext->asfContext.playOPLs.minOPL,
                                       &f_pContSessContext->asfContext.fOPLValid ) );

        ChkDR( OEM_NDT_GetLicenseInclusionList( &f_pContSessContext->asfContext.InclusionList,
                                                &f_pContSessContext->asfContext.fInclusionListValid ) );

        f_pContSessContext->asfContext.dwDRMStatus            = NDT_DRM_STATUS_PROTECTED;
        f_pContSessContext->asfContext.dwLicenseStatus        = NDT_DRM_LICENSE_ALLOWS_PLAYBACK;
        f_pContSessContext->asfContext.fEncryptClear          = TRUE;
        f_pContSessContext->asfContext.fCommitted             = TRUE;
        f_pContSessContext->asfContext.ParserState.fEncrypted = TRUE;
    }

ErrorExit:
    /* Clean up file information */
    NDTMGR_ASF_Internal_ClearFileInfo( &AsfFileInfo );

    Oem_File_SetFilePointer( f_pContSessContext->asfContext.hInFile, 0, OEM_FILE_BEGIN, NULL );

    if ( ( dr == DRM_E_LICENSE_NOT_FOUND )
      || ( dr == DRM_E_RIGHTS_NOT_AVAILABLE )
      || ( dr == DRM_E_CLK_NOT_SET )
      || ( dr == DRM_E_NO_CLK_SUPPORTED ) )
    {
        dr = DRM_E_LICENSE_NOT_FOUND;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

