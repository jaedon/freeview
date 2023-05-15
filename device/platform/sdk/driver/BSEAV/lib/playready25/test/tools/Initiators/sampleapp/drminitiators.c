/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h>

#include <drmwindowsenv.h>
#include <drmcommon.h>
#include <drmxmlparser.h>
#include <drmtypes.h>
#include <drmutilities.h>
#include <drmmanager.h>
#include <drmcmdlnpars.h>
#include <drmtoolsutils.h>
#include <drmtoolsmediafile.h>
#include <drmheaderparser.h>
#include <drmutf.h>
#include <drminitparser.h>
#include <tstutils.h>
#include <toemimp_pd.h>
#include <clientlogs.h>
#include "tNetHeaders.h"

#if DRM_TEST_SUPPORT_NET_IO
extern const DRM_CONST_STRING g_dstrPRInitiatorTag;

/* Name of the HDS file used by this application */
const DRM_WCHAR  g_rgwchHdsStoreFile          [] = { ONE_WCHAR('c', '\0'), ONE_WCHAR(':', '\0'), ONE_WCHAR('\\', '\0'), ONE_WCHAR('w', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('\\', '\0'), ONE_WCHAR('S', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('m', '\0'), ONE_WCHAR('p', '\0'), ONE_WCHAR('l', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('.', '\0'), ONE_WCHAR('h', '\0'), ONE_WCHAR('d', '\0'), ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0') }; /* "c:\\wmdrmpd\\Sample.hds" */

/* Path to store downloaded content  */
const DRM_WCHAR  g_rgwchDownloadPath          [] = { ONE_WCHAR('c', '\0'), ONE_WCHAR(':', '\0'), ONE_WCHAR('\\', '\0'), ONE_WCHAR('I', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('i', '\0'), ONE_WCHAR('a', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('r', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('o', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('e', '\0'), ONE_WCHAR('n', '\0'), ONE_WCHAR('t', '\0'), ONE_WCHAR('\\', '\0'), ONE_WCHAR('\0', '\0') }; /* "c:\\InitiatorContent\\" */
const DRM_CONST_STRING  g_dstrDownloadPath               = CREATE_DRM_STRING(g_rgwchDownloadPath);

/* Special characters that needs to be HTML encoded. This array is used by _HTMLEncode() function. */
const DRM_CHAR g_rgchHTMLEncode[] = { ' ', '+' };

/* Some custom data defines, depend on the specific application needs */
#define MAX_CUSTOM_DATA_SIZE        1024
#define MAX_NUM_CUSTOM_DATA         20
#define FRIENDLY_NAME               "My Phone"
#define CUSTOM_DATA_MSG             "HelloWorld"


/* ".cms" */
const DRM_WCHAR g_rgwchFileExtensionCMS [] =
{
    ONE_WCHAR( '.', '\0' ), ONE_WCHAR( 'c', '\0' ), ONE_WCHAR( 'm', '\0' ), ONE_WCHAR( 's', '\0' ),
    ONE_WCHAR( '\0', '\0' )
};
const DRM_CONST_STRING g_dstrFileExtensionCMS = CREATE_DRM_STRING( g_rgwchFileExtensionCMS );

/*******************************************************************************
**
** Function:    _ConvertFileNameFromWmdrmToPlayReady
**
** Synopsis:    Function that converts a WMDRM file name into a PlayReady file name.
**              based on the extension of the WMDRM file name.
**              ".WMA" is converted to ".PYA".
**              ".WMV" is converted to ".PYV".
**              Everything else is converted to ".PMF".
**
** Arguments:
**
** [f_pdstrWmdrmFileName]     -- Pointer to the WMDRM file name to be converted.
** [f_pdstrPlayReadyFileName] -- Pointer to a variable to receive the converted PlayReady
**                             file name. Caller is responsible to free the memory allocated.
**
** Returns:
**                          DRM_SUCCESS  The function completed successfully.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
*******************************************************************************/
static DRM_RESULT _ConvertFileNameFromWmdrmToPlayReady(
    __in DRM_CONST_STRING *f_pdstrWmdrmFileName,
    __out DRM_CONST_STRING *f_pdstrPlayReadyFileName )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD i = 0;
    DRM_CONST_STRING dstrWMA = CREATE_DRM_STRING( L".wma" );
    DRM_CONST_STRING dstrWMV = CREATE_DRM_STRING( L".wmv" );
    DRM_CONST_STRING dstrPYA = CREATE_DRM_STRING( L".pya" );
    DRM_CONST_STRING dstrPYV = CREATE_DRM_STRING( L".pyv" );
    DRM_CONST_STRING dstrPMF = CREATE_DRM_STRING( L".pmf" );
    DRM_CONST_STRING dstrOldExt = EMPTY_DRM_STRING;
    DRM_CONST_STRING *pdstrNewExt = NULL;
    DRM_CONST_STRING dstrNewFileName = EMPTY_DRM_STRING;
    DRM_DWORD cchNewFileName = 0;
    DRM_DWORD cchNewFileExt = 0;
    DRM_DWORD cbAllocated = 0;

    ChkDRMString( f_pdstrWmdrmFileName );
    /* Make sure the first character of the WMDRM file name is not '.'. */
    ChkArg( f_pdstrWmdrmFileName->pwszString[ 0 ] != WCHAR_CAST( '.' ) );
    ChkArg( f_pdstrPlayReadyFileName != NULL );

    /* Try to find the first location of '.' starting from the end of the WMDRM file name. */
    for ( i = f_pdstrWmdrmFileName->cchString - 1; i > 0; i-- )
    {
        if ( f_pdstrWmdrmFileName->pwszString[ i ] == WCHAR_CAST( '.' ) )
        {
            break;
        }
    }

    if ( i == 0 )
    {
        /* No '.' is found, use ".pmf" as the new extension. */
        cchNewFileName = f_pdstrWmdrmFileName->cchString;

        pdstrNewExt = &dstrPMF;
    }
    else
    {
        cchNewFileName = i;

        dstrOldExt.pwszString = f_pdstrWmdrmFileName->pwszString + i;
        dstrOldExt.cchString = f_pdstrWmdrmFileName->cchString - i;

        if ( DRM_UTL_DSTRStringsEqual( &dstrOldExt, &dstrWMA ) )
        {
            pdstrNewExt = &dstrPYA;
        }
        else if ( DRM_UTL_DSTRStringsEqual( &dstrOldExt, &dstrWMV ) )
        {
            pdstrNewExt = &dstrPYV;
        }
        else
        {
            /* Convert any other extension to ".pmf". */
            pdstrNewExt = &dstrPMF;
        }
    }

    dstrNewFileName.pwszString = f_pdstrWmdrmFileName->pwszString;
    dstrNewFileName.cchString = cchNewFileName;

    cchNewFileExt = pdstrNewExt->cchString;

    /*
    ** Allocate one more character to store the trailing 0, which is used by
    ** the following DRM_STR_StringCchCatNW calls.
    */
    f_pdstrPlayReadyFileName->cchString = cchNewFileName + cchNewFileExt + 1;

    cbAllocated = f_pdstrPlayReadyFileName->cchString * SIZEOF( DRM_WCHAR );

    ChkOverflow( cbAllocated, f_pdstrPlayReadyFileName->cchString );

    ChkMem( f_pdstrPlayReadyFileName->pwszString = ( DRM_WCHAR * )Oem_MemAlloc( cbAllocated ) );

    ZEROMEM( PB_DSTR( f_pdstrPlayReadyFileName ), CB_DSTR( f_pdstrPlayReadyFileName ) );

    ChkDR( DRM_STR_StringCchCatNW( ( DRM_WCHAR * )f_pdstrPlayReadyFileName->pwszString,
                                   f_pdstrPlayReadyFileName->cchString,
                                   dstrNewFileName.pwszString,
                                   dstrNewFileName.cchString ) );

    ChkDR( DRM_STR_StringCchCatNW( ( DRM_WCHAR * )f_pdstrPlayReadyFileName->pwszString,
                                   f_pdstrPlayReadyFileName->cchString,
                                   pdstrNewExt->pwszString,
                                   pdstrNewExt->cchString ) );

    /* Deduct the count for the trailing 0. */
    ( f_pdstrPlayReadyFileName->cchString )--;

ErrorExit:

    return dr;
}

/*******************************************************************************
**
** Function:    _HTMLEncode
**
** Synopsis:    Function that does HTML encoding of special characters (defined in
**              g_rgchHTMLEncode above) contained in an input UTF8/ANSI string.
**              The special characters are encoded in the format of %XX, with XX be
**              the hex ASCII value of the character. For example, '+' will be encoded
**              as '%2B'.
**
** Arguments:
**
** [f_rgchIn]               -- Pointer to buffer that contains the string to be
**                             HTML encoded.
** [f_cchIn]                -- Size (number of UTF8/ANSI characters) of the input
**                             string to be HTML encoded.
** [f_rgchOut]              -- Pointer to a buffer that is used to received the result
**                             of the HTML encoded string. If it is NULL, the size of
**                             buffer needed is contained in *f_pcchOut.
** [f_pcchOut]              -- Pointer to a variable that contains the size (number of
**                             UTF8/ANSI characters) of the output buffer during input,
**                             and receives the size of the output buffer actually used
**                             during output.
**
** Returns:
**                          DRM_SUCCESS  The function completed successfully.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_BUFFERTOOSMALL if the output buffer is not big enough.
**
*******************************************************************************/
static DRM_RESULT _HTMLEncode(
    IN DRM_CHAR *f_rgchIn,
    IN DRM_DWORD f_cchIn,
    OUT DRM_CHAR *f_rgchOut,
    IN OUT DRM_DWORD *f_pcchOut )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR *pchCurr = f_rgchOut;
    DRM_CHAR *pchEnd = NULL, ch = 0;
    DRM_DWORD i = 0, j = 0;
    DRM_DWORD cch = 0;

    ChkArg( f_rgchIn != NULL );
    ChkArg( f_cchIn > 0 );
    ChkArg( f_pcchOut != NULL );
    ChkArg( f_rgchOut == NULL || *f_pcchOut > 0 );

    if ( f_rgchOut != NULL )
    {
        pchEnd = f_rgchOut + *f_pcchOut;
    }

    for ( i = 0; i < f_cchIn; i++ )
    {
        for ( j = 0; j < SIZEOF( g_rgchHTMLEncode ); j++ )
        {
            if ( f_rgchIn[ i ] == g_rgchHTMLEncode[ j ] )
                break;
        }

        if ( j < SIZEOF( g_rgchHTMLEncode ) )
        {
            /* A special character is found. */
            cch += 3;

            if ( f_rgchOut + cch <= pchEnd )
            {
                /*
                ** Convert the special character to %XX format,
                ** with XX to be its hex ASCII value.
                */
                *pchCurr++ = '%';

                ch = f_rgchIn[ i ] >> 4;

                if ( ch >= 10 )
                {
                    *pchCurr++ = 'A' + ( ch - 10 );
                }
                else
                {
                    *pchCurr++ = '0' + ch;
                }

                ch = f_rgchIn[ i ] % 16;

                if ( ch >= 10 )
                {
                    *pchCurr++ = 'A' + ( ch - 10 );
                }
                else
                {
                    *pchCurr++ = '0' + ch;
                }
            }
        }
        else
        {
            cch ++;

            if ( f_rgchOut + cch <= pchEnd )
            {
                *pchCurr++ = f_rgchIn[ i ];
            }
        }
    }

    *f_pcchOut = cch;

ErrorExit:

    if ( f_rgchOut + cch > pchEnd )
    {
        dr = DRM_E_BUFFERTOOSMALL;
    }

    return dr;
}

/*********************************************************************
**
** Function: _ProcessMeterDataResponse
**
** Synopsis: Function that processes a meter data response received
**           from the non-SOAP based metering server. After the successful
**           processing of the meter data response, the metering records
**           in the local HDS will be cleared.
**
**           The received meter data response is in UTF8/ANSI and needs
**           to be converted to UTF-16 before being consumed by
**           Drm_Metering_ProcessResponse().
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_pbResponse]           -- Pointer to a buffer that contains the meter
**                             data response from the metering server.
** [f_cbResponse]           -- Size of the meter data response received
**                             from the metering server.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _ProcessMeterDataResponse(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_BYTE *f_pbResponse,
    IN DRM_DWORD f_cbResponse)
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_BYTE  *pbResponse = NULL;
    DRM_DWORD  dwFlag     = 0;
    DRM_STRING dstrResponse  = EMPTY_DRM_STRING;

    ChkArg( f_poAppContext);
    ChkArg( f_pbResponse != NULL && f_cbResponse > 0 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    ChkMem( pbResponse =
                ( DRM_BYTE * )Oem_MemAlloc( f_cbResponse * SIZEOF( DRM_WCHAR ) ) );

    DSTR_FROM_PB( &dstrResponse, pbResponse, f_cbResponse * SIZEOF( DRM_WCHAR ) );

    /* Convert the server response to UTF-16. */
    ChkDR( DRM_STR_UTF8toDSTR( ( DRM_CHAR * )f_pbResponse, f_cbResponse, &dstrResponse ) );

    ChkDR( Drm_Metering_ProcessResponse( f_poAppContext,
                                         pbResponse,
                                         f_cbResponse * SIZEOF( DRM_WCHAR ),
                                         &dwFlag ) );

ErrorExit:
    SAFE_OEM_FREE( pbResponse );
    return dr;
}



/*********************************************************************
**
** Function: _GenerateMeterCertChallenge
**
** Synopsis: Function that generates a meter certificate challenge
**           to be sent to the SOAP based metering server.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poMID]                -- Pointer to a meter ID whose matching meter
**                             certificate is to be retrieved from the server.
** [f_ppbMeterCertChallenge]-- Pointer to pointer of a buffer that receives the
**                             generated meter certificate challenge.
** [f_pcbMeterCertChallenge]-- Pointer to a variable that receives size of the
**                             meter certificate challenge.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _GenerateMeterCertChallenge(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_MID *f_poMID,
    OUT DRM_BYTE **f_ppbMeterCertChallenge,
    IN OUT DRM_DWORD *f_pcbMeterCertChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poAppContext != NULL);
    ChkArg( f_poMID != NULL );
    ChkArg( f_ppbMeterCertChallenge != NULL );
    ChkArg( f_pcbMeterCertChallenge != NULL );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    dr = Drm_MeterCert_GenerateChallenge( f_poAppContext,
                                          f_poMID,
                                          NULL,
                                          0,
                                          NULL,
                                          f_pcbMeterCertChallenge );

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( *f_ppbMeterCertChallenge =
                    ( DRM_BYTE * )Oem_MemAlloc( *f_pcbMeterCertChallenge ) );

        ChkDR( Drm_MeterCert_GenerateChallenge( f_poAppContext,
                                                f_poMID,
                                                NULL,
                                                0,
                                                *f_ppbMeterCertChallenge,
                                                f_pcbMeterCertChallenge ) );
    }
    else
    {
        goto ErrorExit;
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _ProcessMeterCertResponse
**
** Synopsis: Function that processes a meter certificate response
**           received from the SOAP based metering server.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_pbResponse]           -- Pointer to a buffer that contains the meter
**                             certificate response from the metering server.
** [f_cbResponse]           -- Size of the meter certificate response
**                             from the metering server.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _ProcessMeterCertResponse(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_BYTE *f_pbResponse,
    IN DRM_DWORD f_cbResponse)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_RESULT dr1 = DRM_SUCCESS;

    ChkArg( f_poAppContext);
    ChkArg( f_pbResponse != NULL && f_cbResponse > 0 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    ChkDR( Drm_MeterCert_ProcessResponse( f_poAppContext,
                                          f_pbResponse,
                                          f_cbResponse,
                                          &dr1 ) );

    if ( DRM_SUCCEEDED( dr ) )
    {
        /*
        ** Get the detailed status code from the server
        ** response.
        */
        dr = dr1;
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _GenerateMeterDataChallenge
**
** Synopsis: Function that generates a meter data challenge
**           to be sent to the non-SOAP based metering server.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_pdstrMeterCert]       -- Pointer to a DRM string that contains a meter
**                             certificate.
** [f_pwchURL]              -- Pointer to a DRM_WCHAR buffer to receive the
**                             URL to post the meter data challenge.
** [f_pcchURL]              -- Pointer to a variable that contains the size (in
**                             wide characters) of the buffer used to receive
** [f_ppbMeterCertChallenge]-- Pointer to pointer of a buffer that receives the
**                             generated meter certificate challenge.
** [f_pcbMeterCertChallenge]-- Pointer to a variable that receives size of the
**                             meter certificate challenge.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _GenerateMeterDataChallenge(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_CONST_STRING *f_pdstrMeterCert,
    OUT DRM_CHAR *f_pchURL,
    IN OUT DRM_DWORD *f_pcchURL,
    OUT DRM_BYTE **f_ppbMeterDataChallenge,
    IN OUT DRM_DWORD *f_pcbMeterDataChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbTemp1 = NULL, *pbTemp2 = NULL;
    DRM_DWORD cbTemp1 = 0, cbTemp2 = 0, cbTemp3 = 0;

    ChkArg( f_poAppContext);
    ChkDRMString( f_pdstrMeterCert );
    ChkArg( f_pchURL != NULL );
    ChkArg( f_pcchURL != NULL );
    ChkArg( f_ppbMeterDataChallenge != NULL );
    ChkArg( f_pcbMeterDataChallenge != NULL );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    dr = Drm_Metering_GenerateChallenge( f_poAppContext,
                                         PB_DSTR(f_pdstrMeterCert),
                                         CB_DSTR(f_pdstrMeterCert),
                                         NULL,
                                         0,
                                         f_pchURL,
                                         f_pcchURL,
                                         NULL,
                                         &cbTemp1 );

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbTemp1 = ( DRM_BYTE * )Oem_MemAlloc( cbTemp1 ) );

        /* Generate the meter data challenge. The result is a UTF-16 string. */
        ChkDR( Drm_Metering_GenerateChallenge( f_poAppContext,
                                               PB_DSTR(f_pdstrMeterCert),
                                               CB_DSTR(f_pdstrMeterCert),
                                               NULL,
                                               0,
                                               f_pchURL,
                                               f_pcchURL,
                                               pbTemp1,
                                               &cbTemp1 ) );

        /* Convert cbTemp1 from byte counter to character counter */
        cbTemp1 /= SIZEOF( DRM_WCHAR );
        dr = DRM_STR_UTF16toUTF8( ( DRM_WCHAR * )pbTemp1, cbTemp1, NULL, 0, &cbTemp2 );
        ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, dr );

        ChkMem( pbTemp2 = ( DRM_BYTE * )Oem_MemAlloc( cbTemp2 ) );

        /* Convert the UTF-16 string into UTF8/ANSI before sending to the server. */
        ChkDR( DRM_STR_UTF16toUTF8( ( DRM_WCHAR * )pbTemp1, cbTemp1, ( DRM_CHAR * )pbTemp2, 0, &cbTemp2 ) );

        /*
        ** Calculate the size of the meter data challenge that will be HTML
        ** encoded by passing in a NULL buffer.
        */
        dr = _HTMLEncode( ( DRM_CHAR * )pbTemp2, cbTemp2, NULL, &cbTemp3 );

        if ( DRM_FAILED( dr ) )
        {
            if ( dr == DRM_E_BUFFERTOOSMALL )
            {
                ChkMem( *f_ppbMeterDataChallenge =
                            ( DRM_BYTE * )Oem_MemAlloc( cbTemp3 ) );

                /* HTML encode all special characters in the meter data challenge. */
                ChkDR( _HTMLEncode( ( DRM_CHAR * )pbTemp2,
                                   cbTemp2,
                                   ( DRM_CHAR * )*f_ppbMeterDataChallenge,
                                   &cbTemp3 ) );

                /* Update the total size of the challenge to consider the prefix string. */
                *f_pcbMeterDataChallenge = cbTemp3;
            }
            else
            {
                ChkDR( dr );
            }
        }
        else
        {
            ChkDR( DRM_E_FAIL );
        }
    }
    else
    {
        goto ErrorExit;
    }

ErrorExit:
    SAFE_OEM_FREE( pbTemp1 );
    SAFE_OEM_FREE( pbTemp2 );
    return dr;
}


/*********************************************************************
**
** Function: _DoMetering
**
** Synopsis: Based on information read from initiator message
**           function generates request to metering certificates server
**           and reports metering data.
**
** Arguments:
**
** [f_poAppContext]    -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poMeteringData]  -- Pointer to metering data read from initiator.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
static DRM_RESULT _DoMetering(
    __in DRM_APP_CONTEXT *f_poAppContext,
    __in DRM_INIT_METERING_DATA *f_poMeteringData
 )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_RESULT dr2 = DRM_SUCCESS;
    DRM_BYTE *pbChallenge = NULL;
    DRM_DWORD cbChallenge = 0;
    DRM_BYTE *pbResponse = NULL;
    DRM_DWORD cbResponse = 0;
    DRM_METERCERT_ENUM oMeterCertEnumContext = { 0 };
    DRM_MID oMID = { 0 };
    DRM_BYTE *pbMeterCert = NULL;
    DRM_DWORD cbMeterCert = 0;
    DRM_BOOL fIsV2 = FALSE;
    DRM_CHAR rgchURL[ MAX_URL_SIZE ] = { 0 };
    DRM_WCHAR rgwchURL[ MAX_URL_SIZE ] = { 0 };
    DRM_SUBSTRING dasstrURL = EMPTY_DRM_SUBSTRING;
    DRM_DWORD cchURL = MAX_URL_SIZE;
    DRM_STRING dstrMeteringServerURL = EMPTY_DRM_STRING;
    DRM_BOOL fCertFound = FALSE;

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_poMeteringData != NULL );

    /*
    ** Retrieve the MID from the meter cert store.
    */
    ChkDR( Drm_MeterCert_InitEnum( f_poAppContext, &oMeterCertEnumContext ) );

    DRM_BYT_CopyBytes(&oMID, 0, &f_poMeteringData->oMID, 0, SIZEOF(DRM_MID));

    while( DRM_E_NOMORE != dr && !fCertFound )
    {
        dr = Drm_MeterCert_EnumNext(&oMeterCertEnumContext,
                                    &oMID,
                                    NULL,
                                    0,
                                    &fIsV2,
                                    NULL,
                                    &cbMeterCert );

        if ( dr == DRM_E_BUFFERTOOSMALL )
        {
            ChkMem( pbMeterCert = ( DRM_BYTE * )Oem_MemAlloc( cbMeterCert ) );

            /*
            ** Retrieve the meter certificate, compare MIDs and keep looping if there is no match
            */
            ChkDR( Drm_MeterCert_EnumNext( &oMeterCertEnumContext,
                                           &oMID,
                                           NULL,
                                           0,
                                           &fIsV2,
                                           pbMeterCert,
                                           &cbMeterCert ) );

            if ( DRM_BYT_CompareBytes(&f_poMeteringData->oMID, 0, &oMID, 0, SIZEOF(DRM_MID) ) == 0 )
            {
                dr = DRM_SUCCESS;
                LogMessage( "\nMeter certificate found in store\n" );
                fCertFound = TRUE;
            }
        }

    }

    if ( DRM_FAILED( dr ) )
    {
        if ( f_poMeteringData->dstrMeteringCertServer.cchString == 0 )
        {
            LogMessage( "Cannot send challenge to the meter cert server because its URL is missing\n" );
            ChkDR( DRM_E_INVALIDARG );
        }

        /*
        ** receive meter certificate from the metering server
        */
        dr = Drm_MeterCert_GenerateChallenge( f_poAppContext, &oMID, NULL, 0, NULL, &cbChallenge );

        if ( dr == DRM_E_BUFFERTOOSMALL )
        {
            ChkMem( pbChallenge = ( DRM_BYTE * )Oem_MemAlloc( cbChallenge ) );
            dr = Drm_MeterCert_GenerateChallenge( f_poAppContext,
                                                  &oMID,
                                                  NULL,
                                                  0,
                                                  pbChallenge,
                                                  &cbChallenge );
        }
        ChkDR( dr );

        ChkDR( LogPacket( TRUE, pbChallenge, cbChallenge ) );

        /*
        ** Post the meter certificate challenge using the SOAP protocol.
        ** For WMDRM metering, we already have meter cert after parsing the message.
        */
        ChkDR( tOEM_SendNetDataDSTR( &f_poMeteringData->dstrMeteringCertServer,
                                     eDRM_TEST_NET_METERCERT,
                                     pbChallenge,
                                     cbChallenge,
                                    &pbResponse,
                                    &cbResponse ) );

        ChkDR( LogPacket( FALSE, pbResponse, cbResponse ) );

        LogMessage( "\n\nProcessing response from meter certificate server...\n\n" );

        ChkDR( Drm_MeterCert_ProcessResponse( f_poAppContext,
                                              pbResponse,
                                              cbResponse,
                                              &dr2 ) );
        if ( DRM_SUCCEEDED( dr ) )
        {
            /*
            ** Get the detailed status code from the server
            ** response.
            */
            ChkDR(dr2);
        }
        SAFE_OEM_FREE( pbChallenge );
        SAFE_OEM_FREE( pbResponse );

        /*
        ** At this moment, the meter certificate should have been installed
        ** at the meter cert store. Restart the enumeration to retrieve the
        ** installed meter certificate.
        */
        ChkDR( Drm_MeterCert_InitEnum( f_poAppContext, &oMeterCertEnumContext ) );

        /*
        ** Get the size of the meter certificate.
        */
        dr = Drm_MeterCert_EnumNext( &oMeterCertEnumContext,
                                     &oMID,
                                     NULL,
                                     0,
                                     &fIsV2,
                                     NULL,
                                     &cbMeterCert );

        if ( DRM_FAILED( dr ) )
        {
            if ( dr == DRM_E_BUFFERTOOSMALL )
            {
                ChkMem( pbMeterCert = ( DRM_BYTE * )Oem_MemAlloc( cbMeterCert ) );

                /* Retrieve the meter certificate needed to generate a meter data challenge. */
                ChkDR( Drm_MeterCert_EnumNext( &oMeterCertEnumContext,
                                               &oMID,
                                               NULL,
                                               0,
                                               &fIsV2,
                                               pbMeterCert,
                                               &cbMeterCert ) );
            }
            else
            {
                ChkDR( dr );
            }
        }
        else
        {
            return DRM_E_FAIL;
        }
    }

    /*
    ** Generate the metering data challenge.
    */
    dr = Drm_Metering_GenerateChallenge( f_poAppContext,
                                           pbMeterCert,
                                           cbMeterCert,
                                           NULL,
                                           0,
                                           rgchURL,
                                           &cchURL,
                                           NULL,
                                           &cbChallenge );

    if( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( pbChallenge = ( DRM_BYTE * ) Oem_MemAlloc( cbChallenge ) );

        cchURL = MAX_URL_SIZE; /* The URL buffer size is MAX_URL_SIZE, but cchURL could have been changed */

        dr = Drm_Metering_GenerateChallenge( f_poAppContext,
                                             pbMeterCert,
                                             cbMeterCert,
                                             NULL,
                                             0,
                                             rgchURL,
                                             &cchURL,
                                             pbChallenge,
                                             &cbChallenge );

    }
    ChkDR( dr );

    LogMessage( "\nMetering server URL (from meter certificate): %s\n\n", rgchURL );

    ChkDR( LogPacket( TRUE, pbChallenge, cbChallenge ) );

    /*
    ** Make the URL into a DRM_STRING
    */
    dstrMeteringServerURL.pwszString = rgwchURL;
    dstrMeteringServerURL.cchString = NO_OF( rgwchURL );

    dasstrURL.m_cch = cchURL;
    DRM_UTL_PromoteASCIItoUNICODE( rgchURL, &dasstrURL, &dstrMeteringServerURL );

    /*
    ** Post the metering data challenge.
    */
    ChkDR( tOEM_SendNetDataDSTR( ( DRM_CONST_STRING * ) &dstrMeteringServerURL,
                                  fIsV2 ? eDRM_TEST_NET_METERDATA_LEGACY : eDRM_TEST_NET_METERDATA,
                                  pbChallenge,
                                  cbChallenge,
                                 &pbResponse,
                                 &cbResponse ) );

    ChkDR( LogPacket( FALSE, pbResponse, cbResponse ) );

    /*
    ** Process the metering data response to clear the metering
    ** records in the local HDS.
    */
    LogMessage( "Now processing response from metering server...\n\n" );

    ChkDR( _ProcessMeterDataResponse( f_poAppContext, pbResponse, cbResponse ) );

    LogMessage( "Report Metering: Success\n" );

ErrorExit:

    SAFE_OEM_FREE( pbMeterCert );
    SAFE_OEM_FREE( pbChallenge );
    SAFE_OEM_FREE( pbResponse );

    if ( DRM_FAILED( dr ) )
    {
        LogMessage( "Report Metering: Failed\n" );
    }

    return dr;
}

