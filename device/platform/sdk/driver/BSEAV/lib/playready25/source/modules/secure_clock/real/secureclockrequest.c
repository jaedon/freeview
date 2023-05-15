/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#define DRM_BUILDING_SECURECLOCKREQUEST_C
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmxmlbuilder.h>
#include <drmxmlparser.h>
#include <drmlicense.h>
#include <oemsha1.h>
#include <drmdevcert.h>
#include <drmsecureclock.h>
#include <drmsecureclockstatus.h>
#include <drmantirollbackclock.h>
#include <drmsecureclockconstants.h>
#include <drmconstants.h>
#include <drmprofile.h>
#include <drmlastinclude.h>

ENTER_PK_NAMESPACE_CODE;

PREFAST_PUSH_DISABLE_EXPLAINED(__WARNING_USE_WIDE_API, "Ignore prefast warning about ANSI functions for this entire file.");

#define CLK_CHALLENGE_EXTRA_SIZE 255
 /*Date has to be in ZULU format     #yyyymmdd hh:mm:ssZ# */
#define CLK_ZULU_POUND_SIZE 1
#define CLK_ZULU_YEAR_SIZE 4
#define CLK_ZULU_MONTH_SIZE 2
#define CLK_ZULU_DAY_SIZE 2
#define CLK_ZULU_SPACE_SIZE 1
#define CLK_ZULU_HOUR_SIZE 2
#define CLK_ZULU_FIRST_COLON_SIZE 1
#define CLK_ZULU_MINUTE_SIZE 2
#define CLK_ZULU_SECOND_COLON_SIZE 1
#define CLK_ZULU_SECOND_SIZE 2
#define CLK_ZULU_Z_SIZE 1
#define CLK_ZULU_POUND_SIZE 1
#define CLK_ZULU_NULL_SIZE 1

#define CLK_ZULU_TIME_SIZE  CLK_ZULU_POUND_SIZE + \
                            CLK_ZULU_YEAR_SIZE + \
                            CLK_ZULU_MONTH_SIZE + \
                            CLK_ZULU_DAY_SIZE + \
                            CLK_ZULU_SPACE_SIZE + \
                            CLK_ZULU_HOUR_SIZE + \
                            CLK_ZULU_FIRST_COLON_SIZE + \
                            CLK_ZULU_MINUTE_SIZE + \
                            CLK_ZULU_SECOND_COLON_SIZE + \
                            CLK_ZULU_SECOND_SIZE + \
                            CLK_ZULU_Z_SIZE + \
                            CLK_ZULU_POUND_SIZE + \
                            CLK_ZULU_NULL_SIZE

#define CLK_WITH_STATUS_XML_SIZE 250

DRM_EXPORT_VAR const DRM_ID g_idSData     = { 'S', 0, 'E', 0, 'C', 0, 'U', 0, 'R', 0, 'E', 0, '1', 0, '\0', 0 };
DRM_EXPORT_VAR const DRM_ID g_idSDataPrev = { 'S', 0, 'E', 0, 'C', 0, 'U', 0, 'R', 0, 'E', 0, '2', 0, '\0', 0 };



DRM_API DRM_VOID DRM_CALL _PrepareSecureClockDataForWriting( DRM_SECSTORE_CLOCK_DATA *pData )
{
    FIX_ENDIAN_DWORD( pData->flag  );
    FIX_ENDIAN_DWORD( pData->fInGracePeriod );
    FIX_ENDIAN_QWORD( pData->LastKnownGoodSecureClock );
    FIX_ENDIAN_QWORD( pData->LastKnownRealtimeSecureClock );
    FIX_ENDIAN_QWORD( pData->LastKnownGracePeriodStartTime );
    FIX_ENDIAN_QWORD( pData->RefreshDate );
    FIX_ENDIAN_QWORD( pData->ChallengeTime );
}

/*******************************************************************
 *              PRIVATE FUNCTION _CreateNewTID
 *
 * purpose: create a new TID-sized blob of random bytes
 *******************************************************************/

static DRM_RESULT _CreateNewTID (DRM_VOID *pOEMContext, DRM_TID *ptid)
{
    return Oem_Random_GetBytes (pOEMContext, (DRM_BYTE *) ptid, SIZEOF (DRM_TID));
}

