/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/
#include <stdio.h> /* for missing CRT functions like printf */

#include <drmutilities.h>
#include <drmcontextsizes.h>
#include <drmmanager.h>
#include <drmcrt.h>
#include <drmcmdlnpars.h>
#include <drmtoolsutils.h>
#include <drmmeterapi.h>
#include <drmtoolsinit.h>

DRM_RESULT DumpMeteringForMID (      DRM_CONST_STRING *f_pdstrXMLMeterCert,
                               const DRM_CONST_STRING *f_pdstrFileChallenge,
                               const DRM_CONST_STRING *f_pdstrFileURL,
                               const DRM_CONST_STRING *f_pdstrFileDataStore)
{
    DRM_APP_CONTEXT *pcontextManager    = NULL;
    DRM_BYTE        *pbOpaqueBuffer     = NULL;
    DRM_RESULT       dr                 = DRM_SUCCESS;
    _XMBContext     *pbMeteringData     = NULL;
    DRM_DWORD        cbMeteringData     = 0;
    DRM_DWORD        cchURL             = 0;
    DRM_CHAR        *pszURL             = NULL;
    OEM_FILEHDL      hfile              = OEM_INVALID_HANDLE_VALUE;

    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, f_pdstrFileDataStore, TOOLS_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &pcontextManager ) );

    dr = Drm_Metering_GenerateChallenge(pcontextManager,
                                        PB_DSTR(f_pdstrXMLMeterCert),
                                        CB_DSTR(f_pdstrXMLMeterCert),
                                        NULL,
                                        0,
                                        NULL,
                                       &cchURL,
                                        NULL,
                                       &cbMeteringData);

    if (dr != DRM_E_BUFFERTOOSMALL)
    {
        ChkDR(dr);
    }

    ChkMem(pbMeteringData = (_XMBContext *) Oem_MemAlloc(cbMeteringData));
    ChkMem(pszURL         = (DRM_CHAR *) Oem_MemAlloc(cchURL));

    ChkDR(Drm_Metering_GenerateChallenge(pcontextManager,
                                         PB_DSTR(f_pdstrXMLMeterCert),
                                         CB_DSTR(f_pdstrXMLMeterCert),
                                         NULL,
                                         0,
                                         pszURL,
                                        &cchURL,
                              (DRM_BYTE*)pbMeteringData,
                                        &cbMeteringData));

    printf ("output:\n%S", (DRM_WCHAR *) pbMeteringData);

    if ((hfile = Oem_File_Open(NULL,
                              f_pdstrFileChallenge->pwszString,
                              OEM_GENERIC_WRITE,
                              OEM_FILE_SHARE_READ,
                              OEM_CREATE_ALWAYS,
                              OEM_ATTRIBUTE_NORMAL)) != OEM_INVALID_HANDLE_VALUE)
    {
        DRM_DWORD cbWritten    = 0;

        Oem_File_Write(hfile, pbMeteringData, cbMeteringData, &cbWritten);
        Oem_File_Close(hfile);
    }
    else
    {
        printf ("error opening file %S\n", f_pdstrFileChallenge->pwszString);
    }

    /* if requested write URL to a separate file */

    if (f_pdstrFileURL->cchString > 0)
    {
        OEM_FILEHDL hfileURL = OEM_INVALID_HANDLE_VALUE;

        hfileURL = Oem_File_Open(NULL,
                                f_pdstrFileURL->pwszString,
                                OEM_GENERIC_WRITE,
                                OEM_FILE_SHARE_READ,
                                OEM_CREATE_ALWAYS,
                                OEM_ATTRIBUTE_NORMAL);

        if (hfileURL != OEM_INVALID_HANDLE_VALUE)
        {
            DRM_DWORD cbWritten    = 0;

            Oem_File_Write(hfileURL, pszURL, cchURL, &cbWritten);
            Oem_File_Close(hfileURL);
        }
        else
        {
            printf("error opening file %S\n", f_pdstrFileURL->pwszString);
        }
    }
ErrorExit:

    if (pbMeteringData != NULL)
    {
        Oem_MemFree(pbMeteringData);
    }

    if (pszURL != NULL)
    {
        Oem_MemFree(pszURL);
    }

    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &pcontextManager );

    return dr;
}

/******************************************************************************
** Print a command menu to console
*******************************************************************************/