/*********************************************************************
**
** Function: _GenerateDomainJoinChallenge
**
** Synopsis: Function that generates a domain join challenge to be
**           sent to the domain server.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poDomainID]           -- Pointer to a domain ID of the domain to join.
** [f_fEchoDomainID]        -- Flag indicating whether to enforce the server to
**                             use the domain ID in the challenge rather than
**                             create its own to send to the client.
** [f_ppbChallenge]         -- Pointer to pointer of a buffer that receives the
**                             generated domain join challenge.
** [f_pcbChallenge]         -- Pointer to a variable that receives size of the
**                             domain join challenge.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _GenerateDomainJoinChallenge(
    __in DRM_APP_CONTEXT *f_poAppContext,
    __in DRM_GUID   *f_oServiceID,
    __in DRM_GUID   *f_oAccountID,
    __out DRM_BYTE **f_ppbChallenge,
    __out DRM_DWORD *f_pcbChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR rgchFriendlyName[] = FRIENDLY_NAME;
    DRM_CHAR rgchCustomData[MAX_CUSTOM_DATA_SIZE] = CUSTOM_DATA_MSG;
    DRM_DWORD cchFriendlyName = 0;
    DRM_DWORD cchCustomData = 0;
    DRM_DOMAIN_ID oDomainID = EMPTY_DRM_DOMAIN_ID;


    ChkArg( f_poAppContext != NULL );
    ChkArg( f_oServiceID != NULL );
    ChkArg( f_oAccountID != NULL );
    ChkArg( f_ppbChallenge != NULL );
    ChkArg( f_pcbChallenge != NULL );

    MEMCPY( &(oDomainID.m_oAccountID), f_oAccountID, SIZEOF( DRM_ID ) );
    MEMCPY( &(oDomainID.m_oServiceID), f_oServiceID, SIZEOF( DRM_ID ) );

    ChkDR( DRM_STR_StringCchLengthA( rgchCustomData, SIZEOF( rgchCustomData ), &cchCustomData ) );
    ChkDR( DRM_STR_StringCchLengthA( rgchFriendlyName, SIZEOF( rgchFriendlyName ), &cchFriendlyName ) );

    dr = Drm_JoinDomain_GenerateChallenge( f_poAppContext,
                                           DRM_REGISTER_CUSTOM_DATA,
                                           &oDomainID,
                                           rgchFriendlyName,
                                           cchFriendlyName,
                                           rgchCustomData,
                                           cchCustomData,
                                           NULL,
                                           f_pcbChallenge );

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( *f_ppbChallenge = ( DRM_BYTE *)Oem_MemAlloc( *f_pcbChallenge ) );

        ChkDR( Drm_JoinDomain_GenerateChallenge( f_poAppContext,
                                                 DRM_REGISTER_CUSTOM_DATA,
                                                 &oDomainID,
                                                 rgchFriendlyName,
                                                 cchFriendlyName,
                                                 rgchCustomData,
                                                 cchCustomData,
                                                 *f_ppbChallenge,
                                                 f_pcbChallenge ) );
    }
    else
    {
        goto ErrorExit;
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _ProcessDomainJoinResponse
**
** Synopsis: Function that processes a domain join response received
**           from the domain server.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_pbResponse]           -- Pointer to a buffer that contains the domain
**                             join response from the domain server.
** [f_cbResponse]           -- Size of the domain join response from the
**                             domain server.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _ProcessDomainJoinResponse(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_BYTE *f_pbResponse,
    IN DRM_DWORD f_cbResponse )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_RESULT dr1 = DRM_SUCCESS;
    DRM_DOMAIN_ID oDomainID = EMPTY_DRM_DOMAIN_ID;

    ChkArg( f_poAppContext != NULL );

    ChkDR( Drm_JoinDomain_ProcessResponse( f_poAppContext,
                                           f_pbResponse,
                                           f_cbResponse,
                                           &dr1,
                                           &oDomainID ) );

    if ( DRM_SUCCEEDED( dr ) )
    {
        /*
        ** Get the detailed status code from the server
        ** response.
        */
        dr = dr1;
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _DoJoinDomain
**
** Synopsis: Function generates joindomain challenge message, sends it
**           over the net and processes response.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poDomainData]         -- Pointer to domain data read from initiator.
** [f_dstrCustomData]       -- Pointer to custom data string.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
static DRM_RESULT _DoJoinDomain(
    __in DRM_APP_CONTEXT *f_poAppContext,
    __in DRM_INIT_DOMAIN_DATA *f_poDomainData,
    __in DRM_CONST_STRING *f_dstrCustomData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbChallenge = NULL;
    DRM_DWORD cbChallenge = 0;
    DRM_BYTE *pbResponse = NULL;
    DRM_DWORD cbResponse = 0;

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_poDomainData != NULL );
    ChkArg( f_dstrCustomData != NULL );

    ChkDR( _GenerateDomainJoinChallenge( f_poAppContext,
                                         &f_poDomainData->oServiceID,
                                         &f_poDomainData->oAccountID,
                                         &pbChallenge,
                                         &cbChallenge ) );

    ChkDR( LogPacket( TRUE, pbChallenge, cbChallenge ) );

    ChkDR( tOEM_SendNetDataDSTR( &f_poDomainData->dstrDomainServer,
                                 eDRM_TEST_NET_DOMJOIN,
                                 pbChallenge,
                                 cbChallenge,
                                &pbResponse,
                                &cbResponse ) );

    ChkDR( LogPacket( FALSE, pbResponse, cbResponse ) );

    ChkDR( _ProcessDomainJoinResponse( f_poAppContext, pbResponse, cbResponse ) );

    LogMessage( "Join Domain: Success\n" );

