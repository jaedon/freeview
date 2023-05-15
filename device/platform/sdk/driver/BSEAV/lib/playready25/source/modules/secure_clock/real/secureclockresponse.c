/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_SECURECLOCKRESPONSE_C
#include <drmutilities.h>
#include <drmcontextsizes.h>
#include <drmxmlbuilder.h>
#include <drmxmlparser.h>
#include <drmblackbox.h>
#include <drmlicacq.h>
#include <oemsha1.h>
#include <drmsecureclock.h>
#include <drmcrt.h>
#include <drmantirollbackclock.h>
#include <drmsecureclockconstants.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.")

#define ONE_MIN_IN_FILETIME 10000000*60

/******************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_CLK_ProcessResponse(
    __in                    const PUBKEY                *ppubkeySecureClockServer,
    __in_ecount(cbResponse)       DRM_BYTE              *pbResponse,
    __in                          DRM_DWORD              cbResponse,
    __in                    const DRM_BYTE               rgbSecStoreInPswd [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __out                         DRM_RESULT            *pResult,
    __in                          DRM_CRYPTO_CONTEXT    *pContext,
    __in                          DRM_DST               *poDatastore,
    __in                          DRM_SECSTORE_CONTEXT  *poSecureStoreContext,
    __in                          DRM_LICEVAL_CONTEXT   *poLicEvalContext,
    __in                          DRM_VOID              *pOEMContext )
{
    DRM_RESULT       dr          = DRM_SUCCESS;
    DRM_DWORD        cbDecoded   = 0;
    DRM_DWORD        cbSecureData= 0;
    DRM_UINT64       u64DRMTime;
    DRMSYSTEMTIME    oSystemTime;
    DRMFILETIME      filetime;
    DRM_TID          tid;

    DRM_CONST_STRING dstrResponse          = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrData              = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrSign              = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrCert              = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrValue             = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrErrorNodes        = EMPTY_DRM_STRING;
    DRM_SUBSTRING    dasstrEncoded         = { 0 };
    DRM_SECSTORE_CLOCK_DATA oClkData       = { 0 };
    DRM_BYTE *pbAligned                     = NULL;
    DRM_DWORD cbAligned                     = 0;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_SECURECLOCKRESPONSE, PERF_FUNC_DRM_CLK_ProcessResponse);

    ChkArg( pbResponse           != NULL
         && pContext             != NULL
         && cbResponse            > 0
         && pResult              != NULL
         && rgbSecStoreInPswd    != NULL
         && poDatastore         != NULL
         && poSecureStoreContext != NULL
         && poLicEvalContext     != NULL );

    *pResult = DRM_SUCCESS;
    dasstrEncoded.m_ich = 0;
    dasstrEncoded.m_cch = cbResponse;
    cbDecoded           = cbResponse;

    ChkDR(DRM_B64_DecodeA((DRM_CHAR *) pbResponse,
                          &dasstrEncoded,
                          &cbDecoded,
                           NULL,
                           DRM_BASE64_DECODE_IN_PLACE ) );

    ChkDR( DRM_UTL_EnsureDataAlignment( pbResponse, cbResponse, &pbAligned, &cbAligned, SIZEOF( DRM_DWORD_PTR ) / CB_NATIVE_BYTE, NULL ) );

    if( cbAligned < cbDecoded )
    {
        /* Shouldn't ever happen since the B64 decode gives us lots of extra slack */
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    if( pbAligned != pbResponse )
    {
        DRM_BYT_MoveBytes( pbAligned, 0, pbResponse, 0, cbDecoded );
        pbResponse = pbAligned;
    }


    /* Decode this ANSI encoded blob in place */

    DSTR_FROM_PB(&dstrResponse, pbResponse + dasstrEncoded.m_ich, cbDecoded);

    ChkDR( DRM_XML_GetNode(
            &dstrResponse,
            &g_dstrTagDRMReqRoot,
            NULL,
            NULL,
            0,
            NULL,
            &dstrValue ) );



    if ( DRM_SUCCEEDED( DRM_XML_GetNode( &dstrValue, &g_dstrTagError,  NULL, NULL, 0, NULL, &dstrErrorNodes )  )  )
    {
        /* We have error code */
        ChkDR( DRMCRT_wcsntol( dstrErrorNodes.pwszString, dstrErrorNodes.cchString, pResult ) );
        goto ErrorExit;
    }
    else
    {
        if (dr == DRM_E_XMLNOTFOUND)
        {
            ChkDR(DRM_SUCCESS); /* This sets 'dr' and keeps the PREfast tool happy. */
        }
        else
        {
            ChkDR(dr);  /* other errors */
        }
     }

    /*Here verify the response*/
    /*
    **  Extract the signature, data and cert chain nodes from the response
    */
    ChkDR( DRM_XML_GetNode( &dstrValue, &g_dstrTagSignature,        NULL, NULL, 0, &dstrSign, NULL  ) );
    ChkDR( DRM_XML_GetNode( &dstrValue, &g_dstrTagCertificateChain, NULL, NULL, 0, &dstrCert, NULL ) );
    ChkDR( DRM_XML_GetNode( &dstrValue, &g_dstrTagData,             NULL, NULL, 0, &dstrData, NULL  ) );

    /*
    **  Extract the public key from the devcert
    */

    MEMCPY( pContext->pubKey.y, ppubkeySecureClockServer, SIZEOF( PUBKEY ) );
    ChkDR( DRM_UTL_VerifyXMLSignature( &dstrData, &dstrSign, &dstrCert, TRUE, TRUE, WM_DRM_ROOTPUBKEY_CLK, poLicEvalContext ) );

    /*
    ** Open secure store and Read the Values.
    ** If it cant be opened, FAIL as we dont have any TID to verify with
    */
    ZEROMEM(&oClkData, SIZEOF(oClkData));
    cbSecureData = SIZEOF(oClkData);

    ChkDR( DRM_SST_GetData(
            poSecureStoreContext,
            &g_idSData,
            &g_idSDataPrev,
            rgbSecStoreInPswd,
            SECURE_STORE_GLOBAL_DATA,
            poDatastore,
            (DRM_BYTE *) &oClkData,
            &cbSecureData) );
    _PrepareSecureClockDataForReading( &oClkData );

    ChkDR( DRM_XML_GetNode(
            &dstrValue,
            &g_dstrTagData,
            NULL,
            NULL,
            0,
            NULL,
            &dstrData ) );

    /*Get TID, Decode it and compare it with stored TID*/
    ChkDR( DRM_XML_GetNode(
            &dstrData,
            &g_dstrTagTID,
            NULL,
            NULL,
            0,
            NULL,
            &dstrValue ) );

    cbDecoded = SIZEOF(tid);

    ChkDR(DRM_B64_DecodeW(
            (const DRM_CONST_STRING *)&dstrValue,
                                      &cbDecoded,
                          (DRM_BYTE *)&tid,
                                       0));

    if (cbDecoded != SIZEOF(DRM_TID))
    {
        ChkDR( DRM_E_CLK_INVALID_RESPONSE );
    }

    if (MEMCMP(&tid, &oClkData.tid, SIZEOF(DRM_TID)) != 0 )
    {
        ChkDR( DRM_E_CLK_INVALID_RESPONSE );
    }

    /*Check whether the response is received within 5 minutes */

    /*Get Time and store it in secure store. Response should not come after 5 min of this time*/
    Oem_Clock_GetSystemTime( pOEMContext, &oSystemTime );
