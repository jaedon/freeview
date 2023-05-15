/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_SCRATCH_SIZE            4096
#define MAX_HTTP_RETRIES            3

#if !SIXTEEN_BIT_ADDRESSING
/* windows based include files are not available on sixteen bit platform*/
#include <drmwindowsenv.h>
//#include <winbase.h>
#endif

#include <tOEMImp.h>
#include <tOEMImp_PD.h>
#include "drmcontextsizes.h"
#include <drmutf.h>

#include "tstutils.h"
#include "tstHDSutils.h"
#include "tstXMLutils.h"
#include "drmcrt.h"
#include "tclib.h"
#include "tNetHeaders.h"

#include "drmsecureclock.h"
#include "drmmanager.h"
#include <drmtoolsinit.h>

#include "oemrsa.h"
#include <drmwmdrmnet.h>
#include <drmwmdrm.h>

#if DRM_TEST_SUPPORT_NET_IO
#include <wininet.h>

#pragma comment( lib, "wininet.lib" )

/*
** Redefine ChkWR to always log the error
*/
#ifdef ChkWR
#undef ChkWR
#endif

#define ChkWR(expr) {               \
            if (!(expr))            \
            {                       \
                dr = 0/*GESTATE*/;\
                dr = DRM_RESULT_FROM_WIN32(dr);\
                Log( "ChkWR", "WIN32 error %#X at %s : %d.\n%s\n", dr, __FILE__, __LINE__, #expr ); \
                goto ErrorExit;     \
            }                       \
        }


#ifndef ERROR_WINHTTP_TIMEOUT
/*
** From MSDN documentation, value of ERROR_WINHTTP_TIMEOUT
*/
#define ERROR_WINHTTP_TIMEOUT 12002
#endif

#endif

#ifndef TOOLS_DRM_BUFFER_SIZE
#define TOOLS_DRM_BUFFER_SIZE MINIMUM_APPCONTEXT_OPAQUE_BUFFER_SIZE
#endif /* TOOLS_DRM_BUFFER_SIZE */

ENTER_PKTEST_NAMESPACE_CODE


DRM_RESULT DRM_CALL SetDeviceEnv(
    const DRM_WCHAR  *f_wszDevcertTemplateFileName,
    const DRM_WCHAR  *f_wszPrvKeyFileName,
    const DRM_WCHAR  *f_wszGroupCertFileName,
    const DRM_WCHAR  *f_wszGroupCertPrvKeyFileName,
          DRM_BOOL    f_fCreateDevCert)
{
    DRM_RESULT       dr              = DRM_SUCCESS;
    DRM_APP_CONTEXT *poDrmContext    = NULL;
    DRM_BYTE        *pbOpaqueBuffer  = NULL;
    DRM_CONST_STRING dstrPathname    = EMPTY_DRM_STRING;
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

#if !WINCE_TEST
    ChkDR( _CopyFileToDRMFolder( f_wszPrvKeyFileName, PRIVATE_KEY_FILE_NAME ) );
    ChkDR( _CopyFileToDRMFolder( f_wszDevcertTemplateFileName, DEVCERT_TEMPLATE_FILE_NAME ) );
#else
    /*
    ** Use Built in device/keys cert on WinCE. If they do not exist then use the copied ones.
    */
    _CopyFileToDRMFolder( f_wszPrvKeyFileName, WMDRMPD_KEY_FILE_NAME );
    _CopyFileToDRMFolder( f_wszDevcertTemplateFileName, DEVCERT_TEMPLATE_FILE_NAME );
#endif

    ChkDR( _CopyFileToDRMFolder( f_wszGroupCertPrvKeyFileName, GC_PRIVATE_KEY_FILE_NAME ) );
    ChkDR( _CopyFileToDRMFolder( f_wszGroupCertFileName, GROUP_CERT_FILE_NAME ) );
    ChkDR( RemoveDRMFile( RMFILE_DEVCERT ) );

    if (f_fCreateDevCert)
    {
        ChkDR( tGetDeviceStorePathname(wszPathName, &dstrPathname) );

        ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, &dstrPathname, TOOLS_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &poDrmContext ) );
    }
ErrorExit:
    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &poDrmContext );
    return dr;
}

DRM_RESULT DRM_CALL InitWithOemData(IN DRM_VOID *f_pOEMContext)
{
    DRM_RESULT       dr              = DRM_SUCCESS;
    DRM_APP_CONTEXT *poDrmContext    = NULL;
    DRM_BYTE        *pbOpaqueBuffer  = NULL;
    DRM_CONST_STRING dstrPathname    = EMPTY_DRM_STRING;
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

    ChkDR( tGetDeviceStorePathname(wszPathName, &dstrPathname) );
    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, &dstrPathname, TOOLS_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &poDrmContext ) );

ErrorExit:
    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &poDrmContext );
    return dr;
}


DRM_RESULT DRM_CALL SetNDREnv(
    const DRM_WCHAR  *f_wszNDRCertTemplateFileName,
    const DRM_WCHAR  *f_wszNDRGroupPrvKeyFileName,
          DRM_BOOL    f_fCreateDevCert )
{
    DRM_RESULT          dr              = DRM_SUCCESS;
    DRM_APP_CONTEXT     *poDrmContext   = NULL;
    DRM_BYTE            *pbOpaqueBuffer = NULL;
    DRM_CONST_STRING    dstrPathname    = EMPTY_DRM_STRING;
    DRM_WCHAR wszPathName[GET_DRM_FILE_NAME_PATH_LENGTH] = {'\0'};

    ChkDR( _CopyFileToDRMFolder( f_wszNDRCertTemplateFileName, NDR_MODEL_CERT ) );
    ChkDR( _CopyFileToDRMFolder( f_wszNDRGroupPrvKeyFileName, NDR_MODEL_PRIVATE_KEY ) );

    if ( f_fCreateDevCert )
    {
        ChkDR( tGetDeviceStorePathname(wszPathName, &dstrPathname) );
        ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, &dstrPathname, TOOLS_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &poDrmContext ) );
        ChkDR( DRM_NDCert_Certificate_Initialize( poDrmContext, NULL ) );
    }