ErrorExit:

    SAFE_OEM_FREE( pbChallenge );

    SAFE_OEM_FREE( pbResponse );

    if ( DRM_FAILED( dr ) )
    {
        LogMessage( "Join Domain: Failed\n" );
    }

    return dr;
}


/*********************************************************************
**
** Function: _ProcessDomainLeaveResponse
**
** Synopsis: Function that processes a domain leave response received
**           from the domain server.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_pbResponse]           -- Pointer to a buffer that contains the domain
**                             leave response from the domain server.
** [f_cbResponse]           -- Size of the domain leave response from the
**                             domain server.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _ProcessDomainLeaveResponse(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_BYTE *f_pbResponse,
    IN DRM_DWORD f_cbResponse)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_RESULT dr1 = DRM_SUCCESS;

    ChkArg( f_poAppContext != NULL );

    ChkDR( Drm_LeaveDomain_ProcessResponse( f_poAppContext,
                                            f_pbResponse,
                                            f_cbResponse,
                                            &dr1 ) );

    if ( DRM_SUCCEEDED( dr ) )
    {
        /*
        ** Get the detailed status code from the server
        ** response.
        */
        dr = dr1;
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _GenerateDomainLeaveChallenge
**
** Synopsis: Function that generates a domain leave challenge to be
**           sent to the domain server.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poDomainID]           -- Pointer to a domian ID to be left from.
** [f_ppbChallenge]         -- Pointer to pointer of a buffer that receives the
**                             generated domain leave challenge.
** [f_pcbChallenge]         -- Pointer to a variable that receives size of the
**                             domain leave challenge.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _GenerateDomainLeaveChallenge(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_DOMAIN_ID *f_poDomainID,
    OUT DRM_BYTE **f_ppbChallenge,
    OUT DRM_DWORD *f_pcbChallenge)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR rgchCustomData[ MAX_CUSTOM_DATA_SIZE ] = CUSTOM_DATA_MSG;
    DRM_DWORD cchCustomData = 0;

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_poDomainID != NULL );
    ChkArg( f_ppbChallenge != NULL );
    ChkArg( f_pcbChallenge != NULL );

    cchCustomData = ( DRM_DWORD )DRMCRT_strlen( rgchCustomData );

    dr = Drm_LeaveDomain_GenerateChallenge( f_poAppContext,
                                            DRM_REGISTER_CUSTOM_DATA,
                                            f_poDomainID,
                                            rgchCustomData,
                                            cchCustomData,
                                            NULL,
                                            f_pcbChallenge);

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( *f_ppbChallenge =
                    ( DRM_BYTE * )Oem_MemAlloc( *f_pcbChallenge ) );

        ChkDR( Drm_LeaveDomain_GenerateChallenge( f_poAppContext,
                                                  DRM_REGISTER_CUSTOM_DATA,
                                                  f_poDomainID,
                                                  rgchCustomData,
                                                  cchCustomData,
                                                  *f_ppbChallenge,
                                                  f_pcbChallenge ) );
    }
    else
    {
        goto ErrorExit;
    }

