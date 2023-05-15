/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdio.h>
#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <drmcmdlnpars.h>
#include <drmtoolsutils.h>
#include <drmplayreadyobj.h>
#include <drmtoolsmediafile.h>
#include <drmtoolsconstants.h>
#include <drmxmlparser.h>
#include <drmxmr.h>

/*********************************************************************
**
** Function: _PrintUsage
**
** Synopsis: Function that shows how the command line syntax of using
**           the application.
**
** Arguments:
**
** [f_pwszAppName]          -- Pointer to a wide character string of the
**                             application executable.
**
** Returns:                 N/A
**
**********************************************************************/
static DRM_VOID _PrintUsage(
    __in const DRM_WCHAR * f_pwszAppName )
{
    printf( "Syntax: %S [-?] { -c:ContentFile | Filename } [-h:HeaderFile]\n", f_pwszAppName );
}

/**********************************************************************
**
** Function: _DateTimeToString
**
** Synopsis: Converts a UINT64 to a Date String
**
** Arguments:
**
** [f_u64DateTime]          -- DRM_UINT64 with a date
** [f_pdstrDateTime]        -- DRM_STRING to hold the date. must be at least 24 characters.
**
** Returns:                 DRM_E_INVALIDARG -- f_pdstrDateTime is incorrect
**                          DRM_E_FAIL       -- The Conversion Fails.
**                          DRM_SUCCESS      -- Otherwise
**
**********************************************************************/
static DRM_RESULT _DateTimeToString( DRM_UINT64 f_u64DateTime, DRM_STRING *f_pdstrDateTime )
{
    DRM_RESULT    dr       = DRM_SUCCESS;
    DRMFILETIME   filetime = { 0, 0 };
    DRMSYSTEMTIME date     = { 0 } ;

    ChkArg( f_pdstrDateTime != NULL );
    ChkArg( f_pdstrDateTime->pwszString != NULL );
    ChkArg( f_pdstrDateTime->cchString >= 24 );

    UI64_TO_FILETIME( f_u64DateTime, filetime );

    if( Oem_Clock_FileTimeToSystemTime(&filetime, &date) )
    {
        DRM_WCHAR *pwchCurr  = f_pdstrDateTime->pwszString;
        DRM_DWORD  cchRBuffer = f_pdstrDateTime->cchString;

        ChkDR( DRM_UTL_NumberToString( date.wMonth, pwchCurr, cchRBuffer + 1, 2, 10, &cchRBuffer ) );
        pwchCurr += 2;
        *pwchCurr = WCHAR_CAST( '/' );
        pwchCurr++;

        ChkDR( DRM_UTL_NumberToString( date.wDay, pwchCurr, cchRBuffer + 1, 2, 10, &cchRBuffer ) );
        pwchCurr += 2;
        *pwchCurr = WCHAR_CAST( '/' );
        pwchCurr++;

        ChkDR( DRM_UTL_NumberToString( date.wYear, pwchCurr, cchRBuffer + 1, 4, 10, &cchRBuffer ) );
        pwchCurr += 4;
        *pwchCurr = WCHAR_CAST( ' ' );
        pwchCurr++;

        ChkDR( DRM_UTL_NumberToString( date.wHour, pwchCurr, cchRBuffer + 1, 2, 10, &cchRBuffer ) );
        pwchCurr += 2;
        *pwchCurr = WCHAR_CAST( ':' );
        pwchCurr++;

        ChkDR( DRM_UTL_NumberToString( date.wMinute, pwchCurr, cchRBuffer + 1, 2, 10, &cchRBuffer ) );
        pwchCurr += 2;
        *pwchCurr = WCHAR_CAST( ':' );
        pwchCurr++;

        ChkDR( DRM_UTL_NumberToString( date.wSecond, pwchCurr, cchRBuffer + 1, 2, 10, &cchRBuffer ) );
        pwchCurr += 2;
        *pwchCurr = WCHAR_CAST( ' ' );
        pwchCurr++;

        *pwchCurr = WCHAR_CAST( 'G' );
        pwchCurr++;
        *pwchCurr = WCHAR_CAST( 'M' );
        pwchCurr++;
        *pwchCurr = WCHAR_CAST( 'T' );
        pwchCurr++;
        *pwchCurr = WCHAR_CAST( '\0' );
        pwchCurr++;

        f_pdstrDateTime->cchString = 23;
    }
    else
    {
        ChkDR( DRM_E_FAIL );
    }

    dr = DRM_SUCCESS;

ErrorExit:

    return dr;
}