/******************************************************************************/
DRM_API DRM_RESULT DRM_CALL DRM_CLK_CreateChallenge(
    __in                       const DRM_CONST_STRING      *pdstrURL,
    __in                       const DRM_BYTE               rgbSecStoreInPswd [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                             DRM_CRYPTO_CONTEXT    *pContext,
    __in                             DRM_DST               *poDatastore,
    __in                             DRM_SECSTORE_CONTEXT  *poSecureStoreContext,
    __in                             DRM_VOID              *pOEMContext,
    __out_ecount_opt(*pcchUrl)       DRM_WCHAR             *pwszUrl,
    __inout_opt                      DRM_DWORD             *pcchUrl,
    __out_ecount_opt(*pbData)        DRM_BYTE              *pbData,
    __inout                          DRM_DWORD             *pcbData )
{
    DRM_RESULT               dr            = DRM_SUCCESS;
    DRM_DWORD                dwSize        = 0;
    _XMBContext             *pXmlData      = NULL;
    DRM_DWORD                cbXmlData     = 0;
    DRM_DWORD                cbEncodedSize = 0;
    DRM_DWORD                cbSecureData  = 0;
    DRM_SECSTORE_CLOCK_DATA  oClkData;
    DRM_STRING               dstrClose     = EMPTY_DRM_STRING;
    DRM_CONST_STRING         dstrTid       = EMPTY_DRM_STRING;
    DRM_DWORD                dwLen         = 0;
    DRMSYSTEMTIME            oSystemTime;
    DRMFILETIME              filetime;

    DRM_PROFILING_ENTER_SCOPE(PERF_MOD_SECURECLOCKREQUEST, PERF_FUNC_DRM_CLK_CreateChallenge);

    ChkArg( pcbData             != NULL
         && pContext            != NULL
         && rgbSecStoreInPswd   != NULL
         && poDatastore        != NULL
         && poSecureStoreContext!= NULL );
    ChkDRMString( pdstrURL );

    dwLen = pdstrURL->cchString;

    /* Calculate safe size  including base64 ANSI encoding */

    dwSize = ( dwLen*SIZEOF(DRM_WCHAR) + CLK_CHALLENGE_EXTRA_SIZE );

    /* For final ANSI encoding */
    dwSize = CCH_BASE64_EQUIV( dwSize ) ;

    ChkOverflow( dwLen + 1, dwLen );

    if ( pbData  == NULL
      || *pcbData < dwSize
      || ( pcchUrl != NULL
        && ( pwszUrl == NULL
          || *pcchUrl < dwLen+1 )
          )
       )

    {
        /* safe size */
        *pcbData = dwSize;
        if ( pcchUrl )
        {
            *pcchUrl = dwLen+1;
        }
        dr = DRM_E_BUFFERTOOSMALL;
        goto ErrorExit;
    }


    /* Initialize buffer */
    ZEROMEM(pbData, *pcbData );

    pXmlData  =  (_XMBContext *)pbData;
    cbXmlData = *pcbData;


    /*Check the byte alignment of pbXmlData */
    ChkDR( DRM_UTL_EnsureDataAlignment( (DRM_BYTE *)pXmlData, cbXmlData, (DRM_BYTE **)&pXmlData, &cbXmlData, SIZEOF(DRM_DWORD_PTR), NULL ) );

    /* Init XML Builder
    ** Add document root tag: <DRMCLOCK type="challenge">
    */
    ChkDR( DRM_XMB_CreateDocument( cbXmlData, pXmlData,            &g_dstrTagDRMReqRoot ) );
    ChkDR( DRM_XMB_AddAttribute(   pXmlData, &g_dstrAttributeType, &g_dstrChallenge ) );

    /* Add <DATA> node to the root node */
    ChkDR( DRM_XMB_OpenNode( pXmlData, &g_dstrTagData ) );

    if ( pcchUrl != NULL && pwszUrl != NULL )
    {
        ChkDR( DRM_STR_StringCchCopyNW( pwszUrl, *pcchUrl, pdstrURL->pwszString, pdstrURL->cchString ) );
        *pcchUrl = pdstrURL->cchString+1;
    }

    /* Add <URL> node to data node */
    ChkDR( DRM_XMB_OpenNode( pXmlData, &g_dstrTagURL ) );
    ChkDR( DRM_XMB_AddData( pXmlData, pdstrURL ) );
    ChkDR( DRM_XMB_CloseCurrNode( pXmlData, NULL ) );
    /* Read secure store */
    ZEROMEM(&oClkData, SIZEOF( oClkData ));
    cbSecureData = SIZEOF( oClkData );
    dr = DRM_SST_GetData(   poSecureStoreContext,
                           &g_idSData,
                           &g_idSDataPrev,
                            rgbSecStoreInPswd,
                            SECURE_STORE_GLOBAL_DATA,
                            poDatastore,
              (DRM_BYTE *) &oClkData,
                           &cbSecureData);
    _PrepareSecureClockDataForReading( &oClkData );
    /*Read can Fail. But it doesnt matter */
    if ( DRM_SUCCESS != dr )
    {
        ZEROMEM(&oClkData, SIZEOF(oClkData));
    }

    /* Now create a new TID and write it to the secure store */
    ChkDR( _CreateNewTID ( pOEMContext, &oClkData.tid ) );
    oClkData.flag |= CLK_TID_VALID;

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
    FILETIME_TO_UI64( filetime, oClkData.ChallengeTime );

    oClkData.flag |= CLK_CHALLENGETIME_VALID;

    /*Write it to secure store*/
    cbSecureData = SIZEOF( oClkData );

    _PrepareSecureClockDataForWriting( &oClkData );
    ChkDR( DRM_SST_SetData( poSecureStoreContext,
                           &g_idSData,
                           &g_idSDataPrev,
                            rgbSecStoreInPswd,
                            SECURE_STORE_GLOBAL_DATA,
                            poDatastore,
              (DRM_BYTE *) &oClkData,
                            cbSecureData ) );
    _PrepareSecureClockDataForReading( &oClkData );

    /*Encode it*/
    /* We are using Cryto buffer as working buffer.
    Clear this buffer before using any crypto API*/
    DSTR_FROM_PB( &dstrTid, pContext->rgbCryptoContext, DRM_PKCRYPTO_CONTEXT_BUFFER_SIZE );

    ChkDR( DRM_B64_EncodeW((const DRM_BYTE * )&oClkData.tid,
                            SIZEOF(DRM_TID),
                            (DRM_WCHAR *) dstrTid.pwszString,
                            &dstrTid.cchString,
                             0));

    /* Add TID */
    ChkDR( DRM_XMB_OpenNode( pXmlData, &g_dstrTagTID ) );
    ChkDR( DRM_XMB_AddData( pXmlData, &dstrTid ) );
    ChkDR(DRM_XMB_CloseCurrNode( pXmlData, NULL ) );

    /* Close data node */
    ChkDR(DRM_XMB_CloseCurrNode( pXmlData, &dstrClose ) );

    /* Close Document */
    ChkDR( DRM_XMB_CloseDocument( pXmlData, &dstrClose ) );


    /* Now Base64Encode it in ANSI. Use Crypto context as working buffer */
    cbEncodedSize = CCH_BASE64_EQUIV( CB_DSTR( &dstrClose ) );
    if ( *pcbData < cbEncodedSize )
    {
        *pcbData = cbEncodedSize;
        dr = DRM_E_BUFFERTOOSMALL;
        goto ErrorExit;
    }

    /*Move this data to front as we want to encode in place*/
    MEMMOVE(pbData, PB_DSTR( &dstrClose ), CB_DSTR( &dstrClose ) );

    cbEncodedSize = *pcbData;

    ChkDR( DRM_B64_EncodeA(pbData,
                           CB_DSTR(&dstrClose),
               (DRM_CHAR *)pbData,
                          &cbEncodedSize,
                           0));
    *pcbData = cbEncodedSize;

ErrorExit:

    DRM_PROFILING_LEAVE_SCOPE;

    return dr;
}

/******************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_CLK_GetSecureClock(
    __out_ecount_opt(*pcchSecureTime)                DRM_WCHAR            *pwszSecureTime,
    __inout_opt                                      DRM_DWORD            *pcchSecureTime,
    __out_ecount_opt(1)                              DRM_DWORD            *pdwFlags,
    __out_ecount_opt(*pcbSecureTimeWithStatus)       DRM_BYTE             *pbSecureTimeWithStatus,
    __inout_opt                                      DRM_DWORD            *pcbSecureTimeWithStatus,
    __in                                       const DRM_BYTE              rgbSecStoreInPswd [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in                                             DRM_CRYPTO_CONTEXT   *pContext,
    __in                                             DRM_DST              *poDatastore,
    __in                                             DRM_SECSTORE_CONTEXT *poSecureStoreContext,
    __in                                             DRM_VOID             *pOEMContext )
{
    DRM_RESULT               dr           = DRM_SUCCESS;
    DRM_SECSTORE_CLOCK_DATA  oClkData;
    DRMSYSTEMTIME            oSystemTime;
    DRMFILETIME              filetime;
    DRM_UINT64               u64DRMTime;
    DRM_DWORD                cbSecureData = 0;
    DRM_BOOL                 fResetState  = FALSE;
    DRM_WCHAR               *pwszTemp     = NULL;
    _XMBContext             *pXmlData     = NULL;
    DRM_DWORD                cbXmlData    = 0;
    DRM_DWORD                dwFlags      = 0;
    const DRM_CONST_STRING  *pdstrStatus;
    DRM_CONST_STRING         dstrZuluTime = EMPTY_DRM_STRING;

    ChkArg( pContext             != NULL
         && rgbSecStoreInPswd    != NULL
         && poDatastore         != NULL
         && poSecureStoreContext != NULL );

    /*Date has to be in ZULU format     #yyyymmdd hh:mm:ssZ# */

    if ( ( NULL != pcchSecureTime
        && (  NULL == pwszSecureTime
          || *pcchSecureTime < CLK_ZULU_TIME_SIZE ) )
      || ( NULL != pcbSecureTimeWithStatus )
        &&  ( NULL == pbSecureTimeWithStatus
           || *pcbSecureTimeWithStatus < CLK_WITH_STATUS_XML_SIZE ) )
    {
        if ( NULL != pcchSecureTime )
        {
            *pcchSecureTime = CLK_ZULU_TIME_SIZE;
        }
        if ( NULL != pcbSecureTimeWithStatus )
        {
            *pcbSecureTimeWithStatus = CLK_WITH_STATUS_XML_SIZE;
        }
        ChkDR( DRM_E_BUFFERTOOSMALL );
    }

    /* Read secure store */
    ZEROMEM(&oClkData, SIZEOF( oClkData ));
    cbSecureData = SIZEOF( oClkData );

    ChkDR( DRM_SST_GetData( poSecureStoreContext,
                            &g_idSData,
                            &g_idSDataPrev,
                            rgbSecStoreInPswd,
                            SECURE_STORE_GLOBAL_DATA,
                            poDatastore,
                            (DRM_BYTE *) &oClkData,
                            &cbSecureData ) );

    _PrepareSecureClockDataForReading( &oClkData );
    Oem_Clock_GetSystemTime( pOEMContext, &oSystemTime );
#if 0
	printf("#####Func = %s, GetSystemTime To [%04d-%02d-%02d %02d:%02d:%02d.%02d]\n", __FUNCTION__, oSystemTime.wYear, oSystemTime.wMonth, oSystemTime.wDay,
		oSystemTime.wHour,oSystemTime.wMinute,oSystemTime.wSecond, oSystemTime.wMilliseconds);
#endif
    /*Use DRM Crypto context as buffer*/
    pwszTemp = ( DRM_WCHAR * )pContext->rgbCryptoContext;

    /* Convert system time in ZULU format */
    *pwszTemp = g_wchPound;
    pwszTemp +=CLK_ZULU_POUND_SIZE;

    ChkDR( DRM_UTL_NumberToString ( ( DRM_DWORD ) oSystemTime.wYear,
                                    pwszTemp,
                                    ( DRM_DWORD ) CLK_ZULU_YEAR_SIZE+1,
                                    ( DRM_DWORD ) CLK_ZULU_YEAR_SIZE,
                                    10,
                                    NULL ) );
    pwszTemp +=CLK_ZULU_YEAR_SIZE;

    ChkDR( DRM_UTL_NumberToString ( ( DRM_DWORD ) oSystemTime.wMonth,
                                    pwszTemp,
                                    ( DRM_DWORD ) CLK_ZULU_MONTH_SIZE+1,
                                    ( DRM_DWORD ) CLK_ZULU_MONTH_SIZE,
                                    10,
                                    NULL ) );
    pwszTemp +=CLK_ZULU_MONTH_SIZE;

    ChkDR( DRM_UTL_NumberToString ( ( DRM_DWORD ) oSystemTime.wDay,
                                    pwszTemp,
                                    ( DRM_DWORD ) CLK_ZULU_DAY_SIZE+1,
                                    ( DRM_DWORD ) CLK_ZULU_DAY_SIZE,
                                    10,
                                    NULL ) );
    pwszTemp +=CLK_ZULU_DAY_SIZE;

    *pwszTemp = g_wchSpace;
    pwszTemp +=CLK_ZULU_SPACE_SIZE;

    ChkDR( DRM_UTL_NumberToString ( ( DRM_DWORD ) oSystemTime.wHour,
                                    pwszTemp,
                                    ( DRM_DWORD ) CLK_ZULU_HOUR_SIZE+1,
                                    ( DRM_DWORD ) CLK_ZULU_HOUR_SIZE,
                                    10,
                                    NULL ) );
    pwszTemp +=CLK_ZULU_HOUR_SIZE;

    *pwszTemp = g_wchColon;
    pwszTemp +=CLK_ZULU_FIRST_COLON_SIZE;

    ChkDR( DRM_UTL_NumberToString ( ( DRM_DWORD ) oSystemTime.wMinute,
                                    pwszTemp,
                                    ( DRM_DWORD ) CLK_ZULU_MINUTE_SIZE+1,
                                    ( DRM_DWORD ) CLK_ZULU_MINUTE_SIZE,
                                    10,
                                    NULL ) );
    pwszTemp +=CLK_ZULU_MINUTE_SIZE;

    *pwszTemp = g_wchColon;
    pwszTemp +=CLK_ZULU_SECOND_COLON_SIZE;

    ChkDR( DRM_UTL_NumberToString ( ( DRM_DWORD ) oSystemTime.wSecond,
                                    pwszTemp,
                                    ( DRM_DWORD ) CLK_ZULU_SECOND_SIZE+1,
                                    ( DRM_DWORD ) CLK_ZULU_SECOND_SIZE,
                                    10,
                                    NULL ) );
    pwszTemp +=CLK_ZULU_SECOND_SIZE;

    *pwszTemp = g_wchZ;
    pwszTemp +=CLK_ZULU_Z_SIZE;
    *pwszTemp = g_wchPound;
    pwszTemp +=CLK_ZULU_POUND_SIZE;
    *pwszTemp = g_wchNull;

    if ( !Oem_Clock_SystemTimeToFileTime( &oSystemTime, &filetime ) )
    {
        ChkDR( DRM_E_CLK_INVALID_DATE );
    }

    FILETIME_TO_UI64( filetime, u64DRMTime );

    ChkDR( Oem_Clock_GetResetState( pOEMContext, &fResetState ) );

    /*If Clock was never set or device is in grace period = NOT_SET*/

    if ( fResetState
      || ( ( oClkData.flag & CLK_LK_RSC_VALID ) == 0 )
      || ( ( oClkData.flag & CLK_EVER_SET )     == 0 )
      || ( ( oClkData.flag & CLK_IN_GP_VALID ) && oClkData.fInGracePeriod ) )
    {
        dwFlags = DRM_CLK_NOT_SET;
        pdstrStatus = &g_dstrSecureClockNotSet;
    }
    else if ( ( oClkData.flag & CLK_REFRESHDATE_VALID )
         && ( DRM_UI64Les( u64DRMTime, oClkData.RefreshDate ) ) )
    {
        dwFlags = DRM_CLK_SET;
        pdstrStatus = &g_dstrSecureClockSet;
    }
    else
    {
        dwFlags = DRM_CLK_NEEDS_REFRESH;
        pdstrStatus = &g_dstrSecureClockNeedsRefresh;
    }



    if ( NULL != pcbSecureTimeWithStatus
      && NULL != pbSecureTimeWithStatus
      && *pcbSecureTimeWithStatus >= CLK_WITH_STATUS_XML_SIZE )
    {
       /* < DRMCLOCK type = status >
            <VALUE> Secure tine value in ZULU format</VALUE>
            <FLAG> 0 1 or 2 depending on status</FLAG>
          </ DRMCLOCK > */

        ChkDR(DRM_UTL_EnsureDataAlignment(pbSecureTimeWithStatus,
                                         *pcbSecureTimeWithStatus,
                            (DRM_BYTE **)&pXmlData,
                                         &cbXmlData,
                                          SIZEOF(DRM_DWORD),
                                          NULL));

        ChkDR( DRM_XMB_CreateDocument( cbXmlData, pXmlData, &g_dstrTagDRMReqRoot ) );
        ChkDR( DRM_XMB_AddAttribute( pXmlData, &g_dstrAttributeType, &g_dstrStatus ) );
        /*Open Value Node*/
        ChkDR( DRM_XMB_OpenNode( pXmlData, &g_dstrTagValue ) );
        dstrZuluTime.pwszString = ( DRM_WCHAR * )pContext->rgbCryptoContext;
        /*Dont copy null termination*/
        dstrZuluTime.cchString = CLK_ZULU_TIME_SIZE -1;
        ChkDR( DRM_XMB_AddData( pXmlData, &dstrZuluTime ) );
        ChkDR(DRM_XMB_CloseCurrNode( pXmlData, NULL ) );

        ChkDR( DRM_XMB_OpenNode( pXmlData, &g_dstrFlag ) );

        ChkDR( DRM_XMB_AddData( pXmlData, pdstrStatus ) );
        ChkDR(DRM_XMB_CloseCurrNode( pXmlData, NULL ) );
        ChkDR( DRM_XMB_CloseDocument( pXmlData, (DRM_STRING *)&dstrZuluTime ) );
        MEMMOVE( pbSecureTimeWithStatus, PB_DSTR(&dstrZuluTime), CB_DSTR(&dstrZuluTime) );
        *pcbSecureTimeWithStatus = CB_DSTR(&dstrZuluTime);
    }

    if ( NULL != pcchSecureTime
      && NULL != pwszSecureTime
      && *pcchSecureTime >= CLK_ZULU_TIME_SIZE )
    {
        *pcchSecureTime = CLK_ZULU_TIME_SIZE;
        MEMCPY( (DRM_BYTE *)pwszSecureTime, pContext->rgbCryptoContext, (*pcchSecureTime)*SIZEOF( DRM_WCHAR ) );
    }
    if (NULL != pdwFlags)
    {
        *pdwFlags = dwFlags;
    }

ErrorExit:
    return dr;
}