ErrorExit:
    return dr;
}


/*********************************************************************
**
** Function: _DoLeaveDomain
**
** Synopsis: Function generates leavedomain challenge message, sends it
**           over the net and processes response.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poDomainData]         -- Pointer to domain data read from initiator.
** [f_dstrCustomData]       -- Pointer to custom data string.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
static DRM_RESULT _DoLeaveDomain(
    __in DRM_APP_CONTEXT *f_poAppContext,
    __in DRM_INIT_DOMAIN_DATA *f_poDomainData,
    __in DRM_CONST_STRING *f_dstrCustomData )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DOMAIN_CERT_ENUM_CONTEXT oDomainCertEnumContext = { 0 };
    DRM_DOMAINCERT_INFO oDomainCertInfo = { 0 };
    DRM_DOMAIN_ID oDomainID = EMPTY_DRM_DOMAIN_ID;
    DRM_DWORD cchDomainCert = 0;
    DRM_BYTE *pbChallenge = NULL;
    DRM_DWORD cbChallenge = 0;
    DRM_BYTE *pbResponse = NULL;
    DRM_DWORD cbResponse = 0;

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_poDomainData != NULL );
    ChkArg( f_dstrCustomData != NULL );

    MEMCPY( &(oDomainID.m_oAccountID), &f_poDomainData->oAccountID, SIZEOF( DRM_ID ) );
    MEMCPY( &(oDomainID.m_oServiceID), &f_poDomainData->oServiceID, SIZEOF( DRM_ID ) );

    for ( ; ; )
    {
        /*
        ** Since the delete operation will break the total count of the
        ** existing enumeration. The enumeration has to be initialized
        ** every time a delete happens.
        */
        ChkDR( Drm_DomainCert_InitEnum( f_poAppContext,
                                        &oDomainCertEnumContext ) );

        dr = Drm_DomainCert_EnumNext( &oDomainCertEnumContext,
                                      &cchDomainCert,
                                      &oDomainCertInfo );

        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;

            break;
        }
        else
        {
            ChkDR( dr );
        }

        ChkDR( _GenerateDomainLeaveChallenge( f_poAppContext,
                                              &oDomainID,
                                              &pbChallenge,
                                              &cbChallenge ) );

        ChkDR( LogPacket( TRUE, pbChallenge, cbChallenge ) );

        ChkDR( tOEM_SendNetDataDSTR( &f_poDomainData->dstrDomainServer,
                                     eDRM_TEST_NET_DOMLEAVE,
                                     pbChallenge,
                                     cbChallenge,
                                     &pbResponse,
                                     &cbResponse ) );

        ChkDR( LogPacket( FALSE, pbResponse, cbResponse ) );

        ChkDR( _ProcessDomainLeaveResponse( f_poAppContext, pbResponse, cbResponse ) );

        SAFE_OEM_FREE( pbChallenge );

        SAFE_OEM_FREE( pbResponse );
    }

    LogMessage( "Leave Domain: Success\n" );

ErrorExit:
    SAFE_OEM_FREE( pbChallenge );

    SAFE_OEM_FREE( pbResponse );

    if ( DRM_FAILED( dr ) )
    {
        LogMessage( "Leave Domain: Failed\n" );
    }

    return dr;
}


