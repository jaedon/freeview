/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <drmwindowsenv.h>
#include <wininet.h>

#include <drmfeatures.h>
#include <drmcommon.h>
#include <drmtypes.h>
#include <drmutilities.h>
#include <drmmanager.h>
#include <drmdomainapi.h>
#include <drmcmdlnpars.h>
#include <drmtoolsutils.h>
#include <drmtoolsmediafile.h>
#include <drmheaderparser.h>
#include <drmutf.h>
#include <drmsoapxmlutility.h>
#include <drmmeterapi.h>
#include <drmdla.h>

#include <drmtrace.h>
#include <drmtoolsinit.h>

#include <stdio.h>
#include <stdlib.h>

#if !TARGET_LITTLE_ENDIAN
#error OTADLAClient.c cannot compile for Big Endian systems.
#endif

#pragma comment( lib, "wininet.lib" )

#pragma warning ( disable: 6031 )

/* Name of the HDS file. */
#define HDS_STORE_FILE  L".\\playready.hds"

/*
** The full path of the media player application. It should be changed
** to reflect the local setting.
*/
#define PLAYER_PATH_STRING          L"C:\\Program Files\\Windows Media Player\\wmplayer.exe"

#define CHALLENGE_STRING            L"Challenge"
#define RESPONSE_STRING             L"Response"
#define DATEXTENSION_STRING         L".dat"

#define SPACE_STRING                L" "
#define BACKSLASH_STRING            L"\\"

#define MAX_COMMAND_LINE_SIZE       1024
#define MAX_PLAY_BUFFER_SIZE        4096
#define MAX_TRANSACTION_ID_SIZE     128
#define MAX_URL_SIZE                1024
#define MAX_HTTP_HEADER_SIZE        4096
#define MAX_CUSTOM_DATA_SIZE        1024
#define MAX_SCRATCH_SIZE            4096
#define MAX_LIC_ACKS                100
#define MAX_REDIRECTIONS_PER_REQUEST    5
#define MAX_HTTP_SERVER_NAME_LEN    50
#define MAX_HTTP_URL_LEN            100
#define MAX_NUM_CUSTOM_DATA         20

/*
** Flag indicating whether to demonstrate the domain exclusion scenario
** during license acquisition.
*/
#define DEMO_DOMAIN_EXCLUSION       0

/*
** Flag indicating whether to launch a player after an encrypted content
** file is successfully decrypted.
*/
#define DEMO_LAUNCH_PLAYER          0

/* Flag indicating whether to requested a chained license from the server (XMR only). */
#define DEMO_CHAINED_LICENSE        1

/* Flag indicating whether to to request a Silverlight license. */
#define DEMO_SILVERLIGHT_LICENSE    0

/* Flag indicating whether to log packets between server and the client. */
#define LOG_PACKETS                 0

/*
** Server name (DNS name or IP address) of the server hosting the web services.
** It should be changed to reflect the actual server deployment.
*/
#define HTTP_SERVER_NAME            "playready.directtaps.net"
#define HTTP_SERVER_PORT            80

/*
** Fallback license/metering server name (DNS name or IP address) of the server hosting
** the WMDRM-PD license/metering service. It should be changed to reflect the actual server
** deployment. Fallback server license server is used to demonstrate the license
** protocol fallback scenario, as well as to process the meter data challenge.
*/
#define HTTP_FALLBACK_SERVER_NAME   "wmtap.smdisp.net/Janus"
#define HTTP_FALLBACK_SERVER_PORT   80

/*
** ASP page that implements the WMDRM-PD licenseservice. This should be changed
** according to the actual server deployment. This page is hosted on the fallback server.
*/
#define HTTP_FALLBACK_LICENSE_PAGE      "/wm/simple.asp"
#define HTTP_FALLBACK_LICENSE_HEADER    "Content-Type: application/x-www-form-urlencoded\r\n"

/*
** ASP page that implements the WMDRM-PD metering service. This should be changed
** according to the actual server deployment. This page is hosted on the fallback server.
*/
#define HTTP_FALLBACK_METERING_PAGE     "/wm/metering.asp"
#define HTTP_FALLBACK_METERING_HEADER   "Content-Type: application/x-www-form-urlencoded\r\n"

/*
** Virtual directories of the web services should be changed
** according to the actual server deployment.
*/
#define HTTP_WEB_SERVICE_ROOT       "/pr/svc/rightsmanager.asmx"

#define HTTP_HEADER_JOIN   "Content-Type: text/xml; charset=utf-8\r\nSOAPAction: \"http://schemas.microsoft.com/DRM/2007/03/protocols/JoinDomain\"\r\n"
#define HTTP_HEADER_LEAVE  "Content-Type: text/xml; charset=utf-8\r\nSOAPAction: \"http://schemas.microsoft.com/DRM/2007/03/protocols/LeaveDomain\"\r\n"
#define HTTP_HEADER_LICGET "Content-Type: text/xml; charset=utf-8\r\nSOAPAction: \"http://schemas.microsoft.com/DRM/2007/03/protocols/AcquireLicense\"\r\n"
#define HTTP_HEADER_LICACK "Content-Type: text/xml; charset=utf-8\r\nSOAPAction: \"http://schemas.microsoft.com/DRM/2007/03/protocols/AcknowledgeLicense\"\r\n"
#define HTTP_HEADER_METERCERT   "Content-Type: text/xml; charset=utf-8\r\nSOAPAction: \"http://schemas.microsoft.com/DRM/2007/03/protocols/GetMeteringCertificate\"\r\n"
#define HTTP_HEADER_METERDATA   "Content-Type: text/xml; charset=utf-8\r\nSOAPAction: \"http://schemas.microsoft.com/DRM/2007/03/protocols/ProcessMeteringData\"\r\n"

/* HTTP status code of temporary redirection. */
#define HTTP_STATUS_TEMPORARY_REDIRECT  307

/* V2 license challenge prefix string. */
#define V2_LICENSE_CHALLENGE_PREFIX     "challenge="

/* V2 meter data challenge prefix string. */
#define V2_METERDATA_CHALLENGE_PREFIX   "meterchallenge="

/* URL to download revocation package from */
#define HTTP_REVOCATION_PACKAGE_PAGE "/fwlink/?LinkId=110086"
#define HTTP_REVOCATION_PACKAGE_SERVER "go.microsoft.com"

const DRM_WCHAR g_rgwchExecuteRight[] = { ONE_WCHAR('6','\0'), ONE_WCHAR('3','\0'), ONE_WCHAR('\0','\0') };

const DRM_CONST_STRING g_dstrExecuteRight = CREATE_DRM_STRING( g_rgwchExecuteRight );

/* Parameters of HTTP connection. */
typedef struct __tagOTADLA_HttpParams
{
    /* Name (or IP address) of the server. */
    DRM_CHAR m_rgchServerName[ MAX_HTTP_SERVER_NAME_LEN ];

    /* TCP port of the server. */
    DRM_DWORD m_dwHttpPort;

    /* URL of the server. */
    DRM_CHAR m_rgchURL[ MAX_HTTP_URL_LEN ];

} OTADLA_HttpParams;

/* Type of states in the state machine. */
typedef enum
{
    /* "Initial" state. */
    OTADLA_ENTER = 0,

    /* "Init DRM" state. */
    OTADLA_INITDRM,

    /* "Play Music" state. */
    OTADLA_PLAYMUSIC,

    /* "Get License" state. */
    OTADLA_GETLICENSE,

    /* "Join Domain" state. */
    OTADLA_JOINDOMAIN,

    /* "Leave Domain" state. */
    OTADLA_LEAVEDOMAIN,

    /* "Report metering" state. */
    OTADLA_REPORTMETERING,

    /* "Update revocation" state. */
    OTADLA_UPDATEREVOCATION,

    /* "Deinit DRM" state. */
    OTADLA_DEINITDRM,

    /* "Final" state. */
    OTADLA_EXIT,

    /* Number of states, not a valid state in itself */
    OTADLA_MAX_STATES

} OTADLA_PROCESS_STATE;

/* Type of web services. */
typedef enum
{
    OTADLA_NET_JOINDOMAIN = 0,

    OTADLA_NET_LEAVEDOMAIN = 1,

    OTADLA_NET_GETLICENSE = 2,

    OTADLA_NET_ACKLICENSE = 3,

    OTADLA_NET_METERCERT = 4,

    OTADLA_NET_METERDATA = 5,

    OTADLA_NET_REVOCATIONPACKAGE = 6

} OTADLA_NET_COMMAND_TYPE;

/* State machine context. */
typedef struct __tagOTADLA_STATE
{
    /* Current state. */
    OTADLA_PROCESS_STATE m_State;

    /* Previous state. */
    OTADLA_PROCESS_STATE m_PrevState;

    /*
    ** Flag indicating whether a domain membership
    ** is needed to get a license.
    */
    DRM_BOOL m_fNeedDomain;

    /*
    ** Flag indicating a special mode that only joins
    ** the domain and then exits.
    */
    DRM_BOOL m_fJoinDomainOnly;

    /*
    ** Flag indicating a special mode that only leaves
    ** the domain and then exits.
    */
    DRM_BOOL m_fLeaveDomainOnly;

    /*
    ** Flag indicating whether the application has
    ** joined a domain.
    */
    DRM_BOOL m_fInDomain;

    /*
    ** Flag indicating whether or not we should use the
    ** URL from the file. Defaults to TRUE.
    */
    DRM_BOOL m_fUseFileUrl;

    /* Buffer to receive the parsed out URL. */
    DRM_CHAR m_rgchURL[ MAX_URL_SIZE ];

    /*
    ** Variable that contains the size of the URL buffer
    ** during input and receives the size of the URL buffer
    ** during output.
    */
    DRM_DWORD m_cchURL;

    /*
    ** Pointer to a DRM string that contains the name of an
    ** input file to be decrypted.
    */
    DRM_CONST_STRING *m_pdstrEncryptedFile;

    /*
    ** Pointer to the media file context in use as the input file.
    */
    DRM_MEDIA_FILE_CONTEXT *m_poMediaFile;

    /*
    ** Pointer to a DRM string that contains the name of an
    ** decrypted output file.
    */
    DRM_CONST_STRING *m_pdstrDecryptedFile;

    /*
    ** Pointer to a buffer used to process the revocation
    ** information during license acquisition.
    */
    DRM_BYTE *m_pbRevocationBuffer;

    /*
    ** Flag inficating whether the server should return
    ** a "domain excluded" error during license acquisition.
    */
    DRM_BOOL m_fDomainExcluded;

    /*
    ** Flag indicating whether the encrypted package is created
    ** using AES cipher. If it is FALSE the package is using RC4
    ** cipher.
    */
    DRM_BOOL m_fAESPackage;

    /*
    ** Flag indicating if source file is an envelope file.
    */
    DRM_BOOL m_fEnvelopeFile;

    /*
    ** Flag indicating whether to demonstrate the license protocol
    ** fallback scenario.
    */
    DRM_BOOL m_fProtocolFallback;

    /*
    ** The version of the license protocol to use.
    */
    DRM_LICENSE_PROTOCOL_TYPE m_ProtocolVersion;

    /*
    ** Domain ID that contains a service ID retrieved from the server
    ** error message during license acquisition.
    */
    DRM_DOMAIN_ID m_oDomainID;

    /*
    ** Flag indicating whether the input file contains only the RM header.
    */
    DRM_BOOL m_fHeaderOnly;

    /*
    ** Flog indicating that the Execute Right should be used.
    */
    DRM_BOOL m_fExecute;

    /*
    ** HDS filename. If this is NULL, the default HDS is used.
    */
    DRM_CONST_STRING *m_pdstrHDSFileName;

} OTADLA_STATE;

/* String lookup table for states. */
const DRM_CHAR *g_StateNames[OTADLA_MAX_STATES] = { "Enter", "Init DRM", "Play Music", "Get License",
                                   "Join Domain", "Leave Domain", "Report Metering",
                                   "Update Revocation", "Deinit DRM", "Exit" };

/* Default HTTP connection parameters for RM server. */
OTADLA_HttpParams g_oServer = { HTTP_SERVER_NAME, HTTP_SERVER_PORT, HTTP_WEB_SERVICE_ROOT };

/* Default HTTP connection parameters for Fallback server. */
OTADLA_HttpParams g_oFallback = { HTTP_FALLBACK_SERVER_NAME,
                                  HTTP_FALLBACK_SERVER_PORT,
                                  HTTP_FALLBACK_LICENSE_PAGE };

/* For storing custom data string. */
DWORD g_dwCustomData = 0;
DRM_ANSI_STRING g_customData[ MAX_NUM_CUSTOM_DATA ];

/* Special characters that needs to be HTML encoded. This array is used by HTMLEncode() function. */
DRM_CHAR g_rgchHTMLEncode[] = { ' ', '+' };

/* Packet counts used as part of the file name of the packet logs. */
DRM_DWORD g_dwChallengePacketCount = 1;
DRM_DWORD g_dwResponsePacketCount = 1;

/*******************************************************************************
**
** Function:    IsV2MeterCert
**
** Synopsis:    Function that checks whether the passed in mertering certificate
**              is a V2 metering certificate.
** Arguments:
**
** [f_pbCert]               -- Pointer to buffer that contains the metering certificate.
** [f_cbCert]               -- Size (number of bytes) of the buffer.
** [f_pfIsV2]               -- Pointer to a variable to receive the result.
**
** Returns:
**                          DRM_SUCCESS  The function completed successfully.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
*******************************************************************************/
static DRM_RESULT IsV2MeterCert(
    __in_bcount( f_cbCert ) const DRM_BYTE *f_pbCert,
    __in                          DRM_DWORD f_cbCert,
    __out                         DRM_BOOL *f_pfIsV2 )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WCHAR rgwchTagMeterCert[] = { ONE_WCHAR('<', '\0'), ONE_WCHAR('M', '\0'), ONE_WCHAR('E', '\0'), ONE_WCHAR('T', '\0'),
                                      ONE_WCHAR('E', '\0'), ONE_WCHAR('R', '\0'), ONE_WCHAR('C', '\0'), ONE_WCHAR('E', '\0'),
                                      ONE_WCHAR('R', '\0'), ONE_WCHAR('T', '\0'), ONE_WCHAR('\0', '\0') };
    DRM_DWORD cbTagMeterCert = 0;

    ChkArg( f_pbCert != NULL );
    ChkArg( f_cbCert > 0 );
    ChkArg( f_pfIsV2 != NULL );

    cbTagMeterCert = SIZEOF( rgwchTagMeterCert ) - SIZEOF( WCHAR );

    if ( f_cbCert > cbTagMeterCert )
    {
        *f_pfIsV2 = ( MEMCMP( f_pbCert,
                              rgwchTagMeterCert,
                              cbTagMeterCert ) == 0 );
    }
    else
    {
        *f_pfIsV2 = FALSE;
    }

