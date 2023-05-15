/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmsymopt.h>
#include <drmxmlparser.h>
#include <drmlicacqv3constants.h>
#include <drmsymoptconstants.h>

ENTER_PK_NAMESPACE_CODE;

DRM_API DRM_BOOL DRM_CALL DRM_SYMOPT_LA_PrepareSLKFromLicense(
    IN           DRM_DST                              *f_pDatastore,
    IN           DRM_LICEVAL_CONTEXT                  *f_pcontextLEVL,
    IN     const DRM_CHAR                             *f_pbResponse,
    IN           DRM_SUBSTRING                        *f_pdasstrLicenseList,
    OUT          DRM_SLKDATA                          *f_pslkdata )
{
    DRM_RESULT            dr                = DRM_SUCCESS;
    DRM_SUBSTRING         dasstrNodeData    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrTagData     = EMPTY_DRM_SUBSTRING;
    DRM_ID                idSLKNew = { 0 };
    DRM_DWORD             cbidSLK  = SIZEOF( idSLKNew );
    DRM_BYTE              rgbSLKKey[__CB_DECL( DRM_ECC160_CIPHERTEXT_LEN )] = { 0 };
    DRM_BOOL              fSLKValid = FALSE;

    dr = DRM_SYMOPT_LA_GetSLK( f_pslkdata, f_pDatastore, f_pcontextLEVL );

    if ( DRM_SUCCEEDED( dr )
      && f_pslkdata->slk.cbKey == DES_KEYSIZE )
    {
        fSLKValid = TRUE;
    }

    ChkDR( DRM_XML_GetNodeA((DRM_CHAR*) f_pbResponse,
                           f_pdasstrLicenseList,
                          &g_dastrTagSLK,
                           NULL,
                           NULL,
                           0,
                           NULL,
                          &dasstrTagData ) );

    /* Extract the SLK ID from the response */
    ChkDR( DRM_XML_GetNodeA( (DRM_CHAR*) f_pbResponse,
                          &dasstrTagData,
                          &g_dastrTagSLKID,
                           NULL,
                           NULL,
                           0,
                           NULL,
                          &dasstrNodeData ) );

    ChkDR( DRM_B64_DecodeA((DRM_CHAR *) f_pbResponse,
                         &dasstrNodeData,
                         &cbidSLK,
                         (DRM_BYTE*)&idSLKNew,
                          0) );

    if( !fSLKValid
      || MEMCMP( f_pslkdata->idSLK.rgb, idSLKNew.rgb, SIZEOF( idSLKNew ) ) != 0 )
    {
        /* New SLK.  Extract the SLK and decrypt */
        ChkDR( DRM_XML_GetNodeA( (DRM_CHAR*) f_pbResponse,
                               &dasstrTagData,
                               &g_dastrTagSLKDATA,
                                NULL,
                                NULL,
                                0,
                                NULL,
                               &dasstrNodeData ) );

        cbidSLK = SIZEOF( rgbSLKKey );
        ChkDR( DRM_B64_DecodeA((DRM_CHAR *) f_pbResponse,
                             &dasstrNodeData,
                             &cbidSLK,
                              rgbSLKKey,
                              0) );

        ChkFAIL( cbidSLK == SIZEOF( rgbSLKKey ) );

        fSLKValid = TRUE;

        ChkDR( DRM_SYMOPT_LA_UpdateSLK( &idSLKNew,
                                        rgbSLKKey,
                                        DRM_ECC160_CIPHERTEXT_LEN,
                                        f_pDatastore,
                                        f_pcontextLEVL,
                                        f_pslkdata ) );
    }

ErrorExit:
    /* Whether our output SLK is valid is all the caller cares about */
    return fSLKValid;
}

DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_LIC_UpdateLicenseWithSymmetricDataDuringLacq(
    __in_bcount( DRM_MAX_LICENSESIZE )  DRM_BYTE             f_rgbLicenseBuffer[__CB_DECL(DRM_MAX_LICENSESIZE)],
    __inout                             DRM_CONST_STRING    *f_pdstrKID,
    __inout                             DRM_DWORD           *f_pcbVersionLicense,
    __inout                             DRM_BOOL            *f_pfSLKValid,
    __inout                             DRM_BOOL            *f_pfTriedLoadingSLK,
    __in                                DRM_DST             *f_pDatastore,
    __in                                DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    __in_bcount( f_cbResponse )         DRM_BYTE            *f_pbResponse,
    __in                                DRM_DWORD            f_cbResponse,
    __inout                             DRM_SUBSTRING       *f_pdasstrLicenseList,
    __inout                             DRM_SLKDATA         *f_pslkdata )
{
    DRM_RESULT          dr          = DRM_SUCCESS;
    DRM_CONST_STRING    dstrTemp    = EMPTY_DRM_STRING;

    UNREFERENCED_PARAMETER( f_cbResponse ); /* Only passed in as a parameter to satisfy SAL */

    DSTR_FROM_PB( f_pdstrKID, f_rgbLicenseBuffer + __CB_DECL( SIZEOF( DRM_DWORD ) ), *f_pcbVersionLicense );

    /* Only bother with searching for the SLK if there's a SymSig XML node in the license */
    if( DRM_SUCCEEDED( DRM_XML_GetSubNodeByPath( f_pdstrKID,
                                &g_dstrXPathSymSig,
                                    NULL,
                                    NULL,
                                    NULL,
                                &dstrTemp,
                                    g_wchForwardSlash ) ) )
    {
        if( !(*f_pfSLKValid) && !(*f_pfTriedLoadingSLK) )
        {
            *f_pfSLKValid = DRM_SYMOPT_LA_PrepareSLKFromLicense( f_pDatastore, f_pcontextLEVL, (DRM_CHAR*)f_pbResponse, f_pdasstrLicenseList, f_pslkdata );
            *f_pfTriedLoadingSLK = TRUE;
        }
    }

    ChkDR( DRM_SYMOPT_LIC_UpdateLicenseWithSymmetricData( (DRM_STRING*)f_pdstrKID,
                                                          DRM_MAX_LICENSESIZE,
                                                          f_pcontextLEVL,
                                                          FALSE,
                                                          (*f_pfSLKValid) ? &f_pslkdata->slk : NULL ) );
    *f_pcbVersionLicense = CB_DSTR( f_pdstrKID );

ErrorExit:
    return dr;
}