/*********************************************************************
**
** Function: _ProcessAdditionalInformation
**
** Synopsis: Function that extracts additional information from the server
**           response from either the license server or the domain server,
**           depending on the input data type.
**
** Arguments:
**
** [f_pAppContext]          -- Pointer to application context.
** [f_pbResponse]           -- Pointer to a buffer that contains the server
**                             response.
** [f_cbResponse]           -- Size of the server response.
** [f_dwDataType]           -- Type of data to retriev from the server response.
** [f_pchData]              -- Pointer to a buffer that receives the extracted
**                             information from the server response.
** [f_pcchData]             -- Pointer to a variable that receives size of the
**                             extracted information from the server response.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _ProcessAdditionalInformation(
    IN DRM_APP_CONTEXT *f_pAppContext,
    IN DRM_BYTE *f_pbResponse,
    IN DRM_DWORD f_cbResponse,
    IN DRM_DWORD f_dwDataType,
    OUT DRM_CHAR *f_pchData,
    IN OUT DRM_DWORD *f_pcchData )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_pbResponse != NULL );
    ChkArg( f_cbResponse > 0 );
    ChkArg( f_pchData != NULL );
    ChkArg( f_pcchData != NULL );
    ChkArg( *f_pcchData > 0 );

    dr = Drm_GetAdditionalResponseData( f_pAppContext,
                                        f_pbResponse,
                                        f_cbResponse,
                                        f_dwDataType,
                                        f_pchData,
                                        f_pcchData );

    if ( DRM_SUCCEEDED( dr ) )
    {
        f_pchData[ *f_pcchData ] = 0;
    }

ErrorExit:

    return dr;
}


/*********************************************************************
**
** Function: _HandleLicenseError
**
** Synopsis: Function that handles error returned from the license server
**           during license acquisition.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poState]              -- Pointer to the state machine context.
** [f_poLicResponse]        -- Pointer to a structure that contains the
**                             result of processing the license acquisition
**                             response.
** [f_pbResponse]           -- Pointer to a buffer that contains the
**                             license acquisition response from the server.
** [f_cbResponse]           -- Size of the license acquisition response
**                             from the server.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _HandleLicenseError(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_LICENSE_RESPONSE *f_poLicResponse,
    DRM_BYTE *f_pbResponse,
    DRM_DWORD f_cbResponse )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR rgchGUID[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) + 1 ] = { 0 };
    DRM_DWORD cchGUID = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );
    DRM_BYTE *pbHeader = NULL;
    DRM_DWORD cbHeader = 0;

    DRM_CHAR  rgchURL[ MAX_URL_SIZE ];
    DRM_DWORD cchURL;

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_poLicResponse != NULL );
    ChkArg( f_pbResponse != NULL && f_cbResponse > 0 );

    ZEROMEM( rgchGUID, SIZEOF( rgchGUID ) );

    if ( f_poLicResponse->m_dwResult == DRM_E_SERVER_DOMAIN_REQUIRED )
    {
        LogMessage( "Get License (Challenge): Need to join a domain.\n" );

        cchURL = SIZEOF( rgchURL );

        /*
        ** Extract the domain server URL from the server response.
        ** Technically the extract domain server URL can be used to
        ** redirect the domain service to a server other than the
        ** one that host the license service. However in this
        ** application the extracted information is just displayed
        ** and is not actually used.
        */
        ChkDR( _ProcessAdditionalInformation( f_poAppContext,
                                             f_pbResponse,
                                             f_cbResponse,
                                             DRM_GARD_REDIRECT_URL,
                                             rgchURL,
                                             &cchURL ) );

        /* Display the extracted domain server URL. */
        LogMessage( "\nDomain server URL (from server's response):\n" );
        LogMessage( rgchURL );

        ChkDR( _ProcessAdditionalInformation( f_poAppContext,
                                             f_pbResponse,
                                             f_cbResponse,
                                             DRM_GARD_SERVICE_ID,
                                             rgchGUID,
                                             &cchGUID ) );
        /* Display the extracted service ID. */
        LogMessage( "\n\nService ID (from server's response):" );
        LogMessage( rgchGUID );
        LogMessage( "\n" );
    }
    else if ( f_poLicResponse->m_dwResult == DRM_E_SERVER_RENEW_DOMAIN )
    {
        LogMessage( "Get License (Challenge): Domain expired.\n" );

        cchURL = SIZEOF( rgchURL );

        /*
        ** Extract the domain server URL from the server response.
        ** Technically the extracted domain server URL can be used to
        ** redirect the domain service to a server other than the
        ** one that host the license service. However in this
        ** application the extracted information is just displayed
        ** and is not actually used.
        */
        ChkDR( _ProcessAdditionalInformation( f_poAppContext,
                                             f_pbResponse,
                                             f_cbResponse,
                                             DRM_GARD_REDIRECT_URL,
                                             rgchURL,
                                             &cchURL ) );

        /* Display the extracted domain server URL. */
        LogMessage( "\nDomain server URL (from server's response):\n" );
        LogMessage( rgchURL );

        /*
        ** Extract the service ID and account ID from the server response.
        ** Technically the extracted service ID and account ID can be used to
        ** formulate a new domain join challenge to be sent to the server.
        ** However in this application the extracted service ID and account
        ** ID are just displayed and is not actually used.
        */
        ChkDR( _ProcessAdditionalInformation( f_poAppContext,
                                             f_pbResponse,
                                             f_cbResponse,
                                             DRM_GARD_SERVICE_ID,
                                             rgchGUID,
                                             &cchGUID ) );

        /* Display the extracted service ID. */
        LogMessage( "\n\nService ID (from server's response):\n" );
        LogMessage( rgchGUID );

        ChkDR( _ProcessAdditionalInformation( f_poAppContext,
                                             f_pbResponse,
                                             f_cbResponse,
                                             DRM_GARD_ACCOUNT_ID,
                                             rgchGUID,
                                             &cchGUID ) );

        /* Display the extracted account ID. */
        LogMessage( "\n\nAccount ID (from server's response):\n" );
        LogMessage( rgchGUID );
        LogMessage( "\n" );
    }
    else if ( f_poLicResponse->m_dwResult == DRM_E_SERVER_PROTOCOL_FALLBACK )
    {
        DRM_CONST_STRING dstrHeader = EMPTY_DRM_STRING;
        DRM_CONST_STRING dstrURL = EMPTY_DRM_STRING;
        DRM_CONTENT_SET_PROPERTY eHeaderInContext = DRM_CSP_HEADER_NOT_SET;
        DRM_DWORD cbHeaderInContext = SIZEOF( DRM_CONTENT_SET_PROPERTY );

        LogMessage( "Get License (Challenge): Protocol fallback.\n" );

        LogMessage( "\nSwitch to use V2 license protocol and retry.\n" );

        /* Retrieve the current content header type in App context. */
        ChkDR( Drm_Content_GetProperty( f_poAppContext,
                                        DRM_CGP_HEADER_TYPE,
                                        ( DRM_BYTE * )&eHeaderInContext,
                                        &cbHeaderInContext ) );

        if ( eHeaderInContext != DRM_CSP_V2_4_HEADER &&
             eHeaderInContext != DRM_CSP_V2_HEADER )
        {
            LogMessage( "License fallback can only use content header type V2 or V2.4.\n\n" );
            LogMessage( "The current content header type is not V2 or V2.4.\n\n\n" );

            ChkDR( DRM_E_FAIL );
        }

        /* Retrieve the size of the content header from the App context. */
        dr = Drm_Content_GetProperty( f_poAppContext,
                                      DRM_CGP_HEADER,
                                      NULL,
                                      &cbHeader );

        DRMASSERT( dr == DRM_E_BUFFERTOOSMALL );

        /* Allocate memory to store the retrieved content header. */
        ChkMem( pbHeader = ( DRM_BYTE * )Oem_MemAlloc( cbHeader + 1 ) );

        /* Retrieve the content header from the App context. */
        ChkDR( Drm_Content_GetProperty( f_poAppContext,
                                        DRM_CGP_HEADER,
                                        pbHeader,
                                        &cbHeader ) );

        DSTR_FROM_PB( &dstrHeader, pbHeader, cbHeader );

        /*
        ** Extract the fallback license server URL from the content header.
        ** Technically the extracted license fallback server URL can be used to
        ** redirect the license service to a license server other than the current one
        ** being used (defined by HTTP_FALLBACK_XXX constants at the beginning of this file).
        ** However in this application the extracted information is just displayed and is not
        ** actually used.
        */
        ChkDR( DRM_HDR_GetAttribute( &dstrHeader,
                                     NULL,
                                     DRM_HEADER_ATTRIB_LAINFO,
                                     &dstrURL,
                                     0 ) );

        /* Add a NULL character at the end of the URL string to make it displayable. */
        ( ( DRM_WCHAR * )dstrURL.pwszString )[ dstrURL.cchString ] = 0;

        LogMessage( "\nFallback license server URL (from content header): %S\n",
                    dstrURL.pwszString );
    }
    else
    {
        dr = f_poLicResponse->m_dwResult;
    }

    LogMessage( "Get License (Challenge): Failed\n" );

ErrorExit:

    SAFE_OEM_FREE( pbHeader );

    return dr;
}



