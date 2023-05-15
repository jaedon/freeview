/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdio.h>

#include <drmutilities.h>
#include <drmcrt.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <drmcmdlnpars.h>
#include <drmtoolsutils.h>
#include <drmdla.h>
#include <drmtoolsinit.h>
#include <drmsyncstore.h>
#include <drmconstants.h>

/******************************************************************************
** Print a command menu to console
*******************************************************************************
*/
static void PrintUsage( const DRM_WCHAR *pwszAppName )
{
    printf("Syntax: %S [-?] -o:ChallengeOutputFileBase -s:DataStoreName [-u:URLFileBase] [-c:MaxCount] [-h:MaxHours]\n", pwszAppName);
}

static DRM_RESULT
GenerateChallenges(const DRM_CONST_STRING *pdstrOutputFileName,
                   const DRM_CONST_STRING *f_pdstrFileURL,
                   const DRM_CONST_STRING *pdstrDataStoreFile,
                         DRM_DWORD         cCountMax,
                         DRM_DWORD         cHoursMax)
{
    DRM_APP_CONTEXT           *poManagerContext                = NULL;
    DRM_BYTE                  *pbOpaqueBuffer                  = NULL;
    const DRM_CONST_STRING    *rgpdstrRights [1]               = { 0 };
    DRM_DWORD                  cbRead                          = 0,
                               cbWritten                       = 0,
                               cchUrl                          = 0,
                               dwChallenges                    = 0,
                               cchChallenge                    = 0;
    OEM_FILEHDL                hfileOut                        = OEM_INVALID_HANDLE_VALUE;
    OEM_FILEHDL                hfileURL                        = OEM_INVALID_HANDLE_VALUE;
    DRM_CHAR                  *pszUrl                          = NULL;
    DRM_CHAR                  *pszChallenge                    = NULL;
    DRM_DWORD                  i                               = 0;
    DRM_RESULT                 dr                              = DRM_SUCCESS;
    DRM_KID                    oKID                            = {0};
    DRM_WCHAR                  rgwszFilename[1024]             = {0};
    const DRM_CONST_STRING    *rgdstrRights[]                  = { &g_dstrWMDRM_RIGHT_PLAYBACK };
    DRM_LICENSE_STATE_DATA     lsdLicense[NO_OF(rgdstrRights)] = {0};

    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, pdstrDataStoreFile, TOOLS_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &poManagerContext ) );

    rgpdstrRights [0] = &g_dstrWMDRM_RIGHT_PLAYBACK;

    ChkDR (Drm_SyncListItem_InitEnum (poManagerContext));

    do {
        dr = Drm_SyncListItem_EnumNext( poManagerContext, cCountMax, cHoursMax, &oKID );
        if( dr == DRM_E_NOMORE )
        {
            dr = DRM_SUCCESS;
            break;
        }
        ChkDR( dr );

        dwChallenges++;

        printf("Processing entry #%d\r\n", dwChallenges);

        ChkDR( Drm_LicenseQuery_GetState( poManagerContext, rgdstrRights, NO_OF( rgdstrRights ), lsdLicense, NULL, NULL ) );
        printf( ( "  Content rights information:\n" ) );

        for( i = 0; i < NO_OF( rgdstrRights ); i++ )
        {
            printf( ( "    %ws:\n" ), rgdstrRights[i]->pwszString );
            DRMTOOLS_PrintLicenseStateData( ("      "), &(lsdLicense[ i ]) );
        }

        /* We don't have a valid license */
        dr = Drm_LicenseAcq_GenerateChallenge (poManagerContext,
                                               rgpdstrRights,
                                               1,
                                               NULL,
                                               NULL,
                                               0,
                                               NULL,
                                               &cchUrl,
                                               NULL,
                                               NULL,
                                               NULL,
                                               &cchChallenge);
        if( dr != DRM_E_BUFFERTOOSMALL )
        {
            printf("Error generating a license challenge, dr = 0x%X\r\n\r\n",  dr);
            continue;
        }
        ChkMem(pszUrl       = (DRM_CHAR *)  Oem_MemAlloc(cchUrl));
        ChkMem(pszChallenge = (DRM_CHAR  *) Oem_MemAlloc(cchChallenge));

        ChkDR(Drm_LicenseAcq_GenerateChallenge(poManagerContext,
                                               rgpdstrRights,
                                               NO_OF(rgpdstrRights),
                                               NULL,
                                               NULL,
                                               0,
                                               pszUrl,
                                               &cchUrl,
                                               NULL,
                                               NULL,
                                               (DRM_BYTE *)pszChallenge,
                                               &cchChallenge));

        /*
         * Prepare the challenge filename by appending the challenge number to the supplied name
         */
        ChkDR( DRM_STR_StringCchCopyNW( rgwszFilename,
                        NO_OF( rgwszFilename ),
                        pdstrOutputFileName->pwszString,
                        pdstrOutputFileName->cchString ) );

        ChkOverflow( NO_OF( rgwszFilename ), pdstrOutputFileName->cchString );
        ChkBOOL( NO_OF( rgwszFilename ) - pdstrOutputFileName->cchString > DRM_MAX_CCH_BASE10_DWORD_STRING + 1, DRM_E_BUFFERTOOSMALL );
        ChkDR( DRM_UTL_NumberToString( dwChallenges,
                        rgwszFilename + pdstrOutputFileName->cchString,
                        NO_OF( rgwszFilename ) - pdstrOutputFileName->cchString,
                        0, 10, NULL ) );

        hfileOut = Oem_File_Open(NULL, rgwszFilename,
                                      OEM_GENERIC_READ    | OEM_GENERIC_WRITE,
                                      OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
                                      OEM_CREATE_ALWAYS,
                                      OEM_ATTRIBUTE_NORMAL);

        if (hfileOut != OEM_INVALID_HANDLE_VALUE)
        {
            if (! Oem_File_Write(hfileOut, pszChallenge, cchChallenge, &cbRead)
            ||  cbRead != cchChallenge)
            {
                printf("Failed to write challenge\n");
                goto ErrorExit;
            }
            Oem_File_Close(hfileOut);
        }
        else
        {
            printf( "Challenge file not opened\n" );
            goto ErrorExit;
        }

        /* if Url output file requested */

        if (f_pdstrFileURL->cchString > 0)
        {
            /*
             * Prepare the URL filename by appending the challenge number to the supplied name
             */
            ChkDR( DRM_STR_StringCchCopyNW( rgwszFilename,
                            NO_OF( rgwszFilename ),
                            f_pdstrFileURL->pwszString,
                            f_pdstrFileURL->cchString ) );

            ChkBOOL( NO_OF( rgwszFilename ) - f_pdstrFileURL->cchString > DRM_MAX_CCH_BASE10_DWORD_STRING+1, DRM_E_BUFFERTOOSMALL );
            ChkDR( DRM_UTL_NumberToString( dwChallenges,
                            rgwszFilename + f_pdstrFileURL->cchString,
                            NO_OF( rgwszFilename ) - f_pdstrFileURL->cchString,
                            0, 10, NULL ) );

            hfileURL = Oem_File_Open(NULL, rgwszFilename,
                                    OEM_GENERIC_READ    | OEM_GENERIC_WRITE,
                                    OEM_FILE_SHARE_READ | OEM_FILE_SHARE_WRITE,
                                    OEM_CREATE_ALWAYS,
                                    OEM_ATTRIBUTE_NORMAL);

            if (hfileURL != OEM_INVALID_HANDLE_VALUE)
            {
                DRM_DWORD cbUrl = cchUrl;

                if (! Oem_File_Write(hfileURL, pszUrl, cbUrl, &cbWritten)
                ||  cbWritten != cbUrl)
                {
                    printf("Failed to write Url file\n");
                }

                Oem_File_Close(hfileURL);
            }
            else
            {
                printf("Url file not opened\n");
            }
        }

        printf("\r\n");
    } while(TRUE);