ErrorExit:

    return dr;
}

/*******************************************************************************
**
** Function:    HTMLEncode
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
static DRM_RESULT DRM_CALL HTMLEncode(
    IN  const DRM_CHAR  *f_rgchIn,
    IN        DRM_DWORD  f_cchIn,
    OUT       DRM_CHAR  *f_rgchOut,
    IN OUT    DRM_DWORD *f_pcchOut )
{
    DRM_RESULT       dr         = DRM_SUCCESS;
    DRM_CHAR        *pchCurr    = f_rgchOut;
    const DRM_CHAR  *pchEnd     = NULL;
    DRM_CHAR         ch         = 0;
    DRM_DWORD        i          = 0;
    DRM_DWORD        j          = 0;
    DRM_DWORD        cch        = 0;

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

/*******************************************************************************
**
** Function:    PolicyCallback
**
** Synopsis:    This function is called back by Bind during license policy evalu
**              ation to process such things as OPLs, inclusion lists, and exten
**              sible restrictions. To properly implement your own policy callback
**              you must be careful of the compliance and robustness rules.
**
** Arguments:
**
** [f_pvPolicyCallbackData] -- Pointer to callback data in a format dependent on
**                             the type of callback.
** [f_dwCallbackType]       -- The type of callback being made.
** [f_pv]                   -- Pointer to custom data, for OTADLA this will be NULL.
**
** Returns:
**                          DRM_SUCCESS  The function completed successfully.
**
*******************************************************************************/
DRM_RESULT DRM_CALL PolicyCallback(
    IN const DRM_VOID *f_pvPolicyCallbackData,
    IN DRM_POLICY_CALLBACK_TYPE f_dwCallbackType,
    IN const DRM_VOID *f_pv )
{
    return DRMTOOLS_PrintPolicyCallback( f_pvPolicyCallbackData, f_dwCallbackType );
}

/*********************************************************************
**
** Function: PrintUsage
**
** Synopsis: Function that shows how the application is to be used.
**
** Arguments:               N/A
**
** Returns:                 N/A
**
**********************************************************************/
DRM_VOID PrintUsage()
{
    printf( "Usage:\nOTADLAClient.exe [-i:<input file name> | -j | -l | -r] [-o:<output file name>] [-s:<server page url>] [-u] [-p] [-d] [-f | -f:<fallback page url>] [-h] [-e] [t:<HDS Filename>]\n" );
    printf( "-j: Join the domain and then exit.\n" );
    printf( "-l: Leave the domain and then exit.\n" );
    printf( "-r: Download latest revocation data package from server only.\n");
    printf( "-s: Overwrite default URL of a server.\n");
    printf( "    Example: -s:http://rmserver.contoso.com:1080/playready/default.asmx\n" );
    printf( "-u: Use the default test URL instead of the URL from the content.\n" );
    printf( "-p: Pause during transaction between states.\n" );
    printf( "-d: Require a domain membership to get a license.\n" );
    printf( "-c: Add custom data string to license challenge. Up to %d strings are allowed\n", MAX_NUM_CUSTOM_DATA );
    printf( "-f: Demonstrate license protocol fallback. Has option to overwrite default URL of a fallback server.\n" );
    printf( "    Example: -f:http://fallbacksrv.contoso.com:8080/fallback/page.asp\n" );
    printf( "-h: The input file contains RM header only\n" );
    printf( "-e: Use the Execute Right to Bind.\n" );
    printf( "-t: HDS to use. If this is used the HDS isn't deleted before and after execution.\n" );
}

/*********************************************************************
**
** Function: LogMessage
**
** Synopsis: Function that writes information to a standard output.
**
** Arguments:
**
** [f_pszMessage]           -- Pointer to a format string.
**
** Returns:                 N/A
**
**********************************************************************/
DRM_VOID DRM_CALL LogMessage(
    __in_z __format_string DRM_CHAR *f_pszMessage, ... )
{
    va_list va;

    va_start( va, f_pszMessage );

    vprintf( f_pszMessage, va );

    va_end( va );

    printf( "\n" );
}

/*********************************************************************
**
** Function: LogPacket
**
** Synopsis: Function that logs a packet into a file. If the file exists,
**           its content will be overwritten.
**
** Arguments:
**
** [f_fChallenge]           -- Flag indicating whether the packet is a
**                             challenge or response packet.
** [f_pbPacket]             -- Pointer to a buffer that contains the packet.
** [f_cbPacket]             -- Size of the packet.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL if file I/O error happens.
**
**********************************************************************/
DRM_RESULT DRM_CALL LogPacket(
    IN DRM_BOOL f_fChallenge,
    IN DRM_BYTE *f_pbPacket,
    IN DRM_DWORD f_cbPacket )
{
    DRM_RESULT dr = DRM_SUCCESS;

#if LOG_PACKETS

    OEM_FILEHDL fp                                                = OEM_INVALID_HANDLE_VALUE;
    DRM_WCHAR   rgwchFileName[ MAX_PATH ]                         = { 0 };
    DRM_WCHAR   rgwchCount[ DRM_MAX_CCH_BASE10_DWORD_STRING + 1 ] = { 0 };  /* DRM_MAX_CCH_BASE10_DWORD_STRING doesn't include null terminator */
    DRM_DWORD   cbWritten                                         = 0;

    ChkArg( f_pbPacket != NULL && f_cbPacket > 0 );

    ChkDR( DRM_UTL_NumberToString(
        f_fChallenge ? g_dwChallengePacketCount++ : g_dwResponsePacketCount++,
        rgwchCount,
        NO_OF( rgwchCount ),
        NO_OF( rgwchCount ) - 1, /* Pad string up to full length NOT including null terminator */
        10, /* base 10 */
        NULL ) );

    ChkDR( DRM_STR_StringCchCopyW(
        rgwchFileName,
        NO_OF( rgwchFileName ),
        f_fChallenge ? CHALLENGE_STRING : RESPONSE_STRING ) );
    ChkDR( DRM_STR_StringCchCatW(
        rgwchFileName,
        NO_OF( rgwchFileName ),
        rgwchCount ) );
    ChkDR( DRM_STR_StringCchCatW(
        rgwchFileName,
        NO_OF( rgwchFileName ),
        DATEXTENSION_STRING ) );

    fp = Oem_File_Open( NULL,
                        rgwchFileName,
                        OEM_GENERIC_WRITE,
                        OEM_FILE_SHARE_NONE,
                        OEM_CREATE_ALWAYS,
                        OEM_ATTRIBUTE_NORMAL );

    if ( fp == OEM_INVALID_HANDLE_VALUE )
    {
        LogMessage( "LogPacket: Failed to write file %S.",
                    rgwchFileName );

        ChkDR( DRM_E_FAIL );
    }

    if ( !Oem_File_Write( fp, f_pbPacket, f_cbPacket, &cbWritten ) ||
         cbWritten != f_cbPacket )
    {
        ChkDR( DRM_E_FAIL );
    }

ErrorExit:

    if ( fp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( fp );
    }

#endif  /* LOG_PACKETS */

    return dr;
}

#if DEMO_LAUNCH_PLAYER

/*********************************************************************
**
** Function: LaunchPlayer
**
** Synopsis: Function that launches a media player application to play
**           a decrypted content file.
**
** Arguments:
**
** [f_poState]              -- Pointer to the state machine context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL if process cannot be created.
**
**********************************************************************/
DRM_RESULT DRM_CALL LaunchPlayer(
    IN OTADLA_STATE *f_poState )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_WCHAR rgwchCommand[ MAX_COMMAND_LINE_SIZE ] = { 0 };
    DRM_WCHAR rgwchDirectory[ MAX_PATH ] = { 0 };
    STARTUPINFOW oStartupInfo = { SIZEOF( STARTUPINFO ) };
    PROCESS_INFORMATION oProcInfo = { 0 };
    DRM_BOOL fRes = FALSE;

    ChkArg( f_poState != NULL );

    if ( GetCurrentDirectoryW( NO_OF( rgwchDirectory ), (LPWSTR)rgwchDirectory  ) == 0 )
    {
        goto ErrorExit;
    }

    ChkDR( DRM_STR_StringCchCopyW(
        rgwchCommand,
        NO_OF( rgwchCommand ),
        PLAYER_PATH_STRING ) );
    ChkDR( DRM_STR_StringCchCatW(
        rgwchCommand,
        NO_OF( rgwchCommand ),
        SPACE_STRING ) );
    ChkDR( DRM_STR_StringCchCatW(
        rgwchCommand,
        NO_OF( rgwchCommand ),
        rgwchDirectory ) );
    ChkDR( DRM_STR_StringCchCatW(
        rgwchCommand,
        NO_OF( rgwchCommand ),
        BACKSLASH_STRING ) );
    ChkDR( DRM_STR_StringCchCatW(
        rgwchCommand,
        NO_OF( rgwchCommand ),
        f_poState->m_pdstrDecryptedFile->pwszString ) );

    ChkBOOL( CreateProcessW( NULL,
                             (LPWSTR)rgwchCommand,
                             NULL,
                             NULL,
                             FALSE,
                             0,
                             NULL,
                             (LPCWSTR)rgwchDirectory,
                             &oStartupInfo,
                             &oProcInfo ) != 0, DRM_E_FAIL );

    if ( oProcInfo.hProcess != NULL )
    {
        CloseHandle( oProcInfo.hProcess );
    }

    if ( oProcInfo.hThread != NULL )
    {
        CloseHandle( oProcInfo.hThread );
    }

ErrorExit:

    return dr;
}

#endif  /* DEMO_LAUNCH_PLAYER */

