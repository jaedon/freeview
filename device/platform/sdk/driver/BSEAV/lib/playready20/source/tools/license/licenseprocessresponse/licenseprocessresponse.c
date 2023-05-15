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
static DRM_VOID _PrintUsage( IN const DRM_WCHAR *f_pwszAppName )
{
    printf( "Syntax: %S [-?] -r:ResponseFile -s:DataStore [-o:AcknowledgementOutputFile ] [-t (flags the clock as being set)] [-a]\n",
            f_pwszAppName );
    printf( "-a: The response file contains a license acknowledgement response.\n");
    printf( "Note: By default the response file contains a license acquisition response unless -a is specified.\n" );
    printf( "      License acknowledgement is generated after the license acquisition response is processed.\n" );
    printf( "      The optional acknowledgment output file is only used when the response is of license acquisition type.\n" );
}

/*********************************************************************
**
** Function: _ProcessResponse
**
** Synopsis: Function that processes a license acquisition or license
**           acknowledgement response.
**
** Arguments:
**
** [f_pdstrResponseFile]    -- Pointer to a DRM string of a response file.
** [f_pdstrURLFile]         -- Pointer to a DRM string of a HDS file.
** [f_pdstrAckFile]         -- Pointer to a DRM string of an acknowledgement output file.
** [f_fAck]                 -- Flag indicating whether the response is an
**                             acknowledgement response.
**
** Returns:                 DRM_SUCCESS on success.
**                          DRM_E_INVALIDARG if the arguments are invalid.
**                          DRM_E_FILEOPEN if file open fails.
**                          DRM_E_FILEREADERROR if file read fails.
**
**********************************************************************/
static DRM_RESULT _ProcessResponse(
    IN DRM_CONST_STRING *f_pdstrResponseFile,
    IN DRM_CONST_STRING *f_pdstrDataStoreFile,
    IN DRM_CONST_STRING *f_pdstrAckFile,
    IN DRM_BOOL f_fAck )
{
    DRM_RESULT               dr                     = DRM_SUCCESS;
    DRM_RESULT               dr1                    = DRM_SUCCESS;
    DRM_APP_CONTEXT         *poAppContext           = NULL;
    DRM_BYTE                *pbOpaqueBuffer         = NULL;
    DRM_DWORD                cbFile                 = 0;
    DRM_BYTE                *pbData                 = NULL;
    OEM_FILEHDL              fpResponse             = OEM_INVALID_HANDLE_VALUE;
    OEM_FILEHDL              hfileAck               = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD                cbRead                 = 0;
    DRM_BYTE                *pbRevocationBuffer     = NULL;
    DRM_LICENSE_RESPONSE     oLicenseResponse       = { eUnknownProtocol, 0 };
    DRM_BYTE                *pbChallenge            = NULL;
    DRM_DWORD                cbChallenge            = 0;
    DRM_DWORD                cbWritten              = 0;

    ChkDRMString( f_pdstrResponseFile );
    ChkDRMString( f_pdstrDataStoreFile);
    ChkArg( f_pdstrAckFile != NULL );

    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, f_pdstrDataStoreFile, TOOLS_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &poAppContext ) );

    ChkDR( DRM_STR_StringCchCopyNW( ( DRM_WCHAR * )pbOpaqueBuffer,
                               TOOLS_DRM_BUFFER_SIZE / SIZEOF( DRM_WCHAR ),
                               f_pdstrResponseFile->pwszString,
                               f_pdstrResponseFile->cchString ) );

    fpResponse = Oem_File_Open( NULL,
                               ( DRM_WCHAR * )pbOpaqueBuffer,
                               OEM_GENERIC_READ,
                               OEM_FILE_SHARE_READ,
                               OEM_OPEN_EXISTING,
                               OEM_ATTRIBUTE_NORMAL );

    if ( OEM_INVALID_HANDLE_VALUE == fpResponse )
    {
        printf( "Failed to open response file.\n" );

        ChkDR( DRM_E_FILEOPEN );
    }

    if( DRM_REVOCATION_IsRevocationSupported() )
    {
        ChkMem( pbRevocationBuffer = ( DRM_BYTE * )Oem_MemAlloc( REVOCATION_BUFFER_SIZE ) );
        ChkDR( Drm_Revocation_SetBuffer( poAppContext, pbRevocationBuffer, REVOCATION_BUFFER_SIZE ) );
    }

    ChkFAIL( Oem_File_GetSize( fpResponse, &cbFile ) );

    ChkFAIL( Oem_File_SetFilePointer( fpResponse, 0, OEM_FILE_BEGIN, NULL ) );

    ChkMem( pbData = ( DRM_BYTE * )Oem_MemAlloc( cbFile ) );

    if ( !Oem_File_Read( fpResponse, pbData, cbFile, &cbRead ) ||
         cbRead != cbFile )
    {
        printf( "Failed to read response file.\n" );

        ChkDR( DRM_E_FILEREADERROR );
    }

    if ( !f_fAck )
    {
        ChkDR( Drm_LicenseAcq_ProcessResponse( poAppContext,
                                               DRM_PROCESS_LIC_RESPONSE_SIGNATURE_NOT_REQUIRED,
                                               NULL,
                                               NULL,
                                               pbData,
                                               cbFile,
                                               &oLicenseResponse ) );

        if ( f_pdstrAckFile->cchString > 0 )
        {
            dr = Drm_LicenseAcq_GenerateAck( poAppContext,
                                             &oLicenseResponse,
                                             NULL,
                                             &cbChallenge );

            if ( DRM_FAILED( dr ) )
            {
                if ( dr != DRM_E_BUFFERTOOSMALL )
                {
                    ChkDR( dr );
                }
            }
            else
            {
                ChkDR( DRM_E_FAIL );
            }

            ChkMem( pbChallenge = ( DRM_BYTE * )Oem_MemAlloc( cbChallenge ) );

            ChkDR( Drm_LicenseAcq_GenerateAck( poAppContext,
                                               &oLicenseResponse,
                                               pbChallenge,
                                               &cbChallenge ) );

            hfileAck = Oem_File_Open( NULL,
                                     f_pdstrAckFile->pwszString,
                                     OEM_GENERIC_READ | OEM_GENERIC_WRITE,
                                     OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
                                     OEM_CREATE_ALWAYS,
                                     OEM_ATTRIBUTE_NORMAL );

            if ( hfileAck != OEM_INVALID_HANDLE_VALUE )
            {
                if ( !Oem_File_Write( hfileAck, pbChallenge, cbChallenge, &cbWritten ) ||
                     cbWritten != cbChallenge )
                {
                    printf( "Failed to write acknowledgement output file.\n" );

                    ChkDR( DRM_E_FILEWRITEERROR );
                }
            }
            else
            {
                printf( "Failed to open acknowledgement output file.\n" );

                ChkDR( DRM_E_FILEOPEN );
            }
        }
    }
    else
    {
        ChkDR( Drm_LicenseAcq_ProcessAckResponse( poAppContext,
                                                  pbData,
                                                  cbFile,
                                                  &dr1 ) );
    }