ErrorExit:

    if (hfileOut != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close(hfileOut);
    }

    if (hfileURL != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close(hfileURL);
    }

    SAFE_OEM_FREE(pszUrl);
    SAFE_OEM_FREE(pszChallenge);

    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &poManagerContext );

    return dr;
}

/******************************************************************************
**
*******************************************************************************
*/
DRM_LONG DRM_CALL DRM_Main( DRM_LONG argc, DRM_WCHAR** argv )
{
    DRM_CONST_STRING dstrOutputFile    = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrFileURL       = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrDataStoreFile = EMPTY_DRM_STRING;
    DRM_DWORD        cHoursMax          = DRM_SYNC_IGNORE_THRESHOLD_PARAMETER,
                     cCountMax          = DRM_SYNC_IGNORE_THRESHOLD_PARAMETER;
    DRM_LONG         i                  = 0;
    DRM_RESULT       dr                 = DRM_SUCCESS;

    PrintCopyrightInfo( "Generate License Sync Challenges" );

    if( DRM_WMDRMDLA_IsWMDRMDLASupported()
     && DRM_SNC_IsLicenseSyncSupported() )
    {
        if ( argc <= 1 )
        {
            dr = DRM_S_MORE_DATA;
            goto _PrintUsage;
        }

        if ( argc > 3 )
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        ChkArg( argv != NULL );
        for( i = 1; i < argc; i++ )
        {
            DRM_WCHAR        wchOption;
            DRM_CONST_STRING dstrParam;

            if (! DRM_CMD_ParseCmdLine(argv [i], &wchOption, &dstrParam, NULL))
            {
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }

            wchOption = DRMCRT_towlower( wchOption );

            switch (wchOption)
            {
                case WCHAR_CAST('o'):
                    dstrOutputFile.pwszString = dstrParam.pwszString;
                    dstrOutputFile.cchString  = dstrParam.cchString;
                    break;

                case WCHAR_CAST('s'):
                    dstrDataStoreFile.pwszString = dstrParam.pwszString;
                    dstrDataStoreFile.cchString  = dstrParam.cchString;
                    break;

                case WCHAR_CAST('u'):
                    ASSIGN_DRM_STRING(dstrFileURL, dstrParam);
                    break;

                case WCHAR_CAST('h'):
                    if( DRM_FAILED( DRMCRT_wcsntol(dstrParam.pwszString, dstrParam.cchString, (DRM_LONG*)&cHoursMax) ) )
                    {
                       printf( "Invalid number %S\n", dstrParam.pwszString );
                       goto _PrintUsage;
                    }
                    break;

                case WCHAR_CAST('c'):
                    if( DRM_FAILED( DRMCRT_wcsntol(dstrParam.pwszString, dstrParam.cchString, (DRM_LONG*)&cCountMax) ) )
                    {
                       printf( "Invalid number %S\n", dstrParam.pwszString );
                       goto _PrintUsage;
                    }
                    break;

                case WCHAR_CAST('?'):
                    goto _PrintUsage;

            default:
                dr = DRM_E_INVALID_COMMAND_LINE;
                goto _PrintUsage;
            }
        }

        /* dstrFileURL is optional */

        if( !dstrOutputFile.cchString || !dstrDataStoreFile.cchString )
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        ChkDR( GenerateChallenges( &dstrOutputFile, &dstrFileURL, &dstrDataStoreFile, cCountMax, cHoursMax ) );
    }
    else
    {
        printf( "This tool requires that the real license sync and dla libraries be linked to it.\r\n" );
        ChkDR( DRM_E_NOTIMPL );
    }

ErrorExit:
    if( DRM_SUCCEEDED(dr) )
    {
        printf("Operation returned success\r\n");
    }
    else
    {
        printf("Operation failed with error code: 0x%X\r\n", dr);
    }
    return dr;

_PrintUsage:
    PrintUsage(argv == NULL ? &g_wchNull : argv[0]);
    return dr;
}