/*********************************************************************
**
** Function: _PrintELS
**
** Synopsis: Prints the Licenses in the Embedded Store
**
** Arguments:
**
** [f_pbEmbeddedObject]   -- Pointer to an Embedded Store
** [f_cbEmbeddedObject]   -- Size in bytes of the store
**
** Returns:             DRM_E_INVALIDARG -- when any arg fails to verify
**                      Various errors propogated up
**
**********************************************************************/
static DRM_RESULT DRM_CALL _PrintELS(
    IN DRM_BYTE   *f_pbEmbeddedObject,
    IN DRM_DWORD   f_cbEmbeddedObject
)
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_DST                      oDatastoreEST = {eDRM_DST_NONE,0};
    DRM_EST_CONTEXT              oContextEST   = {0};
    DRM_BOOL                     fDSTInited    = FALSE;
    DRM_BOOL                     fDSTOpened    = FALSE;
    DRM_BOOL                     fLSTOpened    = FALSE;
    DRM_LICSTORE_CONTEXT         oELSContext   = {0};
    DRM_LICSTOREENUM_CONTEXT     oLicEnum      = {0};
    DRM_KID                      oKID          = {0};
    DRM_LID                      oLID          = {0};
    DRM_DST_SLOT_HINT            oSlotHint     = {0};
    DRM_DWORD                    cbLicense     = 0;
    DRM_BYTE                    *pbLicense     = NULL;
    DRM_WCHAR                    rgwszLID[DRM_GUID_STRING_LEN] = {0};
    DRM_BYTE                     rgbStack[1024]                = {0};
    DRM_DWORD                    cbStack            = 1024;
    DRM_STACK_ALLOCATOR_CONTEXT  oStack             = { 0 };
    DRM_WCHAR                    rgwchDateTime[100] = { 0 };
    DRM_STRING                   dstrDateTime       = EMPTY_DRM_STRING;
    DRM_WCHAR                    rgwchGuid[38]      = { 0 };
    DRMFILETIME                  ftXMR              = { 0 };
    DRM_UINT64                   ui64XMR            = DRM_UI64LITERAL( 0, 0 );
    DRM_XMR_LICENSE              oXMR               = { 0 };

    ChkArg( f_pbEmbeddedObject != NULL );

    dstrDateTime.pwszString = rgwchDateTime;
    dstrDateTime.cchString  = NO_OF( rgwchDateTime ) - 1;

    oStack.cbStack = cbStack;
    oStack.pbStack = rgbStack;

    ChkDR( DRM_DST_Init( eDRM_DST_EMBEDDED,
                       (DRM_DST_CONTEXT *) &oContextEST,
                       SIZEOF( DRM_EST_CONTEXT ),
                      &oDatastoreEST ) );

    fDSTInited = TRUE;

    dr = DRM_DST_OpenStore( NULL,
                             f_pbEmbeddedObject,
                             f_cbEmbeddedObject,
                             0,
                             &oDatastoreEST );
    if( dr == DRM_E_DSTCORRUPTED )
    {
        printf( "Not Inited" );
        dr = DRM_SUCCESS;
        goto ErrorExit;
    }

    fDSTOpened = TRUE;

    ChkDR( DRM_LST_Open( &oELSContext,
                         &oDatastoreEST,
                         eDRM_LICENSE_STORE_XMR ) );
    fLSTOpened = TRUE;

    ChkDR( DRM_LST_InitEnum( &oELSContext,
                              NULL,
                              FALSE,
                             &oLicEnum ) );

    printf( "\nLicenses:\n" );

    while( TRUE )
    {

        dr = DRM_LST_EnumNext( &oLicEnum,
                               &oKID,
                               &oLID,
                               &oSlotHint,
                               &cbLicense );
        if ( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;
            break;
        }
        else
        {
            ChkDR( dr );
        }

        SAFE_OEM_FREE( pbLicense );
        ChkMem( pbLicense = ( DRM_BYTE * ) Oem_MemAlloc( cbLicense ) );
        ChkDR( DRM_LST_GetLicense( &oELSContext,
                                   &oKID,
                                   &oLID,
                                   &oSlotHint,
                                    pbLicense,
                                   &cbLicense ) );

        ChkDR( DRM_UTL_GuidToString( (DRM_GUID *) &oLID, rgwszLID ) );
        printf( "\n%S:", rgwszLID );

        ChkDR( DRM_XMR_UnpackLicense( pbLicense, cbLicense, &oStack, &oXMR ) );

        if( XMR_IS_UPLINK_KID_VALID( &oXMR ) )
        {
            ChkDR( DRM_UTL_GuidToString( ( DRM_GUID * )(&(oXMR.containerOuter.containerKeys.UplinkKid.pbguidUplinkKID[oXMR.containerOuter.containerKeys.UplinkKid.iguidUplinkKID])),
                                         rgwchGuid ) );
            printf( "\n\tUplink KID: %S", rgwchGuid );
        }
        if( XMR_IS_DOMAIN_ID_VALID( &oXMR ) )
        {
            ChkDR( DRM_UTL_GuidToString( ( DRM_GUID * )(&(oXMR.containerOuter.containerGlobalPolicies.DomainID.pbAccountID[oXMR.containerOuter.containerGlobalPolicies.DomainID.ibAccountID])),
                                         rgwchGuid ) );
            printf( "\n\tDomain Account ID: %S", rgwchGuid );
        }
        printf( "\n\tExpiration: " );
        if( XMR_IS_EXPIRATION_VALID( &oXMR ) )
        {
             CREATE_FILE_TIME( oXMR.containerOuter.containerGlobalPolicies.Expiration.dwBeginDate, ftXMR );
             FILETIME_TO_UI64( ftXMR, ui64XMR );
             dr = _DateTimeToString( ui64XMR, &dstrDateTime );
             if( DRM_SUCCEEDED( dr ) )
             {
                 printf( "\n\t\tBegin Date: %S ", dstrDateTime.pwszString );
             }
             else
             {
                 printf( "\n\t\tBegin Date: Invalid " );
             }

             CREATE_FILE_TIME( oXMR.containerOuter.containerGlobalPolicies.Expiration.dwEndDate, ftXMR );
             FILETIME_TO_UI64( ftXMR, ui64XMR );
             dr = _DateTimeToString( ui64XMR, &dstrDateTime );
             if( DRM_SUCCEEDED( dr ) )
             {
                 printf( "\n\t\tEnd Date: %S ", dstrDateTime.pwszString );
             }
             else
             {
                 printf( "\n\t\tEnd Date: Invalid " );
             }
        }
        else
        {
            printf( "None" );
        }
    }
