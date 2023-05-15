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
#include <drmcontextsizes.h>
#include <drmcmdlnpars.h>
#include <drmcrt.h>
#include <drmtoolsutils.h>
#include <drmdatastore.h>
#include <drmhds.h>

/*Global DST file name*/
DRM_WCHAR g_wszDeviceStoreName[DRM_MAX_PATH];

/* DRM_WCHAR g_wszDeviceStoreName[] = { ONE_WCHAR('-', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('c', '\0'),  ONE_WCHAR(':', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('j', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('n', '\0'),  ONE_WCHAR('u', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('\\', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('a', '\0'),  ONE_WCHAR('m', '\0'),  ONE_WCHAR('p', '\0'),  ONE_WCHAR('l', '\0'),  ONE_WCHAR('e', '\0'), ONE_WCHAR('.', '\0'),  ONE_WCHAR('h', '\0'),  ONE_WCHAR('d', '\0'),  ONE_WCHAR('s', '\0'),  ONE_WCHAR('\0', '\0')}; */

DRM_RESULT DumpLicenses( DRM_KID* pKid )
{
    DRM_RESULT                dr          = DRM_SUCCESS;
    DRM_BYTE                 *pbLicense   = NULL;
    DRM_DWORD                 cbLicense   = 0;
    DRM_KID                   oKID;
    DRM_LID                   oLID;
    DRM_DST                   oDatastore  = { eDRM_DST_NONE, { 0 }, NULL };
    DRM_DST_CONTEXT          *pDSTContext = NULL;
    DRM_LICSTOREENUM_CONTEXT *pLicEnum    = NULL;
    DRM_LICSTORE_CONTEXT     *pcontextLST = NULL;
    DRM_BOOL                  fOpened     = FALSE;

    ChkMem( pDSTContext = (DRM_DST_CONTEXT*)      Oem_MemAlloc( SIZEOF( DRM_DST_CONTEXT ) ) );
    ChkMem( pcontextLST = (DRM_LICSTORE_CONTEXT*) Oem_MemAlloc( SIZEOF( DRM_LICSTORE_CONTEXT ) ) );

    ZEROMEM( pDSTContext, SIZEOF( *pDSTContext ) );
    ChkDR( DRM_DST_Init( eDRM_DST_HDS, pDSTContext, SIZEOF( DRM_DST_CONTEXT ), &oDatastore ) );

    dr = DRM_DST_OpenStore( NULL,
              ( DRM_BYTE * )g_wszDeviceStoreName,
                            NO_OF( g_wszDeviceStoreName ) * SIZEOF( DRM_WCHAR ),
                            0,
                            &oDatastore );

    if ( dr == DRM_E_FILENOTFOUND )   /* store does not exist */
    {
        printf("Can not find data store\n ");
    }

    ChkDR(dr);

    fOpened = TRUE;

    ChkDR( DRM_LST_Open(pcontextLST, &oDatastore, eDRM_LICENSE_STORE_XML) ); /* Default to XML */

    /* Query for licenses based on KID */
    ChkMem( pLicEnum = (DRM_LICSTOREENUM_CONTEXT *)Oem_MemAlloc( SIZEOF(DRM_LICSTOREENUM_CONTEXT) ) );
    ChkDR( DRM_LST_InitEnum( pcontextLST,
                             pKid, /* Could be NULL for all licenses */
                             FALSE, /* dump all licenses without care of the priority value */
                             pLicEnum) );

    while( TRUE )
    {
        DRM_DST_SLOT_HINT slotHint;

        cbLicense = 0;
        ChkDR( DRM_LST_EnumNext(pLicEnum, &oKID, &oLID, &slotHint, &cbLicense) );

        ChkMem( pbLicense = (DRM_BYTE *)Oem_MemAlloc( cbLicense + SIZEOF( DRM_WCHAR ) ) );
        ChkDR( DRM_LST_GetLicense(pcontextLST, &oKID, &oLID, &slotHint, pbLicense, &cbLicense) );
        if( pbLicense[0] == 0xff && pbLicense[1] == 0xfe )
        {
            printf( "%S\n", (DRM_WCHAR*)(pbLicense+2) );
        }
        else
        {
            printf( "%S\n", (DRM_WCHAR*)pbLicense );
        }
        Oem_MemFree( pbLicense );
    }

ErrorExit:
    if( pcontextLST != NULL )
    {
        Oem_MemFree( pcontextLST );
    }
    if ( pLicEnum != NULL )
    {
        Oem_MemFree(pLicEnum);
    }
    if ( fOpened )
    {
        DRM_DST_CloseStore(&oDatastore);
        DRM_DST_Uninit(&oDatastore);
    }
    if( pDSTContext != NULL )
    {
        Oem_MemFree( pDSTContext );
    }

    return dr;
}