/*********************************************************************
**
** Function: NetClientSendRequest
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
** [f_pszURL]               -- Optional server URL to use.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL if any HTTP error happens or too many
**                          redirections have been tried.
**
**********************************************************************/
DRM_RESULT DRM_CALL NetClientSendRequest(
    IN               HINTERNET  f_hSession,
    IN OUT           HINTERNET *f_phConnect,
    IN OUT           HINTERNET *f_phHttpFile,
    IN               DRM_BYTE  *f_pbChallenge,
    IN               DRM_DWORD  f_cbChallenge,
    IN               DRM_CHAR  *f_pszHeader,
    IN               DRM_DWORD  f_dwLevel,
    IN               DRM_BOOL   f_fUseSOAP,
    __in_z_opt const DRM_CHAR  *f_pszURL )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR rgbQuery[ MAX_URL_SIZE ] = { 0 };
    DRM_DWORD cbQuery = 0;
    URL_COMPONENTSA oUrlComponents = { 0 };
    DRM_CHAR rgbHostName[ MAX_URL_SIZE ] = { 0 };
    DRM_CHAR rgbUrlPath[ MAX_URL_SIZE ] = { 0 };

    ChkArg( f_phConnect != NULL );
    ChkArg( f_phHttpFile != NULL );
    ChkArg( f_pbChallenge != NULL || f_cbChallenge == 0 );
    ChkArg( f_dwLevel <= MAX_REDIRECTIONS_PER_REQUEST );
    ChkArg( ( f_dwLevel == 0 && *f_phConnect == NULL && *f_phHttpFile == NULL ) ||
            ( f_dwLevel > 0 && *f_phConnect != NULL && *f_phHttpFile != NULL ) );
    ChkArg( f_pszURL == NULL || f_dwLevel == 0 );

    if ( f_dwLevel > 0 )
    {
        cbQuery = SIZEOF( rgbQuery );

        /* Retrieve the returned status code. */
        ChkBOOL( HttpQueryInfoA( *f_phHttpFile,
                                 HTTP_QUERY_STATUS_CODE,
                                 rgbQuery,
                                 (LPDWORD)&cbQuery,
                                 NULL ), DRM_E_FAIL );

        /* Check if the status code is HTTP_STATUS_TEMPORARY_REDIRECT. */
        if ( atoi( rgbQuery ) == HTTP_STATUS_TEMPORARY_REDIRECT )
        {
            cbQuery = SIZEOF( rgbQuery );

            /* Retrieve the new location (URL) to be redirected. */
            ChkBOOL( HttpQueryInfoA( *f_phHttpFile,
                                     HTTP_QUERY_LOCATION ,
                                     rgbQuery,
                                     (LPDWORD)&cbQuery,
                                     NULL ), DRM_E_FAIL );

            ZEROMEM( &oUrlComponents, SIZEOF( URL_COMPONENTSA ) );

            oUrlComponents.dwStructSize = SIZEOF( URL_COMPONENTSA );
            oUrlComponents.lpszHostName = rgbHostName;
            oUrlComponents.dwHostNameLength = SIZEOF( rgbHostName );
            oUrlComponents.lpszUrlPath = rgbUrlPath;
            oUrlComponents.dwUrlPathLength = SIZEOF( rgbUrlPath );

            /* Parse the full redirected URL string into components. */
            ChkBOOL( InternetCrackUrlA( rgbQuery,
                                        0,
                                        0,
                                        &oUrlComponents ), DRM_E_FAIL );
        }
        else
        {
            /* The error status code is something else, exit the function. */
            ChkDR( DRM_E_FAIL );
        }
    }
    else
    {
        if ( f_pszURL != NULL )
        {
            ZEROMEM( &oUrlComponents, SIZEOF( URL_COMPONENTSA ) );

            oUrlComponents.dwStructSize = SIZEOF( URL_COMPONENTSA );
            oUrlComponents.lpszHostName = rgbHostName;
            oUrlComponents.dwHostNameLength = SIZEOF( rgbHostName );
            oUrlComponents.lpszUrlPath = rgbUrlPath;
            oUrlComponents.dwUrlPathLength = SIZEOF( rgbUrlPath );

            ChkBOOL( InternetCrackUrlA( f_pszURL,
                                        0,
                                        0,
                                        &oUrlComponents ), DRM_E_FAIL );
        }
        else
        {
            if ( f_fUseSOAP )
            {
                oUrlComponents.lpszHostName = g_oServer.m_rgchServerName;
                oUrlComponents.nPort = g_oServer.m_dwHttpPort;
                oUrlComponents.lpszUrlPath = g_oServer.m_rgchURL;
            }
            else
            {
                /* If SOAP is not used, assume fallback server is being used. */
                oUrlComponents.lpszHostName = g_oFallback.m_rgchServerName;
                oUrlComponents.nPort = g_oFallback.m_dwHttpPort;
                oUrlComponents.lpszUrlPath = g_oFallback.m_rgchURL;
            }
        }
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
                                     oUrlComponents.lpszHostName,
                                     oUrlComponents.nPort,
                                     NULL,
                                     NULL,
                                     INTERNET_SERVICE_HTTP,
                                     0,
                                     0 );

    if ( *f_phConnect == NULL )
    {
        ChkDR( DRM_E_FAIL );
    }

    *f_phHttpFile = HttpOpenRequestA( *f_phConnect,
                                      f_pbChallenge == NULL ? "GET" : "POST",
                                      oUrlComponents.lpszUrlPath,
                                      NULL,
                                      NULL,
                                      0,
                                      INTERNET_FLAG_DONT_CACHE,
                                      0 );

    if ( *f_phHttpFile == NULL )
    {
        ChkDR( DRM_E_FAIL );
    }

    if( f_pszHeader != NULL )
    {
        /* Add the customized HTTP header to the HTTP request. */
        ChkBOOL( HttpAddRequestHeadersA( *f_phHttpFile,
                                         f_pszHeader,
                                         ( DRM_DWORD )DRMCRT_strlen( f_pszHeader ),
                                         HTTP_ADDREQ_FLAG_ADD_IF_NEW ), DRM_E_FAIL );
    }

    /* Send the HTTP request to the server. */
    if ( !HttpSendRequestA( *f_phHttpFile,
                            NULL,
                            0,
                            f_pbChallenge,
                            f_cbChallenge ) )
    {
        /*
        ** The server returns an error, recursively checks whether
        ** redirection is needed.
        */
        DRM_RESULT drNetwork = HRESULT_FROM_WIN32( 0/*GESTATE*/ );

        dr = NetClientSendRequest( f_hSession,
                                   f_phConnect,
                                   f_phHttpFile,
                                   f_pbChallenge,
                                   f_cbChallenge,
                                   f_pszHeader,
                                   f_dwLevel + 1,
                                   f_fUseSOAP,
                                   NULL );

        if( dr == DRM_E_FAIL )
        {
            dr = drNetwork;
            if( SUCCEEDED( dr ) )
            {
                dr = DRM_E_FAIL;
            }
        }
        ChkDR( dr );
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: NetClient
**
** Synopsis: Function that sends a challenge packet to the server and
**           receives a response packetfrom the server.
**           This function uses Windows Internet API library to do
**           the operations. It can be configured to use different
**           proxy settings.
**
** Arguments:
**
** [f_Type]                 -- Type of web service to communicate with.
** [f_pbChallenge]          -- Pointer to buffer that contains a challenge
**                             packet to be sent to the server.
** [f_cbChallenge]          -- Size of the buffer that contains a challenge
**                             packet to be sent to the server.
** [f_fUseSOAP]             -- Flag indicating whether to use SOAP to send
**                             the challenge message.
** [f_pszURL]               -- Optional server URL to use.
** [f_ppbResponse]          -- Pointer to pointer to a buffer that receives
**                             a response packet from the server.
** [f_ppbResponse]          -- Pointer to a variable that reveives the size of
**                             a response packet from the server.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL if any HTTP error happens.
**
**********************************************************************/
static DRM_RESULT DRM_CALL NetClient(
    IN               OTADLA_NET_COMMAND_TYPE   f_Type,
    IN               DRM_BYTE                 *f_pbChallenge,
    IN               DRM_DWORD                 f_cbChallenge,
    IN               DRM_BOOL                  f_fUseSOAP,
    __in_z_opt const DRM_CHAR                 *f_pszURL,
    OUT              DRM_BYTE                **f_ppbResponse,
    OUT              DRM_DWORD                *f_pcbResponse )
{
    DRM_RESULT dr = DRM_SUCCESS;
    HINTERNET hSession = NULL;
    HINTERNET hConnect = NULL;
    HINTERNET hHttpFile = NULL;
    DRM_BYTE rgbHeaderBuffer[ MAX_HTTP_HEADER_SIZE ] = { 0 };
    DRM_DWORD cbHeaderBuffer = MAX_HTTP_HEADER_SIZE;
    DRM_BYTE *pbCurrent = NULL;
    DRM_DWORD cbCurrent = 0, cbRead = 0;
    DRM_CHAR *pszHeader = NULL;

    ChkArg( f_ppbResponse != NULL );
    ChkArg( f_pcbResponse != NULL );

    if ( f_fUseSOAP )
    {
        switch ( f_Type )
        {
            case OTADLA_NET_JOINDOMAIN:
                                        pszHeader = HTTP_HEADER_JOIN;
                                        break;

            case OTADLA_NET_LEAVEDOMAIN:
                                        pszHeader = HTTP_HEADER_LEAVE;
                                        break;

            case OTADLA_NET_GETLICENSE:
                                        pszHeader = HTTP_HEADER_LICGET;
                                        break;

            case OTADLA_NET_ACKLICENSE:
                                        pszHeader = HTTP_HEADER_LICACK;
                                        break;

            case OTADLA_NET_METERCERT:
                                        pszHeader = HTTP_HEADER_METERCERT;
                                        break;

            case OTADLA_NET_METERDATA:
                                        pszHeader = HTTP_HEADER_METERDATA;
                                        break;

            default:
                                        ChkDR( DRM_E_FAIL );
                                        break;
        }
    }
    else
    {
        switch ( f_Type )
        {
            case OTADLA_NET_GETLICENSE:
                                        pszHeader = HTTP_FALLBACK_LICENSE_HEADER;

                                        DRM_STR_StringCchCopyA( g_oFallback.m_rgchURL,
                                                                SIZEOF( g_oFallback.m_rgchURL ),
                                                                HTTP_FALLBACK_LICENSE_PAGE );
                                        break;

            case OTADLA_NET_METERDATA:
                                        pszHeader = HTTP_FALLBACK_METERING_HEADER;

                                        DRM_STR_StringCchCopyA( g_oFallback.m_rgchURL,
                                                                SIZEOF( g_oFallback.m_rgchURL ),
                                                                HTTP_FALLBACK_METERING_PAGE );

                                        DRM_STR_StringCchCopyA( g_oFallback.m_rgchServerName,
                                                                SIZEOF( g_oFallback.m_rgchServerName ),
                                                                HTTP_FALLBACK_SERVER_NAME );
                                        break;
            case OTADLA_NET_REVOCATIONPACKAGE:
                                        pszHeader = NULL;

                                        DRM_STR_StringCchCopyA( g_oFallback.m_rgchURL,
                                                                SIZEOF( g_oFallback.m_rgchURL ),
                                                                HTTP_REVOCATION_PACKAGE_PAGE );

                                        DRM_STR_StringCchCopyA( g_oFallback.m_rgchServerName,
                                                                SIZEOF( g_oFallback.m_rgchServerName ),
                                                                HTTP_REVOCATION_PACKAGE_SERVER );

                                        break;


            default:
                                        ChkDR( DRM_E_FAIL );
                                        break;
        }
    }

    /* Use proxy settings configured in the Internet Explorer. */
    hSession = InternetOpenA( "OTADLAClient",
                              PRE_CONFIG_INTERNET_ACCESS,
                              "",
                              INTERNET_INVALID_PORT_NUMBER,
                              0 );

    if ( hSession == NULL )
    {
        ChkDR( DRM_E_FAIL );
    }

    /*
    ** Send the challenge to the server and handle possible
    ** HTTP redirection(s).
    */
    ChkDR( NetClientSendRequest( hSession,
                                 &hConnect,
                                 &hHttpFile,
                                 f_pbChallenge,
                                 f_cbChallenge,
                                 pszHeader,
                                 0,
                                 f_fUseSOAP,
                                 f_pszURL ) );

    /*
    ** Query for the content length from the HTTP header of the
    ** server response. Content length indicates the size of
    ** HTTP data that follows the HTTP header in the server response.
    */
    ChkBOOL( HttpQueryInfoA( hHttpFile,
                             HTTP_QUERY_CONTENT_LENGTH,
                             rgbHeaderBuffer,
                             (LPDWORD)&cbHeaderBuffer,
                             NULL ), DRM_E_FAIL );

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
        ChkBOOL( InternetReadFile( hHttpFile,
                                   pbCurrent,
                                   cbCurrent,
                                   ( LPDWORD )&cbRead ), DRM_E_FAIL );

        ChkBOOL( cbRead > 0, DRM_E_FAIL );

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

    return dr;
}

/*********************************************************************
**
** Function: SetContentHeaderUsingMediaFile
**
** Synopsis: Function that loads the input content file to initialize
**           the content header in the DRM App context.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poMediaFile]          -- Pointer to an opened DRM_MEDIA_FILE_CONTEXT
**                             with the input file.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL SetContentHeaderUsingMediaFile(
    IN DRM_APP_CONTEXT        *f_poAppContext,
    IN DRM_MEDIA_FILE_CONTEXT *f_poMediaFile )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poAppContext != NULL );

    ChkArg( f_poMediaFile  != NULL );

    ChkDR( Drm_Content_SetProperty( f_poAppContext,
                                    DRM_CSP_AUTODETECT_HEADER,
                                    f_poMediaFile->rgbDRMHeaderData,
                                    f_poMediaFile->cbDRMHeaderData ) );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: SetContentHeaderUsingRMHeader
**
** Synopsis: Function that loads the RM header in the input file to
**           initialize the content header in the DRM App context.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_pdstrHeaderFile]      -- Pointer to a input file that contains the RM
**                             header.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL SetContentHeaderUsingRMHeader(
    IN DRM_APP_CONTEXT        *f_poAppContext,
    IN DRM_CONST_STRING       *f_pdstrHeaderFile )
{
    DRM_RESULT dr = DRM_SUCCESS;
    OEM_FILEHDL Infp = OEM_INVALID_HANDLE_VALUE;
    DRM_BYTE *pbHeaderData = NULL;
    DRM_DWORD cbHeaderData = 0;
    DRM_DWORD cbRead = 0;

    ChkArg( f_poAppContext != NULL );

    ChkDRMString( f_pdstrHeaderFile );

    ChkBOOL( ( Infp = Oem_File_Open( NULL,
                                     f_pdstrHeaderFile->pwszString,
                                     OEM_GENERIC_READ,
                                     OEM_FILE_SHARE_READ,
                                     OEM_OPEN_EXISTING,
                                     OEM_ATTRIBUTE_NORMAL ) ) != OEM_INVALID_HANDLE_VALUE,
             DRM_E_FILEOPEN );

    ChkBOOL( Oem_File_SetFilePointer( Infp,
                                      0,
                                      OEM_FILE_END,
                                      &cbHeaderData ), DRM_E_FILESEEKERROR );

    ChkMem( pbHeaderData = ( DRM_BYTE * )Oem_MemAlloc( cbHeaderData ) );

    ChkBOOL( Oem_File_SetFilePointer( Infp,
                                      0,
                                      OEM_FILE_BEGIN,
                                      NULL ), DRM_E_FILESEEKERROR );

    ChkBOOL( Oem_File_Read( Infp,
                            pbHeaderData,
                            cbHeaderData,
                            &cbRead ), DRM_E_FILEREADERROR );

    ChkBOOL( cbRead == cbHeaderData, DRM_E_FILEREADERROR );

    ChkDR( Drm_Content_SetProperty( f_poAppContext,
                                    DRM_CSP_AUTODETECT_HEADER,
                                    pbHeaderData,
                                    cbHeaderData ) );

ErrorExit:

    if ( Infp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( Infp );
    }

    SAFE_OEM_FREE( pbHeaderData );

    return dr;
}

/*********************************************************************
**
** Function: ProcessAdditionalInformation
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
DRM_RESULT DRM_CALL ProcessAdditionalInformation(
    IN           DRM_APP_CONTEXT *f_pAppContext,
    IN     const DRM_BYTE        *f_pbResponse,
    IN           DRM_DWORD        f_cbResponse,
    IN           DRM_DWORD        f_dwDataType,
    OUT          DRM_CHAR        *f_pchData,
    IN OUT       DRM_DWORD       *f_pcchData )
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
** Function: GenerateDomainJoinChallenge
**
** Synopsis: Function that generates a domain join challenge to be
**           sent to the domain server.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poDomainID]           -- Pointer to a domain ID of the domain to join.
** [f_fOldRevision]         -- Flag indicating whether to retrieve domain keys
**                             of old revisions.
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
DRM_RESULT DRM_CALL GenerateDomainJoinChallenge(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_DOMAIN_ID *f_poDomainID,
    IN DRM_BOOL f_fOldRevision,
    IN DRM_BOOL f_fEchoDomainID,
    OUT DRM_BYTE **f_ppbChallenge,
    OUT DRM_DWORD *f_pcbChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR rgchFriendlyName[] = "My Phone";
    DRM_CHAR rgchCustomData[ MAX_CUSTOM_DATA_SIZE ] = "HelloWorld";
    DRM_DWORD cchFriendlyName = 0;
    DRM_DWORD cchCustomData = 0;
    /* Predefined domain account ID on the server. */
    DRM_ID oAccountID = { 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30, 0x39,
                          0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31 };
    /* Predefined domain service ID on the server. */
    DRM_ID oServiceID = { 0x00, 0x7f, 0xB4, 0xde, 0x3b, 0x8a, 0x6d, 0x41,
                          0x9b, 0x1e, 0x5d, 0x55, 0xfd, 0x02, 0x30, 0x44 };

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_poDomainID != NULL );
    ChkArg( f_ppbChallenge != NULL );
    ChkArg( f_pcbChallenge != NULL );

    if ( f_fOldRevision )
    {
        ChkDR( DRM_STR_StringCchCatA( rgchCustomData,
                                      SIZEOF( rgchCustomData ),
                                      " WellKnownRevisionExpired" ) );
    }

    if ( f_fEchoDomainID )
    {
        ChkDR( DRM_STR_StringCchCatA( rgchCustomData,
                                      SIZEOF( rgchCustomData ),
                                      "  EchoJoinParameters" ) );
    }
    else
    {
        MEMCPY( &f_poDomainID->m_oAccountID, &oAccountID, SIZEOF( DRM_ID ) );
        MEMCPY( &f_poDomainID->m_oServiceID, &oServiceID, SIZEOF( DRM_ID ) );
    }

    ChkDR( DRM_STR_StringCchLengthA( rgchCustomData, SIZEOF( rgchCustomData ), &cchCustomData ) );
    ChkDR( DRM_STR_StringCchLengthA( rgchFriendlyName, SIZEOF( rgchFriendlyName ), &cchFriendlyName ) );

    dr = Drm_JoinDomain_GenerateChallenge( f_poAppContext,
                                           DRM_REGISTER_CUSTOM_DATA,
                                           f_poDomainID,
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
                                                 f_poDomainID,
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
** Function: ProcessDomainJoinResponse
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
DRM_RESULT DRM_CALL ProcessDomainJoinResponse(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_BYTE *f_pbResponse,
    IN DRM_DWORD f_cbResponse )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_RESULT dr1 = DRM_SUCCESS;
    DRM_DOMAIN_ID oDomainID = EMPTY_DRM_DOMAIN_ID;

    ChkArg( f_poAppContext != NULL );

    ChkDR( Drm_JoinDomain_ProcessResponse( f_poAppContext,
                                           DRM_PROCESS_DJ_RESPONSE_SIGNATURE_NOT_REQUIRED,
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
** Function: GenerateDomainLeaveChallenge
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
DRM_RESULT DRM_CALL GenerateDomainLeaveChallenge(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_DOMAIN_ID *f_poDomainID,
    OUT DRM_BYTE **f_ppbChallenge,
    OUT DRM_DWORD *f_pcbChallenge)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR rgchCustomData[ MAX_CUSTOM_DATA_SIZE ] = "HelloWorld";
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
** Function: ProcessDomainLeaveResponse
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
DRM_RESULT DRM_CALL ProcessDomainLeaveResponse(
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
** Function: GenerateLicenseChallenge
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
DRM_RESULT DRM_CALL GenerateLicenseChallenge(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_BOOL f_fDomain,
    IN DRM_BOOL f_fDomainExcluded,
    IN DRM_BOOL f_fAESPackage,
    IN DRM_BOOL f_fProtocolFallback,
    IN DRM_LICENSE_PROTOCOL_TYPE f_ProtocolVersion,
    IN DRM_DOMAIN_ID *f_poDomainID,
    OUT DRM_BYTE **f_ppbChallenge,
    IN OUT DRM_DWORD *f_pcbChallenge,
    OUT DRM_CHAR *f_pchURL,
    IN OUT DRM_DWORD *f_pcchURL )
{
    DRM_RESULT dr = DRM_SUCCESS;
    const DRM_CONST_STRING *rgpdstrRights[ 1 ]=
            { ( DRM_CONST_STRING * )&g_dstrWMDRM_RIGHT_PLAYBACK };
    DRM_CHAR rgchCustomData[ MAX_CUSTOM_DATA_SIZE ] = "AddMid";
    DRM_DWORD cchCustomData = 0;
    DRM_DWORD cbOffset = 0;
    DRM_DWORD i = 0;

    ChkArg( f_poAppContext );
    ChkArg( f_ppbChallenge != NULL );
    ChkArg( f_pcbChallenge != NULL );

#if DEMO_CHAINED_LICENSE

    ChkDR( DRM_STR_StringCchCatA( rgchCustomData,
                                  SIZEOF( rgchCustomData ),
                                  " UseChainedLicense" ) );

#endif

#if DEMO_SILVERLIGHT_LICENSE

    ChkDR( DRM_STR_StringCchCatA( rgchCustomData,
                                  SIZEOF( rgchCustomData ),
                                  " UseSilverlightLicense" ) );

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

    for ( i = 0; i < g_dwCustomData; i++ )
    {
        DRM_STR_StringCchCatA( rgchCustomData, SIZEOF( rgchCustomData ), " " );

        DRM_STR_StringCchCatA( rgchCustomData,
                               SIZEOF( rgchCustomData ),
                               g_customData[i].pszString );
    }

    ChkDR( DRM_STR_StringCchLengthA( rgchCustomData,
                                     SIZEOF( rgchCustomData ),
                                     &cchCustomData ) );

    /*
    ** The license server's URL is extracted from the content header.
    */
    dr = Drm_LicenseAcq_GenerateChallenge( f_poAppContext,
                                           f_ProtocolVersion == eV2Protocol ?
                                           rgpdstrRights : NULL,
                                           f_ProtocolVersion == eV2Protocol ?
                                           NO_OF( rgpdstrRights ) : 0,
                                           f_ProtocolVersion == eV2Protocol ?
                                           NULL : f_poDomainID,
                                           cchCustomData == 0 ? NULL : rgchCustomData,
                                           cchCustomData,
                                           f_pchURL,
                                           f_pcchURL,
                                           NULL,
                                           NULL,
                                           NULL,
                                           f_pcbChallenge);

    if ( dr == DRM_E_BUFFERTOOSMALL )
    {
        if ( f_ProtocolVersion == eV2Protocol )
        {
            /*
            ** For V2 license challenge, the generated challenge needs to
            ** have a prefix. calculate the size of buffer needed for the
            ** prefix.
            */
            cbOffset = DRMCRT_strlen( V2_LICENSE_CHALLENGE_PREFIX );

            ChkOverflow( *f_pcbChallenge + cbOffset, *f_pcbChallenge );

            *f_pcbChallenge += cbOffset;
        }

        ChkMem( *f_ppbChallenge = ( DRM_BYTE * )Oem_MemAlloc( *f_pcbChallenge ) );

        if ( f_ProtocolVersion == eV2Protocol )
        {
            /* Store the V2 challenge prefix string at the beginning of the buffer. */
            MEMCPY( *f_ppbChallenge,
                    V2_LICENSE_CHALLENGE_PREFIX,
                    cbOffset );
        }

        ChkDR( Drm_LicenseAcq_GenerateChallenge( f_poAppContext,
                                                 f_ProtocolVersion == eV2Protocol ?
                                                 rgpdstrRights : NULL,
                                                 f_ProtocolVersion == eV2Protocol ?
                                                 NO_OF( rgpdstrRights ) : 0,
                                                 f_ProtocolVersion == eV2Protocol ?
                                                 NULL : f_poDomainID,
                                                 cchCustomData == 0 ? NULL : rgchCustomData,
                                                 cchCustomData,
                                                 f_pchURL,
                                                 f_pcchURL,
                                                 NULL,
                                                 NULL,
                                                 *f_ppbChallenge + cbOffset,
                                                 f_pcbChallenge));

        if ( f_ProtocolVersion == eV2Protocol )
        {
            /*
            ** The total length of the V2 license challange should include the length
            ** of the V2 challenge prefix string.
            */
            *f_pcbChallenge += cbOffset;
        }
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
** Function: ProcessLicenseChallengeResponse
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
DRM_RESULT DRM_CALL ProcessLicenseChallengeResponse(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_BYTE *f_pbResponse,
    IN DRM_DWORD f_cbResponse,
    OUT DRM_LICENSE_RESPONSE *f_poLicResponse )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_pbResponse != NULL && f_cbResponse > 0 );
    ChkArg( f_poLicResponse != NULL );

    ChkDR( Drm_LicenseAcq_ProcessResponse( f_poAppContext,
                                           DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
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
** Function: GenerateLicenseAck
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
DRM_RESULT DRM_CALL GenerateLicenseAck(
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
DRM_RESULT DRM_CALL ProcessLicenseAckResponse(
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

/*********************************************************************
**
** Function: GenerateMeterCertChallenge
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
DRM_RESULT DRM_CALL GenerateMeterCertChallenge(
    __in                                         DRM_APP_CONTEXT *f_poAppContext,
    __in                                         DRM_MID         *f_poMID,
    __deref_out_bcount(*f_pcbMeterCertChallenge) DRM_BYTE       **f_ppbMeterCertChallenge,
    __inout                                      DRM_DWORD       *f_pcbMeterCertChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;

    ChkArg( f_poAppContext != NULL );
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
** Function: ProcessMeterCertResponse
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
DRM_RESULT DRM_CALL ProcessMeterCertResponse(
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
** Function: GenerateMeterDataChallenge
**
** Synopsis: Function that generates a meter data challenge
**           to be sent to a metering server.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_pbMeterCert]          -- Pointer to byte buffer that contains a meter
**                             certificate.
** [f_cbMeterCert]          -- Size of the byte buffer that contains a meter
**                             certificate.
** [f_fIsV2]                -- Flag indicating whether the meter certificate
**                             is a V2 certificate.
** [f_pchURL]               -- Pointer to a DRM_CHAR buffer to receive the
**                             URL to post the meter data challenge.
** [f_pcchURL]              -- Pointer to a variable that contains the size (in
**                             characters) of the buffer used to receive
** [f_ppbMeterCertChallenge]-- Pointer to pointer of a buffer that receives the
**                             generated meter certificate challenge.
** [f_pcbMeterCertChallenge]-- Pointer to a variable that receives size of the
**                             meter certificate challenge.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL GenerateMeterDataChallenge(
    IN       DRM_APP_CONTEXT *f_poAppContext,
    IN const DRM_BYTE        *f_pbMeterCert,
    IN       DRM_DWORD        f_cbMeterCert,
    IN       DRM_BOOL         f_fIsV2,
    OUT      DRM_CHAR        *f_pchURL,
    IN OUT   DRM_DWORD       *f_pcchURL,
    OUT      DRM_BYTE       **f_ppbMeterDataChallenge,
    IN OUT   DRM_DWORD       *f_pcbMeterDataChallenge )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DWORD cbOffset = 0;
    DRM_BYTE *pbTemp1 = NULL, *pbTemp2 = NULL;
    DRM_DWORD cbTemp1 = 0, cbTemp2 = 0, cbTemp3 = 0;

    ChkArg( f_poAppContext);
    ChkArg( f_pbMeterCert && f_cbMeterCert > 0 );
    ChkArg( f_pchURL != NULL );
    ChkArg( f_pcchURL != NULL );
    ChkArg( f_ppbMeterDataChallenge != NULL );
    ChkArg( f_pcbMeterDataChallenge != NULL );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    dr = Drm_Metering_GenerateChallenge( f_poAppContext,
                                         f_pbMeterCert,
                                         f_cbMeterCert,
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
                                               f_pbMeterCert,
                                               f_cbMeterCert,
                                               NULL,
                                               0,
                                               f_pchURL,
                                               f_pcchURL,
                                               pbTemp1,
                                               &cbTemp1 ) );

        if ( f_fIsV2 )
        {
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
            dr = HTMLEncode( ( DRM_CHAR * )pbTemp2, cbTemp2, NULL, &cbTemp3 );

            if ( DRM_FAILED( dr ) )
            {
                if ( dr == DRM_E_BUFFERTOOSMALL )
                {
                    cbOffset = DRMCRT_strlen( V2_METERDATA_CHALLENGE_PREFIX );

                    ChkMem( *f_ppbMeterDataChallenge =
                                ( DRM_BYTE * )Oem_MemAlloc( cbTemp3 + cbOffset ) );

                    /* Store the meter data challenge prefix string at the beginning of the buffer. */
                    MEMCPY( *f_ppbMeterDataChallenge,
                            V2_METERDATA_CHALLENGE_PREFIX,
                            cbOffset );

                    /* HTML encode all special characters in the meter data challenge. */
                    ChkDR( HTMLEncode( ( DRM_CHAR * )pbTemp2,
                                       cbTemp2,
                                       ( DRM_CHAR * )*f_ppbMeterDataChallenge + cbOffset,
                                       &cbTemp3 ) );

                    /* Update the total size of the challenge to consider the prefix string. */
                    *f_pcbMeterDataChallenge = cbTemp3 + cbOffset;
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
            ChkMem( *f_ppbMeterDataChallenge =
                                ( DRM_BYTE * )Oem_MemAlloc( cbTemp1 ) );

            *f_pcbMeterDataChallenge = cbTemp1;

            MEMCPY( *f_ppbMeterDataChallenge,
                     pbTemp1,
                     cbTemp1 );
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
** Function: ProcessMeterDataResponse
**
** Synopsis: Function that processes a meter data response received
**           from a metering server. After the successful processing
**           of the meter data response, the metering records in the
**           local HDS will be cleared.
**
**           For V2 meter data response, it is in UTF8/ANSI and needs
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
** [f_fIsV2]                -- Flag indicating whether the meter response
**                             is a V2 response.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL ProcessMeterDataResponse(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN DRM_BYTE *f_pbResponse,
    IN DRM_DWORD f_cbResponse,
    IN DRM_BOOL f_fIsV2)
{
    DRM_RESULT dr         = DRM_SUCCESS;
    DRM_BYTE  *pbResponse = NULL;
    DRM_DWORD  dwFlag     = 0;
    DRM_STRING        dstrResponse  = EMPTY_DRM_STRING;

    ChkArg( f_poAppContext);
    ChkArg( f_pbResponse != NULL && f_cbResponse > 0 );

    ChkBOOL( DRM_METERING_IsMeteringSupported(), DRM_E_NOTIMPL );

    if ( f_fIsV2 )
    {
        ChkMem( pbResponse =
                    ( DRM_BYTE * )Oem_MemAlloc( f_cbResponse * SIZEOF( DRM_WCHAR ) ) );

        DSTR_FROM_PB( &dstrResponse, pbResponse, f_cbResponse * SIZEOF( DRM_WCHAR ) );

        /* Convert the server response to UTF-16. */
        ChkDR( DRM_STR_UTF8toDSTR( ( DRM_CHAR * )f_pbResponse, f_cbResponse, &dstrResponse ) );
    }

    ChkDR( Drm_Metering_ProcessResponse( f_poAppContext,
                                         f_fIsV2 ? pbResponse : f_pbResponse,
                                         f_fIsV2 ? f_cbResponse * SIZEOF( DRM_WCHAR ) : f_cbResponse,
                                         &dwFlag ) );

ErrorExit:
    SAFE_OEM_FREE( pbResponse );
    return dr;
}

/*********************************************************************
**
** Function: JoinDomain
**
** Synopsis: Function that gets executed when the current state of the
**           application is "Join Domain".
**           It constructs a domain join challenge and sends it to a
**           domain server. It then processes the response from the server.
**           Upon exit, it transits into the "Get License" state.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poState]              -- Pointer to the state machine context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL JoinDomain(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN OTADLA_STATE    *f_poState )
{
    DRM_RESULT     dr          = DRM_SUCCESS;
    DRM_BYTE      *pbChallenge = NULL;
    DRM_DWORD      cbChallenge = 0;
    DRM_BYTE      *pbResponse  = NULL;
    DRM_DWORD      cbResponse  = 0;
    DRM_DOMAIN_ID  oDomainID   = EMPTY_DRM_DOMAIN_ID;

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_poState      != NULL );

    ChkDR( GenerateDomainJoinChallenge( f_poAppContext,
                                        &oDomainID,
                                        !f_poState->m_fDomainExcluded,
                                        FALSE,
                                        &pbChallenge,
                                        &cbChallenge ) );

    ChkDR( LogPacket( TRUE,
                      pbChallenge,
                      cbChallenge ) );

    ChkDR( NetClient( OTADLA_NET_JOINDOMAIN,
                      pbChallenge,
                      cbChallenge,
                      TRUE,
                      NULL,
                      &pbResponse,
                      &cbResponse ) );

    ChkDR( LogPacket( FALSE,
                      pbResponse,
                      cbResponse ) );

    ChkDR( ProcessDomainJoinResponse( f_poAppContext,
                                      pbResponse,
                                      cbResponse ) );

    f_poState->m_PrevState = f_poState->m_State;

    /*
    ** If the flag is set to only join the domain, set the next state to deinit,
    ** otherwise transition into the state to get a license.
    */
    if( f_poState->m_fJoinDomainOnly )
    {
        f_poState->m_State = OTADLA_DEINITDRM;
    }
    else
    {
        f_poState->m_State = OTADLA_GETLICENSE;
    }

    f_poState->m_fInDomain = TRUE;

    LogMessage( "Join Domain: Success" );

ErrorExit:

    SAFE_OEM_FREE( pbChallenge );

    SAFE_OEM_FREE( pbResponse );

    if ( DRM_FAILED( dr ) )
    {
        LogMessage( "Join Domain: Failed" );
    }

    return dr;
}

/*********************************************************************
**
** Function: LeaveDomain
**
** Synopsis: Function that gets executed when the current state of the
**           application is "Leave Domain".
**           It constructs a domain leave challenge and sends it to a
**           domain server. It then processes the response from the server.
**           Upon exit, it transits into the "Deinit DRM" state.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poState]              -- Pointer to the state machine context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL LeaveDomain(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN OTADLA_STATE    *f_poState )
{
    DRM_RESULT                    dr                     = DRM_SUCCESS;
    DRM_DOMAIN_CERT_ENUM_CONTEXT  oDomainCertEnumContext = { 0 };
    DRM_DOMAINCERT_INFO           oDomainCertInfo        = { 0 };
    DRM_DWORD                     cchDomainCert          = 0;
    DRM_BYTE                     *pbChallenge            = NULL;
    DRM_DWORD                     cbChallenge            = 0;
    DRM_BYTE                     *pbResponse             = NULL;
    DRM_DWORD                     cbResponse             = 0;

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_poState != NULL );

    if ( !f_poState->m_fInDomain )
    {
        goto ErrorExit;
    }

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

        ChkDR( GenerateDomainLeaveChallenge( f_poAppContext,
                                             &oDomainCertInfo.m_oDomainID,
                                             &pbChallenge,
                                             &cbChallenge ) );

        ChkDR( LogPacket( TRUE,
                          pbChallenge,
                          cbChallenge ) );

        ChkDR( NetClient( OTADLA_NET_LEAVEDOMAIN,
                          pbChallenge,
                          cbChallenge,
                          TRUE,
                          NULL,
                          &pbResponse,
                          &cbResponse ) );

        ChkDR( LogPacket( FALSE,
                          pbResponse,
                          cbResponse ) );

        ChkDR( ProcessDomainLeaveResponse( f_poAppContext,
                                           pbResponse,
                                           cbResponse ) );

        SAFE_OEM_FREE( pbChallenge );

        SAFE_OEM_FREE( pbResponse );
    }

    f_poState->m_fInDomain = FALSE;

    LogMessage( "Leave Domain: Success" );

ErrorExit:

    if( f_poState != NULL )
    {
        f_poState->m_PrevState = f_poState->m_State;
        f_poState->m_State     = OTADLA_DEINITDRM;
    }

    SAFE_OEM_FREE( pbChallenge );

    SAFE_OEM_FREE( pbResponse );

    if ( DRM_FAILED( dr ) )
    {
        LogMessage( "Leave Domain: Failed" );
    }

    return dr;
}

/*********************************************************************
**
** Function: HandleLicenseError
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
static DRM_RESULT DRM_CALL HandleLicenseError(
    IN       DRM_APP_CONTEXT      *f_poAppContext,
    IN       OTADLA_STATE         *f_poState,
    IN       DRM_LICENSE_RESPONSE *f_poLicResponse,
       const DRM_BYTE             *f_pbResponse,
             DRM_DWORD             f_cbResponse )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CHAR rgchGUID[ CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) ) + 1 ] = { 0 };
    DRM_DWORD cchGUID = CCH_BASE64_EQUIV( SIZEOF( DRM_GUID ) );
    DRM_DWORD cbGUID = SIZEOF( DRM_GUID );
    DRM_SUBSTRING dasstrGUID = EMPTY_DRM_SUBSTRING;

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_poState != NULL );
    ChkArg( f_poLicResponse != NULL );
    ChkArg( f_pbResponse != NULL && f_cbResponse > 0 );

    ZEROMEM( rgchGUID, SIZEOF( rgchGUID ) );

    if ( f_poLicResponse->m_dwResult == DRM_E_SERVER_DOMAIN_REQUIRED )
    {
        LogMessage( "Get License (Challenge): Need to join a domain." );

        f_poState->m_cchURL = SIZEOF( f_poState->m_rgchURL );

        /*
        ** Extract the domain server URL from the server response.
        ** Technically the extract domain server URL can be used to
        ** redirect the domain service to a server other than the
        ** one that host the license service. However in this
        ** application the extracted information is just displayed
        ** and is not actually used.
        */
        ChkDR( ProcessAdditionalInformation( f_poAppContext,
                                             f_pbResponse,
                                             f_cbResponse,
                                             DRM_GARD_REDIRECT_URL,
                                             f_poState->m_rgchURL,
                                             &f_poState->m_cchURL ) );

        /* Display the extracted domain server URL. */
        LogMessage( "\nDomain server URL (from server's response):" );
        LogMessage( f_poState->m_rgchURL );
        LogMessage( "" );

        ZEROMEM( &f_poState->m_oDomainID, SIZEOF( DRM_DOMAIN_ID ) );

        ChkDR( ProcessAdditionalInformation( f_poAppContext,
                                             f_pbResponse,
                                             f_cbResponse,
                                             DRM_GARD_SERVICE_ID,
                                             rgchGUID,
                                             &cchGUID ) );
        /* Display the extracted service ID. */
        LogMessage( "\nService ID (from server's response):" );
        LogMessage( rgchGUID );

        dasstrGUID.m_cch = cchGUID;

        ChkDR( DRM_B64_DecodeA( rgchGUID,
                                &dasstrGUID,
                                &cbGUID,
                                ( DRM_BYTE * )&f_poState->m_oDomainID.m_oServiceID,
                                0 ) );

        f_poState->m_PrevState = f_poState->m_State;
        f_poState->m_State = OTADLA_JOINDOMAIN;
    }
    else if ( f_poLicResponse->m_dwResult == DRM_E_SERVER_RENEW_DOMAIN )
    {
        LogMessage( "Get License (Challenge): Domain expired." );

        f_poState->m_cchURL = SIZEOF( f_poState->m_rgchURL );

        /*
        ** Extract the domain server URL from the server response.
        ** Technically the extracted domain server URL can be used to
        ** redirect the domain service to a server other than the
        ** one that host the license service. However in this
        ** application the extracted information is just displayed
        ** and is not actually used.
        */
        ChkDR( ProcessAdditionalInformation( f_poAppContext,
                                             f_pbResponse,
                                             f_cbResponse,
                                             DRM_GARD_REDIRECT_URL,
                                             f_poState->m_rgchURL,
                                             &f_poState->m_cchURL ) );

        /* Display the extracted domain server URL. */
        LogMessage( "\nDomain server URL (from server's response):" );
        LogMessage( f_poState->m_rgchURL );

        /*
        ** Extract the service ID and account ID from the server response.
        ** Technically the extracted service ID and account ID can be used to
        ** formulate a new domain join challenge to be sent to the server.
        ** However in this application the extracted service ID and account
        ** ID are just displayed and is not actually used.
        */
        ChkDR( ProcessAdditionalInformation( f_poAppContext,
                                             f_pbResponse,
                                             f_cbResponse,
                                             DRM_GARD_SERVICE_ID,
                                             rgchGUID,
                                             &cchGUID ) );

        /* Display the extracted service ID. */
        LogMessage( "\nService ID (from server's response):" );
        LogMessage( rgchGUID );

        ChkDR( ProcessAdditionalInformation( f_poAppContext,
                                             f_pbResponse,
                                             f_cbResponse,
                                             DRM_GARD_ACCOUNT_ID,
                                             rgchGUID,
                                             &cchGUID ) );

        /* Display the extracted account ID. */
        LogMessage( "\nAccount ID (from server's response):" );
        LogMessage( rgchGUID );
        LogMessage( "" );

        f_poState->m_fDomainExcluded = FALSE;
        f_poState->m_PrevState = f_poState->m_State;
        f_poState->m_State = OTADLA_JOINDOMAIN;
    }
    else
    {
        dr = f_poLicResponse->m_dwResult;
    }

    LogMessage( "Get License (Challenge): Failed" );

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: GetLicense
**
** Synopsis: Function that gets executed when the current state of the
**           application is "Get License".
**           It constructs a license acquisition challenge and sends it to a
**           license server. It then processes the response from the server and
**           send the result of response processing as an acknowledgement to
**           the server.
**           Upon exit, it transits into the
**           1. "Play Music" if the license acquisition is successfully completed.
**           2. "Join Domain" if the license acquisition cannot be completed,
**              and the server returns an error "Domain Membership Required" or
**              "Domain Expired".
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poState]              -- Pointer to the state machine context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL GetLicense(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN OTADLA_STATE *f_poState )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbChallenge = NULL;
    DRM_DWORD cbChallenge = 0;
    DRM_BYTE *pbResponse = NULL;
    DRM_DWORD cbResponse = 0;
    DRM_LICENSE_RESPONSE oLicResponse = { eUnknownProtocol };
    DRM_BOOL fUseSoap = TRUE;

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_poState != NULL );
    ChkArg( f_poState->m_ProtocolVersion == eV2Protocol ||
            f_poState->m_ProtocolVersion == eV3Protocol );

    /*
    ** Need to clear the App Context State.
    */
    ChkDR( Drm_Reinitialize( f_poAppContext ) );

    if ( !f_poState->m_fHeaderOnly )
    {
        ChkDR( SetContentHeaderUsingMediaFile( f_poAppContext, f_poState->m_poMediaFile ) );
    }
    else
    {
        ChkDR( SetContentHeaderUsingRMHeader( f_poAppContext, f_poState->m_pdstrEncryptedFile ) );
    }

    /*
    ** If the version of the license protocol to use is V2 and the current content
    ** header type in the App context is of version V2.4, explicitly set the current
    ** content header type in the App context is of version V2.
    */
    if ( f_poState->m_ProtocolVersion == eV2Protocol )
    {
        DRM_CONTENT_SET_PROPERTY eHeaderInContext = DRM_CSP_HEADER_NOT_SET;
        DRM_DWORD cbHeaderInContext = SIZEOF( DRM_CONTENT_SET_PROPERTY );

        /* Retrieve the current content header type in App context. */
        ChkDR( Drm_Content_GetProperty( f_poAppContext,
                                        DRM_CGP_HEADER_TYPE,
                                        ( DRM_BYTE * )&eHeaderInContext,
                                        &cbHeaderInContext ) );

        DRMASSERT( cbHeaderInContext == SIZEOF( DRM_CONTENT_SET_PROPERTY ) &&
                   eHeaderInContext != DRM_CSP_HEADER_NOT_SET );

        if ( eHeaderInContext != DRM_CSP_V2_HEADER )
        {
            LogMessage( " Unexpected content header type.\n");

            ChkDR( DRM_E_FAIL );
        }
    }

    f_poState->m_cchURL = SIZEOF( f_poState->m_rgchURL );

    ChkDR( GenerateLicenseChallenge( f_poAppContext,
                                     f_poState->m_fNeedDomain,
                                     f_poState->m_fDomainExcluded,
                                     f_poState->m_fAESPackage,
                                     f_poState->m_fProtocolFallback,
                                     f_poState->m_ProtocolVersion,
                                     &f_poState->m_oDomainID,
                                     &pbChallenge,
                                     &cbChallenge,
                                     f_poState->m_rgchURL,
                                     &f_poState->m_cchURL ) );

    /* Display the extracted license server URL. */
    LogMessage( "\nLicense server URL (from header of the input file):" );
    LogMessage( f_poState->m_rgchURL );
    LogMessage( "" );

    /* Display the version of the license protocol being used. */
    LogMessage( "\nLicense protocol version: %s",
                f_poState->m_ProtocolVersion == eV2Protocol ? "V2" : "V3" );
    LogMessage( "" );

    ChkDR( LogPacket( TRUE,
                      pbChallenge,
                      cbChallenge ) );

    ChkDR( NetClient( OTADLA_NET_GETLICENSE,
                      pbChallenge,
                      cbChallenge,
                      fUseSoap,
                      f_poState->m_fUseFileUrl ? f_poState->m_rgchURL : NULL,
                      &pbResponse,
                      &cbResponse ) );

    ChkDR( LogPacket( FALSE,
                      pbResponse,
                      cbResponse ) );

    oLicResponse.m_dwResult = DRM_SUCCESS;

    ChkDR( ProcessLicenseChallengeResponse( f_poAppContext,
                                            pbResponse,
                                            cbResponse,
                                            &oLicResponse ) );

    if ( !DRM_SUCCEEDED( oLicResponse.m_dwResult ) )
    {
        ChkDR( HandleLicenseError( f_poAppContext,
                                   f_poState,
                                   &oLicResponse,
                                   pbResponse,
                                   cbResponse ) );
    }
    else
    {
        LogMessage( "Get License (Challenge): Success" );

        if ( ( f_poState->m_ProtocolVersion == eV3Protocol )  &&
             (oLicResponse.m_cbTransactionID > 0))
        {
            ChkDR( GenerateLicenseAck( f_poAppContext,
                                       &oLicResponse,
                                       &pbChallenge,
                                       &cbChallenge ) );

            ChkDR( LogPacket( TRUE,
                              pbChallenge,
                              cbChallenge ) );

            ChkDR( NetClient( OTADLA_NET_ACKLICENSE,
                              pbChallenge,
                              cbChallenge,
                              fUseSoap,
                              NULL,
                              &pbResponse,
                              &cbResponse ) );

            ChkDR( LogPacket( FALSE,
                              pbResponse,
                              cbResponse ) );

            ChkDR( ProcessLicenseAckResponse( f_poAppContext,
                                              pbResponse,
                                              cbResponse ) );

            LogMessage( "Get License (Ack): Success" );
        }


        f_poState->m_PrevState = f_poState->m_State;
        f_poState->m_State = OTADLA_PLAYMUSIC;
    }

ErrorExit:

    SAFE_OEM_FREE( pbChallenge );

    SAFE_OEM_FREE( pbResponse );

    if ( DRM_FAILED( dr ) )
    {
        LogMessage( "Get License (Challenge/Ack): Failed" );
    }

    return dr;
}

/*********************************************************************
**
** Function: ReportMetering
**
** Synopsis: Function that gets executed when the current state of the
**           application is "Report Metering".
**           It constructs a meter certificate challenge and sends it to a
**           metering server using the SOAP protocol. It then processes the
**           meter certificate response from the server. After that it
**           constructs a metering data challenge and sends it to a
**           different metering server without using the SOAP protocol.
**           It then processed the metering data response.
**           Upon exit, it transits into the
**           1. "Leave Domain" if the application has previously joined a domain,
**              and if the input file can be successfully played.
**           2. "Deinit DRM" if the application has not previous joined a domain,
**              and if the input file can be successfully played.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poState]              -- Pointer to the state machine context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL ReportMetering(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN OTADLA_STATE *f_poState )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbChallenge = NULL;
    DRM_DWORD cbChallenge = 0;
    DRM_BYTE *pbResponse = NULL;
    DRM_DWORD cbResponse = 0;
    DRM_METERCERT_ENUM oMeterCertEnumContext1 = { 0 };
    DRM_METERCERT_ENUM oMeterCertEnumContext2 = { 0 };
    DRM_MID oMID1 = { 0 }, oMID2 = { 0 };
    DRM_BYTE *pbMeterCert = NULL;
    DRM_DWORD cbMeterCert = 0;
    DRM_BOOL fIsV2 = FALSE;
    DRM_WCHAR rgwchURL[ MAX_URL_SIZE ] = { 0 };
    DRM_DWORD cchURL = MAX_URL_SIZE;

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_poState != NULL );

    if( !DRM_METERING_IsMeteringSupported() )
    {
        /*
        ** Nothing to report
        */
        goto ErrorExit;
    }

    /*
    ** Retrieve the MID from the meter cert store. The MID information is added
    ** into the meter cert store during the call of Drm_Reader_Commit().
    */
    ChkDR( Drm_MeterCert_InitEnum( f_poAppContext,
                                   &oMeterCertEnumContext1 ) );

    for ( ; ; )
    {
        cchURL = MAX_URL_SIZE;

        dr = Drm_MeterCert_EnumNext( &oMeterCertEnumContext1,
                                     &oMID1,
                                     rgwchURL,
                                     &cchURL,
                                     NULL,
                                     NULL,
                                     NULL );

        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;
            break;
        }

        ChkDR( dr );

        f_poState->m_cchURL = MAX_URL_SIZE;

        /* Convert the metering cert URL from UTF16/UNICODE to UTF8/ASCII */
        ChkDR( DRM_STR_UTF16toUTF8( rgwchURL,
                                    cchURL,
                                    f_poState->m_rgchURL,
                                    0,
                                    &f_poState->m_cchURL ) );

        f_poState->m_rgchURL[ f_poState->m_cchURL ] = 0;

        /* Generate the meter certificate challenge. */
        ChkDR( GenerateMeterCertChallenge( f_poAppContext,
                                           &oMID1,
                                           &pbChallenge,
                                           &cbChallenge ) );

        ChkDR( LogPacket( TRUE,
                          pbChallenge,
                          cbChallenge ) );

        /* Post the metering certificate challenge using the SOAP protocol. */
        ChkDR( NetClient( OTADLA_NET_METERCERT,
                          pbChallenge,
                          cbChallenge,
                          TRUE,
                          NULL,
                          &pbResponse,
                          &cbResponse ) );

        ChkDR( LogPacket( FALSE,
                          pbResponse,
                          cbResponse ) );

        ChkDR( ProcessMeterCertResponse( f_poAppContext,
                                         pbResponse,
                                         cbResponse ) );

        SAFE_OEM_FREE( pbChallenge );

        SAFE_OEM_FREE( pbResponse );

        /*
        ** At this moment, the meter certificate should have been installed
        ** at the meter cert store. Restart the enumeration to retrieve the
        ** installed meter certificate.
        */
        ChkDR( Drm_MeterCert_InitEnum( f_poAppContext,
                                       &oMeterCertEnumContext2 ) );

        for ( ; ; )
        {
            cbMeterCert = 0;

            /* Get the size of the meter certificate. */
            dr = Drm_MeterCert_EnumNext( &oMeterCertEnumContext2,
                                         &oMID2,
                                         NULL,
                                         0,
                                         &fIsV2,
                                         NULL,
                                         &cbMeterCert );

            if ( DRM_FAILED( dr ) )
            {
                if ( dr == DRM_E_BUFFERTOOSMALL )
                {
                    ChkMem( pbMeterCert =
                        ( DRM_BYTE * )Oem_MemAlloc( cbMeterCert ) );

                        /* Retrieve the meter certificate needed to generate a meter data challenge. */
                        ChkDR( Drm_MeterCert_EnumNext( &oMeterCertEnumContext2,
                                                       &oMID2,
                                                       NULL,
                                                       0,
                                                       &fIsV2,
                                                       pbMeterCert,
                                                       &cbMeterCert ) );

                    if ( MEMCMP( &oMID2, &oMID1, SIZEOF( DRM_MID ) ) == 0 )
                    {
                        /*
                        ** Compare whether the current MID of the inner loop
                        ** matches the current MID of the outer loop. If so,
                        ** break from the inner loop.
                        */
                        break;
                    }
                    else
                    {
                        SAFE_OEM_FREE( pbMeterCert );
                        continue;
                    }
                }
                else
                {
                    /*
                    ** Any other types of failure (including DRM_E_NOMORE)
                    ** is a real failure.
                    */
                    ChkDR( dr );
                }
            }
            else
            {
                /* Success is unexpected. */
                ChkDR( DRM_E_FAIL );
            }
        }

        ChkDR( IsV2MeterCert( pbMeterCert, cbMeterCert, &fIsV2 ) );

        /* Generate the metering data challenge. */
        dr = GenerateMeterDataChallenge( f_poAppContext,
                                         pbMeterCert,
                                         cbMeterCert,
                                         fIsV2,
                                         f_poState->m_rgchURL,
                                         &f_poState->m_cchURL,
                                         &pbChallenge,
                                         &cbChallenge );

        if ( dr == DRM_E_NOMORE )
        {
            /*
            ** There is no metering data to report for the current MID.
            ** Just loop for the next MID.
            */
            ChkDR( DRM_SUCCESS );
            continue;
        }
        else
        {
            ChkDR( dr );
        }

        f_poState->m_rgchURL[ f_poState->m_cchURL ] = 0;

        LogMessage( "\nMetering server URL (from meter certificate): %s", f_poState->m_rgchURL );
        LogMessage( "" );

        ChkDR( LogPacket( TRUE,
                          pbChallenge,
                          cbChallenge ) );

        /* Post the metering data challenge. */
        ChkDR( NetClient( OTADLA_NET_METERDATA,
                          pbChallenge,
                          cbChallenge,
                          !fIsV2,
                          f_poState->m_rgchURL,
                          &pbResponse,
                          &cbResponse ) );

        ChkDR( LogPacket( FALSE,
                          pbResponse,
                          cbResponse ) );

        /*
        ** Process the metering data response to clear the metering
        ** records in the local HDS.
        */
        ChkDR( ProcessMeterDataResponse( f_poAppContext,
                                         pbResponse,
                                         cbResponse,
                                         fIsV2 ) );

        SAFE_OEM_FREE( pbMeterCert );

        SAFE_OEM_FREE( pbChallenge );

        SAFE_OEM_FREE( pbResponse );
    }

    f_poState->m_PrevState = f_poState->m_State;

    if ( f_poState->m_fNeedDomain )
    {
        f_poState->m_State = OTADLA_LEAVEDOMAIN;
    }
    else
    {
        f_poState->m_State = OTADLA_DEINITDRM;
    }

    LogMessage( "Report Metering: Success" );

ErrorExit:

    SAFE_OEM_FREE( pbMeterCert );
    SAFE_OEM_FREE( pbChallenge );
    SAFE_OEM_FREE( pbResponse );

    if ( DRM_FAILED( dr ) )
    {
        LogMessage( "Report Metering: Failed" );
    }

    return dr;
}

/*********************************************************************
**
** Function: PlayMusic
**
** Synopsis: Function that gets executed when the current state of the
**           application is "Play Music".
**           It opens a music file, binds it to a license and decrypts it.
**           After that it lauches an external player application to play
**           the decrypted file.
**           Upon exit, it transits into the
**           1. "Report Metering" if the input file can be successfully played.
**           2. "Get License" if the input file cannot be successfully played and
**              the error from the license binding is "License Not Found".
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poState]              -- Pointer to the state machine context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL if an input file cannot be opened.
**
**********************************************************************/
DRM_RESULT DRM_CALL PlayMusic(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN OTADLA_STATE *f_poState )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE rgbBuffer[ __CB_DECL( MAX_PLAY_BUFFER_SIZE ) ] = { 0 };
    OEM_FILEHDL Outfp = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD cbRead = 0, cbWritten = 0;

    DRM_DECRYPT_CONTEXT oDecryptContext = { 0 };
    const DRM_CONST_STRING *rgpdstrRights[ 1 ];

    ChkArg( f_poAppContext != NULL );
    ChkArg( f_poState != NULL );

    if ( f_poState->m_pdstrDecryptedFile != NULL )
    {
        Outfp = Oem_File_Open( NULL,
                              f_poState->m_pdstrDecryptedFile->pwszString,
                              OEM_GENERIC_WRITE,
                              OEM_FILE_SHARE_NONE,
                              OEM_CREATE_ALWAYS,
                              OEM_ATTRIBUTE_NORMAL );

        if ( Outfp == OEM_INVALID_HANDLE_VALUE )
        {
            LogMessage( "Play Music: Failed to write file %S.",
                        f_poState->m_pdstrDecryptedFile->pwszString );

            ChkDR( DRM_E_FAIL );
        }
    }

    /*
    ** We need to Reinitialize the App context before setting the header.
    */
    ChkDR( Drm_Reinitialize( f_poAppContext ) );

    if ( !f_poState->m_fHeaderOnly )
    {
        ChkDR( SetContentHeaderUsingMediaFile( f_poAppContext, f_poState->m_poMediaFile ) );
    }
    else
    {
        ChkDR( SetContentHeaderUsingRMHeader( f_poAppContext, f_poState->m_pdstrEncryptedFile ) );
    }

    if(  f_poState->m_fExecute )
    {
        LogMessage( "Play Music: Using Execute Right.\n" );
        rgpdstrRights[ 0 ] = ( DRM_CONST_STRING * )&g_dstrExecuteRight;
    }
    else
    {
        rgpdstrRights[ 0 ] = ( DRM_CONST_STRING * )&g_dstrWMDRM_RIGHT_PLAYBACK;
    }

    dr = Drm_Reader_Bind( f_poAppContext,
                          rgpdstrRights,
                          NO_OF( rgpdstrRights ),
                          PolicyCallback,
                          NULL,
                          &oDecryptContext );

    if( DRM_FAILED( dr ) )
    {
        if ( dr == DRM_E_LICENSENOTFOUND )
        {
            if( f_poState->m_State != OTADLA_GETLICENSE )
            {
                f_poState->m_PrevState = f_poState->m_State;
                f_poState->m_State = OTADLA_GETLICENSE;

                dr = DRM_SUCCESS;

                LogMessage( "Play Music: Need a license." );
            }
            else
            {
                /*
                ** Just tried and failed to get a license.
                */

                LogMessage( "Play Music: No License found, we already tried to get one. " );
                ChkDR( dr );
            }
        }
        else if ( dr == DRM_E_DOMAIN_BIND_LICENSE )
        {
            f_poState->m_PrevState = f_poState->m_State;
            f_poState->m_State = OTADLA_JOINDOMAIN;

            dr = DRM_SUCCESS;

            LogMessage( "Play Music: Has license. Need to join domain." );
        }
        else if(  dr == DRM_E_RIV_TOO_SMALL
               || dr == DRM_E_LICEVAL_REQUIRED_REVOCATION_LIST_NOT_AVAILABLE )
        {
            f_poState->m_PrevState = f_poState->m_State;
            f_poState->m_State = OTADLA_UPDATEREVOCATION;

            dr = DRM_SUCCESS;

            LogMessage( "Play Music: Revocation data required." );
        }
    }
    else
    {
        if ( !f_poState->m_fHeaderOnly )
        {
            if( f_poState->m_fEnvelopeFile )
            {
                ChkDR( Drm_Envelope_InitializeRead( &f_poState->m_poMediaFile->FormatContext.oEnvFile, &oDecryptContext ) );

                do
                {
                    ChkDR( Drm_Envelope_Read( &f_poState->m_poMediaFile->FormatContext.oEnvFile,
                                              rgbBuffer,
                                              SIZEOF( rgbBuffer ),
                                              &cbRead ) );

                    if ( f_poState->m_pdstrDecryptedFile != NULL )
                    {
                        if ( !Oem_File_Write(Outfp, rgbBuffer, cbRead, &cbWritten) ||
                             cbWritten != cbRead )
                        {
                            ChkDR( DRM_E_FAIL );
                        }
                    }
                } while( cbRead == SIZEOF( rgbBuffer ) );

                LogMessage( "Play Music: File decrypted." );

#if DEMO_LAUNCH_PLAYER
                LaunchPlayer( f_poState );

                LogMessage( "Play Music: Player launched." );
#endif
            }
        }

        /* Commit the playback to record the metering information if it exists. */
        ChkDR( Drm_Reader_Commit( f_poAppContext,
                                  PolicyCallback,
                                  NULL ) );

        f_poState->m_PrevState = f_poState->m_State;

        f_poState->m_State = OTADLA_REPORTMETERING;

        LogMessage( "Play Music: Success" );
    }

ErrorExit:

    if ( Outfp != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( Outfp );
    }

    if ( DRM_FAILED( dr ) )
    {
        LogMessage( "Play Music: Failed" );
    }

    return dr;
}