ErrorExit:
    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &poDrmContext );
    return dr;
}

DRM_RESULT DRM_CALL tSetSecureClock(DRM_DST* poDST, DRM_BYTE* rgbSSTPassword)
{
    DRM_RESULT dr;
    DRMFILETIME timeCurrent;
    DRM_SECSTORE_CLOCK_DATA oClkData;
    DRM_WORD wSecureDataLen =0;
    DRM_SECSTORE_CONTEXT *pSecStoreContext = NULL;

    /* From secureclockrequest.c */
#if !SIXTEEN_BIT_ADDRESSING
    const DRM_ID g_idSData     = { 'S', 0, 'E', 0, 'C', 0, 'U', 0, 'R', 0, 'E', 0, '1', 0, '\0', 0 };
    const DRM_ID g_idSDataPrev = { 'S', 0, 'E', 0, 'C', 0, 'U', 0, 'R', 0, 'E', 0, '2', 0, '\0', 0 };
#else
    const DRM_ID g_idSData     = { 'S', 'E', 'C', 'U', 'R', 'E', '1', '\0'  };
    const DRM_ID g_idSDataPrev = { 'S', 'E', 'C', 'U', 'R', 'E', '2', '\0'  };
#endif

    Oem_Clock_GetSystemTimeAsFileTime(NULL, &timeCurrent);

    MEMSET(&oClkData, 0, SIZEOF(oClkData));
    oClkData.flag = CLK_LK_GSC_VALID | CLK_LK_RSC_VALID | CLK_REFRESHDATE_VALID | CLK_EVER_SET;
    FILETIME_TO_UI64(timeCurrent, oClkData.LastKnownGoodSecureClock);
    FILETIME_TO_UI64(timeCurrent, oClkData.LastKnownRealtimeSecureClock);

    /* Refresh date is 30 days later. */
    FILETIME_TO_UI64(timeCurrent, oClkData.RefreshDate);
    oClkData.RefreshDate = DRM_UI64Add(oClkData.RefreshDate, DRM_UI64Mul(DRM_UI64(30 * 24 * 60 * 60), DRM_UI64(FILETIME_TO_SECONDS)));

    ChkMem(pSecStoreContext = (DRM_SECSTORE_CONTEXT*)Oem_MemAlloc(SIZEOF(DRM_SECSTORE_CONTEXT)));

    /* Now set the clock data to the secure store. */
    wSecureDataLen = SIZEOF(oClkData);
    ChkDR( DRM_SST_SetData( pSecStoreContext,
                        &g_idSData,
                        &g_idSDataPrev,
                        rgbSSTPassword,
                        SECURE_STORE_GLOBAL_DATA,
                        poDST,
                        (DRM_BYTE *) &oClkData,
                        wSecureDataLen ) );

    ChkDR(Oem_Clock_SetResetState(NULL, FALSE));
ErrorExit:
    SAFE_OEM_FREE(pSecStoreContext);
    return dr;
}

DRM_RESULT DRM_CALL tGetBBContext(DRM_BB_CONTEXT **ppBB)
{
    DRM_RESULT dr;
    DRM_BYTE rgbBuffer[MAX_DEVICE_CERT_SIZE];
    DRM_CONST_STRING dstrDevCert = EMPTY_DRM_STRING;

    ChkArg(ppBB);
    ChkMem(*ppBB = (DRM_BB_CONTEXT*)Oem_MemAlloc(SIZEOF(DRM_BB_CONTEXT)));
    DSTR_FROM_PB( &dstrDevCert, rgbBuffer, SIZEOF(rgbBuffer) );

    ChkMem( (*ppBB)->pKeyFileContext = ( DRM_KEYFILE_CONTEXT * ) Oem_MemAlloc( SIZEOF( DRM_KEYFILE_CONTEXT ) ) );
    ChkDR( DRM_KF_Initialize( NULL, (*ppBB)->pKeyFileContext ) );
    ChkDR( DRM_KF_Open( (*ppBB)->pKeyFileContext, TRUE, OEM_GENERIC_READ ) );
    ChkDR( DRM_KF_LoadFromFile( (*ppBB)->pKeyFileContext, TRUE ) );
    ChkDR( DRM_BBX_SetupSecureStoreKey( (*ppBB)->pKeyFileContext, NULL ) );
    ChkDR( DRM_KF_Close( (*ppBB)->pKeyFileContext ) );

    /*blackbox initialized */
    if ( DRM_FAILED( TST_UTL_GetWMDRMCert( (DRM_STRING*)&dstrDevCert,
                                           &((*ppBB)->CryptoContext) ) ) )
    {
        ChkDR( SetDeviceEnv( DEVCERT_TEMPLATE_FILE_NAME,
                             PRIVATE_KEY_FILE_NAME,
                             GROUP_CERT_FILE_NAME,
                             GC_PRIVATE_KEY_FILE_NAME,
                             FALSE ) );
    }
    if( DRM_WMDRM_IsWMDRMSupported() )
    {
        ChkDR( TST_UTL_SetWMDRMCertCachedValues( *ppBB ) );
    }
    ChkDR( TST_UTL_SetPlayReadyCertCachedValues( *ppBB ) );

    ChkDR( DRM_BBX_Initialize( NULL, *ppBB ) );
ErrorExit:
    FREEDRMSTRING( dstrDevCert );
    return dr;
}