/*********************************************************************
**
** Function: _ProcessLicenseChallengeResponse
**
** Synopsis: Function that processes a license challenge response
**           received from the license server.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_pbResponse]           -- Pointer to a buffer that contains the license
**                             challenge response from the license server.
** [f_cbResponse]           -- Size of the license challenge response from
**                             the license server.
** [f_poLicResponse]        -- Pointer to a structure to receive the result of
**                             the license processing.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _ProcessLicenseChallengeResponse(
    IN DRM_APP_CONTEXT       *f_poAppContext,
    IN DRM_BYTE              *f_pbResponse,
    IN DRM_DWORD             f_cbResponse,
    OUT DRM_LICENSE_RESPONSE *f_poLicResponse )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_pbResponse != NULL && f_cbResponse > 0 );
    ChkArg( f_poLicResponse != NULL );

    ChkDR( Drm_LicenseAcq_ProcessResponse( f_poAppContext,
                                           NULL,
                                           NULL,
                                           f_pbResponse,
                                           f_cbResponse,
                                           f_poLicResponse ) );

ErrorExit:

    return dr;
}


/*********************************************************************
**
** Function: _GenerateLicenseChallenge
**
** Synopsis: Function that generates a license challenge packet to be
**           sent to the license server.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_fDomain]              -- A flag indicating whether to ask the license
**                             server to enforce domain membership when issuing
**                             a license.
** [f_fDomainExcluded]      -- A flag indicating whether to trigger the license
**                             server to return "domain excluded" error.
** [f_fAESPackage]          -- A flag indicating whether the returned license
**                             should contain an AES key. If it is FALSE, the
**                             returned license contains a RC4 key.
** [f_fProtocolFallback]    -- A flag indicating whether to trigger the license
**                             protcol fallback error.
** [f_ProtocolVersion]      -- Version of the license protocol to use.
** [f_poDomainID]           -- Pointer to a domain ID that contains the service ID
**                             that the server wants to use. It can be NULL pointer
**                             or a pointer to an empty domain ID. It is only used
**                             for V3 license protocol.
** [f_ppbChallenge]         -- Pointer to pointer of a buffer that receives the
**                             generated license challenge packet.
** [f_pcbChallenge]         -- Pointer to a variable that receives size of the
**                             license challenge packet.
** [f_pchURL]               -- Pointer to a buffer that receives the parsed out
**                             domain server URL. A URL is only used when
**                             the license server requires the caller to join
**                             a domain before issuing a license.
** [f_pcchURL]              -- Pointer to a variable that contains the size
**                             (of the buffer that is used to store the domain
**                             server URL, and stores the actual size of the
**                             parsed out domain server URL. A URL is only used
**                             when the license server requires the caller to join
**                             a domain before issuing a license.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _GenerateLicenseChallenge(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_BOOL         f_fDomain,
    IN DRM_BOOL         f_fDomainExcluded,
    IN DRM_BOOL         f_fAESPackage,
    IN DRM_BOOL         f_fProtocolFallback,
    IN DRM_LICENSE_PROTOCOL_TYPE f_ProtocolVersion,
    IN DRM_DOMAIN_ID    *f_poDomainID,
    OUT DRM_BYTE        **f_ppbChallenge,
    IN OUT DRM_DWORD    *f_pcbChallenge,
    OUT DRM_CHAR        *f_pchURL,
    IN OUT DRM_DWORD    *f_pcchURL )
{
    DRM_RESULT dr = DRM_SUCCESS;
    const DRM_CONST_STRING *rgpdstrRights[ 1 ]=
            { ( DRM_CONST_STRING * )&g_dstrWMDRM_RIGHT_PLAYBACK };
    DRM_CHAR rgchCustomData[ MAX_CUSTOM_DATA_SIZE ] = "AddMid";
    DRM_DWORD cchCustomData = 0;

    ChkArg( f_poAppContext );
    ChkArg( f_ppbChallenge != NULL );
    ChkArg( f_pcbChallenge != NULL );

#if DEMO_CHAINED_LICENSE

    ChkDR( DRM_STR_StringCchCatA( rgchCustomData,
                                  SIZEOF( rgchCustomData ),
                                  " UseChainedLicense" ) );

#endif

    if ( f_fDomain )
    {
        ChkDR( DRM_STR_StringCchCatA( rgchCustomData,
                                      SIZEOF( rgchCustomData ),
                                      " UseDomainCert" ) );
    }

    if ( f_fDomainExcluded )
    {
        ChkDR( DRM_STR_StringCchCatA( rgchCustomData,
                                      SIZEOF( rgchCustomData ),
                                      " DomainCertExpired" ) );
    }

    if ( f_fAESPackage )
    {
        ChkDR( DRM_STR_StringCchCatA( rgchCustomData,
                                      SIZEOF( rgchCustomData ),
                                      " UseAesKey" ) );
    }

    /*
    ** To avoid conflict, domain exclusion has a higher priority
    ** than license protocol fallback.
    */
    if ( !f_fDomainExcluded && f_fProtocolFallback )
    {
        ChkDR( DRM_STR_StringCchCatA( rgchCustomData,
                                      SIZEOF( rgchCustomData ),
                                      " ReplyWithFallback" ) );
    }

    ChkDR( DRM_STR_StringCchLengthA( rgchCustomData,
                                     SIZEOF( rgchCustomData ),
                                     &cchCustomData ) );

    /*
    ** The license server's URL is extracted from the content header.
    ** Technically the extract license server URL can be used to direct
    ** the license service to a server specified in the content header.
    ** However in this application the extracted information is just
    ** displayed and is not actually used.
    */
    dr = Drm_LicenseAcq_GenerateChallenge( f_poAppContext,
                                           f_ProtocolVersion == eV2Protocol ?
                                           rgpdstrRights : NULL,
                                           f_ProtocolVersion == eV2Protocol ?
                                           NO_OF( rgpdstrRights ) : 0,
                                           f_ProtocolVersion == eV2Protocol ?
                                           NULL : f_poDomainID,
                                           rgchCustomData,
                                           cchCustomData,
                                           f_pchURL,
                                           f_pcchURL,
                                           NULL,
                                           NULL,
                                           NULL,
                                           f_pcbChallenge);

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {

        ChkMem( *f_ppbChallenge = ( DRM_BYTE * )Oem_MemAlloc( *f_pcbChallenge ) );


        ChkDR( Drm_LicenseAcq_GenerateChallenge( f_poAppContext,
                                                 f_ProtocolVersion == eV2Protocol ?
                                                 rgpdstrRights : NULL,
                                                 f_ProtocolVersion == eV2Protocol ?
                                                 NO_OF( rgpdstrRights ) : 0,
                                                 f_ProtocolVersion == eV2Protocol ?
                                                 NULL : f_poDomainID,
                                                 rgchCustomData,
                                                 cchCustomData,
                                                 f_pchURL,
                                                 f_pcchURL,
                                                 NULL,
                                                 NULL,
                                                 *f_ppbChallenge,
                                                 f_pcbChallenge));
    }
    else
    {
        goto ErrorExit;
    }

ErrorExit:

    return dr;
}


/*********************************************************************
**
** Function: _GenerateLicenseAck
**
** Synopsis: Function that generates a license acknowledgement challenge
**           to be sent to the license server.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poLicResponse]        -- Pointer to a structure that contains the result
**                             of license processing.
** [f_ppbAcknowledgement]   -- Pointer to pointer of a buffer that receives the
**                             generated license acknowledgement challenge.
** [f_pcbAcknowledgement]   -- Pointer to a variable that receives size of the
**                             license acknowledgement challenge.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _GenerateLicenseAck(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_LICENSE_RESPONSE *f_poLicResponse,
    OUT DRM_BYTE **f_ppbAcknowledgement,
    IN OUT DRM_DWORD *f_pcbAcknowledgement )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_poLicResponse != NULL );
    ChkArg( f_ppbAcknowledgement != NULL );
    ChkArg( f_pcbAcknowledgement != NULL );

    dr = Drm_LicenseAcq_GenerateAck( f_poAppContext,
                                     f_poLicResponse,
                                     NULL,
                                     f_pcbAcknowledgement);

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        ChkMem( *f_ppbAcknowledgement =
                    (DRM_BYTE *)Oem_MemAlloc( *f_pcbAcknowledgement ) );

        ChkDR( Drm_LicenseAcq_GenerateAck( f_poAppContext,
                                           f_poLicResponse,
                                           *f_ppbAcknowledgement,
                                           f_pcbAcknowledgement ) );
    }
    else
    {
        goto ErrorExit;
    }

ErrorExit:

    return dr;
}


/*********************************************************************
**
** Function: ProcessLicenseAckResponse
**
** Synopsis: Function that processes a license acknowledgement response
**           from the license server.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_pbResponse]           -- Pointer to a buffer that contains the license
**                             acknowledgement response from the license server.
** [f_cbResponse]           -- Size of the license acknowledgement response
**                             from the license server.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
static DRM_RESULT _ProcessLicenseAckResponse(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_BYTE *f_pbResponse,
    IN DRM_DWORD f_cbResponse)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_RESULT dr1 = DRM_SUCCESS;

    ChkArg( f_poAppContext);
    ChkArg( f_pbResponse != NULL && f_cbResponse > 0 );

    ChkDR( Drm_LicenseAcq_ProcessAckResponse( f_poAppContext,
                                              f_pbResponse,
                                              f_cbResponse,
                                              &dr1 ) );

    if ( DRM_SUCCEEDED( dr ) )
    {
        /*
        ** Get the detailed status code from the server
        ** response.
        */
        dr = dr1;
    }

ErrorExit:

    return dr;
}


