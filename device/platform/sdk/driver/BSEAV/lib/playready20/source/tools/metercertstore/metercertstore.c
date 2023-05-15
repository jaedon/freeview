/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

#include <stdio.h> /* for missing CRT functions like printf */
#include <drmcommon.h>
#include <drmutilities.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <drmcrt.h>
#include <drmcmdlnpars.h>
#include <drmtoolsutils.h>
#include <drmmeterapi.h>
#include <drmtoolsinit.h>

DRM_BOOL DRM_CALL _LoadFile(const DRM_WCHAR* pwszFile, DRM_BYTE **ppBuffer, DRM_DWORD *pSize)
{
    DRM_BOOL fRet = FALSE;
    OEM_FILEHDL hFile = OEM_INVALID_HANDLE_VALUE;
    DRM_DWORD cbRead = 0;

    if (pwszFile == NULL || ppBuffer == NULL ||pSize == NULL)
    {
        goto ErrorExit;
    }

    *pSize = 0;
    *ppBuffer = 0;

    hFile = Oem_File_Open(NULL, pwszFile, OEM_GENERIC_READ, OEM_FILE_SHARE_READ, OEM_OPEN_EXISTING, OEM_ATTRIBUTE_NORMAL);
    if (OEM_INVALID_HANDLE_VALUE == hFile)
    {
        goto ErrorExit;
    }

    if (!Oem_File_GetSize(hFile, pSize))
    {
        goto ErrorExit;
    }

    if ((*ppBuffer = (DRM_BYTE*) Oem_MemAlloc(*pSize + SIZEOF(DRM_WCHAR))) == NULL)
    {
        goto ErrorExit;
    }

    MEMSET(*ppBuffer, 0, SIZEOF(DRM_WCHAR) + *pSize);

    fRet = Oem_File_Read(hFile, *ppBuffer, *pSize, &cbRead);

    if( cbRead != *pSize )
    {
        fRet = FALSE;
    }

ErrorExit:
    if (hFile != OEM_INVALID_HANDLE_VALUE)
    {
        Oem_File_Close(hFile);
    }
    return fRet;
}

#define METERCERT_BUFFER 10000
#define URL_BUFFER 5000
DRM_RESULT DoMeterCert (DRM_CONST_STRING *f_pdstrXMLMeterCert,
                               DRM_CONST_STRING *f_pdstrFileDataStore)
{
    DRM_APP_CONTEXT     *pcontextManager    = NULL;
    DRM_BYTE            *pbOpaqueBuffer     = NULL;
    DRM_METERCERT_ENUM   oMeterCertEnum     = {0};
    DRM_RESULT           dr                 = DRM_SUCCESS;
    DRM_WCHAR           *pwszMeterCert      = NULL;
    DRM_DWORD            cchMeterCert       = 0;
    DRM_DWORD            cbMeterCert        = 0;
    DRM_DWORD            cchURL             = 0;
    DRM_WCHAR           *pwszURL            = NULL;
    DRM_MID              oMID               = {0};
    DRM_BOOL             fIsV2              = FALSE;

    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, f_pdstrFileDataStore, TOOLS_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &pcontextManager ) );

    if( f_pdstrXMLMeterCert == NULL
     || f_pdstrXMLMeterCert->cchString == 0 )
    {
        ChkDR( Drm_MeterCert_InitEnum(pcontextManager, &oMeterCertEnum) );

        ChkMem(pwszMeterCert = (DRM_WCHAR *) Oem_MemAlloc( METERCERT_BUFFER ));
        ChkMem(pwszURL       = (DRM_WCHAR *) Oem_MemAlloc(URL_BUFFER * SIZEOF (DRM_WCHAR)));

        do
        {
            cbMeterCert = METERCERT_BUFFER;
            cchURL = URL_BUFFER;
            dr = Drm_MeterCert_EnumNext(&oMeterCertEnum,
                                            &oMID,
                                             pwszURL,
                                            &cchURL,
                                            &fIsV2,
                                             ( DRM_BYTE * )pwszMeterCert,
                                            &cbMeterCert);

            if( dr == DRM_E_NOMORE )
            {
                dr = DRM_SUCCESS;
                break;
            }
            ChkDR( dr );

            cchMeterCert = cbMeterCert / SIZEOF( DRM_WCHAR );

            pwszMeterCert[cchMeterCert] = 0;
            pwszURL[cchURL] = 0;
            printf ("Metercert:\r\n%S", (DRM_WCHAR *) pwszMeterCert);
            printf ("Acquisiton URL:\r\n%S", (DRM_WCHAR *) pwszURL);
            printf ("Is V2 server: %d\r\n", fIsV2 );
            printf("\r\n\r\n");
        } while( TRUE );
    }
    else
    {
        DRM_CONST_STRING dstrMeterCert = EMPTY_DRM_STRING;

        ChkBOOL( _LoadFile( f_pdstrXMLMeterCert->pwszString, (DRM_BYTE**)&dstrMeterCert.pwszString, &dstrMeterCert.cchString), DRM_E_FILEOPEN );
        dstrMeterCert.cchString = dstrMeterCert.cchString / SIZEOF( DRM_WCHAR );
        ChkDR( Drm_MeterCert_Update( pcontextManager, PB_DSTR( &dstrMeterCert ), CB_DSTR( &dstrMeterCert ), &oMID ) );
    }