DRM_RESULT DRM_CALL GetRevocationPackage(
    IN DRM_APP_CONTEXT *f_poAppContext,
    IN OTADLA_STATE *f_poState )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_BYTE *pbResponse = NULL;
    DRM_DWORD cbResponse = 0;

    ChkArg( f_poAppContext != NULL );

    LogMessage( "\nDownloading Revocation Package from Microsoft Server..." );

    ChkDR( NetClient( OTADLA_NET_REVOCATIONPACKAGE,
                      NULL,
                      0,
                      FALSE,
                      NULL,
                      &pbResponse,
                      &cbResponse ) );

    ChkDR( LogPacket( FALSE,
                      pbResponse,
                      cbResponse ) );

    ChkDR( Drm_Revocation_StorePackage( f_poAppContext, ( DRM_CHAR * )pbResponse, cbResponse ) );

    if( f_poState != NULL )
    {
        f_poState->m_State = f_poState->m_PrevState;
        f_poState->m_PrevState = OTADLA_UPDATEREVOCATION;
    }

ErrorExit:

    SAFE_OEM_FREE( pbResponse );

    if ( DRM_FAILED( dr ) )
    {
        LogMessage( "Get Revocation Package: Failed" );
    }

    return dr;
}


/*********************************************************************
**
** Function: InitDRM
**
** Synopsis: Function that gets executed when the current state of the
**           application is "DRM Init".
**           It allocates and initializes resources for the DRM engine
**           and transits into the "Play Music" state.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poState]              -- Pointer to the state machine context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL InitDRM(
    IN  OTADLA_STATE     *f_poState,
    OUT DRM_BYTE        **f_ppbOpaqueBuffer,
    OUT DRM_APP_CONTEXT **f_ppoAppContext )
{
    DRM_RESULT       dr                = DRM_SUCCESS;
    DRM_CONST_STRING dstrDataStoreFile = CREATE_DRM_STRING( HDS_STORE_FILE );

    ChkArg( *f_ppoAppContext == NULL );

    if( f_poState->m_pdstrDecryptedFile             != NULL
     && f_poState->m_pdstrDecryptedFile->pwszString != NULL )
    {
        /* Delete the decrypted output file if it exists. */
        ( DRM_VOID )Oem_File_Delete( f_poState->m_pdstrDecryptedFile->pwszString );
    }

    if( !f_poState->m_fJoinDomainOnly
     && !f_poState->m_fLeaveDomainOnly
     && NULL == f_poState->m_pdstrHDSFileName )
    {
        /* Delete the HDS file if it exists. */
        ( DRM_VOID )Oem_File_Delete( HDS_STORE_FILE );
    }

    Oem_Clock_SetResetState( NULL, FALSE );

    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, NULL == f_poState->m_pdstrHDSFileName ? &dstrDataStoreFile : f_poState->m_pdstrHDSFileName, TOOLS_DRM_BUFFER_SIZE, f_ppbOpaqueBuffer, f_ppoAppContext ) );

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        ChkMem( f_poState->m_pbRevocationBuffer =
                    ( DRM_BYTE * )Oem_MemAlloc( REVOCATION_BUFFER_SIZE ) );

        ChkDR( Drm_Revocation_SetBuffer( *f_ppoAppContext,
                                         f_poState->m_pbRevocationBuffer,
                                         REVOCATION_BUFFER_SIZE ) );
    }

    f_poState->m_PrevState = f_poState->m_State;

    if( f_poState->m_fJoinDomainOnly )
    {
        f_poState->m_State = OTADLA_JOINDOMAIN;
    }
    else if( f_poState->m_fLeaveDomainOnly )
    {
        f_poState->m_State = OTADLA_LEAVEDOMAIN;
    }
    else
    {
        f_poState->m_State = OTADLA_PLAYMUSIC;
    }

    LogMessage( "Init DRM: Success" );