#if 0
	printf("#####Func = %s, GetSystemTime To [%04d-%02d-%02d %02d:%02d:%02d.%02d]\n", __FUNCTION__, oSystemTime.wYear, oSystemTime.wMonth, oSystemTime.wDay,
		oSystemTime.wHour,oSystemTime.wMinute,oSystemTime.wSecond, oSystemTime.wMilliseconds);
#endif
    if ( !Oem_Clock_SystemTimeToFileTime( &oSystemTime, &filetime ) )
    {
        ChkDR( DRM_E_CLK_INVALID_DATE );
    }

    FILETIME_TO_UI64( filetime, u64DRMTime );

    if ( !( oClkData.flag & CLK_CHALLENGETIME_VALID )
      || DRM_UI64Les( u64DRMTime, oClkData.ChallengeTime )
      || DRM_UI64Les( DRM_UI64Add( oClkData.ChallengeTime, DRM_UI64Mul(DRM_UI64(5),DRM_UI64(ONE_MIN_IN_FILETIME))), u64DRMTime ) )
    {
        ChkDR( DRM_E_CLK_INVALID_RESPONSE );
    }

    ChkDR( DRM_XML_GetNode(
            &dstrData,
            &g_dstrTagGmtTime,
            NULL,
            NULL,
            0,
            NULL,
            &dstrValue ) );

    if( DRM_FAILED( DRM_UTL_GetZuluTimeFromStringAsUI64( &dstrValue, &oClkData.LastKnownGoodSecureClock ) ) )
    {
        ChkDR( DRM_E_CLK_INVALID_RESPONSE );
    }
    oClkData.flag |= CLK_LK_GSC_VALID;

    MEMCPY(&oClkData.LastKnownRealtimeSecureClock, &oClkData.LastKnownGoodSecureClock, SIZEOF(DRM_UINT64));
    oClkData.flag |= CLK_LK_RSC_VALID;

    oClkData.fInGracePeriod = FALSE;
    oClkData.flag |= CLK_IN_GP_VALID;

    ZEROMEM(&oClkData.LastKnownGracePeriodStartTime, SIZEOF( oClkData.LastKnownGracePeriodStartTime ) );
    oClkData.flag |= CLK_LK_GP_VALID;

    ChkDR( DRM_XML_GetNode(
            &dstrData,
            &g_dstrTagRefreshDate,
            NULL,
            NULL,
            0,
            NULL,
            &dstrValue ) );

    if( DRM_FAILED( DRM_UTL_GetZuluTimeFromStringAsUI64( &dstrValue, &oClkData.RefreshDate ) ) )
    {
        ChkDR( DRM_E_CLK_INVALID_RESPONSE );
    }

    oClkData.flag |= CLK_REFRESHDATE_VALID;
    oClkData.flag |= CLK_EVER_SET;


    /*Write it to secure store*/
    cbSecureData = SIZEOF( oClkData );

    _PrepareSecureClockDataForWriting( &oClkData );
    ChkDR( DRM_SST_SetData(
            poSecureStoreContext,
            &g_idSData,
            &g_idSDataPrev,
            rgbSecStoreInPswd,
            SECURE_STORE_GLOBAL_DATA,
            poDatastore,
            (DRM_BYTE *) &oClkData,
            cbSecureData ) );

    _PrepareSecureClockDataForReading( &oClkData );

    ChkDR( Oem_Clock_SetResetState( pOEMContext, FALSE ) );
    UI64_TO_FILETIME( oClkData.LastKnownGoodSecureClock, filetime );
    if (! Oem_Clock_FileTimeToSystemTime( &filetime, &oSystemTime ) )
    {
        ChkDR( DRM_E_CLK_INVALID_RESPONSE );
    }
#if 0
	printf("Func = %s, SetSystemTime To [%04d-%02d-%02d %02d:%02d:%02d.%02d]\n", __FUNCTION__, oSystemTime.wYear, oSystemTime.wMonth, oSystemTime.wDay,
		oSystemTime.wHour,oSystemTime.wMinute,oSystemTime.wSecond, oSystemTime.wMilliseconds);
#endif
    Oem_Clock_SetSystemTime( pOEMContext, &oSystemTime );

    ChkDR( DRM_ARCLK_LIC_ResetClockRollback( poLicEvalContext ) );

ErrorExit:

    if( DRM_FAILED( dr ) )
    {
        dr = DRM_E_CLK_INVALID_RESPONSE;
    }

    DRM_PROFILING_LEAVE_SCOPE;

    return( dr );
}

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

EXIT_PK_NAMESPACE_CODE;
