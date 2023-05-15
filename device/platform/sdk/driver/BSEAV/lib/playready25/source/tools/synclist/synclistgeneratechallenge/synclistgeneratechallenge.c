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
#include <drmtoolsinit.h>
#include <drmsyncstore.h>
#include <drmconstants.h>

DRM_RESULT GenerateSyncChallenge (const DRM_WCHAR        *pszOutput,
                                  const DRM_CONST_STRING *pdstrDataStore,
                                        DRM_DWORD         cCountMax,
                                        DRM_DWORD         cHoursMax)
{
    DRM_APP_CONTEXT     *pcontextManager = NULL;
    DRM_BYTE            *pbOpaqueBuffer  = NULL;
    DRM_RESULT           dr              = DRM_SUCCESS;
    DRM_BYTE            *pbXML           = NULL;
    DRM_DWORD            cbXML           = 0;
    DRM_DWORD            cKIDs           = 0;
    DRM_DWORD            iKIDNext        = 0;
    OEM_FILEHDL          hfile           = OEM_INVALID_HANDLE_VALUE;

    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, pdstrDataStore, TOOLS_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &pcontextManager ) );

    dr = Drm_Sync_GenerateChallenge (pcontextManager,
                                        cCountMax,
                                        cHoursMax,
                                        0,
                                        1000,
                                       &cKIDs,
                                       &iKIDNext,
                                        NULL,
                                       &cbXML);

    ChkFAIL (dr == DRM_E_BUFFERTOOSMALL
          || DRM_SUCCEEDED (dr));

    ChkMem (pbXML = (DRM_BYTE *)Oem_MemAlloc (cbXML));

    ChkDR  (Drm_Sync_GenerateChallenge (pcontextManager,
                                           cCountMax,
                                           cHoursMax,
                                           0,
                                           1000,
                                          &cKIDs,
                                          &iKIDNext,
                                (DRM_BYTE*)pbXML,
                                          &cbXML));

    printf ("output:\n%S", (DRM_WCHAR *) pbXML);

    if ((hfile = Oem_File_Open (NULL, pszOutput,
                               OEM_GENERIC_WRITE,
                               OEM_FILE_SHARE_READ,
                               OEM_CREATE_ALWAYS,
                               OEM_ATTRIBUTE_NORMAL)) != OEM_INVALID_HANDLE_VALUE)
    {
        DRM_DWORD       cbWritten    = 0;

        Oem_File_Write (hfile, pbXML, cbXML, &cbWritten);
        Oem_File_Close (hfile);
    }
    else
    {
        printf ("error opening file %S", pszOutput);
    }

ErrorExit:

    if (pbXML != NULL)
    {
        Oem_MemFree (pbXML);
    }

    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &pcontextManager );

    return dr;
}

/******************************************************************************
** Print a command menu to console
*******************************************************************************/

static void PrintUsage( const DRM_WCHAR *pwszAppName )
{
    printf ("Syntax: %S [-?] -s:DataStoreName -o:OutputFile [-c:MaxCount] [-h:MaxHours]]\n", pwszAppName);
}

#if EMBEDDED_WITH_NO_PARAMS

/* syncchal.exe */
DRM_WCHAR g_Arg1 [] = { ONE_WCHAR('s', '\0'),  ONE_WCHAR('y', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
/*-s:datastore.hds */
DRM_WCHAR g_Arg2 [] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')};
/*-o:challenge.xml*/
DRM_WCHAR g_Arg3 [] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('g', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('\0', '\0')};

DRM_WCHAR *g_argv [] =
{
    g_Arg1,
    g_Arg2,
    g_Arg3
};

DRM_LONG g_argc = 3;

#endif

DRM_LONG DRM_CALL DRM_Main( DRM_LONG argc, DRM_WCHAR **argv )
{
    DRM_RESULT dr                       = DRM_SUCCESS;
    DRM_CONST_STRING dstrStoreName      = EMPTY_DRM_STRING,
                     dstrOutput         = EMPTY_DRM_STRING;
    DRM_LONG         iArg               = 0;
    DRM_DWORD        cHoursMax          = DRM_SYNC_IGNORE_THRESHOLD_PARAMETER,
                     cCountMax          = DRM_SYNC_IGNORE_THRESHOLD_PARAMETER;

    PrintCopyrightInfo( "DRM synclist challenge generation tool" );

    if( DRM_SNC_IsLicenseSyncSupported() )
    {
#if EMBEDDED_WITH_NO_PARAMS
        argc = g_argc;
        argv = g_argv;
#endif

        if ( argc <= 1 )
        {
            dr = DRM_S_MORE_DATA;
            goto _PrintUsage;
        }

        ChkArg( argv != NULL );
        for (iArg = 1; iArg < argc; iArg++)
        {
            DRM_WCHAR        wchOption = 0;
            DRM_CONST_STRING dstrParam = EMPTY_DRM_STRING;

            if (!DRM_CMD_ParseCmdLine (argv [iArg], &wchOption, &dstrParam, NULL))
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
                wchOption = DRMCRT_towlower (wchOption);

                switch (wchOption)
                {
                    case WCHAR_CAST('s'):
                        dstrStoreName = dstrParam;
                        break;

                    case WCHAR_CAST('o'):
                        dstrOutput = dstrParam;
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
        }
        if (dstrStoreName.cchString == 0
          ||dstrOutput   .cchString == 0)
        {
            printf( "Missing arguments.\n");
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        if (dstrStoreName.cchString > (DRM_MAX_PATH - 1))
        {
            printf ("Data Store name is too long. Max length is %d.\n", DRM_MAX_PATH - 1);
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        ChkDR( GenerateSyncChallenge( dstrOutput.pwszString,
                                     &dstrStoreName,
                                      cCountMax,
                                      cHoursMax ) );
    }
    else
    {
        printf( "This tool requires that the real license sync library be linked to it.\r\n" );
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
    PrintUsage (argv == NULL ? &g_wchNull : argv [0]);
    return dr;
}