ErrorExit:
    SAFE_OEM_FREE(pbLicense);
    if ( fLSTOpened )
    {
        DRM_LST_Close( &oELSContext );
    }
    if ( fDSTOpened )
    {
        DRM_DST_CloseStore( &oDatastoreEST );
    }
    if ( fDSTInited )
    {
        DRM_DST_Uninit( &oDatastoreEST );
    }
    return dr;
}


/*********************************************************************
**
** Function: _PrintEnvelopeFileInfo
**
** Synopsis: Prints information relevant to an Emvelope file from a
**           media file context
**
** Arguments:
**
** [f_pEnvFile]          -- Pointer to an Envelope Context
**
** Returns:             DRM_E_INVALIDARG -- when any arg fails to verify
**                      Various errors propogated up
**
**********************************************************************/
static DRM_RESULT DRM_CALL _PrintEnvelopeFileInfo( IN DRM_ENVELOPED_FILE_CONTEXT *f_pEnvFile )
{
    DRM_RESULT          dr            = DRM_SUCCESS;
    DRM_WCHAR          *pwszOriginalFilename = NULL;
    DRM_DWORD           cchOriginalFilename  = 0;

    printf( "PlayReady Envelope:\n" );
    printf( "\tFormat Version: %hu\n", f_pEnvFile->oEnvHeader.wFormatVersion );
    printf( "\tCompatibility Version: %hu\n", f_pEnvFile->oEnvHeader.wCompatibleVersion );
    switch ( f_pEnvFile->oEnvHeader.dwCipherType )
    {
        case eDRM_AES_COUNTER_CIPHER:
            printf( "\tCipher Type: AES-Counter Mode\n" );
            break;

        case eDRM_RC4_CIPHER:
            printf( "\tCipher Type: RC4-Cocktail\n" );
            break;

        default:
            printf( "\tCipher Type: Invalid\n" );
    }

    dr = Drm_Envelope_GetOriginalFilename( f_pEnvFile, pwszOriginalFilename, &cchOriginalFilename );
    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        printf( "\tNo Valid Original Filename\n" );
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkMem( pwszOriginalFilename = ( DRM_WCHAR * ) Oem_MemAlloc( ( cchOriginalFilename + 1 ) * SIZEOF( DRM_WCHAR ) ) );
        ChkDR( Drm_Envelope_GetOriginalFilename( f_pEnvFile, pwszOriginalFilename, &cchOriginalFilename ) );
        pwszOriginalFilename[cchOriginalFilename] = ONE_WCHAR( '\0', '\0' );
        printf( "\tOriginal Filename: %S\n", pwszOriginalFilename );
    }


