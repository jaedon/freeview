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
#include <drmxmlparser.h>
#include <drmxmlbuilder.h>
#include <drmtoolsutils.h>
#include <drmmeterapi.h>
#include <drmtoolsinit.h>

/*    <METERDATA type="response">
        <DATA>
            <MID>UiW5yBMep2CuevGg5+FgA3==</MID>
            <TID>Mep2CuevGgUiW5yB5+FgA3==</TID>
            <COMMAND>RESET</COMMAND>
            <RECORDS>
                <KID value="KID1" />
                <KID value="KID2" />
                ...
            </RECORDS>
            <PARTIALDATA>1</PARTIALDATA>
        </DATA>
        <HASH>
            <HASHALGORITHM type="HMAC" />
            <VALUE>
                Base64-encoded hash
            </VALUE>
        </HASH>
    </METERDATA> */

/******************************************************************************
** Function:    _drm_ProcessMeterResponse
**
** Synopsis:    Processes metering response, stores it in data store
**
** Arguments:   [pdstrMeterResp]    - String with metering response
**              [pflagsResponse]    - Pointer to flag filled with data if
                [pdstrDataStore]    - Path the data store file.
**
** Returns:     DRM_SUCCESS        - on success
**              Error from Drm_Metering_ProcessResponse of from Drm_Initialize
**
** Notes:       None.
******************************************************************************/

static DRM_RESULT _drm_ProcessMeterResponse (__in_ecount ( 1 ) const  DRM_CONST_STRING  *pdstrMeterResp,
                                             __out_ecount( 1 )        DRM_DWORD         *pflagsResponse,
                                             __in_ecount ( 1 ) const  DRM_CONST_STRING  *pdstrDataStore)
{
    DRM_APP_CONTEXT *pcontextManager = NULL;
    DRM_BYTE        *pbOpaqueBuffer  = NULL;
    DRM_RESULT       dr              = DRM_SUCCESS;

    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, pdstrDataStore, TOOLS_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &pcontextManager ) );

    ChkDR  (Drm_Metering_ProcessResponse (pcontextManager,
                             (DRM_BYTE *) PB_DSTR (pdstrMeterResp),
                                          CB_DSTR (pdstrMeterResp),
                                          pflagsResponse));
ErrorExit:
    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &pcontextManager );
    return dr;
}

/******************************************************************************
** Print a command menu to console
*******************************************************************************/

static void PrintUsage( const DRM_WCHAR *pwszAppName )
{
    printf ("Syntax: %S [-?] -s:DataStoreName -i:meterreponse.xml \n", pwszAppName);
}

#if EMBEDDED_WITH_NO_PARAMS

/* meterresp.exe */
DRM_WCHAR g_Arg1 [] = { ONE_WCHAR('m', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('\0', '\0')};
/*-s:datastore.hds */
DRM_WCHAR g_Arg2 [] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('t', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('s', '\0'), ONE_WCHAR('\0', '\0')};
/*-i:response.xml*/
DRM_WCHAR g_Arg3 [] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('i', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('r', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('o', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('e', '\0'),  ONE_WCHAR('.', '\0'),  ONE_WCHAR('x', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('\0', '\0')};

DRM_WCHAR *g_argv [] =
{
    g_Arg1,
    g_Arg2,
    g_Arg3
};

DRM_LONG g_argc = 3;

#endif

DRM_LONG DRM_CALL DRM_Main (DRM_LONG argc, DRM_WCHAR **argv)
{
    DRM_RESULT          dr            = DRM_SUCCESS;
    DRM_LONG            iArg          = 0;
    OEM_FILEHDL         hfile         = OEM_INVALID_HANDLE_VALUE;
    DRM_CONST_STRING    dstrFile      = EMPTY_DRM_STRING;
    DRM_DWORD           cbFile        = 0,
                        cbRead        = 0,
                        flagsResponse = 0;
    DRM_CONST_STRING    dstrStoreName = EMPTY_DRM_STRING,
                        dstrFileMeterResp  = EMPTY_DRM_STRING;

    PrintCopyrightInfo( "DRM metering response tool" );

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

            if(!DRM_CMD_ParseCmdLine(argv [iArg], &wchOption, &dstrParam, NULL))
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
                case WCHAR_CAST('i'):
                    dstrFileMeterResp = dstrParam;
                    break;
                case WCHAR_CAST('?'):
                    goto _PrintUsage;
                default:
                    dr = DRM_E_INVALID_COMMAND_LINE;
                    goto _PrintUsage;
                }
            }
        }

        if ( dstrStoreName    .cchString == 0
          || dstrFileMeterResp.cchString == 0)
        {
            /* argument not supplied */
            printf ("Missing arguments.\n");
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }

        hfile = Oem_File_Open (NULL,
                              dstrFileMeterResp.pwszString,
                              OEM_GENERIC_READ,
                              OEM_FILE_SHARE_NONE,
                              OEM_OPEN_EXISTING,
                              OEM_ATTRIBUTE_NORMAL);
        if( hfile == OEM_INVALID_HANDLE_VALUE)
        {
            printf( "Unable to open file %S\n", dstrFileMeterResp.pwszString );
            ChkDR( DRM_E_FILENOTFOUND );
        }
        if (!Oem_File_GetSize (hfile, &cbFile))
        {
            printf( "Unable to get the size of file %S\n", dstrFileMeterResp.pwszString );
            ChkDR( DRM_E_FILEREADERROR );
        }

        ChkMem( dstrFile.pwszString = (DRM_WCHAR*)Oem_MemAlloc (cbFile));
        if (!Oem_File_Read (hfile, (DRM_VOID *)dstrFile.pwszString, cbFile, &cbRead))
        {
            printf ("Unable to input file read file %S.\n", dstrFileMeterResp.pwszString);
            ChkDR( DRM_E_FILEREADERROR );
        }
         dstrFile.cchString = cbRead / SIZEOF( DRM_WCHAR );

        ChkDR(_drm_ProcessMeterResponse( &dstrFile, &flagsResponse, &dstrStoreName));
        printf ("Success processing file %S.\nPartial Data: %s\n", dstrFileMeterResp.pwszString,
                 ((flagsResponse & DRM_METER_RESPONSE_PARTIAL) ? "yes" : "no"));
    }
    else
    {
        printf( "This tool requires that the real metering library be linked to it.\r\n" );
        ChkDR( DRM_E_METERING_NOT_SUPPORTED );
    }

ErrorExit:
    if( hfile != OEM_INVALID_HANDLE_VALUE )
    {
        Oem_File_Close (hfile);
    }
    if( dstrFile.pwszString != NULL )
    {
        Oem_MemFree( (DRM_VOID *)dstrFile.pwszString );
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