ErrorExit:

    if ( DRM_FAILED( dr ) )
    {
        LogMessage( "Init DRM: Failed" );
    }

    return dr;
}

/*********************************************************************
**
** Function: DeinitDRM
**
** Synopsis: Function that gets executed when the current state of the
**           application is "DRM Deinit".
**           It frees up resources allocated for the DRM engine and transits
**           into the "Exit" state.
**
** Arguments:
**
** [f_poAppContext]         -- Pointer to a DRM App context initialized via
**                             Drm_Initialize().
** [f_poState]              -- Pointer to the state machine context.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************/
DRM_RESULT DRM_CALL DeinitDRM(
    IN DRM_APP_CONTEXT **f_ppoAppContext,
    IN DRM_BYTE        **f_ppbOpaqueBuffer,
    IN OTADLA_STATE     *f_poState )
{
    SAFE_OEM_FREE( f_poState->m_pbRevocationBuffer );

    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( f_ppbOpaqueBuffer, f_ppoAppContext );

    f_poState->m_PrevState = f_poState->m_State;
    f_poState->m_State     = OTADLA_EXIT;

    LogMessage( "Deinit DRM: Success" );

    return DRM_SUCCESS;
}


/**********************************************************************************
**
** Function: ParseHttpParams
**
** Synopsis: Parses server name, port number and page URL from a connection string.
**           Example: http://rmserver.contoso.com:8080/dir/page.asmx
**
** Arguments:
**
** [f_pdstrURL]     -- pointer to parameter string, if be NULL or zero length
**                     then do nothing.
**
** [f_poHttpParams] -- Pointer to structure initialized with default HTTP connection
**                     parameters. If function succeeds the structure's data will be
**                     overwritten.
**
** Returns:            DRM_SUCCESS on success.
**                     DRM_E_INVALIDARG if the arguments are invalid.
**
**********************************************************************************/
DRM_RESULT DRM_CALL ParseHttpParams(
    IN DRM_CONST_STRING *f_pdstrURL,
    IN OUT OTADLA_HttpParams *f_poHttpParams )
{
    DRM_RESULT      dr             = DRM_SUCCESS;
    URL_COMPONENTSA oUrlComponents = { 0 };
    DRM_ANSI_STRING dstrUTF8URL   = EMPTY_DRM_STRING;

    ChkDRMString( f_pdstrURL );
    ChkArg( f_poHttpParams != NULL );

    dr = DRM_STR_UTF16toUTF8( f_pdstrURL->pwszString, f_pdstrURL->cchString, NULL, 0, &dstrUTF8URL.cchString );
    ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, dr );

    ChkMem( dstrUTF8URL.pszString = ( DRM_CHAR * )Oem_MemAlloc( dstrUTF8URL.cchString ) );

    /* Convert the UTF-16 string into UTF8/ANSI before sending to the server. */
    ChkDR( DRM_STR_UTF16toUTF8( f_pdstrURL->pwszString,
                                f_pdstrURL->cchString,
                                dstrUTF8URL.pszString,
                                0,
                               &dstrUTF8URL.cchString ) );

    ZEROMEM( &oUrlComponents, SIZEOF( URL_COMPONENTSA ) );

    oUrlComponents.dwStructSize = SIZEOF( URL_COMPONENTSA );
    oUrlComponents.lpszHostName = f_poHttpParams->m_rgchServerName;
    oUrlComponents.dwHostNameLength = SIZEOF( f_poHttpParams->m_rgchServerName );
    oUrlComponents.lpszUrlPath = f_poHttpParams->m_rgchURL;
    oUrlComponents.dwUrlPathLength = SIZEOF( f_poHttpParams->m_rgchURL );

    /* Parse the URL string into components. */
    ChkBOOL( InternetCrackUrlA( dstrUTF8URL.pszString,
                                0,
                                0,
                                &oUrlComponents ), DRM_E_FAIL );

    f_poHttpParams->m_dwHttpPort = oUrlComponents.nPort;