ErrorExit:
    SAFE_OEM_FREE( pwszOriginalFilename );
    return dr;
}

/*********************************************************************
**
** Function: _PrintAsfFileInfo
**
** Synopsis: Prints information relevant to an ASF file from a
**           media file context
**
** Arguments:
**
** [f_pEnvFile]          -- Pointer to an Envelope Context
**
** Returns:             DRM_E_INVALIDARG -- when any arg fails to verify
**                      Various errors propogated up
**
**********************************************************************/
static DRM_RESULT DRM_CALL _PrintAsfFileInfo( IN DRM_TOOLS_ASF_CONTEXT *f_poAsfFile )
{
    DRM_RESULT dr      = DRM_SUCCESS;
    DRM_DWORD  iStream = 0;

    ChkArg( f_poAsfFile != NULL );

    printf( "ASF File:\n" );
    if ( f_poAsfFile->oFoundObjects.fFoundV1 )
    {
        printf( "\tV1 KID: %s\n", ( DRM_CHAR * ) f_poAsfFile->oV1Data.pbKeyID );
        printf( "\tV1 URL: %s\n", ( DRM_CHAR * ) f_poAsfFile->oV1Data.pbURL );
    }
    printf( "\tNumber of Encrypted Streams: %hu\n", f_poAsfFile->dwNumEncryptedStreams );

    if ( f_poAsfFile->dwNumEncryptedStreams > 0 )
    {
        printf( "\n\tStream Info:\n" );
        for( iStream = 0; iStream < f_poAsfFile->dwNumEncryptedStreams; iStream++ )
        {
            printf( "\t\tStream ID: %hu\n", f_poAsfFile->rgoEncryptedStreams[ iStream ].bStreamID );
            if ( MEMCMP( &f_poAsfFile->rgoEncryptedStreams[ iStream ].guidEncryptedStreamType, &(g_guidASFXStreamTypeAcmAudio), SIZEOF( DRM_GUID ) ) == 0 )
            {
                printf( "\t\tStream Type: Audio\n" );
                switch( f_poAsfFile->rgoEncryptedStreams[ iStream ].wFormatTag )
                {
                case DRMTOOLS_ASF_AUDIO_TYPE_WMA:
                    printf( "\t\tStream Format: WMA\n\n" );
                    break;
                case DRMTOOLS_ASF_AUDIO_TYPE_GSM_AMR_FIXED:
                    printf( "\t\tStream Format: GSM-AMR (fixed bitrate, no SID)\n\n" );
                    break;
                case DRMTOOLS_ASF_AUDIO_TYPE_GSM_AMR_VBR:
                    printf( "\t\tStream Format: GSM-AMR (variable bitrate, including SID)\n\n" );
                    break;
                default:
                    printf( "\t\tStream Format: %hu ( unrecognized )\n\n", f_poAsfFile->rgoEncryptedStreams[ iStream ].wFormatTag );
                    break;
                }

            }
            else if ( MEMCMP( &f_poAsfFile->rgoEncryptedStreams[ iStream ].guidEncryptedStreamType, &(g_guidASFXStreamTypeIcmVideo), SIZEOF( DRM_GUID ) ) == 0 )
            {
                printf( "\t\tStream Type: Video\n\n" );
            }
            else
            {
                printf( "\t\tStream Type: Unknown\n\n" );
            }
        }
    }


ErrorExit:
    return dr;
}