void DRM_CALL tFreeBBContext(DRM_BB_CONTEXT *pBB)
{
    if (pBB != NULL) {
        DRM_BBX_Shutdown(pBB);

        DRM_KF_Uninitialize(pBB->pKeyFileContext);
        SAFE_OEM_FREE( pBB->pKeyFileContext );

        SAFE_OEM_FREE(pBB);
    }
}

void DRM_CALL tGetBBVersion(DRM_BB_CONTEXT *pBB, DRM_BYTE *pBBVer)
{
    /* The real current version of drm is 2.3.101.0 */
    MEMCPY( pBBVer, pBB->cachedCertValues.m_BBCompatibilityCert.pk.version, VERSION_LEN);
}


#if DRM_TEST_SUPPORT_NET_IO
#define MAX_REDIRECTIONS_PER_REQUEST    5
#define MAX_HTTP_HEADER_SIZE        4096


/* HTTP status code of temporary redirection. */
#define HTTP_STATUS_TEMPORARY_REDIRECT  307
/*******************************************************************************
** Function:    _SetHttpHeader
**
** Synopsis:    Sets the http header string based on the action given.
**
**              [f_eHttpAction]   The action to setup up.
**              [f_pdastrHeader]  pointer to a DRM_ANSI_STRING. On output the
**                                members of the struct are initialized and
**                                contain the HTTP header to send up to the
**                                server.
**              [f_pfPost]        pointer to a Boolean to say if this is a post message or not.
**
** Returns:
**              DRM_SUCCESS            The function completed successfully.
**              DRM_E_NOTIMPL          Tried to use an unsupoorted action.
**              DRM_E_TEST_INVALIDARG  One or more arguments were invalid
**
**
** Notes:       The user must take care to free the pszString member of
**              f_pdastrHeader. Memory for the buffer will be alloced by this function.
*******************************************************************************/
static DRM_RESULT DRM_CALL _CrackAction(
    __in const eDRM_TEST_NET_ACTION  f_eHttpAction,
    __out_ecount(1) DRM_ANSI_STRING *f_pdastrHeader,
    __out_ecount(1) DRM_BOOL        *f_pfPost,
    __out_ecount(1) DRM_BOOL        *f_pfUseSoap,
    __out_ecount(1) DRM_ANSI_STRING *f_pdastrFormat
    )
{
    DRM_RESULT dr = DRM_SUCCESS;
    ChkArg( f_pdastrHeader != NULL );
    ChkArg( f_pdastrFormat != NULL );
    ChkArg( f_pfPost != NULL );
    ChkArg( f_pfUseSoap != NULL );
    *f_pfPost = TRUE; /* defaults to TRUE */
    *f_pfUseSoap = TRUE; /* defaults to TRUE */

    switch ( f_eHttpAction )
    {
    case eDRM_TEST_NET_LICGET:
        f_pdastrHeader->cchString = DRMCRT_strlen( HTTP_HEADER_LICGET );
        ChkMem( f_pdastrHeader->pszString = ( DRM_CHAR *) Oem_MemAlloc( f_pdastrHeader->cchString + 1 ) );
        ChkDR( TST_UTL_StringCchCopyNA(f_pdastrHeader->pszString, f_pdastrHeader->cchString + 1, HTTP_HEADER_LICGET, f_pdastrHeader->cchString ) );
        break;
    case eDRM_TEST_NET_LICGET_LEGACY:
        f_pdastrHeader->cchString = DRMCRT_strlen( HTTP_HEADER_LEGACY );
        ChkMem( f_pdastrHeader->pszString = ( DRM_CHAR *) Oem_MemAlloc( f_pdastrHeader->cchString + 1 ) );
        ChkDR( TST_UTL_StringCchCopyNA(f_pdastrHeader->pszString, f_pdastrHeader->cchString + 1, HTTP_HEADER_LEGACY, f_pdastrHeader->cchString ) );

        f_pdastrFormat->cchString = DRMCRT_strlen( LEGACY_CHALLENGE_FORMAT );
        ChkMem( f_pdastrFormat->pszString = ( DRM_CHAR *) Oem_MemAlloc( f_pdastrFormat->cchString + 1 ) );
        ChkDR( TST_UTL_StringCchCopyNA(f_pdastrFormat->pszString, f_pdastrFormat->cchString + 1, LEGACY_CHALLENGE_FORMAT, f_pdastrFormat->cchString ) );

        *f_pfUseSoap = FALSE;
        break;
    case eDRM_TEST_NET_METERDATA_LEGACY:
        f_pdastrHeader->cchString = DRMCRT_strlen( HTTP_HEADER_LEGACY );
        ChkMem( f_pdastrHeader->pszString = ( DRM_CHAR *) Oem_MemAlloc( f_pdastrHeader->cchString + 1 ) );
        ChkDR( TST_UTL_StringCchCopyNA(f_pdastrHeader->pszString, f_pdastrHeader->cchString + 1, HTTP_HEADER_LEGACY, f_pdastrHeader->cchString ) );

        f_pdastrFormat->cchString = DRMCRT_strlen( LEGACY_METERCHALLENGE_FORMAT );
        ChkMem( f_pdastrFormat->pszString = ( DRM_CHAR *) Oem_MemAlloc( f_pdastrFormat->cchString + 1 ) );
        ChkDR( TST_UTL_StringCchCopyNA(f_pdastrFormat->pszString, f_pdastrFormat->cchString + 1, LEGACY_METERCHALLENGE_FORMAT, f_pdastrFormat->cchString ) );

        *f_pfUseSoap = FALSE;
        break;
    case eDRM_TEST_NET_LICACK:
        f_pdastrHeader->cchString = DRMCRT_strlen( HTTP_HEADER_LICACK );
        ChkMem( f_pdastrHeader->pszString = ( DRM_CHAR *) Oem_MemAlloc( f_pdastrHeader->cchString + 1 ) );
        ChkDR( TST_UTL_StringCchCopyNA(f_pdastrHeader->pszString, f_pdastrHeader->cchString + 1, HTTP_HEADER_LICACK, f_pdastrHeader->cchString ) );
        break;
    case eDRM_TEST_NET_DOMLEAVE:
        f_pdastrHeader->cchString = DRMCRT_strlen( HTTP_HEADER_LEAVE );
        ChkMem( f_pdastrHeader->pszString = ( DRM_CHAR *) Oem_MemAlloc( f_pdastrHeader->cchString + 1 ) );
        ChkDR( TST_UTL_StringCchCopyNA(f_pdastrHeader->pszString, f_pdastrHeader->cchString + 1, HTTP_HEADER_LEAVE, f_pdastrHeader->cchString ) );
        break;
    case eDRM_TEST_NET_DOMJOIN:
        f_pdastrHeader->cchString = DRMCRT_strlen( HTTP_HEADER_JOIN );
        ChkMem( f_pdastrHeader->pszString = ( DRM_CHAR *) Oem_MemAlloc( f_pdastrHeader->cchString + 1 ) );
        ChkDR( TST_UTL_StringCchCopyNA(f_pdastrHeader->pszString, f_pdastrHeader->cchString + 1, HTTP_HEADER_JOIN, f_pdastrHeader->cchString ) );
        break;
    case eDRM_TEST_NET_METERCERT:
        f_pdastrHeader->cchString = DRMCRT_strlen( HTTP_HEADER_METERCERT );
        ChkMem( f_pdastrHeader->pszString = ( DRM_CHAR *) Oem_MemAlloc( f_pdastrHeader->cchString + 1 ) );
        ChkDR( TST_UTL_StringCchCopyNA(f_pdastrHeader->pszString, f_pdastrHeader->cchString + 1, HTTP_HEADER_METERCERT, f_pdastrHeader->cchString ) );
        break;
    case eDRM_TEST_NET_METERDATA:
        f_pdastrHeader->cchString = DRMCRT_strlen( HTTP_HEADER_METERDATA );
        ChkMem( f_pdastrHeader->pszString = ( DRM_CHAR *) Oem_MemAlloc( f_pdastrHeader->cchString + 1 ) );
        ChkDR( TST_UTL_StringCchCopyNA(f_pdastrHeader->pszString, f_pdastrHeader->cchString + 1, HTTP_HEADER_METERDATA, f_pdastrHeader->cchString ) );
        break;
    case eDRM_TEST_NET_DOWNLOAD:
        *f_pfPost = FALSE;
        *f_pfUseSoap = FALSE;
        break;
    default:
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: _tOEM_SendRequestRedirectable
**
** Synopsis: Function that sends (or resends) the challenge packet to the server.
**           It resend the challenge packet to a new location if the returned
**           server status code is HTTP_STATUS_TEMPORARY_REDIRECT.
**
** Arguments:
**
** [f_hSession]             -- HTTP session handle.
** [f_phConnect]            -- Pointer to a HTTP connection handle.
** [f_phHttpFile]           -- Pointer to a HTTP file handle.
** [f_pbChallenge]          -- Pointer to a buffer that contains the challenge packet.
** [f_cbChallenge]          -- Size of the challenge packet buffer.
** [f_pszHeader]            -- Pointer to a buffer that contains a HTTP header string.
** [f_dwLevel]              -- Level of current HTTP redirection.
** [f_fUseSOAP]             -- Flag indicating whether to use SOAP to send
**                             the challenge packet.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL if any HTTP error happens or too many
**                          redirections have been tried.
**
**********************************************************************/
static DRM_RESULT DRM_CALL _tOEM_SendRequestRedirectable(
    IN HINTERNET       f_hSession,
    IN OUT HINTERNET  *f_phConnect,
    IN OUT HINTERNET  *f_phHttpFile,
    IN URL_COMPONENTSA f_oUrlComponents,
    IN DRM_BYTE *f_pbChallenge,
    IN DRM_DWORD f_cbChallenge,
    IN DRM_CHAR *f_pszHeader,
    IN DRM_DWORD f_dwLevel,
    IN DRM_BOOL  f_fPost
 )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR rgbQuery[ MAX_URL_SIZE ] = { 0 };
    DRM_DWORD cbQuery = 0;
    DRM_CHAR rgbHostName[ MAX_URL_SIZE ] = { 0 };
    DRM_CHAR rgbUrlPath[ MAX_URL_SIZE ] = { 0 };
    int      retries  = 0;

    ChkArg( f_phConnect != NULL );
    ChkArg( f_phHttpFile != NULL );
    ChkArg( f_dwLevel <= MAX_REDIRECTIONS_PER_REQUEST );
    ChkArg( ( f_dwLevel == 0 && *f_phConnect == NULL && *f_phHttpFile == NULL ) ||
            ( f_dwLevel > 0 && *f_phConnect != NULL && *f_phHttpFile != NULL ) );

    if ( f_dwLevel > 0 )
    {
        cbQuery = SIZEOF( rgbQuery );

        /* Retrieve the returned status code. */
        ChkWR( HttpQueryInfoA( *f_phHttpFile,
                                HTTP_QUERY_STATUS_CODE,
                                rgbQuery,
                                (LPDWORD)&cbQuery,
                                NULL ) );

        /* Check if the status code is HTTP_STATUS_TEMPORARY_REDIRECT. */
        if ( atoi( rgbQuery ) == HTTP_STATUS_TEMPORARY_REDIRECT )
        {
            cbQuery = SIZEOF( rgbQuery );

            /* Retrieve the new location (URL) to be redirected. */
            ChkWR( HttpQueryInfoA( *f_phHttpFile,
                                    HTTP_QUERY_LOCATION ,
                                    rgbQuery,
                                    (LPDWORD)&cbQuery,
                                    NULL ) );

            ZEROMEM( &f_oUrlComponents, SIZEOF( URL_COMPONENTSA ) );

            f_oUrlComponents.dwStructSize = SIZEOF( URL_COMPONENTSA );
            f_oUrlComponents.lpszHostName = rgbHostName;
            f_oUrlComponents.dwHostNameLength = SIZEOF( rgbHostName );
            f_oUrlComponents.lpszUrlPath = rgbUrlPath;
            f_oUrlComponents.dwUrlPathLength = SIZEOF( rgbUrlPath );

            /* Parse the full redirected URL string into components. */
            ChkWR( InternetCrackUrlA( rgbQuery,
                                      0,
                                      0,
                                     &f_oUrlComponents ) );
        }
        else
        {
            /* The error status code is something else, exit the function. */
            Log("", "\n\nHttp Status Returned is:%d", atoi( rgbQuery ) );
            ChkDR( DRM_E_FAIL );
        }
    }
    else
    {
        Log( "", "\n\nSending HTTP request to:\n\tHost:%s\n\tPort:%d\n\t Url:%s\n\n", f_oUrlComponents.lpszHostName, f_oUrlComponents.nPort, f_oUrlComponents.lpszUrlPath );
    }

    /* Close the current connection and file handles if opened. */
    if ( *f_phHttpFile != NULL )
    {
        InternetCloseHandle( *f_phHttpFile );
    }

    if ( *f_phConnect != NULL )
    {
        InternetCloseHandle( *f_phConnect );
    }

    *f_phConnect = InternetConnectA( f_hSession,
                                     f_oUrlComponents.lpszHostName,
                                     f_oUrlComponents.nPort,
                                     NULL,
                                     NULL,
                                     INTERNET_SERVICE_HTTP,
                                     0,
                                     0 );

    ChkWR( *f_phConnect != NULL );

    *f_phHttpFile = HttpOpenRequestA( *f_phConnect,
                                      f_fPost? "POST" : "GET",
                                      f_oUrlComponents.lpszUrlPath,
                                      NULL,
                                      NULL,
                                      0,
                                      INTERNET_FLAG_DONT_CACHE,
                                      0 );

    ChkWR( *f_phHttpFile != NULL );

    if ( f_pszHeader != NULL )
    {

        /* Add the customized HTTP header to the HTTP request. */
        ChkWR( HttpAddRequestHeadersA( *f_phHttpFile,
                                        f_pszHeader,
                                        ( DRM_DWORD )DRMCRT_strlen( f_pszHeader ),
                                        HTTP_ADDREQ_FLAG_ADD_IF_NEW ) );
    }
    /* Send the HTTP request to the server. */
    for (retries = 0; retries < MAX_HTTP_RETRIES; ++retries)
    {
        if (HttpSendRequestA(*f_phHttpFile,
                             NULL,
                             0,
                             f_pbChallenge,
                             f_cbChallenge ))
        {
            break;
        }
    }

    if (MAX_HTTP_RETRIES == retries)
    {
        DRM_RESULT drInner = DRM_RESULT_FROM_WIN32( 0/*GESTATE*/ );

        if ( drInner == DRM_RESULT_FROM_WIN32( ERROR_WINHTTP_TIMEOUT ) )
        {
            Log( "", "SendRequest timed out");
            ChkDR( drInner );
        }
        else
        {
            /*
            ** The server returns an error, recursively checks whether
            ** redirection is needed.
            */
            dr = _tOEM_SendRequestRedirectable( f_hSession,
                                                f_phConnect,
                                                f_phHttpFile,
                                                f_oUrlComponents,
                                                f_pbChallenge,
                                                f_cbChallenge,
                                                f_pszHeader,
                                                f_dwLevel + 1,
                                                f_fPost );
            if( DRM_E_FAIL == dr )
            {
                /*
                ** DRM_E_FAIL is returned when the HTTP status isn't redirect.
                ** In this casse we need to check the Win32 error.
                */
                ChkDR( drInner );
            }
            ChkDR( dr );
        }
    }

ErrorExit:

    return dr;
}

/*******************************************************************************
** Function:    tOEM_SendNetDataDSTR
**
** Synopsis:    Sends challenge data to the server and url given in f_inetInfo.
**              The result is stored in f_ppbResponse.
**
**              [f_pdstrURL]     DRM_STRING holding the URL.
**              [f_eHttpAction]  DRM_TEST_NET_ACTION flag that says for which
**                               PlayReady RM action to send and receive data.
**              [f_pbChallenge]  Pointer to a buffer containing the challenge
**                               data.
**              [f_cbChallenge]  Holds the number of bytes in the challenge.
**              [f_ppbResponse]  pointer to a pointer to a buffer for the output
**                               data. The buffer will be alloced and should
**                               be freed by the user.
**              [f_pcbResponse]  On output, holds the number of bytes written
**                               to f_ppbResponse.
**
** Returns:
**              DRM_E_TEST_INVALIDARG  Any pointer argument is NULL.
**              DRM_E_FAIL             Any of the Network operations failed.
**
**
** Notes:       The caller should be careful to free f_ppbResonse after
**              use.
**              This version of tOEM_SendNetData takes a Wide URL and demotes it.
*******************************************************************************/
DRM_RESULT DRM_CALL tOEM_SendNetDataDSTR(
    __in                                 DRM_CONST_STRING     *f_pdstrURL,
    __in                                 eDRM_TEST_NET_ACTION  f_eAction,
    __in_bcount( f_cbChallenge )         DRM_BYTE             *f_pbChallenge,
    __in                                 DRM_DWORD             f_cbChallenge,
    __deref_out_bcount( *f_pcbResponse ) DRM_BYTE            **f_ppbResponse,
    __out_ecount( 1 )                    DRM_DWORD            *f_pcbResponse)
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_ANSI_STRING dstrUTF8URL   = EMPTY_DRM_STRING;

    ChkDRMString( f_pdstrURL );
    ChkArg( f_eAction     != eDRM_TEST_NET_UNKNOWN );
    ChkArg( f_ppbResponse != NULL );
    ChkArg( f_pcbResponse != NULL );

    dr = DRM_STR_UTF16toUTF8( f_pdstrURL->pwszString, f_pdstrURL->cchString, NULL, 0, &dstrUTF8URL.cchString );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, dr );

    ChkMem( dstrUTF8URL.pszString = ( DRM_CHAR * )Oem_MemAlloc( dstrUTF8URL.cchString ) );

    /* Convert the UTF-16 string into UTF8/ANSI before sending to the server. */
     ChkDR( DRM_STR_UTF16toUTF8( f_pdstrURL->pwszString,
                                 f_pdstrURL->cchString,
                                 dstrUTF8URL.pszString,
                                 0,
                               &dstrUTF8URL.cchString ) );



    ChkDR( tOEM_SendNetData( dstrUTF8URL.pszString,
                             f_eAction,
                             f_pbChallenge,
                             f_cbChallenge,
                             f_ppbResponse,
                             f_pcbResponse ) );