ErrorExit:
    SAFE_OEM_FREE( dstrUTF8URL.pszString );

    if ( DRM_FAILED( dr ) )
    {
        LogMessage( "Invalid HTTP connection parameter: %S", f_pdstrURL->pwszString );
    }

    return dr;
}

/*********************************************************************
**
** Function: DoProcess
**
** Synopsis: Function that runs the states machine.
**
** Arguments:
**
** [f_pdstrEncryptedFile]   -- Pointer to a DRM string that contains
**                             the name of the input file to be decrypted.
**                             if f_fHeaderOnly is true, the content of the
**                             file is actually a RM header.
** [f_pdstrDecryptedFile]   -- Pointer to a DRM string that contains
**                             the name of the decrypted output file.
** [f_pdstrHDSFileName]     -- Pointer to a DRM string that contains
**                             the name of the HDS File to use. If NULL,
**                             the default is used.
** [f_fPause]               -- Flag indicating whether there should be a
**                             pause between state transition. Pause can be
**                             used to allow using tools to evaluate the
**                             states of the HDS storage.
** [f_fNeedDomain]          -- Flag indicating whether a domain membership
**                             is required to get a license from the server.
** [f_fJoinDomainOnly]      -- Flag indicating whether to join the domain and
**                             then exit.
** [f_fLeaveDomainOnly]     -- Flag indicating whether a leave the domain and
**                             then exit.
** [f_fAESPackage]          -- Flag indicating whether the input file
**                             is packaged using AES cipher.
** [f_fEnvelopeFile]        -- Flag indicating whether the input file
**                             is packaged in an envelope file.
** [f_fUseFileUrl]          -- Flag indicating whether to use the
**                             RMSDK URL in the input file for LicAcq.
** [f_fProtocolFallback]    -- Flag indicating whether to demonstrate the
**                             license protocol fallback scenario.
** [f_fHeaderOnly]          -- Flag indicating whether the content of the
**                             input file contains RM header only.
** [f_fExecuteRight]        -- Flag indicating to use the Exeute Right.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FAIL if unexpected state occurrs.
**
**********************************************************************/
DRM_RESULT DRM_CALL DoProcess(
    IN DRM_CONST_STRING       *f_pdstrEncryptedFile,
    IN DRM_MEDIA_FILE_CONTEXT *f_poMediaFile,
    IN DRM_CONST_STRING       *f_pdstrDecryptedFile,
    IN DRM_CONST_STRING       *f_pdstrHDSFileName,
    IN DRM_BOOL                f_fPause,
    IN DRM_BOOL                f_fNeedDomain,
    IN DRM_BOOL                f_fJoinDomainOnly,
    IN DRM_BOOL                f_fLeaveDomainOnly,
    IN DRM_BOOL                f_fRevocationOnly,
    IN DRM_BOOL                f_fAESPackage,
    IN DRM_BOOL                f_fEnvelopeFile,
    IN DRM_BOOL                f_fUseFileUrl,
    IN DRM_BOOL                f_fProtocolFallback,
    IN DRM_BOOL                f_fHeaderOnly,
    IN DRM_BOOL                f_fExecuteRight )
{
    DRM_RESULT       dr              = DRM_SUCCESS;
    DRM_APP_CONTEXT *poAppContext    = NULL;
    DRM_BYTE        *pbOpaqueBuffer  = NULL;
    OTADLA_STATE     oState          = { OTADLA_ENTER };
    DRM_BOOL         fExit           = FALSE;

    /*
    ** Only check the two file names if we aren't doing a
    ** RevocationOnly, JoinDomainOnly, LeaveDomainOnly or HeaderOnly.
    */
    if( !f_fJoinDomainOnly && !f_fLeaveDomainOnly && !f_fRevocationOnly && !f_fHeaderOnly )
    {
        ChkDRMString( f_pdstrEncryptedFile );
        /*
        ** If there is an input file, it should have already been opened.
        */
        ChkBOOL( f_poMediaFile != NULL && f_poMediaFile->fOpened, DRM_E_FILEREADERROR );

        /*
        ** Only Check the output file string if there is an output file.
        */
        if( f_pdstrDecryptedFile != NULL )
        {
            ChkDRMString( f_pdstrDecryptedFile );
        }
    }

    ZEROMEM( &oState, SIZEOF( OTADLA_STATE ));

    oState.m_pdstrEncryptedFile = f_pdstrEncryptedFile;
    oState.m_poMediaFile        = f_poMediaFile;
    oState.m_pdstrDecryptedFile = f_pdstrDecryptedFile;
    oState.m_pdstrHDSFileName   = f_pdstrHDSFileName;
    oState.m_PrevState          = OTADLA_ENTER;
    oState.m_State              = OTADLA_INITDRM;
    oState.m_fNeedDomain        = f_fNeedDomain;
    oState.m_fJoinDomainOnly    = f_fJoinDomainOnly;
    oState.m_fLeaveDomainOnly   = f_fLeaveDomainOnly;
    oState.m_fAESPackage        = f_fAESPackage;
    oState.m_fEnvelopeFile      = f_fEnvelopeFile;
    oState.m_fUseFileUrl        = f_fUseFileUrl;
    oState.m_fDomainExcluded    = ( f_fNeedDomain && DEMO_DOMAIN_EXCLUSION );
    oState.m_fProtocolFallback  = f_fProtocolFallback;
    oState.m_ProtocolVersion    = eV3Protocol;
    oState.m_fHeaderOnly        = f_fHeaderOnly;
    oState.m_fExecute           = f_fExecuteRight;

    /*
    ** If we are only leaving a domain, force the global state
    ** to think we joined a domain already.
    */
    if( f_fLeaveDomainOnly )
    {
        oState.m_fInDomain = TRUE;
    }

    LogMessage( "Server HTTP connection parameters: http://%s:%d%s",
                g_oServer.m_rgchServerName,
                g_oServer.m_dwHttpPort,
                g_oServer.m_rgchURL );

    LogMessage( "Fallback HTTP connection parameters: http://%s:%d%s",
                g_oFallback.m_rgchServerName,
                g_oFallback.m_dwHttpPort,
                g_oFallback.m_rgchURL );

    while ( !fExit )
    {
        if ( f_fPause )
        {
            printf("\nPress <Return> to continue...\n");

            getchar();
        }

        if( f_fRevocationOnly )
        {
            ChkDR( InitDRM( &oState, &pbOpaqueBuffer, &poAppContext ) );
            ChkDR( GetRevocationPackage( poAppContext, &oState ) );
            ChkDR( DeinitDRM( &poAppContext, &pbOpaqueBuffer, &oState ) );
            break;
        }
        else
        {
            LogMessage( "\n------------- Transit from [%s] to [%s] -------------\n",
                        g_StateNames[ oState.m_PrevState ],
                        g_StateNames[ oState.m_State ] );

            switch ( oState.m_State )
            {
                case OTADLA_INITDRM:
                            ChkDR( InitDRM( &oState, &pbOpaqueBuffer, &poAppContext ) );
                            break;

                case OTADLA_PLAYMUSIC:
                            ChkDR( PlayMusic( poAppContext, &oState ) );
                            break;

                case OTADLA_GETLICENSE:
                            ChkDR( GetLicense( poAppContext, &oState ) );
                            break;

                case OTADLA_JOINDOMAIN:
                            ChkDR( JoinDomain( poAppContext, &oState ) );
                            break;

                case OTADLA_LEAVEDOMAIN:
                            ChkDR( LeaveDomain( poAppContext, &oState ) );
                            break;

                case OTADLA_REPORTMETERING:
                            ChkDR( ReportMetering( poAppContext, &oState ) );
                            break;

                case OTADLA_UPDATEREVOCATION:
                            ChkDR( GetRevocationPackage( poAppContext, &oState ) );
                            break;

                case OTADLA_DEINITDRM:
                            ChkDR( DeinitDRM( &poAppContext, &pbOpaqueBuffer, &oState ) );
                            break;

                case OTADLA_EXIT:
                            fExit = TRUE;
                            break;

                default:
                            ChkDR( DRM_E_FAIL );
                            break;
            }
        }
    }

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: DRM_Main
**
** Synopsis: The main function.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- An array of arguments.
**
** Returns:                 DRM_SUCCESS on success.
**
**********************************************************************/
DRM_LONG DRM_CALL DRM_Main( IN DRM_LONG argc, IN DRM_WCHAR *argv[] )
{
    DRM_RESULT              dr                = DRM_SUCCESS;
    DRM_LONG                i                 = 0;
    DRM_WCHAR               wchOption         = 0;
    DRM_BOOL                fPause            = FALSE;
    DRM_BOOL                fNeedDomain       = FALSE;
    DRM_BOOL                fJoinDomainOnly   = FALSE;
    DRM_BOOL                fLeaveDomainOnly  = FALSE;
    DRM_BOOL                fAESPackage       = FALSE;
    DRM_BOOL                fEnvelopeFile     = TRUE;
    DRM_BOOL                fProtocolFallback = FALSE;
    DRM_BOOL                fRevocationOnly   = FALSE;
    DRM_BOOL                fUseFileUrl       = TRUE;
    DRM_BOOL                fHasOutputFile    = FALSE;
    DRM_BOOL                fHasSpecificHDS   = FALSE;
    DRM_BOOL                fHeaderOnly       = FALSE;
    DRM_BOOL                fExecuteRight     = FALSE;
    DRM_STRING              dstrParam         = EMPTY_DRM_STRING;
    DRM_CONST_STRING        dstrEncryptedFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING        dstrDecryptedFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING        dstrHDSFileName   = EMPTY_DRM_STRING;
    DRM_MEDIA_FILE_CONTEXT *poMediaFile       = NULL;

    PrintCopyrightInfo( "Sample of Over-the-Air Dynamic License Acquisition" );

    if( DRM_DLA_IsDLASupported() )
    {
        if ( argc <= 1 )
        {
            dr = DRM_S_MORE_DATA;
            goto _PrintUsage;
        }

        for ( i = 1; i < argc; i++ )
        {
            if ( !DRM_CMD_ParseCmdLine( argv[ i ],
                                        &wchOption,
                                        ( DRM_CONST_STRING * )&dstrParam,
                                        NULL ) )
            {
                /* See if it's a default parameter that PK knows how to handle */
                if ( DRM_FAILED( DRM_CMD_TryProcessDefaultOption( argv[i] ) ) )
                {
                    dr = DRM_E_INVALID_COMMAND_LINE;
                    goto _PrintUsage;
                }
            }
            else
            {
                wchOption = DRMCRT_towlower( wchOption );

                switch ( wchOption )
                {
                    case WCHAR_CAST( 'i' ):
                        dstrEncryptedFile.pwszString = dstrParam.pwszString;
                        dstrEncryptedFile.cchString  = dstrParam.cchString;
                        break;

                    case WCHAR_CAST( 'o' ):
                        fHasOutputFile = TRUE;
                        dstrDecryptedFile.pwszString = dstrParam.pwszString;
                        dstrDecryptedFile.cchString  = dstrParam.cchString;
                        break;

                    case WCHAR_CAST( 't' ):
                        fHasSpecificHDS = TRUE;
                        dstrHDSFileName.pwszString = dstrParam.pwszString;
                        dstrHDSFileName.cchString  = dstrParam.cchString;
                        break;

                    case WCHAR_CAST( 's' ):
                        fUseFileUrl = FALSE;
                        ChkDR( ParseHttpParams( ( DRM_CONST_STRING * )&dstrParam, &g_oServer ) );
                        break;

                    case WCHAR_CAST( 'p' ):
                        fPause = TRUE;
                        break;

                    case WCHAR_CAST( 'd' ):
                        fNeedDomain = TRUE;
                        break;

                    case WCHAR_CAST( 'j' ):
                        fJoinDomainOnly = TRUE;
                        break;

                    case WCHAR_CAST( 'l' ):
                        fLeaveDomainOnly = TRUE;
                        break;

                    case WCHAR_CAST( 'r' ):
                        fRevocationOnly = TRUE;
                        break;

                    case WCHAR_CAST( 'u' ):
                        fUseFileUrl = FALSE;
                        break;

                    case WCHAR_CAST( 'e' ):
                        fExecuteRight = TRUE;
                        break;

                    case WCHAR_CAST( 'f' ):
                        fProtocolFallback = TRUE;
                        if ( dstrParam.cchString > 0 )
                        {
                            ChkDR( ParseHttpParams( ( DRM_CONST_STRING * )&dstrParam, &g_oFallback ) );
                        }
                        break;

                    case WCHAR_CAST( 'h' ):
                        fHeaderOnly = TRUE;
                        break;

                    case WCHAR_CAST( 'c' ):
                        if ( g_dwCustomData < MAX_NUM_CUSTOM_DATA )
                        {
                            dr = DRM_STR_UTF16toUTF8( dstrParam.pwszString,
                                                      dstrParam.cchString,
                                                      NULL,
                                                      0,
                                                     &g_customData[ g_dwCustomData ].cchString );

                            ChkBOOL( dr == DRM_E_BUFFERTOOSMALL, dr );

                            ChkMem( g_customData[ g_dwCustomData ].pszString = ( DRM_CHAR * )Oem_MemAlloc( g_customData[ g_dwCustomData ].cchString ) );

                            /* Convert the UTF-16 string into UTF8/ANSI before sending to the server. */
                            ChkDR( DRM_STR_UTF16toUTF8( dstrParam.pwszString,
                                                        dstrParam.cchString,
                                                        g_customData[ g_dwCustomData ].pszString,
                                                        0,
                                                       &g_customData[ g_dwCustomData ].cchString ) );
                            g_dwCustomData++;
                        }
                        break;

                    case WCHAR_CAST( '?' ):
                        goto _PrintUsage;

                    default:
                        dr = DRM_E_INVALID_COMMAND_LINE;
                        goto _PrintUsage;
                }
            }
        }

        if( fRevocationOnly )
        {
            if( fJoinDomainOnly || fLeaveDomainOnly )
            {
                LogMessage( "\nCannot use JoinDomainOnly or LeaveDomainOnly with Revocation Only.\n" );

                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }
        }
        else
        {
            if ( fJoinDomainOnly && fLeaveDomainOnly )
            {
                LogMessage( "\nCannot use JoinDomainOnly and LeaveDomainOnly at the same time.\n" );

                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }

            if( dstrEncryptedFile.pwszString == NULL )
            {
                LogMessage( "Input file: <not specified>" );
            }
            else
            {
                LogMessage( "Input file: %S", dstrEncryptedFile.pwszString );

                if ( !fHeaderOnly )
                {
                    ChkMem( poMediaFile = (DRM_MEDIA_FILE_CONTEXT *) Oem_MemAlloc( SIZEOF( DRM_MEDIA_FILE_CONTEXT ) ) );
                    ZEROMEM( poMediaFile, SIZEOF( DRM_MEDIA_FILE_CONTEXT ) );

                    ChkDR( DRMTOOLS_OpenMediaFile( &dstrEncryptedFile, poMediaFile ) );

                    if ( poMediaFile->eFileType == eDRM_MEDIA_FILE_TYPE_ENVELOPE )
                    {
                        fEnvelopeFile = TRUE;
                        switch ( poMediaFile->FormatContext.oEnvFile.oEnvHeader.dwCipherType )
                        {
                            case eDRM_AES_COUNTER_CIPHER:
                                fAESPackage = TRUE;
                                break;

                            case eDRM_RC4_CIPHER:
                                fAESPackage = FALSE;
                                break;

                            default:
                                ChkDR( DRM_E_ENVELOPE_FILE_NOT_COMPATIBLE );
                        }
                    }
                    else if ( poMediaFile->eFileType == eDRM_MEDIA_FILE_TYPE_ASF )
                    {
                        fEnvelopeFile = FALSE;
                        fAESPackage = TRUE;
                    }

                    if ( fAESPackage != TRUE )
                    {
                        LogMessage( "Input file: %S not supported - is not encrypted using AES.\n", dstrEncryptedFile.pwszString );
                        ChkBOOL (fAESPackage, DRM_E_FAIL );
                    }

                    if ( fAESPackage && fProtocolFallback )
                    {
                        LogMessage( "\nCannot use AES packaged file for the license fallback scenario.\n" );

                        dr = DRM_E_INVALID_COMMAND_LINE;
                        goto _PrintUsage;
                    }
                }
            }

            if( dstrDecryptedFile.pwszString == NULL )
            {
                LogMessage( "Output file: <not specified>" );
            }
            else
            {
                LogMessage( "Output file: %S", dstrDecryptedFile.pwszString );
            }
        }

#if DRM_SUPPORT_PROFILING

        ChkDR( DRM_Start_Profiling( NULL, NULL ) );

#endif

        DRM_TRACE_START( NULL, NULL );

        /*
        ** Note: set global trace level to or above TRACE_INFO will turn on scope enter/leave tracing,
        ** which will significantly increase the size of the trace log.
        */
        DRM_GLOBAL_TRACE_LEVEL( TRACE_WARNING );

        DRM_TRACE_CHANNEL_ON( TRACE_FILE_CHANNEL );

        ChkDR( DoProcess( &dstrEncryptedFile,
                          poMediaFile,
                          fHasOutputFile  ? &dstrDecryptedFile : NULL,
                          fHasSpecificHDS ? &dstrHDSFileName   : NULL,
                          fPause,
                          fNeedDomain,
                          fJoinDomainOnly,
                          fLeaveDomainOnly,
                          fRevocationOnly,
                          fAESPackage,
                          fEnvelopeFile,
                          fUseFileUrl,
                          fProtocolFallback,
                          fHeaderOnly,
                          fExecuteRight ) );
    }
    else
    {
        printf( "\nFailure: OTADLAclient is compiled without the real dla library linked to it.\n" );
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:

    DRM_TRACE_STOP();

#if DRM_SUPPORT_PROFILING

    DRM_Stop_Profiling();

#endif

    if( poMediaFile != NULL && poMediaFile->fOpened )
    {
        DRMTOOLS_CloseMediaFile( poMediaFile );
    }

    SAFE_OEM_FREE( poMediaFile );

    for ( i = 0; i < ( DRM_LONG )g_dwCustomData; i++ )
    {
        SAFE_OEM_FREE( g_customData[ i ].pszString );
    }

    if ( DRM_SUCCEEDED( dr ) )
    {
        printf( "Success!\r\n" );
    }
    else
    {
        printf( "Failed with error code: 0x%X\r\n.", dr );
    }

    if ( fPause )
    {
        printf("\nPress <Return> to exit...\n");
        getchar();
    }

    return dr;

_PrintUsage:

    PrintUsage();
    return dr;
}