static DRM_RESULT _PrintB64StringAsGuidString( IN DRM_CONST_STRING *f_pdstrB64 )
{
    DRM_RESULT dr       = DRM_SUCCESS;
    DRM_GUID   oGUID    = EMPTY_DRM_GUID;
    DRM_DWORD  cbGUID   = SIZEOF( DRM_GUID );
    DRM_WCHAR  rgwszGUID[DRM_GUID_STRING_LEN] = {0};

    ChkDRMString ( f_pdstrB64 );

    ChkDR( DRM_B64_DecodeW( f_pdstrB64, &cbGUID, ( DRM_BYTE *) &oGUID, 0 ) );

    ChkDR( DRM_UTL_GuidToString( &oGUID, rgwszGUID ) );

    printf( "%S", rgwszGUID );
ErrorExit:
    return dr;
}


static DRM_RESULT _PrintServiceId( IN DRM_CONST_STRING *f_pdstrHeader )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_STRING dstrServiceID = EMPTY_DRM_STRING;

    ChkArg( f_pdstrHeader != NULL );

    printf( "\tDomain Service ID: " );
    /*
    ** node is <WRMHEADER><DATA><DS_ID>...
    ** look for DS_ID in the second layer
    */
    dr = DRM_XML_GetSubNode( f_pdstrHeader, &g_dstrTagDSID, NULL, NULL,0, NULL, (DRM_CONST_STRING *) &dstrServiceID, 2 );

    if ( dr == DRM_E_XMLNOTFOUND )
    {
        printf( "None" );
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( dr );
        ChkDR( _PrintB64StringAsGuidString( (DRM_CONST_STRING *) &dstrServiceID ) );
    }
ErrorExit:
    printf("\n");
    return dr;
}

static DRM_RESULT _PrintKID( IN DRM_CONST_STRING *f_pdstrHeader )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_STRING dstrKID = EMPTY_DRM_STRING;

    ChkArg( f_pdstrHeader != NULL );

    printf( "\tKID: " );
    /*
    ** node is <WRMHEADER><DATA><DS_ID>...
    ** look for DS_ID in the second layer
    */
    dr = DRM_XML_GetSubNode( f_pdstrHeader, &g_dstrTagKID, NULL, NULL,0, NULL, (DRM_CONST_STRING *) &dstrKID, 2 );

    if ( dr == DRM_E_XMLNOTFOUND )
    {
        printf( "None" );
        dr = DRM_SUCCESS;
    }
    else
    {
        ChkDR( dr );
        ChkDR( _PrintB64StringAsGuidString( (DRM_CONST_STRING *) &dstrKID ) );
    }
