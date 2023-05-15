/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmliceval.h>
#include <drmapprevocation.h>
#include <drmlicreason.h>
#include <drmwmdrm.h>
#include <drmlicacq.h>

ENTER_PK_NAMESPACE_CODE;

/*
**
*/
DRM_API DRM_RESULT DRM_CALL DRM_LA_EvaluateLicense(
    IN     DRM_LICEVAL_CONTEXT *f_pcontextLEVL,
    IN     DRM_DST             *f_pDatastore )
{
    DRM_BYTE         rgbLicensePassword [__CB_DECL(DRM_SHA1_DIGEST_LEN)] = { 0x00 };
    DRM_BOOL         fOK       = FALSE;
    DRM_RESULT       dr        = DRM_SUCCESS;
    DRM_LONG         lReasonForFailure  = 0;

    ChkArg (f_pcontextLEVL != NULL
        &&  f_pDatastore   != NULL);

    if ( f_pcontextLEVL->fLicenseIsXMR )
    {
        ChkArg( f_pcontextLEVL->plicenseXMR != NULL );
    }
    else
    {
        ChkDRMString( &f_pcontextLEVL->dstrContentLicense );
    }

    /*
    **  Cache the reason for failure in case we fail before any of the
    **  DRM_LEVL_PerformOperations
    */
    lReasonForFailure = f_pcontextLEVL->lReasonForFail;

    if ( DRM_APPREVOCATION_IsAppRevocationSupported() && !f_pcontextLEVL->fLicenseIsXMR )
    {
        DRM_BOOL fUpdatedRevocationList = FALSE;

        fUpdatedRevocationList = f_pcontextLEVL->idCRLsCurrent.fUpdatedRevocationList;

        ChkDR(DRM_RVK_PerformAppRevocationCheck(&f_pcontextLEVL->pcontextBBX->CryptoContext,
                                                 f_pcontextLEVL->pContextRev,
                                                &f_pcontextLEVL->dstrContentLicense,
                                                &f_pcontextLEVL->appcert,
                                                 f_pcontextLEVL->pbRevocationBuffer,
                                                 f_pcontextLEVL->cbRevocationBuffer,
                                                &f_pcontextLEVL->idCRLsCurrent));

        f_pcontextLEVL->idCRLsCurrent.fUpdatedRevocationList = (fUpdatedRevocationList || f_pcontextLEVL->idCRLsCurrent.fUpdatedRevocationList);
    }

    /* construct lic password */
    ChkDR(DRM_SST_CreateLicenseStatePassword(&f_pcontextLEVL->LID, rgbLicensePassword, f_pcontextLEVL->pcontextBBX));
    ChkDR(DRM_SST_OpenKeyTokens( f_pcontextLEVL->pcontextSSTLicense,
                                &f_pcontextLEVL->LID,
                                 NULL,
                                 rgbLicensePassword,
                                 DRM_SECURE_STORE_OPEN_ALWAYS,
                                 SECURE_STORE_LICENSE_DATA,
                                 f_pDatastore));

    /* call License Eval */
    f_pcontextLEVL->fReserved              = FALSE;
    f_pcontextLEVL->dwFlags                = LICEVAL_STORE_CHECK_SETTINGS;

    dr = DRM_LEVL_PerformOperations( f_pcontextLEVL,
                                     DRM_LICENSE_EVAL_STORE,
                                     DRM_LICENSE_EVAL_CAN_DO_OPERATION,
                                     NULL,
                                    &fOK,
                                     NULL,
                                     f_pDatastore,
                                     NULL,
                                     NULL );
    /*
    **  Cache the reason for failure. We want to return this, and not the reason
    **  for failure we get from ONSELECT
    */
    lReasonForFailure = f_pcontextLEVL->lReasonForFail;
    ChkDR( dr );

    /* check to see if the license is selectable */
    if ( fOK )
    {
        /* save the content header */
        DRM_CONST_STRING dstrTemp = f_pcontextLEVL->dstrContentHeader;

        /* set content header NULL to avoid KID matching */
        f_pcontextLEVL->dstrContentHeader.pwszString = NULL;
        f_pcontextLEVL->dstrContentHeader.cchString = 0;
        dr = DRM_LEVL_PerformOperations( f_pcontextLEVL,
                                         DRM_LICENSE_EVAL_SELECT,
                                         DRM_LICENSE_EVAL_CAN_DO_OPERATION,
                                         NULL,
                                         &fOK,
                                         NULL,
                                         f_pDatastore,
                                         NULL,
                                         NULL );

        /* restore the content header */
        f_pcontextLEVL->dstrContentHeader = dstrTemp;
        ChkDR(dr);
    }

    if ( !fOK )
    {
        /*
        **  We need to make sure that the reason for failure is set correctly.
        **  This is a workaround to compensate for the fact that v9
        **  licenses do not set the reason correctly sometimes.
        */
        if ( f_pcontextLEVL->lReasonForFail == 0 )
        {
            /* No reason given by license. Try parsing to get the reason, if we can. */
            (void) DRM_WMDRM_LEVL_GetLicenseReasonForUnusable( f_pcontextLEVL,
                                                              &f_pcontextLEVL->lReasonForFail );
        }

        /* for the following cases, do not save the license to store. */
        if (   f_pcontextLEVL->lReasonForFail == LR_LICENSE_EXPIRED
            || f_pcontextLEVL->lReasonForFail == LR_LICENSE_STORE_NOT_ALLOWED
            || f_pcontextLEVL->lReasonForFail == LR_LICENSE_CERT_EXPIRED)
        {
            lReasonForFailure = f_pcontextLEVL->lReasonForFail;
            dr = DRM_E_FAILED_TO_STORE_LICENSE; /* License store not allowed  */
        }
    }

    /* ignore the error code here */
    (void)DRM_SST_CloseKey(f_pcontextLEVL->pcontextSSTLicense, f_pDatastore, NULL, NULL);

ErrorExit:
    if ( f_pcontextLEVL != NULL )
    {
        f_pcontextLEVL->lReasonForFail = lReasonForFailure;
    }

    return dr;
}

EXIT_PK_NAMESPACE_CODE;