/**************************************************************************************
**
** Function: _DoLicAcquisition
**
** Synopsis: Function sets application context property, generates license
**           acquisition request, sends it over the net and processes response.
**
** Arguments:
**
** [f_poAppContext]       -- Pointer to a DRM App context initialized via
**                           Drm_Initialize().
** [f_pdstrContentHeader] -- Pointer to content header parsed from initiator.
** [f_oProtocolVersion]   -- version of licence acq protocol, must be set
**                           i.e. eUnknownProtocol is not allowed.
**
**
** Returns:                 DRM_SUCCESS on success.
**
**************************************************************************************/
static DRM_RESULT _DoLicAcquisition(
    __in DRM_APP_CONTEXT *f_poAppContext,
    __in DRM_STRING *f_pdstrContentHeader,
    __in DRM_LICENSE_PROTOCOL_TYPE f_oProtocolVersion )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbChallenge = NULL;
    DRM_DWORD cbChallenge = 0;
    DRM_BYTE *pbResponse = NULL;
    DRM_DWORD cbResponse = 0;
    DRM_LICENSE_RESPONSE oLicResponse = { eUnknownProtocol };
    DRM_BOOL fUseSoap = TRUE;
    DRM_BOOL fServerEnforceDomain = FALSE;

    DRM_CHAR rgchURL[ MAX_URL_SIZE ];
    DRM_DWORD cchURL;

    DRM_CONTENT_SET_PROPERTY eHeaderInContext = DRM_CSP_HEADER_NOT_SET;
    DRM_DWORD cbHeaderInContext = SIZEOF( DRM_CONTENT_SET_PROPERTY );

    DRM_DOMAIN_ID oDomainID = EMPTY_DRM_DOMAIN_ID;


    ChkArg( f_poAppContext != NULL );
    ChkArg( f_pdstrContentHeader != NULL );
    ChkArg( f_oProtocolVersion == eV2Protocol ||
            f_oProtocolVersion == eV3Protocol );

    ChkDR( Drm_Content_SetProperty( f_poAppContext,
                                    DRM_CSP_AUTODETECT_HEADER,
                                    (DRM_BYTE*)f_pdstrContentHeader->pwszString,
                                    f_pdstrContentHeader->cchString * SIZEOF(DRM_WCHAR) ) );


    /*
    ** Retrieve the current content header type in App context.
    */
    ChkDR( Drm_Content_GetProperty( f_poAppContext,
                                    DRM_CGP_HEADER_TYPE,
                                    ( DRM_BYTE * )&eHeaderInContext,
                                    &cbHeaderInContext ) );

    DRMASSERT( cbHeaderInContext == SIZEOF( DRM_CONTENT_SET_PROPERTY ) &&
               eHeaderInContext != DRM_CSP_HEADER_NOT_SET );

    /*
    ** If the version of the license protocol to use is V2 and the current content
    ** header type in the App context is of version V2.4, explicitly set the current
    ** content header type in the App context is of version V2.
    */
    if ( f_oProtocolVersion == eV2Protocol )
    {
        fUseSoap = FALSE;

        /*
        ** If the current content header type is V2.4, and the license protocol being
        ** used is V2, explicitly set the current content header type to be V2.
        */
        if ( eHeaderInContext == DRM_CSP_V2_4_HEADER )
        {
            /* Explicitly set the content header type to V2. */
            ChkDR( Drm_Content_SetProperty( f_poAppContext, DRM_CSP_V2_HEADER, NULL, 0 ) );
        }
        else if ( eHeaderInContext != DRM_CSP_V2_HEADER )
        {
            LogMessage( " Unexpected content header type.\n\n");

            ChkDR( DRM_E_FAIL );
        }
    }

    cchURL = SIZEOF( rgchURL );

    if ( DRM_CSP_V4_HEADER == eHeaderInContext || DRM_CSP_V4_1_HEADER == eHeaderInContext )
    {
        fServerEnforceDomain = TRUE;
    }

    ChkDR( _GenerateLicenseChallenge( f_poAppContext,
                                      fServerEnforceDomain,
                                      FALSE,
                                      ( f_oProtocolVersion == eV2Protocol ) ? FALSE : TRUE,
                                      FALSE,
                                      f_oProtocolVersion,
                                      &oDomainID,
                                      &pbChallenge,
                                      &cbChallenge,
                                      rgchURL,
                                      &cchURL ) );

    /* Display the extracted license server URL. */
    LogMessage( "\nLicense server URL (from header of the input file): " );
    LogMessage( rgchURL );

    /* Display the version of the license protocol being used. */
    LogMessage( "\nLicense protocol version: %s\n",
                f_oProtocolVersion == eV2Protocol ? "V2" : "V3" );

    ChkDR( LogPacket( TRUE, pbChallenge, cbChallenge ) );

    ChkDR( tOEM_SendNetData( rgchURL,
                             fUseSoap ? eDRM_TEST_NET_LICGET : eDRM_TEST_NET_LICGET_LEGACY,
                             pbChallenge,
                             cbChallenge,
                            &pbResponse,
                            &cbResponse ) );

    ChkDR( LogPacket( FALSE, pbResponse, cbResponse ) );

    oLicResponse.m_dwResult = DRM_SUCCESS;

    ChkDR( _ProcessLicenseChallengeResponse( f_poAppContext,
                                             pbResponse,
                                             cbResponse,
                                             &oLicResponse ) );

    if ( !DRM_SUCCEEDED( oLicResponse.m_dwResult ) )
    {
        ChkDR( _HandleLicenseError( f_poAppContext,
                                    &oLicResponse,
                                    pbResponse,
                                    cbResponse ) );
    }
    else
    {
        LogMessage( "Get License (Challenge): Success\n" );

        if ( f_oProtocolVersion == eV3Protocol )
        {
            ChkDR( _GenerateLicenseAck( f_poAppContext,
                                        &oLicResponse,
                                        &pbChallenge,
                                        &cbChallenge ) );

            ChkDR( LogPacket( TRUE, pbChallenge, cbChallenge ) );

            ChkDR( tOEM_SendNetData( rgchURL,
                                     eDRM_TEST_NET_LICACK,
                                     pbChallenge,
                                     cbChallenge,
                                    &pbResponse,
                                    &cbResponse ) );

            ChkDR( LogPacket( FALSE, pbResponse, cbResponse ) );

            ChkDR( _ProcessLicenseAckResponse( f_poAppContext,
                                               pbResponse,
                                               cbResponse ) );
        }
        LogMessage( "Get License (Ack): Success\n" );
    }

ErrorExit:

    SAFE_OEM_FREE( pbChallenge );
    SAFE_OEM_FREE( pbResponse );

    if ( DRM_FAILED( dr ) )
    {
        LogMessage( "Get License (Challenge/Ack): Failed\n" );
    }

    return dr;
}


/*********************************************************************
**
** Function: _DownloadContent
**
** Synopsis: Calls tOem_NetSaveFile to download a file then optionally embeds
**           licenses in the content.
**
** Arguments:
**
** [f_poInitData]          -- Pointer to the license acquisition initiator data.
** [f_pdstrFileName]       -- Filename to hold the downloaded content
** [f_fEmbed]              -- Flag to say if the embedded store should be updated
** [f_poAppContext]        -- Pointer to a DRM App context initialized via
**                            Drm_Initialize().
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
#if defined(_PREFAST_)
#pragma prefast(push)
#pragma prefast(disable:6262) /* Temporarily disable stack limit check. */
#endif
static DRM_RESULT _DownloadContent( __in     DRM_INIT_LICACQ_DATA *f_poInitData,
                                    __in     DRM_CONST_STRING *f_pdstrFileName,
                                    __in     DRM_BOOL          f_fEmbed,
                                    __in_opt DRM_APP_CONTEXT  *f_pAppContext )
{
    DRM_RESULT             dr         = DRM_SUCCESS;
    DRM_MEDIA_FILE_CONTEXT oMediaFile = EMPTY_DRM_MEDIA_FILE_CONTEXT;
    DRM_CONST_STRING       dstrFullPath = EMPTY_DRM_STRING;
    DRM_CONST_STRING       dstrFullPathConverted = EMPTY_DRM_STRING;
    DRM_CONST_STRING       *pdstrPath = NULL;

    ChkArg( f_poInitData != NULL );

    /* Build the path for storing the file. */
    dstrFullPath.cchString = f_pdstrFileName->cchString + g_dstrDownloadPath.cchString;
    ChkOverflow( dstrFullPath.cchString+1, dstrFullPath.cchString );
    ChkMem( dstrFullPath.pwszString = (DRM_WCHAR*) Oem_MemAlloc( (dstrFullPath.cchString+1) * SIZEOF(DRM_WCHAR) ) );
    ZEROMEM( (DRM_BYTE*) dstrFullPath.pwszString, ( ( dstrFullPath.cchString + 1 ) * SIZEOF( DRM_WCHAR ) ) );

    MEMCPY( (DRM_BYTE*) dstrFullPath.pwszString, g_dstrDownloadPath.pwszString, g_dstrDownloadPath.cchString*SIZEOF(DRM_WCHAR) );
    MEMCPY( (DRM_BYTE*)  dstrFullPath.pwszString + (g_dstrDownloadPath.cchString*SIZEOF(DRM_WCHAR)), f_pdstrFileName->pwszString, f_pdstrFileName->cchString *SIZEOF(DRM_WCHAR) );

    PUT_BYTE((DRM_BYTE*)  dstrFullPath.pwszString, ( dstrFullPath.cchString * SIZEOF(DRM_WCHAR) ), 0x00 );
    PUT_BYTE((DRM_BYTE*)  dstrFullPath.pwszString, ( dstrFullPath.cchString * SIZEOF(DRM_WCHAR) ) + 1, 0x00 );

    ChkDR( tOEM_NetSaveFileDSTR( &f_poInitData->dstrContentURL,
                                 &dstrFullPath,
                                 NULL ) );

    printf( "File saved to: %S\n", dstrFullPath.pwszString );

    if ( f_poInitData->fWMConvert )
    {
        ChkDR( _ConvertFileNameFromWmdrmToPlayReady( &dstrFullPath, &dstrFullPathConverted ) );

        ChkDR( TST_UTL_ConvertFileFromWMDRMToPlayReady( &dstrFullPath,
                                                        &dstrFullPathConverted,
                                                        &f_poInitData->dstrLAURL,
                                                        &f_poInitData->dstrLUIURL,
                                                        &f_poInitData->dstrDSID ) );

        pdstrPath = &dstrFullPathConverted;
    }
    else
    {
        pdstrPath = &dstrFullPath;
    }

    if ( f_fEmbed )
    {
        ChkArg( f_pAppContext );
        ChkDR( DRMTOOLS_OpenMediaFile( pdstrPath, &oMediaFile ) );
        ChkDR( Drm_Reinitialize( f_pAppContext ) );
        ChkDR( Drm_Content_SetProperty( f_pAppContext,
                                        DRM_CSP_AUTODETECT_HEADER,
                                        oMediaFile.rgbDRMHeaderData,
                                        oMediaFile.cbDRMHeaderData ) );

        ChkDR( Drm_Content_UpdateEmbeddedStore( f_pAppContext ) );
        ChkDR( Drm_Content_GetProperty( f_pAppContext,
                                        DRM_CGP_PLAYREADY_OBJ,
                                        oMediaFile.rgbDRMHeaderData,
                                       &oMediaFile.cbDRMHeaderData ) );
        ChkDR( DRMTOOLS_UpdatePlayReadyObject( &oMediaFile ) );

        printf( "Embedded Store updated.\n" );
    }

ErrorExit:

    if ( oMediaFile.fOpened )
    {
        DRMTOOLS_CloseMediaFile( &oMediaFile );
    }

    FREEDRMSTRING( dstrFullPathConverted );

    FREEDRMSTRING( dstrFullPath );

    return dr;
}
#if defined(_PREFAST_)
#pragma prefast(pop)
#endif