static void PrintUsage( const DRM_WCHAR *pwszAppName )
{
    printf ("Syntax: %S [-?] -s:DataStoreName -m:metercert.xml -o:OutputFile [-u:URLFile]\n", pwszAppName);
}

#if EMBEDDED_WITH_NO_PARAMS

/* metergen.exe */
DRM_WCHAR g_Arg1 [] = { ONE_WCHAR('m', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
/*-s:datastore.hds */
DRM_WCHAR g_Arg2 [] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')};
/*-m:metercertsignedU.xml */
DRM_WCHAR g_Arg3 [] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('U', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('\0', '\0')};
/*-o:challenge.xml*/
DRM_WCHAR g_Arg4 [] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('\0', '\0')};
/*-u:url.txt*/
DRM_WCHAR g_Arg5 [] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('\0', '\0')};

DRM_WCHAR *g_argv [] =
{
    g_Arg1,
    g_Arg2,
    g_Arg3,
    g_Arg4,
    g_Arg5
};

DRM_LONG  g_argc = 5;

#endif

DRM_LONG DRM_CALL DRM_Main( DRM_LONG argc, DRM_WCHAR **argv )
{
    DRM_RESULT dr = DRM_SUCCESS;
    DRM_CONST_STRING dstrStoreName      = EMPTY_DRM_STRING,
                     dstrOutput         = EMPTY_DRM_STRING,
                     dstrFileMeterCert  = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrXMLMeterCert   = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrFileURL        = EMPTY_DRM_STRING;
    DRM_LONG         iArg               = 0;
    OEM_FILEHDL hfile    = OEM_INVALID_HANDLE_VALUE;
    DRM_WCHAR  *pwchFile = NULL;
    DRM_DWORD   cbFile   = 0,
                cbRead   = 0;

    PrintCopyrightInfo( "DRM Metering store dump tool" );

#if EMBEDDED_WITH_NO_PARAMS
    argc = g_argc;
    argv = g_argv;
#endif

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

                case WCHAR_CAST('o'):
                    ASSIGN_DRM_STRING(dstrOutput, dstrParam);
                    break;

                case WCHAR_CAST('u'):
                    ASSIGN_DRM_STRING(dstrFileURL, dstrParam);
                    break;

                case WCHAR_CAST('?'):
                    goto _PrintUsage;

                default:
                    dr = DRM_E_INVALID_COMMAND_LINE;
                    goto _PrintUsage;
                }
            }
        }

        if( dstrStoreName    .cchString == 0
         || dstrFileMeterCert.cchString == 0
         || dstrOutput       .cchString == 0 )
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        if( dstrStoreName    .cchString > (DRM_MAX_PATH-1)
         || dstrFileMeterCert.cchString > (DRM_MAX_PATH-1)
         || dstrOutput       .cchString > (DRM_MAX_PATH-1) )
        {
            printf ("File name too long. Max length is %d.\n", DRM_MAX_PATH - 1);
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        hfile = Oem_File_Open (NULL,
                              dstrFileMeterCert.pwszString,
                              OEM_GENERIC_READ,
                              OEM_FILE_SHARE_NONE,
                              OEM_OPEN_EXISTING,
                              OEM_ATTRIBUTE_NORMAL);
        if( hfile == OEM_INVALID_HANDLE_VALUE)
        {
            printf( "Input file %S could not open.\n", dstrFileMeterCert.pwszString );
            ChkDR( DRM_E_FILENOTFOUND );
        }

        if (!Oem_File_GetSize (hfile, &cbFile))
        {
            printf ("Unable to get size for file %S.\n", dstrFileMeterCert.pwszString);
            ChkDR( DRM_E_FAIL );
        }

        ChkMem(pwchFile = (DRM_WCHAR*)Oem_MemAlloc (cbFile));
        if (Oem_File_Read (hfile, pwchFile, cbFile, &cbRead))
        {
            DSTR_FROM_PB( &dstrXMLMeterCert, pwchFile, cbFile );
            ChkDR( DumpMeteringForMID( &dstrXMLMeterCert,
                                       &dstrOutput,
                                       &dstrFileURL,
                                       &dstrStoreName ) );
        }
        else
        {
            printf ("Unable to read input file %S.\n", dstrFileMeterCert.pwszString);
            ChkDR( DRM_E_FAIL );
        }
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

