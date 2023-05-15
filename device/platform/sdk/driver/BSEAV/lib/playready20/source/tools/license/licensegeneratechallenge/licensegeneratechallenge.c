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
#include <drmdla.h>
#include <drmtoolsinit.h>

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
static DRM_VOID _PrintUsage( IN const DRM_WCHAR * f_pwszAppName )
{
    printf( "Syntax: %S [-?] [ -c:ContentFile | -h:ContentHeaderFile ] -o:ChallengeOutputFile -s:DataStoreFile [-u:URLFile] [-e] [-v]\n",
            f_pwszAppName );
    printf( "-e: The content file is NOT an envelope packaged file. It is ignored if a header file is provided.\n" );
    printf( "-v: Generate using the non SOAP based license challenge format.\n" );
    printf( "Note: If a header file is provided, a content file is not necessary and will be ignored if provided.\n" );
    printf( "      If a content file is provided, it will be opened as an envelope packaged file by default unless -e is specified.\n" );
    printf( "      By default, SOAP based license challenge format will be used unless -v is specified.\n" );
}

/*********************************************************************
**
** Function: _GenerateChallenge
**
** Synopsis: Function that generates a license acquisition challenge XML to
**           be sent to the license server.
**
** Arguments:
**
** [f_pdstrContentFile]     -- Pointer to a DRM string of a packaged content file.
** [f_pdstrContentFile]     -- Pointer to a DRM string of a content header file.
** [f_pdstrOutputFile]      -- Pointer to a DRM string of an challenge output file.
** [f_pdstrURLFile]         -- Pointer to a DRM string of a URL file.
** [f_fEnvelope]            -- Flag indicating whether a packaged content file is
**                             packaged using envelope.
** [f_fSOAP]                -- Flag indicating whether the generated challenge uses
**                             SOAP format used in the V3 license acquisition protocol.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FILEOPEN if file open fails.
**                          DRM_E_FILEREADERROR if file read fails.
**                          DRM_E_FILEWRITEERROR if file read fails.
**
**********************************************************************/
static DRM_RESULT _GenerateChallenge(
    IN DRM_CONST_STRING *f_pdstrContentFile,
    IN DRM_CONST_STRING *f_pdstrHeaderFile,
    IN DRM_CONST_STRING *f_pdstrOutputFile,
    IN DRM_CONST_STRING *f_pdstrURLFile,
    IN DRM_CONST_STRING *f_pdstrDataStoreFile,
    IN DRM_BOOL f_fEnvelope,
    IN DRM_BOOL f_fSOAP )
{
    DRM_RESULT               dr                 = DRM_SUCCESS;
    DRM_APP_CONTEXT         *poAppContext       = NULL;
    DRM_BYTE                *pbOpaqueBuffer     = NULL;
    const DRM_CONST_STRING  *rgpdstrRights[ 1 ] = { 0 };
    DRM_DWORD                cbChunk            = 0;
    DRM_DWORD                cbFile             = 0;
    DRM_DWORD                cbRead             = 0;
    DRM_DWORD                cbXML              = 0;
    DRM_DWORD                cbWritten          = 0;
    DRM_DWORD                cchUrl             = 0;
    DRM_DWORD                cbChallenge        = 0;
    DRM_BYTE                *pbXML              = NULL;
    OEM_FILEHDL              hfileIn            = OEM_INVALID_HANDLE_VALUE;
    OEM_FILEHDL              hfileOut           = OEM_INVALID_HANDLE_VALUE;
    OEM_FILEHDL              hfileURL           = OEM_INVALID_HANDLE_VALUE;
    DRM_CHAR                *pchUrl             = NULL;
    DRM_BOOL                 fIsHeader          = FALSE;
    DRM_BYTE                *pbChallenge        = NULL;

    ChkArg( f_pdstrContentFile != NULL );
    ChkArg( f_pdstrHeaderFile != NULL );
    ChkDRMString( f_pdstrOutputFile );
    ChkArg( f_pdstrURLFile != NULL );
    ChkDRMString( f_pdstrDataStoreFile );

    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, f_pdstrDataStoreFile, TOOLS_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &poAppContext ) );

    rgpdstrRights[ 0 ] = &g_dstrWMDRM_RIGHT_PLAYBACK;

    if ( f_pdstrHeaderFile->cchString > 0 )
    {
        hfileIn = Oem_File_Open( NULL,
                                f_pdstrHeaderFile->pwszString,
                                OEM_GENERIC_READ,
                                OEM_FILE_SHARE_READ,
                                OEM_OPEN_EXISTING,
                                OEM_ATTRIBUTE_NORMAL );

        if ( hfileIn == OEM_INVALID_HANDLE_VALUE )
        {
            printf( "Failed to open header file.\n" );

            ChkDR( DRM_E_FILEOPEN );
        }

        fIsHeader = TRUE;
    }
    else if ( !f_fEnvelope )
    {
        ChkDRMString( f_pdstrContentFile );

        hfileIn = Oem_File_Open( NULL,
                                f_pdstrContentFile->pwszString,
                                OEM_GENERIC_READ,
                                OEM_FILE_SHARE_READ,
                                OEM_OPEN_EXISTING,
                                OEM_ATTRIBUTE_NORMAL );

        if ( hfileIn == OEM_INVALID_HANDLE_VALUE )
        {
            printf( "Failed to open non-enveloped content file.\n" );

            ChkDR( DRM_E_FILEOPEN );
        }
    }

    if ( !fIsHeader )
    {
        if ( !f_fEnvelope )
        {
            ChkFAIL( Oem_File_GetSize( hfileIn, &cbFile ) );

            /*
            ** File structure of a content file that does not use envelope is:
            ** cbChunk(DWORD) | cbXML(DWORD) | cbXML bytes of header data | content data
            */
            ChkFAIL( cbFile >= SIZEOF( cbChunk ) );

            if ( Oem_File_Read( hfileIn, &cbChunk, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
                 cbRead != SIZEOF( DRM_DWORD) )
            {
                printf( "Failed to read the block size.\n" );

                ChkDR( DRM_E_FILEREADERROR );
            }

            FIX_ENDIAN_DWORD( cbChunk );

            if ( Oem_File_Read( hfileIn, &cbXML, SIZEOF( DRM_DWORD ), &cbRead ) == FALSE ||
                 cbRead != SIZEOF( DRM_DWORD ) )
            {
                printf( "Failed to read size of the content header.\n" );

                ChkDR( DRM_E_FILEREADERROR );
            }

            FIX_ENDIAN_DWORD( cbXML );

            if ( ( cbXML % SIZEOF( DRM_WCHAR ) ) != 0 )
            {
                ChkDR( DRM_E_FILEREADERROR );
            }
        }
        else
        {
            DRM_ENVELOPED_FILE_CONTEXT oEnvFile = { 0 };

            dr = Drm_Envelope_Open( poAppContext,
                                       NULL,
                                       f_pdstrContentFile->pwszString,
                                       &oEnvFile );

            if ( DRM_FAILED( dr ) )
            {
                printf( "Failed to open enveloped content file.\n" );

                ChkDR( dr );
            }

            ChkDR( Drm_Envelope_Close( &oEnvFile ) );
        }
    }
    else
    {
        ChkFAIL( Oem_File_GetSize( hfileIn, &cbFile ) );

        /* A content header is provided, read the whole file. */
        cbXML = cbFile;
    }

    /* Do not set content header when an envelope packaged content is provided. */
    if( fIsHeader || !f_fEnvelope )
    {
        ChkMem( pbXML = ( DRM_BYTE * )Oem_MemAlloc( cbXML ) );

        if ( Oem_File_Read( hfileIn, pbXML, cbXML, &cbRead ) == FALSE ||
             cbRead != cbXML )
        {
            printf( "Failed to read the content header.\n" );

            ChkDR( DRM_E_FILEREADERROR );
        }

        ChkDR( Drm_Content_SetProperty( poAppContext, DRM_CSP_AUTODETECT_HEADER, pbXML, cbXML ) );
    }

    dr = Drm_LicenseAcq_GenerateChallenge( poAppContext,
                                           rgpdstrRights,
                                           NO_OF( rgpdstrRights ),
                                           NULL,
                                           NULL,
                                           0,
                                           NULL,
                                           &cchUrl,
                                           NULL,
                                           NULL,
                                           NULL,
                                           &cbChallenge );

    if ( dr != DRM_E_BUFFERTOOSMALL )
    {
        goto ErrorExit;
    }

    ChkMem( pchUrl = ( DRM_CHAR * )Oem_MemAlloc( cchUrl ) );

    ChkMem( pbChallenge = ( DRM_BYTE * )Oem_MemAlloc( cbChallenge ) );

    ChkDR( Drm_LicenseAcq_GenerateChallenge( poAppContext,
                                             rgpdstrRights,
                                             NO_OF( rgpdstrRights ),
                                             NULL,
                                             NULL,
                                             0,
                                             pchUrl,
                                             &cchUrl,
                                             NULL,
                                             NULL,
                                             ( DRM_BYTE * )pbChallenge,
                                             &cbChallenge ) );

    hfileOut = Oem_File_Open( NULL,
                             f_pdstrOutputFile->pwszString,
                             OEM_GENERIC_READ | OEM_GENERIC_WRITE,
                             OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
                             OEM_CREATE_ALWAYS,
                             OEM_ATTRIBUTE_NORMAL );

    if ( hfileOut != OEM_INVALID_HANDLE_VALUE )
    {
        if ( !Oem_File_Write( hfileOut, pbChallenge, cbChallenge, &cbRead ) ||
             cbRead != cbChallenge )
        {
            printf( "Failed to write challenge.\n" );

            ChkDR( DRM_E_FILEWRITEERROR );
        }
    }
    else
    {
        printf( "Failed to open output file.\n" );

        ChkDR( DRM_E_FILEOPEN );
    }

    if ( f_pdstrURLFile->cchString > 0 )
    {
        hfileURL = Oem_File_Open( NULL,
                                 f_pdstrURLFile->pwszString,
                                 OEM_GENERIC_READ | OEM_GENERIC_WRITE,
                                 OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
                                 OEM_CREATE_ALWAYS,
                                 OEM_ATTRIBUTE_NORMAL );

        if ( hfileURL != OEM_INVALID_HANDLE_VALUE )
        {
            if ( !Oem_File_Write( hfileURL, pchUrl, cchUrl, &cbWritten ) ||
                 cbWritten != cchUrl )
            {
                printf( "Failed to write Url file\n" );

                ChkDR( DRM_E_FILEWRITEERROR );
            }
        }
        else
        {
            printf( "Failed to open URL file.\n" );

            ChkDR( DRM_E_FILEOPEN );
        }
    }

ErrorExit:

    if ( hfileURL != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hfileURL );
    }

    if ( hfileOut != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hfileOut );
    }

    if ( hfileIn != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hfileIn );
    }

    SAFE_OEM_FREE( pchUrl );

    SAFE_OEM_FREE( pbChallenge );

    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &poAppContext );

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
** Returns:                 N/A
**
**********************************************************************/
DRM_LONG DRM_CALL DRM_Main( IN DRM_LONG argc, IN DRM_WCHAR** argv )
{
    DRM_RESULT dr = DRM_SUCCESS;

    DRM_CONST_STRING dstrContentFile   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrOutputFile    = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrURLFile       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDataStoreFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrHeaderFile    = EMPTY_DRM_STRING;
    DRM_LONG i         = 0;
    DRM_BOOL fEnvelope = TRUE;
    DRM_BOOL fSOAP     = TRUE;

    PrintCopyrightInfo( "Generate DRM License Challenge" );

    if( DRM_DLA_IsDLASupported() )
    {
        if ( argc <= 1 )
        {
            dr = DRM_S_MORE_DATA;
            goto _PrintUsage;
        }
        if ( argc > 7 )
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        for ( i = 1; i < argc; i++ )
        {
            DRM_WCHAR wchOption;
            DRM_CONST_STRING dstrParam;

            if ( !DRM_CMD_ParseCmdLine( argv[ i ], &wchOption, &dstrParam, NULL ) )
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
                    case WCHAR_CAST( 'c' ):
                        dstrContentFile.pwszString = dstrParam.pwszString;
                        dstrContentFile.cchString  = dstrParam.cchString;
                        break;

                    case WCHAR_CAST( 'o' ):
                        dstrOutputFile.pwszString = dstrParam.pwszString;
                        dstrOutputFile.cchString  = dstrParam.cchString;
                        break;

                    case WCHAR_CAST( 's' ):
                        dstrDataStoreFile.pwszString = dstrParam.pwszString;
                        dstrDataStoreFile.cchString  = dstrParam.cchString;
                        break;

                    case WCHAR_CAST( 'u' ):
                        ASSIGN_DRM_STRING( dstrURLFile, dstrParam );
                        break;

                    case WCHAR_CAST( 'h' ):
                        ASSIGN_DRM_STRING( dstrHeaderFile, dstrParam );
                        break;

                    case WCHAR_CAST( 'e' ):
                        /* The content file specified is not envelope packaged. */
                        fEnvelope = FALSE;
                        break;

                    case WCHAR_CAST( 'v' ):
                        /* Generate the challenge using non SOAP based format. */
                        fSOAP = FALSE;
                        break;

                    case WCHAR_CAST('?'):
                        goto _PrintUsage;

                default:
                        dr = DRM_E_INVALID_COMMAND_LINE;
                        goto _PrintUsage;
                }
            }
        }
        /* dstrFileURL is optional */

        if( ( !dstrContentFile.cchString && !dstrHeaderFile.cchString ) ||
              !dstrOutputFile.cchString ||
              !dstrDataStoreFile.cchString )
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        ChkDR( _GenerateChallenge( &dstrContentFile,
                                   &dstrHeaderFile,
                                   &dstrOutputFile,
                                   &dstrURLFile,
                                   &dstrDataStoreFile,
                                   fEnvelope,
                                   fSOAP ) );
    }
    else
    {
        printf( "This tool requires that the real dla library be linked to it.\r\n" );
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:
    if ( DRM_SUCCEEDED( dr ) )
    {
        printf( "Success!\r\n" );
    }
    else
    {
        printf( "Failed with error code: 0x%X\r\n.", dr );
    }
    return dr;

_PrintUsage:

    _PrintUsage( argv[ 0 ] );
    return dr;
}