/**************************************************************************************
**
** Function: _MainLoop
**
** Synopsis: Function loops thru the list of initiator messages in the input data string
**           and calls other routines to process each message according to its type.
**           There is no state preserved between calls other than in application context.
**           Also no verification of correctness of initiator messages sequence - for example,
**           three identical joindomain initiator messages in a row is not considered
**           an error - but real-life customer's application may check for these issues.
**
** Arguments:
**
** [f_poAppContext]       -- Pointer to a DRM App context initialized via
**                           Drm_Initialize().
** [f_pdstrContentHeader] -- Pointer to content header parsed from initiator.
** [f_oProtocolVersion]   -- version of licence acq protocol, must be set
**                           i.e. eUnknownProtocol is not allowed.
**
**
** Returns:                 DRM_SUCCESS on success.
**
**************************************************************************************/
static DRM_RESULT _MainLoop(
    __in DRM_APP_CONTEXT *f_poAppContext,
    __in DRM_STRING *f_pdstrInitiatorList
)
{
    DRM_RESULT                  dr                  = DRM_SUCCESS;
    DRM_INITIATORS_ENUM_CONTEXT oEnumContext        = {0};
    DRM_INITIATOR_DATA          oInitiatorData      = EMPTY_DRM_INITIATOR_DATA;
    DRM_DWORD                   cbDecodedCustomData = 0;
    DRM_BYTE                   *pbDecodedCustomData = NULL;
    DRM_CONST_STRING            dstrDataStoreFile   = CREATE_DRM_STRING( g_rgwchHdsStoreFile );
    DRM_BYTE                   *pbRevocationBuffer  = NULL;
    DRM_DWORD                   cbRevocationBuffer  = REVOCATION_BUFFER_SIZE;



    ChkDR( Drm_Initiator_InitEnum( (DRM_CONST_STRING*) f_pdstrInitiatorList,
                                    &oEnumContext ) );

    LogMessage( "\nReceived PlayReady Initiator:\n" );

    /*
    ** Allocate a buffer for revocation data processing
    */
    ChkMem( pbRevocationBuffer = (DRM_BYTE *) Oem_MemAlloc( REVOCATION_BUFFER_SIZE ) );

    while ( dr != DRM_E_NOMORE )
    {
        dr = Drm_Initiator_EnumNext( &oEnumContext, &oInitiatorData );
        if ( dr == DRM_E_NOMORE )
        {
            break;
        }
        ChkDR(dr);

        if ( oInitiatorData.dstrCustomData.cchString > 0 )
        {
            cbDecodedCustomData = CCH_BASE64_EQUIV( oInitiatorData.dstrCustomData.cchString * SIZEOF(DRM_WCHAR) + 1 );
            pbDecodedCustomData = (DRM_BYTE*)Oem_MemAlloc(cbDecodedCustomData);
            ZEROMEM(pbDecodedCustomData, cbDecodedCustomData);

            ChkDR( DRM_B64_DecodeW( (DRM_CONST_STRING*) &oInitiatorData.dstrCustomData,
                                    &cbDecodedCustomData,
                                    ( DRM_BYTE * )pbDecodedCustomData,
                                    0 ) );

            LogMessage( "Custom Data:\n\t" );
            printf( (const char*)pbDecodedCustomData );
            LogMessage("\n");
        }

        ChkDR( Drm_Initialize( f_poAppContext, NULL, &dstrDataStoreFile ) );
        ChkDR( Drm_Revocation_SetBuffer( f_poAppContext, pbRevocationBuffer, cbRevocationBuffer ) );

        /*
        ** parse initiator of particular type and compose a message to a server
        ** depending on what we are asked to do.
        */
        switch( oInitiatorData.eType )
        {
            case DRM_INIT_METERING:
            {
                ChkDR( _DoMetering( f_poAppContext,
                                    &oInitiatorData.Data.oMeteringData ) );
            }
            break;

            case DRM_INIT_JOINDOMAIN:
            {
                LogMessage( "Joining a Domain:\n\tURL:" );
                LogDrmString( (DRM_CONST_STRING*)&oInitiatorData.Data.oDomainData.dstrDomainServer );
                LogMessage( "\n" );

                ChkDR( _DoJoinDomain( f_poAppContext,
                                      &oInitiatorData.Data.oDomainData,
                                      &oInitiatorData.dstrCustomData ) );
            }
            break;

            case DRM_INIT_LEAVEDOMAIN:
            {
                LogMessage( "Leaving a Domain\n\n" );

                ChkDR( _DoLeaveDomain( f_poAppContext,
                                       &oInitiatorData.Data.oDomainData,
                                       &oInitiatorData.dstrCustomData ) );
            }
            break;

            case DRM_INIT_LICENSEACQUISITION:
            {
                DRM_CONST_STRING dstrContentFile   = EMPTY_DRM_STRING;

                LogMessage( "Acquiring a License\n" );

                ChkDR( Drm_Reinitialize( f_poAppContext ) );

                ChkDR( _DoLicAcquisition( f_poAppContext,
                                          (DRM_STRING*)&oInitiatorData.Data.oLicAcqData.dstrContentHeader,
                                          eV3Protocol ) );

                if( oInitiatorData.Data.oLicAcqData.dstrContentURL.cchString != 0 )
                {
                    DRM_BOOL fEmbed = FALSE;

                    if ( oInitiatorData.dstrCustomData.cchString > 0 )
                    {
                        /*
                        ** Example of possible usage of custom data:
                        ** custom data should look like "d;string",
                        ** where if d == '1' then license should be embedded, otherwise don't embed,
                        ** and 'string' is name of downloaded content file.
                        */
                        if ( '1' == GET_CHAR(pbDecodedCustomData, 0 ) )
                        {
                           fEmbed = TRUE;
                        }
                        ChkBOOL( ';' == GET_CHAR( pbDecodedCustomData, 1 ), DRM_E_FAIL );
                        ChkDR( MakeDRMString( &dstrContentFile, ( const char *) pbDecodedCustomData+2) );
                    }
                    else
                    {
                        ChkDR( MakeDRMString( &dstrContentFile, "downloadedfile.wma" ) );
                    }
                    dr = _DownloadContent( &oInitiatorData.Data.oLicAcqData,
                                           &dstrContentFile,
                                           fEmbed,
                                           f_poAppContext);

                    FREEDRMSTRING( dstrContentFile );

                    ChkDR(dr);
                }
            }
            break;

            default:
                LogMessage( "Cannot parse the message:\n" );
                LogDrmString( (DRM_CONST_STRING*) &oEnumContext.dstrCurrent );
                LogMessage("\n");

                ChkDR( DRM_E_FAIL );
                break;
        }

        Drm_Uninitialize( f_poAppContext );
    }
    if( dr == DRM_E_NOMORE )
    {
        dr = DRM_SUCCESS;
    }
ErrorExit:
    SAFE_OEM_FREE( pbRevocationBuffer );
    SAFE_OEM_FREE( pbDecodedCustomData );
    return dr;
}


/************************************************************************************
**
** Function: DRM_Main
**
** Synopsis: Parses and verifies input arguments where the 2nd one
**           is initiators message data, initializes app context
**           and starts messages parsing loop.
**           Because the message is HTTP response the function assumes
**           it is in UTF-8 format but the format can be realization-specific.
**
** Returns:                 DRM_SUCCESS on success.
**
************************************************************************************/
DRM_LONG DRM_CALL DRM_Main(
    DRM_LONG argc,
    DRM_WCHAR** argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_APP_CONTEXT *poAppContext = NULL;
    DRM_STRING dstrInputFile = EMPTY_DRM_STRING;
    DRM_STRING dstrFileExtension = EMPTY_DRM_STRING;
    DRM_DWORD cbInputData = 0;
    DRM_BYTE *pbInputData = NULL;
    DRM_STRING dstrUnicodeData = EMPTY_DRM_STRING;

    /*
    ** first parameter is an input file name
    */
    ChkBOOL( argc == 2, DRM_E_INVALIDARG );

    dstrInputFile.pwszString = argv[1];
    ChkDR( DRM_STR_StringCchLengthW( argv[1], DRM_STRSAFE_MAX_CCH, &dstrInputFile.cchString ) );

    /*
    ** If file name ends with ".cms" then it's PlayReady initiator message
    */
    dstrFileExtension.cchString = 4;
    dstrFileExtension.pwszString = dstrInputFile.pwszString + dstrInputFile.cchString - 4;
    ChkBOOL( DRM_UTL_DSTRStringsEqual( (DRM_CONST_STRING*)&dstrFileExtension, &g_dstrFileExtensionCMS ), DRM_E_INVALIDARG );

    /*
    ** read UTF-8 data, may contain multibytes in URLs
    */
    ChkDR( DRMTOOLS_ReadBinaryFileData( (DRM_CONST_STRING*)&dstrInputFile, &pbInputData, &cbInputData ) );

    /*
    ** convert to unicode for easier parsing
    */
    dr = DRM_STR_UTF8toDSTR(
        (DRM_CHAR *)pbInputData,
        cbInputData,
        &dstrUnicodeData );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, dr );

    #pragma prefast(push)
    #pragma prefast(disable:22011) /*ChkOverflow checks for arithmetic but Prefast still warns */
    ChkOverflow( dstrUnicodeData.cchString * SIZEOF (DRM_WCHAR), dstrUnicodeData.cchString );
    dstrUnicodeData.pwszString = (DRM_WCHAR *) Oem_MemAlloc(dstrUnicodeData.cchString * SIZEOF (DRM_WCHAR));
    #pragma prefast(pop)

    ChkDR( DRM_STR_UTF8toDSTR(
        (DRM_CHAR *)pbInputData,
        cbInputData,
        &dstrUnicodeData ) );

    ChkMem( poAppContext = ( DRM_APP_CONTEXT * )Oem_MemAlloc( SIZEOF( DRM_APP_CONTEXT ) ) );
    ZEROMEM( poAppContext, SIZEOF( DRM_APP_CONTEXT ) );

    Oem_Clock_SetResetState( NULL, FALSE );

    LogMessage( "Parsing PlayReady initiator.\n" );

    /*
    ** process initiator(s): detect its type and act accordingly
    */
    ChkDR( _MainLoop( poAppContext, &dstrUnicodeData ) );

ErrorExit:
    printf( "return code: 0x%X\r\n.", dr );

    printf("\nPress <Return> to exit...\n");
    if( 0 == getchar() && DRM_SUCCEEDED( dr ) )
    {
        dr = DRM_S_FALSE;
    }

    SAFE_OEM_FREE(pbInputData);
    FREEDRMSTRING( dstrUnicodeData );

    return dr;
}


#else

DRM_LONG DRM_CALL DRM_Main(
    DRM_LONG argc,
    DRM_WCHAR** argv )
{
    printf( "The Initiator Helper App requires DRM_TEST_SUPPORT_NET_IO=1" );
    return DRM_S_FALSE;
}

#endif /* DRM_TEST_SUPPORT_NET_IO */