/******************************************************************************
** Print a command menu to console
*******************************************************************************
*/
static void PrintUsage( const DRM_WCHAR *pwszAppName )
{
    printf("Syntax: %S [-?] [-s:DataStoreName] [-k:KID]\n", pwszAppName);
}

DRM_LONG DRM_CALL DRM_Main( DRM_LONG argc, DRM_WCHAR **argv )
{
    DRM_RESULT       dr            = DRM_SUCCESS;
    DRM_CONST_STRING dstrStoreName = EMPTY_DRM_STRING;
    DRM_CONST_STRING dstrKID       = EMPTY_DRM_STRING;
    DRM_LONG i = 0;

    PrintCopyrightInfo( "DRM License Store Viewing Tool" );

    if ( argc <= 1 )
    {
        dr = DRM_S_MORE_DATA;
        goto _PrintUsage;
    }

    for( i = 1; i<argc; i++ )
    {
        DRM_WCHAR wchOption =  0;
        DRM_CONST_STRING dstrParam = EMPTY_DRM_STRING;

        if ( !DRM_CMD_ParseCmdLine(argv[i], &wchOption, &dstrParam, NULL) )
        {
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }
        wchOption = DRMCRT_towlower( wchOption );
        switch( wchOption )
        {
        case WCHAR_CAST('s'):
            dstrStoreName = dstrParam;
            break;
        case WCHAR_CAST('k'):
            dstrKID = dstrParam;
            break;
        case WCHAR_CAST('?'):
            goto _PrintUsage;
        default:
            dr = DRM_E_INVALID_COMMAND_LINE;
            goto _PrintUsage;
        }
    }

    if( dstrStoreName.cchString == 0 )
    {
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    if( dstrStoreName.cchString > (DRM_MAX_PATH - 1) )
    {
        printf("Data Store name is too long. Max length is %d.\n", (DRM_MAX_PATH - 1) );
        dr = DRM_E_INVALID_COMMAND_LINE;
        goto _PrintUsage;
    }

    ZEROMEM(g_wszDeviceStoreName, SIZEOF(g_wszDeviceStoreName));
    MEMCPY( g_wszDeviceStoreName,
            PB_DSTR( &dstrStoreName ),
            CB_DSTR( &dstrStoreName ) );

    if( dstrKID.cchString > 0 )
    {
        DRM_KID oKid;

        /* Enumerate licenses for a specific KID */
        printf( "Enumerating licenses for single KID.\n");
        if( DRM_FAILED( dr = DRM_UTL_DecodeKID( &dstrKID, &oKid ) ) )
        {
            printf( "--Bad KID\n");
            goto ErrorExit;
        }
        ChkDR( DumpLicenses( &oKid ) );
    }
    else
    {
        /* Enumerate all licenses */
        printf( "Enumerating all licenses in the license store.\n");
        ChkDR( DumpLicenses( NULL ) );
    }

ErrorExit:
    if ( dr == DRM_E_NOMORE )
    {
        printf( "Enumeration complete.\n" );
    }
    /* Do not print error message on DRM_E_NOMORE */
    else if ( DRM_FAILED( dr ) )
    {
        printf( "Error: 0x%X\r\n", dr );
    }
    return dr;

_PrintUsage:
    PrintUsage( argv[0] );
    return dr;
}