ErrorExit:
    SAFE_OEM_FREE(pwszMeterCert);
    SAFE_OEM_FREE(pwszURL);

    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &pcontextManager );

    return dr;
}

/******************************************************************************
** Print a command menu to console
*******************************************************************************/

static void PrintUsage( const DRM_WCHAR *pwszAppName )
{
    printf ("Enumerates metering certificates and acquisition URLs, or if a metercert file is specified will verify and add it to the store.\r\n\r\nSyntax: %S [-?] -s:DataStoreName [-m:metercert.xml]\n", pwszAppName);
}


DRM_LONG DRM_CALL DRM_Main (DRM_LONG argc, DRM_WCHAR **argv )
{
    DRM_RESULT  dr       = DRM_SUCCESS;
    DRM_LONG    iArg     = 0;
    OEM_FILEHDL hfile    = OEM_INVALID_HANDLE_VALUE;
    DRM_WCHAR  *pwchFile = NULL;
    DRM_CONST_STRING dstrStoreName     = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrFileMeterCert = EMPTY_DRM_STRING;

    PrintCopyrightInfo( "DRM Metering Certificate Tool" );

    if( DRM_METERING_IsMeteringSupported() )
    {
        if ( argc <= 1 )
        {
            dr = DRM_S_MORE_DATA;
            goto _PrintUsage;
        }

        for (iArg = 1; iArg < argc; iArg++)
        {
            DRM_WCHAR        wchOption = 0;
            DRM_CONST_STRING dstrParam = EMPTY_DRM_STRING;

            if( !DRM_CMD_ParseCmdLine(argv [iArg], &wchOption, &dstrParam, NULL))
            {
                /* See if it's a default parameter that PK knows how to handle */
                if ( DRM_FAILED( DRM_CMD_TryProcessDefaultOption( argv[iArg] ) ) )
                {
                    dr = DRM_E_INVALID_COMMAND_LINE;
                    goto _PrintUsage;
                }
            }
            else
            {
                wchOption = DRMCRT_towlower(wchOption);

                switch (wchOption)
                {
                case WCHAR_CAST('s'):
                    ASSIGN_DRM_STRING(dstrStoreName, dstrParam);
                    break;

                case WCHAR_CAST('m'):
                    ASSIGN_DRM_STRING(dstrFileMeterCert, dstrParam);
                    break;

                case WCHAR_CAST('?'):
                    goto _PrintUsage;

                default:
                    dr = DRM_E_INVALID_COMMAND_LINE;
                    goto _PrintUsage;
                }
            }
        }

        if( dstrStoreName.cchString == 0 )
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        if( dstrStoreName    .cchString > (DRM_MAX_PATH-1)
         || dstrFileMeterCert.cchString > (DRM_MAX_PATH-1) )
        {
            printf ("File name too long. Max length is %d.\n", DRM_MAX_PATH - 1);
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        ChkDR( DoMeterCert( &dstrFileMeterCert, &dstrStoreName ) );
    }
    else
    {
        printf( "This tool requires that the real metering library be linked to it.\r\n" );
        ChkDR( DRM_E_METERING_NOT_SUPPORTED );
    }

ErrorExit:
    if( hfile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close( hfile );
    }
    if( pwchFile != NULL )
    {
        Oem_MemFree (pwchFile);
    }

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
    PrintUsage (argv [0]);
    return dr;
}

