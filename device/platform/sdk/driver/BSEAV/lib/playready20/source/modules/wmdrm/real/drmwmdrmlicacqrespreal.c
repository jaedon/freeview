/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmwmdrm.h>
#include <drmlicacq.h>
#include <drmsymopt.h>
#include <drmutilitieslite.h>
#include <drmxmlparser.h>
#include <drmsyncstore.h>
#include <drmchain.h>
#include <drmwmdrmconstants.h>

ENTER_PK_NAMESPACE_CODE;

/******************************************************************************
** Function:    DRM_WMDRM_LA_ProcessResponse
** Synopsis:    Process license request response received from license server
** Arguments:   [f_pbResponse] -- Response string received from license server
**              [f_cbResponse] -- size of pbResponse in bytes.
**              [f_pLicEvalContext] --pointer to the context of license eval object
**              [f_pbLicenseStoreContext] --pointer to the context of license store object
**              [f_pInclusionListCallback]  -- pointer to the inclusion list callback structure
**              [f_pfnCallback] -- pointer to a callback function if the caller wants notification of successfull license stores
**              [f_pvCallbackContext] -- pointer to a context that will be passed to the callback function.
**              [f_pDatastore] --pointer to the current DST context that should be used
**              [f_pcontextViewRights] --pointer to a view rights context.  This is only used if Sync Support is enabled.
**              [f_rgbLicenseBuffer] -- Pointer to a buffer of size DRM_MAX_LICENSESIZE bytes.  This will be used as a temporary working buffer.
**              [f_plReason] -- to receive the reason code
** Returns:     DRM_SUCCESS on success, or failed with
**              DRM_E_INVALIDARG,
**              DRM_E_INVALIDLICENSE, and
**              error codes from License store.
** Notes:       This function process each license(s) contained in the response
**              received from the license server. It tries to evaluate each license to see
**              if it can store in the license store.
** Sample Response:
**
** <LICENSERESPONSE>
**     <LICENSE version="2.0.0.0">
**         {base64-encoded DRMv7 License}
**     </LICENSE>
**     ...
**     <LICENSE version="2.0.0.0">
**         {base64-encoded DRMv7 License}
**     </LICENSE>
** </LICENSERESPONSE>
******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_WMDRM_LA_ProcessResponse(
    __in_bcount( f_cbResponse ) DRM_BYTE *f_pbResponse,
    IN     DRM_DWORD                             f_cbResponse,
    IN     DRM_LICEVAL_CONTEXT                  *f_pcontextLEVL,
    IN     DRM_LICSTORE_CONTEXT                 *f_pcontextLSTXML,
    IN     DRM_LICSTORE_CONTEXT                 *f_pcontextLSTXMR,
    __in   DRM_LICSTOREENUM_CONTEXT             *f_pLicStoreEnumContext,
    IN     DRM_INCLUSION_LIST_CALLBACK_STRUCT   *f_pInclusionListCallback,
    IN     pfnLicenseResponseCallback            f_pfnCallback,
    IN     DRM_VOID                             *f_pvCallbackContext,
    IN     DRM_DST                              *f_pDatastore,
    IN     DRM_VIEW_RIGHTS_CONTEXT              *f_pcontextASD,
    __in_bcount( DRM_MAX_LICENSESIZE ) DRM_BYTE f_rgbLicenseBuffer[__CB_DECL(DRM_MAX_LICENSESIZE)],
    IN     DRM_SYNC_CONTEXT                     *f_pcontextSync,
       OUT DRM_LONG                             *f_plReason)
{
    DRM_RESULT            dr                = DRM_SUCCESS;
    DRM_RESULT            drReason          = DRM_SUCCESS;
    DRM_DWORD             iNode             = 0;
    DRM_DWORD             cStored           = 0;
    DRM_SUBSTRING         dasstrLicenseList = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrLicenseData = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrNodeData    = EMPTY_DRM_SUBSTRING;
    DRM_SUBSTRING         dasstrTagData     = EMPTY_DRM_SUBSTRING;
    DRM_SLKDATA           slkdata           = { 0 };
    DRM_BOOL              fSLKValid         = FALSE;
    DRM_BOOL              fTriedLoadingSLK  = FALSE;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_DRMLICACQRESP, PERF_FUNC_DRM_WMDRM_LA_ProcessResponse);

    ChkArg( f_pcontextLEVL         != NULL );
    ChkArg( f_pcontextLSTXML       != NULL );
    ChkArg( f_pcontextLSTXMR       != NULL );
    ChkArg( f_pbResponse           != NULL );
    ChkArg( f_pDatastore           != NULL );
    ChkArg( f_rgbLicenseBuffer     != NULL );
    ChkArg( f_cbResponse           != 0    );
    ChkArg( f_pLicStoreEnumContext != NULL );

    if( f_pfnCallback != NULL )
    {
        /* The Inclusion List Callback struct is only needed if they want us to call them back */
        ChkArg( f_pInclusionListCallback != NULL );
    }

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
        ChkArg( f_pcontextASD  != NULL
             && f_pcontextSync != NULL );
    }

    /* extract the license list */
    dasstrLicenseData.m_ich = 0;
    dasstrLicenseData.m_cch = f_cbResponse;

    ChkDR(DRM_XML_GetNodeA((DRM_CHAR*) f_pbResponse,
                            &dasstrLicenseData,
                            &g_adstrLicenseRespTag,
                             NULL,
                             NULL,
                             0,
                             NULL,
                            &dasstrLicenseList));

    dasstrLicenseData.m_ich = 0;
    dasstrLicenseData.m_cch = f_cbResponse;

    /* for each license in dastrLicenseList, Add license to license store */
    for (iNode = 0; ; iNode++)
    {
        DRM_CONST_STRING dstrKID         = EMPTY_DRM_STRING;
        DRM_BYTE        *pbLicense       = NULL;
        DRM_DWORD        dwPriority      = 0;
#if SIXTEEN_BIT_ADDRESSING
        DRM_BYTE         bHead           = 0;
        DRM_BOOL         fDecryptOK      = FALSE;
#endif
        dr = DRM_XML_GetNodeA((DRM_CHAR*) f_pbResponse,
                             &dasstrLicenseList,
                             &g_dastrTagLicense,
                              NULL,
                              NULL,
                              iNode,
                             &dasstrTagData,
                             &dasstrNodeData);

        /* A <LICENSE> node was found. */

        if (DRM_SUCCEEDED(dr))
        {
            DRM_DWORD     cbVersionLicense     = 0;
            DRM_SUBSTRING dasstrVersionLicense = { 0 };

            /* Check the version information */
            dr = DRM_XML_GetNodeAttributeA((DRM_CHAR*) f_pbResponse,
                                           &dasstrTagData,
                                           &g_dastrAttributeVersion,
                                           &dasstrVersionLicense);

            if (DRM_FAILED(dr))
            {
                continue;
            }
            else
            {
                {
                    /*
                    **  Check to see if there is an "encrypted" attribute.
                    **  If this value is 'false' then the license is directly inline.
                    **  If 'true' or non-existent then the license is B64 encoded and
                    **  privacy encrypted.
                    */

                    dr = DRM_XML_GetNodeAttributeA((DRM_CHAR*) f_pbResponse,
                                                   &dasstrTagData,
                                                   &g_dastrAttributeEncrypted,
                                                   &dasstrVersionLicense);

                    drReason = DRM_B64_DecodeA((DRM_CHAR *) f_pbResponse,
                                                &dasstrNodeData,
                                                &cbVersionLicense,
                                                 NULL,
                                                 DRM_BASE64_DECODE_IN_PLACE);

                    if (DRM_FAILED(drReason))
                    {
                        drReason = DRM_E_INVALIDLICENSE;
                        continue;
                    }

                    if(  DRM_FAILED( dr )
                     || !DRM_UTL_DASSTRStringsEqual((DRM_CHAR *) f_pbResponse,
                                                    &dasstrVersionLicense,
                                                    &g_dastrAttributeFALSE))
                    {
                        /* Attribute doesn't exist or it isn't set to FALSE */

                        /* License is base64 decoded.  Now it should be decrypted. */

#if SIXTEEN_BIT_ADDRESSING
                        /* DecryptLicense expects an aligned CHAR* pointer, not a packed one; rather than
                        change its interface this code temporarily moves a packed buffer into alignment */

                        if ((dasstrNodeData.m_ich % CB_NATIVE_BYTE) != 0)
                        {
                            pbLicense = f_pbResponse + (dasstrNodeData.m_ich & ~CB_NATIVE_BYTE);

                            DRM_16B_IncrementPackedPointer(pbLicense, dasstrNodeData.m_cch, &bHead);
                        }
                        else
                        {
                            pbLicense = f_pbResponse + __CB_DECL(dasstrNodeData.m_ich);
                        }

                        if( cbVersionLicense                             > DRM_ECC160_CIPHERTEXT_LEN
                         && cbVersionLicense - DRM_ECC160_CIPHERTEXT_LEN > DRM_MAX_LICENSESIZE )
                        {
                            drReason = DRM_E_BUFFERTOOSMALL;
                            continue;
                        }

                        fDecryptOK = DRM_BBX_DecryptLicense(pbLicense, cbVersionLicense, f_rgbLicenseBuffer + __CB_DECL( SIZEOF( DRM_DWORD) ), f_pcontextLEVL->pcontextBBX, NULL);

                        if ((dasstrNodeData.m_ich % CB_NATIVE_BYTE) != 0)
                        {
                            DRM_16B_DecrementPackedPointer(pbLicense, dasstrNodeData.m_cch, bHead);
                        }

                        if (! fDecryptOK)
                        {
                            drReason = DRM_E_INVALIDLICENSE;
                            continue;
                        }
#else
                        /* must always add this offset when ANSI B64 decoding in place */

                        pbLicense = f_pbResponse + dasstrNodeData.m_ich;
                        if( cbVersionLicense                             > DRM_ECC160_CIPHERTEXT_LEN
                         && cbVersionLicense - DRM_ECC160_CIPHERTEXT_LEN > DRM_MAX_LICENSESIZE )
                        {
                            drReason = DRM_E_BUFFERTOOSMALL;
                            continue;
                        }

                        ChkArg( dasstrNodeData.m_ich + cbVersionLicense <= f_cbResponse );

                        if(! DRM_BBX_DecryptLicense(pbLicense, cbVersionLicense, f_rgbLicenseBuffer + __CB_DECL( SIZEOF( DRM_DWORD ) ), f_pcontextLEVL->pcontextBBX, NULL))
                        {
                            drReason = DRM_E_INVALIDLICENSE;
                            continue;
                        }
#endif
                        cbVersionLicense -= DRM_ECC160_CIPHERTEXT_LEN;
                    }
                    else
                    {
                        if( cbVersionLicense > DRM_MAX_LICENSESIZE )
                        {
                            drReason = DRM_E_BUFFERTOOSMALL;
                            continue;
                        }

                        /* License is inline.  Copy it to the license buffer */
                        DRM_BYT_CopyBytes( f_rgbLicenseBuffer + __CB_DECL( SIZEOF( DRM_DWORD ) ), 0, f_pbResponse, dasstrNodeData.m_ich, cbVersionLicense );
                    }


#if DEBUG__SAVE_PROTOCOL_INFORMATION
                {
                    /* save the lic to a file */
                    OEM_FILEHDL fp=OEM_INVALID_HANDLE_VALUE;
                    DRM_DWORD dwNumberOfBytesWritten=0;

                    fp = Oem_File_Open(
                            NULL,
                            L"c:\\licresp.xml",
                            OEM_GENERIC_READ|OEM_GENERIC_WRITE,
                            OEM_FILE_SHARE_READ|OEM_FILE_SHARE_WRITE,
                            OEM_CREATE_NEW,
                            OEM_ATTRIBUTE_NORMAL);
                    if ( fp != OEM_INVALID_HANDLE_VALUE )
                    {
                        Oem_File_Write(fp, f_rgbLicenseBuffer + __CB_DECL( SIZEOF( DRM_DWORD ) ), cbVersionLicense, &dwNumberOfBytesWritten);
                        Oem_File_Close(fp);
                    }
                }
#endif

                    ChkDR( DRM_SYMOPT_LIC_UpdateLicenseWithSymmetricDataDuringLacq(  f_rgbLicenseBuffer,
                                                                                    &dstrKID,
                                                                                    &cbVersionLicense,
                                                                                    &fSLKValid,
                                                                                    &fTriedLoadingSLK,
                                                                                     f_pDatastore,
                                                                                     f_pcontextLEVL,
                                                                                     f_pbResponse,
                                                                                     f_cbResponse,
                                                                                    &dasstrLicenseList,
                                                                                    &slkdata ) );

                    DSTR_FROM_PB( &f_pcontextLEVL->dstrContentLicense,
                                  f_rgbLicenseBuffer + __CB_DECL( SIZEOF( DRM_DWORD ) ),
                                  cbVersionLicense );
                    f_pcontextLEVL->fUseCachedAttribs = FALSE;

                    {
                        DRM_CONST_STRING dstrEmptyLIData = EMPTY_DRM_STRING;

                        ChkDRContinue( DRM_WMDRM_LIC_GetAttribute( &f_pcontextLEVL->dstrContentLicense,
                                                                    NULL,
                                                                    DRM_LICENSE_ATTRIB_PRIORITY,
                                                                   &dstrEmptyLIData,
                                                                   &dstrKID,
                                                                    0 ) );

                        ChkDRContinue( DRMCRT_wcsntol( dstrKID.pwszString, dstrKID.cchString, (DRM_LONG *) &dwPriority ) );

                        ChkDRContinue( DRM_WMDRM_LIC_GetAttribute( &f_pcontextLEVL->dstrContentLicense,
                                                                    NULL,
                                                                    DRM_LICENSE_ATTRIB_LID,
                                                                   &dstrEmptyLIData,
                                                                   &dstrKID,
                                                                    0 ) );
                        ChkDRContinue( DRM_UTL_StringToGuid( &dstrKID, (DRM_GUID*)f_pcontextLEVL->LID.rgb ) );

                        ChkDRContinue( DRM_WMDRM_LIC_GetAttribute( &f_pcontextLEVL->dstrContentLicense,
                                                                    NULL,
                                                                    DRM_LICENSE_ATTRIB_KID,
                                                                   &dstrEmptyLIData,
                                                                   &dstrKID,
                                                                    0 ) );
                        ChkDRContinue( DRM_UTL_DecodeKID( &dstrKID, &f_pcontextLEVL->KID ) );
                        /* Do not change dstrKID after this call.  Later code depends on it really pointing to the KID */

                        f_pcontextLEVL->fUseCachedAttribs = TRUE;
                    }
                }

                drReason = DRM_LA_EvaluateLicense( f_pcontextLEVL,
                                                   f_pDatastore );

                if ( f_plReason != NULL )
                {
                    *f_plReason = f_pcontextLEVL->lReasonForFail;
                }

                /* if they gave us a callback then we need to send over the inclusion GUIDs */
                if ( DRM_SUCCEEDED( drReason )
                    && f_pfnCallback != NULL)
                {
                    ChkDR( DRM_WMDRM_LIC_GetInclusionList( &f_pcontextLEVL->dstrContentLicense,
                                                            f_pInclusionListCallback->rgInclusionList,
                                                            f_pInclusionListCallback->rgfInclusionListValid,
                                                            0 ) );

                    drReason = f_pfnCallback( (DRM_VOID*)f_pInclusionListCallback, DRM_STORE_LICENSE_NOTIFY_INCLUSION_LIST, f_pvCallbackContext );
                }

                /* store the license if the license evaluated ok */
                if (DRM_SUCCEEDED(drReason) )
                {
                    if( f_pcontextLEVL->fLicenseIsXMR )
                    {
                        drReason = DRM_LST_AddLicense(f_pcontextLSTXMR,
                                                      cbVersionLicense,
                                                      pbLicense,
                                                     &f_pcontextLEVL->KID,
                                                     &f_pcontextLEVL->LID,
                                                      dwPriority);
                    }
                    else
                    {
                        drReason = DRM_LST_AddLicense(f_pcontextLSTXML,
                                                      CB_DSTR( &f_pcontextLEVL->dstrContentLicense ),
                                                      f_rgbLicenseBuffer,
                                                     &f_pcontextLEVL->KID,
                                                     &f_pcontextLEVL->LID,
                                                      dwPriority);
                    }

                    if (DRM_SUCCEEDED(drReason))
                    {
                        cStored++;

                        if (f_pfnCallback != NULL)
                        {
                            f_pfnCallback( (DRM_VOID*) &dstrKID, DRM_STORE_LICENSE_NOTIFY_KID, f_pvCallbackContext);
                        }
                    }
                } /* end if EvaluateLicense () succeeded */

                if( DRM_SNC_IsLicenseSyncSupported() )
                {
                    /* don't stop the license processing loop on a synclist failure */
                    ChkDRContinue( DRM_SNC_UpdateKID(f_pcontextSync, f_pcontextASD, &f_pcontextLEVL->KID, NULL, DRM_SYNC_UPDATE_AGGREGATE ) );
                }

                ChkArg( f_cbResponse >= dasstrNodeData.m_ich + dasstrNodeData.m_cch );

                /* Fill the data back in with what could be valid base64.  If not the decoded license may
                    cause further calls into the xml parser to fail */

                DRM_BYT_SetBytes(f_pbResponse,
                                 dasstrNodeData.m_ich,
                                 dasstrNodeData.m_cch,
                                 'a');
            }
        } /* end if got license tag */
        else
        {
            if (dr == DRM_E_XMLNOTFOUND)
            {
                /* no more license found */
                ChkDR(DRM_SUCCESS);  /* This sets 'dr' and keeps the PREfast tool happy. */
                break;
            }

            ChkDR(dr);  /* other errors */
        }
    } /* end for license */

    /* Explicitly make the global secure store writeabele */
    f_pcontextLEVL->fGlobalSecStoreWritable = TRUE;

    ChkDR(DRM_LIC_CheckClockRollback( f_pcontextLEVL,
                                      f_pcontextLSTXML,
                                      f_pcontextLSTXMR,
                                      f_pLicStoreEnumContext,
                                      f_pbResponse,
                                      f_cbResponse,
                                      f_pDatastore,
                                      f_rgbLicenseBuffer,
                                      DRM_MAX_LICENSESIZE ));

ErrorExit:

    if (DRM_SUCCEEDED(dr))
    {
        if (iNode == 1 && cStored == 0)
        {
            if (DRM_FAILED(drReason))
            {
                /* Only one license in response. No license was stored. */
                dr = drReason;
            }
            else
            {
                dr = DRM_E_FAILED_TO_STORE_LICENSE;
            }
        }
        else if (iNode > 1 && cStored == 0)
        {
            /* None were stored. Return usual error as done in past. */
            dr = DRM_E_FAILED_TO_STORE_LICENSE;
        }
        else if (iNode > 1 && cStored < iNode)
        {
            /* Multiple licenses in response. One or more licenses were not stored. */
            dr = DRM_E_NOT_ALL_STORED;
        }
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