ErrorExit:

    if ( hfileAck != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hfileAck );
    }

    if ( fpResponse != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( fpResponse );
    }

    SAFE_OEM_FREE( pbData );

    SAFE_OEM_FREE( pbRevocationBuffer );

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
** Returns: DRM_SUCCESS     -- on success
**
**********************************************************************/
DRM_LONG DRM_CALL DRM_Main( IN DRM_LONG argc, IN DRM_WCHAR** argv )
{
    DRM_RESULT       dr               = DRM_SUCCESS;
    DRM_CONST_STRING dstrStoreFile    = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrResponseFile = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrAckFile      = EMPTY_DRM_STRING;
    DRM_LONG i    = 0;
    DRM_BOOL fAck = FALSE;

    PrintCopyrightInfo( "Process DRM License Response tool" );

#if EMBEDDED_WITH_NO_PARAMS
    argc = g_argc;
    argv = g_argv;
#endif

    if ( argc <= 1 )
    {
        dr = DRM_S_MORE_DATA;
        goto _PrintUsage;
    }
    if ( argc > 5 )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    for ( i = 1; i < argc; i++ )
    {
        DRM_WCHAR wchOption = 0;
        DRM_CONST_STRING dstrParam = EMPTY_DRM_STRING;

        if ( DRM_CMD_ParseCmdLine( argv[ i ], &wchOption, &dstrParam, NULL ) == FALSE )
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
                case WCHAR_CAST( 'r' ):
                    dstrResponseFile.pwszString = dstrParam.pwszString;
                    dstrResponseFile.cchString  = dstrParam.cchString;
                    break;

                case WCHAR_CAST( 's' ):
                    dstrStoreFile.pwszString = dstrParam.pwszString;
                    dstrStoreFile.cchString  = dstrParam.cchString;
                    break;

                case WCHAR_CAST( 't' ):
                    Oem_Clock_SetResetState( NULL, FALSE );
                    break;

                case WCHAR_CAST( 'a' ):
                    fAck = TRUE;
                    break;

               case WCHAR_CAST( 'o' ):
                    dstrAckFile.pwszString = dstrParam.pwszString;
                    dstrAckFile.cchString  = dstrParam.cchString;
                    break;

               case WCHAR_CAST('?'):
                    goto _PrintUsage;

                default:
                    dr = DRM_E_INVALID_COMMAND_LINE;
                    goto _PrintUsage;
            }
        }
    }

    if ( dstrResponseFile.cchString == 0 ||
         dstrStoreFile.cchString == 0 )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    ChkDR( _ProcessResponse( &dstrResponseFile, &dstrStoreFile, &dstrAckFile, fAck ) );

ErrorExit:
    if ( DRM_SUCCEEDED( dr ) )
    {
        printf( "Success: 0x%X\r\n", dr );
    }
    else
    {
        printf( "Error: 0x%X\r\n", dr );
    }

    return dr;

_PrintUsage:

    _PrintUsage( argv[ 0 ] );
    return dr;
}