ErrorExit:
    SAFE_OEM_FREE( dstrUTF8URL.pszString );
    return dr;
}


/*******************************************************************************
** Function:    tOEM_SendNetData
**
** Synopsis:    Sends challenge data to the server and url given in f_inetInfo.
**              The result is stored in f_ppbResponse.
**
**              [f_pszURL]       Null Terminated ANSI character string containing
**                               the URL to which to post data.
**              [f_eHttpAction]  DRM_TEST_NET_ACTION flag that says for which
**                               PlayReady RM action to send and receive data.
**              [f_pbChallenge]  Pointer to a buffer containing the challenge
**                               data.
**              [f_cbChallenge]  Holds the number of bytes in the challenge.
**              [f_ppbResponse]  pointer to a pointer to a buffer for the output
**                               data. The buffer will be alloced and should
**                               be freed by the user.
**              [f_pcbResponse]  On output, holds the number of bytes written
**                               to f_ppbResponse.
**
** Returns:
**              DRM_E_TEST_INVALIDARG  Any pointer argument is NULL.
**              DRM_E_FAIL             Any of the Network operations failed.
**
**
** Notes:       The caller should be careful to free f_ppbResonse after
**              use.
*******************************************************************************/
DRM_RESULT DRM_CALL tOEM_SendNetData(
    __in_z                               DRM_CHAR             *f_pszUrl,
    __in                                 eDRM_TEST_NET_ACTION  f_eAction,
    __in_bcount( f_cbChallenge )         DRM_BYTE             *f_pbChallenge,
    __in                                 DRM_DWORD             f_cbChallenge,
    __deref_out_bcount( *f_pcbResponse ) DRM_BYTE            **f_ppbResponse,
    __out_ecount( 1 )                    DRM_DWORD            *f_pcbResponse)
{
    DRM_RESULT dr = DRM_SUCCESS;
    HINTERNET hSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hHttpFile = NULL;
    URL_COMPONENTSA oComponents = {0};
    DRM_BYTE rgbHeaderBuffer[ MAX_HTTP_HEADER_SIZE ] = { 0 };
    DRM_DWORD cbHeaderBuffer = MAX_HTTP_HEADER_SIZE;
    DRM_BYTE *pbCurrent = NULL;
    DRM_DWORD cbCurrent = 0, cbRead = 0;
    DRM_ANSI_STRING dastrHeader = EMPTY_DRM_STRING;
    DRM_ANSI_STRING dastrFormat = EMPTY_DRM_STRING;
    DRM_BYTE  *pbFormattedChallenge  = NULL;
    DRM_DWORD cbFormattedChallenge   = 0;
    DRM_BOOL fPost = TRUE;
    DRM_BOOL fUseSoap = TRUE;

    ChkArg( f_pszUrl      != NULL );
    ChkArg( f_eAction     != eDRM_TEST_NET_UNKNOWN );
    ChkArg( f_ppbResponse != NULL );
    ChkArg( f_pcbResponse != NULL );

    ChkDR( _CrackAction( f_eAction, &dastrHeader, &fPost, &fUseSoap, &dastrFormat ) );

    if ( fPost )
    {
        ChkArg( f_pbChallenge != NULL );
    }
    if ( !fUseSoap )
    {
        /* Legacy Challenges need a prefix. */
        ChkOverflow( f_cbChallenge + 1, f_cbChallenge );
        cbFormattedChallenge = f_cbChallenge + 1 + dastrFormat.cchString;
        ChkOverflow( cbFormattedChallenge, f_cbChallenge + 1 );
        ChkMem( pbFormattedChallenge = (DRM_BYTE*)Oem_MemAlloc(cbFormattedChallenge) );
        MEMCPY( pbFormattedChallenge, dastrFormat.pszString, dastrFormat.cchString );
        MEMCPY( pbFormattedChallenge + dastrFormat.cchString, f_pbChallenge, f_cbChallenge );
        PUT_CHAR(pbFormattedChallenge , cbFormattedChallenge - 1, '\0');
    }
    else
    {
        pbFormattedChallenge  = f_pbChallenge;
        cbFormattedChallenge  = f_cbChallenge;
    }

    oComponents.dwStructSize = SIZEOF( URL_COMPONENTSA);
    oComponents.dwHostNameLength = MAX_URL_SIZE;
    ChkMem( oComponents.lpszHostName = ( DRM_CHAR * ) Oem_MemAlloc( oComponents.dwHostNameLength ));
    oComponents.dwUrlPathLength = MAX_URL_SIZE;
    ChkMem( oComponents.lpszUrlPath = ( DRM_CHAR * ) Oem_MemAlloc( oComponents.dwUrlPathLength ));

    ChkBOOL( InternetCrackUrlA( f_pszUrl, 0, 0, &oComponents ), DRM_E_FAIL);

    Log( "tOEM_SendNetData", "ServerName: %s", oComponents.lpszHostName );
    Log( "tOEM_SendNetData", "Path: %s", oComponents.lpszUrlPath );
    Log( "tOEM_SendNetData", "Header: %s", dastrHeader.pszString );

    /* Use proxy settings configured in the Internet Explorer. */
    hSession = InternetOpenA( AGENT_NAME,
                              PRE_CONFIG_INTERNET_ACCESS,
                              "",
                              INTERNET_INVALID_PORT_NUMBER,
                              0 );

    ChkWR( hSession != NULL );

    /*
    ** Send the challenge to the server and handle possible
    ** HTTP redirection(s).
    */
    ChkDR( _tOEM_SendRequestRedirectable( hSession,
                                 &hConnect,
                                 &hHttpFile,
                                 oComponents,
                                 pbFormattedChallenge,
                                 cbFormattedChallenge,
                                 dastrHeader.pszString,
                                 0,
                                 fPost ) );

    ChkWR( HttpQueryInfoA( hHttpFile,
                             HTTP_QUERY_RAW_HEADERS_CRLF,
                             rgbHeaderBuffer,
                             (LPDWORD)&cbHeaderBuffer,
                             NULL ) );

    Log("tOEM_SendNetData", "Response Header:%s\n",rgbHeaderBuffer);

    /*
    ** Query for the content length from the HTTP header of the
    ** server respinse. Content length indicates the size of
    ** HTTP data that follows the HTTP header in the server response.
    */
    cbHeaderBuffer = MAX_HTTP_HEADER_SIZE;
    ChkWR( HttpQueryInfoA( hHttpFile,
                             HTTP_QUERY_CONTENT_LENGTH,
                             rgbHeaderBuffer,
                             (LPDWORD)&cbHeaderBuffer,
                             NULL ) );

    Log("tOEM_SendNetData", "Response Content Length:%s\n",rgbHeaderBuffer);

    /* Convert the size of HTTP data into an integer. */
    *f_pcbResponse = atol( ( DRM_CHAR * )rgbHeaderBuffer );

    ChkMem( *f_ppbResponse = ( DRM_BYTE * )Oem_MemAlloc( *f_pcbResponse ) );

    ZEROMEM( *f_ppbResponse, *f_pcbResponse );

    pbCurrent = *f_ppbResponse;
    cbCurrent = *f_pcbResponse;

    /*
    ** Read the HTTP data of the server response into the
    ** buffer just allocated.
    */
    while ( cbCurrent > 0 )
    {
        ChkWR( InternetReadFile( hHttpFile,
                                   pbCurrent,
                                   cbCurrent,
                                   (LPDWORD)&cbRead ) );

        pbCurrent += cbRead;

        cbCurrent -= cbRead;
    }

ErrorExit:

    if ( hHttpFile != NULL )
    {
        InternetCloseHandle( hHttpFile );
    }

    if ( hConnect != NULL )
    {
        InternetCloseHandle( hConnect );
    }

    if ( hSession != NULL )
    {
        InternetCloseHandle( hSession );
    }
    if ( f_eAction == eDRM_TEST_NET_LICGET_LEGACY )
    {
        SAFE_OEM_FREE( pbFormattedChallenge );
    }
    SAFE_OEM_FREE( oComponents.lpszHostName );
    SAFE_OEM_FREE( oComponents.lpszUrlPath );
    SAFE_OEM_FREE( dastrHeader.pszString );
    SAFE_OEM_FREE( dastrFormat.pszString );
    return dr;
}