ErrorExit:
    printf("\n");
    return dr;
}

/*********************************************************************
**
** Function: _PrintFileInfo
**
** Synopsis: Prints information relevant from a Media File Context
**
** Arguments:
**
** [f_pEnvFile]          -- Pointer to an Envelope Context
**
** Returns:             DRM_E_INVALIDARG -- when any arg fails to verify
**                      Various errors propogated up
**
**********************************************************************/
static DRM_RESULT _PrintFileInfo( IN DRM_MEDIA_FILE_CONTEXT *f_poMediaFile )
{
    DRM_RESULT dr            = DRM_SUCCESS;
    DRM_STRING dstrDRMHeader = EMPTY_DRM_STRING;
    DRM_BOOL   fPRO          = FALSE;

    switch ( f_poMediaFile->eFileType )
    {
    case eDRM_MEDIA_FILE_TYPE_ENVELOPE:
        ChkDR( _PrintEnvelopeFileInfo( &f_poMediaFile->FormatContext.oEnvFile ) );
        break;
    case eDRM_MEDIA_FILE_TYPE_ASF:
        ChkDR( _PrintAsfFileInfo( &f_poMediaFile->FormatContext.oAsfFile ) );
        break;
    default:
        ChkDR( DRM_E_FAIL );
    }

    if ( DRM_PRO_IsValidObject( f_poMediaFile->rgbDRMHeaderData, f_poMediaFile->cbDRMHeaderData ) == DRM_SUCCESS )
    {
        ChkDR( DRM_PRO_GetRecord( f_poMediaFile->rgbDRMHeaderData, f_poMediaFile->cbDRMHeaderData, PLAYREADY_WRMHEADER, (DRM_BYTE **)&dstrDRMHeader.pwszString, &dstrDRMHeader.cchString ) );
        dstrDRMHeader.cchString /= SIZEOF( DRM_WCHAR );
        printf( "\tPlayReady Object: TRUE\n" );
        fPRO = TRUE;
    }
    else
    {
        printf( "\tPlayReady Object: FALSE\n" );
        dstrDRMHeader.pwszString = (DRM_WCHAR * ) f_poMediaFile->rgbDRMHeaderData;
        dstrDRMHeader.cchString  = DRMCRT_wcslen( dstrDRMHeader.pwszString );
    }

    ChkDR( _PrintKID( ( DRM_CONST_STRING * ) &dstrDRMHeader ) );
    ChkDR( _PrintServiceId( ( DRM_CONST_STRING * ) &dstrDRMHeader ) );

    if ( dstrDRMHeader.cchString == 0 )
    {
        printf( "\tDRM Header has length 0.\n" );
    }
    else
    {
        DRM_DWORD ichHeader = 0;
        const DRM_WCHAR *pwszHeader = dstrDRMHeader.pwszString;
        printf( "\tDRM Header: " );

        /*
        ** The Header may not be NULL terminated.
        ** We need to print each character.
        */
        while( ichHeader < dstrDRMHeader.cchString )
        {
            if( (ichHeader % 60) == 0 )
            {
                /*
                ** Every 60 Characters, print a new line and indent.
                */
                printf( "\n\t\t" );
            }
            printf( "%C", pwszHeader[ichHeader] );
            ichHeader++;
        }
        printf( "\n" );
    }
    if ( fPRO )
    {
        DRM_BYTE  rgbELS[MAX_EMBEDDED_STORE_LEN] = {0};
        DRM_BYTE *pbELS                          = rgbELS;
        DRM_DWORD cbELS                          = MAX_EMBEDDED_STORE_LEN;


        printf( "\nEmbedded Store: " );

        dr =  DRM_PRO_GetRecord( f_poMediaFile->rgbDRMHeaderData,
                                 f_poMediaFile->cbDRMHeaderData,
                                 PLAYREADY_EMBEDDED_LICENSE_STORE,
                                &pbELS,
                                &cbELS);

        if ( dr != DRM_E_RECORD_NOT_FOUND )
        {
            if( DRM_FAILED( dr ) )
            {
                printf( "Invalid. Error 0x%X returned.\n", dr );
                goto ErrorExit;
            }

            ChkDR( _PrintELS( pbELS, cbELS ) );
        }
        else
        {
            printf( "Not Present. \n" );
        }

    }

ErrorExit:
    return dr;
}