/******************************************************************************
** Function:    DRM_SYMOPT_LA_UpdateSLK
** Synopsis:    Uses the SLK Data parameter to locate and update the session key
**              in the SST
**
** Arguments:   [f_pSLKID]       : Contains the new SLK ID
**              [f_pbSLKKey]     : Contains the encrypted SLK key from the license response
**              [f_cbKeyLength]  : Contains the length of f_pbSLKKey
**              [f_pDatastore]   : Pointer to the current DST
**              [f_pcontextLEVL] : Pointer to the current liceval context
**              [f_pSLK]         : Contains the current SLK ID information.
**                                 Returns SLK Data containing the Session ID and
**                                 new symmetric session key
**
** Returns:     DRM_SUCCESS
**                  Success
**              DRM_E_INVALIDARG
**                  A required argument is NULL or improperly initialized
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_LA_UpdateSLK(
    IN                                 DRM_ID               *f_pSLKID,
    __in_bcount( f_cbKeyLength ) const DRM_BYTE             *f_pbSLKKey,
    IN                                 DRM_DWORD             f_cbKeyLength,
    IN                                 DRM_DST              *f_pDatastore,
    IN                                 DRM_LICEVAL_CONTEXT  *f_pcontextLEVL,
    OUT                                DRM_SLKDATA          *f_pSLK )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE   rgbPasswordSST[__CB_DECL( DRM_SHA1_DIGEST_LEN )] = { 0 };

    ChkArg( f_pcontextLEVL != NULL );
    ChkArg( f_pSLK         != NULL );
    ChkArg( f_pSLKID       != NULL );

    ChkDR( DRM_BBX_RebindSLK( f_pcontextLEVL->pcontextBBX,
                              f_pbSLKKey,
                              f_cbKeyLength,
                              &f_pSLK->slk ) );

    MEMCPY( &f_pSLK->idSLK, f_pSLKID, SIZEOF( DRM_ID ) );

    ChkDR( DRM_SST_CreateLicenseStatePassword( &g_idSLKSST,
                                               rgbPasswordSST,
                                               f_pcontextLEVL->pcontextBBX) );
    ChkDR( DRM_SST_SetData( f_pcontextLEVL->pcontextSSTLicense,
                            &g_idSLKSST,
                            NULL,
                            rgbPasswordSST,
                            SECURE_STORE_GLOBAL_DATA,
                            f_pDatastore,
                            ( DRM_BYTE * )f_pSLK,
                            SIZEOF( DRM_SLKDATA ) ) );

ErrorExit:
    return dr;
}

/******************************************************************************
** Function:    DRM_SYMOPT_LA_GetSLK
** Synopsis:    Retrieves the current SLK from the secure store
** Arguments:   [f_pSLK]         : Returns SLK Data containing the Session ID and symmetric session key
**              [f_pDatastore]   : Pointer to the current DST
**              [f_pcontextLEVL] : Pointer to the current liceval context
**
** Returns:     DRM_SUCCESS
**                  Success
**              DRM_E_INVALIDARG
**                  f_pSLK is NULL
**              DRM_E_INVALID_SLK
**                  A matching session key was found, but contained invalid data
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_LA_GetSLK(
    OUT     DRM_SLKDATA           *f_pSLK,
    IN      DRM_DST               *f_pDatastore,
    IN      DRM_LICEVAL_CONTEXT   *f_pcontextLEVL )
{
    DRM_RESULT dr    = DRM_SUCCESS;
    DRM_DWORD  cbSLK = SIZEOF( DRM_SLKDATA );
    DRM_BYTE   rgbPasswordSST[__CB_DECL( DRM_SHA1_DIGEST_LEN )] = { 0 };

    ChkArg( f_pcontextLEVL != NULL );

    ChkDR( DRM_SST_CreateLicenseStatePassword( &g_idSLKSST,
                                               rgbPasswordSST,
                                               f_pcontextLEVL->pcontextBBX ) );

    ChkDR( DRM_SST_GetData( f_pcontextLEVL->pcontextSSTLicense,
                            &g_idSLKSST,
                            NULL,
                            rgbPasswordSST,
                            SECURE_STORE_GLOBAL_DATA,
                            f_pDatastore,
                            ( DRM_BYTE * )f_pSLK,
                            &cbSLK ) );

    ChkBOOL( cbSLK == SIZEOF( DRM_SLKDATA ), DRM_E_INVALID_SLK );

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: DRM_SYMOPT_LA_ExtractSLKDataFromLicenseResponse
**
** Synopsis: Function that extracts SLK data from a PC response
**           string and then processes and returns the ID
**
** Arguments:
**
** [f_poLicRespContext]     -- Pointer to a data structure that contains
**                             information needed to process licenses.
** [f_pszResponse]          -- Pointer to a character buffer that contains
**                             the server response.
** [f_pdasstrResponse]      -- Pointer to a sub string that defines the License Acq string
**                             ( pre-B64 decoded ) within the server
**                             response buffer.
** [f_pSLKData]             -- Pointer to a variable that receives the SLK data
** [f_pfValidSLK]           -- Returns whether a valid SLK was found
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_INVALID_SLK if an invalid SLK was parsed from response
**
**********************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_SYMOPT_LA_ExtractSLKDataFromLicenseResponse(
    __in  DRM_LICRESP_CONTEXT_V3 *f_poLicRespContext,
    __in  DRM_ANSI_CONST_STRING  *f_pdastrResponse,
    __in  DRM_SUBSTRING          *f_pdasstrResponse,
    __out DRM_SLKDATA            *f_pSLKData,
    __out DRM_BOOL               *f_pfValidSLK )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_SUBSTRING dasstrSLKData = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING dasstrSLKNodeData = EMPTY_DRM_SUBSTRING;
    DRM_ID idSLKNew = { 0 };
    DRM_DWORD cbidSLK = SIZEOF( idSLKNew );
    DRM_BYTE rgbSLKKey[ __CB_DECL( ECC_P256_CIPHERTEXT_SIZE_IN_BYTES ) ] = { 0 };

    ChkArg( f_poLicRespContext != NULL );
    ChkArg( f_pdastrResponse != NULL );
    ChkArg( f_pdasstrResponse != NULL );
    ChkArg( f_pfValidSLK != NULL );
    ChkArg( f_pSLKData != NULL );

    *f_pfValidSLK = FALSE;

    /*
    ** Attempt to read an SLK node: For symmetrically optimized ILA licenses
    */
    ChkBOOL( f_pdastrResponse->cchString >=  f_pdasstrResponse->m_ich + f_pdasstrResponse->m_cch, DRM_E_INVALIDARG );
    dr = DRM_XML_GetSubNodeByPathA( f_pdastrResponse->pszString,
                                    f_pdasstrResponse,
                                    &g_dastrLicSLKPath,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &dasstrSLKData,
                                    g_chForwardSlash );
    if ( DRM_SUCCEEDED( dr ) )
    {
        ChkDR( DRM_XML_GetNodeA( f_pdastrResponse->pszString,
                                 &dasstrSLKData,
                                 &g_dastrTagSLKID,
                                 NULL,
                                 NULL,
                                 0,
                                 NULL,
                                &dasstrSLKNodeData ) );

        ChkDR( DRM_B64_DecodeA( f_pdastrResponse->pszString,
                                &dasstrSLKNodeData,
                                &cbidSLK,
                                ( DRM_BYTE * )&idSLKNew,
                                0 ) );

        /*
        ** Update the SLK if the ID doesn't match the existing one
        */
        if ( DRM_FAILED( DRM_SYMOPT_LA_GetSLK( f_pSLKData,
                                               f_poLicRespContext->m_poDatastore,
                                               f_poLicRespContext->m_poLicEvalContext ) )
         || MEMCMP( &idSLKNew, &f_pSLKData->idSLK, SIZEOF( DRM_ID ) ) != 0 )
        {
            ChkDR( DRM_XML_GetNodeA( f_pdastrResponse->pszString,
                                     &dasstrSLKData,
                                     &g_dastrTagSLKDATA,
                                     NULL,
                                     NULL,
                                     0,
                                     NULL,
                                     &dasstrSLKNodeData ) );

            cbidSLK = ECC_P256_CIPHERTEXT_SIZE_IN_BYTES;

            ChkDR( DRM_B64_DecodeA( f_pdastrResponse->pszString,
                                    &dasstrSLKNodeData,
                                    &cbidSLK,
                                    rgbSLKKey,
                                    0 ) );

            ChkBOOL( cbidSLK == ECC_P256_CIPHERTEXT_SIZE_IN_BYTES,
                     DRM_E_INVALID_SLK );

            ChkDR( DRM_SYMOPT_LA_UpdateSLK( &idSLKNew,
                                            rgbSLKKey,
                                            ECC_P256_CIPHERTEXT_SIZE_IN_BYTES,
                                            f_poLicRespContext->m_poDatastore,
                                            f_poLicRespContext->m_poLicEvalContext,
                                            f_pSLKData ) );
        }

        *f_pfValidSLK = TRUE;
    }
    else if ( dr == DRM_E_XMLNOTFOUND )
    {
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( dr );
    }

ErrorExit:
    return dr;
}

EXIT_PK_NAMESPACE_CODE;

