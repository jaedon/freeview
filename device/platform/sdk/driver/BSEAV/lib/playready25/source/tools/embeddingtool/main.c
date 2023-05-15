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
#include <drmplayreadyobj.h>
#include <drmtoolsmediafile.h>
#include <drmtoolsconstants.h>
#include <drmxmlparser.h>
#include <drmtoolsinit.h>
#include <drmconstants.h>

/******************************************************************************
** Print a command menu to console
*******************************************************************************
*/
static void PrintUsage( const DRM_WCHAR *pwszAppName )
{
    printf("\
Syntax: %S [-?] -s:HDSFile -c:ContentFile \n\
Updates ContentFile with licenses from HDSFile if embeddable licenses are available.\n\
", pwszAppName);
}

/******************************************************************************
** Main Function.
*******************************************************************************
*/
DRM_LONG DRM_CALL DRM_Main( DRM_LONG argc, DRM_WCHAR** argv )
{
    DRM_CONST_STRING             dstrStoreName      = EMPTY_DRM_STRING;
    DRM_CONST_STRING             dstrContentFile    = EMPTY_DRM_STRING;
    DRM_APP_CONTEXT             *poManagerContext   = NULL;
    DRM_BYTE                    *pbOpaqueBuffer     = NULL;
    DRM_MEDIA_FILE_CONTEXT      *poMediaFile        = NULL;
    DRM_BOOL                     fFileOpen          = FALSE;
    DRM_LONG                     i                  = 0;
    DRM_RESULT                   dr                 = DRM_SUCCESS;

    PrintCopyrightInfo( "Sample DRM license embedding application" );

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
    for( i = 1; i < argc ; i++ )
    {
        DRM_WCHAR wchOptionChar = ONE_WCHAR( '\0', '\0' );
        DRM_CONST_STRING dstrParam = EMPTY_DRM_STRING;

        if ( !DRM_CMD_ParseCmdLine(argv[i], &wchOptionChar, &dstrParam, NULL) )
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }
        wchOptionChar = DRMCRT_towlower( wchOptionChar );
        switch( wchOptionChar )
        {
        case WCHAR_CAST('\0'):
        case WCHAR_CAST('c'):
            dstrContentFile.pwszString = dstrParam.pwszString;
            dstrContentFile.cchString  = dstrParam.cchString;
            break;
        case WCHAR_CAST('s'):
            dstrStoreName.pwszString = dstrParam.pwszString;
            dstrStoreName.cchString  = dstrParam.cchString;
            break;
        case WCHAR_CAST('?'):
            goto _PrintUsage;
        default:
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }
    }

    if( ( dstrContentFile.cchString == 0 )
     || ( dstrStoreName.cchString   == 0 ) )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    ChkDR( DRMTOOLS_DrmInitializeWithOpaqueBuffer( NULL, &dstrStoreName, TOOLS_DRM_BUFFER_SIZE, &pbOpaqueBuffer, &poManagerContext ) );

    ChkMem( poMediaFile = ( DRM_MEDIA_FILE_CONTEXT * )Oem_MemAlloc( SIZEOF(DRM_MEDIA_FILE_CONTEXT) ) );
    ZEROMEM( poMediaFile, SIZEOF (DRM_MEDIA_FILE_CONTEXT));

    ChkDR( DRMTOOLS_OpenMediaFile( &dstrContentFile, poMediaFile ) );
    fFileOpen = TRUE;
    ChkDR( Drm_Content_SetProperty( poManagerContext,
                                    DRM_CSP_AUTODETECT_HEADER,
                                    poMediaFile->rgbDRMHeaderData,
                                    poMediaFile->cbDRMHeaderData ) );

    ChkDR( Drm_Content_UpdateEmbeddedStore( poManagerContext ) );
    ChkDR( Drm_Content_GetProperty( poManagerContext,
                                    DRM_CGP_PLAYREADY_OBJ,
                                    poMediaFile->rgbDRMHeaderData,
                                   &poMediaFile->cbDRMHeaderData ) );
    ChkDR( DRMTOOLS_UpdatePlayReadyObject( poMediaFile ) );

ErrorExit:
    if ( DRM_SUCCEEDED( dr ) )
    {
        printf( "Success!\r\n" );
    }
    else
    {
        printf( "Failed with error code: 0x%X\r\n.", dr );
    }
    if ( fFileOpen )
    {
        DRMTOOLS_CloseMediaFile( poMediaFile );
    }
    DRMTOOLS_DrmUninitializeWithOpaqueBuffer( &pbOpaqueBuffer, &poManagerContext );
    return dr;

_PrintUsage:
    PrintUsage(argv == NULL ? &g_wchNull : argv[0]);
    return dr;
}