/*******************************************************************************
** Function:    tOEM_NetSaveFileDSTR
**
** Synopsis:    This downloads the URL provided to the filename given.
**
**              [f_pdstrURL]     DRM_STRING containing the URL from which to download.
**              [f_pdstrFile]    Output file.
**              [f_fnProgress]   Optional progress callback, currently ignored.
**
** Returns:
**              DRM_E_TEST_INVALIDARG  Any pointer argument is NULL.
**              DRM_E_FAIL             Any of the Network operations failed.
**
**
*******************************************************************************/
DRM_RESULT DRM_CALL tOEM_NetSaveFileDSTR(
    __in                                 DRM_CONST_STRING     *f_pdstrURL,
    __in                                 DRM_CONST_STRING     *f_pdstrFile,
    __out_opt                            PFNPROGRESSCALLBACK   f_fnProgress)
{
    DRM_RESULT dr     = DRM_SUCCESS;
    DRM_ANSI_STRING dstrUTF8URL   = EMPTY_DRM_STRING;

    ChkDRMString( f_pdstrURL );
    ChkDRMString( f_pdstrFile );

    dr = DRM_STR_UTF16toUTF8( f_pdstrURL->pwszString, f_pdstrURL->cchString, NULL, 0, &dstrUTF8URL.cchString );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, dr );

    ChkMem( dstrUTF8URL.pszString = ( DRM_CHAR * )Oem_MemAlloc( dstrUTF8URL.cchString ) );

    /* Convert the UTF-16 string into UTF8/ANSI before sending to the server. */
     ChkDR( DRM_STR_UTF16toUTF8( f_pdstrURL->pwszString,
                                 f_pdstrURL->cchString,
                                 dstrUTF8URL.pszString,
                                 0,
                               &dstrUTF8URL.cchString ) );



    ChkDR( tOEM_NetSaveFile( dstrUTF8URL.pszString,
                             f_pdstrFile,
                             f_fnProgress ) );