/******************************************************************************/

DRM_API DRM_RESULT DRM_CALL DRM_CLK_InitCheckSecureClock(
    __in    const DRM_BYTE              rgbSecStoreInPswd [__CB_DECL(DRM_SHA1_DIGEST_LEN)],
    __in    const DRM_CRYPTO_CONTEXT   *pContext,
    __in          DRM_DST              *poDatastore,
    __inout       DRM_BOOL             *pfClockSet,
    __in          DRM_SECSTORE_CONTEXT *poSecureStoreContext,
    __in          DRM_LICEVAL_CONTEXT  *poLicEvalContext,
    __in          DRM_VOID             *pOEMContext )
{
    DRM_RESULT      dr = DRM_SUCCESS;
    DRM_DWORD       cbSecureData = 0;
    DRM_BOOL        fResetState = FALSE;
    DRMSYSTEMTIME   oSystemTime;
    DRM_SECSTORE_CLOCK_DATA oClkData;

    ChkArg( pContext            != NULL
        && rgbSecStoreInPswd    != NULL
        && poDatastore         != NULL
        && poSecureStoreContext != NULL
        && poLicEvalContext     != NULL
        && pfClockSet           != NULL );

    ZEROMEM(&oClkData, SIZEOF( oClkData ) );

    /*
     * There can be 3 situations.
     * 1. Data store CLK_EVER_SET is not set and Clock is reset = New Device
     * 2. Data store CLK_EVER_SET is set and Clock is reset = Go in Grace period.
     * 3. Clock is set and data store don't care = Normal operation.
     */

    /*Get reset state*/
    ChkDR( Oem_Clock_GetResetState( pOEMContext, &fResetState ) );

    /* Read secure store */
    cbSecureData = SIZEOF(oClkData);
    dr = DRM_SST_GetData( poSecureStoreContext,
                          &g_idSData,
                          &g_idSDataPrev,
                          rgbSecStoreInPswd,
                          SECURE_STORE_GLOBAL_DATA,
                          poDatastore,
                          (DRM_BYTE *) &oClkData,
                          &cbSecureData );
    _PrepareSecureClockDataForReading( &oClkData );
    if ( fResetState )
    {
        /*State 1 or 2*/
        if ( DRM_SUCCEEDED(dr)
          && (oClkData.flag & CLK_EVER_SET) )
        {
            /*Device is in (or going to be in) Grace Period*/

            ChkDR( Oem_Clock_SetResetState( pOEMContext, FALSE ) );
            if ( oClkData.flag & CLK_LK_RSC_VALID )
            {
                DRMFILETIME filetime;
                UI64_TO_FILETIME( oClkData.LastKnownRealtimeSecureClock, filetime );
                if ( !Oem_Clock_FileTimeToSystemTime( &filetime, &oSystemTime ) )
                {
                    ChkDR( DRM_E_CLK_INVALID_DATE );
                }
#if 0
				printf("Func = %s, SetSystemTime To [%04d-%02d-%02d %02d:%02d:%02d.%02d]\n", __FUNCTION__, oSystemTime.wYear, oSystemTime.wMonth, oSystemTime.wDay,
					oSystemTime.wHour,oSystemTime.wMinute,oSystemTime.wSecond, oSystemTime.wMilliseconds);
#endif
                Oem_Clock_SetSystemTime( poLicEvalContext->pcontextBBX->pOEMContext, &oSystemTime );

                ChkDR( DRM_ARCLK_LIC_ResetClockRollback( poLicEvalContext ) );
            }
            if ( ( ( oClkData.flag & CLK_IN_GP_VALID ) == 0 )
              || ( !oClkData.fInGracePeriod ) )
            {
                oClkData.fInGracePeriod = TRUE;
                oClkData.flag |= CLK_IN_GP_VALID;
                MEMCPY( &oClkData.LastKnownGracePeriodStartTime, &oClkData.LastKnownRealtimeSecureClock, SIZEOF( oClkData.LastKnownGracePeriodStartTime ) );
                oClkData.flag |= CLK_LK_GP_VALID;
            }
        }
    }
    else
    {
        /*Normal Operation*/
        /*Get System Time and Put it in LastKnownRealtimeSecureClock */
        DRMFILETIME filetime;
        Oem_Clock_GetSystemTimeAsFileTime ( poLicEvalContext->pcontextBBX->pOEMContext, &filetime );
        FILETIME_TO_UI64( filetime, oClkData.LastKnownRealtimeSecureClock );
        oClkData.flag |= CLK_LK_RSC_VALID;
        oClkData.flag |= CLK_EVER_SET;
    }


    /*Write to secure store */
    cbSecureData = SIZEOF( oClkData );
    _PrepareSecureClockDataForWriting( &oClkData );
    ChkDR( DRM_SST_SetData( poSecureStoreContext,
                            &g_idSData,
                            &g_idSDataPrev,
                            rgbSecStoreInPswd,
                            SECURE_STORE_GLOBAL_DATA,
                            poDatastore,
                            (DRM_BYTE *) &oClkData,
                            cbSecureData ) );
    _PrepareSecureClockDataForReading( &oClkData );

    *pfClockSet = ( oClkData.flag & CLK_EVER_SET ) ? TRUE : FALSE ;

ErrorExit:
    return dr;
}

PREFAST_POP /* ignore prefast warning about ANSI functions for this entire file  */

EXIT_PK_NAMESPACE_CODE;