/*********************************************************************
**
** Function: DRM_Main
**
** Synopsis: The DRM_Main function.
**
** Arguments:
**
** [argc]                   -- Total number of arguments.
** [argv]                   -- An array of arguments.
**
** Returns: DRM_SUCCESS     -- on success
**
**********************************************************************/
#if defined(_PREFAST_)
#pragma prefast(push)
#pragma prefast(disable:6262) /* Temporarily disable stack limit check. */
#endif
DRM_LONG DRM_CALL DRM_Main(
    IN DRM_LONG argc,
    IN DRM_WCHAR** argv )
{
    DRM_RESULT             dr              = DRM_SUCCESS;
    DRM_CONST_STRING       dstrContentFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING       dstrHeaderFile  = EMPTY_DRM_STRING;
    DRM_MEDIA_FILE_CONTEXT oMediaFile      = EMPTY_DRM_MEDIA_FILE_CONTEXT;
    DRM_BOOL               fPause          = FALSE;
    DRM_LONG i;

    PrintCopyrightInfo( "DRM File Viewer" );

    if ( argc <= 1 )
    {
        dr = DRM_S_MORE_DATA;
        goto _PrintUsage;
    }

    for ( i = 1; i < argc; i++ )
    {
        DRM_WCHAR wchOption;
        DRM_CONST_STRING dstrParam;

        if ( !DRM_CMD_ParseCmdLine( argv[ i ], &wchOption, &dstrParam, NULL ) )
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        wchOption = DRMCRT_towlower( wchOption );

        switch ( wchOption )
        {
            case WCHAR_CAST( '\0' ):
                fPause = TRUE;
            case WCHAR_CAST( 'c' ):
                dstrContentFile.pwszString = dstrParam.pwszString;
                dstrContentFile.cchString  = dstrParam.cchString;
                break;

            case WCHAR_CAST( 'h' ):
                dstrHeaderFile.pwszString = dstrParam.pwszString;
                dstrHeaderFile.cchString  = dstrParam.cchString;
                break;

            case WCHAR_CAST( '?' ):
                goto _PrintUsage;

            default:
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
        }
    }

    if( !dstrContentFile.cchString )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    ChkDR( DRMTOOLS_OpenMediaFile( &dstrContentFile,
                                   &oMediaFile ) );
    ChkDR( _PrintFileInfo( &oMediaFile ) );

    if( dstrHeaderFile.cchString )
    {
        ChkDR( DRMTOOLS_SaveContentHeaderToFile( &oMediaFile, &dstrHeaderFile ) );
        printf( "Header stored in: %S", dstrHeaderFile.pwszString );
    }

ErrorExit:
    if ( DRM_FAILED( dr ) )
    {
        printf( "Failed with error code: 0x%X\r\n.", dr );
    }
    DRMTOOLS_CloseMediaFile( &oMediaFile );

    if ( fPause )
    {
        printf("\nPress <Return> to exit...\n");
        if( 0 == getchar() && DRM_SUCCEEDED( dr ) )
        {
            dr = DRM_S_FALSE;
        }
    }
    return dr;
_PrintUsage:

    _PrintUsage( argv[ 0 ] );
    return dr;
}
#if defined(_PREFAST_)
#pragma prefast(pop)
#endif