ErrorExit:
    SAFE_OEM_FREE( dstrUTF8URL.pszString );
    return dr;
}

/*******************************************************************************
** Function:    tOEM_NetSaveFile
**
** Synopsis:    This downloads the URL provided to the filename given.
**
**              [f_pszUrl]       Null terminated string containing the URL from
**                               which to download.
**              [f_pdstrFile]    Output file.
**              [f_fnProgress]   Optional progress callback, currently ignored.
**
** Returns:
**              DRM_E_TEST_INVALIDARG  Any pointer argument is NULL.
**              DRM_E_FAIL             Any of the Network operations failed.
**
**
*******************************************************************************/
DRM_RESULT DRM_CALL tOEM_NetSaveFile(
    __in_z                               DRM_CHAR             *f_pszUrl,
    __in                                 DRM_CONST_STRING     *f_pdstrFile,
    __out_opt                            PFNPROGRESSCALLBACK   f_fnProgress)

{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE  *pbContent = NULL;
    DRM_DWORD  cbContent = 0;

    ChkDRMString( f_pdstrFile );
    ChkArg( f_pszUrl != NULL );

    ChkDR( tOEM_SendNetData( f_pszUrl,
                             eDRM_TEST_NET_DOWNLOAD,
                             NULL,
                             0,
                            &pbContent,
                            &cbContent ) );

    ChkDR( SaveToFileW( f_pdstrFile->pwszString, pbContent, cbContent ) );
ErrorExit:
    SAFE_OEM_FREE( pbContent );
    return dr;
}

