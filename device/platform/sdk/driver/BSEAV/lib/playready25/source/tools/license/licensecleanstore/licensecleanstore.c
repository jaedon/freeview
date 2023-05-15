/**@@@+++@@@@******************************************************************
**
** Microsoft (r) PlayReady (r)
** Copyright (c) Microsoft Corporation. All rights reserved.
**
***@@@---@@@@******************************************************************
*/

/**
 ** DESCRIPTION: Tool to cleanup a license store.
 **              Involving two main steps:
 **                 - Delete licenses not required any more
 **                 - Defrag the store
 **
 **/

#include <stdio.h>

#include <drmcrt.h>
#include <drmutilities.h>
#include <drmcontextsizes.h>
#include <drmcmdlnpars.h>
#include <drmmanager.h>
#include <drmtoolsutils.h>
#include <drmtoolsinit.h>
#include <drmconstants.h>

/******************************************************************************
** Print a command menu to console
*******************************************************************************/
static void PrintUsage( const DRM_WCHAR *pwszAppName )
{
    printf("Syntax: %S [-?] [-s:DataStoreName]\n", pwszAppName);
}

/********************************************************************
** Function :   ReportProgress
** Synopsis :   Callback function to report progress
*********************************************************************/
DRM_RESULT DRM_CALL  ReportProgress(
    __in const DRM_VOID  *pvCallerData,
    __in       DRM_DWORD  cLicensesProcessed,
    __in       DRM_DWORD  cTotalLicenses  /*  # of licenses remaining */
    )
{
    DRM_RESULT   dr = DRM_SUCCESS;

    printf("%d license(s) out of %d licenses processed\n", cLicensesProcessed, cTotalLicenses );
    if (pvCallerData)
    {
        dr = DRM_E_DEVICE_NOT_INITIALIZED;
    }

   return dr;
}

/********************************************************************
** Function :   DRM_Main
** Synopsis :   Cleanup a license store. Involving two main steps:
**              - Delete licenses not required any more
**              - Defrag the store
*********************************************************************/
DRM_LONG DRM_CALL DRM_Main( DRM_LONG argc, DRM_WCHAR **argv )
{
    DRM_CONST_STRING     dstrStoreName      = EMPTY_DRM_STRING;
    DRM_LONG             i                  = 0;
    DRM_APP_CONTEXT     *poManagerContext   = NULL;
    DRM_BYTE            *pbOpaqueBuffer     = NULL;
    DRM_RESULT           dr                 = DRM_SUCCESS;

    PrintCopyrightInfo( "DRM License store cleanup tool" );

    if ( argc <= 1 )
    {
        dr = DRM_S_MORE_DATA;
        goto _PrintUsage;
    }

    ChkArg( argv != NULL );
    for( i = 1; i < argc; i++ )
    {
        DRM_WCHAR           wchOption = 0;
        DRM_CONST_STRING    dstrParam = EMPTY_DRM_STRING;

        if ( !DRM_CMD_ParseCmdLine(argv[i], &wchOption, &dstrParam, NULL) )
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
            switch( wchOption )
            {
            case WCHAR_CAST('s'):
                dstrStoreName = dstrParam;
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

    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, &dstrStoreName, TOOLS_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &poManagerContext ) );

    ChkDR( Drm_StoreMgmt_CleanupStore( poManagerContext,
                                       DRM_STORE_CLEANUP_ALL,
                                       NULL,
                                       5,
                                       ReportProgress));
    printf( "100 percent done\n" );

ErrorExit:
    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &poManagerContext );
    if ( DRM_FAILED(dr) )
    {
        printf( "Error: 0x%08X\n", dr );
    }
    return dr;

_PrintUsage:
    PrintUsage(argv == NULL ? &g_wchNull : argv[0]);
    return dr;
}