#else  /* DRM_TEST_SUPPORT_NET_IO */

DRM_RESULT DRM_CALL tOEM_SendNetDataDSTR(
    __in                                 DRM_CONST_STRING     *f_pdstrURL,
    __in                                 eDRM_TEST_NET_ACTION  f_eAction,
    __in_bcount( f_cbChallenge )         DRM_BYTE             *f_pbChallenge,
    __in                                 DRM_DWORD             f_cbChallenge,
    __deref_out_bcount( *f_pcbResponse ) DRM_BYTE            **f_ppbResponse,
    __out_ecount( 1 )                    DRM_DWORD            *f_pcbResponse )
{
    UNREFERENCED_PARAMETER( f_pdstrURL    );
    UNREFERENCED_PARAMETER( f_eAction     );
    UNREFERENCED_PARAMETER( f_pbChallenge );
    UNREFERENCED_PARAMETER( f_cbChallenge );
    UNREFERENCED_PARAMETER( f_ppbResponse );
    UNREFERENCED_PARAMETER( f_pcbResponse );
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_CALL tOEM_SendNetData(
    __in_z                               DRM_CHAR             *f_pszUrl,
    __in                                 eDRM_TEST_NET_ACTION  f_eAction,
    __in_bcount( f_cbChallenge )         DRM_BYTE             *f_pbChallenge,
    __in                                 DRM_DWORD             f_cbChallenge,
    __deref_out_bcount( *f_pcbResponse ) DRM_BYTE            **f_ppbResponse,
    __out_ecount( 1 )                    DRM_DWORD            *f_pcbResponse )
{
    UNREFERENCED_PARAMETER( f_pszUrl      );
    UNREFERENCED_PARAMETER( f_eAction     );
    UNREFERENCED_PARAMETER( f_pbChallenge );
    UNREFERENCED_PARAMETER( f_cbChallenge );
    UNREFERENCED_PARAMETER( f_ppbResponse );
    UNREFERENCED_PARAMETER( f_pcbResponse );
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_CALL tOEM_NetSaveFile(
    __in_z                               DRM_CHAR             *f_pszUrl,
    __in                                 DRM_CONST_STRING     *f_pdstrFile,
    __out_opt                            PFNPROGRESSCALLBACK   f_fnProgress )
{
    UNREFERENCED_PARAMETER( f_pszUrl     );
    UNREFERENCED_PARAMETER( f_pdstrFile  );
    UNREFERENCED_PARAMETER( f_fnProgress );
    return DRM_E_NOTIMPL;
}

DRM_RESULT DRM_CALL tOEM_NetSaveFileDSTR(
    __in                                 DRM_CONST_STRING     *f_pdstrURL,
    __in                                 DRM_CONST_STRING     *f_pdstrFile,
    __out_opt                            PFNPROGRESSCALLBACK   f_fnProgress )
{
    UNREFERENCED_PARAMETER( f_pdstrURL   );
    UNREFERENCED_PARAMETER( f_pdstrFile  );
    UNREFERENCED_PARAMETER( f_fnProgress );
    return DRM_E_NOTIMPL;
}

#endif /* DRM_TEST_SUPPORT_NET_IO */

EXIT_PKTEST_NAMESPACE_CODE

